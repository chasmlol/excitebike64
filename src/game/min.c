#include "game.h"



void race_init_replay(void)
{
    s32 *piVar1;

    sDemoActive = 0;
    gDemoPlayback = 0;
    gReplayPlayback = 0;
    if (gGame.mode == 8 && (gGame.flags & 0x80000) != 0) {
        if ((gGame.flags & 0x80) != 0) {
            race_load_ghost_setup((s32)(gActiveReplay_2 + 0x1900));
        }
    }
    if ((gGame.flags & 0x81) == 0 && gGame.mode == 8) {
        gGhostRace = 1;
        if ((gGame.flags & 0x80000) != 0) {
            sGhostData = gActiveReplay_2 + 0x1900;
        } else {
            sGhostData = 0;
        }
    } else if ((gGame.flags & 0x8000) != 0) {
        gReplayPlayback = 1;
        sReplayRequest = 0;
        replay_play_start_2((s32)gActiveReplay_2);
    } else {
        if ((gGame.flags & 0x81) == 0 && gGame.mode == 9 &&
            gTutorialLessons[gSeason->round].demo != 0 && (gEvent.flags & 0x4000) == 0) {
            gEvent.flags = gEvent.flags & 0xffffbfff;
            gGhostRace = 0;
            gDemoPlayback = 1;
            sDemoActive = 1;
            if ((gGame.flags & 0x4000) != 0) {
                gActiveReplay_2 = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
            } else {
                piVar1 = res_load(gResourceSet, *gTutorialLessons[gSeason->round].demo);
                *(volatile s32 **)&gActiveReplay_2 = piVar1;
                gActiveReplaySize = piVar1[1] - piVar1[0];
                gActiveReplay_2 = piVar1 + 2;
                replay_play_start_2((s32)gActiveReplay_2);
            }
        } else {
            gGhostRace = 0;
        }
    }
    if (gGhostRace != 0) {
        gReplayRecording = 0;
        race_init_ghost_replay((s32)sGhostData);
        gCamTargetOverride = gGhostSlot ^ 1;
        if (gGhostRace != 0) {
            gRiderAlpha[gGhostSlot] = 0x80;
        }
    }
    if (gGame.mode == 9 || gGhostRace != 0 || gDemoPlayback != 0 ||
        (gGame.flags & 0x4001) != 0 || gGame.mode == 0xd ||
        gGame.mode == 0xe || (gGame.flags & 0x80) != 0) {
        gReplayRecording = 0;
    } else {
        gReplayRecording = 1;
    }
    if (gReplayRecording != 0 && gReplayPlayback == 0 && gActiveReplay_2 != 0) {
        replay_record_start((s32)gActiveReplay_2, gActiveReplaySize);
    }
}


void race_finish_replay(s32 param_1)
{
    Game *g = &gGame;
    if ((g->flags & 0x85) != 0) {
        if ((g->flags & 0x80) == 0) {
            return;
        }
        if (gRaceExitCode != 3) {
            return;
        }
    }
    if (gGhostRace == 0) {
        if (gDemoPlayback != 0) {
            gActiveReplay = 0;
            res_unload(&gResourceSet->handles[gTutorialLessons[gSeason->round].demo[0]]);
            goto T;
        }
        if (gRaceExitCode == 3 || g->mode == 9) goto T;
        if (g->mode < 10) goto F0;
        if (g->mode >= 15) goto CALL;
        if (g->mode < 13) goto CALL;
        goto T;
F0:
        if (g->mode == 2) goto T;
    }
CALL:
    replay_record_finish();
T:
    if (sReplayRequest == 0 && param_1 != 0) {
        race_load_frontend_code(1);
    }
}

void race_update_replay(void)
{
    if (gReplayRecording != 0) {
        if (gReplayPlayback == 0) {
            if (gGame.mode == 9 || gEvent.allFinished == 0) {
                replay_record_frame(gPlayerInputs);
            }
        } else if (gGame.mode == 9 || gEvent.allFinished == 0) {
            replay_play_frame(gPlayerInputs);
        }
        if (sReplayRequest != 0 && sClearFrames < 5) {
            if (gReplayPlayback == 0) {
                replay_record_finish();
            }
            race_exit();
            mem_fill(gPlayerInputsPrev_2, 0, 0x18);
            gReplayPlayback = 1;
            sReplayRequest = 0;
            race_init(0);
            replay_play_start(gActiveReplay);
            replay_play_frame(gPlayerInputs);
        }
    } else if (gGhostRace != 0) {
        if (gRiders[gGhostSlot ^ 1].race.finishPlace == -1) {
            if (gMirrorMode != 0) {
                gPlayerInputs[gGhostSlot ^ 1] ^= 0xC;
            }
            replay_record_frame(&gPlayerInputs[gGhostSlot ^ 1]);
            if (gMirrorMode != 0) {
                gPlayerInputs[gGhostSlot ^ 1] ^= 0xC;
            }
        }
        if (sGhostData != 0) {
            replay_play_frame(&gPlayerInputs[gGhostSlot]);
        }
    } else if (gDemoPlayback != 0 && sDemoActive != 0) {
        if ((gEvent.flags & 0x40) && replay_play_frame(gPlayerInputs) != 0) {
            bikes_free();
            decal_clear_all();
            mem_fill((u8 *)gRiders, 0, 0x3F00);
            tutorial_init_lesson(0);
            bikes_init();
            gEvent.flags = (gEvent.flags | 0x80) & ~0x2000;
            replay_play_start(gActiveReplay);
        }
    }
    decal_update_tire_tracks();
    tube_update_splashes();
}

typedef struct {
    u8 pad00[0x2C];
    s32 f2C;        /* 0x2C */
    u8 pad30[0x20];
    u32 f50;        /* 0x50 */
    u8 pad54[0x20];
    s16 h74;        /* 0x74 */
    s16 h76;        /* 0x76 */
    Ent1C *p78;     /* 0x78 */
    Ent1C *p7C;     /* 0x7C */
    s32 *p80;       /* 0x80 */
} Obj_6;

extern Obj_6 *gCurTrack_14;

