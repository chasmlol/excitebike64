#include "game.h"

typedef struct { s32 w[9]; } M9;

struct Joint {
    u8 pad0[8];
    s32 len;
    s32 len2;
    Mtx3 rot;
    Vec3i pos;
    Mtx3 wrot;
    u8 pad64[0x1C];
};

struct Obj_30 {
    u8 pad0[0x18];
    Vec3i v;
};

void rider_ik_arm(Joint *j, s32 idx, Vec3i *tgt, Obj_30 *o)
{
    Mtx3 m;
    Mtx3 m2;
    Vec3i t;
    Vec3i d;
    s16 a1;
    s32 w;
    s16 a2, a3, a4;
    s32 rot, dist, n, l1, l2, x, rad;

    t.x = -(o->v.x * 80) / 16384 + tgt->x;
    t.y = -(o->v.y * 80) / 16384 + tgt->y;
    t.z = -(o->v.z * 80) / 16384 + tgt->z;
    d.x = t.x - j[idx].pos.x;
    d.y = t.y - j[idx].pos.y;
    d.z = t.z - j[idx].pos.z;
    m = j[1].wrot;
    mtx3i_transpose(&m);
    vec3i_mul_mtx3i(&d, &m, &t);
    if (idx == 3) {
        rot = -0x2000;
    } else {
        rot = 0x2000;
    }
    rot = rot * D_800EA774 / 256;
    mtx3i_from_angles(&m, 0, rot, 0);
    vec3i_mul_mtx3i(&t, &m, &d);
    dist = isqrt_u32_3(d.x * d.x + d.y * d.y + d.z * d.z);
    n = j[idx + 1].len2 + j[idx + 2].len2;
    if (n < dist) {
        d.x = d.x * n / dist;
        d.y = d.y * n / dist;
        d.z = d.z * n / dist;
        dist = n;
    }
    l1 = j[idx + 1].len2;
    l2 = j[idx + 2].len2;
    x = (l1 + l2) * (l1 - l2);
    x = x / (dist * 2) + dist / 2;
    {
        s32 sq = x * x;
        rad = isqrt_u32_3(l1 * l1 - sq);
    }
    math_to_polar(rad, x, &a1, &w);
    math_to_polar(rad, dist - x, &a2, &w);
    if (idx == 6) {
        a1 = -a1;
        a2 = -a2;
    }
    math_to_polar(d.x, d.z, &a3, &w);
    a3 = -a3;
    w = isqrt_u32_3(d.x * d.x + d.z * d.z);
    math_to_polar(d.y, w, &a4, &w);
    a4 = -a4;
    mtx3i_from_angles(&m, a4, 0, a3 + a1);
    mtx3i_from_angles(&m2, 0, -rot, 0);
    mtx3i_mul(&m, &m2, &j[idx].rot);
    mtx3i_from_angles(&j[idx + 1].rot, 0, 0, -(a1 + a2));
}

struct Joint_2 {
    u8 pad0[8];
    s32 len;
    s32 pad0C;
    Mtx3 rot;
    Vec3i pos;
    Mtx3 wrot;
    u8 pad64[0x1C];
};

void rider_ik_leg(Joint_2 *j, s32 idx, Vec3i *tgt, s32 *arg3)
{
    Mtx3 m;
    Mtx3 m2;
    Vec3i t;
    Vec3i d;
    s16 a1;
    s32 w;
    s16 a2, a3, a4;
    s32 rot, dist, n, l1, n1, n2, x, rad;

    t.x = tgt->x;
    t.y = tgt->y;
    t.z = tgt->z;
    d.x = t.x - j[idx].pos.x;
    d.y = t.y - j[idx].pos.y;
    d.z = t.z - j[idx].pos.z;
    m = j->wrot;
    mtx3i_transpose(&m);
    vec3i_mul_mtx3i(&d, &m, &t);
    if (idx == 9) {
        rot = -0x800;
    } else {
        rot = 0x800;
    }
    mtx3i_from_angles(&m, 0, 0, rot);
    vec3i_mul_mtx3i(&t, &m, &d);
    dist = isqrt_u32_3(d.x * d.x + d.y * d.y + d.z * d.z);
    n = -(j[idx + 1].len + j[idx + 2].len);
    if (n < dist) {
        d.x = d.x * n / dist;
        d.y = d.y * n / dist;
        d.z = d.z * n / dist;
        dist = n;
    }
    l1 = j[idx + 1].len;
    n1 = -l1;
    n2 = -j[idx + 2].len;
    x = (n1 + n2) * (n1 - n2);
    x = x / (dist * 2) + dist / 2;
    {
        s32 sq = x * x;
        rad = isqrt_u32_3(l1 * l1 - sq);
    }
    math_to_polar(rad, x, &a1, &w);
    math_to_polar(rad, dist - x, &a2, &w);
    math_to_polar(d.x, d.y, &a3, &w);
    a3 = -(a3 ^ 0x8000);
    w = isqrt_u32_3(d.x * d.x + d.y * d.y);
    math_to_polar(d.z, w, &a4, &w);
    a4 = -a4;
    mtx3i_from_angles(&m, a4 - a1, 0, 0);
    mtx3i_from_angles(&m2, 0, a3, -rot);
    mtx3i_mul(&m, &m2, &j[idx].rot);
    mtx3i_from_angles(&j[idx + 1].rot, a1 + a2, 0, 0);
}

