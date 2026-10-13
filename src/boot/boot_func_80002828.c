#include "ctx.h"

extern void boot_func_80001BB8(s32 addr, s32 val);
extern void boot_func_80001C2C(s32 ms);
extern s32 boot_func_80001BF0(s32 addr);

void boot_func_80002828(s32 p1, s32 p2, s32 p3, s32 p4)
{
    s32 v;

    boot_func_80001BB8(0xB1FFFFE4, p2);
    boot_func_80001BB8(0xB1FFFFE8, p3);
    boot_func_80001BB8(0xB1FFFFEC, p4);
    boot_func_80001BB8(0xB1FFFFE0, p1);

    do {
        boot_func_80001C2C(1000);
        v = boot_func_80001BF0(0xB1FFFFF0);
        if (v != 0) {
            __asm__ volatile("break 0x400");
        }
        v = boot_func_80001BF0(0xB1FFFFE0);
    } while (v != 0);

    boot_func_80001BF0(0xB1FFFFDC);
}
