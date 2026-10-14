#include "game.h"

/* The fields of the current track (gTracks) that the bikes read. */
typedef struct {
    /* 0x00 */ u8 pad0[0x2C];
    /* 0x2C */ s32 envType;         /* 0 stadium, 1 outdoor, 2 desert */
    /* 0x30 */ u8 pad30[0x50 - 0x30];
    /* 0x50 */ u32 flags;           /* 2: the user track (no records, the UsrTrk.c overlay) */
} TrackView;

extern CamMode gCamModes[];
extern TrackView *gCurTrack_17;

/* Sets up the riders for a race: skeletons, sphere trees, camera modes, handling, places on the grid. */
void bikes_init(void)
{
    Vec3f unused[4]; /* sizes the stack frame */
    s32 i;
    s32 headingNode;
    s32 j;
    s32 cosIdx;
    s32 camMode;
    s32 gridPos;
    s32 rnd;
    s32 gridCount;
    s32 startNode;
    f32 lateral;
    f32 zero;
    f32 laneWidth;
    u16 heading;
    s32 magnitude;
    Rider *rider;
    Vec3f *unusedPtr;

    mem_fill_2(gPlayerInputs, 0, 0x10);
    if (gReplayPlayback == 0) {
        race_apply_grid_order();
    } else {
        race_restore_grid_order();
    }
    stunt_init_input_tables();
    i = 0;
    gAiRouteToggle = 0;
    D_800E76C0 = 0;
    gPhysTicks = 1;
    gCrashSlowMotion = 0;
    D_800E7700 ^= 1;
    lateral = 0.0f; /* one variable: zero here, the grid lane's offset below */
    gAiDebugTargetPos.z = lateral;
    gAiDebugTargetPos.y = lateral;
    gAiDebugTargetPos.x = lateral;
    gAiDebugTargetDir.z = lateral;
    gAiDebugTargetDir.y = lateral;
    gAiDebugTargetDir.x = lateral;
    collide_init_node_pool();
    race_load_extra_rider_spawns(gResourceSet);
    r250_init();
    D_800E7704 = 1;
    gRaceFrame = 0;
    for (; i < RIDER_COUNT; i++) {
        zero = 0.0f;
        gRiderAlpha[i] = 0xFF;
        gRiders[i].unk_778 = -60;
        if (gGhostRace != 0 || (gRiders[i].isCpu == 0 && (gCheatFlags & CHEAT_TRANSPARENT_RIDER))) {
            gRiders[i].noCollide = 1;
        }
        gRiders[i].bikeSkel = mem_alloc_dbg(sizeof(BikeSkeleton), 0, "bikenew.c", 233);
        gRiders[i].riderSkel = mem_alloc_dbg(sizeof(RiderSkeleton), 0, "bikenew.c", 234);
        *gRiders[i].bikeSkel = *gBikeSkelTemplate;
        *gRiders[i].riderSkel = *gRiderSkelTemplate;
        mem_fill_2(gRiders[i].trickList, -1, 0x10);
        mem_fill_2(gRiders[i].trickHist, -1, 6);
        engine_reset(&gRiders[i].engine);
        gRiders[i].speed = 0.0f;
        gRiders[i].bumpSeed = 0.0f;
        gRiders[i].queuedTrick = -1;
        gRiders[i].inactive = 1;
        gRiders[i].race.placeAnim = 0x100;
        gRiders[i].trickQuat[1] = zero;
        gRiders[i].trickQuat[2] = zero;
        gRiders[i].trickQuat[3] = zero;
        gRiders[i].trickOffset.x = zero;
        gRiders[i].trickOffset.y = zero;
        gRiders[i].trickOffset.z = zero;
        gRiders[i].anim = ANIM_NONE;
        gRiders[i].prevAnim = ANIM_NONE;
        gRiders[i].trickQuat[0] = 1.0f;
        gRiders[i].bike.vel.x = gRiders[i].bike.fwd.x * zero;
        gRiders[i].bike.vel.y = gRiders[i].bike.fwd.y * gRiders[i].speed;
        gRiders[i].bike.vel.z = gRiders[i].bike.fwd.z * gRiders[i].speed;
        rnd = random_byte();
        gRiders[i].bike.restThreshold = zero;
        gRiders[i].bike.flags = 1;
        gRiders[i].body.type = BODY_RIDER;
        gRiders[i].body.flags = 0;
        gRiders[i].bike.inertia = 250.0f;
        gRiders[i].bike.type = gRiders[i].isCpu != 0; /* BODY_BIKE or BODY_CPU_BIKE */
        gRiders[i].body.mass = 90.0f;
        gRiders[i].body.inertia = 45.0f;
        gRiders[i].headTimer = rnd % 150;
        if (gGame.mode == GAME_MODE_SOCCER) {
            gRiders[i].bike.tree = collide_build_sphere_tree(sBikeSphereTreeDescSoccer);
        } else {
            gRiders[i].bike.tree = collide_build_sphere_tree(sBikeSphereTreeDesc);
        }
        gRiders[i].body.tree = collide_build_sphere_tree(sRiderSphereDesc);
        gRiders[i].bike.owner = &gRiders[i];
        gRiders[i].body.owner = &gRiders[i];
        gRiders[i].frontWheel.surface = gSurfaceTable;
        gRiders[i].rearWheel.surface = gSurfaceTable;
        switch (gGame.mode) {
        case GAME_MODE_HILL_CLIMB:
            gRiders[i].camMode = CAM_MODE_HILL_CLIMB;
            break;
        case GAME_MODE_SOCCER:
            gRiders[i].camMode = CAM_MODE_SOCCER;
            break;
        case GAME_MODE_EXCITE_3D:
            if (i < gGame.numPlayers) {
                camMode = gGame.padCamMode[gGame.playerPad[i]];
            } else {
                camMode = CAM_MODE_CHASE_MID;
            }
            if (camMode != CAM_MODE_SIDE_VIEW && camMode != CAM_MODE_CHASE_MID) {
                camMode = CAM_MODE_CHASE_MID;
            }
            gRiders[i].camMode = camMode;
            if (i < gGame.numPlayers) {
                gGame.padCamMode[gGame.playerPad[i]] = camMode;
            }
            break;
        default:
            if (i < gGame.numPlayers) {
                camMode = gGame.padCamMode[gGame.playerPad[i]];
            } else {
                camMode = CAM_MODE_CHASE_MID;
            }
            if (gCamModes[camMode].flags & CAM_FLAG_NO_CYCLE) {
                camMode = CAM_MODE_CHASE_MID;
            }
            gRiders[i].camMode = camMode;
            if (i < gGame.numPlayers) {
                gGame.padCamMode[gGame.playerPad[i]] = camMode;
            }
            break;
        }
        mem_fill_2(gCamLastModes, -1, 0x18);
        gRiders[i].walkPhase = 0x3000;
        gRiders[i].savedCamMode = (u16)gRiders[i].camMode;
    }
    i = 0;
    if (gEvent.numRiders > 0) {
        do {
            rider = &gRiders[i];
            if (i < MAXCONTROLLERS) {
                gCamTargets[i] = i;
            }
            if (!(gGame.flags & (GAME_FLAG_FRONT_END_RACE | 0x80)) && gCreditsMode == 0 && i < gGame.numPlayers) {
                rider->isCpu = 0;
            } else {
                rider->isCpu = 1;
            }
            bike_apply_rider_stats(rider, gGame.riders[i]);
            rider->timeScale = 1.0f;
            rider->inactive = 0;
            rider->speed = 0.0f;
            switch (gGame.mode) {
            case GAME_MODE_SOCCER:
                /* two or four riders facing each other across the centre spot */
                if (gGame.numPlayers == 4) {
                    switch (i) {
                    case 0:
                        gRiders[i].bike.pos.x = -1.0f;
                        gRiders[i].bike.pos.z = -10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 0.0f);
                        break;
                    case 1:
                        gRiders[i].bike.pos.x = 1.0f;
                        gRiders[i].bike.pos.z = -10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 0.0f);
                        break;
                    case 2:
                        gRiders[i].bike.pos.x = 1.0f;
                        gRiders[i].bike.pos.z = 10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 3.1415927f);
                        break;
                    case 3:
                        gRiders[i].bike.pos.x = -1.0f;
                        gRiders[i].bike.pos.z = 10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 3.1415927f);
                        break;
                    }
                } else {
                    switch (i) {
                    case 0:
                        gRiders[i].bike.pos.x = 0.01f;
                        gRiders[i].bike.pos.z = -10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 0.00958738f);
                        break;
                    case 1:
                        gRiders[i].bike.pos.x = -0.015f;
                        gRiders[i].bike.pos.z = 10.0f;
                        quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 3.15118f);
                        break;
                    }
                }
                break;
            case 4:
            case GAME_MODE_DESERT:
                rider->bike.pos.x = sStartLineOffsets[i].x;
                rider->bike.pos.z = sStartLineOffsets[i].z;
                quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 0.0f);
                break;
            default:
                if ((gGame.flags & (GAME_FLAG_FRONT_END_RACE | GAME_FLAG_RIDER_SELECT)) == (GAME_FLAG_FRONT_END_RACE | GAME_FLAG_RIDER_SELECT)) {
                    /* behind the rider select: spread around the track */
                    startNode = i * gPathNodeCount / 6;
                } else if (gGame.mode == GAME_MODE_TEST_LOOP || gGame.mode == GAME_MODE_TEST_OPEN
                           || (gGame.mode == GAME_MODE_EXHIBITION && (gCurTrack_17->flags & 2))) {
                    startNode = 1;
                } else if (gGame.mode == GAME_MODE_TUTORIAL) {
                    startNode = gTutorialLessons[gSeason->round].startNode;
                } else {
                    startNode = gPathStartNode;
                }
                gridCount = gEvent.numRiders;
                if (gGhostRace != 0 && gridCount == 1) {
                    gridCount = 2;
                }
                if (gGame.flags & (GAME_FLAG_RIDER_SELECT | 0x80)) {
                    lateral = 0.0f;
                } else {
                    /* the grid: lanes laneWidth apart, centred on the middle of the track */
                    laneWidth = 1.5f;
                    lateral = (gPathNodes[startNode].widthPos - gPathNodes[startNode].widthNeg) * 0.5f;
                    gridPos = gGridOrder_2[i];
                    if (gCurTrack_17->flags & 2) {
                        laneWidth = 1.1f;
                    }
                    lateral += laneWidth * ((f32)gridPos - (f32)(gridCount - 1) * 0.5f);
                    rider->race.place = gridPos;
                    rider->race.prevPlace = gridPos;
                    rider->race.placeAnim = 0x100;
                }
                rider->segment = startNode;
                if (gGame.mode == GAME_MODE_HILL_CLIMB) {
                    rider->bestSegment = startNode;
                }
                headingNode = startNode;
                if (gGame.mode == GAME_MODE_TUTORIAL && gSeason->round == 5) {
                    headingNode = startNode + 1;
                }
                math_to_polar(gPathNodes[headingNode].lateralZ * 1024.0f, gPathNodes[headingNode].lateralX * 1024.0f, (s16 *)&heading, &magnitude);
                quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, (f32)(s16)heading * 3.1415927f * 3.0517578e-05f);
                if (gCurTrack_17->envType == 0 && !(gGame.flags & GAME_FLAG_RIDER_SELECT)) {
                    /* stadium: square to the nearest quarter turn */
                    heading = (heading + 0x1000) & ~0x3FFF;
                    quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, (f32)(s16)heading * 3.1415927f * 3.0517578e-05f);
                }
                if (gGame.mode != GAME_MODE_EXCITE_3D) {
                    /* gCosTable[a - 0x400] is the sine */
                    cosIdx = (s16)heading >> 4;
                    rider->bike.pos.x = gPathNodes[startNode].pos.x + (f32)gCosTable[cosIdx & 0xFFF] * lateral * 6.1035156e-05f
                             - (f32)gCosTable[(cosIdx - 0x400) & 0xFFF] * 1.5258789e-05f;
                    rider->bike.pos.z = gPathNodes[startNode].pos.z + (f32)gCosTable[(cosIdx - 0x400) & 0xFFF] * lateral * 6.1035156e-05f
                             + (f32)gCosTable[cosIdx & 0xFFF] * 1.5258789e-05f;
                    rider->laneOffset = lateral;
                } else {
                    rider->bike.pos.z = gPathNodes[startNode].pos.z;
                    rider->bike.pos.x = (f32)i * 2.178f;
                    rider->laneOffset = (f32)-i * 2.178f;
                }
                rider->gridSlot = i;
                break;
            }
            bike_place_on_ground(rider);
            bike_update_suspension(rider);
            bike_update_render_pose(rider);
            rider->heading = (u16)rider->drawRot[2];
            for (j = 0; j < BIKE_CONTACT_COUNT; j++) {
                gRiders[i].bike.contacts[j] = gRiders[i].bike.pos;
            }
            i++;
        } while (i < gEvent.numRiders);
    }
    race_init_standings(RIDER_COUNT);
    bike_set_contact_friction(0.5f);
    sBikeMaxReverseSpeed = -5.0f;
    race_init_ai_skills(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    switch (gGame.mode) {
    case GAME_MODE_SOCCER:
        sBikeMaxReverseSpeed = -20.0f;
        soccer_ball_reset(1);
        break;
    case GAME_MODE_DESERT:
        desert_init();
        break;
    case GAME_MODE_HILL_CLIMB:
        bike_set_contact_friction(0.3f);
        break;
    case GAME_MODE_EXCITE_3D:
        sBikeMaxReverseSpeed = 0.0f;
        bike_set_contact_friction(0.6f);
        excite3d_init();
        break;
    }
    sBikeTuneMenu = 1;
}

/* A handling profile of sRiderStatsTable (8 profiles). */
typedef struct {
    /* 0x00 */ f32 landing;
    /* 0x04 */ f32 handling;
    /* 0x08 */ f32 airTurn;
    /* 0x0C */ f32 airFloat;
    /* 0x10 */ f32 turnRadius;
    /* 0x14 */ f32 turbo;
    /* 0x18 */ f32 weight;
    /* 0x1C */ f32 wallFriction;
    /* 0x20 */ f32 power;
    /* 0x24 */ f32 unk_24;
} RiderStats; /* size 0x28 */

extern RiderStats sRiderStatsTable[];

/* Copies a handling profile into a human rider's stats; CPU riders get the standard ones (and its weight). */
void bike_apply_rider_stats(Rider *rider, s32 profile)
{
    if (rider->isCpu == 0) {
        rider->bike.mass = 250.0f;
        rider->landing = sRiderStatsTable[profile].landing;
        /* reads turnRadius before it is set from the profile below */
        rider->handling = sRiderStatsTable[profile].handling * (sBikeTune[14] * sBikeTune[5]) * 32.0f * rider->turnRadius;
        rider->airTurn = sRiderStatsTable[profile].airTurn * sBikeTune[12] * 3.1415927f * 0.00390625f;
        rider->airFloat = sRiderStatsTable[profile].airFloat / 48.0f;
        rider->turnRadius = sRiderStatsTable[profile].turnRadius * 0.57f;
        rider->turbo = sRiderStatsTable[profile].turbo;
        rider->weight = sRiderStatsTable[profile].weight;
        rider->wallFriction = sRiderStatsTable[profile].wallFriction;
        rider->power = sRiderStatsTable[profile].power;
        return;
    }
    rider->bike.mass = 250.0f;
    rider->wallFriction = 0.3f;
    rider->landing = 1.0f;
    rider->turbo = 1.0f;
    {
        f32 turnRadius = 0.57f;
        f32 airTurn = sBikeTune[12] * 3.1415927f * 0.00390625f;
        rider->turnRadius = turnRadius;
        rider->airFloat = 0.020833334f;
        rider->airTurn = airTurn;
        rider->handling = sBikeTune[14] * sBikeTune[5] * 32.0f * turnRadius;
    }
    if (gGame.mode == 15) {
        rider->power = 0.7f;
    } else {
        rider->power = 1.0f;
    }
    rider->weight = sRiderStatsTable[profile].weight;
}

/* Finds where the line through pos along the bike's up axis meets the ground (steps, then bisects); -2 off the
   ground mesh. The dead tests and the copies of the scale only keep the original's register use. */
