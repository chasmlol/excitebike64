#include "ctx.h"

extern u8 *D_800EE498;

s32 func_8004A788(Vec3f *pos, s32 start, s32 count, u8 mode);

s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode)
{
    s32 idx;
    f32 *e;
    u8 *base;

    idx = func_8004A788(pos, start, count, mode);
    base = D_800EE498;
    e = (f32 *)((u32)(idx * 0x6c) + (u32)base);
    if ((pos->z - e[2]) * e[5] - (pos->x - e[0]) * e[6] < 0.0f) {
        idx = *(s16 *)((u8 *)e + 0x4a);
    }
    return idx;
}
