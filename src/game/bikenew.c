#include "game.h"

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x50 - 0x30];
    u32 f50;
} Opt_3;

typedef struct {
    u8 pad0[0xC];
    u16 fC;
    u8 padE[2];
} Bk;

extern Bk gCamModes[];
extern Opt_3 *gCurTrack_17;

void bikes_init(void) {
    Vec3f unused[4];
    s32 i;
    s32 k;
    s32 j;
    s32 a;
    s32 c;
    s32 cc;
    s32 r;
    s32 n;
    s32 idx;
    f32 zero;
    f32 z;
    f32 sc;
    u16 ang;
    s32 ratio;
    Rider *p;
    Vec3f *v;

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
    zero = 0.0f;
    gAiDebugTargetPos.z = zero;
    gAiDebugTargetPos.y = zero;
    gAiDebugTargetPos.x = zero;
    gAiDebugTargetDir.z = zero;
    gAiDebugTargetDir.y = zero;
    gAiDebugTargetDir.x = zero;
    collide_init_node_pool();
    race_load_extra_rider_spawns(gResourceSet);
    r250_init();
    D_800E7704 = 1;
    gRaceFrame = 0;
    for (; i < 8; i++) {
        z = 0.0f;
        gRiderAlpha[i] = 0xFF;
        gRiders[i].unk_778 = -60;
        if (gGhostRace != 0 || (gRiders[i].isCpu == 0 && (gCheatFlags & 0x100))) {
            gRiders[i].noCollide = 1;
        }
        gRiders[i].bikeSkel = mem_alloc_dbg(0x500, 0, "bikenew.c", 233);
        gRiders[i].riderSkel = mem_alloc_dbg(0x780, 0, "bikenew.c", 234);
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
        gRiders[i].trickQuat[1] = z;
        gRiders[i].trickQuat[2] = z;
        gRiders[i].trickQuat[3] = z;
        gRiders[i].trickOffset.x = z;
        gRiders[i].trickOffset.y = z;
        gRiders[i].trickOffset.z = z;
        gRiders[i].anim = -1;
        gRiders[i].prevAnim = -1;
        gRiders[i].trickQuat[0] = 1.0f;
        gRiders[i].bike.vel.x = gRiders[i].bike.fwd.x * z;
        gRiders[i].bike.vel.y = gRiders[i].bike.fwd.y * gRiders[i].speed;
        gRiders[i].bike.vel.z = gRiders[i].bike.fwd.z * gRiders[i].speed;
        r = random_byte();
        gRiders[i].bike.unk_170 = z;
        gRiders[i].bike.flags = 1;
        gRiders[i].body.type = 3;
        gRiders[i].body.flags = 0;
        gRiders[i].bike.inertia = 250.0f;
        gRiders[i].bike.type = gRiders[i].isCpu != 0;
        gRiders[i].body.mass = 90.0f;
        gRiders[i].body.inertia = 45.0f;
        gRiders[i].headTimer = r % 150;
        if (gGame.mode == 2) {
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
        case 7:
            gRiders[i].camMode = 6;
            break;
        case 2:
            gRiders[i].camMode = 0;
            break;
        case 6:
            if (i < gGame.numPlayers) {
                c = gGame.padCamMode[gGame.playerPad[i]];
            } else {
                c = 2;
            }
            if (c != 4 && c != 2) {
                c = 2;
            }
            gRiders[i].camMode = c;
            if (i < gGame.numPlayers) {
                gGame.padCamMode[gGame.playerPad[i]] = c;
            }
            break;
        default:
            if (i < gGame.numPlayers) {
                c = gGame.padCamMode[gGame.playerPad[i]];
            } else {
                c = 2;
            }
            if (gCamModes[c].fC & 8) {
                c = 2;
            }
            gRiders[i].camMode = c;
            if (i < gGame.numPlayers) {
                gGame.padCamMode[gGame.playerPad[i]] = c;
            }
            break;
        }
        mem_fill_2(gCamLastModes, -1, 0x18);
        gRiders[i].unk_412 = 0x3000;
        gRiders[i].savedCamMode = (u16)gRiders[i].camMode;
    }
    i = 0;
    if (gEvent.numRiders > 0) {
        do {
            p = &gRiders[i];
            if (i < 4) {
                gCamTargets[i] = i;
            }
            if (!(gGame.flags & 0x81) && gCreditsMode == 0 && i < gGame.numPlayers) {
                p->isCpu = 0;
            } else {
                p->isCpu = 1;
            }
            bike_apply_rider_stats(p, gGame.riders[i]);
            p->timeScale = 1.0f;
            p->inactive = 0;
            p->speed = 0.0f;
            switch (gGame.mode) {
            case 2:
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
            case 5:
                p->bike.pos.x = sStartLineOffsets[i].x;
                p->bike.pos.z = sStartLineOffsets[i].z;
                quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, 0.0f);
                break;
            default:
                if ((gGame.flags & 0x11) == 0x11) {
                    idx = i * gPathNodeCount / 6;
                } else if (gGame.mode == 13 || gGame.mode == 14
                           || (gGame.mode == 0 && (gCurTrack_17->f50 & 2))) {
                    idx = 1;
                } else if (gGame.mode == 9) {
                    idx = gTutorialLessons[gSeason->round].startNode;
                } else {
                    idx = gPathStartNode;
                }
                n = gEvent.numRiders;
                if (gGhostRace != 0 && n == 1) {
                    n = 2;
                }
                if (gGame.flags & 0x90) {
                    zero = 0.0f;
                } else {
                    sc = 1.5f;
                    zero = (gPathNodes[idx].widthPos - gPathNodes[idx].widthNeg) * 0.5f;
                    cc = gGridOrder_2[i];
                    if (gCurTrack_17->f50 & 2) {
                        sc = 1.1f;
                    }
                    zero += sc * ((f32)cc - (f32)(n - 1) * 0.5f);
                    p->race.place = cc;
                    p->race.prevPlace = cc;
                    p->race.placeAnim = 0x100;
                }
                p->segment = idx;
                if (gGame.mode == 7) {
                    p->bestSegment = idx;
                }
                k = idx;
                if (gGame.mode == 9 && gSeason->round == 5) {
                    k = idx + 1;
                }
                math_to_polar(gPathNodes[k].lateralZ * 1024.0f, gPathNodes[k].lateralX * 1024.0f, (s16 *)&ang, &ratio);
                quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                if (gCurTrack_17->f2C == 0 && !(gGame.flags & 0x10)) {
                    ang = (ang + 0x1000) & ~0x3FFF;
                    quat_from_euler(gRiders[i].bike.quat, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                }
                if (gGame.mode != 6) {
                    a = (s16)ang >> 4;
                    p->bike.pos.x = gPathNodes[idx].pos.x + (f32)gCosTable[a & 0xFFF] * zero * 6.1035156e-05f
                             - (f32)gCosTable[(a - 0x400) & 0xFFF] * 1.5258789e-05f;
                    p->bike.pos.z = gPathNodes[idx].pos.z + (f32)gCosTable[(a - 0x400) & 0xFFF] * zero * 6.1035156e-05f
                             + (f32)gCosTable[a & 0xFFF] * 1.5258789e-05f;
                    p->laneOffset = zero;
                } else {
                    p->bike.pos.z = gPathNodes[idx].pos.z;
                    p->bike.pos.x = (f32)i * 2.178f;
                    p->laneOffset = (f32)-i * 2.178f;
                }
                p->gridSlot = i;
                break;
            }
            bike_place_on_ground(p);
            bike_update_suspension(p);
            bike_update_render_pose(p);
            p->heading = (u16)p->drawRot[2];
            for (j = 0; j < 11; j++) {
                gRiders[i].bike.contacts[j] = gRiders[i].bike.pos;
            }
            i++;
        } while (i < gEvent.numRiders);
    }
    race_init_standings(8);
    bike_set_contact_friction(0.5f);
    sBikeMaxReverseSpeed = -5.0f;
    race_init_ai_skills(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    switch (gGame.mode) {
    case 2:
        sBikeMaxReverseSpeed = -20.0f;
        soccer_ball_reset(1);
        break;
    case 5:
        desert_init();
        break;
    case 7:
        bike_set_contact_friction(0.3f);
        break;
    case 6:
        sBikeMaxReverseSpeed = 0.0f;
        bike_set_contact_friction(0.6f);
        excite3d_init();
        break;
    }
    sBikeTuneMenu = 1;
}

typedef struct {
    f32 f[10];
} Rec28;

extern Rec28 sRiderStatsTable[];

void bike_apply_rider_stats(Rider *a0, s32 a1)
{
    if (a0->isCpu == 0) {
        a0->bike.mass = 250.0f;
        a0->landing = sRiderStatsTable[a1].f[0];
        a0->handling = sRiderStatsTable[a1].f[1] * (sBikeTune[14] * sBikeTune[5]) * 32.0f * a0->turnRadius;
        a0->airTurn = sRiderStatsTable[a1].f[2] * sBikeTune[12] * 3.1415927f * 0.00390625f;
        a0->airFloat = sRiderStatsTable[a1].f[3] / 48.0f;
        a0->turnRadius = sRiderStatsTable[a1].f[4] * 0.57f;
        a0->turbo = sRiderStatsTable[a1].f[5];
        a0->weight = sRiderStatsTable[a1].f[6];
        a0->unk_7D4 = sRiderStatsTable[a1].f[7];
        a0->power = sRiderStatsTable[a1].f[8];
        return;
    }
    a0->bike.mass = 250.0f;
    a0->unk_7D4 = 0.3f;
    a0->landing = 1.0f;
    a0->turbo = 1.0f;
    {
        f32 k57 = 0.57f;
        f32 t = sBikeTune[12] * 3.1415927f * 0.00390625f;
        a0->turnRadius = k57;
        a0->airFloat = 0.020833334f;
        a0->airTurn = t;
        a0->handling = sBikeTune[14] * sBikeTune[5] * 32.0f * k57;
    }
    if (gGame.mode == 0xF) {
        a0->power = 0.7f;
    } else {
        a0->power = 1.0f;
    }
    a0->weight = sRiderStatsTable[a1].f[6];
}

f32 bike_wheel_ground_bisect(Rider *o, f32 *p, f32 p3)
{
    f32 t, step, d, b, tb, h, diff, sc, sc3, sc4, lim;
    f32 v[3];
    f32 x, y, y2, tb2, tz, zp;
    f32 *vp;
    u8 *tbl;
    u8 *tbl2;
    f32 *vq;
    s32 n, m, k;

    t = 0.0f;
    step = -1.0f;
    if ((s32)o < step) n = 1; else n = 2;
    if (o) n = 1; else if (p) n = 2; else n = 3;
    h = o->bike.pos.y;
    v[1] = h;
    if (p[0] < h) n = 1; else n = 3;
    sc = 1000.0f;
    n = height_get_ground((s32)(p[0] * sc), (s32)(p[1] * sc), (s32)(p[2] * sc));
    d = (f32)n / sc - p[1];
    if (0.0f < d) step = 1.0f;
    vp = v;
    tbl = (u8 *)&gGame;
    lim = 10000.0f;
    sc3 = sc;
    do {
        t += step;
        x = p[0] + t * o->bike.up.x;
        v[0] = x;
        tb = t * o->bike.up.y;
        y = p[1];
        v[2] = p[2] + t * o->bike.up.z;
        do { } while (0);
        b = y + tb;
        if (tbl) n = 1; else n = 2;
        if (x < b) n = 1; else n = 2;
        n = height_get_ground((s32)(vp[0] * sc3), (s32)(vp[1] * sc3), (s32)(vp[2] * sc3));
        h = (f32)n / sc3;
        if (lim < h) n = 3; else n = 4;
        if (step < h) n = 18; else n = 19;
        if (h < step) n = 20; else n = 21;
        if (d < h) n = 22; else n = 23;
        if (gGroundFound == 0) {
            o->pendingCrash = -0x1c;
            return -2.0f;
        }
        diff = h - b;
    } while (0.0f < d * diff);
    if (0.0f < d) step = -step;
    lim = 0.0f;
    tbl2 = (u8 *)&gGame;
    d = 10000.0f;
    step *= 0.5f;
    sc4 = 1000.0f;
    do {
        vq = v;
        if (lim < diff) t -= step;
        else t += step;
        x = p[0] + t * o->bike.up.x;
        v[0] = x;
        step *= 0.5f;
        tb2 = t * o->bike.up.y;
        tz = t * o->bike.up.z;
        zp = p[2];
        do { } while (0);
        vq = v;
        y2 = p[1];
        v[2] = zp + tz;
        do { } while (0);
        b = y2 + tb2;
        k = (s32)t;
        m = (s32)step;
        if (k == m) h = t;
        if (tbl2) { h = b; n = 1; } else { h = x; n = 2; }
        n = height_get_ground((s32)(vq[0] * sc4), (s32)(vq[1] * sc4), (s32)(vq[2] * sc4));
        h = (f32)n / sc4;
        if (d < h) n = 3; else n = 4;
        diff = h - b;
        if (lim < diff) {
            if (!(p3 < diff)) return t;
        } else {
            if (!(p3 < -diff)) return t;
        }
    } while (step < -0.000625f);
    return t;
}

void bike_wheel_find_ground(Rider *b, Wheel *w, f32 dt, u8 flag)
{
    f32 P[3];
    Vec3f Q;
    f32 A[4];
    f32 L;
    f32 t, dif, base, thr, k, ts, ds;
    s32 i;

    gGroundHeight = w->contact.y * 1000.0f;
    P[0] = b->bike.pos.x + b->bike.fwd.x * dt;
    P[1] = b->bike.pos.y + b->bike.fwd.y * dt;
    P[2] = b->bike.pos.z + b->bike.fwd.z * dt;
    t = w->groundDist;
    gGroundArea = w->groundArea;
    Q.x = P[0] + t * b->bike.up.x;
    base = P[1] + t * b->bike.up.y;
    Q.z = P[2] + t * b->bike.up.z;
    i = 0;
    Q.y = b->bike.pos.y;
    L = (f32)height_get_ground(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
    dif = L - base;
    thr = 0.002f;
    k = -1.0f;
    if (flag) thr = 0.1f;
    if (b->rearWheel.airTicks > 0 && (dif < 0.0f || 0.5f < t)) thr = 0.05f;
    while ((0.0f < dif) ? (thr < dif) : (thr < -dif)) {
        ts = t;
        ds = dif;
        t -= k * dif;
        if (t < -32.0f || 8.0f < t) {
            t = bike_wheel_ground_bisect(b, P, thr);
            break;
        }
        Q.x = P[0] + t * b->bike.up.x;
        base = P[1] + t * b->bike.up.y;
        Q.z = P[2] + t * b->bike.up.z;
        L = (f32)height_get_ground(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
        dif = L - base;
        if (i++ >= 9) {
            if (i >= 17) {
                t = w->groundDist - 0.5f;
                break;
            }
            height_get_ground_normal(A);
            if (-0.2f < vec3_dot(A, &b->bike.up.x)) {
                k = (t - ts) / ((dif - ds) * 2.0f);
            } else {
                k = -1.0f;
            }
        } else {
            k = (t - ts) / (dif - ds);
        }
    }
    t += 0.5f;
    Q.y = L;
    if (gGroundFound) {
        w->surface = (Surface *)((u8 *)gSurfaceTable + ((gGroundSurface << 2) & 0x3C0)); /* bits 4-7 */
        w->groundFlags = gGroundSurface;
        w->groundArea = gGroundArea;
        height_get_ground_normal(&w->normal.x);
        if (0.25f < w->suspDelta && 0.5f <= w->normal.y && i >= 17) {
            w->suspDelta = 0.0f;
            b->pendingCrash = 0x19;
            b->crashVel = b->bike.vel;
            return;
        }
    } else {
        w->normal.x = w->normal.z = 0.0f;
        w->normal.y = 1.0f;
    }
    if (w->groundDist < w->suspLength) w->suspDelta = t - w->suspLength;
    else w->suspDelta = t - w->groundDist;
    w->groundDist = t;
    w->contactOffset.x = Q.x - b->bike.pos.x;
    w->contactOffset.y = Q.y - b->bike.pos.y;
    w->contactOffset.z = Q.z - b->bike.pos.z;
    w->contactDelta.x = Q.x - w->contact.x;
    w->contactDelta.y = Q.y - w->contact.y;
    w->contactDelta.z = Q.z - w->contact.z;
    w->contact = Q;
}

void bike_update_wheel_contacts(Rider *b) {
    f32 diff = 1.0f;
    s32 idx;
    s32 t;
    s32 w;

    do {
        w = b->rearWheel.groundArea;
        t = b->airFrames;
    } while (0);
    gGroundTri = 0;
    gGroundSurface = 0;
    gGroundArea = w;
    if (t > 0) {
        s32 h;
        t = (s32)&b->bike.pos;

        h = height_get_ground((s32)(((f32 *)t)[(u8)b->pendingCrash >> 8] * 1000.0f), (s32)(((f32 *)t)[1] * 1000.0f), (s32)(((f32 *)t)[2] * 1000.0f));

        diff = b->bike.pos.y - (f32)h / 1000.0f;
        idx = (D_800E84F4[1] >> 4) & 0xF;
        *(volatile s32 *)&b->surfaceIdx = idx;
        if ((gGame.mode == 7 || gCurTrack == &gTracks[TRACK_GOLDMINE_RUSH]) && diff > 2.0f && (gSurfaceTable[idx].flags & 0x800)) {
            bike_queue_crash_2(b, -25);
            return;
        }
    } else {
        b->surfaceIdx = b->rearWheel.surface - gSurfaceTable;
    }
    if (diff < 1.0f) {
        if (b->frontWheel.groundDist < -2.0f || b->rearWheel.groundDist < -2.0f) {
            b->frontWheel.groundDist = 0.0f;
            b->rearWheel.groundDist = 0.0f;
        }
    }
    bike_wheel_find_ground(b, &b->frontWheel, 0.6f, diff > 1.5f);
    bike_wheel_find_ground(b, &b->rearWheel, -0.65f, diff > 1.5f);
    if (b->frontWheel.surface->flags & 0x800) {
        b->pendingCrash = 0x1C;
        b->crashVel = b->bike.vel;
    }
    if (b->rearWheel.surface->flags & 0x800) {
        b->pendingCrash = 0x1C;
        b->crashVel = b->bike.vel;
    }
}

#define MAXRPM (gGame.mode == 6 ? gBikeTuneSpringRate * 20000.0f : 15000.0f)
#define RATE (gGame.mode == 6 ? gBikeTuneDamping * 2000.0f : 2200.0f)

void bike_wheel_spring_force(Wheel *p, f32 x)
{
    f32 a, k, r, v;

    if (p->airTicks > 0) {
        p->springForce = 0.0f;
        a = p->groundDist - p->suspLength;
        if (p->springForce < a) {
            p->airTicks = 0;
            k = a / (a - p->unk_20);
        }
        p->unk_20 = a;
    } else {
        k = 1.0f;
    }

    if (p->airTicks <= 0) {
        r = (p->groundDist + 0.05f + p->suspDelta * 0.5f - x) * (MAXRPM * sSuspSpringScale);
        v = p->suspDelta / gPhysDeltaTime * RATE * sSuspDampScale;
        p->springForce = r + v;
        if (p->springForce < 0.0f) {
            p->springForce = 0.0f;
            p->airTicks = gPhysTicks;
            p->unk_20 = p->suspDelta;
        } else {
            p->airTicks -= gPhysTicks;
            p->suspLength = p->groundDist;
        }
        p->springForce *= k;
    } else {
        p->suspLength = p->suspLength - p->suspLength * (MAXRPM / RATE) * gPhysDeltaTime;
        p->airTicks += gPhysTicks;
    }
}

void bike_update_suspension(Rider *s)
{
    f32 x;
    f32 m1;
    f32 one, fv2, B, A3, t1, t2, t3, fs1v, m2;

    x = s->bike.mass;
    m1 = x * 0.65f;
    m2 = x * 0.6f;
    A3 = ((m1 / 1.25f) - 11.5199995f) * 14.700001f;
    B = ((m2 / 1.25f) + 11.5199995f) * 14.700001f;
    one = 1.0f;
    fv2 = one - one / sSuspSpringScale;
    t1 = fv2 * B;
    if (gGame.mode == 6) {
        fs1v = t1 / (gBikeTuneSpringRate * 20000.0f);
    } else {
        fs1v = t1 / 15000.0f;
    }
    t2 = fv2 * A3;
    if (gGame.mode == 6) {
        t3 = t2 / (gBikeTuneSpringRate * 20000.0f);
    } else {
        t3 = t2 / 15000.0f;
    }
    bike_wheel_spring_force(&s->frontWheel, t3);

    if (((s->speed + 5.0f) * 250.0f * 30.0f) < s->frontWheel.springForce) {
        s->frontWheel.groundDist = 0.0f;
        s->rearWheel.groundDist = 0.0f;
        bike_crash_begin(s, 0x1c);
    }
    bike_wheel_spring_force(&s->rearWheel, fs1v);
    if (((s->speed + 5.0f) * 250.0f * 30.0f) < s->rearWheel.springForce) {
        s->frontWheel.groundDist = 0.0f;
        s->rearWheel.groundDist = 0.0f;
        bike_crash_begin(s, 0x19);
    }
    if (3675.0002f < s->frontWheel.springForce || 3675.0002f < s->rearWheel.springForce) {
        if (0x78 < s->hardLandFrame - gRaceFrame) {
            bike_crash_begin(s, 0x1c);
        }
    } else {
        s->hardLandFrame = gRaceFrame;
    }
    if (s->frontWheel.airTicks == gPhysTicks || s->rearWheel.airTicks == gPhysTicks) {
        bike_sync_wheel_contacts(s);
    }
}

#define LIM(a) (((a)->input & 0x10000) ? 250.0f : 1000.0f)

void bike_rear_tire_force(Rider *a, Wheel *b)
{
    f32 q[3];
    f32 n[3];
    f32 d[3];
    f32 scale, ref, s;
    f32 len3, dv, side, k, len, len2, sq, base, x2, y2, t, v;

    d[0] = b->contact.x - a->bike.pos.x;
    d[1] = b->contact.y - a->bike.pos.y;
    d[2] = b->contact.z - a->bike.pos.z;
    n[0] = a->bike.angVel.y * d[2] - d[1] * a->bike.angVel.z;
    n[1] = a->bike.angVel.z * d[0] - d[2] * a->bike.angVel.x;
    n[2] = a->bike.angVel.x * d[1] - d[0] * a->bike.angVel.y;
    n[0] += a->bike.vel.x;
    n[1] += a->bike.vel.y;
    n[2] += a->bike.vel.z;
    side = vec3_dot(n, &b->latAxis.x);
    ref = vec3_dot(n, &b->lonAxis.x);
    scale = sBikeTune[10];
    b->grip = scale * b->surface->grip;
    s = sBikeTune[10];
    if (a->input & 0x20000) {
        k = sTurboTraction / s;
    } else {
        k = sBikeTune[11] / s;
    }
    if (a->powerSlide == 1) {
        b->grip = b->grip / 3.0f;
        k = 1.0f;
    }
    x2 = b->latForce;
    x2 = x2 * x2;
    y2 = b->lonForce;
    y2 = y2 * y2;
    sq = x2 + y2;
    len = __builtin_sqrtf(sq);
    if (len > 0.0f) {
        b->grip = b->grip * (sq / (x2 + y2 / k));
    }
    base = vec3_dot(&b->normal.x, &a->bike.up.x);
    t = b->springForce - vec3_dot(&b->force.x, &a->bike.up.x);
    t = t / base;
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->input & 0x10000) {
            v = 250.0f;
        }
    }
    if (len < v * b->grip || gGame.mode == 15) {
        d[0] = b->normal.x * t;
        d[1] = b->normal.y * t;
        d[2] = b->normal.z * t;
        b->force.x += d[0];
        b->force.y += d[1];
        b->force.z += d[2];
        b->slip = 0.0f;
        b->spin = ref;
        return;
    }
    y2 = ref - b->spin;
    len2 = __builtin_sqrtf(y2 * y2 + side * side);
    b->slip = len2;
    if (len2 > 2.0f) {
        y2 = -y2 / len2;
        d[0] = b->lonAxis.x * y2;
        d[1] = b->lonAxis.y * y2;
        d[2] = b->lonAxis.z * y2;
        side = -side / len2;
        q[0] = b->latAxis.x * side;
        q[1] = b->latAxis.y * side;
        q[2] = b->latAxis.z * side;
        q[0] += d[0];
        q[1] += d[1];
        q[2] += d[2];
        x2 = side * side;
        y2 = y2 * y2;
        b->grip = (scale * (x2 + y2)) / (x2 + y2 / k);
    } else {
        *(Vec3f *)q = b->force;
        len3 = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
        if (len3 != 0.0f) {
            q[0] /= len3;
            q[1] /= len3;
            q[2] /= len3;
        }
    }
    v = base + vec3_dot(q, &a->bike.up.x) * b->grip;
    if (v < 0.5f) {
        v = 0.5f;
    }
    t = b->springForce / v;
    if (t < 0.0f) {
        t = 0.0f;
    }
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->input & 0x10000) {
            v = 250.0f;
        }
    }
    len = v;
    len *= b->grip;
    b->force.x = t * b->normal.x + len * q[0];
    b->force.y = t * b->normal.y + len * q[1];
    b->force.z = t * b->normal.z + len * q[2];
    b->spin += ((a->driveForce - b->brakeTorque) - vec3_dot(&b->force.x, &b->lonAxis.x)) / 9765.625f;
    if (b->spin < 0.0f) {
        b->spin = 0.0f;
    }
    dv = b->spin - ref;
    if (dv > 10.0f) {
        b->spin = ref + 10.0f;
    } else if (dv < -10.0f) {
        b->spin = ref - 10.0f;
    }
}

