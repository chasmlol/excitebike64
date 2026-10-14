#include "game.h"

typedef struct { s32 w[9]; } M9; /* a 36-byte block, for a copy */

/* Bends the arm from bone arm (a bicep) so that the wrist reaches target, pulled back 80 mm along the grip's z axis:
   the elbow from the two bone lengths, the shoulder turned toward the target and twisted by armTwist. */
void rider_ik_arm(Bone *bones, s32 arm, Vec3i *target, Mtx3 *gripRot)
{
    Mtx3 mtx;
    Mtx3 untwist;
    Vec3i wrist;
    Vec3i reach;
    s16 shoulderAng;
    s32 len;
    s16 wristAng, yaw, pitch;
    s32 twist, dist, maxReach, upperLen, foreLen, along, elbowOfs;

    wrist.x = -(gripRot->m[6] * 80) / 16384 + target->x;
    wrist.y = -(gripRot->m[7] * 80) / 16384 + target->y;
    wrist.z = -(gripRot->m[8] * 80) / 16384 + target->z;
    reach.x = wrist.x - bones[arm].worldPos.x;
    reach.y = wrist.y - bones[arm].worldPos.y;
    reach.z = wrist.z - bones[arm].worldPos.z;
    mtx = bones[RIDER_BONE_CHEST].worldRot;
    mtx3i_transpose(&mtx);
    vec3i_mul_mtx3i(&reach, &mtx, &wrist); /* wrist: now the reach in the chest's frame */
    if (arm == RIDER_BONE_L_BICEP) {
        twist = -0x2000;
    } else {
        twist = 0x2000;
    }
    twist = twist * sRiderIkTargets.armTwist / 256;
    mtx3i_from_angles(&mtx, 0, twist, 0);
    vec3i_mul_mtx3i(&wrist, &mtx, &reach);
    dist = isqrt_u32_3(reach.x * reach.x + reach.y * reach.y + reach.z * reach.z);
    maxReach = bones[arm + 1].pos.z + bones[arm + 2].pos.z;
    if (maxReach < dist) {
        reach.x = reach.x * maxReach / dist;
        reach.y = reach.y * maxReach / dist;
        reach.z = reach.z * maxReach / dist;
        dist = maxReach;
    }
    /* along: from the shoulder to the foot of the elbow on the reach line (rider_ik_split_len); elbowOfs: the
       elbow's distance from that line */
    upperLen = bones[arm + 1].pos.z;
    foreLen = bones[arm + 2].pos.z;
    along = (upperLen + foreLen) * (upperLen - foreLen);
    along = along / (dist * 2) + dist / 2;
    {
        s32 alongSq = along * along;
        elbowOfs = isqrt_u32_3(upperLen * upperLen - alongSq);
    }
    math_to_polar(elbowOfs, along, &shoulderAng, &len);
    math_to_polar(elbowOfs, dist - along, &wristAng, &len);
    if (arm == RIDER_BONE_R_BICEP) {
        shoulderAng = -shoulderAng;
        wristAng = -wristAng;
    }
    math_to_polar(reach.x, reach.z, &yaw, &len);
    yaw = -yaw;
    len = isqrt_u32_3(reach.x * reach.x + reach.z * reach.z);
    math_to_polar(reach.y, len, &pitch, &len);
    pitch = -pitch;
    mtx3i_from_angles(&mtx, pitch, 0, yaw + shoulderAng);
    mtx3i_from_angles(&untwist, 0, -twist, 0);
    mtx3i_mul(&mtx, &untwist, &bones[arm].rot);
    mtx3i_from_angles(&bones[arm + 1].rot, 0, 0, -(shoulderAng + wristAng));
}

/* Bends the leg from bone leg (a thigh) so that the ankle reaches target, as rider_ik_arm does, splayed out by
   0x800. footRot is unused. */
void rider_ik_leg(Bone *bones, s32 leg, Vec3i *target, s32 *footRot)
{
    Mtx3 mtx;
    Mtx3 dirMtx;
    Vec3i foot;
    Vec3i reach;
    s16 hipAng;
    s32 len;
    s16 ankleAng, yaw, pitch;
    s32 splay, dist, maxReach, kneeY, thighLen, shinLen, along, kneeOfs;

    foot.x = target->x;
    foot.y = target->y;
    foot.z = target->z;
    reach.x = foot.x - bones[leg].worldPos.x;
    reach.y = foot.y - bones[leg].worldPos.y;
    reach.z = foot.z - bones[leg].worldPos.z;
    mtx = bones->worldRot; /* the hips' */
    mtx3i_transpose(&mtx);
    vec3i_mul_mtx3i(&reach, &mtx, &foot); /* foot: now the reach in the hips' frame */
    if (leg == RIDER_BONE_L_THIGH) {
        splay = -0x800;
    } else {
        splay = 0x800;
    }
    mtx3i_from_angles(&mtx, 0, 0, splay);
    vec3i_mul_mtx3i(&foot, &mtx, &reach);
    dist = isqrt_u32_3(reach.x * reach.x + reach.y * reach.y + reach.z * reach.z);
    maxReach = -(bones[leg + 1].pos.y + bones[leg + 2].pos.y);
    if (maxReach < dist) {
        reach.x = reach.x * maxReach / dist;
        reach.y = reach.y * maxReach / dist;
        reach.z = reach.z * maxReach / dist;
        dist = maxReach;
    }
    /* along and kneeOfs as in rider_ik_arm */
    kneeY = bones[leg + 1].pos.y;
    thighLen = -kneeY;
    shinLen = -bones[leg + 2].pos.y;
    along = (thighLen + shinLen) * (thighLen - shinLen);
    along = along / (dist * 2) + dist / 2;
    {
        s32 alongSq = along * along;
        kneeOfs = isqrt_u32_3(kneeY * kneeY - alongSq);
    }
    math_to_polar(kneeOfs, along, &hipAng, &len);
    math_to_polar(kneeOfs, dist - along, &ankleAng, &len);
    math_to_polar(reach.x, reach.y, &yaw, &len);
    yaw = -(yaw ^ 0x8000);
    len = isqrt_u32_3(reach.x * reach.x + reach.y * reach.y);
    math_to_polar(reach.z, len, &pitch, &len);
    pitch = -pitch;
    mtx3i_from_angles(&mtx, pitch - hipAng, 0, 0);
    mtx3i_from_angles(&dirMtx, 0, yaw, -splay);
    mtx3i_mul(&mtx, &dirMtx, &bones[leg].rot);
    mtx3i_from_angles(&bones[leg + 1].rot, hipAng + ankleAng, 0, 0);
}

