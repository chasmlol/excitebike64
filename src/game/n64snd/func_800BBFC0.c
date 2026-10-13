#include "ctx.h"

void func_800BB47C(s32 *arg0, s32 *arg1, s32 *arg2);

s32 func_800BBFC0(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    func_800BB47C(arg0, arg1, arg2);
    *arg1 = 3;
    *arg3 = 20;
    if (*arg2 > 22050) {
        *arg2 = 22050;
    }
    return 0;
}
