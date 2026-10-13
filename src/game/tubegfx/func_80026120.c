#include "ctx.h"

typedef struct MtxS {
    s16 i[4][4];
    u16 f[4][4];
} MtxS;

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
    s32 x, y, z;
} Vec3i;

typedef struct {
    s32 a;
    u32 img;
} TexEnt;

typedef struct {
    s32 pad0, pad4;
    TexEnt e[1];
} TexTab;

extern ItemList *D_800C9648;
extern ItemList *D_800C9644;
extern s32 D_800C8C74;
extern s32 D_800E7770[];
extern s32 D_800E7780[];
extern Vec3i D_800E8090[];
extern s32 D_800E77D0;
extern s32 D_800E30A8;
extern MtxS *D_800E7798[];
extern u8 D_800E7824[];
extern s32 D_800E77D8[];
extern s32 D_800E77A0[];
extern Vec3i D_800E7840[];
extern TexTab *D_800E92C8;
extern Vtx D_800C95F8[];

void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void func_80014A68(s32 a0);
Gfx *func_80013D70(s32 a0, s32 a1, void *a2, Gfx *gdl);

static inline MtxS *getmtx(s32 x, s32 y, s32 z, s32 view)
{
    MtxS *m;

    if (D_800E77D0 >= 300) {
        return NULL;
    }
    x = x * 16 - D_800E8090[view].x;
    y = y * 16 - D_800E8090[view].y;
    z = z * 16 - D_800E8090[view].z;
    m = &D_800E7798[D_800E30A8][D_800E77D0];
    func_80011490(m, 0, 0, 0, x, y, z, 0x100);
    D_800E77D0++;
    return m;
}

Gfx *func_80026120(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    s32 i;
    MtxS *m;
    Item *it;
    s32 tmp[18];

    if (D_800C9648 == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800E7780[view], D_800E7770[view]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;

    for (i = 0; i < D_800C9648->n; i++) {
        it = &D_800C9648->it[i];
        if ((it->mask >> (view * 2)) & 1) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gdl = func_80013D70(0, D_800C9648->it[i].id, D_800E7824, gdl);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    for (i = 0; i < 8; i++) {
        if (D_800E77D8[i] == 0) continue;
        it = &D_800C9644->it[D_800E77A0[i]];
        if ((it->mask >> (view * 2)) & 1) {
            s32 a = D_800E77D8[i] / 2;
            s32 f;
            if (a >= 256) {
                a = 255;
            }
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, a);
            f = (120 - D_800E77D8[i]) / 12;
            if (f >= 5) {
                f = (f - 2) % 3 + 2;
            }
            gDPLoadTextureBlock(gdl++, D_800E92C8->e[f].img, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                G_TX_MIRROR, G_TX_MIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            m = getmtx(D_800E7840[i].x, D_800E7840[i].y, D_800E7840[i].z, view);
            if (m != NULL) {
                func_80011490(m, 0, 0, 0, D_800E7840[i].x * 16 - D_800E8090[view].x, D_800E7840[i].y * 16 - D_800E8090[view].y,
                              D_800E7840[i].z * 16 - D_800E8090[view].z, 0x100);
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(gdl++, D_800C95F8, 4, 0);
                gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
    D_800C8C74 = 0;
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    func_80014A68(0);
    return gdl;
}
