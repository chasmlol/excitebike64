#include "game.h"

/* Excite 3D (game mode 6): a straight track along -z that repeats every 997.157 m, four lanes 2.178 m apart along x
   (lane n at x = n * 2.178, laneOffset = -x). The path nodes keep 2 bits of ground type per lane (path.c): 1 to
   leave, 2 to move onto, 3 to wheelie over. */

/* Parks the extra riders (6 and 7) out of the way and sets the riders' time scales. */
void excite3d_init(void)
{
    s32 i;

    sExcite3DSpawnTime = 600;
    gRaceFrame = 0;
    for (i = 6; i < RIDER_COUNT; i++) {
        gRiders[i].frozen = 1;
        gRiders[i].isCpu = 1;
        gRiders[i].inactive = 0;
        gRiders[i].bike.pos.x = -1000.0f;
        gRiders[i].bike.pos.y = -50.0f;
        gRiders[i].bike.pos.z = -1000.0f;
        bike_apply_rider_stats(&gRiders[i], 0);
        quat_from_euler(&gRiders[i].bike.pos.x + 12, 0.0f, 0.0f, 0.0f); /* bike.quat */
    }
    for (i = 0; i < RIDER_COUNT; i++) {
        gRiders[i].timeScale = sExcite3DTimeScales[i];
    }
}

/* Steers a CPU rider between the lanes by the ground codes up to 40 m ahead: changes lanes away from code 1 or onto
   code 2, and wheelies over code 3. */
void excite3d_cpu_steer(Rider *rider)
{
    s32 otherSeg;
    s32 seg;
    s32 lane;
    s32 otherLane;
    s32 code;
    s32 otherCode;
    s32 shift;
    s32 otherShift;
    PathNode *s;
    f32 targetX;
    f32 dx;
    f32 curX;
    f32 newX;

    otherSeg = rider->segment;
    seg = otherSeg;
    lane = (s32)((rider->bike.pos.x + 1.089f) / 2.178f);
    otherLane = lane ^ 1;
    shift = lane << 1;
    code = (gPathNodes[seg].speedTweak >> shift) & 3;
    if (code == 0) {
        for (;;) {
            seg++;
            {
                f32 dd = gPathNodes[seg].pos.z - rider->bike.pos.z;
                if (!(-40.0f < dd)) break;
            }
            code = (gPathNodes[seg].speedTweak >> shift) & 3;
            if (code != 0) break;
        }
    }
    otherShift = otherLane << 1;
    otherCode = (gPathNodes[otherSeg].speedTweak >> otherShift) & 3;
    if (otherCode == 0) {
        for (;;) {
            otherSeg++;
            {
                f32 dd = gPathNodes[otherSeg].pos.z - rider->bike.pos.z;
                if (!(-40.0f < dd)) break;
            }
            otherCode = (gPathNodes[otherSeg].speedTweak >> otherShift) & 3;
            if (otherCode != 0) break;
        }
    }
    if ((code == 1 && (otherCode != code || seg < otherSeg)) || otherCode == 2) {
        lane = otherLane;
        if (10.0f < rider->speed) {
            rider->input &= ~(INPUT_Z | INPUT_A);
        }
    }
    targetX = lane * 2.178f;
    curX = rider->bike.pos.x;
    dx = targetX - curX;
    rider->laneOffset = -targetX;
    if (0.21779999f < dx) { /* a tenth of a lane */
        newX = curX + rider->speed * gPhysDeltaTime / 10.0f;
    } else if (dx < -0.21779999f) {
        newX = curX - rider->speed * gPhysDeltaTime / 10.0f;
    } else {
        goto done;
    }
    rider->bike.pos.x = newX;
    rider->laneOffset = -newX;
done:
    if (rider->airFrames <= 0) {
        if (code == 3) {
            rider->wheelieTimer = 0x10;
            rider->wheelieBalance = 0;
            return;
        }
        if (rider->wheelieTimer != 0) {
            rider->wheelieTimer -= 1;
        }
    }
}

/* Excite 3D's frame: CPU lane changes, the race order, the endless track (rider 0, then the others, moved back a lap
   past its end), and the extra riders, brought back ahead beyond the next crest or 10 m behind once out of range. */