f32 bike_wheel_ground_bisect(Rider *rider, f32 *pos, f32 tolerance)
{
    f32 dist, step, startDiff, probeY, upY, groundY, diff, mmPerM, mmPerM2, mmPerM3, limit;
    f32 probe[3];
    f32 probeX, posY, posY2, upY2, upZ, posZ;
    f32 *probePtr;
    u8 *dummy;
    u8 *dummy2;
    f32 *probePtr2;
    s32 groundMm, stepInt, distInt;

    dist = 0.0f;
    step = -1.0f;
    if ((s32)rider < step) groundMm = 1; else groundMm = 2;
    if (rider) groundMm = 1; else if (pos) groundMm = 2; else groundMm = 3;
    groundY = rider->bike.pos.y;
    probe[1] = groundY;
    if (pos[0] < groundY) groundMm = 1; else groundMm = 3;
    mmPerM = 1000.0f;
    groundMm = height_get_ground((s32)(pos[0] * mmPerM), (s32)(pos[1] * mmPerM), (s32)(pos[2] * mmPerM));
    startDiff = (f32)groundMm / mmPerM - pos[1];
    if (0.0f < startDiff) step = 1.0f;
    probePtr = probe;
    dummy = (u8 *)&gGame;
    limit = 10000.0f;
    mmPerM2 = mmPerM;
    do {
        dist += step;
        probeX = pos[0] + dist * rider->bike.up.x;
        probe[0] = probeX;
        upY = dist * rider->bike.up.y;
        posY = pos[1];
        probe[2] = pos[2] + dist * rider->bike.up.z;
        do { } while (0);
        probeY = posY + upY;
        if (dummy) groundMm = 1; else groundMm = 2;
        if (probeX < probeY) groundMm = 1; else groundMm = 2;
        groundMm = height_get_ground((s32)(probePtr[0] * mmPerM2), (s32)(probePtr[1] * mmPerM2), (s32)(probePtr[2] * mmPerM2));
        groundY = (f32)groundMm / mmPerM2;
        if (limit < groundY) groundMm = 3; else groundMm = 4;
        if (step < groundY) groundMm = 18; else groundMm = 19;
        if (groundY < step) groundMm = 20; else groundMm = 21;
        if (startDiff < groundY) groundMm = 22; else groundMm = 23;
        if (gGroundFound == 0) {
            rider->pendingCrash = -ANIM_CRASH_FRONT;
            return -2.0f;
        }
        diff = groundY - probeY;
    } while (0.0f < startDiff * diff);
    if (0.0f < startDiff) step = -step;
    limit = 0.0f;
    dummy2 = (u8 *)&gGame;
    startDiff = 10000.0f;
    step *= 0.5f;
    mmPerM3 = 1000.0f;
    do {
        probePtr2 = probe;
        if (limit < diff) dist -= step;
        else dist += step;
        probeX = pos[0] + dist * rider->bike.up.x;
        probe[0] = probeX;
        step *= 0.5f;
        upY2 = dist * rider->bike.up.y;
        upZ = dist * rider->bike.up.z;
        posZ = pos[2];
        do { } while (0);
        probePtr2 = probe;
        posY2 = pos[1];
        probe[2] = posZ + upZ;
        do { } while (0);
        probeY = posY2 + upY2;
        distInt = (s32)dist;
        stepInt = (s32)step;
        if (distInt == stepInt) groundY = dist;
        if (dummy2) { groundY = probeY; groundMm = 1; } else { groundY = probeX; groundMm = 2; }
        groundMm = height_get_ground((s32)(probePtr2[0] * mmPerM3), (s32)(probePtr2[1] * mmPerM3), (s32)(probePtr2[2] * mmPerM3));
        groundY = (f32)groundMm / mmPerM3;
        if (startDiff < groundY) groundMm = 3; else groundMm = 4;
        diff = groundY - probeY;
        if (limit < diff) {
            if (!(tolerance < diff)) return dist;
        } else {
            if (!(tolerance < -diff)) return dist;
        }
    } while (step < -0.000625f);
    return dist;
}

/* Finds the ground under a wheel fwdOffset metres along the bike: its surface, normal, travel and contact point. */
void bike_wheel_find_ground(Rider *rider, Wheel *wheel, f32 fwdOffset, u8 coarse)
{
    f32 axisPos[3];
    Vec3f point;
    f32 normal[4];
    f32 groundY;
    f32 dist, diff, pointY, tolerance, slope, prevDist, prevDiff;
    s32 iter;

    gGroundHeight = wheel->contact.y * 1000.0f;
    axisPos[0] = rider->bike.pos.x + rider->bike.fwd.x * fwdOffset;
    axisPos[1] = rider->bike.pos.y + rider->bike.fwd.y * fwdOffset;
    axisPos[2] = rider->bike.pos.z + rider->bike.fwd.z * fwdOffset;
    dist = wheel->groundDist;
    gGroundArea = wheel->groundArea;
    point.x = axisPos[0] + dist * rider->bike.up.x;
    pointY = axisPos[1] + dist * rider->bike.up.y;
    point.z = axisPos[2] + dist * rider->bike.up.z;
    iter = 0;
    point.y = rider->bike.pos.y;
    groundY = (f32)height_get_ground(point.x * 1000.0f, point.y * 1000.0f, point.z * 1000.0f) / 1000.0f;
    diff = groundY - pointY;
    tolerance = 0.002f;
    slope = -1.0f;
    if (coarse) tolerance = 0.1f;
    if (rider->rearWheel.airTicks > 0 && (diff < 0.0f || 0.5f < dist)) tolerance = 0.05f;
    while ((0.0f < diff) ? (tolerance < diff) : (tolerance < -diff)) {
        prevDist = dist;
        prevDiff = diff;
        dist -= slope * diff;
        if (dist < -32.0f || 8.0f < dist) {
            dist = bike_wheel_ground_bisect(rider, axisPos, tolerance);
            break;
        }
        point.x = axisPos[0] + dist * rider->bike.up.x;
        pointY = axisPos[1] + dist * rider->bike.up.y;
        point.z = axisPos[2] + dist * rider->bike.up.z;
        groundY = (f32)height_get_ground(point.x * 1000.0f, point.y * 1000.0f, point.z * 1000.0f) / 1000.0f;
        diff = groundY - pointY;
        if (iter++ >= 9) {
            if (iter >= 17) {
                dist = wheel->groundDist - 0.5f;
                break;
            }
            height_get_ground_normal(normal);
            if (-0.2f < vec3_dot(normal, &rider->bike.up.x)) {
                slope = (dist - prevDist) / ((diff - prevDiff) * 2.0f);
            } else {
                slope = -1.0f;
            }
        } else {
            slope = (dist - prevDist) / (diff - prevDiff);
        }
    }
    dist += 0.5f;
    point.y = groundY;
    if (gGroundFound) {
        wheel->surface = (Surface *)((u8 *)gSurfaceTable + ((gGroundSurface << 2) & 0x3C0)); /* bits 4-7 */
        wheel->groundFlags = gGroundSurface;
        wheel->groundArea = gGroundArea;
        height_get_ground_normal(&wheel->normal.x);
        if (0.25f < wheel->suspDelta && 0.5f <= wheel->normal.y && iter >= 17) {
            wheel->suspDelta = 0.0f;
            rider->pendingCrash = ANIM_CRASH_BACK;
            rider->crashVel = rider->bike.vel;
            return;
        }
    } else {
        wheel->normal.x = wheel->normal.z = 0.0f;
        wheel->normal.y = 1.0f;
    }
    if (wheel->groundDist < wheel->suspLength) wheel->suspDelta = dist - wheel->suspLength;
    else wheel->suspDelta = dist - wheel->groundDist;
    wheel->groundDist = dist;
    wheel->contactOffset.x = point.x - rider->bike.pos.x;
    wheel->contactOffset.y = point.y - rider->bike.pos.y;
    wheel->contactOffset.z = point.z - rider->bike.pos.z;
    wheel->contactDelta.x = point.x - wheel->contact.x;
    wheel->contactDelta.y = point.y - wheel->contact.y;
    wheel->contactDelta.z = point.z - wheel->contact.z;
    wheel->contact = point;
}

/* Finds the ground under both wheels; a wheel on a drop (SURFACE_FALL), or the bike high above one, crashes. */
void bike_update_wheel_contacts(Rider *rider)
{
    f32 height = 1.0f;
    s32 surface;
    s32 airFrames;
    s32 area;

    do {
        area = rider->rearWheel.groundArea;
        airFrames = rider->airFrames;
    } while (0);
    gGroundTri = 0;
    gGroundSurface = 0;
    gGroundArea = area;
    if (airFrames > 0) {
        s32 groundMm;
        airFrames = (s32)&rider->bike.pos; /* the variable reused as a pointer, and the [0] index below, match */

        groundMm = height_get_ground((s32)(((f32 *)airFrames)[(u8)rider->pendingCrash >> 8] * 1000.0f), (s32)(((f32 *)airFrames)[1] * 1000.0f), (s32)(((f32 *)airFrames)[2] * 1000.0f));

        height = rider->bike.pos.y - (f32)groundMm / 1000.0f;
        surface = (D_800E84F4[1] >> 4) & 0xF; /* D_800E84F4[1] is gGroundSurface */
        *(volatile s32 *)&rider->surfaceIdx = surface;
        if ((gGame.mode == GAME_MODE_HILL_CLIMB || gCurTrack == &gTracks[TRACK_GOLDMINE_RUSH]) && height > 2.0f && (gSurfaceTable[surface].flags & SURFACE_FALL)) {
            bike_queue_crash_2(rider, -ANIM_CRASH_BACK);
            return;
        }
    } else {
        rider->surfaceIdx = rider->rearWheel.surface - gSurfaceTable;
    }
    if (height < 1.0f) {
        if (rider->frontWheel.groundDist < -2.0f || rider->rearWheel.groundDist < -2.0f) {
            rider->frontWheel.groundDist = 0.0f;
            rider->rearWheel.groundDist = 0.0f;
        }
    }
    bike_wheel_find_ground(rider, &rider->frontWheel, 0.6f, height > 1.5f);
    bike_wheel_find_ground(rider, &rider->rearWheel, -0.65f, height > 1.5f);
    if (rider->frontWheel.surface->flags & SURFACE_FALL) {
        rider->pendingCrash = ANIM_CRASH_FRONT;
        rider->crashVel = rider->bike.vel;
    }
    if (rider->rearWheel.surface->flags & SURFACE_FALL) {
        rider->pendingCrash = ANIM_CRASH_FRONT;
        rider->crashVel = rider->bike.vel;
    }
}

/* The suspension's spring (N/m) and damper (N s/m); Excite 3D takes them from the tuning table. */
#define SUSP_SPRING (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneSpringRate * 20000.0f : 15000.0f)
#define SUSP_DAMPER (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneDamping * 2000.0f : 2200.0f)

/* A wheel's suspension force (spring and damper); none in the air, part of it on the tick of touchdown. */
void bike_wheel_spring_force(Wheel *wheel, f32 preload)
{
    f32 gap, frac, springTerm, damperTerm;

    if (wheel->airTicks > 0) {
        wheel->springForce = 0.0f;
        gap = wheel->groundDist - wheel->suspLength;
        if (wheel->springForce < gap) { /* the 0.0f just stored, read back */
            wheel->airTicks = 0;
            frac = gap / (gap - wheel->airGap);
        }
        wheel->airGap = gap;
    } else {
        frac = 1.0f;
    }

    if (wheel->airTicks <= 0) {
        springTerm = (wheel->groundDist + 0.05f + wheel->suspDelta * 0.5f - preload) * (SUSP_SPRING * sSuspSpringScale);
        damperTerm = wheel->suspDelta / gPhysDeltaTime * SUSP_DAMPER * sSuspDampScale;
        wheel->springForce = springTerm + damperTerm;
        if (wheel->springForce < 0.0f) {
            wheel->springForce = 0.0f;
            wheel->airTicks = gPhysTicks;
            wheel->airGap = wheel->suspDelta;
        } else {
            wheel->airTicks -= gPhysTicks;
            wheel->suspLength = wheel->groundDist;
        }
        wheel->springForce *= frac;
    } else {
        wheel->suspLength = wheel->suspLength - wheel->suspLength * (SUSP_SPRING / SUSP_DAMPER) * gPhysDeltaTime;
        wheel->airTicks += gPhysTicks;
    }
}

/* Runs both wheels' springs; a spring force beyond what the speed allows crashes the rider. */
void bike_update_suspension(Rider *rider)
{
    f32 mass;
    f32 frontShare;
    f32 one, scaleFrac, rearLoad, frontLoad, rearPreloadForce, frontPreloadForce, frontPreload, rearPreload, rearShare;

    mass = rider->bike.mass;
    frontShare = mass * 0.65f;
    rearShare = mass * 0.6f;
    frontLoad = ((frontShare / 1.25f) - 11.5199995f) * GRAVITY;
    rearLoad = ((rearShare / 1.25f) + 11.5199995f) * GRAVITY;
    one = 1.0f;
    scaleFrac = one - one / sSuspSpringScale;
    rearPreloadForce = scaleFrac * rearLoad;
    if (gGame.mode == GAME_MODE_EXCITE_3D) {
        rearPreload = rearPreloadForce / (gBikeTuneSpringRate * 20000.0f);
    } else {
        rearPreload = rearPreloadForce / 15000.0f;
    }
    frontPreloadForce = scaleFrac * frontLoad;
    if (gGame.mode == GAME_MODE_EXCITE_3D) {
        frontPreload = frontPreloadForce / (gBikeTuneSpringRate * 20000.0f);
    } else {
        frontPreload = frontPreloadForce / 15000.0f;
    }
    bike_wheel_spring_force(&rider->frontWheel, frontPreload);

    if (((rider->speed + 5.0f) * 250.0f * 30.0f) < rider->frontWheel.springForce) {
        rider->frontWheel.groundDist = 0.0f;
        rider->rearWheel.groundDist = 0.0f;
        bike_crash_begin(rider, ANIM_CRASH_FRONT);
    }
    bike_wheel_spring_force(&rider->rearWheel, rearPreload);
    if (((rider->speed + 5.0f) * 250.0f * 30.0f) < rider->rearWheel.springForce) {
        rider->frontWheel.groundDist = 0.0f;
        rider->rearWheel.groundDist = 0.0f;
        bike_crash_begin(rider, ANIM_CRASH_BACK);
    }
    if (3675.0002f < rider->frontWheel.springForce || 3675.0002f < rider->rearWheel.springForce) {
        /* reversed: hardLandFrame - gRaceFrame is never above 120 within a race */
        if (0x78 < rider->hardLandFrame - gRaceFrame) {
            bike_crash_begin(rider, ANIM_CRASH_FRONT);
        }
    } else {
        rider->hardLandFrame = gRaceFrame;
    }
    if (rider->frontWheel.airTicks == gPhysTicks || rider->rearWheel.airTicks == gPhysTicks) {
        bike_sync_wheel_contacts(rider);
    }
}

/* The least load a tire works with (N): less while R is held. */
#define MIN_TIRE_LOAD(rider) (((rider)->input & INPUT_R) ? 250.0f : 1000.0f)

