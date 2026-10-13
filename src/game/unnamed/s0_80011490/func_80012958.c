#include "ctx.h"

extern s16 D_800C2C60[];

#define SINS(x) D_800C2C60[(x) & 0xFFF]
#define COSS(x) D_800C2C60[((x) - 0x400) & 0xFFF]

void func_80012958(f32 *m, s16 a, s16 b, s16 c)
{
    f32 sa, sb, cb, sc, cc;
    f32 ca;
    s32 ia, ib, ic;
    s32 i1, i2;
    s16 *tb;
    f32 cbca, cbsa;

    tb = D_800C2C60;
    ia = a >> 4;
    i1 = tb[ia & 0xFFF];
    i2 = tb[(ia - 0x400) & 0xFFF];
    ib = b >> 4;
    sa = i1 * 6.1035156e-05f;
    ca = i2 * 6.1035156e-05f;
    sb = tb[ib & 0xFFF] * 6.1035156e-05f;
    cb = tb[(ib - 0x400) & 0xFFF] * 6.1035156e-05f;
    ic = c >> 4;
    sc = tb[ic & 0xFFF] * 6.1035156e-05f;
    cc = tb[(ic - 0x400) & 0xFFF] * 6.1035156e-05f;
    cbca = cb * ca;
    cbsa = cb * sa;
    m[0] = sb * sc;
    m[1] = cb;
    m[2] = sb * cc;
    m[3] = -sc * cbsa + -ca * cc;
    m[4] = sa * sb;
    m[5] = -cc * cbsa + ca * sc;
    m[6] = sc * cbca + -sa * cc;
    m[7] = -ca * sb;
    m[8] = cc * cbca + sa * sc;
}
