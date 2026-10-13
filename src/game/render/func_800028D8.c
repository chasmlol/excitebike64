#include "ctx.h"

extern Gfx D_800C2A38[];
extern Gfx D_800C2A00[];
extern Vp D_800C25E8;
extern void *D_800C29D8[];
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern u16 D_800E1B44;
extern s32 D_800E1718[];
extern s32 D_800E19CC;

Gfx *func_800028D8(Gfx *gdl, s32 fb, u8 *mtx) {
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gSPViewport(gdl++, &D_800C25E8);
    gSPPerspNormalize(gdl++, D_800E1B44);
    gDPPipeSync(gdl++);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0x10001);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0], D_800E1718[1],
                  D_800E1718[0] + D_800E1718[2], D_800E1718[1] + D_800E1718[3]);
    gSPSetGeometryMode(gdl++, D_800E19CC);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    return gdl;
}
