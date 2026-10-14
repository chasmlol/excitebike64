#include "game.h"

typedef struct { s32 w[9]; } M9;

void rider_ik_arm(Bone *j, s32 idx, Vec3i *tgt, Mtx3 *o)
{
    Mtx3 m;
    Mtx3 m2;
    Vec3i t;
    Vec3i d;
    s16 a1;
    s32 w;
    s16 a2, a3, a4;
    s32 rot, dist, n, l1, l2, x, rad;

    t.x = -(o->m[6] * 80) / 16384 + tgt->x;
    t.y = -(o->m[7] * 80) / 16384 + tgt->y;
    t.z = -(o->m[8] * 80) / 16384 + tgt->z;
    d.x = t.x - j[idx].worldPos.x;
    d.y = t.y - j[idx].worldPos.y;
    d.z = t.z - j[idx].worldPos.z;
    m = j[1].worldRot;
    mtx3i_transpose(&m);
    vec3i_mul_mtx3i(&d, &m, &t);
    if (idx == 3) {
        rot = -0x2000;
    } else {
        rot = 0x2000;
    }
    rot = rot * sRiderIkTargets.armTwist / 256;
    mtx3i_from_angles(&m, 0, rot, 0);
    vec3i_mul_mtx3i(&t, &m, &d);
    dist = isqrt_u32_3(d.x * d.x + d.y * d.y + d.z * d.z);
    n = j[idx + 1].pos.z + j[idx + 2].pos.z;
    if (n < dist) {
        d.x = d.x * n / dist;
        d.y = d.y * n / dist;
        d.z = d.z * n / dist;
        dist = n;
    }
    l1 = j[idx + 1].pos.z;
    l2 = j[idx + 2].pos.z;
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

void rider_ik_leg(Bone *j, s32 idx, Vec3i *tgt, s32 *arg3)
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
    d.x = t.x - j[idx].worldPos.x;
    d.y = t.y - j[idx].worldPos.y;
    d.z = t.z - j[idx].worldPos.z;
    m = j->worldRot;
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
    n = -(j[idx + 1].pos.y + j[idx + 2].pos.y);
    if (n < dist) {
        d.x = d.x * n / dist;
        d.y = d.y * n / dist;
        d.z = d.z * n / dist;
        dist = n;
    }
    l1 = j[idx + 1].pos.y;
    n1 = -l1;
    n2 = -j[idx + 2].pos.y;
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

void rider_apply_ik(Bone *b)
{
    s32 *p;
    s32 *q;
    s32 *o;
    volatile s32 pad[10];

    p = b[0].worldRot.m;
    q = b[1].worldRot.m;
    mtx3i_mul(b[1].rot.m, p, q);
    vec3i_mul_mtx3i(&b[1].pos.x, p, &b[1].worldPos.x);
    b[1].worldPos.x = b[1].worldPos.x + b[0].worldPos.x;
    b[1].worldPos.y = b[1].worldPos.y + b[0].worldPos.y;
    b[1].worldPos.z = b[1].worldPos.z + b[0].worldPos.z;
    vec3i_mul_mtx3i(&b[3].pos.x, q, &b[3].worldPos.x);
    b[3].worldPos.x = b[3].worldPos.x + b[1].worldPos.x;
    b[3].worldPos.y = b[3].worldPos.y + b[1].worldPos.y;
    b[3].worldPos.z = b[3].worldPos.z + b[1].worldPos.z;
    vec3i_mul_mtx3i(&b[6].pos.x, q, &b[6].worldPos.x);
    b[6].worldPos.x = b[6].worldPos.x + b[1].worldPos.x;
    b[6].worldPos.y = b[6].worldPos.y + b[1].worldPos.y;
    b[6].worldPos.z = b[6].worldPos.z + b[1].worldPos.z;
    vec3i_mul_mtx3i(&b[9].pos.x, p, &b[9].worldPos.x);
    b[9].worldPos.x = b[9].worldPos.x + b[0].worldPos.x;
    b[9].worldPos.y = b[9].worldPos.y + b[0].worldPos.y;
    b[9].worldPos.z = b[9].worldPos.z + b[0].worldPos.z;
    vec3i_mul_mtx3i(&b[12].pos.x, p, &b[12].worldPos.x);
    b[12].worldPos.x = b[12].worldPos.x + b[0].worldPos.x;
    b[12].worldPos.y = b[12].worldPos.y + b[0].worldPos.y;
    b[12].worldPos.z = b[12].worldPos.z + b[0].worldPos.z;

    rider_ik_arm(b, 3, &sRiderIkTargets.limb[0].pos, &sRiderIkTargets.limb[0].rot);
    rider_ik_arm(b, 6, &sRiderIkTargets.limb[1].pos, &sRiderIkTargets.limb[1].rot);
    rider_ik_leg(b, 9, &sRiderIkTargets.limb[2].pos, sRiderIkTargets.limb[2].rot.m);
    rider_ik_leg(b, 12, &sRiderIkTargets.limb[3].pos, sRiderIkTargets.limb[3].rot.m);
    skel_update_world_transforms(b, 15);

    b[5].worldRot = sRiderIkTargets.limb[0].rot;
    b[8].worldRot = D_800EA6CC;
    b[11].worldRot = D_800EA6FC;
    b[14].worldRot = D_800EA72C;
    o = b[2].worldRot.m;
    *(Mtx3 *)o = D_800EA750;
}

#define B (gRiders[i])
#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]
#define COSS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

typedef struct {
    u8 pad[0xA4C];
    u8 *fA4C;
    Bone *fA50;
    u8 padA54[0x2C];
} Slot_7;

extern Slot_7 gRiderGfx_10[];

void rider_update_pose(RiderSkeleton *m, s32 i) {
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
        fl = gRiderAnims[st].flags;
        if (fl & 1) {
            if (B.animFrame < 0x400) scale = B.animFrame / 4;
        } else if (fl & 2) {
            fl = gRiderAnims[st].body->count;
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
        rider_pose_snapshot(m->bones, 0xF);
        B.anim = st;
        B.animFrame = t;
    } else if (B.anim == -1 || scale != 0x100) {
        v[0] = 0;
        a = (B.drawRot[1] * 250) / 8192;
        if (a > 0) a = -a;
        if (a < -500) a = -500;
        c = B.crestCrouch * 10;
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
        sRiderIkTargets.armTwist = 0x80 - s3 * 16;
        if (sRiderIkTargets.armTwist < 0) sRiderIkTargets.armTwist = 0;
        mtx3i_from_angles(&mtx, B.drawRot[0], (B.drawRot[1] * 7) / 8, B.drawRot[2]);
        vec3i_mul_mtx3i(v, mtx.m, out);
        m->bones[0].pos.x = m->bones[0].worldPos.x = out[0];
        m->bones[0].pos.y = m->bones[0].worldPos.y = out[1];
        m->bones[0].pos.z = m->bones[0].worldPos.z = out[2];
        mtx3i_from_angles(&m->bones[0].rot, B.drawRot[0] + 4000, 0, B.drawRot[2]);
        m->bones[0].worldRot = m->bones[0].rot;
        r = s3 * 128 - 4000;
        mtx3i_from_angles(&m->bones[1].rot, sRiderChestPitch[i] = d - r, 0, 0);
        sRiderIkTargets.limb[0].pos = gBikeSkelTemplate->bones[4].worldPos;
        sRiderIkTargets.limb[1].pos = gBikeSkelTemplate->bones[5].worldPos;
        sRiderIkTargets.limb[2].pos = gBikeSkelTemplate->bones[6].worldPos;
        sRiderIkTargets.limb[3].pos = gBikeSkelTemplate->bones[9].worldPos;

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
                y = height_get_ground(B.drawPos.x + sRiderIkTargets.limb[3].pos.x + x, B.drawPos.y, B.drawPos.z + sRiderIkTargets.limb[3].pos.z + z) - B.drawPos.y;
                y -= sRiderIkTargets.limb[3].pos.y;
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
            sRiderIkTargets.limb[3].pos.x += x;
            sRiderIkTargets.limb[3].pos.y += y;
            sRiderIkTargets.limb[3].pos.z += z;
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
                y = height_get_ground(B.drawPos.x + sRiderIkTargets.limb[3].pos.x + x, B.drawPos.y, B.drawPos.z + sRiderIkTargets.limb[3].pos.z + z) - B.drawPos.y;
                y -= sRiderIkTargets.limb[2].pos.y;
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
            sRiderIkTargets.limb[2].pos.x += x;
            sRiderIkTargets.limb[2].pos.y += y;
            sRiderIkTargets.limb[2].pos.z += z;
        }
        mtx3i_from_angles(sRiderIkTargets.limb[0].rot.m, 0x1000, 0, B.drawRot[2] + B.drawSteer);
        mtx3i_from_angles(sRiderIkTargets.limb[1].rot.m, B.throttleTwist + 0x1000, 0, B.drawRot[2] + B.drawSteer);
        mtx3i_from_angles(sRiderIkTargets.limb[2].rot.m, B.drawRot[0] / 2, 0, B.drawRot[2]);
        mtx3i_from_angles(sRiderIkTargets.limb[3].rot.m, B.drawRot[0] / 2, 0, B.drawRot[2]);
        h = B.headYaw;
        if (sRiderLookBack[i] == 0) {
            if (h < 0) {
                h = SINS(h * 2) / 2 - 0x2000;
            } else {
                h = -SINS(h * 2) / 2 + 0x2000;
            }
        }
        st = h;
        if (st < 0) st = -st;
        h = (h * 12000) >> 14;
        mtx3i_from_angles(sRiderIkTargets.hips.m, (0x4000 - SINS(st * 4)) >> 4, 0, B.drawRot[2] + h);
        rider_apply_ik(m->bones);
        st = B.fistPump;
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
            rider_pose_snapshot(m->bones, 0xF);
        }
    }
    if (B.anim >= 0) {
        rider_anim_apply(m, i, scale);
    }
    if (gRiderGfx_10[i].fA50 != 0) {
        ponytail_update(i, m, gRiderGfx_10[i].fA50, gRiderGfx_10[i].fA4C + gGfxFrameIndex * 0xC0);
    }
}


