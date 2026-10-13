#include "game.h"

void func_8005EA20(u8 *param_1, s32 param_2)
{
    func_8000ED10(D_800F0850, 0, 0xc0);
    func_8000ED10(D_800F093C, 0, 6);
    func_8000ED10(D_800F0944, 0, 6);
    func_8000ED10(D_800F091C, 0, 6);
    D_800F0A30 = 0;
    D_800F0A3C = (s32)param_1;
    D_800F0954 = (s32)param_1;
    D_800F0950 = param_2;
    D_800F0910 = param_2;
    D_800F0A28 = 8;
    D_800F0A34 = 0;
    D_800F094C = 0;
    func_8000ED10(param_1, 0, param_2);
    func_8005EB68(0x7b, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005F924();
    D_800F0A1C = D_800EFDAC;
    D_800F0924 = D_800F0954;
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
}

void func_8005EB68(s32 v, s32 n)
{
    s32 b;

    if (D_800F0A34 != 0) {
        return;
    }
    while (1) {
        v &= ((1 << n) - 1) << (8 - n);
        b = v & 0xFF;
        { s32 sh = 8 - D_800F0A28; *D_800F0954_2 |= b >> sh; }
        if (n < D_800F0A28) {
            D_800F0A28 -= n;
            return;
        }
        n -= D_800F0A28;
        v = b << D_800F0A28;
        D_800F0A28 = 8;
        D_800F0954_2++;
        D_800F0910--;
        if (D_800F0910 == 0) {
            D_800F0A34 = 1;
        }
    }
}

typedef struct {
    s8 x;
    s8 y;
    u8 bits;
    u8 pad;
} Ent_60;

extern Ent_60 D_800F0850_2[][8];

void func_8005EC14(u32 *p)
{
    s32 i, j, k;
    s32 x, y, bits, c, v, a, n;
    u32 w;

    if (D_800F0A34 != 0) {
        return;
    }
    D_800F094C++;
    for (i = 0; i < D_800F0A1C; i++) {
        w = p[i];
        x = (w >> 24) & 0xF;
        if (w & 4) x = -x;
        y = w >> 28;
        if (w & 2) y = -y;
        bits = (w >> 4) & 1;
        if (w & 0x20) bits |= 2;
        if (w & 0x20000) bits |= 4;
        if (w & 0x100) bits |= 8;
        c = bits;
        if (w & 0x10000) c |= 0x10;
        D_800F0850_2[i][D_800F0A30].x = x - D_800F093C[i];
        D_800F0850_2[i][D_800F0A30].y = y - D_800F0944[i];
        D_800F0850_2[i][D_800F0A30].bits = D_800F091C[i] ^ c;
        D_800F093C[i] = x;
        D_800F0944[i] = y;
        D_800F091C[i] = c;
    }
    D_800F0A30++;
    if (D_800F0A30 != 8) {
        return;
    }
    D_800F0A30 = 0;
    for (i = 0; i < D_800F0A1C; i++) {
        for (j = 0; j < 8; j++) {
            if (D_800F0850_2[i][j].bits != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 5; k++) {
                    if ((D_800F0850_2[i][j].bits >> k) & 1) {
                        func_8005EB68(((k << 4) | ~0x7F) & 0xF0, 4);
                    }
                }
                func_8005EB68(0, 1);
            }
        }
        for (j = 0; j < 8; j++) {
            if (D_800F0850_2[i][j].x != 0 || D_800F0850_2[i][j].y != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
            continue;
        }
        func_8005EB68(0x80, 1);
        for (j = 0; j < 8; j++) {
            if (D_800F0850_2[i][j].x != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (D_800F0850_2[i][j].x == 0) {
                    a = 0;
                    n = 1;
                } else {
                    func_8005EB68(0x80, 1);
                    v = D_800F0850_2[i][j].x;
                    if (v > 0) {
                        func_8005EB68(0x80, 1);
                        v &= 0x1F;
                    } else {
                        func_8005EB68(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                func_8005EB68(a, n);
            }
        }
        for (j = 0; j < 8; j++) {
            if (D_800F0850_2[i][j].y != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (D_800F0850_2[i][j].y == 0) {
                    a = 0;
                    n = 1;
                } else {
                    func_8005EB68(0x80, 1);
                    v = D_800F0850_2[i][j].y;
                    if (v > 0) {
                        func_8005EB68(0x80, 1);
                        v &= 0x1F;
                    } else {
                        func_8005EB68(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                func_8005EB68(a, n);
            }
        }
    }
    D_800F0A20 = ((D_800F0954 - D_800F0A3C) * 100) / D_800F0950;
}

s32 func_8005F188(void)
{
    u32 buf[6];
    s32 i;
    s32 n;
    s32 acc;
    u8 *p;

    D_800F0924_2[0] = D_800F094C;
    D_800F0924_2[1] = D_800F094C >> 8;
    D_800F0924_2[2] = D_800F094C >> 16;
    func_8000ED10((u8 *)buf, 0, 0x18);
    while (D_800F0A30 != 0) {
        func_8005EC14(buf);
    }
    if (D_800F0A28 != 8) {
        func_8005EB68(0, D_800F0A28);
    }
    acc = 0;
    n = D_800F0954_2 - D_800F0A3C_2 - 4;
    p = D_800F0A3C_2 + 4;
    for (i = 0; i < n; i++) {
        acc += p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    D_800F0A3C_2[1] = acc;
    D_800F0A3C_2[2] = acc >> 8;
    D_800F0A3C_2[3] = acc >> 16;
    if (D_800F0A34 != 0) {
        return -1;
    }
    return D_800F0954_2 - D_800F0A3C_2;
}

#define RB() rb()

typedef struct {
    s8 a;
    s8 b;
    u8 c;
    u8 d;
} Cell_5;

extern Cell_5 D_800F0A40[][8];

static inline s32 rb(void)
{
    u8 *p = D_800F0A38;
    s32 b = *p;
    s32 c = D_800F0A24 - 1;
    s32 bit;

    D_800F0A24 = c;
    bit = (b >> c) & 1;
    if (c == 0) {
        D_800F0A38 = p + 1;
        D_800F0A24 = 8;
    }
    return bit;
}

void func_8005F2D8(void)
{
    s32 i, j, k, n, s;
    u32 v, f;
    s32 x, y, z;

    for (i = 0; i < D_800F0A18; i++) {
        func_8000ED10((u8 *)D_800F0A40[i], 0, 0x20);
        if (RB()) {
            for (j = 0; j < 8; j++) {
                while (RB()) {
                    k = RB() << 2;
                    k |= RB() << 1;
                    k |= RB();
                    D_800F0A40[i][j].c |= 1 << k;
                }
            }
        }
        if (RB()) {
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    D_800F0A40[i][j].a = n;
                }
            }
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    D_800F0A40[i][j].b = n;
                }
            }
        }
    }

    for (i = 0; i < D_800F0A18; i++) {
        for (j = 0; j < 8; j++) {
            x = D_800F0928_2[i] + D_800F0A40[i][j].a;
            y = D_800F0930_2[i] + D_800F0A40[i][j].b;
            z = D_800F0914_2[i] ^ D_800F0A40[i][j].c;
            D_800F0928_2[i] = x;
            D_800F0930_2[i] = y;
            D_800F0914_2[i] = z;
            k = (z & 1) << 4;
            if (z & 2) k |= 0x20;
            if (z & 4) k |= 0x20000;
            if (z & 8) k |= 0x100;
            if (z & 0x10) k |= 0x10000;
            if (x < 0) {
                x = -x;
                k |= 4;
                k |= (x & 0xF) << 24;
            } else if (x != 0) {
                k |= 8;
                k |= (x & 0xF) << 24;
            }
            if (y < 0) {
                y = -y;
                k |= 2;
                k |= (y & 0xF) << 28;
            } else if (y != 0) {
                k |= 1;
                k |= (y & 0xF) << 28;
            }
            D_800F0958[i][j] = k;
        }
    }
    D_800F0A2C = 8;
}

void func_8005F924(void)
{
    s32 i;
    u8 *q;
    u8 *p;
    u32 lo, hi;

    func_8005EB68(D_800EFD70_2[0x3F], 8);
    func_8005EB68(D_800EFD70_2[0x47], 8);
    func_8005EB68(D_800EFD70_2[0x13], 8);
    func_8005EB68(D_800EFD70_2[0x43], 8);
    for (i = 0; i < 6; i++) {
        q = D_800EFD70_2 + i;
        lo = q[0x43C] & 0xF;
        hi = (q[0x7AA] & 0xF) << 4;
        func_8005EB68(hi | lo, 8);
    }
    for (i = 0; i < 6; i++) {
        p = D_800EFD70_2;
        if (D_800E3700 == i) {
            func_8005EB68(((s32)p[i * 4 + 0x1F] | -0x80) & 0xFF, 8);
        } else {
            func_8005EB68(p[i * 4 + 0x1F], 8);
        }
    }
}

typedef struct {
    s32 pad0[7];
    s32 w[6];
    u8 pad34[0x43C - 0x34];
    u8 lo[6];
    u8 pad442[0x7AA - 0x442];
    u8 hi[6];
} Tab_6;

extern Tab_6 D_800EFD70_45;

static inline s32 getbits(s32 n)
{
    s32 v;
    s32 j;
    s32 b;
    s32 c;

    v = 0;
    for (j = 0; j < n; j++) {
        v <<= 1;
        b = *D_800F0A38;
        c = D_800F0A24 - 1;
        D_800F0A24 = c;
        b = b >> c;
        b = b & 1;
        if (c == 0) {
            D_800F0A38++;
            D_800F0A24 = 8;
        }
        v |= b;
    }
    return v;
}

void func_8005FA18(void)
{
    s32 i;
    s32 n;

    D_800EFDAC = getbits(8);
    D_800EFDB4 = getbits(8);
    getbits(8);
    getbits(8);
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        D_800EFD70_45.lo[i] = n & 0xF;
        D_800EFD70_45.hi[i] = (n >> 4) & 0xF;
    }
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        D_800EFD70_45.w[i] = n & 0x7F;
        if (n & 0x80) {
            D_800E3700 = i;
        }
    }
}

#define STEP(i)                       \
    do {                              \
        c = D_800F0A24;               \
        v = D_800F0A38;               \
        c = c - 1;                    \
        D_800F0A24 = c;               \
        if (c == 0) {                 \
            D_800F0A38 = v + 1;       \
            D_800F0A24 = 8;           \
        }                             \
        i++;                          \
    } while (i < 8)

void func_8005FC68(void)
{
    s32 i;
    s32 j;
    s32 c;
    s32 v;
    s32 w;

    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    j = 0;
    do {
        i = 0;
        do {
            v = D_800F0A38;
            c = D_800F0A24;
            c = c - 1;
            D_800F0A24 = c;
            if (c == 0) {
                w = v + 1;
                D_800F0A38 = w;
                D_800F0A24 = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
    j = 0;
    do {
        i = 0;
        do {
            v = D_800F0A38;
            c = D_800F0A24;
            c = c - 1;
            D_800F0A24 = c;
            if (c == 0) {
                w = v + 1;
                D_800F0A38 = w;
                D_800F0A24 = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
}

void func_8005FE10(s32 param_1)
{
    s32 u;

    D_800F0A2C = 0;
    D_800F0A38 = param_1;
    D_800F0A24 = 8;
    D_800F0A38 = param_1 + 4;
    func_8005FA18();
    D_800F0A18 = D_800EFDAC;
    D_800F0938 = func_80060204(8);
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x100;
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x10000;
    func_8000ED10(&D_800F0928[0], 0, 6);
    func_8000ED10(&D_800F0930[0], 0, 6);
    func_8000ED10(&D_800F0914[0], 0, 6);
}

void func_8005FEE0(s32 param_1)
{
    s32 u;

    D_800F0A2C = 0;
    D_800F0A38 = param_1;
    D_800F0A24 = 8;
    D_800F0A38 = param_1 + 4;
    func_8005FC68();
    D_800F0A18 = D_800EFDAC;
    D_800F0938 = func_80060204(8);
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x100;
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x10000;
    func_8000ED10(&D_800F0928[0], 0, 6);
    func_8000ED10(&D_800F0930[0], 0, 6);
    func_8000ED10(&D_800F0914[0], 0, 6);
}

s32 func_8005FFB0(s32 *dst)
{
    s32 i;

    if (D_800F0938 == 0) {
        return 1;
    }
    if (D_800F0A2C == 0) {
        func_8005F2D8();
    }
    for (i = 0; i < D_800F0A18; i++) {
        dst[i] = D_800F0958[i][8 - D_800F0A2C];
    }
    D_800F0A2C--;
    D_800F0938--;
    return 0;
}

s32 func_80060078(u8 *s, u32 *a1, u32 *a2, u32 *a3)
{
    u32 v;

    if (s[0] != 0x7B) {
        return 1;
    }
    *a1 = s[6];
    *a2 = s[7];
    v = s[0x14];
    *a3 = v;
    v = v + (s[0x15] << 8);
    *a3 = v;
    *a3 = v + (s[0x16] << 16);
    return 0;
}

s32 func_800600D4(u8 *p, s32 n)
{
    s32 sum;
    s32 i;
    s32 lim;
    u32 hdr;
    u8 *q;

    if (p[0] != 0x7B) {
        return 1;
    }
    if (n != 0x2800) {
        return 1;
    }
    q = p + 4;
    lim = 0x27FC;
    sum = 0;
    i = sum;
    do {
        u8 b = q[i];
        sum += b << (i % 8);
        i++;
    } while (i < lim);
    sum &= 0xFFFFFF;
    hdr = p[1] | (p[2] << 8) | (p[3] << 16);
    return (hdr ^ sum) != 0;
}

u32 func_80060170(u8 *p, s32 n)
{
    s32 i;
    u32 acc = 0;

    for (i = 0; i < n; i++) {
        acc += (u32)p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    return acc;
}

s32 func_800601C4(void)
{
    u8 *p = D_800F0A38;
    s32 byte = *p;
    s32 n = D_800F0A24 - 1;
    s32 bit;

    D_800F0A24 = n;
    bit = byte >> n;
    bit = bit & 1;
    if (n == 0) {
        D_800F0A38 = p + 1;
        D_800F0A24 = 8;
    }
    return bit;
}

s32 func_80060204(s32 n)
{
    s32 r = 0;
    s32 i;

    for (i = 0; i < n; i++) {
        u8 *p = D_800F0A38;
        s32 byte = *p;
        s32 c = D_800F0A24 - 1;
        s32 bit;

        r = r << 1;
        D_800F0A24 = c;
        bit = (byte >> c) & 1;
        if (c == 0) {
            D_800F0A38 = p + 1;
            D_800F0A24 = 8;
        }
        r = r | bit;
    }
    return r;
}
