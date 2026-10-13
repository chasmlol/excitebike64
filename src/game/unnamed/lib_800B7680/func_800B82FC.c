#include "ctx.h"

extern s32 D_800D33E0;
extern s32 D_800D33E8;
extern s32 D_800D33F0;

void func_8000EE20(void);
void func_8000EE28(void);
void func_800B78F0(s32 param_1);
void func_800BCB04(s32 param_1);

void func_800B82FC(s32 *param_1)
{
    s32 *piVar1;
    s32 *piVar2;
    s32 iVar3;
    s32 *iVar4;
    s32 *pv;

    func_8000EE20();
    piVar1 = (s32 *)D_800D33E0;
    if (piVar1 == param_1) {
        D_800D33E0 = *param_1;
    } else {
        piVar2 = piVar1;
        while (piVar2 != NULL) {
            if (*piVar2 == (s32)param_1) {
                *piVar2 = *param_1;
                break;
            }
            piVar2 = (s32 *)*piVar2;
        }
    }
    func_8000EE28();
    if (param_1[1] != -1) {
        iVar3 = param_1[1];
        if (D_800D33F0 != 0) {
            func_8000EE20();
            for (iVar4 = (s32 *)D_800D33E8; iVar4 != NULL; iVar4 = (s32 *)iVar4[1]) {
                if (iVar4[4] == iVar3) {
                    func_8000EE28();
                    pv = iVar4;
                    goto LAB_800b83c8;
                }
            }
            func_8000EE28();
            pv = NULL;
LAB_800b83c8:
            if (pv != NULL) {
                pv[0x13] |= 1;
            }
        }
        param_1[1] = -1;
    }
    if (param_1[2] != -1) {
        func_800B78F0(param_1[2]);
        param_1[2] = -1;
    }
    if (param_1[3] != -1) {
        func_800B78F0(param_1[3]);
        param_1[3] = -1;
    }
    func_800BCB04(param_1[6]);
    func_800BCB04((s32)param_1);
    return;
}
