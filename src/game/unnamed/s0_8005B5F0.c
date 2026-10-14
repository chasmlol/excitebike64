#include "game.h"

/* Sets a CPU rider's throttle, brake or turbo to hold the racing line's target speed at its position (stop or slow
   down once finished; brake to a stop in the stunt course and hill climb). */
void ai_update_throttle(Rider *rider)
{
    s32 nextSeg;
    f32 targetSpeed;
    f32 excess;
    u32 mode;
    u8 finishState;

    nextSeg = rider->segment + 1;
    targetSpeed = gPathNodes[rider->segment].maxSpeed * (1.0f - rider->segmentT)
                + gPathNodes[nextSeg % gPathNodeCount].maxSpeed * rider->segmentT;
    mode = gGame.mode;
    if (mode == GAME_MODE_STUNT_COURSE) goto brake_to_stop;
    if (mode < 11) {
        if (mode == GAME_MODE_HILL_CLIMB) goto brake_to_stop;
    } else if (mode == 0xF) {
        targetSpeed = 50.0f;
    }
    goto race;

brake_to_stop:
    if (!(0.5f < rider->fwdSpeed)) goto coast;
brake:
    rider->input = INPUT_B;
    return;
coast:
    rider->input = 0;
    return;

race:
    finishState = rider->finishState;
    if (finishState == 1) goto stop;
    if (finishState == 2) goto slow;
    goto match_speed;
stop:
    targetSpeed = 0.0f;
    goto match_speed;
slow:
    targetSpeed = 5.0f;
match_speed:
    excess = rider->speed - targetSpeed;
    if (0.5f < excess) {
        rider->frontWheel.brakeRamp = 6;
        rider->rearWheel.brakeRamp = 6;
        goto brake;
    }
    if (excess < -0.5f && rider->airFrames < 3 && rider->laneBlocked == 0) {
        if (excess < -5.0f && rider->engine.heat < 9500.0f) {
            if ((rider->prevInput & INPUT_Z) == 0) {
                rider->turboPressFrame = gRaceFrame;
            }
            rider->input = INPUT_Z;
            return;
        }
        rider->input = INPUT_A;
    }
    return;
}

/* Sets a CPU rider's steering angle (radians), clamped to half the bike's limit at its speed, and the stick x that
   matches it. */
void ai_set_steering(Rider *rider, f32 angle)
{
    f32 speed = rider->speed;
    s32 i = 0;
    f32 t;
    f32 maxAngle;

    while (sSteerSpeedSteps[i + 1] < speed) {
        i++;
    }
    if (speed < 0.0f) {
        t = 0.0f;
    } else {
        t = (sSteerSpeedSteps[i + 1] - speed) / (sSteerSpeedSteps[i + 1] - sSteerSpeedSteps[i]);
    }
    maxAngle = (sSteerMaxBySpeed[i] * t + sSteerMaxBySpeed[i + 1] * (1.0f - t)) * 0.18849557638168335f * 0.5f;
    if (maxAngle < angle) {
        angle = maxAngle;
    } else if (maxAngle < -angle) {
        angle = -maxAngle;
    }
    rider->steer = angle;
    rider->stickX = (s32)((angle * 13.0f) / maxAngle);
}

/* Steers a CPU rider: power slides through the corners flagged for them, follows crests, aims at the landing point
   in the air, and otherwise steers along a curve from the bike to a point on its lane a few path nodes ahead. */
