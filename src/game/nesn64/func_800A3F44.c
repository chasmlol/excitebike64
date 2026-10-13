#include "ctx.h"

extern u32 D_800F5BB8;

void func_800A3F44(void)
{
    u32 *out = &D_800F5BB8;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    *out = p[1] | (nes[(u16)(i + 2)] << 8);
}
