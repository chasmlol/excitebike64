#include "game.h"

Gfx *func_8006C510(Gfx *param_1, s32 param_2, s32 param_3)
{
    s32 aiStack_e0[10];
    s32 aux[3];
    s16 asStack_a8[32];
    s16 auStack_68[32];
    f32 *q;
    f32 fx;
    s32 i;
    s32 iVar5;
    s32 tbl;
    s32 iVar1;
    Gfx *gA, *gB, *gC, *gD, *gE, *gF;
    Gfx *gP, *gQ, *gT, *gU, *gV;

    func_80013650(aiStack_e0, D_800F1510_2 + 12);
    aux[0] = 0;
    aux[1] = 0;
    aux[2] = 0;
    func_800139A8(aiStack_e0, (u16 *)aux, auStack_68);
    func_80011490(asStack_a8, 0, 0, 0, (s32)(D_800F1510_2[0] * 1000.0f), (s32)(D_800F1510_2[1] * 1000.0f),
                  (s32)(D_800F1510_2[2] * 1000.0f), 8);
    tbl = (u32)D_800F16E8 + (param_3 << 8);
    func_800120A4(auStack_68, asStack_a8, tbl);

    gA = param_1++;
    ((u32 *)gA)[0] = 0xDA380003;
    ((u32 *)gA)[1] = tbl;
    gB = param_1++;
    ((u32 *)gB)[0] = 0xE7000000;
    ((u32 *)gB)[1] = 0;
    gC = param_1++;
    ((u32 *)gC)[0] = 0xD9FEFFFF;
    ((u32 *)gC)[1] = 0;
    gD = param_1++;
    ((u32 *)gD)[0] = 0xE200001C;
    ((u32 *)gD)[1] = 0x442230;
    gE = param_1++;
    ((u32 *)gE)[0] = 0xFCFFFFFF;
    ((u32 *)gE)[1] = 0xFFFCF279;
    gF = param_1++;
    ((u32 *)gF)[0] = 0xE7000000;
    ((u32 *)gF)[1] = 0;

    for (i = 0; i < D_800C2594; i++) {
        param_1 = func_80002418_2(param_1, param_2, i);
        gP = param_1++;
        gQ = param_1++;
        ((u32 *)gP)[0] = 0xDA380007;
        ((u32 *)gP)[1] = param_2 + i * 0x40 + 0x80000000;
        ((u32 *)gQ)[0] = 0xDE000000;
        ((u32 *)gQ)[1] = D_800F18F0;
    }

    gT = param_1++;
    ((u32 *)gT)[0] = 0xE3001001;
    ((u32 *)gT)[1] = 0x8000;
    gU = param_1++;
    ((u32 *)gU)[0] = 0xE200001C;
    ((u32 *)gU)[1] = 0x442230;
    gV = param_1++;
    ((u32 *)gV)[0] = 0xFCFFFFFF;
    ((u32 *)gV)[1] = 0xFFFCF279;
    return param_1;
}

#define gDPSetCombineRaw(pkt, a, b) \
    { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = 0xFC000000 | (a); _g->words.w1 = (b); }

typedef struct { u8 p0[0x58]; u16 a58; u8 p1[6]; u16 a60; } Dev;
typedef struct { f32 w[0x22]; } Cam_4;
typedef struct { s16 m[4][4][2]; } MtxQ;

extern Cam_4 *D_800EE498_32;
extern Dev *D_800EFD64_35;
extern MtxQ D_800F16E8_2[][4];

