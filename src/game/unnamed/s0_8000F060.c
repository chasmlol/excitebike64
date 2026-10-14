#include "game.h"

typedef struct {
    s32 *p;
    s32 v;
} Pair_3;

typedef struct {
    s32 a;
    s32 id;
} Snd;

extern Pair_3 D_800C2C10[];

void func_8000F060(void)
{
    s32 l[4];

    D_800D5604 = 0;
    D_800E32A0 = 0x100;
    func_800BBAB0((s32)D_800C04EC);
    D_800FE860 = (u32)D_800D563C;
    func_800B8B0C(&l[0], &l[1], &l[2], &l[3]);
    {
        s32 p0, p1, p2, p3;
        p0 = l[0];
        p1 = l[1];
        p2 = l[2];
        p3 = l[3];
        func_800B8B34(p0, p1, p2, p3, (void *)D_800E32A4, (void *)D_800E35C8);
    }
    if (D_800D5604 != 0) {
        func_800B99F0(0x400);
        func_800B9A18(0x258);
        D_800D5710 = (u32)func_800110DC;
        D_800D5730 = (u32)func_800110DC;
        D_800C2B5C = 1;
        D_800C2B60 = 0;
        func_8004B710(4);
        func_800BBABC(D_800C29E0, 0x25800);
        func_80010FC8();
        func_800BBABC(0, 0);
    }
}

typedef struct {
    s32 *p;
    s32 b;
    s32 c;
    s32 d;
    s32 pad[2];
} Ent_6;

extern Ent_6 D_800C2B68_2[];

void func_8000F164(void) {
    u32 i;
    s32 local[4];

    if (D_800C2AD0 == 0) {
        func_800BBABC(D_800C29E0, 0x25800);
        for (i = 0; i < 7; i++) {
            if (D_800C2B68_2[i].p != NULL) {
                D_800C2B68_2[i].b = func_800B781C(*D_800C2B68_2[i].p);
            } else {
                D_800C2B68_2[i].b = D_800C2B68_2[i - 1].b;
            }
            func_800B8E08(D_800C2B68_2[i].b, local);
            D_800C2B68_2[i].c = local[0];
            D_800C2B68_2[i].d = func_800BBB88(D_800C2B68_2[i].b);
        }
        func_800BBABC(0, 0);
        D_800C2AD0 = 1;
    }
}

typedef struct {
    s32 f0, f4, f8, fC, f10;
    s32 pad14[9];
    s32 f38, f3C;
    s32 pad40;
    s32 f44, f48, f4C, f50;
    s32 pad54[4];
} Ent_7;

struct Slot_2 {
    u8 pad[0x38];
    s32 a;
    s32 r;
};

extern Ent_7 D_800E32A8_2[];

