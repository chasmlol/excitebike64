#include "ctx.h"

/* 6502 RTS-style pull of a 16-bit word from the stack page. */
void func_800A375C(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    u32 lo, hi;
    s32 a, b;

    st[0x4105]++;
    a = st[0x4105] + 0x100;
    lo = ram[a];
    st[0x4105]++;
    b = st[0x4105] + 0x100;
    hi = ram[b];
    *(u16 *)(st + 0x4100) = lo | (hi << 8);
}
