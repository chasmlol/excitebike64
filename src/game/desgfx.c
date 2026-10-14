#include "game.h"

/* Loads the desert: textures, object models, the patch cache, the height map and its tiles, the 16 seam display
   lists, the arrow and the checkpoint marker. */
void des_init(void)
{
    s32 i;

    gFarClip = 0x4000;
    gTerrainTexSet = res_load(gResourceSet, sDesTerrainTexResId);
    for (i = 0; sDesObjModels[i].res != NULL; i++) {
        sDesObjModels[i].model = (PrimModel *)res_load(gResourceSet, *sDesObjModels[i].res);
    }
    gObjectTexSet = res_load(gResourceSet, sDesObjTexResId);
    sDesPatches = mem_alloc_dbg(DES_NUM_PATCHES * sizeof(DesPatch), 0, sStrDesgfxC, 0xe9);
    for (i = 0; i < DES_NUM_PATCHES; i++) {
        sDesPatches[i].life = 0;
    }
    gDesertHeightMap = mem_alloc_dbg(DESERT_MAP_SIZE * DESERT_MAP_SIZE, 0, sStrDesgfxC, 0xed);
    fractal_init((u8 *)gDesertHeightMap);
    fractal_reset();
    for (i = 0; i < 16; i++) { /* one per combination of seams */
        sDesPatchDLs[i] = des_build_patch_dl(i);
    }
    mem_fill_2(sDesPatchGrid, -1, sizeof(sDesPatchGrid));
    mem_fill_2(sDesPatchGridOrigin, 0, sizeof(sDesPatchGridOrigin));
    sDesFreePatchCount = DES_NUM_PATCHES;
    for (i = DES_NUM_PATCHES - 1; i >= 0; i--) {
        sDesFreePatches[i] = i;
    }
    sDesObjMtx[0] = mem_alloc_dbg(DES_MAX_OBJECTS * sizeof(Mtx), 0, sStrDesgfxC, 0xfe);
    sDesObjMtx[1] = mem_alloc_dbg(DES_MAX_OBJECTS * sizeof(Mtx), 0, sStrDesgfxC, 0xff);
    sDesArrowTex = (TexSet *)res_load(gResourceSet, sDesArrowTexResId);
    sDesArrowModel = (PrimModel *)res_load(gResourceSet, sDesArrowModelResId);
    sDesArrowMtx = mem_alloc_dbg(2 * sizeof(Mtx), 0, sStrDesgfxC, 0x104);
    sDesMarkerAnim = (TexSet *)res_load(gResourceSet, sDesMarkerAnimResId);
    D_800EA98C = res_load(gResourceSet, D_800D0BC8);
}

/* A frame of the desert: generates more terrain, picks the patches of each ring around the camera, makes the
   matrices of the blocks they lie in and of the visible objects, the arrow and the marker. */
void des_update_view(s32 view)
{
    s32 i;
    s32 j;
    Mtx rot;
    Mtx place;

    sDesMarkerFrame = gRaceFrame / 4;
    fractal_generate_step(100);
    sDesCamX = gCamPos[0].x;
    sDesCamY = gCamPos[0].y;
    sDesCamZ = gCamPos[0].z;
    divmod_floor(sDesCamX, DES_BLOCK_MM, &sDesCamBlockX, &sDesCamBlockOffX);
    divmod_floor(sDesCamZ, DES_BLOCK_MM, &sDesCamBlockZ, &sDesCamBlockOffZ);
    fractal_update_tiles(sDesCamX, sDesCamZ);
    des_calc_lod_grid();
    des_update_patches();
    sDesBlockCount = 0;
    for (i = 0; i < DES_NUM_PATCHES; i++) {
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
                              (sDesPatches[i].blockX - sDesCamBlockX) * DES_BLOCK_MM - sDesCamBlockOffX + 64000,
                              -sDesCamY,
                              (sDesPatches[i].blockZ - sDesCamBlockZ) * DES_BLOCK_MM - sDesCamBlockOffZ + 64000,
                              0x100);
                sDesPatches[i].block = sDesBlockCount;
                sDesBlockCount++;
            }
        }
    }
    sDesObjectCount = fractal_get_visible_objects(sDesCamX, sDesCamZ, gCamYaw[0], 128000, sDesObjects, DES_MAX_OBJECTS);
    for (i = 0; i < sDesObjectCount; i++) {
        mtx_from_angles_pos_scale(&sDesObjMtx[gGfxFrameIndex][i], 0, 0, 0,
                      sDesObjects[i].x - sDesCamX, sDesObjects[i].y - sDesCamY, sDesObjects[i].z - sDesCamZ, 0x10);
    }
    if (sDesArrowAngle >= 0) {
        /* the arrow turned to the checkpoint, tilted, at the bottom left of the view */
        mtx_from_angles_pos_scale(&rot, 0, 0, sDesArrowAngle, 0, 0, 0, 0x10);
        mtx_from_angles_pos_scale(&place, -0x1000, 0, 0, -70, -55, 300, 10);
        mtx_mul(&rot, &place, &sDesArrowMtx[gGfxFrameIndex]);
    }
    mtx_from_angles_pos_scale(&sDesMarkerMtx[gGfxFrameIndex], 0, 0, gCamYaw[0], 0, 0, 1000, 0x100);
}

