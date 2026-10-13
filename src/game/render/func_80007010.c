#include "ctx.h"

typedef struct {
    u32 seg0;
    u32 seg4;
    u32 seg8;
    u8 padC[0x14 - 0xC];
    u32 tlut14;
    u8 *tlut18;
    u16 pal1C[(0x224 - 0x1C) / 2];
    Gfx *dl224;
    Gfx *dl228;
    u32 seg22C;
    u32 seg230;
    u32 seg234[2];
    Mtx *mtx23C[2];
    u8 pad244[0x448 - 0x244];
    Mtx m448[16];
    Mtx m848[8];
    u8 padA48[0xA4C - 0xA48];
    u8 *fA4C;
    u8 padA50[4];
    void *fA54;
    u8 padA58[0xA80 - 0xA58];
} Bike;

typedef struct {
    Gfx *dl0;
    Gfx *dl4;
} DlPair;

typedef struct {
    u8 pad0[4];
    u32 seg;
} SegSrc;

typedef struct {
    SegSrc *p0;
    SegSrc *p4;
} SegPair;

typedef struct {
    u8 pad0[0x44];
    u8 b44;
    u8 pad45[0x7A5 - 0x45];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Rider;

typedef struct {
    u32 w0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[8];
    s32 w3C;
    u8 pad40[0x3C0 - 0x40];
    s32 w3C0;
    s32 w3C4;
} Game;

extern s32 D_800C2594;
extern Game D_800EFD70;
extern Bike D_800DC150[];
extern Rider D_800E37C0[];
extern u32 D_800E18A0[];
extern s32 D_800C238C[];
extern u32 D_800E1600[][6];
extern u32 D_800DC0B8[];
extern DlPair D_800DC110[];
extern SegPair D_800E1900[];
extern s32 D_800E1898;
extern s32 D_800F05E4;
extern s32 D_800C9650;
extern s32 D_800C2358;
extern s32 D_800E1A98;
extern u32 D_800C2350;
extern s32 D_800E30A8;
extern u32 D_800C2280;
extern u8 *D_800C8C70;
extern Gfx *D_800E1B48;
extern s32 D_800E1A9C;

Gfx *func_80002418(Gfx *dl, s32 p2, s32 p3, s32 p4);
Gfx *func_80013D70(s32 flags, void *mdl, void *segs, Gfx *gdl);
void func_80007D90(s32 a, s32 b);

Gfx *func_80007010(Gfx *gdl, s32 vp, s32 p3, s32 xlu) {
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    u32 v;
    s32 started;
    u32 *segs;

    for (i = 0; i < D_800C2594; i++) {
        started = 0;
        for (j = 0; j < 8; j++) {
            if (D_800E37C0[j].b7A5 != 0) {
                continue;
            }
            v = D_800E18A0[j] >> (i * 8);
            if (!(v & 0xFF)) {
                continue;
            }
            k = D_800C238C[j] != 0xFF;
            if (k != xlu) {
                continue;
            }
            if (D_800EFD70.w0 & 0x10) {
                m = 2;
                if (j == D_800EFD70.f1C[i]) {
                    m = 3;
                }
            } else {
                m = v & 0xF;
                if (D_800E1898 >= 3) {
                    if (m == 3) {
                        m = 2;
                    }
                    if (i == j && m == 1) {
                        m = 2;
                    }
                }
                if (D_800E1898 == 2 && i == j) {
                    m = 3;
                }
                if (D_800F05E4 != 0 && j == 0) {
                    m = 3;
                }
                if (D_800E1898 == 1 && (j == D_800C9650 || (D_800C9650 == -1 && j == 0))) {
                    m = 3;
                }
            }
            if (D_800C2358 != 0) {
                m = 3;
                if (D_800DC150[j].dl228 == 0) {
                    continue;
                }
            }
            if (!started) {
                gdl = func_80002418(gdl, p3, i, 1);
                started = 1;
            }
            D_800E1A98++;
            if (!(D_800DC0B8[j] & 4)) {
                func_80007D90(j, i);
                if (!(D_800EFD70.w0 & 0x10)) {
                    D_800DC0B8[j] |= 4;
                }
            }
            if (D_800EFD70.w0 & 0x10) {
                if (D_800EFD70.w3C == 1) {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150[j].tlut18 + D_800EFD70.w3C0 * 0x200);
                } else {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150[j].tlut18 + i * 0x200);
                }
            } else {
                gDPLoadTLUT_pal256(gdl++, D_800DC150[j].tlut14);
            }
            if (D_800E37C0[j].b44 != 0) {
                gSPMatrix(gdl++, &D_800DC150[j].m848[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            } else {
                gSPMatrix(gdl++, &D_800DC150[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            }
            if (xlu == 0) {
                gDPSetRenderMode(gdl++, 0x442230, 0);
            } else {
                gDPSetRenderMode(gdl++, 0x404A70, 0);
                gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[j]);
            }
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
            if (D_800C2350 & 1) {
                if (m == 3) {
                    gSPSegment(gdl++, 1, D_800DC150[j].seg230 & 0xFFFFFF);
                    if (D_800EFD70.w0 & 0x10) {
                        if (D_800EFD70.w3C == 1) {
                            gSPSegment(gdl++, 2, D_800E1600[j][D_800EFD70.w3C0] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, D_800DC150[j].seg0 & 0xFFFFFF);
                        gSPSegment(gdl++, 3, D_800DC150[j].seg4 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, D_800DC150[j].seg234[D_800E30A8]);
                    if (!(D_800C2280 & 0x20)) {
                        gSPDisplayList(gdl++, D_800DC150[j].dl228);
                    }
                } else {
                    gSPSegment(gdl++, 1, D_800DC150[j].seg22C & 0xFFFFFF);
                    if (D_800EFD70.w0 & 0x10) {
                        if (D_800EFD70.w3C == 1) {
                            gSPSegment(gdl++, 2, D_800E1600[j][D_800EFD70.w3C0 % 5] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, D_800DC150[j].seg0 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, D_800DC150[j].seg234[D_800E30A8]);
                    if (!(D_800C2280 & 0x20)) {
                        gSPDisplayList(gdl++, D_800DC150[j].dl224);
                    }
                }
                if (D_800DC150[j].fA54 != 0) {
                    D_800C8C70 = D_800DC150[j].fA4C + vp * 0xC0;
                    if (!(D_800C2280 & 0x20)) {
                        if (D_800EFD70.w0 & 0x10) {
                            segs = D_800E1600[j];
                            gdl = func_80013D70(0, D_800DC150[j].fA54,
                                                (D_800EFD70.w3C == 1) ? segs + D_800EFD70.w3C0 : segs + i, gdl);
                        } else {
                            gdl = func_80013D70(0, D_800DC150[j].fA54, &D_800DC150[j], gdl);
                        }
                    }
                }
            }
            if (D_800C2358 == 0) {
                if (D_800E37C0[j].b44 != 0) {
                    gSPMatrix(gdl++, &D_800DC150[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                }
                if (D_800EFD70.w0 & 0x10) {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150[(j + D_800EFD70.w3C4) % 6].pal1C);
                } else {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150[j].pal1C);
                }
                for (k = 0; k < 6; k++) {
                    if (xlu != 1) {
                        if (k != 5 && k != 2 && m != 1) {
                            gDPSetRenderMode(gdl++, 0x442230, 0);
                        } else {
                            gDPSetRenderMode(gdl++, 0x553078, 0);
                        }
                        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                    } else {
                        gDPSetRenderMode(gdl++, 0x404A70, 0);
                        gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
                    }
                    if (D_800C2350 & 1) {
                        gSPMatrix(gdl++, D_800DC150[j].mtx23C[D_800E30A8] + k, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        if (D_800EFD70.w0 & 0x10) {
                            { Gfx *g = gdl++; g->words.w0 = 0xDB060008; if (j == 0) v = 0; gdl[-1].words.w1 = D_800DC150[(j + D_800EFD70.w3C4) & 1].seg8; }
                        } else {
                            gSPSegment(gdl++, 2, D_800DC150[j].seg8);
                        }
                        if (m == 1) {
                            gSPSegment(gdl++, 2, D_800DC150[j].seg8);
                            gSPDisplayList(gdl++, D_800E1B48);
                            break;
                        }
                        if (m == 3) {
                            gSPSegment(gdl++, 1, D_800E1900[k].p4->seg);
                            gSPDisplayList(gdl++, D_800DC110[k].dl4);
                        } else {
                            gSPSegment(gdl++, 1, D_800E1900[k].p0->seg);
                            gSPDisplayList(gdl++, D_800DC110[k].dl0);
                        }
                    }
                }
            }
            {
                s32 t;
                if (((u32)t >> 28) & 16) {
                    k = 5;
                }
            }
            gDPPipeSync(gdl++);
        }
    }
    D_800E1A9C = -1;
    return gdl;
}
