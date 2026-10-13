#include "ctx.h"

extern u32 D_800E2004;
extern u8 D_800E1C78[];

void func_8000CD00(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000EF8C(char *a0, char *a1);
void func_8008C5E8(u8 *p, s32 n);
s32 func_800B3A40(u8 *tbl, u16 a1, s32 a2, u8 *a3, u8 *a4, s32 a5, s32 *a6);

s32 func_8008C0E4(u32 param_1, char *param_2, char *param_3, s32 param_4, s32 param_5, s32 param_6)
{
    s32 iVar2;
    u8 acStack_38[16];
    u8 acStack_28[8];
    s32 local_20[2];

    if ((D_800E2004 & (1 << param_1)) == 0) {
        iVar2 = -1;
    } else {
        func_8000CD00();
        func_8000ED10(acStack_28, 0, 4);
        func_8000EF8C(acStack_28, param_3);
        func_8000ED10(acStack_38, 0, 0x10);
        func_8000EF8C(acStack_38, param_2);
        func_8008C5E8(acStack_38, 0x10);
        func_8008C5E8(acStack_28, 4);
        iVar2 = func_800B3A40(D_800E1C78 + param_1 * 0x68, (u16)param_4, param_5, acStack_38, acStack_28, param_6, local_20);
        if (iVar2 == 0) {
            iVar2 = local_20[0];
        } else {
            iVar2 = -iVar2;
        }
    }
    return iVar2;
}
