#include "game.h"

static inline MtxS *getmtx(s32 x, s32 y, s32 z, s32 view)
{
    MtxS *m;

    if (sTubeMtxCount >= 300) {
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

void tube_init(void)
{
    s32 i, j;
    TexSet **terrain;
    WallMesh **walls;

    for (i = 0; i < 2; i++) {
        sTubeMtxPool[i] = mem_alloc_dbg(0x4B00, 0, D_800D6A50, 0xCA);
        for (j = 0; j < 300; j++) {
            mtx_from_angles_pos_scale(&sTubeMtxPool[i][j], 0, 0, 0, 0, 0, 0, 0x100);
        }
    }
    D_800E7820 = -1;
    gFarClip = 8000;
    res_register_type((u8 *)D_800D6A5C, (s32)arn_relocate);
    res_register_type((u8 *)D_800D6A60, (s32)height_init_walls_reloc);
    gObjectTexSet = (TexSet *)track_res_get(D_800D1508);
    terrain = &gTerrainTexSet;
    *terrain = (TexSet *)track_res_get(D_800D1504);
    gWaterTexSet = (TexSet *)track_res_get(D_800D1510);
    gAnimTexSet = (TexSet *)track_res_get(D_800D1500);
    D_800E77C4 = (TexSet *)track_res_get(D_800D14FC);
    gTrackObjects = (TrackModelList *)track_res_get(D_800D14E0);
    sObjectSegs[0] = gObjectTexSet;
    sObjectSegs[1] = D_800E77C4;
    sObjectSegs[2] = gAnimTexSet;
    gObjectDLs = tube_build_dlists(gTrackObjects, sObjectSegs, 0x4C, 1);
    gTrackSections = (TrackModelList *)track_res_get(D_800D14DC);
    gSectionDLs = tube_build_dlists(gTrackSections, terrain, 0, 0);
    gWaterModels = (TrackModelList *)track_res_get(D_800D14E4);
    if (gCheatFlags & 0x80) {
        tube_rotate_vertex_colors(gTrackSections);
        tube_rotate_vertex_colors(gTrackObjects);
    }
    gObjectFlags = mem_alloc_dbg(gTrackObjects->count * 4, 0, D_800D6A50, 0xF9);
    for (i = 0; i < gTrackObjects->count; i++) {
        gObjectFlags[i] = gTrackObjects->e[i].mask;
    }
    shatter_init(gTrackObjects);
    gSectionMtxs = mem_alloc_dbg(gTrackSections->count * 16, 0, D_800D6A50, 0x103);
    tube_load_pvs();
    for (i = 0; i < 256; i++) {
        if (gObjectTexSet->pal[i] == 0xF83F) {
            gObjectTexSet->pal[i] = 0;
        }
    }
    walls = &D_800C9574;
    *walls = (WallMesh *)track_res_get(D_800D1518);
    remove_walls_init(gTrackObjects, *walls);
    height_init(gTrackSections, gTrackObjects, *walls, (u32 *)track_res_get(D_800D14F4));
    sSectionBounds = mem_alloc_dbg(gTrackSections->count * 16, 0, D_800D6A50, 0x11D);
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
    instances_load(D_800E7728, track_res_find(D_800D1528), 0, gTrackSections, gObjectTexSet);
}

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
    D_800E77C4 = 0;
    gWaterModels = 0;
    gWaterTexSet = 0;
    gTrackSections = NULL;
}

/* Culls the sections for view n, then gives each object and water model the view bits of its section. */
void tube_update_view(s32 n)
{
    u32 i;
    TrackModel *it;

    tube_calc_fog_range(n, &sTubeFogNear[n], &sTubeFogFar[n]);
    tube_cull_sections(gTrackSections, n);
    for (i = 0; i < gTrackObjects->count; i++) {
        it = &gTrackObjects->e[i];
        it->mask = (it->mask & 0xFFFF0000) | gTrackSections->e[it->mask >> 16].mask;
    }
    if (gWaterModels != NULL) {
        for (i = 0; i < gWaterModels->count; i++) {
            it = &gWaterModels->e[i];
            it->mask = (it->mask & 0xFFFF0000) | gTrackSections->e[it->mask >> 16].mask;
        }
    }
    if (gGfxFrameIndex != D_800E7820) {
        sTubeMtxCount = 0;
    }
    D_800E7820 = gGfxFrameIndex;
}

Gfx *tube_draw_layer(Gfx *gdl, s32 mtx, s32 a2, s32 view, s32 fog)
{
    u32 i;
    MtxS *m;
    TrackModel *it;
    u32 sh;

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
        it = &gTrackSections->e[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) continue;
        if (it->mask & (0x100 << view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            gSectionMtxs[i * 4 + view] = m;
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gSectionDLs[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    if (fog != 0) {
        gdl = instances_draw(gdl, (Mtx *)mtx, a2, view, 1);
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
        if (gObjectFlags[i] & 8) continue;
        it = &gTrackObjects->e[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) continue;
        if (it->mask & (0x100 << view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x, z;
            gdl = tube_set_anim_tex_segs(gdl, i);
            x = (it->x0 + it->x1) / 2;
            z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                if (gObjectFlags[i] & 1) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gObjectDLs[i]);
                if (gObjectFlags[i] & 1) {
                    gSPSetGeometryMode(gdl++, gCullMode);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}

Gfx *tube_draw_far(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    u32 i;
    MtxS *m;
    TrackModel *it;
    u32 sh;
    s32 tmp[18];

    gSPSegment(gdl++, 1, gObjectTexSet->pal);
    if (D_800E77C4 != NULL) {
        gSPSegment(gdl++, 2, D_800E77C4->pal);
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
        it = &gTrackSections->e[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            gSectionMtxs[i * 4 + view] = m;
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gSectionDLs[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gdl = instances_draw(gdl, (Mtx *)mtx, a2, view, 3);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPSegment(gdl++, 6, sTexOpaqueFogDL);
    gSPSegment(gdl++, 7, sTexCutoutFogDL);

    for (i = 0; i < gTrackObjects->count; i++) {
        if (gObjectFlags[i] & 8) continue;
        it = &gTrackObjects->e[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x, z;
            gdl = tube_set_anim_tex_segs(gdl, i);
            x = (it->x0 + it->x1) / 2;
            z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                if (gObjectFlags[i] & 1) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, gObjectDLs[i]);
                if (gObjectFlags[i] & 1) {
                    gSPSetGeometryMode(gdl++, gCullMode);
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

Gfx *tube_draw_water(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    s32 i;
    MtxS *m;
    TrackModel *it;
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
    gSPFogPosition(gdl++, sTubeFogFar[view], sTubeFogNear[view]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;

    for (i = 0; i < gWaterModels->count; i++) {
        it = &gWaterModels->e[i];
        if ((it->mask >> (view * 2)) & 1) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gdl = prim_build_dl(0, gWaterModels->e[i].model, &gWaterTexSet, gdl);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    for (i = 0; i < 8; i++) {
        if (sSplashTimer[i] == 0) continue;
        it = &gTrackSections->e[sSplashSection[i]];
        if ((it->mask >> (view * 2)) & 1) {
            s32 a = sSplashTimer[i] / 2;
            s32 f;
            if (a >= 256) {
                a = 255;
            }
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, a);
            f = (120 - sSplashTimer[i]) / 12;
            if (f >= 5) {
                f = (f - 2) % 3 + 2;
            }
            gDPLoadTextureBlock(gdl++, D_800E92C8->tex[f].data, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                G_TX_MIRROR, G_TX_MIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            m = getmtx(sSplashPos[i].x, sSplashPos[i].y, sSplashPos[i].z, view);
            if (m != NULL) {
                mtx_from_angles_pos_scale(m, 0, 0, 0, sSplashPos[i].x * 16 - gCamPos[view].x, sSplashPos[i].y * 16 - gCamPos[view].y,
                              sSplashPos[i].z * 16 - gCamPos[view].z, 0x100);
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(gdl++, sSplashQuadVtx, 4, 0);
                gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
    gPrimIATextures = 0;
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    prim_set_tex_mode_callback(0);
    return gdl;
}

typedef struct { u8 pad[0x5AA]; s16 f5AA; u8 pad2[0x7E0 - 0x5AC]; } Player_9;

extern Player_9 gRiders_22[];

/* Sets the sections' view bits for view n from the PVS list of the section under the camera, then clears those
   behind the camera or outside the field of view and marks the distant ones fogged (0x100 << n). On Excite 3D the
   sections near the leader are visible instead. */
void tube_cull_sections(TrackModelList *l, s32 n)
{
    s32 k;
    s32 i;
    s32 base;
    s32 px, py, pz;
    s32 cnt;
    s32 bit;
    u16 *e;
    s32 n1, tot;
    s32 h;
    s32 lim;
    s32 ang;
    s32 sn, cs;
    s32 d[4];
    s32 w[4];

    k = n;
    if (gCamTargetOverride != -1) {
        k = gCamTargetOverride;
    }
    sCullSlope = 0x68;
    if (gNumScreens == 2) {
        sCullSlope = 0x9C;
    }
    if (gCurTrack_2 - gTracks == 20 && D_800E3D6A == 4) {
        n1 = gRiderSection[0];
        if (n1 >= 0 && n1 < l->count) {
            s32 ref = l->e[n1].z0 + l->e[n1].z1;
            for (i = 0; i < l->count; i++) {
                s32 v = l->e[i].mask & (-4 << (n * 2));
                s32 dd = l->e[i].z0 + l->e[i].z1;
                l->e[i].mask = v;
                if (dd >= ref - 7500 && dd <= ref + 2500) {
                    l->e[i].mask = v | (1 << (n * 2));
                }
            }
        }
    } else {
        base = 0;
        if (gNumScreens != 1) {
            base = l->count;
        }
        px = gCamPos[n].x;
        py = gCamPos[n].y;
        pz = gCamPos[n].z;
        cnt = 0;
        if (gRiders_22[n].f5AA == 9) {
            sCamSection = gRiderSection[k];
        } else {
            gGroundFound = 0;
            height_get_ground(px, py, pz);
            if (gGroundFound == 0) {
                sCamSection = gRiderSection[k];
                gGroundFound = 1;
            } else {
                sCamSection = gGroundArea;
            }
        }
        bit = 1 << (n * 2);
        for (i = 0; i < l->count; i++) {
            l->e[i].mask &= ~(3 << (n * 2));
        }
        e = sPvsIndex[sCamSection + base];
        n1 = e[0];
        tot = n1 + e[1];
        for (i = 0; i < tot; i++) {
            l->e[e[i + 2]].mask |= bit;
        }
        for (i = 0; i < n1; i++) {
            l->e[e[i + 2]].mask |= 2 << (n * 2);
        }
        px >>= 4;
        pz >>= 4;
        l->e[sCamSection].mask |= bit;
        lim = 0xC48A / (0x3EE - sTubeFogNear[n]);
        ang = gCamYaw[n] >> 4;
        sn = gCosTable[ang & 0xFFF];
        cs = gCosTable[(ang - 0x400) & 0xFFF];
        for (i = 0; i < l->count; i++) {
            TrackModel *r = &l->e[i];
            TrackBounds *q = &sSectionBounds[i];
            if (r->mask & bit) {
                cnt++;
                if (gGroundFound == 0 && cnt >= 16) {
                    r->mask &= ~bit;
                    cnt--;
                } else {
                    s32 t, u, f, x, z;
                    x = r->x0;
                    z = r->z0;
                    t = (sn * (z - pz) - cs * (x - px)) >> 14;
                    x = r->x1 - x;
                    h = (-cs * x) >> 14;
                    f = (sn * (r->z1 - z)) >> 14;
                    u = t + h;
                    d[0] = t;
                    d[1] = u;
                    d[2] = t + f;
                    d[3] = u + f;
                    if (d[0] < 11 && d[1] < 11 && d[2] < 11 && d[3] < 11) {
                        cnt--;
                        r->mask &= ~(3 << (n * 2));
                    } else {
                        if (d[0] >= lim || d[1] >= lim || d[2] >= lim || d[3] >= lim) {
                            r->mask |= 0x100 << n;
                        } else {
                            r->mask &= ~(0x100 << n);
                        }
                        if (gCurTrack_2 - gTracks != 20 && d[0] > 300 && d[1] > 300 && d[2] > 300 && d[3] > 300) {
                            s32 y = q->x0;
                            s32 u2;
                            t = (sn * (y - px) + cs * (q->z0 - pz)) >> 14;
                            y = q->x1 - y;
                            h = (sn * y) >> 14;
                            u2 = t + h;
                            f = (cs * (q->z1 - q->z0)) >> 14;
                            w[0] = (t << 8) / d[0];
                            w[1] = (u2 << 8) / d[1];
                            w[2] = ((t + f) << 8) / d[2];
                            w[3] = ((u2 + f) << 8) / d[3];
                            if ((w[0] < -sCullSlope && w[1] < -sCullSlope && w[2] < -sCullSlope && w[3] < -sCullSlope) ||
                                (sCullSlope < w[0] && sCullSlope < w[1] && sCullSlope < w[2] && sCullSlope < w[3])) {
                                cnt--;
                                r->mask &= ~(3 << (n * 2));
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
void tube_assign_objects(TrackModelList *a, TrackModelList *b)
{
    u32 i, k;
    s32 x, z, fx, fz, d, dx, dz;
    s32 best, bd;

    for (i = 0; i < a->count; i++) {
        gGroundFound = 0;
        x = (a->e[i].x0 + a->e[i].x1) / 2;
        z = (a->e[i].z0 + a->e[i].z1) / 2;
        height_get_ground(x << 4, 999999, z << 4);
        if (gGroundFound != 0) {
            { s32 n = gGroundArea; if (n) dz = 1; best = n; }
        } else {
            x /= 4;
            z /= 4;
            for (k = 0; k < b->count; k++) {
                fx = (b->e[k].x0 + b->e[k].x1) / 2;
                fz = (b->e[k].z0 + b->e[k].z1) / 2;
                fx /= 4;
                fz /= 4;
                dx = x - fx;
                dz = z - fz;
                { s32 t; t = dx * dx; do { d = dz * dz; } while (0); dz = d; d = t + dz; }
                if (d < bd || k == 0) {
                    bd = d;
                    best = k;
                }
            }
        }
        do {
            a->e[i].mask = best << 16;
        } while (0);
        if (a->e[i].x0 < ((TrackBounds *)((best << 4) + (u32)sSectionBounds))->x0) {
            sSectionBounds[best].x0 = a->e[i].x0;
        }
        if (a->e[i].z0 < sSectionBounds[best].z0) {
            sSectionBounds[best].z0 = a->e[i].z0;
        }
        if (sSectionBounds[best].x1 < a->e[i].x1) {
            sSectionBounds[best].x1 = a->e[i].x1;
        }
        if (sSectionBounds[best].z1 < a->e[i].z1) {
            sSectionBounds[best].z1 = a->e[i].z1;
        }
    }
}

/* The height of the water under (x, z) in mm: the triangle of a water model containing the point. */
s32 tube_get_water_level(s32 x, s32 y, s32 *out)
{
    u32 i;
    s32 k;
    s32 la[4];
    s32 lb[4];
    s32 fx, fy;
    TrackModel *e;
    u8 *pb;
    u8 *base;

    if (gWaterModels == NULL) {
        return 0;
    }
    x >>= 4;
    y >>= 4;
    for (i = 0; i < gWaterModels->count; i++) {
        e = &gWaterModels->e[i];
        if (x < e->x0 || e->x1 < x || y < e->z0 || e->z1 < y) {
            continue;
        }
        if (gCurTrack_3[0xb] == 1) {
            fx = x - (e->x0 + e->x1) / 2;
            fy = y - (e->z0 + e->z1) / 2;
        } else {
            fx = x;
            fy = y;
        }
        /* walk the model's commands: vertex loads set the base, triangles are tested in x/z */
        for (pb = e->model->cmds; pb[0] != 0; pb += 4) {
            if ((pb[0] >> 4) == 3) {
                for (k = 0; k < 3; k++) {
                    la[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base);
                    lb[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base + 4);
                }
                if (point_in_tri_xz(la, lb, fx, fy)) {
                    *out = *(s16 *)((u32)(pb[1] << 4) + (u32)base + 2) << 4;
                    return 1;
                }
            } else if ((pb[0] >> 4) == 1) {
                base = (u8 *)e->model->vtx + (*(u16 *)(pb + 2) << 4);
            }
        }
    }
    return 0;
}

/* Builds the display lists of a model list. Track resources first move their command streams to the spare
   framebuffer so the resource can shrink to the vertices. With animTex, records the animated texture segments each
   model uses (tube_remap_tex_addr). */
Gfx **tube_build_dlists(TrackModelList *h, TexSet **sets, u32 flags, s32 animTex)
{
    u8 *buf;
    s32 off;
    s32 n;
    u32 i;
    Gfx **tbl;
    Gfx *t;
    s32 sz;
    TexSegRef *nb;
    s32 j;
    s32 d;

    buf = gFramebuffers[1 - gGfxFrameIndex];
    if (!(*(s32 *)(gCurTrack + 0x50) & 2)) {
        i = (s32)h->e[0].model->cmds - (s32)h;
        n = mem_get_size(h);
        memcpy(buf, (u8 *)h + i, n - i);
        h = mem_realloc_dbg(h, i, 0, D_800D6A50, 0x51B);
        do { } while (0);
        sz = 0;
        d = (s32)buf - ((s32)h + i);
        if (h->count != 0) {
            do {
                h->e[sz].model->cmds += d;
                sz++;
            } while (sz < h->count);
        }
    }
    tbl = mem_alloc_dbg(h->count * 4, 0, D_800D6A50, 0x524);
    prim_set_tex_mode_callback(animTex ? tube_tex_mode_callback : NULL);
    if (animTex && gAnimTexSet) {
        tube_init_tex_groups();
        sTexSegListIndex = mem_alloc_dbg((h->count + 1) * 4, 0, D_800D6A50, 0x52E);
        D_800E77C8 = sTexSegList = (TexSegRef *)(gZBuffer + 0x1E000);
    }
    for (i = 0; i < h->count; i++) {
        gPrimTexAlphaMode = -1;
        t = mem_alloc_dbg((s32)prim_build_dl(flags | 1, h->e[i].model, sets, (Gfx *)gZBuffer) - gZBuffer, 0,
                          D_800D6A50, 0x541);
        tbl[i] = t;
        if (t == NULL) {
            for (;;) {
            }
        }
        if (animTex && gAnimTexSet) {
            prim_set_tex_addr_callback(tube_remap_tex_addr);
            sTexSegListIndex[i] = D_800E77C8;
            D_800E7790 = 0;
        }
        gPrimTexAlphaMode = -1;
        prim_build_dl(flags | 1, h->e[i].model, sets, tbl[i]);
        if (animTex && gAnimTexSet) {
            D_800E77C8 += D_800E7790;
            prim_set_tex_addr_callback(NULL);
        }
    }
    if (animTex && gAnimTexSet) {
        sTexSegListIndex[i] = D_800E77C8;
        i++;
        sz = (s32)D_800E77C8 - (s32)sTexSegList;
        if (sz != 0) {
            sz *= 4;
            nb = mem_alloc_dbg(sz, 0, D_800D6A50, 0x565);
            memcpy(nb, sTexSegList, sz);
            for (j = 0; j < (s32)i; j++) {
                sTexSegListIndex[j] = nb + (((s32)sTexSegListIndex[j] - (s32)sTexSegList) >> 2);
            }
            sTexSegList = nb;
        } else {
            sTexSegList = NULL;
        }
    }
    prim_set_tex_mode_callback(NULL);
    return tbl;
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
    u32 a;

    a = *addr;
    last = gAnimTexSet->count - 1;
    if (((u32)gAnimTexSet->tex[0].data <= a) && (a <= (u32)gAnimTexSet->tex[last].data) && (-1 < last)) {
        k = 0;
        row = gAnimTexSet;
        while (k <= last) {
            if ((u32)row->tex[0].data == a) {
                break;
            }
            k = k + 1;
            row = (TexSet *)((TexEntry *)row + 1);
        }
        if ((k <= last) && (1 < sTexAnimGroupLen[k])) {
            last = 0;
            while (last < D_800E7790) {
                if (D_800E77C8[last].offset + D_800E77C8[last].groupStart == k) {
                    break;
                }
                last = last + 1;
            }
            if (last == D_800E7790) {
                if (last <= 7) goto store;
                do {
                } while (1);
store:
                /* the low bytes of the s32 entries */
                D_800E77C8[D_800E7790].groupStart = ((u8 *)&sTexAnimGroupStart[k])[3];
                D_800E77C8[D_800E7790].groupLen = ((u8 *)&sTexAnimGroupLen[k])[3];
                D_800E77C8[D_800E7790].offset = (u8)(k - ((u8 *)&sTexAnimGroupStart[k])[3]);
                D_800E7790 = D_800E7790 + 1;
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
        if (!(gAnimTexSet->tex[i].flags & 0x34)) {
            for (k = start; k <= i; k++) {
                sTexAnimGroupStart[k] = start;
                sTexAnimGroupLen[k] = i - start + 1;
            }
            start = i + 1;
        }
    }
}

void tube_load_pvs(void)
{
    s32 idx;
    s32 i;
    u32 j;
    s32 n;
    s32 a, b;
    u8 *dst;
    s32 *offs;
    s32 sz;
    s32 *q;
    u16 *p;
    s32 t1, t2;

    while (sched_get_current_task() != 0) {
    }
    sPvsData = (u16 *)track_res_get(D_800D1514);
    if (gTrackSections->count != *sPvsData) {
        fatal_halt(1);
    }
    idx = 1;
    dst = gZBuffer;
    offs = (s32 *)(gZBuffer + 0x1E000);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < gTrackSections->count; j++) {
            a = sPvsData[idx];
            b = sPvsData[idx + 1];
            if (i == 0 || i == gGame.numPlayers - 1) {
                n = a + b;
                *offs++ = dst - gZBuffer;
                memcpy(dst, (u16 *)((idx << 1) + (s32)sPvsData), (n + 2) * 2);
                dst += n * 2 + 4;
            }
            t1 = idx + 2;
            t2 = a + b;
            idx = t1 + t2;
        }
    }
    idx = 0;
    res_unload(&D_800E7728->handles[track_res_find(D_800D1514)]);
    i = dst - gZBuffer;
    sPvsData = mem_alloc_dbg(i, idx, D_800D6A50, 0x61B);
    memcpy(sPvsData, gZBuffer, i);
    i = (s32)offs + (-0x1E000 - (s32)gZBuffer);
    sPvsIndex = mem_alloc_dbg(i, idx, D_800D6A50, 0x620);
    memcpy(sPvsIndex, gZBuffer + 0x1E000, i);
    p = sPvsData;
    q = (s32 *)sPvsIndex;
    for (; idx < i / 4; idx++) {
        *q++ += (s32)p;
    }
}

typedef struct { u8 pad[0x628]; s16 f628; u8 rest[0x7E0 - 0x62A]; } Rec7E0;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x60 - 0x48]; u32 bits; u8 rest[0x6C - 0x64]; } Node_2;
typedef struct { u8 pad[0x58]; u16 a[4]; u16 b[4]; u8 rest[0xA0 - 0x68]; } Trk_7;

extern Rec7E0 gRiders_23[];
extern Trk_7 *gCurTrack_19;
extern Trk_7 gTracks_8[];

void tube_calc_fog_range(s32 a, s32 *b, s32 *c)
{
    f32 v[3];
    s32 n;
    f32 t, fa, fb, fx, fy;
    s32 i, sh, sh4;
    s32 a0, a1, b0, b1;
    u32 w, w2;
    Node_2 *nd, *base;
    s32 x, y, j, g;

    v[0] = (f32)gCamPos[a].x / 1000.0f;
    v[1] = (f32)gCamPos[a].y / 1000.0f;
    v[2] = (f32)gCamPos[a].z / 1000.0f;
    path_get_segment_and_t(v, &n, &t, gRiders_23[gCamTargets[a]].f628);
    base = gPathNodes;
    g = gNumScreens;
    i = g - 1;
    nd = (Node_2 *)(n * sizeof(Node_2) + (u32)base);
    j = nd->next;
    if (i > 2) {
        i = 2;
    }
    i *= 8;
    sh4 = i + 4;
    w = nd->bits;
    a0 = (w >> sh4) & 0xF;
    if (a0 >= 8) a0 -= 16;
    a1 = (w >> i) & 0xF;
    if (a1 >= 8) a1 -= 16;
    w2 = ((Node_2 *)(j * sizeof(Node_2) + (u32)base))->bits;
    b0 = (w2 >> sh4) & 0xF;
    if (b0 >= 8) b0 -= 16;
    b1 = (w2 >> i) & 0xF;
    if (b1 >= 8) b1 -= 16;
    b0 -= a0;
    b0 *= 16;
    b1 -= a1;
    b1 *= 2;
    fa = (f32)b0 * t;
    fb = (f32)b1 * t;
    a0 *= 16;
    a1 *= 2;
    fx = (f32)a0 + fa;
    fy = (f32)a1 + fb;
    x = (s32)fx;
    y = (s32)fy;
    x = gCurTrack_19->a[g - 1] - x;
    y = gCurTrack_19->b[g - 1] - y;
    if (y < x) {
        x = y;
    }
    if (gCurTrack_19 - gTracks_8 == 8 && g == 1) {
        x = 1000;
        y = 0x41A;
    }
    *b = x;
    *c = y;
}

/* G_MOVEWORD 0xDB, index 6 (segment table), offset seg*4 */
Gfx *tube_draw(Gfx *gdl, s32 a, s32 b, s32 c) {
    Gfx *q;

    gSPSegment(gdl++, 1, gObjectTexSet->pal);
    if (D_800E77C4 != NULL) {
        gSPSegment(gdl++, 2, D_800E77C4->pal);
    }
    if (gAnimTexSet != NULL) {
        gSPSegment(gdl++, 3, gAnimTexSet->pal);
    }
    q = tube_draw_layer(gdl, a, b, c, 0);
    return tube_draw_layer(q, a, b, c, 1);
}

short *tube_alloc_mtx(s32 p1, s32 p2, s32 p3, s32 p4)
{
    short *r;
    s32 n;
    s32 t;

    n = sTubeMtxCount;
    if (n >= 300) {
        return 0;
    }
    p2 = p2 * 0x10 - gCamPos[p1].x;
    p3 = p3 * 0x10 - gCamPos[p1].y;
    p4 = p4 * 0x10 - gCamPos[p1].z;
    t = n * 0x40;
    r = (short *)((u8 *)sTubeMtxPool[gGfxFrameIndex] + t);
    mtx_from_angles_pos_scale(r, 0, 0, 0, p2, p3, p4, 0x100);
    sTubeMtxCount = sTubeMtxCount + 1;
    return r;
}

Gfx *tube_set_anim_tex_segs(Gfx *gdl, s32 idx)
{
    if (gAnimTexSet != NULL && sTexSegListIndex != NULL) {
        TexSegRef *cur = sTexSegListIndex[idx];
        TexSegRef *end = sTexSegListIndex[idx + 1];
        s32 i;

        for (i = 0; cur != end; cur++, i++) {
            gSPSegment(gdl++, i + 8, gAnimTexSet->tex[cur->offset + cur->groupStart].data);
        }
    }
    return gdl;
}

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
    TrackModelList *p = gTrackObjects;
    TrackModelList *c;
    u32 i = 0;
    u32 *q;
    u32 m;

    if (p->count != 0) {
        m = 0xfffffff7;
        c = p;
        q = gObjectFlags;
        do {
            i++;
            *q = *q & m;
            q++;
        } while (i < c->count);
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

void tube_animate_textures(void)
{
    volatile s32 pad[10];
    s32 v = gRaceFrame / 2;
    if (v == sTexAnimTick) return;
    sTexAnimTick = v;
    if (gPodiumMode != 0 && gDesaturation == 0x100) return;
    if (gAnimTexSet != 0) {
        prim_animate_textures(gAnimTexSet, v);
    }
    if (gWaterTexSet != 0) {
        gPrimIATextures = 1;
        prim_animate_textures(gWaterTexSet, sTexAnimTick);
        gPrimIATextures = 0;
    }
}

typedef struct {
    u8 pad0[0x4F8];
    s32 f4F8;
    u8 rest[0x7E0 - 0x4F8 - 4];
} Rec37;

extern Rec37 gRiders_24[];

void tube_spawn_splash(s32 param_1, s32 param_2, s32 param_3, s32 param_4)
{
    if (sSplashTimer[param_1] == 0 && gRiders_24[param_1].f4F8 >= 0) {
        gGroundFound = 0;
        height_get_ground(param_2, param_3, param_4);
        if (gGroundFound != 0) sSplashSection[param_1] = gGroundArea;
        else sSplashSection[param_1] = gRiderSection[param_1];
        sSplashTimer[param_1] = 0x78;
        sSplashPos[param_1].x = param_2 >> 4;
        sSplashPos[param_1].y = param_3 >> 4;
        sSplashPos[param_1].z = param_4 >> 4;
        snd_play_rider_sfx(6, param_1, 0xff);
    }
}

void tube_update_splashes(void)
{
    s32 i;
    s32 *p;

    i = 0;
    p = sSplashTimer;
    do {
        i = i + 1;
        if (*p != 0) {
            *p = *p - 1;
        }
        p = p + 1;
    } while (i < 8);
}

Gfx *tube_tex_mode_callback(Gfx *gdl, s32 flag)
{
    if (flag) {
        gSPDisplayList(gdl++, 0x07000000);
    } else {
        gSPDisplayList(gdl++, 0x06000000);
    }
    return gdl;
}
