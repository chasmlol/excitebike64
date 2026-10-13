#include "ctx.h"

f32 func_800228A0(f32 *p);
f32 sqrtf(f32 x);

extern s32 D_800C9760;
extern s32 D_800C2280;
extern s32 D_800E8508;
extern u8 *D_800EE498;

f32 func_8002A4EC(u8 *param_1)
{
    f32 fs;
    f32 v[3];
    f32 len;
    f32 d;
    f32 t;
    f32 k8;
    f32 s2;
    u8 fl;
    s32 *gp = &D_800C9760;

    fl = D_800EE498[*(s16 *)(param_1 + 0x628) * 0x6C + 0x64];
    fs = (f32)*gp;
    if ((fl & 1) == 0) {
        s2 = *(f32 *)(param_1 + 0xB8) * *(f32 *)(param_1 + 0xB8) +
             *(f32 *)(param_1 + 0xC0) * *(f32 *)(param_1 + 0xC0);
        k8 = 8.0f;
        len = sqrtf(s2);
        v[0] = *(f32 *)(param_1 + 0x494) + (*(f32 *)(param_1 + 0xB8) * k8) / len;
        v[2] = *(f32 *)(param_1 + 0x49C) + (*(f32 *)(param_1 + 0xC0) * k8) / len;
        v[1] = *(f32 *)(param_1 + 0x498);
        d = func_800228A0(v);
        v[1] = d;
        if (D_800E8508 != 0) {
            t = *(f32 *)(param_1 + 0x498) - d;
            fs = t * 1024.0f;
            if ((D_800C2280 & 8) == 0) {
                if (fs < -2048.0f) fs = -2048.0f;
                else if (fs > 1024.0f) fs = 1024.0f;
            } else {
                if (fs < -1024.0f) fs = -1024.0f;
                else if (fs > 4096.0f) fs = 4096.0f;
            }
        }
    }
    return fs + 2000.0f;
}
