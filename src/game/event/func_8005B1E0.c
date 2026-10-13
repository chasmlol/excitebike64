#include "ctx.h"

extern u8 D_800F05A8[];

void func_8005B1E0(void)
{
    u8 *p;
    s32 w;

    p = D_800F05A8;
    w = *(s32 *)(p + 0x58);
    *(u16 *)(p + 0x26) = 0xB4;
    *(s32 *)&D_800F05A8 = 6;
    *(u8 *)(p + 0x6E) = 0;
    *(u16 *)(p + 0x68) = 0;
    *(u16 *)(p + 0x6A) = 0;
    *(u16 *)(p + 0x6C) = 0;
    *(s32 *)(p + 0x58) = w | 0x10;
}
