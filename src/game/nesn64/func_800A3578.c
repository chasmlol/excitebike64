#include "ctx.h"

extern u16 D_800F5BBA;

void func_800A3578(void) {
    u8 *nes = (u8 *)0x80390000;
    u32 base = 0x80380000;
    u8 v;
    u32 i;
    u32 j;

    i = nes[0x4105];
    i += 0x100;
    *(u8 *)(base + i) = (u8)(*(u16 *)(nes + 0x4100) >> 8);
    v = nes[0x4101];
    nes[0x4105] = nes[0x4105] - 1;
    j = (u8)nes[0x4105];
    j += 0x100;
    *(u8 *)(base + j) = v;
    nes[0x4105] = nes[0x4105] - 1;
    *(u16 *)(nes + 0x4100) = D_800F5BBA;
}
