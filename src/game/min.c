#include "game.h"

typedef struct { u8 pad[0x64]; s16 idx; } PtrA;

extern PtrA *D_800CA930_3;

typedef struct {
    s32 f[7];
    s32 *p;
    s32 g[2];
} Ent40_2;

extern Ent40_2 D_800CADE4_3[];

void func_80014E10(void)
{
    s32 *piVar1;

    D_800E37A8 = 0;
    D_800C8CDC = 0;
    D_800C8CE8 = 0;
    if (D_800EFD70_4[0x13] == 8 && (D_800EFD70_4[0] & 0x80000) != 0) {
        if ((D_800EFD70_4[0] & 0x80) != 0) {
            func_80019EC0((s32)(D_800C8CE4_2 + 0x1900));
        }
    }
    if ((D_800EFD70_4[0] & 0x81) == 0 && D_800EFD70_4[0x13] == 8) {
        D_800C8CD8 = 1;
        if ((D_800EFD70_4[0] & 0x80000) != 0) {
            D_800C8CD4 = D_800C8CE4_2 + 0x1900;
        } else {
            D_800C8CD4 = 0;
        }
    } else if ((D_800EFD70_4[0] & 0x8000) != 0) {
        D_800C8CE8 = 1;
        D_800C8CEC = 0;
        func_8005FE10_2((s32)D_800C8CE4_2);
    } else {
        if ((D_800EFD70_4[0] & 0x81) == 0 && D_800EFD70_4[0x13] == 9 &&
            D_800CADE4_3[*(s16 *)(D_800CA930 + 0x64)].p != 0 && (D_800F05A8_3[22] & 0x4000) == 0) {
            D_800F05A8_3[22] = D_800F05A8_3[22] & 0xffffbfff;
            D_800C8CD8 = 0;
            D_800C8CDC = 1;
            D_800E37A8 = 1;
            if ((D_800EFD70_4[0] & 0x4000) != 0) {
                D_800C8CE4_2 = 0;
                D_800C8CDC = 0;
                D_800E37A8 = 0;
            } else {
                piVar1 = func_800BF79C(D_800E7728, *D_800CADE4_3[*(s16 *)(D_800CA930 + 0x64)].p);
                *(volatile s32 **)&D_800C8CE4_2 = piVar1;
                D_800E3748 = piVar1[1] - piVar1[0];
                D_800C8CE4_2 = piVar1 + 2;
                func_8005FE10_2((s32)D_800C8CE4_2);
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
    if (D_800EFD70_4[0x13] == 9 || D_800C8CD8 != 0 || D_800C8CDC != 0 ||
        (D_800EFD70_4[0] & 0x4001) != 0 || D_800EFD70_4[0x13] == 0xd ||
        D_800EFD70_4[0x13] == 0xe || (D_800EFD70_4[0] & 0x80) != 0) {
        D_800C8CF0 = 0;
    } else {
        D_800C8CF0 = 1;
    }
    if (D_800C8CF0 != 0 && D_800C8CE8 == 0 && D_800C8CE4_2 != 0) {
        func_8005EA20((s32)D_800C8CE4_2, D_800E3748);
    }
}

typedef struct { u8 pad[0x1C]; s32 *ptr; u8 pad2[8]; } Ent_10;

extern Ent_10 D_800CADE4_4[];

void func_80015124(s32 param_1)
{
    u32 *g = (u32 *)D_800EFD70_5;
    if ((g[0] & 0x85) != 0) {
        if ((g[0] & 0x80) == 0) {
            return;
        }
        if (D_800CA920 != 3) {
            return;
        }
    }
    if (D_800C8CD8 == 0) {
        if (D_800C8CDC != 0) {
            D_800C8CE4 = 0;
            func_800BFEA4((u32 *)((s32 *)D_800E7728_2[1] + D_800CADE4_4[D_800CA930_3->idx].ptr[0]));
            goto T;
        }
        if (D_800CA920 == 3 || g[19] == 9) goto T;
        if (g[19] < 10) goto F0;
        if (g[19] >= 15) goto CALL;
        if (g[19] < 13) goto CALL;
        goto T;
F0:
        if (g[19] == 2) goto T;
    }
CALL:
    func_8005F188();
T:
    if (D_800C8CEC == 0 && param_1 != 0) {
        func_800197B4(1);
    }
}

typedef struct Ent_11 {
    u8 pad0[0x668];
    s16 f668;
    u8 pad66A[0x7E0 - 0x66A];
} Ent_11;

typedef struct Blk_5 {
    u8 pad0[0x58];
    s32 f58;
} Blk_5;

extern Blk_5 D_800F05A8_7;
extern Ent_11 D_800E37C0_13[];

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
            func_8000ED10(D_800E36E0_2, 0, 0x18);
            D_800C8CE8 = 1;
            D_800C8CEC = 0;
            func_800154FC(0);
            func_8005FE10(D_800C8CE4);
            func_8005FFB0(D_800E3770);
        }
    } else if (D_800C8CD8 != 0) {
        if (D_800E37C0_13[D_800E3700 ^ 1].f668 == -1) {
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
        if ((D_800F05A8_7.f58 & 0x40) && func_8005FFB0(D_800E3770) != 0) {
            func_800229B8();
            func_8005E9BC();
            func_8000ED10((u8 *)D_800E37C0_13, 0, 0x3F00);
            func_8004F9B0(0);
            func_8001A190();
            D_800F05A8_7.f58 = (D_800F05A8_7.f58 | 0x80) & ~0x2000;
            func_8005FE10(D_800C8CE4);
        }
    }
    func_8005D444();
    func_80028590();
}

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
} Game_9;

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
} Obj_6;

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
    u8 pad[0x22];
    s16 h22;
} Lim;

typedef struct {
    s32 data[0x1E0];
} Blk_6;

typedef struct {
    u8 pad[0x65C];
    Blk_6 *p;
    u8 pad660[0x180];
} Slot_4;

extern Game_9 D_800EFD70_19;
extern Obj_6 *D_800EFD64_14;
extern Small D_800E3730_5;
extern Tab28 D_800CADE4_5[];
extern Lim D_800F05A8_8;
extern Blk_6 *D_800E1A78_3;
extern Slot_4 D_800E37C0_14[8];

