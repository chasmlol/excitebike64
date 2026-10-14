#include "game.h"

typedef struct {
    u8 pad0[0x62C];
    f32 f62C;
    f32 f630;
    u8 pad1[0x662 - 0x634];
    s16 h662;
    u8 pad2[0x668 - 0x664];
    s16 h668;
    u8 pad3[0x7A5 - 0x66A];
    u8 b7A5;
} Bike_39;

struct Ent_65 {
    u8 pad0[0x630];
    f32 f630;
    u8 pad634[0x6E8 - 0x634];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
};

typedef struct {
    s16 idx;
    Ent_65 *e;
    f32 d;
} Rec_35;

typedef struct {
    s32 w0;
    s32 w1;
    f32 f2;
} Rec_36;

typedef struct {
    u8 b[6];
} Blob6;

extern Bike_39 *sRidersByLapPos[];
extern Rec_35 sRiderQueueBehind[];
extern Rec_36 sRiderQueueAhead[];
extern Blob6 sGridOrderSaved;
extern Blob6 gGridOrder;

struct Spawn {
    s16 _0;
    s16 track;
    s32 x;
    s32 z;
    s16 scale;
    u16 spd;
};

typedef struct {
    u8 pad0[0x44];
    u8 b44;
    u8 b45;
    u8 pad46[2];
    Vec3f pos;          /* 0x48 */
    f32 sx, sy, sz;     /* 0x54 */
    f32 v60, v64, v68;  /* 0x60 */
    u8 pad6C[0xC];
    Quat quat;          /* 0x78 */
    f32 f88, f8C, f90, f94; /* 0x88 */
    u8 pad98[8];
    f32 mtx[6];         /* 0xA0 */
    f32 mx, my, mz;     /* 0xB8 */
    u8 padC4[4];
    Vec3f hist[20];     /* 0xC8 */
    f32 f1B8;           /* 0x1B8 */
    u8 pad1BC[8];
    u8 b1C4;
    u8 pad1C5[0x36C - 0x1C5];
    u8 b36C;
    u8 pad36D[3];
    Quat quat2;         /* 0x370 */
    u8 pad380[0x3D0 - 0x380];
    s8 b3D0;
    s8 b3D1;
    u8 pad3D2[0x416 - 0x3D2];
    s16 h416;
    u8 pad418[0x42C - 0x418];
    f32 f42C;
    f32 f430, f434, f438;
    u8 pad43C[0x44C - 0x43C];
    f32 f44C;
    u8 pad450[8];
    s32 w458;
    u8 pad45C[0x478 - 0x45C];
    f32 f478;
    u8 pad47C[0x4FC - 0x47C];
    s32 w4FC;
    u8 pad500[0x51C - 0x500];
    f32 f51C;
    u8 pad520[0x5CD - 0x520];
    u8 b5CD;
    u8 pad5CE[0x624 - 0x5CE];
    s16 h624;
    u8 pad626[2];
    s16 h628;
    u8 pad62A[0x63C - 0x62A];
    f32 f63C;
    u8 pad640[8];
    s32 w648;
    u8 pad64C[8];
    u8 b654;
    u8 pad655[0x6E8 - 0x655];
    u8 b6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 w744;
    s32 w748;
    u8 pad74C[0x7A5 - 0x74C];
    u8 b7A5;
    u8 pad7A6[0x7B0 - 0x7A6];
    s32 w7B0;
    u8 pad7B4[0x7E0 - 0x7B4];
} Bike_38;

extern Bike_38 gRiders_79[8];

