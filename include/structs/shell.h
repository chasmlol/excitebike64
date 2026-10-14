/* The front end's screens (shell.c). sScreens holds the 24 screens in ScreenId order (the data names some of them
   again: sScreenMainMenu is sScreens[SCREEN_MAIN_MENU]). Each screen has a list of items; the screen's type selects
   its input handler in shell_update, what shell_load_screen loads and how shell_draw_screen draws it. */
#ifndef STRUCTS_SHELL_H
#define STRUCTS_SHELL_H

#include "common.h"

/* The game's input word (main_convert_pad_input): gPadInput, Rider.input, and the buttons shell_update passes to
   the screens. It is not libultra's button layout. In the screens' buttons (pressed this frame) the stick bits
   also hold the D-pad, and shell_update drops stick directions under 6. */
#define INPUT_STICK_UP      0x00000001
#define INPUT_STICK_DOWN    0x00000002
#define INPUT_STICK_LEFT    0x00000004
#define INPUT_STICK_RIGHT   0x00000008
#define INPUT_A             0x00000010
#define INPUT_B             0x00000020
#define INPUT_START         0x00000040
#define INPUT_C_UP          0x00000080
#define INPUT_C_DOWN        0x00000100
#define INPUT_C_LEFT        0x00000200
#define INPUT_C_RIGHT       0x00000400
#define INPUT_D_UP          0x00000800
#define INPUT_D_DOWN        0x00001000
#define INPUT_D_LEFT        0x00002000
#define INPUT_D_RIGHT       0x00004000
#define INPUT_L             0x00008000
#define INPUT_R             0x00010000
#define INPUT_Z             0x00020000
/* Bits 24-27 and 28-31: how far the stick is pushed along x and y (0-15); the direction bits give the signs. */
#define INPUT_STICK_X_SHIFT 24
#define INPUT_STICK_Y_SHIFT 28

/* snd_play_menu_sound */
#define MENU_SOUND_MOVE   0
#define MENU_SOUND_SELECT 1
#define MENU_SOUND_BACK   2
#define MENU_SOUND_ERROR  3

/* gGame.mode, set by the menu items' callbacks. Modes 3 and 4 are not identified (shell_cb_desert_unused sets 4). */
#define GAME_MODE_EXHIBITION    0
#define GAME_MODE_SEASON        1
#define GAME_MODE_SOCCER        2
#define GAME_MODE_DESERT        5
#define GAME_MODE_EXCITE_3D     6
#define GAME_MODE_HILL_CLIMB    7
#define GAME_MODE_TIME_TRIAL    8
#define GAME_MODE_TUTORIAL      9
#define GAME_MODE_STUNT_COURSE  10
#define GAME_MODE_ORIGINAL      12  /* Original Excitebike, the NES game */
#define GAME_MODE_TEST_LOOP     13  /* the editor's test ride, closed track */
#define GAME_MODE_TEST_OPEN     14  /* the editor's test ride, track not closed */

/* gGame.flags (include/structs/game.h) */
#define GAME_FLAG_FRONT_END_RACE    0x00000001 /* a race the front end runs behind the title or rider select */
#define GAME_FLAG_EDITOR_TO_SHELL   0x00000002 /* leaving the editor returns to the front end */
#define GAME_FLAG_REPEAT_STATE      0x00000004 /* run the same game state again */
#define GAME_FLAG_TRACK_LISTS       0x00000008 /* coming back from a race opens the track lists */
#define GAME_FLAG_RIDER_SELECT      0x00000010
#define GAME_FLAG_EDITOR            0x00000020 /* leaving the front end starts the track editor */
#define GAME_FLAG_NES               0x00000040 /* leaving the front end starts Original Excitebike */
#define GAME_FLAG_TRACK_CHOSEN      0x00000080
#define GAME_FLAG_PLAYERS_POPUP     0x00000200 /* list screens: the number of players popup is up */
#define GAME_FLAG_PLAYER_READY(n)   (0x400 << (n)) /* rider select: player n has chosen */
#define GAME_FLAG_PLAYERS_READY     0x00003C00
#define GAME_FLAG_REPLAY            0x00008000 /* replay playback */
#define GAME_FLAG_RESULTS_SEEN      0x00020000 /* the race's results were shown (season progress counted) */
#define GAME_FLAG_PODIUM            0x00040000 /* the podium ceremony (gPodiumMode) */
#define GAME_FLAG_GHOST_LOADED      0x00080000 /* time trial: a ghost was loaded from a Controller Pak */
/* Unlock messages, set by shell_update_season_progress */
#define GAME_FLAG_ROUND_WON         0x00100000 /* a round won for the first time */
#define GAME_FLAG_NEW_UNLOCKS       0x00200000 /* save_update_unlocks opened something (gGame.newUnlocks) */
#define GAME_FLAG_DIFFICULTY_UP     0x00400000 /* all rounds won: the next difficulty is open */
#define GAME_FLAG_SEASON_COMPLETE   0x00800000 /* ... at the top difficulty: the next race is the credits run */
#define GAME_FLAG_UNLOCK_MESSAGES   0x00F00000
#define GAME_FLAG_CREDITS_FROM_OPTIONS 0x01000000 /* the credits run of the options: B leaves it */
#define GAME_FLAG_RACE_PAK_TRACK    0x02000000 /* the user track loaded from a Controller Pak is raced, not edited */
#define GAME_FLAG_LOW_RES           0x04000000 /* the race stays in low resolution (min.c) */
#define GAME_FLAG_NO_CONTROLLER     0x08000000 /* no controller in socket 1 */
#define GAME_FLAG_SPLASH_3          0x10000000 /* SCREEN_SPLASH_3 is up: screen_draw_wipe draws at full level */
#define GAME_FLAG_DEBUG             0x20000000 /* the Debug cheat */

