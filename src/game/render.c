#include "game.h"

#define G(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u16 c[256]; } Pal_3;
typedef struct { s32 pad0; Pal_3 *pal; } Img_3;
typedef struct { void *a[2]; void *b[2]; } Rec16;

typedef struct {
    u8 pad0[0xC];
    u8 r, g, b, a;
} Col16;

typedef struct {
    u32 count;
    Col16 *items;
} ColList;

typedef struct { u32 w0; u32 img; } TexHdr;

struct Dyn {
    Mtx m0[4];  /* 0x000 */
    Mtx m1[4];  /* 0x100 */
    Mtx m2[4];  /* 0x200 */
    Mtx m3[4];  /* 0x300 */
    Mtx m4;     /* 0x400 */
    Gfx *glist; /* 0x440 */
};

extern Rec16 sRiderModelCache[];
extern Img_3 *D_800E1600[][6];
extern Pal_3 *D_800E1880[];
extern ColList *sSkyModel;

typedef struct { s32 *f0, *f4, *f8, *fC; u8 pad[0x18]; } Ent28;

typedef struct {
    void *f0;
    void *f4;
    Img_3 *f8;
    Pal_3 *fC;
    Pal_3 *f10;
    Pal_3 *f14;
    Pal_3 *f18;
    u16 pal[256];
    Rgb col;
    u8 pad21F;
    void *f220;
    void *a224[2];
    void *a22C[2];
    void *a234[2];
    void *b23C[2];
    u8 rest[0xA80 - 0x244];
} Plr;

extern Ent28 sRiderModelTable[];
extern Plr gRiderGfx_2[];

void render_load_rider(ResFile *r, s32 n, s32 **ids, s32 arg3, s32 arg4, s32 arg5)
{
    s32 buf[2];
    void *tmp;
    Ent28 *e;
    s32 c;
    s32 i;
    s32 k;
    s32 id;
    u32 *m;
    Rgb *col;
    void *x;
    s32 len;

    e = &sRiderModelTable[arg5];
    if (gPodiumMode == 0) {
        if (arg5 == 0) {
            ponytail_init(n);
        }
        gRiderGfx_2[n].f8 = res_load(r, *D_800C23C8[arg3]);
    }
    if (gGame.flags & 0x10) {
        gRiderGfx_2[n].f18 = mem_alloc_dbg(0xC00, 0, D_800D5EBC, 0x1FF);
    } else {
        gRiderGfx_2[n].f14 = mem_alloc_dbg(0x200, 0, D_800D5EBC, 0x203);
    }
    if (!(gGame.flags & 0x10)) {
        c = gGame.riderSkins[n];
    } else {
        c = gGame.riders[n & 3] & 3;
    }
    col = &gRiderGfx_2[n].col;
    mem_fill((u8 *)col, 0xFF, 3);
    k = res_get_size(r, *ids[c]);
    if (gGame.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            D_800E1600[n][i] = mem_alloc_dbg(k, 0, D_800D5EBC, 0x220);
            tmp = res_load(r, *ids[i]);
            prim_pkg_unrelocate((u32 *)&tmp);
            memcpy(D_800E1600[n][i], tmp, k);
            res_unload(&r->tbl[*ids[i]]);
            prim_pkg_relocate((u32 *)&D_800E1600[n][i]);
        }
        gRiderGfx_2[n].f0 = D_800E1600[n][0];
        gRiderGfx_2[n].fC = 0;
    } else if (n >= 6) {
        gRiderGfx_2[n].f0 = res_load(r, D_800D0C88);
        D_800E1A80[0] = ((Img_3 *)res_load(r, D_800D0C88))->pal;
        D_800E1A80[1] = ((Img_3 *)res_load(r, D_800D06E8))->pal;
        D_800E1A80[2] = ((Img_3 *)res_load(r, D_800D06FC))->pal;
        gRiderGfx_2[n].fC = D_800E1A80[n % 3];
    } else {
        tmp = res_load(r, *ids[c]);
        gRiderGfx_2[n].f0 = tmp;
        gRiderGfx_2[n].fC = ((Img_3 *)tmp)->pal;
        *col = sRiderColors[c];
    }
    if (gPodiumMode != 0) {
        if (n < 6) {
            gRiderGfx_2[n].f4 = res_load(r, *e->f4);
        } else {
            gRiderGfx_2[n].f4 = res_load(r, *sRiderModelTable[0].f4);
        }
    }
    if (gGame.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            gRiderGfx_2[n].f18[i] = *D_800E1600[n][i]->pal;
        }
    } else {
        *gRiderGfx_2[n].f14 = *gRiderGfx_2[n].fC;
    }
    if (gPodiumMode == 0) {
        if (arg4 == 0) {
            gRiderGfx_2[n].f10 = gRiderGfx_2[n].f8->pal;
        } else {
            gRiderGfx_2[n].f10 = D_800E1880[arg3 * 3 + (arg4 - 1)];
        }
        __builtin_memcpy(gRiderGfx_2[n].pal, gRiderGfx_2[n].f10, sizeof(Pal_3));
        gRiderGfx_2[n].pal[0] &= 0xFFFE;
        gRiderGfx_2[n].f10->c[0] &= 0xFFFE;
        gRiderGfx_2[n].f8->pal->c[0] &= 0xFFFE;
    }
    if (sRiderModelCache[arg5].a[0] == 0) {
        for (k = 0; k < 2; k++) {
            if (gPodiumMode != 0) {
                if (k == 0) {
                    continue;
                }
                id = *e->f0;
            } else if (k != 0) {
                id = *e->f8;
            } else {
                id = *e->fC;
            }
            res_set_load_buffer((s32)(gZBuffer + 0x1E000));
            m = (u32 *)((u32)res_load(r, id) & ~3);
            skel_make_verts_bone_local(gRiderSkelTemplate, m);
            buf[0] = (s32)gRiderGfx_2[n].f0;
            gPrimBoneMtx = 0x06000000;
            buf[1] = (s32)gRiderGfx_2[n].f4;
            if (gPodiumMode != 0) {
                prim_alloc_seg_dl(&sRiderModelCache[arg5].a[k], m, (s32)buf, 0xC);
                palmod_register((s32)gRiderGfx_2[n].f4, 0);
                palmod_register((s32)gRiderGfx_2[n].f0, 0);
            } else {
                prim_alloc_seg_dl(&sRiderModelCache[arg5].a[k], m, (s32)buf, 0);
            }
            x = mem_alloc_dbg(m[0] << 4, 0, D_800D5EBC, 0x294);
            sRiderModelCache[arg5].b[k] = x;
            memcpy(x, (void *)m[1], m[0] << 4);
            if (k == 0 && n == 0) {
                m[1] = 0x01000000;
                gPrimBoneMtx = 0x06000000;
                len = (u32)prim_build_dl(0x23, m, 0, gZBuffer) - (u32)gZBuffer;
                x = mem_alloc_dbg(len, 0, D_800D5EBC, 0x2A0);
                gRiderGfx_2[0].f220 = x;
                memcpy(x, gZBuffer, len);
            }
            res_unload(&r->tbl[id]);
            res_set_load_buffer(0);
        }
    }
    gRiderGfx_2[n].a22C[0] = sRiderModelCache[arg5].b[0];
    gRiderGfx_2[n].a22C[1] = sRiderModelCache[arg5].b[1];
    gRiderGfx_2[n].a224[0] = sRiderModelCache[arg5].a[0];
    gRiderGfx_2[n].a224[1] = sRiderModelCache[arg5].a[1];
    for (k = 0; k < 2; k++) {
        gRiderGfx_2[n].a234[k] = mem_alloc_dbg(0x3C0, 0, D_800D5EBC, 0x2B3);
        if (gPodiumMode == 0) {
            gRiderGfx_2[n].b23C[k] = mem_alloc_dbg(0x280, 0, D_800D5EBC, 0x2B6);
        }
    }
}

void render_update_screen_layout(void)
{
    s32 n;

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
    n = gCurTrack_2 - gTracks;
    if (n == 8 || n == 0xB) {
        n = 4;
    } else {
        n = 3;
    }
    if (gNumScreens >= n) {
        gSkyMode = 0;
        gSkyImage = 0;
    }
}

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x20];
    s32 f50;
    u8 c54, c55, c56;
    u8 pad57[0xA0 - 0x57];
} Track_3;

typedef struct {
    u8 pad0[0x21C];
    u8 rgb[3];
    u8 pad21F[0xA60 - 0x21F];
    u16 pal[16];
} PalEnt;

typedef struct {
    u8 pad0[0x10];
    u8 f10[0x30];
    u8 f40[0x40];
} Ent80;

typedef struct {
    u8 pad0[0x28];
} Ent28_2;

typedef struct {
    void *p[2];
} Pair_5;

typedef struct {
    s32 f0;
    u16 *f4;
    s32 f8;
    u8 *fC;
} Img_4;

typedef struct {
    s32 f0;
    u16 *f4;
} Res;

extern Track_3 *gCurTrack_9;
extern Track_3 gTracks_4[];
extern Ent28_2 D_800C26C8[];
extern Res *sMarkerImage;
extern Img_4 *gSkyImage_2;
extern PalEnt gRiderGfx_3[8];
extern Pair_5 D_800E1900[6];
extern Ent80 *gRiderSkelTemplate_2;

