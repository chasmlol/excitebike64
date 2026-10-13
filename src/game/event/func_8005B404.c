#include "ctx.h"

extern u8 *D_800EFD6C;
s32 func_8000A690(s32 a, s32 b, u8 *c, u32 *d, s32 e, s32 f, s32 g, s32 h, s32 i);
#define PKT(a, b) { Gfx *_g = dl++; _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8005B404(s32 p1, s32 p2, s32 p3, Gfx *dl, s32 p5)
{
    s32 i = 0;
    s32 v;
    u8 *base;
    u8 *pb;

    PKT(0xE3001001, 0);
    PKT(0xFC119623, 0xFF2FFFFF);
    base = D_800EFD6C;
    pb = base + 0x58;
    dl = (Gfx *)func_8000A690(p1 - p3 * *pb * 8 - base[0x60] * 8, p2, base + 0x60, (u32 *)dl, p5, 1, 1, 6, 0);
    for (i = 0; i < p3; i++) {
        v = p3 - i;
        dl = (Gfx *)func_8000A690(p1 - v * *pb * 8, p2, pb, (u32 *)dl, p5, 1, 1, 6, 0);
    }
    return dl;
}
