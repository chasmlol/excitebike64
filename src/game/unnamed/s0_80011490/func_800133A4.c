#include "ctx.h"

void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r)
{
    r->x = a->y * b->z - b->y * a->z;
    r->y = a->z * b->x - b->z * a->x;
    r->z = a->x * b->y - b->x * a->y;
}
