#include "game.h"

void bike_crash_begin(Bike_23 *b, s32 n)
{
    s16 rx, ry, rz;
    Vec3f v;
    Vec3f c;
    Vec3f w;
    s32 idx;
    s32 i;
    f32 s;
    Vec3f *src;

    if (b->active != 0) {
        return;
    }
    if (b->x6E8 == 0 && sCrashAnnounceFrame != gRaceFrame) {
        announce_crash(b - gRiders_46);
    }
    b->x644 = 0;
    b->x655 = 0;
    if (b == &gRiders_46[1]) {
        gEvent.flags &= ~0x1000;
    }
    b->x3FB = 0;
    b->x7A8 = 0;
    b->x5A8 = b->x5AA;
    if (b->x6E8 == 0) {
        flash_start(0x3C);
        tunes_queue_crowd(0);
    }
    if (gGame.mode == 6) {
        b->x626 = path_find_segment(&b->pos, b->x628, 10, 1);
    }
    idx = b - gRiders_46;
    b->x416 = 0;
    b->x744 = 0;
    b->x1C4 = 2;
    b->x36C = 1;
    b->x36D = (n - 0x19) / 3;
    bike_skel_update(b->x658, idx);
    rider_update_pose(b->x65C, idx);
    mtx3i_to_angles((s32 *)b->x65C->rot, (u16 *)&rx, (u16 *)&ry, (u16 *)&rz);
    quat_from_euler(b->x1FC, (f32)rx * 3.1415927f * 3.0517578e-05f,
                  (f32)ry * 3.1415927f * 3.0517578e-05f,
                  (f32)rz * 3.1415927f * 3.0517578e-05f);
    b->active = 1;
    b->x624 = 0;
    b->x3D1 = b->x3D0;
    b->x3CC = b->x3AC;
    b->x428 = b->x420 + 5.0f;
    if (b->x428 > 32.0f) {
        b->x428 = 32.0f;
    }
    b->x420 = 0.0f;
    for (i = 0; i < 11; i++) {
        vec3_mul_mtx3f(sBikeCrashContactPoints[i], (f32 *)b->mtx, &v.x);
        b->pts[i].x = v.x + b->pos.x;
        b->pts[i].y = v.y + b->pos.y;
        b->pts[i].z = v.z + b->pos.z;
    }
    if (n == 0x22) {
        s = 2.0f;
        goto adj;
    }
    if (n == 0x1F) {
        s = -2.0f;
    adj:
        if (b->x420 < 5.0f) {
            s *= 0.25f;
        }
        b->p88.x += s * b->mtx[2].x;
        b->p88.y += s * b->mtx[2].y;
        b->p88.z += s * b->mtx[2].z;
    }
    b->p1CC.x = (f32)(b->x65C->x34 + b->p0[0]) / 1000.0f;
    b->p1CC.y = (f32)(b->x65C->x38 + b->p0[1]) / 1000.0f;
    b->p1CC.z = (f32)(b->x65C->x3C + b->p0[2]) / 1000.0f;
    s = __builtin_sqrtf(b->vel.x * b->vel.x + b->vel.y * b->vel.y + b->vel.z * b->vel.z) / 5.0f;
    if (s < 1.0f) {
        s = 1.0f;
    }
    if (s > 5.0f) {
        s = 5.0f;
    }
    src = &b->vel;
    if (b->x45 < 0) {
        src = &b->p43C;
    }
    vec3_cross(&b->p88, &b->mtx[1], &c);
    b->p1D8.x = src->x + c.x + b->mtx[1].x * s;
    b->p1D8.y = src->y + c.y + b->mtx[1].y * s;
    b->p1D8.z = src->z + c.z + b->mtx[1].z * s;
    b->p20C = b->p88;
    if (n == 0x19) {
        b->p20C.x -= b->mtx[0].x + b->mtx[0].x;
        b->p20C.y -= b->mtx[0].y + b->mtx[0].y;
        b->p20C.z -= b->mtx[0].z + b->mtx[0].z;
    }
    b->x3AC = 0;
    b->x349 |= 1;
    b->x1C5 |= 1;
    for (i = 0; i < 8; i++) {
        vec3_mul_mtx3f(sRiderCrashContactPoints[b->x36D][i], (f32 *)b->mtx, &w.x);
        b->pts2[i].x = w.x + b->p1CC.x;
        b->pts2[i].y = w.y + b->p1CC.y;
        b->pts2[i].z = w.z + b->p1CC.z;
    }
    b->x1B8 = 1.0f;
    b->x33C = 1.0f;
    stunt_start(b, n);
    collide_tree_clear_cache(b->x248);
}

