#include "game.h"

typedef struct { u8 pad[0x20]; f32 pos; f32 vel; } S28;

struct Rec6 {
    s32 x, y, z;
    s32 rx, ry, rz;
};

struct Rec_4 {
    s32 x, y, z, a, b, c;
};

struct Ent_2 { s32 w[6]; };

extern Ent_2 sCamHistorySum[];
extern S28 sCamPitchSprings[];

void cam_init(s32 arg0)
{
    s32 i, j;

    mem_fill(sCamShakeY, 0, 0x10);
    mem_fill(sCamFrozen, 0, 0x20);
    sCamSegOrder_2 = mem_alloc_dbg_2(gPathNodeCount << 1, 0, D_800D6A70, 0x239);
    cam_build_segment_order();
    mem_fill(sCamHistorySum, 0, 0x60);
    mem_fill(sCamHistory, 0, 0x600);
    mem_fill(sCamHistoryPos, 0, 0x10);
    mem_fill(gCamZoneState_2, 0, 0x20);
    mem_fill((u8 *)sCamPitchSprings, 0, 0xa0);
    mem_fill(sCamAirBlend, 0, 0x10);
    sCamFrameCount = 0;
    for (i = 0; i < 4; i++) {
        gCamPos_4[i].a = 0;
        gCamPos_4[i].b = 0xbb0;
        gCamPos_4[i].c = 0;
        gCamTargets[i] = -1;
        for (j = 0; j < 0x78; j++) {
            sCamPitchSprings[i].vel += (2000.0f - sCamPitchSprings[i].pos) * sCamSpringUp;
            sCamPitchSprings[i].pos += sCamPitchSprings[i].vel;
            sCamPitchSprings[i].vel *= sCamDampUp;
        }
    }
}

typedef struct {
    u8 unk0[0x46];
    s16 next;
    s16 branch;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E[0x6C - 0x4E];
} Seg_5;

extern Seg_5 *gPathNodes_5;

void cam_build_segment_order(void) {
    s32 j;
    s32 i;
    s32 n;

    i = 0;
    n = 0;
    do {
        sCamSegOrder[i] = n;
        if (gPathNodes_5[i].branch != -1) {
            j = gPathNodes_5[i].branch;
            n++;
            do {
                sCamSegOrder[j] = n;
                j = gPathNodes_5[j].next;
                n++;
            } while (gPathNodes_5[j].unk4C == -1);
        } else {
            n++;
        }
        i = gPathNodes_5[i].next;
    } while (i != 0);
}

typedef struct {
    s32 f0;
    u8 pad4[0x1C - 4];
    s32 f1C[4];
    u8 pad2C[0x3C - 0x2C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x2C8 - 0x50];
    s8 f2C8[4];
    u8 pad2CC[0x760 - 0x2CC];
    u8 f760[4];
} Game_13;

typedef struct {
    s32 f0;
    u8 pad4[0x26 - 4];
    s16 f26;
    u8 pad28[0x40 - 0x28];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 f58;
} Cam_2;

typedef struct {
    u16 flags;
    s16 mode;
    u8 pad[4];
} CamFlags;

typedef struct {
    void (*f0)(void);
    void (*f4)(void *, s32 *);
    void (*f8)(void);
    u16 fC;
    u16 pad;
} CamMode;

typedef struct {
    s32 x, y, z, w, f10, f14;
} CamPos;

typedef struct {
    u8 pad[0x64];
    u8 b64;
    u8 pad2[0x6C - 0x65];
} Area;

extern CamMode gCamModes_2[];
extern CamFlags gCamZoneState_3[];
extern CamPos sCamHistorySum_2[];
extern Area *gPathNodes_6;
extern Game_13 gGame_28;
extern Cam_2 gEvent_12;

