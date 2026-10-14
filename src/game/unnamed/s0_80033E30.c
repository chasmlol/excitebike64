#include "game.h"

/* The clip height written through a one-field struct: as a plain s32 the store is scheduled differently. */
typedef struct { s32 v; } ClipStore;
typedef struct { u8 pad[0x2C]; s32 f2c; } Obj_26;

extern Obj_26 *gCurTrack_24;

void sky_scan_zbuffer(s32 arg0)
{
    s32 i, x, y, m;
    Rect *r;
    Rect *q;
    ClipStore *p;

    if ((gGame.flags & 0x10000) == 0) {
        return;
    }
    m = gNumViews - 1;
    for (i = 0; i < gNumViews; i++) {
        if (gCurTrack_24->f2c == 0) {
            sSkyClipHeight[i] = gViewRects[m][i].h;
        } else {
            sSkyClipHeight[i] = 0;
            y = gViewRects[m][i].y;
            /* r and q add the row and column offsets to the base in one go (&gViewRects[m][i] does not match) */
            r = (Rect *)((u8 *)gViewRects + (m * 64 + i * 16));
            if (y < r->y + r->h) {
            yl:
                p = (ClipStore *)&sSkyClipHeight[i];
                for (x = r->x; x < r->x + r->w; x += 8) {
                    if (*(u16 *)((u32)gZBuffer + ((y * gScreenWidth + x) << 1)) > 0xFFEF) {
                        p->v = y - r->y;
                    }
                }
                y += 16;
                if (y < r->y + r->h) goto yl;
            }
            sSkyClipHeight[i] += 16;
            if (sSkyPrevClipHeight[i] > 16 && sSkyClipHeight[i] < sSkyPrevClipHeight[i] - 16) {
                sSkyClipHeight[i] = sSkyPrevClipHeight[i] - 16;
            }
            q = (Rect *)((u8 *)gViewRects + (m * 64 + i * 16));
            if (sSkyClipHeight[i] > q->h) {
                sSkyClipHeight[i] = q->h;
            }
            if (sSkyClipHeight[i] < q->h / 2) {
                sSkyClipHeight[i] = q->h;
            }
        }
        sSkyPrevClipHeight[i] = sSkyClipHeight[i];
    }
}

void sky_update_scroll(void)
{
    s32 i;
    s32 t;
    s32 v;
    s32 n;

    if (gSkyImage != 0) {
        t = gSkyImage->tex[0].height;
        if (t == 0) {
            t = 0x100;
        }
        i = 0;
        if (i < gNumViews) {
        n = gNumViews;
        do {
            sSkyScaleX[i] = 0x80;
            sSkyScaleY[i] = 0x68;
            v = gCamYaw[i] & 0x1FFF;
            if (gMirrorMode != 0) {
                v = 0x2000 - v;
            }
            sSkyScrollX[i] = (v / 32 - t) & 0xFF;
            v = gCamPitch[i];
            v = v / 32;
            v += 0x80;
            if (gCurTrack == gTrackHillClimb) {
                v -= 0x50;
            }
            sSkyScrollY[i] = v;
            i++;
        } while (i < n);
        }
    }
}

