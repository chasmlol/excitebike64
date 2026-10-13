#include "ctx.h"

extern s32 D_800E1B70;

void func_8006AA9C(u16 *dst, u8 *src)
{
    s32 i;
    s32 r, g, b, c;

    for (i = 0; i < 0x100; i++) {
        r = *src++;
        g = *src++;
        b = *src++;
        c = (r + g + b) / 3;
        r += ((c - r) * D_800E1B70) >> 8;
        g += ((c - g) * D_800E1B70) >> 8;
        b += ((c - b) * D_800E1B70) >> 8;
        c = (r << 11) | (g << 6) | (b << 1);
        c |= *dst & 1;
        *dst = c;
        dst++;
    }
}
