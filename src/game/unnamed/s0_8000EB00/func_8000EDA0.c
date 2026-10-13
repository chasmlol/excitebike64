#include "ctx.h"

static inline s32 up(s32 c)
{
    if ((u32)(c - 0x61) < 0x1A) {
        return c - 0x20;
    }
    return c;
}

s32 func_8000EDA0(u8 *param_1, u8 *param_2)
{
    for (;;) {
        s32 c1 = *param_1;
        s32 c2;
        if (up(c1) < up(c2 = *param_2)) {
            return -1;
        }
        if (up(c1) > up(c2)) {
            return 1;
        }
        if (c1 == 0) {
            return 0;
        }
        param_1++;
        param_2++;
    }
}
