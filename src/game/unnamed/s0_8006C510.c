#include "game.h"

Gfx *soccer_ball_draw(Gfx *param_1, s32 param_2, s32 param_3)
{
    s32 aiStack_e0[10];
    s32 aux[3];
    s16 asStack_a8[32];
    s16 auStack_68[32];
    f32 *q;
    f32 fx;
    s32 i;
    s32 iVar5;
    s32 tbl;
    s32 iVar1;
    Gfx *gA, *gB, *gC, *gD, *gE, *gF;
    Gfx *gP, *gQ, *gT, *gU, *gV;

    quat_to_mtx3i(aiStack_e0, gSoccerBall_2 + 12);
    aux[0] = 0;
    aux[1] = 0;
    aux[2] = 0;
    mtx_from_rot3i_pos(aiStack_e0, (u16 *)aux, auStack_68);
    mtx_from_angles_pos_scale(asStack_a8, 0, 0, 0, (s32)(gSoccerBall_2[0] * 1000.0f), (s32)(gSoccerBall_2[1] * 1000.0f),
                  (s32)(gSoccerBall_2[2] * 1000.0f), 8);
    tbl = (u32)sPropMtx + (param_3 << 8);
    mtx_mul(auStack_68, asStack_a8, tbl);

    gA = param_1++;
    ((u32 *)gA)[0] = 0xDA380003;
    ((u32 *)gA)[1] = tbl;
    gB = param_1++;
    ((u32 *)gB)[0] = 0xE7000000;
    ((u32 *)gB)[1] = 0;
    gC = param_1++;
    ((u32 *)gC)[0] = 0xD9FEFFFF;
    ((u32 *)gC)[1] = 0;
    gD = param_1++;
    ((u32 *)gD)[0] = 0xE200001C;
    ((u32 *)gD)[1] = 0x442230;
    gE = param_1++;
    ((u32 *)gE)[0] = 0xFCFFFFFF;
    ((u32 *)gE)[1] = 0xFFFCF279;
    gF = param_1++;
    ((u32 *)gF)[0] = 0xE7000000;
    ((u32 *)gF)[1] = 0;

    for (i = 0; i < gNumViews; i++) {
        param_1 = render_set_viewport_2(param_1, param_2, i);
        gP = param_1++;
        gQ = param_1++;
        ((u32 *)gP)[0] = 0xDA380007;
        ((u32 *)gP)[1] = param_2 + i * 0x40 + 0x80000000;
        ((u32 *)gQ)[0] = 0xDE000000;
        ((u32 *)gQ)[1] = sPropDL;
    }

    gT = param_1++;
    ((u32 *)gT)[0] = 0xE3001001;
    ((u32 *)gT)[1] = 0x8000;
    gU = param_1++;
    ((u32 *)gU)[0] = 0xE200001C;
    ((u32 *)gU)[1] = 0x442230;
    gV = param_1++;
    ((u32 *)gV)[0] = 0xFCFFFFFF;
    ((u32 *)gV)[1] = 0xFFFCF279;
    return param_1;
}

#define gDPSetCombineRaw(pkt, a, b) \
    { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = 0xFC000000 | (a); _g->words.w1 = (b); }

typedef struct { u8 p0[0x58]; u16 a58; u8 p1[6]; u16 a60; } Dev;
typedef struct { s16 m[4][4][2]; } MtxQ;

extern Dev *gCurTrack_35;
extern MtxQ sPropMtx_2[][4];

