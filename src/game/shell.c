/* shell.c: the front end. Screens (sScreens) are shown one at a time with a fade between them; each has a type
   that picks its input handler (shell_update), what is loaded for it (shell_load_screen) and how it is drawn
   (shell_draw_screen). Choosing an item runs its callback, which sets up the game mode, and goes to its target
   screen; a NULL target leaves the front end for the race (or the editor, or the NES game). */
#include "game.h"

/* The alpha of gFontColors[28] (TEXT_COLOR_WHITE), which the records screen and shell_draw fade. */
#define FONT_FADE_ALPHA (*(u8 *)((u32)gFontColors + 28 * 4 + 3))

/* The Controller Pak screen's describe callback (gPakScreen.describe): the label of one of the game's notes from
   its header, "Ghost: <track> <time>" or "Track: <name>". compact: the ghost's label without the time. Returns a
   PAK_NOTE_*: the notes of the kind the screen does not load are hidden. */
s32 shell_format_pak_file_label(u8 *header, char *dst, s32 compact)
{
    s32 track, rider, time, ret;
    s32 n, m, v;

    dst[0] = 0;
    if (replay_get_info(header, (u32 *)&track, (u32 *)&rider, (u32 *)&time) == 0) {
        /* a ghost */
        time = time - 0x102;
        __builtin_memcpy(dst, sStrGhostLabel, 8);
        str_cat(dst, gTracks_13 + track * 160);
        if (compact == 0) {
            str_cat(dst, sStrKeySpace);
            v = time / 3600;
            if (v < 10) {
                str_cat(dst, sStrKeyZero);
            }
            int_to_str(v, dst + strlen(dst));
            str_cat(dst, sStrGhostTimeColon);
            n = strlen(dst);
            v = time / 60 % 60;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = '.';
            v = time % 60 * 100 / 60;
            n++;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = 0;
        }
        ret = gGame.pakScreenMode;
        ret = ret ^ PAK_SCREEN_LOAD_USER_TRACK;
    } else {
        /* a user track: the checksum, then the packed track (usrtrk_pack_track) */
        if (header[4] != 0x35) {
            __builtin_memcpy(dst, sStrUnknownFile, 28);
            return PAK_NOTE_FOREIGN;
        }
        __builtin_memcpy(dst, sStrTrackLabel, 8);
        m = strlen(dst);
        dst[m + 10] = 0;
        m = strlen(dst);
        __builtin_memcpy(dst + m, header + 6, 10);
        ret = gGame.pakScreenMode;
        ret = ret ^ PAK_SCREEN_LOAD_GHOST;
    }
    return (ret == 0) << 1;
}

/* Fills gGame.greyPalette: pal (a CI8 palette) blended towards a grey ramp by alpha (0-255; 255: the ramp). */
void shell_blend_palette_to_gray(s32 alpha, u16 *pal)
{
    s32 i;
    s32 r, g, b;
    u32 color;

    gGame.greyPalette[0] = 0;
    for (i = 1; i < 0x100; i++) {
        if (alpha == 0xFF) {
            b = i;
            g = i;
            r = i;
        } else {
            color = pal[i];
            b = (color >> 11) * 8;
            g = (color >> 3) & 0xF8;
            r = (color << 2) & 0xF8;
            b = b + ((i - b) * alpha) / 255;
            g = g + ((i - g) * alpha) / 255;
            r = r + ((i - r) * alpha) / 255;
        }
        gGame.greyPalette[i] = ((r >> 3) << 1) | ((g >> 3) << 6) | ((b >> 3) << 11) | 1;
    }
}

typedef struct {
    u8 pad0[0x34];
    TrackRecord record;
    s32 flags;          /* TRACK_FLAG_* */
    u8 pad54[0xA0 - 0x54];
} Course_2;

extern Course_2 gTracks_14[];

/* The front end's init (game state 0). After power-on it clears gGame and starts with the boot splashes; after a
   race it picks the screen to come back to from gRaceExitCode (2 race over, 3 left during the intro, 4 quit from
   the pause menu, 6 left the front end or the editor, 7 podium over) and the game mode. */
void shell_init(void)
{
    s32 toTrackLists;
    s32 exitCode;
    s32 i;
    TrackRecord *dst;
    s32 n;
    Screen *next;
    u8 *bikeSkin;
    u8 *riderSkin;

    toTrackLists = 0;
    gPodiumMode = 0;
    gReplayPlayback = 0;
    if (gRaceExitCode == 2) {
        toTrackLists = toTrackLists < (u32)(gGame.flags & GAME_FLAG_TRACK_LISTS);
    }
    gSplitVertical = 0;
    gGame.flags &= ~GAME_FLAG_TRACK_LISTS;
    if (gRaceExitCode == 0) {
        /* power-on */
        mem_fill((u8 *)&gGame, 0, sizeof(Game));
        n = 0;
        for (i = 0; i < TRACK_COUNT; i++) {
            s32 flags = gTracks_14[i].flags;
            TrackRecord *src = &gTracks_14[i].record;
            if (flags & TRACK_FLAG_RECORDS) {
                dst = &gGame.records[n];
                *dst = *src;
                n++;
            }
        }
        gGame.twoPlayerCpuRiders = 2;
        gGame.riders[0] = 4;
        gGame.mainMenuSel = 0;
        gGame.trackSel = 0;
        gGame.cupSel = 0;
        gGame.riders[1] = 0;
        gGame.riders[2] = 1;
        gGame.riders[4] = 3;
        gGame.riders[5] = 5;
        gGame.riders[3] = 2;
        gGame.numPlayers = 1;
        gGame.numLaps = -1;
        gGame.optionsSel = 0;
        gGame.sponsorLogo = 0;
        gGame.specialTracksSel = 0;
        gGame.unk_018 = 0;
        gGame.padConfig[0] = 0;
        gGame.padConfig[1] = 0;
        gGame.padConfig[2] = 0;
        gGame.padConfig[3] = 0;
        gGame.unk_2A4 = 0;
        gGame.unk_2A5 = 1;
        gGame.riderSkinSel = 0;
        gGame.padCamMode[0] = 2;
        gGame.padCamMode[1] = 2;
        gGame.padCamMode[2] = 2;
        gGame.padCamMode[3] = 2;
        /* bikeSkins[i] = i and riderSkins[i] = i, through byte pointers */
        i = 5;
        bikeSkin = (u8 *)&gGame + i;
        do {
            bikeSkin[0x7AA] = i;
            bikeSkin--;
            i--;
        } while (i >= 0);
        i = 5;
        riderSkin = (u8 *)&gGame + i;
        do {
            riderSkin[0x43C] = i;
            riderSkin--;
            i--;
        } while (i >= 0);
    } else if (gGame.mode == GAME_MODE_TIME_TRIAL) {
        gGame.numPlayers = 1;
    }
    if (gRaceExitCode == 7) {
        gGame.flags &= ~GAME_FLAG_UNLOCK_MESSAGES;
    }
    gGame.fadeState = 3;
    res_set_load_buffer((s32)gZBuffer_3);
    i = res_open_set("resource", &gResourceSet);
    res_set_load_buffer(0);
    if (i != 0) {
        fatal_halt(1);
    }
    font_load_menu_set((u32 *)gResourceSet);
    res_close_set((s32 *)gResourceSet);
    /* the tracks of the records screen */
    gGame.numRecordTracks = 0;
    for (i = 0; i < TRACK_COUNT; i++) {
        if ((gTracks_14[i].flags & TRACK_FLAG_RECORDS) && !(gTracks_14[i].flags & TRACK_FLAG_USER)) {
            gZBuffer_3[gGame.numRecordTracks++] = i;
        }
    }
    gGame.recordTracks = mem_alloc_dbg(gGame.numRecordTracks * 2, 0, "shell.c", 0x75C);
    memcpy(gGame.recordTracks, gZBuffer_3, gGame.numRecordTracks * 2);
    gGame.numCpuRiders = 5;
    gRaceSeries_4 = 0;
    gGame.flags = (gGame.flags & (GAME_FLAG_DEBUG | GAME_FLAG_UNLOCK_MESSAGES | GAME_FLAG_GHOST_LOADED
                                  | GAME_FLAG_REPLAY)) | GAME_FLAG_SHELL_RACE;
    res_open_set("guires", &gGuiRes);
    res_register_type("ARN", arn_relocate);
    prim_init();
    sCurScreen = 0;
    sPrevScreen = 0;
    gGame.popupFrames = res_load(gGuiRes, sPopupFramesResId);
    /* the viewport: scale and translation of x, y, z */
    sShellViewport[2] = 0x1FF;
    sShellViewport[6] = 0x1FF;
    sShellViewport[0] = gScreenWidth * 2;
    sShellViewport[1] = gScreenHeight * 2;
    sShellViewport[4] = gScreenWidth * 2;
    sShellViewport[5] = gScreenHeight * 2;
    gGame.greyPalette = mem_alloc_dbg(0x200, 0, "shell.c", 0x777);
    shell_blend_palette_to_gray(0xFF, 0);
    if (gRaceExitCode == 4) {
        gCurTrack_8->w50 &= ~TRACK_FLAG_NEW_RECORD;
        if (gActiveReplay != 0) {
            gActiveReplay = 0;
            gGame.flags &= ~GAME_FLAG_GHOST_LOADED;
        }
    }
    if (gEditor != NULL) {
        /* the editor came here to load or save the user track on a Controller Pak */
        if (gEditor->flags & EDIT_PAK_LOAD) {
            gGame.pakScreenMode = PAK_SCREEN_LOAD_USER_TRACK;
            shell_goto_screen(&sScreens[SCREEN_PAK], 0, 1);
        } else {
            gGame.pakScreenMode = PAK_SCREEN_SAVE_USER_TRACK;
            shell_goto_screen(&sScreens[SCREEN_PAK], 0, 1);
        }
    } else {
        exitCode = gRaceExitCode;
        if (exitCode == 0) {
            shell_goto_screen(&sScreens[SCREEN_BOOT_SPLASH], 0, 1);
        } else if ((gGame.flags & GAME_FLAG_REPLAY) || gCreditsMode != 0 || exitCode == 3) {
            /* back from a replay, the credits or the intro: the screen left */
            if (gActiveReplay != 0) {
                if (!(gGame.flags & GAME_FLAG_REPLAY)) {
                    if (!(gGame.flags & GAME_FLAG_GHOST_LOADED)) {
                        gActiveReplay = 0;
                    }
                }
            }
            if (gRaceExitCode == 3 && gGame.mode == GAME_MODE_SEASON) {
                shell_restore_race_results();
                if (gSeason->round > 0) {
                    gSeason->round--;
                }
            }
            if (gGame.flags & GAME_FLAG_REPLAY) {
                shell_restore_race_results();
            }
            if (gCreditsMode != 0 && gGame.returnScreen != &sScreens[SCREEN_OPTIONS]) {
                gCreditsMode = 0;
                shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
            } else {
                shell_goto_screen(gGame.returnScreen, 0, 1);
            }
            gGame.flags &= ~(GAME_FLAG_CREDITS_FROM_OPTIONS | GAME_FLAG_REPLAY);
        } else if (toTrackLists) {
            shell_goto_screen(&sScreens[SCREEN_BRONZE_TRACKS], 1, 1);
        } else {
            /* the results of a finished race, else the title or the main menu */
            if (gEvent.allFinished != 0 && exitCode != 0 && exitCode != 6 && gGame.mode == GAME_MODE_SEASON) {
                if (gSeason != NULL && gSeason->cup - gCups == CUP_CHALLENGE) {
                    next = &sScreens[SCREEN_SEASON_STANDINGS];
                } else {
                    next = &sScreens[SCREEN_SEASON_RACE_RESULT];
                }
            } else if ((gEvent.allFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGame.mode == GAME_MODE_EXHIBITION)
                       || (gEvent.allFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGame.mode == GAME_MODE_TIME_TRIAL)) {
                next = &sScreens[SCREEN_RACE_RESULT];
            } else {
                if (gActiveReplay != 0) {
                    gActiveReplay = 0;
                }
                switch (gRaceExitCode) {
                case 0:
                    next = &sScreens[SCREEN_TITLE];
                    break;
                case 1:
                    next = &sScreens[SCREEN_RACE_RESULT];
                    break;
                default:
                    next = &sScreens[SCREEN_MAIN_MENU];
                    if (gUserTrackLayout != 0) {
                        mem_free(gUserTrackLayout);
                        gUserTrackLayout = 0;
                    }
                    break;
                }
            }
            /* a new record: its initials first */
            if (gRaceExitCode != 0 && gCurTrack_8 != NULL && (gCurTrack_8->w50 & TRACK_FLAG_NEW_RECORD)) {
                sScreens[SCREEN_INITIALS].items->target = next;
                shell_goto_screen(&sScreens[SCREEN_INITIALS], 1, 1);
            } else {
                shell_goto_screen(next, 1, 1);
            }
        }
    }
    if (gRaceExitCode == 0) {
        save_load(0);
    }
    save_update_unlocks();
    race_apply_volumes();
}

/* Item callback of Soccer: the riders' teams, all riding rider 4, and the controllers of the players. */
void shell_cb_soccer(s32 chosen)
{
    s32 i;
    s32 next;
    s32 player;
    Game *game;

    if (chosen != 0) {
        i = 0;
        do {
            next = i + 1;
            gGame.riderSkins[i] = (u8)(((s32)(i & 3) >> ((gGame.numPlayers - 1) >> 1)) << 1);
            i = next;
        } while (next < 6);
        i = 0;
        player = i;
        game = &gGame;
        game->mode = GAME_MODE_SOCCER;
        gCurTrack = &gTracks[TRACK_SOCCER];
        gSplitVertical = 0;
        game->riders[0] = 4;
        game->riders[1] = 4;
        game->riders[2] = 4;
        game->riders[3] = 4;
        gGame.flags = gGame.flags & ~GAME_FLAG_TRACK_CHOSEN;
        for (; i < MAXCONTROLLERS; i++) {
            if (((s32)gContMask >> i & 1) != 0) {
                gGame.playerPad[player] = (u8)i;
                player = player + 1;
                if (player == gGame.numPlayers) {
                    return;
                }
            }
        }
    }
}

typedef struct {
    u8 unk0[0x2C1];
    u8 tutorialProgress;
} Unk1900;

extern Season sSeasonState;
extern Unk1900 gSaveData_8;

/* Item callback of Season: starts the season of the chosen cup; at difficulty 0 the tutorial instead, at the
   first lesson not done. */
void shell_cb_season(s32 chosen) {
    s32 i;

    if (chosen != 0) {
        gGame.mode = GAME_MODE_SEASON;
        gSeason = &sSeasonState;
        gGame.flags |= GAME_FLAG_TRACK_CHOSEN;
        mem_fill(&sSeasonState, 0, sizeof(Season));
        gSeason->cup = &gCups[gGame.cupSel];
        if (gGame.difficulty == 0) {
            gCurTrack = &gTracks[TRACK_TUTORIAL];
            gGame.mode = GAME_MODE_TUTORIAL;
            *(volatile s32 *)&gGame.numPlayers = 1; /* volatile: the store stays before the next one */
            *(volatile s32 *)&gEvent.tutorialBuf = 0;
            gGame.riders[0] = 4;
            gGame.flags &= ~GAME_FLAG_TRACK_CHOSEN;
            gSeason->cup = &gCups[CUP_TUTORIAL];
            gSeason->round = 0;
            for (i = 0; i < 17; i++) {
                if (gTutorialLessons[i].level == gSaveData_8.tutorialProgress) {
                    gSeason->round = i;
                    return;
                }
            }
        }
    }
}

#define PRESSED(m) ((gPadInput & (m)) != 0 && (gPadInputPrev & (m)) == 0)

/* The track lists, the records and the custom round's track picker show the tracks 5 to a page. */
#define TRACKS_PER_PAGE 5

/* The splash and notice screens: A or START skips one once it has been up 60 frames; it ends by itself when
   gGame.screenTimer runs out (shell_load_screen sets it). */
void shell_splash_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 skip;
    s32 canSkip;
    s16 type;

    skip = 0;
    type = screen->type;
    if (type == SCREEN_TYPE_SPLASH || type == SCREEN_TYPE_SPLASH_3) {
        canSkip = gGame.screenTimer < 180 - 60;
    } else if (type == SCREEN_TYPE_PAK_STATUS) {
        canSkip = gGame.screenTimer < 1200 - 60;
    } else if (type == SCREEN_TYPE_BACKUP_LOST) {
        canSkip = gGame.screenTimer < 600 - 60;
    } else {
        canSkip = gGame.screenTimer < 300 - 60;
    }
    if (canSkip) {
        if (PRESSED(INPUT_A) | PRESSED(INPUT_START)) {
            skip = 1;
        }
    }
    if (--gGame.screenTimer == 0 || skip) {
        if (skip && gGame.fadeState == 3) {
            snd_play_menu_sound(MENU_SOUND_SELECT);
        }
        shell_goto_screen(item->target, 0, 1);
    }
}

