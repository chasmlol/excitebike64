#include "game.h"

/* Starts a crash with crash animation crashAnim (ANIM_CRASH_*): the bike becomes a free body, the rider is thrown off
   as the second body (from the pose's root bone, with the bike's velocity and an upward kick). */
void bike_crash_begin(Rider *rider, s32 crashAnim)
{
    s16 angX, angY, angZ;
    Vec3f bikeOffset;
    Vec3f spinVel;
    Vec3f bodyOffset;
    s32 riderIdx;
    s32 i;
    f32 kick;
    Vec3f *launchVel;

    if (rider->crashState != CRASH_STATE_RIDING) {
        return;
    }
    if (rider->isCpu == 0 && sCrashAnnounceFrame != gRaceFrame) { /* not announced already (phys_body_step) */
        announce_crash(rider - gRiders);
    }
    rider->engine.rpm = 0.0f;
    rider->engine.gear = 0;
    if (rider == &gRiders[1]) {
        gEvent.flags &= ~EVENT_TAKEDOWN_REQUIRED;
    }
    rider->trickDoneTimer = 0;
    rider->fistPump = 0;
    rider->savedCamMode = (u16)rider->camMode;
    if (rider->isCpu == 0) {
        flash_start(60);
        tunes_queue_crowd(0);
    }
    if (gGame.mode == GAME_MODE_EXCITE_3D) {
        rider->prevSegment = path_find_segment(&rider->bike.pos, rider->segment, 10, 1);
    }
    riderIdx = rider - gRiders;
    rider->wheelieTimer = 0;
    rider->airFrames = 0;
    rider->bike.type = BODY_CRASHED_BIKE;
    rider->crashPhase = CRASH_PHASE_START;
    rider->crashDir = (crashAnim - ANIM_CRASH_BACK) / 3;
    bike_skel_update(rider->bikeSkel, riderIdx);
    rider_update_pose(rider->riderSkel, riderIdx);
    mtx3i_to_angles(rider->riderSkel->bones[0].worldRot.m, (u16 *)&angX, (u16 *)&angY, (u16 *)&angZ);
    quat_from_euler(rider->body.quat, (f32)angX * 3.1415927f * 3.0517578e-05f,
                    (f32)angY * 3.1415927f * 3.0517578e-05f,
                    (f32)angZ * 3.1415927f * 3.0517578e-05f);
    rider->crashState = CRASH_STATE_CRASHED;
    rider->crashTimer = 0;
    rider->prevAnim = (u8)rider->anim;
    rider->prevAnimFrame = rider->animFrame;
    rider->crashSpeedCap = rider->speed + 5.0f;
    if (rider->crashSpeedCap > 32.0f) {
        rider->crashSpeedCap = 32.0f;
    }
    rider->speed = 0.0f;
    for (i = 0; i < BIKE_CONTACT_COUNT; i++) {
        vec3_mul_mtx3f(sBikeCrashContactPoints[i], (f32 *)&rider->bike.right, &bikeOffset.x);
        rider->bike.contacts[i].x = bikeOffset.x + rider->bike.pos.x;
        rider->bike.contacts[i].y = bikeOffset.y + rider->bike.pos.y;
        rider->bike.contacts[i].z = bikeOffset.z + rider->bike.pos.z;
    }
    /* a sideways crash rolls the bike (speed was just cleared: the roll is always the slow one) */
    if (crashAnim == ANIM_CRASH_RIGHT) {
        kick = 2.0f;
        goto roll;
    }
    if (crashAnim == ANIM_CRASH_LEFT) {
        kick = -2.0f;
    roll:
        if (rider->speed < 5.0f) {
            kick *= 0.25f;
        }
        rider->bike.angVel.x += kick * rider->bike.fwd.x;
        rider->bike.angVel.y += kick * rider->bike.fwd.y;
        rider->bike.angVel.z += kick * rider->bike.fwd.z;
    }
    rider->body.pos.x = (f32)(rider->riderSkel->bones[0].worldPos.x + rider->drawPos.x) / 1000.0f;
    rider->body.pos.y = (f32)(rider->riderSkel->bones[0].worldPos.y + rider->drawPos.y) / 1000.0f;
    rider->body.pos.z = (f32)(rider->riderSkel->bones[0].worldPos.z + rider->drawPos.z) / 1000.0f;
    /* the thrown rider leaves along the bike's up axis at a fifth of its speed, 1 to 5 m/s */
    kick = __builtin_sqrtf(rider->bike.vel.x * rider->bike.vel.x + rider->bike.vel.y * rider->bike.vel.y + rider->bike.vel.z * rider->bike.vel.z) / 5.0f;
    if (kick < 1.0f) {
        kick = 1.0f;
    }
    if (kick > 5.0f) {
        kick = 5.0f;
    }
    launchVel = &rider->bike.vel;
    if (rider->pendingCrash < 0) { /* knocked down: the velocity at the hit */
        launchVel = &rider->crashVel;
    }
    vec3_cross(&rider->bike.angVel, &rider->bike.up, &spinVel);
    rider->body.vel.x = launchVel->x + spinVel.x + rider->bike.up.x * kick;
    rider->body.vel.y = launchVel->y + spinVel.y + rider->bike.up.y * kick;
    rider->body.vel.z = launchVel->z + spinVel.z + rider->bike.up.z * kick;
    rider->body.angVel = rider->bike.angVel;
    if (crashAnim == ANIM_CRASH_BACK) { /* and spins about the bike's right axis */
        rider->body.angVel.x -= rider->bike.right.x + rider->bike.right.x;
        rider->body.angVel.y -= rider->bike.right.y + rider->bike.right.y;
        rider->body.angVel.z -= rider->bike.right.z + rider->bike.right.z;
    }
    rider->animFrame = 0;
    rider->body.flags |= BODY_MOVING;
    rider->bike.flags |= BODY_MOVING;
    for (i = 0; i < RIDER_CONTACT_COUNT; i++) {
        vec3_mul_mtx3f(sRiderCrashContactPoints[rider->crashDir][i], (f32 *)&rider->bike.right, &bodyOffset.x);
        rider->body.contacts[i].x = bodyOffset.x + rider->body.pos.x;
        rider->body.contacts[i].y = bodyOffset.y + rider->body.pos.y;
        rider->body.contacts[i].z = bodyOffset.z + rider->body.pos.z;
    }
    rider->bike.restThreshold = 1.0f;
    rider->body.restThreshold = 1.0f;
    stunt_start(rider, crashAnim);
    collide_tree_clear_cache(rider->body.tree);
}

