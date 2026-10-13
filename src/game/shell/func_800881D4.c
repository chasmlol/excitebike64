#include "ctx.h"

extern s32 func_80060170(void *p, s32 n);

s32 func_800881D4(u32 *p, s32 n)
{
    s32 r;
    if (n != 0x400) {
        return 1;
    }
    r = func_80060170((u8 *)(p + 1), 0x3FC);
    return r != *p;
}
