#include "ctx.h"

extern s32 D_800F5584;
extern s32 D_800F5588;
extern s32 D_800F5590;
extern s32 D_800F5598;

void func_8008F07C(s32 a0, s32 a1, s32 a2, s32 a3)
{
    D_800F5584 = a0;
    D_800F5588 = a1;
    D_800F5590 = a0 + a2;
    D_800F5598 = a1 + a3;
}
