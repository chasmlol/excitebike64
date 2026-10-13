#include "ctx.h"

typedef struct {
    u8 pad0[0x34];
    s16 ang;            /* 0x34 */
    u8 pad36[0x48 - 0x36];
    f32 x;              /* 0x48 */
    f32 y;              /* 0x4C */
    f32 z;              /* 0x50 */
    u8 pad54[0x3FF - 0x54];
    u8 f3FF;            /* 0x3FF */
    u8 pad400[2];
    u8 wp;              /* 0x402 */
    u8 pad403;
    f32 dist;           /* 0x404 */
    u8 pad408[0x44C - 0x408];
    f32 f44C;           /* 0x44C */
    u8 pad450[0x620 - 0x450];
    u8 f620;            /* 0x620 */
    u8 f621;            /* 0x621 */
    u8 pad622[0x630 - 0x622];
    f32 f630;           /* 0x630 */
    u8 pad634[0x662 - 0x634];
    s16 f662;           /* 0x662 */
    u8 pad664[4];
    s16 f668;           /* 0x668 */
    u8 pad66A[0x7A4 - 0x66A];
    u8 f7A4;            /* 0x7A4 */
    u8 f7A5;            /* 0x7A5 */
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike;

typedef struct {
    s32 state;
    u8 pad4[0x1C];
    s16 f20;
    s16 n;              /* 0x22 */
    u8 pad24[0x1C];
    s32 f40;            /* 0x40 */
} Game;

extern Bike D_800E37C0[];
extern Game D_800F05A8;
extern Bike *D_800F0C10[];
extern u8 D_800E3EA8;
extern f32 D_800EE370[];
extern Vec3f D_800CA6F0[];
extern s32 D_800E8000;

void func_80042620(s32 a0);
void func_8004281C(s32 x);
f32 func_80013098(f32 p1, f32 p2);
void func_80046CC0(Bike *b);
u32 func_80050F7C(s32 p0);
void func_800635A4(void);

void func_80047A90(void)
{
    Bike *b;
    Bike *a;
    Bike *c;
    f32 dx, dz, d, ang, e, f, gx, gz;
    s32 i;
    u8 swapped;
    s32 pad[4];

    d = 0.0f;
    for (b = D_800E37C0; b < &D_800E37C0[D_800F05A8.n]; b++) {
        b->f44C = 1.0f;
        if (b->f7A5 != 0) {
            continue;
        }
        if (b == D_800E37C0) {
            dx = D_800CA6F0[b->wp].x - b->x;
            ang = func_80013098(D_800CA6F0[b->wp].z - b->z, -dx);
            func_80042620((s32)(ang * 32768.0f / 3.1415927f) - D_800E8000 & 0xFFFF);
        }
        {
            gx = b->x - D_800E37C0[0].x;
            gz = b->z - D_800E37C0[0].z;
            e = __builtin_sqrtf(gx * gx + gz * gz);
        }
        if (b->f7A4 != 0) {
            if (e < 100.0f) {
                b->f7A4 = 0;
                func_80046CC0(b);
                b->f620 = 1;
                b->f621 = 1;
            } else {
                dx = D_800CA6F0[b->wp].x - b->x;
                dz = D_800CA6F0[b->wp].z - b->z;
                d = __builtin_sqrtf(dx * dx + dz * dz);
                ang = func_80013098(dz, dx);
                b->ang = ang * 32768.0f / 3.1415927f;
                b->x += dx / d * 0.16666667f;
                b->z += dz / d * 0.16666667f;
            }
        } else if (120.0f < e) {
            b->f7A4 = 1;
        }
        dx = b->x - D_800CA6F0[b->wp].x;
        dz = b->z - D_800CA6F0[b->wp].z;
        f = __builtin_sqrtf(dx * dx + dz * dz);
        b->dist = f;
        if (f < 1.0f) {
            if (++b->wp == 10) {
                b->wp = 0;
                if (b->f3FF == 0) {
                    func_80050F7C(b - D_800E37C0);
                    if (b == D_800E37C0) {
                        D_800F05A8.f40 = 1;
                    }
                }
            } else {
                gx = b->x - D_800CA6F0[b->wp].x;
                gz = b->z - D_800CA6F0[b->wp].z;
                b->dist = gx * gx + gz * gz;
            }
            if (b == D_800E37C0) {
                func_8004281C(b->wp);
            }
        }
        b->f630 = D_800EE370[b->wp] - b->dist;
    }
    if (D_800E3EA8 == 0) {
        do {
            swapped = 0;
            for (i = 0; i < D_800F05A8.n - 1; i++) {
                a = D_800F0C10[i];
                if (a->f668 < 0) {
                    c = D_800F0C10[i + 1];
                    if (a->wp < c->wp || (a->wp == c->wp && a->dist > c->dist)) {
                        swapped = 1;
                        D_800F0C10[i] = c;
                        D_800F0C10[i + 1] = a;
                    }
                }
            }
        } while (swapped);
        for (i = 0; i < D_800F05A8.n; i++) {
            D_800F0C10[i]->f662 = i;
        }
        func_800635A4();
    }
}