void excite3d_update(void)
{
    Rider *rider;
    Rider *a;
    Rider *b;
    Rider *player;
    PathNode *u;
    s16 m;
    Rider *racer;
    PathNode *t;
    s32 i;
    s32 j;
    s32 node;
    s16 n;
    u8 swapped;
    u8 numHumans;
    u8 r;
    s32 now;
    f32 playerZ;
    f32 crestY;
    f32 range;
    f32 gap;
    f32 nodeY;
    f32 behind;

    for (rider = gRiders; rider < &gRiders[RIDER_COUNT]; rider++) {
        if (rider->frozen == 0 && rider->isCpu != 0 && rider->crashState == CRASH_STATE_RIDING && rider->airFrames < 3) {
            excite3d_cpu_steer(rider);
        }
    }

    do {
        swapped = 0;
        for (i = 0; i < gEvent.numRiders - 1; i++) {
            a = gRiderStandings[i];
            b = gRiderStandings[i + 1];
            if (a->raceDist < b->raceDist && (a->inactive | b->inactive) == 0) {
                swapped = 1;
                gRiderStandings[i] = b;
                gRiderStandings[i + 1] = a;
            }
        }
    } while (swapped);

    for (i = 0; i < gEvent.numRiders; i++) {
        gRiderStandings[i]->race.place = i;
    }

    race_announce_position_changes();

    if (gRiders[0].bike.pos.z < -970.375f) {
        /* rider 0 and the camera go back a lap */
        gRiders[0].bike.pos.z -= -997.15717f;
        gRiders[0].drawPos.z = gRiders[0].drawPos.z - -997157.2f;
        gRiders[0].drawBodyPos.z = gRiders[0].drawBodyPos.z - -997157.2f;
        gRiders[0].bike.smoothPos.z -= -997.15717f;
        cam_shift_z(0, 0xF3725); /* 997157 mm */
        player = gRiders;
        j = 0;
        while (player->bike.pos.z < gPathNodes[j].pos.z) {
            j++;
        }
        player->segment = j;
        decal_end_track(0);
        event_rider_lap_completed(0);
    }

    /* the others count the lap at the end, and go back once rider 0 has (they are then 70 m ahead of it) */
    for (racer = &gRiders[1]; racer < &gRiders[RIDER_COUNT]; racer++) {
        if (racer->frozen == 0 && racer->bike.pos.z < -970.375f) {
            if (racer->lapAdjust == 0) {
                if (racer - gRiders < 4) { /* a player */
                    event_rider_lap_completed(racer - gRiders);
                }
                racer->lapAdjust = 1;
            }
            gap = racer->bike.pos.z - gRiders[0].bike.pos.z;
            if (gap < -70.0f) {
                racer->lapAdjust = 0;
                racer->bike.pos.z -= -997.15717f;
                racer->bike.smoothPos.z -= -997.15717f;
                {
                    s32 jj = 0;
                    t = gPathNodes;
                    while (racer->bike.pos.z < t[jj].pos.z) {
                        jj++;
                    }
                    racer->segment = jj;
                }
                decal_end_track(racer - gRiders);
            }
        }
    }

    numHumans = 0;
    for (racer = gRiders; racer < &gRiders[RIDER_COUNT]; racer++) {
        if (racer->isCpu == 0 && racer->inactive == 0) {
            numHumans++;
            playerZ = racer->bike.pos.z;
        }
    }

    if (numHumans != 0) {
        /* the range: up to the first node beyond a crest (the track rises 1.5 m above rider 0, then falls 1.5 m), or
           100 m; crestY is the height there */
        node = gRiders[0].segment + 4;
        range = gRiders[0].bike.pos.z - gPathNodes[node].pos.z;
        crestY = gRiders[0].bike.pos.y;
        if (!(range > 100.0f)) goto body;
        crestY = 4.0f;
        goto found;
        do {
        body:
            nodeY = gPathNodes[node].pos.y;
            if (crestY < nodeY) {
                crestY = nodeY;
            }
            if (crestY - gRiders[0].bike.pos.y > 1.5f && crestY - nodeY > 1.5f) {
                goto found;
            }
            node = (node + 1) % gPathNodeCount;
            range = gRiders[0].bike.pos.z - gPathNodes[node].pos.z;
        } while (!(range > 100.0f));
        crestY = 4.0f;
    found:
        behind = -10.0f;
        for (rider = &gRiders[6]; rider < &gRiders[RIDER_COUNT]; rider++) {
            if (!(crestY < rider->bike.pos.y) && gRaceFrame - sExcite3DSpawnTime > 60 && rider->isCpu != 0 &&
                gRaceFrame - rider->spawnFrame > 179 && rider->inactive == 0) {
                gap = playerZ - rider->bike.pos.z;
                if (gap < behind || range < gap) {
                    now = gRaceFrame;
                    sExcite3DSpawnTime = now;
                    if (now & 1) {
                        rider->power = 0.2f;
                        rider->bike.pos.z = playerZ - range;
                    } else {
                        rider->power = 0.2f;
                        rider->bike.pos.z = playerZ - behind;
                    }
                    collide_reset_body((u8 *)&rider->bike.pos.x);
                    rider->bike.type = BODY_CPU_BIKE;
                    rider->gridSlot = r250_next() & 3; /* a random lane */
                    rider->bike.pos.y = 40.0f;
                    rider->bike.pos.x = rider->gridSlot * 2.178f;
                    rider->bike.pos.y = bike_get_ground_height(&rider->bike.pos.x);
                    bike_place_on_ground(rider);
                    rider->speed = 10.0f;
                    rider->laneOffset = -rider->bike.pos.x;
                    rider->bike.vel.y = rider->speed * rider->bike.fwd.y;
                    rider->bike.vel.z = rider->speed * rider->bike.fwd.z;
                    rider->spawnFrame = gRaceFrame;
                    rider->bike.vel.x = 0.0f;
                    rider->blinkCount = 0;
                    rider->noCollide = 0;
                    rider->crashPhase = 0;
                    rider->jumpState = 0;
                    rider->airFrames = 0;
                    rider->roughLanding = 0;
                    rider->wheelieTimer = 0;
                    rider->pendingCrash = 0;
                    rider->crashTimer = 0;
                    rider->crashState = CRASH_STATE_RIDING;
                    rider->engine.heat = 0.0f;
                    rider->engine.overheated = 0;
                    rider->frozen = 0;
                    rider->anim = ANIM_NONE;
                    rider->prevAnim = ANIM_NONE;
                }
            }
        }
    }
}