Gfx *finishline_draw_user_track(Gfx *gdl, Mtx *proj, s32 i, s32 j)
{
    MtxQ ma;
    MtxQ mb;
    s16 ang;
    s32 ratio;
    MtxQ *m;

    math_to_polar((s32)(gPathNodes[1].lateralZ * 1024.0f), (s32)(gPathNodes[1].lateralX * 1024.0f), &ang, &ratio);
    mtx_from_angles_pos_scale(&ma, 0, 0, ang, 0, 0, 0, 0x100);
    mtx_from_angles_pos_scale(&mb, 0, 0, 0, (s32)(gPathNodes[1].pos.x * 1000.0f), (s32)(gPathNodes[1].pos.y * 1000.0f),
                  (s32)(gPathNodes[1].pos.z * 1000.0f), 0x10);
    m = &sPropMtx_2[i][j];
    mtx_mul(&ma, &mb, m);

    gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)render_set_viewport_3(gdl, proj, j);
    gSPMatrix(gdl++, &proj[j + 4], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombineRaw(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, gCurTrack_35->a60, gCurTrack_35->a58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    gdl = prim_build_dl(0, D_800F16E0, D_800EFD6C_3, gdl);
    gdl = prim_build_dl(0, D_800F16E0, D_800EFD6C_3, gdl);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombineRaw(gdl++, 0x127E24, 0xFFFFF3F9);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    gSPFogPosition(gdl++, gCurTrack_35->a58, gCurTrack_35->a60);
    gPrimIATextures = 0;
    gSPDisplayList(gdl++, sTexCutoutFogDL);
    gSPDisplayList(gdl++, sPropDL);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

typedef struct { s16 m[32]; } MtxS_4;

typedef struct { u8 pad[0x58]; u16 f58; u8 pad5A[6]; u16 f60; } Fog_2;
typedef struct { Mtx m[4]; } MtxSet;

extern MtxSet sPropMtx_3[];
extern Fog_2 *gCurTrack_36;

Gfx *finishline_draw_tutorial(Gfx *gdl, Mtx *proj, s32 n) {
    MtxS_4 m1;
    MtxS_4 m2;
    s16 ang;
    s32 ratio;
    PathNode *nd;

    if (gTutorialLessons[gSeason->round].goalNode < gPathNodeCount) {
        { u32 b = (u32)gPathNodes; nd = (PathNode *)(gTutorialLessons[gSeason->round].goalNode * sizeof(PathNode) + b); }
        math_to_polar(nd->lateralZ * 1024.0f, nd->lateralX * 1024.0f, &ang, &ratio);
        mtx_from_angles_pos_scale(&m1, 0, 0, ang, 0, 0, 0, 0x100);
        mtx_from_angles_pos_scale(&m2, 0, 0, 0,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.x * 1000.0f,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.y * 1000.0f,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.z * 1000.0f, 0x10);
        mtx_mul(m1.m, m2.m, (s16 *)&sPropMtx_3[n].m[0]);
    }
    nd = (PathNode *)(gTutorialLessons[gSeason->round].startNode * sizeof(PathNode) + (u32)gPathNodes);
    math_to_polar(nd->lateralZ * 1024.0f, nd->lateralX * 1024.0f, &ang, &ratio);
    mtx_from_angles_pos_scale(&m1, 0, 0, ang, 0, 0, 0, 0x100);
    mtx_from_angles_pos_scale(&m2, 0, 0, 0,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.x * 1000.0f,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.y * 1000.0f,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.z * 1000.0f, 0x10);
    mtx_mul(m1.m, m2.m, (s16 *)&sPropMtx_3[n].m[1]);

    gSPMatrix(gdl++, &sPropMtx_3[n].m[1], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)render_set_viewport_4((u32 *)gdl, (s32)proj, 0);
    gSPMatrix(gdl++, proj + 4, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, 0x200004);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, gCurTrack_36->f60, gCurTrack_36->f58);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    gdl = prim_build_dl(0, D_800F16E0, D_800EFD6C_3, gdl);
    if (gTutorialLessons[gSeason->round].flags & 2) {
        gPrimIATextures = 0;
    } else {
        gSPMatrix(gdl++, &sPropMtx_3[n].m[0], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPPipeSync(gdl++);
        gdl = prim_build_dl(0, D_800F16E0, D_800EFD6C_3, gdl);
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gSPSetGeometryMode(gdl++, 0x200004);
        gSPSetGeometryMode(gdl++, G_ZBUFFER);
        gDPSetBlendColor(gdl++, 0, 0, 0, 255);
        gDPSetAlphaCompare(gdl++, G_AC_NONE);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
        gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
        gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
        gSPFogPosition(gdl++, gCurTrack_36->f58, gCurTrack_36->f60);
        gPrimIATextures = 0;
        gSPDisplayList(gdl++, sTexCutoutFogDL);
        gSPDisplayList(gdl++, sPropDL);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_FOG);
    }
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

#define SX(n) (gScreenWidth / 2 - gScreenWidth * (n) / 512)
#define SY(n) (gScreenHeight * (n) / 240)

typedef struct {
    s32 *font0;         /* 0x00 */
    u8 pad4[0xC];
    s32 *f10;
    s32 *f14;
    s32 *f18;
    s32 *f1C;
    s32 *f20;
    s32 *f24;
    s32 *f28;
    s32 *f2C;
    s32 *f30;
    s32 *f34;
    s32 *f38;
    u8 pad3C[0x128 - 0x3C];
    u8 *f128;           /* 0x128 */
    Pic *f12C[5];       /* 0x12C */
    Pic *f140;          /* 0x140 */
    Pic *f144;          /* 0x144 */
} Res_5;

typedef struct {
    char name[0x1E];
    s16 f1E;
    u8 pad20[0x14];
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    char f44[3];
    char f47[3];
    char f4A[3];
    char f4D[3];
} Info_2;

extern Res_5 *gRaceTextObjs_2;
extern Info_2 *gCurTrack_37;

Gfx *hud_draw_track_intro(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3)
{
    char buf[0x40];
    s32 i;
    s32 c;
    s32 sel;
    s32 x;
    s32 y;
    u8 *p;
    Game *g;

    font_reset_queue();
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPLoadTLUT_pal256(gdl++, gRaceTextObjs_2->f140->pal);
    gdl = render_draw_image(gViewRects_2.x + 8, gViewRects_2.y + 3, gRaceTextObjs_2->f140->img, gdl, arg2, 1, 1, 3, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (gGame.mode == 1) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        for (i = 0; i < gSeason->cup->numRounds; i++) {
            gDPLoadTLUT_pal256(gdl++, gRaceTextObjs_2->f12C[i]->pal);
            p = gRaceTextObjs_2->f12C[i]->img;
            if (i != gSeason->round || (gFrameCounter & 0x20)) {
                font_text_create(gRaceTextObjs_2->font0, 0x40, 2, (i == gSeason->round) ? 7 : 2, (i != gSeason->round) ? 0x20000021 : 0x21);
                font_text_add(gRaceTextObjs_2->font0, gTracks_11[gSeason->cup->tracks[i]]);
                font_draw_text(gRaceTextObjs_2->font0, gViewRects_2.x + gViewRects_2.w - 0x69,
                              gViewRects_2.y + 0x15 + gRaceTextObjs_2->f128[0x21] * i);
            }
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - p[0] * 8 - 8,
                                gViewRects_2.y + 0xB + gRaceTextObjs_2->f128[0x21] * i, p, gdl, arg2, 1, 1, 3, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = gRaceTextObjs_2->f128 + 0x20;
        for (i = 0; i < gSeason->cup->numRounds; i++) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - p[0] * 8, gViewRects_2.y + 6 + p[1] * i,
                                p, gdl, arg2, 1, 1, 6, 0);
        }
        p = gRaceTextObjs_2->f128 + 0x18;
    } else {
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = gRaceTextObjs_2->f128 + 0x18;
    }
    if (gFrameCounter & 0x20) {
        y = 0xAA;
        if (gGame.mode == 1) y = 0x9E;
        font_text_create(gRaceTextObjs_2->f10, 0x40, 2, 4, 0x21);
        font_text_add(gRaceTextObjs_2->f10, "Press the A Button to Race");
        font_draw_text(gRaceTextObjs_2->f10, gScreenWidth / 2 - ((gGame.mode == 1) ? 0x52 : 0), y);
    }
    if (gGame.mode == 1) {
        font_text_create(gRaceTextObjs_2->f10, 0x40, 2, 7, 0x21);
        if (gSeason->cup - gCups == 4) {
            if (gSeason->round == gSeason->cup->numRounds - 1) {
                font_text_add(gRaceTextObjs_2->f10, D_800D8F58);
            } else {
                font_text_add(gRaceTextObjs_2->f10, D_800D8F84);
            }
        } else {
            font_text_add(gRaceTextObjs_2->f10, D_800D8FA4);
        }
        font_draw_text(gRaceTextObjs_2->f10, gScreenWidth / 2 - 0x52, 0xAA);
    }
    gdl = render_draw_image(gViewRects_2.x, gViewRects_2.y, p, gdl, arg2, 1, 1, 6, 0);
    p = gRaceTextObjs_2->f128 + 8;
    gdl = render_draw_image(gViewRects_2.x, gViewRects_2.y + gViewRects_2.h - p[1], p, gdl, arg2, 1, 1, 6, 0);
    if (gGame.mode == 0 && gGame.numPlayers >= 2 && (gFrameCounter & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, gRaceTextObjs_2->f144->pal);
        if (gGame.numLaps >= 2) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - 0x4B, gViewRects_2.y + gViewRects_2.h - 0xE,
                                gRaceTextObjs_2->f144->img, gdl, arg2, 1, 1, 3, 0);
        }
        if (gGame.numLaps < 6) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - 0x28, gViewRects_2.y + gViewRects_2.h - 0xE,
                                gRaceTextObjs_2->f144->img + 8, gdl, arg2, 1, 1, 3, 0);
        }
    }
    font_text_create(gRaceTextObjs_2->f10, 0x40, 1, 4, 0x20);
    font_text_add(gRaceTextObjs_2->f10, "Best Times:");
    font_draw_text(gRaceTextObjs_2->f10, SX(221), SY(197));
    font_text_create(gRaceTextObjs_2->f14, 0x40, 1, 4, 0x20);
    font_text_add(gRaceTextObjs_2->f14, "Best Lap:");
    font_draw_text(gRaceTextObjs_2->f14, SX(-7), SY(197));
    g = &gGame;
    font_text_create(gRaceTextObjs_2->f14, 0x40, 1, 4, 0x20);
    font_text_add(gRaceTextObjs_2->f14, "Number of Laps:");
    font_draw_text(gRaceTextObjs_2->f14, SX(-2), SY(214));
    font_text_create(gRaceTextObjs_2->f14, 0x40, 1, 2, (g->mode == 0 && g->numPlayers >= 2) ? 0x21 : 0x22);
    int_to_str(gGame.numLaps, buf);
    font_text_add(gRaceTextObjs_2->f14, buf);
    font_draw_text(gRaceTextObjs_2->f14, SX(-217) - ((gGame.mode == 0 && gGame.numPlayers >= 2) ? 0x1E : 0), SY(214));
    font_text_create(gRaceTextObjs_2->f1C, 0x40, 1, 2, 0x20);
    if (gGame.numLaps != gCurTrack_37->f1E) {
        font_text_create(gRaceTextObjs_2->f18, 0x40, 2, 2, 0x21);
        font_text_create(gRaceTextObjs_2->f28, 0x40, 2, 2, 0x21);
        font_text_create(gRaceTextObjs_2->f2C, 0x40, 2, 2, 0x21);
        font_text_add(gRaceTextObjs_2->f18, "Unavailable");
        font_draw_text(gRaceTextObjs_2->f18, SX(61), SY(196));
        font_text_add(gRaceTextObjs_2->f28, "Unavailable");
        font_draw_text(gRaceTextObjs_2->f28, SX(64), SY(205));
        font_text_add(gRaceTextObjs_2->f2C, "Unavailable");
        font_draw_text(gRaceTextObjs_2->f2C, SX(66), SY(214));
    } else {
        font_text_create(gRaceTextObjs_2->f18, 0x40, 2, 2, 0x20);
        font_text_create(gRaceTextObjs_2->f28, 0x40, 2, 2, 0x20);
        font_text_create(gRaceTextObjs_2->f2C, 0x40, 2, 2, 0x20);
        event_format_time(gCurTrack_37->f34, buf);
        font_text_add(gRaceTextObjs_2->f18, buf);
        font_draw_text(gRaceTextObjs_2->f18, SX(116), SY(196));
        event_format_time(gCurTrack_37->f38, buf);
        font_text_add(gRaceTextObjs_2->f28, buf);
        font_draw_text(gRaceTextObjs_2->f28, SX(119), SY(205));
        event_format_time(gCurTrack_37->f3C, buf);
        font_text_add(gRaceTextObjs_2->f2C, buf);
        font_draw_text(gRaceTextObjs_2->f2C, SX(121), SY(214));
    }
    event_format_time(gCurTrack_37->f40, buf);
    font_text_add(gRaceTextObjs_2->f1C, buf);
    font_draw_text(gRaceTextObjs_2->f1C, gScreenWidth / 2 + gScreenWidth * 111 / 512, SY(197));
    if (gGame.numLaps == gCurTrack_37->f1E) {
        font_text_create(gRaceTextObjs_2->f20, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, gCurTrack_37->f44, 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs_2->f20, buf);
        font_draw_text(gRaceTextObjs_2->f20, SX(7), SY(196));
        font_text_create(gRaceTextObjs_2->f30, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, gCurTrack_37->f47, 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs_2->f30, buf);
        font_draw_text(gRaceTextObjs_2->f30, SX(10), SY(205));
        font_text_create(gRaceTextObjs_2->f34, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, gCurTrack_37->f4A, 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs_2->f34, buf);
        font_draw_text(gRaceTextObjs_2->f34, SX(12), SY(214));
    }
    font_text_create(gRaceTextObjs_2->f24, 0x40, 1, 2, 0x22);
    __builtin_memcpy(buf, gCurTrack_37->f4D, 3);
    buf[3] = 0;
    font_text_add(gRaceTextObjs_2->f24, buf);
    font_draw_text(gRaceTextObjs_2->f24, SX(-220), SY(197));
    font_text_create(gRaceTextObjs_2->f38, 0x40, 4, 2, 0x20);
    font_text_add(gRaceTextObjs_2->f38, gCurTrack_37->name);
    font_draw_text(gRaceTextObjs_2->f38, SX(215), SY(29));
    return font_render(gdl);
}