void rider_apply_ik(s32 *param_1)
{
    s32 *p;
    s32 *q;
    s32 *o;
    volatile s32 pad[10];

    p = param_1 + 0x10;
    q = param_1 + 0x30;
    mtx3i_mul(param_1 + 0x24, p, q);
    vec3i_mul_mtx3i(param_1 + 0x21, p, param_1 + 0x2d);
    param_1[0x2d] = param_1[0x2d] + param_1[0xd];
    param_1[0x2e] = param_1[0x2e] + param_1[0xe];
    param_1[0x2f] = param_1[0x2f] + param_1[0xf];
    vec3i_mul_mtx3i(param_1 + 0x61, q, param_1 + 0x6d);
    param_1[0x6d] = param_1[0x6d] + param_1[0x2d];
    param_1[0x6e] = param_1[0x6e] + param_1[0x2e];
    param_1[0x6f] = param_1[0x6f] + param_1[0x2f];
    vec3i_mul_mtx3i(param_1 + 0xc1, q, param_1 + 0xcd);
    param_1[0xcd] = param_1[0xcd] + param_1[0x2d];
    param_1[0xce] = param_1[0xce] + param_1[0x2e];
    param_1[0xcf] = param_1[0xcf] + param_1[0x2f];
    vec3i_mul_mtx3i(param_1 + 0x121, p, param_1 + 0x12d);
    param_1[0x12d] = param_1[0x12d] + param_1[0xd];
    param_1[0x12e] = param_1[0x12e] + param_1[0xe];
    param_1[0x12f] = param_1[0x12f] + param_1[0xf];
    vec3i_mul_mtx3i(param_1 + 0x181, p, param_1 + 0x18d);
    param_1[0x18d] = param_1[0x18d] + param_1[0xd];
    param_1[0x18e] = param_1[0x18e] + param_1[0xe];
    param_1[0x18f] = param_1[0x18f] + param_1[0xf];

    rider_ik_arm(param_1, 3, sRiderIkTargets, sRiderIkTargets + 3);
    rider_ik_arm(param_1, 6, sRiderIkTargets + 12, sRiderIkTargets + 15);
    rider_ik_leg(param_1, 9, sRiderIkTargets + 24, sRiderIkTargets + 27);
    rider_ik_leg(param_1, 12, sRiderIkTargets + 36, sRiderIkTargets + 39);
    skel_update_world_transforms(param_1, 15);

    *(M9 *)(param_1 + 0xb0) = *(M9 *)(sRiderIkTargets + 3);
    *(M9 *)(param_1 + 0x110) = *(M9 *)D_800EA6CC;
    *(M9 *)(param_1 + 0x170) = *(M9 *)D_800EA6FC;
    *(M9 *)(param_1 + 0x1d0) = *(M9 *)D_800EA72C;
    o = param_1 + 0x50;
    *(M9 *)o = *(M9 *)D_800EA750;
}

#define B (gRiders[i])
#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]
#define COSS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

typedef struct { Vec3i pos; Mtx3 rot; } Joint_3;

typedef struct {
    Joint_3 j[4];   /* 0x00 */
    Mtx3 rot;    /* 0xC0 */
    s32 fE4;      /* 0xE4 */
} Cam_3;

typedef struct {
    Vec3i p;
    u8 pad[0x74];
} Node80;

typedef struct {
    u8 pad[0x234];
    Node80 n[6];
} Skel_2;

typedef struct {
    s32 f0;
    u8 pad4[0x8];
    struct { u8 pad[4]; s32 f4; } *fC;
    u8 pad10[0x2C];
} Ent_36;

typedef struct {
    u8 pad[0xA4C];
    u8 *fA4C;
    s32 fA50;
    u8 padA54[0x2C];
} Slot_7;

extern Ent_36 gRiderAnims_2[];
extern Cam_3 sRiderIkTargets_2;
extern Skel_2 *gBikeSkelTemplate_4;
extern Slot_7 gRiderGfx_10[];

