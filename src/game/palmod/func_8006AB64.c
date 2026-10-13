#include "ctx.h"

extern s32 D_800E1B70;

void func_8006AB64(u16 *dst, u16 *src)
{
    s32 i;
    u32 v;
    s32 g, b, avg;

    for (i = 0; i < 0x100; i++) {
        v = *src;
        g = (v >> 3) & 0xF8;
        b = (v << 2) & 0xF8;
        v = (v >> 8) & 8;
        avg = (s32)(v + g + b) / 3;
        v += ((avg - (s32)v) * D_800E1B70) >> 8;
        v = (s32)v >> 3;
        g += ((avg - g) * D_800E1B70) >> 8;
        g >>= 3;
        b += ((avg - b) * D_800E1B70) >> 8;
        b >>= 3;
        avg = (v << 11) | (g << 6) | (b << 1);
        avg |= *dst & 1;
        *dst = avg;
        dst++;
        src++;
    }
}
