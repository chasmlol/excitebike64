#include "ctx.h"

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;
u32 func_800A2444();

void func_800A3B94(void) {
    u8 *st = (u8 *)0x80390000;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 s, r, a;

    s = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4103];
    D_800F5BB0 = s;
    a = (u16)s;
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
