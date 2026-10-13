#include "ctx.h"

#define EL(m, i) (((s32)(m)[i] << 16) | (u16)(m)[(i) + 16])

void func_800120A4(s16 *a, s16 *b, s16 *o)
{
    s32 r, c, k, t, p, n, u;
    f32 sum;

    for (r = 0; r < 4; r++) {
        c = 0;
        p = r * 4;
        n = p;
    next:
        sum = 0.0f;
        do {
            for (k = 0; k < 4; k++) {
                t = EL(a, n + k);
                u = EL(b, k * 4 + c);
                sum += (f32)t * (f32)u * (1.0f / 65536.0f);
            }
        } while (0);
        if (sum < -1073741800.0f || 1073741800.0f < sum) {
            sum = 0.0f;
            do {
                for (k = 0; k < 4; k++) {
                    t = EL(a, n + k);
                    u = EL(b, k * 4 + c);
                    t = (f32)t * 0.25f;
                    u = (f32)u * 0.25f;
                    sum += (f32)t * (f32)u * (1.0f / 65536.0f);
                }
            } while (0);
            t = (s32)sum << 4;
            o[p] = t >> 16;
            o[p + 16] = t;
        } else {
            t = (s32)sum;
            o[p] = t >> 16;
            o[p + 16] = t;
        }
        p++;
        c++;
        if (c < 4) {
            goto next;
        }
    }
}
