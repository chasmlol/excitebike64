#include "ctx.h"

typedef struct Sub {
    u8 pad0[0x8];
    s32 f8;
    s16 c;
    s16 e;
    u8 pad1[0x2];
    s16 cur;
    u8 pad2[0x4];
    s16 arr[2];
} Sub;

typedef struct Game {
    u8 pad0[0x184];
    Sub *sub;
    u8 pad1[0x16FC - 0x188];
    s32 idx;
    u8 pad2[0x1738 - 0x1700];
    s32 f1738;
    u8 pad3[0x1758 - 0x173C];
    s32 t1758;
    s32 t175c;
} Game;

typedef struct T2 {
    s16 cnt[2];
    u8 pad[0x2C];
    u8 *p30;
    u8 *p34;
    u8 pad2[0x8];
} T2;

typedef struct U4 {
    u32 w;
} __attribute__((packed)) U4;

extern Game *D_800CEA40;
extern T2 D_800CE280[];
extern u8 D_800F55B8[];

void func_8008D3B4(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);
void func_800A4FF0(u8 *a0, u8 *a1, u8 *a2, s32 a3);

void func_8009C7D4(void)
{
    T2 *t;
    s32 x;
    s32 y;
    s32 i;
    s16 sv;
    Sub *sub;
    Game *g;

    t = &D_800CE280[D_800CEA40->idx + 18];
    for (y = 0; y < t->cnt[(D_800CEA40->sub->cur + 1) & 1]; y++) {
        for (x = 0; x < t->cnt[D_800CEA40->sub->cur & 1]; x++) {
            func_8008D3B4(D_800CEA40->sub->c + x, D_800CEA40->sub->e + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    for (i = 0; i < D_800CE280[D_800CEA40->idx + 18].cnt[1]; i++) {
        __builtin_memcpy(D_800F55B8 + D_800CEA40->idx * 30 + i * 4, D_800F55B8 + D_800CEA40->idx * 30 + i * 4 + 4, 4);
    }
    g = D_800CEA40;
    D_800CE280[g->idx + 18].cnt[1]--;
    {
        s32 k = D_800CE280[g->idx + 18].cnt[1] * 4 - 2;
        k += g->idx * 30;
        *(s16 *)(D_800F55B8 + k) = 0;
    }
    func_800A4FF0(D_800CE280[g->idx + 18].p30, D_800CE280[g->idx + 18].p34,
                  D_800F55B8 + g->idx * 30, D_800CE280[g->idx + 18].cnt[1]);

    sub = D_800CEA40->sub;
    sv = sub->arr[sub->f8 == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->c++;
        }
    } else {
        sub->e++;
    }
    if (D_800CEA40->f1738 > 45000) {
        D_800CEA40->t175c = -10000;
        D_800CEA40->t1758 = 50;
    }
}
