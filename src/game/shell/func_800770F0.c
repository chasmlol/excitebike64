#include "ctx.h"

typedef struct Item {
    u8 pad0[4];
    struct Item *p4;
    struct Item *p8;
    u8 padC[0x14 - 0xC];
    s16 h14;
    u8 pad16[2];
    s16 h18;
    u8 pad1A[0x20 - 0x1A];
    s32 *p20;
    u8 pad24[4];
    s32 p28;
} Item;

typedef struct {
    u8 pad0[4];
    s32 p4;
    u16 h8;
} Ctl;

typedef struct {
    s32 w0;
    u8 pad4[0x3C - 0x4];
    s32 w3C;
    u8 pad40[0x4C - 0x40];
    s32 w4C;
    u8 pad50[4];
    s32 w54;
    u8 pad58[0x17C - 0x58];
    s32 w17C;
    u8 pad180[0x29C - 0x180];
    s32 w29C;
    s32 w2A0;
    u8 pad2A4[0x2E8 - 0x2A4];
    s32 w2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 w3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 w410;
    u8 pad414[0x47C - 0x414];
    s16 h47C;
    u8 pad47E[0x488 - 0x47E];
    s16 h488;
    u8 pad48A[0x75C - 0x48A];
    s32 w75C;
} Game;

typedef struct {
    u8 pad0[0x2C0];
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
} S1900;

extern Game D_800EFD70;
extern s32 D_800EFD64;
extern u8 D_800CBF30[];
extern Item D_800CD63C[];
extern f32 D_800CDB5C;
extern f32 D_800CDB60;
extern u32 D_800E30B0;
extern S1900 D_800F1900;
extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800F1DF4[];
extern Item *D_800F47C4;
extern Item *curA_800F47C4;

void func_8001102C(s32 param_1);
void func_800A44EC(u8 *p);
void func_8007AF38(s32 p1, s32 p2, s32 p3);

static inline s32 canGo(Item *n) {
    s32 k;
    if (D_800EFD70.w4C == 8 || D_800EFD70.w4C == 0) {
        k = D_800F1900.b2C0;
    } else {
        k = D_800EFD70.w410;
    }
    switch (k) {
    case 1:
        if (n->h14 < 10) return 1;
        break;
    case 2:
        if (n->h14 < 15) return 1;
        break;
    case 3:
        return 1;
    }
    return 0;
}