void hud_draw_congratulations(u32 *param_1, Dyn *arg1, s32 arg2)
{
    s32 iVar3;
    s32 h;
    s32 iVar4;
    s32 *piVar1;
    char *pcVar2;
    s32 *pf;

    font_reset_queue();
    iVar4 = gScreenHeight_2 / 2 - 12;
    if ((gGame.flags & 0x200000) != 0) {
        iVar4 = gScreenHeight_2 / 2 - 24;
    }
    font_text_create((s32 *)*gRaceTextObjs, 0x40, 4, 7, 0x21);
    font_text_add((s32 *)*gRaceTextObjs, D_800D8FFC);
    font_draw_text((s32 *)*gRaceTextObjs, gScreenWidth / 2, iVar4);
    if ((gFrameCounter & 0x20) != 0) {
        font_text_create((s32 *)*gRaceTextObjs, 0x40, 5, 4, 0x21);
        font_text_add((s32 *)*gRaceTextObjs, D_800D9010);
        font_draw_text((s32 *)*gRaceTextObjs, *(volatile s32 *)&gScreenWidth / 2, *(volatile s32 *)&gScreenHeight_2 * 7 / 8);
    }
    font_text_create((s32 *)*gRaceTextObjs, 0x40, 4, 2, 0x21);
    if ((gGame.flags & 0x400000) != 0) {
        font_text_add((s32 *)*gRaceTextObjs, D_800D9024);
        goto LAB_after;
    }
    if ((gGame.flags & 0x100000) != 0) {
        font_text_add((s32 *)*gRaceTextObjs, D_800D9044);
        goto LAB_after;
    }
    if ((gGame.flags & 0x800000) != 0) {
        font_text_add((s32 *)*gRaceTextObjs, D_800D9060);
        goto LAB_after;
    }
    font_text_add((s32 *)*gRaceTextObjs, D_800D9078);
LAB_after:
    font_draw_text((s32 *)*gRaceTextObjs, gScreenWidth / 2, iVar4 + 0x18);
    pf = (s32 *)&gGame.flags;
    if ((pf[0] & 0x200000) != 0) {
        font_text_create((s32 *)*gRaceTextObjs, 0x40, 4, 4, 0x21);
        switch (pf[0x1D5]) {
        case 1:
            font_text_add((s32 *)*gRaceTextObjs, D_800D9090);
            break;
        case 2:
            font_text_add((s32 *)*gRaceTextObjs, D_800D90A8);
            break;
        case 3:
            font_text_add((s32 *)*gRaceTextObjs, D_800D90C8);
            break;
        case 4:
            font_text_add((s32 *)*gRaceTextObjs, D_800D90DC);
            break;
        case 5:
            font_text_add((s32 *)*gRaceTextObjs, D_800D90F4);
            break;
        default:
            goto LAB_end;
        }
    LAB_end:
        font_draw_text((s32 *)*gRaceTextObjs, gScreenWidth / 2, iVar4 + 0x30);
    }
    font_render(param_1);
}

