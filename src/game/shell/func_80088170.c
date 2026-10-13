#include "ctx.h"

extern s32 D_800EFD70[];

void func_80088170(void)
{
    D_800EFD70[19] = 12;
    D_800EFD70[0] = (D_800EFD70[0] & ~0x80) | 0x40;
}
