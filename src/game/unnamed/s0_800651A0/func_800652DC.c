#include "ctx.h"

extern void func_800B2A50();
extern void func_800B1280();
extern s32 func_800B0610();
extern s32 func_800B2540();
extern void func_800B1150();
extern void func_800B2CC0();
extern void func_800B3020();
extern void func_800B2C70(f32);
extern void func_8000CD00(void);
extern void func_800AD990();
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);
extern void func_8000D0A0(u32 a0, s32 a1);
extern void func_800658CC(u8 *param_1);
extern void func_8000D064(void);
extern void func_80065868(void *arg);
extern void func_800659B4(s32 x, s32 y, u32 bits);
extern void func_80065904(s32 p);

typedef struct Item {
    u8 pad0[0xC];
    s32 f0C;
    u8 pad10[0x40];
    s32 f50;
    s32 f54;
} Item;

typedef struct Chan {
    s32 f00;
    s32 f04;
    s32 f08;
} Chan;

typedef struct CurNext {
    Item *cur;
    Item *next;
} CurNext;

typedef struct Pair {
    s32 a;
    s32 b;
} Pair;

extern Chan D_800F13B8;
extern Chan D_800F13D0;
extern s32 D_800F13E8;
extern s32 D_800F4780;
extern s32 D_800F0DA0;
extern s32 D_800F0DA8;
extern CurNext D_800F0DC8;
extern s32 D_800F11F0;
extern s32 D_800F11F4;
extern s32 D_800F141C;
extern s32 D_800F1418;
extern Pair D_800F45F0[];
extern s32 D_800F46EC;
extern s32 D_800EFD70;
extern s32 D_800C2930;
extern s32 D_800C2924;
extern s32 D_800C29D8;
extern s32 D_800CCCA0;
extern s32 D_800CCCA4;
extern s32 D_800CCCA8;
extern s32 D_800CCCC4;
extern s32 D_800CCCC8;
extern s32 D_800CCCCC;

void func_800652DC(void)
{
    s32 msg;
    s32 n;
    s32 i;
    Pair *p;
    Pair *base;

    func_800B2A50(&D_800F13B8, 1, 1);
    func_800B1280(9, &D_800F13B8, 2);
    func_800B1280(0xE, &D_800F13B8, 3);
    if (D_800F4780 == 0) {
        func_800B1280(0xC, &D_800F13B8, 4);
        func_800B1280(0xA, &D_800F13B8, 4);
    }

    while (1) {
        func_800B0610(&D_800F13B8, &msg, 1);
        if (msg == 1) {
            if (D_800C2930 != 0) {
                continue;
            }
            D_800F11F4 = func_800B2540();
            D_800CCCC4++;
            if (D_800CCCA4 != D_800CCCA8) {
                func_80065904(D_800CCCA4);
            }
            D_800F11F0++;
            if (D_800F13E8 != 0) {
                if (D_800F0DC8.next == NULL) {
                    if (func_800B0610(&D_800F13D0, &msg, 0) == 0) {
                        D_800F0DC8.next = (Item *)msg;
                    }
                }
                D_800F0DC8.cur = D_800F0DC8.next;
                D_800F0DC8.next = NULL;
                if (D_800F0DC8.cur != NULL) {
                    func_800658CC((u8 *)D_800F0DC8.cur);
                    D_800F11F0 = 0;
                }
            }
            func_80065868(&D_800F0DA8);
            if (D_800CCCC8 != 0) {
                func_800B1150(&D_800F13B8, 2, 0);
                D_800CCCC8 = 0;
            }
        } else if (msg == 2) {
            n = func_800B2540();
            if (n < 10 || D_800F13B8.f08 == 1) {
                func_800B1150(&D_800F13B8, msg, 0);
                continue;
            }
            if (D_800F11F0 < D_800F0DA0 && D_800CCCCC != 0) {
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
            func_800B2CC0(D_800F0DC8.cur->f0C);
            if (D_800CCCA0 != 0) {
                if (--D_800CCCA0 == 0) {
                    func_800B3020(0);
                }
            }
            func_800B1150(D_800F0DC8.cur->f50, D_800F0DC8.cur->f54, 0);
        } else if (msg == 3) {
            func_800B3020(1);
            func_80065904(0);
            func_800B3020(1);
            func_800B2C70(1.0f);
            func_8000CD00();
            for (i = 0; i < 4; i++) {
                func_8000D0A0(i, -1);
            }
            D_800C2924 = 1;
            func_8000D064();
            func_8000CD00();
            for (;;) ;
        } else if (msg == 4) {
            base = D_800F45F0;
            func_80065904(0);
            func_8000ED10((u8 *)D_800C29D8, 0, 0x25800);
            func_800AD990();
            func_800B2CC0(D_800C29D8);
            func_800B3020(0);
            if (!(D_800EFD70 & 0x20000000)) {
                for (;;) ;
            }
            func_800659B4(0x32, 0x1E, D_800F46EC);
            for (i = 0; i < 0x1F; i++) {
                func_800659B4((i & 1) * 150 + 0x32, (i / 2) * 10 + 0x28, base[i].b);
            }
            func_800AD990();
            for (;;) ;
        }
    }
}
