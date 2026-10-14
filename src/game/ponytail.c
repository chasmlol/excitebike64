#include "game.h"

typedef struct Node_22 {
    Vec3f pos;          /* 0x00 */
    Vec3f ppos;         /* 0x0C */
    Vec3f vel;          /* 0x18 */
    Vec3f acc;          /* 0x24 */
    s32 pad30;
    s32 pad34;
    struct Node_22 *prev;  /* 0x38 */
    struct Node_22 *next;  /* 0x3C */
    f32 len40;
    f32 len44;
} Node_22;

typedef struct {
    s32 f0;
    s32 pos[3];         /* 0x04 */
    u8 pad10[0x24];
    s32 p[3];           /* 0x34 */
    s32 m[9];           /* 0x40 */
    u8 pad64[0x80 - 0x64];
} Skel_3;

typedef struct {
    u8 pad0[0xA48];
    Node_22 *nodes;        /* 0xA48 */
    s32 padA4C;
    Skel_3 *skel;         /* 0xA50 */
    u8 padA54[0xA80 - 0xA54];
} Rider_7;

typedef struct {
    s32 pos[3];         /* 0x00 */
    s32 alt[3];         /* 0x0C */
    u8 pad18[0x44 - 0x18];
    u8 f44;
    u8 pad45[0x54 - 0x45];
    Vec3f vel;          /* 0x54 */
    f32 f60;
    f32 f64;
    f32 f68;
    u8 pad6C[0x420 - 0x6C];
    f32 f420;
    u8 pad424[0x7E0 - 0x424];
} Player_26;

struct Obj_60 {
    u8 pad0[0xB4];
    s32 pos[3];         /* 0xB4 */
    Mtx3 mC0;           /* 0xC0 */
    u8 padE4[0x134 - 0xE4];
    s32 off[3];         /* 0x134 */
    Mtx3 m140;          /* 0x140 */
};

extern Rider_7 gRiderGfx_12[];
extern Player_26 gRiders_90[];