/* Shrinks a suspension length by spring rate x dt / damping each tick (Excite 3D: the tuned values). */
#define SUSP_RELAX(len)                                                                                \
    {                                                                                                  \
        f32 r, k, v, w;                                                                                \
        k = (gGame.mode == GAME_MODE_EXCITE_3D) ? gBikeTuneSpringRate * 20000.0f : 15000.0f;           \
        v = (len);                                                                                     \
        w = v;                                                                                         \
        if (gGame.mode == GAME_MODE_EXCITE_3D)                                                         \
            r = v - v * (k / ((gBikeTuneDamping * 2000.0f) / gPhysDeltaTime));                        \
        else                                                                                           \
            r = w - w * (k / (2200.0f / gPhysDeltaTime));                                              \
        (len) = r;                                                                                     \
    }

/* Caps the speed of the rider's bike or body to crashSpeedCap (through the function's bodySpeed). */
#define CAP_SPEED(rider, body)                                                                         \
    {                                                                                                  \
        bodySpeed = __builtin_sqrtf(rider->body.vel.x * rider->body.vel.x +                            \
                                    rider->body.vel.y * rider->body.vel.y +                            \
                                    rider->body.vel.z * rider->body.vel.z);                            \
        if (rider->crashSpeedCap < bodySpeed) {                                                        \
            rider->body.vel.x *= rider->crashSpeedCap / bodySpeed;                                     \
            rider->body.vel.y *= rider->crashSpeedCap / bodySpeed;                                     \
            rider->body.vel.z *= rider->crashSpeedCap / bodySpeed;                                     \
        }                                                                                              \
    }

/* Seconds until the body falls to the ground (0 below it); 29.4 is twice the gravity. */
static inline f32 airtime(RigidBody *body, f32 gravity)
{
    f32 height, time, root, negApex, velY;

    height = body->pos.y - (f32)height_get_ground(body->pos.x * 1000.0f, body->pos.y * 1000.0f, body->pos.z * 1000.0f) / 1000.0f;
    if (height < 0.0f) {
        return 0.0f;
    }
    velY = body->vel.y;
    root = __builtin_sqrtf(velY * velY + height * 29.400002f) / gravity;
    negApex = -velY / gravity; /* minus the time to the top of the arc */
    time = -negApex + root;
    if (time > 0.0f) {
        return time;
    }
    return root + negApex;
}

/* Starts the animation step places after the current one (the next part of a crash). */
static inline void crash_next_anim(Rider *rider, s32 step)
{
    s32 anim = rider->anim + step;
    rider->prevAnim = rider->anim;
    rider->prevAnimFrame = rider->animFrame;
    stunt_start(rider, anim);
}

/* A tick of a crash: the bike and the thrown rider fall and tumble, the crash animations follow the fall
   (CRASH_PHASE_*), and the rider respawns after 2 seconds or more. */
void bike_crash_update(Rider *rider)
{
    s32 volume, id, step, respawnTime;
    s32 unused[8];
    s32 animFlags[2];
    f32 gravity, dt, bodySpeed;

    stunt_animate(rider, 0);
    rider->crashTimer++;
    SUSP_RELAX(rider->frontWheel.suspLength);
    SUSP_RELAX(rider->rearWheel.suspLength);
    rider->steer *= 0.9f;
    if (rider->isCpu == 0 && (gGame.numPlayers == 1 || gGame.mode == GAME_MODE_TIME_TRIAL)) {
        gCrashSlowMotion = 1;
    }
    height_get_ground(rider->bike.pos.x * 1000.0f, rider->bike.pos.y * 1000.0f, rider->bike.pos.z * 1000.0f);
    if ((gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & SURFACE_FALL) && rider->isCpu == 0) {
        cam_start_fall_view(rider);
    }
    if (rider->bike.flags & BODY_MOVING) {
        rider->bike.acc.z = 0.0f;
        rider->bike.acc.x = 0.0f;
        rider->bike.acc.y = -14.700001f;
        rider->impactSpeed = rider->slideSpeed = rider->bike.acc.z;
        phys_body_step(&rider->bike, sBikeCrashContactPoints, BIKE_CONTACT_COUNT, 1);
        CAP_SPEED(rider, bike);
        /* impactSpeed and slideSpeed: the hardest ground contact of this step */
        if (rider->impactSpeed > 0.5f) {
            volume = rider->impactSpeed * 128.0f;
            volume += 0x30;
            if (volume >= 0x100) {
                volume = 0xFF;
            }
            if (volume > 0x80 || gRaceFrame - sImpactSfxFrame >= 31) {
                sImpactSfxFrame = gRaceFrame;
                if (gRaceFrame & 1) {
                    snd_play_rider_sfx(4, rider - gRiders, volume);
                    snd_play_rider_sfx(4, rider - gRiders, volume);
                } else {
                    snd_play_rider_sfx(8, rider - gRiders, volume);
                    snd_play_rider_sfx(8, rider - gRiders, volume);
                }
            }
        }
        rider->bike.vel.y -= gPhysDeltaTime * 14.700001f;
        rider->bike.angVel.x -= rider->bike.angVel.x * gPhysDeltaTime * 0.5f;
        rider->bike.angVel.y -= rider->bike.angVel.y * gPhysDeltaTime * 0.5f;
        rider->bike.angVel.z -= rider->bike.angVel.z * gPhysDeltaTime * 0.5f;
    } else {
        rider->crashTimer += 20;
    }
    if (rider->body.flags & BODY_MOVING) {
        rider->body.acc.x = 0.0f;
        rider->body.acc.z = 0.0f;
        rider->body.acc.y = -14.700001f;
        phys_body_step(&rider->body, sRiderCrashContactPoints[rider->crashDir], RIDER_CONTACT_COUNT, 1);
        CAP_SPEED(rider, body);
        dt = gPhysDeltaTime;
        gravity = 14.700001f;
        rider->body.vel.y -= dt * gravity;
        rider->body.angVel.x -= rider->body.angVel.x * gPhysDeltaTime;
        rider->body.angVel.y -= rider->body.angVel.y * gPhysDeltaTime;
        rider->body.angVel.z -= rider->body.angVel.z * gPhysDeltaTime;
        switch (rider->crashPhase) {
        case CRASH_PHASE_START:
            /* at the end of the first animation: the falling one, or the landing one if near the ground */
            if (rider_anim_get_length(rider->anim, animFlags) - (rider->animFrame >> 8) < 2) {
                step = (airtime(&rider->body, gravity) < 0.5f) ? 2 : 1;
                crash_next_anim(rider, step);
                rider->crashPhase += step;
            }
            break;
        case CRASH_PHASE_FALL:
            if (airtime(&rider->body, gravity) < 0.5f) {
                crash_next_anim(rider, 1);
                rider->crashPhase = CRASH_PHASE_LAND;
            }
            break;
        case CRASH_PHASE_LAND:
            break;
        }
    }
    respawnTime = 120;
    if (gCrashSlowMotion != 0 && rider->isCpu == 0) {
        respawnTime = 240;
    }
    if (gGame.mode == GAME_MODE_HILL_CLIMB) {
        respawnTime = 300;
    }
    if (respawnTime < (s16)rider->crashTimer) {
        bike_respawn(rider);
    }
}