void func_8000F240(Slot_2 *param_1)
{
    s32 i;
    u32 idx;
    u32 f;
    s32 a;
    volatile s32 pad[4];

    if (D_800C2B5C == 0) {
        return;
    }
    if (D_800C2358 != 0) {
        return;
    }
    if (!(D_800EFD70_5[0] & 0x80)) {
        func_8000F164();
    }
    i = D_800EFD64_2 - D_800CB210;
    if (i == 0xD || i == 0x13 || i == 0x11) {
        D_800C2AD4 = 1;
    } else {
        D_800C2AD4 = 0;
    }
    if (osMemSize > 0x400000) {
        D_800C2AD4 = 0;
    }
    func_800BBABC(D_800C29E0, 0x25800);
    i = 0;
    D_800C2B64 = 1;
    func_8000ED10((u8 *)D_800E32A8_2, i, 800);
    for (i = 0; i < 8; i++) {
        D_800E32A8_2[i].f38 = -0x28;
        D_800E32A8_2[i].f3C = -0x14;
        D_800E32A8_2[i].f0 = 0x3C;
        D_800E32A8_2[i].fC = -1;
        D_800E32A8_2[i].f4 = -1;
        D_800E32A8_2[i].f8 = -1;
        D_800E32A8_2[i].f10 = -1;
        D_800E32A8_2[i].f44 = -1;
        D_800E32A8_2[i].f48 = -1;
        D_800E32A8_2[i].f4C = -1;
        D_800E32A8_2[i].f50 = -1;
    }
    for (i = 0; D_800C2C10[i].p != 0; i++) {
        if (D_800C2AD4 != 0 && i >= 8) {
            D_800C2C10[i].v = -1;
        } else {
            D_800C2C10[i].v = func_800B781C(*D_800C2C10[i].p);
        }
    }
    D_800E35CC = param_1;
    for (i = 0; i < 16; i++) {
        a = D_800E35CC[i].a;
        if (a == 0) {
            D_800E35CC[i].r = -1;
        } else {
            if (a & 0x80000000) {
                a = *(s32 *)a;
            }
            D_800E35CC[i].r = func_800B781C(a);
        }
    }
    if (func_8004F180() == 0) {
        D_800C2B44 = 1;
        if (D_800C8CE8 != 0) {
            func_8004C22C(D_800D11D8);
        } else {
            idx = D_800EFD64_2 - D_800CB210;
            f = D_800EFD70_5[0];
            if ((f & 0x91) == 1 || !(f & 1)) {
                if (f & 0x40000) {
                    func_8004C22C(D_800D11E4);
                } else if (f & 0x1000000) {
                    func_8004C22C(D_800D11D8);
                } else if (f & 0x80) {
                    func_8004C22C(D_800D11EC);
                } else if (f & 1) {
                    func_8004C22C(D_800D1214);
                } else if (D_800EFD70_5[0x13] != 9) {
                    if (idx < 0x1B) {
                        func_8004C22C(*D_800C2AD8[idx]);
                    } else {
                        func_8004C22C(D_800D1224);
                    }
                }
            }
        }
    } else {
        D_800C2B44 = 0;
    }
    func_800BBABC(0, 0);
    func_800B99B0((s32)D_800111A0);
}

typedef struct {
    s32 a;
    s32 arg;
    s32 pad[4];
} Ent18;

typedef struct {
    s32 key;
    s32 arg;
} Ent8;

typedef struct {
    u8 pad[0x3C];
    s32 arg;
} Ent40;

extern Ent8 D_800C2C10_2[];
extern Ent18 D_800C2B68_3[];
extern Ent40 *D_800E35CC_2;

void func_8000F600(void)
{
    s32 i;
    u32 j;
    u8 *p;
    Ent8 *q;
    s32 a;

    if (D_800C2B64 != 0) {
        D_800C2B64 = 0;
        if (D_800C2B44 != 0) {
            func_8004C3C4();
        }
        func_800B99B0(0);
        for (i = 0; i < 8; i++) {
            p = D_800E32A8 + i * 0x64;
            if (D_800C2B64 != 0) {
                func_80011098((s32 *)(p + 0xC));
                func_80011098((s32 *)(p + 0x4));
                func_80011098((s32 *)(p + 0x44));
                func_80011098((s32 *)(p + 0x50));
                func_80011098((s32 *)(p + 0x4C));
                func_80011098((s32 *)(p + 0x48));
            }
        }
        func_8004F534();
        func_800B8F2C();
        func_800B7BE0();
        func_800B98B4();
        if (D_800C2AD0 != 0) {
            for (j = 0; j < 7; j++) {
                func_800B78F0(D_800C2B68_3[j].arg);
            }
            D_800C2AD0 = 0;
        }
        if (D_800C2C10_2[0].key != 0) {
            i = 0;
            do {
                if (D_800C2C10_2[i].arg != -1) {
                    func_800B78F0(D_800C2C10_2[i].arg);
                }
                i++;
            } while (D_800C2C10_2[i].key != 0);
        }
        for (i = 0; i < 16; i++) {
            a = D_800E35CC_2[i].arg;
            if (a != -1) {
                func_800B78F0(a);
            }
        }
        func_800BC370();
        while (D_800D5638 != 0) {
            func_800BBDE0();
        }
    }
}

