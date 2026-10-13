#include "ctx.h"

typedef struct Bike {
    u8 pad0[0x18];
    s32 f18;
    u8 pad1C[0x8];
    s32 f24;
    u8 pad28[0x1C];
    u8 f44;
    u8 pad45[0x3];
    Vec3f pos;      /* 0x48 */
    Vec3f vel;      /* 0x54 */
    u8 pad60[0x28];
    Vec3f f88;
    u8 pad94[0xC];
    Vec3f fA0;
    Vec3f fAC;
    u8 padB8[0x2F8];
    s32 f3B0;
    u8 pad3B4[0x1C];
    s8 f3D0;
    u8 pad3D1[0x45];
    s16 f416;
    u8 pad418[0x8];
    f32 f420;
    u8 pad424[0x34];
    s32 f458;
    u8 pad45C[0x38];
    Vec3f f494;
    u8 pad4A0[0x5C];
    s32 f4FC;
    u8 pad500[0xB0];
    s32 f5B0;
    u8 pad5B4[0x4];
    s32 f5B8;
    u8 pad5BC[0x10];
    u8 f5CC;
    u8 f5CD;
    u8 pad5CE[0x2];
    s8 f5D0;
    u8 pad5D1;
    s8 f5D2;
    u8 pad5D3[0x4F];
    s16 f622;
    u8 pad624[0x4];
    u16 f628;
    u8 pad62A[0x2A];
    u8 f654;
    u8 pad655[0x93];
    u8 f6E8;
    u8 pad6E9[0x2];
    u8 f6EB;
    u8 f6EC;
    u8 pad6ED[0x3];
    f32 f6F0;
    f32 f6F4;
    f32 f6F8;
    Vec3f f6FC;
    Vec3f f708;
    s16 f714;
    u8 pad716[0x2];
    f32 f718;
    Vec3f f71C;
    Vec3f f728;
    s16 f734;
    u8 pad736[0x2];
    f32 f738;
    s32 f73C;
    s16 f740;
    u8 pad742[0x2];
    s32 f744;
    s32 f748;
    Vec3f f74C;
    f32 f758;
    u8 pad75C[0x5C];
    f32 f7B8;
    u8 pad7BC[0xC];
    f32 f7C8;
    u8 pad7CC[0x14];
} Bike;

typedef struct {
    u8 pad0[0x28];
    s32 f28;
    u8 pad2C[0x14];
} Surf;

typedef struct {
    u8 pad0[4];
    s32 f4;
} Trk;

typedef struct {
    u8 pad0[0xC];
    Trk *fC;
    u8 pad10[0x2C];
} Ent;

typedef struct {
    u8 pad0[0x64];
    u8 f64;
    u8 pad65[0x7];
} Seg;

extern Bike D_800E37C0[];
extern u8 D_800E76C7;
extern s32 D_800E76C4;
extern s32 D_800E76D0;
extern s32 D_800E84F8;
extern s32 D_800EFDBC;
extern f32 D_800C8E04;
extern Surf D_800C9080[];
extern Ent D_800C9A40[];
extern Seg *D_800EE498;

