#include "ctx.h"

void func_80011A58(s32 *p, s32 a, s32 b, s32 c);
void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);
void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_8000EB00(void *a, ...);
extern u8 D_800D60C0[];

static __inline__ void mulrot(s32 *in, s32 *t, s32 *out, s32 rx, s32 ry, s32 rz)
{
    func_80011A58(t, rx, ry, rz);
    func_80011C1C(in, t, out);
}

void func_80012C00(void)
{
    s32 a[10];
    s32 b[10];
    s32 c[10];
    s16 ang[4];
    s32 w;
    s16 y;
    s16 x;
    s32 *pa;

    func_80011A58(a, 0, 0, 0x2000);
    pa = a;
    func_8001254C(pa[2], a[0], &ang[3], &w);
    mulrot(pa, b, c, 0, 0, -ang[3]);
    func_8001254C(c[1], c[0], &y, &w);
    mulrot(c, b, pa, 0, -y, 0);
    func_8001254C(pa[5], pa[4], &x, &w);
    mulrot(pa, b, c, -x, 0, 0);
    ang[0] = x;
    ang[1] = y;
    ang[2] = ang[3];
    func_8000EB00(D_800D60C0, x, y, ang[3]);
}