/* gGame.fadeState: the fade between two screens (gGame.fadeLevel) */
#define FADE_OUT    0       /* the screen left fades out */
#define FADE_SWAP   1       /* faded out: the screens are unloaded and loaded */
#define FADE_IN     2
#define FADE_DONE   3       /* the screen takes input */

/* gGame.pakScreenMode: what the Controller Pak screen is opened for */
#define PAK_SCREEN_MANAGER         0
#define PAK_SCREEN_LOAD_GHOST      1
#define PAK_SCREEN_SAVE_GHOST      2
#define PAK_SCREEN_LOAD_USER_TRACK 3
#define PAK_SCREEN_SAVE_USER_TRACK 4

/* gCups: the season's rounds (Season.cup - gCups), then the custom round and the tutorial's lessons */
#define CUP_BRONZE    0
#define CUP_SILVER    1
#define CUP_GOLD      2
#define CUP_PLATINUM  3
#define CUP_CHALLENGE 4 /* one race against one rider, after Platinum */
#define CUP_CUSTOM    5 /* the custom round the player puts together */
#define CUP_TUTORIAL  6

/* gSaveData.options */
#define SAVE_OPT_STEREO     0x01
#define SAVE_OPT_HIGH_RES   0x02
#define SAVE_OPT_USER_TRACK 0x08    /* a user track is saved on the Game Pak */
#define SAVE_OPT_NO_DRONES  0x10

/* gTracks flags (+0x50) the front end tests */
#define TRACK_FLAG_USER         0x002   /* the user track: keeps no records */
#define TRACK_FLAG_NEW_LAP      0x080   /* this race set a best lap: the initials are asked for */
#define TRACK_FLAG_NEW_TIME     0x100   /* ... a best race time (or score) */
#define TRACK_FLAG_NEW_RECORD   0x180
#define TRACK_FLAG_RECORDS      0x200   /* keeps records (gGame.records, the save data) */
#define TRACK_FLAG_SCORES       0x400   /* the records are scores, not times (the stunt course) */

/* gGame.screenState: what the current screen is doing besides its own input (0). Each value belongs to one kind
   of screen. */
enum ScreenState {
    SCREEN_STATE_NORMAL = 0,
    SCREEN_STATE_RESULTS_MENU = 3,           /* race results: the menu after the race */
    SCREEN_STATE_SEASON_MENU = 4,            /* season results: Continue / Replay / Retire */
    SCREEN_STATE_DIFFICULTY = 5,             /* list screens: the difficulty popup */
    SCREEN_STATE_ERASE_DIALOG = 6,           /* options: "Erase all data?" (no option leads there in this build) */
    SCREEN_STATE_RETIRE_DIALOG = 7,          /* season results: "Really retire?" */
    SCREEN_STATE_ASK_GHOST = 8,              /* "Continue" / "Load Saved Ghost Data" */
    SCREEN_STATE_ASK_USER_TRACK = 9,         /* "Race Saved Track" / "Go To Editor" */
    SCREEN_STATE_ASK_TRACK_SOURCE = 10,      /* load the user track from the Game Pak or a Controller Pak */
    SCREEN_STATE_RECORDS_FADE = 11,          /* records: the picture fades to grey, the records fade in */
    SCREEN_STATE_RECORDS_SHOWN = 12,
    SCREEN_STATE_RECORDS_PICTURE = 13,       /* records: the track's picture alone, for a few frames */
    SCREEN_STATE_ERASING = 14,               /* options: drawn once, then */
    SCREEN_STATE_ERASE = 15,                 /* the save is erased */
    SCREEN_STATE_USER_TRACK_DIFFICULTY = 16, /* the difficulty popup for the user track */
    SCREEN_STATE_CHEAT_ACCEPTED = 17,
    SCREEN_STATE_CHEAT_INVALID = 18
};

