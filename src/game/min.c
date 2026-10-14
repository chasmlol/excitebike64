#include "game.h"



/* Chooses what the race records or plays back: a replay, the time trial's ghost, a tutorial demo, or a new
   recording of the players' input. */
void race_init_replay(void)
{
    s32 *demo;

    sDemoActive = 0;
    gDemoPlayback = 0;
    gReplayPlayback = 0;
    if (gGame.mode == MODE_TIME_TRIAL && (gGame.flags & GAME_GHOST) != 0) {
        if ((gGame.flags & GAME_TRACK_INTRO) != 0) {
            race_load_ghost_setup((s32)(gActiveReplay_2 + 0x1900)); /* the ghost, 0x6400 bytes in */
        }
    }
    if ((gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) == 0 && gGame.mode == MODE_TIME_TRIAL) {
        gGhostRace = 1;
        if ((gGame.flags & GAME_GHOST) != 0) {
            sGhostData = gActiveReplay_2 + 0x1900;
        } else {
            sGhostData = 0;
        }
    } else if ((gGame.flags & GAME_REPLAY) != 0) {
        gReplayPlayback = 1;
        sReplayRequest = 0;
        replay_play_start_2((s32)gActiveReplay_2);
    } else {
        if ((gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) == 0 && gGame.mode == MODE_TUTORIAL &&
            gTutorialLessons[gSeason->round].demo != 0 && (gEvent.flags & EVENT_NO_DEMO) == 0) {
            gEvent.flags = gEvent.flags & ~EVENT_NO_DEMO;
            gGhostRace = 0;
            gDemoPlayback = 1;
            sDemoActive = 1;
            if ((gGame.flags & 0x4000) != 0) {
                gActiveReplay_2 = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
            } else {
                demo = res_load(gResourceSet, *gTutorialLessons[gSeason->round].demo);
                *(volatile s32 **)&gActiveReplay_2 = demo;
                gActiveReplaySize = demo[1] - demo[0];
                gActiveReplay_2 = demo + 2;
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
    if (gGame.mode == MODE_TUTORIAL || gGhostRace != 0 || gDemoPlayback != 0 ||
        (gGame.flags & (0x4000 | GAME_FRONTEND_RACE)) != 0 || gGame.mode == MODE_TEST_RIDE ||
        gGame.mode == MODE_TEST_RIDE_OPEN || (gGame.flags & GAME_TRACK_INTRO) != 0) {
        gReplayRecording = 0;
    } else {
        gReplayRecording = 1;
    }
    if (gReplayRecording != 0 && gReplayPlayback == 0 && gActiveReplay_2 != 0) {
        replay_record_start((s32)gActiveReplay_2, gActiveReplaySize);
    }
}


/* Ends the race's recording (or frees the tutorial's demo), then reloads the front-end code if asked. */
void race_finish_replay(s32 reloadFrontend)
{
    Game *game = &gGame;
    if ((game->flags & (GAME_FRONTEND_RACE | GAME_REPEAT_STATE | GAME_TRACK_INTRO)) != 0) {
        if ((game->flags & GAME_TRACK_INTRO) == 0) {
            return;
        }
        if (gRaceExitCode != RACE_EXIT_BACK) {
            return;
        }
    }
    if (gGhostRace == 0) {
        if (gDemoPlayback != 0) {
            gActiveReplay = 0;
            res_unload(&gResourceSet->handles[gTutorialLessons[gSeason->round].demo[0]]);
            goto reload;
        }
        /* no recording to finish after backing out, in the tutorial, soccer and the test rides */
        if (gRaceExitCode == RACE_EXIT_BACK || game->mode == MODE_TUTORIAL) goto reload;
        if (game->mode < MODE_STUNT) goto check_soccer;
        if (game->mode >= MODE_TEST_RIDE_OPEN + 1) goto finish;
        if (game->mode < MODE_TEST_RIDE) goto finish;
        goto reload;
check_soccer:
        if (game->mode == MODE_SOCCER) goto reload;
    }
finish:
    replay_record_finish();
reload:
    if (sReplayRequest == 0 && reloadFrontend != 0) {
        race_load_frontend_code(1);
    }
}

/* Records the players' input, plays a replay or the ghost back into gPlayerInputs, restarts a tutorial demo when
   it ends, and starts the replay of a race just ridden when one is requested. */
void race_update_replay(void)
{
    if (gReplayRecording != 0) {
        if (gReplayPlayback == 0) {
            if (gGame.mode == MODE_TUTORIAL || gEvent.allFinished == 0) {
                replay_record_frame(gPlayerInputs);
            }
        } else if (gGame.mode == MODE_TUTORIAL || gEvent.allFinished == 0) {
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
        /* the time trial: record the player (steering unmirrored), play the ghost back */
        if (gRiders[gGhostSlot ^ 1].race.finishPlace == -1) {
            if (gMirrorMode != 0) {
                gPlayerInputs[gGhostSlot ^ 1] ^= INPUT_STICK_LEFT | INPUT_STICK_RIGHT;
            }
            replay_record_frame(&gPlayerInputs[gGhostSlot ^ 1]);
            if (gMirrorMode != 0) {
                gPlayerInputs[gGhostSlot ^ 1] ^= INPUT_STICK_LEFT | INPUT_STICK_RIGHT;
            }
        }
        if (sGhostData != 0) {
            replay_play_frame(&gPlayerInputs[gGhostSlot]);
        }
    } else if (gDemoPlayback != 0 && sDemoActive != 0) {
        if ((gEvent.flags & EVENT_DEMO) && replay_play_frame(gPlayerInputs) != 0) {
            bikes_free();
            decal_clear_all();
            mem_fill((u8 *)gRiders, 0, sizeof(Rider) * 8);
            tutorial_init_lesson(0);
            bikes_init();
            gEvent.flags = (gEvent.flags | EVENT_CAM_RESET) & ~EVENT_RIDER_STOPPED;
            replay_play_start(gActiveReplay);
        }
    }
    decal_update_tire_tracks();
    tube_update_splashes();
}

/* gCurTrack, a gTracks record */
typedef struct {
    u8 pad00[0x2C];
    s32 envType;                    /* 0x2C TRACK_ENV_* */
    u8 pad30[0x20];
    u32 flags;                      /* 0x50 TRACK_FLAG_* */
    u8 pad54[0x20];
    s16 numFollowZones;             /* 0x74 the camera zones */
    s16 numDollyZones;              /* 0x76 */
    CamFollowZone *followZones;     /* 0x78 */
    CamDollyZone *dollyZones;       /* 0x7C */
    s32 *resMapId;                  /* 0x80 */
} TrackInfo;

extern TrackInfo *gCurTrack_14;

/* The race game state's init: the video mode, the race's resources, the riders, the event, the camera and
   everything drawn, then the track intro or the fade in. */
void race_init(s32 arg)
{
    u16 *zoneRes;
    CamFollowZone *zones;
    TrackInfo *track;
    s32 i;
    s32 alpha;

    if (!(gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) && (gSaveData.options & SAVE_OPT_HIGH_RES) &&
        gHasExpansionPak != 0 && (gGame.numPlayers == 1 || gGame.mode == MODE_TIME_TRIAL) &&
        gCurTrack_14->envType != TRACK_ENV_DESERT && !(gGame.flags & (GAME_LOW_RES | GAME_PODIUM))) {
        gViModeRequest = VI_MODE_HI_RES;
    } else {
        gViModeRequest = VI_MODE_MED_RES;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();

    if ((gGame.mode == MODE_EXHIBITION || gGame.mode == MODE_SEASON || gGame.mode == MODE_TIME_TRIAL) &&
        (gCheatFlags & 2) /* Mirror */ && !(gGame.flags & (GAME_PODIUM | GAME_FRONTEND_RACE))) {
        gMirrorMode = 1;
    } else {
        gMirrorMode = 0;
    }
    gPodiumMode = 0;
    gTimeTrialPlayer = 0;
    gPauseMenu = PAUSE_NONE;
    gPauseScreen.timer = -1;
    gPauseScreen.clearFrames = 0;
    gPauseScreen.loaded = 0;
    gSchedFrameLimit = 1;
    gGame.flags |= GAME_IN_RACE;
    race_load_frontend_code(0);
    if (gReplayPlayback != 0) {
        gCamTargetOverride = gEvent.camTarget;
    } else {
        gCamTargetOverride = -1;
    }
    osWritebackDCacheAll();
    res_open_set(sStrRaceResource, &gResourceSet);
    if (gCurTrack_14 != NULL) {
        track_res_load_map(gCurTrack_14->resMapId);
    }
    /* the camera zones of replays and demos: the counts, the follow zones, then the dolly zones */
    zoneRes = (u16 *)track_res_get(gGidSpCam);
    if (zoneRes != NULL && !(gCurTrack_14->flags & TRACK_FLAG_USER)) {
        track = gCurTrack_14;
        track->numDollyZones = zoneRes[1];
        track->numFollowZones = zoneRes[0];
        zones = (CamFollowZone *)(zoneRes + 2);
        track->followZones = zones;
        track->dollyZones = (CamDollyZone *)(zones + track->numFollowZones);
    } else {
        gCurTrack_14->numDollyZones = 0;
        gCurTrack_14->numFollowZones = 0;
        gCurTrack_14->followZones = NULL;
        gCurTrack_14->dollyZones = NULL;
    }
    if (gGame.flags & (GAME_PODIUM | GAME_TRACK_INTRO)) {
        font_load_menu_set(gResourceSet);
    } else if (!(gGame.flags & GAME_FRONTEND_RACE)) {
        font_load_race_set(gResourceSet);
    }

    if (gGame.mode == 4 || gGame.mode == MODE_DESERT) {
        gNumRiders = 6;
    } else if (gGame.mode == MODE_TUTORIAL) {
        if (gTutorialLessons[gSeason->round].flags & LESSON_TAKEDOWN) {
            gNumRiders = 2;
        } else {
            gNumRiders = 1;
        }
    } else if ((gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) == GAME_TRACK_INTRO) {
        gNumRiders = 1;
    } else if (gGame.mode == MODE_EXHIBITION && gGame.numPlayers == 2) {
        gNumRiders = gGame.twoPlayerCpuRiders + 2;
    } else if (gGame.mode == MODE_TEST_RIDE || gGame.mode == MODE_TEST_RIDE_OPEN) {
        gNumRiders = 1;
    } else if (gGame.flags & GAME_FRONTEND_RACE) {
        gNumRiders = 6;
        D_800E3704 = 0;
    } else if (gGame.mode == MODE_EXCITE_3D) {
        gNumRiders = 4;
        gGame.numPlayers = 1;
    } else if (gGame.mode == MODE_SOCCER || gGame.mode == MODE_STUNT) {
        gNumRiders = gGame.numPlayers;
    } else {
        gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
    }

    if (gGame.mode == MODE_TIME_TRIAL && !(gGame.flags & GAME_FRONTEND_RACE)) {
        gNumRiders = 1;
    }
    alpha = 0xFF;
    for (i = 7; i >= 0; i--) {
        gRiderAlpha[i] = alpha;
    }
    if (gGame.flags & GAME_RIDER_SELECT) {
        gNumRiders = 6;
    }
    race_init_replay();
    if (gGame.flags & GAME_PODIUM) {
        gNumRiders = gGame.numPlayers;
        gGame.numPlayers = 1;
    }
    event_init(gNumRiders, gGame.numLaps);
    if (!(gGame.flags & (GAME_REPLAY | GAME_TRACK_INTRO | GAME_FRONTEND_RACE))) {
        race_randomize_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
        race_set_grid_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    }
    gPhysDeltaTime = 0.016666668f;
    engine_init_dial_vtx();
    mem_fill(gPlayerInputsPrev_2, 0, 0x18);
    palmod_init((gGame.flags >> 18) & 1); /* GAME_PODIUM */
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
    if (gPodiumMode != 0 && gCurTrack_14->envType == TRACK_ENV_STADIUM) {
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
    if (gGame.mode == MODE_TUTORIAL) {
        event_stop_sounds_rumble();
    }
    if (gPodiumMode != 0) {
        podium_init();
        race_alloc_text_objs();
    }
    if (gGame.flags & GAME_TRACK_INTRO) {
        gRacePhase = RACE_PHASE_INTRO_FADE_IN;
        gRacePhaseTimer = PHASE_TIMER_START;
        race_intro_load();
    } else if (gGame.flags & GAME_FRONTEND_RACE) {
        gRacePhase = RACE_PHASE_RACING;
    } else {
        gRacePhase = RACE_PHASE_FADE_IN;
        gRacePhaseTimer = PHASE_TIMER_START;
    }
    gTimeTrialPlayer = 0;
    if (gGame.mode == MODE_TIME_TRIAL && !(gGame.flags & GAME_TRACK_INTRO) && gGhostSlot == 0) {
        gTimeTrialPlayer = 1;
    }
}

/* The race game state's exit: frees what race_init loaded and finishes the recording. */
void race_exit(void)
{
    u32 *flags;
    u32 cleared;
    u32 frontendRace;

    rumble_stop_all();
    if ((*(u32 *)(gCurTrack + 0x50) & TRACK_FLAG_USER) != 0 && (gGame.mode - MODE_TEST_RIDE) < 2) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    gGame.flags = gGame.flags & ~GAME_IN_RACE;
    hacks_exit();
    decal_free();
    rider_anims_unload();
    flags = &gGame.flags;
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
    cleared = gGame.flags & 0xFFFFBFFF;
    frontendRace = gGame.flags & GAME_FRONTEND_RACE;
    gGame.flags = cleared;
    if (frontendRace == 0) {
        font_unload_set();
    }
    race_finish_replay(1);
    gSchedFrameLimit = 0;
    gSchedMinRetraces = 1;
    if (gPodiumMode != 0) {
        gRaceExitCode = RACE_EXIT_PODIUM;
        flags[0x13] = 0; /* gGame.mode = MODE_EXHIBITION */
    }
}

/* Starts the race proper after the track intro, or again after a restart: the views, the riders, the camera and
   the effects. */
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
    if (gGame.mode == MODE_TUTORIAL) {
        path_init(gResourceSet);
    }
    if ((gGame.flags & (GAME_REPLAY | GAME_TRACK_INTRO | GAME_REPEAT_STATE | GAME_FRONTEND_RACE)) == 0 &&
        (gGame.mode != MODE_SEASON || gSeason->round == 0)) {
        race_randomize_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
        race_set_grid_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
    }
    gPauseMenu = PAUSE_NONE;
    if ((gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) == 0 && (gSaveData.options & SAVE_OPT_HIGH_RES) != 0 &&
        gHasExpansionPak != 0 && (gGame.numPlayers == 1 || gGame.mode == MODE_TIME_TRIAL) &&
        gCurTrack_4->f2C != TRACK_ENV_DESERT && (gGame.flags & (GAME_LOW_RES | GAME_PODIUM)) == 0) {
        gViModeRequest = VI_MODE_HI_RES;
    } else {
        gViModeRequest = VI_MODE_MED_RES;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();
    race_apply_volumes();
    if (gCurTrack_4->f2C == TRACK_ENV_DESERT) {
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
    if (gCurTrack_4->f2C == TRACK_ENV_OUTDOOR) {
        tube_reset_objects();
    }
    gDrawFlags = gDrawFlags | DRAW_HUD;
    event_hud_init();
    snd_race_init(gSurfaceTable);
    rain_init();
    snow_init();
    announce_reset();
    gTimeTrialPlayer = 0;
    if (gGame.mode == MODE_TIME_TRIAL && (gGame.flags & GAME_TRACK_INTRO) == 0 && gGhostSlot == 0) {
        gTimeTrialPlayer = 1;
    }
    sky_reset_clip();
    if (gNumScreens >= 2) {
        gSchedMinRetraces = 2;
    } else {
        gSchedMinRetraces = 1;
    }
}

/* Controller 1's input word: the stick's y magnitude is the top four bits. */
typedef union {
    u32 w;
    struct {
        u32 stickY : 4;
        u32 stickX : 4;
        u32 rest : 24;
    } b;
} PadWord;

extern PadWord gPadInput_5;

/* The track intro (track preview): left and right change the laps of a multiplayer exhibition, B goes back, A or
   START starts the race; then the intro is torn down and the race begins. */
void race_update_intro(void)
{
    s32 left, right, pressA, pressB, pressStart;
    u32 input;

    /* the stick, pushed far enough, works as the D-pad */
    if ((gPadInput_5.w & (INPUT_STICK_UP | INPUT_STICK_DOWN)) &&
        ((gPadInput_5.w & INPUT_STICK_UP) ? gPadInput_5.b.stickY : -gPadInput_5.b.stickY) >= 4) {
        gPadInput_5.w |= INPUT_D_UP;
    } else if ((gPadInput_5.w & (INPUT_STICK_UP | INPUT_STICK_DOWN)) &&
               ((gPadInput_5.w & INPUT_STICK_UP) ? gPadInput_5.b.stickY : -gPadInput_5.b.stickY) < -3) {
        gPadInput_5.w |= INPUT_D_DOWN;
    }
    if ((gPadInput_5.w & (INPUT_STICK_LEFT | INPUT_STICK_RIGHT)) &&
        ((gPadInput_5.w & INPUT_STICK_RIGHT) ? ((s32)gPadInput_5.w >> 24 & 0xF) : -((s32)gPadInput_5.w >> 24 & 0xF)) >= 4) {
        gPadInput_5.w |= INPUT_D_RIGHT;
    } else if ((gPadInput_5.w & (INPUT_STICK_LEFT | INPUT_STICK_RIGHT)) &&
               ((gPadInput_5.w & INPUT_STICK_RIGHT) ? ((s32)gPadInput_5.w >> 24 & 0xF) : -((s32)gPadInput_5.w >> 24 & 0xF)) < -3) {
        gPadInput_5.w |= INPUT_D_LEFT;
    }
    input = gPadInput_5.w;
    left = (input & INPUT_D_LEFT) && !(gPadInputPrev & INPUT_D_LEFT);
    right = (input & INPUT_D_RIGHT) && !(gPadInputPrev & INPUT_D_RIGHT);
    pressA = (input & INPUT_A) && !(gPadInputPrev & INPUT_A);
    pressB = (input & INPUT_B) && !(gPadInputPrev & INPUT_B);
    pressStart = (input & INPUT_START) && !(gPadInputPrev & INPUT_START);

    if (gGame.mode == MODE_EXHIBITION && gGame.numPlayers >= 2) {
        if (left && gGame.numLaps >= 2) gGame.numLaps--;
        if (right && gGame.numLaps < 6) gGame.numLaps++;
    }

    switch (gRacePhase_2) {
    case RACE_PHASE_INTRO_FADE_IN:
        if (gRacePhaseTimer != PHASE_TIMER_START) {
            if (--gRacePhaseTimer == 0) gRacePhase_2 = RACE_PHASE_INTRO;
        }
        break;
    case RACE_PHASE_INTRO_FADE_OUT:
        if (gRacePhaseTimer == PHASE_TIMER_FADED) {
            if (gRaceExitCode == RACE_EXIT_BACK) {
                race_intro_unload();
                gRacePhase_2 = RACE_PHASE_DONE;
                gRacePhaseTimer = 100;
                gCurGameState->mode = STATE_EXIT;
            } else {
                gRacePhase_2 = RACE_PHASE_INTRO_DONE;
                gRacePhaseTimer = 2;
            }
        } else if (gRacePhaseTimer > 0) {
            if (--gRacePhaseTimer == 0) gRacePhaseTimer = PHASE_TIMER_OVER;
        }
        break;
    case RACE_PHASE_INTRO_DONE:
        if (gRacePhaseTimer == 0) {
            /* tear the intro down and set the race up */
            gRacePhase_2 = RACE_PHASE_FADE_IN;
            gRacePhaseTimer = PHASE_TIMER_START;
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
            if (gGame.mode == MODE_TUTORIAL) path_free();
            if (gDrawFlags & DRAW_HUD) event_hud_free();
            race_intro_unload();
            gGame.flags &= ~GAME_TRACK_INTRO;
            gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
            if (gGame.mode == MODE_TIME_TRIAL && !(gGame.flags & GAME_FRONTEND_RACE)) {
                gNumRiders = 1;
            } else if (gGame.mode == MODE_EXHIBITION && gGame.numPlayers == 2) {
                gNumRiders = gGame.twoPlayerCpuRiders + 2;
            }
            race_init_replay();
            event_init(gNumRiders, gGame.numLaps);
            race_begin();
            if (gGame.mode == MODE_TUTORIAL) event_stop_sounds_rumble();
        }
        break;
    case RACE_PHASE_INTRO:
        if (pressB && (gGame.mode != MODE_SEASON || gSeason->round == 0)) {
            snd_play_menu_sound(MENU_SFX_BACK);
            gRacePhase_2 = RACE_PHASE_INTRO_FADE_OUT;
            gRacePhaseTimer = 20;
            gRaceExitCode = RACE_EXIT_BACK;
            gGame.flags &= ~GAME_REPEAT_STATE;
        } else if (pressA || pressStart) {
            snd_play_menu_sound(MENU_SFX_SELECT);
            gRaceExitCode = RACE_EXIT_NONE;
            gRacePhase_2 = RACE_PHASE_INTRO_FADE_OUT;
            gRacePhaseTimer = 20;
        }
        break;
    }
}

/* Loads the track intro: the text objects and its pictures (the season's shows the cup's tracks). */
void race_intro_load(void)
{
    s32 i;
    s32 res;

    gRaceTextObjs = mem_alloc_dbg(sizeof(RaceTexts), 0, sStrMinFile, 0x52C);
    i = 0;
    do {
        gRaceTextObjs->text[i] = font_text_create(NULL, 0x40, 1, 2, TEXT_VALIGN_CENTER);
        i = i + 1;
    } while (i < RACE_NUM_TEXTS);
    res = res_load(gResourceSet, sIntroImagesResId);
    gRaceTextObjs->introImages = (u8 *)res;
    res = res_load(gResourceSet, sIntroPicResId);
    gRaceTextObjs->introPic = (Pic *)res;
    if (gGame.mode == MODE_SEASON) {
        i = 0;
        do {
            res = res_load(gResourceSet, *(s32 *)sIntroTrackPicResIds[gSeason->cup->tracks[i]]);
            gRaceTextObjs->roundPics[i] = (Pic *)res;
            i = i + 1;
        } while (i < 5);
    }
    res = res_load(gResourceSet, sLapArrowsResId);
    gRaceTextObjs->lapArrows = (Pic *)res;
}

/* Frees what race_intro_load loaded and switches to the race's fonts. */
void race_intro_unload(void) {
    s32 i;

    for (i = 0; i < RACE_NUM_TEXTS; i++) {
        font_text_destroy(gRaceTextObjs->text[i]);
        mem_free(gRaceTextObjs->text[i]);
    }
    res_unload(&gResourceSet->handles[sIntroImagesResId]);
    res_unload(&gResourceSet->handles[sIntroPicResId]);
    res_unload(&gResourceSet->handles[sLapArrowsResId]);
    if (gGame.mode == MODE_SEASON) {
        for (i = 0; i < 5; i++) {
            res_unload(&gResourceSet->handles[*sIntroTrackPicResIds_2[gSeason->cup->tracks[i]]]);
        }
    }
    mem_free(gRaceTextObjs);
    gRaceTextObjs = NULL;
    if (gRaceExitCode != RACE_EXIT_BACK) {
        gGame.flags &= ~GAME_TRACK_INTRO;
    }
    font_load_race_set(gResourceSet);
}

/* the controller of the player who paused */
#define PAUSE_PAD (gGame.playerPad[gPausePlayer])
#define OPTION_PRESSED(m) ((gPadInput_2[PAUSE_PAD] & (m)) && !(gPadInputPrev_2[PAUSE_PAD] & (m)))

/* The pause menu's options screen: the music, effects and speech volumes (each previewed while it is chosen) and
   mono/stereo. B goes back to the pause menu. */
void race_pause_update_options(void) {
    s32 step;
    s32 prevCursor;
    s32 input;

    if (sOptionsPreviewDelay != 0) {
        sOptionsPreviewDelay--;
    }
    if (++gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    /* the stick, pushed far enough, works as the D-pad */
    if (((gPadInput_2[PAUSE_PAD] >> INPUT_STICK_X_SHIFT) & 0xF) < 6) {
        gPadInput_2[PAUSE_PAD] &= ~(INPUT_STICK_LEFT | INPUT_STICK_RIGHT);
    }
    if ((s32)((u32)gPadInput_2[PAUSE_PAD] >> INPUT_STICK_Y_SHIFT) < 6) {
        gPadInput_2[PAUSE_PAD] &= ~(INPUT_STICK_UP | INPUT_STICK_DOWN);
    }
    if (gPadInput_2[PAUSE_PAD] & INPUT_STICK_LEFT) {
        gPadInput_2[PAUSE_PAD] |= INPUT_D_LEFT;
    }
    if (gPadInput_2[PAUSE_PAD] & INPUT_STICK_RIGHT) {
        gPadInput_2[PAUSE_PAD] |= INPUT_D_RIGHT;
    }
    if (gPadInput_2[PAUSE_PAD] & INPUT_STICK_UP) {
        gPadInput_2[PAUSE_PAD] |= INPUT_D_UP;
    }
    if (gPadInput_2[PAUSE_PAD] & INPUT_STICK_DOWN) {
        gPadInput_2[PAUSE_PAD] |= INPUT_D_DOWN;
    }
    if (OPTION_PRESSED(INPUT_B)) {
        snd_set_volume(SND_VOL_EFFECTS, 0);
        snd_set_volume(SND_VOL_MUSIC, 0);
        snd_set_volume(SND_VOL_SPEECH, 0);
        save_write(5, 0);
        gPauseMenu = PAUSE_MAIN;
        gPauseScreen.timer = 3;
        sfx_stop_all();
        tunes_stop_speech();
        return;
    }
    if (OPTION_PRESSED(INPUT_A)) {
        switch (gOptionsCursor) {
        case OPTION_MUSIC:
            break;
        case OPTION_STEREO:
            snd_play_menu_sound(MENU_SFX_SELECT);
            gSaveData.options ^= SAVE_OPT_STEREO;
            return;
        case OPTION_EFFECTS:
            sfx_play(sOptionsTestSfx, 0x2B11, gSaveData.volumes[VOLUME_EFFECTS], 7, 1, 1);
            return;
        case OPTION_SPEECH:
            if (sOptionsPreviewDelay == 0) {
                tunes_queue_speech(0x9B);
                sOptionsPreviewDelay = 0x34;
            }
            return;
        case 4:
            snd_play_menu_sound(MENU_SFX_ERROR);
            return;
        }
        return;
    }
    prevCursor = gOptionsCursor;
    if (OPTION_PRESSED(INPUT_D_DOWN)) {
        if (prevCursor < 3) {
            gOptionsCursor = prevCursor + 1;
        } else {
            gOptionsCursor = prevCursor + 2;
        }
        if (gOptionsCursor >= 4) {
            gOptionsCursor = 0;
        }
    }
    if (OPTION_PRESSED(INPUT_D_UP)) {
        if (gOptionsCursor < 4) {
            gOptionsCursor = gOptionsCursor - 1;
        } else {
            gOptionsCursor = gOptionsCursor - 2;
        }
        if (gOptionsCursor < 0) {
            gOptionsCursor = OPTION_STEREO;
        }
    }
    if (prevCursor != gOptionsCursor) {
        /* the speech preview stops when the cursor leaves it; the effects and speech play when it arrives */
        switch (prevCursor) {
        case OPTION_MUSIC:
        case OPTION_EFFECTS:
            break;
        case OPTION_SPEECH:
            tunes_stop_speech();
            break;
        }
        switch (gOptionsCursor) {
        case OPTION_MUSIC:
            break;
        case OPTION_SPEECH:
            tunes_queue_speech(0x9B);
            sOptionsPreviewDelay = 0x34;
            break;
        case OPTION_EFFECTS:
            sfx_play(sOptionsTestSfx, 0x2B11, gSaveData.volumes[VOLUME_EFFECTS], 7, 1, 1);
            break;
        }
    }
    if (gOptionsCursor < OPTION_STEREO) {
        /* a volume bar: left and right step by 8, the stick by its magnitude / 3 */
        if (OPTION_PRESSED(INPUT_D_LEFT)) {
            step = -8;
        } else if (OPTION_PRESSED(INPUT_D_RIGHT)) {
            step = 8;
        } else {
            input = gPadInput_2[PAUSE_PAD];
            if (input & (INPUT_STICK_LEFT | INPUT_STICK_RIGHT)) {
                if (input & INPUT_STICK_RIGHT) {
                    step = ((input >> INPUT_STICK_X_SHIFT) & 0xF) / 3;
                } else {
                    step = -((input >> INPUT_STICK_X_SHIFT) & 0xF) / 3;
                }
            } else {
                step = 0;
            }
        }
        if (step != 0 && sOptionsPreviewDelay == 0) {
            switch (gOptionsCursor) {
            case OPTION_EFFECTS:
                sfx_play(sOptionsTestSfx, 0x2B11, gSaveData.volumes[VOLUME_EFFECTS], 7, 1, 1);
                sOptionsPreviewDelay = 0x28;
                break;
            case OPTION_SPEECH:
                tunes_queue_speech(0x9B);
                sOptionsPreviewDelay = 0x34;
                break;
            }
        }
        if (step < 0) {
            switch (gOptionsCursor) {
            case OPTION_EFFECTS:
                if (gSaveData.volumes[VOLUME_EFFECTS] >= -step) {
                    gSaveData.volumes[VOLUME_EFFECTS] += step;
                } else {
                    gSaveData.volumes[VOLUME_EFFECTS] = 0;
                }
                break;
            case OPTION_MUSIC:
                if (gSaveData.volumes[VOLUME_MUSIC] >= -step) {
                    gSaveData.volumes[VOLUME_MUSIC] += step;
                } else {
                    gSaveData.volumes[VOLUME_MUSIC] = 0;
                }
                break;
            case OPTION_SPEECH:
                if (gSaveData.volumes[VOLUME_SPEECH] >= -step) {
                    gSaveData.volumes[VOLUME_SPEECH] += step;
                } else {
                    gSaveData.volumes[VOLUME_SPEECH] = 0;
                }
                break;
            }
        } else {
            s32 vol;
            switch (gOptionsCursor) {
            case OPTION_EFFECTS:
                vol = gSaveData.volumes[VOLUME_EFFECTS] + step;
                if (vol >= 0x100) {
                    gSaveData.volumes[VOLUME_EFFECTS] = 0xFF;
                } else {
                    gSaveData.volumes[VOLUME_EFFECTS] = vol;
                }
                break;
            case OPTION_MUSIC:
                vol = gSaveData.volumes[VOLUME_MUSIC] + step;
                if (vol >= 0x100) {
                    gSaveData.volumes[VOLUME_MUSIC] = 0xFF;
                } else {
                    gSaveData.volumes[VOLUME_MUSIC] = vol;
                }
                break;
            case OPTION_SPEECH:
                vol = gSaveData.volumes[VOLUME_SPEECH] + step;
                if (vol >= 0x100) {
                    gSaveData.volumes[VOLUME_SPEECH] = 0xFF;
                } else {
                    gSaveData.volumes[VOLUME_SPEECH] = vol;
                }
                break;
            }
        }
    }
    /* only the chosen channel is heard */
    snd_set_volume(SND_VOL_EFFECTS, 0);
    snd_set_volume(SND_VOL_MUSIC, 0);
    snd_set_volume(SND_VOL_SPEECH, 0);
    switch (gOptionsCursor) {
    case OPTION_EFFECTS:
        snd_set_volume(SND_VOL_EFFECTS, gSaveData.volumes[VOLUME_EFFECTS]);
        break;
    case OPTION_MUSIC:
        snd_set_volume(SND_VOL_MUSIC, gSaveData.volumes[VOLUME_MUSIC]);
        break;
    case OPTION_SPEECH:
        snd_set_volume(SND_VOL_SPEECH, gSaveData.volumes[VOLUME_SPEECH]);
        break;
    }
    if (gOptionsCursor != prevCursor) {
        snd_play_menu_sound(MENU_SFX_MOVE);
    }
}

/* The pause menu's controller setup screen: A cycles the controller's button layout, B goes back. */
void race_pause_update_controller(void)
{
    s32 pad;
    s32 layout;

    if (((gPadInput_3[gGame.playerPad[gPausePlayer]] & INPUT_B) != 0) &&
        ((gPadInputPrev_3[gGame.playerPad[gPausePlayer]] & INPUT_B) == 0)) {
        gPauseMenu = PAUSE_MAIN;
        gPauseScreen.timer = 3;
        save_write(5, 0);
    } else {
        u32 *inputs = gPadInput_3;
        pad = gGame.playerPad[gPausePlayer];
        if (((inputs[pad] & INPUT_A) != 0) && ((gPadInputPrev_3[pad] & INPUT_A) == 0)) {
            layout = gGame.padConfig[pad] + 1;
            gGame.padConfig[pad] = layout;
            if (3 < (s8)layout) {
                gGame.padConfig[gGame.playerPad[gPausePlayer]] -= 4;
            }
            snd_play_menu_sound(MENU_SFX_MOVE);
        }
    }
}

#define PAUSE_PRESSED(m) ((gPadInput_3[PAUSE_PAD] & (m)) && !(gPadInputPrev_3[PAUSE_PAD] & (m)))

typedef struct { u32 w[6]; } Pad24;

/* gCurTrack, a gTracks record */
typedef struct {
    u8 pad0[0x2C];
    s32 envType;    /* 0x2C TRACK_ENV_* */
    u8 pad30[0x28];
    u16 unk_58[4];  /* per number of views */
    u16 unk_60[4];
} TrackViewInfo;

/* gSaveData's options and volumes */
typedef struct {
    u8 pad0[0x2C4];
    u8 options;
    u8 musicVolume;
    u8 effectsVolume;
    u8 speechVolume;
} SaveOptions;

/* pointers to values the debug menu may have tuned */
typedef struct {
    s32 *loDetailThresh;
    s32 *visibleThresh;
    s32 *w;
    s32 *h;
    s32 *riderCount;
    void *sfxVolume;
} DebugTunables;

extern TrackViewInfo *gCurTrack_15;
extern Pad24 gPlayerInputs_3;
extern Pad24 gPlayerInputsPrev_4;
extern Pad24 D_800E3750;
extern Pad24 D_800E3718;
extern SaveOptions gSaveData_3;

/* The race game state's update: maps the controllers to the players (D-pad as full stick), rumble, the pause
   screens' loading, the pause menus, the bikes, cameras and the event, START to pause, and the phases (fades,
   restarts). */
void race_update(u32 *pads)
{
    DebugTunables tunables;
    Pad16 playerPads;
    Pad16 unused;
    s32 w;
    s32 h;
    s32 menuChosen;
    s32 i;
    s32 j;
    s32 t;

    menuChosen = 0;
    if ((pads[0] >> INPUT_STICK_Y_SHIFT) == 3 && D_800C8CD0 != 0) {
        D_800C29DC = 0x80020000;
    }
    playerPads = *(Pad16 *)pads;
    mem_fill((u8 *)&playerPads, 0, 0x10);
    for (j = 0; j < gGame.numPlayers; j++) {
        playerPads.w[j] = pads[gGame.playerPad[j]];
    }
    mem_fill((u8 *)&gPlayerInputs_3, 0, 0x18);
    *(Pad16 *)&gPlayerInputs_3 = playerPads;
    for (i = 0; i < 4; i++) {
        gPlayerInputs_3.w[i] = bike_remap_buttons(gPlayerInputs_3.w[i], i);
    }
    /* the D-pad steers as the stick pushed all the way */
    for (i = 0; i < 4; i++) {
        if (gPlayerInputs_3.w[i] & INPUT_D_LEFT) gPlayerInputs_3.w[i] |= INPUT_STICK_X_MAX | INPUT_STICK_LEFT;
        if (gPlayerInputs_3.w[i] & INPUT_D_RIGHT) gPlayerInputs_3.w[i] |= INPUT_STICK_X_MAX | INPUT_STICK_RIGHT;
        if (gPlayerInputs_3.w[i] & INPUT_D_UP) gPlayerInputs_3.w[i] |= INPUT_STICK_Y_MAX | INPUT_STICK_UP;
        if (gPlayerInputs_3.w[i] & INPUT_D_DOWN) gPlayerInputs_3.w[i] |= INPUT_STICK_Y_MAX | INPUT_STICK_DOWN;
        if (gPlayerInputs_3.w[i] & INPUT_STICK_LEFT) gPlayerInputs_3.w[i] &= ~INPUT_STICK_RIGHT;
        if (gPlayerInputs_3.w[i] & INPUT_STICK_UP) gPlayerInputs_3.w[i] &= ~INPUT_STICK_DOWN;
        gPlayerInputs_3.w[i] &= ~(INPUT_D_UP | INPUT_D_DOWN | INPUT_D_LEFT | INPUT_D_RIGHT);
    }
    /* rumble: after a landing (as long as its impact / 20 frames), on tire tracks and on surfaces with a sound */
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
        if (gPauseMenu != PAUSE_NONE) j = 0;
        if (gRiders[i].isCpu != 0) j = 0;
        if (gGhostRace != 0 && i == gGhostSlot) j = 0;
        if (gReplayPlayback != 0) j = 0;
        if (gGame.fadeState != 3) j = 0;
        if (gGame.flags & GAME_FRONTEND_RACE) j = 0;
        if (gGame.mode == MODE_TUTORIAL && gEvent.state == EVENT_STATE_FREE_RIDE) j = 0;
        t = gGame.playerPad[i];
        if (j) rumble_start(t, 1);
    }
    shatter_update();
    snow_update();
    ambient_update();
    {
        s32 *loDetail = gLoDetailThresh;
        s32 *visible = gVisibleThresh;
        w = gCurTrack_15->unk_58[gNumViews - 1];
        h = gCurTrack_15->unk_60[gNumViews - 1];
        tunables.loDetailThresh = &loDetail[gNumViews - 1];
        tunables.visibleThresh = &visible[gNumViews - 1];
    }
    tunables.w = &w;
    tunables.h = &h;
    tunables.riderCount = &gDebugRiderCount;
    tunables.sfxVolume = &gSfxVolume;
    if (gPauseMenu == PAUSE_CONTROLLER || gPauseMenu == PAUSE_OPTIONS) {
        /* open the screen: its fonts, texts and pictures, in the Z buffer's memory, at 512x240 */
        if (gPauseScreen.timer == 0 && gPauseScreen.loaded == 0) {
            mem_free(gZBuffer);
            gPauseScreen.loaded = 1;
            if (gPauseMenu == PAUSE_CONTROLLER) {
                font_load_menu_set(gResourceSet);
            }
            res_open_set("guires", &gGuiRes);
            for (j = 0; j < MENU_NUM_TEXTS; j++) {
                gGame.texts[j] = font_text_create(0, 0x40, 4, 2, TEXT_VALIGN_CENTER);
            }
            if (gPauseMenu == PAUSE_CONTROLLER) {
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
            gViModeRequest = VI_MODE_MED_RES;
            while (gViMode != gViModeRequest) {
            }
            render_update_screen_layout();
        }
    } else if (gPauseMenu == PAUSE_MAIN && gPauseScreen.timer == 0 && gPauseScreen.loaded != 0) {
        /* back from the screen: free it, take the Z buffer back, restore the race's video mode and fonts */
        gPauseScreen.loaded = 0;
        gPauseScreen.clearFrames = 2;
        for (j = 0; j < MENU_NUM_TEXTS; j++) {
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
        if (!(gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) && (gSaveData_3.options & SAVE_OPT_HIGH_RES) &&
            gHasExpansionPak != 0 && (gGame.numPlayers == 1 || gGame.mode == MODE_TIME_TRIAL) &&
            gCurTrack_15->envType != TRACK_ENV_DESERT && !(gGame.flags & (GAME_LOW_RES | GAME_PODIUM))) {
            gViModeRequest = VI_MODE_HI_RES;
        } else {
            gViModeRequest = VI_MODE_MED_RES;
        }
        while (gViMode != gViModeRequest) {
        }
        render_update_screen_layout();
        font_load_race_set(gResourceSet);
        gPauseScreen.timer = -1;
    }
    /* C-right on controller 1 cycles the HUD's levels of detail */
    if (gGame.mode != MODE_TUTORIAL && !(gGame.flags & (GAME_REPLAY | GAME_RIDER_SELECT)) &&
        (gPadInput_3[0] & INPUT_C_RIGHT) && !(gPadInputPrev_3[0] & INPUT_C_RIGHT)) {
        gHudLevel = (gHudLevel + 1) & 3;
    }
    particles_emit_bike_dirt();
    tube_animate_textures();
    if (!(gGame.flags & GAME_TRACK_INTRO)) {
        /* A or START ends a replay */
        if (gReplayPlayback != 0 && gRacePhase_2 == RACE_PHASE_RACING &&
            ((gPlayerInputs_3.w[0] | gPlayerInputs_3.w[1] | gPlayerInputs_3.w[2] | gPlayerInputs_3.w[3]) &
             (INPUT_A | INPUT_START))) {
            gRacePhase_2 = RACE_PHASE_FADE_OUT;
            gRacePhaseTimer = PHASE_TIMER_START;
            gRaceExitCode = RACE_EXIT_FINISHED;
            gGame.flags &= ~GAME_REPEAT_STATE;
            snd_play_menu_sound(MENU_SFX_SELECT);
            return;
        }
    } else {
        race_update_intro();
    }

    if (gPauseMenu == PAUSE_CONTROLLER) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            race_pause_update_controller();
        }
    } else if (gPauseMenu == PAUSE_OPTIONS) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            race_pause_update_options();
        }
    } else if (gPauseMenu == PAUSE_TEST_RIDE) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            if (PAUSE_PRESSED(INPUT_B)) {
                gPauseMenu = PAUSE_NONE;
                snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
                snd_play_menu_sound(MENU_SFX_BACK);
            } else if (PAUSE_PRESSED(INPUT_A) || PAUSE_PRESSED(INPUT_START)) {
                snd_play_menu_sound(MENU_SFX_SELECT);
                menuChosen = 1;
                switch (gPauseMenuCursor) {
                case TEST_RIDE_ITEM_CONTINUE:
                    gPauseMenu = PAUSE_NONE;
                    snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                    snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                    snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
                    break;
                case TEST_RIDE_ITEM_RESET:
                    bikes_free();
                    mem_fill((u8 *)gRiders, 0, sizeof(Rider) * 8);
                    bikes_init();
                    gPauseMenu = PAUSE_NONE;
                    gEvent.flags |= EVENT_CAM_RESET;
                    snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                    snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                    snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
                    break;
                case TEST_RIDE_ITEM_EDITOR:
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    gRaceExitCode = RACE_EXIT_EDITOR;
                    break;
                }
            } else if (PAUSE_PRESSED(INPUT_STICK_UP) || PAUSE_PRESSED(INPUT_D_UP)) {
                gPauseMenuCursor = (gPauseMenuCursor + 2) % 3;
                snd_play_menu_sound(MENU_SFX_MOVE);
            } else if (PAUSE_PRESSED(INPUT_STICK_DOWN) || PAUSE_PRESSED(INPUT_D_DOWN)) {
                gPauseMenuCursor = (gPauseMenuCursor + 1) % 3;
                snd_play_menu_sound(MENU_SFX_MOVE);
            }
        }
        if (gPauseMenu == PAUSE_NONE) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == PAUSE_TUTORIAL) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            if (PAUSE_PRESSED(INPUT_B) && gTutorialMenuForced_2 == 0 && gTutorialResult == 0) {
                gPauseMenu = PAUSE_NONE;
                snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
                snd_play_menu_sound(MENU_SFX_BACK);
            } else if (PAUSE_PRESSED(INPUT_A) || PAUSE_PRESSED(INPUT_START)) {
                snd_play_menu_sound(MENU_SFX_SELECT);
                menuChosen = 1;
                switch (gPauseMenuCursor) {
                case TUTORIAL_ITEM_PREV:
                    gEvent.tutorialAction = TUTORIAL_PREV;
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    break;
                case TUTORIAL_ITEM_NEXT:
                    gEvent.tutorialAction = TUTORIAL_NEXT;
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    break;
                case TUTORIAL_ITEM_DEMO:
                    gEvent.tutorialAction = TUTORIAL_DEMO;
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    break;
                case TUTORIAL_ITEM_RETRY:
                    gEvent.tutorialAction = TUTORIAL_RETRY;
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    break;
                case TUTORIAL_ITEM_EXIT:
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    gRaceExitCode = RACE_EXIT_FINISHED;
                    gGame.flags &= ~GAME_REPEAT_STATE;
                    break;
                }
            } else if (PAUSE_PRESSED(INPUT_STICK_UP) || PAUSE_PRESSED(INPUT_D_UP)) {
                /* skip the items that are not shown */
                do {
                    gPauseMenuCursor = (gPauseMenuCursor + 4) % 5;
                } while ((gPauseMenuCursor == TUTORIAL_ITEM_NEXT && tutorial_next_lesson_unlocked() == 0) ||
                         (gPauseMenuCursor == TUTORIAL_ITEM_DEMO && tutorial_lesson_has_demo() == 0) ||
                         (gPauseMenuCursor == TUTORIAL_ITEM_PREV && tutorial_has_prev_lesson() == 0));
                snd_play_menu_sound(MENU_SFX_MOVE);
            } else if (PAUSE_PRESSED(INPUT_STICK_DOWN) || PAUSE_PRESSED(INPUT_D_DOWN)) {
                do {
                    gPauseMenuCursor = (gPauseMenuCursor + 1) % 5;
                } while ((gPauseMenuCursor == TUTORIAL_ITEM_NEXT && tutorial_next_lesson_unlocked() == 0) ||
                         (gPauseMenuCursor == TUTORIAL_ITEM_DEMO && tutorial_lesson_has_demo() == 0) ||
                         (gPauseMenuCursor == TUTORIAL_ITEM_PREV && tutorial_has_prev_lesson() == 0));
                snd_play_menu_sound(MENU_SFX_MOVE);
            }
            if (gPauseMenu == PAUSE_NONE) {
                sfx_unmute();
            }
        }
        if (gPauseMenu == PAUSE_NONE) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == PAUSE_MAIN) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            if (PAUSE_PRESSED(INPUT_B)) {
                snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
                gPauseMenu = PAUSE_NONE;
                snd_play_menu_sound(MENU_SFX_BACK);
            } else if (PAUSE_PRESSED(INPUT_A) || PAUSE_PRESSED(INPUT_START)) {
                snd_play_menu_sound(MENU_SFX_SELECT);
                menuChosen = 1;
                switch (gPauseMenuCursor) {
                case PAUSE_ITEM_CONTROLLER:
                    gPauseMenu = PAUSE_CONTROLLER;
                    sOptionsPreviewDelay = 0;
                    gOptionsCursor = gGame.padPlayer[PAUSE_PAD];
                    gPauseScreen.timer = 3;
                    break;
                case PAUSE_ITEM_OPTIONS:
                    gPauseMenu = PAUSE_OPTIONS;
                    gPauseScreen.timer = 3;
                    gOptionsCursor = OPTION_MUSIC;
                    break;
                case PAUSE_ITEM_RESTART:
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    gGame.flags |= GAME_REPEAT_STATE;
                    if (gGame.mode == MODE_SEASON) {
                        event_clear_season_race_points();
                    }
                    break;
                case PAUSE_ITEM_QUIT:
                    gPauseMenu = PAUSE_QUIT;
                    gPauseMenuCursor = 1;
                    break;
                case PAUSE_ITEM_CONTINUE:
                case 5:
                    gPauseMenu = PAUSE_NONE;
                    break;
                }
            } else if (PAUSE_PRESSED(INPUT_STICK_UP) || PAUSE_PRESSED(INPUT_D_UP)) {
                /* the season's menu has no Restart Race */
                gPauseMenuCursor = (gPauseMenuCursor + (gGame.mode == MODE_SEASON ? 3 : 4)) % (gGame.mode == MODE_SEASON ? 4 : 5);
                snd_play_menu_sound(MENU_SFX_MOVE);
            } else if (PAUSE_PRESSED(INPUT_STICK_DOWN) || PAUSE_PRESSED(INPUT_D_DOWN)) {
                gPauseMenuCursor = (gPauseMenuCursor + 1) % (gGame.mode == MODE_SEASON ? 4 : 5);
                snd_play_menu_sound(MENU_SFX_MOVE);
            }
            if (gPauseMenu == PAUSE_NONE) {
                sfx_unmute();
                snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
            }
        }
        if (gPauseMenu == PAUSE_NONE) {
            gPauseScreen.clearFrames = 2;
        }
    } else if (gPauseMenu == PAUSE_QUIT) {
        if (gRacePhase_2 == RACE_PHASE_RACING && gPauseScreen.timer == -1) {
            if (PAUSE_PRESSED(INPUT_B)) {
                gPauseMenu = PAUSE_NONE;
                snd_play_menu_sound(MENU_SFX_BACK);
                snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                snd_set_volume(SND_VOL_MUSIC, gSaveData_3.musicVolume);
                snd_set_volume(SND_VOL_SPEECH, gSaveData_3.speechVolume);
            } else if (PAUSE_PRESSED(INPUT_A)) {
                switch (gPauseMenuCursor) {
                case 0: /* Really Quit */
                    gRacePhase_2 = RACE_PHASE_FADE_OUT;
                    gRacePhaseTimer = PHASE_TIMER_START;
                    gRaceExitCode = RACE_EXIT_QUIT;
                    gGame.flags &= ~GAME_REPEAT_STATE;
                    snd_play_menu_sound(MENU_SFX_SELECT);
                    if (gGame.mode == MODE_SEASON) {
                        event_clear_season_race_points();
                    }
                    break;
                case 1: /* Just Kidding */
                    snd_play_menu_sound(MENU_SFX_SELECT);
                    gPauseMenu = PAUSE_MAIN;
                    gPauseMenuCursor = PAUSE_ITEM_CONTINUE;
                    break;
                }
            } else if (PAUSE_PRESSED(INPUT_STICK_UP) || PAUSE_PRESSED(INPUT_D_UP)) {
                gPauseMenuCursor = 0;
                snd_play_menu_sound(MENU_SFX_MOVE);
            } else if (PAUSE_PRESSED(INPUT_STICK_DOWN) || PAUSE_PRESSED(INPUT_D_DOWN)) {
                gPauseMenuCursor = 1;
                snd_play_menu_sound(MENU_SFX_MOVE);
            }
        }
        if (gPauseMenu == PAUSE_NONE) {
            gPauseScreen.clearFrames = 2;
        }
    } else {
        if (gEvent.flags & EVENT_MESSAGE) {
            /* a tutorial message holds the game until it times out; A ends it */
            event_update(0);
            snd_set_volume(SND_VOL_EFFECTS, 0);
            if (gEvent.tutorialTimeout != 0) {
                if ((gPlayerInputs_3.w[0] & INPUT_A) && !(gPlayerInputsPrev_4.w[0] & INPUT_A)) {
                    snd_play_menu_sound(MENU_SFX_SELECT);
                    gEvent.tutorialTimeout = 1;
                }
                if (--gEvent.tutorialTimeout == 0) {
                    gEvent.flags &= ~(EVENT_CAM_UPDATE | EVENT_INTRO_MESSAGE | EVENT_MESSAGE);
                    snd_set_volume(SND_VOL_EFFECTS, gSaveData_3.effectsVolume);
                }
            }
            if (gEvent.flags & EVENT_CAM_UPDATE) {
                gEvent.flags &= ~EVENT_CAM_UPDATE;
                gCamView = 0;
                cam_update();
            }
        } else {
            D_800E3750 = gPlayerInputs_3;
            race_update_replay();
            if (gReplayPlayback != 0 && gCamTargetOverride != -1 && !(gGame.flags & GAME_TRACK_INTRO)) {
                /* replays: C-right on any controller follows the next player */
                for (j = 0; j < 4; j++) {
                    if ((gPadInput_3[j] & INPUT_C_RIGHT) && !(gPadInputPrev_3[j] & INPUT_C_RIGHT)) break;
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
            if (gGame.flags & GAME_RIDER_SELECT) {
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
            /* the riders' hands twist the throttle open and back */
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

    /* START pauses (in the tutorial, START on any controller) */
    if (gReplayPlayback == 0 && gEvent.allFinished == 0 && gPauseMenu == PAUSE_NONE &&
        !(gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO))) {
        for (j = 0; j < gEvent.numRiders; j++) {
            if (((gPlayerInputs_3.w[j] & INPUT_START) && !(gPlayerInputsPrev_4.w[j] & INPUT_START)) ||
                (gGame.mode == MODE_TUTORIAL && (gPadInput_3[j] & INPUT_START) && !(gPadInputPrev_3[j] & INPUT_START))) {
                if (menuChosen == 0 && gRacePhase_2 == RACE_PHASE_RACING) {
                    gPlayerInputsPrev_4.w[j] |= INPUT_START;
                    if (gPodiumMode != 0) {
                        gRacePhase_2 = RACE_PHASE_FADE_OUT;
                        gRacePhaseTimer = PHASE_TIMER_START;
                        gRaceExitCode = RACE_EXIT_QUIT;
                        gGame.flags &= ~GAME_REPEAT_STATE;
                    } else if (gGame.mode == MODE_TEST_RIDE || gGame.mode == MODE_TEST_RIDE_OPEN) {
                        gPauseMenu = PAUSE_TEST_RIDE;
                        gPauseMenuCursor = TEST_RIDE_ITEM_CONTINUE;
                    } else if (gGame.mode == MODE_TUTORIAL) {
                        gPauseMenu = gRacePhase_2; /* RACE_PHASE_RACING, which is PAUSE_TUTORIAL */
                        gPauseMenuCursor = TUTORIAL_ITEM_RETRY;
                    } else {
                        gPauseMenu = PAUSE_MAIN;
                        gPauseMenuCursor = PAUSE_ITEM_CONTINUE;
                    }
                    gPausePlayer = j;
                    snd_stop_all_riders();
                    snd_set_volume(SND_VOL_EFFECTS, 0);
                    snd_set_volume(SND_VOL_MUSIC, 0);
                    snd_set_volume(SND_VOL_SPEECH, 0);
                    tunes_stop_speech();
                    rumble_stop_all();
                    break;
                }
            }
        }
    }

    switch (gRacePhase_2) {
    case RACE_PHASE_FADE_IN:
        if (gRacePhaseTimer != PHASE_TIMER_START) {
            if (--gRacePhaseTimer == 0) {
                gRacePhase_2 = RACE_PHASE_RACING;
            }
        }
        break;
    case RACE_PHASE_FADE_OUT:
        if (gRacePhaseTimer == PHASE_TIMER_FADED) {
            if ((gGame.flags & GAME_REPEAT_STATE) || (gGame.mode == MODE_TUTORIAL && gRaceExitCode == RACE_EXIT_NONE)) {
                /* restart */
                gRacePhase_2 = RACE_PHASE_DONE;
                gRacePhaseTimer = 2;
            } else {
                gRacePhase_2 = RACE_PHASE_DONE;
                gRacePhaseTimer = 100;
                gCurGameState->mode = STATE_EXIT;
            }
        } else if (gRacePhaseTimer > 0) {
            if (--gRacePhaseTimer == 0) {
                gRacePhaseTimer = PHASE_TIMER_OVER;
            }
        }
        break;
    case RACE_PHASE_DONE:
        if (gRacePhaseTimer == 0) {
            /* restart the race (in the tutorial, the lesson the menu chose) */
            gRacePhase_2 = RACE_PHASE_FADE_IN;
            gRacePhaseTimer = PHASE_TIMER_START;
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
            if (gGame.mode == MODE_TUTORIAL) path_free();
            if (gDrawFlags & DRAW_HUD) event_hud_free();
            if (gGame.mode == MODE_TUTORIAL) {
                switch (gEvent.tutorialAction) {
                case TUTORIAL_PREV:
                    gSeason->round--;
                    break;
                case TUTORIAL_NEXT:
                    gSeason->round++;
                    break;
                case TUTORIAL_RETRY:
                    gEvent.flags |= EVENT_NO_DEMO;
                    break;
                case TUTORIAL_DEMO:
                    break;
                default:
                    /* anything else: the lesson again, from its demo */
                    gEvent.state = EVENT_STATE_FREE_RIDE;
                    gEvent.countdown = 0x3F;
                    gEvent.tutorialTimer = 0x3C;
                    gEvent.tutorialAction = TUTORIAL_SHOW;
                    gEvent.flags = (gEvent.flags & ~EVENT_STOPPED) | EVENT_DEMO;
                    break;
                }
                gPauseMenu = PAUSE_NONE;
                gEvent.flags &= ~EVENT_RIDER_STOPPED;
                if (gTutorialLessons[gSeason->round].flags & LESSON_TAKEDOWN) {
                    gNumRiders = 2;
                } else {
                    gNumRiders = 1;
                }
            } else if (gGame.mode == MODE_EXCITE_3D) {
                gNumRiders = 4;
                gGame.numPlayers = 1;
            } else if (gGame.mode == MODE_STUNT || gGame.mode == MODE_SOCCER) {
                gNumRiders = gGame.numPlayers;
            } else if (gGame.mode == MODE_EXHIBITION && gGame.numPlayers == 2) {
                gNumRiders = gGame.twoPlayerCpuRiders + 2;
            } else {
                gNumRiders = gGame.numPlayers + gGame.numCpuRiders;
                if (gGame.mode == MODE_TIME_TRIAL && !(gGame.flags & GAME_FRONTEND_RACE)) {
                    gNumRiders = 1;
                }
            }
            race_init_replay();
            event_init(gNumRiders, gGame.numLaps);
            race_begin();
            if (gGame.mode == MODE_TUTORIAL) event_stop_sounds_rumble();
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
    if (gCurTrack_15->envType == TRACK_ENV_DESERT) {
        fractal_emit_fire_smoke();
    }
    hacks_update();
}

typedef struct { u8 b[12]; } Colors12;

/* A ground surface in the track's materials resource */
typedef struct {
    char name[0x10];
    s32 params[6];      /* the Surface's six floats from +0x10, in thousandths */
    s32 flags;
    Colors12 colors;
    s32 soundKey;       /* looked up in the surface sounds table */
} SurfaceRes;

typedef struct {
    s32 count;
    SurfaceRes e[1];
} SurfaceFile;

struct Res_3 {
    s32 f0;
    u32 *arr;
};


/* Fills gSurfaceTable from the track's materials resource, with each surface's rolling sound. */
void race_load_surface_table(Res_3 *resSet, s32 resId)
{
    Pair_4 *sounds;
    SurfaceFile *file;
    s32 i, j;

    sounds = res_load(gSfxResSet_3, sSurfaceSoundsResId);
    file = res_load(resSet, resId);
    for (i = 0; i < 16; i++) {
        str_copy(gSurfaceTable[i].name, file->e[i].name);
        for (j = 0; j < 6; j++) {
            ((f32 *)&gSurfaceTable[i])[j + 4] = (f32)file->e[i].params[j] / 1000.0f;
        }
        gSurfaceTable[i].flags = file->e[i].flags;
        gSurfaceTable[i].color[0] = file->e[i].colors.b[0];
        gSurfaceTable[i].color[1] = file->e[i].colors.b[1];
        gSurfaceTable[i].color[2] = file->e[i].colors.b[2];
        gSurfaceTable[i].color[3] = file->e[i].colors.b[3];
        gSurfaceTable[i].color[4] = file->e[i].colors.b[4];
        gSurfaceTable[i].color[5] = file->e[i].colors.b[5];
        gSurfaceTable[i].color[6] = file->e[i].colors.b[6];
        gSurfaceTable[i].color[7] = file->e[i].colors.b[7];
        gSurfaceTable[i].color[8] = file->e[i].colors.b[8];
        gSurfaceTable[i].color[9] = file->e[i].colors.b[9];
        gSurfaceTable[i].color[10] = file->e[i].colors.b[10];
        gSurfaceTable[i].color[11] = file->e[i].colors.b[11];
        for (j = 0; sounds[j].key != 0; j++) {
            if (sounds[j].key == file->e[i].soundKey) {
                break;
            }
        }
        gSurfaceTable[i].sfxId = sounds[j].val;
    }
    res_unload(&resSet->arr[resId]);
    res_unload(&gSfxResSet_3->arr[sSurfaceSoundsResId]);
}

typedef struct { u8 b[48]; } Blk48;
typedef struct { u8 b[16]; } Blk16;

typedef struct {
    u8 pad[0x50];
    s32 flags;      /* TRACK_FLAG_* */
} TrackFlags;

/* The header the boot loader left (gBootHeader): the BRSOHYES container, its four SWD-packed segments */
typedef struct {
    s32 tag[2];
    s32 s0Size;
    s32 s0Packed;
    s32 s1Size;     /* the front-end code */
    s32 s1Packed;
    s32 s2Size;     /* UsrTrk.c */
    s32 s2Packed;
    s32 s3Size;
    s32 s3Packed;
    s32 romAddr;    /* the container's: 0x28 bytes of header, then the segments */
} BootHeader;

extern TrackFlags *gCurTrack_16;
extern BootHeader gBootHeader_2;

/* Gives the front-end code and the UsrTrk.c overlay (not on the user track) back to the heap during a race
   (load 0), or reloads them from the ROM (load 1): uncompressed after a direct load, else SWD-unpacked from the
   container. The blocks' heap headers are kept across the unpacking. */
void race_load_frontend_code(s32 load)
{
    Blk48 hdr;
    s32 dma;
    s32 rom;
    s32 size;
    s32 last;
    u8 *frontendStart;
    u8 *usrTrkStart;
    u8 *usrTrkEnd;

    if (load == sFrontendLoaded || (gGame.flags & GAME_FRONTEND_RACE)) {
        return;
    }
    frontendStart = gFrontendOvlStart;
    usrTrkStart = gUsrTrkOvlStart;
    usrTrkEnd = gUsrTrkOvlEnd;
    if ((u32)frontendStart & 0xF) fatal_halt(0x33);
    if ((u32)usrTrkStart & 0xF) fatal_halt(0x34);
    if ((u32)usrTrkEnd & 0xF) fatal_halt(0x35);
    if (load != 0) {
        size = (u32)&gUsrTrkOvlStart[-0x30] - (u32)frontendStart;
        mem_alloc_at_dbg(frontendStart + 0x30, size, sStrMinFile, 0xBDD);
        if (!(gCurTrack_16->flags & TRACK_FLAG_USER)) {
            mem_alloc_at_dbg(usrTrkStart + 0x10, (u32)&gUsrTrkOvlEnd[-0x10] - (u32)usrTrkStart, sStrMinFile, 0xBE0);
        }
        osWritebackDCacheAll();
        if (gLoadedByBootLoader == 0) {
            rom = 0xB0000C00;
            while ((dma = dma_start(gCartRomHandle, 0, (s32)frontendStart + rom + 0x30, (s32)(frontendStart + 0x30), size)) < 0);
            while (dma_get_status(dma) != 3);
            dma_release(dma);
            while ((dma = dma_start(gCartRomHandle, 0, rom + (s32)usrTrkStart + 0x10, (s32)(usrTrkStart + 0x10), (last = (s32)usrTrkEnd - 0x10) - (s32)usrTrkStart)) < 0);
            while (dma_get_status(dma) != 3);
            dma_release(dma);
        } else {
            /* the packed segment goes to the Z buffer, then unpacks to its place (the source end is s0Packed past
               the start for both) */
            while (sched_get_current_task() != 0);
            rom = gBootHeader_2.romAddr + 0x28;
            rom += gBootHeader_2.s0Packed;
            while ((dma = dma_start(gCartRomHandle, 0, rom, gZBuffer, gBootHeader_2.s1Packed)) < 0);
            while (dma_get_status(dma) != 3);
            dma_release(dma);
            gSwdDst = frontendStart;
            gSwdDstEnd = usrTrkStart;
            gSwdSrc = gZBuffer;
            gSwdSrcEnd = gZBuffer + gBootHeader_2.s0Packed;
            hdr = *(Blk48 *)frontendStart;
            swd_decompress();
            *(Blk48 *)frontendStart = hdr;
            rom += gBootHeader_2.s1Packed;
            while ((dma = dma_start(gCartRomHandle, 0, rom, gZBuffer, gBootHeader_2.s2Packed)) < 0);
            while (dma_get_status(dma) != 3);
            dma_release(dma);
            gSwdDst = usrTrkStart;
            gSwdDstEnd = usrTrkEnd;
            __builtin_memcpy(&hdr, usrTrkStart, 16);
            gSwdSrc = gZBuffer;
            gSwdSrcEnd = gZBuffer + gBootHeader_2.s0Packed;
            swd_decompress();
            *(Blk16 *)usrTrkStart = *(Blk16 *)&hdr;
        }
        osInvalDCache(0, 0x800000);
        osInvalICache(0, 0x800000);
        sFrontendLoaded = 1;
    } else {
        mem_free(frontendStart + 0x30);
        if (!(gCurTrack_16->flags & TRACK_FLAG_USER)) {
            mem_free(usrTrkStart + 0x10);
        }
        sFrontendLoaded = 0;
    }
}

/* Starts the time trial's recording of the player (and, with a ghost, its playback), with the ghost in the other
   rider slot. */
void race_init_ghost_replay(s32 ghostData)
{
    if (ghostData != 0) {
        Game *game;
        gGhostSlot ^= 1;
        game = &gGame;
        game->numPlayers = 1;
        game->numCpuRiders = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gNumRiders = 2;
        game->numPlayers = 2;
        gGhostSlot ^= 1;
    } else {
        Game *game = &gGame;
        game->numPlayers = 1;
        game->numCpuRiders = 0;
        gNumRiders = 1;
        gGhostSlot = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gGhostSlot = 1;
    }
    if (gGhostRace != 0) {
        Game *game = &gGame;
        if (game->flags & GAME_GHOST) {
            game->numPlayers = 1;
            replay_play_start_keep_setup(gActiveReplay + 0x6400);
            game->numPlayers = 2;
        }
    }
}

/* Frees what race_begin set up. */
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
    if (gGame.mode == MODE_TUTORIAL) {
        path_free();
    }
    if (gDrawFlags & DRAW_HUD) {
        event_hud_free();
    }
}

/* Sets the effects, music and speech volumes from the save data (SaveData.volumes at 0x2C5). */
void race_apply_volumes(void)
{
    u8 *save = &gSaveData_4[0];
    snd_set_volume(SND_VOL_EFFECTS, save[0x2C6]);
    snd_set_volume(SND_VOL_MUSIC, save[0x2C5]);
    snd_set_volume(SND_VOL_SPEECH, save[0x2C7]);
}

/* The time trial's ghost from a replay: its header sets the riders up; the player keeps their rider, controller and
   skins in the slot the ghost does not take. */
void race_load_ghost_setup(s32 replay)
{
    Game *game;
    u8 *base;
    s32 rider;
    s32 pad;
    u8 riderSkin;
    u8 bikeSkin;
    s32 slot;

    if (replay != 0) {
        /* base + 0x3700 is gGhostSlot */
        base = (u8 *)0x800E0000;
        __asm__ __volatile__("" : "+r"(base));
        game = &gGame;
        rider = game->riders[0];
        __asm__ __volatile__("" : "+r"(rider) : : "memory");
        *(s32 *)(base + 0x3700) = -1;
        pad = game->playerPad[0];
        riderSkin = game->riderSkins[0];
        bikeSkin = game->bikeSkins[0];
        replay_play_start(replay);
        slot = *(s32 *)(base + 0x3700) ^ 1;
        game->riders[slot] = rider;
        game->playerPad[slot] = pad;
        game->riderSkins[*(s32 *)(base + 0x3700) ^ 1] = riderSkin;
        game->bikeSkins[*(s32 *)(base + 0x3700) ^ 1] = bikeSkin;
        game->playerPad[*(s32 *)(base + 0x3700)] = 0;
    }
}

/* Waits for the video mode the race uses (race_init's test). */
void race_set_video_mode(void)
{
    if ((gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) == 0 && (gSaveData.options & SAVE_OPT_HIGH_RES) != 0 &&
        gHasExpansionPak != 0 && (gGame.numPlayers == 1 || gGame.mode == MODE_TIME_TRIAL) &&
        gCurTrack_3[11] != TRACK_ENV_DESERT && (gGame.flags & (GAME_LOW_RES | GAME_PODIUM)) == 0) {
        gViModeRequest = VI_MODE_HI_RES;
    } else {
        gViModeRequest = VI_MODE_MED_RES;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();
}

/* Creates the race's text objects (the podium uses them). */
void race_alloc_text_objs(void)
{
    s32 i;
    TextObj *text;

    gRaceTextObjs = mem_alloc_dbg(sizeof(RaceTexts), 0, "min.c", 0x4E3);
    i = 0;
    do {
        text = font_text_create(NULL, 0x40, 1, 2, TEXT_VALIGN_CENTER);
        gRaceTextObjs->text[i] = text;
        i++;
    } while (i < RACE_NUM_TEXTS);
}

void race_free_text_objs(void)
{
    s32 i;

    i = 0;
    do {
        font_text_destroy(gRaceTextObjs->text[i]);
        mem_free(gRaceTextObjs->text[i]);
        i++;
    } while (i < RACE_NUM_TEXTS);
    mem_free(gRaceTextObjs);
}

void race_mute_volumes(void)
{
    snd_set_volume(SND_VOL_EFFECTS, 0);
    snd_set_volume(SND_VOL_MUSIC, 0);
    snd_set_volume(SND_VOL_SPEECH, 0);
}