void rider_anims_load(u8 *param_1)
{
    s32 i;
    s32 v;
    RiderAnim *p;
    s32 m;
    AnimClip *r;

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
        for (i = 0; gRiderAnims[i].bodyRes != 0; i++) {
            if (i >= 16 || !(gGame.flags & 0x10)) {
                gRiderAnims[i].body = res_load(param_1, *gRiderAnims[i].bodyRes);
                gRiderAnims[i].body->data = gRiderAnims[i].body->frames;
                if (gRiderAnims[i].rootRes != 0) {
                    gRiderAnims[i].root = res_load(param_1, *gRiderAnims[i].rootRes);
                    gRiderAnims[i].root->data = gRiderAnims[i].root->frames;
                }
            }
        }
    }
}

#define LERPROT(b, x, y, z, t)                                              {                                                                           u16 cx = (b)->angles[0];                                                u16 cy;                                                                 u16 cz;                                                                 cx += (s16)((x) - cx) * (t) / 256;                                      cy = (b)->angles[1];                                                    cy += (s16)((y) - cy) * (t) / 256;                                      cz = (b)->angles[2];                                                    cz += (s16)((z) - cz) * (t) / 256;                                      mtx3i_from_angles((b)->rot.m, (s16)cx, (s16)cy, (s16)cz);               }


