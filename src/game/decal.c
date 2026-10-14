#include "game.h"

typedef struct {
    u8 pad0[0x44C];
    f32 f44C;
    f32 f450;
    u8 pad1[0x7E0 - 0x454];
} Ent_58;

typedef struct {
    u8 pad0[0x20];
    f32 f20[1];
} Ply;

extern Ply *gCurTrack_31;
extern Ent_58 gRiders_73[];

void debug_update_value_display(void)
{
    s32 i;
    char buf[0x28];

    if (gPlayerInputs_2 & 0x80) {
        if (gPlayerInputsPrev == 0) {
            sDebugPage++;
            if (sDebugPage == 4) {
                sDebugPage = 0;
            }
        }
    }
    for (i = 0x13; i >= 0; i--) {
    }
    switch (sDebugPage) {
    case 1:
        if (*(s32 *)((u8 *)gCurTrack_31 + 0x2C) == 2) {
            break;
        }
        if (D_800C8D08 != 0 || D_800C8D10 != 0 || D_800C8D00 != 0 || D_800C8D18 != 0) {
            break;
        }
        debug_print_float(D_800D8B10, D_800E76E8);
        debug_print_float(D_800D8B18, D_800E76EC);
        break;
    case 0:
        debug_print_float(D_800D8B20, D_800E76E8);
        debug_print_float(D_800D8B24, D_800E76EC);
        debug_print_float(D_800D8B28, D_800E76F0);
        debug_print_float(D_800D8B2C, D_800E76F4);
        debug_print_float(D_800D8B30, D_800E76F8);
        debug_print_float(D_800D8B34, D_800E76FC);
        break;
    case 2:
        for (i = 0; i < 6; i++) {
            debug_ftoa(buf, gRiders_73[i].f44C / gCurTrack_31->f20[gGame.difficulty - 1], 4);
            debug_print_float(buf, gRiders_73[i].f450);
        }
        break;
    }
}

typedef struct {
    u8 b10[16];
    u8 b20[16];
    u8 b30[16];
    u8 b40[16];
} Frame5CB20;

void debug_dump_spline(void)
{
    Frame5CB20 f;
    s32 k;
    u32 u;

    debug_log(D_800CB17C);
    int_to_str(sSplineNodeCount, f.b40);
    debug_log(f.b40);
    debug_log(D_800CB1D4);
    for (k = 1; k <= sSplineNodeCount; k++) {
        int_to_str((s32)(sSplineNodes[k].x * 1000.0f), f.b10);
        int_to_str((s32)(sSplineNodes[k].z * 1000.0f), f.b30);
        u = height_get_ground((s32)(sSplineNodes[k].x * 1000.0f), (s32)(sSplineNodes[k].y * 1000.0f), (s32)(sSplineNodes[k].z * 1000.0f));
        int_to_str(u, f.b20);
        debug_log(f.b10);
        debug_log(D_800D8B44);
        debug_log(f.b30);
        debug_log(D_800D8B48);
    }
    debug_log(D_800CB1F0);
}

struct Obj_40 {
    u8 pad[0x48];
    Vec3f pos;
};

void debug_record_spline_node(Obj_40 *o)
{
    s32 n;
    s32 ni;
    Vec3f **gp;
    Vec3f *p;
    Vec3f *q;
    f32 dx;
    f32 dy;
    f32 dz;

    if (sSplineNodeCount < 2000) {
        gp = &sSplineNodes;
        n = sSplineNodeCount;
        p = (Vec3f *)((u32)(n * 12) + (u32)*gp);
        dx = p->x - o->pos.x;
        dy = p->y - o->pos.y;
        dz = p->z - o->pos.z;
        if (1.0f < dx * dx + dy * dy + dz * dz) {
            ni = n + 1;
            q = (Vec3f *)((u32)(ni * 12) + (u32)*gp);
            sSplineNodeCount = ni;
            *q = o->pos;
        }
    }
}

typedef struct {
    u8 c[4];
} S4_2;

extern S4_2 D_800D8B64;