void func_800770F0(Item *item, Ctl *ctl, s32 *btn) {
    s32 snd;
    s32 m;
    s32 b;
    s32 s;
    s32 v;
    Item *prev;
    Item *n;
    s32 k;
    s32 ok;

    D_800EFD70.w29C = D_800CDB5C;
    D_800EFD70.w2A0 = D_800CDB60;
    D_800CDB60 -= (D_800E30B0 & 3) ? ((D_800E30B0 & 1) ? (s32)(D_800E30B0 >> 28) / 6.0f : -(s32)(D_800E30B0 >> 28) / 6.0f) : 0.0f;
    if (++D_800EFD70.w17C == 60) D_800EFD70.w17C = 0;
    if (++D_800EFD70.w3EC == 64) D_800EFD70.w3EC = 0;
    b = *btn;
    if (b & 0x50) {
        snd = 0;
        if (!(ctl->h8 & 1)) {
            snd = 1;
        } else {
            func_8001102C(3);
        }
        if (item - D_800CD63C == 8 && (D_800E30B0 & 0x8500) == 0x8500) {
            func_8007AF38((s32)&D_800CD63C[4], 1, 0);
        } else if ((ctl->h8 & 0x800) && D_800EFD70.w2E8 != 8) {
            D_800EFD70.w2E8 = 8;
            D_800EFD70.h47C = 0;
        } else if ((ctl->h8 & 0x1000) && D_800EFD70.w2E8 != 16 && D_800EFD70.w2E8 != 9
                   && D_800EFD70.w2E8 != 10 && !(D_800EFD70.w0 & 0x200)) {
            if (!(ctl->h8 & 1)) {
                D_800EFD70.w2E8 = 9;
                D_800EFD70.h47C = 0;
            }
        } else if (D_800EFD70.w2E8 == 16) {
            D_800EFD70.w4C = 0;
            D_800EFD70.w2E8 = 0;
            D_800EFD70.w0 |= 0x200;
        } else if (!(ctl->h8 & 0x100) || D_800EFD70.w2E8 == 5) {
          if (!(ctl->h8 & 1)) {
            if (D_800EFD70.w2E8 == 8 && D_800EFD70.h47C == 1) {
                D_800EFD70.h488 = 1;
                func_8007AF38((s32)&D_800CD63C[15], 1, 1);
            } else if (D_800EFD70.w2E8 == 10 && !(D_800EFD70.w0 & 0x200)) {
                D_800EFD70.w2E8 = 16;
                if (D_800EFD70.w410 == 0) {
                    D_800EFD70.w410 = 1;
                }
            } else if (D_800EFD70.w2E8 == 9 && D_800EFD70.h47C == 0) {
                D_800EFD70.w2E8 = 10;
                if (D_800F1900.b2C4 & 8) {
                    D_800EFD70.h47C = 0;
                } else {
                    D_800EFD70.h47C = 1;
                }
            } else {
                if (D_800EFD70.w0 & 0x200) {
                    if (D_800EFD70.h47C != 0) {
                        D_800EFD70.w0 |= 0x2000000;
                        D_800EFD70.h488 = 3;
                        D_800EFD70.w0 &= ~0x20;
                        func_8007AF38((s32)&D_800CD63C[15], 1, 1);
                    } else {
                        D_800EFD64 = (s32)D_800CBF30;
                        func_8007AF38((s32)&D_800CD63C[9], 1, 1);
                        func_800A44EC(D_800F1DF4);
                        D_800EFD70.w4C = 0;
                        D_800EFD70.w0 &= ~0x80;
                        D_800EFD70.w0 &= ~0x20;
                    }
                }
                if (D_800EFD70.w2E8 == 5 && D_800EFD70.w410 == 0) {
                    func_8007AF38(0, 0, 1);
                } else {
                    func_8007AF38(ctl->p4, 1, 1);
                }
            }
          }
        } else {
            D_800EFD70.w2E8 = 5;
            if ((ctl->h8 & 0x400) && D_800EFD70.w410 == 0) {
                D_800EFD70.w410 = 1;
            }
        }
        if (snd) {
            func_8001102C(1);
        }
    } else if (b & 0x20) {
        if (D_800EFD70.w0 & 0x200) {
            D_800EFD70.w2E8 = 16;
            func_8001102C(2);
            D_800EFD70.w0 &= ~0x200;
            D_800EFD70.w0 &= ~0x20;
            return;
        }
        s = D_800EFD70.w2E8;
        if (s == 16) {
            D_800EFD70.w2E8 = 10;
        } else if (s == 8 || s == 5 || s == 9 || s == 10) {
            func_8001102C(2);
            D_800EFD70.w2E8 = 0;
            D_800EFD70.w0 &= ~0x20;
        } else if (item->p28) {
            func_8001102C(2);
            func_8007AF38(item->p28, 0, 0);
        } else {
            func_8001102C(3);
        }
    } else if (D_800EFD70.w0 & 0x200) {
        m = D_800EFD70.w4C;
        if (m == 2) {
            if (D_800C292D < 2) {
                func_8001102C(2);
                D_800EFD70.w0 &= ~0x200;
            } else if (D_800EFD70.w3C == 0) {
                D_800EFD70.w3C = 2;
            } else if (D_800C292D < 4 && D_800EFD70.w3C == 4) {
                func_8001102C(0);
                D_800EFD70.w3C = 2;
            }
            if (D_800C292D == 4 && (*btn & 3)) {
                func_8001102C(0);
                if (D_800EFD70.w3C == 2) {
                    D_800EFD70.w3C = 4;
                } else {
                    D_800EFD70.w3C = 2;
                }
            }
        } else if (m == 7 || m == 10) {
            if (D_800EFD70.w3C == 0 || D_800C292C == 0) {
                D_800EFD70.w3C = 1;
            } else {
                s32 o = D_800EFD70.w3C;
                if (D_800C292D < o) {
                    D_800EFD70.w3C = D_800C292D;
                } else if (b & 3) {
                    D_800EFD70.w3C = (o & 1) + 1;
                    if (D_800C292D == 1) {
                        D_800EFD70.w3C = 1;
                    }
                    if (D_800EFD70.w3C != o) {
                        func_8001102C(0);
                    }
                }
            }
        } else if (D_800C292C == 0) {
            D_800EFD70.w3C = 1;
        } else {
            if (D_800EFD70.w3C == 0) {
                D_800EFD70.w3C = 1;
            }
            v = D_800C292D;
            if (v < D_800EFD70.w3C) {
                D_800EFD70.w3C = v;
            }
            if (*btn & 1) {
                s32 old = D_800EFD70.w3C;
                if ((D_800EFD70.w3C = old - 1) <= 0) {
                    D_800EFD70.w3C = v;
                }
                if (D_800EFD70.w3C != old) {
                    func_8001102C(0);
                }
            }
            if (*btn & 2) {
                s32 old = D_800EFD70.w3C;
                D_800EFD70.w3C = old + 1;
                if (D_800C292D < old + 1) {
                    D_800EFD70.w3C = 1;
                }
                if (D_800EFD70.w3C != old) {
                    func_8001102C(0);
                }
            }
            if (D_800EFD70.w4C == 0 && D_800EFD70.w3C == 2) {
                if (*btn & 4) {
                    s32 old;
                    func_8001102C(0);
                    old = D_800EFD70.w75C;
                    if ((D_800EFD70.w75C = old - 1) < 0) {
                        D_800EFD70.w75C = old + 2;
                    }
                }
                if (*btn & 8) {
                    func_8001102C(0);
                    if (++D_800EFD70.w75C >= 3) {
                        D_800EFD70.w75C -= 3;
                    }
                }
            }
        }
    } else {
        s = D_800EFD70.w2E8;
        if ((u32)(s - 8) < 3) {
            if ((s != 10 || (D_800F1900.b2C4 & 8)) && (b & 3)) {
                func_8001102C(0);
                D_800EFD70.h47C ^= 1;
            }
            return;
        }
        if (b & 1) {
            if ((s == 16 || s == 5) && D_800F1900.b2C0) {
                s32 old = D_800EFD70.w410;
                if ((D_800EFD70.w410 = old - 1) < 0) {
                    D_800EFD70.w410 = D_800F1900.b2C0;
                } else if (D_800EFD70.w410 == 0 && ((ctl->h8 & 0x400) || s == 16)) {
                    D_800EFD70.w410 = D_800F1900.b2C0;
                }
                if (D_800EFD70.w410 != old) {
                    func_8001102C(0);
                }
            } else {
                s32 old = *item->p20;
                if ((*item->p20 = old - 1) < 0) {
                    *item->p20 = item->h18 - 1;
                }
                if (*item->p20 != old) {
                    func_8001102C(0);
                }
            }
        }
        if (*btn & 2) {
            s32 t = D_800EFD70.w2E8;
            if ((t == 16 || t == 5) && D_800F1900.b2C0) {
                s32 old = D_800EFD70.w410;
                D_800EFD70.w410 = old + 1;
                if (D_800F1900.b2C0 < old + 1) {
                    if ((ctl->h8 & 0x400) || t == 16) {
                        D_800EFD70.w410 = 1;
                    } else {
                        D_800EFD70.w410 = 0;
                    }
                }
                if (D_800EFD70.w410 != old) {
                    func_8001102C(0);
                }
            } else {
                s32 old = *item->p20;
                *item->p20 = old + 1;
                if (old + 1 >= item->h18) {
                    *item->p20 = 0;
                }
                if (*item->p20 != old) {
                    func_8001102C(0);
                }
            }
        }
        if (D_800EFD70.w54 != 1) {
            prev = curA_800F47C4;
            if (*btn & 4) {
                if (item->p4 == NULL) {
                    if (item->p8) {
                        D_800F47C4 = item;
                    }
                } else {
                    item = item->p4;
                    D_800F47C4 = item;
                }
            }
            if (*btn & 8) {
                if (item->p8 == NULL) {
                    if (item->p4) {
                        D_800F47C4 = item;
                    }
                } else {
                    if (canGo(item->p8)) {
                        item = item->p8;
                    }
                    D_800F47C4 = item;
                }
            }
            if (prev != D_800F47C4) {
                func_8001102C(0);
            }
        }
    }
}
