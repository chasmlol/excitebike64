#include "ctx.h"

extern f32 D_800CA6F0[];
f32 func_80013098(f32 p1, f32 p2);
void func_80013310(f32 *a, f32 *b, f32 *o);
void func_8005C5BC(u8 *s, s32 m);

void func_80047F7C(u8 *s)
{
    f32 v[3];
    f32 o[3];
    f32 f3, f4, f5, d, sum;
    f32 *tp;
    s8 *cp;
    u32 uVar;

    v[0] = D_800CA6F0[s[0x402] * 3] - *(f32 *)(s + 0x48);
    tp = &D_800CA6F0[s[0x402] * 3];
    v[1] = tp[1] - *(f32 *)(s + 0x4C);
    tp = &D_800CA6F0[s[0x402] * 3];
    v[2] = tp[2] - *(f32 *)(s + 0x50);
    func_80013310(v, (f32 *)(s + 0xA0), o);
    f3 = func_80013098(o[2], o[0]);
    sum = o[0] * o[0] + o[1] * o[1] + o[2] * o[2];
    f5 = __builtin_sqrtf(sum);

    cp = (s8 *)(s + 0x5D0);
    *cp = (s8)(s32)(-f3 * 14.0f);
    if (*cp > 14) *cp = 14;
    if (*cp < -14) *cp = -14;

    if (!(0.0f < f3)) f3 = -f3;

    f4 = 5.0f;
    if (!(f5 < 5.0f)) {
        if (f5 < 20.0f) {
            if (f3 < 0.1f) f4 = 20.0f;
        } else {
            f4 = 10.0f;
            if (f3 < 1.0f) f4 = 40.0f;
        }
    }
    if (s[0x3FF] == 1) goto zero;
    if (s[0x3FF] == 2) goto five;
    goto after;
zero:
    f4 = 0.0f;
    goto after;
five:
    f4 = 5.0f;
after:

    d = *(f32 *)(s + 0x420) - f4;
    if (0.5f < d) {
        uVar = 6;
        *(s16 *)(s + 0x55C) = uVar;
        *(s16 *)(s + 0x4B8) = uVar;
        uVar = 0x20;
        goto store;
    }
    if (!(d < -0.5f) || *(s32 *)(s + 0x744) >= 3 || s[0x7A6] != 0) goto done;
    uVar = 0x10;
    if (!(d < -5.0f)) goto store;
    uVar = 0x20000;
    if (*(f32 *)(s + 0x648) < 9500.0f) goto store;
    uVar = 0x10;
store:
    *(u32 *)(s + 0x5B0) = uVar;
done:
    func_8005C5BC(s, 1);
}
