#include "game.h"

#define gDPRaw(pkt, a, b) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = (a); \
    _g->words.w1 = (b); \
}

typedef struct {
    u8 *f0;
    u8 p04[0x60];
    s16 f64;
} Prof;

typedef struct { s32 a, b; s16 f8; u8 pad[0x7E0-0xA]; } BikeX;

typedef struct {
    s32 w0;
    u8 pad4[0x22];
    s16 h26;
} Mode;

extern Prof *D_800CA930_5;
extern BikeX D_800E3E20[];
extern Mode D_800F05A8_5;

#define CUR (D_800CADE4_8[*(s16 *)(D_800CA930 + 0x64)])

typedef struct {
    s32 f00;
    u8 p04[0x22];
    s16 f26;
    u8 p28[4];
    s32 f2c;
    u8 p30[0x28];
    u32 f58;
    u8 p5c[0x14];
    s32 f70;
    s32 f74;
    s32 f78;
    void *f7c;
    s32 f80;
} Blk_13;

typedef struct {
    u8 p0[0xC];
    u8 fl;
    u8 p1[7];
    s32 w14;
    u8 p2[0x28 - 0x18];
} Ent_54;

extern Blk_13 D_800F05A8_18;
extern Ent_54 D_800CADE4_8[];

void func_8004F9B0(s32 a0)
{
    Blk_13 *s;

    func_80064DB8();
    s = &D_800F05A8_18;
    if (s->f7c == 0) {
        s->f7c = func_800BC908(0x80, 0, (s32)D_800D7340, 0x16D);
    }
    D_800F0180 = 1;
    if (CUR.fl & 4) {
        s->f2c = 0;
        s->f58 |= 0x1000;
    } else {
        s->f58 &= ~0x1000;
    }
    if (CUR.fl & 8) {
        D_800F05A8_18.f26 = 0x3F;
        D_800F05A8_18.f00 = 1;
        D_800F05A8_18.f58 |= 0x10;
    } else {
        D_800F05A8_18.f58 &= ~0x10;
    }
    D_800F05A8_18.f58 &= ~0x4000;
    if (CUR.fl & 8) {
        D_800F05A8_18.f58 |= 0x18800;
        D_800F05A8_18.f80 = 600;
        if (CUR.fl & 2) {
            D_800F05A8_18.f2c = 0;
        } else if (D_800C8CE4 == 0) {
            D_800F05A8_18.f2c = CUR.w14;
        }
    } else {
        D_800F05A8_18.f00 = 0xE;
        D_800F05A8_18.f2c = 0;
    }
    if (D_800C8CE4 == 0) {
        D_800F05A8_18.f70 = 5;
        D_800F05A8_18.f58 &= ~0x40;
    } else {
        D_800F05A8_18.f26 = 0x3F;
        D_800F05A8_18.f70 = 0;
        D_800F05A8_18.f78 = 0x3C;
        D_800F05A8_18.f58 |= 0x40;
    }
    D_800F0738 = 0;
    D_800F06AC = 0;
    D_800F0746 = 0;
}

void func_8004FBE8(void)
{
    if (D_800E36F8 == 5) {
        if (((D_800E30B0 & 0x200) != 0) && ((D_800E25B0 & 0x200) == 0)) {
            if (*(s16 *)(D_800CA930 + 0x64) < 1) {
                return;
            }
            D_800F0618 = 3;
            D_800E36F8 = 6;
            D_800E374C = -1;
            func_8001102C(1);
            return;
        }
        if ((D_800E30B0 & 0x400) == 0) {
            return;
        }
        if ((D_800E25B0 & 0x400) != 0) {
            return;
        }
        if (func_800595A4() == 0) {
            return;
        }
        D_800F0618 = 4;
        D_800E36F8 = 6;
        D_800E374C = -1;
        func_8001102C(1);
    }
}

#define G_6 D_800F05A8_19
#define TB (D_800CADE4_9[D_800CA930_5->f64])
#define SEG(i) ((Seg_13 *)((s32)((i) * 0x6C) + (s32)D_800EE498))

typedef struct {
    u32 state;      /* 0x00 */
    u8 p04[0x22];
    s16 f26;        /* 0x26 */
    s16 f28;        /* 0x28 */
    u8 p2A[2];
    s32 f2c;        /* 0x2C */
    u8 p30[0x10];
    s32 f40;        /* 0x40 */
    u8 p44[0x14];
    u32 f58;        /* 0x58 */
    u8 p5C[0x14];
    s32 f70;        /* 0x70 */
    s32 f74;
    s32 f78;        /* 0x78 */
    s32 f7c;
    s32 f80;        /* 0x80 */
} Race_2;

typedef struct {
    s32 v;
    u8 pad[0xC];
} Gate;

typedef struct {
    u8 p00[0xC];
    u8 fl;          /* 0x0C */
    u8 fD;          /* 0x0D */
    u8 p0E[0xA];
    s16 f18;        /* 0x18 */
    s16 f1A;        /* 0x1A */
    u8 p1C[4];
    s32 n;          /* 0x20 */
    Gate *list;     /* 0x24 */
} Track_11;

typedef struct {
    u8 p000[0x3D2];
    s8 f3D2;
    u8 p3D3[0x28];
    u8 f3FB;
    u8 p3FC[0x22C];
    s16 f628;
    u8 p62A[0x66];
    u32 f690;
    u8 p694[0x14C];
} Bike_29;

typedef struct {
    u8 p00[0x4A];
    s16 f4A;
    u8 p4C[0x20];
} Seg_13;

extern Race_2 D_800F05A8_19;
extern Track_11 D_800CADE4_9[];
extern Bike_29 D_800E37C0_54[];

