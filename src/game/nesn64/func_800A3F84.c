#include "ctx.h"

extern u32 D_800F5BB8;

void func_800A3F84(void)
{
    u32 *out = &D_800F5BB8;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    u16 v = (nes[(u16)(i + 2)] << 8) | p[1];
    u8 *q = nes + v;
    u32 w;
    *out = v;
    w = q[0] | (nes[(u16)(v + 1)] << 8);
    *out = w;
}
