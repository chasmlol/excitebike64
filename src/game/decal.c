#include "game.h"

typedef struct {
    u8 pad[8];
    s16 a;
    s16 b;
    u8 pad2[4];
} E16;

extern E16 *sTireTrackVtx;

typedef struct {
    u8 pad0[0x20];
    f32 f20[1];
} Ply;

typedef struct {
    u8 pad0[0x410];
    s32 f410;
} Gm;

extern Ply *gCurTrack_31;
extern Gm gGame_44;

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
            debug_ftoa(buf, gRiders[i].timeScale / gCurTrack_31->f20[gGame_44.f410 - 1], 4);
            debug_print_float(buf, gRiders[i].unk_450);
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

void debug_print_float(char *param_1, f32 param_2)
{
    char buf[40];
    s32 len;
    str_copy(buf, param_1);
    len = strlen(buf);
    debug_ftoa(buf + len, param_2, 3);
}

void debug_print_float2(char *param_1, f32 param_2, f32 param_3)
{
    char buf[40];
    s32 n;
    s32 i;

    mem_fill((u8 *)buf, 0x20, 0x1E);
    str_copy(buf, param_1);
    n = strlen(buf);
    debug_ftoa(buf + n, param_2, 3);
    i = 0;
    if (0 < n + 6) {
        do {
            if (buf[i] == 0) {
                buf[i] = ' ';
            }
            i++;
        } while (i < n + 6);
    }
    debug_ftoa(buf + 6 + n, param_3, 3);
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

void debug_print_int(char *param_1, s32 param_2) {
    char pad[16];
    char buf[40];
    char *b;

    b = buf;
    str_copy(b, param_1);
    int_to_str(param_2, b + strlen(b));
}

void debug_print_hex(char *param_1, u32 param_2) {
    char buf[56];
    char *s = buf + 16;
    str_copy(s, param_1);
    u32_to_hex_str(param_2, s + strlen(s));
}

void decal_init(void)
{
    s32 i;

    sTireTracks = mem_alloc_dbg(0xC00, 0, (s32)D_800D8B90, 0x67);
    sTireTrackVtx = mem_alloc_dbg(0x2000, 0, (s32)D_800D8B90, 0x68);
    sTireTrackNext = 0;
    mem_fill(sTireTrackCur, 0, 0x20);
    mem_fill(gTireTrackTimers, 0, 0x20);
    mem_fill(sTireTrackLastY, 0, 0x20);
    mem_fill(sTireTrackMudTimer, 0, 0x20);
    mem_fill(sTireTracks, 0, 0xC00);
    mem_fill(sTireTrackSurface, 0, 0x20);
    mem_fill(sTireTrackMudSurface, 0, 0x20);
    for (i = 0; i < 0x80; i++) {
        sTireTracks[i * 0x18] = 0;
    }
    for (i = 0; i < 0x200; i += 4) {
        sTireTrackVtx[i].a = 0;
        sTireTrackVtx[i].b = 0;
        sTireTrackVtx[i + 1].a = 0x200;
        sTireTrackVtx[i + 1].b = 0;
        sTireTrackVtx[i + 2].a = 0;
        sTireTrackVtx[i + 2].b = 0;
        sTireTrackVtx[i + 3].a = 0x200;
        sTireTrackVtx[i + 3].b = 0;
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

typedef struct Rec_32 {
    s32 f0;
    s32 key;
    s32 x, z;
    s32 nx, nz;
} Rec_32;

typedef struct Seg_14 {
    u8 pad[0x28];
    s32 flags;
    u8 pad2[0x40 - 0x2C];
} Seg_14;

typedef struct Gate_2 {
    u8 pad[0x20];
    s16 a[3];
    u8 pad26[0x30 - 0x26];
    s16 b[3];
    u8 pad36[0x40 - 0x36];
} Gate_2;

typedef struct Cell_4 {
    s32 f0, f4;
    s32 x0, z0, x1, z1;
    u8 pad[0x20 - 0x18];
} Cell_4;

extern Cell_4 *sHeightAreas_5;
extern Seg_14 *gSurfaceTablePtr_2;
extern Rec_32 *sTireTracks_2;
extern Rec_32 *sTireTrackCur_2[];
extern Seg_14 *sTireTrackSurface_2[];
extern Gate_2 *sTireTrackVtx_2;
extern Seg_14 *sTireTrackMudSurface_2[];

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
    s32 i, chg, key, idx, ox, oz, dx, dz, nx, nz, len, k;
    Rider *p;
    Seg_14 *t;
    Rec_32 *r;
    s32 n;
    s32 l2;
    Cell_4 *ce;

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive != 0) continue;
        p = &gRiders[i];
        if (sTireTrackMudTimer_2[i] != 0) sTireTrackMudTimer_2[i]--;
        if (gRiders[i].rearWheel.slip != 0.0f) {
            gTireTrackTimers[i] = 1;
        } else if (gTireTrackTimers[i] != 0) {
            gTireTrackTimers[i]--;
        }
        if (!(p->rearWheel.surface->flags & 1)) gTireTrackTimers[i] = 0;
        t = sTireTrackSurface_2[i];
        if (p->rearWheel.surface != t) {
            chg = 1;
            if (t != NULL && (t->flags & 0x40) && p->rearWheel.airTicks <= 0) {
                sTireTrackMudSurface_2[i] = t;
                gTireTrackTimers[i] = 45;
                sTireTrackMudTimer_2[i] = 45;
            }
        } else {
            chg = 0;
        }
        sTireTrackSurface_2[i] = p->rearWheel.surface;
        if (p->crashState != 0) gTireTrackTimers[i] = 0;
        if (D_800EE140 != 0) gTireTrackTimers[i] = 0;
        if (gTireTrackTimers[i] == 0) {
            sTireTrackCur_2[i] = NULL;
            continue;
        }
        pos[0] = p->rearWheel.contact.x * 1000.0f;
        pos[1] = p->rearWheel.contact.y * 1000.0f;
        pos[2] = p->rearWheel.contact.z * 1000.0f;
        quat_to_mtx3i(m, p->drawQuat);
        d[0] = -p->rearWheel.contactDelta.z * 1000.0f;
        d[2] = p->rearWheel.contactDelta.x * 1000.0f;
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
        key = gGroundArea;
        if (gCurTrack_5->mode == 2) {
            divmod_floor(pos[0], 128000, &qr[0], &qr[1]);
            divmod_floor(pos[2], 128000, &qr[2], &qr[3]);
            key = (qr[0] << 16) | (u16)qr[2];
        }
        if (sTireTrackCur_2[i] != NULL && (u32)(a[1] - sTireTrackLastY_2[i] + 500) > 1000) {
            sTireTrackCur_2[i] = NULL;
            gTireTrackTimers[i] = 0;
            continue;
        }
        idx = (Seg_14 *)p->rearWheel.surface - gSurfaceTablePtr_2;
        if (sTireTrackMudTimer_2[i] != 0) idx = sTireTrackMudSurface_2[i] - gSurfaceTablePtr_2;
        if (sTireTrackCur_2[i] != NULL) {
            k = sTireTrackCur_2[i]->key;
            if (gCurTrack_5->mode == 1) {
                ce = &sHeightAreas_5[k];
                ox = (ce->x0 + ce->x1) / 2;
                oz = (ce->z0 + ce->z1) / 2;
            } else if (gCurTrack_5->mode == 2) {
                ox = (k >> 16) * 8000 + 4000;
                oz = (s16)k * 8000 + 4000;
            } else {
                ox = oz = 0;
            }
            r = sTireTrackCur_2[i];
            dx = pos[0] - r->x;
            dz = pos[2] - r->z;
            nx = r->nx;
            nz = r->nz;
            l2 = isqrt_u32_2(dx * dx + dz * dz);
            if (l2 != 0) {
                dx = (dx << 14) / l2;
                dz = (dz << 14) / l2;
            }
            if ((nx * dx + nz * dz) >> 14 < 0x3F07 || key != sTireTrackCur_2[i]->key || chg) {
                Gate_2 *g;
                s16 *q, *q2;
                len = sTireTrackCur_2[i] - sTireTracks_2;
                g = (Gate_2 *)((len << 6) + (u32)sTireTrackVtx_2);
                q = g->a;
                c[0] = q[0] + ox;
                c[1] = q[1];
                c[2] = q[2] + oz;
                q2 = g->b;
                e[0] = q2[0] + ox;
                e[1] = q2[1];
                e[2] = q2[2] + oz;
                c[0] *= 16;
                c[1] *= 16;
                c[2] *= 16;
                e[0] *= 16;
                e[1] *= 16;
                e[2] *= 16;
                sTireTrackCur_2[i] = decal_add_quad(key, c, e, a, b, idx);
                sTireTrackLastY_2[i] = a[1];
            } else {
                sTireTrackCur_2[i]->x = pos[0];
                sTireTrackCur_2[i]->z = pos[2];
                {
                Gate_2 *g;
                len = sTireTrackCur_2[i] - sTireTracks_2;
                g = &sTireTrackVtx_2[len];
                g->a[0] = a[0] / 16 - ox;
                g->a[1] = a[1] / 16;
                g->a[2] = a[2] / 16 - oz;
                g->b[0] = b[0] / 16 - ox;
                g->b[1] = b[1] / 16;
                g->b[2] = b[2] / 16 - oz;
                }
                sTireTrackLastY_2[i] = a[1];
            }
        } else {
            sTireTrackCur_2[i] = decal_add_quad(key, a, b, a, b, idx);
            sTireTrackLastY_2[i] = a[1];
        }
    }
}

