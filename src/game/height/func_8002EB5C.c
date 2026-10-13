#include "ctx.h"

extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E8D64;
extern s8 *D_800EA990;

void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);

s32 func_8002EB5C(s32 x, s32 z)
{
    struct {
        s32 q;
        s32 rx;
        s32 rz;
    } L;
    s32 *qp;
    s32 ix, lt, v, fx, fz, h0, h1, h2;

    qp = &L.q;
    D_800E8508 = 1;
    func_8004262C(x, 2000, qp, &L.rx);
    ix = L.q;
    func_8004262C(z, 2000, qp, &L.rz);
    z = L.q;
    D_800E8958 = ((ix >> 2) & 0xffff) + ((z >> 2) << 16);
    v = ((ix & 3) + (z & 3) * 4) * 2;
    D_800E8D64 = v;
    if (L.rx < 1999 - L.rz) {
        D_800E8D64 = v + 1;
    }
    ix &= 0x1ff;
    z &= 0x1ff;
    fx = L.rx;
    fz = L.rz;
    if (fx < 1999 - fz) {
        h0 = D_800EA990[z * 0x200 + ix];
        h1 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = D_800EA990[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * fz + (h2 - h0) * fx) / 2000;
    } else {
        h0 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = D_800EA990[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * (2000 - L.rx) + (h2 - h0) * (2000 - L.rz)) / 2000;
    }
    return L.q;
}
