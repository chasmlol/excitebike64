#include "ctx.h"

extern u8 *D_800F4FF0;
extern void func_80011490(u8 *, s32, s32, s32, s32, s32, s32, s32);

void func_8008EB74(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5)
{
    s32 off;
    u8 *p;
    s32 n;
    u8 *q;
    u8 *base;

    base = D_800F4FF0;
    off = param_2 * 0x710;
    p = (u8 *)(off + (s32)base);
    n = *(s32 *)(p + 0xa8);
    if (n < 0xc) {
        q = p + n * 0x88;
        *(s32 *)(q + 0xb0) = param_3;
        func_80011490(p + (*(s32 *)(p + 0xa8) * 0x88 + 0xb0) + (param_1 * 0x40 + 8), 0, 0, 0, param_4, 0, param_5, 0x100);
        *(s32 *)((off + (s32)D_800F4FF0) + 0xa8) += 1;
    }
}