/* The rear tire's force: rolling within the grip limit, sliding beyond it; spins the wheel up or down. */
void bike_rear_tire_force(Rider *rider, Wheel *wheel)
{
    f32 forceDir[3];
    f32 vel[3];
    f32 vec[3];
    f32 baseGrip, lonSpeed, tuneGrip;
    f32 forceLen, spinDiff, latSpeed, tractionRatio, forceMag, slipSpeed, forceSq, normalDot, latTerm, lonTerm, normalForce, load;

    vec[0] = wheel->contact.x - rider->bike.pos.x;
    vec[1] = wheel->contact.y - rider->bike.pos.y;
    vec[2] = wheel->contact.z - rider->bike.pos.z;
    vel[0] = rider->bike.angVel.y * vec[2] - vec[1] * rider->bike.angVel.z;
    vel[1] = rider->bike.angVel.z * vec[0] - vec[2] * rider->bike.angVel.x;
    vel[2] = rider->bike.angVel.x * vec[1] - vec[0] * rider->bike.angVel.y;
    vel[0] += rider->bike.vel.x;
    vel[1] += rider->bike.vel.y;
    vel[2] += rider->bike.vel.z;
    latSpeed = vec3_dot(vel, &wheel->latAxis.x);
    lonSpeed = vec3_dot(vel, &wheel->lonAxis.x);
    baseGrip = sBikeTune[10];
    wheel->grip = baseGrip * wheel->surface->grip;
    tuneGrip = sBikeTune[10];
    if (rider->input & INPUT_Z) {
        tractionRatio = sTurboTraction / tuneGrip;
    } else {
        tractionRatio = sBikeTune[11] / tuneGrip;
    }
    if (rider->powerSlide == 1) {
        wheel->grip = wheel->grip / 3.0f;
        tractionRatio = 1.0f;
    }
    latTerm = wheel->latForce;
    latTerm = latTerm * latTerm;
    lonTerm = wheel->lonForce;
    lonTerm = lonTerm * lonTerm;
    forceSq = latTerm + lonTerm;
    forceMag = __builtin_sqrtf(forceSq);
    if (forceMag > 0.0f) {
        wheel->grip = wheel->grip * (forceSq / (latTerm + lonTerm / tractionRatio));
    }
    normalDot = vec3_dot(&wheel->normal.x, &rider->bike.up.x);
    normalForce = wheel->springForce - vec3_dot(&wheel->force.x, &rider->bike.up.x);
    normalForce = normalForce / normalDot;
    load = normalForce;
    if (normalForce > 4000.0f) {
        load = 4000.0f;
    }
    if (load < MIN_TIRE_LOAD(rider)) {
        load = 1000.0f;
        if (rider->input & INPUT_R) {
            load = 250.0f;
        }
    }
    if (forceMag < load * wheel->grip || gGame.mode == 15) {
        vec[0] = wheel->normal.x * normalForce;
        vec[1] = wheel->normal.y * normalForce;
        vec[2] = wheel->normal.z * normalForce;
        wheel->force.x += vec[0];
        wheel->force.y += vec[1];
        wheel->force.z += vec[2];
        wheel->slip = 0.0f;
        wheel->spin = lonSpeed;
        return;
    }
    lonTerm = lonSpeed - wheel->spin;
    slipSpeed = __builtin_sqrtf(lonTerm * lonTerm + latSpeed * latSpeed);
    wheel->slip = slipSpeed;
    if (slipSpeed > 2.0f) {
        lonTerm = -lonTerm / slipSpeed;
        vec[0] = wheel->lonAxis.x * lonTerm;
        vec[1] = wheel->lonAxis.y * lonTerm;
        vec[2] = wheel->lonAxis.z * lonTerm;
        latSpeed = -latSpeed / slipSpeed;
        forceDir[0] = wheel->latAxis.x * latSpeed;
        forceDir[1] = wheel->latAxis.y * latSpeed;
        forceDir[2] = wheel->latAxis.z * latSpeed;
        forceDir[0] += vec[0];
        forceDir[1] += vec[1];
        forceDir[2] += vec[2];
        latTerm = latSpeed * latSpeed;
        lonTerm = lonTerm * lonTerm;
        wheel->grip = (baseGrip * (latTerm + lonTerm)) / (latTerm + lonTerm / tractionRatio);
    } else {
        *(Vec3f *)forceDir = wheel->force;
        forceLen = __builtin_sqrtf(forceDir[0] * forceDir[0] + forceDir[1] * forceDir[1] + forceDir[2] * forceDir[2]);
        if (forceLen != 0.0f) {
            forceDir[0] /= forceLen;
            forceDir[1] /= forceLen;
            forceDir[2] /= forceLen;
        }
    }
    load = normalDot + vec3_dot(forceDir, &rider->bike.up.x) * wheel->grip;
    if (load < 0.5f) {
        load = 0.5f;
    }
    normalForce = wheel->springForce / load;
    if (normalForce < 0.0f) {
        normalForce = 0.0f;
    }
    load = normalForce;
    if (normalForce > 4000.0f) {
        load = 4000.0f;
    }
    if (load < MIN_TIRE_LOAD(rider)) {
        load = 1000.0f;
        if (rider->input & INPUT_R) {
            load = 250.0f;
        }
    }
    forceMag = load;
    forceMag *= wheel->grip;
    wheel->force.x = normalForce * wheel->normal.x + forceMag * forceDir[0];
    wheel->force.y = normalForce * wheel->normal.y + forceMag * forceDir[1];
    wheel->force.z = normalForce * wheel->normal.z + forceMag * forceDir[2];
    wheel->spin += ((rider->driveForce - wheel->brakeTorque) - vec3_dot(&wheel->force.x, &wheel->lonAxis.x)) / 9765.625f;
    if (wheel->spin < 0.0f) {
        wheel->spin = 0.0f;
    }
    spinDiff = wheel->spin - lonSpeed;
    if (spinDiff > 10.0f) {
        wheel->spin = lonSpeed + 10.0f;
    } else if (spinDiff < -10.0f) {
        wheel->spin = lonSpeed - 10.0f;
    }
}

/* The ground height (m) under pos. The dead first test keeps the original's register use. */
static inline f32 getHeight(Vec3f *pos)
{
    s32 groundMm;
    if (((s32)pos + 1) * 2 != (s32)pos->x) groundMm = ((s32 *)pos)[12]; else groundMm = ((s32 *)pos)[13];
    groundMm = height_get_ground((s32)(pos->x * 1000.0f), (s32)(pos->y * 1000.0f), (s32)(pos->z * 1000.0f));
    return (f32)groundMm / 1000.0f;
}

/* A secant search along dir from origin for the ground: returns the distance and writes the ground normal. */
f32 bike_raycast_ground(Vec3f *origin, Vec3f *dir, f32 dist, f32 unusedTolerance, f32 *normal)
{
    Vec3f point;
    Vec3f offset;
    f32 groundY, diff, prevDiff, prevDist, slope, tolerance;
    s32 iter;

    offset.x = dir->x * dist;
    offset.y = dir->y * dist;
    offset.z = dir->z * dist;
    point.x = offset.x + origin->x;
    point.y = offset.y + origin->y;
    point.z = offset.z + origin->z;
    iter = 0;
    groundY = getHeight(&point);
    diff = groundY - point.y;
    tolerance = 0.002f;
    slope = -1.0f;
    while ((diff > 0.0f ? diff : -diff) > tolerance) {
        prevDist = dist;
        dist -= slope * diff;
        offset.x = dir->x * dist;
        offset.y = dir->y * dist;
        offset.z = dir->z * dist;
        point.x = offset.x + origin->x;
        point.y = offset.y + origin->y;
        point.z = offset.z + origin->z;
        prevDiff = diff;
        groundY = getHeight(&point);
        diff = groundY - point.y;
        slope = (dist - prevDist) / (diff - prevDiff);
        if (iter++ >= 11) {
            point.y = groundY;
            tolerance *= 2.0f;
        }
    }
    height_get_ground_normal(normal);
    return dist;
}

