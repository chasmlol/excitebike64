#include "game.h"

/* A graphics task for the scheduler (unnamed/sched.c): the SDK's OSScTask, with the timing fields. */
typedef struct OSScTask_s {
    struct OSScTask_s *next;    /* 0x00 */
    u32 state;                  /* 0x04 */
    u32 flags;                  /* 0x08 */
    void *framebuffer;          /* 0x0C */
    OSTask list;                /* 0x10 */
    OSMesgQueue *msgQ;          /* 0x50 */
    OSMesg msg;                 /* 0x54 */
    u64 startTime;              /* 0x58 */
    u64 totalTime;              /* 0x60 */
} OSScTask;

/* OSScTask.flags, as in the SDK's sched.h */
#define SC_NEEDS_RDP    0x0001
#define SC_NEEDS_RSP    0x0002
#define SC_LAST_TASK    0x0020  /* the last task of the frame */
#define SC_SWAPBUFFER   0x0040  /* show its framebuffer when it is done */

/* The scheduler's messages, and main_sched_event_thread's to the main thread (a pointer to the s16 type) */
#define SC_MSG_RETRACE  1
#define SC_MSG_GFX_DONE 2

/* The u16 at 0x80000402 (in the entry code) says how the game image was started. */
#define BOOT_MAGIC_DIRECT   0xDEAD  /* loaded directly */
#define BOOT_MAGIC_LOADER   0xBEEF  /* by the boot loader, whose header is still at 0x803FFFC0 */

/* gGfxContexts: one per framebuffer, used in turn. */
typedef struct Ctx {
    u8 pad0[0x440];
    Gfx *dl;                    /* 0x440 */
    u8 pad444[0x4C8 - 0x444];
    s16 msgtype;                /* 0x4C8 */
    u8 pad4CA[0x4E8 - 0x4CA];
    OSScTask task;              /* 0x4E8 */
    u8 pad550[0x560 - 0x550];
} Ctx;

typedef struct { u16 button; s8 x, y; } Pad;

extern Pad gContPads_2[4];
extern Pad gPadInputPrev_4[4];
extern Ctx gGfxContexts[2];
extern Ctx *gCurGfxContext;
extern Pad gPadInput_4[4];

/* The main thread: sets up memory, framebuffers, the scheduler, controllers and sound, then runs the game states
   (gGameStates) on each retrace message and submits their display lists. */
