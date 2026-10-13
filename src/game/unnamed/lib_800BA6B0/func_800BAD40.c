#include "ctx.h"

typedef struct {
    void *ptr;
    s32 size;
    s32 f8;
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
} Ent;

extern Ent D_800FE8F0[];
extern char D_800DB170[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800B9FE0(u8 *d, u8 *s, s32 n);
void *func_800BC908();

s32 func_800BAD40(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 n;
    s32 t;
    Ent *e;
    Ent *base;
    Ent *w;
    void *q;

    base = D_800FE8F0;
    n = 0;
    w = base;
top:
    if (w->size == 0) {
        goto done;
    }
    n++;
    t = n < 0x100;
    w++;
    if (t) {
        goto top;
    }
done:
    if (n == 0x100) {
        return -1;
    }
    e = (Ent *)((s32)(n * 0x1C) + (s32)base);
    func_8000ED10((u8 *)e, 0, 0x1c);
    e->size = p2;
    if (p2 == 0) {
        return -3;
    }
    q = func_800BC908(p2, 0, D_800DB170, 0x421);
    e->ptr = q;
    if (q == 0) {
        return -4;
    }
    func_800B9FE0(q, p1, e->size);
    e->f8 = p3;
    e->fc = p4;
    e->f10 = p5;
    e->f18 = -1;
    return n;
}
