#include "ctx.h"

extern u32 D_800F5BB0;
extern u32 D_800F5BB8;
u32 func_800A2444();

static inline u32 rd(s32 a)
{
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        return func_800A2444(a);
    }
    return *(u8 *)(a + 0x80380000);
}

void func_800A3C30(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    u32 r;
    s32 a;

    a = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4104];
    D_800F5BB0 = a;
    r = rd((u16)a);
    D_800F5BB8 = r;
}