/* Draws the patches of the inner ring close to the camera, without fog. */
Gfx *des_draw_near_terrain(Gfx *gdl, Mtx *dyn, s32 fb, s32 view)
{
    s32 i, j;
    s16 id;
    DesPatch *patch;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    sDesCurBlock = -1;
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet->tex[0].data, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP, G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    for (j = 0; j < 1; j++) {
        for (i = 0; i < DES_GRID_SIZE * DES_GRID_SIZE; i++) {
            id = sDesPatchGrid[j][i];
            if (id != -1) {
                patch = (DesPatch *)(id * sizeof(DesPatch) + (s32)sDesPatches);
                if (patch->near == 1) {
                    if (patch->block != sDesCurBlock) {
                        gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][patch->block], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        sDesCurBlock = patch->block;
                    }
                    gSPVertex(gdl++, patch->vtx, 25, 0);
                    gSPDisplayList(gdl++, sDesPatchDLs[patch->seams]);
                }
            }
        }
    }
    gSPMatrix(gdl++, &dyn[16] /* Dyn.m4 */, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

/* gCurTrack as this file sees it: the fog range per number of views */
typedef struct { u8 pad[0x58]; u16 fmin[4]; u16 fmax[4]; } FogCfg;

/* Draws the rest of the terrain, block by block with fog and the far viewport, then the visible objects. */
Gfx *des_draw_terrain(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    FogCfg *track;
    s32 i, ring, j;
    s16 id;
    DesPatch *patch;

    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    track = gCurTrack;
    gSPFogPosition(gdl++, track->fmin[gNumViews - 1], track->fmax[gNumViews - 1]);
    RAW(gdl++, 0xFC127E24, 0xFFFFF9FC);
    RAW(gdl++, 0xE200001C, 0xC8004030);
    sDesCurBlock = -1;
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    gDPLoadTextureBlock_4b(gdl++, gTerrainTexSet->tex[0].data, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPViewport(gdl++, &gViewports[view + 4]);
    for (j = 0; j < sDesBlockCount; j++) {
        sDesCurBlock = 0;
        for (ring = 0; ring < DES_NUM_RINGS; ring++) {
            for (i = 0; i < DES_GRID_SIZE * DES_GRID_SIZE; i++) {
                id = sDesPatchGrid[ring][i];
                if (id != -1) {
                    patch = (DesPatch *)(id * sizeof(DesPatch) + (u32)sDesPatches);
                    if (patch->block == j && (ring != 0 || patch->near == 0)) {
                        if (sDesCurBlock == 0) {
                            gSPMatrix(gdl++, &sDesBlockMtx[gGfxFrameIndex][j], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            sDesCurBlock = 1;
                        }
                        gSPVertex(gdl++, patch->vtx, 25, 0);
                        { Gfx **dl = sDesPatchDLs; gSPDisplayList(gdl++, dl[patch->seams]); }
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
        gSPMatrix(gdl++, &sDesObjMtx[fb][i], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = prim_build_dl(0, sDesObjModels[sDesObjects[i].kind].model, &gObjectTexSet, gdl);
    }
    prim_set_tex_mode_callback(0);
    gDPPipeSync(gdl++);
    return gdl;
}

#define PKT(p, a, b) { Gfx *_g = (Gfx *)(p++); _g->words.w0 = (a); _g->words.w1 = (b); }

/* Draws the animated marker over the campfire of the next checkpoint, tinted by the frame's palette colour. */
Gfx *des_draw_waypoint_marker(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    Gfx *p;
    s32 count;
    s32 i;
    TexSet *anim;
    u32 frame;
    u32 pix;
    u8 r, g;
    u32 col;

    PKT(gdl, 0xD9DFFFFB, 0);
    PKT(gdl, 0xD9FEFFFF, 0);
    PKT(gdl, 0xE200001C, 0x00404A50);
    PKT(gdl, 0xE3001001, 0);
    PKT(gdl, 0xFC30B261, 0xFF67FFFF);

    count = sDesObjectCount;
    i = 0;
    if (count > 0) {
        p = gdl;
        do {
            /* the campfire of the next checkpoint */
            if (sDesObjects[i].kind == DESERT_OBJ_CAMPFIRE && sDesObjects[i].ref == gDesWaypoint) {
                PKT(p, 0xDA380003, (u32)&sDesObjMtx[fb][i]);
                PKT(p, 0xDA380001, (u32)&sDesMarkerMtx[fb]);
                /* each frame of the animation has its colour in the palette */
                frame = sDesMarkerFrame % (anim = sDesMarkerAnim)->count;
                pix = frame * 2 + (u32)anim->pal;
                pix = *(u16 *)pix;
                r = (pix >> 11) << 3;
                g = ((pix >> 6) & 0x1F) << 3;
                pix <<= 2;
                col = (r << 24) | (g << 16);
                pix &= 0xF8;
                col |= pix << 8;
                PKT(p, 0xFA000000, col | 0xFF);
                PKT(p, 0xFD700000, (u32)anim->tex[frame].data);
                PKT(p, 0xF5700000, 0x07098250);
                PKT(p, 0xE6000000, 0);
                PKT(p, 0xF3000000, 0x073FF200);
                PKT(p, 0xE7000000, 0);
                PKT(p, 0xF5680800, 0x00098250);
                PKT(p, 0xF2000000, 0x0007C0FC);
                PKT(p, 0x01004008, (u32)sDesMarkerVtx);
                gdl = gdl + 12; /* the 12 packets written through p */
                PKT(p, 0x06000204, 0x406);
            }
            i = i + 1;
        } while (i < sDesObjectCount);
    }
    return gdl;
}

#define PKT_2(a, b) { Blk_2 *_g = gdl++; _g->w0 = (a); _g->w1 = (b); }

/* Draws the arrow that points to the next checkpoint, with the HUD's perspective. */
Blk_2 *des_draw_arrow(Blk_2 *gdl, s32 dyn, s32 fb, s32 view)
{
    s32 mtxOff;

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
        mtxOff = gGfxFrameIndex * 0x40;
        PKT_2(0xDA380003, (u32)((u8 *)sDesArrowMtx + mtxOff))
        PKT_2(0xDA380007, (u32)((u8 *)gHudPerspMtx + mtxOff))
        gdl = (Blk_2 *)prim_build_dl(0, sDesArrowModel, &sDesArrowTex, (Gfx *)gdl);
        PKT_2(0xDA380003, dyn + 0x400) /* back to Dyn.m4 */
        return gdl;
    }
}

#define G sDesLodGrid

/* Fills the 32x32 grid of patch cells around the camera with the rings that cover each (bit r: ring r), from the
   distance ahead and the field of view, keeps one ring per place, and marks in sDesSeamFlags where a ring meets a
   coarser one. */
void des_calc_lod_grid(void) {
    s32 i, j, camX, k;
    s32 x, z, side, ahead, camZ;
    s32 cosYaw, sinYaw, idx, xx;
    s32 cell = 4000;
    s32 yaw, parent;

    camX = sDesCamX >> 4;
    camZ = sDesCamZ >> 4;
    sDesLodCellX = xx = camX / cell;
    sDesLodOffX = camX - sDesLodCellX * cell + (((u32)camX & 1) >> 1);
    yaw = (u16)gCamYaw[0];
    yaw >>= 4;
    /* + 0, written as an expression of the address in the original */
    cosYaw = gCosTable[yaw] + (((u32)&gCosTable[yaw] & 1) >> 1);
    sinYaw = gCosTable[(yaw - 0x400) & 0xFFF];
    if (sDesLodOffX < 0) {
        sDesLodOffX += cell;
        sDesLodCellX -= 1;
    }
    sDesLodCellZ = camZ / cell;
    xx = sDesLodCellZ * cell;
    sDesLodOffZ = camZ - xx + (((u32)camZ & 1) >> 1);
    if (sDesLodOffZ < 0) {
        sDesLodOffZ += cell;
        sDesLodCellZ -= 1;
    }
    idx = 0;
    for (i = 0; i < DES_GRID_SIZE; i++) {
        for (j = 0; j < DES_GRID_SIZE; j++) {
            G[idx] = 0;
            x = ((j - 15) * 8000 - 4000) >> 4;
            k = ((u32)(sDesLodOffX >> 31) & 1) >> 1;
            z = (i * 8000 - 0x1E460) >> 4;
            x -= sDesLodOffX % 500;
            z -= sDesLodOffZ % 500;
            k = cosYaw * x;
            ahead = (cosYaw * z - sinYaw * x) >> 14;
            side = (k + sinYaw * z) >> 14;
            if (ahead < 5000) G[idx] = 3;
            if (ahead < 4000) G[idx] |= 4;
            if (i < 8 || i >= 24 || j < 8 || j >= 24) G[idx] &= 0xFB;
            if (ahead < 1500) G[idx] &= 0xF1;
            if (ahead >= -359 && side >= -359 && side < 360 && ahead < 360) {
                G[idx] |= 1;
            } else if (ahead < 0) {
                G[idx] = 0;
            } else {
                ahead = ahead / 2;
                if (side < 0) {
                    if (side + 360 < -ahead) G[idx] &= 0xFE;
                    if (side + 720 < -ahead) G[idx] &= 0xF1;
                } else {
                    if (ahead < side - 360) G[idx] &= 0xFE;
                    if (ahead < side - 720) G[idx] &= 0xF1;
                }
            }
            idx++;
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
            s32 row, idx1, idx2;
            row = ((i / 2 + 8) << 5) + 8;
            parent = row + j / 2;
            idx = ((i - sDesLod0ShiftZ) << 5) + (j - sDesLod0ShiftX);
            idx1 = ((i - sDesLod1ShiftZ) << 5) + (j - sDesLod1ShiftX);
            idx2 = ((i - sDesLod2ShiftZ) << 5) + (j - sDesLod2ShiftX);
            if (G[idx] & G[idx + 1] & G[idx + 32] & G[idx + 33] & 1) G[parent] &= 0xFD;
            if (G[idx1] & G[idx1 + 1] & G[idx1 + 32] & G[idx1 + 33] & 2) G[parent] &= 0xFB;
            if (G[idx2] & G[idx2 + 1] & G[idx2 + 32] & G[idx2 + 33] & 4) G[parent] &= 0xF7;
            if (G[parent] & 2) { G[idx] &= 0xFE; G[idx + 1] &= 0xFE; G[idx + 32] &= 0xFE; G[idx + 33] &= 0xFE; }
            if (G[parent] & 4) { G[idx1] &= 0xFD; G[idx1 + 1] &= 0xFD; G[idx1 + 32] &= 0xFD; G[idx1 + 33] &= 0xFD; }
            if (G[parent] & 8) { G[idx2] &= 0xFB; G[idx2 + 1] &= 0xFB; G[idx2 + 32] &= 0xFB; G[idx2 + 33] &= 0xFB; }
        }
    }
    mem_fill(sDesSeamFlags, 0, 0x400);
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 row = (((i >> 1) + 8) << 5) + 8;
            parent = row + (j >> 1);
            idx = (i - sDesLod0ShiftZ) * 32 + (j - sDesLod0ShiftX);
            if (sDesLodGrid[parent + 32] & 2) {
                if (sDesLodGrid[idx + 32] & 1) sDesSeamFlags[idx + 32] |= 0x10;
                if (sDesLodGrid[idx + 33] & 1) sDesSeamFlags[idx + 33] |= 0x10;
            }
            if (sDesLodGrid[parent - 32] & 2) {
                if (sDesLodGrid[idx] & 1) sDesSeamFlags[idx] |= 0x20;
                if (sDesLodGrid[idx + 1] & 1) sDesSeamFlags[idx + 1] |= 0x20;
            }
            if (sDesLodGrid[parent + 1] & 2) {
                if (sDesLodGrid[idx + 1] & 1) sDesSeamFlags[idx + 1] |= 0x40;
                if (sDesLodGrid[idx + 33] & 1) sDesSeamFlags[idx + 33] |= 0x40;
            }
            if (sDesLodGrid[parent - 1] & 2) {
                if (sDesLodGrid[idx] & 1) sDesSeamFlags[idx] |= 0x80;
                if (sDesLodGrid[idx + 32] & 1) sDesSeamFlags[idx + 32] |= 0x80;
            }
            idx = (i - sDesLod1ShiftZ) * 32 + (j - sDesLod1ShiftX);
            if (sDesLodGrid[parent + 32] & 4) {
                if (sDesLodGrid[idx + 32] & 2) sDesSeamFlags[idx + 32] |= 1;
                if (sDesLodGrid[idx + 33] & 2) sDesSeamFlags[idx + 33] |= 1;
            }
            if (sDesLodGrid[parent - 32] & 4) {
                if (sDesLodGrid[idx] & 2) sDesSeamFlags[idx] |= 2;
                if (sDesLodGrid[idx + 1] & 2) sDesSeamFlags[idx + 1] |= 2;
            }
            if (sDesLodGrid[parent + 1] & 4) {
                if (sDesLodGrid[idx + 1] & 2) sDesSeamFlags[idx + 1] |= 4;
                if (sDesLodGrid[idx + 33] & 2) sDesSeamFlags[idx + 33] |= 4;
            }
            if (sDesLodGrid[parent - 1] & 4) {
                if (sDesLodGrid[idx] & 2) sDesSeamFlags[idx] |= 8;
                if (sDesLodGrid[idx + 32] & 2) sDesSeamFlags[idx + 32] |= 8;
            }
        }
    }
}

/* divmod_floor written inline */
static inline void divmod(s32 v, s32 d, s32 *q, s32 *r)
{
    *q = v / d;
    *r = v - *q * d;
    if (*r < 0) {
        *r += d;
        *q -= 1;
    }
}

/* Assigns a cached patch to each cell of each ring that the LOD grid marks, building the patches that moved or
   are new (until the cache runs out), and sets each patch's seams. */
void des_update_patches(void)
{
    s32 camCellX, rx, camCellZ, rz, ringX, ringZ;
    s32 i, j, patch, slot, patchX, patchZ, b, rowDist, colDist;

    divmod(sDesCamX >> 4, 500, &camCellX, &rx);
    divmod(sDesCamZ >> 4, 500, &camCellZ, &rz);

    for (i = 0; i < DES_NUM_RINGS; i++) {
        j = 1 << i;
        sDesPatchGridOrigin[i] = ((camCellZ / j) * 0x20 + camCellX / j) & 0x3ff;
    }
    sDesCamCellX = camCellX;
    sDesCamCellZ = camCellZ;

    for (i = 0; i < DES_NUM_PATCHES; i++) {
        if (sDesPatches[i].life > 0)
            sDesPatches[i].life--;
        sDesPatches[i].visible = 0;
    }

    for (i = 0; i < DES_NUM_RINGS; i++) {
        for (j = 0; j < DES_GRID_SIZE * DES_GRID_SIZE; j++) {
            patch = sDesPatchGrid[i][j];
            if (patch != -1 && sDesPatches[patch].life <= 0)
                sDesPatchGrid[i][j] = -1;
        }
    }

    sDesFreePatchCount = 0;
    for (i = 0; i < DES_NUM_PATCHES; i++) {
        if (sDesPatches[i].life <= 0) {
            sDesFreePatches[sDesFreePatchCount++] = i;
        }
    }

    for (i = 0; i < DES_NUM_RINGS; i++) {
        divmod(sDesCamX >> 4, 500 << i, &ringX, &rx);
        divmod(sDesCamZ >> 4, 500 << i, &ringZ, &rz);
        slot = sDesPatchGridOrigin[i];
        patchZ = (ringZ - 16) * (500 << i);
        for (j = 0; j < 0x20; j++) {
            s32 k;
            patchX = (ringX - 16) * (500 << i);
            for (k = 0; k < 0x20; k++) {
                if ((sDesLodGrid[j * 32 + k] >> i) & 1) {
                    s32 t = sDesPatchGrid[i][slot];
                    s32 n;
                    if (t == -1) {
                        if (sDesFreePatchCount == 0) goto out; patch = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][slot] = patch; des_build_patch(patch, patchX, patchZ, i);
                    } else {
                        patch = t;
                        if (sDesPatches[patch].x != patchX || sDesPatches[patch].z != patchZ) {
                            if (sDesFreePatchCount == 0) goto out; patch = sDesFreePatches[--sDesFreePatchCount]; sDesPatchGrid[i][slot] = patch; des_build_patch(patch, patchX, patchZ, i);
                        } else {
                            sDesPatches[patch].life = 2;
                        }
                    }
                    sDesPatches[patch].visible = 1;
                    rowDist = j - 16;
                    if (rowDist < 0)
                        rowDist = -rowDist;
                    colDist = k - 16;
                    if (colDist < 0)
                        colDist = -colDist;
                    if (colDist + rowDist < 7)
                        sDesPatches[patch].near = 1;
                    else
                        sDesPatches[patch].near = 0;
                } else {
                    sDesPatchGrid[i][slot] = -1;
                }
                slot = (slot + 1) & 0x3ff;
                patchX += 500 << i;
            }
            patchZ += 500 << i;
        }
    }
out:
    for (i = 0; i < DES_NUM_PATCHES; i++)
        sDesPatches[i].seams = 0;

    for (i = 0; i < 2; i++) {
        slot = sDesPatchGridOrigin[i];
        for (j = 0; j < DES_GRID_SIZE * DES_GRID_SIZE; j++) {
            patch = sDesPatchGrid[i][slot];
            if (patch != -1) {
                DesPatch *r = (DesPatch *)(patch * sizeof(DesPatch) + (u32)sDesPatches);
                u8 x = sDesSeamFlags[j];
                if (i == 0)
                    r->seams = x >> 4;
                else
                    r->seams = x & 0xf;
            }
            slot = (slot + 1) % 0x400;
        }
    }
}

/* divmod_floor written inline again */
static __inline__ void fdiv(s32 x, s32 d, s32 *q, s32 *r)
{
    *q = x / d;
    *r = x - *q * d;
    if (*r < 0) {
        *r += d;
        (*q)--;
    }
}

/* Builds patch `index` at (x, z) (16 mm units) for a ring: 5x5 vertices from the height map every 1 << lod cells,
   shaded by the slope towards +z. */
void des_build_patch(s32 index, s32 x, s32 z, s32 lod)
{
    s32 q, r, blockX, blockZ;
    s32 mapCol, mapRow, px, vz, col, row, k;
    s32 h, shade;
    Vtx *v;

    sDesPatches[index].x = x;
    sDesPatches[index].z = z;
    fdiv(x, 125, &q, &r);
    mapCol = q;
    fdiv(z, 125, &q, &r);
    mapRow = q;
    blockX = 0;
    blockZ = 0;
    fdiv(x, 8000, &blockX, &q);
    x = q - 4000;
    fdiv(z, 8000, &blockZ, &q);
    vz = q - 4000;
    k = 0;
    sDesPatches[index].lod = lod;
    sDesPatches[index].life = 2;
    sDesPatches[index].blockX = blockX;
    sDesPatches[index].blockZ = blockZ;
    row = mapRow;
    for (q = 0; q < 5; q++) {
        s32 xx = x;
        col = mapCol;
        for (r = 0; r < 5; r++) {
            sDesPatches[index].vtx[k].v.ob[0] = xx;
            sDesPatches[index].vtx[k].v.ob[1] = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)] * 4;
            sDesPatches[index].vtx[k].v.ob[2] = vz;
            shade = gDesertHeightMap[((row & 0x1FF) << 9) | (col & 0x1FF)];
            shade = (gDesertHeightMap[(((row + 1) & 0x1FF) << 9) | (col & 0x1FF)] - shade) * 8 + 0x80;
            if (shade < 0) shade = 0;
            if (shade > 255) shade = 255;
            sDesPatches[index].vtx[k].v.cn[0] = shade * 245 / 256 + 10;
            sDesPatches[index].vtx[k].v.cn[1] = shade * 230 / 256 + 25;
            sDesPatches[index].vtx[k].v.cn[2] = shade * 170 / 256 + 85;
            sDesPatches[index].vtx[k].v.tc[0] = (r << 10) << lod;
            sDesPatches[index].vtx[k].v.tc[1] = (q << 10) << lod;
            sDesPatches[index].vtx[k].v.cn[3] = 255;
            k++;
            xx += 125 << lod;
            col += 1 << lod;
        }
        vz += 125 << lod;
        row += 1 << lod;
    }
}

/* The triangles of a 5x5 patch: the inner 3x3 vertices, then each side either full or, where it meets a coarser
   ring (its bit in seams), with every other edge vertex skipped. Built in the Z buffer and copied. */
Gfx *des_build_patch_dl(s32 seams)
{
    Gfx *gdl = gZBuffer;
    s32 i, j, k, step, inward;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            k = i * 6 - j * 4;
            gSP2Triangles(gdl++, k + 10, k + 11, k + 16, 0, k + 10, k + 6, k + 11, 0);
            gSP2Triangles(gdl++, k + 11, k + 12, k + 16, 0, k + 11, k + 6, k + 12, 0);
        }
    }

    for (i = 0; i < 4; i++) {
        if ((seams >> i) & 1) {
            j = sDesSeamStartVtx[i];
            step = sDesSeamStep[i];
            inward = sDesSeamInward[i];
            gSP2Triangles(gdl++, j, j + step + inward, j + 2 * step, 0, j + 2 * step, j + 3 * step + inward, j + 4 * step, 0);
        } else {
            j = sDesSeamStartVtx[i];
            step = sDesSeamStep[i];
            inward = sDesSeamInward[i];
            gSP2Triangles(gdl++, j, j + step + inward, j + step, 0, j + step, j + step + inward, j + 2 * step, 0);
            gSP2Triangles(gdl++, j + 2 * step, j + 3 * step + inward, j + 3 * step, 0, j + 3 * step, j + 3 * step + inward, j + 4 * step, 0);
        }
    }

    gSPEndDisplayList(gdl++);
    i = (u8 *)gdl - (u8 *)gZBuffer;
    gdl = mem_alloc_dbg(i, 0, (s32)sStrDesgfxC, 0x508);
    memcpy(gdl, gZBuffer, i);
    return gdl;
}

