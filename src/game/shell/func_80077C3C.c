#include "ctx.h"

typedef struct { u32 w[4]; } Pad4;

typedef struct {
    u8 pad0[4];
    s32 p4;
    u16 h8;
    u8 padA[6];
} Ctl;

typedef struct Item {
    u8 pad0[4];
    struct Item *p4;
    struct Item *p8;
    Ctl *pC;
    u16 h10;
    u8 pad12[2];
    s16 h14;
    s16 h16;
    u8 pad18[0x20 - 0x18];
    s32 *p20;
    u8 pad24[4];
    s32 p28;
} Item;

typedef struct {
    u32 w0;
    u8 pad4[0x10 - 4];
    s32 w10;
    s32 w14;
    u8 pad18[0x4C - 0x18];
    s32 w4C;
    u8 pad50[4];
    u32 w54;
    s16 h58;
    s16 h5A;
    u8 pad5C[0x3E8 - 0x5C];
    u8 *w3E8;
    u8 pad3EC[0x488 - 0x3EC];
    s16 h488;
    u8 pad48A[0x740 - 0x48A];
    s32 w740;
} Game;

typedef struct { u8 pad[0xA0]; } Rec;
typedef struct { u8 pad0[8]; s16 h8[1]; } Trk;
typedef struct { Trk *p0; u8 pad4[0x64 - 4]; s16 h64; } TrkSel;
typedef struct { u8 pad[0x38]; u16 h38; } S370;
typedef struct { u8 pad[0x10]; s32 w10; } S29CC;

extern Game D_800EFD70;
extern Rec *D_800EFD64;
extern Rec D_800CB210[];
extern Item D_800CD63C[];
extern Item *D_800F47A4;
extern Item *D_800F47C4;
extern Pad4 D_800F47B0;
extern s32 D_800F47A0;
extern s32 D_800E2038;
extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800C292E;
extern S370 D_800CD370;
extern s32 D_800F4A04;
extern s32 D_800F4AF4;
extern u8 *D_800C8CE4;
extern u8 *D_800E203C;
extern s32 D_800E3704;
extern s32 D_800E3748;
extern S29CC *D_800C29CC;
extern TrkSel *D_800CA930;
extern s32 D_800D11D4;
extern u8 D_800CD090[];

