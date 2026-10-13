#include "ctx.h"

typedef struct {
    u8 f0[3];
    u8 f3;
} Pad4;
typedef struct {
    u8 pad[0x68];
} Rec68;

extern s32 D_800E1E18;
extern u8 D_800E1C48[];
extern Pad4 D_800E1C68[];
extern Rec68 D_800E1C78[];
extern Rec68 D_800E1E60[];
extern s32 D_800E2010[];
extern s32 D_800E2020;
extern s32 D_800E2004;
extern s32 D_800E1C60;
extern s32 D_800E2008;
extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800C292E;
extern s32 D_800C2928;
extern u8 D_800F4878[];

void func_8000CD00(void);
void func_800AD9C0(u8 *p);
void func_800B0610(u8 *p, u32 *q, s32 n);
void func_800ADA40(void *p);
s32 func_800B4D20(u8 *p, void *q, s32 n);
s32 func_800B3704(u8 *p, void *q, s32 n);
s32 func_800B33C0(u8 *p, u8 *q, s32 n);
void func_800B35C0(void *p, s32 n);
s32 func_800AC810(u8 *p);

void func_8008BC30(void)
{
    u32 buf[2];
    s32 i;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(D_800E1C48);
    func_800B0610(D_800E1C48, buf, 1);
    func_800ADA40(D_800E1C68);
    D_800C292C = 0;
    D_800C292D = 0;
    for (i = 0; i < 4; i++) {
        if (D_800E1C68[i].f3 == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C |= 1 << i;
            D_800C292D++;
        }
    }
    D_800E2020 = 0;
    D_800E2004 = 0;
    D_800E1C60 = 0;
    D_800E2008 = 0;
    for (i = 0; i < 4; i++) {
        D_800E2010[i] = 0;
        if ((D_800C292C >> i) & 1) {
            switch (func_800B4D20(D_800E1C48, &D_800E1C78[i], i)) {
            case 0:
                D_800E2004 |= 1 << i;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                break;
            case 11:
                if (func_800B3704(D_800E1C48, &D_800E1E60[i], i) == 0) {
                    func_800B35C0(&D_800E1E60[i], 0);
                    D_800E1C60 |= 1 << i;
                } else if (func_800B33C0(D_800E1C48, D_800F4878, i) != 0) {
                    D_800E2008 |= 1 << i;
                }
                break;
            case 10:
                if (func_800B3704(D_800E1C48, &D_800E1E60[i], i) == 0) {
                    func_800B35C0(&D_800E1E60[i], 0);
                    D_800E1C60 |= 1 << i;
                } else {
                    D_800E2008 |= 1 << i;
                }
                break;
            }
        }
    }
    D_800C2928 = func_800AC810(D_800E1C48);
}
