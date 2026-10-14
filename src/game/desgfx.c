#include "game.h"

void des_init(void)
{
    s32 i;

    gFarClip = 0x4000;
    gTerrainTexSet = res_load(gResourceSet, sDesTerrainTexResId);
    for (i = 0; sDesObjModels[i].res != NULL; i++) {
        sDesObjModels[i].model = (PrimModel *)res_load(gResourceSet, *sDesObjModels[i].res);
    }
    gObjectTexSet = res_load(gResourceSet, sDesObjTexResId);
    sDesPatches = mem_alloc_dbg(250 * sizeof(DesPatch), 0, sStrDesgfxC, 0xe9);
    for (i = 0; i < 250; i++) {
        sDesPatches[i].life = 0;
    }
    gDesertHeightMap = mem_alloc_dbg(0x40000, 0, sStrDesgfxC, 0xed);
    fractal_init((u8 *)gDesertHeightMap);
    fractal_reset();
    for (i = 0; i < 16; i++) {
        sDesPatchDLs[i] = des_build_patch_dl(i);
    }
    mem_fill_2(sDesPatchGrid, -1, sizeof(sDesPatchGrid));
    mem_fill_2(sDesPatchGridOrigin, 0, sizeof(sDesPatchGridOrigin));
    sDesFreePatchCount = 250;
    for (i = 249; i >= 0; i--) {
        sDesFreePatches[i] = i;
    }
    sDesObjMtx[0] = mem_alloc_dbg(16 * sizeof(Mtx), 0, sStrDesgfxC, 0xfe);
    sDesObjMtx[1] = mem_alloc_dbg(16 * sizeof(Mtx), 0, sStrDesgfxC, 0xff);
    sDesArrowTex = (TexSet *)res_load(gResourceSet, sDesArrowTexResId);
    sDesArrowModel = (PrimModel *)res_load(gResourceSet, sDesArrowModelResId);
    sDesArrowMtx = mem_alloc_dbg(2 * sizeof(Mtx), 0, sStrDesgfxC, 0x104);
    sDesMarkerAnim = (TexSet *)res_load(gResourceSet, sDesMarkerAnimResId);
    D_800EA98C = res_load(gResourceSet, D_800D0BC8);
}

void des_update_view(s32 arg0)
{
    s32 i;
    s32 j;
    Mtx m1;
    Mtx m2;

    sDesMarkerFrame = gRaceFrame / 4;
    fractal_generate_step(100);
    sDesCamX = gCamPos[0].x;
    sDesCamY = gCamPos[0].y;
    sDesCamZ = gCamPos[0].z;
    divmod_floor(sDesCamX, 0x1F400, &sDesCamBlockX, &sDesCamBlockOffX);
    divmod_floor(sDesCamZ, 0x1F400, &sDesCamBlockZ, &sDesCamBlockOffZ);
    fractal_update_tiles(sDesCamX, sDesCamZ);
    des_calc_lod_grid();
    des_update_patches();
    sDesBlockCount = 0;
    for (i = 0; i < 250; i++) {
        if (sDesPatches[i].visible != 0) {
            for (j = 0; j < sDesBlockCount; j++) {
                if (gDesBlockX[j] == sDesPatches[i].blockX && gDesBlockZ[j] == sDesPatches[i].blockZ) {
                    sDesPatches[i].block = j;
                    break;
                }
            }
            if (j == sDesBlockCount) {
                gDesBlockX[j] = sDesPatches[i].blockX;
                gDesBlockZ[j] = sDesPatches[i].blockZ;
                mtx_from_angles_pos_scale(&sDesBlockMtx[gGfxFrameIndex][j], 0, 0, 0,
                              (sDesPatches[i].blockX - sDesCamBlockX) * 0x1F400 - sDesCamBlockOffX + 64000,
                              -sDesCamY,
                              (sDesPatches[i].blockZ - sDesCamBlockZ) * 0x1F400 - sDesCamBlockOffZ + 64000,
                              0x100);
                sDesPatches[i].block = sDesBlockCount;
                sDesBlockCount++;
            }
        }
    }
    sDesObjectCount = fractal_get_visible_objects(sDesCamX, sDesCamZ, gCamYaw[0], 0x1F400, sDesObjects, 16);
    for (i = 0; i < sDesObjectCount; i++) {
        mtx_from_angles_pos_scale(&sDesObjMtx[gGfxFrameIndex][i], 0, 0, 0,
                      sDesObjects[i].x - sDesCamX, sDesObjects[i].y - sDesCamY, sDesObjects[i].z - sDesCamZ, 0x10);
    }
    if (sDesArrowAngle >= 0) {
        mtx_from_angles_pos_scale(&m1, 0, 0, sDesArrowAngle, 0, 0, 0, 0x10);
        mtx_from_angles_pos_scale(&m2, -0x1000, 0, 0, -70, -55, 300, 10);
        mtx_mul(&m1, &m2, &sDesArrowMtx[gGfxFrameIndex]);
    }
    mtx_from_angles_pos_scale(&sDesMarkerMtx[gGfxFrameIndex], 0, 0, gCamYaw[0], 0, 0, 1000, 0x100);
}

