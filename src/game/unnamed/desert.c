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
    u8 pad0[0x34];
    s16 ang;            /* 0x34 */
    u8 pad36[0x48 - 0x36];
    f32 x;              /* 0x48 */
    f32 y;              /* 0x4C */
    f32 z;              /* 0x50 */
    u8 pad54[0x3FF - 0x54];
    u8 f3FF;            /* 0x3FF */
    u8 pad400[2];
    u8 wp;              /* 0x402 */
    u8 pad403;
    f32 dist;           /* 0x404 */
    u8 pad408[0x44C - 0x408];
    f32 f44C;           /* 0x44C */
    u8 pad450[0x620 - 0x450];
    u8 f620;            /* 0x620 */
    u8 f621;            /* 0x621 */
    u8 pad622[0x630 - 0x622];
    f32 f630;           /* 0x630 */
    u8 pad634[0x662 - 0x634];
    s16 f662;           /* 0x662 */
    u8 pad664[4];
    s16 f668;           /* 0x668 */
    u8 pad66A[0x7A4 - 0x66A];
    u8 f7A4;            /* 0x7A4 */
    u8 f7A5;            /* 0x7A5 */
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_27;

extern Bike_27 gRiders_50[];
extern Bike_27 *gRiderStandings_2[];

void desert_update_race(void)
{
    Bike_27 *b;
    Bike_27 *a;
    Bike_27 *c;
    f32 dx, dz, d, ang, e, f, gx, gz;
    s32 i;
    u8 swapped;
    s32 pad[4];

    d = 0.0f;
    for (b = gRiders_50; b < &gRiders_50[gEvent.numRiders]; b++) {
        b->f44C = 1.0f;
        if (b->f7A5 != 0) {
            continue;
        }
        if (b == gRiders_50) {
            dx = sDesertCheckpoints[b->wp].x - b->x;
            ang = math_atan2f(sDesertCheckpoints[b->wp].z - b->z, -dx);
            des_set_arrow_angle((s32)(ang * 32768.0f / 3.1415927f) - gCamYaw_2 & 0xFFFF);
        }
        {
            gx = b->x - gRiders_50[0].x;
            gz = b->z - gRiders_50[0].z;
            e = __builtin_sqrtf(gx * gx + gz * gz);
        }
        if (b->f7A4 != 0) {
            if (e < 100.0f) {
                b->f7A4 = 0;
                bike_respawn(b);
                b->f620 = 1;
                b->f621 = 1;
            } else {
                dx = sDesertCheckpoints[b->wp].x - b->x;
                dz = sDesertCheckpoints[b->wp].z - b->z;
                d = __builtin_sqrtf(dx * dx + dz * dz);
                ang = math_atan2f(dz, dx);
                b->ang = ang * 32768.0f / 3.1415927f;
                b->x += dx / d * 0.16666667f;
                b->z += dz / d * 0.16666667f;
            }
        } else if (120.0f < e) {
            b->f7A4 = 1;
        }
        dx = b->x - sDesertCheckpoints[b->wp].x;
        dz = b->z - sDesertCheckpoints[b->wp].z;
        f = __builtin_sqrtf(dx * dx + dz * dz);
        b->dist = f;
        if (f < 1.0f) {
            if (++b->wp == 10) {
                b->wp = 0;
                if (b->f3FF == 0) {
                    event_rider_finished(b - gRiders_50);
                    if (b == gRiders_50) {
                        gEvent.allFinished = 1;
                    }
                }
            } else {
                gx = b->x - sDesertCheckpoints[b->wp].x;
                gz = b->z - sDesertCheckpoints[b->wp].z;
                b->dist = gx * gx + gz * gz;
            }
            if (b == gRiders_50) {
                des_set_waypoint(b->wp);
            }
        }
        b->f630 = sDesertCheckpointDist[b->wp] - b->dist;
    }
    if (D_800E3EA8 == 0) {
        do {
            swapped = 0;
            for (i = 0; i < gEvent.numRiders - 1; i++) {
                a = gRiderStandings_2[i];
                if (a->f668 < 0) {
                    c = gRiderStandings_2[i + 1];
                    if (a->wp < c->wp || (a->wp == c->wp && a->dist > c->dist)) {
                        swapped = 1;
                        gRiderStandings_2[i] = c;
                        gRiderStandings_2[i + 1] = a;
                    }
                }
            }
        } while (swapped);
        for (i = 0; i < gEvent.numRiders; i++) {
            gRiderStandings_2[i]->f662 = i;
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