typedef struct Ent_8 {
    s32 x, y, z;
    u8 pad0C[0x5AA - 0xC];
    s16 f5AA;
    u8 pad5AC[0x6E8 - 0x5AC];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Ent_8;

typedef struct Slot_3 {
    u8 pad00[0x28];
    s32 vol;
    s32 best;
    s32 dist;
    s32 delta;
    u8 pad38[0x54 - 0x38];
    s32 taken;
    u8 pad58[0x64 - 0x58];
} Slot_3;

extern Ent_8 D_800E37C0_11[];
extern Slot_3 D_800E32A8_3[];

void func_8000F7A4(void)
{
    Vec *pts;
    Vec *pt;
    Slot_3 *sl;
    Ent_8 *e;
    Ent_8 *base;
    s32 i, bi, dist, n, k, j, best, bv;
    f32 z, t, kk, d, bd, ex, ey, ez, v;

    func_8004C4E4();
    if (D_800C2B64 == 0) {
        return;
    }
    if (D_800C2B58 < 0) {
        D_800C2B58 = 0;
    }
    if (D_800C2B58 >= 0x100) {
        D_800C2B58 = 0xFF;
    }
    base = D_800E37C0_11;
    pts = D_800E8090;
    sl = D_800E32A8_3;
    e = base;
loop:
    if (e->f7A5 != 0) {
        sl->delta = 0;
        sl->best = 0;
        sl->dist = 0;
        sl->vol = 0;
    } else {
        bd = 0.0f;
        n = 0;
        if (n < D_800C2594) {
            do {
            do {
                pt = &pts[n];
                d = pt->x;
                d -= (f32)e->x;
                d *= d;
                d += ((f32)pt->y - (f32)e->y) * ((f32)pt->y - (f32)e->y);
                d += ((f32)pt->z - (f32)e->z) * ((f32)pt->z - (f32)e->z);
                if (d < bd || n == 0) {
                    bd = d;
                    bi = n;
                }
                n++;
            } while (n < D_800C2594);
            } while (0);
        }
        bd = __builtin_sqrtf(bd);
        if (D_800EFD64_7 - D_800CB210_3 == 20 && ((s32)e ^ (s32)base) == 0 && base->f5AA == 4) {
            bd = 10000.0f;
        }
        v = bd;
        dist = (s32)v;
        if (v < 5000.0f) {
            v = 5000.0f;
        }
        if (v > 30000.0f) {
            v = 30000.0f;
        }
        v = 30000.0f - v;
        v = v * 255.0f / 25000.0f;
        if (v < 0.0f) v = 0.0f;
        if (v > 0.0f) k = 1; else k = 2;
        if (v < 25000.0f) k = 1; else k = 2;
        if (v > 25000.0f) k = 1; else k = 2;
        if (v > 255.0f) v = 255.0f;



        sl->vol = (s32)v;
        if (sl->best == bi) {
            sl->delta = sl->dist - dist;
        } else {
            sl->delta = 0;
        }
        sl->best = bi;
        sl->dist = dist;
        sl->taken = 0;
    }
    e++;
    sl++;
    if ((s32)e < (s32)(base + 8)) {
        goto loop;
    }

    n = D_800C2B48[D_800E1898 - 1];
    for (k = 0; k < n; k++) {
        bv = -1;
        best = 0;
        for (i = 0; i < 8; i++) {
            if (D_800E37C0_11[i].f7A5 == 0 && D_800E32A8_3[i].taken == 0) {
                if (bv < D_800E32A8_3[i].vol ||
                    (D_800E37C0_11[i].f6E8 == 0 && D_800E37C0_11[best].f6E8 != 0)) {
                    bv = D_800E32A8_3[i].vol;
                    best = i;
                }
            }
        }
        if (bv == -1) {
            break;
        }
        D_800E32A8_3[best].taken = 1;
    }
    for (i = 0; i < 8; i++) {
        if (D_800E32A8_3[i].taken == 0) {
            D_800E32A8_3[i].vol = 0;
        }
        func_8000FB44(i);
    }
}

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    s32 f44;
    s32 f48;
    s32 f4C;
    s32 f50;
    s32 f54;
    s32 f58;
    s32 f5C;
    s32 f60;
} Snd_2;

