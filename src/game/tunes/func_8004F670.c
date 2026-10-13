#include "ctx.h"

extern s32 D_800EFD20;
extern s32 D_800CA85C;
extern s32 D_800EFD50;
extern s32 *D_800EECC4;

void func_8004F670(s32 *p, s32 n)
{
    if (D_800EFD20 < n) {
        n = D_800EFD20;
    }
    if (D_800CA85C == 0) {
        D_800EFD20 = D_800EFD20 - n;
        while (n != 0) {
            *p = *p + D_800EECC4[D_800EFD50];
            p++;
            *p = *p + D_800EECC4[D_800EFD50];
            p++;
            D_800EFD50 = (D_800EFD50 + 1) & 0x7FF;
            n--;
        }
    }
}
