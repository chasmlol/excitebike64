#include "ctx.h"

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
} Snd;

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
} Bike;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
} Gear;

typedef struct {
    s32 *p;
    s32 v;
} Pair;

extern s32 D_800C2B5C;
extern s32 D_800C2B64;
extern s32 D_800C2AD0;
extern s32 D_800C2B58;
extern s32 D_800C2594;
extern s32 D_800E3710;
extern s32 D_800E3700;
extern s32 D_800EFDBC;
extern s32 D_800E76D0;
extern Bike D_800E37C0[];
extern Snd D_800E32A8[];
extern Gear D_800C2B68[];
extern Pair D_800C2C10[];

s32 func_8000EFE8(void);
void func_8003AA8C(s32 param_1);
void func_800B8E08(s32 id, s32 *out);
void func_800B8E98(s32 id);
void func_800B93E8(s32 p1, u32 p2);
void func_800B9188(s32 key, s32 val);
void func_800B92F8(s32 pk, s32 pv);
s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);

void func_8000FB44(s32 arg0) {
    Snd *s;
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
        s = &D_800E32A8[arg0];
        if (D_800E37C0[arg0].f656 != 0) {
            s->f58 = 10;
        }
        if (s->f58 != 0) {
            s->f58--;
        }
        if (D_800E37C0[arg0].f5B0 & 0x20000) {
            s->f5C = 20;
            s->f60++;
        }
        if (s->f5C != 0) {
            if (--s->f5C == 0) {
                s->f60 = 0;
            }
        }
        if (s->f58 != 0 && D_800E3710 == 0) {
            if (D_800E37C0[arg0].f654 != 0 && (func_8000EFE8() & 7) == 0) {
                func_8003AA8C(arg0);
            }
            if ((D_800E37C0[arg0].f5B0 & 0x20000) && (func_8000EFE8() & 7) == 0) {
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
            if ((func_8000EFE8() & 0x3F) == 0 && D_800E37C0[arg0].f7A5 == 0) {
                func_8003AA8C(arg0);
            }
            return;
        }

        if (D_800E37C0[arg0].f764 & 1) {
            v = D_800E37C0[arg0].f768;
            v = (v * vol) >> 8;
            v = v * 2 / 3;
            D_800E37C0[arg0].f764 &= 0xFE;
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
        if (D_800E37C0[arg0].f654 == 0) {
            x = D_800E37C0[arg0].f648;
            if (x > 10000.0f && D_800E37C0[arg0].f6E8 == 0 && (D_800EFDBC != 8 || arg0 != D_800E3700)) {
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
        if (D_800E37C0[arg0].f44 == 0) {
            v = (s32)(D_800E37C0[arg0].f4F0 * 128.0f * 0.5f);
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
                if (D_800E37C0[arg0].f44 != 0) {
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

        v = D_800E37C0[arg0].f655;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        if (D_800E32A8[arg0].f58 != 0 && v == s->f8) {
            s->f1C++;
        } else {
            s->f1C = 0;
        }
        if (s->f1C == s->f0) {
            s->f8 = -999;
        }
        vol2 = vol * 200 / 256;

        if (D_800E32A8[arg0].f58 != 0) {
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
                    g = (func_8000EFE8() & 1) + 3;
                }
                if (s->f1C >= s->f0) {
                    s->f1C = 0;
                    if (g >= 4) {
                        s->f0 = func_8000EFE8() % 20 + 20;
                    } else {
                        s->f0 = func_8000EFE8() % 30 + 30;
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
            k = (s32)(D_800E37C0[arg0].f644 - 5000.0f);
            k = D_800C2B68[s->f20].f10 + (D_800C2B68[s->f20].f14 - D_800C2B68[s->f20].f10) * k / 7000;
            k += func_8000EFE8() * 500 / 256 - 250;
            n = vol2;
            if (s->f5C != 0) {
                if (D_800E37C0[arg0].f6E8 == 0) {
                    k = 32000;
                }
                n = vol2 * 2;
            } else {
                s32 m = 300;
                k = k * m / 256;
            }
            if (D_800E37C0[arg0].f654 != 0) {
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
                k = (s32)(D_800E37C0[arg0].f644 - 1000.0f);
                k = D_800C2B68[5].f10 + (D_800C2B68[5].f14 - D_800C2B68[5].f10) * k / 7000;
                func_800B92F8(s->f4, (s32)(k * f) * 16 / 22);
            }
        }
end:
        s->f8 = v;
        v = (s32)(D_800E37C0[arg0].f420 * 255.0f / 15.0f);
        if (v > 255) {
            v = 255;
        }
        if (v < 0) {
            v = 0;
        }
        if (D_800E37C0[arg0].f4E4->f38 != 0 && v != 0 && D_800E37C0[arg0].f744 < 6) {
            if (s->f48 == -1) {
                s->f48 = func_800B7990(D_800E37C0[arg0].f4E4->f3C, 0x2B11, (vol * v) >> 8, 7, 0, 1);
            } else {
                func_800B9188(s->f48, (vol * v) >> 8);
            }
        } else if (s->f48 != -1) {
            func_800B8E98(s->f48);
            s->f48 = -1;
        }
    }
}
