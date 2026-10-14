#include "game.h"

/* tube_alloc_mtx written inline: a matrix of the frame's pool placing a model at (x, y, z) (16 mm units) relative to
   the view's camera, or NULL when the pool is used up. */
static inline MtxS *getmtx(s32 x, s32 y, s32 z, s32 view)
{
    MtxS *m;

    if (sTubeMtxCount >= TUBE_MTX_POOL) {
        return NULL;
    }
    x = x * 16 - gCamPos[view].x;
    y = y * 16 - gCamPos[view].y;
    z = z * 16 - gCamPos[view].z;
    m = &sTubeMtxPool[gGfxFrameIndex][sTubeMtxCount];
    mtx_from_angles_pos_scale(m, 0, 0, 0, x, y, z, 0x100);
    sTubeMtxCount++;
    return m;
}

/* Loads an outdoor track: textures, objects and sections with their display lists, object flags, breakable objects,
   the PVS, walls and ground, each section's bounds grown to hold its objects, and the instances. */
void tube_init(void)
{
    s32 i, j;
    TexSet **terrain;
    WallMesh **walls;

    for (i = 0; i < 2; i++) {
        sTubeMtxPool[i] = mem_alloc_dbg(TUBE_MTX_POOL * sizeof(Mtx), 0, sStrTubegfxC, 0xCA);
        for (j = 0; j < TUBE_MTX_POOL; j++) {
            mtx_from_angles_pos_scale(&sTubeMtxPool[i][j], 0, 0, 0, 0, 0, 0, 0x100);
        }
    }
    sTubeMtxFrame = -1;
    gFarClip = 8000;
    res_register_type((u8 *)sStrTubeArn, (s32)arn_relocate);
    res_register_type((u8 *)sStrTubeWal, (s32)height_init_walls_reloc);
    gObjectTexSet = (TexSet *)track_res_get(gGidPkgScene);
    terrain = &gTerrainTexSet;
    *terrain = (TexSet *)track_res_get(gGidPkgGround);
    gWaterTexSet = (TexSet *)track_res_get(gGidPkgWater);
    gAnimTexSet = (TexSet *)track_res_get(gGidPkgAnim);
    gAltTexSet = (TexSet *)track_res_get(gGidPkgAlt);
    gTrackObjects = (TrackModelList *)track_res_get(gGidArnScene);
    sObjectSegs[0] = gObjectTexSet;
    sObjectSegs[1] = gAltTexSet;
    sObjectSegs[2] = gAnimTexSet;
    gObjectDLs = tube_build_dlists(gTrackObjects, sObjectSegs, PRIM_TLUT_SEGMENT | PRIM_RESET_TLUT | PRIM_LOAD_TLUT, 1);
    gTrackSections = (TrackModelList *)track_res_get(gGidArnGround);
    gSectionDLs = tube_build_dlists(gTrackSections, terrain, 0, 0);
    gWaterModels = (TrackModelList *)track_res_get(gGidArnWater);
    if (gCheatFlags & CHEAT_ODD_COLOR) {
        tube_rotate_vertex_colors(gTrackSections);
        tube_rotate_vertex_colors(gTrackObjects);
    }
    gObjectFlags = mem_alloc_dbg(gTrackObjects->count * 4, 0, sStrTubegfxC, 0xF9);
    for (i = 0; i < gTrackObjects->count; i++) {
        gObjectFlags[i] = gTrackObjects->e[i].mask;
    }
    shatter_init(gTrackObjects);
    gSectionMtxs = mem_alloc_dbg(gTrackSections->count * 4 * sizeof(Mtx *), 0, sStrTubegfxC, 0x103); /* per view */
    tube_load_pvs();
    for (i = 0; i < 256; i++) {
        if (gObjectTexSet->pal[i] == 0xF83F) { /* opaque magenta: the colour key, made transparent */
            gObjectTexSet->pal[i] = 0;
        }
    }
    walls = &gTrackWallMesh;
    *walls = (WallMesh *)track_res_get(gGidWalCol);
    remove_walls_init(gTrackObjects, *walls);
    height_init(gTrackSections, gTrackObjects, *walls, (u32 *)track_res_get(gGidPatches));
    sSectionBounds = mem_alloc_dbg(gTrackSections->count * 16, 0, sStrTubegfxC, 0x11D);
    for (i = 0; i < gTrackSections->count; i++) {
        sSectionBounds[i].x0 = gTrackSections->e[i].x0;
        sSectionBounds[i].z0 = gTrackSections->e[i].z0;
        sSectionBounds[i].x1 = gTrackSections->e[i].x1;
        sSectionBounds[i].z1 = gTrackSections->e[i].z1;
    }
    tube_assign_objects(gTrackObjects, gTrackSections);
    if (gWaterModels != NULL) {
        tube_assign_objects(gWaterModels, gTrackSections);
    }
    instances_load(gResourceSet, track_res_find(gGidInstanceDef), 0, gTrackSections, gObjectTexSet);
}

