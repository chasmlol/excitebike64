#include "game.h"

#define gDPRaw(pkt, a, b) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = (a); \
    _g->words.w1 = (b); \
}

typedef struct {
    u8 *f0;
    u8 p04[0x60];
    s16 f64;
} Prof;

typedef struct { s32 a, b; s16 f8; u8 pad[0x7E0-0xA]; } BikeX;

extern Prof *gSeason_5;
extern BikeX D_800E3E20[];

#define CUR (gTutorialLessons_8[*(s16 *)(gSeason + 0x64)])

typedef struct {
    u8 p0[0xC];
    u8 fl;
    u8 p1[7];
    s32 w14;
    u8 p2[0x28 - 0x18];
} Ent_54;

extern Ent_54 gTutorialLessons_8[];

void tutorial_init_lesson(s32 a0)
{
    Event *s;

    race_reset_grid_order();
    s = &gEvent;
    if (s->tutorialBuf == 0) {
        s->tutorialBuf = mem_alloc_dbg(0x80, 0, (s32)D_800D7340, 0x16D);
    }
    gGame.difficulty = 1;
    if (CUR.fl & 4) {
        s->time = 0;
        s->flags |= 0x1000;
    } else {
        s->flags &= ~0x1000;
    }
    if (CUR.fl & 8) {
        gEvent.countdown = 0x3F;
        gEvent.state = 1;
        gEvent.flags |= 0x10;
    } else {
        gEvent.flags &= ~0x10;
    }
    gEvent.flags &= ~0x4000;
    if (CUR.fl & 8) {
        gEvent.flags |= 0x18800;
        gEvent.tutorialTimeout = 600;
        if (CUR.fl & 2) {
            gEvent.time = 0;
        } else if (gActiveReplay == 0) {
            gEvent.time = CUR.w14;
        }
    } else {
        gEvent.state = 0xE;
        gEvent.time = 0;
    }
    if (gActiveReplay == 0) {
        gEvent.tutorialAction = 5;
        gEvent.flags &= ~0x40;
    } else {
        gEvent.countdown = 0x3F;
        gEvent.tutorialAction = 0;
        gEvent.tutorialTimer = 0x3C;
        gEvent.flags |= 0x40;
    }
    D_800F0738 = 0;
    gTutorialResult = 0;
    gTutorialMenuForced = 0;
}

void tutorial_check_lesson_skip(void)
{
    if (gRacePhase == 5) {
        if (((gPadInput & 0x200) != 0) && ((gPadInputPrev & 0x200) == 0)) {
            if (*(s16 *)(gSeason + 0x64) < 1) {
                return;
            }
            gEvent.tutorialAction = 3;
            gRacePhase = 6;
            gRacePhaseTimer = -1;
            snd_play_menu_sound(1);
            return;
        }
        if ((gPadInput & 0x400) == 0) {
            return;
        }
        if ((gPadInputPrev & 0x400) != 0) {
            return;
        }
        if (tutorial_next_lesson_unlocked() == 0) {
            return;
        }
        gEvent.tutorialAction = 4;
        gRacePhase = 6;
        gRacePhaseTimer = -1;
        snd_play_menu_sound(1);
    }
}

#define TB (gTutorialLessons_9[gSeason_5->f64])
#define SEG(i) ((Seg_13 *)((s32)((i) * 0x6C) + (s32)gPathNodes))

typedef struct {
    s32 v;
    u8 pad[0xC];
} Gate;

typedef struct {
    u8 p00[0xC];
    u8 fl;          /* 0x0C */
    u8 fD;          /* 0x0D */
    u8 p0E[0xA];
    s16 f18;        /* 0x18 */
    s16 f1A;        /* 0x1A */
    u8 p1C[4];
    s32 n;          /* 0x20 */
    Gate *list;     /* 0x24 */
} Track_11;

typedef struct {
    u8 p000[0x3D2];
    s8 f3D2;
    u8 p3D3[0x28];
    u8 f3FB;
    u8 p3FC[0x22C];
    s16 f628;
    u8 p62A[0x66];
    u32 f690;
    u8 p694[0x14C];
} Bike_29;

typedef struct {
    u8 p00[0x4A];
    s16 f4A;
    u8 p4C[0x20];
} Seg_13;

extern Track_11 gTutorialLessons_9[];
extern Bike_29 gRiders_54[];