void ponytail_simulate(s32 idx, Obj_60 *o) {
    Vec3f prev;
    s32 v[3];
    s32 w[3];
    Mtx3 m;
    s32 c[3];
    s32 d[3];
    s16 a1;
    s32 r1;
    s16 a2;
    s32 r2;
    Node_22 *n;
    Node_22 *nd;
    Skel_3 *sk;
    s32 bx, by, bz;
    s32 i;
    s32 k;
    f32 dx, dy, dz, len, t, g;
    s32 iy;
    s32 sn;
    s16 cs;

    n = gRiderGfx_12[idx].nodes;
    prev = n->pos;
    v[0] = 0;
    v[1] = 10;
    v[2] = -88;
    vec3i_mul_mtx3i(v, o->m140.m, w);
    w[0] += o->off[0];
    w[1] += o->off[1];
    w[2] += o->off[2];
    if (gRiders_90[idx].f44 == 0) {
        bx = gRiders_90[idx].pos[0];
        by = gRiders_90[idx].pos[1];
        bz = gRiders_90[idx].pos[2];
    } else {
        bx = gRiders_90[idx].alt[0];
        by = gRiders_90[idx].alt[1];
        bz = gRiders_90[idx].alt[2];
    }
    n->pos.x = (f32)(w[0] + bx) / 1000.0f;
    n->pos.y = (f32)(w[1] + by) / 1000.0f;
    n->pos.z = (f32)(w[2] + bz) / 1000.0f;
    dx = n->pos.x - prev.x;
    dy = n->pos.y - prev.y;
    dz = n->pos.z - prev.z;
    for (i = 1; i < 4; i++) {
        n[i].pos.x += dx * 0.7f;
        n[i].pos.y += dy * 0.7f;
        n[i].pos.z += dz * 0.7f;
    }

    g = 9.8f;
    prev = gRiders_90[idx].vel;
    {
        f32 l = __builtin_sqrtf(prev.x * prev.x + prev.y * prev.y + prev.z * prev.z);
        if (l != 0.0f) {
            prev.x /= l;
            prev.y /= l;
            prev.z /= l;
        }
    }
    t = gRiders_90[idx].f60 * prev.z - gRiders_90[idx].f68 * prev.x;
    if (t < 2.0f && t > -2.0f) {
        t = 0.0f;
    }
    prev.x *= t;
    prev.y *= t;
    prev.z *= t;
    for (i = 0; i < 4; i++) {
        n[i].acc.x = 0.0f;
        n[i].acc.y = -g * 0.05f;
        n[i].acc.z = 0.0f;
        n[i].acc.x -= prev.z * 0.1f;
        n[i].acc.z += prev.x * 0.1f;
        n[i].acc.x -= gRiders_90[idx].vel.x * 0.05f;
        n[i].acc.y -= gRiders_90[idx].vel.y * 0.05f;
        n[i].acc.z -= gRiders_90[idx].vel.z * 0.05f;
    }

    for (k = 1; k < 4; k++) {
        if (n[k].prev != NULL) {
            dx = n[k].prev->pos.x - n[k].pos.x;
            dy = n[k].prev->pos.y - n[k].pos.y;
            dz = n[k].prev->pos.z - n[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= n[k].len40;
            n[k].acc.x += dx * len;
            n[k].acc.y += dy * len;
            n[k].acc.z += dz * len;
        }
        if (n[k].next != NULL) {
            dx = n[k].next->pos.x - n[k].pos.x;
            dy = n[k].next->pos.y - n[k].pos.y;
            dz = n[k].next->pos.z - n[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= n[k].len44;
            n[k].acc.x += dx * len;
            n[k].acc.y += dy * len;
            n[k].acc.z += dz * len;
        }
    }

    for (i = 1; i < 4; i++) {
        n[i].vel.x += n[i].acc.x / 0.05f / 60.0f;
        n[i].vel.y += n[i].acc.y / 0.05f / 60.0f;
        n[i].vel.z += n[i].acc.z / 0.05f / 60.0f;
        n[i].vel.x *= 0.96f;
        n[i].vel.y *= 0.96f;
        n[i].vel.z *= 0.96f;
        n[i].pos.x += n[i].vel.x / 60.0f;
        n[i].pos.y += n[i].vel.y / 60.0f;
        n[i].pos.z += n[i].vel.z / 60.0f;
    }

    for (i = 0; i < 4; i++) {
        if (n[i].next != NULL) {
            dx = n[i].next->pos.x - n[i].pos.x;
            dy = n[i].next->pos.y - n[i].pos.y;
            dz = n[i].next->pos.z - n[i].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len > n[i].len44 + 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].len44 + 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].len44 + 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].len44 + 0.05f);
            }
            if (len < n[i].len44 - 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].len44 - 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].len44 - 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].len44 - 0.05f);
            }
        }
    }

    m = o->mC0;
    mtx3i_transpose(m.m);
    for (i = 1; i < 4; i++) {
        v[0] = (s32)(n[i].pos.x * 1000.0f) - (bx + o->pos[0]);
        v[1] = (s32)(n[i].pos.y * 1000.0f) - (by + o->pos[1]);
        v[2] = (s32)(n[i].pos.z * 1000.0f) - (bz + o->pos[2]);
        vec3i_mul_mtx3i(v, m.m, w);
        if (w[2] > -100) {
            w[2] = -100;
        }
        vec3i_mul_mtx3i(w, o->mC0.m, v);
        n[i].pos.x = (f32)(v[0] + bx + o->pos[0]) / 1000.0f;
        n[i].pos.y = (f32)(v[1] + by + o->pos[1]) / 1000.0f;
        n[i].pos.z = (f32)(v[2] + bz + o->pos[2]) / 1000.0f;
    }

    if (idx) i = 5;
    sk = gRiderGfx_12[idx].skel;
    sk->pos[0] = (s32)(n->pos.x * 1000.0f) - bx;
    sk->pos[1] = (s32)(n->pos.y * 1000.0f) - by;
    sk->pos[2] = (s32)(n->pos.z * 1000.0f) - bz;
    n->ppos = n->pos;
    for (i = 0; i < 3; i++) {
        sk[i].p[0] = n[i].ppos.x * 1000.0f - bx;
        sk[i].p[1] = n[i].ppos.y * 1000.0f - by;
        sk[i].p[2] = n[i].ppos.z * 1000.0f - bz;
        iy = (n[i + 1].pos.y - n[i].ppos.y) * 1000.0f;
        math_to_polar((n[i + 1].pos.x - n[i].ppos.x) * 1000.0f, (n[i + 1].pos.z - n[i].ppos.z) * 1000.0f, &a1, &r1);
        math_to_polar(iy, r1, &a2, &r2);
        if (i != 0) {
            a1 = a1 + gCosTable[((u32)(gRaceFrame * 4000 + i * 10000) >> 4) & 0xFFF] / 16 * gRiders_90[idx].f420 / 25.0f;
            a2 = a2 + (s16)((s16)gCosTable[(((gRaceFrame * 4400 + i * 12000) >> 4) - 0x400) & 0xFFF] / 12) * gRiders_90[idx].f420 / 25.0f;
        }
        mtx3i_from_angles(sk[i].m, a2, 0, 0x8000 - a1);
        c[0] = 0;
        c[1] = 0;
        c[2] = sPonytailNodeOffsets[i + 1] * 1000.0f;
        vec3i_mul_mtx3i(c, sk[i].m, d);
        n[i + 1].ppos.x = n[i].pos.x + (f32)d[0] / 1000.0f;
        n[i + 1].ppos.y = n[i].pos.y + (f32)d[1] / 1000.0f;
        n[i + 1].ppos.z = n[i].pos.z + (f32)d[2] / 1000.0f;
    }
}

