#include "ctx.h"

void func_8000ED70(u8 *d, u8 *s, s32 n)
{
    for (;;) {
        n = n - 1;
        *d = *s;
        if (n == 0) {
            *d = 0;
            return;
        }
        d = d + 1;
        if (*s == 0) {
            return;
        }
        s = s + 1;
    }
}
