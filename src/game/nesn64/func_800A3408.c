#include "ctx.h"

void func_800A3408(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 v = nes[0x4103] + -1;
    u8 m;
    nes[0x4103] = v;
    m = (u8)v & 0x80;
    nes[0x410B] = ((u8)v == 0);
    nes[0x4108] = m;
}
