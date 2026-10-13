#include "ctx.h"

void func_8000EE30(s32 v, char *out)
{
    s32 u;
    s32 d;
    s32 q;

    if (v < 0) {
        *out = '-';
        out++;
        v = -v;
    }
    u = v & 0xFFFFFFF;
    d = 1;
    if (u != 0) {
        do {
            d = d * 10;
        } while (d <= u);
    }
    q = 0;
    if (u != 0) {
        d = d / 10;
    }
    for (; 0 < d; d = d / 10) {
        q = u / d;
        *out = q + '0';
        out++;
        u = u - q * d;
    }
    *out = 0;
}

