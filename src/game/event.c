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

typedef struct {
    s32 w0;
    u8 pad4[0x22];
    s16 h26;
} Mode;

extern Prof *gSeason_5;
extern Mode gEvent_5;

#define CUR (gTutorialLessons_8[*(s16 *)(gSeason + 0x64)])

typedef struct {
    s32 f00;
    u8 p04[0x22];
    s16 f26;
    u8 p28[4];
    s32 f2c;
    u8 p30[0x28];
    u32 f58;
    u8 p5c[0x14];
    s32 f70;
    s32 f74;
    s32 f78;
    void *f7c;
    s32 f80;
} Blk_13;

typedef struct {
    u8 p0[0xC];
    u8 fl;
    u8 p1[7];
    s32 w14;
    u8 p2[0x28 - 0x18];
} Ent_54;

extern Blk_13 gEvent_18;
extern Ent_54 gTutorialLessons_8[];

void tutorial_init_lesson(s32 a0)
{
    Blk_13 *s;

    race_reset_grid_order();
    s = &gEvent_18;
    if (s->f7c == 0) {
        s->f7c = mem_alloc_dbg(0x80, 0, (s32)D_800D7340, 0x16D);
    }
    D_800F0180 = 1;
    if (CUR.fl & 4) {
        s->f2c = 0;
        s->f58 |= 0x1000;
    } else {
        s->f58 &= ~0x1000;
    }
    if (CUR.fl & 8) {
        gEvent_18.f26 = 0x3F;
        gEvent_18.f00 = 1;
        gEvent_18.f58 |= 0x10;
    } else {
        gEvent_18.f58 &= ~0x10;
    }
    gEvent_18.f58 &= ~0x4000;
    if (CUR.fl & 8) {
        gEvent_18.f58 |= 0x18800;
        gEvent_18.f80 = 600;
        if (CUR.fl & 2) {
            gEvent_18.f2c = 0;
        } else if (gActiveReplay == 0) {
            gEvent_18.f2c = CUR.w14;
        }
    } else {
        gEvent_18.f00 = 0xE;
        gEvent_18.f2c = 0;
    }
    if (gActiveReplay == 0) {
        gEvent_18.f70 = 5;
        gEvent_18.f58 &= ~0x40;
    } else {
        gEvent_18.f26 = 0x3F;
        gEvent_18.f70 = 0;
        gEvent_18.f78 = 0x3C;
        gEvent_18.f58 |= 0x40;
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
            D_800F0618 = 3;
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
        D_800F0618 = 4;
        gRacePhase = 6;
        gRacePhaseTimer = -1;
        snd_play_menu_sound(1);
    }
}

#define G_6 gEvent_19
#define TB (gTutorialLessons_9[gSeason_5->f64])
#define SEG(i) ((Seg_13 *)((s32)((i) * 0x6C) + (s32)gPathNodes))

typedef struct {
    u32 state;      /* 0x00 */
    u8 p04[0x22];
    s16 f26;        /* 0x26 */
    s16 f28;        /* 0x28 */
    u8 p2A[2];
    s32 f2c;        /* 0x2C */
    u8 p30[0x10];
    s32 f40;        /* 0x40 */
    u8 p44[0x14];
    u32 f58;        /* 0x58 */
    u8 p5C[0x14];
    s32 f70;        /* 0x70 */
    s32 f74;
    s32 f78;        /* 0x78 */
    s32 f7c;
    s32 f80;        /* 0x80 */
} Race_2;

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
    u8 p00[0x4A];
    s16 f4A;
    u8 p4C[0x20];
} Seg_13;

extern Race_2 gEvent_19;
extern Track_11 gTutorialLessons_9[];

void tutorial_update(void)
{
    s32 i;

    gRiders[1].timeScale = 1.0375f;
    if (!(G_6.f58 & 0x800)) {
        if ((TB.fl & 8) && (G_6.state < 5 || G_6.state == 15 || G_6.state == 14)) {
            event_update_countdown();
        }
        if (G_6.f26 == 0x2B && G_6.state == 1 && (G_6.f58 & 0x8000)) {
            G_6.f80 = 300;
            G_6.f58 |= 0x10800;
        }
    } else {
        snd_stop_all_riders();
    }
    if (G_6.state == 5) {
        G_6.f58 &= ~0x10;
        if (gActiveReplay != 0 && gRiderAlpha_2 != 0xFF) {
            G_6.f70 = 1;
            G_6.state = 14;
        } else {
            G_6.state = 15;
            G_6.f70 = 5;
        }
    }
    if (G_6.state >= 5 && !(G_6.f58 & 0x800)) {
        if (gActiveReplay == 0) {
            if (TB.fl & 2) {
                G_6.f2c++;
            } else if (!(gRiders[0].flags & 0x10)) {
                G_6.f2c--;
            }
        } else {
            for (i = 0; i < TB.n; i++) {
                if (G_6.f2c == TB.list[i].v) {
                    snd_stop_all_riders();
                    rumble_stop_all();
                    G_6.f80 = 1800;
                    G_6.f58 |= 0x800;
                    break;
                }
            }
            G_6.f2c++;
        }
        if (G_6.f40 == 0) {
            G_6.f28 = 180;
        } else if (G_6.f28 != 0) {
            G_6.f28--;
        }
    }
    if ((G_6.f70 == 0 || G_6.f70 == 2) && G_6.f78 != 0) {
        G_6.f78--;
    }
    if (G_6.state < 5) {
        tutorial_check_lesson_skip();
        return;
    }
    switch (G_6.state) {
    case 13:
        switch (G_6.f70) {
        case 0:
            if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
                G_6.f70 = 2;
                G_6.f78 = 59 - G_6.f78;
            } else if (G_6.f78 == 0) {
                G_6.f70 = 1;
            }
            break;
        case 1:
            if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
                G_6.f70 = 2;
                G_6.f78 = 60;
                G_6.f58 = (G_6.f58 & ~0x10) | 0x40;
            } else {
                tutorial_check_lesson_skip();
            }
            break;
        case 2:
            if (G_6.f78 == 0) {
                G_6.state = 14;
                G_6.f26 = 0x3F;
                G_6.f70 = 0;
                G_6.f78 = 60;
            }
            break;
        }
        break;
    case 14:
        switch (G_6.f70) {
        case 0:
            if (G_6.f78 == 0) {
                G_6.f70 = 1;
                return;
            }
            break;
        case 1:
            if ((gPadInput & 0x40) && !(gPadInputPrev & 0x40)) {
                G_6.f70 = 6;
                gRacePhase = 6;
                gRacePhaseTimer = -1;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
                return;
            }
            if (gRiders[0].segment == TB.f1A) {
                if (gRiders[0].flags & 0x10) {
                    if (G_6.f58 & 0x2000) {
                        bikes_free();
                        decal_clear_all();
                        mem_fill((u8 *)gRiders, 0, sizeof(Rider) * 8);
                        tutorial_init_lesson(0);
                        bikes_init();
                        replay_play_start(gActiveReplay);
                        G_6.f58 = (G_6.f58 | 0x80) & ~0x2000;
                    }
                } else {
                    gRiders[0].flags |= 0x10;
                    G_6.f58 &= ~0x2000;
                }
                return;
            }
            if ((TB.fl & 4) && gRiders[1].segment >= TB.f1A &&
                (TB.f18 < TB.f1A || (gRiders[1].segment < TB.f18 - 2 && TB.f1A < TB.f18))) {
                gRiders[1].flags |= 0x10;
                return;
            }
            break;
        case 5:
            if (gRiders[0].segment == TB.f1A) {
                gRiders[0].flags |= 0x10;
            }
            if ((G_6.f58 & 0x2000) && gPauseMenu != 5) {
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
            if (G_6.f2c >= 3 && D_800F0738 == 0) {
                D_800F0738 = 2;
            }
            if (gRiders[0].unk_3FB == 0 && gRiders[0].trick == TB.list[D_800F0738].v) {
                D_800F0738++;
            }
            if (D_800F0738 >= 10) {
                if (gEventFlags & 0x2000) {
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
                            gGame &= ~4;
                            return;
                        }
                    }
                    if (D_800F0738 < 10) {
                        D_800F0738 = 10;
                    }
                } else if (!(gRiders[0].flags & 0x10)) {
                    gRiders[0].flags |= 0x10;
                }
            }
        }
        tutorial_check_lesson_skip();
        if (gRiders[0].segment == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            if (event_request_exit() != 0) {
                D_800F0618 = 6;
                gActiveReplay = 0;
                gDemoPlayback = 0;
                sDemoActive = 0;
                gReplayRecording = 0;
            }
            return;
        }
        if ((u32)(gRiders[0].segment - TB.f1A) < 10 || (G_6.f58 & 0x2000)) {
            if (gRiders[0].flags & 0x10) {
                if (gSeason_5->f64 < 16 && TB.fD != gTutorialLessons_9[gSeason_5->f64 + 1].fD &&
                    tutorial_lesson_passed_2()) {
                    if (TB.fD == gSeason_5->f0[1]) {
                        gSeason_5->f0[1] = TB.fD + 1;
                        D_800F1BC1 = D_800CAAA1;
                    }
                }
                if ((G_6.f58 & 0x2000) && D_800F0738 == 0) {
                    snd_stop_all_riders();
                    G_6.f58 &= ~0x2000;
                    gTutorialResult = tutorial_lesson_passed_2() ? 1 : -1;
                    gPauseMenu = 5;
                    gPauseMenuCursor = tutorial_lesson_passed_2() ? 3 : 0;
                }
            } else {
                gRiders[0].flags |= 0x10;
            }
        } else if (gRiders[0].segment == SEG(SEG(TB.f18)->f4A)->f4A && !(TB.fl & 2)) {
            G_6.f70 = 6;
            gRacePhase = 6;
            gRacePhaseTimer = -1;
            gActiveReplay = 0;
            gDemoPlayback = 0;
            sDemoActive = 0;
            gReplayRecording = 0;
        } else if ((TB.fl & 4) && gRiders[1].segment >= TB.f1A &&
                   (TB.f18 < TB.f1A || (gRiders[1].segment < TB.f18 - 2 && TB.f1A < TB.f18))) {
            gRiders[1].flags |= 0x10;
        }
        break;
    }
}