Gfx *func_8006C794(Gfx *gdl, Mtx *proj, s32 i, s32 j)
{
    MtxQ ma;
    MtxQ mb;
    s16 ang;
    s32 ratio;
    MtxQ *m;

    func_8001254C((s32)(D_800EE498_32->w[0x21] * 1024.0f), (s32)(D_800EE498_32->w[0x20] * 1024.0f), &ang, &ratio);
    func_80011490(&ma, 0, 0, ang, 0, 0, 0, 0x100);
    func_80011490(&mb, 0, 0, 0, (s32)(D_800EE498_32->w[0x1b] * 1000.0f), (s32)(D_800EE498_32->w[0x1c] * 1000.0f),
                  (s32)(D_800EE498_32->w[0x1d] * 1000.0f), 0x10);
    m = &D_800F16E8_2[i][j];
    func_800120A4(&ma, &mb, m);

    gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)func_80002418_3(gdl, proj, j);
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
    gSPFogPosition(gdl++, D_800EFD64_35->a60, D_800EFD64_35->a58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C_3, gdl);
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C_3, gdl);
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
    gSPFogPosition(gdl++, D_800EFD64_35->a58, D_800EFD64_35->a60);
    D_800C8C74 = 0;
    gSPDisplayList(gdl++, D_800C95A0);
    gSPDisplayList(gdl++, D_800F18F0);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

typedef struct { s16 m[32]; } MtxS_4;

typedef struct {
    u8 pad0[0xC];
    u8 fC;
    u8 padD[0x18 - 0xD];
    s16 f18;
    s16 f1A;
    u8 pad1C[0x28 - 0x1C];
} Track_15;

typedef struct {
    f32 x, y, z;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x6C - 0x1C];
} Node_21;

typedef struct { u8 pad[0x58]; u16 f58; u8 pad5A[6]; u16 f60; } Fog_2;
typedef struct { Mtx m[4]; } MtxSet;

extern Track_15 D_800CADE4_13[];
extern Node_21 *D_800EE498_33;
extern MtxSet D_800F16E8_3[];
extern Fog_2 *D_800EFD64_36;

Gfx *func_8006CD3C(Gfx *gdl, Mtx *proj, s32 n) {
    MtxS_4 m1;
    MtxS_4 m2;
    s16 ang;
    s32 ratio;
    Node_21 *nd;

    if (D_800CADE4_13[D_800CA930_2->f64].f1A < D_800EE490) {
        { u32 b = (u32)D_800EE498_33; nd = (Node_21 *)(D_800CADE4_13[D_800CA930_2->f64].f1A * sizeof(Node_21) + b); }
        func_8001254C(nd->f18 * 1024.0f, nd->f14 * 1024.0f, &ang, &ratio);
        func_80011490(&m1, 0, 0, ang, 0, 0, 0, 0x100);
        func_80011490(&m2, 0, 0, 0,
                      D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f1A].x * 1000.0f,
                      D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f1A].y * 1000.0f,
                      D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f1A].z * 1000.0f, 0x10);
        func_800120A4(m1.m, m2.m, (s16 *)&D_800F16E8_3[n].m[0]);
    }
    nd = (Node_21 *)(D_800CADE4_13[D_800CA930_2->f64].f18 * sizeof(Node_21) + (u32)D_800EE498_33);
    func_8001254C(nd->f18 * 1024.0f, nd->f14 * 1024.0f, &ang, &ratio);
    func_80011490(&m1, 0, 0, ang, 0, 0, 0, 0x100);
    func_80011490(&m2, 0, 0, 0,
                  D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f18].x * 1000.0f,
                  D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f18].y * 1000.0f,
                  D_800EE498_33[D_800CADE4_13[D_800CA930_2->f64].f18].z * 1000.0f, 0x10);
    func_800120A4(m1.m, m2.m, (s16 *)&D_800F16E8_3[n].m[1]);

    gSPMatrix(gdl++, &D_800F16E8_3[n].m[1], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)func_80002418_4((u32 *)gdl, (s32)proj, 0);
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
    gSPFogPosition(gdl++, D_800EFD64_36->f60, D_800EFD64_36->f58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    gdl = func_80013D70(0, D_800F16E0, D_800EFD6C_3, gdl);
    if (D_800CADE4_13[D_800CA930_2->f64].fC & 2) {
        D_800C8C74 = 0;
    } else {
        gSPMatrix(gdl++, &D_800F16E8_3[n].m[0], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPPipeSync(gdl++);
        gdl = func_80013D70(0, D_800F16E0, D_800EFD6C_3, gdl);
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gSPSetGeometryMode(gdl++, 0x200004);
        gSPSetGeometryMode(gdl++, G_ZBUFFER);
        gDPSetBlendColor(gdl++, 0, 0, 0, 255);
        gDPSetAlphaCompare(gdl++, G_AC_NONE);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
        gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
        gDPSetFogColor(gdl++, D_800DC0E0_2.r, D_800DC0E0_2.g, D_800DC0E0_2.b, 255);
        gSPFogPosition(gdl++, D_800EFD64_36->f58, D_800EFD64_36->f60);
        D_800C8C74 = 0;
        gSPDisplayList(gdl++, D_800C95A0);
        gSPDisplayList(gdl++, D_800F18F0);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_FOG);
    }
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

#define SX(n) (D_800F13EC / 2 - D_800F13EC * (n) / 512)
#define SY(n) (D_800F13F0 * (n) / 240)

typedef struct {
    s32 *font0;         /* 0x00 */
    u8 pad4[0xC];
    s32 *f10;
    s32 *f14;
    s32 *f18;
    s32 *f1C;
    s32 *f20;
    s32 *f24;
    s32 *f28;
    s32 *f2C;
    s32 *f30;
    s32 *f34;
    s32 *f38;
    u8 pad3C[0x128 - 0x3C];
    u8 *f128;           /* 0x128 */
    Pic *f12C[5];       /* 0x12C */
    Pic *f140;          /* 0x140 */
    Pic *f144;          /* 0x144 */
} Res_5;

typedef struct {
    u8 n;
    u8 pad1[7];
    s16 ids[14];        /* 0x08 */
} List_7;

typedef struct {
    List_7 *list;
    u8 pad4[0x60];
    s16 sel;            /* 0x64 */
} Sel_2;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    s32 f40;
    u8 pad44[8];
    s32 f4C;
} Glob_3;

