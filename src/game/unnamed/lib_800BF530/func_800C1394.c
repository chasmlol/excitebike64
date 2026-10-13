#include "ctx.h"

extern s32 D_800D5758;
extern s32 D_800D575C;
extern u32 D_80105440[];
extern u32 D_801054E0[];
extern u8 D_80101368[];
extern u8 *D_8010536C;
extern u8 *D_80101360;
extern Gfx *func_80048370(Gfx *g);
extern Gfx *func_800C1838(Gfx *g, u32 *x);
extern Gfx *func_800C14F4(Gfx *g, u32 *x);

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_800C1394(Gfx *gdl)
{
    u32 i;

    gdl = func_80048370(gdl);
    PKT(0xE2001E01, 1);
    PKT(0xE200001C, 0x504240);
    PKT(0xFC119623, 0xFF2FFFFF);
    if (D_800D5758 == 0x10000 && D_800D575C == 0x10000) {
        gdl = func_800C1838(gdl, D_80105440);
        gdl = func_800C1838(gdl, D_801054E0);
    } else {
        gdl = func_800C14F4(gdl, D_80105440);
        gdl = func_800C14F4(gdl, D_801054E0);
    }
    PKT(0xE7000000, 0);
    D_8010536C = D_80101368;
    PKT(0xE2001E01, 0);
    D_80101360 = D_80101368 + 0x3FE0;
    for (i = 0; i < 0x28; i++) {
        D_80105440[i] = 0;
        D_801054E0[i] = 0;
    }
    return gdl;
}