/* Controller settings: left and right pick a connected controller, A cycles its button layout (A-D). */
void shell_controller_settings_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 sound;
    s32 next;
    s32 prevSel;
    s32 config;
    u32 pressed;
    u16 sel;
    u16 oldSel;

    prevSel = (s16)gGame.padConfigSel;
    gGame.blinkTimer = gGame.blinkTimer + 1;
    if (gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    pressed = *buttons;
    if ((pressed & INPUT_B) != 0) {
        shell_goto_screen(screen->back, 0, 0);
        sound = MENU_SOUND_BACK;
        goto play;
    }
    if ((pressed & INPUT_LEFT) != 0) {
        do {
            oldSel = gGame.padConfigSel;
            sel = oldSel - 1;
            gGame.padConfigSel = sel;
            if ((s32)((u32)sel << 16) < 0) {
                gGame.padConfigSel = oldSel + 3;
            }
        } while (((gContMask >> (s16)gGame.padConfigSel) & 1) == 0);
        goto check;
    }
    if ((pressed & INPUT_RIGHT) != 0) {
        do {
            oldSel = gGame.padConfigSel;
            sel = oldSel + 1;
            gGame.padConfigSel = sel;
            if ((s16)sel > 3) {
                gGame.padConfigSel = oldSel - 3;
            }
        } while (((gContMask >> (s16)gGame.padConfigSel) & 1) == 0);
        goto check;
    }
    if ((pressed & INPUT_A) == 0) {
        goto check;
    }
    config = gGame.padConfig[prevSel];
    gGame.padConfig[prevSel] = config + 1;
    if ((s8)(config + 1) > 3) {
        gGame.padConfig[(s16)gGame.padConfigSel] = gGame.padConfig[(s16)gGame.padConfigSel] - 4;
    }
    sound = MENU_SOUND_SELECT;
play:
    snd_play_menu_sound(sound);
check:
    {
        /* the controller was unplugged: the next one connected */
        u8 mask;
        mask = gContMask;
        if (((mask >> (s16)gGame.padConfigSel) & 1) == 0) {
            do {
                next = ((u16)gGame.padConfigSel + 1) & 3;
                gGame.padConfigSel = next;
            } while (next != prevSel && ((mask >> next) & 1) == 0);
        }
    }
    if (prevSel != (s16)gGame.padConfigSel) {
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
    return;
}

/* Records: the cursor moves over the record tracks, a page of 5 to a column. Each move shows the track's picture
   for 6 frames, then fades it to grey under the records over 59 frames. */
void shell_records_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 sel;
    s32 oldSel;
    s16 numTracks;
    u32 pressed;
    s32 animTimer;
    s32 blinkTimer;
    s32 screenTimer;
    s32 sel2;
    s32 sel3;

    animTimer = gGame.menuAnimTimer + 1;
    gGame.menuAnimTimer = animTimer;
    if (animTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    blinkTimer = gGame.blinkTimer + 1;
    gGame.blinkTimer = blinkTimer;
    if (blinkTimer == 64) {
        gGame.blinkTimer = 0;
    }
    if (gGame.screenTimer != 0) {
        screenTimer = gGame.screenTimer - 1;
        gGame.screenTimer = screenTimer;
        if (screenTimer == 0) {
            if (gGame.screenState != SCREEN_STATE_RECORDS_FADE) {
                if (gGame.screenState == SCREEN_STATE_RECORDS_PICTURE) {
                    gGame.screenState = SCREEN_STATE_RECORDS_FADE;
                    gGame.screenTimer = 59;
                }
            } else {
                gGame.screenState = SCREEN_STATE_RECORDS_SHOWN;
            }
        }
    }
    pressed = *buttons;
    if ((pressed & INPUT_B) != 0) {
        shell_goto_screen(screen->back, 0, 0);
        snd_play_menu_sound(MENU_SOUND_BACK);
        return;
    }
    oldSel = gGame.recordsSel;
    if ((pressed & INPUT_UP) != 0) {
        if (oldSel % TRACKS_PER_PAGE != 0) {
            gGame.recordsSel = gGame.recordsSel - 1;
        }
    }
    if ((*buttons & INPUT_DOWN) != 0) {
        sel2 = gGame.recordsSel;
        if (sel2 % TRACKS_PER_PAGE < TRACKS_PER_PAGE - 1 && sel2 + 1 < gGame.numRecordTracks) {
            gGame.recordsSel = sel2 + 1;
        }
    }
    if ((*buttons & INPUT_LEFT) != 0) {
        sel3 = gGame.recordsSel;
        if (sel3 / 5 + (sel3 >> 31) != (sel3 >> 31)) { /* not on the first page */
            gGame.recordsSel = sel3 - TRACKS_PER_PAGE;
        }
    }
    if ((*buttons & INPUT_RIGHT) != 0) {
        sel = gGame.recordsSel;
        numTracks = gGame.numRecordTracks;
        if (sel + TRACKS_PER_PAGE < numTracks) {
            gGame.recordsSel = sel + TRACKS_PER_PAGE;
        } else if (sel / TRACKS_PER_PAGE < (s16)(numTracks / TRACKS_PER_PAGE)) {
            gGame.recordsSel = numTracks - 1;
        }
    }
    if (oldSel != gGame.recordsSel) {
        gGame.screenState = SCREEN_STATE_RECORDS_PICTURE;
        gGame.screenTimer = 6;
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
}

#define BT gPadInput_2[pad]
#define PRESSED_2(pad, m) ((gPadInput_2[pad] & (m)) && !(gPadInputPrev_2[pad] & (m)))

/* The text entry keyboard (gKeyboardKeys): 8 keys a row, 5 rows. The cursor moves in 0-799 on both axes, a key
   being 100 wide and 160 high. */
#define KEYBOARD_COLUMNS 8
#define KEY_DEL          38
#define KEY_OK           39
#define KEY_CURSOR_RANGE 800
#define CHEAT_CODE_LEN   10
#define INITIALS_LEN     3
#define NUM_CHEATS       14 /* sCheatTable */
#define CHEAT_DEBUG      2  /* sets GAME_FLAG_DEBUG instead of its bit of sCheatMask */

typedef struct {
    u8 c[3];
} Initials;

/* Cheat code and initials entry: the stick or the D-pad moves over the keyboard, A types the key (Del and OK
   included), B deletes. A cheat code toggles its cheat; initials go to the race's records (gEvent.record) and
   the player's initials, then to the next player who set a record. */
void shell_text_entry_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 isCheat;
    s32 pad;
    s32 player;
    s32 found;
    s32 i;
    s32 sound;
    s32 pressed;
    s32 oldKey;
    s32 minX, maxX, minY, maxY;
    u8 *code;
    u8 *name;
    s32 c;

    isCheat = screen->type == SCREEN_TYPE_CHEAT_ENTRY;
    if (++gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    if (isCheat) {
        pad = 0;
    } else {
        player = gGame.recordPlayer[gGame.entryRecord];
        pad = gGame.playerPad[player];
    }
    if ((u32)(gGame.screenState - SCREEN_STATE_CHEAT_ACCEPTED) < 2) {
        /* the cheat's result: A goes back to the main menu */
        if (buttons[pad] & INPUT_A) {
            shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 0);
        }
        return;
    }
    pressed = buttons[pad];
    if (pressed & INPUT_A) {
        found = 0;
        if (gGame.keyIndex == KEY_DEL && gGame.nameLen == 0) {
            sound = MENU_SOUND_ERROR;
        } else if (isCheat && gGame.keyIndex == KEY_OK) {
            {
                for (i = 0; i < NUM_CHEATS; i++) {
                    code = (u8 *)sCheatTable[i].code;
                    name = gGame.name;
                loop:
                    c = *code;
                    if (c == *name) {
                        if (c == 0) {
                            found = 1;
                            c = 1 << i;
                            gGame.cheatEntered = i;
                            if (c == 1 << CHEAT_DEBUG) {
                                gGame.flags ^= GAME_FLAG_DEBUG;
                            } else {
                                sCheatMask ^= c;
                            }
                        } else {
                            code++;
                            name++;
                            goto loop;
                        }
                    }
                    if (found) {
                        goto ok;
                    }
                }
                sound = MENU_SOUND_ERROR;
            }
        } else {
        ok:
            sound = MENU_SOUND_SELECT;
        }
        snd_play_menu_sound(sound);
        if (gGame.keyIndex == KEY_DEL) {
            if (gGame.nameLen != 0) {
                gGame.name[gGame.nameLen--] = 0;
            }
        } else if (gGame.keyIndex == KEY_OK) {
            if (isCheat) {
                if (found) {
                    gGame.screenState = SCREEN_STATE_CHEAT_ACCEPTED;
                } else {
                    gGame.screenState = SCREEN_STATE_CHEAT_INVALID;
                }
                return;
            }
            /* the same initials for the player's other records of the race */
            for (i = gGame.entryRecord + 1; i < 4; i++) {
                if (gGame.recordPlayer[i] == gGame.recordPlayer[gGame.entryRecord]) {
                    *(Initials *)gEvent.record.initials[i] = *(Initials *)gGame.name;
                    if (i == 3) {
                        gGame.bestLapPlayer = gGame.recordPlayer[i];
                    }
                    gGame.recordPlayer[i] = -1;
                }
            }
            { s32 k = gGame.entryRecord; *(Initials *)gEvent.record.initials[k] = *(Initials *)gGame.name; }
            {
                Initials *initials = (Initials *)gGame.playerInitials;
                initials[player] = *(Initials *)gGame.name;
            }
            /* the next player with a record */
            for (i = gGame.entryRecord + 1; i < 4; i++) {
                if (gGame.recordPlayer[i] != gGame.recordPlayer[gGame.entryRecord] && gGame.recordPlayer[i] != -1) {
                    gGame.entryRecord = i;
                    break;
                }
            }
            if (i < 4) {
                s32 k = gGame.recordPlayer[gGame.entryRecord];
                if (gGame.playerInitials[k][0] != 0) {
                    /* the player's initials of before, the cursor on OK */
                    Initials *initials = (Initials *)gGame.playerInitials;
                    *(Initials *)gGame.name = initials[k];
                    gGame.name[3] = 0;
                    gGame.nameLen = strlen(gGame.name);
                    gGame.keyCursorX = KEY_CURSOR_RANGE - 1;
                    gGame.keyCursorY = KEY_CURSOR_RANGE - 1;
                    gGame.keyIndex = KEY_OK;
                } else {
                    gGame.name[0] = 'A';
                    gGame.name[1] = 0;
                    gGame.name[2] = 0;
                    gGame.name[3] = 0;
                    gGame.nameLen = 0;
                    gGame.keyCursorX = 0;
                    gGame.keyCursorY = 0;
                    gGame.keyIndex = 0;
                }
                {
                    Pic *bg = screen->bg;
                    screen->bg = (Pic *)gGame.otherBg;
                    gGame.otherBg = (Img *)bg;
                }
                return;
            }
            shell_goto_screen(screen->items->target, 0, 0);
        } else {
            /* a character: the cursor goes to OK once the name is full */
            s32 len = (s16)++gGame.nameLen;
            if (len >= (isCheat ? CHEAT_CODE_LEN : INITIALS_LEN)) {
                gGame.keyCursorX = KEY_CURSOR_RANGE - 1;
                gGame.keyCursorY = KEY_CURSOR_RANGE - 1;
                gGame.keyIndex = KEY_OK;
            }
        }
        return;
    }
    if (pressed & INPUT_B) {
        if (gGame.nameLen != 0) {
            gGame.name[gGame.nameLen - 1] = gGame.name[gGame.nameLen];
            sound = MENU_SOUND_SELECT;
            gGame.name[gGame.nameLen--] = 0;
        } else {
            sound = MENU_SOUND_ERROR;
        }
        snd_play_menu_sound(sound);
        return;
    }
    oldKey = gGame.keyIndex;
    if (gGame.nameLen < (isCheat ? CHEAT_CODE_LEN : INITIALS_LEN)) {
        /* the stick moves the cursor by how far it is pushed, the D-pad a key at a time; it wraps around */
        s32 cursorX, cursorY;
        minX = 0;
        minY = minX;
        maxX = KEY_CURSOR_RANGE;
        maxY = maxX;
        gGame.keyCursorX += (BT & (INPUT_LEFT | INPUT_RIGHT)) ? ((BT & INPUT_RIGHT) ? ((BT >> 24) & 0xF) : -((BT >> 24) & 0xF)) : 0;
        if (PRESSED_2(pad, INPUT_DPAD_LEFT)) {
            gGame.keyCursorX -= 100;
        }
        if (PRESSED_2(pad, INPUT_DPAD_RIGHT)) {
            gGame.keyCursorX += 100;
        }
        if (gGame.keyCursorX >= maxX) {
            gGame.keyCursorX = minX;
        } else if (gGame.keyCursorX < minX) {
            gGame.keyCursorX = maxX - 1;
        }
        gGame.keyCursorY -= (BT & (INPUT_UP | INPUT_DOWN)) ? ((BT & INPUT_UP) ? ((s32)((u32)BT >> 28) * 3 >> 1) : (-(s32)((u32)BT >> 28) * 3) / 2) : 0;
        if (PRESSED_2(pad, INPUT_DPAD_UP)) {
            gGame.keyCursorY -= 160;
        }
        if (PRESSED_2(pad, INPUT_DPAD_DOWN)) {
            gGame.keyCursorY += 160;
        }
        if (gGame.keyCursorY >= maxY) {
            gGame.keyCursorY = minY;
        } else if (gGame.keyCursorY < minY) {
            gGame.keyCursorY = maxY - 1;
        }
        cursorX = gGame.keyCursorX;
        cursorY = gGame.keyCursorY;
        gGame.keyIndex = cursorX / 100 + cursorY / 160 * KEYBOARD_COLUMNS;
    } else if (buttons[pad] & (INPUT_LEFT | INPUT_RIGHT)) {
        /* the name is full: left and right go between Del and OK */
        if (gGame.keyIndex == KEY_OK) {
            gGame.keyCursorX = 600;
            gGame.keyCursorY = KEY_CURSOR_RANGE - 1;
            gGame.keyIndex = KEY_DEL;
        } else {
            gGame.keyCursorX = KEY_CURSOR_RANGE - 1;
            gGame.keyCursorY = KEY_CURSOR_RANGE - 1;
            gGame.keyIndex = KEY_OK;
        }
    }
    /* the key under the cursor shows as the next character */
    if (gGame.keyIndex < KEY_DEL) {
        if (strlen(gGame.name) == 0) {
            gGame.name[1] = 0;
        }
        gGame.name[gGame.nameLen] = *gKeyboardKeys[gGame.keyIndex];
    } else {
        gGame.name[gGame.nameLen] = 0;
    }
    if (oldKey != gGame.keyIndex) {
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
}

typedef struct {
    u8 pad[0x50];
    u32 flags;          /* TRACK_FLAG_* */
} Trk_11;

extern Trk_11 *gCurTrack_38;

/* The menu of the results screens (gGame.menuSel; the strings are sExhibitionResultOptions and the others) */
#define RESULTS_RETRY  0 /* Retry, New Track, Continue */
#define RESULTS_REPLAY 1 /* Show Replay; time trial: Save Ghost (greyed and skipped in this build's menus) */
#define RESULTS_QUIT   2 /* Quit, Main Menu, Retire */

#define GHOST_SIZE 0x2800 /* a time trial's replay, as saved */

/* Race results: A opens the menu, then runs its choice: race again or pick a new track, watch the replay (save
   the ghost in a time trial), or quit to the main menu. The replay item is skipped when the replay did not fit. */
void shell_race_results_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    u32 pressed;
    s32 oldSel, sel, i;

    gGame.menuAnimTimer++;
    if (gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    gGame.blinkTimer++;
    if (gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    pressed = *buttons;
    if (pressed & INPUT_B) {
        if (gGame.screenState == SCREEN_STATE_RESULTS_MENU) {
            gGame.screenState = SCREEN_STATE_NORMAL;
        }
        return;
    }
    if (pressed & INPUT_A) {
        snd_play_menu_sound(MENU_SOUND_SELECT);
        if (gGame.screenState == SCREEN_STATE_RESULTS_MENU) {
            switch (gGame.menuSel) {
            case RESULTS_RETRY:
                if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                    if (!(gGame.flags & GAME_FLAG_GHOST_LOADED)) {
                        gActiveReplay = 0;
                    }
                    shell_goto_screen(0, 0, 0);
                } else if (gCurTrack_38->flags & TRACK_FLAG_USER) {
                    shell_goto_screen(0, 0, 0);
                    gActiveReplay = 0;
                } else {
                    gActiveReplay = 0;
                    shell_goto_screen(&sScreens[SCREEN_BRONZE_TRACKS], 0, 1);
                }
                break;
            case RESULTS_REPLAY:
                if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                    gGame.pakScreenMode = PAK_SCREEN_SAVE_GHOST;
                    shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
                } else {
                    /* as shell_backup_race_results */
                    gGame.resultsNumRiders = gEvent.numRiders;
                    for (i = 0; i < 6; i++) {
                        gGame.resultsOrder[i] = gEvent.finishOrder[i];
                        gGame.resultsTimes[i] = gRiders[i].race.finishTime;
                        gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
                        if (gSeason != 0) {
                            gGame.resultsSeasonOrder[i] = gSeason->standings[i];
                        }
                    }
                    shell_goto_screen(0, 0, 1);
                    gReplayPlayback = 1;
                    gGame.returnScreen = screen;
                    gGame.flags |= GAME_FLAG_REPLAY;
                }
                break;
            case RESULTS_QUIT:
                gGame.flags &= ~GAME_FLAG_GHOST_LOADED;
                if (gCurTrack_38->flags & TRACK_FLAG_USER) {
                    mem_free((void *)gUserTrackLayout);
                    gUserTrackLayout = 0;
                }
                shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                gActiveReplay = 0;
                break;
            }
        } else if (screen->type == SCREEN_TYPE_SEASON_RACE_RESULT) {
            shell_goto_screen(&sScreens[SCREEN_SEASON_STANDINGS], 1, 1);
        } else {
            gGame.menuSel = RESULTS_RETRY;
            gGame.screenState = SCREEN_STATE_RESULTS_MENU;
        }
        return;
    }
    if (gGame.screenState != SCREEN_STATE_RESULTS_MENU) {
        return;
    }
    oldSel = gGame.menuSel;
    if (pressed & (INPUT_DPAD_UP | INPUT_UP)) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame.mode == GAME_MODE_TIME_TRIAL) {
            gGame.menuSel = oldSel ^ 2;
        } else {
            gGame.menuSel = oldSel - 1;
            if (oldSel - 1 < 0) {
                gGame.menuSel = oldSel + 2;
            }
        }
    }
    if (*buttons & (INPUT_DPAD_DOWN | INPUT_DOWN)) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame.mode == GAME_MODE_TIME_TRIAL) {
            gGame.menuSel ^= 2;
        } else {
            sel = gGame.menuSel;
            gGame.menuSel = sel + 1;
            if (sel + 1 >= 3) {
                gGame.menuSel = sel - 2;
            }
        }
    }
    if (gGame.menuSel != oldSel) {
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
}


/* Season results and standings: A opens the menu: Continue to the next round (or the podium, or the round over
   screen), Show Replay, or Retire (with a yes / no dialog). */
void shell_season_standings_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    u32 pressed;
    Cup *cup;
    s32 i;
    s32 replaySize;

    if (++gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    if (++gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    pressed = *buttons;
    if (pressed & INPUT_B) {
        snd_play_menu_sound(MENU_SOUND_BACK);
        if (gGame.screenState == SCREEN_STATE_SEASON_MENU) {
            gGame.screenState = SCREEN_STATE_NORMAL;
            return;
        }
        if (gGame.screenState == SCREEN_STATE_RETIRE_DIALOG) {
            gGame.screenState = SCREEN_STATE_SEASON_MENU;
            return;
        }
        if (screen->back != NULL) {
            shell_goto_screen(screen->back, 0, 0);
        }
    } else if (pressed & INPUT_A) {
        snd_play_menu_sound(MENU_SOUND_SELECT);
        if (gGame.screenState == SCREEN_STATE_SEASON_MENU) {
            switch (gGame.menuSel) {
            case RESULTS_RETRY:
                gGame.flags |= GAME_FLAG_TRACK_CHOSEN;
                cup = gSeason->cup;
                if (gSeason->round + 1 >= cup->numRounds) {
                    /* the last round: the podium when first overall (and the challenge race won) */
                    if (gSeason->standings[0] == 0 && (cup - gCups != CUP_CHALLENGE || gEvent.finishOrder[0] == 0)) {
                        gGame.flags = (gGame.flags & ~GAME_FLAG_TRACK_CHOSEN) | GAME_FLAG_PODIUM;
                        shell_goto_screen(0, 0, 1);
                        gActiveReplay = 0;
                        return;
                    }
                } else if (cup - gCups != CUP_CHALLENGE || gEvent.finishOrder[0] == 0) {
                    /* the next round */
                    shell_goto_screen(0, 0, 1);
                    gSeason->round++;
                    race_jitter_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
                    race_grid_from_standings();
                    gActiveReplay = 0;
                    return;
                }
                shell_goto_screen(&sScreens[SCREEN_ROUND_OVER], 0, 1);
                gActiveReplay = 0;
                return;
            case RESULTS_REPLAY:
                /* as shell_backup_race_results */
                gGame.resultsNumRiders = gEvent.numRiders;
                for (i = 0; i < 6; i++) {
                    gGame.resultsOrder[i] = gEvent.finishOrder[i];
                    gGame.resultsTimes[i] = gRiders[i].race.finishTime;
                    gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
                    if (gSeason != 0) {
                        gGame.resultsSeasonOrder[i] = gSeason->standings[i];
                    }
                }
                shell_goto_screen(0, 0, 1);
                gReplayPlayback = 1;
                gGame.returnScreen = screen;
                gGame.flags |= GAME_FLAG_REPLAY;
                return;
            case RESULTS_QUIT:
                gGame.screenState = SCREEN_STATE_RETIRE_DIALOG;
                gGame.dialogSel = 0;
                return;
            }
        } else {
            if (gGame.screenState == SCREEN_STATE_RETIRE_DIALOG) {
                switch (gGame.dialogSel) {
                case 1: /* yes */
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                    gActiveReplay = 0;
                    return;
                case 0:
                    gGame.screenState = SCREEN_STATE_SEASON_MENU;
                    return;
                }
            } else {
                gGame.menuSel = RESULTS_RETRY;
                gGame.screenState = SCREEN_STATE_SEASON_MENU;
            }
        }
    } else {
        if (gGame.screenState == SCREEN_STATE_RETIRE_DIALOG) {
            if (pressed & (INPUT_UP | INPUT_DOWN)) {
                gGame.dialogSel ^= 1;
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
        } else if (gGame.screenState == SCREEN_STATE_SEASON_MENU) {
            if (pressed & (INPUT_DPAD_UP | INPUT_UP)) {
                replaySize = gReplayWritePtr - gReplayBuf;
                if (gGame.mode != GAME_MODE_TIME_TRIAL ? replaySize >= gReplayBufSize : replaySize >= GHOST_SIZE) {
                    gGame.menuSel ^= 2;
                } else {
                    if (--gGame.menuSel < 0) {
                        gGame.menuSel += 3;
                    }
                }
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
            if (*buttons & (INPUT_DPAD_DOWN | INPUT_DOWN)) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
                replaySize = gReplayWritePtr - gReplayBuf;
                if (gGame.mode != GAME_MODE_TIME_TRIAL ? replaySize >= gReplayBufSize : replaySize >= GHOST_SIZE) {
                    gGame.menuSel ^= 2;
                } else {
                    if (++gGame.menuSel >= 3) {
                        gGame.menuSel -= 3;
                    }
                }
            }
        }
    }
}

void shell_options_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 m;
    s32 old;
    s32 d;

    if (++gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    if (gGame.optionsSel & 0x100) {
        gGame.optionsSel &= ~0x100;
        if (gGame.optionsSel == 0) {
            tunes_play_song(gReplaySongResId);
        }
    }
    if ((gPadInput & 0x20) && !(gPadInputPrev & 0x20) && gGame.fadeState == 3) {
        if (gGame.screenState == SCREEN_STATE_ERASE_DIALOG) {
            gGame.screenState = SCREEN_STATE_NORMAL;
            snd_play_menu_sound(MENU_SOUND_BACK);
        } else if (gGame.screenState != SCREEN_STATE_ERASING && gGame.screenState != SCREEN_STATE_ERASE) {
            shell_goto_screen(screen->back, 0, 0);
            sfx_stop_all();
            tunes_stop_speech();
            if (tunes_is_song_playing() != 0) {
                tunes_stop_song();
            }
            snd_play_menu_sound(MENU_SOUND_BACK);
        }
        gCreditsMode = 0;
        return;
    }
    if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10) && gGame.fadeState == 3) {
        switch (gGame.optionsSel) {
        case 3:
                        gSaveData.options ^= 1;
            snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        case 8:
                        gSaveData.options ^= 0x10;
            snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        case 1:
            sfx_play(gGame.optionsSfx, 0x2B11, gSaveData.volumes[1], 7, 1, 1);
            return;
        case 2:
            if (gGame.repeatTimer == 0) {
                tunes_queue_speech(0x9B);
                gGame.repeatTimer = 0x34;
            }
            return;
        case 10:
                        snd_play_menu_sound(MENU_SOUND_ERROR);
            return;
        case 9:
                        snd_play_menu_sound(MENU_SOUND_ERROR);
            return;
        case 7:
            if (gHasExpansionPak == 0) {
                return;
            }
                        gSaveData.options ^= 2;
            snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        case 4:
            shell_goto_screen(&sScreens[SCREEN_CONTROLLER_SETTINGS], 1, 1);
                        snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        case 5:
            shell_goto_screen(&sScreens[SCREEN_RECORDS], 1, 1);
                        snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        case 6:
            shell_goto_screen(NULL, 0, 1);
                        gCreditsMode = 1;
            gGame.mode = GAME_MODE_EXHIBITION;
            gCurTrack = &gTracks[TRACK_GRAVEL_PIT];
            gGame.numPlayers = 1;
            gGame.returnScreen = screen;
            {
                s32 f = gGame.flags & ~0x80;
                *(volatile s32 *)&gGame.flags = f;
                gGame.flags = f | 0x05000000;
            }
            snd_play_menu_sound(MENU_SOUND_SELECT);
            return;
        }
        return;
    }
    m = gGame.screenState;
    if (m == 6 || m == 15 || m == 14) {
        if (m == 15) {
            save_load(1);
            gGame.screenState = SCREEN_STATE_NORMAL;
        } else if (m != 14) {
            if (*buttons & 3) {
                gGame.dialogSel ^= 1;
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
        }
        return;
    }
    old = gGame.optionsSel;
    if (*buttons & 2) {
        if (old < 3) {
            gGame.optionsSel += 1;
        } else {
            gGame.optionsSel += 2;
        }
        if (gGame.optionsSel >= 11) {
            gGame.optionsSel = 0;
        }
    }
    if (*buttons & 1) {
        if (gGame.optionsSel < 4) {
            gGame.optionsSel -= 1;
        } else {
            gGame.optionsSel -= 2;
        }
        if (gGame.optionsSel < 0) {
            gGame.optionsSel = 9;
        }
    }
    if (old != gGame.optionsSel) {
        switch (old) {
        case 2:
            tunes_stop_speech();
            break;
        case 0:
            tunes_stop_song();
            break;
        case 1:
            break;
        }
        switch (gGame.optionsSel) {
        case 2:
            tunes_queue_speech(0x9B);
            gGame.repeatTimer = 0x34;
            break;
        case 0:
            tunes_play_song(gReplaySongResId);
            break;
        case 1:
            sfx_play(gGame.optionsSfx, 0x2B11, gSaveData.volumes[1], 7, 1, 1);
            break;
        }
    }
    if (gGame.optionsSel >= 3) {
        if (*buttons & 0xC) {
            if ((gGame.optionsSel - 3) & 1) {
                gGame.optionsSel--;
            } else {
                gGame.optionsSel++;
            }
        }
    } else {
        if (gGame.repeatTimer != 0) {
            gGame.repeatTimer--;
        }
        if ((gPadInput & 0x2000) && !(gPadInputPrev & 0x2000)) {
            d = -8;
        } else if ((gPadInput & 0x4000) && !(gPadInputPrev & 0x4000)) {
            d = 8;
        } else {
            s32 b = gPadInput;
            if (b & 0xC) {
                d = ((b & 8) ? ((b >> 24) & 0xF) : -((b >> 24) & 0xF)) / 3;
            } else {
                d = 0;
            }
        }
        if (d != 0 && gGame.repeatTimer == 0) {
            switch (gGame.optionsSel) {
            case 1:
                sfx_play(gGame.optionsSfx, 0x2B11, gSaveData.volumes[1], 7, 1, 1);
                gGame.repeatTimer = 0x28;
                break;
            case 2:
                tunes_queue_speech(0x9B);
                gGame.repeatTimer = 0x34;
                break;
            }
        }
        if (d < 0) {
            switch (gGame.optionsSel) {
            case 1:
                if (gSaveData.volumes[1] >= -d) {
                    gSaveData.volumes[1] += d;
                } else {
                    gSaveData.volumes[1] = 0;
                }
                break;
            case 0:
                if (gSaveData.volumes[0] >= -d) {
                    gSaveData.volumes[0] += d;
                } else {
                    gSaveData.volumes[0] = 0;
                }
                break;
            case 2:
                if (gSaveData.volumes[2] >= -d) {
                    gSaveData.volumes[2] += d;
                } else {
                    gSaveData.volumes[2] = 0;
                }
                break;
            }
        } else {
            s32 t;
            switch (gGame.optionsSel) {
            case 1:
                t = gSaveData.volumes[1] + d;
                if (t >= 256) {
                    gSaveData.volumes[1] = 255;
                } else {
                    gSaveData.volumes[1] = t;
                }
                break;
            case 0:
                t = gSaveData.volumes[0] + d;
                if (t >= 256) {
                    gSaveData.volumes[0] = 255;
                } else {
                    gSaveData.volumes[0] = t;
                }
                break;
            case 2:
                t = gSaveData.volumes[2] + d;
                if (t >= 256) {
                    gSaveData.volumes[2] = 255;
                } else {
                    gSaveData.volumes[2] = t;
                }
                break;
            }
        }
        snd_set_volume(1, gSaveData.volumes[0]);
        snd_set_volume(2, gSaveData.volumes[2]);
        snd_set_volume(0, gSaveData.volumes[1]);
    }
    if (gGame.optionsSel != old) {
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
}

void shell_rider_select_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    Game *g;
    Game *h;
    Game *k;
    Game *m;
    s32 i;
    s32 ok;
    s32 idx;
    s32 t;
    u32 mask;

    g = &gGame;
    for (i = 0; i < 4; i++) {
        if (gGame.riderSelectTimer[i] != 0) {
            gGame.riderSelectTimer[i]--;
        }
        gGame.riderSelectRot[i][0] = (gGame.riderSelectRot[i][0] + 0x23) & 0xFFFF;
    }

    ok = 1;
    h = &gGame;
    for (i = 0; i < h->numPlayers; i++) {
        if (!(gGame.flags & GAME_FLAG_PLAYER_READY(i))) {
            ok = 0;
        }
    }
    race_update(sNoInputPads);

    k = &gGame;
    if (k->numPlayers == 1) {
        if (*buttons & 0x780) snd_play_menu_sound(MENU_SOUND_MOVE);
        if (*buttons & 0x200) k->riderSkinSel = (k->riderSkinSel + 5) % 6;
        if (*buttons & 0x400) k->riderSkinSel = (k->riderSkinSel + 1) % 6;
        if (k->mode != GAME_MODE_TIME_TRIAL) {
            if (*buttons & 0x80) k->bikeSkinSel = (k->bikeSkinSel + 5) % 6;
            if (*buttons & 0x100) k->bikeSkinSel = (k->bikeSkinSel + 1) % 6;
        }
    }

    if ((*buttons & 0x50) && ok) {
        snd_play_menu_sound(MENU_SOUND_SELECT);
        if (gGame.savedMode < GAME_MODE_TIME_TRIAL) {
            if (gGame.savedMode >= GAME_MODE_SOCCER) {
                shell_goto_screen(0, 1, 1);
            } else {
                shell_goto_screen(screen->items->target, 1, 1);
            }
        } else {
            shell_goto_screen(screen->items->target, 1, 1);
        }
        return;
    }
    if (*buttons & 0x20) {
        if (!(gGame.flags & 0x400)) {
            if (screen->back != NULL) {
                if (screen->back == &sScreens[SCREEN_PAK] && gUserTrackLayout != 0) {
                    gGame.flags |= 0x2000000;
                }
                shell_goto_screen(screen->back, 0, 0);
                snd_play_menu_sound(MENU_SOUND_BACK);
            }
            return;
        }
    }
    m = &gGame;
    if (m->fadeState != 3) return;
    for (i = 0; i < gGame.numPlayers; i++) {
        if (!((gContMask >> (idx = gGame.playerPad[i])) & 1)) continue;
        if (gGame.riderSelectState[i] == 0 && !(gGame.flags & GAME_FLAG_PLAYER_READY(i))) {
            if (buttons[idx] & 4) {
                t = gGame.riders[i];
                gGame.riders[i] = t - 1;
                if (t - 1 < 0) gGame.riders[i] = t + 5;
                if (gGame.numPlayers == 1) gGame.riderSkinSel = (gGame.riderSkinSel + 5) % 6;
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
            if (buttons[gGame.playerPad[i]] & 8) {
                t = gGame.riders[i];
                gGame.riders[i] = t + 1;
                if (t + 1 >= 6) gGame.riders[i] = t - 5;
                if (gGame.numPlayers == 1) gGame.riderSkinSel = (gGame.riderSkinSel + 1) % 6;
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
        }
        if (gGame.numPlayers >= 3 || gGame.riderSelectState[i] == 0) {
            if (!(gGame.flags & GAME_FLAG_PLAYER_READY(i)) && (buttons[gGame.playerPad[i]] & 0x50)) {
                gGame.riderSelectTimer[i] = 0x28 - gGame.riderSelectTimer[i];
                gGame.flags |= GAME_FLAG_PLAYER_READY(i);
                snd_play_menu_sound(MENU_SOUND_SELECT);
            } else if ((gPadInput & 0x20000) && gGame.numPlayers == 1) {
                gGame.riderSelectState[i] = 1;
                gGame.riderSelectAnim[i] = 0x3D;
            }
        } else {
            if (gGame.riderSelectState[i] == 1) {
                if (--gGame.riderSelectAnim[i] <= 0) {
                    gGame.riderSelectState[i] = 2;
                } else if (!(gPadInput & 0x20000)) {
                    gGame.riderSelectState[i] = 3;
                    gGame.riderSelectAnim[i] = 0x3E - gGame.riderSelectAnim[i];
                }
            } else if (gGame.riderSelectState[i] == 2) {
                if (!(gPadInput & 0x20000)) {
                    gGame.riderSelectState[i] = 3;
                    gGame.riderSelectAnim[i] = 0x3D;
                }
            } else if (gGame.riderSelectState[i] == 3) {
                if (gPadInput & 0x20000) {
                    gGame.riderSelectState[i] = 1;
                    gGame.riderSelectAnim[i] = 0x3E - gGame.riderSelectAnim[i];
                }
                if (--gGame.riderSelectAnim[i] <= 0) {
                    gGame.riderSelectState[i] = 0;
                }
            }
        }
        mask = GAME_FLAG_PLAYER_READY(i);
        if ((gGame.flags & mask) && (buttons[gGame.playerPad[i]] & 0x20)) {
            snd_play_menu_sound(MENU_SOUND_BACK);
            gGame.riderSelectTimer[i] = 0x28 - gGame.riderSelectTimer[i];
            gGame.flags &= ~mask;
        }
    }
}

#define OK (gSaveData_9.f2C0 > gGame.difficulty \
    || (gSaveData_9.f2C0 == gGame.difficulty && (*screen->cursor == 0 || ((gSaveData_9.f2BF >> (*screen->cursor - 1)) & 1))) \
    || (gGame.mode == GAME_MODE_SEASON && gSeason != NULL && gSeason->cup - gCups == CUP_CUSTOM \
        && gSaveData_9.f2C0 == gGame.difficulty && (gSaveData_9.f2BF & 8)))

typedef struct {
    u8 pad0[0x2A8];
    u8 f2A8[5];
    u8 pad2AD[0x2BF - 0x2AD];
    u8 f2BF;
    u8 f2C0;
} Save_3;

extern Save_3 gSaveData_9;

void shell_season_select_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 k, j, i;

    if (++gGame.menuAnimTimer == 60) gGame.menuAnimTimer = 0;
    if (++gGame.blinkTimer == 64) gGame.blinkTimer = 0;
    switch (gGame.seasonMenuState) {
    case 0:
        if (++gGame.cupPreviewTimer == 60) {
            if (++gGame.cupPreviewTrack >= gCups[*screen->cursor].numRounds) gGame.cupPreviewTrack = 0;
            gGame.cupPreviewTimer = 0;
        }
        if (*screen->cursor == CUP_CUSTOM && gCups[CUP_CUSTOM].numRounds != 0) {
            if (*buttons & 1) {
                s32 v = gGame.roundSel;
                gGame.roundSel = v - 1;
                if (v - 1 < 0) gGame.roundSel = v + 1;
            }
            if (*buttons & 2) {
                s32 v = gGame.roundSel;
                if (++gGame.roundSel >= 2) gGame.roundSel -= 2;
            }
        }
        if (gGame.difficulty != 0 && (*buttons & 0xC)) {
            if (*buttons & 8) {
                s32 *p = screen->cursor;
                s32 v = *p;
                switch (gGame.difficulty) {
                case 1:
                    if (v <= 0) *p = v + 1;
                    break;
                case 2:
                    if (v < 2) *p = v + 1;
                    break;
                case 3:
                    if (v < 5) *p = v + 1;
                    break;
                }
                if (v != *screen->cursor) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                    gGame.cupPreviewTimer = 0;
                    gGame.cupPreviewTrack = 0;
                }
            } else if (*screen->cursor > 0) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
                (*screen->cursor)--;
                gGame.cupPreviewTimer = 0;
                gGame.cupPreviewTrack = 0;
            }
        }
        if ((*buttons & 0x50) && !OK) {
            snd_play_menu_sound(MENU_SOUND_ERROR);
        } else if ((*buttons & 0x50) && OK) {
            if (*screen->cursor == CUP_CUSTOM && (gGame.roundSel == 1 || gCups[CUP_CUSTOM].numRounds == 0)) {
                gGame.seasonMenuState = 1;
                gGame.roundSel = 0;
            } else if (!(item->flags & SCREEN_ITEM_LOCKED)) {
                shell_goto_screen(item->target, 1, 1);
                snd_play_menu_sound(MENU_SOUND_SELECT);
            } else {
                snd_play_menu_sound(MENU_SOUND_ERROR);
            }
        } else if ((*buttons & 0x20) && screen->back != NULL) {
            shell_goto_screen(screen->back, 0, 0);
            snd_play_menu_sound(MENU_SOUND_BACK);
        }
        break;
    case 1:
        if ((*buttons & 1) && gGame.roundSel != 0) gGame.roundSel--;
        if (gGame.roundSel < gCups[*screen->cursor].numRounds) {
            if (*buttons & 4) {
                if (gCups[*screen->cursor].laps[gGame.roundSel] >= 2) gCups[*screen->cursor].laps[gGame.roundSel]--;
            }
            if (*buttons & 8) {
                if (gCups[*screen->cursor].laps[gGame.roundSel] < 6) gCups[*screen->cursor].laps[gGame.roundSel]++;
            }
        }
        if ((*buttons & 2) && gGame.roundSel < gCups[*screen->cursor].numRounds) {
            s32 v = gGame.roundSel + 1;
            if (v < 5) gGame.roundSel = v;
        }
        k = *buttons;
        if (k & 0x50) {
            u16 w;
            gGame.seasonMenuState = 2;
            if (gGame.roundSel == gCups[*screen->cursor].numRounds) {
                gGame.customTrackSel = 0;
                gCups[*screen->cursor].progress[0]++;
                gCups[*screen->cursor].progress[1]++;
                gCups[*screen->cursor].progress[2]++;
                gCups[*screen->cursor].numRounds++;
                gGame.customTrackPrev = -1;
            } else {
                w = gCups[*screen->cursor].tracks[gGame.roundSel];
                gGame.customTrackSel = w;
                gGame.customTrackPrev = w;
            }
            gCups[*screen->cursor].tracks[gGame.roundSel] = gGame.customTrackSel;
        } else if (k & 0x20) {
            gGame.seasonMenuState = 0;
            gGame.roundSel = 0;
            for (i = 0; i < 5; i++) {
                if (i < gCups[CUP_CUSTOM].numRounds) gSaveData_9.f2A8[i] = gCups[CUP_CUSTOM].tracks[i] & 0x1F;
                else gSaveData_9.f2A8[i] = 0x3F;
                gSaveData_9.f2A8[i] |= (gCups[CUP_CUSTOM].laps[i] & 7) << 5;
            }
            save_write(1, 0);
        } else {
            if (gGame.roundSel != gCups[*screen->cursor].numRounds && (k & 0x20000)) {
                for (j = gGame.roundSel; j < gCups[*screen->cursor].numRounds - 1; j++) {
                    gCups[*screen->cursor].tracks[j] = gCups[*screen->cursor].tracks[j + 1];
                }
                { u8 m = --gCups[*screen->cursor].numRounds;
                gCups[*screen->cursor].tracks[m] = 0; }
            }
        }
        break;
    case 2:
        if (*buttons & 4) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v - 1;
            if ((s16)v % 5 == 0) gGame.customTrackSel = v + 4;
        }
        if (*buttons & 8) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v + 1;
            if ((s16)(v + 1) % 5 == 0) gGame.customTrackSel = v - 4;
        }
        if (*buttons & 1) {
            gGame.customTrackSel -= 5;
            if (gGame.customTrackSel < 0) gGame.customTrackSel += 20;
        }
        if (*buttons & 2) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v + 5;
            if (gGame.customTrackSel >= 20) gGame.customTrackSel = v - 15;
        }
        k = *buttons;
        if (k & 0x50) {
            gGame.seasonMenuState = 1;
        } else if (k & 0x20) {
            gGame.seasonMenuState = 1;
            if (gGame.customTrackPrev == -1) gCups[*screen->cursor].numRounds--;
            else gGame.customTrackSel = gGame.customTrackPrev;
        }
        gCups[*screen->cursor].tracks[gGame.roundSel] = gGame.customTrackSel;
        break;
    }
    gSeason->cup = &gCups[*screen->cursor];
}

