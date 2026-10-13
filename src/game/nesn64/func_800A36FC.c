#include "ctx.h"

/* 6502 PLP: pull the status byte from the stack page and split it into the flag bytes. */
void func_800A36FC(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    u32 v;
    s32 a;

    st[0x4105]++;
    a = st[0x4105] + 0x100;
    v = ram[a];
    st[0x4108] = v & 0x80;
    st[0x4106] = v & 0x40;
    st[0x4107] = v & 0x10;
    st[0x4109] = v & 0x08;
    st[0x410A] = v & 0x04;
    st[0x410B] = v & 0x02;
    st[0x410C] = v & 0x01;
}