void tutorial_update(void)
{
    s32 i;

    D_800E43EC = 1.0375f;
    if (!(gEvent.flags & 0x800)) {
        if ((TB.fl & 8) && (gEvent.state < 5 || gEvent.state == 15 || gEvent.state == 14)) {
            event_update_countdown();
        }
        if (gEvent.countdown == 0x2B && gEvent.state == 1 && (gEvent.flags & 0x8000)) {
            gEvent.tutorialTimeout = 300;
            gEvent.flags |= 0x10800;
        }
    } else {
        snd_stop_all_riders();
    }
    if (gEvent.state == 5) {
        gEvent.flags &= ~0x10;
        if (gActiveReplay != 0 && gRiderAlpha_2 != 0xFF) {
            gEvent.tutorialAction = 1;
            gEvent.state = 14;
        } else {
            gEvent.state = 15;
            gEvent.tutorialAction = 5;
        }
    }
    if (gEvent.state >= 5 && !(gEvent.flags & 0x800)) {
        if (gActiveReplay == 0) {
            if (TB.fl & 2) {
                gEvent.time++;
            } else if (!(D_800E3E50 & 0x10)) {
                gEvent.time--;
            }
        } else {
            for (i = 0; i < TB.n; i++) {
                if (gEvent.time == TB.list[i].v) {
                    snd_stop_all_riders();
                    rumble_stop_all();
                    gEvent.tutorialTimeout = 1800;
                    gEvent.flags |= 0x800;
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
    if ((gEvent.tutorialAction == 0 || gEvent.tutorialAction == 2) && gEvent.tutorialTimer != 0) {
        gEvent.tutorialTimer--;
    }
    if (gEvent.state < 5) {
        tutorial_check_lesson_skip();
        return;
    }
    switch (gEvent.state) {
    case 13:
        switch (gEvent.tutorialAction) {
        case 0:
            if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
                gEvent.tutorialAction = 2;
                gEvent.tutorialTimer = 59 - gEvent.tutorialTimer;
            } else if (gEvent.tutorialTimer == 0) {
                gEvent.tutorialAction = 1;
            }
            break;
        case 1:
            if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
                gEvent.tutorialAction = 2;
                gEvent.tutorialTimer = 60;
                gEvent.flags = (gEvent.flags & ~0x10) | 0x40;
            } else {
                tutorial_check_lesson_skip();
            }
            break;
        case 2:
            if (gEvent.tutorialTimer == 0) {
                gEvent.state = 14;
                gEvent.countdown = 0x3F;
                gEvent.tutorialAction = 0;
                gEvent.tutorialTimer = 60;
            }
            break;
        }
        break;
    case 14:
        switch (gEvent.tutorialAction) {
        case 0:
            if (gEvent.tutorialTimer == 0) {
                gEvent.tutorialAction = 1;
                return;
            }
            break;
        case 1:
            if ((gPadInput & 0x40) && !(gPadInputPrev & 0x40)) {
                gEvent.tutorialAction = 6;
                gRacePhase = 6;
                gRacePhaseTimer = -1;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
                return;
            }
            if (gRiders_54[0].f628 == TB.f1A) {
                if (gRiders_54[0].f690 & 0x10) {
                    if (gEvent.flags & 0x2000) {
                        bikes_free();
                        decal_clear_all();
                        mem_fill((u8 *)gRiders_54, 0, sizeof(Bike_29) * 8);
                        tutorial_init_lesson(0);
                        bikes_init();
                        replay_play_start(gActiveReplay);
                        gEvent.flags = (gEvent.flags | 0x80) & ~0x2000;
                    }
                } else {
                    gRiders_54[0].f690 |= 0x10;
                    gEvent.flags &= ~0x2000;
                }
                return;
            }
            if ((TB.fl & 4) && gRiders_54[1].f628 >= TB.f1A &&
                (TB.f18 < TB.f1A || (gRiders_54[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
                gRiders_54[1].f690 |= 0x10;
                return;
            }
            break;
        case 5:
            if (gRiders_54[0].f628 == TB.f1A) {
                gRiders_54[0].f690 |= 0x10;
            }
            if ((gEvent.flags & 0x2000) && gPauseMenu != 5) {
                gPauseMenuCursor = 0;
                gPauseMenu = 5;
                gTutorialMenuForced = 1;
            }
            break;
        }
        tutorial_check_lesson_skip();
        break;
    case 15:
        if (TB.fl & 2) {
            if (gEvent.time >= 3 && D_800F0738 == 0) {
                D_800F0738 = 2;
            }
            if (gRiders_54[0].f3FB == 0 && gRiders_54[0].f3D2 == TB.list[D_800F0738].v) {
                D_800F0738++;
            }
            if (D_800F0738 >= 10) {
                if (gEvent.flags & 0x2000) {
                    gTutorialResult = 1;
                    if (((gPadInput & 0x40) && !(gPadInputPrev & 0x40)) ||
                        ((gPadInput & 0x10) && !(gPadInputPrev & 0x10))) {
                        if (D_800F0738 == 10) {
                            D_800F0738 = 11;
                        } else if (D_800F0738 == 11) {
                            gSeason_5->f0[1] = TB.fD + 1;
                            D_800F1BC1 = D_800CAAA1;
                            event_request_exit();
                            gRaceExitCode = 2;
                            gPauseMenu = 0;
                            gGame.flags &= ~4;
                            return;
                        }
                    }
                    if (D_800F0738 < 10) {
                        D_800F0738 = 10;
                    }
                } else if (!(gRiders_54[0].f690 & 0x10)) {
                    gRiders_54[0].f690 |= 0x10;
                }
            }
        }
        tutorial_check_lesson_skip();
        if (D_800E3DE8 == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            if (event_request_exit() != 0) {
                gEvent.tutorialAction = 6;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
            }
            return;
        }
        if ((u32)(gRiders_54[0].f628 - TB.f1A) < 10 || (gEvent.flags & 0x2000)) {
            if (gRiders_54[0].f690 & 0x10) {
                if (gSeason_5->f64 < 16 && TB.fD != gTutorialLessons_9[gSeason_5->f64 + 1].fD &&
                    tutorial_lesson_passed_2()) {
                    if (TB.fD == gSeason_5->f0[1]) {
                        gSeason_5->f0[1] = TB.fD + 1;
                        D_800F1BC1 = D_800CAAA1;
                    }
                }
                if ((gEvent.flags & 0x2000) && D_800F0738 == 0) {
                    snd_stop_all_riders();
                    gEvent.flags &= ~0x2000;
                    gTutorialResult = tutorial_lesson_passed_2() ? 1 : -1;
                    gPauseMenu = 5;
                    gPauseMenuCursor = tutorial_lesson_passed_2() ? 3 : 0;
                }
            } else {
                gRiders_54[0].f690 |= 0x10;
            }
        } else if (gRiders_54[0].f628 == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            gEvent.tutorialAction = 6;
            gRacePhase = 6;
            gRacePhaseTimer = -1;
            gActiveReplay = 0;
            gDemoPlayback = 0;
            sDemoActive = 0;
            gReplayRecording = 0;
        } else if ((TB.fl & 4) && gRiders_54[1].f628 >= TB.f1A &&
                   (TB.f18 < TB.f1A || (gRiders_54[1].f628 < TB.f18 - 2 && TB.f1A < TB.f18))) {
            gRiders_54[1].f690 |= 0x10;
        }
        break;
    }
}

typedef struct {
    u8 pad[0x34];
    TrackRecord record; /* 0x34 */
    s32 f50;        /* 0x50 */
    u8 pad54[0xA0 - 0x54];
} Track_12;

typedef struct {
    Ent24 *p;
    u8 pad4[0x62];
    u8 b66[1];
} Race_3;

typedef struct {
    u8 pad[0x660];
    u8 pad660[8];
    s16 f668;
    u8 pad66A[0x69C - 0x66A];
    s32 f69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Bike_30;

typedef struct { s32 n; s32 f4; s32 *p; } Alloc;

extern Track_12 *gCurTrack_28;
extern Race_3 *gSeason_9;
extern Track_12 gTracks_10[];
extern Alloc D_800CA924_2;
extern Bike_30 gRiders_55[];

void event_init(s32 a0, s32 a1)
{
    Bike_30 *b;
    s32 i;
    s32 j;

    sRaceEndFrame = -1;
    mem_fill_2(gEvent.lapLeaderTimes, -1, 12);
    if (gGame.mode == 1) {
        for (i = 0; i < gEvent.numRiders; i++) {
            gSeason_9->b66[i] = 0;
        }
    }
    gRaceExitCode = 6;
    *(TrackRecord *)&gEvent.record = gCurTrack_28->record; /* a copy through a pointer: the base register differs otherwise */
    gCurTrack_28->f50 &= ~0x180;
    gEvent.finishTimer = -1;
    gEvent.flags = 0;
    gEvent.allFinished = 0;
    gEvent.messageTimer = 0;
    gEvent.camTarget = 0;
    gEvent.idleFrames = 0;
    gEvent.numRiders = a0;
    gEvent.numLaps = a1;
    if (gGame.mode == 1 && !(gGame.flags & 0x81) &&
        gSeason_9->p - D_800CA9C8 == 4) {
        gEvent.flags = 0x200;
        gEvent.numRiders = 2;
        gNumRiders = 2;
    }
    gEvent.countdown = 0x96;
    if (gCreditsMode != 0 || (gGame.flags & 0x81) ||
        gGame.mode == 0xD || gGame.mode == 0xE) {
        gEvent.state = 5;
    } else {
        if ((gGame.mode == 0 || gGame.mode == 1) &&
            gCurTrack_28 - gTracks_10 != 9) {
            gEvent.state = 0;
            gEvent.countdown = 0x100;
        } else {
            gEvent.state = 1;
            gEvent.countdown = 0x96;
        }
        gEvent.flags |= 0x10;
    }
    gEvent.time = 0;
    gEvent.noPlayers = (gGame.flags & 0x81) != 0;
    mem_fill_2(gRiders_55, 0, 0x3F00);
    gGame.flags &= ~4;
    if (D_800CA924_2.n > 0 && D_800CA924_2.f4 == 0) {
        D_800CA924_2.p = mem_calloc_dbg(D_800CA924_2.n * 4, 0, (s32)D_800D7340, 0x6D9);
    }
    switch (gGame.mode) {
    case 2:
        gEvent.countdown = 0xB4;
        gEvent.state = 6;
        gEvent.ballToucher = 0;
        gEvent.goals[0] = 0;
        gEvent.goals[1] = 0;
        gEvent.goalTimer = 0;
        gEvent.flags |= 0x10;
        break;
    case 5:
        gEvent.flags |= 0x10;
        race_reset_grid_order();
        break;
    case 7:
        gEvent.unk_48 = -1;
        gEvent.flags |= 0x10;
        race_reset_grid_order();
        for (b = gRiders_55; b < &gRiders_55[gGame.numPlayers]; b++) {
            b->f69C = 0x1C20;
        }
        break;
    case 9:
        tutorial_init_lesson(1);
        break;
    case 10:
        gEvent.time = 0x1C20;
        gEvent.flags |= 0x10;
        race_reset_grid_order();
        break;
    case 0xD:
    case 0xE:
        break;
    default:
        if (gEvent.state == 8) {
            race_set_grid_order(2, 0);
        }
        gEvent.numFinished = 0;
        for (j = 0; j < gEvent.numRiders; j++) {
            D_800E3E20[j].f8 = -1;
        }
        break;
    }
}

s32 event_check_record_time(s32 *p)
{
    u8 ret = 0;
    s32 i;
    s32 j;
    s32 *ch;
    u8 *src;

    if (((u8 *)p)[0x6E8] != 0) goto done;
    if (gReplayPlayback != 0) goto done;
    if (gGame.mode == 1 && gSeason_4 != 0 && ((*gSeason_4 - (s32)D_800CA9C8) * 0x38E38E39 >> 2) == 5) goto done;
    ch = gCurTrack_3;
    if (ch[0x14] & 2) goto done;
    if (gEvent.time >= gEvent.record.times[2]) goto done;
    if (gGame.mode == 8 && p != gRiders_56 + D_800C8CE0 * 0x1f8) goto done;
    if (gGame.numLaps != *(s16 *)((u8 *)ch + 0x1E)) goto done;

    ret = gEvent.time < gEvent.record.times[0];
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
    gEvent.record.initials[i][2] = (gGame.mode == 8) ? 0 : (u8)(((u32)((s32)p - (s32)gRiders_56) * 0xBEFBEFBFu) >> 5);
    gCurTrack_3[0x14] |= 0x100;
    if (ret) announce_record((((s32)p - (s32)gRiders_56) * -0x41041041) >> 5);
done:
    return ret;
}

u32 event_rider_finished(s32 p0)
{
    s32 p = p0;
    u8 *s0;
    u8 *s2;
    Event *v;
    u8 *q;
    u8 *tb;
    s32 a0;
    s32 a1;
    s32 rv;
    s32 a2;
    s32 ti;
    u32 t1;
    u16 hv;
    f32 fv;
    u8 *w;
    u8 *w2;

    a1 = ((p << 6) - p) << 5;
    s0 = D_800E3E20_2 + a1;
    rv = *(s16 *)(s0 + 8);
    if (rv >= 0) {
        return rv;
    }
    v = &gEvent;
    v->finishOrder[v->numFinished] = p;
    a0 = (u16)v->numFinished;
    v->numFinished = a0 + 1;
    *(u16 *)(s0 + 8) = a0;
    *(s32 *)(s0 + 0x2C) = v->time;
    if (*(u8 *)(D_800E3E20_2 + a1 + 0x88) == 0 && gReplayPlayback == 0) {
        if ((gGame.mode != 8 || p == D_800C8CE0) && gNumScreens == 1) {
            func_8001106C();
        }
    }
    s2 = gRiders + ((((p << 6) - p)) << 5);
    t1 = event_check_record_time(s2);
    if ((gGame.flags & 0x8000) == 0) {
        a2 = gGame.mode;
        if (a2 == 1) {
            { s32 ix = (p * 7 + *(s16 *)(gSeason + 0x64)) * 2; *(u16 *)(gSeason + ix + 0x10) = *(u16 *)(s0 + 8); }
            ti = *(s16 *)(s0 + 8);
            ti = (s16)ti;
            tb = (u8 *)(gRacePoints + ti * 4);
            {
                u8 *pp = gSeason + p;
                pp[0x66] = tb[3];
            }
            if (s2[0x6E8] != 0) {
                goto L1fc;
            }
            a1 = gEvent.numFinished;
            if (a1 != a2) {
                goto L19c;
            }
            q = *(u8 **)gSeason + (gGame.difficulty - 1);
            q[4] = q[4] | (a1 << *(s16 *)(gSeason + 0x64));
        }
    }
L19c:
    {
        u8 *bse = gRiders;
        w = bse + (((p << 6) - p) << 5);
    }
    if (w[0x6E8] == 0) {
        Event *vb = &gEvent;
        if (vb->numFinished == 1) {
            vb->camTarget = p;
        }
        do { do { do {
        *(u32 *)(s0 + 0x30) = *(u32 *)(s0 + 0x30) | 8;
        fv = *(f32 *)(w + 0x638);
        hv = *(u16 *)(w + 0x628);
        } while (0); } while (0); } while (0);
        w[0x6E8] = 1;
        *(f32 *)(w + 0x42C) = fv;
        *(u16 *)(w + 0x734) = hv;
        *(u16 *)(w + 0x714) = hv;
    }
L1fc:
    {
        u8 *bse2 = gRiders;
        w2 = bse2 + (((p << 6) - p) << 5);
    }
    w2[0x3FF] = 1;
    *(f32 *)(w2 + 0x42C) = *(f32 *)(w2 + 0x638);
    return t1 & 0xff;
}

typedef struct Rec_31 {
    s16 cnt;          /* 0x660 */
    s16 f662;         /* 0x662 */
    u8 pad664[4];
    s16 f668;         /* 0x668 */
    s16 f66A;         /* 0x66A */
    s16 f66C;         /* 0x66C */
    u8 pad66E[2];
    s32 f670;         /* 0x670 */
    s32 times[29];    /* 0x674 */
    u8 f6E8;          /* 0x6E8 */
    u8 pad6E9[4];
    s8 f6ED;          /* 0x6ED */
    u8 pad6EE[0x180 - 0x8E];
} Rec_31;

typedef struct Bike_31 {
    u8 pad000[0x660];
    Rec_31 r;
} Bike_31;

typedef struct G64 {
    u8 pad[0x50];
    u32 f50;
} G64;

extern G64 *gCurTrack_29;
extern Bike_31 gRiders_57[];

void event_rider_lap_completed(s32 p)
{
    s32 st = 0;
    Rec_31 *b;
    s32 t;
    s32 i;
    s16 v;

    switch (gGame.mode) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 9:
    case 13:
    case 14:
        return;
    }
    b = &gRiders_57[p].r;
    if (b->cnt < gEvent.numLaps) {
        if (gRiders_57[p].r.f6E8 == 0 && gReplayPlayback == 0 && gGame.mode != 7 && gGame.mode != 5 &&
            !(gCurTrack_29->f50 & 2) && (t = b->times[b->cnt]) < gEvent.record.bestLap &&
            (gGame.mode != 8 || p == D_800C8CE0)) {
            gEvent.record.bestLap = t;
            gEvent.record.initials[3][0] = 0xFF;
            gEvent.record.initials[3][1] = 0xFF;
            gEvent.record.initials[3][2] = (gGame.mode == 8) ? 0 : p;
            st = 3;
            gCurTrack_29->f50 |= 0x80;
        }
        if (gGame.flags & 0x81) {
            goto end;
        }
        v = gEvent.lapLeaderTimes[b->cnt];
        if (v == -1) {
            gEvent.lapLeaderTimes[b->cnt] = ((u16 *)&gEvent.time)[1];
        } else {
            b->f670 = v - gEvent.time;
            b->f66C = 0x78;
            if (b->f662 == 1) {
                for (i = 0; i < gGame.numPlayers; i++) {
                    if (b->cnt < gRiders_57[i].r.cnt + gRiders_57[i].r.f6ED) {
                        gRiders_57[i].r.f66C = 0x78;
                        gRiders_57[i].r.f670 = -b->f670;
                        break;
                    }
                }
            }
        }
        b->f66A = 0x78;
        ((volatile Rec_31 *)b)->cnt++;
        if (b->cnt != gEvent.numLaps) {
            if (st != 3) {
                if (gGame.mode == 8 && gRiderAlpha[p] == 0xFF) {
                    st = 1;
                } else if (b->f662 == 0) {
                    st = 2;
                } else if (gRiders_57[p].r.f6E8 == 0 && b->f670 < -300) {
                    st = 1;
                }
            }
        } else if ((u8)event_rider_finished(p)) {
            if (st != 3) {
                if (b->f668 == 0 && gEvent.numRiders >= 2 && st == 0) {
                    st = 4;
                } else if (gRiders_57[p].r.f6E8 == 0) {
                    st = 5;
                }
            }
        } else if (p < gGame.numPlayers) {
            st = 6;
        }
    }
end:
    if (gGhostRace != 0 && gGhostSlot == p) {
        st = 0;
    }
    switch (st) {
    case 1:
        if (gGhostRace == 0 || p != gGhostSlot) {
            announce_lap(b->cnt - 1);
        }
        break;
    case 2:
        announce_leader_lap(p, b->cnt - 1);
        break;
    case 3:
        announce_best_lap(p);
        break;
    case 4:
        announce_winner(p);
        break;
    case 5:
        announce_finish(p);
        break;
    case 6:
        break;
    }
}

void event_update_countdown(void)
{
    if (gEvent.countdown != 0 && --gEvent.countdown == 0) {
        switch (gEvent.state) {
        case 0:
            gEvent.countdown = 150;
            gEvent.state = 1;
            break;
        case 1:
            gEvent.countdown = 36;
            gEvent.state = 2;
            tunes_play_speech(0x6F, 5, 1);
            break;
        case 2:
            gEvent.countdown = 36;
            gEvent.state = 3;
            tunes_play_speech(0x6D, 6, 1);
            break;
        case 3:
            gEvent.countdown = 36;
            gEvent.state = 4;
            tunes_play_speech(0x6E, 7, 1);
            break;
        case 4:
            gEvent.countdown = 15;
            gEvent.state = 5;
            gEvent.flags &= ~0x10;
            tunes_play_speech(0x70, 8, 1);
            break;
        }
    }
}

typedef struct {
    u8 pad0[0x3FF];
    u8 done;
    u8 pad400[0x668 - 0x400];
    s16 f668;
    s16 f66A;
    u8 pad66C[0x69C - 0x66C];
    s32 f69C;
    u8 pad6A0[0x6E8 - 0x6A0];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Rider_3;

extern Rider_3 gRiders_58[];

void event_update_hillclimb(void)
{
    Event *g;
    Rider_3 *r;
    u8 flag;

    event_update_countdown();
    g = &gEvent;
    if (!(g->flags & 0x10) && g->allFinished == 0) {
        g->time++;
    }
    flag = 1;
    for (r = gRiders_58; r < &gRiders_58[gEvent.numRiders]; r++) {
        if (r->f66A != 0) {
            r->f66A--;
            r->f69C += 30;
        }
        if (gEvent.time == r->f69C && r->done == 0) {
            r->done = 1;
            r->f668 = 5;
            r->f6E8 = 1;
        }
        if (r->done == 0) flag = 0;
    }
    gEvent.allFinished = flag;
    if (flag) {
        if (((gPlayerInputs_2 & 0x40) && !(gPlayerInputsPrev & 0x40)) ||
            ((gPlayerInputs_2 & 0x10) && !(gPlayerInputsPrev & 0x10))) {
            event_request_exit();
        }
    }
}

typedef struct {
    u8 pad0[0x3C8];
    s32 key;
    u8 pad3CC[0x3FF - 0x3CC];
    u8 f3FF;
    u8 pad400[0x668 - 0x400];
    s16 f668;
    u8 pad66A[0x6E8 - 0x66A];
    u8 finished;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_12;

typedef struct {
    u8 pad[0x50];
    s32 flags;
} Hud;

extern Hud *gCurTrack_30;
extern Player_12 gRiders_59[];

void event_update_stunt(void)
{
    Player_12 *e;
    s32 i, j, k;

    event_update_countdown();
    if (gEvent.flags & 0x10) return;
    if (--gEvent.time > 0) return;
    if (((gPlayerInputs_2 & 0x40) && !(gPlayerInputsPrev & 0x40)) ||
        ((gPlayerInputs_2 & 0x10) && !(gPlayerInputsPrev & 0x10))) {
        if (gRacePhase == 5) {
            gRacePhase = 6;
            gRacePhaseTimer = 0x14;
        }
    }
    gEvent.time = 0;
    if (gEvent.allFinished) return;
    for (i = 0; i < gGame.numPlayers; i++) {
        e = &gRiders_59[i];
        if (e->finished) continue;
        if (gReplayPlayback) continue;
        if (gGame.mode == 1 && gSeason_10 != NULL && *gSeason_10 - D_800CA9C8 == 5) continue;
        if (e->key > gEvent.record.times[2]) {
            for (j = 0; j < 3; j++) {
                if (e->key >= gEvent.record.times[j]) {
                    for (k = 1; k >= j; k--) {
                        gEvent.record.times[k + 1] = gEvent.record.times[k];
                        gEvent.record.initials[k + 1][0] = gEvent.record.initials[k][0];
                        gEvent.record.initials[k + 1][1] = gEvent.record.initials[k][1];
                        gEvent.record.initials[k + 1][2] = gEvent.record.initials[k][2];
                    }
                    break;
                }
            }
            gEvent.record.times[j] = e->key;
            gEvent.record.initials[j][2] = e - gRiders_59;
            gEvent.record.initials[j][0] = 0xFF;
            gEvent.record.initials[j][1] = 0xFF;
            gCurTrack_30->flags |= 0x100;
        }
    }
    gEvent.allFinished = 1;
    for (j = 0; j < gGame.numPlayers; j++) {
        gRiders_59[j].finished = 1;
    }
    gRiders_59[0].f3FF = 1;
    *(volatile s16 *)&gRiders_59[0].f668 = 1;
    if (gGame.numPlayers == 2) {
        if (gRiders_59[0].key > gRiders_59[1].key) {
            gRiders_59[0].f668 = 0;
            gRiders_59[1].f668 = 5;
        } else if (gRiders_59[0].key == gRiders_59[1].key) {
            gRiders_59[0].f668 = 1;
            gRiders_59[1].f668 = 1;
        } else {
            gRiders_59[0].f668 = 5;
            gRiders_59[1].f668 = 0;
        }
        D_800E439F = 1;
    }
}

#define PRESSED(i, b) ((gPlayerInputs[i] & (b)) && !(gPlayerInputsPrev_3[i] & (b)))

typedef struct {
    u8 b[0x7E0];
} Ent_55;

extern Ent_55 gRiders_60[];

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
        if (--gEvent.goalTimer == 0) {
            for (i = 0; i < gGame.numPlayers; i++) {
                bike_respawn(&gRiders_60[i]);
            }
            soccer_ball_reset(0);
            gEvent.flags &= ~0x1F;
        }
        return;
    }
    if (gEvent.state == 7 || gEvent.state == 9 || gEvent.state == 11) {
        if (!(gEvent.flags & 0xF)) {
            if (gSoccerBall_2[0] >= -24.0f && gSoccerBall_2[0] < 23.0f && gSoccerBall_2[2] >= 79.0f && gSoccerBall_2[2] < 87.0f &&
                gSoccerBall_2[1] >= 0.0f && gSoccerBall_2[1] < 6.2f) {
                gEvent.goalTimer = 180;
                gEvent.goals[0]++;
                announce_goal();
                D_800E3F68 = 1;
                gEvent.flags |= 0x11;
                if ((gEvent.ballToucher >> ((gGame.numPlayers - 1) >> 1)) == 1) {
                    gEvent.flags |= 0x20;
                } else {
                    gEvent.flags &= ~0x20;
                }
            } else if (gSoccerBall_2[0] >= -24.0f && gSoccerBall_2[0] < 23.0f && gSoccerBall_2[2] >= -80.0f && gSoccerBall_2[2] < -73.0f &&
                       gSoccerBall_2[1] >= 0.0f && gSoccerBall_2[1] < 6.2f) {
                gEvent.goalTimer = 180;
                gEvent.goals[1]++;
                announce_goal();
                D_800E4748 = 1;
                gEvent.flags |= 0x12;
                if ((gEvent.ballToucher >> ((gGame.numPlayers - 1) >> 1)) == 0) {
                    gEvent.flags |= 0x20;
                } else {
                    gEvent.flags &= ~0x20;
                }
            }
        }
    }
    switch (gEvent.state) {
    case 6:
        if (--gEvent.countdown == 0) {
            gEvent.state = 7;
            gEvent.time = 0x2A30;
            gEvent.flags &= ~0x10;
        }
        break;
    case 7:
        if (--gEvent.time == 0) {
            j = 0;
            gEvent.state = 8;
            gEvent.countdown = 240;
            gEvent.flags |= 0x10;
            for (; j < gGame.numPlayers; j++) {
                bike_respawn(&gRiders_60[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 8:
        if (--gEvent.countdown == 0) {
            gEvent.state = 9;
            gEvent.time = 0x2A30;
            gEvent.flags &= ~0x10;
        }
        break;
    case 9:
        if (--gEvent.time == 0) {
            gEvent.flags |= 0x10;
            if (gEvent.goals[0] == gEvent.goals[1]) {
                gEvent.state = 10;
                gEvent.countdown = 240;
            } else {
                gEvent.state = 12;
                gEvent.countdown = 600;
                gEvent.allFinished = 1;
            }
            for (i = 0; i < gGame.numPlayers; i++) {
                bike_respawn(&gRiders_60[i]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 10:
        if (--gEvent.countdown == 0) {
            gEvent.state = 11;
            gEvent.time = 0x1C20;
            gEvent.flags &= ~0x10;
        }
        break;
    case 11:
        if (--gEvent.time == 0) {
            j = 0;
            gEvent.state = 12;
            gEvent.allFinished = 1;
            gEvent.countdown = 600;
            gEvent.flags |= 0x10;
            for (; j < gGame.numPlayers; j++) {
                bike_respawn(&gRiders_60[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 12:
        if (PRESSED(0, 0x40) || PRESSED(1, 0x40) || PRESSED(2, 0x40) || PRESSED(3, 0x40) ||
            (gEvent.messageTimer == 0 &&
             (PRESSED(0, 0x10) || PRESSED(1, 0x10) || PRESSED(2, 0x10) || PRESSED(3, 0x10)))) {
            if (gRacePhase == 5) {
                gRacePhase = 6;
                gRacePhaseTimer = 20;
            }
        }
        break;
    }
}

typedef struct {
    s16 f0;
    s16 pad2[3];
    s16 f8;
    s16 padA[5];
    s32 f14[6];
    s32 f2C;
} Sub_10;

typedef struct {
    u8 pad0[0x630];
    f32 f630;
    u8 pad634[0x660 - 0x634];
    Sub_10 sub;
    s32 f690;
    u8 pad694[0x6E8 - 0x694];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_13;

typedef struct {
    u8 pad0[4];
    s16 f4[6];
    s16 f10[42];
    s16 f64;
    u8 f66[5];
    u8 f6B;
    u8 f6C[8];
} Race_4;

extern Player_13 gRiders_61[];
extern Player_13 *gRiderStandings_3[];
extern Race_4 *gSeason_11;

void event_update_race(void) {
    s32 i, j, k, m;
    s32 cnt, n1;
    s32 pad[8];
    u8 buf[4];
    f32 f;
    s32 s, t;
    u8 tmp;
    Player_13 *p;
    Sub_10 *q;

    event_update_countdown();
    n1 = 0;
    cnt = n1;
    if (!(gEvent.flags & 0x10)) {
        gEvent.time++;
    }
    *(s32 *)buf = 0;
    for (m = 0; m < gEvent.numRiders; m++) {
        q = &gRiders_61[m].sub;
        if ((gRiders_61[m].f690 & 8) || (gGhostRace != 0 && gGhostSlot == m) || gRiders_61[m].f6E8 == 0) {
            cnt++;
        }
        if (gRiders_61[m].sub.f8 == -1 && m < cnt) {
            n1++;
        }
        if (q->f8 != -1 || gRiders_61[m].f6E8 != 0) {
            buf[m] = 1;
        }
        if (gEvent.state == 5) {
            s32 k = q->f0;
            if (k < gEvent.numLaps) {
                q->f14[k]++;
            }
        }
    }
    for (m = 0; m < cnt; m++) {
        if (!buf[m]) break;
    }
    if (!((m == cnt && n1 == 0 && gGame.mode != 8) || (gGame.mode == 8 && buf[D_800C8CE0]))) return;
    if (gGame.flags & 0x81) return;
    if (gRaceExitCode == 3) return;
    if (gEvent.allFinished == 0) {
        gEvent.messageTimer = 180;
    } else if (gEvent.messageTimer != 0) {
        gEvent.messageTimer--;
    }
    gEvent.allFinished = 1;
    gRaceExitCode = 2;
    if (sRaceEndFrame == -1) {
        sRaceEndFrame = gRaceFrame;
    }
    if (gReplayPlayback == 0 && gRacePhase == 5) {
        if ((gPadInput_2[0] & 0x40) && !(gPadInputPrev_2[0] & 0x40)) goto go;
        if ((gPadInput_2[1] & 0x40) && !(gPadInputPrev_2[1] & 0x40)) goto go;
        if ((gPadInput_2[2] & 0x40) && !(gPadInputPrev_2[2] & 0x40)) goto go;
        if ((gPadInput_2[3] & 0x40) && !(gPadInputPrev_2[3] & 0x40)) goto go;
        if (gCreditsMode != 0 && (gGame.flags & 0x01000000) && (gPadInput_2[0] & 0x20) && !(gPadInputPrev_2[0] & 0x20)) goto go;
        if (gEvent.messageTimer != 0 && gCreditsMode == 0) goto end;
        if ((gPadInput_2[0] & 0x10) && !(gPadInputPrev_2[0] & 0x10)) goto go;
        if ((gPadInput_2[1] & 0x10) && !(gPadInputPrev_2[1] & 0x10)) goto go;
        if ((gPadInput_2[2] & 0x10) && !(gPadInputPrev_2[2] & 0x10)) goto go;
        if (!(gPadInput_2[3] & 0x10) || (gPadInputPrev_2[3] & 0x10)) goto end;
    go:
        if (gCreditsMode != 0) {
            snd_play_menu_sound(2);
        }
        if (gRacePhase == 5) {
            gRacePhase = 6;
            gRacePhaseTimer = 20;
        }
        for (k = 0; k < gEvent.numRiders; k++) {
            if (gRiders_61[k].sub.f8 == -1) {
                f = gRiders_61[k].f630 / (gPathLapLength * gEvent.numLaps);
                if (f <= 0.1f) {
                    f = 0.1f;
                } else if (f > 1.0f) {
                    f = 1.0f;
                }
                f = gEvent.time / f;
                gRiders_61[k].sub.f2C = f;
            }
        }
        for (k = 0; k < gEvent.numRiders; k++) {
            if (gRiders_61[k].sub.f8 == -1) {
                gRiders_61[k].sub.f8 = 0;
                for (j = 0; j < gEvent.numRiders; j++) {
                    if ((j != k && k < j && gRiders_61[j].sub.f2C < gRiders_61[k].sub.f2C) ||
                        (j < k && gRiders_61[j].sub.f2C <= gRiders_61[k].sub.f2C)) {
                        gRiders_61[k].sub.f8++;
                    }
                }
                if (gGame.mode == 1) {
                    gSeason_11->f66[k] += gRacePoints_2[gRiders_61[k].sub.f8].b3;
                }
            }
            gEvent.finishOrder[gRiders_61[k].sub.f8] = k;
        }
        gEvent.numFinished = gEvent.numRiders;
        if (gGame.mode == 1) {
            for (i = 0; i < gEvent.numRiders; i++) {
                gSeason_11->f4[i] += gSeason_11->f66[i];
                gSeason_11->f6C[i] = i;
            }
            for (i = 0; i < gEvent.numRiders; i++) {
                j = i;
                if (i != 0) {
                    s = gSeason_11->f4[gSeason_11->f6C[i - 1]];
                }
                t = gSeason_11->f4[gSeason_11->f6C[i]];
                while (j != 0 && s < t) {
                    tmp = gSeason_11->f6C[j];
                    gSeason_11->f6C[j] = gSeason_11->f6C[j - 1];
                    gSeason_11->f6C[j - 1] = tmp;
                    j--;
                    if (j != 0) {
                        s = gSeason_11->f4[gSeason_11->f6C[j - 1]];
                    }
                }
            }
        }
    }
end:
    if (gCurGameState->mode == 3) {
        s = -1;
        for (i = gEvent.numFinished; i < gEvent.numRiders; i++) {
            q = &gRiderStandings_3[i]->sub;
            if (q->f8 == s) {
                gEvent.finishOrder[gEvent.numFinished] = i;
                q->f8 = gEvent.numFinished++;
                q->f2C = (gEvent.time * gRiderStandings_3[0]->f630) / gRiders_61[i].f630;
                if (gGame.mode == 1 + (((u32)&gRiders_61 & 1) >> 1)) {
                    gSeason_11->f10[(gRiderStandings_3[i] - gRiders_61) * 7 + gSeason_11->f64] = q->f8;
                    gSeason_11->f66[gRiderStandings_3[i] - gRiders_61] = ((Pts *)((u32)gRacePoints_2 | (((u32)gRacePoints_2 & 1) >> 1)))[q->f8].b3;
                }
            }
        }
    }
}

void event_update(s32 arg0)
{
    s32 i;
    s32 *p;

    if ((gGame.flags & 0x80) == 0) {
        s32 bits;
        i = 0;
        bits = gContMask;
        for (p = gPlayerInputs; i < 4; i++, p++) {
            if (((bits >> i) & 1) != 0 && *p != 0) break;
        }
        if (i == 4) {
            gEvent.idleFrames = gEvent.idleFrames + 1;
        } else {
            gEvent.idleFrames = 0;
        }
    }
    switch (gGame.mode) {
    case 2:
        event_update_soccer();
        break;
    case 5:
        event_update_countdown();
        if (gEvent.allFinished == 0) {
            if ((gEvent.flags & 0x10) != 0) break;
            gEvent.time = gEvent.time + 1;
            if (gEvent.time == 0x34bc0) {
                event_rider_finished(0);
                gEvent.allFinished = 1;
            }
        } else if ((gPlayerInputs[0] & 0x40) != 0) {
            if ((gPlayerInputsPrev & 0x40) == 0) {
                event_request_exit();
            }
            break;
        }
        break;
    case 7:
        event_update_hillclimb();
        break;
    case 9:
        tutorial_update();
        break;
    case 10:
        event_update_stunt();
        break;
    default:
        event_update_race();
    case 0xd:
    case 0xe:
        break;
    }
    if (gGame.mode < 2 && gEvent.numFinished > 0 && gEvent.finishOrder[0] < gGame.numPlayers &&
        gEvent.finishTimer != -2 && (gGame.numPlayers == 1 || gGame.mode == 8)) {
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

void event_hud_init(void)
{
    s32 iVar2;
    s32 iVar3;
    u16 *puVar4;
    s32 auStack_50[16];
    s32 i;
    s32 t13;
    Game *s2;

    if (gGame.mode == 9) {
        sGreyPalette = (u16 *)mem_alloc_dbg(0x200, 0, D_800D7340, 0xB4B);
        *sGreyPalette = 0;
        puVar4 = sGreyPalette + 1;
        iVar3 = 1;
        do {
            iVar2 = iVar3 >> 4;
            *puVar4 = (u16)(iVar2 << 1) | (u16)((iVar2 << 6) | 1) | (u16)(iVar2 << 11);
            puVar4 = puVar4 + 1;
            iVar3 = iVar3 + 1;
        } while (iVar3 < 0x100);
    }
    sCountdownTextures = res_load(D_800E7728, D_800D0D00);
    s2 = &gGame;
    if (1 < s2->numPlayers) {
        sTempGaugeTextures = res_load(D_800E7728, D_800D11B0);
    }
    if (s2->mode == 9) {
        s2->font = (struct Font *)res_load(D_800E7728, D_800D09F8);
        D_800F0740 = res_load(D_800E7728, D_800D10E8);
        D_800EFD6C = res_load(D_800E7728, D_800D0DEC);
        D_800EFD68 = res_load(D_800E7728, D_800D09D4);
    }
    res_register_type(D_800D8424, prim_n64_relocate);
    if (s2->numPlayers == 1) {
        D_800F0730[0] = mem_alloc_dbg(0x1c0, 0, D_800D7340, 0xB6B);
        D_800F0730[1] = mem_alloc_dbg(0x1c0, 0, D_800D7340, 0xB6C);
    }
    t13 = *(volatile s32 *)&gScreenHeight;
    guPerspective(auStack_50, sHudPerspNorm, 33.0f, 320.0f / (f32)t13, 50.0f, 4192.0f, 1.0f);
    iVar3 = 0;
    if (s2->mode == 2) {
        sSoccerMapTexture = res_load(D_800E7728, D_800D1084);
    }
    for (i = 0; i < 0x1E; i++) {
        gHudTextObjs[i] = font_text_create(0, 0x40, 2, 2, 0);
        gMenuTextObjs[i] = font_text_create(0, 0x40, 2, 2, 0);
    }
}

typedef struct { char c[9]; } Str9;
typedef struct { char c[2]; } Str2;

extern Str9 D_800D8438;

void event_format_time(s32 param_1, char *param_2)
{
    char buf[32];

    if (param_1 > 0x9E340) {
        *(Str9 *)param_2 = D_800D8438;
        return;
    }
    int_to_str(param_1 / 0xE10, buf);
    if (strlen(buf) == 1) {
        *(Str2 *)param_2 = *(Str2 *)D_800D8444;
        str_cat(param_2, buf);
    } else {
        str_copy(param_2, buf);
    }
    str_cat(param_2, D_800D8448);
    int_to_str((param_1 / 60) % 60, buf);
    if (strlen(buf) == 1) {
        str_cat(param_2, D_800D8444);
        str_cat(param_2, buf);
    } else {
        str_cat(param_2, buf);
    }
    str_cat(param_2, D_800D844C);
    int_to_str(((param_1 % 60) * 100) / 60, buf);
    if (strlen(buf) == 1) {
        str_cat(param_2, D_800D8444);
        str_cat(param_2, buf);
    } else {
        str_cat(param_2, buf);
    }
}

void event_format_time_short(s32 param_1, char *param_2)
{
    s32 iVar1;
    char acStack_30[32];

    *param_2 = 0;
    if (param_1 > 0xe0f) {
        int_to_str(param_1 / 0xe10, param_2);
        str_cat(param_2, D_800D8448);
    }
    int_to_str((param_1 / 0x3c) % 0x3c, acStack_30);
    iVar1 = strlen(acStack_30);
    if ((iVar1 == 1) && (param_1 > 0xe0f)) {
        str_cat(param_2, D_800D8444);
    }
    str_cat(param_2, acStack_30);
    str_cat(param_2, D_800D844C);
    int_to_str(((param_1 % 0x3c) * 100) / 0x3c, acStack_30);
    iVar1 = strlen(acStack_30);
    if (iVar1 == 1) {
        str_cat(param_2, D_800D8444);
    }
    str_cat(param_2, acStack_30);
    return;
}

void event_set_place_text(u8 *p, s32 n, s32 arg2)
{
    char buf[0x28];
    s32 m;
    u8 *base;
    s32 off;
    Quad *r;

    if (gEvent.numRiders >= 2) {
        buf[0] = n + 0x31;
        m = n;
        if (m > 3) {
            m = 3;
        }
        buf[1] = m + 0x7B;
        strcpy(buf + 2, " Place");
        font_text_add(p, buf);
    } else {
        font_text_add(p, "Finished");
    }
    base = gViewRects;
    off = gCurView * 16 + (gNumScreens - 1) * 64;
    r = (Quad *)(base + off);
    font_draw_text(p, r->a + r->c / 2, r->b + r->d / 2);
}

Gfx *event_draw_hud_testride(Gfx *gdl, s32 p1, s32 p2, s32 p3)
{
    char buf[32];
    s32 x;
    s32 y;
    f32 fa;
    s32 *tab;

    font_reset_queue();
    if (gFrameCounter & 0x20) {
        font_text_create(gHudTextObjs_2, 0x40, 1, 2, 0x10);
        font_text_add(gHudTextObjs_2, "Testing User Track");
        tab = gViewRects_4;
        x = gScreenWidth * 0x12;
        if (x < 0) {
            x = x + 0x1ff;
        }
        font_draw_text(gHudTextObjs_2, gViewRects_4[0] + (x >> 9), tab[1] + (gScreenHeight * 0x11) / 0xf0);
    }
    fa = gRiders_62[0x108 + p3 * 0x1f8];
    if (0.0f < fa) {
        int_to_str((s32)(fa * 3600.0f / 1612.9f), buf);
    } else {
        int_to_str((s32)(-fa * 3600.0f / 1612.9f), buf);
    }
    font_text_create(gHudTextObjs_2, 0x40, 3, 2, 0x12);
    font_text_add(gHudTextObjs_2, buf);
    x = 0x1ba;
    if (gViMode == 2) {
        x = 0x228;
    }
    y = 0xd6;
    if (gViMode == 2) {
        y = 0x180;
    }
    font_draw_text(gHudTextObjs_2, x, y);
    gdl = font_render(gdl);
    gdl = render_draw_temp_gauge(gdl, p1, p2, p3);
    return gdl;
}

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x3FF - 0x54];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x66A - 0x424];
    s16 h66A;
    u8 pad66C[0x68C - 0x66C];
    s32 w68C;
    u8 pad690[0x69C - 0x690];
    s32 w69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Player_14;

extern Player_14 gRiders_63[];

Gfx *event_draw_hud_hillclimb(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[32];
    f32 v;
    s32 x1, y1;
    s32 t;
    s32 *p;

    font_reset_queue();
    font_text_create(gHudTextObjs[0], 0x40, 1, 2, 0x20);
    font_text_create(gHudTextObjs[1], 0x40, 1, 2, 0x20);
    font_text_create(gHudTextObjs[2], 0x40, 1, 4, 0x20);
    font_text_create(gHudTextObjs[3], 0x40, 1, 4, 0x20);
    font_text_create(gHudTextObjs[7], 0x40, 1, 6, 0x20);
    font_text_create(gHudTextObjs[8], 0x40, 1, 6, 0x20);
    if (gViMode == 2) {
        font_set_scale(0x10000, 0x20000, 0, 0);
    }
    if (gNumViews == 1) {
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, 0x12);
        v = gRiders_63[pl].f420;
        int_to_str((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        x1 = 0x1BA;
        if (gViMode == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (gViMode == 2) {
            y1 = 0x180;
        }
        font_draw_text(gHudTextObjs[6], x1, y1);
        gdl = render_draw_temp_gauge(gdl, a1, a2, pl);
        font_text_create(gHudTextObjs[4], 0x40, 1, 2, 0x20);
        font_text_create(gHudTextObjs[5], 0x40, 1, 4, 0x20);
        event_format_time(gRiders_63[pl].w69C - gEvent.time, buf);
        font_text_add(gHudTextObjs[4], "Time:");
        font_draw_text(gHudTextObjs[4], 0x23, gViMode == 2 ? 0x180 : 0xD6);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], 100, gViMode == 2 ? 0x180 : 0xD6);
    } else {
        font_text_create(gHudTextObjs[4], 0x40, 2, 2, 0x20);
        font_text_create(gHudTextObjs[5], 0x40, 2, 4, 0x20);
        t = -gEvent.time + gRiders_63[pl].w69C;
        if (gRiders_63[pl].b3FF) {
            t = gRiders_63[pl].w68C;
        }
        if (t < 0) {
            t = 0;
        }
        event_format_time(t, buf);
        font_text_add(gHudTextObjs[4], "Time:");
        font_draw_text(gHudTextObjs[4], gViewRects_5[gNumScreens - 1][pl][0] + 10, 0x1E);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], gViewRects_5[gNumScreens - 1][pl][0] + 0x32, 0x1E);
        font_text_create(gHudTextObjs[6], 0x40, 3, 2, 0x12);
        v = gRiders_63[pl].f420;
        int_to_str((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        font_draw_text(gHudTextObjs[6],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] - 0x2D,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] - 0xB);
        gdl = event_draw_temp_gauge(gdl, a1, a2, pl);
    }
    if (gEvent.countdown != 0 && gEvent.state != 0 && (gEvent.state != 1 || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, pl);
    }
    if (gRiders_63[pl].h66A != 0 && gPauseMenu == 0) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, 0x21);
        font_text_add(gHudTextObjs[7], "Stage Clear!");
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] / 2,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] / 2);
    }
    if (gRiders_63[pl].b3FF) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, 0x21);
        if (gEvent.allFinished == 0 || pl != 0) {
            font_text_add(gHudTextObjs[7], "Game Over");
        } else {
            font_text_add(gHudTextObjs[7], D_800D84A8);
        }
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] / 2,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] / 2);
    }
    if (gGame.flags & 0x20000000) {
        s32 *p1;
        s32 *p2;
        s32 *p3;
        p1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, 0x10);
        debug_ftoa(buf, gRiders_63[0].f48, 5);
        font_text_add(p1, buf);
        font_draw_text(p1, 0x28, 0x1E);
        p2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
        debug_ftoa(buf, gRiders_63[0].f50, 5);
        font_text_add(p2, buf);
        font_draw_text(p2, 0x28, 0x28);
        p3 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
        int_to_str(gDebugFreeMem, buf);
        font_text_add(p3, buf);
        font_draw_text(p3, 0x28, 0x32);
    }
    return font_render(gdl);
}

typedef struct {
    u8 pad0[0x18];
    u32 w18;
} Txt;

typedef struct {
    u8 pad0[0x18];
    char *name;
    u8 pad1C[0x3C - 0x1C];
} Trick;

typedef struct {
    u8 pad0[0x3C4];
    s32 w3C4;
    s32 w3C8;
    u8 pad3CC[4];
    u8 b3D0;
    u8 pad3D1;
    s8 s3D2;
    u8 pad3D3[0x3EA - 0x3D3];
    u8 b3EA;
    s8 s3EB[0x3FB - 0x3EB];
    u8 b3FB;
    u8 pad3FC[3];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x668 - 0x424];
    s16 h668;
    u8 pad66A[0x7E0 - 0x66A];
} Player_15;

extern Txt *gHudTextObjs_3[];
extern Player_15 gRiders_64[];
extern Trick gRiderAnims_11[];

Gfx *event_draw_hud_stunt(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    Player_15 *p = &gRiders_64[pl];
    char buf[40];
    s32 idx, t;
    s32 x, y, ty, yt, yy;
    s32 x1, y1, xa;
    s32 x3, y3, ax, ay;
    s32 k, i;
    s32 by, dy;
    u8 c;
    s8 d;
    f32 v;

    font_reset_queue();
    if (gGame.numPlayers >= 2) {
        if (gEvent.allFinished != 0) {
            font_text_create(gHudTextObjs_3[7], 0x40, 1, 4, 0x21);
            switch (p->h668) {
            case 0:
                font_text_add(gHudTextObjs_3[7], "Winner");
                break;
            case 1:
                font_text_add(gHudTextObjs_3[7], D_800D84E0);
                break;
            default:
                font_text_add(gHudTextObjs_3[7], D_800D84E4);
                break;
            }
            font_draw_text(gHudTextObjs_3[7],
                          gViewRects[gNumScreens - 1][pl].x + gViewRects[gNumScreens - 1][pl].w / 2,
                          gViewRects[gNumScreens - 1][pl].y + gViewRects[gNumScreens - 1][pl].h / 2);
        }
        yt = gViewRects[gNumScreens - 1][pl].y + gViewRects[gNumScreens - 1][pl].h - 11;
        font_text_create(gHudTextObjs_3[0], 0x40, 2, 2, 0x20);
        font_text_create(gHudTextObjs_3[1], 0x40, 2, 4, 0x20);
        idx = -1;
        do {
        } while (0);
        event_format_time(gEvent.time, buf);
        font_text_add(gHudTextObjs_3[0], "Time:");
        font_draw_text(gHudTextObjs_3[0], 0x3C, yt);
        font_text_add(gHudTextObjs_3[1], buf);
        font_draw_text(gHudTextObjs_3[1], 100, yt);
        xa = gViewRects[gNumScreens - 1][pl].x + gViewRects[gNumScreens - 1][pl].w - 20;
        yy = gViewRects[gNumScreens - 1][pl].y;
        font_text_create(gHudTextObjs_3[4], 0x40, 2, 2, 0x22);
        font_text_add(gHudTextObjs_3[4], "Total");
        ty = yy + 8;
        font_draw_text(gHudTextObjs_3[4], xa, ty);
        font_text_create(gHudTextObjs_3[5], 0x40, 2, 4, 0x22);
        int_to_str(p->w3C8, buf);
        font_text_add(gHudTextObjs_3[5], buf);
        font_draw_text(gHudTextObjs_3[5], xa, yy + 16);
        c = p->b3D0;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            font_text_create(gHudTextObjs_3[2], 0x40, 2, 2, 0x21);
            x3 = 0x100;
            font_text_add(gHudTextObjs_3[2], gRiderAnims_11[idx].name);
            y3 = ty + 8;
            font_text_create(gHudTextObjs_3[3], 0x40, 2, 4, 0x21);
            int_to_str(p->w3C4, buf);
            font_text_add(gHudTextObjs_3[3], buf);
            ax = 0x100;
            ay = ty;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (xa - (gHudTextObjs_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((gHudTextObjs_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (ty + i * 8) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs_3[2], ax, ay);
            font_draw_text(gHudTextObjs_3[3], x3, y3);
        }
        for (i = 0; i < p->b3EA; i++) {
            font_text_create(gHudTextObjs_3[i + 8], 0x40, 2, 2, 0x20);
            font_text_add(gHudTextObjs_3[i + 8], gRiderAnims_11[p->s3EB[i]].name);
            font_draw_text(gHudTextObjs_3[i + 8], 0x3C, ty + i * 8);
        }
        font_text_create(gHudTextObjs_3[6], 0x40, 3, 2, 0x12);
        v = p->f420;
        int_to_str((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs_3[6], buf);
        font_draw_text(gHudTextObjs_3[6],
                      gViewRects[gNumScreens - 1][pl].x + gViewRects[gNumScreens - 1][pl].w - 0x2D,
                      gViewRects[gNumScreens - 1][pl].y + gViewRects[gNumScreens - 1][pl].h - 0xB);
        gdl = event_draw_temp_gauge(gdl, a1, a2, pl);
    } else {
        if (gViMode == 2) {
            font_set_scale(0x10000, 0x20000, 0, 0);
        }
        font_text_create(gHudTextObjs_3[0], 0x40, 1, 2, 0x10);
        font_text_create(gHudTextObjs_3[1], 0x40, 1, 4, 0x10);
        event_format_time(gEvent.time, buf);
        font_text_add(gHudTextObjs_3[0], "Time:");
        font_draw_text(gHudTextObjs_3[0], 0x23, gViMode == 2 ? 0x180 : 0xD6);
        font_text_add(gHudTextObjs_3[1], buf);
        font_draw_text(gHudTextObjs_3[1], 100, gViMode == 2 ? 0x180 : 0xD6);
        y = 0x19;
        x = gScreenWidth - 62;
        if (gViMode == 2) {
            y = 0x69;
        }
        font_text_create(gHudTextObjs_3[4], 0x40, 1, 2, 0x22);
        font_text_add(gHudTextObjs_3[4], "Total");
        font_draw_text(gHudTextObjs_3[4], x, y);
        if (gViMode == 2) {
            y += 0x19;
        } else {
            y += 0xC;
        }
        font_text_create(gHudTextObjs_3[5], 0x40, 1, 4, 0x22);
        int_to_str(p->w3C8, buf);
        font_text_add(gHudTextObjs_3[5], buf);
        font_draw_text(gHudTextObjs_3[5], x, y);
        if (gViMode == 2) {
            by = 0x69;
            dy = 0x14;
        } else {
            by = 0x5A;
            dy = 0xA;
        }
        c = p->b3D0;
        idx = -1;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->b3FB != 0) {
            d = p->s3D2;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            font_text_create(gHudTextObjs_3[2], 0x40, 1, 2, 0x21);
            font_text_add(gHudTextObjs_3[2], gRiderAnims_11[idx].name);
            font_text_create(gHudTextObjs_3[3], 0x40, 1, 4, 0x21);
            int_to_str(p->w3C4, buf);
            font_text_add(gHudTextObjs_3[3], buf);
            x3 = gScreenWidth / 2;
            y3 = D_800E171C + 0x28;
            ax = gScreenWidth / 2;
            ay = D_800E171C + 0x14;
            if (p->b3FB >= 2) {
                k = p->b3FB * 8;
                x3 = (x3 * k + (x - (gHudTextObjs_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                y3 = (y3 * k + y * (256 - k)) / 256;
                for (i = 0; i < p->b3EA; i++) {
                    if (idx == p->s3EB[i]) {
                        break;
                    }
                }
                if (i == p->b3EA) {
                    ax = (ax * k + ((gHudTextObjs_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (by + i * dy) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs_3[2], ax, ay);
            font_draw_text(gHudTextObjs_3[3], x3, y3);
        }
        v = gRiders_64[pl].f420;
        int_to_str((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        font_text_create(gHudTextObjs_3[6], 0x40, 3, 2, 0x12);
        font_text_add(gHudTextObjs_3[6], buf);
        x1 = 0x1BA;
        if (gViMode == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (gViMode == 2) {
            y1 = 0x180;
        }
        font_draw_text(gHudTextObjs_3[6], x1, y1);
        for (i = 0; i < p->b3EA; i++) {
            font_text_create(gHudTextObjs_3[i + 8], 0x40, 2, 2, 0x20);
            font_text_add(gHudTextObjs_3[i + 8], gRiderAnims_11[p->s3EB[i]].name);
            font_draw_text(gHudTextObjs_3[i + 8], 0x23, by + i * dy);
        }
        if (gEvent.allFinished != 0) {
            font_text_create(gHudTextObjs_3[7], 0x40, 1, 4, 0x21);
            font_text_add(gHudTextObjs_3[7], D_800E3BBF >= 3 ? "Press START to Continue" : "Game Over");
            font_draw_text(gHudTextObjs_3[7],
                          gViewRects[gNumScreens - 1][pl].x + gViewRects[gNumScreens - 1][pl].w / 2,
                          gViewRects[gNumScreens - 1][pl].y + gViewRects[gNumScreens - 1][pl].h / 2);
        }
    }
    gdl = font_render(gdl);
    gdl = render_draw_temp_gauge(gdl, a1, a2, pl);
    if (gEvent.countdown != 0 && gEvent.state != 0 && (gEvent.state != 1 || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, pl);
    }
    return gdl;
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct {
    u8 pad0[0x3FF]; u8 f3FF;
    u8 pad1[0x420 - 0x400]; f32 f420;
    u8 pad2[0x668 - 0x424]; s16 f668;
    u8 pad3[0x7E0 - 0x66A];
} Rec37_3;

extern Rec37_3 gRiders_65[];

Gfx *event_draw_hud_gates(Gfx *gdl, s32 a1, s32 a2, s32 idx)
{
    char buf[32];
    f32 fv;
    s32 y;
    s32 *h;
    s32 n;
    s32 x;
    s32 yy;
    s32 z;
    char *p;

    PKT(0xE7000000, 0);
    PKT(0xE3000C00, 0);
    PKT(0xE3001001, 0);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xE200001C, 0x00404240);
    PKT(0xD9FFFFFE, 0);
    PKT(0xD7000002, 0x80008000);
    gdl = render_draw_temp_gauge(gdl, a1, a2, idx);
    font_reset_queue();
    gdl = event_draw_position(gdl, 0, font_text_create(D_800F0544, 0x20, 3, 2, 0x21));

    fv = gRiders_65[idx].f420;
    int_to_str((s32)(((0.0f < fv) ? fv * 3600.0f : -fv * 3600.0f) / 1612.9f), buf);

    font_text_create(gHudTextObjs[0], 0x40, 3, 2, 0x12);
    font_text_add(gHudTextObjs[0], buf);
    x = 0x1BA;
    if (*(volatile s32 *)&gViMode == 2) x = 0x228;
    yy = 0xD6;
    if (*(volatile s32 *)&gViMode == 2) yy = 0x180;
    font_draw_text(gHudTextObjs[0], x, yy);

    if (gHudLevel <= 0) {
        font_text_create(gHudTextObjs[1], 0x40, 1, 2, 0x20);
        font_text_create(gHudTextObjs[2], 0x40, 1, 4, 0x20);
        event_format_time(gEvent.time, buf);
        y = gViewRects_10[gNumScreens - 1][idx].b;
        font_text_add(gHudTextObjs[1], D_800D8484);
        z = y + 0x14;
        font_draw_text(gHudTextObjs[1], 0x23, z);
        font_text_add(gHudTextObjs[2], buf);
        font_draw_text(gHudTextObjs[2], 100, z);
        font_text_create(gHudTextObjs[3], 0x40, 1, 2, 0x20);
        font_text_create(gHudTextObjs[4], 0x40, 1, 4, 0x20);
        int_to_str(D_800E3BC2 + 1, buf);
        n = strlen(buf);
        int_to_str(10, buf + n + 1);
        p = buf;
        n = strlen(p);
        p += n;
        *p = '/';
        if (gViMode == 2) z = y + 0x2C;
        else z = y + 0x20;
        font_text_add(gHudTextObjs_4[3], D_800D850C);
        font_draw_text(gHudTextObjs_4[3], 0x23, z);
        font_text_add(gHudTextObjs_4[4], buf);
        font_draw_text(gHudTextObjs_4[4], 100, z);
    }

    if (gRiders_65[0].f668 != -1) {
        h = font_text_create(D_800F0548, 0x20, 1, 4, 0x21);
        if (gRiders_65[0].f3FF == 0) {
            font_text_add(h, D_800D84A8);
            font_draw_text(h,
                          gViewRects_10[gNumScreens - 1][idx].a + gViewRects_10[gNumScreens - 1][idx].c / 2,
                          gViewRects_10[gNumScreens - 1][idx].b + gViewRects_10[gNumScreens - 1][idx].d / 2);
        } else {
            event_set_place_text(h, gRiders_65[0].f668, idx);
        }
    }

    gdl = event_draw_position(gdl, 0, font_text_create(D_800F0544, 0x20, 3, 2, 0x21));
    gdl = font_render(gdl);
    if (gEvent.countdown != 0 && gEvent.state != 0 &&
        (gEvent.state != 1 || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, idx);
    }
    return gdl;
}

typedef struct { s32 unk0; u16 *col; Img_2 img[5]; } Hud_3;
extern Hud_3 *sCountdownTextures_2;

Gfx *event_draw_countdown(Gfx *gdl, s32 a)
{
    f32 scale;
    s32 alpha;
    u16 col;
    Img_2 *img;
    s32 sw, dsdx, th;
    s32 *r;

    if (gEvent.state == 5 && gEvent.countdown == 0)
        return gdl;
    if (D_800C8CE0 != 0)
        a = 0;
    switch (gEvent.state) {
    default:
        return gdl;
    case 1: {
        f32 v;
        col = sCountdownTextures_2->col[4];
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
    case 2: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[2];
        v = scale * 100.0f;
        v += 0.0f;
        col = sCountdownTextures_2->col[2];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 3: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[1];
        v = scale * 100.0f;
        v += 0.0f;
        col = sCountdownTextures_2->col[1];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 4: {
        f32 v;
        scale = gEvent.countdown + 10;
        scale /= 46.0f;
        img = &sCountdownTextures_2->img[0];
        v = scale * 100.0f;
        v += 0.0f;
        col = sCountdownTextures_2->col[0];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 5:
    case 14:
    case 15:
        scale = gEvent.countdown + 1;
        scale *= 0.0625f;
        img = &sCountdownTextures_2->img[3];
        col = sCountdownTextures_2->col[3];
        alpha = scale * 120.0f;
        scale = 3.5f - scale * 3.0f;
        break;
    }

    sw = scale * (img->w * 8.0f);
    dsdx = 1024.0f / scale;
    if (gEvent.state == 1)
        th = scale * img->h * 0.5f;
    else
        th = scale * img->h;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, 0x4041C8, 0);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, img->img);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPLoadSync(gdl++);
    gDPLoadTile(gdl++, G_TX_LOADTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetPrimColor(gdl++, 0, 0, (col >> 8) & 0xF8, (col >> 3) & 0xF8, (col << 2) & 0xF8, alpha);
    {
        u8 *t = (u8 *)gViewRects;
        r = (s32 *)((a << 4) + ((gNumScreens - 1) << 6) + t);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, r[0], r[1], r[0] + r[2] - 1, r[1] + r[3] - 1);
    gSPTextureRectangle(gdl++, (r[0] + r[2] / 2 - sw / 2) << 2, (r[1] + r[3] / 2 - th / 2) << 2,
                        (r[0] + r[2] / 2 + sw / 2) << 2, (r[1] + r[3] / 2 + th / 2) << 2,
                        G_TX_RENDERTILE, 0, (img->h - 1) << 5, dsdx,
                        gEvent.state == 1 ? -(dsdx * 2) : -dsdx);
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    u8 pad[0x664];
    s16 num;
    s16 ang;
    u8 pad2[0x7E0 - 0x668];
} Player_16;

void *event_draw_position(void *param_1, s32 param_2, void *param_3)
{
    Player_16 *base = gRiders;
    Player_16 *pl = &base[param_2];
    s32 a = pl->ang;
    char buf[3];

    if (a > 0x180) {
        a = 0x300 - a;
    }
    param_1 = font_render(param_1);
    font_reset_queue();
    buf[0] = ((u8 *)&pl->num)[1] + '1';
    buf[1] = (pl->num < 3) ? ((u8 *)&pl->num)[1] + ':' : '=';
    buf[2] = 0;
    font_text_add(param_3, buf);
    if (gViMode == 2) {
        font_set_scale(a << 9, a << 9, 0, 0);
        font_draw_text(param_3, gViewRects[gNumScreens - 1][gCurView].x + gViewRects[gNumScreens - 1][gCurView].w - 0x3C,
                      gViewRects[gNumScreens - 1][gCurView].y + 0x14);
        param_1 = font_render(param_1);
        font_set_scale(0x10000, 0x20000, 0, 0);
    } else {
        font_set_scale(a << 8, a << 8, 0, 0);
        font_draw_text(param_3, gViewRects[gNumScreens - 1][gCurView].x + gViewRects[gNumScreens - 1][gCurView].w - 0x28,
                      gViewRects[gNumScreens - 1][gCurView].y + 0xA);
        param_1 = font_render(param_1);
        font_set_scale(0x10000, 0x10000, 0, 0);
    }
    font_reset_queue();
    return param_1;
}

#define VP gViewRects[gNumScreens - 1][gCurView]

typedef struct {
    u8 pad0[4];
    s32 w4;
    u8 pad8[0x18 - 0x8];
    u32 w18;
} Txt_2;

typedef struct {
    s16 h0;
    union { s16 h; u8 b[2]; } u2;
    u8 pad4[4];
    s16 h8;
    s16 hA;
    s16 hC;
    u8 padE[2];
    s32 w10;
    s32 w14[6];
    s32 w2C;
} Lap;

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x420 - 0x54];
    f32 f420;
    u8 pad424[0x660 - 0x424];
    Lap lap;
    u8 pad690[0x6EE - 0x690];
    u8 b6EE;
    u8 pad6EF[0x7AC - 0x6EF];
    s16 h7AC;
    u8 pad7AE[0x7E0 - 0x7AE];
} Player_17;

extern Player_17 gRiders_66[];

Gfx *event_draw_hud_race(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[88];
    char *p;
    Lap *lp;
    Txt_2 *tRace;
    Txt_2 *tLap;
    Txt_2 *tBest;
    Txt_2 *tPlace;
    Txt_2 *tRaceL;
    Txt_2 *tLapL;
    Txt_2 *tBestL;
    Txt_2 *tSpeed;
    Txt_2 *tMsg;
    Txt_2 *t;
    s32 v;
    f32 f;
    s32 x, y;
    s32 xa, xb, yy, dy;

    lp = &gRiders_66[(gCamTargetOverride == -1) ? pl + D_800C8CE0 : gCamTargetOverride].lap;
    if (gNumScreens == 1 && gCamTargetOverride != -1) {
        pl = gCamTargetOverride;
    }
    font_reset_queue();
    if (gViMode == 2) {
        font_set_scale(0x10000, 0x20000, 0, 0);
    }
    if (gNumScreens < 3) {
        tPlace = font_text_create(gHudTextObjs[0], 0x20, 3, 2, 0x21);
        if (gNumScreens == 1) {
            tSpeed = font_text_create(gHudTextObjs[1], 0x20, 3, 2, 0x12);
        } else {
            tSpeed = font_text_create(gHudTextObjs[1], 0x20, 3, 2, 0x11);
        }
        tRace = font_text_create(gHudTextObjs[2], 0x20, 2, 2, 0x10);
        tBest = font_text_create(gHudTextObjs[3], 0x20, 2, 2, 0x10);
        tRaceL = font_text_create(gHudTextObjs[4], 0x20, 2, 2, 0x10);
        tBestL = font_text_create(gHudTextObjs[5], 0x20, 2, 2, 0x10);
        if (gEvent.countdown != 0 && gEvent.state == 5) {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 2, 0x21);
        } else {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 4, 0x21);
        }
        if (lp->hA != 0 || lp->h0 == gEvent.numLaps) {
            if (lp->hA != 0) {
                lp->hA--;
            }
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 4, 0x10);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 4, 0x10);
        } else {
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 2, 0x10);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 2, 0x10);
        }
        if (gGame.flags & 0x20000000) {
            Txt_2 *t1, *t2, *t3;
            t1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, 0x10);
            debug_ftoa(buf, gRiders_66[0].f48, 5);
            font_text_add(t1, buf);
            font_draw_text(t1, 0x28, 0x1E);
            t2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
            debug_ftoa(buf, gRiders_66[0].f50, 5);
            font_text_add(t2, buf);
            font_draw_text(t2, 0x28, 0x28);
            t3 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
            int_to_str(gDebugFreeMem, buf);
            font_text_add(t3, buf);
            font_draw_text(t3, 0x28, 0x32);
        }
        if (gGame.mode == 6 && gEvent.time == 0) {
            t = font_text_create(gHudTextObjs[19], 0x20, 2, 2, 0x11);
            font_text_add(t, "Press Up C Button for side view");
            font_draw_text(t, 0x100, 0x28);
        }
    } else {
        tPlace = font_text_create(gHudTextObjs[0], 8, 2, 2, 0x12);
        tSpeed = font_text_create(gHudTextObjs[1], 0x20, 2, 2, 0x12);
        tRace = font_text_create(gHudTextObjs[2], 0x20, 2, 2, 0x10);
        tBest = font_text_create(gHudTextObjs[3], 0x20, 2, 2, 0x10);
        tRaceL = font_text_create(gHudTextObjs[4], 0x20, 2, 2, 0x10);
        tBestL = font_text_create(gHudTextObjs[5], 0x20, 2, 2, 0x10);
        tLap = font_text_create(gHudTextObjs[6], 0x20, 2, 4, 0x10);
        tLapL = font_text_create(gHudTextObjs[7], 0x20, 2, 4, 0x10);
        if (gEvent.countdown != 0 && gEvent.state == 5) {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 2, 0x21);
        } else {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 4, 0x21);
        }
    }
    if (lp->h8 != -1) {
        event_set_place_text(tMsg, lp->h8, pl);
    }
    if (gReplayPlayback == 0) {
        if (gEvent.countdown != 0 && gEvent.state != 0 && (gEvent.state != 1 || gEvent.countdown < 0x5B)) {
            gdl = event_draw_countdown(gdl, pl);
        } else if (gPauseMenu == 0 && lp->h8 == -1) {
            if (gRiders_66[pl].lap.hC != 0) {
                v = gRiders_66[pl].lap.w10;
                gRiders_66[pl].lap.hC--;
                if (v < 0) {
                    v = -v;
                    buf[0] = '+';
                    tMsg->w4 = 8;
                } else if (v > 0) {
                    buf[0] = '-';
                    tMsg->w4 = 7;
                }
                buf[1] = 0;
                event_format_time_short(v, buf + 1);
                font_text_add(tMsg, buf);
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 4);
            } else if (((gRiders_66[pl].h7AC - 1) & 0x20) == 0) {
                font_text_add(tMsg, "Off Course");
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            } else if (gRiders_66[pl].b6EE != 0 && (gRaceFrame & 0x20)) {
                font_text_add(tMsg, "Wrong Way");
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            }
        }
        switch (gNumScreens) {
        case 1:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPPipeSync(gdl++);
            if (gEvent.numRiders >= 2) {
                gdl = event_draw_position(gdl, pl, tPlace);
            }
            f = gRiders_66[pl].f420;
            int_to_str((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            font_text_add(tSpeed, buf);
            x = 0x1BA;
            if (gViMode == 2) {
                x = 0x228;
            }
            y = 0xD6;
            if (gViMode == 2) {
                y = 0x180;
            }
            font_draw_text(tSpeed, x, y);
            if (lp->h0 == gEvent.numLaps) {
                event_format_time(lp->w2C, buf);
            } else {
                event_format_time(gEvent.time, buf);
            }
            if (gHudLevel < 2) {
                font_text_add(tRace, buf);
                font_text_add(tRaceL, "Race: ");
                xa = VP.x + 0x43;
                xb = VP.x + 0x14;
                if (gViMode != 2) {
                    yy = VP.y + VP.h - 0x1C;
                    dy = 10;
                } else {
                    yy = VP.y + VP.h - 0x38;
                    dy = 0x14;
                }
                if (gCurTrack_8->w50 & 2) {
                    yy += dy;
                }
                font_draw_text(tRaceL, xb, yy);
                font_draw_text(tRace, xa, yy);
                yy += dy;
                __builtin_strcpy(buf, "Lap ");
                p = buf;
                if (lp->hA != 0 || lp->h0 == gEvent.numLaps) {
                    int_to_str(lp->h0, p + strlen(p));
                } else {
                    int_to_str(lp->h0 + 1, p + strlen(p));
                }
                str_cat(p, ": ");
                font_text_add(tLapL, buf);
                event_format_time(lp->w14[(lp->hA != 0 || lp->h0 == gEvent.numLaps) ? lp->h0 - 1 : lp->h0], buf);
                font_text_add(tLap, buf);
                font_draw_text(tLapL, xb, yy);
                font_draw_text(tLap, xa, yy);
                yy += dy;
                if (!(gCurTrack_8->w50 & 2)) {
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
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gdl = render_draw_temp_gauge(gdl, a1, a2, gCurView);
            break;
        case 2:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            gdl = event_draw_position(gdl, pl, tPlace);
            if (gHudLevel < 2) {
                f = gRiders_66[pl].f420;
                int_to_str((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
                font_text_add(tSpeed, buf);
                font_draw_text(tSpeed, VP.x + VP.w - 0x2D, VP.y + VP.h - 0xB);
                if (lp->h0 == gEvent.numLaps) {
                    event_format_time(lp->w2C, buf);
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
            if (lp->hA != 0 || lp->h0 == gEvent.numLaps) {
                int_to_str(lp->h0, p + strlen(p));
            } else {
                int_to_str(lp->h0 + 1, p + strlen(p));
            }
            str_cat(p, ": ");
            font_text_add(tLapL, buf);
            font_draw_text(tLapL, VP.x + 10, VP.y + VP.h - 8);
            event_format_time(lp->w14[(lp->hA != 0 || lp->h0 == gEvent.numLaps) ? lp->h0 - 1 : lp->h0], buf);
            font_text_add(tLap, buf);
            font_draw_text(tLap, VP.x + 0x43, VP.y + VP.h - 8);
            gdl = event_draw_temp_gauge(font_render(gdl), a1, a2, pl);
            break;
        default:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            buf[0] = gRiders_66[pl].lap.u2.b[1] + '1';
            buf[1] = (gRiders_66[pl].lap.u2.h < 3) ? gRiders_66[pl].lap.u2.b[1] + '{' : '~';
            buf[2] = 0;
            font_text_add(tPlace, buf);
            font_draw_text(tPlace, VP.x + VP.w - 10, VP.y + 13);
            f = gRiders_66[pl].f420;
            int_to_str((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
            font_text_add(tSpeed, buf);
            font_text_add(tSpeed, "mph");
            font_draw_text(tSpeed, VP.x + VP.w - 10, VP.y + VP.h - 6);
            gdl = event_draw_temp_gauge(font_render(gdl), a1, a2, pl);
            break;
        }
    } else {
        gdl = font_render(gdl);
    }
    if (gEvent.allFinished != 0 && gCurView == gNumScreens - 1) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                      gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
        font_text_create(gMenuTextObjs[0], 0x40, 1, 2, 0x21);
        font_text_create(gMenuTextObjs[1], 0x40, 1, 2, 0x21);
        font_text_add(gMenuTextObjs[0], "Press START to Continue");
        if (gNumScreens == 1) {
            font_draw_text(gMenuTextObjs[0], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 + 0x17);
        } else {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xA0);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x404240, 0);
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

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
} Player_18;

extern Player_18 gRiders_67[];

Gfx *event_draw_hud_soccer(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    char buf[32];
    char msg[64];
    s32 cx;
    s32 cy;
    s32 n;
    s32 team;

    font_reset_queue();
    if (p3 == 0) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                      gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
        if (gEvent.state == 7 || gEvent.state == 9 || gEvent.state == 11) {
            gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xE6);
            gDPRaw(gdl++, 0xE3000C00, 0);
            gDPRaw(gdl++, 0xD7000002, 0x80008000);
            gDPRaw(gdl++, 0xE3001001, 0);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            cx = 0x100;
            cy = 0x78;
            if (gGame.numPlayers == 4) {
                cx = 0x13C;
            }
            gdl = render_draw_image((cx - sSoccerMapTexture_2[8] * 4) | 1, cy - (sSoccerMapTexture_2[9] >> 1) - 1, sSoccerMapTexture_2 + 8, gdl,
                                p2, 1, 1, 6, 0);
            gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            gDPRaw(gdl++, 0xD7000000, 0x80008000);
            gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x28, 0xFF, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders_67[0].f50 * 0.25f / 0.625f - 2.0f, cy + gRiders_67[0].f48 * 0.25f - 2.0f,
                             cx + gRiders_67[0].f50 * 0.25f / 0.625f + 2.0f, cy + gRiders_67[0].f48 * 0.25f + 2.0f);
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0x28, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders_67[1].f50 * 0.25f / 0.625f - 2.0f, cy + gRiders_67[1].f48 * 0.25f - 2.0f,
                             cx + gRiders_67[1].f50 * 0.25f / 0.625f + 2.0f, cy + gRiders_67[1].f48 * 0.25f + 2.0f);
            if (gGame.numPlayers == 4) {
                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0xFA, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + gRiders_67[2].f50 * 0.25f / 0.625f - 2.0f, cy + gRiders_67[2].f48 * 0.25f - 2.0f,
                                 cx + gRiders_67[2].f50 * 0.25f / 0.625f + 2.0f, cy + gRiders_67[2].f48 * 0.25f + 2.0f);
                gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0x28, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + gRiders_67[3].f50 * 0.25f / 0.625f - 2.0f, cy + gRiders_67[3].f48 * 0.25f - 2.0f,
                                 cx + gRiders_67[3].f50 * 0.25f / 0.625f + 2.0f, cy + gRiders_67[3].f48 * 0.25f + 2.0f);
            }
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0xFA, 0xFF);
            gDPFillRectangle(gdl++, cx + gSoccerBall.z * 0.25f / 0.625f - 1.0f, cy + gSoccerBall.x * 0.25f - 1.0f,
                             cx + gSoccerBall.z * 0.25f / 0.625f + 1.0f, cy + gSoccerBall.x * 0.25f + 1.0f);
        }
        switch (gEvent.state) {
        case 6:
            font_text_create(gHudTextObjs[0], 0x40, 1, 4, 0x21);
            if (gGame.numPlayers == 2) {
                font_text_add(gHudTextObjs[0], "One on One Soccer: First Half");
            } else {
                font_text_add(gHudTextObjs[0], D_800D865C);
            }
            font_draw_text(gHudTextObjs[0], gScreenWidth / 2, gScreenHeight / 2);
            break;
        case 8:
        case 10:
        case 12:
            font_text_create(gHudTextObjs[0], 0x40, 1, 6, 0x21);
            switch (gEvent.state) {
            case 10:
                font_text_add(gHudTextObjs[0], "Full Time Draw : Overtime Required");
                break;
            case 8:
                font_text_add(gHudTextObjs[0], "Half Time");
                break;
            case 12:
                font_text_add(gHudTextObjs[0], "Final");
                break;
            }
            font_draw_text(gHudTextObjs[0], gScreenWidth / 2, gScreenHeight / 2 - 13);
            font_text_create(gHudTextObjs[1], 0x40, 1, 2, 0x20);
            font_text_create(gHudTextObjs[2], 0x40, 1, 2, 0x20);
            font_text_create(gHudTextObjs[3], 0x40, 1, 4, 0x20);
            font_text_create(gHudTextObjs[4], 0x40, 1, 4, 0x20);
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
            if (gEvent.state == 12) {
                font_text_create(gHudTextObjs[0], 0x40, 1, 4, 0x21);
                font_text_add(gHudTextObjs[0], "Press START To Continue");
                font_draw_text(gHudTextObjs[0], gScreenWidth / 2,
                              gViewRects[gNumScreens - 1][0].y + gViewRects[gNumScreens - 1][0].h / 2);
                font_draw_text(gHudTextObjs[0], gScreenWidth / 2,
                              gViewRects[gNumScreens - 1][gNumScreens - 1].y +
                                  gViewRects[gNumScreens - 1][gNumScreens - 1].h / 2);
            }
            break;
        default:
            font_text_create(gHudTextObjs[0], 0x40, 1, 2, 0x20);
            font_text_create(gHudTextObjs[1], 0x40, 1, 2, 0x20);
            font_text_create(gHudTextObjs[2], 0x40, 1, 4, 0x20);
            font_text_create(gHudTextObjs[3], 0x40, 1, 4, 0x20);
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
            font_text_create(gHudTextObjs[4], 0x40, 1, 2, 0x22);
            font_text_create(gHudTextObjs[5], 0x40, 1, 4, 0x22);
            switch (gEvent.state) {
            case 11:
                font_text_add(gHudTextObjs[4], "Overtime");
                break;
            case 7:
                font_text_add(gHudTextObjs[4], "First Half");
                break;
            case 9:
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
    n = gEvent.flags & 0xF;
    if (n != 0) {
        team = n - 1;
        font_text_create(gHudTextObjs[7], 0x40, gGame.numPlayers == 4 ? 2 : 1, 4, 0x21);
        if (team == (p3 >> ((gGame.numPlayers - 1) >> 1))) {
            if (gEvent.flags & 0x20) {
                font_text_add(gHudTextObjs[7], "Team ");
                int_to_str((team ^ 1) + 1, buf);
                font_text_add(gHudTextObjs[7], buf);
                font_text_add(gHudTextObjs[7], " scored for you!");
            } else {
                font_text_add(gHudTextObjs[7], D_800D871C);
            }
        } else {
            if (gEvent.flags & 0x20) {
                strcpy(msg, "You helped team ");
                int_to_str(n, msg + strlen(msg));
                str_cat(msg, " score!");
            } else {
                strcpy(msg, "Team ");
                int_to_str(n, msg + strlen(msg));
                str_cat(msg, " scored!");
            }
            font_text_add(D_800F054C, msg);
        }
        font_draw_text(D_800F054C, gViewRects[gNumScreens - 1][p3].x + gViewRects[gNumScreens - 1][p3].w / 2,
                      gViewRects[gNumScreens - 1][p3].y + gViewRects[gNumScreens - 1][p3].h / 2);
    }
    return font_render(gdl);
}

#define gDPWords(pkt, a, b) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (a); \
        _g->words.w1 = (b); \
    }

typedef struct {
    u8 w;
    u8 h;
    u8 unk2[6];
} Glyph;

typedef struct Font {
    s32 unk0;
    u16 *palette;
    Glyph glyphs[1];
} Font;

Gfx *event_draw_glyph_string(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y) {
    while (*str != 0) {
        s32 w; s32 hh;

        gDPWords(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPWords(gdl++, 0xE3001001, 0x8000);
        gDPWords(gdl++, 0xFD100000, (u32)gGame.font->palette);
        gDPWords(gdl++, 0xE8000000, 0);
        gDPWords(gdl++, 0xF5000100, 0x07000000);
        gDPWords(gdl++, 0xE6000000, 0);
        gDPWords(gdl++, 0xF0000000, 0x073FC000);
        gDPWords(gdl++, 0xE7000000, 0);
        w = gGame.font->glyphs[*str].w << 3;
        hh = gGame.font->glyphs[*str].h;
        gdl = render_draw_image(x, y - (hh >> 1), &gGame.font->glyphs[*str], gdl, arg1, 1, 1, 3, 0);
        str++;
        x += w;
    }
    return gdl;
}

#define CUR_FLAGS (gTutorialLessons_10[gSeason_5->f64].flags)

typedef struct {
    s32 val;
    char *str[3];
} Entry_4;

typedef struct {
    u8 pad0[0xC];
    u8 flags;
    u8 num;
    u8 padE[2];
    char *name;
    u8 pad14[0xC];
    u32 count;
    Entry_4 *entries;
} Track_13;

typedef struct {
    u8 pad[0x420];
    f32 f420;
    u8 pad2[0x3BC];
} Bike_32;

extern Track_13 gTutorialLessons_10[];
extern Bike_32 gRiders_68[];

Gfx *tutorial_draw_hud(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3)
{
    Track_13 *trk;
    Entry_4 *e;
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
    f32 f;
    char buf[32];
    s32 cx;
    u8 *g;

    trk = &gTutorialLessons_10[gSeason_5->f64];
    font_reset_queue();
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPRaw(gdl++, 0xE200001C, 0x404240);
    if (gEvent.state == 0xD) {
        if (gEvent.tutorialAction == 0) {
            off = -(gEvent.tutorialTimer * 2);
        } else {
            off = 0;
            if (gEvent.tutorialAction == 2) {
                off = -((60 - gEvent.tutorialTimer) * 2);
            }
        }
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800EFD68_2 + 4));
        cx = gScreenWidth / 2;
        w = D_800EFD68_2[0x10] * 4 - 10;
        gdl = render_draw_image(cx - w, off + 0x1E, D_800EFD68_2 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        gdl = render_draw_image(gScreenWidth / 2 - D_800EFD6C_2[0x18] * 4, off + 0x1A, D_800EFD6C_2 + 0x18, gdl, arg2, 1, 1, 6, 0);
        font_text_create(gHudTextObjs[i], 0x40, 1, 2, 0x20);
        font_text_add(gHudTextObjs[i], trk->name);
        font_draw_text(gHudTextObjs[i], 0x4D - i * 3, i * 13 + 0x44 + off);
        font_text_create(gHudTextObjs[3], 0x40, 1, 2, 0x21);
        __builtin_memcpy(buf, D_800D88D0, 9);
        buf[strlen(buf) - 1] += trk->num;
        font_text_add(gHudTextObjs[3], buf);
        font_draw_text(gHudTextObjs[3], gScreenWidth / 2 + 3, off + 0x29);
    } else {
        if (gActiveReplay != 0) {
            if (gFrameCounter & 0x20) {
                font_text_create(gHudTextObjs[5], 0x40, 1, 2, 0x20);
                font_text_add(gHudTextObjs[5], "Press START");
                font_draw_text(gHudTextObjs[5], 0x2F, 0x2D);
            }
        } else {
            flags = CUR_FLAGS;
            if (flags & 4) {
                font_text_create(gHudTextObjs[0], 0x40, 2, 6, 0x20);
                if (gEvent.flags & 0x1000) {
                    font_text_add(gHudTextObjs[0], "Takedown Required");
                } else {
                    font_text_add(gHudTextObjs[0], "Takedown Complete");
                }
                font_draw_text(gHudTextObjs[0], 0x21, 0x2D);
            } else if (flags & 8) {
              if (!(flags & 2)) {
                font_text_create(gHudTextObjs[0], 0x40, 1, 6, 0x20);
                v = gEvent.time;
                if (v < 0) {
                    v = -v;
                }
                event_format_time_short(v, buf);
                font_text_add(gHudTextObjs[0], "Time: ");
                font_text_add(gHudTextObjs[0], buf);
                if (gEvent.time < 0) {
                    gHudTextObjs[0][1] = 8;
                }
                font_draw_text(gHudTextObjs[0], 0x21, 0x2D);
              }
            }
        }
        font_text_create(gHudTextObjs[1], 0x40, 1, 4, 0x20);
        font_text_add(gHudTextObjs[1], trk->name);
        font_draw_text(gHudTextObjs[1], 0x3D, 0x20);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        if ((CUR_FLAGS & 2) && D_800F0738 >= 2 && D_800F0738 < 10) {
            font_text_create(gHudTextObjs[2], 0x40, 2, 2, 0x20);
            font_text_add(gHudTextObjs[2], trk->entries[D_800F0738].str[0]);
            font_draw_text(gHudTextObjs[2], 0x140, 0x28);
            p = gdl;
            gDPSetTextureLUT(p++, G_TT_NONE);
            gDPRaw(p++, 0xFC119623, 0xFF2FFFFF);
            k = 27;
            x0 = 0x2EE;
            g = D_800EFD6C_2 + 0x58;
            p = render_draw_image(x0 - g[0] * k * 8 - D_800EFD6C_2[0x60] * 8, 0x1C, D_800EFD6C_2 + 0x60, p, arg2, 1, 1, 6, 0);
            for (m = 0; m < 27; m++) {
                p = render_draw_image(x0 - (k - m) * g[0] * 8, 0x1C, g, p, arg2, 1, 1, 6, 0);
            }
            gdl = event_draw_glyph_string(p, arg2, trk->entries[D_800F0738].str[1], 0x140, 0x40);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        } else {
            if (gTutorialResult == 1) {
                if (D_800F0738 == 0) {
                    i = trk->count - 1;
                } else {
                    i = D_800F0738;
                }
                goto show;
            }
            if (gTutorialResult == -1) {
                for (j = 0; j < 3; j++) {
                    font_text_create(gHudTextObjs[j + 2], 0x40, 2, 2, 0x20);
                    font_text_add(gHudTextObjs[j + 2], sTutorialFailText[j]);
                    font_draw_text(gHudTextObjs[j + 2], 0x20, 0xB2 + j * 9);
                }
            } else {
                if (gActiveReplay == 0 && !(gEvent.flags & 0x8000)) {
                    goto skip;
                }
                if (!(gEvent.flags & 0x800)) {
                    goto skip;
                }
                for (i = 0; i < trk->count; i++) {
                    d = trk->entries[i].val - gEventTime;
                    if (d <= 0) {
                        d = gEventTime - trk->entries[i].val;
                    }
                    if (d >= 3 && !(gEventFlags & 0x8000)) {
                        continue;
                    }
                    {
                    show:
                        n = 3;
                        if (trk->entries[i].str[2] == NULL) {
                            n = 2;
                            if (trk->entries[i].str[1] == NULL) {
                                n = 1;
                            }
                        }
                        for (j = 0; j < n; j++) {
                            font_text_create(gHudTextObjs[j + 2], 0x40, 2, 2, 0x20);
                            font_text_add(gHudTextObjs[j + 2], trk->entries[i].str[j]);
                            font_draw_text(gHudTextObjs[j + 2], 0x20, 0xBB - (n - 1) * 9 / 2 + j * 9);
                        }
                        break;
                    }
                }
            }
            img = D_800EFD6C_2 + 0x30;
            for (i = 0; i < 19; i++) {
                gdl = render_draw_image(i * img[0] * 8 + 0x10, 0xA9, img, gdl, arg2, 1, 1, 6, 0);
            }
            p = render_draw_image(img[0] * 0x98 + 0x10, 0xA9, D_800EFD6C_2 + 0x38, gdl, arg2, 1, 1, 6, 0);
            gdl = event_draw_glyph_string(p, arg2, D_800C9A3C, 0x136, 0xC3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        }
    skip:
        if (gEvent.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img = D_800EFD6C_2 + 0x30;
        } else {
            img = D_800EFD6C_2 + 0x40;
        }
        for (i = 0; i < 9; i++) {
            gdl = render_draw_image(i * img[0] * 8 + 0x10, 0x14, img, gdl, arg2, 1, 1, 6, 0);
        }
        if (gEvent.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img2 = D_800EFD6C_2 + 0x38;
        } else {
            img2 = D_800EFD6C_2 + 0x48;
        }
        gdl = render_draw_image(img[0] * 0x48 + 0x10, 0x14, img2, gdl, arg2, 1, 1, 6, 0);
        gDPRaw(gdl++, 0xE200001C, 0x404240);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        if (gSeason_5->f64 > 0) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, sGreyPalette);
        }
        gdl = render_draw_image(0x21, 0x1B, D_800F0740 + 0x20, gdl, arg2, 1, 1, 3, 0);
        if (gSeason_5->f64 < 16 && gTutorialLessons_10[gSeason_5->f64 + 1].num <= gSeason_5->f0[1]) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, sGreyPalette);
        }
        gdl = render_draw_image(0x96, 0x1B, D_800F0740 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
    }
    gdl = render_draw_temp_gauge(gdl, arg1, arg2, 0);
    font_text_create(gHudTextObjs[6], 0x40, 3, 2, 0x12);
    f = gRiders_68[arg3].f420;
    int_to_str((s32)((f > 0.0f ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
    font_text_add(gHudTextObjs[6], buf);
    x = 0x1BA;
    if (gViMode == 2) {
        x = 0x228;
    }
    y = 0xD6;
    if (gViMode == 2) {
        y = 0x180;
    }
    font_draw_text(gHudTextObjs[6], x, y);
    if ((CUR_FLAGS & 8) && gEvent.countdown != 0
        && (gEvent.state < 5 || gEvent.state == 15 || gEvent.state == 14)
        && (gEvent.state != 1 || gEvent.countdown < 0x5B)) {
        gdl = event_draw_countdown(gdl, arg3);
    }
    return font_render(gdl);
}

typedef struct {
    u8 *hdr;
    u8 pad[0x60];
    s16 idx;
} Ctl_15;

s32 tutorial_next_lesson_unlocked(void) {
    Ctl_15 *c = gSeason;
    s32 a1 = c->idx;
    s32 n = a1 + 1;
    u8 v1, v0;

    if (n > 16) {
        return 0;
    }
    v1 = gTutorialLessons[a1 * 0x28 + 0xD];
    v0 = gTutorialLessons[n * 0x28 + 0xD];
    if (v1 == v0) {
        return 1;
    }
    return v1 < c->hdr[1];
}

#define ROWS (gGame.numPlayers >= 2 ? 6 : 9)
#define COLS (gGame.numPlayers >= 3 ? 8 : 12)
#define RECT (gViewRects[gNumViews - 1][gEvent.finishOrder[0]])

typedef struct Plr_7 {
    u8 pad0[0x64];
    s16 h64;
} Plr_7;

typedef struct Trk_10 {
    u8 pad0[0x1C];
    s32 f1C;
    u8 pad20[0x28 - 0x20];
} Trk_10;

extern Plr_7 *gSeason_12;
extern Trk_10 gTutorialLessons_11[];

Gfx *event_draw_hud(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    int i;
    int j;
    s32 a;
    s32 prev;
    s32 sp;
    int n;

    if (gHudLevel != 3 || gGame.mode == 9) {
        switch (gGame.mode) {
        case 2:
            if (gPauseMenu == 0) {
                gdl = event_draw_hud_soccer(gdl, p1, p2, p3);
            }
            break;
        case 5:
            gdl = event_draw_hud_gates(gdl, p1, p2, p3);
            break;
        case 7:
            gdl = event_draw_hud_hillclimb(gdl, p1, p2, p3);
            break;
        case 10:
            gdl = event_draw_hud_stunt(gdl, p1, p2, p3);
            break;
        case 13:
        case 14:
            gdl = event_draw_hud_testride(gdl, p1, p2, p3);
            break;
        case 9:
            gdl = tutorial_draw_hud(gdl, p1, p2, p3);
            break;
        default:
            gdl = event_draw_hud_race(gdl, p1, p2, p3);
            break;
        }
    } else if (gReplayPlayback == 0) {
        gdl = event_draw_countdown(gdl, p3);
    }
    if (gEvent.finishTimer >= 0 || gEvent.finishTimer == -2) {
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
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
    sp = 14;
    if (gViMode_2 == 2) {
        font_set_scale(0x10000, 0x20000, 0, 1);
        sp = 28;
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects[0][0].x, gViewRects[0][0].y,
                  gViewRects[0][0].x + gViewRects[0][0].w - 1, gViewRects[0][0].y + gViewRects[0][0].h - 1);
    font_reset_queue();
    switch (gPauseMenu) {
    case 6:
        for (n = 0; n < 3; n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, 1);
            font_text_add(gMenuTextObjs[n], sTestRideMenuItems[n]);
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp + sp * n);
        }
        break;
    case 1:
        for (n = 0; n < (gGame.mode == 1 ? 4 : 5); n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, 0x11);
            if (gGame.mode == 1) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSeason[n]);
            } else if (gGame.mode == 2) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSoccer[n]);
            } else {
                font_text_add(gMenuTextObjs[n], sPauseMenuItems[n]);
            }
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp * 2 + sp * n);
        }
        break;
    case 5:
        if (gEvent.tutorialAction != 6) {
            s32 k;
            k = gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp * 2;
            for (n = 0; n < 5; n++) {
                if (n == 1 && gTutorialLessons_11[gSeason_12->h64].f1C == 0) {
                    continue;
                }
                if (n == 3 && tutorial_next_lesson_unlocked() == 0) {
                    continue;
                }
                if (n == 4 && gSeason_12->h64 == 0) {
                    continue;
                }
                font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, 1);
                font_text_add(gMenuTextObjs[n], sTutorialMenuItems[n]);
                font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2, k);
                k += sp;
            }
        }
        break;
    case 4:
        for (n = 0; n < 2; n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, 1);
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

#define RP_2(T, n) ((Rect *)(((gCurView << 4) + (((n) - 1) << 6)) + (u8 *)T))
#define RX(T, n) (RP_2(T, n)->x + RP_2(T, n)->w)
#define RY(T, n) (RP_2(T, n)->y + RP_2(T, n)->h)
#define R8(c) (((c) >> 8) & 0xF8)
#define G8(c) (((c) >> 3) & 0xF8)
#define B8(c) (((c) << 2) & 0xF8)
#define SL(v,s,w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define DIM(c, a) (SL(R8(c) >> 2, 24, 8) | SL(G8(c) >> 2, 16, 8) | SL(B8(c) >> 2, 8, 8) | SL(a, 0, 8))
#define GW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 w, h; u16 pad; void *addr; } Img_6;
typedef struct { s32 f0; u16 *pal; Img_6 img[5]; } HudTex;

typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 pad655[0x7E0 - 0x655];
} Player_19;

extern HudTex *sTempGaugeTextures_2;
extern HudTex *sTempGaugeTextures_3[];
extern Player_19 gRiders_69[];

Gfx *event_draw_temp_gauge(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 fp, n, bar, k;
    u16 col;
    Img_6 *img;
    Rect *r;


    fp = -54;
    n = gNumScreens;
    if (n == 2) fp = -46;
    bar = gRiders_69[pl].f648 * 38.0f / 12000.0f;
    if (bar >= 39) bar = 38;
    GW(gdl++, 0xE7000000, 0);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xD9FFFFFE, 0);
    GW(gdl++, 0xD7000002, 0x80008000);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xFA000000, DIM(sTempGaugeTextures_2->pal[0], 0x8C));
    { Rect *tab = gViewLayouts_3; gdl = render_draw_image(RX(tab, n) - 24, RY(tab, n) + fp, &sTempGaugeTextures_2->img[0], gdl, a2, 1, 1, 6, 0); }
    GW(gdl++, 0xFA000000, DIM(sTempGaugeTextures_2->pal[4], 0xA0));
    gdl = render_draw_image(RX(gViewLayouts_3, gNumScreens) - 24, RY(gViewLayouts_3, gNumScreens) + fp, &sTempGaugeTextures_2->img[4], gdl, a2, 1, 1, 6, 0);
    bar++; GW(gdl++, 0xFA000000, DIM(sTempGaugeTextures_2->pal[3], 0x8C));
    gdl = render_draw_image(RX(gViewLayouts_3, gNumScreens) - 24, RY(gViewLayouts_3, gNumScreens) + fp, &sTempGaugeTextures_2->img[3], gdl, a2, 1, 1, 6, 0);
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
        GW(gdl++, 0xE4000000 | ((((r->x + r->w) * 4 + (img->w * 8 - 24) * (gScreenWidth << 2) / 512) & 0xFFF) << 12) |
                      (((r->y + r->h) * 4 + (fp + img->h) * (gScreenHeight << 2) / 240) & 0xFFF),
           ((((r->x + r->w) * 4 + -24 * (gScreenWidth * 4) / 512) & 0xFFF) << 12) |
               (((r->y + r->h) * 4 + (fp + img->h - bar) * (gScreenHeight << 2) / 240) & 0xFFF));
        GW(gdl++, 0xE1000000, ((bar - 1) << 5) & 0xFFFF);
        GW(gdl++, 0xF1000000, ((gScreenWidth == 640) ? 0x03330000 : 0x04000000) | ((gScreenHeight == 240) ? 0xFC00 : 0xFE00));
        GW(gdl++, 0xE7000000, 0);
    }
    if (gRiders_69[pl].f654) {
        HudTex *h = sTempGaugeTextures_3[0]; GW(gdl++, 0xFA000000, (((h->pal[2] >> 8) & 0xF8) << 24) | (((gFrameCounter << 3) & 0x100) ? ((gFrameCounter << 3) & 0xFF) : (0xFF ^ ((gFrameCounter << 3) & 0xFF))));
            { Rect *tab = gViewLayouts_3; gdl = render_draw_image(RX(tab, gNumScreens) - 24, RY(tab, gNumScreens) + fp, &sTempGaugeTextures_2->img[2], gdl, a2, 1, 1, 6, 0); }
    }
    GW(gdl++, 0xFA000000, (((sTempGaugeTextures_2->pal[1] >> 8) & 0xF8) << 24) | (((sTempGaugeTextures_2->pal[1] >> 3) & 0xF8) << 16) | ((sTempGaugeTextures_2->pal[1] << 10) & 0xF800) | 0x82);
    { Rect *tab = gViewLayouts_3; return render_draw_image(RX(tab, gNumScreens) - 24, RY(tab, gNumScreens) + fp, &sTempGaugeTextures_2->img[1], gdl, a2, 1, 1, 6, 0); }
}

