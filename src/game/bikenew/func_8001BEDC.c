#include "ctx.h"

typedef struct {
    u8 pad0[0x18];
    f32 f18;
} Sub;

typedef struct {
    u8 pad0[0x8];
    Vec3f pos;
    f32 f14;
    f32 f18;
    f32 f1C;
    u8 pad20[0x40 - 0x20];
    f32 v40[3];
    u8 pad4C[0x68 - 0x4C];
    f32 f68;
    f32 v6C[3];
    f32 v78[3];
    f32 v84[3];
    Sub *p90;
    u8 pad94[0x98 - 0x94];
    f32 f98;
    f32 f9C;
    f32 fA0;
} Cam;

typedef struct {
    u8 pad0[0x48];
    f32 v48[3];
    f32 v54[3];
    u8 pad60[0x88 - 0x60];
    f32 v88[3];
    u8 pad94[0xAC - 0x94];
    f32 vAC[3];
    u8 padB8[0x41B - 0xB8];
    u8 b41B;
    u8 pad41C[0x448 - 0x41C];
    f32 f448;
    u8 pad44C[0x5B0 - 0x44C];
    u32 flags;
} Bike;

extern f32 func_80013408(f32 *a, f32 *b);
extern f32 func_800ADFB0(f32);
extern f32 D_800C8DF8[];
extern f32 D_800C9978;
extern s32 D_800EFDBC;

#define LIM(a) (((a)->flags & 0x10000) ? 250.0f : 1000.0f)

void func_8001BEDC(Bike *a, Cam *b)
{
    f32 q[3];
    f32 n[3];
    f32 d[3];
    f32 scale, ref, s;
    f32 len3, dv, side, k, len, len2, sq, base, x2, y2, t, v;

    d[0] = b->v40[0] - a->v48[0];
    d[1] = b->v40[1] - a->v48[1];
    d[2] = b->v40[2] - a->v48[2];
    n[0] = a->v88[1] * d[2] - d[1] * a->v88[2];
    n[1] = a->v88[2] * d[0] - d[2] * a->v88[0];
    n[2] = a->v88[0] * d[1] - d[0] * a->v88[1];
    n[0] += a->v54[0];
    n[1] += a->v54[1];
    n[2] += a->v54[2];
    side = func_80013408(n, b->v78);
    ref = func_80013408(n, b->v84);
    scale = D_800C8DF8[10];
    b->fA0 = scale * b->p90->f18;
    s = D_800C8DF8[10];
    if (a->flags & 0x20000) {
        k = D_800C9978 / s;
    } else {
        k = D_800C8DF8[11] / s;
    }
    if (a->b41B == 1) {
        b->fA0 = b->fA0 / 3.0f;
        k = 1.0f;
    }
    x2 = b->f14;
    x2 = x2 * x2;
    y2 = b->f18;
    y2 = y2 * y2;
    sq = x2 + y2;
    len = __builtin_sqrtf(sq);
    if (len > 0.0f) {
        b->fA0 = b->fA0 * (sq / (x2 + y2 / k));
    }
    base = func_80013408(b->v6C, a->vAC);
    t = b->f1C - func_80013408(&b->pos.x, a->vAC);
    t = t / base;
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->flags & 0x10000) {
            v = 250.0f;
        }
    }
    if (len < v * b->fA0 || D_800EFDBC == 15) {
        d[0] = b->v6C[0] * t;
        d[1] = b->v6C[1] * t;
        d[2] = b->v6C[2] * t;
        b->pos.x += d[0];
        b->pos.y += d[1];
        b->pos.z += d[2];
        b->f9C = 0.0f;
        b->f98 = ref;
        return;
    }
    y2 = ref - b->f98;
    len2 = __builtin_sqrtf(y2 * y2 + side * side);
    b->f9C = len2;
    if (len2 > 2.0f) {
        y2 = -y2 / len2;
        d[0] = b->v84[0] * y2;
        d[1] = b->v84[1] * y2;
        d[2] = b->v84[2] * y2;
        side = -side / len2;
        q[0] = b->v78[0] * side;
        q[1] = b->v78[1] * side;
        q[2] = b->v78[2] * side;
        q[0] += d[0];
        q[1] += d[1];
        q[2] += d[2];
        x2 = side * side;
        y2 = y2 * y2;
        b->fA0 = (scale * (x2 + y2)) / (x2 + y2 / k);
    } else {
        *(Vec3f *)q = b->pos;
        len3 = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
        if (len3 != 0.0f) {
            q[0] /= len3;
            q[1] /= len3;
            q[2] /= len3;
        }
    }
    v = base + func_80013408(q, a->vAC) * b->fA0;
    if (v < 0.5f) {
        v = 0.5f;
    }
    t = b->f1C / v;
    if (t < 0.0f) {
        t = 0.0f;
    }
    v = t;
    if (t > 4000.0f) {
        v = 4000.0f;
    }
    if (v < LIM(a)) {
        v = 1000.0f;
        if (a->flags & 0x10000) {
            v = 250.0f;
        }
    }
    len = v;
    len *= b->fA0;
    b->pos.x = t * b->v6C[0] + len * q[0];
    b->pos.y = t * b->v6C[1] + len * q[1];
    b->pos.z = t * b->v6C[2] + len * q[2];
    b->f98 += ((a->f448 - b->f68) - func_80013408(&b->pos.x, b->v84)) / 9765.625f;
    if (b->f98 < 0.0f) {
        b->f98 = 0.0f;
    }
    dv = b->f98 - ref;
    if (dv > 10.0f) {
        b->f98 = ref + 10.0f;
    } else if (dv < -10.0f) {
        b->f98 = ref - 10.0f;
    }
}