typedef struct {
    u8 pad0[0x38];
    s32 f38;
    s32 f3C;
} Surf;

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x420 - 0x45];
    f32 f420;
    u8 pad424[0x4E4 - 0x424];
    Surf *f4E4;
    u8 pad4E8[0x4F0 - 0x4E8];
    f32 f4F0;
    u8 pad4F4[0x5B0 - 0x4F4];
    u32 f5B0;
    u8 pad5B4[0x644 - 0x5B4];
    f32 f644;
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 f655;
    u8 f656;
    u8 pad657[0x6E8 - 0x657];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x764 - 0x748];
    u8 f764;
    u8 pad765[3];
    s32 f768;
    u8 pad76C[0x7A5 - 0x76C];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_3;

extern Bike_3 D_800E37C0_12[];
extern Snd_2 D_800E32A8_4[];

void func_8000FB44(s32 arg0) {
    Snd_2 *s;
    s32 vol;
    s32 vol2;
    s32 v;
    s32 k;
    s32 g;
    s32 n;
    f32 f;
    f32 x;
    s32 buf[4];

    if (D_800C2B5C != 0 && D_800C2B64 != 0 && D_800C2AD0 != 0) {
        s = &D_800E32A8_4[arg0];
        if (D_800E37C0_12[arg0].f656 != 0) {
            s->f58 = 10;
        }
        if (s->f58 != 0) {
            s->f58--;
        }
        if (D_800E37C0_12[arg0].f5B0 & 0x20000) {
            s->f5C = 20;
            s->f60++;
        }
        if (s->f5C != 0) {
            if (--s->f5C == 0) {
                s->f60 = 0;
            }
        }
        if (s->f58 != 0 && D_800E3710 == 0) {
            if (D_800E37C0_12[arg0].f654 != 0 && (func_8000EFE8_2() & 7) == 0) {
                func_8003AA8C(arg0);
            }
            if ((D_800E37C0_12[arg0].f5B0 & 0x20000) && (func_8000EFE8_2() & 7) == 0) {
                func_8003AA8C(arg0);
            }
        }
        vol = (D_800C2B58 * s->f28) / 256;
        if (vol <= 32) {
            if (s->fC != -1) {
                func_800B8E98(s->fC);
                s->fC = -1;
            }
            if (s->f4 != -1) {
                func_800B8E98(s->f4);
                s->f4 = -1;
            }
            if (s->f44 != -1) {
                func_800B8E98(s->f44);
                s->f44 = -1;
            }
            if (s->f50 != -1) {
                func_800B8E98(s->f50);
                s->f50 = -1;
            }
            if (s->f4C != -1) {
                func_800B8E98(s->f4C);
                s->f4C = -1;
            }
            if (s->f48 != -1) {
                func_800B8E98(s->f48);
                s->f48 = -1;
            }
            if ((func_8000EFE8_2() & 0x3F) == 0 && D_800E37C0_12[arg0].f7A5 == 0) {
                func_8003AA8C(arg0);
            }
            return;
        }

        if (D_800E37C0_12[arg0].f764 & 1) {
            v = D_800E37C0_12[arg0].f768;
            v = (v * vol) >> 8;
            v = v * 2 / 3;
            D_800E37C0_12[arg0].f764 &= 0xFE;
            if (v > 255) {
                v = 255;
            }
            func_800B7990(D_800C2C10[0].v, 0x5622, v, 7, 0, 1);
        }

        if (D_800C2594 == 1 && s->f2C == 0) {
            f = (f32)s->f34 * 60.0f / 1000.0f;
            f = f / 331.45f;
            f = f * 2.0f;
            f = f - 1.0f;
            if (f < 0.1f && f > -0.1f) {
                f = -1.0f;
            }
            f = -1.0f / f;
            if (f > 1.2f) {
                f = 1.2f;
            }
            if (f < 0.8f) {
                f = 0.8f;
            }
        } else {
            f = 1.0f;
        }

        v = 0;
        if (D_800E37C0_12[arg0].f654 == 0) {
            x = D_800E37C0_12[arg0].f648;
            if (x > 10000.0f && D_800E37C0_12[arg0].f6E8 == 0 && (D_800EFDBC != 8 || arg0 != D_800E3700)) {
                v = (s32)((x - 10000.0f) * 70.0f / 2000.0f) + 64;
                v = (v * vol) >> 8;
                if (v < 0) {
                    v = 0;
                }
            }
        }
        if (v != 0) {
            if (s->f4C == -1) {
                s->f4C = func_800B7990(D_800C2C10[2].v, 8000, v, 7, 1, 1);
            }
            if (s->f4C != -1) {
                func_800B9188(s->f4C, v);
            }
        } else if (s->f4C != -1) {
            func_800B8E98(s->f4C);
            s->f4C = -1;
        }

        v = 0;
        if (D_800E37C0_12[arg0].f44 == 0) {
            v = (s32)(D_800E37C0_12[arg0].f4F0 * 128.0f * 0.5f);
            if (v > 192) {
                v = 192;
            }
            v = (v * vol) >> 8;
            if (v < 0) {
                v = 0;
            }
        }
        if (v != 0) {
            if (s->f44 == -1) {
                s->f44 = func_800B7990(D_800C2C10[1].v, 0x2B11, v, 7, 0, 1);
            } else {
                func_800B9188(s->f44, v);
            }
        } else if (s->f44 != -1) {
            func_800B8E98(s->f44);
            s->f44 = -1;
        }

        if (D_800E76D0 < s->f38) {
            s->f38 = -30;
        }
        k = s->f40 * vol / 255;
        k = k * 350 / 255;
        if (k < 32) {
            k = 0;
        }
        if (k > 255) {
            k = 255;
        }
        if (k != 0 && D_800E76D0 - s->f38 < 30) {
            if (s->f50 == -1) {
                s->f50 = func_800B7990(D_800C2C10[5].v, 0x2B11, k, 7, 0, 1);
                if (D_800E37C0_12[arg0].f44 != 0) {
                    func_800B8E08(D_800C2C10[5].v, buf);
                    func_800B93E8(s->f50, buf[1]);
                }
            } else {
                func_800B9188(s->f50, k);
            }
        } else if (s->f50 != -1) {
            func_800B8E98(s->f50);
            s->f50 = -1;
        }

        v = D_800E37C0_12[arg0].f655;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        if (D_800E32A8_4[arg0].f58 != 0 && v == s->f8) {
            s->f1C++;
        } else {
            s->f1C = 0;
        }
        if (s->f1C == s->f0) {
            s->f8 = -999;
        }
        vol2 = vol * 200 / 256;

        if (D_800E32A8_4[arg0].f58 != 0) {
            if (s->f4 != -1) {
                func_800B8E98(s->f4);
                s->f4 = -1;
            }
            s->f10 = 0;
            if (v != s->f8) {
                if (D_800E3710 == 0) {
                    func_8003AA8C(arg0);
                }
                if (s->fC != -1) {
                    func_800B8E98(s->fC);
                    s->fC = -1;
                }
            }
            g = v;
            if (s->fC == -1) {
                if (v == 3 || v == 4) {
                    g = (func_8000EFE8_2() & 1) + 3;
                }
                if (s->f1C >= s->f0) {
                    s->f1C = 0;
                    if (g >= 4) {
                        s->f0 = func_8000EFE8_2() % 20 + 20;
                    } else {
                        s->f0 = func_8000EFE8_2() % 30 + 30;
                    }
                }
                s->fC = func_800B7990(D_800C2B68[g].f4, 0, 0xC0, 7, 0, 1);
                if (v == s->f8 - 1 && s->fC != -1) {
                    func_800B8E08(D_800C2B68[g].f4, buf);
                    func_800B93E8(s->fC, buf[1]);
                }
                s->f20 = g;
                s->f24 = 0;
                if (s->fC == -1) {
                    goto end;
                }
            }
            k = (s32)(D_800E37C0_12[arg0].f644 - 5000.0f);
            k = D_800C2B68[s->f20].f10 + (D_800C2B68[s->f20].f14 - D_800C2B68[s->f20].f10) * k / 7000;
            k += func_8000EFE8_2() * 500 / 256 - 250;
            n = vol2;
            if (s->f5C != 0) {
                if (D_800E37C0_12[arg0].f6E8 == 0) {
                    k = 32000;
                }
                n = vol2 * 2;
            } else {
                s32 m = 300;
                k = k * m / 256;
            }
            if (D_800E37C0_12[arg0].f654 != 0) {
                k /= 2;
                n *= 2;
            }
            if (n > 255) {
                n = 255;
            }
            if (n < 0) {
                n = 0;
            }
            func_800B92F8(s->fC, (s32)(k * f) * 16 / 22);
            func_800B9188(s->fC, n);
        } else {
            if (++s->f10 >= 7) {
                s->f10 = 6;
                if (s->fC != -1) {
                    func_800B8E98(s->fC);
                    s->fC = -1;
                }
            }
            k = s->f10 * 255 / 6;
            if (s->fC != -1) {
                func_800B9188(s->fC, (255 - k) * 192 / 256 * vol2 / 256);
            }
            if (s->f4 == -1) {
                s->f4 = func_800B7990(D_800C2B68[5].f4, 0, 0xC0, 7, 0, 1);
            }
            if (s->f14 != 0) {
                s->f14 += s->f18;
                if (s->f14 < 0) {
                    s->f14 = 0;
                }
            }
            if (s->f14 > 64) {
                s->f18 = -2;
            }
            if (s->f8 == v + 1) {
                s->f14 = 1;
                s->f18 = 4;
            }
            if (s->f4 != -1) {
                s32 t = k / 2 + s->f14;
                if (t > 255) {
                    t = 255;
                }
                func_800B9188(s->f4, t * vol2 / 256);
                k = (s32)(D_800E37C0_12[arg0].f644 - 1000.0f);
                k = D_800C2B68[5].f10 + (D_800C2B68[5].f14 - D_800C2B68[5].f10) * k / 7000;
                func_800B92F8(s->f4, (s32)(k * f) * 16 / 22);
            }
        }
end:
        s->f8 = v;
        v = (s32)(D_800E37C0_12[arg0].f420 * 255.0f / 15.0f);
        if (v > 255) {
            v = 255;
        }
        if (v < 0) {
            v = 0;
        }
        if (D_800E37C0_12[arg0].f4E4->f38 != 0 && v != 0 && D_800E37C0_12[arg0].f744 < 6) {
            if (s->f48 == -1) {
                s->f48 = func_800B7990(D_800E37C0_12[arg0].f4E4->f3C, 0x2B11, (vol * v) >> 8, 7, 0, 1);
            } else {
                func_800B9188(s->f48, (vol * v) >> 8);
            }
        } else if (s->f48 != -1) {
            func_800B8E98(s->f48);
            s->f48 = -1;
        }
    }
}

