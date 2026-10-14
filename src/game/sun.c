#include "game.h"

/* gCurTrack as this file sees it */
typedef struct {
    u8 pad[0x2C];
    s32 envType;
    u8 pad2[0x20];
    s32 flags;
} Cfg_6;

extern Cfg_6 *gCurTrack_23;

/* Loads the track's flare list (the desert has its own) and the glare textures, and on night tracks scatters 64
   stars over a dome. */
void sun_init(void)
{
    s32 resId;
    s32 i, j, k, n, idx;
    s32 x, z;

    sStars = 0;
    sSunVtxBufs[0] = 0;
    sSunVtxBufs[1] = 0;
    sSunList = 0;
    if (gCurTrack_23->envType == TRACK_ENV_DESERT) {
        resId = sDesertLightDefResId;
    } else {
        resId = track_res_find(gGidLightDef);
    }
    if (resId < 0) {
        sSunList = 0;
        return;
    }
    idx = 0;
    sSunList = (FlareList *)res_load(gResourceSet, resId);
    for (; idx < sSunList->count; idx++) {
        sSunList->e[idx].flags &= ~(FLARE_ON_SCREEN | FLARE_SAMPLED);
    }
    for (j = 0; j < 2; j++) {
        sSunVtxBufs[j] = mem_alloc_dbg(SUN_NUM_VTX * sizeof(Vtx), 0, sStrSunC, 0xCB);
        for (n = 0; n < SUN_NUM_VTX; n++) {
            sSunVtxBufs[j][n].v.ob[2] = 1000;
        }
    }
    sFlareTextures = (TexSet *)res_load(gResourceSet, sSunList->texRes);
    for (idx = 0; idx < sFlareTextures->count; idx++) {
        n = sFlareTextures->tex[idx].width * sFlareTextures->tex[idx].height * 8;
        for (k = 0; k < n; k++) {
            sFlareTextures->tex[idx].data[k] |= 0xF0; /* full intensity: the glare's shape is its alpha */
        }
    }
    if (gCurTrack_23->flags & TRACK_FLAG_STARS) {
        sStarTexture = (TexSet *)res_load(gResourceSet, sStarTexResId);
        sStars = mem_alloc_dbg(SUN_NUM_STARS * sizeof(Vec3i), 0, sStrSunC, 0xE3);
        for (i = 0; i < SUN_NUM_STARS; i++) {
            x = (random_byte() - 0x80) * 16;
            z = (random_byte() - 0x80) * 16;
            sStars[i].y = isqrt_u32_4(0x400000 - (x * x + z * z)); /* on a sphere of radius 2048 */
            sStars[i].x = x;
            sStars[i].z = z;
        }
    }
    gSunGlare = 0;
}

/* Projects each flare to the screen and marks those on it (with a margin of 50 pixels). */
void sun_update_screen_pos(s32 dyn, s32 view, s32 fb)
{
    s32 sx, sy, sz, depth, depth2;
    s32 i;
    Flare *flare;
    u32 flags;

    if (sSunList != NULL) {
        for (i = 0; i < sSunList->count; i++) {
            flare = &sSunList->e[i];
            flags = flare->flags;
            flare->flags = flags & ~FLARE_ON_SCREEN;
            if (flags & FLARE_SUN) {
                if (sky_get_sun_screen_pos(&sx, &sy, dyn, view) == 0) {
                    flare->sz = 0x7FFF; /* at infinity */
                    flare->flags |= FLARE_ON_SCREEN;
                    flare->sx = sx;
                    flare->sy = sy;
                }
            } else if (flags & FLARE_FIXED) {
                if (project_to_screen(0, 20, -300, &sx, &sy, &sz, &depth, dyn, 0) == 0) {
                    flare->sz = 0x7FFF;
                    flare->flags |= FLARE_ON_SCREEN;
                    flare->sx = sx;
                    flare->sy = sy;
                }
            } else {
                if (project_to_screen(flare->x, flare->y, flare->z, &sx, &sy, &sz, &depth2, dyn, view) == 0) {
                    flare->sx = sx;
                    flare->sy = sy;
                    flare->sz = sz;
                    flare->flags |= FLARE_ON_SCREEN;
                }
            }
            if (flare->sx < -50 || flare->sy < -50 ||
                gScreenWidth + 50 < flare->sx || gScreenHeight + 50 < flare->sy) {
                flare->flags &= ~FLARE_ON_SCREEN;
            }
        }
    }
}

/* After the frame: samples 8x8 Z buffer pixels around each flare on screen and sets how much of it shows
   (visible: -1 hidden to 5). */
