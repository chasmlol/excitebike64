#include "ctx.h"

extern f32 D_800E76C8;
extern void func_80013310(f32 *a, f32 *b, f32 *o);
extern f32 func_80013408(f32 *a, f32 *b);
extern void func_80029B1C(u8 *p);

void func_8001FB40(u8 *param_1)
{
    f32 len;
    f32 r;

    *(f32 *)(param_1 + 0x54) = *(f32 *)(param_1 + 0x54) + *(f32 *)(param_1 + 0x60) * D_800E76C8;
    *(f32 *)(param_1 + 0x58) = *(f32 *)(param_1 + 0x58) + *(f32 *)(param_1 + 0x64) * D_800E76C8;
    *(f32 *)(param_1 + 0x5C) = *(f32 *)(param_1 + 0x5C) + *(f32 *)(param_1 + 0x68) * D_800E76C8;
    *(f32 *)(param_1 + 0x88) = *(f32 *)(param_1 + 0x88) + *(f32 *)(param_1 + 0x94) * D_800E76C8;
    *(f32 *)(param_1 + 0x8C) = *(f32 *)(param_1 + 0x8C) + *(f32 *)(param_1 + 0x98) * D_800E76C8;
    *(f32 *)(param_1 + 0x90) = *(f32 *)(param_1 + 0x90) + *(f32 *)(param_1 + 0x9C) * D_800E76C8;
    func_80013310((f32 *)(param_1 + 0x88), (f32 *)(param_1 + 0xA0), (f32 *)(param_1 + 0x430));
    len = *(f32 *)(param_1 + 0x54) * *(f32 *)(param_1 + 0x54)
        + *(f32 *)(param_1 + 0x58) * *(f32 *)(param_1 + 0x58)
        + *(f32 *)(param_1 + 0x5C) * *(f32 *)(param_1 + 0x5C);
    *(f32 *)(param_1 + 0x420) = __builtin_sqrtf(len);
    r = func_80013408((f32 *)(param_1 + 0x54), (f32 *)(param_1 + 0xB8));
    *(f32 *)(param_1 + 0x424) = r;
    if (*(u8 *)(param_1 + 0x44) == 0 && *(s8 *)(param_1 + 0x3D0) != 0xF) {
        func_80029B1C(param_1);
    }
}
