#include "game.h"

#define gDPRaw(pkt, a, b) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = (a); \
    _g->words.w1 = (b); \
}

/* the lesson being ridden */
#define LESSON (gTutorialLessons[gSeason->round])

#define HUD_NUM_TEXTS 30 /* gHudTextObjs, gMenuTextObjs */

/* Sets the event up for the current tutorial lesson: the takedown, the countdown and its message, the clock, and
   the demo when one plays. */
void tutorial_init_lesson(s32 first)
{
    Event *event;

    race_reset_grid_order();
    event = &gEvent;
    if (event->tutorialBuf == 0) {
        event->tutorialBuf = mem_alloc_dbg(0x80, 0, (s32)sStrEventFile, 0x16D);
    }
    gGame.difficulty = 1;
    if (LESSON.flags & LESSON_TAKEDOWN) {
        event->time = 0;
        event->flags |= EVENT_TAKEDOWN;
    } else {
        event->flags &= ~EVENT_TAKEDOWN;
    }
    if (LESSON.flags & LESSON_COUNTDOWN) {
        gEvent.countdown = 0x3F;
        gEvent.state = EVENT_STATE_READY;
        gEvent.flags |= EVENT_STOPPED;
    } else {
        gEvent.flags &= ~EVENT_STOPPED;
    }
    gEvent.flags &= ~EVENT_NO_DEMO;
    if (LESSON.flags & LESSON_COUNTDOWN) {
        gEvent.flags |= EVENT_CAM_UPDATE | EVENT_INTRO_MESSAGE | EVENT_MESSAGE;
        gEvent.tutorialTimeout = 600;
        if (LESSON.flags & LESSON_TRICKS) {
            gEvent.time = 0;
        } else if (gActiveReplay == 0) {
            gEvent.time = LESSON.time;
        }
    } else {
        gEvent.state = EVENT_STATE_FREE_RIDE;
        gEvent.time = 0;
    }
    if (gActiveReplay == 0) {
        gEvent.tutorialAction = TUTORIAL_PLAY;
        gEvent.flags &= ~EVENT_DEMO;
    } else {
        gEvent.countdown = 0x3F;
        gEvent.tutorialAction = TUTORIAL_SHOW;
        gEvent.tutorialTimer = 0x3C;
        gEvent.flags |= EVENT_DEMO;
    }
    sTutorialStep = 0;
    gTutorialResult = 0;
    gTutorialMenuForced = 0;
}

/* C-left on controller 1 goes to the previous lesson, C-right to the next one if it is open. */
void tutorial_check_lesson_skip(void)
{
    if (gRacePhase == RACE_PHASE_RACING) {
        if (((gPadInput & INPUT_C_LEFT) != 0) && ((gPadInputPrev & INPUT_C_LEFT) == 0)) {
            if (gSeason->round < 1) {
                return;
            }
            gEvent.tutorialAction = TUTORIAL_PREV;
            gRacePhase = RACE_PHASE_FADE_OUT;
            gRacePhaseTimer = PHASE_TIMER_START;
            snd_play_menu_sound(MENU_SFX_SELECT);
            return;
        }
        if ((gPadInput & INPUT_C_RIGHT) == 0) {
            return;
        }
        if ((gPadInputPrev & INPUT_C_RIGHT) != 0) {
            return;
        }
        if (tutorial_next_lesson_unlocked() == 0) {
            return;
        }
        gEvent.tutorialAction = TUTORIAL_NEXT;
        gRacePhase = RACE_PHASE_FADE_OUT;
        gRacePhaseTimer = PHASE_TIMER_START;
        snd_play_menu_sound(MENU_SFX_SELECT);
    }
}

/* racing line node i */
#define NODE(i) ((PathNode *)((s32)((i) * 0x6C) + (s32)gPathNodes))

/* The tutorial's rules: the countdown and its message, the clock, the steps' messages during a demo, the title,
   the demo, and the lesson's goal (a racing line node, the takedown of the second rider, or ten tricks in order).
   Rider flag 0x10 makes a rider brake to a stop. */
void tutorial_update(void)
{
    s32 i;

    gRiders[1].timeScale = 1.0375f;
    if (!(gEvent.flags & EVENT_MESSAGE)) {
        if ((LESSON.flags & LESSON_COUNTDOWN) &&
            (gEvent.state < EVENT_STATE_GO || gEvent.state == EVENT_STATE_TIMED || gEvent.state == EVENT_STATE_FREE_RIDE)) {
            event_update_countdown();
        }
        if (gEvent.countdown == 0x2B && gEvent.state == EVENT_STATE_READY && (gEvent.flags & EVENT_INTRO_MESSAGE)) {
            gEvent.tutorialTimeout = 300;
            gEvent.flags |= EVENT_CAM_UPDATE | EVENT_MESSAGE;
        }
    } else {
        snd_stop_all_riders();
    }
    if (gEvent.state == EVENT_STATE_GO) {
        gEvent.flags &= ~EVENT_STOPPED;
        if (gActiveReplay != 0 && gRiderAlpha_2 != 0xFF) {
            gEvent.tutorialAction = TUTORIAL_WAIT;
            gEvent.state = EVENT_STATE_FREE_RIDE;
        } else {
            gEvent.state = EVENT_STATE_TIMED;
            gEvent.tutorialAction = TUTORIAL_PLAY;
        }
    }
    if (gEvent.state >= EVENT_STATE_GO && !(gEvent.flags & EVENT_MESSAGE)) {
        if (gActiveReplay == 0) {
            /* tricks count up; a timed lesson counts down until rider 0 finishes */
            if (LESSON.flags & LESSON_TRICKS) {
                gEvent.time++;
            } else if (!(gRiders[0].flags & 0x10)) {
                gEvent.time--;
            }
        } else {
            /* the demo stops at each step's time for its message */
            for (i = 0; i < LESSON.numSteps; i++) {
                if (gEvent.time == LESSON.steps[i].value) {
                    snd_stop_all_riders();
                    rumble_stop_all();
                    gEvent.tutorialTimeout = 1800;
                    gEvent.flags |= EVENT_MESSAGE;
                    break;
                }
            }
            gEvent.time++;
        }
        if (gEvent.allFinished == 0) {
            gEvent.messageTimer = 180;
        } else if (gEvent.messageTimer != 0) {
            gEvent.messageTimer--;
        }
    }
    if ((gEvent.tutorialAction == TUTORIAL_SHOW || gEvent.tutorialAction == TUTORIAL_HIDE) && gEvent.tutorialTimer != 0) {
        gEvent.tutorialTimer--;
    }
    if (gEvent.state < EVENT_STATE_GO) {
        tutorial_check_lesson_skip();
        return;
    }
    switch (gEvent.state) {
    case EVENT_STATE_TITLE:
        switch (gEvent.tutorialAction) {
        case TUTORIAL_SHOW:
            if ((gPadInput & INPUT_A) && !(gPadInputPrev & INPUT_A)) {
                gEvent.tutorialAction = TUTORIAL_HIDE;
                gEvent.tutorialTimer = 59 - gEvent.tutorialTimer;
            } else if (gEvent.tutorialTimer == 0) {
                gEvent.tutorialAction = TUTORIAL_WAIT;
            }
            break;
        case TUTORIAL_WAIT:
            if ((gPadInput & INPUT_A) && !(gPadInputPrev & INPUT_A)) {
                gEvent.tutorialAction = TUTORIAL_HIDE;
                gEvent.tutorialTimer = 60;
                gEvent.flags = (gEvent.flags & ~EVENT_STOPPED) | EVENT_DEMO;
            } else {
                tutorial_check_lesson_skip();
            }
            break;
        case TUTORIAL_HIDE:
            if (gEvent.tutorialTimer == 0) {
                gEvent.state = EVENT_STATE_FREE_RIDE;
                gEvent.countdown = 0x3F;
                gEvent.tutorialAction = TUTORIAL_SHOW;
                gEvent.tutorialTimer = 60;
            }
            break;
        }
        break;
    case EVENT_STATE_FREE_RIDE:
        switch (gEvent.tutorialAction) {
        case TUTORIAL_SHOW:
            if (gEvent.tutorialTimer == 0) {
                gEvent.tutorialAction = TUTORIAL_WAIT;
                return;
            }
            break;
        case TUTORIAL_WAIT:
            /* the demo: START skips to the lesson; at the goal it starts over */
            if ((gPadInput & INPUT_START) && !(gPadInputPrev & INPUT_START)) {
                gEvent.tutorialAction = TUTORIAL_RETRY;
                gRacePhase = RACE_PHASE_FADE_OUT;
                gRacePhaseTimer = PHASE_TIMER_START;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
                return;
            }
            if (gRiders[0].segment == LESSON.goalNode) {
                if (gRiders[0].flags & 0x10) {
                    if (gEvent.flags & EVENT_RIDER_STOPPED) {
                        bikes_free();
                        decal_clear_all();
                        mem_fill((u8 *)gRiders, 0, sizeof(Rider) * 8);
                        tutorial_init_lesson(0);
                        bikes_init();
                        replay_play_start(gActiveReplay);
                        gEvent.flags = (gEvent.flags | EVENT_CAM_RESET) & ~EVENT_RIDER_STOPPED;
                    }
                } else {
                    gRiders[0].flags |= 0x10;
                    gEvent.flags &= ~EVENT_RIDER_STOPPED;
                }
                return;
            }
            if ((LESSON.flags & LESSON_TAKEDOWN) && gRiders[1].segment >= LESSON.goalNode &&
                (LESSON.startNode < LESSON.goalNode || (gRiders[1].segment < LESSON.startNode - 2 && LESSON.goalNode < LESSON.startNode))) {
                gRiders[1].flags |= 0x10;
                return;
            }
            break;
        case TUTORIAL_PLAY:
            /* a lesson without the countdown: at the goal, stop and show the menu */
            if (gRiders[0].segment == LESSON.goalNode) {
                gRiders[0].flags |= 0x10;
            }
            if ((gEvent.flags & EVENT_RIDER_STOPPED) && gPauseMenu != PAUSE_TUTORIAL) {
                gPauseMenuCursor = TUTORIAL_ITEM_RETRY;
                gPauseMenu = PAUSE_TUTORIAL;
                gTutorialMenuForced = 1;
            }
            break;
        }
        tutorial_check_lesson_skip();
        break;
    case EVENT_STATE_TIMED:
        if (LESSON.flags & LESSON_TRICKS) {
            /* steps 2-9 are the tricks to perform; 10 and 11 the end, A or START goes on */
            if (gEvent.time >= 3 && sTutorialStep == 0) {
                sTutorialStep = 2;
            }
            if (gRiders[0].unk_3FB == 0 && gRiders[0].trick == LESSON.steps[sTutorialStep].value) {
                sTutorialStep++;
            }
            if (sTutorialStep >= 10) {
                if (gEvent.flags & EVENT_RIDER_STOPPED) {
                    gTutorialResult = 1;
                    if (((gPadInput & INPUT_START) && !(gPadInputPrev & INPUT_START)) ||
                        ((gPadInput & INPUT_A) && !(gPadInputPrev & INPUT_A))) {
                        if (sTutorialStep == 10) {
                            sTutorialStep = 11;
                        } else if (sTutorialStep == 11) {
                            gSeason->cup->progress[0] = LESSON.level + 1;
                            gSaveData.tutorialProgress = gCups[6].progress[0];
                            event_request_exit();
                            gRaceExitCode = RACE_EXIT_FINISHED;
                            gPauseMenu = PAUSE_NONE;
                            gGame.flags &= ~GAME_REPEAT_STATE;
                            return;
                        }
                    }
                    if (sTutorialStep < 10) {
                        sTutorialStep = 10;
                    }
                } else if (!(gRiders[0].flags & 0x10)) {
                    gRiders[0].flags |= 0x10;
                }
            }
        }
        tutorial_check_lesson_skip();
        /* back two nodes before the start: the rider went the wrong way */
        if (gRiders[0].segment == NODE(NODE(LESSON.startNode)->prev)->prev && !(LESSON.flags & LESSON_TRICKS)) {
            if (event_request_exit() != 0) {
                gEvent.tutorialAction = TUTORIAL_RETRY;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
            }
            return;
        }
        if ((u32)(gRiders[0].segment - LESSON.goalNode) < 10 || (gEvent.flags & EVENT_RIDER_STOPPED)) {
            /* past the goal: stop, open the next level if this was its last lesson, then the menu */
            if (gRiders[0].flags & 0x10) {
                if (gSeason->round < TUTORIAL_NUM_LESSONS - 1 && LESSON.level != gTutorialLessons[gSeason->round + 1].level &&
                    tutorial_lesson_passed_2()) {
                    if (LESSON.level == gSeason->cup->progress[0]) {
                        gSeason->cup->progress[0] = LESSON.level + 1;
                        gSaveData.tutorialProgress = gCups[6].progress[0];
                    }
                }
                if ((gEvent.flags & EVENT_RIDER_STOPPED) && sTutorialStep == 0) {
                    snd_stop_all_riders();
                    gEvent.flags &= ~EVENT_RIDER_STOPPED;
                    gTutorialResult = tutorial_lesson_passed_2() ? 1 : -1;
                    gPauseMenu = PAUSE_TUTORIAL;
                    gPauseMenuCursor = tutorial_lesson_passed_2() ? TUTORIAL_ITEM_NEXT : TUTORIAL_ITEM_RETRY;
                }
            } else {
                gRiders[0].flags |= 0x10;
            }
        } else if (gRiders[0].segment == NODE(NODE(LESSON.startNode)->prev)->prev && !(LESSON.flags & LESSON_TRICKS)) {
            gEvent.tutorialAction = TUTORIAL_RETRY;
            gRacePhase = RACE_PHASE_FADE_OUT;
            gRacePhaseTimer = PHASE_TIMER_START;
            gActiveReplay = 0;
            gDemoPlayback = 0;
            sDemoActive = 0;
            gReplayRecording = 0;
        } else if ((LESSON.flags & LESSON_TAKEDOWN) && gRiders[1].segment >= LESSON.goalNode &&
                   (LESSON.startNode < LESSON.goalNode || (gRiders[1].segment < LESSON.startNode - 2 && LESSON.goalNode < LESSON.startNode))) {
            gRiders[1].flags |= 0x10;
        }
        break;
    }
}

/* gCurTrack, a gTracks record */
typedef struct {
    u8 pad[0x34];
    TrackRecord record; /* 0x34 */
    s32 flags;          /* 0x50 TRACK_FLAG_* */
    u8 pad54[0xA0 - 0x54];
} TrackRecordInfo;

/* gRaceSeries: races run one after the other */
typedef struct { s32 count; s32 done; s32 *buf; } RaceSeries;

extern TrackRecordInfo *gCurTrack_28;
extern TrackRecordInfo gTracks_10[];
extern RaceSeries gRaceSeries_2;

/* Sets the event up for a race of numRiders riders and numLaps laps: the track's records, the start (the
   countdown, or racing at once), and what each mode needs. */
