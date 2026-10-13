#include "ctx.h"

extern u32 D_800F0600;
extern f32 D_800E37BC;
extern f32 D_800E76C8;

void func_8001CDE0(u8 *p)
{
    f32 a514;
    f32 f3, f5, a470;
    s16 sv;

    f3 = 0.5f;
    a514 = *(f32 *)(p + 0x514);
    a470 = *(f32 *)(p + 0x470);
    *(u32 *)(p + 0x4bc) = 0;
    f5 = a514 + a470;
    *(u32 *)(p + 0x560) = 0;
    if (*(f32 *)(p + 0x4bc) < f5) {
        f3 = a514 / f5;
    }
    if ((*(u32 *)(p + 0x5b0) & 0x20) != 0) {
        f5 = 6.0f;
        sv = *(s16 *)(p + 0x55c) + 1;
        *(s16 *)(p + 0x55c) = sv;
        if (f5 < (f32)sv) {
            *(s16 *)(p + 0x55c) = 6;
        }
        if (*(s32 *)(p + 0x4fc) < 1) {
            *(f32 *)(p + 0x560) = ((f32)*(s16 *)(p + 0x55c) * 2572.5f * f3) / f5;
        }
        sv = *(s16 *)(p + 0x4b8) + 1;
        *(s16 *)(p + 0x4b8) = sv;
        if (f5 < (f32)sv) {
            *(s16 *)(p + 0x4b8) = 6;
        }
        if (*(s32 *)(p + 0x458) < 1) {
            *(f32 *)(p + 0x4bc) = ((f32)*(s16 *)(p + 0x4b8) * 2572.5f * (1.0f - f3)) / f5;
        }
    } else {
        *(s16 *)(p + 0x55c) = 0;
        *(s16 *)(p + 0x4b8) = 0;
    }
    f5 = *(f32 *)(p + 0x424);
    if (0.0f < f5) {
        *(u8 *)(p + 0x6e9) = 0;
    } else {
        *(u8 *)(p + 0x6e9) = 1;
    }
    if ((D_800F0600 & 0x10) == 0) {
        if ((*(u32 *)(p + 0x5b0) & 0x20010) != 0 && f5 < 0.0f) {
            f32 m, q;
            m = f3 * -2572.5f;
            q = -2572.5f - m;
            *(f32 *)(p + 0x560) = m;
            *(f32 *)(p + 0x4bc) = q;
            return;
        }
        if (*(s16 *)(p + 0x416) == 0) {
            if (f5 < D_800E37BC) {
                goto neg;
            }
            return;
        }
        if (!(f5 < 0.0f)) {
            return;
        }
neg:
        *(f32 *)(p + 0x560) = -*(f32 *)(p + 0x560);
        *(f32 *)(p + 0x4bc) = -*(f32 *)(p + 0x4bc);
        return;
    }
    {
        f32 m, dv;
        m = f5 * 250.0f;
        dv = D_800E76C8 + D_800E76C8;
        *(s16 *)(p + 0x412) = 0x3000;
        m = m / dv;
        *(f32 *)(p + 0x560) = m;
        *(f32 *)(p + 0x4bc) = m;
        return;
    }
}
