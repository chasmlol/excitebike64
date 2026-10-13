#include "ctx.h"

extern s32 D_800EE128;

void func_80043768(u8 *buf)
{
    s32 step;
    s32 u;
    s32 k;
    s32 seed;
    s32 v;
    s32 t;

    step = 0x40;
    do {
        u = step;
        if (step < 0x80) {
            k = step * 2;
            do {
                D_800EE128 = (D_800EE128 * 0x35df + 0x3038) % 0xfffb;
                seed = D_800EE128;
                t = ((s32)buf[u - step] + (s32)buf[u + step]) >> 1;
                v = ((seed & 0xff) - 0x80) * step;
                if (v < 0) {
                    v = v + 0x1f;
                }
                t = t + (v >> 5);
                if (t <= 0) {
                    t = 1;
                }
                if (t > 0xff) {
                    t = 0xff;
                }
                buf[u] = (u8)t;
                u = u + k;
            } while (u < 0x80);
        }
        step = step >> 1;
    } while (step != 0);
}
