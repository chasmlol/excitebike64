#include "ctx.h"

extern u8 D_800EFD70[];

void func_80087EC8(s32 a0)
{
    u8 *base;

    if (a0 != 0) {
        base = D_800EFD70;
        *(s32 *)(base + 0x4C) = 8;
        *(s32 *)(base + 0x3C) = 1;
        *(s32 *)(base) = *(s32 *)(base) | 0x80;
    }
}
