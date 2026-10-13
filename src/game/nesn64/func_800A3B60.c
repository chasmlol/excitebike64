#include "ctx.h"

extern s32 D_800F5BB0;
extern u32 D_800F5BB8;

void func_800A3B60(void)
{
    u8 *nes;
    u8 *ram;
    u32 idx;

    nes = (u8 *)0x80390000;
    ram = (u8 *)0x80380000;
    D_800F5BB0 = -1;
    idx = *(u16 *)(nes + 0x4100);
    D_800F5BB8 = ram[idx + 1];
}
