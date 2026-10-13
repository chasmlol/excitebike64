#include "ctx.h"

extern s32 D_800D33F0;
extern s32 D_800D33F4;

s32 func_800B99CC(void)
{
    if (D_800D33F0 == 0) {
        return 0;
    }
    return D_800D33F4;
}
