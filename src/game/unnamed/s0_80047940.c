#include "game.h"

void func_80047940(void) {
    s32 i;
    f32 sum;
    f32 dx, dz;

    sum = 0.0f;
    D_800EE370[0] = sum;
    for (i = 1; i < 10; i++) {
        dx = D_800CA6F0[i].x - D_800CA6F0[i - 1].x;
        dz = D_800CA6F0[i].z - D_800CA6F0[i - 1].z;
        sum += __builtin_sqrtf(dx * dx + dz * dz);
        D_800EE370[i] = sum;
    }
    func_8004281C(0);
    func_8005B218();
}

void func_80047A1C(void) {
    s32 i;
    D_800CA410 = 10;
    for (i = 0; i < 10; i++) {
        D_800EE068_2[i].x = D_800CA6F0[i].x * 1000.0f;
        D_800EE068_2[i].z = D_800CA6F0[i].y * 1000.0f;
        D_800EE068_2[i].y = D_800CA6F0[i].z * 1000.0f;
    }
}

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
} Bike_27;

typedef struct {
    s32 state;
    u8 pad4[0x1C];
    s16 f20;
    s16 n;              /* 0x22 */
    u8 pad24[0x1C];
    s32 f40;            /* 0x40 */
} Game_18;

extern Bike_27 D_800E37C0_50[];
extern Game_18 D_800F05A8_17;
extern Bike_27 *D_800F0C10_2[];

void func_80047A90(void)
{
    Bike_27 *b;
    Bike_27 *a;
    Bike_27 *c;
    f32 dx, dz, d, ang, e, f, gx, gz;
    s32 i;
    u8 swapped;
    s32 pad[4];

    d = 0.0f;
    for (b = D_800E37C0_50; b < &D_800E37C0_50[D_800F05A8_17.n]; b++) {
        b->f44C = 1.0f;
        if (b->f7A5 != 0) {
            continue;
        }
        if (b == D_800E37C0_50) {
            dx = D_800CA6F0[b->wp].x - b->x;
            ang = func_80013098(D_800CA6F0[b->wp].z - b->z, -dx);
            func_80042620((s32)(ang * 32768.0f / 3.1415927f) - D_800E8000_2 & 0xFFFF);
        }
        {
            gx = b->x - D_800E37C0_50[0].x;
            gz = b->z - D_800E37C0_50[0].z;
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
                    func_80050F7C(b - D_800E37C0_50);
                    if (b == D_800E37C0_50) {
                        D_800F05A8_17.f40 = 1;
                    }
                }
            } else {
                gx = b->x - D_800CA6F0[b->wp].x;
                gz = b->z - D_800CA6F0[b->wp].z;
                b->dist = gx * gx + gz * gz;
            }
            if (b == D_800E37C0_50) {
                func_8004281C(b->wp);
            }
        }
        b->f630 = D_800EE370[b->wp] - b->dist;
    }
    if (D_800E3EA8 == 0) {
        do {
            swapped = 0;
            for (i = 0; i < D_800F05A8_17.n - 1; i++) {
                a = D_800F0C10_2[i];
                if (a->f668 < 0) {
                    c = D_800F0C10_2[i + 1];
                    if (a->wp < c->wp || (a->wp == c->wp && a->dist > c->dist)) {
                        swapped = 1;
                        D_800F0C10_2[i] = c;
                        D_800F0C10_2[i + 1] = a;
                    }
                }
            }
        } while (swapped);
        for (i = 0; i < D_800F05A8_17.n; i++) {
            D_800F0C10_2[i]->f662 = i;
        }
        func_800635A4();
    }
}

void func_80047F7C(u8 *s)
{
    f32 v[3];
    f32 o[3];
    f32 f3, f4, f5, d, sum;
    f32 *tp;
    s8 *cp;
    u32 uVar;

    v[0] = D_800CA6F0_2[s[0x402] * 3] - *(f32 *)(s + 0x48);
    tp = &D_800CA6F0_2[s[0x402] * 3];
    v[1] = tp[1] - *(f32 *)(s + 0x4C);
    tp = &D_800CA6F0_2[s[0x402] * 3];
    v[2] = tp[2] - *(f32 *)(s + 0x50);
    func_80013310(v, (f32 *)(s + 0xA0), o);
    f3 = func_80013098(o[2], o[0]);
    sum = o[0] * o[0] + o[1] * o[1] + o[2] * o[2];
    f5 = __builtin_sqrtf(sum);

    cp = (s8 *)(s + 0x5D0);
    *cp = (s8)(s32)(-f3 * 14.0f);
    if (*cp > 14) *cp = 14;
    if (*cp < -14) *cp = -14;

    if (!(0.0f < f3)) f3 = -f3;

    f4 = 5.0f;
    if (!(f5 < 5.0f)) {
        if (f5 < 20.0f) {
            if (f3 < 0.1f) f4 = 20.0f;
        } else {
            f4 = 10.0f;
            if (f3 < 1.0f) f4 = 40.0f;
        }
    }
    if (s[0x3FF] == 1) goto zero;
    if (s[0x3FF] == 2) goto five;
    goto after;
zero:
    f4 = 0.0f;
    goto after;
five:
    f4 = 5.0f;
after:

    d = *(f32 *)(s + 0x420) - f4;
    if (0.5f < d) {
        uVar = 6;
        *(s16 *)(s + 0x55C) = uVar;
        *(s16 *)(s + 0x4B8) = uVar;
        uVar = 0x20;
        goto store;
    }
    if (!(d < -0.5f) || *(s32 *)(s + 0x744) >= 3 || s[0x7A6] != 0) goto done;
    uVar = 0x10;
    if (!(d < -5.0f)) goto store;
    uVar = 0x20000;
    if (*(f32 *)(s + 0x648) < 9500.0f) goto store;
    uVar = 0x10;
store:
    *(u32 *)(s + 0x5B0) = uVar;
done:
    func_8005C5BC(s, 1);
}
