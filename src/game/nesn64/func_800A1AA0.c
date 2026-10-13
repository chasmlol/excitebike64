#include "ctx.h"

extern u8 *D_800F5A50;

s32 func_800A1AA0(void)
{
    u8 *p = D_800F5A50 + 0x10000;
    if (p[0x2084] == 0x27 && p[0x2108] == 0x9A) {
        return 1;
    }
    return 0;
}