static inline void lerpRot(Bone *b, s32 x, s32 y, s32 z, s32 t)
{
    u16 cx = b->angles[0];
    u16 cy;
    u16 cz;
    cx += (s16)(x - cx) * t / 256;
    cy = b->angles[1];
    cy += (s16)(y - cy) * t / 256;
    cz = b->angles[2];
    cz += (s16)(z - cz) * t / 256;
    mtx3i_from_angles(&b->rot.m, (s16)cx, (s16)cy, (s16)cz);
}

static inline s16 *getFrame(s16 *d, s32 n, s32 size)
{
    return d + n * size;
}

void rider_anim_apply(Bone *bones, s32 param_2, s32 param_3)
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
    AnimClip *sub;
    RiderAnim *t;
    AnimClip *keys;
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
        RiderAnim *tb = gRiderAnims;
        t = &tb[ix];
    }
    keys = t->body;
    sub = t->root;
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
            mtx3i_from_angles(bones[i].rot.m, rot0[i * 3] << 8, rot0[i * 3 + 1] << 8, rot0[i * 3 + 2] << 8);
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
        mtx3i_mul(m2, m0, &bones->rot.m);
    } else {
        if (param_3 == 0) {
            w = 1;
            i = 2;
        } else {
            w = 3;
            i = 4;
        }
        quat_to_mtx3i(m1, ((Rider *)((u8 *)base + off))->body.quat);
        mtx3i_mul(m2, m1, &bones->rot.m);
        vec3i_mul_mtx3i(v2, m1, v0);
    }

    if (param_3 != 0x100 && gRiders[param_2].crashPhase != 1) {
        for (i = 0; i < 3; i++) {
            (&bones->pos.x)[i] = (v0[i] * param_3 + bones->baseAngles[i] * (0x100 - param_3)) / 256;
        }
        mtx3i_to_angles(&bones->rot.m, &rx, &ry, &rz);
        w = param_3;
        LERPROT(bones, rx, ry, rz, w);
    } else {
        bones->pos.x = v0[0];
        bones->pos.y = v0[1];
        bones->pos.z = v0[2];
    }

    if (gRiders[param_2].crashState != 0) {
        bones->pos.x = 0;
        bones->pos.y = 0;
        bones->pos.z = 0;
    }
    bones->worldRot = bones->rot;
    bones->worldPos = bones->pos;
    skel_update_world_transforms(bones, 0xf);
}


