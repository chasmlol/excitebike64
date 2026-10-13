#include "ctx.h"

f32 func_800ADFC0(f32 x);
f32 func_800AEA30(f32 x);

void func_80013438(f32 *out, f32 x, f32 y, f32 z)
{
    f32 hx, hy, hz;
    f32 c1, s1, c2, s2, c3, s3;
    f32 p21, p2s, ps1, pss;

    hx = x * 0.5f;
    c1 = func_800ADFC0(hx);
    s1 = func_800AEA30(hx);
    hy = y * 0.5f;
    c2 = func_800ADFC0(hy);
    s2 = func_800AEA30(hy);
    hz = z * 0.5f;
    c3 = func_800ADFC0(hz);
    s3 = func_800AEA30(hz);

    p21 = c2 * c1;
    p2s = c2 * s1;
    ps1 = s2 * c1;
    pss = s2 * s1;
    out[0] = p21 * c3 + pss * s3;
    out[1] = -ps1 * s3 + p2s * c3;
    out[2] = -p21 * s3 + pss * c3;
    out[3] = p2s * s3 + ps1 * c3;
}
