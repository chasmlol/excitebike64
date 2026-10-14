/* Values the whole game shares: the input word, the game's mode and flags, cheats, cups, track flags, the save
   options and volumes, menu sounds, fades, video modes, gravity and how a race ended. */
#ifndef CONSTANTS_H
#define CONSTANTS_H

/* The game's input word (gPadInput, gPlayerInputs, Rider.input), which main_convert_pad_input builds from an
   OSContPad; it is not libultra's button layout. The stick sets a direction bit and how far it is pushed (0-15) in
   bits 24-27 (x) and 28-31 (y); the direction bits give the signs. In the buttons shell_update gives the screens
   (pressed this frame) the stick bits also hold the D-pad, and stick directions under 6 are dropped. */
#define INPUT_STICK_UP          0x00000001
#define INPUT_STICK_DOWN        0x00000002
#define INPUT_STICK_LEFT        0x00000004
#define INPUT_STICK_RIGHT       0x00000008
#define INPUT_A                 0x00000010  /* throttle */
#define INPUT_B                 0x00000020  /* brake */
#define INPUT_START             0x00000040
#define INPUT_C_UP              0x00000080
#define INPUT_C_DOWN            0x00000100  /* tricks: the stick's moves are read while it is held */
#define INPUT_C_LEFT            0x00000200
#define INPUT_C_RIGHT           0x00000400
#define INPUT_D_UP              0x00000800
#define INPUT_D_DOWN            0x00001000
#define INPUT_D_LEFT            0x00002000
#define INPUT_D_RIGHT           0x00004000
#define INPUT_L                 0x00008000
#define INPUT_R                 0x00010000  /* with the stick in the air, the air turns; with B, a power slide */
#define INPUT_Z                 0x00020000  /* turbo */
#define INPUT_STICK_X_SHIFT     24
#define INPUT_STICK_Y_SHIFT     28
#define INPUT_STICK_X_MAX       0x0F000000  /* full x magnitude */
#define INPUT_STICK_Y_MAX       0xF0000000  /* full y magnitude */

/* gGame.mode, set by the menu items' callbacks. Modes 3, 4, 11 and 15 are not identified (shell_cb_desert_unused
   sets 4; the rider code tests 15). */
#define GAME_MODE_EXHIBITION    0
#define GAME_MODE_SEASON        1
#define GAME_MODE_SOCCER        2
#define GAME_MODE_DESERT        5   /* ten checkpoint gates */
#define GAME_MODE_EXCITE_3D     6
#define GAME_MODE_HILL_CLIMB    7
#define GAME_MODE_TIME_TRIAL    8
#define GAME_MODE_TUTORIAL      9
#define GAME_MODE_STUNT_COURSE  10
#define GAME_MODE_ORIGINAL      12  /* Original Excitebike, the NES game */
#define GAME_MODE_TEST_LOOP     13  /* the editor's test ride of a closed track */
#define GAME_MODE_TEST_OPEN     14  /* ... of a track that is not closed into a loop */

/* gGame.flags */
#define GAME_FLAG_FRONT_END_RACE    0x00000001  /* a race the front end runs behind its screens (title, rider select) */
#define GAME_FLAG_EDITOR_TO_SHELL   0x00000002  /* leaving the editor returns to the front end, not to a test ride */
#define GAME_FLAG_REPEAT_STATE      0x00000004  /* run the same game state again (restart the race) */
#define GAME_FLAG_TRACK_LISTS       0x00000008  /* coming back from a race opens the track lists */
#define GAME_FLAG_RIDER_SELECT      0x00000010
#define GAME_FLAG_EDITOR            0x00000020  /* leaving the front end starts the track editor */
#define GAME_FLAG_NES               0x00000040  /* ... or Original Excitebike */
#define GAME_FLAG_TRACK_INTRO       0x00000080  /* a track was chosen: the race opens on the track intro, which clears
                                                   it */
#define GAME_FLAG_PLAYERS_POPUP     0x00000200  /* list screens: the number of players popup is up */
#define GAME_FLAG_PLAYER_READY(n)   (0x400 << (n)) /* rider select: player n has chosen */
#define GAME_FLAG_PLAYERS_READY     0x00003C00
#define GAME_FLAG_REPLAY            0x00008000  /* replay playback */
#define GAME_FLAG_IN_RACE           0x00010000  /* from race_init to race_exit */
#define GAME_FLAG_RESULTS_SEEN      0x00020000  /* the race's results were shown (season progress counted) */
#define GAME_FLAG_PODIUM            0x00040000  /* the podium ceremony (gPodiumMode) */
#define GAME_FLAG_GHOST_LOADED      0x00080000  /* time trial: a ghost loaded from a Controller Pak rides along */
/* Unlock messages, set by shell_update_season_progress */
#define GAME_FLAG_ROUND_WON         0x00100000  /* a round won for the first time: the next one is open */
#define GAME_FLAG_NEW_UNLOCKS       0x00200000  /* save_update_unlocks opened something (gGame.newUnlocks) */
#define GAME_FLAG_DIFFICULTY_UP     0x00400000  /* all rounds won: the next difficulty is open */
#define GAME_FLAG_SEASON_COMPLETE   0x00800000  /* ... at the top difficulty: the next race is the credits run */
#define GAME_FLAG_UNLOCK_MESSAGES   0x00F00000
#define GAME_FLAG_CREDITS_FROM_OPTIONS 0x01000000 /* the credits run of the options: B leaves it too */
#define GAME_FLAG_RACE_PAK_TRACK    0x02000000  /* the user track loaded from a Controller Pak is raced, not edited */
#define GAME_FLAG_LOW_RES           0x04000000  /* the race stays in low resolution (min.c) */
#define GAME_FLAG_NO_CONTROLLER     0x08000000  /* no controller in socket 1 */
#define GAME_FLAG_SPLASH_3          0x10000000  /* SCREEN_SPLASH_3 is up: screen_draw_wipe draws at full level */
#define GAME_FLAG_DEBUG             0x20000000  /* the Debug cheat */

