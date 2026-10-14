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

/* A cup of the season mode. gCups holds the five cups, then the custom round (gCustomRound) and the tutorial's
   lessons (sTutorialRound). */
typedef struct Cup {
    /* 0x00 */ u8 numRounds;
    /* 0x01 */ u8 progress[3];      /* per difficulty 1-3: the last round open (tutorial: lessons open) */
    /* 0x04 */ u8 won[4];           /* per difficulty: bit n, round n won */
    /* 0x08 */ s16 tracks[7];       /* gTracks index of each round */
    /* 0x16 */ s16 laps[7];         /* custom round: laps of each round */
} Cup; /* size 0x24 */

/* The season being raced: gSeason points at sSeasonState (NULL outside a season). */
typedef struct Season {
    /* 0x00 */ Cup *cup;
    /* 0x04 */ s16 points[6];       /* each rider's total */
    /* 0x10 */ s16 places[6][7];    /* each rider's place in each round */
    /* 0x64 */ s16 round;           /* tutorial: the lesson */
    /* 0x66 */ u8 racePoints[6];    /* each rider's points of the last race (gRacePoints) */
    /* 0x6C */ u8 standings[8];     /* the riders by points */
} Season; /* size 0x74 */

/* A step of a tutorial lesson: a message shown at an event time, or with flag 2 a trick to perform. */
typedef struct TutorialStep {
    /* 0x00 */ s32 value;           /* event time, or trick */
    /* 0x04 */ char *lines[3];
} TutorialStep; /* size 0x10 */

/* gTutorialLessons: the 17 lessons and tests of the tutorial (gSeason->round is the lesson). */
typedef struct TutorialLesson {
    /* 0x00 */ s32 unk_00[3];
    /* 0x0C */ u8 flags;            /* 2 trick steps, 4 a second rider (takedown), 8 the start countdown */
    /* 0x0D */ u8 level;            /* lessons of a level share it; gCups[6].progress[0] is the level open */
    /* 0x0E */ u8 unk_0E[2];
    /* 0x10 */ char *name;
    /* 0x14 */ s32 time;            /* gEvent.time at the start */
    /* 0x18 */ s16 startNode;       /* racing line nodes (gPathNodes) */
    /* 0x1A */ s16 goalNode;
    /* 0x1C */ s32 *demo;           /* the resource id of the demo replay, or NULL */
    /* 0x20 */ s32 numSteps;
    /* 0x24 */ TutorialStep *steps;
} TutorialLesson; /* size 0x28 */

/* The race HUD's glyph images (gGame.hudGlyphs, event_draw_glyph_string): an image set of res_load. */
typedef struct HudGlyphs {
    /* 0x00 */ s32 count;
    /* 0x04 */ u16 *palette;
    /* 0x08 */ Img glyphs[1];       /* by character */
} HudGlyphs;

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
    /* 0x6E */ s8 ballToucher;      /* soccer: the rider who touched the ball last */
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
