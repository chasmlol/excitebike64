#include "game.h"

void bike_crash_begin(Rider *b, s32 n)
{
    s16 rx, ry, rz;
    Vec3f v;
    Vec3f c;
    Vec3f w;
    s32 idx;
    s32 i;
    f32 s;
    Vec3f *src;

    if (b->crashState != 0) {
        return;
    }
    if (b->isCpu == 0 && sCrashAnnounceFrame != gRaceFrame) {
        announce_crash(b - gRiders);
    }
    b->engine.rpm = 0.0f;
    b->engine.gear = 0;
    if (b == &gRiders[1]) {
        gEvent.flags &= ~0x1000;
    }
    b->unk_3FB = 0;
    b->fistPump = 0;
    b->savedCamMode = (u16)b->camMode;
    if (b->isCpu == 0) {
        flash_start(0x3C);
        tunes_queue_crowd(0);
    }
    if (gGame.mode == 6) {
        b->prevSegment = path_find_segment(&b->bike.pos, b->segment, 10, 1);
    }
    idx = b - gRiders;
    b->wheelieTimer = 0;
    b->airFrames = 0;
    b->bike.type = 2;
    b->crashPhase = 1;
    b->crashDir = (n - 0x19) / 3;
    bike_skel_update(b->bikeSkel, idx);
    rider_update_pose(b->riderSkel, idx);
    mtx3i_to_angles(b->riderSkel->bones[0].worldRot.m, (u16 *)&rx, (u16 *)&ry, (u16 *)&rz);
    quat_from_euler(b->body.quat, (f32)rx * 3.1415927f * 3.0517578e-05f,
                  (f32)ry * 3.1415927f * 3.0517578e-05f,
                  (f32)rz * 3.1415927f * 3.0517578e-05f);
    b->crashState = 1;
    b->crashTimer = 0;
    b->prevAnim = (u8)b->anim;
    b->prevAnimFrame = b->animFrame;
    b->crashSpeedCap = b->speed + 5.0f;
    if (b->crashSpeedCap > 32.0f) {
        b->crashSpeedCap = 32.0f;
    }
    b->speed = 0.0f;
    for (i = 0; i < 11; i++) {
        vec3_mul_mtx3f(sBikeCrashContactPoints[i], (f32 *)&b->bike.right, &v.x);
        b->bike.contacts[i].x = v.x + b->bike.pos.x;
        b->bike.contacts[i].y = v.y + b->bike.pos.y;
        b->bike.contacts[i].z = v.z + b->bike.pos.z;
    }
    if (n == 0x22) {
        s = 2.0f;
        goto adj;
    }
    if (n == 0x1F) {
        s = -2.0f;
    adj:
        if (b->speed < 5.0f) {
            s *= 0.25f;
        }
        b->bike.angVel.x += s * b->bike.fwd.x;
        b->bike.angVel.y += s * b->bike.fwd.y;
        b->bike.angVel.z += s * b->bike.fwd.z;
    }
    b->body.pos.x = (f32)(b->riderSkel->bones[0].worldPos.x + b->drawPos.x) / 1000.0f;
    b->body.pos.y = (f32)(b->riderSkel->bones[0].worldPos.y + b->drawPos.y) / 1000.0f;
    b->body.pos.z = (f32)(b->riderSkel->bones[0].worldPos.z + b->drawPos.z) / 1000.0f;
    s = __builtin_sqrtf(b->bike.vel.x * b->bike.vel.x + b->bike.vel.y * b->bike.vel.y + b->bike.vel.z * b->bike.vel.z) / 5.0f;
    if (s < 1.0f) {
        s = 1.0f;
    }
    if (s > 5.0f) {
        s = 5.0f;
    }
    src = &b->bike.vel;
    if (b->pendingCrash < 0) {
        src = &b->crashVel;
    }
    vec3_cross(&b->bike.angVel, &b->bike.up, &c);
    b->body.vel.x = src->x + c.x + b->bike.up.x * s;
    b->body.vel.y = src->y + c.y + b->bike.up.y * s;
    b->body.vel.z = src->z + c.z + b->bike.up.z * s;
    b->body.angVel = b->bike.angVel;
    if (n == 0x19) {
        b->body.angVel.x -= b->bike.right.x + b->bike.right.x;
        b->body.angVel.y -= b->bike.right.y + b->bike.right.y;
        b->body.angVel.z -= b->bike.right.z + b->bike.right.z;
    }
    b->animFrame = 0;
    b->body.flags |= 1;
    b->bike.flags |= 1;
    for (i = 0; i < 8; i++) {
        vec3_mul_mtx3f(sRiderCrashContactPoints[b->crashDir][i], (f32 *)&b->bike.right, &w.x);
        b->body.contacts[i].x = w.x + b->body.pos.x;
        b->body.contacts[i].y = w.y + b->body.pos.y;
        b->body.contacts[i].z = w.z + b->body.pos.z;
    }
    b->bike.unk_170 = 1.0f;
    b->body.unk_170 = 1.0f;
    stunt_start(b, n);
    collide_tree_clear_cache(b->body.tree);
}

