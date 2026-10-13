#include "ctx.h"

typedef struct { s32 x, y, z; } Vec3i;
typedef struct { s32 m[9]; } Mat3i;
typedef struct { Vec3i pos; Mat3i rot; } Joint;

typedef struct Model {
    s32 f0;
    Vec3i pos;    /* 0x04 */
    Mat3i rot;    /* 0x10 */
    Vec3i pos2;   /* 0x34 */
    Mat3i rot2;   /* 0x40 */
    u8 pad64[0x2C];
    Mat3i rot3;   /* 0x90 */
} Model;

typedef struct {
    Joint j[4];   /* 0x00 */
    Mat3i rot;    /* 0xC0 */
    s32 fE4;      /* 0xE4 */
} Cam;

typedef struct {
    Vec3i p;
    u8 pad[0x74];
} Node80;

typedef struct {
    u8 pad[0x234];
    Node80 n[6];
} Skel;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    u8 padC[0xC];
    s32 f18;
    s32 f1C;
    s32 f20;
    u8 pad24[0xC];
    s32 f30;
    u8 pad34[0x6C];
    f32 fA0, fA4, fA8;
    u8 padAC[0xC];
    f32 fB8, fBC, fC0;
    u8 padC4[0x2E8];
    s32 f3AC;
    u8 pad3B0[4];
    s32 f3B4;
    u8 pad3B8[4];
    s16 f3BC;
    u8 pad3BE[0xE];
    s32 f3CC;
    s8 f3D0;
    s8 f3D1;
    u8 pad3D2[0x3E];
    s16 f410;
    s16 f412;
    u8 pad414[2];
    s16 f416;
    u8 pad418;
    u8 f419;
    u8 f41A;
    u8 pad41B[0x191];
    s32 f5AC;
    u8 pad5B0[0x48];
    s32 f5F8;
    u8 pad5FC[0x11];
    s8 f60D;
    u8 pad60E[0x16E];
    s16 f77C;
    u8 pad77E[0x2A];
    s8 f7A8;
    u8 pad7A9[0x37];
} Bike;

typedef struct {
    s32 f0;
    u8 pad4[0x8];
    struct { u8 pad[4]; s32 f4; } *fC;
    u8 pad10[0x2C];
} Ent;

typedef struct {
    u8 pad[0xA4C];
    u8 *fA4C;
    s32 fA50;
    u8 padA54[0x2C];
} Slot;

extern s32 D_800C2358;
extern Bike D_800E37C0[];
extern Ent D_800C9A40[];
extern Cam D_800EA690;
extern s32 D_800F2140[];
extern Skel *D_800E1878;
extern s32 D_800E76D0;
extern s16 D_800C2C60[];
extern u8 D_800EA7B8[];
extern struct { u8 pad[4]; s32 f4; } *D_800C9E84;
extern Slot D_800DC150[];
extern s32 D_800E30A8;

void func_8003E330(Model *m, s32 i, s32 s);
void func_8003F564(Model *m, s32 n);
void func_80012DC8(s32 *a, s32 *b, s32 *c);
void func_80011A58(void *p, s32 a, s32 b, s32 c);
s32 func_8002E920(s32 x, s32 y, s32 z);
void func_8003F274(Model *m, s32 a, s32 b, s32 c, s32 d);
void func_8003D180(Model *m);
void func_80071678(s32 i, Model *m, s32 a, void *b);

#define B (D_800E37C0[i])
#define SINS(a) D_800C2C60[((a) >> 4) & 0xFFF]
#define COSS(a) D_800C2C60[(((a) >> 4) - 0x400) & 0xFFF]

