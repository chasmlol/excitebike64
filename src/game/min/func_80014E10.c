#include "ctx.h"

extern s32 func_8005FE10(s32 x);
extern void func_80019EC0(s32 x);
extern void func_80019CCC(s32 x);
extern s32 *func_800BF79C(s32 a, s32 b);
extern void func_8005EA20(s32 a, s32 b);

typedef struct {
    s32 f[7];
    s32 *p;
    s32 g[2];
} Ent40;

extern s32 D_800E37A8;
extern s32 D_800C8CDC;
extern s32 D_800C8CE8;
extern s32 D_800C8CEC;
extern s32 D_800C8CD8;
extern s32 D_800C8CF0;
extern s32 D_800C9650;
extern s32 D_800E3748;
extern s32 D_800E3700;
extern s32 D_800E7728;
extern s32 *D_800C8CD4;
extern s32 *D_800C8CE4;
extern u8 *D_800CA930;
extern s32 D_800C238C[];
extern s32 D_800EFD70[];
extern s32 D_800F05A8[];
extern Ent40 D_800CADE4[];

void func_80014E10(void)
{
    s32 *piVar1;

    D_800E37A8 = 0;
    D_800C8CDC = 0;
    D_800C8CE8 = 0;
    if (D_800EFD70[0x13] == 8 && (D_800EFD70[0] & 0x80000) != 0) {
        if ((D_800EFD70[0] & 0x80) != 0) {
            func_80019EC0((s32)(D_800C8CE4 + 0x1900));
        }
    }
    if ((D_800EFD70[0] & 0x81) == 0 && D_800EFD70[0x13] == 8) {
        D_800C8CD8 = 1;
        if ((D_800EFD70[0] & 0x80000) != 0) {
            D_800C8CD4 = D_800C8CE4 + 0x1900;
        } else {
            D_800C8CD4 = 0;
        }
    } else if ((D_800EFD70[0] & 0x8000) != 0) {
        D_800C8CE8 = 1;
        D_800C8CEC = 0;
        func_8005FE10((s32)D_800C8CE4);
    } else {
        if ((D_800EFD70[0] & 0x81) == 0 && D_800EFD70[0x13] == 9 &&
            D_800CADE4[*(s16 *)(D_800CA930 + 0x64)].p != 0 && (D_800F05A8[22] & 0x4000) == 0) {
            D_800F05A8[22] = D_800F05A8[22] & 0xffffbfff;
            D_800C8CD8 = 0;
            D_800C8CDC = 1;
            D_800E37A8 = 1;
            if ((D_800EFD70[0] & 0x4000) != 0) {
                D_800C8CE4 = 0;
                D_800C8CDC = 0;
                D_800E37A8 = 0;
            } else {
                piVar1 = func_800BF79C(D_800E7728, *D_800CADE4[*(s16 *)(D_800CA930 + 0x64)].p);
                *(volatile s32 **)&D_800C8CE4 = piVar1;
                D_800E3748 = piVar1[1] - piVar1[0];
                D_800C8CE4 = piVar1 + 2;
                func_8005FE10((s32)D_800C8CE4);
            }
        } else {
            D_800C8CD8 = 0;
        }
    }
    if (D_800C8CD8 != 0) {
        D_800C8CF0 = 0;
        func_80019CCC((s32)D_800C8CD4);
        D_800C9650 = D_800E3700 ^ 1;
        if (D_800C8CD8 != 0) {
            D_800C238C[D_800E3700] = 0x80;
        }
    }
    if (D_800EFD70[0x13] == 9 || D_800C8CD8 != 0 || D_800C8CDC != 0 ||
        (D_800EFD70[0] & 0x4001) != 0 || D_800EFD70[0x13] == 0xd ||
        D_800EFD70[0x13] == 0xe || (D_800EFD70[0] & 0x80) != 0) {
        D_800C8CF0 = 0;
    } else {
        D_800C8CF0 = 1;
    }
    if (D_800C8CF0 != 0 && D_800C8CE8 == 0 && D_800C8CE4 != 0) {
        func_8005EA20((s32)D_800C8CE4, D_800E3748);
    }
}
