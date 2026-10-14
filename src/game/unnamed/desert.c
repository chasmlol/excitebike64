#include "game.h"

/* Sums the distances along the checkpoints and aims the arrow at the first. */
void desert_init(void) {
    s32 i;
    f32 sum;
    f32 dx, dz;

    sum = 0.0f;
    sDesertCheckpointDist[0] = sum;
    for (i = 1; i < DESERT_NUM_CHECKPOINTS; i++) {
        dx = sDesertCheckpoints[i].x - sDesertCheckpoints[i - 1].x;
        dz = sDesertCheckpoints[i].z - sDesertCheckpoints[i - 1].z;
        sum += __builtin_sqrtf(dx * dx + dz * dz);
        sDesertCheckpointDist[i] = sum;
    }
    des_set_waypoint(0);
    event_reset_places();
}

/* Gives fractal.c the checkpoints, in mm, to place the campfires at. */
void desert_export_checkpoints(void) {
    s32 i;
    gDesertFireCount = DESERT_NUM_CHECKPOINTS;
    for (i = 0; i < DESERT_NUM_CHECKPOINTS; i++) {
        gDesertFires[i].x = sDesertCheckpoints[i].x * 1000.0f;
        gDesertFires[i].y = sDesertCheckpoints[i].y * 1000.0f;
        gDesertFires[i].z = sDesertCheckpoints[i].z * 1000.0f;
    }
}

/* A frame of the desert race: the arrow to the player's next checkpoint, CPU riders far from the player moved
   directly, checkpoints and laps, then the standings. */
void desert_update_race(void)
{
    Rider *rider;
    Rider *ahead;
    Rider *behind;
    f32 dx, dz, dist, angle, leaderDist, checkDist, ox, oz;
    s32 i;
    u8 swapped;
    s32 pad[4];

    dist = 0.0f;
    for (rider = gRiders; rider < &gRiders[gEvent.numRiders]; rider++) {
        rider->timeScale = 1.0f;
        if (rider->inactive != 0) {
            continue;
        }
        if (rider == gRiders) {
            dx = sDesertCheckpoints[rider->checkpoint].x - rider->bike.pos.x;
            angle = math_atan2f(sDesertCheckpoints[rider->checkpoint].z - rider->bike.pos.z, -dx);
            /* radians to 0x10000 per turn */
            des_set_arrow_angle((s32)(angle * 32768.0f / 3.1415927f) - gCamYaw[0] & 0xFFFF);
        }
        {
            ox = rider->bike.pos.x - gRiders[0].bike.pos.x;
            oz = rider->bike.pos.z - gRiders[0].bike.pos.z;
            leaderDist = __builtin_sqrtf(ox * ox + oz * oz);
        }
        /* more than 120 m from the player, a rider is moved straight at its checkpoint at 10 m/s until it is back
           within 100 m */
        if (rider->frozen != 0) {
            if (leaderDist < 100.0f) {
                rider->frozen = 0;
                bike_respawn(rider);
                rider->blinkCount = 1;
                rider->blinkTimer = 1;
            } else {
                dx = sDesertCheckpoints[rider->checkpoint].x - rider->bike.pos.x;
                dz = sDesertCheckpoints[rider->checkpoint].z - rider->bike.pos.z;
                dist = __builtin_sqrtf(dx * dx + dz * dz);
                angle = math_atan2f(dz, dx);
                rider->heading = angle * 32768.0f / 3.1415927f;
                rider->bike.pos.x += dx / dist * 0.16666667f;
                rider->bike.pos.z += dz / dist * 0.16666667f;
            }
        } else if (120.0f < leaderDist) {
            rider->frozen = 1;
        }
        dx = rider->bike.pos.x - sDesertCheckpoints[rider->checkpoint].x;
        dz = rider->bike.pos.z - sDesertCheckpoints[rider->checkpoint].z;
        checkDist = __builtin_sqrtf(dx * dx + dz * dz);
        rider->checkpointDist = checkDist;
        if (checkDist < 1.0f) {
            if (++rider->checkpoint == DESERT_NUM_CHECKPOINTS) {
                rider->checkpoint = 0;
                if (rider->finishState == 0) {
                    event_rider_finished(rider - gRiders);
                    if (rider == gRiders) {
                        gEvent.allFinished = 1;
                    }
                }
            } else {
                ox = rider->bike.pos.x - sDesertCheckpoints[rider->checkpoint].x;
                oz = rider->bike.pos.z - sDesertCheckpoints[rider->checkpoint].z;
                rider->checkpointDist = ox * ox + oz * oz; /* squared, unlike the others */
            }
            if (rider == gRiders) {
                des_set_waypoint(rider->checkpoint);
            }
        }
        rider->raceDist = sDesertCheckpointDist[rider->checkpoint] - rider->checkpointDist;
    }
    if (gRiders[0].isCpu == 0) {
        /* bubble sort: further checkpoint first, then nearer to it */
        do {
            swapped = 0;
            for (i = 0; i < gEvent.numRiders - 1; i++) {
                ahead = gRiderStandings[i];
                if (ahead->race.finishPlace < 0) {
                    behind = gRiderStandings[i + 1];
                    if (ahead->checkpoint < behind->checkpoint || (ahead->checkpoint == behind->checkpoint && ahead->checkpointDist > behind->checkpointDist)) {
                        swapped = 1;
                        gRiderStandings[i] = behind;
                        gRiderStandings[i + 1] = ahead;
                    }
                }
            }
        } while (swapped);
        for (i = 0; i < gEvent.numRiders; i++) {
            gRiderStandings[i]->race.place = i;
        }
        race_announce_position_changes();
    }
}

