#include "game.h"

/* Loads a stadium track: its sections, objects, water and stands (for the user track, built from the editor's
   pieces), their display lists, the ground and walls, the instances, and the section draw order. */
void arn_init(void)
{
    s32 index;
    s32 count;

    gFarClip = 0x4000;
    /* (gCurTrack - gTracks_2) / 0xA0, the track index, as a multiplication */
    index = (s32)(gCurTrack - gTracks_2) * -0x33333333 >> 5;
    sArnNoFarLod = 0;
    if (index == TRACK_STUNT_COURSE) {
        sArnNoFarLod = 1;
    }
    if (index == TRACK_SOCCER) {
        sArnNoFarLod = 1;
    }
    res_register_type(sStrArnArn, (s32)arn_relocate);
    res_register_type(sStrArnWal, (s32)height_init_walls_reloc);
    if ((*(u32 *)(gCurTrack + 0x50) & TRACK_FLAG_USER) != 0) {
        usrtrk_load_pieces();
        usrtrk_build_track();
        if (gUserTrackData.collision != 0) {
            gTrackWallMesh = gUserTrackData.collision;
        }
        gWaterModels = 0;
        gWaterTexSet = 0;
        gTerrainTexSet = gUserTrackData.dirtTex;
        gObjectTexSet = gUserTrackData.objectTex;
        sStandModels = (TrackModelList *)res_load(gResourceSet, sUserTrackStandResId);
        sStandLowModel = (TrackModelList *)res_load(gResourceSet, sUserTrackStandLowResId);
        sStandTextures = (TexSet *)res_load(gResourceSet, sUserTrackStandTexResId);
        sObjectSegs[1] = 0;
        sObjectSegs[2] = 0;
        gAnimTexSet = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections = (TrackModelList *)gUserTrackData.meshA;
        gSectionDLs = tube_build_dlists((TrackModelList *)gUserTrackData.meshA, &gTerrainTexSet, 0, 0);
        gTrackObjects = (TrackModelList *)gUserTrackData.meshB;
        gObjectDLs = tube_build_dlists((TrackModelList *)gUserTrackData.meshB, sObjectSegs,
                                       PRIM_LOAD_TLUT | PRIM_RESET_TLUT, 1);
        arn_apply_noise_shading(gTrackSections);
    } else {
        gTrackWallMesh = track_res_get(gGidWalCol);
        gWaterModels = (TrackModelList *)track_res_get(gGidArnWater);
        gWaterTexSet = (TexSet *)track_res_get(gGidPkgWater);
        gTerrainTexSet = (TexSet *)track_res_get(gGidPkgGround);
        gObjectTexSet = (TexSet *)track_res_get(gGidPkgScene);
        sStandModels = (TrackModelList *)track_res_get(gGidArnStand);
        sStandLowModel = (TrackModelList *)track_res_get(gGidArnStandLow);
        sStandTextures = (TexSet *)track_res_get(gGidPkgStand);
        gAnimTexSet = (TexSet *)track_res_get(gGidPkgAnim);
        sObjectSegs[1] = gAnimTexSet;
        sObjectSegs[2] = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections = (TrackModelList *)track_res_get(gGidArnGround);
        gSectionDLs = tube_build_dlists(gTrackSections, &gTerrainTexSet, 0, 0);
        gTrackObjects = (TrackModelList *)track_res_get(gGidArnScene);
        gObjectDLs = tube_build_dlists(gTrackObjects, sObjectSegs, PRIM_LOAD_TLUT | PRIM_RESET_TLUT, 1);
    }
    mtx_from_angles_pos_scale(D_800E7730, 0, 0, 0, 0, 0, 0, 300);
    height_init(gTrackSections, gTrackObjects, gTrackWallMesh, NULL);
    index = track_res_find(gGidInstanceDef);
    instances_load(gResourceSet, index, 1 /* world coordinates */, gTrackSections, gObjectTexSet);
    if ((gNumScreens == 1) && (sArnNoFarLod == 0)) {
        count = gTrackSections->count;
        /* twice as many slots as sections */
        sSectionDrawOrderLen = count << 1;
        sSectionDrawOrder = mem_alloc_dbg(count << 2, 0, (s32)sStrArngfxC, 0xC9);
    } else {
        sSectionDrawOrder = 0;
    }
}

/* gCurTrack as this file sees it: the fog range per number of screens */
typedef struct {
    u8 pad[0x58];
    u16 fogmin[4];
    u16 fogmax[4];
} FogTab;

/* Draws the near part of a stadium track for a view: the objects, the sections (in the draw order for view 0)
   without and then with fog, and the stands (one low model with three or four views). */
