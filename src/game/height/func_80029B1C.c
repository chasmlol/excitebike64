#include "ctx.h"

void func_8001254C(s32 a, s32 b, u16 *c, s32 *d);

#define FL(o) (*(f32 *)(s + (o)))
#define HW(o) (*(s16 *)(s + (o)))
#define WD(o) (*(s32 *)(s + (o)))
#define BY(o) (*(u8 *)(s + (o)))
#define SB(o) (*(s8 *)(s + (o)))

int func_80029B1C(u8 *s)
{
    struct { u16 a[2]; s32 b; u16 c[4]; } L;
    s32 iv1, iv2, a;

    if (BY(0x44) != 0) goto done;
    func_8001254C((s32)(-FL(0xB8) * 1000.0f), (s32)(FL(0xC0) * 1000.0f), L.a, &L.b);
    func_8001254C((s32)(-FL(0x54) * 1000.0f), (s32)(FL(0x5C) * 1000.0f), L.c, &L.b);
    L.b = (s16)(L.a[0] - L.c[0]);
    if (((WD(0x5B0) & 0x10000) == 0) || (WD(0x744) >= 0)) goto normal;
    a = SB(0x5D0);
    if (a < 0) a = -a;
    if (a < 6) goto normal;
    if (HW(0x36) < 0x10) HW(0x36) = HW(0x36) + 1;
    goto after;
normal:
    if (BY(0x41B) == 1) {
        if (HW(0x36) < 0x18) HW(0x36) = HW(0x36) + 1;
    } else {
        a = L.b;
        if (a < 0) a = -a;
        if (a < 0x800 && HW(0x36) != 0) HW(0x36) = HW(0x36) - 1;
    }
after:
    iv2 = 0;
    if (WD(0x744) < 0) iv2 = HW(0x36);
    if (FL(0x424) < 2.0f) iv2 = 0x20;
    iv1 = L.b * iv2;
    iv2 = (s16)L.c[0];
    if (iv1 < 0) iv1 = iv1 + 0x1f;
    L.b = iv2 + (iv1 >> 5);
    HW(0x34) = ((u16 *)&L.b)[1];
done:
    return HW(0x34);
}
