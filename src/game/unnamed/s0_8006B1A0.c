#include "game.h"

void excite3d_init(void)
{
    s32 i;

    sExcite3DSpawnTime = 600;
    gRaceFrame = 0;
    for (i = 6; i < 8; i++) {
        gRiders[i].frozen = 1;
        gRiders[i].isCpu = 1;
        gRiders[i].inactive = 0;
        gRiders[i].bike.pos.x = -1000.0f;
        gRiders[i].bike.pos.y = -50.0f;
        gRiders[i].bike.pos.z = -1000.0f;
        bike_apply_rider_stats(&gRiders[i], 0);
        quat_from_euler(&gRiders[i].bike.pos.x + 12, 0.0f, 0.0f, 0.0f);
    }
    for (i = 0; i < 8; i++) {
        gRiders[i].timeScale = sExcite3DTimeScales[i];
    }
}

typedef struct {
    u8 p0[8];
    f32 f8;
    u8 p1[0x65 - 0xC];
    s8 f65;
    u8 p2[0x6C - 0x66];
} Seg_16;

extern Seg_16 *gPathNodes_30;

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
    Seg_16 *s;
    f32 f;
    f32 d;
    f32 h;
    f32 n;

    i0 = *(s16 *)(p + 0x628);
    i1 = i0;
    side = (s32)((*(f32 *)(p + 0x48) + 1.089f) / 2.178f);
    other = side ^ 1;
    sh1 = side << 1;
    t0 = (gPathNodes_30[i1].f65 >> sh1) & 3;
    if (t0 == 0) {
        for (;;) {
            i1++;
            {
                f32 dd = gPathNodes_30[i1].f8 - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t0 = (gPathNodes_30[i1].f65 >> sh1) & 3;
            if (t0 != 0) break;
        }
    }
    sh2 = other << 1;
    t1 = (gPathNodes_30[i0].f65 >> sh2) & 3;
    if (t1 == 0) {
        for (;;) {
            i0++;
            {
                f32 dd = gPathNodes_30[i0].f8 - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t1 = (gPathNodes_30[i0].f65 >> sh2) & 3;
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
    f32 f0;
    f32 f4;
    f32 f8;
    u8 padC[0x6C - 0xC];
} Seg_17;

extern Seg_17 *gPathNodes_31;

void excite3d_update(void)
{
    Rider *p;
    Rider *a;
    Rider *b;
    Rider *q;
    Seg_17 *u;
    s16 m;
    Rider *e;
    Seg_17 *t;
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

    for (p = gRiders; p < &gRiders[8]; p++) {
        if (p->frozen == 0 && p->isCpu != 0 && p->crashState == 0 && p->airFrames < 3) {
            excite3d_cpu_steer((u8 *)p);
        }
    }

    do {
        sw = 0;
        for (i = 0; i < gEvent.n - 1; i++) {
            a = gRiderStandings[i];
            b = gRiderStandings[i + 1];
            if (a->distToGo < b->distToGo && (a->inactive | b->inactive) == 0) {
                sw = 1;
                gRiderStandings[i] = b;
                gRiderStandings[i + 1] = a;
            }
        }
    } while (sw);

    for (i = 0; i < gEvent.n; i++) {
        gRiderStandings[i]->race.place = i;
    }

    race_announce_position_changes();

    if (gRiders[0].bike.pos.z < -970.375f) {
        gRiders[0].bike.pos.z -= -997.15717f;
        gRiders[0].drawPos.z = gRiders[0].drawPos.z - -997157.2f;
        gRiders[0].drawBodyPos.z = gRiders[0].drawBodyPos.z - -997157.2f;
        gRiders[0].bike.smoothPos.z -= -997.15717f;
        cam_shift_z(0, 0xF3725);
        q = gRiders;
        j = 0;
        while (q->bike.pos.z < gPathNodes_31[j].f8) {
            j++;
        }
        q->segment = j;
        decal_end_track(0);
        event_rider_lap_completed(0);
    }

    for (e = &gRiders[1]; e < &gRiders[8]; e++) {
        if (e->frozen == 0 && e->bike.pos.z < -970.375f) {
            if (e->lapAdjust == 0) {
                if (e - gRiders < 4) {
                    event_rider_lap_completed(e - gRiders);
                }
                e->lapAdjust = 1;
            }
            d = e->bike.pos.z - gRiders[0].bike.pos.z;
            if (d < -70.0f) {
                e->lapAdjust = 0;
                e->bike.pos.z -= -997.15717f;
                e->bike.smoothPos.z -= -997.15717f;
                {
                    s32 jj = 0;
                    t = gPathNodes_31;
                    while (e->bike.pos.z < t[jj].f8) {
                        jj++;
                    }
                    e->segment = jj;
                }
                decal_end_track(e - gRiders);
            }
        }
    }

    cnt = 0;
    for (e = gRiders; e < &gRiders[8]; e++) {
        if (e->isCpu == 0 && e->inactive == 0) {
            cnt++;
            z = e->bike.pos.z;
        }
    }

    if (cnt != 0) {
        k = gRiders[0].segment + 4;
        dist = gRiders[0].bike.pos.z - gPathNodes_31[k].f8;
        h = gRiders[0].bike.pos.y;
        if (!(dist > 100.0f)) goto body;
        h = 4.0f;
        goto found;
        do {
        body:
            v = gPathNodes_31[k].f4;
            if (h < v) {
                h = v;
            }
            if (h - gRiders[0].bike.pos.y > 1.5f && h - v > 1.5f) {
                goto found;
            }
            k = (k + 1) % gPathNodeCount;
            dist = gRiders[0].bike.pos.z - gPathNodes_31[k].f8;
        } while (!(dist > 100.0f));
        h = 4.0f;
    found:
        lo = -10.0f;
        for (p = &gRiders[6]; p < &gRiders[8]; p++) {
            if (!(h < p->bike.pos.y) && gRaceFrame - sExcite3DSpawnTime > 60 && p->isCpu != 0 &&
                gRaceFrame - p->spawnFrame > 179 && p->inactive == 0) {
                d = z - p->bike.pos.z;
                if (d < lo || dist < d) {
                    now = gRaceFrame;
                    sExcite3DSpawnTime = now;
                    if (now & 1) {
                        p->power = 0.2f;
                        p->bike.pos.z = z - dist;
                    } else {
                        p->power = 0.2f;
                        p->bike.pos.z = z - lo;
                    }
                    collide_reset_body((u8 *)&p->bike.pos.x);
                    p->bike.type = 1;
                    p->gridSlot = r250_next() & 3;
                    p->bike.pos.y = 40.0f;
                    p->bike.pos.x = p->gridSlot * 2.178f;
                    p->bike.pos.y = bike_get_ground_height(&p->bike.pos.x);
                    bike_place_on_ground(p);
                    p->speed = 10.0f;
                    p->laneOffset = -p->bike.pos.x;
                    p->bike.vel.y = p->speed * p->bike.fwd.y;
                    p->bike.vel.z = p->speed * p->bike.fwd.z;
                    p->spawnFrame = gRaceFrame;
                    p->bike.vel.x = 0.0f;
                    p->blinkCount = 0;
                    p->noCollide = 0;
                    p->crashPhase = 0;
                    p->jumpState = 0;
                    p->airFrames = 0;
                    p->roughLanding = 0;
                    p->wheelieTimer = 0;
                    p->pendingCrash = 0;
                    p->crashTimer = 0;
                    p->crashState = 0;
                    p->engine.heat = 0.0f;
                    p->engine.overheated = 0;
                    p->frozen = 0;
                    p->anim = -1;
                    p->prevAnim = -1;
                }
            }
        }
    }
}

void excite3d_player_steer(Rider *p)
{
    if (gEventTime <= 0) {
        return;
    }
    if ((s8)p->laneChange != 0) {
        if ((s8)p->laneChange > 0) {
            p->laneChange--;
        } else {
            p->laneChange++;
        }
    } else if (p->airFrames < 0) {
        if (p->stickX < -5) {
            if (p->gridSlot != 0 && excite3d_lane_is_clear_2(p, p->gridSlot - 1)) {
                p->gridSlot--;
                p->laneChange = (s8)-gLaneChangeFrames;
            }
        } else if (p->stickX >= 6 && p->gridSlot < 3 && excite3d_lane_is_clear_2(p, p->gridSlot + 1)) {
            p->gridSlot++;
            p->laneChange = (s8)gLaneChangeFrames;
        }
    }
    p->bike.vel.x = 0.0f;
    p->bike.pos.x = p->gridSlot * 2.178f - (s8)p->laneChange * (2.178f / gLaneChangeFrames);
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

typedef struct Ent_73 {
    u8 p0[8];
    f32 f8;
    u8 rest[0x60];
} Ent_73;

void excite3d_update_progress(Rider *o)
{
    Ent_73 *t = gPathNodes;
    s32 count = 0;
    f32 f;
    if (o->bike.pos.z < t->f8) {
        do {
            count++;
            t++;
        } while (o->bike.pos.z < t->f8);
    }
    f = (f32)o->race.lap * -997.15717f;
    o->segment = count;
    o->lateral = -o->bike.pos.x;
    o->distToGo = -f - o->bike.pos.z;
}

typedef struct {
    u8 pad0[8];
    f32 f8;
    u8 pad1[0x6C - 12];
} Ent_74;

struct Obj_55 {
    u8 pad0[0x50];
    f32 f50;
    u8 pad1[0x628 - 0x54];
    s16 s628;
};

void excite3d_find_segment(Obj_55 *o) {
    s16 c = 0;
    Ent_74 *p = gPathNodes;
    if (o->f50 < p->f8) {
        do {
            p++;
            c++;
        } while (o->f50 < p->f8);
    }
    o->s628 = c;
}