void func_8004FCBC(void)
{
    s32 i;

    D_800E43EC = 1.0375f;
    if (!(G_6.f58 & 0x800)) {
        if ((TB.fl & 8) && (G_6.state < 5 || G_6.state == 15 || G_6.state == 14)) {
            func_800515A8();
        }
        if (G_6.f26 == 0x2B && G_6.state == 1 && (G_6.f58 & 0x8000)) {
            G_6.f80 = 300;
            G_6.f58 |= 0x10800;
        }
    } else {
        func_80010F40();
    }
    if (G_6.state == 5) {
        G_6.f58 &= ~0x10;
        if (D_800C8CE4 != 0 && D_800C238C_2 != 0xFF) {
            G_6.f70 = 1;
            G_6.state = 14;
        } else {
            G_6.state = 15;
            G_6.f70 = 5;
        }
    }
    if (G_6.state >= 5 && !(G_6.f58 & 0x800)) {
        if (D_800C8CE4 == 0) {
            if (TB.fl & 2) {
                G_6.f2c++;
            } else if (!(D_800E3E50 & 0x10)) {
                G_6.f2c--;
            }
        } else {
            for (i = 0; i < TB.n; i++) {
                if (G_6.f2c == TB.list[i].v) {
                    func_80010F40();
                    func_8000D0D8();
                    G_6.f80 = 1800;
                    G_6.f58 |= 0x800;
                    break;
                }
            }
            G_6.f2c++;
        }
        if (G_6.f40 == 0) {
            G_6.f28 = 180;
        } else if (G_6.f28 != 0) {
            G_6.f28--;
        }
    }
    if ((G_6.f70 == 0 || G_6.f70 == 2) && G_6.f78 != 0) {
        G_6.f78--;
    }
    if (G_6.state < 5) {
        func_8004FBE8();
        return;
    }
    switch (G_6.state) {
    case 13:
        switch (G_6.f70) {
        case 0:
            if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
                G_6.f70 = 2;
                G_6.f78 = 59 - G_6.f78;
            } else if (G_6.f78 == 0) {
                G_6.f70 = 1;
            }
            break;
        case 1:
            if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
                G_6.f70 = 2;
                G_6.f78 = 60;
                G_6.f58 = (G_6.f58 & ~0x10) | 0x40;
            } else {
                func_8004FBE8();
            }
            break;
        case 2:
            if (G_6.f78 == 0) {
                G_6.state = 14;
                G_6.f26 = 0x3F;
                G_6.f70 = 0;
                G_6.f78 = 60;
            }
            break;
        }
        break;
    case 14:
        switch (G_6.f70) {
        case 0:
            if (G_6.f78 == 0) {
                G_6.f70 = 1;
                return;
            }
            break;
        case 1:
            if ((D_800E30B0 & 0x40) && !(D_800E25B0 & 0x40)) {
                G_6.f70 = 6;
                D_800E36F8 = 6;
                D_800E374C = -1;
                D_800C8CE4 = 0;
                D_800C8CDC = 0;
                D_800E37A8 = 0;
                D_800C8CF0 = 0;
                return;
            }
            if (D_800E37C0_54[0].f628 == TB.f1A) {
                if (D_800E37C0_54[0].f690 & 0x10) {
                    if (G_6.f58 & 0x2000) {
                        func_800229B8();
                        func_8005E9BC();
                        func_8000ED10((u8 *)D_800E37C0_54, 0, sizeof(Bike_29) * 8);
                        func_8004F9B0(0);
                        func_8001A190();
                        func_8005FE10(D_800C8CE4);
                        G_6.f58 = (G_6.f58 | 0x80) & ~0x2000;
                    }
                } else {
                    D_800E37C0_54[0].f690 |= 0x10;
                    G_6.f58 &= ~0x2000;
                }
                return;
            }
            if ((TB.fl & 4) && D_800E37C0_54[1].f628 >= TB.f1A &&
                (TB.f18 < TB.f1A || (D_800E37C0_54[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
                D_800E37C0_54[1].f690 |= 0x10;
                return;
            }
            break;
        case 5:
            if (D_800E37C0_54[0].f628 == TB.f1A) {
                D_800E37C0_54[0].f690 |= 0x10;
            }
            if ((G_6.f58 & 0x2000) && D_800E3710 != 5) {
                D_800E3768 = 0;
                D_800E3710 = 5;
                D_800F0746 = 1;
            }
            break;
        }
        func_8004FBE8();
        break;
    case 15:
        if (TB.fl & 2) {
            if (G_6.f2c >= 3 && D_800F0738 == 0) {
                D_800F0738 = 2;
            }
            if (D_800E37C0_54[0].f3FB == 0 && D_800E37C0_54[0].f3D2 == TB.list[D_800F0738].v) {
                D_800F0738++;
            }
            if (D_800F0738 >= 10) {
                if (D_800F0600 & 0x2000) {
                    D_800F06AC = 1;
                    if (((D_800E30B0 & 0x40) && !(D_800E25B0 & 0x40)) ||
                        ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10))) {
                        if (D_800F0738 == 10) {
                            D_800F0738 = 11;
                        } else if (D_800F0738 == 11) {
                            D_800CA930_5->f0[1] = TB.fD + 1;
                            D_800F1BC1 = D_800CAAA1;
                            func_8005AFFC();
                            D_800CA920 = 2;
                            D_800E3710 = 0;
                            D_800EFD70 &= ~4;
                            return;
                        }
                    }
                    if (D_800F0738 < 10) {
                        D_800F0738 = 10;
                    }
                } else if (!(D_800E37C0_54[0].f690 & 0x10)) {
                    D_800E37C0_54[0].f690 |= 0x10;
                }
            }
        }
        func_8004FBE8();
        if (D_800E3DE8 == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            if (func_8005AFFC() != 0) {
                D_800F0618 = 6;
                D_800C8CE4 = 0;
                D_800C8CDC = 0;
                D_800E37A8 = 0;
                D_800C8CF0 = 0;
            }
            return;
        }
        if ((u32)(D_800E37C0_54[0].f628 - TB.f1A) < 10 || (G_6.f58 & 0x2000)) {
            if (D_800E37C0_54[0].f690 & 0x10) {
                if (D_800CA930_5->f64 < 16 && TB.fD != D_800CADE4_9[D_800CA930_5->f64 + 1].fD &&
                    func_8005AF90_2()) {
                    if (TB.fD == D_800CA930_5->f0[1]) {
                        D_800CA930_5->f0[1] = TB.fD + 1;
                        D_800F1BC1 = D_800CAAA1;
                    }
                }
                if ((G_6.f58 & 0x2000) && D_800F0738 == 0) {
                    func_80010F40();
                    G_6.f58 &= ~0x2000;
                    D_800F06AC = func_8005AF90_2() ? 1 : -1;
                    D_800E3710 = 5;
                    D_800E3768 = func_8005AF90_2() ? 3 : 0;
                }
            } else {
                D_800E37C0_54[0].f690 |= 0x10;
            }
        } else if (D_800E37C0_54[0].f628 == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            G_6.f70 = 6;
            D_800E36F8 = 6;
            D_800E374C = -1;
            D_800C8CE4 = 0;
            D_800C8CDC = 0;
            D_800E37A8 = 0;
            D_800C8CF0 = 0;
        } else if ((TB.fl & 4) && D_800E37C0_54[1].f628 >= TB.f1A &&
                   (TB.f18 < TB.f1A || (D_800E37C0_54[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
            D_800E37C0_54[1].f690 |= 0x10;
        }
        break;
    }
}

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
} Game_19;

typedef struct {
    s32 flags;      /* 0x00 */
    s32 pad4[14];
    s32 f3C;        /* 0x3C */
    s32 pad40[3];
    s32 mode;       /* 0x4C */
} Cfg_8;

typedef struct {
    u8 pad[0x34];
    V7 v;           /* 0x34 */
    s32 f50;        /* 0x50 */
    u8 pad54[0xA0 - 0x54];
} Track_12;

typedef struct {
    Ent24 *p;
    u8 pad4[0x62];
    u8 b66[1];
} Race_3;

typedef struct {
    u8 pad[0x660];
    u8 pad660[8];
    s16 f668;
    u8 pad66A[0x69C - 0x66A];
    s32 f69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Bike_30;

typedef struct { s32 n; s32 f4; s32 *p; } Alloc;

extern Game_19 D_800F05A8_20;
extern Cfg_8 D_800EFD70_36;
extern Track_12 *D_800EFD64_28;
extern Race_3 *D_800CA930_9;
extern Track_12 D_800CB210_10[];
extern Alloc D_800CA924_2;
extern Bike_30 D_800E37C0_55[];

void func_80050838(s32 a0, s32 a1)
{
    Bike_30 *b;
    s32 i;
    s32 j;

    D_800F0520 = -1;
    func_8000ED10_2(D_800F05A8_20.f30, -1, 12);
    if (D_800EFD70_36.mode == 1) {
        for (i = 0; i < D_800F05A8_20.f22; i++) {
            D_800CA930_9->b66[i] = 0;
        }
    }
    D_800CA920 = 6;
    *(V7 *)((u8 *)&D_800F05A8_20 + 4) = D_800EFD64_28->v;
    D_800EFD64_28->f50 &= ~0x180;
    D_800F05A8_20.f88 = -1;
    D_800F05A8_20.f58 = 0;
    D_800F05A8_20.f40 = 0;
    D_800F05A8_20.f28 = 0;
    D_800F05A8_20.f44 = 0;
    D_800F05A8_20.f4C = 0;
    D_800F05A8_20.f22 = a0;
    D_800F05A8_20.f20 = a1;
    if (D_800EFD70_36.mode == 1 && !(D_800EFD70_36.flags & 0x81) &&
        D_800CA930_9->p - D_800CA9C8 == 4) {
        D_800F05A8_20.f58 = 0x200;
        D_800F05A8_20.f22 = 2;
        D_800E370C = 2;
    }
    D_800F05A8_20.f26 = 0x96;
    if (D_800CCD64 != 0 || (D_800EFD70_36.flags & 0x81) ||
        D_800EFD70_36.mode == 0xD || D_800EFD70_36.mode == 0xE) {
        D_800F05A8_20.state = 5;
    } else {
        if ((D_800EFD70_36.mode == 0 || D_800EFD70_36.mode == 1) &&
            D_800EFD64_28 - D_800CB210_10 != 9) {
            D_800F05A8_20.state = 0;
            D_800F05A8_20.f26 = 0x100;
        } else {
            D_800F05A8_20.state = 1;
            D_800F05A8_20.f26 = 0x96;
        }
        D_800F05A8_20.f58 |= 0x10;
    }
    D_800F05A8_20.f2C = 0;
    D_800F05A8_20.f3C = (D_800EFD70_36.flags & 0x81) != 0;
    func_8000ED10_2(D_800E37C0_55, 0, 0x3F00);
    D_800EFD70_36.flags &= ~4;
    if (D_800CA924_2.n > 0 && D_800CA924_2.f4 == 0) {
        D_800CA924_2.p = func_800BD730(D_800CA924_2.n * 4, 0, (s32)D_800D7340, 0x6D9);
    }
    switch (D_800EFD70_36.mode) {
    case 2:
        D_800F05A8_20.f26 = 0xB4;
        D_800F05A8_20.state = 6;
        D_800F05A8_20.f6E = 0;
        D_800F05A8_20.f68 = 0;
        D_800F05A8_20.f6A = 0;
        D_800F05A8_20.f6C = 0;
        D_800F05A8_20.f58 |= 0x10;
        break;
    case 5:
        D_800F05A8_20.f58 |= 0x10;
        func_80064DB8();
        break;
    case 7:
        D_800F05A8_20.f48 = -1;
        D_800F05A8_20.f58 |= 0x10;
        func_80064DB8();
        for (b = D_800E37C0_55; b < &D_800E37C0_55[D_800EFD70_36.f3C]; b++) {
            b->f69C = 0x1C20;
        }
        break;
    case 9:
        func_8004F9B0(1);
        break;
    case 10:
        D_800F05A8_20.f2C = 0x1C20;
        D_800F05A8_20.f58 |= 0x10;
        func_80064DB8();
        break;
    case 0xD:
    case 0xE:
        break;
    default:
        if (D_800F05A8_20.state == 8) {
            func_80064D28(2, 0);
        }
        D_800F05A8_20.f24 = 0;
        for (j = 0; j < D_800F05A8_20.f22; j++) {
            D_800E3E20[j].f8 = -1;
        }
        break;
    }
}

#define S (&D_800F05A8_21)

typedef struct { s32 f0; s32 arr[4]; u8 b[4][3]; u8 pad20[0xC]; s32 val; } SS;

extern SS D_800F05A8_21;

s32 func_80050CF4(s32 *p)
{
    u8 ret = 0;
    s32 i;
    s32 j;
    s32 *ch;
    u8 *src;

    if (((u8 *)p)[0x6E8] != 0) goto done;
    if (D_800C8CE8 != 0) goto done;
    if (D_800EFDBC == 1 && D_800CA930_4 != 0 && ((*D_800CA930_4 - (s32)D_800CA9C8) * 0x38E38E39 >> 2) == 5) goto done;
    ch = D_800EFD64_3;
    if (ch[0x14] & 2) goto done;
    if (S->val >= S->arr[2]) goto done;
    if (D_800EFD70_4[19] == 8 && p != D_800E37C0_56 + D_800C8CE0 * 0x1f8) goto done;
    if (D_800EFD70_4[16] != *(s16 *)((u8 *)ch + 0x1E)) goto done;

    ret = S->val < S->arr[0];
    for (i = 0; i < 3; i++) {
        if (S->val <= S->arr[i]) {
            for (j = 1; j >= i; j--) {
                S->arr[j + 1] = S->arr[j];
                S->b[j + 1][0] = S->b[j][0];
                S->b[j + 1][1] = S->b[j][1];
                S->b[j + 1][2] = S->b[j][2];
            }
            break;
        }
    }
    S->arr[i] = S->val;
    S->b[i][0] = 0xFF;
    S->b[i][1] = 0xFF;
    S->b[i][2] = (D_800EFDBC == 8) ? 0 : (u8)(((u32)((s32)p - (s32)D_800E37C0_56) * 0xBEFBEFBFu) >> 5);
    D_800EFD64_3[0x14] |= 0x100;
    if (ret) func_8004EE2C((((s32)p - (s32)D_800E37C0_56) * -0x41041041) >> 5);
done:
    return ret;
}

u32 func_80050F7C(s32 p0)
{
    s32 p = p0;
    u8 *s0;
    u8 *s2;
    u8 *v;
    u8 *q;
    u8 *tb;
    s32 a0;
    s32 a1;
    s32 rv;
    s32 a2;
    s32 ti;
    u32 t1;
    u16 hv;
    f32 fv;
    u8 *w;
    u8 *w2;

    a1 = ((p << 6) - p) << 5;
    s0 = D_800E3E20_2 + a1;
    rv = *(s16 *)(s0 + 8);
    if (rv >= 0) {
        return rv;
    }
    v = D_800F05A8_2;
    *(u8 *)(v + *(s16 *)(v + 0x24) + 0x50) = p;
    a0 = *(u16 *)(v + 0x24);
    *(u16 *)(v + 0x24) = a0 + 1;
    *(u16 *)(s0 + 8) = a0;
    *(s32 *)(s0 + 0x2C) = *(s32 *)(v + 0x2C);
    if (*(u8 *)(D_800E3E20_2 + a1 + 0x88) == 0 && D_800C8CE8 == 0) {
        if ((D_800EFDBC != 8 || p == D_800C8CE0) && D_800E1898 == 1) {
            func_8001106C();
        }
    }
    s2 = D_800E37C0 + ((((p << 6) - p)) << 5);
    t1 = func_80050CF4(s2);
    if ((*(s32 *)D_800EFD70_2 & 0x8000) == 0) {
        a2 = *(s32 *)(D_800EFD70_2 + 0x4C);
        if (a2 == 1) {
            { s32 ix = (p * 7 + *(s16 *)(D_800CA930 + 0x64)) * 2; *(u16 *)(D_800CA930 + ix + 0x10) = *(u16 *)(s0 + 8); }
            ti = *(s16 *)(s0 + 8);
            ti = (s16)ti;
            tb = (u8 *)(D_800CA938 + ti * 4);
            {
                u8 *pp = D_800CA930 + p;
                pp[0x66] = tb[3];
            }
            if (s2[0x6E8] != 0) {
                goto L1fc;
            }
            a1 = D_800F05CC;
            if (a1 != a2) {
                goto L19c;
            }
            q = *(u8 **)D_800CA930 + (*(s32 *)(D_800EFD70_2 + 0x410) - 1);
            q[4] = q[4] | (a1 << *(s16 *)(D_800CA930 + 0x64));
        }
    }
L19c:
    {
        u8 *bse = D_800E37C0;
        w = bse + (((p << 6) - p) << 5);
    }
    if (w[0x6E8] == 0) {
        u8 *vb = D_800F05A8_2;
        if (*(s16 *)(vb + 0x24) == 1) {
            *(s32 *)(vb + 0x44) = p;
        }
        do { do { do {
        *(u32 *)(s0 + 0x30) = *(u32 *)(s0 + 0x30) | 8;
        fv = *(f32 *)(w + 0x638);
        hv = *(u16 *)(w + 0x628);
        } while (0); } while (0); } while (0);
        w[0x6E8] = 1;
        *(f32 *)(w + 0x42C) = fv;
        *(u16 *)(w + 0x734) = hv;
        *(u16 *)(w + 0x714) = hv;
    }
L1fc:
    {
        u8 *bse2 = D_800E37C0;
        w2 = bse2 + (((p << 6) - p) << 5);
    }
    w2[0x3FF] = 1;
    *(f32 *)(w2 + 0x42C) = *(f32 *)(w2 + 0x638);
    return t1 & 0xff;
}

typedef struct Rec_31 {
    s16 cnt;          /* 0x660 */
    s16 f662;         /* 0x662 */
    u8 pad664[4];
    s16 f668;         /* 0x668 */
    s16 f66A;         /* 0x66A */
    s16 f66C;         /* 0x66C */
    u8 pad66E[2];
    s32 f670;         /* 0x670 */
    s32 times[29];    /* 0x674 */
    u8 f6E8;          /* 0x6E8 */
    u8 pad6E9[4];
    s8 f6ED;          /* 0x6ED */
    u8 pad6EE[0x180 - 0x8E];
} Rec_31;

typedef struct Bike_31 {
    u8 pad000[0x660];
    Rec_31 r;
} Bike_31;

typedef struct Blk_14 {
    u8 pad00[0x10];
    s32 best;         /* 0x10 */
    u8 pad14[0x1D - 0x14];
    u8 f1D;
    u8 f1E;
    u8 f1F;
    s16 nlaps;        /* 0x20 */
    s16 f22;          /* 0x22 */
    u8 pad24[0x2C - 0x24];
    s32 f2C;          /* 0x2C */
    s16 arr[1];       /* 0x30 */
} Blk_14;

typedef struct Ctl_11 {
    u32 flags;
    u8 pad04[0x3C - 4];
    s32 n;            /* 0x3C */
} Ctl_11;

typedef struct G64 {
    u8 pad[0x50];
    u32 f50;
} G64;

extern G64 *D_800EFD64_29;
extern Ctl_11 D_800EFD70_37;
extern Bike_31 D_800E37C0_57[];
extern Blk_14 D_800F05A8_22;

void func_800511B8(s32 p)
{
    s32 st = 0;
    Rec_31 *b;
    s32 t;
    s32 i;
    s16 v;

    switch (D_800EFDBC) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 9:
    case 13:
    case 14:
        return;
    }
    b = &D_800E37C0_57[p].r;
    if (b->cnt < D_800F05A8_22.nlaps) {
        if (D_800E37C0_57[p].r.f6E8 == 0 && D_800C8CE8 == 0 && D_800EFDBC != 7 && D_800EFDBC != 5 &&
            !(D_800EFD64_29->f50 & 2) && (t = b->times[b->cnt]) < D_800F05A8_22.best &&
            (D_800EFDBC != 8 || p == D_800C8CE0)) {
            D_800F05A8_22.best = t;
            D_800F05A8_22.f1D = 0xFF;
            D_800F05A8_22.f1E = 0xFF;
            D_800F05A8_22.f1F = (D_800EFDBC == 8) ? 0 : p;
            st = 3;
            D_800EFD64_29->f50 |= 0x80;
        }
        if (D_800EFD70_37.flags & 0x81) {
            goto end;
        }
        v = D_800F05A8_22.arr[b->cnt];
        if (v == -1) {
            D_800F05A8_22.arr[b->cnt] = ((u16 *)&D_800F05A8_22.f2C)[1];
        } else {
            b->f670 = v - D_800F05A8_22.f2C;
            b->f66C = 0x78;
            if (b->f662 == 1) {
                for (i = 0; i < D_800EFD70_37.n; i++) {
                    if (b->cnt < D_800E37C0_57[i].r.cnt + D_800E37C0_57[i].r.f6ED) {
                        D_800E37C0_57[i].r.f66C = 0x78;
                        D_800E37C0_57[i].r.f670 = -b->f670;
                        break;
                    }
                }
            }
        }
        b->f66A = 0x78;
        ((volatile Rec_31 *)b)->cnt++;
        if (b->cnt != D_800F05A8_22.nlaps) {
            if (st != 3) {
                if (D_800EFDBC == 8 && D_800C238C[p] == 0xFF) {
                    st = 1;
                } else if (b->f662 == 0) {
                    st = 2;
                } else if (D_800E37C0_57[p].r.f6E8 == 0 && b->f670 < -300) {
                    st = 1;
                }
            }
        } else if ((u8)func_80050F7C(p)) {
            if (st != 3) {
                if (b->f668 == 0 && D_800F05A8_22.f22 >= 2 && st == 0) {
                    st = 4;
                } else if (D_800E37C0_57[p].r.f6E8 == 0) {
                    st = 5;
                }
            }
        } else if (p < D_800EFDAC) {
            st = 6;
        }
    }
end:
    if (D_800C8CD8 != 0 && D_800E3700 == p) {
        st = 0;
    }
    switch (st) {
    case 1:
        if (D_800C8CD8 == 0 || p != D_800E3700) {
            func_8004CD78(b->cnt - 1);
        }
        break;
    case 2:
        func_8004CE80(p, b->cnt - 1);
        break;
    case 3:
        func_8004ECB4(p);
        break;
    case 4:
        func_8004D7DC(p);
        break;
    case 5:
        func_8004E960(p);
        break;
    case 6:
        break;
    }
}

typedef struct {
    s32 state;
    u8 pad4[0x26 - 4];
    s16 timer;
    u8 pad28[0x58 - 0x28];
    s32 flags;
} Ctl_12;

extern Ctl_12 D_800F05A8_23;

void func_800515A8(void)
{
    if (D_800F05A8_23.timer != 0 && --D_800F05A8_23.timer == 0) {
        switch (D_800F05A8_23.state) {
        case 0:
            D_800F05A8_23.timer = 150;
            D_800F05A8_23.state = 1;
            break;
        case 1:
            D_800F05A8_23.timer = 36;
            D_800F05A8_23.state = 2;
            func_8004F270(0x6F, 5, 1);
            break;
        case 2:
            D_800F05A8_23.timer = 36;
            D_800F05A8_23.state = 3;
            func_8004F270(0x6D, 6, 1);
            break;
        case 3:
            D_800F05A8_23.timer = 36;
            D_800F05A8_23.state = 4;
            func_8004F270(0x6E, 7, 1);
            break;
        case 4:
            D_800F05A8_23.timer = 15;
            D_800F05A8_23.state = 5;
            D_800F05A8_23.flags &= ~0x10;
            func_8004F270(0x70, 8, 1);
            break;
        }
    }
}

typedef struct {
    u8 pad0[0x22];
    s16 count;
    u8 pad24[0x2C - 0x24];
    s32 tick;
    u8 pad30[0x40 - 0x30];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 flags;
} Game_20;

typedef struct {
    u8 pad0[0x3FF];
    u8 done;
    u8 pad400[0x668 - 0x400];
    s16 f668;
    s16 f66A;
    u8 pad66C[0x69C - 0x66C];
    s32 f69C;
    u8 pad6A0[0x6E8 - 0x6A0];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Rider_3;

extern Game_20 D_800F05A8_24;
extern Rider_3 D_800E37C0_58[];

void func_800516DC(void)
{
    Game_20 *g;
    Game_20 *g2;
    Rider_3 *r;
    u8 flag;

    func_800515A8();
    g = &D_800F05A8_24;
    if (!(g->flags & 0x10) && g->f40 == 0) {
        g->tick++;
    }
    flag = 1;
    for (r = D_800E37C0_58; r < &D_800E37C0_58[D_800F05A8_24.count]; r++) {
        if (r->f66A != 0) {
            r->f66A--;
            r->f69C += 30;
        }
        if (D_800F05A8_24.tick == r->f69C && r->done == 0) {
            r->done = 1;
            r->f668 = 5;
            r->f6E8 = 1;
        }
        if (r->done == 0) flag = 0;
    }
    D_800F05E8 = flag;
    if (flag) {
        if (((D_800E3770_2 & 0x40) && !(D_800E36E0 & 0x40)) ||
            ((D_800E3770_2 & 0x10) && !(D_800E36E0 & 0x10))) {
            func_8005AFFC();
        }
    }
}

typedef struct {
    s32 pad0;
    s32 key[3];
    u8 pad10[4];
    u8 tri[3][3];
    u8 pad1D[0x2C - 0x1D];
    s32 timer;
    u8 pad30[0x10];
    s32 done;
    u8 pad44[0x14];
    s32 flags;
} Rank;

typedef struct {
    u8 pad0[0x3C8];
    s32 key;
    u8 pad3CC[0x3FF - 0x3CC];
    u8 f3FF;
    u8 pad400[0x668 - 0x400];
    s16 f668;
    u8 pad66A[0x6E8 - 0x66A];
    u8 finished;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_12;

typedef struct {
    u8 pad0[0x3C];
    s32 n;
    u8 pad40[0xC];
    s32 mode;
} Game_21;

typedef struct {
    u8 pad[0x50];
    s32 flags;
} Hud;

extern Game_21 D_800EFD70_38;
extern Hud *D_800EFD64_30;
extern Rank D_800F05A8_25;
extern Player_12 D_800E37C0_59[];

void func_8005183C(void)
{
    Rank *q;
    Game_21 *g;
    Player_12 *e;
    s32 i, j, k;

    func_800515A8();
    if (D_800F05A8_25.flags & 0x10) return;
    if (--D_800F05A8_25.timer > 0) return;
    if (((D_800E3770_2 & 0x40) && !(D_800E36E0 & 0x40)) ||
        ((D_800E3770_2 & 0x10) && !(D_800E36E0 & 0x10))) {
        if (D_800E36F8 == 5) {
            D_800E36F8 = 6;
            D_800E374C = 0x14;
        }
    }
    D_800F05A8_25.timer = 0;
    if (D_800F05A8_25.done) return;
    for (i = 0; i < D_800EFD70_38.n; i++) {
        e = &D_800E37C0_59[i];
        if (e->finished) continue;
        if (D_800C8CE8) continue;
        if (D_800EFD70_38.mode == 1 && D_800CA930_10 != NULL && *D_800CA930_10 - D_800CA9C8 == 5) continue;
        if (e->key > D_800F05A8_25.key[2]) {
            for (j = 0; j < 3; j++) {
                if (e->key >= D_800F05A8_25.key[j]) {
                    for (k = 1; k >= j; k--) {
                        D_800F05A8_25.key[k + 1] = D_800F05A8_25.key[k];
                        D_800F05A8_25.tri[k + 1][0] = D_800F05A8_25.tri[k][0];
                        D_800F05A8_25.tri[k + 1][1] = D_800F05A8_25.tri[k][1];
                        D_800F05A8_25.tri[k + 1][2] = D_800F05A8_25.tri[k][2];
                    }
                    break;
                }
            }
            D_800F05A8_25.key[j] = e->key;
            D_800F05A8_25.tri[j][2] = e - D_800E37C0_59;
            D_800F05A8_25.tri[j][0] = 0xFF;
            D_800F05A8_25.tri[j][1] = 0xFF;
            D_800EFD64_30->flags |= 0x100;
        }
    }
    D_800F05A8_25.done = 1;
    for (j = 0; j < D_800EFD70_38.n; j++) {
        D_800E37C0_59[j].finished = 1;
    }
    D_800E37C0_59[0].f3FF = 1;
    *(volatile s16 *)&D_800E37C0_59[0].f668 = 1;
    if (D_800EFDAC == 2) {
        if (D_800E37C0_59[0].key > D_800E37C0_59[1].key) {
            D_800E37C0_59[0].f668 = 0;
            D_800E37C0_59[1].f668 = 5;
        } else if (D_800E37C0_59[0].key == D_800E37C0_59[1].key) {
            D_800E37C0_59[0].f668 = 1;
            D_800E37C0_59[1].f668 = 1;
        } else {
            D_800E37C0_59[0].f668 = 5;
            D_800E37C0_59[1].f668 = 0;
        }
        D_800E439F = 1;
    }
}

#define PRESSED(i, b) ((D_800E3770[i] & (b)) && !(D_800E36E0_3[i] & (b)))

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
} Game_22;

typedef struct {
    u8 p00[0x3C];
    s32 nplayers;
} Ctl_13;

typedef struct {
    u8 b[0x7E0];
} Ent_55;

extern Game_22 D_800F05A8_26;
extern Ctl_13 D_800EFD70_39;
extern Ent_55 D_800E37C0_60[];

void func_80051B6C(void)
{
    s32 i;
    s32 j;

    if (D_800F05A8_26.f40 == 0) {
        D_800F05A8_26.t28 = 180;
    } else if (D_800F05A8_26.t28 != 0) {
        D_800F05A8_26.t28--;
    }
    if (D_800F05A8_26.t6C != 0) {
        if (--D_800F05A8_26.t6C == 0) {
            for (i = 0; i < D_800EFD70_39.nplayers; i++) {
                func_80046CC0(&D_800E37C0_60[i]);
            }
            func_8006C28C(0);
            D_800F05A8_26.flags &= ~0x1F;
        }
        return;
    }
    if (D_800F05A8_26.state == 7 || D_800F05A8_26.state == 9 || D_800F05A8_26.state == 11) {
        if (!(D_800F05A8_26.flags & 0xF)) {
            if (D_800F1510_2[0] >= -24.0f && D_800F1510_2[0] < 23.0f && D_800F1510_2[2] >= 79.0f && D_800F1510_2[2] < 87.0f &&
                D_800F1510_2[1] >= 0.0f && D_800F1510_2[1] < 6.2f) {
                D_800F05A8_26.t6C = 180;
                D_800F05A8_26.c68++;
                func_8004EFA4();
                D_800E3F68 = 1;
                D_800F05A8_26.flags |= 0x11;
                if ((D_800F05A8_26.f6E >> ((D_800EFDAC - 1) >> 1)) == 1) {
                    D_800F05A8_26.flags |= 0x20;
                } else {
                    D_800F05A8_26.flags &= ~0x20;
                }
            } else if (D_800F1510_2[0] >= -24.0f && D_800F1510_2[0] < 23.0f && D_800F1510_2[2] >= -80.0f && D_800F1510_2[2] < -73.0f &&
                       D_800F1510_2[1] >= 0.0f && D_800F1510_2[1] < 6.2f) {
                D_800F05A8_26.t6C = 180;
                D_800F05A8_26.c6A++;
                func_8004EFA4();
                D_800E4748 = 1;
                D_800F05A8_26.flags |= 0x12;
                if ((D_800F05A8_26.f6E >> ((D_800EFDAC - 1) >> 1)) == 0) {
                    D_800F05A8_26.flags |= 0x20;
                } else {
                    D_800F05A8_26.flags &= ~0x20;
                }
            }
        }
    }
    switch (D_800F05A8_26.state) {
    case 6:
        if (--D_800F05A8_26.t26 == 0) {
            D_800F05A8_26.state = 7;
            D_800F05A8_26.t2C = 0x2A30;
            D_800F05A8_26.flags &= ~0x10;
        }
        break;
    case 7:
        if (--D_800F05A8_26.t2C == 0) {
            j = 0;
            D_800F05A8_26.state = 8;
            D_800F05A8_26.t26 = 240;
            D_800F05A8_26.flags |= 0x10;
            for (; j < D_800EFD70_39.nplayers; j++) {
                func_80046CC0(&D_800E37C0_60[j]);
            }
            func_8006C28C(0);
        }
        break;
    case 8:
        if (--D_800F05A8_26.t26 == 0) {
            D_800F05A8_26.state = 9;
            D_800F05A8_26.t2C = 0x2A30;
            D_800F05A8_26.flags &= ~0x10;
        }
        break;
    case 9:
        if (--D_800F05A8_26.t2C == 0) {
            D_800F05A8_26.flags |= 0x10;
            if (D_800F05A8_26.c68 == D_800F05A8_26.c6A) {
                D_800F05A8_26.state = 10;
                D_800F05A8_26.t26 = 240;
            } else {
                D_800F05A8_26.state = 12;
                D_800F05A8_26.t26 = 600;
                D_800F05A8_26.f40 = 1;
            }
            for (i = 0; i < D_800EFD70_39.nplayers; i++) {
                func_80046CC0(&D_800E37C0_60[i]);
            }
            func_8006C28C(0);
        }
        break;
    case 10:
        if (--D_800F05A8_26.t26 == 0) {
            D_800F05A8_26.state = 11;
            D_800F05A8_26.t2C = 0x1C20;
            D_800F05A8_26.flags &= ~0x10;
        }
        break;
    case 11:
        if (--D_800F05A8_26.t2C == 0) {
            j = 0;
            D_800F05A8_26.state = 12;
            D_800F05A8_26.f40 = 1;
            D_800F05A8_26.t26 = 600;
            D_800F05A8_26.flags |= 0x10;
            for (; j < D_800EFD70_39.nplayers; j++) {
                func_80046CC0(&D_800E37C0_60[j]);
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

typedef struct {
    s16 f0;
    s16 pad2[3];
    s16 f8;
    s16 padA[5];
    s32 f14[6];
    s32 f2C;
} Sub_10;

typedef struct {
    u8 pad0[0x630];
    f32 f630;
    u8 pad634[0x660 - 0x634];
    Sub_10 sub;
    s32 f690;
    u8 pad694[0x6E8 - 0x694];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_13;

typedef struct {
    s32 f0;
    u8 pad4[0x1C];
    s16 f20;
    s16 f22;
    s16 f24;
    s16 pad26;
    s16 f28;
    s16 pad2A;
    s32 f2C;
    u8 pad30[0x10];
    s32 f40;
    u8 pad44[0xC];
    s8 f50[8];
    s32 f58;
} Game_23;

typedef struct {
    u8 pad0[4];
    s16 f4[6];
    s16 f10[42];
    s16 f64;
    u8 f66[5];
    u8 f6B;
    u8 f6C[8];
} Race_4;

extern Game_23 D_800F05A8_27;
extern Player_13 D_800E37C0_61[];
extern Player_13 *D_800F0C10_3[];
extern Race_4 *D_800CA930_11;

void func_800521F4(void) {
    s32 i, j, k, m;
    s32 cnt, n1;
    s32 pad[8];
    u8 buf[4];
    f32 f;
    s32 s, t;
    u8 tmp;
    Player_13 *p;
    Sub_10 *q;

    func_800515A8();
    n1 = 0;
    cnt = n1;
    if (!(D_800F05A8_27.f58 & 0x10)) {
        D_800F05A8_27.f2C++;
    }
    *(s32 *)buf = 0;
    for (m = 0; m < D_800F05A8_27.f22; m++) {
        q = &D_800E37C0_61[m].sub;
        if ((D_800E37C0_61[m].f690 & 8) || (D_800C8CD8 != 0 && D_800E3700 == m) || D_800E37C0_61[m].f6E8 == 0) {
            cnt++;
        }
        if (D_800E37C0_61[m].sub.f8 == -1 && m < cnt) {
            n1++;
        }
        if (q->f8 != -1 || D_800E37C0_61[m].f6E8 != 0) {
            buf[m] = 1;
        }
        if (D_800F05A8_27.f0 == 5) {
            s32 k = q->f0;
            if (k < D_800F05A8_27.f20) {
                q->f14[k]++;
            }
        }
    }
    for (m = 0; m < cnt; m++) {
        if (!buf[m]) break;
    }
    if (!((m == cnt && n1 == 0 && D_800EFD70_8.f4C != 8) || (D_800EFD70_8.f4C == 8 && buf[D_800C8CE0]))) return;
    if (D_800EFD70_8.f0 & 0x81) return;
    if (D_800CA920 == 3) return;
    if (D_800F05A8_27.f40 == 0) {
        D_800F05A8_27.f28 = 180;
    } else if (D_800F05A8_27.f28 != 0) {
        D_800F05A8_27.f28--;
    }
    D_800F05E8 = 1;
    D_800CA920 = 2;
    if (D_800F0520 == -1) {
        D_800F0520 = D_800E76D0;
    }
    if (D_800C8CE8 == 0 && D_800E36F8 == 5) {
        if ((D_800E30B0_2[0] & 0x40) && !(D_800E25B0_2[0] & 0x40)) goto go;
        if ((D_800E30B0_2[1] & 0x40) && !(D_800E25B0_2[1] & 0x40)) goto go;
        if ((D_800E30B0_2[2] & 0x40) && !(D_800E25B0_2[2] & 0x40)) goto go;
        if ((D_800E30B0_2[3] & 0x40) && !(D_800E25B0_2[3] & 0x40)) goto go;
        if (D_800CCD64 != 0 && (D_800EFD70_8.f0 & 0x01000000) && (D_800E30B0_2[0] & 0x20) && !(D_800E25B0_2[0] & 0x20)) goto go;
        if (D_800F05D0 != 0 && D_800CCD64 == 0) goto end;
        if ((D_800E30B0_2[0] & 0x10) && !(D_800E25B0_2[0] & 0x10)) goto go;
        if ((D_800E30B0_2[1] & 0x10) && !(D_800E25B0_2[1] & 0x10)) goto go;
        if ((D_800E30B0_2[2] & 0x10) && !(D_800E25B0_2[2] & 0x10)) goto go;
        if (!(D_800E30B0_2[3] & 0x10) || (D_800E25B0_2[3] & 0x10)) goto end;
    go:
        if (D_800CCD64 != 0) {
            func_8001102C(2);
        }
        if (D_800E36F8 == 5) {
            D_800E36F8 = 6;
            D_800E374C = 20;
        }
        for (k = 0; k < D_800F05A8_27.f22; k++) {
            if (D_800E37C0_61[k].sub.f8 == -1) {
                f = D_800E37C0_61[k].f630 / (D_800EE48C * D_800F05A8_27.f20);
                if (f <= 0.1f) {
                    f = 0.1f;
                } else if (f > 1.0f) {
                    f = 1.0f;
                }
                f = D_800F05A8_27.f2C / f;
                D_800E37C0_61[k].sub.f2C = f;
            }
        }
        for (k = 0; k < D_800F05A8_27.f22; k++) {
            if (D_800E37C0_61[k].sub.f8 == -1) {
                D_800E37C0_61[k].sub.f8 = 0;
                for (j = 0; j < D_800F05A8_27.f22; j++) {
                    if ((j != k && k < j && D_800E37C0_61[j].sub.f2C < D_800E37C0_61[k].sub.f2C) ||
                        (j < k && D_800E37C0_61[j].sub.f2C <= D_800E37C0_61[k].sub.f2C)) {
                        D_800E37C0_61[k].sub.f8++;
                    }
                }
                if (D_800EFD70_8.f4C == 1) {
                    D_800CA930_11->f66[k] += D_800CA938_2[D_800E37C0_61[k].sub.f8].b3;
                }
            }
            D_800F05A8_27.f50[D_800E37C0_61[k].sub.f8] = k;
        }
        D_800F05A8_27.f24 = D_800F05A8_27.f22;
        if (D_800EFD70_8.f4C == 1) {
            for (i = 0; i < D_800F05A8_27.f22; i++) {
                D_800CA930_11->f4[i] += D_800CA930_11->f66[i];
                D_800CA930_11->f6C[i] = i;
            }
            for (i = 0; i < D_800F05A8_27.f22; i++) {
                j = i;
                if (i != 0) {
                    s = D_800CA930_11->f4[D_800CA930_11->f6C[i - 1]];
                }
                t = D_800CA930_11->f4[D_800CA930_11->f6C[i]];
                while (j != 0 && s < t) {
                    tmp = D_800CA930_11->f6C[j];
                    D_800CA930_11->f6C[j] = D_800CA930_11->f6C[j - 1];
                    D_800CA930_11->f6C[j - 1] = tmp;
                    j--;
                    if (j != 0) {
                        s = D_800CA930_11->f4[D_800CA930_11->f6C[j - 1]];
                    }
                }
            }
        }
    }
end:
    if (D_800C29CC->f10 == 3) {
        s = -1;
        for (i = D_800F05A8_27.f24; i < D_800F05A8_27.f22; i++) {
            q = &D_800F0C10_3[i]->sub;
            if (q->f8 == s) {
                D_800F05A8_27.f50[D_800F05A8_27.f24] = i;
                q->f8 = D_800F05A8_27.f24++;
                q->f2C = (D_800F05A8_27.f2C * D_800F0C10_3[0]->f630) / D_800E37C0_61[i].f630;
                if (D_800EFD70_8.f4C == 1 + (((u32)&D_800E37C0_61 & 1) >> 1)) {
                    D_800CA930_11->f10[(D_800F0C10_3[i] - D_800E37C0_61) * 7 + D_800CA930_11->f64] = q->f8;
                    D_800CA930_11->f66[D_800F0C10_3[i] - D_800E37C0_61] = ((Pts *)((u32)D_800CA938_2 | (((u32)D_800CA938_2 & 1) >> 1)))[q->f8].b3;
                }
            }
        }
    }
}

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
} Ctl_14;

extern Ctl2 D_800EFD70_40;
extern Ctl_14 D_800F05A8_28;

void func_80052A6C(s32 arg0)
{
    s32 i;
    s32 *p;

    if ((D_800EFD70_40.w00 & 0x80) == 0) {
        s32 bits;
        i = 0;
        bits = D_800C292C;
        for (p = D_800E3770; i < 4; i++, p++) {
            if (((bits >> i) & 1) != 0 && *p != 0) break;
        }
        if (i == 4) {
            D_800F05A8_28.w4C = D_800F05A8_28.w4C + 1;
        } else {
            D_800F05A8_28.w4C = 0;
        }
    }
    switch (D_800EFD70_40.w4C) {
    case 2:
        func_80051B6C();
        break;
    case 5:
        func_800515A8();
        if (D_800F05A8_28.w40 == 0) {
            if ((D_800F05A8_28.w58 & 0x10) != 0) break;
            D_800F05A8_28.w2C = D_800F05A8_28.w2C + 1;
            if (D_800F05A8_28.w2C == 0x34bc0) {
                func_80050F7C(0);
                D_800F05A8_28.w40 = 1;
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
    if (D_800EFD70_40.w4C < 2 && D_800F05A8_28.w24 > 0 && D_800F05A8_28.p50 < D_800EFD70_40.w3C &&
        D_800F05A8_28.w88 != -2 && (D_800EFD70_40.w3C == 1 || D_800EFD70_40.w4C == 8)) {
        D_800F05A8_28.w88 = D_800F05A8_28.w88 + 1;
        if (D_800EFD70_40.w3C >= 3) {
            if (D_800F05A8_28.w88 >= 100) goto store;
        } else if (D_800F05A8_28.w88 >= 0x8c) {
            goto store;
        }
        return;
store:
        D_800F05A8_28.w88 = -2;
    }
    return;
}

void func_80052C90(void)
{
    s32 iVar2;
    s32 iVar3;
    u16 *puVar4;
    s32 auStack_50[16];
    s32 i;
    s32 t13;
    u8 *s2;

    if (D_800EFDBC == 9) {
        D_800F06B0 = (u16 *)func_800BC908(0x200, 0, D_800D7340, 0xB4B);
        *D_800F06B0 = 0;
        puVar4 = D_800F06B0 + 1;
        iVar3 = 1;
        do {
            iVar2 = iVar3 >> 4;
            *puVar4 = (u16)(iVar2 << 1) | (u16)((iVar2 << 6) | 1) | (u16)(iVar2 << 11);
            puVar4 = puVar4 + 1;
            iVar3 = iVar3 + 1;
        } while (iVar3 < 0x100);
    }
    D_800EFD60 = func_800BF79C(D_800E7728, D_800D0D00);
    s2 = D_800EFD70_2;
    if (1 < *(s32 *)(s2 + 0x3C)) {
        D_800F0524 = func_800BF79C(D_800E7728, D_800D11B0);
    }
    if (*(s32 *)(s2 + 0x4C) == 9) {
        *(s32 *)(s2 + 0x730) = func_800BF79C(D_800E7728, D_800D09F8);
        D_800F0740 = func_800BF79C(D_800E7728, D_800D10E8);
        D_800EFD6C = func_800BF79C(D_800E7728, D_800D0DEC);
        D_800EFD68 = func_800BF79C(D_800E7728, D_800D09D4);
    }
    func_800BFC40(D_800D8424, func_80014D20);
    if (*(s32 *)(s2 + 0x3C) == 1) {
        D_800F0730[0] = func_800BC908(0x1c0, 0, D_800D7340, 0xB6B);
        D_800F0730[1] = func_800BC908(0x1c0, 0, D_800D7340, 0xB6C);
    }
    t13 = *(volatile s32 *)&D_800F13F0;
    guPerspective(auStack_50, D_800F0744, 33.0f, 320.0f / (f32)t13, 50.0f, 4192.0f, 1.0f);
    iVar3 = 0;
    if (*(s32 *)(s2 + 0x4C) == 2) {
        D_800F0528 = func_800BF79C(D_800E7728, D_800D1084);
    }
    for (i = 0; i < 0x1E; i++) {
        D_800F0530[i] = func_800C1E60(0, 0x40, 2, 2, 0);
        D_800F06B8[i] = func_800C1E60(0, 0x40, 2, 2, 0);
    }
}

typedef struct { char c[9]; } Str9;
typedef struct { char c[2]; } Str2;

extern Str9 D_800D8438;

void func_80052F30(s32 param_1, char *param_2)
{
    char buf[32];

    if (param_1 > 0x9E340) {
        *(Str9 *)param_2 = D_800D8438;
        return;
    }
    func_8000EE30(param_1 / 0xE10, buf);
    if (strlen(buf) == 1) {
        *(Str2 *)param_2 = *(Str2 *)D_800D8444;
        func_8000ED30(param_2, buf);
    } else {
        func_8000EF8C(param_2, buf);
    }
    func_8000ED30(param_2, D_800D8448);
    func_8000EE30((param_1 / 60) % 60, buf);
    if (strlen(buf) == 1) {
        func_8000ED30(param_2, D_800D8444);
        func_8000ED30(param_2, buf);
    } else {
        func_8000ED30(param_2, buf);
    }
    func_8000ED30(param_2, D_800D844C);
    func_8000EE30(((param_1 % 60) * 100) / 60, buf);
    if (strlen(buf) == 1) {
        func_8000ED30(param_2, D_800D8444);
        func_8000ED30(param_2, buf);
    } else {
        func_8000ED30(param_2, buf);
    }
}

void func_8005313C(s32 param_1, char *param_2)
{
    s32 iVar1;
    char acStack_30[32];

    *param_2 = 0;
    if (param_1 > 0xe0f) {
        func_8000EE30(param_1 / 0xe10, param_2);
        func_8000ED30(param_2, D_800D8448);
    }
    func_8000EE30((param_1 / 0x3c) % 0x3c, acStack_30);
    iVar1 = strlen(acStack_30);
    if ((iVar1 == 1) && (param_1 > 0xe0f)) {
        func_8000ED30(param_2, D_800D8444);
    }
    func_8000ED30(param_2, acStack_30);
    func_8000ED30(param_2, D_800D844C);
    func_8000EE30(((param_1 % 0x3c) * 100) / 0x3c, acStack_30);
    iVar1 = strlen(acStack_30);
    if (iVar1 == 1) {
        func_8000ED30(param_2, D_800D8444);
    }
    func_8000ED30(param_2, acStack_30);
    return;
}

void func_800532D4(u8 *p, s32 n, s32 arg2)
{
    char buf[0x28];
    s32 m;
    u8 *base;
    s32 off;
    Quad *r;

    if (D_800F05CA >= 2) {
        buf[0] = n + 0x31;
        m = n;
        if (m > 3) {
            m = 3;
        }
        buf[1] = m + 0x7B;
        strcpy(buf + 2, " Place");
        func_800C1F2C(p, buf);
    } else {
        func_800C1F2C(p, "Finished");
    }
    base = D_800E1718;
    off = D_800C2598 * 16 + (D_800E1898 - 1) * 64;
    r = (Quad *)(base + off);
    func_800C0ACC(p, r->a + r->c / 2, r->b + r->d / 2);
}

Gfx *func_800533D0(Gfx *gdl, s32 p1, s32 p2, s32 p3)
{
    char buf[32];
    s32 x;
    s32 y;
    f32 fa;
    s32 *tab;

    func_800C2224();
    if (D_800E2038 & 0x20) {
        func_800C1E60(D_800F0530_2, 0x40, 1, 2, 0x10);
        func_800C1F2C(D_800F0530_2, "Testing User Track");
        tab = D_800E1718_4;
        x = D_800F13EC * 0x12;
        if (x < 0) {
            x = x + 0x1ff;
        }
        func_800C0ACC(D_800F0530_2, D_800E1718_4[0] + (x >> 9), tab[1] + (D_800F13F0 * 0x11) / 0xf0);
    }
    fa = D_800E37C0_62[0x108 + p3 * 0x1f8];
    if (0.0f < fa) {
        func_8000EE30((s32)(fa * 3600.0f / 1612.9f), buf);
    } else {
        func_8000EE30((s32)(-fa * 3600.0f / 1612.9f), buf);
    }
    func_800C1E60(D_800F0530_2, 0x40, 3, 2, 0x12);
    func_800C1F2C(D_800F0530_2, buf);
    x = 0x1ba;
    if (D_800CCCA8 == 2) {
        x = 0x228;
    }
    y = 0xd6;
    if (D_800CCCA8 == 2) {
        y = 0x180;
    }
    func_800C0ACC(D_800F0530_2, x, y);
    gdl = func_800C1394(gdl);
    gdl = func_80009754(gdl, p1, p2, p3);
    return gdl;
}

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x3FF - 0x54];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x66A - 0x424];
    s16 h66A;
    u8 pad66C[0x68C - 0x66C];
    s32 w68C;
    u8 pad690[0x69C - 0x690];
    s32 w69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Player_14;

extern Player_14 D_800E37C0_63[];

Gfx *func_800535B8(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[32];
    f32 v;
    s32 x1, y1;
    s32 t;
    s32 *p;

    func_800C2224();
    func_800C1E60(D_800F0530[0], 0x40, 1, 2, 0x20);
    func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
    func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
    func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
    func_800C1E60(D_800F0530[7], 0x40, 1, 6, 0x20);
    func_800C1E60(D_800F0530[8], 0x40, 1, 6, 0x20);
    if (D_800CCCA8 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 0);
    }
    if (D_800C2594 == 1) {
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        v = D_800E37C0_63[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530[6], buf);
        x1 = 0x1BA;
        if (D_800CCCA8 == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (D_800CCCA8 == 2) {
            y1 = 0x180;
        }
        func_800C0ACC(D_800F0530[6], x1, y1);
        gdl = func_80009754(gdl, a1, a2, pl);
        func_800C1E60(D_800F0530[4], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[5], 0x40, 1, 4, 0x20);
        func_80052F30(D_800E37C0_63[pl].w69C - D_800F05D4, buf);
        func_800C1F2C(D_800F0530[4], "Time:");
        func_800C0ACC(D_800F0530[4], 0x23, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], 100, D_800CCCA8 == 2 ? 0x180 : 0xD6);
    } else {
        func_800C1E60(D_800F0530[4], 0x40, 2, 2, 0x20);
        func_800C1E60(D_800F0530[5], 0x40, 2, 4, 0x20);
        t = -D_800F05D4 + D_800E37C0_63[pl].w69C;
        if (D_800E37C0_63[pl].b3FF) {
            t = D_800E37C0_63[pl].w68C;
        }
        if (t < 0) {
            t = 0;
        }
        func_80052F30(t, buf);
        func_800C1F2C(D_800F0530[4], "Time:");
        func_800C0ACC(D_800F0530[4], D_800E1718_5[D_800E1898 - 1][pl][0] + 10, 0x1E);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], D_800E1718_5[D_800E1898 - 1][pl][0] + 0x32, 0x1E);
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        v = D_800E37C0_63[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530[6], buf);
        func_800C0ACC(D_800F0530[6],
                      D_800E1718_5[D_800E1898 - 1][pl][0] + D_800E1718_5[D_800E1898 - 1][pl][2] - 0x2D,
                      D_800E1718_5[D_800E1898 - 1][pl][1] + D_800E1718_5[D_800E1898 - 1][pl][3] - 0xB);
        gdl = func_8005A4C8(gdl, a1, a2, pl);
    }
    if (D_800F05A8_5.h26 != 0 && D_800F05A8_5.w0 != 0 && (D_800F05A8_5.w0 != 1 || D_800F05A8_5.h26 < 0x5B)) {
        gdl = func_80054ED0(gdl, pl);
    }
    if (D_800E37C0_63[pl].h66A != 0 && D_800E3710 == 0) {
        func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
        func_800C1F2C(D_800F0530[7], "Stage Clear!");
        func_800C0ACC(D_800F0530[7],
                      D_800E1718_5[D_800E1898 - 1][pl][0] + D_800E1718_5[D_800E1898 - 1][pl][2] / 2,
                      D_800E1718_5[D_800E1898 - 1][pl][1] + D_800E1718_5[D_800E1898 - 1][pl][3] / 2);
    }
    if (D_800E37C0_63[pl].b3FF) {
        func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
        if (D_800F05E8 == 0 || pl != 0) {
            func_800C1F2C(D_800F0530[7], "Game Over");
        } else {
            func_800C1F2C(D_800F0530[7], D_800D84A8);
        }
        func_800C0ACC(D_800F0530[7],
                      D_800E1718_5[D_800E1898 - 1][pl][0] + D_800E1718_5[D_800E1898 - 1][pl][2] / 2,
                      D_800E1718_5[D_800E1898 - 1][pl][1] + D_800E1718_5[D_800E1898 - 1][pl][3] / 2);
    }
    if (D_800EFD70 & 0x20000000) {
        s32 *p1;
        s32 *p2;
        s32 *p3;
        p1 = func_800C1E60(D_800F0530[9], 0x20, 2, 2, 0x10);
        func_8005CD30(buf, D_800E37C0_63[0].f48, 5);
        func_800C1F2C(p1, buf);
        func_800C0ACC(p1, 0x28, 0x1E);
        p2 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
        func_8005CD30(buf, D_800E37C0_63[0].f50, 5);
        func_800C1F2C(p2, buf);
        func_800C0ACC(p2, 0x28, 0x28);
        p3 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
        func_8000EE30(D_800E2060, buf);
        func_800C1F2C(p3, buf);
        func_800C0ACC(p3, 0x28, 0x32);
    }
    return func_800C1394(gdl);
}

typedef struct {
    u8 pad0[0x18];
    u32 w18;
} Txt;

typedef struct {
    u8 pad0[0x18];
    char *name;
    u8 pad1C[0x3C - 0x1C];
} Trick;

typedef struct {
    u8 pad0[0x3C4];
    s32 w3C4;
    s32 w3C8;
    u8 pad3CC[4];
    u8 b3D0;
    u8 pad3D1;
    s8 s3D2;
    u8 pad3D3[0x3EA - 0x3D3];
    u8 b3EA;
    s8 s3EB[0x3FB - 0x3EB];
    u8 b3FB;
    u8 pad3FC[3];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x668 - 0x424];
    s16 h668;
    u8 pad66A[0x7E0 - 0x66A];
} Player_15;

extern Txt *D_800F0530_3[];
extern Player_15 D_800E37C0_64[];
extern Trick D_800C9A40_11[];

Gfx *func_80053D58(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    Player_15 *p = &D_800E37C0_64[pl];
    char buf[40];
    s32 idx, t;
    s32 x, y, ty, yt, yy;
    s32 x1, y1, xa;
    s32 x3, y3, ax, ay;
    s32 k, i;
    s32 by, dy;
    u8 c;
    s8 d;
    f32 v;

    func_800C2224();
    if (D_800EFDAC >= 2) {
        if (D_800F05E8 != 0) {
            func_800C1E60(D_800F0530_3[7], 0x40, 1, 4, 0x21);
            switch (p->h668) {
            case 0:
                func_800C1F2C(D_800F0530_3[7], "Winner");
                break;
            case 1:
                func_800C1F2C(D_800F0530_3[7], D_800D84E0);
                break;
            default:
                func_800C1F2C(D_800F0530_3[7], D_800D84E4);
                break;
            }
            func_800C0ACC(D_800F0530_3[7],
                          D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w / 2,
                          D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h / 2);
        }
        yt = D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h - 11;
        func_800C1E60(D_800F0530_3[0], 0x40, 2, 2, 0x20);
        func_800C1E60(D_800F0530_3[1], 0x40, 2, 4, 0x20);
        idx = -1;
        do {
        } while (0);
        func_80052F30(D_800F05D4, buf);
        func_800C1F2C(D_800F0530_3[0], "Time:");
        func_800C0ACC(D_800F0530_3[0], 0x3C, yt);
        func_800C1F2C(D_800F0530_3[1], buf);
        func_800C0ACC(D_800F0530_3[1], 100, yt);
        xa = D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w - 20;
        yy = D_800E1718[D_800E1898 - 1][pl].y;
        func_800C1E60(D_800F0530_3[4], 0x40, 2, 2, 0x22);
        func_800C1F2C(D_800F0530_3[4], "Total");
        ty = yy + 8;
        func_800C0ACC(D_800F0530_3[4], xa, ty);
        func_800C1E60(D_800F0530_3[5], 0x40, 2, 4, 0x22);
        func_8000EE30(p->w3C8, buf);
        func_800C1F2C(D_800F0530_3[5], buf);
        func_800C0ACC(D_800F0530_3[5], xa, yy + 16);
        c = p->b3D0;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            func_800C1E60(D_800F0530_3[2], 0x40, 2, 2, 0x21);
            x3 = 0x100;
            func_800C1F2C(D_800F0530_3[2], D_800C9A40_11[idx].name);
            y3 = ty + 8;
            func_800C1E60(D_800F0530_3[3], 0x40, 2, 4, 0x21);
            func_8000EE30(p->w3C4, buf);
            func_800C1F2C(D_800F0530_3[3], buf);
            ax = 0x100;
            ay = ty;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (xa - (D_800F0530_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((D_800F0530_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (ty + i * 8) * (256 - k)) / 256;
                }
            }
            func_800C0ACC(D_800F0530_3[2], ax, ay);
            func_800C0ACC(D_800F0530_3[3], x3, y3);
        }
        for (i = 0; i < p->b3EA; i++) {
            func_800C1E60(D_800F0530_3[i + 8], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530_3[i + 8], D_800C9A40_11[p->s3EB[i]].name);
            func_800C0ACC(D_800F0530_3[i + 8], 0x3C, ty + i * 8);
        }
        func_800C1E60(D_800F0530_3[6], 0x40, 3, 2, 0x12);
        v = p->f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530_3[6], buf);
        func_800C0ACC(D_800F0530_3[6],
                      D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w - 0x2D,
                      D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h - 0xB);
        gdl = func_8005A4C8(gdl, a1, a2, pl);
    } else {
        if (D_800CCCA8 == 2) {
            func_800C1D54(0x10000, 0x20000, 0, 0);
        }
        func_800C1E60(D_800F0530_3[0], 0x40, 1, 2, 0x10);
        func_800C1E60(D_800F0530_3[1], 0x40, 1, 4, 0x10);
        func_80052F30(D_800F05D4, buf);
        func_800C1F2C(D_800F0530_3[0], "Time:");
        func_800C0ACC(D_800F0530_3[0], 0x23, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        func_800C1F2C(D_800F0530_3[1], buf);
        func_800C0ACC(D_800F0530_3[1], 100, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        y = 0x19;
        x = D_800F13EC - 62;
        if (D_800CCCA8 == 2) {
            y = 0x69;
        }
        func_800C1E60(D_800F0530_3[4], 0x40, 1, 2, 0x22);
        func_800C1F2C(D_800F0530_3[4], "Total");
        func_800C0ACC(D_800F0530_3[4], x, y);
        if (D_800CCCA8 == 2) {
            y += 0x19;
        } else {
            y += 0xC;
        }
        func_800C1E60(D_800F0530_3[5], 0x40, 1, 4, 0x22);
        func_8000EE30(p->w3C8, buf);
        func_800C1F2C(D_800F0530_3[5], buf);
        func_800C0ACC(D_800F0530_3[5], x, y);
        if (D_800CCCA8 == 2) {
            by = 0x69;
            dy = 0x14;
        } else {
            by = 0x5A;
            dy = 0xA;
        }
        c = p->b3D0;
        idx = -1;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            func_800C1E60(D_800F0530_3[2], 0x40, 1, 2, 0x21);
            func_800C1F2C(D_800F0530_3[2], D_800C9A40_11[idx].name);
            func_800C1E60(D_800F0530_3[3], 0x40, 1, 4, 0x21);
            func_8000EE30(p->w3C4, buf);
            func_800C1F2C(D_800F0530_3[3], buf);
            x3 = D_800F13EC / 2;
            y3 = D_800E171C + 0x28;
            ax = D_800F13EC / 2;
            ay = D_800E171C + 0x14;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (x - (D_800F0530_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                y3 = (y3 * k + y * (256 - k)) / 256;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((D_800F0530_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (by + i * dy) * (256 - k)) / 256;
                }
            }
            func_800C0ACC(D_800F0530_3[2], ax, ay);
            func_800C0ACC(D_800F0530_3[3], x3, y3);
        }
        v = D_800E37C0_64[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1E60(D_800F0530_3[6], 0x40, 3, 2, 0x12);
        func_800C1F2C(D_800F0530_3[6], buf);
        x1 = 0x1BA;
        if (D_800CCCA8 == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (D_800CCCA8 == 2) {
            y1 = 0x180;
        }
        func_800C0ACC(D_800F0530_3[6], x1, y1);
        for (i = 0; i < p->b3EA; i++) {
            func_800C1E60(D_800F0530_3[i + 8], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530_3[i + 8], D_800C9A40_11[p->s3EB[i]].name);
            func_800C0ACC(D_800F0530_3[i + 8], 0x23, by + i * dy);
        }
        if (D_800F05E8 != 0) {
            func_800C1E60(D_800F0530_3[7], 0x40, 1, 4, 0x21);
            func_800C1F2C(D_800F0530_3[7], D_800E3BBF >= 3 ? "Press START to Continue" : "Game Over");
            func_800C0ACC(D_800F0530_3[7],
                          D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w / 2,
                          D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h / 2);
        }
    }
    gdl = func_800C1394(gdl);
    gdl = func_80009754(gdl, a1, a2, pl);
    if (D_800F05A8_5.h26 != 0 && D_800F05A8_5.w0 != 0 && (D_800F05A8_5.w0 != 1 || D_800F05A8_5.h26 < 0x5B)) {
        gdl = func_80054ED0(gdl, pl);
    }
    return gdl;
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct {
    u8 pad0[0x3FF]; u8 f3FF;
    u8 pad1[0x420 - 0x400]; f32 f420;
    u8 pad2[0x668 - 0x424]; s16 f668;
    u8 pad3[0x7E0 - 0x66A];
} Rec37_3;

typedef struct { s32 f0; u8 pad[0x22]; s16 f26; } Obj5A8;

extern Rec37_3 D_800E37C0_65[];
extern Obj5A8 D_800F05A8_29;

Gfx *func_800549E4(Gfx *gdl, s32 a1, s32 a2, s32 idx)
{
    char buf[32];
    f32 fv;
    s32 y;
    s32 *h;
    s32 n;
    s32 x;
    s32 yy;
    s32 z;
    char *p;

    PKT(0xE7000000, 0);
    PKT(0xE3000C00, 0);
    PKT(0xE3001001, 0);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xE200001C, 0x00404240);
    PKT(0xD9FFFFFE, 0);
    PKT(0xD7000002, 0x80008000);
    gdl = func_80009754(gdl, a1, a2, idx);
    func_800C2224();
    gdl = func_80055658(gdl, 0, func_800C1E60(D_800F0544, 0x20, 3, 2, 0x21));

    fv = D_800E37C0_65[idx].f420;
    func_8000EE30((s32)(((0.0f < fv) ? fv * 3600.0f : -fv * 3600.0f) / 1612.9f), buf);

    func_800C1E60(D_800F0530[0], 0x40, 3, 2, 0x12);
    func_800C1F2C(D_800F0530[0], buf);
    x = 0x1BA;
    if (*(volatile s32 *)&D_800CCCA8 == 2) x = 0x228;
    yy = 0xD6;
    if (*(volatile s32 *)&D_800CCCA8 == 2) yy = 0x180;
    func_800C0ACC(D_800F0530[0], x, yy);

    if (D_800CA934 <= 0) {
        func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
        func_80052F30(D_800F05D4, buf);
        y = D_800E1718_10[D_800E1898 - 1][idx].b;
        func_800C1F2C(D_800F0530[1], D_800D8484);
        z = y + 0x14;
        func_800C0ACC(D_800F0530[1], 0x23, z);
        func_800C1F2C(D_800F0530[2], buf);
        func_800C0ACC(D_800F0530[2], 100, z);
        func_800C1E60(D_800F0530[3], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[4], 0x40, 1, 4, 0x20);
        func_8000EE30(D_800E3BC2 + 1, buf);
        n = strlen(buf);
        func_8000EE30(10, buf + n + 1);
        p = buf;
        n = strlen(p);
        p += n;
        *p = '/';
        if (D_800CCCA8 == 2) z = y + 0x2C;
        else z = y + 0x20;
        func_800C1F2C(B_800F0530[3], D_800D850C);
        func_800C0ACC(B_800F0530[3], 0x23, z);
        func_800C1F2C(B_800F0530[4], buf);
        func_800C0ACC(B_800F0530[4], 100, z);
    }

    if (D_800E37C0_65[0].f668 != -1) {
        h = func_800C1E60(D_800F0548, 0x20, 1, 4, 0x21);
        if (D_800E37C0_65[0].f3FF == 0) {
            func_800C1F2C(h, D_800D84A8);
            func_800C0ACC(h,
                          D_800E1718_10[D_800E1898 - 1][idx].a + D_800E1718_10[D_800E1898 - 1][idx].c / 2,
                          D_800E1718_10[D_800E1898 - 1][idx].b + D_800E1718_10[D_800E1898 - 1][idx].d / 2);
        } else {
            func_800532D4(h, D_800E37C0_65[0].f668, idx);
        }
    }

    gdl = func_80055658(gdl, 0, func_800C1E60(D_800F0544, 0x20, 3, 2, 0x21));
    gdl = func_800C1394(gdl);
    if (D_800F05A8_29.f26 != 0 && D_800F05A8_29.f0 != 0 &&
        (D_800F05A8_29.f0 != 1 || D_800F05A8_29.f26 < 0x5B)) {
        gdl = func_80054ED0(gdl, idx);
    }
    return gdl;
}

typedef struct { s32 unk0; u16 *col; Img_2 img[5]; } Hud_3;
typedef struct { s32 mode; u8 pad[0x22]; s16 t; } State_5;

extern State_5 D_800F05A8_30;
extern Hud_3 *D_800EFD60_2;

Gfx *func_80054ED0(Gfx *gdl, s32 a)
{
    f32 scale;
    s32 alpha;
    u16 col;
    Img_2 *img;
    s32 sw, dsdx, th;
    s32 *r;

    if (D_800F05A8_30.mode == 5 && D_800F05A8_30.t == 0)
        return gdl;
    if (D_800C8CE0 != 0)
        a = 0;
    switch (D_800F05A8_30.mode) {
    default:
        return gdl;
    case 1: {
        f32 v;
        col = D_800EFD60_2->col[4];
        img = &D_800EFD60_2->img[4];
        scale = D_800F05A8_30.t + 10;
        scale /= 100.0f;
        scale *= 2.0f;
        if (scale > 1.0f)
            v = 100.0f - (scale - 1.0f) * 100.0f;
        else
            v = scale * 100.0f;
        alpha = v;
        if (alpha < 0)
            alpha = 0;
        scale = 2.0f;
        break;
    }
    case 2: {
        f32 v;
        scale = D_800F05A8_30.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60_2->img[2];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60_2->col[2];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 3: {
        f32 v;
        scale = D_800F05A8_30.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60_2->img[1];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60_2->col[1];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 4: {
        f32 v;
        scale = D_800F05A8_30.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60_2->img[0];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60_2->col[0];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 5:
    case 14:
    case 15:
        scale = D_800F05A8_30.t + 1;
        scale *= 0.0625f;
        img = &D_800EFD60_2->img[3];
        col = D_800EFD60_2->col[3];
        alpha = scale * 120.0f;
        scale = 3.5f - scale * 3.0f;
        break;
    }

    sw = scale * (img->w * 8.0f);
    dsdx = 1024.0f / scale;
    if (D_800F05A8_30.mode == 1)
        th = scale * img->h * 0.5f;
    else
        th = scale * img->h;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, 0x4041C8, 0);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, img->img);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPLoadSync(gdl++);
    gDPLoadTile(gdl++, G_TX_LOADTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetPrimColor(gdl++, 0, 0, (col >> 8) & 0xF8, (col >> 3) & 0xF8, (col << 2) & 0xF8, alpha);
    {
        u8 *t = (u8 *)D_800E1718;
        r = (s32 *)((a << 4) + ((D_800E1898 - 1) << 6) + t);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, r[0], r[1], r[0] + r[2] - 1, r[1] + r[3] - 1);
    gSPTextureRectangle(gdl++, (r[0] + r[2] / 2 - sw / 2) << 2, (r[1] + r[3] / 2 - th / 2) << 2,
                        (r[0] + r[2] / 2 + sw / 2) << 2, (r[1] + r[3] / 2 + th / 2) << 2,
                        G_TX_RENDERTILE, 0, (img->h - 1) << 5, dsdx,
                        D_800F05A8_30.mode == 1 ? -(dsdx * 2) : -dsdx);
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    u8 pad[0x664];
    s16 num;
    s16 ang;
    u8 pad2[0x7E0 - 0x668];
} Player_16;

void *func_80055658(void *param_1, s32 param_2, void *param_3)
{
    Player_16 *base = D_800E37C0;
    Player_16 *pl = &base[param_2];
    s32 a = pl->ang;
    char buf[3];

    if (a > 0x180) {
        a = 0x300 - a;
    }
    param_1 = func_800C1394(param_1);
    func_800C2224();
    buf[0] = ((u8 *)&pl->num)[1] + '1';
    buf[1] = (pl->num < 3) ? ((u8 *)&pl->num)[1] + ':' : '=';
    buf[2] = 0;
    func_800C1F2C(param_3, buf);
    if (D_800CCCA8 == 2) {
        func_800C1D54(a << 9, a << 9, 0, 0);
        func_800C0ACC(param_3, D_800E1718[D_800E1898 - 1][D_800C2598].x + D_800E1718[D_800E1898 - 1][D_800C2598].w - 0x3C,
                      D_800E1718[D_800E1898 - 1][D_800C2598].y + 0x14);
        param_1 = func_800C1394(param_1);
        func_800C1D54(0x10000, 0x20000, 0, 0);
    } else {
        func_800C1D54(a << 8, a << 8, 0, 0);
        func_800C0ACC(param_3, D_800E1718[D_800E1898 - 1][D_800C2598].x + D_800E1718[D_800E1898 - 1][D_800C2598].w - 0x28,
                      D_800E1718[D_800E1898 - 1][D_800C2598].y + 0xA);
        param_1 = func_800C1394(param_1);
        func_800C1D54(0x10000, 0x10000, 0, 0);
    }
    func_800C2224();
    return param_1;
}

#define VP D_800E1718[D_800E1898 - 1][D_800C2598]

typedef struct {
    u8 pad0[4];
    s32 w4;
    u8 pad8[0x18 - 0x8];
    u32 w18;
} Txt_2;

typedef struct {
    s16 h0;
    union { s16 h; u8 b[2]; } u2;
    u8 pad4[4];
    s16 h8;
    s16 hA;
    s16 hC;
    u8 padE[2];
    s32 w10;
    s32 w14[6];
    s32 w2C;
} Lap;

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x420 - 0x54];
    f32 f420;
    u8 pad424[0x660 - 0x424];
    Lap lap;
    u8 pad690[0x6EE - 0x690];
    u8 b6EE;
    u8 pad6EF[0x7AC - 0x6EF];
    s16 h7AC;
    u8 pad7AE[0x7E0 - 0x7AE];
} Player_17;

typedef struct {
    s32 w0;
    u8 pad4[0x10 - 0x4];
    s32 w10;
    u8 pad14[0x20 - 0x14];
    s16 h20;
    s16 h22;
    u8 pad24[2];
    s16 h26;
    u8 pad28[4];
    s32 w2C;
    u8 pad30[0x40 - 0x30];
    s32 w40;
} Mode_2;

typedef struct {
    u32 w0;
    u8 pad4[0x4C - 4];
    s32 w4C;
} Game_24;

extern Player_17 D_800E37C0_66[];
extern Mode_2 D_800F05A8_31;
extern Game_24 D_800EFD70_41;

Gfx *func_8005581C(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[88];
    char *p;
    Lap *lp;
    Txt_2 *tRace;
    Txt_2 *tLap;
    Txt_2 *tBest;
    Txt_2 *tPlace;
    Txt_2 *tRaceL;
    Txt_2 *tLapL;
    Txt_2 *tBestL;
    Txt_2 *tSpeed;
    Txt_2 *tMsg;
    Txt_2 *t;
    s32 v;
    f32 f;
    s32 x, y;
    s32 xa, xb, yy, dy;

    lp = &D_800E37C0_66[(D_800C9650 == -1) ? pl + D_800C8CE0 : D_800C9650].lap;
    if (D_800E1898 == 1 && D_800C9650 != -1) {
        pl = D_800C9650;
    }
    func_800C2224();
    if (D_800CCCA8 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 0);
    }
    if (D_800E1898 < 3) {
        tPlace = func_800C1E60(D_800F0530[0], 0x20, 3, 2, 0x21);
        if (D_800E1898 == 1) {
            tSpeed = func_800C1E60(D_800F0530[1], 0x20, 3, 2, 0x12);
        } else {
            tSpeed = func_800C1E60(D_800F0530[1], 0x20, 3, 2, 0x11);
        }
        tRace = func_800C1E60(D_800F0530[2], 0x20, 2, 2, 0x10);
        tBest = func_800C1E60(D_800F0530[3], 0x20, 2, 2, 0x10);
        tRaceL = func_800C1E60(D_800F0530[4], 0x20, 2, 2, 0x10);
        tBestL = func_800C1E60(D_800F0530[5], 0x20, 2, 2, 0x10);
        if (D_800F05A8_31.h26 != 0 && D_800F05A8_31.w0 == 5) {
            tMsg = func_800C1E60(D_800F0530[6], 0x20, 1, 2, 0x21);
        } else {
            tMsg = func_800C1E60(D_800F0530[6], 0x20, 1, 4, 0x21);
        }
        if (lp->hA != 0 || lp->h0 == D_800F05A8_31.h20) {
            if (lp->hA != 0) {
                lp->hA--;
            }
            tLap = func_800C1E60(D_800F0530[7], 0x20, 2, 4, 0x10);
            tLapL = func_800C1E60(D_800F0530[8], 0x20, 2, 4, 0x10);
        } else {
            tLap = func_800C1E60(D_800F0530[7], 0x20, 2, 2, 0x10);
            tLapL = func_800C1E60(D_800F0530[8], 0x20, 2, 2, 0x10);
        }
        if (D_800EFD70_41.w0 & 0x20000000) {
            Txt_2 *t1, *t2, *t3;
            t1 = func_800C1E60(D_800F0530[9], 0x20, 2, 2, 0x10);
            func_8005CD30(buf, D_800E37C0_66[0].f48, 5);
            func_800C1F2C(t1, buf);
            func_800C0ACC(t1, 0x28, 0x1E);
            t2 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
            func_8005CD30(buf, D_800E37C0_66[0].f50, 5);
            func_800C1F2C(t2, buf);
            func_800C0ACC(t2, 0x28, 0x28);
            t3 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
            func_8000EE30(D_800E2060, buf);
            func_800C1F2C(t3, buf);
            func_800C0ACC(t3, 0x28, 0x32);
        }
        if (D_800EFD70_41.w4C == 6 && D_800F05A8_31.w2C == 0) {
            t = func_800C1E60(D_800F0530[19], 0x20, 2, 2, 0x11);
            func_800C1F2C(t, "Press Up C Button for side view");
            func_800C0ACC(t, 0x100, 0x28);
        }
    } else {
        tPlace = func_800C1E60(D_800F0530[0], 8, 2, 2, 0x12);
        tSpeed = func_800C1E60(D_800F0530[1], 0x20, 2, 2, 0x12);
        tRace = func_800C1E60(D_800F0530[2], 0x20, 2, 2, 0x10);
        tBest = func_800C1E60(D_800F0530[3], 0x20, 2, 2, 0x10);
        tRaceL = func_800C1E60(D_800F0530[4], 0x20, 2, 2, 0x10);
        tBestL = func_800C1E60(D_800F0530[5], 0x20, 2, 2, 0x10);
        tLap = func_800C1E60(D_800F0530[6], 0x20, 2, 4, 0x10);
        tLapL = func_800C1E60(D_800F0530[7], 0x20, 2, 4, 0x10);
        if (D_800F05A8_31.h26 != 0 && D_800F05A8_31.w0 == 5) {
            tMsg = func_800C1E60(D_800F0530[8], 0x20, 1, 2, 0x21);
        } else {
            tMsg = func_800C1E60(D_800F0530[8], 0x20, 1, 4, 0x21);
        }
    }
    if (lp->h8 != -1) {
        func_800532D4(tMsg, lp->h8, pl);
    }
    if (D_800C8CE8 == 0) {
        if (D_800F05A8_31.h26 != 0 && D_800F05A8_31.w0 != 0 && (D_800F05A8_31.w0 != 1 || D_800F05A8_31.h26 < 0x5B)) {
            gdl = func_80054ED0(gdl, pl);
        } else if (D_800E3710 == 0 && lp->h8 == -1) {
            if (D_800E37C0_66[pl].lap.hC != 0) {
                v = D_800E37C0_66[pl].lap.w10;
                D_800E37C0_66[pl].lap.hC--;
                if (v < 0) {
                    v = -v;
                    buf[0] = '+';
                    tMsg->w4 = 8;
                } else if (v > 0) {
                    buf[0] = '-';
                    tMsg->w4 = 7;
                }
                buf[1] = 0;
                func_8005313C(v, buf + 1);
                func_800C1F2C(tMsg, buf);
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 4);
            } else if (((D_800E37C0_66[pl].h7AC - 1) & 0x20) == 0) {
                func_800C1F2C(tMsg, "Off Course");
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            } else if (D_800E37C0_66[pl].b6EE != 0 && (D_800E76D0 & 0x20)) {
                func_800C1F2C(tMsg, "Wrong Way");
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            }
        }
        switch (D_800E1898) {
        case 1:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPPipeSync(gdl++);
            if (D_800F05A8_31.h22 >= 2) {
                gdl = func_80055658(gdl, pl, tPlace);
            }
            f = D_800E37C0_66[pl].f420;
            func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            func_800C1F2C(tSpeed, buf);
            x = 0x1BA;
            if (D_800CCCA8 == 2) {
                x = 0x228;
            }
            y = 0xD6;
            if (D_800CCCA8 == 2) {
                y = 0x180;
            }
            func_800C0ACC(tSpeed, x, y);
            if (lp->h0 == D_800F05A8_31.h20) {
                func_80052F30(lp->w2C, buf);
            } else {
                func_80052F30(D_800F05A8_31.w2C, buf);
            }
            if (D_800CA934 < 2) {
                func_800C1F2C(tRace, buf);
                func_800C1F2C(tRaceL, "Race: ");
                xa = VP.x + 0x43;
                xb = VP.x + 0x14;
                if (D_800CCCA8 != 2) {
                    yy = VP.y + VP.h - 0x1C;
                    dy = 10;
                } else {
                    yy = VP.y + VP.h - 0x38;
                    dy = 0x14;
                }
                if (D_800EFD64_8->w50 & 2) {
                    yy += dy;
                }
                func_800C0ACC(tRaceL, xb, yy);
                func_800C0ACC(tRace, xa, yy);
                yy += dy;
                __builtin_strcpy(buf, "Lap ");
                p = buf;
                if (lp->hA != 0 || lp->h0 == D_800F05A8_31.h20) {
                    func_8000EE30(lp->h0, p + strlen(p));
                } else {
                    func_8000EE30(lp->h0 + 1, p + strlen(p));
                }
                func_8000ED30(p, ": ");
                func_800C1F2C(tLapL, buf);
                func_80052F30(lp->w14[(lp->hA != 0 || lp->h0 == D_800F05A8_31.h20) ? lp->h0 - 1 : lp->h0], buf);
                func_800C1F2C(tLap, buf);
                func_800C0ACC(tLapL, xb, yy);
                func_800C0ACC(tLap, xa, yy);
                yy += dy;
                if (!(D_800EFD64_8->w50 & 2)) {
                    func_80052F30(D_800F05A8_31.w10, buf);
                    func_800C1F2C(tBestL, "Best: ");
                    func_800C1F2C(tBest, buf);
                    func_800C0ACC(tBestL, xb, yy);
                    func_800C0ACC(tBest, xa, yy);
                }
            }
            gdl = func_800C1394(gdl);
            gDPPipeSync(gdl++);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gdl = func_80009754(gdl, a1, a2, D_800C2598);
            break;
        case 2:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            gdl = func_80055658(gdl, pl, tPlace);
            if (D_800CA934 < 2) {
                f = D_800E37C0_66[pl].f420;
                func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
                func_800C1F2C(tSpeed, buf);
                func_800C0ACC(tSpeed, VP.x + VP.w - 0x2D, VP.y + VP.h - 0xB);
                if (lp->h0 == D_800F05A8_31.h20) {
                    func_80052F30(lp->w2C, buf);
                } else {
                    func_80052F30(D_800F05A8_31.w2C, buf);
                }
                func_800C1F2C(tRace, buf);
                func_800C1F2C(tRaceL, "Race: ");
                func_800C0ACC(tRaceL, VP.x + 10, VP.y + VP.h - 0x12);
                func_800C0ACC(tRace, VP.x + 0x43, VP.y + VP.h - 0x12);
            }
            __builtin_strcpy(buf, "Lap ");
            p = buf;
            if (lp->hA != 0 || lp->h0 == D_800F05A8_31.h20) {
                func_8000EE30(lp->h0, p + strlen(p));
            } else {
                func_8000EE30(lp->h0 + 1, p + strlen(p));
            }
            func_8000ED30(p, ": ");
            func_800C1F2C(tLapL, buf);
            func_800C0ACC(tLapL, VP.x + 10, VP.y + VP.h - 8);
            func_80052F30(lp->w14[(lp->hA != 0 || lp->h0 == D_800F05A8_31.h20) ? lp->h0 - 1 : lp->h0], buf);
            func_800C1F2C(tLap, buf);
            func_800C0ACC(tLap, VP.x + 0x43, VP.y + VP.h - 8);
            gdl = func_8005A4C8(func_800C1394(gdl), a1, a2, pl);
            break;
        default:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            buf[0] = D_800E37C0_66[pl].lap.u2.b[1] + '1';
            buf[1] = (D_800E37C0_66[pl].lap.u2.h < 3) ? D_800E37C0_66[pl].lap.u2.b[1] + '{' : '~';
            buf[2] = 0;
            func_800C1F2C(tPlace, buf);
            func_800C0ACC(tPlace, VP.x + VP.w - 10, VP.y + 13);
            f = D_800E37C0_66[pl].f420;
            func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            func_800C1F2C(tSpeed, buf);
            func_800C1F2C(tSpeed, "mph");
            func_800C0ACC(tSpeed, VP.x + VP.w - 10, VP.y + VP.h - 6);
            gdl = func_8005A4C8(func_800C1394(gdl), a1, a2, pl);
            break;
        }
    } else {
        gdl = func_800C1394(gdl);
    }
    if (D_800F05A8_31.w40 != 0 && D_800C2598 == D_800E1898 - 1) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                      D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
        func_800C1E60(D_800F06B8[0], 0x40, 1, 2, 0x21);
        func_800C1E60(D_800F06B8[1], 0x40, 1, 2, 0x21);
        func_800C1F2C(D_800F06B8[0], "Press START to Continue");
        if (D_800E1898 == 1) {
            func_800C0ACC(D_800F06B8[0], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + 0x17);
        } else {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xA0);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
            gDPFillRectangle(gdl++, D_800F13EC * 0, D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - 7,
                             D_800F13EC - 1, D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + 7);
            func_800C0ACC(D_800F06B8[0], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2);
        }
        gdl = func_800C1394(gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
} Player_18;

typedef struct {
    u32 w0;
    u8 pad4[0x2C - 0x4];
    s32 w2C;
    u8 pad30[0x58 - 0x30];
    s32 w58;
    u8 pad5C[0x68 - 0x5C];
    s16 h68;
    s16 h6A;
} Mode_3;

typedef struct {
    u8 pad0[0x3C];
    s32 w3C;
} Game_25;

extern Player_18 D_800E37C0_67[];
extern Mode_3 D_800F05A8_32;
extern Game_25 D_800EFD70_42;

Gfx *func_80056C94(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    char buf[32];
    char msg[64];
    s32 cx;
    s32 cy;
    s32 n;
    s32 team;

    func_800C2224();
    if (p3 == 0) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                      D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
        if (D_800F05A8_32.w0 == 7 || D_800F05A8_32.w0 == 9 || D_800F05A8_32.w0 == 11) {
            gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xE6);
            gDPRaw(gdl++, 0xE3000C00, 0);
            gDPRaw(gdl++, 0xD7000002, 0x80008000);
            gDPRaw(gdl++, 0xE3001001, 0);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            cx = 0x100;
            cy = 0x78;
            if (D_800EFDAC == 4) {
                cx = 0x13C;
            }
            gdl = func_8000A690((cx - D_800F0528_2[8] * 4) | 1, cy - (D_800F0528_2[9] >> 1) - 1, D_800F0528_2 + 8, gdl,
                                p2, 1, 1, 6, 0);
            gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            gDPRaw(gdl++, 0xD7000000, 0x80008000);
            gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x28, 0xFF, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800E37C0_67[0].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0_67[0].f48 * 0.25f - 2.0f,
                             cx + D_800E37C0_67[0].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0_67[0].f48 * 0.25f + 2.0f);
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0x28, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800E37C0_67[1].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0_67[1].f48 * 0.25f - 2.0f,
                             cx + D_800E37C0_67[1].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0_67[1].f48 * 0.25f + 2.0f);
            if (D_800EFDAC == 4) {
                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0xFA, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + D_800E37C0_67[2].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0_67[2].f48 * 0.25f - 2.0f,
                                 cx + D_800E37C0_67[2].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0_67[2].f48 * 0.25f + 2.0f);
                gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0x28, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + D_800E37C0_67[3].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0_67[3].f48 * 0.25f - 2.0f,
                                 cx + D_800E37C0_67[3].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0_67[3].f48 * 0.25f + 2.0f);
            }
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0xFA, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800F1510.z * 0.25f / 0.625f - 1.0f, cy + D_800F1510.x * 0.25f - 1.0f,
                             cx + D_800F1510.z * 0.25f / 0.625f + 1.0f, cy + D_800F1510.x * 0.25f + 1.0f);
        }
        switch (D_800F05A8_32.w0) {
        case 6:
            func_800C1E60(D_800F0530[0], 0x40, 1, 4, 0x21);
            if (D_800EFDAC == 2) {
                func_800C1F2C(D_800F0530[0], "One on One Soccer: First Half");
            } else {
                func_800C1F2C(D_800F0530[0], D_800D865C);
            }
            func_800C0ACC(D_800F0530[0], D_800F13EC / 2, D_800F13F0 / 2);
            break;
        case 8:
        case 10:
        case 12:
            func_800C1E60(D_800F0530[0], 0x40, 1, 6, 0x21);
            switch (D_800F05A8_32.w0) {
            case 10:
                func_800C1F2C(D_800F0530[0], "Full Time Draw : Overtime Required");
                break;
            case 8:
                func_800C1F2C(D_800F0530[0], "Half Time");
                break;
            case 12:
                func_800C1F2C(D_800F0530[0], "Final");
                break;
            }
            func_800C0ACC(D_800F0530[0], D_800F13EC / 2, D_800F13F0 / 2 - 13);
            func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[2], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
            func_800C1E60(D_800F0530[4], 0x40, 1, 4, 0x20);
            func_800C1F2C(D_800F0530[1], "Team 1:");
            func_800C1F2C(D_800F0530[2], "Team 2:");
            func_8000EE30(D_800F05A8_32.h68, buf);
            func_800C1F2C(D_800F0530[3], buf);
            func_8000EE30(D_800F05A8_32.h6A, buf);
            func_800C1F2C(D_800F0530[4], buf);
            func_800C0ACC(D_800F0530[1], D_800F13EC / 2 - 45, D_800F13F0 / 2);
            func_800C0ACC(D_800F0530[2], D_800F13EC / 2 - 45, D_800F13F0 / 2 + 13);
            func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 39, D_800F13F0 / 2);
            func_800C0ACC(D_800F0530[4], D_800F13EC / 2 + 39, D_800F13F0 / 2 + 13);
            if (D_800F05A8_32.w0 == 12) {
                func_800C1E60(D_800F0530[0], 0x40, 1, 4, 0x21);
                func_800C1F2C(D_800F0530[0], "Press START To Continue");
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2,
                              D_800E1718[D_800E1898 - 1][0].y + D_800E1718[D_800E1898 - 1][0].h / 2);
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2,
                              D_800E1718[D_800E1898 - 1][D_800E1898 - 1].y +
                                  D_800E1718[D_800E1898 - 1][D_800E1898 - 1].h / 2);
            }
            break;
        default:
            func_800C1E60(D_800F0530[0], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
            func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
            func_800C1F2C(D_800F0530[0], "Team 1:");
            func_800C1F2C(D_800F0530[1], "Team 2:");
            func_8000EE30(D_800F05A8_32.h68, buf);
            func_800C1F2C(D_800F0530[2], buf);
            func_8000EE30(D_800F05A8_32.h6A, buf);
            func_800C1F2C(D_800F0530[3], buf);
            if (D_800EFDAC == 4) {
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2 - 45, 30);
                func_800C0ACC(D_800F0530[1], D_800F13EC / 2 - 45, D_800F13F0 - 30);
                func_800C0ACC(D_800F0530[2], D_800F13EC / 2 + 39, 30);
                func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 39, D_800F13F0 - 30);
            } else {
                func_800C0ACC(D_800F0530[0], 65, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[1], 65, D_800F13F0 / 2 + 7);
                func_800C0ACC(D_800F0530[2], 150, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[3], 150, D_800F13F0 / 2 + 7);
            }
            func_800C1E60(D_800F0530[4], 0x40, 1, 2, 0x22);
            func_800C1E60(D_800F0530[5], 0x40, 1, 4, 0x22);
            switch (D_800F05A8_32.w0) {
            case 11:
                func_800C1F2C(D_800F0530[4], "Overtime");
                break;
            case 7:
                func_800C1F2C(D_800F0530[4], "First Half");
                break;
            case 9:
                func_800C1F2C(D_800F0530[4], "Second Half");
                break;
            }
            func_80052F30(D_800F05A8_32.w2C, buf);
            func_800C1F2C(D_800F0530[5], buf);
            if (D_800EFDAC == 4) {
                func_800C0ACC(D_800F0530[4], D_800F13EC / 2 - 10, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[5], D_800F13EC / 2 - 10, D_800F13F0 / 2 + 7);
            } else {
                func_800C0ACC(D_800F0530[4], D_800F13EC - 65, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[5], D_800F13EC - 65, D_800F13F0 / 2 + 7);
            }
            break;
        }
    }
    n = D_800F05A8_32.w58 & 0xF;
    if (n != 0) {
        team = n - 1;
        func_800C1E60(D_800F0530[7], 0x40, D_800EFD70_42.w3C == 4 ? 2 : 1, 4, 0x21);
        if (team == (p3 >> ((D_800EFD70_42.w3C - 1) >> 1))) {
            if (D_800F05A8_32.w58 & 0x20) {
                func_800C1F2C(D_800F0530[7], "Team ");
                func_8000EE30((team ^ 1) + 1, buf);
                func_800C1F2C(D_800F0530[7], buf);
                func_800C1F2C(D_800F0530[7], " scored for you!");
            } else {
                func_800C1F2C(D_800F0530[7], D_800D871C);
            }
        } else {
            if (D_800F05A8_32.w58 & 0x20) {
                strcpy(msg, "You helped team ");
                func_8000EE30(n, msg + strlen(msg));
                func_8000ED30(msg, " score!");
            } else {
                strcpy(msg, "Team ");
                func_8000EE30(n, msg + strlen(msg));
                func_8000ED30(msg, " scored!");
            }
            func_800C1F2C(D_800F054C, msg);
        }
        func_800C0ACC(D_800F054C, D_800E1718[D_800E1898 - 1][p3].x + D_800E1718[D_800E1898 - 1][p3].w / 2,
                      D_800E1718[D_800E1898 - 1][p3].y + D_800E1718[D_800E1898 - 1][p3].h / 2);
    }
    return func_800C1394(gdl);
}