void func_800154FC(s32 arg)
{
    u16 *p;
    Ent1C *q;
    Obj_6 *o;
    s32 i;
    s32 v;

    if (!(D_800EFD70_19.flags & 0x81) && (D_800F1BC4 & 2) && D_800C2940 != 0
        && (D_800EFD70_19.f3C == 1 || D_800EFD70_19.mode == 8) && D_800EFD64_14->f2C != 2
        && !(D_800EFD70_19.flags & 0x04040000)) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();

    if ((D_800EFD70_19.mode == 0 || D_800EFD70_19.mode == 1 || D_800EFD70_19.mode == 8) && (D_800C2280 & 2)
        && !(D_800EFD70_19.flags & 0x40001)) {
        D_800C2364 = 1;
    } else {
        D_800C2364 = 0;
    }
    D_800C2358 = 0;
    D_800C8CE0 = 0;
    D_800E3710 = 0;
    D_800E3730_5.a = -1;
    D_800E3730_5.b = 0;
    D_800E3730_5.c = 0;
    D_800CCCCC = 1;
    D_800EFD70_19.flags |= 0x10000;
    func_800197B4(0);
    if (D_800C8CE8 != 0) {
        D_800C9650 = D_800F05EC;
    } else {
        D_800C9650 = -1;
    }
    func_800AD990();
    func_800BF530(D_800D6160, &D_800E7728);
    if (D_800EFD64_14 != NULL) {
        func_8005AF4C(D_800EFD64_14->p80);
    }
    p = (u16 *)func_8005AED8(D_800D151C);
    if (p != NULL && !(D_800EFD64_14->f50 & 2)) {
        o = D_800EFD64_14;
        o->h76 = p[1];
        o->h74 = p[0];
        q = (Ent1C *)(p + 2);
        o->p78 = q;
        o->p7C = q + o->h74;
    } else {
        D_800EFD64_14->h76 = 0;
        D_800EFD64_14->h74 = 0;
        D_800EFD64_14->p78 = NULL;
        D_800EFD64_14->p7C = NULL;
    }
    if (D_800EFD70_19.flags & 0x40080) {
        func_800482D0(D_800E7728);
    } else if (!(D_800EFD70_19.flags & 1)) {
        func_80048230(D_800E7728);
    }

    if (D_800EFD70_19.mode == 4 || D_800EFD70_19.mode == 5) {
        D_800E370C = 6;
    } else if (D_800EFD70_19.mode == 9) {
        if (D_800CADE4_5[D_800CA930_3->idx].fl & 4) {
            D_800E370C = 2;
        } else {
            D_800E370C = 1;
        }
    } else if ((D_800EFD70_19.flags & 0x81) == 0x80) {
        D_800E370C = 1;
    } else if (D_800EFD70_19.mode == 0 && D_800EFD70_19.f3C == 2) {
        D_800E370C = D_800EFD70_19.f75C + 2;
    } else if (D_800EFD70_19.mode == 13 || D_800EFD70_19.mode == 14) {
        D_800E370C = 1;
    } else if (D_800EFD70_19.flags & 1) {
        D_800E370C = 6;
        D_800E3704 = 0;
    } else if (D_800EFD70_19.mode == 6) {
        D_800E370C = 4;
        D_800EFD70_19.f3C = 1;
    } else if (D_800EFD70_19.mode == 2 || D_800EFD70_19.mode == 10) {
        D_800E370C = D_800EFD70_19.f3C;
    } else {
        D_800E370C = D_800EFD70_19.f3C + D_800EFD70_19.f44;
    }

    if (D_800EFD70_19.mode == 8 && !(D_800EFD70_19.flags & 1)) {
        D_800E370C = 1;
    }
    v = 0xFF;
    for (i = 7; i >= 0; i--) {
        D_800C238C[i] = v;
    }
    if (D_800EFD70_19.flags & 0x10) {
        D_800E370C = 6;
    }
    func_80014E10();
    if (D_800EFD70_19.flags & 0x40000) {
        D_800E370C = D_800EFD70_19.f3C;
        D_800EFD70_19.f3C = 1;
    }
    func_80050838(D_800E370C, D_800EFD70_19.f40);
    if (!(D_800EFD70_19.flags & 0x8081)) {
        func_80064DF4(D_800EFD70_19.f3C, D_800F05A8_8.h22 - D_800EFD70_19.f3C);
        func_80064D28(D_800EFD70_19.f3C, D_800F05A8_8.h22 - D_800EFD70_19.f3C);
    }
    D_800E76C8 = 0.016666668f;
    func_800358C0();
    func_8000ED10(D_800E36E0_2, 0, 0x18);
    func_8006A8B0((D_800EFD70_19.flags >> 18) & 1);
    D_800C9644_5 = 0;
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
    if (D_800C2358 != 0 && D_800EFD64_14->f2C == 0) {
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
        *D_800E37C0_14[i].p = *D_800E1A78_3;
    }
    func_8000F240(D_800C9080);
    func_80019E78();
    func_8005D270();
    func_8003A780();
    func_80071990();
    if (D_800EFD70_19.mode == 9) {
        func_8005B05C();
    }
    if (D_800C2358 != 0) {
        func_8000BEA0();
        func_8001A044();
    }
    if (D_800EFD70_19.flags & 0x80) {
        D_800E36F8 = 0;
        D_800E374C = -1;
        func_800166CC();
    } else if (D_800EFD70_19.flags & 1) {
        D_800E36F8 = 5;
    } else {
        D_800E36F8 = 4;
        D_800E374C = -1;
    }
    D_800C8CE0 = 0;
    if (D_800EFD70_19.mode == 8 && !(D_800EFD70_19.flags & 0x80) && D_800E3700 == 0) {
        D_800C8CE0 = 1;
    }
}

void func_80015CD8(void)
{
    u32 *r;
    u32 t;
    u32 f;

    func_8000D0D8();
    if ((*(u32 *)(D_800EFD64 + 0x50) & 2) != 0 && (D_800EFDBC_2 - 0xD) < 2) {
        func_800BCB04(D_800C9570);
        D_800C9570 = 0;
    }
    D_800EFD70 = D_800EFD70 & 0xFFFEFFFF;
    func_80062D38();
    func_8005E960();
    func_8003F658();
    r = (u32 *)&D_800EFD70;
    func_8000F600();
    func_8004B6CC();
    func_8003A720();
    func_8006B160();
    func_800020CC();
    func_80066268();
    func_800229B8();
    func_8005AE4C();
    func_8003C2AC();
    func_800BFDD4(D_800E7728);
    func_8006A944();
    func_80069C6C();
    func_80071978();
    func_8003A84C();
    func_80072A9C();
    if (D_800C2358 != 0) {
        func_8000CC48();
        func_8001A0D4();
    }
    t = D_800EFD70 & 0xFFFFBFFF;
    f = D_800EFD70 & 1;
    D_800EFD70 = t;
    if (f == 0) {
        func_8004834C();
    }
    func_80015124(1);
    D_800CCCCC = 0;
    D_800F0DA0 = 1;
    if (D_800C2358 != 0) {
        D_800CA920 = 7;
        r[0x13] = 0;
    }
}

typedef struct { s32 f00; u8 pad04[0x38]; s32 f3C; u8 pad40[0xC]; s32 f4C; } Blk_7;
typedef struct { u8 pad[0x22]; s16 f22; } Blk2;

extern Blk_7 D_800EFD70_20;
extern Blk2 D_800F05A8_9;

