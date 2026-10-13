#include "ctx.h"

extern s32 D_800D33F0;
extern s32 D_800D33F4;

void func_800B99B0(s32 a0)
{
    if (D_800D33F0 != 0) {
        D_800D33F4 = a0;
    }
}
