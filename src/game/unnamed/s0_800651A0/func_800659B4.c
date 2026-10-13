#include "ctx.h"

extern u8 D_800CCC20[];
extern u16 *D_800C29D8;

void func_800659B4(s32 x, s32 y, u32 bits)
{
    s32 i, j, k;
    u8 *r;
    u16 *p;

    for (i = 0; i < 8; i++) {
        r = D_800CCC20 + ((bits >> ((7 - i) * 4)) & 0xF) * 8;
        p = D_800C29D8 + (y * 0x140 + x);
        for (j = 0; j < 8; j++) {
            k = 7;
            do {
                if (((*r >> k) & 1) != 0) {
                    *p = 0xFFFF;
                } else {
                    *p = 1;
                }
                k--;
                p++;
            } while (k >= 0);
            r++;
            p += 0x140 - 8;
        }
        x += 8;
    }
}