void event_init(s32 numRiders, s32 numLaps)
{
    Rider *rider;
    s32 i;
    s32 j;

    sRaceEndFrame = -1;
    mem_fill_2(gEvent.lapLeaderTimes, -1, 12);
    if (gGame.mode == MODE_SEASON) {
        for (i = 0; i < gEvent.numRiders; i++) {
            gSeason->racePoints[i] = 0;
        }
    }
    gRaceExitCode = RACE_EXIT_NONE;
    *(TrackRecord *)&gEvent.record = gCurTrack_28->record; /* a copy through a pointer: the base register differs otherwise */
    gCurTrack_28->flags &= ~(TRACK_FLAG_NEW_LAP | TRACK_FLAG_NEW_TIME);
    gEvent.finishTimer = -1;
    gEvent.flags = 0;
    gEvent.allFinished = 0;
    gEvent.messageTimer = 0;
    gEvent.camTarget = 0;
    gEvent.idleFrames = 0;
    gEvent.numRiders = numRiders;
    gEvent.numLaps = numLaps;
    if (gGame.mode == MODE_SEASON && !(gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) &&
        gSeason->cup - gCups == 4) {
        gEvent.flags = EVENT_DUEL;
        gEvent.numRiders = 2;
        gNumRiders = 2;
    }
    gEvent.countdown = 150;
    if (gCreditsMode != 0 || (gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) ||
        gGame.mode == MODE_TEST_RIDE || gGame.mode == MODE_TEST_RIDE_OPEN) {
        gEvent.state = EVENT_STATE_GO;
    } else {
        if ((gGame.mode == MODE_EXHIBITION || gGame.mode == MODE_SEASON) &&
            gCurTrack_28 - gTracks_10 != TRACK_LAS_VEGAS) {
            gEvent.state = EVENT_STATE_WAIT;
            gEvent.countdown = 0x100;
        } else {
            gEvent.state = EVENT_STATE_READY;
            gEvent.countdown = 150;
        }
        gEvent.flags |= EVENT_STOPPED;
    }
    gEvent.time = 0;
    gEvent.noPlayers = (gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) != 0;
    mem_fill_2(gRiders, 0, sizeof(Rider) * 8);
    gGame.flags &= ~GAME_REPEAT_STATE;
    if (gRaceSeries_2.count > 0 && gRaceSeries_2.done == 0) {
        gRaceSeries_2.buf = mem_calloc_dbg(gRaceSeries_2.count * 4, 0, (s32)sStrEventFile, 0x6D9);
    }
    switch (gGame.mode) {
    case MODE_SOCCER:
        gEvent.countdown = 180;
        gEvent.state = EVENT_STATE_KICKOFF;
        gEvent.ballToucher = 0;
        gEvent.goals[0] = 0;
        gEvent.goals[1] = 0;
        gEvent.goalTimer = 0;
        gEvent.flags |= EVENT_STOPPED;
        break;
    case MODE_DESERT:
        gEvent.flags |= EVENT_STOPPED;
        race_reset_grid_order();
        break;
    case MODE_HILL_CLIMB:
        gEvent.unk_48 = -1;
        gEvent.flags |= EVENT_STOPPED;
        race_reset_grid_order();
        for (rider = gRiders; rider < &gRiders[gGame.numPlayers]; rider++) {
            rider->timeLimit = 7200; /* two minutes for the first stage */
        }
        break;
    case MODE_TUTORIAL:
        tutorial_init_lesson(1);
        break;
    case MODE_STUNT:
        gEvent.time = 7200; /* two minutes */
        gEvent.flags |= EVENT_STOPPED;
        race_reset_grid_order();
        break;
    case MODE_TEST_RIDE:
    case MODE_TEST_RIDE_OPEN:
        break;
    default:
        if (gEvent.state == 8) {
            race_set_grid_order(2, 0);
        }
        gEvent.numFinished = 0;
        for (j = 0; j < gEvent.numRiders; j++) {
            /* the race fields of rider j, as an offset from rider 0's */
            ((RiderRace *)((u8 *)&gRiders[0].race + j * sizeof(Rider)))->finishPlace = -1;
        }
        break;
    }
}

/* Enters the rider's race time in the track's three best times if it is one (not for CPU riders, replays, the
   custom round, the user track, the time trial's other rider or a race of other than the track's laps); the
   initials are entered later. Returns whether it is the best. */
s32 event_check_record_time(Rider *rider)
{
    u8 best = 0;
    s32 i;
    s32 j;
    s32 *track;
    u8 *src;

    if (rider->isCpu != 0) goto done;
    if (gReplayPlayback != 0) goto done;
    /* the custom round: the cup's index (/ sizeof(Cup)) is 5 */
    if (gGame.mode == MODE_SEASON && gSeason != 0 && (((s32)gSeason->cup - (s32)gCups) * 0x38E38E39 >> 2) == 5) goto done;
    track = gCurTrack_3;
    if (track[0x50 / 4] & TRACK_FLAG_USER) goto done;
    if (gEvent.time >= gEvent.record.times[2]) goto done;
    if (gGame.mode == MODE_TIME_TRIAL && rider != &gRiders[gTimeTrialPlayer]) goto done;
    if (gGame.numLaps != *(s16 *)((u8 *)track + 0x1E)) goto done; /* the track's laps */

    best = gEvent.time < gEvent.record.times[0];
    for (i = 0; i < 3; i++) {
        if (gEvent.time <= gEvent.record.times[i]) {
            for (j = 1; j >= i; j--) {
                gEvent.record.times[j + 1] = gEvent.record.times[j];
                gEvent.record.initials[j + 1][0] = gEvent.record.initials[j][0];
                gEvent.record.initials[j + 1][1] = gEvent.record.initials[j][1];
                gEvent.record.initials[j + 1][2] = gEvent.record.initials[j][2];
            }
            break;
        }
    }
    gEvent.record.times[i] = gEvent.time;
    gEvent.record.initials[i][0] = 0xFF;
    gEvent.record.initials[i][1] = 0xFF;
    gEvent.record.initials[i][2] = (gGame.mode == MODE_TIME_TRIAL) ? 0 : (u8)(rider - gRiders);
    gCurTrack_3[0x50 / 4] |= TRACK_FLAG_NEW_TIME;
    if (best) announce_record(rider - gRiders);
done:
    return best;
}

/* A rider crosses the line for the last time: their finishing place and time, the finish music, the record, the
   season's places and points; a human rider becomes a CPU rider holding its line. Returns whether the time is the
   track's best (or the place if the rider had finished already). */
u32 event_rider_finished(s32 riderIdx)
{
    s32 p = riderIdx;
    u8 *race;
    u8 *rider;
    Event *event;
    u8 *won;
    u8 *points;
    s32 place;
    s32 ofs;
    s32 prevPlace;
    s32 mode;
    s32 placeIdx;
    u32 isBest;
    u16 segment;
    f32 lateral;
    u8 *r;
    u8 *r2;

    /* the riders by hand: p * sizeof(Rider) is ((p << 6) - p) << 5; gRiders[p].race at race */
    ofs = ((p << 6) - p) << 5;
    race = (u8 *)&gRiders[0].race + ofs;
    prevPlace = *(s16 *)(race + 8); /* race.finishPlace */
    if (prevPlace >= 0) {
        return prevPlace;
    }
    event = &gEvent;
    event->finishOrder[event->numFinished] = p;
    place = (u16)event->numFinished;
    event->numFinished = place + 1;
    *(u16 *)(race + 8) = place;
    *(s32 *)(race + 0x2C) = event->time; /* race.finishTime */
    if (*(u8 *)((u8 *)&gRiders[0].race + ofs + 0x88) == 0 && gReplayPlayback == 0) { /* isCpu */
        if ((gGame.mode != MODE_TIME_TRIAL || p == gTimeTrialPlayer) && gNumScreens == 1) {
            snd_play_finish_song();
        }
    }
    rider = (u8 *)gRiders + ((((p << 6) - p)) << 5);
    isBest = event_check_record_time((Rider *)rider);
    if ((gGame.flags & GAME_REPLAY) == 0) {
        mode = gGame.mode;
        if (mode == MODE_SEASON) {
            gSeason->places[p][gSeason->round] = *(u16 *)(race + 8);
            placeIdx = *(s16 *)(race + 8);
            placeIdx = (s16)placeIdx;
            points = (u8 *)&gRacePoints[placeIdx];
            {
                gSeason->racePoints[p] = points[3];
            }
            if (rider[0x6E8] != 0) { /* isCpu */
                goto finished;
            }
            /* the first to finish (the place count equals MODE_SEASON, 1) wins the round */
            ofs = gEvent.numFinished;
            if (ofs != mode) {
                goto human;
            }
            won = (u8 *)gSeason->cup + (gGame.difficulty - 1);
            won[4] = won[4] | (ofs << gSeason->round); /* cup->won[difficulty - 1] */
        }
    }
human:
    {
        u8 *base = (u8 *)gRiders;
        r = base + (((p << 6) - p) << 5);
    }
    if (r[0x6E8] == 0) {
        /* a human rider: the camera follows the winner, and the rider is driven home on its line */
        Event *e = &gEvent;
        if (e->numFinished == 1) {
            e->camTarget = p;
        }
        do { do { do {
        *(u32 *)(race + 0x30) = *(u32 *)(race + 0x30) | 8; /* flags: out of the race */
        lateral = *(f32 *)(r + 0x638);
        segment = *(u16 *)(r + 0x628);
        } while (0); } while (0); } while (0);
        r[0x6E8] = 1;                       /* isCpu */
        *(f32 *)(r + 0x42C) = lateral;      /* laneOffset */
        *(u16 *)(r + 0x734) = segment;      /* landingSegment2 */
        *(u16 *)(r + 0x714) = segment;      /* landingSegment1 */
    }
finished:
    {
        u8 *base2 = (u8 *)gRiders;
        r2 = base2 + (((p << 6) - p) << 5);
    }
    r2[0x3FF] = 1;                                  /* finishState */
    *(f32 *)(r2 + 0x42C) = *(f32 *)(r2 + 0x638);    /* laneOffset = lateral */
    return isBest & 0xff;
}

typedef struct {
    u8 pad[0x50];
    u32 flags;      /* TRACK_FLAG_* */
} TrackFlags;

extern TrackFlags *gCurTrack_29;

/* What event_rider_lap_completed has the announcer say */
#define LAP_SAY_NOTHING     0
#define LAP_SAY_LAP         1   /* the lap */
#define LAP_SAY_LEADER_LAP  2   /* the leader's lap */
#define LAP_SAY_BEST_LAP    3
#define LAP_SAY_WINNER      4
#define LAP_SAY_FINISH      5
#define LAP_SAY_NONE        6   /* a player finished with no record */

/* A rider completed a lap: the best lap record, the gap to the leader shown to the riders (and to the leader, the
   gap to the first player behind), the finish on the last lap, and the announcer. */
void event_rider_lap_completed(s32 p)
{
    s32 say = LAP_SAY_NOTHING;
    RiderRace *race;
    s32 lapTime;
    s32 i;
    s16 leaderTime;

    switch (gGame.mode) {
    case MODE_SOCCER:
    case 3:
    case 4:
    case MODE_DESERT:
    case MODE_TUTORIAL:
    case MODE_TEST_RIDE:
    case MODE_TEST_RIDE_OPEN:
        return;
    }
    race = &gRiders[p].race;
    if (race->lap < gEvent.numLaps) {
        if (gRiders[p].isCpu == 0 && gReplayPlayback == 0 && gGame.mode != MODE_HILL_CLIMB && gGame.mode != MODE_DESERT &&
            !(gCurTrack_29->flags & TRACK_FLAG_USER) && (lapTime = race->lapTimes[race->lap]) < gEvent.record.bestLap &&
            (gGame.mode != MODE_TIME_TRIAL || p == gTimeTrialPlayer)) {
            gEvent.record.bestLap = lapTime;
            gEvent.record.initials[3][0] = 0xFF;
            gEvent.record.initials[3][1] = 0xFF;
            gEvent.record.initials[3][2] = (gGame.mode == MODE_TIME_TRIAL) ? 0 : p;
            say = LAP_SAY_BEST_LAP;
            gCurTrack_29->flags |= TRACK_FLAG_NEW_LAP;
        }
        if (gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) {
            goto end;
        }
        leaderTime = gEvent.lapLeaderTimes[race->lap];
        if (leaderTime == -1) {
            gEvent.lapLeaderTimes[race->lap] = ((u16 *)&gEvent.time)[1];
        } else {
            race->gap = leaderTime - gEvent.time;
            race->gapMsgTimer = 120;
            if (race->place == 1) {
                for (i = 0; i < gGame.numPlayers; i++) {
                    if (race->lap < gRiders[i].race.lap + gRiders[i].lapAdjust) {
                        gRiders[i].race.gapMsgTimer = 120;
                        gRiders[i].race.gap = -race->gap;
                        break;
                    }
                }
            }
        }
        race->lapMsgTimer = 120;
        ((volatile RiderRace *)race)->lap++;
        if (race->lap != gEvent.numLaps) {
            if (say != LAP_SAY_BEST_LAP) {
                if (gGame.mode == MODE_TIME_TRIAL && gRiderAlpha[p] == 0xFF) {
                    say = LAP_SAY_LAP;
                } else if (race->place == 0) {
                    say = LAP_SAY_LEADER_LAP;
                } else if (gRiders[p].isCpu == 0 && race->gap < -300) {
                    say = LAP_SAY_LAP;
                }
            }
        } else if ((u8)event_rider_finished(p)) {
            if (say != LAP_SAY_BEST_LAP) {
                if (race->finishPlace == 0 && gEvent.numRiders >= 2 && say == LAP_SAY_NOTHING) {
                    say = LAP_SAY_WINNER;
                } else if (gRiders[p].isCpu == 0) {
                    say = LAP_SAY_FINISH;
                }
            }
        } else if (p < gGame.numPlayers) {
            say = LAP_SAY_NONE;
        }
    }
end:
    if (gGhostRace != 0 && gGhostSlot == p) {
        say = LAP_SAY_NOTHING;
    }
    switch (say) {
    case LAP_SAY_LAP:
        if (gGhostRace == 0 || p != gGhostSlot) {
            announce_lap(race->lap - 1);
        }
        break;
    case LAP_SAY_LEADER_LAP:
        announce_leader_lap(p, race->lap - 1);
        break;
    case LAP_SAY_BEST_LAP:
        announce_best_lap(p);
        break;
    case LAP_SAY_WINNER:
        announce_winner(p);
        break;
    case LAP_SAY_FINISH:
        announce_finish(p);
        break;
    case LAP_SAY_NONE:
        break;
    }
}

/* The start: 150 frames of "ready", then 3, 2, 1 (36 frames each) and go, with the announcer; the clock starts on
   go. */
void event_update_countdown(void)
{
    if (gEvent.countdown != 0 && --gEvent.countdown == 0) {
        switch (gEvent.state) {
        case EVENT_STATE_WAIT:
            gEvent.countdown = 150;
            gEvent.state = EVENT_STATE_READY;
            break;
        case EVENT_STATE_READY:
            gEvent.countdown = 36;
            gEvent.state = EVENT_STATE_COUNT_3;
            tunes_play_speech(0x6F, 5, 1);
            break;
        case EVENT_STATE_COUNT_3:
            gEvent.countdown = 36;
            gEvent.state = EVENT_STATE_COUNT_2;
            tunes_play_speech(0x6D, 6, 1);
            break;
        case EVENT_STATE_COUNT_2:
            gEvent.countdown = 36;
            gEvent.state = EVENT_STATE_COUNT_1;
            tunes_play_speech(0x6E, 7, 1);
            break;
        case EVENT_STATE_COUNT_1:
            gEvent.countdown = 15;
            gEvent.state = EVENT_STATE_GO;
            gEvent.flags &= ~EVENT_STOPPED;
            tunes_play_speech(0x70, 8, 1);
            break;
        }
    }
}

/* Hill climb: each stage cleared adds 30 frames to the rider's time limit; a rider whose time runs out is out.
   When all are, A or START leaves. */
void event_update_hillclimb(void)
{
    Event *event;
    Rider *rider;
    u8 allOut;

    event_update_countdown();
    event = &gEvent;
    if (!(event->flags & EVENT_STOPPED) && event->allFinished == 0) {
        event->time++;
    }
    allOut = 1;
    for (rider = gRiders; rider < &gRiders[gEvent.numRiders]; rider++) {
        if (rider->race.lapMsgTimer != 0) {
            rider->race.lapMsgTimer--;
            rider->timeLimit += 30;
        }
        if (gEvent.time == rider->timeLimit && rider->finishState == 0) {
            rider->finishState = 1;
            rider->race.finishPlace = 5;
            rider->isCpu = 1;
        }
        if (rider->finishState == 0) allOut = 0;
    }
    gEvent.allFinished = allOut;
    if (allOut) {
        if (((gPlayerInputs_2 & INPUT_START) && !(gPlayerInputsPrev & INPUT_START)) ||
            ((gPlayerInputs_2 & INPUT_A) && !(gPlayerInputsPrev & INPUT_A))) {
            event_request_exit();
        }
    }
}

extern TrackFlags *gCurTrack_30;

/* Stunt course: when the clock runs out, the players' scores go into the records, the riders stop, and with two
   players the higher score wins (place 1 is a tie). */