#define gDPWords(pkt, a, b) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (a); \
        _g->words.w1 = (b); \
    }

typedef struct {
    u8 w;
    u8 h;
    u8 unk2[6];
} Glyph;

typedef struct {
    s32 unk0;
    u16 *palette;
    Glyph glyphs[1];
} Font;

typedef struct {
    u8 unk0[0x730];
    Font *font;
} Hud_4;

extern Hud_4 D_800EFD70_43;

Gfx *func_80058294(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y) {
    while (*str != 0) {
        s32 w; s32 hh;

        gDPWords(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPWords(gdl++, 0xE3001001, 0x8000);
        gDPWords(gdl++, 0xFD100000, (u32)D_800EFD70_43.font->palette);
        gDPWords(gdl++, 0xE8000000, 0);
        gDPWords(gdl++, 0xF5000100, 0x07000000);
        gDPWords(gdl++, 0xE6000000, 0);
        gDPWords(gdl++, 0xF0000000, 0x073FC000);
        gDPWords(gdl++, 0xE7000000, 0);
        w = D_800EFD70_43.font->glyphs[*str].w << 3;
        hh = D_800EFD70_43.font->glyphs[*str].h;
        gdl = func_8000A690(x, y - (hh >> 1), &D_800EFD70_43.font->glyphs[*str], gdl, arg1, 1, 1, 3, 0);
        str++;
        x += w;
    }
    return gdl;
}

#define CUR_FLAGS (D_800CADE4_10[D_800CA930_5->f64].flags)

typedef struct {
    s32 val;
    char *str[3];
} Entry_4;

typedef struct {
    u8 pad0[0xC];
    u8 flags;
    u8 num;
    u8 padE[2];
    char *name;
    u8 pad14[0xC];
    u32 count;
    Entry_4 *entries;
} Track_13;

typedef struct {
    u32 state;
    u8 pad4[0x22];
    s16 f26;
    u8 pad28[4];
    s32 f2C;
    u8 pad30[0x28];
    s32 f58;
    u8 pad5C[0x14];
    s32 f70;
    u8 pad74[4];
    s32 f78;
} Game_26;

typedef struct {
    u8 pad[0x420];
    f32 f420;
    u8 pad2[0x3BC];
} Bike_32;

extern Track_13 D_800CADE4_10[];
extern Game_26 D_800F05A8_33;
extern Bike_32 D_800E37C0_68[];

Gfx *func_80058438(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3)
{
    Track_13 *trk;
    Entry_4 *e;
    Gfx *p;
    u8 *img;
    u8 *img2;
    s32 off;
    u32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 d;
    s32 v;
    s32 n;
    s32 w;
    s32 x0;
    s32 x;
    s32 y;
    u8 flags;
    f32 f;
    char buf[32];
    s32 cx;
    u8 *g;

    trk = &D_800CADE4_10[D_800CA930_5->f64];
    func_800C2224();
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPRaw(gdl++, 0xE200001C, 0x404240);
    if (D_800F05A8_33.state == 0xD) {
        if (D_800F05A8_33.f70 == 0) {
            off = -(D_800F05A8_33.f78 * 2);
        } else {
            off = 0;
            if (D_800F05A8_33.f70 == 2) {
                off = -((60 - D_800F05A8_33.f78) * 2);
            }
        }
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800EFD68_2 + 4));
        cx = D_800F13EC / 2;
        w = D_800EFD68_2[0x10] * 4 - 10;
        gdl = func_8000A690(cx - w, off + 0x1E, D_800EFD68_2 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        gdl = func_8000A690(D_800F13EC / 2 - D_800EFD6C_2[0x18] * 4, off + 0x1A, D_800EFD6C_2 + 0x18, gdl, arg2, 1, 1, 6, 0);
        func_800C1E60(D_800F0530[i], 0x40, 1, 2, 0x20);
        func_800C1F2C(D_800F0530[i], trk->name);
        func_800C0ACC(D_800F0530[i], 0x4D - i * 3, i * 13 + 0x44 + off);
        func_800C1E60(D_800F0530[3], 0x40, 1, 2, 0x21);
        __builtin_memcpy(buf, D_800D88D0, 9);
        buf[strlen(buf) - 1] += trk->num;
        func_800C1F2C(D_800F0530[3], buf);
        func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 3, off + 0x29);
    } else {
        if (D_800C8CE4 != 0) {
            if (D_800E2038 & 0x20) {
                func_800C1E60(D_800F0530[5], 0x40, 1, 2, 0x20);
                func_800C1F2C(D_800F0530[5], "Press START");
                func_800C0ACC(D_800F0530[5], 0x2F, 0x2D);
            }
        } else {
            flags = CUR_FLAGS;
            if (flags & 4) {
                func_800C1E60(D_800F0530[0], 0x40, 2, 6, 0x20);
                if (D_800F05A8_33.f58 & 0x1000) {
                    func_800C1F2C(D_800F0530[0], "Takedown Required");
                } else {
                    func_800C1F2C(D_800F0530[0], "Takedown Complete");
                }
                func_800C0ACC(D_800F0530[0], 0x21, 0x2D);
            } else if (flags & 8) {
              if (!(flags & 2)) {
                func_800C1E60(D_800F0530[0], 0x40, 1, 6, 0x20);
                v = D_800F05A8_33.f2C;
                if (v < 0) {
                    v = -v;
                }
                func_8005313C(v, buf);
                func_800C1F2C(D_800F0530[0], "Time: ");
                func_800C1F2C(D_800F0530[0], buf);
                if (D_800F05A8_33.f2C < 0) {
                    D_800F0530[0][1] = 8;
                }
                func_800C0ACC(D_800F0530[0], 0x21, 0x2D);
              }
            }
        }
        func_800C1E60(D_800F0530[1], 0x40, 1, 4, 0x20);
        func_800C1F2C(D_800F0530[1], trk->name);
        func_800C0ACC(D_800F0530[1], 0x3D, 0x20);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        if ((CUR_FLAGS & 2) && D_800F0738 >= 2 && D_800F0738 < 10) {
            func_800C1E60(D_800F0530[2], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530[2], trk->entries[D_800F0738].str[0]);
            func_800C0ACC(D_800F0530[2], 0x140, 0x28);
            p = gdl;
            gDPSetTextureLUT(p++, G_TT_NONE);
            gDPRaw(p++, 0xFC119623, 0xFF2FFFFF);
            k = 27;
            x0 = 0x2EE;
            g = D_800EFD6C_2 + 0x58;
            p = func_8000A690(x0 - g[0] * k * 8 - D_800EFD6C_2[0x60] * 8, 0x1C, D_800EFD6C_2 + 0x60, p, arg2, 1, 1, 6, 0);
            for (m = 0; m < 27; m++) {
                p = func_8000A690(x0 - (k - m) * g[0] * 8, 0x1C, g, p, arg2, 1, 1, 6, 0);
            }
            gdl = func_80058294(p, arg2, trk->entries[D_800F0738].str[1], 0x140, 0x40);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        } else {
            if (D_800F06AC == 1) {
                if (D_800F0738 == 0) {
                    i = trk->count - 1;
                } else {
                    i = D_800F0738;
                }
                goto show;
            }
            if (D_800F06AC == -1) {
                for (j = 0; j < 3; j++) {
                    func_800C1E60(D_800F0530[j + 2], 0x40, 2, 2, 0x20);
                    func_800C1F2C(D_800F0530[j + 2], D_800CB098[j]);
                    func_800C0ACC(D_800F0530[j + 2], 0x20, 0xB2 + j * 9);
                }
            } else {
                if (D_800C8CE4 == 0 && !(D_800F05A8_33.f58 & 0x8000)) {
                    goto skip;
                }
                if (!(D_800F05A8_33.f58 & 0x800)) {
                    goto skip;
                }
                for (i = 0; i < trk->count; i++) {
                    d = trk->entries[i].val - D_800F05D4;
                    if (d <= 0) {
                        d = D_800F05D4 - trk->entries[i].val;
                    }
                    if (d >= 3 && !(D_800F0600 & 0x8000)) {
                        continue;
                    }
                    {
                    show:
                        n = 3;
                        if (trk->entries[i].str[2] == NULL) {
                            n = 2;
                            if (trk->entries[i].str[1] == NULL) {
                                n = 1;
                            }
                        }
                        for (j = 0; j < n; j++) {
                            func_800C1E60(D_800F0530[j + 2], 0x40, 2, 2, 0x20);
                            func_800C1F2C(D_800F0530[j + 2], trk->entries[i].str[j]);
                            func_800C0ACC(D_800F0530[j + 2], 0x20, 0xBB - (n - 1) * 9 / 2 + j * 9);
                        }
                        break;
                    }
                }
            }
            img = D_800EFD6C_2 + 0x30;
            for (i = 0; i < 19; i++) {
                gdl = func_8000A690(i * img[0] * 8 + 0x10, 0xA9, img, gdl, arg2, 1, 1, 6, 0);
            }
            p = func_8000A690(img[0] * 0x98 + 0x10, 0xA9, D_800EFD6C_2 + 0x38, gdl, arg2, 1, 1, 6, 0);
            gdl = func_80058294(p, arg2, D_800C9A3C, 0x136, 0xC3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        }
    skip:
        if (D_800F05A8_33.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img = D_800EFD6C_2 + 0x30;
        } else {
            img = D_800EFD6C_2 + 0x40;
        }
        for (i = 0; i < 9; i++) {
            gdl = func_8000A690(i * img[0] * 8 + 0x10, 0x14, img, gdl, arg2, 1, 1, 6, 0);
        }
        if (D_800F05A8_33.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img2 = D_800EFD6C_2 + 0x38;
        } else {
            img2 = D_800EFD6C_2 + 0x48;
        }
        gdl = func_8000A690(img[0] * 0x48 + 0x10, 0x14, img2, gdl, arg2, 1, 1, 6, 0);
        gDPRaw(gdl++, 0xE200001C, 0x404240);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        if (D_800CA930_5->f64 > 0) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, D_800F06B0);
        }
        gdl = func_8000A690(0x21, 0x1B, D_800F0740 + 0x20, gdl, arg2, 1, 1, 3, 0);
        if (D_800CA930_5->f64 < 16 && D_800CADE4_10[D_800CA930_5->f64 + 1].num <= D_800CA930_5->f0[1]) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, D_800F06B0);
        }
        gdl = func_8000A690(0x96, 0x1B, D_800F0740 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
    }
    gdl = func_80009754(gdl, arg1, arg2, 0);
    func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
    f = D_800E37C0_68[arg3].f420;
    func_8000EE30((s32)((f > 0.0f ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
    func_800C1F2C(D_800F0530[6], buf);
    x = 0x1BA;
    if (D_800CCCA8 == 2) {
        x = 0x228;
    }
    y = 0xD6;
    if (D_800CCCA8 == 2) {
        y = 0x180;
    }
    func_800C0ACC(D_800F0530[6], x, y);
    if ((CUR_FLAGS & 8) && D_800F05A8_33.f26 != 0
        && (D_800F05A8_33.state < 5 || D_800F05A8_33.state == 15 || D_800F05A8_33.state == 14)
        && (D_800F05A8_33.state != 1 || D_800F05A8_33.f26 < 0x5B)) {
        gdl = func_80054ED0(gdl, arg3);
    }
    return func_800C1394(gdl);
}

typedef struct {
    u8 *hdr;
    u8 pad[0x60];
    s16 idx;
} Ctl_15;

s32 func_800595A4(void) {
    Ctl_15 *c = D_800CA930;
    s32 a1 = c->idx;
    s32 n = a1 + 1;
    u8 v1, v0;

    if (n > 16) {
        return 0;
    }
    v1 = D_800CADE4[a1 * 0x28 + 0xD];
    v0 = D_800CADE4[n * 0x28 + 0xD];
    if (v1 == v0) {
        return 1;
    }
    return v1 < c->hdr[1];
}

#define ROWS (D_800EFDAC_2[0] >= 2 ? 6 : 9)
#define COLS (D_800EFDAC_2[0] >= 3 ? 8 : 12)
#define RECT (D_800E1718[D_800C2594 - 1][D_800F05F8])

typedef struct Plr_7 {
    u8 pad0[0x64];
    s16 h64;
} Plr_7;

typedef struct Trk_10 {
    u8 pad0[0x1C];
    s32 f1C;
    u8 pad20[0x28 - 0x20];
} Trk_10;

extern Plr_7 *D_800CA930_12;
extern Trk_10 D_800CADE4_11[];

Gfx *func_80059614(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    int i;
    int j;
    s32 a;
    s32 prev;
    s32 sp;
    int n;

    if (D_800CA934 != 3 || D_800EFDBC == 9) {
        switch (D_800EFDBC) {
        case 2:
            if (D_800E3710 == 0) {
                gdl = func_80056C94(gdl, p1, p2, p3);
            }
            break;
        case 5:
            gdl = func_800549E4(gdl, p1, p2, p3);
            break;
        case 7:
            gdl = func_800535B8(gdl, p1, p2, p3);
            break;
        case 10:
            gdl = func_80053D58(gdl, p1, p2, p3);
            break;
        case 13:
        case 14:
            gdl = func_800533D0(gdl, p1, p2, p3);
            break;
        case 9:
            gdl = func_80058438(gdl, p1, p2, p3);
            break;
        default:
            gdl = func_8005581C(gdl, p1, p2, p3);
            break;
        }
    } else if (D_800C8CE8 == 0) {
        gdl = func_80054ED0(gdl, p3);
    }
    if (D_800F0630 >= 0 || D_800F0630 == -2) {
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
        prev = 35;
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, prev);
        for (i = 0; i < ROWS; i++) {
            for (j = i & 1; j < COLS; j += 2) {
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (D_800C2C60[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 15) >> 12;
                    } else {
                        a = ((D_800C2C60[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 25) >> 14) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 35);
        prev = 35;
        for (i = 0; i < ROWS; i++) {
            for (j = ~i & 1; j < COLS; j += 2) {
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (D_800C2C60[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 255) >> 14;
                    } else {
                        a = ((D_800C2C60[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 55) >> 12) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
    }
    sp = 14;
    if (D_800CCCA8_2 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 1);
        sp = 28;
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                  D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
    func_800C2224();
    switch (D_800E3710) {
    case 6:
        for (n = 0; n < 3; n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
            func_800C1F2C(D_800F06B8[n], D_800CB0FC[n]);
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp + sp * n);
        }
        break;
    case 1:
        for (n = 0; n < (D_800EFDBC == 1 ? 4 : 5); n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 0x11);
            if (D_800EFDBC == 1) {
                func_800C1F2C(D_800F06B8[n], D_800CB108[n]);
            } else if (D_800EFDBC == 2) {
                func_800C1F2C(D_800F06B8[n], D_800CB0E8[n]);
            } else {
                func_800C1F2C(D_800F06B8[n], D_800CB0D4[n]);
            }
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp * 2 + sp * n);
        }
        break;
    case 5:
        if (D_800F0618 != 6) {
            s32 k;
            k = D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp * 2;
            for (n = 0; n < 5; n++) {
                if (n == 1 && D_800CADE4_11[D_800CA930_12->h64].f1C == 0) {
                    continue;
                }
                if (n == 3 && func_800595A4() == 0) {
                    continue;
                }
                if (n == 4 && D_800CA930_12->h64 == 0) {
                    continue;
                }
                func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
                func_800C1F2C(D_800F06B8[n], D_800CB0C0[n]);
                func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2, k);
                k += sp;
            }
        }
        break;
    case 4:
        for (n = 0; n < 2; n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
            func_800C1F2C(D_800F06B8[n], D_800CB0B8[n]);
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + sp * n);
        }
        break;
    }
    gdl = func_800C1394(gdl);
    func_800C1D54(0x10000, 0x10000, 0, 0);
    return gdl;
}

#define RP_2(T, n) ((Rect *)(((D_800C2598 << 4) + (((n) - 1) << 6)) + (u8 *)T))
#define RX(T, n) (RP_2(T, n)->x + RP_2(T, n)->w)
#define RY(T, n) (RP_2(T, n)->y + RP_2(T, n)->h)
#define R8(c) (((c) >> 8) & 0xF8)
#define G8(c) (((c) >> 3) & 0xF8)
#define B8(c) (((c) << 2) & 0xF8)
#define SL(v,s,w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define DIM(c, a) (SL(R8(c) >> 2, 24, 8) | SL(G8(c) >> 2, 16, 8) | SL(B8(c) >> 2, 8, 8) | SL(a, 0, 8))
#define GW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 w, h; u16 pad; void *addr; } Img_6;
typedef struct { s32 f0; u16 *pal; Img_6 img[5]; } HudTex;

typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 pad655[0x7E0 - 0x655];
} Player_19;

extern HudTex *D_800F0524_2;
extern HudTex *hudA_800F0524[];
extern Player_19 D_800E37C0_69[];

Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 fp, n, bar, k;
    u16 col;
    Img_6 *img;
    Rect *r;


    fp = -54;
    n = D_800E1898;
    if (n == 2) fp = -46;
    bar = D_800E37C0_69[pl].f648 * 38.0f / 12000.0f;
    if (bar >= 39) bar = 38;
    GW(gdl++, 0xE7000000, 0);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xD9FFFFFE, 0);
    GW(gdl++, 0xD7000002, 0x80008000);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xFA000000, DIM(D_800F0524_2->pal[0], 0x8C));
    { Rect *tab = D_800C2494_3; gdl = func_8000A690(RX(tab, n) - 24, RY(tab, n) + fp, &D_800F0524_2->img[0], gdl, a2, 1, 1, 6, 0); }
    GW(gdl++, 0xFA000000, DIM(D_800F0524_2->pal[4], 0xA0));
    gdl = func_8000A690(RX(D_800C2494_3, D_800E1898) - 24, RY(D_800C2494_3, D_800E1898) + fp, &D_800F0524_2->img[4], gdl, a2, 1, 1, 6, 0);
    bar++; GW(gdl++, 0xFA000000, DIM(D_800F0524_2->pal[3], 0x8C));
    gdl = func_8000A690(RX(D_800C2494_3, D_800E1898) - 24, RY(D_800C2494_3, D_800E1898) + fp, &D_800F0524_2->img[3], gdl, a2, 1, 1, 6, 0);
    for (k = 0; k < 3; k++) {
        switch (k) {
        case 0:
            img = &D_800F0524_2->img[0];
            col = D_800F0524_2->pal[0];
            break;
        case 1:
            img = &D_800F0524_2->img[4];
            col = D_800F0524_2->pal[4];
            break;
        case 2:
            img = &D_800F0524_2->img[3];
            col = D_800F0524_2->pal[3];
            break;
        }
        GW(gdl++, 0xFD680000 | ((img->w * 8 - 1) & 0xFFF), img->addr);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0x07000000);
        GW(gdl++, 0xE6000000, 0);
        GW(gdl++, 0xF4000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | 0x07000000 | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0);
        GW(gdl++, 0xF2000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xFA000000, (((col >> 8) & 0xF8) << 24) | (((col >> 3) & 0xF8) << 16) | ((col << 10) & 0xF800) | 0xFF);
        r = RP_2(D_800E1718, D_800E1898);
        GW(gdl++, 0xE4000000 | ((((r->x + r->w) * 4 + (img->w * 8 - 24) * (D_800F13EC << 2) / 512) & 0xFFF) << 12) |
                      (((r->y + r->h) * 4 + (fp + img->h) * (D_800F13F0 << 2) / 240) & 0xFFF),
           ((((r->x + r->w) * 4 + -24 * (D_800F13EC * 4) / 512) & 0xFFF) << 12) |
               (((r->y + r->h) * 4 + (fp + img->h - bar) * (D_800F13F0 << 2) / 240) & 0xFFF));
        GW(gdl++, 0xE1000000, ((bar - 1) << 5) & 0xFFFF);
        GW(gdl++, 0xF1000000, ((D_800F13EC == 640) ? 0x03330000 : 0x04000000) | ((D_800F13F0 == 240) ? 0xFC00 : 0xFE00));
        GW(gdl++, 0xE7000000, 0);
    }
    if (D_800E37C0_69[pl].f654) {
        HudTex *h = hudA_800F0524[0]; GW(gdl++, 0xFA000000, (((h->pal[2] >> 8) & 0xF8) << 24) | (((D_800E2038 << 3) & 0x100) ? ((D_800E2038 << 3) & 0xFF) : (0xFF ^ ((D_800E2038 << 3) & 0xFF))));
            { Rect *tab = D_800C2494_3; gdl = func_8000A690(RX(tab, D_800E1898) - 24, RY(tab, D_800E1898) + fp, &D_800F0524_2->img[2], gdl, a2, 1, 1, 6, 0); }
    }
    GW(gdl++, 0xFA000000, (((D_800F0524_2->pal[1] >> 8) & 0xF8) << 24) | (((D_800F0524_2->pal[1] >> 3) & 0xF8) << 16) | ((D_800F0524_2->pal[1] << 10) & 0xF800) | 0x82);
    { Rect *tab = D_800C2494_3; return func_8000A690(RX(tab, D_800E1898) - 24, RY(tab, D_800E1898) + fp, &D_800F0524_2->img[1], gdl, a2, 1, 1, 6, 0); }
}

