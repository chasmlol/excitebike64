#include "ctx.h"
extern u8 D_800F5BBB;

void func_800A3458(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4102] ^ D_800F5BBB;
    nes[0x4102] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}