typedef struct {
    char name[0x1E];
    s16 f1E;
    u8 pad20[0x14];
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    char f44[3];
    char f47[3];
    char f4A[3];
    char f4D[3];
} Info_2;

extern Res_5 *D_800C2354_2;
extern Sel_2 *D_800CA930_14;
extern List_7 D_800CA9C8_2[];
extern Info_2 *D_800EFD64_37;
extern Glob_3 D_800EFD70_48;

Gfx *func_8006D664(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3)
{
    char buf[0x40];
    s32 i;
    s32 c;
    s32 sel;
    s32 x;
    s32 y;
    u8 *p;
    Glob_3 *g;

    func_800C2224();
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPLoadTLUT_pal256(gdl++, D_800C2354_2->f140->pal);
    gdl = func_8000A690(D_800E1718_2.x + 8, D_800E1718_2.y + 3, D_800C2354_2->f140->img, gdl, arg2, 1, 1, 3, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (D_800EFD70_48.f4C == 1) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        for (i = 0; i < D_800CA930_14->list->n; i++) {
            gDPLoadTLUT_pal256(gdl++, D_800C2354_2->f12C[i]->pal);
            p = D_800C2354_2->f12C[i]->img;
            if (i != D_800CA930_14->sel || (D_800E2038 & 0x20)) {
                func_800C1E60(D_800C2354_2->font0, 0x40, 2, (i == D_800CA930_14->sel) ? 7 : 2, (i != D_800CA930_14->sel) ? 0x20000021 : 0x21);
                func_800C1F2C(D_800C2354_2->font0, D_800CB210_11[D_800CA930_14->list->ids[i]]);
                func_800C0ACC(D_800C2354_2->font0, D_800E1718_2.x + D_800E1718_2.w - 0x69,
                              D_800E1718_2.y + 0x15 + D_800C2354_2->f128[0x21] * i);
            }
            gdl = func_8000A690(D_800E1718_2.x + D_800E1718_2.w - p[0] * 8 - 8,
                                D_800E1718_2.y + 0xB + D_800C2354_2->f128[0x21] * i, p, gdl, arg2, 1, 1, 3, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800C2354_2->f128 + 0x20;
        for (i = 0; i < D_800CA930_14->list->n; i++) {
            gdl = func_8000A690(D_800E1718_2.x + D_800E1718_2.w - p[0] * 8, D_800E1718_2.y + 6 + p[1] * i,
                                p, gdl, arg2, 1, 1, 6, 0);
        }
        p = D_800C2354_2->f128 + 0x18;
    } else {
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800C2354_2->f128 + 0x18;
    }
    if (D_800E2038 & 0x20) {
        y = 0xAA;
        if (D_800EFD70_48.f4C == 1) y = 0x9E;
        func_800C1E60(D_800C2354_2->f10, 0x40, 2, 4, 0x21);
        func_800C1F2C(D_800C2354_2->f10, "Press the A Button to Race");
        func_800C0ACC(D_800C2354_2->f10, D_800F13EC / 2 - ((D_800EFD70_48.f4C == 1) ? 0x52 : 0), y);
    }
    if (D_800EFD70_48.f4C == 1) {
        func_800C1E60(D_800C2354_2->f10, 0x40, 2, 7, 0x21);
        if (D_800CA930_14->list - D_800CA9C8_2 == 4) {
            if (D_800CA930_14->sel == D_800CA930_14->list->n - 1) {
                func_800C1F2C(D_800C2354_2->f10, D_800D8F58);
            } else {
                func_800C1F2C(D_800C2354_2->f10, D_800D8F84);
            }
        } else {
            func_800C1F2C(D_800C2354_2->f10, D_800D8FA4);
        }
        func_800C0ACC(D_800C2354_2->f10, D_800F13EC / 2 - 0x52, 0xAA);
    }
    gdl = func_8000A690(D_800E1718_2.x, D_800E1718_2.y, p, gdl, arg2, 1, 1, 6, 0);
    p = D_800C2354_2->f128 + 8;
    gdl = func_8000A690(D_800E1718_2.x, D_800E1718_2.y + D_800E1718_2.h - p[1], p, gdl, arg2, 1, 1, 6, 0);
    if (D_800EFD70_48.f4C == 0 && D_800EFD70_48.f3C >= 2 && (D_800E2038 & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, D_800C2354_2->f144->pal);
        if (D_800EFD70_48.f40 >= 2) {
            gdl = func_8000A690(D_800E1718_2.x + D_800E1718_2.w - 0x4B, D_800E1718_2.y + D_800E1718_2.h - 0xE,
                                D_800C2354_2->f144->img, gdl, arg2, 1, 1, 3, 0);
        }
        if (D_800EFD70_48.f40 < 6) {
            gdl = func_8000A690(D_800E1718_2.x + D_800E1718_2.w - 0x28, D_800E1718_2.y + D_800E1718_2.h - 0xE,
                                D_800C2354_2->f144->img + 8, gdl, arg2, 1, 1, 3, 0);
        }
    }
    func_800C1E60(D_800C2354_2->f10, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354_2->f10, "Best Times:");
    func_800C0ACC(D_800C2354_2->f10, SX(221), SY(197));
    func_800C1E60(D_800C2354_2->f14, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354_2->f14, "Best Lap:");
    func_800C0ACC(D_800C2354_2->f14, SX(-7), SY(197));
    g = &D_800EFD70_48;
    func_800C1E60(D_800C2354_2->f14, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354_2->f14, "Number of Laps:");
    func_800C0ACC(D_800C2354_2->f14, SX(-2), SY(214));
    func_800C1E60(D_800C2354_2->f14, 0x40, 1, 2, (g->f4C == 0 && g->f3C >= 2) ? 0x21 : 0x22);
    func_8000EE30(D_800EFD70_48.f40, buf);
    func_800C1F2C(D_800C2354_2->f14, buf);
    func_800C0ACC(D_800C2354_2->f14, SX(-217) - ((D_800EFD70_48.f4C == 0 && D_800EFD70_48.f3C >= 2) ? 0x1E : 0), SY(214));
    func_800C1E60(D_800C2354_2->f1C, 0x40, 1, 2, 0x20);
    if (D_800EFD70_48.f40 != D_800EFD64_37->f1E) {
        func_800C1E60(D_800C2354_2->f18, 0x40, 2, 2, 0x21);
        func_800C1E60(D_800C2354_2->f28, 0x40, 2, 2, 0x21);
        func_800C1E60(D_800C2354_2->f2C, 0x40, 2, 2, 0x21);
        func_800C1F2C(D_800C2354_2->f18, "Unavailable");
        func_800C0ACC(D_800C2354_2->f18, SX(61), SY(196));
        func_800C1F2C(D_800C2354_2->f28, "Unavailable");
        func_800C0ACC(D_800C2354_2->f28, SX(64), SY(205));
        func_800C1F2C(D_800C2354_2->f2C, "Unavailable");
        func_800C0ACC(D_800C2354_2->f2C, SX(66), SY(214));
    } else {
        func_800C1E60(D_800C2354_2->f18, 0x40, 2, 2, 0x20);
        func_800C1E60(D_800C2354_2->f28, 0x40, 2, 2, 0x20);
        func_800C1E60(D_800C2354_2->f2C, 0x40, 2, 2, 0x20);
        func_80052F30(D_800EFD64_37->f34, buf);
        func_800C1F2C(D_800C2354_2->f18, buf);
        func_800C0ACC(D_800C2354_2->f18, SX(116), SY(196));
        func_80052F30(D_800EFD64_37->f38, buf);
        func_800C1F2C(D_800C2354_2->f28, buf);
        func_800C0ACC(D_800C2354_2->f28, SX(119), SY(205));
        func_80052F30(D_800EFD64_37->f3C, buf);
        func_800C1F2C(D_800C2354_2->f2C, buf);
        func_800C0ACC(D_800C2354_2->f2C, SX(121), SY(214));
    }
    func_80052F30(D_800EFD64_37->f40, buf);
    func_800C1F2C(D_800C2354_2->f1C, buf);
    func_800C0ACC(D_800C2354_2->f1C, D_800F13EC / 2 + D_800F13EC * 111 / 512, SY(197));
    if (D_800EFD70_48.f40 == D_800EFD64_37->f1E) {
        func_800C1E60(D_800C2354_2->f20, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64_37->f44, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354_2->f20, buf);
        func_800C0ACC(D_800C2354_2->f20, SX(7), SY(196));
        func_800C1E60(D_800C2354_2->f30, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64_37->f47, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354_2->f30, buf);
        func_800C0ACC(D_800C2354_2->f30, SX(10), SY(205));
        func_800C1E60(D_800C2354_2->f34, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64_37->f4A, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354_2->f34, buf);
        func_800C0ACC(D_800C2354_2->f34, SX(12), SY(214));
    }
    func_800C1E60(D_800C2354_2->f24, 0x40, 1, 2, 0x22);
    __builtin_memcpy(buf, D_800EFD64_37->f4D, 3);
    buf[3] = 0;
    func_800C1F2C(D_800C2354_2->f24, buf);
    func_800C0ACC(D_800C2354_2->f24, SX(-220), SY(197));
    func_800C1E60(D_800C2354_2->f38, 0x40, 4, 2, 0x20);
    func_800C1F2C(D_800C2354_2->f38, D_800EFD64_37->name);
    func_800C0ACC(D_800C2354_2->f38, SX(215), SY(29));
    return func_800C1394(gdl);
}

void func_8006EB34(u32 *param_1, Dyn *arg1, s32 arg2)
{
    s32 iVar3;
    s32 h;
    s32 iVar4;
    s32 *piVar1;
    char *pcVar2;
    s32 *pf;

    func_800C2224();
    iVar4 = D_800F13F0_2 / 2 - 12;
    if ((D_800EFD70 & 0x200000) != 0) {
        iVar4 = D_800F13F0_2 / 2 - 24;
    }
    func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 7, 0x21);
    func_800C1F2C((s32 *)*D_800C2354, D_800D8FFC);
    func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4);
    if ((D_800E2038 & 0x20) != 0) {
        func_800C1E60((s32 *)*D_800C2354, 0x40, 5, 4, 0x21);
        func_800C1F2C((s32 *)*D_800C2354, D_800D9010);
        func_800C0ACC((s32 *)*D_800C2354, *(volatile s32 *)&D_800F13EC / 2, *(volatile s32 *)&D_800F13F0_2 * 7 / 8);
    }
    func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 2, 0x21);
    if ((D_800EFD70 & 0x400000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9024);
        goto LAB_after;
    }
    if ((D_800EFD70 & 0x100000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9044);
        goto LAB_after;
    }
    if ((D_800EFD70 & 0x800000) != 0) {
        func_800C1F2C((s32 *)*D_800C2354, D_800D9060);
        goto LAB_after;
    }
    func_800C1F2C((s32 *)*D_800C2354, D_800D9078);
