#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
} Rng;

extern Rng D_800E8518;

s32 func_80031FA0(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.b && p3 >= D_800E8518.d) {
        return 0;
    }
    if (p3 < D_800E8518.b && p3 < D_800E8518.d) {
        return 0;
    }
    t = ((p3 - D_800E8518.b) << 8) / (D_800E8518.d - D_800E8518.b);
    v = D_800E8518.a + ((D_800E8518.c - D_800E8518.a) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}
