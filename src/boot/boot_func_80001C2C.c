#include "ctx.h"

extern volatile s32 D_80018100;

void boot_func_80001C2C(s32 param_1) {
    D_80018100 = 0;
    while (D_80018100 < param_1) {
        D_80018100 = D_80018100 + 1;
        D_80018100;
    }
}
