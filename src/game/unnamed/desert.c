#include "game.h"

void desert_init(void) {
    s32 i;
    f32 sum;
    f32 dx, dz;

    sum = 0.0f;
    sDesertCheckpointDist[0] = sum;
    for (i = 1; i < 10; i++) {
        dx = sDesertCheckpoints[i].x - sDesertCheckpoints[i - 1].x;
        dz = sDesertCheckpoints[i].z - sDesertCheckpoints[i - 1].z;
        sum += __builtin_sqrtf(dx * dx + dz * dz);
        sDesertCheckpointDist[i] = sum;
    }
    des_set_waypoint(0);
    event_reset_places();
}

void desert_export_checkpoints(void) {
    s32 i;
    gDesertFireCount = 10;
    for (i = 0; i < 10; i++) {
        gDesertFires_2[i].x = sDesertCheckpoints[i].x * 1000.0f;
        gDesertFires_2[i].z = sDesertCheckpoints[i].y * 1000.0f;
        gDesertFires_2[i].y = sDesertCheckpoints[i].z * 1000.0f;
    }
}

typedef struct {
    s32 state;
    u8 pad4[0x1C];
    s16 f20;
    s16 n;              /* 0x22 */
    u8 pad24[0x1C];
    s32 f40;            /* 0x40 */
} Game_18;

extern Game_18 gEvent_17;

void desert_update_race(void)
{
    Rider *b;
    Rider *a;
    Rider *c;
    f32 dx, dz, d, ang, e, f, gx, gz;
    s32 i;
    u8 swapped;
    s32 pad[4];

    d = 0.0f;
    for (b = gRiders; b < &gRiders[gEvent_17.n]; b++) {
        b->timeScale = 1.0f;
        if (b->inactive != 0) {
            continue;
        }
        if (b == gRiders) {
            dx = sDesertCheckpoints[b->checkpoint].x - b->bike.pos.x;
            ang = math_atan2f(sDesertCheckpoints[b->checkpoint].z - b->bike.pos.z, -dx);
            des_set_arrow_angle((s32)(ang * 32768.0f / 3.1415927f) - gCamYaw_2 & 0xFFFF);
        }
        {
            gx = b->bike.pos.x - gRiders[0].bike.pos.x;
            gz = b->bike.pos.z - gRiders[0].bike.pos.z;
            e = __builtin_sqrtf(gx * gx + gz * gz);
        }
        if (b->frozen != 0) {
            if (e < 100.0f) {
                b->frozen = 0;
                bike_respawn(b);
                b->blinkCount = 1;
                b->blinkTimer = 1;
            } else {
                dx = sDesertCheckpoints[b->checkpoint].x - b->bike.pos.x;
                dz = sDesertCheckpoints[b->checkpoint].z - b->bike.pos.z;
                d = __builtin_sqrtf(dx * dx + dz * dz);
                ang = math_atan2f(dz, dx);
                b->heading = ang * 32768.0f / 3.1415927f;
                b->bike.pos.x += dx / d * 0.16666667f;
                b->bike.pos.z += dz / d * 0.16666667f;
            }
        } else if (120.0f < e) {
            b->frozen = 1;
        }
        dx = b->bike.pos.x - sDesertCheckpoints[b->checkpoint].x;
        dz = b->bike.pos.z - sDesertCheckpoints[b->checkpoint].z;
        f = __builtin_sqrtf(dx * dx + dz * dz);
        b->checkpointDist = f;
        if (f < 1.0f) {
            if (++b->checkpoint == 10) {
                b->checkpoint = 0;
                if (b->finishState == 0) {
                    event_rider_finished(b - gRiders);
                    if (b == gRiders) {
                        gEvent_17.f40 = 1;
                    }
                }
            } else {
                gx = b->bike.pos.x - sDesertCheckpoints[b->checkpoint].x;
                gz = b->bike.pos.z - sDesertCheckpoints[b->checkpoint].z;
                b->checkpointDist = gx * gx + gz * gz;
            }
            if (b == gRiders) {
                des_set_waypoint(b->checkpoint);
            }
        }
        b->distToGo = sDesertCheckpointDist[b->checkpoint] - b->checkpointDist;
    }
    if (gRiders[0].isCpu == 0) {
        do {
            swapped = 0;
            for (i = 0; i < gEvent_17.n - 1; i++) {
                a = gRiderStandings[i];
                if (a->race.finishPlace < 0) {
                    c = gRiderStandings[i + 1];
                    if (a->checkpoint < c->checkpoint || (a->checkpoint == c->checkpoint && a->checkpointDist > c->checkpointDist)) {
                        swapped = 1;
                        gRiderStandings[i] = c;
                        gRiderStandings[i + 1] = a;
                    }
                }
            }
        } while (swapped);
        for (i = 0; i < gEvent_17.n; i++) {
            gRiderStandings[i]->race.place = i;
        }
        race_announce_position_changes();
    }
}

void desert_ai_steer(u8 *s)
{
    f32 v[3];
    f32 o[3];
    f32 f3, f4, f5, d, sum;
    f32 *tp;
    s8 *cp;
    u32 uVar;

    v[0] = sDesertCheckpoints_2[s[0x402] * 3] - *(f32 *)(s + 0x48);
    tp = &sDesertCheckpoints_2[s[0x402] * 3];
    v[1] = tp[1] - *(f32 *)(s + 0x4C);
    tp = &sDesertCheckpoints_2[s[0x402] * 3];
    v[2] = tp[2] - *(f32 *)(s + 0x50);
    mtx3f_mul_vec3(v, (f32 *)(s + 0xA0), o);
    f3 = math_atan2f(o[2], o[0]);
    sum = o[0] * o[0] + o[1] * o[1] + o[2] * o[2];
    f5 = __builtin_sqrtf(sum);

    cp = (s8 *)(s + 0x5D0);
    *cp = (s8)(s32)(-f3 * 14.0f);
    if (*cp > 14) *cp = 14;
    if (*cp < -14) *cp = -14;

    if (!(0.0f < f3)) f3 = -f3;

    f4 = 5.0f;
    if (!(f5 < 5.0f)) {
        if (f5 < 20.0f) {
            if (f3 < 0.1f) f4 = 20.0f;
        } else {
            f4 = 10.0f;
            if (f3 < 1.0f) f4 = 40.0f;
        }
    }
    if (s[0x3FF] == 1) goto zero;
    if (s[0x3FF] == 2) goto five;
    goto after;
zero:
    f4 = 0.0f;
    goto after;
five:
    f4 = 5.0f;
after:

    d = *(f32 *)(s + 0x420) - f4;
    if (0.5f < d) {
        uVar = 6;
        *(s16 *)(s + 0x55C) = uVar;
        *(s16 *)(s + 0x4B8) = uVar;
        uVar = 0x20;
        goto store;
    }
    if (!(d < -0.5f) || *(s32 *)(s + 0x744) >= 3 || s[0x7A6] != 0) goto done;
    uVar = 0x10;
    if (!(d < -5.0f)) goto store;
    uVar = 0x20000;
    if (*(f32 *)(s + 0x648) < 9500.0f) goto store;
    uVar = 0x10;
store:
    *(u32 *)(s + 0x5B0) = uVar;
done:
    bike_update_steering(s, 1);
}