void rider_update_pose(Model_4 *m, s32 i) {
    s32 v[3];
    Mtx3 mtx;
    s32 out[3];
    s32 scale;
    s32 a, c, d, w, u, s3, n;
    s32 st;
    s32 fl;
    s32 t;
    s32 r, h;

    if (gPodiumMode != 0) {
        return;
    }
    st = B.anim;
    scale = 0x100;
    if (st >= 0) {
        fl = gRiderAnims_2[st].f0;
        if (fl & 1) {
            if (B.animFrame < 0x400) scale = B.animFrame / 4;
        } else if (fl & 2) {
            fl = gRiderAnims_2[st].fC->f4;
            if (B.animFrame >= (fl - 4) << 8) {
                scale = ((fl << 8) - B.animFrame) / 4;
            } else if (B.animFrame < 0x400) {
                scale = B.animFrame / 4;
            }
        } else if (fl & 4) {
            if (B.animFrame < 0x400 && B.animHold == 0) scale = B.animFrame / 4;
        } else {
            if (B.animFrame < 0x400) scale = B.animFrame / 4;
        }
    }
    if (B.prevAnim >= 25) {
        scale = 0x100;
    }
    if (B.prevAnim != -1 && scale != 0x100) {
        st = B.anim;
        t = B.animFrame;
        B.anim = B.prevAnim;
        B.animFrame = B.prevAnimFrame;
        rider_anim_apply(m, i, 0x100);
        rider_pose_snapshot(m, 0xF);
        B.anim = st;
        B.animFrame = t;
    } else if (B.anim == -1 || scale != 0x100) {
        v[0] = 0;
        a = (B.drawRot[1] * 250) / 8192;
        if (a > 0) a = -a;
        if (a < -500) a = -500;
        c = B.unk_410 * 10;
        if (c > 250) c = 250;
        s3 = B.inHistAvg;
        s3 -= B.drawPos.y;
        v[1] = a + 900 + c + s3;
        if (v[1] < 800) v[1] = 800;
        if (v[1] > 0x41A) v[1] = 0x41A;
        d = v[1] - 900;
        if (d < 0) d = 0;
        s3 = B.stickHist8Avg;
        v[2] = s3 * 6 - 250;
        d *= 10;
        if (B.unk_77C != 0) {
            w = B.unk_77C;
            if (w > 256) w = 256;
            if (w < 0) w = 0;
            u = 256 - w;
            d = (d * u + w * 6000) >> 8;
            v[2] = (v[2] * u - w * 520) >> 8;
            v[1] = (v[1] * u + w * 900) >> 8;
        }
        sRiderIkTargets_2.fE4 = 0x80 - s3 * 16;
        if (sRiderIkTargets_2.fE4 < 0) sRiderIkTargets_2.fE4 = 0;
        mtx3i_from_angles(&mtx, B.drawRot[0], (B.drawRot[1] * 7) / 8, B.drawRot[2]);
        vec3i_mul_mtx3i(v, mtx.m, out);
        m->pos.x = m->pos2.x = out[0];
        m->pos.y = m->pos2.y = out[1];
        m->pos.z = m->pos2.z = out[2];
        mtx3i_from_angles(&m->rot, B.drawRot[0] + 4000, 0, B.drawRot[2]);
        m->rot2 = m->rot;
        r = s3 * 128 - 4000;
        mtx3i_from_angles(&m->rot3, D_800F2140[i] = d - r, 0, 0);
        sRiderIkTargets_2.j[0].pos = gBikeSkelTemplate_4->n[0].p;
        sRiderIkTargets_2.j[1].pos = gBikeSkelTemplate_4->n[1].p;
        sRiderIkTargets_2.j[2].pos = gBikeSkelTemplate_4->n[2].p;
        sRiderIkTargets_2.j[3].pos = gBikeSkelTemplate_4->n[5].p;

        if (B.leftFootOut != 0) {
            s32 x, y, z, p, q, n;
            f32 k;

            if (B.wheelieTimer != 0) {
                f32 f = COSS((gRaceFrame << 11) + 0x4000) * 0.00390625f + 128.0f;
                x = B.bike.right.x * f + B.bike.fwd.x * 128.0f;
                y = B.bike.right.y * f + B.bike.fwd.y * 128.0f;
                y += 100;
                z = B.bike.right.z * f + B.bike.fwd.z * 128.0f;
            } else {
                k = 256.0f;
                if (B.rightFootOut != 0) k = 64.0f;
                x = B.bike.right.x * k;
                x -= B.bike.right.z * k;
                z = -(B.bike.fwd.x * k);
                z += B.bike.fwd.z * k;
                y = height_get_ground(B.drawPos.x + sRiderIkTargets_2.j[3].pos.x + x, B.drawPos.y, B.drawPos.z + sRiderIkTargets_2.j[3].pos.z + z) - B.drawPos.y;
                y -= sRiderIkTargets_2.j[3].pos.y;
                y += 100;
                if (B.rightFootOut != 0) {
                    if (B.unk_412 > 0) {
                        p = (0x4000 - B.unk_412) / 64;
                        q = 0;
                    } else {
                        p = SINS(B.unk_412) / 64;
                        q = -COSS(B.unk_412) / 256;
                    }
                    {
                        f32 f;
                        p -= 200;
                        f = p;
                        x += f * B.bike.fwd.x;
                        z += f * B.bike.fwd.z;
                        y += q;
                    }
                }
            }
            n = B.leftFootOut;
            x = (x * n) >> 4;
            y = (y * n) >> 4;
            z = (z * n) >> 4;
            sRiderIkTargets_2.j[3].pos.x += x;
            sRiderIkTargets_2.j[3].pos.y += y;
            sRiderIkTargets_2.j[3].pos.z += z;
        }
        if (B.rightFootOut != 0) {
            s32 x, y, z, p, q, n;
            f32 k;

            if (B.wheelieTimer != 0) {
                f32 f = -COSS(gRaceFrame << 11) * 0.00390625f - 128.0f;
                x = B.bike.right.x * f + B.bike.fwd.x * 128.0f;
                y = B.bike.right.y * f + B.bike.fwd.y * 128.0f;
                y += 100;
                z = B.bike.right.z * f + B.bike.fwd.z * 128.0f;
            } else {
                k = 256.0f;
                if (B.leftFootOut != 0) k = 64.0f;
                x = -(B.bike.right.x * k);
                z = B.bike.fwd.x * k;
                x -= B.bike.right.z * k;
                z += B.bike.fwd.z * k;
                y = height_get_ground(B.drawPos.x + sRiderIkTargets_2.j[3].pos.x + x, B.drawPos.y, B.drawPos.z + sRiderIkTargets_2.j[3].pos.z + z) - B.drawPos.y;
                y -= sRiderIkTargets_2.j[2].pos.y;
                y += 100;
                if (B.leftFootOut != 0) {
                    if (B.unk_412 < 0) {
                        p = -(B.unk_412 + 0x4000) / 64;
                        q = 0;
                    } else {
                        p = -SINS(B.unk_412) / 64;
                        q = COSS(B.unk_412) / 256;
                    }
                    {
                        f32 f;
                        p -= 200;
                        f = p;
                        x += f * B.bike.fwd.x;
                        z += f * B.bike.fwd.z;
                        y += q;
                    }
                }
            }
            n = B.rightFootOut;
            x = (x * n) >> 4;
            y = (y * n) >> 4;
            z = (z * n) >> 4;
            sRiderIkTargets_2.j[2].pos.x += x;
            sRiderIkTargets_2.j[2].pos.y += y;
            sRiderIkTargets_2.j[2].pos.z += z;
        }
        mtx3i_from_angles(&sRiderIkTargets_2.j[0].rot, 0x1000, 0, B.drawRot[2] + B.drawSteer);
        mtx3i_from_angles(&sRiderIkTargets_2.j[1].rot, B.unk_5AC + 0x1000, 0, B.drawRot[2] + B.drawSteer);
        mtx3i_from_angles(&sRiderIkTargets_2.j[2].rot, B.drawRot[0] / 2, 0, B.drawRot[2]);
        mtx3i_from_angles(&sRiderIkTargets_2.j[3].rot, B.drawRot[0] / 2, 0, B.drawRot[2]);
        h = B.headYaw;
        if (D_800EA7B8[i] == 0) {
            if (h < 0) {
                h = SINS(h * 2) / 2 - 0x2000;
            } else {
                h = -SINS(h * 2) / 2 + 0x2000;
            }
        }
        st = h;
        if (st < 0) st = -st;
        h = (h * 12000) >> 14;
        mtx3i_from_angles(&sRiderIkTargets_2.rot, (0x4000 - SINS(st * 4)) >> 4, 0, B.drawRot[2] + h);
        rider_apply_ik(m);
        st = B.award;
        if (st != 0) {
            s32 o;
            struct { u8 pad[4]; s32 f4; } *pp = D_800C9E84;
            if (st >= 8) {
                if (pp->f4 - 8 < st) {
                    o = (pp->f4 - st) << 5;
                } else {
                    o = 0x100;
                }
            } else {
                o = st << 5;
            }
            rider_anim_overlay_limb(m, 0x12, 3, st, o);
        }
        if (B.anim >= 0) {
            rider_pose_snapshot(m, 0xF);
        }
    }
    if (B.anim >= 0) {
        rider_anim_apply(m, i, scale);
    }
    if (gRiderGfx_10[i].fA50 != 0) {
        ponytail_update(i, m, gRiderGfx_10[i].fA50, gRiderGfx_10[i].fA4C + gGfxFrameIndex * 0xC0);
    }
}

