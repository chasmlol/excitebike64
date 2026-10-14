#include "game.h"

#define G(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u16 c[256]; } Pal_3;
/* A texture set, as far as its palette */
typedef struct { s32 pad0; Pal_3 *pal; } Img_3;
/* sRiderModelCache: per rider type, the body's display lists and vertices, low and high detail */
typedef struct { void *dl[2]; void *vtx[2]; } Rec16;

/* The sky model (a PrimModel) as far as its vertex colours: the fog colour is their average */
typedef struct {
    u8 pad0[0xC];
    u8 r, g, b, a;
} Col16;

typedef struct {
    u32 numVtx;
    Col16 *vtx;
} ColList;

struct Dyn {
    Mtx m0[4];  /* 0x000 */
    Mtx m1[4];  /* 0x100 */
    Mtx m2[4];  /* 0x200 */
    Mtx m3[4];  /* 0x300 */
    Mtx m4;     /* 0x400 */
    Gfx *glist; /* 0x440 */
};

extern Rec16 sRiderModelCache[];
extern Img_3 *sRiderSkinTextures[][6];
extern Pal_3 *sBikeAltPalettes[];
extern ColList *sSkyModel;

/* A rider's resource ids: sRiderModelTable, per rider in sRiderBios order. */
typedef struct {
    /* 0x00 */ s32 *podiumModel;    /* the podium ceremony's model */
    /* 0x04 */ s32 *podiumTex;      /* its texture set */
    /* 0x08 */ s32 *modelHi;        /* race model, near LOD */
    /* 0x0C */ s32 *modelLo;        /* race model, far LOD */
    /* 0x10 */ s32 *skins[6];       /* the texture sets of the six skins */
} RiderModelIds; /* size 0x28 */

/* gRiderGfx, the render side of a rider (0xA80 bytes), as the functions of this file see it */
typedef struct {
    /* 0x000 */ void *riderTex;      /* the skin's texture set */
    /* 0x004 */ void *podiumTex;     /* the podium model's texture set */
    /* 0x008 */ Img_3 *bikeTex;      /* the bike's texture set */
    /* 0x00C */ Pal_3 *riderPalSrc;  /* the skin's palette */
    /* 0x010 */ Pal_3 *bikePalSrc;   /* the bike's palette (bikeTex's, or one of sBikeAltPalettes) */
    /* 0x014 */ Pal_3 *riderPal;     /* the rider palette drawn: riderPalSrc tinted (render_update_rider_palette) */
    /* 0x018 */ Pal_3 *selectPals;   /* rider select: a palette per skin */
    /* 0x01C */ u16 bikePal[256];    /* the bike palette drawn */
    /* 0x21C */ Rgb color;           /* the rider's colour (dots, place markers) */
    /* 0x21F */ u8 pad21F;
    /* 0x220 */ void *shadowDL;      /* rider 0's untextured model (shadows) */
    /* 0x224 */ void *riderDL[2];    /* the body's display lists: low and high detail */
    /* 0x22C */ void *riderVtx[2];   /* their vertices (segment 1) */
    /* 0x234 */ void *boneMtx[2];    /* per frame: the 15 bone matrices (segment 6) */
    /* 0x23C */ void *partMtx[2];    /* per frame: the matrices of the six bike parts */
    /* 0x244 */ u8 rest[0xA80 - 0x244];
} Plr;

extern RiderModelIds sRiderModelTable[];
extern Plr gRiderGfx_2[];

void render_load_rider(ResFile *resSet, s32 index, s32 **skins, s32 bikeModel, s32 bikePalette, s32 riderType)
{
    s32 texSets[2];
    void *tmp;
    RiderModelIds *modelIds;
    s32 skin;
    s32 i;
    s32 k;
    s32 id;
    u32 *model;
    Rgb *color;
    void *copy;
    s32 len;

    modelIds = &sRiderModelTable[riderType];
    if (gPodiumMode == 0) {
        if (riderType == 0) {
            ponytail_init(index);
        }
        gRiderGfx_2[index].bikeTex = res_load(resSet, *sBikeTexResIds[bikeModel]);
    }
    if (gGame.flags & 0x10) {
        gRiderGfx_2[index].selectPals = mem_alloc_dbg(0xC00, 0, sStrRenderC, 0x1FF);
    } else {
        gRiderGfx_2[index].riderPal = mem_alloc_dbg(0x200, 0, sStrRenderC, 0x203);
    }
    if (!(gGame.flags & 0x10)) {
        skin = gGame.riderSkins[index];
    } else {
        skin = gGame.riders[index & 3] & 3;
    }
    color = &gRiderGfx_2[index].color;
    mem_fill((u8 *)color, 0xFF, 3);
    k = res_get_size(resSet, *skins[skin]);
    if (gGame.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            sRiderSkinTextures[index][i] = mem_alloc_dbg(k, 0, sStrRenderC, 0x220);
            tmp = res_load(resSet, *skins[i]);
            prim_pkg_unrelocate((u32 *)&tmp);
            memcpy(sRiderSkinTextures[index][i], tmp, k);
            res_unload(&resSet->tbl[*skins[i]]);
            prim_pkg_relocate((u32 *)&sRiderSkinTextures[index][i]);
        }
        gRiderGfx_2[index].riderTex = sRiderSkinTextures[index][0];
        gRiderGfx_2[index].riderPalSrc = 0;
    } else if (index >= 6) {
        gRiderGfx_2[index].riderTex = res_load(resSet, sExtraRiderTexResId);
        sExtraRiderPalettes[0] = ((Img_3 *)res_load(resSet, sExtraRiderTexResId))->pal;
        sExtraRiderPalettes[1] = ((Img_3 *)res_load(resSet, sExtraRiderPal1ResId))->pal;
        sExtraRiderPalettes[2] = ((Img_3 *)res_load(resSet, sExtraRiderPal2ResId))->pal;
        gRiderGfx_2[index].riderPalSrc = sExtraRiderPalettes[index % 3];
    } else {
        tmp = res_load(resSet, *skins[skin]);
        gRiderGfx_2[index].riderTex = tmp;
        gRiderGfx_2[index].riderPalSrc = ((Img_3 *)tmp)->pal;
        *color = sRiderColors[skin];
    }
    if (gPodiumMode != 0) {
        if (index < 6) {
            gRiderGfx_2[index].podiumTex = res_load(resSet, *modelIds->podiumTex);
        } else {
            gRiderGfx_2[index].podiumTex = res_load(resSet, *sRiderModelTable[0].podiumTex);
        }
    }
    if (gGame.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            gRiderGfx_2[index].selectPals[i] = *sRiderSkinTextures[index][i]->pal;
        }
    } else {
        *gRiderGfx_2[index].riderPal = *gRiderGfx_2[index].riderPalSrc;
    }
    if (gPodiumMode == 0) {
        if (bikePalette == 0) {
            gRiderGfx_2[index].bikePalSrc = gRiderGfx_2[index].bikeTex->pal;
        } else {
            gRiderGfx_2[index].bikePalSrc = sBikeAltPalettes[bikeModel * 3 + (bikePalette - 1)];
        }
        __builtin_memcpy(gRiderGfx_2[index].bikePal, gRiderGfx_2[index].bikePalSrc, sizeof(Pal_3));
        gRiderGfx_2[index].bikePal[0] &= 0xFFFE;
        gRiderGfx_2[index].bikePalSrc->c[0] &= 0xFFFE;
        gRiderGfx_2[index].bikeTex->pal->c[0] &= 0xFFFE;
    }
    if (sRiderModelCache[riderType].dl[0] == 0) {
        for (k = 0; k < 2; k++) {
            if (gPodiumMode != 0) {
                if (k == 0) {
                    continue;
                }
                id = *modelIds->podiumModel;
            } else if (k != 0) {
                id = *modelIds->modelHi;
            } else {
                id = *modelIds->modelLo;
            }
            res_set_load_buffer((s32)(gZBuffer + 0x1E000));
            model = (u32 *)((u32)res_load(resSet, id) & ~3);
            skel_make_verts_bone_local(gRiderSkelTemplate->bones, model);
            texSets[0] = (s32)gRiderGfx_2[index].riderTex;
            gPrimBoneMtx = (Mtx *)0x06000000;
            texSets[1] = (s32)gRiderGfx_2[index].podiumTex;
            if (gPodiumMode != 0) {
                prim_alloc_seg_dl(&sRiderModelCache[riderType].dl[k], model, (s32)texSets, 0xC);
                palmod_register((s32)gRiderGfx_2[index].podiumTex, 0);
                palmod_register((s32)gRiderGfx_2[index].riderTex, 0);
            } else {
                prim_alloc_seg_dl(&sRiderModelCache[riderType].dl[k], model, (s32)texSets, 0);
            }
            copy = mem_alloc_dbg(model[0] << 4, 0, sStrRenderC, 0x294);
            sRiderModelCache[riderType].vtx[k] = copy;
            memcpy(copy, (void *)model[1], model[0] << 4);
            if (k == 0 && index == 0) {
                model[1] = 0x01000000;
                gPrimBoneMtx = (Mtx *)0x06000000;
                len = (u32)prim_build_dl(0x23, model, 0, gZBuffer) - (u32)gZBuffer;
                copy = mem_alloc_dbg(len, 0, sStrRenderC, 0x2A0);
                gRiderGfx_2[0].shadowDL = copy;
                memcpy(copy, gZBuffer, len);
            }
            res_unload(&resSet->tbl[id]);
            res_set_load_buffer(0);
        }
    }
    gRiderGfx_2[index].riderVtx[0] = sRiderModelCache[riderType].vtx[0];
    gRiderGfx_2[index].riderVtx[1] = sRiderModelCache[riderType].vtx[1];
    gRiderGfx_2[index].riderDL[0] = sRiderModelCache[riderType].dl[0];
    gRiderGfx_2[index].riderDL[1] = sRiderModelCache[riderType].dl[1];
    for (k = 0; k < 2; k++) {
        gRiderGfx_2[index].boneMtx[k] = mem_alloc_dbg(0x3C0, 0, sStrRenderC, 0x2B3);
        if (gPodiumMode == 0) {
            gRiderGfx_2[index].partMtx[k] = mem_alloc_dbg(0x280, 0, sStrRenderC, 0x2B6);
        }
    }
}

void render_update_screen_layout(void)
{
    s32 limit;

    if (gViMode == 2) {
        gViewLayouts = sLayout1PHiRes;
        sGaugeY = 300;
    } else {
        gViewLayouts = sLayout1P;
        sGaugeY = 0xAA;
    }
    gScreenViewport_2[0] = gScreenWidth * 2;
    gScreenViewport_2[1] = gScreenHeight * 2;
    gScreenViewport_2[4] = gScreenWidth * 2;
    gScreenViewport_2[5] = gScreenHeight * 2;
    sGaugeX = gScreenWidth * 0x186 / 512;
    sGaugeNeedleX = gScreenWidth * 0x28 / 512;
    sGaugeNeedleY = gScreenHeight * 0x19 / 240;
    render_reset_viewports();
    if ((gCurTrack_2 - gTracks) == 8 && gNumScreens >= 2) {
        mem_fill(gFogColor, 0, 3);
    }
    limit = gCurTrack_2 - gTracks;
    if (limit == 8 || limit == 0xB) {
        limit = 4;
    } else {
        limit = 3;
    }
    if (gNumScreens >= limit) {
        gSkyMode = 0;
        gSkyImage = 0;
    }
}

/* gTracks as render_init sees it */
typedef struct {
    u8 pad0[0x2C];
    s32 envType;      /* TRACK_ENV_* */
    u8 pad30[0x20];
    s32 flags;        /* TRACK_FLAG_* */
    u8 fogR, fogG, fogB; /* outdoor tracks without a sky: the fog colour */
    u8 pad57[0xA0 - 0x57];
} Track_3;

/* gRiderGfx: the rider's colour and the place marker's palette (shades of it) */
typedef struct {
    u8 pad0[0x21C];
    u8 color[3];
    u8 pad21F[0xA60 - 0x21F];
    u16 markerPal[16];
} PalEnt;

/* sBikePartModels: the six bike parts' models, low and high detail */
typedef struct {
    void *model[2];
} Pair_5;

/* A texture set's header */
typedef struct {
    s32 count;
    u16 *pal;
} Res;

extern Track_3 *gCurTrack_9;
extern Track_3 gTracks_4[];
extern Res *sMarkerImage;
extern PalEnt gRiderGfx_3[8];
extern Pair_5 sBikePartModels[6];