void race_init(s32 arg)
{
    u16 *p;
    Ent1C *q;
    Obj_6 *o;
    s32 i;
    s32 v;

    if (!(gGame.flags & 0x81) && (gSaveOptions & 2) && gHasExpansionPak != 0
        && (gGame.numPlayers == 1 || gGame.mode == 8) && gCurTrack_14->f2C != 2
        && !(gGame.flags & 0x04040000)) {
        gViModeRequest = 2;
    } else {
        gViModeRequest = 1;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();

    if ((gGame.mode == 0 || gGame.mode == 1 || gGame.mode == 8) && (gCheatFlags & 2)
        && !(gGame.flags & 0x40001)) {
        gMirrorMode = 1;
    } else {
        gMirrorMode = 0;
    }
    gPodiumMode = 0;
    gTimeTrialPlayer = 0;
    gPauseMenu = 0;
    gPauseScreen.timer = -1;
    gPauseScreen.clearFrames = 0;
    gPauseScreen.loaded = 0;
    gSchedFrameLimit = 1;
    gGame.flags |= 0x10000;
    race_load_frontend_code(0);
    if (gReplayPlayback != 0) {
        gCamTargetOverride = gEvent.camTarget;
    } else {
        gCamTargetOverride = -1;
    }
    osWritebackDCacheAll();
    res_open_set(sStrRaceResource, &gResourceSet);
    if (gCurTrack_14 != NULL) {
        track_res_load_map(gCurTrack_14->p80);
    }
    p = (u16 *)track_res_get(gGidSpCam);
    if (p != NULL && !(gCurTrack_14->f50 & 2)) {
        o = gCurTrack_14;
        o->h76 = p[1];
        o->h74 = p[0];
        q = (Ent1C *)(p + 2);
        o->p78 = q;
        o->p7C = q + o->h74;
    } else {
        gCurTrack_14->h76 = 0;
        gCurTrack_14->h74 = 0;
        gCurTrack_14->p78 = NULL;
        gCurTrack_14->p7C = NULL;
    }
    if (gGame.flags & 0x40080) {
        font_load_menu_set(gResourceSet);
    } else if (!(gGame.flags & 1)) {
        font_load_race_set(gResourceSet);
    }

    if (gGame.mode == 4 || gGame.mode == 5) {
        gNumRiders = 6;
    } else if (gGame.mode == 9) {
        if (gTutorialLessons[gSeason->round].flags & 4) {
            gNumRiders = 2;
        } else {
            gNumRiders = 1;
        }
    } else if ((gGame.flags & 0x81) == 0x80) {
        gNumRiders = 1;
    } else if (gGame.mode == 0 && gGame.numPlayers == 2) {
        gNumRiders = gGame.twoPlayerCpuRiders + 2;
    } else if (gGame.mode == 13 || gGame.mode == 14) {
        gNumRiders = 1;
    } else if (gGame.flags & 1) {
        gNumRiders = 6;
        D_800E3704 = 0;
    } else if (gGame.mode == 6) {
        gNumRiders = 4;
        gGame.numPlayers = 1;
    } else if (gGame.mode == 2 || gGame.mode == 10) {
        gNumRiders = gGame.numPlayers;
    } else {
        gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
    }

    if (gGame.mode == 8 && !(gGame.flags & 1)) {
        gNumRiders = 1;
    }
    v = 0xFF;
    for (i = 7; i >= 0; i--) {
        gRiderAlpha[i] = v;
    }
    if (gGame.flags & 0x10) {
        gNumRiders = 6;
    }
    race_init_replay();
    if (gGame.flags & 0x40000) {
        gNumRiders = gGame.numPlayers;
        gGame.numPlayers = 1;
    }
    event_init(gNumRiders, gGame.numLaps);
    if (!(gGame.flags & 0x8081)) {
        race_randomize_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
        race_set_grid_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    }
    gPhysDeltaTime = 0.016666668f;
    engine_init_dial_vtx();
    mem_fill(gPlayerInputsPrev_2, 0, 0x18);
    palmod_init((gGame.flags >> 18) & 1);
    gTrackSections = 0;
    gTrackObjects = 0;
    gWaterModels = 0;
    gTerrainTexSet = 0;
    gObjectTexSet = 0;
    gAltTexSet = 0;
    gAnimTexSet = 0;
    gWaterTexSet = 0;
    render_init();
    palmod_register(gTerrainTexSet, 1);
    palmod_register(gObjectTexSet, 2);
    palmod_register(gAnimTexSet, 0);
    palmod_register(gWaterTexSet, 0);
    palmod_register(gAltTexSet, 0);
    if (gPodiumMode != 0 && gCurTrack_14->f2C == 0) {
        palmod_register(sStandTextures, 0);
    }
    ambient_load();
    announce_reset();
    particles_load_textures(gResourceSet);
    race_load_surface_table(gResourceSet, track_res_find(gGidMaterials));
    hacks_init();
    flash_init(gResourceSet);
    path_init(gResourceSet);
    cam_init(gResourceSet);
    shadow_init();
    map_init();
    particles_init(gSurfaceTable);
    rider_anims_load(gResourceSet);
    bikes_init();
    for (i = 0; i < 8; i++) {
        *gRiders[i].riderSkel = *gRiderSkelTemplate;
    }
    snd_race_init(gSurfaceTable);
    race_apply_volumes();
    decal_init();
    rain_init();
    snow_init();
    if (gGame.mode == 9) {
        event_stop_sounds_rumble();
    }
    if (gPodiumMode != 0) {
        podium_init();
        race_alloc_text_objs();
    }
    if (gGame.flags & 0x80) {
        gRacePhase = 0;
        gRacePhaseTimer = -1;
        race_intro_load();
    } else if (gGame.flags & 1) {
        gRacePhase = 5;
    } else {
        gRacePhase = 4;
        gRacePhaseTimer = -1;
    }
    gTimeTrialPlayer = 0;
    if (gGame.mode == 8 && !(gGame.flags & 0x80) && gGhostSlot == 0) {
        gTimeTrialPlayer = 1;
    }
}

void race_exit(void)
{
    u32 *r;
    u32 t;
    u32 f;

    rumble_stop_all();
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0 && (gGame.mode - 0xD) < 2) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    gGame.flags = gGame.flags & 0xFFFEFFFF;
    hacks_exit();
    decal_free();
    rider_anims_unload();
    r = &gGame.flags;
    snd_race_exit();
    path_free();
    particles_free();
    flash_free();
    render_free();
    instances_free();
    bikes_free();
    event_exit();
    shadow_free();
    res_close_set(gResourceSet);
    palmod_free();
    map_free();
    ambient_clear();
    rain_free();
    snow_free();
    if (gPodiumMode != 0) {
        podium_exit();
        race_free_text_objs();
    }
    t = gGame.flags & 0xFFFFBFFF;
    f = gGame.flags & 1;
    gGame.flags = t;
    if (f == 0) {
        font_unload_set();
    }
    race_finish_replay(1);
    gSchedFrameLimit = 0;
    gSchedMinRetraces = 1;
    if (gPodiumMode != 0) {
        gRaceExitCode = 7;
        r[0x13] = 0;
    }
}

