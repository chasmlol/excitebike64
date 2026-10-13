#include "ctx.h"

typedef struct {
    s32 w[5];
    s32 f14;
} Row24;

extern Row24 D_800FA860[];

void func_800BA5F0(s32 a0)
{
    D_800FA860[a0].f14 = 1;
}