void debug_ftoa(char *p, f32 x, s32 n)
{
    s32 ip;
    s32 pw;
    s32 d;

    if (x < 0.0f) {
        *p++ = '-';
        x = -x;
    } else {
        *p++ = ' ';
    }
    if (x >= 1.0f) {
        if (x > 1000000.0f) {
            *(S4_2 *)p = D_800D8B64;
            return;
        }
        pw = 10;
        ip = (s32)x;
        x -= (f32)ip;
        while (ip >= pw) {
            pw *= 10;
        }
        pw /= 10;
        do {
            n--;
            *p++ = ip / pw + '0';
            ip -= (ip / pw) * pw;
            pw /= 10;
        } while (pw != 0);
    } else {
        *p++ = '0';
    }
    if (n > 0) {
        if (x > 0.0001f) {
            *p++ = '.';
        }
        do {
            n--;
            {
                f32 t = x;
                if (!(t > 0.0001f)) break;
            }
            ip = (s32)(x * 10.0f + 0.0001f);
            *p++ = ip + '0';
            x = x * 10.0f - (f32)ip;
        } while (n > 0);
    }
    *p = '\0';
}

f32 func_8005CEFC(f32 x)
{
    if (!(0.0f < x)) {
        x = -x;
    }
    return x / 200.0f;
}

f32 debug_round_sig3(f32 x)
{
    f32 a;
    f32 b;
    f32 sx;
    f32 res;

    sx = x;
    a = x;
    if (0.0f < sx) {
        a = x;
    } else {
        a = -x;
    }
    b = 1.0f;
    for (; b < a; b = b * 10.0f) {
    }
    for (; a < b; b = b / 10.0f) {
    }
    res = a;
    if (b == 0.0f) {
        return res;
    }
    a = ((f32)(s32)((a * 100.0f) / b + 0.5f) * b) / 100.0f;
    if (0.0f < sx) {
        return a;
    }
    return -a;
}

void debug_print_float(char *label, f32 value)
{
    char buf[40];
    s32 len;
    str_copy(buf, label);
    len = strlen(buf);
    debug_ftoa(buf + len, value, 3);
}

void debug_print_float2(char *label, f32 a, f32 b)
{
    char buf[40];
    s32 n;
    s32 i;

    mem_fill((u8 *)buf, 0x20, 0x1E);
    str_copy(buf, label);
    n = strlen(buf);
    debug_ftoa(buf + n, a, 3);
    i = 0;
    if (0 < n + 6) {
        do {
            if (buf[i] == 0) {
                buf[i] = ' ';
            }
            i++;
        } while (i < n + 6);
    }
    debug_ftoa(buf + 6 + n, b, 3);
}

void debug_print_vec3(s32 arg0, f32 *v)
{
    char bx[24];
    char by[24];
    char bz[24];

    strcpy(bx, "\nx=");
    debug_ftoa(bx + strlen(bx), v[0], 6);
    strcpy(by, "y=");
    debug_ftoa(by + strlen(by), v[1], 6);
    strcpy(bz, "z=");
    debug_ftoa(bz + strlen(bz), v[2], 6);
}

void debug_print_int(char *label, s32 value) {
    char pad[16];
    char buf[40];
    char *b;

    b = buf;
    str_copy(b, label);
    int_to_str(value, b + strlen(b));
}

void debug_print_hex(char *label, u32 value) {
    char buf[56];
    char *s = buf + 16;
    str_copy(s, label);
    u32_to_hex_str(value, s + strlen(s));
}

