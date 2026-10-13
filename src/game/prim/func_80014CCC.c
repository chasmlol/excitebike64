#include "ctx.h"

typedef struct {
    u32 a;
    u32 b;
} Entry;

typedef struct {
    u32 n;
    u32 p;
    Entry e[1];
} Hdr;

void func_80014CCC(Hdr **pp)
{
    Hdr *h = *pp;
    u32 i;

    h = (Hdr *)((u32)h & ~2);
    if (h->p != 0) {
        h->p += (u32)h;
    }
    for (i = 0; i < h->n; i++) {
        h->e[i].b += (u32)h;
    }
}
