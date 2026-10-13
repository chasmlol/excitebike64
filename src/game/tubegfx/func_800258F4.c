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
    s32 pad0;
    void *seg;
} SegRef;

extern SegRef *D_800E781C;
extern SegRef *D_800E77C4;
extern SegRef *D_800E77F8;
extern SegRef *D_800E783C;
extern u8 D_800DC0E0[];
extern s32 D_800E7770[];
extern s32 D_800E7780[];
extern Vp *D_800E19C0;
extern ItemList *D_800C9644;
extern ItemList *D_800C9640;
extern Vec3i D_800E8090[];
extern s32 D_800E77D0;
extern s32 D_800E30A8;
extern MtxS *D_800E7798[];
extern MtxS **D_800E77D4;
extern Gfx **D_800E78A0;
extern Gfx **D_800E7828;
extern s32 *D_800E7834;
extern s32 D_800E19CC;
extern u8 D_800C9588[];
extern u8 D_800C95A0[];

void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
Gfx *func_800281F4(Gfx *gdl, s32 i);
Gfx *func_80066070(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis);

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

Gfx *func_800258F4(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    u32 i;
    MtxS *m;
    Item *it;
    u32 sh;
    s32 tmp[18];

    gSPSegment(gdl++, 1, D_800E781C->seg);
    if (D_800E77C4 != NULL) {
        gSPSegment(gdl++, 2, D_800E77C4->seg);
    }
    if (D_800E77F8 != NULL) {
        gSPSegment(gdl++, 3, D_800E77F8->seg);
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    gSPFogPosition(gdl++, D_800E7770[view], D_800E7780[view]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C->seg);
    gSPViewport(gdl++, &D_800E19C0[view + 4]);

    for (i = 0; i < D_800C9644->n; i++) {
        it = &D_800C9644->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            D_800E77D4[i * 4 + view] = m;
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E78A0[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gdl = func_80066070(gdl, (Mtx *)mtx, a2, view, 3);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPSegment(gdl++, 6, D_800C9588);
    gSPSegment(gdl++, 7, D_800C95A0);

    for (i = 0; i < D_800C9640->n; i++) {
        if (D_800E7834[i] & 8) continue;
        it = &D_800C9640->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x, z;
            gdl = func_800281F4(gdl, i);
            x = (it->x0 + it->x1) / 2;
            z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                if (D_800E7834[i] & 1) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E7828[i]);
                if (D_800E7834[i] & 1) {
                    gSPSetGeometryMode(gdl++, D_800E19CC);
                }
            }
        }
    }
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}
