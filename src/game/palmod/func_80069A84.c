#include "ctx.h"

void func_80069A84(f32 *a, f32 *b, f32 *c)
{
    f32 t = (a[0x16] - a[0x17]) * 0.5f;
    b[0] = a[0] - a[5] * t;
    c[0] = a[2] - a[6] * t;
}
