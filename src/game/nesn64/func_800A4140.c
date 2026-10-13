#include "ctx.h"

extern s32 D_800F5BB0;

void func_800A4140(void)
{
    u8 *nes = (u8 *)0x80390000;
    u32 a = *(u16 *)(nes + 0x4100);
    D_800F5BB0 = *(u8 *)(a + 0x80380000 + 1) + nes[0x4103];
}