void game_main(void)
{
    char line[0x780];
    OSMesg msg;
    s32 viMode;
    s32 bootMagic;
    s32 fbSize;
    OSScTask *task = NULL;
    s32 i;
    u32 fbTop;
    Pad *pads;
    GameState *next;
    GameState *cur;

    msg = NULL;
    bootMagic = *(u16 *)0x80000402;
    if (bootMagic == BOOT_MAGIC_DIRECT) {
        gLoadedByBootLoader = 0;
    } else if (bootMagic == BOOT_MAGIC_LOADER) {
        gLoadedByBootLoader = 1;
    } else {
        for (;;) {
        }
    }
    if (gLoadedByBootLoader != 0) {
        __builtin_memcpy(gBootHeader, (void *)0x803FFFC0, 0x30);
        gResourceRomAddr = *(s32 *)(gBootHeader + 0x2C);  /* the resources' ROM address the boot loader left */
        bootMagic = *(u16 *)0x80000406;
        if (bootMagic == 0xDEAF) {
            D_800C2954 = 1;
        }
    }
    mem_fill((u8 *)gGfxContexts, 0, sizeof(gGfxContexts));
    if (osMemSize == 0x800000) {
        gHasExpansionPak = 1;
    } else {
        gHasExpansionPak = 0;
    }
    D_800E2034 = 0;
    gFrameCounter = 0;
    sLastDrawFrame = -1;
    sLastUpdateFrame = -1;
    main_init_memory();
    /* the two framebuffers below fbTop, the Z buffer above it */
    if (gHasExpansionPak != 0) {
        fbTop = 0x80600000;
        gFbAllocWidth = 640;
        gFbAllocHeight = 480;
    } else {
        fbTop = 0x80300000;
        gFbAllocWidth = 512;
        gFbAllocHeight = 240;
    }
    fbSize = gFbAllocWidth * gFbAllocHeight * 4 + 0x40;
    gFramebuffers_2[0] = mem_alloc_at_dbg(fbTop - fbSize, fbSize - 0x10, sStrMainFile, 361);
    gFramebuffers_2[1] = (u16 *)((u8 *)gFramebuffers_2[0] + gFbAllocWidth * gFbAllocHeight * 2);
    {
    void *z;
    gZBuffer = z = mem_alloc_at_dbg(fbTop, gFbAllocWidth * gFbAllocHeight * 2, sStrMainFile, 363);
    if (gFramebuffers_2[0] == NULL || z == NULL) {
        fatal_halt(1);
    }
    }
    mem_fill((u8 *)gFramebuffers_2[0], 0, gFbAllocWidth * gFbAllocHeight * 4);
    dma_init();
    main_init_sched();
    mem_fill((u8 *)gPadInput_4, 0, sizeof(gPadInput_4));
    cont_init();
    start_controller_read();
    cont_finish_read();
    cont_read_pads();
    for (i = 0; i < MAXCONTROLLERS; i++) {
        main_convert_pad_input(&gContPads_2[i], &gPadInput_4[i]);
    }
    gReplayBuffer = mem_alloc_dbg(0xC800, 0, sStrMainFile, 405);
    sGfxDramStack = mem_alloc_dbg(0x401, 0, sStrMainFile, 412);
    sGfxYieldBuffer = mem_alloc_dbg(OS_YIELD_DATA_SIZE, 0, sStrMainFile, 413);
    file_init(gResourceRomAddr);
    res_set_load_buffer(0);
    gCurGameState = &gGameStates[GAME_STATE_SHELL];
    gGfxContexts[0].dl = NULL;
    gGfxContexts[1].dl = NULL;
    func_8005B5E0();
    osCreateMesgQueue(&sSchedEventQueue, sSchedEventMsgs, 16);
    /* the thread's stack ends where gPadInputPrev starts */
    osCreateThread(&sSchedEventThread, 5, main_sched_event_thread, NULL, gPadInputPrev_4, 12);
    osStartThread(&sSchedEventThread);
    sched_add_client((s32)sSchedClient, (s32)&sSchedEventQueue);
    snd_init();
    gIsDrawing = 0;
    main_init_dev_pi_handle();
    main_set_debug_freeze(0);

    for (;;) {
        /* a debug lag generator: a busy loop that grows by 100 each frame */
        if (sDebugLagEnable != 0) {
            for (i = 0; i < sDebugLagLoops; i++) {
            }
            sDebugLagLoops += 100;
        }
        sSpStatus = __osSpGetStatus();
        {
        s32 n;
        gDebugFreeMem = osMemSize - (n = gMemFreeUnits * 16 + 0x400000);
        }
        gDebugFreeMem = gDebugFreeMem - gFbAllocWidth * gFbAllocHeight * 6 + 0xB4000;
        osRecvMesg(&sMainQueue, &msg, OS_MESG_BLOCK);
        switch (*(s16 *)msg) {
        case SC_MSG_RETRACE:
            if (gCurGameState->mode == STATE_INIT) {
                /* start the state: its display lists, its video mode, cleared framebuffers */
                if (gGfxContexts[0].dl == NULL) {
                    gGfxContexts[0].dl = mem_alloc_dbg(gCurGameState->dlSize * 8, 0, sStrMainFile, 523);
                    gGfxContexts[1].dl = mem_alloc_dbg(gCurGameState->dlSize * 8, 0, sStrMainFile, 524);
                }
                i = gCurGameState - gGameStates;
                if (i == GAME_STATE_SHELL || i == GAME_STATE_EDITOR) {
                    viMode = VI_MODE_MED_RES;
                } else if (i == GAME_STATE_NES) {
                    viMode = VI_MODE_LO_RES;
                } else if (i == GAME_STATE_RACE) {
                    viMode = VI_MODE_MED_RES;
                }
                gViModeRequest = viMode;
                while (gViModeRequest != gViMode) {
                }
                mem_fill((u8 *)gFramebuffers_2[0], 0, gFbAllocWidth * gFbAllocHeight * 2);
                mem_fill((u8 *)gFramebuffers_2[1], 0, gFbAllocWidth * gFbAllocHeight * 2);
                gCurGameState->mode = STATE_RUNNING;
                D_800C29E4 = 4;
                if (gGame.flags & GAME_REPLAY) {
                    gReplayPlayback = 1;
                } else {
                    gReplayPlayback = 0;
                }
                gCurGameState->init(0);
                break;
            }
            gFrameCounter++;
            if (gContReadPending != 0) {
                cont_finish_read();
                cont_read_pads();
            }
            /* debug: D-pad up and down on controller 1 freeze the game until left and right are pressed */
            if ((gContPads_2[0].button & (U_JPAD | D_JPAD)) == (U_JPAD | D_JPAD)) {
                main_set_debug_freeze(1);
                do {
                    start_controller_read();
                    cont_finish_read();
                    cont_read_pads();
                } while ((gContPads_2[0].button & (L_JPAD | R_JPAD)) != (L_JPAD | R_JPAD));
                main_set_debug_freeze(0);
                do {
                    start_controller_read();
                    cont_finish_read();
                    cont_read_pads();
                } while ((gContPads_2[0].button & (L_JPAD | R_JPAD)) == (L_JPAD | R_JPAD));
            }
            start_controller_read();
            *(Blk *)gPadInputPrev_4 = *(Blk *)gPadInput_4;
            for (i = 0; i < MAXCONTROLLERS; i++) {
                main_convert_pad_input(&gContPads_2[i], &gPadInput_4[i]);
            }
            pads = gPadInput_4;
            sAudioTickStart = osGetCount();
            snd_update();
            sAudioTickEnd = osGetCount();
            sAudioTickTime = sAudioTickEnd - sAudioTickStart;
            if (gFrameCounter - sLastUpdateFrame > 0 || gCurGameState->mode == STATE_WAIT_GPU) {
                if (gCurGameState->mode == STATE_RUNNING) {
                    if (gCurGameState->update != NULL) {
                        gCurGameState->update((u32 *)pads);
                    }
                }
                if (gCurGameState->mode == STATE_EXIT) {
                    gCurGameState->mode = STATE_WAIT_GPU;
                    gCurGameState->wait = 10;
                }
                if (gCurGameState->mode == STATE_WAIT_GPU && gCurGameState->wait == 0) {
                    gCurGameState->mode = STATE_EXIT;
                }
                if (gCurGameState->mode == STATE_EXIT) {
                    /* leave the state and pick the next */
                    gCurGameState->mode = STATE_EXITING;
                    gCurGameState->exit(0);
                    i = gCurGameState - gGameStates;
                    gCurGameState->mode = STATE_INIT;
                    if (i == GAME_STATE_EDITOR) {
                        /* the editor goes back to the front end, or to a test ride on the user track */
                        i = GAME_STATE_SHELL;
                        if (gGame.flags & GAME_EDITOR_TO_SHELL) {
                            gRaceExitCode = RACE_EXIT_NONE;
                        } else {
                            gCurTrack = &gTracks[TRACK_USER];
                            i = GAME_STATE_RACE;
                        }
                    } else if (i == GAME_STATE_NES) {
                        i = GAME_STATE_SHELL;
                    } else if (gGame.flags & GAME_NES) {
                        i = GAME_STATE_NES;
                    } else if (i == GAME_STATE_SHELL && (gGame.flags & GAME_EDITOR)) {
                        i = GAME_STATE_EDITOR;
                    } else if (gRaceSeries[0] > 0 && gRaceSeries[1] < gRaceSeries[0]) {
                        i = GAME_STATE_RACE;
                    } else {
                        /* the front end and the race take turns */
                        if (gGame.mode == MODE_SEASON && gSeason != NULL && !(gGame.flags & GAME_REPLAY)) {
                            gCurTrack = &gTracks[gSeason->cup->tracks[gSeason->round]];
                        }
                        if (i == GAME_STATE_RACE) {
                            if (gGame.flags & GAME_PODIUM) {
                                s32 k;
                                for (k = 0; k < 6; k++) {
                                    gGame.riders[k] = gGame.savedRiders[k];
                                    gGame.riderSkins[k] = gGame.savedRiderSkins[k];
                                }
                            }
                        }
                        if (i == GAME_STATE_RACE && gRaceExitCode == RACE_EXIT_EDITOR) {
                            i = GAME_STATE_EDITOR;
                        } else if (i == GAME_STATE_RACE &&
                                   (gGame.flags & (GAME_SEASON_COMPLETE | GAME_REPLAY)) == GAME_SEASON_COMPLETE) {
                            /* the season is won: the credits run on The Gravel Pit */
                            gCreditsMode = 1;
                            gGame.mode = MODE_EXHIBITION;
                            gGame.flags = (gGame.flags | GAME_LOW_RES) &
                                          ~(GAME_SEASON_COMPLETE | GAME_DIFFICULTY_UP | GAME_NEW_UNLOCKS |
                                            GAME_ROUND_WON | GAME_PODIUM | GAME_TRACK_INTRO);
                            gRaceExitCode = RACE_EXIT_NONE;
                            gCurTrack = &gTracks[TRACK_GRAVEL_PIT];
                            gGame.numPlayers = 1;
                        } else if (!(gGame.flags & GAME_REPEAT_STATE)) {
                            i ^= 1;
                        }
                    }
                    next = &gGameStates[i];
                    if (gCurGameState->dlSize != next->dlSize) {
                        mem_free(gGfxContexts[0].dl);
                        mem_free(gGfxContexts[1].dl);
                        gGfxContexts[0].dl = NULL;
                    }
                    gCurGameState = next;
                }
                sLastUpdateFrame = gFrameCounter;
            }
            sAudioTickStart = osGetCount();
            snd_update();
            sAudioTickEnd = osGetCount();
            sAudioTickTime = sAudioTickEnd - sAudioTickStart;
            if (sGfxTasksPending < 2 && gCurGameState->draw != NULL) {
                if (gCurGameState->mode == STATE_WAIT_GPU) {
                    if (sGfxTasksPending == 0) {
                        gCurGameState->wait--;
                    }
                } else if (gCurGameState->mode == STATE_RUNNING) {
                    /* draw the frame and submit it */
                    sLastDrawFrame = gFrameCounter;
                    gCurGfxContext = &gGfxContexts[sGfxContextIndex];
                    sGfxHead = gCurGfxContext->dl;
                    {
                    OSScTask *t = &gGfxContexts[sGfxContextIndex].task;
                    if (gRaceFrame < 60) {
                        func_80065A5C();
                    }
                    gIsDrawing = 1;
                    gGfxFrameIndex = sGfxContextIndex;
                    gCurGameState->draw(gCurGfxContext, sGfxContextIndex, &sGfxHead);
                    gIsDrawing = 0;
                    gDPPipeSync(sGfxHead++);
                    gDPFullSync(sGfxHead++);
                    gSPEndDisplayList(sGfxHead++);
                    sGfxDlLength = sGfxHead - gCurGfxContext->dl;
                    cur = gCurGameState;
                    if (sGfxDlMaxLength < sGfxDlLength) {
                        sGfxDlMaxLength = sGfxDlLength;
                    }
                    if (sGfxDlLength >= cur->dlSize) {
                        /* the display list overflowed */
                        for (;;) {
                        }
                    }
                    osWritebackDCacheAll();
                    t->list.t.data_ptr = (u64 *)gCurGfxContext->dl;
                    task = t;
                    task->list.t.data_size = (sGfxHead - gCurGfxContext->dl) * sizeof(Gfx);
                    task->list.t.type = M_GFXTASK;
                    task->list.t.flags = OS_TASK_DP_WAIT | OS_TASK_LOADABLE;
                    task->list.t.ucode_boot = (u64 *)gRspBootText;
                    task->list.t.ucode_boot_size = gF3dex2NonText - gRspBootText;
                    task->list.t.ucode = (u64 *)gF3dlx2RejText;
                    task->list.t.ucode_data = (u64 *)gF3dlx2RejData;
                    task->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
                    task->list.t.dram_stack = sGfxDramStack;
                    task->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
                    task->list.t.output_buff = NULL;
                    task->list.t.output_buff_size = NULL;
                    task->list.t.yield_data_ptr = sGfxYieldBuffer;
                    task->list.t.yield_data_size = OS_YIELD_DATA_SIZE;
                    task->next = NULL;
                    task->flags = SC_SWAPBUFFER | SC_LAST_TASK | SC_NEEDS_RSP | SC_NEEDS_RDP;
                    task->msgQ = &sSchedEventQueue;
                    gGfxContexts[sGfxContextIndex].msgtype = SC_MSG_GFX_DONE;
                    task->msg = &gGfxContexts[sGfxContextIndex].msgtype;
                    task->framebuffer = gFramebuffers_2[sGfxContextIndex];
                    task->totalTime = 0;
                    sched_submit_task((s32)task);
                    sGfxTasksPending++;
                    sGfxContextIndex = 1 - sGfxContextIndex;
                    sAudioTickStart = osGetCount();
                    snd_update();
                    sAudioTickEnd = osGetCount();
                    sAudioTickTime = sAudioTickEnd - sAudioTickStart;
                    }
                }
            }
            break;
        case SC_MSG_GFX_DONE:
            /* a debug screenshot to e:\log\scr*.raw: the file writes are gone, only the conversion to RGB is left */
            if (sScreenshotRequest != 0) {
                s32 x, y;
                __builtin_strcpy(line, "e:\\log\\scr");
                sScreenshotCounter = gFrameCounter * 765 % 99999;
                int_to_str(sScreenshotCounter, line + strlen(line));
                str_cat(line, ".raw");
                sScreenshotCounter++;
                for (y = 0; y < gScreenHeight; y++) {
                    for (x = 0; x < gScreenWidth; x++) {
                        s32 c = gFramebuffers_2[sLastDoneFb][y * gScreenWidth + x];
                        line[x * 3 + 0] = (c >> 11) << 3;
                        line[x * 3 + 1] = (c >> 6) << 3;
                        line[x * 3 + 2] = (c >> 1) << 3;
                    }
                }
                sScreenshotRequest = 0;
            }
            sLastDoneFb = 1 - sLastDoneFb;
            sGfxTasksPending--;
            break;
        }
    }
}

