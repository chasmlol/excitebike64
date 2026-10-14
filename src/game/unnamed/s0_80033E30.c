#include "game.h"

/* gSkyMode, chosen by render_init from the track's resources. */
#define SKY_MODE_CLEAR          0   /* no sky: the views are cleared to the fog colour */
#define SKY_MODE_IMAGE          1   /* the 2D backdrop drawn here (GID_PkgSky) */
#define SKY_MODE_MODEL          2   /* a sky model (GID_ArnSky) */
#define SKY_MODE_MODEL_TEXTURED 3   /* a sky model textured with the sky image */

/* The sky image is 256 x 256 CI8 texels, loaded and drawn in strips of 8 rows; this is the last strip's row. */
#define SKY_LAST_STRIP  0xF8

/* A Z buffer value above this is still the clear value: nothing was drawn there. */
#define ZBUFFER_FAR     0xFFEF

/* The clip height written through a one-field struct: as a plain s32 the store is scheduled differently. */
typedef struct { s32 v; } ClipStore;

/* A track record of gTracks (0xA0 bytes) as far as this file reads it. */
typedef struct { u8 pad[0x2C]; s32 envType; } SkyTrack;

extern SkyTrack *gCurTrack_24;

/* After each frame of a race (gGame flag 0x10000), finds for each view how far down the sky shows: on every 16th
   row and 8th column of the Z buffer, the lowest row still at the clear value. The backdrop is drawn only down to
   there (sSkyClipHeight), which rises by at most 16 lines per frame; on stadium tracks it covers the whole view. */
void sky_scan_zbuffer(s32 frame)
{
    s32 i, x, y, layout;
    Rect *rect;
    Rect *rect2;
    ClipStore *clip;

    if ((gGame.flags & 0x10000) == 0) {
        return;
    }
    layout = gNumViews - 1;
    for (i = 0; i < gNumViews; i++) {
        if (gCurTrack_24->envType == 0) {
            sSkyClipHeight[i] = gViewRects[layout][i].h;
        } else {
            sSkyClipHeight[i] = 0;
            y = gViewRects[layout][i].y;
            /* rect and rect2 add the row and column offsets to the base in one go (&gViewRects[layout][i] does not
               compile the same) */
            rect = (Rect *)((u8 *)gViewRects + (layout * 64 + i * 16));
            if (y < rect->y + rect->h) {
            next_row:
                clip = (ClipStore *)&sSkyClipHeight[i];
                for (x = rect->x; x < rect->x + rect->w; x += 8) {
                    if (*(u16 *)((u32)gZBuffer + ((y * gScreenWidth + x) << 1)) > ZBUFFER_FAR) {
                        clip->v = y - rect->y;
                    }
                }
                y += 16;
                if (y < rect->y + rect->h) goto next_row;
            }
            sSkyClipHeight[i] += 16;
            if (sSkyPrevClipHeight[i] > 16 && sSkyClipHeight[i] < sSkyPrevClipHeight[i] - 16) {
                sSkyClipHeight[i] = sSkyPrevClipHeight[i] - 16;
            }
            rect2 = (Rect *)((u8 *)gViewRects + (layout * 64 + i * 16));
            if (sSkyClipHeight[i] > rect2->h) {
                sSkyClipHeight[i] = rect2->h;
            }
            if (sSkyClipHeight[i] < rect2->h / 2) {
                sSkyClipHeight[i] = rect2->h;
            }
        }
        sSkyPrevClipHeight[i] = sSkyClipHeight[i];
    }
}

/* Scrolls the backdrop with each view's camera: the image repeats every eighth of a turn of yaw (0x2000), and the
   pitch moves it up and down (lower on Hill Climb). */
void sky_update_scroll(void)
{
    s32 i;
    s32 height;
    s32 scroll;
    s32 numViews;

    if (gSkyImage != 0) {
        height = gSkyImage->tex[0].height;
        if (height == 0) {
            height = 0x100;
        }
        i = 0;
        if (i < gNumViews) {
        numViews = gNumViews;
        do {
            sSkyScaleX[i] = 0x80;
            sSkyScaleY[i] = 0x68;
            scroll = gCamYaw[i] & 0x1FFF;
            if (gMirrorMode != 0) {
                scroll = 0x2000 - scroll;
            }
            sSkyScrollX[i] = (scroll / 32 - height) & 0xFF;
            scroll = gCamPitch[i];
            scroll = scroll / 32;
            scroll += 0x80;
            if (gCurTrack == &gTracks[TRACK_HILL_CLIMB]) {
                scroll -= 0x50;
            }
            sSkyScrollY[i] = scroll;
            i++;
        } while (i < numViews);
        }
    }
}

