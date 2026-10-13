#include "ctx.h"

void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3)
{
    s32 iVar1;

    iVar1 = param_1[0] * param_2[0] + param_1[1] * param_2[3] + param_1[2] * param_2[6];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[0] = iVar1 >> 14;
    iVar1 = param_1[0] * param_2[1] + param_1[1] * param_2[4] + param_1[2] * param_2[7];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[1] = iVar1 >> 14;
    iVar1 = param_1[0] * param_2[2] + param_1[1] * param_2[5] + param_1[2] * param_2[8];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[2] = iVar1 >> 14;
}
