#include "ctx.h"

void func_800A2178(s32 a, s32 b);

void func_800A4254(s32 addr, s32 val) {
    if (addr < 0x2000) {
        u8 *ram = (u8 *)0x80380000;
        ram[addr & 0x7FF] = val;
    } else if (addr < 0x8000) {
        func_800A2178(addr, val);
    }
}
