#include "ctx.h"

typedef struct { s32 w[0x500 / 4]; } Blk500;
typedef struct { s32 w[0x780 / 4]; } Blk780;

typedef struct {
    u8 pad0[0x22];
    u16 f22;
    u8 pad24[0x34 - 0x24];
    u16 f34;
    u8 pad36[0x48 - 0x36];
    Vec3f pos;          /* 0x48 */
    Vec3f vel;          /* 0x54 */
    u8 pad60[0x78 - 0x60];
    u8 f78[0x40];       /* 0x78 */
    Vec3f dir;          /* 0xB8 */
    s32 fC4;
    Vec3f hist[11];     /* 0xC8 */
    u8 pad14C[0x1B8 - 0x14C];
    f32 f1B8;
    u8 pad1BC[4];
    f32 f1C0;
    u8 f1C4;
    u8 f1C5;
    u8 pad1C6[2];
    void *f1C8;
    u8 pad1CC[0x248 - 0x1CC];
    s32 f248;
    u8 pad24C[0x340 - 0x24C];
    f32 f340;
    f32 f344;
    u8 f348;
    u8 f349;
    u8 pad34A[2];
    void *f34C;
    u8 pad350[0x388 - 0x350];
    s32 f388;
    f32 f38C;
    f32 f390[6];
    u8 pad3A8[0x3C0 - 0x3A8];
    s16 f3C0;
    u8 pad3C2[0x3D0 - 0x3C2];
    s8 f3D0;
    s8 f3D1;
    u8 pad3D2;
    s8 f3D3;
    u8 pad3D4[0x3E4 - 0x3D4];
    u8 f3E4[7];
    u8 f3EB[0x10];
    u8 pad3FB[0x412 - 0x3FB];
    s16 f412;
    u8 pad414[0x41D - 0x414];
    u8 f41D;
    u8 pad41E[2];
    f32 f420;
    u8 pad424[0x42C - 0x424];
    f32 f42C;
    u8 pad430[0x44C - 0x430];
    f32 f44C;
    u8 pad450[0x4E4 - 0x450];
    void *f4E4;
    u8 pad4E8[0x588 - 0x4E8];
    void *f588;
    u8 pad58C[0x5A8 - 0x58C];
    u16 f5A8;
    u16 f5AA;
    u8 pad5AC[0x628 - 0x5AC];
    s16 f628;
    u8 pad62A[0x644 - 0x62A];
    Vec3f f644;
    u8 pad650[0x658 - 0x650];
    Blk500 *f658;
    Blk780 *f65C;
    u8 pad660[2];
    s16 f662;
    s16 f664;
    s16 f666;
    u8 pad668[0x6B2 - 0x668];
    s16 f6B2;
    u8 pad6B4[0x6E8 - 0x6B4];
    u8 f6E8;
    u8 pad6E9[0x778 - 0x6E9];
    s32 f778;
    u8 pad77C[0x7A5 - 0x77C];
    u8 f7A5;
    u8 pad7A6[3];
    u8 f7A9;
    u8 pad7AA[0x7E0 - 0x7AA];
} Player;

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[4];
    u8 pad2C[0x3C - 0x2C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    u32 f4C;
    u8 pad50[0x2C8 - 0x50];
    s8 f2C8[8];
    u8 pad2D0[0x760 - 0x2D0];
    u8 f760[16];
} State;

typedef struct {
    Vec3f pos;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x58 - 0x1C];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} Track;

typedef struct {
    u8 pad0[0x18];
    s16 f18;
    u8 pad1A[0x28 - 0x1A];
} Course;

typedef struct {
    u8 pad0[0x64];
    s16 f64;
} Sel;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x50 - 0x30];
    u32 f50;
} Opt;

typedef struct {
    u8 pad0[0xC];
    u16 fC;
    u8 padE[2];
} Bk;

typedef struct {
    f32 x, z;
} Start;

