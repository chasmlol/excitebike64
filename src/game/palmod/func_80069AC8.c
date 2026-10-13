#include "ctx.h"

extern u8 D_800DC150[][0xA80];

Gfx *func_80069AC8(Gfx *gdl, s32 idx, s32 flag, s32 x, s32 y) {
    gDPSetPrimColor(gdl++, 0, 0, D_800DC150[idx][0x21C], D_800DC150[idx][0x21D],
                    D_800DC150[idx][0x21E], 255);
    if (flag != 0) {
        gDPFillRectangle(gdl++, x - 4, y - 3, x + 4, y + 3);
    } else {
        gDPFillRectangle(gdl++, x - 2, y - 2, x + 2, y + 2);
    }
    return gdl;
}
