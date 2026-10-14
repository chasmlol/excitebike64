#include "game.h"

/* An entry of a skeleton resource: the bones in preorder, each with its child count. */
typedef struct {
    s32 parent;                 /* filled in by skel_load */
    s32 rot[9];                 /* 1.0 = 0x4000 */
    s32 x;
    s32 y;
    s32 z;
    s32 childCount;
} Ent_33; /* size 0x38 */

extern Bone sSkelLoadBuf[];

/* skel_assign_parents, inlined for the root: sets the parent of the next count entries and recurses into their
   children. */
static inline void walk(Ent_33 *entries, s32 parent, s32 count)
{
    s32 cur;

    while (count != 0) {
        cur = sSkelWalkIndex;
        entries[cur].parent = parent;
        if (entries[cur].childCount != 0) {
            sSkelWalkIndex++;
            skel_assign_parents(entries, cur, entries[sSkelWalkIndex - 1].childCount);
        } else {
            sSkelWalkIndex++;
        }
        count--;
    }
}

/* Loads skeleton resource id of set and returns its bones in a new allocation: parents from the child counts,
   local positions and rotations. */
void *skel_load(u32 *set, s32 id)
{
    Ent_33 *entries;
    Ent_33 *p;
    s32 pending;
    s32 count;
    s32 i;
    s32 j;
    void *bones;

    entries = res_load(set, id);
    /* count the entries of the tree: pending is the number still to come, minus one */
    pending = 0;
    count = 0;
    do {
        entries[count].parent = -1;
        pending += entries[count].childCount;
        pending--;
        count++;
    } while (pending >= 0);
    sSkelWalkIndex = 0;
    walk(entries, -1, 1);
    for (i = 0; i < count; i++) {
        sSkelLoadBuf[i].pos.x = entries[i].x;
        sSkelLoadBuf[i].pos.y = entries[i].y;
        sSkelLoadBuf[i].pos.z = entries[i].z;
        sSkelLoadBuf[i].parent = entries[i].parent;
        for (pending = 0; pending < 9; pending++) {
            sSkelLoadBuf[i].rot.m[pending] = entries[i].rot[pending];
        }
    }
    res_unload((u32 *)(set[1] + id * 4)); /* the set's handle of the resource */
    bones = mem_alloc_dbg(count * 0x80, 0, sStrSkelC, 0x74);
    memcpy(bones, sSkelLoadBuf, count * 0x80);
    return bones;
}

/* Computes the world position and rotation of each bone from its parent's (the parents come first). */
void skel_update_world_transforms(Bone *bones, s32 count)
{
    s32 i;
    Bone *parent;
    s32 v[3];

    bones[0].worldPos.x = bones[0].pos.x;
    bones[0].worldPos.y = bones[0].pos.y;
    bones[0].worldPos.z = bones[0].pos.z;
    for (i = 1; i < count; i++) {
        parent = &bones[bones[i].parent];
        mtx3i_mul(bones[i].rot.m, parent->worldRot.m, bones[i].worldRot.m);
        vec3i_mul_mtx3i(&bones[i].pos.x, parent->worldRot.m, &bones[i].worldPos.x);
        bones[i].worldPos.x += parent->worldPos.x;
        bones[i].worldPos.y += parent->worldPos.y;
        bones[i].worldPos.z += parent->worldPos.z;
    }
}

/* Writes the position of view's camera in the frame of each bone of a skeleton placed at pos with rotation rot
   (3 per bone in out). Returns 1, doing nothing, when the camera is more than 0x8000 away (x + y + z). */
s32 skel_camera_to_bone_space(Bone *bones, s32 count, s32 *out, s32 *rot, s32 *pos, s32 view)
{
    s32 rel[3];
    s32 world[3];
    s32 d, dist;
    s32 i;

    d = pos[0] - gCamPos[view].x;
    if (d < 0) d = -d;
    dist = d;
    d = pos[1] - gCamPos[view].y;
    if (d < 0) d = -d;
    dist += d;
    d = pos[2] - gCamPos[view].z;
    if (d < 0) d = -d;
    dist += d;
    if (dist > 0x8000) return 1;
    for (i = 0; i < count; i++) {
        vec3i_mul_mtx3i(&bones[i].worldPos.x, rot, world);
        world[0] += pos[0];
        world[1] += pos[1];
        world[2] += pos[2];
        rel[0] = (gCamPos[view].x >> 6) - world[0];
        rel[1] = (gCamPos[view].y >> 6) - world[1];
        rel[2] = (gCamPos[view].z >> 6) - world[2];
        mtx3i_transpose(bones[i].worldRot.m);
        vec3i_mul_mtx3i(rel, bones[i].worldRot.m, &out[i * 3]);
        mtx3i_transpose(bones[i].worldRot.m);
    }
    return 0;
}

/* A model vertex: position and the index of the bone it follows. */
typedef struct { s16 x, y, z; u16 bone; u8 pad[8]; } Ent_34;
struct List_6 { u32 count; Ent_34 *verts; };

/* Makes the vertices of a model relative to the world position of their bone. */
void skel_make_verts_bone_local(Bone *bones, List_6 *list)
{
    u32 i;

    for (i = 0; i < list->count; i++) {
        u16 bone = list->verts[i].bone;
        list->verts[i].x -= bones[bone].worldPos.x;
        list->verts[i].y -= bones[bone].worldPos.y;
        list->verts[i].z -= bones[bone].worldPos.z;
    }
}

/* Clears the rotations of a skeleton and recomputes its world transforms. */
void skel_reset_pose(Bone *bones, s32 count)
{
    s32 i;
    Bone *bone;

    i = 0;
    if (count > 0) {
        bone = bones;
        do {
            mtx3i_from_angles(bone->rot.m, 0, 0, 0);
            mtx3i_from_angles(bone->worldRot.m, 0, 0, 0);
            i++;
            bone++;
        } while (i < count);
    }
    skel_update_world_transforms(bones, count);
}

/* Ent_33 as skel_assign_parents sees it. */
struct Ent_35 {
    s32 parent;
    s32 pad[12];
    s32 childCount;
};

/* Sets the parent of the next count entries, from sSkelWalkIndex on, and recurses into the children of each. */
void skel_assign_parents(Ent_35 *entries, s32 parent, s32 count)
{
    s32 cur;

    while (count != 0) {
        cur = sSkelWalkIndex;
        entries[cur].parent = parent;
        if (entries[cur].childCount != 0) {
            sSkelWalkIndex++;
            skel_assign_parents(entries, cur, entries[sSkelWalkIndex - 1].childCount);
        } else {
            sSkelWalkIndex++;
        }
        count--;
    }
}
