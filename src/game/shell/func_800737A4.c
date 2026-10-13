#include "ctx.h"

typedef struct {
    s32 w[7];
} Rec1C;

typedef struct {
    s32 w0;
    s32 w4;
    s32 w8;
    s32 wC;
    s32 w10;
    s32 w14;
    s32 w18;
    s32 w1C;
    s32 w20;
    s32 w24;
    s32 w28;
    s32 w2C;
    s32 w30;
    u8 pad34[8];
    s32 w3C;
    s32 w40;
    s32 w44;
    s16 h48;
    s16 h4A;
    s32 w4C;
    u8 b50;
    u8 b51;
    u8 b52;
    u8 b53;
    s32 w54;
    u8 pad58[0x13C - 0x58];
    s32 w13C;
    u8 pad140[0x2A4 - 0x140];
    u8 b2A4;
    u8 b2A5;
    u8 pad2A6[0x3C0 - 0x2A6];
    s32 w3C0;
    u8 pad3C4[0x3FC - 0x3C4];
    void *p3FC;
    s32 w400;
    s32 w404;
    u8 pad408[0x43C - 0x408];
    u8 b43C[6];
    u8 pad442[0x448 - 0x442];
    s16 h448;
    s16 h44A;
    s32 w44C;
    u8 pad450[0x490 - 0x450];
    Rec1C rec[25];
    u8 pad74C[0x75C - 0x74C];
    s32 w75C;
    u8 b760[4];
    u8 pad764[0x7AA - 0x764];
    u8 b7AA[6];
} Game;

typedef struct {
    u8 pad0[0x34];
    Rec1C r34;
    s32 w50;
    u8 pad54[0xA0 - 0x54];
} Course;

typedef struct {
    u8 pad0[4];
    void *p4;
} Link;

typedef struct {
    u8 pad0[0xC];
    Link *pC;
    u8 pad10[0x2C - 0x10];
} Menu;

typedef struct {
    u8 pad0[0x24];
} Item;

typedef struct {
    Item *p0;
    u8 pad4[0x60];
    s16 h64;
} Sel;

typedef struct {
    u8 pad0[0x50];
    s32 w50;
} Opt;

extern s32 D_800C2358;
extern s32 D_800C8CE8;
extern s32 D_800CA920;
extern Game D_800EFD70;
extern s32 D_800C2388;
extern Course D_800CB210[];
extern s16 *D_800C29E0;
extern s32 D_800E7728;
extern s32 D_800CA924;
extern s32 D_800F47C0;
extern s32 D_800F47C4;
extern s32 D_800F47A4;
extern s32 D_800D1438;
extern s16 D_800CDA60[];
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern Opt *D_800EFD64;
extern s32 D_800C8CE4;
typedef struct { u8 pad0[0x174]; s32 w174; } Ctl;
extern Ctl *D_800CEA40;
extern s16 D_800F01F8;
extern Menu D_800CD63C[];
extern s32 D_800CCD64;
extern s32 D_800EFDBC;
extern Sel *D_800CA930;
extern Item D_800CA9C8[];
extern void *D_800F016C;
extern s32 D_800F05E8;
extern s32 D_800C9570;
extern u8 D_800248EC[];

