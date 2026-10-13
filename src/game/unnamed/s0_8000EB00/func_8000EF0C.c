#include "ctx.h"

void func_8000EF0C(u32 param_1, char *param_2) {
    s32 i;
    s32 d;
    for (i = 0; i < 8; i++) {
        d = (param_1 >> (28 - 4 * i)) & 0xF;
        if (d >= 10) {
            d += 7;
        }
        d += '0';
        param_2[i] = d;
    }
    param_2[i] = 0;
}
