#include "ctx.h"

#define FL(o) (*(f32 *)((u8 *)b + (o)))
#define WD(o) (*(s32 *)((u8 *)b + (o)))
#define PF(o) ((f32 *)((u8 *)b + (o)))

typedef struct {
    s32 w[4];
} Quat;

typedef struct {
    u8 pad[0x28];
    s32 f28;
} Info;

typedef struct Bike Bike;

extern void func_8001327C(f32 *v, f32 *m, f32 *o);
extern void func_80013310(f32 *a, f32 *b, f32 *o);
extern void func_80013568(f32 *m, f32 *q);
extern void func_80013C20(f32 *o, f32 *p, f32 *q);
extern void func_8003E944(Bike *b);
extern void func_80012250(f32 *p, s32 *o1, s32 *o2, s32 *o3);
extern void func_800445B0(Bike *b, s32 n);
extern f32 func_800ADFC0(f32);
extern f32 func_800AEA30(f32);
extern s32 D_800E76D0;
extern s16 D_800C2C60[];

void func_8001FE1C(Bike *b)
{
    f32 tmp[3];
    f32 q[4];
    f32 o[3];
    f32 r1[4];
    f32 r2[4];
    f32 x, y, m, d, len, ang, s, s2, c, k;
    s32 v, r;

    x = FL(0x51C);
    y = FL(0x478);
    if (*((u8 *)b + 0x44) == 0) {
        func_80013310(PF(0x564), PF(0xA0), tmp);
        x += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
        func_80013310(PF(0x4C0), PF(0xA0), tmp);
        y += (1.0f - tmp[1] / __builtin_sqrtf(1.0f - tmp[0] * tmp[0])) * 0.25f;
    }
    m = 0.0f;
    WD(0x38) = x / 0.001f;
    WD(0x3C) = y / 0.001f;
    if (x > 0.2f) {
        WD(0x38) = 200;
        m = 0.2f - x;
    }
    if (y > 0.2f) {
        d = 0.2f - y;
        WD(0x3C) = 200;
        if (d < m) {
            m = d;
        }
    }
    m += 0.5f;
    if (y < -0.1f) {
        WD(0x3C) = -100;
    }
    WD(0x38) = (f32)WD(0x38) + 50.0f;
    WD(0x0) = (FL(0x48) - FL(0xAC) * m) * 1000.0f;
    WD(0x4) = (FL(0x4C) - FL(0xB0) * m) * 1000.0f;
    WD(0x8) = (FL(0x50) - FL(0xB4) * m) * 1000.0f;
    WD(0x30) = FL(0x5A0) * 32768.0f / 3.1415927f;
    *(Quat *)PF(0x370) = *(Quat *)PF(0x78);
    if (*((u8 *)b + 0x44) == 0) {
        func_80012250(PF(0x78), &WD(0x24), &WD(0x28), &WD(0x2C));
        if (*((s8 *)b + 0x3D0) == -1 || *((s8 *)b + 0x3D0) == 15) {
            v = WD(0x24);
            if (v > 0x3000) {
                func_800445B0(b, 0x19);
            } else if (v < -0x3000) {
                func_800445B0(b, 0x1C);
            }
        }
        func_8003E944(b);
        if (FL(0x3A8) != 0.0f) {
            q[0] = (1.0f - FL(0x3A8)) + FL(0x38C) * FL(0x3A8);
            q[1] = FL(0x390) * FL(0x3A8);
            q[2] = FL(0x394) * FL(0x3A8);
            q[3] = FL(0x398) * FL(0x3A8);
            len = __builtin_sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
            q[0] /= len;
            q[1] /= len;
            q[2] /= len;
            q[3] /= len;
            func_80013C20(PF(0x370), PF(0x78), q);
            func_8001327C(PF(0x39C), PF(0xA0), o);
            WD(0x0) = (f32)WD(0x0) + o[0] * (FL(0x3A8) * 1000.0f);
            WD(0x4) = (f32)WD(0x4) + o[1] * (FL(0x3A8) * 1000.0f);
            WD(0x8) = (f32)WD(0x8) + o[2] * (FL(0x3A8) * 1000.0f);
        }
        func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        if (*((u8 *)b + 0x6E8) != 0) {
            v = WD(0x1C);
            if (v > 0x1800) {
                r = 0;
            } else if (v > 0xC00) {
                r = 0x1800 - v;
            } else if (v >= -0xBFF) {
                r = WD(0x28);
            } else if (v >= -0x17FF) {
                r = -0x1800 - v;
            } else {
                r = 0;
            }
            ang = r;
            ang *= 3.1415927f;
            ang *= 3.0517578e-05f;
            ang *= 0.5f;
            c = func_800ADFC0(ang);
            s = func_800AEA30(ang);
            r1[0] = c;
            r1[1] = s * FL(0xB8);
            r1[2] = s * FL(0xBC);
            r1[3] = s * FL(0xC0);
            func_80013C20(PF(0x370), r1, PF(0x370));
            func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        } else if ((WD(0x5B0) & 0x20000) && D_800E76D0 - WD(0x5B8) < 0x3C &&
                   ((*(Info **)((u8 *)b + 0x4E4))->f28 & 4) && WD(0x744) < 0 && FL(0x420) < 5.0f) {
            if (FL(0x420) > 2.0f) {
                k = (f32)(WD(0x5B8) - (D_800E76D0 - 0x3C)) * 6.1035155e-07f / 60.0f;
            } else {
                k = 1.2207031e-06f;
            }
            ang = D_800C2C60[((((D_800E76D0 - WD(0x5B8)) << 13) >> 4) - 0x400) & 0xFFF];
            ang *= k;
            r2[0] = func_800ADFC0(ang);
            s2 = func_800AEA30(ang);
            r2[1] = s2 * FL(0xAC);
            r2[2] = s2 * FL(0xB0);
            r2[3] = s2 * FL(0xB4);
            func_80013C20(PF(0x370), r2, PF(0x370));
            func_80012250(PF(0x370), &WD(0x18), &WD(0x1C), &WD(0x20));
        }
    } else {
        func_80013568(PF(0x224), PF(0x1FC));
        WD(0xC) = FL(0x1CC) * 1000.0f;
        WD(0x10) = FL(0x1D0) * 1000.0f;
        WD(0x14) = FL(0x1D4) * 1000.0f;
    }
    WD(0x30) = FL(0x5A0) * 32768.0f / 3.1415927f;
    func_80013568(PF(0xA0), PF(0x78));
}
