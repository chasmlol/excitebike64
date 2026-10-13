#include "ctx.h"

typedef struct {
    u8 pad0[0x10];
    f32 f10;
    f32 f14;
} Surf;

typedef struct {
    /* 0x000 */ u8 pad0[0x24];
    /* 0x024 */ s32 f24;
    /* 0x028 */ u8 pad28[0x34 - 0x28];
    /* 0x034 */ s16 f34;
    /* 0x036 */ u8 pad36[0x45 - 0x36];
    /* 0x045 */ s8 f45;
    /* 0x046 */ u8 pad46[2];
    /* 0x048 */ Vec3f v48;
    /* 0x054 */ Vec3f v54;
    /* 0x060 */ Vec3f v60;
    /* 0x06C */ u8 pad6C[0x88 - 0x6C];
    /* 0x088 */ Vec3f v88;
    /* 0x094 */ Vec3f v94;
    /* 0x0A0 */ f32 mA0[3][3];
    /* 0x0C4 */ u8 padC4[0x1BC - 0xC4];
    /* 0x1BC */ f32 f1BC;
    /* 0x1C0 */ u8 pad1C0[0x1C6 - 0x1C0];
    /* 0x1C6 */ s8 f1C6;
    /* 0x1C7 */ u8 pad1C7[0x3AC - 0x1C7];
    /* 0x3AC */ s32 f3AC;
    /* 0x3B0 */ u8 pad3B0[0x3D0 - 0x3B0];
    /* 0x3D0 */ s8 f3D0;
    /* 0x3D1 */ u8 pad3D1[0x414 - 0x3D1];
    /* 0x414 */ s16 f414;
    /* 0x416 */ s16 f416;
    /* 0x418 */ u8 pad418[3];
    /* 0x41B */ u8 f41B;
    /* 0x41C */ u8 f41C;
    /* 0x41D */ u8 pad41D[3];
    /* 0x420 */ f32 f420;
    /* 0x424 */ u8 pad424[0x430 - 0x424];
    /* 0x430 */ f32 f430;
    /* 0x434 */ f32 f434;
    /* 0x438 */ f32 f438;
    /* 0x43C */ u8 pad43C[0x458 - 0x43C];
    /* 0x458 */ s32 f458;
    /* 0x45C */ Vec3f v45C;
    /* 0x468 */ u8 pad468[0x478 - 0x468];
    /* 0x478 */ f32 f478;
    /* 0x47C */ f32 f47C;
    /* 0x480 */ u8 pad480[0x498 - 0x480];
    /* 0x498 */ f32 f498;
    /* 0x49C */ u8 pad49C[4];
    /* 0x4A0 */ Vec3f v4A0;
    /* 0x4AC */ u8 pad4AC[0x4C0 - 0x4AC];
    /* 0x4C0 */ Vec3f v4C0;
    /* 0x4CC */ u8 pad4CC[0x4E4 - 0x4CC];
    /* 0x4E4 */ Surf *f4E4;
    /* 0x4E8 */ u8 pad4E8[0x4FC - 0x4E8];
    /* 0x4FC */ s32 f4FC;
    /* 0x500 */ Vec3f v500;
    /* 0x50C */ u8 pad50C[0x51C - 0x50C];
    /* 0x51C */ f32 f51C;
    /* 0x520 */ f32 f520;
    /* 0x524 */ u8 pad524[0x53C - 0x524];
    /* 0x53C */ f32 f53C;
    /* 0x540 */ u8 pad540[4];
    /* 0x544 */ Vec3f v544;
    /* 0x550 */ u8 pad550[0x564 - 0x550];
    /* 0x564 */ Vec3f v564;
    /* 0x570 */ u8 pad570[0x588 - 0x570];
    /* 0x588 */ Surf *f588;
    /* 0x58C */ u8 pad58C[0x5B0 - 0x58C];
    /* 0x5B0 */ u32 f5B0;
    /* 0x5B4 */ u8 pad5B4[0x5C0 - 0x5B4];
    /* 0x5C0 */ s32 f5C0;
    /* 0x5C4 */ u8 pad5C4[0x5CC - 0x5C4];
    /* 0x5CC */ u8 f5CC;
    /* 0x5CD */ u8 f5CD;
    /* 0x5CE */ u8 pad5CE[2];
    /* 0x5D0 */ s8 f5D0;
    /* 0x5D1 */ u8 pad5D1;
    /* 0x5D2 */ s8 f5D2;
    /* 0x5D3 */ u8 pad5D3[0x622 - 0x5D3];
    /* 0x622 */ s16 f622;
    /* 0x624 */ u8 pad624[0x6E8 - 0x624];
    /* 0x6E8 */ u8 f6E8;
    /* 0x6E9 */ u8 pad6E9[0x738 - 0x6E9];
    /* 0x738 */ f32 f738;
    /* 0x73C */ s32 f73C;
    /* 0x740 */ u8 pad740[4];
    /* 0x744 */ s32 f744;
    /* 0x748 */ u8 pad748[0x7BC - 0x748];
    /* 0x7BC */ f32 f7BC;
    /* 0x7C0 */ u8 pad7C0[4];
    /* 0x7C4 */ f32 f7C4;
} Bike;

