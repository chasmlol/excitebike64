#include "ctx.h"

typedef struct Ent {
    s32 w0;
    u32 flags;
    s16 n;
    s16 cur;
    u32 mask;
} Ent;

typedef struct Ctl {
    s32 w0;
    s16 n;
    s16 cur;
    Ent *ents;
    u32 flags;
    s16 last;
} Ctl;

extern u32 D_800E30B0;
extern u32 D_800E25B0;

void func_8001102C(s32 param_1);

s32 func_8008F878(Ctl *c) {
    Ent *e;
    s32 i;
    s16 cc;
    s32 n;
    s32 ec;
    u32 bit;

    if (c == NULL) {
        return 0;
    }
    if (c->flags & 1) {
        e = c->ents + c->cur;
        if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
            c->last = c->cur;
            func_8001102C(1);
            return 1;
        }
        if ((D_800E30B0 & 0x800) && !(D_800E25B0 & 0x800)) {
            i = c->cur;
            for (;;) {
                i = i - 1;
                if (i == c->cur) {
                    break;
                }
                if (i < 0) {
                    i = c->n - 1;
                }
                if (c->ents[i].flags & 0x10) {
                    c->cur = i;
                    break;
                }
            }
        }
        if ((D_800E30B0 & 0x1000) && !(D_800E25B0 & 0x1000)) {
            i = c->cur;
            i = i + 1;
            cc = c->cur;
            if (i != cc) {
                n = c->n;
                do {
                    if (i >= n) {
                        i = 0;
                    }
                    if (c->ents[i].flags & 0x10) {
                        c->cur = i;
                        break;
                    }
                    i = i + 1;
                } while (i != cc);
            }
        }
        if (e->n != 0) {
            if ((D_800E30B0 & 0x2000) && !(D_800E25B0 & 0x2000)) {
                i = e->cur;
                for (;;) {
                    i = i - 1;
                    if (i < 0) {
                        i = e->n - 1;
                    }
                    ec = e->cur;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->mask & bit) == 0) {
                        break;
                    }
                }
                if (e->cur != i) {
                    e->cur = i;
                    func_8001102C(0);
                }
            }
            if ((D_800E30B0 & 0x4000) && !(D_800E25B0 & 0x4000)) {
                i = e->cur;
                for (;;) {
                    i = i + 1;
                    if (i >= e->n) {
                        i = 0;
                    }
                    ec = e->cur;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->mask & bit) == 0) {
                        break;
                    }
                }
                if (e->cur != i) {
                    e->cur = i;
                    func_8001102C(0);
                }
            }
        }
    }
    if (c->last != c->cur) {
        c->last = c->cur;
        func_8001102C(0);
    }
    return 0;
}