/* Frees the outdoor track's display lists, matrices, PVS, ground and walls. */
void tube_free(void)
{
    u32 i;

    shatter_free();
    i = 0;
    mem_free(sSectionBounds);
    mem_free(gSectionMtxs);
    mem_free(gObjectFlags);
    if (gTrackSections->count != 0) {
        do {
            mem_free(gSectionDLs[i]);
            i++;
        } while (i < gTrackSections->count);
    }
    i = 0;
    if (gTrackObjects->count != 0) {
        do {
            mem_free(gObjectDLs[i]);
            i++;
        } while (i < gTrackObjects->count);
    }
    mem_free(gSectionDLs);
    mem_free(gObjectDLs);
    tube_free_tex_segs();
    mem_free(sTubeMtxPool[0]);
    mem_free(sTubeMtxPool[1]);
    mem_free(sPvsIndex);
    mem_free(sPvsData);
    sPvsIndex = 0;
    sPvsData = 0;
    height_free();
    remove_walls_free();
    gAnimTexSet = 0;
    gAltTexSet = 0;
    gWaterModels = 0;
    gWaterTexSet = 0;
    gTrackSections = NULL;
}

/* Culls the sections for a view, then gives each object and water model the view bits of its section (whose index
   is in the high half of its mask). */
void tube_update_view(s32 view)
{
    u32 i;
    TrackModel *model;

    tube_calc_fog_range(view, &sTubeFogNear[view], &sTubeFogFar[view]);
    tube_cull_sections(gTrackSections, view);
    for (i = 0; i < gTrackObjects->count; i++) {
        model = &gTrackObjects->e[i];
        model->mask = (model->mask & 0xFFFF0000) | gTrackSections->e[model->mask >> 16].mask;
    }
    if (gWaterModels != NULL) {
        for (i = 0; i < gWaterModels->count; i++) {
            model = &gWaterModels->e[i];
            model->mask = (model->mask & 0xFFFF0000) | gTrackSections->e[model->mask >> 16].mask;
        }
    }
    if (gGfxFrameIndex != sTubeMtxFrame) {
        sTubeMtxCount = 0;
    }
    sTubeMtxFrame = gGfxFrameIndex;
}

/* Draws the near sections and objects of a view, either those within the fog range (fog 0) or those beyond it with
   fog (fog 1, with the near instances). */
