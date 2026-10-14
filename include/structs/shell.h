/* The front end's screens (shell.c). sScreens holds the 24 screens in ScreenId order (the data names some of them
   again: sScreenMainMenu is sScreens[SCREEN_MAIN_MENU]). Each screen has a list of items; the screen's type selects
   its input handler in shell_update, what shell_load_screen loads and how shell_draw_screen draws it. */
#ifndef STRUCTS_SHELL_H
#define STRUCTS_SHELL_H

#include "common.h"

typedef struct Screen Screen;

/* ScreenItem.flags */
#define SCREEN_ITEM_LOCKED          0x0001 /* greyed out: choosing it plays the error sound */
#define SCREEN_ITEM_ASK_PLAYERS     0x0100 /* choosing it opens the players and difficulty popup */
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
    SCREEN_TYPE_SPLASH = 21,            /* boot splash, 120 frames */
    SCREEN_TYPE_SPLASH_2 = 22,          /* boot splash with the Expansion Pak picture blinking when present */
    SCREEN_TYPE_SPLASH_3 = 23,
    SCREEN_TYPE_PAK_STATUS = 24,        /* free pages of each Controller Pak */
    SCREEN_TYPE_BACKUP_LOST = 25
};

/* Index in sScreens */
enum ScreenId {
    SCREEN_SEASON_RACE_RESULT,  /* 0 */
    SCREEN_RACE_RESULT,         /* 1 */
    SCREEN_SEASON_STANDINGS,    /* 2: sScreenSeasonStandings */
    SCREEN_ROUND_OVER,          /* 3 */
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
    SCREEN_SPLASH_2,            /* 20 */
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

#endif