LAB_after:
    func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4 + 0x18);
    pf = (s32 *)&D_800EFD70;
    if ((pf[0] & 0x200000) != 0) {
        func_800C1E60((s32 *)*D_800C2354, 0x40, 4, 4, 0x21);
        switch (pf[0x1D5]) {
        case 1:
            func_800C1F2C((s32 *)*D_800C2354, D_800D9090);
            break;
        case 2:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90A8);
            break;
        case 3:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90C8);
            break;
        case 4:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90DC);
            break;
        case 5:
            func_800C1F2C((s32 *)*D_800C2354, D_800D90F4);
            break;
        default:
            goto LAB_end;
        }
    LAB_end:
        func_800C0ACC((s32 *)*D_800C2354, D_800F13EC / 2, iVar4 + 0x30);
    }
    func_800C1394(param_1);
}

void func_8006EE58(void)
{
    D_800F18E8 = func_800BF79C(D_800E7728, D_800D06D0);
    D_800F18EC = func_800BF79C(D_800E7728, D_800D08E0);
    func_80014B84(D_800F18F0_2, D_800F18E8, &D_800F18EC, 0xC);
}

void func_8006EEC4(void)
{
    u32 pa;
    D_800EFD6C = func_800BF79C(D_800E7728, D_800D0DEC);
    D_800F18E8 = func_800BF79C(D_800E7728, D_800D0FB0);
    pa = (u32)&D_800F18EC;
    D_800F18EC = func_800BF79C(D_800E7728, D_800D10F8);
    D_800F16E0 = func_800BF79C(D_800E7728, D_800D0770);
    func_80014B84(D_800F18F0_3, D_800F18E8, pa, 12);
}