void race_begin(void)
{
    while (sched_get_current_task() != 0) { }
    snd_load_engine_samples();
    if (gGhostRace != 0) {
        gNumViews = 1;
    } else {
        gNumViews = gGame.numPlayers;
    }
    gNumScreens = gNumViews;
    if (gGame.mode == 9) {
        path_init(gResourceSet);
    }
    if ((gGame.flags & 0x8085) == 0 && (gGame.mode != 1 || gSeason->round == 0)) {
        race_randomize_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
        race_set_grid_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    }
    gPauseMenu = 0;
    if ((gGame.flags & 0x81) == 0 && (gSaveOptions & 2) != 0 && gHasExpansionPak != 0 &&
        (gGame.numPlayers == 1 || gGame.mode == 8) &&
        gCurTrack_4->f2C != 2 && (gGame.flags & 0x04040000) == 0) {
        gViModeRequest = 2;
    } else {
        gViModeRequest = 1;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();
    race_apply_volumes();
    if (gCurTrack_4->f2C == 2) {
        fractal_reset();
    }
    cam_init(gResourceSet);
    bikes_init();
    gCamView = 0;
    if (0 < gNumViews) {
        do {
            cam_update();
            gCamView = gCamView + 1;
        } while (gCamView < gNumViews);
    }
    decal_init();
    rider_anims_load(gResourceSet);
    particles_init(gSurfaceTable);
    flash_init_2();
    shadow_init();
    map_init();
    if (gCurTrack_4->f2C == 1) {
        tube_reset_objects();
    }
    gDrawFlags = gDrawFlags | 0x800;
    event_hud_init();
    snd_race_init(gSurfaceTable);
    rain_init();
    snow_init();
    announce_reset();
    gTimeTrialPlayer = 0;
    if (gGame.mode == 8 && (gGame.flags & 0x80) == 0 && gGhostSlot == 0) {
        gTimeTrialPlayer = 1;
    }
    sky_reset_clip();
    if (gNumScreens >= 2) {
        gSchedMinRetraces = 2;
    } else {
        gSchedMinRetraces = 1;
    }
}

typedef union {
    u32 w;
    struct {
        u32 x : 4;
        u32 y : 4;
        u32 rest : 24;
    } b;
} Pad_2;

extern Pad_2 gPadInput_5;

void race_update_intro(void)
{
    s32 up, down, b10, b20, b40;
    u32 f;

    if ((gPadInput_5.w & 3) && ((gPadInput_5.w & 1) ? gPadInput_5.b.x : -gPadInput_5.b.x) >= 4) {
        gPadInput_5.w |= 0x800;
    } else if ((gPadInput_5.w & 3) && ((gPadInput_5.w & 1) ? gPadInput_5.b.x : -gPadInput_5.b.x) < -3) {
        gPadInput_5.w |= 0x1000;
    }
    if ((gPadInput_5.w & 0xC) && ((gPadInput_5.w & 8) ? ((s32)gPadInput_5.w >> 24 & 0xF) : -((s32)gPadInput_5.w >> 24 & 0xF)) >= 4) {
        gPadInput_5.w |= 0x4000;
    } else if ((gPadInput_5.w & 0xC) && ((gPadInput_5.w & 8) ? ((s32)gPadInput_5.w >> 24 & 0xF) : -((s32)gPadInput_5.w >> 24 & 0xF)) < -3) {
        gPadInput_5.w |= 0x2000;
    }
    f = gPadInput_5.w;
    up = (f & 0x2000) && !(gPadInputPrev & 0x2000);
    down = (f & 0x4000) && !(gPadInputPrev & 0x4000);
    b10 = (f & 0x10) && !(gPadInputPrev & 0x10);
    b20 = (f & 0x20) && !(gPadInputPrev & 0x20);
    b40 = (f & 0x40) && !(gPadInputPrev & 0x40);

    if (gGame.mode == 0 && gGame.numPlayers >= 2) {
        if (up && gGame.numLaps >= 2) gGame.numLaps--;
        if (down && gGame.numLaps < 6) gGame.numLaps++;
    }

    switch (gRacePhase_2) {
    case 0:
        if (gRacePhaseTimer != -1) {
            if (--gRacePhaseTimer == 0) gRacePhase_2 = 1;
        }
        break;
    case 2:
        if (gRacePhaseTimer == -3) {
            if (gRaceExitCode == 3) {
                race_intro_unload();
                gRacePhase_2 = 7;
                gRacePhaseTimer = 100;
                gCurGameState->mode = 3;
            } else {
                gRacePhase_2 = 3;
                gRacePhaseTimer = 2;
            }
        } else if (gRacePhaseTimer > 0) {
            if (--gRacePhaseTimer == 0) gRacePhaseTimer = -2;
        }
        break;
    case 3:
        if (gRacePhaseTimer == 0) {
            gRacePhase_2 = 4;
            gRacePhaseTimer = -1;
            race_finish_replay(0);
            cam_free();
            bikes_free();
            decal_free();
            rider_anims_unload();
            particles_free();
            flash_free();
            shadow_free();
            snd_race_exit();
            map_free();
            rain_free();
            snow_free();
            if (gGame.mode == 9) path_free();
            if (gDrawFlags & 0x800) event_hud_free();
            race_intro_unload();
            gGame.flags &= ~0x80;
            gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
            if (gGame.mode == 8 && !(gGame.flags & 1)) {
                gNumRiders = 1;
            } else if (gGame.mode == 0 && gGame.numPlayers == 2) {
                gNumRiders = gGame.twoPlayerCpuRiders + 2;
            }
            race_init_replay();
            event_init(gNumRiders, gGame.numLaps);
            race_begin();
            if (gGame.mode == 9) event_stop_sounds_rumble();
        }
        break;
    case 1:
        if (b20 && (gGame.mode != 1 || gSeason->round == 0)) {
            snd_play_menu_sound(2);
            gRacePhase_2 = 2;
            gRacePhaseTimer = 20;
            gRaceExitCode = 3;
            gGame.flags &= ~4;
        } else if (b10 || b40) {
            snd_play_menu_sound(1);
            gRaceExitCode = 6;
            gRacePhase_2 = 2;
            gRacePhaseTimer = 20;
        }
        break;
    }
}

void race_intro_load(void)
{
    s32 i;
    s32 r;

    gRaceTextObjs = mem_alloc_dbg(0x148, 0, sStrMinFile, 0x52C);
    i = 0;
    do {
        gRaceTextObjs->text[i] = font_text_create(NULL, 0x40, 1, 2, 0x20);
        i = i + 1;
    } while (i < 0x4a);
    r = res_load(gResourceSet, sIntroImagesResId);
    gRaceTextObjs->introImages = (u8 *)r;
    r = res_load(gResourceSet, sIntroPicResId);
    gRaceTextObjs->introPic = (Pic *)r;
    if (gGame.mode == 1) {
        i = 0;
        do {
            r = res_load(gResourceSet, *(s32 *)sIntroTrackPicResIds[gSeason->cup->tracks[i]]);
            gRaceTextObjs->roundPics[i] = (Pic *)r;
            i = i + 1;
        } while (i < 5);
    }
    r = res_load(gResourceSet, sLapArrowsResId);
    gRaceTextObjs->lapArrows = (Pic *)r;
}

void race_intro_unload(void) {
    s32 i;

    for (i = 0; i < 0x4A; i++) {
        font_text_destroy(gRaceTextObjs->text[i]);
        mem_free(gRaceTextObjs->text[i]);
    }
    res_unload(&gResourceSet->handles[sIntroImagesResId]);
    res_unload(&gResourceSet->handles[sIntroPicResId]);
    res_unload(&gResourceSet->handles[sLapArrowsResId]);
    if (gGame.mode == 1) {
        for (i = 0; i < 5; i++) {
            res_unload(&gResourceSet->handles[*sIntroTrackPicResIds_2[gSeason->cup->tracks[i]]]);
        }
    }
    mem_free(gRaceTextObjs);
    gRaceTextObjs = NULL;
    if (gRaceExitCode != 3) {
        gGame.flags &= ~0x80;
    }
    font_load_race_set(gResourceSet);
}

#define PAD (gGame.playerPad[gPausePlayer])
#define PRESSED(m) ((gPadInput_2[PAD] & (m)) && !(gPadInputPrev_2[PAD] & (m)))

void race_pause_update_options(void) {
    s32 s0;
    s32 old;
    s32 b;

    if (sOptionsPreviewDelay != 0) {
        sOptionsPreviewDelay--;
    }
    if (++gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    if (((gPadInput_2[PAD] >> 24) & 0xF) < 6) {
        gPadInput_2[PAD] &= ~0xC;
    }
    if ((s32)((u32)gPadInput_2[PAD] >> 28) < 6) {
        gPadInput_2[PAD] &= ~0x3;
    }
    if (gPadInput_2[PAD] & 4) {
        gPadInput_2[PAD] |= 0x2000;
    }
    if (gPadInput_2[PAD] & 8) {
        gPadInput_2[PAD] |= 0x4000;
    }
    if (gPadInput_2[PAD] & 1) {
        gPadInput_2[PAD] |= 0x800;
    }
    if (gPadInput_2[PAD] & 2) {
        gPadInput_2[PAD] |= 0x1000;
    }
    if (PRESSED(0x20)) {
        snd_set_volume(0, 0);
        snd_set_volume(1, 0);
        snd_set_volume(2, 0);
        save_write(5, 0);
        gPauseMenu = 1;
        gPauseScreen.timer = 3;
        sfx_stop_all();
        tunes_stop_speech();
        return;
    }
    if (PRESSED(0x10)) {
        switch (gOptionsCursor) {
        case 0:
            break;
        case 3:
            snd_play_menu_sound(1);
            gSaveData.f2C4 ^= 1;
            return;
        case 1:
            sfx_play(sOptionsTestSfx, 0x2B11, gSaveSfxVolume, 7, 1, 1);
            return;
        case 2:
            if (sOptionsPreviewDelay == 0) {
                tunes_queue_speech(0x9B);
                sOptionsPreviewDelay = 0x34;
            }
            return;
        case 4:
            snd_play_menu_sound(3);
            return;
        }
        return;
    }
    old = gOptionsCursor;
    if (PRESSED(0x1000)) {
        if (old < 3) {
            gOptionsCursor = old + 1;
        } else {
            gOptionsCursor = old + 2;
        }
        if (gOptionsCursor >= 4) {
            gOptionsCursor = 0;
        }
    }
    if (PRESSED(0x800)) {
        if (gOptionsCursor < 4) {
            gOptionsCursor = gOptionsCursor - 1;
        } else {
            gOptionsCursor = gOptionsCursor - 2;
        }
        if (gOptionsCursor < 0) {
            gOptionsCursor = 3;
        }
    }
    if (old != gOptionsCursor) {
        switch (old) {
        case 0:
        case 1:
            break;
        case 2:
            tunes_stop_speech();
            break;
        }
        switch (gOptionsCursor) {
        case 0:
            break;
        case 2:
            tunes_queue_speech(0x9B);
            sOptionsPreviewDelay = 0x34;
            break;
        case 1:
            sfx_play(sOptionsTestSfx, 0x2B11, gSaveSfxVolume, 7, 1, 1);
            break;
        }
    }
    if (gOptionsCursor < 3) {
        if (PRESSED(0x2000)) {
            s0 = -8;
        } else if (PRESSED(0x4000)) {
            s0 = 8;
        } else {
            b = gPadInput_2[PAD];
            if (b & 0xC) {
                if (b & 8) {
                    s0 = ((b >> 24) & 0xF) / 3;
                } else {
                    s0 = -((b >> 24) & 0xF) / 3;
                }
            } else {
                s0 = 0;
            }
        }
        if (s0 != 0 && sOptionsPreviewDelay == 0) {
            switch (gOptionsCursor) {
            case 1:
                sfx_play(sOptionsTestSfx, 0x2B11, gSaveSfxVolume, 7, 1, 1);
                sOptionsPreviewDelay = 0x28;
                break;
            case 2:
                tunes_queue_speech(0x9B);
                sOptionsPreviewDelay = 0x34;
                break;
            }
        }
        if (s0 < 0) {
            switch (gOptionsCursor) {
            case 1:
                if (gSaveData.f2C6 >= -s0) {
                    gSaveData.f2C6 += s0;
                } else {
                    gSaveData.f2C6 = 0;
                }
                break;
            case 0:
                if (gSaveData.f2C5 >= -s0) {
                    gSaveData.f2C5 += s0;
                } else {
                    gSaveData.f2C5 = 0;
                }
                break;
            case 2:
                if (gSaveData.f2C7 >= -s0) {
                    gSaveData.f2C7 += s0;
                } else {
                    gSaveData.f2C7 = 0;
                }
                break;
            }
        } else {
            s32 v;
            switch (gOptionsCursor) {
            case 1:
                v = gSaveData.f2C6 + s0;
                if (v >= 0x100) {
                    gSaveData.f2C6 = 0xFF;
                } else {
                    gSaveData.f2C6 = v;
                }
                break;
            case 0:
                v = gSaveData.f2C5 + s0;
                if (v >= 0x100) {
                    gSaveData.f2C5 = 0xFF;
                } else {
                    gSaveData.f2C5 = v;
                }
                break;
            case 2:
                v = gSaveData.f2C7 + s0;
                if (v >= 0x100) {
                    gSaveData.f2C7 = 0xFF;
                } else {
                    gSaveData.f2C7 = v;
                }
                break;
            }
        }
    }
    snd_set_volume(0, 0);
    snd_set_volume(1, 0);
    snd_set_volume(2, 0);
    switch (gOptionsCursor) {
    case 1:
        snd_set_volume(0, gSaveSfxVolume);
        break;
    case 0:
        snd_set_volume(1, gSaveVolumes_2);
        break;
    case 2:
        snd_set_volume(2, gSaveSpeechVolume);
        break;
    }
    if (gOptionsCursor != old) {
        snd_play_menu_sound(0);
    }
}

void race_pause_update_controller(void)
{
    s32 cur;
    s32 n;

    if (((gPadInput_3[gGame.playerPad[gPausePlayer]] & 0x20) != 0) &&
        ((gPadInputPrev_3[gGame.playerPad[gPausePlayer]] & 0x20) == 0)) {
        gPauseMenu = 1;
        gPauseScreen.timer = 3;
        save_write(5, 0);
    } else {
        u32 *t30 = gPadInput_3;
        cur = gGame.playerPad[gPausePlayer];
        if (((t30[cur] & 0x10) != 0) && ((gPadInputPrev_3[cur] & 0x10) == 0)) {
            n = gGame.padConfig[cur] + 1;
            gGame.padConfig[cur] = n;
            if (3 < (s8)n) {
                gGame.padConfig[gGame.playerPad[gPausePlayer]] -= 4;
            }
            snd_play_menu_sound(0);
        }
    }
}

#define PAD_2 (gGame.playerPad[gPausePlayer])
#define PRESSED_2(m) ((gPadInput_3[PAD_2] & (m)) && !(gPadInputPrev_3[PAD_2] & (m)))

typedef struct { u32 w[6]; } Pad24;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x28];
    u16 f58[4];
    u16 f60[4];
} Cfg64;

