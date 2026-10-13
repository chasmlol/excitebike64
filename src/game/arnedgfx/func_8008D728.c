#include "ctx.h"

typedef struct {
    s32 n;
    Vtx_t *v;
} Model;

typedef struct Seg Seg;

typedef struct {
    s32 w[16];
} MtxS;

typedef struct {
    Model *m0;
    Model *m4;
    s16 slot;
    u8 rotA;
    u8 rotB;
    u8 flags;
    u8 dirty;
    s16 dx;
    s16 dz;
    s16 pad12;
} Cell;

typedef struct {
    Model *mdl;
    s32 pad4;
    Mtx mtx[2];
} ObjEnt;

typedef struct {
    s32 pad0[2];
    Vp vp[2];
    Mtx mtx[2];
    s32 count;
    u16 persp;
    u16 padAE;
    ObjEnt ent[12];
} ObjSet;

typedef struct {
    u8 pad0[0x170];
    s32 mode;
    u8 pad174[0x1710 - 0x174];
    Seg *segs;
    Model *mdl;
} Scene;

extern Cell *D_800CE1FC;
extern Scene *D_800CEA40;
extern s16 D_800C2C60[];
extern Mtx D_800C2678;
extern u16 D_800E1B44;
extern s32 D_800E2038;
extern MtxS D_800F4B00[];
extern s16 D_800F4B80[450];
extern Mtx D_800F4F08[];
extern Seg *D_800F4F88;
extern s32 D_800F4F8C;
extern s32 D_800F4F90;
extern s32 D_800F4F94;
extern s32 D_800F4F98;
extern Vtx *D_800F4F9C;
extern Lights1 D_800F4FA0[];
extern Vp D_800F4FD8;
extern s32 D_800F4FE8;
extern s32 D_800F4FEC;
extern ObjSet *D_800F4FF0;
extern Seg *D_800F5574;
extern s32 D_800F5584;
extern s32 D_800F5588;
extern s32 D_800F5590;
extern s32 D_800F5598;
extern Vtx_t **D_800F55A0;

void func_8008EDE4(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag);
Gfx *func_8008CC80(Gfx *gdl);
void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
Gfx *func_80013D70(s32 flags, Model *mdl, Seg **segs, Gfx *gdl);

#define SINS(a) D_800C2C60[((a) >> 4) & 0xFFF]

