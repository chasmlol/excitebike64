#include "game.h"

void sched_init(u8 *param_1, int arg1)
{
    mem_fill(sSchedClients, 0, 0x10);
    mem_fill(sSchedCurTask, 0, 8);
    sSchedRdpIdle = -1;
    sSchedRetraceMsg = 1;
    sSchedSwapDeferred = 0;
    gSchedMinRetraces = 1;
    gSchedFrameLimit = 0;
    osCreateMesgQueue(&sSchedSysEventQueue, sSchedSysEventMsgs, 8);
    osCreateMesgQueue(&sSchedTaskQueue, sSchedTaskMsgs, 8);
    osCreateViManager(param_1 + 10);
    sched_init_vi_mode(0);
    sched_init_vi_mode(1);
    sched_init_vi_mode(2);
    sched_set_vi_mode(1);
    osViBlack(1);
    osViSetSpecialFeatures(0xAA);
    osViSwapBuffer(0x80200000);
    osViSwapBuffer(0x80200000);
    osViRepeatLine(0);
    osCreateThread(&sSchedThread, 1, sched_thread, 0, sSchedRetracesSinceTask, param_1);
    osStartThread(&sSchedThread);
}

typedef struct Item_5 {
    u8 pad0[0xC];
    s32 f0C;
    u8 pad10[0x40];
    s32 f50;
    s32 f54;
} Item_5;

typedef struct CurNext {
    Item_5 *cur;
    Item_5 *next;
} CurNext;

typedef struct Pair_8 {
    s32 a;
    s32 b;
} Pair_8;

extern CurNext sSchedCurTask_2;
extern Pair_8 D_800F45F0[];

