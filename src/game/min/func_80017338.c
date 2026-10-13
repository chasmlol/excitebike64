#include "ctx.h"

typedef struct { u32 w[4]; } Pad16;
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
} Game;

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
} Player;

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
} Opt;

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
} Res;

typedef struct {
    u8 pad0[0x64];
    s16 h64;
} S930;

typedef struct {
    u8 pad0[0xC];
    u8 bC;
    u8 padD[0x28 - 0xD];
} Trk;

typedef struct {
    u8 pad0[0x10];
    s32 f10;
} S29CC;

typedef struct {
    s32 *a;
    s32 *b;
    s32 *w;
    s32 *h;
    s32 *c;
    void *d;
} Cfg;

extern s32 D_800C8CD0;
extern s32 D_800C29DC;
extern Game D_800EFD70;
extern Cfg64 *D_800EFD64;
extern Pad24 D_800E3770;
extern Pad24 D_800E36E0;
extern Pad24 D_800E3750;
extern Pad24 D_800E3718;
extern Player D_800E37C0[8];
extern s32 D_800F07D8[4];
extern s32 D_800E76D0;
extern s32 D_800E2038;
extern s32 D_800E3710;
extern s32 D_800C8CD8;
extern s32 D_800E3700;
extern s32 D_800C8CE8;
extern Sess D_800F05A8;
extern s32 D_800C2594;
extern s32 D_800C9550[];
extern s32 D_800C9560[];
extern s32 D_800DC0B4;
extern s32 D_800C2B58;
extern s32 D_800C29E0;
extern Gui D_800E3730;
extern Res *D_800E7728;
extern s32 D_800F47C0;
extern s32 D_800D0C24;
extern s32 D_800D108C;
extern s32 D_800D1488;
extern s32 D_800D1428;
extern s32 D_800D1424;
extern s32 D_800D1478;
extern s32 D_800D1080;
extern s32 D_800D0724;
extern s32 D_800CCCA4;
extern volatile s32 D_800CCCA8;
extern s32 D_800E20E8;
extern s32 D_800E20EC;
extern Opt D_800F1900;
extern s32 D_800C2940;
extern u32 D_800E30B0[4];
extern u32 D_800E25B0[4];
extern s32 D_800CA934;
extern u32 D_800E36F8;
extern s32 D_800E374C;
extern s32 D_800CA920;
extern s32 D_800E37AC;
extern s32 D_800E3768;
extern s32 D_800E3708;
extern s32 D_800E36FC;
extern s8 D_800F0746;
extern s8 D_800F06AC;
extern s32 D_800C9684;
extern s32 D_800C9650;
extern s32 D_800C2358;
extern s32 D_800EE140;
extern s32 D_800C2350;
extern s32 D_800E370C;
extern S930 *D_800CA930;
extern Trk D_800CADE4[];
extern S29CC *D_800C29CC;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
u32 func_80022324(u32, s32);
void func_8000D0A0(u32 a0, s32 a1);
void func_80067B54(void);
void func_80072114(void);
void func_800716C0(void);
void func_800BCB04(s32);
void func_800482D0(Res *);
void func_800BF530(char *, s32 *);
s32 func_800C1E60(s32, s32, s32, s32, s32);
s32 func_800BF79C(s32, s32);
s32 func_80065858(void);
void func_80000F0C(void);
void func_800C204C(s32);
void func_800BFDD4(s32);
void func_800BFEA4(s32 *);
void func_8004834C(void);
s32 func_800BCCF0(s32, s32, char *, s32);
void func_800BC3D0(void);
void func_80048230(Res *);
void func_800368A4(void);
void func_800283C0(void);
void func_8001102C(s32);
void func_8001618C(void);
void func_80017214(void);
void func_800169BC(void);
void func_80010E5C(s32, s32);
void func_800229B8(void);
void func_8001A190(void);
s32 func_800595A4();
s32 func_8005B55C(void);
s32 func_8005AE38(void);
void func_800B9F4C(void);
void func_8005B28C(void);
void func_80052A6C(s32);
void func_80028864(void);
void func_80015254();
void func_80022448(s32);
void func_8000C450(void);
void func_8003ED1C(void);
void func_8003F128(void);
void func_8003F204(void);
void func_8006ADF0(void);
void func_8000F7A4(void);
void func_80010F40(void);
void func_8004F534(void);
void func_8000D0D8(void);
void func_80015124(s32);
void func_8002D8A0(void);
void func_8005E960(void);
void func_8003F658(void);
void func_8003A720(void);
void func_8006B160(void);
void func_8003C2AC(void);
void func_8000F600(void);
void func_80069C6C(void);
void func_8003A84C(void);
void func_80072A9C(void);
void func_8004B6CC(void);
void func_8005B33C(void);
void func_80014E10(void);
void func_80050838(s32, s32);
void func_80015E60(void);
void func_8005B05C(void);
void func_80044174(void);
void func_80062D0C(void);