s32 tutorial_has_prev_lesson(void)
{
    return *(s16 *)(gSeason + 0x64) != 0;
}

typedef struct {
    s32 limit;
    s32 counter;
    s32 arg;
} Ctr_2;

extern Ctr_2 D_800CA924_3;

void event_exit(void)
{
    if (0 < D_800CA924_3.limit && (gGame.flags & 4) == 0) {
        D_800CA924_3.counter = D_800CA924_3.counter + 1;
        if (D_800CA924_3.counter == D_800CA924_3.limit) {
            mem_free(D_800CA924_3.arg);
        }
    }
    if (gGame.mode == 9 && gEvent.tutorialBuf != 0) {
        mem_free(gEvent.tutorialBuf);
    }
    save_write(1, 0);
}

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
    return res_load(D_800E7728, idx);
}

s32 track_res_load_map(s32 *p) {
    s32 r = 0;
    if (p != NULL) {
        sTrackResMap = res_load(D_800E7728, *p);
        r = sTrackResMap != 0;
    }
    return r;
}

u32 tutorial_lesson_passed(void) {
    u8 *e = gTutorialLessons;
    s16 idx = *(s16 *)(gSeason + 0x64);
    e = e + idx * 40;
    if (e[0xC] & 4) {
        return ((gEvent.flags >> 12) ^ 1) & 1;
    }
    if (*(u32 *)(e + 0x1C) != 0) {
        return 0;
    }
    return (u32)~gEvent.time >> 31;
}

