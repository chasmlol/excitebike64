#include "ctx.h"

extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E8D64;
extern s8 *D_800EA990;

float func_8002ED38(float x, float z)
{
    s32 ix, iz, h0, h1, h2;
    float rz, f0, f2;

    x *= 0.5f;
    z *= 0.5f;
    ix = x;
    D_800E8508 = 1;
    if (ix < 0) {
        ix--;
    }
    iz = z;
    x -= ix;
    if (iz < 0) {
        iz--;
    }
    D_800E8958 = ((ix >> 2) & 0xffff) + ((iz >> 2) << 16);
    z -= iz;
    rz = 1.0f - z;
    D_800E8D64 = ((ix & 3) + (iz & 3) * 4) * 2;
    if (x < rz) {
        D_800E8D64 = D_800E8D64 + 1;
    }
    ix &= 0x1ff;
    iz &= 0x1ff;
    if (x < rz) {
        float f1;
        h0 = D_800EA990[iz * 0x200 + ix];
        h1 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        f1 -= f0;
        f2 = D_800EA990[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + f1 * z + (f2 - f0) * x) / 1000.0f;
    } else {
        float f1, d;
        h0 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        d = f1 - f0;
        f1 = 1.0f - x;
        f2 = D_800EA990[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + d * f1 + (f2 - f0) * rz) / 1000.0f;
    }
}
