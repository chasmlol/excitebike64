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

extern Racer gRiders_2[];
extern Player gRiderGfx[];

void map_init(void)
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

    sMapImage = 0;
    switch (gGame.mode) {
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
        if ((gGame.flags & 0x8000) == 0) {
            for (i = 0; i < gPathNodeCount; i++) {
                if (gPathNodes_2[i].x < sMapBoundsMin.x || i == 0) {
                    sMapBoundsMin.x = gPathNodes_2[i].x;
                }
                if (gPathNodes_2[i].z < sMapBoundsMin.z || i == 0) {
                    sMapBoundsMin.z = gPathNodes_2[i].z;
                }
                if (gPathNodes_2[i].x > sMapBoundsMax.x || i == 0) {
                    sMapBoundsMax.x = gPathNodes_2[i].x;
                }
                if (gPathNodes_2[i].z > sMapBoundsMax.z || i == 0) {
                    sMapBoundsMax.z = gPathNodes_2[i].z;
                }
            }
            x = sMapPos[(gNumViews - 1) * 2];
            wA = gScreenWidth;
            hA = gScreenHeight;
            wB = gScreenWidth;
            hB = gScreenHeight;
            y = sMapPos[(gNumViews - 1) * 2 + 1];
            sMapW = wB / 5;
            sMapX = (x * wA) / 0x140;
            sMapH = (hB * 0x40) / 0xF0;
            sMapY = (y * hA) / 0xF0;
            if (gGame.mode == 7) {
                sMapW = sMapW + sMapW / 2;
                sMapH = sMapH + sMapH / 2;
            }
            if (gViMode == 2) {
                sMapY = sMapY + 0x34;
            }
            D_800F1450 = 0x10000 / sMapW;
            D_800F1454 = 0x10000 / sMapH;
            map_build_image();
        }
        break;
    }
}

typedef struct {
    Quad r[4];
} Ent40_3;

extern Ent40_3 gViewRects_11[];