/* The ground height (m) under a point (m). */
#define GROUND_HEIGHT(v) ((f32)height_get_ground((s32)((v)->x * 1000.0f), (s32)((v)->y * 1000.0f), (s32)((v)->z * 1000.0f)) / 1000.0f)

/* A contact point of a body (sBikeCrashContactPoints, sRiderCrashContactPoints, sBikeContactPoints, ...). */
struct Pt_4 {
    Vec3f pos;          /* in the body's frame */
    f32 friction;       /* phys_contact_impulse's limit on sliding friction; above 0.15 a hard point (sparks, sound) */
};

/* The start of gCurTrack: f2C is tested against 0 here, 1 in path.c (respawn where there is no ground), 2 in
   camera.c. */
typedef struct { u8 pad[0x2C]; s32 f2C; } Game_16;

extern Game_16 *gCurTrack_25;

/* Moves a rigid body by a tick in substeps: pushes its contact points out of the ground (freeBody: not a riding
   bike) and the walls, lets a free body come to rest, then collides it with the other bodies. Returns 0. */
s32 phys_body_step(RigidBody *body, Pt_4 *pts, s32 count, u8 freeBody)
{
    Vec3f newPos;
    Vec3f hitVel;
    Vec3f offsets[40];
    Vec3f points[40];
    Quat_2 spin;
    Quat_2 newQuat;
    f32 depth[42];
    Vec3f vec;
    Vec3f savedPos;
    Quat_2 savedQuat;
    Vec3f arm;
    Vec3f contactVel;
    f32 hitT;
    f32 maxX, minX, maxZ, minZ;
    f32 normalSpeed, hitSpeedSq, tmp, friction, dt, angSpeed, halfAngle, approach, sinScale;
    s32 i, hitIdx;
    u8 wallHit;
    Rider *rider;
    s32 idx;

    /* gather the walls within a metre of the path of this tick */
    if (body->vel.x > 0.0f) {
        maxX = body->vel.x * gPhysDeltaTime + 1.0f;
        minX = -1.0f;
    } else {
        maxX = 1.0f;
        minX = body->vel.x * gPhysDeltaTime - 1.0f;
    }
    if (body->vel.z > 0.0f) {
        maxZ = body->vel.z * gPhysDeltaTime + 1.0f;
        minZ = -1.0f;
    } else {
        maxZ = 1.0f;
        minZ = body->vel.z * gPhysDeltaTime - 1.0f;
    }
    if ((gDrawFlags & 0x200) && body == &gRiders[0].bike) { /* the wall debug display: a fixed box */
        maxX = 4.0f;
        minX = -4.0f;
        maxZ = maxX;
        minZ = minX;
    }
    height_gather_walls((s32)((body->pos.x + minX) * 1000.0f), (s32)((body->pos.z + minZ) * 1000.0f),
                        (s32)((body->pos.x + maxX) * 1000.0f), (s32)((body->pos.z + maxZ) * 1000.0f));
    if (body->hitTimer != 0) {
        body->hitTimer -= ((u8 *)&gPhysTicks)[3]; /* the low byte */
        if (body->hitTimer < 0) {
            body->hitTimer = 0;
        }
    }
    dt = gPhysDeltaTime;
    do {
        /* the new position and orientation, from the velocities at mid-step */
        vec.x = body->vel.x + body->acc.x * dt * 0.5f;
        vec.y = body->vel.y + body->acc.y * dt * 0.5f;
        vec.z = body->vel.z + body->acc.z * dt * 0.5f;
        newPos.x = vec.x * dt;
        newPos.y = vec.y * dt;
        newPos.z = vec.z * dt;
        newPos.x = body->pos.x + newPos.x;
        newPos.y = body->pos.y + newPos.y;
        newPos.z = body->pos.z + newPos.z;
        vec.x = body->angVel.x + body->angAcc.x * dt * 0.5f;
        vec.y = body->angVel.y + body->angAcc.y * dt * 0.5f;
        vec.z = body->angVel.z + body->angAcc.z * dt * 0.5f;
        hitT = 1.0f;
        angSpeed = __builtin_sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
        halfAngle = angSpeed * dt * 0.5f;
        if (angSpeed != 0.0f) {
            sinScale = sinf(halfAngle) / angSpeed;
        } else {
            sinScale = 0.0f;
        }
        spin.q[0] = cosf(halfAngle);
        spin.q[1] = vec.x * sinScale;
        spin.q[2] = vec.y * sinScale;
        spin.q[3] = vec.z * sinScale;
        quat_mul(newQuat.q, spin.q, body->quat);
        quat_to_mtx3f(&body->right.x, newQuat.q);
        for (i = 0; i < count; i++) {
            vec3_mul_mtx3f(&pts[i].pos.x, &body->right.x, &offsets[i].x);
            points[i].x = offsets[i].x + newPos.x;
            points[i].y = offsets[i].y + newPos.y;
            points[i].z = offsets[i].z + newPos.z;
        }
        if (freeBody != 0) {
            if (body->type == BODY_BALL) {
                /* a sphere of 1 m radius bouncing on the ground */
                tmp = newPos.y - (f32)height_get_ground((s32)(newPos.x * 1000.0f), (s32)(newPos.y * 1000.0f), (s32)(newPos.z * 1000.0f)) / 1000.0f - 1.0f;
                if (tmp < 0.0f) {
                    vec.x = 0.0f;
                    vec.y = -1.0f;
                    vec.z = 0.0f;
                    newPos.y += 0.005f - tmp;
                    vec3_cross(&body->angVel, &vec, &hitVel);
                    hitVel.x = body->vel.x + hitVel.x;
                    hitVel.y = body->vel.y + hitVel.y;
                    hitVel.z = body->vel.z + hitVel.z;
                    height_get_ground_normal(&gContactNormal_2);
                    phys_contact_impulse(body, &gContactNormal_2, &vec, 0.3f, &hitVel, 0.7f);
                }
            } else {
                /* the point deepest in the ground takes the impulse; a slow body resting on its points stops */
                tmp = 0.0f;
                for (i = 0; i < count; i++) {
                    depth[i] = GROUND_HEIGHT(points + i) - points[i].y;
                    if (tmp < depth[i]) {
                        tmp = depth[i];
                        hitIdx = i;
                        height_get_ground_normal(&gContactNormal_2);
                    }
                }
                if (tmp > 0.0f) {
                    if (phys_check_resting(&body->vel.x, &body->angVel.x, depth, offsets, count, body->restThreshold) < 0.0f) {
                        body->flags &= ~BODY_MOVING;
                        return 0;
                    }
                    newPos.y += depth[hitIdx] + 0.01f;
                    hitVel.x = (points[hitIdx].x - body->contacts[hitIdx].x) / dt;
                    hitVel.y = (points[hitIdx].y - body->contacts[hitIdx].y) / dt;
                    hitVel.z = (points[hitIdx].z - body->contacts[hitIdx].z) / dt;
                    friction = pts[hitIdx].friction;
                    if (body->owner != NULL && body->owner->crashState != CRASH_STATE_RIDING) {
                        friction /= body->owner->weight;
                    }
                    phys_contact_impulse(body, &gContactNormal_2, &offsets[hitIdx], friction, &hitVel, 0.1f);
                    hitSpeedSq = hitVel.x * hitVel.x + hitVel.y * hitVel.y + hitVel.z * hitVel.z;
                    rider = body->owner;
                    /* a human's crashed bike hitting the ground hard (rider is read before its NULL test) */
                    if (body->type == BODY_CRASHED_BIKE && rider->isCpu == 0 && hitSpeedSq > 4.0f && pts[hitIdx].friction > 0.15f) {
                        if (rider != NULL) {
                            if (sImpactSlideSpeed > rider->slideSpeed) {
                                rider->slideSpeed = sImpactSlideSpeed;
                            }
                            if (sImpactNormalSpeed > rider->impactSpeed) {
                                rider->impactSpeed = sImpactNormalSpeed;
                            }
                            if (gRiderAlpha[rider - gRiders] == 0xFF) {
                                particles_start_dust_cloud(&points[hitIdx], rider - gRiders);
                            }
                        }
                        if (rider->isCpu == 0 && gRaceFrame - rider->landingFrame >= 16) {
                            rider->landingImpact = hitSpeedSq;
                            rider->landingFrame = gRaceFrame;
                            rider->impactFlags = 0xFF;
                        }
                    }
                    for (i = 0; i < count; i++) {
                        points[i].x = newPos.x + offsets[i].x;
                        points[i].y = newPos.y + offsets[i].y;
                        points[i].z = newPos.z + offsets[i].z;
                    }
                }
            }
        }
        /* the walls: the point whose move hits one first */
        wallHit = 0;
        for (i = 0; i < count; i++) {
            if (height_raycast_walls(&body->contacts[i], &points[i], &gContactNormal_2, &hitT, 0)) {
                hitVel.x = (points[i].x - body->contacts[i].x) / dt;
                hitVel.y = (points[i].y - body->contacts[i].y) / dt;
                hitIdx = i;
                hitVel.z = (points[i].z - body->contacts[i].z) / dt;
                if (gWallHitFlags & 1) {
                    wallHit = 1;
                }
            }
        }
        if (hitT < 1.0f) {
            normalSpeed = vec3_dot(&gContactNormal_2.x, &hitVel.x);
            tmp = 0.3f;
            if (body->type < BODY_CRASHED_BIKE) {
                /* a riding bike: landing on a face after a long jump (or on points past the wheels) crashes, so does
                   hitting a wall at more than 15 m/s */
                rider = body->owner;
                if (gContactNormal_2.y > 0.75f && (hitIdx > 5 || rider->airFrames > 60)) {
                    rider->landingImpact = 0x200;
                    rider->landingFrame = gRaceFrame;
                    rider->impactFlags = 0xFF;
                    rider->pendingCrash = (pts[hitIdx].pos.z > 0.0f) ? -ANIM_CRASH_BACK : -ANIM_CRASH_FRONT;
                    rider->crashVel = rider->bike.vel;
                } else if (normalSpeed < -15.0f) {
                    if (gCurTrack_25->f2C == 0 && gGame.mode != GAME_MODE_STUNT_COURSE && gGame.mode != GAME_MODE_SOCCER) {
                        sCrashAnnounceFrame = gRaceFrame;
                        if ((f32)height_get_ground((s32)(rider->bike.pos.x * 1000.0f), (s32)(rider->bike.pos.y * 1000.0f), (s32)(rider->bike.pos.z * 1000.0f)) / 1000.0f - rider->bike.pos.y > 2.0f) {
                            announce_crash(rider - gRiders);
                        } else {
                            announce_crash_landing(rider - gRiders);
                        }
                    }
                    rider->landingImpact = 0x200;
                    rider->landingFrame = gRaceFrame;
                    rider->impactFlags = 0xFF;
                    rider->pendingCrash = -ANIM_CRASH_FRONT;
                    rider->crashVel = rider->bike.vel;
                }
                if (rider->speed < 10.0f) {
                    tmp = 0.2f;
                } else {
                    tmp = rider->wallFriction;
                }
            }
            phys_contact_impulse(body, &gContactNormal_2, &offsets[hitIdx], tmp, &hitVel, 0.05f);
            if (body->type < BODY_CRASHED_BIKE) {
                /* a wall hit in a wheelie or a trick on the ground (the test leaves out trick 0) */
                Rider *rider = body->owner;
                if ((rider->wheelieTimer != 0 || ((u8)rider->anim > 0 && (u8)rider->anim < ANIM_TRICK_COUNT)) && rider->airFrames <= 0) {
                    rider->pendingCrash = -ANIM_CRASH_BACK;
                    rider->crashVel = rider->bike.vel;
                }
            }
            {
                f32 hitSpeed = __builtin_sqrtf(hitVel.x * hitVel.x + hitVel.y * hitVel.y + hitVel.z * hitVel.z);
                if (hitSpeed > 2.0f && wallHit) {
                    if (pts[hitIdx].friction > 0.15f) {
                        if (body->type < BODY_CRASHED_BIKE) {
                            body->owner->sparkDir = pts[hitIdx].pos;
                            body->owner->sparkTimer = 10;
                            snd_trigger_impact(body->owner - gRiders, (s32)(hitSpeed * 16.0f));
                        } else if (body->type == BODY_CRASHED_BIKE) {
                            Rider *rider = body->owner;
                            if (sImpactSlideSpeed > rider->slideSpeed) {
                                rider->slideSpeed = sImpactSlideSpeed;
                            }
                            if (sImpactNormalSpeed > rider->impactSpeed) {
                                rider->impactSpeed = sImpactNormalSpeed;
                            }
                        }
                    }
                }
            }
            /* try again with a shorter step */
            dt *= 0.8f;
            body->hitTimer = 15;
        } else {
            f32 horizLen;
            savedPos = body->pos;
            savedQuat = *(Quat_2 *)body->quat;
            body->pos = newPos;
            *(Quat_2 *)body->quat = newQuat;
            /* a riding bike's wheel against a steep face (normal y < 0.5): a crash after a long jump or when the bike
               is far from square to it, else the bike is pushed off it horizontally and the step undone */
            if (body->type < BODY_CRASHED_BIKE && freeBody == 0) {
                rider = body->owner;
                bike_update_wheel_contacts(rider);
                if (rider->frontWheel.normal.y < 0.5f && rider->frontWheel.suspLength <= rider->frontWheel.groundDist + 0.01f) {
                    if (rider->airFrames > 60 || vec3_dot(&rider->frontWheel.normal.x, &rider->bike.up.x) < 0.3f) {
                        rider->pendingCrash = -ANIM_CRASH_FRONT;
                        rider->crashVel = rider->bike.vel;
                    } else {
                        arm.x = rider->frontWheel.contact.x - newPos.x;
                        arm.y = rider->frontWheel.contact.y - newPos.y;
                        arm.z = rider->frontWheel.contact.z - newPos.z;
                        vec3_cross(&arm, &body->angVel, &contactVel);
                        contactVel.x = body->vel.x + contactVel.x;
                        contactVel.y = body->vel.y + contactVel.y;
                        contactVel.z = body->vel.z + contactVel.z;
                        horizLen = rider->frontWheel.normal.x * rider->frontWheel.normal.x + rider->frontWheel.normal.z * rider->frontWheel.normal.z;
                        horizLen = __builtin_sqrtf(horizLen);
                        vec.x = rider->frontWheel.normal.x / horizLen;
                        vec.y = 0.0f;
                        vec.z = rider->frontWheel.normal.z / horizLen;
                        approach = vec3_dot(&vec.x, &contactVel.x);
                        if (approach > 0.0f) {
                            contactVel.x -= approach * vec.x;
                            contactVel.z -= approach * vec.z;
                        }
                        dt *= 0.5f;
                        phys_contact_impulse(body, &vec, &arm, 0.0f, &contactVel, 0.1f);
                        body->pos = savedPos;
                        *(Quat_2 *)body->quat = savedQuat;
                        quat_to_mtx3f(&body->right.x, body->quat);
                        bike_update_wheel_contacts(rider);
                        continue;
                    }
                }
                if (rider->rearWheel.normal.y < 0.5f && rider->rearWheel.suspLength <= rider->rearWheel.groundDist + 0.01f) {
                    if (rider->airFrames > 60 || vec3_dot(&rider->rearWheel.normal.x, &rider->bike.up.x) < 0.3f) {
                        rider->pendingCrash = -ANIM_CRASH_FRONT;
                        rider->crashVel = rider->bike.vel;
                    } else {
                        arm.x = rider->rearWheel.contact.x - newPos.x;
                        arm.y = rider->rearWheel.contact.y - newPos.y;
                        arm.z = rider->rearWheel.contact.z - newPos.z;
                        vec3_cross(&arm, &body->angVel, &contactVel);
                        contactVel.x = body->vel.x + contactVel.x;
                        contactVel.y = body->vel.y + contactVel.y;
                        contactVel.z = body->vel.z + contactVel.z;
                        /* no square root here, unlike the front wheel's */
                        horizLen = rider->rearWheel.normal.x * rider->rearWheel.normal.x + rider->rearWheel.normal.z * rider->rearWheel.normal.z;
                        vec.x = rider->rearWheel.normal.x / horizLen;
                        vec.y = 0.0f;
                        vec.z = rider->rearWheel.normal.z / horizLen;
                        approach = vec3_dot(&vec.x, &contactVel.x);
                        if (approach > 0.0f) {
                            contactVel.x -= approach * vec.x;
                            contactVel.z -= approach * vec.z;
                        }
                        phys_contact_impulse(body, &vec, &arm, 0.0f, &contactVel, 0.1f);
                        body->pos = savedPos;
                        *(Quat_2 *)body->quat = savedQuat;
                        vec.x *= 0.125f;
                        vec.y *= 0.125f;
                        vec.z *= 0.125f;
                        body->vel.x = vec.x + body->vel.x;
                        body->vel.y = vec.y + body->vel.y;
                        body->vel.z = vec.z + body->vel.z;
                        dt *= 0.5f;
                        quat_to_mtx3f(&body->right.x, body->quat);
                        bike_update_wheel_contacts(rider);
                        dt -= 0.25f; /* ends the loop */
                        continue;
                    }
                }
            }
            dt = 0.0f;
        }
    } while (dt > 0.0002f);

    for (i = 0; i < count; i++) {
        body->contacts[i] = points[i];
    }
    /* the bodies of a crash collide with each other, the others with the riding bikes (and the ball) */
    if (body->type != BODY_BALL) {
        rider = body->owner;
        if (rider == NULL || rider->noCollide != 0) {
            return 0;
        }
        if (rider->crashState != CRASH_STATE_RIDING) {
            if (body->tree != 0) {
                collide_bodies(body, &rider->bike);
                collide_bodies(body, &rider->body);
            }
            return 0;
        }
    }
    if (body->tree != 0) {
        for (i = 0; i < RIDER_COUNT; i++) {
            if (gRiders[i].inactive == 0 && gRiders[i].noCollide == 0 && gRiders[i].crashState == CRASH_STATE_RIDING) {
                collide_bodies(body, &gRiders[i].bike);
            }
        }
        if (gGame.mode == GAME_MODE_SOCCER) {
            collide_bodies(body, &gSoccerBall);
        }
    }
    return 0;
}