void soccer_ball_load_model(void)
{
    D_800F18E8 = res_load(D_800E7728, D_800D06D0);
    D_800F18EC = res_load(D_800E7728, D_800D08E0);
    prim_alloc_dl(sPropDL_2, D_800F18E8, &D_800F18EC, 0xC);
}

void finishline_load_models(void)
{
    u32 pa;
    D_800EFD6C = res_load(D_800E7728, D_800D0DEC);
    D_800F18E8 = res_load(D_800E7728, D_800D0FB0);
    pa = (u32)&D_800F18EC;
    D_800F18EC = res_load(D_800E7728, D_800D10F8);
    D_800F16E0 = res_load(D_800E7728, D_800D0770);
    prim_alloc_dl(sPropDL_3, D_800F18E8, pa, 12);
}

void soccer_ball_free_model(void) {
    mem_free(sPropDL);
}

void finishline_free_models(void) {
    mem_free(sPropDL);
}

typedef struct {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xA0 - 0x54];
} Rider_6;

typedef struct {
    u32 magic;
    u32 sum;
    TrackRecord recs[24];
    u8 name[0x17];
    u8 f2BF;
    u8 f2C0;
    u8 f2C1;
    u8 pad2C2[2];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
    u8 f2C8;
    u8 pad2C9[7];
    u32 f2D0;
    u8 f2D4[0x804 - 0x2D4];
} Save;

