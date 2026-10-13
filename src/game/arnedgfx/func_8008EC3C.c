#include "ctx.h"

extern s32 D_800F4F8C;
extern s32 D_800F4F90;
extern u8 *D_800CE1FC;
extern s32 *D_800F55A0;

s16 *func_8008EC3C(s32 param_1, s32 param_2)
{
    s32 w;
    s32 h;
    s32 iVar5;
    s32 iVar8;
    s32 *puVar1;
    s32 *puVar7;
    s16 *psVar2;
    s16 *base;
    u32 cnt;
    s32 iVar6;
    s32 q5;
    s32 q8;
    u32 uVar3;
    u32 uVar4;

    w = D_800F4F8C;
    iVar8 = param_1 - w * 250;
    h = D_800F4F90;
    iVar5 = param_2 - h * 250;
    q5 = iVar5 / 500;
    q8 = iVar8 / 500;
    puVar1 = (s32 *)(D_800CE1FC + (q5 * w + q8) * 0x14);
    puVar7 = (s32 *)*puVar1;
    if (puVar7 == NULL || (iVar6 = *(s16 *)(puVar1 + 2)) < 0) {
        return NULL;
    }
    cnt = *(u32 *)puVar7;
    base = (s16 *)D_800F55A0[iVar6];
    uVar4 = 0;
    if (cnt != 0) {
        uVar3 = cnt;
        psVar2 = base;
        do {
            uVar4 = uVar4 + 1;
            if (psVar2[0] == iVar8 && psVar2[2] == iVar5) {
                return psVar2;
            }
            psVar2 += 8;
        } while (uVar4 < uVar3);
    }
    return NULL;
}