void render_init(void) {
    u8 textBuf[0x60];
    s32 red, green, blue;
    s32 i, j, k, count, size;
    void *skel;
    u32 sumR, sumG, sumB, m;
    s32 trackIdx;
    s32 cr, cg, cb;

    if (gGame.flags & 0x40000) {
        gPodiumMode = 1;
    } else {
        gPodiumMode = 0;
    }
    if (gCheatFlags & 1) {
        sBigHeadCheat = 1;
    } else {
        sBigHeadCheat = 0;
    }
    if (gCheatFlags & 0x2000) {
        sPinHeadCheat = 1;
    } else {
        sPinHeadCheat = 0;
    }
    mem_fill(sRiderPalDirty, 1, 8);
    if (gCreditsMode != 0) {
        i = 0;
        sCreditsText = res_load(gResourceSet, sCreditsTextResId);
        sCreditsScrollY = 0xF0;
        sCreditsLineCount = 0;
        while (sCreditsText[i] != '@' || sCreditsText[i + 1] != 'E') {
            if (sCreditsText[i++] == '\r') {
                sCreditsLineCount++;
            }
        }
        sCreditsLineCount += 3;
        sCreditsFont = font_text_create(NULL, 0x40, 1, 4, 0);
        sCreditsHeaderFont = font_text_create(NULL, 0x40, 1, 2, 0);
        sTeamPhotoImage = res_load(gResourceSet, sTeamPhotoResId);
        prim_flip_images(sTeamPhotoImage);
    }
    render_update_screen_layout();
    sMarkerImage = res_load(gResourceSet, sMarkerImageResId);
    prim_flip_images(sMarkerImage);
    *sMarkerImage->pal = 0;
    gFontDirectDrawFunc = render_blit_place_glyph;
    sPlaceTextures = mem_alloc_dbg(0xC00, 0, sStrRenderC, 0x349);
    mem_fill(sPlaceTextures, 0, 0xC00);
    for (i = 0; i < 6; i++) {
        sPlaceTexDst = sPlaceTextures + i * 0x200;
        font_text_create((TextObj *)textBuf, 3, 1, 1, 0x08000000);
        font_text_add((TextObj *)textBuf, sPlaceStrings[i]);
        font_draw_text((TextObj *)textBuf, 0, 0);
    }
    gFontDirectDrawFunc = NULL;
    gLightningFlash = 0;
    if (gSplitVertical != 0) {
        sViewLayoutsMulti = sLayout2PVertical;
    } else {
        sViewLayoutsMulti = sLayout2PHorizontal;
    }
    render_reset_viewports();
    sGaugeNeedleVtx = mem_alloc_dbg(0x100, 0, sStrRenderC, 0x359);
    sPonytailMesh = 0;
    if (gMirrorMode != 0) {
        gCullMode = 0x200;
    } else {
        gCullMode = 0x400;
    }
    gDebugRiderCount = 8;
    sRiderColors = res_load(gResourceSet, sRiderColorsResId);
    if (gCurTrack_9->envType != 2) {
        if (gReplayPlayback != 0 || (gGame.flags & 0x80) || gGhostRace != 0) {
            gNumViews = 1;
        } else {
            gNumViews = gGame.numPlayers;
        }
    }
    if (gCurTrack_9->envType == 2) {
        gNumViews = 1;
    }
    sClearFrames = 5;
    gNumScreens = gNumViews;
    mem_fill(gRiderSection, 0, 0x20);
    prim_init();
    if (gCurTrack_9->envType == 0) {
        arn_init();
    } else if (gCurTrack_9->envType == 1) {
        tube_init();
    } else if (gCurTrack_9->envType == 2) {
        des_init();
    }
    sGaugeTextures = res_load(gResourceSet, sGaugeTexResId);
    prim_flip_images(sGaugeTextures);
    sGaugePalettes = mem_alloc_dbg(0x400, 0, sStrRenderC, 0x38B);
    mem_fill(sGaugePalettes, 0, 0x400);
    switch (gGame.mode) {
    case 2:
        soccer_ball_load_model();
        break;
    case 0:
    case 13:
    case 14:
        if (!(gCurTrack_9->flags & 2)) {
            break;
        }
    case 9:
        finishline_load_models();
        break;
    }
    skel = skel_load(gResourceSet, sRiderSkelResId);
    gRiderSkelTemplate = skel;
    for (i = 0; i < 15; i++) {
        mtx3i_from_angles(gRiderSkelTemplate->bones[i].rot.m, 0, 0, 0);
        mtx3i_from_angles(gRiderSkelTemplate->bones[i].worldRot.m, 0, 0, 0);
    }
    skel_update_world_transforms(gRiderSkelTemplate->bones, 15);
    if (gCurTrack_9->envType == 0) {
        j = i;
    } else if (gCurTrack_9->envType == 1) {
        j = count;
    }
    mem_fill(sRiderModelCache, 0, 0x70);
    mem_fill((u8 *)gRiderGfx_3, 0, 0x5400);
    count = 8;
    if (gGame.numPlayers >= 2 && !(gGame.flags & 0x10)) {
        count = 4;
    }
    if ((gGame.flags & 1) && gEvent.numRiders == 1) {
        count = 2;
    }
    if (gPodiumMode != 0) {
        count = gEvent.numRiders;
    }
    if (gCreditsMode != 0) {
        count = 1;
    }
    for (i = 0; i < 6; i++) {
        sBikeAltPalettes[i] = (s32)((Res *)res_load(gResourceSet, *sBikeAltPalResIds[i]))->pal;
    }
    for (j = 0; j < count; j++) {
        if (gGame.flags & 0x10) {
            if (j >= 6) {
                break;
            }
            i = j;
        } else {
            i = 4;
            if (j < 6) {
                i = gGame.riders[j];
            }
        }
        render_load_rider(gResourceSet, j, sRiderModelTable[i].skins, gGame.bikeSkins[j % 6] & 1, gGame.bikeSkins[j % 6] >> 1, i);
    }
    if (!(gGame.flags & 0x81)) {
        event_hud_init();
    }
    for (i = 0; i < 8; i++) {
        gRiderGfx_3[i].markerPal[0] = 0;
        for (j = 1; j < 8; j++) {
            cr = gRiderGfx_3[i].color[0] * j / 7 >> 3;
            cg = gRiderGfx_3[i].color[1] * j / 7 >> 3;
            cb = gRiderGfx_3[i].color[2] * j / 7 >> 3;
            gRiderGfx_3[i].markerPal[j] = (cr << 11) | (cg << 6) | (cb << 1) | 1;
        }
        for (j = 0; j < 8; j++) {
            cr = gRiderGfx_3[i].color[0];
            cg = gRiderGfx_3[i].color[1];
            cb = gRiderGfx_3[i].color[2];
            cr = (cr + (255 - cr) * j / 7) >> 3;
            cg = (cg + (255 - cg) * j / 7) >> 3;
            cb = (cb + (255 - cb) * j / 7) >> 3;
            gRiderGfx_3[i].markerPal[j + 8] = (cr << 11) | (cg << 6) | (cb << 1) | 1;
        }
    }
    mem_fill((u8 *)sBikePartModels, 0, 0x30);
    mem_fill((u8 *)sBikePartDLs, 0, 0x30);
    mem_fill((u8 *)gBikeShadowDLs, 0, 0x18);
    sBikeLowDL = 0;
    gBikeSkelTemplate = skel_load(gResourceSet, sBikeSkelResId);
    skel_reset_pose(gBikeSkelTemplate->bones, 10);
    if (gPodiumMode == 0) {
        for (i = 0; i < 6; i++) {
            sBikePartModels[i].model[1] = res_load(gResourceSet, *sBikePartResIds[i]);
            sBikePartModels[i].model[0] = res_load(gResourceSet, *sBikePartResIds[i + 6]);
        }
        sBikeLowModel = res_load(gResourceSet, sBikeLowModelResId);
        prim_alloc_dl(&sBikeLowDL, sBikeLowModel, &gRiderGfx_2[0].bikeTex, 0x20);
        for (i = 0; i < 6; i++) {
            skel_make_verts_bone_local(&gBikeSkelTemplate->bones[sBikePartBones[i]], sBikePartModels[i].model[1]);
            skel_make_verts_bone_local(&gBikeSkelTemplate->bones[sBikePartBones[i]], sBikePartModels[i].model[0]);
        }
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 2; j++) {
                prim_alloc_seg_dl(&sBikePartDLs[i][j], sBikePartModels[i].model[j], &gRiderGfx_2[0].bikeTex, 0);
            }
            size = (s32)prim_build_dl(3, sBikePartModels[i].model[1], NULL, (Gfx *)gZBuffer) - gZBuffer;
            gBikeShadowDLs[i] = mem_alloc_dbg(size, 0, sStrRenderC, 0x434);
            memcpy(gBikeShadowDLs[i], gZBuffer, size);
        }
    }
    sSkyModel = (ColList *)track_res_get(gGidArnSky);
    gSkyImage = (TexSet *)track_res_get(gGidPkgSky);
    if (sSkyModel != NULL) {
        if (gSkyImage != NULL) {
            gSkyMode = 3;
        } else {
            gSkyMode = 2;
        }
    } else if (gSkyImage != NULL) {
        gSkyMode = 1;
        prim_flip_image_rows(gSkyImage->tex[0].data, 0x100, 0x100);
    } else {
        gSkyMode = 0;
    }
    if (gSkyImage != NULL) {
        palmod_register(gSkyImage, 0);
    }
    sun_init();
    sky_init();
    if (gSkyImage != NULL) {
        gFogColor[0] = (*gSkyImage->pal >> 11) << 3;
        gFogColor[1] = ((*gSkyImage->pal >> 6) & 0x1F) << 3;
        gFogColor[2] = ((*gSkyImage->pal >> 1) & 0x1F) << 3;
        if (gCreditsMode != 0) {
            red = (gFogColor[0] + gFogColor[1] + gFogColor[2]) / 3;
            palmod_sepia(red, &red, &green, &blue);
            gFogColor[0] = red;
            gFogColor[1] = green;
            gFogColor[2] = blue;
        }
    } else if (sSkyModel != NULL) {
        sumR = 0;
        sumG = 0;
        sumB = 0;
        for (i = 0; i < sSkyModel->numVtx; i++) {
            sumR += sSkyModel->vtx[i].r;
            sumG += sSkyModel->vtx[i].g;
            sumB += sSkyModel->vtx[i].b;
        }
        sumR /= sSkyModel->numVtx;
        sumG /= sSkyModel->numVtx;
        sumB /= sSkyModel->numVtx;
        gFogColor[0] = sumR;
        gFogColor[1] = sumG;
        gFogColor[2] = sumB;
    } else if (gCurTrack_9->envType == 1) {
        gFogColor[0] = gCurTrack_9->fogR;
        gFogColor[1] = gCurTrack_9->fogG;
        gFogColor[2] = gCurTrack_9->fogB;
    } else {
        gFogColor[0] = 0;
        gFogColor[1] = 0;
        gFogColor[2] = 0;
    }
    trackIdx = gCurTrack_9 - gTracks_4;
    if (trackIdx == 3 || trackIdx == 20) {
        gSkyImage = NULL;
        gSkyMode = 0;
    }
    if (gCheatFlags & 0x40) {
        gSkyImage = NULL;
        gSkyMode = 0;
        mem_fill(gFogColor, 0, 3);
    }
    if (gEvent.noPlayers != 0) {
        gDrawFlags &= ~0x800;
    } else {
        gDrawFlags |= 0x800;
    }
    gInsetViewActive = -1;
    render_reset_viewports();
}

/* gRiderGfx: what render_free frees */
typedef struct { u8 pad0[0x14]; s32 riderPal; s32 selectPals; u8 pad1c[0x234 - 0x1C]; s32 boneMtx[2]; s32 partMtx[2]; u8 rest[0xA80 - 0x244]; } Plr_2;
typedef struct { u8 pad[0x2C]; s32 envType; u8 pad30[0x20]; u32 flags; } Cfg_4;

extern Cfg_4 *gCurTrack_10;
extern Plr_2 gRiderGfx_4[];

void render_free(void)
{
    s32 i, k, rider;

    if (gCreditsMode != 0) {
        font_text_destroy(sCreditsFont);
        font_text_destroy(sCreditsHeaderFont);
        mem_free(sCreditsFont);
        mem_free(sCreditsHeaderFont);
    }
    mem_free(sPlaceTextures);
    mem_free(sGaugeNeedleVtx);
    if (gCurTrack_10->envType == 0) {
        arn_free();
    } else if (gCurTrack_10->envType == 1) {
        tube_free();
    } else if (gCurTrack_10->envType == 2) {
        des_free();
    }
    mem_free(gBikeSkelTemplate);
    mem_free(gRiderSkelTemplate);
    mem_free(sGaugePalettes);
    if ((gGame.flags & 0x81) == 0) {
        event_hud_free();
    }
    switch (gGame.mode) {
    case 2:
        soccer_ball_free_model();
        break;
    case 0:
    case 13:
    case 14:
        if ((gCurTrack_10->flags & 2) == 0) {
            break;
        }
    case 9:
        finishline_free_models();
        break;
    }
    prim_exit();
    for (rider = 0; rider < 8; rider++) {
        if (gGame.flags & 0x10) {
            if (rider >= 6) {
                break;
            }
            mem_free(gRiderGfx_4[rider].selectPals);
            for (i = 0; i < 6; i++) {
                mem_free(sRiderSkinTextures[rider][i]);
            }
        } else if (gRiderGfx_4[rider].riderPal != 0) {
            mem_free(gRiderGfx_4[rider].riderPal);
        }
        ponytail_free(rider);
        for (i = 0; i < 2; i++) {
            mem_free(gRiderGfx_4[rider].boneMtx[i]);
            mem_free(gRiderGfx_4[rider].partMtx[i]);
        }
    }
    mem_free(gRiderGfx_2[0].shadowDL);
    for (i = 0; i < 7; i++) {
        for (k = 0; k < 2; k++) {
            if (sRiderModelCache[i].dl[k] != 0) {
                mem_free(sRiderModelCache[i].dl[k]);
            }
            if (sRiderModelCache[i].vtx[k] != 0) {
                mem_free(sRiderModelCache[i].vtx[k]);
            }
        }
    }
    for (i = 0; i < 6; i++) {
        mem_free(sBikePartDLs_2[i].y);
        mem_free(sBikePartDLs_2[i].x);
        mem_free(gBikeShadowDLs[i]);
    }
    mem_free(sBikeLowDL);
    cam_free();
    sun_free();
}

