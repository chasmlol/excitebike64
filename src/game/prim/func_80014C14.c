#include "ctx.h"

void func_80014C14(u8 *p, s32 stride, s32 rows) {
    u8 *q = p + stride * (rows - 1);
    s32 i;
    s32 j;
    for (i = 0; i < rows / 2; i++) {
        for (j = 0; j < stride; j++) {
            u8 t = *p;
            *p = *q;
            p++;
            *q = t;
            q++;
        }
        q -= stride * 2;
    }
}
