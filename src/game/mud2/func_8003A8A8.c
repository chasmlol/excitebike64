#include "ctx.h"

extern s32 D_800E1898;
extern Vec3f D_800F0B20;

void func_80013310(f32 *a, f32 *b, f32 *o);

void func_8003A8A8(u8 *p)
{
    f32 buf[8];

    if (D_800E1898 == 1) {
        buf[0] = D_800F0B20.x - *(f32 *)(p + 0x48);
        buf[1] = D_800F0B20.y - *(f32 *)(p + 0x4C);
        buf[2] = D_800F0B20.z - *(f32 *)(p + 0x50);
        func_80013310(buf, (f32 *)(p + 0xA0), (f32 *)(p + 0x354));
        *(s32 *)(p + 0x350) = 0xF;
    }
}