/* Frees the desert's buffers and display lists. */
void des_free(void)
{
    s32 i;
    Gfx **dls;

    mem_free(sDesPatches);
    mem_free(gDesertHeightMap);
    mem_free(sDesObjMtx[0]);
    mem_free(sDesObjMtx[1]);
    i = 0;
    mem_free(gDesertTiles);
    mem_free(sDesArrowMtx);
    dls = sDesPatchDLs;
    do {
        Gfx *dl = *dls;
        dls++;
        i++;
        mem_free(dl);
    } while (i < 16);
}

/* Sets the arrow's angle on screen (0x10000 per turn; negative hides it). */
void des_set_arrow_angle(s32 angle)
{
    sDesArrowAngle = angle;
}

/* Division rounding down: num = q * den + r with 0 <= r < den. */
void divmod_floor(s32 num, s32 den, s32 *q, s32 *r)
{
    s32 quot = num / den;
    s32 rem;
    *q = quot;
    rem = num - quot * den;
    *r = rem;
    if (rem < 0) {
        *r = rem + den;
        *q = *q - 1;
    }
}

/* Grows a bounding box by a point; not called. */
void des_bbox_add(s32 x, s32 z)
{
    if (x < sDesBBoxMinX) {
        sDesBBoxMinX = x;
    }
    if (z < sDesBBoxMinZ) {
        sDesBBoxMinZ = z;
    }
    if (sDesBBoxMaxX < x) {
        sDesBBoxMaxX = x;
    }
    if (sDesBBoxMaxZ < z) {
        sDesBBoxMaxZ = z;
    }
}

/* Empties the bounding box; not called. */
void des_bbox_reset(void)
{
    sDesBBoxMinZ = 999;
    sDesBBoxMinX = 999;
    sDesBBoxMaxZ = -999;
    sDesBBoxMaxX = -999;
}

#define PKT_3(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

/* prim_build_dl's texture mode callback for the desert's objects: the render mode and combiner for cut-out
   (alpha-tested) or opaque textures. */
Gfx *des_tex_mode_callback(Gfx *gdl, s32 cutout)
{
    Gfx *p0;
    Gfx *p1;

    if (cutout != 0) {
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

/* Sets the checkpoint the marker and the smoke show. */
void des_set_waypoint(s32 checkpoint)
{
    gDesWaypoint = checkpoint;
}
