#include "ctx.h"

extern s32 D_800E77D8[];

void func_80028590(void)
{
    s32 i;
    s32 *p;

    i = 0;
    p = D_800E77D8;
    do {
        i = i + 1;
        if (*p != 0) {
            *p = *p - 1;
        }
        p = p + 1;
    } while (i < 8);
}