s32 event_request_exit(void)
{
    if (gRacePhase == 5) {
        gRacePhase = 6;
        gRacePhaseTimer = 0x14;
        return 1;
    }
    return 0;
}

void event_gates_init(void) {
    gEvent.flags |= 0x10;
    race_reset_grid_order();
}

void event_stop_sounds_rumble(void)
{
    snd_stop_all_riders();
    rumble_stop_all();
}

void event_update_gates(void)
{
    Event *g;

    event_update_countdown();
    g = &gEvent;
    if (g->allFinished == 0) {
        if ((g->flags & 0x10) == 0) {
            s32 n = g->time + 1;
            g->time = n;
            if (n == 0x34BC0) {
                event_rider_finished(0);
                g->allFinished = 1;
            }
        }
    } else if ((gPlayerInputs_2 & 0x40) != 0 && (gPlayerInputsPrev & 0x40) == 0) {
        event_request_exit();
    }
}

typedef struct {
    u8 pad0[0x69C];
    s32 f69C;
    u8 pad1[0x7E0 - 0x6A0];
} Ent_56;

extern Ent_56 gRiders_70[];

void event_hillclimb_init(void) {
    Ent_56 *e;

    gEvent.unk_48 = -1;
    gEvent.flags |= 0x10;
    race_reset_grid_order();
    for (e = gRiders_70; e < &gRiders_70[gGame.numPlayers]; e++) {
        e->f69C = 7200;
    }
}

