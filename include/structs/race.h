/* The running event: gEvent, the rules state of a race (event.c), and the track records it keeps. */
#ifndef STRUCTS_RACE_H
#define STRUCTS_RACE_H

#include "common.h"

/* A track's records: kept in each track (+0x34), in gGame.records, the save data and gEvent.record. */
typedef struct TrackRecord {
    /* 0x00 */ s32 times[3];        /* the three best race times (frames), best first */
    /* 0x0C */ s32 bestLap;
    /* 0x10 */ u8 initials[4][3];   /* of times[0-2] and bestLap; 0xFF 0xFF n: still to be entered by player n */
} TrackRecord; /* size 0x1C */

/* gEvent. state: 0-5 the start countdown, 6-12 soccer halves and overtime, 13-15 tutorial.
   flags: 0x10 timer paused, 0x40 tutorial demo (a replay), 0x800 message, 0x1000 takedown required, 0x2000 lesson
   done. */
typedef struct Event {
    /* 0x00 */ u32 state;
    /* 0x04 */ TrackRecord record;  /* the track's records, updated during the race */
    /* 0x20 */ s16 numLaps;
    /* 0x22 */ s16 numRiders;       /* gEventNumRiders */
    /* 0x24 */ s16 numFinished;     /* the next finishing place */
    /* 0x26 */ s16 countdown;
    /* 0x28 */ s16 messageTimer;
    /* 0x2A */ s16 unk_2A;
    /* 0x2C */ s32 time;            /* frames (gEventTime) */
    /* 0x30 */ s16 lapLeaderTimes[6]; /* the time (low half) the first rider completed each lap, -1 none */
    /* 0x3C */ s32 noPlayers;       /* a race of the front end or a replay: gGame.flags & 0x81 */
    /* 0x40 */ s32 allFinished;     /* gEventAllFinished */
    /* 0x44 */ s32 camTarget;       /* gCamTargetOverride */
    /* 0x48 */ s32 unk_48;          /* hill climb: -1 */
    /* 0x4C */ s32 idleFrames;      /* frames without input */
    /* 0x50 */ u8 finishOrder[8];   /* rider of each finishing place */
    /* 0x58 */ u32 flags;           /* gEventFlags */
    /* 0x5C */ u8 unk_5C[0x68 - 0x5C];
    /* 0x68 */ s16 goals[2];        /* soccer score */
    /* 0x6C */ s16 goalTimer;       /* soccer: 180 frames after a goal */
    /* 0x6E */ s8 unk_6E;           /* soccer */
    /* 0x6F */ u8 unk_6F;
    /* 0x70 */ s32 tutorialAction;
    /* 0x74 */ s32 unk_74;
    /* 0x78 */ s32 tutorialTimer;
    /* 0x7C */ void *tutorialBuf;
    /* 0x80 */ s32 tutorialTimeout;
    /* 0x84 */ s32 unk_84;
    /* 0x88 */ s32 finishTimer;     /* after the first player finished; -2 done */
} Event; /* size 0x8C */

#endif
