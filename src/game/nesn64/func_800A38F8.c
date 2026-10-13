#include "ctx.h"

extern s32 D_800F5BB0;

void func_800A2178(s32 addr, s32 value);

void func_800A38F8(void) {
    s32 addr = D_800F5BB0;
    s32 value = *(u8 *)0x80394104;

    if (addr < 0x2000) {
        ((u8 *)0x80380000)[addr & 0x7FF] = value;
    } else if (addr < 0x8000) {
        func_800A2178(addr, value);
    }
}
