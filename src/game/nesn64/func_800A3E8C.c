#include "ctx.h"

extern u32 D_800F5BB0;
extern u32 D_800F5BB8;
extern u32 func_800A2444(u32 a);

static inline u32 rd2(s32 a)
{
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a <= 0x7FFF) {
        return func_800A2444(a);
    }
    return *(u8 *)(a + 0x80380000);
}

void func_800A3E8C(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    s32 a;
    a = ram[*(u16 *)(st + 0x4100) + 1] + st[0x4104];
    D_800F5BB0 = a;
    D_800F5BB8 = rd2(a);
}
