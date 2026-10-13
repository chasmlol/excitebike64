#include "ctx.h"

typedef struct Slot {
    s32 w0, w1, w2, w3, w4, w5, w6, w7;
    u8 w8[4];
    s32 w9, w10, w11, w12, w13, w14, w15, w16, w17, w18;
} Slot;

extern Slot D_80100720[];
extern void (*D_800D5710)(void);

void func_800BED20(void);

s32 func_800BEC20(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s32 i;
    s32 off;
    s32 t;
    Slot *base;
    Slot *p;
    Slot *e;

    func_800BED20();
    if (D_800D5710 != 0) {
        D_800D5710();
    }
    i = 0;
    base = D_80100720;
    p = base;
    for (; i < 8; i++) {
        if (p->w0 == 0) {
            break;
        }
        p++;
    }
    if (i == 8) {
        return -2;
    }
    off = i * 76;
    e = (Slot *)(off + (s32)base);
    e->w15 = a3;
    e->w16 = a2;
    e->w17 = a4;
    e->w14 = a0;
    e->w18 = a1;
    e->w8[2] = 0;
    t = (s32)&base->w1;
    e->w9 = off + t;
    e->w0 = 2;
    e->w10 = e->w15;
    e->w11 = e->w16;
    e->w12 = e->w17;
    return i;
}
