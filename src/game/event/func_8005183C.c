#include "ctx.h"

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
    u8 pad0[0x3C8];
    s32 key;
    u8 pad3CC[0x3FF - 0x3CC];
    u8 f3FF;
    u8 pad400[0x668 - 0x400];
    s16 f668;
    u8 pad66A[0x6E8 - 0x66A];
    u8 finished;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player;

typedef struct {
    u8 pad0[0x3C];
    s32 n;
    u8 pad40[0xC];
    s32 mode;
} Game;

typedef struct {
    u8 pad[0x24];
} Thing;

typedef struct {
    u8 pad[0x50];
    s32 flags;
} Hud;

extern s32 D_800E3770;
extern s32 D_800E36E0;
extern s32 D_800E36F8;
extern s32 D_800E374C;
extern Game D_800EFD70;
extern volatile s32 D_800EFDAC;
extern Hud *D_800EFD64;
extern Rank D_800F05A8;
extern Player D_800E37C0[];
extern u8 D_800E439F;
extern Thing **D_800CA930;
extern s32 D_800C8CE8;
extern Thing D_800CA9C8[];

void func_800515A8(void);

void func_8005183C(void)
{
    Rank *q;
    Game *g;
    Player *e;
    s32 i, j, k;

    func_800515A8();
    if (D_800F05A8.flags & 0x10) return;
    if (--D_800F05A8.timer > 0) return;
    if (((D_800E3770 & 0x40) && !(D_800E36E0 & 0x40)) ||
        ((D_800E3770 & 0x10) && !(D_800E36E0 & 0x10))) {
        if (D_800E36F8 == 5) {
            D_800E36F8 = 6;
            D_800E374C = 0x14;
        }
    }
    D_800F05A8.timer = 0;
    if (D_800F05A8.done) return;
    for (i = 0; i < D_800EFD70.n; i++) {
        e = &D_800E37C0[i];
        if (e->finished) continue;
        if (D_800C8CE8) continue;
        if (D_800EFD70.mode == 1 && D_800CA930 != NULL && *D_800CA930 - D_800CA9C8 == 5) continue;
        if (e->key > D_800F05A8.key[2]) {
            for (j = 0; j < 3; j++) {
                if (e->key >= D_800F05A8.key[j]) {
                    for (k = 1; k >= j; k--) {
                        D_800F05A8.key[k + 1] = D_800F05A8.key[k];
                        D_800F05A8.tri[k + 1][0] = D_800F05A8.tri[k][0];
                        D_800F05A8.tri[k + 1][1] = D_800F05A8.tri[k][1];
                        D_800F05A8.tri[k + 1][2] = D_800F05A8.tri[k][2];
                    }
                    break;
                }
            }
            D_800F05A8.key[j] = e->key;
            D_800F05A8.tri[j][2] = e - D_800E37C0;
            D_800F05A8.tri[j][0] = 0xFF;
            D_800F05A8.tri[j][1] = 0xFF;
            D_800EFD64->flags |= 0x100;
        }
    }
    D_800F05A8.done = 1;
    for (j = 0; j < D_800EFD70.n; j++) {
        D_800E37C0[j].finished = 1;
    }
    D_800E37C0[0].f3FF = 1;
    *(volatile s16 *)&D_800E37C0[0].f668 = 1;
    if (D_800EFDAC == 2) {
        if (D_800E37C0[0].key > D_800E37C0[1].key) {
            D_800E37C0[0].f668 = 0;
            D_800E37C0[1].f668 = 5;
        } else if (D_800E37C0[0].key == D_800E37C0[1].key) {
            D_800E37C0[0].f668 = 1;
            D_800E37C0[1].f668 = 1;
        } else {
            D_800E37C0[0].f668 = 5;
            D_800E37C0[1].f668 = 0;
        }
        D_800E439F = 1;
    }
}
