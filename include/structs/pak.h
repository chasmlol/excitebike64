/* pak.c: the Controller Pak screen (sScreens[SCREEN_PAK]), for the notes of ghosts and user tracks (game code
   'NMXG', company "01"). The shell sets the mode, the data and its size and the callbacks in gPakScreen, then polls
   gPakScreenDone and gPakScreenAborted. */
#ifndef STRUCTS_PAK_H
#define STRUCTS_PAK_H

#include "common.h"

#define PAK_GAME_CODE 0x4E4D5847    /* 'NMXG' */

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
    /* 0x70 */ u8 status;           /* the describe callback's result */
    /* 0x71 */ u8 unk_71[3];
} PakNote; /* size 0x74 */

/* PakScreen.mode */
#define PAK_MODE_MANAGER 0          /* list and delete notes */
#define PAK_MODE_LOAD    1
#define PAK_MODE_SAVE    2

/* Nonzero: the loaded data is not valid. */
typedef s32 (*PakValidateFunc)(u8 *data, s32 size);
/* Writes what the list shows of one of the game's notes (from its header) to out; returns 0 the game's, 1 foreign,
   2 hidden. */
typedef s32 (*PakDescribeFunc)(u8 *header, char *out, s32 inManager);

/* The screen (gPakScreen). The front end sets mode, pages, data and the callbacks before pak_screen_init. */
typedef struct PakScreen {
    /* 0x00 */ s32 mode;            /* PAK_MODE_* */
    /* 0x04 */ s32 state;           /* 0-20, also the index of the state's message in sPakMessages */
    /* 0x08 */ s32 subState;        /* steps of the saving / loading / erasing states */
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