void func_80070560();
f32 func_80013408(f32 *a, f32 *b);
s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
void func_80011378(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_8002F4B0(f32 *p);
s32 func_8002E920(s32 x, s32 y, s32 z);
void func_80020504(Bike *b);
void func_80020B18(Bike *b);
f32 func_80020744(Bike *b, f32 k, f32 v, Vec3f *p);
void func_8006FE84(Bike *b, s32 m);

static inline s32 hgt(Vec3f *v) {
    return func_8002E920(v->x * 1000.0f, v->y * 1000.0f, v->z * 1000.0f);
}

void func_80021068(Bike *b) {
    volatile s32 pad[12];
    Vec3f t;
    Vec3f c;
    Vec3f p;
    f32 x, z, len;
    f32 fs2, fs3, fs4;
    f32 sp, th, e, d;
    f32 s1v, t1v, d1, g, m, c2, d2;
    s32 n, r;
    f32 k;

    if (b->f5CD != 0) {
        b->f5CD += D_800E76C7;
        if (b->f5CD >= 60) {
            b->f5CD = 0;
        }
    }
    if (b->f748 == 1 && (b->f44 != 0 || b->f3D0 != -1)) {
        b->f748 = -1;
    }
    if (b->f458 > 0 && b->f4FC > 0) {
        if (b->f744 <= 0) {
            b->f740 = 0;
            b->f6F8 = (b->vel.y + b->vel.y) / 10.106251f;
            b->f718 = (b->vel.y + b->vel.y) / 19.29375f;
            b->f6F4 = (b->vel.y + b->vel.y) / 14.700001f;
            func_80070560(b);
            s1v = b->f420;
            t1v = 0.5f;
            if (!(s1v > 20.0f)) {
                t1v = 0.25f;
                if (s1v > 10.0f) {
                    t1v = (s1v - 10.0f) / 40.0f + 0.25f;
                }
            }
            if (t1v < b->f6F4) {
                if (b->f416 != 0 && b->f416 < 45) {
                    b->f416 = 0;
                }
                d1 = func_80013408(&b->fA0.x, &b->f88.x);
                b->f88.x -= d1 * b->fA0.x;
                b->f88.y -= d1 * b->fA0.y;
                b->f88.z -= d1 * b->fA0.z;
                if (b->f6E8 == 0 && b->f44 == 0 && D_800E76D0 - b->f5B8 < 10 && (b->f5B0 & 0x20000)) {
                    sp = b->f420;
                    if (sp < 20.0f && b->f654 == 0) {
                        th = 5.0f;
                        if (sp > 10.0f) {
                            th = 5.0f - (sp - 10.0f) * 0.5f;
                        }
                        if (sp != 0.0f) {
                            sp = th / sp + 1.0f;
                        } else {
                            sp = 0.0f;
                        }
                        b->vel.x *= sp;
                        b->vel.y *= sp;
                        b->vel.z *= sp;
                        if (b->f24 >= -0x17FF) {
                            b->f88.x -= b->fA0.x * 3.0f;
                            b->f88.y -= b->fA0.y * 3.0f;
                            b->f88.z -= b->fA0.z * 3.0f;
                        }
                        func_80011378(7, b - D_800E37C0, 0xC0, 0xC0, 1);
                    }
                }
                if (D_800EFDBC == 6) {
                    Vec3f *q = &b->f74C;
                    if (b->f420 > 4.0f) {
                    th = q->x; sp = q->z; if (b == 0) r = 0; func_8002E920(th * 1000.0f, q->y * 1000.0f, sp * 1000.0f);
                    if (D_800C9080[(D_800E84F8 >> 4) & 0xF].f28 & 0x400) {
                        f32 k;
                        k = 10.0f;
                        g = D_800C8E04 * k / b->f420 + 1.0f;
                        b->vel.x *= g;
                        b->vel.y *= g;
                        b->vel.z *= g;
                    }
                    }
                }
            }
            t.x = b->vel.x * b->f6F8;
            t.y = b->vel.y * b->f6F8;
            t.z = b->vel.z * b->f6F8;
            b->f6FC.x = t.x + b->pos.x;
            b->f6FC.y = t.y + b->pos.y;
            b->f6FC.z = t.z + b->pos.z;
            t.x = b->vel.x * b->f718;
            t.y = b->vel.y * b->f718;
            t.z = b->vel.z * b->f718;
            b->f71C.x = t.x + b->pos.x;
            b->f71C.y = t.y + b->pos.y;
            b->f71C.z = t.z + b->pos.z;
            b->f5CC = 0;
            b->f622 = 0;
            {
                u16 h = b->f628;
                b->f734 = h;
                b->f714 = h;
            }
            b->f744 = D_800E76C4;
        } else {
            b->f744 += D_800E76C4;
        }
        if (b->f744 >= 4) {
            b->f718 = func_80020744(b, -19.29375f, b->f718, &b->f71C);
            func_8002F4B0(&b->f728.x);
            b->f6F8 = func_80020744(b, -10.106251f, b->f6F8, &b->f6FC);
            func_8002F4B0(&b->f708.x);
            b->f6F4 = (b->f6F8 + b->f718) * 0.5f;
            if (b->f5B0 & 0x10000) {
                n = b->f5D0;
                if (b->f3D0 == -1 && b->f6F4 > 0.5f) {
                    if (n > 10) {
                        func_8006FE84(b, 16);
                    } else if (n < -10) {
                        func_8006FE84(b, 17);
                    }
                    if (b->f3D0 != -1) {
                        b->f3B0 = (f32)(D_800C9A40[b->f3D0].fC->f4 << 9) / (b->f6F4 * 60.0f);
                    }
                }
                if (b->f3D0 == 16) {
                    if (n <= 0) {
                        n = 1;
                    }
                    b->f622 = n << 9;
                } else if (b->f3D0 == 17) {
                    if (n >= 0) {
                        n = -1;
                    }
                    b->f622 = n << 9;
                }
            }
            if (b->f6E8 == 0) {
                if (b->f3D0 == -1 && !(b->f5B0 & 0x100)) {
                    m = 0.015625f;
                    if (b->f5D2 < 0) {
                        m = b->f7C8;
                    }
                    b->f738 = b->f5D2 * m;
                    if (b == 0) r = 0;
                    b->f73C = b->f5D2;
                } else {
                    b->f73C = 0;
                    b->f738 = b->f7C8 * -15.0f;
                }
            } else {
                n = 5;
                if (b->f744 >= 6) {
                    n = 1;
                }
                x = b->vel.x;
                z = b->vel.z;
                len = __builtin_sqrtf(x * x + z * z);
                if (len == 0.0f) {
                    len = 1.0f;
                }
                x /= len;
                z /= len;
                fs3 = b->f71C.y * 14.700001f;
                fs4 = (x * b->f728.x + z * b->f728.z) / b->f728.y;
                if (D_800EFDBC != 5) {
                    b->f734 = func_8004B5F0(&b->f71C, b->f734, n, b->f6EB);
                    if (D_800EE498[b->f734].f64 & 0x10) {
                        fs3 -= 1000.0f;
                    }
                }
                fs2 = b->f6FC.y * 14.700001f;
                e = b->f728.x * b->vel.x + b->f728.y * (b->vel.y - b->f718 * 17.640001f) + b->f728.z * b->vel.z;
                x = (x * b->f708.x + z * b->f708.z) / b->f708.y;
                fs3 -= e * e;
                if (D_800EFDBC != 5) {
                    b->f714 = func_8004B5F0(&b->f6FC, b->f714, n, b->f6EB);
                    if (D_800EE498[b->f714].f64 & 0x10) {
                        fs2 -= 500.0f;
                    }
                }
                e = b->f708.x * b->vel.x + b->f708.y * (b->vel.y - b->f6F8 * 11.760001f) + b->f708.z * b->vel.z;
                b->f6F4 = (b->f6F8 + b->f718) * 0.5f;
                fs2 -= e * e;
                if (fs3 < fs2) {
                    b->f6EC = 1;
                    b->f738 = -0.3125f;
                } else {
                    fs4 = x;
                    b->f6EC = 0;
                    b->f738 = 0.3125f;
                }
                if (b->f6F4 < 1.5f) {
                    r = fs4 * 15.0f + (f32)((s32)(fs4 * 8192.0f - (f32)b->f18) / 512);
                    if (r > 15) {
                        r = 15;
                    } else if (r < -15) {
                        r = -15;
                    }
                    b->f740 = r;
                    b->f73C = b->f740;
                }
            }
        }
        if (b->f5CC != 0) {
            func_80020B18(b);
        } else if (b->f6F4 < 0.25f && b->vel.y < 0.0f && b->f744 >= 31 && b->f416 == 0) {
            if (b->f6E8 == 0) {
                p = b->pos;
                th = p.x + b->vel.x * b->f6F4;
                p.x = th;
                m = p.z + b->vel.z * b->f6F4;
                p.z = m;
                if (b == 0) e = 1.0f; else if (n == 0) e = 2.0f; else e = 3.0f;
                p.y = (f32)func_8002E920(p.x * 1000.0f, p.y * 1000.0f, p.z * 1000.0f) / 1000.0f;
                func_8002F4B0(&t.x);
                c.x = b->fAC.y * t.z - t.y * b->fAC.z;
                c.y = b->fAC.z * t.x - t.z * b->fAC.x;
                c.z = b->fAC.x * t.y - t.x * b->fAC.y;
                d2 = func_80013408(&c.x, &b->fA0.x);
                c2 = b->f7B8 * 10.0f;
                if (d2 > 0.0f ? d2 < c2 : -d2 < c2) {
                    b->f5CC = 1;
                }
            } else {
                b->f5CC = 1;
            }
        }
        if (b->f748 != -1) {
            b->f748 = 1;
            b->f758 = __builtin_sqrtf((b->pos.x - b->f74C.x) * (b->pos.x - b->f74C.x)
                                      + (b->pos.y - b->f74C.y) * (b->pos.y - b->f74C.y)
                                      + (b->pos.z - b->f74C.z) * (b->pos.z - b->f74C.z));
        }
    } else {
        if (b->f744 > 0) {
            func_80020504(b);
        } else {
            b->f744 -= D_800E76C4;
        }
        if (b->f748 == 1) {
            b->f748 = 0;
        }
        b->f74C = b->f494;
    }
    b->f6F0 = b->vel.y;
}
