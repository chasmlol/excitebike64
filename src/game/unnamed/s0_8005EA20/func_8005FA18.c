#include "ctx.h"

typedef struct {
    s32 pad0[7];
    s32 w[6];
    u8 pad34[0x43C - 0x34];
    u8 lo[6];
    u8 pad442[0x7AA - 0x442];
    u8 hi[6];
} Tab;

extern u8 *D_800F0A38;
extern s32 D_800F0A24;
extern s32 D_800EFDAC;
extern s32 D_800EFDB4;
extern Tab D_800EFD70;
extern s32 D_800E3700;


static inline s32 getbits(s32 n)
{
    s32 v;
    s32 j;
    s32 b;
    s32 c;

    v = 0;
    for (j = 0; j < n; j++) {
        v <<= 1;
        b = *D_800F0A38;
        c = D_800F0A24 - 1;
        D_800F0A24 = c;
        b = b >> c;
        b = b & 1;
        if (c == 0) {
            D_800F0A38++;
            D_800F0A24 = 8;
        }
        v |= b;
    }
    return v;
}

void func_8005FA18(void)
{
    s32 i;
    s32 n;

    D_800EFDAC = getbits(8);
    D_800EFDB4 = getbits(8);
    getbits(8);
    getbits(8);
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        D_800EFD70.lo[i] = n & 0xF;
        D_800EFD70.hi[i] = (n >> 4) & 0xF;
    }
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        D_800EFD70.w[i] = n & 0x7F;
        if (n & 0x80) {
            D_800E3700 = i;
        }
    }
}