void func_8006EF68(void) {
    func_800BCB04(D_800F18F0);
}

void func_8006EF8C(void) {
    func_800BCB04(D_800F18F0);
}

typedef struct {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xA0 - 0x54];
} Rider_6;

typedef struct {
    u32 magic;
    u32 sum;
    Rec_3 recs[24];
    u8 name[0x17];
    u8 f2BF;
    u8 f2C0;
    u8 f2C1;
    u8 pad2C2[2];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
    u8 f2C8;
    u8 pad2C9[7];
    u32 f2D0;
    u8 f2D4[0x804 - 0x2D4];
} Save;

extern Save D_800F1900_5;

void func_8006EFB0(void)
{
    Rec_3 *pd;
    Rec_3 *ps;
    Rider_6 *pc;
    u8 *q;
    u8 c;
    u32 sum;
    u32 *p;
    s32 i;

    func_8000ED10((u8 *)&D_800F1900_5, 0, sizeof(Save));
    func_800A1B0C(D_800F1900_5.f2D4);
    i = 0;
    pc = D_800CB210;
    pd = D_800F1900_5.recs;
    ps = D_800F0200;
    do {
        i++;
        if (pc->flags & 0x200) {
            *pd = *ps;
            pd++;
            ps++;
        }
        pc++;
    } while (i < 27);
    c = '?';
    D_800F1900_5.f2C0 = 1;
    D_800F1900_5.f2BF = 0;
    i = 4;
    q = (u8 *)&D_800F1900_5 + 4;
    do {
        q[0x2A8] = c;
        i--;
        q--;
    } while (i >= 0);
    sum = 0;
    D_800F1900_5.f2C5 = 0x8C;
    D_800F1900_5.f2C6 = 200;
    D_800F1900_5.f2C7 = 0xA0;
    D_800F1900_5.f2C4 = 1;
    D_800F1900_5.f2D0 = 1;
    D_800F1900_5.f2BF = 0x1F;
    D_800F1900_5.f2C1 = 7;
    D_800F1900_5.f2C8 = 0;
    D_800F1900_5.f2C0 = 3;
    D_800F1900_5.magic = 0x5748FB3A;
    p = (u32 *)&D_800F1900_5;
    for (i = 0; i < 0x200; i++) {
        if (i != 1) {
            sum += p[i];
        }
    }
    sum ^= 0xFCB1121D;
    p[1] = sum;
}

