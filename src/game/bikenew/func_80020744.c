#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    f32 x;
    f32 y;
    f32 z;
    f32 vx;
    f32 vy;
    f32 vz;
    u8 pad1[0x420 - 0x60];
    f32 f420;
} Body;

s32 func_8002E920(s32 a, s32 b, s32 c);

f32 func_80020744(Body *p, f32 g, f32 t, f32 *out)
{
    f32 diff, prev, v, step, lim, w;

    out[0] = p->x + p->vx * t;
    out[1] = p->y + (p->vy + g * t * 0.5f) * t;
    w = p->z + p->vz * t;
    out[2] = w;
    if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else w = 3.0f;
        diff = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
    v = p->vy + g * t;
    if (0.0f < v ? v < 0.1f : -v < 0.1f) v = 0.1f;
    v = 1.0f / v;
    if (10.0f < v) v = 10.0f;
    else if (v < -10.0f) v = -10.0f;
    lim = 0.1f;
    while (lim < diff || diff < -lim) {
        step = diff * v;
        if (t + step < 0.0f) step = -t;
        t += step;
        prev = diff;
        out[1] = p->y + (p->vy + g * t * 0.5f) * t;
        out[0] = p->x + p->vx * t;
        w = p->z + p->vz * t;
        out[2] = w;
        if (out == 0) w = 1.0f; else if (p == 0) w = 2.0f; else if (out == (f32 *)p) w = 4.0f; else if (out == &p->x) w = 5.0f; else if (out == &p->y) w = 6.0f; else w = 3.0f;
                diff = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    diff = (diff - out[1]) + 0.5f;
        if (prev == diff) return t;
        v = step / (prev - diff);
        if (10.0f < v) v = 10.0f;
        else if (v < -10.0f) v = -10.0f;
        lim += 0.05f;
    }
    if (p->f420 != 0.0f) {
        out[0] = out[0] - p->vx * (0.5f / p->f420);
        out[2] = out[2] - p->vz * (0.5f / p->f420);
    }
    if (out == 0) diff = 1.0f; else if (p == 0) diff = 2.0f; else diff = 3.0f;
        out[1] = (f32)func_8002E920(out[0] * 1000.0f, out[1] * 1000.0f, out[2] * 1000.0f) / 1000.0f;
    if (t < 0.0f) t = 0.0f;
    return t;
}
