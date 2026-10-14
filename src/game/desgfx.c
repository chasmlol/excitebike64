#include "game.h"

typedef struct { u8 pad[0x198]; u8 f198; u8 rest[0x1A8 - 0x199]; } B1A8;

typedef struct {
    s32 x, y, z;
    u8 pad[0x1C - 12];
} Elem;

extern B1A8 *sDesPatches;
extern Elem sDesObjects[];

typedef struct { s32 *name; s32 val; } NP;

extern NP sDesObjModels[];

void des_init(void)
{
    s32 i;

    gFarClip = 0x4000;
    gTerrainTexSet = res_load(D_800E7728, D_800D0B68);
    for (i = 0; sDesObjModels[i].name != 0; i++) {
        sDesObjModels[i].val = res_load(D_800E7728, *sDesObjModels[i].name);
    }
    gObjectTexSet = res_load(D_800E7728, D_800D1014);
    sDesPatches = mem_alloc_dbg(0x19e10, 0, D_800D6FD0, 0xe9);
    for (i = 0; i < 0xfa; i++) {
        sDesPatches[i].f198 = 0;
    }
    gDesertHeightMap = mem_alloc_dbg(0x40000, 0, D_800D6FD0, 0xed);
    fractal_init(gDesertHeightMap);
    fractal_reset();
    for (i = 0; i < 0x10; i++) {
        sDesPatchDLs[i] = des_build_patch_dl(i);
    }
    mem_fill_2(sDesPatchGrid, -1, 0x1800);
    mem_fill_2(D_800EA998, 0, 0xc);
    sDesFreePatchCount = 0xfa;
    for (i = 0xf9; i >= 0; i--) {
        sDesFreePatches[i] = i;
    }
    sDesObjMtx[0] = mem_alloc_dbg(0x400, 0, D_800D6FD0, 0xfe);
    sDesObjMtx[1] = mem_alloc_dbg(0x400, 0, D_800D6FD0, 0xff);
    sDesArrowTex = res_load(D_800E7728, D_800D0DF0);
    sDesArrowModel = res_load(D_800E7728, D_800D0C50);
    sDesArrowMtx = mem_alloc_dbg(0x80, 0, D_800D6FD0, 0x104);
    sDesMarkerAnim = res_load(D_800E7728, D_800D07C4);
    D_800EA98C = res_load(D_800E7728, D_800D0BC8);
}

typedef struct {
    u8 pad[0x19C];
    s16 x;
    s16 y;
    u8 slot;
    s8 active;
    u8 pad2[0x1A8 - 0x1A2];
} Obj_32;

extern Obj_32 *sDesPatches_2;

void des_update_view(s32 arg0)
{
    s32 i;
    s32 j;
    Elem *e;
    Mtx m1;
    Mtx m2;

    D_800CA3B8 = gRaceFrame / 4;
    fractal_generate_step(100);
    sDesCamX = gCamPos_2.x;
    sDesCamY = gCamPos_2.y;
    sDesCamZ = gCamPos_2.z;
    divmod_floor(sDesCamX, 0x1F400, &D_800EDFD4, &D_800EBDF0);
    divmod_floor(sDesCamZ, 0x1F400, &D_800EDFD8, &D_800EBDF4);
    fractal_update_tiles(sDesCamX, sDesCamZ);
    des_calc_lod_grid();
    des_update_patches();
    sDesBlockCount = 0;
    for (i = 0; i < 250; i++) {
        if (sDesPatches_2[i].active != 0) {
            for (j = 0; j < sDesBlockCount; j++) {
                if (D_800ED6A8[j] == sDesPatches_2[i].x && D_800ED728[j] == sDesPatches_2[i].y) {
                    sDesPatches_2[i].slot = j;
                    break;
                }
            }
            if (j == sDesBlockCount) {
                D_800ED6A8[j] = sDesPatches_2[i].x;
                D_800ED728[j] = sDesPatches_2[i].y;
                mtx_from_angles_pos_scale(&sDesBlockMtx[gGfxFrameIndex][j], 0, 0, 0,
                              (sDesPatches_2[i].x - D_800EDFD4) * 0x1F400 - D_800EBDF0 + 64000,
                              -sDesCamY,
                              (sDesPatches_2[i].y - D_800EDFD8) * 0x1F400 - D_800EBDF4 + 64000,
                              0x100);
                sDesPatches_2[i].slot = sDesBlockCount;
                sDesBlockCount++;
            }
        }
    }
    sDesObjectCount = fractal_get_visible_objects(sDesCamX, sDesCamZ, gCamYaw_2, 0x1F400, sDesObjects, 0x10);
    for (i = 0; i < sDesObjectCount; i++) {
        mtx_from_angles_pos_scale(&sDesObjMtx[gGfxFrameIndex][i], 0, 0, 0,
                      sDesObjects[i].x - sDesCamX, sDesObjects[i].y - sDesCamY, sDesObjects[i].z - sDesCamZ, 0x10);
    }
    if (sDesArrowAngle >= 0) {
        mtx_from_angles_pos_scale(&m1, 0, 0, sDesArrowAngle, 0, 0, 0, 0x10);
        mtx_from_angles_pos_scale(&m2, -0x1000, 0, 0, -70, -55, 300, 10);
        mtx_mul(&m1, &m2, &sDesArrowMtx_2[gGfxFrameIndex]);
    }
    mtx_from_angles_pos_scale(&D_800ED610[gGfxFrameIndex], 0, 0, gCamYaw_2, 0, 0, 1000, 0x100);
}

