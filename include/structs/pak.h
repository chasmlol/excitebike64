/* pak.c: the Controller Pak screen (sScreens[SCREEN_PAK]), for the notes of ghosts and user tracks (game code
   PAK_GAME_CODE, company "01"). The shell sets the mode, the data and its size and the callbacks in gPakScreen, then
   polls gPakScreenDone and gPakScreenAborted. */
#ifndef STRUCTS_PAK_H
#define STRUCTS_PAK_H

#include "common.h"

/* the notes' game code: the cartridge's (the ROM header's) */
#if VERSION == VERSION_LODGENET
#define PAK_GAME_CODE    0x4E4D5847 /* 'NMXG' */
#elif VERSION == VERSION_US_REV1
#define PAK_GAME_CODE    0x4E4D5845 /* 'NMXE' */
#else
#error "PAK_GAME_CODE: the version's game code"
#endif
#define PAK_COMPANY_CODE 0x3031     /* "01" */
#define PAK_MAX_NOTES    16         /* notes of a Controller Pak */
#define PAK_PAGE_SIZE    0x100      /* bytes: notes are counted in pages */
#define PAK_LIST_ROWS    8          /* notes the list shows at once */

/* PakScreen.state: also the index of what the state shows in sPakMessages */
enum PakState {
    PAK_STATE_SELECT_PAK,        /* 0: choose a Controller Pak */
    PAK_STATE_DAMAGED,           /* 1: "... is damaged or corrupted": repair it? */
    PAK_STATE_REPAIRING,         /* 2 */
    PAK_STATE_REPAIRED,          /* 3 */
    PAK_STATE_REPAIR_FAILED,     /* 4: retry? */
    PAK_STATE_SELECT_NOTE,       /* 5 */
    PAK_STATE_CONFIRM_OVERWRITE, /* 6 */
    PAK_STATE_CONFIRM_ERASE,     /* 7 */
    PAK_STATE_PAK_FULL,          /* 8: enter the manager? */
    PAK_STATE_LOADING,           /* 9 */
    PAK_STATE_SAVING,            /* 10 */
    PAK_STATE_ERROR,             /* 11: an error while communicating with the Controller Pak */
    PAK_STATE_NO_NOTES,          /* 12: the pak is empty, or has none of the notes to load */
    PAK_STATE_ERASING,           /* 13 */
    PAK_STATE_NO_PAK,            /* 14: no Controller Pak inserted */
    PAK_STATE_SAVED,             /* 15 */
    PAK_STATE_LOADED,            /* 16 */
    PAK_STATE_CANCEL_SAVE,       /* 17: "Cancel Save?" */
    PAK_STATE_CANCEL_LOAD,       /* 18: "Cancel Load?" */
    PAK_STATE_INVALID_NOTE,      /* 19: the loaded note did not validate: erase it? */
    PAK_STATE_RUMBLE_NOTICE,     /* 20: "If you have any Rumble Paks you wish to use, insert them now." */
    PAK_STATE_COUNT
};

/* PakScreen.subState of the states that work over several frames (repairing, loading, saving, erasing) */
#define PAK_STEP_START 0
#define PAK_STEP_WAIT  1 /* the message stays up for 30 frames first */
#define PAK_STEP_RUN   2

/* PakDescribeFunc results, PakNote.status */
#define PAK_NOTE_OURS    0
#define PAK_NOTE_FOREIGN 1
#define PAK_NOTE_HIDDEN  2

/* A note of a Controller Pak as the pak screen lists it (sPakNoteTables: 16 per Controller Pak; sPakNoteList: the
   notes listed, after sPakNewNoteEntry when saving). The first fields are an OSPfsState. */
typedef struct PakNote {
    /* 0x00 */ u32 size;            /* bytes */
    /* 0x04 */ u32 gameCode;        /* PAK_GAME_CODE for this game's notes */
    /* 0x08 */ u16 companyCode;
    /* 0x0A */ char ext[4];         /* in ASCII once read (pak_pfs_to_ascii) */
    /* 0x0E */ char name[16];       /* in ASCII */
    /* 0x1E */ s16 fileNo;          /* -1: no note */
    /* 0x20 */ u8 header[0x20];     /* the first bytes of one of the game's notes */
    /* 0x40 */ char label[0x30];    /* the line of the list: name, extension and what the describe callback says */
    /* 0x70 */ u8 status;           /* PAK_NOTE_OURS or PAK_NOTE_FOREIGN */
    /* 0x71 */ u8 unk_71[3];
} PakNote; /* size 0x74 */

/* PakScreen.mode */
#define PAK_MODE_MANAGER 0          /* list and delete notes */
#define PAK_MODE_LOAD    1
#define PAK_MODE_SAVE    2

/* Nonzero: the loaded data is not valid. */
typedef s32 (*PakValidateFunc)(u8 *data, s32 size);
/* Writes what the list shows of one of the game's notes (from its header) to out; returns a PAK_NOTE_*: hidden
   for a note of the other kind when loading. */
typedef s32 (*PakDescribeFunc)(u8 *header, char *out, s32 inManager);

/* The screen (gPakScreen). The front end sets mode, pages, data and the callbacks before pak_screen_init. */
typedef struct PakScreen {
    /* 0x00 */ s32 mode;            /* PAK_MODE_* */
    /* 0x04 */ s32 state;           /* PAK_STATE_* */
    /* 0x08 */ s32 subState;        /* PAK_STEP_* of the repairing / loading / saving / erasing states */
    /* 0x0C */ s32 pak;             /* the Controller Pak shown */
    /* 0x10 */ s32 sel;             /* the note selected */
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 scroll;          /* first note of the list shown */
    /* 0x1C */ s32 pages;           /* size of the data to save or load, in 256-byte pages */
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ u8 *data;            /* what is saved, or where a note is loaded */
    /* 0x28 */ PakValidateFunc validate; /* loading; NULL accepts anything */
    /* 0x2C */ PakDescribeFunc describe;
} PakScreen; /* size 0x30 */

/* What a state shows (sPakMessages). */
typedef struct PakMessage {
    /* 0x00 */ char **lines;        /* a box of lines, NULL-terminated */
    /* 0x04 */ char *text;          /* else one line at the bottom */
    /* 0x08 */ char *buttons;       /* button hints, @X for the icon of button X */
} PakMessage; /* size 0xC */

#endif
