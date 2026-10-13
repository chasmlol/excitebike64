#include "ctx.h"

void func_800B9FE0(u8 *d, u8 *s, s32 n)
{
    while (n != 0) {
        *d++ = *s++;
        n = n - 1;
    }
}
