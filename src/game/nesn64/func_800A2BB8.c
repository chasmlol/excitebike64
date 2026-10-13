#include "ctx.h"

extern u8 D_80394105;
extern void func_800A375C(void);

void func_800A2BB8(void)
{
    u32 v;
    u32 bits;
    u32 base;

    base = 0x80380100;
    v = (u32)D_80394105 + 1;
    *(u8 *)0x80394105 = v;
    v = v & 0xFF;
    bits = *(u8 *)(base | v);
    *(u8 *)0x80394108 = bits & 0x80;
    *(u8 *)0x80394106 = bits & 0x40;
    *(u8 *)0x80394107 = bits & 0x10;
    *(u8 *)0x80394109 = bits & 0x8;
    *(u8 *)0x8039410A = bits & 0x4;
    *(u8 *)0x8039410B = bits & 0x2;
    *(u8 *)0x8039410C = bits & 0x1;
    func_800A375C();
}
