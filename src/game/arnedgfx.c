#include "game.h"

typedef struct {
    u32 x0;
    void *pal;
    Img_2 tex;
} TexSet_4;

typedef struct {
    u8 pad0[8];
    s16 f8;
    u8 pad1[0xA];
} Cell_7;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u8 pad[0xA];
} GridPt;

typedef struct Seg_19 Seg_19;

extern TexSet_4 *D_800F5578;
extern Cell_7 *D_800CE1FC;
extern GridPt *D_800F4F9C;
extern Seg_19 *D_800F4F88;
extern Seg_19 *D_800F5574;

typedef struct {
    s16 ob[3];
    u16 flag;
    s16 tc[2];
    u8 cn[4];
} V_2;

extern V_2 *D_800F557C_2;

void func_8008CB10(s32 x, s32 z, s32 w, s32 h)
{
    D_800F557C_2[D_800F4FD4].ob[0] = x;
    D_800F557C_2[D_800F4FD4].ob[1] = 0;
    D_800F557C_2[D_800F4FD4].ob[2] = z;
    D_800F557C_2[D_800F4FD4].tc[0] = 0;
    D_800F557C_2[D_800F4FD4].tc[1] = 0;
    D_800F4FD4++;
    D_800F557C_2[D_800F4FD4].ob[0] = x + w;
    D_800F557C_2[D_800F4FD4].ob[1] = 0;
    D_800F557C_2[D_800F4FD4].ob[2] = z + h;
    D_800F557C_2[D_800F4FD4].tc[0] = 0x800;
    D_800F557C_2[D_800F4FD4].tc[1] = 0;
    D_800F4FD4++;
    if (w != 0) {
        z += w / 2;
        x = (x * D_800F4F8C + 1) / D_800F4F8C;
    }
    if (h != 0) {
        x -= h / 2;
        z = (z * D_800F4F90 + 1) / D_800F4F90;
    }
    D_800F557C_2[D_800F4FD4].ob[0] = x;
    D_800F557C_2[D_800F4FD4].ob[1] = 500;
    D_800F557C_2[D_800F4FD4].ob[2] = z;
    D_800F557C_2[D_800F4FD4].tc[0] = 0;
    D_800F557C_2[D_800F4FD4].tc[1] = 0x1000;
    D_800F4FD4++;
    D_800F557C_2[D_800F4FD4].ob[0] = x + w;
    D_800F557C_2[D_800F4FD4].ob[1] = 500;
    D_800F557C_2[D_800F4FD4].ob[2] = z + h;
    D_800F557C_2[D_800F4FD4].tc[0] = 0x800;
    D_800F557C_2[D_800F4FD4].tc[1] = 0x1000;
    D_800F4FD4++;
}

typedef struct {
    u8 pad[0x170];
    s32 mode;
} GState_6;

extern GState_6 *D_800CEA40_4;

Gfx *func_8008CC80(Gfx *gdl)
{
    s32 i, j, n;
    TexSet_4 *s;
    Img_2 *t;

    if (D_800CEA40_4->mode == 6) {
        return gdl;
    }
    {
        s = D_800F5578;
        gDPLoadTLUT_pal256(gdl++, s->pal);
        t = &s->tex;
        gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b,
                            t->w * 8, t->h, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 6, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0; i < D_800F4FD4; i += 32) {
            n = D_800F4FD4 - i;
            if (n > 32) n = 32;
            gSPVertex(gdl++, &D_800F557C_3[i], n, 0);
            for (j = 0; j < n / 4; j++) {
                gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
            }
        }
    }
    return gdl;
}