void func_8003D4BC(Model *m, s32 i) {
    s32 v[3];
    Mat3i mtx;
    s32 out[3];
    s32 scale;
    s32 a, c, d, w, u, s3, n;
    s32 st;
    s32 fl;
    s32 t;
    s32 r, h;

    if (D_800C2358 != 0) {
        return;
    }
    st = B.f3D0;
    scale = 0x100;
    if (st >= 0) {
        fl = D_800C9A40[st].f0;
        if (fl & 1) {
            if (B.f3AC < 0x400) scale = B.f3AC / 4;
        } else if (fl & 2) {
            fl = D_800C9A40[st].fC->f4;
            if (B.f3AC >= (fl - 4) << 8) {
                scale = ((fl << 8) - B.f3AC) / 4;
            } else if (B.f3AC < 0x400) {
                scale = B.f3AC / 4;
            }
        } else if (fl & 4) {
            if (B.f3AC < 0x400 && B.f3B4 == 0) scale = B.f3AC / 4;
        } else {
            if (B.f3AC < 0x400) scale = B.f3AC / 4;
        }
    }
    if (B.f3D1 >= 25) {
        scale = 0x100;
    }
    if (B.f3D1 != -1 && scale != 0x100) {
        st = B.f3D0;
        t = B.f3AC;
        B.f3D0 = B.f3D1;
        B.f3AC = B.f3CC;
        func_8003E330(m, i, 0x100);
        func_8003F564(m, 0xF);
        B.f3D0 = st;
        B.f3AC = t;
    } else if (B.f3D0 == -1 || scale != 0x100) {
        v[0] = 0;
        a = (B.f1C * 250) / 8192;
        if (a > 0) a = -a;
        if (a < -500) a = -500;
        c = B.f410 * 10;
        if (c > 250) c = 250;
        s3 = B.f5F8;
        s3 -= B.f4;
        v[1] = a + 900 + c + s3;
        if (v[1] < 800) v[1] = 800;
        if (v[1] > 0x41A) v[1] = 0x41A;
        d = v[1] - 900;
        if (d < 0) d = 0;
        s3 = B.f60D;
        v[2] = s3 * 6 - 250;
        d *= 10;
        if (B.f77C != 0) {
            w = B.f77C;
            if (w > 256) w = 256;
            if (w < 0) w = 0;
            u = 256 - w;
            d = (d * u + w * 6000) >> 8;
            v[2] = (v[2] * u - w * 520) >> 8;
            v[1] = (v[1] * u + w * 900) >> 8;
        }
        D_800EA690.fE4 = 0x80 - s3 * 16;
        if (D_800EA690.fE4 < 0) D_800EA690.fE4 = 0;
        func_80011A58(&mtx, B.f18, (B.f1C * 7) / 8, B.f20);
        func_80012DC8(v, mtx.m, out);
        m->pos.x = m->pos2.x = out[0];
        m->pos.y = m->pos2.y = out[1];
        m->pos.z = m->pos2.z = out[2];
        func_80011A58(&m->rot, B.f18 + 4000, 0, B.f20);
        m->rot2 = m->rot;
        r = s3 * 128 - 4000;
        func_80011A58(&m->rot3, D_800F2140[i] = d - r, 0, 0);
        D_800EA690.j[0].pos = D_800E1878->n[0].p;
        D_800EA690.j[1].pos = D_800E1878->n[1].p;
        D_800EA690.j[2].pos = D_800E1878->n[2].p;
        D_800EA690.j[3].pos = D_800E1878->n[5].p;

        if (B.f419 != 0) {
            s32 x, y, z, p, q, n;
            f32 k;

            if (B.f416 != 0) {
                f32 f = COSS((D_800E76D0 << 11) + 0x4000) * 0.00390625f + 128.0f;
                x = B.fA0 * f + B.fB8 * 128.0f;
                y = B.fA4 * f + B.fBC * 128.0f;
                y += 100;
                z = B.fA8 * f + B.fC0 * 128.0f;
            } else {
                k = 256.0f;
                if (B.f41A != 0) k = 64.0f;
                x = B.fA0 * k;
                x -= B.fA8 * k;
                z = -(B.fB8 * k);
                z += B.fC0 * k;
                y = func_8002E920(B.f0 + D_800EA690.j[3].pos.x + x, B.f4, B.f8 + D_800EA690.j[3].pos.z + z) - B.f4;
                y -= D_800EA690.j[3].pos.y;
                y += 100;
                if (B.f41A != 0) {
                    if (B.f412 > 0) {
                        p = (0x4000 - B.f412) / 64;
                        q = 0;
                    } else {
                        p = SINS(B.f412) / 64;
                        q = -COSS(B.f412) / 256;
                    }
                    {
                        f32 f;
                        p -= 200;
                        f = p;
                        x += f * B.fB8;
                        z += f * B.fC0;
                        y += q;
                    }
                }
            }
            n = B.f419;
            x = (x * n) >> 4;
            y = (y * n) >> 4;
            z = (z * n) >> 4;
            D_800EA690.j[3].pos.x += x;
            D_800EA690.j[3].pos.y += y;
            D_800EA690.j[3].pos.z += z;
        }
        if (B.f41A != 0) {
            s32 x, y, z, p, q, n;
            f32 k;

            if (B.f416 != 0) {
                f32 f = -COSS(D_800E76D0 << 11) * 0.00390625f - 128.0f;
                x = B.fA0 * f + B.fB8 * 128.0f;
                y = B.fA4 * f + B.fBC * 128.0f;
                y += 100;
                z = B.fA8 * f + B.fC0 * 128.0f;
            } else {
                k = 256.0f;
                if (B.f419 != 0) k = 64.0f;
                x = -(B.fA0 * k);
                z = B.fB8 * k;
                x -= B.fA8 * k;
                z += B.fC0 * k;
                y = func_8002E920(B.f0 + D_800EA690.j[3].pos.x + x, B.f4, B.f8 + D_800EA690.j[3].pos.z + z) - B.f4;
                y -= D_800EA690.j[2].pos.y;
                y += 100;
                if (B.f419 != 0) {
                    if (B.f412 < 0) {
                        p = -(B.f412 + 0x4000) / 64;
                        q = 0;
                    } else {
                        p = -SINS(B.f412) / 64;
                        q = COSS(B.f412) / 256;
                    }
                    {
                        f32 f;
                        p -= 200;
                        f = p;
                        x += f * B.fB8;
                        z += f * B.fC0;
                        y += q;
                    }
                }
            }
            n = B.f41A;
            x = (x * n) >> 4;
            y = (y * n) >> 4;
            z = (z * n) >> 4;
            D_800EA690.j[2].pos.x += x;
            D_800EA690.j[2].pos.y += y;
            D_800EA690.j[2].pos.z += z;
        }
        func_80011A58(&D_800EA690.j[0].rot, 0x1000, 0, B.f20 + B.f30);
        func_80011A58(&D_800EA690.j[1].rot, B.f5AC + 0x1000, 0, B.f20 + B.f30);
        func_80011A58(&D_800EA690.j[2].rot, B.f18 / 2, 0, B.f20);
        func_80011A58(&D_800EA690.j[3].rot, B.f18 / 2, 0, B.f20);
        h = B.f3BC;
        if (D_800EA7B8[i] == 0) {
            if (h < 0) {
                h = SINS(h * 2) / 2 - 0x2000;
            } else {
                h = -SINS(h * 2) / 2 + 0x2000;
            }
        }
        st = h;
        if (st < 0) st = -st;
        h = (h * 12000) >> 14;
        func_80011A58(&D_800EA690.rot, (0x4000 - SINS(st * 4)) >> 4, 0, B.f20 + h);
        func_8003D180(m);
        st = B.f7A8;
        if (st != 0) {
            s32 o;
            struct { u8 pad[4]; s32 f4; } *pp = D_800C9E84;
            if (st >= 8) {
                if (pp->f4 - 8 < st) {
                    o = (pp->f4 - st) << 5;
                } else {
                    o = 0x100;
                }
            } else {
                o = st << 5;
            }
            func_8003F274(m, 0x12, 3, st, o);
        }
        if (B.f3D0 >= 0) {
            func_8003F564(m, 0xF);
        }
    }
    if (B.f3D0 >= 0) {
        func_8003E330(m, i, scale);
    }
    if (D_800DC150[i].fA50 != 0) {
        func_80071678(i, m, D_800DC150[i].fA50, D_800DC150[i].fA4C + D_800E30A8 * 0xC0);
    }
}