typedef struct Patch {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    u8 vis;
    u8 dl;
    s16 bx;
    s16 by;
    s8 mtx;
    u8 pad1A1[7];
} Patch;

typedef struct TexSet_2 {
    s32 pad0;
    u16 *pal;
    s32 pad8;
    u8 *img;
} TexSet_2;

extern TexSet_2 *gTerrainTexSet_3;

Gfx *des_draw_near_terrain(Gfx *gdl, Mtx *m, s32 arg2, s32 arg3)
{
    s32 i, j;
    s16 id;
    Patch *p;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, 0x200004);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet_3->pal);
    D_800EBE00 = -1;
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet_3->img, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP, G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    for (j = 0; j < 1; j++) {
        for (i = 0; i < 0x400; i++) {
            id = sDesPatchGrid[j][i];
            if (id != -1) {
                p = (Patch *)(id * 0x1A8 + (s32)sDesPatches);
                if (p->vis == 1) {
                    if (p->mtx != D_800EBE00) {
                        gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][p->mtx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        D_800EBE00 = p->mtx;
                    }
                    gSPVertex(gdl++, p->v, 25, 0);
                    gSPDisplayList(gdl++, sDesPatchDLs[p->dl]);
                }
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 pad[0x58]; u16 fmin[4]; u16 fmax[4]; } FogCfg;
typedef struct { u32 pad0; void *pal; u32 pad8; void *tex; } TexSet_3;
typedef struct { u8 pad[0x19A]; u8 f19A; u8 f19B; u8 pad19C[4]; s8 f1A0; u8 pad1A1[7]; } Obj_33;
typedef struct { u8 pad[0x14]; s32 f14; u8 pad18[4]; } Ent1C_2;

extern TexSet_3 *gTerrainTexSet_4;
extern TexSet_3 *gObjectTexSet_5;
extern Ent1C_2 sDesObjects_2[];

Gfx *des_draw_terrain(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    FogCfg *fc;
    s32 i, k, j;
    s16 v;
    Obj_33 *o;

    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor_2.r, gFogColor_2.g, gFogColor_2.b, 255);
    fc = gCurTrack;
    gSPFogPosition(gdl++, fc->fmin[gNumViews - 1], fc->fmax[gNumViews - 1]);
    RAW(gdl++, 0xFC127E24, 0xFFFFF9FC);
    RAW(gdl++, 0xE200001C, 0xC8004030);
    D_800EBE00 = -1;
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet_4->pal);
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet_4->tex, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPViewport(gdl++, &gViewports[a3 + 4]);
    for (j = 0; j < sDesBlockCount; j++) {
        D_800EBE00 = 0;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x400; i++) {
                v = sDesPatchGrid[k][i];
                if (v != -1) {
                    o = (Obj_33 *)(v * sizeof(Obj_33) + (u32)sDesPatches);
                    if (o->f1A0 == j && (k != 0 || o->f19A == 0)) {
                        if (D_800EBE00 == 0) {
                            gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][j], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            D_800EBE00 = 1;
                        }
                        gSPVertex(gdl++, o, 25, 0);
                        { Gfx **dl = sDesPatchDLs; gSPDisplayList(gdl++, dl[o->f19B]); }
                    }
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    prim_set_tex_mode_callback((s32)D_80042718);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    RAW(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    RAW(gdl++, 0xE200001C, 0x00442230);
    gDPLoadTLUT_pal256(gdl++, gObjectTexSet_5->pal);
    for (i = 0; i < sDesObjectCount; i++) {
        gSPMatrix(gdl++, &sDesObjMtx[a2][i], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = prim_build_dl(0, sDesObjModels_2[sDesObjects_2[i].f14].f4, &gObjectTexSet_5, gdl);
    }
    prim_set_tex_mode_callback(0);
    gDPPipeSync(gdl++);
    return gdl;
}

#define PKT(p, a, b) { Gfx *_g = (Gfx *)(p++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u32 pad[5]; u32 f5; u32 f6; } Ent_40;

Gfx *des_draw_waypoint_marker(Gfx *gdl, s32 p2, s32 idx, s32 arg3)
{
    Gfx *w;
    u32 *g;
    s32 n;
    s32 i;
    Ent_40 *e;
    u32 *s1;
    u8 *s0;
    u32 *puVar3;
    u32 uVar9;
    u32 uVar1;
    u8 r, gg, b;
    u32 col;

    PKT(gdl, 0xD9DFFFFB, 0);
    PKT(gdl, 0xD9FEFFFF, 0);
    PKT(gdl, 0xE200001C, 0x00404A50);
    PKT(gdl, 0xE3001001, 0);
    PKT(gdl, 0xFC30B261, 0xFF67FFFF);

    n = sDesObjectCount;
    i = 0;
    if (n > 0) {
        w = gdl;
        do {
            u8 *q = (u8 *)sDesObjects + i * 0x1C;
            if (*(u32 *)(q + 0x14) == 2 && *(u32 *)(q + 0x18) == gDesWaypoint) {
                PKT(w, 0xDA380003, sDesObjMtx_2[idx] + i * 0x40);
                PKT(w, 0xDA380001, (u32)((u8 *)D_800ED610 + idx * 0x40));
                uVar9 = D_800CA3B8 % *(puVar3 = sDesMarkerAnim);
                uVar1 = uVar9 * 2 + puVar3[1];
                uVar1 = *(u16 *)uVar1;
                r = (uVar1 >> 11) << 3;
                gg = ((uVar1 >> 6) & 0x1F) << 3;
                uVar1 <<= 2;
                col = (r << 24) | (gg << 16);
                uVar1 &= 0xF8;
                col |= uVar1 << 8;
                PKT(w, 0xFA000000, col | 0xFF);
                PKT(w, 0xFD700000, *(u32 *)((u8 *)puVar3 + (uVar9 << 3) + 12));
                PKT(w, 0xF5700000, 0x07098250);
                PKT(w, 0xE6000000, 0);
                PKT(w, 0xF3000000, 0x073FF200);
                PKT(w, 0xE7000000, 0);
                PKT(w, 0xF5680800, 0x00098250);
                PKT(w, 0xF2000000, 0x0007C0FC);
                PKT(w, 0x01004008, (u32)sDesMarkerVtx);
                gdl = gdl + 12;
                PKT(w, 0x06000204, 0x406);
            }
            i = i + 1;
        } while (i < sDesObjectCount);
    }
    return gdl;
}

#define PKT_2(a, b) { Blk_2 *_g = gdl++; _g->w0 = (a); _g->w1 = (b); }

Blk_2 *des_draw_arrow(Blk_2 *gdl, s32 p2, s32 arg2, s32 arg3)
{
    s32 off;
    Blk_2 *puVar3;

    if (sDesArrowAngle < 0) {
        return gdl;
    }
    {
        PKT_2(0xFCFFFFFF, 0xFFFCF87C)
        PKT_2(0xE200001C, 0x0C084000)
        PKT_2(0xE3001001, 0x00008000)
        PKT_2(0xD7000002, 0x80008000)
        PKT_2(0xE3000C00, 0x00080000)
        PKT_2(0xD9FFFFFE, 0)
        PKT_2(0xFD100000, sDesArrowTex_2[1])
        PKT_2(0xE8000000, 0)
        PKT_2(0xF5000100, 0x07000000)
        PKT_2(0xE6000000, 0)
        PKT_2(0xF0000000, 0x073FC000)
        PKT_2(0xE7000000, 0)
        off = gGfxFrameIndex * 0x40;
        PKT_2(0xDA380003, (u32)(sDesArrowMtx + off))
        PKT_2(0xDA380007, (u32)(D_800E1938_2 + off))
        gdl = (Blk_2 *)prim_build_dl(0, sDesArrowModel, (s32)&sDesArrowTex_2, gdl);
        PKT_2(0xDA380003, p2 + 0x400)
        return gdl;
    }
}

#define G sDesLodGrid

void des_calc_lod_grid(void) {
    s32 i, j, a, k;
    s32 x, z, u, v, h;
    s32 s, c, n, xx;
    s32 d = 4000;
    s32 w, m;

    a = sDesCamX >> 4;
    h = sDesCamZ >> 4;
    D_800EA7C0 = xx = a / d;
    D_800ED7A8 = a - D_800EA7C0 * d + (((u32)a & 1) >> 1);
    w = D_800E8002;
    w >>= 4;
    s = gCosTable[w] + (((u32)&gCosTable[w] & 1) >> 1);
    c = gCosTable[(w - 0x400) & 0xFFF];
    if (D_800ED7A8 < 0) {
        D_800ED7A8 += d;
        D_800EA7C0 -= 1;
    }
    D_800EA7C4 = h / d;
    xx = D_800EA7C4 * d;
    D_800ED7B0 = h - xx + (((u32)h & 1) >> 1);
    if (D_800ED7B0 < 0) {
        D_800ED7B0 += d;
        D_800EA7C4 -= 1;
    }
    n = 0;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            G[n] = 0;
            x = ((j - 15) * 8000 - 4000) >> 4;
            k = ((u32)(D_800ED7A8 >> 31) & 1) >> 1;
            z = (i * 8000 - 0x1E460) >> 4;
            x -= D_800ED7A8 % 500;
            z -= D_800ED7B0 % 500;
            k = s * x;
            v = (s * z - c * x) >> 14;
            u = (k + c * z) >> 14;
            if (v < 5000) G[n] = 3;
            if (v < 4000) G[n] |= 4;
            if (i < 8 || i >= 24 || j < 8 || j >= 24) G[n] &= 0xFB;
            if (v < 1500) G[n] &= 0xF1;
            if (v >= -359 && u >= -359 && u < 360 && v < 360) {
                G[n] |= 1;
            } else if (v < 0) {
                G[n] = 0;
            } else {
                v = v / 2;
                if (u < 0) {
                    if (u + 360 < -v) G[n] &= 0xFE;
                    if (u + 720 < -v) G[n] &= 0xF1;
                } else {
                    if (v < u - 360) G[n] &= 0xFE;
                    if (v < u - 720) G[n] &= 0xF1;
                }
            }
            n++;
        }
    }
    D_800ED7A8 /= 500;
    D_800ED7B0 /= 500;
    D_800EDBB0 = D_800ED7A8 & 1;
    D_800EDBBC = D_800ED7B0 & 1;
    D_800EDBB8 = (D_800ED7A8 & 2) >> 1;
    D_800EDBC4 = (D_800ED7B0 & 2) >> 1;
    D_800EDBC0 = D_800ED7A8 & 4;
    D_800EDBC0 /= 4;
    D_800EDBC8 = D_800ED7B0 & 4;
    D_800EDBC8 /= 4;
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r, kb, kc;
            r = ((i / 2 + 8) << 5) + 8;
            m = r + j / 2;
            n = ((i - D_800EDBBC) << 5) + (j - D_800EDBB0);
            kb = ((i - D_800EDBC4) << 5) + (j - D_800EDBB8);
            kc = ((i - D_800EDBC8) << 5) + (j - D_800EDBC0);
            if (G[n] & G[n + 1] & G[n + 32] & G[n + 33] & 1) G[m] &= 0xFD;
            if (G[kb] & G[kb + 1] & G[kb + 32] & G[kb + 33] & 2) G[m] &= 0xFB;
            if (G[kc] & G[kc + 1] & G[kc + 32] & G[kc + 33] & 4) G[m] &= 0xF7;
            if (G[m] & 2) { G[n] &= 0xFE; G[n + 1] &= 0xFE; G[n + 32] &= 0xFE; G[n + 33] &= 0xFE; }
            if (G[m] & 4) { G[kb] &= 0xFD; G[kb + 1] &= 0xFD; G[kb + 32] &= 0xFD; G[kb + 33] &= 0xFD; }
            if (G[m] & 8) { G[kc] &= 0xFB; G[kc + 1] &= 0xFB; G[kc + 32] &= 0xFB; G[kc + 33] &= 0xFB; }
        }
    }
    mem_fill(sDesSeamFlags, 0, 0x400);
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r = (((i >> 1) + 8) << 5) + 8;
            m = r + (j >> 1);
            n = (i - D_800EDBBC) * 32 + (j - D_800EDBB0);
            if (sDesLodGrid[m + 32] & 2) {
                if (sDesLodGrid[n + 32] & 1) sDesSeamFlags[n + 32] |= 0x10;
                if (sDesLodGrid[n + 33] & 1) sDesSeamFlags[n + 33] |= 0x10;
            }
            if (sDesLodGrid[m - 32] & 2) {
                if (sDesLodGrid[n] & 1) sDesSeamFlags[n] |= 0x20;
                if (sDesLodGrid[n + 1] & 1) sDesSeamFlags[n + 1] |= 0x20;
            }
            if (sDesLodGrid[m + 1] & 2) {
                if (sDesLodGrid[n + 1] & 1) sDesSeamFlags[n + 1] |= 0x40;
                if (sDesLodGrid[n + 33] & 1) sDesSeamFlags[n + 33] |= 0x40;
            }
            if (sDesLodGrid[m - 1] & 2) {
                if (sDesLodGrid[n] & 1) sDesSeamFlags[n] |= 0x80;
                if (sDesLodGrid[n + 32] & 1) sDesSeamFlags[n + 32] |= 0x80;
            }
            n = (i - D_800EDBC4) * 32 + (j - D_800EDBB8);
            if (sDesLodGrid[m + 32] & 4) {
                if (sDesLodGrid[n + 32] & 2) sDesSeamFlags[n + 32] |= 1;
                if (sDesLodGrid[n + 33] & 2) sDesSeamFlags[n + 33] |= 1;
            }
            if (sDesLodGrid[m - 32] & 4) {
                if (sDesLodGrid[n] & 2) sDesSeamFlags[n] |= 2;
                if (sDesLodGrid[n + 1] & 2) sDesSeamFlags[n + 1] |= 2;
            }
            if (sDesLodGrid[m + 1] & 4) {
                if (sDesLodGrid[n + 1] & 2) sDesSeamFlags[n + 1] |= 4;
                if (sDesLodGrid[n + 33] & 2) sDesSeamFlags[n + 33] |= 4;
            }
            if (sDesLodGrid[m - 1] & 4) {
                if (sDesLodGrid[n] & 2) sDesSeamFlags[n] |= 8;
                if (sDesLodGrid[n + 32] & 2) sDesSeamFlags[n + 32] |= 8;
            }
        }
    }
}

