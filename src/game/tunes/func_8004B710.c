#include "ctx.h"

extern s32 D_800CA858;
extern s32 D_800CA860;
extern s32 D_800CA85C;
extern s32 D_800CA844;
extern u8 D_800D7330[];
extern u8 D_800D7338[];
extern s32 D_800D5604;
extern s32 D_800D1244;
extern s32 D_800EECD4;
extern s32 D_800EECBC;
extern s32 D_800EFD4C;
extern u8 *D_800EECC4;
extern s32 D_800EFD20;
extern s32 D_800EECD0;
extern s32 D_800EFD50;
extern void *D_800D56B0;
extern u8 D_800EECE0[];
extern s32 D_800EFD54;
extern s32 D_800EECCC;
extern u8 *D_800EFD24;
extern u8 *D_800EFD20_p;

u8 *func_800BE9B0(u8 *param_1);
void *func_800BF79C(s32 a0, s32 a1);
u8 *func_800BC908(s32 a0, s32 a1, u8 *a2, s32 a3);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8004F70C(s32 *p);
void func_8004F670(void);

void func_8004B710(s32 param_1)
{
    u8 *p;
    s32 i;

    D_800CA858 = 0;
    p = func_800BE9B0(D_800D7330);
    if (p != NULL) {
        D_800EECD4 = *(s32 *)(p + 0x18);
        D_800EECBC = D_800EECD4 + *(s32 *)(p + 0x14);
        D_800EFD4C = (s32)func_800BF79C(D_800D5604, D_800D1244);
        if (D_800EFD4C != 0) {
            D_800EECC4 = func_800BC908(0x2000, 0, D_800D7338, 0xFC);
            D_800EFD20 = 0;
            D_800EECD0 = 0;
            D_800EFD50 = 0;
            D_800D56B0 = func_8004F670;
            func_8000ED10(D_800EECE0, 0, 0x1000);
            D_800EFD54 = 0;
            D_800EECCC = param_1;
            D_800EFD24 = func_800BC908(0x130, 0, D_800D7338, 0x10C);
            func_8000ED10(D_800EFD24, 0, 0x130);
            for (i = 0; i < D_800EECCC; i++) {
                func_8004F70C((s32 *)(D_800EFD24 + i * 0x4C));
            }
            D_800CA860 = 0;
            D_800CA85C = 0;
            D_800CA844 = 1;
        }
    }
}
