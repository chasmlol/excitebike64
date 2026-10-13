#include "ctx.h"

extern s32 D_800F5BB0;

void func_800A3FE0(void)
{
    s32 *dst = &D_800F5BB0;
    u8 *base = (u8 *)0x80380000;
    u32 h = *(u16 *)0x80394100;
    u8 *p = (u8 *)((u32)base + h);
    *dst = (u32)p[1] | ((u32)base[(u16)(h + 2)] << 8);
}
