#include "ctx.h"

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[0x54 - 0x46];
    Vec3f f54;
    u8 pad60[0x3D0 - 0x60];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[0x420 - 0x418];
    f32 f420;
    u8 pad424[0x43C - 0x424];
    Vec3f f43C;
    u8 pad448[0x458 - 0x448];
    s32 f458;
    u8 pad45C[0x4D8 - 0x45C];
    f32 f4D8[5];
    f32 f4EC;
    u8 pad4F0[0x4FC - 0x4F0];
    s32 f4FC;
    u8 pad500[0x57C - 0x500];
    f32 f57C[5];
    f32 f590;
    u8 pad594[0x662 - 0x594];
    s16 f662;
    u8 pad664[0x6E8 - 0x664];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x784 - 0x748];
    s32 f784[12];
    f32 f7B4;
    u8 pad7B8[0x7E0 - 0x7B8];
} Bike;

typedef struct {
    Vec3f pos;
    Vec3f vel;
    u8 pad18[0x40 - 0x18];
    Vec3f rot;
    u8 pad4C[0x174 - 0x4C];
    f32 mass;
    f32 inertia;
    u8 type;
    u8 pad17D[3];
    Bike *bike;
} Obj;

typedef struct {
    u8 pad0[0x10];
    Vec3f pos;
    f32 r;
} Ball;

extern Ball *D_800F0B10;
extern Ball *D_800F0B14;
extern f32 D_800F0B18;
extern Vec3f D_800F0B20;
extern Bike D_800E37C0[];
extern s32 D_800E76D0;
extern s32 D_800EFD28[];
extern s32 D_800F05D4;

f32 func_80013408(Vec3f *a, Vec3f *b);
void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r);
void func_8003A8A8(Bike *p);
void func_80010994(s32 k, s32 n, s32 vol);
void func_8004C854(s32 a, s32 b);
void func_8004D1F4(s32 a, s32 b);
s32 func_80060AA0(Bike *p1, Bike *p2);

#define NORM(v) { f32 len = __builtin_sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); if (len != 0.0f) { v.x /= len; v.y /= len; v.z /= len; } }

