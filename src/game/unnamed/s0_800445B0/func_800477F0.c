#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
} Trio;

void func_800477F0(u8 *p, u8 v)
{
    p[0x45] = v;
    *(Trio *)(p + 0x43C) = *(Trio *)(p + 0x54);
}
