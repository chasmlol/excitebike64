#include "ctx.h"

void func_800A36D0(void)
{
    u8 *nes;
    u32 a;

    nes = (u8 *)0x80390000;
    nes[0x4105] = nes[0x4105] + 1;
    a = (u8)nes[0x4105] | 0x80380100;
    nes[0x4102] = *(u8 *)a;
}