void sun_check_occlusion(s32 unused)
{
    s32 i, row, col;
    s32 x, y, minDepth, z, visibleCount, samples;
    s32 viewX, viewY, viewW, viewH, xe, ye;
    Flare *flare;
    u16 *zpix;

    if (sSunList == NULL) {
        return;
    }
    osInvalDCache(0, 0x400000); /* read the Z buffer as the RDP left it */
    viewW = gViewRects[0][0].w;
    viewH = gViewRects[0][0].h;
    viewX = gViewRects[0][0].x;
    viewY = gViewRects[0][0].y;
    i = 0;
    if (i < sSunList->count) {
        do {
            flare = &sSunList->e[i];
            flare->visible = -1;
            if (!(flare->flags & FLARE_ON_SCREEN)) {
                flare->flags &= ~FLARE_SAMPLED;
            } else {
                samples = 0;
                visibleCount = 0;
                x = flare->sx - 4;
                y = flare->sy - 4;
                minDepth = flare->sz - 0x19;
                for (row = 0; row < 8; row++) {
                    if (y < viewY) {
                        y = viewY;
                    }
                    if (y >= viewY + viewH) {
                        y = viewY + viewH - 1;
                    }
                    for (col = 0; col < 8; col++) {
                        if (x < viewX) {
                            x = viewX;
                        }
                        if (x >= viewX + viewW) {
                            x = viewX + viewW - 1;
                        }
                        zpix = &gZBuffer_2[y * gScreenWidth + x];
                        z = zbuffer_decode(*zpix);
                        if (z > 0x7FFF) {
                            z = 0x7FFF;
                        }
                        if (z >= minDepth) {
                            visibleCount++;
                        }
                        samples++;
                        x++;
                    }
                    x -= 8;
                    y++;
                }
                if (samples != 0) {
                    visibleCount = visibleCount * 7 / samples;
                }
                visibleCount--;
                if (visibleCount > 5) {
                    visibleCount = 5;
                }
                flare->visible = visibleCount;
                flare->flags |= FLARE_SAMPLED;
            }
            i++;
        } while (i < sSunList->count);
    }
}

/* Cosine and sine of a 16-bit angle, 1.0 = 0x4000 */
#define COS16(a) gCosTable[((a) >> 4) & 0xFFF]
#define SIN16(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

/* sun_alloc_vtx written inline */
static inline Vtx *allocVtx(s32 fb, s32 count) {
    s32 used = sSunVtxUsed[fb];
    if (used + count > SUN_NUM_VTX) {
        return NULL;
    }
    sSunVtxUsed[fb] = used + count;
    return &sSunVtxBufs[fb][used];
}

/* Draws the lens flares: for each visible flare, four hexagonal rings along the line from it through the centre
   of the screen, then its glare sprite (turning on stadium tracks). The sun's brightness becomes gSunGlare. */
