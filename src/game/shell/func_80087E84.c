#include "ctx.h"

extern s32 D_800EFD70[];
extern u8 *D_800EFD64;
extern u8 D_800CB210[];

void func_80087E84(s32 a) {
    if (a != 0) {
        D_800EFD64 = D_800CB210 + D_800EFD70[4] * 0xA0;
        D_800EFD70[0] |= 0x80;
    }
}
