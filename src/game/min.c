#include "game.h"

typedef struct { u8 pad[0x64]; s16 idx; } PtrA;

extern PtrA *gSeason_3;

typedef struct {
    s32 f[7];
    s32 *p;
    s32 g[2];
} Ent40_2;

extern Ent40_2 gTutorialLessons_3[];

void race_init_replay(void)
{
    s32 *piVar1;

    sDemoActive = 0;
    gDemoPlayback = 0;
    gReplayPlayback = 0;
    if (gGame_4[0x13] == 8 && (gGame_4[0] & 0x80000) != 0) {
        if ((gGame_4[0] & 0x80) != 0) {
            func_80019EC0((s32)(gActiveReplay_2 + 0x1900));
        }
    }
    if ((gGame_4[0] & 0x81) == 0 && gGame_4[0x13] == 8) {
        gGhostRace = 1;
        if ((gGame_4[0] & 0x80000) != 0) {
            sGhostData = gActiveReplay_2 + 0x1900;
        } else {
            sGhostData = 0;
        }
    } else if ((gGame_4[0] & 0x8000) != 0) {
        gReplayPlayback = 1;
        sReplayRequest = 0;
        replay_play_start_2((s32)gActiveReplay_2);
    } else {
        if ((gGame_4[0] & 0x81) == 0 && gGame_4[0x13] == 9 &&
            gTutorialLessons_3[*(s16 *)(gSeason + 0x64)].p != 0 && (gEvent_3[22] & 0x4000) == 0) {
            gEvent_3[22] = gEvent_3[22] & 0xffffbfff;
            gGhostRace = 0;
            gDemoPlayback = 1;
            sDemoActive = 1;
            if ((gGame_4[0] & 0x4000) != 0) {
                gActiveReplay_2 = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
            } else {
                piVar1 = res_load(D_800E7728, *gTutorialLessons_3[*(s16 *)(gSeason + 0x64)].p);
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
    if (gGame_4[0x13] == 9 || gGhostRace != 0 || gDemoPlayback != 0 ||
        (gGame_4[0] & 0x4001) != 0 || gGame_4[0x13] == 0xd ||
        gGame_4[0x13] == 0xe || (gGame_4[0] & 0x80) != 0) {
        gReplayRecording = 0;
    } else {
        gReplayRecording = 1;
    }
    if (gReplayRecording != 0 && gReplayPlayback == 0 && gActiveReplay_2 != 0) {
        replay_record_start((s32)gActiveReplay_2, gActiveReplaySize);
    }
}

typedef struct { u8 pad[0x1C]; s32 *ptr; u8 pad2[8]; } Ent_10;

extern Ent_10 gTutorialLessons_4[];

void race_finish_replay(s32 param_1)
{
    u32 *g = (u32 *)gGame_5;
    if ((g[0] & 0x85) != 0) {
        if ((g[0] & 0x80) == 0) {
            return;
        }
        if (gRaceExitCode != 3) {
            return;
        }
    }
    if (gGhostRace == 0) {
        if (gDemoPlayback != 0) {
            gActiveReplay = 0;
            res_unload((u32 *)((s32 *)D_800E7728_2[1] + gTutorialLessons_4[gSeason_3->idx].ptr[0]));
            goto T;
        }
        if (gRaceExitCode == 3 || g[19] == 9) goto T;
        if (g[19] < 10) goto F0;
        if (g[19] >= 15) goto CALL;
        if (g[19] < 13) goto CALL;
        goto T;
F0:
        if (g[19] == 2) goto T;
    }
CALL:
    replay_record_finish();
T:
    if (sReplayRequest == 0 && param_1 != 0) {
        race_load_frontend_code(1);
    }
}

typedef struct Blk_5 {
    u8 pad0[0x58];
    s32 f58;
} Blk_5;

extern Blk_5 gEvent_7;

void race_update_replay(void)
{
    if (gReplayRecording != 0) {
        if (gReplayPlayback == 0) {
            if (gGameMode == 9 || gEventAllFinished == 0) {
                replay_record_frame(gPlayerInputs);
            }
        } else if (gGameMode == 9 || gEventAllFinished == 0) {
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
        if ((gEvent_7.f58 & 0x40) && replay_play_frame(gPlayerInputs) != 0) {
            bikes_free();
            decal_clear_all();
            mem_fill((u8 *)gRiders, 0, 0x3F00);
            tutorial_init_lesson(0);
            bikes_init();
            gEvent_7.f58 = (gEvent_7.f58 | 0x80) & ~0x2000;
            replay_play_start(gActiveReplay);
        }
    }
    decal_update_tire_tracks();
    tube_update_splashes();
}

typedef struct {
    u32 flags;      /* 0x00 */
    u8 pad04[0x38];
    s32 f3C;        /* 0x3C */
    s32 f40;        /* 0x40 */
    s32 f44;        /* 0x44 */
    s32 f48;
    s32 mode;       /* 0x4C */
    u8 pad50[0x70C];
    s32 f75C;       /* 0x75C */
} Game_9;

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

typedef struct {
    s16 a;
    u8 b;
    u8 c;
} Small;

typedef struct {
    u8 pad[0xC];
    u8 fl;
    u8 pad0D[0x1B];
} Tab28;

typedef struct {
    u8 pad[0x22];
    s16 h22;
} Lim;

extern Game_9 gGame_19;
extern Obj_6 *gCurTrack_14;
extern Small D_800E3730_5;
extern Tab28 gTutorialLessons_5[];
extern Lim gEvent_8;

void race_init(s32 arg)
{
    u16 *p;
    Ent1C *q;
    Obj_6 *o;
    s32 i;
    s32 v;

    if (!(gGame_19.flags & 0x81) && (D_800F1BC4 & 2) && gHasExpansionPak != 0
        && (gGame_19.f3C == 1 || gGame_19.mode == 8) && gCurTrack_14->f2C != 2
        && !(gGame_19.flags & 0x04040000)) {
        gViModeRequest = 2;
    } else {
        gViModeRequest = 1;
    }
    while (gViMode != gViModeRequest) {
    }
    render_update_screen_layout();

    if ((gGame_19.mode == 0 || gGame_19.mode == 1 || gGame_19.mode == 8) && (gCheatFlags & 2)
        && !(gGame_19.flags & 0x40001)) {
        gMirrorMode = 1;
    } else {
        gMirrorMode = 0;
    }
    gPodiumMode = 0;
    D_800C8CE0 = 0;
    gPauseMenu = 0;
    D_800E3730_5.a = -1;
    D_800E3730_5.b = 0;
    D_800E3730_5.c = 0;
    gSchedFrameLimit = 1;
    gGame_19.flags |= 0x10000;
    race_load_frontend_code(0);
    if (gReplayPlayback != 0) {
        gCamTargetOverride = D_800F05EC;
    } else {
        gCamTargetOverride = -1;
    }
    osWritebackDCacheAll();
    res_open_set(D_800D6160, &D_800E7728);
    if (gCurTrack_14 != NULL) {
        track_res_load_map(gCurTrack_14->p80);
    }
    p = (u16 *)track_res_get(D_800D151C);
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
    if (gGame_19.flags & 0x40080) {
        font_load_menu_set(D_800E7728);
    } else if (!(gGame_19.flags & 1)) {
        font_load_race_set(D_800E7728);
    }

    if (gGame_19.mode == 4 || gGame_19.mode == 5) {
        gNumRiders = 6;
    } else if (gGame_19.mode == 9) {
        if (gTutorialLessons_5[gSeason_3->idx].fl & 4) {
            gNumRiders = 2;
        } else {
            gNumRiders = 1;
        }
    } else if ((gGame_19.flags & 0x81) == 0x80) {
        gNumRiders = 1;
    } else if (gGame_19.mode == 0 && gGame_19.f3C == 2) {
        gNumRiders = gGame_19.f75C + 2;
    } else if (gGame_19.mode == 13 || gGame_19.mode == 14) {
        gNumRiders = 1;
    } else if (gGame_19.flags & 1) {
        gNumRiders = 6;
        D_800E3704 = 0;
    } else if (gGame_19.mode == 6) {
        gNumRiders = 4;
        gGame_19.f3C = 1;
    } else if (gGame_19.mode == 2 || gGame_19.mode == 10) {
        gNumRiders = gGame_19.f3C;
    } else {
        gNumRiders = gGame_19.f3C + gGame_19.f44;
    }

    if (gGame_19.mode == 8 && !(gGame_19.flags & 1)) {
        gNumRiders = 1;
    }
    v = 0xFF;
    for (i = 7; i >= 0; i--) {
        gRiderAlpha[i] = v;
    }
    if (gGame_19.flags & 0x10) {
        gNumRiders = 6;
    }
    race_init_replay();
    if (gGame_19.flags & 0x40000) {
        gNumRiders = gGame_19.f3C;
        gGame_19.f3C = 1;
    }
    event_init(gNumRiders, gGame_19.f40);
    if (!(gGame_19.flags & 0x8081)) {
        race_randomize_ai_order(gGame_19.f3C, gEvent_8.h22 - gGame_19.f3C);
        race_set_grid_order(gGame_19.f3C, gEvent_8.h22 - gGame_19.f3C);
    }
    gPhysDeltaTime = 0.016666668f;
    func_800358C0();
    mem_fill(gPlayerInputsPrev_2, 0, 0x18);
    palmod_init((gGame_19.flags >> 18) & 1);
    gTrackSections_5 = 0;
    gTrackObjects = 0;
    gWaterModels = 0;
    gTerrainTexSet = 0;
    gObjectTexSet = 0;
    D_800E77C4 = 0;
    gAnimTexSet = 0;
    gWaterTexSet = 0;
    render_init();
    palmod_register(gTerrainTexSet, 1);
    palmod_register(gObjectTexSet, 2);
    palmod_register(gAnimTexSet, 0);
    palmod_register(gWaterTexSet, 0);
    palmod_register(D_800E77C4, 0);
    if (gPodiumMode != 0 && gCurTrack_14->f2C == 0) {
        palmod_register(sStandTextures, 0);
    }
    ambient_load();
    announce_reset();
    particles_load_textures(D_800E7728);
    race_load_surface_table(D_800E7728, track_res_find(D_800D14F0));
    hacks_init();
    flash_init(D_800E7728);
    path_init(D_800E7728);
    cam_init(D_800E7728);
    shadow_init();
    map_init();
    particles_init(gSurfaceTable);
    rider_anims_load(D_800E7728);
    bikes_init();
    for (i = 0; i < 8; i++) {
        *gRiders[i].riderSkel = *gRiderSkelTemplate;
    }
    snd_race_init(gSurfaceTable);
    race_apply_volumes();
    decal_init();
    rain_init();
    snow_init();
    if (gGame_19.mode == 9) {
        event_stop_sounds_rumble();
    }
    if (gPodiumMode != 0) {
        podium_init();
        race_alloc_text_objs();
    }
    if (gGame_19.flags & 0x80) {
        gRacePhase = 0;
        gRacePhaseTimer = -1;
        race_intro_load();
    } else if (gGame_19.flags & 1) {
        gRacePhase = 5;
    } else {
        gRacePhase = 4;
        gRacePhaseTimer = -1;
    }
    D_800C8CE0 = 0;
    if (gGame_19.mode == 8 && !(gGame_19.flags & 0x80) && gGhostSlot == 0) {
        D_800C8CE0 = 1;
    }
}

void race_exit(void)
{
    u32 *r;
    u32 t;
    u32 f;

    rumble_stop_all();
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0 && (gGameMode_2 - 0xD) < 2) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    gGame = gGame & 0xFFFEFFFF;
    hacks_exit();
    decal_free();
    rider_anims_unload();
    r = (u32 *)&gGame;
    snd_race_exit();
    path_free();
    particles_free();
    flash_free();
    render_free();
    instances_free();
    bikes_free();
    event_exit();
    shadow_free();
    res_close_set(D_800E7728);
    palmod_free();
    map_free();
    ambient_clear();
    rain_free();
    snow_free();
    if (gPodiumMode != 0) {
        podium_exit();
        race_free_text_objs();
    }
    t = gGame & 0xFFFFBFFF;
    f = gGame & 1;
    gGame = t;
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

typedef struct { s32 f00; u8 pad04[0x38]; s32 f3C; u8 pad40[0xC]; s32 f4C; } Blk_7;
typedef struct { u8 pad[0x22]; s16 f22; } Blk2;

extern Blk_7 gGame_20;
extern Blk2 gEvent_9;

void race_begin(void)
{
    while (sched_get_current_task() != 0) { }
    snd_load_engine_samples();
    if (gGhostRace != 0) {
        gNumViews = 1;
    } else {
        gNumViews = gGame_20.f3C;
    }
    gNumScreens = gNumViews;
    if (gGame_20.f4C == 9) {
        path_init(D_800E7728);
    }
    if ((gGame_20.f00 & 0x8085) == 0 && (gGame_20.f4C != 1 || gSeason_2->f64 == 0)) {
        race_randomize_ai_order(gGame_20.f3C, gEvent_9.f22 - gGame_20.f3C);
        race_set_grid_order(gGame_20.f3C, gEvent_9.f22 - gGame_20.f3C);
    }
    gPauseMenu = 0;
    if ((gGame_20.f00 & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && gHasExpansionPak != 0 &&
        (gGame_20.f3C == 1 || gGame_20.f4C == 8) &&
        gCurTrack_4->f2C != 2 && (gGame_20.f00 & 0x04040000) == 0) {
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
    cam_init(D_800E7728);
    bikes_init();
    gCamView = 0;
    if (0 < gNumViews) {
        do {
            cam_update();
            gCamView = gCamView + 1;
        } while (gCamView < gNumViews);
    }
    decal_init();
    rider_anims_load(D_800E7728);
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
    D_800C8CE0 = 0;
    if (gGame_20.f4C == 8 && (gGame_20.f00 & 0x80) == 0 && gGhostSlot == 0) {
        D_800C8CE0 = 1;
    }
    sky_reset_clip();
    if (gNumScreens >= 2) {
        gSchedMinRetraces = 2;
    } else {
        gSchedMinRetraces = 1;
    }
}

typedef struct {
    u32 flags;          /* 0x00 */
    u8 pad04[0x38];
    s32 f3C;            /* 0x3C */
    s32 f40;            /* 0x40 */
    s32 f44;            /* 0x44 */
    u8 pad48[4];
    s32 f4C;            /* 0x4C */
    u8 pad50[0x75C - 0x50];
    s32 f75C;           /* 0x75C */
} Ctl_3;

typedef union {
    u32 w;
    struct {
        u32 x : 4;
        u32 y : 4;
        u32 rest : 24;
    } b;
} Pad_2;

extern Pad_2 gPadInput_5;
extern Ctl_3 gGame_21;

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

    if (gGame_21.f4C == 0 && gGame_21.f3C >= 2) {
        if (up && gGame_21.f40 >= 2) gGame_21.f40--;
        if (down && gGame_21.f40 < 6) gGame_21.f40++;
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
                gCurGameState->f10 = 3;
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
            if (gGame_21.f4C == 9) path_free();
            if (gDrawFlags & 0x800) event_hud_free();
            race_intro_unload();
            gGame_21.flags &= ~0x80;
            gNumRiders = gGame_21.f3C + gGame_21.f44;
            if (gGame_21.f4C == 8 && !(gGame_21.flags & 1)) {
                gNumRiders = 1;
            } else if (gGame_21.f4C == 0 && gGame_21.f3C == 2) {
                gNumRiders = gGame_21.f75C + 2;
            }
            race_init_replay();
            event_init(gNumRiders, gGame_21.f40);
            race_begin();
            if (gGame_21.f4C == 9) event_stop_sounds_rumble();
        }
        break;
    case 1:
        if (b20 && (gGame_21.f4C != 1 || gSeason_2->f64 == 0)) {
            snd_play_menu_sound(2);
            gRacePhase_2 = 2;
            gRacePhaseTimer = 20;
            gRaceExitCode = 3;
            gGame_21.flags &= ~4;
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

    gRaceTextObjs = mem_alloc_dbg(0x148, 0, D_800D6170, 0x52C);
    i = 0;
    do {
        *(s32 *)((u8 *)gRaceTextObjs + (i << 2)) = (s32)font_text_create((s32 *)0, 0x40, 1, 2, 0x20);
        i = i + 1;
    } while (i < 0x4a);
    r = res_load(D_800E7728, D_800D088C);
    gRaceTextObjs[0x4a] = r;
    r = res_load(D_800E7728, D_800D09B0);
    gRaceTextObjs[0x50] = r;
    if (gGameMode == 1) {
        i = 0;
        do {
            r = res_load(D_800E7728, *(s32 *)D_800C8D68[*(s16 *)((u8 *)*(s16 **)gSeason + (i << 1) + 8)]);
            *(s32 *)((u8 *)gRaceTextObjs + (i << 2) + 0x12C) = r;
            i = i + 1;
        } while (i < 5);
    }
    r = res_load(D_800E7728, D_800D085C);
    gRaceTextObjs[0x51] = r;
}

typedef struct {
    s32 unk0;
    s32 *unk4;
} Unk7728;

typedef struct {
    u8 pad[8];
    s16 a[5];
} Row_2;

extern Unk7728 *D_800E7728_3;
extern Row_2 **gSeason_7;

void race_intro_unload(void) {
    s32 i;
    s32 off;

    for (i = 0; i < 0x4A; i++) {
        off = i * 4;
        func_800C204C(*(void **)((s32)gRaceTextObjs + off));
        mem_free(*(void **)((s32)gRaceTextObjs + off));
    }
    res_unload(&D_800E7728_3->unk4[D_800D088C]);
    res_unload(&D_800E7728_3->unk4[D_800D09B0]);
    res_unload(&D_800E7728_3->unk4[D_800D085C]);
    if (gGameMode == 1) {
        for (i = 0; i < 5; i++) {
            res_unload(&D_800E7728_3->unk4[*D_800C8D68_2[(*gSeason_7)->a[i]]]);
        }
    }
    mem_free(gRaceTextObjs);
    gRaceTextObjs = NULL;
    if (gRaceExitCode != 3) {
        gGame &= ~0x80;
    }
    font_load_race_set(D_800E7728_3);
}

#define PAD (gGame_22.f2C8[gPausePlayer])
#define PRESSED(m) ((gPadInput_2[PAD] & (m)) && !(gPadInputPrev_2[PAD] & (m)))

typedef struct {
    u8 pad0[0x17C];
    s32 f17C;
    u8 pad180[0x2C8 - 0x180];
    s8 f2C8[4];
} Ctl_4;

extern Ctl_4 gGame_22;

void race_pause_update_options(void) {
    s32 s0;
    s32 old;
    s32 b;

    if (sOptionsPreviewDelay != 0) {
        sOptionsPreviewDelay--;
    }
    if (++gGame_22.f17C == 60) {
        gGame_22.f17C = 0;
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
        D_800E3730 = 3;
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
            sfx_play(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
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
            sfx_play(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
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
                sfx_play(D_800C2C4C, 0x2B11, D_800F1BC6, 7, 1, 1);
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
        snd_set_volume(0, D_800F1BC6);
        break;
    case 0:
        snd_set_volume(1, D_800F1BC5_2);
        break;
    case 2:
        snd_set_volume(2, D_800F1BC7);
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
    u8 *pc;

    if (((gPadInput_3[(s8)gGame_2[0x2C8 + gPausePlayer]] & 0x20) != 0) &&
        ((gPadInputPrev_3[(s8)gGame_2[0x2C8 + gPausePlayer]] & 0x20) == 0)) {
        gPauseMenu = 1;
        D_800E3730 = 3;
        save_write(5, 0);
    } else {
        u32 *t30 = gPadInput_3;
        cur = (s8)gGame_2[0x2C8 + gPausePlayer];
        if (((t30[cur] & 0x10) != 0) && ((gPadInputPrev_3[cur] & 0x10) == 0)) {
            pc = gGame_2 + cur;
            n = pc[0x50] + 1;
            pc[0x50] = n;
            if (3 < (s8)n) {
                u8 *pd = gGame_2 + (s8)gGame_2[0x2C8 + gPausePlayer];
                pd[0x50] = pd[0x50] - 4;
            }
            snd_play_menu_sound(0);
        }
    }
}

#define PAD_2 (gGame_23.f2C8[gPausePlayer])
#define PRESSED_2(m) ((gPadInput_3[PAD_2] & (m)) && !(gPadInputPrev_3[PAD_2] & (m)))

typedef struct { u32 w[6]; } Pad24;

typedef struct {
    u32 f0;             /* 0x000 */
    u8 pad04[0x38];
    s32 f3C;            /* 0x03C */
    s32 f40;            /* 0x040 */
    s32 f44;            /* 0x044 */
    u8 pad48[4];
    s32 f4C;            /* 0x04C */
    u8 pad50[4];
    s32 f54;            /* 0x054 */
    u8 pad58[0x1C];
    s32 f74[0x30];      /* 0x074 */
    u8 pad134[0x2C8 - 0x134];
    s8 f2C8[4];         /* 0x2C8 */
    s8 f2CC[4];         /* 0x2CC */
    u8 pad2D0[0x75C - 0x2D0];
    s32 f75C;           /* 0x75C */
} Game_10;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x28];
    u16 f58[4];
    u16 f60[4];
} Cfg64;

typedef struct {
    s32 f0;             /* 0x00 */
    u8 pad04[0x1E];
    s16 h22;            /* 0x22 */
    u8 pad24[2];
    s16 h26;            /* 0x26 */
    u8 pad28[0x18];
    s32 f40;            /* 0x40 */
    u8 pad44[0x14];
    u32 f58;            /* 0x58 */
    u8 pad5C[0x14];
    s32 f70;            /* 0x70 */
    u8 pad74[4];
    s32 f78;            /* 0x78 */
    u8 pad7C[4];
    s32 f80;            /* 0x80 */
} Sess;

typedef struct {
    u8 pad0[0x2C4];
    u8 b2C4;
    u8 b2C5;
    u8 b2C6;
    u8 b2C7;
} Opt_2;

typedef struct {
    s16 h0;
    u8 b2;
    u8 b3;
    s32 w4;
    s32 w8;
    s32 wC;
    s32 w10;
    s32 w14;
} Gui;

typedef struct {
    u8 pad0[4];
    s32 *f4;
} Res_2;

typedef struct {
    u8 pad0[0x64];
    s16 h64;
} S930;

typedef struct {
    u8 pad0[0xC];
    u8 bC;
    u8 padD[0x28 - 0xD];
} Trk_6;

typedef struct {
    s32 *a;
    s32 *b;
    s32 *w;
    s32 *h;
    s32 *c;
    void *d;
} Cfg_5;

extern Game_10 gGame_23;
extern Cfg64 *gCurTrack_15;
extern Pad24 gPlayerInputs_3;
extern Pad24 gPlayerInputsPrev_4;
extern Pad24 D_800E3750;
extern Pad24 D_800E3718;
extern Sess gEvent_10;
extern Gui D_800E3730_6;
extern Res_2 *D_800E7728_4;
extern Opt_2 gSaveData_3;
extern S930 *gSeason_8;
extern Trk_6 gTutorialLessons_6[];

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
    for (j = 0; j < gGame_23.f3C; j++) {
        buf.w[j] = arg[gGame_23.f2C8[j]];
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
        if (gRiders[i].rearWheel.surface != NULL && gRiders[i].rearWheel.surface->unk_38 != 0 &&
            gRiders[i].speed > 8.0f && gFrameCounter % 6 == 0) j = 1;
        if (gPauseMenu != 0) j = 0;
        if (gRiders[i].isCpu != 0) j = 0;
        if (gGhostRace != 0 && i == gGhostSlot) j = 0;
        if (gReplayPlayback != 0) j = 0;
        if (gGame_23.f54 != 3) j = 0;
        if (gGame_23.f0 & 1) j = 0;
        if (gGame_23.f4C == 9 && gEvent_10.f0 == 0xE) j = 0;
        t = gGame_23.f2C8[i];
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
    cfg.c = &D_800DC0B4;
    cfg.d = &gSfxVolume;
    if (gPauseMenu == 2 || gPauseMenu == 3) {
        if (D_800E3730_6.h0 == 0 && D_800E3730_6.b3 == 0) {
            mem_free(gZBuffer);
            D_800E3730_6.b3 = 1;
            if (gPauseMenu == 2) {
                font_load_menu_set(D_800E7728_4);
            }
            res_open_set("guires", &gGuiRes);
            for (j = 0; j < 0x30; j++) {
                gGame_23.f74[j] = font_text_create(0, 0x40, 4, 2, 0x20);
            }
            if (gPauseMenu == 2) {
                D_800E3730_6.w4 = res_load((s32)D_800E7728_4, D_800D0C24);
            } else {
                D_800E3730_6.w8 = res_load((s32)D_800E7728_4, D_800D108C);
                D_800E3730_6.w4 = res_load(gGuiRes, D_800D1488);
                D_800E3730_6.w10 = res_load(gGuiRes, D_800D1428);
                D_800E3730_6.w14 = res_load(gGuiRes, D_800D1424);
                D_800E3730_6.wC = res_load(gGuiRes, D_800D1478);
            }
            D_800E3730_6.h0 = -1;
            while (sched_get_current_task() != 0) {
            }
            gViModeRequest = 1;
            while (gViMode != gViModeRequest) {
            }
            render_update_screen_layout();
        }
    } else if (gPauseMenu == 1 && D_800E3730_6.h0 == 0 && D_800E3730_6.b3 != 0) {
        D_800E3730_6.b3 = 0;
        D_800E3730_6.b2 = 2;
        for (j = 0; j < 0x30; j++) {
            func_800C204C(gGame_23.f74[j]);
            mem_free(gGame_23.f74[j]);
        }
        res_close_set(gGuiRes);
        res_unload(&D_800E7728_4->f4[D_800D0C24]);
        res_unload(&D_800E7728_4->f4[D_800D1080]);
        res_unload(&D_800E7728_4->f4[D_800D0724]);
        res_unload(&D_800E7728_4->f4[D_800D108C]);
        font_unload_set();
        gZBuffer = mem_alloc_at_dbg(gZBuffer, gFbAllocWidth * gFbAllocHeight * 2, "min.c", 0x799);
        if (gZBuffer == 0) {
            mem_report_2();
            while (1) {
            }
        }
        if (!(gGame_23.f0 & 0x81) && (gSaveData_3.b2C4 & 2) && gHasExpansionPak != 0 &&
            (gGame_23.f3C == 1 || gGame_23.f4C == 8) && gCurTrack_15->f2C != 2 &&
            !(gGame_23.f0 & 0x04040000)) {
            gViModeRequest = 2;
        } else {
            gViModeRequest = 1;
        }
        while (gViMode != gViModeRequest) {
        }
        render_update_screen_layout();
        font_load_race_set(D_800E7728_4);
        D_800E3730_6.h0 = -1;
    }
    if (gGame_23.f4C != 9 && !(gGame_23.f0 & 0x8010) && (gPadInput_3[0] & 0x400) &&
        !(gPadInputPrev_3[0] & 0x400)) {
        gHudLevel = (gHudLevel + 1) & 3;
    }
    particles_emit_bike_dirt();
    tube_animate_textures();
    if (!(gGame_23.f0 & 0x80)) {
        if (gReplayPlayback != 0 && gRacePhase_2 == 5 &&
            ((gPlayerInputs_3.w[0] | gPlayerInputs_3.w[1] | gPlayerInputs_3.w[2] | gPlayerInputs_3.w[3]) & 0x50)) {
            gRacePhase_2 = 6;
            gRacePhaseTimer = -1;
            gRaceExitCode = 2;
            gGame_23.f0 &= ~4;
            snd_play_menu_sound(1);
            return;
        }
    } else {
        race_update_intro();
    }

    if (gPauseMenu == 2) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
            race_pause_update_controller();
        }
    } else if (gPauseMenu == 3) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
            race_pause_update_options();
        }
    } else if (gPauseMenu == 6) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
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
                    gEvent_10.f58 |= 0x80;
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
            D_800E3730_6.b2 = 2;
        }
    } else if (gPauseMenu == 5) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
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
                    gEvent_10.f70 = 3;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 3:
                    gEvent_10.f70 = 4;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 1:
                    gEvent_10.f70 = 7;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 0:
                    gEvent_10.f70 = 6;
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    break;
                case 2:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gRaceExitCode = 2;
                    gGame_23.f0 &= ~4;
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
            D_800E3730_6.b2 = 2;
        }
    } else if (gPauseMenu == 1) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
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
                    gOptionsCursor = gGame_23.f2CC[PAD_2];
                    D_800E3730_6.h0 = 3;
                    break;
                case 1:
                    gPauseMenu = 3;
                    D_800E3730_6.h0 = 3;
                    gOptionsCursor = 0;
                    break;
                case 4:
                    gRacePhase_2 = 6;
                    gRacePhaseTimer = -1;
                    gGame_23.f0 |= 4;
                    if (gGame_23.f4C == 1) {
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
                gPauseMenuCursor = (gPauseMenuCursor + (gGame_23.f4C == 1 ? 3 : 4)) % (gGame_23.f4C == 1 ? 4 : 5);
                snd_play_menu_sound(0);
            } else if (PRESSED_2(2) || PRESSED_2(0x1000)) {
                gPauseMenuCursor = (gPauseMenuCursor + 1) % (gGame_23.f4C == 1 ? 4 : 5);
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
            D_800E3730_6.b2 = 2;
        }
    } else if (gPauseMenu == 4) {
        if (gRacePhase_2 == 5 && D_800E3730_6.h0 == -1) {
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
                    gGame_23.f0 &= ~4;
                    snd_play_menu_sound(1);
                    if (gGame_23.f4C == 1) {
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
            D_800E3730_6.b2 = 2;
        }
    } else {
        if (gEvent_10.f58 & 0x800) {
            event_update(0);
            snd_set_volume(0, 0);
            if (gEvent_10.f80 != 0) {
                if ((gPlayerInputs_3.w[0] & 0x10) && !(gPlayerInputsPrev_4.w[0] & 0x10)) {
                    snd_play_menu_sound(1);
                    gEvent_10.f80 = 1;
                }
                if (--gEvent_10.f80 == 0) {
                    gEvent_10.f58 &= 0xFFFE77FF;
                    snd_set_volume(0, gSaveData_3.b2C6);
                }
            }
            if (gEvent_10.f58 & 0x10000) {
                gEvent_10.f58 &= ~0x10000;
                gCamView = 0;
                cam_update();
            }
        } else {
            D_800E3750 = gPlayerInputs_3;
            race_update_replay();
            if (gReplayPlayback != 0 && gCamTargetOverride != -1 && !(gGame_23.f0 & 0x80)) {
                for (j = 0; j < 4; j++) {
                    if ((gPadInput_3[j] & 0x400) && !(gPadInputPrev_3[j] & 0x400)) break;
                }
                if (j != 4) {
                    for (j = 1; j <= gEvent_10.h22; j++) {
                        t = (gCamTargetOverride + j) % gEvent_10.h22;
                        if (t < gGame_23.f3C) break;
                    }
                    gCamTargetOverride = t;
                }
            }
            if (gPodiumMode == 0) {
                bikes_update(1);
            } else {
                podium_update();
            }
            if (gGame_23.f0 & 0x10) {
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
                func_8003F128();
                rider_update_fist_pump();
            }
            flash_update();
            snd_update_riders();
            if (D_800EE140 == 0) {
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

    if (gReplayPlayback == 0 && gEvent_10.f40 == 0 && gPauseMenu == 0 && !(gGame_23.f0 & 0x81)) {
        for (j = 0; j < gEvent_10.h22; j++) {
            if (((gPlayerInputs_3.w[j] & 0x40) && !(gPlayerInputsPrev_4.w[j] & 0x40)) ||
                (gGame_23.f4C == 9 && (gPadInput_3[j] & 0x40) && !(gPadInputPrev_3[j] & 0x40))) {
                if (flag == 0 && gRacePhase_2 == 5) {
                    gPlayerInputsPrev_4.w[j] |= 0x40;
                    if (gPodiumMode != 0) {
                        gRacePhase_2 = 6;
                        gRacePhaseTimer = -1;
                        gRaceExitCode = 4;
                        gGame_23.f0 &= ~4;
                    } else if (gGame_23.f4C == 13 || gGame_23.f4C == 14) {
                        gPauseMenu = 6;
                        gPauseMenuCursor = 0;
                    } else if (gGame_23.f4C == 9) {
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
            if ((gGame_23.f0 & 4) || (gGame_23.f4C == 9 && gRaceExitCode == 6)) {
                gRacePhase_2 = 7;
                gRacePhaseTimer = 2;
            } else {
                gRacePhase_2 = 7;
                gRacePhaseTimer = 100;
                gCurGameState->f10 = 3;
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
            if (gGame_23.f4C == 9) path_free();
            if (gDrawFlags & 0x800) event_hud_free();
            if (gGame_23.f4C == 9) {
                switch (gEvent_10.f70) {
                case 3:
                    gSeason_8->h64--;
                    break;
                case 4:
                    gSeason_8->h64++;
                    break;
                case 6:
                    gEvent_10.f58 |= 0x4000;
                    break;
                case 7:
                    break;
                default:
                    gEvent_10.f0 = 0xE;
                    gEvent_10.h26 = 0x3F;
                    gEvent_10.f78 = 0x3C;
                    gEvent_10.f70 = 0;
                    gEvent_10.f58 = (gEvent_10.f58 & ~0x10) | 0x40;
                    break;
                }
                gPauseMenu = 0;
                gEvent_10.f58 &= ~0x2000;
                if (gTutorialLessons_6[gSeason_8->h64].bC & 4) {
                    gNumRiders = 2;
                } else {
                    gNumRiders = 1;
                }
            } else if (gGame_23.f4C == 6) {
                gNumRiders = 4;
                gGame_23.f3C = 1;
            } else if (gGame_23.f4C == 10 || gGame_23.f4C == 2) {
                gNumRiders = gGame_23.f3C;
            } else if (gGame_23.f4C == 0 && gGame_23.f3C == 2) {
                gNumRiders = gGame_23.f75C + 2;
            } else {
                gNumRiders = gGame_23.f3C + gGame_23.f44;
                if (gGame_23.f4C == 8 && !(gGame_23.f0 & 1)) {
                    gNumRiders = 1;
                }
            }
            race_init_replay();
            event_init(gNumRiders, gGame_23.f40);
            race_begin();
            if (gGame_23.f4C == 9) event_stop_sounds_rumble();
        }
        break;
    }
    D_800E3718 = D_800E3750;
    gPlayerInputsPrev_4 = gPlayerInputs_3;
    if (D_800DC0B4 <= 0) {
        D_800DC0B4 = 1;
    }
    if (D_800DC0B4 >= 9) {
        D_800DC0B4 = 8;
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

    tab = res_load(gSfxResSet_3, D_800D123C);
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
        gSurfaceTable[i].unk_38 = tab[j].val;
    }
    res_unload(&r->arr[idx]);
    res_unload(&gSfxResSet_3->arr[D_800D123C]);
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

    if (x == sFrontendLoaded || (gGame & 1)) {
        return;
    }
    p3 = D_800731D0;
    p6 = D_800A42C0;
    p7 = D_800A9C20;
    if ((u32)p3 & 0xF) fatal_halt(0x33);
    if ((u32)p6 & 0xF) fatal_halt(0x34);
    if ((u32)p7 & 0xF) fatal_halt(0x35);
    if (x != 0) {
        sz = (u32)&D_800A42C0[-0x30] - (u32)p3;
        mem_alloc_at_dbg(p3 + 0x30, sz, D_800D6170, 0xBDD);
        if (!(gCurTrack_16->f50 & 2)) {
            mem_alloc_at_dbg(p6 + 0x10, (u32)&D_800A9C20[-0x10] - (u32)p6, D_800D6170, 0xBE0);
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
            D_80100658 = p6;
            gSwdSrc = gZBuffer;
            D_80100654 = gZBuffer + gBootHeader_2.fC;
            buf = *(Blk48 *)p3;
            swd_decompress();
            *(Blk48 *)p3 = buf;
            cart += gBootHeader_2.f14;
            while ((h = dma_start(gCartRomHandle, 0, cart, gZBuffer, gBootHeader_2.f1C)) < 0);
            while (dma_get_status(h) != 3);
            dma_release(h);
            gSwdDst = p6;
            D_80100658 = p7;
            __builtin_memcpy(&buf, p6, 16);
            gSwdSrc = gZBuffer;
            D_80100654 = gZBuffer + gBootHeader_2.fC;
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

typedef struct {
    s32 flags;
    s32 pad[14];
    s32 f3C;
    s32 f40;
    s32 f44;
} Ctl_5;

void race_init_ghost_replay(s32 p)
{
    if (p != 0) {
        Ctl_5 *c;
        gGhostSlot ^= 1;
        c = &gGame;
        c->f3C = 1;
        c->f44 = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gNumRiders = 2;
        c->f3C = 2;
        gGhostSlot ^= 1;
    } else {
        Ctl_5 *c = &gGame;
        c->f3C = 1;
        c->f44 = 0;
        gNumRiders = 1;
        gGhostSlot = 0;
        replay_record_start(gActiveReplay, gActiveReplaySize);
        gGhostSlot = 1;
    }
    if (gGhostRace != 0) {
        Ctl_5 *c = &gGame;
        if (c->flags & 0x80000) {
            c->f3C = 1;
            replay_play_start_keep_setup(gActiveReplay + 0x6400);
            c->f3C = 2;
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
    if (gGameMode == 9) {
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

void func_80019EC0(s32 param_1)
{
    u8 *b;
    u8 *gb;
    s32 s2;
    s32 s3;
    u8 s4;
    u8 s5;
    s32 idx;

    if (param_1 != 0) {
        gb = (u8 *)0x800E0000;
        __asm__ __volatile__("" : "+r"(gb));
        b = gGame_2;
        s2 = *(s32 *)(b + 0x1c);
        __asm__ __volatile__("" : "+r"(s2) : : "memory");
        *(s32 *)(gb + 0x3700) = -1;
        s3 = *(s8 *)(b + 0x2c8);
        s4 = *(u8 *)(b + 0x43c);
        s5 = *(u8 *)(b + 0x7aa);
        replay_play_start(param_1);
        idx = *(s32 *)(gb + 0x3700) ^ 1;
        *(s32 *)(b + 0x1c + idx * 4) = s2;
        *(s8 *)(b + 0x2c8 + idx) = s3;
        *(u8 *)(b + 0x43c + (*(s32 *)(gb + 0x3700) ^ 1)) = s4;
        *(u8 *)(b + 0x7aa + (*(s32 *)(gb + 0x3700) ^ 1)) = s5;
        *(u8 *)(b + 0x2c8 + *(s32 *)(gb + 0x3700)) = 0;
    }
}

typedef struct {
    u32 flags;
    u8 pad0[0x3C - 4];
    s32 f3c;
    u8 pad1[0x4C - 0x40];
    s32 f4c;
} Blk70;

extern Blk70 gGame_24;

void race_set_video_mode(void)
{
    if ((gGame_24.flags & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && gHasExpansionPak != 0 &&
        (gGame_24.f3c == 1 || gGame_24.f4c == 8) &&
        gCurTrack_3[11] != 2 && (gGame_24.flags & 0x4040000) == 0) {
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
    s32 *p;
    s32 off;

    gRaceTextObjs = mem_alloc_dbg(0x148, 0, "min.c", 0x4E3);
    i = 0;
    do {
        p = font_text_create(0, 0x40, 1, 2, 0x20);
        off = i * 4;
        *(s32 *)((u8 *)gRaceTextObjs + off) = (s32)p;
        i++;
    } while (i < 0x4A);
}

void race_free_text_objs(void)
{
    s32 i;
    void **p;

    i = 0;
    do {
        p = (void **)gRaceTextObjs + i;
        func_800C204C(*p);
        p = (void **)gRaceTextObjs + i;
        mem_free(*p);
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