s32 func_8005AE38(void)
{
    return *(s16 *)(D_800CA930 + 0x64) != 0;
}

typedef struct {
    s32 limit;
    s32 counter;
    s32 arg;
} Ctr_2;

extern Ctr_2 D_800CA924_3;

void func_8005AE4C(void)
{
    if (0 < D_800CA924_3.limit && (D_800EFD70 & 4) == 0) {
        D_800CA924_3.counter = D_800CA924_3.counter + 1;
        if (D_800CA924_3.counter == D_800CA924_3.limit) {
            func_800BCB04(D_800CA924_3.arg);
        }
    }
    if (D_800EFDBC == 9 && D_800F0624 != 0) {
        func_800BCB04(D_800F0624);
    }
    func_8006F5F8(1, 0);
}

u32 func_8005AED8(s32 key)
{
    Pair_4 *p = D_800F073C;
    s32 idx;
    if (p == NULL) {
        idx = -1;
    } else {
        for (; p->val >= 0; p++) {
            if (p->key == key) {
                idx = ((volatile Pair_4 *)p)->val;
                goto found;
            }
        }
        idx = -1;
    }
found:
    if (idx < 0) {
        return 0;
    }
    return func_800BF79C(D_800E7728, idx);
}

s32 func_8005AF4C(s32 *p) {
    s32 r = 0;
    if (p != NULL) {
        D_800F073C = func_800BF79C(D_800E7728, *p);
        r = D_800F073C != 0;
    }
    return r;
}

