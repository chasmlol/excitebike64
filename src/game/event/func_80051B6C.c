#include "ctx.h"

typedef struct {
    s32 state;
    u8 p04[0x26 - 0x04];
    s16 t26;
    s16 t28;
    u8 p2A[2];
    s32 t2C;
    u8 p30[0x40 - 0x30];
    s32 f40;
    u8 p44[0x58 - 0x44];
    s32 flags;
    u8 p5C[0x68 - 0x5C];
    s16 c68;
    s16 c6A;
    s16 t6C;
    s8 f6E;
} Game;

typedef struct {
    u8 p00[0x3C];
    s32 nplayers;
} Ctl;

typedef struct {
    u8 b[0x7E0];
} Ent;

extern Game D_800F05A8;
extern Ctl D_800EFD70;
extern Ent D_800E37C0[];
extern f32 D_800F1510[3];
extern u8 D_800E3F68;
extern u8 D_800E4748;
extern s32 D_800EFDAC;
extern s32 D_800E3770[4];
extern s32 D_800E36E0[4];
extern s16 D_800F05D0;
extern s32 D_800E36F8;
extern s32 D_800E374C;

void func_80046CC0(Ent *e);
void func_8004EFA4(void);
void func_8006C28C(s32 a0);

#define PRESSED(i, b) ((D_800E3770[i] & (b)) && !(D_800E36E0[i] & (b)))

void func_80051B6C(void)
{
    s32 i;
    s32 j;

    if (D_800F05A8.f40 == 0) {
        D_800F05A8.t28 = 180;
    } else if (D_800F05A8.t28 != 0) {
        D_800F05A8.t28--;
    }
    if (D_800F05A8.t6C != 0) {
        if (--D_800F05A8.t6C == 0) {
            for (i = 0; i < D_800EFD70.nplayers; i++) {
                func_80046CC0(&D_800E37C0[i]);
            }
            func_8006C28C(0);
            D_800F05A8.flags &= ~0x1F;
        }
        return;
    }
    if (D_800F05A8.state == 7 || D_800F05A8.state == 9 || D_800F05A8.state == 11) {
        if (!(D_800F05A8.flags & 0xF)) {
            if (D_800F1510[0] >= -24.0f && D_800F1510[0] < 23.0f && D_800F1510[2] >= 79.0f && D_800F1510[2] < 87.0f &&
                D_800F1510[1] >= 0.0f && D_800F1510[1] < 6.2f) {
                D_800F05A8.t6C = 180;
                D_800F05A8.c68++;
                func_8004EFA4();
                D_800E3F68 = 1;
                D_800F05A8.flags |= 0x11;
                if ((D_800F05A8.f6E >> ((D_800EFDAC - 1) >> 1)) == 1) {
                    D_800F05A8.flags |= 0x20;
                } else {
                    D_800F05A8.flags &= ~0x20;
                }
            } else if (D_800F1510[0] >= -24.0f && D_800F1510[0] < 23.0f && D_800F1510[2] >= -80.0f && D_800F1510[2] < -73.0f &&
                       D_800F1510[1] >= 0.0f && D_800F1510[1] < 6.2f) {
                D_800F05A8.t6C = 180;
                D_800F05A8.c6A++;
                func_8004EFA4();
                D_800E4748 = 1;
                D_800F05A8.flags |= 0x12;
                if ((D_800F05A8.f6E >> ((D_800EFDAC - 1) >> 1)) == 0) {
                    D_800F05A8.flags |= 0x20;
                } else {
                    D_800F05A8.flags &= ~0x20;
                }
            }
        }
    }
    switch (D_800F05A8.state) {
    case 6:
        if (--D_800F05A8.t26 == 0) {
            D_800F05A8.state = 7;
            D_800F05A8.t2C = 0x2A30;
            D_800F05A8.flags &= ~0x10;
        }
        break;
    case 7:
        if (--D_800F05A8.t2C == 0) {
            j = 0;
            D_800F05A8.state = 8;
            D_800F05A8.t26 = 240;
            D_800F05A8.flags |= 0x10;
            for (; j < D_800EFD70.nplayers; j++) {
                func_80046CC0(&D_800E37C0[j]);
            }
            func_8006C28C(0);
        }
        break;
    case 8:
        if (--D_800F05A8.t26 == 0) {
            D_800F05A8.state = 9;
            D_800F05A8.t2C = 0x2A30;
            D_800F05A8.flags &= ~0x10;
        }
        break;
    case 9:
        if (--D_800F05A8.t2C == 0) {
            D_800F05A8.flags |= 0x10;
            if (D_800F05A8.c68 == D_800F05A8.c6A) {
                D_800F05A8.state = 10;
                D_800F05A8.t26 = 240;
            } else {
                D_800F05A8.state = 12;
                D_800F05A8.t26 = 600;
                D_800F05A8.f40 = 1;
            }
            for (i = 0; i < D_800EFD70.nplayers; i++) {
                func_80046CC0(&D_800E37C0[i]);
            }
            func_8006C28C(0);
        }
        break;
    case 10:
        if (--D_800F05A8.t26 == 0) {
            D_800F05A8.state = 11;
            D_800F05A8.t2C = 0x1C20;
            D_800F05A8.flags &= ~0x10;
        }
        break;
    case 11:
        if (--D_800F05A8.t2C == 0) {
            j = 0;
            D_800F05A8.state = 12;
            D_800F05A8.f40 = 1;
            D_800F05A8.t26 = 600;
            D_800F05A8.flags |= 0x10;
            for (; j < D_800EFD70.nplayers; j++) {
                func_80046CC0(&D_800E37C0[j]);
            }
            func_8006C28C(0);
        }
        break;
    case 12:
        if (PRESSED(0, 0x40) || PRESSED(1, 0x40) || PRESSED(2, 0x40) || PRESSED(3, 0x40) ||
            (D_800F05D0 == 0 &&
             (PRESSED(0, 0x10) || PRESSED(1, 0x10) || PRESSED(2, 0x10) || PRESSED(3, 0x10)))) {
            if (D_800E36F8 == 5) {
                D_800E36F8 = 6;
                D_800E374C = 20;
            }
        }
        break;
    }
}
