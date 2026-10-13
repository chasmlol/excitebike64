#include "game.h"

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x6E8 - 0x45];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Racer;

typedef struct {
    u8 unk0[0x21C];
    u8 r;
    u8 g;
    u8 b;
    u8 unk21F[0xA80 - 0x21F];
} Player;

typedef struct Rec_40 {
    u8 *buf;
    s32 val;
    Obj_51 *obj;
    u16 *src;
    u8 *dst;
} Rec_40;

typedef struct {
    u16 count;
    u16 best;
    s16 v[3][3];
    s16 x3, z3;
} Rec_41;

extern Racer D_800E37C0_2[];
extern Player D_800DC150[];
extern Rec_40 *D_800F14D0;
extern Rec_41 *D_800F14F0;

void func_80067B80(void)
{
    s32 i;
    Spot *p;
    s32 n;
    s32 wA;
    s32 hA;
    s32 wB;
    s32 hB;
    s32 k;
    s32 x, y;

    D_800F1458 = 0;
    switch (D_800EFDBC) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 13:
    case 14:
        break;
    default:
        if ((D_800EFD70 & 0x8000) == 0) {
            for (i = 0; i < D_800EE490; i++) {
                if (D_800EE498_2[i].x < D_800F1468.x || i == 0) {
                    D_800F1468.x = D_800EE498_2[i].x;
                }
                if (D_800EE498_2[i].z < D_800F1468.z || i == 0) {
                    D_800F1468.z = D_800EE498_2[i].z;
                }
                if (D_800EE498_2[i].x > D_800F1470.x || i == 0) {
                    D_800F1470.x = D_800EE498_2[i].x;
                }
                if (D_800EE498_2[i].z > D_800F1470.z || i == 0) {
                    D_800F1470.z = D_800EE498_2[i].z;
                }
            }
            x = D_800CCD00[(D_800C2594 - 1) * 2];
            wA = D_800F13EC;
            hA = D_800F13F0;
            wB = D_800F13EC;
            hB = D_800F13F0;
            y = D_800CCD00[(D_800C2594 - 1) * 2 + 1];
            D_800F14B8 = wB / 5;
            D_800F145C = (x * wA) / 0x140;
            D_800F14BC = (hB * 0x40) / 0xF0;
            D_800F1460 = (y * hA) / 0xF0;
            if (D_800EFDBC == 7) {
                D_800F14B8 = D_800F14B8 + D_800F14B8 / 2;
                D_800F14BC = D_800F14BC + D_800F14BC / 2;
            }
            if (D_800CCCA8 == 2) {
                D_800F1460 = D_800F1460 + 0x34;
            }
            D_800F1450 = 0x10000 / D_800F14B8;
            D_800F1454 = 0x10000 / D_800F14BC;
            func_80069158();
        }
        break;
    }
}

typedef struct {
    Quad r[4];
} Ent40_3;

typedef struct {
    s32 f0, f4, f8, fC;
    u8 pad[0x50 - 0x10];
} Ent50;

extern Ent40_3 D_800E1718_11[];

