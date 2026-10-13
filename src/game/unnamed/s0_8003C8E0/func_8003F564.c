#include "ctx.h"

typedef struct Ent {
    u8 p0[6];
    u16 f06;
    u8 p1[2];
    u16 f0A;
    u8 p2[2];
    u16 f0E;
    s32 f10;
    u8 p3[0x50];
    u16 f64;
    u16 f66;
    u16 f68;
    u16 f6A;
    u16 f6C;
    u16 f6E;
    u8 p4[0x10];
} Ent;

void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);

void func_8003F564(Ent *p, s32 n)
{
    s32 i;
    u16 x, y, z;

    for (i = 0; i < n; i++) {
        func_80012EBC(&p[i].f10, &x, &y, &z);
        p[i].f64 = x;
        p[i].f66 = y;
        p[i].f68 = z;
    }
    p->f6A = p->f06;
    p->f6C = p->f0A;
    p->f6E = p->f0E;
}
