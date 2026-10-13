#include "ctx.h"

extern u8 D_800F5BBB;

void func_800A3678(void)
{
    u8 *nes;
    u8 v;
    u8 z;
    u8 w;

    nes = (u8 *)0x80390000;
    v = nes[0x4102] | D_800F5BBB;
    z = (v == 0);
    nes[0x4102] = v;
    w = v & 0x80;
    nes[0x410B] = z;
    nes[0x4108] = w;
}
