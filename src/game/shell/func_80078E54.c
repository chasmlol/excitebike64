#include "ctx.h"

typedef struct {
    s32 f0;
    u32 *tab;
} AssetMgr;

typedef struct {
    s32 w[7];
} Blk1C;

typedef struct {
    u8 pad0[0x34];
    Blk1C f34;
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Bike;

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[8];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    u32 f4C;
    u8 pad50[0x74 - 0x50];
    void *f74[0x30];
    u8 pad134[0x2E8 - 0x134];
    s32 f2E8;
    u8 pad2EC[0x3B8 - 0x2EC];
    u32 f3B8;
    Bike *f3BC;
    s32 f3C0;
    s32 f3C4;
    u8 pad3C8[0x43C - 0x3C8];
    u8 f43C[6];
    u8 pad442[0x44A - 0x442];
    s16 f44A;
    s16 *f44C;
    u8 pad450[0x778 - 0x450];
    s32 f778[6];
    s32 f790[6];
    u8 pad7A8[0x7AA - 0x7A8];
    u8 f7AA[6];
} GState;

typedef struct {
    s32 f0;
    s32 f4;
    u8 pad8[0x10 - 0x8];
    u16 f10;
    s16 f12;
    s16 f14;
    s16 f16;
    s16 f18;
    u8 pad1A[0x1C - 0x1A];
    void *f1C;
} Scr;

extern GState D_800EFD70;
extern AssetMgr *D_800F47C0;
extern AssetMgr *D_800E7728;
extern Bike *D_800EFD64;
extern Bike *D_800F0634;
extern Bike D_800CB210[];
extern Blk1C D_800F05AC;
extern s32 D_800C2388;
extern s32 D_800E3700;
extern void *D_800F01B4;
extern s32 D_800F04E4;
extern s32 *D_800CD148[];
extern s32 *D_800CD0B8[];
extern s32 D_800CA958[];
extern s32 D_800D09F8;
extern s32 D_800D1380;
extern s32 D_800D1388;
extern s32 D_800D1394;
extern s32 D_800D1398;
extern s32 D_800D139C;
extern s32 D_800D13A0;
extern s32 D_800D13A8;
extern s32 D_800D13B4;
extern s32 D_800D13BC;
extern s32 D_800D13C0;
extern s32 D_800D13C8;
extern s32 D_800D13CC;
extern s32 D_800D13E0;
extern s32 D_800D13EC;
extern s32 D_800D13F4;
extern s32 D_800D140C;
extern s32 D_800D1410;
extern s32 D_800D1414;
extern s32 D_800D1418;
extern s32 D_800D141C;
extern s32 D_800D1424;
extern s32 D_800D1428;
extern s32 D_800D142C;
extern s32 D_800D1430;
extern s32 D_800D143C;
extern s32 D_800D1444;
extern s32 D_800D1448;
extern s32 D_800D144C;
extern s32 D_800D1450;
extern s32 D_800D1470;
extern s32 D_800D1488;
extern s32 D_800D148C;
extern s32 D_800D1490;
extern s32 D_800D1494;
extern s32 D_800D1498;
extern s32 D_800D149C;
extern s32 D_800D14A0;
extern s32 D_800D14A4;
extern s32 D_800D14AC;
extern s32 D_800D14C8;

void func_800C204C(void *p);
void func_8006F5F8(s32 a, s32 b);
void func_800BFEA4(u32 *p);
void func_800B78F0(s32 param_1);
void func_80073664(s32 alpha, u16 *src);
void func_8008B6DC(void);
void func_800BCB04(void *ptr);
void func_8008BC30(void);
void func_80015CD8(void);

#define LOAD(id) func_800BFEA4(&D_800F47C0->tab[id])

