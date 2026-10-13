#include "ctx.h"

typedef struct { u8 pad0[0x14]; s32 f14; s32 f18; u8 pad1c[0x234 - 0x1C]; s32 a[2]; s32 b[2]; u8 rest[0xA80 - 0x244]; } Plr;
typedef struct { s32 a[2]; s32 b[2]; } Rec16;
typedef struct { s32 x; s32 y; } Pair;
typedef struct { s32 flags; u8 pad[0x48]; s32 mode; } State;
typedef struct { u8 pad[0x2C]; s32 m; u8 pad30[0x20]; u32 f50; } Cfg;

extern s32 D_800CCD64;
extern s32 D_800C27AC;
extern s32 D_800C27B0;
extern s32 D_800E1558;
extern s32 D_800DC0A8;
extern Cfg *D_800EFD64;
extern s32 D_800E1878;
extern s32 D_800E1A78;
extern s32 D_800DC0A4;
extern State D_800EFD70;
extern s32 D_800E1600[][6];
extern Plr D_800DC150[];
extern s32 D_800DC370;
extern Rec16 D_800E1568[];
extern s32 D_800E16C0[];
extern Pair D_800DC110[];
extern s32 D_800E1B48;

void func_800C204C(s32 a0);
void func_800BCB04(s32 a0);
void func_80024708(void);
void func_80024DA4(void);
void func_80042584(void);
void func_8006EF68(void);
void func_8006EF8C(void);
void func_8005B33C(void);
void func_80014AD0(void);
void func_800715DC(s32 a0);
void func_8002D8A0(void);
void func_80033D40(void);

void func_800020CC(void)
{
    s32 i, k, n;

    if (D_800CCD64 != 0) {
        func_800C204C(D_800C27AC);
        func_800C204C(D_800C27B0);
        func_800BCB04(D_800C27AC);
        func_800BCB04(D_800C27B0);
    }
    func_800BCB04(D_800E1558);
    func_800BCB04(D_800DC0A8);
    if (D_800EFD64->m == 0) {
        func_80024708();
    } else if (D_800EFD64->m == 1) {
        func_80024DA4();
    } else if (D_800EFD64->m == 2) {
        func_80042584();
    }
    func_800BCB04(D_800E1878);
    func_800BCB04(D_800E1A78);
    func_800BCB04(D_800DC0A4);
    if ((D_800EFD70.flags & 0x81) == 0) {
        func_8005B33C();
    }
    switch (D_800EFD70.mode) {
    case 2:
        func_8006EF68();
        break;
    case 0:
    case 13:
    case 14:
        if ((D_800EFD64->f50 & 2) == 0) {
            break;
        }
    case 9:
        func_8006EF8C();
        break;
    }
    func_80014AD0();
    for (n = 0; n < 8; n++) {
        if (D_800EFD70.flags & 0x10) {
            if (n >= 6) {
                break;
            }
            func_800BCB04(D_800DC150[n].f18);
            for (i = 0; i < 6; i++) {
                func_800BCB04(D_800E1600[n][i]);
            }
        } else if (D_800DC150[n].f14 != 0) {
            func_800BCB04(D_800DC150[n].f14);
        }
        func_800715DC(n);
        for (i = 0; i < 2; i++) {
            func_800BCB04(D_800DC150[n].a[i]);
            func_800BCB04(D_800DC150[n].b[i]);
        }
    }
    func_800BCB04(D_800DC370);
    for (i = 0; i < 7; i++) {
        for (k = 0; k < 2; k++) {
            if (D_800E1568[i].a[k] != 0) {
                func_800BCB04(D_800E1568[i].a[k]);
            }
            if (D_800E1568[i].b[k] != 0) {
                func_800BCB04(D_800E1568[i].b[k]);
            }
        }
    }
    for (i = 0; i < 6; i++) {
        func_800BCB04(D_800DC110[i].y);
        func_800BCB04(D_800DC110[i].x);
        func_800BCB04(D_800E16C0[i]);
    }
    func_800BCB04(D_800E1B48);
    func_8002D8A0();
    func_80033D40();
}
