#include "ctx.h"

typedef struct {
    s32 x, y, w, h;
} Rect;

extern s32 D_800C2594;
extern Rect D_800E1718[][4];
extern s32 D_800E8EA0[];
extern s32 D_800E8EB0[];
extern s32 D_800E8EC0[];
extern s32 D_800E8ED0[];
extern s32 D_800E8EE0[];

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

Gfx *func_80034100(s32 a, s32 b, s32 *tex, Gfx *gdl)
{
    s32 rw, rh, xoff, yoff, tw, th, x0, y0;
    s32 y;
    Gfx *p;
    Gfx *q;

    rh = D_800E1718[D_800C2594 - 1][b].h;
    rw = D_800E1718[D_800C2594 - 1][b].w;
    tw = (rw << 7) / D_800E8EC0[b];
    th = (rh * 4) / D_800E8ED0[b];
    yoff = (a - D_800E8EB0[b]) * (rh / 2) / D_800E8ED0[b];
    xoff = D_800E8EA0[b] * (rw / 2) / D_800E8EC0[b];
    yoff += rh / 2;
    x0 = D_800E1718[D_800C2594 - 1][b].x;
    y0 = D_800E1718[D_800C2594 - 1][b].y;
    q = drawTex(gdl, x0 + xoff - tw, y0 + yoff, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + D_800E8EE0[b] - 1, tex);
    gdl = drawTex(q, x0 + xoff, y0 + yoff, tw, th + 1, 0, 0, q, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + D_800E8EE0[b] - 1, tex);
    if (a == 0) {
        y = y0 + yoff;
        while (y > 0) {
            y -= th;
        }
        for (; y < y0 + yoff; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, (p = gdl = 0, tex));
            gdl = p;
            gdl = drawTex(gdl, x0 + xoff, y, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, tex);
        }
    }
    if (a == 0xF8) {
        for (y = y0 + yoff + th; y < y0 + D_800E8EE0[b]; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, (p = gdl = 0, tex));
            gdl = drawTex(p, x0 + xoff, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, tex);
        }
    }
    return gdl;
}
