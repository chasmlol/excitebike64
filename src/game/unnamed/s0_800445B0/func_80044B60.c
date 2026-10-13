#include "ctx.h"

typedef struct {
    f32 x, y, z;      /* 0x00 */
    f32 vx, vy, vz;   /* 0x0C */
    s32 i18;          /* 0x18 */
    f32 f1C;          /* 0x1C */
    union { f32 f; s32 i; } f20;          /* 0x20 */
    u8 pad24[0x40 - 0x24];
    f32 r40, r44, r48; /* 0x40 */
} Body;

typedef struct Bike {
    u8 pad0[0x48];
    Body b0;                 /* 0x48 */
    u8 pad94[0x1C5 - 0x94];
    u8 f1C5;
    u8 pad1C6[0x1CC - 0x1C6];
    Body b1;                 /* 0x1CC */
    u8 pad218[0x349 - 0x218];
    u8 f349;
    u8 pad34A[0x36C - 0x34A];
    u8 f36C;
    u8 f36D;
    u8 pad36E[0x380 - 0x36E];
    f32 f380;
    f32 f384;
    u8 pad388[0x3AC - 0x388];
    s32 f3AC;
    u8 pad3B0[0x3CC - 0x3B0];
    s32 f3CC;
    s8 f3D0;
    u8 f3D1;
    u8 pad3D2[0x428 - 0x3D2];
    f32 f428;
    u8 pad42C[0x478 - 0x42C];
    f32 f478;
    u8 pad47C[0x51C - 0x47C];
    f32 f51C;
    u8 pad520[0x5A0 - 0x520];
    f32 f5A0;
    u8 pad5A4[0x624 - 0x5A4];
    u16 f624;
    u8 pad626[0x6E8 - 0x626];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Bike;

typedef struct { u8 pad[0x28]; s32 flags; u8 pad2[0x40 - 0x2C]; } Track;
typedef struct { u8 pad[0x3C]; s32 f3C; u8 pad2[0x4C - 0x40]; s32 f4C; } Game;

extern f32 D_800C8E14, D_800C8E18;
extern Track D_800C9080[];
extern u8 D_800CA440[];
extern u8 D_800CA4F0[][0x80];
extern Bike D_800E37C0[];
extern f32 D_800E76C8;
extern s32 D_800E76D0;
extern s32 D_800E84F8;
extern s32 D_800EE29C;
extern u8 D_800EE30C;
extern Game D_800EFD70;
extern s32 D_800EFDBC;

s32 func_8003F534(s32 a0, s32 *a1);
void func_80010994(s32 k, s32 n, s32 vol);
s32 func_8002E920(s32 x, s32 y, s32 z);
s32 func_800701B4(s32 pi, s32 a1);
void func_8002AF64(Bike *p0);
void func_8006FE84(Bike *o, s32 n);
void func_800453D8(Body *b, void *t, s32 n, s32 m);
void func_80046CC0(Bike *b);

static inline f32 damp2(f32 v, f32 k)
{
    return v - v * (k / (2200.0f / D_800E76C8));
}
#define DAMP(x) { f32 k, v, w; k = (D_800EFD70.f4C == 6) ? D_800C8E14 * 20000.0f : 15000.0f; v = (x); w = v; if (D_800EFD70.f4C == 6) (x) = v - v * (k / ((D_800C8E18 * 2000.0f) / D_800E76C8)); else (x) = w - w * (k / (2200.0f / D_800E76C8)); }

#define DAMPR(x) { f32 r, k, v, w; k = (D_800EFD70.f4C == 6) ? D_800C8E14 * 20000.0f : 15000.0f; v = (x); w = v; if (D_800EFD70.f4C == 6) r = v - v * (k / ((D_800C8E18 * 2000.0f) / D_800E76C8)); else r = w - w * (k / (2200.0f / D_800E76C8)); (x) = r; }

#define CLAMP(b, p) { m = __builtin_sqrtf(b->p.vx * b->p.vx + b->p.vy * b->p.vy + b->p.vz * b->p.vz);     if (b->f428 < m) { b->p.vx *= b->f428 / m; b->p.vy *= b->f428 / m; b->p.vz *= b->f428 / m; } }

static inline f32 airtime(Body *p, f32 g)
{
    f32 h, t, s, a, v;
    h = p->y - (f32)func_8002E920(p->x * 1000.0f, p->y * 1000.0f, p->z * 1000.0f) / 1000.0f;
    if (h < 0.0f) return 0.0f;
    v = p->vy;
    s = __builtin_sqrtf(v * v + h * 29.400002f) / g;
    a = -v / g;
    t = -a + s;
    if (t > 0.0f) return t;
    return s + a;
}

static inline void lane(Bike *b, s32 n) { s32 t = b->f3D0 + n; b->f3D1 = b->f3D0; b->f3CC = b->f3AC; func_8006FE84(b, t); }

void func_80044B60(Bike *b)
{
    s32 vol, id, n, ch;
    s32 unused[8];
    s32 tmp[2];
    f32 g, dt, m;

    func_800701B4((s32)b, 0);
    b->f624++;
    DAMPR(b->f51C);
    DAMPR(b->f478);
    b->f5A0 *= 0.9f;
    if (b->f6E8 == 0 && (D_800EFD70.f3C == 1 || D_800EFD70.f4C == 8)) {
        D_800EE30C = 1;
    }
    func_8002E920(b->b0.x * 1000.0f, b->b0.y * 1000.0f, b->b0.z * 1000.0f);
    if ((D_800C9080[(D_800E84F8 >> 4) & 0xF].flags & 0x800) && b->f6E8 == 0) {
        func_8002AF64(b);
    }
    if (b->f1C5 & 1) {
        b->b0.f20.i = 0;
        b->b0.i18 = 0;
        b->b0.f1C = -14.700001f;
        b->f380 = b->f384 = b->b0.f20.f;
        func_800453D8(&b->b0, D_800CA440, 0xB, 1);
        CLAMP(b, b0);
        if (b->f380 > 0.5f) {
            vol = b->f380 * 128.0f;
            vol += 0x30;
            if (vol >= 0x100) vol = 0xFF;
            if (vol > 0x80 || D_800E76D0 - D_800EE29C >= 0x1F) {
                D_800EE29C = D_800E76D0;
                if (D_800E76D0 & 1) {
                    func_80010994(4, b - D_800E37C0, vol);
                    func_80010994(4, b - D_800E37C0, vol);
                } else {
                    func_80010994(8, b - D_800E37C0, vol);
                    func_80010994(8, b - D_800E37C0, vol);
                }
            }
        }
        b->b0.vy -= D_800E76C8 * 14.700001f;
        b->b0.r40 -= b->b0.r40 * D_800E76C8 * 0.5f;
        b->b0.r44 -= b->b0.r44 * D_800E76C8 * 0.5f;
        b->b0.r48 -= b->b0.r48 * D_800E76C8 * 0.5f;
    } else {
        b->f624 += 20;
    }
    if (b->f349 & 1) {
        b->b1.i18 = 0;
        b->b1.f20.i = 0;
        b->b1.f1C = -14.700001f;
        func_800453D8(&b->b1, D_800CA4F0[b->f36D], 8, 1);
        CLAMP(b, b1);
        dt = D_800E76C8;
        g = 14.700001f;
        b->b1.vy -= dt * g;
        b->b1.r40 -= b->b1.r40 * D_800E76C8;
        b->b1.r44 -= b->b1.r44 * D_800E76C8;
        b->b1.r48 -= b->b1.r48 * D_800E76C8;
        switch (b->f36C) {
        case 1:
            if (func_8003F534(b->f3D0, tmp) - (b->f3AC >> 8) < 2) {
                n = (airtime(&b->b1, g) < 0.5f) ? 2 : 1;
                lane(b, n);
                b->f36C += n;
            }
            break;
        case 2:
            if (airtime(&b->b1, g) < 0.5f) {
                lane(b, 1);
                b->f36C = 3;
            }
            break;
        case 3:
            break;
        }
    }
    ch = 0x78;
    if (D_800EE30C != 0 && b->f6E8 == 0) ch = 0xF0;
    if (D_800EFD70.f4C == 7) ch = 0x12C;
    if (ch < (s16)b->f624) func_80046CC0(b);
}
