#include "ctx.h"

extern u8 *D_800EE138;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80042830(s32 a0, s32 a1);
void func_80042A5C(u8 *a0, s32 a1);

void func_80044358(void)
{
    s32 i;
    s32 off;
    u8 *p;
    u32 hi;

    func_8000ED10(D_800EE138, 0, 4);
    i = 0xF;
    p = D_800EE138 + 0x3C3C;
    do {
        *(s32 *)p = -0x3E7;
        i = i - 1;
        p = p - 0x404;
    } while (i >= 0);
    func_80042830(0, 0);
    i = 0;
    hi = 0x800F0000;
    off = i;
    for (; i < 0x10; i++) {
        func_80042A5C(*(u8 **)(hi - 0x1EC8) + off, -1);
        off = off + 0x404;
    }
}