void func_8008CF94(s32 w, s32 h)
{
    s32 size;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 nx;
    s32 ny;
    s32 hh;
    u8 *p;
    GridPt *g;

    if (D_800CE1FC != NULL) {
        func_8008ED48();
    }
    D_800F4FF0 = func_800BC908(0x3880, 0, (s32)D_800DA830, 0xD4);
    size = w * ((h << 2) + h << 2);
    D_800CE1FC = p = func_800BC908(size, 0, (s32)D_800DA830, 0xD7);
    if (p == NULL) {
        p = func_8000ECB8_2(1);
    }
    D_800F4F8C = w;
    D_800F4F90 = h;
    func_8000ED10(p, 0, size);
    n = D_800F4F8C * D_800F4F90;
    for (i = 0; i < n; i++) {
        D_800CE1FC[i].f8 = -1;
    }
    n = -1;
    for (i = 0x1C1; i >= 0; i--) {
        D_800F4B80[i] = n;
    }
    D_800F55A0 = func_800BC908(0x708, 0, (s32)D_800DA830, 0xE4);
    for (i = 0; i < 0x1C2; i++) {
        p = func_800BC908(0x750, 0, (s32)D_800DA830, 0xE7);
        D_800F55A0[i] = p;
        func_8000ED10(p, 0, 0x750);
    }
    hh = h;
    do {
        GridPt **pp = &D_800F4F9C;
        nx = w + 1;
        ny = hh + 1;
        *pp = func_800BC908(nx * (ny << 4), 0, (s32)D_800DA830, 0xEF);
        i = 0;
    } while (0);
    k = 0;
    if (ny > 0) {
        do {
            for (j = 0; j < w + 1; j++) {
                D_800F4F9C[k].x = j * 500 - w * 250;
                D_800F4F9C[k].y = 0;
                D_800F4F9C[k].z = i * 500 - hh * 250;
                k++;
            }
        } while (++i < hh + 1);
    }
    i = 0;
    func_8000ED10(D_800F4FF0, 0, 0x3880);
    D_800F557C = func_800BC908((D_800F4F8C + D_800F4F90) * 0x80, 0, (s32)D_800DA830, 0x100);
    D_800F4FD4 = 0;
    for (i = 0; i < D_800F4F8C; i++) {
        func_8008CB10(i * 500 - D_800F4F8C * 250, D_800F4F90 * 250, 500, 0);
        func_8008CB10(D_800F4F8C * 250 - i * 500, -(D_800F4F90 * 250), -500, 0);
    }
    for (i = 0; i < D_800F4F90; i++) {
        func_8008CB10(-(D_800F4F8C * 250), i * 500 - D_800F4F90 * 250, 0, 500);
        func_8008CB10(D_800F4F8C * 250, D_800F4F90 * 250 - i * 500, 0, -500);
    }
    D_800F55A4 = -1;
}

typedef struct {
    s32 w0;
    s32 w1;
    u8 x8;
    u8 x9;
    u8 b0a;
    u8 b0b;
    u8 b0c;
    u8 flag;
    s16 h0;
    s16 h1;
    s16 pad12;
} Ent14;

extern Ent14 *D_800CE1FC_2;

void func_8008D3B4(s32 x, s32 y, s32 w0, s32 b0a, s32 w1, s32 b0b, s32 b0c, s32 h0, s32 h1)
{
    s32 idx = D_800F4F8C * y + x;

    if ((b0c & 2) || D_800CE1FC_2[idx].w0 != w0 || D_800CE1FC_2[idx].w1 != w1 || D_800CE1FC_2[idx].b0a != b0a ||
        D_800CE1FC_2[idx].b0b != b0b || D_800CE1FC_2[idx].b0c != b0c || D_800CE1FC_2[idx].h0 != h0 ||
        D_800CE1FC_2[idx].h1 != h1) {
        D_800CE1FC_2[idx].flag = 1;
    }
    D_800CE1FC_2[idx].w0 = w0;
    D_800CE1FC_2[idx].w1 = w1;
    D_800CE1FC_2[idx].b0a = b0a;
    D_800CE1FC_2[idx].b0b = b0b;
    D_800CE1FC_2[idx].b0c = b0c & ~2;
    D_800CE1FC_2[idx].h0 = h0;
    D_800CE1FC_2[idx].h1 = h1;
}

typedef struct { Pad16 r[4]; } M;

