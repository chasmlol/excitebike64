#include "ctx.h"

extern u32 D_800EFD70;
extern char *D_800EFD64;

void func_80088198(int param_1)
{
    if (param_1 != 0) {
        u32 *b = &D_800EFD70;
        b[0x13] = 10;
        D_800EFD64 = "Stunt Course";
        D_800EFD70 = D_800EFD70 & 0xffffff7f;
    }
}