typedef struct {
    s32 state;      /* 0x00 */
    V7 v;           /* 0x04 */
    s16 f20;        /* 0x20 */
    s16 f22;        /* 0x22 */
    s16 f24;        /* 0x24 */
    s16 f26;        /* 0x26 */
    s16 f28;        /* 0x28 */
    s16 pad2A;
    s32 f2C;        /* 0x2C */
    s32 f30[3];     /* 0x30 */
    s32 f3C;        /* 0x3C */
    s32 f40;        /* 0x40 */
    s32 f44;        /* 0x44 */
    s32 f48;        /* 0x48 */
    s32 f4C;        /* 0x4C */
    s32 f50;
    s32 f54;
    s32 f58;        /* 0x58 */
    s32 pad5C[3];
    s16 f68;
    s16 f6A;
    s16 f6C;
    u8 f6E;
    u8 pad6F;
    s32 pad70[6];
    s32 f88;
} Game_19;

typedef struct {
    s32 flags;      /* 0x00 */
    s32 pad4[14];
    s32 f3C;        /* 0x3C */
    s32 pad40[3];
    s32 mode;       /* 0x4C */
} Cfg_8;

typedef struct {
    u8 pad[0x34];
    V7 v;           /* 0x34 */
    s32 f50;        /* 0x50 */
    u8 pad54[0xA0 - 0x54];
} Track_12;

typedef struct {
    Ent24 *p;
    u8 pad4[0x62];
    u8 b66[1];
} Race_3;

typedef struct { s32 n; s32 f4; s32 *p; } Alloc;

extern Game_19 gEvent_20;
extern Cfg_8 gGame_36;
extern Track_12 *gCurTrack_28;
extern Race_3 *gSeason_9;
extern Track_12 gTracks_10[];
extern Alloc D_800CA924_2;

void event_init(s32 a0, s32 a1)
{
    Rider *b;
    s32 i;
    s32 j;

    sRaceEndFrame = -1;
    mem_fill_2(gEvent_20.f30, -1, 12);
    if (gGame_36.mode == 1) {
        for (i = 0; i < gEvent_20.f22; i++) {
            gSeason_9->b66[i] = 0;
        }
    }
    gRaceExitCode = 6;
    *(V7 *)((u8 *)&gEvent_20 + 4) = gCurTrack_28->v;
    gCurTrack_28->f50 &= ~0x180;
    gEvent_20.f88 = -1;
    gEvent_20.f58 = 0;
    gEvent_20.f40 = 0;
    gEvent_20.f28 = 0;
    gEvent_20.f44 = 0;
    gEvent_20.f4C = 0;
    gEvent_20.f22 = a0;
    gEvent_20.f20 = a1;
    if (gGame_36.mode == 1 && !(gGame_36.flags & 0x81) &&
        gSeason_9->p - D_800CA9C8 == 4) {
        gEvent_20.f58 = 0x200;
        gEvent_20.f22 = 2;
        gNumRiders = 2;
    }
    gEvent_20.f26 = 0x96;
    if (gCreditsMode != 0 || (gGame_36.flags & 0x81) ||
        gGame_36.mode == 0xD || gGame_36.mode == 0xE) {
        gEvent_20.state = 5;
    } else {
        if ((gGame_36.mode == 0 || gGame_36.mode == 1) &&
            gCurTrack_28 - gTracks_10 != 9) {
            gEvent_20.state = 0;
            gEvent_20.f26 = 0x100;
        } else {
            gEvent_20.state = 1;
            gEvent_20.f26 = 0x96;
        }
        gEvent_20.f58 |= 0x10;
    }
    gEvent_20.f2C = 0;
    gEvent_20.f3C = (gGame_36.flags & 0x81) != 0;
    mem_fill_2(gRiders, 0, 0x3F00);
    gGame_36.flags &= ~4;
    if (D_800CA924_2.n > 0 && D_800CA924_2.f4 == 0) {
        D_800CA924_2.p = mem_calloc_dbg(D_800CA924_2.n * 4, 0, (s32)D_800D7340, 0x6D9);
    }
    switch (gGame_36.mode) {
    case 2:
        gEvent_20.f26 = 0xB4;
        gEvent_20.state = 6;
        gEvent_20.f6E = 0;
        gEvent_20.f68 = 0;
        gEvent_20.f6A = 0;
        gEvent_20.f6C = 0;
        gEvent_20.f58 |= 0x10;
        break;
    case 5:
        gEvent_20.f58 |= 0x10;
        race_reset_grid_order();
        break;
    case 7:
        gEvent_20.f48 = -1;
        gEvent_20.f58 |= 0x10;
        race_reset_grid_order();
        for (b = gRiders; b < &gRiders[gGame_36.f3C]; b++) {
            b->timeLimit = 0x1C20;
        }
        break;
    case 9:
        tutorial_init_lesson(1);
        break;
    case 10:
        gEvent_20.f2C = 0x1C20;
        gEvent_20.f58 |= 0x10;
        race_reset_grid_order();
        break;
    case 0xD:
    case 0xE:
        break;
    default:
        if (gEvent_20.state == 8) {
            race_set_grid_order(2, 0);
        }
        gEvent_20.f24 = 0;
        for (j = 0; j < gEvent_20.f22; j++) {
            /* the race fields of rider j, as an offset from rider 0's */
            ((RiderRace *)((u8 *)&gRiders[0].race + j * sizeof(Rider)))->finishPlace = -1;
        }
        break;
    }
}

#define S (&gEvent_21)

typedef struct { s32 f0; s32 arr[4]; u8 b[4][3]; u8 pad20[0xC]; s32 val; } SS;

extern SS gEvent_21;

s32 event_check_record_time(Rider *p)
{
    u8 ret = 0;
    s32 i;
    s32 j;
    s32 *ch;
    u8 *src;

    if (p->isCpu != 0) goto done;
    if (gReplayPlayback != 0) goto done;
    if (gGameMode == 1 && gSeason_4 != 0 && ((*gSeason_4 - (s32)D_800CA9C8) * 0x38E38E39 >> 2) == 5) goto done;
    ch = gCurTrack_3;
    if (ch[0x14] & 2) goto done;
    if (S->val >= S->arr[2]) goto done;
    if (gGame_4[19] == 8 && p != &gRiders[D_800C8CE0]) goto done;
    if (gGame_4[16] != *(s16 *)((u8 *)ch + 0x1E)) goto done;

    ret = S->val < S->arr[0];
    for (i = 0; i < 3; i++) {
        if (S->val <= S->arr[i]) {
            for (j = 1; j >= i; j--) {
                S->arr[j + 1] = S->arr[j];
                S->b[j + 1][0] = S->b[j][0];
                S->b[j + 1][1] = S->b[j][1];
                S->b[j + 1][2] = S->b[j][2];
            }
            break;
        }
    }
    S->arr[i] = S->val;
    S->b[i][0] = 0xFF;
    S->b[i][1] = 0xFF;
    S->b[i][2] = (gGameMode == 8) ? 0 : (u8)(p - gRiders);
    gCurTrack_3[0x14] |= 0x100;
    if (ret) announce_record(p - gRiders);
done:
    return ret;
}

u32 event_rider_finished(s32 p0)
{
    s32 p = p0;
    u8 *s0;
    u8 *s2;
    u8 *v;
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
    s0 = (u8 *)&gRiders[0].race + a1;
    rv = *(s16 *)(s0 + 8);
    if (rv >= 0) {
        return rv;
    }
    v = gEvent_2;
    *(u8 *)(v + *(s16 *)(v + 0x24) + 0x50) = p;
    a0 = *(u16 *)(v + 0x24);
    *(u16 *)(v + 0x24) = a0 + 1;
    *(u16 *)(s0 + 8) = a0;
    *(s32 *)(s0 + 0x2C) = *(s32 *)(v + 0x2C);
    if (*(u8 *)((u8 *)&gRiders[0].race + a1 + 0x88) == 0 && gReplayPlayback == 0) {
        if ((gGameMode != 8 || p == D_800C8CE0) && gNumScreens == 1) {
            func_8001106C();
        }
    }
    s2 = (u8 *)gRiders + ((((p << 6) - p)) << 5);
    t1 = event_check_record_time((Rider *)s2);
    if ((*(s32 *)gGame_2 & 0x8000) == 0) {
        a2 = *(s32 *)(gGame_2 + 0x4C);
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
            a1 = D_800F05CC;
            if (a1 != a2) {
                goto L19c;
            }
            q = *(u8 **)gSeason + (*(s32 *)(gGame_2 + 0x410) - 1);
            q[4] = q[4] | (a1 << *(s16 *)(gSeason + 0x64));
        }
    }
