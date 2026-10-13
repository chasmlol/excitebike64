#include "ctx.h"

extern s32 D_800EE128;

s32 func_8004367C(s32 a0, s32 a1)
{
    s32 y;

    y = ((a0 * (a1 + 0x7B) * 0x1C8) << (a0 % 0x315A)) + a1 * 0x55;
    D_800EE128 = (y * 0x35DF + 0x3038) % 0xFFFB;
    return D_800EE128 & 0xFF;
}
