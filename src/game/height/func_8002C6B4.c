#include "ctx.h"

typedef struct {
    f32 x, y, z;
    u8 pad[0x6c - 12];
} Key;

extern s32 D_800EE490;
extern Key *D_800EE498;

void func_8002C6B4(s16 param_1, f32 param_2, f32 *param_3, f32 *param_4, f32 *param_5)
{
    s32 i0, i1;
    s32 t;

    if (0.0f <= param_2) {
        i0 = param_1;
        i1 = i0 + 1;
        if (i1 >= D_800EE490) {
            i1 -= D_800EE490;
        }
    } else {
        t = param_1;
        i0 = t - 1;
        i1 = t;
        if (i0 < 0) {
            i0 = D_800EE490 - 1;
        }
        param_2 = param_2 + 1.0f;
    }
    *param_3 = D_800EE498[i0].x + (D_800EE498[i1].x - D_800EE498[i0].x) * param_2;
    *param_4 = D_800EE498[i0].y + (D_800EE498[i1].y - D_800EE498[i0].y) * param_2;
    *param_5 = D_800EE498[i0].z + (D_800EE498[i1].z - D_800EE498[i0].z) * param_2;
}
