#include "ctx.h"

typedef struct {
    u8 pad0[0x17C];
    s32 f17C;
    u8 pad180[0x2C8 - 0x180];
    s8 f2C8[4];
} Ctl;

typedef struct {
    u8 pad0[0x2C4];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
} Opt;

extern Ctl D_800EFD70;
extern Opt D_800F1900;
extern s32 D_800E30B0[];
extern s32 D_800E25B0[];
extern s32 D_800E36FC;
extern s32 D_800E3708;
extern s32 D_800E37AC;
extern s32 D_800E3710;
extern s16 D_800E3730;
extern s32 D_800C2C4C;
extern u8 D_800F1BC5;
extern u8 D_800F1BC6;
extern u8 D_800F1BC7;

void func_8006F5F8(s32 x, s32 y);
void func_8004F4DC(s32 a0);
void func_8001102C(s32 param_1);
void func_800B8F2C(void);
void func_80010E5C(s32 mode, s32 v);
void func_8004F534(void);
s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);

#define PAD (D_800EFD70.f2C8[D_800E37AC])
#define PRESSED(m) ((D_800E30B0[PAD] & (m)) && !(D_800E25B0[PAD] & (m)))

void func_800169BC(void) {
    s32 s0;
    s32 old;
    s32 b;

    if (D_800E3708 != 0) {
        D_800E3708--;
    }
    if (++D_800EFD70.f17C == 60) {
        D_800EFD70.f17C = 0;
    }
    if (((D_800E30B0[PAD] >> 24) & 0xF) < 6) {
        D_800E30B0[PAD] &= ~0xC;
    }
    if ((s32)((u32)D_800E30B0[PAD] >> 28) < 6) {
        D_800E30B0[PAD] &= ~0x3;
    }
    if (D_800E30B0[PAD] & 4) {
        D_800E30B0[PAD] |= 0x2000;
    }
    if (D_800E30B0[PAD] & 8) {
        D_800E30B0[PAD] |= 0x4000;
    }
    if (D_800E30B0[PAD] & 1) {
        D_800E30B0[PAD] |= 0x800;
    }
    if (D_800E30B0[PAD] & 2) {
        D_800E30B0[PAD] |= 0x1000;
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
            b = D_800E30B0[PAD];
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
        func_80010E5C(1, D_800F1BC5);
        break;
    case 2:
        func_80010E5C(2, D_800F1BC7);
        break;
    }
    if (D_800E36FC != old) {
        func_8001102C(0);
    }
}
