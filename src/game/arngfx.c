#include "game.h"

void arn_init(void)
{
    s32 iVar1;
    s32 t;

    gFarClip = 0x4000;
    iVar1 = (s32)(gCurTrack - gTracks_2) * -0x33333333 >> 5;
    D_800E7718 = 0;
    if (iVar1 == 0x18) {
        D_800E7718 = 1;
    }
    if (iVar1 == 0x16) {
        D_800E7718 = 1;
    }
    res_register_type(D_800D6A30, (s32)D_800248EC);
    res_register_type(D_800D6A34, (s32)D_80031D08);
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0) {
        usrtrk_load_pieces();
        usrtrk_build_track();
        if (gUserTrackData.collision != 0) {
            D_800C9574 = gUserTrackData.collision;
        }
        gWaterModels = 0;
        gWaterTexSet = 0;
        gTerrainTexSet = gUserTrackData.dirtTex;
        gObjectTexSet = gUserTrackData.objectTex;
        sStandModels = (TrackModelList *)res_load(D_800E7728, D_800D07C0);
        sStandLowModel = (TrackModelList *)res_load(D_800E7728, D_800D06F4);
        sStandTextures = (TexSet *)res_load(D_800E7728, D_800D07D4);
        sObjectSegs[1] = 0;
        sObjectSegs[2] = 0;
        gAnimTexSet = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections = (TrackModelList *)gUserTrackData.meshA;
        gSectionDLs = tube_build_dlists((TrackModelList *)gUserTrackData.meshA, &gTerrainTexSet, 0, 0);
        gTrackObjects = (TrackModelList *)gUserTrackData.meshB;
        gObjectDLs = tube_build_dlists((TrackModelList *)gUserTrackData.meshB, sObjectSegs, 0xC, 1);
        arn_apply_noise_shading(gTrackSections);
    } else {
        D_800C9574 = track_res_get(D_800D1518);
        gWaterModels = (TrackModelList *)track_res_get(D_800D14E4);
        gWaterTexSet = (TexSet *)track_res_get(D_800D1510);
        gTerrainTexSet = (TexSet *)track_res_get(D_800D1504);
        gObjectTexSet = (TexSet *)track_res_get(D_800D1508);
        sStandModels = (TrackModelList *)track_res_get(D_800D1584);
        sStandLowModel = (TrackModelList *)track_res_get(D_800D1588);
        sStandTextures = (TexSet *)track_res_get(D_800D158C);
        gAnimTexSet = (TexSet *)track_res_get(D_800D1500);
        sObjectSegs[1] = gAnimTexSet;
        sObjectSegs[2] = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections = (TrackModelList *)track_res_get(D_800D14DC);
        gSectionDLs = tube_build_dlists(gTrackSections, &gTerrainTexSet, 0, 0);
        gTrackObjects = (TrackModelList *)track_res_get(D_800D14E0);
        gObjectDLs = tube_build_dlists(gTrackObjects, sObjectSegs, 0xC, 1);
    }
    mtx_from_angles_pos_scale(D_800E7730, 0, 0, 0, 0, 0, 0, 300);
    height_init(gTrackSections, gTrackObjects, D_800C9574, NULL);
    iVar1 = track_res_find(D_800D1528);
    instances_load(D_800E7728, iVar1, 1, gTrackSections, gObjectTexSet);
    if ((gNumScreens == 1) && (D_800E7718 == 0)) {
        t = gTrackSections->count;
        sSectionDrawOrderLen = t << 1;
        sSectionDrawOrder = mem_alloc_dbg(t << 2, 0, (s32)D_800D6A38, 0xC9);
    } else {
        sSectionDrawOrder = 0;
    }
}

typedef struct {
    u8 pad[0x58];
    u16 fogmin[4];
    u16 fogmax[4];
} FogTab;

