#include "ctx.h"

extern Vec3f D_800CA6F0[];
extern f32 D_800EE370[];
void func_8004281C(s32 x);
void func_8005B218(void);

void func_80047940(void) {
    s32 i;
    f32 sum;
    f32 dx, dz;

    sum = 0.0f;
    D_800EE370[0] = sum;
    for (i = 1; i < 10; i++) {
        dx = D_800CA6F0[i].x - D_800CA6F0[i - 1].x;
        dz = D_800CA6F0[i].z - D_800CA6F0[i - 1].z;
        sum += __builtin_sqrtf(dx * dx + dz * dz);
        D_800EE370[i] = sum;
    }
    func_8004281C(0);
    func_8005B218();
}

