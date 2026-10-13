#include "ctx.h"

void func_800515A8(void);
s32 func_8005AFFC(void);

typedef struct {
    u8 pad0[0x22];
    s16 count;
    u8 pad24[0x2C - 0x24];
    s32 tick;
    u8 pad30[0x40 - 0x30];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 flags;
} Game;

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
} Rider;

extern Game D_800F05A8;
extern Rider D_800E37C0[];
extern s32 D_800F05E8;
extern s32 D_800E3770;
extern s32 D_800E36E0;

void func_800516DC(void)
{
    Game *g;
    Game *g2;
    Rider *r;
    u8 flag;

    func_800515A8();
    g = &D_800F05A8;
    if (!(g->flags & 0x10) && g->f40 == 0) {
        g->tick++;
    }
    flag = 1;
    for (r = D_800E37C0; r < &D_800E37C0[D_800F05A8.count]; r++) {
        if (r->f66A != 0) {
            r->f66A--;
            r->f69C += 30;
        }
        if (D_800F05A8.tick == r->f69C && r->done == 0) {
            r->done = 1;
            r->f668 = 5;
            r->f6E8 = 1;
        }
        if (r->done == 0) flag = 0;
    }
    D_800F05E8 = flag;
    if (flag) {
        if (((D_800E3770 & 0x40) && !(D_800E36E0 & 0x40)) ||
            ((D_800E3770 & 0x10) && !(D_800E36E0 & 0x10))) {
            func_8005AFFC();
        }
    }
}
