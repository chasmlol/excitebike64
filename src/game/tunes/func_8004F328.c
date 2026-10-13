#include "ctx.h"

extern s32 D_800CA858;
extern s32 D_800EFD00[];
extern u8 *D_800EFD4C;

s32 func_8004F328(void)
{
    s32 i, sum, w, f;

    sum = 0;
    for (i = 0; i < D_800CA858; i++) {
        w = *(s32 *)(D_800EFD4C + ((D_800EFD00[i] * 5) << 2) + 0xc);
        f = w * 2;
        if (*(s32 *)(D_800EFD4C + ((f * 5) << 2) + 0x10) & 1) {
            f = w << 2;
        }
        sum += (f * 0x3c) / 0x5622;
    }
    return sum;
}
