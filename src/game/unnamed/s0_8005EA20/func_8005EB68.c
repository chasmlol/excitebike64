#include "ctx.h"

extern s32 D_800F0A34;
extern s32 D_800F0A28;
extern u8 *D_800F0954;
extern s32 D_800F0910;

void func_8005EB68(s32 v, s32 n)
{
    s32 b;

    if (D_800F0A34 != 0) {
        return;
    }
    while (1) {
        v &= ((1 << n) - 1) << (8 - n);
        b = v & 0xFF;
        { s32 sh = 8 - D_800F0A28; *D_800F0954 |= b >> sh; }
        if (n < D_800F0A28) {
            D_800F0A28 -= n;
            return;
        }
        n -= D_800F0A28;
        v = b << D_800F0A28;
        D_800F0A28 = 8;
        D_800F0954++;
        D_800F0910--;
        if (D_800F0910 == 0) {
            D_800F0A34 = 1;
        }
    }
}