void func_80078E54(Scr *s)
{
    s32 i;

    if (s == NULL) {
        return;
    }
    D_800EFD70.f0 &= ~0x200;
    LOAD(D_800D1398);
    LOAD(D_800D1410);
    if ((s->f10 & 0x20) && s->f4 == 0) {
        for (i = 0; i < 0x30; i++) {
            func_800C204C(D_800EFD70.f74[i]);
            func_800BCB04(D_800EFD70.f74[i]);
        }
    }
    if (s->f10 & 0x80) {
        LOAD(D_800D1450);
        LOAD(D_800D14C8);
    }
    if (s->f1C != NULL) {
        if (s->f16 == 4) {
            for (i = 0; i < 7; i++) {
                LOAD(*((s32 **)s->f1C)[i]);
            }
        } else {
            LOAD(*(s32 *)s->f1C);
        }
    }
    switch (s->f16) {
    case 23:
        D_800EFD70.f0 &= ~0x10000000;
        break;
    case 22:
        func_8008BC30();
        LOAD(D_800D13B4);
        break;
    case 0:
        i = 0;
        LOAD(D_800D1490);
        LOAD(D_800D14A4);
        func_80015CD8();
        D_800EFD70.f3C = 1;
        D_800F0634 = D_800EFD64;
        for (; i < 6; i++) {
            D_800EFD70.f1C[i] = D_800EFD70.f778[i];
            D_800EFD70.f43C[i] = D_800EFD70.f790[i];
        }
        break;
    case 2:
        func_80015CD8();
        if (!(D_800EFD70.f0 & 0x400)) {
            D_800C2388 = 0;
        }
        D_800EFD70.f0 &= ~0x3C10;
        switch (D_800EFD70.f3B8) {
        case 8:
            if (D_800E3700 == 0) {
                D_800EFD70.f1C[1] = D_800EFD70.f1C[0];
            }
        case 0:
        case 1:
            D_800EFD70.f0 |= 0x80;
            if (D_800EFD70.f3BC != NULL && (D_800EFD70.f3BC->f50 & 2)) {
                D_800EFD70.f0 &= ~0x80;
            }
            break;
        }
        i = 0;
        func_800BFEA4(&D_800E7728->tab[D_800D09F8]);
        LOAD(D_800D149C);
        LOAD(D_800D1388);
        LOAD(D_800D144C);
        LOAD(D_800D1494);
        LOAD(D_800D1414);
        LOAD(D_800D13A8);
        LOAD(D_800D1388);
        LOAD(D_800D13EC);
        D_800EFD70.f2E8 = 0;
        D_800EFD70.f4C = D_800EFD70.f3B8;
        D_800EFD64 = D_800EFD70.f3BC;
        for (; i < 6; i++) {
            if (D_800EFD70.f3C >= 2) {
                D_800EFD70.f43C[i] = i;
            } else {
                D_800EFD70.f43C[i] = (D_800EFD70.f3C0 + i) % 6;
            }
            D_800EFD70.f7AA[i] = D_800EFD70.f3C4;
            D_800EFD70.f7AA[i] += D_800EFD70.f1C[i];
            D_800EFD70.f7AA[i] %= 6;
        }
        break;
    case 19:
        func_80073664(0xFF, NULL);
        LOAD(D_800D1470);
        LOAD(D_800D1498);
        LOAD(D_800D139C);
        LOAD(D_800D13A0);
        func_800BCB04(D_800F01B4);
        break;
    case 6:
        func_8006F5F8(5, 0);
        func_8006F5F8(1, 0);
        func_800B78F0(D_800F04E4);
        LOAD(D_800D1470);
        LOAD(D_800D1424);
        LOAD(D_800D1444);
        LOAD(D_800D1428);
        break;
    case 24:
        LOAD(D_800D14A0);
        LOAD(D_800D13BC);
        break;
    case 25:
        LOAD(D_800D13E0);
        LOAD(D_800D142C);
        break;
    case 9:
    case 20:
        if (s->f16 == 9) {
            LOAD(D_800D13CC);
        } else {
            D_800EFD64->f34 = D_800F05AC;
            D_800EFD64->f50 &= ~0x180;
            func_8006F5F8(2, D_800EFD64 - D_800CB210);
            for (i = 0; i < D_800EFD70.f44A; i++) {
                if (D_800EFD70.f44C[i] == D_800EFD64 - D_800CB210) {
                    break;
                }
            }
            LOAD(*D_800CD148[D_800CA958[i]]);
        }
        func_800BCB04(D_800F01B4);
        LOAD(D_800D1488);
        LOAD(D_800D1380);
        LOAD(D_800D148C);
        LOAD(D_800D13A0);
        break;
    case 7:
    case 13:
    case 14:
        func_800BCB04(D_800F01B4);
        LOAD(D_800D13A0);
        LOAD(D_800D143C);
        LOAD(D_800D13C0);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        break;
    case 12:
        func_8008B6DC();
        D_800EFD70.f0 &= ~0x02000000;
        break;
    case 3:
        LOAD(D_800D141C);
        LOAD(D_800D1430);
    case 11:
        LOAD(D_800D149C);
        D_800EFD70.f2E8 = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D140C);
        LOAD(D_800D13F4);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (i = 0; i < s->f18; i++) {
            if (s->f16 == 11) {
                LOAD(*D_800CD148[D_800CA958[i + s->f14]]);
            } else {
                LOAD(*D_800CD0B8[i + s->f12]);
            }
        }
        break;
    case 4:
    case 18:
        i = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1448);
        LOAD(D_800D13F4);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (; i < 0x18; i++) {
            LOAD(*D_800CD148[i]);
        }
        func_80073664(0xFF, NULL);
        break;
    }
}