#define REC(h) (&sDesPatches_3[h])

typedef struct { u8 pad[0x190]; s32 x; s32 y; s8 life; u8 p199; u8 near; u8 snd; u8 p19c[5]; u8 vis; u8 p1a2[6]; } Rec_23;

extern Rec_23 *sDesPatches_3;

static inline void divmod(s32 v, s32 d, s32 *q, s32 *r)
{
    *q = v / d;
    *r = v - *q * d;
    if (*r < 0) {
        *r += d;
        *q -= 1;
    }
}

void des_update_patches(void)
{
    s32 qx0, rx, qy0, ry, qx, qy;
    s32 i, j, h, s3, wx, wy, b, dr, dc;

    divmod(sDesCamX >> 4, 500, &qx0, &rx);
    divmod(sDesCamZ >> 4, 500, &qy0, &ry);

    for (i = 0; i < 3; i++) {
        j = 1 << i;
        D_800EA998_2[i] = ((qy0 / j) * 0x20 + qx0 / j) & 0x3ff;
    }
    D_800ED690 = qx0;
    D_800ED694 = qy0;

    for (i = 0; i < 250; i++) {
        if (REC(i)->life > 0)
            REC(i)->life--;
        REC(i)->vis = 0;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 0x400; j++) {
            h = sDesPatchGrid[i][j];
            if (h != -1 && REC(h)->life <= 0)
                sDesPatchGrid[i][j] = -1;
        }
    }

    sDesFreePatchCount = 0;
    for (i = 0; i < 250; i++) {
        if (REC(i)->life <= 0) {
            sDesFreePatches[sDesFreePatchCount++] = i;
        }
    }

    for (i = 0; i < 3; i++) {
        divmod(sDesCamX >> 4, 500 << i, &qx, &rx);
        divmod(sDesCamZ >> 4, 500 << i, &qy, &ry);
        s3 = D_800EA998_2[i];
        wy = (qy - 16) * (500 << i);
        for (j = 0; j < 0x20; j++) {
            s32 k;
            wx = (qx - 16) * (500 << i);
            for (k = 0; k < 0x20; k++) {
                if ((sDesLodGrid_2[0][j * 32 + k] >> i) & 1) {
                    s32 t = sDesPatchGrid[i][s3];
                    s32 n;
                    if (t == -1) {
                        if (sDesFreePatchCount == 0) goto out; h = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][s3] = h; des_build_patch(h, wx, wy, i);
                    } else {
                        h = t;
                        if (REC(h)->x != wx || REC(h)->y != wy) {
                            if (sDesFreePatchCount == 0) goto out; h = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][s3] = h; des_build_patch(h, wx, wy, i);
                        } else {
                            REC(h)->life = 2;
                        }
                    }
                    REC(h)->vis = 1;
                    dr = j - 16;
                    if (dr < 0)
                        dr = -dr;
                    dc = k - 16;
                    if (dc < 0)
                        dc = -dc;
                    if (dc + dr < 7)
                        REC(h)->near = 1;
                    else
                        REC(h)->near = 0;
                } else {
                    sDesPatchGrid[i][s3] = -1;
                }
                s3 = (s3 + 1) & 0x3ff;
                wx += 500 << i;
            }
            wy += 500 << i;
        }
    }
