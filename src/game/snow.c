#include "game.h"

typedef struct {
    u32 pad0;
    s32 type;
    u32 flags;
    s32 index;
    s32 count;
    u16 *list;
} SubEnt;

extern SubEnt D_800F21B8[];

void func_80071990(void)
{
    s32 i;

    if (D_800EFD64_2 - D_800CB210 == 0x13 && D_800C2594 == 1) {
        D_800CCFFC = func_800BC908(0xE00, 0, (s32)D_800D9190, 0x6B);
        D_800F219C = func_800BF79C(D_800E7728, D_800D1000);
        for (i = 0; i < 0x80; i++) {
            func_800729D0(&D_800CCFFC[i]);
        }
        D_800CCFF8 = 0;
    }
}

typedef struct { s32 x, y, z, pad[3]; s32 on; } Star;
typedef struct { u8 pad[8]; u8 w, h; u8 pad2[2]; void *img2; u8 pad3[4]; void *img; } TexInfo_3;

extern TexInfo_3 *D_800F219C_2;

Gfx *func_80071A7C(Gfx *gdl, s32 mtx, s32 arg2)
{
    Star *p;
    s32 i, cx, cy, s, x, y, z, a, r;

    p = D_800CCFFC;
    if (p == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gSPSetOtherMode(gdl++, G_SETOTHERMODE_L, G_MDSFT_RENDERMODE, 29, 0x404240);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2DFEFF);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    { Gfx *g = gdl++; g->words.w0 = 0xE3000C00; g->words.w1 = ((u32)g & 1) >> 1; }
    { Gfx *g = gdl++; g->words.w0 = 0xE3001001; g->words.w1 = ((u32)g & 1) >> 1; }
    {
        s32 w = D_800F13EC;
        s32 h = D_800F13F0;
        cx = w / 2;
        cy = h / 2;
    }
    gDPLoadTextureBlock(gdl++, D_800F219C_2->img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < 0x80; i++) {
        if (p[i].on != 0) {
            s = D_800C2C60[(((D_800E76D0 & 0xFFFFFF) * 256 + i * 512) >> 4) & 0xFFF] / 2;
            z = p[i].z;
            x = p[i].x + s;
            x = cx + x * cx / z;
            y = p[i].y + s;
            y = cy - y * cy / z;
            a = 0xFF - (z >> 8);
            if (a < 0x20) {
                a = 0x20;
            }
            r = 0x80 / (z >> 8);
            r = (r >> 1) + 1;
            if (r > 4) {
                r = 4;
            }
            gDPPipeSync(gdl++);
            gDPSetColor(gdl++, G_SETPRIMCOLOR, a | 0xFFFFFF00);
            gSPTextureRectangle(gdl++, (x - r) << 2, (y - r) << 2, (x + r) << 2, (y + r) << 2,
                                G_TX_RENDERTILE, 0, 0, D_800CD000[r].b, D_800CD000[r].b);
        }
    }
    if (D_800CCFF8 != 0) {
        gDPSetColor(gdl++, G_SETPRIMCOLOR, (D_800CCFF8 * 255 / 60) | 0xFFFFFF00);
        gDPLoadTextureBlock(gdl++, D_800F219C_2->img2, G_IM_FMT_IA, G_IM_SIZ_8b,
                            D_800F219C_2->w * 8, D_800F219C_2->h, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 8, 8, G_TX_NOLOD, G_TX_NOLOD);
        gDPPipeSync(gdl++);
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
        gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPVertex(gdl++, D_800CD018, 4, 0);
        gSP2Triangles(gdl++, 0, 2, 1, 0, 0, 3, 2, 0);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    s32 x, y, z;
    s32 pad[2];
    s32 f14;
    s32 f18;
} Part_6;

extern Part_6 *D_800CCFFC_2;

static inline void respawn(Part_6 *p, s32 k) {
    if (D_800E7FF0 & 0x40) {
        p->f18 = 0;
    } else {
        p->f18 = 1;
    }
    p->z = func_8000EFE8() * 192 + 0x4000;
    p->x = ((s32)func_8000EFE8() - 128) * p->z / 128;
    if (func_8000EFE8() & 1) {
        p->y = ((s32)func_8000EFE8() - 128) * p->z / 256;
    } else {
        p->y = p->z;
    }
    p->f14 = func_8000EFE8() + 0x100;
}

void func_80072114(void) {
    s32 dx, dz, ang, s, c;
    s32 mx, mz, rot, dy, ny;
    s32 i;
    s32 x, y, z, sx, sy, hw, hh;
    s32 a, t3, t2;
    s32 w, h;

    if (D_800CCFFC_2 == NULL || D_800E3710 != 0 || (D_800C2358 != 0 && D_800E1B70 == 0x100)) {
        return;
    }
    dz = D_800E8090_2.z - D_800F2190.z;
    ang = -D_800E8000_2 >> 4;
    s = D_800C2C60[ang & 0xFFF];
    c = D_800C2C60[(ang - 0x400) & 0xFFF];
    dx = D_800E8090_2.x - D_800F2190.x;
    D_800CCFF0 = -1;
    dy = D_800E8090_2.y - D_800F2190.y;
    mz = (s * dz + c * dx) >> 14;
    mx = (c * dz - s * dx) >> 14;
    rot = (D_800E8000_2 - D_800F21A0) / 64;
    i = 0;
    ny = -dy * 3;
    D_800F2190.x = D_800E8090_2.x;
    D_800F2190.y = D_800E8090_2.y;
    D_800F2190.z = D_800E8090_2.z;
    D_800F21A0 = D_800E8000_2;
    h = D_800F13F0;
    for (; i < 128; i++) {
        { s32 t = D_800CCFFC_2[i].z; D_800CCFFC_2[i].z = t - mz * 3; }
        D_800CCFFC_2[i].y += ny - D_800CCFFC_2[i].f14;
        { s32 t = D_800CCFFC_2[i].x; D_800CCFFC_2[i].x = t + mx * 3; }
        D_800CCFFC_2[i].x += rot * D_800CCFFC_2[i].z / 256;
        z = D_800CCFFC_2[i].z;
        if (z < 0x1000) {
            if (D_800CCFFC_2[i].x < z && -z < D_800CCFFC_2[i].x) {
                if (D_800CCFFC_2[i].y < z && -z < D_800CCFFC_2[i].y && z > 0 && D_800CCFFC_2[i].f18 != 0) {
                    hw = D_800F13EC / 2;
                    sx = hw + D_800CCFFC_2[i].x * hw / z;
                    hh = D_800F13F0 / 2;
                    sy = hh - D_800CCFFC_2[i].y * hh / z;
                    if (sx >= 20 && sx < D_800F13EC - 20 && sy >= 20 && sy < D_800F13F0 - 20) {
                        D_800CCFF0 = sx;
                        D_800CCFF4 = sy;
                    }
                }
            }
            respawn(&D_800CCFFC_2[i], i);
        } else if (D_800CCFFC_2[i].y < -z) {
            respawn(&D_800CCFFC_2[i], i);
        } else if (D_800CCFFC_2[i].x < -z) {
            respawn(&D_800CCFFC_2[i], i);
        } else if (z < D_800CCFFC_2[i].x) {
            respawn(&D_800CCFFC_2[i], i);
        } else if (z > 0x10000) {
            respawn(&D_800CCFFC_2[i], i);
        }
    }
    if (D_800CCFF8 != 0) {
        D_800CCFF8--;
    }
    func_8000EFE8();
    func_8000EFE8();
    if (D_800CCFF8 == 0 && D_800CCFF0 != -1) {
        i = func_8000EFE8() << 5;
        t2 = D_800C2C60[(i >> 4) & 0xFFF] >> 10;
        t3 = D_800C2C60[((i >> 4) - 0x400) & 0xFFF] >> 10;
        x = D_800CCFF0;
        y = D_800CCFF4;
        D_800CCFF8 = 60;
        D_800CD018[0].v.ob[0] = (x + t3 - 160) * D_800F13EC / 320;
        D_800CD018[0].v.ob[1] = (y + t2 - 120) * D_800F13F0 / 240;
        D_800CD018[1].v.ob[0] = (x + t2 - 160) * D_800F13EC / 320;
        D_800CD018[1].v.ob[1] = (y - t3 - 120) * D_800F13F0 / 240;
        D_800CD018[2].v.ob[0] = (x - t3 - 160) * D_800F13EC / 320;
        D_800CD018[2].v.ob[1] = (y - t2 - 120) * D_800F13F0 / 240;
        D_800CD018[3].v.ob[0] = (x - t2 - 160) * D_800F13EC / 320;
        D_800CD018[3].v.ob[1] = (y + t3 - 120) * D_800F13F0 / 240;
    }
}

void func_800729D0(s32 *param_1)
{
    s32 x;
    u32 u;

    if (D_800E7FF0 & 0x40) {
        param_1[6] = 0;
    } else {
        param_1[6] = 1;
    }
    u = func_8000EFE8();
    param_1[2] = u * 0xc0 + 0x4000;
    u = func_8000EFE8();
    x = ((s32)u - 0x80) * param_1[2];
    if (x < 0) {
        x += 0x7f;
    }
    param_1[0] = x >> 7;
    u = func_8000EFE8();
    if (u & 1) {
        u = func_8000EFE8();
        x = ((s32)u - 0x80) * param_1[2];
        if (x < 0) {
            x += 0xff;
        }
        x = x >> 8;
    } else {
        x = param_1[2];
    }
    param_1[1] = x;
    u = func_8000EFE8();
    param_1[5] = u + 0x100;
}

void func_80072A9C(void)
{
    if (D_800CCFFC != 0) {
        func_800BCB04(D_800CCFFC);
        D_800CCFFC = 0;
    }
}

typedef struct {
    u32 flags;
    u8 pad[0x1C];
} Rec_45;

struct Mdl2 {
    u32 n;
    u8 pad[0x1C];
    Rec_45 r[1];
};

struct Obj2 {
    u8 pad[0x18];
    s32 n;
};

void func_80072AD0(Mdl2 *m, Obj2 *o)
{
    s32 i, j, k;
    SubEnt *e;

    D_800F21B0 = 0;
    for (i = 0; i < m->n; i++) {
        if (m->r[i].flags & 6) {
            D_800F21B8[D_800F21B0].index = i;
            D_800F21B8[D_800F21B0].flags = m->r[i].flags;
            D_800F21B8[D_800F21B0++].type = (m->r[i].flags >> 1) & 3;
        }
    }
    if (D_800F21B0 == 0) return;
    for (i = 0; i < D_800F21B0; i++) {
        D_800F21B8[i].count = 0;
    }
    for (k = 0; k < o->n; k++) {
        i = (D_800C9800[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < D_800F21B0; j++) {
                if (D_800F21B8[j].type == i) break;
            }
            D_800F21B8[j].count++;
        }
    }
    for (i = 0; i < D_800F21B0; i++) {
        if (D_800F21B8[i].count) {
            D_800F21B8[i].list = func_800BC908(D_800F21B8[i].count * 2, 0, (s32)D_800D91A0, 0x5D);
            D_800F21B8[i].count = 0;
        }
    }
    for (k = 0; k < o->n; k++) {
        i = (D_800C9800[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < D_800F21B0; j++) {
                if (D_800F21B8[j].type == i) break;
            }
            D_800F21B8[j].list[D_800F21B8[j].count++] = k;
        }
    }
}

typedef struct {
    s32 f0;
    s32 id;
    s32 f8;
    s32 idx;
    s32 n;
    u16 *tab;
} Ent_81;

extern Ent_81 D_800F21B8_2[];

void func_80072D64(s32 id)
{
    s32 i;
    s32 k;

    for (i = 0; i < D_800F21B0; i++) {
        if (D_800F21B8_2[i].id == id) {
            break;
        }
    }
    if (i != D_800F21B0) {
        for (k = 0; k < D_800F21B8_2[i].n; k++) {
            D_800C9800[D_800F21B8_2[i].tab[k]] |= 0x20;
        }
        k = D_800F21B8_2[i].idx;
        if (D_800E7834[k] & 1) {
            func_80067818(k);
        } else {
            D_800E7834[k] |= 8;
        }
    }
}

typedef struct {
    s32 pad[4];
    s32 f10;
    s32 f14;
} Ent_82;

extern Ent_82 D_800F21B8_3[];

void func_80072E7C(void)
{
    s32 i = 0;
    if (0 < D_800F21B0) {
        do {
            if (D_800F21B8_3[i].f10 != 0) {
                func_800BCB04(D_800F21B8_3[i].f14);
            }
            i++;
        } while (D_800F21B0 > i);
    }
}

typedef struct {
    u8 pad[0x10];
    s32 n;
    u16 *idx;
} Ent_83;

void func_80072EF0(void) {
    s32 i;
    s32 j;
    for (i = 0; i < D_800F21B0; i++) {
        Ent_83 *e = &D_800F21B8[i];
        for (j = 0; j < e->n; j++) {
            u8 *b = D_800C9800 + e->idx[j];
            *b &= 0xDF;
        }
    }
}

typedef struct {
    s32 flags;
    u8 pad4[0x50];
    s32 mode;
    s16 f58;
} State_6;

extern State_6 D_800EFD70_52;

Gfx *func_80072F70(Gfx *gdl, Dyn *arg1, s32 arg2)
{
    s32 a;
    s32 i;
    s32 c;
    s32 w;
    s32 x;

    if (D_800EFD70_52.flags & 0x10000000) {
        a = 0xFF;
    } else {
        if (D_800EFD70_52.mode == 1) {
            return gdl;
        }
        if (D_800EFD70_52.mode == 0) {
            goto m0;
        }
        if (D_800EFD70_52.mode == 2) {
            goto m2;
        }
        goto body;
    m0:
        a = 0xFF - D_800EFD70_52.f58;
        goto body;
    m2:
        a = D_800EFD70_52.f58;
    }
body:
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPViewport(gdl++, &D_800C25E8);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
    for (i = 0; i < 64; i++) {
        c = a * 2 + i * 8 - 0x100;
        if (c > 0xFF) {
            c = 0xFF;
        }
        if (c < 0) {
            c = 0;
        }
        w = D_800F13EC / 64;
        x = i * D_800F13EC / 64;
        if (D_800EFD70_52.mode == 2) {
            x = D_800F13EC - w - x;
        }
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, c);
        gDPFillRectangle(gdl++, x, 0, x + w, D_800F13F0);
    }
    return gdl;
}
