#include "ctx.h"

s32 func_8000ECE0(s32 a0)
{
    if ((u32)(a0 - 0x61) < 0x1A) {
        a0 = a0 - 0x20;
    }
    return a0;
}