Gfx *render_set_viewport(Gfx *gdl, Mtx *projMtx, s32 view, s32 loadProj)
{
    Rect *rect;
    s32 i;
    u8 *rects;

    if (view == sCurViewport) {
        return gdl;
    }
    rects = (u8 *)gViewRects;
    rect = (Rect *)(rects + (view * 16 + (gNumViews - 1) * 64));
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
    if (loadProj) {
        gSPMatrix(gdl++, &projMtx[view], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    gSPViewport(gdl++, &gViewports[view]);
    sCurViewport = view;
    return gdl;
}

typedef struct { u8 pad0[0x2C]; s32 envType; u8 pad1[0xA0 - 0x30]; } Trk_4;

extern Trk_4 *gCurTrack_11;
extern Trk_4 gTracks_5[];

s32 render_get_rider_lod(s32 x, s32 y, s32 z, s32 view, s32 section, s32 dyn, s32 rider)
{
    s32 sx, sy, sz, depth;
    s32 halfW, halfH, lodDist;
    s32 x0, x1, y0, y1;
    Rect *rect;
    u8 *rects;

    if (gInsetViewActive != 0 && rider == 0) {
        return 3;
    }
    if (!(gRiders[rider].camMode == 9 && gCamTargets[view] == rider)) {
        if ((u32)gCurTrack_11->envType < 2 && section != -1) {
            if (!((gTrackSections->e[section].mask >> (view * 2)) & 1)) {
                return 0;
            }
        }
    }
    if (gCurTrack_11->envType == 0) {
        if (project_to_screen(x >> 4, y >> 4, z >> 4, &sx, &sy, &sz, &depth, dyn, view) != 0) {
            return 0;
        }
    } else {
        if (project_to_screen((x - gCamPos[view].x) >> 4, (y - gCamPos[view].y) >> 4,
                          (z - gCamPos[view].z) >> 4, &sx, &sy, &sz, &depth, dyn, view) != 0) {
            return 0;
        }
    }
    if (depth <= 0) {
        return 0;
    }
    if (gCurTrack_11->envType == 2 && depth > 8000) {
        return 0;
    }
    sRiderCamDist[rider] = depth;
    rects = (u8 *)gViewRects;
    rect = (Rect *)(rects + (view * 16 + (gNumViews - 1) * 64));
    x0 = rect->x;
    x1 = rect->w + x0;
    y0 = rect->y;
    y1 = rect->h + y0;
    halfW = (sProjScaleX[view] * 90.0f) / depth;
    halfH = (sProjScaleY[view] * 90.0f) / depth;
    if (sx + halfW < x0) {
        return 0;
    }
    if (x1 < sx - halfW) {
        return 0;
    }
    if (sy + halfH < y0) {
        return 0;
    }
    if (y1 < sy - halfH) {
        return 0;
    }
    if (gCurTrack_11 - gTracks_5 == 20 && gRiders[0].camMode == 4) {
        return 3;
    }
    lodDist = gLoDetailThresh[gNumScreens - 1];
    if (lodDist * 6 / 10 < depth) {
        return 1;
    }
    if (lodDist * 4 / 10 < depth) {
        return 2;
    }
    return 3;
}

Gfx *render_begin_menu_frame(Gfx *gdl, s32 fb, u8 *mtx) {
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, gRdpInitDL);
    gSPDisplayList(gdl++, gRspInitDL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gSPViewport(gdl++, &gScreenViewport);
    gSPPerspNormalize(gdl++, gPerspNorm);
    gDPPipeSync(gdl++);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0x10001);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects_4[0], gViewRects_4[1],
                  gViewRects_4[0] + gViewRects_4[2], gViewRects_4[1] + gViewRects_4[3]);
    gSPSetGeometryMode(gdl++, gCullMode);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    return gdl;
}

/* gSaveData as far as the options */
typedef struct {
    u8 pad0[0x2C4];
    u8 options;       /* bit 0 stereo */
} Opts;

extern Opts gSaveData_2;

Gfx *render_draw_sound_menu(Gfx *gdl, s32 fb, Dyn *dyn)
{
    s32 seg;
    s32 row;
    s32 x;
    s32 y;
    Img *img;
    s32 col;
    u8 *p;

    if (gPauseScreen.timer > 0) {
        gPauseScreen.timer--;
        gdl = render_begin_menu_frame_2(gdl, fb);
    } else if (gPauseScreen.timer == -1) {
        gdl = render_begin_menu_frame_2(gdl, fb);
        G(gdl++, 0xE200001C, 0x0C084000);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, gPauseScreen.background->addr);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = render_draw_image(0, 0, &gPauseScreen.background[1], gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xFA000000, 0xE6E6E6FF);
        G(gdl++, 0xE3000C00, 0);
        G(gdl++, 0xE200001C, 0x00404240);
        font_reset_queue();
        img = &gPauseScreen.title[1];
        G(gdl++, 0xFD100000, gPauseScreen.title->addr);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = render_draw_image(0x45, 0x3D, img, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xE3001001, 0);
        G(gdl++, 0xD7000002, 0x80008000);
        G(gdl++, 0xE200001C, 0x00404240);
        G(gdl++, 0xFC119623, 0xFF2FFFFF);
        for (row = 0; row < 3; row++) {
            for (seg = 0; seg < 29; seg++) {
                col = seg * 6 + 282;
                x = col - row * 7;
                y = sSoundMenuRowY[row] - 1;
                if (seg <= ((u8 *)gSaveData.volumes)[row] * 29 / 255) {
                    if (row == gOptionsCursor) {
                        G(gdl++, 0xFA000000, (((seg * 255 / 29) & 0xFF) << 16) | 0xFF0000FF);
                    } else {
                        G(gdl++, 0xFA000000, (((seg * 140 / 29) & 0xFF) << 16) | 0x8C0000FF);
                    }
                } else {
                    G(gdl++, 0xFA000000, 0x4B4B4BFF);
                }
                gdl = render_draw_image(x, y, &gPauseScreen.volumeBar[5], gdl, fb, 1, 1, 6, 0);
            }
        }
        G(gdl++, 0xFA000000, 0xD2D2D2FF);
        img = &gPauseScreen.volumeBar[6];
        gdl = render_draw_image(0x100 - ((u8 *)img)[0] * 4, 0x78 - (((u8 *)img)[1] >> 1), img, gdl, fb, 1, 1, 6, ((u32)img & 1) >> 1);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, gPauseScreen.cursorPal + 4);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        img = &gPauseScreen.cursorImg[(gGame.menuAnimTimer / 2) % 30 + 1];
        for (row = 0; row < 4; row++) {
            if (row < 3) {
                x = 0x48 - row * 7;
                y = sSoundMenuRowY[row];
            } else {
                col = ((row - 3) & 1) * 194 + 0x48;
                x = col - ((row - 3) / 2 + 3) * 7;
                y = sSoundMenuRowY[row];
            }
            font_text_create(gGame.texts[0], 0x40, 1, (row == gOptionsCursor) ? 4 : 2, 0x20);
            font_text_add(gGame.texts[0], gOptionsLabels[row]);
            font_draw_text(gGame.texts[0], x + 0x24, y + 8);
            if (row == gOptionsCursor) {
                gdl = render_draw_image(x - 2, y, img, gdl, fb, 1, 1, 3, 0);
            }
            font_text_create(gGame.texts[0], 0x40, 1, (row == gOptionsCursor) ? 6 : 2, 0x22);
            if (row == 3) {
                font_text_add(gGame.texts[0], gMonoStereoStrings[gSaveData_2.options & 1]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
            }
        }
        gdl = font_render(gdl);
    }
    return gdl;
}


Gfx *render_draw_controls_menu(Gfx *gdl, s32 fb, u8 *mtx)
{
    char buf[32];
    s32 i;
    s32 color;
    s32 textFlags;
    s32 odd;
    s32 left;

    if (gPauseScreen.timer > 0) {
        gPauseScreen.timer--;
        gdl = render_begin_menu_frame(gdl, fb, mtx);
    } else if (gPauseScreen.timer == -1) {
    gdl = render_begin_menu_frame(gdl, fb, mtx);
    G(gdl++, 0xE200001C, 0x00404240);
    G(gdl++, 0xE3001001, 0x00008000);
    G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    G(gdl++, 0xFD100000, gPauseScreen.background->addr);
    G(gdl++, 0xE8000000, 0);
    G(gdl++, 0xF5000100, 0x07000000);
    G(gdl++, 0xE6000000, 0);
    G(gdl++, 0xF0000000, 0x073FC000);
    G(gdl++, 0xE7000000, 0);
    G(gdl++, 0xE7000000, 0);
    gdl = render_draw_image(gViewRects_6[0], gViewRects_6[1], &gPauseScreen.background[1], gdl, fb, 1, 1, 3, 0);
    font_reset_queue();
    font_text_create(gGame.texts[0], 0x40, 4, 2, 0x21);
    strcpy(buf, "Controller ");
    i = 0;
    int_to_str(gGame.playerPad[gPausePlayer] + 1, buf + strlen(buf));
    font_text_add(gGame.texts[0], buf);
    font_draw_text(gGame.texts[0], 0xBE, 0x2A);
    font_text_create(gGame.texts[0], 0x40, 4, 2, 0x21);
    strcpy(buf, "Configuration ");
    buf[strlen(buf) + 1] = 0;
    buf[strlen(buf)] = gGame.padConfig[gGame.playerPad[gPausePlayer]] + 'A';
    font_text_add(gGame.texts[0], buf);
    font_draw_text(gGame.texts[0], 0xEE, 0xC2);

    for (; i < 9; i++) {
        odd = i & 1;
        color = odd ? 4 : 6;
        left = i < 5;
        textFlags = left ? 0x22 : 0x20;
        if (!odd) {
            textFlags |= 0x40000000;
        }
        font_text_create(gGame.texts[0], 0x40, 5, color, textFlags);
        font_text_add(gGame.texts[0], gControlActionLabels[gControlConfigs[(s8)gGame.padConfig[gGame.playerPad[gPausePlayer]]][i]]);
        font_draw_text(gGame.texts[0], gControlLabelPos[i].x + 4, gControlLabelPos[i].y + 5);
        if (gControlConfigs[(s8)gGame.padConfig[gGame.playerPad[gPausePlayer]]][i] == 6) {
            color = odd ? 4 : 6;
            textFlags = left ? 0x22 : 0x20;
            if (!odd) {
                textFlags |= 0x40000000;
            }
            font_text_create(gGame.texts[0], 0x40, 5, color, textFlags);
            font_text_add(gGame.texts[0], gControlReverseLabel);
            font_draw_text(gGame.texts[0], gControlLabelPos[i].x + 4, gControlLabelPos[i].y + 0x10);
        }
    }
    for (i = 0; i < 3; i++) {
        font_text_create(gGame.texts[0], 0x40, 5, (i == 2) ? 4 : 6, (i != 2) ? 0x40000020 : 0x20);
        font_text_add(gGame.texts[0], gCameraButtonLabels[i]);
        font_draw_text(gGame.texts[0], gCameraLabelPos[i].x + 4, gCameraLabelPos[i].y + 5);
    }
    gdl = font_render(gdl);
    }
    return gdl;
}