Gfx *func_80067E4C(Gfx *gdl)
{
    s32 i;
    s32 n = D_800C2594;

    if (D_800C2388 == 0) {
        D_800CCCF0 = D_800E1718_11[n - 1].r[0].a + 4;
        D_800CCCF4 = D_800E1718_11[n - 1].r[1].a + D_800E1718_11[n - 1].r[1].c - 8;
    } else {
        D_800CCCF0 = D_800E1718_11[n - 1].r[0].b + 6;
        D_800CCCF4 = D_800E1718_11[n - 1].r[n - 1].b + D_800E1718_11[n - 1].r[n - 1].d - 12;
    }

    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);

    if (D_800C2388 == 0) {
        gDPLoadTextureBlock(gdl++, D_800F1478, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800CCCF0 << 2, 116 << 2, D_800CCCF4 << 2, 124 << 2,
                            G_TX_RENDERTILE, 0, 0, 0, 0x400);
    } else {
        gDPLoadTextureBlock(gdl++, D_800CCCF8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, (D_800F13EC * 252 / 512) << 2, D_800CCCF0 << 2,
                            (D_800F13EC * 260 / 512) << 2, D_800CCCF4 << 2,
                            G_TX_RENDERTILE, 0, 0, 0x80000 / D_800F13EC, 0);
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);

    for (i = 0; i < 6; i++) {
        if (D_800E37C0_2[i].f7A5 == 0) {
            if (D_800E37C0_2[i].f44 == 0 || D_800E76D0 % 8 >= 4) {
                if (D_800E37C0_2[i].f6E8 != 0) {
                    gdl = func_800685D8(gdl, i, 0);
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (D_800E37C0_2[i].f7A5 == 0) {
            if (D_800E37C0_2[i].f44 == 0 || D_800E76D0 % 8 >= 4) {
                if (D_800E37C0_2[i].f6E8 == 0) {
                    gdl = func_800685D8(gdl, i, 1);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    return gdl;
}

#define _SHIFTL_2(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define G_RDPPIPESYNC_2 0xE7
#define G_SETPRIMCOLOR_2 0xFA
#define G_FILLRECT_2 0xF6

#define gDPNoParam_2(pkt, cmd) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = _SHIFTL_2(cmd, 24, 8); \
        _g->words.w1 = 0; \
    }

#define gDPPipeSync_2(pkt) gDPNoParam_2(pkt, G_RDPPIPESYNC_2)

#define gDPSetPrimColor_2(pkt, m, l, r, g, b, a) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (_SHIFTL_2(G_SETPRIMCOLOR_2, 24, 8) | _SHIFTL_2(m, 8, 8) | _SHIFTL_2(l, 0, 8)); \
        _g->words.w1 = (_SHIFTL_2(r, 24, 8) | _SHIFTL_2(g, 16, 8) | _SHIFTL_2(b, 8, 8) | _SHIFTL_2(a, 0, 8)); \
    }

#define gDPFillRectangle_2(pkt, ulx, uly, lrx, lry) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (_SHIFTL_2(G_FILLRECT_2, 24, 8) | _SHIFTL_2((lrx), 14, 10) | _SHIFTL_2((lry), 2, 10)); \
        _g->words.w1 = (_SHIFTL_2((ulx), 14, 10) | _SHIFTL_2((uly), 2, 10)); \
    }

Gfx *func_80068484(Gfx *gdl, s32 player, s32 big)
{
    s32 x;
    s32 y;
    s32 cx;
    s32 cy;
    Gfx *g;

    func_80068F38(player, &x, &y);
    gDPPipeSync_2(gdl++);
    gDPSetPrimColor_2(gdl, 0, 0, D_800DC150[player].r, D_800DC150[player].g, D_800DC150[player].b, 255);
    g = gdl + 1;
    cx = x;
    cy = y;
    if (big) {
        gDPFillRectangle_2(g++, cx - 4, cy - 3, cx + 4, cy + 3);
    } else {
        gDPFillRectangle_2(g++, cx - 2, cy - 2, cx + 2, cy + 2);
    }
    return g;
}

typedef struct {
    u8 pad0[0x46];
    s16 next;
    u8 pad48[0x66 - 0x48];
    u16 pos;
    u8 pad68[0x6C - 0x68];
} Node_19;

typedef struct {
    u8 pad0[0x628];
    s16 node;
    u8 pad62A[0x63C - 0x62A];
    f32 frac;
    u8 pad640[0x7E0 - 0x640];
} Rider_5;

extern Node_19 *D_800EE498_27;

Gfx *func_800685D8(Gfx *gdl, s32 idx, s32 half)
{
    Rider_5 *r;
    Rider_5 *rb;
    Node_19 *a;
    Node_19 *b;
    s32 v;
    s32 h;
    s32 k;
    f32 f;

    rb = D_800E37C0;
    r = &rb[idx];
    k = r->node;
    a = &D_800EE498_27[k];
    v = a->pos;
    k = a->next;
    b = &D_800EE498_27[k];
    if (b->pos < a->pos) {
        f = r->frac;
        if (f < 0.0f) {
            f = 0.0f;
        }
        if (1.0f < f) {
            f = 1.0f;
        }
        v = (s32)((f32)v + f * (f32)(b->pos - v));
    } else {
        v = b->pos;
    }
    if (D_800C2388 != 0) {
        v -= 0x3DD3;
        v = (v * 0x8000) / 0x422D;
    }
    v = 0x8000 - v;
    if (v < 0) {
        v = 0;
    }
    if (v > 0x8000) {
        v = 0x8000;
    }
    if (D_800C2388 != 0) {
        v = 0x7FFF - v;
    }
    v = (v * (D_800CCCF4 - D_800CCCF0)) >> 15;
    v += D_800CCCF0;
    if (D_800C2388 == 0) {
        Gfx *g = gdl;

        gDPSetPrimColor(g++, 0, 0, D_800DC150[idx].r, D_800DC150[idx].g, D_800DC150[idx].b, 255);
        if (half) {
            gDPFillRectangle(g++, v - 4, 117, v + 4, 123);
        } else {
            gDPFillRectangle(g++, v - 2, 118, v + 2, 122);
        }
        gdl = g;
    } else {
        Gfx *g;

        h = D_800F13EC / 2;
        if (v > 0xD5) {
            v = 0xD5;
        }
        g = gdl;
        gDPSetPrimColor(g++, 0, 0, D_800DC150[idx].r, D_800DC150[idx].g, D_800DC150[idx].b, 255);
        if (half) {
            gDPFillRectangle(g++, h - 4, v - 3, h + 4, v + 3);
        } else {
            gDPFillRectangle(g++, h - 2, v - 2, h + 2, v + 2);
        }
        gdl = g;
    }
    return gdl;
}

#define WD(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_80068910(Gfx *gdl, Dyn *arg1, s32 arg2) {
    Racer *e;
    s32 i;

    if (D_800CA934 != 0 || D_800F1458 == 0 || (D_800EFD70_8.f0 & 0x81) || D_800C2384 != 0) {
        return gdl;
    }
    if (D_800C2594 >= 2) {
        return func_80067E4C(gdl);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    WD(gdl++, 0xE200001C, 0x00404240);
    WD(gdl++, 0xFCFF97FF, 0xFF2CFE7F);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0x10);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 0x60);
    gDPLoadTextureBlock(gdl++, D_800F1458 + 8, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTextureRectangle(gdl++, D_800F145C * 4, D_800F1460 * 4,
                        (D_800F145C + D_800F14B8) * 4, (D_800F1460 + D_800F14BC) * 4,
                        G_TX_RENDERTILE, 0, 0, D_800F1452, D_800F1456);
    gDPPipeSync(gdl++);
    WD(gdl++, 0xE200001C, 0x0C084000);
    WD(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);

    e = D_800E37C0_2;
    for (i = 0; i < 6; i++) {
        if (D_800E37C0_2[i].f7A5 != 0) continue;
        if (D_800E37C0_2[i].f44 != 0 && D_800E76D0 % 8 < 4) continue;
        if (D_800E37C0_2[i].f6E8 != 0 || (D_800EFD70_8.f4C == 8 && i != D_800C8CE0)) {
            gdl = func_80068484(gdl, i, 0);
        }
    }
    e = D_800E37C0_2;
    for (i = 0; i < 6; i++) {
        if (D_800E37C0_2[i].f7A5 != 0) continue;
        if (D_800E37C0_2[i].f44 != 0 && D_800E76D0 % 8 < 4) continue;
        if (D_800E37C0_2[i].f6E8 != 0) continue;
        if (D_800EFD70_8.f4C == 8 && i != D_800C8CE0) continue;
        gdl = func_80068484(gdl, i, 1);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct { f32 x, y; } V2;
typedef struct { u8 pad[0x628]; s16 obj; u8 pad2[0x63C - 0x62A]; f32 t; u8 pad3[0x7E0 - 0x640]; } Player_25;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x6C - 0x48]; } Obj_50;

extern Obj_50 *D_800EE498_28;

void func_80068F38(s32 player, s32 *px, s32 *py)
{
    Player_25 *pl;
    Player_25 *base;
    Obj_50 *o;
    Obj_50 *o2;
    V2 a, b;
    f32 cx, cy, t;
    s32 x, y, w, h;
    f32 fy, nx, ny, dx, dy;
    s32 p, q;

    base = D_800E37C0;
    pl = &base[player];
    cx = (D_800F1468_2[0] + D_800F1470_2[0]) * 0.5f;
    cy = (D_800F1468_2[1] + D_800F1470_2[1]) * 0.5f;
    o = &D_800EE498_28[pl->obj];
    o2 = &D_800EE498_28[o->next];
    func_80069A84((f32 *)o, &a.x, &a.y);
    func_80069A84((f32 *)o2, &b.x, &b.y);
    t = pl->t;
    if (t < 0.0f) {
        t = 0.0f;
    }
    if (1.0f < t) {
        t = 1.0f;
    }
    dx = t * (b.x - a.x);
    dy = t * (b.y - a.y);
    nx = a.x + dx;
    t = (nx - cx) * (f32)D_800F1458_2[0];
    ny = a.y + dy;
    fy = (ny - cy) * (f32)D_800F1458_2[1];
    t *= 6.1035156e-05f;
    fy *= 6.1035156e-05f;
    w = D_800F14B8;
    x = (s32)t;
    p = x * w;
    a.x = nx;
    a.y = ny;
    y = (s32)fy;
    y = -y;
    if (p < 0) {
        p += 0x7F;
    }
    x = p >> 7;
    h = D_800F14BC;
    q = y * h;
    if (q < 0) {
        q += 0x7F;
    }
    y = q >> 7;
    x += D_800F145C + w / 2;
    y += D_800F1460 + h / 2;
    *px = x;
    *py = y;
}

typedef struct {
    f32 x, y, z;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x46 - 0x1C];
    s16 f46;
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} Node_20;

typedef struct { s32 sx, sy; u8 data[0x1000]; } MapImg;
typedef struct { u8 pad[0x4C]; s32 f4C; } Race_6;

extern MapImg *D_800F1458_3;
extern Node_20 *D_800EE498_29;
extern Race_6 D_800EFD70_47;

static inline void getpos(Node_20 *n, f32 *x, f32 *z) {
    f32 h = (n->f58 - n->f5C) * 0.5f;
    *x = n->x - n->f14 * h;
    *z = n->z - n->f18 * h;
}

static inline f32 dist(f32 x, f32 y) {
    x -= 7.5f;
    y -= 7.5f;
    return __builtin_sqrtf(x * x + y * y);
}

static inline void stamp(u16 *brush, u16 *map, s32 cx, s32 cy) {
    s32 x, y, k;
    k = 0;
    for (y = cy - 8; y < cy + 8; y++) {
        for (x = cx - 8; x < cx + 8; x++) {
            if (y >= 0 && x >= 0 && y < 128 && x < 128) {
                if (brush[k] < map[y * 128 + x]) map[y * 128 + x] = brush[k];
            }
            k++;
        }
    }
}

void func_80069158(void) {
    f32 px, pz;
    f32 mx, my;
    f32 sx, sy, r, len, dx, dz, x0f;
    s32 x1, y1;
    s32 i, j, steps, x0, y0, ddx, ddy, ax, ay, t, cx, cy, x, y, k;
    u16 *brush;
    u16 *map;
    u16 *q;
    u16 *pp;
    f32 *pa, *pb;
    Node_20 *n;
    Node_20 *m;

    for (i = 0; i < 64; i++) {
        D_800F1478[i] = D_800CCCF8[i / 8];
    }
    D_800F1458_3 = func_800BC908(0x1003, 0, (s32)D_800D8DCC, 0x23E);
    for (i = 0; i < D_800EE490; i++) {
        if (D_800EE498_29[i].x < D_800F1468.x || i == 0) D_800F1468.x = D_800EE498_29[i].x;
        if (D_800EE498_29[i].z < D_800F1468.z || i == 0) D_800F1468.z = D_800EE498_29[i].z;
        if (D_800EE498_29[i].x > D_800F1470.x || i == 0) D_800F1470.x = D_800EE498_29[i].x;
        if (D_800EE498_29[i].z > D_800F1470.z || i == 0) D_800F1470.z = D_800EE498_29[i].z;
    }
    mx = (D_800F1468.x + D_800F1470.x) * 0.5f;
    my = (D_800F1468.z + D_800F1470.z) * 0.5f;
    sx = 118.0f / (D_800F1470.x - D_800F1468.x);
    sy = 118.0f / (D_800F1470.z - D_800F1468.z);
    if (sx < sy) sy = sx; else sx = sy;
    r = sx / sy;
    if (r > 0.8f && r < 1.2f) {
        sx /= 1.3f;
        sy /= 1.3f;
    }
    D_800F1458_3->sx = sx * 16384.0f;
    D_800F1458_3->sy = sy * 16384.0f;
    steps = sx * 6.0f;
    if (steps < 2) steps = 2;

    brush = D_800C29E0_2;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            brush[i * 16 + j] = (u32)(dist(i, j) * 256.0f);
        }
    }
    map = D_800C29E0_2 + 0x100;
    func_8000ED10((u8 *)map, 0xFF, 0x8000);

    for (i = 0; i < D_800EE490; i++) {
        n = (Node_20 *)(i * sizeof(Node_20) + (u32)D_800EE498_29);
        j = n->f46;
        if (j == 0 && D_800EFD70_47.f4C == 7) break;
        getpos(n, &px, &pz);
        x0 = (px - mx) * sx + 64.0f;
        y0 = 64.0f - (pz - my) * sy;
        getpos(&D_800EE498_29[j], &px, &pz);
        cx = (px - mx) * sx + 64.0f;
        y1 = 64.0f - (pz - my) * sy;
        for (t = 0; t < steps; t++) {
            stamp(brush, map, x0 + t * (cx - x0) / steps, y0 + t * (y1 - y0) / steps);
        }
    }

    j = D_800EE498_29[D_800EE494].f46;
    x0 = (D_800EE498_29[D_800EE494].x - mx) * sx + 64.0f;
    y0 = 64.0f - (D_800EE498_29[D_800EE494].z - my) * sy;
    px = D_800EE498_29[j].x - D_800EE498_29[D_800EE494].x;
    pz = D_800EE498_29[j].z - D_800EE498_29[D_800EE494].z;
    len = __builtin_sqrtf(px * px + pz * pz);
    if (len > 0.0f) {
        px /= len;
        pz /= len;
    }
    pz = -pz;
    dz = pz;
    dx = px;
    for (i = -8; i < 9; i++) {
        cx = x0 + (s32)(i * dz);
        cy = y0 - (s32)(i * dx);
        {
            s32 xx, yy;
            t = 0;
            for (yy = cy - 8; yy < cy + 8; yy++) {
                for (xx = cx - 8; xx < cx + 8; xx++) {
                    if (yy >= 0 && xx >= 0 && yy < 128 && xx < 128) {
                        if (brush[t] < map[yy * 128 + xx]) map[yy * 128 + xx] = brush[t];
                    }
                    t++;
                }
            }
        }
    }

    for (i = 0; i < 0x4000; i++) {
        t = map[i] >> 7;
        if (t >= 16) t = 15;
        map[i] = D_800CCD20[t].b;
    }
    k = x = 0;
    for (i = 0; i < 128; i += 2) {
        for (j = 0; j < 128; j += 2) {
            u16 a = map[x], b = map[x + 1], cc = map[x + 128], d = map[x + 129];
            s32 lo;
            x += 2;
            t = (a >> 4) + (b >> 4) + (cc >> 4) + (d >> 4);
            lo = (a & 15) + (b & 15) + (cc & 15) + (d & 15);
            t >>= 2;
            lo >>= 2;
            D_800F1458_3->data[k] = (t << 4) | lo;
            k++;
        }
        x += 128;
    }
}

