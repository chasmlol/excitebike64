#include "game.h"

void func_800651A0(u8 *param_1, int arg1)
{
    func_8000ED10(D_800F13A8, 0, 0x10);
    func_8000ED10(D_800F0DC8, 0, 8);
    D_800F13E8 = -1;
    D_800F0DA8 = 1;
    D_800CCCC8 = 0;
    D_800F0DA0 = 1;
    D_800CCCCC = 0;
    osCreateMesgQueue(&D_800F13B8, D_800F13F8, 8);
    osCreateMesgQueue(&D_800F13D0, D_800F0DD0, 8);
    osCreateViManager(param_1 + 10);
    func_800656B4(0);
    func_800656B4(1);
    func_800656B4(2);
    func_80065904(1);
    osViBlack(1);
    osViSetSpecialFeatures(0xAA);
    osViSwapBuffer(0x80200000);
    osViSwapBuffer(0x80200000);
    osViRepeatLine(0);
    osCreateThread(&D_800F11F8, 1, func_800652DC, 0, D_800F11F0, param_1);
    osStartThread(&D_800F11F8);
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

extern CurNext D_800F0DC8_2;
extern Pair_8 D_800F45F0[];

void func_800652DC(void)
{
    s32 msg;
    s32 n;
    s32 i;
    Pair_8 *p;
    Pair_8 *base;

    osViSetEvent(&D_800F13B8, 1, 1);
    osSetEventMesg(9, &D_800F13B8, 2);
    osSetEventMesg(0xE, &D_800F13B8, 3);
    if (D_800F4780 == 0) {
        osSetEventMesg(0xC, &D_800F13B8, 4);
        osSetEventMesg(0xA, &D_800F13B8, 4);
    }

    while (1) {
        osRecvMesg(&D_800F13B8, &msg, 1);
        if (msg == 1) {
            if (D_800C2930 != 0) {
                continue;
            }
            D_800F11F4 = osViGetCurrentLine();
            D_800CCCC4++;
            if (D_800CCCA4 != D_800CCCA8) {
                func_80065904(D_800CCCA4);
            }
            D_800F11F0_2++;
            if (D_800F13E8 != 0) {
                if (D_800F0DC8_2.next == NULL) {
                    if (osRecvMesg(&D_800F13D0, &msg, 0) == 0) {
                        D_800F0DC8_2.next = (Item_5 *)msg;
                    }
                }
                D_800F0DC8_2.cur = D_800F0DC8_2.next;
                D_800F0DC8_2.next = NULL;
                if (D_800F0DC8_2.cur != NULL) {
                    func_800658CC((u8 *)D_800F0DC8_2.cur);
                    D_800F11F0_2 = 0;
                }
            }
            func_80065868(&D_800F0DA8);
            if (D_800CCCC8 != 0) {
                osSendMesg(&D_800F13B8, 2, 0);
                D_800CCCC8 = 0;
            }
        } else if (msg == 2) {
            n = osViGetCurrentLine();
            if (n < 10 || D_800F13B8.validCount == 1) {
                osSendMesg(&D_800F13B8, msg, 0);
                continue;
            }
            if (D_800F11F0_2 < D_800F0DA0 && D_800CCCCC != 0) {
                D_800CCCC8 = 1;
                continue;
            }
            if (D_800F141C < n) {
                D_800F141C = n;
            }
            if (n < D_800F1418) {
                D_800F1418 = n;
            }
            D_800F13E8 = 1;
            osViSwapBuffer(D_800F0DC8_2.cur->f0C);
            if (D_800CCCA0 != 0) {
                if (--D_800CCCA0 == 0) {
                    osViBlack(0);
                }
            }
            osSendMesg(D_800F0DC8_2.cur->f50, D_800F0DC8_2.cur->f54, 0);
        } else if (msg == 3) {
            osViBlack(1);
            func_80065904(0);
            osViBlack(1);
            osViSetYScale(1.0f);
            func_8000CD00();
            for (i = 0; i < 4; i++) {
                func_8000D0A0(i, -1);
            }
            D_800C2924 = 1;
            start_controller_read();
            func_8000CD00();
            for (;;) ;
        } else if (msg == 4) {
            base = D_800F45F0;
            func_80065904(0);
            func_8000ED10((u8 *)D_800C29D8_3, 0, 0x25800);
            osWritebackDCacheAll();
            osViSwapBuffer(D_800C29D8_3);
            osViBlack(0);
            if (!(D_800EFD70 & 0x20000000)) {
                for (;;) ;
            }
            func_800659B4(0x32, 0x1E, D_800F46EC);
            for (i = 0; i < 0x1F; i++) {
                func_800659B4((i & 1) * 150 + 0x32, (i / 2) * 10 + 0x28, base[i].b);
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

extern Rec_37 D_800F0CB0[];

void func_800656B4(s32 p)
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
        func_8000E9B8();
    }
    D_800F0CB0[p] = *s;
    off = p * 0x50;
    base = D_800F0CB0;
    if (p != 2) {
        b = (Rec_37 *)(off + (s32)base);
        b->f8 = D_800CCCAC[p];
        b->f20 = (D_800CCCAC[p] << 9) / 320;
        b->f28 = D_800CCCAC[p] << 1;
        b->f3C = D_800CCCAC[p] << 1;
    }
}

void func_800657FC(s32 a0, s32 a1)
{
    s32 i;
    s32 *p;

    i = 0;
    p = D_800F13A8;
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
    osSendMesg(&D_800F13D0, a0, 1);
}

s32 func_80065858(void) {
    return D_800F0DC8_3;
}

void func_80065868(void *arg)
{
    s32 i;
    u32 *p;
    for (i = 0, p = &D_800F13A8; i < 4; i++, p++) {
        if (p[0] != 0) {
            osSendMesg(p[0], arg, 0);
        }
    }
}

void func_800658CC(u8 *param_1) {
    void *p = param_1 + 0x10;
    D_800F13E8 = 0;
    osSpTaskLoad(p);
    osSpTaskStartGo(p);
}

void func_80065904(s32 p)
{
    osViBlack(1);
    D_800CCCA0 = 3;
    osWritebackDCache(0, 0x800000);
    osViSetMode(D_800F0CB0_2 + p * 0x50);
    osViSetSpecialFeatures(0xA2);
    *(volatile s32 *)&D_800F13EC = D_800CCCAC[p];
    *(volatile s32 *)&D_800F13F0 = D_800CCCB8[p];
    *(volatile s32 *)&D_800CCCA8 = p;
    osViBlack(1);
}

void func_800659B4(s32 x, s32 y, u32 bits)
{
    s32 i, j, k;
    u8 *r;
    u16 *p;

    for (i = 0; i < 8; i++) {
        r = D_800CCC20 + ((bits >> ((7 - i) * 4)) & 0xF) * 8;
        p = D_800C29D8_4 + (y * 0x140 + x);
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

void *func_80065A64(s32 a0) {
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
        func_8000E9B8();
    }
    return s0;
}