s32 func_800610B0(Obj *a, Obj *b) {
    Vec3f n;
    Vec3f ra;
    Vec3f rb;
    Vec3f va;
    Vec3f vb;
    Vec3f dv;
    Vec3f t;
    Vec3f tq;
    Vec3f imp;
    f32 k, len, h, d, d1, ka, kb, j, lim;
    Bike *ba, *bb, *pa, *pb;
    s32 r, vol, ia, ib;

    n.x = D_800F0B10->pos.x - D_800F0B14->pos.x;
    n.y = D_800F0B10->pos.y - D_800F0B14->pos.y;
    n.z = D_800F0B10->pos.z - D_800F0B14->pos.z;
    NORM(n);
    if (n.y > 0.25f) {
        n.y = 0.25f;
        NORM(n);
    } else if (n.y < -0.25f) {
        n.y = -0.25f;
        NORM(n);
    }
    h = D_800F0B18 * 0.5f;
    D_800F0B20.x = n.x * (D_800F0B14->r - h);
    D_800F0B20.y = n.y * (D_800F0B14->r - h);
    D_800F0B20.z = n.z * (D_800F0B14->r - h);
    D_800F0B20.x = D_800F0B14->pos.x + D_800F0B20.x;
    D_800F0B20.y = D_800F0B14->pos.y + D_800F0B20.y;
    D_800F0B20.z = D_800F0B14->pos.z + D_800F0B20.z;
    D_800F0B20.y = (a->pos.y + b->pos.y) * 0.5f;
    ra.x = D_800F0B20.x - a->pos.x;
    ra.y = D_800F0B20.y - a->pos.y;
    ra.z = D_800F0B20.z - a->pos.z;
    func_800133A4(&ra, &a->rot, &va);
    va.x = a->vel.x + va.x;
    va.y = a->vel.y + va.y;
    va.z = a->vel.z + va.z;
    rb.x = D_800F0B20.x - b->pos.x;
    rb.y = D_800F0B20.y - b->pos.y;
    rb.z = D_800F0B20.z - b->pos.z;
    func_800133A4(&rb, &b->rot, &vb);
    vb.x = b->vel.x + vb.x;
    vb.y = b->vel.y + vb.y;
    vb.z = b->vel.z + vb.z;
    dv.x = va.x - vb.x;
    dv.y = va.y - vb.y;
    dv.z = va.z - vb.z;
    d = func_80013408(&dv, &n);
    if (d > 0.0f) {
        return 0;
    }
    if (a->type < 2 && b->type < 2) {
        ba = a->bike;
        bb = b->bike;
        a->mass *= ba->f7B4;
        a->inertia *= ba->f7B4;
        b->mass *= bb->f7B4;
        b->inertia *= bb->f7B4;
        if (ba->f44 == 0 && bb->f44 == 0) {
            r = func_80060AA0(ba, bb);
            bb->f784[ba - D_800E37C0] = D_800E76D0;
            ba->f784[bb - D_800E37C0] = D_800E76D0;
            if (r != 0) {
                return r;
            }
        }
        if (a->type == 0) {
            func_8003A8A8(ba);
        }
        if (b->type == 0) {
            func_8003A8A8(bb);
        }
        vol = (s32)(-d) * 128.0f / 7.0f + 128.0f;
        if (vol > 255) {
            vol = 255;
        }
        if (vol < 0) {
            vol = 0;
        }
        if (vol > 32) {
            func_80010994(4, ba - D_800E37C0, vol);
        }
    }
    d1 = func_80013408(&n, &ra);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = ra.x - t.x;
    t.y = ra.y - t.y;
    t.z = ra.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    ka = a->mass / (k * a->mass / a->inertia + 1.0f);
    d1 = func_80013408(&n, &rb);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = rb.x - t.x;
    t.y = rb.y - t.y;
    t.z = rb.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    kb = b->mass / (k * b->mass / b->inertia + 1.0f);
    if (a->type == 4 || b->type == 4) {
        j = (d - 2.0f) * -1.5f * (ka * kb) / (ka + kb);
    } else {
        j = (d - 2.0f) * -1.1f * (ka * kb) / (ka + kb);
    }
    imp.x = n.x * (j / a->mass);
    imp.y = n.y * (j / a->mass);
    imp.z = n.z * (j / a->mass);
    a->vel.x = a->vel.x + imp.x;
    a->vel.y = a->vel.y + imp.y;
    a->vel.z = a->vel.z + imp.z;
    imp.x = n.x * (j / b->mass);
    imp.y = n.y * (j / b->mass);
    imp.z = n.z * (j / b->mass);
    b->vel.x = b->vel.x - imp.x;
    b->vel.y = b->vel.y - imp.y;
    b->vel.z = b->vel.z - imp.z;
    func_800133A4(&ra, &n, &tq);
    a->rot.x = a->rot.x + tq.x * (j / a->inertia);
    a->rot.y = a->rot.y + tq.y * (j / a->inertia);
    a->rot.z = a->rot.z + tq.z * (j / a->inertia);
    func_800133A4(&rb, &n, &tq);
    b->rot.x = b->rot.x - tq.x * (j / b->inertia);
    b->rot.y = b->rot.y - tq.y * (j / b->inertia);
    b->rot.z = b->rot.z - tq.z * (j / b->inertia);
    if (a->type < 2 && b->type < 2) {
        a->mass = 250.0f;
        a->inertia = 250.0f;
        b->mass = 250.0f;
        b->inertia = 250.0f;
        pa = a->bike;
        pb = b->bike;
        if (pa->f458 <= 0) {
            pa->f4EC = func_80013408((Vec3f *)pa->f4D8, &pa->f54);
        }
        if (pa->f4FC <= 0) {
            pa->f590 = func_80013408((Vec3f *)pa->f57C, &pa->f54);
        }
        pa->f420 = __builtin_sqrtf(pa->f54.x * pa->f54.x + pa->f54.y * pa->f54.y + pa->f54.z * pa->f54.z);
        if (pb->f458 <= 0) {
            pb->f4EC = func_80013408((Vec3f *)pb->f4D8, &pb->f54);
        }
        if (pb->f4FC <= 0) {
            pb->f590 = func_80013408((Vec3f *)pb->f57C, &pb->f54);
        }
        pb->f420 = __builtin_sqrtf(pb->f54.x * pb->f54.x + pb->f54.y * pb->f54.y + pb->f54.z * pb->f54.z);
        if ((pa->f416 != 0 || pa->f3D0 < 17) && pa->f744 <= 0) {
            pa->f45 = -25;
            pa->f43C = pa->f54;
            return 1;
        }
        if ((pb->f416 != 0 || pb->f3D0 < 16) && pb->f744 <= 0) {
            pb->f45 = -25;
            pb->f43C = pb->f54;
            return 1;
        }
        if (pa->f662 + pb->f662 == 1 && D_800F05D4 < 360 && !(pa->f6E8 & pb->f6E8)) {
            if (D_800E76D0 - D_800EFD28[pa - D_800E37C0] < 600) {
                func_8004D1F4(pb - D_800E37C0, pb->f662);
                return 1;
            }
            ib = pb - D_800E37C0;
            if (D_800E76D0 - D_800EFD28[ib] < 600) {
                func_8004D1F4(pa - D_800E37C0, pa->f662);
                return 1;
            }
            func_8004C854(pa - D_800E37C0, ib);
        }
    }
    return 1;
}
