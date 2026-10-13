#include "ctx.h"

extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA858;
extern s32 D_800EFD00[];

void func_8004F4DC(s32 a0)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0) {
        s32 n = D_800CA858;
        if (n < 8) {
            D_800EFD00[n] = a0;
            D_800CA858 = n + 1;
        }
    }
}
