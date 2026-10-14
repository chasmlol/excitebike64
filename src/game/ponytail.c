#include "game.h"

/* gRiderGfx entries (0xA80 bytes per rider): the ponytail's part. */
typedef struct {
    u8 pad0[0xA48];
    PonytailNode *nodes;        /* 0xA48 */
    s32 padA4C;
    Bone *bones;                /* 0xA50 */
    u8 padA54[0xA80 - 0xA54];
} Rider_7;

extern Rider_7 gRiderGfx_12[];

/* Steps rider idx's ponytail: a spring chain behind the head with gravity and wind from the speed, then the three
   bones along it, with a wobble that grows with speed. */
void ponytail_simulate(s32 idx, RiderSkeleton *skel)
{
    Vec3f vec;
    s32 v[3];
    s32 w[3];
    Mtx3 chestInv;
    s32 seg[3];
    s32 segRot[3];
    s16 yaw;
    s32 horiz;
    s16 pitch;
    s32 dist;
    PonytailNode *nodes;
    PonytailNode *nd;
    Bone *bones;
    s32 baseX, baseY, baseZ;
    s32 i;
    s32 k;
    f32 dx, dy, dz, len, side, gravity;
    s32 rise;
    s32 sn;
    s16 cs;

    nodes = gRiderGfx_12[idx].nodes;
    /* the root node: 88 mm behind the head; the others take 70% of its move */
    vec = nodes->pos;
    v[0] = 0;
    v[1] = 10;
    v[2] = -88;
    vec3i_mul_mtx3i(v, skel->bones[2].worldRot.m, w);
    w[0] += skel->bones[2].worldPos.x;
    w[1] += skel->bones[2].worldPos.y;
    w[2] += skel->bones[2].worldPos.z;
    if (gRiders[idx].crashState == 0) {
        baseX = gRiders[idx].drawPos.x;
        baseY = gRiders[idx].drawPos.y;
        baseZ = gRiders[idx].drawPos.z;
    } else {
        baseX = gRiders[idx].drawBodyPos.x;
        baseY = gRiders[idx].drawBodyPos.y;
        baseZ = gRiders[idx].drawBodyPos.z;
    }
    nodes->pos.x = (f32)(w[0] + baseX) / 1000.0f;
    nodes->pos.y = (f32)(w[1] + baseY) / 1000.0f;
    nodes->pos.z = (f32)(w[2] + baseZ) / 1000.0f;
    dx = nodes->pos.x - vec.x;
    dy = nodes->pos.y - vec.y;
    dz = nodes->pos.z - vec.z;
    for (i = 1; i < 4; i++) {
        nodes[i].pos.x += dx * 0.7f;
        nodes[i].pos.y += dy * 0.7f;
        nodes[i].pos.z += dz * 0.7f;
    }

    /* vec: the direction of travel, scaled by the sideways acceleration (ignored below 2) */
    gravity = 9.8f;
    vec = gRiders[idx].bike.vel;
    {
        f32 l = __builtin_sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
        if (l != 0.0f) {
            vec.x /= l;
            vec.y /= l;
            vec.z /= l;
        }
    }
    side = gRiders[idx].bike.acc.x * vec.z - gRiders[idx].bike.acc.z * vec.x;
    if (side < 2.0f && side > -2.0f) {
        side = 0.0f;
    }
    vec.x *= side;
    vec.y *= side;
    vec.z *= side;
    for (i = 0; i < 4; i++) {
        nodes[i].acc.x = 0.0f;
        nodes[i].acc.y = -gravity * 0.05f;
        nodes[i].acc.z = 0.0f;
        nodes[i].acc.x -= vec.z * 0.1f;
        nodes[i].acc.z += vec.x * 0.1f;
        nodes[i].acc.x -= gRiders[idx].bike.vel.x * 0.05f;
        nodes[i].acc.y -= gRiders[idx].bike.vel.y * 0.05f;
        nodes[i].acc.z -= gRiders[idx].bike.vel.z * 0.05f;
    }

    /* springs to the neighbours, toward their rest lengths */
    for (k = 1; k < 4; k++) {
        if (nodes[k].prev != NULL) {
            dx = nodes[k].prev->pos.x - nodes[k].pos.x;
            dy = nodes[k].prev->pos.y - nodes[k].pos.y;
            dz = nodes[k].prev->pos.z - nodes[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= nodes[k].prevLen;
            nodes[k].acc.x += dx * len;
            nodes[k].acc.y += dy * len;
            nodes[k].acc.z += dz * len;
        }
        if (nodes[k].next != NULL) {
            dx = nodes[k].next->pos.x - nodes[k].pos.x;
            dy = nodes[k].next->pos.y - nodes[k].pos.y;
            dz = nodes[k].next->pos.z - nodes[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= nodes[k].nextLen;
            nodes[k].acc.x += dx * len;
            nodes[k].acc.y += dy * len;
            nodes[k].acc.z += dz * len;
        }
    }

    for (i = 1; i < 4; i++) {
        nodes[i].vel.x += nodes[i].acc.x / 0.05f / 60.0f;
        nodes[i].vel.y += nodes[i].acc.y / 0.05f / 60.0f;
        nodes[i].vel.z += nodes[i].acc.z / 0.05f / 60.0f;
        nodes[i].vel.x *= 0.96f;
        nodes[i].vel.y *= 0.96f;
        nodes[i].vel.z *= 0.96f;
        nodes[i].pos.x += nodes[i].vel.x / 60.0f;
        nodes[i].pos.y += nodes[i].vel.y / 60.0f;
        nodes[i].pos.z += nodes[i].vel.z / 60.0f;
    }

    /* keep each link within 5 cm of its rest length */
    for (i = 0; i < 4; i++) {
        if (nodes[i].next != NULL) {
            dx = nodes[i].next->pos.x - nodes[i].pos.x;
            dy = nodes[i].next->pos.y - nodes[i].pos.y;
            dz = nodes[i].next->pos.z - nodes[i].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len > nodes[i].nextLen + 0.05f) {
                nodes[i].next->pos.x = nodes[i].pos.x + dx / len * (nodes[i].nextLen + 0.05f);
                nodes[i].next->pos.y = nodes[i].pos.y + dy / len * (nodes[i].nextLen + 0.05f);
                nodes[i].next->pos.z = nodes[i].pos.z + dz / len * (nodes[i].nextLen + 0.05f);
            }
            if (len < nodes[i].nextLen - 0.05f) {
                nodes[i].next->pos.x = nodes[i].pos.x + dx / len * (nodes[i].nextLen - 0.05f);
                nodes[i].next->pos.y = nodes[i].pos.y + dy / len * (nodes[i].nextLen - 0.05f);
                nodes[i].next->pos.z = nodes[i].pos.z + dz / len * (nodes[i].nextLen - 0.05f);
            }
        }
    }

    /* keep the nodes at least 100 mm behind the chest, along its z axis */
    chestInv = skel->bones[1].worldRot;
    mtx3i_transpose(chestInv.m);
    for (i = 1; i < 4; i++) {
        v[0] = (s32)(nodes[i].pos.x * 1000.0f) - (baseX + skel->bones[1].worldPos.x);
        v[1] = (s32)(nodes[i].pos.y * 1000.0f) - (baseY + skel->bones[1].worldPos.y);
        v[2] = (s32)(nodes[i].pos.z * 1000.0f) - (baseZ + skel->bones[1].worldPos.z);
        vec3i_mul_mtx3i(v, chestInv.m, w);
        if (w[2] > -100) {
            w[2] = -100;
        }
        vec3i_mul_mtx3i(w, skel->bones[1].worldRot.m, v);
        nodes[i].pos.x = (f32)(v[0] + baseX + skel->bones[1].worldPos.x) / 1000.0f;
        nodes[i].pos.y = (f32)(v[1] + baseY + skel->bones[1].worldPos.y) / 1000.0f;
        nodes[i].pos.z = (f32)(v[2] + baseZ + skel->bones[1].worldPos.z) / 1000.0f;
    }

    /* the bones: each from its node toward the next node (with a wobble that grows with speed), ending at the
       next node's chainPos */
    if (idx) i = 5;
    bones = gRiderGfx_12[idx].bones;
    bones->pos.x = (s32)(nodes->pos.x * 1000.0f) - baseX;
    bones->pos.y = (s32)(nodes->pos.y * 1000.0f) - baseY;
    bones->pos.z = (s32)(nodes->pos.z * 1000.0f) - baseZ;
    nodes->chainPos = nodes->pos;
    for (i = 0; i < 3; i++) {
        bones[i].worldPos.x = nodes[i].chainPos.x * 1000.0f - baseX;
        bones[i].worldPos.y = nodes[i].chainPos.y * 1000.0f - baseY;
        bones[i].worldPos.z = nodes[i].chainPos.z * 1000.0f - baseZ;
        rise = (nodes[i + 1].pos.y - nodes[i].chainPos.y) * 1000.0f;
        math_to_polar((nodes[i + 1].pos.x - nodes[i].chainPos.x) * 1000.0f, (nodes[i + 1].pos.z - nodes[i].chainPos.z) * 1000.0f, &yaw, &horiz);
        math_to_polar(rise, horiz, &pitch, &dist);
        if (i != 0) {
            yaw = yaw + gCosTable[((u32)(gRaceFrame * 4000 + i * 10000) >> 4) & 0xFFF] / 16 * gRiders[idx].speed / 25.0f;
            pitch = pitch + (s16)((s16)gCosTable[(((gRaceFrame * 4400 + i * 12000) >> 4) - 0x400) & 0xFFF] / 12) * gRiders[idx].speed / 25.0f;
        }
        mtx3i_from_angles(bones[i].worldRot.m, pitch, 0, 0x8000 - yaw);
        seg[0] = 0;
        seg[1] = 0;
        seg[2] = sPonytailNodeOffsets[i + 1] * 1000.0f;
        vec3i_mul_mtx3i(seg, bones[i].worldRot.m, segRot);
        nodes[i + 1].chainPos.x = nodes[i].pos.x + (f32)segRot[0] / 1000.0f;
        nodes[i + 1].chainPos.y = nodes[i].pos.y + (f32)segRot[1] / 1000.0f;
        nodes[i + 1].chainPos.z = nodes[i].pos.z + (f32)segRot[2] / 1000.0f;
    }
}

/* gRiderGfx entries, as ponytail_init sees them */
typedef struct Pl_2 {
    u8 pad0[0xA48];
    PonytailNode *nodes;
    s32 *mtx;                   /* the three bone matrices, for each of the two frame buffers */
    Bone *bones;
    s32 *mesh;                  /* sPonytailMesh */
    s32 *a58;
    u8 pad5C[0xA80 - 0xA5C];
} Pl_2;

typedef struct { u16 x, y, z; } V3u;
extern Pl_2 gRiderGfx_13[];
extern V3u sPonytailPoseA[];
extern V3u sPonytailPoseB[];

/* Sets up rider idx's ponytail: its matrices, its three bones and four nodes at rest along z; loads the shared mesh
   and the angles of its two poses on first use. */
void ponytail_init(s32 idx)
{
    Pl_2 *gfx = &gRiderGfx_13[idx];
    s32 i;
    Bone *pose;

    gfx->mtx = mem_alloc_dbg(0x180, 0, sStrPonytailC, 0x16A);
    gfx->a58 = mem_alloc_dbg(0x10, 0, sStrPonytailC, 0x16B);
    i = 0;
    gfx->bones = skel_load(gResourceSet, sPonytailSkelResId);
    skel_reset_pose(gfx->bones, 3);
    gfx->nodes = mem_alloc_dbg(0x120, 0, sStrPonytailC, 0x170);
    mem_fill(gfx->nodes, 0, 0x120);
    for (; i < 4; i++) {
        gfx->nodes[i].pos.x = 0.0f;
        gfx->nodes[i].pos.y = 0.0f;
        gfx->nodes[i].pos.z = sPonytailNodeOffsets[i];
        if (i != 0) {
            gfx->nodes[i].prev = &gfx->nodes[i - 1];
        }
        if (i < 3) {
            gfx->nodes[i].next = &gfx->nodes[i + 1];
        }
    }
    for (i = 0; i < 4; i++) {
        if (gfx->nodes[i].prev != NULL) {
            gfx->nodes[i].prevLen = gfx->nodes[i].prev->pos.z - gfx->nodes[i].pos.z;
        }
        if (gfx->nodes[i].next != NULL) {
            gfx->nodes[i].nextLen = -(gfx->nodes[i].next->pos.z - gfx->nodes[i].pos.z);
        }
    }
    gfx->mesh = sPonytailMesh;
    if (sPonytailMesh == NULL) {
        sPonytailMesh = res_load(gResourceSet, sPonytailMeshResId);
        skel_make_verts_bone_local(gfx->bones, sPonytailMesh);
        pose = skel_load(gResourceSet, sPonytailPoseAResId);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(pose[i].rot.m, &sPonytailPoseA[i].x, &sPonytailPoseA[i].y, &sPonytailPoseA[i].z);
        }
        mem_free(pose);
        pose = skel_load(gResourceSet, sPonytailPoseBResId);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(pose[i].rot.m, &sPonytailPoseB[i].x, &sPonytailPoseB[i].y, &sPonytailPoseB[i].z);
        }
        mem_free(pose);
        gfx->mesh = sPonytailMesh;
    }
}

