#include "ctx.h"

typedef struct Ent {
    u8 pad0[0x668];
    s16 f668;
    u8 pad66A[0x7E0 - 0x66A];
} Ent;

typedef struct Blk {
    u8 pad0[0x58];
    s32 f58;
} Blk;

extern s32 D_800C8CF0;
extern s32 D_800C8CE8;
extern s32 D_800C8CEC;
extern s32 D_800C8CE4;
extern s32 D_800C8CD8;
extern s32 D_800C8CDC;
extern s32 D_800C8CD4;
extern s32 D_800C2364;
extern s32 D_800EFDBC;
extern s32 D_800F05E8;
extern s32 D_800DC148;
extern s32 D_800E3700;
extern s32 D_800E37A8;
extern Blk D_800F05A8;
extern s32 D_800E3770[];
extern Ent D_800E37C0[];
extern u8 D_800E36E0[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80028590(void);
void func_8005E9BC(void);
s32 func_8005FFB0(s32 *dst);
void func_800229B8(void);
void func_8005FE10(s32 param_1);
void func_8004F9B0(s32 a0);
void func_8005EC14(s32 *p);
s32 func_8005F188(void);
void func_80015CD8(void);
void func_800154FC(s32 a0);
void func_8005D444(void);
void func_8001A190(void);

void func_80015254(void)
{
    if (D_800C8CF0 != 0) {
        if (D_800C8CE8 == 0) {
            if (D_800EFDBC == 9 || D_800F05E8 == 0) {
                func_8005EC14(D_800E3770);
            }
        } else if (D_800EFDBC == 9 || D_800F05E8 == 0) {
            func_8005FFB0(D_800E3770);
        }
        if (D_800C8CEC != 0 && D_800DC148 < 5) {
            if (D_800C8CE8 == 0) {
                func_8005F188();
            }
            func_80015CD8();
            func_8000ED10(D_800E36E0, 0, 0x18);
            D_800C8CE8 = 1;
            D_800C8CEC = 0;
            func_800154FC(0);
            func_8005FE10(D_800C8CE4);
            func_8005FFB0(D_800E3770);
        }
    } else if (D_800C8CD8 != 0) {
        if (D_800E37C0[D_800E3700 ^ 1].f668 == -1) {
            if (D_800C2364 != 0) {
                D_800E3770[D_800E3700 ^ 1] ^= 0xC;
            }
            func_8005EC14(&D_800E3770[D_800E3700 ^ 1]);
            if (D_800C2364 != 0) {
                D_800E3770[D_800E3700 ^ 1] ^= 0xC;
            }
        }
        if (D_800C8CD4 != 0) {
            func_8005FFB0(&D_800E3770[D_800E3700]);
        }
    } else if (D_800C8CDC != 0 && D_800E37A8 != 0) {
        if ((D_800F05A8.f58 & 0x40) && func_8005FFB0(D_800E3770) != 0) {
            func_800229B8();
            func_8005E9BC();
            func_8000ED10((u8 *)D_800E37C0, 0, 0x3F00);
            func_8004F9B0(0);
            func_8001A190();
            D_800F05A8.f58 = (D_800F05A8.f58 | 0x80) & ~0x2000;
            func_8005FE10(D_800C8CE4);
        }
    }
    func_8005D444();
    func_80028590();
}
