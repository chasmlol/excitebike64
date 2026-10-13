#include "ctx.h"

extern s32 D_800C29D8[];
extern Gfx D_800C2A38[];
extern Gfx D_800C2A00[];
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 D_800F5A98;
extern s32 D_800F5A58;
extern Vp D_800F5A88;

void func_80011490(s32 p, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);

Gfx *func_8009FDF4(s32 a0, s32 a1, Gfx *gdl)
{
    func_80011490(a0 + 0x400, 0, 0, 0, 0, 0, 0, 0x100);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    D_800F5A88.vp.vscale[0] = D_800F13EC * 2;
    D_800F5A88.vp.vscale[1] = D_800F13F0 * 2;
    D_800F5A88.vp.vtrans[0] = D_800F13EC * 2;
    D_800F5A88.vp.vtrans[1] = D_800F13F0 * 2;
    gSPViewport(gdl++, &D_800F5A88);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[a1]);
    if (D_800F5A98 != 0) {
        D_800F5A98--;
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gDPSetColorDither(gdl++, G_CD_DISABLE);
    if (D_800F5A98 != 0) {
        return gdl;
    }
    {
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_POINT);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, D_800F5A58);
        return gdl;
    }
    return gdl;
}