void sched_thread(void)
{
    s32 msg;
    s32 n;
    s32 i;
    Pair_8 *p;
    Pair_8 *base;

    osViSetEvent(&sSchedSysEventQueue, 1, 1);
    osSetEventMesg(9, &sSchedSysEventQueue, 2);
    osSetEventMesg(0xE, &sSchedSysEventQueue, 3);
    if (D_800F4780 == 0) {
        osSetEventMesg(0xC, &sSchedSysEventQueue, 4);
        osSetEventMesg(0xA, &sSchedSysEventQueue, 4);
    }

    while (1) {
        osRecvMesg(&sSchedSysEventQueue, &msg, 1);
        if (msg == 1) {
            if (D_800C2930 != 0) {
                continue;
            }
            D_800F11F4 = osViGetCurrentLine();
            sSchedRetraceCount++;
            if (gViModeRequest != gViMode) {
                sched_set_vi_mode(gViModeRequest);
            }
            sSchedRetracesSinceTask_2++;
            if (sSchedRdpIdle != 0) {
                if (sSchedCurTask_2.next == NULL) {
                    if (osRecvMesg(&sSchedTaskQueue, &msg, 0) == 0) {
                        sSchedCurTask_2.next = (Item_5 *)msg;
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
                osSendMesg(&sSchedSysEventQueue, 2, 0);
                sSchedSwapDeferred = 0;
            }
        } else if (msg == 2) {
            n = osViGetCurrentLine();
            if (n < 10 || sSchedSysEventQueue.validCount == 1) {
                osSendMesg(&sSchedSysEventQueue, msg, 0);
                continue;
            }
            if (sSchedRetracesSinceTask_2 < gSchedMinRetraces && gSchedFrameLimit != 0) {
                sSchedSwapDeferred = 1;
                continue;
            }
            if (D_800F141C < n) {
                D_800F141C = n;
            }
            if (n < D_800F1418) {
                D_800F1418 = n;
            }
            sSchedRdpIdle = 1;
            osViSwapBuffer(sSchedCurTask_2.cur->f0C);
            if (sViBlackFrames != 0) {
                if (--sViBlackFrames == 0) {
                    osViBlack(0);
                }
            }
            osSendMesg(sSchedCurTask_2.cur->f50, sSchedCurTask_2.cur->f54, 0);
        } else if (msg == 3) {
            osViBlack(1);
            sched_set_vi_mode(0);
            osViBlack(1);
            osViSetYScale(1.0f);
            cont_finish_read();
            for (i = 0; i < 4; i++) {
                rumble_start(i, -1);
            }
            gRumbleResetRequest = 1;
            start_controller_read();
            cont_finish_read();
            for (;;) ;
        } else if (msg == 4) {
            base = D_800F45F0;
            sched_set_vi_mode(0);
            mem_fill((u8 *)gFramebuffers_3, 0, 0x25800);
            osWritebackDCacheAll();
            osViSwapBuffer(gFramebuffers_3);
            osViBlack(0);
            if (!(gGame & 0x20000000)) {
                for (;;) ;
            }
            fault_draw_hex(0x32, 0x1E, D_800F46EC);
            for (i = 0; i < 0x1F; i++) {
                fault_draw_hex((i & 1) * 150 + 0x32, (i / 2) * 10 + 0x28, base[i].b);
            }
            osWritebackDCacheAll();
            for (;;) ;
        }
    }
}

typedef struct {
    s32 f0[2];
    s32 f8;
    s32 fC[5];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C[4];
    s32 f3C;
    s32 f40[4];
} Rec_37;

extern Rec_37 sViModes[];

void sched_init_vi_mode(s32 p)
{
    Rec_37 *s;
    Rec_37 *b;
    s32 off;
    Rec_37 *base;

    if (osTvType == 1) {
        if (p == 2) {
            s = &osViModeNtscHpf1;
        } else {
            s = &osViModeNtscLpn1;
        }
    } else if (osTvType == 2) {
        if (p == 2) {
            s = &osViModeMpalHpf1;
        } else {
            s = &osViModeMpalLpn1;
        }
    } else {
        main_hang();
    }
    sViModes[p] = *s;
    off = p * 0x50;
    base = sViModes;
    if (p != 2) {
        b = (Rec_37 *)(off + (s32)base);
        b->f8 = sViModeWidths[p];
        b->f20 = (sViModeWidths[p] << 9) / 320;
        b->f28 = sViModeWidths[p] << 1;
        b->f3C = sViModeWidths[p] << 1;
    }
}

void sched_add_client(s32 a0, s32 a1)
{
    s32 i;
    s32 *p;

    i = 0;
    p = sSchedClients;
    do {
        i = i + 1;
        if (*p == 0) {
            *p = a1;
            return;
        }
        p = p + 1;
    } while (i < 4);
}

void sched_submit_task(s32 a0)
{
    osSendMesg(&sSchedTaskQueue, a0, 1);
}

s32 sched_get_current_task(void) {
    return sSchedCurTask_3;
}

void sched_notify_clients(void *arg)
{
    s32 i;
    u32 *p;
    for (i = 0, p = &sSchedClients; i < 4; i++, p++) {
        if (p[0] != 0) {
            osSendMesg(p[0], arg, 0);
        }
    }
}

void sched_run_task(u8 *param_1) {
    void *p = param_1 + 0x10;
    sSchedRdpIdle = 0;
    osSpTaskLoad(p);
    osSpTaskStartGo(p);
}

void sched_set_vi_mode(s32 p)
{
    osViBlack(1);
    sViBlackFrames = 3;
    osWritebackDCache(0, 0x800000);
    osViSetMode(sViModes_2 + p * 0x50);
    osViSetSpecialFeatures(0xA2);
    *(volatile s32 *)&gScreenWidth = sViModeWidths[p];
    *(volatile s32 *)&gScreenHeight = sViModeHeights[p];
    *(volatile s32 *)&gViMode = p;
    osViBlack(1);
}

void fault_draw_hex(s32 x, s32 y, u32 bits)
{
    s32 i, j, k;
    u8 *r;
    u16 *p;

    for (i = 0; i < 8; i++) {
        r = sFaultHexFont + ((bits >> ((7 - i) * 4)) & 0xF) * 8;
        p = gFramebuffers_4 + (y * 0x140 + x);
        for (j = 0; j < 8; j++) {
            k = 7;
            do {
                if (((*r >> k) & 1) != 0) {
                    *p = 0xFFFF;
                } else {
                    *p = 1;
                }
                k--;
                p++;
            } while (k >= 0);
            r++;
            p += 0x140 - 8;
        }
        x += 8;
    }
}

void func_80065A5C(void)
{
}

void *sched_get_os_vi_mode(s32 a0) {
    void *s0;
    if (osTvType == 1) {
        if (a0 == 2) {
            s0 = osViModeNtscHpf1_2;
        } else {
            s0 = osViModeNtscLpn1_2;
        }
    } else if (osTvType == 2) {
        if (a0 == 2) {
            s0 = osViModeMpalHpf1_2;
        } else {
            s0 = osViModeMpalLpn1_2;
        }
    } else {
        main_hang();
    }
    return s0;
}
