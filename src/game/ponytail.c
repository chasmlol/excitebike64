#include "game.h"

typedef struct {
    u8 pad0[0xA48];
    PonytailNode *nodes;        /* 0xA48 */
    s32 padA4C;
    Bone *skel;         /* 0xA50 */
    u8 padA54[0xA80 - 0xA54];
} Rider_7;

extern Rider_7 gRiderGfx_12[];

void ponytail_simulate(s32 idx, RiderSkeleton *o) {
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
    PonytailNode *n;
    PonytailNode *nd;
    Bone *sk;
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
    vec3i_mul_mtx3i(v, o->bones[2].worldRot.m, w);
    w[0] += o->bones[2].worldPos.x;
    w[1] += o->bones[2].worldPos.y;
    w[2] += o->bones[2].worldPos.z;
    if (gRiders[idx].crashState == 0) {
        bx = gRiders[idx].drawPos.x;
        by = gRiders[idx].drawPos.y;
        bz = gRiders[idx].drawPos.z;
    } else {
        bx = gRiders[idx].drawBodyPos.x;
        by = gRiders[idx].drawBodyPos.y;
        bz = gRiders[idx].drawBodyPos.z;
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
    prev = gRiders[idx].bike.vel;
    {
        f32 l = __builtin_sqrtf(prev.x * prev.x + prev.y * prev.y + prev.z * prev.z);
        if (l != 0.0f) {
            prev.x /= l;
            prev.y /= l;
            prev.z /= l;
        }
    }
    t = gRiders[idx].bike.acc.x * prev.z - gRiders[idx].bike.acc.z * prev.x;
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
        n[i].acc.x -= gRiders[idx].bike.vel.x * 0.05f;
        n[i].acc.y -= gRiders[idx].bike.vel.y * 0.05f;
        n[i].acc.z -= gRiders[idx].bike.vel.z * 0.05f;
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
            len -= n[k].prevLen;
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
            len -= n[k].nextLen;
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
            if (len > n[i].nextLen + 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].nextLen + 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].nextLen + 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].nextLen + 0.05f);
            }
            if (len < n[i].nextLen - 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].nextLen - 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].nextLen - 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].nextLen - 0.05f);
            }
        }
    }

    m = o->bones[1].worldRot;
    mtx3i_transpose(m.m);
    for (i = 1; i < 4; i++) {
        v[0] = (s32)(n[i].pos.x * 1000.0f) - (bx + o->bones[1].worldPos.x);
        v[1] = (s32)(n[i].pos.y * 1000.0f) - (by + o->bones[1].worldPos.y);
        v[2] = (s32)(n[i].pos.z * 1000.0f) - (bz + o->bones[1].worldPos.z);
        vec3i_mul_mtx3i(v, m.m, w);
        if (w[2] > -100) {
            w[2] = -100;
        }
        vec3i_mul_mtx3i(w, o->bones[1].worldRot.m, v);
        n[i].pos.x = (f32)(v[0] + bx + o->bones[1].worldPos.x) / 1000.0f;
        n[i].pos.y = (f32)(v[1] + by + o->bones[1].worldPos.y) / 1000.0f;
        n[i].pos.z = (f32)(v[2] + bz + o->bones[1].worldPos.z) / 1000.0f;
    }

    if (idx) i = 5;
    sk = gRiderGfx_12[idx].skel;
    sk->pos.x = (s32)(n->pos.x * 1000.0f) - bx;
    sk->pos.y = (s32)(n->pos.y * 1000.0f) - by;
    sk->pos.z = (s32)(n->pos.z * 1000.0f) - bz;
    n->chainPos = n->pos;
    for (i = 0; i < 3; i++) {
        sk[i].worldPos.x = n[i].chainPos.x * 1000.0f - bx;
        sk[i].worldPos.y = n[i].chainPos.y * 1000.0f - by;
        sk[i].worldPos.z = n[i].chainPos.z * 1000.0f - bz;
        iy = (n[i + 1].pos.y - n[i].chainPos.y) * 1000.0f;
        math_to_polar((n[i + 1].pos.x - n[i].chainPos.x) * 1000.0f, (n[i + 1].pos.z - n[i].chainPos.z) * 1000.0f, &a1, &r1);
        math_to_polar(iy, r1, &a2, &r2);
        if (i != 0) {
            a1 = a1 + gCosTable[((u32)(gRaceFrame * 4000 + i * 10000) >> 4) & 0xFFF] / 16 * gRiders[idx].speed / 25.0f;
            a2 = a2 + (s16)((s16)gCosTable[(((gRaceFrame * 4400 + i * 12000) >> 4) - 0x400) & 0xFFF] / 12) * gRiders[idx].speed / 25.0f;
        }
        mtx3i_from_angles(sk[i].worldRot.m, a2, 0, 0x8000 - a1);
        c[0] = 0;
        c[1] = 0;
        c[2] = sPonytailNodeOffsets[i + 1] * 1000.0f;
        vec3i_mul_mtx3i(c, sk[i].worldRot.m, d);
        n[i + 1].chainPos.x = n[i].pos.x + (f32)d[0] / 1000.0f;
        n[i + 1].chainPos.y = n[i].pos.y + (f32)d[1] / 1000.0f;
        n[i + 1].chainPos.z = n[i].pos.z + (f32)d[2] / 1000.0f;
    }
}

