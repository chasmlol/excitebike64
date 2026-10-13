#include "ctx.h"

extern u8 D_800F0B30;

u8 func_80061FD4(s8 *p, s16 v)
{
    if (*(s16 *)(p + 0x660) < v) {
        return 0;
    }
    D_800F0B30 = D_800F0B30 ^ 1;
    return D_800F0B30;
}