#define DAMPR(x) { f32 r, k, v, w; k = (gGame.mode == 6) ? gBikeTuneSpringRate * 20000.0f : 15000.0f; v = (x); w = v; if (gGame.mode == 6) r = v - v * (k / ((gBikeTuneDamping * 2000.0f) / gPhysDeltaTime)); else r = w - w * (k / (2200.0f / gPhysDeltaTime)); (x) = r; }
#define CLAMP(b, p) { m = __builtin_sqrtf(b->p.vel.x * b->p.vel.x + b->p.vel.y * b->p.vel.y + b->p.vel.z * b->p.vel.z);     if (b->crashSpeedCap < m) { b->p.vel.x *= b->crashSpeedCap / m; b->p.vel.y *= b->crashSpeedCap / m; b->p.vel.z *= b->crashSpeedCap / m; } }

static inline f32 airtime(RigidBody *p, f32 g)
{
    f32 h, t, s, a, v;
    h = p->pos.y - (f32)height_get_ground(p->pos.x * 1000.0f, p->pos.y * 1000.0f, p->pos.z * 1000.0f) / 1000.0f;
    if (h < 0.0f) return 0.0f;
    v = p->vel.y;
    s = __builtin_sqrtf(v * v + h * 29.400002f) / g;
    a = -v / g;
    t = -a + s;
    if (t > 0.0f) return t;
    return s + a;
}

static inline void lane(Rider *b, s32 n) { s32 t = b->anim + n; b->prevAnim = b->anim; b->prevAnimFrame = b->animFrame; stunt_start(b, t); }

void bike_crash_update(Rider *b)
{
    s32 vol, id, n, ch;
    s32 unused[8];
    s32 tmp[2];
    f32 g, dt, m;

    stunt_animate(b, 0);
    b->crashTimer++;
    DAMPR(b->frontWheel.suspLength);
    DAMPR(b->rearWheel.suspLength);
    b->steer *= 0.9f;
    if (b->isCpu == 0 && (gGame.numPlayers == 1 || gGame.mode == 8)) {
        gCrashSlowMotion = 1;
    }
    height_get_ground(b->bike.pos.x * 1000.0f, b->bike.pos.y * 1000.0f, b->bike.pos.z * 1000.0f);
    if ((gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & 0x800) && b->isCpu == 0) {
        cam_start_fall_view(b);
    }
    if (b->bike.flags & 1) {
        b->bike.acc.z = 0.0f;
        b->bike.acc.x = 0.0f;
        b->bike.acc.y = -14.700001f;
        b->impactSpeed = b->slideSpeed = b->bike.acc.z;
        phys_body_step(&b->bike, sBikeCrashContactPoints, 0xB, 1);
        CLAMP(b, bike);
        if (b->impactSpeed > 0.5f) {
            vol = b->impactSpeed * 128.0f;
            vol += 0x30;
            if (vol >= 0x100) vol = 0xFF;
            if (vol > 0x80 || gRaceFrame - sImpactSfxFrame >= 0x1F) {
                sImpactSfxFrame = gRaceFrame;
                if (gRaceFrame & 1) {
                    snd_play_rider_sfx(4, b - gRiders, vol);
                    snd_play_rider_sfx(4, b - gRiders, vol);
                } else {
                    snd_play_rider_sfx(8, b - gRiders, vol);
                    snd_play_rider_sfx(8, b - gRiders, vol);
                }
            }
        }
        b->bike.vel.y -= gPhysDeltaTime * 14.700001f;
        b->bike.angVel.x -= b->bike.angVel.x * gPhysDeltaTime * 0.5f;
        b->bike.angVel.y -= b->bike.angVel.y * gPhysDeltaTime * 0.5f;
        b->bike.angVel.z -= b->bike.angVel.z * gPhysDeltaTime * 0.5f;
    } else {
        b->crashTimer += 20;
    }
    if (b->body.flags & 1) {
        b->body.acc.x = 0.0f;
        b->body.acc.z = 0.0f;
        b->body.acc.y = -14.700001f;
        phys_body_step(&b->body, sRiderCrashContactPoints[b->crashDir], 8, 1);
        CLAMP(b, body);
        dt = gPhysDeltaTime;
        g = 14.700001f;
        b->body.vel.y -= dt * g;
        b->body.angVel.x -= b->body.angVel.x * gPhysDeltaTime;
        b->body.angVel.y -= b->body.angVel.y * gPhysDeltaTime;
        b->body.angVel.z -= b->body.angVel.z * gPhysDeltaTime;
        switch (b->crashPhase) {
        case 1:
            if (rider_anim_get_length(b->anim, tmp) - (b->animFrame >> 8) < 2) {
                n = (airtime(&b->body, g) < 0.5f) ? 2 : 1;
                lane(b, n);
                b->crashPhase += n;
            }
            break;
        case 2:
            if (airtime(&b->body, g) < 0.5f) {
                lane(b, 1);
                b->crashPhase = 3;
            }
            break;
        case 3:
            break;
        }
    }
    ch = 0x78;
    if (gCrashSlowMotion != 0 && b->isCpu == 0) ch = 0xF0;
    if (gGame.mode == 7) ch = 0x12C;
    if (ch < (s16)b->crashTimer) bike_respawn(b);
}

