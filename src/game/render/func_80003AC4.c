#include "ctx.h"

typedef struct {
    s32 x, y, w, h;
} Rect;

extern u8 D_800DC0E0[];
extern s32 D_800E1898;
extern Rect D_800E1718[][4];

Gfx *func_80003AC4(Gfx *gdl)
{
    s32 i;
    s32 n;
    s32 m;
    s32 lim;

    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    n = D_800E1898;
    gDPSetFillColor(gdl++, (GPACK_RGBA5551(D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 1) << 16) |
                               GPACK_RGBA5551(D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 1));
    i = 0;
    m = n - 1;
    if (n > 0) {

    do {
        gDPFillRectangle(gdl++, D_800E1718[m][i].x, D_800E1718[m][i].y,
                         D_800E1718[m][i].x + D_800E1718[m][i].w - 1,
                         D_800E1718[m][i].y + D_800E1718[m][i].h - 1);
    i++;
    } while (i < D_800E1898);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}
