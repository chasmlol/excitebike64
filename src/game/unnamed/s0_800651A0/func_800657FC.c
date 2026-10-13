#include "ctx.h"

extern s32 D_800F13A8[];

void func_800657FC(s32 a0, s32 a1)
{
    s32 i;
    s32 *p;

    i = 0;
    p = D_800F13A8;
    do {
        i = i + 1;
        if (*p == 0) {
            *p = a1;
            return;
        }
        p = p + 1;
    } while (i < 4);
}
