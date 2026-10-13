#include "ctx.h"

typedef struct { s32 x; s32 y; s32 z; s32 pad[4]; } Ent;
extern Ent D_800E8D78[];
extern s32 D_800E850C;
void func_80030ECC(s32, s32, s32, s32);
f32 sqrtf(f32 x);
int abs(int);

void func_80030478(f32 *p)
{
    s32 nx, ny, i, dx, dy, ax, d2, lim;
    f32 r;

    nx = (s32)(p[0] * 1000.0f);
    ny = (s32)(p[2] * 1000.0f);
    func_80030ECC(nx - 0x1000, ny - 0x1000, nx + 0x1000, ny + 0x1000);
    if (D_800E850C > 0) {
    i = 0;
    lim = 0x900000;
    do {
        dx = D_800E8D78[i].x - nx;
        if (abs(dx) < 0xc01) {
            dy = D_800E8D78[i].z - ny;
            if (abs(dy) < 0xc01) {
                d2 = dx * dx + dy * dy;
                if (d2 <= lim) {
                    r = sqrtf((f32)d2);
                    nx = (s32)((f32)D_800E8D78[i].x - (f32)(dx * 0xc00) / r);
                    ny = (s32)((f32)D_800E8D78[i].z - (f32)(dy * 0xc00) / r);
                }
            }
        }
    } while (++i < D_800E850C);
    }
    p[0] = (f32)nx / 1000.0f;
    p[2] = (f32)ny / 1000.0f;
}
