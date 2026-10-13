#include "ctx.h"

extern f32 D_800C94B0;
extern f32 D_800C8E14;
extern s32 D_800EFDBC;
extern s32 D_800E76D0;
extern s32 D_800E76C4;

void func_8001BA5C(s32 p, f32 v);
void func_800445B0(u8 *s, s32 k);
void func_80022908(u8 *s);

#define F(off) (*(f32 *)(s + (off)))
#define W(off) (*(s32 *)(s + (off)))

void func_8001BC98(u8 *s)
{
    f32 x;
    f32 m1;
    f32 one, fv2, B, A3, t1, t2, t3, fs1v, m2;

    x = F(0x1BC);
    m1 = x * 0.65f;
    m2 = x * 0.6f;
    A3 = ((m1 / 1.25f) - 11.5199995f) * 14.700001f;
    B = ((m2 / 1.25f) + 11.5199995f) * 14.700001f;
    one = 1.0f;
    fv2 = one - one / D_800C94B0;
    t1 = fv2 * B;
    if (D_800EFDBC == 6) {
        fs1v = t1 / (D_800C8E14 * 20000.0f);
    } else {
        fs1v = t1 / 15000.0f;
    }
    t2 = fv2 * A3;
    if (D_800EFDBC == 6) {
        t3 = t2 / (D_800C8E14 * 20000.0f);
    } else {
        t3 = t2 / 15000.0f;
    }
    func_8001BA5C((s32)(s + 0x4F8), t3);

    if (((F(0x420) + 5.0f) * 250.0f * 30.0f) < F(0x514)) {
        W(0x520) = 0;
        W(0x47C) = 0;
        func_800445B0(s, 0x1c);
    }
    func_8001BA5C((s32)(s + 0x454), fs1v);
    if (((F(0x420) + 5.0f) * 250.0f * 30.0f) < F(0x470)) {
        W(0x520) = 0;
        W(0x47C) = 0;
        func_800445B0(s, 0x19);
    }
    if (3675.0002f < F(0x514) || 3675.0002f < F(0x470)) {
        if (0x78 < W(0x5C8) - D_800E76D0) {
            func_800445B0(s, 0x1c);
        }
    } else {
        W(0x5C8) = D_800E76D0;
    }
    if (W(0x4FC) == D_800E76C4 || W(0x458) == D_800E76C4) {
        func_80022908(s);
    }
}
