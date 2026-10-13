#include "ctx.h"

extern s32 D_800D33F0;
extern s32 *D_800D33E8;
extern void func_8000EE20(void);
extern void func_8000EE28(void);

void func_800B94A0(s32 param_1, s32 param_2, s32 param_3)
{
    s32 *n;
    s32 *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = (s32 *)n[1]) {
            if (n[4] == param_1) {
                func_8000EE28();
                q = n;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q[0xf] = param_3;
            q[0x10] = param_2;
        }
    }
}