Gfx *tube_draw_layer(Gfx *gdl, s32 dyn, s32 fb, s32 view, s32 fog)
{
    u32 i;
    MtxS *mtx;
    TrackModel *model;
    u32 bits;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (fog == 0) {
        gDPSetRenderMode(gdl++, 0x442230, 0);
    } else {
        gSPSetGeometryMode(gdl++, G_FOG);
        gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
        gSPFogPosition(gdl++, sTubeFogNear[view], sTubeFogFar[view]);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
    }

    for (i = 0; i < gTrackSections->count; i++) {
        model = &gTrackSections->e[i];
        bits = model->mask >> (view * 2);
        if (!(bits & 1)) continue;
        if (bits & 2) continue;
        if (model->mask & TRACK_MODEL_FOGGED(view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x = (model->x0 + model->x1) / 2;
            s32 z = (model->z0 + model->z1) / 2;
            mtx = getmtx(x, 0, z, view);
            gSectionMtxs[i * 4 + view] = mtx;
            if (mtx != NULL) {
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gSectionDLs[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    if (fog != 0) {
        gdl = instances_draw(gdl, (Mtx *)dyn, fb, view, 1);
        gDPPipeSync(gdl++);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    if (fog == 0) {
        gSPSegment(gdl++, 6, sTexOpaqueDL);
        gSPSegment(gdl++, 7, sTexCutoutDL);
    } else {
        gSPSegment(gdl++, 6, sTexOpaqueFogDL);
        gSPSegment(gdl++, 7, sTexCutoutFogDL);
    }

    for (i = 0; i < gTrackObjects->count; i++) {
        if (gObjectFlags[i] & OBJ_HIDDEN) continue;
        model = &gTrackObjects->e[i];
        bits = model->mask >> (view * 2);
        if (!(bits & 1)) continue;
        if (bits & 2) continue;
        if (model->mask & TRACK_MODEL_FOGGED(view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x, z;
            gdl = tube_set_anim_tex_segs(gdl, i);
            x = (model->x0 + model->x1) / 2;
            z = (model->z0 + model->z1) / 2;
            mtx = getmtx(x, 0, z, view);
            if (mtx != NULL) {
                if (gObjectFlags[i] & OBJ_BREAKABLE) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gObjectDLs[i]);
                if (gObjectFlags[i] & OBJ_BREAKABLE) {
                    gSPSetGeometryMode(gdl++, gCullMode);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, dyn + 0x400 /* Dyn.m4 */, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}

/* Draws the sections, instances and objects of a view beyond the level-of-detail distance, fogged, with the far
   viewport. */
Gfx *tube_draw_far(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    u32 i;
    MtxS *mtx;
    TrackModel *model;
    u32 bits;
    s32 tmp[18];

    gSPSegment(gdl++, 1, gObjectTexSet->pal);
    if (gAltTexSet != NULL) {
        gSPSegment(gdl++, 2, gAltTexSet->pal);
    }
    if (gAnimTexSet != NULL) {
        gSPSegment(gdl++, 3, gAnimTexSet->pal);
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    gSPFogPosition(gdl++, sTubeFogNear[view], sTubeFogFar[view]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    gSPViewport(gdl++, &gViewports[view + 4]);

    for (i = 0; i < gTrackSections->count; i++) {
        model = &gTrackSections->e[i];
        bits = model->mask >> (view * 2);
        if (!(bits & 1)) continue;
        if (bits & 2) {
            s32 x = (model->x0 + model->x1) / 2;
            s32 z = (model->z0 + model->z1) / 2;
            mtx = getmtx(x, 0, z, view);
            gSectionMtxs[i * 4 + view] = mtx;
            if (mtx != NULL) {
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gSectionDLs[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gdl = instances_draw(gdl, (Mtx *)dyn, fb, view, 3);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPSegment(gdl++, 6, sTexOpaqueFogDL);
    gSPSegment(gdl++, 7, sTexCutoutFogDL);

    for (i = 0; i < gTrackObjects->count; i++) {
        if (gObjectFlags[i] & OBJ_HIDDEN) continue;
        model = &gTrackObjects->e[i];
        bits = model->mask >> (view * 2);
        if (!(bits & 1)) continue;
        if (bits & 2) {
            s32 x, z;
            gdl = tube_set_anim_tex_segs(gdl, i);
            x = (model->x0 + model->x1) / 2;
            z = (model->z0 + model->z1) / 2;
            mtx = getmtx(x, 0, z, view);
            if (mtx != NULL) {
                if (gObjectFlags[i] & OBJ_BREAKABLE) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gObjectDLs[i]);
                if (gObjectFlags[i] & OBJ_BREAKABLE) {
                    gSPSetGeometryMode(gdl++, gCullMode);
                }
            }
        }
    }
    gSPMatrix(gdl++, dyn + 0x400 /* Dyn.m4 */, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

/* Draws the visible water models (IA textures tinted by their palette colour) and the riders' big splashes. */
Gfx *tube_draw_water(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    s32 i;
    MtxS *mtx;
    TrackModel *model;
    s32 tmp[18];

    if (gWaterModels == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, sTubeFogFar[view], sTubeFogNear[view]); /* near and far swapped */
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;

    for (i = 0; i < gWaterModels->count; i++) {
        model = &gWaterModels->e[i];
        if ((model->mask >> (view * 2)) & 1) {
            s32 x = (model->x0 + model->x1) / 2;
            s32 z = (model->z0 + model->z1) / 2;
            mtx = getmtx(x, 0, z, view);
            if (mtx != NULL) {
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gdl = prim_build_dl(0, gWaterModels->e[i].model, &gWaterTexSet, gdl);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    for (i = 0; i < 8; i++) {
        if (sSplashTimer[i] == 0) continue;
        model = &gTrackSections->e[sSplashSection[i]];
        if ((model->mask >> (view * 2)) & 1) {
            s32 alpha = sSplashTimer[i] / 2;
            s32 frame;
            if (alpha >= 256) {
                alpha = 255;
            }
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, alpha);
            /* frames 0-4, then 2-4 looping */
            frame = (120 - sSplashTimer[i]) / 12;
            if (frame >= 5) {
                frame = (frame - 2) % 3 + 2;
            }
            gDPLoadTextureBlock(gdl++, gWaterSplashTextures->tex[frame].data, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                G_TX_MIRROR, G_TX_MIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            mtx = getmtx(sSplashPos[i].x, sSplashPos[i].y, sSplashPos[i].z, view);
            if (mtx != NULL) {
                mtx_from_angles_pos_scale(mtx, 0, 0, 0, sSplashPos[i].x * 16 - gCamPos[view].x, sSplashPos[i].y * 16 - gCamPos[view].y,
                              sSplashPos[i].z * 16 - gCamPos[view].z, 0x100);
                gSPMatrix(gdl++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(gdl++, sSplashQuadVtx, 4, 0);
                gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
    gPrimIATextures = 0;
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, dyn + 0x400 /* Dyn.m4 */, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    prim_set_tex_mode_callback(0);
    return gdl;
}

/* Sets the sections' bits for a view from the PVS list of the section under the camera (its near list also far),
   then clears those behind the camera or outside the field of view and marks those past the fog's start fogged.
   On Excite 3D's side view the sections near the player's are visible instead. */
void tube_cull_sections(TrackModelList *sections, s32 view)
{
    s32 target;
    s32 i;
    s32 pvsBase;
    s32 camX, camY, camZ;
    s32 count;
    s32 visBit;
    u16 *pvs;
    s32 numFar, total;
    s32 h;
    s32 fogDepth;
    s32 yaw;
    s32 cosYaw, sinYaw;
    s32 d[4];
    s32 slope[4];

    target = view;
    if (gCamTargetOverride != -1) {
        target = gCamTargetOverride;
    }
    sCullSlope = 0x68;
    if (gNumScreens == 2) {
        sCullSlope = 0x9C;
    }
    if (gCurTrack_2 - gTracks == TRACK_EXCITE_3D && gRiders[0].camMode == 4) {
        numFar = gRiderSection[0]; /* here the player's section */
        if (numFar >= 0 && numFar < sections->count) {
            s32 refMid = sections->e[numFar].z0 + sections->e[numFar].z1;
            for (i = 0; i < sections->count; i++) {
                s32 bitsKept = sections->e[i].mask & (-4 << (view * 2)); /* also clears the views before */
                s32 mid = sections->e[i].z0 + sections->e[i].z1;
                sections->e[i].mask = bitsKept;
                if (mid >= refMid - 7500 && mid <= refMid + 2500) {
                    sections->e[i].mask = bitsKept | (1 << (view * 2));
                }
            }
        }
    } else {
        pvsBase = 0;
        if (gNumScreens != 1) {
            pvsBase = sections->count;
        }
        camX = gCamPos[view].x;
        camY = gCamPos[view].y;
        camZ = gCamPos[view].z;
        count = 0;
        if (gRiders[view].camMode == 9) { /* the fall view */
            sCamSection = gRiderSection[target];
        } else {
            gGroundFound = 0;
            height_get_ground(camX, camY, camZ);
            if (gGroundFound == 0) {
                sCamSection = gRiderSection[target];
                gGroundFound = 1;
            } else {
                sCamSection = gGroundArea;
            }
        }
        visBit = 1 << (view * 2);
        for (i = 0; i < sections->count; i++) {
            sections->e[i].mask &= ~TRACK_MODEL_BITS(view);
        }
        pvs = sPvsIndex[sCamSection + pvsBase];
        numFar = pvs[0];
        total = numFar + pvs[1];
        for (i = 0; i < total; i++) {
            sections->e[pvs[i + 2]].mask |= visBit;
        }
        for (i = 0; i < numFar; i++) {
            sections->e[pvs[i + 2]].mask |= TRACK_MODEL_FAR(view);
        }
        camX >>= 4;
        camZ >>= 4;
        sections->e[sCamSection].mask |= visBit;
        fogDepth = 0xC48A / (0x3EE - sTubeFogNear[view]);
        yaw = gCamYaw[view] >> 4;
        cosYaw = gCosTable[yaw & 0xFFF];
        sinYaw = gCosTable[(yaw - 0x400) & 0xFFF];
        for (i = 0; i < sections->count; i++) {
            TrackModel *sec = &sections->e[i];
            TrackBounds *bounds = &sSectionBounds[i];
            if (sec->mask & visBit) {
                count++;
                if (gGroundFound == 0 && count >= 16) {
                    sec->mask &= ~visBit;
                    count--;
                } else {
                    s32 t, u, f, x, z;
                    x = sec->x0;
                    z = sec->z0;
                    t = (cosYaw * (z - camZ) - sinYaw * (x - camX)) >> 14;
                    x = sec->x1 - x;
                    h = (-sinYaw * x) >> 14;
                    f = (cosYaw * (sec->z1 - z)) >> 14;
                    u = t + h;
                    d[0] = t;
                    d[1] = u;
                    d[2] = t + f;
                    d[3] = u + f;
                    /* behind the camera */
                    if (d[0] < 11 && d[1] < 11 && d[2] < 11 && d[3] < 11) {
                        count--;
                        sec->mask &= ~TRACK_MODEL_BITS(view);
                    } else {
                        if (d[0] >= fogDepth || d[1] >= fogDepth || d[2] >= fogDepth || d[3] >= fogDepth) {
                            sec->mask |= TRACK_MODEL_FOGGED(view);
                        } else {
                            sec->mask &= ~TRACK_MODEL_FOGGED(view);
                        }
                        /* outside the field of view: every corner's side / depth beyond the slope */
                        if (gCurTrack_2 - gTracks != TRACK_EXCITE_3D && d[0] > 300 && d[1] > 300 && d[2] > 300 && d[3] > 300) {
                            s32 y = bounds->x0;
                            s32 u2;
                            t = (cosYaw * (y - camX) + sinYaw * (bounds->z0 - camZ)) >> 14;
                            y = bounds->x1 - y;
                            h = (cosYaw * y) >> 14;
                            u2 = t + h;
                            f = (sinYaw * (bounds->z1 - bounds->z0)) >> 14;
                            slope[0] = (t << 8) / d[0];
                            slope[1] = (u2 << 8) / d[1];
                            slope[2] = ((t + f) << 8) / d[2];
                            slope[3] = ((u2 + f) << 8) / d[3];
                            if ((slope[0] < -sCullSlope && slope[1] < -sCullSlope && slope[2] < -sCullSlope && slope[3] < -sCullSlope) ||
                                (sCullSlope < slope[0] && sCullSlope < slope[1] && sCullSlope < slope[2] && sCullSlope < slope[3])) {
                                count--;
                                sec->mask &= ~TRACK_MODEL_BITS(view);
                            }
                        }
                    }
                }
            }
        }
    }
}

/* Assigns each model of list a to the section under its centre (or the nearest one) and grows that section's
   bounds to hold it. */
void tube_assign_objects(TrackModelList *models, TrackModelList *sections)
{
    u32 i, k;
    s32 x, z, cx, cz, dist, dx, dz;
    s32 section, bestDist;

    for (i = 0; i < models->count; i++) {
        gGroundFound = 0;
        x = (models->e[i].x0 + models->e[i].x1) / 2;
        z = (models->e[i].z0 + models->e[i].z1) / 2;
        height_get_ground(x << 4, 999999, z << 4);
        if (gGroundFound != 0) {
            { s32 n = gGroundArea; if (n) dz = 1; section = n; }
        } else {
            x /= 4;
            z /= 4;
            for (k = 0; k < sections->count; k++) {
                cx = (sections->e[k].x0 + sections->e[k].x1) / 2;
                cz = (sections->e[k].z0 + sections->e[k].z1) / 2;
                cx /= 4;
                cz /= 4;
                dx = x - cx;
                dz = z - cz;
                { s32 t; t = dx * dx; do { dist = dz * dz; } while (0); dz = dist; dist = t + dz; }
                if (dist < bestDist || k == 0) {
                    bestDist = dist;
                    section = k;
                }
            }
        }
        do {
            models->e[i].mask = section << 16;
        } while (0);
        if (models->e[i].x0 < ((TrackBounds *)((section << 4) + (u32)sSectionBounds))->x0) {
            sSectionBounds[section].x0 = models->e[i].x0;
        }
        if (models->e[i].z0 < sSectionBounds[section].z0) {
            sSectionBounds[section].z0 = models->e[i].z0;
        }
        if (sSectionBounds[section].x1 < models->e[i].x1) {
            sSectionBounds[section].x1 = models->e[i].x1;
        }
        if (sSectionBounds[section].z1 < models->e[i].z1) {
            sSectionBounds[section].z1 = models->e[i].z1;
        }
    }
}

/* The height of the water under (x, z) in mm: the triangle of a water model containing the point. */
s32 tube_get_water_level(s32 x, s32 z, s32 *out)
{
    u32 i;
    s32 k;
    s32 xs[4];
    s32 zs[4];
    s32 px, pz;
    TrackModel *water;
    u8 *cmd;
    u8 *vtxBase;

    if (gWaterModels == NULL) {
        return 0;
    }
    x >>= 4;
    z >>= 4;
    for (i = 0; i < gWaterModels->count; i++) {
        water = &gWaterModels->e[i];
        if (x < water->x0 || water->x1 < x || z < water->z0 || water->z1 < z) {
            continue;
        }
        if (gCurTrack_3[0xb] == TRACK_ENV_OUTDOOR) { /* gTracks +0x2C: vertices relative to the box's centre */
            px = x - (water->x0 + water->x1) / 2;
            pz = z - (water->z0 + water->z1) / 2;
        } else {
            px = x;
            pz = z;
        }
        /* walk the model's commands: vertex loads set the base, triangles are tested in x/z */
        for (cmd = water->model->cmds; cmd[0] != 0; cmd += 4) {
            if ((cmd[0] >> 4) == PRIM_CMD_TRI) {
                for (k = 0; k < 3; k++) {
                    xs[k] = *(s16 *)((u32)(cmd[k + 1] << 4) + (u32)vtxBase);
                    zs[k] = *(s16 *)((u32)(cmd[k + 1] << 4) + (u32)vtxBase + 4);
                }
                if (point_in_tri_xz(xs, zs, px, pz)) {
                    *out = *(s16 *)((u32)(cmd[1] << 4) + (u32)vtxBase + 2) << 4;
                    return 1;
                }
            } else if ((cmd[0] >> 4) == PRIM_CMD_VTX) {
                vtxBase = (u8 *)water->model->vtx + (*(u16 *)(cmd + 2) << 4);
            }
        }
    }
    return 0;
}

/* Builds the display lists of a model list. Track resources first move their command streams to the spare
   framebuffer so the resource can shrink to the vertices. With animTex, records the animated texture segments each
   model uses (tube_remap_tex_addr). */
Gfx **tube_build_dlists(TrackModelList *list, TexSet **sets, u32 flags, s32 animTex)
{
    u8 *spare;
    s32 off;
    s32 size;
    u32 i;
    Gfx **dls;
    Gfx *dl;
    s32 sz;
    TexSegRef *copy;
    s32 j;
    s32 delta;

    spare = gFramebuffers[1 - gGfxFrameIndex];
    if (!(*(s32 *)(gCurTrack + 0x50) & TRACK_FLAG_USER)) {
        i = (s32)list->e[0].model->cmds - (s32)list;
        size = mem_get_size(list);
        memcpy(spare, (u8 *)list + i, size - i);
        list = mem_realloc_dbg(list, i, 0, sStrTubegfxC, 0x51B);
        do { } while (0);
        sz = 0;
        delta = (s32)spare - ((s32)list + i);
        if (list->count != 0) {
            do {
                list->e[sz].model->cmds += delta;
                sz++;
            } while (sz < list->count);
        }
    }
    dls = mem_alloc_dbg(list->count * 4, 0, sStrTubegfxC, 0x524);
    prim_set_tex_mode_callback(animTex ? tube_tex_mode_callback : NULL);
    if (animTex && gAnimTexSet) {
        tube_init_tex_groups();
        sTexSegListIndex = mem_alloc_dbg((list->count + 1) * 4, 0, sStrTubegfxC, 0x52E);
        sTexSegListCur = sTexSegList = (TexSegRef *)(gZBuffer + 0x1E000);
    }
    for (i = 0; i < list->count; i++) {
        gPrimTexAlphaMode = -1;
        /* built once into the Z buffer to measure it */
        dl = mem_alloc_dbg((s32)prim_build_dl(flags | PRIM_END_DL, list->e[i].model, sets, (Gfx *)gZBuffer) - gZBuffer, 0,
                          sStrTubegfxC, 0x541);
        dls[i] = dl;
        if (dl == NULL) {
            for (;;) { /* out of memory: hang */
            }
        }
        if (animTex && gAnimTexSet) {
            prim_set_tex_addr_callback(tube_remap_tex_addr);
            sTexSegListIndex[i] = sTexSegListCur;
            sTexSegCount = 0;
        }
        gPrimTexAlphaMode = -1;
        prim_build_dl(flags | PRIM_END_DL, list->e[i].model, sets, dls[i]);
        if (animTex && gAnimTexSet) {
            sTexSegListCur += sTexSegCount;
            prim_set_tex_addr_callback(NULL);
        }
    }
    if (animTex && gAnimTexSet) {
        sTexSegListIndex[i] = sTexSegListCur;
        i++;
        sz = (s32)sTexSegListCur - (s32)sTexSegList;
        if (sz != 0) {
            sz *= 4;
            copy = mem_alloc_dbg(sz, 0, sStrTubegfxC, 0x565);
            memcpy(copy, sTexSegList, sz);
            for (j = 0; j < (s32)i; j++) {
                sTexSegListIndex[j] = copy + (((s32)sTexSegListIndex[j] - (s32)sTexSegList) >> 2);
            }
            sTexSegList = copy;
        } else {
            sTexSegList = NULL;
        }
    }
    prim_set_tex_mode_callback(NULL);
    return dls;
}

/* prim_build_dl's texture address callback while building the outdoor models: a texture of an animated group
   (in gAnimTexSet) is drawn through segment 8 + n, n its place in the model's list of animated textures. */
void tube_remap_tex_addr(u32 *addr)
{
    TexSet *row; /* entry k is row->tex[0]: the walk steps over whole entries from the set's start */
    s32 iVar4;
    s32 last;
    s32 k;
    u8 *pb;
    u32 texAddr;

    texAddr = *addr;
    last = gAnimTexSet->count - 1;
    if (((u32)gAnimTexSet->tex[0].data <= texAddr) && (texAddr <= (u32)gAnimTexSet->tex[last].data) && (-1 < last)) {
        k = 0;
        row = gAnimTexSet;
        while (k <= last) {
            if ((u32)row->tex[0].data == texAddr) {
                break;
            }
            k = k + 1;
            row = (TexSet *)((TexEntry *)row + 1);
        }
        if ((k <= last) && (1 < sTexAnimGroupLen[k])) {
            last = 0;
            while (last < sTexSegCount) {
                if (sTexSegListCur[last].offset + sTexSegListCur[last].groupStart == k) {
                    break;
                }
                last = last + 1;
            }
            if (last == sTexSegCount) {
                if (last <= 7) goto store;
                do { /* more than 8 animated textures in a model: hang */
                } while (1);
store:
                /* the low bytes of the s32 entries */
                sTexSegListCur[sTexSegCount].groupStart = ((u8 *)&sTexAnimGroupStart[k])[3];
                sTexSegListCur[sTexSegCount].groupLen = ((u8 *)&sTexAnimGroupLen[k])[3];
                sTexSegListCur[sTexSegCount].offset = (u8)(k - ((u8 *)&sTexAnimGroupStart[k])[3]);
                sTexSegCount = sTexSegCount + 1;
            }
            *addr = (u32)((last + 8) << 24);
        }
    }
}

/* For each texture of gAnimTexSet: the first texture and the length of its animation group. */
void tube_init_tex_groups(void)
{
    s32 i, start, k;

    sTexAnimGroupStart = (s32 *)(gZBuffer + 0x18000);
    sTexAnimGroupLen = sTexAnimGroupStart + gAnimTexSet->count;
    start = 0;
    for (i = 0; i < gAnimTexSet->count; i++) {
        if (!(gAnimTexSet->tex[i].flags & TEX_ANIM_RATE)) {
            for (k = start; k <= i; k++) {
                sTexAnimGroupStart[k] = start;
                sTexAnimGroupLen[k] = i - start + 1;
            }
            start = i + 1;
        }
    }
}

/* Loads the PVS resource and keeps the lists for one screen and for the number of players: per section, a count of
   near and of far sections, then their indices (sPvsIndex points at each section's). */
void tube_load_pvs(void)
{
    s32 idx;
    s32 i;
    u32 j;
    s32 n;
    s32 numFar, numOther;
    u8 *dst;
    s32 *index;
    s32 sz;
    s32 *entry;
    u16 *data;
    s32 t1, t2;

    while (sched_get_current_task() != 0) { /* the Z buffer is free once the GPU is idle */
    }
    sPvsData = (u16 *)track_res_get(gGidVisInfo);
    if (gTrackSections->count != *sPvsData) {
        fatal_halt(1);
    }
    idx = 1;
    dst = gZBuffer;
    index = (s32 *)(gZBuffer + 0x1E000);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < gTrackSections->count; j++) {
            numFar = sPvsData[idx];
            numOther = sPvsData[idx + 1];
            if (i == 0 || i == gGame.numPlayers - 1) {
                n = numFar + numOther;
                *index++ = dst - gZBuffer;
                memcpy(dst, (u16 *)((idx << 1) + (s32)sPvsData), (n + 2) * 2);
                dst += n * 2 + 4;
            }
            t1 = idx + 2;
            t2 = numFar + numOther;
            idx = t1 + t2;
        }
    }
    idx = 0;
    res_unload(&gResourceSet->handles[track_res_find(gGidVisInfo)]);
    i = dst - gZBuffer;
    sPvsData = mem_alloc_dbg(i, idx, sStrTubegfxC, 0x61B);
    memcpy(sPvsData, gZBuffer, i);
    i = (s32)index + (-0x1E000 - (s32)gZBuffer);
    sPvsIndex = mem_alloc_dbg(i, idx, sStrTubegfxC, 0x620);
    memcpy(sPvsIndex, gZBuffer + 0x1E000, i);
    data = sPvsData;
    entry = (s32 *)sPvsIndex;
    for (; idx < i / 4; idx++) {
        *entry++ += (s32)data;
    }
}

/* gTracks as this file sees it: the fog range per number of screens */
typedef struct { u8 pad[0x58]; u16 fogNear[4]; u16 fogFar[4]; u8 rest[0xA0 - 0x68]; } Trk_7;

extern Trk_7 *gCurTrack_19;
extern Trk_7 gTracks_8[];

/* A view's fog range: the track's, moved by the racing line's per-node offsets (interpolated along the camera's
   segment). */
void tube_calc_fog_range(s32 view, s32 *fogNear, s32 *fogFar)
{
    f32 v[3];
    s32 seg;
    f32 t, nearT, farT, nearF, farF;
    s32 shift, sh, shift4;
    s32 nearOfs, farOfs, nearStep, farStep;
    u32 range, nextRange;
    PathNode *node, *nodes;
    s32 near, far, j, screens;

    v[0] = (f32)gCamPos[view].x / 1000.0f;
    v[1] = (f32)gCamPos[view].y / 1000.0f;
    v[2] = (f32)gCamPos[view].z / 1000.0f;
    path_get_segment_and_t(v, &seg, &t, gRiders[gCamTargets[view]].segment);
    nodes = gPathNodes;
    screens = gNumScreens;
    shift = screens - 1;
    node = (PathNode *)(seg * sizeof(PathNode) + (u32)nodes);
    j = node->next;
    if (shift > 2) {
        shift = 2;
    }
    shift *= 8;
    shift4 = shift + 4;
    range = node->fogRange;
    nearOfs = (range >> shift4) & 0xF;
    if (nearOfs >= 8) nearOfs -= 16;
    farOfs = (range >> shift) & 0xF;
    if (farOfs >= 8) farOfs -= 16;
    nextRange = ((PathNode *)(j * sizeof(PathNode) + (u32)nodes))->fogRange;
    nearStep = (nextRange >> shift4) & 0xF;
    if (nearStep >= 8) nearStep -= 16;
    farStep = (nextRange >> shift) & 0xF;
    if (farStep >= 8) farStep -= 16;
    nearStep -= nearOfs;
    nearStep *= 16;
    farStep -= farOfs;
    farStep *= 2;
    nearT = (f32)nearStep * t;
    farT = (f32)farStep * t;
    nearOfs *= 16;
    farOfs *= 2;
    nearF = (f32)nearOfs + nearT;
    farF = (f32)farOfs + farT;
    near = (s32)nearF;
    far = (s32)farF;
    near = gCurTrack_19->fogNear[screens - 1] - near;
    far = gCurTrack_19->fogFar[screens - 1] - far;
    if (far < near) {
        near = far;
    }
    if (gCurTrack_19 - gTracks_8 == TRACK_CONGO_COURSE && screens == 1) {
        near = 1000;
        far = 0x41A;
    }
    *fogNear = near;
    *fogFar = far;
}

/* Draws the near part of an outdoor track: the texture sets' palettes in segments 1-3, then the sections and
   objects within the fog and beyond it. */
Gfx *tube_draw(Gfx *gdl, s32 dyn, s32 fb, s32 view) {
    Gfx *p;

    gSPSegment(gdl++, 1, gObjectTexSet->pal);
    if (gAltTexSet != NULL) {
        gSPSegment(gdl++, 2, gAltTexSet->pal);
    }
    if (gAnimTexSet != NULL) {
        gSPSegment(gdl++, 3, gAnimTexSet->pal);
    }
    p = tube_draw_layer(gdl, dyn, fb, view, 0);
    return tube_draw_layer(p, dyn, fb, view, 1);
}

/* A matrix of the frame's pool placing a model at (x, y, z) (16 mm units) relative to the view's camera, or NULL
   when the pool is used up. */
short *tube_alloc_mtx(s32 view, s32 x, s32 y, s32 z)
{
    short *mtx;
    s32 count;
    s32 offset;

    count = sTubeMtxCount;
    if (count >= TUBE_MTX_POOL) {
        return 0;
    }
    x = x * 0x10 - gCamPos[view].x;
    y = y * 0x10 - gCamPos[view].y;
    z = z * 0x10 - gCamPos[view].z;
    offset = count * sizeof(Mtx);
    mtx = (short *)((u8 *)sTubeMtxPool[gGfxFrameIndex] + offset);
    mtx_from_angles_pos_scale(mtx, 0, 0, 0, x, y, z, 0x100);
    sTubeMtxCount = sTubeMtxCount + 1;
    return mtx;
}

/* Points segments 8 and up at the current frame of each animated texture the object uses. */
Gfx *tube_set_anim_tex_segs(Gfx *gdl, s32 object)
{
    if (gAnimTexSet != NULL && sTexSegListIndex != NULL) {
        TexSegRef *cur = sTexSegListIndex[object];
        TexSegRef *end = sTexSegListIndex[object + 1];
        s32 i;

        for (i = 0; cur != end; cur++, i++) {
            gSPSegment(gdl++, i + 8, gAnimTexSet->tex[cur->offset + cur->groupStart].data);
        }
    }
    return gdl;
}

/* Frees the animated texture segment lists. */
void tube_free_tex_segs(void)
{
    if (sTexSegList != 0) {
        mem_free(sTexSegList);
        sTexSegList = 0;
    }
    if (sTexSegListIndex != 0) {
        mem_free(sTexSegListIndex);
        sTexSegListIndex = 0;
    }
}

/* Shows the broken objects again. */
void tube_reset_objects(void)
{
    TrackModelList *objects = gTrackObjects;
    TrackModelList *list;
    u32 i = 0;
    u32 *flags;
    u32 mask;

    if (objects->count != 0) {
        mask = ~OBJ_HIDDEN;
        list = objects;
        flags = gObjectFlags;
        do {
            i++;
            *flags = *flags & mask;
            flags++;
        } while (i < list->count);
    }
    shatter_reset();
    remove_walls_reset();
}

/* The Odd Color cheat: rotates the vertex colours (r, g, b) -> (g, b, r). */
void tube_rotate_vertex_colors(TrackModelList *list)
{
    u32 i, j;
    for (i = 0; i < list->count; i++) {
        for (j = 0; j < list->e[i].model->numVtx; j++) {
            u32 v = *(u32 *)list->e[i].model->vtx[j].v.cn;
            v = (v & 0xFF) | ((v >> 16) & 0xFF00) | ((v << 8) & 0xFFFF0000);
            *(u32 *)list->e[i].model->vtx[j].v.cn = v;
        }
    }
}

/* Every other frame: the next frame of the animated and the water textures. */
void tube_animate_textures(void)
{
    volatile s32 pad[10];
    s32 tick = gRaceFrame / 2;
    if (tick == sTexAnimTick) return;
    sTexAnimTick = tick;
    if (gPodiumMode != 0 && gDesaturation == 0x100) return;
    if (gAnimTexSet != 0) {
        prim_animate_textures(gAnimTexSet, tick);
    }
    if (gWaterTexSet != 0) {
        gPrimIATextures = 1;
        prim_animate_textures(gWaterTexSet, sTexAnimTick);
        gPrimIATextures = 0;
    }
}

/* A big splash where a rider hit deep water, (x, y, z) in mm: 120 frames, drawn with the section under it. */
void tube_spawn_splash(s32 rider, s32 x, s32 y, s32 z)
{
    if (sSplashTimer[rider] == 0 && gRiders[rider].frontWheel.groundArea >= 0) {
        gGroundFound = 0;
        height_get_ground(x, y, z);
        if (gGroundFound != 0) sSplashSection[rider] = gGroundArea;
        else sSplashSection[rider] = gRiderSection[rider];
        sSplashTimer[rider] = 120;
        sSplashPos[rider].x = x >> 4;
        sSplashPos[rider].y = y >> 4;
        sSplashPos[rider].z = z >> 4;
        snd_play_rider_sfx(6, rider, 0xff); /* the big splash sound */
    }
}

/* Counts the splashes down. */
void tube_update_splashes(void)
{
    s32 i;
    s32 *timer;

    i = 0;
    timer = sSplashTimer;
    do {
        i = i + 1;
        if (*timer != 0) {
            *timer = *timer - 1;
        }
        timer = timer + 1;
    } while (i < 8);
}

/* prim_build_dl's texture mode callback: calls the render mode display list of segment 7 (cut-out) or 6
   (opaque), which the drawing code points at the fogged or plain versions. */
Gfx *tube_tex_mode_callback(Gfx *gdl, s32 cutout)
{
    if (cutout) {
        gSPDisplayList(gdl++, 0x07000000);
    } else {
        gSPDisplayList(gdl++, 0x06000000);
    }
    return gdl;
}