typedef struct {
    s32 w0[10];
    s32 f28;
    s32 w1[4];
    s32 f3c;
    s32 w2[9];
} Rec25;

extern Rec25 D_800E32A8_5[];

void func_80010994(s32 k, s32 n, s32 vol)
{
    s32 buf[4];
    Snd *s, *tb;
    s32 t, x;

    if (D_800C2B64 != 0) {
        if (k == 8 && D_800C2C54 == -1) {
            k = 4;
        }
        if (k == 4 || k == 8) {
            if (D_800E76D0 - D_800E32A8_5[n].f3c < 20) {
                return;
            }
            D_800E32A8_5[n].f3c = D_800E76D0;
        }
        tb = D_800C2C10;
        s = &tb[k];
        func_800B8E08(s->id, buf);
        t = D_800C2B58 * D_800E32A8_5[n].f28 / 256;
        x = t * vol;
        t = x / 255;
        if (t >= 32) {
            if (t > 255) {
                t = 255;
            }
            func_800B7990(s->id, buf[3], t, 7, 1, 0);
        }
    }
}

void func_80010AFC(void)
{
    s32 iVar1;
    s32 a;

    if (D_800C2B64 == 0) {
        while (D_800D33F8 != 0) {
            if (D_800C2B5C != 0 && D_800C2B60 == 0) {
                a = 0; a = 2; a = 3;
                func_8004F8A4();
                func_800BBBAC(D_800E32A0);
                D_800C2B60 = 1;
                func_800BBDE0();
                func_800B98B4();
                func_800BED20();
                func_8004F7E8();
                D_800C2B60 = 0;
                if (D_800C2B64 == 0) goto z1;
                a = 0;
                if (D_800E3710 != 0) goto c1;
                if (D_800E1898 != 1) goto c1;
                a = 1;
                if (D_800E7FF0 & 0x40) goto c1;
            z1:
                a = 0;
            c1:
                func_800BA4A8(a);
            }
        }
        iVar1 = D_8010062C;
        {
        while (D_8010062C < iVar1 + 2) {
            if (D_800C2B5C != 0 && D_800C2B60 == 0) {
                a = 0; a = 2; a = 3;
                func_8004F8A4();
                func_800BBBAC(D_800E32A0);
                D_800C2B60 = 1;
                func_800BBDE0();
                func_800B98B4();
                func_800BED20();
                func_8004F7E8();
                D_800C2B60 = 0;
                if (D_800C2B64 == 0) goto z2;
                a = 0;
                if (D_800E3710 != 0) goto c2;
                if (D_800E1898 != 1) goto c2;
                a = 1;
                if (D_800E7FF0 & 0x40) goto c2;
            z2:
                a = 0;
            c2:
                func_800BA4A8(a);
            }
        }
        }
    }
}

