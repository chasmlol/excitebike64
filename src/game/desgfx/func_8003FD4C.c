#include "ctx.h"

typedef struct Patch {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    u8 vis;
    u8 dl;
    s16 bx;
    s16 by;
    s8 mtx;
    u8 pad1A1[7];
} Patch;

typedef struct TexSet {
    s32 pad0;
    u16 *pal;
    s32 pad8;
    u8 *img;
} TexSet;

extern TexSet *D_800E783C;
extern s32 D_800EBE00;
extern s16 D_800EBE08[][1024];
extern Patch *D_800ED7AC;
extern s32 D_800E30A8;
extern Mtx D_800EA9E8[][32];
extern Gfx *D_800EA9A8[];

Gfx *func_8003FD4C(Gfx *gdl, Mtx *m)
{
    s32 i, j;
    s16 id;
    Patch *p;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, 0x200004);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C->pal);
    D_800EBE00 = -1;
    gDPLoadTextureBlock_4b(gdl++, D_800E783C->img, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP, G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    for (j = 0; j < 1; j++) {
        for (i = 0; i < 0x400; i++) {
            id = D_800EBE08[j][i];
            if (id != -1) {
                p = (Patch *)(id * 0x1A8 + (s32)D_800ED7AC);
                if (p->vis == 1) {
                    if (p->mtx != D_800EBE00) {
                        gSPMatrix(gdl++, &D_800EA9E8[D_800E30A8][p->mtx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        D_800EBE00 = p->mtx;
                    }
                    gSPVertex(gdl++, p->v, 25, 0);
                    gSPDisplayList(gdl++, D_800EA9A8[p->dl]);
                }
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}
