#include "game.h"

extern CamMode gCamModes[12]; /* here: bikenew.c declares its own view */

void cam_init(s32 arg0)
{
    s32 i, j;
    s16 *order;

    mem_fill((u8 *)sCamShakeY, 0, sizeof(sCamShakeY));
    mem_fill((u8 *)sCamFrozen, 0, sizeof(sCamFrozen));
    order = mem_alloc_dbg(gPathNodeCount << 1, 0, (s32)D_800D6A70, 0x239);
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
        for (j = 0; j < 120; j++) {
            sCamPitchSprings[i].vel += (2000.0f - sCamPitchSprings[i].pos) * sCamSpringUp;
            sCamPitchSprings[i].pos += sCamPitchSprings[i].vel;
            sCamPitchSprings[i].vel *= sCamDampUp;
        }
    }
}

void cam_build_segment_order(void) {
    s32 j;
    s32 i;
    s32 n;

    i = 0;
    n = 0;
    do {
        sCamSegOrder[i] = n;
        if (gPathNodes[i].branch != -1) {
            j = gPathNodes[i].branch;
            n++;
            do {
                sCamSegOrder[j] = n;
                j = gPathNodes[j].next;
                n++;
            } while (gPathNodes[j].join == -1);
        } else {
            n++;
        }
        i = gPathNodes[i].next;
    } while (i != 0);
}