/* Turns a controller's state into the game input word (INPUT_*): the stick scaled to +-127 with a dead zone of 5,
   its direction bits and 4-bit magnitudes, and one bit per button. */
void main_convert_pad_input(OSContPad *pad, u32 *out)
{
    s32 x;
    s32 y;
    u32 yMag;
    u32 xMag;
    u32 word;

    *out = 0;
    x = pad->stick_x;
    x = x * 127 / 80;
    y = pad->stick_y;
    y = y * 127 / 80;
    if (x < -127) {
        x = -127;
    }
    if (x > 127) {
        x = 127;
    }
    if (y < -127) {
        y = -127;
    }
    if (y > 127) {
        y = 127;
    }
    if (x < -5) {
        xMag = (-x >> 3) << INPUT_STICK_X_SHIFT;
        if (xMag != 0) {
            *out = xMag | INPUT_STICK_LEFT;
        }
    } else if (x > 5) {
        xMag = (x >> 3) << INPUT_STICK_X_SHIFT;
        if (xMag != 0) {
            *out = xMag | INPUT_STICK_RIGHT;
        }
    }
    if (y < -5) {
        yMag = (-y >> 3) << INPUT_STICK_Y_SHIFT;
        if (yMag == 0) {
            goto done;
        }
        word = *out | INPUT_STICK_DOWN;
    } else {
        if (y < 6 || (yMag = (y >> 3) << INPUT_STICK_Y_SHIFT) == 0) {
            goto done;
        }
        word = *out | INPUT_STICK_UP;
    }
    *out = word | yMag;
done:
    if (pad->button & A_BUTTON) {
        *out |= INPUT_A;
    }
    if (pad->button & B_BUTTON) {
        *out |= INPUT_B;
    }
    if (pad->button & START_BUTTON) {
        *out |= INPUT_START;
    }
    if (pad->button & U_CBUTTONS) {
        *out |= INPUT_C_UP;
    }
    if (pad->button & D_CBUTTONS) {
        *out |= INPUT_C_DOWN;
    }
    if (pad->button & L_CBUTTONS) {
        *out |= INPUT_C_LEFT;
    }
    if (pad->button & R_CBUTTONS) {
        *out |= INPUT_C_RIGHT;
    }
    if (pad->button & U_JPAD) {
        *out |= INPUT_D_UP;
    }
    if (pad->button & D_JPAD) {
        *out |= INPUT_D_DOWN;
    }
    if (pad->button & L_JPAD) {
        *out |= INPUT_D_LEFT;
    }
    if (pad->button & R_JPAD) {
        *out |= INPUT_D_RIGHT;
    }
    if (pad->button & L_TRIG) {
        *out |= INPUT_L;
    }
    if (pad->button & R_TRIG) {
        *out |= INPUT_R;
    }
    if (pad->button & Z_TRIG) {
        *out |= INPUT_Z;
    }
}

