#include "ctx.h"

typedef struct {
    /* 0x000 */ s32 f0;
    /* 0x004 */ u8 pad4[0x3C - 0x4];
    /* 0x03C */ s32 f3C;
    /* 0x040 */ u8 pad40[0x4C - 0x40];
    /* 0x04C */ s32 f4C;
    /* 0x050 */ u8 pad50[0x54 - 0x50];
    /* 0x054 */ s32 f54;
    /* 0x058 */ u8 pad58[0x17C - 0x58];
    /* 0x17C */ s32 f17C;
    /* 0x180 */ u8 pad180[0x2E8 - 0x180];
    /* 0x2E8 */ s32 f2E8;
    /* 0x2EC */ u8 pad2EC[0x3FC - 0x2EC];
    /* 0x3FC */ void *f3FC;
    /* 0x400 */ u8 pad400[0x448 - 0x400];
    /* 0x448 */ s16 f448;
    /* 0x44A */ u8 pad44A[0x47C - 0x44A];
    /* 0x47C */ u16 f47C;
    /* 0x47E */ u8 pad47E[0x770 - 0x47E];
    /* 0x770 */ s32 f770;
    /* 0x774 */ s32 f774;
} State;

typedef struct {
    u8 pad[0x2C4];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
} Opts;

typedef struct {
    u8 pad[0x28];
    s32 f28;
} Arg0;

extern State D_800EFD70;
extern Opts D_800F1900;
extern void *D_800EFD64;
extern s32 D_800CBA30[];
extern s32 D_800CD928[];
extern s32 D_800CD8FC[];
extern s32 D_800C2940;
extern s32 D_800CCD64;
extern s32 D_800D11D8;
extern s32 D_800E25B0;
extern s32 D_800E30B0;
extern u8 D_800F1BC6;

s32 func_8004F180(void);
void func_8004F4DC(s32 a0);
void func_8001102C(s32 param_1);
void func_800B8F2C(void);
void func_80010E5C(s32 mode, s32 v);
void func_8004F534(void);
void func_8004C3C4(void);
void func_8004C22C(s32 param_1);
void func_8006F3E4(s32);
void func_8007AF38(void *, s32, s32);
void func_800B7990(s32, s32, s32, s32, s32, s32);

