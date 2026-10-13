#include "ctx.h"

extern u32 D_80105370;
extern u32 D_80105380[16];

void func_800BCB04(u32 v);

u32 func_800C1DEC(void) {
    u32 i;
    if (D_80105370 != 0) {
        func_800BCB04(D_80105370);
        D_80105370 = 0;
    }
    if (D_80105380[0] != 0) {
        func_800BCB04(D_80105380[0]);
        D_80105380[0] = 0;
    }
    for (i = 1; i < 16; i++) {
        D_80105380[i] = 0;
    }
    return 0;
}