/* Clears the memory after the BSS and makes all of it, from the front-end overlay up, the heap; the code and data
   in it are reserved, so that the front end and UsrTrk.c can be given back during a race
   (race_load_frontend_code). The overlay bounds must be 16-byte aligned. */
void main_init_memory(void)
{
    u32 top;
    u32 frontendStart;
    u32 usrTrkStart;
    u32 usrTrkEnd;
    u32 heapStart;
    u32 hdr;

    top = ((u32)osMemSize | 0x80000000) - 0x100;
    frontendStart = (u32)gFrontendOvlStart;
    usrTrkStart = (u32)gUsrTrkOvlStart;
    usrTrkEnd = (u32)gUsrTrkOvlEnd;
    if (frontendStart & 0xF) {
        fatal_halt(0x33);
    }
    if (usrTrkStart & 0xF) {
        fatal_halt(0x34);
    }
    if (usrTrkEnd & 0xF) {
        fatal_halt(0x35);
    }
    heapStart = (u32)sBssEnd;
    heapStart = (heapStart + 0xFF) & ~0xFF;
    osViBlack(1);
    mem_fill((u8 *)heapStart, 0, top - heapStart);
    mem_init(0, 0, frontendStart, top - frontendStart);
    hdr = 0x30;
    mem_alloc_at_dbg(frontendStart + 0x30, usrTrkStart - hdr - frontendStart, (u32)sStrMainFile, 0x4FA);
    hdr = 0x10;
    mem_alloc_at_dbg(usrTrkStart + 0x10, usrTrkEnd - hdr - usrTrkStart, (u32)sStrMainFile, 0x4FB);
    mem_alloc_at_dbg(usrTrkEnd + 0x10, heapStart - hdr - usrTrkEnd, (u32)sStrMainFile, 0x4FC);
}

