#include "ctx.h"

extern f32 func_80013408(f32 *a, f32 *b);
extern void func_800445B0(void *s, s32 k);
extern f32 D_800C8E08;
extern s32 D_800EFDBC;
extern s32 D_800E76D0;

typedef struct { s32 a, b, c; } T3;

void func_80020504(void *a0)
{
    u8 *s1 = (u8 *)a0;
    u8 *s0 = s1 + 0x54;
    f32 fs0, d;
    s32 iv, v1;

    fs0 = func_80013408((f32 *)s0, (f32 *)(s1 + 0x4C0)) + func_80013408((f32 *)s0, (f32 *)(s1 + 0x564));
    if (s1[0x3D0] < 0x10 && *(s32 *)(s1 + 0x744) >= 0xB)
        func_800445B0(s1, 0x1C);
    *(s32 *)(s1 + 0x5C0) = 0;
    if (fs0 < -10.0f)
        *(s16 *)(s1 + 0x416) = 0;
    if (D_800EFDBC == 0xF) {
        *(f32 *)(s1 + 0x7B8) = D_800C8E08;
        *(s32 *)(s1 + 0x8C) = 0;
    }
    if (*(s32 *)(s1 + 0x744) < 0x29)
        goto L1c0;
    d = *(f32 *)(s1 + 0x520) - *(f32 *)(s1 + 0x47C);
    *(f32 *)(s1 + 0x75C) = *(f32 *)(s1 + 0x758);
    if (d < -2.0f) {
        func_800445B0(s1, 0x19);
        goto L178;
    }
    if (d < -0.8f) {
        *(s8 *)(s1 + 0x45) = 0x19;
        goto L160;
    }
    if (d < -0.3f) {
        *(s8 *)(s1 + 0x5CD) = 1;
        goto L178;
    }
    if (d < 0.3f) {
        *(s8 *)(s1 + 0x5CC) = 1;
        goto L178;
    }
    if (d < 0.6f) {
        *(s8 *)(s1 + 0x5CD) = 1;
        goto L178;
    }
    *(s8 *)(s1 + 0x45) = 0x1C;
L160:
    *(T3 *)(s1 + 0x43C) = *(T3 *)(s1 + 0x54);
L178:
    if (*(s8 *)(s1 + 0x45) != 0 && -10.0f < fs0) {
        *(s8 *)(s1 + 0x45) = 0;
        *(s8 *)(s1 + 0x5CD) = 1;
    }
    if (*(u8 *)(s1 + 0x5CC) != 0) {
        f32 zf = 0.0f;
        *(f32 *)(s1 + 0x90) = zf;
        *(f32 *)(s1 + 0x8C) = zf;
        *(f32 *)(s1 + 0x88) = zf;
    }
L1c0:
    if (*(s32 *)(s1 + 0x744) < 0x15)
        goto END;
    iv = (s32)(-fs0 * 10.0f);
    *(s32 *)(s1 + 0x768) = iv;
    if (iv < 0)
        *(s32 *)(s1 + 0x768) = 0;
    if (*(s32 *)(s1 + 0x768) >= 0x100)
        *(s32 *)(s1 + 0x768) = 0xFF;
    v1 = D_800E76D0;
    *(u8 *)(s1 + 0x764) = 0xFF;
    *(s32 *)(s1 + 0x5A0) = 0;
    *(s32 *)(s1 + 0x774) = v1;
END:
    *(s32 *)(s1 + 0x744) = 0;
}