void func_80010CFC(void)
{
    if (D_800C2B5C != 0) {
        func_8004F190();
        D_800D5710 = 0;
        D_800C2B5C = 0;
        D_800D5730 = 0;
        func_800B8C8C();
    }
}

void func_80010D44(void) {
    if (D_800C2B5C != 0 && D_800CEC04 == 0) {
        func_800110DC();
        func_800B98B4();
    }
}

void func_80010D84(s32 a0) {
    Pal *e;
    Pal *b;
    s32 buf[4];
    s32 n;

    if (D_800C2B64 != 0) {
        b = D_800C2C10;
        e = b + a0;
        func_800B8E08(e->f4, buf);
        n = D_800C2B58;
        if (n > 0xFF) {
            n = 0xFF;
        }
        func_800B7990(e->f4, buf[3], n, 7, 1, 0);
    }
}

void func_80010DF8(u32 a) {
    D_800E32A0 = a;
    func_8004F8A4();
}

s32 func_80010E18(void) {
    return D_800E32A0;
}

void func_80010E28(s32 a0, s32 a1, s32 a2, s32 a3)
{
    u8 *base;
    u8 *rec;
    s32 off;
    s32 v;

    base = D_800E32A8;
    off = a0 * 100;
    v = D_800E76D0;
    rec = base + off;
    *(s32 *)(rec + 0x40) = a1;
    *(s32 *)(rec + 0x38) = v;
}

