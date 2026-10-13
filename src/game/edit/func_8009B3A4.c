#include "ctx.h"

typedef struct Node {
    u8 pad[0xC];
    s16 px;
    s16 py;
    u16 flags;
    u16 cnt;
    u16 *tab;
} Node;

extern u8 *D_800CEA40;

s32 func_8009B11C(Node *n, s32 k);
s32 func_8009B258(Node *n, s32 k);

#define TB(p, i) (*(u16 *)((u8 *)(p) + ((i) << 1)))
#define SX16(v) ((s32)((u32)(v) << 16))
#define HALF(v, s) ((s32)((SX16(v) >> 16) + (s32)((u32)SX16(v) >> 31)) >> (s))

void func_8009B3A4(Node *p, Node *c)
{
    Node *q = p;
    s32 k = 1;
    s32 m = 0;
    s32 t, t2, r, v, ka, ma;

    if (q == 0) {
        u16 *tg = *(u16 **)(D_800CEA40 + 0x16AC);
        c->px = HALF(tg[0], k) - HALF(c->tab[0], k);
        tg = *(u16 **)(D_800CEA40 + 0x16AC);
        c->py = HALF(tg[1], k) - HALF(c->tab[1], k);
        return;
    }
    if (q->flags & 2) k = 0;
    if (c->flags & 2) m = 1;
    c->px = q->px;
    *(volatile s16 *)&c->py = q->py;
    t = q->cnt + (*(u16 *)((u8 *)q->tab + (k << 1) + 4) - *(u16 *)((u8 *)*(u16 *volatile *)&c->tab + (m << 1) + 4)) + 2;
    t2 = t & 3;
    c->cnt = t2;
    r = (*(s16 *)((u8 *)q->tab + (k << 1) + 4) + (s32)(s16)q->cnt) & 3;
    switch (r) {
    case 0:
        c->py += TB(q->tab, ((s32)(s16)q->cnt + 1) & 1);
        v = func_8009B11C(q, k);
        c->px += v;
        v = func_8009B11C(c, m);
        c->px -= v;
        break;
    case 2:
        c->py -= TB(c->tab, (t2 + 1) & 1);
        v = func_8009B11C(q, k);
        c->px += v;
        v = func_8009B11C(c, m);
        c->px -= v;
        break;
    case 3:
        c->px -= TB(c->tab, t & 1);
        v = func_8009B258(q, k);
        c->py += v;
        v = func_8009B258(c, m);
        c->py -= v;
        break;
    case 1:
        c->px += TB(q->tab, q->cnt & 1);
        v = func_8009B258(q, k);
        c->py += v;
        v = func_8009B258(c, m);
        c->py -= v;
        break;
    }
}
