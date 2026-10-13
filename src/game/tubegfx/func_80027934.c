#include "ctx.h"

extern s32 *D_800E77F8;
extern s32 D_800E7790;
extern u8 *D_800E77C8;
extern u8 *D_800E782C;
extern u8 *D_800E7830;

void func_80027934(u32 *param_1)
{
    s32 *piVar1;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    u8 *pb;
    u32 uVar3;

    uVar3 = *param_1;
    iVar5 = D_800E77F8[0] - 1;
    if (((u32)D_800E77F8[3] <= uVar3) && (uVar3 <= *(u32 *)((u8 *)D_800E77F8 + (iVar5 << 3) + 0xC)) &&
        (-1 < iVar5)) {
        iVar6 = 0;
        piVar1 = D_800E77F8;
        while (iVar6 <= iVar5) {
            if ((u32)piVar1[3] == uVar3) {
                break;
            }
            iVar6 = iVar6 + 1;
            piVar1 = piVar1 + 2;
        }
        if ((iVar6 <= iVar5) && (1 < *(s32 *)(D_800E782C + iVar6 * 4))) {
            iVar5 = 0;
            while (iVar5 < D_800E7790) {
                if (D_800E77C8[iVar5 * 4 + 2] + D_800E77C8[iVar5 * 4] == iVar6) {
                    break;
                }
                iVar5 = iVar5 + 1;
            }
            if (iVar5 == D_800E7790) {
                if (iVar5 <= 7) goto store;
                do {
                } while (1);
store:
                D_800E77C8[D_800E7790 * 4] = D_800E7830[iVar6 * 4 + 3];
                D_800E77C8[D_800E7790 * 4 + 1] = D_800E782C[iVar6 * 4 + 3];
                D_800E77C8[D_800E7790 * 4 + 2] = (u8)(iVar6 - D_800E7830[iVar6 * 4 + 3]);
                D_800E7790 = D_800E7790 + 1;
            }
            *param_1 = (u32)((iVar5 + 8) << 24);
        }
    }
}
