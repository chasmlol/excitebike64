#include "ctx.h"

typedef struct {
    u8 pad0[4];
    s32 w4;
    u8 pad8[0x18 - 0x8];
    u32 w18;
} Txt;

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    s16 h0;
    union { s16 h; u8 b[2]; } u2;
    u8 pad4[4];
    s16 h8;
    s16 hA;
    s16 hC;
    u8 padE[2];
    s32 w10;
    s32 w14[6];
    s32 w2C;
} Lap;

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x420 - 0x54];
    f32 f420;
    u8 pad424[0x660 - 0x424];
    Lap lap;
    u8 pad690[0x6EE - 0x690];
    u8 b6EE;
    u8 pad6EF[0x7AC - 0x6EF];
    s16 h7AC;
    u8 pad7AE[0x7E0 - 0x7AE];
} Player;

typedef struct {
    s32 w0;
    u8 pad4[0x10 - 0x4];
    s32 w10;
    u8 pad14[0x20 - 0x14];
    s16 h20;
    s16 h22;
    u8 pad24[2];
    s16 h26;
    u8 pad28[4];
    s32 w2C;
    u8 pad30[0x40 - 0x30];
    s32 w40;
} Mode;

typedef struct {
    u32 w0;
    u8 pad4[0x4C - 4];
    s32 w4C;
} Game;

typedef struct {
    u8 pad0[0x50];
    s32 w50;
} Track;

extern Txt *D_800F0530[];
extern Txt *D_800F06B8[];
extern volatile s32 D_800CCCA8;
extern Player D_800E37C0[];
extern Rect D_800E1718[][4];
extern s32 D_800E1898;
extern s32 D_800C2598;
extern s32 D_800C9650;
extern s32 D_800C8CE0;
extern s32 D_800C8CE8;
extern s32 D_800CA934;
extern Mode D_800F05A8;
extern Game D_800EFD70;
extern Track *D_800EFD64;
extern s32 D_800E2060;
extern s32 D_800E3710;
extern s32 D_800E76D0;
extern volatile s32 D_800F13EC;