/* Applies a contact impulse to body at arm against a surface of the given normal, with bounce (restitution) and
   friction up to maxFriction times the normal impulse; notes the speeds in sImpactNormalSpeed, sImpactSlideSpeed. */
void phys_contact_impulse(RigidBody *body, Vec3f *normal, Vec3f *arm, f32 maxFriction, Vec3f *vel, f32 restitution)
{
    Vec3f slide;
    Vec3f angDelta;
    volatile s32 pad[4];
    Vec3f impulse;
    Vec3f perp;
    f32 tmp, closing, effMass, slideSpeed, friction, normalImp, frictionImp;

    /* the effective mass at the contact point */
    tmp = vec3_dot((f32 *)normal, (f32 *)arm);
    perp.x = normal->x * tmp;
    perp.y = normal->y * tmp;
    perp.z = normal->z * tmp;
    perp.x = arm->x - perp.x;
    perp.y = arm->y - perp.y;
    perp.z = arm->z - perp.z;
    tmp = perp.x * perp.x + perp.y * perp.y + perp.z * perp.z;
    effMass = body->mass / (tmp * (body->mass / body->inertia) + 1.0f);
    closing = -vec3_dot((f32 *)vel, (f32 *)normal);
    sImpactNormalSpeed = closing;
    if (64.0f < closing) {
        vec3_cross(&body->angVel, arm, vel);
        vel->x += body->vel.x;
        vel->y += body->vel.y;
        vel->z += body->vel.z;
        closing = -vec3_dot((f32 *)vel, (f32 *)normal);
    }
    closing += 0.05f;
    normalImp = (restitution + 1.0f) * closing * effMass;
    if (normalImp < 0.0f) {
        normalImp = 0.0f;
    }
    /* the sliding velocity along the surface, and the friction that would stop it (capped) */
    slide.x = vel->x + normal->x * closing;
    slide.y = vel->y + normal->y * closing;
    slide.z = vel->z + normal->z * closing;
    slideSpeed = __builtin_sqrtf(slide.x * slide.x + slide.y * slide.y + slide.z * slide.z);
    sImpactSlideSpeed = slideSpeed;
    if (slideSpeed != 0.0f) {
        slide.x /= slideSpeed;
        slide.y /= slideSpeed;
        slide.z /= slideSpeed;
    }
    friction = 0.0f;
    if (closing != 0.0f) {
        friction = slideSpeed / closing;
    }
    if (maxFriction < friction) {
        friction = maxFriction;
    }
    frictionImp = -friction;
    frictionImp = frictionImp * normalImp;
    impulse.x = normal->x * normalImp + slide.x * frictionImp;
    impulse.y = normal->y * normalImp + slide.y * frictionImp;
    impulse.z = normal->z * normalImp + slide.z * frictionImp;
    body->vel.x += impulse.x / body->mass;
    body->vel.y += impulse.y / body->mass;
    body->vel.z += impulse.z / body->mass;
    vec3_cross(arm, &impulse, &angDelta);
    body->angVel.x += angDelta.x / body->inertia;
    body->angVel.y += angDelta.y / body->inertia;
    body->angVel.z += angDelta.z / body->inertia;
    body->acc.z = 0.0f;
    body->acc.y = 0.0f;
    body->acc.x = 0.0f;
    body->angAcc.z = 0.0f;
    body->angAcc.y = 0.0f;
    body->angAcc.x = 0.0f;
}

