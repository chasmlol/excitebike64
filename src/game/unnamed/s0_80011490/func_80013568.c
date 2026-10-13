#include "ctx.h"

void func_80013568(f32 *m, f32 *q)
{
    f32 xx, xy, xz, yy, yz, zz, wx, wy, wz;

    xx = q[1] * q[1];
    xy = q[1] * q[2];
    xz = q[1] * q[3];
    yy = q[2] * q[2];
    yz = q[2] * q[3];
    zz = q[3] * q[3];
    wx = q[0] * q[1];
    wy = q[0] * q[2];
    wz = q[0] * q[3];
    m[0] = (0.5f - yy - zz) * 2.0f;
    m[1] = (wz + xy) * 2.0f;
    m[2] = (xz - wy) * 2.0f;
    m[3] = (xy - wz) * 2.0f;
    m[4] = (0.5f - xx - zz) * 2.0f;
    m[5] = (yz + wx) * 2.0f;
    m[6] = (xz + wy) * 2.0f;
    m[7] = (yz - wx) * 2.0f;
    m[8] = (0.5f - xx - yy) * 2.0f;
}