Gfx *des_draw_near_terrain(Gfx *gdl, Mtx *m, s32 arg2, s32 arg3)
{
    s32 i, j;
    s16 id;
    DesPatch *p;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, 0x200004);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    sDesCurBlock = -1;
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet->tex[0].data, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP, G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    for (j = 0; j < 1; j++) {
        for (i = 0; i < 0x400; i++) {
            id = sDesPatchGrid[j][i];
            if (id != -1) {
                p = (DesPatch *)(id * sizeof(DesPatch) + (s32)sDesPatches);
                if (p->near == 1) {
                    if (p->block != sDesCurBlock) {
                        gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][p->block], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        sDesCurBlock = p->block;
                    }
                    gSPVertex(gdl++, p->vtx, 25, 0);
                    gSPDisplayList(gdl++, sDesPatchDLs[p->seams]);
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

Gfx *des_draw_terrain(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    FogCfg *fc;
    s32 i, k, j;
    s16 v;
    DesPatch *o;

    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    fc = gCurTrack;
    gSPFogPosition(gdl++, fc->fmin[gNumViews - 1], fc->fmax[gNumViews - 1]);
    RAW(gdl++, 0xFC127E24, 0xFFFFF9FC);
    RAW(gdl++, 0xE200001C, 0xC8004030);
    sDesCurBlock = -1;
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet->tex[0].data, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPViewport(gdl++, &gViewports[a3 + 4]);
    for (j = 0; j < sDesBlockCount; j++) {
        sDesCurBlock = 0;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x400; i++) {
                v = sDesPatchGrid[k][i];
                if (v != -1) {
                    o = (DesPatch *)(v * sizeof(DesPatch) + (u32)sDesPatches);
                    if (o->block == j && (k != 0 || o->near == 0)) {
                        if (sDesCurBlock == 0) {
                            gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][j], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            sDesCurBlock = 1;
                        }
                        gSPVertex(gdl++, o->vtx, 25, 0);
                        { Gfx **dl = sDesPatchDLs; gSPDisplayList(gdl++, dl[o->seams]); }
                    }
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    prim_set_tex_mode_callback((s32)des_tex_mode_callback);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    RAW(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    RAW(gdl++, 0xE200001C, 0x00442230);
    gDPLoadTLUT_pal256(gdl++, gObjectTexSet->pal);
    for (i = 0; i < sDesObjectCount; i++) {
        gSPMatrix(gdl++, &sDesObjMtx[a2][i], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = prim_build_dl(0, sDesObjModels[sDesObjects[i].kind].model, &gObjectTexSet, gdl);
    }
    prim_set_tex_mode_callback(0);
    gDPPipeSync(gdl++);
    return gdl;
}

#define PKT(p, a, b) { Gfx *_g = (Gfx *)(p++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *des_draw_waypoint_marker(Gfx *gdl, s32 p2, s32 idx, s32 arg3)
{
    Gfx *w;
    s32 n;
    s32 i;
    TexSet *anim;
    u32 frame;
    u32 c;
    u8 r, g;
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
            /* the campfire of the next checkpoint */
            if (sDesObjects[i].kind == 2 && sDesObjects[i].ref == gDesWaypoint) {
                PKT(w, 0xDA380003, (u32)&sDesObjMtx[idx][i]);
                PKT(w, 0xDA380001, (u32)&sDesMarkerMtx[idx]);
                /* each frame of the animation has its colour in the palette */
                frame = sDesMarkerFrame % (anim = sDesMarkerAnim)->count;
                c = frame * 2 + (u32)anim->pal;
                c = *(u16 *)c;
                r = (c >> 11) << 3;
                g = ((c >> 6) & 0x1F) << 3;
                c <<= 2;
                col = (r << 24) | (g << 16);
                c &= 0xF8;
                col |= c << 8;
                PKT(w, 0xFA000000, col | 0xFF);
                PKT(w, 0xFD700000, (u32)anim->tex[frame].data);
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
        PKT_2(0xFD100000, (u32)sDesArrowTex->pal)
        PKT_2(0xE8000000, 0)
        PKT_2(0xF5000100, 0x07000000)
        PKT_2(0xE6000000, 0)
        PKT_2(0xF0000000, 0x073FC000)
        PKT_2(0xE7000000, 0)
        off = gGfxFrameIndex * 0x40;
        PKT_2(0xDA380003, (u32)((u8 *)sDesArrowMtx + off))
        PKT_2(0xDA380007, (u32)((u8 *)gHudPerspMtx + off))
        gdl = (Blk_2 *)prim_build_dl(0, sDesArrowModel, &sDesArrowTex, (Gfx *)gdl);
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
    sDesLodCellX = xx = a / d;
    sDesLodOffX = a - sDesLodCellX * d + (((u32)a & 1) >> 1);
    w = (u16)gCamYaw[0];
    w >>= 4;
    s = gCosTable[w] + (((u32)&gCosTable[w] & 1) >> 1);
    c = gCosTable[(w - 0x400) & 0xFFF];
    if (sDesLodOffX < 0) {
        sDesLodOffX += d;
        sDesLodCellX -= 1;
    }
    sDesLodCellZ = h / d;
    xx = sDesLodCellZ * d;
    sDesLodOffZ = h - xx + (((u32)h & 1) >> 1);
    if (sDesLodOffZ < 0) {
        sDesLodOffZ += d;
        sDesLodCellZ -= 1;
    }
    n = 0;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            G[n] = 0;
            x = ((j - 15) * 8000 - 4000) >> 4;
            k = ((u32)(sDesLodOffX >> 31) & 1) >> 1;
            z = (i * 8000 - 0x1E460) >> 4;
            x -= sDesLodOffX % 500;
            z -= sDesLodOffZ % 500;
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
    sDesLodOffX /= 500;
    sDesLodOffZ /= 500;
    sDesLod0ShiftX = sDesLodOffX & 1;
    sDesLod0ShiftZ = sDesLodOffZ & 1;
    sDesLod1ShiftX = (sDesLodOffX & 2) >> 1;
    sDesLod1ShiftZ = (sDesLodOffZ & 2) >> 1;
    sDesLod2ShiftX = sDesLodOffX & 4;
    sDesLod2ShiftX /= 4;
    sDesLod2ShiftZ = sDesLodOffZ & 4;
    sDesLod2ShiftZ /= 4;
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r, kb, kc;
            r = ((i / 2 + 8) << 5) + 8;
            m = r + j / 2;
            n = ((i - sDesLod0ShiftZ) << 5) + (j - sDesLod0ShiftX);
            kb = ((i - sDesLod1ShiftZ) << 5) + (j - sDesLod1ShiftX);
            kc = ((i - sDesLod2ShiftZ) << 5) + (j - sDesLod2ShiftX);
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
            n = (i - sDesLod0ShiftZ) * 32 + (j - sDesLod0ShiftX);
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
            n = (i - sDesLod1ShiftZ) * 32 + (j - sDesLod1ShiftX);
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
    s32 qx0, rx, qz0, rz, qx, qz;
    s32 i, j, h, s3, wx, wz, b, dr, dc;

    divmod(sDesCamX >> 4, 500, &qx0, &rx);
    divmod(sDesCamZ >> 4, 500, &qz0, &rz);

    for (i = 0; i < 3; i++) {
        j = 1 << i;
        sDesPatchGridOrigin[i] = ((qz0 / j) * 0x20 + qx0 / j) & 0x3ff;
    }
    sDesCamCellX = qx0;
    sDesCamCellZ = qz0;

    for (i = 0; i < 250; i++) {
        if (sDesPatches[i].life > 0)
            sDesPatches[i].life--;
        sDesPatches[i].visible = 0;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 0x400; j++) {
            h = sDesPatchGrid[i][j];
            if (h != -1 && sDesPatches[h].life <= 0)
                sDesPatchGrid[i][j] = -1;
        }
    }

    sDesFreePatchCount = 0;
    for (i = 0; i < 250; i++) {
        if (sDesPatches[i].life <= 0) {
            sDesFreePatches[sDesFreePatchCount++] = i;
        }
    }

    for (i = 0; i < 3; i++) {
        divmod(sDesCamX >> 4, 500 << i, &qx, &rx);
        divmod(sDesCamZ >> 4, 500 << i, &qz, &rz);
        s3 = sDesPatchGridOrigin[i];
        wz = (qz - 16) * (500 << i);
        for (j = 0; j < 0x20; j++) {
            s32 k;
            wx = (qx - 16) * (500 << i);
            for (k = 0; k < 0x20; k++) {
                if ((sDesLodGrid[j * 32 + k] >> i) & 1) {
                    s32 t = sDesPatchGrid[i][s3];
                    s32 n;
                    if (t == -1) {
                        if (sDesFreePatchCount == 0) goto out; h = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][s3] = h; des_build_patch(h, wx, wz, i);
                    } else {
                        h = t;
                        if (sDesPatches[h].x != wx || sDesPatches[h].z != wz) {
                            if (sDesFreePatchCount == 0) goto out; h = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][s3] = h; des_build_patch(h, wx, wz, i);
                        } else {
                            sDesPatches[h].life = 2;
                        }
                    }
                    sDesPatches[h].visible = 1;
                    dr = j - 16;
                    if (dr < 0)
                        dr = -dr;
                    dc = k - 16;
                    if (dc < 0)
                        dc = -dc;
                    if (dc + dr < 7)
                        sDesPatches[h].near = 1;
                    else
                        sDesPatches[h].near = 0;
                } else {
                    sDesPatchGrid[i][s3] = -1;
                }
                s3 = (s3 + 1) & 0x3ff;
                wx += 500 << i;
            }
            wz += 500 << i;
        }
    }
out:
    for (i = 0; i < 250; i++)
        sDesPatches[i].seams = 0;

    for (i = 0; i < 2; i++) {
        s3 = sDesPatchGridOrigin[i];
        for (j = 0; j < 0x400; j++) {
            h = sDesPatchGrid[i][s3];
            if (h != -1) {
                DesPatch *r = (DesPatch *)(h * sizeof(DesPatch) + (u32)sDesPatches);
                u8 x = sDesSeamFlags[j];
                if (i == 0)
                    r->seams = x >> 4;
                else
                    r->seams = x & 0xf;
            }
            s3 = (s3 + 1) % 0x400;
        }
    }
}

static __inline__ void fdiv(s32 x, s32 d, s32 *q, s32 *r)
{
    *q = x / d;
    *r = x - *q * d;
    if (*r < 0) {
        *r += d;
        (*q)--;
    }
}

void des_build_patch(s32 n, s32 x, s32 z, s32 lod)
{
    s32 a, b, c, d;
    s32 tx, tz, px, pz, col, row, k;
    s32 h, s;
    Vtx *v;

    sDesPatches[n].x = x;
    sDesPatches[n].z = z;
    fdiv(x, 125, &a, &b);
    tx = a;
    fdiv(z, 125, &a, &b);
    tz = a;
    c = 0;
    d = 0;
    fdiv(x, 8000, &c, &a);
    x = a - 4000;
    fdiv(z, 8000, &d, &a);
    pz = a - 4000;
    k = 0;
    sDesPatches[n].lod = lod;
    sDesPatches[n].life = 2;
    sDesPatches[n].blockX = c;
    sDesPatches[n].blockZ = d;
    row = tz;
    for (a = 0; a < 5; a++) {
        s32 xx = x;
        col = tx;
        for (b = 0; b < 5; b++) {
            sDesPatches[n].vtx[k].v.ob[0] = xx;
            sDesPatches[n].vtx[k].v.ob[1] = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)] * 4;
            sDesPatches[n].vtx[k].v.ob[2] = pz;
            s = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)];
            s = (gDesertHeightMap[(((row + 1) & 0x1FF) << 9) | (col & 0x1FF)] - s) * 8 + 0x80;
            if (s < 0) s = 0;
            if (s > 255) s = 255;
            sDesPatches[n].vtx[k].v.cn[0] = s * 245 / 256 + 10;
            sDesPatches[n].vtx[k].v.cn[1] = s * 230 / 256 + 25;
            sDesPatches[n].vtx[k].v.cn[2] = s * 170 / 256 + 85;
            sDesPatches[n].vtx[k].v.tc[0] = (b << 10) << lod;
            sDesPatches[n].vtx[k].v.tc[1] = (a << 10) << lod;
            sDesPatches[n].vtx[k].v.cn[3] = 255;
            k++;
            xx += 125 << lod;
            col += 1 << lod;
        }
        pz += 125 << lod;
        row += 1 << lod;
    }
}

