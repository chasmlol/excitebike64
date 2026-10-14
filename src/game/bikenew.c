#include "game.h"

typedef struct {
    u8 pad0[0x18];
    f32 f18;
} Sub;

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
} Player_8;

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
} State_2;

typedef struct {
    Vec3f pos;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x58 - 0x1C];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} Track_6;

typedef struct {
    u8 pad0[0x18];
    s16 f18;
    u8 pad1A[0x28 - 0x1A];
} Course;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x50 - 0x30];
    u32 f50;
} Opt_3;

typedef struct {
    u8 pad0[0xC];
    u16 fC;
    u8 padE[2];
} Bk;

extern Player_8 D_800E37C0_16[];
extern State_2 D_800EFD70_25;
extern Blk500 *D_800E1878_3;
extern Blk780 *D_800E1A78_4;
extern Bk D_800C9698[];
extern Opt_3 *D_800EFD64_17;
extern Course D_800CADE4_7[];
extern Track_6 *D_800EE498_3;
extern Track_6 *trk_800EE498;

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
    Player_8 *p;
    Vec3f *v;

    func_8000ED10_2(D_800E3770, 0, 0x10);
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
        D_800E37C0_16[i].f778 = -60;
        if (D_800C8CD8 != 0 || (D_800E37C0_16[i].f6E8 == 0 && (D_800C2280 & 0x100))) {
            D_800E37C0_16[i].f41D = 1;
        }
        D_800E37C0_16[i].f658 = func_800BC908(0x500, 0, "bikenew.c", 233);
        D_800E37C0_16[i].f65C = func_800BC908(0x780, 0, "bikenew.c", 234);
        *D_800E37C0_16[i].f658 = *D_800E1878_3;
        *D_800E37C0_16[i].f65C = *D_800E1A78_4;
        func_8000ED10_2(D_800E37C0_16[i].f3EB, -1, 0x10);
        func_8000ED10_2(D_800E37C0_16[i].f3E4, -1, 6);
        func_800361EC(&D_800E37C0_16[i].f644);
        D_800E37C0_16[i].f420 = 0.0f;
        D_800E37C0_16[i].f388 = 0;
        D_800E37C0_16[i].f3D3 = -1;
        D_800E37C0_16[i].f7A5 = 1;
        D_800E37C0_16[i].f666 = 0x100;
        D_800E37C0_16[i].f390[0] = z;
        D_800E37C0_16[i].f390[1] = z;
        D_800E37C0_16[i].f390[2] = z;
        D_800E37C0_16[i].f390[3] = z;
        D_800E37C0_16[i].f390[4] = z;
        D_800E37C0_16[i].f390[5] = z;
        D_800E37C0_16[i].f3D0 = -1;
        D_800E37C0_16[i].f3D1 = -1;
        D_800E37C0_16[i].f38C = 1.0f;
        D_800E37C0_16[i].vel.x = D_800E37C0_16[i].dir.x * z;
        D_800E37C0_16[i].vel.y = D_800E37C0_16[i].dir.y * D_800E37C0_16[i].f420;
        D_800E37C0_16[i].vel.z = D_800E37C0_16[i].dir.z * D_800E37C0_16[i].f420;
        r = func_8000EFE8();
        D_800E37C0_16[i].f1B8 = z;
        D_800E37C0_16[i].f1C5 = 1;
        D_800E37C0_16[i].f348 = 3;
        D_800E37C0_16[i].f349 = 0;
        D_800E37C0_16[i].f1C0 = 250.0f;
        D_800E37C0_16[i].f1C4 = D_800E37C0_16[i].f6E8 != 0;
        D_800E37C0_16[i].f340 = 90.0f;
        D_800E37C0_16[i].f344 = 45.0f;
        D_800E37C0_16[i].f3C0 = r % 150;
        if (D_800EFD70_25.f4C == 2) {
            D_800E37C0_16[i].fC4 = func_80060270(D_800CC6CC);
        } else {
            D_800E37C0_16[i].fC4 = func_80060270(D_800CC324);
        }
        D_800E37C0_16[i].f248 = func_80060270(D_800CC30C);
        D_800E37C0_16[i].f1C8 = &D_800E37C0_16[i];
        D_800E37C0_16[i].f34C = &D_800E37C0_16[i];
        D_800E37C0_16[i].f588 = D_800C9080;
        D_800E37C0_16[i].f4E4 = D_800C9080;
        switch (D_800EFD70_25.f4C) {
        case 7:
            D_800E37C0_16[i].f5AA = 6;
            break;
        case 2:
            D_800E37C0_16[i].f5AA = 0;
            break;
        case 6:
            if (i < D_800EFD70_25.f3C) {
                c = D_800EFD70_25.f760[D_800EFD70_25.f2C8[i]];
            } else {
                c = 2;
            }
            if (c != 4 && c != 2) {
                c = 2;
            }
            D_800E37C0_16[i].f5AA = c;
            if (i < D_800EFD70_25.f3C) {
                D_800EFD70_25.f760[D_800EFD70_25.f2C8[i]] = c;
            }
            break;
        default:
            if (i < D_800EFD70_25.f3C) {
                c = D_800EFD70_25.f760[D_800EFD70_25.f2C8[i]];
            } else {
                c = 2;
            }
            if (D_800C9698[c].fC & 8) {
                c = 2;
            }
            D_800E37C0_16[i].f5AA = c;
            if (i < D_800EFD70_25.f3C) {
                D_800EFD70_25.f760[D_800EFD70_25.f2C8[i]] = c;
            }
            break;
        }
        func_8000ED10_2(D_800C966C, -1, 0x18);
        D_800E37C0_16[i].f412 = 0x3000;
        D_800E37C0_16[i].f5A8 = D_800E37C0_16[i].f5AA;
    }
    i = 0;
    if (D_800F05CA > 0) {
        do {
            p = &D_800E37C0_16[i];
            if (i < 4) {
                D_800C9654[i] = i;
            }
            if (!(D_800EFD70_25.f0 & 0x81) && D_800CCD64 == 0 && i < D_800EFD70_25.f3C) {
                p->f6E8 = 0;
            } else {
                p->f6E8 = 1;
            }
            func_8001AEB0(p, D_800EFD70_25.f1C[i]);
            p->f44C = 1.0f;
            p->f7A5 = 0;
            p->f420 = 0.0f;
            switch (D_800EFD70_25.f4C) {
            case 2:
                if (D_800EFD70_25.f3C == 4) {
                    switch (i) {
                    case 0:
                        D_800E37C0_16[i].pos.x = -1.0f;
                        D_800E37C0_16[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                        break;
                    case 1:
                        D_800E37C0_16[i].pos.x = 1.0f;
                        D_800E37C0_16[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                        break;
                    case 2:
                        D_800E37C0_16[i].pos.x = 1.0f;
                        D_800E37C0_16[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 3.1415927f);
                        break;
                    case 3:
                        D_800E37C0_16[i].pos.x = -1.0f;
                        D_800E37C0_16[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 3.1415927f);
                        break;
                    }
                } else {
                    switch (i) {
                    case 0:
                        D_800E37C0_16[i].pos.x = 0.01f;
                        D_800E37C0_16[i].pos.z = -10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 0.00958738f);
                        break;
                    case 1:
                        D_800E37C0_16[i].pos.x = -0.015f;
                        D_800E37C0_16[i].pos.z = 10.0f;
                        func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 3.15118f);
                        break;
                    }
                }
                break;
            case 4:
            case 5:
                p->pos.x = D_800C9480[i].x;
                p->pos.z = D_800C9480[i].z;
                func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, 0.0f);
                break;
            default:
                if ((D_800EFD70_25.f0 & 0x11) == 0x11) {
                    idx = i * D_800EE490 / 6;
                } else if (D_800EFD70_25.f4C == 13 || D_800EFD70_25.f4C == 14
                           || (D_800EFD70_25.f4C == 0 && (D_800EFD64_17->f50 & 2))) {
                    idx = 1;
                } else if (D_800EFD70_25.f4C == 9) {
                    idx = D_800CADE4_7[D_800CA930_2->f64].f18;
                } else {
                    idx = D_800EE494;
                }
                n = D_800F05CA;
                if (D_800C8CD8 != 0 && n == 1) {
                    n = 2;
                }
                if (D_800EFD70_25.f0 & 0x90) {
                    zero = 0.0f;
                } else {
                    sc = 1.5f;
                    zero = (D_800EE498_3[idx].f5C - D_800EE498_3[idx].f58) * 0.5f;
                    cc = D_800F0CA0_2[i];
                    if (D_800EFD64_17->f50 & 2) {
                        sc = 1.1f;
                    }
                    zero += sc * ((f32)cc - (f32)(n - 1) * 0.5f);
                    p->f662 = cc;
                    p->f664 = cc;
                    p->f666 = 0x100;
                }
                p->f628 = idx;
                if (D_800EFD70_25.f4C == 7) {
                    p->f6B2 = idx;
                }
                k = idx;
                if (D_800EFD70_25.f4C == 9 && D_800CA930_2->f64 == 5) {
                    k = idx + 1;
                }
                func_8001254C(D_800EE498_3[k].f18 * 1024.0f, D_800EE498_3[k].f14 * 1024.0f, (s16 *)&ang, &ratio);
                func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                if (D_800EFD64_17->f2C == 0 && !(D_800EFD70_25.f0 & 0x10)) {
                    ang = (ang + 0x1000) & ~0x3FFF;
                    func_80013438((u8 *)&D_800E37C0_16[i].pos + 0x30, 0.0f, 0.0f, (f32)(s16)ang * 3.1415927f * 3.0517578e-05f);
                }
                if (D_800EFD70_25.f4C != 6) {
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
                D_800E37C0_16[i].hist[j] = D_800E37C0_16[i].pos;
            }
            i++;
        } while (i < D_800F05CA);
    }
    func_80064EB8(8);
    func_800477B4(0.5f);
    D_800E37BC = -5.0f;
    func_80064A8C(D_800EFD70_25.f3C, D_800F05CA - D_800EFD70_25.f3C);
    switch (D_800EFD70_25.f4C) {
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

#define FLD(p, off) (*(f32 *)((u8 *)(p) + (off)))

typedef struct {
    f32 f[10];
} Rec28;

extern Rec28 D_800C8E98[];

void func_8001AEB0(u8 *a0, s32 a1)
{
    if (a0[0x6E8] == 0) {
        FLD(a0, 0x1BC) = 250.0f;
        FLD(a0, 0x7B8) = D_800C8E98[a1].f[0];
        FLD(a0, 0x7BC) = D_800C8E98[a1].f[1] * (D_800C8DF8[14] * D_800C8DF8[5]) * 32.0f * FLD(a0, 0x7D0);
        FLD(a0, 0x7C4) = D_800C8E98[a1].f[2] * D_800C8DF8[12] * 3.1415927f * 0.00390625f;
        FLD(a0, 0x7C8) = D_800C8E98[a1].f[3] / 48.0f;
        FLD(a0, 0x7D0) = D_800C8E98[a1].f[4] * 0.57f;
        FLD(a0, 0x7C0) = D_800C8E98[a1].f[5];
        FLD(a0, 0x7B4) = D_800C8E98[a1].f[6];
        FLD(a0, 0x7D4) = D_800C8E98[a1].f[7];
        FLD(a0, 0x7D8) = D_800C8E98[a1].f[8];
        return;
    }
    FLD(a0, 0x1BC) = 250.0f;
    FLD(a0, 0x7D4) = 0.3f;
    FLD(a0, 0x7B8) = 1.0f;
    FLD(a0, 0x7C0) = 1.0f;
    {
        f32 k57 = 0.57f;
        f32 t = D_800C8DF8[12] * 3.1415927f * 0.00390625f;
        FLD(a0, 0x7D0) = k57;
        FLD(a0, 0x7C8) = 0.020833334f;
        FLD(a0, 0x7C4) = t;
        FLD(a0, 0x7BC) = D_800C8DF8[14] * D_800C8DF8[5] * 32.0f * k57;
    }
    if (D_800EFDBC == 0xF) {
        FLD(a0, 0x7D8) = 0.7f;
    } else {
        FLD(a0, 0x7D8) = 1.0f;
    }
    FLD(a0, 0x7B4) = D_800C8E98[a1].f[6];
}

struct Obj_7 {
    char pad0[0x45];
    s8 b45;
    char pad1[0x6];
    f32 f4c;
    char pad2[0x5c];
    f32 fac;
    f32 fb0;
    f32 fb4;
};

f32 func_8001B05C(Obj_7 *o, f32 *p, f32 p3)
{
    f32 t, step, d, b, tb, h, diff, sc, sc3, sc4, lim;
    f32 v[3];
    f32 x, y, y2, tb2, tz, zp;
    f32 *vp;
    u8 *tbl;
    u8 *tbl2;
    f32 *vq;
    s32 n, m, k;

    t = 0.0f;
    step = -1.0f;
    if ((s32)o < step) n = 1; else n = 2;
    if (o) n = 1; else if (p) n = 2; else n = 3;
    h = o->f4c;
    v[1] = h;
    if (p[0] < h) n = 1; else n = 3;
    sc = 1000.0f;
    n = func_8002E920((s32)(p[0] * sc), (s32)(p[1] * sc), (s32)(p[2] * sc));
    d = (f32)n / sc - p[1];
    if (0.0f < d) step = 1.0f;
    vp = v;
    tbl = D_800EFD70_2;
    lim = 10000.0f;
    sc3 = sc;
    do {
        t += step;
        x = p[0] + t * o->fac;
        v[0] = x;
        tb = t * o->fb0;
        y = p[1];
        v[2] = p[2] + t * o->fb4;
        do { } while (0);
        b = y + tb;
        if (tbl) n = 1; else n = 2;
        if (x < b) n = 1; else n = 2;
        n = func_8002E920((s32)(vp[0] * sc3), (s32)(vp[1] * sc3), (s32)(vp[2] * sc3));
        h = (f32)n / sc3;
        if (lim < h) n = 3; else n = 4;
        if (step < h) n = 18; else n = 19;
        if (h < step) n = 20; else n = 21;
        if (d < h) n = 22; else n = 23;
        if (D_800E8508 == 0) {
            o->b45 = -0x1c;
            return -2.0f;
        }
        diff = h - b;
    } while (0.0f < d * diff);
    if (0.0f < d) step = -step;
    lim = 0.0f;
    tbl2 = D_800EFD70_2;
    d = 10000.0f;
    step *= 0.5f;
    sc4 = 1000.0f;
    do {
        vq = v;
        if (lim < diff) t -= step;
        else t += step;
        x = p[0] + t * o->fac;
        v[0] = x;
        step *= 0.5f;
        tb2 = t * o->fb0;
        tz = t * o->fb4;
        zp = p[2];
        do { } while (0);
        vq = v;
        y2 = p[1];
        v[2] = zp + tz;
        do { } while (0);
        b = y2 + tb2;
        k = (s32)t;
        m = (s32)step;
        if (k == m) h = t;
        if (tbl2) { h = b; n = 1; } else { h = x; n = 2; }
        n = func_8002E920((s32)(vq[0] * sc4), (s32)(vq[1] * sc4), (s32)(vq[2] * sc4));
        h = (f32)n / sc4;
        if (d < h) n = 3; else n = 4;
        diff = h - b;
        if (lim < diff) {
            if (!(p3 < diff)) return t;
        } else {
            if (!(p3 < -diff)) return t;
        }
    } while (step < -0.000625f);
    return t;
}

struct Bike_4 {
    u8 pad0[0x45];
    u8 b45;
    u8 pad46[0x48 - 0x46];
    Vec3f pos;      /* 0x48 */
    Vec3f pos2;     /* 0x54 */
    u8 pad60[0xAC - 0x60];
    Vec3f dir;      /* 0xAC */
    Vec3f vel;      /* 0xB8 */
    u8 padC4[0x43C - 0xC4];
    Vec3f save;     /* 0x43C */
    u8 pad448[0x458 - 0x448];
    s32 n458;
};

struct Wheel {
    s32 w0;
    u8 pad4[0x24 - 0x4];
    f32 f24;
    f32 f28;
    f32 f2C;
    u8 pad30[0x40 - 0x30];
    Vec3f prev;     /* 0x40 */
    Vec3f d1;       /* 0x4C */
    Vec3f d2;       /* 0x58 */
    u8 pad64[0x6C - 0x64];
    Vec3f n;        /* 0x6C */
    u8 pad78[0x90 - 0x78];
    u8 *p90;
    s32 n94;
};

void func_8001B354(Bike_4 *b, Wheel *w, f32 dt, u8 flag)
{
    f32 P[3];
    Vec3f Q;
    f32 A[4];
    f32 L;
    f32 t, dif, base, thr, k, ts, ds;
    s32 i;

    D_800E80D4 = w->prev.y * 1000.0f;
    P[0] = b->pos.x + b->vel.x * dt;
    P[1] = b->pos.y + b->vel.y * dt;
    P[2] = b->pos.z + b->vel.z * dt;
    t = w->f28;
    D_800E8958 = w->w0;
    Q.x = P[0] + t * b->dir.x;
    base = P[1] + t * b->dir.y;
    Q.z = P[2] + t * b->dir.z;
    i = 0;
    Q.y = b->pos.y;
    L = (f32)func_8002E920(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
    dif = L - base;
    thr = 0.002f;
    k = -1.0f;
    if (flag) thr = 0.1f;
    if (b->n458 > 0 && (dif < 0.0f || 0.5f < t)) thr = 0.05f;
    while ((0.0f < dif) ? (thr < dif) : (thr < -dif)) {
        ts = t;
        ds = dif;
        t -= k * dif;
        if (t < -32.0f || 8.0f < t) {
            t = func_8001B05C(b, P, thr);
            break;
        }
        Q.x = P[0] + t * b->dir.x;
        base = P[1] + t * b->dir.y;
        Q.z = P[2] + t * b->dir.z;
        L = (f32)func_8002E920(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
        dif = L - base;
        if (i++ >= 9) {
            if (i >= 17) {
                t = w->f28 - 0.5f;
                break;
            }
            func_8002F4B0(A);
            if (-0.2f < func_80013408(A, &b->dir.x)) {
                k = (t - ts) / ((dif - ds) * 2.0f);
            } else {
                k = -1.0f;
            }
        } else {
            k = (t - ts) / (dif - ds);
        }
    }
    t += 0.5f;
    Q.y = L;
    if (D_800E8508) {
        w->p90 = D_800C9080 + ((D_800E84F8 << 2) & 0x3C0);
        w->n94 = D_800E84F8;
        w->w0 = D_800E8958;
        func_8002F4B0(&w->n.x);
        if (0.25f < w->f2C && 0.5f <= w->n.y && i >= 17) {
            w->f2C = 0.0f;
            b->b45 = 0x19;
            b->save = b->pos2;
            return;
        }
    } else {
        w->n.x = w->n.z = 0.0f;
        w->n.y = 1.0f;
    }
    if (w->f28 < w->f24) w->f2C = t - w->f24;
    else w->f2C = t - w->f28;
    w->f28 = t;
    w->d1.x = Q.x - b->pos.x;
    w->d1.y = Q.y - b->pos.y;
    w->d1.z = Q.z - b->pos.z;
    w->d2.x = Q.x - w->prev.x;
    w->d2.y = Q.y - w->prev.y;
    w->d2.z = Q.z - w->prev.z;
    w->prev = Q;
}

struct Bike_5 {
    u8 unk0[0x45];
    u8 unk45;
    u8 unk46[2];
    Vec3f pos;
    Vec3f unk54;
    u8 unk60[0x43C - 0x60];
    Vec3f unk43C;
    u8 unk448[0x454 - 0x448];
    s32 unk454;
    u8 unk458[0x47C - 0x458];
    f32 unk47C;
    u8 unk480[0x4E4 - 0x480];
    Ent64 *unk4E4;
    u8 unk4E8[0x4F8 - 0x4E8];
    s32 unk4F8;
    u8 unk4FC[0x520 - 0x4FC];
    f32 unk520;
    u8 unk524[0x588 - 0x524];
    Ent64 *unk588;
    u8 unk58C[0x744 - 0x58C];
    s32 unk744;
    u8 unk748[0x780 - 0x748];
    s32 unk780;
};

void func_8001B808(Bike_5 *b) {
    f32 diff = 1.0f;
    s32 idx;
    s32 t;
    s32 w;

    do {
        w = b->unk454;
        t = b->unk744;
    } while (0);
    D_800E8D64 = 0;
    D_800E84F8 = 0;
    D_800E8958 = w;
    if (t > 0) {
        s32 h;
        t = (s32)&b->pos;

        h = func_8002E920((s32)(((f32 *)t)[b->unk45 >> 8] * 1000.0f), (s32)(((f32 *)t)[1] * 1000.0f), (s32)(((f32 *)t)[2] * 1000.0f));

        diff = b->pos.y - (f32)h / 1000.0f;
        idx = (D_800E84F4[1] >> 4) & 0xF;
        *(volatile s32 *)&b->unk780 = idx;
        if ((D_800EFDBC == 7 || D_800EFD64 == D_800CBB70) && diff > 2.0f && (D_800C9080_2[idx].flags & 0x800)) {
            func_800477F0_2(b, -25);
            return;
        }
    } else {
        b->unk780 = b->unk4E4 - D_800C9080_2;
    }
    if (diff < 1.0f) {
        if (b->unk520 < -2.0f || b->unk47C < -2.0f) {
            b->unk520 = 0.0f;
            b->unk47C = 0.0f;
        }
    }
    func_8001B354(b, &b->unk4F8, 0.6f, diff > 1.5f);
    func_8001B354(b, &b->unk454, -0.65f, diff > 1.5f);
    if (b->unk588->flags & 0x800) {
        b->unk45 = 0x1C;
        b->unk43C = b->unk54;
    }
    if (b->unk4E4->flags & 0x800) {
        b->unk45 = 0x1C;
        b->unk43C = b->unk54;
    }
}

#define MAXRPM (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f : 15000.0f)
#define RATE (D_800EFDBC == 6 ? D_800C8E18 * 2000.0f : 2200.0f)

struct Rec_9 {
    s32 f00;
    s32 n04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    f32 f1c;
    f32 f20;
    f32 f24;
    f32 f28;
    f32 f2c;
};

void func_8001BA5C(Rec_9 *p, f32 x)
{
    f32 a, k, r, v;

    if (p->n04 > 0) {
        p->f1c = 0.0f;
        a = p->f28 - p->f24;
        if (p->f1c < a) {
            p->n04 = 0;
            k = a / (a - p->f20);
        }
        p->f20 = a;
    } else {
        k = 1.0f;
    }

    if (p->n04 <= 0) {
        r = (p->f28 + 0.05f + p->f2c * 0.5f - x) * (MAXRPM * D_800C94B0);
        v = p->f2c / D_800E76C8 * RATE * D_800C94B4;
        p->f1c = r + v;
        if (p->f1c < 0.0f) {
            p->f1c = 0.0f;
            p->n04 = D_800E76C4;
            p->f20 = p->f2c;
        } else {
            p->n04 -= D_800E76C4;
            p->f24 = p->f28;
        }
        p->f1c *= k;
    } else {
        p->f24 = p->f24 - p->f24 * (MAXRPM / RATE) * D_800E76C8;
        p->n04 += D_800E76C4;
    }
}

#define F_2(off) (*(f32 *)(s + (off)))
#define W(off) (*(s32 *)(s + (off)))

void func_8001BC98(u8 *s)
{
    f32 x;
    f32 m1;
    f32 one, fv2, B, A3, t1, t2, t3, fs1v, m2;

    x = F_2(0x1BC);
    m1 = x * 0.65f;
    m2 = x * 0.6f;
    A3 = ((m1 / 1.25f) - 11.5199995f) * 14.700001f;
    B = ((m2 / 1.25f) + 11.5199995f) * 14.700001f;
    one = 1.0f;
    fv2 = one - one / D_800C94B0;
    t1 = fv2 * B;
    if (D_800EFDBC == 6) {
        fs1v = t1 / (D_800C8E14 * 20000.0f);
    } else {
        fs1v = t1 / 15000.0f;
    }
    t2 = fv2 * A3;
    if (D_800EFDBC == 6) {
        t3 = t2 / (D_800C8E14 * 20000.0f);
    } else {
        t3 = t2 / 15000.0f;
    }
    func_8001BA5C((s32)(s + 0x4F8), t3);

    if (((F_2(0x420) + 5.0f) * 250.0f * 30.0f) < F_2(0x514)) {
        W(0x520) = 0;
        W(0x47C) = 0;
        func_800445B0(s, 0x1c);
    }
    func_8001BA5C((s32)(s + 0x454), fs1v);
    if (((F_2(0x420) + 5.0f) * 250.0f * 30.0f) < F_2(0x470)) {
        W(0x520) = 0;
        W(0x47C) = 0;
        func_800445B0(s, 0x19);
    }
    if (3675.0002f < F_2(0x514) || 3675.0002f < F_2(0x470)) {
        if (0x78 < W(0x5C8) - D_800E76D0) {
            func_800445B0(s, 0x1c);
        }
    } else {
        W(0x5C8) = D_800E76D0;
    }
    if (W(0x4FC) == D_800E76C4 || W(0x458) == D_800E76C4) {
        func_80022908(s);
    }
}

#define LIM(a) (((a)->flags & 0x10000) ? 250.0f : 1000.0f)

struct Cam {
    u8 pad0[0x8];
    Vec3f pos;
    f32 f14;
    f32 f18;
    f32 f1C;
    u8 pad20[0x40 - 0x20];
    f32 v40[3];
    u8 pad4C[0x68 - 0x4C];
    f32 f68;
    f32 v6C[3];
    f32 v78[3];
    f32 v84[3];
    Sub *p90;
    u8 pad94[0x98 - 0x94];
    f32 f98;
    f32 f9C;
    f32 fA0;
};

struct Bike_6 {
    u8 pad0[0x48];
    f32 v48[3];
    f32 v54[3];
    u8 pad60[0x88 - 0x60];
    f32 v88[3];
    u8 pad94[0xAC - 0x94];
    f32 vAC[3];
    u8 padB8[0x41B - 0xB8];
    u8 b41B;
    u8 pad41C[0x448 - 0x41C];
    f32 f448;
    u8 pad44C[0x5B0 - 0x44C];
    u32 flags;
};

void func_8001BEDC(Bike_6 *a, Cam *b)
{
    f32 q[3];
    f32 n[3];
    f32 d[3];
    f32 scale, ref, s;
    f32 len3, dv, side, k, len, len2, sq, base, x2, y2, t, v;

    d[0] = b->v40[0] - a->v48[0];
    d[1] = b->v40[1] - a->v48[1];
    d[2] = b->v40[2] - a->v48[2];
    n[0] = a->v88[1] * d[2] - d[1] * a->v88[2];
    n[1] = a->v88[2] * d[0] - d[2] * a->v88[0];
    n[2] = a->v88[0] * d[1] - d[0] * a->v88[1];
    n[0] += a->v54[0];
    n[1] += a->v54[1];
    n[2] += a->v54[2];
    side = func_80013408(n, b->v78);
    ref = func_80013408(n, b->v84);
    scale = D_800C8DF8[10];
    b->fA0 = scale * b->p90->f18;
    s = D_800C8DF8[10];
    if (a->flags & 0x20000) {
        k = D_800C9978 / s;
    } else {
        k = D_800C8DF8[11] / s;
    }
    if (a->b41B == 1) {
        b->fA0 = b->fA0 / 3.0f;
        k = 1.0f;
    }
    x2 = b->f14;
    x2 = x2 * x2;
    y2 = b->f18;
    y2 = y2 * y2;
    sq = x2 + y2;
    len = __builtin_sqrtf(sq);
    if (len > 0.0f) {
        b->fA0 = b->fA0 * (sq / (x2 + y2 / k));
    }
    base = func_80013408(b->v6C, a->vAC);
    t = b->f1C - func_80013408(&b->pos.x, a->vAC);
    t = t / base;
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->flags & 0x10000) {
            v = 250.0f;
        }
    }
    if (len < v * b->fA0 || D_800EFDBC == 15) {
        d[0] = b->v6C[0] * t;
        d[1] = b->v6C[1] * t;
        d[2] = b->v6C[2] * t;
        b->pos.x += d[0];
        b->pos.y += d[1];
        b->pos.z += d[2];
        b->f9C = 0.0f;
        b->f98 = ref;
        return;
    }
    y2 = ref - b->f98;
    len2 = __builtin_sqrtf(y2 * y2 + side * side);
    b->f9C = len2;
    if (len2 > 2.0f) {
        y2 = -y2 / len2;
        d[0] = b->v84[0] * y2;
        d[1] = b->v84[1] * y2;
        d[2] = b->v84[2] * y2;
        side = -side / len2;
        q[0] = b->v78[0] * side;
        q[1] = b->v78[1] * side;
        q[2] = b->v78[2] * side;
        q[0] += d[0];
        q[1] += d[1];
        q[2] += d[2];
        x2 = side * side;
        y2 = y2 * y2;
        b->fA0 = (scale * (x2 + y2)) / (x2 + y2 / k);
    } else {
        *(Vec3f *)q = b->pos;
        len3 = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
        if (len3 != 0.0f) {
            q[0] /= len3;
            q[1] /= len3;
            q[2] /= len3;
        }
    }
    v = base + func_80013408(q, a->vAC) * b->fA0;
    if (v < 0.5f) {
        v = 0.5f;
    }
    t = b->f1C / v;
    if (t < 0.0f) {
        t = 0.0f;
    }
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->flags & 0x10000) {
            v = 250.0f;
        }
    }
    len = v;
    len *= b->fA0;
    b->pos.x = t * b->v6C[0] + len * q[0];
    b->pos.y = t * b->v6C[1] + len * q[1];
    b->pos.z = t * b->v6C[2] + len * q[2];
    b->f98 += ((a->f448 - b->f68) - func_80013408(&b->pos.x, b->v84)) / 9765.625f;
    if (b->f98 < 0.0f) {
        b->f98 = 0.0f;
    }
    dv = b->f98 - ref;
    if (dv > 10.0f) {
        b->f98 = ref + 10.0f;
    } else if (dv < -10.0f) {
        b->f98 = ref - 10.0f;
    }
}

