#include "game.h"

/* Draws the soccer ball (gSoccerBall) in every view: its rotation and position (metres to millimetres) make the
   model matrix, in sPropMtx's slot of the frame buffer `fb`, then each view's projection from `dyn` and the model. */
Gfx *soccer_ball_draw(Gfx *gdl, s32 dyn, s32 fb)
{
    s32 rot[10];
    s32 origin[3];
    s16 translate[32];
    s16 rotate[32];
    s32 i;
    s32 mtx;

    quat_to_mtx3i(rot, gSoccerBall.quat);
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;
    mtx_from_rot3i_pos(rot, (u16 *)origin, rotate);
    mtx_from_angles_pos_scale(translate, 0, 0, 0, (s32)(gSoccerBall.pos.x * 1000.0f), (s32)(gSoccerBall.pos.y * 1000.0f),
                  (s32)(gSoccerBall.pos.z * 1000.0f), 8);
    mtx = (u32)sPropMtx + (fb << 8);
    mtx_mul(rotate, translate, mtx);

    gSPMatrix(gdl++, mtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetRenderMode(gdl++, G_RM_ZB_OPA_SURF, 0);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPPipeSync(gdl++);

    for (i = 0; i < gNumViews; i++) {
        gdl = render_set_viewport_2(gdl, dyn, i);
        /* the view's projection matrix in dyn: adding 0x80000000 to its KSEG0 address gives the physical one */
        gSPMatrix(gdl++, dyn + i * 0x40 + 0x80000000, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPDisplayList(gdl++, sPropDL);
    }

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_ZB_OPA_SURF, 0);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    return gdl;
}

/* A track of gTracks (0xA0 bytes) as far as this file reads it. */
typedef struct TrackInfoView {
    /* 0x00 */ char name[0x1E];
    /* 0x1E */ s16 laps;            /* the race length the records are kept for */
    /* 0x20 */ u8 unk_20[0x34 - 0x20];
    /* 0x34 */ TrackRecord record;
    /* 0x50 */ u32 flags;           /* TRACK_FLAG_RECORDS */
    /* 0x54 */ u8 unk_54[4];
    /* 0x58 */ u16 fogMax[4];       /* by the number of screens */
    /* 0x60 */ u16 fogMin[4];
    /* 0x68 */ u8 unk_68[0xA0 - 0x68];
} TrackInfoView; /* size 0xA0 */

#define TRACK_FLAG_RECORDS 0x200    /* the track keeps records (gGame.records, the save data) */

/* N64 matrices (Mtx) as these functions declare them. */
typedef struct { s16 m[4][4][2]; } MtxQ;

extern TrackInfoView *gCurTrack_35;
extern MtxQ sPropMtx_2[][4];

/* Draws the finish line of a user track, across the racing line at its node 1, in view `view` (with the projection
   of `dyn`): the line itself (sFinishLineModel, IA textures, fogged), then the prop model (sPropDL) with cutout
   textures. The line is drawn twice. */
Gfx *finishline_draw_user_track(Gfx *gdl, Mtx *dyn, s32 fb, s32 view)
{
    MtxQ rotate;
    MtxQ translate;
    s16 heading;
    s32 length;
    MtxQ *mtx;

    math_to_polar((s32)(gPathNodes[1].lateralZ * 1024.0f), (s32)(gPathNodes[1].lateralX * 1024.0f), &heading, &length);
    mtx_from_angles_pos_scale(&rotate, 0, 0, heading, 0, 0, 0, 0x100);
    mtx_from_angles_pos_scale(&translate, 0, 0, 0, (s32)(gPathNodes[1].pos.x * 1000.0f),
                              (s32)(gPathNodes[1].pos.y * 1000.0f), (s32)(gPathNodes[1].pos.z * 1000.0f), 0x10);
    mtx = &sPropMtx_2[fb][view];
    mtx_mul(&rotate, &translate, mtx);

    gSPMatrix(gdl++, mtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)render_set_viewport_3(gdl, dyn, view);
    gSPMatrix(gdl++, &dyn[view + 4], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gdl++, G_RM_ZB_XLU_SURF, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, gCurTrack_35->fogMin[0], gCurTrack_35->fogMax[0]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    gdl = prim_build_dl(0, sFinishLineModel, gFinishLineTexSet_3, gdl);
    gdl = prim_build_dl(0, sFinishLineModel, gFinishLineTexSet_3, gdl);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
    gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
    /* fog near and far the other way round */
    gSPFogPosition(gdl++, gCurTrack_35->fogMax[0], gCurTrack_35->fogMin[0]);
    gPrimIATextures = 0;
    gSPDisplayList(gdl++, sTexCutoutFogDL);
    gSPDisplayList(gdl++, sPropDL);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, dyn, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

typedef struct { s16 m[32]; } MtxS_4;

typedef struct { Mtx m[4]; } MtxSet;

extern MtxSet sPropMtx_3[];
extern TrackInfoView *gCurTrack_36;

/* Draws the tutorial lesson's lines across the racing line (one view): at its start node, and unless the lesson has
   trick steps (flag 2) at its goal node too, with the prop model. sPropMtx_3[fb] holds the goal's matrix, then the
   start's. */
Gfx *finishline_draw_tutorial(Gfx *gdl, Mtx *dyn, s32 fb)
{
    MtxS_4 rotate;
    MtxS_4 translate;
    s16 heading;
    s32 length;
    PathNode *node;

    if (gTutorialLessons[gSeason->round].goalNode < gPathNodeCount) {
        {
            u32 base = (u32)gPathNodes;
            node = (PathNode *)(gTutorialLessons[gSeason->round].goalNode * sizeof(PathNode) + base);
        }
        math_to_polar(node->lateralZ * 1024.0f, node->lateralX * 1024.0f, &heading, &length);
        mtx_from_angles_pos_scale(&rotate, 0, 0, heading, 0, 0, 0, 0x100);
        mtx_from_angles_pos_scale(&translate, 0, 0, 0,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.x * 1000.0f,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.y * 1000.0f,
                      gPathNodes[gTutorialLessons[gSeason->round].goalNode].pos.z * 1000.0f, 0x10);
        mtx_mul(rotate.m, translate.m, (s16 *)&sPropMtx_3[fb].m[0]);
    }
    node = (PathNode *)(gTutorialLessons[gSeason->round].startNode * sizeof(PathNode) + (u32)gPathNodes);
    math_to_polar(node->lateralZ * 1024.0f, node->lateralX * 1024.0f, &heading, &length);
    mtx_from_angles_pos_scale(&rotate, 0, 0, heading, 0, 0, 0, 0x100);
    mtx_from_angles_pos_scale(&translate, 0, 0, 0,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.x * 1000.0f,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.y * 1000.0f,
                  gPathNodes[gTutorialLessons[gSeason->round].startNode].pos.z * 1000.0f, 0x10);
    mtx_mul(rotate.m, translate.m, (s16 *)&sPropMtx_3[fb].m[1]);

    gSPMatrix(gdl++, &sPropMtx_3[fb].m[1], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gdl = (Gfx *)render_set_viewport_4((u32 *)gdl, (s32)dyn, 0);
    gSPMatrix(gdl++, dyn + 4, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 255);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gdl++, G_RM_ZB_XLU_SURF, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, gCurTrack_36->fogMin[0], gCurTrack_36->fogMax[0]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gPrimIATextures = 1;
    gdl = prim_build_dl(0, sFinishLineModel, gFinishLineTexSet_3, gdl);
    if (gTutorialLessons[gSeason->round].flags & 2) {
        gPrimIATextures = 0;
    } else {
        gSPMatrix(gdl++, &sPropMtx_3[fb].m[0], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPPipeSync(gdl++);
        gdl = prim_build_dl(0, sFinishLineModel, gFinishLineTexSet_3, gdl);
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        gSPSetGeometryMode(gdl++, G_ZBUFFER);
        gDPSetBlendColor(gdl++, 0, 0, 0, 255);
        gDPSetAlphaCompare(gdl++, G_AC_NONE);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
        gDPSetFogColor(gdl++, gFogColor[0], gFogColor[1], gFogColor[2], 255);
        gSPFogPosition(gdl++, gCurTrack_36->fogMax[0], gCurTrack_36->fogMin[0]);
        gPrimIATextures = 0;
        gSPDisplayList(gdl++, sTexCutoutFogDL);
        gSPDisplayList(gdl++, sPropDL);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_FOG);
    }
    gSPMatrix(gdl++, dyn, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

/* Screen positions given for a 512x240 screen: x from the centre (positive to the left), y from the top. */
#define SX(n) (gScreenWidth / 2 - gScreenWidth * (n) / 512)
#define SY(n) (gScreenHeight * (n) / 240)

/* gCups: the five season cups, then the custom cup and the tutorial's levels. */
#define CUP_CHALLENGE   4   /* one race against one rider */
#define CUP_CUSTOM      5
#define CUP_TUTORIAL    6

/* Text alignments of font_text_create. */
#define TEXT_LEFT_MIDDLE   (TEXT_HALIGN_LEFT | TEXT_VALIGN_CENTER)
#define TEXT_CENTER_MIDDLE (TEXT_HALIGN_CENTER | TEXT_VALIGN_CENTER)
#define TEXT_RIGHT_MIDDLE  (TEXT_HALIGN_RIGHT | TEXT_VALIGN_CENTER)

extern TrackInfoView *gCurTrack_37;

/* Draws the track intro screen over the race view: the track's picture and name, the season's rounds (the current one
   blinking) and its goal, the number of laps (with arrows when it can be changed), the three best times and the best
   lap with their initials ("Unavailable" when the race is not the length the records are for), and the blinking
   "Press the A Button to Race". */
Gfx *hud_draw_track_intro(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    char buf[0x40];
    s32 i;
    s32 y;
    u8 *img;
    Game *game;

    font_reset_queue();
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, 0);
    gDPLoadTLUT_pal256(gdl++, gRaceTextObjs->introPic->pal);
    gdl = render_draw_image(gViewRects_2.x + 8, gViewRects_2.y + 3, gRaceTextObjs->introPic->img, gdl, fb, 1, 1, 3, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (gGame.mode == GAME_MODE_SEASON) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        for (i = 0; i < gSeason->cup->numRounds; i++) {
            gDPLoadTLUT_pal256(gdl++, gRaceTextObjs->roundPics[i]->pal);
            img = gRaceTextObjs->roundPics[i]->img;
            if (i != gSeason->round || (gFrameCounter & 0x20)) {
                font_text_create(gRaceTextObjs->text[0], 0x40, 2, (i == gSeason->round) ? 7 : 2,
                                 (i != gSeason->round) ? TEXT_GREYED | TEXT_CENTER_MIDDLE : TEXT_CENTER_MIDDLE);
                font_text_add(gRaceTextObjs->text[0], gTracks_11[gSeason->cup->tracks[i]]);
                font_draw_text(gRaceTextObjs->text[0], gViewRects_2.x + gViewRects_2.w - 0x69,
                              gViewRects_2.y + 0x15 + gRaceTextObjs->introImages[0x21] * i);
            }
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - img[0] * 8 - 8,
                                gViewRects_2.y + 0xB + gRaceTextObjs->introImages[0x21] * i, img, gdl, fb, 1, 1, 3, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        img = gRaceTextObjs->introImages + 0x20;
        for (i = 0; i < gSeason->cup->numRounds; i++) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - img[0] * 8, gViewRects_2.y + 6 + img[1] * i,
                                img, gdl, fb, 1, 1, 6, 0);
        }
        img = gRaceTextObjs->introImages + 0x18;
    } else {
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        img = gRaceTextObjs->introImages + 0x18;
    }
    if (gFrameCounter & 0x20) {
        y = 0xAA;
        if (gGame.mode == GAME_MODE_SEASON) y = 0x9E;
        font_text_create(gRaceTextObjs->text[4], 0x40, 2, 4, TEXT_CENTER_MIDDLE);
        font_text_add(gRaceTextObjs->text[4], "Press the A Button to Race");
        font_draw_text(gRaceTextObjs->text[4], gScreenWidth / 2 - ((gGame.mode == GAME_MODE_SEASON) ? 0x52 : 0), y);
    }
    if (gGame.mode == GAME_MODE_SEASON) {
        font_text_create(gRaceTextObjs->text[4], 0x40, 2, 7, TEXT_CENTER_MIDDLE);
        if (gSeason->cup - gCups == CUP_CHALLENGE) {
            if (gSeason->round == gSeason->cup->numRounds - 1) {
                font_text_add(gRaceTextObjs->text[4], sStrBeatOpponentFinal);
            } else {
                font_text_add(gRaceTextObjs->text[4], sStrBeatOpponent);
            }
        } else {
            font_text_add(gRaceTextObjs->text[4], sStrPlaceFirstOverall);
        }
        font_draw_text(gRaceTextObjs->text[4], gScreenWidth / 2 - 0x52, 0xAA);
    }
    gdl = render_draw_image(gViewRects_2.x, gViewRects_2.y, img, gdl, fb, 1, 1, 6, 0);
    img = gRaceTextObjs->introImages + 8;
    gdl = render_draw_image(gViewRects_2.x, gViewRects_2.y + gViewRects_2.h - img[1], img, gdl, fb, 1, 1, 6, 0);
    if (gGame.mode == GAME_MODE_EXHIBITION && gGame.numPlayers >= 2 && (gFrameCounter & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPLoadTLUT_pal256(gdl++, gRaceTextObjs->lapArrows->pal);
        if (gGame.numLaps >= 2) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - 0x4B, gViewRects_2.y + gViewRects_2.h - 0xE,
                                gRaceTextObjs->lapArrows->img, gdl, fb, 1, 1, 3, 0);
        }
        if (gGame.numLaps < 6) {
            gdl = render_draw_image(gViewRects_2.x + gViewRects_2.w - 0x28, gViewRects_2.y + gViewRects_2.h - 0xE,
                                gRaceTextObjs->lapArrows->img + 8, gdl, fb, 1, 1, 3, 0);
        }
    }
    font_text_create(gRaceTextObjs->text[4], 0x40, 1, 4, TEXT_LEFT_MIDDLE);
    font_text_add(gRaceTextObjs->text[4], "Best Times:");
    font_draw_text(gRaceTextObjs->text[4], SX(221), SY(197));
    font_text_create(gRaceTextObjs->text[5], 0x40, 1, 4, TEXT_LEFT_MIDDLE);
    font_text_add(gRaceTextObjs->text[5], "Best Lap:");
    font_draw_text(gRaceTextObjs->text[5], SX(-7), SY(197));
    game = &gGame;
    font_text_create(gRaceTextObjs->text[5], 0x40, 1, 4, TEXT_LEFT_MIDDLE);
    font_text_add(gRaceTextObjs->text[5], "Number of Laps:");
    font_draw_text(gRaceTextObjs->text[5], SX(-2), SY(214));
    font_text_create(gRaceTextObjs->text[5], 0x40, 1, 2,
                     (game->mode == GAME_MODE_EXHIBITION && game->numPlayers >= 2) ? TEXT_CENTER_MIDDLE
                                                                                   : TEXT_RIGHT_MIDDLE);
    int_to_str(gGame.numLaps, buf);
    font_text_add(gRaceTextObjs->text[5], buf);
    font_draw_text(gRaceTextObjs->text[5],
                   SX(-217) - ((gGame.mode == GAME_MODE_EXHIBITION && gGame.numPlayers >= 2) ? 0x1E : 0), SY(214));
    font_text_create(gRaceTextObjs->text[7], 0x40, 1, 2, TEXT_LEFT_MIDDLE);
    if (gGame.numLaps != gCurTrack_37->laps) {
        font_text_create(gRaceTextObjs->text[6], 0x40, 2, 2, TEXT_CENTER_MIDDLE);
        font_text_create(gRaceTextObjs->text[10], 0x40, 2, 2, TEXT_CENTER_MIDDLE);
        font_text_create(gRaceTextObjs->text[11], 0x40, 2, 2, TEXT_CENTER_MIDDLE);
        font_text_add(gRaceTextObjs->text[6], "Unavailable");
        font_draw_text(gRaceTextObjs->text[6], SX(61), SY(196));
        font_text_add(gRaceTextObjs->text[10], "Unavailable");
        font_draw_text(gRaceTextObjs->text[10], SX(64), SY(205));
        font_text_add(gRaceTextObjs->text[11], "Unavailable");
        font_draw_text(gRaceTextObjs->text[11], SX(66), SY(214));
    } else {
        font_text_create(gRaceTextObjs->text[6], 0x40, 2, 2, TEXT_LEFT_MIDDLE);
        font_text_create(gRaceTextObjs->text[10], 0x40, 2, 2, TEXT_LEFT_MIDDLE);
        font_text_create(gRaceTextObjs->text[11], 0x40, 2, 2, TEXT_LEFT_MIDDLE);
        event_format_time(gCurTrack_37->record.times[0], buf);
        font_text_add(gRaceTextObjs->text[6], buf);
        font_draw_text(gRaceTextObjs->text[6], SX(116), SY(196));
        event_format_time(gCurTrack_37->record.times[1], buf);
        font_text_add(gRaceTextObjs->text[10], buf);
        font_draw_text(gRaceTextObjs->text[10], SX(119), SY(205));
        event_format_time(gCurTrack_37->record.times[2], buf);
        font_text_add(gRaceTextObjs->text[11], buf);
        font_draw_text(gRaceTextObjs->text[11], SX(121), SY(214));
    }
    event_format_time(gCurTrack_37->record.bestLap, buf);
    font_text_add(gRaceTextObjs->text[7], buf);
    font_draw_text(gRaceTextObjs->text[7], gScreenWidth / 2 + gScreenWidth * 111 / 512, SY(197));
    if (gGame.numLaps == gCurTrack_37->laps) {
        font_text_create(gRaceTextObjs->text[8], 0x40, 2, 2, TEXT_RIGHT_MIDDLE);
        __builtin_memcpy(buf, gCurTrack_37->record.initials[0], 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs->text[8], buf);
        font_draw_text(gRaceTextObjs->text[8], SX(7), SY(196));
        font_text_create(gRaceTextObjs->text[12], 0x40, 2, 2, TEXT_RIGHT_MIDDLE);
        __builtin_memcpy(buf, gCurTrack_37->record.initials[1], 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs->text[12], buf);
        font_draw_text(gRaceTextObjs->text[12], SX(10), SY(205));
        font_text_create(gRaceTextObjs->text[13], 0x40, 2, 2, TEXT_RIGHT_MIDDLE);
        __builtin_memcpy(buf, gCurTrack_37->record.initials[2], 3);
        buf[3] = 0;
        font_text_add(gRaceTextObjs->text[13], buf);
        font_draw_text(gRaceTextObjs->text[13], SX(12), SY(214));
    }
    font_text_create(gRaceTextObjs->text[9], 0x40, 1, 2, TEXT_RIGHT_MIDDLE);
    __builtin_memcpy(buf, gCurTrack_37->record.initials[3], 3);
    buf[3] = 0;
    font_text_add(gRaceTextObjs->text[9], buf);
    font_draw_text(gRaceTextObjs->text[9], SX(-220), SY(197));
    font_text_create(gRaceTextObjs->text[14], 0x40, 4, 2, TEXT_LEFT_MIDDLE);
    font_text_add(gRaceTextObjs->text[14], gCurTrack_37->name);
    font_draw_text(gRaceTextObjs->text[14], SX(215), SY(29));
    return font_render(gdl);
}

/* The special tracks menu (gSpecialTracksItems): the items, and the codes save_update_unlocks returns for the one
   that has just opened (gGame.newUnlocks), which the congratulations screen announces. */
#define SPECIAL_TRACK_DESERT        0
#define SPECIAL_TRACK_STUNT_COURSE  1
#define SPECIAL_TRACK_NES           2   /* Original Excitebike */
#define SPECIAL_TRACK_SOCCER        3
#define SPECIAL_TRACK_HILL_CLIMB    4
#define SPECIAL_TRACK_EXCITE_3D     5

/* Draws the screen after a season's last race: "Congratulations!", what was achieved (gGame flags 0x400000 a new
   difficulty, 0x100000 a new round, 0x800000 the game finished) and, with flag 0x200000, the special track that has
   opened. */
void hud_draw_congratulations(u32 *gdl, Dyn *dyn, s32 fb)
{
    s32 y;
    s32 *game;

    font_reset_queue();
    y = gScreenHeight_2 / 2 - 12;
    if ((gGame.flags & 0x200000) != 0) {
        y = gScreenHeight_2 / 2 - 24;
    }
    font_text_create(gRaceTextObjs->text[0], 0x40, 4, 7, TEXT_CENTER_MIDDLE);
    font_text_add(gRaceTextObjs->text[0], sStrCongratulations);
    font_draw_text(gRaceTextObjs->text[0], gScreenWidth / 2, y);
    if ((gFrameCounter & 0x20) != 0) {
        font_text_create(gRaceTextObjs->text[0], 0x40, 5, 4, TEXT_CENTER_MIDDLE);
        font_text_add(gRaceTextObjs->text[0], sStrPressStartToExit);
        font_draw_text(gRaceTextObjs->text[0], *(volatile s32 *)&gScreenWidth / 2,
                       *(volatile s32 *)&gScreenHeight_2 * 7 / 8);
    }
    font_text_create(gRaceTextObjs->text[0], 0x40, 4, 2, TEXT_CENTER_MIDDLE);
    if ((gGame.flags & 0x400000) != 0) {
        font_text_add(gRaceTextObjs->text[0], sStrUnlockedDifficulty);
        goto draw;
    }
    if ((gGame.flags & 0x100000) != 0) {
        font_text_add(gRaceTextObjs->text[0], sStrUnlockedRound);
        goto draw;
    }
    if ((gGame.flags & 0x800000) != 0) {
        font_text_add(gRaceTextObjs->text[0], sStrFinishedGame);
        goto draw;
    }
    font_text_add(gRaceTextObjs->text[0], sStrFinishedRound);
draw:
    font_draw_text(gRaceTextObjs->text[0], gScreenWidth / 2, y + 0x18);
    /* gGame read through a word pointer: game[0] is gGame.flags, game[0x1D5] gGame.newUnlocks */
    game = (s32 *)&gGame.flags;
    if ((game[0] & 0x200000) != 0) {
        font_text_create(gRaceTextObjs->text[0], 0x40, 4, 4, TEXT_CENTER_MIDDLE);
        switch (game[0x1D5]) {
        case SPECIAL_TRACK_STUNT_COURSE:
            font_text_add(gRaceTextObjs->text[0], sStrStuntCourseUnlocked);
            break;
        case SPECIAL_TRACK_NES:
            font_text_add(gRaceTextObjs->text[0], sStrOriginalExcitebikeUnlocked);
            break;
        case SPECIAL_TRACK_SOCCER:
            font_text_add(gRaceTextObjs->text[0], sStrSoccerUnlocked);
            break;
        case SPECIAL_TRACK_HILL_CLIMB:
            font_text_add(gRaceTextObjs->text[0], sStrHillClimbUnlocked);
            break;
        case SPECIAL_TRACK_EXCITE_3D:
            font_text_add(gRaceTextObjs->text[0], sStrExcite3DUnlocked);
            break;
        default:
            goto done;
        }
    done:
        font_draw_text(gRaceTextObjs->text[0], gScreenWidth / 2, y + 0x30);
    }
    font_render(gdl);
}

/* Loads the soccer ball's model and textures and builds its display list (sPropDL). */
void soccer_ball_load_model(void)
{
    sPropModel = res_load(gResourceSet, sSoccerBallModelResId);
    sPropTextures = res_load(gResourceSet, sSoccerBallTexResId);
    prim_alloc_dl(sPropDL_2, sPropModel, &sPropTextures, 0xC);
}

/* Loads the finish line's model and textures and the prop drawn with it (its display list in sPropDL). */
void finishline_load_models(void)
{
    u32 textures;
    gFinishLineTexSet = res_load(gResourceSet, gFinishLineTexResId);
    sPropModel = res_load(gResourceSet, sFinishPropModelResId);
    textures = (u32)&sPropTextures;
    sPropTextures = res_load(gResourceSet, sFinishPropTexResId);
    sFinishLineModel = res_load(gResourceSet, sFinishLineModelResId);
    prim_alloc_dl(sPropDL_3, sPropModel, textures, 12);
}

/* Frees the prop's display list. */
void soccer_ball_free_model(void)
{
    mem_free(sPropDL);
}

void finishline_free_models(void)
{
    mem_free(sPropDL);
}

#define CUSTOM_CUP_END  0x1F    /* a track index that ends the custom cup's list */

/* Fills the save image with its defaults: the tracks' records from gGame.records, an empty custom cup, the volumes,
   stereo, and (in this build) everything open: top difficulty 3 with all its rounds done and the tutorial at 7,
   which opens Original Excitebike. Then the magic and the checksum. */
void save_init_defaults(void)
{
    TrackRecord *dst;
    TrackRecord *src;
    TrackInfoView *track;
    u8 *p;
    u8 empty;
    u32 sum;
    u32 *words;
    s32 i;

    mem_fill((u8 *)&gSaveData, 0, sizeof(SaveData));
    nes_init_saved_track(gSaveData.nesTrack);
    i = 0;
    track = gTracks;
    dst = gSaveData.records;
    src = gGame.records;
    do {
        i++;
        if (track->flags & TRACK_FLAG_RECORDS) {
            *dst = *src;
            dst++;
            src++;
        }
        track++;
    } while (i < TRACK_COUNT);
    /* '?' (0x3F): track CUSTOM_CUP_END, so all five races of the custom cup are empty */
    empty = '?';
    gSaveData.topDifficulty = 1;
    gSaveData.roundsDone = 0;
    i = 4;
    p = (u8 *)&gSaveData + 4;
    do {
        p[0x2A8] = empty; /* gSaveData.customRound[i] */
        i--;
        p--;
    } while (i >= 0);
    sum = 0;
    gSaveData.volumes[0] = 0x8C;
    gSaveData.volumes[1] = 200;
    gSaveData.volumes[2] = 0xA0;
    gSaveData.options = SAVE_OPT_STEREO;
    gSaveData.unlockedTrackCount = 1;
    gSaveData.roundsDone = 0x1F;
    gSaveData.tutorialProgress = 7;
    gSaveData.padConfigs = 0;
    gSaveData.topDifficulty = 3;
    gSaveData.magic = SAVE_MAGIC;
    words = (u32 *)&gSaveData;
    for (i = 0; i < SAVE_WORDS; i++) {
        if (i != 1) {
            sum += words[i];
        }
    }
    sum ^= SAVE_CHECKSUM_XOR;
    words[1] = sum;
}

/* Opens or locks the items of the track list and of the special tracks menu from the season progress in the save
   data, and returns the special track that has just opened (SPECIAL_TRACK_*) or 0. The track list opens in four
   groups of five tracks, at difficulty 1, 1, 2 and 3: a group is open below the top difficulty reached, and at it
   once the round before it is done. gGame.difficulty is borrowed for the comparisons and restored. */
s32 save_update_unlocks(void)
{
    s32 savedDifficulty = gGame.difficulty;
    s32 group, n, open, track, first;
    s32 newUnlock;
    u32 unlocked;
    s32 flags;
    u32 bit;
    ScreenItem *item;

    for (group = 0; group < 4; group++) {
        first = group * 5;
        switch (group) {
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
        open = 0;
        if (gGame.difficulty < gSaveData.topDifficulty) {
            open = 1;
        } else if (gSaveData.topDifficulty == gGame.difficulty) {
            if (group == 0 || ((gSaveData.roundsDone >> (group - 1)) & 1)) open = 1;
        }
        n = 0;
        track = first;
        do {
            if (open) {
                bit = 1 << track;
                unlocked = gUnlockedTracks | bit;
                item = (ScreenItem *)(track << 4);
                item = (ScreenItem *)((u32)item + (u32)sTrackListItems);
                flags = item->flags & ~SCREEN_ITEM_LOCKED;
                gUnlockedTracks = unlocked;
                item->flags = flags;
            } else {
                bit = 1 << track;
                bit = ~bit;
                item = (ScreenItem *)(track << 4);
                item = (ScreenItem *)((u32)item + (u32)sTrackListItems);
                unlocked = gUnlockedTracks & bit;
                flags = item->flags | SCREEN_ITEM_LOCKED;
                gUnlockedTracks = unlocked;
                item->flags = flags;
            }
            n++;
            track++;
        } while (n < 5);
    }

    newUnlock = 0;
    if (gSaveData.tutorialProgress == 7) {
        /* SPECIAL_TRACK_NES if it was locked */
        newUnlock = (gSpecialTracksItems[SPECIAL_TRACK_NES].flags & SCREEN_ITEM_LOCKED) << 1;
        gSpecialTracksItems[SPECIAL_TRACK_NES].flags &= ~SCREEN_ITEM_LOCKED;
    } else {
        gSpecialTracksItems[SPECIAL_TRACK_NES].flags |= SCREEN_ITEM_LOCKED;
    }

    /* soccer is only ever locked here */
    gGame.difficulty = 1;
    if (!(gGame.difficulty < gSaveData.topDifficulty
          || (gSaveData.topDifficulty == gGame.difficulty && (gSaveData.roundsDone & 4)))) {
        gSpecialTracksItems[SPECIAL_TRACK_SOCCER].flags |= SCREEN_ITEM_LOCKED;
    }

    gGame.difficulty = 2;
    if ((gGame.difficulty < gSaveData.topDifficulty
         || (gSaveData.topDifficulty == gGame.difficulty && (gSaveData.roundsDone & 8)))) {
        if (gSpecialTracksItems[SPECIAL_TRACK_HILL_CLIMB].flags & SCREEN_ITEM_LOCKED) {
            newUnlock = SPECIAL_TRACK_HILL_CLIMB;
        }
        gSpecialTracksItems[SPECIAL_TRACK_HILL_CLIMB].flags &= ~SCREEN_ITEM_LOCKED;
    } else {
        gSpecialTracksItems[SPECIAL_TRACK_HILL_CLIMB].flags |= SCREEN_ITEM_LOCKED;
    }

    gGame.difficulty = 3;
    if ((gGame.difficulty < gSaveData.topDifficulty
         || (gSaveData.topDifficulty == gGame.difficulty && (gSaveData.roundsDone & 16)))) {
        if (gSpecialTracksItems[SPECIAL_TRACK_EXCITE_3D].flags & SCREEN_ITEM_LOCKED) {
            newUnlock = SPECIAL_TRACK_EXCITE_3D;
        }
        gSpecialTracksItems[SPECIAL_TRACK_EXCITE_3D].flags &= ~SCREEN_ITEM_LOCKED;
    } else {
        gSpecialTracksItems[SPECIAL_TRACK_EXCITE_3D].flags |= SCREEN_ITEM_LOCKED;
    }

    gGame.difficulty = savedDifficulty;
    return newUnlock;
}

extern TrackInfoView gTracks_12[];

/* Applies the save data after resetting it to the defaults (save_init_defaults: nothing is read from a save in this
   build): the open tracks, the records, the controller layouts, the volumes, the cups' progress, the custom cup, the
   tutorial level and the difficulty. */
void save_load(s32 arg0)
{
    s32 i, k;
    s32 j;
    u8 track;

    save_init_defaults();
    do { gUnlockedTracks = 0; } while (0);
    j = 0;
    for (i = 0; i < TRACK_COUNT; i++) {
        if (i < gSaveData.unlockedTrackCount) {
            gUnlockedTracks |= 1 << i;
        }
        if (gTracks_12[i].flags & TRACK_FLAG_RECORDS) {
            TrackRecord *dst = &gTracks_12[i].record;
            TrackRecord *src = &gSaveData.records[j];
            *dst = *src;
            j++;
        }
    }
    gCups[CUP_CUSTOM].numRounds = 0;
    for (i = 0; i < 4; i++) {
        /* shifted by i, not 2 * i: the layouts of neighbouring controllers overlap */
        gGame.padConfig[i] = (gSaveData.padConfigs >> i) & 3;
    }
    snd_set_volume(0, gSaveData.volumes[1]);
    snd_set_volume(1, gSaveData.volumes[0]);
    snd_set_volume(2, gSaveData.volumes[2]);
    for (i = 0; i < 5; i++) {
        for (k = 0; k < 3; k++) {
            gCups[i].progress[k] = gSaveData.cupProgress[0][i * 3 + k] >> 5;
            gCups[i].won[k] = gSaveData.cupProgress[0][i * 3 + k] & 0x1F;
        }
    }
    for (i = 0; i < 5; i++) {
        track = gSaveData.customRound[i] & 0x1F;
        if (track == CUSTOM_CUP_END) break;
        gCups[CUP_CUSTOM].tracks[i] = track;
        gCups[CUP_CUSTOM].laps[i] = gSaveData.customRound[i] >> 5;
        gCups[CUP_CUSTOM].numRounds++;
    }
    gCups[CUP_TUTORIAL].progress[0] = gSaveData.tutorialProgress;
    gGame.difficulty = gSaveData.topDifficulty;
    save_update_unlocks();
}

/* Called when part of the save data has changed (SAVE_SECTION_*; `track` for SAVE_SECTION_RECORDS). In this build
   nothing is written: only the tutorial level is copied into gSaveData. The unused buffer is what is left of the
   frame of the removed code. */
void save_write(s32 section, s32 track)
{
    u8 buf[0x50];
    if (section == SAVE_SECTION_PROGRESS) {
        gSaveData.tutorialProgress = gCups[CUP_TUTORIAL].progress[0];
    }
}
