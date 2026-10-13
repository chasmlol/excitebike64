#include "ctx.h"

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 pad0C;
    f32 pad10;
    f32 nz;
    f32 nx;
    u8 pad1C[0x10];
    f32 cx[3];
    f32 cz[3];
    u8 pad44[2];
    s16 link;
    u8 pad48[0x24];
} Node;

extern Node *D_800EE498;
extern f32 D_800EE4A0;
extern f32 D_800EE480;
extern f32 D_800EE484;

f32 func_8004A944(Vec3f *pos, s32 idx)
{
    Node *b;
    Node *a;
    Node *base;
    f32 dx, dz;
    f32 dA, dB, t, t2, t3;
    f32 sx, sz, r;

    base = D_800EE498;
    b = (Node *)(idx * sizeof(Node) + (u32)base);
    a = (Node *)(b->link * sizeof(Node) + (u32)base);
    dz = a->z - pos->z;
    dx = a->x - pos->x;
    dA = dz * a->nz - dx * a->nx;
    dz = b->z - pos->z;
    dx = b->x - pos->x;
    dB = dz * b->nz - dx * b->nx;
    t = dB / (dB - dA);
    t2 = t * t;
    t3 = t2 * t;
    dx += t3 * b->cx[0] + t2 * b->cx[1] + t * b->cx[2];
    dz += t3 * b->cz[0] + t2 * b->cz[1] + t * b->cz[2];
    sx = t2 * 3.0f * b->cx[0] + (t + t) * b->cx[1] + b->cx[2];
    sz = t2 * 3.0f * b->cz[0] + (t + t) * b->cz[1] + b->cz[2];
    dB = __builtin_sqrtf(sx * sx + sz * sz);
    D_800EE4A0 = t;
    D_800EE480 = sx / dB;
    D_800EE484 = sz / dB;
    return (sx * dz - sz * dx) / dB;
}
