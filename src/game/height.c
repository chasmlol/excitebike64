#include "game.h"

typedef struct { u8 pad[0x20]; f32 pos; f32 vel; } S28;

struct Player_10 {
    s32 x, y, z;
    u8 pad0C[0x54 - 0xC];
    f32 f54;
    u8 pad58[0x3FF - 0x58];
    u8 b3FF;
    u8 pad400[0x5A8 - 0x400];
    s16 f5A8;
    s16 f5AA;
    u8 pad5AC[0x628 - 0x5AC];
    s16 f628;
    u8 pad62A[0x7E0 - 0x62A];
};

struct Rec6 {
    s32 x, y, z;
    s32 rx, ry, rz;
};

struct Rec_4 {
    s32 x, y, z, a, b, c;
};

struct Ent_2 { s32 w[6]; };

typedef struct {
    s16 x, z;
} Pt;

typedef struct {
    u16 f0, f2, f4;
    u16 n;
    Pt pts[1];
} Poly;

typedef struct { u16 n; u16 id[1]; } Cell;
typedef struct { s32 id; s32 n; s32 v[3][3]; } CTri;

typedef struct {
    u8 pad[4];
    s16 x, z;
} Ent_28;

typedef struct {
    s32 x, y, z, w, h, f14, f18;
} Node_5;

typedef struct { s16 x, z, y, h; } Vtx8;

typedef struct {
    s32 x, d, y;
} Pt_3;

extern Ent_2 sCamHistorySum[];
extern S28 sCamPitchSprings[];
extern Poly **sSurfacePolys;
extern Cell **sHeightGrid;
extern CTri gGroundTris[];
extern Ent_28 *sWallEdges[];
extern Node_5 sDesertObjs[];
extern Pt_3 *sStars;

