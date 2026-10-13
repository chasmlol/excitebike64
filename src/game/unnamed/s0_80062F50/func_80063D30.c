#include "ctx.h"

typedef struct {
    u8 pad0[0x3FF];
    u8 b3FF;
    u8 pad400[0x44C - 0x400];
    f32 f44C;
    f32 f450;
    u8 pad454[0x62C - 0x454];
    f32 f62C;
    f32 f630;
    u8 pad634[0x660 - 0x634];
    s16 h660;
    s16 h662;
    u8 pad664[0x6E8 - 0x664];
    u8 b6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player;

typedef struct {
    u32 w0;
    u8 pad4[0x1C];
    s16 h20;
    s16 h22;
    u8 pad24[8];
    s32 w2C;
    u8 pad30[0x28];
    s32 w58;
} Mode;

typedef struct {
    s32 w0;
    u8 pad4[0x48];
    s32 w4C;
    u8 pad50[0x410 - 0x50];
    s32 w410;
} Cfg;

typedef struct {
    u8 pad0[0x20];
    f32 f20[4];
} Tune;

extern Player D_800E37C0[];
extern Mode D_800F05A8;
extern Cfg D_800EFD70;
extern Tune *D_800EFD64;
extern Player *D_800F0C10[];
extern s32 D_800E76D0;
extern s32 D_800EECB8;
extern s32 D_800EFD28[];
extern s32 D_800EFDAC;
extern f32 D_800EE48C;
extern s32 D_800C2280;

void func_80063488(u8 *p0);
void func_80063768(void);
void func_8004EADC(u32 param_1);
void func_800644E0(void);
void func_8004D1F4(u32 a, u32 b);

void func_80063D30(void)
{
    Player *list[4];
    Player **lp;
    Player *pl;
    s32 n1;
    s32 n0;
    f32 sum0;
    f32 sum1;
    s32 i;
    s32 k;
    s32 t;
    s32 best;
    s32 idx;
    f32 f;
    f32 d;
    f32 e;
    f32 g;

    func_80063768();
    n1 = 0;
    n0 = 0;
    sum0 = 0.0f;
    sum1 = 0.0f;
    for (i = 0; i < D_800F05A8.h22; i++) {
        if (D_800E37C0[i].b7A5 == 0) {
            switch (D_800E37C0[i].b6E8) {
            case 0:
                list[n0] = &D_800E37C0[i];
                n0++;
                sum0 += D_800E37C0[i].f630;
                break;
            case 1:
                n1++;
                sum1 += D_800E37C0[i].f630;
                break;
            }
        }
    }
    if (n0 != 0) {
        t = D_800E76D0;
        if (t - D_800EECB8 > 0x384) {
            best = t;
            idx = -1;
            if (n0 >= 2) {
                for (i = 0; i < D_800EFDAC; i++) {
                    if (D_800EFD28[i] < best) {
                        best = D_800EFD28[i];
                        idx = i;
                    }
                }
                if (idx >= 0) {
                    if (D_800E37C0[idx].f62C > 150.0f || (D_800E37C0[idx].h662 != 0 && D_800E37C0[idx].h660 != D_800F05A8.h20 - 1)) {
                        func_8004D1F4(idx, D_800E37C0[idx].h662 + 1);
                    }
                }
            } else {
                pl = D_800E37C0;
                if (!(t - D_800EFD28[0] > 0x4B0 && (pl->f62C > 150.0f || (pl->h662 != 0 && pl->h660 != D_800F05A8.h20 - 1)))) {
                    if (pl->h662 != 0) {
                        pl = D_800F0C10[pl->h662 - 1];
                    } else {
                        pl = D_800F0C10[pl->h662 + 1];
                    }
                }
                func_8004D1F4(pl - D_800E37C0, pl->h662 + 1);
            }
        }
        if (D_800F05A8.w2C == 300) {
            func_8004EADC(D_800F0C10[0] - D_800E37C0);
        }
    }
    if (n1 != 0) {
        sum1 /= n1;
    }
    if (n0 != 0) {
        sum0 /= n0;
    } else {
        sum0 = sum1;
    }
    if (n1 == 0) {
        sum1 = sum0;
    }
    if (D_800EFD70.w0 & 0x10) {
        k = 0;
        for (i = 0; i < D_800F05A8.h22; i++) {
            pl = D_800F0C10[i];
            if (pl->b6E8 == 1) {
                e = pl->f630 - sum1;
                e += (D_800EE48C / n1) * (k - (n1 - 1) * 0.5f);
                f = 1.0f - e / 100.0f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
                pl->f44C = f;
                k++;
            }
        }
    } else if (n0 == 0) {
        k = 0;
        for (i = 0; i < D_800F05A8.h22; i++) {
            pl = D_800F0C10[i];
            if (pl->b6E8 == 1) {
                e = pl->f630 - sum1;
                e -= (k - (n1 - 1) * 0.5f) * 0.01f;
                f = 1.0f - e * 0.00390625f;
                if (f < 0.9f) {
                    f = 0.9f;
                }
                if (f > 1.1f) {
                    f = 1.1f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
                pl->f44C = f;
                k++;
            }
        }
    } else if (D_800EFD70.w4C != 8 && D_800F05A8.w0 >= 5) {
        d = ((sum1 - sum0) - 30.0f) * 0.00390625f;
        if (d > 0.15f) {
            d = 0.15f;
        } else if (d < -0.15f) {
            d = -0.15f;
        }
        for (i = 0; i < D_800F05A8.h22; i++) {
            pl = &D_800E37C0[i];
            if (pl->b6E8 != 0) {
                f = 0.0f;
                if (n1 >= 2) {
                    f = pl->f450;
                }
                e = pl->f630 - sum1;
                if (e > 64.0f) {
                    f -= 0.1f;
                }
                f += 1.0f - d;
                if (pl->b3FF) {
                    f = 1.0f;
                } else if (D_800C2280 & 0x1000) {
                    f *= D_800EFD64->f20[2] + 0.05f;
                } else {
                    g = D_800EFD64->f20[D_800EFD70.w410 - 1];
                    f *= g;
                    if (D_800F05A8.w58 & 0x200) {
                        f = g * 1.15f;
                    }
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.0002f;
                } else {
                    pl->f44C = pl->f44C + 0.0002f;
                }
            } else {
                e = sum0 - pl->f630;
                if (e > 2.0f) {
                    e += 16.0f;
                } else if (e < -2.0f) {
                    e -= 16.0f;
                }
                f = e * 0.0009765625f + 1.0f;
                if (f > 1.05f) {
                    f = 1.05f;
                } else if (f < 0.95f) {
                    f = 0.95f;
                }
                if (f < pl->f44C) {
                    pl->f44C = pl->f44C - 0.001f;
                } else {
                    pl->f44C = pl->f44C + 0.001f;
                }
            }
        }
    }
    if (D_800EFDAC == 1 && n0 == 1) {
        func_80063488((u8 *)list[0]);
    }
    func_800644E0();
}