void decal_init(void)
{
    s32 i;

    sTireTracks = mem_alloc_dbg(0xC00, 0, (s32)D_800D8B90, 0x67);
    sTireTrackVtx = mem_alloc_dbg(0x2000, 0, (s32)D_800D8B90, 0x68);
    sTireTrackNext = 0;
    mem_fill((u8 *)sTireTrackCur, 0, 0x20);
    mem_fill((u8 *)gTireTrackTimers, 0, 0x20);
    mem_fill((u8 *)sTireTrackLastY, 0, 0x20);
    mem_fill((u8 *)sTireTrackMudTimer, 0, 0x20);
    mem_fill((u8 *)sTireTracks, 0, 0xC00);
    mem_fill((u8 *)sTireTrackSurface, 0, 0x20);
    mem_fill((u8 *)sTireTrackMudSurface, 0, 0x20);
    for (i = 0; i < 0x80; i++) {
        sTireTracks[i].active = 0;
    }
    for (i = 0; i < 0x200; i += 4) {
        sTireTrackVtx[i].v.tc[0] = 0;
        sTireTrackVtx[i].v.tc[1] = 0;
        sTireTrackVtx[i + 1].v.tc[0] = 0x200;
        sTireTrackVtx[i + 1].v.tc[1] = 0;
        sTireTrackVtx[i + 2].v.tc[0] = 0;
        sTireTrackVtx[i + 2].v.tc[1] = 0;
        sTireTrackVtx[i + 3].v.tc[0] = 0x200;
        sTireTrackVtx[i + 3].v.tc[1] = 0;
    }
    sTireTrackTexture = mem_alloc_dbg(8, 0, (s32)D_800D8B90, 0x85);
    sTireTrackTexture[0] = 0xFF;
    sTireTrackTexture[1] = 0xDC;
    sTireTrackTexture[2] = 0x88;
    sTireTrackTexture[3] = 0x58;
    sTireTrackTexture[4] = 0x38;
    sTireTrackTexture[5] = 0x58;
    sTireTrackTexture[6] = 0x8C;
    sTireTrackTexture[7] = 0x1F;
}

typedef struct Seg_14 {
    u8 pad[0x28];
    s32 flags;
    u8 pad2[0x40 - 0x2C];
} Seg_14;

typedef struct Bike_35 {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x370 - 0x45];
    s32 f370[1];
    u8 pad374[0x458 - 0x374];
    s32 f458;
    u8 pad45C[0x494 - 0x45C];
    f32 x, y, z;          /* 0x494 */
    u8 pad4A0[0x4AC - 0x4A0];
    f32 f4AC;
    f32 f4B0;
    f32 f4B4;
    u8 pad4B8[0x4E4 - 0x4B8];
    Seg_14 *seg;             /* 0x4E4 */
    u8 pad4E8[0x4F0 - 0x4E8];
    f32 f4F0;
    u8 pad4F4[0x7A5 - 0x4F4];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_35;

extern Bike_35 gRiders_74[];
extern Seg_14 *gSurfaceTablePtr_2;