typedef struct Hdr_7 {
    u8 pad[0x10];
    u8 *p;
    u8 data[4];
} Hdr_7;

typedef struct Rec_21 {
    s32 f00;
    s32 *f04;
    s32 *f08;
    Hdr_7 *f0C;
    Hdr_7 *f10;
    u8 pad[0x3C - 0x14];
} Rec_21;

extern Rec_21 gRiderAnims_3[];

void rider_anims_load(u8 *param_1)
{
    s32 i;
    s32 v;
    Rec_21 *p;
    s32 m;
    Hdr_7 *r;

    if (gPodiumMode == 0) {
        for (i = 0; i < 0xF; i++) {
            v = res_find_gid(param_1, sRiderBoneNames[i]);
            sRiderBoneNodes[i] = v;
            if (v < 0) {
                fatal_halt(1);
            }
        }
        m = -1;
        for (i = 7; i >= 0; i--) {
            gRiders[i].animFrame = m;
        }
        for (i = 0; gRiderAnims_3[i].f04 != 0; i++) {
            if (i >= 16 || !(gGame & 0x10)) {
                gRiderAnims_3[i].f0C = res_load(param_1, *gRiderAnims_3[i].f04);
                gRiderAnims_3[i].f0C->p = gRiderAnims_3[i].f0C->data;
                if (gRiderAnims_3[i].f08 != 0) {
                    gRiderAnims_3[i].f10 = res_load(param_1, *gRiderAnims_3[i].f08);
                    gRiderAnims_3[i].f10->p = gRiderAnims_3[i].f10->data;
                }
            }
        }
    }
}

