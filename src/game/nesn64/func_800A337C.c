#include "ctx.h"

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;

void func_800A2178(s32 a, s32 b);

static inline void wr(s32 a, s32 v)
{
    if (a < 0x2000) {
        a &= 0x7FF;
        *(u8 *)(a + 0x80380000) = v;
    } else if (a > 0x7FFF) {
    } else {
        func_800A2178(a, v);
    }
}

void func_800A337C(void)
{
    u8 *st;

    D_800F5BB8 = D_800F5BB8 - 1;
    wr(D_800F5BB0, D_800F5BB8);
    st = (u8 *)0x80390000;
    st[0x410B] = (D_800F5BB8 & 0xFF) == 0;
    st[0x4108] = D_800F5BB8 & 0x80;
}