#define DAMPR(x) { f32 r, k, v, w; k = (gGame.mode == 6) ? gBikeTuneSpringRate * 20000.0f : 15000.0f; v = (x); w = v; if (gGame.mode == 6) r = v - v * (k / ((gBikeTuneDamping * 2000.0f) / gPhysDeltaTime)); else r = w - w * (k / (2200.0f / gPhysDeltaTime)); (x) = r; }
#define CLAMP(b, p) { m = __builtin_sqrtf(b->p.vx * b->p.vx + b->p.vy * b->p.vy + b->p.vz * b->p.vz);     if (b->f428 < m) { b->p.vx *= b->f428 / m; b->p.vy *= b->f428 / m; b->p.vz *= b->f428 / m; } }

typedef struct { u8 pad[0x28]; s32 flags; u8 pad2[0x40 - 0x2C]; } Track_9;
extern Track_9 gSurfaceTable_7[];

static inline f32 airtime(Body_2 *p, f32 g)
{
    f32 h, t, s, a, v;
    h = p->y - (f32)height_get_ground(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f;
    if (h < 0.0f) return 0.0f;
    v = p->vy;
    s = __builtin_sqrtf(v * v + h * 29.400002f) / g;
    a = -v / g;
    t = -a + s;
    if (t > 0.0f) return t;
    return s + a;
}

static inline void lane(Bike_24 *b, s32 n) { s32 t = b->f3D0 + n; b->f3D1 = b->f3D0; b->f3CC = b->f3AC; stunt_start(b, t); }

void bike_crash_update(Bike_24 *b)
{
    s32 vol, id, n, ch;
    s32 unused[8];
    s32 tmp[2];
    f32 g, dt, m;

    stunt_animate((s32)b, 0);
    b->f624++;
    DAMPR(b->f51C);
    DAMPR(b->f478);
    b->f5A0 *= 0.9f;
    if (b->f6E8 == 0 && (gGame.numPlayers == 1 || gGame.mode == 8)) {
        gCrashSlowMotion = 1;
    }
    height_get_ground(b->b0.x * 1000.0f, b->b0.y * 1000.0f, b->b0.z * 1000.0f);
    if ((gSurfaceTable_7[(gGroundSurface >> 4) & 0xF].flags & 0x800) && b->f6E8 == 0) {
        cam_start_fall_view(b);
    }
    if (b->f1C5 & 1) {
        b->b0.f20.i = 0;
        b->b0.i18 = 0;
        b->b0.f1C = -14.700001f;
        b->f380 = b->f384 = b->b0.f20.f;
        phys_body_step(&b->b0, sBikeCrashContactPoints, 0xB, 1);
        CLAMP(b, b0);
        if (b->f380 > 0.5f) {
            vol = b->f380 * 128.0f;
            vol += 0x30;
            if (vol >= 0x100) vol = 0xFF;
            if (vol > 0x80 || gRaceFrame - sImpactSfxFrame >= 0x1F) {
                sImpactSfxFrame = gRaceFrame;
                if (gRaceFrame & 1) {
                    snd_play_rider_sfx(4, b - gRiders_47, vol);
                    snd_play_rider_sfx(4, b - gRiders_47, vol);
                } else {
                    snd_play_rider_sfx(8, b - gRiders_47, vol);
                    snd_play_rider_sfx(8, b - gRiders_47, vol);
                }
            }
        }
        b->b0.vy -= gPhysDeltaTime * 14.700001f;
        b->b0.r40 -= b->b0.r40 * gPhysDeltaTime * 0.5f;
        b->b0.r44 -= b->b0.r44 * gPhysDeltaTime * 0.5f;
        b->b0.r48 -= b->b0.r48 * gPhysDeltaTime * 0.5f;
    } else {
        b->f624 += 20;
    }
    if (b->f349 & 1) {
        b->b1.i18 = 0;
        b->b1.f20.i = 0;
        b->b1.f1C = -14.700001f;
        phys_body_step(&b->b1, sRiderCrashContactPoints[b->f36D], 8, 1);
        CLAMP(b, b1);
        dt = gPhysDeltaTime;
        g = 14.700001f;
        b->b1.vy -= dt * g;
        b->b1.r40 -= b->b1.r40 * gPhysDeltaTime;
        b->b1.r44 -= b->b1.r44 * gPhysDeltaTime;
        b->b1.r48 -= b->b1.r48 * gPhysDeltaTime;
        switch (b->f36C) {
        case 1:
            if (rider_anim_get_length(b->f3D0, tmp) - (b->f3AC >> 8) < 2) {
                n = (airtime(&b->b1, g) < 0.5f) ? 2 : 1;
                lane(b, n);
                b->f36C += n;
            }
            break;
        case 2:
            if (airtime(&b->b1, g) < 0.5f) {
                lane(b, 1);
                b->f36C = 3;
            }
            break;
        case 3:
            break;
        }
    }
    ch = 0x78;
    if (gCrashSlowMotion != 0 && b->f6E8 == 0) ch = 0xF0;
    if (gGame.mode == 7) ch = 0x12C;
    if (ch < (s16)b->f624) bike_respawn(b);
}

#define HEIGHT(v) ((f32)height_get_ground((s32)((v)->x * 1000.0f), (s32)((v)->y * 1000.0f), (s32)((v)->z * 1000.0f)) / 1000.0f)

struct Pt_4 {
    Vec3f v;
    f32 w;
};

typedef struct { u8 pad[0x2C]; s32 f2C; } Game_16;

extern Game_16 *gCurTrack_25;

s32 phys_body_step(Body_3 *b, Pt_4 *t, s32 n, u8 m)
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
    Bike_25 *o;
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
    if ((gDrawFlags & 0x200) && b == &D_800E3808) {
        hx = 4.0f;
        lx = -4.0f;
        hz = hx;
        lz = lx;
    }
    height_gather_walls((s32)((b->pos.x + lx) * 1000.0f), (s32)((b->pos.z + lz) * 1000.0f),
                  (s32)((b->pos.x + hx) * 1000.0f), (s32)((b->pos.z + hz) * 1000.0f));
    if (b->timer != 0) {
        b->timer -= D_800E76C7;
        if (b->timer < 0) {
            b->timer = 0;
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
        w.x = b->angvel.x + b->angacc.x * dt * 0.5f;
        w.y = b->angvel.y + b->angacc.y * dt * 0.5f;
        w.z = b->angvel.z + b->angacc.z * dt * 0.5f;
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
        quat_mul(q.q, dq.q, b->q.q);
        quat_to_mtx3f(b->m, q.q);
        for (i = 0; i < n; i++) {
            vec3_mul_mtx3f(&t[i].v.x, b->m, &rot[i].x);
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
                    vec3_cross(&b->angvel, &w, &dv);
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
                    if (phys_check_resting(&b->vel.x, &b->angvel.x, depth, rot, n, b->f170) < 0.0f) {
                        b->flags &= ~1;
                        return 0;
                    }
                    p.y += depth[k] + 0.01f;
                    dv.x = (world[k].x - b->pts[k].x) / dt;
                    dv.y = (world[k].y - b->pts[k].y) / dt;
                    dv.z = (world[k].z - b->pts[k].z) / dt;
                    gk = t[k].w;
                    if (b->owner != NULL && b->owner->f44 != 0) {
                        gk /= b->owner->f7B4;
                    }
                    phys_contact_impulse(b, &gContactNormal_2, &rot[k], gk, &dv, 0.1f);
                    e = dv.x * dv.x + dv.y * dv.y + dv.z * dv.z;
                    o = b->owner;
                    if (b->type == 2 && o->f6E8 == 0 && e > 4.0f && t[k].w > 0.15f) {
                        if (o != NULL) {
                            if (sImpactSlideSpeed > o->f384) {
                                o->f384 = sImpactSlideSpeed;
                            }
                            if (sImpactNormalSpeed > o->f380) {
                                o->f380 = sImpactNormalSpeed;
                            }
                            if (gRiderAlpha[o - gRiders_48] == 0xFF) {
                                particles_start_dust_cloud(&world[k], o - gRiders_48);
                            }
                        }
                        if (o->f6E8 == 0 && gRaceFrame - o->f774 >= 0x10) {
                            o->f768 = e;
                            o->f774 = gRaceFrame;
                            o->f764 = 0xFF;
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
            if (height_raycast_walls(&b->pts[i], &world[i], &gContactNormal_2, &tmax, 0)) {
                dv.x = (world[i].x - b->pts[i].x) / dt;
                dv.y = (world[i].y - b->pts[i].y) / dt;
                k = i;
                dv.z = (world[i].z - b->pts[i].z) / dt;
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
                if (gContactNormal_2.y > 0.75f && (k > 5 || o->f744 > 60)) {
                    o->f768 = 0x200;
                    o->f774 = gRaceFrame;
                    o->f764 = 0xFF;
                    o->f45 = (t[k].v.z > 0.0f) ? -0x19 : -0x1C;
                    o->f43C = o->b0.vel;
                } else if (a < -15.0f) {
                    if (gCurTrack_25->f2C == 0 && gGame.mode != 10 && gGame.mode != 2) {
                        sCrashAnnounceFrame = gRaceFrame;
                        if ((f32)height_get_ground((s32)(o->b0.pos.x * 1000.0f), (s32)(o->b0.pos.y * 1000.0f), (s32)(o->b0.pos.z * 1000.0f)) / 1000.0f - o->b0.pos.y > 2.0f) {
                            announce_crash(o - gRiders_48);
                        } else {
                            announce_crash_landing(o - gRiders_48);
                        }
                    }
                    o->f768 = 0x200;
                    o->f774 = gRaceFrame;
                    o->f764 = 0xFF;
                    o->f45 = -0x1C;
                    o->f43C = o->b0.vel;
                }
                if (o->f420 < 10.0f) {
                    x = 0.2f;
                } else {
                    x = o->f7D4;
                }
            }
            phys_contact_impulse(b, &gContactNormal_2, &rot[k], x, &dv, 0.05f);
            if (b->type < 2) {
                Bike_25 *o = b->owner;
                if ((o->f416 != 0 || (o->f3D0 > 0 && o->f3D0 < 16)) && o->f744 <= 0) {
                    o->f45 = -0x19;
                    o->f43C = o->b0.vel;
                }
            }
            {
            f32 ln = __builtin_sqrtf(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z);
            if (ln > 2.0f && hit) {
                if (t[k].w > 0.15f) {
                    if (b->type < 2) {
                        b->owner->f354 = t[k].v;
                        b->owner->f350 = 10;
                        snd_trigger_impact_2(b->owner - gRiders_48, (s32)(ln * 16.0f));
                    } else if (b->type == 2) {
                        Bike_25 *o = b->owner;
                        if (sImpactSlideSpeed > o->f384) {
                            o->f384 = sImpactSlideSpeed;
                        }
                        if (sImpactNormalSpeed > o->f380) {
                            o->f380 = sImpactNormalSpeed;
                        }
                    }
                }
            }
            }
            dt *= 0.8f;
            b->timer = 15;
        } else {
            f32 ln;
            savepos = b->pos;
            saveq = b->q;
            b->pos = p;
            b->q = q;
            if (b->type < 2 && m == 0) {
                o = b->owner;
                bike_update_wheel_contacts(o);
                if (o->f564.y < 0.5f && o->f51C <= o->f520 + 0.01f) {
                    if (o->f744 > 60 || vec3_dot(&o->f564.x, &o->b0.m[3]) < 0.3f) {
                        o->f45 = -0x1C;
                        o->f43C = o->b0.vel;
                    } else {
                        d.x = o->f538.x - p.x;
                        d.y = o->f538.y - p.y;
                        d.z = o->f538.z - p.z;
                        vec3_cross(&d, &b->angvel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->f564.x * o->f564.x + o->f564.z * o->f564.z;
                        ln = __builtin_sqrtf(ln);
                        w.x = o->f564.x / ln;
                        w.y = 0.0f;
                        w.z = o->f564.z / ln;
                        f = vec3_dot(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        dt *= 0.5f;
                        phys_contact_impulse(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        b->q = saveq;
                        quat_to_mtx3f(b->m, b->q.q);
                        bike_update_wheel_contacts(o);
                        continue;
                    }
                }
                if (o->f4C0.y < 0.5f && o->f478 <= o->f47C + 0.01f) {
                    if (o->f744 > 60 || vec3_dot(&o->f4C0.x, &o->b0.m[3]) < 0.3f) {
                        o->f45 = -0x1C;
                        o->f43C = o->b0.vel;
                    } else {
                        d.x = o->f494.x - p.x;
                        d.y = o->f494.y - p.y;
                        d.z = o->f494.z - p.z;
                        vec3_cross(&d, &b->angvel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->f4C0.x * o->f4C0.x + o->f4C0.z * o->f4C0.z;
                        w.x = o->f4C0.x / ln;
                        w.y = 0.0f;
                        w.z = o->f4C0.z / ln;
                        f = vec3_dot(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        phys_contact_impulse(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        b->q = saveq;
                        w.x *= 0.125f;
                        w.y *= 0.125f;
                        w.z *= 0.125f;
                        b->vel.x = w.x + b->vel.x;
                        b->vel.y = w.y + b->vel.y;
                        b->vel.z = w.z + b->vel.z;
                        dt *= 0.5f;
                        quat_to_mtx3f(b->m, b->q.q);
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
        b->pts[i] = world[i];
    }
    if (b->type != 4) {
        o = b->owner;
        if (o == NULL || o->f41D != 0) {
            return 0;
        }
        if (o->f44 != 0) {
            if (b->f7C != 0) {
                collide_bodies(b, &o->b0);
                collide_bodies(b, &o->b1);
            }
            return 0;
        }
    }
    if (b->f7C != 0) {
        for (i = 0; i < 8; i++) {
            if (gRiders_48[i].f7A5 == 0 && gRiders_48[i].f41D == 0 && gRiders_48[i].f44 == 0) {
                collide_bodies(b, &gRiders_48[i].b0);
            }
        }
        if (gGame.mode == 2) {
            collide_bodies(b, &gSoccerBall);
        }
    }
    return 0;
}

struct Body_4 {
    u8 pad0[0xC];
    Vec3f vel;
    Vec3f f18;
    u8 pad24[0x40 - 0x24];
    Vec3f rot;
    Vec3f f4C;
    u8 pad58[0x174 - 0x58];
    f32 mass;
    f32 inertia;
};

void phys_contact_impulse(Body_4 *b, Vec3f *n, Vec3f *p, f32 maxr, Vec3f *c, f32 fr)
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
        vec3_cross(&b->rot, p, c);
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
    b->rot.x += B.x / b->inertia;
    b->rot.y += B.y / b->inertia;
    b->rot.z += B.z / b->inertia;
    b->f18.z = 0.0f;
    b->f18.y = 0.0f;
    b->f18.x = 0.0f;
    b->f4C.z = 0.0f;
    b->f4C.y = 0.0f;
    b->f4C.x = 0.0f;
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

void bike_respawn(Bike_26 *b) {
    s32 s;
    s32 v, i, k, idx;
    PathNode *n;
    Game *g;
    Bike_26 *p;
    f32 z, r2, half, t, d, dx, dz, y;

    if (b->f6E8 == 2) {
        b->f7A5 = 1;
        return;
    }
    b->f36C = 0;
    b->f748 = 0;
    b->f744 = 0;
    b->f5CD = 0;
    b->f416 = 0;
    b->f45 = 0;
    b->f624 = 0;
    b->f44 = 0;
    b->f7AC = 0;
    b->f648 = 0;
    b->f654 = 0;
    b->f3D1 = -1;
    b->f3D3 = -1;
    b->f3FB = 0;
    b->f7DC = 0;
    if (b->f3FF == 0) {
        b->f620 = 40;
    }
    b->f621 = 16;
    b->f41D = 1;
    b->f44C = 1.0f;
    b->f1C4 = b->f6E8 != 0;
    if (b->f6E8 == 0) {
        gCrashSlowMotion = 0;
        if (gCamTargetOverride == -1) {
            gCamLastModes[b - gRiders_49] = -1;
        } else if (gCamTargetOverride == b - gRiders_49) {
            gCamLastModes[0] = -1;
        }
    }
    s = -1;
    g = &gGame;
    switch (g->mode) {
    case 2:
        if ((gEvent.flags & 0xF) || (gEvent.state != 7 && gEvent.state != 11 && gEvent.state != 9)) {
        b->f620 = 0;
        b->f621 = 0;
        if (g->numPlayers == 4) {
            switch (b - gRiders_49) {
            case 0:
                b->pos.x = -1.0f;
                b->pos.z = -10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 0.0f);
                break;
            case 1:
                b->pos.x = 1.0f;
                b->pos.z = -10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 0.0f);
                break;
            case 2:
                b->pos.x = 1.0f;
                b->pos.z = 10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 3.1415927f);
                break;
            case 3:
                b->pos.x = -1.0f;
                b->pos.z = 10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 3.1415927f);
                break;
            }
        } else {
            switch (b - gRiders_49) {
            case 0:
                b->pos.x = 0.015f;
                b->pos.z = -10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 0.00958738f);
                break;
            case 1:
                b->pos.x = 0.01f;
                b->pos.z = 10.0f;
                quat_from_euler(&b->q.x, 0.0f, 0.0f, 3.15118f);
                break;
            }
        }
        } else {
            height_push_off_wall(&b->pos.x);
            quat_from_euler(&b->q.x, 0.0f, 0.0f, math_atan2f(-(b->pos.z - gSoccerBall.z), b->pos.x - gSoccerBall.x));
        }
        b->f41D = 0;
        break;
    case 4:
    case 5:
        height_push_out_of_desert_objs(&b->pos.x);
        quat_from_euler(&b->q.x, 0.0f, 0.0f, (f32)b->f34 * 3.1415927f * 3.0517578e-05f);
        break;
    case 10:
        height_push_off_wall(&b->pos.x);
        quat_from_euler(&b->q.x, 0.0f, 0.0f, (f32)b->f34 * 3.1415927f * 3.0517578e-05f);
        break;
    default:
        s = b->f626;
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
        v = b->f628;
        for (i = 0; i < 8; i++) {
            if (v == b->f626) {
                b->f6ED += k;
                break;
            }
            if (v == gPathFinishNode) {
                k = -1;
            }
            v = gPathNodes[v].prev;
        }
        b->f628 = s;
        if (gGame.mode == 6) {
            b->f7AA = 0;
            b->pos.x = (f32)b->f7A9 * 2.178f;
        } else {
            r2 = (gPathNodes[s].widthNeg * gPathNodes[s].widthNeg + gPathNodes[s].widthPos * gPathNodes[s].widthPos) / 36.0f;
            half = __builtin_sqrtf(r2) * 0.5f;
            t = 0.0f;
            while (t <= gPathNodes[s].widthPos || -t <= gPathNodes[s].widthNeg) {
                if (!(t > 0.0f ? gPathNodes[s].widthPos < t : gPathNodes[s].widthNeg < -t)) {
                    b->pos.x = gPathNodes[s].pos.x + gPathNodes[s].lateralX * t;
                    d = 10000.0f;
                    b->pos.z = gPathNodes[s].pos.z + gPathNodes[s].lateralZ * t;
                    if (gGhostRace != 0) {
                        break;
                    }
                    for (p = gRiders_49; p < &gRiders_49[8]; p++) {
                        if (p == b || p->f7A5 != 0) {
                            continue;
                        }
                        dx = b->pos.x - p->pos.x;
                        dz = b->pos.z - p->pos.z;
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
                b->pos = gPathNodes[s].pos;
            }
        }
        quat_from_euler(&b->q.x, 0.0f, 0.0f, math_atan2f(gPathNodes[s].lateralX, gPathNodes[s].lateralZ));
        break;
    }
    bike_place_on_ground(b);
    quat_to_mtx3f(b->mtx, &b->q.x);
    b->f3C4 = 0;
    b->f5AA = b->f5A8;
    z = 0.0f;
    b->f51C = z;
    b->f478 = z;
    b->f5C = z;
    b->f58 = z;
    b->f54 = z;
    b->f90 = z;
    b->f8C = z;
    b->f88 = z;
    b->f430 = z;
    b->f434 = z;
    b->f438 = z;
    b->f1B8 = 1.0f;
    if (b == gRiders_49) {
        b->f690 &= ~0x10;
    }
    b->f3D0 = -1;
    b->f1BC = 250.0f;
    b->f1C0 = 250.0f;
    b->q370 = b->q;
    if (s > 0) {
        b->pos.y = gPathNodes[s].pos.y + 1.0f;
    }
    y = (f32)height_get_ground(b->pos.x * 1000.0f, b->pos.y * 1000.0f, b->pos.z * 1000.0f) / 1000.0f + z + 0.5f;
    b->pos.y = y - (gGame.mode == 6 ? 3675.0002f / (2.0f * (gBikeTuneSpringRate * 20000.0f)) : 0.12250001f);
    bike_update_wheel_contacts(b);
    bike_update_wheel_contacts(b);
    bike_update_suspension(b);
    b->f34 = b->f22;
    for (i = 0; i < 11; i++) {
        b->hist[i] = b->pos;
    }
    idx = b - gRiders_49;
    decal_end_track(idx);
    cam_set_frozen(idx, 0);
    collide_tree_clear_cache(b->fC4);
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

void bike_queue_crash(u8 *p, u8 v)
{
    p[0x45] = v;
    *(T3 *)(p + 0x43C) = *(T3 *)(p + 0x54);
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

struct Obj_35 {
    u8 pad0[0x3AC];
    s32 w3ac;
    u8 pad1[0x3CC - 0x3B0];
    s32 w3cc;
    u8 b3d0;
    u8 b3d1;
};

void rider_change_anim(Obj_35 *o, s32 b) {
    o->b3d1 = o->b3d0;
    o->w3cc = o->w3ac;
    stunt_start(o, b);
}