static inline Gfx *drawTex(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                           s32 x0, s32 y0, s32 x1, s32 y1, s32 *flag)
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
        if (*flag != 0) {
            gDPLoadTextureBlock(g++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
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

static inline Gfx *drawTexA(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                           s32 x0, s32 y0, s32 x1, s32 y1, s32 *flag)
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
        if (*flag != 0) {
            gDPLoadTextureBlock(g++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
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

Gfx *sky_draw_strip(s32 a, s32 b, s32 *tex, Gfx *gdl)
{
    s32 rw, rh, xoff, yoff, tw, th, x0, y0;
    s32 y;
    Gfx *p;
    Gfx *q;

    rh = gViewRects[gNumViews - 1][b].h;
    rw = gViewRects[gNumViews - 1][b].w;
    tw = (rw << 7) / sSkyScaleX[b];
    th = (rh * 4) / sSkyScaleY[b];
    yoff = (a - sSkyScrollY[b]) * (rh / 2) / sSkyScaleY[b];
    xoff = sSkyScrollX[b] * (rw / 2) / sSkyScaleX[b];
    yoff += rh / 2;
    x0 = gViewRects[gNumViews - 1][b].x;
    y0 = gViewRects[gNumViews - 1][b].y;
    q = drawTex(gdl, x0 + xoff - tw, y0 + yoff, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + sSkyClipHeight[b] - 1, tex);
    gdl = drawTex(q, x0 + xoff, y0 + yoff, tw, th + 1, 0, 0, q, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + sSkyClipHeight[b] - 1, tex);
    if (a == 0) {
        y = y0 + yoff;
        while (y > 0) {
            y -= th;
        }
        for (; y < y0 + yoff; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + sSkyClipHeight[b] - 1, (p = gdl = 0, tex));
            gdl = p;
            gdl = drawTex(gdl, x0 + xoff, y, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + sSkyClipHeight[b] - 1, tex);
        }
    }
    if (a == 0xF8) {
        for (y = y0 + yoff + th; y < y0 + sSkyClipHeight[b]; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + sSkyClipHeight[b] - 1, (p = gdl = 0, tex));
            gdl = drawTex(p, x0 + xoff, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + sSkyClipHeight[b] - 1, tex);
        }
    }
    return gdl;
}

Gfx *sky_draw(Gfx *gdl, s32 unused, TexSet *sky)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *q0, *q1, *q2;
    s32 n;
    s32 i, j;
    s32 local;

    n = sky->tex[0].height;
    if (n == 0) {
        n = 0x100;
    }
    g0 = gdl++; ((u32 *)g0)[0] = 0xFD100000; ((u32 *)g0)[1] = (u32)sky->pal;
    g1 = gdl++; ((u32 *)g1)[0] = 0xE8000000; ((u32 *)g1)[1] = 0;
    g2 = gdl++; ((u32 *)g2)[0] = 0xF5000100; ((u32 *)g2)[1] = 0x07000000;
    g3 = gdl++; ((u32 *)g3)[0] = 0xE6000000; ((u32 *)g3)[1] = 0;
    g4 = gdl++; ((u32 *)g4)[0] = 0xF0000000; ((u32 *)g4)[1] = 0x073FC000;
    g5 = gdl++; ((u32 *)g5)[0] = 0xE7000000; ((u32 *)g5)[1] = 0;
    g6 = gdl++; ((u32 *)g6)[0] = 0xE3000C00; ((u32 *)g6)[1] = 0;
    g7 = gdl++; ((u32 *)g7)[0] = 0xE3001201; ((u32 *)g7)[1] = 0;
    g8 = gdl++; ((u32 *)g8)[0] = 0xE3001801; ((u32 *)g8)[1] = 0x80;

    if (gInsetViewActive == 0) {
        for (i = 0; i < n; i += 8) {
            local = (s32)sky->tex[0].data + i * 0x100;
            for (j = 0; j < gNumViews; j++) {
                gdl = sky_draw_strip(i, j, &local, gdl);
            }
        }
    } else {
        sSkyClipHeight[1] = gViewRects[1][1].h;
        for (j = 0; j < gNumViews; j++) {
            for (i = 0; i < n; i += 8) {
                local = (s32)sky->tex[0].data + i * 0x100;
                gdl = sky_draw_strip(i, j, &local, gdl);
            }
        }
    }

    q0 = gdl++; ((u32 *)q0)[0] = 0xE3001201; ((u32 *)q0)[1] = 0x2000;
    q1 = gdl++; ((u32 *)q1)[0] = 0xE3000C00; ((u32 *)q1)[1] = 0x00080000;
    q2 = gdl++; ((u32 *)q2)[0] = 0xE3001801; ((u32 *)q2)[1] = 0;
    return gdl;
}

s32 sky_get_sun_screen_pos(s32 *outx, s32 *outy, s32 mtx, s32 view)
{
    s32 x, y, z, w;
    s32 t;
    s32 m;

    if (gSkyMode == 1) {
        t = (gViewRects[0][0].w << 7) / sSkyScaleX[view];
        x = (s16)((u16)gCamYaw[0] - 0x7AEC);
        m = x * t;
        t = (gViewRects[0][0].h << 2) / sSkyScaleY[view];
        x = m / 8192;
        y = gCamPitch[view];
        y = 45 - y * t / 1024;
        y = 0xB9 - sSkyScrollY[view];
    } else {
        if (gCurTrack_7 - gTracks_3 == 1) {
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

void sky_init(void)
{
}

void sky_reset_clip(void) {
    mem_fill(sSkyClipHeight, 0, 0x10);
}

Gfx *sky_texrect(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv,
                   s32 x0, s32 y0, s32 x1, s32 y1, u32 *flag)
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
        if (*flag != 0) {
            gDPLoadTextureBlock(gdl++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
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
