#include "ctx.h"

typedef struct {
    u8 p0[8];
    f32 f8;
    u8 p1[0x65 - 0xC];
    s8 f65;
    u8 p2[0x6C - 0x66];
} Seg;
extern Seg *D_800EE498;
extern f32 D_800E76C8;

void func_8006B2AC(u8 *p)
{
    s32 i0;
    s32 i1;
    s32 side;
    s32 other;
    s32 t0;
    s32 t1;
    s32 sh1;
    s32 sh2;
    Seg *s;
    f32 f;
    f32 d;
    f32 h;
    f32 n;

    i0 = *(s16 *)(p + 0x628);
    i1 = i0;
    side = (s32)((*(f32 *)(p + 0x48) + 1.089f) / 2.178f);
    other = side ^ 1;
    sh1 = side << 1;
    t0 = (D_800EE498[i1].f65 >> sh1) & 3;
    if (t0 == 0) {
        for (;;) {
            i1++;
            {
                f32 dd = D_800EE498[i1].f8 - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t0 = (D_800EE498[i1].f65 >> sh1) & 3;
            if (t0 != 0) break;
        }
    }
    sh2 = other << 1;
    t1 = (D_800EE498[i0].f65 >> sh2) & 3;
    if (t1 == 0) {
        for (;;) {
            i0++;
            {
                f32 dd = D_800EE498[i0].f8 - *(f32 *)(p + 0x50);
                if (!(-40.0f < dd)) break;
            }
            t1 = (D_800EE498[i0].f65 >> sh2) & 3;
            if (t1 != 0) break;
        }
    }
    if ((t0 == 1 && (t1 != t0 || i1 < i0)) || t1 == 2) {
        side = other;
        if (10.0f < *(f32 *)(p + 0x420)) {
            *(s32 *)(p + 0x5B0) &= 0xFFFDFFEF;
        }
    }
    f = side * 2.178f;
    h = *(f32 *)(p + 0x48);
    d = f - h;
    *(f32 *)(p + 0x42C) = -f;
    if (0.21779999f < d) {
        n = h + *(f32 *)(p + 0x420) * D_800E76C8 / 10.0f;
    } else if (d < -0.21779999f) {
        n = h - *(f32 *)(p + 0x420) * D_800E76C8 / 10.0f;
    } else {
        goto done;
    }
    *(f32 *)(p + 0x48) = n;
    *(f32 *)(p + 0x42C) = -n;
done:
    if (*(s32 *)(p + 0x744) <= 0) {
        if (t0 == 3) {
            *(s16 *)(p + 0x416) = 0x10;
            *(s16 *)(p + 0x414) = 0;
            return;
        }
        if (*(s16 *)(p + 0x416) != 0) {
            *(s16 *)(p + 0x416) -= 1;
        }
    }
}
