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


void func_800A3D68(void)
{
    u32 *out = &D_800F5BB0;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 a, b;
    u32 r;

    a = p[1];
    *out = a;
    b = (nes[a] | (nes[(u16)(a + 1)] << 8)) + st[0x4104];
    *out = b;
    r = rd((u16)b);
    D_800F5BB8 = r;
}

