#include "ctx.h"

void func_800BBBAC(s32 x);
void func_800BA4A8(s32 x);
void func_8004F7E8(void);
void func_8004F8A4(void);
void func_800B98B4(void);
void func_800BBDE0(void);
void func_800BED20(void);

extern s32 D_800C2B5C;
extern s32 D_800C2B60;
extern s32 D_800C2B64;
extern s32 D_800D33F8;
extern s32 D_800E32A0;
extern s32 D_800E3710;
extern s32 D_800E1898;
extern s32 D_800E7FF0;
extern s32 D_8010062C;

void func_80010AFC(void)
{
    s32 iVar1;
    s32 a;

    if (D_800C2B64 == 0) {
        while (D_800D33F8 != 0) {
            if (D_800C2B5C != 0 && D_800C2B60 == 0) {
                a = 0; a = 2; a = 3;
                func_8004F8A4();
                func_800BBBAC(D_800E32A0);
                D_800C2B60 = 1;
                func_800BBDE0();
                func_800B98B4();
                func_800BED20();
                func_8004F7E8();
                D_800C2B60 = 0;
                if (D_800C2B64 == 0) goto z1;
                a = 0;
                if (D_800E3710 != 0) goto c1;
                if (D_800E1898 != 1) goto c1;
                a = 1;
                if (D_800E7FF0 & 0x40) goto c1;
            z1:
                a = 0;
            c1:
                func_800BA4A8(a);
            }
        }
        iVar1 = D_8010062C;
        {
        while (D_8010062C < iVar1 + 2) {
            if (D_800C2B5C != 0 && D_800C2B60 == 0) {
                a = 0; a = 2; a = 3;
                func_8004F8A4();
                func_800BBBAC(D_800E32A0);
                D_800C2B60 = 1;
                func_800BBDE0();
                func_800B98B4();
                func_800BED20();
                func_8004F7E8();
                D_800C2B60 = 0;
                if (D_800C2B64 == 0) goto z2;
                a = 0;
                if (D_800E3710 != 0) goto c2;
                if (D_800E1898 != 1) goto c2;
                a = 1;
                if (D_800E7FF0 & 0x40) goto c2;
            z2:
                a = 0;
            c2:
                func_800BA4A8(a);
            }
        }
        }
    }
}