void func_80075764(Arg0 *arg0, s32 arg1, s32 *arg2) {
    s32 m;
    s32 old;
    s32 d;

    if (++D_800EFD70.f17C == 60) {
        D_800EFD70.f17C = 0;
    }
    if (D_800EFD70.f448 & 0x100) {
        D_800EFD70.f448 &= ~0x100;
        if (D_800EFD70.f448 == 0) {
            func_8004C22C(D_800D11D8);
        }
    }
    if ((D_800E30B0 & 0x20) && !(D_800E25B0 & 0x20) && D_800EFD70.f54 == 3) {
        if (D_800EFD70.f2E8 == 6) {
            D_800EFD70.f2E8 = 0;
            func_8001102C(2);
        } else if (D_800EFD70.f2E8 != 14 && D_800EFD70.f2E8 != 15) {
            func_8007AF38((void *)arg0->f28, 0, 0);
            func_800B8F2C();
            func_8004F534();
            if (func_8004F180() != 0) {
                func_8004C3C4();
            }
            func_8001102C(2);
        }
        D_800CCD64 = 0;
        return;
    }
    if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10) && D_800EFD70.f54 == 3) {
        switch (D_800EFD70.f448) {
        case 3:
                        D_800F1900.f2C4 ^= 1;
            func_8001102C(1);
            return;
        case 8:
                        D_800F1900.f2C4 ^= 0x10;
            func_8001102C(1);
            return;
        case 1:
            func_800B7990(D_800EFD70.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            return;
        case 2:
            if (D_800EFD70.f770 == 0) {
                func_8004F4DC(0x9B);
                D_800EFD70.f770 = 0x34;
            }
            return;
        case 10:
                        func_8001102C(3);
            return;
        case 9:
                        func_8001102C(3);
            return;
        case 7:
            if (D_800C2940 == 0) {
                return;
            }
                        D_800F1900.f2C4 ^= 2;
            func_8001102C(1);
            return;
        case 4:
            func_8007AF38(D_800CD928, 1, 1);
                        func_8001102C(1);
            return;
        case 5:
            func_8007AF38(D_800CD8FC, 1, 1);
                        func_8001102C(1);
            return;
        case 6:
            func_8007AF38(NULL, 0, 1);
                        D_800CCD64 = 1;
            D_800EFD70.f4C = 0;
            D_800EFD64 = D_800CBA30;
            D_800EFD70.f3C = 1;
            D_800EFD70.f3FC = arg0;
            {
                s32 f = D_800EFD70.f0 & ~0x80;
                *(volatile s32 *)&D_800EFD70.f0 = f;
                D_800EFD70.f0 = f | 0x05000000;
            }
            func_8001102C(1);
            return;
        }
        return;
    }
    m = D_800EFD70.f2E8;
    if (m == 6 || m == 15 || m == 14) {
        if (m == 15) {
            func_8006F3E4(1);
            D_800EFD70.f2E8 = 0;
        } else if (m != 14) {
            if (*arg2 & 3) {
                D_800EFD70.f47C ^= 1;
                func_8001102C(0);
            }
        }
        return;
    }
    old = D_800EFD70.f448;
    if (*arg2 & 2) {
        if (old < 3) {
            D_800EFD70.f448 += 1;
        } else {
            D_800EFD70.f448 += 2;
        }
        if (D_800EFD70.f448 >= 11) {
            D_800EFD70.f448 = 0;
        }
    }
    if (*arg2 & 1) {
        if (D_800EFD70.f448 < 4) {
            D_800EFD70.f448 -= 1;
        } else {
            D_800EFD70.f448 -= 2;
        }
        if (D_800EFD70.f448 < 0) {
            D_800EFD70.f448 = 9;
        }
    }
    if (old != D_800EFD70.f448) {
        switch (old) {
        case 2:
            func_8004F534();
            break;
        case 0:
            func_8004C3C4();
            break;
        case 1:
            break;
        }
        switch (D_800EFD70.f448) {
        case 2:
            func_8004F4DC(0x9B);
            D_800EFD70.f770 = 0x34;
            break;
        case 0:
            func_8004C22C(D_800D11D8);
            break;
        case 1:
            func_800B7990(D_800EFD70.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            break;
        }
    }
    if (D_800EFD70.f448 >= 3) {
        if (*arg2 & 0xC) {
            if ((D_800EFD70.f448 - 3) & 1) {
                D_800EFD70.f448--;
            } else {
                D_800EFD70.f448++;
            }
        }
    } else {
        if (D_800EFD70.f770 != 0) {
            D_800EFD70.f770--;
        }
        if ((D_800E30B0 & 0x2000) && !(D_800E25B0 & 0x2000)) {
            d = -8;
        } else if ((D_800E30B0 & 0x4000) && !(D_800E25B0 & 0x4000)) {
            d = 8;
        } else {
            s32 b = D_800E30B0;
            if (b & 0xC) {
                d = ((b & 8) ? ((b >> 24) & 0xF) : -((b >> 24) & 0xF)) / 3;
            } else {
                d = 0;
            }
        }
        if (d != 0 && D_800EFD70.f770 == 0) {
            switch (D_800EFD70.f448) {
            case 1:
                func_800B7990(D_800EFD70.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
                D_800EFD70.f770 = 0x28;
                break;
            case 2:
                func_8004F4DC(0x9B);
                D_800EFD70.f770 = 0x34;
                break;
            }
        }
        if (d < 0) {
            switch (D_800EFD70.f448) {
            case 1:
                if (D_800F1900.f2C6 >= -d) {
                    D_800F1900.f2C6 += d;
                } else {
                    D_800F1900.f2C6 = 0;
                }
                break;
            case 0:
                if (D_800F1900.f2C5 >= -d) {
                    D_800F1900.f2C5 += d;
                } else {
                    D_800F1900.f2C5 = 0;
                }
                break;
            case 2:
                if (D_800F1900.f2C7 >= -d) {
                    D_800F1900.f2C7 += d;
                } else {
                    D_800F1900.f2C7 = 0;
                }
                break;
            }
        } else {
            s32 t;
            switch (D_800EFD70.f448) {
            case 1:
                t = D_800F1900.f2C6 + d;
                if (t >= 256) {
                    D_800F1900.f2C6 = 255;
                } else {
                    D_800F1900.f2C6 = t;
                }
                break;
            case 0:
                t = D_800F1900.f2C5 + d;
                if (t >= 256) {
                    D_800F1900.f2C5 = 255;
                } else {
                    D_800F1900.f2C5 = t;
                }
                break;
            case 2:
                t = D_800F1900.f2C7 + d;
                if (t >= 256) {
                    D_800F1900.f2C7 = 255;
                } else {
                    D_800F1900.f2C7 = t;
                }
                break;
            }
        }
        func_80010E5C(1, D_800F1900.f2C5);
        func_80010E5C(2, D_800F1900.f2C7);
        func_80010E5C(0, D_800F1900.f2C6);
    }
    if (D_800EFD70.f448 != old) {
        func_8001102C(0);
    }
}
