#include "ctx.h"

void func_80014D20(u32 *p)
{
    u32 b = *p & 0xFFFFFFFC;
    *(u32 *)(b + 4) = *(u32 *)(b + 4) + b;
    *(u32 *)(b + 12) = *(u32 *)(b + 12) + b;
}