void func_80010E5C(s32 mode, s32 v)
{
    s32 *p;

    if (mode == 1) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        goto end;
    }
    if (mode == 2) {
        goto m2;
    }
    goto end;
m0:
    D_800C2B58 = v;
    D_800CA854 = v;
    goto end;
m1:
    D_800CA848 = v;
    p = &D_800CA848;
    p[1] = v;
    goto end;
m2:
    D_800CA850 = v;
end:
    func_8004F8A4();
}

s32 func_80010ED4(s32 param_1)
{
    switch (param_1) {
    case 0:
        return D_800C2B58;
    case 1:
        return D_800CA848;
    case 2:
        return D_800CA850;
    default:
        return 0;
    }
}

void func_80010F40(void) {
    s32 i;
    u8 *p;
    u8 *hb;

    i = 0;
    hb = (u8 *)0x800C0000;
    p = D_800E32A8;
    do {
        if (*(s32 *)(hb + 0x2B64) != 0) {
            func_80011098((s32 *)(p + 0xC));
            func_80011098((s32 *)(p + 0x4));
            func_80011098((s32 *)(p + 0x44));
            func_80011098((s32 *)(p + 0x50));
            func_80011098((s32 *)(p + 0x4C));
            func_80011098((s32 *)(p + 0x48));
        }
        i++;
        p += 0x64;
    } while (i < 8);
}