extern Save gSaveData_5;

void save_init_defaults(void)
{
    TrackRecord *pd;
    TrackRecord *ps;
    Rider_6 *pc;
    u8 *q;
    u8 c;
    u32 sum;
    u32 *p;
    s32 i;

    mem_fill((u8 *)&gSaveData_5, 0, sizeof(Save));
    nes_init_saved_track(gSaveData_5.f2D4);
    i = 0;
    pc = gTracks;
    pd = gSaveData_5.recs;
    ps = gGame.records;
    do {
        i++;
        if (pc->flags & 0x200) {
            *pd = *ps;
            pd++;
            ps++;
        }
        pc++;
    } while (i < 27);
    c = '?';
    gSaveData_5.f2C0 = 1;
    gSaveData_5.f2BF = 0;
    i = 4;
    q = (u8 *)&gSaveData_5 + 4;
    do {
        q[0x2A8] = c;
        i--;
        q--;
    } while (i >= 0);
    sum = 0;
    gSaveData_5.f2C5 = 0x8C;
    gSaveData_5.f2C6 = 200;
    gSaveData_5.f2C7 = 0xA0;
    gSaveData_5.f2C4 = 1;
    gSaveData_5.f2D0 = 1;
    gSaveData_5.f2BF = 0x1F;
    gSaveData_5.f2C1 = 7;
    gSaveData_5.f2C8 = 0;
    gSaveData_5.f2C0 = 3;
    gSaveData_5.magic = 0x5748FB3A;
    p = (u32 *)&gSaveData_5;
    for (i = 0; i < 0x200; i++) {
        if (i != 1) {
            sum += p[i];
        }
    }
    sum ^= 0xFCB1121D;
    p[1] = sum;
}