static inline f32 getHeight(Vec3f *p)
{
    s32 k;
    if (((s32)p + 1) * 2 != (s32)p->x) k = ((s32 *)p)[12]; else k = ((s32 *)p)[13];
    k = height_get_ground((s32)(p->x * 1000.0f), (s32)(p->y * 1000.0f), (s32)(p->z * 1000.0f));
    return (f32)k / 1000.0f;
}

f32 bike_raycast_ground(Vec3f *a, Vec3f *b, f32 t, f32 d, f32 *e)
{
    Vec3f pos;
    Vec3f ofs;
    f32 h, diff, olddiff, oldt, slope, thr;
    s32 n;

    ofs.x = b->x * t;
    ofs.y = b->y * t;
    ofs.z = b->z * t;
    pos.x = ofs.x + a->x;
    pos.y = ofs.y + a->y;
    pos.z = ofs.z + a->z;
    n = 0;
    h = getHeight(&pos);
    diff = h - pos.y;
    thr = 0.002f;
    slope = -1.0f;
    while ((diff > 0.0f ? diff : -diff) > thr) {
        oldt = t;
        t -= slope * diff;
        ofs.x = b->x * t;
        ofs.y = b->y * t;
        ofs.z = b->z * t;
        pos.x = ofs.x + a->x;
        pos.y = ofs.y + a->y;
        pos.z = ofs.z + a->z;
        olddiff = diff;
        h = getHeight(&pos);
        diff = h - pos.y;
        slope = (t - oldt) / (diff - olddiff);
        if (n++ >= 11) {
            pos.y = h;
            thr *= 2.0f;
        }
    }
    height_get_ground_normal(e);
    return t;
}