Gfx *map_draw_progress_bar(Gfx *gdl)
{
    s32 i;
    s32 n = gNumViews;

    if (gSplitVertical == 0) {
        sMapBarStart = gViewRects_11[n - 1].r[0].a + 4;
        sMapBarEnd = gViewRects_11[n - 1].r[1].a + gViewRects_11[n - 1].r[1].c - 8;
    } else {
        sMapBarStart = gViewRects_11[n - 1].r[0].b + 6;
        sMapBarEnd = gViewRects_11[n - 1].r[n - 1].b + gViewRects_11[n - 1].r[n - 1].d - 12;
    }

    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);

    if (gSplitVertical == 0) {
        gDPLoadTextureBlock(gdl++, sMapBarTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, sMapBarStart << 2, 116 << 2, sMapBarEnd << 2, 124 << 2,
                            G_TX_RENDERTILE, 0, 0, 0, 0x400);
    } else {
        gDPLoadTextureBlock(gdl++, D_800CCCF8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, (gScreenWidth * 252 / 512) << 2, sMapBarStart << 2,
                            (gScreenWidth * 260 / 512) << 2, sMapBarEnd << 2,
                            G_TX_RENDERTILE, 0, 0, 0x80000 / gScreenWidth, 0);
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);

    for (i = 0; i < 6; i++) {
        if (gRiders_2[i].f7A5 == 0) {
            if (gRiders_2[i].f44 == 0 || gRaceFrame % 8 >= 4) {
                if (gRiders_2[i].f6E8 != 0) {
                    gdl = map_draw_progress_marker(gdl, i, 0);
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (gRiders_2[i].f7A5 == 0) {
            if (gRiders_2[i].f44 == 0 || gRaceFrame % 8 >= 4) {
                if (gRiders_2[i].f6E8 == 0) {
                    gdl = map_draw_progress_marker(gdl, i, 1);
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

Gfx *map_draw_rider_dot(Gfx *gdl, s32 player, s32 big)
{
    s32 x;
    s32 y;
    s32 cx;
    s32 cy;
    Gfx *g;

    map_get_rider_pos(player, &x, &y);
    gDPPipeSync_2(gdl++);
    gDPSetPrimColor_2(gdl, 0, 0, gRiderGfx[player].r, gRiderGfx[player].g, gRiderGfx[player].b, 255);
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

extern Node_19 *gPathNodes_27;

Gfx *map_draw_progress_marker(Gfx *gdl, s32 idx, s32 half)
{
    Rider_5 *r;
    Rider_5 *rb;
    Node_19 *a;
    Node_19 *b;
    s32 v;
    s32 h;
    s32 k;
    f32 f;

    rb = gRiders;
    r = &rb[idx];
    k = r->node;
    a = &gPathNodes_27[k];
    v = a->pos;
    k = a->next;
    b = &gPathNodes_27[k];
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
    if (gSplitVertical != 0) {
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
    if (gSplitVertical != 0) {
        v = 0x7FFF - v;
    }
    v = (v * (sMapBarEnd - sMapBarStart)) >> 15;
    v += sMapBarStart;
    if (gSplitVertical == 0) {
        Gfx *g = gdl;

        gDPSetPrimColor(g++, 0, 0, gRiderGfx[idx].r, gRiderGfx[idx].g, gRiderGfx[idx].b, 255);
        if (half) {
            gDPFillRectangle(g++, v - 4, 117, v + 4, 123);
        } else {
            gDPFillRectangle(g++, v - 2, 118, v + 2, 122);
        }
        gdl = g;
    } else {
        Gfx *g;

        h = gScreenWidth / 2;
        if (v > 0xD5) {
            v = 0xD5;
        }
        g = gdl;
        gDPSetPrimColor(g++, 0, 0, gRiderGfx[idx].r, gRiderGfx[idx].g, gRiderGfx[idx].b, 255);
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

Gfx *map_draw_minimap(Gfx *gdl, Dyn *arg1, s32 arg2) {
    Racer *e;
    s32 i;

    if (gHudLevel != 0 || sMapImage == 0 || (gGame.flags & 0x81) || gInsetViewActive != 0) {
        return gdl;
    }
    if (gNumViews >= 2) {
        return map_draw_progress_bar(gdl);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
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
    gDPLoadTextureBlock(gdl++, sMapImage + 8, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTextureRectangle(gdl++, sMapX * 4, sMapY * 4,
                        (sMapX + sMapW) * 4, (sMapY + sMapH) * 4,
                        G_TX_RENDERTILE, 0, 0, D_800F1452, D_800F1456);
    gDPPipeSync(gdl++);
    WD(gdl++, 0xE200001C, 0x0C084000);
    WD(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);

    e = gRiders_2;
    for (i = 0; i < 6; i++) {
        if (gRiders_2[i].f7A5 != 0) continue;
        if (gRiders_2[i].f44 != 0 && gRaceFrame % 8 < 4) continue;
        if (gRiders_2[i].f6E8 != 0 || (gGame.mode == 8 && i != D_800C8CE0)) {
            gdl = map_draw_rider_dot(gdl, i, 0);
        }
    }
    e = gRiders_2;
    for (i = 0; i < 6; i++) {
        if (gRiders_2[i].f7A5 != 0) continue;
        if (gRiders_2[i].f44 != 0 && gRaceFrame % 8 < 4) continue;
        if (gRiders_2[i].f6E8 != 0) continue;
        if (gGame.mode == 8 && i != D_800C8CE0) continue;
        gdl = map_draw_rider_dot(gdl, i, 1);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct { f32 x, y; } V2;
typedef struct { u8 pad[0x628]; s16 obj; u8 pad2[0x63C - 0x62A]; f32 t; u8 pad3[0x7E0 - 0x640]; } Player_25;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x6C - 0x48]; } Obj_50;

extern Obj_50 *gPathNodes_28;

void map_get_rider_pos(s32 player, s32 *px, s32 *py)
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

    base = gRiders;
    pl = &base[player];
    cx = (sMapBoundsMin_2[0] + sMapBoundsMax_2[0]) * 0.5f;
    cy = (sMapBoundsMin_2[1] + sMapBoundsMax_2[1]) * 0.5f;
    o = &gPathNodes_28[pl->obj];
    o2 = &gPathNodes_28[o->next];
    map_get_node_pos((f32 *)o, &a.x, &a.y);
    map_get_node_pos((f32 *)o2, &b.x, &b.y);
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
    t = (nx - cx) * (f32)sMapImage_2[0];
    ny = a.y + dy;
    fy = (ny - cy) * (f32)sMapImage_2[1];
    t *= 6.1035156e-05f;
    fy *= 6.1035156e-05f;
    w = sMapW;
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
    h = sMapH;
    q = y * h;
    if (q < 0) {
        q += 0x7F;
    }
    y = q >> 7;
    x += sMapX + w / 2;
    y += sMapY + h / 2;
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
extern MapImg *sMapImage_3;
extern Node_20 *gPathNodes_29;

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

void map_build_image(void) {
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
        sMapBarTex[i] = D_800CCCF8[i / 8];
    }
    sMapImage_3 = mem_alloc_dbg(0x1003, 0, (s32)D_800D8DCC, 0x23E);
    for (i = 0; i < gPathNodeCount; i++) {
        if (gPathNodes_29[i].x < sMapBoundsMin.x || i == 0) sMapBoundsMin.x = gPathNodes_29[i].x;
        if (gPathNodes_29[i].z < sMapBoundsMin.z || i == 0) sMapBoundsMin.z = gPathNodes_29[i].z;
        if (gPathNodes_29[i].x > sMapBoundsMax.x || i == 0) sMapBoundsMax.x = gPathNodes_29[i].x;
        if (gPathNodes_29[i].z > sMapBoundsMax.z || i == 0) sMapBoundsMax.z = gPathNodes_29[i].z;
    }
    mx = (sMapBoundsMin.x + sMapBoundsMax.x) * 0.5f;
    my = (sMapBoundsMin.z + sMapBoundsMax.z) * 0.5f;
    sx = 118.0f / (sMapBoundsMax.x - sMapBoundsMin.x);
    sy = 118.0f / (sMapBoundsMax.z - sMapBoundsMin.z);
    if (sx < sy) sy = sx; else sx = sy;
    r = sx / sy;
    if (r > 0.8f && r < 1.2f) {
        sx /= 1.3f;
        sy /= 1.3f;
    }
    sMapImage_3->sx = sx * 16384.0f;
    sMapImage_3->sy = sy * 16384.0f;
    steps = sx * 6.0f;
    if (steps < 2) steps = 2;

    brush = gZBuffer_2;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            brush[i * 16 + j] = (u32)(dist(i, j) * 256.0f);
        }
    }
    map = gZBuffer_2 + 0x100;
    mem_fill((u8 *)map, 0xFF, 0x8000);

    for (i = 0; i < gPathNodeCount; i++) {
        n = (Node_20 *)(i * sizeof(Node_20) + (u32)gPathNodes_29);
        j = n->f46;
        if (j == 0 && gGame.mode == 7) break;
        getpos(n, &px, &pz);
        x0 = (px - mx) * sx + 64.0f;
        y0 = 64.0f - (pz - my) * sy;
        getpos(&gPathNodes_29[j], &px, &pz);
        cx = (px - mx) * sx + 64.0f;
        y1 = 64.0f - (pz - my) * sy;
        for (t = 0; t < steps; t++) {
            stamp(brush, map, x0 + t * (cx - x0) / steps, y0 + t * (y1 - y0) / steps);
        }
    }

    j = gPathNodes_29[gPathStartNode].f46;
    x0 = (gPathNodes_29[gPathStartNode].x - mx) * sx + 64.0f;
    y0 = 64.0f - (gPathNodes_29[gPathStartNode].z - my) * sy;
    px = gPathNodes_29[j].x - gPathNodes_29[gPathStartNode].x;
    pz = gPathNodes_29[j].z - gPathNodes_29[gPathStartNode].z;
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
            sMapImage_3->data[k] = (t << 4) | lo;
            k++;
        }
        x += 128;
    }
}

void map_get_node_pos(f32 *a, f32 *b, f32 *c)
{
    f32 t = (a[0x16] - a[0x17]) * 0.5f;
    b[0] = a[0] - a[5] * t;
    c[0] = a[2] - a[6] * t;
}

Gfx *map_draw_dot(Gfx *gdl, s32 idx, s32 flag, s32 x, s32 y) {
    gDPSetPrimColor(gdl++, 0, 0, gRiderGfx_11[idx][0x21C], gRiderGfx_11[idx][0x21D],
                    gRiderGfx_11[idx][0x21E], 255);
    if (flag != 0) {
        gDPFillRectangle(gdl++, x - 4, y - 3, x + 4, y + 3);
    } else {
        gDPFillRectangle(gdl++, x - 2, y - 2, x + 2, y + 2);
    }
    return gdl;
}

void map_stamp_brush(u16 *dst, u16 *src, s32 x, s32 y) {
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

void map_free(void) {
    if (sMapImage != 0) {
        mem_free(sMapImage);
        sMapImage = 0;
    }
}
