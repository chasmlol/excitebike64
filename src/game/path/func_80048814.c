#include "ctx.h"

typedef struct Rec {
    f32 f00, f04, f08, f0c, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34, f38, f3c, f40;
    s16 pad44;
    s16 parent;
    s16 child;
    u8 rest[0x6c - 0x4a];
} Rec;

extern Rec *D_800EE498;

void func_80048814(s32 idx, s32 stop)
{
    Rec *a, *b, *base;
    f32 inv, t, P, Q, R, S, nb, nq, f7, f8, f4, f5;
    s32 par;

    do {
    base = D_800EE498;
    a = (Rec *)(idx * 0x6c + (s32)base);
    inv = a->f0c / 3.0f;
    t = -a->f18;
    S = a->f14;
    par = a->parent;
    b = (Rec *)(par * 0x6c + (s32)base);
    P = a->f00 + t * inv;
    Q = a->f08 + S * inv;
    nb = b->f00 - -b->f18 * inv;
    nq = b->f08 - b->f14 * inv;
    a->f34 = t * 3.0f * inv;
    a->f30 = (nb - P) * 3.0f - a->f34;
    a->f2c = b->f00 - a->f00 - a->f30 - a->f34;
    a->f40 = S * 3.0f * inv;
    a->f3c = (nq - Q) * 3.0f - a->f40;
    a->f38 = b->f08 - a->f08 - a->f3c - a->f40;
    if (a->child >= 0) {
        func_80048814(a->child, stop);
    }
    idx = par;
    } while (idx != stop);
}
