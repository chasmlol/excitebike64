#include "ctx.h"

typedef struct { s32 x, y, z, pad[3]; s32 on; } Star;
typedef struct { u8 pad[8]; u8 w, h; u8 pad2[2]; void *img2; u8 pad3[4]; void *img; } TexInfo;
typedef struct { u16 a, b; } Scale;

extern Star *D_800CCFFC;
extern s32 D_800CCFF8;
extern s16 D_800C2C60[];
extern Scale D_800CD000[];
extern Vtx D_800CD018[];
extern s32 D_800E76D0;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern TexInfo *D_800F219C;

Gfx *func_80071A7C(Gfx *gdl, s32 mtx)
{
    Star *p;
    s32 i, cx, cy, s, x, y, z, a, r;

    p = D_800CCFFC;
    if (p == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gSPSetOtherMode(gdl++, G_SETOTHERMODE_L, G_MDSFT_RENDERMODE, 29, 0x404240);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2DFEFF);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    { Gfx *g = gdl++; g->words.w0 = 0xE3000C00; g->words.w1 = ((u32)g & 1) >> 1; }
    { Gfx *g = gdl++; g->words.w0 = 0xE3001001; g->words.w1 = ((u32)g & 1) >> 1; }
    {
        s32 w = D_800F13EC;
        s32 h = D_800F13F0;
        cx = w / 2;
        cy = h / 2;
    }
    gDPLoadTextureBlock(gdl++, D_800F219C->img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < 0x80; i++) {
        if (p[i].on != 0) {
            s = D_800C2C60[(((D_800E76D0 & 0xFFFFFF) * 256 + i * 512) >> 4) & 0xFFF] / 2;
            z = p[i].z;
            x = p[i].x + s;
            x = cx + x * cx / z;
            y = p[i].y + s;
            y = cy - y * cy / z;
            a = 0xFF - (z >> 8);
            if (a < 0x20) {
                a = 0x20;
            }
            r = 0x80 / (z >> 8);
            r = (r >> 1) + 1;
            if (r > 4) {
                r = 4;
            }
            gDPPipeSync(gdl++);
            gDPSetColor(gdl++, G_SETPRIMCOLOR, a | 0xFFFFFF00);
            gSPTextureRectangle(gdl++, (x - r) << 2, (y - r) << 2, (x + r) << 2, (y + r) << 2,
                                G_TX_RENDERTILE, 0, 0, D_800CD000[r].b, D_800CD000[r].b);
        }
    }
    if (D_800CCFF8 != 0) {
        gDPSetColor(gdl++, G_SETPRIMCOLOR, (D_800CCFF8 * 255 / 60) | 0xFFFFFF00);
        gDPLoadTextureBlock(gdl++, D_800F219C->img2, G_IM_FMT_IA, G_IM_SIZ_8b,
                            D_800F219C->w * 8, D_800F219C->h, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 8, 8, G_TX_NOLOD, G_TX_NOLOD);
        gDPPipeSync(gdl++);
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
        gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPVertex(gdl++, D_800CD018, 4, 0);
        gSP2Triangles(gdl++, 0, 2, 1, 0, 0, 3, 2, 0);
    }
    gDPPipeSync(gdl++);
    return gdl;
}