static inline void tri(s32 id, s32 n, s32 ax, s32 az, s32 bx, s32 bz, s32 cx, s32 cz) {
    gGroundTris[gGroundTriCount].id = id;
    gGroundTris[gGroundTriCount].n = n;
    gGroundTris[gGroundTriCount].v[0][0] = ax * 2000;
    gGroundTris[gGroundTriCount].v[0][1] = gDesertHeightMap[((az & 0x1FF) << 9) | (ax & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[0][2] = az * 2000;
    gGroundTris[gGroundTriCount].v[1][0] = bx * 2000;
    gGroundTris[gGroundTriCount].v[1][1] = gDesertHeightMap[((bz & 0x1FF) << 9) | (bx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[1][2] = bz * 2000;
    gGroundTris[gGroundTriCount].v[2][0] = cx * 2000;
    gGroundTris[gGroundTriCount].v[2][1] = gDesertHeightMap[((cz & 0x1FF) << 9) | (cx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[2][2] = cz * 2000;
    gGroundTriCount++;
}

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
extern Player_10 gRiders_25[];
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
    Player_10 *pl;
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
        if (gRiders_25[gCamView].f5AA != 10) {
            gGame_28.f760[gGame_28.f2C8[gCamView]] = gRiders_25[gCamView].f5AA;
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
        cam_update_rider_select(&gRiders_25[t], buf, t != gCamTargets[gCamView]);
        gCamTargets[gCamView] = sCamTarget;
    } else {
        {
            s32 u = sCamTarget;
            s32 *bs = gCamTargets;
            s32 kk = gCamView;
            s32 n;
            if (u != bs[kk]) changed = 1;
            bs[kk] = u;
            n = gRiders_25[u].f5AA;
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
            func_8002D888(&gRiders_25[sCamTarget], buf);
        } else {
            if ((gReplayPlayback != 0 || gCreditsMode != 0 || (gGame_28.f0 & 0x81)) &&
                (gCamZoneState_3[gCamView].flags & 2)) {
                if (!(gCamZoneState_3[gCamView].flags & 1)) {
                    gCamZoneState_3[gCamView].flags |= 1;
                    func_8002D890();
                }
                switch (gCamZoneState_3[gCamView].mode) {
                case 0:
                    cam_zone_update_dolly(&gRiders_25[sCamTarget], buf);
                    break;
                case 1:
                    cam_zone_update_follow(&gRiders_25[sCamTarget], buf);
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
                    m = gRiders_25[sCamTarget].f5AA;
                    sel = 1;
                } else {
                    k = gCamView;
                    if (gGhostRace != 0 && gGhostSlot == 0) {
                        k = 1;
                    }
                    if ((gPlayerInputs[k] & 0x80) && !(gPlayerInputsPrev_3[k] & 0x80) && !(gEvent_12.f58 & 0x800) &&
                        gEvent_12.f0 != 0 && gEvent_12.f40 == 0) {
                        s32 md = gRiders_25[sCamTarget].f5AA;
                        if (md != 9 && gGameMode != 7) {
                            sel = 1;
                            { Player_10 *b = (Player_10 *)((u32)gRiders_25 + ((md & 1) >> 1)); s5 = &b[sCamTarget].f54; }
                            m = md;
                            if (gGameMode == 6 && gRiders_25[sCamTarget].b3FF == 0) {
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
                cm = &gCamModes_2[gRiders_25[sCamTarget].f5AA];
                if (cm->fC & 2) {
                    changed = 1;
                }
                if (cm->f8 != NULL) {
                    cm->f8();
                }
                pl = &gRiders_25[sCamTarget];
                pl->f5A8 = m;
                pl->f5AA = m;
                gCamLastModes[gCamView] = (s16)m;
                cm = &gCamModes_2[pl->f5AA];
                if (cm->fC & 1) {
                    changed = 1;
                }
                if (cm->f0 != NULL) {
                    cm->f0();
                }
            }
            {
            if (!(gCamZoneState_3[gCamView].flags & 1)) {
                cm = &gCamModes_2[gRiders_25[sCamTarget].f5AA];
                if (gInsetViewActive != 0 && gCamView == 1) {
                    cm = &gCamModes_2[7];
                }
                if (cm->f4 != NULL) {
                    cm->f4(&gRiders_25[gCamTargetOverride != -1 ? gCamTargetOverride : sCamTarget], buf);
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
        dx = (f32)gRiders_25[t].x - (f32)gCamPos[t].x;
        dz = (f32)gRiders_25[t].z - (f32)gCamPos[t].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        gCamPitch[gCamView] += (s32)((f32)sCamShakeY[gCamView] / d * 32768.0f / 3.1415927f);
    }
    gCamRoll[gCamView] = sCamHistorySum_2[gCamView].f10;
    gCamYaw[gCamView] = sCamHistorySum_2[gCamView].f14;
    sCamFrameCount++;
    if (!(gCamZoneState_3[gCamView].flags & 1) && gRiders_25[gCamView].f5AA != 9) {
        gCamSegFlags_2[gCamView] = 0;
        cam_clamp_to_track_2(sCamTarget);
        return;
    }
    pos.x = (f32)gCamPos[gCamView].x / 1000.0f;
    pos.y = (f32)gCamPos[gCamView].y / 1000.0f;
    pos.z = (f32)gCamPos[gCamView].z / 1000.0f;
    path_get_segment_and_t(&pos, &area, &hh, gRiders_25[sCamTarget].f628);
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

struct Obj_13 {
    u8 pad0[0x54];
    f32 vx;
    f32 f58;
    f32 vz;
    u8 pad60[0xB8 - 0x60];
    f32 dx;
    f32 fBC;
    f32 dz;
    u8 padC4[0x416 - 0xC4];
    s16 f416;
    u8 pad418[0x494 - 0x418];
    f32 px, py, pz;
    u8 pad4A0[0x4C0 - 0x4A0];
    f32 f4C0;
    f32 f4C4;
    f32 f4C8;
    u8 pad4CC[0x628 - 0x4CC];
    s16 seg;
};

extern Seg_6 *gPathNodes_7;

f32 cam_calc_ground_pitch(Obj_13 *self)
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

    i = self->seg;
    f3 = 0.0f;
    if ((gPathNodes_7[i].flags & 1) == 0) {
        f0 = self->vx * self->dx + self->vz * self->dz;
        if (f0 < 2.0f) {
            f0 = 2.0f;
        }
        if (16.0f < f0) {
            f0 = 16.0f;
        }
        arr[0] = self->px + self->dx * f0;
        arr[2] = self->pz + self->dz * f0;
        arr[1] = self->py;
        t = bike_get_ground_height(arr);
        if (gGroundFound != 0) {
            f3 = (self->py - t) * (8192.0f / f0);
        }
    } else {
        k = i;
        for (n = 5; n >= 0; n--) {
            k = gPathNodes_7[k].next;
        }
        dx = gPathNodes_7[k].x - self->px;
        dz = gPathNodes_7[k].z - self->pz;
        t = __builtin_sqrtf(dx * dx + dz * dz);
        f3 = -(gPathNodes_7[k].y - self->py) * (8192.0f / t);
    }
    if (f3 < -2048.0f) {
        f3 = -2048.0f;
    } else if (2048.0f < f3) {
        f3 = 2048.0f;
    }
    if (self->f416 == 0) {
        f3 = f3 + 2000.0f;
    } else {
        f3 = f3 + 0.0f;
    }
    q1 = self->vx;
    q2 = self->vz;
    f0 = __builtin_sqrtf(q1 * q1 + q2 * q2);
    if (f0 == 0.0f) {
        f0 = 1.0f;
    }
    q1 = q1 / f0;
    q2 = q2 / f0;
    f0 = self->f4C0 * q1 + self->f4C8 * q2;
    if (0.2f < f0) {
        f0 = (f32)((s16)math_asini((s32)(f0 * 16384.0f)) + 0x400);
        if (f3 < f0) {
            f3 = f0;
        }
    }
    return f3;
}

#define FLD(off) (*(f32 *)(p + (off)))

f32 cam_calc_air_pitch(u8 *p)
{
    f32 fVar2, fVar3;
    f32 fVar4, r;
    f32 fVar5;
    f32 A[3];
    f32 B[3];
    s32 iVar1;
    f32 tt;



    if (*(s32 *)(p + 0x744) > 0 && 0.25f < FLD(0x6f4)) {
        fVar4 = 0.5f;
        A[0] = (FLD(0x6fc) + FLD(0x71c)) * fVar4 - FLD(0x6c);
        A[1] = (FLD(0x700) + FLD(0x720)) * fVar4 - FLD(0x70);
        A[2] = (FLD(0x704) + FLD(0x724)) * fVar4 - FLD(0x74);
        fVar2 = math_atan2f(__builtin_sqrtf(A[0] * A[0] + A[2] * A[2]) + 1.0f, -A[1] - 1.0f);
        fVar2 = (fVar2 * 32768.0f) / 3.1415927f;
        if (FLD(0x58) < 0.0f) {
            B[0] = FLD(0x708) + FLD(0x728);
            B[1] = FLD(0x70c) + FLD(0x72c);
            B[2] = FLD(0x710) + FLD(0x730);
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
    } else if (*(s32 *)(p + 0x744) >= -31) {
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

struct Obj_14 {
    u8 pad[0x6C];
    f32 x, y, z;
};

extern Ctl_6 gGame_29;

void cam_update_rider_select(Obj_14 *p, s32 *out, s32 arg2)
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
    out[0] = p->x * 1000.0f + (f32)((gCosTable[((e[0] >> 4) - 0x400) & 0xFFF] * amp) >> 14);
    out[1] = p->y * 1000.0f + (f32)yoff;
    r = 0;
    out[2] = p->z * 1000.0f + (f32)((gCosTable[(e[0] >> 4) & 0xFFF] * amp) >> 14);
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

#define FL(o) (*(f32 *)(s + (o)))
#define HW(o) (*(s16 *)(s + (o)))
#define WD(o) (*(s32 *)(s + (o)))
#define BY(o) (*(u8 *)(s + (o)))
#define SB(o) (*(s8 *)(s + (o)))

int cam_calc_bike_yaw(u8 *s)
{
    struct { u16 a[2]; s32 b; u16 c[4]; } L;
    s32 iv1, iv2, a;

    if (BY(0x44) != 0) goto done;
    math_to_polar((s32)(-FL(0xB8) * 1000.0f), (s32)(FL(0xC0) * 1000.0f), L.a, &L.b);
    math_to_polar((s32)(-FL(0x54) * 1000.0f), (s32)(FL(0x5C) * 1000.0f), L.c, &L.b);
    L.b = (s16)(L.a[0] - L.c[0]);
    if (((WD(0x5B0) & 0x10000) == 0) || (WD(0x744) >= 0)) goto normal;
    a = SB(0x5D0);
    if (a < 0) a = -a;
    if (a < 6) goto normal;
    if (HW(0x36) < 0x10) HW(0x36) = HW(0x36) + 1;
    goto after;
normal:
    if (BY(0x41B) == 1) {
        if (HW(0x36) < 0x18) HW(0x36) = HW(0x36) + 1;
    } else {
        a = L.b;
        if (a < 0) a = -a;
        if (a < 0x800 && HW(0x36) != 0) HW(0x36) = HW(0x36) - 1;
    }
after:
    iv2 = 0;
    if (WD(0x744) < 0) iv2 = HW(0x36);
    if (FL(0x424) < 2.0f) iv2 = 0x20;
    iv1 = L.b * iv2;
    iv2 = (s16)L.c[0];
    if (iv1 < 0) iv1 = iv1 + 0x1f;
    L.b = iv2 + (iv1 >> 5);
    HW(0x34) = ((u16 *)&L.b)[1];
done:
    return HW(0x34);
}

#define COST(i) gCosTable[(((i) >> 4) - 0x400) & 0xFFF]
#define SINT(i) gCosTable[((i) >> 4) & 0xFFF]

struct Bike_16 {
    u8 pad0[0x48];
    f32 f48;
    f32 f4C;
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
};

typedef struct {
    u8 pad0[0x58];
    s32 f58;
    u8 pad5C[0x6C - 0x5C];
    s16 f6C;
} State_3;

extern State_3 gEvent_13;
extern Bike_16 gRiders_26[];

s32 cam_mode_soccer(Bike_16 *bike, Rec6 *out) {
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
    if (i == ((bike - gRiders_26) >> ((gNumPlayers - 1) >> 1))) {
        switch (n) {
        case 1:
            s = 1000.0f;
            q = gEvent_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang = gEvent_13.f6C * 15000 / 180 - 8000;
            out->x = gSoccerBall.x * s + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent_13.f6C * s * 3.0f / 180.0f;
            fz = gSoccerBall.z * s - (f32)(((0x4844 - gEvent_13.f6C * 10500 / 180) * SINT(gEvent_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.x * s - (f32)out->x, gSoccerBall.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        case 2:
            s = 1000.0f;
            q3 = gEvent_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = gCosTable;
            ang3 = gEvent_13.f6C * 15000 / 180 - 8000;
            out->x = gSoccerBall.x * s - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)gEvent_13.f6C * s * 3.0f / 180.0f;
            fz = gSoccerBall.z * s + (f32)(((0x4844 - gEvent_13.f6C * 10500 / 180) * SINT(gEvent_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            math_to_polar(gSoccerBall.x * s - (f32)out->x, gSoccerBall.z * s - (f32)z, &a1, &ratio);
            math_to_polar(ratio, gSoccerBall.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        default:
            fatal_halt(1);
        }
    } else switch (n) {
    case 1: {
        Bike_16 *p = &gRiders_26[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(gEvent_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f - (f32)((SINT(gEvent_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->rx = 1000;
        out->rz = gEvent_13.f6C * 14000 / 180 - 7000;
        out->ry = 0;
        break; }
    case 2: {
        Bike_16 *p = &gRiders_26[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(7000 - gEvent_13.f6C * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f + (f32)((SINT(7000 - gEvent_13.f6C * 14000 / 180) * 625) >> 11);
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

f32 cam_calc_hill_climb_pitch(u8 *param_1)
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

    fl = gPathNodes[*(s16 *)(param_1 + 0x628) * 0x6C + 0x64];
    fs = (f32)*gp;
    if ((fl & 1) == 0) {
        s2 = *(f32 *)(param_1 + 0xB8) * *(f32 *)(param_1 + 0xB8) +
             *(f32 *)(param_1 + 0xC0) * *(f32 *)(param_1 + 0xC0);
        k8 = 8.0f;
        len = sqrtf(s2);
        v[0] = *(f32 *)(param_1 + 0x494) + (*(f32 *)(param_1 + 0xB8) * k8) / len;
        v[2] = *(f32 *)(param_1 + 0x49C) + (*(f32 *)(param_1 + 0xC0) * k8) / len;
        v[1] = *(f32 *)(param_1 + 0x498);
        d = bike_get_ground_height(v);
        v[1] = d;
        if (gGroundFound != 0) {
            t = *(f32 *)(param_1 + 0x498) - d;
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

struct Obj_15 {
    u8 pad0[0x34];
    u16 s34;
    u8 pad1[0x0E];
    u8 f44;
    u8 pad2[0x03];
    V3 pos;
    u8 pad3[0x5D0];
    u16 c624;
};

void cam_mode_hill_climb(Obj_15 *p, s32 *out)
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

    if (p->f44 == 0) {
        rot = p->s34;
        h = cam_calc_hill_climb_pitch((u8 *)p);
        mtx3i_from_angles(m, h, 0, rot);
        v[0] = 0;
        v[1] = 0;
        v[2] = -sCamHillDist;
        vec3i_mul_mtx3i(v, m, r);
        pv = &p->pos;
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
        a = out[0] - (s32)(p->pos.x * 1000.0f);
        b = (s32)(p->pos.z * 1000.0f) - z2;
        d = (s32)(p->pos.y * 1000.0f) + sCamHillHeight - out[1];
        math_to_polar(a, b, &ang, &dist);
        if (dist > 10000) {
            p->c624 += 3;
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

struct Bike_17 {
    u8 pad0[0x34];
    u16 h34;
    u8 pad36[0x12];
    f32 pos[3];
    f32 v54[3];
    u8 pad60[0x48C - 0x60];
    f32 f48C;
    u8 pad490[0x4E4 - 0x490];
    Trk *p4E4;
    u8 pad4E8[0x5B0 - 0x4E8];
    u32 f5B0;
    u32 f5B4;
    u8 pad5B8[0x5CD - 0x5B8];
    u8 b5CD;
    u8 pad5CE[0x6F4 - 0x5CE];
    f32 f6F4;
    u8 pad6F8[0x744 - 0x6F8];
    s32 f744;
    u8 pad748[0x77C - 0x748];
    s16 h77C;
};

extern Game_14 gGame_30;
extern Rep gEvent_14;

void cam_update_chase(Bike_17 *p, s32 *out, s32 z)
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

    k = p->f744;
    cnt = sCamAirBlend_2;
    n = cnt[gCamView];
    if (p->f6F4 * 60.0f + (f32)k > 60.0f) {
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
    if ((zero = gGame_30.mode, rot = p->h34, zero != 9) && !(gGame_30.flags & 0x8000)) {
        if ((p->f5B0 & 0x200) && gEvent_14.active != 0) {
            rot += -0x8000;
        }
    }
    ang = sCamPitchSprings[gCamView].pos;
    if (p->h77C != 0) {
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
    pp = p->pos;
    out[0] = (f32)res[0] + pp[0] * 1000.0f;
    out[1] = (f32)res[1] + pp[1] * 1000.0f + 600.0f;
    out[2] = (f32)res[2] + pp[2] * 1000.0f;
    out[3] = ang - 500;
    out[5] = (s16)rot;
    out[4] = 0;
    if (gGame_30.mode != 9) {
        if ((p->f5B0 & 0x200) || ((gPlayerInputs_2 & 0x10) && gEvent_14.active == 0 && !(gGame_30.flags & 0x8000))) {
            cam_snap_to(out);
        } else if (p->f5B4 & 0x200) {
            cam_snap_to_moving(out, p->v54);
        } else {
            goto other;
        }
    } else {
other:
        cam_smooth_to(out);
    }
    sCamShakeY[gCamView] = 0;
    if (p->p4E4 != NULL && p->f744 < 6 && (p->p4E4->flags & 0x10)) {
        sCamShakeY[gCamView] = -(s32)(p->f48C * 1000.0f) * 2;
    }
    if (p->b5CD != 0) {
        f = p->b5CD;
        cs = math_exp(-f * 0.0625f);
        q2 = sCamShakeY;
        f = f * 3.1415927f;
        q = &q2[gCamView];
        *q += (s32)(sinf(f / 6.0f) * 1024.0f * cs);
    }
}

struct Ent_23 {
    u8 pad0[4];
    s32 y;
    u8 pad1[0x44C - 8];
    f32 f44C;
    u8 pad2[0x624 - 0x450];
    s16 h624;
    u8 pad3[0x7E0 - 0x626];
};

extern Ent_23 gRiders_27[];

void cam_mode_fall_view(Ent_23 *e, Rec_4 *r)
{
    Rec_4 *b;
    s32 i;
    s32 d;

    b = sCamHistorySum;
    i = e - gRiders_27;
    r->x = b[i].x / 16;
    r->y = b[i].y / 16;
    r->z = b[i].z / 16;
    d = e->y - r->y;
    r->a = b[i].a;
    r->c = b[i].c;
    r->b = 0;
    cam_snap_to(r);
    if (gGameMode == 7) {
        e->f44C = 0.5f;
        if (d < -12000) {
            e->h624 = 10000;
            e->f44C = 1.0f;
        }
    }
}

typedef struct { f32 v[16]; } Row_3;

extern Row_3 gSurfaceTable_5[];

void cam_start_fall_view(Ent_24 *p)
{
    Rec_4 loc;
    s32 m[9];
    s32 v[3];
    s32 o[3];
    s16 ang;
    s32 dist;
    f32 f;
    s32 idx;

    if (p->f5AA != 9) {
        gCamLastModes[p - gRiders_28] = 9;
        p->f5AA = 9;
        if (gCurTrack == D_800CB5D0) {
            loc = sCamFallViewPos;
            math_to_polar(loc.x - p->f0, p->f8 - loc.z, &ang, &dist);
            loc.c = ang;
        } else {
            path_update_bike(p);
            math_to_polar(-(gPathTangentX * 1000.0f), gPathTangentZ * 1000.0f, &ang, &dist);
            if (0.0f < p->f638) {
                ang += 0x4000;
            } else {
                ang -= 0x4000;
            }
            p->f34 = ang;
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
            loc.x = (s32)(p->x * 1000.0f) + o[0];
            loc.y = (s32)(p->y * 1000.0f) + o[1] - 1000;
            loc.z = (s32)(p->z * 1000.0f) + o[2];
            loc.a = 0x1000;
            loc.b = 0;
            loc.c = ang;
        }
        gCamView = idx = p - gRiders_28;
        cam_snap_to(&loc);
        func_8004F064(idx);
    }
}

struct Bike_18 {
    u8 pad00[0x2C];
    s32 f2C;
    u8 pad30[0x6C - 0x30];
    f32 x, y, z;
    u8 pad78[0x3FE - 0x78];
    s8 f3FE;
    u8 pad3FF[0x744 - 0x3FF];
    s32 f744;
    u8 pad748[0x7E0 - 0x748];
};

void cam_mode_stunt_pip(Bike_18 *p, Rec6 *out)
{
    s32 a[3];
    s32 b[3];
    s32 m[10];

    if (p->f3FE != 0) {
        if (p->f744 <= 0) {
            p->f3FE--;
        }
        mtx3i_from_angles(m, 0x800, 0, p->f2C + gCamPipOrbitAngle);
        gCamPipOrbitAngle -= 0x40;
        if (gCamPipOrbitAngle < 9000) {
            gCamPipOrbitAngle = 9000;
        }
        a[0] = 0;
        a[1] = 0;
        a[2] = -4000;
        vec3i_mul_mtx3i(a, m, b);
        out->x = (f32)b[0] + p->x * 1000.0f;
        out->y = (f32)b[1] + p->y * 1000.0f;
        out->z = (f32)b[2] + p->z * 1000.0f;
        out->rx = 0x800;
        out->rz = p->f2C + gCamPipOrbitAngle;
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

typedef struct Big {
    u8 pad0[0x628];
    s16 f628;
    u8 pad1[0x7E0 - 0x62A];
} Big;

typedef struct Node_3 {
    u8 pad0[0x4E];
    s16 next;
    u8 pad1[0x6C - 0x50];
} Node_3;

extern Flag gCamZoneState_4[];
extern Node_3 *gPathNodes_8;
extern Obj_16 *gCurTrack_20;
extern Obj_16 *gCurTrack_43;
extern Big gRiders_29[];

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
    cur = gRiders_29[sCamTarget].f628;
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

struct Obj_17 {
    u8 pad0[0x48];
    Vec3f pos;
    u8 pad54[0x628 - 0x54];
    s16 seg;
    u8 pad62A[0x63C - 0x62A];
    f32 frac;
};

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

f32 cam_zone_get_progress(Obj_17 *p, s32 a, s32 b)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (gCamZoneState_5[gCamView].flags & 8) {
        len = sCamSegOrder[b] - sCamSegOrder[a];
        seg = path_find_segment(&p->pos, gPathNodes_9[p->seg].start, 15, 1);
        path_get_lateral_offset(&p->pos, seg);
        frac = gPathSegmentT;
    } else {
        seg = p->seg;
        seg = (s16)seg;
        frac = p->frac;
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

struct Obj_18 {
    u8 pad0[0x2E];
    u16 h2e;
    u8 pad1[0x3C];
    f32 f6c;
    f32 f70;
    f32 f74;
};

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

void cam_zone_update_follow(Obj_18 *param_1, s32 *param_2)
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
                  (param_1->h2e + sum4) & 0xFFFF);
    local_78[0] = 0;
    local_78[1] = 0;
    local_78[2] = vS;
    vec3i_mul_mtx3i(local_78, aiStack_58, local_68);
    param_2[0] = (s32)(param_1->f6c * 1000.0f + (f32)local_68[0]);
    param_2[1] = (s32)(param_1->f70 * 1000.0f + (f32)local_68[1] + 1000.0f);
    iVar5 = (s32)(param_1->f74 * 1000.0f + (f32)local_68[2]);
    param_2[2] = iVar5;
    math_to_polar((s32)(param_1->f6c * 1000.0f - (f32)param_2[0]),
                  (s32)(param_1->f74 * 1000.0f - (f32)iVar5), t30.h, &t30.w);
    math_to_polar(t30.w, (s32)((param_1->f70 + 0.7f) * 1000.0f - (f32)param_2[1]), local_28, &t30.w);
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

struct Actor {
    u8 pad[0x6C];
    f32 fx, fy, fz;
};

extern Slot_6 gCamZoneState_7[];

void cam_zone_update_dolly(Actor *act, Rec6 *out)
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
                    o->x = (f32)gCamPos[gCamView].x - act->fx * 1000.0f;
                    o->y = (f32)gCamPos[gCamView].y - act->fy * 1000.0f;
                    o->z = (f32)gCamPos[gCamView].z - act->fz * 1000.0f;
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
        out->x = (f32)o->x + act->fx * 1000.0f;
        out->y = (f32)o->y + act->fy * 1000.0f;
        out->z = (f32)o->z + act->fz * 1000.0f;
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
        math_to_polar(act->fx * 1000.0f - (f32)out->x, act->fz * 1000.0f - (f32)out->z, &b1, &r2);
        math_to_polar(r2, (act->fy + 0.7f) * 1000.0f - (f32)out->y, &b2, &r2);
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

typedef struct {
    u8 pre[0x48];
    Vec3f pos;
    u8 post[0x7E0 - 0x48 - 12];
} Node_4;

void cam_collide_walls(s32 a, s32 b, s32 c)
{
    Node_4 *nd, *nd0;
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
    p = nd->pos;
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
    u8 pad[0x6C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad2[0x7E0 - 0x78];
} Bike_19;

typedef struct {
    s16 v;
} Ang;

typedef struct {
    s32 x, y, z;
    s32 pad0C, pad10;
    s32 w14;
} Seg_8;

extern Bike_19 gRiders_30[];
extern Seg_8 sCamHistory_4[][16];
extern Seg_8 sCamHistorySum_4[];

void cam_shift_xz(s32 p1, s32 p2, s32 p3, s32 p4)
{
    Ang a;
    s32 b;
    Ang c;
    s32 dx, dz, d;
    s32 i;

    dx = gRiders_30[p2].x * 1000.0f;
    dx -= gCamPos[p1].x;
    dz = gRiders_30[p2].z * 1000.0f;
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

typedef struct Blk_8 {
    u8 p0[0x6c];
    f32 f6c;
    f32 f70;
    f32 f74;
    u8 p78[0x628 - 0x78];
    s16 s628;
    u8 p62a[0x744 - 0x62a];
    s32 w744;
    u8 p748[0x7e0 - 0x748];
} Blk_8;

typedef struct Ent_25 {
    s32 f00, f04, f08, f0c, f10, f14;
} Ent_25;

typedef struct { u8 p0[0x2C]; s32 f2C; u8 p30[0xA0 - 0x30]; } Trk_9;

extern Trk_9 *gCurTrack_21;
extern Blk_8 gRiders_31[];
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
    Blk_8 *pb;
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
        path_get_segment_and_t(&v, &idx, &t, gRiders_31[m].s628);
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
                if (h < gCamPos[n].y && gRiders_31[m].w744 < 0x1e) {
                    f32 ex, ez;
                    ex = gRiders_31[m].f6c * 1000.0f - (f32)gCamPos[n].x;
                    ez = gRiders_31[m].f74 * 1000.0f - (f32)gCamPos[n].z;
                    ex = __builtin_sqrtf(ex * ex + ez * ez);
                    g = gRiders_31[m].f70;
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

typedef struct {
    u8 pad0[0x6C];
    f32 fA;
    u8 pad1[4];
    f32 fB;
    u8 pad2[0x7E0 - 0x78];
} Rec37_2;

extern Rec37_2 gRiders_32[];

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
        fVar3 = gRiders_32[param_1].fA * 1000.0f - (f32)t[0];
        dz = gRiders_32[param_1].fB * 1000.0f - (f32)t[2];
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

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    u8 pad[0x7E0 - 12];
} R7E0;

extern E18 sCamHistorySum_6[];
extern E18 sCamHistory_6[][16];
extern R7E0 gRiders_33[];

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
    height_get_ground(gRiders_33[p].x, gRiders_33[p].y, gRiders_33[p].z);
    if (gGroundFound != 0) {
        gRiderSection[p] = gGroundArea;
    }
}

void cam_mode_chase_7m(int param_1, int *param_2)
{
    cam_update_chase(param_1, param_2, -7000);
}

void cam_mode_chase_far_alt(s32 a, s32 *b)
{
    cam_update_chase(a, b, -10000);
}

struct Obj_20 {
    u8 pad[0x70];
    f32 f70;
    f32 f74;
};

void cam_mode_side_view(Obj_20 *param_1, s32 *param_2)
{
    f32 k = 1000.0f;

    param_2[0] = (s32)-sCamSideViewParams[0];
    param_2[1] = (s32)(param_1->f70 * k + sCamSideViewParams[1]);
    param_2[2] = (s32)(param_1->f74 * k - sCamSideViewParams[2]);
    param_2[3] = (s32)sCamSideViewParams[4];
    param_2[5] = (s32)-sCamSideViewParams[3];
    param_2[4] = 0;
    cam_snap_to(param_2);
}

struct Obj_21 {
    u8 pad0[0x6C];
    f32 f6c;
    u8 pad1[4];
    f32 f74;
};

struct Out_2 {
    s32 w0, w4, w8, wC, w10, w14;
};

void func_8002D528(Obj_21 *o, Out_2 *out) {
    out->w0 = o->f6c * 1000.0f + -28000.0f;
    out->w4 = 0x5BCC;
    out->w8 = o->f74 * 1000.0f - 10000.0f;
    out->wC = 0x1800;
    out->w14 = -0x4000;
    out->w10 = 0;
    cam_smooth_to(out);
}

void cam_mode_chase_near(u8 *p, s32 *p2) {
    s32 v = -0x9C4;

    if (p[0x44] != 0) {
        v = -5000;
    }
    if (*(f32 *)(p + 0x424) < 0.0f) {
        v = (s32)((f32)v + *(f32 *)(p + 0x424) * 250.0f);
    }
    cam_update_chase(p, p2, v);
}

void cam_mode_chase_mid(s32 param_1, s32 *param_2)
{
    cam_update_chase(param_1, param_2, -5000);
}

void cam_mode_chase_far(s32 a, s32 *b) {
    cam_update_chase(a, b, -10000);
}

struct Obj_22 { u8 p0[0x22]; u16 f22; u8 p1[0x6C - 0x24]; f32 f6c; f32 f70; f32 f74; u8 p2[0x3FF - 0x78]; u8 f3ff; u16 f400; };
typedef struct { u8 p[0x2C]; s32 f2c; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *gCurTrack_22;

void cam_mode_celebration(Obj_22 *o, s32 *out)
{
    Stk st;
    s16 it;
    s16 sv;
    s32 a;

    it = 2000;
    if (gCurTrack_22->f2c == 1) {
        it = 6000;
    }
    a = o->f22 + 0x7000;
    sv = a;
    if (o->f3ff > 1) {
        sv = a - o->f400 * 0x40;
        o->f400 = o->f400 + 1;
    }
    st.l60[0] = 0;
    st.l60[1] = 0;
    st.l60[2] = -5000;
    mtx3i_from_angles(st.arr, it, 0, sv);
    vec3i_mul_mtx3i(st.l60, st.arr, &st.l50);
    out[0] = (s32)((f32)st.l50 + o->f6c * 1000.0f);
    out[1] = (s32)((f32)st.l4c + o->f70 * 1000.0f + 750.0f);
    out[2] = (s32)((f32)st.l48 + o->f74 * 1000.0f);
    out[3] = it;
    out[5] = sv;
    out[4] = 0;
    cam_snap_to(out);
}

void cam_mode_first_person(s32 *p1, s32 *p2)
{
    s32 t[4];
    s32 o[3];
    s32 a[10];
    f32 *pf;
    s32 v;

    mtx3i_from_angles(a, p1[6], p1[7], p1[8]);
    t[0] = 0;
    t[1] = 400;
    t[2] = 0;
    vec3i_mul_mtx3i(t, a, o);
    pf = (f32 *)p1;
    p2[0] = (s32)((f32)o[0] + pf[27] * 1000.0f);
    p2[1] = (s32)((f32)o[1] + pf[28] * 1000.0f);
    p2[2] = (s32)((f32)o[2] + pf[29] * 1000.0f);
    p2[3] = p1[6] / 2;
    p2[5] = p1[8];
    v = p1[7];
    if (v < 0) {
        v += 3;
    }
    p2[4] = v >> 2;
    cam_snap_to(p2);
}

void func_8002D888(Player_10 *arg0, s32 *arg1)
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

typedef struct {
    void *hdr;
    s32 x0, z0, x1, z1;
    void *grid;
    s32 f18;
    s32 f1C;
} Sub_7;

struct Area_2 {
    u32 count;
    Sub_7 subs[1];
};

typedef struct { u16 n; u16 ids[1]; } List_5;

extern List_5 **sHeightGrid_2;

void height_init(Area_2 *area, s32 b, s32 c, u32 *polys)
{
    u32 i;
    s32 n;
    s32 cell;
    s32 x, z;
    s32 j, k, total;
    u16 *out;
    Sub_7 *s;
    Area_2 *r;
    Poly *p;
    Poly **tbl;
    s16 *buf;

    sHeightAreas_2 = area;
    sWallMesh = c;
    gGroundArea = -1;
    if (area != NULL) {
        for (i = 0; i < sHeightAreas_2->count; i++) {
            s = (Sub_7 *)((u8 *)sHeightAreas_2 + (i * 32 + 4));
            if (s->x0 < sHeightAreaMin[0] || i == 0) sHeightAreaMin[0] = s->x0;
            if (s->z0 < sHeightAreaMin[1] || i == 0) sHeightAreaMin[1] = s->z0;
            if (s->x1 > sHeightAreaMax[0] || i == 0) sHeightAreaMax[0] = s->x1;
            if (s->z1 > sHeightAreaMax[1] || i == 0) sHeightAreaMax[1] = s->z1;
        }
        mem_fill((u8 *)gZBuffer_2, 0, 0x200);
        i = 0;
        total = 0;
        out = gZBuffer_2 + 0x100;
        for (k = 0; k < 16; k++) {
            for (j = 0; j < 16; j++) {
                for (n = 0; n < sHeightAreas_2->count; n++) {
                    r = (Area_2 *)((u8 *)sHeightAreas_2 + n * 32);
                    x = r->subs[0].x0 << 4;
                    x >>= 4;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    z = r->subs[0].z0 << 4;
                    z >>= 4;
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    cell = z * 16 + x;
                    z = cell >> 4;
                    if ((cell & 0xF) > j) continue;
                    if (z > k) continue;
                    z = r->subs[0].x1 << 4;
                    z >>= 4;
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    x = r->subs[0].z1 << 4;
                    x >>= 4;
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    cell = x * 16 + z;
                    z = cell >> 4;
                    if ((cell & 0xF) < j) continue;
                    if (z < k) continue;
                    gZBuffer_2[i]++;
                    total++;
                    *out++ = n;
                }
                i++;
            }
        }
        sHeightGridData = mem_alloc_dbg((total + 0x100) * 2, 0, (s32)D_800D6C20, 0xDA);
        sHeightGrid_2 = mem_alloc_dbg(0x400, 0, (s32)D_800D6C20, 0xDB);
        n = 0;
        out = gZBuffer_2 + 0x100;
        for (i = 0; (s32)i < 0x100; i++) {
            sHeightGrid_2[i] = (List_5 *)(sHeightGridData + n);
            sHeightGrid_2[i]->n = gZBuffer_2[i];
            memcpy(sHeightGrid_2[i]->ids, out, sHeightGrid_2[i]->n * 2);
            n++;
            n += sHeightGrid_2[i]->n;
            out += sHeightGrid_2[i]->n;
        }
    } else {
        sHeightGridData = NULL;
        sHeightGrid_2 = NULL;
    }
    sSurfacePolys = NULL;
    sAreaSurfacePolys = NULL;
    if (polys != NULL) {
        sSurfacePolys = mem_alloc_dbg(*polys * 4, 0, (s32)D_800D6C20, 0xF6);
        i = 0;
        p = (Poly *)(polys + 1);
        z = *polys;
        if (z != 0) {
            do {
                sSurfacePolys[i] = p;
                p = (Poly *)((u8 *)p + (p->n * 4 + 8));
            } while (++i < *polys);
        }
        mem_fill((u8 *)gZBuffer_2, 0, sHeightAreas_2->count * 2);
        i = 0;
        out = gZBuffer_2 + sHeightAreas_2->count;
        for (; i < sHeightAreas_2->count; i++) {
            for (cell = 0; cell < *polys; cell++) {
                p = sSurfacePolys[cell];
                for (n = 0; n < p->n; n++) {
                    if (height_test_area(i, p->pts[n].x << 6, 0, p->pts[n].z << 6)) break;
                }
                if (n != p->n) {
                    *out++ = cell;
                    gZBuffer_2[i]++;
                }
            }
        }
        i = 0;
        sAreaSurfacePolys = mem_alloc_dbg(sHeightAreas_2->count * 4, 0, (s32)D_800D6C20, 0x11C);
        mem_fill((u8 *)sAreaSurfacePolys, 0, sHeightAreas_2->count * 4);
        out = gZBuffer_2 + sHeightAreas_2->count;
        for (; i < sHeightAreas_2->count; i++) {
            if (gZBuffer_2[i] != 0) {
                buf = mem_alloc_dbg((gZBuffer_2[i] + 1) * 2, 0, (s32)D_800D6C20, 0x123);
                sAreaSurfacePolys[i] = buf;
                memcpy(buf, out, gZBuffer_2[i] * 2);
                out += gZBuffer_2[i];
                sAreaSurfacePolys[i][gZBuffer_2[i]] = -1;
            }
        }
    }
}

void height_free(void)
{
    u32 i;
    if (gWallEdgeFlags != 0) {
        mem_free(gWallEdgeFlags);
        gWallEdgeFlags = 0;
    }
    if (sHeightGrid != 0) {
        mem_free(sHeightGrid);
        sHeightGrid = 0;
    }
    if (sHeightGridData != 0) {
        mem_free(sHeightGridData);
        sHeightGridData = 0;
    }
    if (sSurfacePolys != 0) {
        mem_free(sSurfacePolys);
        sSurfacePolys = 0;
    }
    if (sAreaSurfacePolys != 0) {
        i = 0;
        if (*sHeightAreas_3 != 0) {
            do {
                if (sAreaSurfacePolys[i] != 0) {
                    mem_free(sAreaSurfacePolys[i]);
                }
                i++;
            } while (i < (u32)*sHeightAreas_3);
        }
        mem_free(sAreaSurfacePolys);
        sAreaSurfacePolys = 0;
    }
}

typedef struct {
    s32 f0;
    Vtx16 *verts;
} Hdr_3;

typedef struct {
    u16 bits[4];
    u16 v[3];
    u16 flags;
} GTri;

typedef struct {
    u16 flags;
    u16 count;
    GTri tris[1];
} Grid;

typedef struct {
    Hdr_3 *hdr;
    s32 x0, z0, x1, z1;
    Grid *grid;
    s32 f18;
} Sub_8;

s32 height_test_area(s32 id, s32 x, s32 y, s32 z)
{
    Sub_8 *a;
    Grid *g2;
    s32 px, pz;
    s32 mask, word, count;
    GTri *tri;
    Vtx16 *verts;
    s32 x0, z0, x1, z1;
    s32 w, d;
    s32 i, start;
    s32 v[3][4];
    Grid *grid;
    s32 dxp, dzp;
    s32 nx, ny, nz, h;
    s32 k;
    s32 c;

    D_800E8E7C++;
    a = (Sub_8 *)((u8 *)sHeightAreas + (id * 32 + 4));
    x0 = a->x0;
    if (x < x0 * 16) return 0;
    z0 = a->z0;
    if (z < z0 * 16) return 0;
    x1 = a->x1;
    if (x1 * 16 < x) return 0;
    z1 = a->z1;
    if (z1 * 16 < z) return 0;
    grid = a->grid;
    if (grid == NULL) return 0;
    if (!(grid->flags & 1)) fatal_halt(1);
    px = x;
    g2 = grid;
    pz = z;
    if (g2->flags & 4) {
        px = x - (x1 + x0) * 8;
        pz = z - (z1 + z0) * 8;
    }
    w = (x1 - x0) * 16;
    if (w == 0) return 0;
    d = (z1 - z0) * 16;
    if (d == 0) return 0;
    i = ((z - z0 * 16) * 8 / d) * 8 + (x - x0 * 16) * 8 / w;
    verts = a->hdr->verts;
    word = i / 16;
    mask = 1 << (i % 16);
    start = gGroundTri;
    count = g2->count;
    if (start < 0) start = 0;
    if (start >= count) start = 0;
    i = start;
    do {
        tri = &g2->tris[i];
        if (tri->bits[word] & mask) {
            v[0][0] = verts[tri->v[0]].x * 16;
            v[0][1] = verts[tri->v[1]].x * 16;
            v[0][2] = verts[tri->v[2]].x * 16;
            v[2][0] = verts[tri->v[0]].z * 16;
            dzp = pz - v[2][0];
            v[2][1] = verts[tri->v[1]].z * 16;
            dxp = px - v[0][0];
            v[2][2] = verts[tri->v[2]].z * 16;
            if ((c = dzp * (v[0][1] - v[0][0]) - dxp * (v[2][1] - v[2][0])) >= 0 &&
                (c = (pz - v[2][1]) * (v[0][2] - v[0][1]) - (px - v[0][1]) * (v[2][2] - v[2][1])) >= 0 &&
                (c = (pz - v[2][2]) * (v[0][0] - v[0][2]) - (px - v[0][2]) * (v[2][0] - v[2][2])) >= 0) {
                v[1][0] = verts[tri->v[0]].y * 16;
                v[1][1] = verts[tri->v[1]].y * 16;
                v[1][2] = verts[tri->v[2]].y * 16;
                v[0][1] -= v[0][0];
                v[0][2] -= v[0][0];
                v[1][1] -= v[1][0];
                v[1][2] -= v[1][0];
                v[2][1] -= v[2][0];
                v[2][2] -= v[2][0];
                v[0][1] >>= 4;
                v[1][1] >>= 4;
                v[2][1] >>= 4;
                v[0][2] >>= 4;
                v[2][2] >>= 4;
                v[1][2] >>= 4;
                nx = v[2][1] * v[1][2] - v[1][1] * v[2][2];
                ny = v[0][1] * v[2][2] - v[2][1] * v[0][2];
                nz = v[1][1] * v[0][2] - v[0][1] * v[1][2];
                if (ny != 0) {
                    c = -nz;
                    h = (dxp * -nx + dzp * c) / ny + v[1][0];
                    if (tri->flags & 0x400) {
                        if (h <= y + 500 && h >= sGroundBestHeight) {
                            sGroundBestHeight = h;
                            gGroundFound = 1;
                            gGroundArea = id;
                            gGroundTri = i;
                            gGroundHeight = h;
                            gGroundSurface = tri->flags;
                        }
                    } else if (sGroundBestHeight == -99999) {
                        gGroundFound = 1;
                        gGroundArea = id;
                        gGroundTri = i;
                        gGroundHeight = h;
                        gGroundSurface = tri->flags;
                        if (sAreaSurfacePolys != NULL) {
                            if (sAreaSurfacePolys[id] == NULL) return 1;
                            k = 0;
                            while (sAreaSurfacePolys[id][k] != -1) {
                                if (point_in_poly_xz(sSurfacePolys_2[sAreaSurfacePolys[id][k]], x, z)) {
                                    gGroundSurface = *sSurfacePolys_2[sAreaSurfacePolys[id][k]];
                                    break;
                                }
                                k++;
                            }
                        }
                        return 1;
                    }
                }
            }
        }
        i++;
        if (i >= count) i = 0;
    } while (i != start);
    return 0;
}

s32 height_get_ground(s32 x, s32 y, s32 z)
{
    s32 ix, iz, i, k;

    sGroundBestHeight = -99999;
    gGroundFound = 0;
    gGroundSurface = 0;
    D_800E8938++;
    if (gCurTrack_5->mode == 2) {
        return height_get_desert(x, z);
    }
    if (gGroundArea != -1 && height_test_area(gGroundArea, x, y, z) != 0) {
        return gGroundHeight;
    }
    ix = x >> 4;
    ix = (ix - sHeightAreaMin_2.a) * 16 / (sHeightAreaMax_2.a - sHeightAreaMin_2.a);
    iz = z >> 4;
    if (ix < 0) ix = 0;
    if (ix > 15) ix = 15;
    iz = (iz - sHeightAreaMin_2.a) * 16 / (sHeightAreaMax_2.b - sHeightAreaMin_2.b);
    if (iz < 0) iz = 0;
    if (iz > 15) iz = 15;
    k = iz * 16 + ix;
    for (i = 0; i < sHeightGrid[k]->n; i++) {
        if (sHeightGrid[k]->id[i] != gGroundArea && height_test_area(sHeightGrid[k]->id[i], x, y, z) != 0) {
            return gGroundHeight;
        }
    }
    if (sGroundBestHeight != -99999) {
        return sGroundBestHeight;
    }
    return gGroundHeight;
}

s32 height_get_desert(s32 x, s32 z)
{
    struct {
        s32 q;
        s32 rx;
        s32 rz;
    } L;
    s32 *qp;
    s32 ix, lt, v, fx, fz, h0, h1, h2;

    qp = &L.q;
    gGroundFound = 1;
    divmod_floor(x, 2000, qp, &L.rx);
    ix = L.q;
    divmod_floor(z, 2000, qp, &L.rz);
    z = L.q;
    gGroundArea = ((ix >> 2) & 0xffff) + ((z >> 2) << 16);
    v = ((ix & 3) + (z & 3) * 4) * 2;
    gGroundTri = v;
    if (L.rx < 1999 - L.rz) {
        gGroundTri = v + 1;
    }
    ix &= 0x1ff;
    z &= 0x1ff;
    fx = L.rx;
    fz = L.rz;
    if (fx < 1999 - fz) {
        h0 = gDesertHeightMap[z * 0x200 + ix];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = gDesertHeightMap[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * fz + (h2 - h0) * fx) / 2000;
    } else {
        h0 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = gDesertHeightMap[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * (2000 - L.rx) + (h2 - h0) * (2000 - L.rz)) / 2000;
    }
    return L.q;
}

float height_get_desert_f(float x, float z)
{
    s32 ix, iz, h0, h1, h2;
    float rz, f0, f2;

    x *= 0.5f;
    z *= 0.5f;
    ix = x;
    gGroundFound = 1;
    if (ix < 0) {
        ix--;
    }
    iz = z;
    x -= ix;
    if (iz < 0) {
        iz--;
    }
    gGroundArea = ((ix >> 2) & 0xffff) + ((iz >> 2) << 16);
    z -= iz;
    rz = 1.0f - z;
    gGroundTri = ((ix & 3) + (iz & 3) * 4) * 2;
    if (x < rz) {
        gGroundTri = gGroundTri + 1;
    }
    ix &= 0x1ff;
    iz &= 0x1ff;
    if (x < rz) {
        float f1;
        h0 = gDesertHeightMap[iz * 0x200 + ix];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        f1 -= f0;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + f1 * z + (f2 - f0) * x) / 1000.0f;
    } else {
        float f1, d;
        h0 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        d = f1 - f0;
        f1 = 1.0f - x;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + d * f1 + (f2 - f0) * rz) / 1000.0f;
    }
}

typedef struct { u8 pad[8]; u16 v[3]; u16 pad2; } Tri3;
typedef struct { u16 flags; u16 pad; Tri3 t[1]; } Shape;
typedef struct { u32 pad; Vtx_t *vtx; } Mesh_3;
typedef struct { Mesh_3 *mesh; s32 x0, z0, x1, z1; Shape *shape; u32 pad[2]; } Ent_26;
typedef struct { u32 n; Ent_26 e[1]; } EntList;

extern EntList *sHeightAreas_4;

void height_get_ground_color(s32 x, s32 z, s32 *out)
{
    s32 m[3][4];
    Ent_26 *e;
    Shape *s;
    Tri3 *t;
    Vtx_t *vb;
    s32 ee, k, q, tt, a, a2, b, d, num, sx, sz, px, pz;

    if (gGroundArea < 0 || gGroundArea >= sHeightAreas_4->n) return;
    e = &sHeightAreas_4->e[gGroundArea];
    s = e->shape;
    if (!(s->flags & 1)) return;
    t = &s->t[gGroundTri];
    sx = (e->x1 + e->x0) * 8;
    sz = (e->z1 + e->z0) * 8;
    if (s->flags & 4) {
        px = x - sx;
        pz = z - sz;
    } else {
        px = x;
        pz = z;
    }
    vb = e->mesh->vtx;
    m[0][0] = vb[t->v[0]].ob[0];
    m[0][1] = vb[t->v[1]].ob[0];
    m[0][2] = vb[t->v[2]].ob[0];
    m[2][0] = vb[t->v[0]].ob[2];
    m[2][1] = vb[t->v[1]].ob[2];
    m[2][2] = vb[t->v[2]].ob[2];
    m[0][1] -= m[0][0];
    m[2][1] -= m[2][0];
    m[0][2] -= m[0][0];
    m[2][2] -= m[2][0];
    m[0][0] <<= 4;
    m[2][0] <<= 4;
    for (k = 0; k < 3; k++) {
        m[1][0] = vb[t->v[0]].cn[k];
        m[1][1] = vb[t->v[1]].cn[k];
        m[1][2] = vb[t->v[2]].cn[k];
        m[1][1] -= m[1][0];
        m[1][2] -= m[1][0];
        tt = m[2][1] * m[1][2];
        a = m[1][1] * m[2][2] - tt;
        b = px - m[0][0];
        q = b * a;
        tt = m[1][1] * m[0][2];
        a2 = m[0][1] * m[1][2] - tt;
        d = m[0][1] * m[2][2];
        q += (pz - m[2][0]) * a2;
        ee = m[2][1] * m[0][2];
        q = q / (d - ee);
        q /= 16; q += m[1][0];
        if (q < 0) q = 0;
        if (q > 255) q = 255;
        out[k] = q;
    }
}

#define IDX(a, b) (((((a) & 0x1FF)) << 9) | ((b) & 0x1FF))

typedef struct { u8 pad[8]; u16 vi[3]; u16 pad2; } MTri;
typedef struct { u16 flags; u16 pad; MTri tri[1]; } Mesh_4;
typedef struct { u8 pad[4]; Vtx16 *verts; } Model_3;
typedef struct { Model_3 *model; s32 a, b, c, d; Mesh_4 *mesh; } Obj_23;

s32 height_get_tri_verts(s32 p1, s32 p2, Vec3i *v)
{
    s32 n, q, u12, u13, lo4;
    Obj_23 *o;
    Mesh_4 *m;
    Vtx16 *vt;
    s32 k;
    u32 f; s32 j;
    u16 *pb, *pv;

    if (*(s32 *)(gCurTrack + 0x2c) == 2) {
        n = p2 / 2;
        lo4 = (p1 << 16) >> 14;
        q = n / 4;
        u12 = lo4 + (n - q * 4);
        u13 = (p1 >> 16) * 4 + q;
        if (p2 & 1) {
            v[0].x = u12 * 2000;
            v[0].z = u13 * 2000;
            v[0].y = gDesertHeightMap[IDX(u13, u12)] << 6;
            v[1].x = (u12 + 1) * 2000;
            v[1].z = u13 * 2000;
            v[1].y = gDesertHeightMap[IDX(u13, u12 + 1)] << 6;
            v[2].x = u12 * 2000;
            v[2].z = (u13 + 1) * 2000;
            v[2].y = gDesertHeightMap[IDX(u13 + 1, u12)] << 6;
        } else {
            v[0].x = (u12 + 1) * 2000;
            v[0].z = (u13 + 1) * 2000;
            v[0].y = gDesertHeightMap[IDX(u13 + 1, u12 + 1)] << 6;
            v[1].x = u12 * 2000;
            v[1].z = (u13 + 1) * 2000;
            v[1].y = gDesertHeightMap[IDX(u13 + 1, u12)] << 6;
            v[2].x = (u12 + 1) * 2000;
            v[2].z = u13 * 2000;
            v[2].y = gDesertHeightMap[IDX(u13, u12 + 1)] << 6;
        }
        return 0;
    }

    o = (Obj_23 *)(sHeightAreas + (p1 * 0x20 + 4));
    m = o->mesh;
    f = m->flags;
    if (!(f & 1)) {
        goto hang;
    }
    pb = (u16 *)m;
    vt = o->model->verts;
    if (f & 4) {
        u12 = (o->c + o->a) * 8;
        n = (o->d + o->b) * 8;
    } else {
        n = 0;
        u12 = 0;
    }
    pv = pb + (p2 * 8 + 2);
    for (p1 = 0; p1 < 3; p1++) {
        v[p1].x = vt[pv[p1 + 4]].x * 16 + u12;
        v[p1].y = vt[pv[p1 + 4]].y * 16;
        v[p1].z = vt[pv[p1 + 4]].z * 16 + n;
    }
    return 0;
hang:
    for (;;) {
    }
}

void height_get_ground_normal(f32 *param_1)
{
    s32 v[9];
    f32 len;
    height_get_tri_verts(gGroundArea, gGroundTri, v);
    v[3] -= v[0];
    v[6] -= v[0];
    v[4] -= v[1];
    v[5] -= v[2];
    v[7] -= v[1];
    v[8] -= v[2];
    param_1[0] = (f32)v[5] * (f32)v[7] - (f32)v[4] * (f32)v[8];
    param_1[1] = (f32)v[3] * (f32)v[8] - (f32)v[5] * (f32)v[6];
    param_1[2] = (f32)v[4] * (f32)v[6] - (f32)v[3] * (f32)v[7];
    len = param_1[0] * param_1[0] + param_1[1] * param_1[1] + param_1[2] * param_1[2];
    len = sqrtf(len);
    param_1[0] = param_1[0] / len;
    param_1[1] = param_1[1] / len;
    param_1[2] = param_1[2] / len;
}

typedef struct { s32 pad; Vtx16 *v; } Mesh_5;
typedef struct { u32 m0, m1; u16 v[3]; u16 flags; } Ent_27;
typedef struct { u16 flags; u16 n; Ent_27 e[1]; } TList;
typedef struct { Mesh_5 *m; s32 x0, z0, x1, z1; TList *tl; u8 pad[8]; } Obj_24;

void height_collect_ground_tris(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy) {

    s32 r[4];
    union { u16 h[4]; u32 w[2]; } mask;
    s32 code[3];
    s32 idx;
    s32 hmin;
    s32 x, z, gx, gz, cx, cz, cell, k, j, n;
    s32 ox, oz, dx, dz;
    s32 bx0, bz0, bx1, bz1, bit;
    u16 f;
    Obj_24 *o;
    TList *tl;
    Ent_27 *en;
    Vtx16 *vt;
    u16 *vi;
    u16 *mp;
    CTri *t;
    s32 *p;

    x0 >>= 4;
    x1 >>= 4;
    z0 >>= 4;
    z1 >>= 4;
    gGroundTriCount = 0;
    hy >>= 4;
    if (gCurTrack_4->f2C == 2) {
        divmod_floor(x0, 125, &r[0], &idx);
        divmod_floor(z0, 125, &r[2], &idx);
        divmod_floor(x1 + 124, 125, &r[1], &idx);
        divmod_floor(z1 + 124, 125, &r[3], &idx);
        for (k = r[2]; k < r[3]; k++) {
            for (n = r[0]; n < r[1]; n++) {
                tri(idx, n * 2 + k * 8, n, k, n + 1, k, n + 1, k + 1);
                if (gGroundTriCount == 24) return;
                tri(idx, n * 2 + k * 8, n, k, n + 1, k + 1, n, k + 1);
                if (gGroundTriCount == 24) return;
            }
        }
        return;
    }
    gx = (x0 + x1) >> 1;
    gz = (z0 + z1) >> 1;
    gx = ((gx - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (gx < 0) gx = 0;
    if (gx >= 16) gx = 15;
    gz = ((gz - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (gz < 0) gz = 0;
    if (gz >= 16) gz = 15;
    cell = gz * 16 + gx;
    for (idx = 0; idx < sHeightGrid[cell]->n; idx++) {
        o = (Obj_24 *)(sHeightAreas + ((sHeightGrid[cell]->id[idx] << 5) + 4));
        if (x1 < o->x0) continue;
        if (z1 < o->z0) continue;
        if (o->x1 < x0) continue;
        if (o->z1 < z0) continue;
        if (!(o->tl->flags & 1)) fatal_halt(1);
        tl = o->tl;
        if (tl->flags & 4) {
            cx = (o->x1 + o->x0) / 2;
            cz = (o->z1 + o->z0) / 2;
        } else {
            cx = cz = 0;
        }
        r[0] = x0 - cx;
        r[1] = x1 - cx;
        r[2] = z0 - cz;
        r[3] = z1 - cz;
        dx = o->x1 - o->x0;
        ox = o->x0;
        bx0 = (x0 - ox) * 8 / dx;
        oz = o->z0;
        dz = o->z1 - oz;
        if (bx0 >= 8) bx0 = 7;
        if (bx0 < 0) bx0 = 0;
        bz0 = (z0 - oz) * 8 / dz;
        if (bz0 >= 8) bz0 = 7;
        if (bz0 < 0) bz0 = 0;
        bx1 = (x1 - ox) * 8 / dx;
        if (bx1 >= 8) bx1 = 7;
        if (bx1 < 0) bx1 = 0;
        bz1 = (z1 - oz) * 8 / dz;
        if (bz1 >= 8) bz1 = 7;
        if (bz1 < 0) bz1 = 0;
        mp = mask.h;
        for (n = bz0; n <= bz1; n++) {
            for (k = bx0; k <= bx1; k++) {
                bit = n * 8 + k;
                mp[bit / 16] |= 1 << (bit % 16);
            }
        }
        for (n = 0; n < tl->n; n++) {
            en = &tl->e[n];
            if (!((en->m0 & mask.w[0]) || (en->m1 & mask.w[1]))) continue;
            if (en->flags & 0x100) continue;
            vi = en->v;
            vt = o->m->v;
            for (k = 0; k < 3; k++) {
                code[k] = 0;
                if (vt[vi[k]].x < r[0]) code[k] = 1;
                if (r[1] < vt[vi[k]].x) code[k] |= 2;
                if (vt[vi[k]].z < r[2]) code[k] |= 4;
                if (r[3] < vt[vi[k]].z) code[k] |= 8;
            }
            if (code[0] & code[1] & code[2]) continue;
            if (en->flags & 0x400) {
                for (k = 0; k < 3; k++) {
                    if (!(hy < vt[vi[k]].y)) break;
                }
                if (k == 3) continue;
                for (k = 0; k < 3; k++) {
                    if (hy - 250 < vt[vi[k]].y) break;
                }
                if (k == 3) continue;
            }
            { CTri *b = gGroundTris; t = &b[gGroundTriCount]; }
            t->n = n;
            t->id = idx;
            for (k = 0; k < 3; k++) {
                gGroundTris[gGroundTriCount].v[k][0] = (cx + vt[vi[k]].x) * 16;
                gGroundTris[gGroundTriCount].v[k][1] = vt[vi[k]].y << 4;
                gGroundTris[gGroundTriCount].v[k][2] = (cz + vt[vi[k]].z) * 16;
            }
            if (++gGroundTriCount == 24) return;
        }
    }
}

typedef struct {
    u8 pad[0x1C];
    u8 *base;
} Hdr_4;

extern Hdr_4 *sWallMesh_2;

void height_push_off_wall(f32 *pos)
{
    s32 i;
    Ent_28 *e;
    f32 f;

    height_gather_walls((pos[0] - 1.0f) * 1000.0f, (pos[2] - 1.0f) * 1000.0f,
                  (pos[0] + 1.0f) * 1000.0f, (pos[2] + 1.0f) * 1000.0f);
    for (i = 0; i < sWallEdgeCount; i++) {
        e = sWallEdges[i];
        if (!(gWallEdgeFlags[((u8 *)e - sWallMesh_2->base) >> 3] & 0x20)) {
            f = __builtin_sqrtf(e->x * e->x + e->z * e->z);
            pos[0] += e->x / f;
            pos[2] += e->z / f;
            return;
        }
    }
}

typedef struct {
    s16 x, y, z;
} Face;

extern Face sDesertBoxNormals[];

s32 height_raycast_desert_objs(f32 *p1, f32 *p2, f32 *p3, f32 *p4)
{
    Face *bestFace;
    s32 best;
    s32 ax, ay, az, bx, by, bz;
    s32 n, i;
    s32 ex, ey, ez;

    ax = (s32)(p1[0] * 1000.0f);
    ay = (s32)(p1[1] * 1000.0f);
    az = (s32)(p1[2] * 1000.0f);
    bx = (s32)(p2[0] * 1000.0f);
    by = (s32)(p2[1] * 1000.0f);
    bz = (s32)(p2[2] * 1000.0f);
    best = 0;
    bestFace = 0;
    for (n = 0; n < sDesertObjCount; n++) {
        Node_5 *nd = &sDesertObjs[n];
        for (i = 0; i < 5; i++) {
            Face *f = &sDesertBoxNormals[i];
            s32 hw = nd->w;
            s32 hh = nd->h;
            s32 nx = nd->x;
            s32 ny = nd->y;
            s32 nz = nd->z;
            s32 px, py, pz;
            s32 d1, d2, dd;
            f32 t;
            s32 dx;
            px = ((hw * f->x) >> 14) + nx;
            py = ((hh * f->y) >> 14) + ny;
            pz = ((hw * f->z) >> 14) + nz;

            d1 = (bx - px) * f->x + (by - py) * f->y + (bz - pz) * f->z;
            if (d1 >= best) continue;
            pz = (ax - px) * f->x + (ay - py) * f->y + (az - pz) * f->z;
            if (pz < 0) continue;
            py = d1 - pz;
            if (py == 0) continue;
            dx = bx - ax;
            t = -(f32)pz / (f32)py;
            px = (f32)ax + t * (f32)dx;
            dx = by - ay;
            py = (f32)ay + t * (f32)dx;
            dx = bz - az;
            pz = (f32)az + t * (f32)dx;
            if (i < 2) {
                if (ny + hh < py || pz < nz - hw || nz + hw < pz) continue;
            } else if (i < 4) {
                if (ny + hh < py || px < nx - hw || nx + hw < px) continue;
            } else {
                if (px < nx - hw || nx + hw < px || pz < nz - hw || nz + hw < pz) continue;
            }
            bestFace = &sDesertBoxNormals[i];
            best = d1;
        }
    }
    if (bestFace) {
        *p4 = -((f32)best / 16384000.0f);
        p3[0] = bestFace->x * 6.1035156e-05f;
        p3[1] = bestFace->y * 6.1035156e-05f;
        p3[2] = bestFace->z * 6.1035156e-05f;
        return 1;
    }
    return 0;
}

void height_push_out_of_desert_objs(f32 *p)
{
    s32 nx, ny, i, dx, dy, ax, d2, lim;
    f32 r;

    nx = (s32)(p[0] * 1000.0f);
    ny = (s32)(p[2] * 1000.0f);
    height_gather_walls(nx - 0x1000, ny - 0x1000, nx + 0x1000, ny + 0x1000);
    if (sDesertObjCount > 0) {
    i = 0;
    lim = 0x900000;
    do {
        dx = sDesertObjs[i].x - nx;
        if (abs(dx) < 0xc01) {
            dy = sDesertObjs[i].z - ny;
            if (abs(dy) < 0xc01) {
                d2 = dx * dx + dy * dy;
                if (d2 <= lim) {
                    r = sqrtf((f32)d2);
                    nx = (s32)((f32)sDesertObjs[i].x - (f32)(dx * 0xc00) / r);
                    ny = (s32)((f32)sDesertObjs[i].z - (f32)(dy * 0xc00) / r);
                }
            }
        }
    } while (++i < sDesertObjCount);
    }
    p[0] = (f32)nx / 1000.0f;
    p[2] = (f32)ny / 1000.0f;
}

typedef struct { s16 a, b, nx, nz; } Edge;
typedef struct { s16 a, b, c, nx, ny, nz; } Tri12;
typedef struct { u8 pad[0x14]; Vtx8 *verts; u8 pad2[4]; Edge *edges; } Track_8;

extern Track_8 *sWallMesh_3;
extern Tri12 *sWallTris[];
extern Edge *sWallHitEdge;

u8 height_raycast_walls(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag)
{
    u8 result = 0;
    f32 sx, sy, sz, ex, ey, ez;
    s32 i;
    f32 t;
    f32 vx, vy, vz;
    f32 d1, d0, px;
    Vtx8 *a, *b;
    s32 iax, iaz, ibx, ibz;
    f32 unused[3];

    if (gCurTrack_5->mode == 2) {
        return height_raycast_desert_objs(start, end, out, tmax);
    }
    if (sWallMesh_3 == NULL) {
        return 0;
    }
    sx = start[0] * 1000.0f;
    sy = start[1] * 1000.0f;
    sz = start[2] * 1000.0f;
    ex = end[0] * 1000.0f;
    ey = end[1] * 1000.0f;
    ez = end[2] * 1000.0f;
    gWallHitFlags = 0;
    if (sWallEdgeCount != 0) {
        for (i = 0; i < sWallEdgeCount; i++) {
            Edge *e = sWallEdges[i];
            u8 fl = gWallEdgeFlags[(e - sWallMesh_3->edges)];
            f32 ax, az, fax, faz, dz, dx, py, y0, y1, len;

            if (fl & 0x20) continue;
            if (wantFlag && !(fl & 0x40)) continue;
            a = &sWallMesh_3->verts[e->a];
            iax = a->x;
            fax = iax;
            iaz = a->z;
            faz = iaz;
            ax = fax * 64.0f;
            az = faz * 64.0f;
            b = &sWallMesh_3->verts[e->b];
            ibz = b->z;
            dz = ibz - iaz;
            ibx = b->x;
            dx = iax - ibx;
            vx = ex - ax;
            vz = ez - az;
            d1 = vx * dz + vz * dx;
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vz = sz - az;
            d0 = vx * dz + vz * dx;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            px = start[0] + (end[0] - start[0]) * t;
            py = start[1] + (end[1] - start[1]) * t;
            vz = start[2] + (end[2] - start[2]) * t;
            px *= 15.625f;
            py *= 1000.0f;
            vz *= 15.625f;
            if (__builtin_abs(e->nz) > __builtin_abs(e->nx)) {
                if (px < fax && px < ibx) continue;
                if (fax < px && ibx < px) continue;
                vz = (px - fax) / (ibx - fax);
            } else {
                if (vz < faz && vz < ibz) continue;
                if (faz < vz && ibz < vz) continue;
                vz = (vz - faz) / (ibz - faz);
            }
            y0 = a->y + vz * (b->y - a->y);
            y0 *= 16.0f;
            y1 = a->h + vz * (b->h - a->h);
            y1 *= 16.0f;
            if (py < y0) continue;
            if (y0 + y1 <= py) continue;
            *tmax = t;
            len = __builtin_sqrtf(dz * dz + dx * dx);
            out[0] = dz / len;
            out[1] = 0.0f;
            out[2] = dx / len;
            result = 1;
            sWallHitEdge = e;
            gWallHitFlags = gWallEdgeFlags[(e - sWallMesh_3->edges)];
            D_800E8E78 = (gWallHitFlags >> 1) & 3;
        }
    }
    if (sWallTriCount != 0) {
        for (i = 0; i < sWallTriCount; i++) {
            Tri12 *tr = sWallTris[i];
            Vtx8 *v = sWallMesh_3->verts;
            f32 fax, faz, ax, ay, az, nx, ny, nz, c, len;
            Vtx8 *cc;
            s32 pos, neg, jz;
            a = &v[tr->a];

            ibx = a->x;
            fax = ibx;
            ax = fax * 64.0f;
            ay = (a->y + a->h) * 16.0f;
            jz = a->z;
            faz = jz;
            az = faz * 64.0f;
            nx = tr->nx;
            ny = tr->ny;
            nz = tr->nz;
            b = &v[tr->b];
            vx = ex - ax;
            vy = ey - ay;
            vz = ez - az;
            d1 = vx * nx + vy * ny + vz * nz;
            cc = &v[tr->c];
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vy = sy - ay;
            vz = sz - az;
            d0 = vx * nx + vy * ny + vz * nz;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            pos = neg = 0;
            px = (sx + (ex - sx) * t) * 0.015625f;
            vz = (sz + (ez - sz) * t) * 0.015625f;
            c = (b->x - ibx) * (vz - faz) - (b->z - jz) * (px - fax);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (cc->x - b->x) * (vz - b->z) - (cc->z - b->z) * (px - b->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (a->x - cc->x) * (vz - cc->z) - (a->z - cc->z) * (px - cc->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            if (pos == 3 || neg == 3) {
                len = __builtin_sqrtf(tr->nx * tr->nx + tr->ny * tr->ny + tr->nz * tr->nz);
                out[0] = tr->nx / len;
                out[1] = tr->ny / len;
                out[2] = tr->nz / len;
                result = 1;
                *tmax = t;
            }
        }
    }
    return result;
}

typedef struct {
    s32 x, y, z, r, a, b, c;
} Ent_30;

typedef struct {
    s32 cx;             /* 0x00 */
    s32 cz;             /* 0x04 */
    s32 f8, fC;
    s32 off;            /* 0x10 */
    u8 pad14[0x320 - 0x14];
    s32 count;          /* 0x320 */
    Ent_30 e[8];           /* 0x324 */
} Tile;

typedef struct {
    s16 v0, v1;
    s16 f4;
    s16 flags;          /* 0x06 */
} CEdge;

typedef struct {
    s16 v[3];
    s16 f6, f8;
    s16 flags;          /* 0x0A */
} CTri_2;

typedef struct {
    u16 *tris;
    u16 *edges;
} CCell;

typedef struct {
    s32 minx, minz, maxx, maxz;
    s32 f10;
    Vtx8 *verts;        /* 0x14 */
    s32 f18;
    CEdge *edges;       /* 0x1C */
    s32 f20;
    CTri_2 *tris;         /* 0x24 */
    CCell cells[257];   /* 0x28 */
} Grid_2;

extern Tile *gDesertTiles_2;
extern Ent_30 sDesertObjs_2[];
extern Grid_2 *sWallMesh_4;
extern CEdge *sWallEdges_2[];
extern CTri_2 *sWallTris_2[];

void height_gather_walls(s32 x0, s32 z0, s32 x1, s32 z1)
{
    s32 px[3];
    s32 py[3];
    s32 pz[3];
    s32 qz[3];
    s32 i, j, k, m;
    s32 ix0, iz0, ix1, iz1;
    s32 n, mask, v, lo, hi;
    CCell *cell;
    CEdge *e;
    CTri_2 *t;
    Vtx8 *a, *b;

    sDesertObjCount = 0;
    if (gCurTrack_5->mode == 2) {
        sDesertObjCount = 0;
        for (i = 0; i < 16; i++) {
            if (gDesertTiles_2[i].off != 0) continue;
            if (x1 < gDesertTiles_2[i].cx * 256000) continue;
            if (z1 < gDesertTiles_2[i].cz * 256000) continue;
            if ((gDesertTiles_2[i].cx + 1) * 256000 < x0) continue;
            if ((gDesertTiles_2[i].cz + 1) * 256000 < z0) continue;
            for (j = 0; j < gDesertTiles_2[i].count; j++) {
                if (x1 >= gDesertTiles_2[i].e[j].x - gDesertTiles_2[i].e[j].r
                    && z1 >= gDesertTiles_2[i].e[j].z - gDesertTiles_2[i].e[j].r
                    && gDesertTiles_2[i].e[j].x + gDesertTiles_2[i].e[j].r >= x0
                    && gDesertTiles_2[i].e[j].z + gDesertTiles_2[i].e[j].r >= z0) {
                    sDesertObjs_2[sDesertObjCount] = gDesertTiles_2[i].e[j];
                    sDesertObjCount++;
                    if (sDesertObjCount == 8) return;
                }
            }
            if (sDesertObjCount == 8) return;
        }
    } else if (sWallMesh_4 != NULL) {
        sWallEdgeCount = 0;
        sWallTriCount = 0;
        ix0 = ((x0 / 16 - sWallMesh_4->minx) * 16) / (sWallMesh_4->maxx - sWallMesh_4->minx);
        iz0 = ((z0 / 16 - sWallMesh_4->minz) * 16) / (sWallMesh_4->maxz - sWallMesh_4->minz);
        ix1 = ((x1 / 16 - sWallMesh_4->minx) * 16) / (sWallMesh_4->maxx - sWallMesh_4->minx);
        iz1 = ((z1 / 16 - sWallMesh_4->minz) * 16) / (sWallMesh_4->maxz - sWallMesh_4->minz);
        if (ix0 > 16 || iz0 > 16 || ix1 < 0 || iz1 < 0) return;
        if (ix0 < 0) ix0 = 0;
        if (ix0 > 15) ix0 = 15;
        if (iz0 < 0) iz0 = 0;
        if (iz0 > 15) iz0 = 15;
        if (ix1 > 15) ix1 = 15;
        if (iz1 > 15) iz1 = 15;
        for (i = ix0; i <= ix1; i++) {
            for (j = iz0; j <= iz1; j++) {
                cell = &sWallMesh_4->cells[j * 16 + i];
                n = (u32)sWallMesh_4->cells[j * 16 + i + 1].edges - (u32)cell->edges;
                n = (u32)n >> 1;
                for (k = 0; k < n; k++) {
                    if (sWallEdgeCount >= 0x100) break;
                    e = &sWallMesh_4->edges[cell->edges[k]];
                    if (e->flags & 1) continue;
                    a = &sWallMesh_4->verts[e->v0];
                    b = &sWallMesh_4->verts[e->v1];
                    if (a->x < b->x) {
                        if ((a->x << 6) >= x1 || x0 >= (b->x << 6)) continue;
                    } else {
                        if ((b->x << 6) >= x1 || x0 >= (a->x << 6)) continue;
                    }
                    if (a->z < b->z) {
                        if ((a->z << 6) >= z1 || z0 >= (b->z << 6)) continue;
                    } else {
                        if ((b->z << 6) >= z1 || z0 >= (a->z << 6)) continue;
                    }
                    {
                        {
                            sWallEdges_2[sWallEdgeCount] = e;
                            sWallEdgeCount++;
                            e->flags |= 1;
                        }
                    }
                }
                cell = &sWallMesh_4->cells[j * 16 + i];
                n = (u32)sWallMesh_4->cells[j * 16 + i + 1].tris - (u32)cell->tris;
                n = (u32)n >> 1;
                for (k = 0; k < n + ((n & 1) >> 1); k++) {
                    if (sWallTriCount >= 0x100) break;
                    t = &sWallMesh_4->tris[cell->tris[k]];
                    if (t->flags & 1) continue;
                    mask = 0xF;
                    for (m = 0; m < 3; m++) {
                        a = &sWallMesh_4->verts[t->v[m]];
                        v = a->x << 6;
                        if (v >= x0) mask &= ~1;
                        if (x1 >= v) mask &= ~2;
                        v = a->z << 6;
                        if (v >= z0) mask &= ~4;
                        if (z1 >= v) mask &= ~8;
                    }
                    if (mask == 0) {
                        sWallTris_2[sWallTriCount++] = t;
                        t->flags |= 1;
                    }
                }
            }
        }
        for (i = 0; i < sWallEdgeCount; i++) {
            sWallEdges_2[i]->flags &= ~1;
        }
        for (i = 0; i < sWallTriCount; i++) {
            sWallTris_2[i]->flags &= ~1;
        }
        if (gDrawFlags & 0x200) {
            for (i = 0; i < sWallEdgeCount; i++) {
                px[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[1] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                pz[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[1] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                height_get_ground(px[0], 10000, pz[0]);
                height_get_ground(px[2], 10000, pz[2]);
                py[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].y * 16;
                py[1] = (sWallMesh_4->verts[sWallEdges_2[i]->v0].y + sWallMesh_4->verts[sWallEdges_2[i]->v0].h) * 16;
                py[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].y * 16;
                px[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[1] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                px[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                pz[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[1] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                pz[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                py[0] = (sWallMesh_4->verts[sWallEdges_2[i]->v0].y + sWallMesh_4->verts[sWallEdges_2[i]->v0].h) * 16;
                py[1] = (sWallMesh_4->verts[sWallEdges_2[i]->v1].y + sWallMesh_4->verts[sWallEdges_2[i]->v1].h) * 16;
            }
            for (i = 0; i < sWallTriCount; i++) {
                px[0] = sWallMesh_4->verts[sWallTris_2[i]->v[0]].x << 6;
                px[1] = sWallMesh_4->verts[sWallTris_2[i]->v[1]].x << 6;
                px[2] = sWallMesh_4->verts[sWallTris_2[i]->v[2]].x << 6;
                py[0] = (sWallMesh_4->verts[sWallTris_2[i]->v[0]].y + sWallMesh_4->verts[sWallTris_2[i]->v[0]].h) * 16;
                py[1] = (sWallMesh_4->verts[sWallTris_2[i]->v[1]].y + sWallMesh_4->verts[sWallTris_2[i]->v[1]].h) * 16;
                py[2] = (sWallMesh_4->verts[sWallTris_2[i]->v[2]].y + sWallMesh_4->verts[sWallTris_2[i]->v[2]].h) * 16;
                qz[0] = sWallMesh_4->verts[sWallTris_2[i]->v[0]].z << 6;
                qz[1] = sWallMesh_4->verts[sWallTris_2[i]->v[1]].z << 6;
                qz[2] = sWallMesh_4->verts[sWallTris_2[i]->v[2]].z << 6;
            }
        }
    }
}

typedef struct {
    s16 x;
    s16 y;
    s32 pad;
} Pt_2;

typedef struct {
    s16 a;
    s16 b;
    s16 nx;
    s16 ny;
} Edge_2;

typedef struct {
    u8 pad[10];
    s16 flags;
} Rec_16;

struct Hdr_5 {
    s32 pad[5];
    Pt_2 *pts;
    s32 nedges;
    Edge_2 *edges;
    s32 nrecs;
    Rec_16 *recs;
    Pair tab[0x101];
};

void height_init_walls(Hdr_5 **param_1, s32 param_2)
{
    Hdr_5 *h = *param_1;
    s32 i;
    s32 dx;
    s32 dy;
    s32 len;

    if (param_2 != 0) {
        h->pts = (Pt_2 *)((s32)h->pts + (s32)h);
        h->edges = (Edge_2 *)((s32)h->edges + (s32)h);
        h->recs = (Rec_16 *)((s32)h->recs + (s32)h);
        for (i = 0; i < 0x101; i++) {
            h->tab[i].a += (s32)h;
            h->tab[i].b += (s32)h;
        }
    }
    gWallEdgeFlags = mem_alloc_dbg(h->nedges, 0, D_800D6C20, 0x87F);
    mem_fill(gWallEdgeFlags, 0, h->nedges);
    for (i = 0; i < h->nedges; i++) {
        gWallEdgeFlags[i] |= 1;
        if (h->edges[i].nx == -0x8000) {
            gWallEdgeFlags[i] = ((u8 *)&h->edges[i])[7] ^ 1;
        }
        dx = h->pts[h->edges[i].b].x - h->pts[h->edges[i].a].x;
        dy = h->pts[h->edges[i].b].y - h->pts[h->edges[i].a].y;
        len = isqrt_u32_4(dx * dx + dy * dy);
        if (len != 0) {
            dx = (dx * 0x4000) / len;
            dy = (dy * 0x4000) / len;
        }
        h->edges[i].nx = dy;
        h->edges[i].ny = -dx;
    }
    for (i = 0; i < h->nedges; i++) {
        h->edges[i].ny &= ~1;
    }
    for (i = 0; i < h->nrecs; i++) {
        h->recs[i].flags &= ~1;
    }
}

void height_init_walls_reloc(int *param_1)
{
    height_init_walls(param_1, 1);
}

void height_free_walls(void)
{
    mem_free(gWallEdgeFlags);
    gWallEdgeFlags = 0;
}

s32 point_in_tri_xz(s32 *p1, s32 *p2, s32 x, s32 y)
{
    s32 b0 = p2[0];
    s32 a1 = p1[1];
    s32 a0 = p1[0];
    s32 b1 = p2[1];
    s32 a2;
    s32 b2;

    if ((y - b0) * (a1 - a0) - (x - a0) * (b1 - b0) <= 0) {
        return 0;
    }
    a2 = p1[2];
    b2 = p2[2];
    if ((y - b1) * (a2 - a1) - (x - a1) * (b2 - b1) <= 0) {
        return 0;
    }
    return (y - b2) * (a0 - a2) - (x - a2) * (b0 - b2) > 0;
}

int height_get_grid_cell(int x, int y)
{
    int a, b;

    b = y >> 4;
    a = x >> 4;
    a = (a - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (a < 0) a = 0;
    if (a > 15) a = 15;
    b = (b - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (b < 0) b = 0;
    if (b > 15) b = 15;
    return b * 16 + a;
}

struct Poly_2 {
    u8 hdr[6];
    u16 n;
    Pt pts[1];
};

s32 point_in_poly_xz(Poly_2 *poly, s32 px, s32 pz)
{
    s32 pos = 0;
    s32 neg;
    s32 i = 0, j, n, cr;

    n = poly->n;
    neg = 0;
    px = px >> 6;
    pz = pz >> 6;
    if (n != 0) {
        do {
            j = (i + 1) % n;
            cr = (pz - poly->pts[i].z) * (poly->pts[j].x - poly->pts[i].x);
            cr -= (px - poly->pts[i].x) * (poly->pts[j].z - poly->pts[i].z);
            if (cr >= 0) {
                pos++;
            }
            if (cr <= 0) {
                neg++;
            }
            n = poly->n;
            i++;
        } while (i < n);
    }
    if (pos == poly->n) {
        goto ret1;
    }
    if (neg != poly->n) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

s32 func_80031FA0(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.b && p3 >= D_800E8518.d) {
        return 0;
    }
    if (p3 < D_800E8518.b && p3 < D_800E8518.d) {
        return 0;
    }
    t = ((p3 - D_800E8518.b) << 8) / (D_800E8518.d - D_800E8518.b);
    v = D_800E8518.a + ((D_800E8518.c - D_800E8518.a) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}

s32 func_8003207C(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.a && p3 >= D_800E8518.c) {
        return 0;
    }
    if (p3 < D_800E8518.a && p3 < D_800E8518.c) {
        return 0;
    }
    t = ((p3 - D_800E8518.a) << 8) / (D_800E8518.c - D_800E8518.a);
    v = D_800E8518.b + ((D_800E8518.d - D_800E8518.b) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}

typedef struct {
    s32 flags;
    u8 pad[0x20];
} Ent_31;

typedef struct {
    s32 f0;
    s32 n;
    Ent_31 e[1];
} Hdr_6;

typedef struct {
    u8 pad[4];
    s16 v;
    u8 pad2[0xA];
} Cell_3;

typedef struct {
    u8 w, h;
    u8 pad[2];
    u8 *buf;
} Item_4;

typedef struct {
    u32 n;
    u32 pad;
    Item_4 it[1];
} Tab;

typedef struct {
    u8 pad[0x2C];
    s32 mode;
    u8 pad2[0x20];
    s32 flags;
} Cfg_6;

extern Hdr_6 *sSunList_2;
extern Cell_3 *sSunVtxBufs_2[2];
extern Tab *sFlareTextures;
extern Cfg_6 *gCurTrack_23;

void sun_init(void)
{
    s32 v;
    s32 i, j, k, n, a, b, c;
    s32 x, y;

    sStars = 0;
    sSunVtxBufs_2[0] = 0;
    sSunVtxBufs_2[1] = 0;
    sSunList_2 = 0;
    if (gCurTrack_23->mode == 2) {
        v = D_800D0F78;
    } else {
        v = track_res_find(D_800D14EC);
    }
    if (v < 0) {
        sSunList_2 = 0;
        return;
    }
    a = 0;
    sSunList_2 = res_load(D_800E7728, v);
    for (; a < sSunList_2->n; a++) {
        sSunList_2->e[a].flags &= ~0x18;
    }
    for (j = 0; j < 2; j++) {
        sSunVtxBufs_2[j] = mem_alloc_dbg(0x800, 0, D_800D6C80, 0xCB);
        for (n = 0; n < 0x80; n++) {
            sSunVtxBufs_2[j][n].v = 1000;
        }
    }
    sFlareTextures = res_load(D_800E7728, sSunList_2->f0);
    for (a = 0; a < sFlareTextures->n; a++) {
        n = sFlareTextures->it[a].w * sFlareTextures->it[a].h * 8;
        for (k = 0; k < n; k++) {
            sFlareTextures->it[a].buf[k] |= 0xF0;
        }
    }
    if (gCurTrack_23->flags & 0x20) {
        sStarTexture = res_load(D_800E7728, D_800D0E00);
        sStars = mem_alloc_dbg(0x300, 0, D_800D6C80, 0xE3);
        for (i = 0; i < 0x40; i++) {
            x = (random_byte() - 0x80) * 16;
            y = (random_byte() - 0x80) * 16;
            sStars[i].d = isqrt_u32_4(0x400000 - (x * x + y * y));
            sStars[i].x = x;
            sStars[i].y = y;
        }
    }
    gSunGlare = 0;
}

typedef struct {
    u32 flags;
    s32 a1, a2, a3;
    s32 p10;
    s32 v14, v18, v1C;
    s32 p20;
} Rec_17;

typedef struct {
    s32 h0;
    s32 count;
    Rec_17 r[1];
} Tab_2;

extern Tab_2 *sSunList_3;

void sun_update_screen_pos(s32 param_1, s32 param_2, s32 arg2)
{
    s32 A, B, C, D, E;
    s32 i;
    Rec_17 *rp;
    u32 fl;

    if (sSunList_3 != NULL) {
        for (i = 0; i < sSunList_3->count; i++) {
            rp = &sSunList_3->r[i];
            fl = rp->flags;
            rp->flags = fl & ~8;
            if (fl & 1) {
                if (sky_get_sun_screen_pos(&A, &B, param_1, param_2) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else if (fl & 2) {
                if (project_to_screen(0, 0x14, -0x12C, &A, &B, &C, &D, param_1, 0) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else {
                if (project_to_screen(rp->a1, rp->a2, rp->a3, &A, &B, &C, &E, param_1, param_2) == 0) {
                    rp->v14 = A;
                    rp->v18 = B;
                    rp->v1C = C;
                    rp->flags |= 8;
                }
            }
            if (rp->v14 < -0x32 || rp->v18 < -0x32 ||
                gScreenWidth + 0x32 < rp->v14 || gScreenHeight + 0x32 < rp->v18) {
                rp->flags &= ~8;
            }
        }
    }
}

typedef struct { u32 flags; s32 pad[4]; s32 x, y, z; s32 res; } Obj_25;
typedef struct { s32 pad; s32 n; Obj_25 o[1]; } Lst_2;

extern Lst_2 *sSunList_4;

void sun_check_occlusion(s32 a0)
{
    s32 i, r, c;
    s32 x, y, thr, v, cnt, tot;
    s32 x0, y0, w, h, xe, ye;
    Obj_25 *o;
    u16 *tb;

    if (sSunList_4 == NULL) {
        return;
    }
    osInvalDCache(0, 0x400000);
    w = gViewRects_2.w;
    h = gViewRects_2.h;
    x0 = gViewRects_2.x;
    y0 = gViewRects_2.y;
    i = 0;
    if (i < sSunList_4->n) {
        do {
            o = &sSunList_4->o[i];
            o->res = -1;
            if (!(o->flags & 8)) {
                o->flags &= ~0x10;
            } else {
                tot = 0;
                cnt = 0;
                x = o->x - 4;
                y = o->y - 4;
                thr = o->z - 0x19;
                for (r = 0; r < 8; r++) {
                    if (y < y0) {
                        y = y0;
                    }
                    if (y >= y0 + h) {
                        y = y0 + h - 1;
                    }
                    for (c = 0; c < 8; c++) {
                        if (x < x0) {
                            x = x0;
                        }
                        if (x >= x0 + w) {
                            x = x0 + w - 1;
                        }
                        tb = &gZBuffer_2[y * gScreenWidth + x];
                        v = zbuffer_decode(*tb);
                        if (v > 0x7FFF) {
                            v = 0x7FFF;
                        }
                        if (v >= thr) {
                            cnt++;
                        }
                        tot++;
                        x++;
                    }
                    x -= 8;
                    y++;
                }
                if (tot != 0) {
                    cnt = cnt * 7 / tot;
                }
                cnt--;
                if (cnt > 5) {
                    cnt = 5;
                }
                o->res = cnt;
                o->flags |= 0x10;
            }
            i++;
        } while (i < sSunList_4->n);
    }
}

#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]
#define COSS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

typedef struct {
    s32 flags;  /* 0x00 */
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 x;      /* 0x14 */
    s32 y;      /* 0x18 */
    s32 f1C;
    s32 tex;    /* 0x20 */
} Ent_32;

typedef struct {
    s32 f0;
    s32 n;
    Ent_32 e[1];
} EntList_2;

typedef struct {
    s16 x, y, z;
    u16 flag;
    s16 s, t;
    u8 c[4];
} V;

extern EntList_2 *sSunList_5;
extern V *sSunVtxBufs_3[];

static inline V *allocVtx(s32 idx, s32 n) {
    s32 c = sSunVtxUsed[idx];
    if (c + n > 128) {
        return NULL;
    }
    sSunVtxUsed[idx] = c + n;
    return &sSunVtxBufs_3[idx][c];
}

Gfx *sun_draw_flare(Gfx *gdl, u8 *mtx, s32 idx) {
    s32 i, j, k;
    Ent_32 *e;
    V *v;
    TexInfo *t;
    s32 a, b, p, q, x, y, ang, c, s, m, n, w;
    s32 bb;
    s32 aa;
    V *vv;

    if (sSunList_5 == NULL) {
        return gdl;
    }
    sSunVtxUsed[idx] = 0;
    if (gCheatFlags & 0x40) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPViewport(gdl++, &gScreenViewport);
    gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects_2.x, gViewRects_2.y, gViewRects_2.x + gViewRects_2.w - 1,
                  gViewRects_2.y + gViewRects_2.h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);

    gSunGlare = 0;
    for (i = 0; i < sSunList_5->n; i++) {
        e = &sSunList_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if (e->tex >= 0) {
            p = gScreenWidth / 2 - e->x;
            q = gScreenHeight / 2 - e->y;
            a = 0x80 - isqrt_u32_2(p * p + q * q);
            if (a >= 0) {
                a = a * e->tex / 6;
                if (a != 0) {
                    a = a * 192 / 128;
                    if (i == 0) {
                        gSunGlare = a;
                    }
                    v = allocVtx(idx, 28);
                    if (v != NULL) {
                        x = e->x - gScreenWidth / 2;
                        y = e->y - gScreenHeight / 2;
                        p = x / 3;
                        q = y / 3;
                        x -= p;
                        y -= q;
                        for (j = 0; j < 4; j++) {
                            v[j * 7].x = x;
                            v[j * 7].y = -y;
                            v[j * 7].z = 50;
                            for (k = 0; k < 6; k++) {
                                b = -y;
                                ang = ((k * 2 + 1) << 15) / 6;
                                v[j * 7 + k + 1].x = x + COSS(ang) * sFlareSizes[j] / 16384;
                                v[j * 7 + k + 1].y = b + SINS(ang) * sFlareSizes[j] / 16384;
                                v[j * 7 + k + 1].z = 50;
                            }
                            x -= p;
                            y -= q;
                            if (j & 1) {
                                x -= p;
                                y -= q;
                            }
                        }
                        gSPVertex(gdl++, v, 28, 0);
                        for (j = 0; j < 4; j++) {
                            gDPPipeSync(gdl++);
                            gDPSetPrimColor(gdl++, 0, 0, sFlareColors[j * 3], sFlareColors[j * 3 + 1],
                                            sFlareColors[j * 3 + 2], a);

                            for (k = 0; k < 3; k++) {
                                s32 n;
                                w = k * 2 + 2;
                                m = k * 2 + 1;
                                n = w % 6 + 1;
                                gSP2Triangles(gdl++, j * 7, j * 7 + w, j * 7 + m, 0, j * 7, j * 7 + n, j * 7 + w, 0);
                            }
                        }
                    }
                }
            }
        }
    }}}

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetAlphaDither(gdl++, G_AD_NOISE);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    if (gCurTrack_4->f2C == 0) {
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
    } else {
        gDPSetTexturePersp(gdl++, G_TP_NONE);
    }

    for (i = 0; i < sSunList_5->n; i++) {
        e = &sSunList_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if ((n = e->tex) >= 0) {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            t = &sFlareTextures_2[n + 1];
            if (gCurTrack_4->f2C == 0) {
                x = e->x - gScreenWidth / 2;
                y = gScreenHeight / 2 - e->y;
                q = t->h - 1;
                c = (COSS((x + y) * 32) * q >> 14) * 2 / 3;
                s = (SINS((x + y) * 32) * q >> 14) * 2 / 3;
                vv = allocVtx(idx, 4);
                if (vv != NULL) {
                    vv[0].x = x + c;
                    vv[0].y = y + s;
                    vv[0].s = 0;
                    vv[0].t = 0;
                    vv[1].x = x + s;
                    vv[1].y = y - c;
                    vv[1].s = q << 6;
                    vv[1].t = 0;
                    vv[2].x = x - c;
                    vv[2].y = y - s;
                    vv[2].s = q << 6;
                    vv[2].t = q << 6;
                    vv[3].x = x - s;
                    vv[3].y = y + c;
                    vv[3].s = 0;
                    vv[3].t = q << 6;
                    gSPVertex(gdl++, vv, 4, 0);
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
                }
            } else {
                bb = aa = 0;
                x = e->x - t->w * 4;
                y = e->y - t->h / 2;
                p = x + t->w * 8;
                q = y + t->h;
                if (x < 0) {
                    bb = -x;
                    x = 0;
                }
                if (y < 0) {
                    aa = -y;
                    y = 0;
                }
                if (p >= gScreenWidth) {
                    p = gScreenWidth - 1;
                }
                if (q >= gScreenHeight) {
                    q = gScreenHeight - 1;
                }
                if (x < p && y < q) {
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSPTextureRectangle(gdl++, x << 2, y << 2, p << 2, q << 2, G_TX_RENDERTILE, bb << 5, aa << 5,
                                        0x400, 0x400);
                }
            }
        }
    }}}

    gSPSetGeometryMode(gdl++, gCullMode);
    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(gdl++, G_AD_DISABLE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPMatrix(gdl++, mtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

Gfx *sun_draw_stars(Gfx *gdl, s32 arg)
{
    s32 c, i;
    u8 *img;
    s32 sx, sy, sz, sw;

    if (sSunList == 0) return gdl;
    if (*(s32 *)(gCurTrack + 0x50) & 0x20) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        { Gfx *_g = gdl++; _g->words.w0 = 0xFC11FE23; _g->words.w1 = 0xFFFFFFFF; }
        img = *(u8 **)(sStarTexture + 0xC) + 0x28;
        gDPLoadTextureBlock(gdl++, img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0, c = 0x80; i < 0x40; i++) {
            if (project_to_screen(sStars_2[i].x + (gCamPos_2.x >> 4), sStars_2[i].y + (gCamPos_2.y >> 4),
                              sStars_2[i].z + (gCamPos_2.z >> 4), &sx, &sy, &sz, &sw, arg, 0) == 0
                && sx >= 0 && sy >= 0 && sx <= gScreenWidth && sy <= gScreenHeight) {
                gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
                gSPTextureRectangle(gdl++, (sx - 1) << 2, (sy - 1) << 2, (sx + 1) << 2, (sy + 1) << 2,
                                    0, 0, 0, 0x400, 0x400);
            }
            c += 2;
        }
    }
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    return gdl;
}

s32 project_to_screen(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9)
{
    f32 m[4][4];
    s16 *ps;
    f32 kx, ky, kz, kw;
    f32 fx, fy, fz, a, b, c, w;
    s32 ret;

    ps = (s16 *)(p9 * 16 + gViewports_2);
    kx = (f32)ps[0] * 0.25f;
    ky = (f32)ps[1] * 0.25f;
    kw = (f32)ps[4] * 0.25f;
    kz = (f32)ps[5] * 0.25f;
    guMtxL2F(&m[0][0], (u32 *)(p8 + p9 * 0x40));

    fx = (f32)x;
    fy = (f32)y;
    fz = (f32)z;
    a = m[0][0] * fx + m[1][0] * fy + m[2][0] * fz + m[3][0];
    b = m[0][1] * fx + m[1][1] * fy + m[2][1] * fz + m[3][1];
    c = m[0][2] * fx + m[1][2] * fy + m[2][2] * fz + m[3][2];
    w = m[0][3] * fx + m[1][3] * fy + m[2][3] * fz + m[3][3];
    ret = 1;
    if (w != 0.0f) {
        a = a / w;
        b = b / w;
        c = c / w;
        if (c <= -1.0f) {
            return ret;
        }
        if (1.0f <= c) {
            return ret;
        }
        *p4 = (s32)(a * kx + kw);
        *p5 = (s32)(b * -ky + kz);
        *p6 = (s32)(c * 1023.0f * 0.5f + 511.5f) << 5;
        if (p7 != 0) {
            *p7 = (s32)w;
        }
        ret = 0;
    }
    return ret;
}

void sun_free(void) {
    if (sStars != 0) {
        mem_free(sStars);
        sStars = 0;
    }
    if (sSunVtxBufs[0] != 0) {
        mem_free(sSunVtxBufs[0]);
        mem_free(sSunVtxBufs[1]);
    }
    sSunList = 0;
}

s32 zbuffer_decode(s32 x) {
    s32 off = x >> 11;
    s32 v = (x >> 2) & 0x7FF;
    u8 *p1 = sZDecodeShift;
    s32 sh = *(s32 *)(p1 + (off & 0x1C));
    s32 add = *(s32 *)(sZDecodeAdd + (off & 0x1C));
    return ((v << sh) + add) >> 3;
}

s32 sun_alloc_vtx(s32 a, s32 idx) {
    s32 n = sSunVtxUsed[idx] + a;
    if (n >= 0x81) {
        return 0;
    }
    sSunVtxUsed[idx] = n;
    return sSunVtxBufs[idx] + (n - a) * 16;
}