#define LERPROT(b, x, y, z, t)                                              {                                                                           u16 cx = (b)->rot[0];                                                   u16 cy;                                                                 u16 cz;                                                                 cx += (s16)((x) - cx) * (t) / 256;                                      cy = (b)->rot[1];                                                       cy += (s16)((y) - cy) * (t) / 256;                                      cz = (b)->rot[2];                                                       cz += (s16)((z) - cz) * (t) / 256;                                      mtx3i_from_angles((b)->mtx.w, (s16)cx, (s16)cy, (s16)cz);               }

typedef struct Anim {
    u8 p00[4];
    s32 count;
    u8 p08[8];
    s16 *data;
} Anim;

typedef struct Tab_4 {
    u32 flags;
    u8 p04[8];
    Anim *keys;
    Anim *sub;
    u8 p14[0x3C - 0x14];
} Tab_4;

extern Tab_4 gRiderAnims_4[];

static inline void lerpRot(Bone_2 *b, s32 x, s32 y, s32 z, s32 t)
{
    u16 cx = b->rot[0];
    u16 cy;
    u16 cz;
    cx += (s16)(x - cx) * t / 256;
    cy = b->rot[1];
    cy += (s16)(y - cy) * t / 256;
    cz = b->rot[2];
    cz += (s16)(z - cz) * t / 256;
    mtx3i_from_angles(b->mtx.w, (s16)cx, (s16)cy, (s16)cz);
}

static inline s16 *getFrame(s16 *d, s32 n, s32 size)
{
    return d + n * size;
}

void rider_anim_apply(Bone_2 *bones, s32 param_2, s32 param_3)
{
    s32 m0[10];
    s32 m1[10];
    s32 v0[4];
    s32 v1[3];
    s32 v2[3];
    s32 m2[14];
    u16 rx;
    u16 ry;
    u16 rz;
    Anim *sub;
    Tab_4 *t;
    Anim *keys;
    s32 s4;
    s16 *fr;
    s16 *sf;
    s8 *rot0;
    s8 *r;
    s32 i;
    s32 w;
    Rider *base;
    s32 off;

    {
        s8 ix = gRiders[param_2].anim;
        Tab_4 *tb = gRiderAnims_4;
        t = &tb[ix];
    }
    keys = t->keys;
    sub = t->sub;
    s4 = gRiders[param_2].animFrame >> 8;
    if ((t->flags & 8) != 0 && !(s4 < keys->count)) {
        s4 -= 5;
        s4 += (s16)(gRiders[param_2].trickCount % 5);
    }
    if (s4 < 0) {
        s4 = 0;
    }
    if (s4 >= keys->count) {
        s4 = keys->count - 1;
    }
    fr = getFrame(keys->data, s4, 26);
    rot0 = (s8 *)fr + 6;

    if (param_3 == 0x100) {
        for (i = 1; i < 15; i++) {
            mtx3i_from_angles(bones[i].mtx.w, rot0[i * 3] << 8, rot0[i * 3 + 1] << 8, rot0[i * 3 + 2] << 8);
        }
    } else {
        w = param_3;
        for (i = 1; i < 15; i++) {
            lerpRot(&bones[i], rot0[i * 3] << 8, rot0[i * 3 + 1] << 8, rot0[i * 3 + 2] << 8, w);
        }
    }

    if (sub != 0) {
        sf = getFrame(sub->data, s4, 6);
        v0[0] = fr[0] - sf[0];
        v0[1] = fr[1] - sf[1];
        v0[2] = fr[2] - sf[2];
        r = (s8 *)sf + 6;
        mtx3i_from_angles(m0, r[0] << 8, r[1] << 8, r[2] << 8);
        mtx3i_transpose(m0);
        vec3i_mul_mtx3i(v0, m0, v1);
        v2[0] = v1[0];
        v2[1] = v1[1];
        v2[2] = v1[2];
        mtx3i_from_angles(m1, rot0[0] << 8, rot0[1] << 8, rot0[2] << 8);
        mtx3i_mul(m1, m0, m2);
    } else {
        mtx3i_from_angles(m2, rot0[0] << 8, rot0[1] << 8, rot0[2] << 8);
        v2[0] = fr[0];
        v2[1] = fr[1];
        v2[2] = fr[2];
    }

    base = gRiders;
    off = param_2 * sizeof(Rider);
    if (((Rider *)((u8 *)base + off))->crashState == 0) {
        mtx3i_from_angles(m0, gRiders[param_2].drawRot[0], gRiders[param_2].drawRot[1], gRiders[param_2].drawRot[2]);
        vec3i_mul_mtx3i(v2, m0, v0);
        mtx3i_mul(m2, m0, bones->mtx.w);
    } else {
        if (param_3 == 0) {
            w = 1;
            i = 2;
        } else {
            w = 3;
            i = 4;
        }
        quat_to_mtx3i(m1, ((Rider *)((u8 *)base + off))->body.quat);
        mtx3i_mul(m2, m1, bones->mtx.w);
        vec3i_mul_mtx3i(v2, m1, v0);
    }

    if (param_3 != 0x100 && gRiders[param_2].crashPhase != 1) {
        for (i = 0; i < 3; i++) {
            bones->pos.w[i] = (v0[i] * param_3 + bones->base[i] * (0x100 - param_3)) / 256;
        }
        mtx3i_to_angles(bones->mtx.w, &rx, &ry, &rz);
        w = param_3;
        LERPROT(bones, rx, ry, rz, w);
    } else {
        bones->pos.w[0] = v0[0];
        bones->pos.w[1] = v0[1];
        bones->pos.w[2] = v0[2];
    }

    if (gRiders[param_2].crashState != 0) {
        bones->pos.w[0] = 0;
        bones->pos.w[1] = 0;
        bones->pos.w[2] = 0;
    }
    bones->pmtx = bones->mtx;
    bones->ppos = bones->pos;
    skel_update_world_transforms((s32 *)bones, 0xf);
}