/* gGfxContexts seen from main_sched_event_thread: the message, the task, and frame times. */
typedef struct {
    u8 pad[0x4C8];
    s16 msg[16];      /* 0x4C8 */
    u8 task[0x68];    /* 0x4E8 */
    u32 showTime;     /* 0x550 */
    u32 doneTime;     /* 0x554 */
    u32 frameTime;    /* 0x558 */
    u32 d;            /* 0x55C */
} GfxCtxTimes;

extern GfxCtxTimes gGfxContexts_2[2];

/* The scheduler's client thread: passes retraces (setting the VI dither filter from gDrawFlags) and finished
   graphics tasks on to the main thread, timing the frames. */
void main_sched_event_thread(void)
{
    s16 *msg;
    u32 fb;
    u32 now;

    for (;;) {
        osRecvMesg(&sSchedEventQueue, (OSMesg *)&msg, OS_MESG_BLOCK);
        switch (*msg) {
        case SC_MSG_RETRACE:
            sRetraceCount++;
            fb = sched_get_current_task() != (s32)gGfxContexts_2[0].task; /* the context being drawn */
            if (fb != sDisplayedFb) {
                gGfxContexts_2[fb].showTime = osGetCount();
            }
            sDisplayedFb = fb;
            if ((sViDitherFilter ^ gDrawFlags) & DRAW_DITHER_FILTER) {
                sViDitherFilter = gDrawFlags & DRAW_DITHER_FILTER;
                osViSetSpecialFeatures(sViDitherFilter
                    ? OS_VI_DITHER_FILTER_ON | OS_VI_DIVOT_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_GAMMA_OFF
                    : OS_VI_DITHER_FILTER_OFF | OS_VI_DIVOT_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_GAMMA_OFF);
            }
            if (MQ_GET_COUNT(&sMainQueue) < 14) {
                osSendMesg(&sMainQueue, msg, OS_MESG_NOBLOCK);
            }
            break;
        case SC_MSG_GFX_DONE:
            fb = msg != gGfxContexts_2[0].msg;
            sGfxDoneCount++;
            if (gNesActive != 0) {
                nes_draw_back_sprites(fb);
            } else {
                sun_check_occlusion(fb);
                sky_scan_zbuffer(fb);
            }
            now = osGetCount();
            gGfxContexts_2[fb].doneTime = now;
            gGfxContexts_2[fb].frameTime = now - gGfxContexts_2[fb].showTime;
            osSendMesg(&sMainQueue, msg, OS_MESG_BLOCK);
            break;
        }
    }
}

