#include "ctx.h"

s32 func_800A2444();

s32 func_800A4204(s32 addr)
{
    addr &= 0xFFFF;
    if (addr < 0x2000) {
        addr &= 0x7FF;
    } else if (addr < 0x8000) {
        return func_800A2444(addr);
    }
    return *(u8 *)(addr + 0x80380000);
}
