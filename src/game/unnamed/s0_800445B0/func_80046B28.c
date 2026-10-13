#include "ctx.h"

f32 func_80046B28(f32 *a, f32 *b, f32 *h, Vec3f *v, s32 n, f32 thr)
{
    s32 idx[10];
    s32 i, j, cnt, pos;
    f32 res;
    Vec3f *pa;
    Vec3f *pb;

    res = 0.2f;
    if (a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + (b[0] * b[0] + b[1] * b[1] + b[2] * b[2]) < thr) {
        res = 0.0f;
        cnt = 0;
        for (i = 0; i < n; i++) {
            if (h[i] > -0.02f) {
                idx[cnt++] = i;
            }
        }
        if (cnt >= 3) {
            pos = 0;
            for (i = 0; i < cnt; i++) {
                j = i + 1;
                if (j == cnt) {
                    j = 0;
                }
                pa = (Vec3f *)(idx[i] * 12 + (s32)v);
                pb = (Vec3f *)(idx[j] * 12 + (s32)v);
                if ((pa->x - pb->x) * pa->z - (pa->z - pb->z) * pa->x > 0.0f) {
                    pos++;
                }
            }
            if (pos == cnt || pos == 0) {
                b[0] = b[1] = b[2] = 0.0f;
                a[0] = a[1] = a[2] = 0.0f;
                return -1.0f;
            }
        }
    }
    return res;
}