Gfx *arn_draw(Gfx *gdl, s32 a1, s32 a2, s32 view)
{
    u32 i, n;
    s32 j;
    s32 k;
    u32 m;
    FogTab *fog;
    TrackModel *it;

    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (gDrawFlags & 0x20) {
        gSPSegment(gdl++, 6, sTexOpaqueDL);
        gSPSegment(gdl++, 7, sTexCutoutDL);
        for (i = 0; i < gTrackObjects->count; i++) {
            m = gTrackObjects->e[i].mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
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
            it = &gTrackSections->e[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->count; k++) {
            it = &gTrackSections->e[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    fog = gCurTrack;
    gSPFogPosition(gdl++, fog->fogmin[gNumScreens - 1], fog->fogmax[gNumScreens - 1]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFFFFF);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    if (sSectionDrawOrder != NULL && view == 0) {
        for (j = 0; j < sSectionDrawOrderLen; j++) {
            k = sSectionDrawOrder[j];
            if (k < 0) continue;
            it = &gTrackSections->e[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->count; k++) {
            it = &gTrackSections->e[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
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
            if (gDrawFlags & 0x10) {
                gdl = prim_build_dl(4, sStandModels->e[i].model, &sStandTextures, gdl);
            }
        }
    } else {
        gdl = prim_build_dl(4, sStandLowModel->e[0].model, &sStandTextures, gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    u8 pad[0x58];
    u16 fogMax[4];
    u16 fogMin[4];
} Fog;

extern Fog *gCurTrack_18;

Gfx *arn_draw_water(Gfx *gdl, s32 p2, s32 p3, u32 sh)
{
    u32 i;
    u32 off;
    u8 pad[0x48];
    TrackModelList *tbl = gWaterModels;

    if (tbl == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, gCurTrack_18->fogMin[gNumScreens - 1], gCurTrack_18->fogMax[gNumScreens - 1]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    for (i = 0; i < gWaterModels->count; i++) {
        off = i * 32 + 4;
        if ((((TrackModel *)((u8 *)gWaterModels + off))->mask >> (sh << 1)) & 1) {
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

Gfx *arn_draw_far(Gfx *gdl, s32 a1, s32 a2, s32 mode)
{
    s32 pos[4];
    s32 out[3];
    s32 mtx[10];
    u16 *fog;
    u32 i;

    mtx3i_from_angles(mtx, 0, 0, gCamYaw[mode]);
    pos[0] = 0;
    pos[1] = -30;
    pos[2] = 0;
    vec3i_mul_mtx3i(pos, mtx, out);
    mtx_from_angles_pos_scale(D_800E7730, 0, 0, 0, out[0], out[1], out[2], 0x100);

    gSPViewport(gdl++, &gViewports[mode + 4]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, 0x200004);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    fog = (u16 *)((u8 *)gCurTrack + ((gNumScreens - 1) << 1));
    gSPFogPosition(gdl++, fog[0x2C], fog[0x30]);
    gSPSegment(gdl++, 6, sTexOpaqueFogDL);
    gSPSegment(gdl++, 7, sTexCutoutFogDL);

    for (i = 0; i < gTrackObjects->count; i++) {
        if ((gTrackObjects->e[i].mask & (1 << (mode * 2))) && (gTrackObjects->e[i].mask & (2 << (mode * 2)))) {
            gdl = tube_set_anim_tex_segs(gdl, i);
            gSPDisplayList(gdl++, gObjectDLs[i]);
        }
    }

    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

/* Sets each model's view bits for view p: visible (bit 2p) when in front of the camera within the field of view or
   close to it, far (bit 2p + 1) beyond the level-of-detail distance; with `slots`, also sorts the visible ones into
   distance slots (the stadium's section draw order). */
void arn_cull_models(TrackModelList *list, s32 p, s32 strict, s16 *slots, s32 nslots)
{
    s32 unused[10];
    s32 d[4];
    s32 r[4];
    s32 cam0;
    s32 cam1;
    s32 cam2;
    s32 far;
    s32 near;
    s32 j;
    s32 sn;
    s32 cs;
    s32 k17;
    s32 k5;
    s32 n;
    u32 i;
    u32 m2;
    u32 m1;
    s32 a;
    s32 h;
    s32 idx;
    s32 ex;
    s32 ey;
    TrackModel *rec;

    far = gLoDetailThresh[gNumScreens - 1];
    near = gVisibleThresh[gNumScreens - 1];
    if (slots != NULL) {
        for (sn = 0; sn < nslots; sn++) {
            slots[sn] = -1;
        }
    }
    cam0 = gCamPos[p].x >> 4;
    sn = gCosTable[(gCamYaw[p] >> 4) & 0xFFF];
    cs = gCosTable[((gCamYaw[p] >> 4) - 0x400) & 0xFFF];
    cam1 = gCamPos[p].y >> 4;
    cam2 = gCamPos[p].z >> 4;
    for (i = 0; i < list->count; i++) {
        rec = &list->e[i];
        ex = ((rec->x1 + rec->x0) >> 1) - cam0;
        ey = ((rec->z1 + rec->z0) >> 1) - cam2;
        rec->visible = 1;
        rec->mask &= ~(2 << (p * 2));
        d[0] = ex;
        d[1] = -cam1;
        d[2] = ey;
        r[0] = (sn * ex + cs * ey) >> 14;
        r[2] = (sn * ey - cs * ex) >> 14;
        if (far < r[2] && strict != 0) {
            rec->mask |= 2 << (p * 2);
        }
        if (gGame.mode == 9 || gGame.mode == 2 || gNumScreens == 2) {
            if (r[2] >= -799 && r[2] < 1200 && r[0] >= -799 && r[0] < 800) goto visible;
        } else {
            if (r[2] >= -399 && r[2] < 800 && r[0] >= -399 && r[0] < 400) goto visible;
        }
        if (r[2] < 0) {
            goto reject;
        }
        a = r[0];
        if (a < -180) {
            a += 360;
        } else if (a > 180) {
            a -= 360;
        }
        h = r[2];
        h /= 2;
        if (gNumScreens == 2 && gInsetViewActive == 0 && gSplitVertical == 0) {
            h = h * 3 >> 1;
            if (gCamPitch[0] > 0x1500) {
                h = (h * 4) / 3;
            }
        }
        if (h < a) {
            goto reject;
        }
        if (a < -h) {
            goto reject;
        }
        if (r[2] > near && strict != 0) {
            goto hidden;
        }
    visible:
        rec->visible = 1;
        rec->mask |= 1 << (p * 2);
        if (slots != NULL) {
            idx = r[2] * nslots / gVisibleThresh[gNumScreens - 1];
            if (idx < 0) {
                idx = 0;
            }
            if (idx >= nslots) {
                idx = nslots - 1;
            }
            while (slots[idx] != -1) {
                idx = (idx + 1) % nslots;
            }
            slots[idx] = i;
        }
        continue;
    reject:
        rec->visible = 0;
    hidden:
        rec->mask &= ~(1 << (p * 2));
    }
}

/* Shades the vertices of the user track's sections with smoothed noise (a 256x256 map in the Z buffer), stretched
   over the bounding box of all sections. */
void arn_apply_noise_shading(TrackModelList *m)
{
    s32 minB = 9999999;
    s32 minA = 9999999;
    s32 maxB = -9999999;
    s32 maxA = -9999999;
    s32 i, off, off2;
    u32 k;
    TrackModel *it;
    u8 *buf;
    u8 *out;
    s32 dx, dy, ix, iy;
    PrimModel *mesh;
    Vtx *vt;
    u8 c;

    for (i = 0; i < m->count; i++) {
        it = &m->e[i];
        if (it->x0 < minA) minA = it->x0;
        if (it->z0 < minB) minB = it->z0;
        if (maxA < it->x1) maxA = it->x1;
        if (maxB < it->z1) maxB = it->z1;
    }
    while (sched_get_current_task() != 0) {
    }
    buf = gZBuffer;
    out = buf + 0x10000;
    for (i = 0; i < 0x10000; i++) {
        buf[i] = random_byte();
    }
    for (i = 0; i < 0x10000; i++) {
        out[i] = (buf[i] + buf[i + 0x100] + buf[i - 0x100] + buf[i + 1] + buf[i - 1]) / 5;
    }
    i = 0;
    if (m->count != 0) {
        dx = maxA - minA;
        dy = maxB - minB;
        off2 = 4;
        do {
            it = (TrackModel *)((u8 *)m + off2);
            mesh = it->model;
            for (k = 0; k < mesh->numVtx; k++) {
                vt = &mesh->vtx[k];
                ix = vt->v.ob[0];
                ix = (ix - minA) * 64 / dx + 5;
                iy = vt->v.ob[2];
                iy -= minB;
                iy = iy * 64 / dy + 5;
                c = out[(ix << 8) + iy];
                vt->v.cn[0] = c;
                vt->v.cn[1] = c;
                vt->v.cn[2] = c;
            }
            off2 += 0x20;
            i++;
        } while (i < m->count);
    }
}

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
    if ((gCurTrack_3[20] & 2) != 0) {
        usrtrk_unload();
    }
    height_free();
    tube_free_tex_segs();
    if (sSectionDrawOrder != 0) {
        mem_free(sSectionDrawOrder);
    }
}

void arn_update_view(s32 param_1)
{
    s32 f = (D_800E7718 == 0);
    void *p3 = sSectionDrawOrder;

    if (p3 != NULL && param_1 == 0) {
        arn_cull_models(gTrackSections, 0, f, p3, sSectionDrawOrderLen);
    } else {
        arn_cull_models(gTrackSections, param_1, f, NULL, 0);
    }
    arn_cull_models(gTrackObjects, param_1, gGame.mode != 2, NULL, 0);
    if (gWaterModels != NULL) {
        arn_cull_models(gWaterModels, param_1, 1, NULL, 0);
    }
}

/* Relocates a model list resource: each entry's model and ground grid, and the model's pointers. */
void arn_relocate(TrackModelList **pp)
{
    TrackModelList *list = *pp;
    TrackModelList *row; /* entry i is row->e[0]: the walk steps over whole entries from the list's start */
    u32 i;
    PrimModel *mdl;
    u32 j;
    u32 n;
    if (list->count != 0) {
        i = 0;
        do {
            row = (TrackModelList *)((TrackModel *)list + i);
            row->e[0].model = (PrimModel *)((u32)list + (u32)row->e[0].model);
            row->e[0].grid = (CollGrid *)((u32)list + (u32)row->e[0].grid);
            mdl = ((volatile TrackModelList *)row)->e[0].model;
            mdl->vtx = (Vtx *)((u32)list + (u32)mdl->vtx);
            mdl->cmds = (u8 *)((u32)list + (u32)mdl->cmds);
            if (mdl->unk_14 != NULL) {
                mdl->unk_14 = (void *)((u32)list + (u32)mdl->unk_14);
            }

            j = 0;
            if (mdl->numVtx != 0) {
                do {
                    j++;
                } while (j < mdl->numVtx);
            }
            i++;
        } while (i < list->count);
    }
}
