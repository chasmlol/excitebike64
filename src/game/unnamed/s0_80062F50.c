#include "game.h"
#include "symaddr.h"

/* Race positions, rubber-banding, the CPU riders' lanes, the starting grid, and the extra riders (6 and 7, the
   "drones") that appear near the leading human rider and leave once out of sight. */

/* An entry of the queues of riders taken out of the race (sRiderQueueBehind, sRiderQueueAhead): the rider, the
   rider whose distance it waits for, and the gap at which it comes back. Rec_36, Ent12 and Ent_67 are other views of
   it. */
typedef struct {
    s16 rider;                  /* index in gRiders */
    Rider *watch;
    f32 gap;
} Rec_35;

typedef struct {
    s32 rider;                  /* the high halfword: index in gRiders */
    s32 watch;                  /* Rider * */
    f32 gap;
} Rec_36;

/* A starting grid: each rider's position across the start line (also the starting place). */
typedef struct {
    u8 slot[6];
} Blob6;

extern Rider *sRidersByLapPos[];
extern Rec_35 sRiderQueueBehind[];
extern Rec_36 sRiderQueueAhead[];
extern Blob6 sGridOrderSaved;
extern Blob6 gGridOrder;

/* An extra rider's spawn point (the track's drones resource; Ent_63 is its view as a table entry). */
struct Spawn {
    s16 lap;                    /* the lap of the leading human rider it is for */
    s16 node;                   /* the path node that triggers it; -1 once used */
    s32 x;                      /* position, mm */
    s32 z;
    s16 speed;                  /* starting speed, mm/s */
    u16 timeScale;              /* 29491 = 1.0 */
};

/* Brings in a free extra rider (6 or 7) at a spawn point: on the ground, moving along the racing line, a CPU rider
   (isCpu 2) that keeps its own lane. */
void race_spawn_extra_rider(Spawn *spawn)
{
    Rider *rider;
    s32 startNode;
    s32 i, riderIdx;
    f32 lane;
    s32 timeScale;
    f32 y, k;
    Vec3f unused;

    if (gSaveData.options & SAVE_OPT_NO_DRONES) {
        return;
    }
    if (gEvent.flags & EVENT_DUEL) {
        return;
    }

    rider = &gRiders[6];
    startNode = spawn->node;
    spawn->node = -1;
    for (; rider < &gRiders[RIDER_COUNT]; rider++) {
        if (rider->inactive != 0) {
            break;
        }
    }
    if (rider == &gRiders[RIDER_COUNT]) {
        return;
    }

    rider->prevAnim = ANIM_NONE;
    rider->inactive = 0;
    rider->crashPhase = 0;
    rider->roughLanding = 0;
    rider->pendingCrash = 0;
    rider->crashState = CRASH_STATE_RIDING;
    rider->engine.overheated = 0;
    rider->jumpState = 0;
    rider->airFrames = 0;
    rider->wheelieTimer = 0;
    rider->crashTimer = 0;
    rider->engine.heat = 0.0f;
    rider->frontWheel.airTicks = 0;
    rider->rearWheel.airTicks = 0;
    rider->spawnFrame = gRaceFrame;
    rider->bike.pos.x = (f32)spawn->x / 1000.0f;
    rider->bike.pos.z = (f32)spawn->z / 1000.0f;
    rider->bike.pos.y = 0.0f;
    rider->bike.pos.y = bike_get_ground_height(&rider->bike.pos);
    for (i = 0; i < 20; i++) {
        rider->bike.contacts[i] = rider->bike.pos;
    }
    rider->segment = path_find_segment(&rider->bike.pos, startNode, 15, 0);
    rider->laneOffset = path_get_lateral_offset(&rider->bike.pos, rider->segment);
    rider->segmentT = gPathSegmentT;
    lane = path_get_lateral_offset(&rider->bike.pos, rider->segment);
    rider->laneOffset = lane;
    sExtraRiderLanes[rider - &gRiders[6]] = lane;
    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, -math_atan2f(gPathTangentZ, gPathTangentX));
    bike_place_on_ground(rider);
    quat_to_mtx3f(&rider->bike.right.x, rider->bike.quat);
    rider->bike.vel.x = rider->bike.fwd.x * ((f32)spawn->speed / 1000.0f);
    rider->bike.vel.y = rider->bike.fwd.y * ((f32)spawn->speed / 1000.0f);
    rider->bike.vel.z = rider->bike.fwd.z * ((f32)spawn->speed / 1000.0f);
    rider->frontWheel.suspLength = 0.0f;
    rider->rearWheel.suspLength = 0.0f;
    rider->bike.angVel.z = 0.0f;
    rider->bike.angVel.y = 0.0f;
    rider->bike.angVel.x = 0.0f;
    rider->bike.angAcc.x = 0.0f;
    rider->bike.acc.z = 0.0f;
    rider->bike.acc.y = 0.0f;
    rider->bike.acc.x = 0.0f;
    rider->localAngVel.x = 0.0f;
    rider->localAngVel.y = 0.0f;
    rider->localAngVel.z = 0.0f;
    rider->bike.restThreshold = 1.0f;
    rider->anim = ANIM_NONE;
    *(Blk *)rider->drawQuat = *(Blk *)rider->bike.quat;
    rider->bike.pos.y = bike_get_ground_height(&rider->bike.pos) + 0.0f + 0.5f
        - (gGame.mode == GAME_MODE_EXCITE_3D ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    rider->airFrames = 4;
    bike_update_wheel_contacts(rider);
    bike_update_wheel_contacts(rider);
    bike_update_suspension(rider);
    riderIdx = rider - gRiders;
    decal_end_track(riderIdx);
    cam_set_frozen(riderIdx, 0);
    timeScale = spawn->timeScale;
    rider->isCpu = 2; /* an extra rider */
    rider->bike.type = BODY_CPU_BIKE;
    rider->timeScale = (f32)timeScale / 29491.0f;
    bike_update_render_pose(rider);
    bike_apply_rider_stats(rider, 0);
}

