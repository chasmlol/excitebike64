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
    u8 pad1[0x16f8 - 0x188];
    s32 n16f8;
    s32 idx;
    u8 pad2[0x1758 - 0x1700];
    s32 t1758;
    s32 t175c;
} Game;

typedef struct T1 {
    s16 cnt[2];
    u8 pad[60];
} T1;

typedef struct T2 {
    s16 w0;
    s16 cnt;
    u8 pad[0x2c];
    u8 *p30;
    u8 *p34;
    u8 pad2[0x8];
} T2;

typedef struct U4 {
    u32 w;
} __attribute__((packed)) U4;

extern Game *D_800CEA40;
extern T1 D_800CE700[];
extern T2 D_800CE280[];
extern u8 D_800F55B8[];
extern u8 D_800F55BC[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8008D3B4(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);
void func_800A4FF0(u8 *a0, u8 *a1, u8 *a2, s32 a3);

void func_8009CD24(void) {
    T1 *t1;
    T2 *t2;
    T2 *b2;
    T2 *bp;
    T2 *bq;
    T2 *tq;
    T2 *tr;
    T2 *ts;
    Game *g;
    u8 *hb;
    Game *gt;
    u8 *ub;
    u8 *sb;
    Sub *sub;
    s32 x;
    s32 y;
    s32 i;
    s32 off;
    s16 sv;

    t1 = &D_800CE700[D_800CEA40->idx];
    for (y = 0; y < t1->cnt[(D_800CEA40->sub->cur + 1) & 1]; y++) {
        for (x = 0; x < t1->cnt[D_800CEA40->sub->cur & 1]; x++) {
            func_8008D3B4(D_800CEA40->sub->c + x, D_800CEA40->sub->e + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    bp = D_800CE280;
    tq = (T2 *)((u8 *)bp + ((D_800CEA40->idx + 0x12) << 6));
    i = tq->cnt - 1;
    if (i >= 0) {
        do {
            off = D_800CEA40->idx * 30 + i * 4;
            ub = D_800F55BC;
            sb = ub - 4;
            *(U4 *)(ub + off) = *(U4 *)(sb + off);
            i--;
        } while (i >= 0);
    }

    func_8000ED10(D_800F55B8 + D_800CEA40->idx * 30, 0, 4);

    g = D_800CEA40;
    bq = D_800CE280;
    tr = (T2 *)((u8 *)bq + ((g->idx + 0x12) << 6));
    tr->cnt = tr->cnt + 1;
    bq = D_800CE280;
    ts = (T2 *)((u8 *)bq + ((g->idx + 0x12) << 6));
    func_800A4FF0(ts->p30, ts->p34, D_800F55B8 + g->idx * 30, ts->cnt);

    D_800CEA40->n16f8 = D_800CEA40->n16f8 + 1;
    sub = D_800CEA40->sub;
    sv = sub->arr[sub->f8 == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->c = sub->c - 1;
        }
    } else {
        sub->e = sub->e - 1;
    }
    D_800CEA40->t175c = 10000;
    D_800CEA40->t1758 = 50;
}
