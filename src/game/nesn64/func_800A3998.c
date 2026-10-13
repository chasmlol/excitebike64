#include "ctx.h"

void func_800A3998(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4105];
    nes[0x4103] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}
