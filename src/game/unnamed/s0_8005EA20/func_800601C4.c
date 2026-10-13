#include "ctx.h"

extern u8 *D_800F0A38;
extern s32 D_800F0A24;

s32 func_800601C4(void)
{
    u8 *p = D_800F0A38;
    s32 byte = *p;
    s32 n = D_800F0A24 - 1;
    s32 bit;

    D_800F0A24 = n;
    bit = byte >> n;
    bit = bit & 1;
    if (n == 0) {
        D_800F0A38 = p + 1;
        D_800F0A24 = 8;
    }
    return bit;
}
