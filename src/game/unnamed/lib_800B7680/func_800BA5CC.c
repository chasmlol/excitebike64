#include "ctx.h"

typedef struct {
    s32 a[5];
    s32 f;
} RecA;

extern RecA D_800FA860[];

void func_800BA5CC(s32 i)
{
    D_800FA860[i].f = -1;
}