void func_8008D4C4(s32 idx, s32 x, s32 y, s32 z, s16 rx, s16 ry)
{
    M persp;
    M rot;
    M a;
    M b;
    M c;
    f32 q;
    s32 f1, f2, f3;

    D_800F558C = x;
    D_800C23F0 = 0x7FBC;
    D_800F5594 = y;
    D_800F559C = z;
    D_800F5580 = rx;
    D_800F5582 = ry;
    f1 = D_800F13F0;
    f2 = D_800F13F0;
    f3 = D_800F13F0;
    q = (f32)f1 * 0.5f * D_800C2C60[0] / D_800C2C60[0xC00];
    *(volatile f32 *)&D_800E1930 = q;
    D_800E1930 = q * f2 * 0.00390625f;
    func_800AE540(&persp, &D_800E1B44, 33.0f, 320.0f / f3, 50.0f, D_800C23F0, 1.0f);
    func_800AE898(&rot, D_800F5582 * 360.0f * 1.5258789e-05f, -(f32)D_800F5580 * 360.0f * 1.5258789e-05f, 0.0f);
    func_80012D30((u8 *)&rot);
    func_80011490(&a, 0, 0, 0, -D_800F558C, -D_800F5594, -D_800F559C, 0x100);
    func_800120A4(&a, &rot, &b);
    func_800120A4(&b, D_800C25F8_2, &a);
    func_800120A4(&a, &persp, &c);
    *(M *)D_800F4F08_2[idx] = c;
}

#define SINS(a) D_800C2C60[((a) >> 4) & 0xFFF]

typedef struct {
    s32 n;
    Vtx_t *v;
} Model_7;

typedef struct {
    Model_7 *m0;
    Model_7 *m4;
    s16 slot;
    u8 rotA;
    u8 rotB;
    u8 flags;
    u8 dirty;
    s16 dx;
    s16 dz;
    s16 pad12;
} Cell_8;

typedef struct {
    Model_7 *mdl;
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
    Seg_19 *segs;
    Model_7 *mdl;
} Scene;

extern Scene *D_800CEA40_5;
extern ObjSet *D_800F4FF0_2;

