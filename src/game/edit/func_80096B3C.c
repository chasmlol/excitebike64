#include "ctx.h"

typedef struct { s16 p[2]; s16 d[2]; s16 l[2]; } Seg;

typedef struct {
    u8 pad[0x10];
    u16 flags;
    s16 off;
    Seg *tab;
    s16 n0;
    s16 n1;
    s16 o1c;
    s16 o1e;
    s16 o20;
    s16 o22;
} Rec;

void func_80096B3C(Rec *r)
{
    s32 a, b;

    if (r == NULL) return;
    if (r->flags & 2) {
        b = 0;
        a = 1;
    } else {
        a = 0;
        b = 1;
    }
    r->n0 = (r->tab->d[a] + r->off) & 3;
    r->n1 = (r->tab->d[b] + r->off) & 3;

    switch (r->n0) {
    case 0:
        switch (r->tab->d[a]) {
        case 0: case 3: r->o1c = r->tab->l[a]; break;
        case 2: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 1: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 2:
        switch (r->tab->d[a]) {
        case 1: case 2: r->o1c = r->tab->l[a]; break;
        case 0: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 3: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 3:
        switch (r->tab->d[a]) {
        case 0: case 3: r->o1c = r->tab->l[a]; break;
        case 2: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 1: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 1:
        switch (r->tab->d[a]) {
        case 1: case 2: r->o1c = r->tab->l[a]; break;
        case 0: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 3: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    }

    switch (r->n1) {
    case 0:
        switch (r->tab->d[b]) {
        case 0: case 3: r->o1e = r->tab->l[b]; break;
        case 2: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 1: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 2:
        switch (r->tab->d[b]) {
        case 1: case 2: r->o1e = r->tab->l[b]; break;
        case 0: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 3: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 3:
        switch (r->tab->d[b]) {
        case 0: case 3: r->o1e = r->tab->l[b]; break;
        case 2: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 1: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 1:
        switch (r->tab->d[b]) {
        case 1: case 2: r->o1e = r->tab->l[b]; break;
        case 0: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 3: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    }

    if (r->off & 1) {
        r->o20 = r->tab->p[1];
        r->o22 = r->tab->p[0];
    } else {
        r->o20 = r->tab->p[0];
        r->o22 = r->tab->p[1];
    }
}
