#include "ctx.h"

s32 func_8000ECE0(s32 a0);

s32 func_800BA008(u8 *a, u8 *b)
{
    s32 x;
    s32 y;
    for (;;) {
        x = func_8000ECE0(*a);
        y = func_8000ECE0(*b);
        if (x != y) {
            return 1;
        }
        if (*a == 0) {
            return 0;
        }
        a++;
        b++;
    }
}
