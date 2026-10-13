#include "ctx.h"

void func_80077BA0(u32 *p)
{
    u32 v;
    s8 a2;
    s8 a1;
    s8 a3;
    s32 i;

    for (i = 0; i < 4; i++) {
        v = *p;
        a2 = (v >> 24) & 0xF;
        a1 = v >> 28;
        a3 = a1;
        if (a2 < 7) {
            *p = v & 0xFFFFFFF3;
            a2 = 0;
        }
        if (a1 < 7) {
            *p = *p & 0xFFFFFFFC;
            a3 = 0;
        }
        if (a2 < a3) {
            *p = *p & 0xFFFFFFF3;
        } else if (a3 < a2) {
            *p = *p & 0xFFFFFFFC;
        }
        p++;
    }
}