Gfx *arn_draw(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    u32 i, n;
    s32 j;
    s32 k;
    u32 mask;
    FogTab *track;
    TrackModel *section;

    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (gDrawFlags & DRAW_BALES) {
        /* segments 6 and 7: the render modes for opaque and cut-out textures (prim_build_dl's callback) */
        gSPSegment(gdl++, 6, sTexOpaqueDL);
        gSPSegment(gdl++, 7, sTexCutoutDL);
        for (i = 0; i < gTrackObjects->count; i++) {
            mask = gTrackObjects->e[i].mask;
            if ((mask & TRACK_MODEL_VISIBLE(view)) && !(mask & TRACK_MODEL_FAR(view))) {
                gdl = tube_set_anim_tex_segs(gdl, i);
                gSPDisplayList(gdl++, gObjectDLs[i]);
            }
        }
    }
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet->pal);
    if (sSectionDrawOrder != NULL && view == 0) {
        for (j = 0; j < sSectionDrawOrderLen; j++) {
            k = sSectionDrawOrder[j];
            if (k < 0) continue;
            section = &gTrackSections->e[k];
            mask = section->mask;
            if ((mask & TRACK_MODEL_VISIBLE(view)) && !(mask & TRACK_MODEL_FAR(view))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->count; k++) {
            section = &gTrackSections->e[k];
            mask = section->mask;
            if ((mask & TRACK_MODEL_VISIBLE(view)) && !(mask & TRACK_MODEL_FAR(view))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    track = gCurTrack;
    gSPFogPosition(gdl++, track->fogmin[gNumScreens - 1], track->fogmax[gNumScreens - 1]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFFFFF);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    if (sSectionDrawOrder != NULL && view == 0) {
        for (j = 0; j < sSectionDrawOrderLen; j++) {
            k = sSectionDrawOrder[j];
            if (k < 0) continue;
            section = &gTrackSections->e[k];
            mask = section->mask;
            if ((mask & TRACK_MODEL_VISIBLE(view)) && (mask & TRACK_MODEL_FAR(view))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->count; k++) {
            section = &gTrackSections->e[k];
            mask = section->mask;
            if ((mask & TRACK_MODEL_VISIBLE(view)) && (mask & TRACK_MODEL_FAR(view))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, sStandTextures->pal);
    if (gNumViews < 3) {
        for (i = 0; i < sStandModels->count; i++) {
            if (gDrawFlags & DRAW_STAND) {
                gdl = prim_build_dl(PRIM_LOAD_TLUT, sStandModels->e[i].model, &sStandTextures, gdl);
            }
        }
    } else {
        gdl = prim_build_dl(PRIM_LOAD_TLUT, sStandLowModel->e[0].model, &sStandTextures, gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    u8 pad[0x58];
    u16 fogMin[4];
    u16 fogMax[4];
} Fog;

extern Fog *gCurTrack_18;

/* Draws the water models visible in a view, IA textures tinted by their palette colour. */
Gfx *arn_draw_water(Gfx *gdl, s32 dyn, s32 fb, u32 view)
{
    u32 i;
    u32 offset;
    u8 pad[0x48];
    TrackModelList *list = gWaterModels;

    if (list == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    /* the track's fog range with near and far swapped */
    gSPFogPosition(gdl++, gCurTrack_18->fogMax[gNumScreens - 1], gCurTrack_18->fogMin[gNumScreens - 1]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    for (i = 0; i < gWaterModels->count; i++) {
        offset = i * sizeof(TrackModel) + 4; /* &gWaterModels->e[i] */
        if ((((TrackModel *)((u8 *)gWaterModels + offset))->mask >> (view << 1)) & 1) {
            gdl = prim_build_dl(0, gWaterModels->e[i].model, &gWaterTexSet, gdl);
        }
    }
    gDPPipeSync(gdl++);
    gPrimIATextures = 0;
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    return gdl;
}

/* Draws the far part of a stadium track for a view: the objects beyond the level-of-detail distance, fogged, with
   the far viewport. */
Gfx *arn_draw_far(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    s32 pos[4];
    s32 out[3];
    s32 mtx[10];
    u16 *fog;
    u32 i;

    mtx3i_from_angles(mtx, 0, 0, gCamYaw[view]);
    pos[0] = 0;
    pos[1] = -30;
    pos[2] = 0;
    vec3i_mul_mtx3i(pos, mtx, out);
    mtx_from_angles_pos_scale(D_800E7730, 0, 0, 0, out[0], out[1], out[2], 0x100);

    gSPViewport(gdl++, &gViewports[view + 4]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    /* the track's fog range (+0x58, +0x60) for the number of screens */
    fog = (u16 *)((u8 *)gCurTrack + ((gNumScreens - 1) << 1));
    gSPFogPosition(gdl++, fog[0x2C], fog[0x30]);
    gSPSegment(gdl++, 6, sTexOpaqueFogDL);
    gSPSegment(gdl++, 7, sTexCutoutFogDL);

    for (i = 0; i < gTrackObjects->count; i++) {
        if ((gTrackObjects->e[i].mask & TRACK_MODEL_VISIBLE(view)) &&
            (gTrackObjects->e[i].mask & TRACK_MODEL_FAR(view))) {
            gdl = tube_set_anim_tex_segs(gdl, i);
            gSPDisplayList(gdl++, gObjectDLs[i]);
        }
    }

    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

/* Sets each model's bits for a view: visible when in front of the camera within the field of view or close to it,
   far beyond the level-of-detail distance; with `slots`, also sorts the visible ones into
   distance slots (the stadium's section draw order). */
void arn_cull_models(TrackModelList *list, s32 view, s32 strict, s16 *slots, s32 nslots)
{
    s32 unused[10];
    s32 rel[4];
    s32 rot[4];
    s32 camX;
    s32 camY;
    s32 camZ;
    s32 far;
    s32 near;
    s32 cosYaw;
    s32 sinYaw;
    u32 i;
    s32 side;
    s32 halfFov;
    s32 slot;
    s32 dx;
    s32 dz;
    TrackModel *model;

    far = gLoDetailThresh[gNumScreens - 1];
    near = gVisibleThresh[gNumScreens - 1];
    if (slots != NULL) {
        for (cosYaw = 0; cosYaw < nslots; cosYaw++) { /* cosYaw is only a counter here */
            slots[cosYaw] = -1;
        }
    }
    camX = gCamPos[view].x >> 4;
    cosYaw = gCosTable[(gCamYaw[view] >> 4) & 0xFFF];
    sinYaw = gCosTable[((gCamYaw[view] >> 4) - 0x400) & 0xFFF];
    camY = gCamPos[view].y >> 4;
    camZ = gCamPos[view].z >> 4;
    for (i = 0; i < list->count; i++) {
        model = &list->e[i];
        dx = ((model->x1 + model->x0) >> 1) - camX;
        dz = ((model->z1 + model->z0) >> 1) - camZ;
        model->visible = 1;
        model->mask &= ~TRACK_MODEL_FAR(view);
        rel[0] = dx;
        rel[1] = -camY;
        rel[2] = dz;
        /* the centre in camera space: rot[0] across, rot[2] ahead */
        rot[0] = (cosYaw * dx + sinYaw * dz) >> 14;
        rot[2] = (cosYaw * dz - sinYaw * dx) >> 14;
        if (far < rot[2] && strict != 0) {
            model->mask |= TRACK_MODEL_FAR(view);
        }
        /* always drawn when close to the camera */
        if (gGame.mode == GAME_MODE_TUTORIAL || gGame.mode == GAME_MODE_SOCCER || gNumScreens == 2) {
            if (rot[2] >= -799 && rot[2] < 1200 && rot[0] >= -799 && rot[0] < 800) goto visible;
        } else {
            if (rot[2] >= -399 && rot[2] < 800 && rot[0] >= -399 && rot[0] < 400) goto visible;
        }
        if (rot[2] < 0) {
            goto reject;
        }
        side = rot[0];
        if (side < -180) {
            side += 360;
        } else if (side > 180) {
            side -= 360;
        }
        /* within the field of view: |side| <= ahead / 2 (wider for a horizontal split) */
        halfFov = rot[2];
        halfFov /= 2;
        if (gNumScreens == 2 && gInsetViewActive == 0 && gSplitVertical == 0) {
            halfFov = halfFov * 3 >> 1;
            if (gCamPitch[0] > 0x1500) {
                halfFov = (halfFov * 4) / 3;
            }
        }
        if (halfFov < side) {
            goto reject;
        }
        if (side < -halfFov) {
            goto reject;
        }
        if (rot[2] > near && strict != 0) {
            goto hidden;
        }
    visible:
        model->visible = 1;
        model->mask |= TRACK_MODEL_VISIBLE(view);
        if (slots != NULL) {
            slot = rot[2] * nslots / gVisibleThresh[gNumScreens - 1];
            if (slot < 0) {
                slot = 0;
            }
            if (slot >= nslots) {
                slot = nslots - 1;
            }
            while (slots[slot] != -1) {
                slot = (slot + 1) % nslots;
            }
            slots[slot] = i;
        }
        continue;
    reject:
        model->visible = 0;
    hidden:
        model->mask &= ~TRACK_MODEL_VISIBLE(view);
    }
}

/* Shades the vertices of the user track's sections with smoothed noise (a 256x256 map in the Z buffer), stretched
   over the bounding box of all sections. */
void arn_apply_noise_shading(TrackModelList *sections)
{
    s32 minZ = 9999999;
    s32 minX = 9999999;
    s32 maxZ = -9999999;
    s32 maxX = -9999999;
    s32 i, offset;
    u32 k;
    TrackModel *section;
    u8 *noise;
    u8 *smooth;
    s32 sizeX, sizeZ, u, v;
    PrimModel *mesh;
    Vtx *vtx;
    u8 shade;

    for (i = 0; i < sections->count; i++) {
        section = &sections->e[i];
        if (section->x0 < minX) minX = section->x0;
        if (section->z0 < minZ) minZ = section->z0;
        if (maxX < section->x1) maxX = section->x1;
        if (maxZ < section->z1) maxZ = section->z1;
    }
    while (sched_get_current_task() != 0) { /* the Z buffer is free once the GPU is idle */
    }
    noise = gZBuffer;
    smooth = noise + 0x10000;
    for (i = 0; i < 0x10000; i++) {
        noise[i] = random_byte();
    }
    for (i = 0; i < 0x10000; i++) {
        smooth[i] = (noise[i] + noise[i + 0x100] + noise[i - 0x100] + noise[i + 1] + noise[i - 1]) / 5;
    }
    i = 0;
    if (sections->count != 0) {
        sizeX = maxX - minX;
        sizeZ = maxZ - minZ;
        offset = 4;
        do {
            section = (TrackModel *)((u8 *)sections + offset);
            mesh = section->model;
            for (k = 0; k < mesh->numVtx; k++) {
                vtx = &mesh->vtx[k];
                u = vtx->v.ob[0];
                u = (u - minX) * 64 / sizeX + 5;
                v = vtx->v.ob[2];
                v -= minZ;
                v = v * 64 / sizeZ + 5;
                shade = smooth[(u << 8) + v];
                vtx->v.cn[0] = shade;
                vtx->v.cn[1] = shade;
                vtx->v.cn[2] = shade;
            }
            offset += 0x20;
            i++;
        } while (i < sections->count);
    }
}

/* Frees the stadium's display lists, the user track, the ground and walls, and the draw order. */
void arn_free(void)
{
    u32 i;

    i = 0;
    if (gTrackSections->count != 0) {
        do {
            mem_free(gSectionDLs[i]);
            i = i + 1;
        } while (i < gTrackSections->count);
    }
    i = 0;
    if (gTrackObjects->count != 0) {
        do {
            mem_free(gObjectDLs[i]);
            i = i + 1;
        } while (i < gTrackObjects->count);
    }
    mem_free((s32)gSectionDLs);
    mem_free((s32)gObjectDLs);
    if ((gCurTrack_3[20] & TRACK_FLAG_USER) != 0) { /* gTracks +0x50 */
        usrtrk_unload();
    }
    height_free();
    tube_free_tex_segs();
    if (sSectionDrawOrder != 0) {
        mem_free(sSectionDrawOrder);
    }
}

/* Culls the stadium's sections, objects and water for a view. */
void arn_update_view(s32 view)
{
    s32 far = (sArnNoFarLod == 0);
    void *order = sSectionDrawOrder;

    if (order != NULL && view == 0) {
        arn_cull_models(gTrackSections, 0, far, order, sSectionDrawOrderLen);
    } else {
        arn_cull_models(gTrackSections, view, far, NULL, 0);
    }
    arn_cull_models(gTrackObjects, view, gGame.mode != GAME_MODE_SOCCER, NULL, 0);
    if (gWaterModels != NULL) {
        arn_cull_models(gWaterModels, view, 1, NULL, 0);
    }
}

/* Relocates a model list resource: each entry's model and ground grid, and the model's pointers. */
void arn_relocate(TrackModelList **pp)
{
    TrackModelList *list = *pp;
    TrackModelList *row; /* entry i is row->e[0]: the walk steps over whole entries from the list's start */
    u32 i;
    PrimModel *model;
    u32 j;
    u32 n;
    if (list->count != 0) {
        i = 0;
        do {
            row = (TrackModelList *)((TrackModel *)list + i);
            row->e[0].model = (PrimModel *)((u32)list + (u32)row->e[0].model);
            row->e[0].grid = (CollGrid *)((u32)list + (u32)row->e[0].grid);
            model = ((volatile TrackModelList *)row)->e[0].model;
            model->vtx = (Vtx *)((u32)list + (u32)model->vtx);
            model->cmds = (u8 *)((u32)list + (u32)model->cmds);
            if (model->unk_14 != NULL) {
                model->unk_14 = (void *)((u32)list + (u32)model->unk_14);
            }

            j = 0;
            if (model->numVtx != 0) {
                do {
                    j++;
                } while (j < model->numVtx);
            }
            i++;
        } while (i < list->count);
    }
}