void bike_update_trick_offset(Rider *param_1)
{
    s8 idx;
    u8 *ent;
    AnimClip *st;
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
    param_1->trickBlend = 0.0f;
    if (param_1->anim == -1) {
        return;
    }
    st = gRiderAnims[param_1->anim].root;
    if (st == 0) {
        return;
    }
    a2 = param_1->animFrame;
    iv = a2 >> 8;
    if (iv < 0) {
        iv = 0;
    }
    n = st->count;
    if (iv >= n) {
        iv = n - 1;
    }
    six = 6;
    ps = (s16 *)st->data + iv * six;
    f = (f32)a2 * 0.0009765625f;
    cp = (s8 *)((u8 *)ps + 6);
    param_1->trickBlend = f;
    if (1.0f < f) {
        param_1->trickBlend = 1.0f;
    }
    quat_from_euler(param_1->trickQuat,
                  (f32)(*(s8 *)((u8 *)ps + 6)) * 3.1415925f * 0.0078125f,
                  (f32)(cp[1]) * 3.1415925f * 0.0078125f,
                  (f32)(cp[2]) * 3.1415925f * 0.0078125f);
    param_1->trickOffset.x = (f32)(ps[0]) / 1000.0f;
    param_1->trickOffset.y = (f32)(ps[1]) / 1000.0f;
    param_1->trickOffset.z = (f32)(ps[2]) / 1000.0f;
}


