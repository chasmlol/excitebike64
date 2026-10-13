#include "ctx.h"

typedef struct {
    u32 flags;      /* 0x00 */
    u8 pad04[0x38];
    s32 f3C;        /* 0x3C */
    s32 f40;        /* 0x40 */
    s32 f44;        /* 0x44 */
    s32 f48;
    s32 mode;       /* 0x4C */
    u8 pad50[0x70C];
    s32 f75C;       /* 0x75C */
} Game;

typedef struct {
    u8 pad[0x1C];
} Ent1C;

typedef struct {
    u8 pad00[0x2C];
    s32 f2C;        /* 0x2C */
    u8 pad30[0x20];
    u32 f50;        /* 0x50 */
    u8 pad54[0x20];
    s16 h74;        /* 0x74 */
    s16 h76;        /* 0x76 */
    Ent1C *p78;     /* 0x78 */
    Ent1C *p7C;     /* 0x7C */
    s32 *p80;       /* 0x80 */
} Obj;

typedef struct {
    s16 a;
    u8 b;
    u8 c;
} Small;

typedef struct {
    u8 pad[0xC];
    u8 fl;
    u8 pad0D[0x1B];
} Tab28;

typedef struct {
    u8 pad[0x64];
    s16 idx;
} Cur;

typedef struct {
    u8 pad[0x22];
    s16 h22;
} Lim;

typedef struct {
    s32 data[0x1E0];
} Blk;

typedef struct {
    u8 pad[0x65C];
    Blk *p;
    u8 pad660[0x180];
} Slot;

extern Game D_800EFD70;
extern Obj *D_800EFD64;
extern u8 D_800F1BC4;
extern s32 D_800C2940;
extern s32 D_800CCCA4;
extern volatile s32 D_800CCCA8;
extern s32 D_800C2280;
extern s32 D_800C2364;
extern s32 D_800C2358;
extern s32 D_800C8CE0;
extern s32 D_800E3710;
extern Small D_800E3730;
extern s32 D_800CCCCC;
extern s32 D_800C8CE8;
extern s32 D_800F05EC;
extern s32 D_800C9650;
extern u8 D_800D6160[];
extern s32 D_800E7728;
extern s32 D_800D151C;
extern s32 D_800E370C;
extern s32 D_800E3704;
extern Cur *D_800CA930;
extern Tab28 D_800CADE4[];
extern s32 D_800C238C[8];
extern Lim D_800F05A8;
extern f32 D_800E76C8;
extern u8 D_800E36E0[];
extern s32 D_800C9644;
extern s32 D_800C9640;
extern s32 D_800C9648;
extern s32 D_800E783C;
extern s32 D_800E781C;
extern s32 D_800E77C4;
extern s32 D_800E77F8;
extern s32 D_800E7824;
extern s32 D_800E771C;
extern s32 D_800D14F0;
extern u8 D_800C9080[];
extern Blk *D_800E1A78;
extern Slot D_800E37C0[8];
extern s32 D_800E36F8;
extern s32 D_800E374C;
extern s32 D_800E3700;