/* Puts a bike on the ground at its static ride height, pitched to the slope under its wheels. */
void bike_place_on_ground(Rider *rider)
{
    f32 normal[8];
    f32 offset[7];
    f32 dist;
    f32 wheelDiff;
    s32 tries;
    s32 pitch;

    rider->drawRot[0] = 0;
    rider->drawRot[1] = 0;
    quat_to_mtx3f(&rider->bike.right.x, rider->bike.quat);
    quat_to_angles(rider->bike.quat, &rider->drawRot[0], &rider->drawRot[1], &rider->drawRot[2]);
    dist = bike_raycast_ground(&rider->bike.pos, &rider->bike.up, 0.0f, 0.002f, normal);
    offset[0] = rider->bike.up.x * -((dist + 0.5f) - rider->bike.mass * GRAVITY / (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    offset[1] = rider->bike.up.y * -((dist + 0.5f) - rider->bike.mass * GRAVITY / (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    offset[2] = rider->bike.up.z * -((dist + 0.5f) - rider->bike.mass * GRAVITY / (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    rider->bike.pos.x -= offset[0];
    rider->bike.pos.y -= offset[1];
    rider->bike.pos.z -= offset[2];
    tries = 16; /* already 16: the loop runs once */
    do {
        quat_to_angles(rider->bike.quat, &rider->drawRot[0], &rider->drawRot[1], &rider->drawRot[2]);
        quat_to_mtx3f(&rider->bike.right.x, rider->bike.quat);
        bike_update_wheel_contacts(rider);
        pitch = (rider->frontWheel.groundDist - rider->rearWheel.groundDist) / 2.5f * 32768.0f / 3.1415927f;
        if (pitch > 0x1000) pitch = 0x1000;
        if (pitch < -0x1000) pitch = -0x1000;
        rider->drawRot[0] -= pitch;
        quat_from_euler(rider->bike.quat, (f32)rider->drawRot[0] * 3.1415927f * 3.0517578e-05f,
                      (f32)rider->drawRot[1] * 3.1415927f * 3.0517578e-05f,
                      (f32)rider->drawRot[2] * 3.1415927f * 3.0517578e-05f);
        wheelDiff = rider->frontWheel.groundDist - rider->rearWheel.groundDist;
    } while ((0.0f < wheelDiff ? 0.02f < wheelDiff : 0.02f < -wheelDiff) && tries++ < 16);
    bike_update_wheel_contacts(rider);
    dist = (((rider->frontWheel.groundDist + rider->rearWheel.groundDist) * 0.5f) - rider->bike.mass * GRAVITY / (gGame.mode == GAME_MODE_EXCITE_3D ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    offset[0] = rider->bike.up.x * -dist;
    offset[1] = rider->bike.up.y * -dist;
    offset[2] = rider->bike.up.z * -dist;
    rider->bike.pos.x -= offset[0];
    rider->bike.pos.y -= offset[1];
    rider->bike.pos.z -= offset[2];
}

#define FABS_GT(x, e) ((0.0f < (x)) ? ((e) < (x)) : ((e) < -(x)))
/* How far a wheel's contact point moved this tick. */
#define ROLLED_DIST(wheel) __builtin_sqrtf((wheel)->contactDelta.x * (wheel)->contactDelta.x + (wheel)->contactDelta.y * (wheel)->contactDelta.y + (wheel)->contactDelta.z * (wheel)->contactDelta.z)

/* Adds the surface's bumps and noise, eased over the distance rolled, to a wheel's ground distance. */
void bike_wheel_surface_bumps(Wheel *wheel, f32 *seed)
{
    f32 dist, bump, noise, blend, diff, decay;

    noise = 0.0f;
    dist = -1.0f;
    diff = wheel->bump - wheel->surface->bumpHeight;
    if (FABS_GT(diff, 0.001f)) {
        dist = ROLLED_DIST(wheel);
        blend = math_exp(-dist);
        wheel->bump = bump = wheel->bump * blend + (1.0f - blend) * wheel->surface->bumpHeight;
    } else {
        bump = wheel->bump = wheel->surface->bumpHeight;
    }
    if (FABS_GT(wheel->bumpNoise, 0.001f) || wheel->surface->bumpNoise != 0.0f) {
        if (dist < 0.0f) {
            dist = ROLLED_DIST(wheel);
        }
        noise = (f32)(s32)(math_sin_rand(seed) - 0x80) * 0.00390625f;
        decay = -dist * wheel->surface->bumpRate;
        noise *= wheel->surface->bumpNoise;
        blend = math_exp(decay);
        noise = blend * wheel->bumpNoise + (1.0f - blend) * noise;
        wheel->bumpNoise = noise;
    }
    wheel->suspDelta += bump + noise;
    wheel->groundDist += bump + noise;
}

/* Brake torques: B ramps them up over 6 ticks, split by the wheels' loads. They hold the bike during the start
   countdown and push forwards when rolling backwards. */
void bike_update_brakes(Rider *rider)
{
    f32 frontLoad;
    f32 frontShare, val, rearLoad;
    s16 ramp;
    /* val is the total load, then the ramp's top, then the forward speed */

    frontShare = 0.5f;
    frontLoad = rider->frontWheel.springForce;
    rearLoad = rider->rearWheel.springForce;
    /* zeroed as words: the float compare below reads the field again */
    *(u32 *)&rider->rearWheel.brakeTorque = 0;
    val = frontLoad + rearLoad;
    *(u32 *)&rider->frontWheel.brakeTorque = 0;
    if (rider->rearWheel.brakeTorque < val) {
        frontShare = frontLoad / val;
    }
    if ((rider->input & INPUT_B) != 0) {
        val = 6.0f;
        ramp = rider->frontWheel.brakeRamp + 1;
        rider->frontWheel.brakeRamp = ramp;
        if (val < (f32)ramp) {
            rider->frontWheel.brakeRamp = 6;
        }
        if (rider->frontWheel.airTicks < 1) {
            rider->frontWheel.brakeTorque = ((f32)rider->frontWheel.brakeRamp * 2572.5f * frontShare) / val;
        }
        ramp = rider->rearWheel.brakeRamp + 1;
        rider->rearWheel.brakeRamp = ramp;
        if (val < (f32)ramp) {
            rider->rearWheel.brakeRamp = 6;
        }
        if (rider->rearWheel.airTicks < 1) {
            rider->rearWheel.brakeTorque = ((f32)rider->rearWheel.brakeRamp * 2572.5f * (1.0f - frontShare)) / val;
        }
    } else {
        rider->frontWheel.brakeRamp = 0;
        rider->rearWheel.brakeRamp = 0;
    }
    val = rider->fwdSpeed;
    if (0.0f < val) {
        rider->reversing = 0;
    } else {
        rider->reversing = 1;
    }
    if ((gEvent.flags & EVENT_STOPPED) == 0) {
        if ((rider->input & (INPUT_Z | INPUT_A)) != 0 && val < 0.0f) {
            f32 torque, rearTorque;
            torque = frontShare * -2572.5f;
            rearTorque = -2572.5f - torque;
            rider->frontWheel.brakeTorque = torque;
            rider->rearWheel.brakeTorque = rearTorque;
            return;
        }
        if (rider->wheelieTimer == 0) {
            if (val < sBikeMaxReverseSpeed) {
                goto oppose_motion;
            }
            return;
        }
        if (!(val < 0.0f)) {
            return;
        }
oppose_motion:
        rider->frontWheel.brakeTorque = -rider->frontWheel.brakeTorque;
        rider->rearWheel.brakeTorque = -rider->rearWheel.brakeTorque;
        return;
    }
    {
        f32 torque, twoDt;
        torque = val * 250.0f;
        twoDt = gPhysDeltaTime + gPhysDeltaTime;
        rider->walkPhase = 0x3000;
        torque = torque / twoDt;
        rider->frontWheel.brakeTorque = torque;
        rider->rearWheel.brakeTorque = torque;
        return;
    }
}

/* The rear wheel's drive force from the engine torque (engine braking when it is negative or just after R). */
void bike_update_drive_force(Rider *rider)
{
    f32 torque, speed, force, rise;

    torque = rider->engine.torque;
    speed = rider->fwdSpeed;
    if (torque < 0.0f || (rider->rHeld >= 1 && rider->rHeld <= 4)) {
        if (0.0f < speed) {
            rider->driveForce = rider->bike.mass * -2.0f;
        } else {
            rider->driveForce = 0.0f;
        }
        return;
    }
    if (speed <= 1.0f) {
        speed = 1.0f;
    }
    force = (torque * rider->power) / speed;
    rider->driveForce = force;
    if (rider->frontWheel.airTicks < 0 && rider->rearWheel.airTicks < 0 && speed < 10.0f) {
        rise = rider->frontWheel.contact.y - rider->rearWheel.contact.y;
        if (0.15f < rise) {
            if (gGame.mode == GAME_MODE_HILL_CLIMB) {
                if (rider->stickY >= 0) {
                    return;
                }
                rider->driveForce = force * (1.0f - (rise - 0.15f) * (rider->stickY * 0.125f));
            } else {
                rider->driveForce = force * ((rise - 0.15f) * 2.0f + 1.0f);
            }
        }
    }
}

/* Turns the input word into stickX/stickY and the turbo, R, hard turn, power slide and dirt spray states. */
void bike_process_input(Rider *rider, s32 ticks)
{
    f32 stickX;
    f32 stickY;
    f32 lean;
    s32 input;
    s32 airTicks;
    s16 yaw;
    s32 yawWord;
    s32 tmp;

    input = rider->input;
    stickY = (s32)((u32)input >> INPUT_STICK_Y_SHIFT);
    if (input & INPUT_STICK_DOWN) {
        stickY = -stickY;
    }
    stickX = (input >> INPUT_STICK_X_SHIFT) & 0xF;
    if (input & INPUT_STICK_RIGHT) {
        stickX = -stickX;
    }
    if (rider->camMode == CAM_MODE_SIDE_VIEW) {
        /* the side view turns the stick a quarter turn */
        tmp = stickX;
        stickX = stickY;
        stickY = -tmp;
    }
    if ((u32)(rider->airFrames + 14) < 15) { /* landed in the last 14 frames */
        airTicks = rider->rearWheel.airTicks;
        if (airTicks < rider->frontWheel.airTicks) {
            airTicks = rider->frontWheel.airTicks;
        }
        if (airTicks >= -6) {
            stickX *= 0.25f;
        }
    }
    if (gGame.mode == GAME_MODE_EXCITE_3D) {
        /* rider 0's heading, whichever rider this is */
        yawWord = (u16)gRiders[0].drawRot[2] - 0x8000;
        yaw = yawWord;
        if (yaw > 0x2000 && 0.0f < stickX) {
            stickX = 0.0f;
            rider->bike.angVel.y = stickX;
        } else if (yaw < -0x2000 && stickX < 0.0f) {
            stickX = 0.0f;
            rider->bike.angVel.y = stickX;
        }
        if (rider->airFrames <= 0) {
            if (yaw > 0x4000) {
                goto respawn;
            }
            if (yaw < -0x4000) {
respawn:
                bike_respawn(rider);
            }
        }
    }
    if (rider->airFrames < 0) {
        if (rider->input & INPUT_C_DOWN) {
            stickX = 0.0f;
        }
    } else if ((rider->input & INPUT_C_DOWN) || (u8)rider->anim < ANIM_TRICK_COUNT) {
        stickY = 0.0f;
        stickX *= 0.25f;
    }
    if (rider->input & INPUT_Z) {
        if (!(rider->prevInput & INPUT_Z)) {
            rider->turboPressFrame = gRaceFrame;
        }
    } else if (rider->prevInput & INPUT_Z) {
        rider->turboReleaseFrame = gRaceFrame;
    }
    if ((rider->input & INPUT_R) && rider->powerSlide == 0) {
        if (rider->rHeld >= 0) {
            rider->rHeld++;
        } else {
            rider->rHeld = 0;
        }
    } else {
        if (rider->rHeld < 0) {
            rider->rHeld--;
        } else {
            rider->rHeld = -1;
        }
    }
    if (rider->wheelieTimer == 0) {
        if (stickY < -8.0f && rider->airFrames < 0 && (8.0f < stickX || stickX < -8.0f)) {
            rider->hardTurn = 1;
            if (0.0f < stickX) {
                stickX = 15.0f;
            } else {
                stickX = -15.0f;
            }
        } else {
            rider->hardTurn = 0;
        }
        if (rider->isCpu == 0) {
            if ((0.0f < stickX) ? (10.0f <= stickX) : (10.0f <= -stickX)) {
                if ((rider->input & (INPUT_R | INPUT_B)) == (INPUT_R | INPUT_B) && 4.0f < rider->speed && (s8)rider->anim != ANIM_NONE) {
                    rider->powerSlide = 1;
                    goto slide_done;
                }
            }
            if (rider->powerSlide == 1) {
                rider->powerSlide = 2;
            } else {
                rider->powerSlide = 0;
            }
        }
slide_done:
        if (rider->rHeld >= -14 && rider->airFrames < 0) {
            lean = (rider->driveForce - rider->rearWheel.brakeTorque - rider->frontWheel.brakeTorque) * 15.625f / 1323.0001f + 4.0f;
            if (13.0f < lean) {
                lean = 13.0f;
            }
            if (lean < 0.0f) {
                lean = 0.0f;
            }
            stickY = (s32)lean;
        }
    }
    rider->stickX = (s32)stickX;
    rider->stickY = (s32)stickY;
    if (rider->rearWheel.airTicks < 0) {
        rider->dirtSpray = 0;
        if (rider->powerSlide != 0) {
            goto spray_hard;
        }
        if (rider->input & INPUT_Z) {
            if (gRaceFrame - rider->turboPressFrame < 30) {
spray_hard:
                rider->dirtSpray = 2;
            } else {
                rider->dirtSpray = 1;
            }
        } else if (rider->input & INPUT_R) {
            if ((0.0f < stickX) ? (8.0f < stickX) : (8.0f < -stickX)) {
                if (5.0f < rider->speed) {
                    rider->dirtSpray = 1;
                }
            }
        }
    }
end:
    rider_update_input_history(rider, ticks);
}

/* sBikeTune[10] and [11] */
typedef struct {
    /* 0x00 */ u8 pad[0x28];
    /* 0x28 */ f32 grip;            /* the tires' grip scale */
    /* 0x2C */ f32 lonGrip;         /* the rear tire's lengthways grip (sTurboTraction with turbo) */
} BikeTuneView;

extern BikeTuneView sBikeTune_2;

/* The tire forces: bike_rear_tire_force for the rear, the same model for the front (where R adds 30% grip). */
void bike_update_tire_forces(Rider *rider)
{
    Vec3f forceDir;
    Vec3f vel;
    Vec3f vec;
    f32 lonSpeed;
    f32 tractionRatio;
    f32 latSpeed, baseGrip, normalShare, forceMag, latTerm, lonTerm, forceSq, normalForce, load, slipSpeed, unused1, normalDot, forceLen, grip, unused2, spinDiff;

    if (rider->rearWheel.airTicks <= 0) {
        rider->rearWheel.lonForce = rider->driveForce - rider->rearWheel.brakeTorque;
        bike_rear_tire_force(rider, &rider->rearWheel);
    } else {
        rider->rearWheel.slip = rider->driveForce = rider->rearWheel.brakeTorque = rider->rearWheel.force.x = rider->rearWheel.force.y = rider->rearWheel.force.z = 0.0f;
    }
    if (rider->frontWheel.airTicks <= 0) {
        vec.x = rider->frontWheel.contact.x - rider->bike.pos.x;
        vec.y = rider->frontWheel.contact.y - rider->bike.pos.y;
        vec.z = rider->frontWheel.contact.z - rider->bike.pos.z;
        vel.x = rider->bike.angVel.y * vec.z - vec.y * rider->bike.angVel.z;
        vel.y = rider->bike.angVel.z * vec.x - vec.z * rider->bike.angVel.x;
        vel.z = rider->bike.angVel.x * vec.y - vec.x * rider->bike.angVel.y;
        vel.x += rider->bike.vel.x;
        vel.y += rider->bike.vel.y;
        vel.z += rider->bike.vel.z;
        latSpeed = vec3_dot(&vel.x, &rider->frontWheel.latAxis.x);
        lonSpeed = vec3_dot(&vel.x, &rider->frontWheel.lonAxis.x);
        baseGrip = sBikeTune_2.grip;
        grip = baseGrip * rider->frontWheel.surface->grip;
        tractionRatio = sBikeTune_2.lonGrip / baseGrip;
        rider->frontWheel.grip = grip;
        if (rider->powerSlide == 1) {
            rider->frontWheel.grip = grip / 3.0f;
            tractionRatio = 1.0f;
        } else if (rider->input & INPUT_R) {
            rider->frontWheel.grip = grip * 1.3f;
        }
        latTerm = rider->frontWheel.latForce;
        latTerm *= latTerm;
        lonTerm = rider->frontWheel.lonForce;
        lonTerm *= lonTerm;
        forceSq = latTerm + lonTerm;
        forceMag = __builtin_sqrtf(forceSq);
        if (forceMag > 0.0f) {
            rider->frontWheel.grip = rider->frontWheel.grip * (forceSq / (latTerm + lonTerm / tractionRatio));
        }
        normalDot = vec3_dot(&rider->frontWheel.normal.x, &rider->bike.up.x);
        normalForce = rider->frontWheel.springForce - vec3_dot(&rider->frontWheel.force.x, &rider->bike.up.x);
        normalForce /= normalDot;
        load = normalForce;
        if (load > 4000.0f) load = 4000.0f;
        if (load < MIN_TIRE_LOAD(rider)) load = MIN_TIRE_LOAD(rider);
        if (forceMag < load * rider->frontWheel.grip || gGame.mode == 15) {
            vec.x = rider->frontWheel.normal.x * normalForce;
            vec.y = rider->frontWheel.normal.y * normalForce;
            vec.z = rider->frontWheel.normal.z * normalForce;
            rider->frontWheel.force.x += vec.x;
            rider->frontWheel.force.y += vec.y;
            rider->frontWheel.force.z += vec.z;
            rider->frontWheel.slip = 0.0f;
            rider->frontWheel.spin = lonSpeed;
        } else {
            lonTerm = lonSpeed - rider->frontWheel.spin;
            slipSpeed = __builtin_sqrtf(lonTerm * lonTerm + latSpeed * latSpeed);
            rider->frontWheel.slip = slipSpeed;
            if (slipSpeed > 2.0f) {
                lonTerm = -lonTerm / slipSpeed;
                vec.x = rider->frontWheel.lonAxis.x * lonTerm;
                vec.y = rider->frontWheel.lonAxis.y * lonTerm;
                vec.z = rider->frontWheel.lonAxis.z * lonTerm;
                latSpeed = -latSpeed / slipSpeed;
                latTerm = latSpeed * latSpeed;
                forceDir.x = rider->frontWheel.latAxis.x * latSpeed;
                lonTerm = lonTerm * lonTerm;
                forceDir.y = rider->frontWheel.latAxis.y * latSpeed;
                forceDir.z = rider->frontWheel.latAxis.z * latSpeed;
                forceDir.y += vec.y;
                forceDir.x += vec.x;
                forceDir.z += vec.z;
                rider->frontWheel.grip = baseGrip * (latTerm + lonTerm) / (latTerm + lonTerm / tractionRatio);
            } else {
                forceDir = *(Vec3f *)&rider->frontWheel.force.x;
                forceLen = __builtin_sqrtf(forceDir.x * forceDir.x + forceDir.y * forceDir.y + forceDir.z * forceDir.z);
                if (forceLen != 0.0f) {
                    forceDir.x /= forceLen;
                    forceDir.y /= forceLen;
                    forceDir.z /= forceLen;
                }
            }
            normalShare = normalDot + vec3_dot(&forceDir.x, &rider->bike.up.x) * rider->frontWheel.grip;
            if (normalShare < 0.5f) normalShare = 0.5f;
            normalForce = rider->frontWheel.springForce / normalShare;
            if (normalForce < 0.0f) normalForce = 0.0f;
            load = normalForce;
            if (load > 4000.0f) load = 4000.0f;
            if (load < MIN_TIRE_LOAD(rider)) load = MIN_TIRE_LOAD(rider);
            forceMag = load;
            forceMag *= rider->frontWheel.grip;
            rider->frontWheel.force.x = normalForce * rider->frontWheel.normal.x + forceMag * forceDir.x;
            rider->frontWheel.force.y = normalForce * rider->frontWheel.normal.y + forceMag * forceDir.y;
            rider->frontWheel.force.z = normalForce * rider->frontWheel.normal.z + forceMag * forceDir.z;
            rider->frontWheel.spin += (-vec3_dot(&rider->frontWheel.force.x, &rider->frontWheel.lonAxis.x) - rider->frontWheel.brakeTorque) / 9765.625f;
            if (rider->frontWheel.spin < 0.0f) rider->frontWheel.spin = 0.0f;
            spinDiff = rider->frontWheel.spin - lonSpeed;
            if (spinDiff > 10.0f) {
                rider->frontWheel.spin = lonSpeed + 10.0f;
            } else if (spinDiff < -10.0f) {
                rider->frontWheel.spin = lonSpeed - 10.0f;
            }
        }
    } else {
        rider->frontWheel.slip = rider->frontWheel.brakeTorque = rider->frontWheel.force.x = rider->frontWheel.force.y = rider->frontWheel.force.z = 0.0f;
    }
}

#define CROSS(r, a, b) \
    (r).x = (a).y * (b).z - (b).y * (a).z; \
    (r).y = (a).z * (b).x - (b).z * (a).x; \
    (r).z = (a).x * (b).y - (b).x * (a).y

#define NORM(v) { \
    f32 len = __builtin_sqrtf((v).x * (v).x + (v).y * (v).y + (v).z * (v).z); \
    if (len != 0.0f) { (v).x /= len; (v).y /= len; (v).z /= len; } }

/* Sets the wheels' axes and solves for the sideways tire forces that stop both contact points sliding sideways. */
void bike_solve_tire_constraints(Rider *rider)
{
    Vec3f steerDir, steerLocal, axis, c1, c2, rearArm, c3, c4, frontVel, rearVel, frontTorque, rearTorque, newVel, newAngVel;
    f32 rearLatVel, frontLatVel, dtOverInertia, A, B, C, D, latDot, frontLat, rearLat, det, along, sideForce, lonSpeed, inv;

    rider->rearWheel.force.z = 0.0f;
    rider->rearWheel.force.y = 0.0f;
    rider->rearWheel.force.x = 0.0f;
    rider->frontWheel.force.z = 0.0f;
    rider->frontWheel.force.y = 0.0f;
    rider->frontWheel.force.x = 0.0f;
    steerLocal.x = cosf(rider->steer);
    steerLocal.y = 0.0f;
    steerLocal.z = sinf(rider->steer);
    vec3_mul_mtx3f(&steerLocal.x, &rider->bike.right.x, &steerDir.x);
    CROSS(axis, steerDir, rider->frontWheel.normal);
    NORM(axis);
    rider->frontWheel.lonAxis = axis;
    CROSS(rider->frontWheel.latAxis, rider->frontWheel.normal, axis);
    CROSS(axis, rider->bike.right, rider->rearWheel.normal);
    NORM(axis);
    rider->rearWheel.lonAxis = axis;
    CROSS(rider->rearWheel.latAxis, rider->rearWheel.normal, axis);
    if (rider->frontWheel.airTicks <= 0) {
        if (rider->rearWheel.airTicks <= 0) {
            newVel.x = rider->bike.vel.x + (rider->frontWheel.force.x + rider->rearWheel.force.x) * (gPhysDeltaTime / rider->bike.mass);
            newVel.y = rider->bike.vel.y + (rider->frontWheel.force.y + rider->rearWheel.force.y) * (gPhysDeltaTime / rider->bike.mass) - gPhysDeltaTime * GRAVITY;
            newVel.z = rider->bike.vel.z + (rider->frontWheel.force.z + rider->rearWheel.force.z) * (gPhysDeltaTime / rider->bike.mass);
            CROSS(frontTorque, rider->frontWheel.contactOffset, rider->frontWheel.force);
            CROSS(rearTorque, rider->rearWheel.contactOffset, rider->rearWheel.force);
            dtOverInertia = gPhysDeltaTime / 250.0f;
            newAngVel.x = rider->bike.angVel.x + (frontTorque.x + rearTorque.x) * dtOverInertia;
            newAngVel.y = rider->bike.angVel.y + (frontTorque.y + rearTorque.y) * dtOverInertia;
            newAngVel.z = rider->bike.angVel.z + (frontTorque.z + rearTorque.z) * dtOverInertia;
            CROSS(rearVel, newAngVel, rider->rearWheel.contactOffset);
            rearVel.x += newVel.x;
            rearVel.y += newVel.y;
            rearVel.z += newVel.z;
            rearLatVel = vec3_dot(&rearVel.x, &rider->rearWheel.latAxis.x);
            CROSS(frontVel, newAngVel, rider->frontWheel.contactOffset);
            frontVel.x += newVel.x;
            frontVel.y += newVel.y;
            frontVel.z += newVel.z;
            frontLatVel = vec3_dot(&frontVel.x, &rider->frontWheel.latAxis.x);
            /* axis: now the front wheel's lever for a sideways force, without its part along the bike */
            CROSS(axis, rider->frontWheel.contactOffset, rider->frontWheel.latAxis);
            along = vec3_dot(&axis.x, &rider->bike.fwd.x);
            axis.x -= along * rider->bike.fwd.x;
            axis.y -= along * rider->bike.fwd.y;
            axis.z -= along * rider->bike.fwd.z;
            CROSS(c1, axis, rider->frontWheel.contactOffset);
            CROSS(c2, axis, rider->rearWheel.contactOffset);
            CROSS(rearArm, rider->rearWheel.contactOffset, rider->rearWheel.latAxis);
            along = vec3_dot(&rearArm.x, &rider->bike.fwd.x);
            rearArm.x -= along * rider->bike.fwd.x;
            rearArm.y -= along * rider->bike.fwd.y;
            rearArm.z -= along * rider->bike.fwd.z;
            CROSS(c3, rearArm, rider->frontWheel.contactOffset);
            CROSS(c4, rearArm, rider->rearWheel.contactOffset);
            latDot = vec3_dot(&rider->rearWheel.latAxis.x, &rider->frontWheel.latAxis.x);
            A = 1.0f / rider->bike.mass + vec3_dot(&c1.x, &rider->frontWheel.latAxis.x) / 250.0f;
            B = latDot / rider->bike.mass + vec3_dot(&c3.x, &rider->frontWheel.latAxis.x) / 250.0f;
            C = latDot / rider->bike.mass + vec3_dot(&c2.x, &rider->rearWheel.latAxis.x) / 250.0f;
            D = 1.0f / rider->bike.mass + vec3_dot(&c4.x, &rider->rearWheel.latAxis.x) / 250.0f;
            if (rider->rearWheel.airTicks > 0) {
                frontLat = -frontLatVel / A;
                rearLat = 0.0f;
            } else if (rider->frontWheel.airTicks > 0) {
                frontLat = 0.0f;
                rearLat = -rearLatVel / D;
            } else {
                frontLat = (B * rearLatVel - D * frontLatVel) / (A * D - B * C);
                rearLat = (C * frontLatVel - A * rearLatVel) / (A * D - B * C);
            }
            rearLat /= gPhysDeltaTime;
            frontLat /= gPhysDeltaTime;
            rider->rearWheel.latForce = rearLat;
            rider->frontWheel.latForce = frontLat;
            rider->rearWheel.force.x = rider->rearWheel.latAxis.x * rearLat + rider->rearWheel.lonAxis.x * (rider->driveForce - rider->rearWheel.brakeTorque);
            rider->rearWheel.force.y = rider->rearWheel.latAxis.y * rearLat + rider->rearWheel.lonAxis.y * (rider->driveForce - rider->rearWheel.brakeTorque);
            rider->rearWheel.force.z = rider->rearWheel.latAxis.z * rearLat + rider->rearWheel.lonAxis.z * (rider->driveForce - rider->rearWheel.brakeTorque);
            rider->frontWheel.force.x = rider->frontWheel.latAxis.x * frontLat - rider->frontWheel.lonAxis.x * rider->frontWheel.brakeTorque;
            rider->frontWheel.force.y = rider->frontWheel.latAxis.y * frontLat - rider->frontWheel.lonAxis.y * rider->frontWheel.brakeTorque;
            rider->frontWheel.force.z = rider->frontWheel.latAxis.z * frontLat - rider->frontWheel.lonAxis.z * rider->frontWheel.brakeTorque;
        }
    } else if (rider->rearWheel.airTicks <= 0 && rider->wheelieTimer != 0) {
        rider->rearWheel.force.x = rider->rearWheel.lonAxis.x * (rider->driveForce - rider->rearWheel.brakeTorque);
        rider->rearWheel.force.y = rider->rearWheel.lonAxis.y * (rider->driveForce - rider->rearWheel.brakeTorque);
        rider->rearWheel.force.z = rider->rearWheel.lonAxis.z * (rider->driveForce - rider->rearWheel.brakeTorque);
        sideForce = vec3_dot(&rider->bike.vel.x, &rider->rearWheel.latAxis.x) * (rider->bike.mass / gPhysDeltaTime);
        rider->rearWheel.force.x -= rider->rearWheel.latAxis.x * sideForce;
        rider->rearWheel.force.y -= rider->rearWheel.latAxis.y * sideForce;
        rider->rearWheel.force.z -= rider->rearWheel.latAxis.z * sideForce;
        lonSpeed = vec3_dot(&rider->bike.vel.x, &rider->rearWheel.lonAxis.x);
        if (lonSpeed > 0.0f) {
            inv = 1.0f / (lonSpeed + 20.0f);
        } else {
            inv = 1.0f / (-lonSpeed + 20.0f);
        }
        lonSpeed = inv * (rider->turnRadius / 0.57f);
        rider->bike.angVel.y += -(rider->stickX * lonSpeed) - rider->bike.angVel.y * 0.5f;
        rider->stickX *= 0.25f;
    }
}

/* The roll torque that leans the bike towards the stick; also puts the feet down when slow or leaning hard. */
f32 bike_calc_lean_torque(Rider *rider)
{
    struct { s16 h[2]; s32 w; } polar;
    f32 speed, targetLean, lean, torque, result;

    if (rider->wheelieTimer == 0) {
        if (rider->fwdSpeed < 1.0f && rider->powerSlide == 0) {
            if ((rider->input & INPUT_B) != 0) {
                rider->walkPhase = (s16)(s32)((f32)(s32)rider->walkPhase + rider->fwdSpeed * 1066.6666f);
            } else {
                rider->walkPhase = D_800C94C0;
            }
            if (rider->rightFootOut < 0x10) rider->rightFootOut += 2;
            if (rider->leftFootOut < 0x10) rider->leftFootOut += 2;
        } else {
            if (rider->drawRot[1] >= 0x1001 &&
                (rider->isCpu != 0 || (rider->input & INPUT_R) != 0 ||
                 rider->powerSlide != 0 || rider->hardTurn != 0 || rider->fwdSpeed < 10.0f)) {
                if (rider->rightFootOut < 0x10) rider->rightFootOut += 2;
            } else {
                if (rider->rightFootOut != 0) rider->rightFootOut -= 1;
            }
            if (rider->drawRot[1] < -0x1000 &&
                (rider->isCpu != 0 || (rider->input & INPUT_R) != 0 ||
                 rider->powerSlide != 0 || rider->hardTurn == 1 || rider->fwdSpeed < 10.0f)) {
                if (rider->leftFootOut < 0x10) rider->leftFootOut += 2;
            } else {
                if (rider->leftFootOut != 0) rider->leftFootOut -= 1;
            }
        }
    }
    math_to_polar((s32)(rider->bike.right.y * 1000.0f), (s32)(rider->bike.up.y * 1000.0f), polar.h, &polar.w);
    lean = (f32)(s32)polar.h[0] * 0.001953125f;

    if (rider->airFrames < 0) {
        targetLean = (f32)(s32)rider->stickX;
        speed = rider->speed;
        if (speed < 0.0f) {
            targetLean = 0.0f;
        } else if (speed < 4.0f) {
            targetLean = targetLean * speed * 0.25f;
        }
        if (rider->rHeld < 0) targetLean = targetLean * 0.6667f;
        if (rider->rHeld >= -9 && (targetLean - lean) * lean < 0.0f) {
            targetLean = lean + (targetLean - lean) * 0.25f;
        }
    } else if (rider->anim == ANIM_NONE) {
        if (rider->isCpu != 0) {
            targetLean = (f32)(s32)rider->stickX * 0.25f;
        } else {
            targetLean = (f32)(s32)rider->stickX * 0.5f;
        }
    } else {
        targetLean = 0.0f;
        if ((u8)((u8)rider->anim - ANIM_AIR_TURN) < 2) { /* ANIM_AIR_TURN or ANIM_AIR_TURN_2 */
            targetLean = (f32)((((s32)((u32)(u16)rider->airTurnYaw << 16)) >> 27) * 3);
        }
    }
    if (rider->reversing != 0) targetLean = 0.0f;
    torque = targetLean - lean;
    if (12.0f < torque) torque = 12.0f;
    else if (torque < -12.0f) torque = -12.0f;

    if (rider->isCpu == 0) {
        torque = sLeanTorqueGain * torque - sLeanTorqueDamping * rider->localAngVel.z;
    } else {
        torque = torque * 0.015f - rider->localAngVel.z * 0.1f;
    }
    result = 0.0f;
    if (gGame.mode != 15) result = torque;
    return result;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* Sums the forces and torques on the bike (tires, drag, gravity, pitch and turn control) into acc and angAcc. */
void bike_compute_forces(Rider *rider)
{
    Vec3f localTorque;
    Vec3f torque;
    Vec3f force;
    Vec3f frontMoment;
    Vec3f rearMoment;
    Vec3f unused;
    Vec3f groundUp;
    Vec3f groundUp2;
    s16 velYaw;
    s32 magnitude;
    s16 fwdYaw;
    s32 animFlags;
    f32 gravity;
    f32 rollRate;
    Vec3f *pos;
    s32 pitchError;

    force.x = rider->rearWheel.force.x + rider->frontWheel.force.x;
    force.y = rider->rearWheel.force.y + rider->frontWheel.force.y;
    force.z = rider->rearWheel.force.z + rider->frontWheel.force.z;
    if (rider->speed != 0.0f) {
        f32 minDrag;
        f32 drag;
        f32 rate;

        minDrag = 0.0f;
        if (rider->roughLanding != 0) {
            minDrag = 7.0f;
        }
        drag = 0.21000001f;
        if (rider->airFrames <= 0) {
            drag = (rider->wheelieTimer != 0 ? (rider->rearWheel.surface->drag - 1.0f) * 0.25f + 1.0f : rider->rearWheel.surface->drag) * 0.21000001f;
        }
        if (drag < minDrag) {
            drag = minDrag;
        }
        drag = drag * rider->speed * rider->speed;
        if (rider->airFrames <= 0) {
            drag += rider->rearWheel.surface->rollingResistance * sqrtf(rider->rearWheel.force.x * rider->rearWheel.force.x + rider->rearWheel.force.y * rider->rearWheel.force.y + rider->rearWheel.force.z * rider->rearWheel.force.z);
            drag += rider->frontWheel.surface->rollingResistance * sqrtf(rider->frontWheel.force.x * rider->frontWheel.force.x + rider->frontWheel.force.y * rider->frontWheel.force.y + rider->frontWheel.force.z * rider->frontWheel.force.z);
            if (rider->hardTurn != 0) {
                drag += sHardTurnDrag;
            }
            if (rider->rHeld < -10 && rider->isCpu == 0 && !(rider->input & INPUT_B)) {
                s32 steerInt;
                f32 steer;
                f32 excess;
                steerInt = ABS(rider->stickX);
                steer = steerInt;
                if (steer > 3.0f) {
                    excess = steer - 3.0f;
                    drag += (excess > 0.0f ? excess : -excess) * rider->handling;
                }
            }
        }
        rate = drag / rider->speed;
        force.x -= rider->bike.vel.x * rate;
        force.y -= rider->bike.vel.y * rate;
        force.z -= rider->bike.vel.z * rate;
    }
    rollRate = rider->localAngVel.z;
    mtx3f_mul_vec3(&rider->bike.angVel, &rider->bike.right, &rider->localAngVel.x);
    rider->localAngVel.z = rollRate;
    rearMoment.x = rider->rearWheel.contactOffset.y * rider->rearWheel.force.z - rider->rearWheel.force.y * rider->rearWheel.contactOffset.z;
    rearMoment.y = rider->rearWheel.contactOffset.z * rider->rearWheel.force.x - rider->rearWheel.force.z * rider->rearWheel.contactOffset.x;
    rearMoment.z = rider->rearWheel.contactOffset.x * rider->rearWheel.force.y - rider->rearWheel.force.x * rider->rearWheel.contactOffset.y;
    frontMoment.x = rider->frontWheel.contactOffset.y * rider->frontWheel.force.z - rider->frontWheel.force.y * rider->frontWheel.contactOffset.z;
    frontMoment.y = rider->frontWheel.contactOffset.z * rider->frontWheel.force.x - rider->frontWheel.force.z * rider->frontWheel.contactOffset.x;
    frontMoment.z = rider->frontWheel.contactOffset.x * rider->frontWheel.force.y - rider->frontWheel.force.x * rider->frontWheel.contactOffset.y;
    torque.x = frontMoment.x + rearMoment.x;
    torque.y = frontMoment.y + rearMoment.y;
    torque.z = frontMoment.z + rearMoment.z;
    mtx3f_mul_vec3(&torque, &rider->bike.right, &localTorque);
    localTorque.x += rider->localAngVel.y * 250.0f * rider->localAngVel.z;
    pos = &rider->bike.pos;
    /* dead stores: they keep the original's register use */
    if (rider == 0) gravity = 1.0f; else if (pos == 0) gravity = 2.0f; else if (rider->rot[0] == rider->animFrame) gravity = 3.0f; else gravity = 4.0f;
    gravity = GRAVITY;
    if (rider->airFrames < 7
        || (f32)height_get_ground(pos->x * 1000.0f, pos->y * 1000.0f, pos->z * 1000.0f) / 1000.0f - rider->bike.pos.y + 0.5f > -0.2f
        || rider->pendingCrash != 0) {
        if (rider->wheelieTimer == 0) {
            f32 t;
            t = (f32)rider->stickY - 5.0f;
            if (rider->hardTurn != 0) {
                t = 3.0f;
            }
            localTorque.x += t * 32.0f / 1000.0f * 1323.0001f;
            if (gGame.mode != GAME_MODE_HILL_CLIMB) {
                if (rider->frontWheel.airTicks >= 8 && rider->rearWheel.suspLength == rider->rearWheel.groundDist && rider->localAngVel.x < 0.0f) {
                    localTorque.x += 8000.0f;
                }
                if (rider->frontWheel.suspLength == rider->frontWheel.groundDist && rider->rearWheel.airTicks >= 8 && rider->localAngVel.x > 0.0f) {
                    localTorque.x -= 8000.0f;
                }
            }
        } else if (rider->wheelieTimer >= 16) {
            pitchError = -0x2800 - rider->wheelieBalance * 3 - rider->rot[0];
            localTorque.x = ((f32)pitchError * 3.1415927f * 3.0517578e-05f - rider->localAngVel.x * 0.5f) * 15000.0f;
        } else {
            pitchError = math_atan2f(1.3f, rider->rearWheel.contact.y - rider->frontWheel.contact.y) * 32768.0f / 3.1415927f - rider->rot[0];
            if (rider->wheelieTimer != 1) {
                localTorque.x = ((f32)pitchError * 3.0f * 3.1415927f * 3.0517578e-05f - rider->localAngVel.x * 0.5f) * 15000.0f;
            } else {
                rider->bike.angVel.x = rider->bike.angVel.y = rider->bike.angVel.z = 0.0f;
            }
        }
    } else {
        gravity = rider->airPitch * gravity + gravity;
        if (rider->anim == ANIM_NONE || rider->anim == ANIM_AIR_TURN_2 || rider->anim == ANIM_AIR_TURN) {
            f32 t;
            t = (f32)rider->stickX * rider->airTurn;
            force.x -= rider->bike.vel.z * t * rider->bike.mass;
            force.z += rider->bike.vel.x * t * rider->bike.mass;
        }
        if (rider->landingAssist == 0) {
            if (rider->wheelieTimer != 0) {
                pitchError = -0x2800 - rider->wheelieBalance * 3 - rider->rot[0];
            } else {
                s32 pitch;
                pitch = rider->rot[0];
                pitchError = rider->airPitchInput;
                if (ABS(pitch) > 0x2000) {
                    pitchError = 0;
                }
                pitchError = pitchError * 384 - pitch;
            }
            localTorque.x += ((f32)(pitchError / 4) * 3.1415927f * 3.0517578e-05f - rider->localAngVel.x * 0.0625f) * 15000.0f;
        }
        if (rider->anim == ANIM_KOBE) {
            /* the bike turns a full circle over the animation */
            s32 animLen;
            math_to_polar(-rider->bike.vel.x * 1000.0f, rider->bike.vel.z * 1000.0f, &velYaw, &magnitude);
            math_to_polar(-rider->bike.fwd.x * 1000.0f, rider->bike.fwd.z * 1000.0f, &fwdYaw, &magnitude);
            rider->heading = velYaw;
            animLen = rider_anim_get_length(ANIM_KOBE, &animFlags);
            velYaw += ((rider->animFrame - (animLen << 8)) << 8) / animLen;
            velYaw -= fwdYaw;
            if (velYaw > 0xC00) {
                velYaw = 0xC00;
            }
            rider->bike.angVel.y = -((f32)velYaw * 3.1415927f * 3.0517578e-05f) * 60.0f;
        } else if (rider->speed > 5.0f) {
            f32 t;
            math_to_polar(rider->bike.vel.x * 1000.0f, rider->bike.vel.z * 1000.0f, &velYaw, &magnitude);
            math_to_polar(rider->bike.fwd.x * 1000.0f, rider->bike.fwd.z * 1000.0f, &fwdYaw, &magnitude);
            velYaw -= rider->airTurnYaw;
            velYaw -= fwdYaw;
            t = (f32)velYaw * 6.1035156e-05f - rider->bike.angVel.y * 0.25f;
            if (t > 0.1f) {
                t = 0.1f;
            } else if (t < -0.1f) {
                t = -0.1f;
            }
            t *= 15000.0f;
            localTorque.x += t * rider->bike.right.y;
            localTorque.y += t * rider->bike.up.y;
            localTorque.z += t * rider->bike.fwd.y;
        }
    }
    force.y -= gravity * rider->bike.mass;
    rider->bike.acc.x = force.x / rider->bike.mass;
    rider->bike.acc.y = force.y / rider->bike.mass;
    rider->bike.acc.z = force.z / rider->bike.mass;
    localTorque.z = bike_calc_lean_torque(rider) * 64.0f * 250.0f;
    localTorque.x += localTorque.z * rider->localAngVel.y * gPhysDeltaTime;
    vec3_mul_mtx3f(&localTorque, &rider->bike.right, &torque);
    rider->bike.angAcc.x = torque.x / 250.0f;
    rider->bike.angAcc.y = torque.y / 250.0f;
    rider->bike.angAcc.z = torque.z / 250.0f;
    if (rider->powerSlide != 0 && (rider->stickX < -10 || rider->stickX > 10)) {
        f32 len;
        f32 t;
        groundUp.x = rider->frontWheel.normal.x + rider->rearWheel.normal.x;
        groundUp.y = rider->frontWheel.normal.y + rider->rearWheel.normal.y;
        groundUp.z = rider->frontWheel.normal.z + rider->rearWheel.normal.z;
        len = sqrtf(groundUp.x * groundUp.x + groundUp.y * groundUp.y + groundUp.z * groundUp.z);
        if (len != 0.0f) {
            groundUp.x /= len;
            groundUp.y /= len;
            groundUp.z /= len;
        }
        t = vec3_dot(&groundUp, &rider->bike.angVel);
        if (rider->powerSlide == 2) {
            f32 rate;
            rate = t / (2.0f * gPhysDeltaTime);
            rider->bike.angAcc.x -= groundUp.x * rate;
            rider->bike.angAcc.y -= groundUp.y * rate;
            rider->bike.angAcc.z -= groundUp.z * rate;
        } else {
            s32 spinInt;
            spinInt = t;
            spinInt = ABS(spinInt);
            if ((f32)spinInt < 1.5f) {
                f32 scale;
                scale = gPhysDeltaTime * 240.0f;
                rider->bike.angAcc.x -= groundUp.x * ((f32)rider->stickX / scale);
                rider->bike.angAcc.y -= groundUp.y * ((f32)rider->stickX / scale);
                rider->bike.angAcc.z -= groundUp.z * ((f32)rider->stickX / scale);
            }
        }
    }
    if (rider->rHeld >= -1 && rider->anim == ANIM_NONE) {
        f32 len;
        f32 t;
        s32 steerInt2;
        groundUp.x = rider->frontWheel.normal.x + rider->rearWheel.normal.x;
        groundUp.y = rider->frontWheel.normal.y + rider->rearWheel.normal.y;
        groundUp.z = rider->frontWheel.normal.z + rider->rearWheel.normal.z;
        len = sqrtf(groundUp.x * groundUp.x + groundUp.y * groundUp.y + groundUp.z * groundUp.z);
        if (len != 0.0f) {
            groundUp.x /= len;
            groundUp.y /= len;
            groundUp.z /= len;
        }
        t = vec3_dot(&groundUp, &rider->bike.angVel);
        steerInt2 = rider->stickX;
        if ((t > 0.5f && steerInt2 >= -3) || (t < -0.5f && steerInt2 < 4)) {
            f32 rate;
            rate = t / (2.0f * gPhysDeltaTime);
            rider->bike.angAcc.x = -groundUp.x * rate;
            rider->bike.angAcc.y = -groundUp.y * rate;
            rider->bike.angAcc.z = -groundUp.z * rate;
        }
    }
    if (rider->bike.hitTimer != 0) {
        f32 len;
        f32 t;
        groundUp2.x = rider->frontWheel.normal.x + rider->rearWheel.normal.x;
        groundUp2.y = rider->frontWheel.normal.y + rider->rearWheel.normal.y;
        groundUp2.z = rider->frontWheel.normal.z + rider->rearWheel.normal.z;
        len = sqrtf(groundUp2.x * groundUp2.x + groundUp2.y * groundUp2.y + groundUp2.z * groundUp2.z);
        if (len != 0.0f) {
            groundUp2.x /= len;
            groundUp2.y /= len;
            groundUp2.z /= len;
        }
        t = vec3_dot(&groundUp2, &rider->bike.angVel) / (gPhysDeltaTime * 16.0f);
        rider->bike.angAcc.x -= groundUp2.x * t;
        rider->bike.angAcc.y -= groundUp2.y * t;
        rider->bike.angAcc.z -= groundUp2.z * t;
    }
}

/* Steps the velocities by the accelerations; updates the speeds and the camera's yaw lag. */
void bike_integrate_velocity(Rider *rider)
{
    f32 speedSq;
    f32 fwdSpeed;

    rider->bike.vel.x = rider->bike.vel.x + rider->bike.acc.x * gPhysDeltaTime;
    rider->bike.vel.y = rider->bike.vel.y + rider->bike.acc.y * gPhysDeltaTime;
    rider->bike.vel.z = rider->bike.vel.z + rider->bike.acc.z * gPhysDeltaTime;
    rider->bike.angVel.x = rider->bike.angVel.x + rider->bike.angAcc.x * gPhysDeltaTime;
    rider->bike.angVel.y = rider->bike.angVel.y + rider->bike.angAcc.y * gPhysDeltaTime;
    rider->bike.angVel.z = rider->bike.angVel.z + rider->bike.angAcc.z * gPhysDeltaTime;
    mtx3f_mul_vec3(&rider->bike.angVel.x, &rider->bike.right.x, &rider->localAngVel.x);
    speedSq = rider->bike.vel.x * rider->bike.vel.x
        + rider->bike.vel.y * rider->bike.vel.y
        + rider->bike.vel.z * rider->bike.vel.z;
    rider->speed = __builtin_sqrtf(speedSq);
    fwdSpeed = vec3_dot(&rider->bike.vel.x, &rider->bike.fwd.x);
    rider->fwdSpeed = fwdSpeed;
    if (rider->crashState == CRASH_STATE_RIDING && rider->anim != ANIM_KOBE) {
        cam_calc_bike_yaw(rider);
    }
}

/* bike_integrate_position's stack, in the original's order */
typedef struct {
    f32 rollShift[3];
    f32 pad0;
    f32 zero[3];                    /* cleared, never read */
    f32 pad1[5];
    f32 hitT[2];                    /* [0]: the segment's end for height_raycast_walls */
} IntegrateLocals;

/* Moves the bike one tick on its contact points; one rider a frame checks the walls (a hit crashes it). */
void bike_integrate_position(Rider *rider)
{
    IntegrateLocals locals;
    f32 halfDt;
    f32 rollShiftScale;
    s32 numContacts;
    s32 riderIdx;
    s32 i;

    halfDt = gPhysDeltaTime * 0.5f;
    locals.zero[0] = locals.zero[1] = locals.zero[2] = 0.0f;
    rollShiftScale = -rider->localAngVel.z * halfDt;
    locals.rollShift[0] = rider->bike.right.x * rollShiftScale;
    locals.rollShift[1] = rider->bike.right.y * rollShiftScale;
    locals.rollShift[2] = rider->bike.right.z * rollShiftScale;
    rider->bike.vel.x = locals.rollShift[0] + rider->bike.vel.x;
    rider->bike.vel.y = locals.rollShift[1] + rider->bike.vel.y;
    rider->bike.vel.z = locals.rollShift[2] + rider->bike.vel.z;
    numContacts = 8;
    if (rider->frontWheel.airTicks <= 0 && rider->rearWheel.airTicks <= 0) {
        numContacts = 6;
    }
    phys_body_step(&rider->bike.pos.x, sBikeContactPoints, numContacts, 0);
    rider->bike.flags |= 1;
    riderIdx = rider - gRiders;
    rider->bike.vel.x -= locals.rollShift[0];
    rider->bike.vel.y -= locals.rollShift[1];
    {
        f32 velZ = rider->bike.vel.z;
        locals.hitT[0] = 1.0f;
        rider->bike.vel.z = velZ - locals.rollShift[2];
    }
    if (riderIdx == (gRaceFrame & 7)) {
        u8 *pairs = sBikeWallCheckPairs;
        u8 *pair = (u8 *)(((gRaceFrame >> 2) & 6) + (s32)pairs);
        if (height_raycast_walls(&rider->bike.contacts[pair[0]].x, &rider->bike.contacts[pair[1]].x, gContactNormal, locals.hitT, 0)) {
            bike_crash_begin(rider, ANIM_CRASH_FRONT);
        }
    }
}

/* The drawing state from the physics: suspension, position, steering, trick blend; pitching too far crashes. */
void bike_update_render_pose(Rider *rider)
{
    f32 localNormal[3];
    f32 trickRot[4];
    f32 offset[3];
    f32 leanQuat[4];
    f32 shakeQuat[4];
    f32 suspFront, suspRear, drop, over, len, angle, sinA, sinB, cosA, amp;
    s32 angleWord, lean;

    suspFront = rider->frontWheel.suspLength;
    suspRear = rider->rearWheel.suspLength;
    if (rider->crashState == CRASH_STATE_RIDING) {
        mtx3f_mul_vec3(&rider->frontWheel.normal.x, &rider->bike.right.x, localNormal);
        suspFront += (1.0f - localNormal[1] / __builtin_sqrtf(1.0f - localNormal[0] * localNormal[0])) * 0.25f;
        mtx3f_mul_vec3(&rider->rearWheel.normal.x, &rider->bike.right.x, localNormal);
        suspRear += (1.0f - localNormal[1] / __builtin_sqrtf(1.0f - localNormal[0] * localNormal[0])) * 0.25f;
    }
    drop = 0.0f;
    rider->drawSuspFront = suspFront / 0.001f;
    rider->drawSuspRear = suspRear / 0.001f;
    if (suspFront > 0.2f) {
        rider->drawSuspFront = 200;
        drop = 0.2f - suspFront;
    }
    if (suspRear > 0.2f) {
        over = 0.2f - suspRear;
        rider->drawSuspRear = 200;
        if (over < drop) {
            drop = over;
        }
    }
    drop += 0.5f;
    if (suspRear < -0.1f) {
        rider->drawSuspRear = -100;
    }
    rider->drawSuspFront = (f32)rider->drawSuspFront + 50.0f;
    rider->drawPos.x = (rider->bike.pos.x - rider->bike.up.x * drop) * 1000.0f;
    rider->drawPos.y = (rider->bike.pos.y - rider->bike.up.y * drop) * 1000.0f;
    rider->drawPos.z = (rider->bike.pos.z - rider->bike.up.z * drop) * 1000.0f;
    rider->drawSteer = rider->steer * 32768.0f / 3.1415927f;
    *(Quat_2 *)rider->drawQuat = *(Quat_2 *)rider->bike.quat;
    if (rider->crashState == CRASH_STATE_RIDING) {
        quat_to_angles(rider->bike.quat, &rider->rot[0], &rider->rot[1], &rider->rot[2]);
        if (rider->anim == ANIM_NONE || rider->anim == ANIM_KOBE) {
            angleWord = rider->rot[0];
            if (angleWord > 0x3000) {
                bike_crash_begin(rider, ANIM_CRASH_BACK);
            } else if (angleWord < -0x3000) {
                bike_crash_begin(rider, ANIM_CRASH_FRONT);
            }
        }
        bike_update_trick_offset(rider);
        if (rider->trickBlend != 0.0f) {
            trickRot[0] = (1.0f - rider->trickBlend) + rider->trickQuat[0] * rider->trickBlend;
            trickRot[1] = rider->trickQuat[1] * rider->trickBlend;
            trickRot[2] = rider->trickQuat[2] * rider->trickBlend;
            trickRot[3] = rider->trickQuat[3] * rider->trickBlend;
            len = __builtin_sqrtf(trickRot[0] * trickRot[0] + trickRot[1] * trickRot[1] + trickRot[2] * trickRot[2] + trickRot[3] * trickRot[3]);
            trickRot[0] /= len;
            trickRot[1] /= len;
            trickRot[2] /= len;
            trickRot[3] /= len;
            quat_mul(rider->drawQuat, rider->bike.quat, trickRot);
            vec3_mul_mtx3f(&rider->trickOffset.x, &rider->bike.right.x, offset);
            rider->drawPos.x = (f32)rider->drawPos.x + offset[0] * (rider->trickBlend * 1000.0f);
            rider->drawPos.y = (f32)rider->drawPos.y + offset[1] * (rider->trickBlend * 1000.0f);
            rider->drawPos.z = (f32)rider->drawPos.z + offset[2] * (rider->trickBlend * 1000.0f);
        }
        quat_to_angles(rider->drawQuat, &rider->drawRot[0], &rider->drawRot[1], &rider->drawRot[2]);
        if (rider->isCpu != 0) {
            angleWord = rider->drawRot[1];
            if (angleWord > 0x1800) {
                lean = 0;
            } else if (angleWord > 0xC00) {
                lean = 0x1800 - angleWord;
            } else if (angleWord >= -0xBFF) {
                lean = rider->rot[1];
            } else if (angleWord >= -0x17FF) {
                lean = -0x1800 - angleWord;
            } else {
                lean = 0;
            }
            angle = lean;
            angle *= 3.1415927f;
            angle *= 3.0517578e-05f;
            angle *= 0.5f;
            cosA = cosf(angle);
            sinA = sinf(angle);
            leanQuat[0] = cosA;
            leanQuat[1] = sinA * rider->bike.fwd.x;
            leanQuat[2] = sinA * rider->bike.fwd.y;
            leanQuat[3] = sinA * rider->bike.fwd.z;
            quat_mul(rider->drawQuat, leanQuat, rider->drawQuat);
            quat_to_angles(rider->drawQuat, &rider->drawRot[0], &rider->drawRot[1], &rider->drawRot[2]);
        } else if ((rider->input & INPUT_Z) && gRaceFrame - rider->turboPressFrame < 0x3C &&
                   (rider->rearWheel.surface->flags & SURFACE_CLODS) && rider->airFrames < 0 && rider->speed < 5.0f) {
            if (rider->speed > 2.0f) {
                amp = (f32)(rider->turboPressFrame - (gRaceFrame - 0x3C)) * 6.1035155e-07f / 60.0f;
            } else {
                amp = 1.2207031e-06f;
            }
            angle = gCosTable[((((gRaceFrame - rider->turboPressFrame) << 13) >> 4) - 0x400) & 0xFFF];
            angle *= amp;
            shakeQuat[0] = cosf(angle);
            sinB = sinf(angle);
            shakeQuat[1] = sinB * rider->bike.up.x;
            shakeQuat[2] = sinB * rider->bike.up.y;
            shakeQuat[3] = sinB * rider->bike.up.z;
            quat_mul(rider->drawQuat, shakeQuat, rider->drawQuat);
            quat_to_angles(rider->drawQuat, &rider->drawRot[0], &rider->drawRot[1], &rider->drawRot[2]);
        }
    } else {
        quat_to_mtx3f(&rider->body.right.x, rider->body.quat);
        rider->drawBodyPos.x = rider->body.pos.x * 1000.0f;
        rider->drawBodyPos.y = rider->body.pos.y * 1000.0f;
        rider->drawBodyPos.z = rider->body.pos.z * 1000.0f;
    }
    rider->drawSteer = rider->steer * 32768.0f / 3.1415927f;
    quat_to_mtx3f(&rider->bike.right.x, rider->bike.quat);
}

/* Lands the bike: cleanly, roughly or in a crash, from the wheels' heights and the impact. */
void bike_handle_landing(Rider *arg)
{
    Rider *rider = arg;
    f32 *vel = &rider->bike.vel.x;
    f32 impactVel, wheelDiff;
    s32 impact, frame;

    impactVel = vec3_dot(vel, &rider->rearWheel.normal.x) + vec3_dot(vel, &rider->frontWheel.normal.x);
    if ((u8)rider->anim < ANIM_TRICK_COUNT && rider->airFrames >= 0xB)
        bike_crash_begin(rider, ANIM_CRASH_FRONT);
    rider->rHeld = 0;
    if (impactVel < -10.0f)
        rider->wheelieTimer = 0;
    if (gGame.mode == 15) {
        rider->landing = sBikeTuneLanding;
        rider->bike.angVel.y = 0.0f;
    }
    if (rider->airFrames < 0x29)
        goto record_impact;
    wheelDiff = rider->frontWheel.groundDist - rider->rearWheel.groundDist;
    rider->prevJumpDist = rider->jumpDist;
    if (wheelDiff < -2.0f) {
        bike_crash_begin(rider, ANIM_CRASH_BACK);
        goto soften;
    }
    if (wheelDiff < -0.8f) {
        rider->pendingCrash = ANIM_CRASH_BACK;
        goto save_crash_vel;
    }
    if (wheelDiff < -0.3f) {
        rider->roughLanding = 1;
        goto soften;
    }
    if (wheelDiff < 0.3f) {
        rider->landingAssist = 1;
        goto soften;
    }
    if (wheelDiff < 0.6f) {
        rider->roughLanding = 1;
        goto soften;
    }
    rider->pendingCrash = ANIM_CRASH_FRONT;
save_crash_vel:
    rider->crashVel = rider->bike.vel;
soften:
    if (rider->pendingCrash != 0 && -10.0f < impactVel) {
        rider->pendingCrash = 0;
        rider->roughLanding = 1;
    }
    if (rider->landingAssist != 0) {
        f32 zf = 0.0f;
        rider->bike.angVel.z = zf;
        rider->bike.angVel.y = zf;
        rider->bike.angVel.x = zf;
    }
record_impact:
    if (rider->airFrames < 0x15)
        goto done;
    impact = (s32)(-impactVel * 10.0f);
    rider->landingImpact = impact;
    if (impact < 0)
        rider->landingImpact = 0;
    if (rider->landingImpact >= 0x100)
        rider->landingImpact = 0xFF;
    frame = gRaceFrame;
    rider->impactFlags = 0xFF;
    rider->steer = 0.0f;
    rider->landingFrame = frame;
done:
    rider->airFrames = 0;
}

/* Predicts when and where the bike lands under gravity g (a secant search from time t); writes the point.
   The dead tests keep the original's register use. */
f32 bike_predict_landing(Rider *rider, f32 gravity, f32 time, f32 *point)
{
    f32 diff, prevDiff, slope, step, tolerance, pointZ;

    point[0] = rider->bike.pos.x + rider->bike.vel.x * time;
    point[1] = rider->bike.pos.y + (rider->bike.vel.y + gravity * time * 0.5f) * time;
    pointZ = rider->bike.pos.z + rider->bike.vel.z * time;
    point[2] = pointZ;
    if (point == 0) pointZ = 1.0f; else if (rider == 0) pointZ = 2.0f; else pointZ = 3.0f;
        diff = (f32)height_get_ground(point[0] * 1000.0f, point[1] * 1000.0f, point[2] * 1000.0f) / 1000.0f;
    diff = (diff - point[1]) + 0.5f;
    slope = rider->bike.vel.y + gravity * time;
    if (0.0f < slope ? slope < 0.1f : -slope < 0.1f) slope = 0.1f;
    slope = 1.0f / slope;
    if (10.0f < slope) slope = 10.0f;
    else if (slope < -10.0f) slope = -10.0f;
    tolerance = 0.1f;
    while (tolerance < diff || diff < -tolerance) {
        step = diff * slope;
        if (time + step < 0.0f) step = -time;
        time += step;
        prevDiff = diff;
        point[1] = rider->bike.pos.y + (rider->bike.vel.y + gravity * time * 0.5f) * time;
        point[0] = rider->bike.pos.x + rider->bike.vel.x * time;
        pointZ = rider->bike.pos.z + rider->bike.vel.z * time;
        point[2] = pointZ;
        if (point == 0) pointZ = 1.0f; else if (rider == 0) pointZ = 2.0f; else if (point == (f32 *)rider) pointZ = 4.0f; else if (point == &rider->bike.pos.x) pointZ = 5.0f; else if (point == &rider->bike.pos.y) pointZ = 6.0f; else pointZ = 3.0f;
                diff = (f32)height_get_ground(point[0] * 1000.0f, point[1] * 1000.0f, point[2] * 1000.0f) / 1000.0f;
    diff = (diff - point[1]) + 0.5f;
        if (prevDiff == diff) return time;
        slope = step / (prevDiff - diff);
        if (10.0f < slope) slope = 10.0f;
        else if (slope < -10.0f) slope = -10.0f;
        tolerance += 0.05f;
    }
    if (rider->speed != 0.0f) {
        point[0] = point[0] - rider->bike.vel.x * (0.5f / rider->speed);
        point[2] = point[2] - rider->bike.vel.z * (0.5f / rider->speed);
    }
    if (point == 0) diff = 1.0f; else if (rider == 0) diff = 2.0f; else diff = 3.0f;
        point[1] = (f32)height_get_ground(point[0] * 1000.0f, point[1] * 1000.0f, point[2] * 1000.0f) / 1000.0f;
    if (time < 0.0f) time = 0.0f;
    return time;
}

/* In the air: pitches the bike towards the slope of the ground where it will land. */
void bike_align_to_ground(Rider *rider)
{
    Vec3f normal;
    Vec3f axis;
    Vec3f unused;
    Vec3f landPos;
    f32 sinAngle;
    f32 angle;
    s32 angleWord, uninit;
    f32 time, rate;

    landPos = rider->bike.pos;
    /* time and rate first hold the landing point's x and z */
    time = landPos.x + rider->bike.vel.x * rider->landingTime;
    landPos.x = time;
    rate = landPos.z + rider->bike.vel.z * rider->landingTime;
    landPos.z = rate;
    /* a dead test (of an uninitialised variable): it keeps the original's register use */
    if (rider == 0) angle = 1.0f; else if (angleWord == uninit) angle = 2.0f; else angle = 3.0f;
    height_get_ground((s32)(landPos.x * 1000.0f), (s32)(landPos.y * 1000.0f), (s32)(landPos.z * 1000.0f));
    height_get_ground_normal(&normal.x);
    if (normal.y < 0.5f) {
        return;
    }
    axis.x = rider->bike.up.y * normal.z - normal.y * rider->bike.up.z;
    axis.y = rider->bike.up.z * normal.x - normal.z * rider->bike.up.x;
    axis.z = rider->bike.up.x * normal.y - normal.x * rider->bike.up.y;
    sinAngle = vec3_dot(&axis.x, &rider->bike.right.x);
    axis.x = rider->bike.right.x * sinAngle;
    axis.y = rider->bike.right.y * sinAngle;
    axis.z = rider->bike.right.z * sinAngle;
    if (sinAngle == 0.0f) {
        return;
    }
    angleWord = math_asini((s32)(sinAngle * 16384.0f));
    angle = (f32)(s16)angleWord * 3.1415927f * 3.0517578e-05f;
    time = rider->landingTime;
    time += 0.1f;
    rate = (angle + angle) / time;
    if (rate > 2.0f) {
        rate = 2.0f;
    } else if (rate < -2.0f) {
        rate = -2.0f;
    }
    rate = rate / sinAngle;
    rider->bike.angVel.x = axis.x * rate;
    rider->bike.angVel.y = axis.y * rate;
    rider->bike.angVel.z = axis.z * rate;
}

/* Wheelies: Z with the stick pulled back starts one and the stick balances it; CPU riders never wheelie. */
void bike_update_wheelie(Rider *rider)
{
    s8 stick;

    /* the mask reads leftFootOut and rightFootOut: both feet on the pegs */
    if (rider->airFrames > 0x3C && (*(s32 *)&rider->crestAhead & 0xFFFF00) == 0) {
        rider->wheelieTimer = 0;
    }
    if (rider->wheelieTimer == 0) {
        if (rider->airFrames < 0) {
            if (rider->frontWheel.airTicks < 0 && rider->speed > 7.5f && (rider->frontWheel.surface->flags & 0x20)) {
                bike_crash_begin(rider, ANIM_CRASH_FRONT);
            }
            if ((rider->input & INPUT_Z) && !(rider->prevInput & INPUT_Z) && abs(rider->stickX) < 3 &&
                rider->fwdSpeed > -1.0f && rider->stickY < -4) {
                rider->wheelieTimer = 0x10;
                rider->wheelieBalance = 0;
            }
        }
    } else {
        if (rider->rearWheel.airTicks <= 0) {
            if (rider->rearWheel.surface->flags & 0x20) {
                rider->wheelieTimer = 0xF;
            }
            if (rider->rearWheel.airTicks <= 0 && vec3_dot(&rider->bike.up.x, &rider->rearWheel.normal.x) < 0.5f) {
                bike_crash_begin(rider, ANIM_CRASH_BACK);
            }
        }
        if (rider->wheelieTimer < 0x10) {
            rider->wheelieTimer--;
        } else {
            if (rider->wheelieTimer >= 0x3D && rider->airFrames <= 0) {
                stick = rider->stickY;
                if (stick < -12) {
                    rider->wheelieBalance += 0x20;
                } else if (stick >= -3) {
                    rider->wheelieBalance -= 0x20;
                }
                rider->wheelieBalance -= rider->wheelieBalance / 64;
                if (rider->wheelieBalance > 0x400) {
                    bike_crash_begin(rider, ANIM_CRASH_BACK);
                    rider->bike.angVel.x -= rider->bike.right.x * 5.0f;
                    rider->bike.angVel.y -= rider->bike.right.y * 5.0f;
                    rider->bike.angVel.z -= rider->bike.right.z * 5.0f;
                } else if (rider->wheelieBalance < -0x400) {
                    rider->wheelieTimer = 0xF;
                }
                if ((rider->stickY < -12 || rider->stickY > -4) && (f32)rider->airFrames < 3.0f) {
                    if (rider->rightFootOut < 0x10) rider->rightFootOut++;
                    if (rider->leftFootOut < 0x10) rider->leftFootOut++;
                } else {
                    if (rider->rightFootOut != 0) rider->rightFootOut--;
                    if (rider->leftFootOut != 0) rider->leftFootOut--;
                }
            } else {
                rider->leftFootOut = 0;
                rider->rightFootOut = 0;
            }
            rider->wheelieTimer++;
            if (rider->wheelieTimer == 0xF0) {
                rider->fistPump = 1;
            }
            if (rider->stickY >= 0) {
                rider->wheelieTimer = 0xF;
            }
        }
    }
    if (rider->isCpu && rider->wheelieTimer != 0) {
        rider->wheelieTimer = 0;
    }
}

/* Takeoff boosts and landing prediction, air turns and air pitch, the landing assist, the jump's distance. */
void bike_update_jump(Rider *rider)
{
    volatile s32 pad[12];
    Vec3f vec;
    Vec3f axis;
    Vec3f landPos;
    f32 dirX, dirZ, horizSpeed;
    f32 score1, score2, slope;
    f32 speed, boost, normalVel, unusedD;
    f32 takeoffSpeed, minAirTime, pitchRate, rampBoost, pitchScale, limit, tilt;
    s32 steer, pitchIn;
    f32 rampScale;

    if (rider->roughLanding != 0) {
        rider->roughLanding += ((u8 *)&gPhysTicks)[3]; /* its low byte */
        if (rider->roughLanding >= 60) {
            rider->roughLanding = 0;
        }
    }
    if (rider->jumpState == 1 && (rider->crashState != CRASH_STATE_RIDING || rider->anim != ANIM_NONE)) {
        rider->jumpState = -1;
    }
    if (rider->rearWheel.airTicks > 0 && rider->frontWheel.airTicks > 0) {
        if (rider->airFrames <= 0) {
            rider->cpuAirPitch = 0;
            rider->landingTime1 = (rider->bike.vel.y + rider->bike.vel.y) / 10.106251f;
            rider->landingTime2 = (rider->bike.vel.y + rider->bike.vel.y) / 19.29375f;
            rider->landingTime = (rider->bike.vel.y + rider->bike.vel.y) / GRAVITY;
            stunt_on_takeoff(rider);
            takeoffSpeed = rider->speed;
            minAirTime = 0.5f;
            if (!(takeoffSpeed > 20.0f)) {
                minAirTime = 0.25f;
                if (takeoffSpeed > 10.0f) {
                    minAirTime = (takeoffSpeed - 10.0f) / 40.0f + 0.25f;
                }
            }
            if (minAirTime < rider->landingTime) {
                if (rider->wheelieTimer != 0 && rider->wheelieTimer < 45) {
                    rider->wheelieTimer = 0;
                }
                pitchRate = vec3_dot(&rider->bike.right.x, &rider->bike.angVel.x);
                rider->bike.angVel.x -= pitchRate * rider->bike.right.x;
                rider->bike.angVel.y -= pitchRate * rider->bike.right.y;
                rider->bike.angVel.z -= pitchRate * rider->bike.right.z;
                if (rider->isCpu == 0 && rider->crashState == CRASH_STATE_RIDING && gRaceFrame - rider->turboPressFrame < 10 && (rider->input & INPUT_Z)) {
                    /* turbo at takeoff: a boost (none when overheated) */
                    speed = rider->speed;
                    if (speed < 20.0f && rider->engine.overheated == 0) {
                        boost = 5.0f;
                        if (speed > 10.0f) {
                            boost = 5.0f - (speed - 10.0f) * 0.5f;
                        }
                        if (speed != 0.0f) {
                            speed = boost / speed + 1.0f;
                        } else {
                            speed = 0.0f;
                        }
                        rider->bike.vel.x *= speed;
                        rider->bike.vel.y *= speed;
                        rider->bike.vel.z *= speed;
                        if (rider->rot[0] >= -0x17FF) {
#if VERSION == VERSION_LODGENET
                            rider->bike.angVel.x -= rider->bike.right.x * 3.0f;
                            rider->bike.angVel.y -= rider->bike.right.y * 3.0f;
                            rider->bike.angVel.z -= rider->bike.right.z * 3.0f;
#else
                            /* retail pitches the bike less */
                            rider->bike.angVel.x -= rider->bike.right.x * 2.0f;
                            rider->bike.angVel.y -= rider->bike.right.y * 2.0f;
                            rider->bike.angVel.z -= rider->bike.right.z * 2.0f;
#endif
                        }
                        snd_play_rider_sfx_pitch(7, rider - gRiders, 0xC0, 0xC0, 1);
                    }
                }
                if (gGame.mode == GAME_MODE_EXCITE_3D) {
                    Vec3f *takeoff = &rider->takeoffPos;
                    if (rider->speed > 4.0f) {
                        /* boost and speed hold the takeoff point's x and z; the dead test keeps the original's
                           register use */
                        boost = takeoff->x;
                        speed = takeoff->z;
                        if (rider == 0) pitchIn = 0;
                        height_get_ground(boost * 1000.0f, takeoff->y * 1000.0f, speed * 1000.0f);
                        if (gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & SURFACE_BOOST_RAMP) {
                            f32 rampScale;
                            rampScale = 10.0f;
                            rampBoost = sBikeTuneRampBoost * rampScale / rider->speed + 1.0f;
                            rider->bike.vel.x *= rampBoost;
                            rider->bike.vel.y *= rampBoost;
                            rider->bike.vel.z *= rampBoost;
                        }
                    }
                }
            }
            vec.x = rider->bike.vel.x * rider->landingTime1;
            vec.y = rider->bike.vel.y * rider->landingTime1;
            vec.z = rider->bike.vel.z * rider->landingTime1;
            rider->landingPos1.x = vec.x + rider->bike.pos.x;
            rider->landingPos1.y = vec.y + rider->bike.pos.y;
            rider->landingPos1.z = vec.z + rider->bike.pos.z;
            vec.x = rider->bike.vel.x * rider->landingTime2;
            vec.y = rider->bike.vel.y * rider->landingTime2;
            vec.z = rider->bike.vel.z * rider->landingTime2;
            rider->landingPos2.x = vec.x + rider->bike.pos.x;
            rider->landingPos2.y = vec.y + rider->bike.pos.y;
            rider->landingPos2.z = vec.z + rider->bike.pos.z;
            rider->landingAssist = 0;
            rider->airTurnYaw = 0;
            {
                u16 seg = (u16)rider->segment;
                rider->landingSegment2 = seg;
                rider->landingSegment1 = seg;
            }
            rider->airFrames = gPhysTicks;
        } else {
            rider->airFrames += gPhysTicks;
        }
        if (rider->airFrames >= 4) {
            rider->landingTime2 = bike_predict_landing(rider, -19.29375f, rider->landingTime2, &rider->landingPos2.x);
            height_get_ground_normal(&rider->landingNormal2.x);
            rider->landingTime1 = bike_predict_landing(rider, -10.106251f, rider->landingTime1, &rider->landingPos1.x);
            height_get_ground_normal(&rider->landingNormal1.x);
            rider->landingTime = (rider->landingTime1 + rider->landingTime2) * 0.5f;
            if (rider->input & INPUT_R) {
                steer = rider->stickX;
                if (rider->anim == ANIM_NONE && rider->landingTime > 0.5f) {
                    if (steer > 10) {
                        stunt_start(rider, ANIM_AIR_TURN);
                    } else if (steer < -10) {
                        stunt_start(rider, ANIM_AIR_TURN_2);
                    }
                    if (rider->anim != ANIM_NONE) {
                        rider->animSpeed = (f32)(gRiderAnims[rider->anim].body->count << 9) / (rider->landingTime * 60.0f);
                    }
                }
                if (rider->anim == ANIM_AIR_TURN) {
                    if (steer <= 0) {
                        steer = 1;
                    }
                    rider->airTurnYaw = steer << 9;
                } else if (rider->anim == ANIM_AIR_TURN_2) {
                    if (steer >= 0) {
                        steer = -1;
                    }
                    rider->airTurnYaw = steer << 9;
                }
            }
            if (rider->isCpu == 0) {
                if (rider->anim == ANIM_NONE && !(rider->input & INPUT_C_DOWN)) {
                    pitchScale = 0.015625f;
                    if (rider->stickY < 0) {
                        pitchScale = rider->airFloat;
                    }
                    rider->airPitch = rider->stickY * pitchScale;
                    if (rider == 0) pitchIn = 0; /* dead: keeps the original's register use */
                    rider->airPitchInput = rider->stickY;
                } else {
                    rider->airPitchInput = 0;
                    rider->airPitch = rider->airFloat * -15.0f;
                }
            } else {
                steer = 5;
                if (rider->airFrames >= 6) {
                    steer = 1;
                }
                dirX = rider->bike.vel.x;
                dirZ = rider->bike.vel.z;
                horizSpeed = __builtin_sqrtf(dirX * dirX + dirZ * dirZ);
                if (horizSpeed == 0.0f) {
                    horizSpeed = 1.0f;
                }
                dirX /= horizSpeed;
                dirZ /= horizSpeed;
                score2 = rider->landingPos2.y * GRAVITY;
                slope = (dirX * rider->landingNormal2.x + dirZ * rider->landingNormal2.z) / rider->landingNormal2.y;
                if (gGame.mode != GAME_MODE_DESERT) {
                    rider->landingSegment2 = path_find_segment(&rider->landingPos2, rider->landingSegment2, steer, rider->pathSearch);
                    if (gPathNodes[rider->landingSegment2].flags & PATH_NODE_BAD_LANDING) {
                        score2 -= 1000.0f;
                    }
                }
                score1 = rider->landingPos1.y * GRAVITY;
                normalVel = rider->landingNormal2.x * rider->bike.vel.x + rider->landingNormal2.y * (rider->bike.vel.y - rider->landingTime2 * 17.640001f) + rider->landingNormal2.z * rider->bike.vel.z;
                dirX = (dirX * rider->landingNormal1.x + dirZ * rider->landingNormal1.z) / rider->landingNormal1.y;
                score2 -= normalVel * normalVel;
                if (gGame.mode != GAME_MODE_DESERT) {
                    rider->landingSegment1 = path_find_segment(&rider->landingPos1, rider->landingSegment1, steer, rider->pathSearch);
                    if (gPathNodes[rider->landingSegment1].flags & PATH_NODE_BAD_LANDING) {
                        score1 -= 500.0f;
                    }
                }
                normalVel = rider->landingNormal1.x * rider->bike.vel.x + rider->landingNormal1.y * (rider->bike.vel.y - rider->landingTime1 * 11.760001f) + rider->landingNormal1.z * rider->bike.vel.z;
                rider->landingTime = (rider->landingTime1 + rider->landingTime2) * 0.5f;
                score1 -= normalVel * normalVel;
                if (score2 < score1) {
                    rider->landingPick = 1;
                    rider->airPitch = -0.3125f;
                } else {
                    slope = dirX;
                    rider->landingPick = 0;
                    rider->airPitch = 0.3125f;
                }
                if (rider->landingTime < 1.5f) {
                    pitchIn = slope * 15.0f + (f32)((s32)(slope * 8192.0f - (f32)rider->drawRot[0]) / 512);
                    if (pitchIn > 15) {
                        pitchIn = 15;
                    } else if (pitchIn < -15) {
                        pitchIn = -15;
                    }
                    rider->cpuAirPitch = pitchIn;
                    rider->airPitchInput = rider->cpuAirPitch;
                }
            }
        }
        if (rider->landingAssist != 0) {
            bike_align_to_ground(rider);
        } else if (rider->landingTime < 0.25f && rider->bike.vel.y < 0.0f && rider->airFrames >= 31 && rider->wheelieTimer == 0) {
            if (rider->isCpu == 0) {
                landPos = rider->bike.pos;
                /* boost and pitchScale hold the landing point's x and z */
                boost = landPos.x + rider->bike.vel.x * rider->landingTime;
                landPos.x = boost;
                pitchScale = landPos.z + rider->bike.vel.z * rider->landingTime;
                landPos.z = pitchScale;
                /* dead tests: they keep the original's register use */
                if (rider == 0) normalVel = 1.0f; else if (steer == 0) normalVel = 2.0f; else normalVel = 3.0f;
                landPos.y = (f32)height_get_ground(landPos.x * 1000.0f, landPos.y * 1000.0f, landPos.z * 1000.0f) / 1000.0f;
                height_get_ground_normal(&vec.x);
                axis.x = rider->bike.up.y * vec.z - vec.y * rider->bike.up.z;
                axis.y = rider->bike.up.z * vec.x - vec.z * rider->bike.up.x;
                axis.z = rider->bike.up.x * vec.y - vec.x * rider->bike.up.y;
                tilt = vec3_dot(&axis.x, &rider->bike.right.x);
                limit = rider->landing * 10.0f;
                if (tilt > 0.0f ? tilt < limit : -tilt < limit) {
                    rider->landingAssist = 1;
                }
            } else {
                rider->landingAssist = 1;
            }
        }
        if (rider->jumpState != -1) {
            rider->jumpState = 1;
            rider->jumpDist = __builtin_sqrtf((rider->bike.pos.x - rider->takeoffPos.x) * (rider->bike.pos.x - rider->takeoffPos.x)
                                      + (rider->bike.pos.y - rider->takeoffPos.y) * (rider->bike.pos.y - rider->takeoffPos.y)
                                      + (rider->bike.pos.z - rider->takeoffPos.z) * (rider->bike.pos.z - rider->takeoffPos.z));
        }
    } else {
        if (rider->airFrames > 0) {
            bike_handle_landing(rider);
        } else {
            rider->airFrames -= gPhysTicks;
        }
        if (rider->jumpState == 1) {
            rider->jumpState = 0;
        }
        rider->takeoffPos = rider->rearWheel.contact;
    }
    rider->takeoffVelY = rider->bike.vel.y;
}

/* The rider's transparency: the ghost, a crashed CPU rider fading out, blinking after a respawn. */
void bike_update_alpha(Rider *rider)
{
    s32 riderIdx;
    s32 alpha;
    u8 timer;
    s8 count;
    s32 *alphas;
    s32 interval;
    s32 *alphaPtr;

    alphas = gRiderAlpha;
    riderIdx = (rider - gRiders);
    alphaPtr = &alphas[riderIdx];
    if (gGhostRace != 0 && gGhostSlot == riderIdx) alpha = 0x80;
    else if (gDemoPlayback != 0) {
        if (riderIdx == 0) alpha = 0x80; else { *alphaPtr = 0xFF; goto crash_fade; }
    } else alpha = 0xFF;
    *alphaPtr = alpha;
crash_fade:
    if (rider->crashState != CRASH_STATE_RIDING && rider->isCpu != 0 && rider->crashTimer > 0x58) {
        gRiderAlpha[rider - gRiders] = (0x78 - rider->crashTimer) * 8;
        return;
    }
    if ((s8)rider->blinkCount != 0) {
        timer = rider->blinkTimer - 1;
        rider->blinkTimer = timer;
        if ((s32)((u32)timer << 24) <= 0) {
            count = rider->blinkCount - 1;
            rider->blinkCount = count;
            if (count == 0 && gGhostRace == 0) {
                if (rider->isCpu == 0) {
                    if ((gCheatFlags & CHEAT_TRANSPARENT_RIDER) == 0) rider->noCollide = 0;
                } else {
                    rider->noCollide = 0;
                }
            }
            interval = (((s32)((u32)rider->blinkCount << 24)) >> 25) - 10;
            rider->blinkTimer = interval;
            if (interval < 4) rider->blinkTimer = 4;
        }
        if (rider->blinkCount & 1) gRiderAlpha[rider - gRiders] = 0;
    }
}

/* One physics step of a rider over `ticks` 60 Hz ticks (bike_crash_update while crashed). */
void bike_update(Rider *rider, s32 ticks)
{
    f32 unused;

    gGroundHeight = 0;
    gGroundSurface = 0;
    gGroundTri = 0;
    gGroundArea = 0;
    gPhysTicks = ticks;
    gPhysDeltaTime = (f32)gPhysTicks * rider->timeScale / 60.0f;
    if (gCrashSlowMotion != 0 && rider->isCpu != 0) {
        gPhysDeltaTime *= 0.5f;
    }
    bike_update_alpha(rider);
    particles_emit_water_splash(rider);
    if (rider->crashState == CRASH_STATE_RIDING) {
        if (rider->pendingCrash > 0) {
            rider->input = 0;
            if (rider->frontWheel.airTicks < 10 && rider->rearWheel.airTicks < 10) {
                bike_crash_begin(rider, rider->pendingCrash);
            }
        }
        if (rider->pendingCrash < 0) {
            bike_crash_begin(rider, -rider->pendingCrash);
        }
        bike_process_input(rider, ticks);
        bike_update_wheelie(rider);
        bike_update_jump(rider);
        if (rider->isCpu != 0) {
            ai_update_throttle(rider);
            rider->dirtSpray = 0;
            if (rider->speed > 5.0f) {
                if (rider->powerSlide != 0 || ((rider->input & INPUT_Z) && gRaceFrame - rider->turboPressFrame < 30)) {
                    rider->dirtSpray = 2;
                } else if (rider->rightFootOut | rider->leftFootOut) {
                    rider->dirtSpray = 1;
                }
            }
            ai_update_steering(rider);
        } else {
            bike_update_steering(rider, ticks);
        }
        if (gEvent.flags & EVENT_STOPPED) {
            rider->walkPhase = 0x3000;
            rider->input |= INPUT_B;
        }
        if (rider->flags & RIDER_FINISHED) {
            if (rider->fwdSpeed > 0.0f) {
                rider->input = INPUT_B;
            } else {
                rider->input = 0;
                rider->stickX = 0;
                if (rider - gRiders == 0) {
                    gEvent.flags |= EVENT_RIDER_STOPPED;
                }
            }
        }
        engine_update(rider - gRiders, ticks);
        bike_update_drive_force(rider);
        bike_update_brakes(rider);
        bike_solve_tire_constraints(rider);
        stunt_update(rider);
        bike_update_tire_forces(rider);
        bike_compute_forces(rider);
        if (rider->crashState == CRASH_STATE_RIDING) {
            bike_integrate_position(rider);
            bike_integrate_velocity(rider);
            bike_wheel_surface_bumps(&rider->frontWheel, &rider->bumpSeed);
            bike_wheel_surface_bumps(&rider->rearWheel, &rider->bumpSeed);
            bike_update_suspension(rider);
            switch (gGame.mode) {
            case GAME_MODE_SOCCER:
            case 4:
            case GAME_MODE_DESERT:
            case GAME_MODE_STUNT_COURSE:
            case GAME_MODE_TEST_OPEN:
                break;
            case GAME_MODE_EXCITE_3D:
                excite3d_update_progress(rider);
                break;
            default:
                path_update_bike(rider);
                break;
            }
            switch (rider->finishState) {
            case 0:
                break;
            case 1:
                if (rider->anim == ANIM_NONE && rider->speed < 8.0f && rider->airFrames < -30) {
                    /* slowed down after the finish: the pose of the finishing place */
                    rider->finishState = 2;
                    stunt_start(rider, rider->race.finishPlace + ANIM_FINISH_1ST);
                    if (!(gGame.flags & GAME_FLAG_REPLAY)) {
                        rider->camMode = CAM_MODE_CELEBRATION;
                    }
                }
                break;
            case 2:
                if (rider->anim < 0) {
                    rider->finishState = 3;
                    if (gGame.mode == GAME_MODE_DESERT) {
                        rider->finishState = 0;
                        rider->isCpu = 1;
                        rider->camMode = (u16)rider->savedCamMode;
                    }
                }
                break;
            }
        } else {
            bike_crash_update(rider);
        }
    } else {
        rider->input = 0;
        engine_update(rider - gRiders, ticks);
        bike_crash_update(rider);
    }
    bike_update_render_pose(rider);
    particles_emit_sparks(rider);
}

/* Applies the player's controller layout to an input word (1: R/Z swapped, 2: B/Z, 3: B->R->Z->B); L is also Z. */
u32 bike_remap_buttons(u32 input, s32 player)
{
    s32 layout;
    u32 out;

    if (input & INPUT_L) input |= INPUT_Z;
    if (player < gGame.numPlayers)
        layout = (s8)gGame.padConfig[gGame.playerPad[player]];
    else
        layout = 0;

    switch (layout) {
    case 0:
        return input;
    case 1:
        out = input & ~(INPUT_R | INPUT_Z);
        if (input & INPUT_R) out |= INPUT_Z;
        if (input & INPUT_Z) out |= INPUT_R;
        return out;
    case 2:
        out = input & ~(INPUT_Z | INPUT_B);
        if (input & INPUT_B) out |= INPUT_Z;
        if (input & INPUT_Z) out |= INPUT_B;
        return out;
    case 3:
        out = input & ~(INPUT_R | INPUT_Z | INPUT_B);
        if (input & INPUT_B) out |= INPUT_R;
        if (input & INPUT_Z) out |= INPUT_B;
        if (input & INPUT_R) out |= INPUT_Z;
        return out;
    }
    return input;
}

/* Runs every rider for a frame of `ticks` ticks (far CPU riders: a double step every other frame), then the
   mode's own update. */
void bikes_update(s32 ticks)
{
    Rider *rider;
    s32 i;
    s32 padInput;
    s32 lastInput;
    s32 pad[2];
    f32 dt, halfDtSq, tk, t2k;
    f32 stepX, stepY, stepZ;

    if (gEvent.flags & EVENT_MESSAGE) {
        return;
    }
    for (i = 0; i < RIDER_COUNT; i++) {
        rider = &gRiders[i];
        if (rider->isCpu == 0) {
            rider->prevInput = rider->input;
            padInput = gPlayerInputs[i];
            rider->input = padInput;
            if (gMirrorMode != 0 && (gGhostRace == 0 || i != gGhostSlot)) {
                rider->input = padInput ^ (INPUT_STICK_LEFT | INPUT_STICK_RIGHT);
            }
        } else {
            lastInput = rider->input;
            rider->stickY = 0;
            rider->stickX = 0;
            rider->input = 0;
            rider->prevInput = lastInput;
        }
    }
    gRaceFrame += ticks;
    if (gDebugFreeze != 0) {
        for (rider = gRiders; rider < gRiders + RIDER_COUNT; rider++) {
            if (rider->inactive == 0) {
                bike_update_render_pose(rider);
            }
        }
    } else {
        for (rider = gRiders; rider < gRiders + RIDER_COUNT; rider++) {
            if (rider->inactive != 0) {
                continue;
            }
            if (rider->frozen != 0) {
                bike_update_render_pose(rider);
                continue;
            }
            if (rider->isCpu != 0 && (rider->airFrames < -30 || (rider->airFrames >= 11 && rider->landingTime > 0.5f))
                && !(rider == gRiders && (gGame.flags & (GAME_FLAG_FRONT_END_RACE | 0x80)))) {
                if (((gRaceFrame ^ (rider - gRiders)) & 1) != 0) {
                    bike_update(rider, 2);
                    rider->bike.smoothPos = rider->bike.pos;
                } else {
                    dt = rider->timeScale / 60.0f;
                    halfDtSq = dt * dt * 0.5f;
                    stepX = rider->bike.vel.x * dt;
                    stepY = rider->bike.vel.y * dt;
                    stepZ = rider->bike.vel.z * dt;
                    rider->bike.smoothPos.x += stepX + rider->bike.acc.x * halfDtSq;
                    rider->bike.smoothPos.y += stepY + rider->bike.acc.y * halfDtSq;
                    rider->bike.smoothPos.z += stepZ + rider->bike.acc.z * halfDtSq;
                    dt *= 1000.0f;
                    halfDtSq *= 1000.0f;
                    rider->drawPos.x = (f32)rider->drawPos.x + (rider->bike.vel.x * dt + rider->bike.acc.x * halfDtSq);
                    rider->drawPos.y = (f32)rider->drawPos.y + (rider->bike.vel.y * dt + rider->bike.acc.y * halfDtSq);
                    rider->drawPos.z = (f32)rider->drawPos.z + (rider->bike.vel.z * dt + rider->bike.acc.z * halfDtSq);
                }
            } else {
                for (i = 0; i < ticks; i++) {
                    bike_update(rider, 1);
                }
                rider->bike.smoothPos = rider->bike.pos;
            }
        }
        switch (gGame.mode) {
        case GAME_MODE_SOCCER:
            soccer_ball_update();
            break;
        case GAME_MODE_DESERT:
            desert_update_race();
            break;
        case GAME_MODE_EXCITE_3D:
            excite3d_update();
            break;
        case GAME_MODE_HILL_CLIMB:
        case GAME_MODE_TUTORIAL:
        case GAME_MODE_STUNT_COURSE:
            break;
        case GAME_MODE_TIME_TRIAL:
            ai_update_lanes();
            race_sort_positions();
            break;
        default:
            race_update_riders();
            break;
        }
    }
    particles_update();
    gRaceFrameDrawn = 0;
}

/* The ground height (m) under pos (m). */
float bike_get_ground_height(float *pos)
{
    float x = pos[0];
    int groundMm = height_get_ground((int)(x * 1000.0f), (int)(pos[1] * 1000.0f), (int)(pos[2] * 1000.0f));
    return (float)groundMm / 1000.0f;
}

/* When a wheel leaves the ground: puts contact points 6 and 7 back where the bike's frame has them. */
void bike_sync_wheel_contacts(Rider *rider)
{
    s32 i;
    Vec3f world;

    sWheelLiftoffFrame = gRaceFrame;
    for (i = 6; i < 8; i++) {
        vec3_mul_mtx3f(&sBikeContactPoints_2[i].x, &rider->bike.right.x, &world.x);
        rider->bike.contacts[i].x = world.x + rider->bike.pos.x;
        rider->bike.contacts[i].y = world.y + rider->bike.pos.y;
        rider->bike.contacts[i].z = world.z + rider->bike.pos.z;
    }
}

/* Frees what bikes_init allocated. */
void bikes_free(void)
{
    s32 i;
    for (i = 0; i < RIDER_COUNT; i++) {
        mem_free(gRiders[i].bikeSkel);
        mem_free(gRiders[i].riderSkel);
    }
    ai_free();
    collide_free_node_pool();
    race_free_extra_rider_spawns();
}

/* Both wheels' surface bumps (bike_update does this inline; unused). */
void bike_apply_surface_bumps(Rider *rider)
{
    bike_wheel_surface_bumps(&rider->frontWheel, &rider->bumpSeed);
    bike_wheel_surface_bumps(&rider->rearWheel, &rider->bumpSeed);
}

/* After the finish: the finishing place's pose and the celebration camera (bike_update does this inline; unused). */
void bike_update_finish_celebration(Rider *rider)
{
    s32 state = rider->finishState;

    if (state == 1) goto slowing;
    if (state < 2) return;
    if (state == 2) goto posing;
    return;

slowing:
    if (rider->anim == ANIM_NONE && rider->speed < 8.0f && rider->airFrames < -30) {
        rider->finishState = 2;
        stunt_start(rider, rider->race.finishPlace + ANIM_FINISH_1ST);
        if ((gGame.flags & GAME_FLAG_REPLAY) == 0) {
            rider->camMode = CAM_MODE_CELEBRATION;
        }
    }
    return;

posing:
    if (rider->anim < 0) {
        rider->finishState = 3;
        if (gGame.mode == GAME_MODE_DESERT) {
            rider->finishState = 0;
            rider->isCpu = 1;
            rider->camMode = (u16)rider->savedCamMode;
        }
    }
}

/* A CPU rider's dirt spray (bike_update does this inline; unused). */
void bike_update_dirt_spray(Rider *rider)
{
    rider->dirtSpray = 0;
    if (5.0f < rider->speed) {
        if (rider->powerSlide != 0 || ((rider->input & INPUT_Z) != 0 && (gRaceFrame - rider->turboPressFrame < 0x1E))) {
            rider->dirtSpray = 2;
            return;
        }
        /* volatile: read in this order, as the original does */
        if ((*(volatile u8 *)&rider->rightFootOut | *(volatile u8 *)&rider->leftFootOut) != 0) {
            rider->dirtSpray = 1;
        }
    }
}