void bike_place_on_ground(Rider *b)
{
    f32 tmp[8];
    f32 d[7];
    f32 r;
    f32 df;
    s32 i;
    s32 a;

    b->drawRot[0] = 0;
    b->drawRot[1] = 0;
    quat_to_mtx3f(&b->bike.right.x, b->bike.quat);
    quat_to_angles(b->bike.quat, &b->drawRot[0], &b->drawRot[1], &b->drawRot[2]);
    r = bike_raycast_ground(&b->bike.pos, &b->bike.up, 0.0f, 0.002f, tmp);
    d[0] = b->bike.up.x * -((r + 0.5f) - b->bike.mass * 14.700001f / (gGame.mode == 6 ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    d[1] = b->bike.up.y * -((r + 0.5f) - b->bike.mass * 14.700001f / (gGame.mode == 6 ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    d[2] = b->bike.up.z * -((r + 0.5f) - b->bike.mass * 14.700001f / (gGame.mode == 6 ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    b->bike.pos.x -= d[0];
    b->bike.pos.y -= d[1];
    b->bike.pos.z -= d[2];
    i = 16;
    do {
        quat_to_angles(b->bike.quat, &b->drawRot[0], &b->drawRot[1], &b->drawRot[2]);
        quat_to_mtx3f(&b->bike.right.x, b->bike.quat);
        bike_update_wheel_contacts(b);
        a = (b->frontWheel.groundDist - b->rearWheel.groundDist) / 2.5f * 32768.0f / 3.1415927f;
        if (a > 0x1000) a = 0x1000;
        if (a < -0x1000) a = -0x1000;
        b->drawRot[0] -= a;
        quat_from_euler(b->bike.quat, (f32)b->drawRot[0] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->drawRot[1] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->drawRot[2] * 3.1415927f * 3.0517578e-05f);
        df = b->frontWheel.groundDist - b->rearWheel.groundDist;
    } while ((0.0f < df ? 0.02f < df : 0.02f < -df) && i++ < 16);
    bike_update_wheel_contacts(b);
    r = (((b->frontWheel.groundDist + b->rearWheel.groundDist) * 0.5f) - b->bike.mass * 14.700001f / (gGame.mode == 6 ? gBikeTuneSpringRate * 20000.0f + gBikeTuneSpringRate * 20000.0f : 30000.0f));
    d[0] = b->bike.up.x * -r;
    d[1] = b->bike.up.y * -r;
    d[2] = b->bike.up.z * -r;
    b->bike.pos.x -= d[0];
    b->bike.pos.y -= d[1];
    b->bike.pos.z -= d[2];
}

#define FABS_GT(x, e) ((0.0f < (x)) ? ((e) < (x)) : ((e) < -(x)))
#define LEN(o) __builtin_sqrtf((o)->contactDelta.x * (o)->contactDelta.x + (o)->contactDelta.y * (o)->contactDelta.y + (o)->contactDelta.z * (o)->contactDelta.z)

void bike_wheel_surface_bumps(Wheel *a0, f32 *a1)
{
    f32 len, pos, acc, t, v, targ;

    acc = 0.0f;
    len = -1.0f;
    v = a0->bump - a0->surface->bumpHeight;
    if (FABS_GT(v, 0.001f)) {
        len = LEN(a0);
        t = math_exp(-len);
        a0->bump = pos = a0->bump * t + (1.0f - t) * a0->surface->bumpHeight;
    } else {
        pos = a0->bump = a0->surface->bumpHeight;
    }
    if (FABS_GT(a0->bumpNoise, 0.001f) || a0->surface->bumpNoise != 0.0f) {
        if (len < 0.0f) {
            len = LEN(a0);
        }
        acc = (f32)(s32)(math_sin_rand(a1) - 0x80) * 0.00390625f;
        targ = -len * a0->surface->bumpRate;
        acc *= a0->surface->bumpNoise;
        t = math_exp(targ);
        acc = t * a0->bumpNoise + (1.0f - t) * acc;
        a0->bumpNoise = acc;
    }
    a0->suspDelta += pos + acc;
    a0->groundDist += pos + acc;
}

void bike_update_brakes(Rider *p)
{
    f32 a514;
    f32 f3, f5, a470;
    s16 sv;

    f3 = 0.5f;
    a514 = p->frontWheel.springForce;
    a470 = p->rearWheel.springForce;
    /* zeroed as words: the float compare below reads the field again */
    *(u32 *)&p->rearWheel.brakeTorque = 0;
    f5 = a514 + a470;
    *(u32 *)&p->frontWheel.brakeTorque = 0;
    if (p->rearWheel.brakeTorque < f5) {
        f3 = a514 / f5;
    }
    if ((p->input & 0x20) != 0) {
        f5 = 6.0f;
        sv = p->frontWheel.brakeRamp + 1;
        p->frontWheel.brakeRamp = sv;
        if (f5 < (f32)sv) {
            p->frontWheel.brakeRamp = 6;
        }
        if (p->frontWheel.airTicks < 1) {
            p->frontWheel.brakeTorque = ((f32)p->frontWheel.brakeRamp * 2572.5f * f3) / f5;
        }
        sv = p->rearWheel.brakeRamp + 1;
        p->rearWheel.brakeRamp = sv;
        if (f5 < (f32)sv) {
            p->rearWheel.brakeRamp = 6;
        }
        if (p->rearWheel.airTicks < 1) {
            p->rearWheel.brakeTorque = ((f32)p->rearWheel.brakeRamp * 2572.5f * (1.0f - f3)) / f5;
        }
    } else {
        p->frontWheel.brakeRamp = 0;
        p->rearWheel.brakeRamp = 0;
    }
    f5 = p->fwdSpeed;
    if (0.0f < f5) {
        p->reversing = 0;
    } else {
        p->reversing = 1;
    }
    if ((gEvent.flags & 0x10) == 0) {
        if ((p->input & 0x20010) != 0 && f5 < 0.0f) {
            f32 m, q;
            m = f3 * -2572.5f;
            q = -2572.5f - m;
            p->frontWheel.brakeTorque = m;
            p->rearWheel.brakeTorque = q;
            return;
        }
        if (p->wheelieTimer == 0) {
            if (f5 < sBikeMaxReverseSpeed) {
                goto neg;
            }
            return;
        }
        if (!(f5 < 0.0f)) {
            return;
        }
neg:
        p->frontWheel.brakeTorque = -p->frontWheel.brakeTorque;
        p->rearWheel.brakeTorque = -p->rearWheel.brakeTorque;
        return;
    }
    {
        f32 m, dv;
        m = f5 * 250.0f;
        dv = gPhysDeltaTime + gPhysDeltaTime;
        p->unk_412 = 0x3000;
        m = m / dv;
        p->frontWheel.brakeTorque = m;
        p->rearWheel.brakeTorque = m;
        return;
    }
}

void bike_update_drive_force(Rider *p)
{
    f32 a, b, t, x;

    a = p->engine.torque;
    b = p->fwdSpeed;
    if (a < 0.0f || (p->rHeld >= 1 && p->rHeld <= 4)) {
        if (0.0f < b) {
            p->driveForce = p->bike.mass * -2.0f;
        } else {
            p->driveForce = 0.0f;
        }
        return;
    }
    if (b <= 1.0f) {
        b = 1.0f;
    }
    t = (a * p->power) / b;
    p->driveForce = t;
    if (p->frontWheel.airTicks < 0 && p->rearWheel.airTicks < 0 && b < 10.0f) {
        x = p->frontWheel.contact.y - p->rearWheel.contact.y;
        if (0.15f < x) {
            if (gGame.mode == 7) {
                if (p->stickY >= 0) {
                    return;
                }
                p->driveForce = t * (1.0f - (x - 0.15f) * (p->stickY * 0.125f));
            } else {
                p->driveForce = t * ((x - 0.15f) * 2.0f + 1.0f);
            }
        }
    }
}

void bike_process_input(Rider *o, s32 a1)
{
    f32 fs0;
    f32 fs1;
    f32 fv0;
    s32 w;
    s32 n;
    s16 t;
    s32 x;
    s32 v;

    w = o->input;
    fs1 = (s32)((u32)w >> 28);
    if (w & 2) {
        fs1 = -fs1;
    }
    fs0 = (w >> 24) & 0xF;
    if (w & 8) {
        fs0 = -fs0;
    }
    if (o->camMode == 4) {
        v = fs0;
        fs0 = fs1;
        fs1 = -v;
    }
    if ((u32)(o->airFrames + 14) < 15) {
        n = o->rearWheel.airTicks;
        if (n < o->frontWheel.airTicks) {
            n = o->frontWheel.airTicks;
        }
        if (n >= -6) {
            fs0 *= 0.25f;
        }
    }
    if (gGame.mode == 6) {
        x = (u16)gRiders[0].drawRot[2] - 0x8000;
        t = x;
        if (t > 0x2000 && 0.0f < fs0) {
            fs0 = 0.0f;
            o->bike.angVel.y = fs0;
        } else if (t < -0x2000 && fs0 < 0.0f) {
            fs0 = 0.0f;
            o->bike.angVel.y = fs0;
        }
        if (o->airFrames <= 0) {
            if (t > 0x4000) {
                goto call;
            }
            if (t < -0x4000) {
call:
                bike_respawn(o);
            }
        }
    }
    if (o->airFrames < 0) {
        if (o->input & 0x100) {
            fs0 = 0.0f;
        }
    } else if ((o->input & 0x100) || (u8)o->anim < 0x10) {
        fs1 = 0.0f;
        fs0 *= 0.25f;
    }
    if (o->input & 0x20000) {
        if (!(o->prevInput & 0x20000)) {
            o->turboPressFrame = gRaceFrame;
        }
    } else if (o->prevInput & 0x20000) {
        o->turboReleaseFrame = gRaceFrame;
    }
    if ((o->input & 0x10000) && o->powerSlide == 0) {
        if (o->rHeld >= 0) {
            o->rHeld++;
        } else {
            o->rHeld = 0;
        }
    } else {
        if (o->rHeld < 0) {
            o->rHeld--;
        } else {
            o->rHeld = -1;
        }
    }
    if (o->wheelieTimer == 0) {
        if (fs1 < -8.0f && o->airFrames < 0 && (8.0f < fs0 || fs0 < -8.0f)) {
            o->hardTurn = 1;
            if (0.0f < fs0) {
                fs0 = 15.0f;
            } else {
                fs0 = -15.0f;
            }
        } else {
            o->hardTurn = 0;
        }
        if (o->isCpu == 0) {
            if ((0.0f < fs0) ? (10.0f <= fs0) : (10.0f <= -fs0)) {
                if ((o->input & 0x10020) == 0x10020 && 4.0f < o->speed && (s8)o->anim != -1) {
                    o->powerSlide = 1;
                    goto L37c;
                }
            }
            if (o->powerSlide == 1) {
                o->powerSlide = 2;
            } else {
                o->powerSlide = 0;
            }
        }
L37c:
        if (o->rHeld >= -14 && o->airFrames < 0) {
            fv0 = (o->driveForce - o->rearWheel.brakeTorque - o->frontWheel.brakeTorque) * 15.625f / 1323.0001f + 4.0f;
            if (13.0f < fv0) {
                fv0 = 13.0f;
            }
            if (fv0 < 0.0f) {
                fv0 = 0.0f;
            }
            fs1 = (s32)fv0;
        }
    }
    o->stickX = (s32)fs0;
    o->stickY = (s32)fs1;
    if (o->rearWheel.airTicks < 0) {
        o->dirtSpray = 0;
        if (o->powerSlide != 0) {
            goto two;
        }
        if (o->input & 0x20000) {
            if (gRaceFrame - o->turboPressFrame < 30) {
two:
                o->dirtSpray = 2;
            } else {
                o->dirtSpray = 1;
            }
        } else if (o->input & 0x10000) {
            if ((0.0f < fs0) ? (8.0f < fs0) : (8.0f < -fs0)) {
                if (5.0f < o->speed) {
                    o->dirtSpray = 1;
                }
            }
        }
    }
end:
    rider_update_input_history(o, a1);
}

#define MINSPD ((b->input & 0x10000) ? 250.0f : 1000.0f)

typedef struct { u8 pad[0x28]; f32 f28; f32 f2C; } Glob_2;

extern Glob_2 sBikeTune_2;

void bike_update_tire_forces(Rider *b)
{
    Vec3f dir;
    Vec3f c;
    Vec3f d;
    f32 d2;
    f32 ratio;
    f32 d1, scale, sp, sl, x2, y2, ss, f, v, len, sv, t, n, m, g, e;

    if (b->rearWheel.airTicks <= 0) {
        b->rearWheel.lonForce = b->driveForce - b->rearWheel.brakeTorque;
        bike_rear_tire_force(b, &b->rearWheel);
    } else {
        b->rearWheel.slip = b->driveForce = b->rearWheel.brakeTorque = b->rearWheel.force.x = b->rearWheel.force.y = b->rearWheel.force.z = 0.0f;
    }
    if (b->frontWheel.airTicks <= 0) {
    d.x = b->frontWheel.contact.x - b->bike.pos.x;
    d.y = b->frontWheel.contact.y - b->bike.pos.y;
    d.z = b->frontWheel.contact.z - b->bike.pos.z;
    c.x = b->bike.angVel.y * d.z - d.y * b->bike.angVel.z;
    c.y = b->bike.angVel.z * d.x - d.z * b->bike.angVel.x;
    c.z = b->bike.angVel.x * d.y - d.x * b->bike.angVel.y;
    c.x += b->bike.vel.x;
    c.y += b->bike.vel.y;
    c.z += b->bike.vel.z;
    d1 = vec3_dot(&c.x, &b->frontWheel.latAxis.x);
    d2 = vec3_dot(&c.x, &b->frontWheel.lonAxis.x);
    scale = sBikeTune_2.f28;
    m = scale * b->frontWheel.surface->grip;
    ratio = sBikeTune_2.f2C / scale;
    b->frontWheel.grip = m;
    if (b->powerSlide == 1) {
        b->frontWheel.grip = m / 3.0f;
        ratio = 1.0f;
    } else if (b->input & 0x10000) {
        b->frontWheel.grip = m * 1.3f;
    }
    x2 = b->frontWheel.latForce;
    x2 *= x2;
    y2 = b->frontWheel.lonForce;
    y2 *= y2;
    ss = x2 + y2;
    sl = __builtin_sqrtf(ss);
    if (sl > 0.0f) {
        b->frontWheel.grip = b->frontWheel.grip * (ss / (x2 + y2 / ratio));
    }
    t = vec3_dot(&b->frontWheel.normal.x, &b->bike.up.x);
    f = b->frontWheel.springForce - vec3_dot(&b->frontWheel.force.x, &b->bike.up.x);
    f /= t;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    if (sl < v * b->frontWheel.grip || gGame.mode == 15) {
        d.x = b->frontWheel.normal.x * f;
        d.y = b->frontWheel.normal.y * f;
        d.z = b->frontWheel.normal.z * f;
        b->frontWheel.force.x += d.x;
        b->frontWheel.force.y += d.y;
        b->frontWheel.force.z += d.z;
        b->frontWheel.slip = 0.0f;
        b->frontWheel.spin = d2;
    } else {
    y2 = d2 - b->frontWheel.spin;
    len = __builtin_sqrtf(y2 * y2 + d1 * d1);
    b->frontWheel.slip = len;
    if (len > 2.0f) {
        y2 = -y2 / len;
        d.x = b->frontWheel.lonAxis.x * y2;
        d.y = b->frontWheel.lonAxis.y * y2;
        d.z = b->frontWheel.lonAxis.z * y2;
        d1 = -d1 / len;
        x2 = d1 * d1;
        dir.x = b->frontWheel.latAxis.x * d1;
        y2 = y2 * y2;
        dir.y = b->frontWheel.latAxis.y * d1;
        dir.z = b->frontWheel.latAxis.z * d1;
        dir.y += d.y;
        dir.x += d.x;
        dir.z += d.z;
        b->frontWheel.grip = scale * (x2 + y2) / (x2 + y2 / ratio);
    } else {
        dir = *(Vec3f *)&b->frontWheel.force.x;
        n = __builtin_sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (n != 0.0f) {
            dir.x /= n;
            dir.y /= n;
            dir.z /= n;
        }
    }
    sp = t + vec3_dot(&dir.x, &b->bike.up.x) * b->frontWheel.grip;
    if (sp < 0.5f) sp = 0.5f;
    f = b->frontWheel.springForce / sp;
    if (f < 0.0f) f = 0.0f;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    sl = v;
    sl *= b->frontWheel.grip;
    b->frontWheel.force.x = f * b->frontWheel.normal.x + sl * dir.x;
    b->frontWheel.force.y = f * b->frontWheel.normal.y + sl * dir.y;
    b->frontWheel.force.z = f * b->frontWheel.normal.z + sl * dir.z;
    b->frontWheel.spin += (-vec3_dot(&b->frontWheel.force.x, &b->frontWheel.lonAxis.x) - b->frontWheel.brakeTorque) / 9765.625f;
    if (b->frontWheel.spin < 0.0f) b->frontWheel.spin = 0.0f;
    e = b->frontWheel.spin - d2;
    if (e > 10.0f) {
        b->frontWheel.spin = d2 + 10.0f;
    } else if (e < -10.0f) {
        b->frontWheel.spin = d2 - 10.0f;
    }
    }
    } else {
        b->frontWheel.slip = b->frontWheel.brakeTorque = b->frontWheel.force.x = b->frontWheel.force.y = b->frontWheel.force.z = 0.0f;
    }
}

#define CROSS(r, a, b) \
    (r).x = (a).y * (b).z - (b).y * (a).z; \
    (r).y = (a).z * (b).x - (b).z * (a).x; \
    (r).z = (a).x * (b).y - (b).x * (a).y

#define NORM(v) { \
    f32 len = __builtin_sqrtf((v).x * (v).x + (v).y * (v).y + (v).z * (v).z); \
    if (len != 0.0f) { (v).x /= len; (v).y /= len; (v).z /= len; } }

void bike_solve_tire_constraints(Rider *b)
{
    Vec3f out, in, t, c1, c2, v, c3, c4, u, s, q, r, p, w;
    f32 ra, rb, k, A, B, C, D, r0, x, y, det, d, e, f, g;

    b->rearWheel.force.z = 0.0f;
    b->rearWheel.force.y = 0.0f;
    b->rearWheel.force.x = 0.0f;
    b->frontWheel.force.z = 0.0f;
    b->frontWheel.force.y = 0.0f;
    b->frontWheel.force.x = 0.0f;
    in.x = cosf(b->steer);
    in.y = 0.0f;
    in.z = sinf(b->steer);
    vec3_mul_mtx3f(&in.x, &b->bike.right.x, &out.x);
    CROSS(t, out, b->frontWheel.normal);
    NORM(t);
    b->frontWheel.lonAxis = t;
    CROSS(b->frontWheel.latAxis, b->frontWheel.normal, t);
    CROSS(t, b->bike.right, b->rearWheel.normal);
    NORM(t);
    b->rearWheel.lonAxis = t;
    CROSS(b->rearWheel.latAxis, b->rearWheel.normal, t);
    if (b->frontWheel.airTicks <= 0) {
        if (b->rearWheel.airTicks <= 0) {
            p.x = b->bike.vel.x + (b->frontWheel.force.x + b->rearWheel.force.x) * (gPhysDeltaTime / b->bike.mass);
            p.y = b->bike.vel.y + (b->frontWheel.force.y + b->rearWheel.force.y) * (gPhysDeltaTime / b->bike.mass) - gPhysDeltaTime * 14.700001f;
            p.z = b->bike.vel.z + (b->frontWheel.force.z + b->rearWheel.force.z) * (gPhysDeltaTime / b->bike.mass);
            CROSS(q, b->frontWheel.contactOffset, b->frontWheel.force);
            CROSS(r, b->rearWheel.contactOffset, b->rearWheel.force);
            k = gPhysDeltaTime / 250.0f;
            w.x = b->bike.angVel.x + (q.x + r.x) * k;
            w.y = b->bike.angVel.y + (q.y + r.y) * k;
            w.z = b->bike.angVel.z + (q.z + r.z) * k;
            CROSS(s, w, b->rearWheel.contactOffset);
            s.x += p.x;
            s.y += p.y;
            s.z += p.z;
            ra = vec3_dot(&s.x, &b->rearWheel.latAxis.x);
            CROSS(u, w, b->frontWheel.contactOffset);
            u.x += p.x;
            u.y += p.y;
            u.z += p.z;
            rb = vec3_dot(&u.x, &b->frontWheel.latAxis.x);
            CROSS(t, b->frontWheel.contactOffset, b->frontWheel.latAxis);
            d = vec3_dot(&t.x, &b->bike.fwd.x);
            t.x -= d * b->bike.fwd.x;
            t.y -= d * b->bike.fwd.y;
            t.z -= d * b->bike.fwd.z;
            CROSS(c1, t, b->frontWheel.contactOffset);
            CROSS(c2, t, b->rearWheel.contactOffset);
            CROSS(v, b->rearWheel.contactOffset, b->rearWheel.latAxis);
            d = vec3_dot(&v.x, &b->bike.fwd.x);
            v.x -= d * b->bike.fwd.x;
            v.y -= d * b->bike.fwd.y;
            v.z -= d * b->bike.fwd.z;
            CROSS(c3, v, b->frontWheel.contactOffset);
            CROSS(c4, v, b->rearWheel.contactOffset);
            r0 = vec3_dot(&b->rearWheel.latAxis.x, &b->frontWheel.latAxis.x);
            A = 1.0f / b->bike.mass + vec3_dot(&c1.x, &b->frontWheel.latAxis.x) / 250.0f;
            B = r0 / b->bike.mass + vec3_dot(&c3.x, &b->frontWheel.latAxis.x) / 250.0f;
            C = r0 / b->bike.mass + vec3_dot(&c2.x, &b->rearWheel.latAxis.x) / 250.0f;
            D = 1.0f / b->bike.mass + vec3_dot(&c4.x, &b->rearWheel.latAxis.x) / 250.0f;
            if (b->rearWheel.airTicks > 0) {
                x = -rb / A;
                y = 0.0f;
            } else if (b->frontWheel.airTicks > 0) {
                x = 0.0f;
                y = -ra / D;
            } else {
                x = (B * ra - D * rb) / (A * D - B * C);
                y = (C * rb - A * ra) / (A * D - B * C);
            }
            y /= gPhysDeltaTime;
            x /= gPhysDeltaTime;
            b->rearWheel.latForce = y;
            b->frontWheel.latForce = x;
            b->rearWheel.force.x = b->rearWheel.latAxis.x * y + b->rearWheel.lonAxis.x * (b->driveForce - b->rearWheel.brakeTorque);
            b->rearWheel.force.y = b->rearWheel.latAxis.y * y + b->rearWheel.lonAxis.y * (b->driveForce - b->rearWheel.brakeTorque);
            b->rearWheel.force.z = b->rearWheel.latAxis.z * y + b->rearWheel.lonAxis.z * (b->driveForce - b->rearWheel.brakeTorque);
            b->frontWheel.force.x = b->frontWheel.latAxis.x * x - b->frontWheel.lonAxis.x * b->frontWheel.brakeTorque;
            b->frontWheel.force.y = b->frontWheel.latAxis.y * x - b->frontWheel.lonAxis.y * b->frontWheel.brakeTorque;
            b->frontWheel.force.z = b->frontWheel.latAxis.z * x - b->frontWheel.lonAxis.z * b->frontWheel.brakeTorque;
        }
    } else if (b->rearWheel.airTicks <= 0 && b->wheelieTimer != 0) {
        b->rearWheel.force.x = b->rearWheel.lonAxis.x * (b->driveForce - b->rearWheel.brakeTorque);
        b->rearWheel.force.y = b->rearWheel.lonAxis.y * (b->driveForce - b->rearWheel.brakeTorque);
        b->rearWheel.force.z = b->rearWheel.lonAxis.z * (b->driveForce - b->rearWheel.brakeTorque);
        e = vec3_dot(&b->bike.vel.x, &b->rearWheel.latAxis.x) * (b->bike.mass / gPhysDeltaTime);
        b->rearWheel.force.x -= b->rearWheel.latAxis.x * e;
        b->rearWheel.force.y -= b->rearWheel.latAxis.y * e;
        b->rearWheel.force.z -= b->rearWheel.latAxis.z * e;
        f = vec3_dot(&b->bike.vel.x, &b->rearWheel.lonAxis.x);
        if (f > 0.0f) {
            g = 1.0f / (f + 20.0f);
        } else {
            g = 1.0f / (-f + 20.0f);
        }
        f = g * (b->turnRadius / 0.57f);
        b->bike.angVel.y += -(b->stickX * f) - b->bike.angVel.y * 0.5f;
        b->stickX *= 0.25f;
    }
}

f32 bike_calc_lean_torque(Rider *p)
{
    struct { s16 h[2]; s32 w; } lo;
    f32 fVar2, fVar3, fVar4, fX, fR;

    if (p->wheelieTimer == 0) {
        if (p->fwdSpeed < 1.0f && p->powerSlide == 0) {
            if ((p->input & 0x20) != 0) {
                p->unk_412 = (s16)(s32)((f32)(s32)p->unk_412 + p->fwdSpeed * 1066.6666f);
            } else {
                p->unk_412 = D_800C94C0;
            }
            if (p->rightFootOut < 0x10) p->rightFootOut += 2;
            if (p->leftFootOut < 0x10) p->leftFootOut += 2;
        } else {
            if (p->drawRot[1] >= 0x1001 &&
                (p->isCpu != 0 || (p->input & 0x10000) != 0 ||
                 p->powerSlide != 0 || p->hardTurn != 0 || p->fwdSpeed < 10.0f)) {
                if (p->rightFootOut < 0x10) p->rightFootOut += 2;
            } else {
                if (p->rightFootOut != 0) p->rightFootOut -= 1;
            }
            if (p->drawRot[1] < -0x1000 &&
                (p->isCpu != 0 || (p->input & 0x10000) != 0 ||
                 p->powerSlide != 0 || p->hardTurn == 1 || p->fwdSpeed < 10.0f)) {
                if (p->leftFootOut < 0x10) p->leftFootOut += 2;
            } else {
                if (p->leftFootOut != 0) p->leftFootOut -= 1;
            }
        }
    }
    math_to_polar((s32)(p->bike.right.y * 1000.0f), (s32)(p->bike.up.y * 1000.0f), lo.h, &lo.w);
    fVar4 = (f32)(s32)lo.h[0] * 0.001953125f;

    if (p->airFrames < 0) {
        fVar3 = (f32)(s32)p->stickX;
        fVar2 = p->speed;
        if (fVar2 < 0.0f) {
            fVar3 = 0.0f;
        } else if (fVar2 < 4.0f) {
            fVar3 = fVar3 * fVar2 * 0.25f;
        }
        if (p->rHeld < 0) fVar3 = fVar3 * 0.6667f;
        if (p->rHeld >= -9 && (fVar3 - fVar4) * fVar4 < 0.0f) {
            fVar3 = fVar4 + (fVar3 - fVar4) * 0.25f;
        }
    } else if (p->anim == -1) {
        if (p->isCpu != 0) {
            fVar3 = (f32)(s32)p->stickX * 0.25f;
        } else {
            fVar3 = (f32)(s32)p->stickX * 0.5f;
        }
    } else {
        fVar3 = 0.0f;
        if ((u8)((u8)p->anim - 0x10) < 2) {
            fVar3 = (f32)((((s32)((u32)(u16)p->unk_622 << 16)) >> 27) * 3);
        }
    }
    if (p->reversing != 0) fVar3 = 0.0f;
    fX = fVar3 - fVar4;
    if (12.0f < fX) fX = 12.0f;
    else if (fX < -12.0f) fX = -12.0f;

    if (p->isCpu == 0) {
        fX = sLeanTorqueGain * fX - sLeanTorqueDamping * p->localAngVel.z;
    } else {
        fX = fX * 0.015f - p->localAngVel.z * 0.1f;
    }
    fR = 0.0f;
    if (gGame.mode != 0xF) fR = fX;
    return fR;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

void bike_compute_forces(Rider *b) {
    Vec3f F;
    Vec3f T;
    Vec3f A;
    Vec3f c2;
    Vec3f c1;
    Vec3f unused;
    Vec3f d;
    Vec3f e;
    s16 ang;
    s32 ratio;
    s16 ang2;
    s32 tmp;
    f32 grav;
    f32 save;
    Vec3f *p;
    s32 n;

    A.x = b->rearWheel.force.x + b->frontWheel.force.x;
    A.y = b->rearWheel.force.y + b->frontWheel.force.y;
    A.z = b->rearWheel.force.z + b->frontWheel.force.z;
    if (b->speed != 0.0f) {
        f32 lo;
        f32 k;
        f32 q;

        lo = 0.0f;
        if (b->roughLanding != 0) {
            lo = 7.0f;
        }
        k = 0.21000001f;
        if (b->airFrames <= 0) {
            k = (b->wheelieTimer != 0 ? (b->rearWheel.surface->drag - 1.0f) * 0.25f + 1.0f : b->rearWheel.surface->drag) * 0.21000001f;
        }
        if (k < lo) {
            k = lo;
        }
        k = k * b->speed * b->speed;
        if (b->airFrames <= 0) {
            k += b->rearWheel.surface->unk_10 * sqrtf(b->rearWheel.force.x * b->rearWheel.force.x + b->rearWheel.force.y * b->rearWheel.force.y + b->rearWheel.force.z * b->rearWheel.force.z);
            k += b->frontWheel.surface->unk_10 * sqrtf(b->frontWheel.force.x * b->frontWheel.force.x + b->frontWheel.force.y * b->frontWheel.force.y + b->frontWheel.force.z * b->frontWheel.force.z);
            if (b->hardTurn != 0) {
                k += sHardTurnDrag;
            }
            if (b->rHeld < -10 && b->isCpu == 0 && !(b->input & 0x20)) {
                s32 n;
                f32 t;
                f32 u;
                n = ABS(b->stickX);
                t = n;
                if (t > 3.0f) {
                    u = t - 3.0f;
                    k += (u > 0.0f ? u : -u) * b->handling;
                }
            }
        }
        q = k / b->speed;
        A.x -= b->bike.vel.x * q;
        A.y -= b->bike.vel.y * q;
        A.z -= b->bike.vel.z * q;
    }
    save = b->localAngVel.z;
    mtx3f_mul_vec3(&b->bike.angVel, &b->bike.right, &b->localAngVel.x);
    b->localAngVel.z = save;
    c1.x = b->rearWheel.contactOffset.y * b->rearWheel.force.z - b->rearWheel.force.y * b->rearWheel.contactOffset.z;
    c1.y = b->rearWheel.contactOffset.z * b->rearWheel.force.x - b->rearWheel.force.z * b->rearWheel.contactOffset.x;
    c1.z = b->rearWheel.contactOffset.x * b->rearWheel.force.y - b->rearWheel.force.x * b->rearWheel.contactOffset.y;
    c2.x = b->frontWheel.contactOffset.y * b->frontWheel.force.z - b->frontWheel.force.y * b->frontWheel.contactOffset.z;
    c2.y = b->frontWheel.contactOffset.z * b->frontWheel.force.x - b->frontWheel.force.z * b->frontWheel.contactOffset.x;
    c2.z = b->frontWheel.contactOffset.x * b->frontWheel.force.y - b->frontWheel.force.x * b->frontWheel.contactOffset.y;
    T.x = c2.x + c1.x;
    T.y = c2.y + c1.y;
    T.z = c2.z + c1.z;
    mtx3f_mul_vec3(&T, &b->bike.right, &F);
    F.x += b->localAngVel.y * 250.0f * b->localAngVel.z;
    p = &b->bike.pos;
    if (b == 0) grav = 1.0f; else if (p == 0) grav = 2.0f; else if (b->rot[0] == b->animFrame) grav = 3.0f; else grav = 4.0f;
    grav = 14.700001f;
    if (b->airFrames < 7
        || (f32)height_get_ground(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f - b->bike.pos.y + 0.5f > -0.2f
        || b->pendingCrash != 0) {
        if (b->wheelieTimer == 0) {
            f32 t;
            t = (f32)b->stickY - 5.0f;
            if (b->hardTurn != 0) {
                t = 3.0f;
            }
            F.x += t * 32.0f / 1000.0f * 1323.0001f;
            if (gGame.mode != 7) {
                if (b->frontWheel.airTicks >= 8 && b->rearWheel.suspLength == b->rearWheel.groundDist && b->localAngVel.x < 0.0f) {
                    F.x += 8000.0f;
                }
                if (b->frontWheel.suspLength == b->frontWheel.groundDist && b->rearWheel.airTicks >= 8 && b->localAngVel.x > 0.0f) {
                    F.x -= 8000.0f;
                }
            }
        } else if (b->wheelieTimer >= 16) {
            n = -0x2800 - b->wheelieBalance * 3 - b->rot[0];
            F.x = ((f32)n * 3.1415927f * 3.0517578e-05f - b->localAngVel.x * 0.5f) * 15000.0f;
        } else {
            n = math_atan2f(1.3f, b->rearWheel.contact.y - b->frontWheel.contact.y) * 32768.0f / 3.1415927f - b->rot[0];
            if (b->wheelieTimer != 1) {
                F.x = ((f32)n * 3.0f * 3.1415927f * 3.0517578e-05f - b->localAngVel.x * 0.5f) * 15000.0f;
            } else {
                b->bike.angVel.x = b->bike.angVel.y = b->bike.angVel.z = 0.0f;
            }
        }
    } else {
        grav = b->airPitch * grav + grav;
        if (b->anim == -1 || b->anim == 17 || b->anim == 16) {
            f32 t;
            t = (f32)b->stickX * b->airTurn;
            A.x -= b->bike.vel.z * t * b->bike.mass;
            A.z += b->bike.vel.x * t * b->bike.mass;
        }
        if (b->landingAssist == 0) {
            if (b->wheelieTimer != 0) {
                n = -0x2800 - b->wheelieBalance * 3 - b->rot[0];
            } else {
                s32 r;
                r = b->rot[0];
                n = b->airPitchInput;
                if (ABS(r) > 0x2000) {
                    n = 0;
                }
                n = n * 384 - r;
            }
            F.x += ((f32)(n / 4) * 3.1415927f * 3.0517578e-05f - b->localAngVel.x * 0.0625f) * 15000.0f;
        }
        if (b->anim == 15) {
            s32 r;
            math_to_polar(-b->bike.vel.x * 1000.0f, b->bike.vel.z * 1000.0f, &ang, &ratio);
            math_to_polar(-b->bike.fwd.x * 1000.0f, b->bike.fwd.z * 1000.0f, &ang2, &ratio);
            b->heading = ang;
            r = rider_anim_get_length(15, &tmp);
            ang += ((b->animFrame - (r << 8)) << 8) / r;
            ang -= ang2;
            if (ang > 0xC00) {
                ang = 0xC00;
            }
            b->bike.angVel.y = -((f32)ang * 3.1415927f * 3.0517578e-05f) * 60.0f;
        } else if (b->speed > 5.0f) {
            f32 t;
            math_to_polar(b->bike.vel.x * 1000.0f, b->bike.vel.z * 1000.0f, &ang, &ratio);
            math_to_polar(b->bike.fwd.x * 1000.0f, b->bike.fwd.z * 1000.0f, &ang2, &ratio);
            ang -= b->unk_622;
            ang -= ang2;
            t = (f32)ang * 6.1035156e-05f - b->bike.angVel.y * 0.25f;
            if (t > 0.1f) {
                t = 0.1f;
            } else if (t < -0.1f) {
                t = -0.1f;
            }
            t *= 15000.0f;
            F.x += t * b->bike.right.y;
            F.y += t * b->bike.up.y;
            F.z += t * b->bike.fwd.y;
        }
    }
    A.y -= grav * b->bike.mass;
    b->bike.acc.x = A.x / b->bike.mass;
    b->bike.acc.y = A.y / b->bike.mass;
    b->bike.acc.z = A.z / b->bike.mass;
    F.z = bike_calc_lean_torque(b) * 64.0f * 250.0f;
    F.x += F.z * b->localAngVel.y * gPhysDeltaTime;
    vec3_mul_mtx3f(&F, &b->bike.right, &T);
    b->bike.angAcc.x = T.x / 250.0f;
    b->bike.angAcc.y = T.y / 250.0f;
    b->bike.angAcc.z = T.z / 250.0f;
    if (b->powerSlide != 0 && (b->stickX < -10 || b->stickX > 10)) {
        f32 len;
        f32 t;
        d.x = b->frontWheel.normal.x + b->rearWheel.normal.x;
        d.y = b->frontWheel.normal.y + b->rearWheel.normal.y;
        d.z = b->frontWheel.normal.z + b->rearWheel.normal.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = vec3_dot(&d, &b->bike.angVel);
        if (b->powerSlide == 2) {
            f32 q;
            q = t / (2.0f * gPhysDeltaTime);
            b->bike.angAcc.x -= d.x * q;
            b->bike.angAcc.y -= d.y * q;
            b->bike.angAcc.z -= d.z * q;
        } else {
            s32 n;
            n = t;
            n = ABS(n);
            if ((f32)n < 1.5f) {
                f32 s;
                s = gPhysDeltaTime * 240.0f;
                b->bike.angAcc.x -= d.x * ((f32)b->stickX / s);
                b->bike.angAcc.y -= d.y * ((f32)b->stickX / s);
                b->bike.angAcc.z -= d.z * ((f32)b->stickX / s);
            }
        }
    }
    if (b->rHeld >= -1 && b->anim == -1) {
        f32 len;
        f32 t;
        s32 m;
        d.x = b->frontWheel.normal.x + b->rearWheel.normal.x;
        d.y = b->frontWheel.normal.y + b->rearWheel.normal.y;
        d.z = b->frontWheel.normal.z + b->rearWheel.normal.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = vec3_dot(&d, &b->bike.angVel);
        m = b->stickX;
        if ((t > 0.5f && m >= -3) || (t < -0.5f && m < 4)) {
            f32 q;
            q = t / (2.0f * gPhysDeltaTime);
            b->bike.angAcc.x = -d.x * q;
            b->bike.angAcc.y = -d.y * q;
            b->bike.angAcc.z = -d.z * q;
        }
    }
    if (b->bike.hitTimer != 0) {
        f32 len;
        f32 t;
        e.x = b->frontWheel.normal.x + b->rearWheel.normal.x;
        e.y = b->frontWheel.normal.y + b->rearWheel.normal.y;
        e.z = b->frontWheel.normal.z + b->rearWheel.normal.z;
        len = sqrtf(e.x * e.x + e.y * e.y + e.z * e.z);
        if (len != 0.0f) {
            e.x /= len;
            e.y /= len;
            e.z /= len;
        }
        t = vec3_dot(&e, &b->bike.angVel) / (gPhysDeltaTime * 16.0f);
        b->bike.angAcc.x -= e.x * t;
        b->bike.angAcc.y -= e.y * t;
        b->bike.angAcc.z -= e.z * t;
    }
}

void bike_integrate_velocity(Rider *param_1)
{
    f32 len;
    f32 r;

    param_1->bike.vel.x = param_1->bike.vel.x + param_1->bike.acc.x * gPhysDeltaTime;
    param_1->bike.vel.y = param_1->bike.vel.y + param_1->bike.acc.y * gPhysDeltaTime;
    param_1->bike.vel.z = param_1->bike.vel.z + param_1->bike.acc.z * gPhysDeltaTime;
    param_1->bike.angVel.x = param_1->bike.angVel.x + param_1->bike.angAcc.x * gPhysDeltaTime;
    param_1->bike.angVel.y = param_1->bike.angVel.y + param_1->bike.angAcc.y * gPhysDeltaTime;
    param_1->bike.angVel.z = param_1->bike.angVel.z + param_1->bike.angAcc.z * gPhysDeltaTime;
    mtx3f_mul_vec3(&param_1->bike.angVel.x, &param_1->bike.right.x, &param_1->localAngVel.x);
    len = param_1->bike.vel.x * param_1->bike.vel.x
        + param_1->bike.vel.y * param_1->bike.vel.y
        + param_1->bike.vel.z * param_1->bike.vel.z;
    param_1->speed = __builtin_sqrtf(len);
    r = vec3_dot(&param_1->bike.vel.x, &param_1->bike.fwd.x);
    param_1->fwdSpeed = r;
    if (param_1->crashState == 0 && param_1->anim != 0xF) {
        cam_calc_bike_yaw(param_1);
    }
}

typedef struct {
    f32 loc[3];
    f32 pad0;
    f32 zv[3];
    f32 pad1[5];
    f32 one[2];
} Locals;

void bike_integrate_position(Rider *b)
{
    Locals L;
    f32 c;
    f32 k;
    s32 n;
    s32 id;
    s32 i;

    c = gPhysDeltaTime * 0.5f;
    L.zv[0] = L.zv[1] = L.zv[2] = 0.0f;
    k = -b->localAngVel.z * c;
    L.loc[0] = b->bike.right.x * k;
    L.loc[1] = b->bike.right.y * k;
    L.loc[2] = b->bike.right.z * k;
    b->bike.vel.x = L.loc[0] + b->bike.vel.x;
    b->bike.vel.y = L.loc[1] + b->bike.vel.y;
    b->bike.vel.z = L.loc[2] + b->bike.vel.z;
    n = 8;
    if (b->frontWheel.airTicks <= 0 && b->rearWheel.airTicks <= 0) {
        n = 6;
    }
    phys_body_step(&b->bike.pos.x, sBikeContactPoints, n, 0);
    b->bike.flags |= 1;
    id = b - gRiders;
    b->bike.vel.x -= L.loc[0];
    b->bike.vel.y -= L.loc[1];
    {
        f32 t2 = b->bike.vel.z;
        L.one[0] = 1.0f;
        b->bike.vel.z = t2 - L.loc[2];
    }
    if (id == (gRaceFrame & 7)) {
        u8 *tb = sBikeWallCheckPairs;
        u8 *t = (u8 *)(((gRaceFrame >> 2) & 6) + (s32)tb);
        if (height_raycast_walls(&b->bike.contacts[t[0]].x, &b->bike.contacts[t[1]].x, gContactNormal, L.one, 0)) {
            bike_crash_begin(b, 0x1C);
        }
    }
}

void bike_update_render_pose(Rider *b)
{
    f32 tmp[3];
    f32 q[4];
    f32 o[3];
    f32 r1[4];
    f32 r2[4];
    f32 x, y, m, d, len, ang, s, s2, c, k;
    s32 v, r;

    x = b->frontWheel.suspLength;
    y = b->rearWheel.suspLength;
    if (b->crashState == 0) {
        mtx3f_mul_vec3(&b->frontWheel.normal.x, &b->bike.right.x, tmp);
        x += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
        mtx3f_mul_vec3(&b->rearWheel.normal.x, &b->bike.right.x, tmp);
        y += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
    }
    m = 0.0f;
    b->drawSuspFront = x / 0.001f;
    b->drawSuspRear = y / 0.001f;
    if (x > 0.2f) {
        b->drawSuspFront = 200;
        m = 0.2f - x;
    }
    if (y > 0.2f) {
        d = 0.2f - y;
        b->drawSuspRear = 200;
        if (d < m) {
            m = d;
        }
    }
    m += 0.5f;
    if (y < -0.1f) {
        b->drawSuspRear = -100;
    }
    b->drawSuspFront = (f32)b->drawSuspFront + 50.0f;
    b->drawPos.x = (b->bike.pos.x - b->bike.up.x * m) * 1000.0f;
    b->drawPos.y = (b->bike.pos.y - b->bike.up.y * m) * 1000.0f;
    b->drawPos.z = (b->bike.pos.z - b->bike.up.z * m) * 1000.0f;
    b->drawSteer = b->steer * 32768.0f / 3.1415927f;
    *(Quat_2 *)b->drawQuat = *(Quat_2 *)b->bike.quat;
    if (b->crashState == 0) {
        quat_to_angles(b->bike.quat, &b->rot[0], &b->rot[1], &b->rot[2]);
        if (b->anim == -1 || b->anim == 15) {
            v = b->rot[0];
            if (v > 0x3000) {
                bike_crash_begin(b, 0x19);
            } else if (v < -0x3000) {
                bike_crash_begin(b, 0x1C);
            }
        }
        bike_update_trick_offset(b);
        if (b->trickBlend != 0.0f) {
            q[0] = (1.0f - b->trickBlend) + b->trickQuat[0] * b->trickBlend;
            q[1] = b->trickQuat[1] * b->trickBlend;
            q[2] = b->trickQuat[2] * b->trickBlend;
            q[3] = b->trickQuat[3] * b->trickBlend;
            len = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
            q[0] /= len;
            q[1] /= len;
            q[2] /= len;
            q[3] /= len;
            quat_mul(b->drawQuat, b->bike.quat, q);
            vec3_mul_mtx3f(&b->trickOffset.x, &b->bike.right.x, o);
            b->drawPos.x = (f32)b->drawPos.x + o[0] * (b->trickBlend * 1000.0f);
            b->drawPos.y = (f32)b->drawPos.y + o[1] * (b->trickBlend * 1000.0f);
            b->drawPos.z = (f32)b->drawPos.z + o[2] * (b->trickBlend * 1000.0f);
        }
        quat_to_angles(b->drawQuat, &b->drawRot[0], &b->drawRot[1], &b->drawRot[2]);
        if (b->isCpu != 0) {
            v = b->drawRot[1];
            if (v > 0x1800) {
                r = 0;
            } else if (v > 0xC00) {
                r = 0x1800 - v;
            } else if (v >= -0xBFF) {
                r = b->rot[1];
            } else if (v >= -0x17FF) {
                r = -0x1800 - v;
            } else {
                r = 0;
            }
            ang = r;
            ang *= 3.1415927f;
            ang *= 3.0517578e-05f;
            ang *= 0.5f;
            c = cosf(ang);
            s = sinf(ang);
            r1[0] = c;
            r1[1] = s * b->bike.fwd.x;
            r1[2] = s * b->bike.fwd.y;
            r1[3] = s * b->bike.fwd.z;
            quat_mul(b->drawQuat, r1, b->drawQuat);
            quat_to_angles(b->drawQuat, &b->drawRot[0], &b->drawRot[1], &b->drawRot[2]);
        } else if ((b->input & 0x20000) && gRaceFrame - b->turboPressFrame < 0x3C &&
                   (b->rearWheel.surface->flags & 4) && b->airFrames < 0 && b->speed < 5.0f) {
            if (b->speed > 2.0f) {
                k = (f32)(b->turboPressFrame - (gRaceFrame - 0x3C)) * 6.1035155e-07f / 60.0f;
            } else {
                k = 1.2207031e-06f;
            }
            ang = gCosTable[((((gRaceFrame - b->turboPressFrame) << 13) >> 4) - 0x400) & 0xFFF];
            ang *= k;
            r2[0] = cosf(ang);
            s2 = sinf(ang);
            r2[1] = s2 * b->bike.up.x;
            r2[2] = s2 * b->bike.up.y;
            r2[3] = s2 * b->bike.up.z;
            quat_mul(b->drawQuat, r2, b->drawQuat);
            quat_to_angles(b->drawQuat, &b->drawRot[0], &b->drawRot[1], &b->drawRot[2]);
        }
    } else {
        quat_to_mtx3f(&b->body.right.x, b->body.quat);
        b->drawBodyPos.x = b->body.pos.x * 1000.0f;
        b->drawBodyPos.y = b->body.pos.y * 1000.0f;
        b->drawBodyPos.z = b->body.pos.z * 1000.0f;
    }
    b->drawSteer = b->steer * 32768.0f / 3.1415927f;
    quat_to_mtx3f(&b->bike.right.x, b->bike.quat);
}

void bike_handle_landing(Rider *a0)
{
    Rider *s1 = a0;
    f32 *s0 = &s1->bike.vel.x;
    f32 fs0, d;
    s32 iv, v1;

    fs0 = vec3_dot(s0, &s1->rearWheel.normal.x) + vec3_dot(s0, &s1->frontWheel.normal.x);
    if ((u8)s1->anim < 0x10 && s1->airFrames >= 0xB)
        bike_crash_begin(s1, 0x1C);
    s1->rHeld = 0;
    if (fs0 < -10.0f)
        s1->wheelieTimer = 0;
    if (gGame.mode == 0xF) {
        s1->landing = sBikeTuneLanding;
        s1->bike.angVel.y = 0.0f;
    }
    if (s1->airFrames < 0x29)
        goto L1c0;
    d = s1->frontWheel.groundDist - s1->rearWheel.groundDist;
    s1->prevJumpDist = s1->jumpDist;
    if (d < -2.0f) {
        bike_crash_begin(s1, 0x19);
        goto L178;
    }
    if (d < -0.8f) {
        s1->pendingCrash = 0x19;
        goto L160;
    }
    if (d < -0.3f) {
        s1->roughLanding = 1;
        goto L178;
    }
    if (d < 0.3f) {
        s1->landingAssist = 1;
        goto L178;
    }
    if (d < 0.6f) {
        s1->roughLanding = 1;
        goto L178;
    }
    s1->pendingCrash = 0x1C;
L160:
    s1->crashVel = s1->bike.vel;
L178:
    if (s1->pendingCrash != 0 && -10.0f < fs0) {
        s1->pendingCrash = 0;
        s1->roughLanding = 1;
    }
    if (s1->landingAssist != 0) {
        f32 zf = 0.0f;
        s1->bike.angVel.z = zf;
        s1->bike.angVel.y = zf;
        s1->bike.angVel.x = zf;
    }
L1c0:
    if (s1->airFrames < 0x15)
        goto END;
    iv = (s32)(-fs0 * 10.0f);
    s1->landingImpact = iv;
    if (iv < 0)
        s1->landingImpact = 0;
    if (s1->landingImpact >= 0x100)
        s1->landingImpact = 0xFF;
    v1 = gRaceFrame;
    s1->impactFlags = 0xFF;
    s1->steer = 0.0f;
    s1->landingFrame = v1;
END:
    s1->airFrames = 0;
}

f32 bike_predict_landing(Rider *p, f32 g, f32 t, f32 *out)
{
    f32 diff, prev, v, step, lim, w;

    out[0] = p->bike.pos.x + p->bike.vel.x * t;
    out[1] = p->bike.pos.y + (p->bike.vel.y + g * t * 0.5f) * t;
    w = p->bike.pos.z + p->bike.vel.z * t;
    out[2] = w;
    if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else w = 3.0f;
        diff = (f32)height_get_ground(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
    v = p->bike.vel.y + g * t;
    if (0.0f < v ? v < 0.1f : -v < 0.1f) v = 0.1f;
    v = 1.0f / v;
    if (10.0f < v) v = 10.0f;
    else if (v < -10.0f) v = -10.0f;
    lim = 0.1f;
    while (lim < diff || diff < -lim) {
        step = diff * v;
        if (t + step < 0.0f) step = -t;
        t += step;
        prev = diff;
        out[1] = p->bike.pos.y + (p->bike.vel.y + g * t * 0.5f) * t;
        out[0] = p->bike.pos.x + p->bike.vel.x * t;
        w = p->bike.pos.z + p->bike.vel.z * t;
        out[2] = w;
        if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else if (out == (f32 *)p) w = 4.0f; else if (out == &p->bike.pos.x) w = 5.0f; else if (out == &p->bike.pos.y) w = 6.0f; else w = 3.0f;
                diff = (f32)height_get_ground(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
        if (prev == diff) return t;
        v = step / (prev - diff);
        if (10.0f < v) v = 10.0f;
        else if (v < -10.0f) v = -10.0f;
        lim += 0.05f;
    }
    if (p->speed != 0.0f) {
        out[0] = out[0] - p->bike.vel.x * (0.5f / p->speed);
        out[2] = out[2] - p->bike.vel.z * (0.5f / p->speed);
    }
    if (out == 0) diff = 1.0f; else if (p == 0) diff = 2.0f; else diff = 3.0f;
        out[1] = (f32)height_get_ground(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    if (t < 0.0f) t = 0.0f;
    return t;
}

void bike_align_to_ground(Rider *b)
{
    Vec3f n;
    Vec3f c;
    Vec3f u;
    Vec3f v;
    f32 len;
    f32 q;
    s32 t, w;
    f32 th, qd;

    v = b->bike.pos;
    th = v.x + b->bike.vel.x * b->landingTime;
    v.x = th;
    qd = v.z + b->bike.vel.z * b->landingTime;
    v.z = qd;
    if (b == 0) q = 1.0f; else if (t == w) q = 2.0f; else q = 3.0f;
    height_get_ground((s32)(v.x * 1000.0f), (s32)(v.y * 1000.0f), (s32)(v.z * 1000.0f));
    height_get_ground_normal(&n.x);
    if (n.y < 0.5f) {
        return;
    }
    c.x = b->bike.up.y * n.z - n.y * b->bike.up.z;
    c.y = b->bike.up.z * n.x - n.z * b->bike.up.x;
    c.z = b->bike.up.x * n.y - n.x * b->bike.up.y;
    len = vec3_dot(&c.x, &b->bike.right.x);
    c.x = b->bike.right.x * len;
    c.y = b->bike.right.y * len;
    c.z = b->bike.right.z * len;
    if (len == 0.0f) {
        return;
    }
    t = math_asini((s32)(len * 16384.0f));
    q = (f32)(s16)t * 3.1415927f * 3.0517578e-05f;
    th = b->landingTime;
    th += 0.1f;
    qd = (q + q) / th;
    if (qd > 2.0f) {
        qd = 2.0f;
    } else if (qd < -2.0f) {
        qd = -2.0f;
    }
    qd = qd / len;
    b->bike.angVel.x = c.x * qd;
    b->bike.angVel.y = c.y * qd;
    b->bike.angVel.z = c.z * qd;
}

void bike_update_wheelie(Rider *e)
{
    s8 c;

    if (e->airFrames > 0x3C && (*(s32 *)&e->crestAhead & 0xFFFF00) == 0) {
        e->wheelieTimer = 0;
    }
    if (e->wheelieTimer == 0) {
        if (e->airFrames < 0) {
            if (e->frontWheel.airTicks < 0 && e->speed > 7.5f && (e->frontWheel.surface->flags & 0x20)) {
                bike_crash_begin(e, 0x1C);
            }
            if ((e->input & 0x20000) && !(e->prevInput & 0x20000) && abs(e->stickX) < 3 &&
                e->fwdSpeed > -1.0f && e->stickY < -4) {
                e->wheelieTimer = 0x10;
                e->wheelieBalance = 0;
            }
        }
    } else {
        if (e->rearWheel.airTicks <= 0) {
            if (e->rearWheel.surface->flags & 0x20) {
                e->wheelieTimer = 0xF;
            }
            if (e->rearWheel.airTicks <= 0 && vec3_dot(&e->bike.up.x, &e->rearWheel.normal.x) < 0.5f) {
                bike_crash_begin(e, 0x19);
            }
        }
        if (e->wheelieTimer < 0x10) {
            e->wheelieTimer--;
        } else {
            if (e->wheelieTimer >= 0x3D && e->airFrames <= 0) {
                c = e->stickY;
                if (c < -12) {
                    e->wheelieBalance += 0x20;
                } else if (c >= -3) {
                    e->wheelieBalance -= 0x20;
                }
                e->wheelieBalance -= e->wheelieBalance / 64;
                if (e->wheelieBalance > 0x400) {
                    bike_crash_begin(e, 0x19);
                    e->bike.angVel.x -= e->bike.right.x * 5.0f;
                    e->bike.angVel.y -= e->bike.right.y * 5.0f;
                    e->bike.angVel.z -= e->bike.right.z * 5.0f;
                } else if (e->wheelieBalance < -0x400) {
                    e->wheelieTimer = 0xF;
                }
                if ((e->stickY < -12 || e->stickY > -4) && (f32)e->airFrames < 3.0f) {
                    if (e->rightFootOut < 0x10) e->rightFootOut++;
                    if (e->leftFootOut < 0x10) e->leftFootOut++;
                } else {
                    if (e->rightFootOut != 0) e->rightFootOut--;
                    if (e->leftFootOut != 0) e->leftFootOut--;
                }
            } else {
                e->leftFootOut = 0;
                e->rightFootOut = 0;
            }
            e->wheelieTimer++;
            if (e->wheelieTimer == 0xF0) {
                e->fistPump = 1;
            }
            if (e->stickY >= 0) {
                e->wheelieTimer = 0xF;
            }
        }
    }
    if (e->isCpu && e->wheelieTimer != 0) {
        e->wheelieTimer = 0;
    }
}

void bike_update_jump(Rider *b) {
    volatile s32 pad[12];
    Vec3f t;
    Vec3f c;
    Vec3f p;
    f32 x, z, len;
    f32 fs2, fs3, fs4;
    f32 sp, th, e, d;
    f32 s1v, t1v, d1, g, m, c2, d2;
    s32 n, r;
    f32 k;

    if (b->roughLanding != 0) {
        b->roughLanding += ((u8 *)&gPhysTicks)[3];
        if (b->roughLanding >= 60) {
            b->roughLanding = 0;
        }
    }
    if (b->jumpState == 1 && (b->crashState != 0 || b->anim != -1)) {
        b->jumpState = -1;
    }
    if (b->rearWheel.airTicks > 0 && b->frontWheel.airTicks > 0) {
        if (b->airFrames <= 0) {
            b->unk_740 = 0;
            b->landingTime1 = (b->bike.vel.y + b->bike.vel.y) / 10.106251f;
            b->landingTime2 = (b->bike.vel.y + b->bike.vel.y) / 19.29375f;
            b->landingTime = (b->bike.vel.y + b->bike.vel.y) / 14.700001f;
            stunt_on_takeoff(b);
            s1v = b->speed;
            t1v = 0.5f;
            if (!(s1v > 20.0f)) {
                t1v = 0.25f;
                if (s1v > 10.0f) {
                    t1v = (s1v - 10.0f) / 40.0f + 0.25f;
                }
            }
            if (t1v < b->landingTime) {
                if (b->wheelieTimer != 0 && b->wheelieTimer < 45) {
                    b->wheelieTimer = 0;
                }
                d1 = vec3_dot(&b->bike.right.x, &b->bike.angVel.x);
                b->bike.angVel.x -= d1 * b->bike.right.x;
                b->bike.angVel.y -= d1 * b->bike.right.y;
                b->bike.angVel.z -= d1 * b->bike.right.z;
                if (b->isCpu == 0 && b->crashState == 0 && gRaceFrame - b->turboPressFrame < 10 && (b->input & 0x20000)) {
                    sp = b->speed;
                    if (sp < 20.0f && b->engine.overheated == 0) {
                        th = 5.0f;
                        if (sp > 10.0f) {
                            th = 5.0f - (sp - 10.0f) * 0.5f;
                        }
                        if (sp != 0.0f) {
                            sp = th / sp + 1.0f;
                        } else {
                            sp = 0.0f;
                        }
                        b->bike.vel.x *= sp;
                        b->bike.vel.y *= sp;
                        b->bike.vel.z *= sp;
                        if (b->rot[0] >= -0x17FF) {
                            b->bike.angVel.x -= b->bike.right.x * 3.0f;
                            b->bike.angVel.y -= b->bike.right.y * 3.0f;
                            b->bike.angVel.z -= b->bike.right.z * 3.0f;
                        }
                        snd_play_rider_sfx_pitch(7, b - gRiders, 0xC0, 0xC0, 1);
                    }
                }
                if (gGame.mode == 6) {
                    Vec3f *q = &b->takeoffPos;
                    if (b->speed > 4.0f) {
                    th = q->x; sp = q->z; if (b == 0) r = 0; height_get_ground(th * 1000.0f, q->y * 1000.0f, sp * 1000.0f);
                    if (gSurfaceTable[(gGroundSurface >> 4) & 0xF].flags & 0x400) {
                        f32 k;
                        k = 10.0f;
                        g = sBikeTuneRampBoost * k / b->speed + 1.0f;
                        b->bike.vel.x *= g;
                        b->bike.vel.y *= g;
                        b->bike.vel.z *= g;
                    }
                    }
                }
            }
            t.x = b->bike.vel.x * b->landingTime1;
            t.y = b->bike.vel.y * b->landingTime1;
            t.z = b->bike.vel.z * b->landingTime1;
            b->landingPos1.x = t.x + b->bike.pos.x;
            b->landingPos1.y = t.y + b->bike.pos.y;
            b->landingPos1.z = t.z + b->bike.pos.z;
            t.x = b->bike.vel.x * b->landingTime2;
            t.y = b->bike.vel.y * b->landingTime2;
            t.z = b->bike.vel.z * b->landingTime2;
            b->landingPos2.x = t.x + b->bike.pos.x;
            b->landingPos2.y = t.y + b->bike.pos.y;
            b->landingPos2.z = t.z + b->bike.pos.z;
            b->landingAssist = 0;
            b->unk_622 = 0;
            {
                u16 h = (u16)b->segment;
                b->landingSegment2 = h;
                b->landingSegment1 = h;
            }
            b->airFrames = gPhysTicks;
        } else {
            b->airFrames += gPhysTicks;
        }
        if (b->airFrames >= 4) {
            b->landingTime2 = bike_predict_landing(b, -19.29375f, b->landingTime2, &b->landingPos2.x);
            height_get_ground_normal(&b->landingNormal2.x);
            b->landingTime1 = bike_predict_landing(b, -10.106251f, b->landingTime1, &b->landingPos1.x);
            height_get_ground_normal(&b->landingNormal1.x);
            b->landingTime = (b->landingTime1 + b->landingTime2) * 0.5f;
            if (b->input & 0x10000) {
                n = b->stickX;
                if (b->anim == -1 && b->landingTime > 0.5f) {
                    if (n > 10) {
                        stunt_start(b, 16);
                    } else if (n < -10) {
                        stunt_start(b, 17);
                    }
                    if (b->anim != -1) {
                        b->animSpeed = (f32)(gRiderAnims[b->anim].body->count << 9) / (b->landingTime * 60.0f);
                    }
                }
                if (b->anim == 16) {
                    if (n <= 0) {
                        n = 1;
                    }
                    b->unk_622 = n << 9;
                } else if (b->anim == 17) {
                    if (n >= 0) {
                        n = -1;
                    }
                    b->unk_622 = n << 9;
                }
            }
            if (b->isCpu == 0) {
                if (b->anim == -1 && !(b->input & 0x100)) {
                    m = 0.015625f;
                    if (b->stickY < 0) {
                        m = b->airFloat;
                    }
                    b->airPitch = b->stickY * m;
                    if (b == 0) r = 0;
                    b->airPitchInput = b->stickY;
                } else {
                    b->airPitchInput = 0;
                    b->airPitch = b->airFloat * -15.0f;
                }
            } else {
                n = 5;
                if (b->airFrames >= 6) {
                    n = 1;
                }
                x = b->bike.vel.x;
                z = b->bike.vel.z;
                len = __builtin_sqrtf(x * x + z * z);
                if (len == 0.0f) {
                    len = 1.0f;
                }
                x /= len;
                z /= len;
                fs3 = b->landingPos2.y * 14.700001f;
                fs4 = (x * b->landingNormal2.x + z * b->landingNormal2.z) / b->landingNormal2.y;
                if (gGame.mode != 5) {
                    b->landingSegment2 = path_find_segment(&b->landingPos2, b->landingSegment2, n, b->pathSearch);
                    if (gPathNodes[b->landingSegment2].flags & 0x10) {
                        fs3 -= 1000.0f;
                    }
                }
                fs2 = b->landingPos1.y * 14.700001f;
                e = b->landingNormal2.x * b->bike.vel.x + b->landingNormal2.y * (b->bike.vel.y - b->landingTime2 * 17.640001f) + b->landingNormal2.z * b->bike.vel.z;
                x = (x * b->landingNormal1.x + z * b->landingNormal1.z) / b->landingNormal1.y;
                fs3 -= e * e;
                if (gGame.mode != 5) {
                    b->landingSegment1 = path_find_segment(&b->landingPos1, b->landingSegment1, n, b->pathSearch);
                    if (gPathNodes[b->landingSegment1].flags & 0x10) {
                        fs2 -= 500.0f;
                    }
                }
                e = b->landingNormal1.x * b->bike.vel.x + b->landingNormal1.y * (b->bike.vel.y - b->landingTime1 * 11.760001f) + b->landingNormal1.z * b->bike.vel.z;
                b->landingTime = (b->landingTime1 + b->landingTime2) * 0.5f;
                fs2 -= e * e;
                if (fs3 < fs2) {
                    b->landingPick = 1;
                    b->airPitch = -0.3125f;
                } else {
                    fs4 = x;
                    b->landingPick = 0;
                    b->airPitch = 0.3125f;
                }
                if (b->landingTime < 1.5f) {
                    r = fs4 * 15.0f + (f32)((s32)(fs4 * 8192.0f - (f32)b->drawRot[0]) / 512);
                    if (r > 15) {
                        r = 15;
                    } else if (r < -15) {
                        r = -15;
                    }
                    b->unk_740 = r;
                    b->airPitchInput = b->unk_740;
                }
            }
        }
        if (b->landingAssist != 0) {
            bike_align_to_ground(b);
        } else if (b->landingTime < 0.25f && b->bike.vel.y < 0.0f && b->airFrames >= 31 && b->wheelieTimer == 0) {
            if (b->isCpu == 0) {
                p = b->bike.pos;
                th = p.x + b->bike.vel.x * b->landingTime;
                p.x = th;
                m = p.z + b->bike.vel.z * b->landingTime;
                p.z = m;
                if (b == 0) e = 1.0f; else if (n == 0) e = 2.0f; else e = 3.0f;
                p.y = (f32)height_get_ground(p.x * 1000.0f, p.y * 1000.0f, p.z * 1000.0f) / 1000.0f;
                height_get_ground_normal(&t.x);
                c.x = b->bike.up.y * t.z - t.y * b->bike.up.z;
                c.y = b->bike.up.z * t.x - t.z * b->bike.up.x;
                c.z = b->bike.up.x * t.y - t.x * b->bike.up.y;
                d2 = vec3_dot(&c.x, &b->bike.right.x);
                c2 = b->landing * 10.0f;
                if (d2 > 0.0f ? d2 < c2 : -d2 < c2) {
                    b->landingAssist = 1;
                }
            } else {
                b->landingAssist = 1;
            }
        }
        if (b->jumpState != -1) {
            b->jumpState = 1;
            b->jumpDist = __builtin_sqrtf((b->bike.pos.x - b->takeoffPos.x) * (b->bike.pos.x - b->takeoffPos.x)
                                      + (b->bike.pos.y - b->takeoffPos.y) * (b->bike.pos.y - b->takeoffPos.y)
                                      + (b->bike.pos.z - b->takeoffPos.z) * (b->bike.pos.z - b->takeoffPos.z));
        }
    } else {
        if (b->airFrames > 0) {
            bike_handle_landing(b);
        } else {
            b->airFrames -= gPhysTicks;
        }
        if (b->jumpState == 1) {
            b->jumpState = 0;
        }
        b->takeoffPos = b->rearWheel.contact;
    }
    b->takeoffVelY = b->bike.vel.y;
}

void bike_update_alpha(Rider *a2)
{
    s32 idx;
    s32 v;
    u8 b;
    s8 c;
    s32 *base;
    s32 n;
    s32 *q;

    base = gRiderAlpha;
    idx = (a2 - gRiders);
    q = &base[idx];
    if (gGhostRace != 0 && gGhostSlot == idx) v = 0x80;
    else if (gDemoPlayback != 0) {
        if (idx == 0) v = 0x80; else { *q = 0xFF; goto L74; }
    } else v = 0xFF;
    *q = v;
L74:
    if (a2->crashState != 0 && a2->isCpu != 0 && a2->crashTimer > 0x58) {
        gRiderAlpha[a2 - gRiders] = (0x78 - a2->crashTimer) * 8;
        return;
    }
    if ((s8)a2->blinkCount != 0) {
        b = a2->blinkTimer - 1;
        a2->blinkTimer = b;
        if ((s32)((u32)b << 24) <= 0) {
            c = a2->blinkCount - 1;
            a2->blinkCount = c;
            if (c == 0 && gGhostRace == 0) {
                if (a2->isCpu == 0) {
                    if ((gCheatFlags & 0x100) == 0) a2->noCollide = 0;
                } else {
                    a2->noCollide = 0;
                }
            }
            n = (((s32)((u32)a2->blinkCount << 24)) >> 25) - 10;
            a2->blinkTimer = n;
            if (n < 4) a2->blinkTimer = 4;
        }
        if (a2->blinkCount & 1) gRiderAlpha[a2 - gRiders] = 0;
    }
}

void bike_update(Rider *b, s32 n)
{
    f32 t;

    gGroundHeight = 0;
    gGroundSurface = 0;
    gGroundTri = 0;
    gGroundArea = 0;
    gPhysTicks = n;
    gPhysDeltaTime = (f32)gPhysTicks * b->timeScale / 60.0f;
    if (gCrashSlowMotion != 0 && b->isCpu != 0) {
        gPhysDeltaTime *= 0.5f;
    }
    bike_update_alpha(b);
    particles_emit_water_splash(b);
    if (b->crashState == 0) {
        if (b->pendingCrash > 0) {
            b->input = 0;
            if (b->frontWheel.airTicks < 10 && b->rearWheel.airTicks < 10) {
                bike_crash_begin(b, b->pendingCrash);
            }
        }
        if (b->pendingCrash < 0) {
            bike_crash_begin(b, -b->pendingCrash);
        }
        bike_process_input(b, n);
        bike_update_wheelie(b);
        bike_update_jump(b);
        if (b->isCpu != 0) {
            ai_update_throttle(b);
            b->dirtSpray = 0;
            if (b->speed > 5.0f) {
                if (b->powerSlide != 0 || ((b->input & 0x20000) && gRaceFrame - b->turboPressFrame < 30)) {
                    b->dirtSpray = 2;
                } else if (b->rightFootOut | b->leftFootOut) {
                    b->dirtSpray = 1;
                }
            }
            ai_update_steering(b);
        } else {
            bike_update_steering(b, n);
        }
        if (gEvent.flags & 0x10) {
            b->unk_412 = 0x3000;
            b->input |= 0x20;
        }
        if (b->flags & 0x10) {
            if (b->fwdSpeed > 0.0f) {
                b->input = 0x20;
            } else {
                b->input = 0;
                b->stickX = 0;
                if (b - gRiders == 0) {
                    gEvent.flags |= 0x2000;
                }
            }
        }
        engine_update(b - gRiders, n);
        bike_update_drive_force(b);
        bike_update_brakes(b);
        bike_solve_tire_constraints(b);
        stunt_update(b);
        bike_update_tire_forces(b);
        bike_compute_forces(b);
        if (b->crashState == 0) {
            bike_integrate_position(b);
            bike_integrate_velocity(b);
            bike_wheel_surface_bumps(&b->frontWheel, &b->bumpSeed);
            bike_wheel_surface_bumps(&b->rearWheel, &b->bumpSeed);
            bike_update_suspension(b);
            switch (gGame.mode) {
            case 2:
            case 4:
            case 5:
            case 10:
            case 14:
                break;
            case 6:
                excite3d_update_progress(b);
                break;
            default:
                path_update_bike(b);
                break;
            }
            switch (b->finishState) {
            case 0:
                break;
            case 1:
                if (b->anim == -1 && b->speed < 8.0f && b->airFrames < -30) {
                    b->finishState = 2;
                    stunt_start(b, b->race.finishPlace + 0x13);
                    if (!(gGame.flags & 0x8000)) {
                        b->camMode = 10;
                    }
                }
                break;
            case 2:
                if (b->anim < 0) {
                    b->finishState = 3;
                    if (gGame.mode == 5) {
                        b->finishState = 0;
                        b->isCpu = 1;
                        b->camMode = (u16)b->savedCamMode;
                    }
                }
                break;
            }
        } else {
            bike_crash_update(b);
        }
    } else {
        b->input = 0;
        engine_update(b - gRiders, n);
        bike_crash_update(b);
    }
    bike_update_render_pose(b);
    particles_emit_sparks(b);
}

u32 bike_remap_buttons(u32 a0, s32 a1)
{
    s32 c;
    u32 u2;

    if (a0 & 0x8000) a0 |= 0x20000;
    if (a1 < gGame.numPlayers)
        c = (s8)gGame.padConfig[gGame.playerPad[a1]];
    else
        c = 0;

    switch (c) {
    case 0:
        return a0;
    case 1:
        u2 = a0 & 0xfffcffff;
        if (a0 & 0x10000) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x10000;
        return u2;
    case 2:
        u2 = a0 & 0xfffdffdf;
        if (a0 & 0x20) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x20;
        return u2;
    case 3:
        u2 = a0 & 0xfffcffdf;
        if (a0 & 0x20) u2 |= 0x10000;
        if (a0 & 0x20000) u2 |= 0x20;
        if (a0 & 0x10000) u2 |= 0x20000;
        return u2;
    }
    return a0;
}

void bikes_update(s32 n) {
    Rider *b;
    s32 i;
    s32 v;
    s32 u;
    s32 pad[2];
    f32 t, t2, tk, t2k;
    f32 x, y, z;

    if (gEvent.flags & 0x800) {
        return;
    }
    for (i = 0; i < 8; i++) {
        b = &gRiders[i];
        if (b->isCpu == 0) {
            b->prevInput = b->input;
            v = gPlayerInputs[i];
            b->input = v;
            if (gMirrorMode != 0 && (gGhostRace == 0 || i != gGhostSlot)) {
                b->input = v ^ 0xC;
            }
        } else {
            u = b->input;
            b->stickY = 0;
            b->stickX = 0;
            b->input = 0;
            b->prevInput = u;
        }
    }
    gRaceFrame += n;
    if (gDebugFreeze != 0) {
        for (b = gRiders; b < gRiders + 8; b++) {
            if (b->inactive == 0) {
                bike_update_render_pose(b);
            }
        }
    } else {
        for (b = gRiders; b < gRiders + 8; b++) {
            if (b->inactive != 0) {
                continue;
            }
            if (b->frozen != 0) {
                bike_update_render_pose(b);
                continue;
            }
            if (b->isCpu != 0 && (b->airFrames < -30 || (b->airFrames >= 11 && b->landingTime > 0.5f))
                && !(b == gRiders && (gGame.flags & 0x81))) {
                if (((gRaceFrame ^ (b - gRiders)) & 1) != 0) {
                    bike_update(b, 2);
                    b->bike.smoothPos = b->bike.pos;
                } else {
                    t = b->timeScale / 60.0f;
                    t2 = t * t * 0.5f;
                    x = b->bike.vel.x * t;
                    y = b->bike.vel.y * t;
                    z = b->bike.vel.z * t;
                    b->bike.smoothPos.x += x + b->bike.acc.x * t2;
                    b->bike.smoothPos.y += y + b->bike.acc.y * t2;
                    b->bike.smoothPos.z += z + b->bike.acc.z * t2;
                    t *= 1000.0f;
                    t2 *= 1000.0f;
                    b->drawPos.x = (f32)b->drawPos.x + (b->bike.vel.x * t + b->bike.acc.x * t2);
                    b->drawPos.y = (f32)b->drawPos.y + (b->bike.vel.y * t + b->bike.acc.y * t2);
                    b->drawPos.z = (f32)b->drawPos.z + (b->bike.vel.z * t + b->bike.acc.z * t2);
                }
            } else {
                for (i = 0; i < n; i++) {
                    bike_update(b, 1);
                }
                b->bike.smoothPos = b->bike.pos;
            }
        }
        switch (gGame.mode) {
        case 2:
            soccer_ball_update();
            break;
        case 5:
            desert_update_race();
            break;
        case 6:
            excite3d_update();
            break;
        case 7:
        case 9:
        case 10:
            break;
        case 8:
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

float bike_get_ground_height(float *p)
{
    float x = p[0];
    int r = height_get_ground((int)(x * 1000.0f), (int)(p[1] * 1000.0f), (int)(p[2] * 1000.0f));
    return (float)r / 1000.0f;
}

void bike_sync_wheel_contacts(Rider *p)
{
    s32 i;
    Vec3f out;

    sWheelLiftoffFrame = gRaceFrame;
    for (i = 6; i < 8; i++) {
        vec3_mul_mtx3f(&sBikeContactPoints_2[i].x, &p->bike.right.x, &out.x);
        p->bike.contacts[i].x = out.x + p->bike.pos.x;
        p->bike.contacts[i].y = out.y + p->bike.pos.y;
        p->bike.contacts[i].z = out.z + p->bike.pos.z;
    }
}

void bikes_free(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        mem_free(gRiders[i].bikeSkel);
        mem_free(gRiders[i].riderSkel);
    }
    ai_free();
    collide_free_node_pool();
    race_free_extra_rider_spawns();
}

void bike_apply_surface_bumps(Rider *p) {
    bike_wheel_surface_bumps(&p->frontWheel, &p->bumpSeed);
    bike_wheel_surface_bumps(&p->rearWheel, &p->bumpSeed);
}

void bike_update_finish_celebration(Rider *p)
{
    s32 st = p->finishState;

    if (st == 1) goto arm1;
    if (st < 2) return;
    if (st == 2) goto arm2;
    return;

arm1:
    if (p->anim == -1 && p->speed < 8.0f && p->airFrames < -30) {
        p->finishState = 2;
        stunt_start(p, p->race.finishPlace + 0x13);
        if ((gGame.flags & 0x8000) == 0) {
            p->camMode = 10;
        }
    }
    return;

arm2:
    if (p->anim < 0) {
        p->finishState = 3;
        if (gGame.mode == 5) {
            p->finishState = 0;
            p->isCpu = 1;
            p->camMode = (u16)p->savedCamMode;
        }
    }
}

void bike_update_dirt_spray(Rider *o) {
    o->dirtSpray = 0;
    if (5.0f < o->speed) {
        if (o->powerSlide != 0 || ((o->input & 0x20000) != 0 && (gRaceFrame - o->turboPressFrame < 0x1E))) {
            o->dirtSpray = 2;
            return;
        }
        if ((*(volatile u8 *)&o->rightFootOut | *(volatile u8 *)&o->leftFootOut) != 0) {
            o->dirtSpray = 1;
        }
    }
}