void event_stunt_init(void)
{
    Event *p = &gEvent;
    p->flags = p->flags | 0x10;
    p->time = 0x1C20;
    race_reset_grid_order();
}

void event_soccer_init(void)
{
    Event *p;
    s32 w;

    p = &gEvent;
    w = p->flags;
    p->countdown = 0xB4;
    gEvent.state = 6;
    p->ballToucher = 0;
    p->goals[0] = 0;
    p->goals[1] = 0;
    p->goalTimer = 0;
    p->flags = w | 0x10;
}

void event_reset_places(void) {
    Event *c = &gEvent;
    s32 i;
    s16 *t;

    if (c->state == 8) {
        race_set_grid_order(2, 0);
    }
    i = 0;
    c->numFinished = 0;
    if (c->numRiders > 0) {
        s16 neg = -1;
        Event *d = c;
        t = D_800E3E20;
        do {
            t[4] = neg;
            t += 0x3F0;
        } while (++i < d->numRiders);
    }
}

typedef struct {
    u8 pad[0x668];
    s16 x668;
    u8 pad2[0x7E0 - 0x66A];
} Rider_4;

typedef struct {
    u8 pad[0x66];
    u8 a[1];
} Thing;

extern Rider_4 gRiders_71[];
extern Thing *gSeason_13;

