#include "game.h"

typedef struct {
    u8 p0[0x48];
    f32 x;
    f32 y;
    f32 z;
    u8 p1[0x78 - 0x54];
    u8 f78[0x44C - 0x78];
    f32 f44C;
    u8 p2[0x6E8 - 0x450];
    u8 f6E8;
    u8 p3[0x7A4 - 0x6E9];
    u8 f7A4;
    u8 f7A5;
    u8 p4[0x7E0 - 0x7A6];
} Ent_72;

extern Ent_72 gRiders_86[];

void excite3d_init(void)
{
    s32 i;

    sExcite3DSpawnTime = 600;
    gRaceFrame = 0;
    for (i = 6; i < 8; i++) {
        gRiders_86[i].f7A4 = 1;
        gRiders_86[i].f6E8 = 1;
        gRiders_86[i].f7A5 = 0;
        gRiders_86[i].x = -1000.0f;
        gRiders_86[i].y = -50.0f;
        gRiders_86[i].z = -1000.0f;
        bike_apply_rider_stats(&gRiders_86[i], 0);
        quat_from_euler(&gRiders_86[i].x + 12, 0.0f, 0.0f, 0.0f);
    }
    for (i = 0; i < 8; i++) {
        gRiders_86[i].f44C = sExcite3DTimeScales[i];
    }
}

