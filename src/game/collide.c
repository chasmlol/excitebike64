#include "game.h"

CollideNode *collide_build_sphere_tree(CollideSphereDesc *p)
{
    CollideNode *n = sCollideNodeNext;
    CollideNode *a;
    CollideNode *b;
    f32 pa[3];
    f32 pb[3];
    f32 d[3];
    f32 len;

    sCollideNodeNext = n + 1;
    sCollideNodeCount = sCollideNodeCount + 1;
    if (p->child == 0) {
        n->pos.x = p->x;
        n->pos.y = p->y - 0.5f;
        n->pos.z = p->z;
        n->radius = p->radius;
    } else {
        a = collide_build_sphere_tree(p + p->child);
        n->child = a;
        b = a->next;
        d[0] = b->pos.x - a->pos.x;
        d[1] = b->pos.y - a->pos.y;
        d[2] = b->pos.z - a->pos.z;
        len = __builtin_sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        if (len != 0.0f) {
            d[0] = d[0] / len;
            d[1] = d[1] / len;
            d[2] = d[2] / len;
        }
        pa[0] = d[0] * -a->radius;
        pa[1] = d[1] * -a->radius;
        pa[2] = d[2] * -a->radius;
        pa[0] = pa[0] + a->pos.x;
        pa[1] = pa[1] + a->pos.y;
        pa[2] = pa[2] + a->pos.z;
        pb[0] = d[0] * b->radius;
        pb[1] = d[1] * b->radius;
        pb[2] = d[2] * b->radius;
        pb[0] = pb[0] + b->pos.x;
        pb[1] = pb[1] + b->pos.y;
        pb[2] = pb[2] + b->pos.z;
        n->pos.x = (pa[0] + pb[0]) * 0.5f;
        n->pos.y = (pa[1] + pb[1]) * 0.5f;
        n->pos.z = (pa[2] + pb[2]) * 0.5f;
        pa[0] = pa[0] - pb[0];
        pa[1] = pa[1] - pb[1];
        pa[2] = pa[2] - pb[2];
        n->radius = __builtin_sqrtf(pa[0] * pa[0] + pa[1] * pa[1] + pa[2] * pa[2]) * 0.5f;
    }
    if (p->next != 0) {
        n->next = collide_build_sphere_tree(p + p->next);
    }
    p->radius = n->radius;
    return n;
}

s32 collide_spheres_overlap(CollideNode *a, CollideNode *b, Vec3f *x1, Vec3f *x2, Vec3f *o1, Vec3f *o2)
{
    f32 r;
    f32 d;
    f32 t1, t2, t3;
    volatile s32 pad[4];

    if (a->cached == 0) {
        a->worldPos.x = a->pos.y * x1[1].x + a->pos.z * x1[2].x + o1->x;
        a->worldPos.y = a->pos.y * x1[1].y + a->pos.z * x1[2].y + o1->y;
        t1 = x1[1].z;
        t2 = x1[2].z;
        t3 = o1->z;
        a->cached = 1;
        a->worldPos.z = a->pos.y * t1 + a->pos.z * t2 + t3;
    }
    if (b->cached == 0) {
        b->worldPos.x = b->pos.y * x2[1].x + b->pos.z * x2[2].x + o2->x;
        b->worldPos.y = b->pos.y * x2[1].y + b->pos.z * x2[2].y + o2->y;
        t1 = x2[1].z;
        t2 = x2[2].z;
        t3 = o2->z;
        b->cached = 1;
        b->worldPos.z = b->pos.y * t1 + b->pos.z * t2 + t3;
    }
    r = a->radius + b->radius;
    d = (a->worldPos.x - b->worldPos.x) * (a->worldPos.x - b->worldPos.x)
      + (a->worldPos.y - b->worldPos.y) * (a->worldPos.y - b->worldPos.y)
      + (a->worldPos.z - b->worldPos.z) * (a->worldPos.z - b->worldPos.z);
    if (d < r * r) {
        if (a->child == 0) {
            if (b->child == 0) {
                return 1;
            }
        } else if (b->child == 0 || b->radius < a->radius) {
            a = a->child;
            do {
                if (collide_spheres_overlap(a, b, x1, x2, o1, o2) & 0xff) {
                    return 1;
                }
                a = a->next;
            } while (a != 0);
            return 0;
        }
        for (b = b->child; b != 0; b = b->next) {
            if (collide_spheres_overlap(a, b, x1, x2, o1, o2) & 0xff) {
                return 1;
            }
        }
    }
    return 0;
}