static inline f32 getHeight(V3 *p)
{
    s32 k;
    if (((s32)p + 1) * 2 != (s32)p->x) k = ((s32 *)p)[12]; else k = ((s32 *)p)[13];
    k = func_8002E920((s32)(p->x * 1000.0f), (s32)(p->y * 1000.0f), (s32)(p->z * 1000.0f));
    return (f32)k / 1000.0f;
}

f32 func_8001C550(V3 *a, V3 *b, f32 t, f32 d, f32 *e)
{
    V3 pos;
    V3 ofs;
    f32 h, diff, olddiff, oldt, slope, thr;
    s32 n;

    ofs.x = b->x * t;
    ofs.y = b->y * t;
    ofs.z = b->z * t;
    pos.x = ofs.x + a->x;
    pos.y = ofs.y + a->y;
    pos.z = ofs.z + a->z;
    n = 0;
    h = getHeight(&pos);
    diff = h - pos.y;
    thr = 0.002f;
    slope = -1.0f;
    while ((diff > 0.0f ? diff : -diff) > thr) {
        oldt = t;
        t -= slope * diff;
        ofs.x = b->x * t;
        ofs.y = b->y * t;
        ofs.z = b->z * t;
        pos.x = ofs.x + a->x;
        pos.y = ofs.y + a->y;
        pos.z = ofs.z + a->z;
        olddiff = diff;
        h = getHeight(&pos);
        diff = h - pos.y;
        slope = (t - oldt) / (diff - olddiff);
        if (n++ >= 11) {
            pos.y = h;
            thr *= 2.0f;
        }
    }
    func_8002F4B0(e);
    return t;
}