void func_8000ECB8(s32 a) __attribute__((noreturn));
void func_800C0258(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80019E78(void);
void func_80014A80(void);
void func_800882A0(void);
void func_800482D0(u32 *param_1);
void func_800BFDD4(s32 *param_1);
s32 func_800BF530(char *name, s32 *out);
s32 func_800BC908(s32 size, s32 a1, char *file, s32 line);
void func_800AEC34(s32 dst, s16 *src, s32 size);
void func_800BFC40(char *name, u8 *p);
s32 func_800BF79C(s32 a, s32 b);
void func_80073664(s32 a, s32 b);
void func_8007AF38(void *menu, s32 a1, s32 a2);
void func_800BCB04(s32 a);
void func_8006F3E4(s32 a);
void func_8006F128(void);

void func_800737A4(void)
{
    s32 flag;
    s32 mode;
    s32 i;
    Rec1C *dst;
    s32 n;
    Course *c;
    void *m;
    u8 *p;
    u8 *q;

    flag = 0;
    D_800C2358 = 0;
    D_800C8CE8 = 0;
    if (D_800CA920 == 2) {
        flag = flag < (u32)(D_800EFD70.w0 & 8);
    }
    D_800C2388 = 0;
    D_800EFD70.w0 &= ~8;
    if (D_800CA920 == 0) {
        func_8000ED10((u8 *)&D_800EFD70, 0, 0x7B0);
        n = 0;
        for (i = 0; i < 27; i++) {
            s32 f = D_800CB210[i].w50;
            Rec1C *src = &D_800CB210[i].r34;
            if (f & 0x200) {
                dst = &D_800EFD70.rec[n];
                *dst = *src;
                n++;
            }
        }
        D_800EFD70.w75C = 2;
        D_800EFD70.w1C = 4;
        D_800EFD70.w4 = 0;
        D_800EFD70.w10 = 0;
        D_800EFD70.wC = 0;
        D_800EFD70.w20 = 0;
        D_800EFD70.w24 = 1;
        D_800EFD70.w2C = 3;
        D_800EFD70.w30 = 5;
        D_800EFD70.w28 = 2;
        D_800EFD70.w3C = 1;
        D_800EFD70.w40 = -1;
        D_800EFD70.h448 = 0;
        D_800EFD70.h48 = 0;
        D_800EFD70.w8 = 0;
        D_800EFD70.w18 = 0;
        D_800EFD70.b50 = 0;
        D_800EFD70.b51 = 0;
        D_800EFD70.b52 = 0;
        D_800EFD70.b53 = 0;
        D_800EFD70.b2A4 = 0;
        D_800EFD70.b2A5 = 1;
        D_800EFD70.w3C0 = 0;
        D_800EFD70.b760[0] = 2;
        D_800EFD70.b760[1] = 2;
        D_800EFD70.b760[2] = 2;
        D_800EFD70.b760[3] = 2;
        i = 5;
        p = (u8 *)&D_800EFD70 + i;
        do {
            p[0x7AA] = i;
            p--;
            i--;
        } while (i >= 0);
        i = 5;
        q = (u8 *)&D_800EFD70 + i;
        do {
            q[0x43C] = i;
            q--;
            i--;
        } while (i >= 0);
    } else if (D_800EFD70.w4C == 8) {
        D_800EFD70.w3C = 1;
    }
    if (D_800CA920 == 7) {
        D_800EFD70.w0 &= 0xFF0FFFFF;
    }
    D_800EFD70.w54 = 3;
    func_800C0258((s32)D_800C29E0);
    i = func_800BF530("resource", &D_800E7728);
    func_800C0258(0);
    if (i != 0) {
        func_8000ECB8(1);
    }
    func_800482D0((u32 *)D_800E7728);
    func_800BFDD4((s32 *)D_800E7728);
    D_800EFD70.h44A = 0;
    for (i = 0; i < 27; i++) {
        if ((D_800CB210[i].w50 & 0x200) && !(D_800CB210[i].w50 & 2)) {
            D_800C29E0[D_800EFD70.h44A++] = i;
        }
    }
    D_800EFD70.w44C = func_800BC908(D_800EFD70.h44A * 2, 0, "shell.c", 0x75C);
    func_800AEC34(D_800EFD70.w44C, D_800C29E0, D_800EFD70.h44A * 2);
    D_800EFD70.w44 = 5;
    D_800CA924 = 0;
    D_800EFD70.w0 = (D_800EFD70.w0 & 0x20F88000) | 1;
    func_800BF530("guires", &D_800F47C0);
    func_800BFC40("ARN", D_800248EC);
    func_80014A80();
    D_800F47C4 = 0;
    D_800F47A4 = 0;
    D_800EFD70.w404 = func_800BF79C(D_800F47C0, D_800D1438);
    D_800CDA60[2] = 0x1FF;
    D_800CDA60[6] = 0x1FF;
    D_800CDA60[0] = D_800F13EC * 2;
    D_800CDA60[1] = D_800F13F0 * 2;
    D_800CDA60[4] = D_800F13EC * 2;
    D_800CDA60[5] = D_800F13F0 * 2;
    D_800EFD70.w13C = func_800BC908(0x200, 0, "shell.c", 0x777);
    func_80073664(0xFF, 0);
    if (D_800CA920 == 4) {
        D_800EFD64->w50 &= ~0x180;
        if (D_800C8CE4 != 0) {
            D_800C8CE4 = 0;
            D_800EFD70.w0 &= 0xFFF7FFFF;
        }
    }
    if (D_800CEA40 != NULL) {
        if (D_800CEA40->w174 & 0x2000) {
            D_800F01F8 = 3;
            func_8007AF38(&D_800CD63C[15], 0, 1);
        } else {
            D_800F01F8 = 4;
            func_8007AF38(&D_800CD63C[15], 0, 1);
        }
    } else {
        mode = D_800CA920;
        if (mode == 0) {
            func_8007AF38(&D_800CD63C[19], 0, 1);
        } else if ((D_800EFD70.w0 & 0x8000) || D_800CCD64 != 0 || mode == 3) {
            if (D_800C8CE4 != 0) {
                if (!(D_800EFD70.w0 & 0x8000)) {
                    if (!(D_800EFD70.w0 & 0x80000)) {
                        D_800C8CE4 = 0;
                    }
                }
            }
            if (D_800CA920 == 3 && D_800EFDBC == 1) {
                func_800882A0();
                if (D_800CA930->h64 > 0) {
                    D_800CA930->h64--;
                }
            }
            if (D_800EFD70.w0 & 0x8000) {
                func_800882A0();
            }
            if (D_800CCD64 != 0 && D_800EFD70.p3FC != &D_800CD63C[5]) {
                D_800CCD64 = 0;
                func_8007AF38(&D_800CD63C[8], 0, 1);
            } else {
                func_8007AF38(D_800F016C, 0, 1);
            }
            D_800EFD70.w0 &= 0xFEFF7FFF;
        } else if (flag) {
            func_8007AF38(&D_800CD63C[10], 1, 1);
        } else {
            if (D_800F05E8 != 0 && mode != 0 && mode != 6 && D_800EFD70.w4C == 1) {
                if (D_800CA930 != NULL && D_800CA930->p0 - D_800CA9C8 == 4) {
                    m = &D_800CD63C[2];
                } else {
                    m = &D_800CD63C[0];
                }
            } else if ((D_800F05E8 != 0 && D_800CA920 != 0 && D_800CA920 != 6 && D_800EFDBC == 0)
                       || (D_800F05E8 != 0 && D_800CA920 != 0 && D_800CA920 != 6 && D_800EFDBC == 8)) {
                m = &D_800CD63C[1];
            } else {
                if (D_800C8CE4 != 0) {
                    D_800C8CE4 = 0;
                }
                switch (D_800CA920) {
                case 0:
                    m = &D_800CD63C[14];
                    break;
                case 1:
                    m = &D_800CD63C[1];
                    break;
                default:
                    m = &D_800CD63C[8];
                    if (D_800C9570 != 0) {
                        func_800BCB04(D_800C9570);
                        D_800C9570 = 0;
                    }
                    break;
                }
            }
            if (D_800CA920 != 0 && D_800EFD64 != NULL && (D_800EFD64->w50 & 0x180)) {
                D_800CD63C[18].pC->p4 = m;
                func_8007AF38(&D_800CD63C[18], 1, 1);
            } else {
                func_8007AF38(m, 1, 1);
            }
        }
    }
    if (D_800CA920 == 0) {
        func_8006F3E4(0);
    }
    func_8006F128();
    func_80019E78();
}