out:
    for (i = 0; i < 250; i++)
        REC(i)->snd = 0;

    for (i = 0; i < 2; i++) {
        s3 = D_800EA998_2[i];
        for (j = 0; j < 0x400; j++) {
            h = sDesPatchGrid[i][s3];
            if (h != -1) {
                Rec_23 *r = (Rec_23 *)(h * 0x1a8 + (u32)sDesPatches_3);
                u8 x = sDesSeamFlags[j];
                if (i == 0)
                    r->snd = x >> 4;
                else
                    r->snd = x & 0xf;
            }
            s3 = (s3 + 1) % 0x400;
        }
    }
}

typedef struct Patch_2 {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    s16 pad19A;
    s16 bx;
    s16 by;
    u8 pad1A0[8];
} Patch_2;

extern Patch_2 *sDesPatches_4;

static __inline__ void fdiv(s32 x, s32 d, s32 *q, s32 *r)
{
    *q = x / d;
    *r = x - *q * d;
    if (*r < 0) {
        *r += d;
        (*q)--;
    }
}

void des_build_patch(s32 n, s32 x, s32 y, s32 mode)
{
    s32 a, b, c, d;
    s32 tx, ty, px, pz, col, row, k;
    s32 h, s;
    Vtx *v;

    sDesPatches_4[n].x = x;
    sDesPatches_4[n].y = y;
    fdiv(x, 125, &a, &b);
    tx = a;
    fdiv(y, 125, &a, &b);
    ty = a;
    c = 0;
    d = 0;
    fdiv(x, 8000, &c, &a);
    x = a - 4000;
    fdiv(y, 8000, &d, &a);
    pz = a - 4000;
    k = 0;
    sDesPatches_4[n].mode = mode;
    sDesPatches_4[n].type = 2;
    sDesPatches_4[n].bx = c;
    sDesPatches_4[n].by = d;
    row = ty;
    for (a = 0; a < 5; a++) {
        s32 xx = x;
        col = tx;
        for (b = 0; b < 5; b++) {
            sDesPatches_4[n].v[k].v.ob[0] = xx;
            sDesPatches_4[n].v[k].v.ob[1] = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)] * 4;
            sDesPatches_4[n].v[k].v.ob[2] = pz;
            s = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)];
            s = (gDesertHeightMap[(((row + 1) & 0x1FF) << 9) | (col & 0x1FF)] - s) * 8 + 0x80;
            if (s < 0) s = 0;
            if (s > 255) s = 255;
            sDesPatches_4[n].v[k].v.cn[0] = s * 245 / 256 + 10;
            sDesPatches_4[n].v[k].v.cn[1] = s * 230 / 256 + 25;
            sDesPatches_4[n].v[k].v.cn[2] = s * 170 / 256 + 85;
            sDesPatches_4[n].v[k].v.tc[0] = (b << 10) << mode;
            sDesPatches_4[n].v[k].v.tc[1] = (a << 10) << mode;
            sDesPatches_4[n].v[k].v.cn[3] = 255;
            k++;
            xx += 125 << mode;
            col += 1 << mode;
        }
        pz += 125 << mode;
        row += 1 << mode;
    }
}

