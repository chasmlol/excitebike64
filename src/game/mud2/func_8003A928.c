#include "ctx.h"

extern s32 *D_800EFD64;
typedef struct { s32 x, y, z; } V3i;
extern V3i D_800E8090[];
s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);

void func_8003A928(f32 *v, s32 *a1, s32 *a2, s32 *a3, s32 *a4, s32 a5, s32 idx)
{
    s32 x, y, z;
    V3i *t;

    if (D_800EFD64[11] == 0) {
        x = v[0] * 1000.0f * 0.0625f;
        y = v[1] * 1000.0f * 0.0625f;
        z = v[2] * 1000.0f * 0.0625f;
        func_80033AC8(x, y, z, a1, a2, a3, a4, a5, idx);
    } else {
        x = v[0] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].x >> 4);
        y = v[1] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].y >> 4);
        z = v[2] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].z >> 4);
        func_80033AC8(x, y, z, a1, a2, a3, a4, a5, idx);
    }
}