u32 func_8005AF90(void) {
    u8 *e = D_800CADE4;
    s16 idx = *(s16 *)(D_800CA930 + 0x64);
    e = e + idx * 40;
    if (e[0xC] & 4) {
        return ((D_800F0600 >> 12) ^ 1) & 1;
    }
    if (*(u32 *)(e + 0x1C) != 0) {
        return 0;
    }
    return ~D_800F05D4_3 >> 31;
}

s32 func_8005AFFC(void)
{
    if (D_800E36F8 == 5) {
        D_800E36F8 = 6;
        D_800E374C = 0x14;
        return 1;
    }
    return 0;
}

typedef struct {
    u8 pad0[0x58];
    s32 w58;
} G_2;

extern G_2 D_800F05A8_34;

void func_8005B030(void) {
    D_800F05A8_34.w58 |= 0x10;
    func_80064DB8();
}

void func_8005B05C(void)
{
    func_80010F40();
    func_8000D0D8();
}

void func_8005B080(void)
{
    s32 *g;

    func_800515A8();
    g = D_800F05A8_3;
    if (g[0x40 / 4] == 0) {
        if ((g[0x58 / 4] & 0x10) == 0) {
            s32 n = g[0x2C / 4] + 1;
            g[0x2C / 4] = n;
            if (n == 0x34BC0) {
                func_80050F7C(0);
                g[0x40 / 4] = 1;
            }
        }
    } else if ((D_800E3770_2 & 0x40) != 0 && (D_800E36E0 & 0x40) == 0) {
        func_8005AFFC();
    }
}

