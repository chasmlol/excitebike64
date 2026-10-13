#include "ctx.h"

extern u8 *D_800CA930;
extern u8 D_800CADE4[];
extern u32 D_800F0600;
extern u32 D_800F05D4;

u32 func_8005AF90(void) {
    u8 *e = D_800CADE4;
    s16 idx = *(s16 *)(D_800CA930 + 0x64);
    e = e + idx * 40;
    if (e[0xC] & 4) {
        return ((D_800F0600 >> 12) ^ 1) & 1;
    }
    if (*(u32 *)(e + 0x1C) != 0) {
        return 0;
    }
    return ~D_800F05D4 >> 31;
}