/* A textured rectangle at (x, y), w x h pixels, clipped to (x0, y0)-(x1, y1): the texture coordinates (u, v) (s10.5)
   advance by the steps (du, dv) (s5.10 per pixel) over what is cut off at the left and the top. When *texture is
   set, the strip at that address (256 x 8 CI8) is loaded first and *texture cleared. Returns the display list after
   it, or `gdl` when nothing is left to draw. The rectangle is written by hand: G_TEXRECT with the corners, then
   G_RDPHALF_1 (0xE1) with the coordinates and G_RDPHALF_2 (0xF1) with the steps. sky_strip_rect_2 and sky_texrect
   are the same function, written for their callers to compile the same. */
static inline Gfx *sky_strip_rect(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                                  s32 x0, s32 y0, s32 x1, s32 y1, s32 *texture)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w > 0 && h > 0) {
        if (*texture != 0) {
            gDPLoadTextureBlock(g++, *texture, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *texture = 0;
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
        return g;
    }
    return gdl;
}

static inline Gfx *sky_strip_rect_2(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                                    s32 x0, s32 y0, s32 x1, s32 y1, s32 *texture)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w <= 0 || h <= 0) {
        return gdl;
    }
    {
        if (*texture != 0) {
            gDPLoadTextureBlock(g++, *texture, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *texture = 0;
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
    }
    gdl = g;
    return gdl;
}

/* Draws the strip of the backdrop that starts at image row `row` in view `view`, twice side by side so that the
   scrolled image wraps around. The first strip is repeated up to the top of the screen, the last one down to the
   clip height. */
Gfx *sky_draw_strip(s32 row, s32 view, s32 *texture, Gfx *gdl)
{
    s32 viewW, viewH, xoff, yoff, tileW, tileH, x0, y0;
    s32 y;
    Gfx *p;
    Gfx *q;

    viewH = gViewRects[gNumViews - 1][view].h;
    viewW = gViewRects[gNumViews - 1][view].w;
    tileW = (viewW << 7) / sSkyScaleX[view];
    tileH = (viewH * 4) / sSkyScaleY[view];
    yoff = (row - sSkyScrollY[view]) * (viewH / 2) / sSkyScaleY[view];
    xoff = sSkyScrollX[view] * (viewW / 2) / sSkyScaleX[view];
    yoff += viewH / 2;
    x0 = gViewRects[gNumViews - 1][view].x;
    y0 = gViewRects[gNumViews - 1][view].y;
    q = sky_strip_rect(gdl, x0 + xoff - tileW, y0 + yoff, tileW, tileH + 1, 0, 0, gdl, 0x40000 / tileW,
                       0x2000 / tileH, x0, y0, x0 + viewW, y0 + sSkyClipHeight[view] - 1, texture);
    gdl = sky_strip_rect(q, x0 + xoff, y0 + yoff, tileW, tileH + 1, 0, 0, q, 0x40000 / tileW, 0x2000 / tileH,
                         x0, y0, x0 + viewW, y0 + sSkyClipHeight[view] - 1, texture);
    if (row == 0) {
        y = y0 + yoff;
        while (y > 0) {
            y -= tileH;
        }
        for (; y < y0 + yoff; y += tileH) {
            p = gdl;
            /* the comma expression in the last argument is how this compiles the same */
            p = sky_strip_rect_2(p, x0 + xoff - tileW, y, tileW, tileH + 1, 0, 0, p, 0x40000 / tileW, 0x2000 / tileH,
                                 x0, y0, x0 + viewW - 1, y0 + sSkyClipHeight[view] - 1, (p = gdl = 0, texture));
            gdl = p;
            gdl = sky_strip_rect(gdl, x0 + xoff, y, tileW, tileH + 1, 0, 0, gdl, 0x40000 / tileW, 0x2000 / tileH,
                                 x0, y0, x0 + viewW - 1, y0 + sSkyClipHeight[view] - 1, texture);
        }
    }
    if (row == SKY_LAST_STRIP) {
        for (y = y0 + yoff + tileH; y < y0 + sSkyClipHeight[view]; y += tileH) {
            p = gdl;
            p = sky_strip_rect_2(p, x0 + xoff - tileW, y, tileW, tileH + 1, 0, 0, p, 0x40000 / tileW, 0x2000 / tileH,
                                 x0, y0, x0 + viewW - 1, y0 + sSkyClipHeight[view] - 1, (p = gdl = 0, texture));
            gdl = sky_strip_rect(p, x0 + xoff, y, tileW, tileH + 1, 0, 0, p, 0x40000 / tileW, 0x2000 / tileH,
                                 x0, y0, x0 + viewW - 1, y0 + sSkyClipHeight[view] - 1, texture);
        }
    }
    return gdl;
}

/* Draws the 2D sky backdrop (SKY_MODE_IMAGE) into every view, strip by strip: point sampled, without perspective
   correction and with noise dither, which are set back afterwards. */
Gfx *sky_draw(Gfx *gdl, s32 unused, TexSet *sky)
{
    s32 height;
    s32 row, view;
    s32 strip;

    height = sky->tex[0].height;
    if (height == 0) {
        height = 0x100;
    }
    gDPLoadTLUT_pal256(gdl++, sky->pal);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureFilter(gdl++, G_TF_POINT);
    gDPSetColorDither(gdl++, G_CD_NOISE);

    if (gInsetViewActive == 0) {
        for (row = 0; row < height; row += 8) {
            strip = (s32)sky->tex[0].data + row * 0x100;
            for (view = 0; view < gNumViews; view++) {
                gdl = sky_draw_strip(row, view, &strip, gdl);
            }
        }
    } else {
        /* the inset view shows the whole backdrop */
        sSkyClipHeight[1] = gViewRects[1][1].h;
        for (view = 0; view < gNumViews; view++) {
            for (row = 0; row < height; row += 8) {
                strip = (s32)sky->tex[0].data + row * 0x100;
                gdl = sky_draw_strip(row, view, &strip, gdl);
            }
        }
    }

    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    return gdl;
}

/* The sun's position on screen for sun.c's flares, in view `view` drawn with `mtx`: with a sky image, x from the
   camera yaw (offset by 0x7AEC) and y from the backdrop's scroll; otherwise a fixed point of the track projected
   (another one on Mountain Quarry). Returns 1 when it is more than 50 pixels off screen. */
s32 sky_get_sun_screen_pos(s32 *outx, s32 *outy, s32 mtx, s32 view)
{
    s32 x, y, z, w;
    s32 scale;
    s32 product;

    if (gSkyMode == SKY_MODE_IMAGE) {
        scale = (gViewRects[0][0].w << 7) / sSkyScaleX[view];
        x = (s16)((u16)gCamYaw[0] - 0x7AEC);
        product = x * scale;
        scale = (gViewRects[0][0].h << 2) / sSkyScaleY[view];
        x = product / 8192;
        y = gCamPitch[view];
        /* the height from the pitch is computed, then replaced by one from the backdrop's scroll */
        y = 45 - y * scale / 1024;
        y = 0xB9 - sSkyScrollY[view];
    } else {
        if (gCurTrack_7 - gTracks_3 == TRACK_MOUNTAIN_QUARRY) {
            if (project_to_screen(300, 0x14, 50, &x, &y, &z, &w, mtx, view)) {
                return 1;
            }
        } else {
            if (project_to_screen(0, 0x14, -300, &x, &y, &z, &w, mtx, view)) {
                return 1;
            }
        }
    }
    if (x < -50 || y < -50 || gScreenWidth < x || gScreenHeight + 50 < y) {
        return 1;
    }
    *outx = x;
    *outy = y;
    return 0;
}

/* Empty: called by render_init. */
void sky_init(void)
{
}

/* Clears the sky's clip heights (one per view) before a race. */
void sky_reset_clip(void)
{
    mem_fill(sSkyClipHeight, 0, 0x10);
}

/* sky_strip_rect as a function of its own; nothing calls it. */
Gfx *sky_texrect(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv,
                   s32 x0, s32 y0, s32 x1, s32 y1, u32 *texture)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w > 0 && h > 0) {
        if (*texture != 0) {
            gDPLoadTextureBlock(gdl++, *texture, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *texture = 0;
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
        return gdl;
    }
    return gdl;
}