typedef struct {
    u8 pad[0x2BF];
    u8 mask;
    u8 cup;
} Save_4;

extern Save_4 gSaveData_10;

s32 shell_is_round_unlocked(s32 n)
{
    if (gGame.difficulty == gSaveData_10.cup) {
        s32 r = 0;
        if (n == 0 || ((gSaveData_10.mask >> (n - 1)) & 1)) {
            r = 1;
        }
        return r;
    }
    switch (gSaveData_10.cup) {
    case 1:
        if (n < 2 && ((gSaveData_10.mask >> (n - 1)) & 1)) return 1;
        break;
    case 2:
        if (n < 2 || (n == 2 && ((gSaveData_10.mask >> (n - 1)) & 1))) return 1;
        break;
    case 3:
        if (n < 3 || (n == 3 && ((gSaveData_10.mask >> (n - 1)) & 1))) return 1;
        break;
    }
    return 0;
}

typedef struct {
    u8 pad0[0x2C0];
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
} S1900;

extern S1900 gSaveData_11;

static inline s32 canGo(Screen *n) {
    s32 k;
    if (gGame.mode == GAME_MODE_TIME_TRIAL || gGame.mode == GAME_MODE_EXHIBITION) {
        k = gSaveData_11.b2C0;
    } else {
        k = gGame.difficulty;
    }
    switch (k) {
    case 1:
        if (n->firstItem < 10) return 1;
        break;
    case 2:
        if (n->firstItem < 15) return 1;
        break;
    case 3:
        return 1;
    }
    return 0;
}

/* sCurScreen again: the read takes its own lui, which the stores below do not share */
extern Screen *sCurScreen_2;

