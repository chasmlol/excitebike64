#include "ctx.h"

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
} Race;

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
} Track;

typedef struct {
    u8 *f0;
    u8 p04[0x60];
    s16 f64;
} Prof;

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
} Bike;

typedef struct {
    u8 p00[0x4A];
    s16 f4A;
    u8 p4C[0x20];
} Seg;

extern Race D_800F05A8;
extern u32 D_800F0600;
extern s32 D_800F0618;
extern Prof *D_800CA930;
extern Track D_800CADE4[];
extern Bike D_800E37C0[];
extern s16 D_800E3DE8;
extern u32 D_800E3E50;
extern Seg *D_800EE498;
extern f32 D_800E43EC;
extern s32 D_800C8CE4;
extern s32 D_800C238C;
extern s32 D_800E30B0;
extern s32 D_800E25B0;
extern s32 D_800E3710;
extern s32 D_800E3768;
extern u8 D_800F0746;
extern s32 D_800F0738;
extern s8 D_800F06AC;
extern u8 D_800CAAA1;
extern u8 D_800F1BC1;
extern s32 D_800CA920;
extern s32 D_800EFD70;
extern s32 D_800E36F8;
extern s32 D_800E374C;
extern s32 D_800C8CDC;
extern s32 D_800E37A8;
extern s32 D_800C8CF0;

