#include "ctx.h"

Gfx *func_80035660(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv,
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
