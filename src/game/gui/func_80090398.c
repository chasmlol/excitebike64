#include "ctx.h"

typedef struct Slot {
    s32 w0;
    u32 flags;
    s16 n;
    s16 cur;
    u32 mask;
} Slot;

typedef struct Ctl {
    s32 w0;
    s16 count;
    s16 x;
    Slot *slots;
    s32 w3;
    s16 sel;
} Ctl;

s32 func_80090398(Ctl *c)
{
    s32 i;
    s32 j;
    Slot *s;
    Slot *t;

    s = c->slots;
    if ((s[c->sel].flags & 0x10) == 0) {
        for (i = 0; i < c->count; i++, s++) {
            if (s->flags & 0x10) {
                c->sel = i;
                c->x = i;
                break;
            }
        }
        if (i == c->count) {
            return 0;
        }
    }

    t = &c->slots[c->sel];
    if (t->n == 0) {
        goto ret1;
    }
    if (t->mask & (1 << t->cur)) {
        goto ret1;
    }
    for (j = 0; j < t->n; j++) {
        if (t->mask & (1 << j)) {
            t->cur = j;
            break;
        }
    }
    if (j < t->n) {
        return 0;
    }
ret1:
    return 1;
}
