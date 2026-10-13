#include "ctx.h"

extern u32 D_800F4A10[4];

void func_800BCB04(u32 v);

void func_8008C744(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        func_800BCB04(D_800F4A10[i]);
    }
}
