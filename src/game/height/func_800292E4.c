#include "ctx.h"

typedef struct {
    f32 x, y, z;
    u8 pad0C[0x46 - 0x0C];
    s16 next;
    u8 pad48[0x64 - 0x48];
    u8 flags;
    u8 pad65[0x6C - 0x65];
} Seg;

typedef struct {
    u8 pad0[0x54];
    f32 vx;
    f32 f58;
    f32 vz;
    u8 pad60[0xB8 - 0x60];
    f32 dx;
    f32 fBC;
    f32 dz;
    u8 padC4[0x416 - 0xC4];
    s16 f416;
    u8 pad418[0x494 - 0x418];
    f32 px, py, pz;
    u8 pad4A0[0x4C0 - 0x4A0];
    f32 f4C0;
    f32 f4C4;
    f32 f4C8;
    u8 pad4CC[0x628 - 0x4CC];
    s16 seg;
} Obj;

extern Seg *D_800EE498;
extern s32 D_800E8508;

f32 func_800228A0(f32 *p);
s32 func_80013010(s32 x);

f32 func_800292E4(Obj *self)
{
    s32 i;
    s32 k;
    s32 n;
    f32 arr[3];
    f32 f0;
    f32 f3;
    f32 t;
    f32 dx, dz;
    f32 q1, q2;

    i = self->seg;
    f3 = 0.0f;
    if ((D_800EE498[i].flags & 1) == 0) {
        f0 = self->vx * self->dx + self->vz * self->dz;
        if (f0 < 2.0f) {
            f0 = 2.0f;
        }
        if (16.0f < f0) {
            f0 = 16.0f;
        }
        arr[0] = self->px + self->dx * f0;
        arr[2] = self->pz + self->dz * f0;
        arr[1] = self->py;
        t = func_800228A0(arr);
        if (D_800E8508 != 0) {
            f3 = (self->py - t) * (8192.0f / f0);
        }
    } else {
        k = i;
        for (n = 5; n >= 0; n--) {
            k = D_800EE498[k].next;
        }
        dx = D_800EE498[k].x - self->px;
        dz = D_800EE498[k].z - self->pz;
        t = __builtin_sqrtf(dx * dx + dz * dz);
        f3 = -(D_800EE498[k].y - self->py) * (8192.0f / t);
    }
    if (f3 < -2048.0f) {
        f3 = -2048.0f;
    } else if (2048.0f < f3) {
        f3 = 2048.0f;
    }
    if (self->f416 == 0) {
        f3 = f3 + 2000.0f;
    } else {
        f3 = f3 + 0.0f;
    }
    q1 = self->vx;
    q2 = self->vz;
    f0 = __builtin_sqrtf(q1 * q1 + q2 * q2);
    if (f0 == 0.0f) {
        f0 = 1.0f;
    }
    q1 = q1 / f0;
    q2 = q2 / f0;
    f0 = self->f4C0 * q1 + self->f4C8 * q2;
    if (0.2f < f0) {
        f0 = (f32)((s16)func_80013010((s32)(f0 * 16384.0f)) + 0x400);
        if (f3 < f0) {
            f3 = f0;
        }
    }
    return f3;
}
