#include "ctx.h"

extern s32 D_800E8958;
extern s32 D_800E8D64;
void func_8002F1D0(s32 a, s32 b, s32 *out);
f32 sqrtf(f32 x);

void func_8002F4B0(f32 *param_1)
{
    s32 v[9];
    f32 len;
    func_8002F1D0(D_800E8958, D_800E8D64, v);
    v[3] -= v[0];
    v[6] -= v[0];
    v[4] -= v[1];
    v[5] -= v[2];
    v[7] -= v[1];
    v[8] -= v[2];
    param_1[0] = (f32)v[5] * (f32)v[7] - (f32)v[4] * (f32)v[8];
    param_1[1] = (f32)v[3] * (f32)v[8] - (f32)v[5] * (f32)v[6];
    param_1[2] = (f32)v[4] * (f32)v[6] - (f32)v[3] * (f32)v[7];
    len = param_1[0] * param_1[0] + param_1[1] * param_1[1] + param_1[2] * param_1[2];
    len = sqrtf(len);
    param_1[0] = param_1[0] / len;
    param_1[1] = param_1[1] / len;
    param_1[2] = param_1[2] / len;
}
