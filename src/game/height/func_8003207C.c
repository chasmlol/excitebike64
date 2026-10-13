#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
} Rng;

extern Rng D_800E8518;

s32 func_8003207C(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.a && p3 >= D_800E8518.c) {
        return 0;
    }
    if (p3 < D_800E8518.a && p3 < D_800E8518.c) {
        return 0;
    }
    t = ((p3 - D_800E8518.a) << 8) / (D_800E8518.c - D_800E8518.a);
    v = D_800E8518.b + ((D_800E8518.d - D_800E8518.b) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}
