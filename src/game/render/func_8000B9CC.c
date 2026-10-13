#include "ctx.h"

extern u8 D_800C2A38[];
extern u8 D_800C2A00[];

#define PKT(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (unsigned int)(b); }
#define LOADUCODE(pkt, s, d) { \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = 0xE1000000; \
    _g->words.w1 = (unsigned int)(d); \
    _g = (Gfx *)(pkt); \
    _g->words.w0 = 0xDD0007FF; \
    _g->words.w1 = (unsigned int)(s); \
}

Gfx *func_8000B9CC(Gfx *gdl, s32 a1, s32 a2, s32 a3, s32 a4)
{
    PKT(gdl++, 0xE7000000, 0);
    LOADUCODE(gdl++, a3, a4);
    PKT(gdl++, 0xDB060000, 0);
    PKT(gdl++, 0xDE000000, D_800C2A38);
    PKT(gdl++, 0xDE000000, D_800C2A00);
    return gdl;
}