void func_80010FC8(void)
{
    D_800E3290[0] = func_800B781C(D_800D11D0);
    D_800E3290[1] = func_800B781C(D_800D1230);
    D_800E3290[2] = func_800B781C(D_800D1240);
    D_800E3290[3] = func_800B781C(D_800D11CC);
}

void func_8001102C(s32 param_1) {
    func_800B7990(D_800E3290[param_1], 0x2B11, 0xFF, 7, 0, 0);
}

void func_8001106C(void) {
    func_8004C3C4();
    func_8004C22C(D_800D11EC);
}

void func_80011098(s32 *p) {
    if (*p != -1) {
        func_800B8E98(*p);
        *p = -1;
    }
}

void func_800110DC(void)
{
    s32 flag;

    if (D_800C2B5C != 0 && D_800C2B60 == 0) {
        func_8004F8A4();
        func_800BBBAC(D_800E32A0);
        D_800C2B60 = 1;
        func_800BBDE0();
        func_800B98B4();
        func_800BED20();
        func_8004F7E8();
        D_800C2B60 = 0;
        if (D_800C2B64 == 0 || D_800E3710 != 0 || D_800E1898 != 1 || !(D_800E7FF0 & 0x40)) {
            flag = 0;
        } else {
            flag = 1;
        }
        func_800BA4A8(flag);
    }
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
} T18;

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    u8 pad[0x64 - 0x28];
} Ent_9;

extern T18 D_800C2B68_4[];
extern Ent_9 D_800E32A8_6[];

void func_800111A0(void)
{
    s32 i;
    s32 loc[4];
    u32 r;

    if (D_800C2B64 != 0) {
        for (i = 0; i < 8; i++) {
            if (D_800E32A8_6[i].f0c != -1 && (u32)(D_800E32A8_6[i].f20 - 1) < 4 && D_800E32A8_6[i].f24 == 0) {
                r = func_800BA330(D_800E32A8_6[i].f0c);
                func_800B8E08(D_800C2B68_4[D_800E32A8_6[i].f20].f04, loc);
                if ((s32)r >= loc[1]) {
                    func_800BA3F4(D_800E32A8_6[i].f0c, D_800C2B68_4[6].f04);
                    func_800B93E8(D_800E32A8_6[i].f0c, r - loc[1]);
                    D_800E32A8_6[i].f24 = 1;
                }
            }
        }
    }
}

void func_800112A4(void)
{
    Gear *p;
    u32 i;

    if (D_800C2AD0 != 0) {
        i = 0;
        p = D_800C2B68;
        do {
            s32 a = p->f4;
            p++;
            i++;
            func_800B78F0(a);
        } while (i < 7);
        D_800C2AD0 = 0;
    }
}

void func_80011300(s32 a0)
{
    u8 *s;

    if (D_800C2B64 != 0) {
        s = D_800E32A8 + a0 * 100;
        func_80011098((s32 *)(s + 0xC));
        func_80011098((s32 *)(s + 0x4));
        func_80011098((s32 *)(s + 0x44));
        func_80011098((s32 *)(s + 0x50));
        func_80011098((s32 *)(s + 0x4C));
        func_80011098((s32 *)(s + 0x48));
    }
}

typedef struct { u8 pad[0x28]; s32 vol; u8 pad2[0x38]; } Ent100;

extern Ent100 D_800E32A8_7[];

void func_80011378(s32 i, s32 j, s32 vol, s32 pan, s32 arg4)
{
    s32 buf[4];
    s32 v;

    if (D_800C2B64 != 0) {
        Snd *tab = D_800C2C10;
        Snd *s = &tab[i];
        func_800B8E08(s->id, buf);
        v = D_800C2B58 * D_800E32A8_7[j].vol / 256;
        v = v * vol / 255;
        if (v >= 0x20) {
            if (v > 0xff) v = 0xff;
            func_800B7990(s->id, buf[3] * pan / 256, v, 7, 1, 1);
        }
    }
}