extern u8 D_800E3770[];
extern s32 D_800C8CE8;
extern u8 D_800F0B30;
extern s32 D_800E76C0;
extern u8 D_800EE30C;
extern s32 D_800E76C4;
extern u8 D_800E7700;
extern Vec3f D_800EE150;
extern Vec3f D_800EE160;
extern s32 D_800E7728;
extern Player D_800E37C0[];
extern State D_800EFD70;
extern s32 D_800C238C[];
extern s32 D_800E7704;
extern s32 D_800E76D0;
extern s32 D_800C8CD8;
extern s32 D_800C2280;
extern Blk500 *D_800E1878;
extern Blk780 *D_800E1A78;
extern u8 D_800CC324[];
extern u8 D_800CC6CC[];
extern u8 D_800CC30C[];
extern u8 D_800C9080[];
extern Bk D_800C9698[];
extern u8 D_800C966C[];
extern s16 D_800F05CA;
extern s32 D_800C9654[];
extern s32 D_800CCD64;
extern Start D_800C9480[];
extern s32 D_800EE490;
extern s32 D_800EE494;
extern Opt *D_800EFD64;
extern Sel *D_800CA930;
extern Course D_800CADE4[];
extern Track *D_800EE498;
extern Track *trk_800EE498;
extern s8 D_800F0CA0[];
extern s16 D_800C2C60[];
extern s16 cos_800C2C60[];
extern f32 D_800E37BC;
extern u8 D_800C8DC0;

void func_8000ED10(void *a0, s32 a1, s32 a2);
void func_80065048(void);
void func_80065080(void);
void func_8006F620(void);
void func_80061DCC(void);
void func_80064CA8(s32);
void func_80012B14(void);
void *func_800BC908(s32, s32, char *, s32);
void func_800361EC(Vec3f *p);
s32 func_8000EFE8(void);
s32 func_80060270(void *);
void func_8001AEB0(Player *, s32);
void func_80013438(void *, f32, f32, f32);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);
void func_8001C780(Player *);
void func_8001BC98(Player *);
void func_8001FE1C(Player *);
void func_80064EB8();
void func_800477B4(f32);
void func_80064A8C(s32, s32);
void func_80047940(void);
void func_8006C28C(s32);
void func_8006B1A0(void);

