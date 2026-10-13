#include "ctx.h"

extern s32 D_800C238C[];
extern s32 D_800C8CD8;
extern s32 D_800C8CDC;
extern s32 D_800E3700;
extern s32 D_800C2280;
extern s32 D_800E37C0;

#define IDX(p) ((((s32)(p) - (s32)&D_800E37C0) * -0x41041041) >> 5)

void func_80021CF8(u8 *a2)
{
    s32 idx;
    s32 v;
    u8 b;
    s8 c;
    s32 *base;
    s32 n;
    s32 *q;

    base = D_800C238C;
    idx = IDX(a2);
    q = &base[idx];
    if (D_800C8CD8 != 0 && D_800E3700 == idx) v = 0x80;
    else if (D_800C8CDC != 0) {
        if (idx == 0) v = 0x80; else { *q = 0xFF; goto L74; }
    } else v = 0xFF;
    *q = v;
L74:
    if (a2[0x44] != 0 && a2[0x6E8] != 0 && *(s16 *)(a2 + 0x624) > 0x58) {
        D_800C238C[IDX(a2)] = (0x78 - *(s16 *)(a2 + 0x624)) * 8;
        return;
    }
    if (*(s8 *)(a2 + 0x620) != 0) {
        b = a2[0x621] - 1;
        a2[0x621] = b;
        if ((s32)((u32)b << 24) <= 0) {
            c = a2[0x620] - 1;
            a2[0x620] = c;
            if (c == 0 && D_800C8CD8 == 0) {
                if (a2[0x6E8] == 0) {
                    if ((D_800C2280 & 0x100) == 0) a2[0x41D] = 0;
                } else {
                    a2[0x41D] = 0;
                }
            }
            n = (((s32)((u32)a2[0x620] << 24)) >> 25) - 10;
            a2[0x621] = n;
            if (n < 4) a2[0x621] = 4;
        }
        if (a2[0x620] & 1) D_800C238C[IDX(a2)] = 0;
    }
}