/* Frees rider idx's ponytail buffers (a58, bones, mtx, nodes) and clears their pointers. */
void ponytail_free(s32 idx)
{
    u8 *gfx;
    u8 *entry;

    gfx = gRiderGfx_14;
    entry = gfx + idx * 0xA80; /* 0xA48 nodes, 0xA4C mtx, 0xA50 bones, 0xA58 a58 (Pl_2) */
    if (*(s32 *)(entry + 0xA58) != 0) {
        mem_free(*(s32 *)(entry + 0xA58));
    }
    if (*(s32 *)(entry + 0xA50) != 0) {
        mem_free(*(s32 *)(entry + 0xA50));
    }
    if (*(s32 *)(entry + 0xA4C) != 0) {
        mem_free(*(s32 *)(entry + 0xA4C));
    }
    if (*(s32 *)(entry + 0xA48) == 0) {
        *(s32 *)(entry + 0xA58) = 0;
    } else {
        mem_free(*(s32 *)(entry + 0xA48));
        *(s32 *)(entry + 0xA58) = 0;
    }
    *(s32 *)(entry + 0xA50) = 0;
    *(s32 *)(entry + 0xA4C) = 0;
    *(s32 *)(entry + 0xA48) = 0;
}

/* Steps rider idx's ponytail and writes its three bone matrices to mtx. */
void ponytail_update(s32 idx, RiderSkeleton *skel, Bone *bones, void *mtx)
{
    ponytail_simulate(idx, skel);
    mtx_from_bones(bones, mtx, 3);
}
