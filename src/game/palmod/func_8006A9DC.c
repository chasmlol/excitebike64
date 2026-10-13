#include "ctx.h"

void func_8006A9DC(s32 a0, s32 *a1, s32 *a2, s32 *a3)
{
    s32 t;
    s32 d;

    if (a0 > 0x80) {
        d = a0 - 0x80;
        t = d * 0xAF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a1 = (t >> 7) + 0x50;
        t = d * 0xB9;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a2 = (t >> 7) + 0x46;
        t = d * 0xFF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a3 = t >> 7;
        return;
    }
    t = a0 * 0x50;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a1 = t >> 7;
    t = a0 * 0x46;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a2 = t >> 7;
    *a3 = 0;
}
