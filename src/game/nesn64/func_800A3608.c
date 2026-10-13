#include "ctx.h"
extern u32 D_800F5BB8;

void func_800A3608(void)
{
    u8 *nes = (u8 *)0x80390000;
    s8 b = D_800F5BB8;
    nes[0x4103] = b;
    nes[0x410B] = (D_800F5BB8 & 0xFF) == 0;
    nes[0x4108] = b & 0x80;
}
