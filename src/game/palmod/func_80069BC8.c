#include "ctx.h"

void func_80069BC8(u16 *dst, u16 *src, s32 x, s32 y) {
    s32 i4;
    s32 i2;
    s32 k;
    u16 *d;

    k = 0;
    for (i4 = y - 8; i4 < y + 8; i4++) {
        i2 = x - 8;
        if (i2 < x + 8) {
        for (; i2 < x + 8; i2++) {
            if (i4 >= 0 && i2 >= 0 && i4 < 0x80 && i2 < 0x80) {
                d = (u16 *)((s32)(i4 * 0x80 + i2) * 2 + (s32)dst);
                if (src[k] < *d) {
                    *d = src[k];
                }
            }
            k++;
        }
        }
    }
}
