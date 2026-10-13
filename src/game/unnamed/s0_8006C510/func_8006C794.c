#include "ctx.h"

typedef struct { u8 p0[0x58]; u16 a58; u8 p1[6]; u16 a60; } Dev;
typedef struct { f32 w[0x22]; } Cam;
typedef struct { s16 m[4][4][2]; } MtxQ;

extern Cam *D_800EE498;
extern Dev *D_800EFD64;
extern u8 D_800EFD6C[];
extern s32 D_800F16E0;
extern MtxQ D_800F16E8[][4];
extern s32 D_800C8C74;
extern Gfx *D_800F18F0;
extern Gfx D_800C95A0[];
extern u8 D_800DC0E0[];

void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);
void func_80011490(MtxQ *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void func_800120A4(MtxQ *a, MtxQ *b, MtxQ *o);
u32 *func_80002418();
Gfx *func_80013D70(s32 a, s32 b, u8 *c, Gfx *d);

#define gDPSetCombineRaw(pkt, a, b) \
    { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = 0xFC000000 | (a); _g->words.w1 = (b); }

Gfx *func_8006C794(Gfx *gdl, Mtx *proj, s32 i, s32 j)
{
    MtxQ ma;
    MtxQ mb;
    s16 ang;
    s32 ratio;
    MtxQ *m;

    func_8001254C((s32)(D_800EE498->w[0x21] * 1024.0f), (s32)(D_800EE498->w[0x20] * 1024.0f), &ang, &ratio);
    func_80011490(&ma, 0, 0, ang, 0, 0, 0, 0x100);
    func_80011490(&mb, 0, 0, 0, (s32)(D_800EE498->w[0x1b] * 1000.0f), (s32)(D_800EE498->w[0x1c] * 1000.0f),
                  (s32)(D_800EE498->w[0x1d] * 1000.0f), 0x10);
    m = &D_800F16E8[i][j];
    func_800120A4(&ma, &mb, m);

    gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)func_80002418(gdl, proj, j);
    gSPMatrix(gdl++, &proj[j + 4], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombineRaw(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800EFD64->a60, D_800EFD64->a58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C, gdl);
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C, gdl);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombineRaw(gdl++, 0x127E24, 0xFFFFF3F9);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    gSPFogPosition(gdl++, D_800EFD64->a58, D_800EFD64->a60);
    D_800C8C74 = 0;
    gSPDisplayList(gdl++, D_800C95A0);
    gSPDisplayList(gdl++, D_800F18F0);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}