/* A human rider's lane changes: the stick moves the bike one lane left or right on the ground when the lane is
   clear, over gLaneChangeFrames frames. */
void excite3d_player_steer(Rider *rider)
{
    if (gEvent.time <= 0) {
        return;
    }
    if (rider->laneChange != 0) {
        if (rider->laneChange > 0) {
            rider->laneChange--;
        } else {
            rider->laneChange++;
        }
    } else if (rider->airFrames < 0) {
        if (rider->stickX < -5) {
            if (rider->gridSlot != 0 && excite3d_lane_is_clear_2(rider, rider->gridSlot - 1)) {
                rider->gridSlot--;
                rider->laneChange = (s8)-gLaneChangeFrames;
            }
        } else if (rider->stickX >= 6 && rider->gridSlot < 3 && excite3d_lane_is_clear_2(rider, rider->gridSlot + 1)) {
            rider->gridSlot++;
            rider->laneChange = (s8)gLaneChangeFrames;
        }
    }
    rider->bike.vel.x = 0.0f;
    rider->bike.pos.x = rider->gridSlot * 2.178f - rider->laneChange * (2.178f / gLaneChangeFrames);
}

/* True when the rider can change to lane: not on a boost ramp, the ground there as high as in its own lane, and no
   boost ramp on the lane over the distance a lane change takes. */
s32 excite3d_lane_is_clear(Rider *rider, s32 lane)
{
    V3 target, probe;
    f32 zero, curX, curY, targetY, dir, step;
    s32 i;
    u32 boost;

    if (rider->rearWheel.surface->flags & SURFACE_BOOST_RAMP) {
        return 0;
    }
    target.x = (f32)lane * 2.178f;
    curX = (f32)rider->gridSlot * 2.178f;
    zero = 0.0f;
    target.y = zero;
    probe.y = zero;
    probe.x = curX;
    target.z = rider->bike.pos.z;
    probe.z = target.z;
    curY = bike_get_ground_height(&probe);
    targetY = bike_get_ground_height(&target);
    if (curY != targetY) {
ret0:
        return 0;
    }
    dir = 1.0f;
    if (zero < rider->bike.vel.z) {
        dir = -1.0f;
    }
    step = rider->bike.vel.z * gLaneChangeFrames / 60.0f - dir - dir;
    for (i = 0; i < 5; i++) {
        probe = target;
        probe.z += (f32)i * step * 0.25f + dir;
        bike_get_ground_height(&probe);
        if (gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & SURFACE_BOOST_RAMP) {
            goto ret0;
        }
    }
    boost = gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & SURFACE_BOOST_RAMP;
    return boost == 0;
}

/* The rider's path node (the last one it has passed), lateral offset and race distance. */
void excite3d_update_progress(Rider *rider)
{
    PathNode *node = gPathNodes;
    s32 seg = 0;
    f32 lapStart;
    if (rider->bike.pos.z < node->pos.z) {
        do {
            seg++;
            node++;
        } while (rider->bike.pos.z < node->pos.z);
    }
    lapStart = (f32)rider->race.lap * -997.15717f;
    rider->segment = seg;
    rider->lateral = -rider->bike.pos.x;
    rider->raceDist = -lapStart - rider->bike.pos.z;
}

/* The fields of Rider used below. */
struct Obj_55 {
    u8 pad0[0x50];
    f32 posZ;                   /* bike.pos.z */
    u8 pad1[0x628 - 0x54];
    s16 segment;
};

/* Sets the rider's path node from its z. */
void excite3d_find_segment(Obj_55 *rider)
{
    s16 seg = 0;
    PathNode *node = gPathNodes;
    if (rider->posZ < node->pos.z) {
        do {
            node++;
            seg++;
        } while (rider->posZ < node->pos.z);
    }
    rider->segment = seg;
}