void race_spawn_extra_rider(Spawn *sp)
{
    Bike_38 *b;
    s32 track;
    s32 i, idx;
    f32 f;
    s32 spd;
    f32 y, k;
    Vec3f unused;

    if (D_800F1BC4 & 0x10) return;
    if (gEvent.flags & 0x200) return;

    b = &gRiders_79[6];
    track = sp->track;
    sp->track = -1;
    for (; b < &gRiders_79[8]; b++) {
        if (b->b7A5 != 0) break;
    }
    if (b == &gRiders_79[8]) return;

    b->b3D1 = -1;
    b->b7A5 = 0;
    b->b36C = 0;
    b->b5CD = 0;
    b->b45 = 0;
    b->b44 = 0;
    b->b654 = 0;
    b->w748 = 0;
    b->w744 = 0;
    b->h416 = 0;
    b->h624 = 0;
    b->w648 = 0;
    b->w4FC = 0;
    b->w458 = 0;
    b->w7B0 = gRaceFrame;
    b->pos.x = (f32)sp->x / 1000.0f;
    b->pos.z = (f32)sp->z / 1000.0f;
    b->pos.y = 0.0f;
    b->pos.y = bike_get_ground_height(&b->pos);
    for (i = 0; i < 20; i++) {
        b->hist[i] = b->pos;
    }
    b->h628 = path_find_segment(&b->pos, track, 15, 0);
    b->f42C = path_get_lateral_offset(&b->pos, b->h628);
    b->f63C = gPathSegmentT;
    f = path_get_lateral_offset(&b->pos, b->h628);
    b->f42C = f;
    sExtraRiderLanes[b - &gRiders_79[6]] = f;
    quat_from_euler(&b->quat, 0.0f, 0.0f, -math_atan2f(gPathTangentZ, gPathTangentX));
    bike_place_on_ground(b);
    quat_to_mtx3f(b->mtx, &b->quat);
    b->sx = b->mx * ((f32)sp->scale / 1000.0f);
    b->sy = b->my * ((f32)sp->scale / 1000.0f);
    b->sz = b->mz * ((f32)sp->scale / 1000.0f);
    b->f51C = 0.0f;
    b->f478 = 0.0f;
    b->f90 = 0.0f;
    b->f8C = 0.0f;
    b->f88 = 0.0f;
    b->f94 = 0.0f;
    b->v68 = 0.0f;
    b->v64 = 0.0f;
    b->v60 = 0.0f;
    b->f430 = 0.0f;
    b->f434 = 0.0f;
    b->f438 = 0.0f;
    b->f1B8 = 1.0f;
    b->b3D0 = -1;
    b->quat2 = b->quat;
    b->pos.y = bike_get_ground_height(&b->pos) + 0.0f + 0.5f
        - (gGame.mode == 6 ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    b->w744 = 4;
    bike_update_wheel_contacts(b);
    bike_update_wheel_contacts(b);
    bike_update_suspension(b);
    idx = b - gRiders_79;
    decal_end_track(idx);
    cam_set_frozen(idx, 0);
    spd = sp->spd;
    b->b6E8 = 2;
    b->b1C4 = 1;
    b->f44C = (f32)spd / 29491.0f;
    bike_update_render_pose(b);
    bike_apply_rider_stats(b, 0);
}

s32 race_try_despawn_extra_rider(Player_21 *p, Player_21 *q)
{
    s16 ang;
    s32 ratio;
    s32 v;
    s32 dt;
    s32 ret;
    f32 dy;
    f32 dx;
    f32 dz;

    if (q->f7A5 != 0) {
        goto fail;
    }
    dt = gRaceFrame - q->f7B0;
    if (dt >= 0xB4) {
        goto ok;
    }
fail:
    return 0;
ok:
    if (dt >= 0x385) {
        q->f7D8 = 0.2f;
    }
    dy = q->f62C - p->f62C;
    if (dy < 20.0f && -50.0f < dy) {
        goto fail;
    }
    dx = q->x - p->x;
    dz = q->z - p->z;
    if (dx * dx + dz * dz < 225.0f) {
        goto fail;
    }
    math_to_polar((s32)(-dx * 1024.0f), (s32)(dz * 1024.0f), &ang, &ratio);
    ang -= p->ang;
    if ((ang < 0 ? -ang : ang) < 0x1800) {
        goto fail;
    }
    ret = 1;
    q->f7A5 = ret;
    return ret;
}

typedef struct { s16 key; s16 id; u8 pad[12]; } Ent_63;
typedef struct { s32 count; Ent_63 e[1]; } Tbl_4;

struct Player_22 {
    u8 pad[0x628];
    s16 f628;
    u8 pad2[0x660 - 0x62A];
    s16 f660;
    s16 f662;
    u8 pad3[0x7E0 - 0x664];
};

extern Tbl_4 *sExtraRiderSpawns_2;
extern Player_22 sExtraRiders[2];

void race_update_extra_riders(Player_22 *p)
{
    Player_22 *q;

    if (gGame.mode == 8) return;
    if (sExtraRiderSpawns_2 != NULL && p->f662 < 2) {
        Ent_63 *e = sExtraRiderSpawns_2->e;
        while (e->key < p->f660) e++;
        while (e->key == p->f660) {
            if (e >= &sExtraRiderSpawns_2->e[sExtraRiderSpawns_2->count]) break;
            if (e->id == p->f628) race_spawn_extra_rider(e);
            e++;
        }
    }
    for (q = sExtraRiders; q < &sExtraRiders[2]; q++) race_try_despawn_extra_rider(p, q);
}

typedef struct {
    u8 pad0[0x662];
    s16 f662;
    s16 f664;
    s16 f666;
    s16 f668;
    u8 pad66A[0x6E8 - 0x66A];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Ent_64;

extern Ent_64 gRiders_80[];

void race_announce_position_changes(void)
{
    Ent_64 *p;
    s32 v;
    s32 idx;

    if (gEvent.time < 0x78) return;
    for (p = gRiders_80; p < gRiders_80 + gGame.numPlayers; p++) {
        v = p->f668 >= 0 ? p->f668 : p->f662;
        if (v != p->f664) {
            if (p->f666 > 0x180) {
                p->f666 = 0x300 - p->f666;
            }
            p->f666 -= 0x20;
            if (p->f666 <= 0) {
                if (p->f6E8 == 0 && p->f668 < 0) {
                    if (gRaceFrame - gAnnounceTimes[p - gRiders_80] > 300 && gEvent.time > 600) {
                        announce_place_change(p - gRiders_80, v, p->f664);
                    }
                }
                p->f666 = 1;
                p->f664 = v;
            }
        } else {
            if (p->f666 < 0x200) {
                p->f666 += 0x10;
            }
        }
    }
}

extern Bike_39 *gRiderStandings_4[];

void race_sort_positions(void)
{
    u8 swapped;
    s32 i;
    Bike_39 *t;

    do {
        swapped = 0;
        for (i = 0; i < 7; i++) {
            if (sRidersByLapPos[i]->f62C > sRidersByLapPos[i + 1]->f62C) {
                swapped = 1;
                t = sRidersByLapPos[i];
                sRidersByLapPos[i] = sRidersByLapPos[i + 1];
                sRidersByLapPos[i + 1] = t;
            }
        }
    } while (swapped);

    do {
        swapped = 0;
        for (i = 0; i < gEvent.numRiders - 1; i++) {
            if (gRiderStandings_4[i]->f630 < gRiderStandings_4[i + 1]->f630
                && (gRiderStandings_4[i]->b7A5 | gRiderStandings_4[i + 1]->b7A5) == 0
                && gRiderStandings_4[i]->h668 < 0) {
                swapped = 1;
                t = gRiderStandings_4[i];
                gRiderStandings_4[i] = gRiderStandings_4[i + 1];
                gRiderStandings_4[i + 1] = t;
            }
        }
    } while (swapped);

    for (i = 0; i < gEvent.numRiders; i++) {
        gRiderStandings_4[i]->h662 = i;
    }
    if (gEvent.time > 120) {
        race_announce_position_changes();
    }
}

extern Ent_65 gRiders_81[];

void race_queue_rider_behind(Ent_65 *p, Ent_65 *p2, s32 p3)
{
    s32 i;

    if (p->f6E8 == 2) {
        p->f7A5 = 1;
        return;
    }
    if (gRiderStandings_5[p3 - 1]->f6E8 == 1 && gRiderStandings_5[p3 - 1]->f7A5 == 0) {
        p->f7A5 = 1;
        p->f630 = -1000000.0f;
        for (i = sRiderQueueBehindCount; i > 0; i--) {
            sRiderQueueBehind[i] = sRiderQueueBehind[i - 1];
        }
        sRiderQueueBehind[0].idx = p - gRiders_81;
        sRiderQueueBehind[0].e = gRiderStandings_5[p3 - 1];
        sRiderQueueBehind[0].d = p2->f630 - gRiderStandings_5[p3 - 1]->f630 - 20.0f;
        sRiderQueueBehindCount++;
    }
}

void race_queue_rider_ahead(u8 *a, u8 *b, s32 n)
{
    u8 *p;
    s32 i;

    if (a[0x6E8] == 2) {
        a[0x7A5] = 1;
        return;
    }
    p = gRiderStandings[n + 1];
    if (p[0x6E8] == 1 && p[0x7A5] == 0) {
        a[0x7A5] = 1;
        i = sRiderQueueAheadCount;
        *(f32 *)(a + 0x630) = 1000000.0f;
        while (i > 0) {
            sRiderQueueAhead[i] = sRiderQueueAhead[i - 1];
            i = i - 1;
        }
        *(s16 *)&sRiderQueueAhead[0] = (s16)(((s32)a - (s32)gRiders) * -0x41041041 >> 5);
        sRiderQueueAhead[0].w1 = (s32)gRiderStandings[n + 1];
        sRiderQueueAhead[0].f2 = (*(f32 *)(gRiderStandings[n + 1] + 0x630) - *(f32 *)(b + 0x630)) - 20.0f;
        sRiderQueueAheadCount = sRiderQueueAheadCount + 1;
    }
}

typedef struct {
    u8 p0[0x626];
    s16 f626;
    u8 p1[0x660 - 0x628];
    s16 f660;
    u8 p2[0x6ED - 0x662];
    u8 f6ED;
    u8 p3[0x7A5 - 0x6EE];
    u8 f7A5;
    u8 p4[0x7E0 - 0x7A6];
} Ent_66;

extern Ent_66 gRiders_82[];

void race_reinsert_queued_rider(u8 *p1, s16 *p2, f32 p3, s32 *p4)
{
    s32 v4;
    s32 idx;
    s32 cnt;
    s32 i;
    s32 lim;
    u8 *r;

    v4 = *(s8 *)(p1 + 0x6ED) + *(s16 *)(p1 + 0x660);
    if (0.0f < p3) {
        idx = *(s16 *)(p1 + 0x628);
        while (0.0f < p3) {
            {
                s32 off = idx * 0x6c;
                r = (u8 *)(off + (s32)gPathNodes);
            }
            idx = *(s16 *)(r + 0x46);
            p3 = p3 - *(f32 *)(r + 0xc);
            if (idx == gPathFinishNode) {
                v4 = v4 + 1;
            }
        }
    } else {
        idx = *(s16 *)(p1 + 0x628);
        while (p3 < 0.0f) {
            {
                s32 off = idx * 0x6c;
                r = (u8 *)(off + (s32)gPathNodes);
            }
            idx = *(s16 *)(r + 0x4a);
            p3 = p3 + *(f32 *)(r + 0xc);
            if (idx == gPathFinishNode) {
                v4 = v4 - 1;
            }
        }
    }
    gRiders_82[*p2].f660 = v4;
    gRiders_82[*p2].f6ED = 0;
    gRiders_82[*p2].f7A5 = 0;
    gRiders_82[*p2].f626 = idx;
    bike_respawn(&gRiders_82[*p2]);
    cnt = *p4 - 1;
    *p4 = cnt;
    for (i = 0; i < *p4; i++) {
        ((T3 *)p2)[i] = ((T3 *)p2)[i + 1];
    }
}

typedef struct {
    u8 pad0[0x3FF];
    u8 b3FF;
    u8 pad400[0x44C - 0x400];
    f32 f44C;
    f32 f450;
    u8 pad454[0x62C - 0x454];
    f32 f62C;
    f32 f630;
    u8 pad634[0x660 - 0x634];
    s16 h660;
    s16 h662;
    u8 pad664[0x6E8 - 0x664];
    u8 b6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_23;

typedef struct {
    u8 pad0[0x20];
    f32 f20[4];
} Tune;

extern Player_23 gRiders_83[];
extern Tune *gCurTrack_33;
extern Player_23 *gRiderStandings_6[];

void race_update_riders(void)
{
    Player_23 *list[4];
    Player_23 **lp;
    Player_23 *pl;
    s32 n1;
    s32 n0;
    f32 sum0;
    f32 sum1;
    s32 i;
    s32 k;
    s32 t;
    s32 best;
    s32 idx;
    f32 f;
    f32 d;
    f32 e;
    f32 g;

    race_sort_positions();
    n1 = 0;
    n0 = 0;
    sum0 = 0.0f;
    sum1 = 0.0f;
    for (i = 0; i < gEvent.numRiders; i++) {
        if (gRiders_83[i].b7A5 == 0) {
            switch (gRiders_83[i].b6E8) {
            case 0:
                list[n0] = &gRiders_83[i];
                n0++;
                sum0 += gRiders_83[i].f630;
                break;
            case 1:
                n1++;
                sum1 += gRiders_83[i].f630;
                break;
            }
        }
    }
    if (n0 != 0) {
        t = gRaceFrame;
        if (t - gLastSpeechTime > 0x384) {
            best = t;
            idx = -1;
            if (n0 >= 2) {
                for (i = 0; i < gGame.numPlayers; i++) {
                    if (gAnnounceTimes[i] < best) {
                        best = gAnnounceTimes[i];
                        idx = i;
                    }
                }
                if (idx >= 0) {
                    if (gRiders_83[idx].f62C > 150.0f || (gRiders_83[idx].h662 != 0 && gRiders_83[idx].h660 != gEvent.numLaps - 1)) {
                        announce_place(idx, gRiders_83[idx].h662 + 1);
                    }
                }
            } else {
                pl = gRiders_83;
                if (!(t - gAnnounceTimes[0] > 0x4B0 && (pl->f62C > 150.0f || (pl->h662 != 0 && pl->h660 != gEvent.numLaps - 1)))) {
                    if (pl->h662 != 0) {
                        pl = gRiderStandings_6[pl->h662 - 1];
                    } else {
                        pl = gRiderStandings_6[pl->h662 + 1];
                    }
                }
                announce_place(pl - gRiders_83, pl->h662 + 1);
            }
        }
        if (gEvent.time == 300) {
            announce_early_leader(gRiderStandings_6[0] - gRiders_83);
        }
    }
    if (n1 != 0) {
        sum1 /= n1;
    }
    if (n0 != 0) {
        sum0 /= n0;
    } else {
        sum0 = sum1;
    }
    if (n1 == 0) {
        sum1 = sum0;
    }
    if (gGame.flags & 0x10) {
        k = 0;
        for (i = 0; i < gEvent.numRiders; i++) {
            pl = gRiderStandings_6[i];
            if (pl->b6E8 == 1) {
                e = pl->f630 - sum1;
                e += (gPathLapLength / n1) * (k - (n1 - 1) * 0.5f);
                f = 1.0f - e / 100.0f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
                pl->f44C = f;
                k++;
            }
        }
    } else if (n0 == 0) {
        k = 0;
        for (i = 0; i < gEvent.numRiders; i++) {
            pl = gRiderStandings_6[i];
            if (pl->b6E8 == 1) {
                e = pl->f630 - sum1;
                e -= (k - (n1 - 1) * 0.5f) * 0.01f;
                f = 1.0f - e * 0.00390625f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
                pl->f44C = f;
                k++;
            }
        }
    } else if (gGame.mode != 8 && gEvent.state >= 5) {
        d = ((sum1 - sum0) - 30.0f) * 0.00390625f;
        if (d > 0.15f) {
            d = 0.15f;
        } else if (d < -0.15f) {
            d = -0.15f;
        }
        for (i = 0; i < gEvent.numRiders; i++) {
            pl = &gRiders_83[i];
            if (pl->b6E8 != 0) {
                f = 0.0f;
                if (n1 >= 2) {
                    f = pl->f450;
                }
                e = pl->f630 - sum1;
                if (e > 64.0f) {
                    f -= 0.1f;
                }
                f += 1.0f - d;
                if (pl->b3FF) {
                    f = 1.0f;
                } else if (gCheatFlags & 0x1000) {
                    f *= gCurTrack_33->f20[2] + 0.05f;
                } else {
                    g = gCurTrack_33->f20[gGame.difficulty - 1];
                    f *= g;
                    if (gEvent.flags & 0x200) {
                        f = g * 1.15f;
                    }
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.0002f;
                } else {
                    pl->f44C = pl->f44C + 0.0002f;
                }
            } else {
                e = sum0 - pl->f630;
                if (e > 2.0f) {
                    e += 16.0f;
                } else if (e < -2.0f) {
                    e -= 16.0f;
                }
                f = e * 0.0009765625f + 1.0f;
                if (f > 1.05f) {
                    f = 1.05f;
                } else if (f < 0.95f) {
                    f = 0.95f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
            }
        }
    }
    if (gGame.numPlayers == 1 && n0 == 1) {
        race_update_extra_riders((u8 *)list[0]);
    }
    ai_update_lanes();
}

#define NEAR(r, x, top, done)                           {                                                       Player_24 *q_;                                         f32 v_, k_;                                     top:                                                    q_ = *l;                                            if (q_->human != 0) { v_ = q_->lane; k_ = 0.999f; } else { v_ = q_->f638; k_ = 0.999f; } v_ = v_ - (x);                                      l++;                                                if (v_ < k_ && (k_ = -k_) < v_) {                              r = 1;                                              goto done;                                      }                                                   if (*l != NULL) {                                       goto top;                                       }                                                   r = 0;                                          done:;                                              }

typedef struct Player_24 {
    u8 pad0[0x48];
    f32 x;          /* 0x48 */
    f32 y;          /* 0x4C */
    f32 z;          /* 0x50 */
    u8 pad54[0x42C - 0x54];
    f32 lane;       /* 0x42C */
    u8 pad430[0x628 - 0x430];
    s16 node;       /* 0x628 */
    s16 pad62A;
    f32 prog;       /* 0x62C */
    u8 pad630[0x638 - 0x630];
    f32 f638;       /* 0x638 */
    u8 pad63C[0x6E8 - 0x63C];
    u8 human;       /* 0x6E8 */
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;        /* 0x7A5 */
    u8 f7A6;        /* 0x7A6 */
    u8 pad7A7[0x7E0 - 0x7A7];
} Player_24;

typedef struct {
    u8 pad0[0x46];
    s16 next;       /* 0x46 */
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} TrackNode;

extern Player_24 sExtraRiders_2[];

void ai_update_lanes(void)
{
    Player_24 *list[8];
    Player_24 *p;
    Player_24 *q;
    Player_24 **out;
    TrackNode *t;
    s32 i;
    s32 j;
    s32 n;
    s32 node;
    f32 lo;
    f32 hi;
    f32 d;
    f32 dx;
    f32 dz;
    f32 dist;
    f32 v;
    f32 c;
    f32 e;
    f32 best;
    f32 target;
    f32 len;
    f32 diff;
    u8 hit;
    f32 w;
    Player_24 **l;
    f32 x;

    for (i = 0; i < 8; i++) {
        p = sRidersByLapPos[i];
        if (p->human == 0 || p->f7A5 != 0) {
            continue;
        }
        if (p >= sExtraRiders_2) {
            p->lane = sExtraRiderLanes[p - sExtraRiders_2];
        }
        lo = -20.0f;
        hi = 20.0f;
        node = p->node;
        j = 6;
        do {
            t = (TrackNode *)(node * sizeof(TrackNode) + (u32)gPathNodes);
            if (lo < -t->f58) {
                lo = -t->f58;
            }
            if (t->f5C < hi) {
                hi = t->f5C;
            }
            j--;
            node = t->next;
        } while (j != 0);
        if (hi < p->lane) {
            p->lane = hi;
        }
        if (p->lane < lo) {
            p->lane = lo;
        }
        n = 0;
        j = 0;
        p->f7A6 = 0;
        for (; j < 8; j++) {
            if (i == j) {
                continue;
            }
            q = sRidersByLapPos[j];
            if (q->f7A5 != 0) {
                continue;
            }
            d = p->prog - q->prog + 1.0f;
            if (d < 0.0f) {
                d += gPathLapLength;
            }
            if (d > 5.0f) {
                continue;
            }
            dx = q->x - p->x;
            dz = q->z - p->z;
            dist = dx * dx + dz * dz;
            if (d * d + 100.0f < dist) {
                continue;
            }
            list[n] = q;
            n++;
            if (dist > 2.25f) {
                continue;
            }
            w = q->f638;
            diff = w - p->f638;
            if (d < 2.0f && dist < 2.25f) {
                if (diff > 0.0f) {
                    if (w < hi) {
                        hi = w;
                    }
                } else {
                    if (lo < w) {
                        lo = w;
                    }
                }
            }
        }
        list[n] = NULL;
        if (n != 0) {
            l = list;
            x = p->lane;
            NEAR(hit, x, n1, d1);
            if (hit) {
                best = 2.0f;
                for (n--; n >= 0; n--) {
                    q = list[n];
                    if (q->human != 0) {
                        v = q->lane;
                    } else {
                        v = q->f638;
                    }
                    c = v - 1.0f;
                    e = p->lane - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n2, d2);
                        if (!hit) {
                            target = c;
                            best = p->lane - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                    c = v + 1.0f;
                    e = p->lane - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n3, d3);
                        if (!hit) {
                            target = c;
                            best = p->lane - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                }
                if (best < 2.0f) {
                    p->lane = target;
                }
                continue;
            }
        }
        if (p < sExtraRiders_2) {
            if (p->lane > 0.0f) {
                p->lane -= 0.02f;
            } else {
                p->lane += 0.02f;
            }
        }
    }
}

typedef struct {
    u8 unk0[0x44C];
    f32 unk44C;
    f32 unk450;
    u8 unk454[0x7E0 - 0x454];
} Racer_2;

extern Racer_2 gRiders_84[];

void race_init_ai_skills(s32 arg0, s32 count) {
    s32 i;


    for (i = 0; i < count; i++) {
        if (count == 1) {
            gRiders_84[sAiRiderOrder[i]].unk450 = 0.0f;
        } else {
            gRiders_84[sAiRiderOrder[i]].unk450 = (i - (count - 1) * 0.5f) / ((count - 1) * 5.0f);
        }
        if (gRiders_84[sAiRiderOrder[i]].unk450 == 0.1f) {
            gRiders_84[sAiRiderOrder[i]].unk450 += 0.01f;
        } else {
            gRiders_84[sAiRiderOrder[i]].unk450 -= 0.01f;
        }
        gRiders_84[sAiRiderOrder[i]].unk44C = 1.03f - gRiders_84[sAiRiderOrder[i]].unk450;
    }
}

void race_jitter_ai_order(s32 p1, s32 n)
{
    s32 arr[8];
    s32 *p;
    s32 i;
    s32 val;
    s32 a;
    s32 b;
    u8 flag;
    s32 t;

    i = 0;
    if (n > 0) {
        p = arr;
        val = i;
        do {
            *p = val;
            a = random_byte();
            b = random_byte();
            val += 100;
            i++;
            *p = *p + (a + b);
            p++;
        } while (i < n);
    }
    do {
        flag = 0;
        for (i = 0; i < n; i++) {
            if (arr[i] > arr[i + 1]) {
                t = sAiRiderOrder[i];
                sAiRiderOrder[i] = sAiRiderOrder[i + 1];
                sAiRiderOrder[i + 1] = t;
                t = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = t;
                flag = 1;
            }
        }
    } while (flag);
}

typedef struct {
    u8 pad0[0x2C];
    s32 state;
    u8 pad1[0x20];
    u32 flags;
} Obj_46;

extern Obj_46 *gCurTrack_34;

void race_load_extra_rider_spawns(s32 arg0)
{
    if (gCurTrack_34->state != 2 && (gCurTrack_34->flags & 2) == 0) {
        D_800F0BD8 = track_res_find(D_800D14E8);
        sExtraRiderSpawns = track_res_get(D_800D14E8);
    }
}

void race_free_extra_rider_spawns(void)
{
    if (sExtraRiderSpawns != 0) {
        sExtraRiderSpawns = 0;
    }
}

void race_set_grid_order(s32 a, s32 b)
{
    s32 n = a + b;
    s32 i;
    s32 half = n / 2;
    u8 *q;

    if (n < 3) {
        u8 *r;
        sGridOrderNext[0] = 0;
        r = sGridOrderNext;
        r[1] = 1;
        return;
    }
    for (i = 0; i < n; i++) {
        q = sGridOrderNext + i;
        if (i & 1) {
            *q = half + i / 2;
        } else {
            *q = half - (i / 2 + 1);
        }
    }
}

void race_reset_grid_order(void)
{
    mem_fill(sGridOrderNext, 0, 8);
    sGridOrderNext[1] = 1;
}

void race_randomize_ai_order(s32 arg0, s32 count)
{
    s32 i, k;

    for (i = 0; i < count; i++) {
        sAiRiderOrder[i] = (((s32)random_byte() * count) >> 8) + arg0;
        for (k = 0; k < i; k++) {
            if (sAiRiderOrder[k] == sAiRiderOrder[i]) {
                i--;
                break;
            }
        }
    }
    race_init_ai_skills(arg0, count);
}

typedef struct {
    s16 h0;
    s16 rest[5];
} Ent12;

void race_init_standings(s32 arg0) {
    s32 i = 0;
    s16 m1 = -1;
    Ent12 *p0 = sRiderQueueAhead;
    Ent12 *p1 = sRiderQueueBehind;
    u8 **p2 = gRiderStandings;
    u8 **p3 = sRidersByLapPos;
    u8 *v = gRiders;
    do {
        *p3 = v;
        *p2 = v;
        p1->h0 = m1;
        p0->h0 = m1;
        p0++;
        p1++;
        p2++;
        p3++;
        v[0x7A7] = i;
        i++;
        v += 0x7E0;
    } while (i < 8);
    sRiderQueueAheadCount = 0;
    sRiderQueueBehindCount = 0;
}

typedef struct {
    s32 unk0;
    u8 *obj;
    f32 thr;
} Ent_67;

extern Ent_67 sRiderQueueAhead_2;
extern Ent_67 sRiderQueueBehind_2;

void race_update_rider_queues(u8 *p)
{
    f32 a, b, d;
    if (sRiderQueueAheadCount > 0) {
        b = *(f32 *)(sRiderQueueAhead_2.obj + 0x630);
        a = *(f32 *)(p + 0x630);
        d = b - a;
        if (d < sRiderQueueAhead_2.thr) {
            race_reinsert_queued_rider(p, &sRiderQueueAhead_2, 70.0f, &sRiderQueueAheadCount);
        }
    }
    if (sRiderQueueBehindCount > 0) {
        b = *(f32 *)(p + 0x630);
        a = *(f32 *)(sRiderQueueBehind_2.obj + 0x630);
        d = b - a;
        if (d < sRiderQueueBehind_2.thr) {
            race_reinsert_queued_rider(p, &sRiderQueueBehind_2, -30.0f, &sRiderQueueBehindCount);
        }
    }
}

struct Obj_47 {
    u8 pad[0x42C];
    f32 f42C;
    u8 pad2[0x638 - 0x430];
    f32 f638;
    u8 pad3[0x6E8 - 0x63C];
    u8 b6E8;
};

s32 ai_lane_is_blocked(Obj_47 **p, f32 x)
{
    f32 d, k;
    Obj_47 *o;

    do {
        o = *p;
        if (o->b6E8 != 0) {
            d = o->f42C;
            k = 0.999f;
            d = d - x;
        } else {
            d = o->f638;
            k = 0.999f;
            d = d - x;
        }
        p++;
        if (d < k && -k < d) {
            return 1;
        }
    } while (*p != 0);
    return 0;
}

void race_restore_grid_order(void)
{
    gGridOrder = sGridOrderSaved;
}

extern Blob6 sGridOrderNext_2;

void race_apply_grid_order(void) {
    gGridOrder = sGridOrderNext_2;
    sGridOrderSaved = gGridOrder;
}

void race_grid_from_standings(void)
{
    s32 i;
    s32 j;
    s32 n;
    u8 *arr;
    Event *t;
    u8 *hb;

    mem_fill_3(sGridOrderNext, -1, 6);
    if (gGame.mode == 1) {
        Event *p = &gEvent;
        i = 0;
        if (p->numRiders > 0) {
            t = p;
            do {
                j = 0;
                if (t->numRiders > 0) {
                    arr = (u8 *)gSeason;
                    hb = (u8 *)0x800F0000;
                    __asm__ __volatile__("" : "+r"(hb));
                    n = *(s16 *)(hb + 0x5CA); /* gEvent.numRiders */
inner:
                    if (*(u8 *)((u32)arr + (u32)j + 0x6c) == i) {
                        goto store;
                    }
                    j++;
                    if (j < n) {
                        goto inner;
                    }
                }
store:
                sGridOrderNext[i] = (u8)j;
                i++;
            } while (i < t->numRiders);
        }
    }
}