void cam_update(void) {
    s32 buf[6];
    Vec3f pos;
    s32 area;
    f32 hh;
    s32 changed, sel;
    f32 *s5;
    s32 m;
    s32 m2;
    s32 mode;
    s32 t, k;
    CamFlags *e;
    u16 f;
    CamMode *cm;
    Rider *pl;
    CamPos *c;
    Vec3i *v;
    f32 dx, dz, d;

    sel = 0;
    changed = 0;
    s5 = NULL;
    if (gPodiumMode != 0) {
        podium_update_camera();
        return;
    }
    if (gCamView < gGame_28.f3C && gGame_28.f4C != 7 && gGame_28.f4C != 2) {
        if (gRiders[gCamView].camMode != 10) {
            gGame_28.f760[gGame_28.f2C8[gCamView]] = gRiders[gCamView].camMode;
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
    if (gEvent_12.f0 == 0) {
        m2 = gEvent_12.f26;
        if (m2 == 0xFF && gCamView == 0) {
            func_8004F5C8(0);
        }
        if ((gPlayerInputs[0] & 0x10) || gCurTrack == D_800CB7B0) {
            if (m2 >= 2) {
                m2 = 1;
            }
            gEvent_12.f26 = m2;
            changed = 1;
        }
    }
    if (gGame_28.f0 & 0x10) {
        t = gGame_28.f1C[gCamView];
        sCamTarget = t;
        cam_update_rider_select(&gRiders[t], buf, t != gCamTargets[gCamView]);
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
        if (gGame_28.f4C == 9 && (gEvent_12.f58 & 0x80)) {
            changed = 1;
            gEvent_12.f58 &= ~0x80;
        }
        if (cam_find_zone() != 0) {
            gCamZoneState_3[gCamView].flags |= 2;
        } else {
            gCamZoneState_3[gCamView].flags &= ~2;
        }
        if (D_800EE140 != 0) {
            func_8002D888(&gRiders[sCamTarget], buf);
        } else {
            if ((gReplayPlayback != 0 || gCreditsMode != 0 || (gGame_28.f0 & 0x81)) &&
                (gCamZoneState_3[gCamView].flags & 2)) {
                if (!(gCamZoneState_3[gCamView].flags & 1)) {
                    gCamZoneState_3[gCamView].flags |= 1;
                    func_8002D890();
                }
                switch (gCamZoneState_3[gCamView].mode) {
                case 0:
                    cam_zone_update_dolly(&gRiders[sCamTarget], buf);
                    break;
                case 1:
                    cam_zone_update_follow(&gRiders[sCamTarget], buf);
                    break;
                }
            } else {
                { CamFlags *bs = gCamZoneState_3; e = &bs[gCamView]; }
                f = e->flags;
                if (f & 1) {
                    mode = e->mode;
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
                    if ((gPlayerInputs[k] & 0x80) && !(gPlayerInputsPrev_3[k] & 0x80) && !(gEvent_12.f58 & 0x800) &&
                        gEvent_12.f0 != 0 && gEvent_12.f40 == 0) {
                        s32 md = gRiders[sCamTarget].camMode;
                        if (md != 9 && gGameMode != 7) {
                            sel = 1;
                            { Rider *b = (Rider *)((u32)gRiders + ((md & 1) >> 1)); s5 = &b[sCamTarget].bike.vel.x; }
                            m = md;
                            if (gGameMode == 6 && gRiders[sCamTarget].finishState == 0) {
                                m ^= 6;
                            } else {
                                do {
                                    m++;
                                    if (m >= 12) {
                                        m = 0;
                                    }
                                } while (gCamModes_2[m].fC & 8);
                            }
                        }
                    }
                }
            }
            if (sel) {
                cm = &gCamModes_2[gRiders[sCamTarget].camMode];
                if (cm->fC & 2) {
                    changed = 1;
                }
                if (cm->f8 != NULL) {
                    cm->f8();
                }
                pl = &gRiders[sCamTarget];
                pl->savedCamMode = m;
                pl->camMode = m;
                gCamLastModes[gCamView] = (s16)m;
                cm = &gCamModes_2[pl->camMode];
                if (cm->fC & 1) {
                    changed = 1;
                }
                if (cm->f0 != NULL) {
                    cm->f0();
                }
            }
            {
            if (!(gCamZoneState_3[gCamView].flags & 1)) {
                cm = &gCamModes_2[gRiders[sCamTarget].camMode];
                if (gInsetViewActive != 0 && gCamView == 1) {
                    cm = &gCamModes_2[7];
                }
                if (cm->f4 != NULL) {
                    cm->f4(&gRiders[gCamTargetOverride != -1 ? gCamTargetOverride : sCamTarget], buf);
                    if (changed) {
                        if (s5 != NULL) {
                            cam_snap_to_moving(buf, s5);
                        } else {
                            cam_snap_to(buf);
                        }
                    }
                }
            }
            }
        }
    }
    t = gCamView;
    gCamPos[t].x = sCamHistorySum_2[t].x / 16;
    gCamPos[t].y = sCamHistorySum_2[t].y / 16 + sCamShakeY[t];
    gCamPos[t].z = sCamHistorySum_2[t].z / 16;
    gCamPitch[t] = sCamHistorySum_2[t].w;
    if (sCamShakeY[t] != 0) {
        dx = (f32)gRiders[t].drawPos.x - (f32)gCamPos[t].x;
        dz = (f32)gRiders[t].drawPos.z - (f32)gCamPos[t].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        gCamPitch[gCamView] += (s32)((f32)sCamShakeY[gCamView] / d * 32768.0f / 3.1415927f);
    }
    gCamRoll[gCamView] = sCamHistorySum_2[gCamView].f10;
    gCamYaw[gCamView] = sCamHistorySum_2[gCamView].f14;
    sCamFrameCount++;
    if (!(gCamZoneState_3[gCamView].flags & 1) && gRiders[gCamView].camMode != 9) {
        gCamSegFlags_2[gCamView] = 0;
        cam_clamp_to_track_2(sCamTarget);
        return;
    }
    pos.x = (f32)gCamPos[gCamView].x / 1000.0f;
    pos.y = (f32)gCamPos[gCamView].y / 1000.0f;
    pos.z = (f32)gCamPos[gCamView].z / 1000.0f;
    path_get_segment_and_t(&pos, &area, &hh, gRiders[sCamTarget].segment);
    gCamSegFlags_2[gCamView] = gPathNodes_6[area].b64;
}

typedef struct {
    f32 x, y, z;
    u8 pad0C[0x46 - 0x0C];
    s16 next;
    u8 pad48[0x64 - 0x48];
    u8 flags;
    u8 pad65[0x6C - 0x65];
} Seg_6;

extern Seg_6 *gPathNodes_7;

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
    if ((gPathNodes_7[i].flags & 1) == 0) {
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
            k = gPathNodes_7[k].next;
        }
        dx = gPathNodes_7[k].x - self->rearWheel.contact.x;
        dz = gPathNodes_7[k].z - self->rearWheel.contact.z;
        t = __builtin_sqrtf(dx * dx + dz * dz);
        f3 = -(gPathNodes_7[k].y - self->rearWheel.contact.y) * (8192.0f / t);
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

typedef struct {
    s32 flags;
    u8 pad0[0x3C - 4];
    s32 mode;
    u8 pad1[0x2A8 - 0x40];
    s32 tab[16][2];
    u8 pad2[0x408 - 0x2A8 - 0x80];
    s16 f408;
    s16 pad3;
    u8 f40C;
} Ctl_6;

extern Ctl_6 gGame_29;

void cam_update_rider_select(Rider *p, s32 *out, s32 arg2)
{
    s32 buf[10];
    s32 in[3];
    s32 res[3];
    s32 amp, yoff;
    s32 r, z;
    s32 *e;
    Ctl_6 *g = &gGame_29;

    if (g->flags & (0x400 << gCamView)) {
        amp = 4000;
        yoff = 0x672;
    } else {
        amp = 6000;
        if (g->mode == 2 && gSplitVertical != 0) {
            amp = 7200;
        }
        yoff = 0x8FC;
    }
    e = gGame_29.tab[gCamView];
    out[0] = p->bike.smoothPos.x * 1000.0f + (f32)((gCosTable[((e[0] >> 4) - 0x400) & 0xFFF] * amp) >> 14);
    out[1] = p->bike.smoothPos.y * 1000.0f + (f32)yoff;
    r = 0;
    out[2] = p->bike.smoothPos.z * 1000.0f + (f32)((gCosTable[(e[0] >> 4) & 0xFFF] * amp) >> 14);
    z = 0;
    out[3] = 0xE10;
    out[4] = 0;
    out[5] = (0x8000U - e[0]) & 0xFFFF;
    mtx3i_from_angles(buf, out[3], out[4], out[5]);
    switch (gGame_29.mode) {
    case 1:
        switch (gGame_29.f40C) {
        case 1:
            r = 0x3E - gGame_29.f408;
            break;
        case 2:
            r = 0x3E;
            break;
        case 3:
            r = gGame_29.f408;
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
    if (gGame_29.flags & (0x400 << gCamView)) {
        in[0] = 0;
        in[1] = 0;
        in[2] = 0;
    } else {
        in[0] = r + 600;
        in[1] = z;
        in[2] = 0;
    }
    vec3i_mul_mtx3i(in, buf, res);
    out[0] += res[0];
    out[1] += res[1];
    out[2] += res[2];
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

typedef struct {
    u8 pad0[0x58];
    s32 f58;
    u8 pad5C[0x6C - 0x5C];
    s16 f6C;
} State_3;

extern State_3 gEvent_13;

s32 cam_mode_soccer(Rider *bike, Rec6 *out) {
    u16 a1;
    s32 ratio;
    u16 a2;
    f32 s;
    f32 fz;
    s32 n;
    s32 z;
    s32 i;
    s32 ang, q, ang3, q3;

    n = gEvent_13.f58 & 0xF;
    if (n != 0) {
    i = n - 1;
    if (i == ((bike - gRiders) >> ((gNumPlayers - 1) >> 1))) {
        switch (n) {
        case 1:
            s = 1000.0f;
            q = gEvent_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang = gEvent_13.f6C * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * s + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent_13.f6C * s * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * s - (f32)(((0x4844 - gEvent_13.f6C * 10500 / 180) * SINT(gEvent_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * s - (f32)out->x, gSoccerBall.pos.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        case 2:
            s = 1000.0f;
            q3 = gEvent_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang3 = gEvent_13.f6C * 15000 / 180 - 8000;
            out->x = gSoccerBall.pos.x * s - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent_13.f6C * s * 3.0f / 180.0f;
            fz = gSoccerBall.pos.z * s + (f32)(((0x4844 - gEvent_13.f6C * 10500 / 180) * SINT(gEvent_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.pos.x * s - (f32)out->x, gSoccerBall.pos.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.pos.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        default:
            fatal_halt(1);
        }
    } else switch (n) {
    case 1: {
        Rider *p = &gRiders[i];
        out->x = p->bike.pos.x * 1000.0f + (f32)((COST(gEvent_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f - (f32)((SINT(gEvent_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->rx = 1000;
        out->rz = gEvent_13.f6C * 14000 / 180 - 7000;
        out->ry = 0;
        break; }
    case 2: {
        Rider *p = &gRiders[i];
        out->x = p->bike.pos.x * 1000.0f + (f32)((COST(7000 - gEvent_13.f6C * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->bike.pos.z * 1000.0f + (f32)((SINT(7000 - gEvent_13.f6C * 14000 / 180) * 625) >> 11);
        out->rx = 1000;
        out->rz = gEvent_13.f6C * 14000 / 180 + 0x64A8;
        out->ry = 0;
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

    fl = gPathNodes[param_1->segment * 0x6C + 0x64];
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

typedef struct {
    s32 x, y, z, w3, w4, w5;
} TblEnt;

void cam_mode_hill_climb(Rider *p, s32 *out)
{
    s32 v[4];
    s32 r[4];
    s32 m[10];
    s16 ang;
    s32 dist;
    TblEnt *tb;
    TblEnt *base;
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
        out[0] = r[0] + pv->x * 1000.0f;
        out[1] = r[1] + pv->y * 1000.0f + sCamHillHeight;
        out[2] = r[2] + pv->z * 1000.0f;
        out[3] = h;
        out[4] = 0;
        out[5] = rot;
    } else {
        base = sCamHistorySum;
        tb = &base[gCamView];
        out[0] = tb->x / 16;
        out[1] = tb->y / 16;
        z = tb->z;
        if (z < 0) {
            z += 0xF;
        }
        z2 = z >> 4;
        out[2] = z2;
        a = out[0] - (s32)(p->bike.pos.x * 1000.0f);
        b = (s32)(p->bike.pos.z * 1000.0f) - z2;
        d = (s32)(p->bike.pos.y * 1000.0f) + sCamHillHeight - out[1];
        math_to_polar(a, b, &ang, &dist);
        if (dist > 10000) {
            p->crashTimer += 3;
        }
        out[5] = ang;
        math_to_polar(-d, dist, &ang, &dist);
        out[3] = ang;
        out[4] = 0;
    }
    cam_smooth_to(out);
}

typedef struct { u32 flags; u8 pad4[0x48]; s32 mode; } Game_14;
typedef struct { s32 active; u8 pad4[0x22]; s16 t; } Rep;

extern Game_14 gGame_30;
extern Rep gEvent_14;

void cam_update_chase(Rider *p, s32 *out, s32 z)
{
    s32 n, k, ang, tv, zero;
    s32 rot;
    f32 f, g, nv, fc, a, b, c, d;
    s32 dy, dz;
    s32 vec[3];
    s32 res[3];
    s32 mtx[10];
    f32 *pp;
    S28 *e, *eb;
    s32 *q, *cnt;
    s32 tb;
    s32 *q2;
    f32 cs;

    k = p->airFrames;
    cnt = sCamAirBlend_2;
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
    sCamAirBlend_2[gCamView] = n;
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
    if ((zero = gGame_30.mode, rot = (u16)p->heading, zero != 9) && !(gGame_30.flags & 0x8000)) {
        if ((p->input & 0x200) && gEvent_14.active != 0) {
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
    if (gEvent_14.active == 0) {
        g = (f32)gEvent_14.t * 0.00390625f;
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
        if (!(gGame_30.flags & 0x8000)) {
            rot += tv;
            vec[0] = (f32)tb * g;
            vec[1] = g * (f32)dy;
            vec[2] = (f32)z + g * (f32)dz;
        }
    }
    mtx3i_from_angles(mtx, ang, 0, (s16)rot);
    vec3i_mul_mtx3i(vec, mtx, res);
    pp = &p->bike.pos.x;
    out[0] = (f32)res[0] + pp[0] * 1000.0f;
    out[1] = (f32)res[1] + pp[1] * 1000.0f + 600.0f;
    out[2] = (f32)res[2] + pp[2] * 1000.0f;
    out[3] = ang - 500;
    out[5] = (s16)rot;
    out[4] = 0;
    if (gGame_30.mode != 9) {
        if ((p->input & 0x200) || ((gPlayerInputs_2 & 0x10) && gEvent_14.active == 0 && !(gGame_30.flags & 0x8000))) {
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

void cam_mode_fall_view(Rider *e, Rec_4 *r)
{
    Rec_4 *b;
    s32 i;
    s32 d;

    b = sCamHistorySum;
    i = e - gRiders;
    r->x = b[i].x / 16;
    r->y = b[i].y / 16;
    r->z = b[i].z / 16;
    d = e->drawPos.y - r->y;
    r->a = b[i].a;
    r->c = b[i].c;
    r->b = 0;
    cam_snap_to(r);
    if (gGameMode == 7) {
        e->timeScale = 0.5f;
        if (d < -12000) {
            e->crashTimer = 10000;
            e->timeScale = 1.0f;
        }
    }
}

typedef struct { f32 v[16]; } Row_3;

extern Row_3 gSurfaceTable_5[];

void cam_start_fall_view(Rider *p)
{
    Rec_4 loc;
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
            loc.c = ang;
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
            f = gSurfaceTable_5[(gGroundSurface >> 4) & 0xF].v[7];
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
            loc.a = 0x1000;
            loc.b = 0;
            loc.c = ang;
        }
        gCamView = idx = p - gRiders;
        cam_snap_to(&loc);
        func_8004F064(idx);
    }
}

void cam_mode_stunt_pip(Rider *p, Rec6 *out)
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
        out->rx = 0x800;
        out->rz = p->rot[2] + gCamPipOrbitAngle;
        out->ry = 0;
        cam_snap_to(out);
    }
}

extern Ent_2 sCamHistory_2[][16];

void cam_smooth_to(Ent_2 *p)
{
    s32 a, b, c, k, j, idx;
    Ent_2 *h, *s, *q;

    if (sCamFrameCount == 0) {
        cam_snap_to(p);
        return;
    }
    c = b = a = 0;
    idx = sCamHistoryPos[gCamView];
        sCamHistorySum[gCamView].w[0] -= sCamHistory_2[gCamView][idx].w[0];
    sCamHistorySum[gCamView].w[1] -= sCamHistory_2[gCamView][idx].w[1];
    sCamHistorySum[gCamView].w[2] -= sCamHistory_2[gCamView][idx].w[2];
    sCamHistory_2[gCamView][idx] = *p;
        sCamHistorySum[gCamView].w[0] += sCamHistory_2[gCamView][idx].w[0];
    sCamHistorySum[gCamView].w[1] += sCamHistory_2[gCamView][idx].w[1];
    sCamHistorySum[gCamView].w[2] += sCamHistory_2[gCamView][idx].w[2];
    j = idx - 1;
    for (k = 0; k < 15; k++) {
        if (j < 0) j += 16;
                a += (s16)(sCamHistory_2[gCamView][j].w[5] - sCamHistory_2[gCamView][idx].w[5]);
        b += (s16)(sCamHistory_2[gCamView][j].w[4] - sCamHistory_2[gCamView][idx].w[4]);
        c += (s16)(sCamHistory_2[gCamView][j].w[3] - sCamHistory_2[gCamView][idx].w[3]);
        j--;
    }
    sCamHistorySum[gCamView].w[3] = c / 16 + sCamHistory_2[gCamView][idx].w[3];
    sCamHistorySum[gCamView].w[4] = b / 16 + sCamHistory_2[gCamView][idx].w[4];
    sCamHistorySum[gCamView].w[5] = a / 16 + sCamHistory_2[gCamView][idx].w[5];
    sCamHistoryPos[gCamView] = (idx + 1) % 16;
}

void cam_snap_to(Ent_2 *src)
{
    s32 i;
    s32 t2;
    s32 off;
    s32 v;
    u8 *base;
    s32 o;
    Ent_2 *d;

    i = 0;
    base = sCamHistory;
    do {
        v = gCamView;
        o = (i * 0x18 + v * 0x180) + (s32)base;
        i++;
        *(Ent_2 *)o = *src;
    } while (i < 16);
    sCamHistorySum[gCamView] = *src;
    d = &sCamHistorySum[gCamView];
    d->w[0] <<= 4;
    d->w[1] <<= 4;
    d->w[2] <<= 4;
}

struct S {
    s32 x, y, z, w, u, v;
};

extern S sCamHistory_3[][16];
extern S sCamHistorySum_3[];

void cam_snap_to_moving(S *p, f32 *v)
{
    s32 i;
    s32 sx, sy, sz;

    sx = sy = sz = 0;
    for (i = 15; i >= 0; i--) {
        sCamHistory_3[gCamView][i] = *p;
        sx += p->x;
        sy += p->y;
        sz += p->z;
        p->x -= (s32)(v[0] * 16.666666f);
        p->y -= (s32)(v[1] * 16.666666f);
        p->z -= (s32)(v[2] * 16.666666f);
    }
    sCamHistorySum_3[gCamView] = *p;
    sCamHistorySum_3[gCamView].x = sx;
    sCamHistorySum_3[gCamView].y = sy;
    sCamHistorySum_3[gCamView].z = sz;
    sCamHistoryPos[gCamView] = 0;
}

typedef struct RecA {
    u8 pad0[0x14];
    s16 i0;
    s16 i1;
    u32 flags;
} RecA;

typedef struct RecB {
    u8 pad0[0x18];
    u32 flags;
    s32 i0;
    s32 i1;
} RecB;

typedef struct Obj_16 {
    u8 pad0[0x74];
    s16 nA;
    s16 nB;
    RecA *la;
    RecB *lb;
} Obj_16;

typedef struct Flag {
    u16 f0;
    s16 h2;
    void *p4;
} Flag;

typedef struct G {
    u8 pad0[0x3C];
    s32 f3C;
} G;

typedef struct Node_3 {
    u8 pad0[0x4E];
    s16 next;
    u8 pad1[0x6C - 0x50];
} Node_3;

extern Flag gCamZoneState_4[];
extern Node_3 *gPathNodes_8;
extern Obj_16 *gCurTrack_20;
extern Obj_16 *gCurTrack_43;

s32 cam_find_zone(void)
{
    s32 pass = 0;
    s32 cur;
    s32 k;
    s32 a, b, v;
    s32 d;
    s32 off;
    s16 *pv;
    Obj_16 *P;
    u16 *pf;
    Flag *fl;
    G *g;
    s16 *tab;
    void *rec;
    s32 a2, b2;
    void *rec2;

    if (gCamView == 0) {
        D_800C97E4 = -1;
    }
    g = &gEvent;
    fl = gCamZoneState_4;
    P = gCurTrack_20;
    tab = sCamSegOrder;
    cur = gRiders[sCamTarget].segment;
    do {
        if (cur < 0) {
            goto done;
        }
        if (P->la != NULL && (k = 0, P->nA > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            pf = (u16 *)(gCamView * 8 + (s32)fl);
            off = k;
        loopA:
            {
                if (pass == 0 || ((*(RecA *)(off + (s32)P->la)).flags & 0x40)) {
                    b = tab[(*(RecA *)(off + (s32)P->la)).i1];
                    a = tab[(*(RecA *)(off + (s32)P->la)).i0];
                    if ((b < a && (*pv >= a || *pv < b)) ||
                        (a < b && *pv >= a && *pv < b)) {
                        if (g->f3C != 0 || !((*(RecA *)(off + (s32)P->la)).flags & 2)) {
                            ((s16 *)pf)[1] = 1;
                            rec = (RecA *)((u8 *)P->la + off);
                            v = rec != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec;
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
                if (pv == (s16 *)tab) v = 1; else v = 2;
                off += sizeof(RecA);
                k++;
            }
            if (k < gCurTrack_20->nA) goto loopA;
        }
        if ((void *)g == (void *)P) k = 1; else k = 2;
        if (pass == 0) k = 1; else k = 2;
        if (tab == 0) k = 1; else k = 2;
        if (P->lb != NULL && (k = 0, P->nB > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            d = gCamView;
            pf = (u16 *)(d * 8 + (s32)fl);
            off = k;
        loopB:
            {
                if (pass == 0 || ((*(RecB *)(off + (s32)P->lb)).flags & 0x40)) {
                    b2 = tab[(*(RecB *)(off + (s32)P->lb)).i1];
                    a2 = tab[(*(RecB *)(off + (s32)P->lb)).i0];
                    if ((b2 < a2 && (*pv >= a2 || *pv < b2)) ||
                        (a2 < b2 && *pv >= a2 && *pv < b2)) {
                        if (g->f3C != 0 || !((*(RecB *)(off + (s32)P->lb)).flags & 2)) {
                            ((s16 *)pf)[1] = 0;
                            rec2 = (RecB *)((u8 *)P->lb + off);
                            v = rec2 != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec2;
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
                off += sizeof(RecB);
                k++;
            }
            if (k < gCurTrack_43->nB) goto loopB;
        }
        pass++;
        cur = gPathNodes_8[cur].next;
    } while (pass < 2);
done:
    return 0;
}

typedef struct {
    u8 pad0[0x4E];
    s16 start;
    u8 pad50[0x6C - 0x50];
} Seg_7;

typedef struct {
    u16 flags;
    u8 pad2[6];
} Trk_8;

extern Trk_8 gCamZoneState_5[];
extern Seg_7 *gPathNodes_9;

f32 cam_zone_get_progress(Rider *p, s32 a, s32 b)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (gCamZoneState_5[gCamView].flags & 8) {
        len = sCamSegOrder[b] - sCamSegOrder[a];
        seg = path_find_segment(&p->bike.pos, gPathNodes_9[p->segment].start, 15, 1);
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
    s32 w0;
    u16 h4;
    u16 h6;
    s32 w8;
    s32 w12;
    s32 w16;
    s16 s20;
    s16 s22;
} Track_7;

typedef struct {
    s16 h[2];
    s32 w;
} Tmp30;

typedef struct {
    u16 flags;
    u16 pad;
    Track_7 *node;
} Slot_5;

extern Slot_5 gCamZoneState_6[];

void cam_zone_update_follow(Rider *param_1, s32 *param_2)
{
    u16 uVar1;
    Track_7 *piVar2;
    f32 fT;
    f32 fA;
    f32 fB;
    u32 uA;
    u32 uB;
    u32 sum4;
    s32 vS;
    s32 iVar5;
    s32 local_78[4];
    s32 local_68[3];
    s32 aiStack_58[10];
    Tmp30 t30;
    s16 local_28[4];

    uVar1 = gCamZoneState_6[gCamView].flags;
    if (uVar1 & 4) {
        gCamZoneState_6[gCamView].flags = uVar1 & 0xFFFB;
    }
    piVar2 = gCamZoneState_6[gCamView].node;
    fT = cam_zone_get_progress(param_1, piVar2->s20, piVar2->s22);
    vS = piVar2->w0 + (s32)((f32)(piVar2->w8 - piVar2->w0) * fT);
    fA = (f32)piVar2->w12 * fT;
    uA = (u32)fA;
    fB = (f32)piVar2->w16 * fT;
    sum4 = piVar2->h4 + uA;
    uB = (u32)fB;
    mtx3i_from_angles(aiStack_58, (-((piVar2->h6 + uB) & 0xFFFF)) + 1000, 0,
                  ((u16)param_1->rot[2] + sum4) & 0xFFFF);
    local_78[0] = 0;
    local_78[1] = 0;
    local_78[2] = vS;
    vec3i_mul_mtx3i(local_78, aiStack_58, local_68);
    param_2[0] = (s32)(param_1->bike.smoothPos.x * 1000.0f + (f32)local_68[0]);
    param_2[1] = (s32)(param_1->bike.smoothPos.y * 1000.0f + (f32)local_68[1] + 1000.0f);
    iVar5 = (s32)(param_1->bike.smoothPos.z * 1000.0f + (f32)local_68[2]);
    param_2[2] = iVar5;
    math_to_polar((s32)(param_1->bike.smoothPos.x * 1000.0f - (f32)param_2[0]),
                  (s32)(param_1->bike.smoothPos.z * 1000.0f - (f32)iVar5), t30.h, &t30.w);
    math_to_polar(t30.w, (s32)((param_1->bike.smoothPos.y + 0.7f) * 1000.0f - (f32)param_2[1]), local_28, &t30.w);
    param_2[4] = 0;
    {
        s32 ta = local_28[0] + -0x4000;
        s32 tb = -t30.h[0];
        param_2[3] = ta;
        param_2[5] = tb;
    }
    cam_snap_to(param_2);
}

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
    s32 flags;
    s32 p7, p8;
} Obj_19;

typedef struct {
    u16 flags;
    u16 pad;
    Obj_19 *obj;
} Slot_6;

extern Slot_6 gCamZoneState_7[];

void cam_zone_update_dolly(Rider *act, Rec6 *out)
{
    s32 m[3];
    s32 mtx[10];
    s16 a1;
    s32 r1;
    s16 a2;
    s16 b1;
    s32 r2;
    s16 b2;
    Slot_6 *sl;
    s32 idx;
    Obj_19 *o;
    u16 sf;
    s32 f;
    s32 rx, rz;
    f32 t, u, v;

    sl = gCamZoneState_7;
    sl += gCamView;
    sf = sl->flags;
    o = sl->obj;
    if (sf & 4) {
        sl->flags = sf & ~4;
        f = o->flags;
        if (f & 8) {
            if (f & 1) {
                if (f & 4) {
                    o->x = (f32)gCamPos[gCamView].x - act->bike.smoothPos.x * 1000.0f;
                    o->y = (f32)gCamPos[gCamView].y - act->bike.smoothPos.y * 1000.0f;
                    o->z = (f32)gCamPos[gCamView].z - act->bike.smoothPos.z * 1000.0f;
                } else {
                    o->x = gCamPos[gCamView].x;
                    o->y = gCamPos[gCamView].y;
                    o->z = gCamPos[gCamView].z;
                }
            } else {
                o->x = gCamPos[gCamView].x;
                o->y = gCamPos[gCamView].y;
                o->z = gCamPos[gCamView].z;
            }
            f = o->flags;
        }
        if (f & 0x10) {
            mtx3i_from_angles(mtx, o->rx, 0, o->rz);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            vec3i_mul_mtx3i(m, mtx, &sCamZoneOffset);
        } else if (f & 4) {
            math_to_polar(-o->x, o->z, &a1, &r1);
            math_to_polar(r1, -o->y + 700, &a2, &r1);
            mtx3i_from_angles(mtx, a2 - 0x4000, 0, -a1);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            vec3i_mul_mtx3i(m, mtx, &sCamZoneOffset);
        }
    }
    t = cam_zone_get_progress(act, o->p7, o->p8);
    f = o->flags;
    if (f & 4) {
        out->x = (f32)o->x + act->bike.smoothPos.x * 1000.0f;
        out->y = (f32)o->y + act->bike.smoothPos.y * 1000.0f;
        out->z = (f32)o->z + act->bike.smoothPos.z * 1000.0f;
        if (o->flags & 0x20) {
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
        out->x = (f32)o->x + (f32)(o->rx - o->x) * t;
        out->y = (f32)o->y + (f32)(o->ry - o->y) * t;
        out->z = (f32)o->z + (f32)(o->rz - o->z) * t;
    } else if (f & 0x10) {
        if (f & 0x20) {
            out->x = (f32)(o->x + sCamZoneOffset.x) - (f32)sCamZoneOffset.x * t;
            out->y = (f32)(o->y + sCamZoneOffset.y) - (f32)sCamZoneOffset.y * t;
            out->z = (f32)(o->z + sCamZoneOffset.z) - (f32)sCamZoneOffset.z * t;
        } else {
            out->x = (f32)o->x + (f32)sCamZoneOffset.x * t;
            out->y = (f32)o->y + (f32)sCamZoneOffset.y * t;
            out->z = (f32)o->z + (f32)sCamZoneOffset.z * t;
        }
    } else {
        out->x = o->x;
        out->y = o->y;
        out->z = o->z;
    }
    if (o->flags & 0x10) {
        out->rx = o->rx;
        out->ry = 0;
        out->rz = o->rz;
    } else {
        math_to_polar(act->bike.smoothPos.x * 1000.0f - (f32)out->x, act->bike.smoothPos.z * 1000.0f - (f32)out->z, &b1, &r2);
        math_to_polar(r2, (act->bike.smoothPos.y + 0.7f) * 1000.0f - (f32)out->y, &b2, &r2);
        out->ry = 0;
        out->rx = b2 - 0x4000;
        out->rz = -b1;
    }
    cam_snap_to(out);
}

typedef struct {
    u8 pad0[0x14];
    f32 fa;
    f32 fb;
    u8 pad1[0x50];
} Rec_13;

extern Rec_13 *gPathNodes_10;

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
    xb = (gPathNodes_10[i2].fb * (1.0f - p2) + gPathNodes_10[i1].fb * p2) * 16384.0f;
    xa = (gPathNodes_10[i2].fa * (1.0f - p2) + gPathNodes_10[i1].fa * p2) * 16384.0f;
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
    *param_3 = gPathNodes_2[i0].x + (gPathNodes_2[i1].x - gPathNodes_2[i0].x) * param_2;
    *param_4 = gPathNodes_2[i0].y + (gPathNodes_2[i1].y - gPathNodes_2[i0].y) * param_2;
    *param_5 = gPathNodes_2[i0].z + (gPathNodes_2[i1].z - gPathNodes_2[i0].z) * param_2;
}

void cam_collide_walls(s32 a, s32 b, s32 c)
{
    Rider *nd, *nd0;
    s32 *tb;
    Vec3f p;
    f32 pt[3];
    f32 n[3];
    f32 q[4];
    f32 tt[2];
    f32 sum, len, L, t;
    s32 *dst;

    nd0 = gRiders;
    nd = nd0 + a;
    tb = gCamPos;
    p = nd->bike.pos;
    dst = &tb[b * 3];
    pt[0] = (f32)dst[0] / 1000.0f;
    pt[1] = (f32)dst[1] / 1000.0f;
    pt[2] = (f32)dst[2] / 1000.0f;
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
        dst[0] = (s32)(pt[0] * 1000.0f);
        dst[1] = (s32)(pt[1] * 1000.0f);
        tt[0] = t;
        dst[2] = (s32)(pt[2] * 1000.0f);
    }
    cam_keep_above_ground(a, b);
}

typedef struct {
    s16 v;
} Ang;

typedef struct {
    s32 x, y, z;
    s32 pad0C, pad10;
    s32 w14;
} Seg_8;

extern Seg_8 sCamHistory_4[][16];
extern Seg_8 sCamHistorySum_4[];

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
            sCamHistory_4[p1][i].x += p3;
            sCamHistory_4[p1][i].z += p4;
            sCamHistory_4[p1][i].w14 -= d;
        }
        sCamHistorySum_4[p1].x += p3 * 16;
        sCamHistorySum_4[p1].z += p4 * 16;
    }
}

typedef struct Rec_14 {
    f32 x;
    f32 y;
    f32 z;
    u8 p0c[8];
    f32 f14;
    f32 f18;
    u8 p1c[0x2a];
    s16 s46;
    u8 p48[10];
    s16 s52;
    s16 s54;
    s16 s56;
    u8 p58[12];
    u8 b64;
    u8 p65[7];
} Rec_14;

typedef struct Ent_25 {
    s32 f00, f04, f08, f0c, f10, f14;
} Ent_25;

typedef struct { u8 p0[0x2C]; s32 f2C; u8 p30[0xA0 - 0x30]; } Trk_9;

extern Trk_9 *gCurTrack_21;
extern Ent_25 sCamHistorySum_5[];
extern Ent_25 sCamHistory_5[][16];
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
    Rec_14 *pr, *pn;
    s32 nx;
    Rec_14 *tab;
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
        pr = (Rec_14 *)(idx * sizeof(Rec_14) + (u32)tab);
        nx = pr->s46;
        gCamSegFlags_2[n] = pr->b64;
        if (gCurTrack_21->f2C == 0) {
            s32 id = gCurTrack_21 - gTracks_9;
            if (id == 0x18 || id == 0x15 || id == 0x16) {
                cam_collide_walls(m, n, 1);
            }
            return;
        }
        D_800E7F98 = 0;
        if (pr->s56 < 0) {
            cam_collide_walls(m, n, 0);
            return;
        }
        if (pr->s56 > 0) {
            pn = (Rec_14 *)(nx * sizeof(Rec_14) + (u32)tab);
            if (pn->s56 > 0) {
                f32 ya, yb;
                D_800E7F98 = 1;
                ya = pr->y * 1000.0f + (f32)pr->s56 * 16.0f;
                yb = pn->y * 1000.0f + (f32)pn->s56 * 16.0f;
                lim = (s32)(lerpi(pr->s52, pn->s52, t) * 16.0f);
                top = (s32)(lerpi(pr->s54, pn->s54, t) * 16.0f);
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
                        sCamHistory_5[n][k].f04 += h - gCamPos[n].y;
                        sCamHistory_5[n][k].f0c -= a1 - a0;
                    }
                    sCamHistorySum_5[n].f04 += (h - gCamPos[n].y) * 16;
                    gCamPos[n].y = h;
                }
                {
                Rec_14 *pr2, *pn2;
                Rec_14 *tab2;
                f32 ax, az;
                tab2 = gPathNodes;
                pr2 = (Rec_14 *)(idx * sizeof(Rec_14) + (u32)tab2);
                nx = pr2->s46;
                pn2 = (Rec_14 *)(nx * sizeof(Rec_14) + (u32)tab2);
                g = lerp(pr2->f14, pn2->f14, t);
                f = lerp(pr2->f18, pn2->f18, t);
                ax = pr2->x * 1000.0f;
                az = pr2->z * 1000.0f;
                dz = (s32)(lerp((f32)gCamPos[n].x - ax, (f32)gCamPos[n].x - pn2->x * 1000.0f, t) * g +
                           lerp((f32)gCamPos[n].z - az, (f32)gCamPos[n].z - pn2->z * 1000.0f, t) * f);
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
    u32 bp = (u32)gCamPos;
    s32 off = param_2 * 12;
    s32 *t = (s32 *)(off + (s32)bp);
    s32 iVar1;
    f32 r;
    f32 fVar2, fVar4;
    f32 fVar3;
    f32 dz;
    s32 bw;
    s32 iStack2c;

    iVar1 = height_get_ground(t[0], t[1] + 0x1F4, t[2]);
    iVar1 = iVar1 + 200;
    if (gGroundFound != 0 && t[1] < iVar1) {
        fVar3 = gRiders[param_1].bike.smoothPos.x * 1000.0f - (f32)t[0];
        dz = gRiders[param_1].bike.smoothPos.z * 1000.0f - (f32)t[2];
        r = sqrtf(fVar3 * fVar3 + dz * dz);
        fVar4 = (f32)gCamPitch[param_2] * 3.1415925f * 3.0517578e-05f;
        fVar3 = r;
        fVar2 = sinf(fVar4);
        fVar4 = cosf(fVar4);
        fVar4 = (fVar2 / fVar4) * fVar3 - (f32)(t[1] - iVar1);
        math_to_polar((s32)fVar4, (s32)fVar3, (s16 *)&bw, &iStack2c);
        gCamPitch[param_2] = (s32)((s16 *)&bw)[0];
        t[1] = iVar1;
    }
}

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 pad[3];
} E18;

extern E18 sCamHistorySum_6[];
extern E18 sCamHistory_6[][16];

void cam_shift_z(s32 p, s32 q)
{
    s32 i;

    sCamHistorySum_6[p].c += q * 16;
    for (i = 0; i < 16; i++) {
        sCamHistory_6[p][i].c += q;
    }
    gCamView = p;
    cam_update();
    gGroundFound = 0;
    height_get_ground(gRiders[p].drawPos.x, gRiders[p].drawPos.y, gRiders[p].drawPos.z);
    if (gGroundFound != 0) {
        gRiderSection[p] = gGroundArea;
    }
}

void cam_mode_chase_7m(Rider *param_1, int *param_2)
{
    cam_update_chase(param_1, param_2, -7000);
}

void cam_mode_chase_far_alt(Rider *a, s32 *b)
{
    cam_update_chase(a, b, -10000);
}

void cam_mode_side_view(Rider *param_1, s32 *param_2)
{
    f32 k = 1000.0f;

    param_2[0] = (s32)-sCamSideViewParams[0];
    param_2[1] = (s32)(param_1->bike.smoothPos.y * k + sCamSideViewParams[1]);
    param_2[2] = (s32)(param_1->bike.smoothPos.z * k - sCamSideViewParams[2]);
    param_2[3] = (s32)sCamSideViewParams[4];
    param_2[5] = (s32)-sCamSideViewParams[3];
    param_2[4] = 0;
    cam_snap_to(param_2);
}

struct Out_2 {
    s32 w0, w4, w8, wC, w10, w14;
};

void func_8002D528(Rider *o, Out_2 *out) {
    out->w0 = o->bike.smoothPos.x * 1000.0f + -28000.0f;
    out->w4 = 0x5BCC;
    out->w8 = o->bike.smoothPos.z * 1000.0f - 10000.0f;
    out->wC = 0x1800;
    out->w14 = -0x4000;
    out->w10 = 0;
    cam_smooth_to(out);
}

void cam_mode_chase_near(Rider *p, s32 *p2) {
    s32 v = -0x9C4;

    if (p->crashState != 0) {
        v = -5000;
    }
    if (p->fwdSpeed < 0.0f) {
        v = (s32)((f32)v + p->fwdSpeed * 250.0f);
    }
    cam_update_chase(p, p2, v);
}

void cam_mode_chase_mid(Rider *param_1, s32 *param_2)
{
    cam_update_chase(param_1, param_2, -5000);
}

void cam_mode_chase_far(Rider *a, s32 *b) {
    cam_update_chase(a, b, -10000);
}

typedef struct { u8 p[0x2C]; s32 f2c; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *gCurTrack_22;

void cam_mode_celebration(Rider *o, s32 *out)
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
    out[0] = (s32)((f32)st.l50 + o->bike.smoothPos.x * 1000.0f);
    out[1] = (s32)((f32)st.l4c + o->bike.smoothPos.y * 1000.0f + 750.0f);
    out[2] = (s32)((f32)st.l48 + o->bike.smoothPos.z * 1000.0f);
    out[3] = it;
    out[5] = sv;
    out[4] = 0;
    cam_snap_to(out);
}

void cam_mode_first_person(Rider *p1, s32 *p2)
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
    p2[0] = (s32)((f32)o[0] + p1->bike.smoothPos.x * 1000.0f);
    p2[1] = (s32)((f32)o[1] + p1->bike.smoothPos.y * 1000.0f);
    p2[2] = (s32)((f32)o[2] + p1->bike.smoothPos.z * 1000.0f);
    p2[3] = p1->drawRot[0] / 2;
    p2[5] = p1->drawRot[2];
    v = p1->drawRot[1];
    if (v < 0) {
        v += 3;
    }
    p2[4] = v >> 2;
    cam_snap_to(p2);
}

void func_8002D888(Rider *arg0, s32 *arg1)
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
    u8 *base;
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
    r = *(f32 *)(base + i2 * 0x6c + 0x24) * (1.0f - f) + *(f32 *)(base + i1 * 0x6c + 0x24) * f;
    *out = (s16)((r * 32768.0f) / 3.1415927f);
}

void cam_set_frozen(s32 i, s32 v)
{
    sCamFrozen[i] = v;
}
