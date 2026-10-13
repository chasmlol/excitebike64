#include "game.h"

s32 func_80061F80(s8 *p, s16 v)
{
    return *(s16 *)(p + 0x660) >= v;
}

s32 func_80061F98(u8 *a0, s16 a1)
{
    return *(s16 *)(a0 + 0x660) <= a1;
}

u8 func_80061FB0(void)
{
    D_800F0B30 = D_800F0B30 ^ 1;
    return D_800F0B30;
}

s32 func_80061FC8(u8 *a0)
{
    return *(s16 *)(a0 + 0x660) > 0;
}

u8 func_80061FD4(s8 *p, s16 v)
{
    if (*(s16 *)(p + 0x660) < v) {
        return 0;
    }
    D_800F0B30 = D_800F0B30 ^ 1;
    return D_800F0B30;
}

s32 func_80062008(void) {
    return D_800E76D3 & 1;
}

#define ABS(x) ((x) > 0.0f ? (x) : -(x))

struct Obj_43 {
    u8 pad[0x42C];
    f32 f42C;
    u8 pad2[0x660 - 0x430];
    s16 x660;
};

s32 func_80062018(Obj_43 *p, s16 s) {
    return p->x660 < s || ABS(p->f42C) > 0.5f;
}

s32 func_80062090(void)
{
    return 0;
}
