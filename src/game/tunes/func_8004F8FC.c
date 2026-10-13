#include "ctx.h"

extern s32 D_800C8CD8;
extern u32 D_800E3700;
extern u32 D_800E76D0;
extern u32 D_800EFD28[];
extern u32 D_800EFD70[];

s32 func_8004F8FC(u32 *p)
{
    u32 *base;
    u32 u;
    if (D_800C8CD8 != 0) {
        if (*p == D_800E3700) {
            return 1;
        }
    }
    base = D_800EFD70;
    if (base[19] == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    u = base[*p + 7];
    *p = u;
    if (u < 6) {
        return (base[19] == 9) << 1;
    }
    return 1;
}