void render_init(void) {
    u8 buf[0x60];
    s32 a, b, c;
    s32 i, j, k, n, size;
    void *p;
    u32 r, g, bl, m;
    s32 idx;
    s32 x, y, z;

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
        sCreditsText = res_load(D_800E7728, D_800D098C);
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
        sTeamPhotoImage = res_load(D_800E7728, D_800D0950);
        prim_flip_images(sTeamPhotoImage);
    }
    render_update_screen_layout();
    sMarkerImage = res_load(D_800E7728, D_800D0D04);
    prim_flip_images(sMarkerImage);
    *sMarkerImage->f4 = 0;
    gFontDirectDrawFunc = render_blit_place_glyph;
    sPlaceTextures = mem_alloc_dbg(0xC00, 0, D_800D5EBC, 0x349);
    mem_fill(sPlaceTextures, 0, 0xC00);
    for (i = 0; i < 6; i++) {
        sPlaceTexDst = sPlaceTextures + i * 0x200;
        font_text_create(buf, 3, 1, 1, 0x08000000);
        font_text_add(buf, sPlaceStrings[i]);
        font_draw_text(buf, 0, 0);
    }
    gFontDirectDrawFunc = NULL;
    gLightningFlash = 0;
    if (gSplitVertical != 0) {
        sViewLayoutsMulti = sLayout2PVertical;
    } else {
        sViewLayoutsMulti = sLayout2PHorizontal;
    }
    render_reset_viewports();
    sGaugeNeedleVtx = mem_alloc_dbg(0x100, 0, D_800D5EBC, 0x359);
    sPonytailMesh = 0;
    if (gMirrorMode != 0) {
        gCullMode = 0x200;
    } else {
        gCullMode = 0x400;
    }
    D_800DC0B4 = 8;
    sRiderColors = res_load(D_800E7728, D_800D0848);
    if (gCurTrack_9->f2C != 2) {
        if (gReplayPlayback != 0 || (gGame.flags & 0x80) || gGhostRace != 0) {
            gNumViews = 1;
        } else {
            gNumViews = gGame.numPlayers;
        }
    }
    if (gCurTrack_9->f2C == 2) {
        gNumViews = 1;
    }
    sClearFrames = 5;
    gNumScreens = gNumViews;
    mem_fill(gRiderSection, 0, 0x20);
    prim_init();
    if (gCurTrack_9->f2C == 0) {
        arn_init();
    } else if (gCurTrack_9->f2C == 1) {
        tube_init();
    } else if (gCurTrack_9->f2C == 2) {
        des_init();
    }
    sGaugeTextures = res_load(D_800E7728, D_800D10B8);
    prim_flip_images(sGaugeTextures);
    sGaugePalettes = mem_alloc_dbg(0x400, 0, D_800D5EBC, 0x38B);
    mem_fill(sGaugePalettes, 0, 0x400);
    switch (gGame.mode) {
    case 2:
        soccer_ball_load_model();
        break;
    case 0:
    case 13:
    case 14:
        if (!(gCurTrack_9->f50 & 2)) {
            break;
        }
    case 9:
        finishline_load_models();
        break;
    }
    p = skel_load(D_800E7728, D_800D0DA4);
    gRiderSkelTemplate_2 = p;
    for (i = 0; i < 15; i++) {
        mtx3i_from_angles(gRiderSkelTemplate_2[i].f10, 0, 0, 0);
        mtx3i_from_angles(gRiderSkelTemplate_2[i].f40, 0, 0, 0);
    }
    skel_update_world_transforms(gRiderSkelTemplate_2, 15);
    if (gCurTrack_9->f2C == 0) {
        j = i;
    } else if (gCurTrack_9->f2C == 1) {
        j = n;
    }
    mem_fill(sRiderModelCache, 0, 0x70);
    mem_fill((u8 *)gRiderGfx_3, 0, 0x5400);
    n = 8;
    if (gGame.numPlayers >= 2 && !(gGame.flags & 0x10)) {
        n = 4;
    }
    if ((gGame.flags & 1) && gEvent.numRiders == 1) {
        n = 2;
    }
    if (gPodiumMode != 0) {
        n = gEvent.numRiders;
    }
    if (gCreditsMode != 0) {
        n = 1;
    }
    for (i = 0; i < 6; i++) {
        D_800E1880[i] = (s32)((Res *)res_load(D_800E7728, *D_800C23B0[i]))->f4;
    }
    for (j = 0; j < n; j++) {
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
        render_load_rider(D_800E7728, j, &D_800C26C8[i], gGame.bikeSkins[j % 6] & 1, gGame.bikeSkins[j % 6] >> 1, i);
    }
    if (!(gGame.flags & 0x81)) {
        event_hud_init();
    }
    for (i = 0; i < 8; i++) {
        gRiderGfx_3[i].pal[0] = 0;
        for (j = 1; j < 8; j++) {
            x = gRiderGfx_3[i].rgb[0] * j / 7 >> 3;
            y = gRiderGfx_3[i].rgb[1] * j / 7 >> 3;
            z = gRiderGfx_3[i].rgb[2] * j / 7 >> 3;
            gRiderGfx_3[i].pal[j] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (j = 0; j < 8; j++) {
            x = gRiderGfx_3[i].rgb[0];
            y = gRiderGfx_3[i].rgb[1];
            z = gRiderGfx_3[i].rgb[2];
            x = (x + (255 - x) * j / 7) >> 3;
            y = (y + (255 - y) * j / 7) >> 3;
            z = (z + (255 - z) * j / 7) >> 3;
            gRiderGfx_3[i].pal[j + 8] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
    }
    mem_fill((u8 *)D_800E1900, 0, 0x30);
    mem_fill((u8 *)D_800DC110, 0, 0x30);
    mem_fill((u8 *)D_800E16C0, 0, 0x18);
    D_800E1B48 = 0;
    gBikeSkelTemplate = skel_load(D_800E7728, D_800D0920);
    skel_reset_pose(gBikeSkelTemplate, 10);
    if (gPodiumMode == 0) {
        for (i = 0; i < 6; i++) {
            D_800E1900[i].p[1] = res_load(D_800E7728, *D_800C259C[i]);
            D_800E1900[i].p[0] = res_load(D_800E7728, *D_800C259C[i + 6]);
        }
        D_800E155C = res_load(D_800E7728, D_800D1130);
        prim_alloc_dl(&D_800E1B48, D_800E155C, D_800DC158, 0x20);
        for (i = 0; i < 6; i++) {
            skel_make_verts_bone_local(gBikeSkelTemplate + D_800C25CC[i] * 0x80, D_800E1900[i].p[1]);
            skel_make_verts_bone_local(gBikeSkelTemplate + D_800C25CC[i] * 0x80, D_800E1900[i].p[0]);
        }
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 2; j++) {
                prim_alloc_seg_dl(&D_800DC110[i][j], D_800E1900[i].p[j], D_800DC158, 0);
            }
            size = prim_build_dl_2(3, D_800E1900[i].p[1], 0, gZBuffer) - gZBuffer;
            D_800E16C0[i] = mem_alloc_dbg(size, 0, D_800D5EBC, 0x434);
            memcpy(D_800E16C0[i], gZBuffer, size);
        }
    }
    sSkyModel = (ColList *)track_res_get(D_800D152C);
    gSkyImage_2 = (Img_4 *)track_res_get(D_800D150C);
    if (sSkyModel != NULL) {
        if (gSkyImage_2 != NULL) {
            gSkyMode = 3;
        } else {
            gSkyMode = 2;
        }
    } else if (gSkyImage_2 != NULL) {
        gSkyMode = 1;
        prim_flip_image_rows(gSkyImage_2->fC, 0x100, 0x100);
    } else {
        gSkyMode = 0;
    }
    if (gSkyImage_2 != NULL) {
        palmod_register(gSkyImage_2, 0);
    }
    sun_init();
    func_80035630();
    if (gSkyImage_2 != NULL) {
        gFogColor[0] = (*gSkyImage_2->f4 >> 11) << 3;
        gFogColor[1] = ((*gSkyImage_2->f4 >> 6) & 0x1F) << 3;
        gFogColor[2] = ((*gSkyImage_2->f4 >> 1) & 0x1F) << 3;
        if (gCreditsMode != 0) {
            a = (gFogColor[0] + gFogColor[1] + gFogColor[2]) / 3;
            palmod_sepia(a, &a, &b, &c);
            gFogColor[0] = a;
            gFogColor[1] = b;
            gFogColor[2] = c;
        }
    } else if (sSkyModel != NULL) {
        r = 0;
        g = 0;
        bl = 0;
        for (i = 0; i < sSkyModel->count; i++) {
            r += sSkyModel->items[i].r;
            g += sSkyModel->items[i].g;
            bl += sSkyModel->items[i].b;
        }
        r /= sSkyModel->count;
        g /= sSkyModel->count;
        bl /= sSkyModel->count;
        gFogColor[0] = r;
        gFogColor[1] = g;
        gFogColor[2] = bl;
    } else if (gCurTrack_9->f2C == 1) {
        gFogColor[0] = gCurTrack_9->c54;
        gFogColor[1] = gCurTrack_9->c55;
        gFogColor[2] = gCurTrack_9->c56;
    } else {
        gFogColor[0] = 0;
        gFogColor[1] = 0;
        gFogColor[2] = 0;
    }
    idx = gCurTrack_9 - gTracks_4;
    if (idx == 3 || idx == 20) {
        gSkyImage_2 = NULL;
        gSkyMode = 0;
    }
    if (gCheatFlags & 0x40) {
        gSkyImage_2 = NULL;
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

typedef struct { u8 pad0[0x14]; s32 f14; s32 f18; u8 pad1c[0x234 - 0x1C]; s32 a[2]; s32 b[2]; u8 rest[0xA80 - 0x244]; } Plr_2;
typedef struct { u8 pad[0x2C]; s32 m; u8 pad30[0x20]; u32 f50; } Cfg_4;

extern Cfg_4 *gCurTrack_10;
extern Plr_2 gRiderGfx_4[];

void render_free(void)
{
    s32 i, k, n;

    if (gCreditsMode != 0) {
        func_800C204C(sCreditsFont);
        func_800C204C(sCreditsHeaderFont);
        mem_free(sCreditsFont);
        mem_free(sCreditsHeaderFont);
    }
    mem_free(sPlaceTextures);
    mem_free(sGaugeNeedleVtx);
    if (gCurTrack_10->m == 0) {
        arn_free();
    } else if (gCurTrack_10->m == 1) {
        tube_free();
    } else if (gCurTrack_10->m == 2) {
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
        if ((gCurTrack_10->f50 & 2) == 0) {
            break;
        }
    case 9:
        finishline_free_models();
        break;
    }
    prim_exit();
    for (n = 0; n < 8; n++) {
        if (gGame.flags & 0x10) {
            if (n >= 6) {
                break;
            }
            mem_free(gRiderGfx_4[n].f18);
            for (i = 0; i < 6; i++) {
                mem_free(D_800E1600[n][i]);
            }
        } else if (gRiderGfx_4[n].f14 != 0) {
            mem_free(gRiderGfx_4[n].f14);
        }
        ponytail_free(n);
        for (i = 0; i < 2; i++) {
            mem_free(gRiderGfx_4[n].a[i]);
            mem_free(gRiderGfx_4[n].b[i]);
        }
    }
    mem_free(D_800DC370);
    for (i = 0; i < 7; i++) {
        for (k = 0; k < 2; k++) {
            if (sRiderModelCache[i].a[k] != 0) {
                mem_free(sRiderModelCache[i].a[k]);
            }
            if (sRiderModelCache[i].b[k] != 0) {
                mem_free(sRiderModelCache[i].b[k]);
            }
        }
    }
    for (i = 0; i < 6; i++) {
        mem_free(D_800DC110_2[i].y);
        mem_free(D_800DC110_2[i].x);
        mem_free(D_800E16C0[i]);
    }
    mem_free(D_800E1B48);
    cam_free();
    sun_free();
}

Gfx *render_set_viewport(Gfx *dl, Mtx *m, s32 n, s32 flag)
{
    Rect *r;
    s32 i;
    u8 *tab;

    if (n == sCurViewport) {
        return dl;
    }
    tab = (u8 *)gViewRects;
    r = (Rect *)(tab + (n * 16 + (gNumViews - 1) * 64));
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, r->x, r->y, r->x + r->w, r->y + r->h);
    if (flag) {
        gSPMatrix(dl++, &m[n], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    gSPViewport(dl++, &gViewports[n]);
    sCurViewport = n;
    return dl;
}

typedef struct { u8 pad0[0x5AA]; s16 sta; u8 pad1[0x7E0 - 0x5AC]; } E37;
typedef struct { u8 pad0[0x2C]; s32 mode; u8 pad1[0xA0 - 0x30]; } Trk_4;
typedef struct { u32 bits; u8 pad[0x1C]; } Cell_2;

extern Cell_2 *gTrackSections_4;
extern E37 gRiders_4[];
extern Trk_4 *gCurTrack_11;
extern Trk_4 gTracks_5[];

s32 render_get_rider_lod(s32 x, s32 y, s32 z, s32 k, s32 c, s32 q, s32 n)
{
    s32 sx, sy, sz, d;
    s32 i3, i4, m;
    s32 x0, x1, y0, y1;
    Rect *b;
    u8 *bb;

    if (gInsetViewActive != 0 && n == 0) {
        return 3;
    }
    if (!(gRiders_4[n].sta == 9 && gCamTargets[k] == n)) {
        if ((u32)gCurTrack_11->mode < 2 && c != -1) {
            Cell_2 *cp = &gTrackSections_4[c];
            if (!((cp[1].bits >> (k * 2)) & 1)) {
                return 0;
            }
        }
    }
    if (gCurTrack_11->mode == 0) {
        if (project_to_screen(x >> 4, y >> 4, z >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    } else {
        if (project_to_screen((x - gCamPos_3[k][0]) >> 4, (y - gCamPos_3[k][1]) >> 4,
                          (z - gCamPos_3[k][2]) >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    }
    if (d <= 0) {
        return 0;
    }
    if (gCurTrack_11->mode == 2 && d > 8000) {
        return 0;
    }
    sRiderCamDist[n] = d;
    bb = (u8 *)gViewRects;
    b = (Rect *)(bb + (k * 16 + (gNumViews - 1) * 64));
    x0 = b->x;
    x1 = b->w + x0;
    y0 = b->y;
    y1 = b->h + y0;
    i3 = (sProjScaleX[k] * 90.0f) / d;
    i4 = (sProjScaleY[k] * 90.0f) / d;
    if (sx + i3 < x0) {
        return 0;
    }
    if (x1 < sx - i3) {
        return 0;
    }
    if (sy + i4 < y0) {
        return 0;
    }
    if (y1 < sy - i4) {
        return 0;
    }
    if (gCurTrack_11 - gTracks_5 == 20 && D_800E3D6A == 4) {
        return 3;
    }
    m = gLoDetailThresh[gNumScreens - 1];
    if (m * 6 / 10 < d) {
        return 1;
    }
    if (m * 4 / 10 < d) {
        return 2;
    }
    return 3;
}

Gfx *render_begin_menu_frame(Gfx *gdl, s32 fb, u8 *mtx) {
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
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

typedef struct {
    s16 cnt;
    s16 pad;
    TexHdr *tex;
    u8 *bar;
    TexHdr *tex2;
    TexHdr *anim;
    TexHdr *tex3;
} EbState;

typedef struct {
    u8 pad0[0x2C4];
    u8 flag;
} Opts;

extern EbState D_800E3730_2;
extern Opts gSaveData_2;

Gfx *render_draw_sound_menu(Gfx *gdl, s32 fb, Dyn *arg2)
{
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    TexHdr *t;
    s32 xi;
    u8 *p;

    if (D_800E3730_2.cnt > 0) {
        D_800E3730_2.cnt--;
        gdl = render_begin_menu_frame_2(gdl, fb);
    } else if (D_800E3730_2.cnt == -1) {
        gdl = render_begin_menu_frame_2(gdl, fb);
        G(gdl++, 0xE200001C, 0x0C084000);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, D_800E3730_2.tex->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = render_draw_image(0, 0, D_800E3730_2.tex + 1, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xFA000000, 0xE6E6E6FF);
        G(gdl++, 0xE3000C00, 0);
        G(gdl++, 0xE200001C, 0x00404240);
        font_reset_queue();
        t = D_800E3730_2.tex2 + 1;
        G(gdl++, 0xFD100000, D_800E3730_2.tex2->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = render_draw_image(0x45, 0x3D, t, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xE3001001, 0);
        G(gdl++, 0xD7000002, 0x80008000);
        G(gdl++, 0xE200001C, 0x00404240);
        G(gdl++, 0xFC119623, 0xFF2FFFFF);
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 29; i++) {
                xi = i * 6 + 282;
                x = xi - j * 7;
                y = D_800C27B8[j] - 1;
                if (i <= D_800F1BC5[j] * 29 / 255) {
                    if (j == gOptionsCursor) {
                        G(gdl++, 0xFA000000, (((i * 255 / 29) & 0xFF) << 16) | 0xFF0000FF);
                    } else {
                        G(gdl++, 0xFA000000, (((i * 140 / 29) & 0xFF) << 16) | 0x8C0000FF);
                    }
                } else {
                    G(gdl++, 0xFA000000, 0x4B4B4BFF);
                }
                gdl = render_draw_image(x, y, D_800E3730_2.bar + 0x28, gdl, fb, 1, 1, 6, 0);
            }
        }
        G(gdl++, 0xFA000000, 0xD2D2D2FF);
        t = (TexHdr *)(D_800E3730_2.bar + 0x30);
        gdl = render_draw_image(0x100 - ((u8 *)t)[0] * 4, 0x78 - (((u8 *)t)[1] >> 1), t, gdl, fb, 1, 1, 6, ((u32)t & 1) >> 1);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, &D_800E3730_2.tex3->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        t = D_800E3730_2.anim + ((gGame.menuAnimTimer / 2) % 30 + 1);
        for (j = 0; j < 4; j++) {
            if (j < 3) {
                x = 0x48 - j * 7;
                y = D_800C27B8[j];
            } else {
                xi = ((j - 3) & 1) * 194 + 0x48;
                x = xi - ((j - 3) / 2 + 3) * 7;
                y = D_800C27B8[j];
            }
            font_text_create(gGame.texts[0], 0x40, 1, (j == gOptionsCursor) ? 4 : 2, 0x20);
            font_text_add(gGame.texts[0], gOptionsLabels[j]);
            font_draw_text(gGame.texts[0], x + 0x24, y + 8);
            if (j == gOptionsCursor) {
                gdl = render_draw_image(x - 2, y, t, gdl, fb, 1, 1, 3, 0);
            }
            font_text_create(gGame.texts[0], 0x40, 1, (j == gOptionsCursor) ? 6 : 2, 0x22);
            if (j == 3) {
                font_text_add(gGame.texts[0], gMonoStereoStrings[gSaveData_2.flag & 1]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
            }
        }
        gdl = font_render(gdl);
    }
    return gdl;
}

typedef struct { s16 cnt; s16 pad; TexHdr *tex; } EbState_2;

extern EbState_2 D_800E3730_3;

Gfx *render_draw_controls_menu(Gfx *gdl, s32 fb, u8 *mtx)
{
    char buf[32];
    s32 i;
    s32 a3v;
    s32 v;
    s32 sel;
    s32 lo;

    if (D_800E3730_3.cnt > 0) {
        D_800E3730_3.cnt--;
        gdl = render_begin_menu_frame(gdl, fb, mtx);
    } else if (D_800E3730_3.cnt == -1) {
    gdl = render_begin_menu_frame(gdl, fb, mtx);
    G(gdl++, 0xE200001C, 0x00404240);
    G(gdl++, 0xE3001001, 0x00008000);
    G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    G(gdl++, 0xFD100000, D_800E3730_3.tex->img);
    G(gdl++, 0xE8000000, 0);
    G(gdl++, 0xF5000100, 0x07000000);
    G(gdl++, 0xE6000000, 0);
    G(gdl++, 0xF0000000, 0x073FC000);
    G(gdl++, 0xE7000000, 0);
    G(gdl++, 0xE7000000, 0);
    gdl = render_draw_image(gViewRects_6[0], gViewRects_6[1], D_800E3730_3.tex + 1, gdl, fb, 1, 1, 3, 0);
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
        sel = i & 1;
        a3v = sel ? 4 : 6;
        lo = i < 5;
        v = lo ? 0x22 : 0x20;
        if (!sel) {
            v |= 0x40000000;
        }
        font_text_create(gGame.texts[0], 0x40, 5, a3v, v);
        font_text_add(gGame.texts[0], gControlActionLabels[gControlConfigs[(s8)gGame.padConfig[gGame.playerPad[gPausePlayer]]][i]]);
        font_draw_text(gGame.texts[0], gControlLabelPos[i].x + 4, gControlLabelPos[i].y + 5);
        if (gControlConfigs[(s8)gGame.padConfig[gGame.playerPad[gPausePlayer]]][i] == 6) {
            a3v = sel ? 4 : 6;
            v = lo ? 0x22 : 0x20;
            if (!sel) {
                v |= 0x40000000;
            }
            font_text_create(gGame.texts[0], 0x40, 5, a3v, v);
            font_text_add(gGame.texts[0], D_800CDAC8);
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
    s32 n;
    s32 m;
    s32 lim;

    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    n = gNumScreens;
    gDPSetFillColor(gdl++, (GPACK_RGBA5551(gFogColor[0], gFogColor[1], gFogColor[2], 1) << 16) |
                               GPACK_RGBA5551(gFogColor[0], gFogColor[1], gFogColor[2], 1));
    i = 0;
    m = n - 1;
    if (n > 0) {

    do {
        gDPFillRectangle(gdl++, gViewRects[m][i].x, gViewRects[m][i].y,
                         gViewRects[m][i].x + gViewRects[m][i].w - 1,
                         gViewRects[m][i].y + gViewRects[m][i].h - 1);
    i++;
    } while (i < gNumScreens);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define GFX_LEFT() (0x1770 - (gdl - dyn->glist))

#define CHECK(n)                 \
    if (GFX_LEFT() < (n)) {      \
        *pgdl = gdl;             \
        return;                  \
    }

typedef struct {
    u8 pad0[0x34];
    s32 v34[3];
    Mtx m40;
} Bone; /* 0x80 */

typedef struct {
    Bone b[10];
} Skel; /* 0x500 */

typedef struct {
    s32 x, y, z;       /* 0x00 */
    s32 x2, y2, z2;    /* 0x0C */
    u8 pad18[0x2C];
    u8 b44;            /* 0x44 */
    u8 pad45[3];
    f32 f48, f4C, f50; /* 0x48 */
    u8 pad54[0x556];
    s16 h5AA;          /* 0x5AA */
    u8 pad5AC[0xAC];
    Skel *skel;        /* 0x658 */
    s32 p65C;          /* 0x65C */
    u8 pad660[0x88];
    u8 b6E8;           /* 0x6E8 */
    u8 pad6E9[0xBC];
    u8 b7A5;           /* 0x7A5 */
    u8 pad7A6[0x3A];
} Player_2; /* 0x7E0 */

typedef struct {
    u8 pad0[0x234];
    Mtx *p234[2]; /* 0x234 */
    Mtx *p23C[2]; /* 0x23C */
    s32 pad244;
    Mtx a[8];     /* 0x248 */
    Mtx b[8];     /* 0x448 */
    Mtx c[8];     /* 0x648 */
    Mtx d[8];     /* 0x848 */
    u8 padA48[0x38];
} PG; /* 0xA80 */

typedef struct {
    u8 pad0[0x2C];
    s32 type;  /* 0x2C */
    u8 pad30[0x20];
    s32 f50;   /* 0x50 */
    u8 pad54[0x4C];
} Track_4; /* 0xA0 */

typedef struct {
    s16 s0;
    u8 c2;
} Fade;

extern Track_4 *gCurTrack_12;
extern Track_4 gTracks_6[];
extern Player_2 gRiders_5[8];
extern Fade D_800E3730_4;
extern Skel *gBikeSkelTemplate_2;
extern PG gRiderGfx_5[8];

void render_draw_race(Dyn *dyn, s32 fb, Gfx **pgdl) {
    Mtx mf;
    Mtx rotB;
    Mtx rotA;
    Mtx out;
    u8 unused[0x70];
    Mtx persp;
    Gfx *gdl;
    s32 i;
    s32 k;
    s32 a;
    s32 b;
    s32 n;
    s32 alpha;
    s32 r;
    s32 ang;
    s32 t;
    Mtx *pp;

    sCurViewport = -1;
    sRidersDrawn = 0;
    if (gCheatFlags & 0x100) {
        for (i = 0; i < 8; i++) {
            gRiderAlpha[i] = 0x80;
        }
    }
    if (gGame.numPlayers == 1 && D_800E3BBE != 0 && !(gGame.flags & 0x81) && gReplayPlayback == 0 &&
        gGame.mode != 6 && gGame.mode != 7 && gGame.mode != 8 && gGame.mode != 9 &&
        gGame.mode != 13 && gGame.mode != 14 && gCurTrack_12->type != 2) {
        sInsetViewWanted = gGame.numPlayers;
    } else {
        sInsetViewWanted = 0;
    }
    render_update_viewports();
    if (D_800DC0B4 <= 0) {
        D_800DC0B4 = 1;
    }
    if (D_800DC0B4 > 8) {
        D_800DC0B4 = 8;
    }
    D_800DC0AC = 0;
    sRenderFrame++;
    palmod_update();

    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
        if (gCurTrack_12->type == 0) {
            arn_update_view(gCurView);
        } else if (gCurTrack_12->type == 1) {
            tube_update_view(gCurView);
        } else if (gCurTrack_12->type == 2) {
            des_update_view(gCurView);
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders_5[i].b7A5 == 0 && ((i + sRenderFrame) & 3) == 0 && gRiders_5[i].h5AA != 9) {
            gGroundFound = 0;
            height_get_ground(gRiders_5[i].x, gRiders_5[i].y, gRiders_5[i].z);
            if (gGroundFound != 0) {
                gRiderSection[i] = gGroundArea;
                D_800E1B50[i] = gGroundSurface;
                if (gCurTrack_12->type != 2) {
                    height_get_ground_color(gRiders_5[i].x, gRiders_5[i].z, &sRiderTint[i * 3]);
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
            pp = &persp;
            mtx_mul(&gViewAxisMtx, pp, &D_800E1938[gGfxFrameIndex]);
            t = ((s16 *)pp)[0];
            t <<= 16;
            t |= ((u16 *)pp)[16];
            sProjScaleX[gCurView] = t * (1.0f / 65536);
            sProjScaleX[gCurView] = sProjScaleX[gCurView] * (gViewRects[gNumViews - 1][gCurView].w * 0.5f);
            t = ((s16 *)pp)[5];
            t <<= 16;
            t |= ((u16 *)pp)[21];
            sProjScaleY[gCurView] = t * (1.0f / 65536);
            sProjScaleY[gCurView] = sProjScaleY[gCurView] * (gViewRects[gNumViews - 1][gCurView].h * 0.5f);
            D_800E19C4 = pp;
            guRotateRPY(&rotA, gCamPitch[gCurView] * 360.0f * (1.0f / 65536),
                          -(f32)gCamYaw[gCurView] * 360.0f * (1.0f / 65536), 0.0f);
            guRotateRPY(&rotB, 0.0f, 0.0f, gCamRoll[gCurView] * 360.0f * (1.0f / 65536));
            guMtxCatL(&rotB, &rotA, &mf);
            mtx_transpose_rot(&mf);
            if (gCurTrack_12->type == 0) {
                mtx_from_angles_pos_scale(&rotB, 0, 0, 0, -gCamPos[gCurView].x, -gCamPos[gCurView].y,
                              -gCamPos[gCurView].z, 0x100);
            } else {
                mtx_from_angles_pos_scale(&rotB, 0, 0, 0, 0, 0, 0, 0x100);
            }
            mtx_mul(&rotB, &mf, &rotA);
            if (gMirrorMode != 0) {
                mtx_mul(&rotA, &sMirrorViewAxisMtx, &rotB);
            } else {
                mtx_mul(&rotA, &gViewAxisMtx, &rotB);
            }
            mtx_mul(&rotB, &persp, &out);
            if (i == 0) {
                dyn->m0[gCurView] = out;
            } else {
                dyn->m1[gCurView] = out;
            }
        }
        if (gCurTrack_12->type == 0) {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, gCamPos[gCurView].x,
                          gCamPos[gCurView].y - 500, gCamPos[gCurView].z, 0x10);
        } else if (gCurTrack_12->type == 1) {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, 0, 0, 0, 0x10);
        } else {
            mtx_from_angles_pos_scale(&dyn->m2[gCurView], 0, 0, 0, 0, -2000, 0, 0x10);
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders_5[i].b7A5 == 0) {
            r = gRiderSection[i];
            gRiderViewLod[i] = 0;
            for (k = 0; k < gNumViews; k++) {
                t = render_get_rider_lod((s32)((f32)(s32)gRiders_5[i].f48 * 1000.0f),
                                  (s32)((f32)(s32)gRiders_5[i].f4C * 1000.0f),
                                  (s32)((f32)(s32)gRiders_5[i].f50 * 1000.0f), k, r, dyn, i) &
                    0xF;
                if (gRiders_5[i].b44 != 0) {
                    t |= (render_get_rider_lod(gRiders_5[i].x2, gRiders_5[i].y2, gRiders_5[i].z2, k, r, dyn, i) & 0xF)
                         << 4;
                } else {
                    t |= t << 4;
                }
                gRiderViewLod[i] |= t << (k * 8);
            }
        }
    }

    gdl = *pgdl;
    mtx_from_angles_pos_scale(&dyn->m4, 0, 0, 0, 0, 0, 0, 0x100);
    if (gRacePhase_2 == 3 || gRacePhase_2 == 7) {
        gSPSegment(gdl++, 0, 0);
        gSPDisplayList(gdl++, D_800C2A38);
        gSPDisplayList(gdl++, D_800C2A00);
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
            if (D_800E3730_4.s0 != -1) {
                if (D_800E3730_4.s0 > 0) {
                    D_800E3730_4.s0--;
                }
                goto end;
            }
            break;
    }
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gCurView = 0;
    CHECK(1000);
    if ((gDrawFlags & 0x40) && gPodiumMode == 0) {
        for (i = 0; i < 8; i++) {
            if (gRiders_5[i].b7A5 == 0 && !((i ^ sRenderFrame) & 1) && gRiderViewLod[i] != 0) {
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
    if (sClearFrames != 0 || D_800E3730_4.c2 != 0 || gPauseMenu != 0) {
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth, gScreenHeight);
        if (D_800E3730_4.c2 != 0) {
            D_800E3730_4.c2--;
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
            gdl = sky_draw(gdl, dyn, gSkyImage_3);
        } else {
            gdl = render_clear_viewports(gdl);
        }
    }
    n = gSkyMode;
    if (n == 2 || n == 3) {
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gSPClipRatio(gdl++, FRUSTRATIO_3);
        if (n == 3) {
            gDPLoadTLUT_pal256(gdl++, gSkyImage_3->f4);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
            gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        } else {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetCombine(gdl++, 0x61FEC3, 0x44FE7F3F);
            gDPSetPrimColor(gdl++, 0, 0, D_800CCD63, D_800CCD63, D_800CCD63, 0);
            gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        }
        for (gCurView = 0; gCurView < gNumViews; gCurView++) {
            gdl = render_set_viewport(gdl, dyn, gCurView, 1);
            gSPMatrix(gdl++, &dyn->m2[gCurView], G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
            if (gDrawFlags & 2) {
                gdl = prim_build_dl(0, sSkyModel, &gSkyImage_3, gdl);
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
        if (gRiders_5[i].b7A5 == 0) {
            if (gRiderViewLod[i] & 0x0F0F0F0F) {
                bike_skel_update(gBikeSkelTemplate_2, i);
                *gRiders_5[i].skel = *gBikeSkelTemplate_2;
                if (gPodiumMode == 0) {
                    for (k = 0; k < 6; k++) {
                        Bone *bn = &gRiders_5[i].skel->b[D_800C25CC[k]];
                        mtx_from_rot3i_pos(&bn->m40, bn->v34, gRiderGfx_5[i].p23C[fb] + k);
                    }
                }
            }
            if (gRiderViewLod[i] & 0xF0F0F0F0) {
                if (gRiders_5[i].b6E8 == 0 || !((sRenderFrame / 2 + i) & 1) || gNumScreens != 1 ||
                    ((gRiderViewLod[i] >> 4) & 0xF) == 3 || gRiders_5[i].b44 != 0 || (gGame.flags & 1)) {
                    rider_update_pose(gRiders_5[i].p65C, i);
                }
                mtx_from_bones(gRiders_5[i].p65C, gRiderGfx_5[i].p234[fb], 0xF);
                if (gPodiumMode == 0 && (sBigHeadCheat != 0 || sPinHeadCheat != 0)) {
                    u16 *m = (u16 *)&gRiderGfx_5[i].p234[fb][2];

                    for (a = 0; a < 3; a++) {
                        for (b = 0; b < 3; b++) {
                            s32 v = m[a * 4 + b];

                            v <<= 16;
                            v |= m[a * 4 + b + 16];

                            if (sBigHeadCheat != 0) {
                                v <<= 1;
                            } else {
                                v >>= 1;
                            }
                            m[a * 4 + b] = v >> 16;
                            m[a * 4 + b + 16] = v;
                        }
                    }
                }
            }
            for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                if (gRiderViewLod[i] & (0xFF << (gCurView * 8))) {
                    if (gCurTrack_12->type == 0) {
                        mtx_from_angles_pos_scale(&gRiderGfx_5[i].a[fb * 4 + gCurView], 0, 0, 0, gRiders_5[i].x,
                                      gRiders_5[i].y, gRiders_5[i].z, 0x10);
                        if (gRiders_5[i].b44 != 0) {
                            mtx_from_angles_pos_scale(&gRiderGfx_5[i].c[fb * 4 + gCurView], 0, 0, 0, gRiders_5[i].x2,
                                          gRiders_5[i].y2, gRiders_5[i].z2, 0x10);
                        }
                    } else {
                        mtx_from_angles_pos_scale(&gRiderGfx_5[i].a[fb * 4 + gCurView], 0, 0, 0,
                                      gRiders_5[i].x - gCamPos[gCurView].x,
                                      gRiders_5[i].y - gCamPos[gCurView].y,
                                      gRiders_5[i].z - gCamPos[gCurView].z, 0x10);
                        if (gRiders_5[i].b44 != 0) {
                            mtx_from_angles_pos_scale(&gRiderGfx_5[i].c[fb * 4 + gCurView], 0, 0, 0,
                                          gRiders_5[i].x2 - gCamPos[gCurView].x,
                                          gRiders_5[i].y2 - gCamPos[gCurView].y,
                                          gRiders_5[i].z2 - gCamPos[gCurView].z, 0x10);
                        }
                    }
                    mtx_mul(&gRiderGfx_5[i].a[fb * 4 + gCurView], &dyn->m0[gCurView],
                                  &gRiderGfx_5[i].b[fb * 4 + gCurView]);
                    if (gRiders_5[i].b44 != 0) {
                        mtx_mul(&gRiderGfx_5[i].c[fb * 4 + gCurView], &dyn->m0[gCurView],
                                      &gRiderGfx_5[i].d[fb * 4 + gCurView]);
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
            if (gCurTrack_12->type == 0) {
                gdl = arn_draw_far(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->type == 1) {
                gdl = tube_draw_far(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->type == 2) {
                gdl = des_draw_terrain(gdl, dyn, fb, gCurView);
            }
        }
        gdl = hacks_draw_pass1(gdl, dyn, fb, gCurView);
    }
    CHECK(500);
    gdl = render_restore_state(render_load_ucode(gdl, fb, dyn, D_800A9D00, D_800D15C0), fb, dyn);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    if (gDrawFlags & 4) {
        for (gCurView = 0; gCurView < gNumViews; gCurView++) {
            CHECK(500);
            gdl = render_set_viewport(gdl, dyn, gCurView, 1);
            if (gCurTrack_12->type == 0) {
                gdl = arn_draw(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->type == 1) {
                gdl = tube_draw_2(gdl, dyn, fb, gCurView);
            } else if (gCurTrack_12->type == 2) {
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
            if (gCurTrack_12->f50 & 2) {
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
            if (gRiders_5[i].b7A5 == 0) {
                for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                    if ((gRiderViewLod[i] >> (gCurView * 8)) & 0xF) {
                        if (!(sRiderDrawFlags[i] & 0x10)) {
                            shadow_build_mesh(i);
                            sRiderDrawFlags[i] |= 0x10;
                        }
                        gdl = render_set_viewport(gdl, dyn, gCurView, 0);
                        gSPMatrix(gdl++, &dyn->m1[gCurView], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                        gSPMatrix(gdl++, &gRiderGfx_5[i].a[fb * 4 + gCurView],
                                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        gdl = shadow_draw(i, dyn, gdl, gCurView);
                    }
                }
                if (gRiders_5[i].b44 != 0) {
                    for (gCurView = 0; gCurView < gNumViews; gCurView++) {
                        if ((gRiderViewLod[i] >> (gCurView * 8)) & 0xF0) {
                            if (!(sRiderDrawFlags[i] & 0x20)) {
                                shadow_build_mesh(i + 8);
                                sRiderDrawFlags[i] |= 0x20;
                            }
                            gdl = render_set_viewport(gdl, dyn, gCurView, 0);
                            gSPMatrix(gdl++, &dyn->m1[gCurView], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                            gSPMatrix(gdl++, &gRiderGfx_5[i].c[fb * 4 + gCurView],
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
        if (gCurTrack_12->type == 1) {
            gdl = tube_draw_water(gdl, dyn, fb, gCurView);
        } else if (gCurTrack_12->type == 0) {
            gdl = arn_draw_water(gdl, dyn, fb, gCurView);
        } else if (gCurTrack_12->type == 2) {
            gdl = des_draw_waypoint_marker(gdl, dyn, fb, gCurView);
        }
    }
    if ((gDrawFlags & 0x80) && D_800EE140 == 0) {
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
        s32 b = y + h;
        s32 b1 = b - 1;

        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPPipeSync(gdl++);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0);
        gDPFillRectangle(gdl++, x - 2, y - 2, x + w + 1, y);
        gDPFillRectangle(gdl++, x - 2, y, x, b1);
        gDPFillRectangle(gdl++, x - 2, b1, x + w + 1, b + 1);
        gDPFillRectangle(gdl++, x + w - 1, y, x + w + 1, b + 1);
    }
    if (gCurTrack_12->type == 2) {
        gdl = des_draw_arrow(gdl, dyn, fb, 0);
    }
    CHECK(1000);
    gdl = render_set_viewport(gdl, dyn, 0, 1);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    for (i = 0; i < 8; i++) {
        if (gRiders_5[i].b7A5 == 0 && gRiderAlpha[i] != 0xFF) {
            gdl = render_restore_state(render_load_ucode(gdl, fb, dyn, D_800AB0A0, D_800D19E0), fb, dyn);
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
    D_800E76CC = 1;
    D_800E19B8 = sRidersDrawn;
}

typedef struct {
    u32 seg0;
    u32 seg4;
    u32 seg8;
    u8 padC[0x14 - 0xC];
    u32 tlut14;
    u8 *tlut18;
    u16 pal1C[(0x224 - 0x1C) / 2];
    Gfx *dl224;
    Gfx *dl228;
    u32 seg22C;
    u32 seg230;
    u32 seg234[2];
    Mtx *mtx23C[2];
    u8 pad244[0x448 - 0x244];
    Mtx m448[16];
    Mtx m848[8];
    u8 padA48[0xA4C - 0xA48];
    u8 *fA4C;
    u8 padA50[4];
    void *fA54;
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

typedef struct {
    u8 pad0[0x44];
    u8 b44;
    u8 pad45[0x7A5 - 0x45];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Rider;

extern Bike gRiderGfx_6[];
extern Rider gRiders_6[];
extern DlPair D_800DC110_3[];
extern SegPair D_800E1900_2[];

Gfx *render_draw_riders(Gfx *gdl, s32 vp, s32 p3, s32 xlu) {
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    u32 v;
    s32 started;
    u32 *segs;

    for (i = 0; i < gNumViews; i++) {
        started = 0;
        for (j = 0; j < 8; j++) {
            if (gRiders_6[j].b7A5 != 0) {
                continue;
            }
            v = gRiderViewLod[j] >> (i * 8);
            if (!(v & 0xFF)) {
                continue;
            }
            k = gRiderAlpha[j] != 0xFF;
            if (k != xlu) {
                continue;
            }
            if (gGame.flags & 0x10) {
                m = 2;
                if (j == gGame.riders[i]) {
                    m = 3;
                }
            } else {
                m = v & 0xF;
                if (gNumScreens >= 3) {
                    if (m == 3) {
                        m = 2;
                    }
                    if (i == j && m == 1) {
                        m = 2;
                    }
                }
                if (gNumScreens == 2 && i == j) {
                    m = 3;
                }
                if (gEvent.noPlayers != 0 && j == 0) {
                    m = 3;
                }
                if (gNumScreens == 1 && (j == gCamTargetOverride || (gCamTargetOverride == -1 && j == 0))) {
                    m = 3;
                }
            }
            if (gPodiumMode != 0) {
                m = 3;
                if (gRiderGfx_6[j].dl228 == 0) {
                    continue;
                }
            }
            if (!started) {
                gdl = render_set_viewport(gdl, p3, i, 1);
                started = 1;
            }
            sRidersDrawn++;
            if (!(sRiderDrawFlags[j] & 4)) {
                render_update_rider_palette(j, i);
                if (!(gGame.flags & 0x10)) {
                    sRiderDrawFlags[j] |= 4;
                }
            }
            if (gGame.flags & 0x10) {
                if (gGame.numPlayers == 1) {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[j].tlut18 + gGame.riderSkinSel * 0x200);
                } else {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[j].tlut18 + i * 0x200);
                }
            } else {
                gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[j].tlut14);
            }
            if (gRiders_6[j].b44 != 0) {
                gSPMatrix(gdl++, &gRiderGfx_6[j].m848[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            } else {
                gSPMatrix(gdl++, &gRiderGfx_6[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            }
            if (xlu == 0) {
                gDPSetRenderMode(gdl++, 0x442230, 0);
            } else {
                gDPSetRenderMode(gdl++, 0x404A70, 0);
                gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[j]);
            }
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
            if (gDrawFlags & 1) {
                if (m == 3) {
                    gSPSegment(gdl++, 1, gRiderGfx_6[j].seg230 & 0xFFFFFF);
                    if (gGame.flags & 0x10) {
                        if (gGame.numPlayers == 1) {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][gGame.riderSkinSel] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, gRiderGfx_6[j].seg0 & 0xFFFFFF);
                        gSPSegment(gdl++, 3, gRiderGfx_6[j].seg4 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, gRiderGfx_6[j].seg234[gGfxFrameIndex]);
                    if (!(gCheatFlags & 0x20)) {
                        gSPDisplayList(gdl++, gRiderGfx_6[j].dl228);
                    }
                } else {
                    gSPSegment(gdl++, 1, gRiderGfx_6[j].seg22C & 0xFFFFFF);
                    if (gGame.flags & 0x10) {
                        if (gGame.numPlayers == 1) {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][gGame.riderSkinSel % 5] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, gRiderGfx_6[j].seg0 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, gRiderGfx_6[j].seg234[gGfxFrameIndex]);
                    if (!(gCheatFlags & 0x20)) {
                        gSPDisplayList(gdl++, gRiderGfx_6[j].dl224);
                    }
                }
                if (gRiderGfx_6[j].fA54 != 0) {
                    gPrimBoneMtx = gRiderGfx_6[j].fA4C + vp * 0xC0;
                    if (!(gCheatFlags & 0x20)) {
                        if (gGame.flags & 0x10) {
                            segs = D_800E1600_2[j];
                            gdl = prim_build_dl(0, gRiderGfx_6[j].fA54,
                                                (gGame.numPlayers == 1) ? segs + gGame.riderSkinSel : segs + i, gdl);
                        } else {
                            gdl = prim_build_dl(0, gRiderGfx_6[j].fA54, &gRiderGfx_6[j], gdl);
                        }
                    }
                }
            }
            if (gPodiumMode == 0) {
                if (gRiders_6[j].b44 != 0) {
                    gSPMatrix(gdl++, &gRiderGfx_6[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                }
                if (gGame.flags & 0x10) {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[(j + gGame.bikeSkinSel) % 6].pal1C);
                } else {
                    gDPLoadTLUT_pal256(gdl++, gRiderGfx_6[j].pal1C);
                }
                for (k = 0; k < 6; k++) {
                    if (xlu != 1) {
                        if (k != 5 && k != 2 && m != 1) {
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
                        gSPMatrix(gdl++, gRiderGfx_6[j].mtx23C[gGfxFrameIndex] + k, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        if (gGame.flags & 0x10) {
                            { Gfx *g = gdl++; g->words.w0 = 0xDB060008; if (j == 0) v = 0; gdl[-1].words.w1 = gRiderGfx_6[(j + gGame.bikeSkinSel) & 1].seg8; }
                        } else {
                            gSPSegment(gdl++, 2, gRiderGfx_6[j].seg8);
                        }
                        if (m == 1) {
                            gSPSegment(gdl++, 2, gRiderGfx_6[j].seg8);
                            gSPDisplayList(gdl++, D_800E1B48);
                            break;
                        }
                        if (m == 3) {
                            gSPSegment(gdl++, 1, D_800E1900_2[k].p4->seg);
                            gSPDisplayList(gdl++, D_800DC110_3[k].dl4);
                        } else {
                            gSPSegment(gdl++, 1, D_800E1900_2[k].p0->seg);
                            gSPDisplayList(gdl++, D_800DC110_3[k].dl0);
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
    u16 *f4;
} Tex;

typedef struct {
    u8 pad0[0xC];
    u16 *src;      /* 0x0C */
    u16 *src2;     /* 0x10 */
    u16 *dst;      /* 0x14 */
    u16 *base;     /* 0x18 */
    u16 pal2[256]; /* 0x1C */
    u8 pad21C[0xA80 - 0x21C];
} Pal_4;

typedef struct {
    u8 pad0[0x654];
    u8 b654;
    u8 pad655[0x6E8 - 0x655];
    u8 b6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_3;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0xA0 - 0x30];
} Track_5;

extern Pal_4 gRiderGfx_7[];
extern Tex *D_800E1600_3[][6];
extern Player_3 gRiders_7[];
extern Track_5 *gCurTrack_13;
extern Track_5 gTracks_7[];

void render_update_rider_palette(s32 pl, s32 k)
{
    s32 r, g, b;
    s32 i, c, v, t;
    s32 fr, fg, fb;
    s32 cr, cg, cb;

    if (gPodiumMode != 0) {
        return;
    }
    if (gGame.flags & 0x10) {
        if (gGame.numPlayers == 1) {
            gRiderGfx_7[pl].src = D_800E1600_3[pl][(k + gGame.riderSkinSel) % 5]->f4;
            gRiderGfx_7[pl].dst = gRiderGfx_7[pl].base + ((k + gGame.riderSkinSel) % 5) * 0x100;
        } else {
            gRiderGfx_7[pl].src = D_800E1600_3[pl][k]->f4;
            gRiderGfx_7[pl].dst = gRiderGfx_7[pl].base + k * 0x100;
        }
    }
    i = 0;
    if (gCreditsMode != 0) {
        do {
            c = gRiderGfx_7[pl].src[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            palmod_sepia(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            gRiderGfx_7[pl].dst[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            c = gRiderGfx_7[pl].src2[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            palmod_sepia(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            gRiderGfx_7[pl].pal2[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            i++;
        } while (i < 0x100);
        gRiderGfx_7[pl].pal2[0] &= 0xFFFE;
        return;
    }
    if (gLightningFlash != 0) {
        do {
            s32 x, y, z;

            x = gRiderGfx_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            gRiderGfx_7[pl].dst[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            x = gRiderGfx_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            gRiderGfx_7[pl].pal2[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            i++;
        } while (i < 0x100);
        gRiderGfx_7[pl].pal2[0] &= 0xFFFE;
        sRiderPalDirty[pl] = 1;
        return;
    }
    if ((gRiders_7[pl].b654 != 0 && gRaceFrame % 30 < 15 && gNumScreens >= 2) ||
        (gRiders_7[pl].b6E8 == 0 && (gCheatFlags & 0x400) && gRaceFrame % 32 < 16)) {
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            gRiderGfx_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = gRiderGfx_7[pl].src2[i];
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            gRiderGfx_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        gRiderGfx_7[pl].pal2[0] &= 0xFFFE;
        sRiderPalDirty[pl] = 1;
        return;
    }
    if (gCurTrack_13->f2C == 2 && sRiderCamDist[pl] > 0xC48) {
        fr = gFogColor[0] * 31 / 256;
        fg = gFogColor[1] * 31 / 256;
        fb = gFogColor[2] * 31 / 256;
        t = (sRiderCamDist[pl] - 0xC48) * 255 / 5241;
        if (t > 255) {
            t = 255;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            gRiderGfx_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = gRiderGfx_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            gRiderGfx_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        return;
    }
    sRiderDrawFlags[pl] |= 4;
    if (D_800E1B50[pl] & 0x200) {
        if (sRiderTint[pl * 3] == sRiderLightColor[pl][0] && sRiderTint[pl * 3 + 1] == sRiderLightColor[pl][1] &&
            sRiderTint[pl * 3 + 2] == sRiderLightColor[pl][2] && sRiderPalDirty[pl] == 0) {
            return;
        }
        sRiderPalDirty[pl] = 0;
        cr = sRiderTint[pl * 3];
        cg = sRiderTint[pl * 3 + 1];
        cb = sRiderTint[pl * 3 + 2];
        if (gCurTrack_13 - gTracks_7 == 15) {
            cr += (255 - cr) / 3;
            cg += (255 - cg) / 3;
            cb += (255 - cb) / 3;
        }
        sRiderLightColor[pl][0] = cr;
        sRiderLightColor[pl][1] = cg;
        sRiderLightColor[pl][2] = cb;
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            gRiderGfx_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = gRiderGfx_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            gRiderGfx_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | (i != 0);
        }
    } else {
        __builtin_memcpy(gRiderGfx_7[pl].dst, gRiderGfx_7[pl].src, 0x200);
        __builtin_memcpy(gRiderGfx_7[pl].pal2, gRiderGfx_7[pl].src2, 0x200);
        sRiderLightColor[pl][0] = 0xFF;
        sRiderLightColor[pl][1] = 0xFF;
        sRiderLightColor[pl][2] = 0xFF;
    }
}

Gfx *render_restore_state(Gfx *gdl, s32 a1, s32 a2)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *g9, *g10, *g11, *g12, *g13, *g14;
    f32 fv5 = (f32)(gScreenWidth - 0x11) * 4.0f;
    f32 fv4 = (f32)(gScreenHeight - 0xd) * 4.0f;
    s32 iv2 = gCurView * 0x40;

    g0 = gdl++;
    ((u32 *)g0)[0] = 0xED040030;
    ((u32 *)g0)[1] = (((s32)fv5 & 0xfff) << 12) | ((s32)fv4 & 0xfff);
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
    ((u32 *)g5)[1] = (u32)gFramebuffers[a1] + 0x80000000;
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
    ((u32 *)g9)[1] = (u32)(a2 + iv2) + 0x80000000;
    g10 = gdl++;
    ((u32 *)g10)[0] = 0xDA380003;
    ((u32 *)g10)[1] = (u32)a2 + 0x80000400;
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
    s32 x, y, z;
    u8 pad0C[0x44 - 0xC];
    u8 b44;
    u8 pad45[0x662 - 0x45];
    s16 h662;
    u8 pad664[0x7A5 - 0x664];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_4;

typedef struct {
    u8 pad[8];
    u8 w;
    u8 h;
    u8 pad2[2];
    void *tex;
} TexInfo_2;

extern TexInfo_2 *sMarkerImage_2;
extern Player_4 gRiders_8[];

Gfx *render_draw_place_markers(Gfx *gdl, s32 arg1) {
    s32 sx, sy, sz, sw;
    s32 w1, h1, s1, t1, w2, h2, s2, t2;
    s32 tw8, tw, th, i, j, y, off;
    s32 pl;
    Player_4 *p;

    w2 = gScreenWidth / 16;
    h2 = gScreenHeight / 15;
    s2 = 0x8000 / w2;
    t2 = 0x4000 / h2;
    tw = sMarkerImage_2->w;
    w1 = gScreenWidth * 24 / 320;
    s1 = (tw << 13) / w1;
    th = sMarkerImage_2->h;
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
    tw8 = tw * 8;
    h1 = gScreenHeight / 10;
    t1 = (th << 10) / h1;
    for (i = 0; i < gNumScreens; i++) {
        gdl = render_set_viewport(gdl, arg1, i, 0);
        for (j = 0; j < 8; j++) {
            pl = (j << 11) + (j << 9) + (j << 7);
            p = &gRiders_8[j];
            if (p->b7A5 != 0) continue;
            if (j >= 6) continue;
            if (!((gRiderViewLod[j] >> (i * 8)) & 0xF)) continue;
            if (p->b44 != 0) continue;
            if (p->h662 != gRiders_8[i].h662 - 1) continue;
            if (gCurTrack_4->f2C == 0) {
                if (project_to_screen(p->x >> 4, (p->y + 2000) >> 4, p->z >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            } else {
                if (project_to_screen((p->x - gCamPos[i].x) >> 4, (p->y - gCamPos[i].y + 2000) >> 4,
                                  (p->z - gCamPos[i].z) >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            }
            if (sx < -50) continue;
            if (sy < -50) continue;
            if (sx > gScreenWidth + 50) continue;
            if (sy > gScreenHeight + 50) continue;
            off = -12;
            y = sy + gScreenHeight * off / 240;
            gDPSetPrimDepth(gdl++, sz, 0);
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetTextureImage(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 1, sMarkerImage_2->tex);
            gDPSetTile(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 7, 0, 0, 7, 0);
            gDPLoadSync(gdl++);
            gDPLoadBlock(gdl++, G_TX_LOADTILE, 0, 0, (s32)((u32)(tw8 * th) >> 2) - 1, CALC_DXT_4b(tw8));
            gDPPipeSync(gdl++);
            {
                Gfx *g = gdl++;
                g->words.w0 = ((u32)((tw8 >> 1) + 7) >> 3 << 9) | 0xF5400000;
                g->words.w1 = 0x1C070;
            }
            gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (tw8 - 1) << G_TEXTURE_IMAGE_FRAC, (th - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPLoadTLUT_pal256(gdl++, &D_800DCBB0[0][pl]);
            gSPTextureRectangle(gdl++, (sx - w1 / 2) * 4, (y - h1 / 2) * 4, (sx + w1 / 2) * 4, (y + h1 / 2) * 4,
                                0, 0, 0, s1, t1);
            gDPPipeSync(gdl++);
            off = -7;
            y = sy + gScreenHeight * off / 240;
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            off = p->h662;
            gDPLoadTextureBlock(gdl++, sPlaceTextures + (off << 9), G_IM_FMT_CI, G_IM_SIZ_8b, 32, 16, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 5, 4, 0, 0);
            gSPTextureRectangle(gdl++, (sx - w2 / 2) * 4, (y - h2 / 2) * 4, (sx + w2 / 2) * 4, (y + h2 / 2) * 4,
                                0, 0, 0, s2, t2);
        }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    return gdl;
}

typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 b654;
    u8 pad655[0x7E0 - 0x655];
} Player_5;

typedef struct {
    u8 pad0[0xC];
    u8 *tC;
    u8 pad10[4];
    u8 *t14;
    u8 pad18[4];
    u8 *t1C;
} TexSet;

extern Player_5 gRiders_9[];
extern TexSet *sGaugeTextures_2;

Gfx *render_draw_temp_gauge(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 p;
    s32 sx, sy;
    s32 lvl, k;
    s32 i;
    u16 *pal;
    s32 x, y;

    if (gGame.mode == 8) {
        p = D_800C8CE0;
    } else {
        p = pl;
    }
    sx = 0x80000 / gScreenWidth;
    sy = 0x3C000 / gScreenHeight;
    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, 0x0C087008, 0);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0xFE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gdl = render_set_viewport(gdl, a1, 0, 1);
    gSPClipRatio(gdl++, FRUSTRATIO_6);

    lvl = gRiders_9[p].f648 * 256.0f / 12000.0f;
    if (lvl < 0) {
        lvl = 0;
    }
    if (lvl > 255) {
        lvl = 255;
    }
    pal = sGaugePalettes_2 + a2 * 256;
    k = lvl * 87 / 256;
    k += 148;
    for (i = 148; i < 192; i++) {
        if (k < i) {
            pal[i] = 0x201;
        } else {
            pal[i] = 0x7C1;
        }
    }
    for (i = 192; i < 224; i++) {
        if (k < i) {
            pal[i] = 0x4201;
        } else {
            pal[i] = 0xFFC1;
        }
    }
    for (i = 224; i < 235; i++) {
        if (k < i) {
            pal[i] = 0x4001;
        } else {
            pal[i] = 0xF801;
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, pal);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, sGaugeTextures_2->t14 + i * 80, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, sGaugeX * 4, (sGaugeY + i * gScreenHeight / 240) * 4,
                            (sGaugeX + gScreenWidth * 80 / 512) * 4,
                            (sGaugeY + (i + 25) * gScreenHeight / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetBlendColor(gdl++, 0, 0, 0, 8);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    if (gRiders_9[p].b654 != 0) {
        gDPSetCombineLERP(gdl++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPLoadTextureBlock(gdl++, sGaugeTextures_2->tC, G_IM_FMT_IA, G_IM_SIZ_8b, 48, 16, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        k = (-gRaceFrame * 8) & 0xFF;
        gDPSetPrimColor(gdl++, 0, 0, 255, 0, 0, k);
        gSPTextureRectangle(gdl++, (sGaugeX + gScreenWidth / 32) * 4, (sGaugeY + gScreenHeight / 24) * 4,
                            (sGaugeX + gScreenWidth / 8) * 4, (sGaugeY + gScreenHeight * 23 / 240) * 4,
                            0, 0, 0, sx, sy);
        gDPPipeSync(gdl++);
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, sGaugeTextures_2->t1C + i * 80, G_IM_FMT_IA, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, sGaugeX * 4, (sGaugeY + i * gScreenHeight / 240) * 4,
                            (sGaugeX + gScreenWidth * 80 / 512) * 4,
                            (sGaugeY + (i + 25) * gScreenHeight / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
    gSPMatrix(gdl++, a1 + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPViewport(gdl++, &gScreenViewport);
    k = 55000 - lvl * 175;
    for (i = 0; i < 4; i++) {
        s32 x0 = D_800C23D0[i * 2];
        s32 y0 = D_800C23D0[i * 2 + 1];
        x = (x0 * gCosTable[(k >> 4) & 0xFFF] >> 14) + (y0 * gCosTable[((k >> 4) - 0x400) & 0xFFF] >> 14);
        y = (y0 * gCosTable[(k >> 4) & 0xFFF] >> 14) - (x0 * gCosTable[((k >> 4) - 0x400) & 0xFFF] >> 14);
        x = x * gScreenWidth / 320;
        y = y * gScreenHeight / 240;
        x += sGaugeX + sGaugeNeedleX;
        y += sGaugeY + sGaugeNeedleY;
        x -= gScreenWidth / 2;
        y = gScreenHeight / 2 - y;
        sGaugeNeedleVtx_2[a2 * 8 + i].v.ob[0] = x;
        sGaugeNeedleVtx_2[a2 * 8 + i].v.ob[1] = y;
        sGaugeNeedleVtx_2[a2 * 8 + i].v.ob[2] = 0x200;
        sGaugeNeedleVtx_2[a2 * 8 + i + 4].v.ob[0] = x + 4;
        sGaugeNeedleVtx_2[a2 * 8 + i + 4].v.ob[1] = y - 2;
        sGaugeNeedleVtx_2[a2 * 8 + i + 4].v.ob[2] = 0x200;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPVertex(gdl++, &sGaugeNeedleVtx_2[a2 * 8], 8, 0);
    gSP2Triangles(gdl++, 7, 4, 5, 0, 4, 5, 6, 0);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0x8C, 0x8C, 0x8C, 0xFF);
    gSP2Triangles(gdl++, 3, 0, 1, 0, 3, 1, 2, 0);
    gSPMatrix(gdl++, a1, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    return gdl;
}

Gfx *render_draw_image(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw) {
    s32 step, row, n, sc, srow, sn, clip, wdt;
    s32 m;
    u8 *addr;

    m = 2;
    if (flags & 2) m = 1;
    step = ((flags & 1) ? m << 11 : m << 12) / (img->w * 8);
    if (img->h < step) step = img->h;

    switch (sx) {
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
    switch (sy) {
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
    for (row = 0; row < img->h; row += step) {
        n = step;
        if (img->h - row < step) n = img->h - row;
        if (flags & 0x100) {
            sc = 2;
            srow = row / 2;
            sn = n / 2;
        } else {
            sc = 1;
            srow = row;
            sn = n;
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
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8 - clip) * 4, (y + srow + sn) * 4,
                                0, clip << 5, 0, sc << 10, sc << 10);
        } else if (flags & 0x40) {
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8) * 4, (y + srow + sn) * 4,
                                0, (img->w << 8) - 0x20, (n - 1) << 5, -(sc << 10), -(sc << 10));
        } else {
            if (flags & 0x80) wdt = dw;
            else wdt = img->w * 8;
            if (gScreenWidth == 640) {
                gSPTextureRectangle(gdl++, x * 4, (y + srow * 2) * 4, x * 4 + (wdt - clip) * 5, (y + srow * 2 + sn * 2) * 4,
                                    0, clip << 5, (n - 1) << 5, sc * 0x333, -(sc << 9));
            } else {
                gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + wdt / sc - clip) * 4, (y + srow + sn) * 4,
                                    0, clip << 5, (n - 1) << 5, sc << 10, -(sc << 10));
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
    Q *pb;
    V4 *s;
    V4 *p;
    s32 t;
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
            t = *gScreenWidth_2;
            D_800E1758[0] = D_800E1758[-4];
            p = D_800E1758 - 4;
            p[5].x = t / 16;
            p[5].z = *gScreenWidth_2 * 130 / 512;
            p[5].w = gScreenHeight_3[0] * 70 / 240;
            if (gViMode == 2) {
                p[5].y = 100;
            } else {
                p[5].y = 30;
            }
        }
        pb = gViewports;
        n = gNumViews - 1;
        for (gCurView = 0; gCurView < 4; gCurView++) {
            s = (V4 *)((u8 *)gViewRects_7 + (gCurView * 16 + n * 64));
            pb[gCurView].v[0] = s->z * 2;
            pb[gCurView].v[1] = s->w * 2;
            pb[gCurView].v[4] = (s->x + s->z / 2) * 4;
            pb[gCurView].v[5] = (s->y + s->w / 2) * 4;
            pb[gCurView + 4].v[0] = s->z * 2;
            pb[gCurView + 4].v[1] = s->w * 2;
            pb[gCurView + 4].v[4] = (s->x + s->z / 2) * 4;
            pb[gCurView + 4].v[5] = (s->y + s->w / 2) * 4 + 2;
            pb[gCurView].v[2] = 0x1FF;
            pb[gCurView].v[6] = 0x1FF;
            pb[gCurView + 4].v[2] = 0x1FF;
            pb[gCurView + 4].v[6] = 0x1FF;
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
    s32 i, y, done, alt, col;
    s32 c;
    s32 *font;
    s32 n;
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
    alt = 0;
    if (y < 0x104) {
    n = 0;
    do {
        switch (sCreditsText[i]) {
        case 0xD:
        case '@':
            buf[n] = 0;
            if (y >= -0x13 && n != 0) {
                if (alt) {
                    font = sCreditsHeaderFont;
                } else {
                    font = sCreditsFont;
                }
                font_text_clear((u8 *)font);
                font_text_add(font, buf);
                font_draw_text(font, col * 128 - font[6] / 2, y);
                alt = 0;
            }
            n = 0;
            if (sCreditsText[i] == '@') {
                i++;
                c = sCreditsText[i];
                switch (c) {
                case 'E':
                    done = 1;
                    i = -1;
                    break;
                case 'H':
                    alt = 1;
                    break;
                case '1':
                case '2':
                case '3':
                    col = c - '0';
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
            buf[n++] = sCreditsText[i++];
            break;
        }
    } while (!done && y < 0x104);
    }
    return font_render(gdl);
}

void render_make_world_mtx(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9)
{
    s32 *t;
    if (gCurTrack_3[11] == 0) {
        mtx_from_angles_pos_scale(p1, p2, p3, p4, p5, p6, p7, p8);
    } else {
        s32 *tb = gCamPos;
        t = tb + p9 * 3;
        mtx_from_angles_pos_scale(p1, p2, p3, p4, p5 - t[0], p6 - t[1], p7 - t[2], p8);
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

Gfx *render_load_ucode(Gfx *gdl, s32 a1, s32 a2, s32 a3, s32 a4)
{
    PKT(gdl++, 0xE7000000, 0);
    LOADUCODE(gdl++, a3, a4);
    PKT(gdl++, 0xDB060000, 0);
    PKT(gdl++, 0xDE000000, D_800C2A38);
    PKT(gdl++, 0xDE000000, D_800C2A00);
    return gdl;
}

void render_reset_viewports(void) {
    gInsetViewActive = 1;
    sInsetViewWanted = 0;
    render_update_viewports();
}

void render_apply_split_layout(void)
{
    Blk *p1;
    Blk *p2;
    Blk *e;

    if (gSplitVertical != 0) {
        p2 = (Blk *)sViewLayoutsMulti_2;
        p1 = (Blk *)sLayout2PVertical_2;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    } else {
        p2 = (Blk *)sViewLayoutsMulti_2;
        p1 = (Blk *)sLayout2PHorizontal_2;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    }
    render_reset_viewports();
}

void render_blit_place_glyph(u8 *src, s32 x, s32 y, s32 w, s32 h)
{
    s32 i, j, xx;

    i = 0;
    do {
        if (h > 0) {
            do {
                if (y < 0) goto next;
                if (y >= 16) goto next;
                do { xx = x; } while (0);
                for (j = 0; j < w; j++, xx++) {
                    if (xx < 0) continue;
                    if (xx >= 32) continue;
                    sPlaceTexDst[y * 32 + xx] = src[i * 64 + j];
                }
                do { next: i++; y++; } while (0);
            } while (i < h);
        }
    } while (0);
}

void render_lerp_rgb(u8 *param_1, u8 *param_2, u8 *param_3, s32 param_4, s32 param_5)
{
    s32 total = param_5 * 3;
    s32 i = 0;
    u8 *pa = param_1;
    u8 *pb = param_2;
    u8 *po = param_3;
    if (total > 0) {
        s32 inv = 0xFF - param_4;
        s32 lim = total;
        do {
            *po = ((s32)*pa * inv + (s32)*pb * param_4) >> 8;
            pa++;
            i++;
            pb++;
            po++;
        } while (i < lim);
    }
}

void render_lerp_palette(u16 *p1, u16 *p2, u16 *p3, s32 a, s32 n)
{
    s32 i;
    u32 c1, c2;
    s32 r, g, b;

    for (i = 0; i < n; i++) {
        c1 = *p1++;
        c2 = *p2++;
        r = (s32)((c1 >> 11) * (0xFF - a) + (c2 >> 11) * a) >> 8;
        g = (s32)(((c1 >> 6) & 0x1F) * (0xFF - a) + ((c2 >> 6) & 0x1F) * a) >> 8;
        b = (s32)(((c1 >> 1) & 0x1F) * (0xFF - a) + ((c2 >> 1) & 0x1F) * a) >> 8;
        *p3++ = (r << 11) | (g << 6) | (b << 1);
    }
}
