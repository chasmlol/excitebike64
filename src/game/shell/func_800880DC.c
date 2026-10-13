#include "ctx.h"

extern u32 D_800EFD70[];
extern u8 *D_800EFD64;
extern u8 D_800CC1B0[];
extern u32 D_800CA924[3];

void func_800880DC(s32 a0) {
    s32 i;
    s8 *b;
    u32 *w;
    if (a0 == 0) {
        return;
    }
    w = D_800EFD70;
    w[15] = 1;
    b = (s8 *)w;
    D_800EFD64 = D_800CC1B0;
    w[19] = 4;
    w[0] = w[0] & 0xFFFFFF7F;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            b[0x2C8] = 0;
        } else {
            b[i + 0x2C8] = -1;
        }
        if (i == 0) {
            b[0x2CC] = 0;
        } else {
            b[i + 0x2CC] = -1;
        }
    }
    D_800CA924[1] = 0;
    D_800CA924[0] = 1;
    D_800CA924[2] = 0;
}
