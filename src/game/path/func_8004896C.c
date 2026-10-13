#include "ctx.h"

typedef struct Ent {
    f32 f00;
    f32 f04;
    f32 f08;
    f32 f0c;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    f32 f20;
    f32 f24;
    u8 pad28[0x1E];
    s16 next;
    s16 child;
    u8 pad4a[0x22];
} Ent;

extern Ent *D_800EE498;

f32 func_800228A0(f32 *p);

void func_8004896C(s32 i, s32 end)
{
    Vec3f a;
    Vec3f b;
    f32 p;
    f32 q;
    f32 fa;
    f32 fb;
    s32 nx;

    do {
        nx = D_800EE498[i].next;
        D_800EE498[i].f24 = (D_800EE498[nx].f04 - D_800EE498[i].f04) / D_800EE498[i].f0c;
        a = *(Vec3f *)&D_800EE498[i].f00;
        b = a;
        p = D_800EE498[i].f14 * 0.25f;
        q = -D_800EE498[i].f18 * 0.25f;
        a.x = a.x + p;
        a.z = a.z - q;
        b.x = b.x - p;
        b.z = b.z + q;
        fa = func_800228A0((f32 *)&a);
        fb = func_800228A0((f32 *)&b);
        D_800EE498[i].f20 = (fa - fb) * 2.0f;
        if (D_800EE498[i].child >= 0) {
            func_8004896C(D_800EE498[i].child, end);
        }
        i = nx;
    } while (i != end);
}
