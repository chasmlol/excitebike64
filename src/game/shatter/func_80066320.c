#include "ctx.h"

typedef struct {
    u8 pad0[6];
    s16 px, py, pz;
    u8 padC[6];
    s16 x[3];
    s16 y[3];
    s16 z[3];
} Tri3;

s32 func_80066320(Tri3 *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz)
{
    s32 i, n, d, best, bi;
    s32 dx, dy, dz;

    for (i = 0; i < 3; i++) {
        n = (i + 1) % 3;
        dx = t->x[n] - t->x[i];
        dy = t->y[n] - t->y[i];
        dz = t->z[n] - t->z[i];
        d = dx * dx + dy * dy + dz * dz;
        if (d >= best || i == 0) {
            best = d;
            bi = i;
        }
    }
    n = (bi + 1) % 3;
    *ox = (t->x[bi] + t->x[n]) / 2 + t->px;
    *oy = (t->y[bi] + t->y[n]) / 2 + t->py;
    *oz = (t->z[bi] + t->z[n]) / 2 + t->pz;
    *outI = bi;
    *outN = n;
    return 0;
}