Gfx *render_clear_viewports(Gfx *gdl)
{
    s32 i;
    s32 screens;
    s32 layout;
    s32 lim;

    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    screens = gNumScreens;
    gDPSetFillColor(gdl++, (GPACK_RGBA5551(gFogColor[0], gFogColor[1], gFogColor[2], 1) << 16) |
                               GPACK_RGBA5551(gFogColor[0], gFogColor[1], gFogColor[2], 1));
    i = 0;
    layout = screens - 1;
    if (screens > 0) {

    do {
        gDPFillRectangle(gdl++, gViewRects[layout][i].x, gViewRects[layout][i].y,
                         gViewRects[layout][i].x + gViewRects[layout][i].w - 1,
                         gViewRects[layout][i].y + gViewRects[layout][i].h - 1);
    i++;
    } while (i < gNumScreens);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define GFX_LEFT() (0x1770 - (gdl - dyn->glist))

/* The low byte of gPalBrightness, read as a scalar at a fixed address (indexing it would order the read after the
   display list stores). */
#define PAL_BRIGHTNESS_LO (*(u8 *)((u32)&gPalBrightness + 3))

#define CHECK(n)                 \
    if (GFX_LEFT() < (n)) {      \
        *pgdl = gdl;             \
        return;                  \
    }

 /* 0x7E0 */

/* gRiderGfx: the matrices; the [8] are per frame and view, frame * 4 + view */
typedef struct {
    u8 pad0[0x234];
    Mtx *boneMtx[2];     /* 0x234 */
    Mtx *partMtx[2];     /* 0x23C */
    s32 pad244;
    Mtx bikeMtx[8];      /* 0x248 the bike's position (relative to the camera off the stadium) */
    Mtx bikeViewMtx[8];  /* 0x448 times the view's projection */
    Mtx bodyMtx[8];      /* 0x648 the thrown rider's */
    Mtx bodyViewMtx[8];  /* 0x848 */
    u8 padA48[0x38];
} PG; /* 0xA80 */

typedef struct {
    u8 pad0[0x2C];
    s32 envType; /* 0x2C */
    u8 pad30[0x20];
    s32 flags;   /* 0x50 */
    u8 pad54[0x4C];
} Track_4; /* 0xA0 */

extern Track_4 *gCurTrack_12;
extern Track_4 gTracks_6[];
extern PG gRiderGfx_5[8];

void render_draw_race(Dyn *dyn, s32 fb, Gfx **pgdl) {
    Mtx rot;
    Mtx tmpB;
    Mtx tmpA;
    Mtx viewProj;
    u8 unused[0x70];
    Mtx persp;
    Gfx *gdl;
    s32 i;
    s32 k;
    s32 row;
    s32 col;
    s32 skyMode;
    s32 alpha;
    s32 section;
    s32 ang;
    s32 lod;
    Mtx *proj;

    sCurViewport = -1;
    sRidersDrawn = 0;
    if (gCheatFlags & 0x100) {
        for (i = 0; i < 8; i++) {
            gRiderAlpha[i] = 0x80;
        }
    }
    if (gGame.numPlayers == 1 && gRiders[0].pipTimer != 0 && !(gGame.flags & 0x81) && gReplayPlayback == 0 &&
        gGame.mode != 6 && gGame.mode != 7 && gGame.mode != 8 && gGame.mode != 9 &&
        gGame.mode != 13 && gGame.mode != 14 && gCurTrack_12->envType != 2) {
        sInsetViewWanted = gGame.numPlayers;
    } else {
        sInsetViewWanted = 0;
    }
    render_update_viewports();
    if (gDebugRiderCount <= 0) {
        gDebugRiderCount = 1;
    }
    if (gDebugRiderCount > 8) {
        gDebugRiderCount = 8;
    }
    D_800DC0AC = 0;
    sRenderFrame++;
    palmod_update();

    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        if (gCurTrack_12->envType == 0) {
            arn_update_view(gCurView);
        } else if (gCurTrack_12->envType == 1) {
            tube_update_view(gCurView);
        } else if (gCurTrack_12->envType == 2) {
            des_update_view(gCurView);
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0 && ((i + sRenderFrame) & 3) == 0 && gRiders[i].camMode != 9) {
            gGroundFound = 0;
            height_get_ground(gRiders[i].drawPos.x, gRiders[i].drawPos.y, gRiders[i].drawPos.z);
            if (gGroundFound != 0) {
                gRiderSection[i] = gGroundArea;
                gRiderGroundSurface[i] = gGroundSurface;
                if (gCurTrack_12->envType != 2) {
                    height_get_ground_color(gRiders[i].drawPos.x, gRiders[i].drawPos.z, &sRiderTint[i * 3]);
                } else {
                    sRiderTint[i * 3] = 0xFF;
                    sRiderTint[i * 3 + 1] = 0xFF;
                    sRiderTint[i * 3 + 2] = 0xFF;
                }
            }
        }
    }

    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        sFovY = 0x21;
        guOrtho(&dyn->m3[gCurView], -gScreenWidth / 2, gScreenWidth / 2, -gScreenHeight / 2, gScreenHeight / 2,
                      50.0f, gFarClip, 1.0f);
        for (i = 0; i < 2; i++) {
            ang = 0;
            gProjScale = gScreenHeight * 0.5f * gCosTable[(ang >> 4) & 0xFFF] /
                         gCosTable[((ang >> 4) - 0x400) & 0xFFF];
            gProjScale = gProjScale * gScreenHeight * 0.00390625f;
            k = (i != 0) * 2;
            if (gNumViews == 2 && gInsetViewActive == 0) {
                if (gSplitVertical != 0) {
                    guPerspective(&persp, &gPerspNorm, sFovY, 256.0f / gScreenHeight, k + 50.0f,
                                  gFarClip + k, 1.0f);
                } else {
                    guPerspective(&persp, &gPerspNorm, sFovY, 480.0f / gScreenHeight, k + 50.0f,
                                  gFarClip + k, 1.0f);
                }
            } else if (gViMode == 2) {
                guPerspective(&persp, &gPerspNorm, sFovY, 640.0f / gScreenHeight, k + 50.0f,
                              gFarClip + k, 1.0f);
            } else {
                guPerspective(&persp, &gPerspNorm, sFovY, 320.0f / gScreenHeight, k + 50.0f,
                              gFarClip + k, 1.0f);
            }
            proj = &persp;
            mtx_mul(&gViewAxisMtx, proj, &gHudPerspMtx[gGfxFrameIndex]);
            lod = ((s16 *)proj)[0];
            lod <<= 16;
            lod |= ((u16 *)proj)[16];
            sProjScaleX[gCurView] = lod * (1.0f / 65536);
            sProjScaleX[gCurView] = sProjScaleX[gCurView] * (gViewRects[gNumViews - 1][gCurView].w * 0.5f);
            lod = ((s16 *)proj)[5];
            lod <<= 16;
            lod |= ((u16 *)proj)[21];
            sProjScaleY[gCurView] = lod * (1.0f / 65536);
            sProjScaleY[gCurView] = sProjScaleY[gCurView] * (gViewRects[gNumViews - 1][gCurView].h * 0.5f);
            D_800E19C4 = proj;
            guRotateRPY(&tmpA, gCamPitch[gCurView] * 360.0f * (1.0f / 65536),
                          -(f32)gCamYaw[gCurView] * 360.0f * (1.0f / 65536), 0.0f);
            guRotateRPY(&tmpB, 0.0f, 0.0f, gCamRoll[gCurView] * 360.0f * (1.0f / 65536));
            guMtxCatL(&tmpB, &tmpA, &rot);
            mtx_transpose_rot(&rot);
            if (gCurTrack_12->envType == 0) {
                mtx_from_angles_pos_scale(&tmpB, 0, 0, 0, -gCamPos[gCurView].x, -gCamPos[gCurView].y,
                              -gCamPos[gCurView].z, 0x100);
            } else {
                mtx_from_angles_pos_scale(&tmpB, 0, 0, 0, 0, 0, 0, 0x100);
            }
            mtx_mul(&tmpB, &rot, &tmpA);
            if (gMirrorMode != 0) {
                mtx_mul(&tmpA, &sMirrorViewAxisMtx, &tmpB);
            } else {
                mtx_mul(&tmpA, &gViewAxisMtx, &tmpB);
            }
            mtx_mul(&tmpB, &persp, &viewProj);
            if (i == 0) {
                dyn->m0[gCurView] = viewProj;
            } else {
                dyn->m1[gCurView] = viewProj;
            }
        }
        if (gCurTrack_12->envType == 0) {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, gCamPos[gCurView].x,
                          gCamPos[gCurView].y - 500, gCamPos[gCurView].z, 0x10);
        } else if (gCurTrack_12->envType == 1) {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, 0, 0, 0, 0x10);
        } else {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, 0, -2000, 0, 0x10);
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0) {
            section = gRiderSection[i];
            gRiderViewLod[i] = 0;
            for (k = 0; k < gNumViews; k++) {
                lod = render_get_rider_lod((s32)((f32)(s32)gRiders[i].bike.pos.x * 1000.0f),
                                  (s32)((f32)(s32)gRiders[i].bike.pos.y * 1000.0f),
                                  (s32)((f32)(s32)gRiders[i].bike.pos.z * 1000.0f), k, section, dyn, i) &
                    0xF;
                if (gRiders[i].crashState != 0) {
                    lod |= (render_get_rider_lod(gRiders[i].drawBodyPos.x, gRiders[i].drawBodyPos.y, gRiders[i].drawBodyPos.z, k, section, dyn, i) & 0xF)
                         << 4;
                } else {
                    lod |= lod << 4;
                }
                gRiderViewLod[i] |= lod << (k * 8);
            }
        }
    }

    gdl = *pgdl;
    mtx_from_angles_pos_scale(&dyn->m4, 0, 0, 0, 0, 0, 0, 0x100);
    if (gRacePhase_2 == 3 || gRacePhase_2 == 7) {
        gSPSegment(gdl++, 0, 0);
        gSPDisplayList(gdl++, gRdpInitDL);
        gSPDisplayList(gdl++, gRspInitDL);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
        gSPViewport(gdl++, &gScreenViewport);
        gSPPerspNormalize(gdl++, gPerspNorm);
        gDPPipeSync(gdl++);
        gDPSetDepthImage(gdl++, gZBuffer + 0x80000000);
        gDPPipeSync(gdl++);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer + 0x80000000);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetFillColor(gdl++, 0xFFFCFFFC);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
        gDPPipeSync(gdl++);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gSPSetGeometryMode(gdl++, gCullMode);
        gDPSetColorDither(gdl++, G_CD_MAGICSQ);
        if (gRacePhaseTimer > 0) {
            gRacePhaseTimer--;
        }
        goto end;
    }
    switch (gPauseMenu) {
        case 3:
            gdl = render_draw_sound_menu(gdl, fb, dyn);
            goto end;
        case 2:
            gdl = render_draw_controls_menu(gdl, fb, dyn);
            goto end;
        case 1:
            if (gPauseScreen.timer != -1) {
                if (gPauseScreen.timer > 0) {
                    gPauseScreen.timer--;
                }
                goto end;
            }
            break;
    }
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, gRdpInitDL);
    gSPDisplayList(gdl++, gRspInitDL);
    gCurView = 0;
    CHECK(1000);
    if ((gDrawFlags & 0x40) && gPodiumMode == 0) {
        for (i = 0; i < 8; i++) {
            if (gRiders[i].inactive == 0 && !((i ^ sRenderFrame) & 1) && gRiderViewLod[i] != 0) {
                gdl = shadow_render_caster(i, shadow_clear_texture(gdl, dyn, i), dyn);
            }
        }
    }
    sky_update_scroll();
    if (gNumScreens == 1) {
        sun_update_screen_pos(dyn, 0, fb);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gSPViewport(gdl++, &gScreenViewport);
    gSPPerspNormalize(gdl++, gPerspNorm);
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, gZBuffer + 0x80000000);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer + 0x80000000);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    if (sClearFrames != 0 || gPauseScreen.clearFrames != 0 || gPauseMenu != 0) {
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
        if (gPauseScreen.clearFrames != 0) {
            gPauseScreen.clearFrames--;
        }
    }
    if (sClearFrames != 0) {
        sClearFrames--;
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, gCullMode);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    if (sClearFrames != 0) {
        *pgdl = gdl;
        return;
    }
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    if (gSkyMode == 0) {
        gdl = render_clear_viewports(gdl);
    } else if (gSkyMode == 1) {
        if (gNumScreens < 4) {
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x0C084000, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
            gdl = sky_draw(gdl, dyn, gSkyImage);
        } else {
            gdl = render_clear_viewports(gdl);
        }
    }
    skyMode = gSkyMode;
    if (skyMode == 2 || skyMode == 3) {
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gSPClipRatio(gdl++, FRUSTRATIO_3);
        if (skyMode == 3) {
            gDPLoadTLUT_pal256(gdl++, gSkyImage->pal);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
            gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        } else {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetCombine(gdl++, 0x61FEC3, 0x44FE7F3F);
            gDPSetPrimColor(gdl++, 0, 0, PAL_BRIGHTNESS_LO, PAL_BRIGHTNESS_LO, PAL_BRIGHTNESS_LO, 0);
            gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        }
        for (gCurView = 0; gCurView < gNumViews; gCurView++) {
            gdl = render_set_viewport(gdl, dyn, gCurView, 1);
            gSPMatrix(gdl++, &dyn->m2[gCurView], G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
            if (gDrawFlags & 2) {
                gdl = prim_build_dl(0, sSkyModel, &gSkyImage, gdl);
            }
            gDPPipeSync(gdl++);
            gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
        }
        gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    }
    if (gInsetViewActive != 0) {
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer + 0x80000000);
        gDPSetFillColor(gdl++, 0x7FFC7FFC);
        gDPFillRectangle(gdl++, gViewRects[1][1].x, gViewRects[1][1].y,
                         gViewRects[1][1].x + gViewRects[1][1].w - 4, gViewRects[1][1].y + gViewRects[1][1].h - 4);
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    }
    if (gNumScreens == 1 && (gReplayPlayback != 0 || (gGame.flags & 0x81))) {
        gdl = sun_draw_stars(render_set_viewport(gdl, dyn, 0, 1), dyn);
    }
    mem_fill((u8 *)sRiderDrawFlags, 0, 0x20);

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0) {
            if (gRiderViewLod[i] & 0x0F0F0F0F) {
                bike_skel_update(gBikeSkelTemplate, i);
                *gRiders[i].bikeSkel = *gBikeSkelTemplate;
                if (gPodiumMode == 0) {
                    for (k = 0; k < 6; k++) {
                        Bone *bn = &gRiders[i].bikeSkel->bones[sBikePartBones[k]];
                        mtx_from_rot3i_pos(bn->worldRot.m, (u16 *)&bn->worldPos, gRiderGfx_5[i].partMtx[fb] + k);
                    }
                }
            }
            if (gRiderViewLod[i] & 0xF0F0F0F0) {
                if (gRiders[i].isCpu == 0 || !((sRenderFrame / 2 + i) & 1) || gNumScreens != 1 ||
                    ((gRiderViewLod[i] >> 4) & 0xF) == 3 || gRiders[i].crashState != 0 || (gGame.flags & 1)) {
                    rider_update_pose(gRiders[i].riderSkel, i);
                }
                mtx_from_bones(gRiders[i].riderSkel->bones, gRiderGfx_5[i].boneMtx[fb], 0xF);
                if (gPodiumMode == 0 && (sBigHeadCheat != 0 || sPinHeadCheat != 0)) {
                    u16 *m = (u16 *)&gRiderGfx_5[i].boneMtx[fb][2];

                    for (row = 0; row < 3; row++) {
                        for (col = 0; col < 3; col++) {
                            s32 v = m[row * 4 + col];

                            v <<= 16;
                            v |= m[row * 4 + col + 16];

                            if (sBigHeadCheat != 0) {
                                v <<= 1;
                            } else {
                                v >>= 1;
                            }
                            m[row * 4 + col] = v >> 16;
                            m[row * 4 + col + 16] = v;
                        }
                    }
                }
            }
            for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                if (gRiderViewLod[i] & (0xFF << (gCurView * 8))) {
                    if (gCurTrack_12->envType == 0) {
                        mtx_from_angles_pos_scale(&gRiderGfx_5[i].bikeMtx[fb * 4 + gCurView], 0, 0, 0, gRiders[i].drawPos.x,
                                      gRiders[i].drawPos.y, gRiders[i].drawPos.z, 0x10);
                        if (gRiders[i].crashState != 0) {
                            mtx_from_angles_pos_scale(&gRiderGfx_5[i].bodyMtx[fb * 4 + gCurView], 0, 0, 0, gRiders[i].drawBodyPos.x,
                                          gRiders[i].drawBodyPos.y, gRiders[i].drawBodyPos.z, 0x10);
                        }
                    } else {
                        mtx_from_angles_pos_scale(&gRiderGfx_5[i].bikeMtx[fb * 4 + gCurView], 0, 0, 0,
                                      gRiders[i].drawPos.x - gCamPos[gCurView].x,
                                      gRiders[i].drawPos.y - gCamPos[gCurView].y,
                                      gRiders[i].drawPos.z - gCamPos[gCurView].z, 0x10);
                        if (gRiders[i].crashState != 0) {
                            mtx_from_angles_pos_scale(&gRiderGfx_5[i].bodyMtx[fb * 4 + gCurView], 0, 0, 0,
                                          gRiders[i].drawBodyPos.x - gCamPos[gCurView].x,
                                          gRiders[i].drawBodyPos.y - gCamPos[gCurView].y,
                                          gRiders[i].drawBodyPos.z - gCamPos[gCurView].z, 0x10);
                        }
                    }
                    mtx_mul(&gRiderGfx_5[i].bikeMtx[fb * 4 + gCurView], &dyn->m0[gCurView],
                                  &gRiderGfx_5[i].bikeViewMtx[fb * 4 + gCurView]);
                    if (gRiders[i].crashState != 0) {
                        mtx_mul(&gRiderGfx_5[i].bodyMtx[fb * 4 + gCurView], &dyn->m0[gCurView],
                                      &gRiderGfx_5[i].bodyViewMtx[fb * 4 + gCurView]);
                    }
                }
            }
        }
    }

    gSPClipRatio(gdl++, FRUSTRATIO_2);
    CHECK(1500);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gdl = render_draw_riders(gdl, fb, dyn, 0);
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_2);
    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        CHECK(500);
        gdl = render_set_viewport(gdl, dyn, gCurView, 1);
        if (gDrawFlags & 8) {
            if (gCurTrack_12->envType == 0) {
                gdl = arn_draw_far(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->envType == 1) {
                gdl = tube_draw_far(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->envType == 2) {
                gdl = des_draw_terrain(gdl, dyn, fb, gCurView);
            }
        }
        gdl = hacks_draw_pass1(gdl, dyn, fb, gCurView);
    }
    CHECK(500);
    gdl = render_restore_state(render_load_ucode(gdl, fb, dyn, gF3dex2NonText, gF3dex2NonData), fb, dyn);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    if (gDrawFlags & 4) {
        for (gCurView = 0; gCurView < gNumViews; gCurView++) {
            CHECK(500);
            gdl = render_set_viewport(gdl, dyn, gCurView, 1);
            if (gCurTrack_12->envType == 0) {
                gdl = arn_draw(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->envType == 1) {
                gdl = tube_draw(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->envType == 2) {
                gdl = des_draw_near_terrain(gdl, dyn, fb, gCurView);
            }
            gdl = hacks_draw_pass2(gdl, dyn, fb, gCurView);
        }
    }
    if (gPodiumMode != 0) {
        gdl = podium_draw(gdl, dyn, fb);
    }
    gDPPipeSync(gdl++);
    switch (gGame.mode) {
        case 2:
            CHECK(2000);
            gdl = soccer_ball_draw(gdl, dyn, fb);
            break;
        case 9:
            CHECK(500);
            gdl = finishline_draw_tutorial(gdl, dyn, fb);
            break;
        case 0:
        case 13:
        case 14:
            if (gCurTrack_12->flags & 2) {
                CHECK(1000);
                for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                    gdl = finishline_draw_user_track(gdl, dyn, fb, gCurView);
                }
            }
            break;
    }
    CHECK(1000);
    if ((gDrawFlags & 0x40) && gPodiumMode == 0) {
        gDPPipeSync(gdl++);
        gSPClipRatio(gdl++, FRUSTRATIO_1);
        for (i = 0; i < 8; i++) {
            if (gRiders[i].inactive == 0) {
                for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                    if ((gRiderViewLod[i] >> (gCurView * 8)) & 0xF) {
                        if (!(sRiderDrawFlags[i] & 0x10)) {
                            shadow_build_mesh(i);
                            sRiderDrawFlags[i] |= 0x10;
                        }
                        gdl = render_set_viewport(gdl, dyn, gCurView, 0);
                        gSPMatrix(gdl++, &dyn->m1[gCurView], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                        gSPMatrix(gdl++, &gRiderGfx_5[i].bikeMtx[fb * 4 + gCurView],
                                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        gdl = shadow_draw(i, dyn, gdl, gCurView);
                    }
                }
                if (gRiders[i].crashState != 0) {
                    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                        if ((gRiderViewLod[i] >> (gCurView * 8)) & 0xF0) {
                            if (!(sRiderDrawFlags[i] & 0x20)) {
                                shadow_build_mesh(i + 8);
                                sRiderDrawFlags[i] |= 0x20;
                            }
                            gdl = render_set_viewport(gdl, dyn, gCurView, 0);
                            gSPMatrix(gdl++, &dyn->m1[gCurView], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                            gSPMatrix(gdl++, &gRiderGfx_5[i].bodyMtx[fb * 4 + gCurView],
                                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            gdl = shadow_draw(i + 8, dyn, gdl, gCurView);
                        }
                    }
                }
            }
        }
        gDPPipeSync(gdl++);
    }
    CHECK(1000);
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        gdl = render_set_viewport(gdl, dyn, gCurView, 0);
        gSPMatrix(gdl++, &dyn->m1[gCurView], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = decal_draw(gdl, fb, dyn, gCurView);
    }
    sCurViewport = -1;
    CHECK(1000);
    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        gdl = render_set_viewport(gdl, dyn, gCurView, 1);
        if (gCurTrack_12->envType == 1) {
            gdl = tube_draw_water(gdl, dyn, fb, gCurView);
        } else if (gCurTrack_12->envType == 0) {
            gdl = arn_draw_water(gdl, dyn, fb, gCurView);
        } else if (gCurTrack_12->envType == 2) {
            gdl = des_draw_waypoint_marker(gdl, dyn, fb, gCurView);
        }
    }
    if ((gDrawFlags & 0x80) && gDebugFreeze == 0) {
        for (gCurView = 0; gCurView < gNumViews; gCurView++) {
            CHECK(1000);
            gdl = particles_draw(render_set_viewport(gdl, dyn, gCurView, 1), gCurView, dyn);
        }
    }
    if (!(gGame.flags & 1) && gRaceFrame >= 0x259 && gReplayPlayback == 0 && gGame.mode != 8) {
        s32 tr = gCurTrack_12 - gTracks_6;

        if (tr != 20 && tr != 23 && tr != 24) {
            gdl = render_draw_place_markers(gdl, dyn);
        }
    }
    CHECK(1000);
    if (gNumScreens == 1) {
        s32 tr = gCurTrack_12 - gTracks_6;

        if (tr != 11 && tr != 20) {
            if (gNumViews != gNumScreens) {
                gdl = render_set_viewport(gdl, dyn, 0, 1);
            }
            gdl = sun_draw_flare(gdl, dyn, fb);
        }
    }
    CHECK(1000);
    gdl = snow_draw(rain_draw(gdl, dyn, fb), dyn, fb);
    if (gInsetViewActive != 0) {
        s32 x = gViewRects[1][1].x;
        s32 y = gViewRects[1][1].y;
        s32 w = gViewRects[1][1].w;
        s32 h = gViewRects[1][1].h;
        s32 col = y + h;
        s32 b1 = col - 1;

        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPPipeSync(gdl++);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0);
        gDPFillRectangle(gdl++, x - 2, y - 2, x + w + 1, y);
        gDPFillRectangle(gdl++, x - 2, y, x, b1);
        gDPFillRectangle(gdl++, x - 2, b1, x + w + 1, col + 1);
        gDPFillRectangle(gdl++, x + w - 1, y, x + w + 1, col + 1);
    }
    if (gCurTrack_12->envType == 2) {
        gdl = des_draw_arrow(gdl, dyn, fb, 0);
    }
    CHECK(1000);
    gdl = render_set_viewport(gdl, dyn, 0, 1);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0 && gRiderAlpha[i] != 0xFF) {
            gdl = render_restore_state(render_load_ucode(gdl, fb, dyn, gF3dlx2RejText, gF3dlx2RejData), fb, dyn);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gSPClipRatio(gdl++, FRUSTRATIO_2);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gSPSetGeometryMode(gdl++, G_ZBUFFER);
            gdl = render_draw_riders(gdl, fb, dyn, 1);
            gSPClipRatio(gdl++, FRUSTRATIO_1);
            break;
        }
    }
    CHECK(800);
    if (gPodiumMode != 0) {
        gdl = hud_draw_congratulations_2(gdl, dyn, fb);
    } else if (gDrawFlags & 0x800) {
        if (gCreditsMode == 0 && !(gGame.flags & 1)) {
            if (gNumViews == 1 || gPauseMenu == 0) {
                gdl = map_draw_minimap(gdl, dyn, fb);
            }
            gSPClearGeometryMode(gdl++, G_CULL_BOTH);
            sCurViewport = -1;
            for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                gdl = event_draw_hud(render_set_viewport(gdl, dyn, gCurView, 1), dyn, fb, gCurView);
                if (gInsetViewActive != 0) {
                    break;
                }
            }
            gSPSetGeometryMode(gdl++, gCullMode);
        }
    }
    if (gGame.flags & 0x80) {
        gdl = hud_draw_track_intro(gdl, dyn, fb, gCurView);
    }
    if (gCreditsMode != 0) {
        gdl = render_draw_credits(render_set_viewport(gdl, dyn, 0, 0));
    }
    if (!(gGame.flags & 1)) {
        switch (gRacePhase_2) {
            case 0:
            case 4:
                if (gRacePhaseTimer == -1) {
                    gRacePhaseTimer = 20;
                }
                gGame.fadeState = 2;
                alpha = gRacePhaseTimer * 255 / 20;
                gGame.fadeLevel = alpha;
                break;
            case 2:
            case 6:
                if (gRacePhaseTimer == -2) {
                    gRacePhaseTimer = -3;
                    alpha = 255;
                } else if (gRacePhaseTimer == -1) {
                    gRacePhaseTimer = 20;
                    alpha = 0;
                } else {
                    alpha = 255 - gRacePhaseTimer * 255 / 20;
                }
                gGame.fadeState = 0;
                gGame.fadeLevel = 255 - alpha;
                break;
            default:
                gGame.fadeState = 3;
                alpha = -1;
                break;
        }
        if (alpha != -1) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
            gdl = screen_draw_wipe(gdl, dyn, fb);
        }
    }
