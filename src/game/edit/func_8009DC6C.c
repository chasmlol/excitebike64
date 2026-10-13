#include "ctx.h"

typedef struct {
    s16 h0;
    s16 h1;
    u8 rest[60];
} CeRow;

extern u8 *D_800CEA40;
extern CeRow D_800CE280[];
extern s16 D_800F55B8[][15];

void func_8001254C(s32 a0, s32 a1, u16 *a2, s32 *a3);

#define LV() (*(s32 *)(D_800CEA40 + 0x16FC))
#define CE2() (D_800CE280[LV() + 0x12].h1 << 1)

s32 func_8009DC6C(s32 param_1)
{
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 w38;
    s32 iStack_34;
    s32 w30;
    u32 uVar1;

    if (param_1 > 0 && param_1 < CE2()) {
        iVar5 = D_800F55B8[LV()][param_1 - 1];
    } else {
        iVar5 = 0;
    }
    iVar4 = iVar5;
    iVar3 = param_1 - 1;
    while (1) {
        D_800F55B8[LV()][iVar3] = (s16)((u16)D_800F55B8[LV()][iVar3] - 1);
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = D_800F55B8[LV()][iVar3];
        } else {
            iVar2 = 0;
        }
        if (iVar3 > 0 && iVar3 < CE2()) {
            iVar2 = iVar2 - D_800F55B8[LV()][param_1 - 2];
        }
        func_8001254C(4000, iVar2 << 4, (u16 *)&w38, &iStack_34);
        ((u16 *)&w38)[0] = 0x4000 - ((u16 *)&w38)[0];
        if (param_1 + 1 > 0 && param_1 + 1 < CE2()) {
            iVar2 = D_800F55B8[LV()][param_1];
        } else {
            iVar2 = 0;
        }
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = iVar2 - D_800F55B8[LV()][iVar3];
        }
        func_8001254C(4000, iVar2 << 4, (u16 *)&w30, &iStack_34);
        uVar1 = ((u16 *)&w30)[0];
        ((u16 *)&w30)[0] = 0x4000 - uVar1;
        if (0x1555 < (s16)((0x4000 - uVar1) - ((u16 *)&w38)[0])) {
            break;
        }
        iVar4 = iVar4 - 1;
    }
    D_800F55B8[LV()][param_1 - 1] = (s16)iVar5;
    return iVar4;
}
