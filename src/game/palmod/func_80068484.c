#include "ctx.h"

#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define G_RDPPIPESYNC 0xE7
#define G_SETPRIMCOLOR 0xFA
#define G_FILLRECT 0xF6
#define gDPNoParam(pkt, cmd) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = _SHIFTL(cmd, 24, 8); \
        _g->words.w1 = 0; \
    }
#define gDPPipeSync(pkt) gDPNoParam(pkt, G_RDPPIPESYNC)
#define gDPSetPrimColor(pkt, m, l, r, g, b, a) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (_SHIFTL(G_SETPRIMCOLOR, 24, 8) | _SHIFTL(m, 8, 8) | _SHIFTL(l, 0, 8)); \
        _g->words.w1 = (_SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8)); \
    }
#define gDPFillRectangle(pkt, ulx, uly, lrx, lry) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (_SHIFTL(G_FILLRECT, 24, 8) | _SHIFTL((lrx), 14, 10) | _SHIFTL((lry), 2, 10)); \
        _g->words.w1 = (_SHIFTL((ulx), 14, 10) | _SHIFTL((uly), 2, 10)); \
    }


typedef struct {
    u8 unk0[0x21C];
    u8 r;
    u8 g;
    u8 b;
    u8 unk21F[0xA80 - 0x21F];
} Player;

extern Player D_800DC150[];

void func_80068F38(s32 player, s32 *px, s32 *py);

Gfx *func_80068484(Gfx *gdl, s32 player, s32 big)
{
    s32 x;
    s32 y;
    s32 cx;
    s32 cy;
    Gfx *g;

    func_80068F38(player, &x, &y);
    gDPPipeSync(gdl++);
    gDPSetPrimColor(gdl, 0, 0, D_800DC150[player].r, D_800DC150[player].g, D_800DC150[player].b, 255);
    g = gdl + 1;
    cx = x;
    cy = y;
    if (big) {
        gDPFillRectangle(g++, cx - 4, cy - 3, cx + 4, cy + 3);
    } else {
        gDPFillRectangle(g++, cx - 2, cy - 2, cx + 2, cy + 2);
    }
    return g;
}