end:
    *pgdl = gdl;
    gRaceFrameDrawn = 1;
    sRidersDrawnLast = sRidersDrawn;
}

/* gRiderGfx as render_draw_riders sees it */
typedef struct {
    u32 riderTex;
    u32 podiumTex;
    u32 bikeTex;
    u8 padC[0x14 - 0xC];
    u32 riderPal;
    u8 *selectPals;
    u16 bikePal[(0x224 - 0x1C) / 2];
    Gfx *riderDLLow;
    Gfx *riderDLHigh;
    u32 riderVtxLow;
    u32 riderVtxHigh;
    u32 boneMtx[2];
    Mtx *partMtx[2];
    u8 pad244[0x448 - 0x244];
    Mtx bikeViewMtx[16];  /* only 8, then bodyMtx */
    Mtx bodyViewMtx[8];
    u8 padA48[0xA4C - 0xA48];
    u8 *ponytailMtx;      /* per frame, three bones */
    u8 padA50[4];
    void *ponytailModel;
    u8 padA58[0xA80 - 0xA58];
} Bike;

typedef struct {
    Gfx *dl0;
    Gfx *dl4;
} DlPair;

typedef struct {
    u8 pad0[4];
    u32 seg;
} SegSrc;

typedef struct {
    SegSrc *p0;
    SegSrc *p4;
} SegPair;

