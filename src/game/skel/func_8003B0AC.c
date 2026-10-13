#include "ctx.h"

extern s32 D_800E8D68;
extern s32 D_800E9670;
extern s32 D_800E92E0;
extern s32 D_800F14E8;
extern s32 D_800C238C[];
extern u8 *D_800E92DC;
extern Vtx *D_800E9628;
extern s32 D_800E30A8;
extern s32 D_800E93D8[];

Gfx *func_8003B0AC(s32 id, s32 unused, Gfx *gdl) {
    s32 i;

    if (D_800E8D68 == 0 || D_800E9670 == 0 || D_800E92E0 == 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetCombine(gdl++, 0xFFB3FF, 0xFF65FEFF);
    if (D_800F14E8 == 0) {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[id % 8] * 97 / 256);
    } else {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[id % 8]);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 1);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPLoadTextureBlock(gdl++, D_800E92DC + (id << 10), G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPVertex(gdl++, &D_800E9628[D_800E30A8 * 1152 + id * 72], D_800E92E0, 0);
    for (i = 0; i < D_800E9670 - 1; i += 2) {
        gSP2Triangles(gdl++, D_800E93D8[i * 3], D_800E93D8[i * 3 + 1], D_800E93D8[i * 3 + 2], 0,
                      D_800E93D8[i * 3 + 3], D_800E93D8[i * 3 + 4], D_800E93D8[i * 3 + 5], 0);
    }
    if (D_800E9670 & 1) {
        gSP1Triangle(gdl++, D_800E93D8[i * 3], D_800E93D8[i * 3 + 1], D_800E93D8[i * 3 + 2], 0);
    }
    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    return gdl;
}
