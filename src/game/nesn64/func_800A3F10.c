#include "ctx.h"

extern s32 D_800F5BB8;

void func_800A3F10(void)
{
    s32 *dst = &D_800F5BB8;
    u32 a = *(u16 *)0x80394100;
    s32 v = a + *(s8 *)(a + 0x80380000 + 1) + 2;
    *dst = v & 0xFFFF;
}
