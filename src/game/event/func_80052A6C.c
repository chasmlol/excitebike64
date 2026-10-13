#include "ctx.h"

void func_80051B6C(void);
void func_800515A8(void);
void func_800521F4(void);
void func_80050F7C(s32);
s32 func_8005AFFC(void);
void func_800516DC(void);
void func_8004FCBC(void);
void func_8005183C(void);

typedef struct {
    s32 w00;
    u8 p04[0x3C - 0x04];
    s32 w3C;
    u8 p40[0x4C - 0x40];
    u32 w4C;
} Ctl2;

typedef struct {
    u8 p00[0x24];
    s16 w24;
    u8 p26[0x2C - 0x26];
    s32 w2C;
    u8 p30[0x40 - 0x30];
    s32 w40;
    u8 p44[0x4C - 0x44];
    s32 w4C;
    u8 p50;
    u8 p51[0x58 - 0x51];
    s32 w58;
    u8 p5C[0x88 - 0x5C];
    s32 w88;
} Ctl;

extern Ctl2 D_800EFD70;
extern Ctl D_800F05A8;
extern u8 D_800C292C;
extern s32 D_800E3770[4];
extern s32 D_800E36E0;

void func_80052A6C(void)
{
    s32 i;
    s32 *p;

    if ((D_800EFD70.w00 & 0x80) == 0) {
        s32 bits;
        i = 0;
        bits = D_800C292C;
        for (p = D_800E3770; i < 4; i++, p++) {
            if (((bits >> i) & 1) != 0 && *p != 0) break;
        }
        if (i == 4) {
            D_800F05A8.w4C = D_800F05A8.w4C + 1;
        } else {
            D_800F05A8.w4C = 0;
        }
    }
    switch (D_800EFD70.w4C) {
    case 2:
        func_80051B6C();
        break;
    case 5:
        func_800515A8();
        if (D_800F05A8.w40 == 0) {
            if ((D_800F05A8.w58 & 0x10) != 0) break;
            D_800F05A8.w2C = D_800F05A8.w2C + 1;
            if (D_800F05A8.w2C == 0x34bc0) {
                func_80050F7C(0);
                D_800F05A8.w40 = 1;
            }
        } else if ((D_800E3770[0] & 0x40) != 0) {
            if ((D_800E36E0 & 0x40) == 0) {
                func_8005AFFC();
            }
            break;
        }
        break;
    case 7:
        func_800516DC();
        break;
    case 9:
        func_8004FCBC();
        break;
    case 10:
        func_8005183C();
        break;
    default:
        func_800521F4();
    case 0xd:
    case 0xe:
        break;
    }
    if (D_800EFD70.w4C < 2 && D_800F05A8.w24 > 0 && D_800F05A8.p50 < D_800EFD70.w3C &&
        D_800F05A8.w88 != -2 && (D_800EFD70.w3C == 1 || D_800EFD70.w4C == 8)) {
        D_800F05A8.w88 = D_800F05A8.w88 + 1;
        if (D_800EFD70.w3C >= 3) {
            if (D_800F05A8.w88 >= 100) goto store;
        } else if (D_800F05A8.w88 >= 0x8c) {
            goto store;
        }
        return;
store:
        D_800F05A8.w88 = -2;
    }
    return;
}
