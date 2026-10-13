#include "ctx.h"

typedef struct {
    s16 x, y, z;
} Face;

typedef struct {
    s32 x, y, z, w, h, f14, f18;
} Node;

extern s32 D_800E850C;
extern Node D_800E8D78[];
extern Face D_800C981C[];

s32 func_800300B0(f32 *p1, f32 *p2, f32 *p3, f32 *p4)
{
    Face *bestFace;
    s32 best;
    s32 ax, ay, az, bx, by, bz;
    s32 n, i;
    s32 ex, ey, ez;

    ax = (s32)(p1[0] * 1000.0f);
    ay = (s32)(p1[1] * 1000.0f);
    az = (s32)(p1[2] * 1000.0f);
    bx = (s32)(p2[0] * 1000.0f);
    by = (s32)(p2[1] * 1000.0f);
    bz = (s32)(p2[2] * 1000.0f);
    best = 0;
    bestFace = 0;
    for (n = 0; n < D_800E850C; n++) {
        Node *nd = &D_800E8D78[n];
        for (i = 0; i < 5; i++) {
            Face *f = &D_800C981C[i];
            s32 hw = nd->w;
            s32 hh = nd->h;
            s32 nx = nd->x;
            s32 ny = nd->y;
            s32 nz = nd->z;
            s32 px, py, pz;
            s32 d1, d2, dd;
            f32 t;
            s32 dx;
            px = ((hw * f->x) >> 14) + nx;
            py = ((hh * f->y) >> 14) + ny;
            pz = ((hw * f->z) >> 14) + nz;

            d1 = (bx - px) * f->x + (by - py) * f->y + (bz - pz) * f->z;
            if (d1 >= best) continue;
            pz = (ax - px) * f->x + (ay - py) * f->y + (az - pz) * f->z;
            if (pz < 0) continue;
            py = d1 - pz;
            if (py == 0) continue;
            dx = bx - ax;
            t = -(f32)pz / (f32)py;
            px = (f32)ax + t * (f32)dx;
            dx = by - ay;
            py = (f32)ay + t * (f32)dx;
            dx = bz - az;
            pz = (f32)az + t * (f32)dx;
            if (i < 2) {
                if (ny + hh < py || pz < nz - hw || nz + hw < pz) continue;
            } else if (i < 4) {
                if (ny + hh < py || px < nx - hw || nx + hw < px) continue;
            } else {
                if (px < nx - hw || nx + hw < px || pz < nz - hw || nz + hw < pz) continue;
            }
            bestFace = &D_800C981C[i];
            best = d1;
        }
    }
    if (bestFace) {
        *p4 = -((f32)best / 16384000.0f);
        p3[0] = bestFace->x * 6.1035156e-05f;
        p3[1] = bestFace->y * 6.1035156e-05f;
        p3[2] = bestFace->z * 6.1035156e-05f;
        return 1;
    }
    return 0;
}