#define PAD (D_800EFD70.f2C8[D_800E37AC])
#define PRESSED(m) ((D_800E30B0[PAD] & (m)) && !(D_800E25B0[PAD] & (m)))

void func_80017338(u32 *arg)
{
    Cfg cfg;
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
    for (j = 0; j < D_800EFD70.f3C; j++) {
        buf.w[j] = arg[D_800EFD70.f2C8[j]];
    }
    func_8000ED10((u8 *)&D_800E3770, 0, 0x18);
    *(Pad16 *)&D_800E3770 = buf;
    for (i = 0; i < 4; i++) {
        D_800E3770.w[i] = func_80022324(D_800E3770.w[i], i);
    }
    for (i = 0; i < 4; i++) {
        if (D_800E3770.w[i] & 0x2000) D_800E3770.w[i] |= 0x0F000004;
        if (D_800E3770.w[i] & 0x4000) D_800E3770.w[i] |= 0x0F000008;
        if (D_800E3770.w[i] & 0x800) D_800E3770.w[i] |= 0xF0000001;
        if (D_800E3770.w[i] & 0x1000) D_800E3770.w[i] |= 0xF0000002;
        if (D_800E3770.w[i] & 4) D_800E3770.w[i] &= ~8;
        if (D_800E3770.w[i] & 1) D_800E3770.w[i] &= ~2;
        D_800E3770.w[i] &= ~0x7800;
    }
    for (i = 0; i < 4; i++) {
        t = D_800E76D0 - D_800E37C0[i].f774 + 4;
        j = 0;
        if (D_800E37C0[i].b5CD) {
            t = t * 2 / 3;
        }
        if (t < D_800E37C0[i].f768 / 20) j = 1;
        if (D_800F07D8[i] != 0 && !(D_800E2038 & 7)) j = 1;
        if (D_800E37C0[i].f4E4 != NULL && D_800E37C0[i].f4E4->f38 != 0 &&
            D_800E37C0[i].f420 > 8.0f && D_800E2038 % 6 == 0) j = 1;
        if (D_800E3710 != 0) j = 0;
        if (D_800E37C0[i].b6E8 != 0) j = 0;
        if (D_800C8CD8 != 0 && i == D_800E3700) j = 0;
        if (D_800C8CE8 != 0) j = 0;
        if (D_800EFD70.f54 != 3) j = 0;
        if (D_800EFD70.f0 & 1) j = 0;
        if (D_800EFD70.f4C == 9 && D_800F05A8.f0 == 0xE) j = 0;
        t = D_800EFD70.f2C8[i];
        if (j) func_8000D0A0(t, 1);
    }
    func_80067B54();
    func_80072114();
    func_800716C0();
    {
        s32 *tab = D_800C9550;
        s32 *tab2 = D_800C9560;
        w = D_800EFD64->f58[D_800C2594 - 1];
        h = D_800EFD64->f60[D_800C2594 - 1];
        cfg.a = &tab[D_800C2594 - 1];
        cfg.b = &tab2[D_800C2594 - 1];
    }
    cfg.w = &w;
    cfg.h = &h;
    cfg.c = &D_800DC0B4;
    cfg.d = &D_800C2B58;
    if (D_800E3710 == 2 || D_800E3710 == 3) {
        if (D_800E3730.h0 == 0 && D_800E3730.b3 == 0) {
            func_800BCB04(D_800C29E0);
            D_800E3730.b3 = 1;
            if (D_800E3710 == 2) {
                func_800482D0(D_800E7728);
            }
            func_800BF530("guires", &D_800F47C0);
            for (j = 0; j < 0x30; j++) {
                D_800EFD70.f74[j] = func_800C1E60(0, 0x40, 4, 2, 0x20);
            }
            if (D_800E3710 == 2) {
                D_800E3730.w4 = func_800BF79C((s32)D_800E7728, D_800D0C24);
            } else {
                D_800E3730.w8 = func_800BF79C((s32)D_800E7728, D_800D108C);
                D_800E3730.w4 = func_800BF79C(D_800F47C0, D_800D1488);
                D_800E3730.w10 = func_800BF79C(D_800F47C0, D_800D1428);
                D_800E3730.w14 = func_800BF79C(D_800F47C0, D_800D1424);
                D_800E3730.wC = func_800BF79C(D_800F47C0, D_800D1478);
            }
            D_800E3730.h0 = -1;
            while (func_80065858() != 0) {
            }
            D_800CCCA4 = 1;
            while (D_800CCCA8 != D_800CCCA4) {
            }
            func_80000F0C();
        }
    } else if (D_800E3710 == 1 && D_800E3730.h0 == 0 && D_800E3730.b3 != 0) {
        D_800E3730.b3 = 0;
        D_800E3730.b2 = 2;
        for (j = 0; j < 0x30; j++) {
            func_800C204C(D_800EFD70.f74[j]);
            func_800BCB04(D_800EFD70.f74[j]);
        }
        func_800BFDD4(D_800F47C0);
        func_800BFEA4(&D_800E7728->f4[D_800D0C24]);
        func_800BFEA4(&D_800E7728->f4[D_800D1080]);
        func_800BFEA4(&D_800E7728->f4[D_800D0724]);
        func_800BFEA4(&D_800E7728->f4[D_800D108C]);
        func_8004834C();
        D_800C29E0 = func_800BCCF0(D_800C29E0, D_800E20E8 * D_800E20EC * 2, "min.c", 0x799);
        if (D_800C29E0 == 0) {
            func_800BC3D0();
            while (1) {
            }
        }
        if (!(D_800EFD70.f0 & 0x81) && (D_800F1900.b2C4 & 2) && D_800C2940 != 0 &&
            (D_800EFD70.f3C == 1 || D_800EFD70.f4C == 8) && D_800EFD64->f2C != 2 &&
            !(D_800EFD70.f0 & 0x04040000)) {
            D_800CCCA4 = 2;
        } else {
            D_800CCCA4 = 1;
        }
        while (D_800CCCA8 != D_800CCCA4) {
        }
        func_80000F0C();
        func_80048230(D_800E7728);
        D_800E3730.h0 = -1;
    }
    if (D_800EFD70.f4C != 9 && !(D_800EFD70.f0 & 0x8010) && (D_800E30B0[0] & 0x400) &&
        !(D_800E25B0[0] & 0x400)) {
        D_800CA934 = (D_800CA934 + 1) & 3;
    }
    func_800368A4();
    func_800283C0();
    if (!(D_800EFD70.f0 & 0x80)) {
        if (D_800C8CE8 != 0 && D_800E36F8 == 5 &&
            ((D_800E3770.w[0] | D_800E3770.w[1] | D_800E3770.w[2] | D_800E3770.w[3]) & 0x50)) {
            D_800E36F8 = 6;
            D_800E374C = -1;
            D_800CA920 = 2;
            D_800EFD70.f0 &= ~4;
            func_8001102C(1);
            return;
        }
    } else {
        func_8001618C();
    }

    if (D_800E3710 == 2) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            func_80017214();
        }
    } else if (D_800E3710 == 3) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            func_800169BC();
        }
    } else if (D_800E3710 == 6) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            if (PRESSED(0x20)) {
                D_800E3710 = 0;
                func_80010E5C(0, D_800F1900.b2C6);
                func_80010E5C(1, D_800F1900.b2C5);
                func_80010E5C(2, D_800F1900.b2C7);
                func_8001102C(2);
            } else if (PRESSED(0x10) || PRESSED(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 0:
                    D_800E3710 = 0;
                    func_80010E5C(0, D_800F1900.b2C6);
                    func_80010E5C(1, D_800F1900.b2C5);
                    func_80010E5C(2, D_800F1900.b2C7);
                    break;
                case 1:
                    func_800229B8();
                    func_8000ED10((u8 *)D_800E37C0, 0, 0x3F00);
                    func_8001A190();
                    D_800E3710 = 0;
                    D_800F05A8.f58 |= 0x80;
                    func_80010E5C(0, D_800F1900.b2C6);
                    func_80010E5C(1, D_800F1900.b2C5);
                    func_80010E5C(2, D_800F1900.b2C7);
                    break;
                case 2:
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 8;
                    break;
                }
            } else if (PRESSED(1) || PRESSED(0x800)) {
                D_800E3768 = (D_800E3768 + 2) % 3;
                func_8001102C(0);
            } else if (PRESSED(2) || PRESSED(0x1000)) {
                D_800E3768 = (D_800E3768 + 1) % 3;
                func_8001102C(0);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730.b2 = 2;
        }
    } else if (D_800E3710 == 5) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            if (PRESSED(0x20) && D_800F0746 == 0 && D_800F06AC == 0) {
                D_800E3710 = 0;
                func_80010E5C(0, D_800F1900.b2C6);
                func_80010E5C(1, D_800F1900.b2C5);
                func_80010E5C(2, D_800F1900.b2C7);
                func_8001102C(2);
            } else if (PRESSED(0x10) || PRESSED(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 4:
                    D_800F05A8.f70 = 3;
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    break;
                case 3:
                    D_800F05A8.f70 = 4;
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    break;
                case 1:
                    D_800F05A8.f70 = 7;
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    break;
                case 0:
                    D_800F05A8.f70 = 6;
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    break;
                case 2:
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 2;
                    D_800EFD70.f0 &= ~4;
                    break;
                }
            } else if (PRESSED(1) || PRESSED(0x800)) {
                do {
                    D_800E3768 = (D_800E3768 + 4) % 5;
                } while ((D_800E3768 == 3 && func_800595A4() == 0) ||
                         (D_800E3768 == 1 && func_8005B55C() == 0) ||
                         (D_800E3768 == 4 && func_8005AE38() == 0));
                func_8001102C(0);
            } else if (PRESSED(2) || PRESSED(0x1000)) {
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
            D_800E3730.b2 = 2;
        }
    } else if (D_800E3710 == 1) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            if (PRESSED(0x20)) {
                func_80010E5C(0, D_800F1900.b2C6);
                func_80010E5C(1, D_800F1900.b2C5);
                func_80010E5C(2, D_800F1900.b2C7);
                D_800E3710 = 0;
                func_8001102C(2);
            } else if (PRESSED(0x10) || PRESSED(0x40)) {
                func_8001102C(1);
                flag = 1;
                switch (D_800E3768) {
                case 0:
                    D_800E3710 = 2;
                    D_800E3708 = 0;
                    D_800E36FC = D_800EFD70.f2CC[PAD];
                    D_800E3730.h0 = 3;
                    break;
                case 1:
                    D_800E3710 = 3;
                    D_800E3730.h0 = 3;
                    D_800E36FC = 0;
                    break;
                case 4:
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    D_800EFD70.f0 |= 4;
                    if (D_800EFD70.f4C == 1) {
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
            } else if (PRESSED(1) || PRESSED(0x800)) {
                D_800E3768 = (D_800E3768 + (D_800EFD70.f4C == 1 ? 3 : 4)) % (D_800EFD70.f4C == 1 ? 4 : 5);
                func_8001102C(0);
            } else if (PRESSED(2) || PRESSED(0x1000)) {
                D_800E3768 = (D_800E3768 + 1) % (D_800EFD70.f4C == 1 ? 4 : 5);
                func_8001102C(0);
            }
            if (D_800E3710 == 0) {
                func_800B9F4C();
                func_80010E5C(0, D_800F1900.b2C6);
                func_80010E5C(1, D_800F1900.b2C5);
                func_80010E5C(2, D_800F1900.b2C7);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730.b2 = 2;
        }
    } else if (D_800E3710 == 4) {
        if (D_800E36F8 == 5 && D_800E3730.h0 == -1) {
            if (PRESSED(0x20)) {
                D_800E3710 = 0;
                func_8001102C(2);
                func_80010E5C(0, D_800F1900.b2C6);
                func_80010E5C(1, D_800F1900.b2C5);
                func_80010E5C(2, D_800F1900.b2C7);
            } else if (PRESSED(0x10)) {
                switch (D_800E3768) {
                case 0:
                    D_800E36F8 = 6;
                    D_800E374C = -1;
                    D_800CA920 = 4;
                    D_800EFD70.f0 &= ~4;
                    func_8001102C(1);
                    if (D_800EFD70.f4C == 1) {
                        func_8005B28C();
                    }
                    break;
                case 1:
                    func_8001102C(1);
                    D_800E3710 = 1;
                    D_800E3768 = 2;
                    break;
                }
            } else if (PRESSED(1) || PRESSED(0x800)) {
                D_800E3768 = 0;
                func_8001102C(0);
            } else if (PRESSED(2) || PRESSED(0x1000)) {
                D_800E3768 = 1;
                func_8001102C(0);
            }
        }
        if (D_800E3710 == 0) {
            D_800E3730.b2 = 2;
        }
    } else {
        if (D_800F05A8.f58 & 0x800) {
            func_80052A6C(0);
            func_80010E5C(0, 0);
            if (D_800F05A8.f80 != 0) {
                if ((D_800E3770.w[0] & 0x10) && !(D_800E36E0.w[0] & 0x10)) {
                    func_8001102C(1);
                    D_800F05A8.f80 = 1;
                }
                if (--D_800F05A8.f80 == 0) {
                    D_800F05A8.f58 &= 0xFFFE77FF;
                    func_80010E5C(0, D_800F1900.b2C6);
                }
            }
            if (D_800F05A8.f58 & 0x10000) {
                D_800F05A8.f58 &= ~0x10000;
                D_800C9684 = 0;
                func_80028864();
            }
        } else {
            D_800E3750 = D_800E3770;
            func_80015254();
            if (D_800C8CE8 != 0 && D_800C9650 != -1 && !(D_800EFD70.f0 & 0x80)) {
                for (j = 0; j < 4; j++) {
                    if ((D_800E30B0[j] & 0x400) && !(D_800E25B0[j] & 0x400)) break;
                }
                if (j != 4) {
                    for (j = 1; j <= D_800F05A8.h22; j++) {
                        t = (D_800C9650 + j) % D_800F05A8.h22;
                        if (t < D_800EFD70.f3C) break;
                    }
                    D_800C9650 = t;
                }
            }
            if (D_800C2358 == 0) {
                func_80022448(1);
            } else {
                func_8000C450();
            }
            if (D_800EFD70.f0 & 0x10) {
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
                if (D_800E37C0[j].b7A5 == 0) {
                    if (D_800E37C0[j].b656 != 0 && D_800E37C0[j].f5AC >= -0xFFF) {
                        D_800E37C0[j].f5AC -= 0x200;
                    }
                    if (D_800E37C0[j].b656 == 0 && D_800E37C0[j].f5AC < 0) {
                        D_800E37C0[j].f5AC += 0x200;
                    }
                }
            }
        }
    }

    if (D_800C8CE8 == 0 && D_800F05A8.f40 == 0 && D_800E3710 == 0 && !(D_800EFD70.f0 & 0x81)) {
        for (j = 0; j < D_800F05A8.h22; j++) {
            if (((D_800E3770.w[j] & 0x40) && !(D_800E36E0.w[j] & 0x40)) ||
                (D_800EFD70.f4C == 9 && (D_800E30B0[j] & 0x40) && !(D_800E25B0[j] & 0x40))) {
                if (flag == 0 && D_800E36F8 == 5) {
                    D_800E36E0.w[j] |= 0x40;
                    if (D_800C2358 != 0) {
                        D_800E36F8 = 6;
                        D_800E374C = -1;
                        D_800CA920 = 4;
                        D_800EFD70.f0 &= ~4;
                    } else if (D_800EFD70.f4C == 13 || D_800EFD70.f4C == 14) {
                        D_800E3710 = 6;
                        D_800E3768 = 0;
                    } else if (D_800EFD70.f4C == 9) {
                        D_800E3710 = D_800E36F8;
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

    switch (D_800E36F8) {
    case 4:
        if (D_800E374C != -1) {
            if (--D_800E374C == 0) {
                D_800E36F8 = 5;
            }
        }
        break;
    case 6:
        if (D_800E374C == -3) {
            if ((D_800EFD70.f0 & 4) || (D_800EFD70.f4C == 9 && D_800CA920 == 6)) {
                D_800E36F8 = 7;
                D_800E374C = 2;
            } else {
                D_800E36F8 = 7;
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
            D_800E36F8 = 4;
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
            if (D_800EFD70.f4C == 9) func_8004B6CC();
            if (D_800C2350 & 0x800) func_8005B33C();
            if (D_800EFD70.f4C == 9) {
                switch (D_800F05A8.f70) {
                case 3:
                    D_800CA930->h64--;
                    break;
                case 4:
                    D_800CA930->h64++;
                    break;
                case 6:
                    D_800F05A8.f58 |= 0x4000;
                    break;
                case 7:
                    break;
                default:
                    D_800F05A8.f0 = 0xE;
                    D_800F05A8.h26 = 0x3F;
                    D_800F05A8.f78 = 0x3C;
                    D_800F05A8.f70 = 0;
                    D_800F05A8.f58 = (D_800F05A8.f58 & ~0x10) | 0x40;
                    break;
                }
                D_800E3710 = 0;
                D_800F05A8.f58 &= ~0x2000;
                if (D_800CADE4[D_800CA930->h64].bC & 4) {
                    D_800E370C = 2;
                } else {
                    D_800E370C = 1;
                }
            } else if (D_800EFD70.f4C == 6) {
                D_800E370C = 4;
                D_800EFD70.f3C = 1;
            } else if (D_800EFD70.f4C == 10 || D_800EFD70.f4C == 2) {
                D_800E370C = D_800EFD70.f3C;
            } else if (D_800EFD70.f4C == 0 && D_800EFD70.f3C == 2) {
                D_800E370C = D_800EFD70.f75C + 2;
            } else {
                D_800E370C = D_800EFD70.f3C + D_800EFD70.f44;
                if (D_800EFD70.f4C == 8 && !(D_800EFD70.f0 & 1)) {
                    D_800E370C = 1;
                }
            }
            func_80014E10();
            func_80050838(D_800E370C, D_800EFD70.f40);
            func_80015E60();
            if (D_800EFD70.f4C == 9) func_8005B05C();
        }
        break;
    }
    D_800E3718 = D_800E3750;
    D_800E36E0 = D_800E3770;
    if (D_800DC0B4 <= 0) {
        D_800DC0B4 = 1;
    }
    if (D_800DC0B4 >= 9) {
        D_800DC0B4 = 8;
    }
    if (D_800EFD64->f2C == 2) {
        func_80044174();
    }
    func_80062D0C();
}
