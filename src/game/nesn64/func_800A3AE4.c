#include "ctx.h"

extern u32 D_800F5BB0;
extern u32 D_800F5BB8;
extern u32 func_800A2444();

static inline s32 rd2(s32 a)
{
    if (a < 0x2000) {
        a = a & 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        return func_800A2444(a);
    }
    return *(u8 *)(a + 0x80380000);
}

static inline void rd3(s32 a)
{
    u32 r;
    if (a < 0x2000) {
        a = a & 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = func_800A2444(a);
        goto store;
    }
    r = *(u8 *)(a + 0x80380000);
store:
    D_800F5BB8 = r;
}

void func_800A3AE4(void)
{
    s32 a = *(u8 *)(*(u16 *)0x80394100 + 0x80380001);
    s32 r;
    D_800F5BB0 = a;
    r = rd2(a);
    D_800F5BB8 = r;
}
