#include "ctx.h"

s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);

typedef struct { s32 x, y, z; } V3i;

extern s32 D_800C9860;
extern u8 *D_800EFD64;
extern V3i *D_800C9840;
extern u8 *D_800E8E98;
extern V3i D_800E8090;
extern s32 D_800F13EC;
extern s32 D_800F13F0;

Gfx *func_8003371C(Gfx *gdl, s32 arg)
{
    s32 c, i;
    u8 *img;
    s32 sx, sy, sz, sw;

    if (D_800C9860 == 0) return gdl;
    if (*(s32 *)(D_800EFD64 + 0x50) & 0x20) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        { Gfx *_g = gdl++; _g->words.w0 = 0xFC11FE23; _g->words.w1 = 0xFFFFFFFF; }
        img = *(u8 **)(D_800E8E98 + 0xC) + 0x28;
        gDPLoadTextureBlock(gdl++, img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0, c = 0x80; i < 0x40; i++) {
            if (func_80033AC8(D_800C9840[i].x + (D_800E8090.x >> 4), D_800C9840[i].y + (D_800E8090.y >> 4),
                              D_800C9840[i].z + (D_800E8090.z >> 4), &sx, &sy, &sz, &sw, arg, 0) == 0
                && sx >= 0 && sy >= 0 && sx <= D_800F13EC && sy <= D_800F13F0) {
                gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
                gSPTextureRectangle(gdl++, (sx - 1) << 2, (sy - 1) << 2, (sx + 1) << 2, (sy + 1) << 2,
                                    0, 0, 0, 0x400, 0x400);
            }
            c += 2;
        }
    }
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    return gdl;
}
