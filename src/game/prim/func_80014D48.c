#include "ctx.h"

s32 func_80014D48(u16 *p, s32 r, s32 g, s32 b, s32 flag, s32 n)
{
    s32 best = 99999999;
    s32 bi = 0;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        if (flag == 0) {
            if ((*p & 1) == 0) {
                return i;
            }
        } else {
            if ((*p & 1) == 0) {
                continue;
            }
        }
        {
            u32 c = *p;
            s32 t;
            s32 d;
            t = (s32)(c >> 11) - r;
            d = t * t;
            t = (s32)((c >> 6) & 0x1F) - g;
            t = t * t;
            d += t;
            t = (s32)((c >> 1) & 0x1F) - b;
            t = t * t;
            d += t;
            if (d < best) {
                best = d;
                bi = i;
            }
        }
    }
    return bi;
}
