#include "ctx.h"

typedef struct Rect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} Rect;

typedef struct Plr {
    u8 pad0[0x64];
    s16 h64;
} Plr;

typedef struct Trk {
    u8 pad0[0x1C];
    s32 f1C;
    u8 pad20[0x28 - 0x20];
} Trk;

extern s32 D_800CA934;
extern s32 D_800EFDBC;
extern s32 D_800C8CE8;
extern s32 D_800E3710;
extern s32 D_800F0630;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern s32 D_800EFDAC[];
extern s16 D_800C2C60[];
extern u8 D_800F05F8;
extern s32 D_800C2594;
extern Rect D_800E1718[][4];
extern s32 D_800CCCA8;
extern s32 D_800E3768;
extern s32 *D_800F06B8[];
extern char *D_800CB0FC[];
extern char *D_800CB0B8[];
extern char *D_800CB0C0[];
extern char *D_800CB108[];
extern char *D_800CB0E8[];
extern char *D_800CB0D4[];
extern s32 D_800F0618;
extern Plr *D_800CA930;
extern Trk D_800CADE4[];

s32 func_800C2224(void);
s32 func_800595A4(void);
void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_80054ED0(Gfx *gdl, s32 a);
Gfx *func_80056C94(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_800549E4(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_800535B8(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_80053D58(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_800533D0(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_80058438(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_8005581C(Gfx *gdl, s32 p1, s32 p2, s32 p3);

#define ROWS (D_800EFDAC[0] >= 2 ? 6 : 9)
#define COLS (D_800EFDAC[0] >= 3 ? 8 : 12)
#define RECT (D_800E1718[D_800C2594 - 1][D_800F05F8])

Gfx *func_80059614(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    int i;
    int j;
    s32 a;
    s32 prev;
    s32 sp;
    int n;

    if (D_800CA934 != 3 || D_800EFDBC == 9) {
        switch (D_800EFDBC) {
        case 2:
            if (D_800E3710 == 0) {
                gdl = func_80056C94(gdl, p1, p2, p3);
            }
            break;
        case 5:
            gdl = func_800549E4(gdl, p1, p2, p3);
            break;
        case 7:
            gdl = func_800535B8(gdl, p1, p2, p3);
            break;
        case 10:
            gdl = func_80053D58(gdl, p1, p2, p3);
            break;
        case 13:
        case 14:
            gdl = func_800533D0(gdl, p1, p2, p3);
            break;
        case 9:
            gdl = func_80058438(gdl, p1, p2, p3);
            break;
        default:
            gdl = func_8005581C(gdl, p1, p2, p3);
            break;
        }
    } else if (D_800C8CE8 == 0) {
        gdl = func_80054ED0(gdl, p3);
    }
    if (D_800F0630 >= 0 || D_800F0630 == -2) {
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
        prev = 35;
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, prev);
        for (i = 0; i < ROWS; i++) {
            for (j = i & 1; j < COLS; j += 2) {
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (D_800C2C60[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 15) >> 12;
                    } else {
                        a = ((D_800C2C60[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 25) >> 14) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 35);
        prev = 35;
        for (i = 0; i < ROWS; i++) {
            for (j = ~i & 1; j < COLS; j += 2) {
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (D_800C2C60[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 255) >> 14;
                    } else {
                        a = ((D_800C2C60[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 55) >> 12) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
    }
    sp = 14;
    if (D_800CCCA8 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 1);
        sp = 28;
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                  D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
    func_800C2224();
    switch (D_800E3710) {
    case 6:
        for (n = 0; n < 3; n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
            func_800C1F2C(D_800F06B8[n], D_800CB0FC[n]);
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp + sp * n);
        }
        break;
    case 1:
        for (n = 0; n < (D_800EFDBC == 1 ? 4 : 5); n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 0x11);
            if (D_800EFDBC == 1) {
                func_800C1F2C(D_800F06B8[n], D_800CB108[n]);
            } else if (D_800EFDBC == 2) {
                func_800C1F2C(D_800F06B8[n], D_800CB0E8[n]);
            } else {
                func_800C1F2C(D_800F06B8[n], D_800CB0D4[n]);
            }
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp * 2 + sp * n);
        }
        break;
    case 5:
        if (D_800F0618 != 6) {
            s32 k;
            k = D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - sp * 2;
            for (n = 0; n < 5; n++) {
                if (n == 1 && D_800CADE4[D_800CA930->h64].f1C == 0) {
                    continue;
                }
                if (n == 3 && func_800595A4() == 0) {
                    continue;
                }
                if (n == 4 && D_800CA930->h64 == 0) {
                    continue;
                }
                func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
                func_800C1F2C(D_800F06B8[n], D_800CB0C0[n]);
                func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2, k);
                k += sp;
            }
        }
        break;
    case 4:
        for (n = 0; n < 2; n++) {
            func_800C1E60(D_800F06B8[n], 0x40, 1, n == D_800E3768 ? 4 : 2, 1);
            func_800C1F2C(D_800F06B8[n], D_800CB0B8[n]);
            func_800C0ACC(D_800F06B8[n], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + sp * n);
        }
        break;
    }
    gdl = func_800C1394(gdl);
    func_800C1D54(0x10000, 0x10000, 0, 0);
    return gdl;
}
