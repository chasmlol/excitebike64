#include "ctx.h"

extern s32 D_800FE8D4;

s32 func_800BB780(s32 i)
{
    return *(s32 *)((u8 *)D_800FE8D4 + i * 48 + 4);
}
