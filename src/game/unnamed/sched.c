#include "game.h"

/* The scheduler thread: it tells up to four client queues of each VI retrace, starts the graphics tasks main.c
   submits, shows a task's framebuffer once the RDP is done with it, handles PRE-NMI and CPU faults, and switches
   between the three VI modes (VI_MODE_*) when gViModeRequest changes. */

/* Sets up the scheduler: no clients and no task, its two queues, the VI manager and the three VI modes, a black
   512x240 screen, then starts the scheduler thread. main.c calls it with priority 13 and 1. */
void sched_init(u8 *priority, int unused)
{
    mem_fill(sSchedClients, 0, SCHED_MAX_CLIENTS * 4);
    mem_fill(sSchedCurTask, 0, 8);      /* the current and the next task */
    sSchedRdpIdle = -1;
    sSchedRetraceMsg = SCHED_CLIENT_RETRACE;
    sSchedSwapDeferred = 0;
    gSchedMinRetraces = 1;
    gSchedFrameLimit = 0;
    osCreateMesgQueue(&sSchedSysEventQueue, sSchedSysEventMsgs, 8);
    osCreateMesgQueue(&sSchedTaskQueue, sSchedTaskMsgs, 8);
    osCreateViManager(priority + 10);
    sched_init_vi_mode(VI_MODE_LO_RES);
    sched_init_vi_mode(VI_MODE_MED_RES);
    sched_init_vi_mode(VI_MODE_HI_RES);
    sched_set_vi_mode(VI_MODE_MED_RES);
    osViBlack(TRUE);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_DIVOT_OFF | OS_VI_DITHER_FILTER_OFF);
    osViSwapBuffer(0x80200000);
    osViSwapBuffer(0x80200000);
    osViRepeatLine(FALSE);
    /* The thread's stack ends where sSchedRetracesSinceTask is. */
    osCreateThread(&sSchedThread, 1, sched_thread, 0, sSchedRetracesSinceTask, priority);
    osStartThread(&sSchedThread);
}

/* A task main.c submits: libultra's OSScTask. */
typedef struct SchedTask {
    /* 0x00 */ u8 pad0[0xC];    /* next, state, flags */
    /* 0x0C */ s32 framebuffer;
    /* 0x10 */ u8 list[0x40];   /* the OSTask */
    /* 0x50 */ s32 msgQ;        /* told when the framebuffer is shown */
    /* 0x54 */ s32 msg;
} SchedTask;

/* sSchedCurTask: the task the RCP runs and the next one, taken from sSchedTaskQueue. */
typedef struct SchedTaskSlots {
    SchedTask *cur;
    SchedTask *next;
} SchedTaskSlots;

/* A saved 64-bit register of an OSThread's context. */
typedef struct RegPair {
    s32 hi;
    s32 lo;
} RegPair;

extern SchedTaskSlots sSchedCurTask_2;

/* The scheduler thread. On a retrace it starts the next task if the RDP is idle and tells the clients; when the RDP
   is done it shows the task's framebuffer (on the next retrace if the frame limit asks for more retraces per frame)
   and sends the task's message; on PRE-NMI it blanks the screen, stops the Rumble Paks and hangs; on a CPU fault
   with the Debug cheat on it draws the main thread's pc and registers in hex, and hangs. */