Gfx *sun_draw_flare(Gfx *gdl, u8 *dyn, s32 fb) {
    s32 i, j, k;
    Flare *flare;
    Vtx *ringVtx;
    TexEntry *tex;
    s32 alpha, b, stepX, stepY, x, y, angle, dx, dy, m, n, w;
    s32 texS;
    s32 texT;
    Vtx *quadVtx;

    if (sSunList == NULL) {
        return gdl;
    }
    sSunVtxUsed[fb] = 0;
    if (gCheatFlags & CHEAT_NIGHT) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPViewport(gdl++, &gScreenViewport);
    /* Dyn.m3, the orthographic projection */
    gSPMatrix(gdl++, dyn + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y, gViewRects[0][0].x + gViewRects[0][0].w - 1,
                  gViewRects[0][0].y + gViewRects[0][0].h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);

    gSunGlare = 0;
    for (i = 0; i < sSunList->count; i++) {
        flare = &sSunList->e[i];
        if (flare->flags & FLARE_ON_SCREEN) { if (flare->flags & FLARE_SAMPLED) { if (flare->visible >= 0) {
            /* brighter the nearer the centre of the screen and the more of it shows */
            stepX = gScreenWidth / 2 - flare->sx;
            stepY = gScreenHeight / 2 - flare->sy;
            alpha = 0x80 - isqrt_u32_2(stepX * stepX + stepY * stepY);
            if (alpha >= 0) {
                alpha = alpha * flare->visible / 6;
                if (alpha != 0) {
                    alpha = alpha * 192 / 128;
                    if (i == 0) {
                        gSunGlare = alpha;
                    }
                    ringVtx = allocVtx(fb, 28); /* 4 rings of a centre and 6 corners */
                    if (ringVtx != NULL) {
                        x = flare->sx - gScreenWidth / 2;
                        y = flare->sy - gScreenHeight / 2;
                        stepX = x / 3;
                        stepY = y / 3;
                        x -= stepX;
                        y -= stepY;
                        for (j = 0; j < 4; j++) {
                            ringVtx[j * 7].v.ob[0] = x;
                            ringVtx[j * 7].v.ob[1] = -y;
                            ringVtx[j * 7].v.ob[2] = 50;
                            for (k = 0; k < 6; k++) {
                                b = -y;
                                angle = ((k * 2 + 1) << 15) / 6;
                                ringVtx[j * 7 + k + 1].v.ob[0] = x + SIN16(angle) * sFlareSizes[j] / 16384;
                                ringVtx[j * 7 + k + 1].v.ob[1] = b + COS16(angle) * sFlareSizes[j] / 16384;
                                ringVtx[j * 7 + k + 1].v.ob[2] = 50;
                            }
                            x -= stepX;
                            y -= stepY;
                            if (j & 1) {
                                x -= stepX;
                                y -= stepY;
                            }
                        }
                        gSPVertex(gdl++, ringVtx, 28, 0);
                        for (j = 0; j < 4; j++) {
                            gDPPipeSync(gdl++);
                            gDPSetPrimColor(gdl++, 0, 0, sFlareColors[j * 3], sFlareColors[j * 3 + 1],
                                            sFlareColors[j * 3 + 2], alpha);

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
    if (gCurTrack_4->f2C == TRACK_ENV_STADIUM) {
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
    } else {
        gDPSetTexturePersp(gdl++, G_TP_NONE);
    }

    for (i = 0; i < sSunList->count; i++) {
        flare = &sSunList->e[i];
        if (flare->flags & FLARE_ON_SCREEN) { if (flare->flags & FLARE_SAMPLED) { if ((n = flare->visible) >= 0) {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            tex = &sFlareTextures->tex[n];
            if (gCurTrack_4->f2C == TRACK_ENV_STADIUM) {
                /* a quad turned by the position on screen */
                x = flare->sx - gScreenWidth / 2;
                y = gScreenHeight / 2 - flare->sy;
                stepY = tex->height - 1;
                dx = (SIN16((x + y) * 32) * stepY >> 14) * 2 / 3;
                dy = (COS16((x + y) * 32) * stepY >> 14) * 2 / 3;
                quadVtx = allocVtx(fb, 4);
                if (quadVtx != NULL) {
                    quadVtx[0].v.ob[0] = x + dx;
                    quadVtx[0].v.ob[1] = y + dy;
                    quadVtx[0].v.tc[0] = 0;
                    quadVtx[0].v.tc[1] = 0;
                    quadVtx[1].v.ob[0] = x + dy;
                    quadVtx[1].v.ob[1] = y - dx;
                    quadVtx[1].v.tc[0] = stepY << 6;
                    quadVtx[1].v.tc[1] = 0;
                    quadVtx[2].v.ob[0] = x - dx;
                    quadVtx[2].v.ob[1] = y - dy;
                    quadVtx[2].v.tc[0] = stepY << 6;
                    quadVtx[2].v.tc[1] = stepY << 6;
                    quadVtx[3].v.ob[0] = x - dy;
                    quadVtx[3].v.ob[1] = y + dx;
                    quadVtx[3].v.tc[0] = 0;
                    quadVtx[3].v.tc[1] = stepY << 6;
                    gSPVertex(gdl++, quadVtx, 4, 0);
                    gDPLoadTextureBlock(gdl++, tex->data, G_IM_FMT_IA, G_IM_SIZ_8b, tex->width * 8, tex->height, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
                }
            } else {
                /* a texture rectangle, clipped to the screen */
                texS = texT = 0;
                x = flare->sx - tex->width * 4;
                y = flare->sy - tex->height / 2;
                stepX = x + tex->width * 8;
                stepY = y + tex->height;
                if (x < 0) {
                    texS = -x;
                    x = 0;
                }
                if (y < 0) {
                    texT = -y;
                    y = 0;
                }
                if (stepX >= gScreenWidth) {
                    stepX = gScreenWidth - 1;
                }
                if (stepY >= gScreenHeight) {
                    stepY = gScreenHeight - 1;
                }
                if (x < stepX && y < stepY) {
                    gDPLoadTextureBlock(gdl++, tex->data, G_IM_FMT_IA, G_IM_SIZ_8b, tex->width * 8, tex->height, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSPTextureRectangle(gdl++, x << 2, y << 2, stepX << 2, stepY << 2, G_TX_RENDERTILE, texS << 5, texT << 5,
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
    gSPMatrix(gdl++, dyn, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

/* Draws the stars of a night track, each a 2x2 sprite a little brighter than the last. */
Gfx *sun_draw_stars(Gfx *gdl, s32 mtx)
{
    s32 grey, i;
    u8 *img;
    s32 sx, sy, sz, w;

    if (sSunList == 0) return gdl;
    if (*(s32 *)(gCurTrack + 0x50) & TRACK_FLAG_STARS) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        { Gfx *_g = gdl++; _g->words.w0 = 0xFC11FE23; _g->words.w1 = 0xFFFFFFFF; }
        img = sStarTexture->tex[0].data + 0x28;
        gDPLoadTextureBlock(gdl++, img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0, grey = 0x80; i < SUN_NUM_STARS; i++) {
            if (project_to_screen(sStars[i].x + (gCamPos[0].x >> 4), sStars[i].y + (gCamPos[0].y >> 4),
                              sStars[i].z + (gCamPos[0].z >> 4), &sx, &sy, &sz, &w, mtx, 0) == 0
                && sx >= 0 && sy >= 0 && sx <= gScreenWidth && sy <= gScreenHeight) {
                gDPSetPrimColor(gdl++, 0, 0, grey, grey, grey, 255);
                gSPTextureRectangle(gdl++, (sx - 1) << 2, (sy - 1) << 2, (sx + 1) << 2, (sy + 1) << 2,
                                    0, 0, 0, 0x400, 0x400);
            }
            grey += 2;
        }
    }
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    return gdl;
}

/* Projects (x, y, z) through view's matrix of the array mtx and its viewport: screen x, y, the Z buffer depth
   and the clip w. Returns 0 when the point is in front of the camera, 1 when it is not. */
s32 project_to_screen(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *w, s32 mtx, s32 view)
{
    f32 m[4][4];
    s16 *vp;
    f32 scaleX, scaleY, transY, transX;
    f32 fx, fy, fz, cx, cy, cz, cw;
    s32 ret;

    /* the viewport's scale and translation (2 fraction bits) */
    vp = (s16 *)(view * 16 + gViewports_2);
    scaleX = (f32)vp[0] * 0.25f;
    scaleY = (f32)vp[1] * 0.25f;
    transX = (f32)vp[4] * 0.25f;
    transY = (f32)vp[5] * 0.25f;
    guMtxL2F(&m[0][0], (u32 *)(mtx + view * 0x40));

    fx = (f32)x;
    fy = (f32)y;
    fz = (f32)z;
    cx = m[0][0] * fx + m[1][0] * fy + m[2][0] * fz + m[3][0];
    cy = m[0][1] * fx + m[1][1] * fy + m[2][1] * fz + m[3][1];
    cz = m[0][2] * fx + m[1][2] * fy + m[2][2] * fz + m[3][2];
    cw = m[0][3] * fx + m[1][3] * fy + m[2][3] * fz + m[3][3];
    ret = 1;
    if (cw != 0.0f) {
        cx = cx / cw;
        cy = cy / cw;
        cz = cz / cw;
        if (cz <= -1.0f) {
            return ret;
        }
        if (1.0f <= cz) {
            return ret;
        }
        *sx = (s32)(cx * scaleX + transX);
        *sy = (s32)(cy * -scaleY + transY);
        *sz = (s32)(cz * 1023.0f * 0.5f + 511.5f) << 5;
        if (w != 0) {
            *w = (s32)cw;
        }
        ret = 0;
    }
    return ret;
}

/* Frees the stars and the vertex buffers. */
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

/* A Z buffer pixel (the RDP's 3-bit exponent, 11-bit mantissa) to a linear depth. */
s32 zbuffer_decode(s32 x) {
    s32 off = x >> 11;
    s32 v = (x >> 2) & 0x7FF;
    u8 *p1 = sZDecodeShift;
    s32 sh = *(s32 *)(p1 + (off & 0x1C));
    s32 add = *(s32 *)(sZDecodeAdd + (off & 0x1C));
    return ((v << sh) + add) >> 3;
}

/* Takes count vertices from frame fb's buffer, or NULL when it is full. */
Vtx *sun_alloc_vtx(s32 count, s32 fb) {
    s32 n = sSunVtxUsed[fb] + count;
    if (n > SUN_NUM_VTX) {
        return 0;
    }
    sSunVtxUsed[fb] = n;
    return &sSunVtxBufs[fb][n - count];
}