/* Whether a slow body comes to rest on its contact points (3 or more within 2 cm of the ground around its centre):
   then stops it and returns -1; otherwise 0 (slow) or 0.2. */
f32 phys_check_resting(f32 *vel, f32 *angVel, f32 *depth, Vec3f *offsets, s32 count, f32 threshold)
{
    s32 touching[10];
    s32 i, j, numTouching, numLeft;
    f32 result;
    Vec3f *cur;
    Vec3f *next;

    result = 0.2f;
    if (vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2] + (angVel[0] * angVel[0] + angVel[1] * angVel[1] + angVel[2] * angVel[2]) < threshold) {
        result = 0.0f;
        numTouching = 0;
        for (i = 0; i < count; i++) {
            if (depth[i] > -0.02f) {
                touching[numTouching++] = i;
            }
        }
        if (numTouching >= 3) {
            numLeft = 0;
            for (i = 0; i < numTouching; i++) {
                j = i + 1;
                if (j == numTouching) {
                    j = 0;
                }
                /* &offsets[touching[i]], &offsets[touching[j]] */
                cur = (Vec3f *)(touching[i] * 12 + (s32)offsets);
                next = (Vec3f *)(touching[j] * 12 + (s32)offsets);
                if ((cur->x - next->x) * cur->z - (cur->z - next->z) * cur->x > 0.0f) {
                    numLeft++;
                }
            }
            if (numLeft == numTouching || numLeft == 0) {
                angVel[0] = angVel[1] = angVel[2] = 0.0f;
                vel[0] = vel[1] = vel[2] = 0.0f;
                return -1.0f;
            }
        }
    }
    return result;
}

