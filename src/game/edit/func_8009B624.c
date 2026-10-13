#include "ctx.h"

typedef struct { s16 size[2]; s16 rot[2]; } Tab;
typedef struct { u8 pad[0xc]; s16 x; s16 y; u16 fl; s16 n; Tab *tab; } Spr;

s32 func_8009B258(Spr *p, s32 k);
s32 func_8009B11C(Spr *p, s32 k);

void func_8009B624(Spr *a, Spr *b)
{
    s32 fa, fb;
    u16 n;
    s32 f; u32 g;

    f = a->fl & 2; fa = f != 0;
    b->x = a->x;
    b->y = a->y;
    g = b->fl & 2; fb = g < 1;
    n = a->n + (a->tab->rot[fa] - b->tab->rot[fb]) + 2;
    b->n = n & 3;
    switch ((a->tab->rot[fa] + a->n) & 3) {
    case 0:
        b->y += a->tab->size[(a->n + 1) & 1];
        b->x += func_8009B11C(a, fa);
        b->x -= func_8009B11C(b, fb);
        break;
    case 2:
        b->y -= b->tab->size[(b->n + 1) & 1];
        b->x += func_8009B11C(a, fa);
        b->x -= func_8009B11C(b, fb);
        break;
    case 3:
        b->x -= b->tab->size[b->n & 1];
        b->y += func_8009B258(a, fa);
        b->y -= func_8009B258(b, fb);
        break;
    case 1:
        b->x += a->tab->size[a->n & 1];
        b->y += func_8009B258(a, fa);
        b->y -= func_8009B258(b, fb);
        break;
    }
}