/* Takes an extra rider out (returns 1) once it is 3 s old, at least 20 m behind or 50 m ahead of the player along
   the lap, 15 m away and more than 34 degrees off the player's heading; after 15 s its power drops to 0.2. */
s32 race_try_despawn_extra_rider(Rider *player, Rider *extra)
{
    s16 angle;
    s32 polarLen;
    s32 v;
    s32 age;
    s32 ret;
    f32 behind;
    f32 dx;
    f32 dz;

    if (extra->inactive != 0) {
        goto fail;
    }
    age = gRaceFrame - extra->spawnFrame;
    if (age >= FRAMES(180)) {
        goto ok;
    }
fail:
    return 0;
ok:
    if (age > FRAMES(900)) {
        extra->power = 0.2f;
    }
    behind = extra->lapDistLeft - player->lapDistLeft;
    if (behind < 20.0f && -50.0f < behind) {
        goto fail;
    }
    dx = extra->bike.pos.x - player->bike.pos.x;
    dz = extra->bike.pos.z - player->bike.pos.z;
    if (dx * dx + dz * dz < 225.0f) {
        goto fail;
    }
    math_to_polar((s32)(-dx * 1024.0f), (s32)(dz * 1024.0f), &angle, &polarLen);
    angle -= player->heading;
    if ((angle < 0 ? -angle : angle) < 0x1800) {
        goto fail;
    }
    ret = 1;
    extra->inactive = ret;
    return ret;
}

/* The spawn table: count entries sorted by lap. */
typedef struct { s16 lap; s16 node; u8 pad[12]; } Ent_63;
typedef struct { s32 count; Ent_63 entries[1]; } Tbl_4;

extern Tbl_4 *sExtraRiderSpawns_2;
extern Rider sExtraRiders[2];   /* gRiders[6] and [7] */

/* With the human rider in first or second place: spawns the extra riders whose node it reaches on their lap, and
   takes out those left behind. */
void race_update_extra_riders(Rider *player)
{
    Rider *extra;

    if (gGame.mode == GAME_MODE_TIME_TRIAL) {
        return;
    }
    if (sExtraRiderSpawns_2 != NULL && player->race.place < 2) {
        Ent_63 *spawn = sExtraRiderSpawns_2->entries;
        while (spawn->lap < player->race.lap) {
            spawn++;
        }
        while (spawn->lap == player->race.lap) {
            if (spawn >= &sExtraRiderSpawns_2->entries[sExtraRiderSpawns_2->count]) {
                break;
            }
            if (spawn->node == player->segment) {
                race_spawn_extra_rider(spawn);
            }
            spawn++;
        }
    }
    for (extra = sExtraRiders; extra < &sExtraRiders[2]; extra++) {
        race_try_despawn_extra_rider(player, extra);
    }
}

/* Animates the human riders' place display when their place changes (placeAnim) and has the announcer call out the
   new place (after the first 10 s, at most every 5 s per player). */
