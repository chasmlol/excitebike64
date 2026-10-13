#include "ctx.h"

typedef struct { s32 m[9]; } Mtx3;
typedef struct { s32 x, y, z; } Vec3i;

typedef struct {
    u8 pad0[8];
    s32 len;
    s32 len2;
    Mtx3 rot;
    Vec3i pos;
    Mtx3 wrot;
    u8 pad64[0x1C];
} Joint;

typedef struct {
    u8 pad0[0x18];
    Vec3i v;
} Obj;

extern s32 D_800EA774;
void func_80012D94(Mtx3 *p);
void func_80012DC8(Vec3i *a, Mtx3 *b, Vec3i *c);
u32 func_8000EB18(u32 r);
void func_80011A58(Mtx3 *p, s32 a, s32 b, s32 c);
void func_80011C1C(Mtx3 *a, Mtx3 *b, Mtx3 *c);
void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);

void func_8003C8E0(Joint *j, s32 idx, Vec3i *tgt, Obj *o)
{
    Mtx3 m;
    Mtx3 m2;
    Vec3i t;
    Vec3i d;
    s16 a1;
    s32 w;
    s16 a2, a3, a4;
    s32 rot, dist, n, l1, l2, x, rad;

    t.x = -(o->v.x * 80) / 16384 + tgt->x;
    t.y = -(o->v.y * 80) / 16384 + tgt->y;
    t.z = -(o->v.z * 80) / 16384 + tgt->z;
    d.x = t.x - j[idx].pos.x;
    d.y = t.y - j[idx].pos.y;
    d.z = t.z - j[idx].pos.z;
    m = j[1].wrot;
    func_80012D94(&m);
    func_80012DC8(&d, &m, &t);
    if (idx == 3) {
        rot = -0x2000;
    } else {
        rot = 0x2000;
    }
    rot = rot * D_800EA774 / 256;
    func_80011A58(&m, 0, rot, 0);
    func_80012DC8(&t, &m, &d);
    dist = func_8000EB18(d.x * d.x + d.y * d.y + d.z * d.z);
    n = j[idx + 1].len2 + j[idx + 2].len2;
    if (n < dist) {
        d.x = d.x * n / dist;
        d.y = d.y * n / dist;
        d.z = d.z * n / dist;
        dist = n;
    }
    l1 = j[idx + 1].len2;
    l2 = j[idx + 2].len2;
    x = (l1 + l2) * (l1 - l2);
    x = x / (dist * 2) + dist / 2;
    {
        s32 sq = x * x;
        rad = func_8000EB18(l1 * l1 - sq);
    }
    func_8001254C(rad, x, &a1, &w);
    func_8001254C(rad, dist - x, &a2, &w);
    if (idx == 6) {
        a1 = -a1;
        a2 = -a2;
    }
    func_8001254C(d.x, d.z, &a3, &w);
    a3 = -a3;
    w = func_8000EB18(d.x * d.x + d.z * d.z);
    func_8001254C(d.y, w, &a4, &w);
    a4 = -a4;
    func_80011A58(&m, a4, 0, a3 + a1);
    func_80011A58(&m2, 0, -rot, 0);
    func_80011C1C(&m, &m2, &j[idx].rot);
    func_80011A58(&j[idx + 1].rot, 0, 0, -(a1 + a2));
}