/* Places the chest and the shoulder and hip joints, bends the arms and legs to sRiderIkTargets, updates the world
   transforms, then sets the world rotations of the hands, feet and head from the targets. */
void rider_apply_ik(Bone *bones)
{
    s32 *hipsRot;
    s32 *chestRot;
    s32 *headRot;
    volatile s32 pad[10]; /* the original's frame size */

    hipsRot = bones[RIDER_BONE_HIPS].worldRot.m;
    chestRot = bones[RIDER_BONE_CHEST].worldRot.m;
    mtx3i_mul(bones[RIDER_BONE_CHEST].rot.m, hipsRot, chestRot);
    vec3i_mul_mtx3i(&bones[RIDER_BONE_CHEST].pos.x, hipsRot, &bones[RIDER_BONE_CHEST].worldPos.x);
    bones[RIDER_BONE_CHEST].worldPos.x = bones[RIDER_BONE_CHEST].worldPos.x + bones[RIDER_BONE_HIPS].worldPos.x;
    bones[RIDER_BONE_CHEST].worldPos.y = bones[RIDER_BONE_CHEST].worldPos.y + bones[RIDER_BONE_HIPS].worldPos.y;
    bones[RIDER_BONE_CHEST].worldPos.z = bones[RIDER_BONE_CHEST].worldPos.z + bones[RIDER_BONE_HIPS].worldPos.z;
    vec3i_mul_mtx3i(&bones[RIDER_BONE_L_BICEP].pos.x, chestRot, &bones[RIDER_BONE_L_BICEP].worldPos.x);
    bones[RIDER_BONE_L_BICEP].worldPos.x = bones[RIDER_BONE_L_BICEP].worldPos.x + bones[RIDER_BONE_CHEST].worldPos.x;
    bones[RIDER_BONE_L_BICEP].worldPos.y = bones[RIDER_BONE_L_BICEP].worldPos.y + bones[RIDER_BONE_CHEST].worldPos.y;
    bones[RIDER_BONE_L_BICEP].worldPos.z = bones[RIDER_BONE_L_BICEP].worldPos.z + bones[RIDER_BONE_CHEST].worldPos.z;
    vec3i_mul_mtx3i(&bones[RIDER_BONE_R_BICEP].pos.x, chestRot, &bones[RIDER_BONE_R_BICEP].worldPos.x);
    bones[RIDER_BONE_R_BICEP].worldPos.x = bones[RIDER_BONE_R_BICEP].worldPos.x + bones[RIDER_BONE_CHEST].worldPos.x;
    bones[RIDER_BONE_R_BICEP].worldPos.y = bones[RIDER_BONE_R_BICEP].worldPos.y + bones[RIDER_BONE_CHEST].worldPos.y;
    bones[RIDER_BONE_R_BICEP].worldPos.z = bones[RIDER_BONE_R_BICEP].worldPos.z + bones[RIDER_BONE_CHEST].worldPos.z;
    vec3i_mul_mtx3i(&bones[RIDER_BONE_L_THIGH].pos.x, hipsRot, &bones[RIDER_BONE_L_THIGH].worldPos.x);
    bones[RIDER_BONE_L_THIGH].worldPos.x = bones[RIDER_BONE_L_THIGH].worldPos.x + bones[RIDER_BONE_HIPS].worldPos.x;
    bones[RIDER_BONE_L_THIGH].worldPos.y = bones[RIDER_BONE_L_THIGH].worldPos.y + bones[RIDER_BONE_HIPS].worldPos.y;
    bones[RIDER_BONE_L_THIGH].worldPos.z = bones[RIDER_BONE_L_THIGH].worldPos.z + bones[RIDER_BONE_HIPS].worldPos.z;
    vec3i_mul_mtx3i(&bones[RIDER_BONE_R_THIGH].pos.x, hipsRot, &bones[RIDER_BONE_R_THIGH].worldPos.x);
    bones[RIDER_BONE_R_THIGH].worldPos.x = bones[RIDER_BONE_R_THIGH].worldPos.x + bones[RIDER_BONE_HIPS].worldPos.x;
    bones[RIDER_BONE_R_THIGH].worldPos.y = bones[RIDER_BONE_R_THIGH].worldPos.y + bones[RIDER_BONE_HIPS].worldPos.y;
    bones[RIDER_BONE_R_THIGH].worldPos.z = bones[RIDER_BONE_R_THIGH].worldPos.z + bones[RIDER_BONE_HIPS].worldPos.z;

    rider_ik_arm(bones, RIDER_BONE_L_BICEP, &sRiderIkTargets.limb[0].pos, &sRiderIkTargets.limb[0].rot);
    rider_ik_arm(bones, RIDER_BONE_R_BICEP, &sRiderIkTargets.limb[1].pos, &sRiderIkTargets.limb[1].rot);
    rider_ik_leg(bones, RIDER_BONE_L_THIGH, &sRiderIkTargets.limb[2].pos, sRiderIkTargets.limb[2].rot.m);
    rider_ik_leg(bones, RIDER_BONE_R_THIGH, &sRiderIkTargets.limb[3].pos, sRiderIkTargets.limb[3].rot.m);
    skel_update_world_transforms(bones, RIDER_BONE_COUNT);

    /* D_800EA6CC, D_800EA6FC, D_800EA72C, D_800EA750: limb[1..3].rot and head (variables.h) */
    bones[RIDER_BONE_L_HAND].worldRot = sRiderIkTargets.limb[0].rot;
    bones[RIDER_BONE_R_HAND].worldRot = D_800EA6CC;
    bones[RIDER_BONE_L_FOOT].worldRot = D_800EA6FC;
    bones[RIDER_BONE_R_FOOT].worldRot = D_800EA72C;
    headRot = bones[RIDER_BONE_HEAD].worldRot.m;
    *(Mtx3 *)headRot = D_800EA750;
}

