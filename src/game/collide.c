#include "game.h"

#define COLLIDE_POOL_SIZE 16000 /* bytes, for the CollideNodes of every sphere tree */

/* Builds the node of desc and its siblings from the pool: a leaf from its sphere (0.5 m lower), an inner node as the
   sphere spanning its first two children. Writes each node's radius back into its description. */
CollideNode *collide_build_sphere_tree(CollideSphereDesc *desc)
{
    CollideNode *node = sCollideNodeNext;
    CollideNode *first;
    CollideNode *second;
    f32 farA[3];
    f32 farB[3];
    f32 dir[3];
    f32 len;

    sCollideNodeNext = node + 1;
    sCollideNodeCount = sCollideNodeCount + 1;
    if (desc->child == 0) {
        node->pos.x = desc->x;
        node->pos.y = desc->y - 0.5f;
        node->pos.z = desc->z;
        node->radius = desc->radius;
    } else {
        first = collide_build_sphere_tree(desc + desc->child);
        node->child = first;
        second = first->next;
        dir[0] = second->pos.x - first->pos.x;
        dir[1] = second->pos.y - first->pos.y;
        dir[2] = second->pos.z - first->pos.z;
        len = __builtin_sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
        if (len != 0.0f) {
            dir[0] = dir[0] / len;
            dir[1] = dir[1] / len;
            dir[2] = dir[2] / len;
        }
        /* the far sides of the two children, along the line between their centres */
        farA[0] = dir[0] * -first->radius;
        farA[1] = dir[1] * -first->radius;
        farA[2] = dir[2] * -first->radius;
        farA[0] = farA[0] + first->pos.x;
        farA[1] = farA[1] + first->pos.y;
        farA[2] = farA[2] + first->pos.z;
        farB[0] = dir[0] * second->radius;
        farB[1] = dir[1] * second->radius;
        farB[2] = dir[2] * second->radius;
        farB[0] = farB[0] + second->pos.x;
        farB[1] = farB[1] + second->pos.y;
        farB[2] = farB[2] + second->pos.z;
        node->pos.x = (farA[0] + farB[0]) * 0.5f;
        node->pos.y = (farA[1] + farB[1]) * 0.5f;
        node->pos.z = (farA[2] + farB[2]) * 0.5f;
        farA[0] = farA[0] - farB[0];
        farA[1] = farA[1] - farB[1];
        farA[2] = farA[2] - farB[2];
        node->radius = __builtin_sqrtf(farA[0] * farA[0] + farA[1] * farA[1] + farA[2] * farA[2]) * 0.5f;
    }
    if (desc->next != 0) {
        node->next = collide_build_sphere_tree(desc + desc->next);
    }
    desc->radius = node->radius;
    return node;
}

/* Whether any leaf of tree a overlaps any leaf of tree b; rotA/rotB are the bodies' matrix rows, posA/posB their
   positions. World positions are cached in the nodes; the larger sphere is split first. */
s32 collide_spheres_overlap(CollideNode *a, CollideNode *b, Vec3f *rotA, Vec3f *rotB, Vec3f *posA, Vec3f *posB)
{
    f32 reach;
    f32 distSq;
    f32 t1, t2, t3;
    volatile s32 pad[4];

    if (a->cached == 0) {
        a->worldPos.x = a->pos.y * rotA[1].x + a->pos.z * rotA[2].x + posA->x;
        a->worldPos.y = a->pos.y * rotA[1].y + a->pos.z * rotA[2].y + posA->y;
        t1 = rotA[1].z;
        t2 = rotA[2].z;
        t3 = posA->z;
        a->cached = 1;
        a->worldPos.z = a->pos.y * t1 + a->pos.z * t2 + t3;
    }
    if (b->cached == 0) {
        b->worldPos.x = b->pos.y * rotB[1].x + b->pos.z * rotB[2].x + posB->x;
        b->worldPos.y = b->pos.y * rotB[1].y + b->pos.z * rotB[2].y + posB->y;
        t1 = rotB[1].z;
        t2 = rotB[2].z;
        t3 = posB->z;
        b->cached = 1;
        b->worldPos.z = b->pos.y * t1 + b->pos.z * t2 + t3;
    }
    reach = a->radius + b->radius;
    distSq = (a->worldPos.x - b->worldPos.x) * (a->worldPos.x - b->worldPos.x)
      + (a->worldPos.y - b->worldPos.y) * (a->worldPos.y - b->worldPos.y)
      + (a->worldPos.z - b->worldPos.z) * (a->worldPos.z - b->worldPos.z);
    if (distSq < reach * reach) {
        if (a->child == 0) {
            if (b->child == 0) {
                return 1;
            }
        } else if (b->child == 0 || b->radius < a->radius) {
            a = a->child;
            do {
                if (collide_spheres_overlap(a, b, rotA, rotB, posA, posB) & 0xff) {
                    return 1;
                }
                a = a->next;
            } while (a != 0);
            return 0;
        }
        for (b = b->child; b != 0; b = b->next) {
            if (collide_spheres_overlap(a, b, rotA, rotB, posA, posB) & 0xff) {
                return 1;
            }
        }
    }
    return 0;
}

