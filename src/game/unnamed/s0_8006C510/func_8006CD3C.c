#include "ctx.h"

typedef struct { s16 m[32]; } MtxS;

typedef struct {
    u8 pad0[0xC];
    u8 fC;
    u8 padD[0x18 - 0xD];
    s16 f18;
    s16 f1A;
    u8 pad1C[0x28 - 0x1C];
} Track;

typedef struct {
    f32 x, y, z;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x6C - 0x1C];
} Node;

typedef struct { u8 pad[0x64]; s16 f64; } Game;
typedef struct { u8 pad[0x58]; u16 f58; u8 pad5A[6]; u16 f60; } Fog;
typedef struct { u8 r, g, b; } RGB;
typedef struct { Mtx m[4]; } MtxSet;

extern Game *D_800CA930;
extern Track D_800CADE4[];
extern s32 D_800EE490;
extern Node *D_800EE498;
extern MtxSet D_800F16E8[];
extern s32 D_800C8C74;
extern s32 D_800F16E0;
extern u8 D_800EFD6C[];
extern Fog *D_800EFD64;
extern RGB D_800DC0E0;
extern Gfx D_800C95A0[];
extern Gfx *D_800F18F0;

u32 *func_80002418(u32 *dl, s32 p2, s32 p3);
void func_800120A4(s16 *a, s16 *b, s16 *o);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);
void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
Gfx *func_80013D70(s32 a, s32 b, void *c, Gfx *gdl);

Gfx *func_8006CD3C(Gfx *gdl, Mtx *proj, s32 n) {
    MtxS m1;
    MtxS m2;
    s16 ang;
    s32 ratio;
    Node *nd;

    if (D_800CADE4[D_800CA930->f64].f1A < D_800EE490) {
        { u32 b = (u32)D_800EE498; nd = (Node *)(D_800CADE4[D_800CA930->f64].f1A * sizeof(Node) + b); }
        func_8001254C(nd->f18 * 1024.0f, nd->f14 * 1024.0f, &ang, &ratio);
        func_80011490(&m1, 0, 0, ang, 0, 0, 0, 0x100);
        func_80011490(&m2, 0, 0, 0,
                      D_800EE498[D_800CADE4[D_800CA930->f64].f1A].x * 1000.0f,
                      D_800EE498[D_800CADE4[D_800CA930->f64].f1A].y * 1000.0f,
                      D_800EE498[D_800CADE4[D_800CA930->f64].f1A].z * 1000.0f, 0x10);
        func_800120A4(m1.m, m2.m, (s16 *)&D_800F16E8[n].m[0]);
    }
    nd = (Node *)(D_800CADE4[D_800CA930->f64].f18 * sizeof(Node) + (u32)D_800EE498);
    func_8001254C(nd->f18 * 1024.0f, nd->f14 * 1024.0f, &ang, &ratio);
    func_80011490(&m1, 0, 0, ang, 0, 0, 0, 0x100);
    func_80011490(&m2, 0, 0, 0,
                  D_800EE498[D_800CADE4[D_800CA930->f64].f18].x * 1000.0f,
                  D_800EE498[D_800CADE4[D_800CA930->f64].f18].y * 1000.0f,
                  D_800EE498[D_800CADE4[D_800CA930->f64].f18].z * 1000.0f, 0x10);
    func_800120A4(m1.m, m2.m, (s16 *)&D_800F16E8[n].m[1]);

    gSPMatrix(gdl++, &D_800F16E8[n].m[1], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)func_80002418((u32 *)gdl, (s32)proj, 0);
    gSPMatrix(gdl++, proj + 4, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, 0x200004);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800EFD64->f60, D_800EFD64->f58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C, gdl);
    if (D_800CADE4[D_800CA930->f64].fC & 2) {
        D_800C8C74 = 0;
    } else {
        gSPMatrix(gdl++, &D_800F16E8[n].m[0], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPPipeSync(gdl++);
        gdl = func_80013D70(0, D_800F16E0, D_800EFD6C, gdl);
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gSPSetGeometryMode(gdl++, 0x200004);
        gSPSetGeometryMode(gdl++, G_ZBUFFER);
        gDPSetBlendColor(gdl++, 0, 0, 0, 255);
        gDPSetAlphaCompare(gdl++, G_AC_NONE);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
        gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
        gDPSetFogColor(gdl++, D_800DC0E0.r, D_800DC0E0.g, D_800DC0E0.b, 255);
        gSPFogPosition(gdl++, D_800EFD64->f58, D_800EFD64->f60);
        D_800C8C74 = 0;
        gSPDisplayList(gdl++, D_800C95A0);
        gSPDisplayList(gdl++, D_800F18F0);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_FOG);
    }
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}
