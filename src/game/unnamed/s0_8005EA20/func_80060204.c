#include "ctx.h"

extern u8 *D_800F0A38;
extern s32 D_800F0A24;

s32 func_80060204(s32 n)
{
    s32 r = 0;
    s32 i;

    for (i = 0; i < n; i++) {
        u8 *p = D_800F0A38;
        s32 byte = *p;
        s32 c = D_800F0A24 - 1;
        s32 bit;

        r = r << 1;
        D_800F0A24 = c;
        bit = (byte >> c) & 1;
        if (c == 0) {
            D_800F0A38 = p + 1;
            D_800F0A24 = 8;
        }
        r = r | bit;
    }
    return r;
}
