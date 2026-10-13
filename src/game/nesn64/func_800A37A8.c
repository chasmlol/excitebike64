#include "ctx.h"

extern u32 D_800F5BB8;

void func_800A37A8(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 r = nes[0x4102];
    s32 a = r;
    s32 x;
    r = r - D_800F5BB8;
    if (nes[0x410C] == 0) {
        r = r - 1;
    }
    nes[0x410C] = ((u32)~r) >> 31;
    nes[0x410B] = ((r & 0xFF) == 0);
    nes[0x4108] = r & -0x80;
    x = a ^ r;
    nes[0x4102] = r;
    nes[0x4106] = x & -0x80;
}
