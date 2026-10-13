#include "game.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
    s32 unk8;
    s32 unkC;
} Rec_18;

typedef struct {
    Rec_18 r[25];
} RecSet;

typedef struct {
    RecSet cur;
    RecSet prev;
} Both;

extern Both D_800E8EF0;
extern RecSet D_800E9080[];

void func_800358C0(void)
{
    s32 i;
    s32 a;
    s32 u1;
    s32 u3;

    func_8000ED10((u8 *)&D_800E8EF0, 0, 800);
    for (i = 0; i < 25; i++) {
        D_800E8EF0.cur.r[i].z = 0x32;
    }
    for (i = 0; i < 7; i++) {
        a = i * 0x2000 + 0xA000;
        D_800E8EF0.cur.r[i * 3].x = (D_800C2C60[((a >> 4) - 0x400) & 0xFFF] * 15) >> 12;
        D_800E8EF0.cur.r[i * 3].y = (D_800C2C60[(a >> 4) & 0xFFF] * 5) >> 11;
        u1 = (a + 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].x = (D_800C2C60[(u1 - 0x400) & 0xFFF] * 0x41) >> 14;
        u3 = (a - 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].y = (D_800C2C60[u1 & 0xFFF] * 0x2D) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].x = (D_800C2C60[(u3 - 0x400) & 0xFFF] * 0x41) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].y = (D_800C2C60[u3 & 0xFFF] * 0x2D) >> 14;
    }
    D_800E9080[0] = D_800E9080[-1];
}

struct Obj_27 {
    u8 pad0[0x5B0];
    u32 flags;      /* 0x5B0 */
    u8 pad1[0x648 - 0x5B4];
    f32 val;        /* 0x648 */
    u8 pad2[0x654 - 0x64C];
    u8 stopped;     /* 0x654 */
    u8 pad3[0x7C0 - 0x655];
    f32 div;        /* 0x7C0 */
};

static inline s32 sel(Obj_27 *o)
{
    u32 t;

    if (o->flags & 0x20000) {
        return 2;
    }
    t = o->flags & 0x10;
    return t != 0;
}

void func_80035A84(Obj_27 *o)
{
    s32 h;
    s32 g;
    f32 f;

    u32 t;

    g = sel(o);
    h = 0;
    if (o->val > 7000.0f) {
        h = 1;
    }
    if (o->stopped) {
        h = g = 0;
    }
    f = D_800C9954[h][g];
    if ((o->flags & 0x20000) && f > 0.0f) {
        f = f / o->div;
    }
    o->val = o->val + f;
    if (o->val < 0.0f) {
        o->val = 0.0f;
    }
    if (o->val > 12000.0f) {
        o->val = 12000.0f;
    }
    if (o->stopped == 0) {
        if (o->val >= 12000.0f) {
            o->stopped = 1;
            o->val = 12000.0f;
        }
    } else if (o->val <= 7000.0f) {
        o->stopped = 0;
    }
}

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
    Trk *info;
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
} Obj_28;

extern Obj_28 D_800E37C0_34[];

void func_80035BAC(s32 idx, s32 dt)
{
    Obj_28 *o;
    f32 fs0, v, fv;
    s32 a1;

    o = &D_800E37C0_34[idx];
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

f32 func_80036108(f32 x)
{
    f32 r;
    f32 lo;
    s32 i;

    if (x < 0.0f) {
        return 0.0f;
    }
    if (14500.0f < x) {
        x = 14500.0f;
    }
    i = (s32)((x * 28.0f) / 14500.0f);
    lo = D_800C98D0[i];
    r = lo;
    if (i < 0x1C) {
        r = r + ((x - (f32)((i * 0x38A4) / 0x1C)) * (D_800C98D0[i + 1] - r)) / 517.0f;
    }
    return r;
}

void func_800361EC(Vec3f *p)
{
    func_8000ED10(p, 0, 4);
    p->x = p->z = 0.0f;
}