/* gCheatFlags and sCheatMask: bit n is the cheat of sCheatTable[n], entered on the cheat screen */
#define CHEAT_BIG_HEAD              0x0001
#define CHEAT_MIRROR                0x0002
#define CHEAT_DEBUG                 0x0004
#define CHEAT_DOWN_HILL             0x0008
#define CHEAT_STUNT_BONUS           0x0010
#define CHEAT_INVISIBLE_RIDER       0x0020
#define CHEAT_NIGHT                 0x0040
#define CHEAT_ODD_COLOR             0x0080
#define CHEAT_TRANSPARENT_RIDER     0x0100
#define CHEAT_ALL_STUNTS            0x0200
#define CHEAT_INVINCIBLE            0x0400
#define CHEAT_TEAM_PHOTO            0x0800
#define CHEAT_BEAT_THIS             0x1000
#define CHEAT_PIN_HEAD              0x2000

/* gCups: the season's rounds (Season.cup - gCups), then the custom round and the tutorial's lessons */
#define CUP_BRONZE      0
#define CUP_SILVER      1
#define CUP_GOLD        2
#define CUP_PLATINUM    3
#define CUP_CHALLENGE   4   /* one race against one rider, after Platinum */
#define CUP_CUSTOM      5   /* the custom round the player puts together */
#define CUP_TUTORIAL    6

/* A track's environment (gTracks +0x2C): which code draws it. */
#define TRACK_ENV_STADIUM       0   /* arngfx.c */
#define TRACK_ENV_OUTDOOR       1   /* tubegfx.c */
#define TRACK_ENV_DESERT        2   /* desgfx.c and fractal.c */

/* A track's flags (gTracks +0x50). */
#define TRACK_FLAG_USER         0x002   /* the user track: built by UsrTrk.c from the editor's pieces, keeps no
                                           records */
#define TRACK_FLAG_STARS        0x020   /* a night sky: sun.c draws stars */
#define TRACK_FLAG_NEW_LAP      0x080   /* this race set a best lap: the initials are asked for */
#define TRACK_FLAG_NEW_TIME     0x100   /* ... a best race time (or score) */
#define TRACK_FLAG_NEW_RECORD   0x180
#define TRACK_FLAG_RECORDS      0x200   /* keeps records (gGame.records, the save data) */
#define TRACK_FLAG_SCORES       0x400   /* the records are scores, not times (the stunt course) */

/* SaveData.options */
#define SAVE_OPT_STEREO         0x01
#define SAVE_OPT_HIGH_RES       0x02
#define SAVE_OPT_USER_TRACK     0x08    /* a user track is saved on the Game Pak */
#define SAVE_OPT_NO_DRONES      0x10    /* no extra riders (6 and 7) */

/* SaveData.volumes */
#define VOLUME_MUSIC            0
#define VOLUME_SFX              1
#define VOLUME_SPEECH           2

/* The volume kinds of snd_set_volume and snd_get_volume. */
#define SND_VOLUME_SFX          0   /* effects and ambience (gSfxVolume, gAmbientVolume) */
#define SND_VOLUME_MUSIC        1   /* both music channels (gStreamVolumes) */
#define SND_VOLUME_SPEECH       2   /* the announcer (gSpeechVolume) */

/* Menu sounds (sMenuSounds, snd_play_menu_sound). */
#define MENU_SOUND_MOVE         0
#define MENU_SOUND_SELECT       1
#define MENU_SOUND_BACK         2
#define MENU_SOUND_ERROR        3
#define MENU_SOUND_COUNT        4

/* gGame.fadeState: the fade between two front end screens, and of the race's start and end (gGame.fadeLevel 0-255
   is how far along). */
#define FADE_OUT                0   /* the screen left fades out */
#define FADE_SWAP               1   /* faded out: the screens are unloaded and loaded */
#define FADE_IN                 2
#define FADE_DONE               3   /* the screen takes input */

/* Video modes (gViMode, gViModeRequest); the scheduler's sViModeWidths and sViModeHeights give the sizes. */
#define VI_MODE_LO_RES          0   /* 320x240 */
#define VI_MODE_MED_RES         1   /* 512x240 */
#define VI_MODE_HI_RES          2   /* 640x480 interlaced, libultra's Hpf1 mode unchanged (SAVE_OPT_HIGH_RES, with
                                       the Expansion Pak) */

#define GRAVITY                 14.700001f  /* the bikes' gravity, m/s^2 */

/* gRaceExitCode: how the last race ended, for shell_init. */
#define RACE_EXIT_BOOT          0   /* none yet: the game just started */
#define RACE_EXIT_FINISHED      2
#define RACE_EXIT_BACK          3   /* B on the track intro */
#define RACE_EXIT_QUIT          4   /* Quit from the pause menu */
#define RACE_EXIT_NONE          6   /* the default while racing; also set on leaving the front end, the editor
                                       or Original Excitebike */
#define RACE_EXIT_PODIUM        7   /* after the podium ceremony */
#define RACE_EXIT_EDITOR        8   /* the test ride's "Return to Editor" */

#endif
