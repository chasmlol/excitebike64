#include "ctx.h"

void func_8004262C(s32 a, s32 b, s32 *q, s32 *r)
{
    s32 quo = a / b;
    s32 rem;
    *q = quo;
    rem = a - quo * b;
    *r = rem;
    if (rem < 0) {
        *r = rem + b;
        *q = *q - 1;
    }
}