void func_8001A190(void) {
    Vec3f unused[4];
    s32 i;
    s32 k;
    s32 j;
    s32 a;
    s32 c;
    s32 cc;
    s32 r;
    s32 n;
    s32 idx;
    f32 zero;
    f32 z;
    f32 sc;
    u16 ang;
    s32 ratio;
    Player *p;
    Vec3f *v;

    func_8000ED10(D_800E3770, 0, 0x10);
    if (D_800C8CE8 == 0) {
        func_80065080();
    } else {
        func_80065048();
    }
    func_8006F620();
    i = 0;
    D_800F0B30 = 0;
    D_800E76C0 = 0;
    D_800E76C4 = 1;
    D_800EE30C = 0;
    D_800E7700 ^= 1;
    zero = 0.0f;
    D_800EE150.z = zero;
    D_800EE150.y = zero;
    D_800EE150.x = zero;
    D_800EE160.z = zero;
    D_800EE160.y = zero;
    D_800EE160.x = zero;
    func_80061DCC();
    func_80064CA8(D_800E7728);
    func_80012B14();
    D_800E7704 = 1;
    D_800E76D0 = 0;
    for (; i < 8; i++) {
        z = 0.0f;
        D_800C238C[i] = 0xFF;
        D_800E37C0[i].f778 = -60;
        if (D_800C8CD8 != 0 || (D_800E37C0[i].f6E8 == 0 && (D_800C2280 & 0x100))) {
            D_800E37C0[i].f41D = 1;
        }
        D_800E37C0[i].f658 = func_800BC908(0x500, 0, "bikenew.c", 233);
        D_800E37C0[i].f65C = func_800BC908(0x780, 0, "bikenew.c", 234);
        *D_800E37C0[i].f658 = *D_800E1878;
        *D_800E37C0[i].f65C = *D_800E1A78;
        func_8000ED10(D_800E37C0[i].f3EB, -1, 0x10);
        func_8000ED10(D_800E37C0[i].f3E4, -1, 6);
        func_800361EC(&D_800E37C0[i].f644);
        D_800E37C0[i].f420 = 0.0f;
        D_800E37C0[i].f388 = 0;
        D_800E37C0[i].f3D3 = -1;
        D_800E37C0[i].f7A5 = 1;
        D_800E37C0[i].f666 = 0x100;
        D_800E37C0[i].f390[0] = z;
        D_800E37C0[i].f390[1] = z;
        D_800E37C0[i].f390[2] = z;
        D_800E37C0[i].f390[3] = z;
        D_800E37C0[i].f390[4] = z;
        D_800E37C0[i].f390[5] = z;
        D_800E37C0[i].f3D0 = -1;
        D_800E37C0[i].f3D1 = -1;
        D_800E37C0[i].f38C = 1.0f;
        D_800E37C0[i].vel.x = D_800E37C0[i].dir.x * z;
        D_800E37C0[i].vel.y = D_800E37C0[i].dir.y * D_800E37C0[i].f420;
        D_800E37C0[i].vel.z = D_800E37C0[i].dir.z * D_800E37C0[i].f420;
        r = func_8000EFE8();
        D_800E37C0[i].f1B8 = z;
        D_800E37C0[i].f1C5 = 1;
        D_800E37C0[i].f348 = 3;
        D_800E37C0[i].f349 = 0;
        D_800E37C0[i].f1C0 = 250.0f;
        D_800E37C0[i].f1C4 = D_800E37C0[i].f6E8 != 0;
        D_800E37C0[i].f340 = 90.0f;
        D_800E37C0[i].f344 = 45.0f;
        D_800E37C0[i].f3C0 = r % 150;
        if (D_800EFD70.f4C == 2) {
            D_800E37C0[i].fC4 = func_80060270(D_800CC6CC);
        } else {
            D_800E37C0[i].fC4 = func_80060270(D_800CC324);
        }
        D_800E37C0[i].f248 = func_80060270(D_800CC30C);
        D_800E37C0[i].f1C8 = &D_800E37C0[i];
        D_800E37C0[i].f34C = &D_800E37C0[i];
        D_800E37C0[i].f588 = D_800C9080;
        D_800E37C0[i].f4E4 = D_800C9080;
        switch (D_800EFD70.f4C) {
        case 7:
            D_800E37C0[i].f5AA = 6;
            break;
        case 2:
            D_800E37C0[i].f5AA = 0;
            break;
        case 6:
            if (i < D_800EFD70.f3C) {
                c = D_800EFD70.f760[D_800EFD70.f2C8[i]];
            } else {
                c = 2;
            }
            if (c != 4 && c != 2) {
                c = 2;
            }
            D_800E37C0[i].f5AA = c;
            if (i < D_800EFD70.f3C) {
                D_800EFD70.f760[D_800EFD70.f2C8[i]] = c;
            }
            break;
        default:
            if (i < D_800EFD70.f3C) {
                c = D_800EFD70.f760[D_800EFD70.f2C8[i]];
            } else {
                c = 2;
            }
            if (D_800C9698[c].fC & 8) {
                c = 2;
            }
            D_800E37C0[i].f5AA = c;
            if (i < D_800EFD70.f3C) {
                D_800EFD70.f760[D_800EFD70.f2C8[i]] = c;
            }
            break;
        }
        func_8000ED10(D_800C966C, -1, 0x18);
        D_800E37C0[i].f412 = 0x3000;
        D_800E37C0[i].f5A8 = D_800E37C0[i].f5AA;
    }
    i = 0;
    if (D_800F05CA > 0) {
        do {
            p = &D_800E37C0[i];
            if (i < 4) {
                D_800C9654[i] = i;
            }
            if (!(D_800EFD70.f0 & 0x81) && D_800CCD64 == 0 && i < D_800EFD70.f3C) {
                p->f6E8 = 0;
            } else {
                p->f6E8 = 1;
            }
            func_8001AEB0(p, D_800EFD70.f1C[i]);
            p->f44C = 1.0f;
            p->f7A5 = 0;
            p->f420 = 0.0f;
            switch (D_800EFD70.f4C) {
            case 2:
                if (D_800EFD70.f3C == 4) {
                    switch (i) {
                    case 0:
                        D_800E37C0[i].pos.x = -1.0f;
                        D_800E37C0[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                        break;
                    case 1:
                        D_800E37C0[i].pos.x = 1.0f;
                        D_800E37C0[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                        break;
                    case 2:
                        D_800E37C0[i].pos.x = 1.0f;
                        D_800E37C0[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 3.1415927f);
                        break;
                    case 3:
                        D_800E37C0[i].pos.x = -1.0f;
                        D_800E37C0[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 3.1415927f);
                        break;
                    }
                } else {
                    switch (i) {
                    case 0:
                        D_800E37C0[i].pos.x = 0.01f;
                        D_800E37C0[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 0.00958738f);
                        break;
                    case 1:
                        D_800E37C0[i].pos.x = -0.015f;
                        D_800E37C0[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 3.15118f);
                        break;
                    }
                }
                break;
            case 4:
            case 5:
                p->pos.x = D_800C9480[i].x;
                p->pos.z = D_800C9480[i].z;
                func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                break;
            default:
                if ((D_800EFD70.f0 & 0x11) == 0x11) {
                    idx = i * D_800EE490 / 6;
                } else if (D_800EFD70.f4C == 13 || D_800EFD70.f4C == 14
                           || (D_800EFD70.f4C == 0 && (D_800EFD64->f50 & 2))) {
                    idx = 1;
                } else if (D_800EFD70.f4C == 9) {
                    idx = D_800CADE4[D_800CA930->f64].f18;
                } else {
                    idx = D_800EE494;
                }
                n = D_800F05CA;
                if (D_800C8CD8 != 0 && n == 1) {
                    n = 2;
                }
                if (D_800EFD70.f0 & 0x90) {
                    zero = 0.0f;
                } else {
                    sc = 1.5f;
                    zero = (D_800EE498[idx].f5C - D_800EE498[idx].f58) * 0.5f;
                    cc = D_800F0CA0[i];
                    if (D_800EFD64->f50 & 2) {
                        sc = 1.1f;
                    }
                    zero += sc * ((f32)cc - (f32)(n - 1) * 0.5f);
                    p->f662 = cc;
                    p->f664 = cc;
                    p->f666 = 0x100;
                }
                p->f628 = idx;
                if (D_800EFD70.f4C == 7) {
                    p->f6B2 = idx;
                }
                k = idx;
                if (D_800EFD70.f4C == 9 && D_800CA930->f64 == 5) {
                    k = idx + 1;
                }
                func_8001254C(D_800EE498[k].f18 * 1024.0f, D_800EE498[k].f14 * 1024.0f, (s16 *)&ang, &ratio);
                func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                if (D_800EFD64->f2C == 0 && !(D_800EFD70.f0 & 0x10)) {
                    ang = (ang + 0x1000) & ~0x3FFF;
                    func_80013438((u8 *)&D_800E37C0[i].pos + 0x30, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                }
                if (D_800EFD70.f4C != 6) {
                    a = (s16)ang >> 4;
                    p->pos.x = trk_800EE498[idx].pos.x + (f32)D_800C2C60[a & 0xFFF] * zero * 6.1035156e-05f
                             - (f32)D_800C2C60[(a - 0x400) & 0xFFF] * 1.5258789e-05f;
                    p->pos.z = trk_800EE498[idx].pos.z + (f32)D_800C2C60[(a - 0x400) & 0xFFF] * zero * 6.1035156e-05f
                             + (f32)D_800C2C60[a & 0xFFF] * 1.5258789e-05f;
                    p->f42C = zero;
                } else {
                    p->pos.z = trk_800EE498[idx].pos.z;
                    p->pos.x = (f32)i * 2.178f;
                    p->f42C = (f32)-i * 2.178f;
                }
                p->f7A9 = i;
                break;
            }
            func_8001C780(p);
            func_8001BC98(p);
            func_8001FE1C(p);
            p->f34 = p->f22;
            for (j = 0; j < 11; j++) {
                D_800E37C0[i].hist[j] = D_800E37C0[i].pos;
            }
            i++;
        } while (i < D_800F05CA);
    }
    func_80064EB8(8);
    func_800477B4(0.5f);
    D_800E37BC = -5.0f;
    func_80064A8C(D_800EFD70.f3C, D_800F05CA - D_800EFD70.f3C);
    switch (D_800EFD70.f4C) {
    case 2:
        D_800E37BC = -20.0f;
        func_8006C28C(1);
        break;
    case 5:
        func_80047940();
        break;
    case 7:
        func_800477B4(0.3f);
        break;
    case 6:
        D_800E37BC = 0.0f;
        func_800477B4(0.6f);
        func_8006B1A0();
        break;
    }
    D_800C8DC0 = 1;
}
