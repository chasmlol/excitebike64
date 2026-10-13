#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_800A1B0C(u8 *p)
{
    func_8000ED10(p, 0x40, 0x220);
    p[0] = 1;
    p[0xCF] = 9;
    func_8000ED10(p + 0xD0, 0, 0x30);
    p[0x100] = 1;
    p[0x1CF] = 9;
    func_8000ED10(p + 0x1D0, 0, 0x50);
    *(s16 *)(p + 0x200) = 0x5A10;
    p[0x204] = 0x2D;
    *(s16 *)(p + 0x20C) = -0x73FF;
    p[0x209] = 1;
    p[0x21B] = 1;
}
