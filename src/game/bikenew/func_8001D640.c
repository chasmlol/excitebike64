#include "ctx.h"

typedef struct Cam Cam;
typedef struct Bike Bike;
typedef struct { u8 pad[0x18]; f32 f18; } Sub588;
typedef struct { u8 pad[0x28]; f32 f28; f32 f2C; } Glob;

#define F(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define I(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define V(p, o) ((f32 *)((u8 *)(p) + (o)))

extern Glob D_800C8DF8;
extern s32 D_800EFDBC;

f32 func_80013408(f32 *a, f32 *b);
void func_8001BEDC(Bike *a, Cam *b);

#define MINSPD ((I(b, 0x5B0) & 0x10000) ? 250.0f : 1000.0f)

void func_8001D640(Bike *b)
{
    Vec3f dir;
    Vec3f c;
    Vec3f d;
    f32 d2;
    f32 ratio;
    f32 d1, scale, sp, sl, x2, y2, ss, f, v, len, sv, t, n, m, g, e;

    if (I(b, 0x458) <= 0) {
        F(b, 0x46C) = F(b, 0x448) - F(b, 0x4BC);
        func_8001BEDC(b, (Cam *)V(b, 0x454));
    } else {
        F(b, 0x4F0) = F(b, 0x448) = F(b, 0x4BC) = F(b, 0x45C) = F(b, 0x460) = F(b, 0x464) = 0.0f;
    }
    if (I(b, 0x4FC) <= 0) {
    d.x = F(b, 0x538) - F(b, 0x48);
    d.y = F(b, 0x53C) - F(b, 0x4C);
    d.z = F(b, 0x540) - F(b, 0x50);
    c.x = F(b, 0x8C) * d.z - d.y * F(b, 0x90);
    c.y = F(b, 0x90) * d.x - d.z * F(b, 0x88);
    c.z = F(b, 0x88) * d.y - d.x * F(b, 0x8C);
    c.x += F(b, 0x54);
    c.y += F(b, 0x58);
    c.z += F(b, 0x5C);
    d1 = func_80013408(&c.x, V(b, 0x570));
    d2 = func_80013408(&c.x, V(b, 0x57C));
    scale = D_800C8DF8.f28;
    m = scale * (*(Sub588 **)((u8 *)b + 0x588))->f18;
    ratio = D_800C8DF8.f2C / scale;
    F(b, 0x598) = m;
    if (*((u8 *)b + 0x41B) == 1) {
        F(b, 0x598) = m / 3.0f;
        ratio = 1.0f;
    } else if (I(b, 0x5B0) & 0x10000) {
        F(b, 0x598) = m * 1.3f;
    }
    x2 = F(b, 0x50C);
    x2 *= x2;
    y2 = F(b, 0x510);
    y2 *= y2;
    ss = x2 + y2;
    sl = __builtin_sqrtf(ss);
    if (sl > 0.0f) {
        F(b, 0x598) = F(b, 0x598) * (ss / (x2 + y2 / ratio));
    }
    t = func_80013408(V(b, 0x564), V(b, 0xAC));
    f = F(b, 0x514) - func_80013408(V(b, 0x500), V(b, 0xAC));
    f /= t;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    if (sl < v * F(b, 0x598) || D_800EFDBC == 15) {
        d.x = F(b, 0x564) * f;
        d.y = F(b, 0x568) * f;
        d.z = F(b, 0x56C) * f;
        F(b, 0x500) += d.x;
        F(b, 0x504) += d.y;
        F(b, 0x508) += d.z;
        F(b, 0x594) = 0.0f;
        F(b, 0x590) = d2;
    } else {
    y2 = d2 - F(b, 0x590);
    len = __builtin_sqrtf(y2 * y2 + d1 * d1);
    F(b, 0x594) = len;
    if (len > 2.0f) {
        y2 = -y2 / len;
        d.x = F(b, 0x57C) * y2;
        d.y = F(b, 0x580) * y2;
        d.z = F(b, 0x584) * y2;
        d1 = -d1 / len;
        x2 = d1 * d1;
        dir.x = F(b, 0x570) * d1;
        y2 = y2 * y2;
        dir.y = F(b, 0x574) * d1;
        dir.z = F(b, 0x578) * d1;
        dir.y += d.y;
        dir.x += d.x;
        dir.z += d.z;
        F(b, 0x598) = scale * (x2 + y2) / (x2 + y2 / ratio);
    } else {
        dir = *(Vec3f *)V(b, 0x500);
        n = __builtin_sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (n != 0.0f) {
            dir.x /= n;
            dir.y /= n;
            dir.z /= n;
        }
    }
    sp = t + func_80013408(&dir.x, V(b, 0xAC)) * F(b, 0x598);
    if (sp < 0.5f) sp = 0.5f;
    f = F(b, 0x514) / sp;
    if (f < 0.0f) f = 0.0f;
    v = f;
    if (v > 4000.0f) v = 4000.0f;
    if (v < MINSPD) v = MINSPD;
    sl = v;
    sl *= F(b, 0x598);
    F(b, 0x500) = f * F(b, 0x564) + sl * dir.x;
    F(b, 0x504) = f * F(b, 0x568) + sl * dir.y;
    F(b, 0x508) = f * F(b, 0x56C) + sl * dir.z;
    F(b, 0x590) += (-func_80013408(V(b, 0x500), V(b, 0x57C)) - F(b, 0x560)) / 9765.625f;
    if (F(b, 0x590) < 0.0f) F(b, 0x590) = 0.0f;
    e = F(b, 0x590) - d2;
    if (e > 10.0f) {
        F(b, 0x590) = d2 + 10.0f;
    } else if (e < -10.0f) {
        F(b, 0x590) = d2 - 10.0f;
    }
    }
    } else {
        F(b, 0x594) = F(b, 0x560) = F(b, 0x500) = F(b, 0x504) = F(b, 0x508) = 0.0f;
    }
}
