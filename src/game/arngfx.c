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

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 pad[6];
    s32 f28;
} Blk5CC0;

extern Blk5CC0 gUserTrackData;

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
        if (gUserTrackData.f28 != 0) {
            D_800C9574 = gUserTrackData.f28;
        }
        gWaterModels = 0;
        gWaterTexSet = 0;
        gTerrainTexSet = gUserTrackData.f4;
        gObjectTexSet = gUserTrackData.fC;
        sStandModels = res_load(D_800E7728, D_800D07C0);
        sStandLowModel = res_load(D_800E7728, D_800D06F4);
        sStandTextures = res_load(D_800E7728, D_800D07D4);
        sObjectSegs[1] = 0;
        sObjectSegs[2] = 0;
        gAnimTexSet = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections_2 = (s32 *)gUserTrackData.f0;
        gSectionDLs = tube_build_dlists(gUserTrackData.f0, (s32)&gTerrainTexSet, 0, 0);
        gTrackObjects = (s32 *)gUserTrackData.f8;
        gObjectDLs = tube_build_dlists(gUserTrackData.f8, (s32)sObjectSegs, 0xC, 1);
        arn_apply_noise_shading((s32)gTrackSections_2);
    } else {
        D_800C9574 = track_res_get(D_800D1518);
        gWaterModels = track_res_get(D_800D14E4);
        gWaterTexSet = track_res_get(D_800D1510);
        gTerrainTexSet = track_res_get(D_800D1504);
        gObjectTexSet = track_res_get(D_800D1508);
        sStandModels = track_res_get(D_800D1584);
        sStandLowModel = track_res_get(D_800D1588);
        sStandTextures = track_res_get(D_800D158C);
        gAnimTexSet = track_res_get(D_800D1500);
        sObjectSegs[1] = gAnimTexSet;
        sObjectSegs[2] = 0;
        sObjectSegs[0] = gObjectTexSet;
        gTrackSections_2 = (s32 *)track_res_get(D_800D14DC);
        gSectionDLs = tube_build_dlists((s32)gTrackSections_2, (s32)&gTerrainTexSet, 0, 0);
        gTrackObjects = (s32 *)track_res_get(D_800D14E0);
        gObjectDLs = tube_build_dlists((s32)gTrackObjects, (s32)sObjectSegs, 0xC, 1);
    }
    mtx_from_angles_pos_scale(D_800E7730, 0, 0, 0, 0, 0, 0, 300);
    height_init((s32)gTrackSections_2, (s32)gTrackObjects, D_800C9574, 0);
    iVar1 = track_res_find(D_800D1528);
    instances_load(D_800E7728, iVar1, 1, (s32)gTrackSections_2, gObjectTexSet);
    if ((gNumScreens == 1) && (D_800E7718 == 0)) {
        t = *gTrackSections_2;
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

extern SegRef *sStandTextures_2;
extern SegRef *sStandLowModel_2;
extern ItemList *sStandModels_2;

Gfx *arn_draw(Gfx *gdl, s32 a1, s32 a2, s32 view)
{
    u32 i, n;
    s32 j;
    s32 k;
    u32 m;
    FogTab *fog;
    Item *it;

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
        for (i = 0; i < gTrackObjects_2->n; i++) {
            m = gTrackObjects_2->it[i].mask;
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
    gDPLoadTLUT_pal256(gdl++, gTerrainTexSet_2->seg);
    if (sSectionDrawOrder_2 != NULL && view == 0) {
        for (j = 0; j < sSectionDrawOrderLen; j++) {
            k = sSectionDrawOrder_2[j];
            if (k < 0) continue;
            it = &gTrackSections->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->n; k++) {
            it = &gTrackSections->it[k];
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
    if (sSectionDrawOrder_2 != NULL && view == 0) {
        for (j = 0; j < sSectionDrawOrderLen; j++) {
            k = sSectionDrawOrder_2[j];
            if (k < 0) continue;
            it = &gTrackSections->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, gSectionDLs[k]);
            }
        }
    } else {
        for (k = 0; k < gTrackSections->n; k++) {
            it = &gTrackSections->it[k];
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
    gDPLoadTLUT_pal256(gdl++, sStandTextures_2->seg);
    if (gNumViews < 3) {
        for (i = 0; i < sStandModels_2->n; i++) {
            if (gDrawFlags & 0x10) {
                gdl = prim_build_dl(4, sStandModels_2->it[i].id, &sStandTextures_2, gdl);
            }
        }
    } else {
        gdl = prim_build_dl(4, (s32)sStandLowModel_2->seg, &sStandTextures_2, gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    Gfx *dl;
    u8 pad[0x18];
    u32 mask;
} Ent_14;

typedef struct {
    u32 n;
    Ent_14 e[1];
} Tbl;

typedef struct {
    u8 pad[0x58];
    u16 fogMax[4];
    u16 fogMin[4];
} Fog;

extern Tbl *gWaterModels_2;
extern Fog *gCurTrack_18;

Gfx *arn_draw_water(Gfx *gdl, s32 p2, s32 p3, u32 sh)
{
    u32 i;
    u32 off;
    u8 pad[0x48];
    Tbl *tbl = gWaterModels_2;


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
    for (i = 0; i < gWaterModels_2->n; i++) {
        off = i * 32 + 4;
        if ((*(u32 *)((u8 *)gWaterModels_2 + off + 0x1C) >> (sh << 1)) & 1) {
            gdl = prim_build_dl(0, gWaterModels_2->e[i].dl, gWaterTexSet_2, gdl);
        }
    }
    gDPPipeSync(gdl++);
    gPrimIATextures = 0;
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    return gdl;
}

typedef struct List {
    u32 n;
    u32 pad[7];
    struct { u32 flags; u32 pad[7]; } e[1];
} List;

extern List *gTrackObjects_4;

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

    for (i = 0; i < gTrackObjects_4->n; i++) {
        if ((gTrackObjects_4->e[i].flags & (1 << (mode * 2))) && (gTrackObjects_4->e[i].flags & (2 << (mode * 2)))) {
            gdl = tube_set_anim_tex_segs(gdl, i);
            gSPDisplayList(gdl++, gObjectDLs[i]);
        }
    }

    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

typedef struct {
    s32 pad0;
    s32 x0;
    s32 z0;
    s32 x1;
    s32 z1;
    s32 pad14;
    s32 flag;
    u32 bits;
} Rec_10;

void arn_cull_models(u32 *list, s32 p, s32 strict, s16 *slots, s32 nslots)
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
    Rec_10 *rec;

    far = gLoDetailThresh[gNumScreens - 1];
    near = gVisibleThresh[gNumScreens - 1];
    if (slots != NULL) {
        for (sn = 0; sn < nslots; sn++) {
            slots[sn] = -1;
        }
    }
    cam0 = gCamPos_3[p][0] >> 4;
    sn = gCosTable[(gCamYaw[p] >> 4) & 0xFFF];
    cs = gCosTable[((gCamYaw[p] >> 4) - 0x400) & 0xFFF];
    cam1 = gCamPos_3[p][1] >> 4;
    cam2 = gCamPos_3[p][2] >> 4;
    for (i = 0; i < list[0]; i++) {
        rec = (Rec_10 *)(list + 1) + i;
        ex = ((rec->x1 + rec->x0) >> 1) - cam0;
        ey = ((rec->z1 + rec->z0) >> 1) - cam2;
        rec->flag = 1;
        rec->bits &= ~(2 << (p * 2));
        d[0] = ex;
        d[1] = -cam1;
        d[2] = ey;
        r[0] = (sn * ex + cs * ey) >> 14;
        r[2] = (sn * ey - cs * ex) >> 14;
        if (far < r[2] && strict != 0) {
            rec->bits |= 2 << (p * 2);
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
        rec->flag = 1;
        rec->bits |= 1 << (p * 2);
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
        rec->flag = 0;
    hidden:
        rec->bits &= ~(1 << (p * 2));
    }
}

typedef struct {
    s16 x, y, z;
    u8 pad[6];
    u8 r, g, b;
    u8 pad2;
} V16;

typedef struct {
    u32 count;
    V16 *v;
} Mesh_2;

typedef struct {
    Mesh_2 *mesh;
    s32 minA, minB, maxA, maxB;
    u8 pad[0xC];
} Item_2;

struct Model_2 {
    u32 count;
    Item_2 items[1];
};

void arn_apply_noise_shading(Model_2 *m)
{
    s32 minB = 9999999;
    s32 minA = 9999999;
    s32 maxB = -9999999;
    s32 maxA = -9999999;
    s32 i, off, off2;
    u32 k;
    Item_2 *it;
    u8 *buf;
    u8 *out;
    s32 dx, dy, ix, iy;
    Mesh_2 *mesh;
    V16 *vt;
    u8 c;

    for (i = 0; i < m->count; i++) {
        it = (Item_2 *)((u8 *)m + (i * 0x20 + 4));
        if (it->minA < minA) minA = it->minA;
        if (it->minB < minB) minB = it->minB;
        if (maxA < it->maxA) maxA = it->maxA;
        if (maxB < it->maxB) maxB = it->maxB;
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
            it = (Item_2 *)((u8 *)m + off2);
            mesh = it->mesh;
            for (k = 0; k < mesh->count; k++) {
                vt = &mesh->v[k];
                ix = vt->x;
                ix = (ix - minA) * 64 / dx + 5;
                iy = vt->z;
                iy -= minB;
                iy = iy * 64 / dy + 5;
                c = out[(ix << 8) + iy];
                vt->r = c;
                vt->g = c;
                vt->b = c;
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
    if (*gTrackSections_2 != 0) {
        do {
            mem_free(gSectionDLs[i]);
            i = i + 1;
        } while (i < (u32)*gTrackSections_2);
    }
    i = 0;
    if (*gTrackObjects != 0) {
        do {
            mem_free(gObjectDLs[i]);
            i = i + 1;
        } while (i < (u32)*gTrackObjects);
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
    arn_cull_models(gTrackObjects, param_1, gGameMode != 2, NULL, 0);
    if (gWaterModels != NULL) {
        arn_cull_models(gWaterModels, param_1, 1, NULL, 0);
    }
}

typedef struct {
    u32 f0, f4, f8, fc, f10, f14, f18, f1c;
} Ent_16;

void arn_relocate(u32 *param)
{
    u32 *base = (u32 *)*param;
    Ent_16 *e;
    u32 i;
    u32 *q;
    u32 j;
    u32 n;
    if (*base != 0) {
        i = 0;
        do {
            e = (Ent_16 *)base + i;
            e->f4 = (u32)base + e->f4;
            e->f18 = (u32)base + e->f18;
            q = (u32 *)((volatile Ent_16 *)e)->f4;
            q[1] = (u32)base + q[1];
            q[3] = (u32)base + q[3];
            if (q[5] != 0) {
                q[5] = (u32)base + q[5];
            }

            j = 0;
            if (q[0] != 0) {
                do {
                    j++;
                } while (j < q[0]);
            }
            i++;
        } while (i < *base);
    }
}