void cam_update(void) {
    CamOut out;
    Vec3f pos;
    s32 area;
    f32 hh;
    s32 changed, sel;
    f32 *s5;
    s32 m;
    s32 m2;
    s32 mode;
    s32 t, k;
    CamZoneState *e;
    u16 f;
    CamMode *cm;
    Rider *pl;
    f32 dx, dz, d;

    sel = 0;
    changed = 0;
    s5 = NULL;
    if (gPodiumMode != 0) {
        podium_update_camera();
        return;
    }
    if (gCamView < gGame.numPlayers && gGame.mode != 7 && gGame.mode != 2) {
        if (gRiders[gCamView].camMode != 10) {
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
        m2 = gEvent.countdown;
        if (m2 == 0xFF && gCamView == 0) {
            func_8004F5C8(0);
        }
        if ((gPlayerInputs[0] & 0x10) || gCurTrack == D_800CB7B0) {
            if (m2 >= 2) {
                m2 = 1;
            }
            gEvent.countdown = m2;
            changed = 1;
        }
    }
    if (gGame.flags & 0x10) {
        t = gGame.riders[gCamView];
        sCamTarget = t;
        cam_update_rider_select(&gRiders[t], &out, t != gCamTargets[gCamView]);
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
        if (gGame.mode == 9 && (gEvent.flags & 0x80)) {
            changed = 1;
            gEvent.flags &= ~0x80;
        }
        if (cam_find_zone() != 0) {
            gCamZoneState[gCamView].flags |= 2;
        } else {
            gCamZoneState[gCamView].flags &= ~2;
        }
        if (D_800EE140 != 0) {
            func_8002D888(&gRiders[sCamTarget], &out);
        } else {
            if ((gReplayPlayback != 0 || gCreditsMode != 0 || (gGame.flags & 0x81)) &&
                (gCamZoneState[gCamView].flags & 2)) {
                if (!(gCamZoneState[gCamView].flags & 1)) {
                    gCamZoneState[gCamView].flags |= 1;
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
                { CamZoneState *bs = gCamZoneState; e = &bs[gCamView]; }
                f = e->flags;
                if (f & 1) {
                    mode = e->type;
                    e->flags = f & 0xFFFE;
                    if (mode < 2) if (mode >= 0) {
                        changed = 1;
                    }
                    m = gRiders[sCamTarget].camMode;
                    sel = 1;
                } else {
                    k = gCamView;
                    if (gGhostRace != 0 && gGhostSlot == 0) {
                        k = 1;
                    }
                    if ((gPlayerInputs[k] & 0x80) && !(gPlayerInputsPrev_3[k] & 0x80) && !(gEvent.flags & 0x800) &&
                        gEvent.state != 0 && gEvent.allFinished == 0) {
                        s32 md = gRiders[sCamTarget].camMode;
                        if (md != 9 && gGame.mode != 7) {
                            sel = 1;
                            { Rider *b = (Rider *)((u32)gRiders + ((md & 1) >> 1)); s5 = &b[sCamTarget].bike.vel.x; }
                            m = md;
                            if (gGame.mode == 6 && gRiders[sCamTarget].finishState == 0) {
                                m ^= 6;
                            } else {
                                do {
                                    m++;
                                    if (m >= 12) {
                                        m = 0;
                                    }
                                } while (gCamModes[m].flags & 8);
                            }
                        }
                    }
                }
            }
            if (sel) {
                cm = &gCamModes[gRiders[sCamTarget].camMode];
                if (cm->flags & 2) {
                    changed = 1;
                }
                if (cm->exit != NULL) {
                    cm->exit();
                }
                pl = &gRiders[sCamTarget];
                pl->savedCamMode = m;
                pl->camMode = m;
                gCamLastModes[gCamView] = (s16)m;
                cm = &gCamModes[pl->camMode];
                if (cm->flags & 1) {
                    changed = 1;
                }
                if (cm->init != NULL) {
                    cm->init();
                }
            }
            {
            if (!(gCamZoneState[gCamView].flags & 1)) {
                cm = &gCamModes[gRiders[sCamTarget].camMode];
                if (gInsetViewActive != 0 && gCamView == 1) {
                    cm = &gCamModes[7];
                }
                if (cm->update != NULL) {
                    cm->update(&gRiders[gCamTargetOverride != -1 ? gCamTargetOverride : sCamTarget], &out);
                    if (changed) {
                        if (s5 != NULL) {
                            cam_snap_to_moving(&out, s5);
                        } else {
                            cam_snap_to(&out);
                        }
                    }
                }
            }
            }
        }
    }
    t = gCamView;
    gCamPos[t].x = sCamHistorySum[t].x / 16;
    gCamPos[t].y = sCamHistorySum[t].y / 16 + sCamShakeY[t];
    gCamPos[t].z = sCamHistorySum[t].z / 16;
    gCamPitch[t] = sCamHistorySum[t].pitch;
    if (sCamShakeY[t] != 0) {
        dx = (f32)gRiders[t].drawPos.x - (f32)gCamPos[t].x;
        dz = (f32)gRiders[t].drawPos.z - (f32)gCamPos[t].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        gCamPitch[gCamView] += (s32)((f32)sCamShakeY[gCamView] / d * 32768.0f / 3.1415927f);
    }
    gCamRoll[gCamView] = sCamHistorySum[gCamView].roll;
    gCamYaw[gCamView] = sCamHistorySum[gCamView].yaw;
    sCamFrameCount++;
    if (!(gCamZoneState[gCamView].flags & 1) && gRiders[gCamView].camMode != 9) {
        gCamSegFlags[gCamView] = 0;
        cam_clamp_to_track(sCamTarget, gCamView);
        return;
    }
    pos.x = (f32)gCamPos[gCamView].x / 1000.0f;
    pos.y = (f32)gCamPos[gCamView].y / 1000.0f;
    pos.z = (f32)gCamPos[gCamView].z / 1000.0f;
    path_get_segment_and_t(&pos, &area, &hh, gRiders[sCamTarget].segment);
    gCamSegFlags[gCamView] = gPathNodes[area].flags;
}

f32 cam_calc_ground_pitch(Rider *self)
{
    s32 i;
    s32 k;
    s32 n;
    f32 arr[3];
    f32 f0;
    f32 f3;
    f32 t;
    f32 dx, dz;
    f32 q1, q2;

    i = self->segment;
    f3 = 0.0f;
    if ((gPathNodes[i].flags & 1) == 0) {
        f0 = self->bike.vel.x * self->bike.fwd.x + self->bike.vel.z * self->bike.fwd.z;
        if (f0 < 2.0f) {
            f0 = 2.0f;
        }
        if (16.0f < f0) {
            f0 = 16.0f;
        }
        arr[0] = self->rearWheel.contact.x + self->bike.fwd.x * f0;
        arr[2] = self->rearWheel.contact.z + self->bike.fwd.z * f0;
        arr[1] = self->rearWheel.contact.y;
        t = bike_get_ground_height(arr);
        if (gGroundFound != 0) {
            f3 = (self->rearWheel.contact.y - t) * (8192.0f / f0);
        }
    } else {
        k = i;
        for (n = 5; n >= 0; n--) {
            k = gPathNodes[k].next;
        }
        dx = gPathNodes[k].pos.x - self->rearWheel.contact.x;
        dz = gPathNodes[k].pos.z - self->rearWheel.contact.z;
        t = __builtin_sqrtf(dx * dx + dz * dz);
        f3 = -(gPathNodes[k].pos.y - self->rearWheel.contact.y) * (8192.0f / t);
    }
    if (f3 < -2048.0f) {
        f3 = -2048.0f;
    } else if (2048.0f < f3) {
        f3 = 2048.0f;
    }
    if (self->wheelieTimer == 0) {
        f3 = f3 + 2000.0f;
    } else {
        f3 = f3 + 0.0f;
    }
    q1 = self->bike.vel.x;
    q2 = self->bike.vel.z;
    f0 = __builtin_sqrtf(q1 * q1 + q2 * q2);
    if (f0 == 0.0f) {
        f0 = 1.0f;
    }
    q1 = q1 / f0;
    q2 = q2 / f0;
    f0 = self->rearWheel.normal.x * q1 + self->rearWheel.normal.z * q2;
    if (0.2f < f0) {
        f0 = (f32)((s16)math_asini((s32)(f0 * 16384.0f)) + 0x400);
        if (f3 < f0) {
            f3 = f0;
        }
    }
    return f3;
}

f32 cam_calc_air_pitch(Rider *p)
{
    f32 fVar2, fVar3;
    f32 fVar4, r;
    f32 fVar5;
    f32 A[3];
    f32 B[3];
    s32 iVar1;
    f32 tt;

    if (p->airFrames > 0 && 0.25f < p->landingTime) {
        fVar4 = 0.5f;
        A[0] = (p->landingPos1.x + p->landingPos2.x) * fVar4 - p->bike.smoothPos.x;
        A[1] = (p->landingPos1.y + p->landingPos2.y) * fVar4 - p->bike.smoothPos.y;
        A[2] = (p->landingPos1.z + p->landingPos2.z) * fVar4 - p->bike.smoothPos.z;
        fVar2 = math_atan2f(__builtin_sqrtf(A[0] * A[0] + A[2] * A[2]) + 1.0f, -A[1] - 1.0f);
        fVar2 = (fVar2 * 32768.0f) / 3.1415927f;
        if (p->bike.vel.y < 0.0f) {
            B[0] = p->landingNormal1.x + p->landingNormal2.x;
            B[1] = p->landingNormal1.y + p->landingNormal2.y;
            B[2] = p->landingNormal1.z + p->landingNormal2.z;
            fVar3 = vec3_dot(B, A);
            r = fVar3 * fVar4;
            r = r / __builtin_sqrtf(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
            if (-0.25f < r) {
                tt = r + 0.25f;
                iVar1 = math_asini((s32)(tt * 16384.0f));
                fVar2 = fVar2 + (f32)(s32)(s16)iVar1;
            }
        }
        sCamAirPitch[gCamView] = fVar2;
    } else if (p->airFrames >= -31) {
        fVar2 = sCamAirPitch[gCamView];
    } else {
        fVar2 = 2000.0f;
    }
    return fVar2;
}

void cam_update_rider_select(Rider *p, CamOut *out, s32 arg2)
{
    s32 buf[10];
    s32 in[3];
    s32 res[3];
    s32 amp, yoff;
    s32 r, z;
    s32 *e;
    Game *g = &gGame;

    if (g->flags & (0x400 << gCamView)) {
        amp = 4000;
        yoff = 0x672;
    } else {
        amp = 6000;
        if (g->numPlayers == 2 && gSplitVertical != 0) {
            amp = 7200;
        }
        yoff = 0x8FC;
    }
    e = gGame.riderSelectRot[gCamView];
    out->x = p->bike.smoothPos.x * 1000.0f + (f32)((gCosTable[((e[0] >> 4) - 0x400) & 0xFFF] * amp) >> 14);
    out->y = p->bike.smoothPos.y * 1000.0f + (f32)yoff;
    r = 0;
    out->z = p->bike.smoothPos.z * 1000.0f + (f32)((gCosTable[(e[0] >> 4) & 0xFFF] * amp) >> 14);
    z = 0;
    out->pitch = 0xE10;
    out->roll = 0;
    out->yaw = (0x8000U - e[0]) & 0xFFFF;
    mtx3i_from_angles(buf, out->pitch, out->roll, out->yaw);
    switch (gGame.numPlayers) {
    case 1:
        switch (gGame.riderSelectState[0]) {
        case 1:
            r = 0x3E - gGame.riderSelectAnim[0];
            break;
        case 2:
            r = 0x3E;
            break;
        case 3:
            r = gGame.riderSelectAnim[0];
            break;
        }
        break;
    case 2:
        r = 0x19;
        if (gSplitVertical != 0) {
            z = -0x226;
        } else {
            r = -0x1E;
            z = -0x64;
        }
        break;
    default:
        r = 0x1E;
        z = -0x1F4;
        break;
    }
    r = (r * -0x564) / 0x3E;
    if (gGame.flags & (0x400 << gCamView)) {
        in[0] = 0;
        in[1] = 0;
        in[2] = 0;
    } else {
        in[0] = r + 600;
        in[1] = z;
        in[2] = 0;
    }
    vec3i_mul_mtx3i(in, buf, res);
    out->x += res[0];
    out->y += res[1];
    out->z += res[2];
    cam_snap_to(out);
}

int cam_calc_bike_yaw(Rider *s)
{
    struct { u16 a[2]; s32 b; u16 c[4]; } L;
    s32 iv1, iv2, a;

    if (s->crashState != 0) goto done;
    math_to_polar((s32)(-s->bike.fwd.x * 1000.0f), (s32)(s->bike.fwd.z * 1000.0f), L.a, &L.b);
    math_to_polar((s32)(-s->bike.vel.x * 1000.0f), (s32)(s->bike.vel.z * 1000.0f), L.c, &L.b);
    L.b = (s16)(L.a[0] - L.c[0]);
    if (((s->input & 0x10000) == 0) || (s->airFrames >= 0)) goto normal;
    a = s->stickX;
    if (a < 0) a = -a;
    if (a < 6) goto normal;
    if (s->yawLag < 0x10) s->yawLag = s->yawLag + 1;
    goto after;
normal:
    if (s->powerSlide == 1) {
        if (s->yawLag < 0x18) s->yawLag = s->yawLag + 1;
    } else {
        a = L.b;
        if (a < 0) a = -a;
        if (a < 0x800 && s->yawLag != 0) s->yawLag = s->yawLag - 1;
    }
after:
    iv2 = 0;
    if (s->airFrames < 0) iv2 = s->yawLag;
    if (s->fwdSpeed < 2.0f) iv2 = 0x20;
    iv1 = L.b * iv2;
    iv2 = (s16)L.c[0];
    if (iv1 < 0) iv1 = iv1 + 0x1f;
    L.b = iv2 + (iv1 >> 5);
    s->heading = ((u16 *)&L.b)[1];
done:
    return s->heading;
}

#define COST(i) gCosTable[(((i) >> 4) - 0x400) & 0xFFF]
#define SINT(i) gCosTable[((i) >> 4) & 0xFFF]

/* Returns what its callees leave in v0: cam_snap_to_2 and cam_update_chase_2 are declared to return s32. */
s32 cam_mode_soccer(Rider *bike, CamOut *out) {
    u16 a1;
    s32 ratio;
    u16 a2;
    f32 s;
    f32 fz;
    s32 n;
    s32 z;
    s32 i;
    s32 ang, q, ang3, q3;

    n = gEvent.flags & 0xF;
    if (n != 0) {
    i = n - 1;
    if (i == ((bike - gRiders) >> ((gGame.numPlayers - 1) >> 1))) {
        switch (n) {
        case 1:
            s = 1000.0f;
            q = gEvent.goalTimer * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang = gEvent.goalTimer * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * s + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent.goalTimer * s * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * s - (f32)(((0x4844 - gEvent.goalTimer * 10500 / 180) * SINT(gEvent.goalTimer * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * s - (f32)out->x, gSoccerBall.pos.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * s - (f32)out->y, &a2, &ratio);
            out->roll = 0;
            out->pitch = a2 - 0x4000;
            out->yaw = -a1;
            break;
        case 2:
            s = 1000.0f;
            q3 = gEvent.goalTimer * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang3 = gEvent.goalTimer * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * s - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent.goalTimer * s * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * s + (f32)(((0x4844 - gEvent.goalTimer * 10500 / 180) * SINT(gEvent.goalTimer * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * s - (f32)out->x, gSoccerBall.pos.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * s - (f32)out->y, &a2, &ratio);
            out->roll = 0;
            out->pitch = a2 - 0x4000;
            out->yaw = -a1;
            break;
        default:
            fatal_halt(1);
        }
    } else switch (n) {
    case 1: {
        Rider *p = &gRiders[i];
        out->x = p->bike.pos.x * 1000.0f + (f32)((COST(gEvent.goalTimer * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f - (f32)((SINT(gEvent.goalTimer * 14000 / 180 - 7000) * 625) >> 11);
        out->pitch = 1000;
        out->yaw = gEvent.goalTimer * 14000 / 180 - 7000;
        out->roll = 0;
        break; }
    case 2: {
        Rider *p = &gRiders[i];
        out->x = p->bike.pos.x * 1000.0f + (f32)((COST(7000 - gEvent.goalTimer * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f + (f32)((SINT(7000 - gEvent.goalTimer * 14000 / 180) * 625) >> 11);
        out->pitch = 1000;
        out->yaw = gEvent.goalTimer * 14000 / 180 + 0x64A8;
        out->roll = 0;
        break; }
    }
    return cam_snap_to_2(out);
    } else {
        return cam_update_chase_2(bike, out, -7000);
    }
}

f32 cam_calc_hill_climb_pitch(Rider *param_1)
{
    f32 fs;
    f32 v[3];
    f32 len;
    f32 d;
    f32 t;
    f32 k8;
    f32 s2;
    u8 fl;
    s32 *gp = &sCamHillPitch;

    fl = gPathNodes[param_1->segment].flags;
    fs = (f32)*gp;
    if ((fl & 1) == 0) {
        s2 = param_1->bike.fwd.x * param_1->bike.fwd.x +
             param_1->bike.fwd.z * param_1->bike.fwd.z;
        k8 = 8.0f;
        len = sqrtf(s2);
        v[0] = param_1->rearWheel.contact.x + (param_1->bike.fwd.x * k8) / len;
        v[2] = param_1->rearWheel.contact.z + (param_1->bike.fwd.z * k8) / len;
        v[1] = param_1->rearWheel.contact.y;
        d = bike_get_ground_height(v);
        v[1] = d;
        if (gGroundFound != 0) {
            t = param_1->rearWheel.contact.y - d;
            fs = t * 1024.0f;
            if ((gCheatFlags & 8) == 0) {
                if (fs < -2048.0f) fs = -2048.0f;
                else if (fs > 1024.0f) fs = 1024.0f;
            } else {
                if (fs < -1024.0f) fs = -1024.0f;
                else if (fs > 4096.0f) fs = 4096.0f;
            }
        }
    }
    return fs + 2000.0f;
}

void cam_mode_hill_climb(Rider *p, CamOut *out)
{
    s32 v[4];
    s32 r[4];
    s32 m[10];
    s16 ang;
    s32 dist;
    CamOut *tb;
    CamOut *base;
    s32 z2, a, b;
    V3 *pv;
    s32 z, d;
    s16 rot, h;

    if (p->crashState == 0) {
        rot = (u16)p->heading;
        h = cam_calc_hill_climb_pitch(p);
        mtx3i_from_angles(m, h, 0, rot);
        v[0] = 0;
        v[1] = 0;
        v[2] = -sCamHillDist;
        vec3i_mul_mtx3i(v, m, r);
        pv = &p->bike.pos;
        out->x = r[0] + pv->x * 1000.0f;
        out->y = r[1] + pv->y * 1000.0f + sCamHillHeight;
        out->z = r[2] + pv->z * 1000.0f;
        out->pitch = h;
        out->roll = 0;
        out->yaw = rot;
    } else {
        base = sCamHistorySum;
        tb = &base[gCamView];
        out->x = tb->x / 16;
        out->y = tb->y / 16;
        z = tb->z;
        if (z < 0) {
            z += 0xF;
        }
        z2 = z >> 4;
        out->z = z2;
        a = out->x - (s32)(p->bike.pos.x * 1000.0f);
        b = (s32)(p->bike.pos.z * 1000.0f) - z2;
        d = (s32)(p->bike.pos.y * 1000.0f) + sCamHillHeight - out->y;
        math_to_polar(a, b, &ang, &dist);
        if (dist > 10000) {
            p->crashTimer += 3;
        }
        out->yaw = ang;
        math_to_polar(-d, dist, &ang, &dist);
        out->pitch = ang;
        out->roll = 0;
    }
    cam_smooth_to(out);
}

void cam_update_chase(Rider *p, CamOut *out, s32 z)
{
    s32 n, k, ang, tv, zero;
    s32 rot;
    f32 f, g, nv, fc, a, b, c, d;
    s32 dy, dz;
    s32 vec[3];
    s32 res[3];
    s32 mtx[10];
    f32 *pp;
    CamPitchSpring *e, *eb;
    s32 *q, *cnt;
    s32 tb;
    s32 *q2;
    f32 cs;

    k = p->airFrames;
    cnt = sCamAirBlend;
    n = cnt[gCamView];
    if (p->landingTime * 60.0f + (f32)k > 60.0f) {
        if (n < 0x20) {
            n++;
        }
    } else if (k < 0) {
        if (n == 0) goto first;
        n--;
    }
    if (n == 0) {
first:
        f = cam_calc_ground_pitch(p);
    } else if (n == 0x20) {
        f = cam_calc_air_pitch(p);
    } else {
        a = cam_calc_ground_pitch(p);
        b = cam_calc_air_pitch(p);
        f = (a * (f32)(0x20 - n) + b * (f32)n) * 0.03125f;
    }
    sCamAirBlend[gCamView] = n;
    eb = sCamPitchSprings;
    e = &eb[gCamView];
    f = f - e->pos;
    if (f > 0.0f) {
        nv = e->vel;
        nv += f * sCamSpringUp;
        fc = sCamDampUp;
        e->vel = nv;
        do { d = nv; } while (0);
    } else {
        nv = e->vel;
        nv += f * sCamSpringDown;
        fc = sCamDampDown;
        e->vel = nv;
        do { d = nv; } while (0);
    }
    d *= fc;
    e->pos += nv;
    do { e->vel = d; } while (0);
    if ((zero = gGame.mode, rot = (u16)p->heading, zero != 9) && !(gGame.flags & 0x8000)) {
        if ((p->input & 0x200) && gEvent.state != 0) {
            rot += -0x8000;
        }
    }
    ang = sCamPitchSprings[gCamView].pos;
    if (p->unk_77C != 0) {
        ang = -500;
    }
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = z;
    if (gEvent.state == 0) {
        g = (f32)gEvent.countdown * 0.00390625f;
        if (g < 0.5f) {
            g = 2.0f * (g * g);
        } else {
            g = 1.0f - g;
            g = 2.0f * (g * g);
            g = 1.0f - g;
        }
        tv = g * 36864.0f;
        tb = gCosTable[(tv >> 4) & 0xFFF] / 8;
        dy = 0;
        dz = -12000;
        if (!(gGame.flags & 0x8000)) {
            rot += tv;
            vec[0] = (f32)tb * g;
            vec[1] = g * (f32)dy;
            vec[2] = (f32)z + g * (f32)dz;
        }
    }
    mtx3i_from_angles(mtx, ang, 0, (s16)rot);
    vec3i_mul_mtx3i(vec, mtx, res);
    pp = &p->bike.pos.x;
    out->x = (f32)res[0] + pp[0] * 1000.0f;
    out->y = (f32)res[1] + pp[1] * 1000.0f + 600.0f;
    out->z = (f32)res[2] + pp[2] * 1000.0f;
    out->pitch = ang - 500;
    out->yaw = (s16)rot;
    out->roll = 0;
    if (gGame.mode != 9) {
        if ((p->input & 0x200) || ((gPlayerInputs_2 & 0x10) && gEvent.state == 0 && !(gGame.flags & 0x8000))) {
            cam_snap_to(out);
        } else if (p->prevInput & 0x200) {
            cam_snap_to_moving(out, &p->bike.vel.x);
        } else {
            goto other;
        }
    } else {
other:
        cam_smooth_to(out);
    }
    sCamShakeY[gCamView] = 0;
    if (p->rearWheel.surface != NULL && p->airFrames < 6 && (p->rearWheel.surface->flags & 0x10)) {
        sCamShakeY[gCamView] = -(s32)(p->rearWheel.bumpNoise * 1000.0f) * 2;
    }
    if (p->roughLanding != 0) {
        f = p->roughLanding;
        cs = math_exp(-f * 0.0625f);
        q2 = sCamShakeY;
        f = f * 3.1415927f;
        q = &q2[gCamView];
        *q += (s32)(sinf(f / 6.0f) * 1024.0f * cs);
    }
}

void cam_mode_fall_view(Rider *e, CamOut *r)
{
    CamOut *b;
    s32 i;
    s32 d;

    b = sCamHistorySum;
    i = e - gRiders;
    r->x = b[i].x / 16;
    r->y = b[i].y / 16;
    r->z = b[i].z / 16;
    d = e->drawPos.y - r->y;
    r->pitch = b[i].pitch;
    r->yaw = b[i].yaw;
    r->roll = 0;
    cam_snap_to(r);
    if (gGame.mode == 7) {
        e->timeScale = 0.5f;
        if (d < -12000) {
            e->crashTimer = 10000;
            e->timeScale = 1.0f;
        }
    }
}



void cam_start_fall_view(Rider *p)
{
    CamOut loc;
    s32 m[9];
    s32 v[3];
    s32 o[3];
    s16 ang;
    s32 dist;
    f32 f;
    s32 idx;

    if (p->camMode != 9) {
        gCamLastModes[p - gRiders] = 9;
        p->camMode = 9;
        if (gCurTrack == D_800CB5D0) {
            loc = sCamFallViewPos;
            math_to_polar(loc.x - p->drawPos.x, p->drawPos.z - loc.z, &ang, &dist);
            loc.yaw = ang;
        } else {
            path_update_bike(p);
            math_to_polar(-(gPathTangentX * 1000.0f), gPathTangentZ * 1000.0f, &ang, &dist);
            if (0.0f < p->lateral) {
                ang += 0x4000;
            } else {
                ang -= 0x4000;
            }
            p->heading = ang;
            mtx3i_from_angles(m, 0x1000, 0, ang);
            f = gSurfaceTable[(gGroundSurface >> 4) & 0xF].bumpHeight;
            if (-0.1f < f) {
                f = -0.1f;
            }
            v[0] = 0;
            v[1] = 0;
            f *= 100000.0f;
            v[2] = f;
            vec3i_mul_mtx3i(v, m, o);
            loc.x = (s32)(p->bike.pos.x * 1000.0f) + o[0];
            loc.y = (s32)(p->bike.pos.y * 1000.0f) + o[1] - 1000;
            loc.z = (s32)(p->bike.pos.z * 1000.0f) + o[2];
            loc.pitch = 0x1000;
            loc.roll = 0;
            loc.yaw = ang;
        }
        gCamView = idx = p - gRiders;
        cam_snap_to(&loc);
        func_8004F064(idx);
    }
}

void cam_mode_stunt_pip(Rider *p, CamOut *out)
{
    s32 a[3];
    s32 b[3];
    s32 m[10];

    if (p->pipTimer != 0) {
        if (p->airFrames <= 0) {
            p->pipTimer--;
        }
        mtx3i_from_angles(m, 0x800, 0, p->rot[2] + gCamPipOrbitAngle);
        gCamPipOrbitAngle -= 0x40;
        if (gCamPipOrbitAngle < 9000) {
            gCamPipOrbitAngle = 9000;
        }
        a[0] = 0;
        a[1] = 0;
        a[2] = -4000;
        vec3i_mul_mtx3i(a, m, b);
        out->x = (f32)b[0] + p->bike.smoothPos.x * 1000.0f;
        out->y = (f32)b[1] + p->bike.smoothPos.y * 1000.0f;
        out->z = (f32)b[2] + p->bike.smoothPos.z * 1000.0f;
        out->pitch = 0x800;
        out->yaw = p->rot[2] + gCamPipOrbitAngle;
        out->roll = 0;
        cam_snap_to(out);
    }
}

void cam_smooth_to(CamOut *p)
{
    s32 a, b, c, k, j, idx;

    if (sCamFrameCount == 0) {
        cam_snap_to(p);
        return;
    }
    c = b = a = 0;
    idx = sCamHistoryPos[gCamView];
    sCamHistorySum[gCamView].x -= sCamHistory[gCamView][idx].x;
    sCamHistorySum[gCamView].y -= sCamHistory[gCamView][idx].y;
    sCamHistorySum[gCamView].z -= sCamHistory[gCamView][idx].z;
    sCamHistory[gCamView][idx] = *p;
    sCamHistorySum[gCamView].x += sCamHistory[gCamView][idx].x;
    sCamHistorySum[gCamView].y += sCamHistory[gCamView][idx].y;
    sCamHistorySum[gCamView].z += sCamHistory[gCamView][idx].z;
    j = idx - 1;
    for (k = 0; k < 15; k++) {
        if (j < 0) j += 16;
        a += (s16)(sCamHistory[gCamView][j].yaw - sCamHistory[gCamView][idx].yaw);
        b += (s16)(sCamHistory[gCamView][j].roll - sCamHistory[gCamView][idx].roll);
        c += (s16)(sCamHistory[gCamView][j].pitch - sCamHistory[gCamView][idx].pitch);
        j--;
    }
    sCamHistorySum[gCamView].pitch = c / 16 + sCamHistory[gCamView][idx].pitch;
    sCamHistorySum[gCamView].roll = b / 16 + sCamHistory[gCamView][idx].roll;
    sCamHistorySum[gCamView].yaw = a / 16 + sCamHistory[gCamView][idx].yaw;
    sCamHistoryPos[gCamView] = (idx + 1) % 16;
}

void cam_snap_to(CamOut *src)
{
    s32 i;
    s32 v;
    u8 *base;
    s32 o;
    CamOut *d;

    i = 0;
    base = (u8 *)sCamHistory;
    do {
        v = gCamView;
        o = (i * sizeof(CamOut) + v * sizeof(sCamHistory[0])) + (s32)base;
        i++;
        *(CamOut *)o = *src;
    } while (i < 16);
    sCamHistorySum[gCamView] = *src;
    d = &sCamHistorySum[gCamView];
    d->x <<= 4;
    d->y <<= 4;
    d->z <<= 4;
}

void cam_snap_to_moving(CamOut *p, f32 *v)
{
    s32 i;
    s32 sx, sy, sz;

    sx = sy = sz = 0;
    for (i = 15; i >= 0; i--) {
        sCamHistory[gCamView][i] = *p;
        sx += p->x;
        sy += p->y;
        sz += p->z;
        p->x -= (s32)(v[0] * 16.666666f);
        p->y -= (s32)(v[1] * 16.666666f);
        p->z -= (s32)(v[2] * 16.666666f);
    }
    sCamHistorySum[gCamView] = *p;
    sCamHistorySum[gCamView].x = sx;
    sCamHistorySum[gCamView].y = sy;
    sCamHistorySum[gCamView].z = sz;
    sCamHistoryPos[gCamView] = 0;
}

typedef struct Obj_16 {
    u8 pad0[0x74];
    s16 nA;
    s16 nB;
    CamFollowZone *la;
    CamDollyZone *lb;
} Obj_16;

extern Obj_16 *gCurTrack_20;
extern Obj_16 *gCurTrack_43;

/* The zone at byte offset off of the track's lists */
#define FOLLOW_ZONE(off) (*(CamFollowZone *)(off + (s32)P->la))
#define DOLLY_ZONE(off) (*(CamDollyZone *)(off + (s32)P->lb))
#define ZONE_STATE ((CamZoneState *)pf)

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
        D_800C97E4 = -1;
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
                if (pass == 0 || (FOLLOW_ZONE(off).flags & 0x40)) {
                    end = order[FOLLOW_ZONE(off).endNode];
                    start = order[FOLLOW_ZONE(off).startNode];
                    if ((end < start && (*pos >= start || *pos < end)) ||
                        (start < end && *pos >= start && *pos < end)) {
                        if (g->noPlayers != 0 || !(FOLLOW_ZONE(off).flags & 2)) {
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
                if (pos == (s16 *)order) v = 1; else v = 2;
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
                if (pass == 0 || (DOLLY_ZONE(off).flags & 0x40)) {
                    end2 = order[DOLLY_ZONE(off).endNode];
                    start2 = order[DOLLY_ZONE(off).startNode];
                    if ((end2 < start2 && (*pos >= start2 || *pos < end2)) ||
                        (start2 < end2 && *pos >= start2 && *pos < end2)) {
                        if (g->noPlayers != 0 || !(DOLLY_ZONE(off).flags & 2)) {
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
                                D_800C97E4 = k;
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

f32 cam_zone_get_progress(Rider *p, s32 a, s32 b)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (gCamZoneState[gCamView].flags & 8) {
        len = sCamSegOrder[b] - sCamSegOrder[a];
        seg = path_find_segment(&p->bike.pos, gPathNodes[p->segment].alt, 15, 1);
        path_get_lateral_offset(&p->bike.pos, seg);
        frac = gPathSegmentT;
    } else {
        seg = p->segment;
        seg = (s16)seg;
        frac = p->segmentT;
        len = sCamSegOrder[b] - sCamSegOrder[a];
    }
    if (len < 0) {
        len += gPathNodeCount;
    }
    d = sCamSegOrder[seg] - sCamSegOrder[a];
    if (d < 0) {
        d += gPathNodeCount;
    }
    return ((f32)d + frac) / (f32)len;
}

typedef struct {
    s16 h[2];
    s32 w;
} Tmp30;

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
    if (flags & 4) {
        gCamZoneState[gCamView].flags = flags & 0xFFFB;
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

void cam_zone_update_dolly(Rider *rider, CamOut *out)
{
    s32 m[3];
    s32 mtx[10];
    s16 a1;
    s32 r1;
    s16 a2;
    s16 b1;
    s32 r2;
    s16 b2;
    CamZoneState *st;
    s32 idx;
    CamDollyZone *zone;
    u16 flags;
    s32 f;
    s32 rx, rz;
    f32 t, u, v;

    st = gCamZoneState;
    st += gCamView;
    flags = st->flags;
    zone = st->zone.dolly;
    if (flags & 4) {
        st->flags = flags & ~4;
        f = zone->flags;
        if (f & 8) {
            if (f & 1) {
                if (f & 4) {
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
            f = zone->flags;
        }
        if (f & 0x10) {
            mtx3i_from_angles(mtx, zone->x1, 0, zone->z1);
            m[0] = 0;
            m[1] = 0;
            m[2] = zone->y1;
            vec3i_mul_mtx3i(m, mtx, &sCamZoneOffset.x);
        } else if (f & 4) {
            math_to_polar(-zone->x, zone->z, &a1, &r1);
            math_to_polar(r1, -zone->y + 700, &a2, &r1);
            mtx3i_from_angles(mtx, a2 - 0x4000, 0, -a1);
            m[0] = 0;
            m[1] = 0;
            m[2] = zone->y1;
            vec3i_mul_mtx3i(m, mtx, &sCamZoneOffset.x);
        }
    }
    t = cam_zone_get_progress(rider, zone->startNode, zone->endNode);
    f = zone->flags;
    if (f & 4) {
        out->x = (f32)zone->x + rider->bike.smoothPos.x * 1000.0f;
        out->y = (f32)zone->y + rider->bike.smoothPos.y * 1000.0f;
        out->z = (f32)zone->z + rider->bike.smoothPos.z * 1000.0f;
        if (zone->flags & 0x20) {
            u = 1.0f - t;
            out->x = (f32)out->x + (f32)sCamZoneOffset.x * u;
            out->y = (f32)out->y + (f32)sCamZoneOffset.y * u;
            out->z = (f32)out->z + (f32)sCamZoneOffset.z * u;
        } else {
            out->x = (f32)out->x + (f32)sCamZoneOffset.x * t;
            out->y = (f32)out->y + (f32)sCamZoneOffset.y * t;
            out->z = (f32)out->z + (f32)sCamZoneOffset.z * t;
        }
    } else if (f & 1) {
        out->x = (f32)zone->x + (f32)(zone->x1 - zone->x) * t;
        out->y = (f32)zone->y + (f32)(zone->y1 - zone->y) * t;
        out->z = (f32)zone->z + (f32)(zone->z1 - zone->z) * t;
    } else if (f & 0x10) {
        if (f & 0x20) {
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
    if (zone->flags & 0x10) {
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

void cam_get_track_heading(s16 p1, f32 p2, u16 *p3)
{
    s32 i1, i2;
    s32 aux[2];
    f32 xa, xb;
    if (0.0f <= p2) {
        i2 = p1;
        i1 = i2 + 1;
        if (i1 >= gPathNodeCount) i1 -= gPathNodeCount;
    } else {
        s32 t = p1;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) i2 += gPathNodeCount;
        p2 = p2 + 1.0f;
    }
    xb = (gPathNodes[i2].lateralZ * (1.0f - p2) + gPathNodes[i1].lateralZ * p2) * 16384.0f;
    xa = (gPathNodes[i2].lateralX * (1.0f - p2) + gPathNodes[i1].lateralX * p2) * 16384.0f;
    math_to_polar((s32)xb, (s32)xa, p3, aux);
}

void cam_get_track_pos(s16 param_1, f32 param_2, f32 *param_3, f32 *param_4, f32 *param_5)
{
    s32 i0, i1;
    s32 t;

    if (0.0f <= param_2) {
        i0 = param_1;
        i1 = i0 + 1;
        if (i1 >= gPathNodeCount) {
            i1 -= gPathNodeCount;
        }
    } else {
        t = param_1;
        i0 = t - 1;
        i1 = t;
        if (i0 < 0) {
            i0 = gPathNodeCount - 1;
        }
        param_2 = param_2 + 1.0f;
    }
    *param_3 = gPathNodes[i0].pos.x + (gPathNodes[i1].pos.x - gPathNodes[i0].pos.x) * param_2;
    *param_4 = gPathNodes[i0].pos.y + (gPathNodes[i1].pos.y - gPathNodes[i0].pos.y) * param_2;
    *param_5 = gPathNodes[i0].pos.z + (gPathNodes[i1].pos.z - gPathNodes[i0].pos.z) * param_2;
}

void cam_collide_walls(s32 a, s32 b, s32 c)
{
    Rider *nd, *nd0;
    Vec3i *tb;
    Vec3f p;
    f32 pt[3];
    f32 n[3];
    f32 q[4];
    f32 tt[2];
    f32 sum, len, L, t;
    Vec3i *dst;

    nd0 = gRiders;
    nd = nd0 + a;
    tb = gCamPos;
    p = nd->bike.pos;
    dst = &tb[b];
    pt[0] = (f32)dst->x / 1000.0f;
    pt[1] = (f32)dst->y / 1000.0f;
    pt[2] = (f32)dst->z / 1000.0f;
    pt[0] = pt[0] - p.x;
    pt[1] = pt[1] - p.y;
    pt[2] = pt[2] - p.z;
    sum = pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2];
    L = __builtin_sqrtf(sum);
    if (0.0f < L) {
        n[0] = pt[0] / L;
        n[1] = pt[1] / L;
        n[2] = pt[2] / L;
    }
    L = L + 0.2f;
    pt[0] = p.x + n[0] * L;
    pt[1] = p.y + n[1] * L;
    pt[2] = p.z + n[2] * L;
    height_gather_walls((s32)((p.x - L) * 1000.0f), (s32)((p.z - L) * 1000.0f),
                  (s32)((p.x + L) * 1000.0f), (s32)((p.z + L) * 1000.0f));
    tt[0] = 1.0f;
    if (height_raycast_walls(&p, pt, q, tt, c)) {
        t = tt[0] * L - 0.2f;
        pt[0] = p.x + t * n[0];
        pt[1] = p.y + t * n[1];
        pt[2] = p.z + t * n[2];
        dst->x = (s32)(pt[0] * 1000.0f);
        dst->y = (s32)(pt[1] * 1000.0f);
        tt[0] = t;
        dst->z = (s32)(pt[2] * 1000.0f);
    }
    cam_keep_above_ground(a, b);
}

typedef struct {
    s16 v;
} Ang;

void cam_shift_xz(s32 p1, s32 p2, s32 p3, s32 p4)
{
    Ang a;
    s32 b;
    Ang c;
    s32 dx, dz, d;
    s32 i;

    dx = gRiders[p2].bike.smoothPos.x * 1000.0f;
    dx -= gCamPos[p1].x;
    dz = gRiders[p2].bike.smoothPos.z * 1000.0f;
    dz -= gCamPos[p1].z;
    math_to_polar(dx, dz, &a.v, &b);
    math_to_polar(dx - p3, dz - p4, &c.v, &b);
    gCamPos[p1].x += p3;
    gCamPos[p1].z += p4;
    gCamYaw[p1] -= (d = c.v - a.v);
    if (b > 0x1000) {
        for (i = 0; i < 16; i++) {
            sCamHistory[p1][i].x += p3;
            sCamHistory[p1][i].z += p4;
            sCamHistory[p1][i].yaw -= d;
        }
        sCamHistorySum[p1].x += p3 * 16;
        sCamHistorySum[p1].z += p4 * 16;
    }
}

typedef struct { u8 p0[0x2C]; s32 f2C; u8 p30[0xA0 - 0x30]; } Trk_9;

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

void cam_clamp_to_track(s32 m, s32 n)
{
    Vec3f v;
    s32 idx;
    f32 t;
    s16 a0;
    s32 ratio;
    s16 a1;
    s32 lim;
    Rider *pb;
    Vec3i *q;
    PathNode *pr, *pn;
    s32 nx;
    PathNode *tab;
    f32 ax, az, bx, bz, qx, qz;
    s32 k, h, top, d, r, dz;
    f32 f, g, sx, sz;

    if (D_800EE140 != 0) {
        return;
    }
    if (gCurTrack_21->f2C == 2) {
        cam_keep_above_ground(m, n);
        return;
    }
    {
        v.x = (f32)gCamPos[n].x / 1000.0f;
        v.y = (f32)gCamPos[n].y / 1000.0f;
        v.z = (f32)gCamPos[n].z / 1000.0f;
        path_get_segment_and_t(&v, &idx, &t, gRiders[m].segment);
        tab = gPathNodes;
        pr = (PathNode *)(idx * sizeof(PathNode) + (u32)tab);
        nx = pr->next;
        gCamSegFlags[n] = pr->flags;
        if (gCurTrack_21->f2C == 0) {
            s32 id = gCurTrack_21 - gTracks_9;
            if (id == 0x18 || id == 0x15 || id == 0x16) {
                cam_collide_walls(m, n, 1);
            }
            return;
        }
        D_800E7F98 = 0;
        if (pr->camCeiling < 0) {
            cam_collide_walls(m, n, 0);
            return;
        }
        if (pr->camCeiling > 0) {
            pn = (PathNode *)(nx * sizeof(PathNode) + (u32)tab);
            if (pn->camCeiling > 0) {
                f32 ya, yb;
                D_800E7F98 = 1;
                ya = pr->pos.y * 1000.0f + (f32)pr->camCeiling * 16.0f;
                yb = pn->pos.y * 1000.0f + (f32)pn->camCeiling * 16.0f;
                lim = (s32)(lerpi(pr->camWidthNeg, pn->camWidthNeg, t) * 16.0f);
                top = (s32)(lerpi(pr->camWidthPos, pn->camWidthPos, t) * 16.0f);
                h = (s32)lerp(ya, yb, t);
                if (h < gCamPos[n].y && gRiders[m].airFrames < 0x1e) {
                    f32 ex, ez;
                    ex = gRiders[m].bike.smoothPos.x * 1000.0f - (f32)gCamPos[n].x;
                    ez = gRiders[m].bike.smoothPos.z * 1000.0f - (f32)gCamPos[n].z;
                    ex = __builtin_sqrtf(ex * ex + ez * ez);
                    g = gRiders[m].bike.smoothPos.y;
                    g *= 1000.0f;
                    r = (s32)ex;
                    g -= (f32)gCamPos[n].y;
                    math_to_polar((s32)g, r, &a0, &ratio);
                    g += (f32)(gCamPos[n].y - h);
                    math_to_polar((s32)g, r, &a1, &ratio);
                    gCamPitch[n] -= a1 - a0;
                    for (k = 0; k < 16; k++) {
                        sCamHistory[n][k].y += h - gCamPos[n].y;
                        sCamHistory[n][k].pitch -= a1 - a0;
                    }
                    sCamHistorySum[n].y += (h - gCamPos[n].y) * 16;
                    gCamPos[n].y = h;
                }
                {
                PathNode *pr2, *pn2;
                PathNode *tab2;
                f32 ax, az;
                tab2 = gPathNodes;
                pr2 = (PathNode *)(idx * sizeof(PathNode) + (u32)tab2);
                nx = pr2->next;
                pn2 = (PathNode *)(nx * sizeof(PathNode) + (u32)tab2);
                g = lerp(pr2->lateralX, pn2->lateralX, t);
                f = lerp(pr2->lateralZ, pn2->lateralZ, t);
                ax = pr2->pos.x * 1000.0f;
                az = pr2->pos.z * 1000.0f;
                dz = (s32)(lerp((f32)gCamPos[n].x - ax, (f32)gCamPos[n].x - pn2->pos.x * 1000.0f, t) * g +
                           lerp((f32)gCamPos[n].z - az, (f32)gCamPos[n].z - pn2->pos.z * 1000.0f, t) * f);
                }
                if (top < dz) {
                    d = top - dz;
                } else if (dz < -lim) {
                    d = -lim - dz;
                } else {
                    cam_keep_above_ground(m, n);
                    return;
                }
                cam_shift_xz(n, m, (s32)((f32)d * g), (s32)((f32)d * f));
            }
        }
    }
    cam_keep_above_ground(m, n);
}

void cam_keep_above_ground(s32 param_1, s32 param_2)
{
    Vec3i *base = gCamPos;
    Vec3i *t = &base[param_2];
    s32 iVar1;
    f32 r;
    f32 fVar2, fVar4;
    f32 fVar3;
    f32 dz;
    s32 bw;
    s32 iStack2c;

    iVar1 = height_get_ground(t->x, t->y + 0x1F4, t->z);
    iVar1 = iVar1 + 200;
    if (gGroundFound != 0 && t->y < iVar1) {
        fVar3 = gRiders[param_1].bike.smoothPos.x * 1000.0f - (f32)t->x;
        dz = gRiders[param_1].bike.smoothPos.z * 1000.0f - (f32)t->z;
        r = sqrtf(fVar3 * fVar3 + dz * dz);
        fVar4 = (f32)gCamPitch[param_2] * 3.1415925f * 3.0517578e-05f;
        fVar3 = r;
        fVar2 = sinf(fVar4);
        fVar4 = cosf(fVar4);
        fVar4 = (fVar2 / fVar4) * fVar3 - (f32)(t->y - iVar1);
        math_to_polar((s32)fVar4, (s32)fVar3, (s16 *)&bw, &iStack2c);
        gCamPitch[param_2] = (s32)((s16 *)&bw)[0];
        t->y = iVar1;
    }
}

void cam_shift_z(s32 p, s32 q)
{
    s32 i;

    sCamHistorySum[p].z += q * 16;
    for (i = 0; i < 16; i++) {
        sCamHistory[p][i].z += q;
    }
    gCamView = p;
    cam_update();
    gGroundFound = 0;
    height_get_ground(gRiders[p].drawPos.x, gRiders[p].drawPos.y, gRiders[p].drawPos.z);
    if (gGroundFound != 0) {
        gRiderSection[p] = gGroundArea;
    }
}

void cam_mode_chase_7m(Rider *param_1, CamOut *param_2)
{
    cam_update_chase(param_1, param_2, -7000);
}

void cam_mode_chase_far_alt(Rider *a, CamOut *b)
{
    cam_update_chase(a, b, -10000);
}

void cam_mode_side_view(Rider *param_1, CamOut *param_2)
{
    f32 k = 1000.0f;

    param_2->x = (s32)-sCamSideViewParams[0];
    param_2->y = (s32)(param_1->bike.smoothPos.y * k + sCamSideViewParams[1]);
    param_2->z = (s32)(param_1->bike.smoothPos.z * k - sCamSideViewParams[2]);
    param_2->pitch = (s32)sCamSideViewParams[4];
    param_2->yaw = (s32)-sCamSideViewParams[3];
    param_2->roll = 0;
    cam_snap_to(param_2);
}

void func_8002D528(Rider *o, CamOut *out) {
    out->x = o->bike.smoothPos.x * 1000.0f + -28000.0f;
    out->y = 0x5BCC;
    out->z = o->bike.smoothPos.z * 1000.0f - 10000.0f;
    out->pitch = 0x1800;
    out->yaw = -0x4000;
    out->roll = 0;
    cam_smooth_to(out);
}

void cam_mode_chase_near(Rider *p, CamOut *p2) {
    s32 v = -0x9C4;

    if (p->crashState != 0) {
        v = -5000;
    }
    if (p->fwdSpeed < 0.0f) {
        v = (s32)((f32)v + p->fwdSpeed * 250.0f);
    }
    cam_update_chase(p, p2, v);
}

void cam_mode_chase_mid(Rider *param_1, CamOut *param_2)
{
    cam_update_chase(param_1, param_2, -5000);
}

void cam_mode_chase_far(Rider *a, CamOut *b) {
    cam_update_chase(a, b, -10000);
}

typedef struct { u8 p[0x2C]; s32 f2c; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *gCurTrack_22;

void cam_mode_celebration(Rider *o, CamOut *out)
{
    Stk st;
    s16 it;
    s16 sv;
    s32 a;

    it = 2000;
    if (gCurTrack_22->f2c == 1) {
        it = 6000;
    }
    a = (u16)o->drawRot[2] + 0x7000;
    sv = a;
    if (o->finishState > 1) {
        sv = a - o->celebrationTimer * 0x40;
        o->celebrationTimer = o->celebrationTimer + 1;
    }
    st.l60[0] = 0;
    st.l60[1] = 0;
    st.l60[2] = -5000;
    mtx3i_from_angles(st.arr, it, 0, sv);
    vec3i_mul_mtx3i(st.l60, st.arr, &st.l50);
    out->x = (s32)((f32)st.l50 + o->bike.smoothPos.x * 1000.0f);
    out->y = (s32)((f32)st.l4c + o->bike.smoothPos.y * 1000.0f + 750.0f);
    out->z = (s32)((f32)st.l48 + o->bike.smoothPos.z * 1000.0f);
    out->pitch = it;
    out->yaw = sv;
    out->roll = 0;
    cam_snap_to(out);
}

void cam_mode_first_person(Rider *p1, CamOut *p2)
{
    s32 t[4];
    s32 o[3];
    s32 a[10];
    s32 v;

    mtx3i_from_angles(a, p1->drawRot[0], p1->drawRot[1], p1->drawRot[2]);
    t[0] = 0;
    t[1] = 400;
    t[2] = 0;
    vec3i_mul_mtx3i(t, a, o);
    p2->x = (s32)((f32)o[0] + p1->bike.smoothPos.x * 1000.0f);
    p2->y = (s32)((f32)o[1] + p1->bike.smoothPos.y * 1000.0f);
    p2->z = (s32)((f32)o[2] + p1->bike.smoothPos.z * 1000.0f);
    p2->pitch = p1->drawRot[0] / 2;
    p2->yaw = p1->drawRot[2];
    v = p1->drawRot[1];
    if (v < 0) {
        v += 3;
    }
    p2->roll = v >> 2;
    cam_snap_to(p2);
}

void func_8002D888(Rider *arg0, CamOut *arg1)
{
}

void func_8002D890(void)
{
}

void func_8002D898(void)
{
}

void cam_free(void) {
    mem_free(sCamSegOrder);
}

void cam_get_track_turn(s16 p1, f32 f, u16 *p3)
{
    u16 loc[2];
    s32 i0, i1;

    i0 = p1 - 1;
    if (i0 < 0) {
        i0 += gPathNodeCount;
    }
    i1 = p1 + 1;
    if (gPathNodeCount <= i1) {
        i1 -= gPathNodeCount;
    }
    cam_get_track_heading((s16)i0, f, &loc[0]);
    cam_get_track_heading((s16)i1, f, &loc[1]);
    *p3 = loc[1] - loc[0];
}

void cam_get_track_slope(s32 a0, f32 f, s16 *out)
{
    s16 i;
    s32 i1, i2;
    f32 r;
    PathNode *base;
    i = a0;
    if (0.0f <= f) {
        i2 = i;
        i1 = i + 1;
        if (gPathNodeCount <= i1) {
            i1 = i1 - gPathNodeCount;
        }
    } else {
        s32 t;
        t = i;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) {
            i2 = i2 + gPathNodeCount;
        }
        f = f + 1.0f;
    }
    base = gPathNodes;
    r = base[i2].slope * (1.0f - f) + base[i1].slope * f;
    *out = (s16)((r * 32768.0f) / 3.1415927f);
}

void cam_set_frozen(s32 i, s32 v)
{
    sCamFrozen[i] = v;
}