/* The rider select screen's rider bio (gGame.riderSelectState, held with Z) */
#define BIO_HIDDEN  0
#define BIO_OPENING 1
#define BIO_SHOWN   2
#define BIO_CLOSING 3

/* The idle frames after which the main menu returns to the title and the title starts the attract race */
#define SHELL_IDLE_TIMEOUT 0xE10

typedef struct Screen Screen;

/* ScreenItem.flags */
#define SCREEN_ITEM_LOCKED          0x0001 /* greyed out: choosing it plays the error sound */
#define SCREEN_ITEM_ASK_DIFFICULTY  0x0100 /* choosing it opens the difficulty popup first */
#define SCREEN_ITEM_NEED_DIFFICULTY 0x0400 /* ... which then has no difficulty 0 */
#define SCREEN_ITEM_ASK_GHOST       0x0800 /* asks "Continue" / "Load Saved Ghost Data" first */
#define SCREEN_ITEM_ASK_USER_TRACK  0x1000 /* asks "Race Saved Track" / "Go To Editor" first */

typedef struct ScreenItem {
    /* 0x00 */ char *text;
    /* 0x04 */ Screen *target;               /* where A goes (NULL: start the race) */
    /* 0x08 */ u16 flags;                    /* SCREEN_ITEM_* */
    /* 0x0A */ u8 unk_0A[2];
    /* 0x0C */ void (*callback)(s32 chosen); /* run when the item is chosen: sets the game mode */
} ScreenItem; /* size 0x10 */

/* Screen.flags */
#define SCREEN_DITHER_FILTER 0x0004 /* VI dither filter on while the screen is shown */
#define SCREEN_A_B_NAVIGATION 0x0010 /* shell_update follows the item's target on A and the back screen on B */
#define SCREEN_TEXT_OBJECTS  0x0020 /* the first screen of a row creates the 48 shell text objects */
#define SCREEN_PANELS        0x0080 /* loads the frame panel images */
#define SCREEN_KEEP_MUSIC    0x0200 /* the menu music keeps playing (no fade of the master volume) */
#define SCREEN_STICK_FILTER  0x2000 /* the stick moves along one axis only (shell_filter_stick_dirs) */

/* Screen.type */
enum ScreenType {
    SCREEN_TYPE_TITLE = 0,              /* title over an attract race */
    SCREEN_TYPE_RIDER_SELECT = 2,
    SCREEN_TYPE_LIST = 3,               /* menu list with a picture per item */
    SCREEN_TYPE_ROUND_SELECT = 4,       /* season round (cup) select */
    SCREEN_TYPE_OPTIONS = 6,
    SCREEN_TYPE_SEASON_RACE_RESULT = 7,
    SCREEN_TYPE_ROUND_OVER = 8,
    SCREEN_TYPE_CHEAT_ENTRY = 9,
    SCREEN_TYPE_TRACK_LIST = 11,
    SCREEN_TYPE_PAK = 12,               /* Controller Pak screen, drawn by pak.c */
    SCREEN_TYPE_RACE_RESULT = 13,
    SCREEN_TYPE_STANDINGS = 14,
    SCREEN_TYPE_RECORDS = 18,
    SCREEN_TYPE_CONTROLLER_SETTINGS = 19,
    SCREEN_TYPE_INITIALS = 20,          /* initials entry for a new record */
    SCREEN_TYPE_SPLASH = 21,            /* a splash (the Left Field logo), 180 frames */
    SCREEN_TYPE_SPLASH_2 = 22,          /* the Nintendo splash, the Expansion Pak picture blinking when present */
    SCREEN_TYPE_SPLASH_3 = 23,          /* the N64 Sports logo (GAME_FLAG_SPLASH_3) */
    SCREEN_TYPE_PAK_STATUS = 24,        /* free pages of each Controller Pak, 1200 frames */
    SCREEN_TYPE_BACKUP_LOST = 25        /* "Game Pak backup memory has been lost.", 600 frames */
};

