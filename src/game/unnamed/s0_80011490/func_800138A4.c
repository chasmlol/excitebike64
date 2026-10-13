#include "ctx.h"

extern u32 D_800E36CC;
extern u8 D_800E35D0[];

u8 func_800138A4(void) {
    u32 x = D_800E36CC;
    u32 idx = x + 0x67;
    u8 r;
    u32 n;

    if (x > 0x92) {
        idx = x - 0x93;
    }
    r = D_800E35D0[x] ^ D_800E35D0[idx];
    D_800E35D0[x] = r;
    n = D_800E36CC + 1;
    D_800E36CC = n;
    if (n > 0xF9) {
        D_800E36CC = 0;
    }
    return r;
}
