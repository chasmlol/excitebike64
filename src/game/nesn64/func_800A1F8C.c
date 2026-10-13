#include "ctx.h"

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
} Chan;

extern u8 D_80384015;
extern s32 D_800D0620[];
extern s32 D_800D05E0[];

void func_800A1F8C(s32 n, s32 m)
{
    u8 *B;
    Chan *c;
    s32 v;
    s32 b;
    u8 *p;
    u8 *q;
    s32 k;
    s32 t;
    s32 e, mm;
    s32 *tb;

    tb = D_800D0620;
    B = (u8 *)0x80380000;
    k = n * 4 + 0x4002;
    v = tb[B[k] & 0xF];
    v = (v * 0x28F) / 0xDC;
    p = B + n * 4;
    c = (Chan *)0x8039412C + n;
    c->f00 = v * 4;
    b = p[0x4003] >> 3;
    if (b & 1) {
        b >>= 1;
        if (b == 0) {
            c->f14 = 0x7e;
        } else {
            c->f14 = b;
        }
    } else {
        c->f14 = D_800D05E0[b >> 1];
    }
    if ((*(u8 *)(0x80384000 + n * 4) & 0x20) != 0) {
        c->f1C = 0;
        c->f18 = 0;
    } else {
        c->f1C = 1;
    }
    if (m == 3) {
        c->f18 = 0;
    }
    if (((D_80384015 >> n) & 1) != 0) {
        c->f20 = c->f20 | 8;
    } else {
        c->f20 = c->f20 & ~8;
    }
    if (m == 0) {
        c->f08 = 0xffff;
    }
    k = n * 4;
    q = (u8 *)(0x80384000 + k);
    e = *q;
    t = e & 0x10;
    mm = e & 0xF;
    if (t != 0) {
        c->f08 = (mm << 16) / 16;
        c->f0C = 0;
        c->f20 = c->f20 & ~0x10;
    } else {
        c->f0C = 0x4000 / (mm + 1);
        if ((*q & 0x20) != 0) {
            c->f20 = c->f20 | 0x10;
        } else {
            c->f20 = c->f20 & ~0x10;
        }
    }
}
