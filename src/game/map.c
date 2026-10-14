#include "game.h"

/* gRiderGfx as this file sees it: the rider's colour */
typedef struct {
    u8 unk0[0x21C];
    u8 r;
    u8 g;
    u8 b;
    u8 unk21F[0xA80 - 0x21F];
} Player;

extern Player gRiderGfx[];

/* Sets up the minimap of a one-player race (not in the modes without a lap of the racing line, nor in replays):
   its place and size on screen, then the image. */
void map_init(void)
{
    s32 i;
    s32 screenW;
    s32 screenH;
    s32 screenW2;
    s32 screenH2;
    s32 x, y;

    sMapImage = 0;
    switch (gGame.mode) {
    case GAME_MODE_SOCCER:
    case 3:
    case 4:
    case GAME_MODE_DESERT:
    case GAME_MODE_EXCITE_3D:
    case GAME_MODE_TUTORIAL:
    case GAME_MODE_STUNT_COURSE:
    case GAME_MODE_TEST_LOOP:
    case GAME_MODE_TEST_OPEN:
        break;
    default:
        if ((gGame.flags & GAME_FLAG_REPLAY) == 0) {
            for (i = 0; i < gPathNodeCount; i++) {
                if (gPathNodes[i].pos.x < sMapBoundsMin.x || i == 0) {
                    sMapBoundsMin.x = gPathNodes[i].pos.x;
                }
                if (gPathNodes[i].pos.z < sMapBoundsMin.z || i == 0) {
                    sMapBoundsMin.z = gPathNodes[i].pos.z;
                }
                if (gPathNodes[i].pos.x > sMapBoundsMax.x || i == 0) {
                    sMapBoundsMax.x = gPathNodes[i].pos.x;
                }
                if (gPathNodes[i].pos.z > sMapBoundsMax.z || i == 0) {
                    sMapBoundsMax.z = gPathNodes[i].pos.z;
                }
            }
            /* sMapPos: the corner per number of views, in 320x240 units */
            x = sMapPos[(gNumViews - 1) * 2];
            screenW = gScreenWidth;
            screenH = gScreenHeight;
            screenW2 = gScreenWidth;
            screenH2 = gScreenHeight;
            y = sMapPos[(gNumViews - 1) * 2 + 1];
            sMapW = screenW2 / 5;
            sMapX = (x * screenW) / 320;
            sMapH = (screenH2 * 64) / 240;
            sMapY = (y * screenH) / 240;
            if (gGame.mode == GAME_MODE_HILL_CLIMB) {
                sMapW = sMapW + sMapW / 2;
                sMapH = sMapH + sMapH / 2;
            }
            if (gViMode == VI_MODE_HI_RES) {
                sMapY = sMapY + 0x34;
            }
            /* texture steps that fit the 64-texel image into the rectangle */
            sMapStepS = 0x10000 / sMapW;
            sMapStepT = 0x10000 / sMapH;
            map_build_image();
        }
        break;
    }
}

typedef struct {
    Quad r[4];
} Ent40_3;

extern Ent40_3 gViewRects_11[];