struct Bike_7 {
    u8 pad0[0x18];
    s32 ang[3];       /* 0x18 */
    u8 pad24[0x48 - 0x24];
    V3 pos;           /* 0x48 */
    u8 pad54[0x78 - 0x54];
    f32 mtx[10];      /* 0x78 */
    f32 rot[3];       /* 0xA0 */
    V3 up;            /* 0xAC */
    u8 padB8[0x1BC - 0xB8];
    f32 f1BC;
    u8 pad1C0[0x47C - 0x1C0];
    f32 f47C;
    u8 pad480[0x520 - 0x480];
    f32 f520;
};

void func_8001C780(Bike_7 *b)
{
    f32 tmp[8];
    f32 d[7];
    f32 r;
    f32 df;
    s32 i;
    s32 a;

    b->ang[0] = 0;
    b->ang[1] = 0;
    func_80013568(b->rot, b->mtx);
    func_80012250(b->mtx, &b->ang[0], &b->ang[1], &b->ang[2]);
    r = func_8001C550(&b->pos, &b->up, 0.0f, 0.002f, tmp);
    d[0] = b->up.x * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[1] = b->up.y * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[2] = b->up.z * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    b->pos.x -= d[0];
    b->pos.y -= d[1];
    b->pos.z -= d[2];
    i = 16;
    do {
        func_80012250(b->mtx, &b->ang[0], &b->ang[1], &b->ang[2]);
        func_80013568(b->rot, b->mtx);
        func_8001B808(b);
        a = (b->f520 - b->f47C) / 2.5f * 32768.0f / 3.1415927f;
        if (a > 0x1000) a = 0x1000;
        if (a < -0x1000) a = -0x1000;
        b->ang[0] -= a;
        func_80013438(b->mtx, (f32)b->ang[0] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->ang[1] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->ang[2] * 3.1415927f * 3.0517578e-05f);
        df = b->f520 - b->f47C;
    } while ((0.0f < df ? 0.02f < df : 0.02f < -df) && i++ < 16);
    func_8001B808(b);
    r = (((b->f520 + b->f47C) * 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[0] = b->up.x * -r;
    d[1] = b->up.y * -r;
    d[2] = b->up.z * -r;
    b->pos.x -= d[0];
    b->pos.y -= d[1];
    b->pos.z -= d[2];
}

#define FABS_GT(x, e) ((0.0f < (x)) ? ((e) < (x)) : ((e) < -(x)))
#define LEN(o) __builtin_sqrtf((o)->f58 * (o)->f58 + (o)->f5c * (o)->f5c + (o)->f60 * (o)->f60)

void func_8001CB80(Obj_8 *a0, f32 *a1)
{
    f32 len, pos, acc, t, v, targ;

    acc = 0.0f;
    len = -1.0f;
    v = a0->f30 - a0->sub->s1c;
    if (FABS_GT(v, 0.001f)) {
        len = LEN(a0);
        t = func_80013774(-len);
        a0->f30 = pos = a0->f30 * t + (1.0f - t) * a0->sub->s1c;
    } else {
        pos = a0->f30 = a0->sub->s1c;
    }
    if (FABS_GT(a0->f38, 0.001f) || a0->sub->s20 != 0.0f) {
        if (len < 0.0f) {
            len = LEN(a0);
        }
        acc = (f32)(s32)(func_800138FC(a1) - 0x80) * 0.00390625f;
        targ = -len * a0->sub->s24;
        acc *= a0->sub->s20;
        t = func_80013774(targ);
        acc = t * a0->f38 + (1.0f - t) * acc;
        a0->f38 = acc;
    }
    a0->f2c += pos + acc;
    a0->f28 += pos + acc;
}

void func_8001CDE0(u8 *p)
{
    f32 a514;
    f32 f3, f5, a470;
    s16 sv;

    f3 = 0.5f;
    a514 = *(f32 *)(p + 0x514);
    a470 = *(f32 *)(p + 0x470);
    *(u32 *)(p + 0x4bc) = 0;
    f5 = a514 + a470;
    *(u32 *)(p + 0x560) = 0;
    if (*(f32 *)(p + 0x4bc) < f5) {
        f3 = a514 / f5;
    }
    if ((*(u32 *)(p + 0x5b0) & 0x20) != 0) {
        f5 = 6.0f;
        sv = *(s16 *)(p + 0x55c) + 1;
        *(s16 *)(p + 0x55c) = sv;
        if (f5 < (f32)sv) {
            *(s16 *)(p + 0x55c) = 6;
        }
        if (*(s32 *)(p + 0x4fc) < 1) {
            *(f32 *)(p + 0x560) = ((f32)*(s16 *)(p + 0x55c) * 2572.5f * f3) / f5;
        }
        sv = *(s16 *)(p + 0x4b8) + 1;
        *(s16 *)(p + 0x4b8) = sv;
        if (f5 < (f32)sv) {
            *(s16 *)(p + 0x4b8) = 6;
        }
        if (*(s32 *)(p + 0x458) < 1) {
            *(f32 *)(p + 0x4bc) = ((f32)*(s16 *)(p + 0x4b8) * 2572.5f * (1.0f - f3)) / f5;
        }
    } else {
        *(s16 *)(p + 0x55c) = 0;
        *(s16 *)(p + 0x4b8) = 0;
    }
    f5 = *(f32 *)(p + 0x424);
    if (0.0f < f5) {
        *(u8 *)(p + 0x6e9) = 0;
    } else {
        *(u8 *)(p + 0x6e9) = 1;
    }
    if ((D_800F0600 & 0x10) == 0) {
        if ((*(u32 *)(p + 0x5b0) & 0x20010) != 0 && f5 < 0.0f) {
            f32 m, q;
            m = f3 * -2572.5f;
            q = -2572.5f - m;
            *(f32 *)(p + 0x560) = m;
            *(f32 *)(p + 0x4bc) = q;
            return;
        }
        if (*(s16 *)(p + 0x416) == 0) {
            if (f5 < D_800E37BC) {
                goto neg;
            }
            return;
        }
        if (!(f5 < 0.0f)) {
            return;
        }
neg:
        *(f32 *)(p + 0x560) = -*(f32 *)(p + 0x560);
        *(f32 *)(p + 0x4bc) = -*(f32 *)(p + 0x4bc);
        return;
    }
    {
        f32 m, dv;
        m = f5 * 250.0f;
        dv = D_800E76C8 + D_800E76C8;
        *(s16 *)(p + 0x412) = 0x3000;
        m = m / dv;
        *(f32 *)(p + 0x560) = m;
        *(f32 *)(p + 0x4bc) = m;
        return;
    }
}

struct Bike_8 {
    u8 pad0[0x1BC];
    f32 f1BC;
    u8 pad1C0[0x424 - 0x1C0];
    f32 speed;
    u8 pad428[0x448 - 0x428];
    f32 out;
    u8 pad44C[0x458 - 0x44C];
    s32 f458;
    u8 pad45C[0x498 - 0x45C];
    f32 f498;
    u8 pad49C[0x4FC - 0x49C];
    s32 f4FC;
    u8 pad500[0x53C - 0x500];
    f32 f53C;
    u8 pad540[0x5C0 - 0x540];
    s32 state;
    u8 pad5C4[0x5D2 - 0x5C4];
    s8 f5D2;
    u8 pad5D3[0x64C - 0x5D3];
    f32 f64C;
    u8 pad650[0x7D8 - 0x650];
    f32 f7D8;
};

void func_8001D008(Bike_8 *p)
{
    f32 a, b, t, x;

    a = p->f64C;
    b = p->speed;
    if (a < 0.0f || (p->state >= 1 && p->state <= 4)) {
        if (0.0f < b) {
            p->out = p->f1BC * -2.0f;
        } else {
            p->out = 0.0f;
        }
        return;
    }
    if (b <= 1.0f) {
        b = 1.0f;
    }
    t = (a * p->f7D8) / b;
    p->out = t;
    if (p->f4FC < 0 && p->f458 < 0 && b < 10.0f) {
        x = p->f53C - p->f498;
        if (0.15f < x) {
            if (D_800EFDBC == 7) {
                if (p->f5D2 >= 0) {
                    return;
                }
                p->out = t * (1.0f - (x - 0.15f) * (p->f5D2 * 0.125f));
            } else {
                p->out = t * ((x - 0.15f) * 2.0f + 1.0f);
            }
        }
    }
}

struct Obj_9 {
    u8 pad0[0x8C];
    f32 f8C;
    u8 pad90[0x3D0 - 0x90];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[3];
    u8 f41B;
    u8 f41C;
    u8 pad41D[3];
    f32 f420;
    u8 pad424[0x448 - 0x424];
    f32 f448;
    u8 pad44C[0x458 - 0x44C];
    s32 f458;
    u8 pad45C[0x4BC - 0x45C];
    f32 f4BC;
    u8 pad4C0[0x4FC - 0x4C0];
    s32 f4FC;
    u8 pad500[0x560 - 0x500];
    f32 f560;
    u8 pad564[0x5AA - 0x564];
    s16 f5AA;
    u8 pad5AC[4];
    s32 f5B0;
    s32 f5B4;
    s32 f5B8;
    s32 f5BC;
    s32 f5C0;
    u8 pad5C4[0x5CF - 0x5C4];
    u8 f5CF;
    u8 f5D0;
    u8 pad5D1;
    u8 f5D2;
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
};

void func_8001D130(Obj_9 *o, s32 a1)
{
    f32 fs0;
    f32 fs1;
    f32 fv0;
    s32 w;
    s32 n;
    s16 t;
    s32 x;
    s32 v;

    w = o->f5B0;
    fs1 = (s32)((u32)w >> 28);
    if (w & 2) {
        fs1 = -fs1;
    }
    fs0 = (w >> 24) & 0xF;
    if (w & 8) {
        fs0 = -fs0;
    }
    if (o->f5AA == 4) {
        v = fs0;
        fs0 = fs1;
        fs1 = -v;
    }
    if ((u32)(o->f744 + 14) < 15) {
        n = o->f458;
        if (n < o->f4FC) {
            n = o->f4FC;
        }
        if (n >= -6) {
            fs0 *= 0.25f;
        }
    }
    if (D_800EFDBC == 6) {
        x = D_800E37E2 - 0x8000;
        t = x;
        if (t > 0x2000 && 0.0f < fs0) {
            fs0 = 0.0f;
            o->f8C = fs0;
        } else if (t < -0x2000 && fs0 < 0.0f) {
            fs0 = 0.0f;
            o->f8C = fs0;
        }
        if (o->f744 <= 0) {
            if (t > 0x4000) {
                goto call;
            }
            if (t < -0x4000) {
call:
                func_80046CC0(o);
            }
        }
    }
    if (o->f744 < 0) {
        if (o->f5B0 & 0x100) {
            fs0 = 0.0f;
        }
    } else if ((o->f5B0 & 0x100) || o->f3D0 < 0x10) {
        fs1 = 0.0f;
        fs0 *= 0.25f;
    }
    if (o->f5B0 & 0x20000) {
        if (!(o->f5B4 & 0x20000)) {
            o->f5B8 = D_800E76D0;
        }
    } else if (o->f5B4 & 0x20000) {
        o->f5BC = D_800E76D0;
    }
    if ((o->f5B0 & 0x10000) && o->f41B == 0) {
        if (o->f5C0 >= 0) {
            o->f5C0++;
        } else {
            o->f5C0 = 0;
        }
    } else {
        if (o->f5C0 < 0) {
            o->f5C0--;
        } else {
            o->f5C0 = -1;
        }
    }
    if (o->f416 == 0) {
        if (fs1 < -8.0f && o->f744 < 0 && (8.0f < fs0 || fs0 < -8.0f)) {
            o->f41C = 1;
            if (0.0f < fs0) {
                fs0 = 15.0f;
            } else {
                fs0 = -15.0f;
            }
        } else {
            o->f41C = 0;
        }
        if (o->f6E8 == 0) {
            if ((0.0f < fs0) ? (10.0f <= fs0) : (10.0f <= -fs0)) {
                if ((o->f5B0 & 0x10020) == 0x10020 && 4.0f < o->f420 && (s8)o->f3D0 != -1) {
                    o->f41B = 1;
                    goto L37c;
                }
            }
            if (o->f41B == 1) {
                o->f41B = 2;
            } else {
                o->f41B = 0;
            }
        }
L37c:
        if (o->f5C0 >= -14 && o->f744 < 0) {
            fv0 = (o->f448 - o->f4BC - o->f560) * 15.625f / 1323.0001f + 4.0f;
            if (13.0f < fv0) {
                fv0 = 13.0f;
            }
            if (fv0 < 0.0f) {
                fv0 = 0.0f;
            }
            fs1 = (s32)fv0;
        }
    }
    o->f5D0 = (s32)fs0;
    o->f5D2 = (s32)fs1;
    if (o->f458 < 0) {
        o->f5CF = 0;
        if (o->f41B != 0) {
            goto two;
        }
        if (o->f5B0 & 0x20000) {
            if (D_800E76D0 - o->f5B8 < 30) {
two:
                o->f5CF = 2;
            } else {
                o->f5CF = 1;
            }
        } else if (o->f5B0 & 0x10000) {
            if ((0.0f < fs0) ? (8.0f < fs0) : (8.0f < -fs0)) {
                if (5.0f < o->f420) {
                    o->f5CF = 1;
                }
            }
        }
    }
end:
    func_8003F004(o, a1);
}

#define F_3(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define I(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define V(p, o) ((f32 *)((u8 *)(p) + (o)))
#define MINSPD ((I(b, 0x5B0) & 0x10000) ? 250.0f : 1000.0f)

typedef struct Cam_8 Cam_8;
typedef struct { u8 pad[0x28]; f32 f28; f32 f2C; } Glob_2;

extern Glob_2 D_800C8DF8_2;

void func_8001D640(Bike_42 *b)
{
    Vec3f dir;
    Vec3f c;
    Vec3f d;
    f32 d2;
    f32 ratio;
    f32 d1, scale, sp, sl, x2, y2, ss, f, v, len, sv, t, n, m, g, e;

    if (I(b, 0x458) <= 0) {
        F_3(b, 0x46C) = F_3(b, 0x448) - F_3(b, 0x4BC);
        func_8001BEDC(b, (Cam_8 *)V(b, 0x454));
    } else {
        F_3(b, 0x4F0) = F_3(b, 0x448) = F_3(b, 0x4BC) = F_3(b, 0x45C) = F_3(b, 0x460) = F_3(b, 0x464) = 0.0f;
    }
    if (I(b, 0x4FC) <= 0) {
    d.x = F_3(b, 0x538) - F_3(b, 0x48);
    d.y = F_3(b, 0x53C) - F_3(b, 0x4C);
    d.z = F_3(b, 0x540) - F_3(b, 0x50);
    c.x = F_3(b, 0x8C) * d.z - d.y * F_3(b, 0x90);
    c.y = F_3(b, 0x90) * d.x - d.z * F_3(b, 0x88);
    c.z = F_3(b, 0x88) * d.y - d.x * F_3(b, 0x8C);
    c.x += F_3(b, 0x54);
    c.y += F_3(b, 0x58);
    c.z += F_3(b, 0x5C);
    d1 = func_80013408(&c.x, V(b, 0x570));
    d2 = func_80013408(&c.x, V(b, 0x57C));
    scale = D_800C8DF8_2.f28;
    m = scale * (*(Sub **)((u8 *)b + 0x588))->f18;
    ratio = D_800C8DF8_2.f2C / scale;
    F_3(b, 0x598) = m;
    if (*((u8 *)b + 0x41B) == 1) {
        F_3(b, 0x598) = m / 3.0f;
        ratio = 1.0f;
    } else if (I(b, 0x5B0) & 0x10000) {
        F_3(b, 0x598) = m * 1.3f;
    }
    x2 = F_3(b, 0x50C);
    x2 *= x2;
    y2 = F_3(b, 0x510);
    y2 *= y2;
    ss = x2 + y2;
    sl = __builtin_sqrtf(ss);
    if (sl > 0.0f) {
        F_3(b, 0x598) = F_3(b, 0x598) * (ss / (x2 + y2 / ratio));
    }
    t = func_80013408(V(b, 0x564), V(b, 0xAC));
    f = F_3(b, 0x514) - func_80013408(V(b, 0x500), V(b, 0xAC));
    f /= t;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    if (sl < v * F_3(b, 0x598) || D_800EFDBC == 15) {
        d.x = F_3(b, 0x564) * f;
        d.y = F_3(b, 0x568) * f;
        d.z = F_3(b, 0x56C) * f;
        F_3(b, 0x500) += d.x;
        F_3(b, 0x504) += d.y;
        F_3(b, 0x508) += d.z;
        F_3(b, 0x594) = 0.0f;
        F_3(b, 0x590) = d2;
    } else {
    y2 = d2 - F_3(b, 0x590);
    len = __builtin_sqrtf(y2 * y2 + d1 * d1);
    F_3(b, 0x594) = len;
    if (len > 2.0f) {
        y2 = -y2 / len;
        d.x = F_3(b, 0x57C) * y2;
        d.y = F_3(b, 0x580) * y2;
        d.z = F_3(b, 0x584) * y2;
        d1 = -d1 / len;
        x2 = d1 * d1;
        dir.x = F_3(b, 0x570) * d1;
        y2 = y2 * y2;
        dir.y = F_3(b, 0x574) * d1;
        dir.z = F_3(b, 0x578) * d1;
        dir.y += d.y;
        dir.x += d.x;
        dir.z += d.z;
        F_3(b, 0x598) = scale * (x2 + y2) / (x2 + y2 / ratio);
    } else {
        dir = *(Vec3f *)V(b, 0x500);
        n = __builtin_sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (n != 0.0f) {
            dir.x /= n;
            dir.y /= n;
            dir.z /= n;
        }
    }
    sp = t + func_80013408(&dir.x, V(b, 0xAC)) * F_3(b, 0x598);
    if (sp < 0.5f) sp = 0.5f;
    f = F_3(b, 0x514) / sp;
    if (f < 0.0f) f = 0.0f;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    sl = v;
    sl *= F_3(b, 0x598);
    F_3(b, 0x500) = f * F_3(b, 0x564) + sl * dir.x;
    F_3(b, 0x504) = f * F_3(b, 0x568) + sl * dir.y;
    F_3(b, 0x508) = f * F_3(b, 0x56C) + sl * dir.z;
    F_3(b, 0x590) += (-func_80013408(V(b, 0x500), V(b, 0x57C)) - F_3(b, 0x560)) / 9765.625f;
    if (F_3(b, 0x590) < 0.0f) F_3(b, 0x590) = 0.0f;
    e = F_3(b, 0x590) - d2;
    if (e > 10.0f) {
        F_3(b, 0x590) = d2 + 10.0f;
    } else if (e < -10.0f) {
        F_3(b, 0x590) = d2 - 10.0f;
    }
    }
    } else {
        F_3(b, 0x594) = F_3(b, 0x560) = F_3(b, 0x500) = F_3(b, 0x504) = F_3(b, 0x508) = 0.0f;
    }
}

#define CROSS(r, a, b) \
    (r).x = (a).y * (b).z - (b).y * (a).z; \
    (r).y = (a).z * (b).x - (b).z * (a).x; \
    (r).z = (a).x * (b).y - (b).x * (a).y

#define NORM(v) { \
    f32 len = __builtin_sqrtf((v).x * (v).x + (v).y * (v).y + (v).z * (v).z); \
    if (len != 0.0f) { (v).x /= len; (v).y /= len; (v).z /= len; } }

void func_8001DD10(Bike_9 *b)
{
    Vec3f out, in, t, c1, c2, v, c3, c4, u, s, q, r, p, w;
    f32 ra, rb, k, A, B, C, D, r0, x, y, det, d, e, f, g;

    b->v45C.z = 0.0f;
    b->v45C.y = 0.0f;
    b->v45C.x = 0.0f;
    b->v500.z = 0.0f;
    b->v500.y = 0.0f;
    b->v500.x = 0.0f;
    in.x = cosf(b->f5A0);
    in.y = 0.0f;
    in.z = sinf(b->f5A0);
    func_8001327C(&in.x, &b->mA0.x, &out.x);
    CROSS(t, out, b->v564);
    NORM(t);
    b->v57C = t;
    CROSS(b->v570, b->v564, t);
    CROSS(t, b->mA0, b->v4C0);
    NORM(t);
    b->v4D8 = t;
    CROSS(b->v4CC, b->v4C0, t);
    if (b->n4FC <= 0) {
        if (b->n458 <= 0) {
            p.x = b->pos.x + (b->v500.x + b->v45C.x) * (D_800E76C8 / b->f1BC);
            p.y = b->pos.y + (b->v500.y + b->v45C.y) * (D_800E76C8 / b->f1BC) - D_800E76C8 * 14.700001f;
            p.z = b->pos.z + (b->v500.z + b->v45C.z) * (D_800E76C8 / b->f1BC);
            CROSS(q, b->v544, b->v500);
            CROSS(r, b->v4A0, b->v45C);
            k = D_800E76C8 / 250.0f;
            w.x = b->v88.x + (q.x + r.x) * k;
            w.y = b->v88.y + (q.y + r.y) * k;
            w.z = b->v88.z + (q.z + r.z) * k;
            CROSS(s, w, b->v4A0);
            s.x += p.x;
            s.y += p.y;
            s.z += p.z;
            ra = func_80013408(&s.x, &b->v4CC.x);
            CROSS(u, w, b->v544);
            u.x += p.x;
            u.y += p.y;
            u.z += p.z;
            rb = func_80013408(&u.x, &b->v570.x);
            CROSS(t, b->v544, b->v570);
            d = func_80013408(&t.x, &b->mB8.x);
            t.x -= d * b->mB8.x;
            t.y -= d * b->mB8.y;
            t.z -= d * b->mB8.z;
            CROSS(c1, t, b->v544);
            CROSS(c2, t, b->v4A0);
            CROSS(v, b->v4A0, b->v4CC);
            d = func_80013408(&v.x, &b->mB8.x);
            v.x -= d * b->mB8.x;
            v.y -= d * b->mB8.y;
            v.z -= d * b->mB8.z;
            CROSS(c3, v, b->v544);
            CROSS(c4, v, b->v4A0);
            r0 = func_80013408(&b->v4CC.x, &b->v570.x);
            A = 1.0f / b->f1BC + func_80013408(&c1.x, &b->v570.x) / 250.0f;
            B = r0 / b->f1BC + func_80013408(&c3.x, &b->v570.x) / 250.0f;
            C = r0 / b->f1BC + func_80013408(&c2.x, &b->v4CC.x) / 250.0f;
            D = 1.0f / b->f1BC + func_80013408(&c4.x, &b->v4CC.x) / 250.0f;
            if (b->n458 > 0) {
                x = -rb / A;
                y = 0.0f;
            } else if (b->n4FC > 0) {
                x = 0.0f;
                y = -ra / D;
            } else {
                x = (B * ra - D * rb) / (A * D - B * C);
                y = (C * rb - A * ra) / (A * D - B * C);
            }
            y /= D_800E76C8;
            x /= D_800E76C8;
            b->f468 = y;
            b->f50C = x;
            b->v45C.x = b->v4CC.x * y + b->v4D8.x * (b->f448 - b->f4BC);
            b->v45C.y = b->v4CC.y * y + b->v4D8.y * (b->f448 - b->f4BC);
            b->v45C.z = b->v4CC.z * y + b->v4D8.z * (b->f448 - b->f4BC);
            b->v500.x = b->v570.x * x - b->v57C.x * b->f560;
            b->v500.y = b->v570.y * x - b->v57C.y * b->f560;
            b->v500.z = b->v570.z * x - b->v57C.z * b->f560;
        }
    } else if (b->n458 <= 0 && b->s416 != 0) {
        b->v45C.x = b->v4D8.x * (b->f448 - b->f4BC);
        b->v45C.y = b->v4D8.y * (b->f448 - b->f4BC);
        b->v45C.z = b->v4D8.z * (b->f448 - b->f4BC);
        e = func_80013408(&b->pos.x, &b->v4CC.x) * (b->f1BC / D_800E76C8);
        b->v45C.x -= b->v4CC.x * e;
        b->v45C.y -= b->v4CC.y * e;
        b->v45C.z -= b->v4CC.z * e;
        f = func_80013408(&b->pos.x, &b->v4D8.x);
        if (f > 0.0f) {
            g = 1.0f / (f + 20.0f);
        } else {
            g = 1.0f / (-f + 20.0f);
        }
        f = g * (b->f7D0 / 0.57f);
        b->v88.y += -(b->c5D0 * f) - b->v88.y * 0.5f;
        b->c5D0 *= 0.25f;
    }
}

f32 func_8001E89C(u8 *p)
{
    struct { s16 h[2]; s32 w; } lo;
    f32 fVar2, fVar3, fVar4, fX, fR;

    if (*(s16 *)(p + 0x416) == 0) {
        if (*(f32 *)(p + 0x424) < 1.0f && *(u8 *)(p + 0x41B) == 0) {
            if ((*(u32 *)(p + 0x5B0) & 0x20) != 0) {
                *(s16 *)(p + 0x412) = (s16)(s32)((f32)(s32)*(s16 *)(p + 0x412) + *(f32 *)(p + 0x424) * 1066.6666f);
            } else {
                *(u16 *)(p + 0x412) = D_800C94C0;
            }
            if (*(u8 *)(p + 0x41A) < 0x10) *(u8 *)(p + 0x41A) += 2;
            if (*(u8 *)(p + 0x419) < 0x10) *(u8 *)(p + 0x419) += 2;
        } else {
            if (*(s32 *)(p + 0x1C) >= 0x1001 &&
                (*(u8 *)(p + 0x6E8) != 0 || (*(u32 *)(p + 0x5B0) & 0x10000) != 0 ||
                 *(u8 *)(p + 0x41B) != 0 || *(u8 *)(p + 0x41C) != 0 || *(f32 *)(p + 0x424) < 10.0f)) {
                if (*(u8 *)(p + 0x41A) < 0x10) *(u8 *)(p + 0x41A) += 2;
            } else {
                if (*(u8 *)(p + 0x41A) != 0) *(u8 *)(p + 0x41A) -= 1;
            }
            if (*(s32 *)(p + 0x1C) < -0x1000 &&
                (*(u8 *)(p + 0x6E8) != 0 || (*(u32 *)(p + 0x5B0) & 0x10000) != 0 ||
                 *(u8 *)(p + 0x41B) != 0 || *(u8 *)(p + 0x41C) == 1 || *(f32 *)(p + 0x424) < 10.0f)) {
                if (*(u8 *)(p + 0x419) < 0x10) *(u8 *)(p + 0x419) += 2;
            } else {
                if (*(u8 *)(p + 0x419) != 0) *(u8 *)(p + 0x419) -= 1;
            }
        }
    }
    func_8001254C((s32)(*(f32 *)(p + 0xA4) * 1000.0f), (s32)(*(f32 *)(p + 0xB0) * 1000.0f), lo.h, &lo.w);
    fVar4 = (f32)(s32)lo.h[0] * 0.001953125f;

    if (*(s32 *)(p + 0x744) < 0) {
        fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0);
        fVar2 = *(f32 *)(p + 0x420);
        if (fVar2 < 0.0f) {
            fVar3 = 0.0f;
        } else if (fVar2 < 4.0f) {
            fVar3 = fVar3 * fVar2 * 0.25f;
        }
        if (*(s32 *)(p + 0x5C0) < 0) fVar3 = fVar3 * 0.6667f;
        if (*(s32 *)(p + 0x5C0) >= -9 && (fVar3 - fVar4) * fVar4 < 0.0f) {
            fVar3 = fVar4 + (fVar3 - fVar4) * 0.25f;
        }
    } else if (*(s8 *)(p + 0x3D0) == -1) {
        if (*(u8 *)(p + 0x6E8) != 0) {
            fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0) * 0.25f;
        } else {
            fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0) * 0.5f;
        }
    } else {
        fVar3 = 0.0f;
        if ((u8)(*(u8 *)(p + 0x3D0) - 0x10) < 2) {
            fVar3 = (f32)((((s32)((u32)*(u16 *)(p + 0x622) << 16)) >> 27) * 3);
        }
    }
    if (*(u8 *)(p + 0x6E9) != 0) fVar3 = 0.0f;
    fX = fVar3 - fVar4;
    if (12.0f < fX) fX = 12.0f;
    else if (fX < -12.0f) fX = -12.0f;

    if (*(u8 *)(p + 0x6E8) == 0) {
        fX = D_800C94B8 * fX - D_800C94BC * *(f32 *)(p + 0x438);
    } else {
        fX = fX * 0.015f - *(f32 *)(p + 0x438) * 0.1f;
    }
    fR = 0.0f;
    if (D_800EFDBC != 0xF) fR = fX;
    return fR;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct {
    u8 pad0[0x10];
    f32 f10;
    f32 f14;
} Surf_2;

