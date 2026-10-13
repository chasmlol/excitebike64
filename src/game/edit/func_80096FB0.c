#include "ctx.h"

typedef struct { s16 x, y, z; u16 f; s16 s, t; u8 r, g, b, a; } V;
typedef struct { u8 pad[0x174C]; V *v[2]; s32 count; } G;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern G *D_800CEA40;

void func_80096FB0(s32 idx, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b)
{
    s32 ax, ay, ox, oy;

    x0 = x0 - D_800F13EC / 2;
    x1 = x1 - D_800F13EC / 2;
    y0 = D_800F13F0 / 2 - y0;
    y1 = -y1;
    y1 += D_800F13F0 / 2;
    if (D_800CEA40->count >= 0x3C) {
        return;
    }
    ax = x1 - x0;
    if (ax <= 0) ax = x0 - x1;
    ay = y1 - y0;
    if ((ay <= 0 ? y0 - y1 : ay) < ax) {
        ox = 0;
        oy = 2;
    } else {
        ox = 3;
        oy = 0;
    }
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].x = x0;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].y = y0;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].z = z0;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].x = x0 + ox;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].y = y0 + oy;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].z = z0;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].x = x1;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].y = y1;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].z = z1;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].x = x1 + ox;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].y = y1 + oy;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].z = z1;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].r = r;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].g = g;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 0].b = b;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].r = r;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].g = g;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 1].b = b;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].r = r;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].g = g;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 2].b = b;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].r = r;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].g = g;
    D_800CEA40->v[idx][D_800CEA40->count * 4 + 3].b = b;
    D_800CEA40->count++;
}
