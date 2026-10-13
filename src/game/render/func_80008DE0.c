#include "ctx.h"

typedef struct {
    s32 x, y, z;
    u8 pad0C[0x44 - 0xC];
    u8 b44;
    u8 pad45[0x662 - 0x45];
    s16 h662;
    u8 pad664[0x7A5 - 0x664];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[8];
    u8 w;
    u8 h;
    u8 pad2[2];
    void *tex;
} TexInfo;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Track;

extern TexInfo *D_800DC140;
extern u8 D_800DCBB0[][0xA80];
extern s32 D_800E1558;
extern s32 D_800E1898;
extern u32 D_800E18A0[];
extern Player D_800E37C0[];
extern Vec3i D_800E8090[];
extern Track *D_800EFD64;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

Gfx *func_80002418(Gfx *dl, s32 p2, s32 p3, s32 p4);
s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);

Gfx *func_80008DE0(Gfx *gdl, s32 arg1) {
    s32 sx, sy, sz, sw;
    s32 w1, h1, s1, t1, w2, h2, s2, t2;
    s32 tw8, tw, th, i, j, y, off;
    s32 pl;
    Player *p;

    w2 = D_800F13EC / 16;
    h2 = D_800F13F0 / 15;
    s2 = 0x8000 / w2;
    t2 = 0x4000 / h2;
    tw = D_800DC140->w;
    w1 = D_800F13EC * 24 / 320;
    s1 = (tw << 13) / w1;
    th = D_800DC140->h;
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gdl++, G_TF_POINT);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    tw8 = tw * 8;
    h1 = D_800F13F0 / 10;
    t1 = (th << 10) / h1;
    for (i = 0; i < D_800E1898; i++) {
        gdl = func_80002418(gdl, arg1, i, 0);
        for (j = 0; j < 8; j++) {
            pl = (j << 11) + (j << 9) + (j << 7);
            p = &D_800E37C0[j];
            if (p->b7A5 != 0) continue;
            if (j >= 6) continue;
            if (!((D_800E18A0[j] >> (i * 8)) & 0xF)) continue;
            if (p->b44 != 0) continue;
            if (p->h662 != D_800E37C0[i].h662 - 1) continue;
            if (D_800EFD64->f2C == 0) {
                if (func_80033AC8(p->x >> 4, (p->y + 2000) >> 4, p->z >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            } else {
                if (func_80033AC8((p->x - D_800E8090[i].x) >> 4, (p->y - D_800E8090[i].y + 2000) >> 4,
                                  (p->z - D_800E8090[i].z) >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            }
            if (sx < -50) continue;
            if (sy < -50) continue;
            if (sx > D_800F13EC + 50) continue;
            if (sy > D_800F13F0 + 50) continue;
            off = -12;
            y = sy + D_800F13F0 * off / 240;
            gDPSetPrimDepth(gdl++, sz, 0);
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetTextureImage(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 1, D_800DC140->tex);
            gDPSetTile(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 7, 0, 0, 7, 0);
            gDPLoadSync(gdl++);
            gDPLoadBlock(gdl++, G_TX_LOADTILE, 0, 0, (s32)((u32)(tw8 * th) >> 2) - 1, CALC_DXT_4b(tw8));
            gDPPipeSync(gdl++);
            {
                Gfx *g = gdl++;
                g->words.w0 = ((u32)((tw8 >> 1) + 7) >> 3 << 9) | 0xF5400000;
                g->words.w1 = 0x1C070;
            }
            gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (tw8 - 1) << G_TEXTURE_IMAGE_FRAC, (th - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPLoadTLUT_pal256(gdl++, &D_800DCBB0[0][pl]);
            gSPTextureRectangle(gdl++, (sx - w1 / 2) * 4, (y - h1 / 2) * 4, (sx + w1 / 2) * 4, (y + h1 / 2) * 4,
                                0, 0, 0, s1, t1);
            gDPPipeSync(gdl++);
            off = -7;
            y = sy + D_800F13F0 * off / 240;
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            off = p->h662;
            gDPLoadTextureBlock(gdl++, D_800E1558 + (off << 9), G_IM_FMT_CI, G_IM_SIZ_8b, 32, 16, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 5, 4, 0, 0);
            gSPTextureRectangle(gdl++, (sx - w2 / 2) * 4, (y - h2 / 2) * 4, (sx + w2 / 2) * 4, (y + h2 / 2) * 4,
                                0, 0, 0, s2, t2);
        }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    return gdl;
}