Gfx *func_8008D728(s32 idx, Gfx *gdl, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9) {
    s32 total;
    s32 x0, z0;
    s32 i, j, x, y, n;
    s32 dx, dz, a;
    s32 r, g, b;
    Cell *cell;
    Vtx_t *out;
    ObjSet *os;
    MtxS *m;

    total = D_800F4F8C * D_800F4F90;
    for (i = 0; i < 450; i++) {
        if (D_800F4B80[i] == -2) {
            D_800F4B80[i] = -1;
        }
    }
    i = 0;
    x0 = -(D_800F4F8C * 250);
    z0 = -(D_800F4F90 * 250);
    for (y = 0; y < D_800F4F90; y++) {
        for (x = 0; x < D_800F4F8C; x++, i++) {
            cell = &D_800CE1FC[i];
            if (cell->dirty != 0) {
                if (cell->slot != -1) {
                    D_800F4B80[cell->slot] = -2;
                }
                for (n = 0; n < 450; n++) {
                    if (D_800F4B80[n] == -1) {
                        break;
                    }
                }
                if (cell->m0 == NULL && cell->m4 == NULL) {
                    n = 450;
                }
                if (n == 450) {
                    cell->slot = -1;
                } else {
                    D_800F4B80[n] = i;
                    cell->slot = n;
                    out = D_800F55A0[n];
                    cell->dirty = 0;
                    n = 0;
                    if (cell->m0 != NULL && (u32)cell->m0->n < 0x76) {
                        func_8008EDE4(cell->m0->v, cell->m0->n, cell->rotA, out, x0 + x * 500, z0 + y * 500, 1);
                        n = cell->m0->n;
                    }
                    if (cell->m4 != NULL && (u32)(cell->m4->n + n) < 0x76) {
                        func_8008EDE4(cell->m4->v, cell->m4->n, cell->rotB, &out[n], x0 + x * 500 + cell->dx, z0 + y * 500 + cell->dz, 0);
                        n += cell->m4->n;
                    }
                    for (; n < 0x75; n++) {
                        out[n].flag = 2;
                    }
                }
            }
        }
    }
    dx = D_800C2C60[(((D_800E2038 << 8) >> 4) - 0x400) & 0xFFF] * 80 / 0x4000;
    dz = D_800C2C60[((D_800E2038 << 8) >> 4) & 0xFFF] * 80 / 0x4000;
    {
    Lights1 light = gdSPDefLights1(0x28, 0x28, 0x28, 0xFF, 0xFF, 0xFF, dx, 0x7F, dz);
    D_800F4FA0[idx] = light;
    }
    gSPSetLights1(gdl++, D_800F4FA0[idx]);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800F4F94, D_800F4F98, D_800F4F94 + D_800F4FE8, D_800F4F98 + D_800F4FEC);
    gSPViewport(gdl++, &D_800F4FD8);
    gSPPerspNormalize(gdl++, D_800E1B44);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &D_800F4F08[idx], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    if (p9 != 0) {
        gDPSetRenderMode(gdl++, 0x0F0A4000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        for (i = 0; i < total; i++) {
            x = i % D_800F4F8C;
            y = i / D_800F4F8C;
            n = (x + y) & 1;
            if (n) {
                r = 16;
                g = 16;
                b = 16;
            } else {
                r = g = x * 100 / D_800F4F8C + 35;
                b = x * 60 / D_800F4F8C + 140;
            }
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, r, g, b, 0xFF);
            gSPVertex(gdl++, &D_800F4F9C[y * (D_800F4F8C + 1) + x], 2, 0);
            gSPVertex(gdl++, &D_800F4F9C[(y + 1) * (D_800F4F8C + 1) + x], 2, 2);
            gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    if (p9 != 0) {
        gdl = func_8008CC80(gdl);
    }
    gDPPipeSync(gdl++);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetEnvColor(gdl++, 0xFF, 0x82, 0x82, 0xFF);
    for (i = 0; i < total; i++) {
        cell = &D_800CE1FC[i];
        if (cell->slot != -1) {
            n = 0;
            if (cell->m0 != NULL && (u32)cell->m0->n < 0x76) {
                gDPPipeSync(gdl++);
                if (cell->flags & 1) {
                    s32 s;
                    s = D_800E2038 << 10;
                    gDPSetPrimColor(gdl++, 0, 0, (SINS(s) * 25 >> 13) + 128, (SINS(s) * 25 >> 13) + 128, (SINS(s) * 127 >> 14) + 128, 0xFF);
                    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
                    gSPClearGeometryMode(gdl++, 0x220004);
                } else {
                    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
                    gSPSetGeometryMode(gdl++, 0x220004);
                }
                out = cell->m0->v;
                cell->m0->v = D_800F55A0[cell->slot];
                gdl = func_80013D70(12, cell->m0, &D_800F4F88, gdl);
                cell->m0->v = out;
                n = cell->m0->n;
            }
            if (cell->m4 != NULL && (D_800CEA40->mode != 6 || !(cell->flags & 1)) && (u32)(cell->m4->n + n) < 0x76) {
                gDPPipeSync(gdl++);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
                gSPClearGeometryMode(gdl++, 0x220004);
                out = cell->m4->v;
                cell->m4->v = D_800F55A0[cell->slot] + n;
                gdl = func_80013D70(12, cell->m4, &D_800F5574, gdl);
                cell->m4->v = out;
            }
        }
    }
    if (p8 != -1 && D_800CEA40->mode != 6) {
        gDPPipeSync(gdl++);
        m = &D_800F4B00[idx];
        func_80011490(m, 0, 0, p8 + 0x4000, p6, 2500, p7, 0x100);
        gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPSetRenderMode(gdl++, 0x504A50, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPPipeSync(gdl++);
        gdl = func_80013D70(12, D_800CEA40->mdl, &D_800CEA40->segs, gdl);
        gDPPipeSync(gdl++);
        gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    for (i = 0; i < 8; i++) {
        n = i & 1;
    }
    if (p9 != 0) {
        gDPPipeSync(gdl++);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x80);
        gSPClearGeometryMode(gdl++, G_CULL_BOTH);
        gSPSetGeometryMode(gdl++, G_CULL_FRONT);
        gdl = func_8008CC80(gdl);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_CULL_BOTH);
        gSPSetGeometryMode(gdl++, G_CULL_BACK);
    }
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gSPSetGeometryMode(gdl++, 0x220004);
    for (i = 0; i < 8; i++) {
        os = D_800F4FF0 + i;
        if (os->count != 0) {
            gSPMatrix(gdl++, &os->mtx[idx], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(gdl++, os->persp);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800F5584, D_800F5588, D_800F5590, D_800F5598);
            gSPViewport(gdl++, &os->vp[idx]);
            for (j = 0; j < os->count; j++) {
                gSPMatrix(gdl++, &os->ent[j].mtx[idx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gdl = func_80013D70(12, os->ent[j].mdl, &D_800F4F88, gdl);
            }
            D_800F4FF0[i].count = 0;
        }
    }
    gSPClearGeometryMode(gdl++, 0x220004);
    return gdl;
}