extern Bike gRiderGfx_6[];
extern DlPair sBikePartDLs_3[];
extern SegPair sBikePartModels_2[];

Gfx *render_draw_riders(Gfx *gdl, s32 fb, s32 dyn, s32 translucent) {
    s32 view;
    s32 rider;
    s32 k;
    s32 lod;
    u32 lodBits;
    s32 viewSet;
    u32 *segs;

    for (view = 0; view < gNumViews; view++) {
        viewSet = 0;
        for (rider = 0; rider < 8; rider++) {
            if (gRiders[rider].inactive != 0) {
                continue;
            }
            lodBits = gRiderViewLod[rider] >> (view * 8);
            if (!(lodBits & 0xFF)) {
                continue;
            }
            k = gRiderAlpha[rider] != 0xFF;
            if (k != translucent) {
                continue;
            }
            if (gGame.flags & 0x10) {
                lod = 2;
                if (rider == gGame.riders[view]) {
                    lod = 3;
                }
            } else {
                lod = lodBits & 0xF;
                if (gNumScreens >= 3) {
                    if (lod == 3) {
                        lod = 2;
                    }
                    if (view == rider && lod == 1) {
                        lod = 2;
                    }
                }
                if (gNumScreens == 2 && view == rider) {
                    lod = 3;
                }
                if (gEvent.noPlayers != 0 && rider == 0) {
                    lod = 3;
                }
                if (gNumScreens == 1 && (rider == gCamTargetOverride || (gCamTargetOverride == -1 && rider == 0))) {
                    lod = 3;
                }
            }
            if (gPodiumMode != 0) {
                lod = 3;
                if (gRiderGfx_6[rider].riderDLHigh == 0) {
                    continue;
                }
            }
            if (!viewSet) {
                gdl = render_set_viewport(gdl, dyn, view, 1);
                viewSet = 1;
            }
            sRidersDrawn++;
            if (!(sRiderDrawFlags[rider] & 4)) {
                render_update_rider_palette(rider, view);
                if (!(gGame.flags & 0x10)) {
                    sRiderDrawFlags[rider] |= 4;
                }
            }
            if (gGame.flags & 0x10) {
                if (gGame.numPlayers == 1) {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[rider].selectPals + gGame.riderSkinSel * 0x200);
                } else {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[rider].selectPals + view * 0x200);
                }
            } else {
                gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[rider].riderPal);
            }
            if (gRiders[rider].crashState != 0) {
                gSPMatrix(gdl++, &gRiderGfx_6[rider].bodyViewMtx[fb * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            } else {
                gSPMatrix(gdl++, &gRiderGfx_6[rider].bikeViewMtx[fb * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            }
            if (translucent == 0) {
                gDPSetRenderMode(gdl++, 0x442230, 0);
            } else {
                gDPSetRenderMode(gdl++, 0x404A70, 0);
                gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[rider]);
            }
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
            if (gDrawFlags & 1) {
                if (lod == 3) {
                    gSPSegment(gdl++, 1, gRiderGfx_6[rider].riderVtxHigh & 0xFFFFFF);
                    if (gGame.flags & 0x10) {
                        if (gGame.numPlayers == 1) {
                            gSPSegment(gdl++, 2, sRiderSkinTextures_2[rider][gGame.riderSkinSel] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, sRiderSkinTextures_2[rider][view] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, gRiderGfx_6[rider].riderTex & 0xFFFFFF);
                        gSPSegment(gdl++, 3, gRiderGfx_6[rider].podiumTex & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, gRiderGfx_6[rider].boneMtx[gGfxFrameIndex]);
                    if (!(gCheatFlags & 0x20)) {
                        gSPDisplayList(gdl++, gRiderGfx_6[rider].riderDLHigh);
                    }
                } else {
                    gSPSegment(gdl++, 1, gRiderGfx_6[rider].riderVtxLow & 0xFFFFFF);
                    if (gGame.flags & 0x10) {
                        if (gGame.numPlayers == 1) {
                            gSPSegment(gdl++, 2, sRiderSkinTextures_2[rider][gGame.riderSkinSel % 5] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, sRiderSkinTextures_2[rider][view] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, gRiderGfx_6[rider].riderTex & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, gRiderGfx_6[rider].boneMtx[gGfxFrameIndex]);
                    if (!(gCheatFlags & 0x20)) {
                        gSPDisplayList(gdl++, gRiderGfx_6[rider].riderDLLow);
                    }
                }
                if (gRiderGfx_6[rider].ponytailModel != 0) {
                    gPrimBoneMtx = (Mtx *)(gRiderGfx_6[rider].ponytailMtx + fb * 0xC0);
                    if (!(gCheatFlags & 0x20)) {
                        if (gGame.flags & 0x10) {
                            segs = sRiderSkinTextures_2[rider];
                            gdl = prim_build_dl(0, gRiderGfx_6[rider].ponytailModel,
                                                (gGame.numPlayers == 1) ? segs + gGame.riderSkinSel : segs + view, gdl);
                        } else {
                            gdl = prim_build_dl(0, gRiderGfx_6[rider].ponytailModel, &gRiderGfx_6[rider], gdl);
                        }
                    }
                }
            }
            if (gPodiumMode == 0) {
                if (gRiders[rider].crashState != 0) {
                    gSPMatrix(gdl++, &gRiderGfx_6[rider].bikeViewMtx[fb * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                }
                if (gGame.flags & 0x10) {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[(rider + gGame.bikeSkinSel) % 6].bikePal);
                } else {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[rider].bikePal);
                }
                for (k = 0; k < 6; k++) {
                    if (translucent != 1) {
                        if (k != 5 && k != 2 && lod != 1) {
                            gDPSetRenderMode(gdl++, 0x442230, 0);
                        } else {
                            gDPSetRenderMode(gdl++, 0x553078, 0);
                        }
                        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                    } else {
                        gDPSetRenderMode(gdl++, 0x404A70, 0);
                        gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
                    }
                    if (gDrawFlags & 1) {
                        gSPMatrix(gdl++, gRiderGfx_6[rider].partMtx[gGfxFrameIndex] + k, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        if (gGame.flags & 0x10) {
                            { Gfx *g = gdl++; g->words.w0 = 0xDB060008; if (rider == 0) lodBits = 0; gdl[-1].words.w1 = gRiderGfx_6[(rider + gGame.bikeSkinSel) & 1].bikeTex; }
                        } else {
                            gSPSegment(gdl++, 2, gRiderGfx_6[rider].bikeTex);
                        }
                        if (lod == 1) {
                            gSPSegment(gdl++, 2, gRiderGfx_6[rider].bikeTex);
                            gSPDisplayList(gdl++, sBikeLowDL);
                            break;
                        }
                        if (lod == 3) {
                            gSPSegment(gdl++, 1, sBikePartModels_2[k].p4->seg);
                            gSPDisplayList(gdl++, sBikePartDLs_3[k].dl4);
                        } else {
                            gSPSegment(gdl++, 1, sBikePartModels_2[k].p0->seg);
                            gSPDisplayList(gdl++, sBikePartDLs_3[k].dl0);
                        }
                    }
                }
            }
            {
                s32 t;
                if (((u32)t >> 28) & 16) {
                    k = 5;
                }
            }
            gDPPipeSync(gdl++);
        }
    }
    sCurViewport = -1;
    return gdl;
}

typedef struct {
    u8 pad0[4];
    u16 *pal;
} Tex;

/* gRiderGfx: the palettes */
typedef struct {
    u8 pad0[0xC];
    u16 *riderPalSrc;  /* 0x0C */
    u16 *bikePalSrc;   /* 0x10 */
    u16 *riderPal;     /* 0x14 */
    u16 *selectPals;   /* 0x18 */
    u16 bikePal[256];  /* 0x1C */
    u8 pad21C[0xA80 - 0x21C];
} Pal_4;

typedef struct {
    u8 pad0[0x2C];
    s32 envType;
    u8 pad30[0xA0 - 0x30];
} Track_5;

extern Pal_4 gRiderGfx_7[];
extern Tex *sRiderSkinTextures_3[][6];
extern Track_5 *gCurTrack_13;
extern Track_5 gTracks_7[];

void render_update_rider_palette(s32 rider, s32 view)
{
    s32 r, g, b;
    s32 i, c, v, fade;
    s32 fogR, fogG, fogB;
    s32 tintR, tintG, tintB;

    if (gPodiumMode != 0) {
        return;
    }
    if (gGame.flags & 0x10) {
        if (gGame.numPlayers == 1) {
            gRiderGfx_7[rider].riderPalSrc = sRiderSkinTextures_3[rider][(view + gGame.riderSkinSel) % 5]->pal;
            gRiderGfx_7[rider].riderPal = gRiderGfx_7[rider].selectPals + ((view + gGame.riderSkinSel) % 5) * 0x100;
        } else {
            gRiderGfx_7[rider].riderPalSrc = sRiderSkinTextures_3[rider][view]->pal;
            gRiderGfx_7[rider].riderPal = gRiderGfx_7[rider].selectPals + view * 0x100;
        }
    }
    i = 0;
    if (gCreditsMode != 0) {
        do {
            c = gRiderGfx_7[rider].riderPalSrc[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            palmod_sepia(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            gRiderGfx_7[rider].riderPal[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            c = gRiderGfx_7[rider].bikePalSrc[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            palmod_sepia(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            gRiderGfx_7[rider].bikePal[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            i++;
        } while (i < 0x100);
        gRiderGfx_7[rider].bikePal[0] &= 0xFFFE;
        return;
    }
    if (gLightningFlash != 0) {
        do {
            s32 x, y, z;

            x = gRiderGfx_7[rider].riderPalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            gRiderGfx_7[rider].riderPal[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            x = gRiderGfx_7[rider].bikePalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            gRiderGfx_7[rider].bikePal[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            i++;
        } while (i < 0x100);
        gRiderGfx_7[rider].bikePal[0] &= 0xFFFE;
        sRiderPalDirty[rider] = 1;
        return;
    }
    if ((gRiders[rider].engine.overheated != 0 && gRaceFrame % 30 < 15 && gNumScreens >= 2) ||
        (gRiders[rider].isCpu == 0 && (gCheatFlags & 0x400) && gRaceFrame % 32 < 16)) {
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[rider].riderPalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            gRiderGfx_7[rider].riderPal[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = gRiderGfx_7[rider].bikePalSrc[i];
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            gRiderGfx_7[rider].bikePal[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        gRiderGfx_7[rider].bikePal[0] &= 0xFFFE;
        sRiderPalDirty[rider] = 1;
        return;
    }
    if (gCurTrack_13->envType == 2 && sRiderCamDist[rider] > 0xC48) {
        fogR = gFogColor[0] * 31 / 256;
        fogG = gFogColor[1] * 31 / 256;
        fogB = gFogColor[2] * 31 / 256;
        fade = (sRiderCamDist[rider] - 0xC48) * 255 / 5241;
        if (fade > 255) {
            fade = 255;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[rider].riderPalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fogR - x) * fade) >> 8;
            y += ((fogG - y) * fade) >> 8;
            z += ((fogB - z) * fade) >> 8;
            gRiderGfx_7[rider].riderPal[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = gRiderGfx_7[rider].bikePalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fogR - x) * fade) >> 8;
            y += ((fogG - y) * fade) >> 8;
            z += ((fogB - z) * fade) >> 8;
            gRiderGfx_7[rider].bikePal[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        return;
    }
    sRiderDrawFlags[rider] |= 4;
    if (gRiderGroundSurface[rider] & 0x200) {
        if (sRiderTint[rider * 3] == sRiderLightColor[rider][0] && sRiderTint[rider * 3 + 1] == sRiderLightColor[rider][1] &&
            sRiderTint[rider * 3 + 2] == sRiderLightColor[rider][2] && sRiderPalDirty[rider] == 0) {
            return;
        }
        sRiderPalDirty[rider] = 0;
        tintR = sRiderTint[rider * 3];
        tintG = sRiderTint[rider * 3 + 1];
        tintB = sRiderTint[rider * 3 + 2];
        if (gCurTrack_13 - gTracks_7 == 15) {
            tintR += (255 - tintR) / 3;
            tintG += (255 - tintG) / 3;
            tintB += (255 - tintB) / 3;
        }
        sRiderLightColor[rider][0] = tintR;
        sRiderLightColor[rider][1] = tintG;
        sRiderLightColor[rider][2] = tintB;
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[rider].riderPalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * tintR) >> 8;
            y = (y * tintG) >> 8;
            z = (z * tintB) >> 8;
            gRiderGfx_7[rider].riderPal[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[rider].bikePalSrc[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * tintR) >> 8;
            y = (y * tintG) >> 8;
            z = (z * tintB) >> 8;
            gRiderGfx_7[rider].bikePal[i] = (x << 11) | (y << 6) | (z << 1) | (i != 0);
        }
    } else {
        __builtin_memcpy(gRiderGfx_7[rider].riderPal, gRiderGfx_7[rider].riderPalSrc, 0x200);
        __builtin_memcpy(gRiderGfx_7[rider].bikePal, gRiderGfx_7[rider].bikePalSrc, 0x200);
        sRiderLightColor[rider][0] = 0xFF;
        sRiderLightColor[rider][1] = 0xFF;
        sRiderLightColor[rider][2] = 0xFF;
    }
}

Gfx *render_restore_state(Gfx *gdl, s32 fb, s32 dyn)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *g9, *g10, *g11, *g12, *g13, *g14;
    f32 clipX = (f32)(gScreenWidth - 0x11) * 4.0f;
    f32 clipY = (f32)(gScreenHeight - 0xd) * 4.0f;
    s32 projOfs = gCurView * 0x40;

    g0 = gdl++;
    ((u32 *)g0)[0] = 0xED040030;
    ((u32 *)g0)[1] = (((s32)clipX & 0xfff) << 12) | ((s32)clipY & 0xfff);
    g1 = gdl++;
    ((u32 *)g1)[0] = 0xDC080008;
    ((u32 *)g1)[1] = (u32)gViewports;
    g2 = gdl++;
    ((u32 *)g2)[0] = 0xE7000000;
    ((u32 *)g2)[1] = 0;
    g3 = gdl++;
    ((u32 *)g3)[0] = 0xDB0E0000;
    ((u32 *)g3)[1] = (u32)gPerspNorm;
    g4 = gdl++;
    ((u32 *)g4)[0] = 0xFE000000;
    ((u32 *)g4)[1] = (u32)gZBuffer + 0x80000000;
    g5 = gdl++;
    ((u32 *)g5)[0] = 0xFF100000 | (((*(volatile s32 *)&gScreenWidth) - 1) & 0xfff);
    ((u32 *)g5)[1] = (u32)gFramebuffers[fb] + 0x80000000;
    g6 = gdl++;
    ((u32 *)g6)[0] = 0xE3000A01;
    ((u32 *)g6)[1] = 0;
    g7 = gdl++;
    ((u32 *)g7)[0] = 0xD9FFFFFF;
    ((u32 *)g7)[1] = (u32)gCullMode;
    g8 = gdl++;
    ((u32 *)g8)[0] = 0xE3001801;
    ((u32 *)g8)[1] = 0;
    g9 = gdl++;
    ((u32 *)g9)[0] = 0xDA380007;
    ((u32 *)g9)[1] = (u32)(dyn + projOfs) + 0x80000000;
    g10 = gdl++;
    ((u32 *)g10)[0] = 0xDA380003;
    ((u32 *)g10)[1] = (u32)dyn + 0x80000400;
    g11 = gdl++;
    ((u32 *)g11)[0] = 0xDB040004;
    ((u32 *)g11)[1] = 1;
    g12 = gdl++;
    ((u32 *)g12)[0] = 0xDB04000C;
    ((u32 *)g12)[1] = 1;
    g13 = gdl++;
    ((u32 *)g13)[0] = 0xDB040014;
    ((u32 *)g13)[1] = 0xFFFF;
    g14 = gdl++;
    ((u32 *)g14)[0] = 0xDB04001C;
    ((u32 *)g14)[1] = 0xFFFF;

    sCurViewport = -1;
    return gdl;
}

typedef struct {
    u8 pad[8];
    u8 w;
    u8 h;
    u8 pad2[2];
    void *tex;
} TexInfo_2;

extern TexInfo_2 *sMarkerImage_2;

Gfx *render_draw_place_markers(Gfx *gdl, s32 dyn) {
    s32 sx, sy, sz, sw;
    s32 markW, markH, markS, markT, digitW, digitH, digitS, digitT;
    s32 texW, texW8, texH, i, j, y, ofs;
    s32 gfxOfs;
    Rider *rider;

    digitW = gScreenWidth / 16;
    digitH = gScreenHeight / 15;
    digitS = 0x8000 / digitW;
    digitT = 0x4000 / digitH;
    texW8 = sMarkerImage_2->w;
    markW = gScreenWidth * 24 / 320;
    markS = (texW8 << 13) / markW;
    texH = sMarkerImage_2->h;
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gdl++, G_TF_POINT);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    texW = texW8 * 8;
    markH = gScreenHeight / 10;
    markT = (texH << 10) / markH;
    for (i = 0; i < gNumScreens; i++) {
        gdl = render_set_viewport(gdl, dyn, i, 0);
        for (j = 0; j < 8; j++) {
            gfxOfs = (j << 11) + (j << 9) + (j << 7);
            rider = &gRiders[j];
            if (rider->inactive != 0) continue;
            if (j >= 6) continue;
            if (!((gRiderViewLod[j] >> (i * 8)) & 0xF)) continue;
            if (rider->crashState != 0) continue;
            if (rider->race.place != gRiders[i].race.place - 1) continue;
            if (gCurTrack_4->f2C == 0) {
                if (project_to_screen(rider->drawPos.x >> 4, (rider->drawPos.y + 2000) >> 4, rider->drawPos.z >> 4, &sx, &sy, &sz, &sw, dyn, i) != 0) continue;
            } else {
                if (project_to_screen((rider->drawPos.x - gCamPos[i].x) >> 4, (rider->drawPos.y - gCamPos[i].y + 2000) >> 4,
                                  (rider->drawPos.z - gCamPos[i].z) >> 4, &sx, &sy, &sz, &sw, dyn, i) != 0) continue;
            }
            if (sx < -50) continue;
            if (sy < -50) continue;
            if (sx > gScreenWidth + 50) continue;
            if (sy > gScreenHeight + 50) continue;
            ofs = -12;
            y = sy + gScreenHeight * ofs / 240;
            gDPSetPrimDepth(gdl++, sz, 0);
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetTextureImage(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 1, sMarkerImage_2->tex);
            gDPSetTile(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 7, 0, 0, 7, 0);
            gDPLoadSync(gdl++);
            gDPLoadBlock(gdl++, G_TX_LOADTILE, 0, 0, (s32)((u32)(texW * texH) >> 2) - 1, CALC_DXT_4b(texW));
            gDPPipeSync(gdl++);
            {
                Gfx *g = gdl++;
                g->words.w0 = ((u32)((texW >> 1) + 7) >> 3 << 9) | 0xF5400000;
                g->words.w1 = 0x1C070;
            }
            gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (texW - 1) << G_TEXTURE_IMAGE_FRAC, (texH - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPLoadTLUT_pal256(gdl++, (u8 *)gRiderGfx_3[0].markerPal + gfxOfs);   /* pl = j * 0xA80: gRiderGfx_3[j].markerPal */
            gSPTextureRectangle(gdl++, (sx - markW / 2) * 4, (y - markH / 2) * 4, (sx + markW / 2) * 4, (y + markH / 2) * 4,
                                0, 0, 0, markS, markT);
            gDPPipeSync(gdl++);
            ofs = -7;
            y = sy + gScreenHeight * ofs / 240;
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            ofs = rider->race.place;
            gDPLoadTextureBlock(gdl++, sPlaceTextures + (ofs << 9), G_IM_FMT_CI, G_IM_SIZ_8b, 32, 16, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 5, 4, 0, 0);
            gSPTextureRectangle(gdl++, (sx - digitW / 2) * 4, (y - digitH / 2) * 4, (sx + digitW / 2) * 4, (y + digitH / 2) * 4,
                                0, 0, 0, digitS, digitT);
        }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    return gdl;
}

Gfx *render_draw_temp_gauge(Gfx *gdl, s32 dyn, s32 view, s32 player) {
    s32 rider;
    s32 stepS, stepT;
    s32 heat, level;
    s32 i;
    u16 *pal;
    s32 x, y;

    if (gGame.mode == 8) {
        rider = gTimeTrialPlayer;
    } else {
        rider = player;
    }
    stepS = 0x80000 / gScreenWidth;
    stepT = 0x3C000 / gScreenHeight;
    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, 0x0C087008, 0);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0xFE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gdl = render_set_viewport(gdl, dyn, 0, 1);
    gSPClipRatio(gdl++, FRUSTRATIO_6);

    heat = gRiders[rider].engine.heat * 256.0f / 12000.0f;
    if (heat < 0) {
        heat = 0;
    }
    if (heat > 255) {
        heat = 255;
    }
    pal = sGaugePalettes + view * 256;
    level = heat * 87 / 256;
    level += 148;
    for (i = 148; i < 192; i++) {
        if (level < i) {
            pal[i] = 0x201;
        } else {
            pal[i] = 0x7C1;
        }
    }
    for (i = 192; i < 224; i++) {
        if (level < i) {
            pal[i] = 0x4201;
        } else {
            pal[i] = 0xFFC1;
        }
    }
    for (i = 224; i < 235; i++) {
        if (level < i) {
            pal[i] = 0x4001;
        } else {
            pal[i] = 0xF801;
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, pal);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, sGaugeTextures->tex[1].data + i * 80, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, sGaugeX * 4, (sGaugeY + i * gScreenHeight / 240) * 4,
                            (sGaugeX + gScreenWidth * 80 / 512) * 4,
                            (sGaugeY + (i + 25) * gScreenHeight / 240) * 4, 0, 0, 0, stepS, stepT);
    }
    gDPPipeSync(gdl++);
    gDPSetBlendColor(gdl++, 0, 0, 0, 8);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    if (gRiders[rider].engine.overheated != 0) {
        gDPSetCombineLERP(gdl++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPLoadTextureBlock(gdl++, sGaugeTextures->tex[0].data, G_IM_FMT_IA, G_IM_SIZ_8b, 48, 16, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        level = (-gRaceFrame * 8) & 0xFF;
        gDPSetPrimColor(gdl++, 0, 0, 255, 0, 0, level);
        gSPTextureRectangle(gdl++, (sGaugeX + gScreenWidth / 32) * 4, (sGaugeY + gScreenHeight / 24) * 4,
                            (sGaugeX + gScreenWidth / 8) * 4, (sGaugeY + gScreenHeight * 23 / 240) * 4,
                            0, 0, 0, stepS, stepT);
        gDPPipeSync(gdl++);
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, sGaugeTextures->tex[2].data + i * 80, G_IM_FMT_IA, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, sGaugeX * 4, (sGaugeY + i * gScreenHeight / 240) * 4,
                            (sGaugeX + gScreenWidth * 80 / 512) * 4,
                            (sGaugeY + (i + 25) * gScreenHeight / 240) * 4, 0, 0, 0, stepS, stepT);
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
    gSPMatrix(gdl++, dyn + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPViewport(gdl++, &gScreenViewport);
    level = 55000 - heat * 175;
    for (i = 0; i < 4; i++) {
        s32 x0 = sGaugeNeedleShape[i * 2];
        s32 y0 = sGaugeNeedleShape[i * 2 + 1];
        x = (x0 * gCosTable[(level >> 4) & 0xFFF] >> 14) + (y0 * gCosTable[((level >> 4) - 0x400) & 0xFFF] >> 14);
        y = (y0 * gCosTable[(level >> 4) & 0xFFF] >> 14) - (x0 * gCosTable[((level >> 4) - 0x400) & 0xFFF] >> 14);
        x = x * gScreenWidth / 320;
        y = y * gScreenHeight / 240;
        x += sGaugeX + sGaugeNeedleX;
        y += sGaugeY + sGaugeNeedleY;
        x -= gScreenWidth / 2;
        y = gScreenHeight / 2 - y;
        sGaugeNeedleVtx[view * 8 + i].v.ob[0] = x;
        sGaugeNeedleVtx[view * 8 + i].v.ob[1] = y;
        sGaugeNeedleVtx[view * 8 + i].v.ob[2] = 0x200;
        sGaugeNeedleVtx[view * 8 + i + 4].v.ob[0] = x + 4;
        sGaugeNeedleVtx[view * 8 + i + 4].v.ob[1] = y - 2;
        sGaugeNeedleVtx[view * 8 + i + 4].v.ob[2] = 0x200;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPVertex(gdl++, &sGaugeNeedleVtx[view * 8], 8, 0);
    gSP2Triangles(gdl++, 7, 4, 5, 0, 4, 5, 6, 0);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0x8C, 0x8C, 0x8C, 0xFF);
    gSP2Triangles(gdl++, 3, 0, 1, 0, 3, 1, 2, 0);
    gSPMatrix(gdl++, dyn, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    return gdl;
}

Gfx *render_draw_image(s32 x, s32 y, Img *img, Gfx *gdl, s32 fb, s32 alignX, s32 alignY, s32 flags, s32 width) {
    s32 rows, row, n, scale, screenRow, screenRows, clip, drawW;
    s32 texScale;
    u8 *addr;

    texScale = 2;
    if (flags & 2) texScale = 1;
    rows = ((flags & 1) ? texScale << 11 : texScale << 12) / (img->w * 8);
    if (img->h < rows) rows = img->h;

    switch (alignX) {
    case 2:
        x = x * gScreenWidth / 512 + gScreenWidth / 2;
        break;
    case 3:
        x = x * gScreenWidth / 512 + gScreenWidth;
        break;
    case 1:
        x = x * gScreenWidth / 512;
        break;
    }
    switch (alignY) {
    case 2:
        y = y * gScreenHeight / 240 + gScreenHeight / 2;
        break;
    case 3:
        y = y * gScreenHeight / 240 + gScreenHeight;
        break;
    case 1:
        y = y * gScreenHeight / 240;
        break;
    }
    if (x >= 0) {
        clip = 0;
    } else {
        if (-x < img->w * 8) {
            clip = -x;
            x = 0;
        } else {
            return gdl;
        }
    }
    for (row = 0; row < img->h; row += rows) {
        n = rows;
        if (img->h - row < rows) n = img->h - row;
        if (flags & 0x100) {
            scale = 2;
            screenRow = row / 2;
            screenRows = n / 2;
        } else {
            scale = 1;
            screenRow = row;
            screenRows = n;
        }
        gDPPipeSync(gdl++);
        if (flags & 2) {
            if (flags & 0x20) addr = img->addr + row * img->w * 8;
            else addr = img->addr + (img->h - row - n) * img->w * 8;
        } else {
            addr = img->addr + (img->h - row - n) * img->w * 4;
        }
        if (flags & 4) {
            if (flags & 2) {
                gDPLoadTextureTile(gdl++, addr, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                                   0, 0, 0, 0, 0, 0, 0);
            } else {
                while (1);
            }
        } else if (flags & 2) {
            gDPLoadTextureTile(gdl++, addr, G_IM_FMT_CI, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                               0, 0, 0, 0, 0, 0, 0);
        } else {
            gDPLoadTextureTile_4b(gdl++, addr, G_IM_FMT_CI, img->w * 8 - clip, 0, clip, 0, img->w * 8 - 1, n - 1,
                                  img->pal & 0xF, 0, 0, 0, 0, 0, 0);
        }
        gDPPipeSync(gdl++);
        if (flags & 0x20) {
            gSPTextureRectangle(gdl++, x * 4, (y + screenRow) * 4, (x + img->w * 8 - clip) * 4, (y + screenRow + screenRows) * 4,
                                0, clip << 5, 0, scale << 10, scale << 10);
        } else if (flags & 0x40) {
            gSPTextureRectangle(gdl++, x * 4, (y + screenRow) * 4, (x + img->w * 8) * 4, (y + screenRow + screenRows) * 4,
                                0, (img->w << 8) - 0x20, (n - 1) << 5, -(scale << 10), -(scale << 10));
        } else {
            if (flags & 0x80) drawW = width;
            else drawW = img->w * 8;
            if (gScreenWidth == 640) {
                gSPTextureRectangle(gdl++, x * 4, (y + screenRow * 2) * 4, x * 4 + (drawW - clip) * 5, (y + screenRow * 2 + screenRows * 2) * 4,
                                    0, clip << 5, (n - 1) << 5, scale * 0x333, -(scale << 9));
            } else {
                gSPTextureRectangle(gdl++, x * 4, (y + screenRow) * 4, (x + drawW / scale - clip) * 4, (y + screenRow + screenRows) * 4,
                                    0, clip << 5, (n - 1) << 5, scale << 10, -(scale << 10));
            }
        }
        gDPPipeSync(gdl++);
    }
    return gdl;
}

typedef struct { s16 v[8]; } Q;

extern Q sInsetViewports[];
extern Q sViewports[];

void render_update_viewports(void)
{
    s32 i;
    s32 j;
    s32 n;
    Q *vps;
    V4 *rect;
    V4 *rects;
    s32 screenW;
    s32 u;

    if (gInsetViewActive != sInsetViewWanted) {
        if (sInsetViewWanted != 0) {
            gNumViews = 2;
            gViewports = sInsetViewports;
            gCamPipOrbitAngle = 24000;
        } else {
            gViewports = sViewports;
            gNumViews = gNumScreens;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                gViewRects_7[i][j].x = gViewLayouts_2[i][j].x * gScreenWidth_2[0] / 512;
                gViewRects_7[i][j].z = gViewLayouts_2[i][j].z * gScreenWidth_2[0] / 512;
                gViewRects_7[i][j].y = gViewLayouts_2[i][j].y * gScreenHeight_3[0] / 240;
                gViewRects_7[i][j].w = gViewLayouts_2[i][j].w * gScreenHeight_3[0] / 240;
            }
        }
        if (sInsetViewWanted != 0) {
            screenW = *gScreenWidth_2;
            D_800E1758[0] = D_800E1758[-4];
            rects = D_800E1758 - 4;
            rects[5].x = screenW / 16;
            rects[5].z = *gScreenWidth_2 * 130 / 512;
            rects[5].w = gScreenHeight_3[0] * 70 / 240;
            if (gViMode == 2) {
                rects[5].y = 100;
            } else {
                rects[5].y = 30;
            }
        }
        vps = gViewports;
        n = gNumViews - 1;
        for (gCurView = 0; gCurView < 4; gCurView++) {
            rect = (V4 *)((u8 *)gViewRects_7 + (gCurView * 16 + n * 64));
            vps[gCurView].v[0] = rect->z * 2;
            vps[gCurView].v[1] = rect->w * 2;
            vps[gCurView].v[4] = (rect->x + rect->z / 2) * 4;
            vps[gCurView].v[5] = (rect->y + rect->w / 2) * 4;
            vps[gCurView + 4].v[0] = rect->z * 2;
            vps[gCurView + 4].v[1] = rect->w * 2;
            vps[gCurView + 4].v[4] = (rect->x + rect->z / 2) * 4;
            vps[gCurView + 4].v[5] = (rect->y + rect->w / 2) * 4 + 2;
            vps[gCurView].v[2] = 0x1FF;
            vps[gCurView].v[6] = 0x1FF;
            vps[gCurView + 4].v[2] = 0x1FF;
            vps[gCurView + 4].v[6] = 0x1FF;
        }
        if (sInsetViewWanted != 0) {
            sInsetViewports[0].v[2] = 0xFF;
            sInsetViewports[0].v[6] = 0x2FF;
            sInsetViewports[4].v[2] = 0xFF;
            sInsetViewports[4].v[6] = 0x2FF;
            sInsetViewports[1].v[2] = 0xFF;
            sInsetViewports[1].v[6] = 0xFF;
            sInsetViewports[5].v[2] = 0xFF;
            sInsetViewports[5].v[6] = 0xFF;
        }
        gInsetViewActive = sInsetViewWanted;
    }
}

typedef struct { s32 x, y, w, h; s32 pad[12]; } View;

extern View gViewRects_8[];

Gfx *render_draw_credits(Gfx *gdl)
{
    View *e;
    s32 i, y, done, header, column;
    s32 c;
    TextObj *font;
    s32 len;
    char buf[64];

    sCreditsScrollY = 0xF0 - (gRaceFrame / 2) % (sCreditsLineCount << 4);
    font_reset_queue();
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects_8[gNumViews - 1].x, gViewRects_8[gNumViews - 1].y, gViewRects_8[gNumViews - 1].x + gViewRects_8[gNumViews - 1].w, gViewRects_8[gNumViews - 1].y + gViewRects_8[gNumViews - 1].h);
    if (gCheatFlags & 0x800) {
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, sTeamPhotoImage_2[1]);
        for (i = 0; i < 0xF0; i += 8) {
            gDPLoadTextureBlock(gdl++, sTeamPhotoImage_2[3] + i * 256, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gdl++, 0, i << 2, 0x800, (i + 8) << 2, G_TX_RENDERTILE, 0, 0, 0x200, 0x400);
        }
    }
    y = sCreditsScrollY;
    i = 0;
    done = 0;
    header = 0;
    if (y < 0x104) {
    len = 0;
    do {
        switch (sCreditsText[i]) {
        case 0xD:
        case '@':
            buf[len] = 0;
            if (y >= -0x13 && len != 0) {
                if (header) {
                    font = sCreditsHeaderFont;
                } else {
                    font = sCreditsFont;
                }
                font_text_clear(font);
                font_text_add(font, buf);
                font_draw_text(font, column * 128 - (s32)font->w / 2, y);
                header = 0;
            }
            len = 0;
            if (sCreditsText[i] == '@') {
                i++;
                c = sCreditsText[i];
                switch (c) {
                case 'E':
                    done = 1;
                    i = -1;
                    break;
                case 'H':
                    header = 1;
                    break;
                case '1':
                case '2':
                case '3':
                    column = c - '0';
                    break;
                }
            } else {
                y += 16;
            }
            i++;
            break;
        case 10:
            i++;
            break;
        default:
            buf[len++] = sCreditsText[i++];
            break;
        }
    } while (!done && y < 0x104);
    }
    return font_render(gdl);
}

void render_make_world_mtx(s32 mtx, s32 rx, s32 ry, s32 rz, s32 x, s32 y, s32 z, s32 scale, s32 view)
{
    s32 *cam;
    if (gCurTrack_3[11] == 0) {
        mtx_from_angles_pos_scale(mtx, rx, ry, rz, x, y, z, scale);
    } else {
        s32 *tb = gCamPos;
        cam = tb + view * 3;
        mtx_from_angles_pos_scale(mtx, rx, ry, rz, x - cam[0], y - cam[1], z - cam[2], scale);
    }
}

#define PKT(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (unsigned int)(b); }

#define LOADUCODE(pkt, s, d) { \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = 0xE1000000; \
    _g->words.w1 = (unsigned int)(d); \
    _g = (Gfx *)(pkt); \
    _g->words.w0 = 0xDD0007FF; \
    _g->words.w1 = (unsigned int)(s); \
}

Gfx *render_load_ucode(Gfx *gdl, s32 fb, s32 dyn, s32 text, s32 data)
{
    PKT(gdl++, 0xE7000000, 0);
    LOADUCODE(gdl++, text, data);
    PKT(gdl++, 0xDB060000, 0);
    PKT(gdl++, 0xDE000000, gRdpInitDL);
    PKT(gdl++, 0xDE000000, gRspInitDL);
    return gdl;
}

void render_reset_viewports(void) {
    gInsetViewActive = 1;
    sInsetViewWanted = 0;
    render_update_viewports();
}

void render_apply_split_layout(void)
{
    Blk *src;
    Blk *dst;
    Blk *end;

    if (gSplitVertical != 0) {
        dst = (Blk *)sViewLayoutsMulti_2;
        src = (Blk *)sLayout2PVertical_2;
        end = src + 4;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end);
    } else {
        dst = (Blk *)sViewLayoutsMulti_2;
        src = (Blk *)sLayout2PHorizontal_2;
        end = src + 4;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end);
    }
    render_reset_viewports();
}

void render_blit_place_glyph(u8 *src, s32 x, s32 y, s32 w, s32 h)
{
    s32 i, j, px;

    i = 0;
    do {
        if (h > 0) {
            do {
                if (y < 0) goto next;
                if (y >= 16) goto next;
                do { px = x; } while (0);
                for (j = 0; j < w; j++, px++) {
                    if (px < 0) continue;
                    if (px >= 32) continue;
                    sPlaceTexDst[y * 32 + px] = src[i * 64 + j];
                }
                do { next: i++; y++; } while (0);
            } while (i < h);
        }
    } while (0);
}

void render_lerp_rgb(u8 *a, u8 *b, u8 *out, s32 t, s32 count)
{
    s32 total = count * 3;
    s32 i = 0;
    u8 *srcA = a;
    u8 *srcB = b;
    u8 *dst = out;
    if (total > 0) {
        s32 inv = 0xFF - t;
        s32 lim = total;
        do {
            *dst = ((s32)*srcA * inv + (s32)*srcB * t) >> 8;
            srcA++;
            i++;
            srcB++;
            dst++;
        } while (i < lim);
    }
}

void render_lerp_palette(u16 *palA, u16 *palB, u16 *out, s32 t, s32 count)
{
    s32 i;
    u32 colA, colB;
    s32 r, g, b;

    for (i = 0; i < count; i++) {
        colA = *palA++;
        colB = *palB++;
        r = (s32)((colA >> 11) * (0xFF - t) + (colB >> 11) * t) >> 8;
        g = (s32)(((colA >> 6) & 0x1F) * (0xFF - t) + ((colB >> 6) & 0x1F) * t) >> 8;
        b = (s32)(((colA >> 1) & 0x1F) * (0xFF - t) + ((colB >> 1) & 0x1F) * t) >> 8;
        *out++ = (r << 11) | (g << 6) | (b << 1);
    }
}