void func_80069A84(f32 *a, f32 *b, f32 *c)
{
    f32 t = (a[0x16] - a[0x17]) * 0.5f;
    b[0] = a[0] - a[5] * t;
    c[0] = a[2] - a[6] * t;
}

Gfx *func_80069AC8(Gfx *gdl, s32 idx, s32 flag, s32 x, s32 y) {
    gDPSetPrimColor(gdl++, 0, 0, D_800DC150_11[idx][0x21C], D_800DC150_11[idx][0x21D],
                    D_800DC150_11[idx][0x21E], 255);
    if (flag != 0) {
        gDPFillRectangle(gdl++, x - 4, y - 3, x + 4, y + 3);
    } else {
        gDPFillRectangle(gdl++, x - 2, y - 2, x + 2, y + 2);
    }
    return gdl;
}

void func_80069BC8(u16 *dst, u16 *src, s32 x, s32 y) {
    s32 i4;
    s32 i2;
    s32 k;
    u16 *d;

    k = 0;
    for (i4 = y - 8; i4 < y + 8; i4++) {
        i2 = x - 8;
        if (i2 < x + 8) {
        for (; i2 < x + 8; i2++) {
            if (i4 >= 0 && i2 >= 0 && i4 < 0x80 && i2 < 0x80) {
                d = (u16 *)((s32)(i4 * 0x80 + i2) * 2 + (s32)dst);
                if (src[k] < *d) {
                    *d = src[k];
                }
            }
            k++;
        }
        }
    }
}

