#include "game.h"

extern CamMode gCamModes[12]; /* here: bikenew.c declares its own view */

/* Resets the cameras for a race: the segment order of the racing line, the histories, zones and pitch springs
   (settled for two seconds). */
void cam_init(s32 unused)
{
    s32 i, j;
    s16 *order;

    mem_fill((u8 *)sCamShakeY, 0, sizeof(sCamShakeY));
    mem_fill((u8 *)sCamFrozen, 0, sizeof(sCamFrozen));
    order = mem_alloc_dbg(gPathNodeCount << 1, 0, (s32)sStrCameraC, 0x239);
    sCamSegOrder = order; /* through a local: the lui after the call */
    cam_build_segment_order();
    mem_fill((u8 *)sCamHistorySum, 0, sizeof(sCamHistorySum));
    mem_fill((u8 *)sCamHistory, 0, sizeof(sCamHistory));
    mem_fill((u8 *)sCamHistoryPos, 0, sizeof(sCamHistoryPos));
    mem_fill((u8 *)gCamZoneState, 0, sizeof(gCamZoneState));
    mem_fill((u8 *)sCamPitchSprings, 0, sizeof(sCamPitchSprings));
    mem_fill((u8 *)sCamAirBlend, 0, sizeof(sCamAirBlend));
    sCamFrameCount = 0;
    for (i = 0; i < 4; i++) {
        gCamPos[i].x = 0;
        gCamPos[i].y = 2992;
        gCamPos[i].z = 0;
        gCamTargets[i] = -1;
        for (j = 0; j < FRAMES(120); j++) {
            sCamPitchSprings[i].vel += (2000.0f - sCamPitchSprings[i].pos) * sCamSpringUp;
            sCamPitchSprings[i].pos += sCamPitchSprings[i].vel;
            sCamPitchSprings[i].vel *= sCamDampUp;
        }
    }
}

/* Numbers the racing line's nodes in driving order, a branch's nodes after the node it leaves, so that a zone
   [startNode, endNode) can be tested with a comparison (sCamSegOrder). */
void cam_build_segment_order(void) {
    s32 j;
    s32 i;
    s32 order;

    i = 0;
    order = 0;
    do {
        sCamSegOrder[i] = order;
        if (gPathNodes[i].branch != -1) {
            j = gPathNodes[i].branch;
            order++;
            do {
                sCamSegOrder[j] = order;
                j = gPathNodes[j].next;
                order++;
            } while (gPathNodes[j].join == -1);
        } else {
            order++;
        }
        i = gPathNodes[i].next;
    } while (i != 0);
}

/* A frame of the camera of view gCamView: the Camera button, the rider select camera, the track's zone cameras in
   replays and demos, else the rider's camera mode; then the smoothed output into gCamPos and the angles, kept on
   the track. */
void cam_update(void) {
    CamOut out;
    Vec3f pos;
    s32 area;
    f32 segT;
    s32 changed, sel;
    f32 *snapVel;
    s32 newMode;
    s32 countdown;
    s32 zoneType;
    s32 target, pad;
    CamZoneState *zone;
    u16 flags;
    CamMode *modeDef;
    Rider *rider;
    f32 dx, dz, d;

    sel = 0;
    changed = 0;
    snapVel = NULL;
    if (gPodiumMode != 0) {
        podium_update_camera();
        return;
    }
    /* remember the player's choice of camera for the controller */
    if (gCamView < gGame.numPlayers && gGame.mode != GAME_MODE_HILL_CLIMB && gGame.mode != GAME_MODE_SOCCER) {
        if (gRiders[gCamView].camMode != CAM_MODE_CELEBRATION) {
            gGame.padCamMode[gGame.playerPad[gCamView]] = gRiders[gCamView].camMode;
        }
    }
    if (gCamTargetOverride == -1) {
        sCamTarget = gCamTargets[gCamView];
    } else {
        sCamTarget = gCamTargetOverride;
    }
    if (sCamFrozen[sCamTarget] != 0) return;
    if (gCamView == 1) {
        if (gInsetViewActive != 0) {
            sCamTarget = 0;
        }
    }
    if (gEvent.state == 0) {
        countdown = gEvent.countdown;
        if (countdown == 0xFF && gCamView == 0) {
            announce_race_start(0);
        }
        /* A or Las Vegas skips the intro sweep */
        if ((gPlayerInputs[0] & INPUT_A) || gCurTrack == &gTracks[TRACK_LAS_VEGAS]) {
            if (countdown >= 2) {
                countdown = 1;
            }
            gEvent.countdown = countdown;
            changed = 1;
        }
    }
    if (gGame.flags & GAME_FLAG_RIDER_SELECT) {
        target = gGame.riders[gCamView];
        sCamTarget = target;
        cam_update_rider_select(&gRiders[target], &out, target != gCamTargets[gCamView]);
        gCamTargets[gCamView] = sCamTarget;
    } else {
        {
            s32 u = sCamTarget;
            s32 *bs = gCamTargets;
            s32 kk = gCamView;
            s32 n;
            if (u != bs[kk]) changed = 1;
            bs[kk] = u;
            n = gRiders[u].camMode;
            if (n != gCamLastModes[kk]) changed = 1;
            gCamLastModes[kk] = n;
        }
        if (gGame.mode == GAME_MODE_TUTORIAL && (gEvent.flags & EVENT_CAM_RESET)) {
            changed = 1;
            gEvent.flags &= ~EVENT_CAM_RESET;
        }
        if (cam_find_zone() != 0) {
            gCamZoneState[gCamView].flags |= CAM_ZONE_STATE_IN;
        } else {
            gCamZoneState[gCamView].flags &= ~CAM_ZONE_STATE_IN;
        }
        if (gDebugFreeze != 0) {
            func_8002D888(&gRiders[sCamTarget], &out); /* empty */
        } else {
            if ((gReplayPlayback != 0 || gCreditsMode != 0 ||
                 (gGame.flags & (GAME_FLAG_FRONT_END_RACE | GAME_FLAG_TRACK_INTRO))) &&
                (gCamZoneState[gCamView].flags & CAM_ZONE_STATE_IN)) {
                if (!(gCamZoneState[gCamView].flags & CAM_ZONE_STATE_ON)) {
                    gCamZoneState[gCamView].flags |= CAM_ZONE_STATE_ON;
                    func_8002D890();
                }
                switch (gCamZoneState[gCamView].type) {
                case 0:
                    cam_zone_update_dolly(&gRiders[sCamTarget], &out);
                    break;
                case 1:
                    cam_zone_update_follow(&gRiders[sCamTarget], &out);
                    break;
                }
            } else {
                { CamZoneState *bs = gCamZoneState; zone = &bs[gCamView]; }
                flags = zone->flags;
                if (flags & CAM_ZONE_STATE_ON) {
                    /* leaving the zone cameras: back to the rider's mode */
                    zoneType = zone->type;
                    zone->flags = flags & 0xFFFE;
                    if (zoneType < 2) if (zoneType >= 0) {
                        changed = 1;
                    }
                    newMode = gRiders[sCamTarget].camMode;
                    sel = 1;
                } else {
                    pad = gCamView;
                    if (gGhostRace != 0 && gGhostSlot == 0) {
                        pad = 1;
                    }
                    /* the Camera button: the next mode the button cycles through */
                    if ((gPlayerInputs[pad] & INPUT_C_UP) && !(gPlayerInputsPrev_3[pad] & INPUT_C_UP) &&
                        !(gEvent.flags & EVENT_MESSAGE) &&
                        gEvent.state != 0 && gEvent.allFinished == 0) {
                        s32 md = gRiders[sCamTarget].camMode;
                        if (md != CAM_MODE_FALL_VIEW && gGame.mode != GAME_MODE_HILL_CLIMB) {
                            sel = 1;
                            /* + 0, written as an expression of md in the original */
                            { Rider *b = (Rider *)((u32)gRiders + ((md & 1) >> 1)); snapVel = &b[sCamTarget].bike.vel.x; }
                            newMode = md;
                            if (gGame.mode == GAME_MODE_EXCITE_3D && gRiders[sCamTarget].finishState == 0) {
                                newMode ^= 6; /* side view and chase mid */
                            } else {
                                do {
                                    newMode++;
                                    if (newMode >= CAM_NUM_MODES) {
                                        newMode = 0;
                                    }
                                } while (gCamModes[newMode].flags & CAM_FLAG_NO_CYCLE);
                            }
                        }
                    }
                }
            }
            if (sel) {
                modeDef = &gCamModes[gRiders[sCamTarget].camMode];
                if (modeDef->flags & CAM_FLAG_SNAP_OUT) {
                    changed = 1;
                }
                if (modeDef->exit != NULL) {
                    modeDef->exit();
                }
                rider = &gRiders[sCamTarget];
                rider->savedCamMode = newMode;
                rider->camMode = newMode;
                gCamLastModes[gCamView] = (s16)newMode;
                modeDef = &gCamModes[rider->camMode];
                if (modeDef->flags & CAM_FLAG_SNAP_IN) {
                    changed = 1;
                }
                if (modeDef->init != NULL) {
                    modeDef->init();
                }
            }
            {
            if (!(gCamZoneState[gCamView].flags & CAM_ZONE_STATE_ON)) {
                modeDef = &gCamModes[gRiders[sCamTarget].camMode];
                if (gInsetViewActive != 0 && gCamView == 1) {
                    modeDef = &gCamModes[CAM_MODE_STUNT_PIP];
                }
                if (modeDef->update != NULL) {
                    modeDef->update(&gRiders[gCamTargetOverride != -1 ? gCamTargetOverride : sCamTarget], &out);
                    if (changed) {
                        if (snapVel != NULL) {
                            cam_snap_to_moving(&out, snapVel);
                        } else {
                            cam_snap_to(&out);
                        }
                    }
                }
            }
            }
        }
    }
    target = gCamView;
    /* the history's sums: positions are 16 times the average */
    gCamPos[target].x = sCamHistorySum[target].x / 16;
    gCamPos[target].y = sCamHistorySum[target].y / 16 + sCamShakeY[target];
    gCamPos[target].z = sCamHistorySum[target].z / 16;
    gCamPitch[target] = sCamHistorySum[target].pitch;
    if (sCamShakeY[target] != 0) {
        dx = (f32)gRiders[target].drawPos.x - (f32)gCamPos[target].x;
        dz = (f32)gRiders[target].drawPos.z - (f32)gCamPos[target].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        gCamPitch[gCamView] += (s32)((f32)sCamShakeY[gCamView] / d * 32768.0f / 3.1415927f);
    }
    gCamRoll[gCamView] = sCamHistorySum[gCamView].roll;
    gCamYaw[gCamView] = sCamHistorySum[gCamView].yaw;
    sCamFrameCount++;
    if (!(gCamZoneState[gCamView].flags & CAM_ZONE_STATE_ON) && gRiders[gCamView].camMode != CAM_MODE_FALL_VIEW) {
        gCamSegFlags[gCamView] = 0;
        cam_clamp_to_track(sCamTarget, gCamView);
        return;
    }
    pos.x = (f32)gCamPos[gCamView].x / 1000.0f;
    pos.y = (f32)gCamPos[gCamView].y / 1000.0f;
    pos.z = (f32)gCamPos[gCamView].z / 1000.0f;
    path_get_segment_and_t(&pos, &area, &segT, gRiders[sCamTarget].segment);
    gCamSegFlags[gCamView] = gPathNodes[area].flags;
}

