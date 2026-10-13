#include "ctx.h"

extern u32 D_800E1C60;
extern s32 D_800E2010[];

void func_8000D0A0(u32 a0, s32 a1)
{
    if ((D_800E1C60 & (1u << a0)) != 0) {
        D_800E2010[a0] = a1 + 1;
    }
}
