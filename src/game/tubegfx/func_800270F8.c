#include "ctx.h"

typedef struct {
    s32 x0, z0, x1, z1;
    s32 pad10, pad14;
    s32 idx;
    s32 pad1c;
} Ent;

typedef struct {
    u32 n;
    s32 pad;
    Ent e[1];
} Lst;

typedef struct {
    s32 x0, x1, z0, z1;
} Box;

extern s32 D_800E8508;
extern s32 D_800E8958;
extern Box *D_800E7814;
void func_8002E920(s32, s32, s32);

void func_800270F8(Lst *a, Lst *b)
{
    Ent *e;
    Ent *f;
    u32 i, k;
    s32 x, z, fx, fz, d, dx, dz;
    s32 best, bd;

    for (i = 0; i < a->n; i++) {
        D_800E8508 = 0;
        x = (a->e[i].x0 + a->e[i].x1) / 2;
        z = (a->e[i].z0 + a->e[i].z1) / 2;
        func_8002E920(x << 4, 999999, z << 4);
        if (D_800E8508 != 0) {
            { s32 n = D_800E8958; if (n) dz = 1; best = n; }
        } else {
            x /= 4;
            z /= 4;
            for (k = 0; k < b->n; k++) {
                fx = (b->e[k].x0 + b->e[k].x1) / 2;
                fz = (b->e[k].z0 + b->e[k].z1) / 2;
                fx /= 4;
                fz /= 4;
                dx = x - fx;
                dz = z - fz;
                { s32 t; t = dx * dx; do { d = dz * dz; } while (0); dz = d; d = t + dz; }
                if (d < bd || k == 0) {
                    bd = d;
                    best = k;
                }
            }
        }
        do {
            a->e[i].idx = best << 16;
        } while (0);
        if (a->e[i].x0 < ((Box *)((best << 4) + (u32)D_800E7814))->x0) {
            D_800E7814[best].x0 = a->e[i].x0;
        }
        if (a->e[i].z0 < D_800E7814[best].z0) {
            D_800E7814[best].z0 = a->e[i].z0;
        }
        if (D_800E7814[best].x1 < a->e[i].x1) {
            D_800E7814[best].x1 = a->e[i].x1;
        }
        if (D_800E7814[best].z1 < a->e[i].z1) {
            D_800E7814[best].z1 = a->e[i].z1;
        }
    }
}
