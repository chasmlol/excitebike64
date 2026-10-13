#include "ctx.h"

Gfx *func_800285C4(Gfx *gdl, s32 flag)
{
    if (flag) {
        gSPDisplayList(gdl++, 0x07000000);
    } else {
        gSPDisplayList(gdl++, 0x06000000);
    }
    return gdl;
}
