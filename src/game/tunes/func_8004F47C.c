#include "ctx.h"

extern s32 D_800EFD28[];
extern s32 D_800EECD8;
extern s32 D_800EECB8;
extern s32 D_800EE4B0;
extern s32 D_800EFCF0;
extern s32 D_800EFCF8;
extern s32 D_800EECC0;

void func_8004F47C(void)
{
    s32 val = -0x708;
    s32 i = 7;

    do {
        D_800EFD28[i] = val;
        i--;
    } while (i >= 0);
    D_800EECD8 = -1;
    D_800EECB8 = -1;
    D_800EE4B0 = -0x708;
    D_800EFCF0 = -0x708;
    D_800EFCF8 = 0;
    D_800EECC0 = 0;
}
