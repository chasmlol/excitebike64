#include "ctx.h"

typedef struct {
    u8 pad0[0x18];
    u32 w18;
} Txt;

typedef struct {
    Txt *p0;
    Txt *p4;
    Txt *p8;
    Txt *pC;
    Txt *p10;
    Txt *p14;
    Txt *p18;
    Txt *p1C;
    Txt *list[8];
} Hud;

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    u8 pad0[0x18];
    char *name;
    u8 pad1C[0x3C - 0x1C];
} Trick;

typedef struct {
    u8 pad0[0x3C4];
    s32 w3C4;
    s32 w3C8;
    u8 pad3CC[4];
    u8 b3D0;
    u8 pad3D1;
    s8 s3D2;
    u8 pad3D3[0x3EA - 0x3D3];
    u8 b3EA;
    s8 s3EB[0x3FB - 0x3EB];
    u8 b3FB;
    u8 pad3FC[3];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x668 - 0x424];
    s16 h668;
    u8 pad66A[0x7E0 - 0x66A];
} Player;

typedef struct {
    s32 w0;
    u8 pad4[0x22];
    s16 h26;
} Mode;

extern Txt *D_800F0530[];
extern volatile s32 D_800CCCA8;
extern Player D_800E37C0[];
extern s32 D_800F05D4;
extern Rect D_800E1718[][4];
extern s32 D_800E171C;
extern s32 D_800E1898;
extern Mode D_800F05A8;
extern s32 D_800F05E8;
extern s32 D_800EFDAC;
extern volatile s32 D_800F13EC;
extern Trick D_800C9A40[];
extern char D_800D84E0[];
extern char D_800D84E4[];
extern u8 D_800E3BBF;