void func_80069C6C(void) {
    if (D_800F1458 != 0) {
        func_800BCB04(D_800F1458);
        D_800F1458 = 0;
    }
}

void func_80069CA0(Obj_51 *a, s32 b)
{
    s32 i;
    s32 k;
    s32 save;

    if (a != NULL && a->src != NULL) {
        if (D_800F14C0 >= D_800F14D8) func_8000ECB8(1);
        D_800F14D0[D_800F14C0].val = b;
        D_800F14D0[D_800F14C0].obj = a;
        D_800F14D0[D_800F14C0].buf = func_800BC908(0x500, 0, D_800D8E10, 0x8C);
        D_800F14D0[D_800F14C0].src = a->src;
        D_800F14D0[D_800F14C0].dst = D_800F14D0[D_800F14C0].buf + 0x300;
        __builtin_memcpy(D_800F14D0[D_800F14C0].dst, D_800F14D0[D_800F14C0].src, 0x200);
        k = 0;
        for (i = 0; i < 0x100; i++) {
            if (D_800C2280 & 0x80) {
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 6) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 1) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = a->src[i] >> 11;
            } else {
                D_800F14D0[D_800F14C0].buf[k++] = a->src[i] >> 11;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 6) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 1) & 0x1F;
            }
        }
        i = D_800E1B70;
        D_800E1B70 = 0;
        func_8006AA9C(a->src, D_800F14D0[D_800F14C0].buf);
        D_800E1B70 = i;
        __builtin_memcpy(D_800F14D0[D_800F14C0].dst, a->src, 0x200);
        D_800F14C0++;
    }
}