void func_80015E60(void)
{
    while (func_80065858() != 0) { }
    func_8000F164();
    if (D_800C8CD8 != 0) {
        D_800C2594 = 1;
    } else {
        D_800C2594 = D_800EFD70_20.f3C;
    }
    D_800E1898 = D_800C2594;
    if (D_800EFD70_20.f4C == 9) {
        func_8004A5FC(D_800E7728);
    }
    if ((D_800EFD70_20.f00 & 0x8085) == 0 && (D_800EFD70_20.f4C != 1 || D_800CA930_2->f64 == 0)) {
        func_80064DF4(D_800EFD70_20.f3C, D_800F05A8_9.f22 - D_800EFD70_20.f3C);
        func_80064D28(D_800EFD70_20.f3C, D_800F05A8_9.f22 - D_800EFD70_20.f3C);
    }
    D_800E3710 = 0;
    if ((D_800EFD70_20.f00 & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && D_800C2940 != 0 &&
        (D_800EFD70_20.f3C == 1 || D_800EFD70_20.f4C == 8) &&
        D_800EFD64_4->f2C != 2 && (D_800EFD70_20.f00 & 0x04040000) == 0) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();
    func_80019E78();
    if (D_800EFD64_4->f2C == 2) {
        func_80044358();
    }
    func_80028600(D_800E7728);
    func_8001A190();
    D_800C9684 = 0;
    if (0 < D_800C2594) {
        do {
            func_80028864();
            D_800C9684 = D_800C9684 + 1;
        } while (D_800C9684 < D_800C2594);
    }
    func_8005D270();
    func_8003E1FC(D_800E7728);
    func_8003A694(D_800C9080);
    func_8006AC40_2();
    func_8003C21C();
    func_80067B80();
    if (D_800EFD64_4->f2C == 1) {
        func_800282C8();
    }
    D_800C2350 = D_800C2350 | 0x800;
    func_80052C90();
    func_8000F240(D_800C9080);
    func_8003A780();
    func_80071990();
    func_8004F47C();
    D_800C8CE0 = 0;
    if (D_800EFD70_20.f4C == 8 && (D_800EFD70_20.f00 & 0x80) == 0 && D_800E3700 == 0) {
        D_800C8CE0 = 1;
    }
    func_80035638();
    if (D_800E1898 >= 2) {
        D_800F0DA0 = 2;
    } else {
        D_800F0DA0 = 1;
    }
}

typedef struct {
    u32 flags;          /* 0x00 */
    u8 pad04[0x38];
    s32 f3C;            /* 0x3C */
    s32 f40;            /* 0x40 */
    s32 f44;            /* 0x44 */
    u8 pad48[4];
    s32 f4C;            /* 0x4C */
    u8 pad50[0x75C - 0x50];
    s32 f75C;           /* 0x75C */
} Ctl_3;

typedef union {
    u32 w;
    struct {
        u32 x : 4;
        u32 y : 4;
        u32 rest : 24;
    } b;
} Pad_2;

extern Pad_2 D_800E30B0_5;
extern Ctl_3 D_800EFD70_21;

void func_8001618C(void)
{
    s32 up, down, b10, b20, b40;
    u32 f;

    if ((D_800E30B0_5.w & 3) && ((D_800E30B0_5.w & 1) ? D_800E30B0_5.b.x : -D_800E30B0_5.b.x) >= 4) {
        D_800E30B0_5.w |= 0x800;
    } else if ((D_800E30B0_5.w & 3) && ((D_800E30B0_5.w & 1) ? D_800E30B0_5.b.x : -D_800E30B0_5.b.x) < -3) {
        D_800E30B0_5.w |= 0x1000;
    }
    if ((D_800E30B0_5.w & 0xC) && ((D_800E30B0_5.w & 8) ? ((s32)D_800E30B0_5.w >> 24 & 0xF) : -((s32)D_800E30B0_5.w >> 24 & 0xF)) >= 4) {
        D_800E30B0_5.w |= 0x4000;
    } else if ((D_800E30B0_5.w & 0xC) && ((D_800E30B0_5.w & 8) ? ((s32)D_800E30B0_5.w >> 24 & 0xF) : -((s32)D_800E30B0_5.w >> 24 & 0xF)) < -3) {
        D_800E30B0_5.w |= 0x2000;
    }
    f = D_800E30B0_5.w;
    up = (f & 0x2000) && !(D_800E25B0 & 0x2000);
    down = (f & 0x4000) && !(D_800E25B0 & 0x4000);
    b10 = (f & 0x10) && !(D_800E25B0 & 0x10);
    b20 = (f & 0x20) && !(D_800E25B0 & 0x20);
    b40 = (f & 0x40) && !(D_800E25B0 & 0x40);

    if (D_800EFD70_21.f4C == 0 && D_800EFD70_21.f3C >= 2) {
        if (up && D_800EFD70_21.f40 >= 2) D_800EFD70_21.f40--;
        if (down && D_800EFD70_21.f40 < 6) D_800EFD70_21.f40++;
    }

    switch (D_800E36F8_2) {
    case 0:
        if (D_800E374C != -1) {
            if (--D_800E374C == 0) D_800E36F8_2 = 1;
        }
        break;
    case 2:
        if (D_800E374C == -3) {
            if (D_800CA920 == 3) {
                func_8001683C();
                D_800E36F8_2 = 7;
                D_800E374C = 100;
                D_800C29CC->f10 = 3;
            } else {
                D_800E36F8_2 = 3;
                D_800E374C = 2;
            }
        } else if (D_800E374C > 0) {
            if (--D_800E374C == 0) D_800E374C = -2;
        }
        break;
    case 3:
        if (D_800E374C == 0) {
            D_800E36F8_2 = 4;
            D_800E374C = -1;
            func_80015124(0);
            func_8002D8A0();
            func_800229B8();
            func_8005E960();
            func_8003F658();
            func_8003A720();
            func_8006B160();
            func_8003C2AC();
            func_8000F600();
            func_80069C6C();
            func_8003A84C();
            func_80072A9C();
            if (D_800EFD70_21.f4C == 9) func_8004B6CC();
            if (D_800C2350 & 0x800) func_8005B33C();
            func_8001683C();
            D_800EFD70_21.flags &= ~0x80;
            D_800E370C = D_800EFD70_21.f3C + D_800EFD70_21.f44;
            if (D_800EFD70_21.f4C == 8 && !(D_800EFD70_21.flags & 1)) {
                D_800E370C = 1;
            } else if (D_800EFD70_21.f4C == 0 && D_800EFD70_21.f3C == 2) {
                D_800E370C = D_800EFD70_21.f75C + 2;
            }
            func_80014E10();
            func_80050838(D_800E370C, D_800EFD70_21.f40);
            func_80015E60();
            if (D_800EFD70_21.f4C == 9) func_8005B05C();
        }
        break;
    case 1:
        if (b20 && (D_800EFD70_21.f4C != 1 || D_800CA930_2->f64 == 0)) {
            func_8001102C(2);
            D_800E36F8_2 = 2;
            D_800E374C = 20;
            D_800CA920 = 3;
            D_800EFD70_21.flags &= ~4;
        } else if (b10 || b40) {
            func_8001102C(1);
            D_800CA920 = 6;
            D_800E36F8_2 = 2;
            D_800E374C = 20;
        }
        break;
    }
}

void func_800166CC(void)
{
    s32 i;
    s32 r;

    D_800C2354 = func_800BC908(0x148, 0, D_800D6170, 0x52C);
    i = 0;
    do {
        *(s32 *)((u8 *)D_800C2354 + (i << 2)) = (s32)func_800C1E60((s32 *)0, 0x40, 1, 2, 0x20);
        i = i + 1;
    } while (i < 0x4a);
    r = func_800BF79C(D_800E7728, D_800D088C);
    D_800C2354[0x4a] = r;
    r = func_800BF79C(D_800E7728, D_800D09B0);
    D_800C2354[0x50] = r;
    if (D_800EFDBC == 1) {
        i = 0;
        do {
            r = func_800BF79C(D_800E7728, *(s32 *)D_800C8D68[*(s16 *)((u8 *)*(s16 **)D_800CA930 + (i << 1) + 8)]);
            *(s32 *)((u8 *)D_800C2354 + (i << 2) + 0x12C) = r;
            i = i + 1;
        } while (i < 5);
    }
    r = func_800BF79C(D_800E7728, D_800D085C);
    D_800C2354[0x51] = r;
}

typedef struct {
    s32 unk0;
    s32 *unk4;
} Unk7728;

typedef struct {
    u8 pad[8];
    s16 a[5];
} Row_2;

extern Unk7728 *D_800E7728_3;
extern Row_2 **D_800CA930_7;

void func_8001683C(void) {
    s32 i;
    s32 off;

    for (i = 0; i < 0x4A; i++) {
        off = i * 4;
        func_800C204C(*(void **)((s32)D_800C2354 + off));
        func_800BCB04(*(void **)((s32)D_800C2354 + off));
    }
    func_800BFEA4(&D_800E7728_3->unk4[D_800D088C]);
    func_800BFEA4(&D_800E7728_3->unk4[D_800D09B0]);
    func_800BFEA4(&D_800E7728_3->unk4[D_800D085C]);
    if (D_800EFDBC == 1) {
        for (i = 0; i < 5; i++) {
            func_800BFEA4(&D_800E7728_3->unk4[*D_800C8D68_2[(*D_800CA930_7)->a[i]]]);
        }
    }
    func_800BCB04(D_800C2354);
    D_800C2354 = NULL;
    if (D_800CA920 != 3) {
        D_800EFD70 &= ~0x80;
    }
    func_80048230(D_800E7728_3);
}

#define PAD (D_800EFD70_22.f2C8[D_800E37AC])
#define PRESSED(m) ((D_800E30B0_2[PAD] & (m)) && !(D_800E25B0_2[PAD] & (m)))

typedef struct {
    u8 pad0[0x17C];
    s32 f17C;
    u8 pad180[0x2C8 - 0x180];
    s8 f2C8[4];
} Ctl_4;

extern Ctl_4 D_800EFD70_22;

void func_800169BC(void) {
    s32 s0;
    s32 old;
    s32 b;

    if (D_800E3708 != 0) {
        D_800E3708--;
    }
    if (++D_800EFD70_22.f17C == 60) {
        D_800EFD70_22.f17C = 0;
    }
    if (((D_800E30B0_2[PAD] >> 24) & 0xF) < 6) {
        D_800E30B0_2[PAD] &= ~0xC;
    }
    if ((s32)((u32)D_800E30B0_2[PAD] >> 28) < 6) {
        D_800E30B0_2[PAD] &= ~0x3;
    }
    if (D_800E30B0_2[PAD] & 4) {
        D_800E30B0_2[PAD] |= 0x2000;
    }
    if (D_800E30B0_2[PAD] & 8) {
        D_800E30B0_2[PAD] |= 0x4000;
    }
    if (D_800E30B0_2[PAD] & 1) {
        D_800E30B0_2[PAD] |= 0x800;
    }
    if (D_800E30B0_2[PAD] & 2) {
        D_800E30B0_2[PAD] |= 0x1000;
    }
    if (PRESSED(0x20)) {
        func_80010E5C(0, 0);
        func_80010E5C(1, 0);
        func_80010E5C(2, 0);
        func_8006F5F8(5, 0);
        D_800E3710 = 1;
        D_800E3730 = 3;
        func_800B8F2C();
        func_8004F534();
        return;
    }
    if (PRESSED(0x10)) {
        switch (D_800E36FC) {
        case 0:
            break;
        case 3:
            func_8001102C(1);
            D_800F1900.f2C4 ^= 1;
            return;
        case 1:
            func_800B7990(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
            return;
        case 2:
            if (D_800E3708 == 0) {
                func_8004F4DC(0x9B);
                D_800E3708 = 0x34;
            }
            return;
        case 4:
            func_8001102C(3);
            return;
        }
        return;
    }
    old = D_800E36FC;
    if (PRESSED(0x1000)) {
        if (old < 3) {
            D_800E36FC = old + 1;
        } else {
            D_800E36FC = old + 2;
        }
        if (D_800E36FC >= 4) {
            D_800E36FC = 0;
        }
    }
    if (PRESSED(0x800)) {
        if (D_800E36FC < 4) {
            D_800E36FC = D_800E36FC - 1;
        } else {
            D_800E36FC = D_800E36FC - 2;
        }
        if (D_800E36FC < 0) {
            D_800E36FC = 3;
        }
    }
    if (old != D_800E36FC) {
        switch (old) {
        case 0:
        case 1:
            break;
        case 2:
            func_8004F534();
            break;
        }
        switch (D_800E36FC) {
        case 0:
            break;
        case 2:
            func_8004F4DC(0x9B);
            D_800E3708 = 0x34;
            break;
        case 1:
            func_800B7990(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
            break;
        }
    }
    if (D_800E36FC < 3) {
        if (PRESSED(0x2000)) {
            s0 = -8;
        } else if (PRESSED(0x4000)) {
            s0 = 8;
        } else {
            b = D_800E30B0_2[PAD];
            if (b & 0xC) {
                if (b & 8) {
                    s0 = ((b >> 24) & 0xF) / 3;
                } else {
                    s0 = -((b >> 24) & 0xF) / 3;
                }
            } else {
                s0 = 0;
            }
        }
        if (s0 != 0 && D_800E3708 == 0) {
            switch (D_800E36FC) {
            case 1:
                func_800B7990(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
                D_800E3708 = 0x28;
                break;
            case 2:
                func_8004F4DC(0x9B);
                D_800E3708 = 0x34;
                break;
            }
        }
        if (s0 < 0) {
            switch (D_800E36FC) {
            case 1:
                if (D_800F1900.f2C6 >= -s0) {
                    D_800F1900.f2C6 += s0;
                } else {
                    D_800F1900.f2C6 = 0;
                }
                break;
            case 0:
                if (D_800F1900.f2C5 >= -s0) {
                    D_800F1900.f2C5 += s0;
                } else {
                    D_800F1900.f2C5 = 0;
                }
                break;
            case 2:
                if (D_800F1900.f2C7 >= -s0) {
                    D_800F1900.f2C7 += s0;
                } else {
                    D_800F1900.f2C7 = 0;
                }
                break;
            }
        } else {
            s32 v;
            switch (D_800E36FC) {
            case 1:
                v = D_800F1900.f2C6 + s0;
                if (v >= 0x100) {
                    D_800F1900.f2C6 = 0xFF;
                } else {
                    D_800F1900.f2C6 = v;
                }
                break;
            case 0:
                v = D_800F1900.f2C5 + s0;
                if (v >= 0x100) {
                    D_800F1900.f2C5 = 0xFF;
                } else {
                    D_800F1900.f2C5 = v;
                }
                break;
            case 2:
                v = D_800F1900.f2C7 + s0;
                if (v >= 0x100) {
                    D_800F1900.f2C7 = 0xFF;
                } else {
                    D_800F1900.f2C7 = v;
                }
                break;
            }
        }
    }
    func_80010E5C(0, 0);
    func_80010E5C(1, 0);
    func_80010E5C(2, 0);
    switch (D_800E36FC) {
    case 1:
        func_80010E5C(0, D_800F1BC6);
        break;
    case 0:
        func_80010E5C(1, D_800F1BC5_2);
        break;
    case 2:
        func_80010E5C(2, D_800F1BC7);
        break;
    }
    if (D_800E36FC != old) {
        func_8001102C(0);
    }
}

void func_80017214(void)
{
    s32 cur;
    s32 n;
    u8 *pc;

    if (((D_800E30B0_3[(s8)D_800EFD70_2[0x2C8 + D_800E37AC]] & 0x20) != 0) &&
        ((D_800E25B0_3[(s8)D_800EFD70_2[0x2C8 + D_800E37AC]] & 0x20) == 0)) {
        D_800E3710 = 1;
        D_800E3730 = 3;
        func_8006F5F8(5, 0);
    } else {
        u32 *t30 = D_800E30B0_3;
        cur = (s8)D_800EFD70_2[0x2C8 + D_800E37AC];
        if (((t30[cur] & 0x10) != 0) && ((D_800E25B0_3[cur] & 0x10) == 0)) {
            pc = D_800EFD70_2 + cur;
            n = pc[0x50] + 1;
            pc[0x50] = n;
            if (3 < (s8)n) {
                u8 *pd = D_800EFD70_2 + (s8)D_800EFD70_2[0x2C8 + D_800E37AC];
                pd[0x50] = pd[0x50] - 4;
            }
            func_8001102C(0);
        }
    }
}

#define PAD_2 (D_800EFD70_23.f2C8[D_800E37AC])
#define PRESSED_2(m) ((D_800E30B0_3[PAD_2] & (m)) && !(D_800E25B0_3[PAD_2] & (m)))

typedef struct { u32 w[6]; } Pad24;

typedef struct {
    u32 f0;             /* 0x000 */
    u8 pad04[0x38];
    s32 f3C;            /* 0x03C */
    s32 f40;            /* 0x040 */
    s32 f44;            /* 0x044 */
    u8 pad48[4];
    s32 f4C;            /* 0x04C */
    u8 pad50[4];
    s32 f54;            /* 0x054 */
    u8 pad58[0x1C];
    s32 f74[0x30];      /* 0x074 */
    u8 pad134[0x2C8 - 0x134];
    s8 f2C8[4];         /* 0x2C8 */
    s8 f2CC[4];         /* 0x2CC */
    u8 pad2D0[0x75C - 0x2D0];
    s32 f75C;           /* 0x75C */
} Game_10;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x28];
    u16 f58[4];
    u16 f60[4];
} Cfg64;

typedef struct {
    u8 pad0[0x38];
    s32 f38;
} Obj4E4;

typedef struct {
    u8 pad0[0x420];
    f32 f420;           /* 0x420 */
    u8 pad424[0x4E4 - 0x424];
    Obj4E4 *f4E4;       /* 0x4E4 */
    u8 pad4E8[0x5AC - 0x4E8];
    s32 f5AC;           /* 0x5AC */
    u8 pad5B0[0x5CD - 0x5B0];
    u8 b5CD;            /* 0x5CD */
    u8 pad5CE[0x656 - 0x5CE];
    u8 b656;            /* 0x656 */
    u8 pad657[0x6E8 - 0x657];
    u8 b6E8;            /* 0x6E8 */
    u8 pad6E9[0x768 - 0x6E9];
    s32 f768;           /* 0x768 */
    u8 pad76C[0x774 - 0x76C];
    s32 f774;           /* 0x774 */
    u8 pad778[0x7A5 - 0x778];
    u8 b7A5;            /* 0x7A5 */
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_7;

typedef struct {
    s32 f0;             /* 0x00 */
    u8 pad04[0x1E];
    s16 h22;            /* 0x22 */
    u8 pad24[2];
    s16 h26;            /* 0x26 */
    u8 pad28[0x18];
    s32 f40;            /* 0x40 */
    u8 pad44[0x14];
    u32 f58;            /* 0x58 */
    u8 pad5C[0x14];
    s32 f70;            /* 0x70 */
    u8 pad74[4];
    s32 f78;            /* 0x78 */
    u8 pad7C[4];
    s32 f80;            /* 0x80 */
} Sess;

typedef struct {
    u8 pad0[0x2C4];
    u8 b2C4;
    u8 b2C5;
    u8 b2C6;
    u8 b2C7;
} Opt_2;

typedef struct {
    s16 h0;
    u8 b2;
    u8 b3;
    s32 w4;
    s32 w8;
    s32 wC;
    s32 w10;
    s32 w14;
} Gui;

typedef struct {
    u8 pad0[4];
    s32 *f4;
} Res_2;

typedef struct {
    u8 pad0[0x64];
    s16 h64;
} S930;

typedef struct {
    u8 pad0[0xC];
    u8 bC;
    u8 padD[0x28 - 0xD];
} Trk_6;

typedef struct {
    s32 *a;
    s32 *b;
    s32 *w;
    s32 *h;
    s32 *c;
    void *d;
} Cfg_5;

extern Game_10 D_800EFD70_23;
extern Cfg64 *D_800EFD64_15;
extern Pad24 D_800E3770_3;
extern Pad24 D_800E36E0_4;
extern Pad24 D_800E3750;
extern Pad24 D_800E3718;
extern Player_7 D_800E37C0_15[8];
extern Sess D_800F05A8_10;
extern Gui D_800E3730_6;
extern Res_2 *D_800E7728_4;
extern Opt_2 D_800F1900_3;
extern S930 *D_800CA930_8;
extern Trk_6 D_800CADE4_6[];

void func_80017338(u32 *arg)
{
    Cfg_5 cfg;
    Pad16 buf;
    Pad16 spare;
    s32 w;
    s32 h;
    s32 flag;
    s32 i;
    s32 j;
    s32 t;

    flag = 0;
    if ((arg[0] >> 28) == 3 && D_800C8CD0 != 0) {
        D_800C29DC = 0x80020000;
    }
    buf = *(Pad16 *)arg;
    func_8000ED10((u8 *)&buf, 0, 0x10);
    for (j = 0; j < D_800EFD70_23.f3C; j++) {
        buf.w[j] = arg[D_800EFD70_23.f2C8[j]];
    }
    func_8000ED10((u8 *)&D_800E3770_3, 0, 0x18);
    *(Pad16 *)&D_800E3770_3 = buf;
    for (i = 0; i < 4; i++) {
        D_800E3770_3.w[i] = func_80022324(D_800E3770_3.w[i], i);
    }
    for (i = 0; i < 4; i++) {
        if (D_800E3770_3.w[i] & 0x2000) D_800E3770_3.w[i] |= 0x0F000004;
        if (D_800E3770_3.w[i] & 0x4000) D_800E3770_3.w[i] |= 0x0F000008;
        if (D_800E3770_3.w[i] & 0x800) D_800E3770_3.w[i] |= 0xF0000001;
        if (D_800E3770_3.w[i] & 0x1000) D_800E3770_3.w[i] |= 0xF0000002;
        if (D_800E3770_3.w[i] & 4) D_800E3770_3.w[i] &= ~8;
        if (D_800E3770_3.w[i] & 1) D_800E3770_3.w[i] &= ~2;
        D_800E3770_3.w[i] &= ~0x7800;
    }
    for (i = 0; i < 4; i++) {
        t = D_800E76D0 - D_800E37C0_15[i].f774 + 4;
        j = 0;
        if (D_800E37C0_15[i].b5CD) {
            t = t * 2 / 3;
        }
        if (t < D_800E37C0_15[i].f768 / 20) j = 1;
        if (D_800F07D8[i] != 0 && !(D_800E2038 & 7)) j = 1;
        if (D_800E37C0_15[i].f4E4 != NULL && D_800E37C0_15[i].f4E4->f38 != 0 &&
            D_800E37C0_15[i].f420 > 8.0f && D_800E2038 % 6 == 0) j = 1;
        if (D_800E3710 != 0) j = 0;
        if (D_800E37C0_15[i].b6E8 != 0) j = 0;
        if (D_800C8CD8 != 0 && i == D_800E3700) j = 0;
        if (D_800C8CE8 != 0) j = 0;
        if (D_800EFD70_23.f54 != 3) j = 0;
        if (D_800EFD70_23.f0 & 1) j = 0;
        if (D_800EFD70_23.f4C == 9 && D_800F05A8_10.f0 == 0xE) j = 0;
        t = D_800EFD70_23.f2C8[i];
        if (j) func_8000D0A0(t, 1);
    }
    func_80067B54();
    func_80072114();
    func_800716C0();
    {
        s32 *tab = D_800C9550;
        s32 *tab2 = D_800C9560;
        w = D_800EFD64_15->f58[D_800C2594 - 1];
        h = D_800EFD64_15->f60[D_800C2594 - 1];
        cfg.a = &tab[D_800C2594 - 1];
        cfg.b = &tab2[D_800C2594 - 1];
    }
    cfg.w = &w;
    cfg.h = &h;
    cfg.c = &D_800DC0B4;
    cfg.d = &D_800C2B58;
    if (D_800E3710 == 2 || D_800E3710 == 3) {
        if (D_800E3730_6.h0 == 0 && D_800E3730_6.b3 == 0) {
            func_800BCB04(D_800C29E0);
            D_800E3730_6.b3 = 1;
            if (D_800E3710 == 2) {
                func_800482D0(D_800E7728_4);
            }
            func_800BF530("guires", &D_800F47C0);
            for (j = 0; j < 0x30; j++) {
                D_800EFD70_23.f74[j] = func_800C1E60(0, 0x40, 4, 2, 0x20);
            }
            if (D_800E3710 == 2) {
                D_800E3730_6.w4 = func_800BF79C((s32)D_800E7728_4, D_800D0C24);
            } else {
                D_800E3730_6.w8 = func_800BF79C((s32)D_800E7728_4, D_800D108C);
                D_800E3730_6.w4 = func_800BF79C(D_800F47C0, D_800D1488);
                D_800E3730_6.w10 = func_800BF79C(D_800F47C0, D_800D1428);
                D_800E3730_6.w14 = func_800BF79C(D_800F47C0, D_800D1424);
                D_800E3730_6.wC = func_800BF79C(D_800F47C0, D_800D1478);
            }
            D_800E3730_6.h0 = -1;
            while (func_80065858() != 0) {
            }
            D_800CCCA4 = 1;
            while (D_800CCCA8 != D_800CCCA4) {
            }
            func_80000F0C();
        }
    } else if (D_800E3710 == 1 && D_800E3730_6.h0 == 0 && D_800E3730_6.b3 != 0) {
        D_800E3730_6.b3 = 0;
        D_800E3730_6.b2 = 2;
        for (j = 0; j < 0x30; j++) {
            func_800C204C(D_800EFD70_23.f74[j]);
            func_800BCB04(D_800EFD70_23.f74[j]);
        }
        func_800BFDD4(D_800F47C0);
        func_800BFEA4(&D_800E7728_4->f4[D_800D0C24]);
        func_800BFEA4(&D_800E7728_4->f4[D_800D1080]);
        func_800BFEA4(&D_800E7728_4->f4[D_800D0724]);
        func_800BFEA4(&D_800E7728_4->f4[D_800D108C]);
        func_8004834C();
        D_800C29E0 = func_800BCCF0(D_800C29E0, D_800E20E8 * D_800E20EC * 2, "min.c", 0x799);
        if (D_800C29E0 == 0) {
            func_800BC3D0_2();
            while (1) {
            }
        }
        if (!(D_800EFD70_23.f0 & 0x81) && (D_800F1900_3.b2C4 & 2) && D_800C2940 != 0 &&
            (D_800EFD70_23.f3C == 1 || D_800EFD70_23.f4C == 8) && D_800EFD64_15->f2C != 2 &&
            !(D_800EFD70_23.f0 & 0x04040000)) {
            D_800CCCA4 = 2;
        } else {
            D_800CCCA4 = 1;
        }
        while (D_800CCCA8 != D_800CCCA4) {
        }
        func_80000F0C();
        func_80048230(D_800E7728_4);
        D_800E3730_6.h0 = -1;
    }
    if (D_800EFD70_23.f4C != 9 && !(D_800EFD70_23.f0 & 0x8010) && (D_800E30B0_3[0] & 0x400) &&
        !(D_800E25B0_3[0] & 0x400)) {
        D_800CA934 = (D_800CA934 + 1) & 3;
    }
    func_800368A4();
    func_800283C0();
    if (!(D_800EFD70_23.f0 & 0x80)) {
        if (D_800C8CE8 != 0 && D_800E36F8_2 == 5 &&
            ((D_800E3770_3.w[0] | D_800E3770_3.w[1] | D_800E3770_3.w[2] | D_800E3770_3.w[3]) & 0x50)) {
            D_800E36F8_2 = 6;
            D_800E374C = -1;
            D_800CA920 = 2;
            D_800EFD70_23.f0 &= ~4;
            func_8001102C(1);
            return;
        }
    } else {
        func_8001618C();
    }

    if (D_800E3710 == 2) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            func_80017214();
        }
    } else if (D_800E3710 == 3) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            func_800169BC();
        }
    } else if (D_800E3710 == 6) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            if (PRESSED_2(0x20)) {
                D_800E3710 = 0;
                func_80010E5C(0, D_800F1900_3.b2C6);
                func_80010E5C(1, D_800F1900_3.b2C5);
                func_80010E5C(2, D_800F1900_3.b2C7);
                func_8001102C(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 0:
                    D_800E3710 = 0;
                    func_80010E5C(0, D_800F1900_3.b2C6);
                    func_80010E5C(1, D_800F1900_3.b2C5);
                    func_80010E5C(2, D_800F1900_3.b2C7);
                    break;
                case 1:
                    func_800229B8();
                    func_8000ED10((u8 *)D_800E37C0_15, 0, 0x3F00);
                    func_8001A190();
                    D_800E3710 = 0;
                    D_800F05A8_10.f58 |= 0x80;
                    func_80010E5C(0, D_800F1900_3.b2C6);
                    func_80010E5C(1, D_800F1900_3.b2C5);
                    func_80010E5C(2, D_800F1900_3.b2C7);
                    break;
                case 2:
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 8;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                D_800E3768 = (D_800E3768 + 2) % 3;
                func_8001102C(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                D_800E3768 = (D_800E3768 + 1) % 3;
                func_8001102C(0);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730_6.b2 = 2;
        }
    } else if (D_800E3710 == 5) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            if (PRESSED_2(0x20) && D_800F0746_2 == 0 && D_800F06AC == 0) {
                D_800E3710 = 0;
                func_80010E5C(0, D_800F1900_3.b2C6);
                func_80010E5C(1, D_800F1900_3.b2C5);
                func_80010E5C(2, D_800F1900_3.b2C7);
                func_8001102C(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 4:
                    D_800F05A8_10.f70 = 3;
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    break;
                case 3:
                    D_800F05A8_10.f70 = 4;
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    break;
                case 1:
                    D_800F05A8_10.f70 = 7;
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    break;
                case 0:
                    D_800F05A8_10.f70 = 6;
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    break;
                case 2:
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 2;
                    D_800EFD70_23.f0 &= ~4;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                do {
                    D_800E3768 = (D_800E3768 + 4) % 5;
                } while ((D_800E3768 == 3 && func_800595A4() == 0) ||
                         (D_800E3768 == 1 && func_8005B55C() == 0) ||
                         (D_800E3768 == 4 && func_8005AE38() == 0));
                func_8001102C(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                do {
                    D_800E3768 = (D_800E3768 + 1) % 5;
                } while ((D_800E3768 == 3 && func_800595A4() == 0) ||
                         (D_800E3768 == 1 && func_8005B55C() == 0) ||
                         (D_800E3768 == 4 && func_8005AE38() == 0));
                func_8001102C(0);
            }
            if (D_800E3710 == 0) {
                func_800B9F4C();
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730_6.b2 = 2;
        }
    } else if (D_800E3710 == 1) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            if (PRESSED_2(0x20)) {
                func_80010E5C(0, D_800F1900_3.b2C6);
                func_80010E5C(1, D_800F1900_3.b2C5);
                func_80010E5C(2, D_800F1900_3.b2C7);
                D_800E3710 = 0;
                func_8001102C(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 0:
                    D_800E3710 = 2;
                    D_800E3708 = 0;
                    D_800E36FC = D_800EFD70_23.f2CC[PAD_2];
                    D_800E3730_6.h0 = 3;
                    break;
                case 1:
                    D_800E3710 = 3;
                    D_800E3730_6.h0 = 3;
                    D_800E36FC = 0;
                    break;
                case 4:
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    D_800EFD70_23.f0 |= 4;
                    if (D_800EFD70_23.f4C == 1) {
                        func_8005B28C();
                    }
                    break;
                case 3:
                    D_800E3710 = 4;
                    D_800E3768 = 1;
                    break;
                case 2:
                case 5:
                    D_800E3710 = 0;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                D_800E3768 = (D_800E3768 + (D_800EFD70_23.f4C == 1 ? 3 : 4)) % (D_800EFD70_23.f4C == 1 ? 4 : 5);
                func_8001102C(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                D_800E3768 = (D_800E3768 + 1) % (D_800EFD70_23.f4C == 1 ? 4 : 5);
                func_8001102C(0);
            }
            if (D_800E3710 == 0) {
                func_800B9F4C();
                func_80010E5C(0, D_800F1900_3.b2C6);
                func_80010E5C(1, D_800F1900_3.b2C5);
                func_80010E5C(2, D_800F1900_3.b2C7);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730_6.b2 = 2;
        }
    } else if (D_800E3710 == 4) {
        if (D_800E36F8_2 == 5 && D_800E3730_6.h0 == -1) {
            if (PRESSED_2(0x20)) {
                D_800E3710 = 0;
                func_8001102C(2);
                func_80010E5C(0, D_800F1900_3.b2C6);
                func_80010E5C(1, D_800F1900_3.b2C5);
                func_80010E5C(2, D_800F1900_3.b2C7);
            } else if (PRESSED_2(0x10)) {
                switch (D_800E3768) {
                case 0:
                    D_800E36F8_2 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 4;
                    D_800EFD70_23.f0 &= ~4;
                    func_8001102C(1);
                    if (D_800EFD70_23.f4C == 1) {
                        func_8005B28C();
                    }
                    break;
                case 1:
                    func_8001102C(1);
                    D_800E3710 = 1;
                    D_800E3768 = 2;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                D_800E3768 = 0;
                func_8001102C(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                D_800E3768 = 1;
                func_8001102C(0);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730_6.b2 = 2;
        }
    } else {
        if (D_800F05A8_10.f58 & 0x800) {
            func_80052A6C(0);
            func_80010E5C(0, 0);
            if (D_800F05A8_10.f80 != 0) {
                if ((D_800E3770_3.w[0] & 0x10) && !(D_800E36E0_4.w[0] & 0x10)) {
                    func_8001102C(1);
                    D_800F05A8_10.f80 = 1;
                }
                if (--D_800F05A8_10.f80 == 0) {
                    D_800F05A8_10.f58 &= 0xFFFE77FF;
                    func_80010E5C(0, D_800F1900_3.b2C6);
                }
            }
            if (D_800F05A8_10.f58 & 0x10000) {
                D_800F05A8_10.f58 &= ~0x10000;
                D_800C9684 = 0;
                func_80028864();
            }
        } else {
            D_800E3750 = D_800E3770_3;
            func_80015254();
            if (D_800C8CE8 != 0 && D_800C9650 != -1 && !(D_800EFD70_23.f0 & 0x80)) {
                for (j = 0; j < 4; j++) {
                    if ((D_800E30B0_3[j] & 0x400) && !(D_800E25B0_3[j] & 0x400)) break;
                }
                if (j != 4) {
                    for (j = 1; j <= D_800F05A8_10.h22; j++) {
                        t = (D_800C9650 + j) % D_800F05A8_10.h22;
                        if (t < D_800EFD70_23.f3C) break;
                    }
                    D_800C9650 = t;
                }
            }
            if (D_800C2358 == 0) {
                func_80022448(1);
            } else {
                func_8000C450();
            }
            if (D_800EFD70_23.f0 & 0x10) {
                for (D_800C9684 = 0; D_800C9684 < 4; D_800C9684++) {
                    func_80028864();
                }
            } else {
                for (D_800C9684 = 0; D_800C9684 < D_800C2594; D_800C9684++) {
                    func_80028864();
                }
            }
            if (D_800C2358 == 0) {
                func_8003ED1C();
                func_8003F128();
                func_8003F204();
            }
            func_8006ADF0();
            func_8000F7A4();
            if (D_800EE140 == 0) {
                func_80052A6C(0);
            }
            for (j = 0; j < 8; j++) {
                if (D_800E37C0_15[j].b7A5 == 0) {
                    if (D_800E37C0_15[j].b656 != 0 && D_800E37C0_15[j].f5AC >= -0xFFF) {
                        D_800E37C0_15[j].f5AC -= 0x200;
                    }
                    if (D_800E37C0_15[j].b656 == 0 && D_800E37C0_15[j].f5AC < 0) {
                        D_800E37C0_15[j].f5AC += 0x200;
                    }
                }
            }
        }
    }

    if (D_800C8CE8 == 0 && D_800F05A8_10.f40 == 0 && D_800E3710 == 0 && !(D_800EFD70_23.f0 & 0x81)) {
        for (j = 0; j < D_800F05A8_10.h22; j++) {
            if (((D_800E3770_3.w[j] & 0x40) && !(D_800E36E0_4.w[j] & 0x40)) ||
                (D_800EFD70_23.f4C == 9 && (D_800E30B0_3[j] & 0x40) && !(D_800E25B0_3[j] & 0x40))) {
                if (flag == 0 && D_800E36F8_2 == 5) {
                    D_800E36E0_4.w[j] |= 0x40;
                    if (D_800C2358 != 0) {
                        D_800E36F8_2 = 6;
                        D_800E374C = -1;
                        D_800CA920 = 4;
                        D_800EFD70_23.f0 &= ~4;
                    } else if (D_800EFD70_23.f4C == 13 || D_800EFD70_23.f4C == 14) {
                        D_800E3710 = 6;
                        D_800E3768 = 0;
                    } else if (D_800EFD70_23.f4C == 9) {
                        D_800E3710 = D_800E36F8_2;
                        D_800E3768 = 0;
                    } else {
                        D_800E3710 = 1;
                        D_800E3768 = 2;
                    }
                    D_800E37AC = j;
                    func_80010F40();
                    func_80010E5C(0, 0);
                    func_80010E5C(1, 0);
                    func_80010E5C(2, 0);
                    func_8004F534();
                    func_8000D0D8();
                    break;
                }
            }
        }
    }

    switch (D_800E36F8_2) {
    case 4:
        if (D_800E374C != -1) {
            if (--D_800E374C == 0) {
                D_800E36F8_2 = 5;
            }
        }
        break;
    case 6:
        if (D_800E374C == -3) {
            if ((D_800EFD70_23.f0 & 4) || (D_800EFD70_23.f4C == 9 && D_800CA920 == 6)) {
                D_800E36F8_2 = 7;
                D_800E374C = 2;
            } else {
                D_800E36F8_2 = 7;
                D_800E374C = 100;
                D_800C29CC->f10 = 3;
            }
        } else if (D_800E374C > 0) {
            if (--D_800E374C == 0) {
                D_800E374C = -2;
            }
        }
        break;
    case 7:
        if (D_800E374C == 0) {
            D_800E36F8_2 = 4;
            D_800E374C = -1;
            func_80015124(0);
            func_8002D8A0();
            func_800229B8();
            func_8005E960();
            func_8003F658();
            func_8003A720();
            func_8006B160();
            func_8003C2AC();
            func_8000F600();
            func_80069C6C();
            func_8003A84C();
            func_80072A9C();
            if (D_800EFD70_23.f4C == 9) func_8004B6CC();
            if (D_800C2350 & 0x800) func_8005B33C();
            if (D_800EFD70_23.f4C == 9) {
                switch (D_800F05A8_10.f70) {
                case 3:
                    D_800CA930_8->h64--;
                    break;
                case 4:
                    D_800CA930_8->h64++;
                    break;
                case 6:
                    D_800F05A8_10.f58 |= 0x4000;
                    break;
                case 7:
                    break;
                default:
                    D_800F05A8_10.f0 = 0xE;
                    D_800F05A8_10.h26 = 0x3F;
                    D_800F05A8_10.f78 = 0x3C;
                    D_800F05A8_10.f70 = 0;
                    D_800F05A8_10.f58 = (D_800F05A8_10.f58 & ~0x10) | 0x40;
                    break;
                }
                D_800E3710 = 0;
                D_800F05A8_10.f58 &= ~0x2000;
                if (D_800CADE4_6[D_800CA930_8->h64].bC & 4) {
                    D_800E370C = 2;
                } else {
                    D_800E370C = 1;
                }
            } else if (D_800EFD70_23.f4C == 6) {
                D_800E370C = 4;
                D_800EFD70_23.f3C = 1;
            } else if (D_800EFD70_23.f4C == 10 || D_800EFD70_23.f4C == 2) {
                D_800E370C = D_800EFD70_23.f3C;
            } else if (D_800EFD70_23.f4C == 0 && D_800EFD70_23.f3C == 2) {
                D_800E370C = D_800EFD70_23.f75C + 2;
            } else {
                D_800E370C = D_800EFD70_23.f3C + D_800EFD70_23.f44;
                if (D_800EFD70_23.f4C == 8 && !(D_800EFD70_23.f0 & 1)) {
                    D_800E370C = 1;
                }
            }
            func_80014E10();
            func_80050838(D_800E370C, D_800EFD70_23.f40);
            func_80015E60();
            if (D_800EFD70_23.f4C == 9) func_8005B05C();
        }
        break;
    }
    D_800E3718 = D_800E3750;
    D_800E36E0_4 = D_800E3770_3;
    if (D_800DC0B4 <= 0) {
        D_800DC0B4 = 1;
    }
    if (D_800DC0B4 >= 9) {
        D_800DC0B4 = 8;
    }
    if (D_800EFD64_15->f2C == 2) {
        func_80044174();
    }
    func_80062D0C();
}

typedef struct { u8 b[12]; } B12;

typedef struct {
    char name[0x10];
    s32 v[6];
    s32 f28;
    B12 b;
    s32 key;
} Src;

typedef struct {
    char name[0x10];
    f32 v[6];
    s32 f28;
    B12 b;
    s32 f38;
    s32 pad3C;
} Dst;

typedef struct {
    s32 n;
    Src e[1];
} File;

struct Res_3 {
    s32 f0;
    u32 *arr;
};

extern Dst D_800C9080_3[];

void func_800195B8(Res_3 *r, s32 idx)
{
    Pair_4 *tab;
    File *f;
    s32 i, j;

    tab = func_800BF79C(D_800D5604_3, D_800D123C);
    f = func_800BF79C(r, idx);
    for (i = 0; i < 16; i++) {
        func_8000EF8C(D_800C9080_3[i].name, f->e[i].name);
        for (j = 0; j < 6; j++) {
            ((f32 *)&D_800C9080_3[i])[j + 4] = (f32)f->e[i].v[j] / 1000.0f;
        }
        D_800C9080_3[i].f28 = f->e[i].f28;
        D_800C9080_3[i].b.b[0] = f->e[i].b.b[0];
        D_800C9080_3[i].b.b[1] = f->e[i].b.b[1];
        D_800C9080_3[i].b.b[2] = f->e[i].b.b[2];
        D_800C9080_3[i].b.b[3] = f->e[i].b.b[3];
        D_800C9080_3[i].b.b[4] = f->e[i].b.b[4];
        D_800C9080_3[i].b.b[5] = f->e[i].b.b[5];
        D_800C9080_3[i].b.b[6] = f->e[i].b.b[6];
        D_800C9080_3[i].b.b[7] = f->e[i].b.b[7];
        D_800C9080_3[i].b.b[8] = f->e[i].b.b[8];
        D_800C9080_3[i].b.b[9] = f->e[i].b.b[9];
        D_800C9080_3[i].b.b[10] = f->e[i].b.b[10];
        D_800C9080_3[i].b.b[11] = f->e[i].b.b[11];
        for (j = 0; tab[j].key != 0; j++) {
            if (tab[j].key == f->e[i].key) {
                break;
            }
        }
        D_800C9080_3[i].f38 = tab[j].val;
    }
    func_800BFEA4(&r->arr[idx]);
    func_800BFEA4(&D_800D5604_3->arr[D_800D123C]);
}

typedef struct { u8 b[48]; } Blk48;
typedef struct { u8 b[16]; } Blk16;

typedef struct {
    u8 pad[0x50];
    s32 f50;
} Lvl;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
} OvlTab;

extern Lvl *D_800EFD64_16;
extern OvlTab D_800E20A8_2;

void func_800197B4(s32 x)
{
    Blk48 buf;
    s32 h;
    s32 cart;
    s32 sz;
    s32 q;
    u8 *p3;
    u8 *p6;
    u8 *p7;

    if (x == D_800C8D1C || (D_800EFD70 & 1)) {
        return;
    }
    p3 = D_800731D0;
    p6 = D_800A42C0;
    p7 = D_800A9C20;
    if ((u32)p3 & 0xF) func_8000ECB8(0x33);
    if ((u32)p6 & 0xF) func_8000ECB8(0x34);
    if ((u32)p7 & 0xF) func_8000ECB8(0x35);
    if (x != 0) {
        sz = (u32)&D_800A42C0[-0x30] - (u32)p3;
        func_800BCCF0(p3 + 0x30, sz, D_800D6170, 0xBDD);
        if (!(D_800EFD64_16->f50 & 2)) {
            func_800BCCF0(p6 + 0x10, (u32)&D_800A9C20[-0x10] - (u32)p6, D_800D6170, 0xBE0);
        }
        func_800AD990();
        if (D_800C2950 == 0) {
            cart = 0xB0000C00;
            while ((h = func_800BEC20(D_801009A4, 0, (s32)p3 + cart + 0x30, (s32)(p3 + 0x30), sz)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            while ((h = func_800BEC20(D_801009A4, 0, cart + (s32)p6 + 0x10, (s32)(p6 + 0x10), (q = (s32)p7 - 0x10) - (s32)p6)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
        } else {
            while (func_80065858() != 0);
            cart = D_800E20A8_2.f28 + 0x28;
            cart += D_800E20A8_2.fC;
            while ((h = func_800BEC20(D_801009A4, 0, cart, D_800C29E0, D_800E20A8_2.f14)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            D_80100660 = p3;
            D_80100658 = p6;
            D_8010065C = D_800C29E0;
            D_80100654 = D_800C29E0 + D_800E20A8_2.fC;
            buf = *(Blk48 *)p3;
            func_800BDE38();
            *(Blk48 *)p3 = buf;
            cart += D_800E20A8_2.f14;
            while ((h = func_800BEC20(D_801009A4, 0, cart, D_800C29E0, D_800E20A8_2.f1C)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            D_80100660 = p6;
            D_80100658 = p7;
            __builtin_memcpy(&buf, p6, 16);
            D_8010065C = D_800C29E0;
            D_80100654 = D_800C29E0 + D_800E20A8_2.fC;
            func_800BDE38();
            *(Blk16 *)p6 = *(Blk16 *)&buf;
        }
        func_800AD7E0(0, 0x800000);
        func_800AD890(0, 0x800000);
        D_800C8D1C = 1;
    } else {
        func_800BCB04(p3 + 0x30);
        if (!(D_800EFD64_16->f50 & 2)) {
            func_800BCB04(p6 + 0x10);
        }
        D_800C8D1C = 0;
    }
}

typedef struct {
    s32 flags;
    s32 pad[14];
    s32 f3C;
    s32 f40;
    s32 f44;
} Ctl_5;

void func_80019CCC(s32 p)
{
    if (p != 0) {
        Ctl_5 *c;
        D_800E3700 ^= 1;
        c = &D_800EFD70;
        c->f3C = 1;
        c->f44 = 0;
        func_8005EA20(D_800C8CE4, D_800E3748);
        D_800E370C = 2;
        c->f3C = 2;
        D_800E3700 ^= 1;
    } else {
        Ctl_5 *c = &D_800EFD70;
        c->f3C = 1;
        c->f44 = 0;
        D_800E370C = 1;
        D_800E3700 = 0;
        func_8005EA20(D_800C8CE4, D_800E3748);
        D_800E3700 = 1;
    }
    if (D_800C8CD8 != 0) {
        Ctl_5 *c = &D_800EFD70;
        if (c->flags & 0x80000) {
            c->f3C = 1;
            func_8005FEE0(D_800C8CE4 + 0x6400);
            c->f3C = 2;
        }
    }
}

void func_80019DCC(void)
{
    func_80015124(0);
    func_8002D8A0();
    func_800229B8();
    func_8005E960();
    func_8003F658();
    func_8003A720();
    func_8006B160();
    func_8003C2AC();
    func_8000F600();
    func_80069C6C();
    func_8003A84C();
    func_80072A9C();
    if (D_800EFDBC == 9) {
        func_8004B6CC();
    }
    if (D_800C2350 & 0x800) {
        func_8005B33C();
    }
}

void func_80019E78(void)
{
    u8 *s = &D_800F1900_4[0];
    func_80010E5C(0, s[0x2C6]);
    func_80010E5C(1, s[0x2C5]);
    func_80010E5C(2, s[0x2C7]);
}

void func_80019EC0(s32 param_1)
{
    u8 *b;
    u8 *gb;
    s32 s2;
    s32 s3;
    u8 s4;
    u8 s5;
    s32 idx;

    if (param_1 != 0) {
        gb = (u8 *)0x800E0000;
        __asm__ __volatile__("" : "+r"(gb));
        b = D_800EFD70_2;
        s2 = *(s32 *)(b + 0x1c);
        __asm__ __volatile__("" : "+r"(s2) : : "memory");
        *(s32 *)(gb + 0x3700) = -1;
        s3 = *(s8 *)(b + 0x2c8);
        s4 = *(u8 *)(b + 0x43c);
        s5 = *(u8 *)(b + 0x7aa);
        func_8005FE10(param_1);
        idx = *(s32 *)(gb + 0x3700) ^ 1;
        *(s32 *)(b + 0x1c + idx * 4) = s2;
        *(s8 *)(b + 0x2c8 + idx) = s3;
        *(u8 *)(b + 0x43c + (*(s32 *)(gb + 0x3700) ^ 1)) = s4;
        *(u8 *)(b + 0x7aa + (*(s32 *)(gb + 0x3700) ^ 1)) = s5;
        *(u8 *)(b + 0x2c8 + *(s32 *)(gb + 0x3700)) = 0;
    }
}

typedef struct {
    u32 flags;
    u8 pad0[0x3C - 4];
    s32 f3c;
    u8 pad1[0x4C - 0x40];
    s32 f4c;
} Blk70;

extern Blk70 D_800EFD70_24;

void func_80019F7C(void)
{
    if ((D_800EFD70_24.flags & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && D_800C2940 != 0 &&
        (D_800EFD70_24.f3c == 1 || D_800EFD70_24.f4c == 8) &&
        D_800EFD64_3[11] != 2 && (D_800EFD70_24.flags & 0x4040000) == 0) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();
}

void func_8001A044(void)
{
    s32 i;
    s32 *p;
    s32 off;

    D_800C2354 = func_800BC908(0x148, 0, "min.c", 0x4E3);
    i = 0;
    do {
        p = func_800C1E60(0, 0x40, 1, 2, 0x20);
        off = i * 4;
        *(s32 *)((u8 *)D_800C2354 + off) = (s32)p;
        i++;
    } while (i < 0x4A);
}

void func_8001A0D4(void)
{
    s32 i;
    void **p;

    i = 0;
    do {
        p = (void **)D_800C2354 + i;
        func_800C204C(*p);
        p = (void **)D_800C2354 + i;
        func_800BCB04(*p);
        i++;
    } while (i < 0x4a);
    func_800BCB04(D_800C2354);
}

void func_8001A14C(void)
{
    func_80010E5C(0, 0);
    func_80010E5C(1, 0);
    func_80010E5C(2, 0);
}