/* Index in sScreens. The data defines sScreens in pieces where other data points into it (sScreenTitle is
   sScreens[SCREEN_TITLE], ...). */
enum ScreenId {
    SCREEN_SEASON_RACE_RESULT,  /* 0 */
    SCREEN_RACE_RESULT,         /* 1 */
    SCREEN_SEASON_STANDINGS,    /* 2: sScreenSeasonStandings */
    SCREEN_ROUND_OVER,          /* 3: sScreenRoundOver */
    SCREEN_CHEAT_ENTRY,         /* 4 */
    SCREEN_OPTIONS,             /* 5: sScreenOptions */
    SCREEN_SEASON,              /* 6: sScreenSeason */
    SCREEN_SPECIAL_TRACKS,      /* 7: sScreenSpecialTracks */
    SCREEN_MAIN_MENU,           /* 8: sScreenMainMenu */
    SCREEN_RIDER_SELECT,        /* 9: sScreenRiderSelect */
    SCREEN_BRONZE_TRACKS,       /* 10: sScreenBronzeTracks; the four track lists form a row */
    SCREEN_SILVER_TRACKS,       /* 11 */
    SCREEN_GOLD_TRACKS,         /* 12 */
    SCREEN_PLATINUM_TRACKS,     /* 13 */
    SCREEN_TITLE,               /* 14: sScreenTitle */
    SCREEN_PAK,                 /* 15: sScreenPak */
    SCREEN_RECORDS,             /* 16: sScreenRecords */
    SCREEN_CONTROLLER_SETTINGS, /* 17: sScreenControllerSettings */
    SCREEN_INITIALS,            /* 18 */
    SCREEN_BOOT_SPLASH,         /* 19: sScreenBootSplash, the first screen after power-on */
    SCREEN_SPLASH_2,            /* 20: sScreenSplash2 (with 21-23) */
    SCREEN_SPLASH_3,            /* 21 */
    SCREEN_PAK_STATUS,          /* 22 */
    SCREEN_BACKUP_LOST,         /* 23 */
    SCREEN_COUNT
};

struct Screen {
    /* 0x00 */ char *title;
    /* 0x04 */ Screen *left;      /* previous screen of a row (the track lists), or NULL */
    /* 0x08 */ Screen *right;     /* next screen of a row, or NULL */
    /* 0x0C */ ScreenItem *items;
    /* 0x10 */ u16 flags;         /* SCREEN_* */
    /* 0x12 */ s16 firstPic;      /* first entry of sMenuItemPicResIds for the items' pictures */
    /* 0x14 */ s16 firstItem;     /* the screen's first entry of items (the track lists share one list) */
    /* 0x16 */ s16 type;          /* SCREEN_TYPE_* */
    /* 0x18 */ s16 numItems;
    /* 0x1A */ u8 unk_1A[2];
    /* 0x1C */ s32 *bgResId;      /* background picture's resource id; SCREEN_TYPE_ROUND_SELECT: s32 *[7] */
    /* 0x20 */ s32 *cursor;       /* selected item (a field of gGame), or NULL */
    /* 0x24 */ Pic *bg;           /* the loaded background */
    /* 0x28 */ Screen *back;      /* where B goes */
}; /* size 0x2C */

/* sCheatTable: entering `code` on the cheat screen toggles bit n of sCheatMask (n = the entry's index) */
typedef struct Cheat {
    /* 0x00 */ char *name; /* shown when the cheat is entered */
    /* 0x04 */ char *code;
} Cheat; /* size 0x8 */

/* sRiderBios: the rider select screen's text and ratings of each rider */
typedef struct RiderBio {
    /* 0x00 */ char *name;
    /* 0x04 */ char *fullName;  /* with the nickname */
    /* 0x08 */ char *text[16];  /* [0] age, [1] hometown, [2] speciality (a trick's name in gRiderAnims), [3] "",
                                   [4..7] their labels, then the quote, a line each and NULL-terminated */
    /* 0x48 */ s32 stats[4];    /* index in sRiderStatRatings of each of the sRiderStatLabels */
} RiderBio; /* size 0x58 */

#endif
