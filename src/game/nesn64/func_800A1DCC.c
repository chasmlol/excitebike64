#include "ctx.h"

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Chan;

extern s32 D_800D05E0[];
extern u8 D_80384015;

void func_800A1DCC(s32 n, s32 m)
{
    u8 *B;
    Chan *c;
    s32 v;
    s32 bVar1;
    s32 k;
    u8 *p;

    B = (u8 *)0x80380000;
    k = n * 4;
    k += 0x4003;
    p = B + k;
    v = B[n * 4 + 0x4002] | ((*p & 7) << 8);
    c = (Chan *)0x8039412C + n;
    if (v == 0) {
        v = 1;
    }
    v = 0x36EE8 / v;
    v = (v << 14) / 0x5622;
    c->f00 = v;
    bVar1 = *p >> 3;
    if (bVar1 & 1) {
        bVar1 >>= 1;
        if (bVar1 == 0) {
            c->f14 = 0x7e;
        } else {
            c->f14 = bVar1;
        }
    } else {
        c->f14 = D_800D05E0[bVar1 >> 1];
    }
    if ((*(u8 *)(0x80384000 + n * 4) & 0x20) != 0) {
        c->f1c = 0;
        c->f18 = 0;
    } else {
        c->f1c = 1;
    }
    if (m == 3) {
        c->f18 = 0;
    }
    if (((D_80384015 >> n) & 1) != 0) {
        c->f20 = c->f20 | 8;
    } else {
        c->f20 = c->f20 & ~8;
    }
    c->f08 = 0xffff;
    bVar1 = *(u8 *)(0x80384000 + n * 4);
    if ((bVar1 & 0x80) != 0) {
        c->f0c = 0;
        return;
    }
    c->f0c = 0x40000 / ((bVar1 & 0x7f) + 1);
}
