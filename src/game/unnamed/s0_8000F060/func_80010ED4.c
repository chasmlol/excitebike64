#include "ctx.h"

extern s32 D_800C2B58;
extern s32 D_800CA848;
extern s32 D_800CA850;

s32 func_80010ED4(s32 param_1)
{
    switch (param_1) {
    case 0:
        return D_800C2B58;
    case 1:
        return D_800CA848;
    case 2:
        return D_800CA850;
    default:
        return 0;
    }
}
