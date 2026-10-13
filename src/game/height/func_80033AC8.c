#include "ctx.h"

extern s32 D_800E19C0;

void func_800B31C0(f32 *a0, u32 *a1);

s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9)
{
    f32 m[4][4];
    s16 *ps;
    f32 kx, ky, kz, kw;
    f32 fx, fy, fz, a, b, c, w;
    s32 ret;

    ps = (s16 *)(p9 * 16 + D_800E19C0);
    kx = (f32)ps[0] * 0.25f;
    ky = (f32)ps[1] * 0.25f;
    kw = (f32)ps[4] * 0.25f;
    kz = (f32)ps[5] * 0.25f;
    func_800B31C0(&m[0][0], (u32 *)(p8 + p9 * 0x40));

    fx = (f32)x;
    fy = (f32)y;
    fz = (f32)z;
    a = m[0][0] * fx + m[1][0] * fy + m[2][0] * fz + m[3][0];
    b = m[0][1] * fx + m[1][1] * fy + m[2][1] * fz + m[3][1];
    c = m[0][2] * fx + m[1][2] * fy + m[2][2] * fz + m[3][2];
    w = m[0][3] * fx + m[1][3] * fy + m[2][3] * fz + m[3][3];
    ret = 1;
    if (w != 0.0f) {
        a = a / w;
        b = b / w;
        c = c / w;
        if (c <= -1.0f) {
            return ret;
        }
        if (1.0f <= c) {
            return ret;
        }
        *p4 = (s32)(a * kx + kw);
        *p5 = (s32)(b * -ky + kz);
        *p6 = (s32)(c * 1023.0f * 0.5f + 511.5f) << 5;
        if (p7 != 0) {
            *p7 = (s32)w;
        }
        ret = 0;
    }
    return ret;
}