typedef struct {
    u8 *src;
    s32 f4;
    Trk_2 *f8;
    u16 *dst[2];
} PalEnt_2;

extern PalEnt_2 *D_800F14D0_2;

static inline void put(u16 *d, s32 *r, s32 *g, s32 *b) {
    *d = (*r << 11) | (*g << 6) | (*b << 1) | (*d & 1);
}

static inline void sepia(s32 v, s32 *r, s32 *g, s32 *b) {
    if (v > 128) {
        v -= 128;
        *r = v * 175 / 128 + 80;
        *g = v * 185 / 128 + 70;
        *b = v * 255 / 128;
    } else {
        *r = v * 80 / 128;
        *g = v * 70 / 128;
        *b = 0;
    }
}

void func_8006A0AC(void) {
    s32 i, j, k;
    u16 *dst;
    u8 *src;
    s32 j1;
    s32 r0, g0, b0;
    s32 v;

    for (i = 0; i < D_800F14C0; i++) {
        D_800F14D0_2[i].f8->f4 = (s32)D_800F14D0_2[i].dst[D_800E30A8];
    }
    if (((D_800EFD64_6 - (s32)D_800CB210) * -0x33333333 >> 5) == 11 && D_800E1898 == 1 && D_800E3710 == 0) {
        if (D_800F14E8 != 0) {
            D_800F14E8--;
        }
        if ((s32)func_8000EFE8() % 80 == 0 && D_800F14E0 < 0) {
            D_800F14E0 = 0;
            D_800F14DC = (s32)func_8000EFE8() % 2;
            func_8004F270(0x67, -1, 1);
        }
        if (D_800F14E0 < 0) {
            D_800F14E8 = 0;
        } else {
            s32 n = D_800F14E0;
            D_800F14E0 = n + 1;
            D_800F14E8 = D_800CCD68[D_800F14DC][n];
            if (D_800F14E0 == 10) {
                D_800F14E0 = -1;
            }
        }
        if (D_800E3710 != 0) {
            D_800F14E8 = 0;
        }
        v = D_800F14E8 != 0;
        if (v != D_800F14C8[D_800E30A8]) {
            D_800F14C8[D_800E30A8] = v;
            for (i = 0; i < D_800F14C0; i++) {
                dst = D_800F14D0_2[i].dst[D_800E30A8];
                src = D_800F14D0_2[i].src;
                for (k = 0; k < 256; k++) {
                    r0 = *src++;
                    g0 = *src++;
                    b0 = *src++;
                    if (D_800F14E8 != 0) {
                        r0 = (r0 + g0 + b0) / 3;
                        r0 *= 3;
                        if (r0 >= 32) {
                            r0 = 31;
                        }
                        b0 = r0;
                        g0 = r0;
                    }
                    j = (r0 << 11) | (g0 << 6) | (b0 << 1);
                    j |= *dst & 1;
                    *dst = j;
                    dst++;
                }
            }
        }
    } else if (D_800C2358 != 0) {
        if (D_800E1B70 != 0) {
            for (i = 0; i < D_800F14C0; i++) {
                s32 r, g, b, a, dr, dg, db;
                u16 *dst = D_800F14D0_2[i].dst[D_800E30A8];
                u8 *src = D_800F14D0_2[i].src;
                for (j1 = 0; j1 < 256; j1++) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = (r + g + b) / 3;
                    r += (a - r) * D_800E1B70 >> 8;
                    g += (a - g) * D_800E1B70 >> 8;
                    b += (a - b) * D_800E1B70 >> 8;
                    a = (r << 11) | (g << 6) | (b << 1);
                    a |= *dst & 1;
                    *dst = a;
                    dst++;
                }
            }
        }
    } else if (((D_800EFD64_6 - (s32)D_800CB210) * -0x33333333 >> 5) != 20) {
        if (D_800CCD64 != 0) {
            k = -123;
            if (D_800F14C8[D_800E30A8] != k) {
                D_800F14C8[D_800E30A8] = k;
                for (i = 0; i < D_800F14C0; i++) {
                    dst = D_800F14D0_2[i].dst[D_800E30A8];
                    src = D_800F14D0_2[i].src;
                    for (j = 0; j < 256; j++) {
                        r0 = *src++;
                        g0 = *src++;
                        b0 = *src++;
                        r0 = (r0 + g0 + b0) / 3;
                        sepia(r0, &r0, &g0, &b0);
                        put(dst, &r0, &g0, &b0);
                        dst++;
                    }
                }
            }
        } else if (((Cfg *)D_800EFD64_6)->f2C == 1) {
            k = (D_800E8E9C - 25) * 3;
            if (k < 0) {
                k = 0;
            }
            if (k > 100) {
                k = 100;
            }
            D_800CCD60 = k;
            if (k != D_800F14C8[D_800E30A8]) {
                D_800F14C8[D_800E30A8] = k;
                for (i = 0; i < D_800F14C0; i++) {
                    dst = D_800F14D0_2[i].dst[D_800E30A8];
                    src = D_800F14D0_2[i].src;
                    for (j = 0; j < 256; j++) {
                        r0 = *src++;
                        g0 = *src++;
                        b0 = *src++;
                        r0 = r0 + (31 - r0) * k / 256;
                        g0 = g0 + (31 - g0) * k / 256;
                        b0 = b0 + (31 - b0) * k / 256;
                        put(dst, &r0, &g0, &b0);
                        dst++;
                    }
                }
            }
        }
    }
}

