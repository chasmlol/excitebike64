#include "ctx.h"

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 pad[0x58]; u16 fmin[4]; u16 fmax[4]; } FogCfg;
typedef struct { u8 r, g, b; } Rgb;
typedef struct { u32 pad0; void *pal; u32 pad8; void *tex; } TexSet;
typedef struct { u8 pad[0x19A]; u8 f19A; u8 f19B; u8 pad19C[4]; s8 f1A0; u8 pad1A1[7]; } Obj;
typedef struct { u8 pad[0x14]; s32 f14; u8 pad18[4]; } Ent1C;
typedef struct { s32 f0; s32 f4; } Pair;

extern FogCfg *D_800EFD64;
extern s32 D_800C2594;
extern Rgb D_800DC0E0;
extern TexSet *D_800E783C;
extern TexSet *D_800E781C;
extern Vp *D_800E19C0;
extern s32 D_800EBE00;
extern s32 D_800EB9EC;
extern s16 D_800EBE08[3][0x400];
extern Obj *D_800ED7AC;
extern s32 D_800E30A8;
extern Mtx D_800EA9E8[][32];
extern Gfx *D_800EA9A8[];
extern char D_80042718[];
extern s32 D_800ED608;
extern Mtx *D_800EBDF8[];
extern Ent1C D_800EA7C8[];
extern Pair D_800CA3BC[];

void func_80014A68(s32 a0);
Gfx *func_80013D70(s32, s32, TexSet **, Gfx *);

Gfx *func_80040190(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    FogCfg *fc;
    s32 i, k, j;
    s16 v;
    Obj *o;

    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0.r, D_800DC0E0.g, D_800DC0E0.b, 255);
    fc = D_800EFD64;
    gSPFogPosition(gdl++, fc->fmin[D_800C2594 - 1], fc->fmax[D_800C2594 - 1]);
    RAW(gdl++, 0xFC127E24, 0xFFFFF9FC);
    RAW(gdl++, 0xE200001C, 0xC8004030);
    D_800EBE00 = -1;
    gDPLoadTLUT_pal256(gdl++, D_800E783C->pal);
    gDPLoadTextureBlock_4b(gdl++, D_800E783C->tex, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPViewport(gdl++, &D_800E19C0[a3 + 4]);
    for (j = 0; j < D_800EB9EC; j++) {
        D_800EBE00 = 0;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x400; i++) {
                v = D_800EBE08[k][i];
                if (v != -1) {
                    o = (Obj *)(v * sizeof(Obj) + (u32)D_800ED7AC);
                    if (o->f1A0 == j && (k != 0 || o->f19A == 0)) {
                        if (D_800EBE00 == 0) {
                            gSPMatrix(gdl++, &D_800EA9E8[D_800E30A8][j], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            D_800EBE00 = 1;
                        }
                        gSPVertex(gdl++, o, 25, 0);
                        { Gfx **dl = D_800EA9A8; gSPDisplayList(gdl++, dl[o->f19B]); }
                    }
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    func_80014A68((s32)D_80042718);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    RAW(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    RAW(gdl++, 0xE200001C, 0x00442230);
    gDPLoadTLUT_pal256(gdl++, D_800E781C->pal);
    for (i = 0; i < D_800ED608; i++) {
        gSPMatrix(gdl++, &D_800EBDF8[a2][i], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = func_80013D70(0, D_800CA3BC[D_800EA7C8[i].f14].f4, &D_800E781C, gdl);
    }
    func_80014A68(0);
    gDPPipeSync(gdl++);
    return gdl;
}
