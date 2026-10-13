#include "ctx.h"

extern u16 *D_800F06B0;
extern u8 D_800D7340[];
extern s32 D_800E7728;
extern s32 D_800D0D00;
extern s32 D_800D11B0;
extern s32 D_800D09F8;
extern s32 D_800D10E8;
extern s32 D_800D0DEC;
extern s32 D_800D09D4;
extern s32 D_800D1084;
extern s32 D_800F13F0;
extern u8 D_800D8424[];
extern u8 D_800EFD70[];
extern s32 D_800EFDBC;
extern s32 D_800F0524;
extern s32 D_800F0528;
extern s32 D_800EFD60;
extern s32 D_800F0740;
extern s32 D_800EFD68;
extern s32 D_800EFD6C;
extern u16 D_800F0744[];
extern void *D_800F0730[];
extern void *D_800F0530[];
extern void *D_800F06B8[];

void *func_800BC908(s32 size, s32 zero, void *file, s32 line);
s32 func_800BF79C(s32 a, s32 b);
void func_800BFC40(void *a, void *b);
void func_800AE540(void *a, void *b, f32 c, f32 d, f32 e, f32 f, f32 g);
void *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
void func_80014D20(void);

void func_80052C90(void)
{
    s32 iVar2;
    s32 iVar3;
    u16 *puVar4;
    s32 auStack_50[16];
    s32 i;
    s32 t13;
    u8 *s2;

    if (D_800EFDBC == 9) {
        D_800F06B0 = (u16 *)func_800BC908(0x200, 0, D_800D7340, 0xB4B);
        *D_800F06B0 = 0;
        puVar4 = D_800F06B0 + 1;
        iVar3 = 1;
        do {
            iVar2 = iVar3 >> 4;
            *puVar4 = (u16)(iVar2 << 1) | (u16)((iVar2 << 6) | 1) | (u16)(iVar2 << 11);
            puVar4 = puVar4 + 1;
            iVar3 = iVar3 + 1;
        } while (iVar3 < 0x100);
    }
    D_800EFD60 = func_800BF79C(D_800E7728, D_800D0D00);
    s2 = D_800EFD70;
    if (1 < *(s32 *)(s2 + 0x3C)) {
        D_800F0524 = func_800BF79C(D_800E7728, D_800D11B0);
    }
    if (*(s32 *)(s2 + 0x4C) == 9) {
        *(s32 *)(s2 + 0x730) = func_800BF79C(D_800E7728, D_800D09F8);
        D_800F0740 = func_800BF79C(D_800E7728, D_800D10E8);
        D_800EFD6C = func_800BF79C(D_800E7728, D_800D0DEC);
        D_800EFD68 = func_800BF79C(D_800E7728, D_800D09D4);
    }
    func_800BFC40(D_800D8424, func_80014D20);
    if (*(s32 *)(s2 + 0x3C) == 1) {
        D_800F0730[0] = func_800BC908(0x1c0, 0, D_800D7340, 0xB6B);
        D_800F0730[1] = func_800BC908(0x1c0, 0, D_800D7340, 0xB6C);
    }
    t13 = *(volatile s32 *)&D_800F13F0;
    func_800AE540(auStack_50, D_800F0744, 33.0f, 320.0f / (f32)t13, 50.0f, 4192.0f, 1.0f);
    iVar3 = 0;
    if (*(s32 *)(s2 + 0x4C) == 2) {
        D_800F0528 = func_800BF79C(D_800E7728, D_800D1084);
    }
    for (i = 0; i < 0x1E; i++) {
        D_800F0530[i] = func_800C1E60(0, 0x40, 2, 2, 0);
        D_800F06B8[i] = func_800C1E60(0, 0x40, 2, 2, 0);
    }
}