/* Puts a crashed rider back on his bike: at the kick-off place in soccer, where he is in the desert and the stunt
   course, else on the racing line before the crash, beside any other rider. Blinks while he can't be hit. */
void bike_respawn(Rider *rider)
{
    s32 seg;
    s32 node, i, lapDelta, riderIdx;
    PathNode *n;
    Game *game;
    Rider *other;
    f32 zero, clearSq, step, offset, distSq, dx, dz, groundY;

    if (rider->isCpu == 2) { /* an extra rider (race_spawn_extra_rider) leaves instead */
        rider->inactive = 1;
        return;
    }
    rider->crashPhase = CRASH_PHASE_NONE;
    rider->jumpState = 0;
    rider->airFrames = 0;
    rider->roughLanding = 0;
    rider->wheelieTimer = 0;
    rider->pendingCrash = 0;
    rider->crashTimer = 0;
    rider->crashState = CRASH_STATE_RIDING;
    rider->offTrackTimer = 0;
    rider->engine.heat = 0.0f;
    rider->engine.overheated = 0;
    rider->prevAnim = ANIM_NONE;
    rider->queuedTrick = ANIM_NONE;
    rider->trickDoneTimer = 0;
    rider->unk_7DC = 0;
    if (rider->finishState == 0) {
        rider->blinkCount = 40;
    }
    rider->blinkTimer = 16;
    rider->noCollide = 1;
    rider->timeScale = 1.0f;
    rider->bike.type = rider->isCpu != 0; /* BODY_BIKE or BODY_CPU_BIKE */
    if (rider->isCpu == 0) {
        gCrashSlowMotion = 0;
        if (gCamTargetOverride == -1) {
            gCamLastModes[rider - gRiders] = -1;
        } else if (gCamTargetOverride == rider - gRiders) {
            gCamLastModes[0] = -1;
        }
    }
    seg = -1;
    game = &gGame;
    switch (game->mode) {
    case GAME_MODE_SOCCER:
        if ((gEvent.flags & 0xF) || (gEvent.state != 7 && gEvent.state != 11 && gEvent.state != 9)) {
            /* the kick-off places, two a side */
            rider->blinkCount = 0;
            rider->blinkTimer = 0;
            if (game->numPlayers == 4) {
                switch (rider - gRiders) {
                case 0:
                    rider->bike.pos.x = -1.0f;
                    rider->bike.pos.z = -10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 0.0f);
                    break;
                case 1:
                    rider->bike.pos.x = 1.0f;
                    rider->bike.pos.z = -10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 0.0f);
                    break;
                case 2:
                    rider->bike.pos.x = 1.0f;
                    rider->bike.pos.z = 10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 3.1415927f);
                    break;
                case 3:
                    rider->bike.pos.x = -1.0f;
                    rider->bike.pos.z = 10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 3.1415927f);
                    break;
                }
            } else {
                switch (rider - gRiders) {
                case 0:
                    rider->bike.pos.x = 0.015f;
                    rider->bike.pos.z = -10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 0.00958738f);
                    break;
                case 1:
                    rider->bike.pos.x = 0.01f;
                    rider->bike.pos.z = 10.0f;
                    quat_from_euler(rider->bike.quat, 0.0f, 0.0f, 3.15118f);
                    break;
                }
            }
        } else {
            height_push_off_wall(&rider->bike.pos.x);
            quat_from_euler(rider->bike.quat, 0.0f, 0.0f, math_atan2f(-(rider->bike.pos.z - gSoccerBall.pos.z), rider->bike.pos.x - gSoccerBall.pos.x));
        }
        rider->noCollide = 0;
        break;
    case 4:
    case GAME_MODE_DESERT:
        height_push_out_of_desert_objs(&rider->bike.pos.x);
        quat_from_euler(rider->bike.quat, 0.0f, 0.0f, (f32)rider->heading * 3.1415927f * 3.0517578e-05f);
        break;
    case GAME_MODE_STUNT_COURSE:
        height_push_off_wall(&rider->bike.pos.x);
        quat_from_euler(rider->bike.quat, 0.0f, 0.0f, (f32)rider->heading * 3.1415927f * 3.0517578e-05f);
        break;
    default:
        /* the segment before the crash, moved to one where respawning is allowed */
        seg = rider->prevSegment;
        if (gGame.mode == GAME_MODE_EXCITE_3D && seg > 0) {
            seg--;
        }
        if (gGame.mode == GAME_MODE_TEST_OPEN) {
            seg = 1;
        }
        while (gPathNodes[seg].respawnDir > 0) {
            seg = gPathNodes[seg].next;
            if (seg > gPathNodeCount) {
                seg = 0;
            }
        }
        while (gPathNodes[seg].respawnDir < 0) {
            seg = gPathNodes[seg].prev;
            if (seg < 0) {
                seg = gPathNodeCount - 1;
            }
        }
        /* a lap counted on the way from there to the crash (across the finish line) is taken back */
        lapDelta = 0;
        node = rider->segment;
        for (i = 0; i < 8; i++) {
            if (node == rider->prevSegment) {
                rider->lapAdjust += lapDelta;
                break;
            }
            if (node == gPathFinishNode) {
                lapDelta = -1;
            }
            node = gPathNodes[node].prev;
        }
        rider->segment = seg;
        if (gGame.mode == GAME_MODE_EXCITE_3D) {
            rider->laneChange = 0;
            rider->bike.pos.x = (f32)rider->gridSlot * 2.178f;
        } else {
            /* across the track, alternating sides from the line, the first spot clear of the other riders; if
               there is none, the line itself */
            clearSq = (gPathNodes[seg].widthNeg * gPathNodes[seg].widthNeg + gPathNodes[seg].widthPos * gPathNodes[seg].widthPos) / 36.0f;
            step = __builtin_sqrtf(clearSq) * 0.5f;
            offset = 0.0f;
            while (offset <= gPathNodes[seg].widthPos || -offset <= gPathNodes[seg].widthNeg) {
                if (!(offset > 0.0f ? gPathNodes[seg].widthPos < offset : gPathNodes[seg].widthNeg < -offset)) {
                    rider->bike.pos.x = gPathNodes[seg].pos.x + gPathNodes[seg].lateralX * offset;
                    distSq = 10000.0f;
                    rider->bike.pos.z = gPathNodes[seg].pos.z + gPathNodes[seg].lateralZ * offset;
                    if (gGhostRace != 0) {
                        break;
                    }
                    for (other = gRiders; other < &gRiders[RIDER_COUNT]; other++) {
                        if (other == rider || other->inactive != 0) {
                            continue;
                        }
                        dx = rider->bike.pos.x - other->bike.pos.x;
                        dz = rider->bike.pos.z - other->bike.pos.z;
                        distSq = dx * dx + dz * dz;
                        if (distSq < clearSq) {
                            break;
                        }
                    }
                    if (clearSq <= distSq) {
                        break;
                    }
                }
                offset += (offset >= 0.0f ? step : 0.0f);
                offset = -offset;
            }
            if (distSq < clearSq) {
                rider->bike.pos = gPathNodes[seg].pos;
            }
        }
        quat_from_euler(rider->bike.quat, 0.0f, 0.0f, math_atan2f(gPathNodes[seg].lateralX, gPathNodes[seg].lateralZ));
        break;
    }
    bike_place_on_ground(rider);
    quat_to_mtx3f(&rider->bike.right.x, rider->bike.quat);
    rider->stuntScore = 0;
    rider->camMode = rider->savedCamMode;
    zero = 0.0f;
    rider->frontWheel.suspLength = zero;
    rider->rearWheel.suspLength = zero;
    rider->bike.vel.z = zero;
    rider->bike.vel.y = zero;
    rider->bike.vel.x = zero;
    rider->bike.angVel.z = zero;
    rider->bike.angVel.y = zero;
    rider->bike.angVel.x = zero;
    rider->localAngVel.x = zero;
    rider->localAngVel.y = zero;
    rider->localAngVel.z = zero;
    rider->bike.restThreshold = 1.0f;
    if (rider == gRiders) {
        rider->flags &= ~RIDER_FINISHED;
    }
    rider->anim = ANIM_NONE;
    rider->bike.mass = 250.0f;
    rider->bike.inertia = 250.0f;
    *(Quat_2 *)rider->drawQuat = *(Quat_2 *)rider->bike.quat;
    if (seg > 0) {
        rider->bike.pos.y = gPathNodes[seg].pos.y + 1.0f;
    }
    groundY = (f32)height_get_ground(rider->bike.pos.x * 1000.0f, rider->bike.pos.y * 1000.0f, rider->bike.pos.z * 1000.0f) / 1000.0f + zero + 0.5f;
    /* minus the suspension's sag under the bike's weight (250 kg x 14.7 over two springs) */
    rider->bike.pos.y = groundY - (gGame.mode == GAME_MODE_EXCITE_3D ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    bike_update_wheel_contacts(rider);
    bike_update_wheel_contacts(rider);
    bike_update_suspension(rider);
    rider->heading = (s16)rider->drawRot[2];
    for (i = 0; i < BIKE_CONTACT_COUNT; i++) {
        rider->bike.contacts[i] = rider->bike.pos;
    }
    riderIdx = rider - gRiders;
    decal_end_track(riderIdx);
    cam_set_frozen(riderIdx, 0);
    collide_tree_clear_cache(rider->bike.tree);
    bike_update_render_pose(rider);
}