/* the rider being posed */
#define RIDER (gRiders[idx])
/* gCosTable: 1.0 = 0x4000, 16-bit angles */
#define COSS(a) gCosTable[((a) >> 4) & 0xFFF]
#define SINS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

/* gRiderGfx entries (0xA80 bytes per rider): the ponytail's part */
typedef struct {
    u8 pad[0xA4C];
    u8 *ponytailMtx;            /* the three bone matrices (0xC0 bytes), for each of the two frame buffers */
    Bone *ponytailBones;        /* NULL: no ponytail */
    u8 padA54[0x2C];
} Slot_7;

extern Slot_7 gRiderGfx_10[];

/* Poses rider idx's skeleton: hips from the lean and the stick, hands and feet on the grips and pegs, the head by
   headYaw, the IK and the fist pump; blends with the animation and steps the ponytail. */
void rider_update_pose(RiderSkeleton *skel, s32 idx)
{
    s32 hipOfs[3];
    Mtx3 mtx;
    s32 hipPos[3];
    s32 blend;
    s32 drop, crouch, pitch, weight, rest, avg, n;
    s32 tmp;
    s32 flags;
    s32 savedFrame;
    s32 lean, yaw;

    if (gPodiumMode != 0) {
        return;
    }
    /* blend: how much of the animation shows (0x100 all), over its first 4 frames (and a loop's last 4) */
    tmp = RIDER.anim;
    blend = 0x100;
    if (tmp >= 0) {
        flags = gRiderAnims[tmp].flags;
        if (flags & ANIM_FLAG_REVERSE) {
            if (RIDER.animFrame < 0x400) blend = RIDER.animFrame / 4;
        } else if (flags & ANIM_FLAG_LOOP) {
            flags = gRiderAnims[tmp].body->count; /* now the frame count */
            if (RIDER.animFrame >= (flags - 4) << 8) {
                blend = ((flags << 8) - RIDER.animFrame) / 4;
            } else if (RIDER.animFrame < 0x400) {
                blend = RIDER.animFrame / 4;
            }
        } else if (flags & 4) {
            if (RIDER.animFrame < 0x400 && RIDER.animHold == 0) blend = RIDER.animFrame / 4;
        } else {
            if (RIDER.animFrame < 0x400) blend = RIDER.animFrame / 4;
        }
    }
    if (RIDER.prevAnim >= ANIM_CRASH_BACK) {
        blend = 0x100;
    }
    if (RIDER.prevAnim != ANIM_NONE && blend != 0x100) {
        /* blending from the previous animation: its current frame is the pose to blend from */
        tmp = RIDER.anim;
        savedFrame = RIDER.animFrame;
        RIDER.anim = RIDER.prevAnim;
        RIDER.animFrame = RIDER.prevAnimFrame;
        rider_anim_apply(skel, idx, 0x100);
        rider_pose_snapshot(skel->bones, RIDER_BONE_COUNT);
        RIDER.anim = tmp;
        RIDER.animFrame = savedFrame;
    } else if (RIDER.anim == ANIM_NONE || blend != 0x100) {
        /* the hips (mm, in the bike's frame): lower when leaning or crouching for a crest, lagging the bike's
           height changes, back or forward with the stick */
        hipOfs[0] = 0;
        drop = (RIDER.drawRot[1] * 250) / 8192;
        if (drop > 0) drop = -drop;
        if (drop < -500) drop = -500;
        crouch = RIDER.crestCrouch * 10;
        if (crouch > 250) crouch = 250;
        avg = RIDER.inHistAvg;
        avg -= RIDER.drawPos.y;
        hipOfs[1] = drop + 900 + crouch + avg;
        if (hipOfs[1] < 800) hipOfs[1] = 800;
        if (hipOfs[1] > 1050) hipOfs[1] = 1050;
        pitch = hipOfs[1] - 900;
        if (pitch < 0) pitch = 0;
        avg = RIDER.stickHist8Avg;
        hipOfs[2] = avg * 6 - 250;
        pitch *= 10;
        if (RIDER.unk_77C != 0) { /* toward a low, forward pose (rider_update_crouch) */
            weight = RIDER.unk_77C;
            if (weight > 256) weight = 256;
            if (weight < 0) weight = 0;
            rest = 256 - weight;
            pitch = (pitch * rest + weight * 6000) >> 8;
            hipOfs[2] = (hipOfs[2] * rest - weight * 520) >> 8;
            hipOfs[1] = (hipOfs[1] * rest + weight * 900) >> 8;
        }
        sRiderIkTargets.armTwist = 0x80 - avg * 16;
        if (sRiderIkTargets.armTwist < 0) sRiderIkTargets.armTwist = 0;
        mtx3i_from_angles(&mtx, RIDER.drawRot[0], (RIDER.drawRot[1] * 7) / 8, RIDER.drawRot[2]);
        vec3i_mul_mtx3i(hipOfs, mtx.m, hipPos);
        skel->bones[RIDER_BONE_HIPS].pos.x = skel->bones[RIDER_BONE_HIPS].worldPos.x = hipPos[0];
        skel->bones[RIDER_BONE_HIPS].pos.y = skel->bones[RIDER_BONE_HIPS].worldPos.y = hipPos[1];
        skel->bones[RIDER_BONE_HIPS].pos.z = skel->bones[RIDER_BONE_HIPS].worldPos.z = hipPos[2];
        mtx3i_from_angles(&skel->bones[RIDER_BONE_HIPS].rot, RIDER.drawRot[0] + 4000, 0, RIDER.drawRot[2]);
        skel->bones[RIDER_BONE_HIPS].worldRot = skel->bones[RIDER_BONE_HIPS].rot;
        lean = avg * 128 - 4000;
        mtx3i_from_angles(&skel->bones[RIDER_BONE_CHEST].rot, sRiderChestPitch[idx] = pitch - lean, 0, 0);
        sRiderIkTargets.limb[0].pos = gBikeSkelTemplate->bones[BIKE_BONE_L_GRIP].worldPos;
        sRiderIkTargets.limb[1].pos = gBikeSkelTemplate->bones[BIKE_BONE_R_GRIP].worldPos;
        sRiderIkTargets.limb[2].pos = gBikeSkelTemplate->bones[BIKE_BONE_L_PEG].worldPos;
        sRiderIkTargets.limb[3].pos = gBikeSkelTemplate->bones[BIKE_BONE_R_PEG].worldPos;

        /* a foot out (out/16 of the way): swinging during a wheelie, else down to the ground beside the bike, and
           stepping when both are out. leftFootOut moves limb[3] (the right foot) and rightFootOut limb[2]. */
        if (RIDER.leftFootOut != 0) {
            s32 x, y, z, step, lift, out;
            f32 spread;

            if (RIDER.wheelieTimer != 0) {
                f32 side = SINS((gRaceFrame << 11) + 0x4000) * 0.00390625f + 128.0f;
                x = RIDER.bike.right.x * side + RIDER.bike.fwd.x * 128.0f;
                y = RIDER.bike.right.y * side + RIDER.bike.fwd.y * 128.0f;
                y += 100;
                z = RIDER.bike.right.z * side + RIDER.bike.fwd.z * 128.0f;
            } else {
                spread = 256.0f;
                if (RIDER.rightFootOut != 0) spread = 64.0f;
                x = RIDER.bike.right.x * spread;
                x -= RIDER.bike.right.z * spread;
                z = -(RIDER.bike.fwd.x * spread);
                z += RIDER.bike.fwd.z * spread;
                y = height_get_ground(RIDER.drawPos.x + sRiderIkTargets.limb[3].pos.x + x, RIDER.drawPos.y, RIDER.drawPos.z + sRiderIkTargets.limb[3].pos.z + z) - RIDER.drawPos.y;
                y -= sRiderIkTargets.limb[3].pos.y;
                y += 100;
                if (RIDER.rightFootOut != 0) {
                    if (RIDER.walkPhase > 0) {
                        step = (0x4000 - RIDER.walkPhase) / 64;
                        lift = 0;
                    } else {
                        step = COSS(RIDER.walkPhase) / 64;
                        lift = -SINS(RIDER.walkPhase) / 256;
                    }
                    {
                        f32 stepF;
                        step -= 200;
                        stepF = step;
                        x += stepF * RIDER.bike.fwd.x;
                        z += stepF * RIDER.bike.fwd.z;
                        y += lift;
                    }
                }
            }
            out = RIDER.leftFootOut;
            x = (x * out) >> 4;
            y = (y * out) >> 4;
            z = (z * out) >> 4;
            sRiderIkTargets.limb[3].pos.x += x;
            sRiderIkTargets.limb[3].pos.y += y;
            sRiderIkTargets.limb[3].pos.z += z;
        }
        if (RIDER.rightFootOut != 0) {
            s32 x, y, z, step, lift, out;
            f32 spread;

            if (RIDER.wheelieTimer != 0) {
                f32 side = -SINS(gRaceFrame << 11) * 0.00390625f - 128.0f;
                x = RIDER.bike.right.x * side + RIDER.bike.fwd.x * 128.0f;
                y = RIDER.bike.right.y * side + RIDER.bike.fwd.y * 128.0f;
                y += 100;
                z = RIDER.bike.right.z * side + RIDER.bike.fwd.z * 128.0f;
            } else {
                spread = 256.0f;
                if (RIDER.leftFootOut != 0) spread = 64.0f;
                x = -(RIDER.bike.right.x * spread);
                z = RIDER.bike.fwd.x * spread;
                x -= RIDER.bike.right.z * spread;
                z += RIDER.bike.fwd.z * spread;
                /* the ground is probed beside limb[3], not limb[2], in the original */
                y = height_get_ground(RIDER.drawPos.x + sRiderIkTargets.limb[3].pos.x + x, RIDER.drawPos.y, RIDER.drawPos.z + sRiderIkTargets.limb[3].pos.z + z) - RIDER.drawPos.y;
                y -= sRiderIkTargets.limb[2].pos.y;
                y += 100;
                if (RIDER.leftFootOut != 0) {
                    if (RIDER.walkPhase < 0) {
                        step = -(RIDER.walkPhase + 0x4000) / 64;
                        lift = 0;
                    } else {
                        step = -COSS(RIDER.walkPhase) / 64;
                        lift = SINS(RIDER.walkPhase) / 256;
                    }
                    {
                        f32 stepF;
                        step -= 200;
                        stepF = step;
                        x += stepF * RIDER.bike.fwd.x;
                        z += stepF * RIDER.bike.fwd.z;
                        y += lift;
                    }
                }
            }
            out = RIDER.rightFootOut;
            x = (x * out) >> 4;
            y = (y * out) >> 4;
            z = (z * out) >> 4;
            sRiderIkTargets.limb[2].pos.x += x;
            sRiderIkTargets.limb[2].pos.y += y;
            sRiderIkTargets.limb[2].pos.z += z;
        }
        /* the hands follow the handlebars (the right one twists the throttle), the feet half the bike's pitch */
        mtx3i_from_angles(sRiderIkTargets.limb[0].rot.m, 0x1000, 0, RIDER.drawRot[2] + RIDER.drawSteer);
        mtx3i_from_angles(sRiderIkTargets.limb[1].rot.m, RIDER.throttleTwist + 0x1000, 0, RIDER.drawRot[2] + RIDER.drawSteer);
        mtx3i_from_angles(sRiderIkTargets.limb[2].rot.m, RIDER.drawRot[0] / 2, 0, RIDER.drawRot[2]);
        mtx3i_from_angles(sRiderIkTargets.limb[3].rot.m, RIDER.drawRot[0] / 2, 0, RIDER.drawRot[2]);
        /* the head: headYaw on an ease curve unless looking back; it dips mid-turn */
        yaw = RIDER.headYaw;
        if (sRiderLookBack[idx] == 0) {
            if (yaw < 0) {
                yaw = COSS(yaw * 2) / 2 - 0x2000;
            } else {
                yaw = -COSS(yaw * 2) / 2 + 0x2000;
            }
        }
        tmp = yaw;
        if (tmp < 0) tmp = -tmp;
        yaw = (yaw * 12000) >> 14;
        mtx3i_from_angles(sRiderIkTargets.head.m, (0x4000 - COSS(tmp * 4)) >> 4, 0, RIDER.drawRot[2] + yaw);
        rider_apply_ik(skel->bones);
        /* the fist pump: the PUMP clip on the left arm, fading in and out over 8 frames (the fade is unused) */
        tmp = RIDER.fistPump;
        if (tmp != 0) {
            s32 pumpBlend;
            struct { u8 pad[4]; s32 count; } *pumpClip = D_800C9E84; /* gRiderAnims[ANIM_PUMP].body */
            if (tmp >= 8) {
                if (pumpClip->count - 8 < tmp) {
                    pumpBlend = (pumpClip->count - tmp) << 5;
                } else {
                    pumpBlend = 0x100;
                }
            } else {
                pumpBlend = tmp << 5;
            }
            rider_anim_overlay_limb(skel, ANIM_PUMP, RIDER_BONE_L_BICEP, tmp, pumpBlend);
        }
        if (RIDER.anim >= 0) {
            rider_pose_snapshot(skel->bones, RIDER_BONE_COUNT);
        }
    }
    if (RIDER.anim >= 0) {
        rider_anim_apply(skel, idx, blend);
    }
    if (gRiderGfx_10[idx].ponytailBones != 0) {
        ponytail_update(idx, skel, gRiderGfx_10[idx].ponytailBones, gRiderGfx_10[idx].ponytailMtx + gGfxFrameIndex * 0xC0);
    }
}