void decal_update_tire_tracks(void)
{
    s32 pos[3];
    s32 a[3];
    s32 b[3];
    s32 d[3];
    s32 m[10];
    s32 c[3];
    s32 e[3];
    s32 qr[4];
    s32 i, changed, area, surface, ox, oz, dx, dz, nx, nz, len, curArea;
    Bike_35 *p;
    Seg_14 *prev;
    TireTrack *r;
    s32 n;
    s32 l2;

    for (i = 0; i < 8; i++) {
        if (gRiders_74[i].f7A5 != 0) continue;
        p = &gRiders_74[i];
        if (sTireTrackMudTimer[i] != 0) sTireTrackMudTimer[i]--;
        if (gRiders_74[i].f4F0 != 0.0f) {
            gTireTrackTimers[i] = 1;
        } else if (gTireTrackTimers[i] != 0) {
            gTireTrackTimers[i]--;
        }
        if (!(p->seg->flags & 1)) gTireTrackTimers[i] = 0;
        prev = sTireTrackSurface[i];
        if (p->seg != prev) {
            changed = 1;
            if (prev != NULL && (prev->flags & 0x40) && p->f458 <= 0) {
                sTireTrackMudSurface[i] = prev;
                gTireTrackTimers[i] = 45;
                sTireTrackMudTimer[i] = 45;
            }
        } else {
            changed = 0;
        }
        sTireTrackSurface[i] = p->seg;
        if (p->f44 != 0) gTireTrackTimers[i] = 0;
        if (D_800EE140 != 0) gTireTrackTimers[i] = 0;
        if (gTireTrackTimers[i] == 0) {
            sTireTrackCur[i] = NULL;
            continue;
        }
        pos[0] = p->x * 1000.0f;
        pos[1] = p->y * 1000.0f;
        pos[2] = p->z * 1000.0f;
        quat_to_mtx3i(m, p->f370);
        d[0] = -p->f4B4 * 1000.0f;
        d[2] = p->f4AC * 1000.0f;
        len = isqrt_u32_2(d[0] * d[0] + d[2] * d[2]);
        if (len == 0) {
            d[1] = 0;
            d[0] = m[0];
            d[2] = m[2];
            len = isqrt_u32_2(m[0] * m[0] + m[2] * m[2]);
        }
        if (len != 0) {
            d[0] = d[0] * 50 / len;
            d[2] = d[2] * 50 / len;
        }
        a[0] = pos[0] - d[0];
        a[2] = pos[2] - d[2];
        b[0] = pos[0] + d[0];
        b[2] = pos[2] + d[2];
        a[1] = height_get_ground(a[0], pos[1], a[2]);
        b[1] = height_get_ground(b[0], pos[1], b[2]);
        if (gGroundFound == 0) continue;
        area = gGroundArea;
        if (gCurTrack_5->mode == 2) {
            divmod_floor(pos[0], 128000, &qr[0], &qr[1]);
            divmod_floor(pos[2], 128000, &qr[2], &qr[3]);
            area = (qr[0] << 16) | (u16)qr[2];
        }
        if (sTireTrackCur[i] != NULL && (u32)(a[1] - sTireTrackLastY[i] + 500) > 1000) {
            sTireTrackCur[i] = NULL;
            gTireTrackTimers[i] = 0;
            continue;
        }
        surface = p->seg - gSurfaceTablePtr_2;
        if (sTireTrackMudTimer[i] != 0) surface = (Seg_14 *)sTireTrackMudSurface[i] - gSurfaceTablePtr_2;
        if (sTireTrackCur[i] != NULL) {
            curArea = sTireTrackCur[i]->area;
            if (gCurTrack_5->mode == 1) {
                ox = (sHeightAreas->e[curArea].x0 + sHeightAreas->e[curArea].x1) / 2;
                oz = (sHeightAreas->e[curArea].z0 + sHeightAreas->e[curArea].z1) / 2;
            } else if (gCurTrack_5->mode == 2) {
                ox = (curArea >> 16) * 8000 + 4000;
                oz = (s16)curArea * 8000 + 4000;
            } else {
                ox = oz = 0;
            }
            r = sTireTrackCur[i];
            dx = pos[0] - r->x;
            dz = pos[2] - r->z;
            nx = r->dirX;
            nz = r->dirZ;
            l2 = isqrt_u32_2(dx * dx + dz * dz);
            if (l2 != 0) {
                dx = (dx << 14) / l2;
                dz = (dz << 14) / l2;
            }
            if ((nx * dx + nz * dz) >> 14 < 0x3F07 || area != sTireTrackCur[i]->area || changed) {
                Vtx *v;
                s16 *q, *q2;
                len = sTireTrackCur[i] - sTireTracks;
                v = (Vtx *)((len << 6) + (u32)sTireTrackVtx);
                q = v[2].v.ob;
                c[0] = q[0] + ox;
                c[1] = q[1];
                c[2] = q[2] + oz;
                q2 = v[3].v.ob;
                e[0] = q2[0] + ox;
                e[1] = q2[1];
                e[2] = q2[2] + oz;
                c[0] *= 16;
                c[1] *= 16;
                c[2] *= 16;
                e[0] *= 16;
                e[1] *= 16;
                e[2] *= 16;
                sTireTrackCur[i] = decal_add_quad(area, c, e, a, b, surface);
                sTireTrackLastY[i] = a[1];
            } else {
                sTireTrackCur[i]->x = pos[0];
                sTireTrackCur[i]->z = pos[2];
                {
                Vtx *v;
                len = sTireTrackCur[i] - sTireTracks;
                v = &sTireTrackVtx[len * 4];
                v[2].v.ob[0] = a[0] / 16 - ox;
                v[2].v.ob[1] = a[1] / 16;
                v[2].v.ob[2] = a[2] / 16 - oz;
                v[3].v.ob[0] = b[0] / 16 - ox;
                v[3].v.ob[1] = b[1] / 16;
                v[3].v.ob[2] = b[2] / 16 - oz;
                }
                sTireTrackLastY[i] = a[1];
            }
        } else {
            sTireTrackCur[i] = decal_add_quad(area, a, b, a, b, surface);
            sTireTrackLastY[i] = a[1];
        }
    }
}