/* A contact point, as Pt_4. */
typedef struct {
    f32 x, y, z, friction;
} S_2;

extern S_2 sBikeCrashContactPoints_2[]; /* sBikeCrashContactPoints */

/* Sets the friction of the crashed bike's contact points (0.1 for points 3 and 4). */
void bike_set_contact_friction(f32 friction)
{
    s32 i;

    for (i = 0; i < BIKE_CONTACT_COUNT; i++) {
        sBikeCrashContactPoints_2[i].friction = friction;
    }
    sBikeCrashContactPoints_2[3].friction = 0.1f;
    sBikeCrashContactPoints_2[4].friction = 0.1f;
}

/* Queues a crash (pendingCrash: > 0 on landing, < 0 at once) with the bike's velocity now. */
void bike_queue_crash(Rider *rider, u8 crash)
{
    rider->pendingCrash = crash;
    rider->crashVel = rider->bike.vel;
}

/* Seconds until a rigid body (pos, then vel at [3]) falls to the ground, as airtime() with gravity 14.7. */
f32 body_time_to_ground(f32 *body)
{
    s32 ground;
    f32 height, root, velY, time;

    ground = height_get_ground((s32)(body[0] * 1000.0f), (s32)(body[1] * 1000.0f), (s32)(body[2] * 1000.0f));
    height = body[1] - (f32)ground / 1000.0f;
    if (height < 0.0f) {
        return 0.0f;
    }
    velY = body[4];
    root = __builtin_sqrtf(velY * velY + height * 29.400002f) / 14.700001f;
    time = -(-velY / 14.700001f) + root;
    if (!(time > 0.0f)) {
        time = root + -velY / 14.700001f;
    }
    return time;
}

/* Starts animation anim, remembering the current one for the blend. */
void rider_change_anim(Rider *rider, s32 anim)
{
    rider->prevAnim = (u8)rider->anim;
    rider->prevAnimFrame = rider->animFrame;
    stunt_start(rider, anim);
}
