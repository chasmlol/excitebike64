#include "ctx.h"

s32 func_8000ECF8(s32 a0)
{
    if ((u32)(a0 - 0x41) < 0x1A) {
        a0 = a0 + 0x20;
    }
    return a0;
}
