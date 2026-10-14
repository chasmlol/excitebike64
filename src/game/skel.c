#include "game.h"

typedef struct {
    s32 f0;
    s32 f4[9];
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
} Ent_33;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10[9];
    s32 pad[19];
} Out_3;

extern Out_3 sSkelLoadBuf[];

static inline void walk(Ent_33 *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = sSkelWalkIndex;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            sSkelWalkIndex++;
            skel_assign_parents(base, i, base[sSkelWalkIndex - 1].f34);
        } else {
            sSkelWalkIndex++;
        }
        n--;
    }
}

void *skel_load(u32 *param_1, s32 param_2)
{
    Ent_33 *ents;
    Ent_33 *p;
    s32 sum;
    s32 cnt;
    s32 i;
    s32 j;
    void *r;

    ents = res_load(param_1, param_2);
    sum = 0;
    cnt = 0;
    do {
        ents[cnt].f0 = -1;
        sum += ents[cnt].f34;
        sum--;
        cnt++;
    } while (sum >= 0);
    sSkelWalkIndex = 0;
    walk(ents, -1, 1);
    for (i = 0; i < cnt; i++) {
        sSkelLoadBuf[i].f4 = ents[i].f28;
        sSkelLoadBuf[i].f8 = ents[i].f2C;
        sSkelLoadBuf[i].fC = ents[i].f30;
        sSkelLoadBuf[i].f0 = ents[i].f0;
        for (sum = 0; sum < 9; sum++) {
            sSkelLoadBuf[i].f10[sum] = ents[i].f4[sum];
        }
    }
    res_unload((u32 *)(param_1[1] + param_2 * 4));
    r = mem_alloc_dbg(cnt * 0x80, 0, sStrSkelC, 0x74);
    memcpy(r, sSkelLoadBuf, cnt * 0x80);
    return r;
}

struct Node_6 {
    s32 parent;
    s32 pos[3];
    s32 rot[9];
    s32 wpos[3];
    s32 wmtx[16];
};

void skel_update_world_transforms(Node_6 *n, s32 count)
{
    s32 i;
    Node_6 *par;
    s32 v[3];

    n[0].wpos[0] = n[0].pos[0];
    n[0].wpos[1] = n[0].pos[1];
    n[0].wpos[2] = n[0].pos[2];
    for (i = 1; i < count; i++) {
        par = &n[n[i].parent];
        mtx3i_mul(n[i].rot, par->wmtx, n[i].wmtx);
        vec3i_mul_mtx3i(n[i].pos, par->wmtx, n[i].wpos);
        n[i].wpos[0] += par->wpos[0];
        n[i].wpos[1] += par->wpos[1];
        n[i].wpos[2] += par->wpos[2];
    }
}

struct Rec_20 {
    u8 pad0[0x34];
    s32 v34[3];
    s32 m40[16];
};

s32 func_8003C588(Rec_20 *recs, s32 n, s32 *out, s32 *mat, s32 *pos, s32 idx)
{
    s32 rel[3];
    s32 wp[3];
    s32 d, sum;
    s32 i;

    d = pos[0] - gCamPos[idx].x;
    if (d < 0) d = -d;
    sum = d;
    d = pos[1] - gCamPos[idx].y;
    if (d < 0) d = -d;
    sum += d;
    d = pos[2] - gCamPos[idx].z;
    if (d < 0) d = -d;
    sum += d;
    if (sum > 0x8000) return 1;
    for (i = 0; i < n; i++) {
        vec3i_mul_mtx3i(recs[i].v34, mat, wp);
        wp[0] += pos[0];
        wp[1] += pos[1];
        wp[2] += pos[2];
        rel[0] = (gCamPos[idx].x >> 6) - wp[0];
        rel[1] = (gCamPos[idx].y >> 6) - wp[1];
        rel[2] = (gCamPos[idx].z >> 6) - wp[2];
        mtx3i_transpose(recs[i].m40);
        vec3i_mul_mtx3i(rel, recs[i].m40, &out[i * 3]);
        mtx3i_transpose(recs[i].m40);
    }
    return 0;
}

struct Blk_10 { u8 pad[0x34]; s32 x, y, z; u8 pad2[0x40]; };
typedef struct { s16 a, b, c; u16 idx; u8 pad[8]; } Ent_34;
struct List_6 { u32 n; Ent_34 *e; };

void skel_make_verts_bone_local(Blk_10 *tbl, List_6 *list) {
    u32 i;
    for (i = 0; i < list->n; i++) {
        u16 k = list->e[i].idx;
        list->e[i].a -= tbl[k].x;
        list->e[i].b -= tbl[k].y;
        list->e[i].c -= tbl[k].z;
    }
}

void skel_reset_pose(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 *p;

    i = 0;
    if (param_2 > 0) {
        p = param_1;
        do {
            mtx3i_from_angles(p + 4, 0, 0, 0);
            mtx3i_from_angles(p + 0x10, 0, 0, 0);
            i++;
            p += 0x20;
        } while (i < param_2);
    }
    skel_update_world_transforms(param_1, param_2);
}

struct Ent_35 {
    s32 f0;
    s32 pad[12];
    s32 f34;
};

void skel_assign_parents(Ent_35 *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = sSkelWalkIndex;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            sSkelWalkIndex++;
            skel_assign_parents(base, i, base[sSkelWalkIndex - 1].f34);
        } else {
            sSkelWalkIndex++;
        }
        n--;
    }
}
