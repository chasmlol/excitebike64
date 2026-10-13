#include "ctx.h"

typedef struct {
    u8 pad0[6];
    s16 f6;
    u8 pad8[4];
    s32 fC;
    s16 f10;
} Menu;

typedef struct Node {
    s32 f0;
    struct Node *prev;
    struct Node *next;
    s32 fC;
    u16 f10;
} Node;

typedef struct {
    s16 w, h;
} Dim;

typedef struct {
    s16 f0;
    u8 pad2[0x22];
} Slot;

typedef struct {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    s32 f174;
    s32 f178;
    Node *f17C;
    Node *f180;
    Node *f184;
    u8 pad188[8];
    s32 f190;
    Slot slots[150];
    Dim *f16AC;
    u8 pad16B0[6];
    s16 f16B6;
    s16 f16B8;
    char name[11];
    u8 f16C5[0xB];
    s32 f16D0, f16D4, f16D8, f16DC;
    Menu *f16E0;
    Menu *f16E4;
    u32 f16E8;
    u8 pad16EC[8];
    s32 f16F4;
    u8 pad16F8[0x2C];
    s32 f1724;
    s32 f1728;
    u8 pad172C[0xCC];
    s32 f17F8;
    s16 f17FC;
    s16 f17FE;
    s16 f1800;
} Game;

typedef struct {
    s32 f0;
    u8 pad4[0x38];
    s32 f3C;
    u8 pad40[0xC];
    s32 f4C;
    u8 pad50[0x398];
    u8 *f3E8;
    u8 pad3EC[0x24];
    s32 f410;
} Sys;

typedef struct {
    u8 pad0[0x2C4];
    u8 f2C4;
    u8 pad2C5[0x22F];
    u8 f4F4[0x310];
} Save;

typedef struct {
    s32 a, b;
} Pair;

typedef struct {
    Pair p;
    u32 f8;
} Cam;

