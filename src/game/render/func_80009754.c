#include "ctx.h"

typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 b654;
    u8 pad655[0x7E0 - 0x655];
} Player;

typedef struct {
    u8 pad0[0xC];
    u8 *tC;
    u8 pad10[4];
    u8 *t14;
    u8 pad18[4];
    u8 *t1C;
} TexSet;

extern s32 D_800EFDBC;
extern s32 D_800C8CE0;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern Player D_800E37C0[];
extern u16 *D_800DC0A4;
extern TexSet *D_800DC0A0;
extern s32 D_800E1A50;
extern s32 D_800E1A54;
extern s32 D_800E76D0;
extern s16 D_800C2C60[];
extern Vtx *D_800DC0A8;
extern s32 D_800DC0D8;
extern s32 D_800DC0DC;
extern s32 D_800C23D0[];
extern Mtx D_800C2678;
extern Vp D_800C25E8;

Gfx *func_80002418(Gfx *dl, s32 p2, s32 p3, s32 p4);

Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 p;
    s32 sx, sy;
    s32 lvl, k;
    s32 i;
    u16 *pal;
    s32 x, y;

    if (D_800EFDBC == 8) {
        p = D_800C8CE0;
    } else {
        p = pl;
    }
    sx = 0x80000 / D_800F13EC;
    sy = 0x3C000 / D_800F13F0;
    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, 0x0C087008, 0);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0xFE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gdl = func_80002418(gdl, a1, 0, 1);
    gSPClipRatio(gdl++, FRUSTRATIO_6);

    lvl = D_800E37C0[p].f648 * 256.0f / 12000.0f;
    if (lvl < 0) {
        lvl = 0;
    }
    if (lvl > 255) {
        lvl = 255;
    }
    pal = D_800DC0A4 + a2 * 256;
    k = lvl * 87 / 256;
    k += 148;
    for (i = 148; i < 192; i++) {
        if (k < i) {
            pal[i] = 0x201;
        } else {
            pal[i] = 0x7C1;
        }
    }
    for (i = 192; i < 224; i++) {
        if (k < i) {
            pal[i] = 0x4201;
        } else {
            pal[i] = 0xFFC1;
        }
    }
    for (i = 224; i < 235; i++) {
        if (k < i) {
            pal[i] = 0x4001;
        } else {
            pal[i] = 0xF801;
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, pal);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, D_800DC0A0->t14 + i * 80, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800E1A50 * 4, (D_800E1A54 + i * D_800F13F0 / 240) * 4,
                            (D_800E1A50 + D_800F13EC * 80 / 512) * 4,
                            (D_800E1A54 + (i + 25) * D_800F13F0 / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetBlendColor(gdl++, 0, 0, 0, 8);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    if (D_800E37C0[p].b654 != 0) {
        gDPSetCombineLERP(gdl++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPLoadTextureBlock(gdl++, D_800DC0A0->tC, G_IM_FMT_IA, G_IM_SIZ_8b, 48, 16, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        k = (-D_800E76D0 * 8) & 0xFF;
        gDPSetPrimColor(gdl++, 0, 0, 255, 0, 0, k);
        gSPTextureRectangle(gdl++, (D_800E1A50 + D_800F13EC / 32) * 4, (D_800E1A54 + D_800F13F0 / 24) * 4,
                            (D_800E1A50 + D_800F13EC / 8) * 4, (D_800E1A54 + D_800F13F0 * 23 / 240) * 4,
                            0, 0, 0, sx, sy);
        gDPPipeSync(gdl++);
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, D_800DC0A0->t1C + i * 80, G_IM_FMT_IA, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800E1A50 * 4, (D_800E1A54 + i * D_800F13F0 / 240) * 4,
                            (D_800E1A50 + D_800F13EC * 80 / 512) * 4,
                            (D_800E1A54 + (i + 25) * D_800F13F0 / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
    gSPMatrix(gdl++, a1 + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPViewport(gdl++, &D_800C25E8);
    k = 55000 - lvl * 175;
    for (i = 0; i < 4; i++) {
        s32 x0 = D_800C23D0[i * 2];
        s32 y0 = D_800C23D0[i * 2 + 1];
        x = (x0 * D_800C2C60[(k >> 4) & 0xFFF] >> 14) + (y0 * D_800C2C60[((k >> 4) - 0x400) & 0xFFF] >> 14);
        y = (y0 * D_800C2C60[(k >> 4) & 0xFFF] >> 14) - (x0 * D_800C2C60[((k >> 4) - 0x400) & 0xFFF] >> 14);
        x = x * D_800F13EC / 320;
        y = y * D_800F13F0 / 240;
        x += D_800E1A50 + D_800DC0D8;
        y += D_800E1A54 + D_800DC0DC;
        x -= D_800F13EC / 2;
        y = D_800F13F0 / 2 - y;
        D_800DC0A8[a2 * 8 + i].v.ob[0] = x;
        D_800DC0A8[a2 * 8 + i].v.ob[1] = y;
        D_800DC0A8[a2 * 8 + i].v.ob[2] = 0x200;
        D_800DC0A8[a2 * 8 + i + 4].v.ob[0] = x + 4;
        D_800DC0A8[a2 * 8 + i + 4].v.ob[1] = y - 2;
        D_800DC0A8[a2 * 8 + i + 4].v.ob[2] = 0x200;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPVertex(gdl++, &D_800DC0A8[a2 * 8], 8, 0);
    gSP2Triangles(gdl++, 7, 4, 5, 0, 4, 5, 6, 0);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0x8C, 0x8C, 0x8C, 0xFF);
    gSP2Triangles(gdl++, 3, 0, 1, 0, 3, 1, 2, 0);
    gSPMatrix(gdl++, a1, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    return gdl;
}