typedef struct { s32 pad[11]; s32 mode; } Ctl_17;

extern Ctl_17 *gCurTrack_32;

/* Starts a quad from the near edge a0-a1 to the far edge b0-b1 (mm) and returns it. */
TireTrack *decal_add_quad(s32 area, s32 *a0, s32 *a1, s32 *b0, s32 *b1, s32 surface)
{
    s32 i, j, mode;
    s32 sx, sy;
    s32 cz, dx, dz;
    u32 r;
    Vtx *pt;
    Vtx *q;
    TireTrack *rec;
    TireTrack *r2;
    TireTrack *rb;

    for (i = 0; i < 4; i++) {
        if (&sTireTracks[sTireTrackNext] == sTireTrackCur[i]) sTireTrackCur[i] = 0;
    }

    mode = gCurTrack_32->mode;
    if (mode == 1) {
        sx = (sHeightAreas->e[area].x0 + sHeightAreas->e[area].x1) / 2;
        sy = (sHeightAreas->e[area].z0 + sHeightAreas->e[area].z1) / 2;
    } else if (mode == 2) {
        sx = (s16)(area >> 16) * 8000 + 4000;
        sy = (s16)area * 8000 + 4000;
    } else {
        sx = sy = 0;
    }

    j = sTireTrackNext * 4;
    pt = (Vtx *)((u32)(j * 16) + (u32)sTireTrackVtx);
    pt[0].v.ob[0] = (a0[0] >> 4) - sx;
    pt[0].v.ob[1] = a0[1] >> 4;
    pt[0].v.ob[2] = (a0[2] >> 4) - sy;
    pt[1].v.ob[0] = (a1[0] >> 4) - sx;
    q = &pt[1];
    q->v.ob[1] = a1[1] >> 4;
    q->v.ob[2] = (a1[2] >> 4) - sy;
    pt[2].v.ob[0] = (b0[0] >> 4) - sx;
    q = &pt[2];
    q->v.ob[1] = b0[1] >> 4;
    q->v.ob[2] = (b0[2] >> 4) - sy;
    pt[3].v.ob[0] = (b1[0] >> 4) - sx;
    pt = &pt[3];
    pt->v.ob[1] = b1[1] >> 4;
    pt->v.ob[2] = (b1[2] >> 4) - sy;

    j = j / 4;
    rb = sTireTracks;
    rec = (TireTrack *)((s32)(j * 24) + (s32)rb);
    rec->x = (b0[0] + b1[0]) / 2;
    rec->z = cz = (b0[2] + b1[2]) / 2;
    dx = rec->x - (a0[0] + a1[0]) / 2;
    dz = cz - (a0[2] + a1[2]) / 2;
    r = isqrt_u32_3(dx * dx + dz * dz);
    if (r != 0) {
        dx = (dx * 0x4000) / (s32)r;
        dz = (dz * 0x4000) / (s32)r;
    }

    r2 = (TireTrack *)((u32)(j * 24) + (u32)sTireTracks);
    r2->active = 1;
    r2->dirX = dx;
    r2->dirZ = dz;
    r2->area = area;
    sTireTracks[j].surface = surface;

    sTireTrackNext = (sTireTrackNext + 1) % 128;
    return &sTireTracks[j];
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 pad[0x34]; u8 r, g, b; u8 pad2[9]; } Col_2;

extern Col_2 *gSurfaceTablePtr_3;