typedef struct {
    u8 pad0[0x48];
    s32 f48;
    u8 pad1[0xC];
    u32 f58;
} Blk_15;

typedef struct {
    u8 pad0[0x69C];
    s32 f69C;
    u8 pad1[0x7E0 - 0x6A0];
} Ent_56;

extern Blk_15 D_800F05A8_35;
extern Ent_56 D_800E37C0_70[];

void func_8005B114(void) {
    Ent_56 *e;

    D_800F05A8_35.f48 = -1;
    D_800F05A8_35.f58 |= 0x10;
    func_80064DB8();
    for (e = D_800E37C0_70; e < &D_800E37C0_70[D_800EFD70_9.count]; e++) {
        e->f69C = 7200;
    }
}

void func_8005B1AC(void)
{
    u32 *p = (u32 *)D_800F05A8_2;
    p[0x58 / 4] = p[0x58 / 4] | 0x10;
    p[0x2C / 4] = 0x1C20;
    func_80064DB8();
}

void func_8005B1E0(void)
{
    u8 *p;
    s32 w;

    p = D_800F05A8_2;
    w = *(s32 *)(p + 0x58);
    *(u16 *)(p + 0x26) = 0xB4;
    *(s32 *)&D_800F05A8_2 = 6;
    *(u8 *)(p + 0x6E) = 0;
    *(u16 *)(p + 0x68) = 0;
    *(u16 *)(p + 0x6A) = 0;
    *(u16 *)(p + 0x6C) = 0;
    *(s32 *)(p + 0x58) = w | 0x10;
}