/* Writes the freeze flag to a development board port (PI 0x0FF70000). */
void main_set_debug_freeze(s32 freeze)
{
    u8 *handle = sDevPiHandle;
    s32 value = 0;
    if (freeze != 0) {
        value = -1;
    }
    osEPiWriteIo(handle, 0xFF70000, value);
    gDebugPadFreeze = freeze;
}

extern OSPiHandle sDevPiHandle_2;

/* Sets up a PI handle for the development board at 0xAFF70000 (domain 2) and adds it to the PI table. */
void main_init_dev_pi_handle(void)
{
    s32 mask;

    mem_fill((u8 *)&sDevPiHandle_2, 0, sizeof(OSPiHandle));
    sDevPiHandle_2.baseAddress = 0xAFF70000;
    sDevPiHandle_2.latency = 5;
    sDevPiHandle_2.pulse = 0xC;
    sDevPiHandle_2.pageSize = 0xD;
    sDevPiHandle_2.relDuration = 2;
    sDevPiHandle_2.type = DEVICE_TYPE_CART;
    sDevPiHandle_2.domain = PI_DOMAIN2;
    mask = __osDisableInt();
    sDevPiHandle_2.next = __osPiTable;
    __osPiTable = &sDevPiHandle_2;
    __osRestoreInt(mask);
}

