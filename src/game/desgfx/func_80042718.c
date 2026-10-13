#include "ctx.h"

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_80042718(Gfx *gdl, s32 x)
{
    Gfx *p0;
    Gfx *p1;

    if (x != 0) {
        p0 = gdl++;
        p1 = gdl++;
        p0->words.w0 = 0xE7000000;
        p0->words.w1 = 0;
        p1->words.w0 = 0xFCFFFFFF;
        p1->words.w1 = 0xFFFCF279;
        PKT(0xE200001C, 0x230);
        PKT(0xF9000000, 0xFE);
        PKT(0xE2001E01, 1);
    } else {
        PKT(0xE7000000, 0);
        PKT(0xE2001E01, 0);
        PKT(0xFCFFFFFF, 0xFFFCF87C);
        PKT(0xE200001C, 0x442230);
    }
    return gdl;
}
