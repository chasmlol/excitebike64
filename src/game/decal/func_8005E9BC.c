#include "ctx.h"

extern s32 D_800F0798[8];
extern s32 D_800F07D8[8];
extern u8 *D_800F0774;

void func_8005E9BC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        D_800F0798[i] = 0;
        D_800F07D8[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        D_800F0774[i * 0x18] = 0;
    }
}
