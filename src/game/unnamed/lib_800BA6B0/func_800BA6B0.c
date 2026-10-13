#include "ctx.h"

extern u32 D_800FE8DC;
extern s32 D_800FE8D8;
extern s32 D_800FE8E0;
extern s32 D_801004F0;
extern s32 D_800D5608;
extern s32 *D_800FE8D4;
extern s32 *D_800FE8D0;
extern s32 D_800FE8F0;
extern s32 D_800DB170;
extern s16 D_800D3404[];
extern s32 D_800DB180;
extern s32 D_800DB184;
extern s32 D_800DB1A4;
extern s32 D_800DB1BC;
extern s32 D_800D5604;
extern s32 *D_800DB8B8;
extern s32 D_800DB8BC;
extern s32 *D_800DB8CC;

s32 *func_800BC908(s32 a, s32 b, s32 *c, s32 d);
void func_800BCB04(s32 *p);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000EB00(s32 a, ...);
s32 func_800BF530(s32 *a, s32 *b);
void func_800BFC40(s32 *a, void (*f)(void));
void func_800BB350(void);

s32 func_800BA6B0(s32 param_1, u32 param_2, s32 param_3)
{
    s32 *piVar1;
    s32 iVar2, iVar3, iVar4, iVar6;
    s16 *psVar5;

    if (param_1 < 9 && param_3 < 0x5623) goto body;
ret1:
    return 1;
body:
    {
        D_800FE8DC = param_2;
        D_800FE8D8 = param_3;
        D_800FE8E0 = param_1;
        D_801004F0 = 0;
        D_800D5608 = 0;
        D_800FE8D4 = func_800BC908(param_1 * 0x30, 0, &D_800DB170, 0x2c8);
        if (D_800FE8D4 == 0) goto ret1;
        {
            func_8000ED10((u8 *)D_800FE8D4, 0, param_1 * 0x30);
            func_8000ED10((u8 *)&D_800FE8F0, 0, 0x1c00);
            D_800FE8D0 = piVar1 = func_800BC908(0x8000, 0, &D_800DB170, 0x2d4);
            if (piVar1 == 0) {
                func_800BCB04(D_800FE8D4);
                return 2;
            }
            iVar6 = 0;
            if (param_2 & 2) {
                s32 *tp = piVar1;
                do {
                    iVar4 = 0;
                    do {
                        s32 prod = D_800D3404[iVar6] * iVar4;
                        tp[iVar6 + iVar4 * 0x100] = prod / 0x1f;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            } else {
                s32 *tp = piVar1;
                s32 sh;
                do {
                    iVar4 = 0;
                    sh = (s8)iVar6;
                    iVar3 = 0;
                    do {
                        tp[iVar6 + iVar4 * 0x100] = iVar3 / 0x1f;
                        iVar3 = iVar3 + sh;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            }
            D_800DB8B8 = D_800FE8D4;
            D_800DB8BC = D_800FE8E0;
            D_800DB8CC = D_800FE8D0;
            if (func_800BF530(&D_800DB180, &D_800D5604) != 0) {
                func_8000EB00((s32)&D_800DB184);
                D_800D5604 = 0;
            } else {
                func_8000EB00((s32)&D_800DB1A4);
            }
            func_800BFC40(&D_800DB1BC, func_800BB350);
            return 0;
        }
}
}