/* Finds the nodes of the rider's bones in set (halts if one is missing), clears the riders' animFrame and loads the
   clips of every animation (not the tricks' at the rider select). */
void rider_anims_load(u8 *set)
{
    s32 i;
    s32 node;
    RiderAnim *p;
    s32 noFrame;
    AnimClip *r;

    if (gPodiumMode == 0) {
        for (i = 0; i < RIDER_BONE_COUNT; i++) {
            node = res_find_gid(set, sRiderBoneNames[i]);
            sRiderBoneNodes[i] = node;
            if (node < 0) {
                fatal_halt(1);
            }
        }
        noFrame = -1;
        for (i = RIDER_COUNT - 1; i >= 0; i--) {
            gRiders[i].animFrame = noFrame;
        }
        for (i = 0; gRiderAnims[i].bodyRes != 0; i++) {
            if (i >= ANIM_TRICK_COUNT || !(gGame.flags & GAME_FLAG_RIDER_SELECT)) {
                gRiderAnims[i].body = res_load(set, *gRiderAnims[i].bodyRes);
                gRiderAnims[i].body->data = gRiderAnims[i].body->frames;
                if (gRiderAnims[i].rootRes != 0) {
                    gRiderAnims[i].root = res_load(set, *gRiderAnims[i].rootRes);
                    gRiderAnims[i].root->data = gRiderAnims[i].root->frames;
                }
            }
        }
    }
}