L19c:
    {
        u8 *bse = (u8 *)gRiders;
        w = bse + (((p << 6) - p) << 5);
    }
    if (w[0x6E8] == 0) {
        u8 *vb = gEvent_2;
        if (*(s16 *)(vb + 0x24) == 1) {
            *(s32 *)(vb + 0x44) = p;
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
        u8 *bse2 = (u8 *)gRiders;
        w2 = bse2 + (((p << 6) - p) << 5);
    }
    w2[0x3FF] = 1;
    *(f32 *)(w2 + 0x42C) = *(f32 *)(w2 + 0x638);
    return t1 & 0xff;
}

typedef struct Blk_14 {
    u8 pad00[0x10];
    s32 best;         /* 0x10 */
    u8 pad14[0x1D - 0x14];
    u8 f1D;
    u8 f1E;
    u8 f1F;
    s16 nlaps;        /* 0x20 */
    s16 f22;          /* 0x22 */
    u8 pad24[0x2C - 0x24];
    s32 f2C;          /* 0x2C */
    s16 arr[1];       /* 0x30 */
} Blk_14;

typedef struct Ctl_11 {
    u32 flags;
    u8 pad04[0x3C - 4];
    s32 n;            /* 0x3C */
} Ctl_11;

typedef struct G64 {
    u8 pad[0x50];
    u32 f50;
} G64;

extern G64 *gCurTrack_29;
extern Ctl_11 gGame_37;
extern Blk_14 gEvent_22;