Gfx *des_build_patch_dl(s32 mask)
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
            j = sDesSeamStartVtx[i];
            b = sDesSeamStep[i];
            c = sDesSeamInward[i];
            gSP2Triangles(gdl++, j, j + b + c, j + 2 * b, 0, j + 2 * b, j + 3 * b + c, j + 4 * b, 0);
        } else {
            j = sDesSeamStartVtx[i];
            b = sDesSeamStep[i];
            c = sDesSeamInward[i];
            gSP2Triangles(gdl++, j, j + b + c, j + b, 0, j + b, j + b + c, j + 2 * b, 0);
            gSP2Triangles(gdl++, j + 2 * b, j + 3 * b + c, j + 3 * b, 0, j + 3 * b, j + 3 * b + c, j + 4 * b, 0);
        }
    }

    gSPEndDisplayList(gdl++);
    i = (u8 *)gdl - (u8 *)gZBuffer;
    gdl = mem_alloc_dbg(i, 0, (s32)sStrDesgfxC, 0x508);
    memcpy(gdl, gZBuffer, i);
    return gdl;
}

void des_free(void)
{
    s32 i;
    Gfx **p;

    mem_free(sDesPatches);
    mem_free(gDesertHeightMap);
    mem_free(sDesObjMtx[0]);
    mem_free(sDesObjMtx[1]);
    i = 0;
    mem_free(gDesertTiles);
    mem_free(sDesArrowMtx);
    p = sDesPatchDLs;
    do {
        Gfx *v = *p;
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
    if (param_1 < sDesBBoxMinX) {
        sDesBBoxMinX = param_1;
    }
    if (param_2 < sDesBBoxMinZ) {
        sDesBBoxMinZ = param_2;
    }
    if (sDesBBoxMaxX < param_1) {
        sDesBBoxMaxX = param_1;
    }
    if (sDesBBoxMaxZ < param_2) {
        sDesBBoxMaxZ = param_2;
    }
}

void des_bbox_reset(void)
{
    sDesBBoxMinZ = 999;
    sDesBBoxMinX = 999;
    sDesBBoxMaxZ = -999;
    sDesBBoxMaxX = -999;
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
