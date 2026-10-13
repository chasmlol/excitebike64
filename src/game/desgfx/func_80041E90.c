#include "ctx.h"

typedef struct Patch {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    s16 pad19A;
    s16 bx;
    s16 by;
    u8 pad1A0[8];
} Patch;

extern Patch *D_800ED7AC;
extern s8 *D_800EA990;

static __inline__ void fdiv(s32 x, s32 d, s32 *q, s32 *r)
{
    *q = x / d;
    *r = x - *q * d;
    if (*r < 0) {
        *r += d;
        (*q)--;
    }
}

void func_80041E90(s32 n, s32 x, s32 y, s32 mode)
{
    s32 a, b, c, d;
    s32 tx, ty, px, pz, col, row, k;
    s32 h, s;
    Vtx *v;

    D_800ED7AC[n].x = x;
    D_800ED7AC[n].y = y;
    fdiv(x, 125, &a, &b);
    tx = a;
    fdiv(y, 125, &a, &b);
    ty = a;
    c = 0;
    d = 0;
    fdiv(x, 8000, &c, &a);
    x = a - 4000;
    fdiv(y, 8000, &d, &a);
    pz = a - 4000;
    k = 0;
    D_800ED7AC[n].mode = mode;
    D_800ED7AC[n].type = 2;
    D_800ED7AC[n].bx = c;
    D_800ED7AC[n].by = d;
    row = ty;
    for (a = 0; a < 5; a++) {
        s32 xx = x;
        col = tx;
        for (b = 0; b < 5; b++) {
            D_800ED7AC[n].v[k].v.ob[0] = xx;
            D_800ED7AC[n].v[k].v.ob[1] = D_800EA990[((row & 0x1FF) << 9) | (col & 0x1FF)] * 4;
            D_800ED7AC[n].v[k].v.ob[2] = pz;
            s = D_800EA990[((row & 0x1FF) << 9) | (col & 0x1FF)];
            s = (D_800EA990[(((row + 1) & 0x1FF) << 9) | (col & 0x1FF)] - s) * 8 + 0x80;
            if (s < 0) s = 0;
            if (s > 255) s = 255;
            D_800ED7AC[n].v[k].v.cn[0] = s * 245 / 256 + 10;
            D_800ED7AC[n].v[k].v.cn[1] = s * 230 / 256 + 25;
            D_800ED7AC[n].v[k].v.cn[2] = s * 170 / 256 + 85;
            D_800ED7AC[n].v[k].v.tc[0] = (b << 10) << mode;
            D_800ED7AC[n].v[k].v.tc[1] = (a << 10) << mode;
            D_800ED7AC[n].v[k].v.cn[3] = 255;
            k++;
            xx += 125 << mode;
            col += 1 << mode;
        }
        pz += 125 << mode;
        row += 1 << mode;
    }
}