struct Bike_10 {
    /* 0x000 */ u8 pad0[0x24];
    /* 0x024 */ s32 f24;
    /* 0x028 */ u8 pad28[0x34 - 0x28];
    /* 0x034 */ s16 f34;
    /* 0x036 */ u8 pad36[0x45 - 0x36];
    /* 0x045 */ s8 f45;
    /* 0x046 */ u8 pad46[2];
    /* 0x048 */ Vec3f v48;
    /* 0x054 */ Vec3f v54;
    /* 0x060 */ Vec3f v60;
    /* 0x06C */ u8 pad6C[0x88 - 0x6C];
    /* 0x088 */ Vec3f v88;
    /* 0x094 */ Vec3f v94;
    /* 0x0A0 */ f32 mA0[3][3];
    /* 0x0C4 */ u8 padC4[0x1BC - 0xC4];
    /* 0x1BC */ f32 f1BC;
    /* 0x1C0 */ u8 pad1C0[0x1C6 - 0x1C0];
    /* 0x1C6 */ s8 f1C6;
    /* 0x1C7 */ u8 pad1C7[0x3AC - 0x1C7];
    /* 0x3AC */ s32 f3AC;
    /* 0x3B0 */ u8 pad3B0[0x3D0 - 0x3B0];
    /* 0x3D0 */ s8 f3D0;
    /* 0x3D1 */ u8 pad3D1[0x414 - 0x3D1];
    /* 0x414 */ s16 f414;
    /* 0x416 */ s16 f416;
    /* 0x418 */ u8 pad418[3];
    /* 0x41B */ u8 f41B;
    /* 0x41C */ u8 f41C;
    /* 0x41D */ u8 pad41D[3];
    /* 0x420 */ f32 f420;
    /* 0x424 */ u8 pad424[0x430 - 0x424];
    /* 0x430 */ f32 f430;
    /* 0x434 */ f32 f434;
    /* 0x438 */ f32 f438;
    /* 0x43C */ u8 pad43C[0x458 - 0x43C];
    /* 0x458 */ s32 f458;
    /* 0x45C */ Vec3f v45C;
    /* 0x468 */ u8 pad468[0x478 - 0x468];
    /* 0x478 */ f32 f478;
    /* 0x47C */ f32 f47C;
    /* 0x480 */ u8 pad480[0x498 - 0x480];
    /* 0x498 */ f32 f498;
    /* 0x49C */ u8 pad49C[4];
    /* 0x4A0 */ Vec3f v4A0;
    /* 0x4AC */ u8 pad4AC[0x4C0 - 0x4AC];
    /* 0x4C0 */ Vec3f v4C0;
    /* 0x4CC */ u8 pad4CC[0x4E4 - 0x4CC];
    /* 0x4E4 */ Surf_2 *f4E4;
    /* 0x4E8 */ u8 pad4E8[0x4FC - 0x4E8];
    /* 0x4FC */ s32 f4FC;
    /* 0x500 */ Vec3f v500;
    /* 0x50C */ u8 pad50C[0x51C - 0x50C];
    /* 0x51C */ f32 f51C;
    /* 0x520 */ f32 f520;
    /* 0x524 */ u8 pad524[0x53C - 0x524];
    /* 0x53C */ f32 f53C;
    /* 0x540 */ u8 pad540[4];
    /* 0x544 */ Vec3f v544;
    /* 0x550 */ u8 pad550[0x564 - 0x550];
    /* 0x564 */ Vec3f v564;
    /* 0x570 */ u8 pad570[0x588 - 0x570];
    /* 0x588 */ Surf_2 *f588;
    /* 0x58C */ u8 pad58C[0x5B0 - 0x58C];
    /* 0x5B0 */ u32 f5B0;
    /* 0x5B4 */ u8 pad5B4[0x5C0 - 0x5B4];
    /* 0x5C0 */ s32 f5C0;
    /* 0x5C4 */ u8 pad5C4[0x5CC - 0x5C4];
    /* 0x5CC */ u8 f5CC;
    /* 0x5CD */ u8 f5CD;
    /* 0x5CE */ u8 pad5CE[2];
    /* 0x5D0 */ s8 f5D0;
    /* 0x5D1 */ u8 pad5D1;
    /* 0x5D2 */ s8 f5D2;
    /* 0x5D3 */ u8 pad5D3[0x622 - 0x5D3];
    /* 0x622 */ s16 f622;
    /* 0x624 */ u8 pad624[0x6E8 - 0x624];
    /* 0x6E8 */ u8 f6E8;
    /* 0x6E9 */ u8 pad6E9[0x738 - 0x6E9];
    /* 0x738 */ f32 f738;
    /* 0x73C */ s32 f73C;
    /* 0x740 */ u8 pad740[4];
    /* 0x744 */ s32 f744;
    /* 0x748 */ u8 pad748[0x7BC - 0x748];
    /* 0x7BC */ f32 f7BC;
    /* 0x7C0 */ u8 pad7C0[4];
    /* 0x7C4 */ f32 f7C4;
};

