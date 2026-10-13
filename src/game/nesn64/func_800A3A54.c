#include "ctx.h"

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;
u32 func_800A2444();

void func_800A3A54(void) {
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    s32 a, r;

    D_800F5BB0 = p[1] | (nes[(u16)(i + 2)] << 8);
    a = D_800F5BB0;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = func_800A2444(a);
        goto store;
    }
    r = *(u8 *)(0x80380000 + a);
store:
    D_800F5BB8 = r;
}
