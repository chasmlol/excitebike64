#include "game.h"

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
    s32 pad0;
    void *seg;
} SegRef;

extern SegRef *gTerrainTexSet_2;
extern ItemList *gTrackSections;
extern ItemList *gTrackObjects_2;

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

typedef struct {
    s32 a, b, c, d;
    u8 pad10[8];
    s32 f18;
    s32 pad1C;
} MEnt;

typedef struct {
    u32 n;
    u32 pad4;
    MEnt e[1];
} Mdl;

typedef struct {
    s32 f0;
    u16 *pal;
} Pal_5;

extern Pal_5 *gObjectTexSet_2;
extern Mdl *gTrackObjects_5;
extern Mdl *gTrackSections_6;

void tube_init(void)
{
    s32 i, j;
    u32 *p;

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
    gObjectTexSet_2 = (Pal_5 *)track_res_get(D_800D1508);
    p = &gTerrainTexSet;
    *p = track_res_get(D_800D1504);
    gWaterTexSet = track_res_get(D_800D1510);
    gAnimTexSet = track_res_get(D_800D1500);
    D_800E77C4 = track_res_get(D_800D14FC);
    gTrackObjects_5 = (Mdl *)track_res_get(D_800D14E0);
    sObjectSegs[0] = (u32)gObjectTexSet_2;
    sObjectSegs[1] = D_800E77C4;
    sObjectSegs[2] = gAnimTexSet;
    gObjectDLs = tube_build_dlists(gTrackObjects_5, sObjectSegs, 0x4C, 1);
    gTrackSections_6 = (Mdl *)track_res_get(D_800D14DC);
    gSectionDLs = tube_build_dlists(gTrackSections_6, p, 0, 0);
    gWaterModels = (Mdl *)track_res_get(D_800D14E4);
    if (gCheatFlags & 0x80) {
        tube_rotate_vertex_colors(gTrackSections_6);
        tube_rotate_vertex_colors(gTrackObjects_5);
    }
    gObjectFlags = mem_alloc_dbg(gTrackObjects_5->n * 4, 0, D_800D6A50, 0xF9);
    for (i = 0; i < gTrackObjects_5->n; i++) {
        gObjectFlags[i] = gTrackObjects_5->e[i].f18;
    }
    shatter_init(gTrackObjects_5);
    gSectionMtxs = mem_alloc_dbg(gTrackSections_6->n * 16, 0, D_800D6A50, 0x103);
    tube_load_pvs();
    for (i = 0; i < 256; i++) {
        if (gObjectTexSet_2->pal[i] == 0xF83F) {
            gObjectTexSet_2->pal[i] = 0;
        }
    }
    p = &D_800C9574;
    *p = track_res_get(D_800D1518);
    remove_walls_init(gTrackObjects_5, *p);
    height_init(gTrackSections_6, gTrackObjects_5, *p, track_res_get(D_800D14F4));
    sSectionBounds = mem_alloc_dbg(gTrackSections_6->n * 16, 0, D_800D6A50, 0x11D);
    for (i = 0; i < gTrackSections_6->n; i++) {
        sSectionBounds[i].x = gTrackSections_6->e[i].a;
        sSectionBounds[i].z = gTrackSections_6->e[i].b;
        sSectionBounds[i].y = gTrackSections_6->e[i].c;
        sSectionBounds[i].w = gTrackSections_6->e[i].d;
    }
    tube_assign_objects(gTrackObjects_5, gTrackSections_6);
    if (gWaterModels != NULL) {
        tube_assign_objects(gWaterModels, gTrackSections_6);
    }
    instances_load(D_800E7728, track_res_find(D_800D1528), 0, gTrackSections_6, gObjectTexSet_2);
}

void tube_free(void)
{
    u32 i;

    shatter_free();
    i = 0;
    mem_free(sSectionBounds);
    mem_free(gSectionMtxs);
    mem_free(gObjectFlags);
    if (*gTrackSections_3 != 0) {
        do {
            mem_free(gSectionDLs[i]);
            i++;
        } while (i < *gTrackSections_3);
    }
    i = 0;
    if (*gTrackObjects != 0) {
        do {
            mem_free(gObjectDLs[i]);
            i++;
        } while (i < *gTrackObjects);
    }
    mem_free(gSectionDLs);
    mem_free(gObjectDLs);
    tube_free_tex_segs();
    mem_free(sTubeMtxPool_2.a);
    mem_free(sTubeMtxPool_2.b);
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
    gTrackSections_3 = (u32 *)0;
}