void ai_update_steering(Rider *rider)
{
    f32 unused[8]; /* keeps the original's frame size */
    s16 heading;
    s32 polarLen;
    s16 pathAngle;
    s32 seg;
    s32 lookahead;
    s32 k;
    s32 node;
    s32 nextNode;
    f32 offset;
    f32 t;
    f32 tSq;
    f32 tCube;
    f32 aimX, aimZ;
    f32 startX, startZ;
    f32 c3x;
    f32 drift;
    f32 dx, dz;
    f32 len, l3;
    f32 c2x, c2z, c1x, c1z, c3z;
    f32 d, handle;
    f32 midX, midZ, along;
    f32 lineDirX, lineDirZ, tanZ, tanX;
    f32 dirX, dirZ, mag;
    f32 r, steer;
    f32 ctrl1X, ctrl1Z;
    f32 ang, s, sinA, across, cosA;
    Vec3f *pos;
    PathNode *n;

    switch (gGame.mode) {
    case GAME_MODE_EXCITE_3D:
        excite3d_cpu_steer(rider);
        break;
    case GAME_MODE_DESERT:
        desert_ai_steer(rider);
        return;
    case GAME_MODE_STUNT_COURSE:
        ai_set_steering(rider, 0.0f);
        rider->stickX = 0;
        break;
    }

    switch (rider->powerSlide) {
    case POWER_SLIDE_NONE: {
        s32 curSeg = rider->segment;
        s32 absDiff;
        /* start a slide in a flagged corner when heading within 22.5 degrees of the line */
        if (!(gPathNodes[curSeg].flags & 4) || gRaceFrame - rider->aiFrame < 300) {
            break;
        }
        math_to_polar(-(rider->bike.fwd.x * 16384.0f), rider->bike.fwd.z * 16384.0f, &heading, &polarLen);
        math_to_polar(gPathNodes[curSeg].lateralZ * 16384.0f, gPathNodes[curSeg].lateralX * 16384.0f, &pathAngle, &polarLen);
        pathAngle -= heading;
        absDiff = pathAngle < 0 ? -pathAngle : pathAngle;
        if (absDiff > 0x1000) {
            break;
        }
        rider->powerSlide = POWER_SLIDE_ON;
    } /* fallthrough */
    case POWER_SLIDE_ON: {
        s32 curSeg, exitNode, absDiff;
        s16 angDiff;
        if (rider->speed < 5.0f) {
            rider->powerSlide = POWER_SLIDE_END;
            break;
        }
        curSeg = rider->segment;
        if (!(gPathNodes[curSeg].flags & 4)) {
            rider->powerSlide = POWER_SLIDE_END;
            break;
        }
        /* slide (B, no throttle) until heading within 5.6 degrees of the line at the corner's exit */
        exitNode = curSeg;
        do {
            exitNode = gPathNodes[exitNode].next;
        } while (gPathNodes[exitNode].flags & 4);
        math_to_polar(-(rider->bike.fwd.x * 16384.0f), rider->bike.fwd.z * 16384.0f, &heading, &polarLen);
        math_to_polar(gPathNodes[exitNode].lateralZ * 16384.0f, gPathNodes[exitNode].lateralX * 16384.0f, &pathAngle, &polarLen);
        pathAngle -= heading;
        angDiff = pathAngle;
        absDiff = angDiff < 0 ? -angDiff : angDiff;
        if (absDiff > 0x400) {
            steer = gPathNodes[exitNode].pos.x - rider->bike.pos.x;
            steer = (steer * gCosTable[(heading >> 4) & 0xFFF]
                 + (gPathNodes[exitNode].pos.z - rider->bike.pos.z) * gCosTable[((heading >> 4) - 0x400) & 0xFFF]) * (1.0f / 16384.0f);
            if (angDiff < 0) {
                rider->stickX = -13;
                steer = -steer;
            } else {
                rider->stickX = 13;
            }
            if (steer > -2.0f) {
                rider->stickX = 0;
            }
            rider->powerSlide = POWER_SLIDE_ON;
            rider->input = (rider->input & ~INPUT_A) | INPUT_B;
        } else {
            rider->powerSlide = POWER_SLIDE_END;
        }
        steer = rider->stickX > 0 ? 0.3f : -0.3f;
        ai_set_steering(rider, steer);
        return;
    }
    case POWER_SLIDE_END:
        rider->powerSlide = POWER_SLIDE_NONE;
        rider->aiFrame = gRaceFrame;
        rider->laneOffset = rider->lateral;
        break;
    }

    offset = rider->laneOffset;
    if (rider->airFrames < 3) {
        pos = &rider->bike.pos;
        seg = rider->segment;
        t = rider->segmentT;
        lookahead = 2;
        if (gRaceFrame - rider->aiFrame < 300) {
            lookahead = 4;
        } else {
            k = gPathNodes[seg].lookahead;
            if (lookahead < k) {
                t = 0.0f;
                lookahead = k;
            }
        }
    } else if (gGame.mode != GAME_MODE_EXCITE_3D) {
        /* in the air: aim at the predicted landing point, or steer back onto the track if it is off it */
        pos = rider->landingPick ? &rider->landingPos1 : &rider->landingPos2;
        seg = rider->landingPick ? rider->landingSegment1 : rider->landingSegment2;
        r = path_get_lateral_offset(pos, seg);
        if (!(gPathNodes[seg].widthPos < r)) {
            if (!(r < -gPathNodes[seg].widthNeg)) {
                t = gPathSegmentT;
                lookahead = 2;
                if (lookahead < gPathNodes[seg].lookahead) {
                    t = 0.0f;
                    lookahead = gPathNodes[seg].lookahead;
                }
            } else {
                rider->stickX = -8;
                return;
            }
        } else {
            rider->stickX = 8;
            return;
        }
    } else {
        rider->stickX = 0;
        return;
    }

    /* a crest one or two nodes ahead: steer against the sideways part of the velocity */
    if ((u32)((u8)rider->crestAhead - 1) < 2 && rider->speed > 2.0f) {
        seg = rider->segment;
        for (k = rider->crestAhead; k > 0; k--) {
            seg = gPathNodes[seg].next;
        }
        drift = (rider->bike.vel.x * gPathNodes[seg].lateralX + rider->bike.vel.z * gPathNodes[seg].lateralZ) / rider->speed;
        rider->stickX = drift * 32.0f;
        if (rider->stickX >= 16) {
            rider->stickX = 15;
        } else if (rider->stickX < -15) {
            rider->stickX = -15;
        }
        ai_set_steering(rider, drift * 8.0f / (rider->speed + 5.0f));
        if (rider == &gRiders[gCamTargetOverride]) {
            gAiDebugTargetPos = gPathNodes[seg].pos;
            gAiDebugTargetDir.z = 2.0f * gPathNodes[seg].lateralX;
            gAiDebugTargetDir.y = 0;
            gAiDebugTargetDir.x = 2.0f * -gPathNodes[seg].lateralZ;
        }
        return;
    }

    switch ((s32)rider->pathSearch) {
    case 2:
        node = seg;
        while (lookahead != 0) {
            lookahead--;
            node = gPathNodes[node].branch >= 0 ? gPathNodes[node].branch : gPathNodes[node].next;
        }
        break;
    case 0:
    case 1:
    default:
        node = seg;
        while (lookahead != 0) {
            lookahead--;
            node = gPathNodes[node].next;
        }
        break;
    }

    /* the point at t on the lane between node and the next one (a cubic along the racing line) */
    rider->laneOffset = offset;
    nextNode = gPathNodes[node].next;
    startX = gPathNodes[node].pos.x + gPathNodes[node].lateralX * offset;
    startZ = gPathNodes[node].pos.z + gPathNodes[node].lateralZ * offset;
    dx = (gPathNodes[nextNode].pos.x + gPathNodes[nextNode].lateralX * offset) - startX;
    dz = (gPathNodes[nextNode].pos.z + gPathNodes[nextNode].lateralZ * offset) - startZ;
    tSq = t * t;
    tCube = tSq * t;
    len = __builtin_sqrtf(dx * dx + dz * dz);

    {
        f32 t1, s1, t2, s2, a1, b1, acx, acz, ds1;
        t1 = gPathNodes[node].lateralZ * -3.0f;
        s1 = gPathNodes[nextNode].lateralZ + gPathNodes[node].lateralZ;
        d = len / 3.0f;
        ds1 = d * s1;
        c1x = t1 * d;
        t2 = gPathNodes[node].lateralX * 3.0f;
        s2 = d * (gPathNodes[nextNode].lateralX + gPathNodes[node].lateralX);
        c1z = t2 * d;
        a1 = (dx + ds1) * 3.0f;
        b1 = (dz - s2) * 3.0f;
        c2x = a1 - c1x;
        c3x = (dx - c2x) - c1x;
        c2z = b1 - c1z;
        c3z = (dz - c2z) - c1z;
        aimX = startX + tCube * c3x + tSq * c2x + t * c1x;
        aimZ = startZ + tCube * c3z + tSq * c2z + t * c1z;
    }
    d = __builtin_sqrtf((aimX - pos->x) * (aimX - pos->x) + (aimZ - pos->z) * (aimZ - pos->z));

    /* curvature of the Bezier from the bike, along its travel direction, to that point along the line */
    lineDirX = tSq * 3.0f * c3x + 2.0f * t * c2x + c1x;
    lineDirZ = tSq * 3.0f * c3z + 2.0f * t * c2z + c1z;
    mag = __builtin_sqrtf(lineDirX * lineDirX + lineDirZ * lineDirZ);
    lineDirX /= mag;
    lineDirZ /= mag;

    dirX = rider->bike.vel.x;
    dirZ = rider->bike.vel.z;
    mag = __builtin_sqrtf(dirX * dirX + dirZ * dirZ);
    if (mag < 2.0f) {
        dirX = rider->bike.fwd.x;
        dirZ = rider->bike.fwd.z;
        mag = __builtin_sqrtf(dirX * dirX + dirZ * dirZ);
    }
    dirX /= mag;
    handle = d / 3.0f;
    ctrl1X = pos->x + dirX * handle;
    dirZ /= mag;
    ctrl1Z = pos->z + dirZ * handle;
    {
        f32 ctrl2X, ctrl2Z;
        ctrl2X = aimX - lineDirX * handle;
        ctrl2Z = aimZ - lineDirZ * handle;
        tanX = d * dirX;
        midX = (ctrl2X - ctrl1X) * 3.0f;
        tanZ = d * dirZ;
        midZ = (ctrl2Z - ctrl1Z) * 3.0f;
    }
    d = tanX * tanX + tanZ * tanZ;
    r = ((midZ - tanZ) * tanX - (midX - tanX) * tanZ) / (d * __builtin_sqrtf(d));

    if (rider->airFrames < 4) {
        /* steer towards the travel direction turned by curvature * wheelbase */
        dx = rider->frontWheel.contact.x - rider->rearWheel.contact.x;
        dz = rider->frontWheel.contact.z - rider->rearWheel.contact.z;
        dx = r * __builtin_sqrtf(dx * dx + dz * dz);
        dx += math_atan2f(tanX, tanZ);
        cosA = cosf(dx);
        sinA = sinf(dx);
        across = rider->bike.right.z * cosA - rider->bike.right.x * sinA;
        along = rider->bike.fwd.x * sinA - rider->bike.fwd.z * cosA;
        steer = 0.0f;
        if (!(r * along < 0.0f)) {
            steer = math_atan2f(-across, along);
        }
    } else {
        steer = r * 1.25f;
    }
    if (rider->airFrames < 4) {
        ai_set_steering(rider, steer);
    } else {
        k = steer * 192.0f;
        if (k >= 16) {
            k = 15;
        } else if (k < -15) {
            k = -15;
        }
        rider->stickX = k / 2;
    }
}