Gfx *decal_draw(Gfx *gdl, s32 arg1, Mtx *arg2, s32 arg3) {
    s32 i, j, k, m, a, idx, started, v, hi, lo;
    Col_2 *c;

    if (D_800EE140 != 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    RAW(gdl++, 0xE200001C, 0x00404A50);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    gDPLoadTextureBlock(gdl++, sTireTrackTexture, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                        G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if (gCurTrack_5->mode == 2) {
        for (i = 0; i < sDesBlockCount; i++) {
            started = 0;
            for (j = 0; j < 128; j++) {
                if (!sTireTracks[j].active) continue;
                v = sTireTracks[j].area;
                hi = v >> 16;
                lo = (s16)v;
                if (hi == D_800ED6A8[i] && lo == D_800ED728[i]) {
                    k = j + 1;
                    if (!started) {
                        gSPMatrix(gdl++, &sDesBlockMtx[arg1][i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        started = 1;
                    }
                    for (; k < 128; k++) {
                        if (k >= j + 8) break;
                        if (!sTireTracks[k].active) break;
                        v = sTireTracks[k].area;
                        hi = v >> 16;
                        lo = (s16)v;
                        if (hi != D_800ED6A8[i] || lo != D_800ED728[i]) break;
                    }
                    k -= j;
                    gSPVertex(gdl++, &sTireTrackVtx[j * 4], k * 4, 0);
                    for (m = 0; m < k; m++) {
                        idx = m + j;
                        a = idx - sTireTrackNext;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks[idx].surface].r, gSurfaceTablePtr_3[sTireTracks[idx].surface].g, gSurfaceTablePtr_3[sTireTracks[idx].surface].b, a);
                            gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                        }
                    }
                    j += k - 1;
                }
            }
        }
    } else if (gCurTrack_5->mode == 1) {
        TrackModelList *l = sHeightAreas;
        u32 bit;
        {
            for (i = 0; i < sHeightAreas->count; i++) {
                if (l->e[i].mask & (1 << (arg3 * 2))) {
                started = 0;
                for (j = 0; j < 128; j++) {
                    if (sTireTracks[j].active && sTireTracks[j].area == i) {
                        k = j + 1;
                        if (!started) {
                            gSPMatrix(gdl++, gSectionMtxs[i * 4 + arg3], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                            started = 1;
                        }
                        for (; k < 128; k++) {
                            if (k >= j + 8) break;
                            if (!sTireTracks[k].active) break;
                            if (sTireTracks[k].area != i) break;
                        }
                        k -= j;
                        gSPVertex(gdl++, &sTireTrackVtx[j * 4], k * 4, 0);
                        for (m = 0; m < k; m++) {
                            idx = m + j;
                            a = idx - sTireTrackNext;
                            if (a < 0) a += 128;
                            if (a >= 16) {
                                gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks[idx].surface].r, gSurfaceTablePtr_3[sTireTracks[idx].surface].g, gSurfaceTablePtr_3[sTireTracks[idx].surface].b, a);
                                gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                            }
                        }
                        j += k - 1;
                    }
                }
                }
            }
        }
    } else {
        for (i = 0; i < 128; i += 8) {
            gSPVertex(gdl++, &sTireTrackVtx[i * 4], 32, 0);
            for (j = 0; j < 8; j++) {
                idx = i + j;
                if (sTireTracks[idx].active) {
                    if (sHeightAreas->e[sTireTracks[idx].area].mask & (1 << (arg3 * 2))) {
                        a = idx - sTireTrackNext;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks[idx].surface].r, gSurfaceTablePtr_3[sTireTracks[idx].surface].g, gSurfaceTablePtr_3[sTireTracks[idx].surface].b, a);
                            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 2, 0, j * 4 + 2, j * 4 + 1, j * 4 + 3, 0);
                        }
                    }
                }
            }
        }
    }
    gSPMatrix(gdl++, arg2 + 16, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    return gdl;
}

void decal_free(void)
{
    mem_free(sTireTrackTexture);
    mem_free(sTireTrackVtx);
    mem_free(sTireTracks);
}

void decal_end_track(s32 rider)
{
    sTireTrackCur[rider] = 0;
}

void decal_clear_all(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        sTireTrackCur[i] = 0;
        gTireTrackTimers[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        sTireTracks[i].active = 0;
    }
}