s32 des_build_patch_dl(s32 mask)
{
    Gfx *gdl = gZBuffer;
    s32 i, j, k, b, c;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            k = i * 6 - j * 4;
            gSP2Triangles(gdl++, k + 10, k + 11, k + 16, 0, k + 10, k + 6, k + 11, 0);
            gSP2Triangles(gdl++, k + 11, k + 12, k + 16, 0, k + 11, k + 6, k + 12, 0);
        }
    }

    for (i = 0; i < 4; i++) {
        if ((mask >> i) & 1) {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + 2 * b, 0, j + 2 * b, j + 3 * b + c, j + 4 * b, 0);
        } else {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + b, 0, j + b, j + b + c, j + 2 * b, 0);
            gSP2Triangles(gdl++, j + 2 * b, j + 3 * b + c, j + 3 * b, 0, j + 3 * b, j + 3 * b + c, j + 4 * b, 0);
        }
    }

    gSPEndDisplayList(gdl++);
    i = (u8 *)gdl - (u8 *)gZBuffer;
    gdl = mem_alloc_dbg(i, 0, (s32)D_800D6FD0, 0x508);
    memcpy(gdl, gZBuffer, i);
    return (s32)gdl;
}

void des_free(void)
{
    s32 i;
    s32 *p;

    mem_free(sDesPatches);
    mem_free(gDesertHeightMap);
    mem_free(sDesObjMtx[0]);
    mem_free(sDesObjMtx[1]);
    i = 0;
    mem_free(gDesertTiles);
    mem_free(sDesArrowMtx);
    p = sDesPatchDLs;
    do {
        s32 v = *p;
        p++;
        i++;
        mem_free(v);
    } while (i < 16);
}

