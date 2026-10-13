#include "ctx.h"

typedef struct {
    u8 pad28[0x28];
    u32 flags;
} Info;

typedef struct {
    u8 pad0[0x40];
    u16 r40;
    u16 r42;
    u8 f44;
    u8 pad45[0x420 - 0x45];
    f32 f420;
    f32 f424;
    u8 pad428[0x458 - 0x428];
    s32 f458;
    u8 pad45C[0x4E4 - 0x45C];
    Info *info;
    u8 pad4E8[4];
    f32 f4EC;
    u8 pad4F0[0x5B0 - 0x4F0];
    u32 flags;
    u8 pad5B4[0x644 - 0x5B4];
    f32 rpm;
    s32 f648;
    f32 f64C;
    s32 f650;
    u8 f654;
    u8 gear;
    u8 f656;
    u8 pad657[0x7E0 - 0x657];
} Obj;

extern Obj D_800E37C0[];
extern s32 D_800E76D0;
extern u32 D_800F0600;
extern f32 D_800C9944[];
extern f32 D_800C9970;
extern f32 D_800C9974;
extern f32 D_800C996C;

extern void func_80035A84(Obj *o);
extern f32 func_80036108(f32 x);
extern u32 func_8000EFE8(void);

void func_80035BAC(s32 idx, s32 dt)
{
    Obj *o;
    f32 fs0, v, fv;
    s32 a1;

    o = &D_800E37C0[idx];
    func_80035A84(o);
    {
        f32 z = 0.0f;
        o->f64C = z;
        fs0 = z;
    }
    if (o->f44 != 0) {
        o->flags &= ~0x20010;
    }
    if (o->f458 < 1 && (D_800F0600 & 0x10) == 0) {
        if (o->f420 < 4.5f && (o->flags & 0x20010) != 0) {
            v = o->rpm;
            if (v < 6500.0f) {
                o->rpm = v + (f32)(dt * 1000);
                if (6500.0f < o->rpm) {
                    o->rpm = 6500.0f;
                }
            } else {
                o->rpm = v - (f32)(dt * 1000);
                if (o->rpm < 6500.0f) {
                    o->rpm = 6500.0f;
                }
            }
        } else {
            o->rpm = o->f4EC * D_800C9944[o->gear];
        }
        fs0 = func_80036108(o->rpm);
        if (D_800E76D0 - o->f650 >= 16) {
            if (o->gear != 0 && o->rpm < 6500.0f) {
                fv = func_80036108(o->f420 * D_800C9944[o->gear - 1]);
                if (fs0 < fv) {
                    o->gear--;
                    o->f650 = D_800E76D0;
                    fs0 = fv;
                }
            }
            if (o->gear < 3 && 9000.0f < o->rpm) {
                fv = func_80036108(o->f420 * D_800C9944[o->gear + 1]);
                if (fs0 < fv) {
                    o->gear++;
                    o->f650 = D_800E76D0;
                    fs0 = fv;
                }
            }
        }
        if (o->info->flags & 0x200) {
            o->f648 = 0;
        }
    } else if (o->flags & 0x20000) {
        o->rpm = o->rpm + (f32)(dt * 10000 / 15);
    } else if (o->flags & 0x10) {
        o->rpm = o->rpm + (f32)(dt * 10000 / 30);
    } else {
        o->rpm = o->rpm - (f32)(dt * 10000 / 60);
    }
    o->f656 = (o->flags & 0x20010) != 0;
    if (o->rpm < 1000.0f) {
        o->rpm = 1000.0f;
    }
    if (14500.0f < o->rpm) {
        o->rpm = 14500.0f;
    }
    if ((o->flags & 0x20010) != 0 && (D_800F0600 & 0x10) == 0) {
        if (o->f458 < 1) {
            if (o->f654 != 0) {
                fv = D_800C9974;
            } else if (o->flags & 0x20000) {
                fv = D_800C996C;
            } else {
                fv = D_800C9970;
            }
            fs0 = fs0 * fv;
            o->f64C = fs0;
        }
    } else if (1.0f < o->f420) {
        o->f64C = -3728.4995f;
    } else {
        o->f64C = 0.0f;
    }
    a1 = o->f424 * 65536.0f / 188.49556f;
    if (a1 > 0x1000) {
        a1 = func_8000EFE8() << 8;
    }
    o->r40 += a1;
    if ((o->flags & 0x20010) != 0 || 1000.0f < o->rpm) {
        a1 = o->rpm / D_800C9944[o->gear] * 65536.0f / 188.49556f;
        if (a1 > 0x1000) {
            a1 = func_8000EFE8() << 8;
        }
    }
    o->r42 += a1;
}