typedef struct {
    s32 state;
    u8 pad[0x1E];
    s16 count;
    s16 f24;
} Ctl_16;

void func_8005B218(void) {
    Ctl_16 *c = &D_800F05A8;
    s32 i;
    s16 *t;

    if (c->state == 8) {
        func_80064D28(2, 0);
    }
    i = 0;
    c->f24 = 0;
    if (c->count > 0) {
        s16 neg = -1;
        Ctl_16 *d = c;
        t = D_800E3E20;
        do {
            t[4] = neg;
            t += 0x3F0;
        } while (++i < d->count);
    }
}

typedef struct {
    u8 pad[0x668];
    s16 x668;
    u8 pad2[0x7E0 - 0x66A];
} Rider_4;

typedef struct {
    u8 pad[0x66];
    u8 a[1];
} Thing;

extern Rider_4 D_800E37C0_71[];
extern Thing *D_800CA930_13;

void func_8005B28C(void) {
    s32 i;
    for (i = 0; i < D_800F05A8.n; i++) {
        if (D_800E37C0_71[i].x668 != -1) {
            D_800CA930_13->a[i] = 0;
        }
    }
}

typedef struct {
    s32 state;
    s32 pad[10];
    s32 cnt;
} Blk_16;

extern Blk_16 D_800F05A8_36;