void func_800515A8(void);
void func_80010F40(void);
void func_8000D0D8(void);
void func_8004FBE8(void);
void func_800229B8(void);
void func_8005E9BC(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8004F9B0();
void func_8001A190(void);
void func_8005FE10(s32);
s32 func_8005AFFC(void);
u8 func_8005AF90(void);

#define G D_800F05A8
#define TB (D_800CADE4[D_800CA930->f64])
#define SEG(i) ((Seg *)((s32)((i) * 0x6C) + (s32)D_800EE498))

void func_8004FCBC(void)
{
    s32 i;

    D_800E43EC = 1.0375f;
    if (!(G.f58 & 0x800)) {
        if ((TB.fl & 8) && (G.state < 5 || G.state == 15 || G.state == 14)) {
            func_800515A8();
        }
        if (G.f26 == 0x2B && G.state == 1 && (G.f58 & 0x8000)) {
            G.f80 = 300;
            G.f58 |= 0x10800;
        }
    } else {
        func_80010F40();
    }
    if (G.state == 5) {
        G.f58 &= ~0x10;
        if (D_800C8CE4 != 0 && D_800C238C != 0xFF) {
            G.f70 = 1;
            G.state = 14;
        } else {
            G.state = 15;
            G.f70 = 5;
        }
    }
    if (G.state >= 5 && !(G.f58 & 0x800)) {
        if (D_800C8CE4 == 0) {
            if (TB.fl & 2) {
                G.f2c++;
            } else if (!(D_800E3E50 & 0x10)) {
                G.f2c--;
            }
        } else {
            for (i = 0; i < TB.n; i++) {
                if (G.f2c == TB.list[i].v) {
                    func_80010F40();
                    func_8000D0D8();
                    G.f80 = 1800;
                    G.f58 |= 0x800;
                    break;
                }
            }
            G.f2c++;
        }
        if (G.f40 == 0) {
            G.f28 = 180;
        } else if (G.f28 != 0) {
            G.f28--;
        }
    }
    if ((G.f70 == 0 || G.f70 == 2) && G.f78 != 0) {
        G.f78--;
    }
    if (G.state < 5) {
        func_8004FBE8();
        return;
    }
    switch (G.state) {
    case 13:
        switch (G.f70) {
        case 0:
            if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
                G.f70 = 2;
                G.f78 = 59 - G.f78;
            } else if (G.f78 == 0) {
                G.f70 = 1;
            }
            break;
        case 1:
            if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
                G.f70 = 2;
                G.f78 = 60;
                G.f58 = (G.f58 & ~0x10) | 0x40;
            } else {
                func_8004FBE8();
            }
            break;
        case 2:
            if (G.f78 == 0) {
                G.state = 14;
                G.f26 = 0x3F;
                G.f70 = 0;
                G.f78 = 60;
            }
            break;
        }
        break;
    case 14:
        switch (G.f70) {
        case 0:
            if (G.f78 == 0) {
                G.f70 = 1;
                return;
            }
            break;
        case 1:
            if ((D_800E30B0 & 0x40) && !(D_800E25B0 & 0x40)) {
                G.f70 = 6;
                D_800E36F8 = 6;
                D_800E374C = -1;
                D_800C8CE4 = 0;
                D_800C8CDC = 0;
                D_800E37A8 = 0;
                D_800C8CF0 = 0;
                return;
            }
            if (D_800E37C0[0].f628 == TB.f1A) {
                if (D_800E37C0[0].f690 & 0x10) {
                    if (G.f58 & 0x2000) {
                        func_800229B8();
                        func_8005E9BC();
                        func_8000ED10((u8 *)D_800E37C0, 0, sizeof(Bike) * 8);
                        func_8004F9B0(0);
                        func_8001A190();
                        func_8005FE10(D_800C8CE4);
                        G.f58 = (G.f58 | 0x80) & ~0x2000;
                    }
                } else {
                    D_800E37C0[0].f690 |= 0x10;
                    G.f58 &= ~0x2000;
                }
                return;
            }
            if ((TB.fl & 4) && D_800E37C0[1].f628 >= TB.f1A &&
                (TB.f18 < TB.f1A || (D_800E37C0[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
                D_800E37C0[1].f690 |= 0x10;
                return;
            }
            break;
        case 5:
            if (D_800E37C0[0].f628 == TB.f1A) {
                D_800E37C0[0].f690 |= 0x10;
            }
            if ((G.f58 & 0x2000) && D_800E3710 != 5) {
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
            if (G.f2c >= 3 && D_800F0738 == 0) {
                D_800F0738 = 2;
            }
            if (D_800E37C0[0].f3FB == 0 && D_800E37C0[0].f3D2 == TB.list[D_800F0738].v) {
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
                            D_800CA930->f0[1] = TB.fD + 1;
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
                } else if (!(D_800E37C0[0].f690 & 0x10)) {
                    D_800E37C0[0].f690 |= 0x10;
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
        if ((u32)(D_800E37C0[0].f628 - TB.f1A) < 10 || (G.f58 & 0x2000)) {
            if (D_800E37C0[0].f690 & 0x10) {
                if (D_800CA930->f64 < 16 && TB.fD != D_800CADE4[D_800CA930->f64 + 1].fD &&
                    func_8005AF90()) {
                    if (TB.fD == D_800CA930->f0[1]) {
                        D_800CA930->f0[1] = TB.fD + 1;
                        D_800F1BC1 = D_800CAAA1;
                    }
                }
                if ((G.f58 & 0x2000) && D_800F0738 == 0) {
                    func_80010F40();
                    G.f58 &= ~0x2000;
                    D_800F06AC = func_8005AF90() ? 1 : -1;
                    D_800E3710 = 5;
                    D_800E3768 = func_8005AF90() ? 3 : 0;
                }
            } else {
                D_800E37C0[0].f690 |= 0x10;
            }
        } else if (D_800E37C0[0].f628 == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            G.f70 = 6;
            D_800E36F8 = 6;
            D_800E374C = -1;
            D_800C8CE4 = 0;
            D_800C8CDC = 0;
            D_800E37A8 = 0;
            D_800C8CF0 = 0;
        } else if ((TB.fl & 4) && D_800E37C0[1].f628 >= TB.f1A &&
                   (TB.f18 < TB.f1A || (D_800E37C0[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
            D_800E37C0[1].f690 |= 0x10;
        }
        break;
    }
}