/* With several views: a bar along the split with a marker per rider at its progress around the lap. */
Gfx *map_draw_progress_bar(Gfx *gdl)
{
    s32 i;
    s32 numViews = gNumViews;

    if (gSplitVertical == 0) {
        sMapBarStart = gViewRects_11[numViews - 1].r[0].a + 4;
        sMapBarEnd = gViewRects_11[numViews - 1].r[1].a + gViewRects_11[numViews - 1].r[1].c - 8;
    } else {
        sMapBarStart = gViewRects_11[numViews - 1].r[0].b + 6;
        sMapBarEnd = gViewRects_11[numViews - 1].r[numViews - 1].b + gViewRects_11[numViews - 1].r[numViews - 1].d - 12;
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
        gDPLoadTextureBlock(gdl++, sMapBarRow, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, (gScreenWidth * 252 / 512) << 2, sMapBarStart << 2,
                            (gScreenWidth * 260 / 512) << 2, sMapBarEnd << 2,
                            G_TX_RENDERTILE, 0, 0, 0x80000 / gScreenWidth, 0);
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);

    /* the CPU riders first, then the players on top; crashed riders blink */
    for (i = 0; i < 6; i++) {
        if (gRiders[i].inactive == 0) {
            if (gRiders[i].crashState == 0 || gRaceFrame % 8 >= 4) {
                if (gRiders[i].isCpu != 0) {
                    gdl = map_draw_progress_marker(gdl, i, 0);
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (gRiders[i].inactive == 0) {
            if (gRiders[i].crashState == 0 || gRaceFrame % 8 >= 4) {
                if (gRiders[i].isCpu == 0) {
                    gdl = map_draw_progress_marker(gdl, i, 1);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    return gdl;
}

/* The SDK's macros, written again: their own copy keeps the original's code */
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

/* A rider's dot on the minimap, in the rider's colour; big for the players. */
Gfx *map_draw_rider_dot(Gfx *gdl, s32 rider, s32 big)
{
    s32 dotX;
    s32 dotY;
    s32 cx;
    s32 cy;
    Gfx *p;

    map_get_rider_pos(rider, &dotX, &dotY);
    gDPPipeSync_2(gdl++);
    gDPSetPrimColor_2(gdl, 0, 0, gRiderGfx[rider].r, gRiderGfx[rider].g, gRiderGfx[rider].b, 255);
    p = gdl + 1;
    cx = dotX;
    cy = dotY;
    if (big) {
        gDPFillRectangle_2(p++, cx - 4, cy - 3, cx + 4, cy + 3);
    } else {
        gDPFillRectangle_2(p++, cx - 2, cy - 2, cx + 2, cy + 2);
    }
    return p;
}

/* A rider's marker on the progress bar, placed by its lap progress (interpolated along the segment). */
Gfx *map_draw_progress_marker(Gfx *gdl, s32 index, s32 big)
{
    Rider *rider;
    Rider *riders;
    PathNode *node;
    PathNode *next;
    s32 pos;
    s32 centerX;
    s32 nodeIdx;
    f32 t;

    riders = gRiders;
    rider = &riders[index];
    nodeIdx = rider->segment;
    node = &gPathNodes[nodeIdx];
    pos = node->lapProgress;
    nodeIdx = node->next;
    next = &gPathNodes[nodeIdx];
    if (next->lapProgress < node->lapProgress) {
        t = rider->segmentT;
        if (t < 0.0f) {
            t = 0.0f;
        }
        if (1.0f < t) {
            t = 1.0f;
        }
        pos = (s32)((f32)pos + t * (f32)(next->lapProgress - pos));
    } else {
        pos = next->lapProgress;
    }
    if (gSplitVertical != 0) {
        pos -= 0x3DD3;
        pos = (pos * 0x8000) / 0x422D;
    }
    pos = 0x8000 - pos;
    if (pos < 0) {
        pos = 0;
    }
    if (pos > 0x8000) {
        pos = 0x8000;
    }
    if (gSplitVertical != 0) {
        pos = 0x7FFF - pos;
    }
    pos = (pos * (sMapBarEnd - sMapBarStart)) >> 15;
    pos += sMapBarStart;
    if (gSplitVertical == 0) {
        Gfx *g = gdl;

        gDPSetPrimColor(g++, 0, 0, gRiderGfx[index].r, gRiderGfx[index].g, gRiderGfx[index].b, 255);
        if (big) {
            gDPFillRectangle(g++, pos - 4, 117, pos + 4, 123);
        } else {
            gDPFillRectangle(g++, pos - 2, 118, pos + 2, 122);
        }
        gdl = g;
    } else {
        Gfx *g;

        centerX = gScreenWidth / 2;
        if (pos > 0xD5) {
            pos = 0xD5;
        }
        g = gdl;
        gDPSetPrimColor(g++, 0, 0, gRiderGfx[index].r, gRiderGfx[index].g, gRiderGfx[index].b, 255);
        if (big) {
            gDPFillRectangle(g++, centerX - 4, pos - 3, centerX + 4, pos + 3);
        } else {
            gDPFillRectangle(g++, centerX - 2, pos - 2, centerX + 2, pos + 2);
        }
        gdl = g;
    }
    return gdl;
}

/* A display list packet from its two words */
#define WD(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

/* Draws the minimap and the riders' dots (or, with several views, the progress bar). */
Gfx *map_draw_minimap(Gfx *gdl, Dyn *dyn, s32 fb) {
    Rider *riders;
    s32 i;

    if (gHudLevel != 0 || sMapImage == 0 || (gGame.flags & (GAME_FLAG_FRONT_END_RACE | GAME_FLAG_TRACK_INTRO)) ||
        gInsetViewActive != 0) {
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
    gDPLoadTextureBlock(gdl++, sMapImage->tex, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTextureRectangle(gdl++, sMapX * 4, sMapY * 4,
                        (sMapX + sMapW) * 4, (sMapY + sMapH) * 4,
                        G_TX_RENDERTILE, 0, 0, ((u16 *)&sMapStepS)[1], ((u16 *)&sMapStepT)[1]);
    gDPPipeSync(gdl++);
    WD(gdl++, 0xE200001C, 0x0C084000);
    WD(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);

    riders = gRiders;
    for (i = 0; i < 6; i++) {
        if (gRiders[i].inactive != 0) continue;
        if (gRiders[i].crashState != 0 && gRaceFrame % 8 < 4) continue;
        if (gRiders[i].isCpu != 0 || (gGame.mode == GAME_MODE_TIME_TRIAL && i != gTimeTrialPlayer)) {
            gdl = map_draw_rider_dot(gdl, i, 0);
        }
    }
    riders = gRiders;
    for (i = 0; i < 6; i++) {
        if (gRiders[i].inactive != 0) continue;
        if (gRiders[i].crashState != 0 && gRaceFrame % 8 < 4) continue;
        if (gRiders[i].isCpu != 0) continue;
        if (gGame.mode == GAME_MODE_TIME_TRIAL && i != gTimeTrialPlayer) continue;
        gdl = map_draw_rider_dot(gdl, i, 1);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct { f32 x, y; } V2;

/* A rider's position on screen: along its segment of the racing line, through the minimap's scale. */
void map_get_rider_pos(s32 index, s32 *px, s32 *py)
{
    Rider *rider;
    Rider *riders;
    PathNode *node;
    PathNode *next;
    V2 posA, posB;
    f32 centerX, centerZ, t;
    s32 mapX, mapY, width, height;
    f32 mz, wx, wz, dx, dz;
    s32 sx, sy;

    riders = gRiders;
    rider = &riders[index];
    centerX = (sMapBoundsMin.x + sMapBoundsMax.x) * 0.5f;
    centerZ = (sMapBoundsMin.z + sMapBoundsMax.z) * 0.5f;
    node = &gPathNodes[rider->segment];
    next = &gPathNodes[node->next];
    map_get_node_pos(node, &posA.x, &posA.y);
    map_get_node_pos(next, &posB.x, &posB.y);
    t = rider->segmentT;
    if (t < 0.0f) {
        t = 0.0f;
    }
    if (1.0f < t) {
        t = 1.0f;
    }
    dx = t * (posB.x - posA.x);
    dz = t * (posB.y - posA.y);
    wx = posA.x + dx;
    t = (wx - centerX) * (f32)sMapImage->scaleX;
    wz = posA.y + dz;
    mz = (wz - centerZ) * (f32)sMapImage->scaleZ;
    t *= 6.1035156e-05f; /* 1 / 0x4000 */
    mz *= 6.1035156e-05f;
    width = sMapW;
    mapX = (s32)t;
    sx = mapX * width;
    posA.x = wx;
    posA.y = wz;
    mapY = (s32)mz;
    mapY = -mapY;
    if (sx < 0) {
        sx += 0x7F;
    }
    mapX = sx >> 7;
    height = sMapH;
    sy = mapY * height;
    if (sy < 0) {
        sy += 0x7F;
    }
    mapY = sy >> 7;
    mapX += sMapX + width / 2;
    mapY += sMapY + height / 2;
    *px = mapX;
    *py = mapY;
}

/* map_get_node_pos written inline: the middle of the track at a node. */
static inline void getpos(PathNode *node, f32 *x, f32 *z) {
    f32 offset = (node->widthNeg - node->widthPos) * 0.5f;
    *x = node->pos.x - node->lateralX * offset;
    *z = node->pos.z - node->lateralZ * offset;
}

/* Distance from the centre of the 16x16 brush. */
static inline f32 dist(f32 dx, f32 dy) {
    dx -= 7.5f;
    dy -= 7.5f;
    return __builtin_sqrtf(dx * dx + dy * dy);
}

/* map_stamp_brush written inline: keeps the smaller distance of brush and map, the brush centred at (cx, cy). */
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

/* Draws the racing line into a 128x128 map of distances (a round brush stamped along each segment, and a bar
   across the start), shades it through sMapShadeRamp and halves it into the 64x64 IA8 texture. */
void map_build_image(void) {
    f32 px, pz;
    f32 centerX, centerZ;
    f32 scaleX, scaleZ, aspect, len, dx, dz;
    s32 nextY;
    s32 i, j, steps, x0, y0, t, nextX, cy, src, y, k;
    u16 *brush;
    u16 *map;
    PathNode *node;

    for (i = 0; i < 64; i++) {
        sMapBarTex[i] = sMapBarRow[i / 8];
    }
    sMapImage = mem_alloc_dbg(0x1003, 0, (s32)sStrMapC, 0x23E);
    for (i = 0; i < gPathNodeCount; i++) {
        if (gPathNodes[i].pos.x < sMapBoundsMin.x || i == 0) sMapBoundsMin.x = gPathNodes[i].pos.x;
        if (gPathNodes[i].pos.z < sMapBoundsMin.z || i == 0) sMapBoundsMin.z = gPathNodes[i].pos.z;
        if (gPathNodes[i].pos.x > sMapBoundsMax.x || i == 0) sMapBoundsMax.x = gPathNodes[i].pos.x;
        if (gPathNodes[i].pos.z > sMapBoundsMax.z || i == 0) sMapBoundsMax.z = gPathNodes[i].pos.z;
    }
    centerX = (sMapBoundsMin.x + sMapBoundsMax.x) * 0.5f;
    centerZ = (sMapBoundsMin.z + sMapBoundsMax.z) * 0.5f;
    scaleX = 118.0f / (sMapBoundsMax.x - sMapBoundsMin.x);
    scaleZ = 118.0f / (sMapBoundsMax.z - sMapBoundsMin.z);
    if (scaleX < scaleZ) scaleZ = scaleX; else scaleX = scaleZ;
    aspect = scaleX / scaleZ;
    if (aspect > 0.8f && aspect < 1.2f) { /* always true: the two scales were just made equal */
        scaleX /= 1.3f;
        scaleZ /= 1.3f;
    }
    sMapImage->scaleX = scaleX * 16384.0f;
    sMapImage->scaleZ = scaleZ * 16384.0f;
    steps = scaleX * 6.0f;
    if (steps < 2) steps = 2;

    /* the brush and the map are built in the Z buffer */
    brush = gZBuffer_2;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            brush[i * 16 + j] = (u32)(dist(i, j) * 256.0f);
        }
    }
    map = gZBuffer_2 + 0x100;
    mem_fill((u8 *)map, 0xFF, 0x8000);

    for (i = 0; i < gPathNodeCount; i++) {
        node = (PathNode *)(i * sizeof(PathNode) + (u32)gPathNodes);
        j = node->next;
        if (j == 0 && gGame.mode == GAME_MODE_HILL_CLIMB) break; /* not a loop */
        getpos(node, &px, &pz);
        x0 = (px - centerX) * scaleX + 64.0f;
        y0 = 64.0f - (pz - centerZ) * scaleZ;
        getpos(&gPathNodes[j], &px, &pz);
        nextX = (px - centerX) * scaleX + 64.0f;
        nextY = 64.0f - (pz - centerZ) * scaleZ;
        for (t = 0; t < steps; t++) {
            stamp(brush, map, x0 + t * (nextX - x0) / steps, y0 + t * (nextY - y0) / steps);
        }
    }

    /* the start line: the brush stamped across the track at the starting grid */
    j = gPathNodes[gPathStartNode].next;
    x0 = (gPathNodes[gPathStartNode].pos.x - centerX) * scaleX + 64.0f;
    y0 = 64.0f - (gPathNodes[gPathStartNode].pos.z - centerZ) * scaleZ;
    px = gPathNodes[j].pos.x - gPathNodes[gPathStartNode].pos.x;
    pz = gPathNodes[j].pos.z - gPathNodes[gPathStartNode].pos.z;
    len = __builtin_sqrtf(px * px + pz * pz);
    if (len > 0.0f) {
        px /= len;
        pz /= len;
    }
    pz = -pz;
    dz = pz;
    dx = px;
    for (i = -8; i < 9; i++) {
        nextX = x0 + (s32)(i * dz);
        cy = y0 - (s32)(i * dx);
        {
            s32 xx, yy;
            t = 0;
            for (yy = cy - 8; yy < cy + 8; yy++) {
                for (xx = nextX - 8; xx < nextX + 8; xx++) {
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
        map[i] = sMapShadeRamp[t];
    }
    k = src = 0;
    for (i = 0; i < 128; i += 2) {
        for (j = 0; j < 128; j += 2) {
            u16 a = map[src], b = map[src + 1], cc = map[src + 128], d = map[src + 129];
            s32 lo;
            src += 2;
            t = (a >> 4) + (b >> 4) + (cc >> 4) + (d >> 4);
            lo = (a & 15) + (b & 15) + (cc & 15) + (d & 15);
            t >>= 2;
            lo >>= 2;
            sMapImage->tex[k] = (t << 4) | lo;
            k++;
        }
        src += 128;
    }
}

/* The middle of the track at a node (the racing line is offset from it by the half-widths). */
void map_get_node_pos(PathNode *node, f32 *x, f32 *z)
{
    f32 offset = (node->widthNeg - node->widthPos) * 0.5f;
    *x = node->pos.x - node->lateralX * offset;
    *z = node->pos.z - node->lateralZ * offset;
}

/* A rider's dot at (x, y) in its colour; not called. */
Gfx *map_draw_dot(Gfx *gdl, s32 index, s32 big, s32 x, s32 y) {
    gDPSetPrimColor(gdl++, 0, 0, gRiderGfx_11[index][0x21C], gRiderGfx_11[index][0x21D],
                    gRiderGfx_11[index][0x21E], 255);
    if (big != 0) {
        gDPFillRectangle(gdl++, x - 4, y - 3, x + 4, y + 3);
    } else {
        gDPFillRectangle(gdl++, x - 2, y - 2, x + 2, y + 2);
    }
    return gdl;
}

/* Stamps the 16x16 brush `src` into the 128x128 map `dst` centred at (x, y), keeping the smaller values; not
   called (map_build_image has its own copy). */
void map_stamp_brush(u16 *dst, u16 *src, s32 x, s32 y) {
    s32 row;
    s32 col;
    s32 k;
    u16 *px;

    k = 0;
    for (row = y - 8; row < y + 8; row++) {
        col = x - 8;
        if (col < x + 8) {
        for (; col < x + 8; col++) {
            if (row >= 0 && col >= 0 && row < 0x80 && col < 0x80) {
                px = (u16 *)((s32)(row * 0x80 + col) * 2 + (s32)dst);
                if (src[k] < *px) {
                    *px = src[k];
                }
            }
            k++;
        }
        }
    }
}

/* Frees the minimap image. */
void map_free(void) {
    if (sMapImage != 0) {
        mem_free(sMapImage);
        sMapImage = 0;
    }
}
