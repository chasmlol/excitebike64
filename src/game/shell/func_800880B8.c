#include "ctx.h"

extern s32 D_800EFD70;

void func_800880B8(s32 x)
{
    if (x != 0) {
        D_800EFD70 = (D_800EFD70 & ~0x80) | 0x20;
    }
}
