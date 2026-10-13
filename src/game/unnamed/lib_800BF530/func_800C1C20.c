#include "ctx.h"

extern u32 D_80101358;
extern s32 D_8010135C;
extern s32 D_80101350;
extern s32 D_80101354;
extern u32 D_800D5770[];
extern u32 D_800D57F0[];

s32 func_800C1C20(u32 *map)
{
    u32 w = (D_80101358 + 1) >> 1;
    s32 h = D_8010135C;
    s32 y = 1;
    s32 xmax = 0x20 - w;
    s32 ymax = 0x40 - h;
    u32 mask = D_800D5770[w - 1];
    u32 bits0 = D_800D57F0[w - 1];
    u32 acc;
    u32 bits;
    u32 *pp;
    s32 i;
    s32 x;

    if (y < ymax) {
        do {
            acc = 0;
            pp = map + h;
            for (i = h; i != 0; i--) {
                pp--;
                acc |= *pp;
                if ((acc & mask) == mask) {
                    break;
                }
            }
            if (i != 0) {
                y += i;
                map += i;
                continue;
            }
            acc = ~acc;
            bits = bits0;
            x = 0;
            if (xmax > 0) {
                do {
                    if ((acc & bits) == bits) {
                        goto found;
                    }
                    x++;
                    bits >>= 1;
                } while (x < xmax);
            }
            y++;
            map++;
        } while (y < ymax);
    }
    return 0;

found:
    while (--h != -1) {
        *map++ |= bits;
    }
    D_80101350 = x << 1;
    D_80101354 = y;
    return 1;
}