void race_announce_position_changes(void)
{
    Rider *rider;
    s32 place;
    s32 idx;

    if (gEvent.time < FRAMES(120)) {
        return;
    }
    for (rider = gRiders; rider < gRiders + gGame.numPlayers; rider++) {
        place = rider->race.finishPlace >= 0 ? rider->race.finishPlace : rider->race.place;
        if (place != rider->race.prevPlace) {
            if (rider->race.placeAnim > 0x180) {
                rider->race.placeAnim = 0x300 - rider->race.placeAnim;
            }
            rider->race.placeAnim -= 0x20;
            if (rider->race.placeAnim <= 0) {
                if (rider->isCpu == 0 && rider->race.finishPlace < 0) {
                    if (gRaceFrame - gAnnounceTimes[rider - gRiders] > FRAMES(300) && gEvent.time > FRAMES(600)) {
                        announce_place_change(rider - gRiders, place, rider->race.prevPlace);
                    }
                }
                rider->race.placeAnim = 1;
                rider->race.prevPlace = place;
            }
        } else {
            if (rider->race.placeAnim < 0x200) {
                rider->race.placeAnim += 0x10;
            }
        }
    }
}

/* Bubble-sorts sRidersByLapPos by distance to the finish line and gRiderStandings (the race order) by raceDist,
   leaving finished and inactive riders in place, then numbers the places. */
void race_sort_positions(void)
{
    u8 swapped;
    s32 i;
    Rider *tmp;

    do {
        swapped = 0;
        for (i = 0; i < RIDER_COUNT - 1; i++) {
            if (sRidersByLapPos[i]->lapDistLeft > sRidersByLapPos[i + 1]->lapDistLeft) {
                swapped = 1;
                tmp = sRidersByLapPos[i];
                sRidersByLapPos[i] = sRidersByLapPos[i + 1];
                sRidersByLapPos[i + 1] = tmp;
            }
        }
    } while (swapped);

    do {
        swapped = 0;
        for (i = 0; i < gEvent.numRiders - 1; i++) {
            if (gRiderStandings[i]->raceDist < gRiderStandings[i + 1]->raceDist
                && (gRiderStandings[i]->inactive | gRiderStandings[i + 1]->inactive) == 0
                && gRiderStandings[i]->race.finishPlace < 0) {
                swapped = 1;
                tmp = gRiderStandings[i];
                gRiderStandings[i] = gRiderStandings[i + 1];
                gRiderStandings[i + 1] = tmp;
            }
        }
    } while (swapped);

    for (i = 0; i < gEvent.numRiders; i++) {
        gRiderStandings[i]->race.place = i;
    }
    if (gEvent.time > FRAMES(120)) {
        race_announce_position_changes();
    }
}

/* Takes rider out of the race until it can come back behind the CPU rider one place ahead of place (an extra rider
   is just removed); the gap is ref's distance to that rider less 20 m. Not called. */
void race_queue_rider_behind(Rider *rider, Rider *ref, s32 place)
{
    s32 i;

    if (rider->isCpu == 2) { /* an extra rider */
        rider->inactive = 1;
        return;
    }
    if (gRiderStandings[place - 1]->isCpu == 1 && gRiderStandings[place - 1]->inactive == 0) {
        rider->inactive = 1;
        rider->raceDist = -1000000.0f;
        for (i = sRiderQueueBehindCount; i > 0; i--) {
            sRiderQueueBehind[i] = sRiderQueueBehind[i - 1];
        }
        sRiderQueueBehind[0].rider = rider - gRiders;
        sRiderQueueBehind[0].watch = gRiderStandings[place - 1];
        sRiderQueueBehind[0].gap = ref->raceDist - gRiderStandings[place - 1]->raceDist - 20.0f;
        sRiderQueueBehindCount++;
    }
}

/* The same with the CPU rider one place behind place. Not called. */
void race_queue_rider_ahead(Rider *rider, Rider *ref, s32 place)
{
    Rider *next;
    s32 i;

    if (rider->isCpu == 2) { /* an extra rider */
        rider->inactive = 1;
        return;
    }
    next = gRiderStandings[place + 1];
    if (next->isCpu == 1 && next->inactive == 0) {
        rider->inactive = 1;
        i = sRiderQueueAheadCount;
        rider->raceDist = 1000000.0f;
        while (i > 0) {
            sRiderQueueAhead[i] = sRiderQueueAhead[i - 1];
            i = i - 1;
        }
        /* rider - gRiders, written out (a multiplication by the inverse of 0x7E0) */
        *(s16 *)&sRiderQueueAhead[0] = (s16)(((s32)rider - (s32)gRiders) * -0x41041041 >> 5);
        sRiderQueueAhead[0].watch = (s32)gRiderStandings[place + 1];
        sRiderQueueAhead[0].gap = (gRiderStandings[place + 1]->raceDist - ref->raceDist) - 20.0f;
        sRiderQueueAheadCount = sRiderQueueAheadCount + 1;
    }
}