typedef struct { u8 pad[0x2BF]; u8 bits; u8 cnt; } S19;
typedef struct { u8 pad[8]; u16 flags; u8 pad2[6]; } Ent_76;

extern S19 gSaveData_6;
extern Ent_76 gSpecialTracksItems[];
extern Ent_76 sTrackListItems[];

s32 save_update_unlocks(void)
{
    s32 saved = gGame.difficulty;
    s32 i, j, act, idx, k;
    s32 ret;
    u32 g;
    s32 hv;
    u32 bit;
    Ent_76 *e;

    for (i = 0; i < 4; i++) {
        k = i * 5;
        switch (i) {
        case 0:
        case 1:
            gGame.difficulty = 1;
            break;
        case 2:
            gGame.difficulty = 2;
            break;
        case 3:
            gGame.difficulty = 3;
            break;
        }
        act = 0;
        if (gGame.difficulty < gSaveData_6.cnt) {
            act = 1;
        } else if (gSaveData_6.cnt == gGame.difficulty) {
            if (i == 0 || ((gSaveData_6.bits >> (i - 1)) & 1)) act = 1;
        }
        j = 0;
        idx = k;
        do {
            if (act) {
                bit = 1 << idx;
                g = gUnlockedTracks | bit;
                e = (Ent_76 *)(idx << 4);
                e = (Ent_76 *)((u32)e + (u32)sTrackListItems);
                hv = e->flags & ~1;
                gUnlockedTracks = g;
                e->flags = hv;
            } else {
                bit = 1 << idx;
                bit = ~bit;
                e = (Ent_76 *)(idx << 4);
                e = (Ent_76 *)((u32)e + (u32)sTrackListItems);
                g = gUnlockedTracks & bit;
                hv = e->flags | 1;
                gUnlockedTracks = g;
                e->flags = hv;
            }
            j++;
            idx++;
        } while (j < 5);
    }

    ret = 0;
    if (D_800F1BC1 == 7) {
        ret = (gSpecialTracksItems[2].flags & 1) << 1;
        gSpecialTracksItems[2].flags &= ~1;
    } else {
        gSpecialTracksItems[2].flags |= 1;
    }

    gGame.difficulty = 1;
    if (!(gGame.difficulty < gSaveData_6.cnt || (gSaveData_6.cnt == gGame.difficulty && (gSaveData_6.bits & 4)))) gSpecialTracksItems[3].flags |= 1;

    gGame.difficulty = 2;
    if ((gGame.difficulty < gSaveData_6.cnt || (gSaveData_6.cnt == gGame.difficulty && (gSaveData_6.bits & 8)))) {
        if (gSpecialTracksItems[4].flags & 1) ret = 4;
        gSpecialTracksItems[4].flags &= ~1;
    } else {
        gSpecialTracksItems[4].flags |= 1;
    }

    gGame.difficulty = 3;
    if ((gGame.difficulty < gSaveData_6.cnt || (gSaveData_6.cnt == gGame.difficulty && (gSaveData_6.bits & 16)))) {
        if (gSpecialTracksItems[5].flags & 1) ret = 5;
        gSpecialTracksItems[5].flags &= ~1;
    } else {
        gSpecialTracksItems[5].flags |= 1;
    }

    gGame.difficulty = saved;
    return ret;
}

