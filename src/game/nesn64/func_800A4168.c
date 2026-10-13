#include "ctx.h"

extern s32 D_800F5BB0;

void func_800A4168(void)
{
    u8 *nes;
    u8 *ram;
    u32 idx;

    nes = (u8 *)0x80390000;
    ram = (u8 *)0x80380000;
    idx = *(u16 *)(nes + 0x4100);
    D_800F5BB0 = ram[idx + 1] + nes[0x4104];
}
