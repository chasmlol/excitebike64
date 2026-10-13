#include "ctx.h"

extern s32 D_80101350;
extern s32 D_80101354;
extern s32 D_80101358;
extern s32 D_8010135C;
extern s32 D_80105370;
extern u32 D_80105374;
extern s32 D_80105378;
s32 func_800C1C20(s32);

s32 func_800C1A6C(u8 *hdr)
{
    u8 *e;
    u32 i;
    u32 j;
    u32 h, w;
    u8 *src;
    u8 *dst;
    u8 *tv;

    e = hdr + 0x10;
    for (i = 0; i < hdr[7]; i++) {
        {
            j = e[5] >> 8;
            D_80101358 = e[4] + 1;
            D_8010135C = e[5] + 1;
            while (j < 0x28) {
                if (func_800C1C20(D_80105370 + (j << 8)) != 0) break;
                j++;
            }
            if (j == 0x28) {
                return -1;
            }
            if (D_80105374 < j) {
                D_80105374 = j;
            }
            h = e[5];
            w = e[4];
            src = e + *(u16 *)e;
            dst = (u8 *)(D_80105378 - (j << 12));
            dst = dst + D_80101350 + D_80101354 * 0x40;
            while (--h != (u32)-1) {
                while (--w != (u32)-1) {
                    *dst++ = *src++;
                }
                w = e[4];
                tv = dst + 0x40;
                dst = tv - w;
            }
            e[7] = j;
            e[0] = D_80101350;
            e[1] = D_80101354;
            e += 8;
        }
    }
    return 0;
}