typedef struct Pl_2 {
    u8 pad0[0xA48];
    PonytailNode *nodes;
    s32 *a4c;
    Bone *a50;
    s32 *a54;
    s32 *a58;
    u8 pad5C[0xA80 - 0xA5C];
} Pl_2;

typedef struct { u16 x, y, z; } V3u;
extern Pl_2 gRiderGfx_13[];
extern V3u sPonytailPoseA[];
extern V3u sPonytailPoseB[];

void ponytail_init(s32 n)
{
    Pl_2 *pl = &gRiderGfx_13[n];
    s32 i;
    Bone *r;

    pl->a4c = mem_alloc_dbg(0x180, 0, sStrPonytailC, 0x16A);
    pl->a58 = mem_alloc_dbg(0x10, 0, sStrPonytailC, 0x16B);
    i = 0;
    pl->a50 = skel_load(gResourceSet, sPonytailSkelResId);
    skel_reset_pose(pl->a50, 3);
    pl->nodes = mem_alloc_dbg(0x120, 0, sStrPonytailC, 0x170);
    mem_fill(pl->nodes, 0, 0x120);
    for (; i < 4; i++) {
        pl->nodes[i].pos.x = 0.0f;
        pl->nodes[i].pos.y = 0.0f;
        pl->nodes[i].pos.z = sPonytailNodeOffsets[i];
        if (i != 0) {
            pl->nodes[i].prev = &pl->nodes[i - 1];
        }
        if (i < 3) {
            pl->nodes[i].next = &pl->nodes[i + 1];
        }
    }
    for (i = 0; i < 4; i++) {
        if (pl->nodes[i].prev != NULL) {
            pl->nodes[i].prevLen = pl->nodes[i].prev->pos.z - pl->nodes[i].pos.z;
        }
        if (pl->nodes[i].next != NULL) {
            pl->nodes[i].nextLen = -(pl->nodes[i].next->pos.z - pl->nodes[i].pos.z);
        }
    }
    pl->a54 = sPonytailMesh;
    if (sPonytailMesh == NULL) {
        sPonytailMesh = res_load(gResourceSet, sPonytailMeshResId);
        skel_make_verts_bone_local(pl->a50, sPonytailMesh);
        r = skel_load(gResourceSet, sPonytailPoseAResId);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(r[i].rot.m, &sPonytailPoseA[i].x, &sPonytailPoseA[i].y, &sPonytailPoseA[i].z);
        }
        mem_free(r);
        r = skel_load(gResourceSet, sPonytailPoseBResId);
        for (i = 0; i < 3; i++) {
            mtx3i_to_angles(r[i].rot.m, &sPonytailPoseB[i].x, &sPonytailPoseB[i].y, &sPonytailPoseB[i].z);
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

void ponytail_update(s32 a0, RiderSkeleton *a1, Bone *a2, void *a3) {
    ponytail_simulate(a0, a1);
    mtx_from_bones(a2, a3, 3);
}
