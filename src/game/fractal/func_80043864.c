#include "ctx.h"

typedef struct {
    s32 x;
    s32 y;
    s32 pad8;
    s32 seed;
    volatile s32 ctl;
    s32 f14;
    s32 f18;
    s32 f1C;
    u8 pad20[0x114 - 0x20];
    u8 a[0x81];
    u8 b[0x81];
    u8 pad216[0x31C - 0x216];
    u8 *buf;
} Gen;

extern u8 D_800EDFE0[];
extern s32 D_800EE128;

void func_8000ED10(u8 *a0, s32 a1, s32 a2);
s32 func_8004367C(s32 a0, s32 a1);
void func_80043768(u8 *a0);

static inline s32 Seed(s32 x, s32 y)
{
    return ((x * (y + 123) * 456) << (x % 12634)) + y;
}

void func_80043864(Gen *p, s32 x, s32 y)
{
    s32 i;
    s32 t;

    for (i = 0; i < 0x10000; i += 0x200) {
        func_8000ED10(p->buf + i, 0, 0x80);
    }
    p->f14 = 0x2000000;
    p->ctl = 0;
    p->ctl = 1;
    p->f18 = 0x1000000;
    p->ctl = 2;
    p->x = x;
    p->y = y;
    p->f1C = 7;
    t = Seed(x, y);
    p->ctl = 3;
    p->seed = t;
    p->buf[0] = func_8004367C(x, y);
    p->b[0] = func_8004367C(x + 1, y);
    p->b[0x80] = func_8004367C(x + 1, y + 1);
    p->a[0] = func_8004367C(x, y + 1);
    p->a[0x80] = p->b[0x80];
    D_800EE128 = Seed(x, y + 1) + 1;
    func_80043768(p->a);
    D_800EE128 = Seed(x + 1, y) + 2;
    func_80043768(p->b);
    D_800EE128 = p->seed + 1;
    D_800EDFE0[0] = p->buf[0];
    D_800EDFE0[0x80] = p->b[0];
    func_80043768(D_800EDFE0);
    for (i = 0; i < 0x80; i++) {
        p->buf[i] = D_800EDFE0[i];
    }
    D_800EE128 = p->seed + 2;
    D_800EDFE0[0] = p->buf[0];
    D_800EDFE0[0x80] = p->a[0];
    func_80043768(D_800EDFE0);
    for (i = 0; i < 0x80; i++) {
        ((u8 (*)[0x200])p->buf)[i][0] = D_800EDFE0[i];
    }
}