typedef struct { u8 pad[0x10]; u8 *anim; u8 rest[0x28]; } Slot_8;

extern Slot_8 gRiderAnims_5[];

void bike_update_trick_offset(u8 *param_1)
{
    s8 idx;
    u8 *ent;
    u8 *st;
    s16 *ps;
    s8 *cp;
    s32 n;
    s32 iv;
    s32 a2;
    s32 six;
    f32 f;

    if (gPodiumMode != 0) {
        return;
    }
    *(s32 *)(param_1 + 0x3A8) = 0;
    if (*(s8 *)(param_1 + 0x3D0) == -1) {
        return;
    }
    st = gRiderAnims_5[*(s8 *)(param_1 + 0x3D0)].anim;
    if (st == 0) {
        return;
    }
    a2 = *(s32 *)(param_1 + 0x3AC);
    iv = a2 >> 8;
    if (iv < 0) {
        iv = 0;
    }
    n = *(s32 *)(st + 4);
    if (iv >= n) {
        iv = n - 1;
    }
    six = 6;
    ps = (s16 *)*(u8 **)(st + 0x10) + iv * six;
    f = (f32)a2 * 0.0009765625f;
    cp = (s8 *)((u8 *)ps + 6);
    *(f32 *)(param_1 + 0x3A8) = f;
    if (1.0f < f) {
        *(f32 *)(param_1 + 0x3A8) = 1.0f;
    }
    quat_from_euler((f32 *)(param_1 + 0x38C),
                  (f32)(*(s8 *)((u8 *)ps + 6)) * 3.1415925f * 0.0078125f,
                  (f32)(cp[1]) * 3.1415925f * 0.0078125f,
                  (f32)(cp[2]) * 3.1415925f * 0.0078125f);
    *(f32 *)(param_1 + 0x39C) = (f32)(ps[0]) / 1000.0f;
    *(f32 *)(param_1 + 0x3A0) = (f32)(ps[1]) / 1000.0f;
    *(f32 *)(param_1 + 0x3A4) = (f32)(ps[2]) / 1000.0f;
}

typedef struct {
    s32 w0;
    s32 count;
    s32 w2;
    s32 w3;
    s8 *data;
} EbSeq;

typedef struct {
    s32 pad0[4];
    EbSeq *seq;
    s32 pad1[10];
} EbRow;

extern EbRow gRiderAnims_6[];

void bike_skel_update(s32 *p1, s32 p2)
{
    Rider *blk;
    Rider *gp;
    s32 v;
    s32 n;
    s32 nn;
    s32 idx;
    s32 i;
    EbSeq *e;
    s8 *bp;
    s8 *d;
    s32 k;
    Quad *src;
    Quad *dst;
    Quad *end;
    s32 a88[10];
    s32 a60[18];

    if (gPodiumMode == 0) {
        gp = gRiders;
        blk = &gp[p2];
        v = blk->drawSuspFront;
        n = blk->drawSuspRear;
        quat_to_mtx3i(p1 + 0x10, gRiders[p2].drawQuat);
        v = v - 0x32;
        if (v > 200) {
            v = 200;
        }
        if (v < -0x3c) {
            v = -0x3c;
        }
        p1[0x42] = v - 0x222;
        p1[0x43] = 0x106 - (v * 2) / 5;
        mtx3i_from_angles(p1 + 0xe4, n * 0x18, 0, 0);
        mtx3i_from_angles(a88, 4000, 0, blk->drawSteer);
        mtx3i_from_angles(a60, -4000, 0, 0);
        mtx3i_mul(a88, a60, p1 + 0x24);
        mtx3i_from_angles(p1 + 100, (s16)blk->rearSpinAngle, 0, 0);
        mtx3i_from_angles(p1 + 0x104, (s16)blk->frontSpinAngle, 0, 0);
        if (blk->anim != -1) {
            e = gRiderAnims_6[blk->anim].seq;
            if (e != 0) {
                idx = blk->animFrame >> 8;
                if (idx < 0) {
                    idx = 0;
                }
                if (idx >= e->count) {
                    idx = e->count - 1;
                }
                d = e->data;
                k = 6;
                for (i = 1; i < 2; i++) {
                    bp = d + idx * k * 2 + k + i * 3;
                    mtx3i_from_angles(p1 + i * 0x20 + 4, bp[0] << 8, bp[1] << 8, bp[2] << 8);
                }
            }
        }
        skel_update_world_transforms(p1, 10);
        dst = (Quad *)gRiders[p2].bikeSkel;
        src = (Quad *)p1;
        end = src + 0x50;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end);
    }
}