void func_8005B2E8(void)
{
    func_800515A8();
    if (D_800F05A8_36.state == 5) {
        if (D_800F05A8_36.cnt != 0) {
            s32 r = D_800F05A8_36.cnt - 1;
            D_800F05A8_36.cnt = r;
            if (r == 0) {
                D_800E3E2A = 0x78;
            }
        }
    }
}

void func_8005B33C(void)
{
    s32 i;
    s32 *p1;
    s32 *p2;

    if (D_800EFD70_4[0x13] == 9) {
        func_800BCB04(D_800F06B0);
    }
    if (D_800EFD70_4[0xF] == 1) {
        func_800BCB04(D_800F0730[0]);
        func_800BCB04(D_800F0730[1]);
    }
    i = 0;
    p2 = D_800F06B8;
    p1 = D_800F0530;
    do {
        i++;
        func_800C204C(*p1);
        func_800BCB04(*p1);
        p1++;
        func_800C204C(*p2);
        func_800BCB04(*p2);
        p2++;
    } while (i < 0x1e);
    return;
}

#define PKT_2(a, b) { Gfx *_g = dl++; _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8005B404(s32 p1, s32 p2, s32 p3, Gfx *dl, s32 p5)
{
    s32 i = 0;
    s32 v;
    u8 *base;
    u8 *pb;

    PKT_2(0xE3001001, 0);
    PKT_2(0xFC119623, 0xFF2FFFFF);
    base = D_800EFD6C;
    pb = base + 0x58;
    dl = (Gfx *)func_8000A690(p1 - p3 * *pb * 8 - base[0x60] * 8, p2, base + 0x60, (u32 *)dl, p5, 1, 1, 6, 0);
    for (i = 0; i < p3; i++) {
        v = p3 - i;
        dl = (Gfx *)func_8000A690(p1 - v * *pb * 8, p2, pb, (u32 *)dl, p5, 1, 1, 6, 0);
    }
    return dl;
}

s32 func_8005B55C(void)
{
    s32 idx = *(s16 *)(D_800CA930 + 0x64);
    return D_800CADE4_12[idx * 10 + 7] != 0;
}

typedef struct Ent_57 {
    s32 key;
    s32 val;
} Ent_57;

s32 func_8005B58C(s32 key)
{
    Ent_57 *p = D_800F073C;
    if (p == NULL) {
        return -1;
    }
    while (p->val >= 0) {
        if (p->key == key) {
            return ((volatile Ent_57 *)p)->val;
        }
        p++;
    }
    return -1;
}