/* lerpRot written out: the root bone's blend compiles differently from the inline helper */
#define LERPROT(b, x, y, z, t)                                      \
    {                                                               \
        u16 cx = (b)->angles[0];                                    \
        u16 cy;                                                     \
        u16 cz;                                                     \
        cx += (s16)((x) - cx) * (t) / 256;                          \
        cy = (b)->angles[1];                                        \
        cy += (s16)((y) - cy) * (t) / 256;                          \
        cz = (b)->angles[2];                                        \
        cz += (s16)((z) - cz) * (t) / 256;                          \
        mtx3i_from_angles((b)->rot.m, (s16)cx, (s16)cy, (s16)cz);   \
    }

/* Turns b from its saved angles toward (x, y, z) by t / 256 (the shorter way). */
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

/* Frame frame of a clip's data, size halfwords per frame. */
static inline s16 *getFrame(s16 *data, s32 frame, s32 size)
{
    return data + frame * size;
}

/* Poses the bones from the current frame of rider idx's animation, blend / 0x100 of the way from the saved pose,
   turned with the bike (or the thrown body). */
void rider_anim_apply(Bone *bones, s32 idx, s32 blend)
{
    s32 rootMtx[10];
    s32 mtx[10];
    s32 pos[4];
    s32 rel[3];
    s32 hipOfs[3];
    s32 hipsMtx[14];
    u16 ax;
    u16 ay;
    u16 az;
    AnimClip *rootClip;
    RiderAnim *anim;
    AnimClip *bodyClip;
    s32 frame;
    s16 *bodyFrame;
    s16 *rootFrame;
    s8 *rots;
    s8 *rootRots;
    s32 i;
    s32 w;
    Rider *base;
    s32 off;

    {
        s8 animIdx = gRiders[idx].anim;
        RiderAnim *anims = gRiderAnims;
        anim = &anims[animIdx];
    }
    bodyClip = anim->body;
    rootClip = anim->root;
    frame = gRiders[idx].animFrame >> 8;
    if ((anim->flags & ANIM_FLAG_HOLD) != 0 && !(frame < bodyClip->count)) { /* held: cycle the last 5 frames */
        frame -= 5;
        frame += (s16)(gRiders[idx].holdTime % 5);
    }
    if (frame < 0) {
        frame = 0;
    }
    if (frame >= bodyClip->count) {
        frame = bodyClip->count - 1;
    }
    /* a body frame: the hips' position (3 halfwords), then each bone's angles (3 bytes, >> 8) */
    bodyFrame = getFrame(bodyClip->data, frame, 26);
    rots = (s8 *)bodyFrame + 6;

    if (blend == 0x100) {
        for (i = 1; i < RIDER_BONE_COUNT; i++) {
            mtx3i_from_angles(bones[i].rot.m, rots[i * 3] << 8, rots[i * 3 + 1] << 8, rots[i * 3 + 2] << 8);
        }
    } else {
        w = blend;
        for (i = 1; i < RIDER_BONE_COUNT; i++) {
            lerpRot(&bones[i], rots[i * 3] << 8, rots[i * 3 + 1] << 8, rots[i * 3 + 2] << 8, w);
        }
    }

    if (rootClip != 0) {
        /* undo the bike's offset and rotation from the root clip */
        rootFrame = getFrame(rootClip->data, frame, 6);
        pos[0] = bodyFrame[0] - rootFrame[0];
        pos[1] = bodyFrame[1] - rootFrame[1];
        pos[2] = bodyFrame[2] - rootFrame[2];
        rootRots = (s8 *)rootFrame + 6;
        mtx3i_from_angles(rootMtx, rootRots[0] << 8, rootRots[1] << 8, rootRots[2] << 8);
        mtx3i_transpose(rootMtx);
        vec3i_mul_mtx3i(pos, rootMtx, rel);
        hipOfs[0] = rel[0];
        hipOfs[1] = rel[1];
        hipOfs[2] = rel[2];
        mtx3i_from_angles(mtx, rots[0] << 8, rots[1] << 8, rots[2] << 8);
        mtx3i_mul(mtx, rootMtx, hipsMtx);
    } else {
        mtx3i_from_angles(hipsMtx, rots[0] << 8, rots[1] << 8, rots[2] << 8);
        hipOfs[0] = bodyFrame[0];
        hipOfs[1] = bodyFrame[1];
        hipOfs[2] = bodyFrame[2];
    }

    /* gRiders[idx] through a byte offset: the original's addressing */
    base = gRiders;
    off = idx * sizeof(Rider);
    if (((Rider *)((u8 *)base + off))->crashState == CRASH_STATE_RIDING) {
        mtx3i_from_angles(rootMtx, gRiders[idx].drawRot[0], gRiders[idx].drawRot[1], gRiders[idx].drawRot[2]);
        vec3i_mul_mtx3i(hipOfs, rootMtx, pos);
        mtx3i_mul(hipsMtx, rootMtx, &bones->rot.m);
    } else {
        /* dead stores: they keep the original's register allocation */
        if (blend == 0) {
            w = 1;
            i = 2;
        } else {
            w = 3;
            i = 4;
        }
        quat_to_mtx3i(mtx, ((Rider *)((u8 *)base + off))->body.quat);
        mtx3i_mul(hipsMtx, mtx, &bones->rot.m);
        vec3i_mul_mtx3i(hipOfs, mtx, pos);
    }

    /* the root: blend its position and rotation from the saved pose too */
    if (blend != 0x100 && gRiders[idx].crashPhase != 1) {
        for (i = 0; i < 3; i++) {
            (&bones->pos.x)[i] = (pos[i] * blend + bones->savedPos[i] * (0x100 - blend)) / 256;
        }
        mtx3i_to_angles(&bones->rot.m, &ax, &ay, &az);
        w = blend;
        LERPROT(bones, ax, ay, az, w);
    } else {
        bones->pos.x = pos[0];
        bones->pos.y = pos[1];
        bones->pos.z = pos[2];
    }

    if (gRiders[idx].crashState != CRASH_STATE_RIDING) {
        bones->pos.x = 0;
        bones->pos.y = 0;
        bones->pos.z = 0;
    }
    bones->worldRot = bones->rot;
    bones->worldPos = bones->pos;
    skel_update_world_transforms(bones, RIDER_BONE_COUNT);
}