void event_update_stunt(void)
{
    Rider *rider;
    s32 i, j, k;

    event_update_countdown();
    if (gEvent.flags & EVENT_STOPPED) return;
    if (--gEvent.time > 0) return;
    if (((gPlayerInputs_2 & INPUT_START) && !(gPlayerInputsPrev & INPUT_START)) ||
        ((gPlayerInputs_2 & INPUT_A) && !(gPlayerInputsPrev & INPUT_A))) {
        if (gRacePhase == RACE_PHASE_RACING) {
            gRacePhase = RACE_PHASE_FADE_OUT;
            gRacePhaseTimer = 20;
        }
    }
    gEvent.time = 0;
    if (gEvent.allFinished) return;
    for (i = 0; i < gGame.numPlayers; i++) {
        rider = &gRiders[i];
        if (rider->isCpu) continue;
        if (gReplayPlayback) continue;
        if (gGame.mode == MODE_SEASON && gSeason != NULL && gSeason->cup - gCups == 5) continue;
        /* raceKey is the stunt score here */
        if (rider->raceKey > gEvent.record.times[2]) {
            for (j = 0; j < 3; j++) {
                if (rider->raceKey >= gEvent.record.times[j]) {
                    for (k = 1; k >= j; k--) {
                        gEvent.record.times[k + 1] = gEvent.record.times[k];
                        gEvent.record.initials[k + 1][0] = gEvent.record.initials[k][0];
                        gEvent.record.initials[k + 1][1] = gEvent.record.initials[k][1];
                        gEvent.record.initials[k + 1][2] = gEvent.record.initials[k][2];
                    }
                    break;
                }
            }
            gEvent.record.times[j] = rider->raceKey;
            gEvent.record.initials[j][2] = rider - gRiders;
            gEvent.record.initials[j][0] = 0xFF;
            gEvent.record.initials[j][1] = 0xFF;
            gCurTrack_30->flags |= TRACK_FLAG_NEW_TIME;
        }
    }
    gEvent.allFinished = 1;
    for (j = 0; j < gGame.numPlayers; j++) {
        gRiders[j].isCpu = 1;
    }
    gRiders[0].finishState = 1;
    *(volatile s16 *)&gRiders[0].race.finishPlace = 1;
    if (gGame.numPlayers == 2) {
        if (gRiders[0].raceKey > gRiders[1].raceKey) {
            gRiders[0].race.finishPlace = 0;
            gRiders[1].race.finishPlace = 5;
        } else if (gRiders[0].raceKey == gRiders[1].raceKey) {
            gRiders[0].race.finishPlace = 1;
            gRiders[1].race.finishPlace = 1;
        } else {
            gRiders[0].race.finishPlace = 5;
            gRiders[1].race.finishPlace = 0;
        }
        gRiders[1].finishState = 1;
    }
}

#define PLAYER_PRESSED(i, b) ((gPlayerInputs[i] & (b)) && !(gPlayerInputsPrev_3[i] & (b)))

/* Soccer: goals (the ball inside a goal box), the halves and the overtime on the clock, and START or A to leave
   at the end. Team 1 scores at +z, team 2 at -z; with four players the team is the player number / 2. */
