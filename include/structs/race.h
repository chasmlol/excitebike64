/* The race: gEvent, the rules state of a race (event.c), the track records, the season (gSeason, gCups), the
   tutorial lessons, the pause screen and the HUD's glyph images. */
#ifndef STRUCTS_RACE_H
#define STRUCTS_RACE_H

#include "common.h"

/* A track's records: kept in each track (+0x34), in gGame.records, the save data and gEvent.record. */
typedef struct TrackRecord {
    /* 0x00 */ s32 times[3];        /* the three best race times (frames), best first */
    /* 0x0C */ s32 bestLap;
    /* 0x10 */ u8 initials[4][3];   /* of times[0-2] and bestLap; 0xFF 0xFF n: still to be entered by player n */
} TrackRecord; /* size 0x1C */

/* The tracks of gTracks (27 records of 0xA0 bytes), in table order. */
enum TrackId {
    TRACK_KYOTO,                /* Kyoto, Japan */
    TRACK_MOUNTAIN_QUARRY,
    TRACK_HOUSTON,              /* Houston, TX */
    TRACK_LEFTYS_MILL,
    TRACK_ORLANDO,              /* Orlando, FL */
    TRACK_NASHVILLE,            /* Nashville, TN */
    TRACK_CANYON_CHASM,
    TRACK_LONG_ISLAND,          /* Long Island, NY */
    TRACK_CONGO_COURSE,
    TRACK_LAS_VEGAS,            /* Las Vegas, NV */
    TRACK_PHOENIX,              /* Phoenix, AZ */
    TRACK_RAINFOREST_RUN,
    TRACK_MADRID,               /* Madrid, Spain */
    TRACK_GRAVEL_PIT,           /* The Gravel Pit */
    TRACK_DETROIT,              /* Detroit, MI */
    TRACK_GOLDMINE_RUSH,
    TRACK_LOS_ANGELES,          /* Los Angeles, CA */
    TRACK_CONSTRUCTION_YARD,
    TRACK_SEATTLE,              /* Seattle, WA */
    TRACK_BLIZZARD_BLITZ,
    TRACK_EXCITE_3D,
    TRACK_USER,                 /* User Track: the editor's track */
    TRACK_SOCCER,
    TRACK_HILL_CLIMB,
    TRACK_STUNT_COURSE,
    TRACK_DESERT,
    TRACK_TUTORIAL,
    TRACK_COUNT
};

/* A gTracks record's environment (+0x2C): which code draws it. */
#define TRACK_ENV_STADIUM   0   /* arngfx.c */
#define TRACK_ENV_OUTDOOR   1   /* tubegfx.c */
#define TRACK_ENV_DESERT    2   /* desgfx.c */

/* A gTracks record's flags (+0x50). */
#define TRACK_FLAG_USER         0x002   /* the user track: built by UsrTrk.c, keeps no records */
#define TRACK_FLAG_NEW_LAP      0x080   /* this race set a best lap: the initials are asked for */
#define TRACK_FLAG_NEW_TIME     0x100   /* ... a best race time */
#define TRACK_FLAG_RECORDS      0x200   /* keeps records (gGame.records, the save data) */

/* gRacePhase: the track intro (track preview) and the race, each faded in and out by render_draw_race. */
#define RACE_PHASE_INTRO_FADE_IN    0
#define RACE_PHASE_INTRO            1   /* B goes back, A or START races */
#define RACE_PHASE_INTRO_FADE_OUT   2
#define RACE_PHASE_INTRO_DONE       3   /* a cleared frame, then the race is set up */
#define RACE_PHASE_FADE_IN          4
#define RACE_PHASE_RACING           5
#define RACE_PHASE_FADE_OUT         6   /* event_request_exit */
#define RACE_PHASE_DONE             7   /* a cleared frame, then leave or restart */

/* gRacePhaseTimer during a fade: -1 to start one (render_draw_race sets 20), counts down to 0, then -2 and -3 when
   the fade out is over. */
#define PHASE_TIMER_START   -1
#define PHASE_TIMER_FADED   -3

/* gRaceExitCode: how the last race ended, for shell_init. */
#define RACE_EXIT_BOOT      0   /* none yet: the game just started */
#define RACE_EXIT_FINISHED  2
#define RACE_EXIT_BACK      3   /* B on the track intro */
#define RACE_EXIT_QUIT      4   /* Quit from the pause menu */
#define RACE_EXIT_NONE      6   /* the default while racing */
#define RACE_EXIT_PODIUM    7   /* after the podium ceremony */
#define RACE_EXIT_EDITOR    8   /* the test ride's "Return to Editor" */

