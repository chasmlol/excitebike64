#include "ctx.h"

extern s32 D_800D56BC;
extern s32 D_800D5628;
extern void (*D_800D5630)(void);
extern void (*D_800D56B0)(s32 *a0, u32 a1);
extern s32 D_800D5638;
extern s32 D_800D5620;
extern s32 *D_80100610;
extern s32 D_80100614;
extern u32 D_80100618;
extern u8 *D_8010061C;
extern s32 D_80100624;
extern s32 D_8010062C;

s32 func_800AD5F0(void);
void func_800AD720(u8 *a0, s32 a1);
void func_800BB09C(s32 *a0, u32 a1);
void func_800BB7B4(s32 a, s32 b, s32 c);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AD910(u8 *a0, s32 a1);

void func_800BBDE0(void)
{
    s32 uVar1;
    s32 *piVar3;
    u32 h;
    s32 iVar4;

    uVar1 = func_800AD5F0();
    if (uVar1 >= 0) {
        iVar4 = 0;
        if ((uVar1 & 0x40000000) == 0) {
            D_800D56BC = D_800D56BC + 1;
        }
        D_800D5628 = D_800D5628 ^ 1;
        func_800AD720(D_8010061C + D_80100614 * D_800D5628, D_80100614);
        piVar3 = D_80100610;
        do {
            if (D_800D5630 != 0) {
                (*D_800D5630)();
            }
            func_800BB09C(piVar3, D_80100618 >> 1);
            if (D_800D56B0 != 0) {
                (*D_800D56B0)(piVar3, D_80100618 >> 1);
            }
            h = D_80100618 >> 1;
            if (D_80100624 & 1) {
                piVar3 += h * 2;
            } else {
                piVar3 += h;
            }
            iVar4 = iVar4 + 1;
        } while (iVar4 < 2);
        func_800BB7B4((s32)D_80100610, (s32)(D_8010061C + D_80100614 * D_800D5628), D_80100618);
        if (D_800D5638 != 0 || D_800D5620 != 0) {
            func_8000ED10(D_8010061C + D_80100614 * D_800D5628, 0, D_80100614);
            if (D_800D5638 != 0) {
                D_800D5638 = D_800D5638 - 1;
            }
        }
        func_800AD910(D_8010061C + D_80100614 * D_800D5628, D_80100614);
        D_8010062C = D_8010062C + 1;
    }
}