void excite3d_cpu_steer(u8 *p)
{
    s32 i0;
    s32 i1;
    s32 side;
    s32 other;
    s32 t0;
    s32 t1;
    s32 sh1;
    s32 sh2;
    PathNode *s;
    f32 f;
    f32 d;
    f32 h;
    f32 n;

    i0 = *(s16 *)(p + 0x628);
    i1 = i0;
    side = (s32)((*(f32 *)(p + 0x48) + 1.089f) / 2.178f);
    other = side ^ 1;
    sh1 = side << 1;
    t0 = (gPathNodes[i1].speedTweak >> sh1) & 3;
    if (t0 == 0) {
        for (;;) {
            i1++;
            {
                f32 dd = gPathNodes[i1].pos.z - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t0 = (gPathNodes[i1].speedTweak >> sh1) & 3;
            if (t0 != 0) break;
        }
    }
    sh2 = other << 1;
    t1 = (gPathNodes[i0].speedTweak >> sh2) & 3;
    if (t1 == 0) {
        for (;;) {
            i0++;
            {
                f32 dd = gPathNodes[i0].pos.z - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t1 = (gPathNodes[i0].speedTweak >> sh2) & 3;
            if (t1 != 0) break;
        }
    }
    if ((t0 == 1 && (t1 != t0 || i1 < i0)) || t1 == 2) {
        side = other;
        if (10.0f < *(f32 *)(p + 0x420)) {
            *(s32 *)(p + 0x5B0) &= 0xFFFDFFEF;
        }
    }
    f = side * 2.178f;
    h = *(f32 *)(p + 0x48);
    d = f - h;
    *(f32 *)(p + 0x42C) = -f;
    if (0.21779999f < d) {
        n = h + *(f32 *)(p + 0x420) * gPhysDeltaTime / 10.0f;
    } else if (d < -0.21779999f) {
        n = h - *(f32 *)(p + 0x420) * gPhysDeltaTime / 10.0f;
    } else {
        goto done;
    }
    *(f32 *)(p + 0x48) = n;
    *(f32 *)(p + 0x42C) = -n;
done:
    if (*(s32 *)(p + 0x744) <= 0) {
        if (t0 == 3) {
            *(s16 *)(p + 0x416) = 0x10;
            *(s16 *)(p + 0x414) = 0;
            return;
        }
        if (*(s16 *)(p + 0x416) != 0) {
            *(s16 *)(p + 0x416) -= 1;
        }
    }
}

typedef struct {
    u8 pad0[0x8];
    s32 f8;             /* 0x8 */
    u8 padC[0x14 - 0xC];
    s32 f14;            /* 0x14 */
    u8 pad18[0x44 - 0x18];
    u8 f44;             /* 0x44 */
    u8 f45;             /* 0x45 */
    u8 pad46[0x48 - 0x46];
    f32 f48;            /* 0x48 */
    f32 f4C;            /* 0x4C */
    f32 f50;            /* 0x50 */
    s32 f54;            /* 0x54 */
    f32 f58;            /* 0x58 */
    f32 f5C;            /* 0x5C */
    u8 pad60[0x74 - 0x60];
    f32 f74;            /* 0x74 */
    u8 pad78[0xBC - 0x78];
    f32 fBC;            /* 0xBC */
    f32 fC0;            /* 0xC0 */
    u8 padC4[0x1C4 - 0xC4];
    u8 f1C4;            /* 0x1C4 */
    u8 pad1C5[0x36C - 0x1C5];
    u8 f36C;            /* 0x36C */
    u8 pad36D[0x3D0 - 0x36D];
    s8 f3D0;            /* 0x3D0 */
    s8 f3D1;            /* 0x3D1 */
    u8 pad3D2[0x416 - 0x3D2];
    s16 f416;           /* 0x416 */
    u8 pad418[0x41D - 0x418];
    u8 f41D;            /* 0x41D */
    u8 pad41E[0x420 - 0x41E];
    f32 f420;           /* 0x420 */
    u8 pad424[0x42C - 0x424];
    f32 f42C;           /* 0x42C */
    u8 pad430[0x5CD - 0x430];
    u8 f5CD;            /* 0x5CD */
    u8 pad5CE[0x620 - 0x5CE];
    u8 f620;            /* 0x620 */
    u8 pad621[0x624 - 0x621];
    s16 f624;           /* 0x624 */
    u8 pad626[0x628 - 0x626];
    s16 f628;           /* 0x628 */
    u8 pad62A[0x630 - 0x62A];
    f32 f630;           /* 0x630 */
    u8 pad634[0x648 - 0x634];
    s32 f648;           /* 0x648 */
    u8 pad64C[0x654 - 0x64C];
    u8 f654;            /* 0x654 */
    u8 pad655[0x662 - 0x655];
    s16 f662;           /* 0x662 */
    u8 pad664[0x6E8 - 0x664];
    u8 f6E8;            /* 0x6E8 */
    u8 pad6E9[0x6ED - 0x6E9];
    s8 f6ED;            /* 0x6ED */
    u8 pad6EE[0x744 - 0x6EE];
    s32 f744;           /* 0x744 */
    s32 f748;           /* 0x748 */
    u8 pad74C[0x7A4 - 0x74C];
    u8 f7A4;            /* 0x7A4 */
    u8 f7A5;            /* 0x7A5 */
    u8 pad7A6[0x7A9 - 0x7A6];
    u8 f7A9;            /* 0x7A9 */
    u8 pad7AA[0x7B0 - 0x7AA];
    s32 f7B0;           /* 0x7B0 */
    u8 pad7B4[0x7D8 - 0x7B4];
    f32 f7D8;           /* 0x7D8 */
    u8 pad7DC[0x7E0 - 0x7DC];
} Bike_40;

extern Bike_40 gRiders_87[8];
extern Bike_40 *gRiderStandings_7[];

void excite3d_update(void)
{
    Bike_40 *p;
    Bike_40 *a;
    Bike_40 *b;
    Bike_40 *q;
    PathNode *u;
    s16 m;
    Bike_40 *e;
    PathNode *t;
    s32 i;
    s32 j;
    s32 k;
    s16 n;
    u8 sw;
    u8 cnt;
    u8 r;
    s32 now;
    f32 z;
    f32 h;
    f32 dist;
    f32 d;
    f32 v;
    f32 lo;

    for (p = gRiders_87; p < &gRiders_87[8]; p++) {
        if (p->f7A4 == 0 && p->f6E8 != 0 && p->f44 == 0 && p->f744 < 3) {
            excite3d_cpu_steer((u8 *)p);
        }
    }

    do {
        sw = 0;
        for (i = 0; i < gEvent.numRiders - 1; i++) {
            a = gRiderStandings_7[i];
            b = gRiderStandings_7[i + 1];
            if (a->f630 < b->f630 && (a->f7A5 | b->f7A5) == 0) {
                sw = 1;
                gRiderStandings_7[i] = b;
                gRiderStandings_7[i + 1] = a;
            }
        }
    } while (sw);

    for (i = 0; i < gEvent.numRiders; i++) {
        gRiderStandings_7[i]->f662 = i;
    }

    race_announce_position_changes();

    if (gRiders_87[0].f50 < -970.375f) {
        gRiders_87[0].f50 -= -997.15717f;
        gRiders_87[0].f8 = gRiders_87[0].f8 - -997157.2f;
        gRiders_87[0].f14 = gRiders_87[0].f14 - -997157.2f;
        gRiders_87[0].f74 -= -997.15717f;
        cam_shift_z(0, 0xF3725);
        q = gRiders_87;
        j = 0;
        while (q->f50 < gPathNodes[j].pos.z) {
            j++;
        }
        q->f628 = j;
        decal_end_track(0);
        event_rider_lap_completed(0);
    }

    for (e = &gRiders_87[1]; e < &gRiders_87[8]; e++) {
        if (e->f7A4 == 0 && e->f50 < -970.375f) {
            if (e->f6ED == 0) {
                if (e - gRiders_87 < 4) {
                    event_rider_lap_completed(e - gRiders_87);
                }
                e->f6ED = 1;
            }
            d = e->f50 - gRiders_87[0].f50;
            if (d < -70.0f) {
                e->f6ED = 0;
                e->f50 -= -997.15717f;
                e->f74 -= -997.15717f;
                {
                    s32 jj = 0;
                    t = gPathNodes;
                    while (e->f50 < t[jj].pos.z) {
                        jj++;
                    }
                    e->f628 = jj;
                }
                decal_end_track(e - gRiders_87);
            }
        }
    }

    cnt = 0;
    for (e = gRiders_87; e < &gRiders_87[8]; e++) {
        if (e->f6E8 == 0 && e->f7A5 == 0) {
            cnt++;
            z = e->f50;
        }
    }

    if (cnt != 0) {
        k = gRiders_87[0].f628 + 4;
        dist = gRiders_87[0].f50 - gPathNodes[k].pos.z;
        h = gRiders_87[0].f4C;
        if (!(dist > 100.0f)) goto body;
        h = 4.0f;
        goto found;
        do {
        body:
            v = gPathNodes[k].pos.y;
            if (h < v) {
                h = v;
            }
            if (h - gRiders_87[0].f4C > 1.5f && h - v > 1.5f) {
                goto found;
            }
            k = (k + 1) % gPathNodeCount;
            dist = gRiders_87[0].f50 - gPathNodes[k].pos.z;
        } while (!(dist > 100.0f));
        h = 4.0f;
    found:
        lo = -10.0f;
        for (p = &gRiders_87[6]; p < &gRiders_87[8]; p++) {
            if (!(h < p->f4C) && gRaceFrame - sExcite3DSpawnTime > 60 && p->f6E8 != 0 &&
                gRaceFrame - p->f7B0 > 179 && p->f7A5 == 0) {
                d = z - p->f50;
                if (d < lo || dist < d) {
                    now = gRaceFrame;
                    sExcite3DSpawnTime = now;
                    if (now & 1) {
                        p->f7D8 = 0.2f;
                        p->f50 = z - dist;
                    } else {
                        p->f7D8 = 0.2f;
                        p->f50 = z - lo;
                    }
                    collide_reset_body((u8 *)&p->f48);
                    p->f1C4 = 1;
                    p->f7A9 = r250_next() & 3;
                    p->f4C = 40.0f;
                    p->f48 = p->f7A9 * 2.178f;
                    p->f4C = bike_get_ground_height(&p->f48);
                    bike_place_on_ground(p);
                    p->f420 = 10.0f;
                    p->f42C = -p->f48;
                    p->f58 = p->f420 * p->fBC;
                    p->f5C = p->f420 * p->fC0;
                    p->f7B0 = gRaceFrame;
                    p->f54 = 0;
                    p->f620 = 0;
                    p->f41D = 0;
                    p->f36C = 0;
                    p->f748 = 0;
                    p->f744 = 0;
                    p->f5CD = 0;
                    p->f416 = 0;
                    p->f45 = 0;
                    p->f624 = 0;
                    p->f44 = 0;
                    p->f648 = 0;
                    p->f654 = 0;
                    p->f7A4 = 0;
                    p->f3D0 = -1;
                    p->f3D1 = -1;
                }
            }
        }
    }
}

void excite3d_player_steer(Obj_52 *p)
{
    if (gEvent.time <= 0) {
        return;
    }
    if ((s8)p->u7aa != 0) {
        if ((s8)p->u7aa > 0) {
            p->u7aa--;
        } else {
            p->u7aa++;
        }
    } else if (p->w744 < 0) {
        if (p->b5d0 < -5) {
            if (p->u7a9 != 0 && excite3d_lane_is_clear_2(p, p->u7a9 - 1)) {
                p->u7a9--;
                p->u7aa = (s8)-gLaneChangeFrames;
            }
        } else if (p->b5d0 >= 6 && p->u7a9 < 3 && excite3d_lane_is_clear_2(p, p->u7a9 + 1)) {
            p->u7a9++;
            p->u7aa = (s8)gLaneChangeFrames;
        }
    }
    p->w54 = 0;
    p->f48 = p->u7a9 * 2.178f - (s8)p->u7aa * (2.178f / gLaneChangeFrames);
}

struct Obj_53 {
    u8 pad0[0x50];
    f32 f50;
    u8 pad54[0x8];
    f32 f5C;
    u8 pad60[0x4E4 - 0x60];
    Ent64 *e;
    u8 pad4E8[0x7A9 - 0x4E8];
    u8 b7A9;
};

s32 excite3d_lane_is_clear(Obj_53 *o, s32 n)
{
    V3 a, b;
    f32 z, bx, r1, r2, sgn, step;
    s32 i;
    u32 fl;

    if (o->e->flags & 0x400) {
        return 0;
    }
    a.x = (f32)n * 2.178f;
    bx = (f32)o->b7A9 * 2.178f;
    z = 0.0f;
    a.y = z;
    b.y = z;
    b.x = bx;
    a.z = o->f50;
    b.z = a.z;
    r1 = bike_get_ground_height(&b);
    r2 = bike_get_ground_height(&a);
    if (r1 != r2) {
ret0:
        return 0;
    }
    sgn = 1.0f;
    if (z < o->f5C) {
        sgn = -1.0f;
    }
    step = o->f5C * gLaneChangeFrames / 60.0f - sgn - sgn;
    for (i = 0; i < 5; i++) {
        b = a;
        b.z += (f32)i * step * 0.25f + sgn;
        bike_get_ground_height(&b);
        if (gSurfaceTable_2[(gGroundSurface >> 4) & 0xF].flags & 0x400) {
            goto ret0;
        }
    }
    fl = gSurfaceTable_2[(gGroundSurface >> 4) & 0xF].flags & 0x400;
    return fl == 0;
}

void excite3d_update_progress(Obj_54 *o)
{
    PathNode *t = gPathNodes;
    s32 count = 0;
    f32 f;
    if (o->f50 < t->pos.z) {
        do {
            count++;
            t++;
        } while (o->f50 < t->pos.z);
    }
    f = (f32)o->s660 * -997.15717f;
    o->s628 = count;
    o->f638 = -o->f48;
    o->f630 = -f - o->f50;
}

struct Obj_55 {
    u8 pad0[0x50];
    f32 f50;
    u8 pad1[0x628 - 0x54];
    s16 s628;
};

void excite3d_find_segment(Obj_55 *o) {
    s16 c = 0;
    PathNode *p = gPathNodes;
    if (o->f50 < p->pos.z) {
        do {
            p++;
            c++;
        } while (o->f50 < p->pos.z);
    }
    o->s628 = c;
}
