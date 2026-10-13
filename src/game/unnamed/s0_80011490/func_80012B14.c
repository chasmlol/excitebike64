#include "ctx.h"

extern u8 D_800E35D0[250];
extern u32 D_800E36CC;

static inline u8 r250(void) {
    u32 i = D_800E36CC;
    u32 j;
    u8 r;

    if (i >= 147) {
        j = i - 147;
    } else {
        j = i + 103;
    }
    r = D_800E35D0[i] ^= D_800E35D0[j];
    if (++D_800E36CC >= 250) {
        D_800E36CC = 0;
    }
    return r;
}

void func_80012B14(void) {
    u32 seed;
    s32 i;
    u8 *p;
    u8 msb;
    u8 mask;

    D_800E36CC = 0;
    p = D_800E35D0;
    seed = 12345;
    for (i = 250; i != 0; i--) {
        seed = (seed * 1277) & 0xFFFF;
        *p++ = seed >> 8;
    }
    msb = 0x80;
    mask = 0xFF;
    p = &D_800E35D0[14];
    while (msb != 0) {
        *p &= mask;
        *p |= msb;
        p += 11;
        msb >>= 1;
        mask >>= 1;
    }
    for (i = 0; (u32)i < 500; i++) {
        r250();
    }
}