void func_8006A8B0(s32 p)
{
    D_800CCD60 = 0;
    D_800F14C0 = 0;
    D_800F14C8[0] = -1;
    D_800F14C8[1] = -1;
    D_800F14E0 = -1;
    if (p != 0) {
        D_800F14D8 = 0x10;
    } else {
        D_800F14D8 = 8;
    }
    D_800F14D0 = (s32)func_800BC908(D_800F14D8 * 0x14, 0, D_800D8E10, 0x74);
}

typedef struct {
    s32 w0, w1, w2, w3, w4;
} Ent_70;

extern Ent_70 *D_800F14D0_3;

void func_8006A944(void)
{
    s32 i;

    for (i = 0; i < D_800F14C0; i++) {
        func_800BCB04(D_800F14D0_3[i].w0);
    }
    D_800F14D4 = 0;
    D_800F14E4 = 0;
    D_800F14C0 = 0;
    func_800BCB04((s32)D_800F14D0_3);
}

void func_8006A9DC(s32 a0, s32 *a1, s32 *a2, s32 *a3)
{
    s32 t;
    s32 d;

    if (a0 > 0x80) {
        d = a0 - 0x80;
        t = d * 0xAF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a1 = (t >> 7) + 0x50;
        t = d * 0xB9;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a2 = (t >> 7) + 0x46;
        t = d * 0xFF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a3 = t >> 7;
        return;
    }
    t = a0 * 0x50;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a1 = t >> 7;
    t = a0 * 0x46;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a2 = t >> 7;
    *a3 = 0;
}

