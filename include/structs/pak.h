/* The Controller Pak screen (pak.c): notes of ghosts and user tracks, game code 'NMXG', company "01". */
#ifndef STRUCTS_PAK_H
#define STRUCTS_PAK_H

#include "common.h"

#define PAK_GAME_CODE 0x4E4D5847    /* 'NMXG' */

/* A note of a pak (sPakNoteTables, 16 per pak): its OSPfsState, then what the screen shows of it. */
typedef struct PakNote {
    /* 0x00 */ u32 size;            /* bytes */
    /* 0x04 */ u32 gameCode;
    /* 0x08 */ u16 companyCode;
    /* 0x0A */ char ext[4];         /* in ASCII once read (pak_pfs_to_ascii) */
    /* 0x0E */ char name[16];
    /* 0x1E */ s16 fileNo;          /* -1: no note */
    /* 0x20 */ u8 header[0x20];     /* the first bytes of one of the game's notes */
    /* 0x40 */ char label[0x30];    /* name, extension and what the describe callback says */
    /* 0x70 */ u8 status;           /* the describe callback's result */
    /* 0x71 */ u8 unk_71[3];
} PakNote; /* size 0x74 */

/* The screen (gPakScreen). The front end sets mode, pages, buf and the callbacks before pak_screen_init. */
typedef struct PakScreen {
    /* 0x00 */ s32 mode;            /* 0 manager, 1 load, 2 save */
    /* 0x04 */ s32 state;           /* 0-20, also the message shown (sPakMessages) */
    /* 0x08 */ s32 sub;             /* steps of the saving / loading / erasing states */
    /* 0x0C */ s32 pak;
    /* 0x10 */ s32 sel;             /* the note selected */
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 scroll;
    /* 0x1C */ s32 pages;           /* size of the data to save or load, in 256-byte pages */
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ u8 *buf;
    /* 0x28 */ s32 (*verify)(u8 *buf, s32 size);            /* nonzero: the loaded data is not valid */
    /* 0x2C */ s32 (*describe)(u8 *header, char *out, s32 inManager); /* 0 the game's, 1 foreign, 2 hidden */
} PakScreen; /* size 0x30 */

/* What a state shows (sPakMessages). */
typedef struct PakMessage {
    /* 0x00 */ char **lines;        /* a box of lines, NULL-terminated */
    /* 0x04 */ char *text;          /* else one line at the bottom */
    /* 0x08 */ char *buttons;       /* button hints, @X for the icon of button X */
} PakMessage; /* size 0xC */

#endif
