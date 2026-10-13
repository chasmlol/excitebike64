#include "ctx.h"

extern s32 D_800C2B5C;
extern s32 D_800C2B60;
extern s32 D_800C2B64;
extern s32 D_800E32A0;
extern s32 D_800E3710;
extern s32 D_800E1898;
extern s32 D_800E7FF0;

void func_800BBBAC(s32 x);
void func_800BA4A8();
void func_8004F7E8(void);
void func_8004F8A4(void);
void func_800B98B4(void);
void func_800BBDE0(void);
void func_800BED20(void);

void func_800110DC(void)
{
    s32 flag;

    if (D_800C2B5C != 0 && D_800C2B60 == 0) {
        func_8004F8A4();
        func_800BBBAC(D_800E32A0);
        D_800C2B60 = 1;
        func_800BBDE0();
        func_800B98B4();
        func_800BED20();
        func_8004F7E8();
        D_800C2B60 = 0;
        if (D_800C2B64 == 0 || D_800E3710 != 0 || D_800E1898 != 1 || !(D_800E7FF0 & 0x40)) {
            flag = 0;
        } else {
            flag = 1;
        }
        func_800BA4A8(flag);
    }
}
