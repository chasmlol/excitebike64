#include "ctx.h"

u32 func_80060170(u8 *p, s32 n)
{
    s32 i;
    u32 acc = 0;

    for (i = 0; i < n; i++) {
        acc += (u32)p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    return acc;
}