/* Brings back the first rider of a queue dist metres ahead of (or behind) ref along the racing line, on the lap
   that puts it there, and removes it from the queue. */
void race_reinsert_queued_rider(Rider *ref, s16 *queue, f32 dist, s32 *count)
{
    s32 lap;
    s32 seg;
    s32 n;
    s32 i;
    s32 lim;
    PathNode *node;

    lap = ref->lapAdjust + ref->race.lap;
    if (0.0f < dist) {
        seg = ref->segment;
        while (0.0f < dist) {
            {
                s32 off = seg * 0x6c; /* &gPathNodes[seg], written out */
                node = (PathNode *)(off + (s32)gPathNodes);
            }
            seg = node->next;
            dist = dist - node->length;
            if (seg == gPathFinishNode) {
                lap = lap + 1;
            }
        }
    } else {
        seg = ref->segment;
        while (dist < 0.0f) {
            {
                s32 off = seg * 0x6c;
                node = (PathNode *)(off + (s32)gPathNodes);
            }
            seg = node->prev;
            dist = dist + node->length;
            if (seg == gPathFinishNode) {
                lap = lap - 1;
            }
        }
    }
    gRiders[*queue].race.lap = lap;
    gRiders[*queue].lapAdjust = 0;
    gRiders[*queue].inactive = 0;
    gRiders[*queue].prevSegment = seg;
    bike_respawn(&gRiders[*queue]);
    n = *count - 1;
    *count = n;
    for (i = 0; i < *count; i++) {
        ((T3 *)queue)[i] = ((T3 *)queue)[i + 1];
    }
}

/* The field of the current gTracks record used here. */
typedef struct {
    u8 pad0[0x20];
    f32 cpuSpeed[4];            /* the CPU riders' time scale, by difficulty */
} Tune;

extern Tune *gCurTrack_33;

/* Sorts the positions, has the announcer call out places now and then, and rubber-bands the riders through their
   time scale; then the extra riders and the CPU riders' lanes. */