typedef struct { u8 pad0[0x34]; TrackRecord b; s32 flags; u8 pad54[0xA0 - 0x54]; } Rec_42;

typedef struct {
    u8 pad0[8];
    TrackRecord blk[24];
    u8 a8[5];
    u8 tbl[5][3];
    u8 pad2BC[4];
    u8 c0, c1;
    u8 pad2C2[3];
    u8 c5, c6, c7, c8;
    u8 pad2C9[7];
    s32 n;
} Save_2;

extern Save_2 gSaveData_7;
extern Rec_42 gTracks_12[];

void save_load(s32 arg0)
{
    s32 i, k;
    s32 j;
    u8 v;

    save_init_defaults();
    do { gUnlockedTracks = 0; } while (0);
    j = 0;
    for (i = 0; i < 27; i++) {
        if (i < gSaveData_7.n) {
            gUnlockedTracks |= 1 << i;
        }
        if (gTracks_12[i].flags & 0x200) {
            TrackRecord *d = &gTracks_12[i].b;
            TrackRecord *s = &gSaveData_7.blk[j];
            *d = *s;
            j++;
        }
    }
    gCustomRound = 0;
    for (i = 0; i < 4; i++) {
        gGame.padConfig[i] = (gSaveData_7.c8 >> i) & 3;
    }
    snd_set_volume(0, gSaveData_7.c6);
    snd_set_volume(1, gSaveData_7.c5);
    snd_set_volume(2, gSaveData_7.c7);
    for (i = 0; i < 5; i++) {
        for (k = 0; k < 3; k++) {
            gCups[i].progress[k] = gSaveData_7.tbl[0][i * 3 + k] >> 5;
            gCups[i].won[k] = gSaveData_7.tbl[0][i * 3 + k] & 0x1F;
        }
    }
    for (i = 0; i < 5; i++) {
        v = gSaveData_7.a8[i] & 0x1F;
        if (v == 0x1F) break;
        gCups[5].tracks[i] = v;
        gCups[5].laps[i] = gSaveData_7.a8[i] >> 5;
        gCups[5].numRounds++;
    }
    D_800CAAA1 = gSaveData_7.c1;
    gGame.difficulty = gSaveData_7.c0;
    save_update_unlocks();
}

void save_write(s32 x, s32 arg1)
{
    u8 buf[0x50];
    if (x == 1) {
        D_800F1BC1 = D_800CAAA1;
    }
}