void event_rider_lap_completed(s32 p)
{
    s32 st = 0;
    RiderRace *b;
    s32 t;
    s32 i;
    s16 v;

    switch (gGameMode) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 9:
    case 13:
    case 14:
        return;
    }
    b = &gRiders[p].race;
    if (b->lap < gEvent_22.nlaps) {
        if (gRiders[p].isCpu == 0 && gReplayPlayback == 0 && gGameMode != 7 && gGameMode != 5 &&
            !(gCurTrack_29->f50 & 2) && (t = b->lapTimes[b->lap]) < gEvent_22.best &&
            (gGameMode != 8 || p == D_800C8CE0)) {
            gEvent_22.best = t;
            gEvent_22.f1D = 0xFF;
            gEvent_22.f1E = 0xFF;
            gEvent_22.f1F = (gGameMode == 8) ? 0 : p;
            st = 3;
            gCurTrack_29->f50 |= 0x80;
        }
        if (gGame_37.flags & 0x81) {
            goto end;
        }
        v = gEvent_22.arr[b->lap];
        if (v == -1) {
            gEvent_22.arr[b->lap] = ((u16 *)&gEvent_22.f2C)[1];
        } else {
            b->gap = v - gEvent_22.f2C;
            b->gapMsgTimer = 0x78;
            if (b->place == 1) {
                for (i = 0; i < gGame_37.n; i++) {
                    if (b->lap < gRiders[i].race.lap + gRiders[i].lapAdjust) {
                        gRiders[i].race.gapMsgTimer = 0x78;
                        gRiders[i].race.gap = -b->gap;
                        break;
                    }
                }
            }
        }
        b->lapMsgTimer = 0x78;
        ((volatile RiderRace *)b)->lap++;
        if (b->lap != gEvent_22.nlaps) {
            if (st != 3) {
                if (gGameMode == 8 && gRiderAlpha[p] == 0xFF) {
                    st = 1;
                } else if (b->place == 0) {
                    st = 2;
                } else if (gRiders[p].isCpu == 0 && b->gap < -300) {
                    st = 1;
                }
            }
        } else if ((u8)event_rider_finished(p)) {
            if (st != 3) {
                if (b->finishPlace == 0 && gEvent_22.f22 >= 2 && st == 0) {
                    st = 4;
                } else if (gRiders[p].isCpu == 0) {
                    st = 5;
                }
            }
        } else if (p < gNumPlayers) {
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
            announce_lap(b->lap - 1);
        }
        break;
    case 2:
        announce_leader_lap(p, b->lap - 1);
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

typedef struct {
    s32 state;
    u8 pad4[0x26 - 4];
    s16 timer;
    u8 pad28[0x58 - 0x28];
    s32 flags;
} Ctl_12;

extern Ctl_12 gEvent_23;

void event_update_countdown(void)
{
    if (gEvent_23.timer != 0 && --gEvent_23.timer == 0) {
        switch (gEvent_23.state) {
        case 0:
            gEvent_23.timer = 150;
            gEvent_23.state = 1;
            break;
        case 1:
            gEvent_23.timer = 36;
            gEvent_23.state = 2;
            tunes_play_speech(0x6F, 5, 1);
            break;
        case 2:
            gEvent_23.timer = 36;
            gEvent_23.state = 3;
            tunes_play_speech(0x6D, 6, 1);
            break;
        case 3:
            gEvent_23.timer = 36;
            gEvent_23.state = 4;
            tunes_play_speech(0x6E, 7, 1);
            break;
        case 4:
            gEvent_23.timer = 15;
            gEvent_23.state = 5;
            gEvent_23.flags &= ~0x10;
            tunes_play_speech(0x70, 8, 1);
            break;
        }
    }
}

typedef struct {
    u8 pad0[0x22];
    s16 count;
    u8 pad24[0x2C - 0x24];
    s32 tick;
    u8 pad30[0x40 - 0x30];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 flags;
} Game_20;

extern Game_20 gEvent_24;

void event_update_hillclimb(void)
{
    Game_20 *g;
    Game_20 *g2;
    Rider *r;
    u8 flag;

    event_update_countdown();
    g = &gEvent_24;
    if (!(g->flags & 0x10) && g->f40 == 0) {
        g->tick++;
    }
    flag = 1;
    for (r = gRiders; r < &gRiders[gEvent_24.count]; r++) {
        if (r->race.lapMsgTimer != 0) {
            r->race.lapMsgTimer--;
            r->timeLimit += 30;
        }
        if (gEvent_24.tick == r->timeLimit && r->finishState == 0) {
            r->finishState = 1;
            r->race.finishPlace = 5;
            r->isCpu = 1;
        }
        if (r->finishState == 0) flag = 0;
    }
    gEventAllFinished = flag;
    if (flag) {
        if (((gPlayerInputs_2 & 0x40) && !(gPlayerInputsPrev & 0x40)) ||
            ((gPlayerInputs_2 & 0x10) && !(gPlayerInputsPrev & 0x10))) {
            event_request_exit();
        }
    }
}

typedef struct {
    s32 pad0;
    s32 key[3];
    u8 pad10[4];
    u8 tri[3][3];
    u8 pad1D[0x2C - 0x1D];
    s32 timer;
    u8 pad30[0x10];
    s32 done;
    u8 pad44[0x14];
    s32 flags;
} Rank;

typedef struct {
    u8 pad0[0x3C];
    s32 n;
    u8 pad40[0xC];
    s32 mode;
} Game_21;

typedef struct {
    u8 pad[0x50];
    s32 flags;
} Hud;

extern Game_21 gGame_38;
extern Hud *gCurTrack_30;
extern Rank gEvent_25;

void event_update_stunt(void)
{
    Rank *q;
    Game_21 *g;
    Rider *e;
    s32 i, j, k;

    event_update_countdown();
    if (gEvent_25.flags & 0x10) return;
    if (--gEvent_25.timer > 0) return;
    if (((gPlayerInputs_2 & 0x40) && !(gPlayerInputsPrev & 0x40)) ||
        ((gPlayerInputs_2 & 0x10) && !(gPlayerInputsPrev & 0x10))) {
        if (gRacePhase == 5) {
            gRacePhase = 6;
            gRacePhaseTimer = 0x14;
        }
    }
    gEvent_25.timer = 0;
    if (gEvent_25.done) return;
    for (i = 0; i < gGame_38.n; i++) {
        e = &gRiders[i];
        if (e->isCpu) continue;
        if (gReplayPlayback) continue;
        if (gGame_38.mode == 1 && gSeason_10 != NULL && *gSeason_10 - D_800CA9C8 == 5) continue;
        if (e->raceKey > gEvent_25.key[2]) {
            for (j = 0; j < 3; j++) {
                if (e->raceKey >= gEvent_25.key[j]) {
                    for (k = 1; k >= j; k--) {
                        gEvent_25.key[k + 1] = gEvent_25.key[k];
                        gEvent_25.tri[k + 1][0] = gEvent_25.tri[k][0];
                        gEvent_25.tri[k + 1][1] = gEvent_25.tri[k][1];
                        gEvent_25.tri[k + 1][2] = gEvent_25.tri[k][2];
                    }
                    break;
                }
            }
            gEvent_25.key[j] = e->raceKey;
            gEvent_25.tri[j][2] = e - gRiders;
            gEvent_25.tri[j][0] = 0xFF;
            gEvent_25.tri[j][1] = 0xFF;
            gCurTrack_30->flags |= 0x100;
        }
    }
    gEvent_25.done = 1;
    for (j = 0; j < gGame_38.n; j++) {
        gRiders[j].isCpu = 1;
    }
    gRiders[0].finishState = 1;
    *(volatile s16 *)&gRiders[0].race.finishPlace = 1;
    if (gNumPlayers == 2) {
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

#define PRESSED(i, b) ((gPlayerInputs[i] & (b)) && !(gPlayerInputsPrev_3[i] & (b)))

typedef struct {
    s32 state;
    u8 p04[0x26 - 0x04];
    s16 t26;
    s16 t28;
    u8 p2A[2];
    s32 t2C;
    u8 p30[0x40 - 0x30];
    s32 f40;
    u8 p44[0x58 - 0x44];
    s32 flags;
    u8 p5C[0x68 - 0x5C];
    s16 c68;
    s16 c6A;
    s16 t6C;
    s8 f6E;
} Game_22;

typedef struct {
    u8 p00[0x3C];
    s32 nplayers;
} Ctl_13;

extern Game_22 gEvent_26;
extern Ctl_13 gGame_39;

void event_update_soccer(void)
{
    s32 i;
    s32 j;

    if (gEvent_26.f40 == 0) {
        gEvent_26.t28 = 180;
    } else if (gEvent_26.t28 != 0) {
        gEvent_26.t28--;
    }
    if (gEvent_26.t6C != 0) {
        if (--gEvent_26.t6C == 0) {
            for (i = 0; i < gGame_39.nplayers; i++) {
                bike_respawn(&gRiders[i]);
            }
            soccer_ball_reset(0);
            gEvent_26.flags &= ~0x1F;
        }
        return;
    }
    if (gEvent_26.state == 7 || gEvent_26.state == 9 || gEvent_26.state == 11) {
        if (!(gEvent_26.flags & 0xF)) {
            if (gSoccerBall.pos.x >= -24.0f && gSoccerBall.pos.x < 23.0f && gSoccerBall.pos.z >= 79.0f && gSoccerBall.pos.z < 87.0f &&
                gSoccerBall.pos.y >= 0.0f && gSoccerBall.pos.y < 6.2f) {
                gEvent_26.t6C = 180;
                gEvent_26.c68++;
                announce_goal();
                gRiders[0].award = 1;
                gEvent_26.flags |= 0x11;
                if ((gEvent_26.f6E >> ((gNumPlayers - 1) >> 1)) == 1) {
                    gEvent_26.flags |= 0x20;
                } else {
                    gEvent_26.flags &= ~0x20;
                }
            } else if (gSoccerBall.pos.x >= -24.0f && gSoccerBall.pos.x < 23.0f && gSoccerBall.pos.z >= -80.0f && gSoccerBall.pos.z < -73.0f &&
                       gSoccerBall.pos.y >= 0.0f && gSoccerBall.pos.y < 6.2f) {
                gEvent_26.t6C = 180;
                gEvent_26.c6A++;
                announce_goal();
                gRiders[1].award = 1;
                gEvent_26.flags |= 0x12;
                if ((gEvent_26.f6E >> ((gNumPlayers - 1) >> 1)) == 0) {
                    gEvent_26.flags |= 0x20;
                } else {
                    gEvent_26.flags &= ~0x20;
                }
            }
        }
    }
    switch (gEvent_26.state) {
    case 6:
        if (--gEvent_26.t26 == 0) {
            gEvent_26.state = 7;
            gEvent_26.t2C = 0x2A30;
            gEvent_26.flags &= ~0x10;
        }
        break;
    case 7:
        if (--gEvent_26.t2C == 0) {
            j = 0;
            gEvent_26.state = 8;
            gEvent_26.t26 = 240;
            gEvent_26.flags |= 0x10;
            for (; j < gGame_39.nplayers; j++) {
                bike_respawn(&gRiders[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 8:
        if (--gEvent_26.t26 == 0) {
            gEvent_26.state = 9;
            gEvent_26.t2C = 0x2A30;
            gEvent_26.flags &= ~0x10;
        }
        break;
    case 9:
        if (--gEvent_26.t2C == 0) {
            gEvent_26.flags |= 0x10;
            if (gEvent_26.c68 == gEvent_26.c6A) {
                gEvent_26.state = 10;
                gEvent_26.t26 = 240;
            } else {
                gEvent_26.state = 12;
                gEvent_26.t26 = 600;
                gEvent_26.f40 = 1;
            }
            for (i = 0; i < gGame_39.nplayers; i++) {
                bike_respawn(&gRiders[i]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 10:
        if (--gEvent_26.t26 == 0) {
            gEvent_26.state = 11;
            gEvent_26.t2C = 0x1C20;
            gEvent_26.flags &= ~0x10;
        }
        break;
    case 11:
        if (--gEvent_26.t2C == 0) {
            j = 0;
            gEvent_26.state = 12;
            gEvent_26.f40 = 1;
            gEvent_26.t26 = 600;
            gEvent_26.flags |= 0x10;
            for (; j < gGame_39.nplayers; j++) {
                bike_respawn(&gRiders[j]);
            }
            soccer_ball_reset(0);
        }
        break;
    case 12:
        if (PRESSED(0, 0x40) || PRESSED(1, 0x40) || PRESSED(2, 0x40) || PRESSED(3, 0x40) ||
            (D_800F05D0 == 0 &&
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
    s32 f0;
    u8 pad4[0x1C];
    s16 f20;
    s16 f22;
    s16 f24;
    s16 pad26;
    s16 f28;
    s16 pad2A;
    s32 f2C;
    u8 pad30[0x10];
    s32 f40;
    u8 pad44[0xC];
    s8 f50[8];
    s32 f58;
} Game_23;

typedef struct {
    u8 pad0[4];
    s16 f4[6];
    s16 f10[42];
    s16 f64;
    u8 f66[5];
    u8 f6B;
    u8 f6C[8];
} Race_4;

extern Game_23 gEvent_27;
extern Race_4 *gSeason_11;

void event_update_race(void) {
    s32 i, j, k, m;
    s32 cnt, n1;
    s32 pad[8];
    u8 buf[4];
    f32 f;
    s32 s, t;
    u8 tmp;
    Rider *p;
    RiderRace *q;

    event_update_countdown();
    n1 = 0;
    cnt = n1;
    if (!(gEvent_27.f58 & 0x10)) {
        gEvent_27.f2C++;
    }
    *(s32 *)buf = 0;
    for (m = 0; m < gEvent_27.f22; m++) {
        q = &gRiders[m].race;
        if ((gRiders[m].flags & 8) || (gGhostRace != 0 && gGhostSlot == m) || gRiders[m].isCpu == 0) {
            cnt++;
        }
        if (gRiders[m].race.finishPlace == -1 && m < cnt) {
            n1++;
        }
        if (q->finishPlace != -1 || gRiders[m].isCpu != 0) {
            buf[m] = 1;
        }
        if (gEvent_27.f0 == 5) {
            s32 k = q->lap;
            if (k < gEvent_27.f20) {
                q->lapTimes[k]++;
            }
        }
    }
    for (m = 0; m < cnt; m++) {
        if (!buf[m]) break;
    }
    if (!((m == cnt && n1 == 0 && gGame_8.f4C != 8) || (gGame_8.f4C == 8 && buf[D_800C8CE0]))) return;
    if (gGame_8.f0 & 0x81) return;
    if (gRaceExitCode == 3) return;
    if (gEvent_27.f40 == 0) {
        gEvent_27.f28 = 180;
    } else if (gEvent_27.f28 != 0) {
        gEvent_27.f28--;
    }
    gEventAllFinished = 1;
    gRaceExitCode = 2;
    if (sRaceEndFrame == -1) {
        sRaceEndFrame = gRaceFrame;
    }
    if (gReplayPlayback == 0 && gRacePhase == 5) {
        if ((gPadInput_2[0] & 0x40) && !(gPadInputPrev_2[0] & 0x40)) goto go;
        if ((gPadInput_2[1] & 0x40) && !(gPadInputPrev_2[1] & 0x40)) goto go;
        if ((gPadInput_2[2] & 0x40) && !(gPadInputPrev_2[2] & 0x40)) goto go;
        if ((gPadInput_2[3] & 0x40) && !(gPadInputPrev_2[3] & 0x40)) goto go;
        if (gCreditsMode != 0 && (gGame_8.f0 & 0x01000000) && (gPadInput_2[0] & 0x20) && !(gPadInputPrev_2[0] & 0x20)) goto go;
        if (D_800F05D0 != 0 && gCreditsMode == 0) goto end;
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
        for (k = 0; k < gEvent_27.f22; k++) {
            if (gRiders[k].race.finishPlace == -1) {
                f = gRiders[k].distToGo / (gPathLapLength * gEvent_27.f20);
                if (f <= 0.1f) {
                    f = 0.1f;
                } else if (f > 1.0f) {
                    f = 1.0f;
                }
                f = gEvent_27.f2C / f;
                gRiders[k].race.finishTime = f;
            }
        }
        for (k = 0; k < gEvent_27.f22; k++) {
            if (gRiders[k].race.finishPlace == -1) {
                gRiders[k].race.finishPlace = 0;
                for (j = 0; j < gEvent_27.f22; j++) {
                    if ((j != k && k < j && gRiders[j].race.finishTime < gRiders[k].race.finishTime) ||
                        (j < k && gRiders[j].race.finishTime <= gRiders[k].race.finishTime)) {
                        gRiders[k].race.finishPlace++;
                    }
                }
                if (gGame_8.f4C == 1) {
                    gSeason_11->f66[k] += gRacePoints_2[gRiders[k].race.finishPlace].b3;
                }
            }
            gEvent_27.f50[gRiders[k].race.finishPlace] = k;
        }
        gEvent_27.f24 = gEvent_27.f22;
        if (gGame_8.f4C == 1) {
            for (i = 0; i < gEvent_27.f22; i++) {
                gSeason_11->f4[i] += gSeason_11->f66[i];
                gSeason_11->f6C[i] = i;
            }
            for (i = 0; i < gEvent_27.f22; i++) {
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
    if (gCurGameState->f10 == 3) {
        s = -1;
        for (i = gEvent_27.f24; i < gEvent_27.f22; i++) {
            q = &gRiderStandings[i]->race;
            if (q->finishPlace == s) {
                gEvent_27.f50[gEvent_27.f24] = i;
                q->finishPlace = gEvent_27.f24++;
                q->finishTime = (gEvent_27.f2C * gRiderStandings[0]->distToGo) / gRiders[i].distToGo;
                if (gGame_8.f4C == 1 + (((u32)&gRiders & 1) >> 1)) {
                    gSeason_11->f10[(gRiderStandings[i] - gRiders) * 7 + gSeason_11->f64] = q->finishPlace;
                    gSeason_11->f66[gRiderStandings[i] - gRiders] = ((Pts *)((u32)gRacePoints_2 | (((u32)gRacePoints_2 & 1) >> 1)))[q->finishPlace].b3;
                }
            }
        }
    }
}

typedef struct {
    s32 w00;
    u8 p04[0x3C - 0x04];
    s32 w3C;
    u8 p40[0x4C - 0x40];
    u32 w4C;
} Ctl2;

typedef struct {
    u8 p00[0x24];
    s16 w24;
    u8 p26[0x2C - 0x26];
    s32 w2C;
    u8 p30[0x40 - 0x30];
    s32 w40;
    u8 p44[0x4C - 0x44];
    s32 w4C;
    u8 p50;
    u8 p51[0x58 - 0x51];
    s32 w58;
    u8 p5C[0x88 - 0x5C];
    s32 w88;
} Ctl_14;

extern Ctl2 gGame_40;
extern Ctl_14 gEvent_28;

void event_update(s32 arg0)
{
    s32 i;
    s32 *p;

    if ((gGame_40.w00 & 0x80) == 0) {
        s32 bits;
        i = 0;
        bits = gContMask;
        for (p = gPlayerInputs; i < 4; i++, p++) {
            if (((bits >> i) & 1) != 0 && *p != 0) break;
        }
        if (i == 4) {
            gEvent_28.w4C = gEvent_28.w4C + 1;
        } else {
            gEvent_28.w4C = 0;
        }
    }
    switch (gGame_40.w4C) {
    case 2:
        event_update_soccer();
        break;
    case 5:
        event_update_countdown();
        if (gEvent_28.w40 == 0) {
            if ((gEvent_28.w58 & 0x10) != 0) break;
            gEvent_28.w2C = gEvent_28.w2C + 1;
            if (gEvent_28.w2C == 0x34bc0) {
                event_rider_finished(0);
                gEvent_28.w40 = 1;
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
    if (gGame_40.w4C < 2 && gEvent_28.w24 > 0 && gEvent_28.p50 < gGame_40.w3C &&
        gEvent_28.w88 != -2 && (gGame_40.w3C == 1 || gGame_40.w4C == 8)) {
        gEvent_28.w88 = gEvent_28.w88 + 1;
        if (gGame_40.w3C >= 3) {
            if (gEvent_28.w88 >= 100) goto store;
        } else if (gEvent_28.w88 >= 0x8c) {
            goto store;
        }
        return;
store:
        gEvent_28.w88 = -2;
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
    u8 *s2;

    if (gGameMode == 9) {
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
    s2 = gGame_2;
    if (1 < *(s32 *)(s2 + 0x3C)) {
        sTempGaugeTextures = res_load(D_800E7728, D_800D11B0);
    }
    if (*(s32 *)(s2 + 0x4C) == 9) {
        *(s32 *)(s2 + 0x730) = res_load(D_800E7728, D_800D09F8);
        D_800F0740 = res_load(D_800E7728, D_800D10E8);
        D_800EFD6C = res_load(D_800E7728, D_800D0DEC);
        D_800EFD68 = res_load(D_800E7728, D_800D09D4);
    }
    res_register_type(D_800D8424, prim_n64_relocate);
    if (*(s32 *)(s2 + 0x3C) == 1) {
        D_800F0730[0] = mem_alloc_dbg(0x1c0, 0, D_800D7340, 0xB6B);
        D_800F0730[1] = mem_alloc_dbg(0x1c0, 0, D_800D7340, 0xB6C);
    }
    t13 = *(volatile s32 *)&gScreenHeight;
    guPerspective(auStack_50, sHudPerspNorm, 33.0f, 320.0f / (f32)t13, 50.0f, 4192.0f, 1.0f);
    iVar3 = 0;
    if (*(s32 *)(s2 + 0x4C) == 2) {
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

    if (gEventNumRiders >= 2) {
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
    fa = gRiders[p3].speed;
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
        v = gRiders[pl].speed;
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
        event_format_time(gRiders[pl].timeLimit - gEventTime, buf);
        font_text_add(gHudTextObjs[4], "Time:");
        font_draw_text(gHudTextObjs[4], 0x23, gViMode == 2 ? 0x180 : 0xD6);
        font_text_add(gHudTextObjs[5], buf);
        font_draw_text(gHudTextObjs[5], 100, gViMode == 2 ? 0x180 : 0xD6);
    } else {
        font_text_create(gHudTextObjs[4], 0x40, 2, 2, 0x20);
        font_text_create(gHudTextObjs[5], 0x40, 2, 4, 0x20);
        t = -gEventTime + gRiders[pl].timeLimit;
        if (gRiders[pl].finishState) {
            t = gRiders[pl].race.finishTime;
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
        v = gRiders[pl].speed;
        int_to_str((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        font_text_add(gHudTextObjs[6], buf);
        font_draw_text(gHudTextObjs[6],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] - 0x2D,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] - 0xB);
        gdl = event_draw_temp_gauge(gdl, a1, a2, pl);
    }
    if (gEvent_5.h26 != 0 && gEvent_5.w0 != 0 && (gEvent_5.w0 != 1 || gEvent_5.h26 < 0x5B)) {
        gdl = event_draw_countdown(gdl, pl);
    }
    if (gRiders[pl].race.lapMsgTimer != 0 && gPauseMenu == 0) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, 0x21);
        font_text_add(gHudTextObjs[7], "Stage Clear!");
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] / 2,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] / 2);
    }
    if (gRiders[pl].finishState) {
        font_text_create(gHudTextObjs[7], 0x40, 1, 4, 0x21);
        if (gEventAllFinished == 0 || pl != 0) {
            font_text_add(gHudTextObjs[7], "Game Over");
        } else {
            font_text_add(gHudTextObjs[7], D_800D84A8);
        }
        font_draw_text(gHudTextObjs[7],
                      gViewRects_5[gNumScreens - 1][pl][0] + gViewRects_5[gNumScreens - 1][pl][2] / 2,
                      gViewRects_5[gNumScreens - 1][pl][1] + gViewRects_5[gNumScreens - 1][pl][3] / 2);
    }
    if (gGame & 0x20000000) {
        s32 *p1;
        s32 *p2;
        s32 *p3;
        p1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, 0x10);
        debug_ftoa(buf, gRiders[0].bike.pos.x, 5);
        font_text_add(p1, buf);
        font_draw_text(p1, 0x28, 0x1E);
        p2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
        debug_ftoa(buf, gRiders[0].bike.pos.z, 5);
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

extern Txt *gHudTextObjs_3[];
extern Trick gRiderAnims_11[];

Gfx *event_draw_hud_stunt(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    Rider *p = &gRiders[pl];
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
    if (gNumPlayers >= 2) {
        if (gEventAllFinished != 0) {
            font_text_create(gHudTextObjs_3[7], 0x40, 1, 4, 0x21);
            switch (p->race.finishPlace) {
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
        event_format_time(gEventTime, buf);
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
        int_to_str(p->raceKey, buf);
        font_text_add(gHudTextObjs_3[5], buf);
        font_draw_text(gHudTextObjs_3[5], xa, yy + 16);
        c = (u8)p->anim;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->unk_3FB != 0) {
            d = p->trick;
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
            int_to_str(p->stuntScore, buf);
            font_text_add(gHudTextObjs_3[3], buf);
            ax = 0x100;
            ay = ty;
            if (p->unk_3FB >= 2) {
                k = p->unk_3FB * 8;
                x3 = (x3 * k + (xa - (gHudTextObjs_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                for (i = 0; i < p->trickListLen; i++) {
                    if (idx == p->trickList[i]) {
                        break;
                    }
                }
                if (i == p->trickListLen) {
                    ax = (ax * k + ((gHudTextObjs_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (ty + i * 8) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs_3[2], ax, ay);
            font_draw_text(gHudTextObjs_3[3], x3, y3);
        }
        for (i = 0; i < p->trickListLen; i++) {
            font_text_create(gHudTextObjs_3[i + 8], 0x40, 2, 2, 0x20);
            font_text_add(gHudTextObjs_3[i + 8], gRiderAnims_11[p->trickList[i]].name);
            font_draw_text(gHudTextObjs_3[i + 8], 0x3C, ty + i * 8);
        }
        font_text_create(gHudTextObjs_3[6], 0x40, 3, 2, 0x12);
        v = p->speed;
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
        event_format_time(gEventTime, buf);
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
        int_to_str(p->raceKey, buf);
        font_text_add(gHudTextObjs_3[5], buf);
        font_draw_text(gHudTextObjs_3[5], x, y);
        if (gViMode == 2) {
            by = 0x69;
            dy = 0x14;
        } else {
            by = 0x5A;
            dy = 0xA;
        }
        c = (u8)p->anim;
        idx = -1;
        if (c < 16) {
            idx = (s8)c;
        } else if (p->unk_3FB != 0) {
            d = p->trick;
            if (d < 16) {
                idx = d;
            }
        }
        if (idx >= 0) {
            font_text_create(gHudTextObjs_3[2], 0x40, 1, 2, 0x21);
            font_text_add(gHudTextObjs_3[2], gRiderAnims_11[idx].name);
            font_text_create(gHudTextObjs_3[3], 0x40, 1, 4, 0x21);
            int_to_str(p->stuntScore, buf);
            font_text_add(gHudTextObjs_3[3], buf);
            x3 = gScreenWidth / 2;
            y3 = D_800E171C + 0x28;
            ax = gScreenWidth / 2;
            ay = D_800E171C + 0x14;
            if (p->unk_3FB >= 2) {
                k = p->unk_3FB * 8;
                x3 = (x3 * k + (x - (gHudTextObjs_3[3]->w18 >> 1)) * (256 - k)) >> 8;
                y3 = (y3 * k + y * (256 - k)) / 256;
                for (i = 0; i < p->trickListLen; i++) {
                    if (idx == p->trickList[i]) {
                        break;
                    }
                }
                if (i == p->trickListLen) {
                    ax = (ax * k + ((gHudTextObjs_3[4]->w18 >> 1) + 0x23) * (256 - k)) >> 8;
                    ay = (ay * k + (by + i * dy) * (256 - k)) / 256;
                }
            }
            font_draw_text(gHudTextObjs_3[2], ax, ay);
            font_draw_text(gHudTextObjs_3[3], x3, y3);
        }
        v = gRiders[pl].speed;
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
        for (i = 0; i < p->trickListLen; i++) {
            font_text_create(gHudTextObjs_3[i + 8], 0x40, 2, 2, 0x20);
            font_text_add(gHudTextObjs_3[i + 8], gRiderAnims_11[p->trickList[i]].name);
            font_draw_text(gHudTextObjs_3[i + 8], 0x23, by + i * dy);
        }
        if (gEventAllFinished != 0) {
            font_text_create(gHudTextObjs_3[7], 0x40, 1, 4, 0x21);
            font_text_add(gHudTextObjs_3[7], gRiders[0].finishState >= 3 ? "Press START to Continue" : "Game Over");
            font_draw_text(gHudTextObjs_3[7],
                          gViewRects[gNumScreens - 1][pl].x + gViewRects[gNumScreens - 1][pl].w / 2,
                          gViewRects[gNumScreens - 1][pl].y + gViewRects[gNumScreens - 1][pl].h / 2);
        }
    }
    gdl = font_render(gdl);
    gdl = render_draw_temp_gauge(gdl, a1, a2, pl);
    if (gEvent_5.h26 != 0 && gEvent_5.w0 != 0 && (gEvent_5.w0 != 1 || gEvent_5.h26 < 0x5B)) {
        gdl = event_draw_countdown(gdl, pl);
    }
    return gdl;
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { s32 f0; u8 pad[0x22]; s16 f26; } Obj5A8;

extern Obj5A8 gEvent_29;

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

    fv = gRiders[idx].speed;
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
        event_format_time(gEventTime, buf);
        y = gViewRects_10[gNumScreens - 1][idx].b;
        font_text_add(gHudTextObjs[1], D_800D8484);
        z = y + 0x14;
        font_draw_text(gHudTextObjs[1], 0x23, z);
        font_text_add(gHudTextObjs[2], buf);
        font_draw_text(gHudTextObjs[2], 100, z);
        font_text_create(gHudTextObjs[3], 0x40, 1, 2, 0x20);
        font_text_create(gHudTextObjs[4], 0x40, 1, 4, 0x20);
        int_to_str(gRiders[0].checkpoint + 1, buf);
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

    if (gRiders[0].race.finishPlace != -1) {
        h = font_text_create(D_800F0548, 0x20, 1, 4, 0x21);
        if (gRiders[0].finishState == 0) {
            font_text_add(h, D_800D84A8);
            font_draw_text(h,
                          gViewRects_10[gNumScreens - 1][idx].a + gViewRects_10[gNumScreens - 1][idx].c / 2,
                          gViewRects_10[gNumScreens - 1][idx].b + gViewRects_10[gNumScreens - 1][idx].d / 2);
        } else {
            event_set_place_text(h, gRiders[0].race.finishPlace, idx);
        }
    }

    gdl = event_draw_position(gdl, 0, font_text_create(D_800F0544, 0x20, 3, 2, 0x21));
    gdl = font_render(gdl);
    if (gEvent_29.f26 != 0 && gEvent_29.f0 != 0 &&
        (gEvent_29.f0 != 1 || gEvent_29.f26 < 0x5B)) {
        gdl = event_draw_countdown(gdl, idx);
    }
    return gdl;
}

typedef struct { s32 unk0; u16 *col; Img_2 img[5]; } Hud_3;
typedef struct { s32 mode; u8 pad[0x22]; s16 t; } State_5;

extern State_5 gEvent_30;
extern Hud_3 *sCountdownTextures_2;

Gfx *event_draw_countdown(Gfx *gdl, s32 a)
{
    f32 scale;
    s32 alpha;
    u16 col;
    Img_2 *img;
    s32 sw, dsdx, th;
    s32 *r;

    if (gEvent_30.mode == 5 && gEvent_30.t == 0)
        return gdl;
    if (D_800C8CE0 != 0)
        a = 0;
    switch (gEvent_30.mode) {
    default:
        return gdl;
    case 1: {
        f32 v;
        col = sCountdownTextures_2->col[4];
        img = &sCountdownTextures_2->img[4];
        scale = gEvent_30.t + 10;
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
        scale = gEvent_30.t + 10;
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
        scale = gEvent_30.t + 10;
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
        scale = gEvent_30.t + 10;
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
        scale = gEvent_30.t + 1;
        scale *= 0.0625f;
        img = &sCountdownTextures_2->img[3];
        col = sCountdownTextures_2->col[3];
        alpha = scale * 120.0f;
        scale = 3.5f - scale * 3.0f;
        break;
    }

    sw = scale * (img->w * 8.0f);
    dsdx = 1024.0f / scale;
    if (gEvent_30.mode == 1)
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
                        gEvent_30.mode == 1 ? -(dsdx * 2) : -dsdx);
    gDPPipeSync(gdl++);
    return gdl;
}

void *event_draw_position(void *param_1, s32 param_2, void *param_3)
{
    Rider *base = gRiders;
    Rider *pl = &base[param_2];
    s32 a = pl->race.placeAnim;
    char buf[3];

    if (a > 0x180) {
        a = 0x300 - a;
    }
    param_1 = font_render(param_1);
    font_reset_queue();
    buf[0] = ((u8 *)&pl->race.prevPlace)[1] + '1';
    buf[1] = (pl->race.prevPlace < 3) ? ((u8 *)&pl->race.prevPlace)[1] + ':' : '=';
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
    s32 w0;
    u8 pad4[0x10 - 0x4];
    s32 w10;
    u8 pad14[0x20 - 0x14];
    s16 h20;
    s16 h22;
    u8 pad24[2];
    s16 h26;
    u8 pad28[4];
    s32 w2C;
    u8 pad30[0x40 - 0x30];
    s32 w40;
} Mode_2;

typedef struct {
    u32 w0;
    u8 pad4[0x4C - 4];
    s32 w4C;
} Game_24;

extern Mode_2 gEvent_31;
extern Game_24 gGame_41;

Gfx *event_draw_hud_race(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[88];
    char *p;
    RiderRace *lp;
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

    lp = &gRiders[(gCamTargetOverride == -1) ? pl + D_800C8CE0 : gCamTargetOverride].race;
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
        if (gEvent_31.h26 != 0 && gEvent_31.w0 == 5) {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 2, 0x21);
        } else {
            tMsg = font_text_create(gHudTextObjs[6], 0x20, 1, 4, 0x21);
        }
        if (lp->lapMsgTimer != 0 || lp->lap == gEvent_31.h20) {
            if (lp->lapMsgTimer != 0) {
                lp->lapMsgTimer--;
            }
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 4, 0x10);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 4, 0x10);
        } else {
            tLap = font_text_create(gHudTextObjs[7], 0x20, 2, 2, 0x10);
            tLapL = font_text_create(gHudTextObjs[8], 0x20, 2, 2, 0x10);
        }
        if (gGame_41.w0 & 0x20000000) {
            Txt_2 *t1, *t2, *t3;
            t1 = font_text_create(gHudTextObjs[9], 0x20, 2, 2, 0x10);
            debug_ftoa(buf, gRiders[0].bike.pos.x, 5);
            font_text_add(t1, buf);
            font_draw_text(t1, 0x28, 0x1E);
            t2 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
            debug_ftoa(buf, gRiders[0].bike.pos.z, 5);
            font_text_add(t2, buf);
            font_draw_text(t2, 0x28, 0x28);
            t3 = font_text_create(gHudTextObjs[10], 0x20, 2, 2, 0x10);
            int_to_str(gDebugFreeMem, buf);
            font_text_add(t3, buf);
            font_draw_text(t3, 0x28, 0x32);
        }
        if (gGame_41.w4C == 6 && gEvent_31.w2C == 0) {
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
        if (gEvent_31.h26 != 0 && gEvent_31.w0 == 5) {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 2, 0x21);
        } else {
            tMsg = font_text_create(gHudTextObjs[8], 0x20, 1, 4, 0x21);
        }
    }
    if (lp->finishPlace != -1) {
        event_set_place_text(tMsg, lp->finishPlace, pl);
    }
    if (gReplayPlayback == 0) {
        if (gEvent_31.h26 != 0 && gEvent_31.w0 != 0 && (gEvent_31.w0 != 1 || gEvent_31.h26 < 0x5B)) {
            gdl = event_draw_countdown(gdl, pl);
        } else if (gPauseMenu == 0 && lp->finishPlace == -1) {
            if (gRiders[pl].race.gapMsgTimer != 0) {
                v = gRiders[pl].race.gap;
                gRiders[pl].race.gapMsgTimer--;
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
            } else if (((gRiders[pl].offTrackTimer - 1) & 0x20) == 0) {
                font_text_add(tMsg, "Off Course");
                font_draw_text(tMsg, VP.x + VP.w / 2, VP.y + VP.h / 2);
            } else if (gRiders[pl].wrongWay != 0 && (gRaceFrame & 0x20)) {
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
            if (gEvent_31.h22 >= 2) {
                gdl = event_draw_position(gdl, pl, tPlace);
            }
            f = gRiders[pl].speed;
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
            if (lp->lap == gEvent_31.h20) {
                event_format_time(lp->finishTime, buf);
            } else {
                event_format_time(gEvent_31.w2C, buf);
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
                if (lp->lapMsgTimer != 0 || lp->lap == gEvent_31.h20) {
                    int_to_str(lp->lap, p + strlen(p));
                } else {
                    int_to_str(lp->lap + 1, p + strlen(p));
                }
                str_cat(p, ": ");
                font_text_add(tLapL, buf);
                event_format_time(lp->lapTimes[(lp->lapMsgTimer != 0 || lp->lap == gEvent_31.h20) ? lp->lap - 1 : lp->lap], buf);
                font_text_add(tLap, buf);
                font_draw_text(tLapL, xb, yy);
                font_draw_text(tLap, xa, yy);
                yy += dy;
                if (!(gCurTrack_8->w50 & 2)) {
                    event_format_time(gEvent_31.w10, buf);
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
                f = gRiders[pl].speed;
                int_to_str((s32)(((0.0f < f) ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
                font_text_add(tSpeed, buf);
                font_draw_text(tSpeed, VP.x + VP.w - 0x2D, VP.y + VP.h - 0xB);
                if (lp->lap == gEvent_31.h20) {
                    event_format_time(lp->finishTime, buf);
                } else {
                    event_format_time(gEvent_31.w2C, buf);
                }
                font_text_add(tRace, buf);
                font_text_add(tRaceL, "Race: ");
                font_draw_text(tRaceL, VP.x + 10, VP.y + VP.h - 0x12);
                font_draw_text(tRace, VP.x + 0x43, VP.y + VP.h - 0x12);
            }
            __builtin_strcpy(buf, "Lap ");
            p = buf;
            if (lp->lapMsgTimer != 0 || lp->lap == gEvent_31.h20) {
                int_to_str(lp->lap, p + strlen(p));
            } else {
                int_to_str(lp->lap + 1, p + strlen(p));
            }
            str_cat(p, ": ");
            font_text_add(tLapL, buf);
            font_draw_text(tLapL, VP.x + 10, VP.y + VP.h - 8);
            event_format_time(lp->lapTimes[(lp->lapMsgTimer != 0 || lp->lap == gEvent_31.h20) ? lp->lap - 1 : lp->lap], buf);
            font_text_add(tLap, buf);
            font_draw_text(tLap, VP.x + 0x43, VP.y + VP.h - 8);
            gdl = event_draw_temp_gauge(font_render(gdl), a1, a2, pl);
            break;
        default:
            gDPPipeSync(gdl++);
            gDPSetCycleType(gdl++, G_CYC_1CYCLE);
            gDPPipeSync(gdl++);
            buf[0] = (u8)gRiders[pl].race.place + '1';
            buf[1] = (gRiders[pl].race.place < 3) ? (u8)gRiders[pl].race.place + '{' : '~';
            buf[2] = 0;
            font_text_add(tPlace, buf);
            font_draw_text(tPlace, VP.x + VP.w - 10, VP.y + 13);
            f = gRiders[pl].speed;
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
    if (gEvent_31.w40 != 0 && gCurView == gNumScreens - 1) {
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
    u32 w0;
    u8 pad4[0x2C - 0x4];
    s32 w2C;
    u8 pad30[0x58 - 0x30];
    s32 w58;
    u8 pad5C[0x68 - 0x5C];
    s16 h68;
    s16 h6A;
} Mode_3;

typedef struct {
    u8 pad0[0x3C];
    s32 w3C;
} Game_25;

extern Mode_3 gEvent_32;
extern Game_25 gGame_42;

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
        if (gEvent_32.w0 == 7 || gEvent_32.w0 == 9 || gEvent_32.w0 == 11) {
            gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xE6);
            gDPRaw(gdl++, 0xE3000C00, 0);
            gDPRaw(gdl++, 0xD7000002, 0x80008000);
            gDPRaw(gdl++, 0xE3001001, 0);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            cx = 0x100;
            cy = 0x78;
            if (gNumPlayers == 4) {
                cx = 0x13C;
            }
            gdl = render_draw_image((cx - sSoccerMapTexture_2[8] * 4) | 1, cy - (sSoccerMapTexture_2[9] >> 1) - 1, sSoccerMapTexture_2 + 8, gdl,
                                p2, 1, 1, 6, 0);
            gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            gDPRaw(gdl++, 0xD7000000, 0x80008000);
            gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x28, 0xFF, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders[0].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[0].bike.pos.x * 0.25f - 2.0f,
                             cx + gRiders[0].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[0].bike.pos.x * 0.25f + 2.0f);
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0x28, 0xFF);
            gDPFillRectangle(gdl++, cx + gRiders[1].bike.pos.z * 0.25f / 0.625f - 2.0f, cy + gRiders[1].bike.pos.x * 0.25f - 2.0f,
                             cx + gRiders[1].bike.pos.z * 0.25f / 0.625f + 2.0f, cy + gRiders[1].bike.pos.x * 0.25f + 2.0f);
            if (gNumPlayers == 4) {
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
        switch (gEvent_32.w0) {
        case 6:
            font_text_create(gHudTextObjs[0], 0x40, 1, 4, 0x21);
            if (gNumPlayers == 2) {
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
            switch (gEvent_32.w0) {
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
            int_to_str(gEvent_32.h68, buf);
            font_text_add(gHudTextObjs[3], buf);
            int_to_str(gEvent_32.h6A, buf);
            font_text_add(gHudTextObjs[4], buf);
            font_draw_text(gHudTextObjs[1], gScreenWidth / 2 - 45, gScreenHeight / 2);
            font_draw_text(gHudTextObjs[2], gScreenWidth / 2 - 45, gScreenHeight / 2 + 13);
            font_draw_text(gHudTextObjs[3], gScreenWidth / 2 + 39, gScreenHeight / 2);
            font_draw_text(gHudTextObjs[4], gScreenWidth / 2 + 39, gScreenHeight / 2 + 13);
            if (gEvent_32.w0 == 12) {
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
            int_to_str(gEvent_32.h68, buf);
            font_text_add(gHudTextObjs[2], buf);
            int_to_str(gEvent_32.h6A, buf);
            font_text_add(gHudTextObjs[3], buf);
            if (gNumPlayers == 4) {
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
            switch (gEvent_32.w0) {
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
            event_format_time(gEvent_32.w2C, buf);
            font_text_add(gHudTextObjs[5], buf);
            if (gNumPlayers == 4) {
                font_draw_text(gHudTextObjs[4], gScreenWidth / 2 - 10, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[5], gScreenWidth / 2 - 10, gScreenHeight / 2 + 7);
            } else {
                font_draw_text(gHudTextObjs[4], gScreenWidth - 65, gScreenHeight / 2 - 7);
                font_draw_text(gHudTextObjs[5], gScreenWidth - 65, gScreenHeight / 2 + 7);
            }
            break;
        }
    }
    n = gEvent_32.w58 & 0xF;
    if (n != 0) {
        team = n - 1;
        font_text_create(gHudTextObjs[7], 0x40, gGame_42.w3C == 4 ? 2 : 1, 4, 0x21);
        if (team == (p3 >> ((gGame_42.w3C - 1) >> 1))) {
            if (gEvent_32.w58 & 0x20) {
                font_text_add(gHudTextObjs[7], "Team ");
                int_to_str((team ^ 1) + 1, buf);
                font_text_add(gHudTextObjs[7], buf);
                font_text_add(gHudTextObjs[7], " scored for you!");
            } else {
                font_text_add(gHudTextObjs[7], D_800D871C);
            }
        } else {
            if (gEvent_32.w58 & 0x20) {
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

typedef struct {
    s32 unk0;
    u16 *palette;
    Glyph glyphs[1];
} Font;

typedef struct {
    u8 unk0[0x730];
    Font *font;
} Hud_4;

extern Hud_4 gGame_43;

Gfx *event_draw_glyph_string(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y) {
    while (*str != 0) {
        s32 w; s32 hh;

        gDPWords(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPWords(gdl++, 0xE3001001, 0x8000);
        gDPWords(gdl++, 0xFD100000, (u32)gGame_43.font->palette);
        gDPWords(gdl++, 0xE8000000, 0);
        gDPWords(gdl++, 0xF5000100, 0x07000000);
        gDPWords(gdl++, 0xE6000000, 0);
        gDPWords(gdl++, 0xF0000000, 0x073FC000);
        gDPWords(gdl++, 0xE7000000, 0);
        w = gGame_43.font->glyphs[*str].w << 3;
        hh = gGame_43.font->glyphs[*str].h;
        gdl = render_draw_image(x, y - (hh >> 1), &gGame_43.font->glyphs[*str], gdl, arg1, 1, 1, 3, 0);
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
    u32 state;
    u8 pad4[0x22];
    s16 f26;
    u8 pad28[4];
    s32 f2C;
    u8 pad30[0x28];
    s32 f58;
    u8 pad5C[0x14];
    s32 f70;
    u8 pad74[4];
    s32 f78;
} Game_26;

extern Track_13 gTutorialLessons_10[];
extern Game_26 gEvent_33;

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
    if (gEvent_33.state == 0xD) {
        if (gEvent_33.f70 == 0) {
            off = -(gEvent_33.f78 * 2);
        } else {
            off = 0;
            if (gEvent_33.f70 == 2) {
                off = -((60 - gEvent_33.f78) * 2);
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
                if (gEvent_33.f58 & 0x1000) {
                    font_text_add(gHudTextObjs[0], "Takedown Required");
                } else {
                    font_text_add(gHudTextObjs[0], "Takedown Complete");
                }
                font_draw_text(gHudTextObjs[0], 0x21, 0x2D);
            } else if (flags & 8) {
              if (!(flags & 2)) {
                font_text_create(gHudTextObjs[0], 0x40, 1, 6, 0x20);
                v = gEvent_33.f2C;
                if (v < 0) {
                    v = -v;
                }
                event_format_time_short(v, buf);
                font_text_add(gHudTextObjs[0], "Time: ");
                font_text_add(gHudTextObjs[0], buf);
                if (gEvent_33.f2C < 0) {
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
                if (gActiveReplay == 0 && !(gEvent_33.f58 & 0x8000)) {
                    goto skip;
                }
                if (!(gEvent_33.f58 & 0x800)) {
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
        if (gEvent_33.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img = D_800EFD6C_2 + 0x30;
        } else {
            img = D_800EFD6C_2 + 0x40;
        }
        for (i = 0; i < 9; i++) {
            gdl = render_draw_image(i * img[0] * 8 + 0x10, 0x14, img, gdl, arg2, 1, 1, 6, 0);
        }
        if (gEvent_33.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
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
    f = gRiders[arg3].speed;
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
    if ((CUR_FLAGS & 8) && gEvent_33.f26 != 0
        && (gEvent_33.state < 5 || gEvent_33.state == 15 || gEvent_33.state == 14)
        && (gEvent_33.state != 1 || gEvent_33.f26 < 0x5B)) {
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

#define ROWS (gNumPlayers_2[0] >= 2 ? 6 : 9)
#define COLS (gNumPlayers_2[0] >= 3 ? 8 : 12)
#define RECT (gViewRects[gNumViews - 1][D_800F05F8])

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

    if (gHudLevel != 3 || gGameMode == 9) {
        switch (gGameMode) {
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
    if (D_800F0630 >= 0 || D_800F0630 == -2) {
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
        prev = 35;
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, prev);
        for (i = 0; i < ROWS; i++) {
            for (j = i & 1; j < COLS; j += 2) {
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
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
                if (D_800F0630 == -2) {
                    a = 35;
                } else {
                    n = D_800F0630 - j * 10;
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
        for (n = 0; n < (gGameMode == 1 ? 4 : 5); n++) {
            font_text_create(gMenuTextObjs[n], 0x40, 1, n == gPauseMenuCursor ? 4 : 2, 0x11);
            if (gGameMode == 1) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSeason[n]);
            } else if (gGameMode == 2) {
                font_text_add(gMenuTextObjs[n], sPauseMenuItemsSoccer[n]);
            } else {
                font_text_add(gMenuTextObjs[n], sPauseMenuItems[n]);
            }
            font_draw_text(gMenuTextObjs[n], gViewRects[0][0].x + gViewRects[0][0].w / 2,
                          gViewRects[0][0].y + gViewRects[0][0].h / 2 - sp * 2 + sp * n);
        }
        break;
    case 5:
        if (D_800F0618 != 6) {
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

extern HudTex *sTempGaugeTextures_2;
extern HudTex *sTempGaugeTextures_3[];

Gfx *event_draw_temp_gauge(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 fp, n, bar, k;
    u16 col;
    Img_6 *img;
    Rect *r;


    fp = -54;
    n = gNumScreens;
    if (n == 2) fp = -46;
    bar = gRiders[pl].engine.heat * 38.0f / 12000.0f;
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
    if (gRiders[pl].engine.overheated) {
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
    if (0 < D_800CA924_3.limit && (gGame & 4) == 0) {
        D_800CA924_3.counter = D_800CA924_3.counter + 1;
        if (D_800CA924_3.counter == D_800CA924_3.limit) {
            mem_free(D_800CA924_3.arg);
        }
    }
    if (gGameMode == 9 && D_800F0624 != 0) {
        mem_free(D_800F0624);
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
        return ((gEventFlags >> 12) ^ 1) & 1;
    }
    if (*(u32 *)(e + 0x1C) != 0) {
        return 0;
    }
    return ~gEventTime_3 >> 31;
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

typedef struct {
    u8 pad0[0x58];
    s32 w58;
} G_2;

extern G_2 gEvent_34;

void event_gates_init(void) {
    gEvent_34.w58 |= 0x10;
    race_reset_grid_order();
}

void event_stop_sounds_rumble(void)
{
    snd_stop_all_riders();
    rumble_stop_all();
}

void event_update_gates(void)
{
    s32 *g;

    event_update_countdown();
    g = gEvent_3;
    if (g[0x40 / 4] == 0) {
        if ((g[0x58 / 4] & 0x10) == 0) {
            s32 n = g[0x2C / 4] + 1;
            g[0x2C / 4] = n;
            if (n == 0x34BC0) {
                event_rider_finished(0);
                g[0x40 / 4] = 1;
            }
        }
    } else if ((gPlayerInputs_2 & 0x40) != 0 && (gPlayerInputsPrev & 0x40) == 0) {
        event_request_exit();
    }
}

typedef struct {
    u8 pad0[0x48];
    s32 f48;
    u8 pad1[0xC];
    u32 f58;
} Blk_15;

extern Blk_15 gEvent_35;

void event_hillclimb_init(void) {
    Rider *e;

    gEvent_35.f48 = -1;
    gEvent_35.f58 |= 0x10;
    race_reset_grid_order();
    for (e = gRiders; e < &gRiders[gGame_9.count]; e++) {
        e->timeLimit = 7200;
    }
}

void event_stunt_init(void)
{
    u32 *p = (u32 *)gEvent_2;
    p[0x58 / 4] = p[0x58 / 4] | 0x10;
    p[0x2C / 4] = 0x1C20;
    race_reset_grid_order();
}

void event_soccer_init(void)
{
    u8 *p;
    s32 w;

    p = gEvent_2;
    w = *(s32 *)(p + 0x58);
    *(u16 *)(p + 0x26) = 0xB4;
    *(s32 *)&gEvent_2 = 6;
    *(u8 *)(p + 0x6E) = 0;
    *(u16 *)(p + 0x68) = 0;
    *(u16 *)(p + 0x6A) = 0;
    *(u16 *)(p + 0x6C) = 0;
    *(s32 *)(p + 0x58) = w | 0x10;
}

typedef struct {
    s32 state;
    u8 pad[0x1E];
    s16 count;
    s16 f24;
} Ctl_16;

void event_reset_places(void) {
    Ctl_16 *c = &gEvent;
    s32 i;
    RiderRace *t;

    if (c->state == 8) {
        race_set_grid_order(2, 0);
    }
    i = 0;
    c->f24 = 0;
    if (c->count > 0) {
        s16 neg = -1;
        Ctl_16 *d = c;
        t = &gRiders[0].race;
        do {
            t->finishPlace = neg;
            t = (RiderRace *)((u8 *)t + sizeof(Rider));
        } while (++i < d->count);
    }
}

typedef struct {
    u8 pad[0x66];
    u8 a[1];
} Thing;

extern Thing *gSeason_13;

void event_clear_season_race_points(void) {
    s32 i;
    for (i = 0; i < gEvent.n; i++) {
        if (gRiders[i].race.finishPlace != -1) {
            gSeason_13->a[i] = 0;
        }
    }
}

typedef struct {
    s32 state;
    s32 pad[10];
    s32 cnt;
} Blk_16;

extern Blk_16 gEvent_36;

void func_8005B2E8(void)
{
    event_update_countdown();
    if (gEvent_36.state == 5) {
        if (gEvent_36.cnt != 0) {
            s32 r = gEvent_36.cnt - 1;
            gEvent_36.cnt = r;
            if (r == 0) {
                gRiders[0].race.lapMsgTimer = 0x78;
            }
        }
    }
}

void event_hud_free(void)
{
    s32 i;
    s32 *p1;
    s32 *p2;

    if (gGame_4[0x13] == 9) {
        mem_free(sGreyPalette);
    }
    if (gGame_4[0xF] == 1) {
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