/* Finds the overlapping pair of leaves of trees a and b that overlap the most: sCollideDepth, sCollideHitA,
   sCollideHitB. The leaves return 0, so the whole overlap is searched. */
s32 collide_find_deepest_overlap(CollideNode *a, CollideNode *b, Vec3f *rotA, Vec3f *rotB, Vec3f *posA, Vec3f *posB)
{
    f32 t1;
    f32 reach;
    f32 t2;
    f32 t3;

    f32 distSq;
    s32 hit;
    Vec3f tmp;

    if (a->cached == 0) {
        a->worldPos.x = a->pos.y * rotA[1].x + a->pos.z * rotA[2].x + posA->x;
        a->worldPos.y = a->pos.y * rotA[1].y + a->pos.z * rotA[2].y + posA->y;
        t1 = rotA[1].z;
        t2 = rotA[2].z;
        t3 = posA->z;
        a->cached = 1;
        a->worldPos.z = a->pos.y * t1 + a->pos.z * t2 + t3;
    }
    if (b->cached == 0) {
        b->worldPos.x = b->pos.y * rotB[1].x + b->pos.z * rotB[2].x + posB->x;
        b->worldPos.y = b->pos.y * rotB[1].y + b->pos.z * rotB[2].y + posB->y;
        t1 = rotB[1].z;
        t2 = rotB[2].z;
        t3 = posB->z;
        b->cached = 1;
        b->worldPos.z = b->pos.y * t1 + b->pos.z * t2 + t3;
    }
    reach = a->radius + b->radius;
    distSq = (a->worldPos.x - b->worldPos.x) * (a->worldPos.x - b->worldPos.x) +
        (a->worldPos.y - b->worldPos.y) * (a->worldPos.y - b->worldPos.y) +
        (a->worldPos.z - b->worldPos.z) * (a->worldPos.z - b->worldPos.z);
    if (distSq < reach * reach) {
        if (a->child == 0) {
            if (b->child == 0) {
                distSq = (a->radius + b->radius) - __builtin_sqrtf(distSq); /* the depth */
                if (sCollideDepth < distSq) {
                    sCollideDepth = distSq;
                    sCollideHitA = a;
                    sCollideHitB = b;
                }
                return 0;
            }
        } else if (b->child == 0 || b->radius < a->radius) {
            a = a->child;
            do {
                hit = collide_find_deepest_overlap(a, b, rotA, rotB, posA, posB);
                if ((hit & 0xff) != 0) {
                    return 1;
                }
                a = a->next;
            } while (a != 0);
            return 0;
        }
        for (b = b->child; b != 0; b = b->next) {
            hit = collide_find_deepest_overlap(a, b, rotA, rotB, posA, posB);
            if ((hit & 0xff) != 0) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

/* Two riding bikes touched: decides whether one rider knocks the other down (landing on him, a side hit in Excite
   3D, a front wheel into a rear wheel). Returns 1 when riderB falls, 2 when riderA does, 0 when nobody does. */
s32 collide_resolve_rider_hit(Rider *riderA, Rider *riderB)
{
    f32 rel[3];
    f32 localA[3];
    f32 localB[3];
    s32 cpuA, cpuB;
    RigidBody *bodyB;
    f32 *posA;
    s32 unused;

    /* invincible: a CPU rider who touches a human always falls */
    if (gCheatFlags & CHEAT_INVINCIBLE) {
        if (riderA->isCpu != 0) {
            if (riderB->isCpu != 0) {
                goto noCheat;
            }
        } else if (riderB->isCpu != 0) {
            bike_crash_begin(riderB, ANIM_CRASH_FRONT);
            return 1;
        }
        if (riderA->isCpu == 0) {
            goto noCheat;
        }
        bike_crash_begin(riderA, ANIM_CRASH_FRONT);
        return 1;
    }
noCheat:

    /* landing on a rider after a second in the air knocks him down */
    if (riderA->airFrames >= 0 && riderB->airFrames >= 0) {
        return 0;
    }
    if (riderA->airFrames < FRAMES(60)) {
        goto tryLandingOnA;
    }
    if (riderB->airFrames >= 0) {
        goto tryLandingOnA;
    }

    cpuA = riderA->isCpu;
    cpuB = riderB->isCpu;
    if ((cpuA & cpuB) == 0 || gEvent.time < FRAMES(300)) {
        announce_takeout(riderA - gRiders, riderB - gRiders);
    }
    riderB->pendingCrash = -ANIM_CRASH_BACK;
    goto landedOn;

tryLandingOnA:
    if (riderB->airFrames < FRAMES(60)) {
        goto sideOrRearHit;
    }
    if (riderA->airFrames >= 0) {
        goto sideOrRearHit;
    }
    cpuA = riderA->isCpu;
    cpuB = riderB->isCpu;
    if ((cpuA & cpuB) == 0 || gEvent.time < FRAMES(300)) {
        announce_takeout(riderB - gRiders, riderA - gRiders);
    }
    riderA->pendingCrash = -ANIM_CRASH_FRONT;

landedOn: /* reached also when riderA is the one who falls */
    riderA->fistPump = 1;
    riderA->engine.heat = 0.0f;

sideOrRearHit:
    /* Excite 3D: the rider further along z falls sideways */
    if (gGame.mode == GAME_MODE_EXCITE_3D) {
        if (riderA->bike.pos.z < riderB->bike.pos.z) {
            if (riderA->bike.pos.x < riderB->bike.pos.x) {
                riderB->pendingCrash = -ANIM_CRASH_LEFT;
            } else {
                riderB->pendingCrash = -ANIM_CRASH_RIGHT;
            }
            riderB->crashVel = riderB->bike.vel;
            return 1;
        }
        if (riderB->bike.pos.x < riderA->bike.pos.x) {
            riderA->pendingCrash = -ANIM_CRASH_LEFT;
        } else {
            riderA->pendingCrash = -ANIM_CRASH_RIGHT;
        }
        riderA->crashVel = riderA->bike.vel;
        return 1;
    }

    if (gRaceFrame - riderA->contactFrame[riderB - gRiders] < FRAMES(60)) {
        return 0;
    }

    /* the contact point in each bike's frame ([2] forward); near the middle of either bike, nothing happens */
    posA = &riderA->bike.pos.x;
    rel[0] = gCollidePoint.x - posA[0];
    rel[1] = gCollidePoint.y - posA[1];
    rel[2] = gCollidePoint.z - posA[2];
    bodyB = &riderB->bike;
    mtx3f_mul_vec3(rel, &riderA->bike.right.x, localA);
    if (-0.2f < localA[2] && localA[2] < 0.2f) {
        return 0;
    }

    rel[0] = gCollidePoint.x - bodyB->pos.x;
    rel[1] = gCollidePoint.y - bodyB->pos.y;
    rel[2] = gCollidePoint.z - bodyB->pos.z;
    mtx3f_mul_vec3(rel, &bodyB->right.x, localB);
    if (-0.2f < localB[2] && localB[2] < 0.2f) {
        return 0;
    }

    /* riderB's front ran into riderA's rear: riderB falls, unless a human braking or off the throttle */
    if (localB[2] > 0.0f && localA[2] < 0.0f) {
        if (riderB->isCpu == 0) {
            if (riderB->input & INPUT_B) {
                return 0;
            }
            if ((riderB->input & (INPUT_Z | INPUT_A)) == 0) {
                return 0;
            }
        }
        riderA->fistPump = 1;
        riderA->engine.heat = 0.0f;
        if (riderA->isCpu == 0 || riderB->isCpu == 0) {
            announce_takeout(riderA - gRiders, riderB - gRiders);
        }
        if (localA[0] - localB[0] > 0.0f) {
            riderB->pendingCrash = -ANIM_CRASH_LEFT;
        } else {
            riderB->pendingCrash = -ANIM_CRASH_RIGHT;
        }
        {
            /* crashVel = bike.vel, copied the way the original does: through pointers to an 8-byte type (u64, not
               f64: the game is compiled with 32-bit doubles) */
            u64 *dst = (u64 *)&riderB->crashVel;
            u64 *src = (u64 *)&riderB->bike.vel;
            __builtin_memcpy(dst, src, 12);
        }
        return 1;
    }
    if (localA[2] > 0.0f && localB[2] < 0.0f) {
        if (riderA->isCpu == 0) {
            if (riderA->input & INPUT_B) {
                return 0;
            }
            if ((riderA->input & (INPUT_Z | INPUT_A)) == 0) {
                return 0;
            }
        }
        riderB->fistPump = 1;
        riderB->engine.heat = 0.0f;
        if (riderA->isCpu == 0 || riderB->isCpu == 0) {
            announce_takeout(riderB - gRiders, riderA - gRiders);
        }
        if (localB[0] - localA[0] > 0.0f) {
            riderA->pendingCrash = -ANIM_CRASH_LEFT;
        } else {
            riderA->pendingCrash = -ANIM_CRASH_RIGHT;
        }
        riderA->crashVel = riderA->bike.vel;
        return 2;
    }
    return 0;
}

/* Normalizes a Vec3f in place (a zero vector stays as it is). */
#define NORM(v) { f32 len = __builtin_sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); if (len != 0.0f) { v.x /= len; v.y /= len; v.z /= len; } }

/* Pushes two overlapping bodies apart with an impulse at the deepest pair of spheres; between two bikes, also the
   knock-downs, sparks and sounds. Returns 0 when they already separate, else 1 (or the knock-down's result). */
s32 collide_apply_impulse(RigidBody *bodyA, RigidBody *bodyB)
{
    Vec3f normal;
    Vec3f armA;
    Vec3f armB;
    Vec3f velA;
    Vec3f velB;
    Vec3f relVel;
    Vec3f perp;
    Vec3f torque;
    Vec3f delta;
    f32 perpSq, len, halfDepth, closing, along, effMassA, effMassB, impulse, lim;
    Rider *riderA, *riderB, *ownerA, *ownerB;
    s32 result, volume, ia, idxB;

    normal.x = sCollideHitA->worldPos.x - sCollideHitB->worldPos.x;
    normal.y = sCollideHitA->worldPos.y - sCollideHitB->worldPos.y;
    normal.z = sCollideHitA->worldPos.z - sCollideHitB->worldPos.z;
    NORM(normal);
    if (normal.y > 0.25f) {
        normal.y = 0.25f;
        NORM(normal);
    } else if (normal.y < -0.25f) {
        normal.y = -0.25f;
        NORM(normal);
    }
    halfDepth = sCollideDepth * 0.5f;
    gCollidePoint.x = normal.x * (sCollideHitB->radius - halfDepth);
    gCollidePoint.y = normal.y * (sCollideHitB->radius - halfDepth);
    gCollidePoint.z = normal.z * (sCollideHitB->radius - halfDepth);
    gCollidePoint.x = sCollideHitB->worldPos.x + gCollidePoint.x;
    gCollidePoint.y = sCollideHitB->worldPos.y + gCollidePoint.y;
    gCollidePoint.z = sCollideHitB->worldPos.z + gCollidePoint.z;
    gCollidePoint.y = (bodyA->pos.y + bodyB->pos.y) * 0.5f;
    armA.x = gCollidePoint.x - bodyA->pos.x;
    armA.y = gCollidePoint.y - bodyA->pos.y;
    armA.z = gCollidePoint.z - bodyA->pos.z;
    vec3_cross(&armA, &bodyA->angVel, &velA);
    velA.x = bodyA->vel.x + velA.x;
    velA.y = bodyA->vel.y + velA.y;
    velA.z = bodyA->vel.z + velA.z;
    armB.x = gCollidePoint.x - bodyB->pos.x;
    armB.y = gCollidePoint.y - bodyB->pos.y;
    armB.z = gCollidePoint.z - bodyB->pos.z;
    vec3_cross(&armB, &bodyB->angVel, &velB);
    velB.x = bodyB->vel.x + velB.x;
    velB.y = bodyB->vel.y + velB.y;
    velB.z = bodyB->vel.z + velB.z;
    relVel.x = velA.x - velB.x;
    relVel.y = velA.y - velB.y;
    relVel.z = velA.z - velB.z;
    closing = vec3_dot(&relVel, &normal);
    if (closing > 0.0f) {
        return 0;
    }
    if (bodyA->type < BODY_CRASHED_BIKE && bodyB->type < BODY_CRASHED_BIKE) {
        riderA = bodyA->owner;
        riderB = bodyB->owner;
        bodyA->mass *= riderA->weight;
        bodyA->inertia *= riderA->weight;
        bodyB->mass *= riderB->weight;
        bodyB->inertia *= riderB->weight;
        if (riderA->crashState == CRASH_STATE_RIDING && riderB->crashState == CRASH_STATE_RIDING) {
            result = collide_resolve_rider_hit(riderA, riderB);
            riderB->contactFrame[riderA - gRiders] = gRaceFrame;
            riderA->contactFrame[riderB - gRiders] = gRaceFrame;
            if (result != 0) {
                return result;
            }
        }
        if (bodyA->type == BODY_BIKE) {
            particles_start_sparks(riderA);
        }
        if (bodyB->type == BODY_BIKE) {
            particles_start_sparks(riderB);
        }
        volume = (s32)(-closing) * 128.0f / 7.0f + 128.0f;
        if (volume > 255) {
            volume = 255;
        }
        if (volume < 0) {
            volume = 0;
        }
        if (volume > 32) {
            snd_play_rider_sfx(4, riderA - gRiders, volume);
        }
    }
    /* each body's effective mass at the contact point */
    along = vec3_dot(&normal, &armA);
    perp.x = normal.x * along;
    perp.y = normal.y * along;
    perp.z = normal.z * along;
    perp.x = armA.x - perp.x;
    perp.y = armA.y - perp.y;
    perp.z = armA.z - perp.z;
    perpSq = perp.x * perp.x + perp.y * perp.y + perp.z * perp.z;
    effMassA = bodyA->mass / (perpSq * bodyA->mass / bodyA->inertia + 1.0f);
    along = vec3_dot(&normal, &armB);
    perp.x = normal.x * along;
    perp.y = normal.y * along;
    perp.z = normal.z * along;
    perp.x = armB.x - perp.x;
    perp.y = armB.y - perp.y;
    perp.z = armB.z - perp.z;
    perpSq = perp.x * perp.x + perp.y * perp.y + perp.z * perp.z;
    effMassB = bodyB->mass / (perpSq * bodyB->mass / bodyB->inertia + 1.0f);
    if (bodyA->type == BODY_BALL || bodyB->type == BODY_BALL) {
        impulse = (closing - 2.0f) * -1.5f * (effMassA * effMassB) / (effMassA + effMassB);
    } else {
        impulse = (closing - 2.0f) * -1.1f * (effMassA * effMassB) / (effMassA + effMassB);
    }
    delta.x = normal.x * (impulse / bodyA->mass);
    delta.y = normal.y * (impulse / bodyA->mass);
    delta.z = normal.z * (impulse / bodyA->mass);
    bodyA->vel.x = bodyA->vel.x + delta.x;
    bodyA->vel.y = bodyA->vel.y + delta.y;
    bodyA->vel.z = bodyA->vel.z + delta.z;
    delta.x = normal.x * (impulse / bodyB->mass);
    delta.y = normal.y * (impulse / bodyB->mass);
    delta.z = normal.z * (impulse / bodyB->mass);
    bodyB->vel.x = bodyB->vel.x - delta.x;
    bodyB->vel.y = bodyB->vel.y - delta.y;
    bodyB->vel.z = bodyB->vel.z - delta.z;
    vec3_cross(&armA, &normal, &torque);
    bodyA->angVel.x = bodyA->angVel.x + torque.x * (impulse / bodyA->inertia);
    bodyA->angVel.y = bodyA->angVel.y + torque.y * (impulse / bodyA->inertia);
    bodyA->angVel.z = bodyA->angVel.z + torque.z * (impulse / bodyA->inertia);
    vec3_cross(&armB, &normal, &torque);
    bodyB->angVel.x = bodyB->angVel.x - torque.x * (impulse / bodyB->inertia);
    bodyB->angVel.y = bodyB->angVel.y - torque.y * (impulse / bodyB->inertia);
    bodyB->angVel.z = bodyB->angVel.z - torque.z * (impulse / bodyB->inertia);
    if (bodyA->type < BODY_CRASHED_BIKE && bodyB->type < BODY_CRASHED_BIKE) {
        bodyA->mass = 250.0f;
        bodyA->inertia = 250.0f;
        bodyB->mass = 250.0f;
        bodyB->inertia = 250.0f;
        ownerA = bodyA->owner;
        ownerB = bodyB->owner;
        if (ownerA->rearWheel.airTicks <= 0) {
            ownerA->rearWheel.spin = vec3_dot(&ownerA->rearWheel.lonAxis.x, &ownerA->bike.vel.x);
        }
        if (ownerA->frontWheel.airTicks <= 0) {
            ownerA->frontWheel.spin = vec3_dot(&ownerA->frontWheel.lonAxis.x, &ownerA->bike.vel.x);
        }
        ownerA->speed = __builtin_sqrtf(ownerA->bike.vel.x * ownerA->bike.vel.x + ownerA->bike.vel.y * ownerA->bike.vel.y + ownerA->bike.vel.z * ownerA->bike.vel.z);
        if (ownerB->rearWheel.airTicks <= 0) {
            ownerB->rearWheel.spin = vec3_dot(&ownerB->rearWheel.lonAxis.x, &ownerB->bike.vel.x);
        }
        if (ownerB->frontWheel.airTicks <= 0) {
            ownerB->frontWheel.spin = vec3_dot(&ownerB->frontWheel.lonAxis.x, &ownerB->bike.vel.x);
        }
        ownerB->speed = __builtin_sqrtf(ownerB->bike.vel.x * ownerB->bike.vel.x + ownerB->bike.vel.y * ownerB->bike.vel.y + ownerB->bike.vel.z * ownerB->bike.vel.z);
        /* a rider hit in a wheelie or a trick on the ground falls (rider A's test also takes ANIM_AIR_TURN) */
        if ((ownerA->wheelieTimer != 0 || (u8)ownerA->anim < 17) && ownerA->airFrames <= 0) {
            ownerA->pendingCrash = -ANIM_CRASH_BACK;
            ownerA->crashVel = ownerA->bike.vel;
            return 1;
        }
        if ((ownerB->wheelieTimer != 0 || (u8)ownerB->anim < ANIM_TRICK_COUNT) && ownerB->airFrames <= 0) {
            ownerB->pendingCrash = -ANIM_CRASH_BACK;
            ownerB->crashVel = ownerB->bike.vel;
            return 1;
        }
        /* first and second bump in the first 6 seconds */
        if (ownerA->race.place + ownerB->race.place == 1 && gEvent.time < FRAMES(360) && !(ownerA->isCpu & ownerB->isCpu)) {
            if (gRaceFrame - gAnnounceTimes[ownerA - gRiders] < FRAMES(600)) {
                announce_place(ownerB - gRiders, ownerB->race.place);
                return 1;
            }
            idxB = ownerB - gRiders;
            if (gRaceFrame - gAnnounceTimes[idxB] < FRAMES(600)) {
                announce_place(ownerA - gRiders, ownerA->race.place);
                return 1;
            }
            announce_lead_collision(ownerA - gRiders, idxB);
        }
    }
    return 1;
}

/* Collides two bodies (the bike and the thrown rider of one crash ignore each other until apart); 1 after an
   impulse. In soccer, notes which human rider touched the ball. */
s32 collide_bodies(RigidBody *bodyA, RigidBody *bodyB)
{
    CollideNode *node;
    CollideNode *tree;
    RigidBody *rider0Bike;
    s32 unusedA, unusedB;

    if (bodyB == bodyA) {
        return 0;
    }
    tree = bodyA->tree;
    sCollideDepth = 0.0f;
    node = tree;
    if (node != 0 && node->cached != 0) {
        node->cached = 0;
        node = node->child;
        while (node != 0) {
            collide_tree_clear_cache(node);
            node = node->next;
        }
    }
    if (bodyA->owner == bodyB->owner &&
        bodyA->owner->crashState != CRASH_STATE_CLEAR) {
        if (collide_spheres_overlap(bodyA->tree, bodyB->tree, &bodyA->right, &bodyB->right, &bodyA->pos, &bodyB->pos) & 0xff) {
            return 0;
        }
        bodyA->owner->crashState = CRASH_STATE_CLEAR;
    }
    collide_find_deepest_overlap(bodyA->tree, bodyB->tree, &bodyA->right, &bodyB->right, &bodyA->pos, &bodyB->pos);
    if (0.0f < sCollideDepth) {
        if (collide_apply_impulse(bodyA, bodyB) != 0) {
            bodyB->flags = bodyB->flags | BODY_MOVING;
            if ((bodyA->type == BODY_BALL && bodyB->type == BODY_BIKE) || (bodyB->type == BODY_BALL && bodyA->type == BODY_BIKE)) {
                if (bodyA->type == BODY_BALL) {
                    bodyA = bodyB;
                }
            } else {
                return 1;
            }
            rider0Bike = &gRiders[0].bike;
            if (bodyA == rider0Bike) {
                gEvent.ballToucher = 0;
            } else if (bodyA == &gRiders[1].bike) {
                gEvent.ballToucher = 1;
            } else if (bodyA == &gRiders[2].bike) {
                gEvent.ballToucher = 2;
            } else if (bodyA == &gRiders[3].bike) {
                gEvent.ballToucher = 3;
            }
            return 1;
        }
        bodyA->hitTimer = FRAMES(15);
        bodyB->hitTimer = FRAMES(15);
    }
    return 0;
}

/* Marks the world positions of a sphere tree out of date (stops at nodes already marked). */
void collide_tree_clear_cache(CollideNode *node)
{
    if (node != NULL && node->cached != 0) {
        node->cached = 0;
        node = node->child;
        while (node != NULL) {
            collide_tree_clear_cache(node);
            node = node->next;
        }
    }
}

void collide_free_node_pool(void)
{
    mem_free(sCollideNodePool);
}

/* Allocates and clears the pool the sphere trees are built in (400 nodes). */
void collide_init_node_pool(void)
{
    sCollideNodeCount = 0;
    sCollideNodePool = mem_alloc_dbg(COLLIDE_POOL_SIZE, 0, sStrCollideC, 0x103);
    sCollideNodeNext = sCollideNodePool;
    mem_fill(sCollideNodePool, 0, COLLIDE_POOL_SIZE);
}

/* Stops a body and turns it to the quaternion (0, 0, 1, 0): a half turn about the vertical axis. */
void collide_reset_body(RigidBody *body)
{
    f32 one = 1.0f;

    body->vel.x = 0.0f;
    body->vel.y = 0.0f;
    body->vel.z = 0.0f;
    body->acc.x = 0.0f;
    body->acc.y = 0.0f;
    body->acc.z = 0.0f;
    body->angVel.x = 0.0f;
    body->angVel.y = 0.0f;
    body->angVel.z = 0.0f;
    body->angAcc.x = 0.0f;
    body->angAcc.y = 0.0f;
    body->angAcc.z = 0.0f;
    body->quat[0] = 0.0f;
    body->quat[1] = 0.0f;
    body->quat[3] = 0.0f;
    body->quat[2] = one;
    quat_to_mtx3f(&body->right.x, body->quat);
}

/* Sets the spin of each wheel on the ground to the bike's speed along it, and the rider's speed. */
void collide_update_bike_speeds(Rider *rider)
{
    f32 speedSq;
    f32 speed;

    if (rider->rearWheel.airTicks < 1) {
        rider->rearWheel.spin = vec3_dot(&rider->rearWheel.lonAxis.x, &rider->bike.vel.x);
    }
    if (rider->frontWheel.airTicks < 1) {
        rider->frontWheel.spin = vec3_dot(&rider->frontWheel.lonAxis.x, &rider->bike.vel.x);
    }
    speedSq = rider->bike.vel.x * rider->bike.vel.x
      + rider->bike.vel.y * rider->bike.vel.y
      + rider->bike.vel.z * rider->bike.vel.z;
    speed = __builtin_sqrtf(speedSq);
    rider->speed = speed;
}

/* Clears the deepest overlap and marks a sphere tree's world positions out of date. */
void collide_reset_tree(CollideNode *tree)
{
    CollideNode *node;

    sCollideDepth = 0;
    node = tree;
    if (node != NULL && node->cached != 0) {
        node->cached = 0;
        for (node = node->child; node != NULL; node = node->next) {
            collide_tree_clear_cache(node);
        }
    }
}
