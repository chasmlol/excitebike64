#include "ctx.h"

extern s32 D_800FE8D4;

void func_800BB6CC(s32 i, s32 v)
{
    *(s32 *)((u8 *)D_800FE8D4 + i * 48 + 8) = v;
}
