#include "ctx.h"

typedef struct { s32 w[7]; } V7;

typedef struct {
    s32 state;      /* 0x00 */
    V7 v;           /* 0x04 */
    s16 f20;        /* 0x20 */
    s16 f22;        /* 0x22 */
    s16 f24;        /* 0x24 */
    s16 f26;        /* 0x26 */
    s16 f28;        /* 0x28 */
    s16 pad2A;
    s32 f2C;        /* 0x2C */
    s32 f30[3];     /* 0x30 */
    s32 f3C;        /* 0x3C */
    s32 f40;        /* 0x40 */
    s32 f44;        /* 0x44 */
    s32 f48;        /* 0x48 */
    s32 f4C;        /* 0x4C */
    s32 f50;
    s32 f54;
    s32 f58;        /* 0x58 */
    s32 pad5C[3];
    s16 f68;
    s16 f6A;
    s16 f6C;
    u8 f6E;
    u8 pad6F;
    s32 pad70[6];
    s32 f88;
} Game;

typedef struct {
    s32 flags;      /* 0x00 */
    s32 pad4[14];
    s32 f3C;        /* 0x3C */
    s32 pad40[3];
    s32 mode;       /* 0x4C */
} Cfg;

typedef struct {
    u8 pad[0x34];
    V7 v;           /* 0x34 */
    s32 f50;        /* 0x50 */
    u8 pad54[0xA0 - 0x54];
} Track;

typedef struct { u8 pad[0x24]; } Ent24;

typedef struct {
    Ent24 *p;
    u8 pad4[0x62];
    u8 b66[1];
} Race;

typedef struct {
    u8 pad[0x660];
    u8 pad660[8];
    s16 f668;
    u8 pad66A[0x69C - 0x66A];
    s32 f69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Bike;

typedef struct { s32 n; s32 f4; s32 *p; } Alloc;

extern Game D_800F05A8;
extern s32 D_800F0520;
extern Cfg D_800EFD70;
extern s32 D_800EFDBC;
extern Track *D_800EFD64;
extern Race *D_800CA930;
extern Ent24 D_800CA9C8[];
extern Track D_800CB210[];
extern s32 D_800CCD64;
extern s32 D_800CA920;
extern Alloc D_800CA924;
extern s32 D_800E370C;
extern Bike D_800E37C0[];
typedef struct { s32 a, b; s16 f8; u8 pad[0x7E0-0xA]; } BikeX;
extern BikeX D_800E3E20[];
extern u8 D_800D7340[];

void func_8000ED10(void *a0, s32 a1, s32 a2);
void func_80064DB8(void);
void func_80064D28(s32 a, s32 b);
s32 *func_800BD730(s32 a0, s32 a1, s32 a2, s32 a3);
void func_8004F9B0();

void func_80050838(s32 a0, s32 a1)
{
    Bike *b;
    s32 i;
    s32 j;

    D_800F0520 = -1;
    func_8000ED10(D_800F05A8.f30, -1, 12);
    if (D_800EFD70.mode == 1) {
        for (i = 0; i < D_800F05A8.f22; i++) {
            D_800CA930->b66[i] = 0;
        }
    }
    D_800CA920 = 6;
    *(V7 *)((u8 *)&D_800F05A8 + 4) = D_800EFD64->v;
    D_800EFD64->f50 &= ~0x180;
    D_800F05A8.f88 = -1;
    D_800F05A8.f58 = 0;
    D_800F05A8.f40 = 0;
    D_800F05A8.f28 = 0;
    D_800F05A8.f44 = 0;
    D_800F05A8.f4C = 0;
    D_800F05A8.f22 = a0;
    D_800F05A8.f20 = a1;
    if (D_800EFD70.mode == 1 && !(D_800EFD70.flags & 0x81) &&
        D_800CA930->p - D_800CA9C8 == 4) {
        D_800F05A8.f58 = 0x200;
        D_800F05A8.f22 = 2;
        D_800E370C = 2;
    }
    D_800F05A8.f26 = 0x96;
    if (D_800CCD64 != 0 || (D_800EFD70.flags & 0x81) ||
        D_800EFD70.mode == 0xD || D_800EFD70.mode == 0xE) {
        D_800F05A8.state = 5;
    } else {
        if ((D_800EFD70.mode == 0 || D_800EFD70.mode == 1) &&
            D_800EFD64 - D_800CB210 != 9) {
            D_800F05A8.state = 0;
            D_800F05A8.f26 = 0x100;
        } else {
            D_800F05A8.state = 1;
            D_800F05A8.f26 = 0x96;
        }
        D_800F05A8.f58 |= 0x10;
    }
    D_800F05A8.f2C = 0;
    D_800F05A8.f3C = (D_800EFD70.flags & 0x81) != 0;
    func_8000ED10(D_800E37C0, 0, 0x3F00);
    D_800EFD70.flags &= ~4;
    if (D_800CA924.n > 0 && D_800CA924.f4 == 0) {
        D_800CA924.p = func_800BD730(D_800CA924.n * 4, 0, (s32)D_800D7340, 0x6D9);
    }
    switch (D_800EFD70.mode) {
    case 2:
        D_800F05A8.f26 = 0xB4;
        D_800F05A8.state = 6;
        D_800F05A8.f6E = 0;
        D_800F05A8.f68 = 0;
        D_800F05A8.f6A = 0;
        D_800F05A8.f6C = 0;
        D_800F05A8.f58 |= 0x10;
        break;
    case 5:
        D_800F05A8.f58 |= 0x10;
        func_80064DB8();
        break;
    case 7:
        D_800F05A8.f48 = -1;
        D_800F05A8.f58 |= 0x10;
        func_80064DB8();
        for (b = D_800E37C0; b < &D_800E37C0[D_800EFD70.f3C]; b++) {
            b->f69C = 0x1C20;
        }
        break;
    case 9:
        func_8004F9B0(1);
        break;
    case 10:
        D_800F05A8.f2C = 0x1C20;
        D_800F05A8.f58 |= 0x10;
        func_80064DB8();
        break;
    case 0xD:
    case 0xE:
        break;
    default:
        if (D_800F05A8.state == 8) {
            func_80064D28(2, 0);
        }
        D_800F05A8.f24 = 0;
        for (j = 0; j < D_800F05A8.f22; j++) {
            D_800E3E20[j].f8 = -1;
        }
        break;
    }
}