/* The chase camera's pitch on the ground: from the slope of the ground ahead of the rear wheel (or of the racing
   line six nodes on), at least as steep as the slope under the wheel. */
f32 cam_calc_ground_pitch(Rider *rider)
{
    s32 i;
    s32 node;
    s32 n;
    f32 ahead[3];
    f32 speed;
    f32 pitch;
    f32 groundY;
    f32 dx, dz;
    f32 dirX, dirZ;

    i = rider->segment;
    pitch = 0.0f;
    if ((gPathNodes[i].flags & 1) == 0) {
        speed = rider->bike.vel.x * rider->bike.fwd.x + rider->bike.vel.z * rider->bike.fwd.z;
        if (speed < 2.0f) {
            speed = 2.0f;
        }
        if (16.0f < speed) {
            speed = 16.0f;
        }
        ahead[0] = rider->rearWheel.contact.x + rider->bike.fwd.x * speed;
        ahead[2] = rider->rearWheel.contact.z + rider->bike.fwd.z * speed;
        ahead[1] = rider->rearWheel.contact.y;
        groundY = bike_get_ground_height(ahead);
        if (gGroundFound != 0) {
            pitch = (rider->rearWheel.contact.y - groundY) * (8192.0f / speed);
        }
    } else {
        node = i;
        for (n = 5; n >= 0; n--) {
            node = gPathNodes[node].next;
        }
        dx = gPathNodes[node].pos.x - rider->rearWheel.contact.x;
        dz = gPathNodes[node].pos.z - rider->rearWheel.contact.z;
        groundY = __builtin_sqrtf(dx * dx + dz * dz);
        pitch = -(gPathNodes[node].pos.y - rider->rearWheel.contact.y) * (8192.0f / groundY);
    }
    if (pitch < -2048.0f) {
        pitch = -2048.0f;
    } else if (2048.0f < pitch) {
        pitch = 2048.0f;
    }
    if (rider->wheelieTimer == 0) {
        pitch = pitch + 2000.0f;
    } else {
        pitch = pitch + 0.0f;
    }
    dirX = rider->bike.vel.x;
    dirZ = rider->bike.vel.z;
    speed = __builtin_sqrtf(dirX * dirX + dirZ * dirZ);
    if (speed == 0.0f) {
        speed = 1.0f;
    }
    dirX = dirX / speed;
    dirZ = dirZ / speed;
    speed = rider->rearWheel.normal.x * dirX + rider->rearWheel.normal.z * dirZ;
    if (0.2f < speed) {
        speed = (f32)((s16)math_asini((s32)(speed * 16384.0f)) + 0x400);
        if (pitch < speed) {
            pitch = speed;
        }
    }
    return pitch;
}

/* The chase camera's pitch in the air: looking down at the predicted landing point, kept for 32 frames after
   landing. */
f32 cam_calc_air_pitch(Rider *rider)
{
    f32 pitch, dot;
    f32 half, cosine;
    f32 fVar5;
    f32 toLanding[3];
    f32 normal[3];
    s32 angle;
    f32 s;

    if (rider->airFrames > 0 && 0.25f < rider->landingTime) {
        half = 0.5f;
        toLanding[0] = (rider->landingPos1.x + rider->landingPos2.x) * half - rider->bike.smoothPos.x;
        toLanding[1] = (rider->landingPos1.y + rider->landingPos2.y) * half - rider->bike.smoothPos.y;
        toLanding[2] = (rider->landingPos1.z + rider->landingPos2.z) * half - rider->bike.smoothPos.z;
        pitch = math_atan2f(__builtin_sqrtf(toLanding[0] * toLanding[0] + toLanding[2] * toLanding[2]) + 1.0f, -toLanding[1] - 1.0f);
        pitch = (pitch * 32768.0f) / 3.1415927f;
        if (rider->bike.vel.y < 0.0f) {
            normal[0] = rider->landingNormal1.x + rider->landingNormal2.x;
            normal[1] = rider->landingNormal1.y + rider->landingNormal2.y;
            normal[2] = rider->landingNormal1.z + rider->landingNormal2.z;
            dot = vec3_dot(normal, toLanding);
            cosine = dot * half;
            cosine = cosine / __builtin_sqrtf(toLanding[0] * toLanding[0] + toLanding[1] * toLanding[1] + toLanding[2] * toLanding[2]);
            if (-0.25f < cosine) {
                s = cosine + 0.25f;
                angle = math_asini((s32)(s * 16384.0f));
                pitch = pitch + (f32)(s32)(s16)angle;
            }
        }
        sCamAirPitch[gCamView] = pitch;
    } else if (rider->airFrames >= -31) {
        pitch = sCamAirPitch[gCamView];
    } else {
        pitch = 2000.0f;
    }
    return pitch;
}

/* The rider select screen's camera: on the turning rider, slid aside as the menu animates; closer once the
   player has chosen. */
void cam_update_rider_select(Rider *rider, CamOut *out, s32 changed)
{
    s32 mtx[10];
    s32 ofs[3];
    s32 res[3];
    s32 radius, height;
    s32 slide, depth;
    s32 *rot;
    Game *game = &gGame;

    if (game->flags & GAME_FLAG_PLAYER_READY(gCamView)) {
        radius = 4000;
        height = 0x672;
    } else {
        radius = 6000;
        if (game->numPlayers == 2 && gSplitVertical != 0) {
            radius = 7200;
        }
        height = 0x8FC;
    }
    rot = gGame.riderSelectRot[gCamView];
    out->x = rider->bike.smoothPos.x * 1000.0f + (f32)((gCosTable[((rot[0] >> 4) - 0x400) & 0xFFF] * radius) >> 14);
    out->y = rider->bike.smoothPos.y * 1000.0f + (f32)height;
    slide = 0;
    out->z = rider->bike.smoothPos.z * 1000.0f + (f32)((gCosTable[(rot[0] >> 4) & 0xFFF] * radius) >> 14);
    depth = 0;
    out->pitch = 0xE10;
    out->roll = 0;
    out->yaw = (0x8000U - rot[0]) & 0xFFFF;
    mtx3i_from_angles(mtx, out->pitch, out->roll, out->yaw);
    switch (gGame.numPlayers) {
    case 1:
        switch (gGame.riderSelectState[0]) {
        case 1:
            slide = 0x3E - gGame.riderSelectAnim[0];
            break;
        case 2:
            slide = 0x3E;
            break;
        case 3:
            slide = gGame.riderSelectAnim[0];
            break;
        }
        break;
    case 2:
        slide = 0x19;
        if (gSplitVertical != 0) {
            depth = -0x226;
        } else {
            slide = -0x1E;
            depth = -0x64;
        }
        break;
    default:
        slide = 0x1E;
        depth = -0x1F4;
        break;
    }
    slide = (slide * -0x564) / 0x3E;
    if (gGame.flags & GAME_FLAG_PLAYER_READY(gCamView)) {
        ofs[0] = 0;
        ofs[1] = 0;
        ofs[2] = 0;
    } else {
        ofs[0] = slide + 600;
        ofs[1] = depth;
        ofs[2] = 0;
    }
    vec3i_mul_mtx3i(ofs, mtx, res);
    out->x += res[0];
    out->y += res[1];
    out->z += res[2];
    cam_snap_to(out);
}

