#include "ctx.h"

typedef struct {
    s16 f0;
    s16 pad2[3];
    s16 f8;
    s16 padA[5];
    s32 f14[6];
    s32 f2C;
} Sub;

typedef struct {
    u8 pad0[0x630];
    f32 f630;
    u8 pad634[0x660 - 0x634];
    Sub sub;
    s32 f690;
    u8 pad694[0x6E8 - 0x694];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player;

typedef struct {
    s32 f0;
    u8 pad4[0x1C];
    s16 f20;
    s16 f22;
    s16 f24;
    s16 pad26;
    s16 f28;
    s16 pad2A;
    s32 f2C;
    u8 pad30[0x10];
    s32 f40;
    u8 pad44[0xC];
    s8 f50[8];
    s32 f58;
} Game;

typedef struct {
    u8 pad0[4];
    s16 f4[6];
    s16 f10[42];
    s16 f64;
    u8 f66[5];
    u8 f6B;
    u8 f6C[8];
} Race;

typedef struct { u8 b0, b1, b2, b3; } Pts;
typedef struct { u8 pad[0x10]; s32 f10; } Mode;
typedef struct { s32 f0; u8 pad[0x48]; s32 f4C; } Cfg;

extern Game D_800F05A8;
extern Player D_800E37C0[];
extern Player *D_800F0C10[];
extern Race *D_800CA930;
extern Pts D_800CA938[];
extern Mode *D_800C29CC;
extern Cfg D_800EFD70;
extern s32 D_800C8CD8, D_800E3700, D_800C8CE0, D_800CA920, D_800F05E8;
extern s32 D_800F0520, D_800E76D0, D_800C8CE8, D_800E36F8, D_800E374C, D_800CCD64;
extern s32 D_800E30B0[4];
extern s32 D_800E25B0[4];
extern s16 D_800F05D0;
extern f32 D_800EE48C;

void func_8001102C(s32 param_1);
void func_800515A8(void);

void func_800521F4(void) {
    s32 i, j, k, m;
    s32 cnt, n1;
    s32 pad[8];
    u8 buf[4];
    f32 f;
    s32 s, t;
    u8 tmp;
    Player *p;
    Sub *q;

    func_800515A8();
    n1 = 0;
    cnt = n1;
    if (!(D_800F05A8.f58 & 0x10)) {
        D_800F05A8.f2C++;
    }
    *(s32 *)buf = 0;
    for (m = 0; m < D_800F05A8.f22; m++) {
        q = &D_800E37C0[m].sub;
        if ((D_800E37C0[m].f690 & 8) || (D_800C8CD8 != 0 && D_800E3700 == m) || D_800E37C0[m].f6E8 == 0) {
            cnt++;
        }
        if (D_800E37C0[m].sub.f8 == -1 && m < cnt) {
            n1++;
        }
        if (q->f8 != -1 || D_800E37C0[m].f6E8 != 0) {
            buf[m] = 1;
        }
        if (D_800F05A8.f0 == 5) {
            s32 k = q->f0;
            if (k < D_800F05A8.f20) {
                q->f14[k]++;
            }
        }
    }
    for (m = 0; m < cnt; m++) {
        if (!buf[m]) break;
    }
    if (!((m == cnt && n1 == 0 && D_800EFD70.f4C != 8) || (D_800EFD70.f4C == 8 && buf[D_800C8CE0]))) return;
    if (D_800EFD70.f0 & 0x81) return;
    if (D_800CA920 == 3) return;
    if (D_800F05A8.f40 == 0) {
        D_800F05A8.f28 = 180;
    } else if (D_800F05A8.f28 != 0) {
        D_800F05A8.f28--;
    }
    D_800F05E8 = 1;
    D_800CA920 = 2;
    if (D_800F0520 == -1) {
        D_800F0520 = D_800E76D0;
    }
    if (D_800C8CE8 == 0 && D_800E36F8 == 5) {
        if ((D_800E30B0[0] & 0x40) && !(D_800E25B0[0] & 0x40)) goto go;
        if ((D_800E30B0[1] & 0x40) && !(D_800E25B0[1] & 0x40)) goto go;
        if ((D_800E30B0[2] & 0x40) && !(D_800E25B0[2] & 0x40)) goto go;
        if ((D_800E30B0[3] & 0x40) && !(D_800E25B0[3] & 0x40)) goto go;
        if (D_800CCD64 != 0 && (D_800EFD70.f0 & 0x01000000) && (D_800E30B0[0] & 0x20) && !(D_800E25B0[0] & 0x20)) goto go;
        if (D_800F05D0 != 0 && D_800CCD64 == 0) goto end;
        if ((D_800E30B0[0] & 0x10) && !(D_800E25B0[0] & 0x10)) goto go;
        if ((D_800E30B0[1] & 0x10) && !(D_800E25B0[1] & 0x10)) goto go;
        if ((D_800E30B0[2] & 0x10) && !(D_800E25B0[2] & 0x10)) goto go;
        if (!(D_800E30B0[3] & 0x10) || (D_800E25B0[3] & 0x10)) goto end;
    go:
        if (D_800CCD64 != 0) {
            func_8001102C(2);
        }
        if (D_800E36F8 == 5) {
            D_800E36F8 = 6;
            D_800E374C = 20;
        }
        for (k = 0; k < D_800F05A8.f22; k++) {
            if (D_800E37C0[k].sub.f8 == -1) {
                f = D_800E37C0[k].f630 / (D_800EE48C * D_800F05A8.f20);
                if (f <= 0.1f) {
                    f = 0.1f;
                } else if (f > 1.0f) {
                    f = 1.0f;
                }
                f = D_800F05A8.f2C / f;
                D_800E37C0[k].sub.f2C = f;
            }
        }
        for (k = 0; k < D_800F05A8.f22; k++) {
            if (D_800E37C0[k].sub.f8 == -1) {
                D_800E37C0[k].sub.f8 = 0;
                for (j = 0; j < D_800F05A8.f22; j++) {
                    if ((j != k && k < j && D_800E37C0[j].sub.f2C < D_800E37C0[k].sub.f2C) ||
                        (j < k && D_800E37C0[j].sub.f2C <= D_800E37C0[k].sub.f2C)) {
                        D_800E37C0[k].sub.f8++;
                    }
                }
                if (D_800EFD70.f4C == 1) {
                    D_800CA930->f66[k] += D_800CA938[D_800E37C0[k].sub.f8].b3;
                }
            }
            D_800F05A8.f50[D_800E37C0[k].sub.f8] = k;
        }
        D_800F05A8.f24 = D_800F05A8.f22;
        if (D_800EFD70.f4C == 1) {
            for (i = 0; i < D_800F05A8.f22; i++) {
                D_800CA930->f4[i] += D_800CA930->f66[i];
                D_800CA930->f6C[i] = i;
            }
            for (i = 0; i < D_800F05A8.f22; i++) {
                j = i;
                if (i != 0) {
                    s = D_800CA930->f4[D_800CA930->f6C[i - 1]];
                }
                t = D_800CA930->f4[D_800CA930->f6C[i]];
                while (j != 0 && s < t) {
                    tmp = D_800CA930->f6C[j];
                    D_800CA930->f6C[j] = D_800CA930->f6C[j - 1];
                    D_800CA930->f6C[j - 1] = tmp;
                    j--;
                    if (j != 0) {
                        s = D_800CA930->f4[D_800CA930->f6C[j - 1]];
                    }
                }
            }
        }
    }
end:
    if (D_800C29CC->f10 == 3) {
        s = -1;
        for (i = D_800F05A8.f24; i < D_800F05A8.f22; i++) {
            q = &D_800F0C10[i]->sub;
            if (q->f8 == s) {
                D_800F05A8.f50[D_800F05A8.f24] = i;
                q->f8 = D_800F05A8.f24++;
                q->f2C = (D_800F05A8.f2C * D_800F0C10[0]->f630) / D_800E37C0[i].f630;
                if (D_800EFD70.f4C == 1 + (((u32)&D_800E37C0 & 1) >> 1)) {
                    D_800CA930->f10[(D_800F0C10[i] - D_800E37C0) * 7 + D_800CA930->f64] = q->f8;
                    D_800CA930->f66[D_800F0C10[i] - D_800E37C0] = ((Pts *)((u32)D_800CA938 | (((u32)D_800CA938 & 1) >> 1)))[q->f8].b3;
                }
            }
        }
    }
}
