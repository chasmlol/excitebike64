#include "ctx.h"

void func_800BCB04(s32 a0);

void func_800C0264(s32 *param_1, u32 param_2)
{
    s32 i;
    u32 *p;
    u32 v;

    if (param_2 == 0) {
        return;
    }
    i = 0;
    if (param_1[0] <= 0) {
        return;
    }
    do {
        if ((param_1[1] & ~3) == (param_2 & ~3)) {
            p = (u32 *)(i * 4 + param_1[1]);
            v = *p;
            if (v == 0) {
                return;
            }
            if ((v & 2) == 0) {
                func_800BCB04(v);
            }
            *p = 0;
            return;
        }
        do { i++; } while (0);
    } while (i < param_1[0]);
}