void main_init_sched(void)
{
    osCreateMesgQueue(&sMainQueue, sMainMsgs, 0x10);
    sched_init(0xD, 1);
}

void main_hang(void)
{
    while (1) {}
}

/* Shows a white 320x240 screen and halts. */
void main_halt_white_screen(void)
{
    osCreateViManager(10);
    osViSetMode(osViModeTable);
    osViBlack(0);
    osViSwapBuffer((void *)0x80200000);
    osViBlack(0);
    osViSwapBuffer((void *)0x80200000);
    mem_fill((u8 *)0x80200000, 0xFF, 320 * 240 * 2);
    osWritebackDCacheAll();
    __asm__ volatile("break 0x407");
    while (1) {}
}

void main_on_dma_start(void)
{
}

void main_debug_newline(void) {
    debug_log();
    debug_log(&sStrCrLf);
}

void func_8000EA54(void)
{
}

/* Checks two words of the cartridge's IPL3 (0x6C07 at 0x574, 0x8965 at 0x578). Never called. */
s32 main_check_cart_ipl3(void)
{
    __asm__ volatile("addu $9, $30, $0" : : : "$9");
    while ((IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) != 0) {
    }
    if ((*(volatile u32 *)0xB0000574 & 0xFFFF) != 0x6C07) {
        return 0;
    }
    while ((IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) != 0) {
    }
    return (*(volatile u32 *)0xB0000578 & 0xFFFF) == 0x8965;
}