s32 collide_find_deepest_overlap(CollideNode *p1, CollideNode *p2, Vec3f *m1, Vec3f *m2, Vec3f *v1, Vec3f *v2)
{
    f32 fVar3;
    f32 fVar4;
    f32 fVar5;
    f32 fz;

    f32 d;
    s32 uVar1;
    Vec3f tmp;

    if (p1->cached == 0) {
        p1->worldPos.x = p1->pos.y * m1[1].x + p1->pos.z * m1[2].x + v1->x;
        p1->worldPos.y = p1->pos.y * m1[1].y + p1->pos.z * m1[2].y + v1->y;
        fVar3 = m1[1].z;
        fVar5 = m1[2].z;
        fz = v1->z;
        p1->cached = 1;
        p1->worldPos.z = p1->pos.y * fVar3 + p1->pos.z * fVar5 + fz;
    }
    if (p2->cached == 0) {
        p2->worldPos.x = p2->pos.y * m2[1].x + p2->pos.z * m2[2].x + v2->x;
        p2->worldPos.y = p2->pos.y * m2[1].y + p2->pos.z * m2[2].y + v2->y;
        fVar3 = m2[1].z;
        fVar5 = m2[2].z;
        fz = v2->z;
        p2->cached = 1;
        p2->worldPos.z = p2->pos.y * fVar3 + p2->pos.z * fVar5 + fz;
    }
    fVar4 = p1->radius + p2->radius;
    d = (p1->worldPos.x - p2->worldPos.x) * (p1->worldPos.x - p2->worldPos.x) +
        (p1->worldPos.y - p2->worldPos.y) * (p1->worldPos.y - p2->worldPos.y) +
        (p1->worldPos.z - p2->worldPos.z) * (p1->worldPos.z - p2->worldPos.z);
    if (d < fVar4 * fVar4) {
        if (p1->child == 0) {
            if (p2->child == 0) {
                d = (p1->radius + p2->radius) - __builtin_sqrtf(d);
                if (sCollideDepth < d) {
                    sCollideDepth = d;
                    sCollideHitA = p1;
                    sCollideHitB = p2;
                }
                return 0;
            }
        } else if (p2->child == 0 || p2->radius < p1->radius) {
            p1 = p1->child;
            do {
                uVar1 = collide_find_deepest_overlap(p1, p2, m1, m2, v1, v2);
                if ((uVar1 & 0xff) != 0) {
                    return 1;
                }
                p1 = p1->next;
            } while (p1 != 0);
            return 0;
        }
        for (p2 = p2->child; p2 != 0; p2 = p2->next) {
            uVar1 = collide_find_deepest_overlap(p1, p2, m1, m2, v1, v2);
            if ((uVar1 & 0xff) != 0) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

s32 collide_resolve_rider_hit(Rider *p1, Rider *p2)
{
    f32 vec[3];
    f32 o1[3];
    f32 o2[3];
    s32 a, b;
    RigidBody *q;
    f32 *pa;
    s32 c;

    if (gCheatFlags & 0x400) {
        if (p1->isCpu != 0) {
            if (p2->isCpu != 0) goto skip0;
        } else if (p2->isCpu != 0) {
            bike_crash_begin(p2, 0x1c);
            return 1;
        }
        if (p1->isCpu == 0) goto skip0;
        bike_crash_begin(p1, 0x1c);
        return 1;
    }
skip0:

    if (p1->airFrames >= 0 && p2->airFrames >= 0) return 0;
    if (p1->airFrames < 0x3c) goto L12c;
    if (p2->airFrames >= 0) goto L12c;

    a = p1->isCpu;
    b = p2->isCpu;
    if ((a & b) == 0 || gEventTime < 300) announce_takeout(p1 - gRiders, p2 - gRiders);
    p2->pendingCrash = -0x19;
    goto L1c8;

L12c:
    if (p2->airFrames < 0x3c) goto L1d8;
    if (p1->airFrames >= 0) goto L1d8;
    a = p1->isCpu;
    b = p2->isCpu;
    if ((a & b) == 0 || gEventTime < 300) announce_takeout(p2 - gRiders, p1 - gRiders);
    p1->pendingCrash = -0x1C;

L1c8:
    p1->fistPump = 1;
    p1->engine.heat = 0.0f;

L1d8:
    if (gGameMode == 6) {
        if (p1->bike.pos.z < p2->bike.pos.z) {
            if (p1->bike.pos.x < p2->bike.pos.x) {
                p2->pendingCrash = -0x1F;
            } else {
                p2->pendingCrash = -0x22;
            }
            p2->crashVel = p2->bike.vel;
            return 1;
        }
        if (p2->bike.pos.x < p1->bike.pos.x) {
            p1->pendingCrash = -0x1F;
        } else {
            p1->pendingCrash = -0x22;
        }
        p1->crashVel = p1->bike.vel;
        return 1;
    }

    if (gRaceFrame - p1->contactFrame[p2 - gRiders] < 0x3c) return 0;

    pa = &p1->bike.pos.x;
    vec[0] = gCollidePoint.x - pa[0];
    vec[1] = gCollidePoint.y - pa[1];
    vec[2] = gCollidePoint.z - pa[2];
    q = &p2->bike;
    mtx3f_mul_vec3(vec, &p1->bike.right.x, o1);
    if (-0.2f < o1[2] && o1[2] < 0.2f) return 0;

    vec[0] = gCollidePoint.x - q->pos.x;
    vec[1] = gCollidePoint.y - q->pos.y;
    vec[2] = gCollidePoint.z - q->pos.z;
    mtx3f_mul_vec3(vec, &q->right.x, o2);
    if (-0.2f < o2[2] && o2[2] < 0.2f) return 0;

    if (o2[2] > 0.0f && o1[2] < 0.0f) {
        if (p2->isCpu == 0) {
            if (p2->input & 0x20) return 0;
            if ((p2->input & 0x20010) == 0) return 0;
        }
        p1->fistPump = 1;
        p1->engine.heat = 0.0f;
        if (p1->isCpu == 0 || p2->isCpu == 0) announce_takeout(p1 - gRiders, p2 - gRiders);
        if (o1[0] - o2[0] > 0.0f) {
            p2->pendingCrash = -0x1F;
        } else {
            p2->pendingCrash = -0x22;
        }
        {
            f64 *d = (f64 *)&p2->crashVel;
            f64 *sp = (f64 *)&p2->bike.vel;
            __builtin_memcpy(d, sp, 12);
        }
        return 1;
    }
    if (o1[2] > 0.0f && o2[2] < 0.0f) {
        if (p1->isCpu == 0) {
            if (p1->input & 0x20) return 0;
            if ((p1->input & 0x20010) == 0) return 0;
        }
        p2->fistPump = 1;
        p2->engine.heat = 0.0f;
        if (p1->isCpu == 0 || p2->isCpu == 0) announce_takeout(p2 - gRiders, p1 - gRiders);
        if (o2[0] - o1[0] > 0.0f) {
            p1->pendingCrash = -0x1F;
        } else {
            p1->pendingCrash = -0x22;
        }
        p1->crashVel = p1->bike.vel;
        return 2;
    }
    return 0;
}

#define NORM(v) { f32 len = __builtin_sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); if (len != 0.0f) { v.x /= len; v.y /= len; v.z /= len; } }


s32 collide_apply_impulse(RigidBody *a, RigidBody *b) {
    Vec3f n;
    Vec3f ra;
    Vec3f rb;
    Vec3f va;
    Vec3f vb;
    Vec3f dv;
    Vec3f t;
    Vec3f tq;
    Vec3f imp;
    f32 k, len, h, d, d1, ka, kb, j, lim;
    Rider *ba, *bb, *pa, *pb;
    s32 r, vol, ia, ib;

    n.x = sCollideHitA->worldPos.x - sCollideHitB->worldPos.x;
    n.y = sCollideHitA->worldPos.y - sCollideHitB->worldPos.y;
    n.z = sCollideHitA->worldPos.z - sCollideHitB->worldPos.z;
    NORM(n);
    if (n.y > 0.25f) {
        n.y = 0.25f;
        NORM(n);
    } else if (n.y < -0.25f) {
        n.y = -0.25f;
        NORM(n);
    }
    h = sCollideDepth * 0.5f;
    gCollidePoint.x = n.x * (sCollideHitB->radius - h);
    gCollidePoint.y = n.y * (sCollideHitB->radius - h);
    gCollidePoint.z = n.z * (sCollideHitB->radius - h);
    gCollidePoint.x = sCollideHitB->worldPos.x + gCollidePoint.x;
    gCollidePoint.y = sCollideHitB->worldPos.y + gCollidePoint.y;
    gCollidePoint.z = sCollideHitB->worldPos.z + gCollidePoint.z;
    gCollidePoint.y = (a->pos.y + b->pos.y) * 0.5f;
    ra.x = gCollidePoint.x - a->pos.x;
    ra.y = gCollidePoint.y - a->pos.y;
    ra.z = gCollidePoint.z - a->pos.z;
    vec3_cross(&ra, &a->angVel, &va);
    va.x = a->vel.x + va.x;
    va.y = a->vel.y + va.y;
    va.z = a->vel.z + va.z;
    rb.x = gCollidePoint.x - b->pos.x;
    rb.y = gCollidePoint.y - b->pos.y;
    rb.z = gCollidePoint.z - b->pos.z;
    vec3_cross(&rb, &b->angVel, &vb);
    vb.x = b->vel.x + vb.x;
    vb.y = b->vel.y + vb.y;
    vb.z = b->vel.z + vb.z;
    dv.x = va.x - vb.x;
    dv.y = va.y - vb.y;
    dv.z = va.z - vb.z;
    d = vec3_dot(&dv, &n);
    if (d > 0.0f) {
        return 0;
    }
    if (a->type < 2 && b->type < 2) {
        ba = a->owner;
        bb = b->owner;
        a->mass *= ba->weight;
        a->inertia *= ba->weight;
        b->mass *= bb->weight;
        b->inertia *= bb->weight;
        if (ba->crashState == 0 && bb->crashState == 0) {
            r = collide_resolve_rider_hit(ba, bb);
            bb->contactFrame[ba - gRiders] = gRaceFrame;
            ba->contactFrame[bb - gRiders] = gRaceFrame;
            if (r != 0) {
                return r;
            }
        }
        if (a->type == 0) {
            particles_start_sparks(ba);
        }
        if (b->type == 0) {
            particles_start_sparks(bb);
        }
        vol = (s32)(-d) * 128.0f / 7.0f + 128.0f;
        if (vol > 255) {
            vol = 255;
        }
        if (vol < 0) {
            vol = 0;
        }
        if (vol > 32) {
            snd_play_rider_sfx(4, ba - gRiders, vol);
        }
    }
    d1 = vec3_dot(&n, &ra);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = ra.x - t.x;
    t.y = ra.y - t.y;
    t.z = ra.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    ka = a->mass / (k * a->mass / a->inertia + 1.0f);
    d1 = vec3_dot(&n, &rb);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = rb.x - t.x;
    t.y = rb.y - t.y;
    t.z = rb.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    kb = b->mass / (k * b->mass / b->inertia + 1.0f);
    if (a->type == 4 || b->type == 4) {
        j = (d - 2.0f) * -1.5f * (ka * kb) / (ka + kb);
    } else {
        j = (d - 2.0f) * -1.1f * (ka * kb) / (ka + kb);
    }
    imp.x = n.x * (j / a->mass);
    imp.y = n.y * (j / a->mass);
    imp.z = n.z * (j / a->mass);
    a->vel.x = a->vel.x + imp.x;
    a->vel.y = a->vel.y + imp.y;
    a->vel.z = a->vel.z + imp.z;
    imp.x = n.x * (j / b->mass);
    imp.y = n.y * (j / b->mass);
    imp.z = n.z * (j / b->mass);
    b->vel.x = b->vel.x - imp.x;
    b->vel.y = b->vel.y - imp.y;
    b->vel.z = b->vel.z - imp.z;
    vec3_cross(&ra, &n, &tq);
    a->angVel.x = a->angVel.x + tq.x * (j / a->inertia);
    a->angVel.y = a->angVel.y + tq.y * (j / a->inertia);
    a->angVel.z = a->angVel.z + tq.z * (j / a->inertia);
    vec3_cross(&rb, &n, &tq);
    b->angVel.x = b->angVel.x - tq.x * (j / b->inertia);
    b->angVel.y = b->angVel.y - tq.y * (j / b->inertia);
    b->angVel.z = b->angVel.z - tq.z * (j / b->inertia);
    if (a->type < 2 && b->type < 2) {
        a->mass = 250.0f;
        a->inertia = 250.0f;
        b->mass = 250.0f;
        b->inertia = 250.0f;
        pa = a->owner;
        pb = b->owner;
        if (pa->rearWheel.airTicks <= 0) {
            pa->rearWheel.spin = vec3_dot(&pa->rearWheel.lonAxis.x, &pa->bike.vel.x);
        }
        if (pa->frontWheel.airTicks <= 0) {
            pa->frontWheel.spin = vec3_dot(&pa->frontWheel.lonAxis.x, &pa->bike.vel.x);
        }
        pa->speed = __builtin_sqrtf(pa->bike.vel.x * pa->bike.vel.x + pa->bike.vel.y * pa->bike.vel.y + pa->bike.vel.z * pa->bike.vel.z);
        if (pb->rearWheel.airTicks <= 0) {
            pb->rearWheel.spin = vec3_dot(&pb->rearWheel.lonAxis.x, &pb->bike.vel.x);
        }
        if (pb->frontWheel.airTicks <= 0) {
            pb->frontWheel.spin = vec3_dot(&pb->frontWheel.lonAxis.x, &pb->bike.vel.x);
        }
        pb->speed = __builtin_sqrtf(pb->bike.vel.x * pb->bike.vel.x + pb->bike.vel.y * pb->bike.vel.y + pb->bike.vel.z * pb->bike.vel.z);
        if ((pa->wheelieTimer != 0 || (u8)pa->anim < 17) && pa->airFrames <= 0) {
            pa->pendingCrash = -25;
            pa->crashVel = pa->bike.vel;
            return 1;
        }
        if ((pb->wheelieTimer != 0 || (u8)pb->anim < 16) && pb->airFrames <= 0) {
            pb->pendingCrash = -25;
            pb->crashVel = pb->bike.vel;
            return 1;
        }
        if (pa->race.place + pb->race.place == 1 && gEventTime < 360 && !(pa->isCpu & pb->isCpu)) {
            if (gRaceFrame - gAnnounceTimes[pa - gRiders] < 600) {
                announce_place(pb - gRiders, pb->race.place);
                return 1;
            }
            ib = pb - gRiders;
            if (gRaceFrame - gAnnounceTimes[ib] < 600) {
                announce_place(pa - gRiders, pa->race.place);
                return 1;
            }
            announce_lead_collision(pa - gRiders, ib);
        }
    }
    return 1;
}

s32 collide_bodies(RigidBody *p1, RigidBody *p2)
{
    CollideNode *s0;
    CollideNode *v;
    RigidBody *tb;
    s32 a, b;

    if (p2 == p1) {
        return 0;
    }
    v = p1->tree;
    sCollideDepth = 0.0f;
    s0 = v;
    if (s0 != 0 && s0->cached != 0) {
        s0->cached = 0;
        s0 = s0->child;
        while (s0 != 0) {
            collide_tree_clear_cache(s0);
            s0 = s0->next;
        }
    }
    if (p1->owner == p2->owner &&
        p1->owner->crashState != 2) {
        if (collide_spheres_overlap(p1->tree, p2->tree, &p1->right, &p2->right, &p1->pos, &p2->pos) & 0xff) {
            return 0;
        }
        p1->owner->crashState = 2;
    }
    collide_find_deepest_overlap(p1->tree, p2->tree, &p1->right, &p2->right, &p1->pos, &p2->pos);
    if (0.0f < sCollideDepth) {
        if (collide_apply_impulse(p1, p2) != 0) {
            p2->flags = p2->flags | 1;
            if ((p1->type == 4 && p2->type == 0) || (p2->type == 4 && p1->type == 0)) {
                if (p1->type == 4) {
                    p1 = p2;
                }
            } else {
                return 1;
            }
            tb = &gRiders[0].bike;
            if (p1 == tb) {
                D_800F0616 = 0;
            } else if (p1 == &gRiders[1].bike) {
                D_800F0616 = 1;
            } else if (p1 == &gRiders[2].bike) {
                D_800F0616 = 2;
            } else if (p1 == &gRiders[3].bike) {
                D_800F0616 = 3;
            }
            return 1;
        }
        p1->unk_17E = 0xf;
        p2->unk_17E = 0xf;
    }
    return 0;
}

void collide_tree_clear_cache(CollideNode *p) {
    if (p != NULL && p->cached != 0) {
        p->cached = 0;
        p = p->child;
        while (p != NULL) {
            collide_tree_clear_cache(p);
            p = p->next;
        }
    }
}

void collide_free_node_pool(void) {
    mem_free(sCollideNodePool);
}

void collide_init_node_pool(void)
{
    sCollideNodeCount = 0;
    sCollideNodePool = mem_alloc_dbg(16000, 0, sStrCollideC, 0x103);
    sCollideNodeNext = sCollideNodePool;
    mem_fill(sCollideNodePool, 0, 16000);
}

void collide_reset_body(RigidBody *p)
{
    f32 one = 1.0f;
    p->vel.x = 0.0f;
    p->vel.y = 0.0f;
    p->vel.z = 0.0f;
    p->acc.x = 0.0f;
    p->acc.y = 0.0f;
    p->acc.z = 0.0f;
    p->angVel.x = 0.0f;
    p->angVel.y = 0.0f;
    p->angVel.z = 0.0f;
    p->angAcc.x = 0.0f;
    p->angAcc.y = 0.0f;
    p->angAcc.z = 0.0f;
    p->quat[0] = 0.0f;
    p->quat[1] = 0.0f;
    p->quat[3] = 0.0f;
    p->quat[2] = one;
    quat_to_mtx3f(&p->right.x, p->quat);
}

void collide_update_bike_speeds(Rider *p)
{
    f32 s;
    f32 r;

    if (p->rearWheel.airTicks < 1) {
        p->rearWheel.spin = vec3_dot(&p->rearWheel.lonAxis.x, &p->bike.vel.x);
    }
    if (p->frontWheel.airTicks < 1) {
        p->frontWheel.spin = vec3_dot(&p->frontWheel.lonAxis.x, &p->bike.vel.x);
    }
    s = p->bike.vel.x * p->bike.vel.x
      + p->bike.vel.y * p->bike.vel.y
      + p->bike.vel.z * p->bike.vel.z;
    r = __builtin_sqrtf(s);
    p->speed = r;
}

void collide_reset_tree(CollideNode *p) {
    CollideNode *q;
    sCollideDepth = 0;
    q = p;
    if (q != NULL && q->cached != 0) {
        q->cached = 0;
        for (q = q->child; q != NULL; q = q->next) {
            collide_tree_clear_cache(q);
        }
    }
}
