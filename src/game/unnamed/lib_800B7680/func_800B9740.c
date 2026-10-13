#include "ctx.h"

extern s32 D_800D33F0;
extern s32 D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

s32 func_800B9740(s32 param_1)
{
    s32 *n;
    s32 *found;
    s32 r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = (s32 *)D_800D33E8; n != 0; n = (s32 *)n[1]) {
        if (n[4] == param_1) {
            func_8000EE28();
            found = n;
            goto check;
        }
    }
    func_8000EE28();
    found = 0;
check:
    r = -1;
    if (found != 0) {
        r = found[7];
        if (r == 0) {
            r = found[8];
        }
    }
    return r;
}