typedef struct { u8 pad[0x410]; s32 cur; } S70_2;
typedef struct { u8 pad[0x2BF]; u8 bits; u8 cnt; } S19;
typedef struct { u8 pad[8]; u16 flags; u8 pad2[6]; } Ent_76;

extern S70_2 D_800EFD70_49;
extern S19 D_800F1900_6;
extern Ent_76 D_800CD370[];
extern Ent_76 D_800CD430[];

static inline s32 active(s32 n)
{
    if (D_800F0180 < D_800F1900_6.cnt) return 1;
    if (D_800F1900_6.cnt == D_800F0180) {
        if (n == 0) return 1;
        return (D_800F1900_6.bits >> (n - 1)) & 1;
    }
    return 0;
}

s32 func_8006F128(void)
{
    s32 saved = D_800EFD70_49.cur;
    s32 i, j, act, idx, k;
    s32 ret;
    u32 g;
    s32 hv;
    u32 bit;
    Ent_76 *e;

    for (i = 0; i < 4; i++) {
        k = i * 5;
        switch (i) {
        case 0:
        case 1:
            D_800EFD70_49.cur = 1;
            break;
        case 2:
            D_800EFD70_49.cur = 2;
            break;
        case 3:
            D_800EFD70_49.cur = 3;
            break;
        }
        act = 0;
        if (D_800EFD70_49.cur < D_800F1900_6.cnt) {
            act = 1;
        } else if (D_800F1900_6.cnt == D_800EFD70_49.cur) {
            if (i == 0 || ((D_800F1900_6.bits >> (i - 1)) & 1)) act = 1;
        }
        j = 0;
        idx = k;
        do {
            if (act) {
                bit = 1 << idx;
                g = D_800CA9C4 | bit;
                e = (Ent_76 *)(idx << 4);
                e = (Ent_76 *)((u32)e + (u32)D_800CD430);
                hv = e->flags & ~1;
                D_800CA9C4 = g;
                e->flags = hv;
            } else {
                bit = 1 << idx;
                bit = ~bit;
                e = (Ent_76 *)(idx << 4);
                e = (Ent_76 *)((u32)e + (u32)D_800CD430);
                g = D_800CA9C4 & bit;
                hv = e->flags | 1;
                D_800CA9C4 = g;
                e->flags = hv;
            }
            j++;
            idx++;
        } while (j < 5);
    }

    ret = 0;
    if (D_800F1BC1 == 7) {
        ret = (D_800CD370[2].flags & 1) << 1;
        D_800CD370[2].flags &= ~1;
    } else {
        D_800CD370[2].flags |= 1;
    }

    D_800F0180 = 1;
    if (!(D_800F0180 < D_800F1900_6.cnt || (D_800F1900_6.cnt == D_800F0180 && (D_800F1900_6.bits & 4)))) D_800CD370[3].flags |= 1;

    D_800F0180 = 2;
    if ((D_800F0180 < D_800F1900_6.cnt || (D_800F1900_6.cnt == D_800F0180 && (D_800F1900_6.bits & 8)))) {
        if (D_800CD370[4].flags & 1) ret = 4;
        D_800CD370[4].flags &= ~1;
    } else {
        D_800CD370[4].flags |= 1;
    }

    D_800F0180 = 3;
    if ((D_800F0180 < D_800F1900_6.cnt || (D_800F1900_6.cnt == D_800F0180 && (D_800F1900_6.bits & 16)))) {
        if (D_800CD370[5].flags & 1) ret = 5;
        D_800CD370[5].flags &= ~1;
    } else {
        D_800CD370[5].flags |= 1;
    }

    D_800F0180 = saved;
    return ret;
}

