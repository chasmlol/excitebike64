#include "ctx.h"

void func_80012D94(s32 *p)
{
    s32 t;
    t = p[1];
    p[1] = p[3];
    p[3] = t;
    t = p[2];
    p[2] = p[6];
    p[6] = t;
    t = p[5];
    p[5] = p[7];
    p[7] = t;
}