typedef struct {
    u8 pad[0x1C];
    u32 f1C;
} Item_3;

typedef struct {
    u32 count;
    Item_3 items[1];
} List_3;

extern List_3 *gTrackObjects_6;
extern List_3 *gTrackSections_7;
extern List_3 *gWaterModels_3;

void tube_update_view(s32 n)
{
    u32 i;
    Item_3 *it;

    tube_calc_fog_range(n, &sTubeFogNear[n], &sTubeFogFar[n]);
    tube_cull_sections(gTrackSections_7, n);
    for (i = 0; i < gTrackObjects_6->count; i++) {
        it = (Item_3 *)&((u32 *)gTrackObjects_6)[i * 8 + 1];
        it->f1C = (it->f1C & 0xFFFF0000) | gTrackSections_7->items[it->f1C >> 16].f1C;
    }
    if (gWaterModels_3 != NULL) {
        for (i = 0; i < gWaterModels_3->count; i++) {
            it = (Item_3 *)&((u32 *)gWaterModels_3)[i * 8 + 1];
            it->f1C = (it->f1C & 0xFFFF0000) | gTrackSections_7->items[it->f1C >> 16].f1C;
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
    Item *it;
    u32 sh;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet_2->seg);
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

    for (i = 0; i < gTrackSections->n; i++) {
        it = &gTrackSections->it[i];
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

    for (i = 0; i < gTrackObjects_2->n; i++) {
        if (gObjectFlags[i] & 8) continue;
        it = &gTrackObjects_2->it[i];
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

extern SegRef *gObjectTexSet_3;
extern SegRef *D_800E77C4_2;
extern SegRef *gAnimTexSet_2;

Gfx *tube_draw_far(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    u32 i;
    MtxS *m;
    Item *it;
    u32 sh;
    s32 tmp[18];

    gSPSegment(gdl++, 1, gObjectTexSet_3->seg);
    if (D_800E77C4_2 != NULL) {
        gSPSegment(gdl++, 2, D_800E77C4_2->seg);
    }
    if (gAnimTexSet_2 != NULL) {
        gSPSegment(gdl++, 3, gAnimTexSet_2->seg);
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
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet_2->seg);
    gSPViewport(gdl++, &gViewports[view + 4]);

    for (i = 0; i < gTrackSections->n; i++) {
        it = &gTrackSections->it[i];
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

    for (i = 0; i < gTrackObjects_2->n; i++) {
        if (gObjectFlags[i] & 8) continue;
        it = &gTrackObjects_2->it[i];
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

typedef struct {
    s32 a;
    u32 img;
} TexEnt;

typedef struct {
    s32 pad0, pad4;
    TexEnt e[1];
} TexTab;

extern ItemList *gWaterModels_4;
extern TexTab *D_800E92C8;

Gfx *tube_draw_water(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    s32 i;
    MtxS *m;
    Item *it;
    s32 tmp[18];

    if (gWaterModels_4 == NULL) {
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

    for (i = 0; i < gWaterModels_4->n; i++) {
        it = &gWaterModels_4->it[i];
        if ((it->mask >> (view * 2)) & 1) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gdl = prim_build_dl(0, gWaterModels_4->it[i].id, gWaterTexSet_2, gdl);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    for (i = 0; i < 8; i++) {
        if (sSplashTimer[i] == 0) continue;
        it = &gTrackSections->it[sSplashSection[i]];
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
            gDPLoadTextureBlock(gdl++, D_800E92C8->e[f].img, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
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

typedef struct { s32 f0, f4, f8, fC, f10, f14, f18, f1C; } Rec_11;
struct List_4 { u32 count; Rec_11 r[1]; };
typedef struct { u8 pad[0x5AA]; s16 f5AA; u8 pad2[0x7E0 - 0x5AC]; } Player_9;

extern Player_9 gRiders_22[];

void tube_cull_sections(List_4 *l, s32 n)
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
            s32 ref = l->r[n1].f8 + l->r[n1].f10;
            for (i = 0; i < l->count; i++) {
                s32 v = l->r[i].f1C & (-4 << (n * 2));
                s32 dd = l->r[i].f8 + l->r[i].f10;
                l->r[i].f1C = v;
                if (dd >= ref - 7500 && dd <= ref + 2500) {
                    l->r[i].f1C = v | (1 << (n * 2));
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
            l->r[i].f1C &= ~(3 << (n * 2));
        }
        e = sPvsIndex_2[sCamSection + base];
        n1 = e[0];
        tot = n1 + e[1];
        for (i = 0; i < tot; i++) {
            l->r[e[i + 2]].f1C |= bit;
        }
        for (i = 0; i < n1; i++) {
            l->r[e[i + 2]].f1C |= 2 << (n * 2);
        }
        px >>= 4;
        pz >>= 4;
        l->r[sCamSection].f1C |= bit;
        lim = 0xC48A / (0x3EE - sTubeFogNear[n]);
        ang = gCamYaw[n] >> 4;
        sn = gCosTable[ang & 0xFFF];
        cs = gCosTable[(ang - 0x400) & 0xFFF];
        for (i = 0; i < l->count; i++) {
            Rec_11 *r = &l->r[i];
            Quad *q = &sSectionBounds[i];
            if (r->f1C & bit) {
                cnt++;
                if (gGroundFound == 0 && cnt >= 16) {
                    r->f1C &= ~bit;
                    cnt--;
                } else {
                    s32 t, u, f, x, z;
                    x = r->f4;
                    z = r->f8;
                    t = (sn * (z - pz) - cs * (x - px)) >> 14;
                    x = r->fC - x;
                    h = (-cs * x) >> 14;
                    f = (sn * (r->f10 - z)) >> 14;
                    u = t + h;
                    d[0] = t;
                    d[1] = u;
                    d[2] = t + f;
                    d[3] = u + f;
                    if (d[0] < 11 && d[1] < 11 && d[2] < 11 && d[3] < 11) {
                        cnt--;
                        r->f1C &= ~(3 << (n * 2));
                    } else {
                        if (d[0] >= lim || d[1] >= lim || d[2] >= lim || d[3] >= lim) {
                            r->f1C |= 0x100 << n;
                        } else {
                            r->f1C &= ~(0x100 << n);
                        }
                        if (gCurTrack_2 - gTracks != 20 && d[0] > 300 && d[1] > 300 && d[2] > 300 && d[3] > 300) {
                            s32 y = q->a;
                            s32 u2;
                            t = (sn * (y - px) + cs * (q->c - pz)) >> 14;
                            y = q->b - y;
                            h = (sn * y) >> 14;
                            u2 = t + h;
                            f = (cs * (q->d - q->c)) >> 14;
                            w[0] = (t << 8) / d[0];
                            w[1] = (u2 << 8) / d[1];
                            w[2] = ((t + f) << 8) / d[2];
                            w[3] = ((u2 + f) << 8) / d[3];
                            if ((w[0] < -sCullSlope && w[1] < -sCullSlope && w[2] < -sCullSlope && w[3] < -sCullSlope) ||
                                (sCullSlope < w[0] && sCullSlope < w[1] && sCullSlope < w[2] && sCullSlope < w[3])) {
                                cnt--;
                                r->f1C &= ~(3 << (n * 2));
                            }
                        }
                    }
                }
            }
        }
    }
}

typedef struct {
    s32 x0, z0, x1, z1;
    s32 pad10, pad14;
    s32 idx;
    s32 pad1c;
} Ent_17;

struct Lst {
    u32 n;
    s32 pad;
    Ent_17 e[1];
};

typedef struct {
    s32 x0, x1, z0, z1;
} Box;

extern Box *sSectionBounds_2;

void tube_assign_objects(Lst *a, Lst *b)
{
    Ent_17 *e;
    Ent_17 *f;
    u32 i, k;
    s32 x, z, fx, fz, d, dx, dz;
    s32 best, bd;

    for (i = 0; i < a->n; i++) {
        gGroundFound = 0;
        x = (a->e[i].x0 + a->e[i].x1) / 2;
        z = (a->e[i].z0 + a->e[i].z1) / 2;
        height_get_ground(x << 4, 999999, z << 4);
        if (gGroundFound != 0) {
            { s32 n = gGroundArea; if (n) dz = 1; best = n; }
        } else {
            x /= 4;
            z /= 4;
            for (k = 0; k < b->n; k++) {
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
            a->e[i].idx = best << 16;
        } while (0);
        if (a->e[i].x0 < ((Box *)((best << 4) + (u32)sSectionBounds_2))->x0) {
            sSectionBounds_2[best].x0 = a->e[i].x0;
        }
        if (a->e[i].z0 < sSectionBounds_2[best].z0) {
            sSectionBounds_2[best].z0 = a->e[i].z0;
        }
        if (sSectionBounds_2[best].x1 < a->e[i].x1) {
            sSectionBounds_2[best].x1 = a->e[i].x1;
        }
        if (sSectionBounds_2[best].z1 < a->e[i].z1) {
            sSectionBounds_2[best].z1 = a->e[i].z1;
        }
    }
}

typedef struct Obj_12 {
    s32 f0;
    u8 *base;
    s32 f8;
    u8 *bytes;
} Obj_12;

typedef struct Ent_18 {
    Obj_12 *obj;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 pad[3];
} Ent_18;

typedef struct Tbl_2 {
    u32 n;
    Ent_18 e[1];
} Tbl_2;

extern Tbl_2 *gWaterModels_5;

s32 tube_get_water_level(s32 x, s32 y, s32 *out)
{
    u32 i;
    s32 k;
    s32 la[4];
    s32 lb[4];
    s32 fx, fy;
    Ent_18 *e;
    u8 *pb;
    u8 *base;

    if (gWaterModels_5 == NULL) {
        return 0;
    }
    x >>= 4;
    y >>= 4;
    for (i = 0; i < gWaterModels_5->n; i++) {
        e = &gWaterModels_5->e[i];
        if (x < e->x0 || e->x1 < x || y < e->y0 || e->y1 < y) {
            continue;
        }
        if (gCurTrack_3[0xb] == 1) {
            fx = x - (e->x0 + e->x1) / 2;
            fy = y - (e->y0 + e->y1) / 2;
        } else {
            fx = x;
            fy = y;
        }
        for (pb = e->obj->bytes; pb[0] != 0; pb += 4) {
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
                base = e->obj->base + (*(u16 *)(pb + 2) << 4);
            }
        }
    }
    return 0;
}

typedef struct {
    u8 pad0[0xC];
    s32 fC;
} Sub_5;

struct Ent_19 {
    u32 n;
    Sub_5 *sub;
    u8 pad8[0x18];
};

s32 prim_build_dl_3(u32 a0, Sub_5 *a1, s32 a2, s32 a3);

s32 **tube_build_dlists(Ent_19 *h, s32 p2, u32 p3, s32 p4)
{
    u8 *buf;
    s32 off;
    s32 n;
    u32 i;
    s32 **tbl;
    s32 *t;
    s32 sz;
    s32 *nb;
    s32 j;
    s32 d;

    buf = gFramebuffers[1 - gGfxFrameIndex];
    if (!(*(s32 *)(gCurTrack + 0x50) & 2)) {
        i = h->sub->fC - (s32)h;
        n = mem_get_size(h);
        memcpy(buf, (u8 *)h + i, n - i);
        h = mem_realloc_dbg(h, i, 0, D_800D6A50, 0x51B);
        do { } while (0);
        sz = 0;
        d = (s32)buf - ((s32)h + i);
        if (h->n != 0) {
            do {
                h[sz].sub->fC += d;
                sz++;
            } while (sz < h->n);
        }
    }
    tbl = mem_alloc_dbg(h->n * 4, 0, D_800D6A50, 0x524);
    prim_set_tex_mode_callback(p4 ? tube_tex_mode_callback : NULL);
    if (p4 && gAnimTexSet) {
        tube_init_tex_groups();
        sTexSegListIndex = mem_alloc_dbg((h->n + 1) * 4, 0, D_800D6A50, 0x52E);
        D_800E77C8 = sTexSegList = (s32 *)(gZBuffer + 0x1E000);
    }
    for (i = 0; i < h->n; i++) {
        gPrimTexAlphaMode = -1;
        t = mem_alloc_dbg(prim_build_dl_3(p3 | 1, h[i].sub, p2, gZBuffer) - gZBuffer, 0, D_800D6A50, 0x541);
        tbl[i] = t;
        if (t == NULL) {
            for (;;) {
            }
        }
        if (p4 && gAnimTexSet) {
            prim_set_tex_addr_callback(tube_remap_tex_addr);
            sTexSegListIndex[i] = (s32)D_800E77C8;
            D_800E7790 = 0;
        }
        gPrimTexAlphaMode = -1;
        prim_build_dl_3(p3 | 1, h[i].sub, p2, (s32)tbl[i]);
        if (p4 && gAnimTexSet) {
            D_800E77C8 += D_800E7790;
            prim_set_tex_addr_callback(NULL);
        }
    }
    if (p4 && gAnimTexSet) {
        sTexSegListIndex[i] = (s32)D_800E77C8;
        i++;
        sz = (s32)D_800E77C8 - (s32)sTexSegList;
        if (sz != 0) {
            sz *= 4;
            nb = mem_alloc_dbg(sz, 0, D_800D6A50, 0x565);
            memcpy(nb, sTexSegList, sz);
            for (j = 0; j < (s32)i; j++) {
                sTexSegListIndex[j] = (s32)(nb + ((sTexSegListIndex[j] - (s32)sTexSegList) >> 2));
            }
            sTexSegList = nb;
        } else {
            sTexSegList = NULL;
        }
    }
    prim_set_tex_mode_callback(NULL);
    return tbl;
}

void tube_remap_tex_addr(u32 *param_1)
{
    s32 *piVar1;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    u8 *pb;
    u32 uVar3;

    uVar3 = *param_1;
    iVar5 = gAnimTexSet_3[0] - 1;
    if (((u32)gAnimTexSet_3[3] <= uVar3) && (uVar3 <= *(u32 *)((u8 *)gAnimTexSet_3 + (iVar5 << 3) + 0xC)) &&
        (-1 < iVar5)) {
        iVar6 = 0;
        piVar1 = gAnimTexSet_3;
        while (iVar6 <= iVar5) {
            if ((u32)piVar1[3] == uVar3) {
                break;
            }
            iVar6 = iVar6 + 1;
            piVar1 = piVar1 + 2;
        }
        if ((iVar6 <= iVar5) && (1 < *(s32 *)(sTexAnimGroupLen + iVar6 * 4))) {
            iVar5 = 0;
            while (iVar5 < D_800E7790) {
                if (D_800E77C8_2[iVar5 * 4 + 2] + D_800E77C8_2[iVar5 * 4] == iVar6) {
                    break;
                }
                iVar5 = iVar5 + 1;
            }
            if (iVar5 == D_800E7790) {
                if (iVar5 <= 7) goto store;
                do {
                } while (1);
store:
                D_800E77C8_2[D_800E7790 * 4] = sTexAnimGroupStart[iVar6 * 4 + 3];
                D_800E77C8_2[D_800E7790 * 4 + 1] = sTexAnimGroupLen[iVar6 * 4 + 3];
                D_800E77C8_2[D_800E7790 * 4 + 2] = (u8)(iVar6 - sTexAnimGroupStart[iVar6 * 4 + 3]);
                D_800E7790 = D_800E7790 + 1;
            }
            *param_1 = (u32)((iVar5 + 8) << 24);
        }
    }
}

typedef struct { u8 pad[2]; u8 flags; u8 pad2[5]; } Ent_20;
typedef struct { u32 count; u32 pad; Ent_20 e[1]; } Tbl_3;

extern Tbl_3 *gAnimTexSet_4;

void tube_init_tex_groups(void)
{
    s32 i, start, k;
    Tbl_3 *t;

    sTexAnimGroupStart_2 = (s32 *)(gZBuffer + 0x18000);
    sTexAnimGroupLen_2 = sTexAnimGroupStart_2 + gAnimTexSet_4->count;
    start = 0;
    for (i = 0; i < gAnimTexSet_4->count; i++) {
        if (!(gAnimTexSet_4->e[i].flags & 0x34)) {
            for (k = start; k <= i; k++) {
                sTexAnimGroupStart_2[k] = start;
                sTexAnimGroupLen_2[k] = i - start + 1;
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
    sPvsData_2 = (u16 *)track_res_get(D_800D1514);
    if (*gTrackSections_3 != *sPvsData_2) {
        fatal_halt(1);
    }
    idx = 1;
    dst = gZBuffer;
    offs = (s32 *)(gZBuffer + 0x1E000);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < *gTrackSections_3; j++) {
            a = sPvsData_2[idx];
            b = sPvsData_2[idx + 1];
            if (i == 0 || i == gGame_4[15] - 1) {
                n = a + b;
                *offs++ = dst - gZBuffer;
                memcpy(dst, (u16 *)((idx << 1) + (s32)sPvsData_2), (n + 2) * 2);
                dst += n * 2 + 4;
            }
            t1 = idx + 2;
            t2 = a + b;
            idx = t1 + t2;
        }
    }
    idx = 0;
    res_unload((u32 *)(D_800E7728_2[1] + track_res_find(D_800D1514) * 4));
    i = dst - gZBuffer;
    sPvsData_2 = mem_alloc_dbg(i, idx, D_800D6A50, 0x61B);
    memcpy(sPvsData_2, gZBuffer, i);
    i = (s32)offs + (-0x1E000 - (s32)gZBuffer);
    sPvsIndex = mem_alloc_dbg(i, idx, D_800D6A50, 0x620);
    memcpy(sPvsIndex, gZBuffer + 0x1E000, i);
    p = sPvsData_2;
    q = sPvsIndex;
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
void tube_draw(Gfx *gdl, s32 a, s32 b, s32 c) {
    Gfx *q;

    gDma1p(gdl++, 0xDB, gObjectTexSet_4[1], 0x4, 0x6);
    if (D_800E77C4_3 != 0) {
        gDma1p(gdl++, 0xDB, D_800E77C4_3[1], 0x8, 0x6);
    }
    if (gAnimTexSet_5 != 0) {
        gDma1p(gdl++, 0xDB, gAnimTexSet_5[1], 0xC, 0x6);
    }
    q = tube_draw_layer(gdl, a, b, c, 0);
    tube_draw_layer(q, a, b, c, 1);
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
    p2 = p2 * 0x10 - gCamPos_4[p1].a;
    p3 = p3 * 0x10 - gCamPos_4[p1].b;
    p4 = p4 * 0x10 - gCamPos_4[p1].c;
    t = n * 0x40;
    r = (short *)(sTubeMtxPool_3[gGfxFrameIndex] + t);
    mtx_from_angles_pos_scale(r, 0, 0, 0, p2, p3, p4, 0x100);
    sTubeMtxCount = sTubeMtxCount + 1;
    return r;
}

typedef struct {
    u32 size;
    void *addr;
} Ent_21;

typedef struct {
    u32 h0;
    u32 h1;
    Ent_21 e[1];
} Bank;

extern Bank *gAnimTexSet_6;

Gfx *tube_set_anim_tex_segs(Gfx *gdl, s32 idx)
{
    if (gAnimTexSet_6 != NULL && sTexSegListIndex != NULL) {
        u8 *cur = sTexSegListIndex[idx];
        u8 *end = sTexSegListIndex[idx + 1];
        s32 i;

        for (i = 0; cur != end; cur += 4, i++) {
            gSPSegment(gdl++, i + 8, gAnimTexSet_6->e[cur[2] + cur[0]].addr);
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

void tube_reset_objects(void)
{
    u32 *p = gTrackObjects;
    u32 *c;
    u32 i = 0;
    u32 *q;
    u32 m;

    if (*p != 0) {
        m = 0xfffffff7;
        c = p;
        q = gObjectFlags;
        do {
            i++;
            *q = *q & m;
            q++;
        } while (i < *c);
    }
    shatter_reset();
    remove_walls_reset();
}

typedef struct { u32 a, b, c; u32 val; } Rec_12;
typedef struct { u32 count; Rec_12 *recs; } Sub_6;
struct Ent_22 { u32 count; Sub_6 *sub; u32 pad[6]; };

void tube_rotate_vertex_colors(Ent_22 *p)
{
    u32 i, j;
    for (i = 0; i < p->count; i++) {
        for (j = 0; j < p[i].sub->count; j++) {
            u32 v = p[i].sub->recs[j].val;
            v = (v & 0xFF) | ((v >> 16) & 0xFF00) | ((v << 8) & 0xFFFF0000);
            p[i].sub->recs[j].val = v;
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
        sSplashPos_2[param_1].a = param_2 >> 4;
        sSplashPos_2[param_1].b = param_3 >> 4;
        sSplashPos_2[param_1].c = param_4 >> 4;
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