/* A CPU rider's steering and throttle: aim at the next checkpoint, at a speed that depends on its distance and on
   how far off the heading it lies. */
void desert_ai_steer(Rider *rider)
{
    f32 toTarget[3];
    f32 local[3];
    f32 angle, targetSpeed, dist, speedDiff, sum;
    f32 *target;
    s8 *stick;
    u32 input;

    toTarget[0] = sDesertCheckpoints_2[rider->checkpoint * 3] - rider->bike.pos.x;
    target = &sDesertCheckpoints_2[rider->checkpoint * 3];
    toTarget[1] = target[1] - rider->bike.pos.y;
    target = &sDesertCheckpoints_2[rider->checkpoint * 3];
    toTarget[2] = target[2] - rider->bike.pos.z;
    mtx3f_mul_vec3(toTarget, &rider->bike.right.x, local);
    angle = math_atan2f(local[2], local[0]);
    sum = local[0] * local[0] + local[1] * local[1] + local[2] * local[2];
    dist = __builtin_sqrtf(sum);

    stick = &rider->stickX;
    *stick = (s8)(s32)(-angle * 14.0f);
    if (*stick > 14) *stick = 14;
    if (*stick < -14) *stick = -14;

    if (!(0.0f < angle)) angle = -angle;

    targetSpeed = 5.0f;
    if (!(dist < 5.0f)) {
        if (dist < 20.0f) {
            if (angle < 0.1f) targetSpeed = 20.0f;
        } else {
            targetSpeed = 10.0f;
            if (angle < 1.0f) targetSpeed = 40.0f;
        }
    }
    if (rider->finishState == 1) goto zero;
    if (rider->finishState == 2) goto five;
    goto after;
zero:
    targetSpeed = 0.0f;
    goto after;
five:
    targetSpeed = 5.0f;
after:

    speedDiff = rider->speed - targetSpeed;
    if (0.5f < speedDiff) {
        input = 6;
        rider->frontWheel.brakeRamp = input;
        rider->rearWheel.brakeRamp = input;
        input = INPUT_B;
        goto store;
    }
    if (!(speedDiff < -0.5f) || rider->airFrames >= 3 || rider->laneBlocked != 0) goto done;
    input = INPUT_A;
    if (!(speedDiff < -5.0f)) goto store;
    input = INPUT_Z; /* turbo while the engine is not too hot */
    if (rider->engine.heat < 9500.0f) goto store;
    input = INPUT_A;
store:
    rider->input = input;
done:
    bike_update_steering(rider, 1);
}