void bike_skel_update(BikeSkeleton *p1, s32 p2)
{
    Rider *blk;
    Rider *gp;
    s32 v;
    s32 n;
    s32 nn;
    s32 idx;
    s32 i;
    AnimClip *e;
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
        quat_to_mtx3i(p1->bones[0].worldRot.m, gRiders[p2].drawQuat);
        v = v - 0x32;
        if (v > 200) {
            v = 200;
        }
        if (v < -0x3c) {
            v = -0x3c;
        }
        p1->bones[2].pos.y = v - 0x222;
        p1->bones[2].pos.z = 0x106 - (v * 2) / 5;
        mtx3i_from_angles(p1->bones[7].rot.m, n * 0x18, 0, 0);
        mtx3i_from_angles(a88, 4000, 0, blk->drawSteer);
        mtx3i_from_angles(a60, -4000, 0, 0);
        mtx3i_mul(a88, a60, p1->bones[1].rot.m);
        mtx3i_from_angles(p1->bones[3].rot.m, (s16)blk->frontSpinAngle, 0, 0);
        mtx3i_from_angles(p1->bones[8].rot.m, (s16)blk->rearSpinAngle, 0, 0);
        if (blk->anim != -1) {
            e = gRiderAnims[blk->anim].root;
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
                    mtx3i_from_angles(p1->bones[i].rot.m, bp[0] << 8, bp[1] << 8, bp[2] << 8);
                }
            }
        }
        skel_update_world_transforms(p1->bones, 10);
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

    if (gDebugFreeze != 0) return;
    for (i = 0; i < 8; i++) {
        best = 625;
        bi = -1;
        if (gRiders[i].crashState) {
            gRiders[i].headYaw = 0;
            if (i) dx = 1; else dx = 2;
            continue;
        }
        if (((gPlayerInputs[i] & 0x20) && gRiders[i].rightFootOut && gRiders[i].leftFootOut) || gRiders[i].fistPump) {
            sRiderLookBack[i] = 1;
            v = 24000;
        } else {
            sRiderLookBack[i] = 0;
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

void rider_update_crouch(void)
{
    s32 i;
    u32 t;
    s32 v;

    i = 0;
    t = gRaceFrame;
    do {
        if (gRiders[i].crestAhead > 0) {
            if ((s16)gRiders[i].crestCrouch < 0x1E) {
                gRiders[i].crestCrouch = (u16)gRiders[i].crestCrouch + 1;
            }
        } else {
            if ((s16)gRiders[i].crestCrouch > 0) {
                gRiders[i].crestCrouch = (u16)gRiders[i].crestCrouch - 2;
            }
        }
        if (gRiderGroundSurface[i] & 0x2000) {
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

void rider_update_fist_pump(void)
{
    s32 i = 0;

    do {
        if (gRiders[i].fistPump == 0) {
            i++;
            continue;
        }
        if ((gRaceFrame & 1) != 0) {
            u8 v = gRiders[i].fistPump + 1;
            gRiders[i].fistPump = v;
            if ((s8)v >= gRiderAnims[18].body->count) {
                gRiders[i].fistPump = 0;
            }
        }
        i++;
    } while (i < 8);
}

typedef struct { s8 v[3]; } B3;


void rider_anim_overlay_limb(Bone *p1, s32 param_2, s32 p3, s32 p4, s32 arg4)
{
    AnimClip *tbl;
    s32 half;
    B3 *pb;
    s32 k;
    s32 sc;
    s32 j; s32 t;
    u16 a, b, c;
    Vec3i save;

    tbl = gRiderAnims[param_2].body;
    half = (tbl->bones * 3 + 7) / 2;
    if (p4 < 0) p4 = 0;
    if (tbl->count - 1 < p4) p4 = tbl->count - 1;
    pb = (B3 *)((u8 *)tbl->data + half * p4 * 2 + 6);

    for (k = 0; k < 15; k++) {
        mtx3i_to_angles(p1[k].rot.m, &a, &b, &c);
        p1[k].angles[0] = a;
        p1[k].angles[1] = b;
        p1[k].angles[2] = c;
    }
    k = 0x100;
    p1[0].baseAngles[0] = (u16)p1[0].pos.x;
    p1[0].baseAngles[1] = (u16)p1[0].pos.y;
    p1[0].baseAngles[2] = (u16)p1[0].pos.z;
    save = p1[p3].pos;
    for (j = 0; j < 3; j++) {

        u16 c0 = p1[p3 + j].angles[0];
        s8 x = pb[p3 + j].v[0];
        s8 y;
        s8 z;
        t = (s16)((x << 8) - c0) * k; if (t) p4 = 1; else p4 = 2;
        y = pb[p3 + j].v[1];
        t = (s16)((y << 8) - p1[p3 + j].angles[1]) * k; if (t) p4 = 3; else p4 = 4;
        z = pb[p3 + j].v[2];
        t = (s16)((z << 8) - p1[p3 + j].angles[2]) * k; if (t) p4 = 5; else p4 = 6;
        mtx3i_from_angles(p1[p3 + j].rot.m, (s16)(x << 8), (s16)(y << 8), (s16)(z << 8));
    }
    mtx3i_mul(p1[p3].rot.m, p1[1].worldRot.m, p1[p3].worldRot.m);
    p1[p3].rot = p1[p3].worldRot;
    *(M9 *)&p1[p3].pos = *(M9 *)&p1[p3].worldPos;
    p1[p3 + 1].parent = 0;
    p1[p3 + 2].parent = 1;
    skel_update_world_transforms(&p1[p3], 3);
    p1[p3 + 1].parent = p3;
    p1[p3 + 2].parent = p3 + 1;
    p1[p3].pos = save;
}

s32 rider_anim_get_length(s32 a0, s32 *a1)
{
    RiderAnim *base;
    RiderAnim *rec;

    base = gRiderAnims;
    rec = &base[a0];
    *a1 = rec->flags;
    return rec->body->count;
}

void rider_pose_snapshot(Bone *p, s32 n)
{
    s32 i;
    u16 x, y, z;

    for (i = 0; i < n; i++) {
        mtx3i_to_angles(p[i].rot.m, &x, &y, &z);
        p[i].angles[0] = x;
        p[i].angles[1] = y;
        p[i].angles[2] = z;
    }
    p->baseAngles[0] = (u16)p->pos.x;
    p->baseAngles[1] = (u16)p->pos.y;
    p->baseAngles[2] = (u16)p->pos.z;
}

s32 rider_ik_split_len(s32 a, s32 b, s32 c) {
    s32 t = (a + b) * (a - b);
    s32 d = c << 1;

    return t / d + c / 2;
}


void rider_anims_unload(void) {
    s32 i;

    if (gPodiumMode == 0 && gRiderAnims[0].bodyRes != NULL) {
        i = 0;
        do {
            gRiderAnims[i].body = NULL;
            gRiderAnims[i].root = NULL;
            i++;
        } while (gRiderAnims[i].bodyRes != NULL);
    }
}