void shell_list_menu_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 snd;
    s32 m;
    s32 b;
    s32 s;
    s32 v;
    Screen *prev;
    Screen *n;
    s32 k;
    s32 ok;

    gGame.unk_29C = D_800CDB5C;
    gGame.unk_2A0 = D_800CDB60;
    D_800CDB60 -= (gPadInput & 3) ? ((gPadInput & 1) ? (s32)(gPadInput >> 28) / 6.0f : -(s32)(gPadInput >> 28) / 6.0f) : 0.0f;
    if (++gGame.menuAnimTimer == 60) gGame.menuAnimTimer = 0;
    if (++gGame.blinkTimer == 64) gGame.blinkTimer = 0;
    b = *buttons;
    if (b & 0x50) {
        snd = MENU_SOUND_MOVE;
        if (!(item->flags & SCREEN_ITEM_LOCKED)) {
            snd = MENU_SOUND_SELECT;
        } else {
            snd_play_menu_sound(MENU_SOUND_ERROR);
        }
        if (screen - sScreens == SCREEN_MAIN_MENU && (gPadInput & 0x8500) == 0x8500) {
            shell_goto_screen(&sScreens[SCREEN_CHEAT_ENTRY], 1, 0);
        } else if ((item->flags & SCREEN_ITEM_ASK_GHOST) && gGame.screenState != SCREEN_STATE_ASK_GHOST) {
            gGame.screenState = SCREEN_STATE_ASK_GHOST;
            gGame.dialogSel = 0;
        } else if ((item->flags & SCREEN_ITEM_ASK_USER_TRACK) && gGame.screenState != SCREEN_STATE_USER_TRACK_DIFFICULTY && gGame.screenState != SCREEN_STATE_ASK_USER_TRACK
                   && gGame.screenState != SCREEN_STATE_ASK_TRACK_SOURCE && !(gGame.flags & 0x200)) {
            if (!(item->flags & SCREEN_ITEM_LOCKED)) {
                gGame.screenState = SCREEN_STATE_ASK_USER_TRACK;
                gGame.dialogSel = 0;
            }
        } else if (gGame.screenState == SCREEN_STATE_USER_TRACK_DIFFICULTY) {
            gGame.mode = GAME_MODE_EXHIBITION;
            gGame.screenState = SCREEN_STATE_NORMAL;
            gGame.flags |= 0x200;
        } else if (!(item->flags & SCREEN_ITEM_ASK_PLAYERS) || gGame.screenState == SCREEN_STATE_DIFFICULTY) {
          if (!(item->flags & SCREEN_ITEM_LOCKED)) {
            if (gGame.screenState == SCREEN_STATE_ASK_GHOST && gGame.dialogSel == 1) {
                gGame.pakScreenMode = PAK_SCREEN_LOAD_GHOST;
                shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
            } else if (gGame.screenState == SCREEN_STATE_ASK_TRACK_SOURCE && !(gGame.flags & 0x200)) {
                gGame.screenState = SCREEN_STATE_USER_TRACK_DIFFICULTY;
                if (gGame.difficulty == 0) {
                    gGame.difficulty = 1;
                }
            } else if (gGame.screenState == SCREEN_STATE_ASK_USER_TRACK && gGame.dialogSel == 0) {
                gGame.screenState = SCREEN_STATE_ASK_TRACK_SOURCE;
                if (gSaveData_11.b2C4 & 8) {
                    gGame.dialogSel = 0;
                } else {
                    gGame.dialogSel = 1;
                }
            } else {
                if (gGame.flags & 0x200) {
                    if (gGame.dialogSel != 0) {
                        gGame.flags |= 0x2000000;
                        gGame.pakScreenMode = PAK_SCREEN_LOAD_USER_TRACK;
                        gGame.flags &= ~0x20;
                        shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
                    } else {
                        gCurTrack = &gTracks[TRACK_USER];
                        shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                        usrtrk_load_layout(gSaveData.userTrack);
                        gGame.mode = GAME_MODE_EXHIBITION;
                        gGame.flags &= ~0x80;
                        gGame.flags &= ~0x20;
                    }
                }
                if (gGame.screenState == SCREEN_STATE_DIFFICULTY && gGame.difficulty == 0) {
                    shell_goto_screen(0, 0, 1);
                } else {
                    shell_goto_screen(item->target, 1, 1);
                }
            }
          }
        } else {
            gGame.screenState = SCREEN_STATE_DIFFICULTY;
            if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) && gGame.difficulty == 0) {
                gGame.difficulty = 1;
            }
        }
        if (snd) {
            snd_play_menu_sound(MENU_SOUND_SELECT);
        }
    } else if (b & 0x20) {
        if (gGame.flags & 0x200) {
            gGame.screenState = SCREEN_STATE_USER_TRACK_DIFFICULTY;
            snd_play_menu_sound(MENU_SOUND_BACK);
            gGame.flags &= ~0x200;
            gGame.flags &= ~0x20;
            return;
        }
        s = gGame.screenState;
        if (s == 16) {
            gGame.screenState = SCREEN_STATE_ASK_TRACK_SOURCE;
        } else if (s == 8 || s == 5 || s == 9 || s == 10) {
            snd_play_menu_sound(MENU_SOUND_BACK);
            gGame.screenState = SCREEN_STATE_NORMAL;
            gGame.flags &= ~0x20;
        } else if (screen->back) {
            snd_play_menu_sound(MENU_SOUND_BACK);
            shell_goto_screen(screen->back, 0, 0);
        } else {
            snd_play_menu_sound(MENU_SOUND_ERROR);
        }
    } else if (gGame.flags & 0x200) {
        m = gGame.mode;
        if (m == 2) {
            if (gContCount < 2) {
                snd_play_menu_sound(MENU_SOUND_BACK);
                gGame.flags &= ~0x200;
            } else if (gGame.numPlayers == 0) {
                gGame.numPlayers = 2;
            } else if (gContCount < 4 && gGame.numPlayers == 4) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
                gGame.numPlayers = 2;
            }
            if (gContCount == 4 && (*buttons & 3)) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
                if (gGame.numPlayers == 2) {
                    gGame.numPlayers = 4;
                } else {
                    gGame.numPlayers = 2;
                }
            }
        } else if (m == 7 || m == 10) {
            if (gGame.numPlayers == 0 || gContMask == 0) {
                gGame.numPlayers = 1;
            } else {
                s32 o = gGame.numPlayers;
                if (gContCount < o) {
                    gGame.numPlayers = gContCount;
                } else if (b & 3) {
                    gGame.numPlayers = (o & 1) + 1;
                    if (gContCount == 1) {
                        gGame.numPlayers = 1;
                    }
                    if (gGame.numPlayers != o) {
                        snd_play_menu_sound(MENU_SOUND_MOVE);
                    }
                }
            }
        } else if (gContMask == 0) {
            gGame.numPlayers = 1;
        } else {
            if (gGame.numPlayers == 0) {
                gGame.numPlayers = 1;
            }
            v = gContCount;
            if (v < gGame.numPlayers) {
                gGame.numPlayers = v;
            }
            if (*buttons & 1) {
                s32 old = gGame.numPlayers;
                if ((gGame.numPlayers = old - 1) <= 0) {
                    gGame.numPlayers = v;
                }
                if (gGame.numPlayers != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
            if (*buttons & 2) {
                s32 old = gGame.numPlayers;
                gGame.numPlayers = old + 1;
                if (gContCount < old + 1) {
                    gGame.numPlayers = 1;
                }
                if (gGame.numPlayers != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
            if (gGame.mode == GAME_MODE_EXHIBITION && gGame.numPlayers == 2) {
                if (*buttons & 4) {
                    s32 old;
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                    old = gGame.twoPlayerCpuRiders;
                    if ((gGame.twoPlayerCpuRiders = old - 1) < 0) {
                        gGame.twoPlayerCpuRiders = old + 2;
                    }
                }
                if (*buttons & 8) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                    if (++gGame.twoPlayerCpuRiders >= 3) {
                        gGame.twoPlayerCpuRiders -= 3;
                    }
                }
            }
        }
    } else {
        s = gGame.screenState;
        if ((u32)(s - 8) < 3) {
            if ((s != 10 || (gSaveData_11.b2C4 & 8)) && (b & 3)) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
                gGame.dialogSel ^= 1;
            }
            return;
        }
        if (b & 1) {
            if ((s == 16 || s == 5) && gSaveData_11.b2C0) {
                s32 old = gGame.difficulty;
                if ((gGame.difficulty = old - 1) < 0) {
                    gGame.difficulty = gSaveData_11.b2C0;
                } else if (gGame.difficulty == 0 && ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || s == 16)) {
                    gGame.difficulty = gSaveData_11.b2C0;
                }
                if (gGame.difficulty != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            } else {
                s32 old = *screen->cursor;
                if ((*screen->cursor = old - 1) < 0) {
                    *screen->cursor = screen->numItems - 1;
                }
                if (*screen->cursor != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
        }
        if (*buttons & 2) {
            s32 t = gGame.screenState;
            if ((t == 16 || t == 5) && gSaveData_11.b2C0) {
                s32 old = gGame.difficulty;
                gGame.difficulty = old + 1;
                if (gSaveData_11.b2C0 < old + 1) {
                    if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || t == 16) {
                        gGame.difficulty = 1;
                    } else {
                        gGame.difficulty = 0;
                    }
                }
                if (gGame.difficulty != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            } else {
                s32 old = *screen->cursor;
                *screen->cursor = old + 1;
                if (old + 1 >= screen->numItems) {
                    *screen->cursor = 0;
                }
                if (*screen->cursor != old) {
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
        }
        if (gGame.fadeState != 1) {
            prev = sCurScreen_2;
            if (*buttons & 4) {
                if (screen->left == NULL) {
                    if (screen->right) {
                        sCurScreen = screen;
                    }
                } else {
                    screen = screen->left;
                    sCurScreen = screen;
                }
            }
            if (*buttons & 8) {
                if (screen->right == NULL) {
                    if (screen->left) {
                        sCurScreen = screen;
                    }
                } else {
                    if (canGo(screen->right)) {
                        screen = screen->right;
                    }
                    sCurScreen = screen;
                }
            }
            if (prev != sCurScreen) {
                snd_play_menu_sound(MENU_SOUND_MOVE);
            }
        }
    }
}

void shell_filter_stick_dirs(u32 *p)
{
    u32 v;
    s8 a2;
    s8 a1;
    s8 a3;
    s32 i;

    for (i = 0; i < 4; i++) {
        v = *p;
        a2 = (v >> 24) & 0xF;
        a1 = v >> 28;
        a3 = a1;
        if (a2 < 7) {
            *p = v & 0xFFFFFFF3;
            a2 = 0;
        }
        if (a1 < 7) {
            *p = *p & 0xFFFFFFFC;
            a3 = 0;
        }
        if (a2 < a3) {
            *p = *p & 0xFFFFFFF3;
        } else if (a3 < a2) {
            *p = *p & 0xFFFFFFFC;
        }
        p++;
    }
}

void shell_update(Pad16 *pad) {
    u32 buf[4];
    s32 sp20;
    s32 sp24;
    s32 sp28;
    Screen *screen;
    ScreenItem *item;
    Screen *p;
    s32 vol;
    s32 i;
    s32 any;

    mem_fill((u8 *)buf, 0, 16);
    if (!(gFrameCounter & 0xF)) {
        pak_query_controllers();
    }
    if (gContFirst || !gContMask) {
        gGame.flags |= 0x08000000;
    } else {
        gGame.flags &= ~0x08000000;
    }
    if (gContCount < 2) {
        gSpecialTracksItems[3].flags |= SCREEN_ITEM_LOCKED;
    } else {
        save_update_unlocks();
    }
    if (gGame.fadeState == 0) {
        screen = sPrevScreen;
        if (screen == &sScreens[SCREEN_OPTIONS] ||
            (!(screen->flags & SCREEN_KEEP_MUSIC) && (sCurScreen == NULL || (sCurScreen->flags & SCREEN_KEEP_MUSIC)))) {
            if (gGame.fadeState == 0) {
                vol = gGame.fadeLevel;
            }
            snd_set_master_volume(vol);
        }
    } else {
        screen = sCurScreen;
    }
    if (screen != NULL && (screen->flags & SCREEN_STICK_FILTER)) {
        shell_filter_stick_dirs(pad->w);
    }
    any = 0;
    for (i = 0; i < 4; i++) {
        if (((gContMask >> i) & 1) && pad->w[i] != 0) {
            any = 1;
        }
        if ((((s32)pad->w[i] >> 24) & 0xF) < 6) {
            pad->w[i] &= ~0xC;
        }
        if ((s32)(pad->w[i] >> 28) < 6) {
            pad->w[i] &= ~3;
        }
    }
    if (any) {
        gGame.idleFrames = 0;
    } else {
        gGame.idleFrames++;
    }
    for (i = 0; i < 4; i++) {
        buf[i] = 0;
        if (gGame.fadeState == 3) {
            buf[i] = pad->w[i] & ~sPrevPadButtons.w[i];
            if (buf[i] & 0x800) {
                buf[i] |= 1;
            }
            if (buf[i] & 0x1000) {
                buf[i] |= 2;
            }
            if (buf[i] & 0x2000) {
                buf[i] |= 4;
            }
            if (buf[i] & 0x4000) {
                buf[i] |= 8;
            }
        }
    }
    if (screen != NULL) {
        if (gGame.fadeState == 0) {
            item = &screen->items[sPrevScreenCursor];
        } else {
            item = screen->items;
            if (screen->cursor != NULL) {
                item += *screen->cursor + screen->firstItem;
            }
        }
        if (gGame.fadeState != 1) {
            switch (screen->type) {
            case SCREEN_TYPE_SPLASH:
            case SCREEN_TYPE_SPLASH_2:
            case SCREEN_TYPE_SPLASH_3:
            case SCREEN_TYPE_PAK_STATUS:
            case SCREEN_TYPE_BACKUP_LOST:
                shell_splash_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_CHEAT_ENTRY:
            case SCREEN_TYPE_INITIALS:
                shell_text_entry_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_RECORDS:
                shell_records_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_SEASON_RACE_RESULT:
            case SCREEN_TYPE_RACE_RESULT:
                shell_race_results_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_STANDINGS:
                shell_season_standings_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_ROUND_OVER:
                if (buf[0] & 0x50) {
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                    snd_play_menu_sound(MENU_SOUND_SELECT);
                } else if ((buf[0] & 0x20) && screen->back != NULL) {
                    shell_goto_screen(screen->back, 0, 0);
                    snd_play_menu_sound(MENU_SOUND_BACK);
                }
                break;
            case SCREEN_TYPE_LIST:
                if (screen - sScreens == SCREEN_MAIN_MENU) {
                    screen->back = &sScreens[SCREEN_TITLE];
                    if (gGame.idleFrames > 0xE10 && gGame.fadeState == 3) {
                        shell_goto_screen(&sScreens[SCREEN_TITLE], 0, 0);
                    }
                }
            case SCREEN_TYPE_TRACK_LIST:
                shell_list_menu_input(screen, item, buf, pad);
                if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                    gGame.trackSel = screen->firstItem + gGame.trackListSel;
                }
                break;
            case SCREEN_TYPE_CONTROLLER_SETTINGS:
                shell_controller_settings_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_PAK:
                pak_screen_update(pad);
                if (gPakScreenDone == 1 && gGame.fadeState == 3) {
                    switch (gGame.pakScreenMode) {
                    case 0:
                        if (screen->back == NULL) {
                            shell_goto_screen(&sScreens[SCREEN_SPLASH_3], 0, 1);
                        } else {
                            shell_goto_screen(screen->back, 0, 0);
                        }
                        break;
                    case 1:
                        if (gPakScreenAborted == 0) {
                            if (replay_get_info(gActiveReplay + 0x6400, &sp20, &sp24, &sp28)) {
                                shell_goto_screen(screen->back, 0, 0);
                                gActiveReplay = 0;
                                gGame.flags &= ~0x80000;
                            } else {
                                gGame.flags |= 0x80000;
                                gCurTrack = &gTracks[sp20];
                                gGame.trackSel = sp20;
                                shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                            }
                        } else {
                            gGame.flags &= ~0x80000;
                            gActiveReplay = 0;
                            shell_goto_screen(screen->back, 0, 0);
                        }
                        break;
                    case 2:
                        shell_goto_screen(screen->back, 0, 0);
                        break;
                    case 4:
                        gGame.flags |= 0x20;
                        shell_goto_screen(0, 0, 1);
                        break;
                    case 3:
                        if (gGame.flags & 0x02000000) {
                            gGame.flags &= ~0x20;
                            if (gPakScreenAborted != 0) {
                                shell_goto_screen(screen->back, 0, 0);
                            } else {
                                gCurTrack = &gTracks[TRACK_USER];
                                shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                                usrtrk_load_layout(gGame.userTrack + 4);
                                gGame.flags &= ~0x80;
                            }
                            gGame.mode = GAME_MODE_EXHIBITION;
                            mem_free(gGame.userTrack);
                            gGame.userTrack = 0;
                        } else {
                            if (gPakScreenAborted != 0) {
                                mem_free(gGame.userTrack);
                                gGame.userTrack = 0;
                            }
                            gGame.flags |= 0x20;
                            shell_goto_screen(0, 0, 1);
                        }
                        break;
                    }
                }
                break;
            case SCREEN_TYPE_OPTIONS:
                shell_options_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_TITLE:
                race_update(sNoInputPads);
                if (gGame.idleFrames > 0xE10 && gGame.fadeState == 3 &&
                    !(gGame.flags & 0x08000000)) {
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 1, 1);
                }
                break;
            case SCREEN_TYPE_RIDER_SELECT:
                shell_rider_select_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_ROUND_SELECT:
                shell_season_select_input(screen, item, buf, pad);
                break;
            }
            if (gGame.fadeState == 3 && (screen->flags & SCREEN_A_B_NAVIGATION)) {
                if (screen->type == SCREEN_TYPE_TITLE && D_800E3704 != 0) {
                    shell_goto_screen(screen->items->target, 1, 1);
                } else if (buf[0] & 0x50) {
                    if ((screen->type != SCREEN_TYPE_TITLE || !(gGame.flags & 0x08000000)) && !(item->flags & SCREEN_ITEM_LOCKED)) {
                        snd_play_menu_sound(MENU_SOUND_SELECT);
                        shell_goto_screen(item->target, 1, 1);
                    }
                } else if (buf[0] & 0x20) {
                    if (screen->back != NULL) {
                        snd_play_menu_sound(MENU_SOUND_BACK);
                        shell_goto_screen(screen->back, 0, 0);
                    } else {
                        snd_play_menu_sound(MENU_SOUND_ERROR);
                    }
                }
            }
        }
    }
    switch (gGame.fadeState) {
    case 0:
        if (gGame.fadeLevel == 0) {
            gGame.fadeLevel = 30;
            gGame.fadeState = 1;
            gGame.fadeTimer = 2;
        } else {
            gGame.fadeLevel -= 16;
            if (gGame.fadeLevel <= 0) {
                gGame.fadeLevel = 0;
            }
        }
        break;
    case 1:
        if (gGame.fadeTimer == 0) {
            snd_drain();
            snd_set_master_volume(0xFF);
            if (sPrevScreen != NULL) {
                if (tunes_is_song_playing() && (sCurScreen == NULL || (sCurScreen->flags & SCREEN_KEEP_MUSIC))) {
                    tunes_stop_song();
                }
                for (p = sPrevScreen; p != NULL; p = p->right) {
                    shell_unload_screen(p);
                }
                for (p = sPrevScreen->left; p != NULL; p = p->left) {
                    shell_unload_screen(p);
                }
            }
            if (sCurScreen != NULL) {
                for (p = sCurScreen; p != NULL; p = p->right) {
                    p->back = sCurScreen->back;
                    shell_load_screen(p);
                }
                for (p = sCurScreen->left; p != NULL; p = p->left) {
                    p->back = sCurScreen->back;
                    shell_load_screen(p);
                }
                if (!tunes_is_song_playing() && !(sCurScreen->flags & SCREEN_KEEP_MUSIC)) {
                    tunes_play_song(sMenuSongId);
                    snd_set_master_volume(0xFF);
                }
                if (sCurScreen->type == SCREEN_TYPE_TRACK_LIST) {
                    while (sCurScreen->left != NULL) {
                        sCurScreen = sCurScreen->left;
                    }
                    for (i = gGame.trackSel - 5; i >= 0; i -= 5) {
                        sCurScreen = sCurScreen->right;
                    }
                    *sCurScreen->cursor = gGame.trackSel % 5;
                }
            } else {
                gCurGameState->mode = 3;
                if (gGame.mode == GAME_MODE_TUTORIAL) {
                    gCurTrack = &gTracks[26];
                } else if (gGame.mode == GAME_MODE_SEASON) {
                    gCurTrack = &gTracks[gSeason->cup->tracks[gSeason->round]];
                }
                if (!(gGame.flags & 0x8000)) {
                    if (!(gGame.flags & 0x20)) {
                        if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                            if (gActiveReplay == 0) {
                                gActiveReplay = gReplayBuffer;
                            }
                            gActiveReplaySize = 0x6400;
                        } else if (gGame.mode != GAME_MODE_TUTORIAL) {
                            gActiveReplaySize = 0xC800;
                            gActiveReplay = gReplayBuffer;
                        }
                    }
                }
            }
            gGame.fadeTimer = -1;
        }
        if (--gGame.fadeLevel == 0) {
            if (gGame.fadeTimer != -2) {
                gGame.fadeLevel++;
            } else {
                gGame.fadeState = 2;
                gGame.fadeLevel = 255;
            }
        }
        break;
    case 2:
        gGame.fadeLevel -= 16;
        if (gGame.fadeLevel <= 0) {
            gGame.fadeState = 3;
            gGame.fadeLevel = 0;
        }
        break;
    }
    sPrevPadButtons = *pad;
}

typedef struct {
    u8 pad[0x1E];
    s16 f1E;
} Lvl_2;

extern Lvl_2 *gCurTrack_39;

void shell_exit(void)
{
    s32 i;
    s32 j;
    s32 v;
    s32 k;
    Game *g;

    gGame.flags |= 0x4000000;
    gCheatFlags = sCheatMask;
    switch (gGame.mode) {
    case 8:
    case 9:
        gCheatFlags = sCheatMask & ~0x210;
        break;
    }
    switch (gGame.mode) {
    case 0:
    case 1:
    case 8:
        if (!(gGame.flags & 0x1000000)) {
            gGame.flags &= ~0x4000000;
        }
        break;
    }
    if (gCurTrack_39 != &gTracks[TRACK_USER] && gUserTrackLayout != 0) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    if (gGame.mode == GAME_MODE_SEASON && gSeason != 0 && gSeason->cup - gCups == CUP_CHALLENGE) {
        gGame.riders[1] = gSeason->round;
        if (gGame.riders[1] == gGame.riders[0]) {
            gGame.riders[1] = 5;
        }
    } else if (gGame.mode != GAME_MODE_TIME_TRIAL) {
        v = 0;
        for (i = gGame.numPlayers; i < 8; i++) {
            for (j = 0; j < gGame.numPlayers; j++) {
                if (v == gGame.riders[j]) {
                    v++;
                    v %= 8;
                }
            }
            gGame.riders[i] = v;
            v++;
            v %= 8;
        }
    }
    if (gGame.mode != GAME_MODE_HILL_CLIMB && gGame.mode != GAME_MODE_SOCCER && gCreditsMode == 0 && !(gGame.flags & 0x40000)) {
        switch (gGame.numPlayers) {
        default:
            D_800EFDB4 = 0; /* its own symbol: gGame is in a register here */
            break;
        case 1:
            gGame.numCpuRiders = 5;
            break;
        case 2:
            gGame.numCpuRiders = gGame.numPlayers;
            break;
        }
    } else {
        gGame.numCpuRiders = 0;
    }
    if (!(gGame.flags & 0x8000)) {
        g = &gGame;
        g->numLaps = (g->mode != GAME_MODE_SEASON || gSeason == 0 || gSeason->cup - gCups != CUP_CHALLENGE) ?
            ((g->mode == GAME_MODE_SEASON && gSeason != 0 && gSeason->cup - gCups == CUP_CUSTOM) ?
             gSeason->cup->laps[gSeason->round] : gCurTrack_39->f1E) : 1;
    }
    mem_free(gGame.greyPalette);
    prim_exit();
    res_close_set(gGuiRes);
    gGuiRes = 0;
    osViSetSpecialFeatures(0xAA);
    gGame.flags &= ~1;
    if (gGame.flags & 0x40000) {
        gGame.numPlayers = gEvent.numRiders;
        if (gGame.numPlayers >= 4) {
            gGame.numPlayers = 3;
        }
        for (k = 0; k < 6; k++) {
            gGame.savedRiders[k] = gGame.riders[k];
            gGame.savedRiderSkins[k] = gGame.riderSkins[k];
        }
        for (k = 0; k < gGame.numPlayers; k++) {
            gGame.riders[k] = gGame.savedRiders[gSeason->standings[k]];
            gGame.riderSkins[k] = gGame.savedRiderSkins[gSeason->standings[k]];
        }
    }
    if (res_open_set(sStrResourceSet, &gResourceSet) != 0) {
        fatal_halt(1);
    }
    res_close_set(gResourceSet);
    mem_free(gGame.recordTracks);
    gRaceExitCode = 6;
    font_unload_set();
}

#define LOAD(id) res_unload(&gGuiRes->handles[id])

typedef struct {
    u8 pad0[0x34];
    TrackRecord f34;
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Bike_41;

extern Bike_41 *gCurTrack_40;
extern Bike_41 *D_800F0634;
extern Bike_41 gTracks_15[];

void shell_unload_screen(Screen *screen)
{
    s32 i;

    if (screen == NULL) {
        return;
    }
    gGame.flags &= ~0x200;
    LOAD(sDialogTitleBarResId);
    LOAD(sDialogFrameResId);
    if ((screen->flags & SCREEN_TEXT_OBJECTS) && screen->left == NULL) {
        for (i = 0; i < 0x30; i++) {
            font_text_destroy(gGame.texts[i]);
            mem_free(gGame.texts[i]);
        }
    }
    if (screen->flags & SCREEN_PANELS) {
        LOAD(sMenuFramesResId);
        LOAD(sMenuItemBarsResId);
    }
    if (screen->bgResId != NULL) {
        if (screen->type == SCREEN_TYPE_ROUND_SELECT) {
            for (i = 0; i < 7; i++) {
                LOAD(*((s32 **)screen->bgResId)[i]);
            }
        } else {
            LOAD(*screen->bgResId);
        }
    }
    switch (screen->type) {
    case SCREEN_TYPE_SPLASH_3:
        gGame.flags &= ~0x10000000;
        break;
    case SCREEN_TYPE_SPLASH_2:
        pak_probe_all();
        LOAD(sExpansionPakDetectedResId);
        break;
    case SCREEN_TYPE_TITLE:
        i = 0;
        LOAD(sTitleLogoResId);
        LOAD(sCopyrightResId);
        race_exit();
        gGame.numPlayers = 1;
        D_800F0634 = gCurTrack_40;
        for (; i < 6; i++) {
            gGame.riders[i] = gGame.savedRiders[i];
            gGame.riderSkins[i] = gGame.savedRiderSkins[i];
        }
        break;
    case SCREEN_TYPE_RIDER_SELECT:
        race_exit();
        if (!(gGame.flags & 0x400)) {
            gSplitVertical = 0;
        }
        gGame.flags &= ~0x3C10;
        switch (gGame.savedMode) {
        case 8:
            if (gGhostSlot == 0) {
                gGame.riders[1] = gGame.riders[0];
            }
        case 0:
        case 1:
            gGame.flags |= 0x80;
            if (gGame.savedTrack != NULL && (((Bike_41 *)gGame.savedTrack)->f50 & 2)) {
                gGame.flags &= ~0x80;
            }
            break;
        }
        i = 0;
        res_unload(&gResourceSet->handles[gHudGlyphsResId]);
        LOAD(sSmallArrowsResId);
        LOAD(sRiderSelectFramesResId);
        LOAD(sRiderSelectBarResId);
        LOAD(sCButtonIconsResId);
        LOAD(D_800D1414);
        LOAD(D_800D13A8);
        LOAD(sRiderSelectFramesResId);
        LOAD(D_800D13EC);
        gGame.screenState = SCREEN_STATE_NORMAL;
        gGame.mode = gGame.savedMode;
        gCurTrack_40 = gGame.savedTrack;
        for (; i < 6; i++) {
            if (gGame.numPlayers >= 2) {
                gGame.riderSkins[i] = i;
            } else {
                gGame.riderSkins[i] = (gGame.riderSkinSel + i) % 6;
            }
            gGame.bikeSkins[i] = gGame.bikeSkinSel;
            gGame.bikeSkins[i] += gGame.riders[i];
            gGame.bikeSkins[i] %= 6;
        }
        break;
    case SCREEN_TYPE_CONTROLLER_SETTINGS:
        shell_blend_palette_to_gray(0xFF, NULL);
        LOAD(sOptionsTitleBarResId);
        LOAD(sControllerSettingsFrameResId);
        LOAD(sControllerDiagramResId);
        LOAD(gControllerIconResId);
        mem_free(gGame.playerPalettes);
        break;
    case SCREEN_TYPE_OPTIONS:
        save_write(5, 0);
        save_write(1, 0);
        sfx_free_sample(gGame.optionsSfx);
        LOAD(sOptionsTitleBarResId);
        LOAD(gCursorPalResId);
        LOAD(sOptionsFrameResId);
        LOAD(gCursorResId);
        break;
    case SCREEN_TYPE_PAK_STATUS:
        LOAD(sPakStatusFrameResId);
        LOAD(sPakStatusTitleBarResId);
        break;
    case SCREEN_TYPE_BACKUP_LOST:
        LOAD(gWarnFrameResId);
        LOAD(gWarnTitleBarResId);
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS:
        if (screen->type == SCREEN_TYPE_CHEAT_ENTRY) {
            LOAD(sCheatPicResId);
        } else {
            gCurTrack_40->f34 = *(TrackRecord *)&gEvent.record; /* copied through a pointer, as in event_init */
            gCurTrack_40->f50 &= ~0x180;
            save_write(2, gCurTrack_40 - gTracks_15);
            for (i = 0; i < gGame.numRecordTracks; i++) {
                if (gGame.recordTracks[i] == gCurTrack_40 - gTracks_15) {
                    break;
                }
            }
            LOAD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        mem_free(gGame.playerPalettes);
        LOAD(gMenuBackgroundResId);
        LOAD(sTextEntryTitleBarResId);
        LOAD(sTextEntryFramesResId);
        LOAD(gControllerIconResId);
        break;
    case SCREEN_TYPE_SEASON_RACE_RESULT:
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS:
        mem_free(gGame.playerPalettes);
        LOAD(gControllerIconResId);
        LOAD(sResultsFramesResId);
        LOAD(sResultsTitleBarsResId);
        LOAD(gCursorResId);
        LOAD(gCursorPalResId);
        break;
    case SCREEN_TYPE_PAK:
        pak_screen_exit();
        gGame.flags &= ~0x02000000;
        break;
    case SCREEN_TYPE_LIST:
        LOAD(sSubmenuFrameResId);
        LOAD(sSubmenuItemBarResId);
    case SCREEN_TYPE_TRACK_LIST:
        LOAD(sSmallArrowsResId);
        gGame.screenState = SCREEN_STATE_NORMAL;
        LOAD(sScreenArrowsResId);
        LOAD(sSponsorLogosResId);
        LOAD(sPadlockResId);
        LOAD(sTrophyResId);
        LOAD(sWonFlagResId);
        LOAD(gTitleBarResId);
        LOAD(gCursorResId);
        LOAD(gCursorPalResId);
        for (i = 0; i < screen->numItems; i++) {
            if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                LOAD(*sTrackPicResIds[sTrackPicIndex[i + screen->firstItem]]);
            } else {
                LOAD(*sMenuItemPicResIds[i + screen->firstPic]);
            }
        }
        break;
    case SCREEN_TYPE_ROUND_SELECT:
    case SCREEN_TYPE_RECORDS:
        i = 0;
        LOAD(sScreenArrowsResId);
        LOAD(sTrophyResId);
        LOAD(sWonFlagResId);
        LOAD(gTitleBarResId);
        LOAD(sCustomRoundArrowsResId);
        LOAD(sPadlockResId);
        LOAD(gCursorResId);
        LOAD(gCursorPalResId);
        for (; i < 0x18; i++) {
            LOAD(*sTrackPicResIds[i]);
        }
        shell_blend_palette_to_gray(0xFF, NULL);
        break;
    }
}

void shell_build_player_palettes(void)
{
    s32 i;
    s32 v;

    gGame.playerPalettes = (u16 *)mem_alloc_dbg(0x800, 0, sStrShellFile, 0x174F);
    gGame.playerPalettes[0] = 0;
    gGame.playerPalettes[0x100] = 0;
    gGame.playerPalettes[0x200] = 0;
    gGame.playerPalettes[0x300] = 0;
    i = 1;
    do {
        gGame.playerPalettes[i] = ((i >> 3) << 1) | 0x18C1;
        v = i >> 3;
        gGame.playerPalettes[i + 0x100] = (v << 11) | 0xC7;
        gGame.playerPalettes[i + 0x200] = (v << 11) | (v << 6) | 7;
        gGame.playerPalettes[i + 0x300] = (v << 6) | 0x1807;
        i++;
    } while (i < 0x100);
}


void shell_update_season_progress(void)
{
    u8 *q;
    s32 old;
    s32 idx;
    u32 fl;
    Game *g;
    Game *g2;
    s32 m;
    s32 idx2;
    s32 mode;
    s32 nv;
    u32 v;
    u32 w;
    s32 md;
    s32 lv;
    u32 f;
    u8 *pb;
    u8 *e;
    u8 t8;
    Season *r;
    u8 *p0;
    u8 *cb;
    u8 *pb2;

    fl = gGame.flags;
    g = &gGame;
    if ((fl & 0x28000) == 0 && gRaceExitCode != 3) {
        v = fl | 0x20000;
        gGame.flags = v;
        mode = g->difficulty;
        switch (mode) {
        case 1: m = 3; break;
        case 2: m = 7; break;
        case 3: m = 0x1f; break;
        }
        save_update_unlocks();
        r = gSeason;
        p0 = (u8 *)r->cup;
        cb = (u8 *)gCups;
        idx = ((p0 - cb) * 0x38E38E39) >> 2;
        if (idx < 5) if (idx >= 0) {
            pb = (u8 *)r->cup;
            if ((s32)*pb <= (s32)r->round + 1) {
                if (!(idx == 4 && gEvent.finishOrder[0] != 0) && r->standings[0] == 0) {
                    g2 = &gGame;
                    q = &gSaveData_12;
                    lv = q[0x2C0];
                    md = g2->difficulty;
                    if (md == lv && ((old = q[0x2BF]) >> idx & 1) == 0) {
                        v = old | (1 << idx);
                        q[0x2BF] = v;
                        if ((((u8)v) & m) == m) {
                            if (md != 3) {
                                t8 = q[0x2C0];
                                gGame.cupSel = 0;
                                do {
                                q[0x2BF] = 0;
                                q[0x2C0] = t8 + 1;
                                } while (0);
                                gGame.flags |= 0x400000;
                            } else {
                                gGame.flags |= 0x800000;
                            }
                        } else {
                            gGame.flags |= 0x100000;
                        }
                    }
                }
            }
            idx2 = (((u8 *)gSeason->cup - (u8 *)gCups) * 0x38E38E39) >> 2;
            if (idx2 != 4 || gEvent.finishOrder[0] == 0) {
                pb2 = (u8 *)gSeason->cup;
                e = pb2 + (gGame.difficulty - 1);
                if (gSeason->round == e[1]) {
                    nv = gSeason->round + 1;
                    if (nv < *pb2) {
                        e[1] = nv;
                    } else if (gSeason->standings[0] == 0) {
                        e[1] = nv;
                    }
                }
            }
            save_write(1, 0);
        }
        f = save_update_unlocks();
        gGame.newUnlocks = f;
        if (f != 0) {
            gGame.flags |= 0x200000;
        }
    }
}

#define LD(x) ((void *)res_load(gGuiRes, x))


void shell_load_screen(Screen *screen) {
    s32 i;
    s32 j;
    s32 c;

    snd_drain();
    if (screen == NULL) {
        return;
    }
    gGame.idleFrames = 0;
    gGame.dialogFrame = LD(sDialogFrameResId);
    gGame.dialogTitleBar = LD(sDialogTitleBarResId);
    if ((screen->flags & SCREEN_TEXT_OBJECTS) && screen->left == NULL) {
        for (i = 0; i < 48; i++) {
            gGame.texts[i] = font_text_create(0, 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
        }
    }
    if (screen->flags & SCREEN_PANELS) {
        gGame.frames = LD(sMenuFramesResId);
        gGame.itemBars = LD(sMenuItemBarsResId);
    }
    if (screen->bgResId != NULL) {
        if (screen->type == SCREEN_TYPE_ROUND_SELECT) {
            for (i = 0; i < 7; i++) {
                gGame.screenPics[i] = LD(*((s32 **)screen->bgResId)[i]);
            }
        } else {
            screen->bg = LD(*screen->bgResId);
        }
    } else {
        screen->bg = NULL;
    }
    osViSetSpecialFeatures((screen->flags & SCREEN_DITHER_FILTER) ? 0x6A : 0xAA);

    switch (screen->type) {
    case SCREEN_TYPE_SPLASH_3:
        gGame.flags |= 0x10000000;
    case SCREEN_TYPE_SPLASH:
        gGame.screenTimer = 0xB4;
        break;
    case SCREEN_TYPE_PAK_STATUS:
        gGame.frames = LD(sPakStatusFrameResId);
        gGame.itemBars = LD(sPakStatusTitleBarResId);
        gGame.screenTimer = 0x4B0;
        break;
    case SCREEN_TYPE_BACKUP_LOST:
        gGame.frames = LD(gWarnFrameResId);
        gGame.itemBars = LD(gWarnTitleBarResId);
        gGame.screenTimer = 0x258;
        break;
    case SCREEN_TYPE_SPLASH_2:
        gGame.screenTimer = 0x12C;
        gGame.titleBar = LD(sExpansionPakDetectedResId);
        break;
    case SCREEN_TYPE_TITLE:
        for (i = 0; i < 6; i++) {
            gGame.savedRiders[i] = gGame.riders[i];
            gGame.savedRiderSkins[i] = gGame.riderSkins[i];
        }
        gGame.riders[0] = 4;
        gGame.riders[2] = 1;
        gGame.riders[3] = 2;
        gGame.riders[4] = 3;
        gGame.riders[1] = 0;
        gGame.riders[5] = 5;
        for (i = 0; i < 6; i++) {
            gGame.riderSkins[i] = i;
        }
        gGame.flags &= ~0x80;
        gGame.titleLogo = LD(sTitleLogoResId);
        gGame.frames = LD(sCopyrightResId);
        gCurTrack_2 = gTracks;
        gGame.mode = GAME_MODE_EXHIBITION;
        gGame.numPlayers = 1;
        race_set_grid_order(0, 6);
        race_init(0);
        break;
    case SCREEN_TYPE_OPTIONS:
        gGame.repeatTimer = 0;
        gGame.screenState = SCREEN_STATE_NORMAL;
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        gGame.optionsSfx = sfx_load_sample_id(sOptionsSfxSampleId);
        softmix_set_decompress_buf(0, 0);
        gGame.frames = LD(sOptionsFrameResId);
        gGame.cursorImg = LD(gCursorResId);
        gGame.cursorPal = LD(gCursorPalResId);
        gGame.titleBar = LD(sOptionsTitleBarResId);
        if (gCreditsMode != 0) {
            gGame.optionsSel = 6;
            gCreditsMode = 0;
        }
        gGame.optionsSel |= 0x100;
        break;
    case SCREEN_TYPE_CONTROLLER_SETTINGS:
        for (i = 1; i < 0x100; i++) {
            c = (i / 3) >> 3;
            gGame.greyPalette[i] = (c << 1) | (c << 6) | (c << 11) | 1;
        }
        gGame.padConfigSel = 0;
        gGame.titleBar = LD(sOptionsTitleBarResId);
        gGame.controllerDiagram = LD(sControllerDiagramResId);
        gGame.frames = LD(sControllerSettingsFrameResId);
        gGame.icons = LD(gControllerIconResId);
        shell_build_player_palettes();
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS:
        gGame.screenState = SCREEN_STATE_NORMAL;
        i = 0;
        if (screen->type == SCREEN_TYPE_CHEAT_ENTRY) {
            gGame.itemPics[0] = LD(sCheatPicResId);
            gGame.name[0] = 0x41;
            for (i = 1; i < 16; i++) {
                gGame.name[i] = 0;
            }
            gGame.nameLen = 0;
            gGame.keyCursorX = 0;
            gGame.keyCursorY = 0;
            gGame.keyIndex = 0;
        } else {
            gGame.entryRecord = -1;
            gGame.bestLapPlayer = 0xFF;
            for (i = 0; i < 4; i++) {
                gGame.recordPlayer[i] = -1;
                if (gEvent.record.initials[i][0] == 0xFF && gEvent.record.initials[i][1] == 0xFF) {
                    gGame.recordPlayer[i] = gEvent.record.initials[i][2];
                    if (gGame.entryRecord == -1) {
                        gGame.entryRecord = i;
                    }
                }
            }
            if (gGame.playerInitials[gGame.recordPlayer[gGame.entryRecord]][0] != 0) {
                __builtin_memcpy(gGame.name, gGame.playerInitials[gGame.recordPlayer[gGame.entryRecord]], 3);
                gGame.name[3] = 0;
                gGame.nameLen = strlen(gGame.name);
                gGame.keyCursorX = 0x31F;
                gGame.keyCursorY = 0x31F;
                gGame.keyIndex = 0x27;
            } else {
                gGame.name[0] = 0x41;
                gGame.name[1] = 0;
                gGame.name[2] = 0;
                gGame.name[3] = 0;
                gGame.nameLen = 0;
                gGame.keyCursorX = 0;
                gGame.keyCursorY = 0;
                gGame.keyIndex = 0;
            }
            for (i = 0; i < gGame.numRecordTracks; i++) {
                if (gGame.recordTracks[i] == gCurTrack_2 - gTracks) {
                    break;
                }
            }
            gGame.itemPics[0] = LD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        gGame.frames = LD(sTextEntryFramesResId);
        gGame.titleBar = LD(sTextEntryTitleBarResId);
        gGame.otherBg = LD(gMenuBackgroundResId);
        gGame.blinkTimer = 0;
        gGame.icons = LD(gControllerIconResId);
        shell_build_player_palettes();
        break;
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS:
        if (screen->type == SCREEN_TYPE_STANDINGS && gGame.mode == GAME_MODE_SEASON && gSeason != NULL
            && gSeason->cup - gCups == CUP_CHALLENGE) {
            shell_update_season_progress();
        }
        if (gGame.flags & 0x8000) {
            gGame.screenState = (screen->type == SCREEN_TYPE_RACE_RESULT) ? 3 : 4;
            gGame.flags = (gGame.flags | 0x20000) & ~0x8000;
        } else {
            gGame.screenState = SCREEN_STATE_NORMAL;
        }
        gGame.blinkTimer = 0;
        gGame.icons = LD(gControllerIconResId);
        shell_build_player_palettes();
        gGame.frames = LD(sResultsFramesResId);
        gGame.itemBars = LD(sMenuItemBarsResId);
        gGame.resultsTitleBars = LD(sResultsTitleBarsResId);
        gGame.cursorImg = LD(gCursorResId);
        gGame.cursorPal = LD(gCursorPalResId);
        break;
    case SCREEN_TYPE_SEASON_RACE_RESULT:
        shell_update_season_progress();
        gGame.screenState = SCREEN_STATE_NORMAL;
        gGame.blinkTimer = 0;
        shell_build_player_palettes();
        gGame.icons = LD(gControllerIconResId);
        gGame.frames = LD(sResultsFramesResId);
        gGame.cursorImg = LD(gCursorResId);
        gGame.cursorPal = LD(gCursorPalResId);
        gGame.resultsTitleBars = LD(sResultsTitleBarsResId);
        gGame.itemBars = LD(sMenuItemBarsResId);
        break;
    case SCREEN_TYPE_PAK:
        gPakScreen.validate = NULL;
        switch (gGame.pakScreenMode) {
        case 0:
            gPakScreen.mode = PAK_MODE_MANAGER;
            gPakScreen.data = NULL;
            gPakScreen.pages = 0;
            break;
        case 1:
            if (gActiveReplay == 0) {
                gActiveReplay = gReplayBuffer;
            }
            gPakScreen.pages = 100;
            gPakScreen.mode = PAK_MODE_LOAD;
            gPakScreen.validate = replay_validate;
            gPakScreen.data = (u8 *)(gActiveReplay + 0x6400);
            break;
        case 2:
            gPakScreen.pages = 40;
            gPakScreen.mode = PAK_MODE_SAVE;
            gPakScreen.data = (u8 *)gActiveReplay;
            break;
        case 3:
            if (gGame.userTrack != NULL) {
                mem_free(gGame.userTrack);
            }
            gGame.userTrack = mem_alloc_dbg(0x400, 0, sStrShellFile, 0x18E6);
            gPakScreen.data = gGame.userTrack;
            gPakScreen.pages = 4;
            gPakScreen.mode = PAK_MODE_LOAD;
            gPakScreen.validate = (PakValidateFunc)shell_pak_track_invalid;
            break;
        case 4:
            gPakScreen.pages = 4;
            gPakScreen.mode = PAK_MODE_SAVE;
            gPakScreen.data = gGame.userTrack;
            break;
        }
        gPakScreen.describe = shell_format_pak_file_label;
        pak_screen_init();
        break;
    case SCREEN_TYPE_LIST:
        gSplitVertical = 0;
        if (screen - sScreens == SCREEN_SPECIAL_TRACKS) {
            gGame.mode = GAME_MODE_EXHIBITION;
            while (gSpecialTracksItems[*screen->cursor].flags & SCREEN_ITEM_LOCKED) {
                (*screen->cursor)--;
            }
        }
        gGame.flags &= 0xFDFFFFFF;
        gGame.submenuFrame = LD(sSubmenuFrameResId);
        gGame.submenuItemBar = LD(sSubmenuItemBarResId);
        gGame.screenState = SCREEN_STATE_NORMAL;
        if (screen - sScreens == SCREEN_MAIN_MENU) {
            gGame.numPlayers = 1;
        }
        if (screen - sScreens == SCREEN_BRONZE_TRACKS) {
            gGame.trackSel = 0;
        }
    case SCREEN_TYPE_TRACK_LIST:
        gGame.smallArrows = LD(sSmallArrowsResId);
        gGame.menuAnimTimer = 0;
        gGame.blinkTimer = 0;
        gGame.screenArrows = LD(sScreenArrowsResId);
        gGame.sponsorLogos = LD(sSponsorLogosResId);
        gGame.titleBar = LD(gTitleBarResId);
        gGame.padlock = LD(sPadlockResId);
        gGame.icons = LD(sTrophyResId);
        gGame.wonFlag = LD(sWonFlagResId);
        gGame.cursorImg = LD(gCursorResId);
        gGame.cursorPal = LD(gCursorPalResId);
        for (i = 0; i < screen->numItems; i++) {
            if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                gGame.itemPics[i + screen->firstItem] = LD(*sTrackPicResIds[sTrackPicIndex[i + screen->firstItem]]);
            } else {
                gGame.itemPics[i + screen->firstItem] = LD(*sMenuItemPicResIds[i + screen->firstPic]);
            }
        }
        break;
    case SCREEN_TYPE_RECORDS:
        gGame.screenState = SCREEN_STATE_RECORDS_PICTURE;
        gGame.screenTimer = 6;
    case SCREEN_TYPE_ROUND_SELECT:
        gGame.seasonMenuState = 0;
        switch (gGame.difficulty) {
        case 1:
            if (gGame.cupSel >= 2) {
                gGame.cupSel = 0;
            }
            break;
        case 2:
            if (gGame.cupSel >= 3) {
                gGame.cupSel = 0;
            }
            break;
        }
        if (gGame.difficulty == 0) {
            gGame.cupSel = 6;
        } else if (gGame.cupSel == 6) {
            gGame.cupSel = 0;
        }
        gGame.roundSel = 0;
        gGame.menuAnimTimer = 0;
        gGame.blinkTimer = 0;
        gGame.screenArrows = LD(sScreenArrowsResId);
        gGame.cursorImg = LD(gCursorResId);
        gGame.cursorPal = LD(gCursorPalResId);
        gGame.titleBar = LD(gTitleBarResId);
        gGame.padlock = LD(sPadlockResId);
        gGame.icons = LD(sTrophyResId);
        gGame.wonFlag = LD(sWonFlagResId);
        gGame.customRoundArrows = LD(sCustomRoundArrowsResId);
        for (i = 0; i < 24; i++) {
            gGame.itemPics[i] = LD(*sTrackPicResIds[i]);
        }
        gGame.cupPreviewTimer = 0;
        gGame.cupPreviewTrack = 0;
        break;
    case SCREEN_TYPE_RIDER_SELECT:
        gGame.riderSelectState[0] = 0;
        gGame.riderSelectState[1] = 0;
        for (i = 3; i >= 0; i--) {
            gGame.riderSelectTimer[i] = 0;
        }
        for (i = 0; i < 6; i++) {
            gGame.bikeSkins[i] = i;
        }
        gGame.screenState = SCREEN_STATE_NORMAL;
        if (gGame.numPlayers == 1) {
            gGame.cButtonIcons = LD(sCButtonIconsResId);
        }
        j = 0;
        gGame.playerPad[1] = -1;
        gGame.playerPad[2] = -1;
        gGame.playerPad[3] = -1;
        gGame.padPlayer[0] = -1;
        gGame.savedMode = gGame.mode;
        gGame.savedTrack = gCurTrack_2;
        gGame.flags &= ~0x3C90;
        for (i = 0; i < 4; i++) {
            if ((gContMask >> i) & 1) {
                gGame.playerPad[j] = i;
                gGame.padPlayer[i] = j;
                j++;
            }
        }
        for (i = 0; i < 4; i++) {
            gGame.riderSelectRot[i][0] = (((gFrameCounter & 0x1FF) << 7) + ((-(i & 1) + (i & 2)) << 14)) & 0xFFFF;
            gGame.riderSelectRot[i][1] = 0;
        }
        gCurTrack_2 = &gTracks[TRACK_LONG_ISLAND];
        gGame.mode = GAME_MODE_EXHIBITION;
        gGame.flags |= 0x10;
        gGame.frames = LD(sRiderSelectFramesResId);
        gGame.itemBars = LD(sRiderSelectBarResId);
        gGame.smallArrows = LD(sSmallArrowsResId);
        race_set_grid_order(0, 6);
        race_init(0);
        gGame.hudGlyphs = (HudGlyphs *)res_load(gResourceSet, gHudGlyphsResId);
        break;
    }
}

void shell_goto_screen(Screen *screen, s32 linkBack, s32 runCallback)
{
    Screen *p;

    if (gGame.fadeState != 0) {
        if (sCurScreen == NULL) {
            if (screen != NULL) {
                p = screen;
                do {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->back = NULL;
                    }
                    shell_load_screen(p);
                    p = p->right;
                } while (p != NULL);
                for (p = screen->left; p != NULL; p = p->left) {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->back = NULL;
                    }
                    shell_load_screen(p);
                }
                if (tunes_is_song_playing() == 0 && (screen->flags & SCREEN_KEEP_MUSIC) == 0) {
                    tunes_play_song(sMenuSongId);
                }
            }
            gGame.fadeState = 1;
            gGame.fadeLevel = 1;
            gGame.fadeTimer = -1;
        } else {
            if (runCallback != 0) {
                ScreenItem *items = sCurScreen->items;
                s32 *cursor = sCurScreen->cursor;
                if ((cursor != NULL ? &items[*cursor] : items)->callback != NULL) {
                    (cursor != NULL ? &items[*cursor] : items)->callback(1);
                }
            }
            gGame.fadeState = 0;
            gGame.fadeLevel = 0xFF;
            sPrevScreenCursor = sCurScreen->cursor != NULL ? *sCurScreen->cursor : 0;
        }
        if (sCurScreen != NULL) {
            sPrevScreen = sCurScreen;
        }
        if (screen != NULL && linkBack != 0) {
            screen->back = sCurScreen;
        }
        gGame.returnScreen = sPrevScreen;
        sCurScreen = screen;
    }
}



void shell_draw_rider_stats(s32 x, s32 y, s32 p3, s32 p4)
{
    Game *g = &gGame;
    s32 mode = g->numPlayers;
    s32 yy, v, i, j, k, sy, t;
    Rect *q;
    u8 *base;

    if (mode == 1) {
        return;
    }
    yy = y;
    if (gGame.flags & GAME_FLAG_PLAYER_READY(p4)) {
        yy += 600;
    }
    base = gViewRects;
    q = (Rect *)(base + (p4 * 16 + (mode - 1) * 64));
    v = q->y + q->h + 5;
    y = yy;
    if (v < yy) {
        y = v;
    }
    font_text_create(gGame.texts[p4 * 9], 0x40, FONT_MAIN, TEXT_COLOR_CYAN, TEXT_CENTER);
    for (i = 1; i < 9; i++) {
        font_text_create(gGame.texts[i + p4 * 9], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
    }
    font_text_add(gGame.texts[p4 * 9], sRiderBios[p3].fullName);
    font_draw_text(gGame.texts[p4 * 9], x, y - 0x1F);
    i = 0;
    do {
        k = i + 1;
        font_text_add(gGame.texts[p4 * 9 + k], (char *)sRiderStatLabels[i]);
        t = i * 14 - 11;
        sy = yy + t;
        font_draw_text(gGame.texts[p4 * 9 + k], x - 0x46, sy);
        j = i + 5;
        font_text_add(gGame.texts[p4 * 9 + j], (char *)sRiderStatRatings[sRiderBios[p3].stats[i]]);
        font_draw_text(gGame.texts[p4 * 9 + j], x + 0x12, sy);
        i = k;
    } while (i < 4);
}

Gfx *shell_draw_results_menu(Gfx *gdl, s32 p2, s32 sel, char **strs)
{
    s32 flag;
    s32 i;
    u8 *img;
    s32 x, y, a, c;

    flag = 0;
    if (gGame.mode == GAME_MODE_TIME_TRIAL || gReplayWritePtr - gReplayBuf >= gReplayBufSize) {
        flag = 1;
    }
    font_reset_queue();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, gGame.itemBars->addr);
    i = 0;
    x = -25;
    y = -13;
    c = 0;
    for (; i < 3; i++) {
        gdl = render_draw_image(0xC0 - c, i * 25 + 0x52, &gGame.itemBars[1], gdl, p2, 1, 1, 3, 0);
        font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (sel == i) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, (i == 1 && flag) ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
        font_text_add(gGame.texts[i], strs[i]);
        font_draw_text(gGame.texts[i], gScreenWidth / 2 - y, gScreenHeight / 2 + x);
        x += 25;
        y += 7;
        c += 7;
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    img = (u8 *)&gGame.popupFrames[2];
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    return render_draw_image(0xA1 - (sel - 1) * 7, sel * 25 + 0x56,
                         &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

Gfx *shell_draw_players_difficulty_popup(Gfx *gdl, s32 p2, s32 p3)
{
    s32 dx, dy, dz;
    s32 i;
    u8 *img;
    char buf[32];
    char num[2];

    if (p3 != 1 || gContCount < 2 || gGame.mode != GAME_MODE_EXHIBITION) {
        dx = 0;
        if (p3 == -1) {
            dx = 7;
        }
    } else {
        dx = -0x18;
    }
    dy = 0;
    if (p3 == -1) {
        dy = -0xC;
    }
    dz = 0;
    if (p3 == 1 && gContCount >= 2 && gGame.mode == GAME_MODE_EXHIBITION) {
        dz = -0x17;
    }
    font_reset_queue();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, gGame.itemBars->addr);
    for (i = (p3 == -1); i < 4; i++) {
        if (p3 == 1 && gContCount >= 2 && gGame.mode == GAME_MODE_EXHIBITION) {
            if (i == p3) {
                img = (u8 *)&gGame.itemBars[2];
            } else {
                img = (u8 *)&gGame.itemBars[3];
            }
        } else {
            img = (u8 *)&gGame.itemBars[1];
        }
        gdl = render_draw_image(0xC0 - i * 7 + dx, i * 25 + 0x46 + dy, img, gdl, p2, 1, 1, 3, 0);
        if (p3 == 1) {
            if (i >= gContCount) {
                font_text_create(gGame.texts[i], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_CENTER);
                font_text_add(gGame.texts[i], sStrInsertController);
                font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            } else {
                if (i == p3 && gGame.mode == GAME_MODE_EXHIBITION) {
                    font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (gGame.numPlayers == 2) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    font_text_add(gGame.texts[i], sStrCpu);
                    font_draw_text(gGame.texts[i], gScreenWidth / 2 + (dx + 10) + 0x50 + dz,
                                  gScreenHeight / 2 + (dy - 14));
                    font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (gGame.numPlayers == 2) ? TEXT_COLOR_CYAN : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    int_to_str(gGame.twoPlayerCpuRiders, buf);
                    font_text_add(gGame.texts[i], buf);
                    font_draw_text(gGame.texts[i], gScreenWidth / 2 + (dx + 10) + 0x7A + dz,
                                  gScreenHeight / 2 + (dy - 14));
                }
                if (((gGame.mode == GAME_MODE_SOCCER || gGame.mode == 3) && !(i & 1)) ||
                    (gGame.mode == GAME_MODE_HILL_CLIMB && i >= 2) || (gGame.mode == GAME_MODE_STUNT_COURSE && i >= 2)) {
                    font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (i == gGame.numPlayers - 1) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_CENTER);
                } else {
                    font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (i == gGame.numPlayers - 1) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                }
                num[0] = i + '1';
                num[1] = 0;
                font_text_add(gGame.texts[i], num);
                font_text_add(gGame.texts[i], sStrPlayerSuffix);
                font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx + dz,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            }
        } else {
            font_text_create(gGame.texts[i], 0x40, FONT_MAIN, (i == gGame.difficulty) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY,
                          (i >= gSaveData.topDifficulty + 1) ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
            font_text_add(gGame.texts[i], sDifficultyNames[i]);
            font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                          gScreenHeight / 2 - 39 + i * 25 + dy);
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (p3 == -1) {
        img = (u8 *)&gGame.popupFrames[2];
    } else if (p3 == 1 && gContCount >= 2 && gGame.mode == GAME_MODE_EXHIBITION) {
        img = (u8 *)&gGame.popupFrames[1];
    } else {
        img = (u8 *)&gGame.frames[3];
    }
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    img = (u8 *)&gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1];
    if (p3 == 1) {
        gdl = render_draw_image(0xA7 - (gGame.numPlayers - 1) * 7 + dx, (gGame.numPlayers - 1) * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    } else {
        gdl = render_draw_image(0xA7 - gGame.difficulty * 7 + dx, gGame.difficulty * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    }
    if (p3 == 1 && gContCount >= 2 && gGame.mode == GAME_MODE_EXHIBITION && gGame.numPlayers == 2 && (gFrameCounter & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPLoadTLUT_pal256(gdl++, gGame.smallArrows->addr);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x60 + dz, gScreenHeight / 2 - 18 + dy, &gGame.smallArrows[3],
                            gdl, p2, 1, 1, 3, 0);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x84 + dz, gScreenHeight / 2 - 18 + dy, &gGame.smallArrows[4],
                            gdl, p2, 1, 1, 3, 0);
    }
    return font_render(gdl);
}

Gfx *shell_draw_yes_no_dialog(Gfx *gdl, s32 p2, char *str)
{
    u8 *img;
    s32 n;
    s32 y;
    s32 t;

    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPLoadTLUT_pal256(gdl++, gGame.dialogTitleBar->addr);
    gdl = render_draw_image(0xA3, 0x58, &gGame.dialogTitleBar[1], gdl, p2, 1, 1, 3, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    img = (u8 *)&gGame.dialogFrame[1];
    gdl = render_draw_image(0x100 - img[0] * 4, 0x78 - (img[1] >> 1), img, gdl, p2, 1, 1, 6, 0);
    font_reset_queue();
    font_text_create(gGame.texts[0], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
    font_text_add(gGame.texts[0], str);
    font_draw_text(gGame.texts[0], 0x105, 0x64);
    font_text_create(gGame.texts[0], 0x40, FONT_MAIN, gGame.dialogSel ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
    font_text_add(gGame.texts[0], sStrYes);
    font_draw_text(gGame.texts[0], 0x108, 0x76);
    font_text_create(gGame.texts[0], 0x40, FONT_MAIN, gGame.dialogSel ? TEXT_COLOR_LIGHT_GREY : TEXT_COLOR_YELLOW, TEXT_CENTER);
    font_text_add(gGame.texts[0], sStrNo);
    font_draw_text(gGame.texts[0], 0x103, 0x88);
    gdl = font_render(gdl);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    n = gGame.dialogSel;
    return render_draw_image(n * 7 + 0x9D, (t = n * 5, y = 0x82, y - t * 4),
                         &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

void shell_draw_rider_bio(s32 p, s32 q)
{
    s32 i, n, off;

    if (gGame.numPlayers != 1) {
        return;
    }
    font_text_create(gGame.texts[20], 0x40, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
    font_text_add(gGame.texts[20], sStrHoldZForBio);
    font_draw_text(gGame.texts[20], 0x140, 0x34);
    font_text_add(gGame.texts[p * 20 + 17], sRiderBios[gGame.riders[p]].text[0]);
    font_draw_text(gGame.texts[p * 20 + 17], gViewRects_12.x + 0x10E, q + gViewRects_12.y - 0x6F);
    font_text_add(gGame.texts[p * 20 + 18], sRiderBios[gGame.riders[p]].text[1]);
    font_draw_text(gGame.texts[p * 20 + 18], gViewRects_12.x + 0x10B, q + gViewRects_12.y - 0x62);
    font_text_add(gGame.texts[p * 20 + 19], sRiderBios[gGame.riders[p]].text[2]);
    font_draw_text(gGame.texts[p * 20 + 19], gViewRects_12.x + 0x108, q + gViewRects_12.y - 0x55);
    for (i = 0; i < 4; i++) {
        font_text_add(gGame.texts[p * 20 + 6 + i], sRiderBios[gGame.riders[p]].text[i + 4]);
        font_draw_text(gGame.texts[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y - 0x6F + i * 13);
    }
    n = 4;
    while (sRiderBios[gGame.riders[p]].text[n + 4] != 0) {
        n++;
    }
    off = -100;
    for (; sRiderBios[gGame.riders[p]].text[i + 4] != 0; i++) {
        font_text_add(gGame.texts[p * 20 + 6 + i], sRiderBios[gGame.riders[p]].text[i + 4]);
        font_draw_text(gGame.texts[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y + off + i * 13);
    }
}

#define SAVE gSaveData_13
#define VP gViewRects
#define CUSTOM5 (gGame.mode == GAME_MODE_SEASON && gSeason != NULL && gSeason->cup - gCups == CUP_CUSTOM)
#define CHAMP4 (gGame.mode == GAME_MODE_SEASON && gSeason != NULL && gSeason->cup - gCups == CUP_CHALLENGE)
#define UNLOCKED(n) (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && ((n) == 0 || ((SAVE.b2BF >> ((n) - 1)) & 1))))
#define LVLDONE (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && (SAVE.b2BF & 8)))
#define AVAILD(n) ((gGame.mode == GAME_MODE_TIME_TRIAL) ? avail8d(n) : shell_is_round_unlocked((n) / 5))
#define AVAIL(n) ((gGame.mode == GAME_MODE_TIME_TRIAL) ? avail8(n) : shell_is_round_unlocked((n) / 5))

#define PREAMBLE()                                                  \
    gDPPipeSync(gdl++);                                             \
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);                           \
    gDPSetTexturePersp(gdl++, G_TP_NONE);                           \
    gDPSetTextureFilter(gdl++, G_TF_BILERP);                        \
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);    \
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);                           \
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);                         \
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA)

#define DRAW(x, y, img, f) gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, (f), 0)
#define PSYNCK(k) { Gfx *_g = gdl++; _g->words.w0 = 0xE7000000; _g->words.w1 = (u16)(k) >> 16; }
#define DRAWK(x, y, img, f, k) gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, (f), (u16)(k) >> 16)
#define DRAWV(x, y, img, f) { s32 _f = (f); gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, _f, 0); }
#define TDRAW(spr, x, y, f) { Img *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->addr); DRAW((x), (y), _t, (f)); }
#define TDRAWP(spr, x, y, f) { Img *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->addr); gDPPipeSync(gdl++); DRAW((x), (y), _t, (f)); }

typedef struct {
    char name[0x1E];
    s16 h1E;
    u8 pad20[0x34 - 0x20];
    s32 f34[3];
    s32 f40;
    char init[3][3];
    char init4D[3];
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Track_17; /* 0xA0 */

typedef struct {
    u8 pad0[0x2BF];
    u8 b2BF;
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
    u8 vol[3];
} Save_5;

extern Track_17 *gCurTrack_41;
extern Save_5 gSaveData_13;
extern Track_17 gTracks_16[];

static inline s32 avail8(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    switch (gSaveData_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = gSaveData_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

static inline s32 avail8d(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    if (k == 9) {
        v = 1;
        n = 2;
    }
    switch (gSaveData_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = gSaveData_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

static inline s32 objOk(Screen *p)
{
    s32 lvl;
    s32 v;

    if (gGame.mode == GAME_MODE_TIME_TRIAL || gGame.mode == GAME_MODE_EXHIBITION) {
        lvl = gSaveData_13.b2C0;
    } else {
        lvl = gGame.difficulty;
    }
    switch (lvl) {
    case 1:
        v = p->firstItem < 10;
        break;
    case 2:
        v = p->firstItem < 15;
        break;
    case 3:
        goto ret1;
    default:
        goto ret0;
    }
    if (!v) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

Gfx *shell_draw_screen(Mtx *m, s32 fb, Gfx *gdl, ScreenItem *item, s32 e)
{
    char unk[0xE8];
    char buf[0x40];
    s16 wid[4];
    char buf2[0x10];
    char buf3[0x20];
    Screen *s;
    s32 k;
    s32 rowh;
    s32 i;
    Track_17 *trk;
    s32 yoff3;
    s32 ry;
    s32 rx;
    s32 w;
    s32 w2;
    s32 w3;
    s32 u1;
    s32 u2;
    s32 u3;
    s32 r;
    s32 j;
    s32 col;
    s32 x;
    s32 y;
    s32 yo2;
    s32 n;
    s32 c;
    s32 tmp;
    s32 pl;
    s32 gr;
    s32 gc;
    s32 t;
    s32 a;
    void *p;
    Img *img;

    if (gGame.fadeState == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    switch (gGame.fadeState) {
    case 0:
        w = gScreenWidth; w2 = gScreenWidth;
        break;
    case 1:
        w3 = gScreenWidth;
        break;
    case 2:
        w3 = gScreenWidth;
        break;
    }
    gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    switch (s->type) {
    case SCREEN_TYPE_SPLASH_2:
        PREAMBLE();
        if (gHasExpansionPak_2 != 0 && (gFrameCounter & 0x20)) {
            Img *t = &gGame.titleBar[1];

            gDPLoadTLUT_pal256(gdl++, gGame.titleBar->addr);
            gdl = render_draw_image(0x77, 0x8D, t, gdl, fb, 1, 1, 3, 0);
        }
        break;
    case SCREEN_TYPE_PAK:
        if (gGame.fadeState != 1) {
            pak_screen_draw(m, fb, &gdl);
        }
        break;
    case SCREEN_TYPE_LIST:
    case SCREEN_TYPE_TRACK_LIST:
        yoff3 = 0;
        if (s->type == SCREEN_TYPE_TRACK_LIST) {
            yoff3 = 12;
        }
        font_reset_queue();
        PREAMBLE();
        if (s->type == SCREEN_TYPE_LIST) {
            s32 w;
            s32 r;
            Img *sp;

            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x32);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            sp = &gGame.sponsorLogos[1];
            w = sp[gGame.sponsorLogo].w << 3;
            r = gFrameCounter % (w + 0x200);
            if (r == 0) {
                gGame.sponsorLogo = (s16)(gGame.sponsorLogo + 1) % 10;
            }
            gdl = render_draw_image(r - w, ((0xF0 - (w + 0x200) / 2) >> 1) + r / 2, &gGame.sponsorLogos[gGame.sponsorLogo + 1], gdl, fb, 1, 1,
                                6, 0);
        } else if (gGame.blinkTimer < 0x20) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPLoadTLUT_pal256(gdl++, gGame.screenArrows->addr);
            if (s->left != NULL) {
                gdl = render_draw_image(0x1C, yoff3 + 0x17, &gGame.screenArrows[1], gdl, fb, 1, 1, 3, 0);
            }
            if (s->right != NULL && objOk(s->right)) {
                gdl = render_draw_image(0x128, yoff3 + 0x17, &gGame.screenArrows[2], gdl, fb, 1, 1, 3, 0);
            }
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        TDRAWP(gGame.titleBar, 0x45, yoff3 + 0x1F, 3);
        if ((s->type == SCREEN_TYPE_TRACK_LIST && !AVAILD(s->firstItem + *s->cursor)) || (s->items[*s->cursor + s->firstItem].flags & SCREEN_ITEM_LOCKED)) {
            gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
            font_text_create(gGame.texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrLocked);
            font_draw_text(gGame.texts[0], 0x16B, 0x82);
        } else {
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPLoadTLUT_pal256(gdl++, gGame.itemPics[s->firstItem + *s->cursor]->addr);
        }
        img = &gGame.itemPics[s->firstItem + *s->cursor][1];
        if (s->type != SCREEN_TYPE_TRACK_LIST) {
            gdl = render_draw_image(0x102, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        } else {
            gdl = render_draw_image(0x108, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        if (s->type == SCREEN_TYPE_LIST) {
            img = &gGame.frames[2];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        } else {
            img = &gGame.frames[6];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        font_text_create(gGame.texts[s->numItems * 2], 0x20, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        font_text_add(gGame.texts[s->numItems * 2], s->title);
        font_draw_text(gGame.texts[s->numItems * 2], 0xB9, yoff3 + 0x28);
        for (i = 0; i < s->numItems; i++) {
            if ((s->type == SCREEN_TYPE_TRACK_LIST && !AVAIL(s->firstItem + i)) || (s->items[i + s->firstItem].flags & SCREEN_ITEM_LOCKED)) {
                font_text_create(gGame.texts[i], 0x20, FONT_MAIN, (i == *s->cursor) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_LEFT);
                font_text_create(gGame.texts[i + s->numItems], 0x20, FONT_MAIN, (i == *s->cursor) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_CENTER);
            } else {
                font_text_create(gGame.texts[i], 0x20, FONT_MAIN, (i == *s->cursor) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                font_text_create(gGame.texts[i + s->numItems], 0x20, FONT_MAIN, (i == *s->cursor) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            }
            if (s->type == SCREEN_TYPE_TRACK_LIST && !AVAIL(gGame.trackSel)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
                gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                gDPLoadTLUT_pal256(gdl++, gGame.padlock->addr);
                gdl = render_draw_image(0x44 - i * 7, 0x3E + i * 25 + yoff3, &gGame.padlock[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], gdl,
                                    fb, 1, 1, 3, 0);
            } else if (i == *s->cursor) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
                gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                gdl = render_draw_image(0x45 - i * 7, 0x3D + i * 25 + yoff3, &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, fb, 1,
                                    1, 3, 0);
            }
            if (s->type == SCREEN_TYPE_TRACK_LIST) {
                font_text_add(gGame.texts[i], gTracks_16[s->firstItem + i].name);
                int_to_str(gTracks_16[s->firstItem + i].h1E, buf);
                font_text_add(gGame.texts[i + s->numItems], buf);
                font_draw_text(gGame.texts[i + s->numItems], 0x112 - i * 7, 0x46 + i * 25 + yoff3);
            } else {
                font_text_add(gGame.texts[i], s->items[i].text);
            }
            font_draw_text(gGame.texts[i], 0x6E - i * 7, 0x46 + i * 25 + yoff3);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        if (gGame.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        } else if (gGame.screenState >= SCREEN_STATE_ASK_GHOST && gGame.screenState <= SCREEN_STATE_ASK_TRACK_SOURCE) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
            gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
            gDPPipeSync(gdl++);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPLoadTLUT_pal256(gdl++, gGame.submenuItemBar->addr);
            img = &gGame.submenuItemBar[1];
            gdl = render_draw_image(0x9B, 0x5C, img, gdl, fb, 1, 1, 3, 0);
            gdl = render_draw_image(0x94, 0x75, img, gdl, fb, 1, 1, 3, 0);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            img = &gGame.submenuFrame[1];
            gdl = render_draw_image(0x104 - img->w * 4, 0x78 - img->h / 2, img, gdl, fb, 1, 1, 6, 0);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
            gdl = render_draw_image(0x81 - gGame.dialogSel * 7, gGame.dialogSel * 25 + 0x62, &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, fb, 1, 1,
                                3, 0);
            font_reset_queue();
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, (gGame.dialogSel == 0) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY,
                          (gGame.screenState == SCREEN_STATE_ASK_TRACK_SOURCE && !(gSaveData_13.b2C4 & 8)) ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
            switch (gGame.screenState) {
            case 9:
                font_text_add(gGame.texts[0], sStrRaceSavedTrack);
                break;
            case 8:
                font_text_add(gGame.texts[0], sStrContinue);
                break;
            case 10:
                font_text_add(gGame.texts[0], sStrLoadFromGamePak);
                break;
            }
            font_draw_text(gGame.texts[0], 0x113, 0x6A);
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, (gGame.dialogSel == 1) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            switch (gGame.screenState) {
            case 9:
                font_text_add(gGame.texts[0], sStrGoToEditor);
                break;
            case 8:
                font_text_add(gGame.texts[0], sStrLoadSavedGhost);
                break;
            case 10:
                font_text_add(gGame.texts[0], sStrLoadFromControllerPak);
                break;
            }
            font_draw_text(gGame.texts[0], 0x10C, 0x83);
            gdl = font_render(gdl);
        } else if (gGame.screenState == SCREEN_STATE_DIFFICULTY || gGame.screenState == SCREEN_STATE_USER_TRACK_DIFFICULTY) {
            if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || gGame.screenState == SCREEN_STATE_USER_TRACK_DIFFICULTY) {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, -1);
            } else {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, 0);
            }
        }
        break;
    case SCREEN_TYPE_ROUND_SELECT:
    case SCREEN_TYPE_RECORDS: {
        s32 xoff;
        s32 yoff;
        s32 u4;

        if (s->type == SCREEN_TYPE_RECORDS) {
            xoff = 0;
            yoff = 0;
        } else {
            xoff = 0;
            yoff = -11;
        }
        PREAMBLE();
        font_reset_queue();
        gDPLoadTLUT_pal256(gdl++, gGame.screenArrows->addr);
        {
        if (gGame.blinkTimer < 0x20 && (s->type == SCREEN_TYPE_RECORDS || gGame.seasonMenuState == 0)) {
            if ((s->type == SCREEN_TYPE_RECORDS && *s->cursor / 5 < (gGame.numRecordTracks + 4) / 5 - 1) ||
                (s->type == SCREEN_TYPE_ROUND_SELECT &&
                 ((gGame.difficulty == 1 && *s->cursor < 1) || (gGame.difficulty == 2 && *s->cursor < 2) || (gGame.difficulty == 3 && *s->cursor < 5)))) {
                DRAW(xoff + 0x128, yoff + 0x22, &gGame.screenArrows[2], 3);
            }
            if (((s->type != SCREEN_TYPE_RECORDS) ? *s->cursor : *s->cursor / 5) > 0) {
                DRAW(xoff + 0x1C, yoff + 0x22, &gGame.screenArrows[1], 3);
            }
        }
        }
        TDRAWP(gGame.titleBar, xoff + 0x49, yoff + 0x2A, 3);
        {
        if (gCups[*s->cursor / 5].numRounds != 0) {
            s32 a;
            void *p;

            if (s->type == SCREEN_TYPE_RECORDS) {
                t = *s->cursor;
            } else {
                switch (gGame.seasonMenuState) {
                case 2:
                    t = gGame.customTrackSel;
                    break;
                case 1:
                    t = gCups[gGame.cupSel].tracks[gGame.roundSel];
                    break;
                default:
                    t = gCups[gGame.cupSel].tracks[gGame.cupPreviewTrack];
                    break;
                }
            }
            if (s->type == SCREEN_TYPE_RECORDS) {
                if (gGame.screenState == SCREEN_STATE_RECORDS_PICTURE) {
                    shell_blend_palette_to_gray(0, gGame.itemPics[sTrackPicIndex[t]]->addr);
                } else {
                    shell_blend_palette_to_gray((0x3B - gGame.screenTimer) * 0xFF / 0x3B, gGame.itemPics[sTrackPicIndex[t]]->addr);
                }
            } else {
                shell_blend_palette_to_gray(0xFF, NULL);
            }
            img = &gGame.itemPics[sTrackPicIndex[t]][1];
            if (s->type != SCREEN_TYPE_RECORDS &&
                !((CUSTOM5 || ((gGame.seasonMenuState != 0 || UNLOCKED(*s->cursor)) && gGame.seasonMenuState != 3 &&
                              (gGame.seasonMenuState != 2 || (gUnlockedTracks & (1 << gGame.customTrackSel))))) &&
                 (!CUSTOM5 || LVLDONE))) {
                if (gGame.seasonMenuState == 0 || gGame.seasonMenuState == 3 || s->type == SCREEN_TYPE_RECORDS) {
                    font_text_create(gGame.texts[20], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    font_text_add(gGame.texts[20], sStrLocked);
                    font_draw_text(gGame.texts[20], xoff + 0x16F, yoff + 0x82);
                } else {
                    font_text_create(gGame.texts[20], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    font_text_add(gGame.texts[20], sStrUnavailable);
                    font_draw_text(gGame.texts[20], xoff + 0x172, yoff + 0x74);
                    font_text_create(gGame.texts[21], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    font_text_add(gGame.texts[21], sStrPlayOtherRounds);
                    font_draw_text(gGame.texts[21], xoff + 0x16F, yoff + 0x82);
                    font_text_create(gGame.texts[22], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    font_text_add(gGame.texts[22], sStrToUnlockThisTrack);
                    font_draw_text(gGame.texts[22], xoff + 0x16E, yoff + 0x90);
                }
                gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
                gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
                gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            } else {
                if (s->type == SCREEN_TYPE_RECORDS) {
                    if (gGame.screenState == SCREEN_STATE_RECORDS_PICTURE) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B, 0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B,
                                        0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B, 0xFF);
                    }
                    gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
                    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                } else {
                    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                    gDPLoadTLUT_pal256(gdl++, gGame.itemPics[sTrackPicIndex[t]]->addr);
                }
            }
            DRAW(xoff + 0x10C, yoff + 0x47, img, 3);
        }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        img = &gGame.frames[(s->type == SCREEN_TYPE_RECORDS) ? 6 : 7];
        DRAW(((0x200 - img->w * 8) >> 1) + 4, (0xF0 - img->h) / 2, img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        for (i = 0; i < 5; i++) {
            if (s->type == SCREEN_TYPE_RECORDS || ((gGame.seasonMenuState == 0 || gGame.seasonMenuState == 3) && i < gCups[*s->cursor].numRounds) ||
                (gGame.seasonMenuState != 0 && i <= gCups[*s->cursor].numRounds)) {
                if (s->type == SCREEN_TYPE_RECORDS) {
                    if (i == *s->cursor % 5) {
                        gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.cursorImg[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_LEFT);
                        font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_RIGHT);
                    } else {
                        font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                        font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                    }
                } else if (gGame.seasonMenuState != 0 && gGame.seasonMenuState != 3) {
                    if (i == gGame.roundSel) {
                        if (gGame.seasonMenuState == 1) {
                            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                        } else {
                            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 0x204);
                        }
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.cursorImg[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_LEFT);
                        font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_RIGHT);
                    } else {
                        font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                        font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                    }
                } else {
                    if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                        gDPLoadTLUT_pal256(gdl++, gGame.padlock->addr);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.padlock[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                    } else if (((gCups[*s->cursor].won[gGame.difficulty - 1] >> i) & 1) && !CUSTOM5) {
                        gDPLoadTLUT_pal256(gdl++, gGame.wonFlag->addr);
                        DRAW(0x49 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.wonFlag[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                    }
                    if (i == gGame.cupPreviewTrack) {
                        if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_GREYED | TEXT_LEFT);
                            font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_GREYED | TEXT_RIGHT);
                        } else {
                            font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_LEFT);
                            font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_RIGHT);
                        }
                    } else {
                        if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_LEFT);
                            font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_RIGHT);
                        } else {
                            font_text_create(gGame.texts[i], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                            font_text_create(gGame.texts[i + 5], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                        }
                    }
                }
                if (s->type == SCREEN_TYPE_RECORDS || i < gCups[*s->cursor].numRounds) {
                    if ((s->type != SCREEN_TYPE_RECORDS && gGame.seasonMenuState != 2) || (s->type == SCREEN_TYPE_RECORDS && (*s->cursor / 5) * 5 + i < gGame.numRecordTracks)) {
                        if (s->type == SCREEN_TYPE_RECORDS) {
                            switch ((*s->cursor / 5) * 5 + i) {
                            case 0x15:
                            case 0x16:
                            case 0x17:
                                strcpy(buf2, "-");
                                break;
                            default:
                                int_to_str(gTracks_16[gGame.recordTracks[(*s->cursor / 5) * 5 + i]].h1E, buf2);
                                break;
                            }
                        } else if (*s->cursor == 5) {
                            int_to_str(gCups[CUP_CUSTOM].laps[i], buf2);
                        } else if (*s->cursor == 4) {
                            int_to_str(1, buf2);
                        } else {
                            int_to_str(gTracks_16[gCups[*s->cursor].tracks[i]].h1E, buf2);
                        }
                        font_text_add(gGame.texts[i + 5], buf2);
                        font_draw_text(gGame.texts[i + 5], 0x11A - i * 7 + xoff, 0x52 + i * 25 + yoff);
                    }
                    if (s->type == SCREEN_TYPE_RECORDS) {
                        if ((*s->cursor / 5) * 5 + i < gGame.numRecordTracks) {
                            font_text_add(gGame.texts[i], gTracks_16[gGame.recordTracks[(*s->cursor / 5) * 5 + i]].name);
                        }
                    } else {
                        font_text_add(gGame.texts[i], gTracks_16[gCups[*s->cursor].tracks[i]].name);
                    }
                    font_draw_text(gGame.texts[i], 0x6F - i * 7 + xoff, 0x52 + i * 25 + yoff);
                } else {
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    if (*s->cursor == 5) {
                        img = &gGame.frames[5];
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
                    if (gGame.seasonMenuState != 1) {
                        img = &gGame.frames[4];
                        DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                    }
                }
            } else {
                gDPSetTextureLUT(gdl++, G_TT_NONE);
                gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                img = &gGame.frames[4];
                gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xF0);
                DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                if (*s->cursor == 5) {
                    img = &gGame.frames[5];
                }
                DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
            }
        }
        if (s->type == SCREEN_TYPE_RECORDS) {
            strcpy(buf, "Records");
        } else if (*s->cursor == 4) {
            strcpy(buf, "Challenge");
        } else if (*s->cursor == 5) {
            if (gGame.seasonMenuState == 2 && (gFrameCounter & 0x20)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                gDPLoadTLUT_pal256(gdl++, gGame.customRoundArrows->addr);
                DRAW(xoff + 0x16E, yoff + 0x34, &gGame.customRoundArrows[2], 3);
                DRAW(xoff + 0x149, yoff + 0xBE, &gGame.customRoundArrows[2], 0x23);
                DRAW(xoff + 0xFE, yoff + 0x79, &gGame.customRoundArrows[1], 0x43);
                DRAW(xoff + 0x1C1, yoff + 0x79, &gGame.customRoundArrows[1], 3);
            }
            switch (gGame.seasonMenuState) {
            case 0:
                strcpy(buf, "Custom");
                if (LVLDONE) {
                    for (i = 0; i < 3; i++) {
                        font_text_create(gGame.texts[i + 12], 0x40, FONT_MAIN, (i == gGame.roundSel) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                    }
                    if (gCups[*s->cursor].numRounds == 0) {
                        font_text_add(gGame.texts[12], sStrCreate);
                        font_draw_text(gGame.texts[12], xoff + 0x16E, yoff + 0x80);
                    } else {
                        font_text_add(gGame.texts[12], sStrPlay);
                        font_draw_text(gGame.texts[12], xoff + 0x16E, yoff + 0x79);
                        font_text_add(gGame.texts[13], sStrEdit);
                        font_draw_text(gGame.texts[13], xoff + 0x16E, yoff + 0x87);
                    }
                }
                break;
            case 1:
                strcpy(buf, "Select Slot");
                font_text_create(gGame.texts[12], 0x40, FONT_MENU, TEXT_COLOR_YELLOW, TEXT_LEFT);
                if (gGame.roundSel == gCups[*s->cursor].numRounds) {
                    font_text_add(gGame.texts[12], sStrPressAToAddTrack);
                } else {
                    font_text_add(gGame.texts[12], sStrChangeOrDeleteTrack);
                }
                font_draw_text(gGame.texts[12], 0x2D, yoff + 0xCF);
                break;
            case 2:
                strcpy(buf, "Choose Track");
                font_text_create(gGame.texts[12], 0x40, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_LEFT);
                switch ((s16)(gGame.customTrackSel / 5)) {
                case 0:
                    font_text_add(gGame.texts[12], sStrBronzeRoundTracks);
                    break;
                case 1:
                    font_text_add(gGame.texts[12], sStrSilverRoundTracks);
                    break;
                case 2:
                    font_text_add(gGame.texts[12], sStrGoldRoundTracks);
                    break;
                case 3:
                    font_text_add(gGame.texts[12], sStrPlatinumRoundTracks);
                    break;
                }
                font_draw_text(gGame.texts[12], 0x2D, yoff + 0xCF);
                break;
            }
        } else {
            switch (*s->cursor) {
            case 0:
                strcpy(buf, "Bronze Round");
                break;
            case 1:
                strcpy(buf, "Silver Round");
                break;
            case 2:
                strcpy(buf, "Gold Round");
                break;
            case 3:
                strcpy(buf, "Platinum Round");
                break;
            }
        }
        font_text_create(gGame.texts[11], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        font_text_add(gGame.texts[11], buf);
        font_draw_text(gGame.texts[11], xoff + 0xBD, yoff + 0x34);
        if (s->type != SCREEN_TYPE_RECORDS && gGame.seasonMenuState == 0) {
            font_text_create(gGame.texts[10], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
            if (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && ((SAVE.b2BF >> *s->cursor) & 1))) {
                font_text_add(gGame.texts[10], sStrRoundComplete);
            } else if ((SAVE.b2C0 > gGame.difficulty || (SAVE.b2C0 == gGame.difficulty && (*s->cursor == 0 || ((SAVE.b2BF >> (*s->cursor - 1)) & 1)))) || (CUSTOM5 && SAVE.b2C0 == gGame.difficulty && (SAVE.b2BF & 8))) {
                s32 fl;

                fl = 0;
                switch (gGame.difficulty) {
                case 1:
                    if (*s->cursor == 1) {
                        fl = 1;
                    }
                    break;
                case 2:
                    if (*s->cursor == 2) {
                        fl = 1;
                    }
                    break;
                case 3:
                    if (*s->cursor == 4) {
                        fl = 1;
                    }
                    break;
                }
                if (fl) {
                    if (gGame.difficulty == 3) {
                        font_text_add(gGame.texts[10], sStrFinishFirstEveryRace);
                    } else {
                        font_text_add(gGame.texts[10], sStrFinishFirstNewDifficulty);
                    }
                } else if (*s->cursor == 5) {
                    if (gCups[CUP_CUSTOM].numRounds != 0) {
                        if (gGame.roundSel == 0) {
                            font_text_add(gGame.texts[10], sStrPressAToStart);
                        } else {
                            font_text_add(gGame.texts[10], sStrPressAToEditSeason);
                        }
                    } else {
                        font_text_add(gGame.texts[10], sStrPressAToCreateRound);
                    }
                } else {
                    font_text_add(gGame.texts[10], sStrFinishFirstNextRound);
                }
            } else if (CUSTOM5) {
                font_text_add(gGame.texts[10], sStrCompletePlatinumToUnlock);
            } else {
                font_text_add(gGame.texts[10], sStrFinishFirstPrevRound);
            }
            font_draw_text(gGame.texts[10], xoff + 0x2D, yoff + 0xCF);
        }
        gdl = font_render(gdl);
        PSYNCK(u4);
        if (s->type == SCREEN_TYPE_RECORDS) {
            s32 dx, dy;

            trk = &gTracks_16[gGame.recordTracks[gGame.recordsSel]];

            if (gGame.screenState == SCREEN_STATE_RECORDS_PICTURE) {
                FONT_FADE_ALPHA = 0;
            } else {
                FONT_FADE_ALPHA = (0x3B - gGame.screenTimer) * 0xFF / 0x3B;
            }
            font_reset_queue();
            if (trk->f50 & 0x400) {
                font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_CENTER);
                font_text_add(gGame.texts[0], sStrBestScores);
                font_draw_text(gGame.texts[0], 0x175, 0x6D);
                for (j = 0; j < 3; j++) {
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_LEFT);
                    int_to_str(trk->f34[j], buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x134 - j * 4, 0x7E + j * 13);
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_RIGHT);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x1AC - j * 4, 0x7E + j * 13);
                }
            } else {
                if (gGame.recordsSel == 0x15 || gGame.recordsSel == 0x17) {
                    dx = -4;
                    dy = 0x14;
                } else {
                    dx = 0;
                    dy = 0;
                }
                font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_CENTER);
                font_text_add(gGame.texts[0], sStrBestTimes);
                font_draw_text(gGame.texts[0], dx + 0x17B, dy + 0x59);
                for (j = 0; j < 3; j++) {
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_LEFT);
                    event_format_time(trk->f34[j], buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], dx + (0x175 - j * 4) - 0x3C, (0x6A + j * 13) + dy);
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_RIGHT);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], dx + (0x175 - j * 4) + 0x3C, (0x6A + j * 13) + dy);
                }
                if (gGame.recordsSel != 0x15 && gGame.recordsSel != 0x17) {
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_CENTER);
                    font_text_add(gGame.texts[0], sStrBestLap);
                    font_draw_text(gGame.texts[0], 0x168, 0x9C);
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_LEFT);
                    event_format_time(trk->f40, buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x129, 0xAC);
                    font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_WHITE, TEXT_RIGHT);
                    __builtin_memcpy(buf, trk->init4D, 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x1A1, 0xAC);
                }
            }
            gdl = font_render(gdl);
        } else if (gGame.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        }
        break;
    }
    case SCREEN_TYPE_OPTIONS: {
        s32 u5;

        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        if (gGame.screenState == SCREEN_STATE_ERASING) {
            gGame.screenState = SCREEN_STATE_ERASE;
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        {
            Img *_t = &gGame.titleBar[1];

            gDPLoadTLUT_pal256(gdl++, gGame.titleBar->addr);
            gDPPipeSync(gdl++);
            DRAWK(0x4E, 0x1B, _t, 3, u5);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 29; k++) {
                x = 0x124 - j * 7 + k * 6;
                y = sOptionsRowY[j] - 1;
                if (*(SAVE.vol + j) * 29 / 255 >= k) {
                    if (j == gGame.optionsSel) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, k * 0xFF / 29, 0, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0x8C, k * 0x8C / 29, 0, 0xFF);
                    }
                } else {
                    gDPSetPrimColor(gdl++, 0, 0, 0x4B, 0x4B, 0x4B, 0xFF);
                }
                DRAW(x, y, &gGame.frames[1], 6);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &gGame.frames[2];
        DRAW(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
        img = &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1];
        for (j = 0; j < 11; j++) {
            if (j < 3) {
                x = 0x52 - j * 7;
            } else {
                x = 0x52 - ((j - 3) / 2 + 3) * 7 + ((j - 3) & 1) * 194;
            }
            y = sOptionsRowY[j];
            if (j == 9 || j == 10) {
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, (j == gGame.optionsSel) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_LEFT);
            } else {
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, (j == gGame.optionsSel) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY,
                              (gHasExpansionPak_2 == 0 && j == 7) ? TEXT_GREYED | TEXT_LEFT : TEXT_LEFT);
            }
            font_text_add(gGame.texts[0], gOptionsLabels[j]);
            font_draw_text(gGame.texts[0], x + 0x24 + ((u16)y >> 16), y + 8);
            if (j == gGame.optionsSel) {
                DRAW(x - 2, y, img, 3);
            }
            switch (j) {
            case 3:
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, (gGame.optionsSel == j) ? TEXT_COLOR_CYAN : TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                font_text_add(gGame.texts[0], gMonoStereoStrings[SAVE.b2C4 & 1]);
                font_draw_text(gGame.texts[0], x + 0xB4 + ((u16)x >> 16), y + 8);
                break;
            case 8:
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, (gGame.optionsSel == j) ? TEXT_COLOR_CYAN : TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                font_text_add(gGame.texts[0], sOffOnStrings[(SAVE.b2C4 & 0x10) == 0]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
                break;
            case 7:
                tmp = 2;
                if (gGame.optionsSel == j) {
                    tmp = 4;
                    if (gHasExpansionPak_2 != 0) {
                        tmp = 6;
                    }
                }
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, tmp, (gHasExpansionPak_2 == 0) ? TEXT_GREYED | TEXT_RIGHT : TEXT_RIGHT);
                font_text_add(gGame.texts[0], sOffOnStrings[(SAVE.b2C4 & 2) >> 1]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
                break;
            }
        }
        font_text_create(gGame.texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
        font_text_add(gGame.texts[0], sStrOptions);
        font_draw_text(gGame.texts[0], 100, 0x23);
        gdl = font_render(gdl);
        if (gGame.screenState == SCREEN_STATE_ERASE_DIALOG || gGame.screenState == SCREEN_STATE_ERASE || gGame.screenState == SCREEN_STATE_ERASING) {
            gdl = shell_draw_yes_no_dialog(gdl, fb, sStrEraseAllData);
        }
        break;
    }
    case SCREEN_TYPE_PAK_STATUS:
    case SCREEN_TYPE_BACKUP_LOST:
        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        img = &gGame.itemBars[1];
        gDPLoadTLUT_pal256(gdl++, gGame.itemBars->addr);
        if (s->type == SCREEN_TYPE_BACKUP_LOST) {
            DRAW(0x106 - img->w * 4, 0x41, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &gGame.frames[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(gGame.texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrWarning);
            font_draw_text(gGame.texts[0], 0x106, 0x4C);
            for (i = 0; sBackupLostMsg[i] != NULL; i++) {
                font_text_create(gGame.texts[0], 0x40, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_CENTER);
                str_copy(buf, sBackupLostMsg[i]);
                font_text_add(gGame.texts[0], buf);
                font_draw_text(gGame.texts[0], 0x100 - i * 4, i * 13 + 0x6D);
            }
        } else {
            s32 slot;
            s32 found = 0;
            s32 line = 0;
            s32 col;
            char **pp;
            s32 k2;

            DRAW(0x110 - img->w * 4, 0x1F, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &gGame.frames[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(gGame.texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrControllerPaks);
            font_draw_text(gGame.texts[0], 0x110, 0x28);
            font_text_create(gGame.texts[0], 0x40, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrTrademark);
            font_draw_text(gGame.texts[0], 400, 0x25);
            for (slot = 0; slot < 5; slot++) {
                if (slot == 4) {
                    if (found) {
                        break;
                    }
                    sprintf(buf3, sStrNoSuitablePaks);
                    line = 4;
                    gPakFreePages[4] = 0;
                    col = 8;
                    pp = sPakMissingMsg;
                } else if (gPakFreePages[slot] != -2) {
                    strcpy(buf3, "Controller Pak #X ");
                    found = 1;
                    if (gPakFreePages[slot] <= 0) {
                        str_cat(buf3, sStrIsFull);
                    } else {
                        str_cat(buf3, sStrHas);
                        int_to_str(gPakFreePages[slot], buf3 + strlen_2(buf3));
                        str_cat(buf3, sStrFreePage);
                        if (gPakFreePages[slot] != found) {
                            str_cat(buf3, sStrPluralS);
                        }
                        str_cat(buf3, sStrKeyDot);
                    }
                    buf3[16] = slot + '1';
                    if (gPakFreePages[slot] < 40) {
                        col = 8;
                        pp = sPakFewPagesMsg;
                    } else {
                        col = 7;
                        pp = sPakOkMsg;
                    }
                } else {
                    continue;
                }
                font_text_create(gGame.texts[0], 0x40, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                font_text_add(gGame.texts[0], buf3);
                font_draw_text(gGame.texts[0], 0x106 - line * 3, line * 11 + 0x46);
                line++;
                for (k2 = 0; pp[k2] != NULL; k2++) {
                    font_text_create(gGame.texts[0], 0x40, FONT_MENU, col, TEXT_CENTER);
                    font_text_add(gGame.texts[0], pp[k2]);
                    font_draw_text(gGame.texts[0], 0x106 - line * 3, line * 11 + 0x46);
                    line++;
                }
            }
        }
        gdl = font_render(gdl);
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS: {
        s32 u6;
        s32 plr;
        s32 cheat;

        font_reset_queue();
        cheat = (s->type == SCREEN_TYPE_CHEAT_ENTRY);
        if (!cheat) {
            plr = gGame.recordPlayer[gGame.entryRecord];
        }
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPLoadTLUT_pal256(gdl++, gGame.itemPics[0]->addr);
        DRAW(0x108, 0x47, &gGame.itemPics[0][1], 3);
        TDRAWP(gGame.titleBar, 0x45, 0x2A, 3);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &gGame.frames[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        img = &gGame.icons[gGame.blinkTimer / 2 % 60 + 1];
        if (gGame.numPlayers >= 2 && !cheat) {
            gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + plr * 0x100);
        } else {
            gDPLoadTLUT_pal256(gdl++, gGame.icons->addr);
        }
        DRAWK(0x18B, 0x2F, img, 3, u6);
        font_text_create(gGame.texts[0], 0x20, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
        if (cheat) {
            strcpy(buf, "Enter A Cheat Code");
            font_text_add(gGame.texts[0], buf);
            font_draw_text(gGame.texts[0], 0x55, 0x35);
        } else {
            strcpy(buf, "Player ");
            int_to_str(plr + 1, buf + strlen_2(buf));
            str_cat(buf, sStrInitialsSuffix);
            font_text_add(gGame.texts[0], buf);
            font_draw_text(gGame.texts[0], 0x55, 0x35);
        }
        if (cheat && gGame.screenState == SCREEN_STATE_CHEAT_ACCEPTED) {
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sCheatTable[gGame.cheatEntered].name);
            if (((1 << gGame.cheatEntered) == 4 && (gGame.flags & 0x20000000)) || ((sCheatMask >> gGame.cheatEntered) & 1)) {
                font_text_add(gGame.texts[0], sStrEnabledSuffix);
            } else {
                font_text_add(gGame.texts[0], sStrDisabledSuffix);
            }
            font_draw_text(gGame.texts[0], 0xAC, 0x71);
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrPressA);
            font_draw_text(gGame.texts[0], 0xA8, 0x7F);
        } else if (cheat && gGame.screenState == SCREEN_STATE_CHEAT_INVALID) {
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrNotAValidCode);
            font_draw_text(gGame.texts[0], 0xAC, 0x71);
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrPressA);
            font_draw_text(gGame.texts[0], 0xA8, 0x7F);
        } else {
            s32 xx;

            for (y = 0; y < 5; y++) {
                for (x = 0; x < 8; x++) {
                    xx = x * 26;
                    font_text_create(gGame.texts[0], 0x20, (y * 8 + x > 0x25) ? FONT_SMALL : FONT_MAIN, TEXT_COLOR_LIGHT_GREY,
                                  (gGame.nameLen == ((cheat) ? 10 : 3) && y * 8 + x < 0x26) ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
                    font_text_add(gGame.texts[0], gKeyboardKeys[y * 8 + x]);
                    font_draw_text(gGame.texts[0], xx + 0x54 - y * 4, y * 17 + 0x56);
                    if (y * 8 + x == gGame.keyIndex) {
                        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
                        gDPSetTextureLUT(gdl++, G_TT_NONE);
                        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                        img = &gGame.frames[3];
                        DRAW(xx + 0x54 - y * 4 - img->w * 4 + 4, y * 17 - (img->h >> 1) + 0x57, img, 6);
                    }
                }
            }
        }
        font_text_create(gGame.texts[0], 0x20, FONT_SMALL, (j == gGame.nameLen) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
        if (!cheat) {
            switch (gGame.entryRecord) {
            case 0:
            case 1:
            case 2:
                if (gGame.entryRecord != 0) {
                    buf[0] = gGame.entryRecord + '1';
                    buf[1] = gGame.entryRecord + '{';
                    buf[2] = ' ';
                    buf[3] = '\0';
                    font_text_add(gGame.texts[0], buf);
                }
                if (gCurTrack_41->f50 & 0x400) {
                    font_text_add(gGame.texts[0], sStrBestScore);
                } else {
                    font_text_add(gGame.texts[0], sStrBestTime);
                }
                if (gGame.recordPlayer[3] == plr || gGame.bestLapPlayer == plr) {
                    font_text_add(gGame.texts[0], sStrCommaBestLap);
                }
                break;
            case 3:
                font_text_add(gGame.texts[0], sStrBestLap);
                break;
            }
            font_draw_text(gGame.texts[0], 0x2F, 0xB6);
        }
        for (j = 0; (cheat) ? (j < 10) : (j < 3); j++) {
            font_text_create(gGame.texts[0], 0x20, FONT_MAIN, (j == gGame.nameLen) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            font_text_add(gGame.texts[0], sStrUnderscore);
            font_draw_text(gGame.texts[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE2), 0xB9);
            if (j < strlen_2(gGame.name)) {
                font_text_create(gGame.texts[0], 0x20, FONT_MAIN, (j == gGame.nameLen) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                buf[0] = gGame.name[j];
                buf[1] = '\0';
                font_text_add(gGame.texts[0], buf);
                font_draw_text(gGame.texts[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE3), 0xB3);
            }
        }
        gdl = font_render(gdl);
        break;
    }
    case SCREEN_TYPE_CONTROLLER_SETTINGS: {
        Game *gp = &gGame;

        font_reset_queue();
        {
            Gfx *_g = gdl++;

            _g->words.w0 = 0xE7000000 | ((u8)(u32)_g >> 8);
            _g->words.w1 = ((u16)(u32)_g >> 16) | (((u32)_g & 0x7F) >> 7);
        }
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_BILERP);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        img = &gp->controllerDiagram[1];
        gDPLoadTLUT_pal256(gdl++, gp->controllerDiagram->addr);
        DRAW(0xFE - img->w * 4, 0x74 - (img->h >> 1), img, 3);
        TDRAWP(gp->titleBar, 0x4A, 0x20, 3);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        font_text_create(gp->texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        strcpy(buf, "Controller ");
        int_to_str(gp->padConfigSel + 1, buf + strlen_2(buf));
        font_text_add(gp->texts[0], buf);
        font_draw_text(gp->texts[0], 0xBA, 0x29);
        font_text_create(gp->texts[0], 0x40, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        strcpy(buf, "Configuration ");
        buf[strlen_2(buf) + 1] = '\0';
        buf[strlen_2(buf)] = gp->padConfig[gp->padConfigSel] + 'A';
        font_text_add(gp->texts[0], buf);
        font_draw_text(gp->texts[0], 0xE9, 0xBF);
        for (j = 0; j < 9; j++) {
            tmp = 6;
            if (j & 1) {
                tmp = 4;
            }
            a = 0x20;
            if (j < 5) {
                a = 0x22;
            }
            if (!(j & 1)) {
                a |= 0x40000000;
            }
            font_text_create(gp->texts[0], 0x40, FONT_MENU, tmp, a);
            font_text_add(gp->texts[0], gControlActionLabels[gControlConfigs[(s8)*(gp->padConfig + gp->padConfigSel)][j]]);
            font_draw_text(gp->texts[0], gControlLabelPos[j].x, gControlLabelPos[j].y);
            if (gControlConfigs[(s8)*(gp->padConfig + gp->padConfigSel)][j] == 6) {
                tmp = 6;
                if (j & 1) {
                    tmp = 4;
                }
                a = 0x20;
                if (j < 5) {
                    a = 0x22;
                }
                if (!(j & 1)) {
                    a |= 0x40000000;
                }
                font_text_create(gp->texts[0], 0x40, FONT_MENU, tmp, a);
                font_text_add(gp->texts[0], gControlReverseLabel_2);
                font_draw_text(gp->texts[0], gControlLabelPos[j].x, gControlLabelPos[j].y + 11);
            }
        }
        for (j = 0; j < 3; j++) {
            font_text_create(gGame.texts[0], 0x40, FONT_MENU, (j == 2) ? TEXT_COLOR_YELLOW : TEXT_COLOR_CYAN, (j != 2) ? TEXT_HIGHLIGHT | TEXT_LEFT : TEXT_LEFT);
            font_text_add(gGame.texts[0], gCameraButtonLabels[j]);
            font_draw_text(gGame.texts[0], gCameraLabelPos[j].x, gCameraLabelPos[j].y);
        }
        for (j = 0; j < gGame.numPlayers; j++) {
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        img = &gGame.frames[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        for (j = 0; j < 4; j++) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            img = &gGame.icons[gGame.blinkTimer / 2 % 60 + 1];
            if (j == gGame.padConfigSel) {
                gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + j * 0x100);
            } else {
                gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
            }
            DRAW(0x132 + j * 0x2C, 0x24, img, 3);
        }
        gdl = font_render(gdl);
        break;
    }
    case SCREEN_TYPE_SEASON_RACE_RESULT:
#define YR (0x46 + j * rowh + ry)
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS: {
        s32 x;
        s32 y;
        s32 px;

        font_reset_queue();
        rx = 0;
        if (s->type == SCREEN_TYPE_STANDINGS) {
            if (CHAMP4) {
                rx = -8;
                ry = 0x1B;
            } else {
                ry = -7;
            }
        } else if (gGame.mode == GAME_MODE_TIME_TRIAL) {
            ry = 0x32;
            rx = -0xD;
        } else {
            ry = 0;
        }
        for (j = 0; j < 3; j++) {
            font_text_create(gGame.texts[j], 0x20, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
        }
        if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
            font_text_add(gGame.texts[0], sStrSeasonStandings);
            font_text_add(gGame.texts[1], sStrPts);
            font_draw_text(gGame.texts[1], 0x18F, ry + 0x28);
        } else {
            if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                font_text_add(gGame.texts[0], sStrTimeTrialResults);
            } else {
                font_text_add(gGame.texts[0], sStrRaceResults);
            }
            font_text_add(gGame.texts[2], sStrTimeHeader);
            if (s->type == SCREEN_TYPE_SEASON_RACE_RESULT) {
                font_text_add(gGame.texts[1], sStrPts);
                font_draw_text(gGame.texts[1], 0x147, ry + 0x28);
                font_draw_text(gGame.texts[2], 0x18D, ry + 0x28);
            } else if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
                font_draw_text(gGame.texts[2], 0x14D, ry + 0x2A);
            } else if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                font_draw_text(gGame.texts[2], 0x178, ry + 0x28);
            } else {
                font_draw_text(gGame.texts[2], 0x184, ry + 0x28);
            }
        }
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            font_draw_text(gGame.texts[0], 0x50, ry + 0x2A);
            font_text_create(gGame.texts[0], 0x20, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
            if (gEvent.finishOrder[0] == 0) {
                font_text_add(gGame.texts[0], sStrYouPlacedFirst);
            } else {
                font_text_add(gGame.texts[0], sStrYouPlacedSecond);
            }
            font_draw_text(gGame.texts[0], 0xEE, 0x97);
        } else {
            font_draw_text(gGame.texts[0], 0x55, ry + 0x28);
        }
        rowh = 0x19;
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            rowh = 0x17;
        }
        for (j = 0; j < gEvent.numRiders; j++) {
            if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
                r = gSeason->standings[j];
            } else {
                r = gEvent.finishOrder[j];
            }
            img = (Img *)((r < gGame.numPlayers || gGame.mode == GAME_MODE_TIME_TRIAL) ? ((gGame.mode == GAME_MODE_TIME_TRIAL) ? ((r == gTimeTrialPlayer) ? 4 : 2) : 4) : 2);
            font_text_create(gGame.texts[j * 4 + 5], 0x20, FONT_MAIN, (s32)img, TEXT_LEFT);
            pl = j;
            while (pl > 0 && s->type == SCREEN_TYPE_RACE_RESULT && gRiders[gEvent.finishOrder[pl]].race.finishTime == gRiders[gEvent.finishOrder[pl - 1]].race.finishTime) {
                s32 d0;

                pl--;
            }
            int_to_str(pl + 1, buf);
            font_text_add(gGame.texts[j * 4 + 5], buf);
            x = rx + 0x4E - j * 7;
            font_draw_text(gGame.texts[j * 4 + 5], (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) ? x - 2 : x, YR);
            str_copy(buf, sRiderBios[gGame.riders[r]].name);
            font_text_create(gGame.texts[j * 4 + 6], 0x20, FONT_MAIN, (s32)img, TEXT_LEFT);
            font_text_add(gGame.texts[j * 4 + 6], buf);
            font_draw_text(gGame.texts[j * 4 + 6], rx - (j * 7 - 0x6A), YR);
            if (r < 4) {
                wid[r] = font_get_char_pos(gGame.texts[j * 4 + 6], 0x6A - j * 7, YR, strlen_2(buf) - 1) >> 16;
            }
            if (s->type == SCREEN_TYPE_SEASON_RACE_RESULT && gRacePoints[j] != 0) {
                font_text_create(gGame.texts[j * 4 + 7], 0x20, FONT_MAIN, (s32)img, TEXT_LEFT);
                int_to_str(gRacePoints[j], buf + 1);
                buf[0] = '+';
                font_text_add(gGame.texts[j * 4 + 7], buf);
                font_draw_text(gGame.texts[j * 4 + 7], 0x14C - j * 7, YR);
            }
            font_text_create(gGame.texts[j * 4 + 7], 0x20, FONT_MAIN, (s32)img, TEXT_LEFT);
            if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
                int_to_str(gSeason->points[r], buf);
            } else {
                event_format_time(gRiders[r].race.finishTime, buf);
            }
            font_text_add(gGame.texts[j * 4 + 7], buf);
            switch (s->type) {
            case SCREEN_TYPE_SEASON_RACE_RESULT:
                font_draw_text(gGame.texts[j * 4 + 7], 0x186 - j * 7, YR);
                break;
            case SCREEN_TYPE_RACE_RESULT:
                if (gGame.mode == GAME_MODE_TIME_TRIAL) {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x170 - j * 7, YR);
                } else {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x17C - j * 7, YR);
                }
                break;
            case SCREEN_TYPE_STANDINGS:
                if (CHAMP4) {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x146 - j * 7, YR);
                } else {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x199 - j * 7, YR);
                }
                break;
            }
        }
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        TDRAW(gGame.resultsTitleBars, rx + 0x44, ry + 0x1F, 3);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            img = &gGame.frames[1];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else if (gGame.mode == GAME_MODE_TIME_TRIAL) {
            img = &gGame.frames[4];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else {
            img = &gGame.frames[(s->type == SCREEN_TYPE_SEASON_RACE_RESULT) ? 2 : 3];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2 + ry, img, 6);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        img = &gGame.icons[gGame.blinkTimer / 2 % 60 + 1];
        for (j = 0; j < gGame.numPlayers; j++) {
            if (gGame.numPlayers >= 2 && gGame.mode != GAME_MODE_TIME_TRIAL) {
                gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + j * 0x100);
                font_text_create(gGame.texts[gEvent.numRiders * 4 + 5 + j], 0x20, FONT_MENU, TEXT_COLOR_CYAN, TEXT_CENTER);
                buf[0] = j + '1';
                buf[1] = '\0';
                font_text_add(gGame.texts[gEvent.numRiders * 4 + 5 + j], buf);
                font_draw_text(gGame.texts[gEvent.numRiders * 4 + 5 + j], wid[j] + 5 + img->w * 4,
                              (img->h >> 1) + 0x37 + gRiders[j].race.finishPlace * 25 + ((s->type != SCREEN_TYPE_STANDINGS) ? 7 : 0));
            } else {
                gDPLoadTLUT_pal256(gdl++, gGame.icons->addr);
            }
            if (s->type == SCREEN_TYPE_STANDINGS) {
                for (k = 0; k < gEvent.numRiders; k++) {
                    if (gSeason->standings[k] == j) {
                        break;
                    }
                }
            }
            px = 5 + wid[j + gTimeTrialPlayer] + rx;
            if (s->type != SCREEN_TYPE_STANDINGS || CHAMP4) {
                y = 0x3E + rowh * gRiders[j + gTimeTrialPlayer].race.finishPlace + ry;
            } else {
                y = 0x3E + rowh * k + ry;
            }
            DRAW(px, y, img, 3);
        }
        gdl = font_render(gdl);
        switch (gGame.screenState) {
        case 7:
            gdl = shell_draw_yes_no_dialog(gdl, fb, sStrReallyRetire);
            break;
        case 4:
            gdl = shell_draw_results_menu(gdl, fb, gGame.menuSel, sSeasonResultOptions);
            gdl = font_render(gdl);
            break;
        case 3:
            gdl = shell_draw_results_menu(gdl, fb, gGame.menuSel, (gGame.mode != GAME_MODE_TIME_TRIAL) ? ((gCurTrack_41->f50 & 2) ? sSpecialTrackResultOptions : sExhibitionResultOptions) : sTimeTrialResultOptions);
            gdl = font_render(gdl);
            break;
        }
        gDPPipeSync(gdl++);
        break;
    }
    case SCREEN_TYPE_TITLE:
        if (gGame.fadeState != 1) {
            render_draw_race(m, fb, &gdl);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            DRAWV(0x43, 0x14, &gGame.titleLogo[1], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xE1, 0xD7, 0x1E, 0xFF);
            DRAWV(0x43, 0x14, &gGame.titleLogo[2], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xF5, 0xF5, 0xF5, 0xFA);
            DRAWV(0xFE - gGame.frames[1].w * 4, 200, &gGame.frames[1], 6);
        }
        font_reset_queue();
        gDPPipeSync(gdl++);
        if (gGame.flags & 0x8000000) {
            font_text_create(gGame.texts[0], 0, FONT_MAIN, TEXT_COLOR_YELLOW, TEXT_VALIGN_BASELINE | TEXT_HALIGN_CENTER);
            font_text_add(gGame.texts[0], sStrNoControllerInSocket1);
            font_draw_text(gGame.texts[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        } else if (gFrameCounter & 0x20) {
            font_text_create(gGame.texts[0], 0, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_VALIGN_BASELINE | TEXT_HALIGN_CENTER);
            font_text_add(gGame.texts[0], sStrPressStart);
            font_draw_text(gGame.texts[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        break;
    case SCREEN_TYPE_RIDER_SELECT: {
        s32 u7;
        s32 u8;
        s32 u9;

        render_draw_race(m, fb, &gdl);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        for (i = 0; i < gGame.numPlayers; i++) {
            s32 yo;
            s32 bx;
            s32 by;
            s32 x;
            s32 y;

            switch (gGame.riderSelectState[0]) {
            case 1:
                yo = (0x3E - gGame.riderSelectAnim[0]) * 2;
                break;
            case 2:
                yo = 0x7C;
                break;
            case 3:
                yo = gGame.riderSelectAnim[0] * 2;
                break;
            default:
                yo = 0;
                break;
            }
            yo2 = yo * 190 / 100;
            font_reset_queue();
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[gNumViews - 1][i].x, VP[gNumViews - 1][i].y,
                          VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - 1,
                          VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 1);
            if (gGame.numPlayers < 3) {
                if (!(gGame.flags & GAME_FLAG_PLAYER_READY(i))) {
                    gDPSetTexturePersp(gdl++, G_TP_NONE);
                    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                    if (gGame.numPlayers == 1) {
                        gDPLoadTLUT_pal256(gdl++, gGame.itemBars->addr);
                        DRAW(VP[gNumViews - 1][i].x + 0x10, VP[gNumViews - 1][i].y + 5 + yo, &gGame.itemBars[1], 3);
                        if ((gFrameCounter & 0x20) && gGame.riderSelectState[0] == 0) {
                            gDPLoadTLUT_pal256(gdl++, gGame.smallArrows->addr);
                            DRAW(0x28, yo + 0x17, &gGame.smallArrows[1], 3);
                            DRAW(0xF5, yo + 0x17, &gGame.smallArrows[2], 3);
                        }
                    } else if (gFrameCounter & 0x20) {
                        if (gSplitVertical != 0 && i == 1) {
                            x = 0x120;
                            y = 0x3C;
                        } else {
                            x = 0x41;
                            y = i * 120 + 0x3C;
                        }
                        gDPLoadTLUT_pal256(gdl++, gGame.smallArrows->addr);
                        DRAW(x, y, &gGame.smallArrows[1], 3);
                        DRAW(x | 0x8C, y, &gGame.smallArrows[2], 3);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
                    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &gGame.frames[8];
                    if (gGame.numPlayers == 2 && i == 0 && gSplitVertical == 0) {
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;
                        s32 dy = VP[gNumViews - 1][i].y;

                        if (gGame.flags & GAME_FLAG_PLAYER_READY(i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    } else {
                        s32 dy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h;
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;

                        if (gGame.flags & GAME_FLAG_PLAYER_READY(i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    }
                    img = &gGame.frames[8];
                    bx = yo2 + VP[gGame.numPlayers - 1][i].x + VP[gGame.numPlayers - 1][i].w - img->w * 8 + 0x1F;
                    if (gGame.numPlayers == 2 && i == 0 && gSplitVertical == 0) {
                        by = VP[gGame.numPlayers - 1][i].y + 11;
                    } else {
                        by = VP[gGame.numPlayers - 1][i].y + VP[gGame.numPlayers - 1][i].h - img->h + 11;
                    }
                    for (j = 1; j < 17; j++) {
                        font_text_create(gGame.texts[j + i * 20], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                    }
                    for (; j < 20; j++) {
                        font_text_create(gGame.texts[j + i * 20], 0x40, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, TEXT_RIGHT);
                    }
                }
                font_text_create(gGame.texts[i * 20], 0x40, FONT_MENU, TEXT_COLOR_YELLOW, TEXT_CENTER);
                font_text_add(gGame.texts[i * 20], sRiderBios[gGame.riders[i]].fullName);
                if (gGame.numPlayers == 1) {
                    if (gGame.flags & GAME_FLAG_PLAYER_READY(i)) {
                        font_draw_text(gGame.texts[i * 20], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x46);
                        font_text_create(gGame.texts[i * 20 + 1], 0x40, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
                        font_text_add(gGame.texts[i * 20 + 1], sStrPressAToContinue);
                        font_draw_text(gGame.texts[i * 20 + 1], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x53);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[0][0].x + 0x7E, VP[0][0].y + 0xF + yo);
                    }
                } else if (gSplitVertical != 0) {
                    if (gGame.flags & GAME_FLAG_PLAYER_READY(i)) {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x24);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2);
                    }
                } else {
                    s32 ty = i * 10 + 0x1C;

                    if (gGame.flags & GAME_FLAG_PLAYER_READY(i)) {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 4 - 10,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    }
                }
                if (!(gGame.flags & GAME_FLAG_PLAYER_READY(i))) {
                    for (j = 1; j < 5; j++) {
                        font_text_add(gGame.texts[i * 20 + j], sRiderStatLabels[j - 1]);
                        font_draw_text(gGame.texts[i * 20 + j], bx - (j - 1) * 6, by + (j - 1) * 20);
                    }
                    if (gGame.numPlayers == 1) {
                        shell_draw_rider_bio(i, yo);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    if (gGame.numPlayers == 1) {
                        j = 0;
                        DRAWV(VP[0][0].x, VP[0][0].y + yo, &gGame.frames[1], 6);
                        img = &gGame.frames[2];
                        DRAWV(VP[0][0].x, VP[0][0].y - img->h + yo, img, 6);
                        for (j = 0; j < 16; j++) {
                            if (str_icmp(sRiderBios[gGame.riders[i]].text[2], gRiderAnims[j].name) == 0) {
                                break;
                            }
                        }
                        if (gRiderAnims[j].inputGlyphs != NULL) {
                            gdl = event_draw_glyph_string(gdl, fb, gRiderAnims[j].inputGlyphs, 0x8C, yo + VP[0][0].y - 0x48);
                        }
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    img = &gGame.frames[6];
                    for (j = 0; j < 4; j++) {
                        s32 ox;
                        s32 oy;

                        ox = j * 6 - 0x55;
                        n = (sRiderBios[gGame.riders[i]].stats[j] + 1) * 2;
                        switch (n >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        c = 0;
                        oy = j * 20 - 7;
                        for (; c < 10; c++) {
                            rowh = n;
                            ry = n;
                            rx = n;
                            t = n;
                            if (c == n) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAW(bx - ox + c * 9, by + oy, img, 6);
                        }
                    }
                }
            } else {
                s32 u10;
                s32 cx;
                s32 cy;
                s32 u11;
                s32 msk;
                s32 nn;

                gDPSetTextureLUT(gdl++, G_TT_NONE);
                gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
                gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                if (!(gGame.flags & GAME_FLAG_PLAYER_READY(i))) {
                    img = &gGame.frames[7];
                    for (j = 0; j < 4; j++) {
                        nn = (sRiderBios[gGame.riders[i]].stats[j] + 1) * 2;
                        nn &= ~1;
                        switch (nn >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        switch (j) {
                        case 0:
                            cx = VP[gNumViews - 1][i].x + 0x12;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x1A;
                            break;
                        case 1:
                            cx = VP[gNumViews - 1][i].x + 0x74;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x1A;
                            break;
                        case 2:
                            cx = VP[gNumViews - 1][i].x + 0xF;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0xD;
                            break;
                        case 3:
                            cx = VP[gNumViews - 1][i].x + 0x71;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0xD;
                            break;
                        }
                        for (c = 0; c < 10; c++) {
                            if (c == nn) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAWV(cx + c * 9, cy, img, 6);
                        }
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &gGame.frames[10];
                    DRAWV(VP[gNumViews - 1][i].x, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h, img, 6);
                }
                font_text_create(gGame.texts[i * 4], 0x40, FONT_MENU, TEXT_COLOR_YELLOW, TEXT_CENTER);
                font_text_add(gGame.texts[i * 4], sRiderBios[gGame.riders[i]].fullName);
                {
                    s32 yy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2;
                    s32 xx = VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2;
                    s32 ty2;

                    msk = GAME_FLAG_PLAYER_READY(i);
                    ty2 = yy + 0xA;
                    if (gGame.flags & msk) {
                        ty2 = yy + 0x23;
                    }
                    font_draw_text(gGame.texts[i * 4], xx, ty2);
                }
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                if (!(gGame.flags & msk) && (gFrameCounter & 0x20)) {
                    gDPLoadTLUT_pal256(gdl++, gGame.smallArrows->addr);
                    DRAWK(VP[gNumViews - 1][i].x + 5, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &gGame.smallArrows[1], 3, u10);
                    DRAWK(VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - 0x14,
                         VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &gGame.smallArrows[2], 3, u11);
                }
            }
            if (gGame.numPlayers < 3) {
                gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[gNumViews - 1][0].x, VP[gNumViews - 1][0].y,
                              VP[gNumViews - 1][gNumViews - 1].x + VP[gNumViews - 1][gNumViews - 1].w - 1,
                              VP[gNumViews - 1][gNumViews - 1].y + VP[gNumViews - 1][gNumViews - 1].h - 1);
            }
            gdl = font_render(gdl);
            gDPPipeSync(gdl++);
        }
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[0][0].x, VP[0][0].y, VP[0][0].x + VP[0][0].w - 1,
                      VP[0][0].y + VP[0][0].h - 1);
        gDPPipeSync(gdl++);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        if (gGame.numPlayers == 1) {
            if (gGame.mode != GAME_MODE_TIME_TRIAL) {
                font_text_create(gGame.texts[0], 0x40, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
                font_text_add(gGame.texts[0], sStrSwitchBikeColor);
                font_draw_text(gGame.texts[0], 0x16C, 0x25);
            }
            font_text_create(gGame.texts[1], 0x40, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_LEFT);
            font_text_add(gGame.texts[1], sStrSwitchRiderColor);
            font_draw_text(gGame.texts[1], 0x16C, 0x19);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gDPLoadTLUT_pal256(gdl++, gGame.cButtonIcons->addr);
            if (gGame.mode != GAME_MODE_TIME_TRIAL) {
                DRAWK(0x140, 0x21, &gGame.cButtonIcons[1], 3, u9);
                DRAW(0x154, 0x21, &gGame.cButtonIcons[3], 3);
            }
            DRAWK(0x140, 0x13, &gGame.cButtonIcons[4], 3, u7);
            DRAWK(0x154, 0x13, &gGame.cButtonIcons[2], 3, u8);
        }
        gdl = font_render(gdl);
        break;
    }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    if (gGame.fadeState != 3) {
        gdl = screen_draw_wipe(gdl, m, gGfxFrameIndex);
    }
    {
        Gfx *_g = gdl++;

        _g->words.w0 = 0xE7000000;
        _g->words.w1 = ((u16)u1 >> 16) + ((u16)u2 >> 16) + ((u16)u3 >> 16);
    }
    return gdl;
}

#define IMG(s) ((s)->type == SCREEN_TYPE_ROUND_SELECT ? gGame.screenPics[*(s)->cursor] : (s)->bg)

typedef struct {
    s16 x, y, w, h;
} Rect_3;

extern Rect_3 sTitleSceneRect;

void shell_draw(Mtx *m, s32 fb, Gfx **pgdl)
{
    u8 unk28[0x70];
    Mtx persp;
    u16 pn;
    Gfx *gdl;
    Screen *s;
    ScreenItem *item;
    u16 alpha;

    gdl = *pgdl;
    if (gGame.fadeState == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    if (s != NULL) {
        if (gGame.fadeState == 0) {
            item = s->items + sPrevScreenCursor;
        } else {
            item = s->items;
            if (s->cursor != NULL) {
                item += *s->cursor;
            }
        }
    }
    alpha = 0xFF;
    if (gGame.fadeState == 1) goto c1;
    if (gGame.fadeState == 0) goto c0;
    if (gGame.fadeState == 2) goto c2;
    goto done;
c1:
    alpha = 0;
    goto done;
c0:
    alpha = gGame.fadeLevel;
    goto done;
c2:
    alpha = 0xFF - gGame.fadeLevel;
done:
    mtx_from_angles_pos_scale(&m[16], 0, 0, 0, 0, 0, 0, 0x100);
    guOrtho(&m[12], -gScreenWidth / 2, gScreenWidth / 2, -gScreenHeight / 2, gScreenHeight / 2, 50.0f, 4192.0f, 1.0f);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, gRdpInitDL);
    gSPDisplayList(gdl++, gRspInitDL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gSPViewport(gdl++, &sShellViewport);
    gDPPipeSync(gdl++);
    guPerspective(&persp, &pn, 33.0f, 1.9512196f, 25.0f, 4192.0f, 1.0f);
    mtx_mul(&gViewAxisMtx, &persp, m);
    gSPPerspNormalize(gdl++, pn);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_DECALRGB, G_CC_DECALRGB);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    if (gGame.fadeState == 1 || s == NULL || s->bgResId == NULL) {
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, gZBuffer);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &m[12], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    font_reset_queue();
    gDPPipeSync(gdl++);
    if (gGame.fadeState == 1) {
        if (gGame.fadeTimer > 0 || gGame.fadeTimer == -1) {
            gGame.fadeTimer--;
        }
    } else {
        if (s->type == SCREEN_TYPE_ROUND_SELECT || s->bgResId != NULL) {
            gDPLoadTLUT_pal256(gdl++, s->type == SCREEN_TYPE_ROUND_SELECT ? gGame.screenPics[*s->cursor]->pal : s->bg->pal);
            gdl = render_draw_image(0, 0, IMG(s)->img, gdl, fb, 1, 1, 3, 0);
        }
        if (s->type == SCREEN_TYPE_TITLE) {
            Rect_3 *r = &sTitleSceneRect;
            sTitleSceneViewport.vp.vscale[2] = 0x1FF;
            sTitleSceneViewport.vp.vscale[3] = 0;
            sTitleSceneViewport.vp.vtrans[2] = 0x1FF;
            sTitleSceneViewport.vp.vtrans[3] = 0;
            sTitleSceneViewport.vp.vscale[0] = r->w * 2;
            sTitleSceneViewport.vp.vscale[1] = r->h * 2;
            sTitleSceneViewport.vp.vtrans[0] = (r->x + r->w / 2) * 4;
            sTitleSceneViewport.vp.vtrans[1] = (r->y + r->h / 2) * 4;
            gSPViewport(gdl++, &sTitleSceneViewport);
            gdl = shell_draw_screen(m, fb, gdl, item, 0);
        } else if (s != NULL) {
            gdl = shell_draw_screen(m, fb, gdl, item, 0);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
            ((u8 *)gFontColors)[28 * 4 + 3] = alpha;  /* FONT_FADE_ALPHA */
        }
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void shell_load_gui_res(void) {
    res_open_set("guires", &gGuiRes);
}

void shell_free_gui_res(void)
{
    res_close_set(gGuiRes);
    gGuiRes = 0;
}

void shell_cb_desert(s32 p) {
    if (p != 0) {
        gCurTrack = &gTracks[TRACK_DESERT];
        gGame.mode = GAME_MODE_DESERT;
        gRaceSeries_5.b = 0;
        gRaceSeries_5.a = 1;
        gRaceSeries_5.c = 0;
        gGame.flags &= 0xFFFFFF7F;
    }
}

void shell_cb_select_track(s32 a) {
    if (a != 0) {
        gCurTrack = gTracks_2 + gGame.trackSel * 0xA0;
        gGame.flags |= 0x80;
    }
}

void shell_cb_time_trials(s32 a0)
{
    Game *base;

    if (a0 != 0) {
        base = &gGame;
        base->mode = GAME_MODE_TIME_TRIAL;
        base->numPlayers = 1;
        base->flags = base->flags | 0x80;
    }
}

typedef struct {
    u8 pad0[0x2C1];
    u8 track;
} Opts_2;

extern Opts_2 gSaveData_14;

void shell_cb_tutorial(s32 arg)
{
    s32 i;

    if (arg != 0) {
        gCurTrack = &gTracks[TRACK_TUTORIAL];
        gGame.numPlayers = 1;
        gGame.mode = GAME_MODE_TUTORIAL;
        gEvent.tutorialBuf = 0;
        gGame.riders[0] = 4;
        gGame.flags &= ~0x80;
        gSeason->cup = &gCups[CUP_TUTORIAL];
        gSeason->round = 0;
        for (i = 0; i < 0x11; i++) {
            if (gTutorialLessons[i].level == gSaveData_14.track) {
                gSeason->round = i;
                return;
            }
        }
    }
}

void shell_cb_exhibition(s32 x)
{
    if (x != 0) {
        gGame.mode = GAME_MODE_EXHIBITION;
    }
}

void shell_cb_excite3d(s32 p) {
    s32 i;

    if (p != 0) {
        i = 0;
        do {
            if (i == 0) {
                gGame.riderSkins[0] = (u8)gGame.riderSkinSel;
            } else {
                gGame.riderSkins[i] = i;
            }
            i++;
        } while (i < 6);
        gGame.mode = GAME_MODE_EXCITE_3D;
        gCurTrack = &gTracks[TRACK_EXCITE_3D];
        gGame.numPlayers = 1;
        gGame.difficulty = 2;
        gGame.flags &= 0xFFFFFF7F;
    }
}

void shell_cb_hill_climb(s32 p)
{
    s32 i;
    if (p != 0) {
        for (i = 0; i < 6; i++) {
            if (gGame.numPlayers < 2) {
                if (i == 0) {
                    gGame.riderSkins[0] = (u8)gGame.riderSkinSel;
                } else {
                    gGame.riderSkins[i] = i;
                }
            } else {
                gGame.riderSkins[i] = i;
            }
        }
        gGame.numCpuRiders = 0;
        gSplitVertical = 1;
        gGame.mode = GAME_MODE_HILL_CLIMB;
        gCurTrack = &gTracks[TRACK_HILL_CLIMB];
        gGame.flags &= ~0x80;
    }
}

void shell_cb_custom_tracks(s32 x)
{
    if (x != 0) {
        gGame.flags = (gGame.flags & ~0x80) | 0x20;
    }
}

void shell_cb_desert_unused(s32 a0) {
    s32 i;
    Game *b;
    Game *w;
    if (a0 == 0) {
        return;
    }
    w = &gGame;
    w->numPlayers = 1;
    b = w;
    gCurTrack = &gTracks[TRACK_DESERT];
    w->mode = 4;
    w->flags = w->flags & 0xFFFFFF7F;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            b->playerPad[0] = 0;
        } else {
            b->playerPad[i] = -1;
        }
        if (i == 0) {
            b->padPlayer[0] = 0;
        } else {
            b->padPlayer[i] = -1;
        }
    }
    gRaceSeries[1] = 0;
    gRaceSeries[0] = 1;
    gRaceSeries[2] = 0;
}

void shell_cb_original_excitebike(s32 chosen)
{
    gGame.mode = GAME_MODE_ORIGINAL_EXCITEBIKE;
    gGame.flags = (gGame.flags & ~0x80) | 0x40;
}

void shell_cb_stunt_course(s32 param_1)
{
    if (param_1 != 0) {
        Game *b = &gGame;
        b->mode = GAME_MODE_STUNT_COURSE;
        gCurTrack = &gTracks[TRACK_STUNT_COURSE];
        gGame.flags = gGame.flags & 0xffffff7f;
    }
}

s32 shell_pak_track_invalid(u32 *p, s32 n)
{
    s32 r;
    if (n != 0x400) {
        return 1;
    }
    r = calc_checksum24((u8 *)(p + 1), 0x3FC);
    return r != *p;
}

void shell_backup_race_results(void)
{
    s32 i = 0;

    gGame.resultsNumRiders = gEvent.numRiders;
    do {
        gGame.resultsOrder[i] = gEvent.finishOrder[i];
        gGame.resultsTimes[i] = gRiders[i].race.finishTime;
        gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
        if (gSeason != 0) {
            gGame.resultsSeasonOrder[i] = gSeason->standings[i];
        }
        i++;
    } while (i < 6);
}

void shell_restore_race_results(void)
{
    s32 i;

    gEvent.numRiders = gGame.resultsNumRiders;
    for (i = 0; i < 6; i++) {
        gEvent.finishOrder[i] = gGame.resultsOrder[i];
        gRiders[i].race.finishTime = gGame.resultsTimes[i];
        gRiders[i].race.finishPlace = gGame.resultsCelebration[i];
        if (gSeason != 0) {
            gSeason->standings[i] = gGame.resultsSeasonOrder[i];
        }
    }
}

s32 shell_is_round_screen_unlocked(Screen *screen)
{
    s32 mode;
    s32 t;

    if (gGame.mode == GAME_MODE_TIME_TRIAL || gGame.mode == GAME_MODE_EXHIBITION) {
        mode = gSaveData.topDifficulty;
    } else {
        mode = gGame.difficulty;
    }
    if (mode == 2) {
        goto two;
    }
    if (mode < 3) {
        if (mode == 1) {
            goto one;
        }
        goto ret0;
    }
    if (mode == 3) {
        goto ret1;
    }
    goto ret0;
one:
    t = screen->firstItem < 10;
tail:
    if (t == 0) {
        goto ret0;
    }
ret1:
    return 1;
two:
    t = screen->firstItem < 0xF;
    goto tail;
ret0:
    return 0;
}

void shell_main_menu_or_back_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 n;
    Screen *back;

    if ((*buttons & 0x50) != 0) {
        shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
        n = 1;
    } else {
        if ((*buttons & 0x20) == 0) {
            return;
        }
        back = screen->back;
        if (back == NULL) {
            return;
        }
        shell_goto_screen(back, 0, 0);
        n = 2;
    }
    snd_play_menu_sound(n);
}

typedef struct {
    u8 pad[0x2BF];
    u8 bits;
    u8 mode;
} Save_6;

extern Save_6 gSaveData_15;

s32 shell_is_round_unlocked_tt(s32 p)
{
    s32 v;
    if (p == 0) goto one;
    switch (gSaveData_15.mode) {
    case 1:
        if (p >= 2) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    one:
        return 1;
    case 2:
        if (p < 2) goto one;
        if (p != 2) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    case 3:
        if (p < 3) goto one;
        if (p != 3) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    default:
        goto zero;
    }
    if (v) goto one;
zero:
    return 0;
}

s32 shell_check_sp_imem_magic(void)
{
    return *(u32 *)0xA4001000 == 0x17D7;
}