extern f32 D_800C94C4;
extern f32 D_800E76C8;
extern s32 D_800EFDBC;

f32 sqrtf(f32 x);
f32 func_80013408(void *a, void *b);
s32 func_8003F534(s32 a0, s32 *a1);
void func_8001327C(void *v, void *m, void *o);
void func_80013310(void *a, void *b, void *o);
f32 func_80013098(f32 p1, f32 p2);
s32 func_8002E920(s32 x, s32 y, s32 z);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);
f32 func_8001E89C(void *p);

#define ABS(x) ((x) < 0 ? -(x) : (x))

static inline f32 getHeight(Vec3f *p) {
    return (f32)func_8002E920(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f;
}

void func_8001EC90(Bike *b) {
    Vec3f F;
    Vec3f T;
    Vec3f A;
    Vec3f c2;
    Vec3f c1;
    Vec3f unused;
    Vec3f d;
    Vec3f e;
    s16 ang;
    s32 ratio;
    s16 ang2;
    s32 tmp;
    f32 grav;
    f32 save;
    Vec3f *p;
    s32 n;

    A.x = b->v45C.x + b->v500.x;
    A.y = b->v45C.y + b->v500.y;
    A.z = b->v45C.z + b->v500.z;
    if (b->f420 != 0.0f) {
        f32 lo;
        f32 k;
        f32 q;

        lo = 0.0f;
        if (b->f5CD != 0) {
            lo = 7.0f;
        }
        k = 0.21000001f;
        if (b->f744 <= 0) {
            k = (b->f416 != 0 ? (b->f4E4->f14 - 1.0f) * 0.25f + 1.0f : b->f4E4->f14) * 0.21000001f;
        }
        if (k < lo) {
            k = lo;
        }
        k = k * b->f420 * b->f420;
        if (b->f744 <= 0) {
            k += b->f4E4->f10 * sqrtf(b->v45C.x * b->v45C.x + b->v45C.y * b->v45C.y + b->v45C.z * b->v45C.z);
            k += b->f588->f10 * sqrtf(b->v500.x * b->v500.x + b->v500.y * b->v500.y + b->v500.z * b->v500.z);
            if (b->f41C != 0) {
                k += D_800C94C4;
            }
            if (b->f5C0 < -10 && b->f6E8 == 0 && !(b->f5B0 & 0x20)) {
                s32 n;
                f32 t;
                f32 u;
                n = ABS(b->f5D0);
                t = n;
                if (t > 3.0f) {
                    u = t - 3.0f;
                    k += (u > 0.0f ? u : -u) * b->f7BC;
                }
            }
        }
        q = k / b->f420;
        A.x -= b->v54.x * q;
        A.y -= b->v54.y * q;
        A.z -= b->v54.z * q;
    }
    save = b->f438;
    func_80013310(&b->v88, b->mA0, &b->f430);
    b->f438 = save;
    c1.x = b->v4A0.y * b->v45C.z - b->v45C.y * b->v4A0.z;
    c1.y = b->v4A0.z * b->v45C.x - b->v45C.z * b->v4A0.x;
    c1.z = b->v4A0.x * b->v45C.y - b->v45C.x * b->v4A0.y;
    c2.x = b->v544.y * b->v500.z - b->v500.y * b->v544.z;
    c2.y = b->v544.z * b->v500.x - b->v500.z * b->v544.x;
    c2.z = b->v544.x * b->v500.y - b->v500.x * b->v544.y;
    T.x = c2.x + c1.x;
    T.y = c2.y + c1.y;
    T.z = c2.z + c1.z;
    func_80013310(&T, b->mA0, &F);
    F.x += b->f434 * 250.0f * b->f438;
    p = &b->v48;
    if (b == 0) grav = 1.0f; else if (p == 0) grav = 2.0f; else if (b->f24 == b->f3AC) grav = 3.0f; else grav = 4.0f;
    grav = 14.700001f;
    if (b->f744 < 7
        || (f32)func_8002E920(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f - b->v48.y + 0.5f > -0.2f
        || b->f45 != 0) {
        if (b->f416 == 0) {
            f32 t;
            t = (f32)b->f5D2 - 5.0f;
            if (b->f41C != 0) {
                t = 3.0f;
            }
            F.x += t * 32.0f / 1000.0f * 1323.0001f;
            if (D_800EFDBC != 7) {
                if (b->f4FC >= 8 && b->f478 == b->f47C && b->f430 < 0.0f) {
                    F.x += 8000.0f;
                }
                if (b->f51C == b->f520 && b->f458 >= 8 && b->f430 > 0.0f) {
                    F.x -= 8000.0f;
                }
            }
        } else if (b->f416 >= 16) {
            n = -0x2800 - b->f414 * 3 - b->f24;
            F.x = ((f32)n * 3.1415927f * 3.0517578e-05f - b->f430 * 0.5f) * 15000.0f;
        } else {
            n = func_80013098(1.3f, b->f498 - b->f53C) * 32768.0f / 3.1415927f - b->f24;
            if (b->f416 != 1) {
                F.x = ((f32)n * 3.0f * 3.1415927f * 3.0517578e-05f - b->f430 * 0.5f) * 15000.0f;
            } else {
                b->v88.x = b->v88.y = b->v88.z = 0.0f;
            }
        }
    } else {
        grav = b->f738 * grav + grav;
        if (b->f3D0 == -1 || b->f3D0 == 17 || b->f3D0 == 16) {
            f32 t;
            t = (f32)b->f5D0 * b->f7C4;
            A.x -= b->v54.z * t * b->f1BC;
            A.z += b->v54.x * t * b->f1BC;
        }
        if (b->f5CC == 0) {
            if (b->f416 != 0) {
                n = -0x2800 - b->f414 * 3 - b->f24;
            } else {
                s32 r;
                r = b->f24;
                n = b->f73C;
                if (ABS(r) > 0x2000) {
                    n = 0;
                }
                n = n * 384 - r;
            }
            F.x += ((f32)(n / 4) * 3.1415927f * 3.0517578e-05f - b->f430 * 0.0625f) * 15000.0f;
        }
        if (b->f3D0 == 15) {
            s32 r;
            func_8001254C(-b->v54.x * 1000.0f, b->v54.z * 1000.0f, &ang, &ratio);
            func_8001254C(-b->mA0[2][0] * 1000.0f, b->mA0[2][2] * 1000.0f, &ang2, &ratio);
            b->f34 = ang;
            r = func_8003F534(15, &tmp);
            ang += ((b->f3AC - (r << 8)) << 8) / r;
            ang -= ang2;
            if (ang > 0xC00) {
                ang = 0xC00;
            }
            b->v88.y = -((f32)ang * 3.1415927f * 3.0517578e-05f) * 60.0f;
        } else if (b->f420 > 5.0f) {
            f32 t;
            func_8001254C(b->v54.x * 1000.0f, b->v54.z * 1000.0f, &ang, &ratio);
            func_8001254C(b->mA0[2][0] * 1000.0f, b->mA0[2][2] * 1000.0f, &ang2, &ratio);
            ang -= b->f622;
            ang -= ang2;
            t = (f32)ang * 6.1035156e-05f - b->v88.y * 0.25f;
            if (t > 0.1f) {
                t = 0.1f;
            } else if (t < -0.1f) {
                t = -0.1f;
            }
            t *= 15000.0f;
            F.x += t * b->mA0[0][1];
            F.y += t * b->mA0[1][1];
            F.z += t * b->mA0[2][1];
        }
    }
    A.y -= grav * b->f1BC;
    b->v60.x = A.x / b->f1BC;
    b->v60.y = A.y / b->f1BC;
    b->v60.z = A.z / b->f1BC;
    F.z = func_8001E89C(b) * 64.0f * 250.0f;
    F.x += F.z * b->f434 * D_800E76C8;
    func_8001327C(&F, b->mA0, &T);
    b->v94.x = T.x / 250.0f;
    b->v94.y = T.y / 250.0f;
    b->v94.z = T.z / 250.0f;
    if (b->f41B != 0 && (b->f5D0 < -10 || b->f5D0 > 10)) {
        f32 len;
        f32 t;
        d.x = b->v564.x + b->v4C0.x;
        d.y = b->v564.y + b->v4C0.y;
        d.z = b->v564.z + b->v4C0.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = func_80013408(&d, &b->v88);
        if (b->f41B == 2) {
            f32 q;
            q = t / (2.0f * D_800E76C8);
            b->v94.x -= d.x * q;
            b->v94.y -= d.y * q;
            b->v94.z -= d.z * q;
        } else {
            s32 n;
            n = t;
            n = ABS(n);
            if ((f32)n < 1.5f) {
                f32 s;
                s = D_800E76C8 * 240.0f;
                b->v94.x -= d.x * ((f32)b->f5D0 / s);
                b->v94.y -= d.y * ((f32)b->f5D0 / s);
                b->v94.z -= d.z * ((f32)b->f5D0 / s);
            }
        }
    }
    if (b->f5C0 >= -1 && b->f3D0 == -1) {
        f32 len;
        f32 t;
        s32 m;
        d.x = b->v564.x + b->v4C0.x;
        d.y = b->v564.y + b->v4C0.y;
        d.z = b->v564.z + b->v4C0.z;
        len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (len != 0.0f) {
            d.x /= len;
            d.y /= len;
            d.z /= len;
        }
        t = func_80013408(&d, &b->v88);
        m = b->f5D0;
        if ((t > 0.5f && m >= -3) || (t < -0.5f && m < 4)) {
            f32 q;
            q = t / (2.0f * D_800E76C8);
            b->v94.x = -d.x * q;
            b->v94.y = -d.y * q;
            b->v94.z = -d.z * q;
        }
    }
    if (b->f1C6 != 0) {
        f32 len;
        f32 t;
        e.x = b->v564.x + b->v4C0.x;
        e.y = b->v564.y + b->v4C0.y;
        e.z = b->v564.z + b->v4C0.z;
        len = sqrtf(e.x * e.x + e.y * e.y + e.z * e.z);
        if (len != 0.0f) {
            e.x /= len;
            e.y /= len;
            e.z /= len;
        }
        t = func_80013408(&e, &b->v88) / (D_800E76C8 * 16.0f);
        b->v94.x -= e.x * t;
        b->v94.y -= e.y * t;
        b->v94.z -= e.z * t;
    }
}