void event_clear_season_race_points(void) {
    s32 i;
    for (i = 0; i < gEvent.numRiders; i++) {
        if (gRiders_71[i].x668 != -1) {
            gSeason_13->a[i] = 0;
        }
    }
}

void func_8005B2E8(void)
{
    event_update_countdown();
    if (gEvent.state == 5) {
        if (gEvent.time != 0) {
            s32 r = gEvent.time - 1;
            gEvent.time = r;
            if (r == 0) {
                D_800E3E2A = 0x78;
            }
        }
    }
}

void event_hud_free(void)
{
    s32 i;
    s32 *p1;
    s32 *p2;

    if (gGame.mode == 9) {
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
        func_800C204C(*p1);
        mem_free(*p1);
        p1++;
        func_800C204C(*p2);
        mem_free(*p2);
        p2++;
    } while (i < 0x1e);
    return;
}

#define PKT_2(a, b) { Gfx *_g = dl++; _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8005B404(s32 p1, s32 p2, s32 p3, Gfx *dl, s32 p5)
{
    s32 i = 0;
    s32 v;
    u8 *base;
    u8 *pb;

    PKT_2(0xE3001001, 0);
    PKT_2(0xFC119623, 0xFF2FFFFF);
    base = D_800EFD6C;
    pb = base + 0x58;
    dl = (Gfx *)render_draw_image(p1 - p3 * *pb * 8 - base[0x60] * 8, p2, base + 0x60, (u32 *)dl, p5, 1, 1, 6, 0);
    for (i = 0; i < p3; i++) {
        v = p3 - i;
        dl = (Gfx *)render_draw_image(p1 - v * *pb * 8, p2, pb, (u32 *)dl, p5, 1, 1, 6, 0);
    }
    return dl;
}

s32 tutorial_lesson_has_demo(void)
{
    s32 idx = *(s16 *)(gSeason + 0x64);
    return gTutorialLessons_12[idx * 10 + 7] != 0;
}

typedef struct Ent_57 {
    s32 key;
    s32 val;
} Ent_57;

s32 track_res_find(s32 key)
{
    Ent_57 *p = sTrackResMap;
    if (p == NULL) {
        return -1;
    }
    while (p->val >= 0) {
        if (p->key == key) {
            return ((volatile Ent_57 *)p)->val;
        }
        p++;
    }
    return -1;
}
