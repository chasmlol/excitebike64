#include "ctx.h"

extern u8 D_800F4A28[];
extern s32 D_800F4A00;

void func_8008CA40(s32 a0, s32 a1)
{
    u8 *p;

    p = D_800F4A28;
    *(s32 *)(p + 4) = a0;
    *(s32 *)(p + 8) = 0;
    if (a1 != -1) {
        D_800F4A00 = a1;
    }
}
