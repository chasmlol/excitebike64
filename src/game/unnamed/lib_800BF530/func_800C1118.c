#include "ctx.h"

typedef struct {
    u8 g0;
    u8 g1;
    s8 xoff;
    u8 g3;
    u8 adv;
    u8 g5;
    s8 kadj;
    u8 g7;
} Glyph;

typedef struct {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    u8 p9[7];
    Glyph gl[1];
} Font;

typedef struct {
    s32 font;
    s32 w4;
    Glyph * volatile cur;
    s32 wc;
    s32 off;
    s32 w14;
    s32 w18;
    s32 w1c;
    u8 text[1];
} Obj;

extern Font *D_80105380[];

s32 func_800C1118(Obj *p, s32 x, s32 unused, s32 n)
{
    Glyph *g;
    u8 *t;
    Font *f;
    u8 *kt;
    u8 *pr;
    s32 start;
    u32 idx;
    s32 c;
    s32 k;
    s32 ch;
    u8 adv;

    g = p->cur;
    t = p->text;
    goto setup;

ret:
    adv = g->adv;
    return ((x + g->xoff + (adv >> 1)) << 16) | adv;

setup:
    x = x + p->off;
    start = x;
    c = *t;
    f = D_80105380[p->font];
    if (c == 0) {
        goto fail;
    }
    do {
        idx = c - f->base;
        if (idx >= f->count) {
            goto fail;
        }
        t++;
        n--;
        g = &f->gl[idx];
        if (n == -1) {
            goto ret;
        }
        if (c == 10) {
            x = start;
        } else {
            x = x + (g->kadj + f->adj);
            kt = f->kern;
            if (kt != 0 && kt[idx] != 0) {
                kt = kt + kt[idx] * 2;
                idx = *t;
                while ((k = *kt) != 0) {
                    if (k == idx) {
                        x = x + (s8)kt[1];
                        break;
                    }
                    kt = kt + 2;
                }
            }
        }
        c = *t;
    } while (c != 0);
fail:
    return -1;
}
