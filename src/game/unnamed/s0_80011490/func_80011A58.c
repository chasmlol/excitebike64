#include "ctx.h"

extern s16 D_800C2C60[];

void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4)
{
    s32 iVar1;
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    s32 iVar7;
    s32 iVar8;
    s32 q4;
    s32 q1;

    iVar7 = D_800C2C60[((param_2 >> 4) - 0x400) & 0xfff];
    iVar8 = D_800C2C60[((param_3 >> 4) - 0x400) & 0xfff];
    iVar4 = iVar8 * iVar7;
    iVar6 = D_800C2C60[(param_2 >> 4) & 0xfff];
    iVar3 = D_800C2C60[(param_3 >> 4) & 0xfff];
    iVar5 = D_800C2C60[(param_4 >> 4) & 0xfff];
    iVar2 = D_800C2C60[((param_4 >> 4) - 0x400) & 0xfff];
    if (iVar4 < 0) {
        iVar4 = iVar4 + 0x3fff;
    }
    q4 = iVar4 >> 14;
    iVar1 = iVar8 * iVar6;
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    q1 = iVar1 >> 14;
    param_1[1] = iVar8;
    param_1[0] = (iVar3 * iVar5) >> 14;
    param_1[2] = (iVar3 * iVar2) >> 14;
    param_1[3] = (-iVar5 * q1 + -iVar7 * iVar2) >> 14;
    param_1[4] = (iVar6 * iVar3) >> 14;
    param_1[5] = (-iVar2 * q1 + iVar7 * iVar5) >> 14;
    param_1[6] = (iVar5 * q4 + -iVar6 * iVar2) >> 14;
    param_1[7] = (-iVar7 * iVar3) >> 14;
    param_1[8] = (iVar2 * q4 + iVar6 * iVar5) >> 14;
}