s32 func_8004F180(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80010DF8(u32 a);
s32 func_80060078(u8 *s, s32 *a1, s32 *a2, s32 *a3);
void func_8001102C(s32 param_1);
void func_80077BA0(u32 *p);
void func_8008BE9C(void);
s32 func_8006F128(void);
void func_80010AFC(void);
void func_8004C22C(s32 param_1);
void func_8004C3C4(void);
void func_80017338(u8 *p);
void func_8007AF38(s32 p1, s32 p2, s32 p3);
void func_8007417C(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80074298(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80074444(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80074684(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80074EF4(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_800752A0(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80075764(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80075F3C(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_8007658C(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_800770F0(Item *a, Ctl *b, u32 *c, Pad4 *d);
void func_80089F2C(Pad4 *p);
void func_80078E54(Item *p);
void func_80079E00(Item *p);
void func_800A44EC(u8 *p);
void func_800BCB04(u8 *p);

void func_80077C3C(Pad4 *pad) {
    u32 buf[4];
    s32 sp20;
    s32 sp24;
    s32 sp28;
    Item *item;
    Ctl *ctl;
    Item *p;
    s32 vol;
    s32 i;
    s32 any;

    func_8000ED10((u8 *)buf, 0, 16);
    if (!(D_800E2038 & 0xF)) {
        func_8008BE9C();
    }
    if (D_800C292E || !D_800C292C) {
        D_800EFD70.w0 |= 0x08000000;
    } else {
        D_800EFD70.w0 &= ~0x08000000;
    }
    if (D_800C292D < 2) {
        D_800CD370.h38 |= 1;
    } else {
        func_8006F128();
    }
    if (D_800EFD70.w54 == 0) {
        item = D_800F47A4;
        if (item == &D_800CD63C[5] ||
            (!(item->h10 & 0x200) && (D_800F47C4 == NULL || (D_800F47C4->h10 & 0x200)))) {
            if (D_800EFD70.w54 == 0) {
                vol = D_800EFD70.h58;
            }
            func_80010DF8(vol);
        }
    } else {
        item = D_800F47C4;
    }
    if (item != NULL && (item->h10 & 0x2000)) {
        func_80077BA0(pad->w);
    }
    any = 0;
    for (i = 0; i < 4; i++) {
        if (((D_800C292C >> i) & 1) && pad->w[i] != 0) {
            any = 1;
        }
        if ((((s32)pad->w[i] >> 24) & 0xF) < 6) {
            pad->w[i] &= ~0xC;
        }
        if ((s32)(pad->w[i] >> 28) < 6) {
            pad->w[i] &= ~3;
        }
    }
    if (any) {
        D_800EFD70.w740 = 0;
    } else {
        D_800EFD70.w740++;
    }
    for (i = 0; i < 4; i++) {
        buf[i] = 0;
        if (D_800EFD70.w54 == 3) {
            buf[i] = pad->w[i] & ~D_800F47B0.w[i];
            if (buf[i] & 0x800) {
                buf[i] |= 1;
            }
            if (buf[i] & 0x1000) {
                buf[i] |= 2;
            }
            if (buf[i] & 0x2000) {
                buf[i] |= 4;
            }
            if (buf[i] & 0x4000) {
                buf[i] |= 8;
            }
        }
    }
    if (item != NULL) {
        if (D_800EFD70.w54 == 0) {
            ctl = &item->pC[D_800F47A0];
        } else {
            ctl = item->pC;
            if (item->p20 != NULL) {
                ctl += *item->p20 + item->h14;
            }
        }
        if (D_800EFD70.w54 != 1) {
            switch (item->h16) {
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                func_8007417C(item, ctl, buf, pad);
                break;
            case 9:
            case 20:
                func_80074684(item, ctl, buf, pad);
                break;
            case 18:
                func_80074444(item, ctl, buf, pad);
                break;
            case 7:
            case 13:
                func_80074EF4(item, ctl, buf, pad);
                break;
            case 14:
                func_800752A0(item, ctl, buf, pad);
                break;
            case 8:
                if (buf[0] & 0x50) {
                    func_8007AF38((s32)&D_800CD63C[8], 0, 1);
                    func_8001102C(1);
                } else if ((buf[0] & 0x20) && item->p28 != 0) {
                    func_8007AF38(item->p28, 0, 0);
                    func_8001102C(2);
                }
                break;
            case 3:
                if (item - D_800CD63C == 8) {
                    item->p28 = (s32)&D_800CD63C[14];
                    if (D_800EFD70.w740 > 0xE10 && D_800EFD70.w54 == 3) {
                        func_8007AF38((s32)&D_800CD63C[14], 0, 0);
                    }
                }
            case 11:
                func_800770F0(item, ctl, buf, pad);
                if (item->h16 == 11) {
                    D_800EFD70.w10 = item->h14 + D_800EFD70.w14;
                }
                break;
            case 19:
                func_80074298(item, ctl, buf, pad);
                break;
            case 12:
                func_80089F2C(pad);
                if (D_800F4A04 == 1 && D_800EFD70.w54 == 3) {
                    switch (D_800EFD70.h488) {
                    case 0:
                        if (item->p28 == 0) {
                            func_8007AF38((s32)&D_800CD63C[21], 0, 1);
                        } else {
                            func_8007AF38(item->p28, 0, 0);
                        }
                        break;
                    case 1:
                        if (D_800F4AF4 == 0) {
                            if (func_80060078(D_800C8CE4 + 0x6400, &sp20, &sp24, &sp28)) {
                                func_8007AF38(item->p28, 0, 0);
                                D_800C8CE4 = 0;
                                D_800EFD70.w0 &= ~0x80000;
                            } else {
                                D_800EFD70.w0 |= 0x80000;
                                D_800EFD64 = &D_800CB210[sp20];
                                D_800EFD70.w10 = sp20;
                                func_8007AF38((s32)&D_800CD63C[9], 1, 1);
                            }
                        } else {
                            D_800EFD70.w0 &= ~0x80000;
                            D_800C8CE4 = 0;
                            func_8007AF38(item->p28, 0, 0);
                        }
                        break;
                    case 2:
                        func_8007AF38(item->p28, 0, 0);
                        break;
                    case 4:
                        D_800EFD70.w0 |= 0x20;
                        func_8007AF38(0, 0, 1);
                        break;
                    case 3:
                        if (D_800EFD70.w0 & 0x02000000) {
                            D_800EFD70.w0 &= ~0x20;
                            if (D_800F4AF4 != 0) {
                                func_8007AF38(item->p28, 0, 0);
                            } else {
                                D_800EFD64 = &D_800CB210[21];
                                func_8007AF38((s32)&D_800CD63C[9], 1, 1);
                                func_800A44EC(D_800EFD70.w3E8 + 4);
                                D_800EFD70.w0 &= ~0x80;
                            }
                            D_800EFD70.w4C = 0;
                            func_800BCB04(D_800EFD70.w3E8);
                            D_800EFD70.w3E8 = 0;
                        } else {
                            if (D_800F4AF4 != 0) {
                                func_800BCB04(D_800EFD70.w3E8);
                                D_800EFD70.w3E8 = 0;
                            }
                            D_800EFD70.w0 |= 0x20;
                            func_8007AF38(0, 0, 1);
                        }
                        break;
                    }
                }
                break;
            case 6:
                func_80075764(item, ctl, buf, pad);
                break;
            case 0:
                func_80017338(D_800CD090);
                if (D_800EFD70.w740 > 0xE10 && D_800EFD70.w54 == 3 &&
                    !(D_800EFD70.w0 & 0x08000000)) {
                    func_8007AF38((s32)&D_800CD63C[8], 1, 1);
                }
                break;
            case 2:
                func_80075F3C(item, ctl, buf, pad);
                break;
            case 4:
                func_8007658C(item, ctl, buf, pad);
                break;
            }
            if (D_800EFD70.w54 == 3 && (item->h10 & 0x10)) {
                if (item->h16 == 0 && D_800E3704 != 0) {
                    func_8007AF38(item->pC->p4, 1, 1);
                } else if (buf[0] & 0x50) {
                    if ((item->h16 != 0 || !(D_800EFD70.w0 & 0x08000000)) && !(ctl->h8 & 1)) {
                        func_8001102C(1);
                        func_8007AF38(ctl->p4, 1, 1);
                    }
                } else if (buf[0] & 0x20) {
                    if (item->p28 != 0) {
                        func_8001102C(2);
                        func_8007AF38(item->p28, 0, 0);
                    } else {
                        func_8001102C(3);
                    }
                }
            }
        }
    }
    switch (D_800EFD70.w54) {
    case 0:
        if (D_800EFD70.h58 == 0) {
            D_800EFD70.h58 = 30;
            D_800EFD70.w54 = 1;
            D_800EFD70.h5A = 2;
        } else {
            D_800EFD70.h58 -= 16;
            if (D_800EFD70.h58 <= 0) {
                D_800EFD70.h58 = 0;
            }
        }
        break;
    case 1:
        if (D_800EFD70.h5A == 0) {
            func_80010AFC();
            func_80010DF8(0xFF);
            if (D_800F47A4 != NULL) {
                if (func_8004F180() && (D_800F47C4 == NULL || (D_800F47C4->h10 & 0x200))) {
                    func_8004C3C4();
                }
                for (p = D_800F47A4; p != NULL; p = p->p8) {
                    func_80078E54(p);
                }
                for (p = D_800F47A4->p4; p != NULL; p = p->p4) {
                    func_80078E54(p);
                }
            }
            if (D_800F47C4 != NULL) {
                for (p = D_800F47C4; p != NULL; p = p->p8) {
                    p->p28 = D_800F47C4->p28;
                    func_80079E00(p);
                }
                for (p = D_800F47C4->p4; p != NULL; p = p->p4) {
                    p->p28 = D_800F47C4->p28;
                    func_80079E00(p);
                }
                if (!func_8004F180() && !(D_800F47C4->h10 & 0x200)) {
                    func_8004C22C(D_800D11D4);
                    func_80010DF8(0xFF);
                }
                if (D_800F47C4->h16 == 11) {
                    while (D_800F47C4->p4 != NULL) {
                        D_800F47C4 = D_800F47C4->p4;
                    }
                    for (i = D_800EFD70.w10 - 5; i >= 0; i -= 5) {
                        D_800F47C4 = D_800F47C4->p8;
                    }
                    *D_800F47C4->p20 = D_800EFD70.w10 % 5;
                }
            } else {
                D_800C29CC->w10 = 3;
                if (D_800EFD70.w4C == 9) {
                    D_800EFD64 = &D_800CB210[26];
                } else if (D_800EFD70.w4C == 1) {
                    D_800EFD64 = &D_800CB210[D_800CA930->p0->h8[D_800CA930->h64]];
                }
                if (!(D_800EFD70.w0 & 0x8000)) {
                    if (!(D_800EFD70.w0 & 0x20)) {
                        if (D_800EFD70.w4C == 8) {
                            if (D_800C8CE4 == 0) {
                                D_800C8CE4 = D_800E203C;
                            }
                            D_800E3748 = 0x6400;
                        } else if (D_800EFD70.w4C != 9) {
                            D_800E3748 = 0xC800;
                            D_800C8CE4 = D_800E203C;
                        }
                    }
                }
            }
            D_800EFD70.h5A = -1;
        }
        if (--D_800EFD70.h58 == 0) {
            if (D_800EFD70.h5A != -2) {
                D_800EFD70.h58++;
            } else {
                D_800EFD70.w54 = 2;
                D_800EFD70.h58 = 255;
            }
        }
        break;
    case 2:
        D_800EFD70.h58 -= 16;
        if (D_800EFD70.h58 <= 0) {
            D_800EFD70.w54 = 3;
            D_800EFD70.h58 = 0;
        }
        break;
    }
    D_800F47B0 = *pad;
}