/* Turns a human rider's steering angle towards the stick's, at a rate and up to a limit that depend on the speed
   (ticks: 1 or 2 simulation steps); also tracks the lane the bike is in. */
void bike_update_steering(Rider *rider, s32 ticks)
{
    s32 stick, i;
    u8 lane;
    f32 speed, limit, t, maxSteer, target, rate, cur;

    stick = rider->stickX;
    if (rider->camMode == CAM_MODE_SIDE_VIEW) {
        excite3d_player_steer(rider);
    } else {
        lane = (s32)(rider->bike.pos.x / 2.178f);
        rider->gridSlot = lane;
        if (rider->crashState != CRASH_STATE_RIDING) {
            rider->laneChange = 0;
        } else {
            rider->laneChange = (s32)((rider->bike.pos.x - lane * 2.178f) * (gLaneChangeFrames / 2.178f));
            if (rider->gridSlot == 0 && rider->laneChange < 0) {
                rider->laneChange = 0;
            }
        }
    }
    if (rider->airFrames >= 8) {
        stick = 0;
    }
    speed = rider->speed;
    i = 0;
    while (sSteerSpeedSteps[i + 1] < speed) {
        i++;
    }
    if (speed < 0.0f) {
        t = 0.0f;
    } else {
        t = (sSteerSpeedSteps[i + 1] - speed) / (sSteerSpeedSteps[i + 1] - sSteerSpeedSteps[i]);
    }
    rate = 0.012566371f; /* radians per stick unit */
    target = stick * rate;
    maxSteer = (sSteerMaxBySpeed[i] * t + sSteerMaxBySpeed[i + 1] * (1.0f - t)) * rider->turnRadius;
    target *= maxSteer;
    limit = 0.18849556f;
    limit *= maxSteer;
    rate = (sSteerRateBySpeed[i] * t + sSteerRateBySpeed[i + 1] * (1.0f - t)) * 0.327f * ticks * rate;
    if (rider->reversing != 0) {
        target *= 0.25f;
    }
    if (target == 0.0f || target * rider->steer < 0.0f) {
        rate *= 5.0f;
    }
    if (rider->hardTurn != 0) {
        target = sHardTurnSteerAngle;
        if (rider->stickX <= 0) {
            target = -target;
        }
    } else if (limit < target) {
        rate = 0.1f;
        target = limit;
    } else {
        maxSteer = -limit;
        if (target < maxSteer) {
            rate = 0.1f;
            target = maxSteer;
        }
    }
    if (gGame.mode == GAME_MODE_HILL_CLIMB) {
        /* the side slope under the front wheel turns the bike */
        target += (rider->bike.fwd.x * rider->frontWheel.normal.z - rider->bike.fwd.z * rider->frontWheel.normal.x) * 16.0f * rate;
    }
    cur = rider->steer;
    if (target < cur) {
        rider->steer = cur - rate;
        if (rider->steer < target) {
            rider->steer = target;
        }
    } else {
        rider->steer = cur + rate;
        if (target < rider->steer) {
            rider->steer = target;
        }
    }
}

/* Empty. */
void func_8005C8F4(void)
{
}

/* Empty; bikes_free calls it. */
void ai_free(void)
{
}