void sched_thread(void)
{
    s32 msg;
    s32 line;
    s32 i;
    RegPair *unused;
    RegPair *regs;

    osViSetEvent(&sSchedSysEventQueue, SCHED_MSG_RETRACE, 1);
    osSetEventMesg(OS_EVENT_DP, &sSchedSysEventQueue, SCHED_MSG_RDP_DONE);
    osSetEventMesg(OS_EVENT_PRENMI, &sSchedSysEventQueue, SCHED_MSG_PRENMI);
    if (D_800F4780 == 0) {
        osSetEventMesg(OS_EVENT_FAULT, &sSchedSysEventQueue, SCHED_MSG_FAULT);
        osSetEventMesg(OS_EVENT_CPU_BREAK, &sSchedSysEventQueue, SCHED_MSG_FAULT);
    }

    while (1) {
        osRecvMesg(&sSchedSysEventQueue, &msg, OS_MESG_BLOCK);
        if (msg == SCHED_MSG_RETRACE) {
            if (gDebugPadFreeze != 0) {
                continue;
            }
            sSchedRetraceLine = osViGetCurrentLine();
            sSchedRetraceCount++;
            if (gViModeRequest != gViMode) {
                sched_set_vi_mode(gViModeRequest);
            }
            sSchedRetracesSinceTask_2++;
            if (sSchedRdpIdle != 0) {
                if (sSchedCurTask_2.next == NULL) {
                    if (osRecvMesg(&sSchedTaskQueue, &msg, OS_MESG_NOBLOCK) == 0) {
                        sSchedCurTask_2.next = (SchedTask *)msg;
                    }
                }
                sSchedCurTask_2.cur = sSchedCurTask_2.next;
                sSchedCurTask_2.next = NULL;
                if (sSchedCurTask_2.cur != NULL) {
                    sched_run_task((u8 *)sSchedCurTask_2.cur);
                    sSchedRetracesSinceTask_2 = 0;
                }
            }
            sched_notify_clients(&sSchedRetraceMsg);
            if (sSchedSwapDeferred != 0) {
                osSendMesg(&sSchedSysEventQueue, SCHED_MSG_RDP_DONE, OS_MESG_NOBLOCK);
                sSchedSwapDeferred = 0;
            }
        } else if (msg == SCHED_MSG_RDP_DONE) {
            /* Too close to the top of the screen, or nothing else queued: try again later. */
            line = osViGetCurrentLine();
            if (line < 10 || sSchedSysEventQueue.validCount == 1) {
                osSendMesg(&sSchedSysEventQueue, msg, OS_MESG_NOBLOCK);
                continue;
            }
            if (sSchedRetracesSinceTask_2 < gSchedMinRetraces && gSchedFrameLimit != 0) {
                sSchedSwapDeferred = 1;
                continue;
            }
            if (sSchedSwapLineMax < line) {
                sSchedSwapLineMax = line;
            }
            if (line < sSchedSwapLineMin) {
                sSchedSwapLineMin = line;
            }
            sSchedRdpIdle = 1;
            osViSwapBuffer(sSchedCurTask_2.cur->framebuffer);
            if (sViBlackFrames != 0) {
                if (--sViBlackFrames == 0) {
                    osViBlack(FALSE);
                }
            }
            osSendMesg(sSchedCurTask_2.cur->msgQ, sSchedCurTask_2.cur->msg, OS_MESG_NOBLOCK);
        } else if (msg == SCHED_MSG_PRENMI) {
            osViBlack(TRUE);
            sched_set_vi_mode(VI_MODE_LO_RES);
            osViBlack(TRUE);
            osViSetYScale(1.0f);
            cont_finish_read();
            for (i = 0; i < MAXCONTROLLERS; i++) {
                rumble_start(i, RUMBLE_OFF);
            }
            gRumbleResetRequest = 1;
            start_controller_read();
            cont_finish_read();
            for (;;) ;
        } else if (msg == SCHED_MSG_FAULT) {
            regs = (RegPair *)&gMainThread.context;  /* the registers as u64 pairs: .lo is the low word */
            sched_set_vi_mode(VI_MODE_LO_RES);
            mem_fill((u8 *)gFramebuffers_3, 0, FB_320X240_SIZE);
            osWritebackDCacheAll();
            osViSwapBuffer(gFramebuffers_3);
            osViBlack(FALSE);
            if (!(gGame.flags & GAME_FLAG_DEBUG)) {
                for (;;) ;
            }
            fault_draw_hex(0x32, 0x1E, *(u32 *)((u32)&gMainThread + 0x11C));  /* gMainThread.context.pc, as a scalar */
            /* The 31 registers from at to ra, in two columns. */
            for (i = 0; i < 0x1F; i++) {
                fault_draw_hex((i & 1) * 150 + 0x32, (i / 2) * 10 + 0x28, regs[i].lo);
            }
            osWritebackDCacheAll();
            for (;;) ;
        }
    }
}

/* libultra's OSViMode as words (0x50 bytes), with the fields the game changes named. */
typedef struct {
    /* 0x00 */ s32 typeAndCtrl[2];  /* type, comRegs.ctrl */
    /* 0x08 */ s32 width;           /* comRegs.width */
    /* 0x0C */ s32 comRegs0C[5];    /* burst, vSync, hSync, leap, hStart */
    /* 0x20 */ s32 xScale;          /* 2.10 fixed point */
    /* 0x24 */ s32 vCurrent;
    /* 0x28 */ s32 origin0;         /* fldRegs[0].origin: byte offset of the first line shown */
    /* 0x2C */ s32 fldRegs0[4];     /* yScale, vStart, vBurst, vIntr */
    /* 0x3C */ s32 origin1;         /* fldRegs[1].origin */
    /* 0x40 */ s32 fldRegs1[4];
} ViMode;

extern ViMode sViModes[];

/* Builds sViModes[mode] from libultra's mode for the TV type (NTSC or MPAL; anything else hangs): the high-resolution
   interlaced mode for VI_MODE_HI_RES, else the low-resolution one with its width and x scale set for
   sViModeWidths[mode]. */
