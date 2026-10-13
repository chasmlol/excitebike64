#include "ctx.h"

void func_800A36A4(void)
{
    u8 *nes;
    u32 a;

    nes = (u8 *)0x80390000;
    a = nes[0x4105] | 0x80380100;
    *(u8 *)a = nes[0x4102];
    nes[0x4105] = nes[0x4105] - 1;
}