/* The heading the chase camera follows: the direction of travel turned towards the bike's facing by yawLag / 32,
   which grows while the bike slides (R with the stick, a power slide); the facing at low speed. */
int cam_calc_bike_yaw(Rider *rider)
{
    struct { u16 heading[2]; s32 diff; u16 velHeading[4]; } polar;
    s32 lag, weight, mag;

    if (rider->crashState != 0) goto done;
    math_to_polar((s32)(-rider->bike.fwd.x * 1000.0f), (s32)(rider->bike.fwd.z * 1000.0f), polar.heading, &polar.diff);
    math_to_polar((s32)(-rider->bike.vel.x * 1000.0f), (s32)(rider->bike.vel.z * 1000.0f), polar.velHeading, &polar.diff);
    polar.diff = (s16)(polar.heading[0] - polar.velHeading[0]);
    if (((rider->input & INPUT_R) == 0) || (rider->airFrames >= 0)) goto normal;
    mag = rider->stickX;
    if (mag < 0) mag = -mag;
    if (mag < 6) goto normal;
    if (rider->yawLag < 0x10) rider->yawLag = rider->yawLag + 1;
    goto after;
normal:
    if (rider->powerSlide == 1) {
        if (rider->yawLag < 0x18) rider->yawLag = rider->yawLag + 1;
    } else {
        mag = polar.diff;
        if (mag < 0) mag = -mag;
        if (mag < 0x800 && rider->yawLag != 0) rider->yawLag = rider->yawLag - 1;
    }
after:
    weight = 0;
    if (rider->airFrames < 0) weight = rider->yawLag;
    if (rider->fwdSpeed < 2.0f) weight = 0x20;
    lag = polar.diff * weight;
    weight = (s16)polar.velHeading[0];
    if (lag < 0) lag = lag + 0x1f;
    polar.diff = weight + (lag >> 5);
    rider->heading = ((u16 *)&polar.diff)[1];
done:
    return rider->heading;
}

/* Soccer: after a goal (EVENT_SCORED: the scoring side) a sweep around the ball or the scorer, else the
   chase camera. Returns what its callees leave in v0: cam_snap_to_2 and cam_update_chase_2 are declared to return
   s32. */