void sched_init_vi_mode(s32 mode)
{
    ViMode *osMode;
    ViMode *vm;
    s32 off;
    ViMode *base;

    if (osTvType == OS_TV_NTSC) {
        if (mode == VI_MODE_HI_RES) {
            osMode = &osViModeNtscHpf1;
        } else {
            osMode = &osViModeNtscLpn1;
        }
    } else if (osTvType == OS_TV_MPAL) {
        if (mode == VI_MODE_HI_RES) {
            osMode = &osViModeMpalHpf1;
        } else {
            osMode = &osViModeMpalLpn1;
        }
    } else {
        main_hang();
    }
    sViModes[mode] = *osMode;
    off = mode * 0x50;      /* sizeof(OSViMode) */
    base = sViModes;
    if (mode != VI_MODE_HI_RES) {
        vm = (ViMode *)(off + (s32)base);
        vm->width = sViModeWidths[mode];
        vm->xScale = (sViModeWidths[mode] << 9) / 320;    /* 0x200 (1.0) for 320 pixels */
        vm->origin0 = sViModeWidths[mode] << 1;
        vm->origin1 = sViModeWidths[mode] << 1;
    }
}

/* Registers a queue to be sent sSchedRetraceMsg on each retrace, in the first free of the four slots; nothing
   happens when all are taken. The first argument is not used. */
void sched_add_client(s32 client, s32 queue)
{
    s32 i;
    s32 *slot;

    i = 0;
    slot = sSchedClients;
    do {
        i = i + 1;
        if (*slot == 0) {
            *slot = queue;
            return;
        }
        slot = slot + 1;
    } while (i < SCHED_MAX_CLIENTS);
}

/* Queues a graphics task (an OSScTask); it starts on a retrace once the RDP is idle. */
void sched_submit_task(s32 task)
{
    osSendMesg(&sSchedTaskQueue, task, OS_MESG_BLOCK);
}

/* The current graphics task (sSchedCurTask.cur); it goes back to 0 on the first retrace after the RDP is done
   that finds no task queued. */
s32 sched_get_current_task(void)
{
    return sSchedCurTask_3;
}

/* Sends msg to every registered client queue. */
void sched_notify_clients(void *msg)
{
    s32 i;
    u32 *slot;
    for (i = 0, slot = &sSchedClients; i < SCHED_MAX_CLIENTS; i++, slot++) {
        if (slot[0] != 0) {
            osSendMesg(slot[0], msg, OS_MESG_NOBLOCK);
        }
    }
}

/* Loads and starts a task's OSTask (at +0x10 of the OSScTask) on the RSP. */
void sched_run_task(u8 *task)
{
    void *spTask = task + 0x10;
    sSchedRdpIdle = 0;
    osSpTaskLoad(spTask);
    osSpTaskStartGo(spTask);
}

/* Switches the VI to one of sViModes and sets gScreenWidth, gScreenHeight and gViMode; the screen stays black for
   the next three framebuffer swaps. */
void sched_set_vi_mode(s32 mode)
{
    osViBlack(TRUE);
    sViBlackFrames = 3;
    osWritebackDCache(0, 0x800000);     /* all of RDRAM */
    osViSetMode(sViModes_2 + mode * 0x50);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_DIVOT_OFF | OS_VI_DITHER_FILTER_OFF);
    *(volatile s32 *)&gScreenWidth = sViModeWidths[mode];
    *(volatile s32 *)&gScreenHeight = sViModeHeights[mode];
    *(volatile s32 *)&gViMode = mode;
    osViBlack(TRUE);
}

/* Draws a word as eight hex digits (8x8 glyphs of sFaultHexFont, white on black) into the 320-pixel-wide fault
   framebuffer at x, y. */
void fault_draw_hex(s32 x, s32 y, u32 value)
{
    s32 digit, row, bit;
    u8 *glyph;
    u16 *pixel;

    for (digit = 0; digit < 8; digit++) {
        glyph = sFaultHexFont + ((value >> ((7 - digit) * 4)) & 0xF) * 8;
        pixel = gFramebuffers_4 + (y * FAULT_FB_WIDTH + x);
        for (row = 0; row < 8; row++) {
            bit = 7;
            do {
                if (((*glyph >> bit) & 1) != 0) {
                    *pixel = 0xFFFF;
                } else {
                    *pixel = 1;
                }
                bit--;
                pixel++;
            } while (bit >= 0);
            glyph++;
            pixel += FAULT_FB_WIDTH - 8;
        }
        x += 8;
    }
}

/* Empty. */
void func_80065A5C(void)
{
}

/* libultra's VI mode behind one of the game's modes for the TV type (NTSC or MPAL; anything else hangs). */
void *sched_get_os_vi_mode(s32 mode)
{
    void *osMode;
    if (osTvType == OS_TV_NTSC) {
        if (mode == VI_MODE_HI_RES) {
            osMode = osViModeNtscHpf1_2;
        } else {
            osMode = osViModeNtscLpn1_2;
        }
    } else if (osTvType == OS_TV_MPAL) {
        if (mode == VI_MODE_HI_RES) {
            osMode = osViModeMpalHpf1_2;
        } else {
            osMode = osViModeMpalLpn1_2;
        }
    } else {
        main_hang();
    }
    return osMode;
}
