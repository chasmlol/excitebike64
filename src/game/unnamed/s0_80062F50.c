#include "game.h"

typedef struct {
    s16 idx;
    Rider *e;
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

extern Rider *sRidersByLapPos[];
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

void race_spawn_extra_rider(Spawn *sp)
{
    Rider *b;
    s32 track;
    s32 i, idx;
    f32 f;
    s32 spd;
    f32 y, k;
    Vec3f unused;

    if (D_800F1BC4 & 0x10) return;
    if (gEventFlags & 0x200) return;

    b = &gRiders[6];
    track = sp->track;
    sp->track = -1;
    for (; b < &gRiders[8]; b++) {
        if (b->inactive != 0) break;
    }
    if (b == &gRiders[8]) return;

    b->prevAnim = -1;
    b->inactive = 0;
    b->crashPhase = 0;
    b->roughLanding = 0;
    b->pendingCrash = 0;
    b->crashState = 0;
    b->engine.overheated = 0;
    b->jumpState = 0;
    b->airFrames = 0;
    b->wheelieTimer = 0;
    b->crashTimer = 0;
    b->engine.heat = 0.0f;
    b->frontWheel.airTicks = 0;
    b->rearWheel.airTicks = 0;
    b->spawnFrame = gRaceFrame;
    b->bike.pos.x = (f32)sp->x / 1000.0f;
    b->bike.pos.z = (f32)sp->z / 1000.0f;
    b->bike.pos.y = 0.0f;
    b->bike.pos.y = bike_get_ground_height(&b->bike.pos);
    for (i = 0; i < 20; i++) {
        b->bike.contacts[i] = b->bike.pos;
    }
    b->segment = path_find_segment(&b->bike.pos, track, 15, 0);
    b->laneOffset = path_get_lateral_offset(&b->bike.pos, b->segment);
    b->segmentT = gPathSegmentT;
    f = path_get_lateral_offset(&b->bike.pos, b->segment);
    b->laneOffset = f;
    sExtraRiderLanes[b - &gRiders[6]] = f;
    quat_from_euler(b->bike.quat, 0.0f, 0.0f, -math_atan2f(gPathTangentZ, gPathTangentX));
    bike_place_on_ground(b);
    quat_to_mtx3f(&b->bike.right.x, b->bike.quat);
    b->bike.vel.x = b->bike.fwd.x * ((f32)sp->scale / 1000.0f);
    b->bike.vel.y = b->bike.fwd.y * ((f32)sp->scale / 1000.0f);
    b->bike.vel.z = b->bike.fwd.z * ((f32)sp->scale / 1000.0f);
    b->frontWheel.suspLength = 0.0f;
    b->rearWheel.suspLength = 0.0f;
    b->bike.angVel.z = 0.0f;
    b->bike.angVel.y = 0.0f;
    b->bike.angVel.x = 0.0f;
    b->bike.angAcc.x = 0.0f;
    b->bike.acc.z = 0.0f;
    b->bike.acc.y = 0.0f;
    b->bike.acc.x = 0.0f;
    b->localAngVel.x = 0.0f;
    b->localAngVel.y = 0.0f;
    b->localAngVel.z = 0.0f;
    b->bike.unk_170 = 1.0f;
    b->anim = -1;
    *(Blk *)b->drawQuat = *(Blk *)b->bike.quat;
    b->bike.pos.y = bike_get_ground_height(&b->bike.pos) + 0.0f + 0.5f
        - (gGameMode == 6 ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    b->airFrames = 4;
    bike_update_wheel_contacts(b);
    bike_update_wheel_contacts(b);
    bike_update_suspension(b);
    idx = b - gRiders;
    decal_end_track(idx);
    cam_set_frozen(idx, 0);
    spd = sp->spd;
    b->isCpu = 2;
    b->bike.type = 1;
    b->timeScale = (f32)spd / 29491.0f;
    bike_update_render_pose(b);
    bike_apply_rider_stats(b, 0);
}

s32 race_try_despawn_extra_rider(Rider *p, Rider *q)
{
    s16 ang;
    s32 ratio;
    s32 v;
    s32 dt;
    s32 ret;
    f32 dy;
    f32 dx;
    f32 dz;

    if (q->inactive != 0) {
        goto fail;
    }
    dt = gRaceFrame - q->spawnFrame;
    if (dt >= 0xB4) {
        goto ok;
    }
fail:
    return 0;
ok:
    if (dt >= 0x385) {
        q->power = 0.2f;
    }
    dy = q->segmentDist - p->segmentDist;
    if (dy < 20.0f && -50.0f < dy) {
        goto fail;
    }
    dx = q->bike.pos.x - p->bike.pos.x;
    dz = q->bike.pos.z - p->bike.pos.z;
    if (dx * dx + dz * dz < 225.0f) {
        goto fail;
    }
    math_to_polar((s32)(-dx * 1024.0f), (s32)(dz * 1024.0f), &ang, &ratio);
    ang -= p->heading;
    if ((ang < 0 ? -ang : ang) < 0x1800) {
        goto fail;
    }
    ret = 1;
    q->inactive = ret;
    return ret;
}

typedef struct { s16 key; s16 id; u8 pad[12]; } Ent_63;
typedef struct { s32 count; Ent_63 e[1]; } Tbl_4;

extern Tbl_4 *sExtraRiderSpawns_2;
extern Rider sExtraRiders[2];

void race_update_extra_riders(Rider *p)
{
    Rider *q;

    if (gGameMode == 8) return;
    if (sExtraRiderSpawns_2 != NULL && p->race.place < 2) {
        Ent_63 *e = sExtraRiderSpawns_2->e;
        while (e->key < p->race.lap) e++;
        while (e->key == p->race.lap) {
            if (e >= &sExtraRiderSpawns_2->e[sExtraRiderSpawns_2->count]) break;
            if (e->id == p->segment) race_spawn_extra_rider(e);
            e++;
        }
    }
    for (q = sExtraRiders; q < &sExtraRiders[2]; q++) race_try_despawn_extra_rider(p, q);
}

void race_announce_position_changes(void)
{
    Rider *p;
    s32 v;
    s32 idx;

    if (gEvent_37.f2C < 0x78) return;
    for (p = gRiders; p < gRiders + gGame_9.count; p++) {
        v = p->race.finishPlace >= 0 ? p->race.finishPlace : p->race.place;
        if (v != p->race.prevPlace) {
            if (p->race.placeAnim > 0x180) {
                p->race.placeAnim = 0x300 - p->race.placeAnim;
            }
            p->race.placeAnim -= 0x20;
            if (p->race.placeAnim <= 0) {
                if (p->isCpu == 0 && p->race.finishPlace < 0) {
                    if (gRaceFrame - gAnnounceTimes[p - gRiders] > 300 && gEvent_37.f2C > 600) {
                        announce_place_change(p - gRiders, v, p->race.prevPlace);
                    }
                }
                p->race.placeAnim = 1;
                p->race.prevPlace = v;
            }
        } else {
            if (p->race.placeAnim < 0x200) {
                p->race.placeAnim += 0x10;
            }
        }
    }
}

typedef struct {
    u8 pad[0x22];
    s16 count;
} Race_5;

extern Race_5 gEvent_38;

void race_sort_positions(void)
{
    u8 swapped;
    s32 i;
    Rider *t;

    do {
        swapped = 0;
        for (i = 0; i < 7; i++) {
            if (sRidersByLapPos[i]->segmentDist > sRidersByLapPos[i + 1]->segmentDist) {
                swapped = 1;
                t = sRidersByLapPos[i];
                sRidersByLapPos[i] = sRidersByLapPos[i + 1];
                sRidersByLapPos[i + 1] = t;
            }
        }
    } while (swapped);

    do {
        swapped = 0;
        for (i = 0; i < gEvent_38.count - 1; i++) {
            if (gRiderStandings[i]->distToGo < gRiderStandings[i + 1]->distToGo
                && (gRiderStandings[i]->inactive | gRiderStandings[i + 1]->inactive) == 0
                && gRiderStandings[i]->race.finishPlace < 0) {
                swapped = 1;
                t = gRiderStandings[i];
                gRiderStandings[i] = gRiderStandings[i + 1];
                gRiderStandings[i + 1] = t;
            }
        }
    } while (swapped);

    for (i = 0; i < gEvent_38.count; i++) {
        gRiderStandings[i]->race.place = i;
    }
    if (gEventTime > 120) {
        race_announce_position_changes();
    }
}

void race_queue_rider_behind(Rider *p, Rider *p2, s32 p3)
{
    s32 i;

    if (p->isCpu == 2) {
        p->inactive = 1;
        return;
    }
    if (gRiderStandings[p3 - 1]->isCpu == 1 && gRiderStandings[p3 - 1]->inactive == 0) {
        p->inactive = 1;
        p->distToGo = -1000000.0f;
        for (i = sRiderQueueBehindCount; i > 0; i--) {
            sRiderQueueBehind[i] = sRiderQueueBehind[i - 1];
        }
        sRiderQueueBehind[0].idx = p - gRiders;
        sRiderQueueBehind[0].e = gRiderStandings[p3 - 1];
        sRiderQueueBehind[0].d = p2->distToGo - gRiderStandings[p3 - 1]->distToGo - 20.0f;
        sRiderQueueBehindCount++;
    }
}

void race_queue_rider_ahead(Rider *a, Rider *b, s32 n)
{
    Rider *p;
    s32 i;

    if (a->isCpu == 2) {
        a->inactive = 1;
        return;
    }
    p = gRiderStandings[n + 1];
    if (p->isCpu == 1 && p->inactive == 0) {
        a->inactive = 1;
        i = sRiderQueueAheadCount;
        a->distToGo = 1000000.0f;
        while (i > 0) {
            sRiderQueueAhead[i] = sRiderQueueAhead[i - 1];
            i = i - 1;
        }
        *(s16 *)&sRiderQueueAhead[0] = (s16)(((s32)a - (s32)gRiders) * -0x41041041 >> 5);
        sRiderQueueAhead[0].w1 = (s32)gRiderStandings[n + 1];
        sRiderQueueAhead[0].f2 = (gRiderStandings[n + 1]->distToGo - b->distToGo) - 20.0f;
        sRiderQueueAheadCount = sRiderQueueAheadCount + 1;
    }
}

void race_reinsert_queued_rider(Rider *p1, s16 *p2, f32 p3, s32 *p4)
{
    s32 v4;
    s32 idx;
    s32 cnt;
    s32 i;
    s32 lim;
    u8 *r;

    v4 = p1->lapAdjust + p1->race.lap;
    if (0.0f < p3) {
        idx = p1->segment;
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
        idx = p1->segment;
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
    gRiders[*p2].race.lap = v4;
    gRiders[*p2].lapAdjust = 0;
    gRiders[*p2].inactive = 0;
    gRiders[*p2].prevSegment = idx;
    bike_respawn(&gRiders[*p2]);
    cnt = *p4 - 1;
    *p4 = cnt;
    for (i = 0; i < *p4; i++) {
        ((T3 *)p2)[i] = ((T3 *)p2)[i + 1];
    }
}

typedef struct {
    u32 w0;
    u8 pad4[0x1C];
    s16 h20;
    s16 h22;
    u8 pad24[8];
    s32 w2C;
    u8 pad30[0x28];
    s32 w58;
} Mode_4;

typedef struct {
    s32 w0;
    u8 pad4[0x48];
    s32 w4C;
    u8 pad50[0x410 - 0x50];
    s32 w410;
} Cfg_9;

typedef struct {
    u8 pad0[0x20];
    f32 f20[4];
} Tune;

extern Mode_4 gEvent_39;
extern Cfg_9 gGame_46;
extern Tune *gCurTrack_33;

void race_update_riders(void)
{
    Rider *list[4];
    Rider **lp;
    Rider *pl;
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
    for (i = 0; i < gEvent_39.h22; i++) {
        if (gRiders[i].inactive == 0) {
            switch (gRiders[i].isCpu) {
            case 0:
                list[n0] = &gRiders[i];
                n0++;
                sum0 += gRiders[i].distToGo;
                break;
            case 1:
                n1++;
                sum1 += gRiders[i].distToGo;
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
                for (i = 0; i < gNumPlayers; i++) {
                    if (gAnnounceTimes[i] < best) {
                        best = gAnnounceTimes[i];
                        idx = i;
                    }
                }
                if (idx >= 0) {
                    if (gRiders[idx].segmentDist > 150.0f || (gRiders[idx].race.place != 0 && gRiders[idx].race.lap != gEvent_39.h20 - 1)) {
                        announce_place(idx, gRiders[idx].race.place + 1);
                    }
                }
            } else {
                pl = gRiders;
                if (!(t - gAnnounceTimes[0] > 0x4B0 && (pl->segmentDist > 150.0f || (pl->race.place != 0 && pl->race.lap != gEvent_39.h20 - 1)))) {
                    if (pl->race.place != 0) {
                        pl = gRiderStandings[pl->race.place - 1];
                    } else {
                        pl = gRiderStandings[pl->race.place + 1];
                    }
                }
                announce_place(pl - gRiders, pl->race.place + 1);
            }
        }
        if (gEvent_39.w2C == 300) {
            announce_early_leader(gRiderStandings[0] - gRiders);
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
    if (gGame_46.w0 & 0x10) {
        k = 0;
        for (i = 0; i < gEvent_39.h22; i++) {
            pl = gRiderStandings[i];
            if (pl->isCpu == 1) {
                e = pl->distToGo - sum1;
                e += (gPathLapLength / n1) * (k - (n1 - 1) * 0.5f);
                f = 1.0f - e / 100.0f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->timeScale) {
                    pl->timeScale = pl->timeScale - 0.001f;
                } else {
                    pl->timeScale = pl->timeScale + 0.001f;
                }
                pl->timeScale = f;
                k++;
            }
        }
    } else if (n0 == 0) {
        k = 0;
        for (i = 0; i < gEvent_39.h22; i++) {
            pl = gRiderStandings[i];
            if (pl->isCpu == 1) {
                e = pl->distToGo - sum1;
                e -= (k - (n1 - 1) * 0.5f) * 0.01f;
                f = 1.0f - e * 0.00390625f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->timeScale) {
                    pl->timeScale = pl->timeScale - 0.001f;
                } else {
                    pl->timeScale = pl->timeScale + 0.001f;
                }
                pl->timeScale = f;
                k++;
            }
        }
    } else if (gGame_46.w4C != 8 && gEvent_39.w0 >= 5) {
        d = ((sum1 - sum0) - 30.0f) * 0.00390625f;
        if (d > 0.15f) {
            d = 0.15f;
        } else if (d < -0.15f) {
            d = -0.15f;
        }
        for (i = 0; i < gEvent_39.h22; i++) {
            pl = &gRiders[i];
            if (pl->isCpu != 0) {
                f = 0.0f;
                if (n1 >= 2) {
                    f = pl->aiSkill;
                }
                e = pl->distToGo - sum1;
                if (e > 64.0f) {
                    f -= 0.1f;
                }
                f += 1.0f - d;
                if (pl->finishState) {
                    f = 1.0f;
                } else if (gCheatFlags & 0x1000) {
                    f *= gCurTrack_33->f20[2] + 0.05f;
                } else {
                    g = gCurTrack_33->f20[gGame_46.w410 - 1];
                    f *= g;
                    if (gEvent_39.w58 & 0x200) {
                        f = g * 1.15f;
                    }
                }
                if (f < pl->timeScale) {
                    pl->timeScale = pl->timeScale - 0.0002f;
                } else {
                    pl->timeScale = pl->timeScale + 0.0002f;
                }
            } else {
                e = sum0 - pl->distToGo;
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
                if (f < pl->timeScale) {
                    pl->timeScale = pl->timeScale - 0.001f;
                } else {
                    pl->timeScale = pl->timeScale + 0.001f;
                }
            }
        }
    }
    if (gNumPlayers == 1 && n0 == 1) {
        race_update_extra_riders(list[0]);
    }
    ai_update_lanes();
}

#define NEAR(r, x, top, done)                           {                                                       Rider *q_;                                         f32 v_, k_;                                     top:                                                    q_ = *l;                                            if (q_->isCpu != 0) { v_ = q_->laneOffset; k_ = 0.999f; } else { v_ = q_->lateral; k_ = 0.999f; } v_ = v_ - (x);                                      l++;                                                if (v_ < k_ && (k_ = -k_) < v_) {                              r = 1;                                              goto done;                                      }                                                   if (*l != NULL) {                                       goto top;                                       }                                                   r = 0;                                          done:;                                              }

typedef struct {
    u8 pad0[0x46];
    s16 next;       /* 0x46 */
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} TrackNode;

void ai_update_lanes(void)
{
    Rider *list[8];
    Rider *p;
    Rider *q;
    Rider **out;
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
    Rider **l;
    f32 x;

    for (i = 0; i < 8; i++) {
        p = sRidersByLapPos[i];
        if (p->isCpu == 0 || p->inactive != 0) {
            continue;
        }
        if (p >= sExtraRiders) {
            p->laneOffset = sExtraRiderLanes[p - sExtraRiders];
        }
        lo = -20.0f;
        hi = 20.0f;
        node = p->segment;
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
        if (hi < p->laneOffset) {
            p->laneOffset = hi;
        }
        if (p->laneOffset < lo) {
            p->laneOffset = lo;
        }
        n = 0;
        j = 0;
        p->laneBlocked = 0;
        for (; j < 8; j++) {
            if (i == j) {
                continue;
            }
            q = sRidersByLapPos[j];
            if (q->inactive != 0) {
                continue;
            }
            d = p->segmentDist - q->segmentDist + 1.0f;
            if (d < 0.0f) {
                d += gPathLapLength;
            }
            if (d > 5.0f) {
                continue;
            }
            dx = q->bike.pos.x - p->bike.pos.x;
            dz = q->bike.pos.z - p->bike.pos.z;
            dist = dx * dx + dz * dz;
            if (d * d + 100.0f < dist) {
                continue;
            }
            list[n] = q;
            n++;
            if (dist > 2.25f) {
                continue;
            }
            w = q->lateral;
            diff = w - p->lateral;
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
            x = p->laneOffset;
            NEAR(hit, x, n1, d1);
            if (hit) {
                best = 2.0f;
                for (n--; n >= 0; n--) {
                    q = list[n];
                    if (q->isCpu != 0) {
                        v = q->laneOffset;
                    } else {
                        v = q->lateral;
                    }
                    c = v - 1.0f;
                    e = p->laneOffset - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n2, d2);
                        if (!hit) {
                            target = c;
                            best = p->laneOffset - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                    c = v + 1.0f;
                    e = p->laneOffset - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n3, d3);
                        if (!hit) {
                            target = c;
                            best = p->laneOffset - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                }
                if (best < 2.0f) {
                    p->laneOffset = target;
                }
                continue;
            }
        }
        if (p < sExtraRiders) {
            if (p->laneOffset > 0.0f) {
                p->laneOffset -= 0.02f;
            } else {
                p->laneOffset += 0.02f;
            }
        }
    }
}

void race_init_ai_skills(s32 arg0, s32 count) {
    s32 i;


    for (i = 0; i < count; i++) {
        if (count == 1) {
            gRiders[sAiRiderOrder[i]].aiSkill = 0.0f;
        } else {
            gRiders[sAiRiderOrder[i]].aiSkill = (i - (count - 1) * 0.5f) / ((count - 1) * 5.0f);
        }
        if (gRiders[sAiRiderOrder[i]].aiSkill == 0.1f) {
            gRiders[sAiRiderOrder[i]].aiSkill += 0.01f;
        } else {
            gRiders[sAiRiderOrder[i]].aiSkill -= 0.01f;
        }
        gRiders[sAiRiderOrder[i]].timeScale = 1.03f - gRiders[sAiRiderOrder[i]].aiSkill;
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
    Rider **p2 = gRiderStandings;
    Rider **p3 = sRidersByLapPos;
    Rider *v = gRiders;
    do {
        *p3 = v;
        *p2 = v;
        p1->h0 = m1;
        p0->h0 = m1;
        p0++;
        p1++;
        p2++;
        p3++;
        v->standing = i;
        i++;
        v++;
    } while (i < 8);
    sRiderQueueAheadCount = 0;
    sRiderQueueBehindCount = 0;
}

typedef struct {
    s32 unk0;
    Rider *obj;
    f32 thr;
} Ent_67;

extern Ent_67 sRiderQueueAhead_2;
extern Ent_67 sRiderQueueBehind_2;

void race_update_rider_queues(Rider *p)
{
    f32 a, b, d;
    if (sRiderQueueAheadCount > 0) {
        b = sRiderQueueAhead_2.obj->distToGo;
        a = p->distToGo;
        d = b - a;
        if (d < sRiderQueueAhead_2.thr) {
            race_reinsert_queued_rider(p, &sRiderQueueAhead_2, 70.0f, &sRiderQueueAheadCount);
        }
    }
    if (sRiderQueueBehindCount > 0) {
        b = p->distToGo;
        a = sRiderQueueBehind_2.obj->distToGo;
        d = b - a;
        if (d < sRiderQueueBehind_2.thr) {
            race_reinsert_queued_rider(p, &sRiderQueueBehind_2, -30.0f, &sRiderQueueBehindCount);
        }
    }
}

s32 ai_lane_is_blocked(Rider **p, f32 x)
{
    f32 d, k;
    Rider *o;

    do {
        o = *p;
        if (o->isCpu != 0) {
            d = o->laneOffset;
            k = 0.999f;
            d = d - x;
        } else {
            d = o->lateral;
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
    Cnt *t;
    u8 *hb;

    mem_fill_3(sGridOrderNext, -1, 6);
    if (gGameMode == 1) {
        Cnt *p = &gEvent;
        i = 0;
        if (p->n > 0) {
            t = p;
            do {
                j = 0;
                if (t->n > 0) {
                    arr = gSeason;
                    hb = (u8 *)0x800F0000;
                    __asm__ __volatile__("" : "+r"(hb));
                    n = *(s16 *)(hb + 0x5CA);
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
            } while (i < t->n);
        }
    }
}
