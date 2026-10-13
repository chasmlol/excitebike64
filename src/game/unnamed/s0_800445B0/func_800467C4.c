#include "ctx.h"

typedef struct {
    u8 pad0[0xC];
    Vec3f vel;
    Vec3f f18;
    u8 pad24[0x40 - 0x24];
    Vec3f rot;
    Vec3f f4C;
    u8 pad58[0x174 - 0x58];
    f32 mass;
    f32 inertia;
} Body;

extern f32 D_800EE1FC;
extern f32 D_800EE260;

f32 func_80013408(f32 *a, f32 *b);
void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r);

void func_800467C4(Body *b, Vec3f *n, Vec3f *p, f32 maxr, Vec3f *c, f32 fr)
{
    Vec3f A;
    Vec3f B;
    volatile s32 pad[4];
    Vec3f C;
    Vec3f L;
    f32 f, d, imp, len, r, k, q;

    f = func_80013408((f32 *)n, (f32 *)p);
    L.x = n->x * f;
    L.y = n->y * f;
    L.z = n->z * f;
    L.x = p->x - L.x;
    L.y = p->y - L.y;
    L.z = p->z - L.z;
    f = L.x * L.x + L.y * L.y + L.z * L.z;
    imp = b->mass / (f * (b->mass / b->inertia) + 1.0f);
    d = -func_80013408((f32 *)c, (f32 *)n);
    D_800EE1FC = d;
    if (64.0f < d) {
        func_800133A4(&b->rot, p, c);
        c->x += b->vel.x;
        c->y += b->vel.y;
        c->z += b->vel.z;
        d = -func_80013408((f32 *)c, (f32 *)n);
    }
    d += 0.05f;
    k = (fr + 1.0f) * d * imp;
    if (k < 0.0f) {
        k = 0.0f;
    }
    A.x = c->x + n->x * d;
    A.y = c->y + n->y * d;
    A.z = c->z + n->z * d;
    len = __builtin_sqrtf(A.x * A.x + A.y * A.y + A.z * A.z);
    D_800EE260 = len;
    if (len != 0.0f) {
        A.x /= len;
        A.y /= len;
        A.z /= len;
    }
    r = 0.0f;
    if (d != 0.0f) {
        r = len / d;
    }
    if (maxr < r) {
        r = maxr;
    }
    q = -r;
    q = q * k;
    C.x = n->x * k + A.x * q;
    C.y = n->y * k + A.y * q;
    C.z = n->z * k + A.z * q;
    b->vel.x += C.x / b->mass;
    b->vel.y += C.y / b->mass;
    b->vel.z += C.z / b->mass;
    func_800133A4(p, &C, &B);
    b->rot.x += B.x / b->inertia;
    b->rot.y += B.y / b->inertia;
    b->rot.z += B.z / b->inertia;
    b->f18.z = 0.0f;
    b->f18.y = 0.0f;
    b->f18.x = 0.0f;
    b->f4C.z = 0.0f;
    b->f4C.y = 0.0f;
    b->f4C.x = 0.0f;
}