void func_80000F0C(void);
void func_800197B4(s32 a);
void func_800AD990(void);
void func_800BF530(u8 *a, s32 *b);
s32 func_8005AF4C(s32 *p);
u32 func_8005AED8(s32 key);
void func_800482D0(s32 a);
void func_80048230(s32 a);
void func_80014E10(void);
void func_80050838(s32 a, s32 b);
void func_80064DF4(s32 a, s32 b);
void func_80064D28(s32 a, s32 b);
void func_800358C0(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8006A8B0(s32 a);
void func_8000115C(void);
void func_80069CA0(s32 a, s32 b);
void func_800718F0(void);
void func_8004F47C(void);
void func_800366AC(s32 a);
s32 func_8005B58C(s32 key);
void func_800195B8(s32 a, s32 b);
void func_800620A0(void);
void func_8006AC40(s32 a);
void func_8004A5FC(s32 a);
void func_80028600(s32 a);
void func_8003C21C(void);
void func_80067B80(void);
void func_8003A694(u8 *a);
void func_8003E1FC(s32 a);
void func_8001A190(void);
void func_8000F240(u8 *a);
void func_80019E78(void);
void func_8005D270(void);
void func_8003A780(void);
void func_80071990(void);
void func_8005B05C(void);
void func_8000BEA0(void);
void func_8001A044(void);
void func_800166CC(void);

void func_800154FC(s32 arg)
{
    u16 *p;
    Ent1C *q;
    Obj *o;
    s32 i;
    s32 v;

    if (!(D_800EFD70.flags & 0x81) && (D_800F1BC4 & 2) && D_800C2940 != 0
        && (D_800EFD70.f3C == 1 || D_800EFD70.mode == 8) && D_800EFD64->f2C != 2
        && !(D_800EFD70.flags & 0x04040000)) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();

    if ((D_800EFD70.mode == 0 || D_800EFD70.mode == 1 || D_800EFD70.mode == 8) && (D_800C2280 & 2)
        && !(D_800EFD70.flags & 0x40001)) {
        D_800C2364 = 1;
    } else {
        D_800C2364 = 0;
    }
    D_800C2358 = 0;
    D_800C8CE0 = 0;
    D_800E3710 = 0;
    D_800E3730.a = -1;
    D_800E3730.b = 0;
    D_800E3730.c = 0;
    D_800CCCCC = 1;
    D_800EFD70.flags |= 0x10000;
    func_800197B4(0);
    if (D_800C8CE8 != 0) {
        D_800C9650 = D_800F05EC;
    } else {
        D_800C9650 = -1;
    }
    func_800AD990();
    func_800BF530(D_800D6160, &D_800E7728);
    if (D_800EFD64 != NULL) {
        func_8005AF4C(D_800EFD64->p80);
    }
    p = (u16 *)func_8005AED8(D_800D151C);
    if (p != NULL && !(D_800EFD64->f50 & 2)) {
        o = D_800EFD64;
        o->h76 = p[1];
        o->h74 = p[0];
        q = (Ent1C *)(p + 2);
        o->p78 = q;
        o->p7C = q + o->h74;
    } else {
        D_800EFD64->h76 = 0;
        D_800EFD64->h74 = 0;
        D_800EFD64->p78 = NULL;
        D_800EFD64->p7C = NULL;
    }
    if (D_800EFD70.flags & 0x40080) {
        func_800482D0(D_800E7728);
    } else if (!(D_800EFD70.flags & 1)) {
        func_80048230(D_800E7728);
    }

    if (D_800EFD70.mode == 4 || D_800EFD70.mode == 5) {
        D_800E370C = 6;
    } else if (D_800EFD70.mode == 9) {
        if (D_800CADE4[D_800CA930->idx].fl & 4) {
            D_800E370C = 2;
        } else {
            D_800E370C = 1;
        }
    } else if ((D_800EFD70.flags & 0x81) == 0x80) {
        D_800E370C = 1;
    } else if (D_800EFD70.mode == 0 && D_800EFD70.f3C == 2) {
        D_800E370C = D_800EFD70.f75C + 2;
    } else if (D_800EFD70.mode == 13 || D_800EFD70.mode == 14) {
        D_800E370C = 1;
    } else if (D_800EFD70.flags & 1) {
        D_800E370C = 6;
        D_800E3704 = 0;
    } else if (D_800EFD70.mode == 6) {
        D_800E370C = 4;
        D_800EFD70.f3C = 1;
    } else if (D_800EFD70.mode == 2 || D_800EFD70.mode == 10) {
        D_800E370C = D_800EFD70.f3C;
    } else {
        D_800E370C = D_800EFD70.f3C + D_800EFD70.f44;
    }

    if (D_800EFD70.mode == 8 && !(D_800EFD70.flags & 1)) {
        D_800E370C = 1;
    }
    v = 0xFF;
    for (i = 7; i >= 0; i--) {
        D_800C238C[i] = v;
    }
    if (D_800EFD70.flags & 0x10) {
        D_800E370C = 6;
    }
    func_80014E10();
    if (D_800EFD70.flags & 0x40000) {
        D_800E370C = D_800EFD70.f3C;
        D_800EFD70.f3C = 1;
    }
    func_80050838(D_800E370C, D_800EFD70.f40);
    if (!(D_800EFD70.flags & 0x8081)) {
        func_80064DF4(D_800EFD70.f3C, D_800F05A8.h22 - D_800EFD70.f3C);
        func_80064D28(D_800EFD70.f3C, D_800F05A8.h22 - D_800EFD70.f3C);
    }
    D_800E76C8 = 0.016666668f;
    func_800358C0();
    func_8000ED10(D_800E36E0, 0, 0x18);
    func_8006A8B0((D_800EFD70.flags >> 18) & 1);
    D_800C9644 = 0;
    D_800C9640 = 0;
    D_800C9648 = 0;
    D_800E783C = 0;
    D_800E781C = 0;
    D_800E77C4 = 0;
    D_800E77F8 = 0;
    D_800E7824 = 0;
    func_8000115C();
    func_80069CA0(D_800E783C, 1);
    func_80069CA0(D_800E781C, 2);
    func_80069CA0(D_800E77F8, 0);
    func_80069CA0(D_800E7824, 0);
    func_80069CA0(D_800E77C4, 0);
    if (D_800C2358 != 0 && D_800EFD64->f2C == 0) {
        func_80069CA0(D_800E771C, 0);
    }
    func_800718F0();
    func_8004F47C();
    func_800366AC(D_800E7728);
    func_800195B8(D_800E7728, func_8005B58C(D_800D14F0));
    func_800620A0();
    func_8006AC40(D_800E7728);
    func_8004A5FC(D_800E7728);
    func_80028600(D_800E7728);
    func_8003C21C();
    func_80067B80();
    func_8003A694(D_800C9080);
    func_8003E1FC(D_800E7728);
    func_8001A190();
    for (i = 0; i < 8; i++) {
        *D_800E37C0[i].p = *D_800E1A78;
    }
    func_8000F240(D_800C9080);
    func_80019E78();
    func_8005D270();
    func_8003A780();
    func_80071990();
    if (D_800EFD70.mode == 9) {
        func_8005B05C();
    }
    if (D_800C2358 != 0) {
        func_8000BEA0();
        func_8001A044();
    }
    if (D_800EFD70.flags & 0x80) {
        D_800E36F8 = 0;
        D_800E374C = -1;
        func_800166CC();
    } else if (D_800EFD70.flags & 1) {
        D_800E36F8 = 5;
    } else {
        D_800E36F8 = 4;
        D_800E374C = -1;
    }
    D_800C8CE0 = 0;
    if (D_800EFD70.mode == 8 && !(D_800EFD70.flags & 0x80) && D_800E3700 == 0) {
        D_800C8CE0 = 1;
    }
}
