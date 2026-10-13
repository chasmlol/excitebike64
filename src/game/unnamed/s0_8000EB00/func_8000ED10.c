#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2)
{
    while (a2 > 0) {
        *a0 = a1;
        a0 = a0 + 1;
        a2 = a2 - 1;
    }
}
