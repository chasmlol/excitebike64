#include "ctx.h"

extern s32 D_800C2AC4;

u32 func_8000EFE8(void) {
    s32 x;
    s32 v;

    x = D_800C2AC4 * 0x35DF + 0x3038;
    v = x - (x / 0xFFFB) * 0xFFFB;
    D_800C2AC4 = v;
    return v & 0xFF;
}