void func_8006AA9C(u16 *dst, u8 *src)
{
    s32 i;
    s32 r, g, b, c;

    for (i = 0; i < 0x100; i++) {
        r = *src++;
        g = *src++;
        b = *src++;
        c = (r + g + b) / 3;
        r += ((c - r) * D_800E1B70) >> 8;
        g += ((c - g) * D_800E1B70) >> 8;
        b += ((c - b) * D_800E1B70) >> 8;
        c = (r << 11) | (g << 6) | (b << 1);
        c |= *dst & 1;
        *dst = c;
        dst++;
    }
}

void func_8006AB64(u16 *dst, u16 *src)
{
    s32 i;
    u32 v;
    s32 g, b, avg;

    for (i = 0; i < 0x100; i++) {
        v = *src;
        g = (v >> 3) & 0xF8;
        b = (v << 2) & 0xF8;
        v = (v >> 8) & 8;
        avg = (s32)(v + g + b) / 3;
        v += ((avg - (s32)v) * D_800E1B70) >> 8;
        v = (s32)v >> 3;
        g += ((avg - g) * D_800E1B70) >> 8;
        g >>= 3;
        b += ((avg - b) * D_800E1B70) >> 8;
        b >>= 3;
        avg = (v << 11) | (g << 6) | (b << 1);
        avg |= *dst & 1;
        *dst = avg;
        dst++;
        src++;
    }
}

