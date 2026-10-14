#include "game.h"

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

typedef struct { u8 pad[8]; s16 arr[1]; } Trk_5;
typedef struct { Trk_5 *trk; u8 pad4[0x60]; s16 f64; } Race;

extern Race *gSeason_6;
extern Pad gContPads_2[4];
extern Pad gPadInputPrev_4[4];
extern Ctx gGfxContexts[2];
extern Ctx *gCurGfxContext;
extern Pad gPadInput_4[4];

void game_main(void)
{
    char buf[0x780];
    OSMesg msg;
    s32 st;
    s32 cfg;
    s32 sz;
    OSScTask *q = NULL;
    s32 i;
    u32 base;
    Pad *pads;
    GameState *next;
    GameState *cur;

    msg = NULL;
    cfg = *(u16 *)0x80000402;
    if (cfg == 0xDEAD) {
        gLoadedByBootLoader = 0;
    } else if (cfg == 0xBEEF) {
        gLoadedByBootLoader = 1;
    } else {
        for (;;) {
        }
    }
    if (gLoadedByBootLoader != 0) {
        __builtin_memcpy(gBootHeader, (void *)0x803FFFC0, 0x30);
        gResourceRomAddr = D_800E20D4;
        cfg = *(u16 *)0x80000406;
        if (cfg == 0xDEAF) {
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
    if (gHasExpansionPak != 0) {
        base = 0x80600000;
        gFbAllocWidth = 640;
        gFbAllocHeight = 480;
    } else {
        base = 0x80300000;
        gFbAllocWidth = 512;
        gFbAllocHeight = 240;
    }
    sz = gFbAllocWidth * gFbAllocHeight * 4 + 0x40;
    gFramebuffers_2[0] = mem_alloc_at_dbg(base - sz, sz - 0x10, D_800D6014, 361);
    gFramebuffers_2[1] = (u16 *)((u8 *)gFramebuffers_2[0] + gFbAllocWidth * gFbAllocHeight * 2);
    {
    void *z;
    gZBuffer = z = mem_alloc_at_dbg(base, gFbAllocWidth * gFbAllocHeight * 2, D_800D6014, 363);
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
    for (i = 0; i < 4; i++) {
        main_convert_pad_input(&gContPads_2[i], &gPadInput_4[i]);
    }
    gReplayBuffer = mem_alloc_dbg(0xC800, 0, D_800D6014, 405);
    sGfxDramStack = mem_alloc_dbg(0x401, 0, D_800D6014, 412);
    sGfxYieldBuffer = mem_alloc_dbg(0xC00, 0, D_800D6014, 413);
    file_init(gResourceRomAddr);
    res_set_load_buffer(0);
    gCurGameState = &gGameStates[0];
    gGfxContexts[0].dl = NULL;
    gGfxContexts[1].dl = NULL;
    func_8005B5E0();
    osCreateMesgQueue(&sSchedEventQueue, sSchedEventMsgs, 16);
    osCreateThread(&sSchedEventThread, 5, main_sched_event_thread, NULL, gPadInputPrev_4, 12);
    osStartThread(&sSchedEventThread);
    sched_add_client((s32)D_800E2040, (s32)&sSchedEventQueue);
    snd_init();
    gIsDrawing = 0;
    main_init_dev_pi_handle();
    func_8000E8B0(0);

    for (;;) {
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
        osRecvMesg(&sMainQueue, &msg, 1);
        switch (*(s16 *)msg) {
        case 1:
            if (gCurGameState->mode == 0) {
                if (gGfxContexts[0].dl == NULL) {
                    gGfxContexts[0].dl = mem_alloc_dbg(gCurGameState->dlSize * 8, 0, D_800D6014, 523);
                    gGfxContexts[1].dl = mem_alloc_dbg(gCurGameState->dlSize * 8, 0, D_800D6014, 524);
                }
                i = gCurGameState - gGameStates;
                if (i == 0 || i == 2) {
                    st = 1;
                } else if (i == 3) {
                    st = 0;
                } else if (i == 1) {
                    st = 1;
                }
                gViModeRequest = st;
                while (gViModeRequest != gViMode) {
                }
                mem_fill((u8 *)gFramebuffers_2[0], 0, gFbAllocWidth * gFbAllocHeight * 2);
                mem_fill((u8 *)gFramebuffers_2[1], 0, gFbAllocWidth * gFbAllocHeight * 2);
                gCurGameState->mode = 2;
                D_800C29E4 = 4;
                if (gGame.flags & 0x8000) {
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
            if ((gContPads_2[0].button & 0xC00) == 0xC00) {
                func_8000E8B0(1);
                do {
                    start_controller_read();
                    cont_finish_read();
                    cont_read_pads();
                } while ((gContPads_2[0].button & 0x300) != 0x300);
                func_8000E8B0(0);
                do {
                    start_controller_read();
                    cont_finish_read();
                    cont_read_pads();
                } while ((gContPads_2[0].button & 0x300) == 0x300);
            }
            start_controller_read();
            *(Blk *)gPadInputPrev_4 = *(Blk *)gPadInput_4;
            for (i = 0; i < 4; i++) {
                main_convert_pad_input(&gContPads_2[i], &gPadInput_4[i]);
            }
            pads = gPadInput_4;
            sAudioTickStart = osGetCount();
            snd_update();
            sAudioTickEnd = osGetCount();
            sAudioTickTime = sAudioTickEnd - sAudioTickStart;
            if (gFrameCounter - sLastUpdateFrame > 0 || gCurGameState->mode == 5) {
                if (gCurGameState->mode == 2) {
                    if (gCurGameState->update != NULL) {
                        gCurGameState->update((u32 *)pads);
                    }
                }
                if (gCurGameState->mode == 3) {
                    gCurGameState->mode = 5;
                    gCurGameState->wait = 10;
                }
                if (gCurGameState->mode == 5 && gCurGameState->wait == 0) {
                    gCurGameState->mode = 3;
                }
                if (gCurGameState->mode == 3) {
                    gCurGameState->mode = 4;
                    gCurGameState->exit(0);
                    i = gCurGameState - gGameStates;
                    gCurGameState->mode = 0;
                    if (i == 2) {
                        i = 0;
                        if (gGame.flags & 2) {
                            gRaceExitCode = 6;
                        } else {
                            gCurTrack = &gTracks[21];
                            i = 1;
                        }
                    } else if (i == 3) {
                        i = 0;
                    } else if (gGame.flags & 0x40) {
                        i = 3;
                    } else if (i == 0 && (gGame.flags & 0x20)) {
                        i = 2;
                    } else if (D_800CA924[0] > 0 && D_800CA924[1] < D_800CA924[0]) {
                        i = 1;
                    } else {
                        if (gGame.mode == 1 && gSeason_6 != NULL && !(gGame.flags & 0x8000)) {
                            gCurTrack = &gTracks[gSeason_6->trk->arr[gSeason_6->f64]];
                        }
                        if (i == 1) {
                            if (gGame.flags & 0x40000) {
                                s32 k;
                                for (k = 0; k < 6; k++) {
                                    gGame.riders[k] = gGame.savedRiders[k];
                                    gGame.riderSkins[k] = gGame.savedRiderSkins[k];
                                }
                            }
                        }
                        if (i == 1 && gRaceExitCode == 8) {
                            i = 2;
                        } else if (i == 1 && (gGame.flags & 0x808000) == 0x800000) {
                            gCreditsMode = 1;
                            gGame.mode = 0;
                            gGame.flags = (gGame.flags | 0x4000000) & 0xFF0BFF7F;
                            gRaceExitCode = 6;
                            gCurTrack = &gTracks[13];
                            gGame.numPlayers = 1;
                        } else if (!(gGame.flags & 4)) {
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
                if (gCurGameState->mode == 5) {
                    if (sGfxTasksPending == 0) {
                        gCurGameState->wait--;
                    }
                } else if (gCurGameState->mode == 2) {
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
                        for (;;) {
                        }
                    }
                    osWritebackDCacheAll();
                    t->list.t.data_ptr = (u64 *)gCurGfxContext->dl;
                    q = t;
#define T q
                    T->list.t.data_size = (sGfxHead - gCurGfxContext->dl) * sizeof(Gfx);
                    T->list.t.type = 1;
                    T->list.t.flags = 6;
                    T->list.t.ucode_boot = (u64 *)D_800A9C30;
                    T->list.t.ucode_boot_size = D_800A9D00 - D_800A9C30;
                    T->list.t.ucode = (u64 *)D_800AB0A0;
                    T->list.t.ucode_data = (u64 *)D_800D19E0;
                    T->list.t.ucode_data_size = 0x800;
                    T->list.t.dram_stack = sGfxDramStack;
                    T->list.t.dram_stack_size = 0x400;
                    T->list.t.output_buff = NULL;
                    T->list.t.output_buff_size = NULL;
                    T->list.t.yield_data_ptr = sGfxYieldBuffer;
                    T->list.t.yield_data_size = 0xC00;
                    T->next = NULL;
                    T->flags = 0x63;
                    T->msgQ = &sSchedEventQueue;
                    gGfxContexts[sGfxContextIndex].msgtype = 2;
                    T->msg = &gGfxContexts[sGfxContextIndex].msgtype;
                    T->framebuffer = gFramebuffers_2[sGfxContextIndex];
                    T->totalTime = 0;
                    sched_submit_task((s32)q);
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
        case 2:
            if (sScreenshotRequest != 0) {
                s32 x, y;
                __builtin_strcpy(buf, "e:\\log\\scr");
                sScreenshotCounter = gFrameCounter * 765 % 99999;
                int_to_str(sScreenshotCounter, buf + strlen(buf));
                str_cat(buf, ".raw");
                sScreenshotCounter++;
                for (y = 0; y < gScreenHeight; y++) {
                    for (x = 0; x < gScreenWidth; x++) {
                        s32 c = gFramebuffers_2[sLastDoneFb][y * gScreenWidth + x];
                        buf[x * 3 + 0] = (c >> 11) << 3;
                        buf[x * 3 + 1] = (c >> 6) << 3;
                        buf[x * 3 + 2] = (c >> 1) << 3;
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

void main_convert_pad_input(OSContPad *pad, u32 *out)
{
    s32 x;
    s32 y;
    u32 v;
    u32 u;
    u32 t;

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
        u = (-x >> 3) << 24;
        if (u != 0) {
            *out = u | 4;
        }
    } else if (x > 5) {
        u = (x >> 3) << 24;
        if (u != 0) {
            *out = u | 8;
        }
    }
    if (y < -5) {
        v = (-y >> 3) << 28;
        if (v == 0) {
            goto done;
        }
        t = *out | 2;
    } else {
        if (y < 6 || (v = (y >> 3) << 28) == 0) {
            goto done;
        }
        t = *out | 1;
    }
    *out = t | v;
done:
    if (pad->button & 0x8000) {
        *out |= 0x10;
    }
    if (pad->button & 0x4000) {
        *out |= 0x20;
    }
    if (pad->button & 0x1000) {
        *out |= 0x40;
    }
    if (pad->button & 8) {
        *out |= 0x80;
    }
    if (pad->button & 4) {
        *out |= 0x100;
    }
    if (pad->button & 2) {
        *out |= 0x200;
    }
    if (pad->button & 1) {
        *out |= 0x400;
    }
    if (pad->button & 0x800) {
        *out |= 0x800;
    }
    if (pad->button & 0x400) {
        *out |= 0x1000;
    }
    if (pad->button & 0x200) {
        *out |= 0x2000;
    }
    if (pad->button & 0x100) {
        *out |= 0x4000;
    }
    if (pad->button & 0x20) {
        *out |= 0x8000;
    }
    if (pad->button & 0x10) {
        *out |= 0x10000;
    }
    if (pad->button & 0x2000) {
        *out |= 0x20000;
    }
}

void main_init_memory(void)
{
    u32 top;
    u32 s0;
    u32 s2;
    u32 s3;
    u32 s1;
    u32 t;

    top = ((u32)osMemSize | 0x80000000) - 0x100;
    s0 = (u32)D_800731D0;
    s2 = (u32)D_800A42C0;
    s3 = (u32)D_800A9C20;
    if (s0 & 0xF) {
        fatal_halt(0x33);
    }
    if (s2 & 0xF) {
        fatal_halt(0x34);
    }
    if (s3 & 0xF) {
        fatal_halt(0x35);
    }
    s1 = (u32)D_80105680;
    s1 = (s1 + 0xFF) & ~0xFF;
    osViBlack(1);
    mem_fill((u8 *)s1, 0, top - s1);
    mem_init(0, 0, s0, top - s0);
    t = 0x30;
    mem_alloc_at_dbg(s0 + 0x30, s2 - t - s0, (u32)D_800D6014, 0x4FA);
    t = 0x10;
    mem_alloc_at_dbg(s2 + 0x10, s3 - t - s2, (u32)D_800D6014, 0x4FB);
    mem_alloc_at_dbg(s3 + 0x10, s1 - t - s3, (u32)D_800D6014, 0x4FC);
}

typedef struct {
    u8 pad[0x4C8];
    s16 msg[16];      /* 0x4C8 */
    u8 f4E8[0x68];    /* 0x4E8 */
    u32 a;            /* 0x550 */
    u32 b;            /* 0x554 */
    u32 c;            /* 0x558 */
    u32 d;            /* 0x55C */
} Task;

extern Task gGfxContexts_2[2];

void main_sched_event_thread(void)
{
    s16 *msg;
    u32 idx;
    u32 t;
    u32 v;

    for (;;) {
        osRecvMesg(&sSchedEventQueue, (OSMesg *)&msg, 1);
        switch (*msg) {
        case 1:
            sRetraceCount++;
            idx = sched_get_current_task() != (s32)gGfxContexts_2[0].f4E8;
            if (idx != sDisplayedFb) {
                gGfxContexts_2[idx].a = osGetCount();
            }
            sDisplayedFb = idx;
            if ((sViDitherFilter ^ gDrawFlags) & 0x1000) {
                sViDitherFilter = gDrawFlags & 0x1000;
                osViSetSpecialFeatures(sViDitherFilter ? 0x6A : 0xAA);
            }
            if (MQ_GET_COUNT(&sMainQueue) < 14) {
                osSendMesg(&sMainQueue, msg, 0);
            }
            break;
        case 2:
            idx = msg != gGfxContexts_2[0].msg;
            sGfxDoneCount++;
            if (gNesActive != 0) {
                nes_draw_back_sprites(idx);
            } else {
                sun_check_occlusion(idx);
                sky_scan_zbuffer(idx);
            }
            t = osGetCount();
            gGfxContexts_2[idx].b = t;
            gGfxContexts_2[idx].c = t - gGfxContexts_2[idx].a;
            osSendMesg(&sMainQueue, msg, 1);
            break;
        }
    }
}

void func_8000E8B0(s32 param_1)
{
    u8 *p = sDevPiHandle;
    s32 flag = 0;
    if (param_1 != 0) {
        flag = -1;
    }
    osEPiWriteIo(p, 0xFF70000, flag);
    D_800C2930 = param_1;
}

typedef struct {
    void *next;
    u8 f4, f5, f6, f7, f8, f9;
    u8 pad[2];
    u32 fc;
} Obj_4;

extern Obj_4 sDevPiHandle_2;

void main_init_dev_pi_handle(void)
{
    s32 r;

    mem_fill((u8 *)&sDevPiHandle_2, 0, 0x74);
    sDevPiHandle_2.fc = 0xAFF70000;
    sDevPiHandle_2.f5 = 5;
    sDevPiHandle_2.f8 = 0xC;
    sDevPiHandle_2.f6 = 0xD;
    sDevPiHandle_2.f7 = 2;
    sDevPiHandle_2.f4 = 0;
    sDevPiHandle_2.f9 = 1;
    r = __osDisableInt();
    sDevPiHandle_2.next = __osPiTable;
    __osPiTable = &sDevPiHandle_2;
    __osRestoreInt(r);
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

void func_8000E9C0(void)
{
    osCreateViManager(10);
    osViSetMode(osViModeTable);
    osViBlack(0);
    osViSwapBuffer((void *)0x80200000);
    osViBlack(0);
    osViSwapBuffer((void *)0x80200000);
    mem_fill((u8 *)0x80200000, 0xFF, 0x25800);
    osWritebackDCacheAll();
    __asm__ volatile("break 0x407");
    while (1) {}
}

void func_8000EA24(void)
{
}

void func_8000EA2C(void) {
    debug_log();
    debug_log(&D_800D6010);
}

void func_8000EA54(void)
{
}

s32 func_8000EA5C(void)
{
    __asm__ volatile("addu $9, $30, $0" : : : "$9");
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    if ((*(volatile u32 *)0xB0000574 & 0xFFFF) != 0x6C07) {
        return 0;
    }
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return (*(volatile u32 *)0xB0000578 & 0xFFFF) == 0x8965;
}
