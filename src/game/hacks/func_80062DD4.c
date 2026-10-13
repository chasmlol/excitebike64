#include "ctx.h"

extern s32 *D_800F0B58;

s32 func_80062DD4(s32 x)
{
    if (D_800F0B58 != 0) {
        x = ((s32 (*)(s32))(D_800F0B58[0x10 / 4]))(x);
    }
    return x;
}