s32 func_800C2224(void);
void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
void func_8000EE30(s32 v, char *out);
void func_8000ED30(char *dst, char *src);
Txt *func_800C1E60(Txt *p, s32 a, s32 b, s32 c, s32 d);
char *func_800C1F2C(Txt *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
void func_80052F30(s32 v, char *s);
void func_8005313C(s32 v, char *s);
void func_800532D4(Txt *p, s32 n, s32 pl);
void func_8005CD30(char *p, f32 x, s32 n);
void func_800C0ACC(Txt *p, s32 x, s32 y);
s32 func_800AEC10(char *s);
Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_80054ED0(Gfx *gdl, s32 pl);
Gfx *func_80055658(Gfx *gdl, s32 pl, Txt *t);

#define VP D_800E1718[D_800E1898 - 1][D_800C2598]

Gfx *func_8005581C(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[88];
    char *p;
    Lap *lp;
    Txt *tRace;
    Txt *tLap;
    Txt *tBest;
    Txt *tPlace;
    Txt *tRaceL;
    Txt *tLapL;
    Txt *tBestL;
    Txt *tSpeed;
    Txt *tMsg;
    Txt *t;
    s32 v;
    f32 f;
    s32 x, y;
    s32 xa, xb, yy, dy;

    lp = &D_800E37C0[(D_800C9650 == -1) ? pl + D_800C8CE0 : D_800C9650].lap;
    if (D_800E1898 == 1 && D_800C9650 != -1) {
        pl = D_800C9650;
    }
    func_800C2224();
    if (D_800CCCA8 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 0);
    }
    if (D_800E1898 < 3) {
        tPlace = func_800C1E60(D_800F0530[0], 0x20, 3, 2, 0x21);
        if (D_800E1898 == 1) {
            tSpeed = func_800C1E60(D_800F0530[1], 0x20, 3, 2, 0x12);
        } else {
            tSpeed = func_800C1E60(D_800F0530[1], 0x20, 3, 2, 0x11);
        }
        tRace = func_800C1E60(D_800F0530[2], 0x20, 2, 2, 0x10);
        tBest = func_800C1E60(D_800F0530[3], 0x20, 2, 2, 0x10);
        tRaceL = func_800C1E60(D_800F0530[4], 0x20, 2, 2, 0x10);
        tBestL = func_800C1E60(D_800F0530[5], 0x20, 2, 2, 0x10);
        if (D_800F05A8.h26 != 0 && D_800F05A8.w0 == 5) {
            tMsg = func_800C1E60(D_800F0530[6], 0x20, 1, 2, 0x21);
        } else {
            tMsg = func_800C1E60(D_800F0530[6], 0x20, 1, 4, 0x21);
        }
        if (lp->hA != 0 || lp->h0 == D_800F05A8.h20) {
            if (lp->hA != 0) {
                lp->hA--;
            }
            tLap = func_800C1E60(D_800F0530[7], 0x20, 2, 4, 0x10);
            tLapL = func_800C1E60(D_800F0530[8], 0x20, 2, 4, 0x10);
        } else {
            tLap = func_800C1E60(D_800F0530[7], 0x20, 2, 2, 0x10);
            tLapL = func_800C1E60(D_800F0530[8], 0x20, 2, 2, 0x10);
        }
        if (D_800EFD70.w0 & 0x20000000) {
            Txt *t1, *t2, *t3;
            t1 = func_800C1E60(D_800F0530[9], 0x20, 2, 2, 0x10);
            func_8005CD30(buf, D_800E37C0[0].f48, 5);
            func_800C1F2C(t1, buf);
            func_800C0ACC(t1, 0x28, 0x1E);
            t2 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
            func_8005CD30(buf, D_800E37C0[0].f50, 5);
            func_800C1F2C(t2, buf);
            func_800C0ACC(t2, 0x28, 0x28);
            t3 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
            func_8000EE30(D_800E2060, buf);
            func_800C1F2C(t3, buf);
            func_800C0ACC(t3, 0x28, 0x32);
        }
        if (D_800EFD70.w4C == 6 && D_800F05A8.w2C == 0) {
            t = func_800C1E60(D_800F0530[19], 0x20, 2, 2, 0x11);
            func_800C1F2C(t, "Press Up C Button for side view");
            func_800C0ACC(t, 0x100, 0x28);
        }
    } else {
        tPlace = func_800C1E60(D_800F0530[0], 8, 2, 2, 0x12);
        tSpeed = func_800C1E60(D_800F0530[1], 0x20, 2, 2, 0x12);
        tRace = func_800C1E60(D_800F0530[2], 0x20, 2, 2, 0x10);
        tBest = func_800C1E60(D_800F0530[3], 0x20, 2, 2, 0x10);
        tRaceL = func_800C1E60(D_800F0530[4], 0x20, 2, 2, 0x10);
        tBestL = func_800C1E60(D_800F0530[5], 0x20, 2, 2, 0x10);
        tLap = func_800C1E60(D_800F0530[6], 0x20, 2, 4, 0x10);
        tLapL = func_800C1E60(D_800F0530[7], 0x20, 2, 4, 0x10);
        if (D_800F05A8.h26 != 0 && D_800F05A8.w0 == 5) {
            tMsg = func_800C1E60(D_800F0530[8], 0x20, 1, 2, 0x21);
        } else {
            tMsg = func_800C1E60(D_800F0530[8], 0x20, 1, 4, 0x21);
        }
    }
    if (lp->h8 != -1) {
        func_800532D4(tMsg, lp->h8, pl);
    }
    if (D_800C8CE8 == 0) {
        if (D_800F05A8.h26 != 0 && D_800F05A8.w0 != 0 && (D_800F05A8.w0 != 1 || D_800F05A8.h26 < 0x5B)) {
            gdl = func_80054ED0(gdl, pl);
        } else if (D_800E3710 == 0 && lp->h8 == -1) {
            if (D_800E37C0[pl].lap.hC != 0) {
                v = D_800E37C0[pl].lap.w10;
                D_800E37C0[pl].lap.hC--;
                if (v < 0) {
                    v = -v;
                    buf[0] = '+';
                    tMsg->w4 = 8;
                } else if (v > 0) {
                    buf[0] = '-';
                    tMsg->w4 = 7;
                }
                buf[1] = 0;
                func_8005313C(v, buf + 1);
                func_800C1F2C(tMsg, buf);
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 4);
            } else if (((D_800E37C0[pl].h7AC - 1) & 0x20) == 0) {
                func_800C1F2C(tMsg, "Off Course");
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            } else if (D_800E37C0[pl].b6EE != 0 && (D_800E76D0 & 0x20)) {
                func_800C1F2C(tMsg, "Wrong Way");
                func_800C0ACC(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            }
        }
        switch (D_800E1898) {
        case 1:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPPipeSync(gdl++);
            if (D_800F05A8.h22 >= 2) {
                gdl = func_80055658(gdl, pl, tPlace);
            }
            f = D_800E37C0[pl].f420;
            func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            func_800C1F2C(tSpeed, buf);
            x = 0x1BA;
            if (D_800CCCA8 == 2) {
                x = 0x228;
            }
            y = 0xD6;
            if (D_800CCCA8 == 2) {
                y = 0x180;
            }
            func_800C0ACC(tSpeed, x, y);
            if (lp->h0 == D_800F05A8.h20) {
                func_80052F30(lp->w2C, buf);
            } else {
                func_80052F30(D_800F05A8.w2C, buf);
            }
            if (D_800CA934 < 2) {
                func_800C1F2C(tRace, buf);
                func_800C1F2C(tRaceL, "Race: ");
                xa = VP.x + 0x43;
                xb = VP.x + 0x14;
                if (D_800CCCA8 != 2) {
                    yy = VP.y + VP.h - 0x1C;
                    dy = 10;
                } else {
                    yy = VP.y + VP.h - 0x38;
                    dy = 0x14;
                }
                if (D_800EFD64->w50 & 2) {
                    yy += dy;
                }
                func_800C0ACC(tRaceL, xb, yy);
                func_800C0ACC(tRace, xa, yy);
                yy += dy;
                __builtin_strcpy(buf, "Lap ");
                p = buf;
                if (lp->hA != 0 || lp->h0 == D_800F05A8.h20) {
                    func_8000EE30(lp->h0, p + func_800AEC10(p));
                } else {
                    func_8000EE30(lp->h0 + 1, p + func_800AEC10(p));
                }
                func_8000ED30(p, ": ");
                func_800C1F2C(tLapL, buf);
                func_80052F30(lp->w14[(lp->hA != 0 || lp->h0 == D_800F05A8.h20) ? lp->h0 - 1 : lp->h0], buf);
                func_800C1F2C(tLap, buf);
                func_800C0ACC(tLapL, xb, yy);
                func_800C0ACC(tLap, xa, yy);
                yy += dy;
                if (!(D_800EFD64->w50 & 2)) {
                    func_80052F30(D_800F05A8.w10, buf);
                    func_800C1F2C(tBestL, "Best: ");
                    func_800C1F2C(tBest, buf);
                    func_800C0ACC(tBestL, xb, yy);
                    func_800C0ACC(tBest, xa, yy);
                }
            }
            gdl = func_800C1394(gdl);
            gDPPipeSync(gdl++);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gdl = func_80009754(gdl, a1, a2, D_800C2598);
            break;
        case 2:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            gdl = func_80055658(gdl, pl, tPlace);
            if (D_800CA934 < 2) {
                f = D_800E37C0[pl].f420;
                func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
                func_800C1F2C(tSpeed, buf);
                func_800C0ACC(tSpeed, VP.x + VP.w - 0x2D, VP.y + VP.h - 0xB);
                if (lp->h0 == D_800F05A8.h20) {
                    func_80052F30(lp->w2C, buf);
                } else {
                    func_80052F30(D_800F05A8.w2C, buf);
                }
                func_800C1F2C(tRace, buf);
                func_800C1F2C(tRaceL, "Race: ");
                func_800C0ACC(tRaceL, VP.x + 10, VP.y + VP.h - 0x12);
                func_800C0ACC(tRace, VP.x + 0x43, VP.y + VP.h - 0x12);
            }
            __builtin_strcpy(buf, "Lap ");
            p = buf;
            if (lp->hA != 0 || lp->h0 == D_800F05A8.h20) {
                func_8000EE30(lp->h0, p + func_800AEC10(p));
            } else {
                func_8000EE30(lp->h0 + 1, p + func_800AEC10(p));
            }
            func_8000ED30(p, ": ");
            func_800C1F2C(tLapL, buf);
            func_800C0ACC(tLapL, VP.x + 10, VP.y + VP.h - 8);
            func_80052F30(lp->w14[(lp->hA != 0 || lp->h0 == D_800F05A8.h20) ? lp->h0 - 1 : lp->h0], buf);
            func_800C1F2C(tLap, buf);
            func_800C0ACC(tLap, VP.x + 0x43, VP.y + VP.h - 8);
            gdl = func_8005A4C8(func_800C1394(gdl), a1, a2, pl);
            break;
        default:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            buf[0] = D_800E37C0[pl].lap.u2.b[1] + '1';
            buf[1] = (D_800E37C0[pl].lap.u2.h < 3) ? D_800E37C0[pl].lap.u2.b[1] + '{' : '~';
            buf[2] = 0;
            func_800C1F2C(tPlace, buf);
            func_800C0ACC(tPlace, VP.x + VP.w - 10, VP.y + 13);
            f = D_800E37C0[pl].f420;
            func_8000EE30((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            func_800C1F2C(tSpeed, buf);
            func_800C1F2C(tSpeed, "mph");
            func_800C0ACC(tSpeed, VP.x + VP.w - 10, VP.y + VP.h - 6);
            gdl = func_8005A4C8(func_800C1394(gdl), a1, a2, pl);
            break;
        }
    } else {
        gdl = func_800C1394(gdl);
    }
    if (D_800F05A8.w40 != 0 && D_800C2598 == D_800E1898 - 1) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                      D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
        func_800C1E60(D_800F06B8[0], 0x40, 1, 2, 0x21);
        func_800C1E60(D_800F06B8[1], 0x40, 1, 2, 0x21);
        func_800C1F2C(D_800F06B8[0], "Press START to Continue");
        if (D_800E1898 == 1) {
            func_800C0ACC(D_800F06B8[0], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + 0x17);
        } else {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xA0);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
            gDPFillRectangle(gdl++, D_800F13EC * 0, D_800E1718[0][0].y + D_800E1718[0][0].h / 2 - 7,
                             D_800F13EC - 1, D_800E1718[0][0].y + D_800E1718[0][0].h / 2 + 7);
            func_800C0ACC(D_800F06B8[0], D_800E1718[0][0].x + D_800E1718[0][0].w / 2,
                          D_800E1718[0][0].y + D_800E1718[0][0].h / 2);
        }
        gdl = func_800C1394(gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}