Gfx *func_8008D728(s32 idx, Gfx *gdl, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9) {
    s32 total;
    s32 x0, z0;
    s32 i, j, x, y, n;
    s32 dx, dz, a;
    s32 r, g, b;
    Cell_8 *cell;
    Vtx_t *out;
    ObjSet *os;
    Blk64 *m;

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
                    out = D_800F55A0_2[n];
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
                cell->m0->v = D_800F55A0_2[cell->slot];
                gdl = func_80013D70(12, cell->m0, &D_800F4F88, gdl);
                cell->m0->v = out;
                n = cell->m0->n;
            }
            if (cell->m4 != NULL && (D_800CEA40_5->mode != 6 || !(cell->flags & 1)) && (u32)(cell->m4->n + n) < 0x76) {
                gDPPipeSync(gdl++);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
                gSPClearGeometryMode(gdl++, 0x220004);
                out = cell->m4->v;
                cell->m4->v = D_800F55A0_2[cell->slot] + n;
                gdl = func_80013D70(12, cell->m4, &D_800F5574, gdl);
                cell->m4->v = out;
            }
        }
    }
    if (p8 != -1 && D_800CEA40_5->mode != 6) {
        gDPPipeSync(gdl++);
        m = &D_800F4B00[idx];
        func_80011490(m, 0, 0, p8 + 0x4000, p6, 2500, p7, 0x100);
        gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPSetRenderMode(gdl++, 0x504A50, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPPipeSync(gdl++);
        gdl = func_80013D70(12, D_800CEA40_5->mdl, &D_800CEA40_5->segs, gdl);
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
        os = D_800F4FF0_2 + i;
        if (os->count != 0) {
            gSPMatrix(gdl++, &os->mtx[idx], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(gdl++, os->persp);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800F5584, D_800F5588, D_800F5590, D_800F5598);
            gSPViewport(gdl++, &os->vp[idx]);
            for (j = 0; j < os->count; j++) {
                gSPMatrix(gdl++, &os->ent[j].mtx[idx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gdl = func_80013D70(12, os->ent[j].mdl, &D_800F4F88, gdl);
            }
            D_800F4FF0_2[i].count = 0;
        }
    }
    gSPClearGeometryMode(gdl++, 0x220004);
    return gdl;
}

void func_8008E76C(s32 param_1, s32 param_2, s32 param_3)
{
    s32 cur;
    s32 *w;
    s32 *q;
    s32 *slot;
    u8 *p;

    cur = D_800F55A4;
    D_800F5574 = param_2;
    D_800F5578 = param_3;
    if (cur != param_1) {
        if (cur == -1) {
            D_800F4F88 = func_800BF79C(*(s32 *)(D_800CEA40 + 0x1720), *D_800CE1F0[param_1]);
        } else {
            func_800C0258(D_800F4F88);
            func_800BF79C(*(s32 *)(D_800CEA40 + 0x1720), *D_800CE1F0[param_1]);
            func_800C0258(0);
            p = D_800CEA40;
            *(s32 *)(((s32 *)*(s32 *)(p + 0x1720))[1] + *D_800CE1F0[D_800F55A4] * 4) = 0;
            w = D_800CE1F0[param_1];
            q = (s32 *)*(s32 *)(p + 0x1720);
            slot = (s32 *)(*w * 4 + q[1]);
            *slot = *slot & 0xFFFFFFFC;
        }
        D_800F55A4 = param_1;
    }
}

typedef struct {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    Vp vp[2];           /* 0x08 */
    Mtx mtx[2];         /* 0x28 */
    s32 fA8;            /* 0xA8 */
    u16 persp;          /* 0xAC */
    u8 fAE;             /* 0xAE */
    u8 pad[0x710 - 0xAF];
} Cam_5;

extern Cam_5 *D_800F4FF0_3;

void func_8008E8A4(s32 n, s32 id, s32 x, s32 y, s32 w, s32 h, s32 p7, s32 p8,
                   s32 p9, s32 p10, s32 p11, s32 p12)
{
    Mtx persp;
    Mtx rot;
    Mtx m1;
    Mtx m2;
    Mtx res;
    Cam_5 *c; Cam_5 *base;
    f32 t, t2, asp;

    base = D_800F4FF0_3;
    c = (Cam_5 *)(id * sizeof(Cam_5) + (u32)base);
    c->fA8 = 0;
    c->x = x;
    c->y = y;
    c->vp[n].vp.vscale[0] = w * 2;
    c->vp[n].vp.vscale[1] = h * 2;
    c->vp[n].vp.vscale[2] = 0x1FF;
    c->vp[n].vp.vtrans[0] = (x + w / 2) * 4;
    c->vp[n].vp.vtrans[1] = (y + h / 2) * 4;
    c->vp[n].vp.vtrans[2] = 0x1FF;
    c->fAE = p12;
    t = (f32)D_800F13F0 * 0.5f * (f32)D_800C2C60[0] / (f32)D_800C2C60[0xC00];
    t2 = t * (f32)D_800F13F0 * 0.00390625f;
    asp = 320.0f / (f32)D_800F13F0;
    *(volatile f32 *)&D_800E1930 = t;
    D_800E1930 = t2;
    func_800AE540(&persp, &D_800F4FF0_3[id].persp, 33.0f, asp, 50.0f, 16384.0f, 1.0f);
    func_800AE898(&rot, (f32)p10 * 360.0f * 1.5258789e-05f, -(f32)p11 * 360.0f * 1.5258789e-05f, 0.0f);
    func_80012D30(&rot);
    func_80011490(&m1, 0, 0, 0, -p7, -p8, -p9, 0x100);
    func_800120A4(&m1, &rot, &m2);
    func_800120A4(&m2, &D_800C25F8, &m1);
    func_800120A4(&m1, &persp, &res);
    D_800F4FF0_3[id].mtx[n] = res;
}

void func_8008EB74(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5)
{
    s32 off;
    u8 *p;
    s32 n;
    u8 *q;
    u8 *base;

    base = D_800F4FF0;
    off = param_2 * 0x710;
    p = (u8 *)(off + (s32)base);
    n = *(s32 *)(p + 0xa8);
    if (n < 0xc) {
        q = p + n * 0x88;
        *(s32 *)(q + 0xb0) = param_3;
        func_80011490(p + (*(s32 *)(p + 0xa8) * 0x88 + 0xb0) + (param_1 * 0x40 + 8), 0, 0, 0, param_4, 0, param_5, 0x100);
        *(s32 *)((off + (s32)D_800F4FF0) + 0xa8) += 1;
    }
}

s16 *func_8008EC3C(s32 param_1, s32 param_2)
{
    s32 w;
    s32 h;
    s32 iVar5;
    s32 iVar8;
    s32 *puVar1;
    s32 *puVar7;
    s16 *psVar2;
    s16 *base;
    u32 cnt;
    s32 iVar6;
    s32 q5;
    s32 q8;
    u32 uVar3;
    u32 uVar4;

    w = D_800F4F8C;
    iVar8 = param_1 - w * 250;
    h = D_800F4F90;
    iVar5 = param_2 - h * 250;
    q5 = iVar5 / 500;
    q8 = iVar8 / 500;
    puVar1 = (s32 *)(D_800CE1FC_3 + (q5 * w + q8) * 0x14);
    puVar7 = (s32 *)*puVar1;
    if (puVar7 == NULL || (iVar6 = *(s16 *)(puVar1 + 2)) < 0) {
        return NULL;
    }
    cnt = *(u32 *)puVar7;
    base = (s16 *)D_800F55A0[iVar6];
    uVar4 = 0;
    if (cnt != 0) {
        uVar3 = cnt;
        psVar2 = base;
        do {
            uVar4 = uVar4 + 1;
            if (psVar2[0] == iVar8 && psVar2[2] == iVar5) {
                return psVar2;
            }
            psVar2 += 8;
        } while (uVar4 < uVar3);
    }
    return NULL;
}

void func_8008ED48(void)
{
    s32 i;
    void **tbl;

    i = 0;
    do {
        tbl = (void **)D_800F55A0;
        func_800BCB04(tbl[i]);
        i++;
    } while (i < 0x1c2);
    func_800BCB04(D_800F55A0);
    func_800BCB04(D_800CE1FC);
    func_800BCB04(D_800F4F9C);
    func_800BCB04(D_800F557C);
    func_800BCB04(D_800F4FF0);
    D_800CE1FC = 0;
}

void func_8008EDE4(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag)
{
    s32 x, z, nx, nz;

    for (; n != 0; n--) {
        switch (rot) {
        case 0:
            x = in->ob[0];
            z = in->ob[2];
            nx = in->cn[0];
            nz = in->cn[2];
            break;
        case 1:
            x = in->ob[2];
            nx = in->cn[2];
            z = -in->ob[0];
            nz = -in->cn[0];
            break;
        case 2:
            x = -in->ob[0];
            z = -in->ob[2];
            nx = -in->cn[0];
            nz = -in->cn[2];
            break;
        case 3:
            z = in->ob[0];
            nz = in->cn[0];
            x = -in->ob[2];
            nx = -in->cn[2];
            break;
        }
        out->ob[0] = dx + x + 0xfa;
        out->ob[1] = in->ob[1];
        out->ob[2] = dz + z + 0xfa;
        out->tc[0] = in->tc[0];
        out->tc[1] = in->tc[1];
        out->cn[0] = nx;
        out->cn[1] = in->cn[1];
        out->cn[2] = nz;
        if (flag != 0) {
            out->flag = 1;
        } else {
            out->flag = 0;
        }
        if (x == -0xfa || x == 0xfa || z == -0xfa || z == 0xfa) {
            out->cn[3] = 2;
        } else {
            out->cn[3] = 1;
        }
        out++;
        in++;
    }
}

void func_8008EF84(s32 a0, s32 a1, s32 a2, s32 a3) {
    s16 *q = D_800F4FD8;
    s32 k = 0x1FF;

    q[0] = a2 << 1;
    q[1] = a3 << 1;
    q[2] = k;
    q[4] = (a0 + a2 / 2) * 4;
    q[5] = (a1 + a3 / 2) * 4;
    D_800F4F94 = a0;
    D_800F4F98 = a1;
    D_800F4FE8 = a2;
    q[6] = k;
    D_800F4FEC = a3;
}

void func_8008EFFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s16 *b = D_800F4FD8;

    b[0] = a2 * 2;
    b[2] = 0x1FF;
    b[4] = (a0 + a2 / 2) * 4;
    b[5] = (a1 + a3 / 2) * 4;
    D_800F4F94 = a0;
    D_800F4F98 = a1;
    D_800F4FE8 = a2;
    b[6] = 0x1FF;
    D_800F4FEC = a3;
    b[1] = (a3 + a4) * 2;
}

void func_8008F07C(s32 a0, s32 a1, s32 a2, s32 a3)
{
    D_800F5584 = a0;
    D_800F5588 = a1;
    D_800F5590 = a0 + a2;
    D_800F5598 = a1 + a3;
}
