#include "ctx.h"

extern u8 D_800E3E20[];
extern u8 D_800E37C0[];
extern u8 D_800F05A8[];
extern u8 D_800EFD70[];
extern s32 D_800EFDBC;
extern s32 D_800C8CE8;
extern s32 D_800C8CE0;
extern s32 D_800E1898;
extern u8 *D_800CA930;
extern u8 D_800CA938[];
extern s16 D_800F05CC;

void func_8001106C(void);
u32 func_80050CF4(void *a);

u32 func_80050F7C(s32 p0)
{
    s32 p = p0;
    u8 *s0;
    u8 *s2;
    u8 *v;
    u8 *q;
    u8 *tb;
    s32 a0;
    s32 a1;
    s32 rv;
    s32 a2;
    s32 ti;
    u32 t1;
    u16 hv;
    f32 fv;
    u8 *w;
    u8 *w2;

    a1 = ((p << 6) - p) << 5;
    s0 = D_800E3E20 + a1;
    rv = *(s16 *)(s0 + 8);
    if (rv >= 0) {
        return rv;
    }
    v = D_800F05A8;
    *(u8 *)(v + *(s16 *)(v + 0x24) + 0x50) = p;
    a0 = *(u16 *)(v + 0x24);
    *(u16 *)(v + 0x24) = a0 + 1;
    *(u16 *)(s0 + 8) = a0;
    *(s32 *)(s0 + 0x2C) = *(s32 *)(v + 0x2C);
    if (*(u8 *)(D_800E3E20 + a1 + 0x88) == 0 && D_800C8CE8 == 0) {
        if ((D_800EFDBC != 8 || p == D_800C8CE0) && D_800E1898 == 1) {
            func_8001106C();
        }
    }
    s2 = D_800E37C0 + ((((p << 6) - p)) << 5);
    t1 = func_80050CF4(s2);
    if ((*(s32 *)D_800EFD70 & 0x8000) == 0) {
        a2 = *(s32 *)(D_800EFD70 + 0x4C);
        if (a2 == 1) {
            { s32 ix = (p * 7 + *(s16 *)(D_800CA930 + 0x64)) * 2; *(u16 *)(D_800CA930 + ix + 0x10) = *(u16 *)(s0 + 8); }
            ti = *(s16 *)(s0 + 8);
            ti = (s16)ti;
            tb = (u8 *)(D_800CA938 + ti * 4);
            {
                u8 *pp = D_800CA930 + p;
                pp[0x66] = tb[3];
            }
            if (s2[0x6E8] != 0) {
                goto L1fc;
            }
            a1 = D_800F05CC;
            if (a1 != a2) {
                goto L19c;
            }
            q = *(u8 **)D_800CA930 + (*(s32 *)(D_800EFD70 + 0x410) - 1);
            q[4] = q[4] | (a1 << *(s16 *)(D_800CA930 + 0x64));
        }
    }
L19c:
    {
        u8 *bse = D_800E37C0;
        w = bse + (((p << 6) - p) << 5);
    }
    if (w[0x6E8] == 0) {
        u8 *vb = D_800F05A8;
        if (*(s16 *)(vb + 0x24) == 1) {
            *(s32 *)(vb + 0x44) = p;
        }
        do { do { do {
        *(u32 *)(s0 + 0x30) = *(u32 *)(s0 + 0x30) | 8;
        fv = *(f32 *)(w + 0x638);
        hv = *(u16 *)(w + 0x628);
        } while (0); } while (0); } while (0);
        w[0x6E8] = 1;
        *(f32 *)(w + 0x42C) = fv;
        *(u16 *)(w + 0x734) = hv;
        *(u16 *)(w + 0x714) = hv;
    }
L1fc:
    {
        u8 *bse2 = D_800E37C0;
        w2 = bse2 + (((p << 6) - p) << 5);
    }
    w2[0x3FF] = 1;
    *(f32 *)(w2 + 0x42C) = *(f32 *)(w2 + 0x638);
    return t1 & 0xff;
}
