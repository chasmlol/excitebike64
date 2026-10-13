#include "ctx.h"

typedef struct {
    s32 id;
    s32 x0, z0, x1, z1;
    s32 pad14, pad18;
    u32 mask;
} Item;

typedef struct {
    u32 n;
    Item it[1];
} ItemList;

typedef struct {
    s32 pad0;
    void *seg;
} SegRef;

typedef struct {
    u8 pad[0x58];
    u16 fogmin[4];
    u16 fogmax[4];
} FogTab;

extern SegRef *D_800E783C;
extern SegRef *D_800E771C;
extern SegRef *D_800E7710;
extern ItemList *D_800E7720;
extern u8 D_800DC0E0[];
extern ItemList *D_800C9644;
extern ItemList *D_800C9640;
extern Gfx **D_800E78A0;
extern Gfx **D_800E7828;
extern s16 *D_800E7714;
extern s32 D_800E7724;
extern s32 D_800C2350;
extern s32 D_800C2594;
extern s32 D_800E1898;
extern FogTab *D_800EFD64;
extern u8 D_800C95B8[];
extern u8 D_800C95D8[];

Gfx *func_800281F4(Gfx *gdl, s32 i);
Gfx *func_80013D70(s32 a, s32 b, SegRef **c, Gfx *gdl);

Gfx *func_80022FD8(Gfx *gdl, s32 a1, s32 a2, s32 view)
{
    u32 i, n;
    s32 j;
    s32 k;
    u32 m;
    FogTab *fog;
    Item *it;

    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (D_800C2350 & 0x20) {
        gSPSegment(gdl++, 6, D_800C95B8);
        gSPSegment(gdl++, 7, D_800C95D8);
        for (i = 0; i < D_800C9640->n; i++) {
            m = D_800C9640->it[i].mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gdl = func_800281F4(gdl, i);
                gSPDisplayList(gdl++, D_800E7828[i]);
            }
        }
    }
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C->seg);
    if (D_800E7714 != NULL && view == 0) {
        for (j = 0; j < D_800E7724; j++) {
            k = D_800E7714[j];
            if (k < 0) continue;
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    } else {
        for (k = 0; k < D_800C9644->n; k++) {
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    fog = D_800EFD64;
    gSPFogPosition(gdl++, fog->fogmin[D_800E1898 - 1], fog->fogmax[D_800E1898 - 1]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFFFFF);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    if (D_800E7714 != NULL && view == 0) {
        for (j = 0; j < D_800E7724; j++) {
            k = D_800E7714[j];
            if (k < 0) continue;
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    } else {
        for (k = 0; k < D_800C9644->n; k++) {
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800E771C->seg);
    if (D_800C2594 < 3) {
        for (i = 0; i < D_800E7720->n; i++) {
            if (D_800C2350 & 0x10) {
                gdl = func_80013D70(4, D_800E7720->it[i].id, &D_800E771C, gdl);
            }
        }
    } else {
        gdl = func_80013D70(4, (s32)D_800E7710->seg, &D_800E771C, gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}