/* Sets the bike's trick rotation (trickQuat) and offset (trickOffset, m) from the current frame of the animation's
   root clip, with trickBlend rising to 1 over its first 4 frames. */
void bike_update_trick_offset(Rider *rider)
{
    s8 idx;
    u8 *ent;
    AnimClip *clip;
    s16 *frame;
    s8 *rots;
    s32 count;
    s32 frameIdx;
    s32 animFrame;
    s32 stride;
    f32 blend;

    if (gPodiumMode != 0) {
        return;
    }
    rider->trickBlend = 0.0f;
    if (rider->anim == ANIM_NONE) {
        return;
    }
    clip = gRiderAnims[rider->anim].root;
    if (clip == 0) {
        return;
    }
    animFrame = rider->animFrame;
    frameIdx = animFrame >> 8;
    if (frameIdx < 0) {
        frameIdx = 0;
    }
    count = clip->count;
    if (frameIdx >= count) {
        frameIdx = count - 1;
    }
    /* a root frame: the offset (3 halfwords, mm) and the angles (3 bytes, pi / 128) */
    stride = 6;
    frame = (s16 *)clip->data + frameIdx * stride;
    blend = (f32)animFrame * 0.0009765625f;
    rots = (s8 *)((u8 *)frame + 6);
    rider->trickBlend = blend;
    if (1.0f < blend) {
        rider->trickBlend = 1.0f;
    }
    quat_from_euler(rider->trickQuat,
                  (f32)(*(s8 *)((u8 *)frame + 6)) * 3.1415925f * 0.0078125f,
                  (f32)(rots[1]) * 3.1415925f * 0.0078125f,
                  (f32)(rots[2]) * 3.1415925f * 0.0078125f);
    rider->trickOffset.x = (f32)(frame[0]) / 1000.0f;
    rider->trickOffset.y = (f32)(frame[1]) / 1000.0f;
    rider->trickOffset.z = (f32)(frame[2]) / 1000.0f;
}

/* Poses the bike's skeleton for rider idx (body from drawQuat, front suspension, swingarm, steering, wheel spin, the
   handlebars from a root clip) and copies it to the rider's bikeSkel. */
