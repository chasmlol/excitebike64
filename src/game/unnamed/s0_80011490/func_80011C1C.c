#include "ctx.h"

void func_80011C1C(s32 *a, s32 *b, s32 *c)
{
    s32 t;

    t = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[0] = t >> 14;
    t = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[1] = t >> 14;
    t = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[2] = t >> 14;
    t = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[3] = t >> 14;
    t = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[4] = t >> 14;
    t = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[5] = t >> 14;
    t = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[6] = t >> 14;
    t = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[7] = t >> 14;
    t = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[8] = t >> 14;
}