struct Rec_33 {
    u8 flag;
    u8 p6;
    u16 pad;
    s32 p1;
    s32 cx;
    s32 cz;
    s32 dx;
    s32 dz;
};

typedef struct { s32 f0, f4, a, b, c, d; s32 pad[2]; } Box_2;
typedef struct { s32 pad[11]; s32 mode; } Ctl_17;

extern Ctl_17 *gCurTrack_32;
extern Box_2 *sHeightAreas_6;

Rec_33 *decal_add_quad(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6)
{
    s32 i, j, mode;
    s32 sx, sy;
    s32 cz, dx, dz;
    u32 r;
    Box_2 *b;
    Vtx16 *pt;
    Vtx16 *q;
    Rec_33 *rec;
    Rec_33 *r2;
    Rec_33 *rb;

    for (i = 0; i < 4; i++) {
        if (&sTireTracks_3[sTireTrackNext] == sTireTrackCur[i]) sTireTrackCur[i] = 0;
    }

    mode = gCurTrack_32->mode;
    if (mode == 1) {
        b = &sHeightAreas_6[p1];
        sx = (b->a + b->c) / 2;
        sy = (b->b + b->d) / 2;
    } else if (mode == 2) {
        sx = (s16)(p1 >> 16) * 8000 + 4000;
        sy = (s16)p1 * 8000 + 4000;
    } else {
        sx = sy = 0;
    }

    j = sTireTrackNext * 4;
    pt = (Vtx16 *)((u32)(j * 16) + (u32)sTireTrackVtx);
    pt[0].x = (p2[0] >> 4) - sx;
    pt[0].y = p2[1] >> 4;
    pt[0].z = (p2[2] >> 4) - sy;
    pt[1].x = (p3[0] >> 4) - sx;
    q = &pt[1];
    q->y = p3[1] >> 4;
    q->z = (p3[2] >> 4) - sy;
    pt[2].x = (p4[0] >> 4) - sx;
    q = &pt[2];
    q->y = p4[1] >> 4;
    q->z = (p4[2] >> 4) - sy;
    pt[3].x = (p5[0] >> 4) - sx;
    pt = &pt[3];
    pt->y = p5[1] >> 4;
    pt->z = (p5[2] >> 4) - sy;

    j = j / 4;
    rb = sTireTracks_3;
    rec = (Rec_33 *)((s32)(j * 24) + (s32)rb);
    rec->cx = (p4[0] + p5[0]) / 2;
    rec->cz = cz = (p4[2] + p5[2]) / 2;
    dx = rec->cx - (p2[0] + p3[0]) / 2;
    dz = cz - (p2[2] + p3[2]) / 2;
    r = isqrt_u32_3(dx * dx + dz * dz);
    if (r != 0) {
        dx = (dx * 0x4000) / (s32)r;
        dz = (dz * 0x4000) / (s32)r;
    }

    r2 = (Rec_33 *)((u32)(j * 24) + (u32)sTireTracks_3);
    r2->flag = 1;
    r2->dx = dx;
    r2->dz = dz;
    r2->p1 = p1;
    sTireTracks_3[j].p6 = p6;

    sTireTrackNext = (sTireTrackNext + 1) % 128;
    return &sTireTracks_3[j];
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 active; u8 col; u8 pad[2]; s32 id; u8 rest[0x10]; } Ent_59;
typedef struct { u8 pad[0x34]; u8 r, g, b; u8 pad2[9]; } Col_2;
typedef struct { u32 count; u8 pad[0x1C]; Obj obj[1]; } ObjList;

