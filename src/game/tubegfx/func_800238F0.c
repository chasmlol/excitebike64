#include "ctx.h"

typedef struct {
    Gfx *dl;
    u8 pad[0x18];
    u32 mask;
} Ent;

typedef struct {
    u32 n;
    Ent e[1];
} Tbl;

typedef struct {
    u8 pad[0x58];
    u16 fogMax[4];
    u16 fogMin[4];
} Fog;

extern Tbl *D_800C9648;
extern s32 D_800C8C74;
extern s32 D_800E1898;
extern Fog *D_800EFD64;
extern u8 D_800E7824[];

Gfx *func_80013D70(s32 a0, Gfx *dl, u8 *a2, Gfx *gdl);

Gfx *func_800238F0(Gfx *gdl, s32 p2, s32 p3, u32 sh)
{
    u32 i;
    u32 off;
    u8 pad[0x48];
    Tbl *tbl = D_800C9648;


    if (tbl == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800EFD64->fogMin[D_800E1898 - 1], D_800EFD64->fogMax[D_800E1898 - 1]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    for (i = 0; i < D_800C9648->n; i++) {
        off = i * 32 + 4;
        if ((*(u32 *)((u8 *)D_800C9648 + off + 0x1C) >> (sh << 1)) & 1) {
            gdl = func_80013D70(0, D_800C9648->e[i].dl, D_800E7824, gdl);
        }
    }
    gDPPipeSync(gdl++);
    D_800C8C74 = 0;
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    return gdl;
}