typedef struct {
    u8 pad0[0x2C4];
    u8 b2C4;
    u8 b2C5;
    u8 b2C6;
    u8 b2C7;
} Opt_2;

typedef struct {
    s32 *a;
    s32 *b;
    s32 *w;
    s32 *h;
    s32 *c;
    void *d;
} Cfg_5;

extern Cfg64 *gCurTrack_15;
extern Pad24 gPlayerInputs_3;
extern Pad24 gPlayerInputsPrev_4;
extern Pad24 D_800E3750;
extern Pad24 D_800E3718;
extern Opt_2 gSaveData_3;

void race_update(u32 *arg)
{
    Cfg_5 cfg;
    Pad16 buf;
    Pad16 spare;
    s32 w;
    s32 h;
    s32 flag;
    s32 i;
    s32 j;
    s32 t;

    flag = 0;
    if ((arg[0] >> 28) == 3 && D_800C8CD0 != 0) {
        D_800C29DC = 0x80020000;
    }
    buf = *(Pad16 *)arg;
    mem_fill((u8 *)&buf, 0, 0x10);
    for (j = 0; j < gGame.numPlayers; j++) {
        buf.w[j] = arg[gGame.playerPad[j]];
    }
    mem_fill((u8 *)&gPlayerInputs_3, 0, 0x18);
    *(Pad16 *)&gPlayerInputs_3 = buf;
    for (i = 0; i < 4; i++) {
        gPlayerInputs_3.w[i] = bike_remap_buttons(gPlayerInputs_3.w[i], i);
    }
    for (i = 0; i < 4; i++) {
        if (gPlayerInputs_3.w[i] & 0x2000) gPlayerInputs_3.w[i] |= 0x0F000004;
        if (gPlayerInputs_3.w[i] & 0x4000) gPlayerInputs_3.w[i] |= 0x0F000008;
        if (gPlayerInputs_3.w[i] & 0x800) gPlayerInputs_3.w[i] |= 0xF0000001;
        if (gPlayerInputs_3.w[i] & 0x1000) gPlayerInputs_3.w[i] |= 0xF0000002;
        if (gPlayerInputs_3.w[i] & 4) gPlayerInputs_3.w[i] &= ~8;
        if (gPlayerInputs_3.w[i] & 1) gPlayerInputs_3.w[i] &= ~2;
        gPlayerInputs_3.w[i] &= ~0x7800;
    }
    for (i = 0; i < 4; i++) {
        t = gRaceFrame - gRiders[i].landingFrame + 4;
        j = 0;
        if (gRiders[i].roughLanding) {
            t = t * 2 / 3;
        }
        if (t < gRiders[i].landingImpact / 20) j = 1;
        if (gTireTrackTimers[i] != 0 && !(gFrameCounter & 7)) j = 1;
        if (gRiders[i].rearWheel.surface != NULL && gRiders[i].rearWheel.surface->sfxId != 0 &&
            gRiders[i].speed > 8.0f && gFrameCounter % 6 == 0) j = 1;
        if (gPauseMenu != 0) j = 0;
        if (gRiders[i].isCpu != 0) j = 0;
        if (gGhostRace != 0 && i == gGhostSlot) j = 0;
        if (gReplayPlayback != 0) j = 0;
        if (gGame.fadeState != 3) j = 0;
        if (gGame.flags & 1) j = 0;
        if (gGame.mode == 9 && gEvent.state == 0xE) j = 0;
        t = gGame.playerPad[i];
        if (j) rumble_start(t, 1);
    }
    shatter_update();
    snow_update();
    ambient_update();
    {
        s32 *tab = gLoDetailThresh;
        s32 *tab2 = gVisibleThresh;
        w = gCurTrack_15->f58[gNumViews - 1];
        h = gCurTrack_15->f60[gNumViews - 1];
        cfg.a = &tab[gNumViews - 1];
        cfg.b = &tab2[gNumViews - 1];
    }
    cfg.w = &w;
    cfg.h = &h;
    cfg.c = &gDebugRiderCount;
    cfg.d = &gSfxVolume;
    if (gPauseMenu == 2 || gPauseMenu == 3) {
        if (gPauseScreen.timer == 0 && gPauseScreen.loaded == 0) {
            mem_free(gZBuffer);
            gPauseScreen.loaded = 1;
            if (gPauseMenu == 2) {
                font_load_menu_set(gResourceSet);
            }
            res_open_set("guires", &gGuiRes);
            for (j = 0; j < 0x30; j++) {
                gGame.texts[j] = font_text_create(0, 0x40, 4, 2, 0x20);
            }
            if (gPauseMenu == 2) {
                gPauseScreen.background = res_load(gResourceSet, sControlsBgResId);
            } else {
                gPauseScreen.volumeBar = res_load(gResourceSet, sVolumeBarResId);
                gPauseScreen.background = res_load(gGuiRes, gMenuBackgroundResId);
                gPauseScreen.cursorImg = res_load(gGuiRes, gCursorResId);
                gPauseScreen.cursorPal = res_load(gGuiRes, gCursorPalResId);
                gPauseScreen.title = res_load(gGuiRes, sOptionsTitleResId);
            }
            gPauseScreen.timer = -1;
            while (sched_get_current_task() != 0) {
            }
            gViModeRequest = 1;
            while (gViMode != gViModeRequest) {
            }
            render_update_screen_layout();
        }
    } else if (gPauseMenu == 1 && gPauseScreen.timer == 0 && gPauseScreen.loaded != 0) {
        gPauseScreen.loaded = 0;
        gPauseScreen.clearFrames = 2;
        for (j = 0; j < 0x30; j++) {
            font_text_destroy(gGame.texts[j]);
            mem_free(gGame.texts[j]);
        }
        res_close_set(gGuiRes);
        res_unload(&gResourceSet->handles[sControlsBgResId]);
        res_unload(&gResourceSet->handles[D_800D1080]);
        res_unload(&gResourceSet->handles[D_800D0724]);
        res_unload(&gResourceSet->handles[sVolumeBarResId]);
        font_unload_set();
        gZBuffer = mem_alloc_at_dbg(gZBuffer, gFbAllocWidth * gFbAllocHeight * 2, "min.c", 0x799);
        if (gZBuffer == 0) {
            mem_report_2();
            while (1) {
            }
        }
        if (!(gGame.flags & 0x81) && (gSaveData_3.b2C4 & 2) && gHasExpansionPak != 0 &&
            (gGame.numPlayers == 1 || gGame.mode == 8) && gCurTrack_15->f2C != 2 &&
            !(gGame.flags & 0x04040000)) {
            gViModeRequest = 2;
        } else {
            gViModeRequest = 1;
        }
        while (gViMode != gViModeRequest) {
        }
        render_update_screen_layout();
        font_load_race_set(gResourceSet);
        gPauseScreen.timer = -1;
    }
    if (gGame.mode != 9 && !(gGame.flags & 0x8010) && (gPadInput_3[0] & 0x400) &&
        !(gPadInputPrev_3[0] & 0x400)) {
        gHudLevel = (gHudLevel + 1) & 3;
    }
    particles_emit_bike_dirt();
    tube_animate_textures();
    if (!(gGame.flags & 0x80)) {
        if (gReplayPlayback != 0 && gRacePhase_2 == 5 &&
            ((gPlayerInputs_3.w[0] | gPlayerInputs_3.w[1] | gPlayerInputs_3.w[2] | gPlayerInputs_3.w[3]) & 0x50)) {
            gRacePhase_2 = 6;
            gRacePhaseTimer = -1;
            gRaceExitCode = 2;
            gGame.flags &= ~4;
            snd_play_menu_sound(1);
            return;
        }
    } else {
        race_update_intro();
    }

    if (gPauseMenu == 2) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            race_pause_update_controller();
        }
    } else if (gPauseMenu == 3) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            race_pause_update_options();
        }
    } else if (gPauseMenu == 6) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            if (PRESSED_2(0x20)) {
                gPauseMenu = 0;
                snd_set_volume(0, gSaveData_3.b2C6);
                snd_set_volume(1, gSaveData_3.b2C5);
                snd_set_volume(2, gSaveData_3.b2C7);
                snd_play_menu_sound(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                snd_play_menu_sound(1);
                flag = 1;
                switch (gPauseMenuCursor) {
                case 0:
                    gPauseMenu = 0;
                    snd_set_volume(0, gSaveData_3.b2C6);
                    snd_set_volume(1, gSaveData_3.b2C5);
                    snd_set_volume(2, gSaveData_3.b2C7);
                    break;
                case 1:
                    bikes_free();
                    mem_fill((u8 *)gRiders, 0, 0x3F00);
                    bikes_init();
                    gPauseMenu = 0;
                    gEvent.flags |= 0x80;
                    snd_set_volume(0, gSaveData_3.b2C6);
                    snd_set_volume(1, gSaveData_3.b2C5);
                    snd_set_volume(2, gSaveData_3.b2C7);
                    break;
                case 2:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gRaceExitCode = 8;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                gPauseMenuCursor = (gPauseMenuCursor + 2) % 3;
                snd_play_menu_sound(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                gPauseMenuCursor = (gPauseMenuCursor + 1) % 3;
                snd_play_menu_sound(0);
            }
        }
        if (gPauseMenu == 0) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == 5) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            if (PRESSED_2(0x20) && gTutorialMenuForced_2 == 0 && gTutorialResult == 0) {
                gPauseMenu = 0;
                snd_set_volume(0, gSaveData_3.b2C6);
                snd_set_volume(1, gSaveData_3.b2C5);
                snd_set_volume(2, gSaveData_3.b2C7);
                snd_play_menu_sound(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                snd_play_menu_sound(1);
                flag = 1;
                switch (gPauseMenuCursor) {
                case 4:
                    gEvent.tutorialAction = 3;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 3:
                    gEvent.tutorialAction = 4;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 1:
                    gEvent.tutorialAction = 7;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 0:
                    gEvent.tutorialAction = 6;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 2:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gRaceExitCode = 2;
                    gGame.flags &= ~4;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                do {
                    gPauseMenuCursor = (gPauseMenuCursor + 4) % 5;
                } while ((gPauseMenuCursor == 3 && tutorial_next_lesson_unlocked() == 0) ||
                         (gPauseMenuCursor == 1 && tutorial_lesson_has_demo() == 0) ||
                         (gPauseMenuCursor == 4 && tutorial_has_prev_lesson() == 0));
                snd_play_menu_sound(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                do {
                    gPauseMenuCursor = (gPauseMenuCursor + 1) % 5;
                } while ((gPauseMenuCursor == 3 && tutorial_next_lesson_unlocked() == 0) ||
                         (gPauseMenuCursor == 1 && tutorial_lesson_has_demo() == 0) ||
                         (gPauseMenuCursor == 4 && tutorial_has_prev_lesson() == 0));
                snd_play_menu_sound(0);
            }
            if (gPauseMenu == 0) {
                sfx_unmute();
            }
        }
        if (gPauseMenu == 0) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == 1) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            if (PRESSED_2(0x20)) {
                snd_set_volume(0, gSaveData_3.b2C6);
                snd_set_volume(1, gSaveData_3.b2C5);
                snd_set_volume(2, gSaveData_3.b2C7);
                gPauseMenu = 0;
                snd_play_menu_sound(2);
            } else if (PRESSED_2(0x10) || PRESSED_2(0x40)) {
                snd_play_menu_sound(1);
                flag = 1;
                switch (gPauseMenuCursor) {
                case 0:
                    gPauseMenu = 2;
                    sOptionsPreviewDelay = 0;
                    gOptionsCursor = gGame.padPlayer[PAD_2];
                    gPauseScreen.timer = 3;
                    break;
                case 1:
                    gPauseMenu = 3;
                    gPauseScreen.timer = 3;
                    gOptionsCursor = 0;
                    break;
                case 4:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gGame.flags |= 4;
                    if (gGame.mode == 1) {
                        event_clear_season_race_points();
                    }
                    break;
                case 3:
                    gPauseMenu = 4;
                    gPauseMenuCursor = 1;
                    break;
                case 2:
                case 5:
                    gPauseMenu = 0;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                gPauseMenuCursor = (gPauseMenuCursor + (gGame.mode == 1 ? 3 : 4)) % (gGame.mode == 1 ? 4 : 5);
                snd_play_menu_sound(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                gPauseMenuCursor = (gPauseMenuCursor + 1) % (gGame.mode == 1 ? 4 : 5);
                snd_play_menu_sound(0);
            }
            if (gPauseMenu == 0) {
                sfx_unmute();
                snd_set_volume(0, gSaveData_3.b2C6);
                snd_set_volume(1, gSaveData_3.b2C5);
                snd_set_volume(2, gSaveData_3.b2C7);
            }
        }
        if (gPauseMenu == 0) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == 4) {
        if (gRacePhase_2 == 5 && gPauseScreen.timer == -1) {
            if (PRESSED_2(0x20)) {
                gPauseMenu = 0;
                snd_play_menu_sound(2);
                snd_set_volume(0, gSaveData_3.b2C6);
                snd_set_volume(1, gSaveData_3.b2C5);
                snd_set_volume(2, gSaveData_3.b2C7);
            } else if (PRESSED_2(0x10)) {
                switch (gPauseMenuCursor) {
                case 0:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gRaceExitCode = 4;
                    gGame.flags &= ~4;
                    snd_play_menu_sound(1);
                    if (gGame.mode == 1) {
                        event_clear_season_race_points();
                    }
                    break;
                case 1:
                    snd_play_menu_sound(1);
                    gPauseMenu = 1;
                    gPauseMenuCursor = 2;
                    break;
                }
            } else if (PRESSED_2(1) || PRESSED_2(0x800)) {
                gPauseMenuCursor = 0;
                snd_play_menu_sound(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                gPauseMenuCursor = 1;
                snd_play_menu_sound(0);
            }
        }
        if (gPauseMenu == 0) {
            gPauseScreen.clearFrames = 2;
        }
    } else {
        if (gEvent.flags & 0x800) {
            event_update(0);
            snd_set_volume(0, 0);
            if (gEvent.tutorialTimeout != 0) {
                if ((gPlayerInputs_3.w[0] & 0x10) && !(gPlayerInputsPrev_4.w[0] & 0x10)) {
                    snd_play_menu_sound(1);
                    gEvent.tutorialTimeout = 1;
                }
                if (--gEvent.tutorialTimeout == 0) {
                    gEvent.flags &= 0xFFFE77FF;
                    snd_set_volume(0, gSaveData_3.b2C6);
                }
            }
            if (gEvent.flags & 0x10000) {
                gEvent.flags &= ~0x10000;
                gCamView = 0;
                cam_update();
            }
        } else {
            D_800E3750 = gPlayerInputs_3;
            race_update_replay();
            if (gReplayPlayback != 0 && gCamTargetOverride != -1 && !(gGame.flags & 0x80)) {
                for (j = 0; j < 4; j++) {
                    if ((gPadInput_3[j] & 0x400) && !(gPadInputPrev_3[j] & 0x400)) break;
                }
                if (j != 4) {
                    for (j = 1; j <= gEvent.numRiders; j++) {
                        t = (gCamTargetOverride + j) % gEvent.numRiders;
                        if (t < gGame.numPlayers) break;
                    }
                    gCamTargetOverride = t;
                }
            }
            if (gPodiumMode == 0) {
                bikes_update(1);
            } else {
                podium_update();
            }
            if (gGame.flags & 0x10) {
                for (gCamView = 0; gCamView < 4; gCamView++) {
                    cam_update();
                }
            } else {
                for (gCamView = 0; gCamView < gNumViews; gCamView++) {
                    cam_update();
                }
            }
            if (gPodiumMode == 0) {
                rider_update_head_look();
                rider_update_crouch();
                rider_update_fist_pump();
            }
            flash_update();
            snd_update_riders();
            if (gDebugFreeze == 0) {
                event_update(0);
            }
            for (j = 0; j < 8; j++) {
                if (gRiders[j].inactive == 0) {
                    if (gRiders[j].engine.throttle != 0 && gRiders[j].throttleTwist >= -0xFFF) {
                        gRiders[j].throttleTwist -= 0x200;
                    }
                    if (gRiders[j].engine.throttle == 0 && gRiders[j].throttleTwist < 0) {
                        gRiders[j].throttleTwist += 0x200;
                    }
                }
            }
        }
    }

    if (gReplayPlayback == 0 && gEvent.allFinished == 0 && gPauseMenu == 0 && !(gGame.flags & 0x81)) {
        for (j = 0; j < gEvent.numRiders; j++) {
            if (((gPlayerInputs_3.w[j] & 0x40) && !(gPlayerInputsPrev_4.w[j] & 0x40)) ||
                (gGame.mode == 9 && (gPadInput_3[j] & 0x40) && !(gPadInputPrev_3[j] & 0x40))) {
                if (flag == 0 && gRacePhase_2 == 5) {
                    gPlayerInputsPrev_4.w[j] |= 0x40;
                    if (gPodiumMode != 0) {
                        gRacePhase_2 = 6;
                        gRacePhaseTimer = -1;
                        gRaceExitCode = 4;
                        gGame.flags &= ~4;
                    } else if (gGame.mode == 13 || gGame.mode == 14) {
                        gPauseMenu = 6;
                        gPauseMenuCursor = 0;
                    } else if (gGame.mode == 9) {
                        gPauseMenu = gRacePhase_2;
                        gPauseMenuCursor = 0;
                    } else {
                        gPauseMenu = 1;
                        gPauseMenuCursor = 2;
                    }
                    gPausePlayer = j;
                    snd_stop_all_riders();
                    snd_set_volume(0, 0);
                    snd_set_volume(1, 0);
                    snd_set_volume(2, 0);
                    tunes_stop_speech();
                    rumble_stop_all();
                    break;
                }
            }
        }
    }

    switch (gRacePhase_2) {
    case 4:
        if (gRacePhaseTimer != -1) {
            if (--gRacePhaseTimer == 0) {
                gRacePhase_2 = 5;
            }
        }
        break;
    case 6:
        if (gRacePhaseTimer == -3) {
            if ((gGame.flags & 4) || (gGame.mode == 9 && gRaceExitCode == 6)) {
                gRacePhase_2 = 7;
                gRacePhaseTimer = 2;
            } else {
                gRacePhase_2 = 7;
                gRacePhaseTimer = 100;
                gCurGameState->mode = 3;
            }
        } else if (gRacePhaseTimer > 0) {
            if (--gRacePhaseTimer == 0) {
                gRacePhaseTimer = -2;
            }
        }
        break;
    case 7:
        if (gRacePhaseTimer == 0) {
            gRacePhase_2 = 4;
            gRacePhaseTimer = -1;
            race_finish_replay(0);
            cam_free();
            bikes_free();
            decal_free();
            rider_anims_unload();
            particles_free();
            flash_free();
            shadow_free();
            snd_race_exit();
            map_free();
            rain_free();
            snow_free();
            if (gGame.mode == 9) path_free();
            if (gDrawFlags & 0x800) event_hud_free();
            if (gGame.mode == 9) {
                switch (gEvent.tutorialAction) {
                case 3:
                    gSeason->round--;
                    break;
                case 4:
                    gSeason->round++;
                    break;
                case 6:
                    gEvent.flags |= 0x4000;
                    break;
                case 7:
                    break;
                default:
                    gEvent.state = 0xE;
                    gEvent.countdown = 0x3F;
                    gEvent.tutorialTimer = 0x3C;
                    gEvent.tutorialAction = 0;
                    gEvent.flags = (gEvent.flags & ~0x10) | 0x40;
                    break;
                }
                gPauseMenu = 0;
                gEvent.flags &= ~0x2000;
                if (gTutorialLessons[gSeason->round].flags & 4) {
                    gNumRiders = 2;
                } else {
                    gNumRiders = 1;
                }
            } else if (gGame.mode == 6) {
                gNumRiders = 4;
                gGame.numPlayers = 1;
            } else if (gGame.mode == 10 || gGame.mode == 2) {
                gNumRiders = gGame.numPlayers;
            } else if (gGame.mode == 0 && gGame.numPlayers == 2) {
                gNumRiders = gGame.twoPlayerCpuRiders + 2;
            } else {
                gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
                if (gGame.mode == 8 && !(gGame.flags & 1)) {
                    gNumRiders = 1;
                }
            }
            race_init_replay();
            event_init(gNumRiders, gGame.numLaps);
            race_begin();
            if (gGame.mode == 9) event_stop_sounds_rumble();
        }
        break;
    }
    D_800E3718 = D_800E3750;
    gPlayerInputsPrev_4 = gPlayerInputs_3;
    if (gDebugRiderCount <= 0) {
        gDebugRiderCount = 1;
    }
    if (gDebugRiderCount >= 9) {
        gDebugRiderCount = 8;
    }
    if (gCurTrack_15->f2C == 2) {
        fractal_emit_fire_smoke();
    }
    hacks_update();
}

typedef struct { u8 b[12]; } B12;

typedef struct {
    char name[0x10];
    s32 v[6];
    s32 f28;
    B12 b;
    s32 key;
} Src;

typedef struct {
    s32 n;
    Src e[1];
} File;

struct Res_3 {
    s32 f0;
    u32 *arr;
};


void race_load_surface_table(Res_3 *r, s32 idx)
{
    Pair_4 *tab;
    File *f;
    s32 i, j;

    tab = res_load(gSfxResSet_3, sSurfaceSoundsResId);
    f = res_load(r, idx);
    for (i = 0; i < 16; i++) {
        str_copy(gSurfaceTable[i].name, f->e[i].name);
        for (j = 0; j < 6; j++) {
            ((f32 *)&gSurfaceTable[i])[j + 4] = (f32)f->e[i].v[j] / 1000.0f;
        }
        gSurfaceTable[i].flags = f->e[i].f28;
        gSurfaceTable[i].color[0] = f->e[i].b.b[0];
        gSurfaceTable[i].color[1] = f->e[i].b.b[1];
        gSurfaceTable[i].color[2] = f->e[i].b.b[2];
        gSurfaceTable[i].color[3] = f->e[i].b.b[3];
        gSurfaceTable[i].color[4] = f->e[i].b.b[4];
        gSurfaceTable[i].color[5] = f->e[i].b.b[5];
        gSurfaceTable[i].color[6] = f->e[i].b.b[6];
        gSurfaceTable[i].color[7] = f->e[i].b.b[7];
        gSurfaceTable[i].color[8] = f->e[i].b.b[8];
        gSurfaceTable[i].color[9] = f->e[i].b.b[9];
        gSurfaceTable[i].color[10] = f->e[i].b.b[10];
        gSurfaceTable[i].color[11] = f->e[i].b.b[11];
        for (j = 0; tab[j].key != 0; j++) {
            if (tab[j].key == f->e[i].key) {
                break;
            }
        }
        gSurfaceTable[i].sfxId = tab[j].val;
    }
    res_unload(&r->arr[idx]);
    res_unload(&gSfxResSet_3->arr[sSurfaceSoundsResId]);
}

typedef struct { u8 b[48]; } Blk48;
typedef struct { u8 b[16]; } Blk16;

typedef struct {
    u8 pad[0x50];
    s32 f50;
} Lvl;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
} OvlTab;

extern Lvl *gCurTrack_16;
extern OvlTab gBootHeader_2;

void race_load_frontend_code(s32 x)
{
    Blk48 buf;
    s32 h;
    s32 cart;
    s32 sz;
    s32 q;
    u8 *p3;
    u8 *p6;
    u8 *p7;

    if (x == sFrontendLoaded || (gGame.flags & 1)) {
        return;
    }
    p3 = gFrontendOvlStart;
    p6 = gUsrTrkOvlStart;
    p7 = gUsrTrkOvlEnd;
    if ((u32)p3 & 0xF) fatal_halt(0x33);
    if ((u32)p6 & 0xF) fatal_halt(0x34);
    if ((u32)p7 & 0xF) fatal_halt(0x35);
    if (x != 0) {
        sz = (u32)&gUsrTrkOvlStart[-0x30] - (u32)p3;
        mem_alloc_at_dbg(p3 + 0x30, sz, sStrMinFile, 0xBDD);
        if (!(gCurTrack_16->f50 & 2)) {
            mem_alloc_at_dbg(p6 + 0x10, (u32)&gUsrTrkOvlEnd[-0x10] - (u32)p6, sStrMinFile, 0xBE0);
        }
        osWritebackDCacheAll();
        if (gLoadedByBootLoader == 0) {
            cart = 0xB0000C00;
            while ((h = dma_start(gCartRomHandle, 0, (s32)p3 + cart + 0x30, (s32)(p3 + 0x30), sz)) < 0);
            while (dma_get_status(h) != 3);
            dma_release(h);
            while ((h = dma_start(gCartRomHandle, 0, cart + (s32)p6 + 0x10, (s32)(p6 + 0x10), (q = (s32)p7 - 0x10) - (s32)p6)) < 0);
            while (dma_get_status(h) != 3);
            dma_release(h);
        } else {
            while (sched_get_current_task() != 0);
            cart = gBootHeader_2.f28 + 0x28;
            cart += gBootHeader_2.fC;
            while ((h = dma_start(gCartRomHandle, 0, cart, gZBuffer, gBootHeader_2.f14)) < 0);
            while (dma_get_status(h) != 3);
            dma_release(h);
            gSwdDst = p3;
            gSwdDstEnd = p6;
            gSwdSrc = gZBuffer;
            gSwdSrcEnd = gZBuffer + gBootHeader_2.fC;
            buf = *(Blk48 *)p3;
            swd_decompress();
            *(Blk48 *)p3 = buf;
            cart += gBootHeader_2.f14;
            while ((h = dma_start(gCartRomHandle, 0, cart, gZBuffer, gBootHeader_2.f1C)) < 0);
            while (dma_get_status(h) != 3);
            dma_release(h);
            gSwdDst = p6;
            gSwdDstEnd = p7;
            __builtin_memcpy(&buf, p6, 16);
            gSwdSrc = gZBuffer;
            gSwdSrcEnd = gZBuffer + gBootHeader_2.fC;
            swd_decompress();
            *(Blk16 *)p6 = *(Blk16 *)&buf;
        }
        osInvalDCache(0, 0x800000);
        osInvalICache(0, 0x800000);
        sFrontendLoaded = 1;
    } else {
        mem_free(p3 + 0x30);
        if (!(gCurTrack_16->f50 & 2)) {
            mem_free(p6 + 0x10);
        }
        sFrontendLoaded = 0;
    }
}

void race_init_ghost_replay(s32 p)
{
    if (p != 0) {
        Game *c;
        gGhostSlot ^= 1;
        c = &gGame;
        c->numPlayers = 1;
        c->numCpuRiders = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gNumRiders = 2;
        c->numPlayers = 2;
        gGhostSlot ^= 1;
    } else {
        Game *c = &gGame;
        c->numPlayers = 1;
        c->numCpuRiders = 0;
        gNumRiders = 1;
        gGhostSlot = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gGhostSlot = 1;
    }
    if (gGhostRace != 0) {
        Game *c = &gGame;
        if (c->flags & 0x80000) {
            c->numPlayers = 1;
            replay_play_start_keep_setup(gActiveReplay + 0x6400);
            c->numPlayers = 2;
        }
    }
}

void race_teardown(void)
{
    race_finish_replay(0);
    cam_free();
    bikes_free();
    decal_free();
    rider_anims_unload();
    particles_free();
    flash_free();
    shadow_free();
    snd_race_exit();
    map_free();
    rain_free();
    snow_free();
    if (gGame.mode == 9) {
        path_free();
    }
    if (gDrawFlags & 0x800) {
        event_hud_free();
    }
}

void race_apply_volumes(void)
{
    u8 *s = &gSaveData_4[0];
    snd_set_volume(0, s[0x2C6]);
    snd_set_volume(1, s[0x2C5]);
    snd_set_volume(2, s[0x2C7]);
}

void race_load_ghost_setup(s32 param_1)
{
    Game *b;
    u8 *gb;
    s32 s2;
    s32 s3;
    u8 s4;
    u8 s5;
    s32 idx;

    if (param_1 != 0) {
        gb = (u8 *)0x800E0000;
        __asm__ __volatile__("" : "+r"(gb));
        b = &gGame;
        s2 = b->riders[0];
        __asm__ __volatile__("" : "+r"(s2) : : "memory");
        *(s32 *)(gb + 0x3700) = -1;
        s3 = b->playerPad[0];
        s4 = b->riderSkins[0];
        s5 = b->bikeSkins[0];
        replay_play_start(param_1);
        idx = *(s32 *)(gb + 0x3700) ^ 1;
        b->riders[idx] = s2;
        b->playerPad[idx] = s3;
        b->riderSkins[*(s32 *)(gb + 0x3700) ^ 1] = s4;
        b->bikeSkins[*(s32 *)(gb + 0x3700) ^ 1] = s5;
        b->playerPad[*(s32 *)(gb + 0x3700)] = 0;
    }
}

void race_set_video_mode(void)
{
    if ((gGame.flags & 0x81) == 0 && (gSaveOptions & 2) != 0 && gHasExpansionPak != 0 &&
        (gGame.numPlayers == 1 || gGame.mode == 8) &&
        gCurTrack_3[11] != 2 && (gGame.flags & 0x4040000) == 0) {
        gViModeRequest = 2;
    } else {
        gViModeRequest = 1;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();
}

void race_alloc_text_objs(void)
{
    s32 i;
    TextObj *p;

    gRaceTextObjs = mem_alloc_dbg(0x148, 0, "min.c", 0x4E3);
    i = 0;
    do {
        p = font_text_create(NULL, 0x40, 1, 2, 0x20);
        gRaceTextObjs->text[i] = p;
        i++;
    } while (i < 0x4A);
}

void race_free_text_objs(void)
{
    s32 i;

    i = 0;
    do {
        font_text_destroy(gRaceTextObjs->text[i]);
        mem_free(gRaceTextObjs->text[i]);
        i++;
    } while (i < 0x4a);
    mem_free(gRaceTextObjs);
}

void race_mute_volumes(void)
{
    snd_set_volume(0, 0);
    snd_set_volume(1, 0);
    snd_set_volume(2, 0);
}