s32 func_800C2224(void);
void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
void func_8000EE30(s32 v, char *out);
Txt *func_800C1E60(Txt *p, s32 a, s32 b, s32 c, s32 d);
char *func_800C1F2C(Txt *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
void func_80052F30(s32 param_1, char *param_2);
u8 *func_800C0ACC(Txt *p, s32 x, s32 y);
Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_80054ED0(Gfx *gdl, s32 pl);

Gfx *func_80053D58(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    Player *p = &D_800E37C0[pl];
    char buf[40];
    s32 idx, t;
    s32 x, y, ty, yt, yy;
    s32 x1, y1, xa;
    s32 x3, y3, ax, ay;
    s32 k, i;
    s32 by, dy;
    u8 c;
    s8 d;
    f32 v;

    func_800C2224();
    if (D_800EFDAC >= 2) {
        if (D_800F05E8 != 0) {
            func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
            switch (p->h668) {
            case 0:
                func_800C1F2C(D_800F0530[7], "Winner");
                break;
            case 1:
                func_800C1F2C(D_800F0530[7], D_800D84E0);
                break;
            default:
                func_800C1F2C(D_800F0530[7], D_800D84E4);
                break;
            }
            func_800C0ACC(D_800F0530[7],
                          D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w / 2,
                          D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h / 2);
        }
        yt = D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h - 11;
        func_800C1E60(D_800F0530[0], 0x40, 2, 2, 0x20);
        func_800C1E60(D_800F0530[1], 0x40, 2, 4, 0x20);
        idx = -1;
        do {
        } while (0);
        func_80052F30(D_800F05D4, buf);
        func_800C1F2C(D_800F0530[0], "Time:");
        func_800C0ACC(D_800F0530[0], 0x3C, yt);
        func_800C1F2C(D_800F0530[1], buf);
        func_800C0ACC(D_800F0530[1], 100, yt);
        xa = D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w - 20;
        yy = D_800E1718[D_800E1898 - 1][pl].y;
        func_800C1E60(D_800F0530[4], 0x40, 2, 2, 0x22);
        func_800C1F2C(D_800F0530[4], "Total");
        ty = yy + 8;
        func_800C0ACC(D_800F0530[4], xa, ty);
        func_800C1E60(D_800F0530[5], 0x40, 2, 4, 0x22);
        func_8000EE30(p->w3C8, buf);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], xa, yy + 16);
        c = p->b3D0;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            func_800C1E60(D_800F0530[2], 0x40, 2, 2, 0x21);
            x3 = 0x100;
            func_800C1F2C(D_800F0530[2], D_800C9A40[idx].name);
            y3 = ty + 8;
            func_800C1E60(D_800F0530[3], 0x40, 2, 4, 0x21);
            func_8000EE30(p->w3C4, buf);
            func_800C1F2C(D_800F0530[3], buf);
            ax = 0x100;
            ay = ty;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (xa - (D_800F0530[3]->w18 >> 1)) * (256 - k)) >> 8;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((D_800F0530[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (ty + i * 8) * (256 - k)) / 256;
                }
            }
            func_800C0ACC(D_800F0530[2], ax, ay);
            func_800C0ACC(D_800F0530[3], x3, y3);
        }
        for (i = 0; i < p->b3EA; i++) {
            func_800C1E60(D_800F0530[i + 8], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530[i + 8], D_800C9A40[p->s3EB[i]].name);
            func_800C0ACC(D_800F0530[i + 8], 0x3C, ty + i * 8);
        }
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        v = p->f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530[6], buf);
        func_800C0ACC(D_800F0530[6],
                      D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w - 0x2D,
                      D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h - 0xB);
        gdl = func_8005A4C8(gdl, a1, a2, pl);
    } else {
        if (D_800CCCA8 == 2) {
            func_800C1D54(0x10000, 0x20000, 0, 0);
        }
        func_800C1E60(D_800F0530[0], 0x40, 1, 2, 0x10);
        func_800C1E60(D_800F0530[1], 0x40, 1, 4, 0x10);
        func_80052F30(D_800F05D4, buf);
        func_800C1F2C(D_800F0530[0], "Time:");
        func_800C0ACC(D_800F0530[0], 0x23, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        func_800C1F2C(D_800F0530[1], buf);
        func_800C0ACC(D_800F0530[1], 100, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        y = 0x19;
        x = D_800F13EC - 62;
        if (D_800CCCA8 == 2) {
            y = 0x69;
        }
        func_800C1E60(D_800F0530[4], 0x40, 1, 2, 0x22);
        func_800C1F2C(D_800F0530[4], "Total");
        func_800C0ACC(D_800F0530[4], x, y);
        if (D_800CCCA8 == 2) {
            y += 0x19;
        } else {
            y += 0xC;
        }
        func_800C1E60(D_800F0530[5], 0x40, 1, 4, 0x22);
        func_8000EE30(p->w3C8, buf);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], x, y);
        if (D_800CCCA8 == 2) {
            by = 0x69;
            dy = 0x14;
        } else {
            by = 0x5A;
            dy = 0xA;
        }
        c = p->b3D0;
        idx = -1;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            func_800C1E60(D_800F0530[2], 0x40, 1, 2, 0x21);
            func_800C1F2C(D_800F0530[2], D_800C9A40[idx].name);
            func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x21);
            func_8000EE30(p->w3C4, buf);
            func_800C1F2C(D_800F0530[3], buf);
            x3 = D_800F13EC / 2;
            y3 = D_800E171C + 0x28;
            ax = D_800F13EC / 2;
            ay = D_800E171C + 0x14;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (x - (D_800F0530[3]->w18 >> 1)) * (256 - k)) >> 8;
                y3 = (y3 * k + y * (256 - k)) / 256;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((D_800F0530[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (by + i * dy) * (256 - k)) / 256;
                }
            }
            func_800C0ACC(D_800F0530[2], ax, ay);
            func_800C0ACC(D_800F0530[3], x3, y3);
        }
        v = D_800E37C0[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        func_800C1F2C(D_800F0530[6], buf);
        x1 = 0x1BA;
        if (D_800CCCA8 == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (D_800CCCA8 == 2) {
            y1 = 0x180;
        }
        func_800C0ACC(D_800F0530[6], x1, y1);
        for (i = 0; i < p->b3EA; i++) {
            func_800C1E60(D_800F0530[i + 8], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530[i + 8], D_800C9A40[p->s3EB[i]].name);
            func_800C0ACC(D_800F0530[i + 8], 0x23, by + i * dy);
        }
        if (D_800F05E8 != 0) {
            func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
            func_800C1F2C(D_800F0530[7], D_800E3BBF >= 3 ? "Press START to Continue" : "Game Over");
            func_800C0ACC(D_800F0530[7],
                          D_800E1718[D_800E1898 - 1][pl].x + D_800E1718[D_800E1898 - 1][pl].w / 2,
                          D_800E1718[D_800E1898 - 1][pl].y + D_800E1718[D_800E1898 - 1][pl].h / 2);
        }
    }
    gdl = func_800C1394(gdl);
    gdl = func_80009754(gdl, a1, a2, pl);
    if (D_800F05A8.h26 != 0 && D_800F05A8.w0 != 0 && (D_800F05A8.w0 != 1 || D_800F05A8.h26 < 0x5B)) {
        gdl = func_80054ED0(gdl, pl);
    }
    return gdl;
}