void race_update_riders(void)
{
    Rider *humans[4];
    Rider **lp;
    Rider *rider;
    s32 numCpu;
    s32 numHumans;
    f32 humanAvg;
    f32 cpuAvg;
    s32 i;
    s32 cpuRank;
    s32 now;
    s32 oldest;
    s32 player;
    f32 scale;
    f32 cpuLead;
    f32 diff;
    f32 trackScale;

    race_sort_positions();
    numCpu = 0;
    numHumans = 0;
    humanAvg = 0.0f;
    cpuAvg = 0.0f;
    for (i = 0; i < gEvent.numRiders; i++) {
        if (gRiders[i].inactive == 0) {
            switch (gRiders[i].isCpu) { /* the extra riders (2) are left out */
            case 0:
                humans[numHumans] = &gRiders[i];
                numHumans++;
                humanAvg += gRiders[i].raceDist;
                break;
            case 1:
                numCpu++;
                cpuAvg += gRiders[i].raceDist;
                break;
            }
        }
    }
    if (numHumans != 0) {
        now = gRaceFrame;
        if (now - gLastSpeechTime > FRAMES(900)) { /* 15 s */
            oldest = now;
            player = -1;
            if (numHumans >= 2) {
                for (i = 0; i < gGame.numPlayers; i++) {
                    if (gAnnounceTimes[i] < oldest) {
                        oldest = gAnnounceTimes[i];
                        player = i;
                    }
                }
                if (player >= 0) {
                    if (gRiders[player].lapDistLeft > 150.0f || (gRiders[player].race.place != 0 && gRiders[player].race.lap != gEvent.numLaps - 1)) {
                        announce_place(player, gRiders[player].race.place + 1);
                    }
                }
            } else {
                /* the player's place, every 20 s at most, else the place of the rider ahead or behind */
                rider = gRiders;
                if (!(now - gAnnounceTimes[0] > FRAMES(1200) && (rider->lapDistLeft > 150.0f || (rider->race.place != 0 && rider->race.lap != gEvent.numLaps - 1)))) {
                    if (rider->race.place != 0) {
                        rider = gRiderStandings[rider->race.place - 1];
                    } else {
                        rider = gRiderStandings[rider->race.place + 1];
                    }
                }
                announce_place(rider - gRiders, rider->race.place + 1);
            }
        }
        if (gEvent.time == FRAMES(300)) {
            announce_early_leader(gRiderStandings[0] - gRiders);
        }
    }
    if (numCpu != 0) {
        cpuAvg /= numCpu;
    }
    if (numHumans != 0) {
        humanAvg /= numHumans;
    } else {
        humanAvg = cpuAvg;
    }
    if (numCpu == 0) {
        cpuAvg = humanAvg;
    }
    if (gGame.flags & GAME_FLAG_RIDER_SELECT) {
        /* the race behind the rider select: the CPU riders spread out evenly around the lap */
        cpuRank = 0;
        for (i = 0; i < gEvent.numRiders; i++) {
            rider = gRiderStandings[i];
            if (rider->isCpu == 1) {
                diff = rider->raceDist - cpuAvg;
                diff += (gPathLapLength / numCpu) * (cpuRank - (numCpu - 1) * 0.5f);
                scale = 1.0f - diff / 100.0f;
                if (scale < 0.9f) {
                    scale = 0.9f;
                }
                if (scale > 1.1f) {
                    scale = 1.1f;
                }
                if (scale < rider->timeScale) {
                    rider->timeScale = rider->timeScale - 0.001f;
                } else {
                    rider->timeScale = rider->timeScale + 0.001f;
                }
                rider->timeScale = scale; /* the step above is overwritten */
                cpuRank++;
            }
        }
    } else if (numHumans == 0) {
        /* no human riders: the CPU riders keep together */
        cpuRank = 0;
        for (i = 0; i < gEvent.numRiders; i++) {
            rider = gRiderStandings[i];
            if (rider->isCpu == 1) {
                diff = rider->raceDist - cpuAvg;
                diff -= (cpuRank - (numCpu - 1) * 0.5f) * 0.01f;
                scale = 1.0f - diff * 0.00390625f;
                if (scale < 0.9f) {
                    scale = 0.9f;
                }
                if (scale > 1.1f) {
                    scale = 1.1f;
                }
                if (scale < rider->timeScale) {
                    rider->timeScale = rider->timeScale - 0.001f;
                } else {
                    rider->timeScale = rider->timeScale + 0.001f;
                }
                rider->timeScale = scale; /* likewise */
                cpuRank++;
            }
        }
    } else if (gGame.mode != GAME_MODE_TIME_TRIAL && gEvent.state >= 5) {
        /* after the countdown: CPU riders slow down as their pack leads the humans by more than 30 m and speed up as
           it falls behind, around the track's speed for the difficulty and their own skill; humans close up */
        cpuLead = ((cpuAvg - humanAvg) - 30.0f) * 0.00390625f;
        if (cpuLead > 0.15f) {
            cpuLead = 0.15f;
        } else if (cpuLead < -0.15f) {
            cpuLead = -0.15f;
        }
        for (i = 0; i < gEvent.numRiders; i++) {
            rider = &gRiders[i];
            if (rider->isCpu != 0) {
                scale = 0.0f;
                if (numCpu >= 2) {
                    scale = rider->aiSkill;
                }
                diff = rider->raceDist - cpuAvg;
                if (diff > 64.0f) {
                    scale -= 0.1f;
                }
                scale += 1.0f - cpuLead;
                if (rider->finishState) {
                    scale = 1.0f;
                } else if (gCheatFlags & CHEAT_BEAT_THIS) {
                    scale *= gCurTrack_33->cpuSpeed[2] + 0.05f;
                } else {
                    trackScale = gCurTrack_33->cpuSpeed[gGame.difficulty - 1];
                    scale *= trackScale;
                    if (gEvent.flags & EVENT_DUEL) {
                        scale = trackScale * 1.15f;
                    }
                }
                if (scale < rider->timeScale) {
                    rider->timeScale = rider->timeScale - 0.0002f;
                } else {
                    rider->timeScale = rider->timeScale + 0.0002f;
                }
            } else {
                diff = humanAvg - rider->raceDist;
                if (diff > 2.0f) {
                    diff += 16.0f;
                } else if (diff < -2.0f) {
                    diff -= 16.0f;
                }
                scale = diff * 0.0009765625f + 1.0f;
                if (scale > 1.05f) {
                    scale = 1.05f;
                } else if (scale < 0.95f) {
                    scale = 0.95f;
                }
                if (scale < rider->timeScale) {
                    rider->timeScale = rider->timeScale - 0.001f;
                } else {
                    rider->timeScale = rider->timeScale + 0.001f;
                }
            }
        }
    }
    if (gGame.numPlayers == 1 && numHumans == 1) {
        race_update_extra_riders(humans[0]);
    }
    ai_update_lanes();
}

