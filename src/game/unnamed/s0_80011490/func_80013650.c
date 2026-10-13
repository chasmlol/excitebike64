#include "ctx.h"

void func_80013650(s32 *m, f32 *q)
{
    f32 x, y, z, w;
    f32 xy, xz, xw, yy, yz, yw, zz, zw, ww;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
    yy = y * y;
    yz = y * z;
    yw = y * w;
    zz = z * z;
    zw = z * w;
    ww = w * w;
    xy = x * y;
    xz = x * z;
    xw = x * w;
    m[0] = (s32)((0.5f - zz - ww) * 32768.0f);
    m[1] = (s32)((xw + yz) * 32768.0f);
    m[2] = (s32)((yw - xz) * 32768.0f);
    m[3] = (s32)((yz - xw) * 32768.0f);
    m[4] = (s32)((0.5f - yy - ww) * 32768.0f);
    m[5] = (s32)((zw + xy) * 32768.0f);
    m[6] = (s32)((yw + xz) * 32768.0f);
    m[7] = (s32)((zw - xy) * 32768.0f);
    m[8] = (s32)((0.5f - yy - zz) * 32768.0f);
}