extern Ent_59 *sTireTracks_4;
extern Col_2 *gSurfaceTablePtr_3;
extern ObjList *sHeightAreas_7;

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
                if (!sTireTracks_4[j].active) continue;
                v = sTireTracks_4[j].id;
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
                        if (!sTireTracks_4[k].active) break;
                        v = sTireTracks_4[k].id;
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
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks_4[idx].col].r, gSurfaceTablePtr_3[sTireTracks_4[idx].col].g, gSurfaceTablePtr_3[sTireTracks_4[idx].col].b, a);
                            gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                        }
                    }
                    j += k - 1;
                }
            }
        }
    } else if (gCurTrack_5->mode == 1) {
        ObjList *l = sHeightAreas_7;
        u32 bit;
        {
            for (i = 0; i < sHeightAreas_7->count; i++) {
                if (l->obj[i].mask & (1 << (arg3 * 2))) {
                started = 0;
                for (j = 0; j < 128; j++) {
                    if (sTireTracks_4[j].active && sTireTracks_4[j].id == i) {
                        k = j + 1;
                        if (!started) {
                            gSPMatrix(gdl++, gSectionMtxs[i * 4 + arg3], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                            started = 1;
                        }
                        for (; k < 128; k++) {
                            if (k >= j + 8) break;
                            if (!sTireTracks_4[k].active) break;
                            if (sTireTracks_4[k].id != i) break;
                        }
                        k -= j;
                        gSPVertex(gdl++, &sTireTrackVtx[j * 4], k * 4, 0);
                        for (m = 0; m < k; m++) {
                            idx = m + j;
                            a = idx - sTireTrackNext;
                            if (a < 0) a += 128;
                            if (a >= 16) {
                                gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks_4[idx].col].r, gSurfaceTablePtr_3[sTireTracks_4[idx].col].g, gSurfaceTablePtr_3[sTireTracks_4[idx].col].b, a);
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
                if (sTireTracks_4[idx].active) {
                    if (sHeightAreas_7->obj[sTireTracks_4[idx].id].mask & (1 << (arg3 * 2))) {
                        a = idx - sTireTrackNext;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr_3[sTireTracks_4[idx].col].r, gSurfaceTablePtr_3[sTireTracks_4[idx].col].g, gSurfaceTablePtr_3[sTireTracks_4[idx].col].b, a);
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

void decal_end_track(s32 a0)
{
    sTireTrackCur[a0] = 0;
}

void decal_clear_all(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        sTireTrackCur[i] = 0;
        gTireTrackTimers[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        sTireTracks[i * 0x18] = 0;
    }
}
