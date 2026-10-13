#include "ctx.h"

extern u8 D_800F0C98[];

void func_80064D28(s32 a, s32 b)
{
    s32 n = a + b;
    s32 i;
    s32 half = n / 2;
    u8 *q;

    if (n < 3) {
        u8 *r;
        D_800F0C98[0] = 0;
        r = D_800F0C98;
        r[1] = 1;
        return;
    }
    for (i = 0; i < n; i++) {
        q = D_800F0C98 + i;
        if (i & 1) {
            *q = half + i / 2;
        } else {
            *q = half - (i / 2 + 1);
        }
    }
}
