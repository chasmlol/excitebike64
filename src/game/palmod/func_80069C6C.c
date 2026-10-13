#include "ctx.h"

extern void func_800BCB04();
extern s32 D_800F1458;

void func_80069C6C(void) {
    if (D_800F1458 != 0) {
        func_800BCB04(D_800F1458);
        D_800F1458 = 0;
    }
}