void bike_skel_update(BikeSkeleton *skel, s32 idx)
{
    Rider *rider;
    Rider *riders;
    s32 suspFront;
    s32 suspRear;
    s32 nn;
    s32 frame;
    s32 i;
    AnimClip *clip;
    s8 *rots;
    s8 *data;
    s32 stride;
    Quad *src;
    Quad *dst;
    Quad *end;
    s32 steerMtx[10];
    s32 rakeMtx[18];

    if (gPodiumMode == 0) {
        riders = gRiders;
        rider = &riders[idx];
        suspFront = rider->drawSuspFront;
        suspRear = rider->drawSuspRear;
        quat_to_mtx3i(skel->bones[BIKE_BONE_FRAME].worldRot.m, gRiders[idx].drawQuat);
        suspFront = suspFront - 50;
        if (suspFront > 200) {
            suspFront = 200;
        }
        if (suspFront < -60) {
            suspFront = -60;
        }
        skel->bones[BIKE_BONE_FRONT_SUSP].pos.y = suspFront - 546;
        skel->bones[BIKE_BONE_FRONT_SUSP].pos.z = 262 - (suspFront * 2) / 5;
        mtx3i_from_angles(skel->bones[BIKE_BONE_SWINGARM].rot.m, suspRear * 24, 0, 0);
        /* steer about the fork's axis, raked by 4000 */
        mtx3i_from_angles(steerMtx, 4000, 0, rider->drawSteer);
        mtx3i_from_angles(rakeMtx, -4000, 0, 0);
        mtx3i_mul(steerMtx, rakeMtx, skel->bones[BIKE_BONE_STEER].rot.m);
        mtx3i_from_angles(skel->bones[BIKE_BONE_FRONT_WHEEL].rot.m, (s16)rider->frontSpinAngle, 0, 0);
        mtx3i_from_angles(skel->bones[BIKE_BONE_REAR_WHEEL].rot.m, (s16)rider->rearSpinAngle, 0, 0);
        if (rider->anim != ANIM_NONE) {
            clip = gRiderAnims[rider->anim].root;
            if (clip != 0) {
                frame = rider->animFrame >> 8;
                if (frame < 0) {
                    frame = 0;
                }
                if (frame >= clip->count) {
                    frame = clip->count - 1;
                }
                /* the root clip's second set of angles turns the handlebars */
                data = clip->data;
                stride = 6;
                for (i = 1; i < 2; i++) {
                    rots = data + frame * stride * 2 + stride + i * 3;
                    mtx3i_from_angles(skel->bones[i].rot.m, rots[0] << 8, rots[1] << 8, rots[2] << 8);
                }
            }
        }
        skel_update_world_transforms(skel->bones, BIKE_BONE_COUNT);
        /* *gRiders[idx].bikeSkel = *skel, 16 bytes at a time */
        dst = (Quad *)gRiders[idx].bikeSkel;
        src = (Quad *)skel;
        end = src + 0x50;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end);
    }
}

/* Turns each rider's head (headYaw, 0x300 a frame) toward the nearest rider within 10 m while headTimer allows,
   or back when stopped with both feet down and B held, or fist-pumping; straight ahead when crashed. */
void rider_update_head_look(void)
{
    s32 i, other, bestDist, nearest, dx, dz, dist;
    u16 ang, nearestAng;
    s16 target;

    if (gDebugFreeze != 0) return;
    for (i = 0; i < RIDER_COUNT; i++) {
        bestDist = 625; /* 10 m, in the 16 mm units below */
        nearest = -1;
        if (gRiders[i].crashState) {
            gRiders[i].headYaw = 0;
            if (i) dx = 1; else dx = 2; /* dead stores: they keep the original's register allocation */
            continue;
        }
        if (((gPlayerInputs[i] & INPUT_B) && gRiders[i].rightFootOut && gRiders[i].leftFootOut) || gRiders[i].fistPump) {
            sRiderLookBack[i] = 1;
            target = 24000;
        } else {
            sRiderLookBack[i] = 0;
            for (other = 0; other < RIDER_COUNT; other++) {
                if (other == i) continue;
                dx = (gRiders[other].drawPos.x >> 4) - (gRiders[i].drawPos.x >> 4);
                dz = (gRiders[other].drawPos.z >> 4) - (gRiders[i].drawPos.z >> 4);
                /* halve both until they fit math_to_polar */
                while (dx > 0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dx < -0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dz > 0xFFFF) { dz >>= 1; dx >>= 1; }
                while (dz < -0xFFFF) { dz >>= 1; dx >>= 1; }
                math_to_polar(dx, dz, &ang, &dist);
                if (dist < bestDist) {
                    bestDist = dist;
                    nearest = other;
                    nearestAng = ang;
                }
            }
            target = 0;
            if (nearest != -1 && gRiders[i].headTimer <= 0) target = -nearestAng - (u16)gRiders[i].drawRot[2];
            if (target < -0x4000) target = -0x4000;
            if (target > 0x4000) target = 0x4000;
        }
        if (gRiders[i].headYaw < target) gRiders[i].headYaw += 0x300;
        if (gRiders[i].headYaw > target) gRiders[i].headYaw -= 0x300;
        /* look for 120 frames, then wait 150 to 449 */
        if (--gRiders[i].headTimer < -120) gRiders[i].headTimer = (s32)(random_byte() * 300) / 256 + 150;
    }
}

/* Pushes the stick (ticks times: y capped at 15 into stickHist8, x into stickHist16) and the bike's height (inHist)
   into the rider's histories, and averages stickHist8 and inHist. */
void rider_update_input_history(Rider *rider, s32 ticks)
{
    s32 stickY;
    s32 stickX;
    s32 i;
    s32 sum;
    s32 sum2;

    stickY = rider->stickY;
    stickX = rider->stickX;
    if (stickY > 15) {
        stickY = 15;
    }
    for (i = 0; i < ticks; i++) {
        rider->stickHist8[rider->stickHist8Idx] = stickY;
        rider->stickHist8Idx = (rider->stickHist8Idx + 1) % 8;
        rider->stickHist16[rider->stickHist16Idx] = stickX;
        rider->stickHist16Idx = (rider->stickHist16Idx + 1) & 0xF;
    }
    rider->inHist[rider->inHistIdx] = rider->drawPos.y;
    rider->inHistIdx = (rider->inHistIdx + 1) % 8;
    sum = 0;
    for (i = 0; i < 8; i++) {
        sum += rider->stickHist8[i];
    }
    rider->stickHist8Avg = sum / 8;
    sum2 = 0;
    for (i = 0; i < 8; i++) {
        sum2 += rider->inHist[i];
    }
    rider->inHistAvg = sum2 / 8;
}

/* For each rider: crestCrouch grows to 30 while a crest is ahead and shrinks by 2 otherwise; unk_77C ramps up to
   0x100 for 60 frames after the ground had flag 0x2000 (unk_778: the frame it last had it), else back to 0. */