/* gPauseMenu: the menu shown over the race (gPauseMenuCursor is its item). */
#define PAUSE_NONE          0
#define PAUSE_MAIN          1   /* PAUSE_ITEM_* */
#define PAUSE_CONTROLLER    2   /* the controller setup screen */
#define PAUSE_OPTIONS       3   /* the sound options screen */
#define PAUSE_QUIT          4   /* Really Quit / Just Kidding */
#define PAUSE_TUTORIAL      5   /* TUTORIAL_ITEM_* */
#define PAUSE_TEST_RIDE     6   /* TEST_RIDE_ITEM_* */

/* The items of the pause menu (sPauseMenuItems; the season's has Quit Season and no Restart Race). */
#define PAUSE_ITEM_CONTROLLER   0
#define PAUSE_ITEM_OPTIONS      1
#define PAUSE_ITEM_CONTINUE     2
#define PAUSE_ITEM_QUIT         3
#define PAUSE_ITEM_RESTART      4

/* The items of the tutorial's menu (sTutorialMenuItems). */
#define TUTORIAL_ITEM_RETRY     0
#define TUTORIAL_ITEM_DEMO      1
#define TUTORIAL_ITEM_EXIT      2
#define TUTORIAL_ITEM_NEXT      3
#define TUTORIAL_ITEM_PREV      4

/* The items of the test ride's menu (sTestRideMenuItems). */
#define TEST_RIDE_ITEM_CONTINUE 0
#define TEST_RIDE_ITEM_RESET    1   /* Reset To Start */
#define TEST_RIDE_ITEM_EDITOR   2   /* Return to Editor */

/* The items of the pause menu's options screen (gOptionsCursor): the three volume bars (SaveData.volumes), then
   mono/stereo. */
#define OPTION_MUSIC        0
#define OPTION_EFFECTS      1
#define OPTION_SPEECH       2
#define OPTION_STEREO       3

/* A cup of the season mode. gCups holds the five cups, then the custom round (gCups[5]) and the tutorial's
   lessons (gCups[6]). */
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

/* TutorialLesson.flags */
#define LESSON_TRICKS       0x02    /* the steps are tricks to perform, in order */
#define LESSON_TAKEDOWN     0x04    /* a second rider to knock down */
#define LESSON_COUNTDOWN    0x08    /* starts with the countdown, then the clock runs */

#define TUTORIAL_NUM_LESSONS    17