typedef struct Node_23 {
    s32 f0;
    s32 f4;
    f32 f8;
    u8 pad0C[0x38 - 0xC];
    struct Node_23 *prev;
    struct Node_23 *next;
    f32 f40;
    f32 f44;
} Node_23;

typedef struct Pl_2 {
    u8 pad0[0xA48];
    Node_23 *nodes;
    s32 *a4c;
    s32 *a50;
    s32 *a54;
    s32 *a58;
    u8 pad5C[0xA80 - 0xA5C];
} Pl_2;

typedef struct { u16 x, y, z; } V3u;
typedef struct { u8 pad0[0x10]; s32 m[0x1C]; } Obj80;

extern Pl_2 gRiderGfx_13[];
extern V3u D_800F2178[];
extern V3u D_800F2160[];

void ponytail_init(s32 n)
{
    Pl_2 *pl = &gRiderGfx_13[n];
    s32 i;
    Obj80 *r;

    pl->a4c = mem_alloc_dbg(0x180, 0, sStrPonytailC, 0x16A);
    pl->a58 = mem_alloc_dbg(0x10, 0, sStrPonytailC, 0x16B);
    i = 0;
    pl->a50 = skel_load(D_800E7728, D_800D0D88);
    skel_reset_pose(pl->a50, 3);
    pl->nodes = mem_alloc_dbg(0x120, 0, sStrPonytailC, 0x170);
    mem_fill(pl->nodes, 0, 0x120);
    for (; i < 4; i++) {
        pl->nodes[i].f0 = 0;
        pl->nodes[i].f4 = 0;
        pl->nodes[i].f8 = sPonytailNodeOffsets[i];
        if (i != 0) {
            pl->nodes[i].prev = &pl->nodes[i - 1];
        }
        if (i < 3) {
            pl->nodes[i].next = &pl->nodes[i + 1];
        }
    }
    for (i = 0; i < 4; i++) {
        if (pl->nodes[i].prev != NULL) {
            pl->nodes[i].f40 = pl->nodes[i].prev->f8 - pl->nodes[i].f8;
        }
        if (pl->nodes[i].next != NULL) {
            pl->nodes[i].f44 = -(pl->nodes[i].next->f8 - pl->nodes[i].f8);
        }
    }
    pl->a54 = sPonytailMesh;
    if (sPonytailMesh == NULL) {
        sPonytailMesh = res_load(D_800E7728, D_800D0A68);
        skel_make_verts_bone_local(pl->a50, sPonytailMesh);
        r = skel_load(D_800E7728, D_800D0D44);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(r[i].m, &D_800F2178[i].x, &D_800F2178[i].y, &D_800F2178[i].z);
        }
        mem_free(r);
        r = skel_load(D_800E7728, D_800D0C18);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(r[i].m, &D_800F2160[i].x, &D_800F2160[i].y, &D_800F2160[i].z);
        }
        mem_free(r);
        pl->a54 = sPonytailMesh;
    }
}

void ponytail_free(s32 a0)
{
    u8 *base;
    u8 *p;

    base = gRiderGfx_14;
    p = base + a0 * 0xA80;
    if (*(s32 *)(p + 0xA58) != 0) {
        mem_free(*(s32 *)(p + 0xA58));
    }
    if (*(s32 *)(p + 0xA50) != 0) {
        mem_free(*(s32 *)(p + 0xA50));
    }
    if (*(s32 *)(p + 0xA4C) != 0) {
        mem_free(*(s32 *)(p + 0xA4C));
    }
    if (*(s32 *)(p + 0xA48) == 0) {
        *(s32 *)(p + 0xA58) = 0;
    } else {
        mem_free(*(s32 *)(p + 0xA48));
        *(s32 *)(p + 0xA58) = 0;
    }
    *(s32 *)(p + 0xA50) = 0;
    *(s32 *)(p + 0xA4C) = 0;
    *(s32 *)(p + 0xA48) = 0;
}

void ponytail_update(s32 a0, s32 a1, s32 a2, void *a3) {
    ponytail_simulate(a0, a1);
    mtx_from_bones(a2, a3, 3);
}