typedef struct { u8 pad0[0x34]; Rec_3 b; s32 flags; u8 pad54[0xA0 - 0x54]; } Rec_42;

typedef struct {
    u8 pad0[8];
    Rec_3 blk[24];
    u8 a8[5];
    u8 tbl[5][3];
    u8 pad2BC[4];
    u8 c0, c1;
    u8 pad2C2[3];
    u8 c5, c6, c7, c8;
    u8 pad2C9[7];
    s32 n;
} Save_2;

typedef struct { u8 p0; u8 a[3]; u8 b[3]; u8 pad[0x24 - 7]; } Ent_77;

typedef struct {
    Ent_77 e[5];
    u8 count;
    u8 padB5[7];
    s16 x[7];
    s16 y[5];
} Cfg_10;

typedef struct { u8 pad[0x50]; u8 b[4]; } Snd_3;

extern Save_2 D_800F1900_7;
extern Rec_42 D_800CB210_12[];
extern Cfg_10 D_800CA9C8_3;
extern Snd_3 D_800EFD70_50;

void func_8006F3E4(s32 arg0)
{
    s32 i, k;
    s32 j;
    u8 v;

    func_8006EFB0();
    do { D_800CA9C4 = 0; } while (0);
    j = 0;
    for (i = 0; i < 27; i++) {
        if (i < D_800F1900_7.n) {
            D_800CA9C4 |= 1 << i;
        }
        if (D_800CB210_12[i].flags & 0x200) {
            Rec_3 *d = &D_800CB210_12[i].b;
            Rec_3 *s = &D_800F1900_7.blk[j];
            *d = *s;
            j++;
        }
    }
    D_800CAA7C = 0;
    for (i = 0; i < 4; i++) {
        D_800EFD70_50.b[i] = (D_800F1900_7.c8 >> i) & 3;
    }
    func_80010E5C(0, D_800F1900_7.c6);
    func_80010E5C(1, D_800F1900_7.c5);
    func_80010E5C(2, D_800F1900_7.c7);
    for (i = 0; i < 5; i++) {
        for (k = 0; k < 3; k++) {
            D_800CA9C8_3.e[i].a[k] = D_800F1900_7.tbl[0][i * 3 + k] >> 5;
            D_800CA9C8_3.e[i].b[k] = D_800F1900_7.tbl[0][i * 3 + k] & 0x1F;
        }
    }
    for (i = 0; i < 5; i++) {
        v = D_800F1900_7.a8[i] & 0x1F;
        if (v == 0x1F) break;
        D_800CA9C8_3.x[i] = v;
        D_800CA9C8_3.y[i] = D_800F1900_7.a8[i] >> 5;
        D_800CA9C8_3.count++;
    }
    D_800CAAA1 = D_800F1900_7.c1;
    D_800F0180 = D_800F1900_7.c0;
    func_8006F128();
}

void func_8006F5F8(s32 x, s32 arg1)
{
    u8 buf[0x50];
    if (x == 1) {
        D_800F1BC1 = D_800CAAA1;
    }
}