extern Game *D_800CEA40;
extern s32 D_800E30B0[4];
extern s32 D_800E25B0[4];
extern Save D_800F1900;
extern u8 D_800F1DF4[];
extern Sys D_800EFD70;
extern char D_800DAB1C[];
extern Menu D_800CEB9C;
extern Menu D_800CEB08;
extern s16 D_800CEB18;
extern s32 D_800CE240;
extern Cam D_800CE21C;
extern Cam D_800CE210;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8006F5F8(s32 x, s32 y);
u32 func_80060170(u8 *param_1, s32 param_2);
void func_8009EB4C(void);
void func_8001102C(s32 param_1);
void func_8008D3B4(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void func_8008E76C(s32 param_1, s32 param_2, s32 param_3);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
s16 func_800A42D0(u8 *p);
void func_800A4700(u8 *p);
void func_80094738(Node *n);
void func_800BCB04(u8 *p);
void func_80096B3C(Node *n);
void func_80090670(void);
void func_80091564(Game *g, s32 flags);
s32 func_8008F878(Menu *m, s32 flags);

void func_80091AFC(void)
{
    Game *p = D_800CEA40;
    s32 flags = p->f174;
    s32 i, j, k, n;

    if (flags & 0x800) {
        if ((D_800E30B0[0] & 0x10) && !(D_800E25B0[0] & 0x10)) {
            D_800E25B0[0] |= 0x10;
            D_800CEA40->f174 &= ~0x800;
            D_800CEA40->f16E0->fC |= 1;
            func_8001102C(1);
        }
    } else if (flags & 0x1000) {
        if ((D_800E30B0[0] & 0x20) && !(D_800E25B0[0] & 0x20)) {
            D_800E25B0[0] |= 0x20;
            D_800CEA40->f174 &= ~0x1000;
            D_800CEA40->f16E0->fC |= 1;
            func_8001102C(2);
        } else if ((D_800E30B0[0] & 0x10) && !(D_800E25B0[0] & 0x10)) {
            D_800E25B0[0] |= 0x10;
            func_8001102C(1);
            if (D_800CEA40->f17FE) {
                switch (D_800CEA40->f17FC) {
                case 7:
                    D_800CEA40->f17FC = 6;
                    if (D_800F1900.f2C4 & 8) {
                        D_800CEA40->f17FE = 1;
                    } else {
                        D_800CEA40->f17FE = 0;
                    }
                    break;
                case 6:
                    if (D_800F1900.f2C4 & 8) {
                        for (j = 0; j < D_800CEA40->f16AC->h; j++) {
                            for (i = 0; i < D_800CEA40->f16AC->w; i++) {
                                func_8008D3B4(i, j, 0, 0, 0, 0, 0, 0, 0);
                            }
                        }
                        D_800CEA40->f16F4 = 8;
                        func_800A4700(D_800F1DF4);
                        func_80094738(D_800CEA40->f180);
                        D_800CEA40->f170 = 0;
                        D_800CEA40->f16E0->fC |= 1;
                        func_8009EB4C();
                        func_8008E76C(D_800CEA40->f17F8, D_800CEA40->f1724, D_800CEA40->f1728);
                        D_800CEA40->f16E0->fC |= 1;
                        D_800CEA40->f174 &= ~0x1000;
                    } else {
                        D_800CEA40->f1800 = 0;
                        D_800CEA40->f174 |= 0x800;
                    }
                    break;
                case 8:
                    D_800EFD70.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x6FE);
                    func_800A42D0(D_800EFD70.f3E8);
                    __builtin_memcpy(D_800F1DF4, D_800EFD70.f3E8, 0x310);
                    func_8006F5F8(4, 0);
                    func_800BCB04(D_800EFD70.f3E8);
                    D_800EFD70.f3E8 = 0;
                    D_800CEA40->f16E0->fC |= 1;
                    D_800CEA40->f174 &= ~0x1000;
                    break;
                case 5:
                    if (D_800F1900.f2C4 & 8) {
                        D_800CEA40->f17FC = 8;
                        D_800CEA40->f17FE = 0;
                    } else {
                        D_800EFD70.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x6FE);
                        func_800A42D0(D_800EFD70.f3E8);
                        __builtin_memcpy(D_800F1900.f4F4, D_800EFD70.f3E8, 0x310);
                        func_8006F5F8(4, 0);
                        func_800BCB04(D_800EFD70.f3E8);
                        D_800EFD70.f3E8 = 0;
                        D_800CEA40->f16E0->fC |= 1;
                        D_800CEA40->f174 &= ~0x1000;
                    }
                    break;
                case 3:
                    D_800EFD70.f0 |= 2;
                    D_800CEA40->f178 = 0;
                    D_800CEA40->f174 |= 0x10;
                    D_800CEA40->f16E0->fC |= 1;
                    D_800CEA40->f174 &= ~0x1000;
                    break;
                case 4:
                    D_800CEA40->f17C = 0;
                    D_800CEA40->f180 = 0;
                    D_800CEA40->f184 = 0;
                    D_800CEA40->f190 = 0;
                    D_800CEA40->f16B6 = 0;
                    D_800CEA40->f16B8 = 0;
                    for (k = 0; k < 150; k++) {
                        D_800CEA40->slots[k].f0 = 0;
                    }
                    D_800CEA40->f16F4 = 8;
                    __builtin_strcpy(D_800CEA40->name, "<NONE>");
                    for (j = 0; j < D_800CEA40->f16AC->h; j++) {
                        for (i = 0; i < D_800CEA40->f16AC->w; i++) {
                            func_8008D3B4(i, j, 0, 0, 0, 0, 0, 0, 0);
                        }
                    }
                    D_800CEA40->f184 = 0;
                    D_800CEA40->f17C = 0;
                    D_800CEA40->f180 = D_800CEA40->f184;
                    func_80096B3C(D_800CEA40->f184);
                    D_800CEA40->f16E0->fC |= 1;
                    D_800CEA40->f17F8 = 0;
                    D_800CEA40->f174 &= ~0x1000;
                    func_8008E76C(0, D_800CEA40->f1724, D_800CEA40->f1728);
                    func_80090670();
                    D_800CE240 = 0;
                    D_800CE21C.p.a = D_800CE210.p.a;
                    D_800CE21C.p.b = D_800CE210.p.b;
                    D_800CE21C.f8 = ((D_800CE21C.f8 + 0x800) >> 13) << 13;
                    break;
                }
            } else {
                switch (D_800CEA40->f17FC) {
                case 8:
                    D_800CEA40->f1800 = 1;
                    D_800CEA40->f174 = (D_800CEA40->f174 & ~0x1000) | 0x800;
                    break;
                case 6:
                    D_800EFD70.f0 |= 2;
                    D_800CEA40->f178 = 0;
                    D_800CEA40->f174 = (D_800CEA40->f174 | 0x2010) & ~0x4000;
                    D_800CEA40->f16E0->fC |= 1;
                    D_800CEA40->f174 &= ~0x1000;
                    break;
                case 5:
                    D_800EFD70.f0 |= 2;
                    D_800CEA40->f178 = 0;
                    D_800CEA40->f174 = (D_800CEA40->f174 | 0x4010) & ~0x2000;
                    D_800EFD70.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x7B3);
                    for (n = func_800A42D0(D_800EFD70.f3E8 + 4) + 4; n < 0x400; n++) {
                        D_800EFD70.f3E8[n] = 0;
                    }
                    *(u32 *)D_800EFD70.f3E8 = func_80060170(D_800EFD70.f3E8 + 4, 0x3FC);
                default:
                    D_800CEA40->f16E0->fC |= 1;
                    D_800CEA40->f174 &= ~0x1000;
                    break;
                }
            }
        } else if (((D_800E30B0[0] & 0x800) && !(D_800E25B0[0] & 0x800)) ||
                   ((D_800E30B0[0] & 0x1000) && !(D_800E25B0[0] & 0x1000))) {
            if ((D_800CEA40->f17FC != 6 || (D_800F1900.f2C4 & 8)) && D_800CEA40->f17FC != 5) {
                D_800CEA40->f17FE ^= 1;
                func_8001102C(0);
            }
        }
    } else {
        if (p->f184 != NULL && p->f170 != 7) {
            if (((D_800E30B0[0] & 0x2000) && !(D_800E25B0[0] & 0x2000) && !(D_800CEA40->f174 & 1)) ||
                ((D_800E30B0[0] & 0x4000) && !(D_800E25B0[0] & 0x4000) && (D_800CEA40->f174 & 1))) {
                if (D_800CEA40->f184->next) {
                    D_800CEA40->f184 = D_800CEA40->f184->next;
                }
            } else if (((D_800E30B0[0] & 0x4000) && !(D_800E25B0[0] & 0x4000) && !(D_800CEA40->f174 & 1)) ||
                       ((D_800E30B0[0] & 0x2000) && !(D_800E25B0[0] & 0x2000) && (D_800CEA40->f174 & 1))) {
                if (D_800CEA40->f184->prev) {
                    D_800CEA40->f184 = D_800CEA40->f184->prev;
                }
            }
        }
        if (D_800CEA40->f170 == 7 || D_800CEA40->f170 == 8) {
            func_80091564(p, flags);
        } else if (func_8008F878(D_800CEA40->f16E0, flags)) {
            switch (D_800CEB18) {
            case 1:
                D_800CEA40->f170 = 0;
                if (D_800CEA40->f16E0 != &D_800CEB9C) {
                    switch (D_800CEA40->f16E8) {
                    case 1:
                        D_800CEA40->f16E0 = &D_800CEB9C;
                        break;
                    case 0:
                        D_800CEA40->f16E8 = 1;
                        D_800CEA40->f16E4 = &D_800CEB9C;
                        break;
                    case 2:
                        D_800CEA40->f16E8 = 3;
                        D_800CEA40->f16E4 = &D_800CEB9C;
                        break;
                    case 3:
                        D_800CEA40->f16E8 = 1;
                        D_800CEA40->f16E0 = &D_800CEB9C;
                        break;
                    }
                    D_800CEA40->f174 &= ~0x10000;
                }
                D_800CEB9C.f10 = 0;
                D_800CEB9C.f6 = 0;
                D_800CEA40->f16C = 1;
                break;
            case 0:
                D_800CEA40->f17FC = 4;
                D_800CEA40->f174 |= 0x1000;
                D_800CEA40->f16E0->fC &= ~1;
                D_800CEA40->f17FE = 0;
                break;
            case 2:
                D_800CEA40->f174 |= 0x1000;
                if (D_800CEA40->f184 != NULL) {
                    D_800CEA40->f17FC = 7;
                    D_800CEA40->f17FE = 0;
                } else {
                    D_800CEA40->f17FC = 6;
                    if (D_800F1900.f2C4 & 8) {
                        D_800CEA40->f17FE = 1;
                    } else {
                        D_800CEA40->f17FE = 0;
                    }
                }
                D_800CEA40->f16E0->fC &= ~1;
                break;
            case 3:
                if (D_800CEA40->name[0] == '<') {
                    D_800CEA40->f174 |= 0x800;
                    D_800CEA40->f16E0->fC &= ~1;
                    D_800CEA40->f1800 = 4;
                    D_800CEB08.f10 = 6;
                    D_800CEB08.f6 = 6;
                } else {
                    D_800CEA40->f17FC = 5;
                    D_800CEA40->f174 |= 0x1000;
                    D_800CEA40->f16E0->fC &= ~1;
                    D_800CEA40->f17FE = 0;
                }
                break;
            case 5:
                if (++D_800CEA40->f17F8 == 3) {
                    D_800CEA40->f17F8 = 0;
                }
                func_8008E76C(D_800CEA40->f17F8, D_800CEA40->f1724, D_800CEA40->f1728);
                break;
            case 7:
                D_800CEA40->f17FC = 3;
                D_800CEA40->f17FE = 0;
                D_800CEA40->f174 = (D_800CEA40->f174 & ~0x6000) | 0x1000;
                D_800CEA40->f16E0->fC &= ~1;
                break;
            case 6:
                D_800CEA40->f16DC = 0;
                D_800CEA40->f16D0 = 0;
                D_800CEA40->f16D4 = 0;
                D_800CEA40->f16D8 = 0;
                func_8000ED10(D_800CEA40->f16C5, 0, 0xB);
                D_800CEA40->f170 = 7;
                D_800CEA40->f16E0->fC &= ~1;
                break;
            case 4:
                if (D_800EFD70.f410 == 0) {
                    D_800EFD70.f410 = 1;
                }
                if (D_800CEA40->f17C->next == D_800CEA40->f180) {
                    D_800EFD70.f4C = 0xD;
                } else {
                    D_800EFD70.f4C = 0xE;
                }
                D_800EFD70.f3C = 1;
                D_800EFD70.f0 &= ~2;
                D_800CEA40->f174 &= ~0x6000;
                D_800EFD70.f0 |= 0x4000000;
                D_800CEA40->f174 |= 0x410;
                D_800CEA40->f178 = 0;
                D_800CEA40->f174 &= ~0x1000;
                if (D_800EFD70.f3E8 == NULL) {
                    D_800EFD70.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x88C);
                    for (n = func_800A42D0(D_800EFD70.f3E8 + 4) + 4; n < 0x400; n++) {
                        D_800EFD70.f3E8[n] = 0;
                    }
                }
                break;
            }
        }
    }
    if (D_800CEA40->f184 != NULL) {
        if (D_800CEA40->f184->f10 & 2) {
            D_800CEA40->f174 |= 8;
        } else {
            D_800CEA40->f174 &= ~8;
        }
    }
}