void rider_update_crouch(void)
{
    s32 i;
    u32 frame;
    s32 blend;

    i = 0;
    frame = gRaceFrame;
    do {
        if (gRiders[i].crestAhead > 0) {
            if ((s16)gRiders[i].crestCrouch < 30) {
                gRiders[i].crestCrouch = (u16)gRiders[i].crestCrouch + 1;
            }
        } else {
            if ((s16)gRiders[i].crestCrouch > 0) {
                gRiders[i].crestCrouch = (u16)gRiders[i].crestCrouch - 2;
            }
        }
        if (gRiderGroundSurface[i] & 0x2000) {
            gRiders[i].unk_778 = frame;
        }
        if ((s32)(frame - (u32)gRiders[i].unk_778) < 60) {
            blend = (u16)gRiders[i].unk_77C + 0x10;
            gRiders[i].unk_77C = blend;
            if ((s16)blend > 0x100) {
                gRiders[i].unk_77C = 0x100;
            }
        } else {
            blend = (u16)gRiders[i].unk_77C - 0x10;
            gRiders[i].unk_77C = blend;
            if ((s16)blend < 0) {
                gRiders[i].unk_77C = 0;
            }
        }
        i++;
    } while (i < RIDER_COUNT);
}

/* Advances each rider's fist pump every other frame, and ends it after the PUMP clip's last frame. */
void rider_update_fist_pump(void)
{
    s32 i = 0;

    do {
        if (gRiders[i].fistPump == 0) {
            i++;
            continue;
        }
        if ((gRaceFrame & 1) != 0) {
            u8 frame = gRiders[i].fistPump + 1;
            gRiders[i].fistPump = frame;
            if ((s8)frame >= gRiderAnims[ANIM_PUMP].body->count) {
                gRiders[i].fistPump = 0;
            }
        }
        i++;
    } while (i < RIDER_COUNT);
}

typedef struct { s8 v[3]; } B3;

/* Poses the three bones of a limb (from its first bone, limb) with frame frame of animation anim, over the current
   pose, and updates their world transforms. Saves the pose for blending first. blend is unused. */
void rider_anim_overlay_limb(Bone *bones, s32 anim, s32 limb, s32 frame, s32 blend)
{
    AnimClip *clip;
    s32 halves;
    B3 *rots;
    s32 k;
    s32 sc;
    s32 j; s32 t;
    u16 ax, ay, az;
    Vec3i limbPos;

    clip = gRiderAnims[anim].body;
    halves = (clip->bones * 3 + 7) / 2;
    if (frame < 0) frame = 0;
    if (clip->count - 1 < frame) frame = clip->count - 1;
    rots = (B3 *)((u8 *)clip->data + halves * frame * 2 + 6);

    for (k = 0; k < RIDER_BONE_COUNT; k++) {
        mtx3i_to_angles(bones[k].rot.m, &ax, &ay, &az);
        bones[k].angles[0] = ax;
        bones[k].angles[1] = ay;
        bones[k].angles[2] = az;
    }
    k = 0x100;
    bones[0].savedPos[0] = (u16)bones[0].pos.x;
    bones[0].savedPos[1] = (u16)bones[0].pos.y;
    bones[0].savedPos[2] = (u16)bones[0].pos.z;
    limbPos = bones[limb].pos;
    for (j = 0; j < 3; j++) {
        /* the blend toward the clip's angles is computed and dropped: the clip's angles are used as they are (the dead
           stores keep the original's register allocation) */
        u16 angX = bones[limb + j].angles[0];
        s8 rx = rots[limb + j].v[0];
        s8 ry;
        s8 rz;
        t = (s16)((rx << 8) - angX) * k; if (t) frame = 1; else frame = 2;
        ry = rots[limb + j].v[1];
        t = (s16)((ry << 8) - bones[limb + j].angles[1]) * k; if (t) frame = 3; else frame = 4;
        rz = rots[limb + j].v[2];
        t = (s16)((rz << 8) - bones[limb + j].angles[2]) * k; if (t) frame = 5; else frame = 6;
        mtx3i_from_angles(bones[limb + j].rot.m, (s16)(rx << 8), (s16)(ry << 8), (s16)(rz << 8));
    }
    /* update the limb as a skeleton of its own, rooted at its world transform */
    mtx3i_mul(bones[limb].rot.m, bones[RIDER_BONE_CHEST].worldRot.m, bones[limb].worldRot.m);
    bones[limb].rot = bones[limb].worldRot;
    *(M9 *)&bones[limb].pos = *(M9 *)&bones[limb].worldPos; /* pos = worldPos, and rot's first rows again */
    bones[limb + 1].parent = 0;
    bones[limb + 2].parent = 1;
    skel_update_world_transforms(&bones[limb], 3);
    bones[limb + 1].parent = limb;
    bones[limb + 2].parent = limb + 1;
    bones[limb].pos = limbPos;
}

/* Returns the frame count of animation anim, and its flags in *flags. */
s32 rider_anim_get_length(s32 anim, s32 *flags)
{
    RiderAnim *anims;
    RiderAnim *entry;

    anims = gRiderAnims;
    entry = &anims[anim];
    *flags = entry->flags;
    return entry->body->count;
}

/* Saves the pose of the bones (angles, and the root's position) for rider_anim_apply to blend from. */
void rider_pose_snapshot(Bone *bones, s32 count)
{
    s32 i;
    u16 ax, ay, az;

    for (i = 0; i < count; i++) {
        mtx3i_to_angles(bones[i].rot.m, &ax, &ay, &az);
        bones[i].angles[0] = ax;
        bones[i].angles[1] = ay;
        bones[i].angles[2] = az;
    }
    bones->savedPos[0] = (u16)bones->pos.x;
    bones->savedPos[1] = (u16)bones->pos.y;
    bones->savedPos[2] = (u16)bones->pos.z;
}

/* For a triangle with sides sideA and sideB on a base: how far along the base, from sideA's end, the foot of the
   apex lies (rider_ik_arm and rider_ik_leg have it inline). */
s32 rider_ik_split_len(s32 sideA, s32 sideB, s32 base)
{
    s32 diff = (sideA + sideB) * (sideA - sideB);
    s32 twice = base << 1;

    return diff / twice + base / 2;
}

/* Forgets the clips of every animation. */
void rider_anims_unload(void)
{
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
