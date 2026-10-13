#include "ctx.h"

s32 func_800BE998(s32 *a0)
{
    u8 *p;
    u32 n;

    p = (u8 *)a0[0];
    n = (u32)a0[3];
    return *(u32 *)(p + 0x1C) <= n;
}