void des_set_arrow_angle(s32 a0)
{
    sDesArrowAngle = a0;
}

void divmod_floor(s32 a, s32 b, s32 *q, s32 *r)
{
    s32 quo = a / b;
    s32 rem;
    *q = quo;
    rem = a - quo * b;
    *r = rem;
    if (rem < 0) {
        *r = rem + b;
        *q = *q - 1;
    }
}

void des_bbox_add(s32 param_1, s32 param_2)
{
    if (param_1 < D_800ED7B4) {
        D_800ED7B4 = param_1;
    }
    if (param_2 < D_800ED7B8) {
        D_800ED7B8 = param_2;
    }
    if (D_800ED7BC < param_1) {
        D_800ED7BC = param_1;
    }
    if (D_800ED7C0 < param_2) {
        D_800ED7C0 = param_2;
    }
}

void des_bbox_reset(void)
{
    D_800ED7B8 = 999;
    D_800ED7B4 = 999;
    D_800ED7C0 = -999;
    D_800ED7BC = -999;
}

#define PKT_3(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *des_tex_mode_callback(Gfx *gdl, s32 x)
{
    Gfx *p0;
    Gfx *p1;

    if (x != 0) {
        p0 = gdl++;
        p1 = gdl++;
        p0->words.w0 = 0xE7000000;
        p0->words.w1 = 0;
        p1->words.w0 = 0xFCFFFFFF;
        p1->words.w1 = 0xFFFCF279;
        PKT_3(0xE200001C, 0x230);
        PKT_3(0xF9000000, 0xFE);
        PKT_3(0xE2001E01, 1);
    } else {
        PKT_3(0xE7000000, 0);
        PKT_3(0xE2001E01, 0);
        PKT_3(0xFCFFFFFF, 0xFFFCF87C);
        PKT_3(0xE200001C, 0x442230);
    }
    return gdl;
}

void des_set_waypoint(s32 x)
{
    gDesWaypoint = x;
}
