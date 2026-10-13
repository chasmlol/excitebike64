#include "ctx.h"

extern s32 *D_800C2354;
extern s32 D_800F13F0;
extern s32 D_800F13EC;
extern s32 D_800EFD70;
extern u32 D_800E2038;
extern s32 D_800F04C4;
extern char D_800D8FFC[];
extern char D_800D9010[];
extern char D_800D9024[];
extern char D_800D9044[];
extern char D_800D9060[];
extern char D_800D9078[];
extern char D_800D9090[];
extern char D_800D90A8[];
extern char D_800D90C8[];
extern char D_800D90DC[];
extern char D_800D90F4[];

s32 func_800C2224(void);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
void func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 x, s32 y);
void func_800C1394(u32 *p);

void func_8006EB34(u32 *param_1)
{
    s32 iVar3;
    s32 h;
    s32 iVar4;
    s32 *piVar1;
    char *pcVar2;
    s32 *pf;

    func_800C2224();
    iVar4 = D_800F13F0 / 2 - 12;
    if ((D_800EFD70 & 0x200000) != 0) {
        iVar4 = D_800F13F0 / 2 - 24;
    }
    func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 7, 0x21);
    func_800C1F2C((s32 *)*D_800C2354, D_800D8FFC);
    func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4);
    if ((D_800E2038 & 0x20) != 0) {
        func_800C1E60((s32 *)*D_800C2354, 0x40, 5, 4, 0x21);
        func_800C1F2C((s32 *)*D_800C2354, D_800D9010);
        func_800C0ACC((s32 *)*D_800C2354, *(volatile s32 *)&D_800F13EC / 2, *(volatile s32 *)&D_800F13F0 * 7 / 8);
    }
    func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 2, 0x21);
    if ((D_800EFD70 & 0x400000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9024);
        goto LAB_after;
    }
    if ((D_800EFD70 & 0x100000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9044);
        goto LAB_after;
    }
    if ((D_800EFD70 & 0x800000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9060);
        goto LAB_after;
    }
    func_800C1F2C((s32 *)*D_800C2354, D_800D9078);
LAB_after:
    func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4 + 0x18);
    pf = (s32 *)&D_800EFD70;
    if ((pf[0] & 0x200000) != 0) {
        func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 4, 0x21);
        switch (pf[0x1D5]) {
        case 1:
            func_800C1F2C((s32 *)*D_800C2354, D_800D9090);
            break;
        case 2:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90A8);
            break;
        case 3:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90C8);
            break;
        case 4:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90DC);
            break;
        case 5:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90F4);
            break;
        default:
            goto LAB_end;
        }
    LAB_end:
        func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4 + 0x30);
    }
    func_800C1394(param_1);
}