void rider_update_head_look(void)
{
    s32 i, j, best, bi, dx, dz, dist;
    u16 ang, sang;
    s16 v;

    if (D_800EE140 != 0) return;
    for (i = 0; i < 8; i++) {
        best = 625;
        bi = -1;
        if (gRiders[i].crashState) {
            gRiders[i].headYaw = 0;
            if (i) dx = 1; else dx = 2;
            continue;
        }
        if (((gPlayerInputs[i] & 0x20) && gRiders[i].rightFootOut && gRiders[i].leftFootOut) || gRiders[i].award) {
            D_800EA7B8[i] = 1;
            v = 24000;
        } else {
            D_800EA7B8[i] = 0;
            for (j = 0; j < 8; j++) {
                if (j == i) continue;
                dx = (gRiders[j].drawPos.x >> 4) - (gRiders[i].drawPos.x >> 4);
                dz = (gRiders[j].drawPos.z >> 4) - (gRiders[i].drawPos.z >> 4);
                while (dx > 0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dx < -0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dz > 0xFFFF) { dz >>= 1; dx >>= 1; }
                while (dz < -0xFFFF) { dz >>= 1; dx >>= 1; }
                math_to_polar(dx, dz, &ang, &dist);
                if (dist < best) {
                    best = dist;
                    bi = j;
                    sang = ang;
                }
            }
            v = 0;
            if (bi != -1 && gRiders[i].headTimer <= 0) v = -sang - (u16)gRiders[i].drawRot[2];
            if (v < -0x4000) v = -0x4000;
            if (v > 0x4000) v = 0x4000;
        }
        if (gRiders[i].headYaw < v) gRiders[i].headYaw += 0x300;
        if (gRiders[i].headYaw > v) gRiders[i].headYaw -= 0x300;
        if (--gRiders[i].headTimer < -0x78) gRiders[i].headTimer = (s32)(random_byte() * 300) / 256 + 150;
    }
}

void rider_update_input_history(Rider *p, s32 n)
{
    s32 v;
    s32 w;
    s32 i;
    s32 sum;
    s32 sum2;

    v = p->stickY;
    w = p->stickX;
    if (v > 15) {
        v = 15;
    }
    for (i = 0; i < n; i++) {
        p->stickHist8[p->stickHist8Idx] = v;
        p->stickHist8Idx = (p->stickHist8Idx + 1) % 8;
        p->stickHist16[p->stickHist16Idx] = w;
        p->stickHist16Idx = (p->stickHist16Idx + 1) & 0xF;
    }
    p->inHist[p->inHistIdx] = p->drawPos.y;
    p->inHistIdx = (p->inHistIdx + 1) % 8;
    sum = 0;
    for (i = 0; i < 8; i++) {
        sum += p->stickHist8[i];
    }
    p->stickHist8Avg = sum / 8;
    sum2 = 0;
    for (i = 0; i < 8; i++) {
        sum2 += p->inHist[i];
    }
    p->inHistAvg = sum2 / 8;
}

void func_8003F128(void)
{
    s32 i;
    u32 t;
    s32 v;

    i = 0;
    t = gRaceFrame;
    do {
        if (gRiders[i].unk_418 > 0) {
            if ((s16)gRiders[i].unk_410 < 0x1E) {
                gRiders[i].unk_410 = (u16)gRiders[i].unk_410 + 1;
            }
        } else {
            if ((s16)gRiders[i].unk_410 > 0) {
                gRiders[i].unk_410 = (u16)gRiders[i].unk_410 - 2;
            }
        }
        if (D_800E1B50[i] & 0x2000) {
            gRiders[i].unk_778 = t;
        }
        if ((s32)(t - (u32)gRiders[i].unk_778) < 0x3C) {
            v = (u16)gRiders[i].unk_77C + 0x10;
            gRiders[i].unk_77C = v;
            if ((s16)v > 0x100) {
                gRiders[i].unk_77C = 0x100;
            }
        } else {
            v = (u16)gRiders[i].unk_77C - 0x10;
            gRiders[i].unk_77C = v;
            if ((s16)v < 0) {
                gRiders[i].unk_77C = 0;
            }
        }
        i++;
    } while (i < 8);
}

typedef struct {
    char pad[0x444];
    Pal *cfg;
} Blk_12;

extern Blk_12 gRiderAnims_7;