typedef struct {
    u32 count;
    s32 pad4;
    s32 x0, z0, x1, z1;
    s32 pad18, pad1C;
} Ent_71;

extern Ent_71 *D_800C9644_9;

void func_8006AC40(s32 arg0)
{
    Rec_41 *list;
    Rec_41 *r;
    Rec_41 *recs;
    s32 i;
    u32 j;
    s32 cx, cz, dx, dz, d;
    s32 best;
    s32 bi;

    D_800F14F0 = list = (Rec_41 *)func_8005AED8(D_800D1590);
    if (list == NULL) {
        return;
    }
    i = 0;
    if (i >= list->count) goto end;
    recs = list;
    for (; i < recs->count; i++) {
        r = &recs[i];
        cx = (r->v[0][0] + r->v[1][0] + r->v[2][0] + r->x3) / 4;
        cz = (r->v[0][2] + r->v[1][2] + r->v[2][2] + r->z3) / 4;
        for (j = 0; j < D_800C9644_9->count; j++) {
            dx = (D_800C9644_9[j].x0 + D_800C9644_9[j].x1) / 2 - cx;
            dz = (D_800C9644_9[j].z0 + D_800C9644_9[j].z1) / 2 - cz;
            dx /= 4;
            dz /= 4;
            dx = dx * dx + dz * dz;
            if (dx < best || j == 0) {
                best = dx;
                bi = j;
            }
        }
        r->best = bi;
    }
end:
    D_800F14F8 = func_800BC908(D_800F14F0->count << 2, 0, D_800D8E20, 0x6F);
    D_800F14F4 = 0;
}

typedef struct {
    u16 idx;
    s16 x0, y0, z0;
    s16 x1, y1, z1;
    s16 pad[3];
    s16 x2, y2, z2;
} BikeTri;

typedef struct { u16 n; BikeTri t[1]; } Tbl_5;

extern BikeTri **D_800F14F8_2;

void func_8006ADF0(void)
{
    Tbl_5 *tbl = D_800F14F0;
    s32 i;
    s32 j;
    s32 o;
    Tbl_5 *b;

    if (tbl != 0 && D_800F14F4 != 0 && D_800C2594 == 1) {
        D_800F14F4 = D_800F14F4 - 1;
        D_800F14FC = 0;
        i = 0;
        if (tbl->n != 0) {
            b = tbl;
            do {
                j = b->t[i].idx;
                o = j * 32;
                if (((ObjInfo *)((s32)D_800C9644_10 + o))->flags & 1) {
                    D_800F14F8_2[D_800F14FC++] = &b->t[i];
                }
                i++;
            } while (i < b->n);
        }
        if (D_800F14FC != 0) {
            i = 0;
            do {
                BikeTri *s;
                s32 a, b;
                f32 v[4];
                s32 w[16];
                i++;
                j = (s32)func_8000EFE8() % D_800F14FC;
                s = D_800F14F8_2[j];
                a = (s32)func_8000EFE8() % 255;
                b = (s32)func_8000EFE8() % 255;
                v[0] = s->x0;
                v[1] = s->y0;
                v[2] = s->z0;
                v[0] += ((f32)s->x1 - (f32)s->x0) * (f32)a * 0.00390625f;
                v[1] += ((f32)s->y1 - (f32)s->y0) * (f32)a * 0.00390625f;
                v[2] += ((f32)s->z1 - (f32)s->z0) * (f32)a * 0.00390625f;
                v[0] += ((f32)s->x2 - (f32)s->x0) * (f32)b * 0.00390625f;
                v[1] += ((f32)s->y2 - (f32)s->y0) * (f32)b * 0.00390625f;
                v[2] += ((f32)s->z2 - (f32)s->z0) * (f32)b * 0.00390625f;
                v[0] *= 0.016f;
                v[1] *= 0.016f;
                v[2] *= 0.016f;
                w[0] = 0;
                w[1] = 0;
                w[2] = 0;
                func_80036374(5, 0, v, w, &D_800C9994);
            } while (i < 1);
        }
    }
}

void func_8006B160(void)
{
    if (D_800F14F0 != 0) {
        func_800BCB04(D_800F14F8);
    }
}

void func_8006B190(s32 x)
{
    D_800F14F4 = x;
}