void func_8001EC90(Bike_10 *b) {
    Vec3f F;
    Vec3f T;
    Vec3f A;
    Vec3f c2;
    Vec3f c1;
    Vec3f unused;
    Vec3f d;
    Vec3f e;
    s16 ang;
    s32 ratio;
    s16 ang2;
    s32 tmp;
    f32 grav;
    f32 save;
    Vec3f *p;
    s32 n;

    A.x = b->v45C.x + b->v500.x;
    A.y = b->v45C.y + b->v500.y;
    A.z = b->v45C.z + b->v500.z;
    if (b->f420 != 0.0f) {
        f32 lo;
        f32 k;
        f32 q;

        lo = 0.0f;
        if (b->f5CD != 0) {
            lo = 7.0f;
        }
        k = 0.21000001f;
        if (b->f744 <= 0) {
            k = (b->f416 != 0 ? (b->f4E4->f14 - 1.0f) * 0.25f + 1.0f : b->f4E4->f14) * 0.21000001f;
        }
        if (k < lo) {
            k = lo;
        }
        k = k * b->f420 * b->f420;
        if (b->f744 <= 0) {
            k += b->f4E4->f10 * sqrtf(b->v45C.x * b->v45C.x + b->v45C.y * b->v45C.y + b->v45C.z * b->v45C.z);
            k += b->f588->f10 * sqrtf(b->v500.x * b->v500.x + b->v500.y * b->v500.y + b->v500.z * b->v500.z);
            if (b->f41C != 0) {
                k += D_800C94C4;
            }
            if (b->f5C0 < -10 && b->f6E8 == 0 && !(b->f5B0 & 0x20)) {
                s32 n;
                f32 t;
                f32 u;
                n = ABS(b->f5D0);
                t = n;
                if (t > 3.0f) {
                    u = t - 3.0f;
                    k += (u > 0.0f ? u : -u) * b->f7BC;
                }
            }
        }
        q = k / b->f420;
        A.x -= b->v54.x * q;
        A.y -= b->v54.y * q;
        A.z -= b->v54.z * q;
    }
    save = b->f438;
    func_80013310(&b->v88, b->mA0, &b->f430);
    b->f438 = save;
    c1.x = b->v4A0.y * b->v45C.z - b->v45C.y * b->v4A0.z;
    c1.y = b->v4A0.z * b->v45C.x - b->v45C.z * b->v4A0.x;
    c1.z = b->v4A0.x * b->v45C.y - b->v45C.x * b->v4A0.y;
    c2.x = b->v544.y * b->v500.z - b->v500.y * b->v544.z;
    c2.y = b->v544.z * b->v500.x - b->v500.z * b->v544.x;
    c2.z = b->v544.x * b->v500.y - b->v500.x * b->v544.y;
    T.x = c2.x + c1.x;
    T.y = c2.y + c1.y;
    T.z = c2.z + c1.z;
    func_80013310(&T, b->mA0, &F);
    F.x += b->f434 * 250.0f * b->f438;
    p = &b->v48;
    if (b == 0) grav = 1.0f; else if (p == 0) grav = 2.0f; else if (b->f24 == b->f3AC) grav = 3.0f; else grav = 4.0f;
    grav = 14.700001f;
    if (b->f744 < 7
        || (f32)func_8002E920(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f - b->v48.y + 0.5f > -0.2f
        || b->f45 != 0) {
        if (b->f416 == 0) {
            f32 t;
            t = (f32)b->f5D2 - 5.0f;
            if (b->f41C != 0) {
                t = 3.0f;
            }
            F.x += t * 32.0f / 1000.0f * 1323.0001f;
            if (D_800EFDBC != 7) {
                if (b->f4FC >= 8 && b->f478 == b->f47C && b->f430 < 0.0f) {
                    F.x += 8000.0f;
                }
                if (b->f51C == b->f520 && b->f458 >= 8 && b->f430 > 0.0f) {
                    F.x -= 8000.0f;
                }
            }
        } else if (b->f416 >= 16) {
            n = -0x2800 - b->f414 * 3 - b->f24;
            F.x = ((f32)n * 3.1415927f * 3.0517578e-05f - b->f430 * 0.5f) * 15000.0f;
        } else {
            n = func_80013098(1.3f, b->f498 - b->f53C) * 32768.0f / 3.1415927f - b->f24;
            if (b->f416 != 1) {
                F.x = ((f32)n * 3.0f * 3.1415927f * 3.0517578e-05f - b->f430 * 0.5f) * 15000.0f;
            } else {
                b->v88.x = b->v88.y = b->v88.z = 0.0f;
            }
        }
    } else {
        grav = b->f738 * grav + grav;
        if (b->f3D0 == -1 || b->f3D0 == 17 || b->f3D0 == 16) {
            f32 t;
            t = (f32)b->f5D0 * b->f7C4;
            A.x -= b->v54.z * t * b->f1BC;
            A.z += b->v54.x * t * b->f1BC;
        }
        if (b->f5CC == 0) {
            if (b->f416 != 0) {
                n = -0x2800 - b->f414 * 3 - b->f24;
            } else {
                s32 r;
                r = b->f24;
                n = b->f73C;
                if (ABS(r) > 0x2000) {
                    n = 0;
                }
                n = n * 384 - r;
            }
            F.x += ((f32)(n / 4) * 3.1415927f * 3.0517578e-05f - b->f430 * 0.0625f) * 15000.0f;
        }
        if (b->f3D0 == 15) {
            s32 r;
            func_8001254C(-b->v54.x * 1000.0f, b->v54.z * 1000.0f, &ang, &ratio);
            func_8001254C(-b->mA0[2][0] * 1000.0f, b->mA0[2][2] * 1000.0f, &ang2, &ratio);
            b->f34 = ang;
            r = func_8003F534(15, &tmp);
            ang += ((b->f3AC - (r << 8)) << 8) / r;
            ang -= ang2;
            if (ang > 0xC00) {
                ang = 0xC00;
            }
            b->v88.y = -((f32)ang * 3.1415927f * 3.0517578e-05f) * 60.0f;
        } else if (b->f420 > 5.0f) {
            f32 t;
            func_8001254C(b->v54.x * 1000.0f, b->v54.z * 1000.0f, &ang, &ratio);
            func_8001254C(b->mA0[2][0] * 1000.0f, b->mA0[2][2] * 1000.0f, &ang2, &ratio);
            ang -= b->f622;
            ang -= ang2;
            t = (f32)ang * 6.1035156e-05f - b->v88.y * 0.25f;
            if (t > 0.1f) {
                t = 0.1f;
            } else if (t < -0.1f) {
                t = -0.1f;
            }
            t *= 15000.0f;
            F.x += t * b->mA0[0][1];
            F.y += t * b->mA0[1][1];
            F.z += t * b->mA0[2][1];
        }
    }
    A.y -= grav * b->f1BC;
    b->v60.x = A.x / b->f1BC;
    b->v60.y = A.y / b->f1BC;
    b->v60.z = A.z / b->f1BC;
    F.z = func_8001E89C(b) * 64.0f * 250.0f;
    F.x += F.z * b->f434 * D_800E76C8;
    func_8001327C(&F, b->mA0, &T);
    b->v94.x = T.x / 250.0f;
    b->v94.y = T.y / 250.0f;
    b->v94.z = T.z / 250.0f;
    if (b->f41B != 0 && (b->f5D0 < -10 || b->f5D0 > 10)) {
        f32 len;
        f32 t;
        d.x = b->v564.x + b->v4C0.x;
        d.y = b->v564.y + b->v4C0.y;
        d.z = b->v564.z + b->v4C0.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = func_80013408(&d, &b->v88);
        if (b->f41B == 2) {
            f32 q;
            q = t / (2.0f * D_800E76C8);
            b->v94.x -= d.x * q;
            b->v94.y -= d.y * q;
            b->v94.z -= d.z * q;
        } else {
            s32 n;
            n = t;
            n = ABS(n);
            if ((f32)n < 1.5f) {
                f32 s;
                s = D_800E76C8 * 240.0f;
                b->v94.x -= d.x * ((f32)b->f5D0 / s);
                b->v94.y -= d.y * ((f32)b->f5D0 / s);
                b->v94.z -= d.z * ((f32)b->f5D0 / s);
            }
        }
    }
    if (b->f5C0 >= -1 && b->f3D0 == -1) {
        f32 len;
        f32 t;
        s32 m;
        d.x = b->v564.x + b->v4C0.x;
        d.y = b->v564.y + b->v4C0.y;
        d.z = b->v564.z + b->v4C0.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = func_80013408(&d, &b->v88);
        m = b->f5D0;
        if ((t > 0.5f && m >= -3) || (t < -0.5f && m < 4)) {
            f32 q;
            q = t / (2.0f * D_800E76C8);
            b->v94.x = -d.x * q;
            b->v94.y = -d.y * q;
            b->v94.z = -d.z * q;
        }
    }
    if (b->f1C6 != 0) {
        f32 len;
        f32 t;
        e.x = b->v564.x + b->v4C0.x;
        e.y = b->v564.y + b->v4C0.y;
        e.z = b->v564.z + b->v4C0.z;
        len = sqrtf(e.x * e.x + e.y * e.y + e.z * e.z);
        if (len != 0.0f) {
            e.x /= len;
            e.y /= len;
            e.z /= len;
        }
        t = func_80013408(&e, &b->v88) / (D_800E76C8 * 16.0f);
        b->v94.x -= e.x * t;
        b->v94.y -= e.y * t;
        b->v94.z -= e.z * t;
    }
}

void func_8001FB40(u8 *param_1)
{
    f32 len;
    f32 r;

    *(f32 *)(param_1 + 0x54) = *(f32 *)(param_1 + 0x54) + *(f32 *)(param_1 + 0x60) * D_800E76C8;
    *(f32 *)(param_1 + 0x58) = *(f32 *)(param_1 + 0x58) + *(f32 *)(param_1 + 0x64) * D_800E76C8;
    *(f32 *)(param_1 + 0x5C) = *(f32 *)(param_1 + 0x5C) + *(f32 *)(param_1 + 0x68) * D_800E76C8;
    *(f32 *)(param_1 + 0x88) = *(f32 *)(param_1 + 0x88) + *(f32 *)(param_1 + 0x94) * D_800E76C8;
    *(f32 *)(param_1 + 0x8C) = *(f32 *)(param_1 + 0x8C) + *(f32 *)(param_1 + 0x98) * D_800E76C8;
    *(f32 *)(param_1 + 0x90) = *(f32 *)(param_1 + 0x90) + *(f32 *)(param_1 + 0x9C) * D_800E76C8;
    func_80013310((f32 *)(param_1 + 0x88), (f32 *)(param_1 + 0xA0), (f32 *)(param_1 + 0x430));
    len = *(f32 *)(param_1 + 0x54) * *(f32 *)(param_1 + 0x54)
        + *(f32 *)(param_1 + 0x58) * *(f32 *)(param_1 + 0x58)
        + *(f32 *)(param_1 + 0x5C) * *(f32 *)(param_1 + 0x5C);
    *(f32 *)(param_1 + 0x420) = __builtin_sqrtf(len);
    r = func_80013408((f32 *)(param_1 + 0x54), (f32 *)(param_1 + 0xB8));
    *(f32 *)(param_1 + 0x424) = r;
    if (*(u8 *)(param_1 + 0x44) == 0 && *(s8 *)(param_1 + 0x3D0) != 0xF) {
        func_80029B1C(param_1);
    }
}

typedef struct {
    f32 loc[3];
    f32 pad0;
    f32 zv[3];
    f32 pad1[5];
    f32 one[2];
} Locals;

void func_8001FC60(Bike_11 *b)
{
    Locals L;
    f32 c;
    f32 k;
    s32 n;
    s32 id;
    s32 i;

    c = D_800E76C8 * 0.5f;
    L.zv[0] = L.zv[1] = L.zv[2] = 0.0f;
    k = -b->f438 * c;
    L.loc[0] = b->dir[0] * k;
    L.loc[1] = b->dir[1] * k;
    L.loc[2] = b->dir[2] * k;
    b->pos[0] = L.loc[0] + b->pos[0];
    b->pos[1] = L.loc[1] + b->pos[1];
    b->pos[2] = L.loc[2] + b->pos[2];
    n = 8;
    if (b->f4FC <= 0 && b->f458 <= 0) {
        n = 6;
    }
    func_800453D8(b->f48, D_800C94C8, n, 0);
    b->flags |= 1;
    id = b - D_800E37C0_17;
    b->pos[0] -= L.loc[0];
    b->pos[1] -= L.loc[1];
    {
        f32 t2 = b->pos[2];
        L.one[0] = 1.0f;
        b->pos[2] = t2 - L.loc[2];
    }
    if (id == (D_800E76D0 & 7)) {
        u8 *tb = D_800C9548;
        u8 *t = (u8 *)(((D_800E76D0 >> 2) & 6) + (s32)tb);
        if (func_80030658(b->pts[t[0]], b->pts[t[1]], D_800EE300, L.one, 0)) {
            func_800445B0(b, 0x1C);
        }
    }
}

#define FL(o) (*(f32 *)((u8 *)b + (o)))
#define WD(o) (*(s32 *)((u8 *)b + (o)))
#define PF(o) ((f32 *)((u8 *)b + (o)))

void func_8001FE1C(Bike_42 *b)
{
    f32 tmp[3];
    f32 q[4];
    f32 o[3];
    f32 r1[4];
    f32 r2[4];
    f32 x, y, m, d, len, ang, s, s2, c, k;
    s32 v, r;

    x = FL(0x51C);
    y = FL(0x478);
    if (*((u8 *)b + 0x44) == 0) {
        func_80013310(PF(0x564), PF(0xA0), tmp);
        x += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
        func_80013310(PF(0x4C0), PF(0xA0), tmp);
        y += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
    }
    m = 0.0f;
    WD(0x38) = x / 0.001f;
    WD(0x3C) = y / 0.001f;
    if (x > 0.2f) {
        WD(0x38) = 200;
        m = 0.2f - x;
    }
    if (y > 0.2f) {
        d = 0.2f - y;
        WD(0x3C) = 200;
        if (d < m) {
            m = d;
        }
    }
    m += 0.5f;
    if (y < -0.1f) {
        WD(0x3C) = -100;
    }
    WD(0x38) = (f32)WD(0x38) + 50.0f;
    WD(0x0) = (FL(0x48) - FL(0xAC) * m) * 1000.0f;
    WD(0x4) = (FL(0x4C) - FL(0xB0) * m) * 1000.0f;
    WD(0x8) = (FL(0x50) - FL(0xB4) * m) * 1000.0f;
    WD(0x30) = FL(0x5A0) * 32768.0f / 3.1415927f;
    *(Blk *)PF(0x370) = *(Blk *)PF(0x78);
    if (*((u8 *)b + 0x44) == 0) {
        func_80012250(PF(0x78), &WD(0x24), &WD(0x28), &WD(0x2C));
        if (*((s8 *)b + 0x3D0) == -1 || *((s8 *)b + 0x3D0) == 15) {
            v = WD(0x24);
            if (v > 0x3000) {
                func_800445B0(b, 0x19);
            } else if (v < -0x3000) {
                func_800445B0(b, 0x1C);
            }
        }
        func_8003E944(b);
        if (FL(0x3A8) != 0.0f) {
            q[0] = (1.0f - FL(0x3A8)) + FL(0x38C) * FL(0x3A8);
            q[1] = FL(0x390) * FL(0x3A8);
            q[2] = FL(0x394) * FL(0x3A8);
            q[3] = FL(0x398) * FL(0x3A8);
            len = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
            q[0] /= len;
            q[1] /= len;
            q[2] /= len;
            q[3] /= len;
            func_80013C20(PF(0x370), PF(0x78), q);
            func_8001327C(PF(0x39C), PF(0xA0), o);
            WD(0x0) = (f32)WD(0x0) + o[0] * (FL(0x3A8) * 1000.0f);
            WD(0x4) = (f32)WD(0x4) + o[1] * (FL(0x3A8) * 1000.0f);
            WD(0x8) = (f32)WD(0x8) + o[2] * (FL(0x3A8) * 1000.0f);
        }
        func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        if (*((u8 *)b + 0x6E8) != 0) {
            v = WD(0x1C);
            if (v > 0x1800) {
                r = 0;
            } else if (v > 0xC00) {
                r = 0x1800 - v;
            } else if (v >= -0xBFF) {
                r = WD(0x28);
            } else if (v >= -0x17FF) {
                r = -0x1800 - v;
            } else {
                r = 0;
            }
            ang = r;
            ang *= 3.1415927f;
            ang *= 3.0517578e-05f;
            ang *= 0.5f;
            c = cosf(ang);
            s = sinf(ang);
            r1[0] = c;
            r1[1] = s * FL(0xB8);
            r1[2] = s * FL(0xBC);
            r1[3] = s * FL(0xC0);
            func_80013C20(PF(0x370), r1, PF(0x370));
            func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        } else if ((WD(0x5B0) & 0x20000) && D_800E76D0 - WD(0x5B8) < 0x3C &&
                   ((*(Info **)((u8 *)b + 0x4E4))->f28 & 4) && WD(0x744) < 0 && FL(0x420) < 5.0f) {
            if (FL(0x420) > 2.0f) {
                k = (f32)(WD(0x5B8) - (D_800E76D0 - 0x3C)) * 6.1035155e-07f / 60.0f;
            } else {
                k = 1.2207031e-06f;
            }
            ang = D_800C2C60[((((D_800E76D0 - WD(0x5B8)) << 13) >> 4) - 0x400) & 0xFFF];
            ang *= k;
            r2[0] = cosf(ang);
            s2 = sinf(ang);
            r2[1] = s2 * FL(0xAC);
            r2[2] = s2 * FL(0xB0);
            r2[3] = s2 * FL(0xB4);
            func_80013C20(PF(0x370), r2, PF(0x370));
            func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        }
    } else {
        func_80013568(PF(0x224), PF(0x1FC));
        WD(0xC) = FL(0x1CC) * 1000.0f;
        WD(0x10) = FL(0x1D0) * 1000.0f;
        WD(0x14) = FL(0x1D4) * 1000.0f;
    }
    WD(0x30) = FL(0x5A0) * 32768.0f / 3.1415927f;
    func_80013568(PF(0xA0), PF(0x78));
}

void func_80020504(void *a0)
{
    u8 *s1 = (u8 *)a0;
    u8 *s0 = s1 + 0x54;
    f32 fs0, d;
    s32 iv, v1;

    fs0 = func_80013408((f32 *)s0, (f32 *)(s1 + 0x4C0)) + func_80013408((f32 *)s0, (f32 *)(s1 + 0x564));
    if (s1[0x3D0] < 0x10 && *(s32 *)(s1 + 0x744) >= 0xB)
        func_800445B0(s1, 0x1C);
    *(s32 *)(s1 + 0x5C0) = 0;
    if (fs0 < -10.0f)
        *(s16 *)(s1 + 0x416) = 0;
    if (D_800EFDBC == 0xF) {
        *(f32 *)(s1 + 0x7B8) = D_800C8E08;
        *(s32 *)(s1 + 0x8C) = 0;
    }
    if (*(s32 *)(s1 + 0x744) < 0x29)
        goto L1c0;
    d = *(f32 *)(s1 + 0x520) - *(f32 *)(s1 + 0x47C);
    *(f32 *)(s1 + 0x75C) = *(f32 *)(s1 + 0x758);
    if (d < -2.0f) {
        func_800445B0(s1, 0x19);
        goto L178;
    }
    if (d < -0.8f) {
        *(s8 *)(s1 + 0x45) = 0x19;
        goto L160;
    }
    if (d < -0.3f) {
        *(s8 *)(s1 + 0x5CD) = 1;
        goto L178;
    }
    if (d < 0.3f) {
        *(s8 *)(s1 + 0x5CC) = 1;
        goto L178;
    }
    if (d < 0.6f) {
        *(s8 *)(s1 + 0x5CD) = 1;
        goto L178;
    }
    *(s8 *)(s1 + 0x45) = 0x1C;
L160:
    *(T3 *)(s1 + 0x43C) = *(T3 *)(s1 + 0x54);
L178:
    if (*(s8 *)(s1 + 0x45) != 0 && -10.0f < fs0) {
        *(s8 *)(s1 + 0x45) = 0;
        *(s8 *)(s1 + 0x5CD) = 1;
    }
    if (*(u8 *)(s1 + 0x5CC) != 0) {
        f32 zf = 0.0f;
        *(f32 *)(s1 + 0x90) = zf;
        *(f32 *)(s1 + 0x8C) = zf;
        *(f32 *)(s1 + 0x88) = zf;
    }
L1c0:
    if (*(s32 *)(s1 + 0x744) < 0x15)
        goto END;
    iv = (s32)(-fs0 * 10.0f);
    *(s32 *)(s1 + 0x768) = iv;
    if (iv < 0)
        *(s32 *)(s1 + 0x768) = 0;
    if (*(s32 *)(s1 + 0x768) >= 0x100)
        *(s32 *)(s1 + 0x768) = 0xFF;
    v1 = D_800E76D0;
    *(u8 *)(s1 + 0x764) = 0xFF;
    *(s32 *)(s1 + 0x5A0) = 0;
    *(s32 *)(s1 + 0x774) = v1;
END:
    *(s32 *)(s1 + 0x744) = 0;
}

struct Body {
    u8 pad0[0x48];
    f32 x;
    f32 y;
    f32 z;
    f32 vx;
    f32 vy;
    f32 vz;
    u8 pad1[0x420 - 0x60];
    f32 f420;
};

f32 func_80020744(Body *p, f32 g, f32 t, f32 *out)
{
    f32 diff, prev, v, step, lim, w;

    out[0] = p->x + p->vx * t;
    out[1] = p->y + (p->vy + g * t * 0.5f) * t;
    w = p->z + p->vz * t;
    out[2] = w;
    if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else w = 3.0f;
        diff = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
    v = p->vy + g * t;
    if (0.0f < v ? v < 0.1f : -v < 0.1f) v = 0.1f;
    v = 1.0f / v;
    if (10.0f < v) v = 10.0f;
    else if (v < -10.0f) v = -10.0f;
    lim = 0.1f;
    while (lim < diff || diff < -lim) {
        step = diff * v;
        if (t + step < 0.0f) step = -t;
        t += step;
        prev = diff;
        out[1] = p->y + (p->vy + g * t * 0.5f) * t;
        out[0] = p->x + p->vx * t;
        w = p->z + p->vz * t;
        out[2] = w;
        if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else if (out == (f32 *)p) w = 4.0f; else if (out == &p->x) w = 5.0f; else if (out == &p->y) w = 6.0f; else w = 3.0f;
                diff = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
        if (prev == diff) return t;
        v = step / (prev - diff);
        if (10.0f < v) v = 10.0f;
        else if (v < -10.0f) v = -10.0f;
        lim += 0.05f;
    }
    if (p->f420 != 0.0f) {
        out[0] = out[0] - p->vx * (0.5f / p->f420);
        out[2] = out[2] - p->vz * (0.5f / p->f420);
    }
    if (out == 0) diff = 1.0f; else if (p == 0) diff = 2.0f; else diff = 3.0f;
        out[1] = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    if (t < 0.0f) t = 0.0f;
    return t;
}

struct Bike_12 {
    u8 pad0[0x48];
    Vec3f pos;      /* 0x48 */
    Vec3f vel;      /* 0x54 */
    u8 pad60[0x88 - 0x60];
    Vec3f rot;      /* 0x88 */
    u8 pad94[0xA0 - 0x94];
    Vec3f fA0;      /* 0xA0 */
    Vec3f fAC;      /* 0xAC */
    u8 padB8[0x6F4 - 0xB8];
    f32 dt;         /* 0x6F4 */
};

void func_80020B18(Bike_12 *b)
{
    Vec3f n;
    Vec3f c;
    Vec3f u;
    Vec3f v;
    f32 len;
    f32 q;
    s32 t, w;
    f32 th, qd;

    v = b->pos;
    th = v.x + b->vel.x * b->dt;
    v.x = th;
    qd = v.z + b->vel.z * b->dt;
    v.z = qd;
    if (b == 0) q = 1.0f; else if (t == w) q = 2.0f; else q = 3.0f;
    func_8002E920((s32)(v.x * 1000.0f), (s32)(v.y * 1000.0f), (s32)(v.z * 1000.0f));
    func_8002F4B0(&n.x);
    if (n.y < 0.5f) {
        return;
    }
    c.x = b->fAC.y * n.z - n.y * b->fAC.z;
    c.y = b->fAC.z * n.x - n.z * b->fAC.x;
    c.z = b->fAC.x * n.y - n.x * b->fAC.y;
    len = func_80013408(&c.x, &b->fA0.x);
    c.x = b->fA0.x * len;
    c.y = b->fA0.y * len;
    c.z = b->fA0.z * len;
    if (len == 0.0f) {
        return;
    }
    t = func_80013010((s32)(len * 16384.0f));
    q = (f32)(s16)t * 3.1415927f * 3.0517578e-05f;
    th = b->dt;
    th += 0.1f;
    qd = (q + q) / th;
    if (qd > 2.0f) {
        qd = 2.0f;
    } else if (qd < -2.0f) {
        qd = -2.0f;
    }
    qd = qd / len;
    b->rot.x = c.x * qd;
    b->rot.y = c.y * qd;
    b->rot.z = c.z * qd;
}

typedef struct { u8 pad[0x28]; s32 flags; } Sub_4;

struct Ent_12 {
    u8 pad0[0x88];
    V3 pos;          /* 0x88 */
    u8 pad94[0xA0 - 0x94];
    V3 vel;          /* 0xA0 */
    V3 vAC;          /* 0xAC */
    u8 padB8[0x414 - 0xB8];
    s16 h414;
    s16 h416;
    u8 b418;
    u8 b419;
    u8 b41A;
    u8 b41B;
    u8 pad41C[4];
    f32 f420;
    f32 f424;
    u8 pad428[0x458 - 0x428];
    s32 w458;
    u8 pad45C[0x4C0 - 0x45C];
    V3 v4C0;
    u8 pad4CC[0x4E4 - 0x4CC];
    Sub_4 *s4E4;
    u8 pad4E8[0x4FC - 0x4E8];
    s32 w4FC;
    u8 pad500[0x588 - 0x500];
    Sub_4 *s588;
    u8 pad58C[0x5B0 - 0x58C];
    u32 btn;
    u32 prev;
    u8 pad5B8[0x5D0 - 0x5B8];
    s8 c5D0;
    s8 c5D1;
    s8 c5D2;
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 b6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 w744;
    u8 pad748[0x7A8 - 0x748];
    u8 b7A8;
};

void func_80020D38(Ent_12 *e)
{
    s8 c;

    if (e->w744 > 0x3C && (*(s32 *)&e->b418 & 0xFFFF00) == 0) {
        e->h416 = 0;
    }
    if (e->h416 == 0) {
        if (e->w744 < 0) {
            if (e->w4FC < 0 && e->f420 > 7.5f && (e->s588->flags & 0x20)) {
                func_800445B0(e, 0x1C);
            }
            if ((e->btn & 0x20000) && !(e->prev & 0x20000) && abs(e->c5D0) < 3 &&
                e->f424 > -1.0f && e->c5D2 < -4) {
                e->h416 = 0x10;
                e->h414 = 0;
            }
        }
    } else {
        if (e->w458 <= 0) {
            if (e->s4E4->flags & 0x20) {
                e->h416 = 0xF;
            }
            if (e->w458 <= 0 && func_80013408(&e->vAC.x, &e->v4C0.x) < 0.5f) {
                func_800445B0(e, 0x19);
            }
        }
        if (e->h416 < 0x10) {
            e->h416--;
        } else {
            if (e->h416 >= 0x3D && e->w744 <= 0) {
                c = e->c5D2;
                if (c < -12) {
                    e->h414 += 0x20;
                } else if (c >= -3) {
                    e->h414 -= 0x20;
                }
                e->h414 -= e->h414 / 64;
                if (e->h414 > 0x400) {
                    func_800445B0(e, 0x19);
                    e->pos.x -= e->vel.x * 5.0f;
                    e->pos.y -= e->vel.y * 5.0f;
                    e->pos.z -= e->vel.z * 5.0f;
                } else if (e->h414 < -0x400) {
                    e->h416 = 0xF;
                }
                if ((e->c5D2 < -12 || e->c5D2 > -4) && (f32)e->w744 < 3.0f) {
                    if (e->b41A < 0x10) e->b41A++;
                    if (e->b419 < 0x10) e->b419++;
                } else {
                    if (e->b41A != 0) e->b41A--;
                    if (e->b419 != 0) e->b419--;
                }
            } else {
                e->b419 = 0;
                e->b41A = 0;
            }
            e->h416++;
            if (e->h416 == 0xF0) {
                e->b7A8 = 1;
            }
            if (e->c5D2 >= 0) {
                e->h416 = 0xF;
            }
        }
    }
    if (e->b6E8 && e->h416 != 0) {
        e->h416 = 0;
    }
}

typedef struct {
    u8 pad0[0x28];
    s32 f28;
    u8 pad2C[0x14];
} Surf_3;

typedef struct {
    u8 pad0[0xC];
    Trk_2 *fC;
    u8 pad10[0x2C];
} Ent_13;

typedef struct {
    u8 pad0[0x64];
    u8 f64;
    u8 pad65[0x7];
} Seg_4;

extern Surf_3 D_800C9080_4[];
extern Ent_13 D_800C9A40[];
extern Seg_4 *D_800EE498_4;

void func_80021068(Bike_13 *b) {
    volatile s32 pad[12];
    Vec3f t;
    Vec3f c;
    Vec3f p;
    f32 x, z, len;
    f32 fs2, fs3, fs4;
    f32 sp, th, e, d;
    f32 s1v, t1v, d1, g, m, c2, d2;
    s32 n, r;
    f32 k;

    if (b->f5CD != 0) {
        b->f5CD += D_800E76C7;
        if (b->f5CD >= 60) {
            b->f5CD = 0;
        }
    }
    if (b->f748 == 1 && (b->f44 != 0 || b->f3D0 != -1)) {
        b->f748 = -1;
    }
    if (b->f458 > 0 && b->f4FC > 0) {
        if (b->f744 <= 0) {
            b->f740 = 0;
            b->f6F8 = (b->vel.y + b->vel.y) / 10.106251f;
            b->f718 = (b->vel.y + b->vel.y) / 19.29375f;
            b->f6F4 = (b->vel.y + b->vel.y) / 14.700001f;
            func_80070560(b);
            s1v = b->f420;
            t1v = 0.5f;
            if (!(s1v > 20.0f)) {
                t1v = 0.25f;
                if (s1v > 10.0f) {
                    t1v = (s1v - 10.0f) / 40.0f + 0.25f;
                }
            }
            if (t1v < b->f6F4) {
                if (b->f416 != 0 && b->f416 < 45) {
                    b->f416 = 0;
                }
                d1 = func_80013408(&b->fA0.x, &b->f88.x);
                b->f88.x -= d1 * b->fA0.x;
                b->f88.y -= d1 * b->fA0.y;
                b->f88.z -= d1 * b->fA0.z;
                if (b->f6E8 == 0 && b->f44 == 0 && D_800E76D0 - b->f5B8 < 10 && (b->f5B0 & 0x20000)) {
                    sp = b->f420;
                    if (sp < 20.0f && b->f654 == 0) {
                        th = 5.0f;
                        if (sp > 10.0f) {
                            th = 5.0f - (sp - 10.0f) * 0.5f;
                        }
                        if (sp != 0.0f) {
                            sp = th / sp + 1.0f;
                        } else {
                            sp = 0.0f;
                        }
                        b->vel.x *= sp;
                        b->vel.y *= sp;
                        b->vel.z *= sp;
                        if (b->f24 >= -0x17FF) {
                            b->f88.x -= b->fA0.x * 3.0f;
                            b->f88.y -= b->fA0.y * 3.0f;
                            b->f88.z -= b->fA0.z * 3.0f;
                        }
                        func_80011378(7, b - D_800E37C0_18, 0xC0, 0xC0, 1);
                    }
                }
                if (D_800EFDBC == 6) {
                    Vec3f *q = &b->f74C;
                    if (b->f420 > 4.0f) {
                    th = q->x; sp = q->z; if (b == 0) r = 0; func_8002E920(th * 1000.0f, q->y * 1000.0f, sp * 1000.0f);
                    if (D_800C9080_4[(D_800E84F8 >> 4) & 0xF].f28 & 0x400) {
                        f32 k;
                        k = 10.0f;
                        g = D_800C8E04 * k / b->f420 + 1.0f;
                        b->vel.x *= g;
                        b->vel.y *= g;
                        b->vel.z *= g;
                    }
                    }
                }
            }
            t.x = b->vel.x * b->f6F8;
            t.y = b->vel.y * b->f6F8;
            t.z = b->vel.z * b->f6F8;
            b->f6FC.x = t.x + b->pos.x;
            b->f6FC.y = t.y + b->pos.y;
            b->f6FC.z = t.z + b->pos.z;
            t.x = b->vel.x * b->f718;
            t.y = b->vel.y * b->f718;
            t.z = b->vel.z * b->f718;
            b->f71C.x = t.x + b->pos.x;
            b->f71C.y = t.y + b->pos.y;
            b->f71C.z = t.z + b->pos.z;
            b->f5CC = 0;
            b->f622 = 0;
            {
                u16 h = b->f628;
                b->f734 = h;
                b->f714 = h;
            }
            b->f744 = D_800E76C4;
        } else {
            b->f744 += D_800E76C4;
        }
        if (b->f744 >= 4) {
            b->f718 = func_80020744(b, -19.29375f, b->f718, &b->f71C);
            func_8002F4B0(&b->f728.x);
            b->f6F8 = func_80020744(b, -10.106251f, b->f6F8, &b->f6FC);
            func_8002F4B0(&b->f708.x);
            b->f6F4 = (b->f6F8 + b->f718) * 0.5f;
            if (b->f5B0 & 0x10000) {
                n = b->f5D0;
                if (b->f3D0 == -1 && b->f6F4 > 0.5f) {
                    if (n > 10) {
                        func_8006FE84(b, 16);
                    } else if (n < -10) {
                        func_8006FE84(b, 17);
                    }
                    if (b->f3D0 != -1) {
                        b->f3B0 = (f32)(D_800C9A40[b->f3D0].fC->f4 << 9) / (b->f6F4 * 60.0f);
                    }
                }
                if (b->f3D0 == 16) {
                    if (n <= 0) {
                        n = 1;
                    }
                    b->f622 = n << 9;
                } else if (b->f3D0 == 17) {
                    if (n >= 0) {
                        n = -1;
                    }
                    b->f622 = n << 9;
                }
            }
            if (b->f6E8 == 0) {
                if (b->f3D0 == -1 && !(b->f5B0 & 0x100)) {
                    m = 0.015625f;
                    if (b->f5D2 < 0) {
                        m = b->f7C8;
                    }
                    b->f738 = b->f5D2 * m;
                    if (b == 0) r = 0;
                    b->f73C = b->f5D2;
                } else {
                    b->f73C = 0;
                    b->f738 = b->f7C8 * -15.0f;
                }
            } else {
                n = 5;
                if (b->f744 >= 6) {
                    n = 1;
                }
                x = b->vel.x;
                z = b->vel.z;
                len = __builtin_sqrtf(x * x + z * z);
                if (len == 0.0f) {
                    len = 1.0f;
                }
                x /= len;
                z /= len;
                fs3 = b->f71C.y * 14.700001f;
                fs4 = (x * b->f728.x + z * b->f728.z) / b->f728.y;
                if (D_800EFDBC != 5) {
                    b->f734 = func_8004B5F0(&b->f71C, b->f734, n, b->f6EB);
                    if (D_800EE498_4[b->f734].f64 & 0x10) {
                        fs3 -= 1000.0f;
                    }
                }
                fs2 = b->f6FC.y * 14.700001f;
                e = b->f728.x * b->vel.x + b->f728.y * (b->vel.y - b->f718 * 17.640001f) + b->f728.z * b->vel.z;
                x = (x * b->f708.x + z * b->f708.z) / b->f708.y;
                fs3 -= e * e;
                if (D_800EFDBC != 5) {
                    b->f714 = func_8004B5F0(&b->f6FC, b->f714, n, b->f6EB);
                    if (D_800EE498_4[b->f714].f64 & 0x10) {
                        fs2 -= 500.0f;
                    }
                }
                e = b->f708.x * b->vel.x + b->f708.y * (b->vel.y - b->f6F8 * 11.760001f) + b->f708.z * b->vel.z;
                b->f6F4 = (b->f6F8 + b->f718) * 0.5f;
                fs2 -= e * e;
                if (fs3 < fs2) {
                    b->f6EC = 1;
                    b->f738 = -0.3125f;
                } else {
                    fs4 = x;
                    b->f6EC = 0;
                    b->f738 = 0.3125f;
                }
                if (b->f6F4 < 1.5f) {
                    r = fs4 * 15.0f + (f32)((s32)(fs4 * 8192.0f - (f32)b->f18) / 512);
                    if (r > 15) {
                        r = 15;
                    } else if (r < -15) {
                        r = -15;
                    }
                    b->f740 = r;
                    b->f73C = b->f740;
                }
            }
        }
        if (b->f5CC != 0) {
            func_80020B18(b);
        } else if (b->f6F4 < 0.25f && b->vel.y < 0.0f && b->f744 >= 31 && b->f416 == 0) {
            if (b->f6E8 == 0) {
                p = b->pos;
                th = p.x + b->vel.x * b->f6F4;
                p.x = th;
                m = p.z + b->vel.z * b->f6F4;
                p.z = m;
                if (b == 0) e = 1.0f; else if (n == 0) e = 2.0f; else e = 3.0f;
                p.y = (f32)func_8002E920(p.x * 1000.0f, p.y * 1000.0f, p.z * 1000.0f) / 1000.0f;
                func_8002F4B0(&t.x);
                c.x = b->fAC.y * t.z - t.y * b->fAC.z;
                c.y = b->fAC.z * t.x - t.z * b->fAC.x;
                c.z = b->fAC.x * t.y - t.x * b->fAC.y;
                d2 = func_80013408(&c.x, &b->fA0.x);
                c2 = b->f7B8 * 10.0f;
                if (d2 > 0.0f ? d2 < c2 : -d2 < c2) {
                    b->f5CC = 1;
                }
            } else {
                b->f5CC = 1;
            }
        }
        if (b->f748 != -1) {
            b->f748 = 1;
            b->f758 = __builtin_sqrtf((b->pos.x - b->f74C.x) * (b->pos.x - b->f74C.x)
                                      + (b->pos.y - b->f74C.y) * (b->pos.y - b->f74C.y)
                                      + (b->pos.z - b->f74C.z) * (b->pos.z - b->f74C.z));
        }
    } else {
        if (b->f744 > 0) {
            func_80020504(b);
        } else {
            b->f744 -= D_800E76C4;
        }
        if (b->f748 == 1) {
            b->f748 = 0;
        }
        b->f74C = b->f494;
    }
    b->f6F0 = b->vel.y;
}

#define IDX(p) ((((s32)(p) - (s32)&D_800E37C0) * -0x41041041) >> 5)

void func_80021CF8(u8 *a2)
{
    s32 idx;
    s32 v;
    u8 b;
    s8 c;
    s32 *base;
    s32 n;
    s32 *q;

    base = D_800C238C;
    idx = IDX(a2);
    q = &base[idx];
    if (D_800C8CD8 != 0 && D_800E3700 == idx) v = 0x80;
    else if (D_800C8CDC != 0) {
        if (idx == 0) v = 0x80; else { *q = 0xFF; goto L74; }
    } else v = 0xFF;
    *q = v;
L74:
    if (a2[0x44] != 0 && a2[0x6E8] != 0 && *(s16 *)(a2 + 0x624) > 0x58) {
        D_800C238C[IDX(a2)] = (0x78 - *(s16 *)(a2 + 0x624)) * 8;
        return;
    }
    if (*(s8 *)(a2 + 0x620) != 0) {
        b = a2[0x621] - 1;
        a2[0x621] = b;
        if ((s32)((u32)b << 24) <= 0) {
            c = a2[0x620] - 1;
            a2[0x620] = c;
            if (c == 0 && D_800C8CD8 == 0) {
                if (a2[0x6E8] == 0) {
                    if ((D_800C2280 & 0x100) == 0) a2[0x41D] = 0;
                } else {
                    a2[0x41D] = 0;
                }
            }
            n = (((s32)((u32)a2[0x620] << 24)) >> 25) - 10;
            a2[0x621] = n;
            if (n < 4) a2[0x621] = 4;
        }
        if (a2[0x620] & 1) D_800C238C[IDX(a2)] = 0;
    }
}

typedef struct {
    u8 pad[0x58];
    s32 flags;
} Game_11;

extern Game_11 D_800F05A8_11;

void func_80021EC8(Bike_14 *b, s32 n)
{
    f32 t;

    D_800E80D4 = 0;
    D_800E84F8 = 0;
    D_800E8D64 = 0;
    D_800E8958 = 0;
    D_800E76C4 = n;
    D_800E76C8 = (f32)D_800E76C4 * b->f44C / 60.0f;
    if (D_800EE30C != 0 && b->f6E8 != 0) {
        D_800E76C8 *= 0.5f;
    }
    func_80021CF8(b);
    func_80036DF4(b);
    if (b->f44 == 0) {
        if (b->f45 > 0) {
            b->f5B0 = 0;
            if (b->f4FC < 10 && b->f458 < 10) {
                func_800445B0(b, b->f45);
            }
        }
        if (b->f45 < 0) {
            func_800445B0(b, -b->f45);
        }
        func_8001D130(b, n);
        func_80020D38(b);
        func_80021068(b);
        if (b->f6E8 != 0) {
            func_8005B5F0(b);
            b->f5CF = 0;
            if (b->f420 > 5.0f) {
                if (b->f41B != 0 || ((b->f5B0 & 0x20000) && D_800E76D0 - b->f5B8 < 30)) {
                    b->f5CF = 2;
                } else if (b->f41A | b->f419) {
                    b->f5CF = 1;
                }
            }
            func_8005B928(b);
        } else {
            func_8005C5BC(b, n);
        }
        if (D_800F05A8_11.flags & 0x10) {
            b->f412 = 0x3000;
            b->f5B0 |= 0x20;
        }
        if (b->f690 & 0x10) {
            if (b->f424 > 0.0f) {
                b->f5B0 = 0x20;
            } else {
                b->f5B0 = 0;
                b->f5D0 = 0;
                if (b - D_800E37C0_19 == 0) {
                    D_800F05A8_11.flags |= 0x2000;
                }
            }
        }
        func_80035BAC(b - D_800E37C0_19, n);
        func_8001D008(b);
        func_8001CDE0(b);
        func_8001DD10(b);
        func_8006FB3C(b);
        func_8001D640(b);
        func_8001EC90(b);
        if (b->f44 == 0) {
            func_8001FC60(b);
            func_8001FB40(b);
            func_8001CB80(b->f4F8, b->f388);
            func_8001CB80(b->f454, b->f388);
            func_8001BC98(b);
            switch (D_800EFDBC) {
            case 2:
            case 4:
            case 5:
            case 10:
            case 14:
                break;
            case 6:
                func_8006BF04(b);
                break;
            default:
                func_8004AC98(b);
                break;
            }
            switch (b->f3FF) {
            case 0:
                break;
            case 1:
                if (b->f3D0 == -1 && b->f420 < 8.0f && b->f744 < -30) {
                    b->f3FF = 2;
                    func_8006FE84(b, b->f668 + 0x13);
                    if (!(D_800EFD70 & 0x8000)) {
                        b->f5AA = 10;
                    }
                }
                break;
            case 2:
                if (b->f3D0 < 0) {
                    b->f3FF = 3;
                    if (D_800EFDBC == 5) {
                        b->f3FF = 0;
                        b->f6E8 = 1;
                        b->f5AA = b->f5A8;
                    }
                }
                break;
            }
        } else {
            func_80044B60(b);
        }
    } else {
        b->f5B0 = 0;
        func_80035BAC(b - D_800E37C0_19, n);
        func_80044B60(b);
    }
    func_8001FE1C(b);
    func_80036544(b);
}

u32 func_80022324(u32 a0, s32 a1)
{
    s8 *base;
    s32 c;
    u32 u2;

    if (a0 & 0x8000) a0 |= 0x20000;
    base = (s8 *)D_800EFD70_26;
    if (a1 < *(s32 *)(base + 0x3C))
        c = (s8)base[(s8)base[a1 + 0x2C8] + 0x50];
    else
        c = 0;

    switch (c) {
    case 0:
        return a0;
    case 1:
        u2 = a0 & 0xfffcffff;
        if (a0 & 0x10000) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x10000;
        return u2;
    case 2:
        u2 = a0 & 0xfffdffdf;
        if (a0 & 0x20) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x20;
        return u2;
    case 3:
        u2 = a0 & 0xfffcffdf;
        if (a0 & 0x20) u2 |= 0x10000;
        if (a0 & 0x20000) u2 |= 0x20;
        if (a0 & 0x10000) u2 |= 0x20000;
        return u2;
    }
    return a0;
}

typedef struct {
    s32 pos[3];          /* 0x000 */
    u8 pad0C[0x48 - 0x0C];
    Vec3f prev;          /* 0x048 */
    Vec3f vel;           /* 0x054 */
    Vec3f acc;           /* 0x060 */
    Vec3f fpos;          /* 0x06C */
    u8 pad78[0x44C - 0x78];
    f32 time;            /* 0x44C */
    u8 pad450[0x5B0 - 0x450];
    s32 in;              /* 0x5B0 */
    s32 inPrev;          /* 0x5B4 */
    u8 pad5B8[0x5D0 - 0x5B8];
    u8 b5D0;             /* 0x5D0 */
    u8 pad5D1;
    u8 b5D2;             /* 0x5D2 */
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 b6E8;             /* 0x6E8 */
    u8 pad6E9[0x6F4 - 0x6E9];
    f32 f6F4;            /* 0x6F4 */
    u8 pad6F8[0x744 - 0x6F8];
    s32 s744;            /* 0x744 */
    u8 pad748[0x7A4 - 0x748];
    u8 b7A4;             /* 0x7A4 */
    u8 b7A5;             /* 0x7A5 */
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_15;

extern Bike_15 D_800E37C0_20[];

void func_80022448(s32 n) {
    Bike_15 *b;
    s32 i;
    s32 v;
    s32 u;
    s32 pad[2];
    f32 t, t2, tk, t2k;
    f32 x, y, z;

    if (D_800F0600 & 0x800) {
        return;
    }
    for (i = 0; i < 8; i++) {
        b = &D_800E37C0_20[i];
        if (b->b6E8 == 0) {
            b->inPrev = b->in;
            v = D_800E3770[i];
            b->in = v;
            if (D_800C2364 != 0 && (D_800C8CD8 == 0 || i != D_800E3700)) {
                b->in = v ^ 0xC;
            }
        } else {
            u = b->in;
            b->b5D2 = 0;
            b->b5D0 = 0;
            b->in = 0;
            b->inPrev = u;
        }
    }
    D_800E76D0 += n;
    if (D_800EE140 != 0) {
        for (b = D_800E37C0_20; b < D_800E37C0_20 + 8; b++) {
            if (b->b7A5 == 0) {
                func_8001FE1C(b);
            }
        }
    } else {
        for (b = D_800E37C0_20; b < D_800E37C0_20 + 8; b++) {
            if (b->b7A5 != 0) {
                continue;
            }
            if (b->b7A4 != 0) {
                func_8001FE1C(b);
                continue;
            }
            if (b->b6E8 != 0 && (b->s744 < -30 || (b->s744 >= 11 && b->f6F4 > 0.5f))
                && !(b == D_800E37C0_20 && (D_800EFD70 & 0x81))) {
                if (((D_800E76D0 ^ (b - D_800E37C0_20)) & 1) != 0) {
                    func_80021EC8(b, 2);
                    b->fpos = b->prev;
                } else {
                    t = b->time / 60.0f;
                    t2 = t * t * 0.5f;
                    x = b->vel.x * t;
                    y = b->vel.y * t;
                    z = b->vel.z * t;
                    b->fpos.x += x + b->acc.x * t2;
                    b->fpos.y += y + b->acc.y * t2;
                    b->fpos.z += z + b->acc.z * t2;
                    t *= 1000.0f;
                    t2 *= 1000.0f;
                    b->pos[0] = (f32)b->pos[0] + (b->vel.x * t + b->acc.x * t2);
                    b->pos[1] = (f32)b->pos[1] + (b->vel.y * t + b->acc.y * t2);
                    b->pos[2] = (f32)b->pos[2] + (b->vel.z * t + b->acc.z * t2);
                }
            } else {
                for (i = 0; i < n; i++) {
                    func_80021EC8(b, 1);
                }
                b->fpos = b->prev;
            }
        }
        switch (D_800EFDBC) {
        case 2:
            func_8006C340();
            break;
        case 5:
            func_80047A90();
            break;
        case 6:
            func_8006B4EC();
            break;
        case 7:
        case 9:
        case 10:
            break;
        case 8:
            func_800644E0();
            func_80063768();
            break;
        default:
            func_80063D30();
            break;
        }
    }
    func_80036230();
    D_800E76CC = 0;
}

float func_800228A0(float *p)
{
    float x = p[0];
    int r = func_8002E920((int)(x * 1000.0f), (int)(p[1] * 1000.0f), (int)(p[2] * 1000.0f));
    return (float)r / 1000.0f;
}

struct Obj_10 {
    u8 pad0[0x48];
    Vec3f pos;
    u8 pad54[0xA0 - 0x54];
    f32 mtx[10];
    Vec3f pts[8];
};

void func_80022908(Obj_10 *p)
{
    s32 i;
    Vec3f out;

    D_800E7708 = D_800E76D0;
    for (i = 6; i < 8; i++) {
        func_8001327C(&D_800C94C8_2[i].x, p->mtx, &out.x);
        p->pts[i].x = out.x + p->pos.x;
        p->pts[i].y = out.y + p->pos.y;
        p->pts[i].z = out.z + p->pos.z;
    }
}

typedef struct {
    u8 pad[0x658];
    void *p658;
    void *p65C;
    u8 pad2[0x7E0 - 0x660];
} Rider_2;

extern Rider_2 D_800E37C0_21[];

void func_800229B8(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_800BCB04(D_800E37C0_21[i].p658);
        func_800BCB04(D_800E37C0_21[i].p65C);
    }
    func_8005C8FC();
    func_80061DA8();
    func_80064D10();
}

void func_80022A24(u8 *p) {
    func_8001CB80(p + 0x4F8, p + 0x388);
    func_8001CB80(p + 0x454, p + 0x388);
}

void func_80022A68(u8 *p)
{
    s32 st = *(u8 *)(p + 0x3FF);

    if (st == 1) goto arm1;
    if (st < 2) return;
    if (st == 2) goto arm2;
    return;

arm1:
    if (*(s8 *)(p + 0x3D0) == -1 && *(f32 *)(p + 0x420) < 8.0f && *(s32 *)(p + 0x744) < -30) {
        *(u8 *)(p + 0x3FF) = 2;
        func_8006FE84(p, *(s16 *)(p + 0x668) + 0x13);
        if ((D_800EFD70 & 0x8000) == 0) {
            *(u16 *)(p + 0x5AA) = 10;
        }
    }
    return;

arm2:
    if (*(s8 *)(p + 0x3D0) < 0) {
        *(u8 *)(p + 0x3FF) = 3;
        if (D_800EFDBC == 5) {
            *(u8 *)(p + 0x3FF) = 0;
            *(u8 *)(p + 0x6E8) = 1;
            *(u16 *)(p + 0x5AA) = *(u16 *)(p + 0x5A8);
        }
    }
}

struct Obj_11 {
    u8 pad0[0x419];
    u8 b419;
    u8 b41a;
    u8 b41b;
    u8 pad1[4];
    f32 f420;
    u8 pad2[0x5B0 - 0x424];
    u32 w5b0;
    u8 pad3[4];
    s32 w5b8;
    u8 pad4[0x5CF - 0x5BC];
    u8 b5cf;
};

void func_80022B50(Obj_11 *o) {
    o->b5cf = 0;
    if (5.0f < o->f420) {
        if (o->b41b != 0 || ((o->w5b0 & 0x20000) != 0 && (D_800E76D0 - o->w5b8 < 0x1E))) {
            o->b5cf = 2;
            return;
        }
        if ((*(volatile u8 *)&o->b41a | *(volatile u8 *)&o->b419) != 0) {
            o->b5cf = 1;
        }
    }
}
