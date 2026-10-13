#include "ctx.h"

extern s32 func_80013010(s32 x);
extern f32 func_800ADFB0(f32 x);
extern s16 D_800C2C60[];

void func_80012250(f32 *p, s32 *o1, s32 *o2, s32 *o3)
{
    s32 q[9];
    s32 *m;
    f32 sum, len, n0, n1, n2, n3;
    f32 n1sq, n1n3, n1n2, n2sq, n3sq, n2n3, n0n1, n0n3, n0n2;
    f32 fa, fb, fc, fd, fe, fg, fh, fi, fj;
    s32 i2, u6, sv2, sv3, sv4;

    sum = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
    len = __builtin_sqrtf(sum);
    p[0] /= len;
    p[1] /= len;
    p[2] /= len;
    p[3] /= len;
    n0 = p[0];
    n1 = p[1];
    n2 = p[2];
    n3 = p[3];
    n1sq = n1 * n1;
    n1n2 = n1 * n2;
    n1n3 = n1 * n3;
    n2sq = n2 * n2;
    n2n3 = n2 * n3;
    n3sq = n3 * n3;
    n0n1 = n0 * n1;
    n0n2 = n0 * n2;
    n0n3 = n0 * n3;
    fa = (n0n3 + n1n2) * 32768.0f;
    fb = (n1n3 - n0n2) * 32768.0f;
    fc = (n1n2 - n0n3) * 32768.0f;
    fd = ((0.5f - n1sq) - n3sq) * 32768.0f;
    fe = (n2n3 + n0n1) * 32768.0f;
    fg = (n1n3 + n0n2) * 32768.0f;
    fh = (n2n3 - n0n1) * 32768.0f;
    fi = ((0.5f - n1sq) - n2sq) * 32768.0f;
    fj = ((0.5f - n2sq) - n3sq) * 32768.0f;
    m = q;
    m[1] = (s32)fa;
    m[2] = (s32)fb;
    m[3] = (s32)fc;
    m[5] = (s32)fe;
    m[6] = (s32)fg;
    m[7] = (s32)fh;
    m[4] = (s32)fd;
    m[8] = (s32)fi;
    m[0] = (s32)fj;
    i2 = func_80013010(q[1]);
    u6 = D_800C2C60[((u32)(i2 << 16)) >> 20];
    sv2 = (s16)i2;
    if (u6 != 0) {
        sv3 = func_80013010((q[2] << 14) / u6);
        if ((q[0] ^ u6 ^ ((s16)sv3 - 0x4000)) >= 0) {
            sv3 = -0x8000 - sv3;
        }
        sv4 = func_80013010((-(q[7] << 14)) / u6);
        if ((q[4] ^ u6 ^ ((s16)sv4 - 0x4000)) >= 0) {
            sv4 = -0x8000 - sv4;
        }
        *o1 = (s16)sv4;
        *o2 = sv2;
        *o3 = (s16)sv3;
    }
}