/* gTutorialLessons: the 17 lessons and tests of the tutorial (gSeason->round is the lesson). */
typedef struct TutorialLesson {
    /* 0x00 */ s32 unk_00[3];
    /* 0x0C */ u8 flags;            /* LESSON_*: 2 trick steps, 4 a second rider (takedown), 8 the start
                                       countdown */
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

/* gPauseScreen: the race's options and controller screens of the pause menu (loaded by race_update, drawn by
   render.c). */
typedef struct PauseScreen {
    /* 0x00 */ s16 timer;           /* > 0 frames before the screen shows, -1 shown */
    /* 0x02 */ u8 clearFrames;      /* frames render_draw_race clears the screen for after it closes */
    /* 0x03 */ u8 loaded;
    /* 0x04 */ Img *background;     /* GUI image sets: entry 0 holds the palette, the images follow */
    /* 0x08 */ Img *volumeBar;
    /* 0x0C */ Img *title;
    /* 0x10 */ Img *cursorImg;      /* 30 frames */
    /* 0x14 */ u8 *cursorPal;
} PauseScreen; /* size 0x18 */

/* The race HUD's glyph images (gGame.hudGlyphs, event_draw_glyph_string): an image set of res_load. */
typedef struct HudGlyphs {
    /* 0x00 */ s32 count;
    /* 0x04 */ u16 *palette;
    /* 0x08 */ Img glyphs[1];       /* by character */
} HudGlyphs;

/* Event.state */
#define EVENT_STATE_WAIT        0   /* before the countdown */
#define EVENT_STATE_READY       1
#define EVENT_STATE_COUNT_3     2
#define EVENT_STATE_COUNT_2     3
#define EVENT_STATE_COUNT_1     4
#define EVENT_STATE_GO          5   /* racing */
#define EVENT_STATE_KICKOFF     6   /* soccer: before the first half */
#define EVENT_STATE_FIRST_HALF  7
#define EVENT_STATE_HALF_TIME   8
#define EVENT_STATE_SECOND_HALF 9
#define EVENT_STATE_DRAW        10  /* "Full Time Draw : Overtime Required" */
#define EVENT_STATE_OVERTIME    11
#define EVENT_STATE_FINAL       12
#define EVENT_STATE_TITLE       13  /* tutorial: the lesson's title */
#define EVENT_STATE_DEMO        14  /* ... the demo replay */
#define EVENT_STATE_LESSON      15  /* ... the player's turn */

/* Event.flags */
#define EVENT_SCORED            0x0000F /* soccer: the team that scored (1 or 2), while goalTimer runs */
#define EVENT_STOPPED           0x00010 /* the clock is stopped (countdown, half time, after a goal) */
#define EVENT_OWN_GOAL          0x00020 /* soccer: the scoring team did not touch the ball last */
#define EVENT_DEMO              0x00040 /* tutorial: the demo replay plays */
#define EVENT_CAM_RESET         0x00080 /* tutorial: the riders were reset, cut the camera */
#define EVENT_DUEL              0x00200 /* the season's fifth cup: one rider against another, no drones */
#define EVENT_MESSAGE           0x00800 /* tutorial: a message holds the game (tutorialTimeout) */
#define EVENT_TAKEDOWN          0x01000 /* tutorial: the takedown is still to do */
#define EVENT_RIDER_STOPPED     0x02000 /* rider 0 finished and came to a stop */
#define EVENT_NO_DEMO           0x04000 /* tutorial: skip the demo */
#define EVENT_INTRO_MESSAGE     0x08000 /* tutorial: the message is the lesson's first step, before the start */
#define EVENT_CAM_UPDATE        0x10000 /* update the camera once while a message holds the game */

/* Tutorial sub-states (Event.tutorialAction): the title and the demo slide in, wait and slide out; the pause
   menu's choices are kept here until the fade out is over. */
#define TUTORIAL_SHOW       0   /* sliding in (tutorialTimer) */
#define TUTORIAL_WAIT       1
#define TUTORIAL_HIDE       2   /* sliding out */
#define TUTORIAL_PREV       3   /* go to the previous lesson */
#define TUTORIAL_NEXT       4   /* go to the next lesson */
#define TUTORIAL_PLAY       5   /* the player rides the lesson */
#define TUTORIAL_RETRY      6   /* ride the lesson again, without the demo */
#define TUTORIAL_DEMO       7   /* watch the demo */

/* gEvent: the rules state of the running event (event.c). */
typedef struct Event {
    /* 0x00 */ u32 state;           /* EVENT_STATE_*: 0-5 the start countdown, 6-12 soccer, 13-15 tutorial */
    /* 0x04 */ TrackRecord record;  /* the track's records, updated during the race */
    /* 0x20 */ s16 numLaps;
    /* 0x22 */ s16 numRiders;
    /* 0x24 */ s16 numFinished;     /* the next finishing place */
    /* 0x26 */ s16 countdown;
    /* 0x28 */ s16 messageTimer;
    /* 0x2A */ s16 unk_2A;
    /* 0x2C */ s32 time;            /* frames (also gEventTime) */
    /* 0x30 */ s16 lapLeaderTimes[6]; /* the time (low half) the first rider completed each lap, -1 none */
    /* 0x3C */ s32 noPlayers;       /* gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO) */
    /* 0x40 */ s32 allFinished;
    /* 0x44 */ s32 camTarget;       /* gCamTargetOverride */
    /* 0x48 */ s32 unk_48;          /* hill climb: -1 */
    /* 0x4C */ s32 idleFrames;      /* frames without input */
    /* 0x50 */ u8 finishOrder[8];   /* rider of each finishing place */
    /* 0x58 */ u32 flags;           /* EVENT_*; also gEventFlags */
    /* 0x5C */ u8 unk_5C[0x68 - 0x5C];
    /* 0x68 */ s16 goals[2];        /* soccer score */
    /* 0x6C */ s16 goalTimer;       /* soccer: 180 frames after a goal */
    /* 0x6E */ s8 ballToucher;      /* soccer: the rider who touched the ball last */
    /* 0x6F */ u8 unk_6F;
    /* 0x70 */ s32 tutorialAction;  /* TUTORIAL_* */
    /* 0x74 */ s32 unk_74;
    /* 0x78 */ s32 tutorialTimer;
    /* 0x7C */ void *tutorialBuf;
    /* 0x80 */ s32 tutorialTimeout; /* frames the message stays up (EVENT_MESSAGE) */
    /* 0x84 */ s32 unk_84;
    /* 0x88 */ s32 finishTimer;     /* after the first player finished; -2 done */
} Event; /* size 0x8C */

#endif
