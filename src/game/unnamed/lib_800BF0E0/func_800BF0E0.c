#include "ctx.h"

/* Numeric addresses (0x800D1DF0 / 0x800D1DF8) give one 'lw' macro each (lui+lw in the same reg);
   'r + t - t' references t in the then-arm so flow marks it global (combine drops the use later),
   global-alloc then gives r its $v0 preference and the flag gets $v1. */
s32 func_800BF0E0(void)
{
    s32 t = *(s32 *)0x800D1DF0;
    s32 r = 0;
    if (t != 0) {
        r = *(s32 *)0x800D1DF8 + t - t;
    }
    return r;
}