s32 cam_mode_soccer(Rider *rider, CamOut *out) {
    u16 a1;
    s32 ratio;
    u16 a2;
    f32 mm;
    f32 fz;
    s32 goal;
    s32 z;
    s32 scorer;
    s32 ang, q, ang3, q3;

    goal = gEvent.flags & EVENT_SCORED;
    if (goal != 0) {
    scorer = goal - 1;
    if (scorer == ((rider - gRiders) >> ((gGame.numPlayers - 1) >> 1))) {
        switch (goal) {
        case 1:
            mm = 1000.0f;
            q = gEvent.goalTimer * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang = gEvent.goalTimer * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * mm + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent.goalTimer * mm * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * mm - (f32)(((0x4844 - gEvent.goalTimer * 10500 / 180) * COS16(gEvent.goalTimer * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * mm - (f32)out->x, gSoccerBall.pos.z * mm - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * mm - (f32)out->y, &a2, &ratio);
            out->roll = 0;
            out->pitch = a2 - 0x4000;
            out->yaw = -a1;
            break;
        case 2:
            mm = 1000.0f;
            q3 = gEvent.goalTimer * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang3 = gEvent.goalTimer * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * mm - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent.goalTimer * mm * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * mm + (f32)(((0x4844 - gEvent.goalTimer * 10500 / 180) * COS16(gEvent.goalTimer * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * mm - (f32)out->x, gSoccerBall.pos.z * mm - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * mm - (f32)out->y, &a2, &ratio);
            out->roll = 0;
            out->pitch = a2 - 0x4000;
            out->yaw = -a1;
            break;
        default:
            fatal_halt(1);
        }
    } else switch (goal) {
    case 1: {
        Rider *p = &gRiders[scorer];
        out->x = p->bike.pos.x * 1000.0f + (f32)((SIN16(gEvent.goalTimer * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f - (f32)((COS16(gEvent.goalTimer * 14000 / 180 - 7000) * 625) >> 11);
        out->pitch = 1000;
        out->yaw = gEvent.goalTimer * 14000 / 180 - 7000;
        out->roll = 0;
        break; }
    case 2: {
        Rider *p = &gRiders[scorer];
        out->x = p->bike.pos.x * 1000.0f + (f32)((SIN16(7000 - gEvent.goalTimer * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f + (f32)((COS16(7000 - gEvent.goalTimer * 14000 / 180) * 625) >> 11);
        out->pitch = 1000;
        out->yaw = gEvent.goalTimer * 14000 / 180 + 0x64A8;
        out->roll = 0;
        break; }
    }
    return cam_snap_to_2(out);
    } else {
        return cam_update_chase_2(rider, out, -7000);
    }
}

/* The hill climb camera's pitch: from the ground 8 m ahead (clamped differently with the Down Hill cheat). */
f32 cam_calc_hill_climb_pitch(Rider *rider)
{
    f32 pitch;
    f32 ahead[3];
    f32 len;
    f32 groundY;
    f32 t;
    f32 dist;
    f32 lenSq;
    u8 flags;
    s32 *hillPitch = &sCamHillPitch;

    flags = gPathNodes[rider->segment].flags;
    pitch = (f32)*hillPitch;
    if ((flags & 1) == 0) {
        lenSq = rider->bike.fwd.x * rider->bike.fwd.x +
             rider->bike.fwd.z * rider->bike.fwd.z;
        dist = 8.0f;
        len = sqrtf(lenSq);
        ahead[0] = rider->rearWheel.contact.x + (rider->bike.fwd.x * dist) / len;
        ahead[2] = rider->rearWheel.contact.z + (rider->bike.fwd.z * dist) / len;
        ahead[1] = rider->rearWheel.contact.y;
        groundY = bike_get_ground_height(ahead);
        ahead[1] = groundY;
        if (gGroundFound != 0) {
            t = rider->rearWheel.contact.y - groundY;
            pitch = t * 1024.0f;
            if ((gCheatFlags & CHEAT_DOWN_HILL) == 0) {
                if (pitch < -2048.0f) pitch = -2048.0f;
                else if (pitch > 1024.0f) pitch = 1024.0f;
            } else {
                if (pitch < -1024.0f) pitch = -1024.0f;
                else if (pitch > 4096.0f) pitch = 4096.0f;
            }
        }
    }
    return pitch + 2000.0f;
}

/* CAM_MODE_HILL_CLIMB: behind and above the rider; after a crash the camera stays and watches (the crash timer
   runs faster once the rider is more than 10 m away). */
void cam_mode_hill_climb(Rider *rider, CamOut *out)
{
    s32 ofs[4];
    s32 res[4];
    s32 mtx[10];
    s16 ang;
    s32 dist;
    CamOut *hist;
    CamOut *hists;
    s32 camZ, dx, dz;
    V3 *pos;
    s32 z, dy;
    s16 yaw, pitch;

    if (rider->crashState == 0) {
        yaw = (u16)rider->heading;
        pitch = cam_calc_hill_climb_pitch(rider);
        mtx3i_from_angles(mtx, pitch, 0, yaw);
        ofs[0] = 0;
        ofs[1] = 0;
        ofs[2] = -sCamHillDist;
        vec3i_mul_mtx3i(ofs, mtx, res);
        pos = &rider->bike.pos;
        out->x = res[0] + pos->x * 1000.0f;
        out->y = res[1] + pos->y * 1000.0f + sCamHillHeight;
        out->z = res[2] + pos->z * 1000.0f;
        out->pitch = pitch;
        out->roll = 0;
        out->yaw = yaw;
    } else {
        hists = sCamHistorySum;
        hist = &hists[gCamView];
        out->x = hist->x / 16;
        out->y = hist->y / 16;
        z = hist->z;
        if (z < 0) {
            z += 0xF;
        }
        camZ = z >> 4;
        out->z = camZ;
        dx = out->x - (s32)(rider->bike.pos.x * 1000.0f);
        dz = (s32)(rider->bike.pos.z * 1000.0f) - camZ;
        dy = (s32)(rider->bike.pos.y * 1000.0f) + sCamHillHeight - out->y;
        math_to_polar(dx, dz, &ang, &dist);
        if (dist > 10000) {
            rider->crashTimer += 3;
        }
        out->yaw = ang;
        math_to_polar(-dy, dist, &ang, &dist);
        out->pitch = ang;
        out->roll = 0;
    }
    cam_smooth_to(out);
}

/* The chase camera at `dist` mm behind the rider: the pitch from the ground or the landing point on a spring, the
   bike's heading (turned around while C-left is held), the intro's swing in from the side, shake on bumpy ground
   and after a rough landing. */
void cam_update_chase(Rider *rider, CamOut *out, s32 dist)
{
    s32 blend, air, pitch, swing, mode;
    s32 yaw;
    f32 target, ease, vel, damp, groundPitch, airPitch, c, d;
    s32 dy, dz;
    s32 ofs[3];
    s32 res[3];
    s32 mtx[10];
    f32 *pos;
    CamPitchSpring *spring, *springs;
    s32 *q, *blends;
    s32 sideOfs;
    s32 *q2;
    f32 decay;

    air = rider->airFrames;
    blends = sCamAirBlend;
    blend = blends[gCamView];
    if (rider->landingTime * 60.0f + (f32)air > 60.0f) {
        if (blend < 0x20) {
            blend++;
        }
    } else if (air < 0) {
        if (blend == 0) goto first;
        blend--;
    }
    if (blend == 0) {
first:
        target = cam_calc_ground_pitch(rider);
    } else if (blend == 0x20) {
        target = cam_calc_air_pitch(rider);
    } else {
        groundPitch = cam_calc_ground_pitch(rider);
        airPitch = cam_calc_air_pitch(rider);
        target = (groundPitch * (f32)(0x20 - blend) + airPitch * (f32)blend) * 0.03125f;
    }
    sCamAirBlend[gCamView] = blend;
    springs = sCamPitchSprings;
    spring = &springs[gCamView];
    target = target - spring->pos;
    if (target > 0.0f) {
        vel = spring->vel;
        vel += target * sCamSpringUp;
        damp = sCamDampUp;
        spring->vel = vel;
        do { d = vel; } while (0);
    } else {
        vel = spring->vel;
        vel += target * sCamSpringDown;
        damp = sCamDampDown;
        spring->vel = vel;
        do { d = vel; } while (0);
    }
    d *= damp;
    spring->pos += vel;
    do { spring->vel = d; } while (0);
    if ((mode = gGame.mode, yaw = (u16)rider->heading, mode != GAME_MODE_TUTORIAL) &&
        !(gGame.flags & GAME_FLAG_REPLAY)) {
        if ((rider->input & INPUT_C_LEFT) && gEvent.state != 0) {
            yaw += -0x8000;
        }
    }
    pitch = sCamPitchSprings[gCamView].pos;
    if (rider->unk_77C != 0) {
        pitch = -500;
    }
    ofs[0] = 0;
    ofs[1] = 0;
    ofs[2] = dist;
    if (gEvent.state == 0) {
        ease = (f32)gEvent.countdown * 0.00390625f;
        if (ease < 0.5f) {
            ease = 2.0f * (ease * ease);
        } else {
            ease = 1.0f - ease;
            ease = 2.0f * (ease * ease);
            ease = 1.0f - ease;
        }
        swing = ease * 36864.0f;
        sideOfs = gCosTable[(swing >> 4) & 0xFFF] / 8;
        dy = 0;
        dz = -12000;
        if (!(gGame.flags & GAME_FLAG_REPLAY)) {
            yaw += swing;
            ofs[0] = (f32)sideOfs * ease;
            ofs[1] = ease * (f32)dy;
            ofs[2] = (f32)dist + ease * (f32)dz;
        }
    }
    mtx3i_from_angles(mtx, pitch, 0, (s16)yaw);
    vec3i_mul_mtx3i(ofs, mtx, res);
    pos = &rider->bike.pos.x;
    out->x = (f32)res[0] + pos[0] * 1000.0f;
    out->y = (f32)res[1] + pos[1] * 1000.0f + 600.0f;
    out->z = (f32)res[2] + pos[2] * 1000.0f;
    out->pitch = pitch - 500;
    out->yaw = (s16)yaw;
    out->roll = 0;
    if (gGame.mode != GAME_MODE_TUTORIAL) {
        if ((rider->input & INPUT_C_LEFT) ||
            ((gPlayerInputs_2 & INPUT_A) && gEvent.state == 0 && !(gGame.flags & GAME_FLAG_REPLAY))) {
            cam_snap_to(out);
        } else if (rider->prevInput & INPUT_C_LEFT) {
            cam_snap_to_moving(out, &rider->bike.vel.x);
        } else {
            goto other;
        }
    } else {
other:
        cam_smooth_to(out);
    }
    sCamShakeY[gCamView] = 0;
    if (rider->rearWheel.surface != NULL && rider->airFrames < 6 && (rider->rearWheel.surface->flags & SURFACE_SHAKE)) {
        sCamShakeY[gCamView] = -(s32)(rider->rearWheel.bumpNoise * 1000.0f) * 2;
    }
    if (rider->roughLanding != 0) {
        target = rider->roughLanding;
        decay = math_exp(-target * 0.0625f);
        q2 = sCamShakeY;
        target = target * 3.1415927f;
        q = &q2[gCamView];
        *q += (s32)(sinf(target / 6.0f) * 1024.0f * decay);
    }
}

/* CAM_MODE_FALL_VIEW: the camera stays where cam_start_fall_view put it; in the hill climb the fall is slowed
   down until the rider is 12 m below it. */
void cam_mode_fall_view(Rider *rider, CamOut *out)
{
    CamOut *hist;
    s32 i;
    s32 drop;

    hist = sCamHistorySum;
    i = rider - gRiders;
    out->x = hist[i].x / 16;
    out->y = hist[i].y / 16;
    out->z = hist[i].z / 16;
    drop = rider->drawPos.y - out->y;
    out->pitch = hist[i].pitch;
    out->yaw = hist[i].yaw;
    out->roll = 0;
    cam_snap_to(out);
    if (gGame.mode == GAME_MODE_HILL_CLIMB) {
        rider->timeScale = 0.5f;
        if (drop < -12000) {
            rider->crashTimer = 10000;
            rider->timeScale = 1.0f;
        }
    }
}



/* A rider went over a cliff: a fixed camera beside the track (Canyon Chasm's own spot) watches the fall. */
void cam_start_fall_view(Rider *rider)
{
    CamOut cam;
    s32 mtx[9];
    s32 ofs[3];
    s32 res[3];
    s16 ang;
    s32 dist;
    f32 back;
    s32 idx;

    if (rider->camMode != CAM_MODE_FALL_VIEW) {
        gCamLastModes[rider - gRiders] = CAM_MODE_FALL_VIEW;
        rider->camMode = CAM_MODE_FALL_VIEW;
        if (gCurTrack == &gTracks[TRACK_CANYON_CHASM]) {
            cam = sCamFallViewPos;
            math_to_polar(cam.x - rider->drawPos.x, rider->drawPos.z - cam.z, &ang, &dist);
            cam.yaw = ang;
        } else {
            path_update_bike(rider);
            math_to_polar(-(gPathTangentX * 1000.0f), gPathTangentZ * 1000.0f, &ang, &dist);
            if (0.0f < rider->lateral) {
                ang += 0x4000;
            } else {
                ang -= 0x4000;
            }
            rider->heading = ang;
            mtx3i_from_angles(mtx, 0x1000, 0, ang);
            back = gSurfaceTable[COLL_TRI_SURFACE(gGroundSurface)].bumpHeight;
            if (-0.1f < back) {
                back = -0.1f;
            }
            ofs[0] = 0;
            ofs[1] = 0;
            back *= 100000.0f;
            ofs[2] = back;
            vec3i_mul_mtx3i(ofs, mtx, res);
            cam.x = (s32)(rider->bike.pos.x * 1000.0f) + res[0];
            cam.y = (s32)(rider->bike.pos.y * 1000.0f) + res[1] - 1000;
            cam.z = (s32)(rider->bike.pos.z * 1000.0f) + res[2];
            cam.pitch = 0x1000;
            cam.roll = 0;
            cam.yaw = ang;
        }
        gCamView = idx = rider - gRiders;
        cam_snap_to(&cam);
        tunes_play_cliff_fall(idx);
    }
}

/* CAM_MODE_STUNT_PIP: the inset view's camera, orbiting the rider while its timer runs. */
void cam_mode_stunt_pip(Rider *rider, CamOut *out)
{
    s32 ofs[3];
    s32 res[3];
    s32 mtx[10];

    if (rider->pipTimer != 0) {
        if (rider->airFrames <= 0) {
            rider->pipTimer--;
        }
        mtx3i_from_angles(mtx, 0x800, 0, rider->rot[2] + gCamPipOrbitAngle);
        gCamPipOrbitAngle -= 0x40;
        if (gCamPipOrbitAngle < 9000) {
            gCamPipOrbitAngle = 9000;
        }
        ofs[0] = 0;
        ofs[1] = 0;
        ofs[2] = -4000;
        vec3i_mul_mtx3i(ofs, mtx, res);
        out->x = (f32)res[0] + rider->bike.smoothPos.x * 1000.0f;
        out->y = (f32)res[1] + rider->bike.smoothPos.y * 1000.0f;
        out->z = (f32)res[2] + rider->bike.smoothPos.z * 1000.0f;
        out->pitch = 0x800;
        out->yaw = rider->rot[2] + gCamPipOrbitAngle;
        out->roll = 0;
        cam_snap_to(out);
    }
}

/* Adds a camera placement to the view's history: the position becomes the average of the last 16, the angles the
   newest plus the average difference of the others. */
void cam_smooth_to(CamOut *in)
{
    s32 yawSum, rollSum, pitchSum, k, j, slot;

    if (sCamFrameCount == 0) {
        cam_snap_to(in);
        return;
    }
    pitchSum = rollSum = yawSum = 0;
    slot = sCamHistoryPos[gCamView];
    sCamHistorySum[gCamView].x -= sCamHistory[gCamView][slot].x;
    sCamHistorySum[gCamView].y -= sCamHistory[gCamView][slot].y;
    sCamHistorySum[gCamView].z -= sCamHistory[gCamView][slot].z;
    sCamHistory[gCamView][slot] = *in;
    sCamHistorySum[gCamView].x += sCamHistory[gCamView][slot].x;
    sCamHistorySum[gCamView].y += sCamHistory[gCamView][slot].y;
    sCamHistorySum[gCamView].z += sCamHistory[gCamView][slot].z;
    j = slot - 1;
    for (k = 0; k < CAM_HISTORY - 1; k++) {
        if (j < 0) j += CAM_HISTORY;
        yawSum += (s16)(sCamHistory[gCamView][j].yaw - sCamHistory[gCamView][slot].yaw);
        rollSum += (s16)(sCamHistory[gCamView][j].roll - sCamHistory[gCamView][slot].roll);
        pitchSum += (s16)(sCamHistory[gCamView][j].pitch - sCamHistory[gCamView][slot].pitch);
        j--;
    }
    sCamHistorySum[gCamView].pitch = pitchSum / 16 + sCamHistory[gCamView][slot].pitch;
    sCamHistorySum[gCamView].roll = rollSum / 16 + sCamHistory[gCamView][slot].roll;
    sCamHistorySum[gCamView].yaw = yawSum / 16 + sCamHistory[gCamView][slot].yaw;
    sCamHistoryPos[gCamView] = (slot + 1) % CAM_HISTORY;
}

/* Fills the view's history with one placement: the camera jumps there. */
void cam_snap_to(CamOut *src)
{
    s32 i;
    s32 view;
    u8 *base;
    s32 addr;
    CamOut *sum;

    i = 0;
    base = (u8 *)sCamHistory;
    do {
        view = gCamView;
        addr = (i * sizeof(CamOut) + view * sizeof(sCamHistory[0])) + (s32)base;
        i++;
        *(CamOut *)addr = *src;
    } while (i < CAM_HISTORY);
    sCamHistorySum[gCamView] = *src;
    sum = &sCamHistorySum[gCamView];
    sum->x <<= 4;
    sum->y <<= 4;
    sum->z <<= 4;
}

/* Fills the view's history with the placement moving at vel (m/s) over the last 16 frames, so the camera keeps up
   with a moving rider. */
void cam_snap_to_moving(CamOut *out, f32 *vel)
{
    s32 i;
    s32 sx, sy, sz;

    sx = sy = sz = 0;
    for (i = CAM_HISTORY - 1; i >= 0; i--) {
        sCamHistory[gCamView][i] = *out;
        sx += out->x;
        sy += out->y;
        sz += out->z;
        out->x -= (s32)(vel[0] * 16.666666f);
        out->y -= (s32)(vel[1] * 16.666666f);
        out->z -= (s32)(vel[2] * 16.666666f);
    }
    sCamHistorySum[gCamView] = *out;
    sCamHistorySum[gCamView].x = sx;
    sCamHistorySum[gCamView].y = sy;
    sCamHistorySum[gCamView].z = sz;
    sCamHistoryPos[gCamView] = 0;
}

/* gTracks as this file sees it: the track's camera zones */
typedef struct Obj_16 {
    u8 pad0[0x74];
    s16 nA;           /* follow zones */
    s16 nB;           /* dolly zones */
    CamFollowZone *la;
    CamDollyZone *lb;
} Obj_16;

extern Obj_16 *gCurTrack_20;
extern Obj_16 *gCurTrack_43;

/* The zone at byte offset off of the track's lists */
#define FOLLOW_ZONE(off) (*(CamFollowZone *)(off + (s32)P->la))
#define DOLLY_ZONE(off) (*(CamDollyZone *)(off + (s32)P->lb))
#define ZONE_STATE ((CamZoneState *)pf)

/* Finds the track's camera zone holding the target rider's node (follow zones first), then from the node's
   alternative node (zones with CAM_ZONE_EARLY), and sets the view's zone state. Returns 1 when found. */
s32 cam_find_zone(void)
{
    s32 pass = 0;
    s32 node;
    s32 k;
    s32 start, end, v;
    s32 d;
    s32 off;
    s16 *pos;
    Obj_16 *P;
    u16 *pf; /* the view's zone state, by its flags: stores through a u16 keep gCamView's load in the loop */
    CamZoneState *states;
    Event *g;
    s16 *order;
    CamFollowZone *follow;
    s32 start2, end2;
    CamDollyZone *dolly;

    if (gCamView == 0) {
        sCamDollyZoneIndex = -1;
    }
    g = &gEvent;
    states = gCamZoneState;
    P = gCurTrack_20;
    order = sCamSegOrder;
    node = gRiders[sCamTarget].segment;
    do {
        if (node < 0) {
            goto done;
        }
        if (P->la != NULL && (k = 0, P->nA > 0)) {
            pos = (s16 *)(node * 2 + (s32)order);
            pf = (u16 *)(gCamView * 8 + (s32)states);
            off = k;
        loopA:
            {
                if (pass == 0 || (FOLLOW_ZONE(off).flags & CAM_ZONE_EARLY)) {
                    end = order[FOLLOW_ZONE(off).endNode];
                    start = order[FOLLOW_ZONE(off).startNode];
                    if ((end < start && (*pos >= start || *pos < end)) ||
                        (start < end && *pos >= start && *pos < end)) {
                        if (g->noPlayers != 0 || !(FOLLOW_ZONE(off).flags & CAM_ZONE_DEMO_ONLY)) {
                            ZONE_STATE->type = 1;
                            follow = (CamFollowZone *)((u8 *)P->la + off);
                            v = follow != ZONE_STATE->zone.follow;
                            ZONE_STATE->zone.follow = follow;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            return 1;
                        }
                    }
                }
                if (pos == (s16 *)order) v = 1; else v = 2; /* dead stores, as in the original */
                off += sizeof(CamFollowZone);
                k++;
            }
            if (k < gCurTrack_20->nA) goto loopA;
        }
        if ((void *)g == (void *)P) k = 1; else k = 2;
        if (pass == 0) k = 1; else k = 2;
        if (order == 0) k = 1; else k = 2;
        if (P->lb != NULL && (k = 0, P->nB > 0)) {
            pos = (s16 *)(node * 2 + (s32)order);
            d = gCamView;
            pf = (u16 *)(d * 8 + (s32)states);
            off = k;
        loopB:
            {
                if (pass == 0 || (DOLLY_ZONE(off).flags & CAM_ZONE_EARLY)) {
                    end2 = order[DOLLY_ZONE(off).endNode];
                    start2 = order[DOLLY_ZONE(off).startNode];
                    if ((end2 < start2 && (*pos >= start2 || *pos < end2)) ||
                        (start2 < end2 && *pos >= start2 && *pos < end2)) {
                        if (g->noPlayers != 0 || !(DOLLY_ZONE(off).flags & CAM_ZONE_DEMO_ONLY)) {
                            ZONE_STATE->type = 0;
                            dolly = (CamDollyZone *)((u8 *)P->lb + off);
                            v = dolly != ZONE_STATE->zone.dolly;
                            ZONE_STATE->zone.dolly = dolly;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            if (d == 0) {
                                sCamDollyZoneIndex = k;
                            }
                            return 1;
                        }
                    }
                }
                off += sizeof(CamDollyZone);
                k++;
            }
            if (k < gCurTrack_43->nB) goto loopB;
        }
        pass++;
        node = gPathNodes[node].alt;
    } while (pass < 2);
done:
    return 0;
}

/* How far (0-1) the rider is through the zone [startNode, endNode), in driving order. */
f32 cam_zone_get_progress(Rider *rider, s32 startNode, s32 endNode)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (gCamZoneState[gCamView].flags & CAM_ZONE_STATE_EARLY) {
        len = sCamSegOrder[endNode] - sCamSegOrder[startNode];
        seg = path_find_segment(&rider->bike.pos, gPathNodes[rider->segment].alt, 15, 1);
        path_get_lateral_offset(&rider->bike.pos, seg);
        frac = gPathSegmentT;
    } else {
        seg = rider->segment;
        seg = (s16)seg;
        frac = rider->segmentT;
        len = sCamSegOrder[endNode] - sCamSegOrder[startNode];
    }
    if (len < 0) {
        len += gPathNodeCount;
    }
    d = sCamSegOrder[seg] - sCamSegOrder[startNode];
    if (d < 0) {
        d += gPathNodeCount;
    }
    return ((f32)d + frac) / (f32)len;
}

typedef struct {
    s16 h[2];
    s32 w;
} Tmp30;

/* A follow zone's camera: orbiting the rider at a distance, yaw and pitch that sweep across the zone. */
void cam_zone_update_follow(Rider *rider, CamOut *out)
{
    u16 flags;
    CamFollowZone *zone;
    f32 t;
    f32 yaw;
    f32 pitchF;
    u32 yawI;
    u32 pitchI;
    u32 yawSum;
    s32 dist;
    s32 z;
    s32 vec[4];
    s32 res[3];
    s32 mtx[10];
    Tmp30 polar;
    s16 pitch[4];

    flags = gCamZoneState[gCamView].flags;
    if (flags & CAM_ZONE_STATE_NEW) {
        gCamZoneState[gCamView].flags = flags & ~CAM_ZONE_STATE_NEW & 0xFFFF;
    }
    zone = gCamZoneState[gCamView].zone.follow;
    t = cam_zone_get_progress(rider, zone->startNode, zone->endNode);
    dist = zone->dist0 + (s32)((f32)(zone->dist1 - zone->dist0) * t);
    yaw = (f32)zone->yawSweep * t;
    yawI = (u32)yaw;
    pitchF = (f32)zone->pitchSweep * t;
    yawSum = zone->yaw0 + yawI;
    pitchI = (u32)pitchF;
    mtx3i_from_angles(mtx, (-((zone->pitch0 + pitchI) & 0xFFFF)) + 1000, 0,
                  ((u16)rider->rot[2] + yawSum) & 0xFFFF);
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = dist;
    vec3i_mul_mtx3i(vec, mtx, res);
    out->x = (s32)(rider->bike.smoothPos.x * 1000.0f + (f32)res[0]);
    out->y = (s32)(rider->bike.smoothPos.y * 1000.0f + (f32)res[1] + 1000.0f);
    z = (s32)(rider->bike.smoothPos.z * 1000.0f + (f32)res[2]);
    out->z = z;
    math_to_polar((s32)(rider->bike.smoothPos.x * 1000.0f - (f32)out->x),
                  (s32)(rider->bike.smoothPos.z * 1000.0f - (f32)z), polar.h, &polar.w);
    math_to_polar(polar.w, (s32)((rider->bike.smoothPos.y + 0.7f) * 1000.0f - (f32)out->y), pitch, &polar.w);
    out->roll = 0;
    {
        s32 ta = pitch[0] + -0x4000;
        s32 tb = -polar.h[0];
        out->pitch = ta;
        out->yaw = tb;
    }
    cam_snap_to(out);
}

/* A dolly zone's camera: a point (fixed, moving, or following the rider) looking at the rider (CamDollyZone). */
void cam_zone_update_dolly(Rider *rider, CamOut *out)
{
    s32 ofs[3];
    s32 mtx[10];
    s16 a1;
    s32 r1;
    s16 a2;
    s16 b1;
    s32 r2;
    s16 b2;
    CamZoneState *state;
    s32 idx;
    CamDollyZone *zone;
    u16 flags;
    s32 zf;
    s32 rx, rz;
    f32 t, u, v;

    state = gCamZoneState;
    state += gCamView;
    flags = state->flags;
    zone = state->zone.dolly;
    if (flags & CAM_ZONE_STATE_NEW) {
        state->flags = flags & ~CAM_ZONE_STATE_NEW;
        zf = zone->flags;
        if (zf & CAM_DOLLY_FROM_CAM) {
            if (zf & CAM_DOLLY_MOVE) {
                if (zf & CAM_DOLLY_RELATIVE) {
                    zone->x = (f32)gCamPos[gCamView].x - rider->bike.smoothPos.x * 1000.0f;
                    zone->y = (f32)gCamPos[gCamView].y - rider->bike.smoothPos.y * 1000.0f;
                    zone->z = (f32)gCamPos[gCamView].z - rider->bike.smoothPos.z * 1000.0f;
                } else {
                    zone->x = gCamPos[gCamView].x;
                    zone->y = gCamPos[gCamView].y;
                    zone->z = gCamPos[gCamView].z;
                }
            } else {
                zone->x = gCamPos[gCamView].x;
                zone->y = gCamPos[gCamView].y;
                zone->z = gCamPos[gCamView].z;
            }
            zf = zone->flags;
        }
        if (zf & CAM_DOLLY_FIXED) {
            mtx3i_from_angles(mtx, zone->x1, 0, zone->z1);
            ofs[0] = 0;
            ofs[1] = 0;
            ofs[2] = zone->y1;
            vec3i_mul_mtx3i(ofs, mtx, &sCamZoneOffset.x);
        } else if (zf & CAM_DOLLY_RELATIVE) {
            math_to_polar(-zone->x, zone->z, &a1, &r1);
            math_to_polar(r1, -zone->y + 700, &a2, &r1);
            mtx3i_from_angles(mtx, a2 - 0x4000, 0, -a1);
            ofs[0] = 0;
            ofs[1] = 0;
            ofs[2] = zone->y1;
            vec3i_mul_mtx3i(ofs, mtx, &sCamZoneOffset.x);
        }
    }
    t = cam_zone_get_progress(rider, zone->startNode, zone->endNode);
    zf = zone->flags;
    if (zf & CAM_DOLLY_RELATIVE) {
        out->x = (f32)zone->x + rider->bike.smoothPos.x * 1000.0f;
        out->y = (f32)zone->y + rider->bike.smoothPos.y * 1000.0f;
        out->z = (f32)zone->z + rider->bike.smoothPos.z * 1000.0f;
        if (zone->flags & CAM_DOLLY_BACKWARD) {
            u = 1.0f - t;
            out->x = (f32)out->x + (f32)sCamZoneOffset.x * u;
            out->y = (f32)out->y + (f32)sCamZoneOffset.y * u;
            out->z = (f32)out->z + (f32)sCamZoneOffset.z * u;
        } else {
            out->x = (f32)out->x + (f32)sCamZoneOffset.x * t;
            out->y = (f32)out->y + (f32)sCamZoneOffset.y * t;
            out->z = (f32)out->z + (f32)sCamZoneOffset.z * t;
        }
    } else if (zf & CAM_DOLLY_MOVE) {
        out->x = (f32)zone->x + (f32)(zone->x1 - zone->x) * t;
        out->y = (f32)zone->y + (f32)(zone->y1 - zone->y) * t;
        out->z = (f32)zone->z + (f32)(zone->z1 - zone->z) * t;
    } else if (zf & CAM_DOLLY_FIXED) {
        if (zf & CAM_DOLLY_BACKWARD) {
            out->x = (f32)(zone->x + sCamZoneOffset.x) - (f32)sCamZoneOffset.x * t;
            out->y = (f32)(zone->y + sCamZoneOffset.y) - (f32)sCamZoneOffset.y * t;
            out->z = (f32)(zone->z + sCamZoneOffset.z) - (f32)sCamZoneOffset.z * t;
        } else {
            out->x = (f32)zone->x + (f32)sCamZoneOffset.x * t;
            out->y = (f32)zone->y + (f32)sCamZoneOffset.y * t;
            out->z = (f32)zone->z + (f32)sCamZoneOffset.z * t;
        }
    } else {
        out->x = zone->x;
        out->y = zone->y;
        out->z = zone->z;
    }
    if (zone->flags & CAM_DOLLY_FIXED) {
        out->pitch = zone->x1;
        out->roll = 0;
        out->yaw = zone->z1;
    } else {
        math_to_polar(rider->bike.smoothPos.x * 1000.0f - (f32)out->x, rider->bike.smoothPos.z * 1000.0f - (f32)out->z, &b1, &r2);
        math_to_polar(r2, (rider->bike.smoothPos.y + 0.7f) * 1000.0f - (f32)out->y, &b2, &r2);
        out->roll = 0;
        out->pitch = b2 - 0x4000;
        out->yaw = -b1;
    }
    cam_snap_to(out);
}

/* The heading of the racing line at node + frac (frac -1 to 1), from its lateral vectors. */
void cam_get_track_heading(s16 node, f32 frac, u16 *out)
{
    s32 next, cur;
    s32 aux[2];
    f32 latX, latZ;
    if (0.0f <= frac) {
        cur = node;
        next = cur + 1;
        if (next >= gPathNodeCount) next -= gPathNodeCount;
    } else {
        s32 t = node;
        cur = t - 1;
        next = t;
        if (cur < 0) cur += gPathNodeCount;
        frac = frac + 1.0f;
    }
    latZ = (gPathNodes[cur].lateralZ * (1.0f - frac) + gPathNodes[next].lateralZ * frac) * 16384.0f;
    latX = (gPathNodes[cur].lateralX * (1.0f - frac) + gPathNodes[next].lateralX * frac) * 16384.0f;
    math_to_polar((s32)latZ, (s32)latX, out, aux);
}

/* The position of the racing line at node + frac (frac -1 to 1). */
void cam_get_track_pos(s16 node, f32 frac, f32 *x, f32 *y, f32 *z)
{
    s32 i0, i1;
    s32 n;

    if (0.0f <= frac) {
        i0 = node;
        i1 = i0 + 1;
        if (i1 >= gPathNodeCount) {
            i1 -= gPathNodeCount;
        }
    } else {
        n = node;
        i0 = n - 1;
        i1 = n;
        if (i0 < 0) {
            i0 = gPathNodeCount - 1;
        }
        frac = frac + 1.0f;
    }
    *x = gPathNodes[i0].pos.x + (gPathNodes[i1].pos.x - gPathNodes[i0].pos.x) * frac;
    *y = gPathNodes[i0].pos.y + (gPathNodes[i1].pos.y - gPathNodes[i0].pos.y) * frac;
    *z = gPathNodes[i0].pos.z + (gPathNodes[i1].pos.z - gPathNodes[i0].pos.z) * frac;
}

/* Pulls the view's camera in front of the first wall between it and the rider (camWalls: only the walls the
   camera collides with), then keeps it above the ground. */
void cam_collide_walls(s32 rider, s32 view, s32 camWalls)
{
    Rider *r, *riders;
    Vec3i *camPos;
    Vec3f bikePos;
    f32 point[3];
    f32 dir[3];
    f32 normal[4];
    f32 tmax[2];
    f32 sum, len, dist, t;
    Vec3i *cam;

    riders = gRiders;
    r = riders + rider;
    camPos = gCamPos;
    bikePos = r->bike.pos;
    cam = &camPos[view];
    point[0] = (f32)cam->x / 1000.0f;
    point[1] = (f32)cam->y / 1000.0f;
    point[2] = (f32)cam->z / 1000.0f;
    point[0] = point[0] - bikePos.x;
    point[1] = point[1] - bikePos.y;
    point[2] = point[2] - bikePos.z;
    sum = point[0] * point[0] + point[1] * point[1] + point[2] * point[2];
    dist = __builtin_sqrtf(sum);
    if (0.0f < dist) {
        dir[0] = point[0] / dist;
        dir[1] = point[1] / dist;
        dir[2] = point[2] / dist;
    }
    dist = dist + 0.2f;
    point[0] = bikePos.x + dir[0] * dist;
    point[1] = bikePos.y + dir[1] * dist;
    point[2] = bikePos.z + dir[2] * dist;
    height_gather_walls((s32)((bikePos.x - dist) * 1000.0f), (s32)((bikePos.z - dist) * 1000.0f),
                  (s32)((bikePos.x + dist) * 1000.0f), (s32)((bikePos.z + dist) * 1000.0f));
    tmax[0] = 1.0f;
    if (height_raycast_walls(&bikePos, point, normal, tmax, camWalls)) {
        t = tmax[0] * dist - 0.2f;
        point[0] = bikePos.x + t * dir[0];
        point[1] = bikePos.y + t * dir[1];
        point[2] = bikePos.z + t * dir[2];
        cam->x = (s32)(point[0] * 1000.0f);
        cam->y = (s32)(point[1] * 1000.0f);
        tmax[0] = t;
        cam->z = (s32)(point[2] * 1000.0f);
    }
    cam_keep_above_ground(rider, view);
}

typedef struct {
    s16 v;
} Ang;

/* Moves the view's camera by (dx, dz) mm, turning it to keep looking at the rider; history and all when it is
   more than 4 m away. */
void cam_shift_xz(s32 view, s32 rider, s32 dx, s32 dz)
{
    Ang angBefore;
    s32 dist;
    Ang angAfter;
    s32 toX, toZ, turn;
    s32 i;

    toX = gRiders[rider].bike.smoothPos.x * 1000.0f;
    toX -= gCamPos[view].x;
    toZ = gRiders[rider].bike.smoothPos.z * 1000.0f;
    toZ -= gCamPos[view].z;
    math_to_polar(toX, toZ, &angBefore.v, &dist);
    math_to_polar(toX - dx, toZ - dz, &angAfter.v, &dist);
    gCamPos[view].x += dx;
    gCamPos[view].z += dz;
    gCamYaw[view] -= (turn = angAfter.v - angBefore.v);
    if (dist > 0x1000) {
        for (i = 0; i < CAM_HISTORY; i++) {
            sCamHistory[view][i].x += dx;
            sCamHistory[view][i].z += dz;
            sCamHistory[view][i].yaw -= turn;
        }
        sCamHistorySum[view].x += dx * 16;
        sCamHistorySum[view].z += dz * 16;
    }
}

/* gTracks as this file sees it: the environment type */
typedef struct { u8 p0[0x2C]; s32 envType; u8 p30[0xA0 - 0x30]; } Trk_9;

extern Trk_9 *gCurTrack_21;
extern Trk_9 gTracks_9[];

static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return a + (b - a) * t;
}

static inline f32 lerpi(s32 a, s32 b, f32 t)
{
    return (f32)a + (f32)(b - a) * t;
}

/* Keeps the view's camera on the track: the desert only above the ground; stadiums out of the walls of a few
   tracks; outdoor tracks under the racing line's ceiling and within its camera corridor, or out of the walls
   where the node says so. */
void cam_clamp_to_track(s32 rider, s32 view)
{
    Vec3f v;
    s32 seg;
    f32 t;
    s16 angBefore;
    s32 ratio;
    s16 angAfter;
    s32 widthNeg;
    Rider *pb;
    Vec3i *q;
    PathNode *node, *next;
    s32 nextIdx;
    PathNode *nodes;
    f32 ax, az, bx, bz, qx, qz;
    s32 k, ceilingY, widthPos, push, horiz, lateral;
    f32 latZ, latX, sx, sz;

    if (gDebugFreeze != 0) {
        return;
    }
    if (gCurTrack_21->envType == TRACK_ENV_DESERT) {
        cam_keep_above_ground(rider, view);
        return;
    }
    {
        v.x = (f32)gCamPos[view].x / 1000.0f;
        v.y = (f32)gCamPos[view].y / 1000.0f;
        v.z = (f32)gCamPos[view].z / 1000.0f;
        path_get_segment_and_t(&v, &seg, &t, gRiders[rider].segment);
        nodes = gPathNodes;
        node = (PathNode *)(seg * sizeof(PathNode) + (u32)nodes);
        nextIdx = node->next;
        gCamSegFlags[view] = node->flags;
        if (gCurTrack_21->envType == TRACK_ENV_STADIUM) {
            s32 id = gCurTrack_21 - gTracks_9;
            if (id == TRACK_STUNT_COURSE || id == TRACK_USER || id == TRACK_SOCCER) {
                cam_collide_walls(rider, view, 1);
            }
            return;
        }
        sCamUnderCeiling = 0;
        if (node->camCeiling < 0) {
            cam_collide_walls(rider, view, 0);
            return;
        }
        if (node->camCeiling > 0) {
            next = (PathNode *)(nextIdx * sizeof(PathNode) + (u32)nodes);
            if (next->camCeiling > 0) {
                f32 ya, yb;
                sCamUnderCeiling = 1;
                ya = node->pos.y * 1000.0f + (f32)node->camCeiling * 16.0f;
                yb = next->pos.y * 1000.0f + (f32)next->camCeiling * 16.0f;
                widthNeg = (s32)(lerpi(node->camWidthNeg, next->camWidthNeg, t) * 16.0f);
                widthPos = (s32)(lerpi(node->camWidthPos, next->camWidthPos, t) * 16.0f);
                ceilingY = (s32)lerp(ya, yb, t);
                if (ceilingY < gCamPos[view].y && gRiders[rider].airFrames < FRAMES(30)) {
                    f32 ex, ez;
                    ex = gRiders[rider].bike.smoothPos.x * 1000.0f - (f32)gCamPos[view].x;
                    ez = gRiders[rider].bike.smoothPos.z * 1000.0f - (f32)gCamPos[view].z;
                    ex = __builtin_sqrtf(ex * ex + ez * ez);
                    latX = gRiders[rider].bike.smoothPos.y;
                    latX *= 1000.0f;
                    horiz = (s32)ex;
                    latX -= (f32)gCamPos[view].y;
                    math_to_polar((s32)latX, horiz, &angBefore, &ratio);
                    latX += (f32)(gCamPos[view].y - ceilingY);
                    math_to_polar((s32)latX, horiz, &angAfter, &ratio);
                    gCamPitch[view] -= angAfter - angBefore;
                    for (k = 0; k < CAM_HISTORY; k++) {
                        sCamHistory[view][k].y += ceilingY - gCamPos[view].y;
                        sCamHistory[view][k].pitch -= angAfter - angBefore;
                    }
                    sCamHistorySum[view].y += (ceilingY - gCamPos[view].y) * 16;
                    gCamPos[view].y = ceilingY;
                }
                {
                PathNode *pr2, *pn2;
                PathNode *tab2;
                f32 ax, az;
                tab2 = gPathNodes;
                pr2 = (PathNode *)(seg * sizeof(PathNode) + (u32)tab2);
                nextIdx = pr2->next;
                pn2 = (PathNode *)(nextIdx * sizeof(PathNode) + (u32)tab2);
                latX = lerp(pr2->lateralX, pn2->lateralX, t);
                latZ = lerp(pr2->lateralZ, pn2->lateralZ, t);
                ax = pr2->pos.x * 1000.0f;
                az = pr2->pos.z * 1000.0f;
                lateral = (s32)(lerp((f32)gCamPos[view].x - ax, (f32)gCamPos[view].x - pn2->pos.x * 1000.0f, t) * latX +
                           lerp((f32)gCamPos[view].z - az, (f32)gCamPos[view].z - pn2->pos.z * 1000.0f, t) * latZ);
                }
                if (widthPos < lateral) {
                    push = widthPos - lateral;
                } else if (lateral < -widthNeg) {
                    push = -widthNeg - lateral;
                } else {
                    cam_keep_above_ground(rider, view);
                    return;
                }
                cam_shift_xz(view, rider, (s32)((f32)push * latX), (s32)((f32)push * latZ));
            }
        }
    }
    cam_keep_above_ground(rider, view);
}

/* Keeps the view's camera 200 mm above the ground, tilting it to keep looking at the same point. */
void cam_keep_above_ground(s32 rider, s32 view)
{
    Vec3i *base = gCamPos;
    Vec3i *cam = &base[view];
    s32 minY;
    f32 horiz;
    f32 sinP, tanP;
    f32 dx;
    f32 dz;
    s32 pitch;
    s32 dist;

    minY = height_get_ground(cam->x, cam->y + 0x1F4, cam->z);
    minY = minY + 200;
    if (gGroundFound != 0 && cam->y < minY) {
        dx = gRiders[rider].bike.smoothPos.x * 1000.0f - (f32)cam->x;
        dz = gRiders[rider].bike.smoothPos.z * 1000.0f - (f32)cam->z;
        horiz = sqrtf(dx * dx + dz * dz);
        tanP = (f32)gCamPitch[view] * 3.1415925f * 3.0517578e-05f;
        dx = horiz;
        sinP = sinf(tanP);
        tanP = cosf(tanP);
        tanP = (sinP / tanP) * dx - (f32)(cam->y - minY);
        math_to_polar((s32)tanP, (s32)dx, (s16 *)&pitch, &dist);
        gCamPitch[view] = (s32)((s16 *)&pitch)[0];
        cam->y = minY;
    }
}

/* Moves the view's camera and its history by dz mm, updates it and the section under the rider. */
void cam_shift_z(s32 view, s32 dz)
{
    s32 i;

    sCamHistorySum[view].z += dz * 16;
    for (i = 0; i < CAM_HISTORY; i++) {
        sCamHistory[view][i].z += dz;
    }
    gCamView = view;
    cam_update();
    gGroundFound = 0;
    height_get_ground(gRiders[view].drawPos.x, gRiders[view].drawPos.y, gRiders[view].drawPos.z);
    if (gGroundFound != 0) {
        gRiderSection[view] = gGroundArea;
    }
}

/* The chase camera 7 m behind. */
void cam_mode_chase_7m(Rider *rider, CamOut *out)
{
    cam_update_chase(rider, out, -7000);
}

/* CAM_MODE_CHASE_FAR_ALT: the chase camera 10 m behind. */
void cam_mode_chase_far_alt(Rider *rider, CamOut *out)
{
    cam_update_chase(rider, out, -10000);
}

/* CAM_MODE_SIDE_VIEW (Excite 3D): from the side of the track at a fixed x, following the rider's z. */
void cam_mode_side_view(Rider *rider, CamOut *out)
{
    f32 k = 1000.0f;

    out->x = (s32)-sCamSideViewParams[0];
    out->y = (s32)(rider->bike.smoothPos.y * k + sCamSideViewParams[1]);
    out->z = (s32)(rider->bike.smoothPos.z * k - sCamSideViewParams[2]);
    out->pitch = (s32)sCamSideViewParams[4];
    out->yaw = (s32)-sCamSideViewParams[3];
    out->roll = 0;
    cam_snap_to(out);
}

/* CAM_MODE_HIGH_SIDE: high up to the side, looking down at the rider. */
void cam_mode_high_side(Rider *rider, CamOut *out) {
    out->x = rider->bike.smoothPos.x * 1000.0f + -28000.0f;
    out->y = 0x5BCC;
    out->z = rider->bike.smoothPos.z * 1000.0f - 10000.0f;
    out->pitch = 0x1800;
    out->yaw = -0x4000;
    out->roll = 0;
    cam_smooth_to(out);
}

/* CAM_MODE_CHASE_NEAR: 2.5 m behind (5 m after a crash), further when riding backwards. */
void cam_mode_chase_near(Rider *rider, CamOut *out) {
    s32 dist = -0x9C4;

    if (rider->crashState != 0) {
        dist = -5000;
    }
    if (rider->fwdSpeed < 0.0f) {
        dist = (s32)((f32)dist + rider->fwdSpeed * 250.0f);
    }
    cam_update_chase(rider, out, dist);
}

/* CAM_MODE_CHASE_MID: 5 m behind. */
void cam_mode_chase_mid(Rider *rider, CamOut *out)
{
    cam_update_chase(rider, out, -5000);
}

/* CAM_MODE_CHASE_FAR: 10 m behind. */
void cam_mode_chase_far(Rider *rider, CamOut *out) {
    cam_update_chase(rider, out, -10000);
}

/* gTracks as this file sees it: the environment type */
typedef struct { u8 p[0x2C]; s32 envType; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *gCurTrack_22;

/* CAM_MODE_CELEBRATION: circling the rider after the finish (higher on outdoor tracks). */
void cam_mode_celebration(Rider *rider, CamOut *out)
{
    Stk st;
    s16 pitch;
    s16 yaw;
    s32 angle;

    pitch = 2000;
    if (gCurTrack_22->envType == TRACK_ENV_OUTDOOR) {
        pitch = 6000;
    }
    angle = (u16)rider->drawRot[2] + 0x7000;
    yaw = angle;
    if (rider->finishState > 1) {
        yaw = angle - rider->celebrationTimer * 0x40;
        rider->celebrationTimer = rider->celebrationTimer + 1;
    }
    st.l60[0] = 0;
    st.l60[1] = 0;
    st.l60[2] = -5000;
    mtx3i_from_angles(st.arr, pitch, 0, yaw);
    vec3i_mul_mtx3i(st.l60, st.arr, &st.l50);
    out->x = (s32)((f32)st.l50 + rider->bike.smoothPos.x * 1000.0f);
    out->y = (s32)((f32)st.l4c + rider->bike.smoothPos.y * 1000.0f + 750.0f);
    out->z = (s32)((f32)st.l48 + rider->bike.smoothPos.z * 1000.0f);
    out->pitch = pitch;
    out->yaw = yaw;
    out->roll = 0;
    cam_snap_to(out);
}

/* CAM_MODE_FIRST_PERSON: 400 mm above the bike, turning with it (half its pitch, a quarter of its roll). */
void cam_mode_first_person(Rider *rider, CamOut *out)
{
    s32 ofs[4];
    s32 res[3];
    s32 mtx[10];
    s32 roll;

    mtx3i_from_angles(mtx, rider->drawRot[0], rider->drawRot[1], rider->drawRot[2]);
    ofs[0] = 0;
    ofs[1] = 400;
    ofs[2] = 0;
    vec3i_mul_mtx3i(ofs, mtx, res);
    out->x = (s32)((f32)res[0] + rider->bike.smoothPos.x * 1000.0f);
    out->y = (s32)((f32)res[1] + rider->bike.smoothPos.y * 1000.0f);
    out->z = (s32)((f32)res[2] + rider->bike.smoothPos.z * 1000.0f);
    out->pitch = rider->drawRot[0] / 2;
    out->yaw = rider->drawRot[2];
    roll = rider->drawRot[1];
    if (roll < 0) {
        roll += 3;
    }
    out->roll = roll >> 2;
    cam_snap_to(out);
}

/* Empty: the camera of the debug freeze, and two other stubs. */
void func_8002D888(Rider *arg0, CamOut *arg1)
{
}

void func_8002D890(void)
{
}

void func_8002D898(void)
{
}

/* Frees the segment order. */
void cam_free(void) {
    mem_free(sCamSegOrder);
}

/* How much the racing line turns over the node before to the node after node + t. */
void cam_get_track_turn(s16 node, f32 t, u16 *out)
{
    u16 headings[2];
    s32 prev, next;

    prev = node - 1;
    if (prev < 0) {
        prev += gPathNodeCount;
    }
    next = node + 1;
    if (gPathNodeCount <= next) {
        next -= gPathNodeCount;
    }
    cam_get_track_heading((s16)prev, t, &headings[0]);
    cam_get_track_heading((s16)next, t, &headings[1]);
    *out = headings[1] - headings[0];
}

/* The slope of the racing line at node + frac (frac -1 to 1), in 0x10000 per turn. */
void cam_get_track_slope(s32 node, f32 frac, s16 *out)
{
    s16 i;
    s32 next, cur;
    f32 slope;
    PathNode *base;
    i = node;
    if (0.0f <= frac) {
        cur = i;
        next = i + 1;
        if (gPathNodeCount <= next) {
            next = next - gPathNodeCount;
        }
    } else {
        s32 t;
        t = i;
        cur = t - 1;
        next = t;
        if (cur < 0) {
            cur = cur + gPathNodeCount;
        }
        frac = frac + 1.0f;
    }
    base = gPathNodes;
    slope = base[cur].slope * (1.0f - frac) + base[next].slope * frac;
    *out = (s16)((slope * 32768.0f) / 3.1415927f);
}

/* Freezes or releases the camera following a rider. */
void cam_set_frozen(s32 rider, s32 frozen)
{
    sCamFrozen[rider] = frozen;
}