void event_update_soccer(void)
{
    s32 i;
    s32 j;

    if (gEvent.allFinished == 0) {
        gEvent.messageTimer = 180;
    } else if (gEvent.messageTimer != 0) {
        gEvent.messageTimer--;
    }
    if (gEvent.goalTimer != 0) {
        /* after a goal: play resumes from the start positions */
        if (--gEvent.goalTimer == 0) {
            for (i = 0; i < gGame.numPlayers; i++) {
                bike_respawn(&gRiders[i]);
            }
            soccer_ball_reset(0);
            gEvent.flags &= ~(EVENT_STOPPED | EVENT_SCORED);
        }
        return;
    }
    if (gEvent.state == EVENT_STATE_FIRST_HALF || gEvent.state == EVENT_STATE_SECOND_HALF || gEvent.state == EVENT_STATE_OVERTIME) {
        if (!(gEvent.flags & EVENT_SCORED)) {
            if (gSoccerBall.pos.x >= -24.0f && gSoccerBall.pos.x < 23.0f && gSoccerBall.pos.z >= 79.0f && gSoccerBall.pos.z < 87.0f &&
                gSoccerBall.pos.y >= 0.0f && gSoccerBall.pos.y < 6.2f) {
                gEvent.goalTimer = 180;
                gEvent.goals[0]++;
                announce_goal();
                gRiders[0].fistPump = 1;
                gEvent.flags |= EVENT_STOPPED | 1;
                if ((gEvent.ballToucher >> ((gGame.numPlayers - 1) >> 1)) == 1) {
                    gEvent.flags |= EVENT_OWN_GOAL;
                } else {
                    gEvent.flags &= ~EVENT_OWN_GOAL;
                }
            } else if (gSoccerBall.pos.x >= -24.0f && gSoccerBall.pos.x < 23.0f && gSoccerBall.pos.z >= -80.0f && gSoccerBall.pos.z < -73.0f &&
                       gSoccerBall.pos.y >= 0.0f && gSoccerBall.pos.y < 6.2f) {
                gEvent.goalTimer = 180;
                gEvent.goals[1]++;
                announce_goal();
                gRiders[1].fistPump = 1;
                gEvent.flags |= EVENT_STOPPED | 2;
                if ((gEvent.ballToucher >> ((gGame.numPlayers - 1) >> 1)) == 0) {
                    gEvent.flags |= EVENT_OWN_GOAL;
                } else {
                    gEvent.flags &= ~EVENT_OWN_GOAL;
                }
            }
        }
    }
    switch (gEvent.state) {
    case EVENT_STATE_KICKOFF:
        if (--gEvent.countdown == 0) {
            gEvent.state = EVENT_STATE_FIRST_HALF;
            gEvent.time = 10800; /* three minutes */
            gEvent.flags &= ~EVENT_STOPPED;
        }
        break;
    case EVENT_STATE_FIRST_HALF:
        if (--gEvent.time == 0) {
            j = 0;
            gEvent.state = EVENT_STATE_HALF_TIME;
            gEvent.countdown = 240;
            gEvent.flags |= EVENT_STOPPED;
            for (; j < gGame.numPlayers; j++) {
                bike_respawn(&gRiders[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case EVENT_STATE_HALF_TIME:
        if (--gEvent.countdown == 0) {
            gEvent.state = EVENT_STATE_SECOND_HALF;
            gEvent.time = 10800;
            gEvent.flags &= ~EVENT_STOPPED;
        }
        break;
    case EVENT_STATE_SECOND_HALF:
        if (--gEvent.time == 0) {
            gEvent.flags |= EVENT_STOPPED;
            if (gEvent.goals[0] == gEvent.goals[1]) {
                gEvent.state = EVENT_STATE_DRAW;
                gEvent.countdown = 240;
            } else {
                gEvent.state = EVENT_STATE_FINAL;
                gEvent.countdown = 600;
                gEvent.allFinished = 1;
            }
            for (i = 0; i < gGame.numPlayers; i++) {
                bike_respawn(&gRiders[i]);
            }
            soccer_ball_reset(0);
        }
        break;
    case EVENT_STATE_DRAW:
        if (--gEvent.countdown == 0) {
            gEvent.state = EVENT_STATE_OVERTIME;
            gEvent.time = 7200; /* two minutes */
            gEvent.flags &= ~EVENT_STOPPED;
        }
        break;
    case EVENT_STATE_OVERTIME:
        if (--gEvent.time == 0) {
            j = 0;
            gEvent.state = EVENT_STATE_FINAL;
            gEvent.allFinished = 1;
            gEvent.countdown = 600;
            gEvent.flags |= EVENT_STOPPED;
            for (; j < gGame.numPlayers; j++) {
                bike_respawn(&gRiders[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case EVENT_STATE_FINAL:
        if (PLAYER_PRESSED(0, INPUT_START) || PLAYER_PRESSED(1, INPUT_START) || PLAYER_PRESSED(2, INPUT_START) ||
            PLAYER_PRESSED(3, INPUT_START) ||
            (gEvent.messageTimer == 0 &&
             (PLAYER_PRESSED(0, INPUT_A) || PLAYER_PRESSED(1, INPUT_A) || PLAYER_PRESSED(2, INPUT_A) || PLAYER_PRESSED(3, INPUT_A)))) {
            if (gRacePhase == RACE_PHASE_RACING) {
                gRacePhase = RACE_PHASE_FADE_OUT;
                gRacePhaseTimer = 20;
            }
        }
        break;
    }
}

/* A race: the clock and the lap times; once every player is done (finished, or a CPU rider), START or A (after the
   message) leaves, and the riders still racing are placed by an estimate of their time; the season's points and
   standings. When the race state is leaving anyway, the riders left get their places from the standings. */
void event_update_race(void) {
    s32 i, j, k, m;
    s32 numPlayers, playersRacing;
    s32 pad[8];
    u8 done[4];
    f32 f;
    s32 s, t;
    u8 tmp;
    RiderRace *race;

    event_update_countdown();
    playersRacing = 0;
    numPlayers = playersRacing;
    if (!(gEvent.flags & EVENT_STOPPED)) {
        gEvent.time++;
    }
    *(s32 *)done = 0;
    for (m = 0; m < gEvent.numRiders; m++) {
        race = &gRiders[m].race;
        if ((gRiders[m].flags & 8) || (gGhostRace != 0 && gGhostSlot == m) || gRiders[m].isCpu == 0) {
            numPlayers++;
        }
        if (gRiders[m].race.finishPlace == -1 && m < numPlayers) {
            playersRacing++;
        }
        if (race->finishPlace != -1 || gRiders[m].isCpu != 0) {
            done[m] = 1;
        }
        if (gEvent.state == EVENT_STATE_GO) {
            s32 k = race->lap;
            if (k < gEvent.numLaps) {
                race->lapTimes[k]++;
            }
        }
    }
    for (m = 0; m < numPlayers; m++) {
        if (!done[m]) break;
    }
    if (!((m == numPlayers && playersRacing == 0 && gGame.mode != MODE_TIME_TRIAL) ||
          (gGame.mode == MODE_TIME_TRIAL && done[gTimeTrialPlayer]))) return;
    if (gGame.flags & (GAME_FRONTEND_RACE | GAME_TRACK_INTRO)) return;
    if (gRaceExitCode == RACE_EXIT_BACK) return;
    if (gEvent.allFinished == 0) {
        gEvent.messageTimer = 180;
    } else if (gEvent.messageTimer != 0) {
        gEvent.messageTimer--;
    }
    gEvent.allFinished = 1;
    gRaceExitCode = RACE_EXIT_FINISHED;
    if (sRaceEndFrame == -1) {
        sRaceEndFrame = gRaceFrame;
    }
    if (gReplayPlayback == 0 && gRacePhase == RACE_PHASE_RACING) {
        if ((gPadInput_2[0] & INPUT_START) && !(gPadInputPrev_2[0] & INPUT_START)) goto go;
        if ((gPadInput_2[1] & INPUT_START) && !(gPadInputPrev_2[1] & INPUT_START)) goto go;
        if ((gPadInput_2[2] & INPUT_START) && !(gPadInputPrev_2[2] & INPUT_START)) goto go;
        if ((gPadInput_2[3] & INPUT_START) && !(gPadInputPrev_2[3] & INPUT_START)) goto go;
        if (gCreditsMode != 0 && (gGame.flags & GAME_CREDITS_SKIPPABLE) && (gPadInput_2[0] & INPUT_B) && !(gPadInputPrev_2[0] & INPUT_B)) goto go;
        if (gEvent.messageTimer != 0 && gCreditsMode == 0) goto end;
        if ((gPadInput_2[0] & INPUT_A) && !(gPadInputPrev_2[0] & INPUT_A)) goto go;
        if ((gPadInput_2[1] & INPUT_A) && !(gPadInputPrev_2[1] & INPUT_A)) goto go;
        if ((gPadInput_2[2] & INPUT_A) && !(gPadInputPrev_2[2] & INPUT_A)) goto go;
        if (!(gPadInput_2[3] & INPUT_A) || (gPadInputPrev_2[3] & INPUT_A)) goto end;
    go:
        if (gCreditsMode != 0) {
            snd_play_menu_sound(MENU_SFX_BACK);
        }
        if (gRacePhase == RACE_PHASE_RACING) {
            gRacePhase = RACE_PHASE_FADE_OUT;
            gRacePhaseTimer = 20;
        }
        /* a rider still racing: the race time over the part of the race done (at least a tenth) */
        for (k = 0; k < gEvent.numRiders; k++) {
            if (gRiders[k].race.finishPlace == -1) {
                f = gRiders[k].distToGo / (gPathLapLength * gEvent.numLaps);
                if (f <= 0.1f) {
                    f = 0.1f;
                } else if (f > 1.0f) {
                    f = 1.0f;
                }
                f = gEvent.time / f;
                gRiders[k].race.finishTime = f;
            }
        }
        for (k = 0; k < gEvent.numRiders; k++) {
            if (gRiders[k].race.finishPlace == -1) {
                gRiders[k].race.finishPlace = 0;
                for (j = 0; j < gEvent.numRiders; j++) {
                    if ((j != k && k < j && gRiders[j].race.finishTime < gRiders[k].race.finishTime) ||
                        (j < k && gRiders[j].race.finishTime <= gRiders[k].race.finishTime)) {
                        gRiders[k].race.finishPlace++;
                    }
                }
                if (gGame.mode == MODE_SEASON) {
                    gSeason->racePoints[k] += (u8)gRacePoints[gRiders[k].race.finishPlace];
                }
            }
            gEvent.finishOrder[gRiders[k].race.finishPlace] = k;
        }
        gEvent.numFinished = gEvent.numRiders;
        if (gGame.mode == MODE_SEASON) {
            /* the totals, and the standings sorted by them */
            for (i = 0; i < gEvent.numRiders; i++) {
                gSeason->points[i] += gSeason->racePoints[i];
                gSeason->standings[i] = i;
            }
            for (i = 0; i < gEvent.numRiders; i++) {
                j = i;
                if (i != 0) {
                    s = gSeason->points[gSeason->standings[i - 1]];
                }
                t = gSeason->points[gSeason->standings[i]];
                while (j != 0 && s < t) {
                    tmp = gSeason->standings[j];
                    gSeason->standings[j] = gSeason->standings[j - 1];
                    gSeason->standings[j - 1] = tmp;
                    j--;
                    if (j != 0) {
                        s = gSeason->points[gSeason->standings[j - 1]];
                    }
                }
            }
        }
    }
end:
    if (gCurGameState->mode == STATE_EXIT) {
        s = -1;
        for (i = gEvent.numFinished; i < gEvent.numRiders; i++) {
            race = &gRiderStandings[i]->race;
            if (race->finishPlace == s) {
                gEvent.finishOrder[gEvent.numFinished] = i;
                race->finishPlace = gEvent.numFinished++;
                race->finishTime = (gEvent.time * gRiderStandings[0]->distToGo) / gRiders[i].distToGo;
                /* (gGame.mode == MODE_SEASON, and gRacePoints[place]'s low byte, in a form that matches) */
                if (gGame.mode == 1 + (((u32)&gRiders & 1) >> 1)) {
                    gSeason->places[gRiderStandings[i] - gRiders][gSeason->round] = race->finishPlace;
                    gSeason->racePoints[gRiderStandings[i] - gRiders] = ((Pts *)((u32)gRacePoints | (((u32)gRacePoints & 1) >> 1)))[race->finishPlace].b3;
                }
            }
        }
    }
}

/* The event's update: counts the frames without input, runs the mode's rules, and times the checkerboard wipe that
   follows the first player's finish in a one-player race. */
void event_update(s32 arg0)
{
    s32 i;
    s32 *input;

    if ((gGame.flags & GAME_TRACK_INTRO) == 0) {
        s32 bits;
        i = 0;
        bits = gContMask;
        for (input = gPlayerInputs; i < 4; i++, input++) {
            if (((bits >> i) & 1) != 0 && *input != 0) break;
        }
        if (i == 4) {
            gEvent.idleFrames = gEvent.idleFrames + 1;
        } else {
            gEvent.idleFrames = 0;
        }
    }
    switch (gGame.mode) {
    case MODE_SOCCER:
        event_update_soccer();
        break;
    case MODE_DESERT:
        /* the clock stops at 60 minutes */
        event_update_countdown();
        if (gEvent.allFinished == 0) {
            if ((gEvent.flags & EVENT_STOPPED) != 0) break;
            gEvent.time = gEvent.time + 1;
            if (gEvent.time == 0x34bc0) {
                event_rider_finished(0);
                gEvent.allFinished = 1;
            }
        } else if ((gPlayerInputs[0] & INPUT_START) != 0) {
            if ((gPlayerInputsPrev & INPUT_START) == 0) {
                event_request_exit();
            }
            break;
        }
        break;
    case MODE_HILL_CLIMB:
        event_update_hillclimb();
        break;
    case MODE_TUTORIAL:
        tutorial_update();
        break;
    case MODE_STUNT:
        event_update_stunt();
        break;
    default:
        event_update_race();
    case MODE_TEST_RIDE:
    case MODE_TEST_RIDE_OPEN:
        break;
    }
    if (gGame.mode < MODE_SOCCER && gEvent.numFinished > 0 && gEvent.finishOrder[0] < gGame.numPlayers &&
        gEvent.finishTimer != -2 && (gGame.numPlayers == 1 || gGame.mode == MODE_TIME_TRIAL)) {
        gEvent.finishTimer = gEvent.finishTimer + 1;
        if (gGame.numPlayers >= 3) {
            if (gEvent.finishTimer >= 100) goto store;
        } else if (gEvent.finishTimer >= 0x8c) {
            goto store;
        }
        return;
store:
        gEvent.finishTimer = -2;
    }
    return;
}

/* Loads the HUD: the countdown, the temperature gauge, the tutorial's pictures and a grey palette, the soccer map,
   and the text objects. */
void event_hud_init(void)
{
    s32 grey;
    s32 i;
    u16 *dst;
    s32 persp[16];
    s32 j;
    s32 height;
    Game *game;

    if (gGame.mode == MODE_TUTORIAL) {
        /* a palette of 256 greys (RGBA5551) */
        sGreyPalette = (u16 *)mem_alloc_dbg(0x200, 0, sStrEventFile, 0xB4B);
        *sGreyPalette = 0;
        dst = sGreyPalette + 1;
        i = 1;
        do {
            grey = i >> 4;
            *dst = (u16)(grey << 1) | (u16)((grey << 6) | 1) | (u16)(grey << 11);
            dst = dst + 1;
            i = i + 1;
        } while (i < 0x100);
    }
    sCountdownTextures = res_load(gResourceSet, sCountdownResId);
    game = &gGame;
    if (1 < game->numPlayers) {
        sTempGaugeTextures = res_load(gResourceSet, sTempGaugeResId);
    }
    if (game->mode == MODE_TUTORIAL) {
        game->hudGlyphs = (HudGlyphs *)res_load(gResourceSet, gHudGlyphsResId);
        sTutorialPrevNextIcons = res_load(gResourceSet, sTutorialPrevNextResId);
        gFinishLineTexSet = res_load(gResourceSet, gFinishLineTexResId);
        sTutorialTitleImage = res_load(gResourceSet, sTutorialTitleResId);
    }
    res_register_type(sStrEventN64, prim_n64_relocate);
    if (game->numPlayers == 1) {
        D_800F0730[0] = mem_alloc_dbg(0x1c0, 0, sStrEventFile, 0xB6B);
        D_800F0730[1] = mem_alloc_dbg(0x1c0, 0, sStrEventFile, 0xB6C);
    }
    /* only the perspective's normalization (sHudPerspNorm) is kept */
    height = *(volatile s32 *)&gScreenHeight;
    guPerspective(persp, sHudPerspNorm, 33.0f, 320.0f / (f32)height, 50.0f, 4192.0f, 1.0f);
    i = 0;
    if (game->mode == MODE_SOCCER) {
        sSoccerMapTexture = res_load(gResourceSet, sSoccerMapResId);
    }
    for (j = 0; j < HUD_NUM_TEXTS; j++) {
        gHudTextObjs[j] = font_text_create(0, 0x40, 2, 2, 0);
        gMenuTextObjs[j] = font_text_create(0, 0x40, 2, 2, 0);
    }
}

typedef struct { char c[9]; } Str9;
typedef struct { char c[2]; } Str2;

extern Str9 sStrOver3Hours;

/* Writes a time in frames as minutes:seconds.hundredths ("MM:SS.hh"), or the "over 3 hours" text. */
void event_format_time(s32 frames, char *out)
{
    char buf[32];

    if (frames > 3 * 60 * 60 * 60) {
        *(Str9 *)out = sStrOver3Hours;
        return;
    }
    int_to_str(frames / (60 * 60), buf);
    if (strlen(buf) == 1) {
        *(Str2 *)out = *(Str2 *)sStrZero;
        str_cat(out, buf);
    } else {
        str_copy(out, buf);
    }
    str_cat(out, sStrColon);
    int_to_str((frames / 60) % 60, buf);
    if (strlen(buf) == 1) {
        str_cat(out, sStrZero);
        str_cat(out, buf);
    } else {
        str_cat(out, buf);
    }
    str_cat(out, sStrDot);
    int_to_str(((frames % 60) * 100) / 60, buf);
    if (strlen(buf) == 1) {
        str_cat(out, sStrZero);
        str_cat(out, buf);
    } else {
        str_cat(out, buf);
    }
}

/* Writes a time in frames as seconds.hundredths, with the minutes only from one minute up. */
void event_format_time_short(s32 frames, char *out)
{
    s32 len;
    char buf[32];

    *out = 0;
    if (frames > 60 * 60 - 1) {
        int_to_str(frames / (60 * 60), out);
        str_cat(out, sStrColon);
    }
    int_to_str((frames / 60) % 60, buf);
    len = strlen(buf);
    if ((len == 1) && (frames > 60 * 60 - 1)) {
        str_cat(out, sStrZero);
    }
    str_cat(out, buf);
    str_cat(out, sStrDot);
    int_to_str(((frames % 60) * 100) / 60, buf);
    len = strlen(buf);
    if (len == 1) {
        str_cat(out, sStrZero);
    }
    str_cat(out, buf);
    return;
}

/* Shows the rider's finishing place ("1st Place": the font has st, nd, rd and th as the glyphs '{' to '~') in the
   middle of the current view, or "Finished" alone. */
void event_set_place_text(TextObj *text, s32 place, s32 view)
{
    char buf[0x28];
    s32 suffix;
    u8 *rects;
    s32 ofs;
    Quad *rect;

    if (gEvent.numRiders >= 2) {
        buf[0] = place + '1';
        suffix = place;
        if (suffix > 3) {
            suffix = 3;
        }
        buf[1] = suffix + '{';
        strcpy(buf + 2, " Place");
        font_text_add(text, buf);
    } else {
        font_text_add(text, "Finished");
    }
    rects = gViewRects;
    ofs = gCurView * 16 + (gNumScreens - 1) * 64;
    rect = (Quad *)(rects + ofs);
    font_draw_text(text, rect->a + rect->c / 2, rect->b + rect->d / 2);
}

/* The test ride's HUD: a blinking "Testing User Track", the speed and the temperature gauge. Speeds are shown in
   mph: m/s * 3600 / 1612.9. */
Gfx *event_draw_hud_testride(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    char buf[32];
    s32 x;
    s32 y;
    f32 speed;
    s32 *rect;

    font_reset_queue();
    if (gFrameCounter & 0x20) {
        font_text_create(gHudTextObjs[0], 0x40, 1, 2, TEXT_VALIGN_BASELINE);
        font_text_add(gHudTextObjs[0], "Testing User Track");
        rect = gViewRects_4;
        x = gScreenWidth * 0x12;
        if (x < 0) {
            x = x + 0x1ff;
        }
        font_draw_text(gHudTextObjs[0], gViewRects_4[0] + (x >> 9), rect[1] + (gScreenHeight * 0x11) / 0xf0);
    }
    speed = gRiders[view].speed;
    if (0.0f < speed) {
        int_to_str((s32)(speed * 3600.0f / 1612.9f), buf);
    } else {
        int_to_str((s32)(-speed * 3600.0f / 1612.9f), buf);
    }
    font_text_create(gHudTextObjs[0], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
    font_text_add(gHudTextObjs[0], buf);
    /* the speedometer's place, at 512x240 or 640x480 */
    x = 0x1ba;
    if (gViMode == VI_MODE_HI_RES) {
        x = 0x228;
    }
    y = 0xd6;
    if (gViMode == VI_MODE_HI_RES) {
        y = 0x180;
    }
    font_draw_text(gHudTextObjs[0], x, y);
    gdl = font_render(gdl);
    gdl = render_draw_temp_gauge(gdl, dyn, fb, view);
    return gdl;
}

/* The hill climb's HUD: speed and time left (one player) or each view's time and speed; "Stage Clear!", "Game
   Over"; the debug position. */
Gfx *event_draw_hud_hillclimb(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    char buf[32];
    f32 speed;
    s32 x1, y1;
    s32 timeLeft;
    s32 *p;

    font_reset_queue();
    font_text_create(gHudTextObjs[0], 0x40, 1, 2, TEXT_VALIGN_CENTER);
    font_text_create(gHudTextObjs[1], 0x40, 1, 2, TEXT_VALIGN_CENTER);
    font_text_create(gHudTextObjs[2], 0x40, 1, 4, TEXT_VALIGN_CENTER);
    font_text_create(gHudTextObjs[3], 0x40, 1, 4, TEXT_VALIGN_CENTER);
    font_text_create(gHudTextObjs[7], 0x40, 1, 6, TEXT_VALIGN_CENTER);
    font_text_create(gHudTextObjs[8], 0x40, 1, 6, TEXT_VALIGN_CENTER);
    if (gViMode == VI_MODE_HI_RES) {
        font_set_scale(0x10000, 0x20000, 0, 0);
    }
    if (gNumViews == 1) {
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        speed = gRiders[view].speed;
        int_to_str((s32)((speed > 0.0f ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        x1 = 0x1BA;
        if (gViMode == VI_MODE_HI_RES) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (gViMode == VI_MODE_HI_RES) {
            y1 = 0x180;
        }
        font_draw_text(gHudTextObjs[6], x1, y1);
        gdl = render_draw_temp_gauge(gdl, dyn, fb, view);
        font_text_create(gHudTextObjs[4], 0x40, 1, 2, TEXT_VALIGN_CENTER);
        font_text_create(gHudTextObjs[5], 0x40, 1, 4, TEXT_VALIGN_CENTER);
        event_format_time(gRiders[view].timeLimit - gEvent.time, buf);
        font_text_add(gHudTextObjs[4], "Time:");
        font_draw_text(gHudTextObjs[4], 0x23, gViMode == VI_MODE_HI_RES ? 0x180 : 0xD6);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], 100, gViMode == VI_MODE_HI_RES ? 0x180 : 0xD6);
    } else {
        font_text_create(gHudTextObjs[4], 0x40, 2, 2, TEXT_VALIGN_CENTER);
        font_text_create(gHudTextObjs[5], 0x40, 2, 4, TEXT_VALIGN_CENTER);
        timeLeft = -gEvent.time + gRiders[view].timeLimit;
        if (gRiders[view].finishState) {
            timeLeft = gRiders[view].race.finishTime;
        }
        if (timeLeft < 0) {
            timeLeft = 0;
        }
        event_format_time(timeLeft, buf);
        font_text_add(gHudTextObjs[4], "Time:");
        font_draw_text(gHudTextObjs[4], gViewRects_5[gNumScreens - 1][view][0] + 10, 0x1E);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], gViewRects_5[gNumScreens - 1][view][0] + 0x32, 0x1E);
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        speed = gRiders[view].speed;
        int_to_str((s32)((speed > 0.0f ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        font_draw_text(gHudTextObjs[6],
                      gViewRects_5[gNumScreens - 1][view][0] + gViewRects_5[gNumScreens - 1][view][2] - 0x2D,
                      gViewRects_5[gNumScreens - 1][view][1] + gViewRects_5[gNumScreens - 1][view][3] - 0xB);
        gdl = event_draw_temp_gauge(gdl, dyn, fb, view);
    }
    if (gEvent.countdown != 0 && gEvent.state != EVENT_STATE_WAIT && (gEvent.state != EVENT_STATE_READY || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, view);
    }
    if (gRiders[view].race.lapMsgTimer != 0 && gPauseMenu == PAUSE_NONE) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        font_text_add(gHudTextObjs[7], "Stage Clear!");
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][view][0] + gViewRects_5[gNumScreens - 1][view][2] / 2,
                      gViewRects_5[gNumScreens - 1][view][1] + gViewRects_5[gNumScreens - 1][view][3] / 2);
    }
    if (gRiders[view].finishState) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        if (gEvent.allFinished == 0 || view != 0) {
            font_text_add(gHudTextObjs[7], "Game Over");
        } else {
            font_text_add(gHudTextObjs[7], sStrPressStartToContinue);
        }
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][view][0] + gViewRects_5[gNumScreens - 1][view][2] / 2,
                      gViewRects_5[gNumScreens - 1][view][1] + gViewRects_5[gNumScreens - 1][view][3] / 2);
    }
    if (gGame.flags & GAME_DEBUG) {
        /* rider 0's x and z, and the free memory */
        TextObj *t1;
        TextObj *t2;
        TextObj *t3;
        t1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        debug_ftoa(buf, gRiders[0].bike.pos.x, 5);
        font_text_add(t1, buf);
        font_draw_text(t1, 0x28, 0x1E);
        t2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        debug_ftoa(buf, gRiders[0].bike.pos.z, 5);
        font_text_add(t2, buf);
        font_draw_text(t2, 0x28, 0x28);
        t3 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        int_to_str(gDebugFreeMem, buf);
        font_text_add(t3, buf);
        font_draw_text(t3, 0x28, 0x32);
    }
    return font_render(gdl);
}

/* The stunt course's HUD: the time, the total, the trick being done with its points (sliding into the list of
   tricks done when it ends), the tricks done, the speed; with two players each view has its own, and the result. */
Gfx *event_draw_hud_stunt(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    Rider *rider = &gRiders[view];
    char buf[40];
    s32 trick, t;
    s32 x, y, ty, yt, yy;
    s32 x1, y1, xa;
    s32 x3, y3, ax, ay;
    s32 k, i;
    s32 by, dy;
    u8 anim;
    s8 d;
    f32 speed;

    font_reset_queue();
    if (gGame.numPlayers >= 2) {
        if (gEvent.allFinished != 0) {
            font_text_create(gHudTextObjs[7], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            switch (rider->race.finishPlace) {
            case 0:
                font_text_add(gHudTextObjs[7], "Winner");
                break;
            case 1:
                font_text_add(gHudTextObjs[7], sStrTie);
                break;
            default:
                font_text_add(gHudTextObjs[7], sStrLoser);
                break;
            }
            font_draw_text(gHudTextObjs[7],
                          gViewRects[gNumScreens - 1][view].x + gViewRects[gNumScreens - 1][view].w / 2,
                          gViewRects[gNumScreens - 1][view].y + gViewRects[gNumScreens - 1][view].h / 2);
        }
        yt = gViewRects[gNumScreens - 1][view].y + gViewRects[gNumScreens - 1][view].h - 11;
        font_text_create(gHudTextObjs[0], 0x40, 2, 2, TEXT_VALIGN_CENTER);
        font_text_create(gHudTextObjs[1], 0x40, 2, 4, TEXT_VALIGN_CENTER);
        trick = -1;
        do {
        } while (0);
        event_format_time(gEvent.time, buf);
        font_text_add(gHudTextObjs[0], "Time:");
        font_draw_text(gHudTextObjs[0], 0x3C, yt);
        font_text_add(gHudTextObjs[1], buf);
        font_draw_text(gHudTextObjs[1], 100, yt);
        xa = gViewRects[gNumScreens - 1][view].x + gViewRects[gNumScreens - 1][view].w - 20;
        yy = gViewRects[gNumScreens - 1][view].y;
        font_text_create(gHudTextObjs[4], 0x40, 2, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
        font_text_add(gHudTextObjs[4], "Total");
        ty = yy + 8;
        font_draw_text(gHudTextObjs[4], xa, ty);
        font_text_create(gHudTextObjs[5], 0x40, 2, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
        int_to_str(rider->raceKey, buf);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], xa, yy + 16);
        /* the trick: the animation playing if it is one (0-15), or the trick just done */
        anim = (u8)rider->anim;
        if (anim < 16) {
            trick = (s8)anim;
        } else if (rider->unk_3FB != 0) {
            d = rider->trick;
            if (d < 16) {
                trick = d;
            }
        }
        if (trick >= 0) {
            font_text_create(gHudTextObjs[2], 0x40, 2, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            x3 = 0x100;
            font_text_add(gHudTextObjs[2], gRiderAnims[trick].name);
            y3 = ty + 8;
            font_text_create(gHudTextObjs[3], 0x40, 2, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            int_to_str(rider->stuntScore, buf);
            font_text_add(gHudTextObjs[3], buf);
            ax = 0x100;
            ay = ty;
            if (rider->unk_3FB >= 2) {
                /* sliding: the points to the total, a new trick's name to the list */
                k = rider->unk_3FB * 8;
                x3 = (x3 * k + (xa - (gHudTextObjs[3]->w >> 1)) * (256 - k)) >> 8;
                for (i = 0; i < rider->trickListLen; i++) {
                    if (trick == rider->trickList[i]) {
                        break;
                    }
                }
                if (i == rider->trickListLen) {
                    ax = (ax * k + ((gHudTextObjs[4]->w >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (ty + i * 8) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs[2], ax, ay);
            font_draw_text(gHudTextObjs[3], x3, y3);
        }
        for (i = 0; i < rider->trickListLen; i++) {
            font_text_create(gHudTextObjs[i + 8], 0x40, 2, 2, TEXT_VALIGN_CENTER);
            font_text_add(gHudTextObjs[i + 8], gRiderAnims[rider->trickList[i]].name);
            font_draw_text(gHudTextObjs[i + 8], 0x3C, ty + i * 8);
        }
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        speed = rider->speed;
        int_to_str((s32)((speed > 0.0f ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        font_draw_text(gHudTextObjs[6],
                      gViewRects[gNumScreens - 1][view].x + gViewRects[gNumScreens - 1][view].w - 0x2D,
                      gViewRects[gNumScreens - 1][view].y + gViewRects[gNumScreens - 1][view].h - 0xB);
        gdl = event_draw_temp_gauge(gdl, dyn, fb, view);
    } else {
        if (gViMode == VI_MODE_HI_RES) {
            font_set_scale(0x10000, 0x20000, 0, 0);
        }
        font_text_create(gHudTextObjs[0], 0x40, 1, 2, TEXT_VALIGN_BASELINE);
        font_text_create(gHudTextObjs[1], 0x40, 1, 4, TEXT_VALIGN_BASELINE);
        event_format_time(gEvent.time, buf);
        font_text_add(gHudTextObjs[0], "Time:");
        font_draw_text(gHudTextObjs[0], 0x23, gViMode == VI_MODE_HI_RES ? 0x180 : 0xD6);
        font_text_add(gHudTextObjs[1], buf);
        font_draw_text(gHudTextObjs[1], 100, gViMode == VI_MODE_HI_RES ? 0x180 : 0xD6);
        y = 0x19;
        x = gScreenWidth - 62;
        if (gViMode == VI_MODE_HI_RES) {
            y = 0x69;
        }
        font_text_create(gHudTextObjs[4], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
        font_text_add(gHudTextObjs[4], "Total");
        font_draw_text(gHudTextObjs[4], x, y);
        if (gViMode == VI_MODE_HI_RES) {
            y += 0x19;
        } else {
            y += 0xC;
        }
        font_text_create(gHudTextObjs[5], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
        int_to_str(rider->raceKey, buf);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], x, y);
        if (gViMode == VI_MODE_HI_RES) {
            by = 0x69;
            dy = 0x14;
        } else {
            by = 0x5A;
            dy = 0xA;
        }
        anim = (u8)rider->anim;
        trick = -1;
        if (anim < 16) {
            trick = (s8)anim;
        } else if (rider->unk_3FB != 0) {
            d = rider->trick;
            if (d < 16) {
                trick = d;
            }
        }
        if (trick >= 0) {
            font_text_create(gHudTextObjs[2], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            font_text_add(gHudTextObjs[2], gRiderAnims[trick].name);
            font_text_create(gHudTextObjs[3], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            int_to_str(rider->stuntScore, buf);
            font_text_add(gHudTextObjs[3], buf);
            x3 = gScreenWidth / 2;
            y3 = gViewRects[0][0].y + 0x28;
            ax = gScreenWidth / 2;
            ay = gViewRects[0][0].y + 0x14;
            if (rider->unk_3FB >= 2) {
                k = rider->unk_3FB * 8;
                x3 = (x3 * k + (x - (gHudTextObjs[3]->w >> 1)) * (256 - k)) >> 8;
                y3 = (y3 * k + y * (256 - k)) / 256;
                for (i = 0; i < rider->trickListLen; i++) {
                    if (trick == rider->trickList[i]) {
                        break;
                    }
                }
                if (i == rider->trickListLen) {
                    ax = (ax * k + ((gHudTextObjs[4]->w >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (by + i * dy) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs[2], ax, ay);
            font_draw_text(gHudTextObjs[3], x3, y3);
        }
        speed = gRiders[view].speed;
        int_to_str((s32)((speed > 0.0f ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        font_text_add(gHudTextObjs[6], buf);
        x1 = 0x1BA;
        if (gViMode == VI_MODE_HI_RES) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (gViMode == VI_MODE_HI_RES) {
            y1 = 0x180;
        }
        font_draw_text(gHudTextObjs[6], x1, y1);
        for (i = 0; i < rider->trickListLen; i++) {
            font_text_create(gHudTextObjs[i + 8], 0x40, 2, 2, TEXT_VALIGN_CENTER);
            font_text_add(gHudTextObjs[i + 8], gRiderAnims[rider->trickList[i]].name);
            font_draw_text(gHudTextObjs[i + 8], 0x23, by + i * dy);
        }
        if (gEvent.allFinished != 0) {
            font_text_create(gHudTextObjs[7], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            font_text_add(gHudTextObjs[7], gRiders[0].finishState >= 3 ? "Press START to Continue" : "Game Over");
            font_draw_text(gHudTextObjs[7],
                          gViewRects[gNumScreens - 1][view].x + gViewRects[gNumScreens - 1][view].w / 2,
                          gViewRects[gNumScreens - 1][view].y + gViewRects[gNumScreens - 1][view].h / 2);
        }
    }
    gdl = font_render(gdl);
    gdl = render_draw_temp_gauge(gdl, dyn, fb, view);
    if (gEvent.countdown != 0 && gEvent.state != EVENT_STATE_WAIT && (gEvent.state != EVENT_STATE_READY || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, view);
    }
    return gdl;
}

/* The desert's HUD: the place, the speed, the time and the gate count ("Gate n/10"), and the finish. */
Gfx *event_draw_hud_gates(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    char buf[32];
    f32 speed;
    s32 y;
    TextObj *text;
    s32 n;
    s32 x;
    s32 yy;
    s32 z;
    char *p;

    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gdl = render_draw_temp_gauge(gdl, dyn, fb, view);
    font_reset_queue();
    gdl = event_draw_position(gdl, 0, font_text_create(gHudTextObjs[5], 0x20, 3, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER));

    speed = gRiders[view].speed;
    int_to_str((s32)(((0.0f < speed) ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);

    font_text_create(gHudTextObjs[0], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
    font_text_add(gHudTextObjs[0], buf);
    x = 0x1BA;
    if (*(volatile s32 *)&gViMode == VI_MODE_HI_RES) x = 0x228;
    yy = 0xD6;
    if (*(volatile s32 *)&gViMode == VI_MODE_HI_RES) yy = 0x180;
    font_draw_text(gHudTextObjs[0], x, yy);

    if (gHudLevel <= 0) {
        font_text_create(gHudTextObjs[1], 0x40, 1, 2, TEXT_VALIGN_CENTER);
        font_text_create(gHudTextObjs[2], 0x40, 1, 4, TEXT_VALIGN_CENTER);
        event_format_time(gEvent.time, buf);
        y = gViewRects_10[gNumScreens - 1][view].b;
        font_text_add(gHudTextObjs[1], sStrTime);
        z = y + 0x14;
        font_draw_text(gHudTextObjs[1], 0x23, z);
        font_text_add(gHudTextObjs[2], buf);
        font_draw_text(gHudTextObjs[2], 100, z);
        font_text_create(gHudTextObjs[3], 0x40, 1, 2, TEXT_VALIGN_CENTER);
        font_text_create(gHudTextObjs[4], 0x40, 1, 4, TEXT_VALIGN_CENTER);
        /* "n/10": the 10 is written after the number's terminator, which the '/' then replaces */
        int_to_str(gRiders[0].checkpoint + 1, buf);
        n = strlen(buf);
        int_to_str(10, buf + n + 1);
        p = buf;
        n = strlen(p);
        p += n;
        *p = '/';
        if (gViMode == VI_MODE_HI_RES) z = y + 0x2C;
        else z = y + 0x20;
        font_text_add(gHudTextObjs[3], sStrGate);
        font_draw_text(gHudTextObjs[3], 0x23, z);
        font_text_add(gHudTextObjs[4], buf);
        font_draw_text(gHudTextObjs[4], 100, z);
    }

    if (gRiders[0].race.finishPlace != -1) {
        text = font_text_create(gHudTextObjs[6], 0x20, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        if (gRiders[0].finishState == 0) {
            font_text_add(text, sStrPressStartToContinue);
            font_draw_text(text,
                          gViewRects_10[gNumScreens - 1][view].a + gViewRects_10[gNumScreens - 1][view].c / 2,
                          gViewRects_10[gNumScreens - 1][view].b + gViewRects_10[gNumScreens - 1][view].d / 2);
        } else {
            event_set_place_text(text, gRiders[0].race.finishPlace, view);
        }
    }

    gdl = event_draw_position(gdl, 0, font_text_create(gHudTextObjs[5], 0x20, 3, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER));
    gdl = font_render(gdl);
    if (gEvent.countdown != 0 && gEvent.state != EVENT_STATE_WAIT &&
        (gEvent.state != EVENT_STATE_READY || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, view);
    }
    return gdl;
}

/* sCountdownTextures: an image set (palette, then the images 1, 2, 3, GO and READY), one colour each */
typedef struct { s32 unk0; u16 *colors; Img_2 img[5]; } CountdownImages;
extern CountdownImages *sCountdownTextures_2;

/* Draws the countdown's image in the middle of the view: READY fades in and out at half height, 3, 2 and 1 shrink
   and fade, GO grows and fades. */
Gfx *event_draw_countdown(Gfx *gdl, s32 view)
{
    f32 scale;
    s32 alpha;
    u16 color;
    Img_2 *img;
    s32 width, dsdx, height;
    s32 *rect;

    if (gEvent.state == EVENT_STATE_GO && gEvent.countdown == 0)
        return gdl;
    if (gTimeTrialPlayer != 0)
        view = 0;
    switch (gEvent.state) {
    default:
        return gdl;
    case EVENT_STATE_READY: {
        f32 v;
        color = sCountdownTextures_2->colors[4];
        img = &sCountdownTextures_2->img[4];
        scale = gEvent.countdown + 10;
        scale /= 100.0f;
        scale *= 2.0f;
        if (scale > 1.0f)
            v = 100.0f - (scale - 1.0f) * 100.0f;
        else
            v = scale * 100.0f;
        alpha = v;
        if (alpha < 0)
            alpha = 0;
        scale = 2.0f;
        break;
    }
    case EVENT_STATE_COUNT_3: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[2];
        v = scale * 100.0f;
        v += 0.0f;
        color = sCountdownTextures_2->colors[2];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case EVENT_STATE_COUNT_2: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[1];
        v = scale * 100.0f;
        v += 0.0f;
        color = sCountdownTextures_2->colors[1];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case EVENT_STATE_COUNT_1: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[0];
        v = scale * 100.0f;
        v += 0.0f;
        color = sCountdownTextures_2->colors[0];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case EVENT_STATE_GO:
    case EVENT_STATE_FREE_RIDE:
    case EVENT_STATE_TIMED:
        scale = gEvent.countdown + 1;
        scale *= 0.0625f;
        img = &sCountdownTextures_2->img[3];
        color = sCountdownTextures_2->colors[3];
        alpha = scale * 120.0f;
        scale = 3.5f - scale * 3.0f;
        break;
    }

    width = scale * (img->w * 8.0f);
    dsdx = 1024.0f / scale;
    if (gEvent.state == EVENT_STATE_READY)
        height = scale * img->h * 0.5f;
    else
        height = scale * img->h;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, G_RM_AA_XLU_SURF, G_RM_NOOP2);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetTextureImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, img->img);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPLoadSync(gdl++);
    gDPLoadTile(gdl++, G_TX_LOADTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetPrimColor(gdl++, 0, 0, (color >> 8) & 0xF8, (color >> 3) & 0xF8, (color << 2) & 0xF8, alpha);
    {
        u8 *rects = (u8 *)gViewRects;
        rect = (s32 *)((view << 4) + ((gNumScreens - 1) << 6) + rects);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, rect[0], rect[1], rect[0] + rect[2] - 1, rect[1] + rect[3] - 1);
    /* drawn upside down from the image's last row; READY at half height */
    gSPTextureRectangle(gdl++, (rect[0] + rect[2] / 2 - width / 2) << 2, (rect[1] + rect[3] / 2 - height / 2) << 2,
                        (rect[0] + rect[2] / 2 + width / 2) << 2, (rect[1] + rect[3] / 2 + height / 2) << 2,
                        G_TX_RENDERTILE, 0, (img->h - 1) << 5, dsdx,
                        gEvent.state == EVENT_STATE_READY ? -(dsdx * 2) : -dsdx);
    gDPPipeSync(gdl++);
    return gdl;
}

/* Draws the rider's place at the top right of the current view, with its pop animation (placeAnim). */
void *event_draw_position(void *gdl, s32 riderIdx, TextObj *text)
{
    Rider *base = gRiders;
    Rider *rider = &base[riderIdx];
    s32 size = rider->race.placeAnim;
    char buf[3];

    if (size > 0x180) {
        size = 0x300 - size;
    }
    gdl = font_render(gdl);
    font_reset_queue();
    /* the place and its suffix glyph ('{' st ... '=' the plain th) */
    buf[0] = ((u8 *)&rider->race.prevPlace)[1] + '1';
    buf[1] = (rider->race.prevPlace < 3) ? ((u8 *)&rider->race.prevPlace)[1] + ':' : '=';
    buf[2] = 0;
    font_text_add(text, buf);
    if (gViMode == VI_MODE_HI_RES) {
        font_set_scale(size << 9, size << 9, 0, 0);
        font_draw_text(text, gViewRects[gNumScreens - 1][gCurView].x + gViewRects[gNumScreens - 1][gCurView].w - 0x3C,
                      gViewRects[gNumScreens - 1][gCurView].y + 0x14);
        gdl = font_render(gdl);
        font_set_scale(0x10000, 0x20000, 0, 0);
    } else {
        font_set_scale(size << 8, size << 8, 0, 0);
        font_draw_text(text, gViewRects[gNumScreens - 1][gCurView].x + gViewRects[gNumScreens - 1][gCurView].w - 0x28,
                      gViewRects[gNumScreens - 1][gCurView].y + 0xA);
        gdl = font_render(gdl);
        font_set_scale(0x10000, 0x10000, 0, 0);
    }
    font_reset_queue();
    return gdl;
}

/* the current view's rectangle */
#define VP gViewRects[gNumScreens - 1][gCurView]

/* A race's HUD: place, speed, race, lap and best lap times (fewer with more views and with gHudLevel), the
   countdown, the gap at each lap, "Off Course", "Wrong Way", the finishing place, the temperature gauge, and
   "Press START to Continue" at the end. */
Gfx *event_draw_hud_race(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    char buf[88];
    char *p;
    RiderRace *race;
    TextObj *tRace;
    TextObj *tLap;
    TextObj *tBest;
    TextObj *tPlace;
    TextObj *tRaceL;
    TextObj *tLapL;
    TextObj *tBestL;
    TextObj *tSpeed;
    TextObj *tMsg;
    TextObj *t;
    s32 gap;
    f32 speed;
    s32 x, y;
    s32 xa, xb, yy, dy;

    race = &gRiders[(gCamTargetOverride == -1) ? view + gTimeTrialPlayer : gCamTargetOverride].race;
    if (gNumScreens == 1 && gCamTargetOverride != -1) {
        view = gCamTargetOverride;
    }
    font_reset_queue();
    if (gViMode == VI_MODE_HI_RES) {
        font_set_scale(0x10000, 0x20000, 0, 0);
    }
    if (gNumScreens < 3) {
        tPlace = font_text_create(gHudTextObjs[0], 0x20, 3, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        if (gNumScreens == 1) {
            tSpeed = font_text_create(gHudTextObjs[1], 0x20, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        } else {
            tSpeed = font_text_create(gHudTextObjs[1], 0x20, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_CENTER);
        }
        tRace = font_text_create(gHudTextObjs[2], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tBest = font_text_create(gHudTextObjs[3], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tRaceL = font_text_create(gHudTextObjs[4], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tBestL = font_text_create(gHudTextObjs[5], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        if (gEvent.countdown != 0 && gEvent.state == EVENT_STATE_GO) {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        } else {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        }
        if (race->lapMsgTimer != 0 || race->lap == gEvent.numLaps) {
            if (race->lapMsgTimer != 0) {
                race->lapMsgTimer--;
            }
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 4, TEXT_VALIGN_BASELINE);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 4, TEXT_VALIGN_BASELINE);
        } else {
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        }
        if (gGame.flags & GAME_DEBUG) {
            /* rider 0's x and z, and the free memory */
            TextObj *t1, *t2, *t3;
            t1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
            debug_ftoa(buf, gRiders[0].bike.pos.x, 5);
            font_text_add(t1, buf);
            font_draw_text(t1, 0x28, 0x1E);
            t2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
            debug_ftoa(buf, gRiders[0].bike.pos.z, 5);
            font_text_add(t2, buf);
            font_draw_text(t2, 0x28, 0x28);
            t3 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
            int_to_str(gDebugFreeMem, buf);
            font_text_add(t3, buf);
            font_draw_text(t3, 0x28, 0x32);
        }
        if (gGame.mode == MODE_EXCITE_3D && gEvent.time == 0) {
            t = font_text_create(gHudTextObjs[19], 0x20, 2, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_CENTER);
            font_text_add(t, "Press Up C Button for side view");
            font_draw_text(t, 0x100, 0x28);
        }
    } else {
        tPlace = font_text_create(gHudTextObjs[0], 8, 2, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        tSpeed = font_text_create(gHudTextObjs[1], 0x20, 2, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
        tRace = font_text_create(gHudTextObjs[2], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tBest = font_text_create(gHudTextObjs[3], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tRaceL = font_text_create(gHudTextObjs[4], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tBestL = font_text_create(gHudTextObjs[5], 0x20, 2, 2, TEXT_VALIGN_BASELINE);
        tLap = font_text_create(gHudTextObjs[6], 0x20, 2, 4, TEXT_VALIGN_BASELINE);
        tLapL = font_text_create(gHudTextObjs[7], 0x20, 2, 4, TEXT_VALIGN_BASELINE);
        if (gEvent.countdown != 0 && gEvent.state == EVENT_STATE_GO) {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        } else {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        }
    }
    if (race->finishPlace != -1) {
        event_set_place_text(tMsg, race->finishPlace, view);
    }
    if (gReplayPlayback == 0) {
        if (gEvent.countdown != 0 && gEvent.state != EVENT_STATE_WAIT && (gEvent.state != EVENT_STATE_READY || gEvent.countdown < 0x5B)) {
            gdl = event_draw_countdown(gdl, view);
        } else if (gPauseMenu == PAUSE_NONE && race->finishPlace == -1) {
            if (gRiders[view].race.gapMsgTimer != 0) {
                /* the gap to the rider ahead (-, colour 7) or behind (+, colour 8) */
                gap = gRiders[view].race.gap;
                gRiders[view].race.gapMsgTimer--;
                if (gap < 0) {
                    gap = -gap;
                    buf[0] = '+';
                    tMsg->color = 8;
                } else if (gap > 0) {
                    buf[0] = '-';
                    tMsg->color = 7;
                }
                buf[1] = 0;
                event_format_time_short(gap, buf + 1);
                font_text_add(tMsg, buf);
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 4);
            } else if (((gRiders[view].offTrackTimer - 1) & 0x20) == 0) {
                font_text_add(tMsg, "Off Course");
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            } else if (gRiders[view].wrongWay != 0 && (gRaceFrame & 0x20)) {
                font_text_add(tMsg, "Wrong Way");
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            }
        }
        switch (gNumScreens) {
        case 1:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gDPPipeSync(gdl++);
            if (gEvent.numRiders >= 2) {
                gdl = event_draw_position(gdl, view, tPlace);
            }
            speed = gRiders[view].speed;
            int_to_str((s32)(((0.0f < speed) ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
            font_text_add(tSpeed, buf);
            x = 0x1BA;
            if (gViMode == VI_MODE_HI_RES) {
                x = 0x228;
            }
            y = 0xD6;
            if (gViMode == VI_MODE_HI_RES) {
                y = 0x180;
            }
            font_draw_text(tSpeed, x, y);
            if (race->lap == gEvent.numLaps) {
                event_format_time(race->finishTime, buf);
            } else {
                event_format_time(gEvent.time, buf);
            }
            if (gHudLevel < 2) {
                font_text_add(tRace, buf);
                font_text_add(tRaceL, "Race: ");
                xa = VP.x + 0x43;
                xb = VP.x + 0x14;
                if (gViMode != VI_MODE_HI_RES) {
                    yy = VP.y + VP.h - 0x1C;
                    dy = 10;
                } else {
                    yy = VP.y + VP.h - 0x38;
                    dy = 0x14;
                }
                if (gCurTrack_8->w50 & TRACK_FLAG_USER) {
                    yy += dy;
                }
                font_draw_text(tRaceL, xb, yy);
                font_draw_text(tRace, xa, yy);
                yy += dy;
                __builtin_strcpy(buf, "Lap ");
                p = buf;
                if (race->lapMsgTimer != 0 || race->lap == gEvent.numLaps) {
                    int_to_str(race->lap, p + strlen(p));
                } else {
                    int_to_str(race->lap + 1, p + strlen(p));
                }
                str_cat(p, ": ");
                font_text_add(tLapL, buf);
                event_format_time(race->lapTimes[(race->lapMsgTimer != 0 || race->lap == gEvent.numLaps) ? race->lap - 1 : race->lap], buf);
                font_text_add(tLap, buf);
                font_draw_text(tLapL, xb, yy);
                font_draw_text(tLap, xa, yy);
                yy += dy;
                if (!(gCurTrack_8->w50 & TRACK_FLAG_USER)) {
                    event_format_time(gEvent.record.bestLap, buf);
                    font_text_add(tBestL, "Best: ");
                    font_text_add(tBest, buf);
                    font_draw_text(tBestL, xb, yy);
                    font_draw_text(tBest, xa, yy);
                }
            }
            gdl = font_render(gdl);
            gDPPipeSync(gdl++);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gdl = render_draw_temp_gauge(gdl, dyn, fb, gCurView);
            break;
        case 2:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            gdl = event_draw_position(gdl, view, tPlace);
            if (gHudLevel < 2) {
                speed = gRiders[view].speed;
                int_to_str((s32)(((0.0f < speed) ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
                font_text_add(tSpeed, buf);
                font_draw_text(tSpeed, VP.x + VP.w - 0x2D, VP.y + VP.h - 0xB);
                if (race->lap == gEvent.numLaps) {
                    event_format_time(race->finishTime, buf);
                } else {
                    event_format_time(gEvent.time, buf);
                }
                font_text_add(tRace, buf);
                font_text_add(tRaceL, "Race: ");
                font_draw_text(tRaceL, VP.x + 10, VP.y + VP.h - 0x12);
                font_draw_text(tRace, VP.x + 0x43, VP.y + VP.h - 0x12);
            }
            __builtin_strcpy(buf, "Lap ");
            p = buf;
            if (race->lapMsgTimer != 0 || race->lap == gEvent.numLaps) {
                int_to_str(race->lap, p + strlen(p));
            } else {
                int_to_str(race->lap + 1, p + strlen(p));
            }
            str_cat(p, ": ");
            font_text_add(tLapL, buf);
            font_draw_text(tLapL, VP.x + 10, VP.y + VP.h - 8);
            event_format_time(race->lapTimes[(race->lapMsgTimer != 0 || race->lap == gEvent.numLaps) ? race->lap - 1 : race->lap], buf);
            font_text_add(tLap, buf);
            font_draw_text(tLap, VP.x + 0x43, VP.y + VP.h - 8);
            gdl = event_draw_temp_gauge(font_render(gdl), dyn, fb, view);
            break;
        default:
            /* three or four views: the place and the speed in mph */
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            buf[0] = (u8)gRiders[view].race.place + '1';
            buf[1] = (gRiders[view].race.place < 3) ? (u8)gRiders[view].race.place + '{' : '~';
            buf[2] = 0;
            font_text_add(tPlace, buf);
            font_draw_text(tPlace, VP.x + VP.w - 10, VP.y + 13);
            speed = gRiders[view].speed;
            int_to_str((s32)(((0.0f < speed) ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
            font_text_add(tSpeed, buf);
            font_text_add(tSpeed, "mph");
            font_draw_text(tSpeed, VP.x + VP.w - 10, VP.y + VP.h - 6);
            gdl = event_draw_temp_gauge(font_render(gdl), dyn, fb, view);
            break;
        }
    } else {
        gdl = font_render(gdl);
    }
    if (gEvent.allFinished != 0 && gCurView == gNumScreens - 1) {
        /* drawn once, over all the views (on a dark band with several) */
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                      gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
        font_text_create(gMenuTextObjs[0], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        font_text_create(gMenuTextObjs[1], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        font_text_add(gMenuTextObjs[0], "Press START to Continue");
        if (gNumScreens == 1) {
            font_draw_text(gMenuTextObjs[0], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 + 0x17);
        } else {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xA0);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
            gDPFillRectangle(gdl++, gScreenWidth * 0, gViewRects[0][0].y + gViewRects[0][0].h / 2 - 7,
                             gScreenWidth - 1, gViewRects[0][0].y + gViewRects[0][0].h / 2 + 7);
            font_draw_text(gMenuTextObjs[0], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2);
        }
        gdl = font_render(gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

/* Soccer's HUD, drawn once for all the views: the field map with the riders and the ball (during play), the half's
   title, the score and the clock, and the goal message in each view. */
Gfx *event_draw_hud_soccer(Gfx *gdl, s32 dyn, s32 fb, s32 view) {
    char buf[32];
    char msg[64];
    s32 cx;
    s32 cy;
    s32 n;
    s32 team;

    font_reset_queue();
    if (view == 0) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                      gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
        if (gEvent.state == EVENT_STATE_FIRST_HALF || gEvent.state == EVENT_STATE_SECOND_HALF || gEvent.state == EVENT_STATE_OVERTIME) {
            /* the map: the field's z across, x down, a quarter of a unit per pixel (z squeezed by 0.625) */
            gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xE6);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            cx = 0x100;
            cy = 0x78;
            if (gGame.numPlayers == 4) {
                cx = 0x13C;
            }
            gdl = render_draw_image((cx - sSoccerMapTexture_2[8] * 4) | 1, cy - (sSoccerMapTexture_2[9] >> 1) - 1, sSoccerMapTexture_2 + 8, gdl,
                                fb, 1, 1, 6, 0);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x28, 0xFF, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders[0].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[0].bike.pos.x * 0.25f - 2.0f,
                             cx + gRiders[0].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[0].bike.pos.x * 0.25f + 2.0f);
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0x28, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders[1].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[1].bike.pos.x * 0.25f - 2.0f,
                             cx + gRiders[1].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[1].bike.pos.x * 0.25f + 2.0f);
            if (gGame.numPlayers == 4) {
                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0xFA, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + gRiders[2].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[2].bike.pos.x * 0.25f - 2.0f,
                                 cx + gRiders[2].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[2].bike.pos.x * 0.25f + 2.0f);
                gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0x28, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + gRiders[3].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[3].bike.pos.x * 0.25f - 2.0f,
                                 cx + gRiders[3].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[3].bike.pos.x * 0.25f + 2.0f);
            }
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0xFA, 0xFF);
            gDPFillRectangle(gdl++, cx + gSoccerBall.pos.z * 0.25f / 0.625f - 1.0f, cy + gSoccerBall.pos.x * 0.25f - 1.0f,
                             cx + gSoccerBall.pos.z * 0.25f / 0.625f + 1.0f, cy + gSoccerBall.pos.x * 0.25f + 1.0f);
        }
        switch (gEvent.state) {
        case EVENT_STATE_KICKOFF:
            font_text_create(gHudTextObjs[0], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            if (gGame.numPlayers == 2) {
                font_text_add(gHudTextObjs[0], "One on One Soccer: First Half");
            } else {
                font_text_add(gHudTextObjs[0], sStrSoccerFirstHalf);
            }
            font_draw_text(gHudTextObjs[0], gScreenWidth / 2, gScreenHeight / 2);
            break;
        case EVENT_STATE_HALF_TIME:
        case EVENT_STATE_DRAW:
        case EVENT_STATE_FINAL:
            font_text_create(gHudTextObjs[0], 0x40, 1, 6, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
            switch (gEvent.state) {
            case EVENT_STATE_DRAW:
                font_text_add(gHudTextObjs[0], "Full Time Draw : Overtime Required");
                break;
            case EVENT_STATE_HALF_TIME:
                font_text_add(gHudTextObjs[0], "Half Time");
                break;
            case EVENT_STATE_FINAL:
                font_text_add(gHudTextObjs[0], "Final");
                break;
            }
            font_draw_text(gHudTextObjs[0], gScreenWidth / 2, gScreenHeight / 2 - 13);
            font_text_create(gHudTextObjs[1], 0x40, 1, 2, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[2], 0x40, 1, 2, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[3], 0x40, 1, 4, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[4], 0x40, 1, 4, TEXT_VALIGN_CENTER);
            font_text_add(gHudTextObjs[1], "Team 1:");
            font_text_add(gHudTextObjs[2], "Team 2:");
            int_to_str(gEvent.goals[0], buf);
            font_text_add(gHudTextObjs[3], buf);
            int_to_str(gEvent.goals[1], buf);
            font_text_add(gHudTextObjs[4], buf);
            font_draw_text(gHudTextObjs[1], gScreenWidth / 2 - 45, gScreenHeight / 2);
            font_draw_text(gHudTextObjs[2], gScreenWidth / 2 - 45, gScreenHeight / 2 + 13);
            font_draw_text(gHudTextObjs[3], gScreenWidth / 2 + 39, gScreenHeight / 2);
            font_draw_text(gHudTextObjs[4], gScreenWidth / 2 + 39, gScreenHeight / 2 + 13);
            if (gEvent.state == EVENT_STATE_FINAL) {
                font_text_create(gHudTextObjs[0], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
                font_text_add(gHudTextObjs[0], "Press START To Continue");
                font_draw_text(gHudTextObjs[0], gScreenWidth / 2,
                              gViewRects[gNumScreens - 1][0].y + gViewRects[gNumScreens - 1][0].h / 2);
                font_draw_text(gHudTextObjs[0], gScreenWidth / 2,
                              gViewRects[gNumScreens - 1][gNumScreens - 1].y +
                                  gViewRects[gNumScreens - 1][gNumScreens - 1].h / 2);
            }
            break;
        default:
            font_text_create(gHudTextObjs[0], 0x40, 1, 2, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[1], 0x40, 1, 2, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[2], 0x40, 1, 4, TEXT_VALIGN_CENTER);
            font_text_create(gHudTextObjs[3], 0x40, 1, 4, TEXT_VALIGN_CENTER);
            font_text_add(gHudTextObjs[0], "Team 1:");
            font_text_add(gHudTextObjs[1], "Team 2:");
            int_to_str(gEvent.goals[0], buf);
            font_text_add(gHudTextObjs[2], buf);
            int_to_str(gEvent.goals[1], buf);
            font_text_add(gHudTextObjs[3], buf);
            if (gGame.numPlayers == 4) {
                font_draw_text(gHudTextObjs[0], gScreenWidth / 2 - 45, 30);
                font_draw_text(gHudTextObjs[1], gScreenWidth / 2 - 45, gScreenHeight - 30);
                font_draw_text(gHudTextObjs[2], gScreenWidth / 2 + 39, 30);
                font_draw_text(gHudTextObjs[3], gScreenWidth / 2 + 39, gScreenHeight - 30);
            } else {
                font_draw_text(gHudTextObjs[0], 65, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[1], 65, gScreenHeight / 2 + 7);
                font_draw_text(gHudTextObjs[2], 150, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[3], 150, gScreenHeight / 2 + 7);
            }
            font_text_create(gHudTextObjs[4], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
            font_text_create(gHudTextObjs[5], 0x40, 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT);
            switch (gEvent.state) {
            case EVENT_STATE_OVERTIME:
                font_text_add(gHudTextObjs[4], "Overtime");
                break;
            case EVENT_STATE_FIRST_HALF:
                font_text_add(gHudTextObjs[4], "First Half");
                break;
            case EVENT_STATE_SECOND_HALF:
                font_text_add(gHudTextObjs[4], "Second Half");
                break;
            }
            event_format_time(gEvent.time, buf);
            font_text_add(gHudTextObjs[5], buf);
            if (gGame.numPlayers == 4) {
                font_draw_text(gHudTextObjs[4], gScreenWidth / 2 - 10, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[5], gScreenWidth / 2 - 10, gScreenHeight / 2 + 7);
            } else {
                font_draw_text(gHudTextObjs[4], gScreenWidth - 65, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[5], gScreenWidth - 65, gScreenHeight / 2 + 7);
            }
            break;
        }
    }
    n = gEvent.flags & EVENT_SCORED;
    if (n != 0) {
        /* the goal, told to each view's team */
        team = n - 1;
        font_text_create(gHudTextObjs[7], 0x40, gGame.numPlayers == 4 ? 2 : 1, 4, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        if (team == (view >> ((gGame.numPlayers - 1) >> 1))) {
            if (gEvent.flags & EVENT_OWN_GOAL) {
                font_text_add(gHudTextObjs[7], "Team ");
                int_to_str((team ^ 1) + 1, buf);
                font_text_add(gHudTextObjs[7], buf);
                font_text_add(gHudTextObjs[7], " scored for you!");
            } else {
                font_text_add(gHudTextObjs[7], sStrYouScored);
            }
        } else {
            if (gEvent.flags & EVENT_OWN_GOAL) {
                strcpy(msg, "You helped team ");
                int_to_str(n, msg + strlen(msg));
                str_cat(msg, " score!");
            } else {
                strcpy(msg, "Team ");
                int_to_str(n, msg + strlen(msg));
                str_cat(msg, " scored!");
            }
            font_text_add(gHudTextObjs[7], msg);
        }
        font_draw_text(gHudTextObjs[7], gViewRects[gNumScreens - 1][view].x + gViewRects[gNumScreens - 1][view].w / 2,
                      gViewRects[gNumScreens - 1][view].y + gViewRects[gNumScreens - 1][view].h / 2);
    }
    return font_render(gdl);
}

/* Draws a string with the HUD's glyph images (gGame.hudGlyphs: the controller buttons), centred on y. */
Gfx *event_draw_glyph_string(Gfx *gdl, s32 fb, u8 *str, s32 x, s32 y) {
    while (*str != 0) {
        s32 w; s32 hh;

        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, (u32)gGame.hudGlyphs->palette);
        w = gGame.hudGlyphs->glyphs[*str].w << 3;
        hh = gGame.hudGlyphs->glyphs[*str].h;
        gdl = render_draw_image(x, y - (hh >> 1), &gGame.hudGlyphs->glyphs[*str], gdl, fb, 1, 1, 3, 0);
        str++;
        x += w;
    }
    return gdl;
}

/* The tutorial's HUD: the lesson's title sliding in and out, or "Press START" during a demo, the takedown or the
   clock, the lesson's name, the trick to do (its name and buttons) or the step's message, the bars, the previous
   and next lesson icons (grey when closed), the speed and the countdown. */
Gfx *tutorial_draw_hud(Gfx *gdl, s32 dyn, s32 fb, s32 view)
{
    TutorialLesson *lesson;
    TutorialStep *e;
    Gfx *p;
    u8 *img;
    u8 *img2;
    s32 off;
    u32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 d;
    s32 v;
    s32 n;
    s32 w;
    s32 x0;
    s32 x;
    s32 y;
    u8 flags;
    f32 speed;
    char buf[32];
    s32 cx;
    u8 *g;

    lesson = &gTutorialLessons[gSeason->round];
    font_reset_queue();
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    if (gEvent.state == EVENT_STATE_TITLE) {
        /* the title slides down in, and up out */
        if (gEvent.tutorialAction == TUTORIAL_SHOW) {
            off = -(gEvent.tutorialTimer * 2);
        } else {
            off = 0;
            if (gEvent.tutorialAction == TUTORIAL_HIDE) {
                off = -((60 - gEvent.tutorialTimer) * 2);
            }
        }
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, *(u32 *)(sTutorialTitleImage_2 + 4));
        cx = gScreenWidth / 2;
        w = sTutorialTitleImage_2[0x10] * 4 - 10;
        gdl = render_draw_image(cx - w, off + 0x1E, sTutorialTitleImage_2 + 0x10, gdl, fb, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gdl = render_draw_image(gScreenWidth / 2 - gFinishLineTexSet_2[0x18] * 4, off + 0x1A, gFinishLineTexSet_2 + 0x18, gdl, fb, 1, 1, 6, 0);
        /* i is not set here (an original bug) */
        font_text_create(gHudTextObjs[i], 0x40, 1, 2, TEXT_VALIGN_CENTER);
        font_text_add(gHudTextObjs[i], lesson->name);
        font_draw_text(gHudTextObjs[i], 0x4D - i * 3, i * 13 + 0x44 + off);
        font_text_create(gHudTextObjs[3], 0x40, 1, 2, TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER);
        /* "Lesson 1" with the level added to the digit */
        __builtin_memcpy(buf, sStrLesson1, 9);
        buf[strlen(buf) - 1] += lesson->level;
        font_text_add(gHudTextObjs[3], buf);
        font_draw_text(gHudTextObjs[3], gScreenWidth / 2 + 3, off + 0x29);
    } else {
        if (gActiveReplay != 0) {
            if (gFrameCounter & 0x20) {
                font_text_create(gHudTextObjs[5], 0x40, 1, 2, TEXT_VALIGN_CENTER);
                font_text_add(gHudTextObjs[5], "Press START");
                font_draw_text(gHudTextObjs[5], 0x2F, 0x2D);
            }
        } else {
            flags = LESSON.flags;
            if (flags & LESSON_TAKEDOWN) {
                font_text_create(gHudTextObjs[0], 0x40, 2, 6, TEXT_VALIGN_CENTER);
                if (gEvent.flags & EVENT_TAKEDOWN) {
                    font_text_add(gHudTextObjs[0], "Takedown Required");
                } else {
                    font_text_add(gHudTextObjs[0], "Takedown Complete");
                }
                font_draw_text(gHudTextObjs[0], 0x21, 0x2D);
            } else if (flags & LESSON_COUNTDOWN) {
              if (!(flags & LESSON_TRICKS)) {
                /* the time left; red (colour 8) once it is out */
                font_text_create(gHudTextObjs[0], 0x40, 1, 6, TEXT_VALIGN_CENTER);
                v = gEvent.time;
                if (v < 0) {
                    v = -v;
                }
                event_format_time_short(v, buf);
                font_text_add(gHudTextObjs[0], "Time: ");
                font_text_add(gHudTextObjs[0], buf);
                if (gEvent.time < 0) {
                    gHudTextObjs[0]->color = 8;
                }
                font_draw_text(gHudTextObjs[0], 0x21, 0x2D);
              }
            }
        }
        font_text_create(gHudTextObjs[1], 0x40, 1, 4, TEXT_VALIGN_CENTER);
        font_text_add(gHudTextObjs[1], lesson->name);
        font_draw_text(gHudTextObjs[1], 0x3D, 0x20);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        if ((LESSON.flags & LESSON_TRICKS) && sTutorialStep >= 2 && sTutorialStep < 10) {
            /* the trick to do: its name, and its buttons on a bar */
            font_text_create(gHudTextObjs[2], 0x40, 2, 2, TEXT_VALIGN_CENTER);
            font_text_add(gHudTextObjs[2], lesson->steps[sTutorialStep].lines[0]);
            font_draw_text(gHudTextObjs[2], 0x140, 0x28);
            p = gdl;
            gDPSetTextureLUT(p++, G_TT_NONE);
            gDPSetCombineMode(p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            k = 27;
            x0 = 0x2EE;
            g = gFinishLineTexSet_2 + 0x58;
            p = render_draw_image(x0 - g[0] * k * 8 - gFinishLineTexSet_2[0x60] * 8, 0x1C, gFinishLineTexSet_2 + 0x60, p, fb, 1, 1, 6, 0);
            for (m = 0; m < 27; m++) {
                p = render_draw_image(x0 - (k - m) * g[0] * 8, 0x1C, g, p, fb, 1, 1, 6, 0);
            }
            gdl = event_draw_glyph_string(p, fb, lesson->steps[sTutorialStep].lines[1], 0x140, 0x40);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        } else {
            if (gTutorialResult == 1) {
                /* passed: the last step's message */
                if (sTutorialStep == 0) {
                    i = lesson->numSteps - 1;
                } else {
                    i = sTutorialStep;
                }
                goto show;
            }
            if (gTutorialResult == -1) {
                for (j = 0; j < 3; j++) {
                    font_text_create(gHudTextObjs[j + 2], 0x40, 2, 2, TEXT_VALIGN_CENTER);
                    font_text_add(gHudTextObjs[j + 2], sTutorialFailText[j]);
                    font_draw_text(gHudTextObjs[j + 2], 0x20, 0xB2 + j * 9);
                }
            } else {
                if (gActiveReplay == 0 && !(gEvent.flags & EVENT_INTRO_MESSAGE)) {
                    goto skip;
                }
                if (!(gEvent.flags & EVENT_MESSAGE)) {
                    goto skip;
                }
                /* the message of the step whose time it is (or the first one, before the start) */
                for (i = 0; i < lesson->numSteps; i++) {
                    d = lesson->steps[i].value - gEventTime;
                    if (d <= 0) {
                        d = gEventTime - lesson->steps[i].value;
                    }
                    if (d >= 3 && !(gEventFlags & EVENT_INTRO_MESSAGE)) {
                        continue;
                    }
                    {
                    show:
                        n = 3;
                        if (lesson->steps[i].lines[2] == NULL) {
                            n = 2;
                            if (lesson->steps[i].lines[1] == NULL) {
                                n = 1;
                            }
                        }
                        for (j = 0; j < n; j++) {
                            font_text_create(gHudTextObjs[j + 2], 0x40, 2, 2, TEXT_VALIGN_CENTER);
                            font_text_add(gHudTextObjs[j + 2], lesson->steps[i].lines[j]);
                            font_draw_text(gHudTextObjs[j + 2], 0x20, 0xBB - (n - 1) * 9 / 2 + j * 9);
                        }
                        break;
                    }
                }
            }
            img = gFinishLineTexSet_2 + 0x30;
            for (i = 0; i < 19; i++) {
                gdl = render_draw_image(i * img[0] * 8 + 0x10, 0xA9, img, gdl, fb, 1, 1, 6, 0);
            }
            p = render_draw_image(img[0] * 0x98 + 0x10, 0xA9, gFinishLineTexSet_2 + 0x38, gdl, fb, 1, 1, 6, 0);
            gdl = event_draw_glyph_string(p, fb, D_800C9A3C, 0x136, 0xC3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        }
    skip:
        /* the bar at the top, in another colour during the timed lessons */
        if (gEvent.state == EVENT_STATE_FREE_RIDE || ((LESSON.flags & LESSON_COUNTDOWN) && !(LESSON.flags & LESSON_TRICKS))) {
            img = gFinishLineTexSet_2 + 0x30;
        } else {
            img = gFinishLineTexSet_2 + 0x40;
        }
        for (i = 0; i < 9; i++) {
            gdl = render_draw_image(i * img[0] * 8 + 0x10, 0x14, img, gdl, fb, 1, 1, 6, 0);
        }
        if (gEvent.state == EVENT_STATE_FREE_RIDE || ((LESSON.flags & LESSON_COUNTDOWN) && !(LESSON.flags & LESSON_TRICKS))) {
            img2 = gFinishLineTexSet_2 + 0x38;
        } else {
            img2 = gFinishLineTexSet_2 + 0x48;
        }
        gdl = render_draw_image(img[0] * 0x48 + 0x10, 0x14, img2, gdl, fb, 1, 1, 6, 0);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        if (gSeason->round > 0) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(sTutorialPrevNextIcons + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, sGreyPalette);
        }
        gdl = render_draw_image(0x21, 0x1B, sTutorialPrevNextIcons + 0x20, gdl, fb, 1, 1, 3, 0);
        if (gSeason->round < TUTORIAL_NUM_LESSONS - 1 && gTutorialLessons[gSeason->round + 1].level <= gSeason->cup->progress[0]) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(sTutorialPrevNextIcons + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, sGreyPalette);
        }
        gdl = render_draw_image(0x96, 0x1B, sTutorialPrevNextIcons + 0x10, gdl, fb, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    }
    gdl = render_draw_temp_gauge(gdl, dyn, fb, 0);
    font_text_create(gHudTextObjs[6], 0x40, 3, 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_RIGHT);
    speed = gRiders[view].speed;
    int_to_str((s32)((speed > 0.0f ? speed * 3600.0f : -speed * 3600.0f) / 1612.9f), buf);
    font_text_add(gHudTextObjs[6], buf);
    x = 0x1BA;
    if (gViMode == VI_MODE_HI_RES) {
        x = 0x228;
    }
    y = 0xD6;
    if (gViMode == VI_MODE_HI_RES) {
        y = 0x180;
    }
    font_draw_text(gHudTextObjs[6], x, y);
    if ((LESSON.flags & LESSON_COUNTDOWN) && gEvent.countdown != 0
        && (gEvent.state < EVENT_STATE_GO || gEvent.state == EVENT_STATE_TIMED || gEvent.state == EVENT_STATE_FREE_RIDE)
        && (gEvent.state != EVENT_STATE_READY || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, view);
    }
    return font_render(gdl);
}

/* Whether the next lesson is open: one of the same level, or of a level reached. */
s32 tutorial_next_lesson_unlocked(void) {
    Season *season = gSeason;
    s32 cur = season->round;
    s32 next = cur + 1;
    u8 curLevel, nextLevel;

    if (next > TUTORIAL_NUM_LESSONS - 1) {
        return 0;
    }
    curLevel = gTutorialLessons[cur].level;
    nextLevel = gTutorialLessons[next].level;
    if (curLevel == nextLevel) {
        return 1;
    }
    return curLevel < season->cup->progress[0];
}

/* the checkerboard's rows and columns, and the winner's view it covers */
#define ROWS (gGame.numPlayers >= 2 ? 6 : 9)
#define COLS (gGame.numPlayers >= 3 ? 8 : 12)
#define RECT (gViewRects[gNumViews - 1][gEvent.finishOrder[0]])


/* The HUD of the current view for the mode (or only the countdown at the last HUD level), the checkerboard that
   wipes over the winner's view after the finish (its squares fading in column by column), and the pause menus. */
Gfx *event_draw_hud(Gfx *gdl, s32 dyn, s32 fb, s32 view) {
    int i;
    int j;
    s32 a;
    s32 prev;
    s32 sp;
    int n;

    if (gHudLevel != 3 || gGame.mode == MODE_TUTORIAL) {
        switch (gGame.mode) {
        case MODE_SOCCER:
            if (gPauseMenu == PAUSE_NONE) {
                gdl = event_draw_hud_soccer(gdl, dyn, fb, view);
            }
            break;
        case MODE_DESERT:
            gdl = event_draw_hud_gates(gdl, dyn, fb, view);
            break;
        case MODE_HILL_CLIMB:
            gdl = event_draw_hud_hillclimb(gdl, dyn, fb, view);
            break;
        case MODE_STUNT:
            gdl = event_draw_hud_stunt(gdl, dyn, fb, view);
            break;
        case MODE_TEST_RIDE:
        case MODE_TEST_RIDE_OPEN:
            gdl = event_draw_hud_testride(gdl, dyn, fb, view);
            break;
        case MODE_TUTORIAL:
            gdl = tutorial_draw_hud(gdl, dyn, fb, view);
            break;
        default:
            gdl = event_draw_hud_race(gdl, dyn, fb, view);
            break;
        }
    } else if (gReplayPlayback == 0) {
        gdl = event_draw_countdown(gdl, view);
    }
    if (gEvent.finishTimer >= 0 || gEvent.finishTimer == -2) {
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
        /* the black squares */
        prev = 35;
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, prev);
        for (i = 0; i < ROWS; i++) {
            for (j = i & 1; j < COLS; j += 2) {
                if (gEvent.finishTimer == -2) {
                    a = 35;
                } else {
                    n = gEvent.finishTimer - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (gCosTable[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 15) >> 12;
                    } else {
                        a = ((gCosTable[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 25) >> 14) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
        /* the white squares */
        gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, 35);
        prev = 35;
        for (i = 0; i < ROWS; i++) {
            for (j = ~i & 1; j < COLS; j += 2) {
                if (gEvent.finishTimer == -2) {
                    a = 35;
                } else {
                    n = gEvent.finishTimer - j * 10;
                    if (n < 0) {
                        a = 0;
                    } else if (n >= 30) {
                        a = 35;
                    } else if (n < 15) {
                        a = (gCosTable[(((n << 14) / 15 >> 4) - 0x400) & 0xFFF] * 255) >> 14;
                    } else {
                        a = ((gCosTable[(((n - 15) << 14) / 15 >> 4) & 0xFFF] * 55) >> 12) + 35;
                    }
                }
                if (a != prev) {
                    gDPSetPrimColor(gdl++, 0, 0, 200, 200, 200, a);
                    prev = a;
                }
                gDPFillRectangle(gdl++, RECT.x + j * RECT.w / COLS, RECT.y + i * RECT.h / ROWS,
                                 RECT.x + (j + 1) * RECT.w / COLS, RECT.y + (i + 1) * RECT.h / ROWS);
            }
        }
    }
    /* the pause menus, sp apart */
    sp = 14;
    if (gViMode_2 == VI_MODE_HI_RES) {
        font_set_scale(0x10000, 0x20000, 0, 1);
        sp = 28;
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                  gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
    font_reset_queue();
    switch (gPauseMenu) {
    case PAUSE_TEST_RIDE:
        for (n = 0; n < 3; n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, TEXT_HALIGN_CENTER);
            font_text_add(gMenuTextObjs[n], sTestRideMenuItems[n]);
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp + sp * n);
        }
        break;
    case PAUSE_MAIN:
        for (n = 0; n < (gGame.mode == MODE_SEASON ? 4 : 5); n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, TEXT_VALIGN_BASELINE | TEXT_HALIGN_CENTER);
            if (gGame.mode == MODE_SEASON) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSeason[n]);
            } else if (gGame.mode == MODE_SOCCER) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSoccer[n]);
            } else {
                font_text_add(gMenuTextObjs[n], sPauseMenuItems[n]);
            }
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp * 2 + sp * n);
        }
        break;
    case PAUSE_TUTORIAL:
        if (gEvent.tutorialAction != TUTORIAL_RETRY) {
            s32 k;
            k = gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp * 2;
            for (n = 0; n < 5; n++) {
                if (n == TUTORIAL_ITEM_DEMO && gTutorialLessons[gSeason->round].demo == 0) {
                    continue;
                }
                if (n == TUTORIAL_ITEM_NEXT && tutorial_next_lesson_unlocked() == 0) {
                    continue;
                }
                if (n == TUTORIAL_ITEM_PREV && gSeason->round == 0) {
                    continue;
                }
                font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, TEXT_HALIGN_CENTER);
                font_text_add(gMenuTextObjs[n], sTutorialMenuItems[n]);
                font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2, k);
                k += sp;
            }
        }
        break;
    case PAUSE_QUIT:
        for (n = 0; n < 2; n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, TEXT_HALIGN_CENTER);
            font_text_add(gMenuTextObjs[n], sQuitConfirmItems[n]);
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 + sp * n);
        }
        break;
    }
    gdl = font_render(gdl);
    font_set_scale(0x10000, 0x10000, 0, 0);
    return gdl;
}

/* the current view's rectangle in a Rect table T laid out like gViewRects */
#define RP_2(T, n) ((Rect *)(((gCurView << 4) + (((n) - 1) << 6)) + (u8 *)T))
#define RX(T, n) (RP_2(T, n)->x + RP_2(T, n)->w)
#define RY(T, n) (RP_2(T, n)->y + RP_2(T, n)->h)
/* an RGBA5551 colour's 8-bit channels */
#define R8(c) (((c) >> 8) & 0xF8)
#define G8(c) (((c) >> 3) & 0xF8)
#define B8(c) (((c) << 2) & 0xF8)
#define GW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 w, h; u16 pad; void *addr; } GaugeImg;
/* sTempGaugeTextures: an image set (palette, then the images), one colour each */
typedef struct { s32 f0; u16 *pal; GaugeImg img[5]; } GaugeImages;

extern GaugeImages *sTempGaugeTextures_2;
extern GaugeImages *sTempGaugeTextures_3[];

/* The engine temperature gauge of a split-screen view, at its bottom right: the frame and the bar in three
   layers, cut to the heat (38 rows at 12000), blinking red when the engine overheats. */
Gfx *event_draw_temp_gauge(Gfx *gdl, s32 dyn, s32 fb, s32 view) {
    s32 yOfs, n, bar, k;
    u16 col;
    GaugeImg *img;
    Rect *r;

    yOfs = -54;
    n = gNumScreens;
    if (n == 2) yOfs = -46;
    bar = gRiders[view].engine.heat * 38.0f / 12000.0f;
    if (bar >= 39) bar = 38;
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    /* the layers dimmed (a quarter of their colour) */
    gDPSetPrimColor(gdl++, 0, 0, R8(sTempGaugeTextures_2->pal[0]) >> 2, G8(sTempGaugeTextures_2->pal[0]) >> 2,
                    B8(sTempGaugeTextures_2->pal[0]) >> 2, 0x8C);
    { Rect *tab = gViewLayouts_3; gdl = render_draw_image(RX(tab, n) - 24, RY(tab, n) + yOfs, &sTempGaugeTextures_2->img[0], gdl, fb, 1, 1, 6, 0); }
    gDPSetPrimColor(gdl++, 0, 0, R8(sTempGaugeTextures_2->pal[4]) >> 2, G8(sTempGaugeTextures_2->pal[4]) >> 2,
                    B8(sTempGaugeTextures_2->pal[4]) >> 2, 0xA0);
    gdl = render_draw_image(RX(gViewLayouts_3, gNumScreens) - 24, RY(gViewLayouts_3, gNumScreens) + yOfs, &sTempGaugeTextures_2->img[4], gdl, fb, 1, 1, 6, 0);
    bar++;
    gDPSetPrimColor(gdl++, 0, 0, R8(sTempGaugeTextures_2->pal[3]) >> 2, G8(sTempGaugeTextures_2->pal[3]) >> 2,
                    B8(sTempGaugeTextures_2->pal[3]) >> 2, 0x8C);
    gdl = render_draw_image(RX(gViewLayouts_3, gNumScreens) - 24, RY(gViewLayouts_3, gNumScreens) + yOfs, &sTempGaugeTextures_2->img[3], gdl, fb, 1, 1, 6, 0);
    /* the layers again in full colour, bar rows of them */
    for (k = 0; k < 3; k++) {
        switch (k) {
        case 0:
            img = &sTempGaugeTextures_2->img[0];
            col = sTempGaugeTextures_2->pal[0];
            break;
        case 1:
            img = &sTempGaugeTextures_2->img[4];
            col = sTempGaugeTextures_2->pal[4];
            break;
        case 2:
            img = &sTempGaugeTextures_2->img[3];
            col = sTempGaugeTextures_2->pal[3];
            break;
        }
        /* load bar rows of the IA8 image (set texture image, tile, load tile, render tile, tile size) */
        GW(gdl++, 0xFD680000 | ((img->w * 8 - 1) & 0xFFF), img->addr);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0x07000000);
        GW(gdl++, 0xE6000000, 0);
        GW(gdl++, 0xF4000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | 0x07000000 | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0);
        GW(gdl++, 0xF2000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xFA000000, (((col >> 8) & 0xF8) << 24) | (((col >> 3) & 0xF8) << 16) | ((col << 10) & 0xF800) | 0xFF);
        r = RP_2(gViewRects, gNumScreens);
        /* a texture rectangle, upside down, scaled from 512x240 to the screen */
        GW(gdl++, 0xE4000000 | ((((r->x + r->w) * 4 + (img->w * 8 - 24) * (gScreenWidth << 2) / 512) & 0xFFF) << 12) |
                      (((r->y + r->h) * 4 + (yOfs + img->h) * (gScreenHeight << 2) / 240) & 0xFFF),
           ((((r->x + r->w) * 4 + -24 * (gScreenWidth * 4) / 512) & 0xFFF) << 12) |
               (((r->y + r->h) * 4 + (yOfs + img->h - bar) * (gScreenHeight << 2) / 240) & 0xFFF));
        GW(gdl++, 0xE1000000, ((bar - 1) << 5) & 0xFFFF);
        GW(gdl++, 0xF1000000, ((gScreenWidth == 640) ? 0x03330000 : 0x04000000) | ((gScreenHeight == 240) ? 0xFC00 : 0xFE00));
        GW(gdl++, 0xE7000000, 0);
    }
    if (gRiders[view].engine.overheated) {
        /* red, its alpha ramping up and down every 32 frames */
        GaugeImages *h = sTempGaugeTextures_3[0]; GW(gdl++, 0xFA000000, (((h->pal[2] >> 8) & 0xF8) << 24) | (((gFrameCounter << 3) & 0x100) ? ((gFrameCounter << 3) & 0xFF) : (0xFF ^ ((gFrameCounter << 3) & 0xFF))));
            { Rect *tab = gViewLayouts_3; gdl = render_draw_image(RX(tab, gNumScreens) - 24, RY(tab, gNumScreens) + yOfs, &sTempGaugeTextures_2->img[2], gdl, fb, 1, 1, 6, 0); }
    }
    /* the frame */
    GW(gdl++, 0xFA000000, (((sTempGaugeTextures_2->pal[1] >> 8) & 0xF8) << 24) | (((sTempGaugeTextures_2->pal[1] >> 3) & 0xF8) << 16) | ((sTempGaugeTextures_2->pal[1] << 10) & 0xF800) | 0x82);
    { Rect *tab = gViewLayouts_3; return render_draw_image(RX(tab, gNumScreens) - 24, RY(tab, gNumScreens) + yOfs, &sTempGaugeTextures_2->img[1], gdl, fb, 1, 1, 6, 0); }
}

s32 tutorial_has_prev_lesson(void)
{
    return gSeason->round != 0;
}

typedef struct {
    s32 limit;
    s32 counter;
    s32 buf;
} RaceSeriesCount;

extern RaceSeriesCount gRaceSeries_3;

/* The event's exit: counts the race series, frees the tutorial's buffer, and saves. */
void event_exit(void)
{
    if (0 < gRaceSeries_3.limit && (gGame.flags & GAME_REPEAT_STATE) == 0) {
        gRaceSeries_3.counter = gRaceSeries_3.counter + 1;
        if (gRaceSeries_3.counter == gRaceSeries_3.limit) {
            mem_free(gRaceSeries_3.buf);
        }
    }
    if (gGame.mode == MODE_TUTORIAL && gEvent.tutorialBuf != 0) {
        mem_free(gEvent.tutorialBuf);
    }
    save_write(1, 0);
}

/* Loads the current track's resource of a group (a gGid* id), through the track's resource map; 0 if it has none. */
u32 track_res_get(s32 key)
{
    Pair_4 *p = sTrackResMap;
    s32 idx;
    if (p == NULL) {
        idx = -1;
    } else {
        for (; p->val >= 0; p++) {
            if (p->key == key) {
                idx = ((volatile Pair_4 *)p)->val;
                goto found;
            }
        }
        idx = -1;
    }
found:
    if (idx < 0) {
        return 0;
    }
    return res_load(gResourceSet, idx);
}

/* Loads the track's resource map: {group id, resource} pairs, ended by a negative resource. */
s32 track_res_load_map(s32 *resId) {
    s32 ok = 0;
    if (resId != NULL) {
        sTrackResMap = res_load(gResourceSet, *resId);
        ok = sTrackResMap != 0;
    }
    return ok;
}

/* Whether the lesson is passed: the takedown done, or time left (never for a lesson with a demo). */
u32 tutorial_lesson_passed(void) {
    TutorialLesson *lesson = gTutorialLessons;
    s16 idx = gSeason->round;
    lesson = lesson + idx;
    if (lesson->flags & LESSON_TAKEDOWN) {
        return ((gEvent.flags >> 12) ^ 1) & 1; /* !(flags & EVENT_TAKEDOWN) */
    }
    if (lesson->demo != 0) {
        return 0;
    }
    return (u32)~gEvent.time >> 31; /* time >= 0 */
}

/* Fades out to leave the race, if it is running. */
s32 event_request_exit(void)
{
    if (gRacePhase == RACE_PHASE_RACING) {
        gRacePhase = RACE_PHASE_FADE_OUT;
        gRacePhaseTimer = 20;
        return 1;
    }
    return 0;
}

void event_gates_init(void) {
    gEvent.flags |= EVENT_STOPPED;
    race_reset_grid_order();
}

void event_stop_sounds_rumble(void)
{
    snd_stop_all_riders();
    rumble_stop_all();
}

/* The desert: the clock, which stops at 60 minutes; START leaves at the end. */
void event_update_gates(void)
{
    Event *event;

    event_update_countdown();
    event = &gEvent;
    if (event->allFinished == 0) {
        if ((event->flags & EVENT_STOPPED) == 0) {
            s32 n = event->time + 1;
            event->time = n;
            if (n == 0x34BC0) {
                event_rider_finished(0);
                event->allFinished = 1;
            }
        }
    } else if ((gPlayerInputs_2 & INPUT_START) != 0 && (gPlayerInputsPrev & INPUT_START) == 0) {
        event_request_exit();
    }
}

void event_hillclimb_init(void) {
    Rider *rider;

    gEvent.unk_48 = -1;
    gEvent.flags |= EVENT_STOPPED;
    race_reset_grid_order();
    for (rider = gRiders; rider < &gRiders[gGame.numPlayers]; rider++) {
        rider->timeLimit = 7200;
    }
}

void event_stunt_init(void)
{
    Event *event = &gEvent;
    event->flags = event->flags | EVENT_STOPPED;
    event->time = 7200;
    race_reset_grid_order();
}

void event_soccer_init(void)
{
    Event *event;
    s32 flags;

    event = &gEvent;
    flags = event->flags;
    event->countdown = 180;
    gEvent.state = EVENT_STATE_KICKOFF;
    event->ballToucher = 0;
    event->goals[0] = 0;
    event->goals[1] = 0;
    event->goalTimer = 0;
    event->flags = flags | EVENT_STOPPED;
}

/* No rider has finished yet. */
void event_reset_places(void) {
    Event *event = &gEvent;
    s32 i;
    RiderRace *race;

    if (event->state == 8) {
        race_set_grid_order(2, 0);
    }
    i = 0;
    event->numFinished = 0;
    if (event->numRiders > 0) {
        s16 none = -1;
        Event *e = event;
        race = &gRiders[0].race;
        do {
            race->finishPlace = none;
            race = (RiderRace *)((u8 *)race + sizeof(Rider));
        } while (++i < e->numRiders);
    }
}

/* Quitting or restarting a season race: the riders who finished lose this race's points. */
void event_clear_season_race_points(void) {
    s32 i;
    for (i = 0; i < gEvent.numRiders; i++) {
        if (gRiders[i].race.finishPlace != -1) {
            gSeason->racePoints[i] = 0;
        }
    }
}

void func_8005B2E8(void)
{
    event_update_countdown();
    if (gEvent.state == EVENT_STATE_GO) {
        if (gEvent.time != 0) {
            s32 r = gEvent.time - 1;
            gEvent.time = r;
            if (r == 0) {
                gRiders[0].race.lapMsgTimer = 120;
            }
        }
    }
}

/* Frees what event_hud_init loaded and created. */
void event_hud_free(void)
{
    s32 i;
    TextObj **p1;
    TextObj **p2;

    if (gGame.mode == MODE_TUTORIAL) {
        mem_free(sGreyPalette);
    }
    if (gGame.numPlayers == 1) {
        mem_free(D_800F0730[0]);
        mem_free(D_800F0730[1]);
    }
    i = 0;
    p2 = gMenuTextObjs;
    p1 = gHudTextObjs;
    do {
        i++;
        font_text_destroy(*p1);
        mem_free(*p1);
        p1++;
        font_text_destroy(*p2);
        mem_free(*p2);
        p2++;
    } while (i < HUD_NUM_TEXTS);
    return;
}

/* Draws a bar of count segments of the finish-line images ending at x, with its end piece. */
Gfx *tutorial_draw_bar(s32 x, s32 y, s32 count, Gfx *dl, s32 fb)
{
    s32 i = 0;
    s32 v;
    u8 *base;
    u8 *seg;

    gDPSetTextureLUT(dl++, G_TT_NONE);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    base = gFinishLineTexSet;
    seg = base + 0x58;
    dl = (Gfx *)render_draw_image(x - count * *seg * 8 - base[0x60] * 8, y, base + 0x60, (u32 *)dl, fb, 1, 1, 6, 0);
    for (i = 0; i < count; i++) {
        v = count - i;
        dl = (Gfx *)render_draw_image(x - v * *seg * 8, y, seg, (u32 *)dl, fb, 1, 1, 6, 0);
    }
    return dl;
}

s32 tutorial_lesson_has_demo(void)
{
    s32 idx = gSeason->round;
    return gTutorialLessons[idx].demo != 0;
}

/* An entry of the track's resource map */
typedef struct TrackResMapEntry {
    s32 key;
    s32 val;
} TrackResMapEntry;

/* The resource of a group in the track's resource map, or -1. */
s32 track_res_find(s32 key)
{
    TrackResMapEntry *p = sTrackResMap;
    if (p == NULL) {
        return -1;
    }
    while (p->val >= 0) {
        if (p->key == key) {
            return ((volatile TrackResMapEntry *)p)->val;
        }
        p++;
    }
    return -1;
}
