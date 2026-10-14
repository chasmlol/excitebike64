/* pak.c: the Controller Pak screen (sScreens[SCREEN_PAK]). The shell sets the mode, the data and its size and the
   callbacks in gPakScreen, then polls gPakScreenDone and gPakScreenAborted. */
#ifndef STRUCTS_PAK_H
#define STRUCTS_PAK_H

#include "common.h"

/* PakScreen.mode */
#define PAK_MODE_MANAGER 0 /* list and delete notes */
#define PAK_MODE_LOAD    1
#define PAK_MODE_SAVE    2

typedef s32 (*PakValidateFunc)(u8 *data, s32 size);
typedef s32 (*PakLabelFunc)(u8 *data, char *dst, s32 nameOnly);

typedef struct PakScreen {
    /* 0x00 */ s32 mode;              /* PAK_MODE_* */
    /* 0x04 */ s32 state;             /* also the index of the state's message in sPakMessages */
    /* 0x08 */ s32 subState;
    /* 0x0C */ s32 pak;               /* the Controller Pak shown */
    /* 0x10 */ s32 sel;               /* selected note */
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 scroll;            /* first note of the list shown */
    /* 0x1C */ s32 pages;             /* size of the data in pages of 256 bytes */
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ u8 *data;              /* what is saved, or where a note is loaded */
    /* 0x28 */ PakValidateFunc validate; /* loading: nonzero rejects the note's data; NULL accepts anything */
    /* 0x2C */ PakLabelFunc label;    /* the text of a note in the list */
} PakScreen; /* size 0x30 */

/* A note of a Controller Pak as the pak screen lists it (sPakNoteTables: 16 per Controller Pak; sPakNoteList: the
   notes listed, after sPakNewNoteEntry when saving). The first fields are an OSPfsState. */
typedef struct PakNote {
    /* 0x00 */ u32 size;          /* bytes */
    /* 0x04 */ u32 gameCode;      /* 'NMXG' for this game's notes */
    /* 0x08 */ u16 companyCode;
    /* 0x0A */ char ext[4];       /* in ASCII (pak_pfs_to_ascii) */
    /* 0x0E */ char name[16];     /* in ASCII */
    /* 0x1E */ s16 fileNo;        /* -1: no note */
    /* 0x20 */ u8 header[0x20];   /* the first bytes of this game's notes */
    /* 0x40 */ char text[0x30];   /* the line of the list */
    /* 0x70 */ u8 status;         /* what PakScreen.label returned */
    /* 0x71 */ u8 pad71[3];
} PakNote; /* size 0x74 */

#endif