/* ai_lane_is_blocked written out in place (with its own labels): result = 1 when a rider of the NULL-terminated
   list at scan is within 0.999 m of lateral offset x. */
#define NEAR(result, x, top, done)                                      \
    {                                                                   \
        Rider *q_;                                                      \
        f32 v_, k_;                                                     \
    top:                                                                \
        q_ = *scan;                                                     \
        if (q_->isCpu != 0) { v_ = q_->laneOffset; k_ = 0.999f; }       \
        else { v_ = q_->lateral; k_ = 0.999f; }                         \
        v_ = v_ - (x);                                                  \
        scan++;                                                         \
        if (v_ < k_ && (k_ = -k_) < v_) {                               \
            result = 1;                                                 \
            goto done;                                                  \
        }                                                               \
        if (*scan != NULL) {                                            \
            goto top;                                                   \
        }                                                               \
        result = 0;                                                     \
    done:;                                                              \
    }

/* Picks each CPU rider's lane (laneOffset from the racing line): within the track, clear of riders alongside,
   otherwise drifting back to the racing line. */
void ai_update_lanes(void)
{
    Rider *nearby[8];
    Rider *rider;
    Rider *other;
    Rider **out;
    PathNode *pathNode;
    s32 i;
    s32 j;
    s32 numNearby;
    s32 node;
    f32 lo;
    f32 hi;
    f32 ahead;
    f32 dx;
    f32 dz;
    f32 distSq;
    f32 otherLane;
    f32 lane;
    f32 delta;
    f32 best;
    f32 target;
    f32 len;
    f32 diff;
    u8 blocked;
    f32 otherLat;
    Rider **scan;
    f32 curLane;

    for (i = 0; i < RIDER_COUNT; i++) {
        rider = sRidersByLapPos[i];
        if (rider->isCpu == 0 || rider->inactive != 0) {
            continue;
        }
        if (rider >= sExtraRiders) {
            rider->laneOffset = sExtraRiderLanes[rider - sExtraRiders];
        }
        lo = -20.0f;
        hi = 20.0f;
        node = rider->segment;
        j = 6;
        do {
            pathNode = (PathNode *)(node * sizeof(PathNode) + (u32)gPathNodes);
            if (lo < -pathNode->widthNeg) {
                lo = -pathNode->widthNeg;
            }
            if (pathNode->widthPos < hi) {
                hi = pathNode->widthPos;
            }
            j--;
            node = pathNode->next;
        } while (j != 0);
        if (hi < rider->laneOffset) {
            rider->laneOffset = hi;
        }
        if (rider->laneOffset < lo) {
            rider->laneOffset = lo;
        }
        numNearby = 0;
        j = 0;
        rider->laneBlocked = 0;
        for (; j < RIDER_COUNT; j++) {
            if (i == j) {
                continue;
            }
            other = sRidersByLapPos[j];
            if (other->inactive != 0) {
                continue;
            }
            /* the riders from 1 m behind to 4 m ahead, within about 10 m */
            ahead = rider->lapDistLeft - other->lapDistLeft + 1.0f;
            if (ahead < 0.0f) {
                ahead += gPathLapLength;
            }
            if (ahead > 5.0f) {
                continue;
            }
            dx = other->bike.pos.x - rider->bike.pos.x;
            dz = other->bike.pos.z - rider->bike.pos.z;
            distSq = dx * dx + dz * dz;
            if (ahead * ahead + 100.0f < distSq) {
                continue;
            }
            nearby[numNearby] = other;
            numNearby++;
            if (distSq > 2.25f) {
                continue;
            }
            /* one alongside, within 1.5 m, bounds the lanes on its side */
            otherLat = other->lateral;
            diff = otherLat - rider->lateral;
            if (ahead < 2.0f && distSq < 2.25f) {
                if (diff > 0.0f) {
                    if (otherLat < hi) {
                        hi = otherLat;
                    }
                } else {
                    if (lo < otherLat) {
                        lo = otherLat;
                    }
                }
            }
        }
        nearby[numNearby] = NULL;
        if (numNearby != 0) {
            scan = nearby;
            curLane = rider->laneOffset;
            NEAR(blocked, curLane, top1, done1);
            if (blocked) {
                best = 2.0f;
                for (numNearby--; numNearby >= 0; numNearby--) {
                    other = nearby[numNearby];
                    if (other->isCpu != 0) {
                        otherLane = other->laneOffset;
                    } else {
                        otherLane = other->lateral;
                    }
                    lane = otherLane - 1.0f;
                    delta = rider->laneOffset - lane;
                    if ((delta > 0.0f ? delta : -delta) < best && lane < hi && lo < lane) {
                        scan = nearby;
                        NEAR(blocked, lane, top2, done2);
                        if (!blocked) {
                            target = lane;
                            best = rider->laneOffset - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                    lane = otherLane + 1.0f;
                    delta = rider->laneOffset - lane;
                    if ((delta > 0.0f ? delta : -delta) < best && lane < hi && lo < lane) {
                        scan = nearby;
                        NEAR(blocked, lane, top3, done3);
                        if (!blocked) {
                            target = lane;
                            best = rider->laneOffset - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                }
                if (best < 2.0f) {
                    rider->laneOffset = target;
                }
                continue;
            }
        }
        if (rider < sExtraRiders) {
            if (rider->laneOffset > 0.0f) {
                rider->laneOffset -= 0.02f;
            } else {
                rider->laneOffset += 0.02f;
            }
        }
    }
}

/* Spreads the CPU riders' skills evenly from -0.1 to 0.1 in sAiRiderOrder (then +0.01 for exactly 0.1, -0.01 for
   the others) and sets their time scale to 1.03 - skill. */
void race_init_ai_skills(s32 first, s32 count)
{
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

/* Shuffles the CPU riders' order a little: each gets the key 100 * position + two random bytes, then the order is
   bubble-sorted by key. */
void race_jitter_ai_order(s32 first, s32 n)
{
    s32 keys[8];
    s32 *key;
    s32 i;
    s32 base;
    s32 rand1;
    s32 rand2;
    u8 swapped;
    s32 tmp;

    i = 0;
    if (n > 0) {
        key = keys;
        base = i;
        do {
            *key = base;
            rand1 = random_byte();
            rand2 = random_byte();
            base += 100;
            i++;
            *key = *key + (rand1 + rand2);
            key++;
        } while (i < n);
    }
    do {
        swapped = 0;
        for (i = 0; i < n; i++) { /* original bug: the last test reads keys[n], never set */
            if (keys[i] > keys[i + 1]) {
                tmp = sAiRiderOrder[i];
                sAiRiderOrder[i] = sAiRiderOrder[i + 1];
                sAiRiderOrder[i + 1] = tmp;
                tmp = keys[i];
                keys[i] = keys[i + 1];
                keys[i + 1] = tmp;
                swapped = 1;
            }
        }
    } while (swapped);
}

/* The fields of the current gTracks record used here. */
typedef struct {
    u8 pad0[0x2C];
    s32 envType;                /* 0 stadium, 1 outdoor, 2 desert */
    u8 pad1[0x20];
    u32 flags;                  /* 2: the user track */
} Obj_46;

extern Obj_46 *gCurTrack_34;

/* Loads the track's extra-rider spawn table (the drones resource), except in the desert and on the user track. */
void race_load_extra_rider_spawns(s32 resSet)
{
    if (gCurTrack_34->envType != 2 && (gCurTrack_34->flags & 2) == 0) {
        sExtraRiderSpawnsIdx = track_res_find(gGidDrones);
        sExtraRiderSpawns = track_res_get(gGidDrones);
    }
}

/* Drops the spawn table. */
void race_free_extra_rider_spawns(void)
{
    if (sExtraRiderSpawns != 0) {
        sExtraRiderSpawns = 0;
    }
}

/* Sets the next race's grid: the riders alternate either side of the middle, so the human riders (the first ones)
   start in the middle positions. */
void race_set_grid_order(s32 numPlayers, s32 numCpu)
{
    s32 n = numPlayers + numCpu;
    s32 i;
    s32 half = n / 2;
    u8 *slot;

    if (n < 3) {
        u8 *order;
        sGridOrderNext[0] = 0;
        order = sGridOrderNext;
        order[1] = 1;
        return;
    }
    for (i = 0; i < n; i++) {
        slot = sGridOrderNext + i;
        if (i & 1) {
            *slot = half + i / 2;
        } else {
            *slot = half - (i / 2 + 1);
        }
    }
}

/* Resets the next race's grid to riders 0 and 1 in positions 0 and 1. */
void race_reset_grid_order(void)
{
    mem_fill(sGridOrderNext, 0, 8);
    sGridOrderNext[1] = 1;
}

/* Puts the count CPU riders (from rider first on) in random order in sAiRiderOrder, then sets their skills. */
void race_randomize_ai_order(s32 first, s32 count)
{
    s32 i, k;

    for (i = 0; i < count; i++) {
        sAiRiderOrder[i] = (((s32)random_byte() * count) >> 8) + first;
        for (k = 0; k < i; k++) {
            if (sAiRiderOrder[k] == sAiRiderOrder[i]) {
                i--;
                break;
            }
        }
    }
    race_init_ai_skills(first, count);
}

typedef struct {
    s16 rider;
    s16 rest[5];
} Ent12;

/* Puts the riders in index order in the standings and the lap order, and empties the rider queues. */
void race_init_standings(s32 count)
{
    s32 i = 0;
    s16 none = -1;
    Ent12 *queueAhead = sRiderQueueAhead;
    Ent12 *queueBehind = sRiderQueueBehind;
    Rider **standing = gRiderStandings;
    Rider **byLapPos = sRidersByLapPos;
    Rider *rider = gRiders;
    do {
        *byLapPos = rider;
        *standing = rider;
        queueBehind->rider = none;
        queueAhead->rider = none;
        queueAhead++;
        queueBehind++;
        standing++;
        byLapPos++;
        rider->standing = i;
        i++;
        rider++;
    } while (i < RIDER_COUNT);
    sRiderQueueAheadCount = 0;
    sRiderQueueBehindCount = 0;
}

typedef struct {
    s32 rider;
    Rider *watch;
    f32 gap;
} Ent_67;

extern Ent_67 sRiderQueueAhead_2;   /* the first entries */
extern Ent_67 sRiderQueueBehind_2;

/* Brings back the first rider of each queue once the rider it watches is within its gap of rider: 70 m ahead of
   rider, or 30 m behind. Not called. */
void race_update_rider_queues(Rider *rider)
{
    f32 a, b, gap;
    if (sRiderQueueAheadCount > 0) {
        b = sRiderQueueAhead_2.watch->raceDist;
        a = rider->raceDist;
        gap = b - a;
        if (gap < sRiderQueueAhead_2.gap) {
            race_reinsert_queued_rider(rider, &sRiderQueueAhead_2, 70.0f, &sRiderQueueAheadCount);
        }
    }
    if (sRiderQueueBehindCount > 0) {
        b = rider->raceDist;
        a = sRiderQueueBehind_2.watch->raceDist;
        gap = b - a;
        if (gap < sRiderQueueBehind_2.gap) {
            race_reinsert_queued_rider(rider, &sRiderQueueBehind_2, -30.0f, &sRiderQueueBehindCount);
        }
    }
}

/* True when a rider of the NULL-terminated list is within 0.999 m of lane (CPU riders by their lane, humans by their
   offset from the racing line). Not called: ai_update_lanes has it written out (NEAR). */
s32 ai_lane_is_blocked(Rider **list, f32 lane)
{
    f32 delta, width;
    Rider *other;

    do {
        other = *list;
        if (other->isCpu != 0) {
            delta = other->laneOffset;
            width = 0.999f;
            delta = delta - lane;
        } else {
            delta = other->lateral;
            width = 0.999f;
            delta = delta - lane;
        }
        list++;
        if (delta < width && -width < delta) {
            return 1;
        }
    } while (*list != 0);
    return 0;
}

/* Restores the grid saved by race_apply_grid_order (replays). */
void race_restore_grid_order(void)
{
    gGridOrder = sGridOrderSaved;
}

extern Blob6 sGridOrderNext_2;

/* Makes the next race's grid current, and saves a copy. */
void race_apply_grid_order(void)
{
    gGridOrder = sGridOrderNext_2;
    sGridOrderSaved = gGridOrder;
}

/* In a season, sets the next race's grid from the season standings. */
void race_grid_from_standings(void)
{
    s32 i;
    s32 j;
    s32 numRiders;
    u8 *season;
    Event *event;
    u8 *hb;

    mem_fill_3(sGridOrderNext, -1, 6);
    if (gGame.mode == GAME_MODE_SEASON) {
        Event *p = &gEvent;
        i = 0;
        if (p->numRiders > 0) {
            event = p;
            do {
                j = 0;
                if (event->numRiders > 0) {
                    season = (u8 *)gSeason;
                    hb = (u8 *)SYM_HI(gEvent);
                    __asm__ __volatile__("" : "+r"(hb)); /* keeps the original's base register */
                    numRiders = *(s16 *)(hb + SYM_LO(gEvent) + 0x22); /* gEvent.numRiders */
inner:
                    if (*(u8 *)((u32)season + (u32)j + 0x6c) == i) { /* Season.standings[j] */
                        goto store;
                    }
                    j++;
                    if (j < numRiders) {
                        goto inner;
                    }
                }
store:
                sGridOrderNext[i] = (u8)j;
                i++;
            } while (i < event->numRiders);
        }
    }
}