void rider_update_fist_pump(void)
{
    s32 i = 0;

    do {
        if (gRiders[i].award == 0) {
            i++;
            continue;
        }
        if ((gRaceFrame & 1) != 0) {
            u8 v = gRiders[i].award + 1;
            gRiders[i].award = v;
            if ((s8)v >= gRiderAnims_7.cfg->f4) {
                gRiders[i].award = 0;
            }
        }
        i++;
    } while (i < 8);
}

typedef struct { s8 v[3]; } B3;

struct Row_4 {
    s32 f0;
    V3_2 pos;          /* 0x04 */
    M9 m;            /* 0x10 */
    u8 pad34[0x0C];  /* 0x34 */
    M9 m2;           /* 0x40 */
    u16 c[3];        /* 0x64 */
    u16 c2[3];       /* 0x6A */
    u8 pad70[0x10];
};

typedef struct {
    u8 pad[0xC];
    s32 *tbl;
    u8 pad2[0x3C - 0x10];
} TEnt;

extern TEnt gRiderAnims_8[];

void rider_anim_overlay_limb(Row_4 *p1, s32 param_2, s32 p3, s32 p4, s32 arg4)
{
    s32 *tbl;
    s32 half;
    B3 *pb;
    s32 k;
    s32 sc;
    s32 j; s32 t;
    u16 a, b, c;
    V3_2 save;

    tbl = gRiderAnims_8[param_2].tbl;
    half = (tbl[0] * 3 + 7) / 2;
    if (p4 < 0) p4 = 0;
    if (tbl[1] - 1 < p4) p4 = tbl[1] - 1;
    pb = (B3 *)((u8 *)tbl[4] + half * p4 * 2 + 6);

    for (k = 0; k < 15; k++) {
        mtx3i_to_angles((s32 *)&p1[k].m, &a, &b, &c);
        p1[k].c[0] = a;
        p1[k].c[1] = b;
        p1[k].c[2] = c;
    }
    k = 0x100;
    p1[0].c2[0] = *(u16 *)((u8 *)p1 + 6);
    p1[0].c2[1] = *(u16 *)((u8 *)p1 + 0xA);
    p1[0].c2[2] = *(u16 *)((u8 *)p1 + 0xE);
    save = p1[p3].pos;
    for (j = 0; j < 3; j++) {

        u16 c0 = p1[p3 + j].c[0];
        s8 x = pb[p3 + j].v[0];
        s8 y;
        s8 z;
        t = (s16)((x << 8) - c0) * k; if (t) p4 = 1; else p4 = 2;
        y = pb[p3 + j].v[1];
        t = (s16)((y << 8) - p1[p3 + j].c[1]) * k; if (t) p4 = 3; else p4 = 4;
        z = pb[p3 + j].v[2];
        t = (s16)((z << 8) - p1[p3 + j].c[2]) * k; if (t) p4 = 5; else p4 = 6;
        mtx3i_from_angles((s32 *)&p1[p3 + j].m, (s16)(x << 8), (s16)(y << 8), (s16)(z << 8));
    }
    mtx3i_mul((s32 *)&p1[p3].m, (s32 *)((u8 *)p1 + 0xC0), (s32 *)&p1[p3].m2);
    p1[p3].m = p1[p3].m2;
    *(M9 *)&p1[p3].pos = *(M9 *)p1[p3].pad34;
    p1[p3 + 1].f0 = 0;
    p1[p3 + 2].f0 = 1;
    skel_update_world_transforms((s32 *)&p1[p3], 3);
    p1[p3 + 1].f0 = p3;
    p1[p3 + 2].f0 = p3 + 1;
    p1[p3].pos = save;
}

s32 rider_anim_get_length(s32 a0, s32 *a1)
{
    u8 *base;
    u8 *rec;

    base = gRiderAnims_9;
    rec = base + a0 * 0x3C;
    *a1 = *(s32 *)rec;
    return *(s32 *)(*(u8 **)(rec + 0xC) + 4);
}

void rider_pose_snapshot(Ent_39 *p, s32 n)
{
    s32 i;
    u16 x, y, z;

    for (i = 0; i < n; i++) {
        mtx3i_to_angles(&p[i].f10, &x, &y, &z);
        p[i].f64 = x;
        p[i].f66 = y;
        p[i].f68 = z;
    }
    p->f6A = p->f06;
    p->f6C = p->f0A;
    p->f6E = p->f0E;
}

s32 rider_ik_split_len(s32 a, s32 b, s32 c) {
    s32 t = (a + b) * (a - b);
    s32 d = c << 1;

    return t / d + c / 2;
}

typedef struct {
    u32 f0;
    void *f4;
    u32 f8;
    u32 fc;
    u32 f10;
    u8 pad[0x3C - 0x14];
} Slot_10;

extern Slot_10 gRiderAnims_10[];

void rider_anims_unload(void) {
    s32 i;

    if (gPodiumMode == 0 && gRiderAnims_10[0].f4 != NULL) {
        i = 0;
        do {
            gRiderAnims_10[i].fc = 0;
            gRiderAnims_10[i].f10 = 0;
            i++;
        } while (gRiderAnims_10[i].f4 != NULL);
    }
}
