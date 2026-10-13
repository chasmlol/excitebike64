#include "ctx.h"

typedef struct Sub { u8 p0[0x1c]; f32 s1c; f32 s20; f32 s24; } Sub;
typedef struct Obj {
    u8 p0[0x28]; f32 f28; f32 f2c; f32 f30; u8 p34[4]; f32 f38;
    u8 p3c[0x1c]; f32 f58; f32 f5c; f32 f60; u8 p64[0x2c]; Sub *sub;
} Obj;

u32 func_800138FC(f32 *p);
f32 func_80013774(f32 x);

#define FABS_GT(x, e) ((0.0f < (x)) ? ((e) < (x)) : ((e) < -(x)))
#define LEN(o) __builtin_sqrtf((o)->f58 * (o)->f58 + (o)->f5c * (o)->f5c + (o)->f60 * (o)->f60)

void func_8001CB80(Obj *a0, f32 *a1)
{
    f32 len, pos, acc, t, v, targ;

    acc = 0.0f;
    len = -1.0f;
    v = a0->f30 - a0->sub->s1c;
    if (FABS_GT(v, 0.001f)) {
        len = LEN(a0);
        t = func_80013774(-len);
        a0->f30 = pos = a0->f30 * t + (1.0f - t) * a0->sub->s1c;
    } else {
        pos = a0->f30 = a0->sub->s1c;
    }
    if (FABS_GT(a0->f38, 0.001f) || a0->sub->s20 != 0.0f) {
        if (len < 0.0f) {
            len = LEN(a0);
        }
        acc = (f32)(s32)(func_800138FC(a1) - 0x80) * 0.00390625f;
        targ = -len * a0->sub->s24;
        acc *= a0->sub->s20;
        t = func_80013774(targ);
        acc = t * a0->f38 + (1.0f - t) * acc;
        a0->f38 = acc;
    }
    a0->f2c += pos + acc;
    a0->f28 += pos + acc;
}
