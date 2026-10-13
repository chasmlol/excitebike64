#include "ctx.h"

extern u32 D_800F5BB0;

void func_800A4038(void)
{
    u32 *out = &D_800F5BB0;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    *out = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4103];
}
