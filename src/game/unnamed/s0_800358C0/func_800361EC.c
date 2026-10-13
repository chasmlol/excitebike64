#include "ctx.h"

void func_8000ED10(void *a0, s32 a1, s32 a2);

void func_800361EC(Vec3f *p)
{
    func_8000ED10(p, 0, 4);
    p->x = p->z = 0.0f;
}
