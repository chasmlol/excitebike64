#include "ctx.h"

extern u8 *D_800E1550;

void func_8000BB34(u8 *src, s32 x, s32 y, s32 w, s32 h)
{
    s32 i, j, xx;

    i = 0;
    do {
        if (h > 0) {
            do {
                if (y < 0) goto next;
                if (y >= 16) goto next;
                do { xx = x; } while (0);
                for (j = 0; j < w; j++, xx++) {
                    if (xx < 0) continue;
                    if (xx >= 32) continue;
                    D_800E1550[y * 32 + xx] = src[i * 64 + j];
                }
                do { next: i++; y++; } while (0);
            } while (i < h);
        }
    } while (0);
}
