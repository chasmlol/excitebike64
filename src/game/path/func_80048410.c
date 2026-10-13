#include "ctx.h"

typedef struct {
    f32 x, y, z, w;
    u8 p0[0x46 - 0x10];
    s16 n46;
    s16 n48;
    s16 p1;
    s16 n4c;
    u8 p2[0x6c - 0x4e];
} Node;

extern Node *D_800EE498;
f32 func_800ADFB0(f32 v);

f32 func_80048410(s32 a, s32 b)
{
    f32 acc;
    f32 K;
    f32 w, d, dx, dz;
    s32 i;

    acc = 0.0f;
    K = 10000.0f;
    for (;;) {
        if (a == b) {
            return acc;
        }
        if (K <= acc) {
            return acc;
        }
        w = D_800EE498[a].w;
        if (D_800EE498[a].n48 >= 0) {
            dx = D_800EE498[a].x - D_800EE498[D_800EE498[a].n48].x;
            dz = D_800EE498[a].z - D_800EE498[D_800EE498[a].n48].z;
            d = __builtin_sqrtf(dx * dx + dz * dz);
            break;
        }
        a = D_800EE498[a].n46;
        acc += w;
    }
    i = D_800EE498[a].n46;
    while (D_800EE498[i].n4c < 0) {
        if (i == b) {
            return acc + w + func_80048410(D_800EE498[a].n46, b);
        }
        i = D_800EE498[i].n46;
    }
    i = D_800EE498[a].n48;
    while (D_800EE498[i].n4c < 0) {
        if (i == b) {
            return acc + d + func_80048410(D_800EE498[a].n48, b);
        }
        i = D_800EE498[i].n46;
    }
    d += func_80048410(D_800EE498[a].n48, b);
    w += func_80048410(D_800EE498[a].n46, b);
    if (d < w) {
        return d + acc;
    }
    return w + acc;
}