#define HEIGHT(v) ((f32)height_get_ground((s32)((v)->x * 1000.0f), (s32)((v)->y * 1000.0f), (s32)((v)->z * 1000.0f)) / 1000.0f)

struct Pt_4 {
    Vec3f v;
    f32 w;
};

typedef struct { u8 pad[0x2C]; s32 f2C; } Game_16;

extern Game_16 *gCurTrack_25;

s32 phys_body_step(RigidBody *b, Pt_4 *t, s32 n, u8 m)
{
    Vec3f p;
    Vec3f dv;
    Vec3f rot[40];
    Vec3f world[40];
    Quat_2 dq;
    Quat_2 q;
    f32 depth[42];
    Vec3f w;
    Vec3f savepos;
    Quat_2 saveq;
    Vec3f d;
    Vec3f r;
    f32 tmax;
    f32 hx, lx, hz, lz;
    f32 a, e, x, gk, dt, len, half, f, sn;
    s32 i, k;
    u8 hit;
    Rider *o;
    s32 idx;

    if (b->vel.x > 0.0f) {
        hx = b->vel.x * gPhysDeltaTime + 1.0f;
        lx = -1.0f;
    } else {
        hx = 1.0f;
        lx = b->vel.x * gPhysDeltaTime - 1.0f;
    }
    if (b->vel.z > 0.0f) {
        hz = b->vel.z * gPhysDeltaTime + 1.0f;
        lz = -1.0f;
    } else {
        hz = 1.0f;
        lz = b->vel.z * gPhysDeltaTime - 1.0f;
    }
    if ((gDrawFlags & 0x200) && b == &gRiders[0].bike) {
        hx = 4.0f;
        lx = -4.0f;
        hz = hx;
        lz = lx;
    }
    height_gather_walls((s32)((b->pos.x + lx) * 1000.0f), (s32)((b->pos.z + lz) * 1000.0f),
                  (s32)((b->pos.x + hx) * 1000.0f), (s32)((b->pos.z + hz) * 1000.0f));
    if (b->hitTimer != 0) {
        b->hitTimer -= ((u8 *)&gPhysTicks)[3];
        if (b->hitTimer < 0) {
            b->hitTimer = 0;
        }
    }
    dt = gPhysDeltaTime;
    do {
        w.x = b->vel.x + b->acc.x * dt * 0.5f;
        w.y = b->vel.y + b->acc.y * dt * 0.5f;
        w.z = b->vel.z + b->acc.z * dt * 0.5f;
        p.x = w.x * dt;
        p.y = w.y * dt;
        p.z = w.z * dt;
        p.x = b->pos.x + p.x;
        p.y = b->pos.y + p.y;
        p.z = b->pos.z + p.z;
        w.x = b->angVel.x + b->angAcc.x * dt * 0.5f;
        w.y = b->angVel.y + b->angAcc.y * dt * 0.5f;
        w.z = b->angVel.z + b->angAcc.z * dt * 0.5f;
        tmax = 1.0f;
        len = __builtin_sqrtf(w.x * w.x + w.y * w.y + w.z * w.z);
        half = len * dt * 0.5f;
        if (len != 0.0f) {
            sn = sinf(half) / len;
        } else {
            sn = 0.0f;
        }
        dq.q[0] = cosf(half);
        dq.q[1] = w.x * sn;
        dq.q[2] = w.y * sn;
        dq.q[3] = w.z * sn;
        quat_mul(q.q, dq.q, b->quat);
        quat_to_mtx3f(&b->right.x, q.q);
        for (i = 0; i < n; i++) {
            vec3_mul_mtx3f(&t[i].v.x, &b->right.x, &rot[i].x);
            world[i].x = rot[i].x + p.x;
            world[i].y = rot[i].y + p.y;
            world[i].z = rot[i].z + p.z;
        }
        if (m != 0) {
            if (b->type == 4) {
                x = p.y - (f32)height_get_ground((s32)(p.x * 1000.0f), (s32)(p.y * 1000.0f), (s32)(p.z * 1000.0f)) / 1000.0f - 1.0f;
                if (x < 0.0f) {
                    w.x = 0.0f;
                    w.y = -1.0f;
                    w.z = 0.0f;
                    p.y += 0.005f - x;
                    vec3_cross(&b->angVel, &w, &dv);
                    dv.x = b->vel.x + dv.x;
                    dv.y = b->vel.y + dv.y;
                    dv.z = b->vel.z + dv.z;
                    height_get_ground_normal(&gContactNormal_2);
                    phys_contact_impulse(b, &gContactNormal_2, &w, 0.3f, &dv, 0.7f);
                }
            } else {
                x = 0.0f;
                for (i = 0; i < n; i++) {
                    depth[i] = HEIGHT(world + i) - world[i].y;
                    if (x < depth[i]) {
                        x = depth[i];
                        k = i;
                        height_get_ground_normal(&gContactNormal_2);
                    }
                }
                if (x > 0.0f) {
                    if (phys_check_resting(&b->vel.x, &b->angVel.x, depth, rot, n, b->unk_170) < 0.0f) {
                        b->flags &= ~1;
                        return 0;
                    }
                    p.y += depth[k] + 0.01f;
                    dv.x = (world[k].x - b->contacts[k].x) / dt;
                    dv.y = (world[k].y - b->contacts[k].y) / dt;
                    dv.z = (world[k].z - b->contacts[k].z) / dt;
                    gk = t[k].w;
                    if (b->owner != NULL && b->owner->crashState != 0) {
                        gk /= b->owner->weight;
                    }
                    phys_contact_impulse(b, &gContactNormal_2, &rot[k], gk, &dv, 0.1f);
                    e = dv.x * dv.x + dv.y * dv.y + dv.z * dv.z;
                    o = b->owner;
                    if (b->type == 2 && o->isCpu == 0 && e > 4.0f && t[k].w > 0.15f) {
                        if (o != NULL) {
                            if (sImpactSlideSpeed > o->slideSpeed) {
                                o->slideSpeed = sImpactSlideSpeed;
                            }
                            if (sImpactNormalSpeed > o->impactSpeed) {
                                o->impactSpeed = sImpactNormalSpeed;
                            }
                            if (gRiderAlpha[o - gRiders] == 0xFF) {
                                particles_start_dust_cloud(&world[k], o - gRiders);
                            }
                        }
                        if (o->isCpu == 0 && gRaceFrame - o->landingFrame >= 0x10) {
                            o->landingImpact = e;
                            o->landingFrame = gRaceFrame;
                            o->impactFlags = 0xFF;
                        }
                    }
                    for (i = 0; i < n; i++) {
                        world[i].x = p.x + rot[i].x;
                        world[i].y = p.y + rot[i].y;
                        world[i].z = p.z + rot[i].z;
                    }
                }
            }
        }
        hit = 0;
        for (i = 0; i < n; i++) {
            if (height_raycast_walls(&b->contacts[i], &world[i], &gContactNormal_2, &tmax, 0)) {
                dv.x = (world[i].x - b->contacts[i].x) / dt;
                dv.y = (world[i].y - b->contacts[i].y) / dt;
                k = i;
                dv.z = (world[i].z - b->contacts[i].z) / dt;
                if (gWallHitFlags & 1) {
                    hit = 1;
                }
            }
        }
        if (tmax < 1.0f) {
            a = vec3_dot(&gContactNormal_2.x, &dv.x);
            x = 0.3f;
            if (b->type < 2) {
                o = b->owner;
                if (gContactNormal_2.y > 0.75f && (k > 5 || o->airFrames > 60)) {
                    o->landingImpact = 0x200;
                    o->landingFrame = gRaceFrame;
                    o->impactFlags = 0xFF;
                    o->pendingCrash = (t[k].v.z > 0.0f) ? -0x19 : -0x1C;
                    o->crashVel = o->bike.vel;
                } else if (a < -15.0f) {
                    if (gCurTrack_25->f2C == 0 && gGame.mode != 10 && gGame.mode != 2) {
                        sCrashAnnounceFrame = gRaceFrame;
                        if ((f32)height_get_ground((s32)(o->bike.pos.x * 1000.0f), (s32)(o->bike.pos.y * 1000.0f), (s32)(o->bike.pos.z * 1000.0f)) / 1000.0f - o->bike.pos.y > 2.0f) {
                            announce_crash(o - gRiders);
                        } else {
                            announce_crash_landing(o - gRiders);
                        }
                    }
                    o->landingImpact = 0x200;
                    o->landingFrame = gRaceFrame;
                    o->impactFlags = 0xFF;
                    o->pendingCrash = -0x1C;
                    o->crashVel = o->bike.vel;
                }
                if (o->speed < 10.0f) {
                    x = 0.2f;
                } else {
                    x = o->unk_7D4;
                }
            }
            phys_contact_impulse(b, &gContactNormal_2, &rot[k], x, &dv, 0.05f);
            if (b->type < 2) {
                Rider *o = b->owner;
                if ((o->wheelieTimer != 0 || ((u8)o->anim > 0 && (u8)o->anim < 16)) && o->airFrames <= 0) {
                    o->pendingCrash = -0x19;
                    o->crashVel = o->bike.vel;
                }
            }
            {
            f32 ln = __builtin_sqrtf(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z);
            if (ln > 2.0f && hit) {
                if (t[k].w > 0.15f) {
                    if (b->type < 2) {
                        b->owner->sparkDir = t[k].v;
                        b->owner->sparkTimer = 10;
                        snd_trigger_impact(b->owner - gRiders, (s32)(ln * 16.0f));
                    } else if (b->type == 2) {
                        Rider *o = b->owner;
                        if (sImpactSlideSpeed > o->slideSpeed) {
                            o->slideSpeed = sImpactSlideSpeed;
                        }
                        if (sImpactNormalSpeed > o->impactSpeed) {
                            o->impactSpeed = sImpactNormalSpeed;
                        }
                    }
                }
            }
            }
            dt *= 0.8f;
            b->hitTimer = 15;
        } else {
            f32 ln;
            savepos = b->pos;
            saveq = *(Quat_2 *)b->quat;
            b->pos = p;
            *(Quat_2 *)b->quat = q;
            if (b->type < 2 && m == 0) {
                o = b->owner;
                bike_update_wheel_contacts(o);
                if (o->frontWheel.normal.y < 0.5f && o->frontWheel.suspLength <= o->frontWheel.groundDist + 0.01f) {
                    if (o->airFrames > 60 || vec3_dot(&o->frontWheel.normal.x, &o->bike.up.x) < 0.3f) {
                        o->pendingCrash = -0x1C;
                        o->crashVel = o->bike.vel;
                    } else {
                        d.x = o->frontWheel.contact.x - p.x;
                        d.y = o->frontWheel.contact.y - p.y;
                        d.z = o->frontWheel.contact.z - p.z;
                        vec3_cross(&d, &b->angVel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->frontWheel.normal.x * o->frontWheel.normal.x + o->frontWheel.normal.z * o->frontWheel.normal.z;
                        ln = __builtin_sqrtf(ln);
                        w.x = o->frontWheel.normal.x / ln;
                        w.y = 0.0f;
                        w.z = o->frontWheel.normal.z / ln;
                        f = vec3_dot(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        dt *= 0.5f;
                        phys_contact_impulse(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        *(Quat_2 *)b->quat = saveq;
                        quat_to_mtx3f(&b->right.x, b->quat);
                        bike_update_wheel_contacts(o);
                        continue;
                    }
                }
                if (o->rearWheel.normal.y < 0.5f && o->rearWheel.suspLength <= o->rearWheel.groundDist + 0.01f) {
                    if (o->airFrames > 60 || vec3_dot(&o->rearWheel.normal.x, &o->bike.up.x) < 0.3f) {
                        o->pendingCrash = -0x1C;
                        o->crashVel = o->bike.vel;
                    } else {
                        d.x = o->rearWheel.contact.x - p.x;
                        d.y = o->rearWheel.contact.y - p.y;
                        d.z = o->rearWheel.contact.z - p.z;
                        vec3_cross(&d, &b->angVel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->rearWheel.normal.x * o->rearWheel.normal.x + o->rearWheel.normal.z * o->rearWheel.normal.z;
                        w.x = o->rearWheel.normal.x / ln;
                        w.y = 0.0f;
                        w.z = o->rearWheel.normal.z / ln;
                        f = vec3_dot(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        phys_contact_impulse(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        *(Quat_2 *)b->quat = saveq;
                        w.x *= 0.125f;
                        w.y *= 0.125f;
                        w.z *= 0.125f;
                        b->vel.x = w.x + b->vel.x;
                        b->vel.y = w.y + b->vel.y;
                        b->vel.z = w.z + b->vel.z;
                        dt *= 0.5f;
                        quat_to_mtx3f(&b->right.x, b->quat);
                        bike_update_wheel_contacts(o);
                        dt -= 0.25f;
                        continue;
                    }
                }
            }
            dt = 0.0f;
        }
    } while (dt > 0.0002f);

    for (i = 0; i < n; i++) {
        b->contacts[i] = world[i];
    }
    if (b->type != 4) {
        o = b->owner;
        if (o == NULL || o->noCollide != 0) {
            return 0;
        }
        if (o->crashState != 0) {
            if (b->tree != 0) {
                collide_bodies(b, &o->bike);
                collide_bodies(b, &o->body);
            }
            return 0;
        }
    }
    if (b->tree != 0) {
        for (i = 0; i < 8; i++) {
            if (gRiders[i].inactive == 0 && gRiders[i].noCollide == 0 && gRiders[i].crashState == 0) {
                collide_bodies(b, &gRiders[i].bike);
            }
        }
        if (gGame.mode == 2) {
            collide_bodies(b, &gSoccerBall);
        }
    }
    return 0;
}

void phys_contact_impulse(RigidBody *b, Vec3f *n, Vec3f *p, f32 maxr, Vec3f *c, f32 fr)
{
    Vec3f A;
    Vec3f B;
    volatile s32 pad[4];
    Vec3f C;
    Vec3f L;
    f32 f, d, imp, len, r, k, q;

    f = vec3_dot((f32 *)n, (f32 *)p);
    L.x = n->x * f;
    L.y = n->y * f;
    L.z = n->z * f;
    L.x = p->x - L.x;
    L.y = p->y - L.y;
    L.z = p->z - L.z;
    f = L.x * L.x + L.y * L.y + L.z * L.z;
    imp = b->mass / (f * (b->mass / b->inertia) + 1.0f);
    d = -vec3_dot((f32 *)c, (f32 *)n);
    sImpactNormalSpeed = d;
    if (64.0f < d) {
        vec3_cross(&b->angVel, p, c);
        c->x += b->vel.x;
        c->y += b->vel.y;
        c->z += b->vel.z;
        d = -vec3_dot((f32 *)c, (f32 *)n);
    }
    d += 0.05f;
    k = (fr + 1.0f) * d * imp;
    if (k < 0.0f) {
        k = 0.0f;
    }
    A.x = c->x + n->x * d;
    A.y = c->y + n->y * d;
    A.z = c->z + n->z * d;
    len = __builtin_sqrtf(A.x * A.x + A.y * A.y + A.z * A.z);
    sImpactSlideSpeed = len;
    if (len != 0.0f) {
        A.x /= len;
        A.y /= len;
        A.z /= len;
    }
    r = 0.0f;
    if (d != 0.0f) {
        r = len / d;
    }
    if (maxr < r) {
        r = maxr;
    }
    q = -r;
    q = q * k;
    C.x = n->x * k + A.x * q;
    C.y = n->y * k + A.y * q;
    C.z = n->z * k + A.z * q;
    b->vel.x += C.x / b->mass;
    b->vel.y += C.y / b->mass;
    b->vel.z += C.z / b->mass;
    vec3_cross(p, &C, &B);
    b->angVel.x += B.x / b->inertia;
    b->angVel.y += B.y / b->inertia;
    b->angVel.z += B.z / b->inertia;
    b->acc.z = 0.0f;
    b->acc.y = 0.0f;
    b->acc.x = 0.0f;
    b->angAcc.z = 0.0f;
    b->angAcc.y = 0.0f;
    b->angAcc.x = 0.0f;
}

f32 phys_check_resting(f32 *a, f32 *b, f32 *h, Vec3f *v, s32 n, f32 thr)
{
    s32 idx[10];
    s32 i, j, cnt, pos;
    f32 res;
    Vec3f *pa;
    Vec3f *pb;

    res = 0.2f;
    if (a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + (b[0] * b[0] + b[1] * b[1] + b[2] * b[2]) < thr) {
        res = 0.0f;
        cnt = 0;
        for (i = 0; i < n; i++) {
            if (h[i] > -0.02f) {
                idx[cnt++] = i;
            }
        }
        if (cnt >= 3) {
            pos = 0;
            for (i = 0; i < cnt; i++) {
                j = i + 1;
                if (j == cnt) {
                    j = 0;
                }
                pa = (Vec3f *)(idx[i] * 12 + (s32)v);
                pb = (Vec3f *)(idx[j] * 12 + (s32)v);
                if ((pa->x - pb->x) * pa->z - (pa->z - pb->z) * pa->x > 0.0f) {
                    pos++;
                }
            }
            if (pos == cnt || pos == 0) {
                b[0] = b[1] = b[2] = 0.0f;
                a[0] = a[1] = a[2] = 0.0f;
                return -1.0f;
            }
        }
    }
    return res;
}

void bike_respawn(Rider *b) {
    s32 s;
    s32 v, i, k, idx;
    PathNode *n;
    Game *g;
    Rider *p;
    f32 z, r2, half, t, d, dx, dz, y;

    if (b->isCpu == 2) {
        b->inactive = 1;
        return;
    }
    b->crashPhase = 0;
    b->jumpState = 0;
    b->airFrames = 0;
    b->roughLanding = 0;
    b->wheelieTimer = 0;
    b->pendingCrash = 0;
    b->crashTimer = 0;
    b->crashState = 0;
    b->offTrackTimer = 0;
    b->engine.heat = 0.0f;
    b->engine.overheated = 0;
    b->prevAnim = -1;
    b->queuedTrick = -1;
    b->unk_3FB = 0;
    b->unk_7DC = 0;
    if (b->finishState == 0) {
        b->blinkCount = 40;
    }
    b->blinkTimer = 16;
    b->noCollide = 1;
    b->timeScale = 1.0f;
    b->bike.type = b->isCpu != 0;
    if (b->isCpu == 0) {
        gCrashSlowMotion = 0;
        if (gCamTargetOverride == -1) {
            gCamLastModes[b - gRiders] = -1;
        } else if (gCamTargetOverride == b - gRiders) {
            gCamLastModes[0] = -1;
        }
    }
    s = -1;
    g = &gGame;
    switch (g->mode) {
    case 2:
        if ((gEvent.flags & 0xF) || (gEvent.state != 7 && gEvent.state != 11 && gEvent.state != 9)) {
        b->blinkCount = 0;
        b->blinkTimer = 0;
        if (g->numPlayers == 4) {
            switch (b - gRiders) {
            case 0:
                b->bike.pos.x = -1.0f;
                b->bike.pos.z = -10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 0.0f);
                break;
            case 1:
                b->bike.pos.x = 1.0f;
                b->bike.pos.z = -10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 0.0f);
                break;
            case 2:
                b->bike.pos.x = 1.0f;
                b->bike.pos.z = 10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 3.1415927f);
                break;
            case 3:
                b->bike.pos.x = -1.0f;
                b->bike.pos.z = 10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 3.1415927f);
                break;
            }
        } else {
            switch (b - gRiders) {
            case 0:
                b->bike.pos.x = 0.015f;
                b->bike.pos.z = -10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 0.00958738f);
                break;
            case 1:
                b->bike.pos.x = 0.01f;
                b->bike.pos.z = 10.0f;
                quat_from_euler(b->bike.quat, 0.0f, 0.0f, 3.15118f);
                break;
            }
        }
        } else {
            height_push_off_wall(&b->bike.pos.x);
            quat_from_euler(b->bike.quat, 0.0f, 0.0f, math_atan2f(-(b->bike.pos.z - gSoccerBall.pos.z), b->bike.pos.x - gSoccerBall.pos.x));
        }
        b->noCollide = 0;
        break;
    case 4:
    case 5:
        height_push_out_of_desert_objs(&b->bike.pos.x);
        quat_from_euler(b->bike.quat, 0.0f, 0.0f, (f32)b->heading * 3.1415927f * 3.0517578e-05f);
        break;
    case 10:
        height_push_off_wall(&b->bike.pos.x);
        quat_from_euler(b->bike.quat, 0.0f, 0.0f, (f32)b->heading * 3.1415927f * 3.0517578e-05f);
        break;
    default:
        s = b->prevSegment;
        if (gGame.mode == 6 && s > 0) {
            s--;
        }
        if (gGame.mode == 14) {
            s = 1;
        }
        while (gPathNodes[s].respawnDir > 0) {
            s = gPathNodes[s].next;
            if (s > gPathNodeCount) {
                s = 0;
            }
        }
        while (gPathNodes[s].respawnDir < 0) {
            s = gPathNodes[s].prev;
            if (s < 0) {
                s = gPathNodeCount - 1;
            }
        }
        k = 0;
        v = b->segment;
        for (i = 0; i < 8; i++) {
            if (v == b->prevSegment) {
                b->lapAdjust += k;
                break;
            }
            if (v == gPathFinishNode) {
                k = -1;
            }
            v = gPathNodes[v].prev;
        }
        b->segment = s;
        if (gGame.mode == 6) {
            b->laneChange = 0;
            b->bike.pos.x = (f32)b->gridSlot * 2.178f;
        } else {
            r2 = (gPathNodes[s].widthNeg * gPathNodes[s].widthNeg + gPathNodes[s].widthPos * gPathNodes[s].widthPos) / 36.0f;
            half = __builtin_sqrtf(r2) * 0.5f;
            t = 0.0f;
            while (t <= gPathNodes[s].widthPos || -t <= gPathNodes[s].widthNeg) {
                if (!(t > 0.0f ? gPathNodes[s].widthPos < t : gPathNodes[s].widthNeg < -t)) {
                    b->bike.pos.x = gPathNodes[s].pos.x + gPathNodes[s].lateralX * t;
                    d = 10000.0f;
                    b->bike.pos.z = gPathNodes[s].pos.z + gPathNodes[s].lateralZ * t;
                    if (gGhostRace != 0) {
                        break;
                    }
                    for (p = gRiders; p < &gRiders[8]; p++) {
                        if (p == b || p->inactive != 0) {
                            continue;
                        }
                        dx = b->bike.pos.x - p->bike.pos.x;
                        dz = b->bike.pos.z - p->bike.pos.z;
                        d = dx * dx + dz * dz;
                        if (d < r2) {
                            break;
                        }
                    }
                    if (r2 <= d) {
                        break;
                    }
                }
                t += (t >= 0.0f ? half : 0.0f);
                t = -t;
            }
            if (d < r2) {
                b->bike.pos = gPathNodes[s].pos;
            }
        }
        quat_from_euler(b->bike.quat, 0.0f, 0.0f, math_atan2f(gPathNodes[s].lateralX, gPathNodes[s].lateralZ));
        break;
    }
    bike_place_on_ground(b);
    quat_to_mtx3f(&b->bike.right.x, b->bike.quat);
    b->stuntScore = 0;
    b->camMode = b->savedCamMode;
    z = 0.0f;
    b->frontWheel.suspLength = z;
    b->rearWheel.suspLength = z;
    b->bike.vel.z = z;
    b->bike.vel.y = z;
    b->bike.vel.x = z;
    b->bike.angVel.z = z;
    b->bike.angVel.y = z;
    b->bike.angVel.x = z;
    b->localAngVel.x = z;
    b->localAngVel.y = z;
    b->localAngVel.z = z;
    b->bike.unk_170 = 1.0f;
    if (b == gRiders) {
        b->flags &= ~0x10;
    }
    b->anim = -1;
    b->bike.mass = 250.0f;
    b->bike.inertia = 250.0f;
    *(Quat_2 *)b->drawQuat = *(Quat_2 *)b->bike.quat;
    if (s > 0) {
        b->bike.pos.y = gPathNodes[s].pos.y + 1.0f;
    }
    y = (f32)height_get_ground(b->bike.pos.x * 1000.0f, b->bike.pos.y * 1000.0f, b->bike.pos.z * 1000.0f) / 1000.0f + z + 0.5f;
    b->bike.pos.y = y - (gGame.mode == 6 ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    bike_update_wheel_contacts(b);
    bike_update_wheel_contacts(b);
    bike_update_suspension(b);
    b->heading = (s16)b->drawRot[2];
    for (i = 0; i < 11; i++) {
        b->bike.contacts[i] = b->bike.pos;
    }
    idx = b - gRiders;
    decal_end_track(idx);
    cam_set_frozen(idx, 0);
    collide_tree_clear_cache(b->bike.tree);
    bike_update_render_pose(b);
}

typedef struct {
    f32 a, b, c, d;
} S_2;

extern S_2 sBikeCrashContactPoints_2[];

void bike_set_contact_friction(f32 x)
{
    s32 i;

    for (i = 0; i < 11; i++) {
        sBikeCrashContactPoints_2[i].d = x;
    }
    sBikeCrashContactPoints_2[3].d = 0.1f;
    sBikeCrashContactPoints_2[4].d = 0.1f;
}

void bike_queue_crash(Rider *p, u8 v)
{
    p->pendingCrash = v;
    p->crashVel = p->bike.vel;
}

f32 body_time_to_ground(f32 *p)
{
    s32 iv;
    f32 d, t, a4, r;

    iv = height_get_ground((s32)(p[0] * 1000.0f), (s32)(p[1] * 1000.0f), (s32)(p[2] * 1000.0f));
    d = p[1] - (f32)iv / 1000.0f;
    if (d < 0.0f) {
        return 0.0f;
    }
    a4 = p[4];
    t = __builtin_sqrtf(a4 * a4 + d * 29.400002f) / 14.700001f;
    r = -(-a4 / 14.700001f) + t;
    if (!(r > 0.0f)) {
        r = t + -a4 / 14.700001f;
    }
    return r;
}

void rider_change_anim(Rider *o, s32 b) {
    o->prevAnim = (u8)o->anim;
    o->prevAnimFrame = o->animFrame;
    stunt_start(o, b);
}
