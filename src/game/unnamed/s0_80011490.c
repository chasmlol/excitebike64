#include "game.h"

struct MtxS_2 {
    s16 i[4][4];
    u16 f[4][4];
};

#define SETM(r, c, x) { v = (x); m->i[r][c] = v >> 16; m->f[r][c] = v; }
#define SETS(r, c, x) { v = (x); v = v * scale / 256; m->i[r][c] = v >> 16; m->f[r][c] = v; }

void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale)
{
    s32 cx, cy, sx, sy, sz, cz;
    s32 cycx, cysx;
    s32 v;

    rx >>= 4;
    ry >>= 4;
    rz >>= 4;
    m->i[0][3] = m->i[1][3] = m->i[2][3] = 0;
    m->i[3][3] = 1;
    m->f[0][3] = m->f[1][3] = m->f[2][3] = 0;
    m->f[3][3] = 0;
    m->i[3][0] = tx >> 4;
    m->i[3][1] = ty >> 4;
    m->i[3][2] = tz >> 4;
    m->f[3][0] = tx << 12;
    m->f[3][1] = ty << 12;
    m->f[3][2] = tz << 12;
    cx = D_800C2C60[(rx - 0x400) & 0xFFF];
    cy = D_800C2C60[(ry - 0x400) & 0xFFF];
    sx = D_800C2C60[rx & 0xFFF];
    sy = D_800C2C60[ry & 0xFFF];
    sz = D_800C2C60[rz & 0xFFF];
    cz = D_800C2C60[(rz - 0x400) & 0xFFF];
    cycx = cy * cx / 0x4000;
    cysx = cy * sx / 0x4000;
    if (scale == 0x100) {
        SETM(0, 0, (sy * sz) >> 12);
        SETM(0, 1, cy << 2);
        SETM(0, 2, (sy * cz) >> 12);
        SETM(1, 0, (-sz * cysx + -cx * cz) >> 12);
        SETM(1, 1, (sx * sy) >> 12);
        SETM(1, 2, (-cz * cysx + cx * sz) >> 12);
        SETM(2, 0, (sz * cycx + -sx * cz) >> 12);
        SETM(2, 1, (-cx * sy) >> 12);
        SETM(2, 2, (cz * cycx + sx * sz) >> 12);
    } else {
        SETS(0, 0, (sy * sz) >> 12);
        SETS(0, 1, cy << 2);
        SETS(0, 2, (sy * cz) >> 12);
        SETS(1, 0, (-sz * cysx + -cx * cz) >> 12);
        SETS(1, 1, (sx * sy) >> 12);
        SETS(1, 2, (-cz * cysx + cx * sz) >> 12);
        SETS(2, 0, (sz * cycx + -sx * cz) >> 12);
        SETS(2, 1, (-cx * sy) >> 12);
        SETS(2, 2, (cz * cycx + sx * sz) >> 12);
    }
}

void func_800118DC(MtxS_2 *m, f32 c, f32 sn, s32 s) {
    s32 ci = (s32)(c * 16384.0f);
    s32 si = (s32)(sn * 16384.0f);
    s32 t;

    func_8000ED10((u8 *)m, 0, 0x40);
    t = (ci << 14) >> 12;
    t = (t * s) / 256;
    m->f[0][0] = t;
    m->i[0][0] = t >> 16;
    m->i[0][1] = 0;
    m->f[0][1] = 0;
    t = (si << 14) >> 12;
    t = (t * s) / 256;
    m->f[0][2] = t;
    m->i[0][2] = t >> 16;
    t = -ci >> 12;
    t = (t * s) / 256;
    m->f[1][0] = t;
    m->i[1][0] = t >> 16;
    t = s << 8;
    m->f[1][1] = t;
    m->i[1][1] = t >> 16;
    t = -si >> 12;
    t = (t * s) / 256;
    m->f[1][2] = t;
    m->i[1][2] = t >> 16;
    t = (-(si << 14)) >> 12;
    t = (t * s) / 256;
    m->i[2][0] = t >> 16;
    m->f[2][0] = t;
    m->i[2][1] = 0;
    m->f[2][1] = 0;
    t = (ci * (s << 2)) / 256;
    m->i[2][2] = t >> 16;
    m->f[2][2] = t;
}

void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4)
{
    s32 iVar1;
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    s32 iVar7;
    s32 iVar8;
    s32 q4;
    s32 q1;

    iVar7 = D_800C2C60[((param_2 >> 4) - 0x400) & 0xfff];
    iVar8 = D_800C2C60[((param_3 >> 4) - 0x400) & 0xfff];
    iVar4 = iVar8 * iVar7;
    iVar6 = D_800C2C60[(param_2 >> 4) & 0xfff];
    iVar3 = D_800C2C60[(param_3 >> 4) & 0xfff];
    iVar5 = D_800C2C60[(param_4 >> 4) & 0xfff];
    iVar2 = D_800C2C60[((param_4 >> 4) - 0x400) & 0xfff];
    if (iVar4 < 0) {
        iVar4 = iVar4 + 0x3fff;
    }
    q4 = iVar4 >> 14;
    iVar1 = iVar8 * iVar6;
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    q1 = iVar1 >> 14;
    param_1[1] = iVar8;
    param_1[0] = (iVar3 * iVar5) >> 14;
    param_1[2] = (iVar3 * iVar2) >> 14;
    param_1[3] = (-iVar5 * q1 + -iVar7 * iVar2) >> 14;
    param_1[4] = (iVar6 * iVar3) >> 14;
    param_1[5] = (-iVar2 * q1 + iVar7 * iVar5) >> 14;
    param_1[6] = (iVar5 * q4 + -iVar6 * iVar2) >> 14;
    param_1[7] = (-iVar7 * iVar3) >> 14;
    param_1[8] = (iVar2 * q4 + iVar6 * iVar5) >> 14;
}

void func_80011C1C(s32 *a, s32 *b, s32 *c)
{
    s32 t;

    t = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[0] = t >> 14;
    t = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[1] = t >> 14;
    t = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[2] = t >> 14;
    t = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[3] = t >> 14;
    t = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[4] = t >> 14;
    t = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[5] = t >> 14;
    t = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[6] = t >> 14;
    t = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[7] = t >> 14;
    t = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    c[8] = t >> 14;
}

void func_80011EF0(f32 *r, f32 *a, f32 *b)
{
    r[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    r[1] = a[0] * b[3] + a[1] * b[4] + a[2] * b[5];
    r[2] = a[0] * b[6] + a[1] * b[7] + a[2] * b[8];
    r[3] = a[3] * b[0] + a[4] * b[1] + a[5] * b[2];
    r[4] = a[3] * b[3] + a[4] * b[4] + a[5] * b[5];
    r[5] = a[3] * b[6] + a[4] * b[7] + a[5] * b[8];
    r[6] = a[6] * b[0] + a[7] * b[1] + a[8] * b[2];
    r[7] = a[6] * b[3] + a[7] * b[4] + a[8] * b[5];
    r[8] = a[6] * b[6] + a[7] * b[7] + a[8] * b[8];
}

#define EL(m, i) (((s32)(m)[i] << 16) | (u16)(m)[(i) + 16])

void func_800120A4(s16 *a, s16 *b, s16 *o)
{
    s32 r, c, k, t, p, n, u;
    f32 sum;

    for (r = 0; r < 4; r++) {
        c = 0;
        p = r * 4;
        n = p;
    next:
        sum = 0.0f;
        do {
            for (k = 0; k < 4; k++) {
                t = EL(a, n + k);
                u = EL(b, k * 4 + c);
                sum += (f32)t * (f32)u * (1.0f / 65536.0f);
            }
        } while (0);
        if (sum < -1073741800.0f || 1073741800.0f < sum) {
            sum = 0.0f;
            do {
                for (k = 0; k < 4; k++) {
                    t = EL(a, n + k);
                    u = EL(b, k * 4 + c);
                    t = (f32)t * 0.25f;
                    u = (f32)u * 0.25f;
                    sum += (f32)t * (f32)u * (1.0f / 65536.0f);
                }
            } while (0);
            t = (s32)sum << 4;
            o[p] = t >> 16;
            o[p + 16] = t;
        } else {
            t = (s32)sum;
            o[p] = t >> 16;
            o[p + 16] = t;
        }
        p++;
        c++;
        if (c < 4) {
            goto next;
        }
    }
}

void func_80012250(f32 *p, s32 *o1, s32 *o2, s32 *o3)
{
    s32 q[9];
    s32 *m;
    f32 sum, len, n0, n1, n2, n3;
    f32 n1sq, n1n3, n1n2, n2sq, n3sq, n2n3, n0n1, n0n3, n0n2;
    f32 fa, fb, fc, fd, fe, fg, fh, fi, fj;
    s32 i2, u6, sv2, sv3, sv4;

    sum = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
    len = __builtin_sqrtf(sum);
    p[0] /= len;
    p[1] /= len;
    p[2] /= len;
    p[3] /= len;
    n0 = p[0];
    n1 = p[1];
    n2 = p[2];
    n3 = p[3];
    n1sq = n1 * n1;
    n1n2 = n1 * n2;
    n1n3 = n1 * n3;
    n2sq = n2 * n2;
    n2n3 = n2 * n3;
    n3sq = n3 * n3;
    n0n1 = n0 * n1;
    n0n2 = n0 * n2;
    n0n3 = n0 * n3;
    fa = (n0n3 + n1n2) * 32768.0f;
    fb = (n1n3 - n0n2) * 32768.0f;
    fc = (n1n2 - n0n3) * 32768.0f;
    fd = ((0.5f - n1sq) - n3sq) * 32768.0f;
    fe = (n2n3 + n0n1) * 32768.0f;
    fg = (n1n3 + n0n2) * 32768.0f;
    fh = (n2n3 - n0n1) * 32768.0f;
    fi = ((0.5f - n1sq) - n2sq) * 32768.0f;
    fj = ((0.5f - n2sq) - n3sq) * 32768.0f;
    m = q;
    m[1] = (s32)fa;
    m[2] = (s32)fb;
    m[3] = (s32)fc;
    m[5] = (s32)fe;
    m[6] = (s32)fg;
    m[7] = (s32)fh;
    m[4] = (s32)fd;
    m[8] = (s32)fi;
    m[0] = (s32)fj;
    i2 = func_80013010(q[1]);
    u6 = D_800C2C60[((u32)(i2 << 16)) >> 20];
    sv2 = (s16)i2;
    if (u6 != 0) {
        sv3 = func_80013010((q[2] << 14) / u6);
        if ((q[0] ^ u6 ^ ((s16)sv3 - 0x4000)) >= 0) {
            sv3 = -0x8000 - sv3;
        }
        sv4 = func_80013010((-(q[7] << 14)) / u6);
        if ((q[4] ^ u6 ^ ((s16)sv4 - 0x4000)) >= 0) {
            sv4 = -0x8000 - sv4;
        }
        *o1 = (s16)sv4;
        *o2 = sv2;
        *o3 = (s16)sv3;
    }
}

void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio)
{
    s32 a;
    s32 t;
    s32 nx, ny;
    s32 r;

    if (x == 0 && y == 0) {
        *pRatio = 0;
        *pAng = 0;
        return;
    }
    if (x > 0) {
        if (y > 0) {
            if (x < y) {
                t = D_800C4C60[(x << 12) / y];
                *pRatio = (y << 14) / D_800C2C60[(t >> 4) & 0xFFF];
                a = t;
                *pAng = a;
                return;
            } else {
                a = 0x4000 - D_800C4C60[(y << 12) / x];
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            }
        } else {
            if (-y < x) {
                a = D_800C4C60[(-y << 12) / x] + 0x4000;
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = 0x8000 - D_800C4C60[(x << 12) / -y];
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            }
        }
    } else {
        nx = -x;
        if (y < 0) {
            ny = -y;
            if (nx < ny) {
                a = D_800C4C60[(nx << 12) / ny] + 0x8000;
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = 0xC000 - D_800C4C60[(ny << 12) / nx];
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            }
        } else {
            if (y < nx) {
                a = D_800C4C60[(y << 12) / nx] + 0xC000;
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = -D_800C4C60[(nx << 12) / y];
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            }
        }
    }
}

#define SINS(x) D_800C2C60[(x) & 0xFFF]
#define COSS(x) D_800C2C60[((x) - 0x400) & 0xFFF]

void func_80012958(f32 *m, s16 a, s16 b, s16 c)
{
    f32 sa, sb, cb, sc, cc;
    f32 ca;
    s32 ia, ib, ic;
    s32 i1, i2;
    s16 *tb;
    f32 cbca, cbsa;

    tb = D_800C2C60;
    ia = a >> 4;
    i1 = tb[ia & 0xFFF];
    i2 = tb[(ia - 0x400) & 0xFFF];
    ib = b >> 4;
    sa = i1 * 6.1035156e-05f;
    ca = i2 * 6.1035156e-05f;
    sb = tb[ib & 0xFFF] * 6.1035156e-05f;
    cb = tb[(ib - 0x400) & 0xFFF] * 6.1035156e-05f;
    ic = c >> 4;
    sc = tb[ic & 0xFFF] * 6.1035156e-05f;
    cc = tb[(ic - 0x400) & 0xFFF] * 6.1035156e-05f;
    cbca = cb * ca;
    cbsa = cb * sa;
    m[0] = sb * sc;
    m[1] = cb;
    m[2] = sb * cc;
    m[3] = -sc * cbsa + -ca * cc;
    m[4] = sa * sb;
    m[5] = -cc * cbsa + ca * sc;
    m[6] = sc * cbca + -sa * cc;
    m[7] = -ca * sb;
    m[8] = cc * cbca + sa * sc;
}

static inline u8 r250(void) {
    u32 i = D_800E36CC;
    u32 j;
    u8 r;

    if (i >= 147) {
        j = i - 147;
    } else {
        j = i + 103;
    }
    r = D_800E35D0[i] ^= D_800E35D0[j];
    if (++D_800E36CC >= 250) {
        D_800E36CC = 0;
    }
    return r;
}

void func_80012B14(void) {
    u32 seed;
    s32 i;
    u8 *p;
    u8 msb;
    u8 mask;

    D_800E36CC = 0;
    p = D_800E35D0;
    seed = 12345;
    for (i = 250; i != 0; i--) {
        seed = (seed * 1277) & 0xFFFF;
        *p++ = seed >> 8;
    }
    msb = 0x80;
    mask = 0xFF;
    p = &D_800E35D0[14];
    while (msb != 0) {
        *p &= mask;
        *p |= msb;
        p += 11;
        msb >>= 1;
        mask >>= 1;
    }
    for (i = 0; (u32)i < 500; i++) {
        r250();
    }
}

static __inline__ void mulrot(s32 *in, s32 *t, s32 *out, s32 rx, s32 ry, s32 rz)
{
    func_80011A58(t, rx, ry, rz);
    func_80011C1C(in, t, out);
}

void func_80012C00(void)
{
    s32 a[10];
    s32 b[10];
    s32 c[10];
    s16 ang[4];
    s32 w;
    s16 y;
    s16 x;
    s32 *pa;

    func_80011A58(a, 0, 0, 0x2000);
    pa = a;
    func_8001254C(pa[2], a[0], &ang[3], &w);
    mulrot(pa, b, c, 0, 0, -ang[3]);
    func_8001254C(c[1], c[0], &y, &w);
    mulrot(c, b, pa, 0, -y, 0);
    func_8001254C(pa[5], pa[4], &x, &w);
    mulrot(pa, b, c, -x, 0, 0);
    ang[0] = x;
    ang[1] = y;
    ang[2] = ang[3];
    func_8000EB00(D_800D60C0, x, y, ang[3]);
}

void func_80012D30(u8 *p)
{
    u16 t1 = *(u16 *)(p + 0x2);
    u16 t0 = *(u16 *)(p + 0x8);
    u16 a3 = *(u16 *)(p + 0x10);
    u16 a2 = *(u16 *)(p + 0x12);
    u16 a1 = *(u16 *)(p + 0x28);
    u16 v1 = *(u16 *)(p + 0x30);
    u16 v0 = *(u16 *)(p + 0x32);
    *(u16 *)(p + 0x8) = t1;
    t1 = *(u16 *)(p + 0x4);
    *(u16 *)(p + 0x10) = t1;
    t1 = *(u16 *)(p + 0xC);
    *(u16 *)(p + 0x12) = t1;
    t1 = *(u16 *)(p + 0x22);
    *(u16 *)(p + 0x28) = t1;
    t1 = *(u16 *)(p + 0x24);
    *(u16 *)(p + 0x30) = t1;
    t1 = *(u16 *)(p + 0x2C);
    *(u16 *)(p + 0x2) = t0;
    *(u16 *)(p + 0x4) = a3;
    *(u16 *)(p + 0xC) = a2;
    *(u16 *)(p + 0x22) = a1;
    *(u16 *)(p + 0x24) = v1;
    *(u16 *)(p + 0x2C) = v0;
    *(u16 *)(p + 0x32) = t1;
}

void func_80012D94(s32 *p)
{
    s32 t;
    t = p[1];
    p[1] = p[3];
    p[3] = t;
    t = p[2];
    p[2] = p[6];
    p[6] = t;
    t = p[5];
    p[5] = p[7];
    p[7] = t;
}

void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3)
{
    s32 iVar1;

    iVar1 = param_1[0] * param_2[0] + param_1[1] * param_2[3] + param_1[2] * param_2[6];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[0] = iVar1 >> 14;
    iVar1 = param_1[0] * param_2[1] + param_1[1] * param_2[4] + param_1[2] * param_2[7];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[1] = iVar1 >> 14;
    iVar1 = param_1[0] * param_2[2] + param_1[1] * param_2[5] + param_1[2] * param_2[8];
    if (iVar1 < 0) {
        iVar1 = iVar1 + 0x3fff;
    }
    param_3[2] = iVar1 >> 14;
}

typedef struct {
    s32 m[10];
    s32 r58[2];
    s32 pad[8];
    u16 l30[2];
    s32 iS;
    u16 l28;
    u16 l26[3];
} Frame;

void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4)
{
    Frame f;

    func_8001254C(param_1[2], param_1[0], f.l30, &f.iS);
    func_80011A58(f.m, 0, 0, -(s16)f.l30[0]);
    func_80011C1C(param_1, f.m, f.r58);
    func_8001254C(f.r58[1], f.r58[0], &f.l28, &f.iS);
    func_80011A58(f.m, 0, -(s16)f.l28, 0);
    func_80011C1C(f.r58, f.m, param_1);
    func_8001254C(param_1[5], param_1[4], f.l26, &f.iS);
    func_80011A58(f.m, -(s16)f.l26[0], 0, 0);
    func_80011C1C(param_1, f.m, f.r58);
    *param_2 = f.l26[0];
    *param_3 = f.l28;
    *param_4 = f.l30[0];
}

s32 func_80012FE4(s32 a) {
    s32 r = func_80013010(a);
    return (s16)(0x4000 - r);
}

s32 func_80013010(s32 x)
{
    s32 t;
    u16 *tbl;
    u16 *tbl2;
    if (x > 0) {
        if (x >= 0x4000) {
            return 0x4000;
        }
        t = x + 2;
        tbl = D_800C6C64;
        if (t < 0) {
            t = x + 5;
        }
        return ((s16 *)tbl)[t >> 2];
    }
    if (x < -0x3FFF) {
        return -0x4000;
    }
    x = 2 - x;
    tbl2 = D_800C6C64;
    if (x < 0) {
        x = x + 3;
    }
    return (s16)(-tbl2[x >> 2]);
}

f32 func_80013098(f32 p1, f32 p2)
{
    f32 r = 0.0f;
    if (p2 < 0.0f) {
        r = func_80013098(p1, -p2);
        r = -r;
    } else if (p1 < 0.0f) {
        r = func_80013098(-p1, p2);
        r = 3.1415927f - r;
    } else if (p1 < p2) {
        r = func_80013098(p2, p1);
        r = 1.5707964f - r;
    } else if (p1 != 0.0f || p2 != 0.0f) {
        r = (f32)D_800C4C60[(s32)(p2 * 4096.0f / p1)] * 3.1415927f * 3.0517578e-05f;
    }
    return r;
}

f32 func_8001318C(f32 p) {
    s32 x;
    s32 v;
    s32 r;

    x = (s32)(p * 16384.0f);
    if (x <= 0) {
        goto neg;
    }
    if (x < 0x4000) {
        goto tbl;
    }
    r = 0x4000;
    goto done;
tbl:
    {
        u16 *tb;
        v = x + 2;
        tb = D_800C6C64;
        if (v < 0) {
            v = x + 5;
        }
        r = tb[v >> 2];
    }
    goto done;
neg:
    if (x < -0x3FFF) {
        r = -0x4000;
        goto done;
    }
    {
        u16 *tb;
        v = 2 - x;
        tb = D_800C6C64;
        if (v < 0) {
            v = v + 3;
        }
        r = -tb[v >> 2];
    }
done:
    return (f32)(s16)r * 3.1415927f * 3.0517578e-05f;
}

void func_80013248(f32 m[3][3])
{
    f32 t;

    t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t;
    t = m[0][2]; m[0][2] = m[2][0]; m[2][0] = t;
    t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
}

void func_8001327C(f32 *v, f32 *m, f32 *o)
{
    o[0] = v[0] * m[0] + v[1] * m[3] + v[2] * m[6];
    o[1] = v[0] * m[1] + v[1] * m[4] + v[2] * m[7];
    o[2] = v[0] * m[2] + v[1] * m[5] + v[2] * m[8];
}

void func_80013310(f32 *a, f32 *b, f32 *o)
{
    o[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    o[1] = a[0] * b[3] + a[1] * b[4] + a[2] * b[5];
    o[2] = a[0] * b[6] + a[1] * b[7] + a[2] * b[8];
}

void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r)
{
    r->x = a->y * b->z - b->y * a->z;
    r->y = a->z * b->x - b->z * a->x;
    r->z = a->x * b->y - b->x * a->y;
}

f32 func_80013408(f32 *a, f32 *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void func_80013438(f32 *out, f32 x, f32 y, f32 z)
{
    f32 hx, hy, hz;
    f32 c1, s1, c2, s2, c3, s3;
    f32 p21, p2s, ps1, pss;

    hx = x * 0.5f;
    c1 = func_800ADFC0(hx);
    s1 = func_800AEA30(hx);
    hy = y * 0.5f;
    c2 = func_800ADFC0(hy);
    s2 = func_800AEA30(hy);
    hz = z * 0.5f;
    c3 = func_800ADFC0(hz);
    s3 = func_800AEA30(hz);

    p21 = c2 * c1;
    p2s = c2 * s1;
    ps1 = s2 * c1;
    pss = s2 * s1;
    out[0] = p21 * c3 + pss * s3;
    out[1] = -ps1 * s3 + p2s * c3;
    out[2] = -p21 * s3 + pss * c3;
    out[3] = p2s * s3 + ps1 * c3;
}

void func_80013568(f32 *m, f32 *q)
{
    f32 xx, xy, xz, yy, yz, zz, wx, wy, wz;

    xx = q[1] * q[1];
    xy = q[1] * q[2];
    xz = q[1] * q[3];
    yy = q[2] * q[2];
    yz = q[2] * q[3];
    zz = q[3] * q[3];
    wx = q[0] * q[1];
    wy = q[0] * q[2];
    wz = q[0] * q[3];
    m[0] = (0.5f - yy - zz) * 2.0f;
    m[1] = (wz + xy) * 2.0f;
    m[2] = (xz - wy) * 2.0f;
    m[3] = (xy - wz) * 2.0f;
    m[4] = (0.5f - xx - zz) * 2.0f;
    m[5] = (yz + wx) * 2.0f;
    m[6] = (xz + wy) * 2.0f;
    m[7] = (yz - wx) * 2.0f;
    m[8] = (0.5f - xx - yy) * 2.0f;
}

void func_80013650(s32 *m, f32 *q)
{
    f32 x, y, z, w;
    f32 xy, xz, xw, yy, yz, yw, zz, zw, ww;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
    yy = y * y;
    yz = y * z;
    yw = y * w;
    zz = z * z;
    zw = z * w;
    ww = w * w;
    xy = x * y;
    xz = x * z;
    xw = x * w;
    m[0] = (s32)((0.5f - zz - ww) * 32768.0f);
    m[1] = (s32)((xw + yz) * 32768.0f);
    m[2] = (s32)((yw - xz) * 32768.0f);
    m[3] = (s32)((yz - xw) * 32768.0f);
    m[4] = (s32)((0.5f - yy - ww) * 32768.0f);
    m[5] = (s32)((zw + xy) * 32768.0f);
    m[6] = (s32)((yw + xz) * 32768.0f);
    m[7] = (s32)((zw - xy) * 32768.0f);
    m[8] = (s32)((0.5f - yy - zz) * 32768.0f);
}

f32 func_80013774(f32 x)
{
    f32 r, u, p;
    s32 i;

    if (10.0f < x) {
        return 0.0f;
    }
    if (x < -10.0f) {
        return 0.0f;
    }
    if (1.0f < x) {
        return func_80013774(x - 1.0f) * 2.7182817f;
    }
    if (x < -1.0f) {
        return func_80013774(x + 1.0f) / 2.7182817f;
    }
    r = x;
    u = x;
    i = 2;
    do {
        u = u * x / i;
        i++;
        r += u;
        u = u * x / i;
        i++;
        p = u * 65536.0f;
        r += u;
    } while (r > 0.0f ? p < -r : p < r);
    return r + 1.0f;
}

u8 func_800138A4(void) {
    u32 x = D_800E36CC;
    u32 idx = x + 0x67;
    u8 r;
    u32 n;

    if (x > 0x92) {
        idx = x - 0x93;
    }
    r = D_800E35D0[x] ^ D_800E35D0[idx];
    D_800E35D0[x] = r;
    n = D_800E36CC + 1;
    D_800E36CC = n;
    if (n > 0xF9) {
        D_800E36CC = 0;
    }
    return r;
}

u32 func_800138FC(f32 *p) {
    f32 c, d;
    s32 i;
    u32 r;

    c = func_800AEA30(*p + 4.0f) * 256.0f;
    i = (s32)c;
    c = c - (f32)i;
    if (c < 0.0f) {
        c = -c;
    }
    d = c * 256.0f;
    *p = c;
    if (!(2147483648.0f <= d)) {
        r = (s32)d;
    } else {
        r = (s32)(d - 2147483648.0f);
        r = r | 0x80000000;
    }
    return r;
}

void func_800139A8(s32 *src, u16 *aux, s16 *dst)
{
    u16 *h = (u16 *)src;
    dst[0] = src[0] >> 14;
    dst[16] = h[1] << 2;
    dst[1] = src[1] >> 14;
    dst[17] = h[3] << 2;
    dst[2] = src[2] >> 14;
    {
        u16 t = h[5];
        dst[3] = 0;
        dst[19] = 0;
        dst[18] = t << 2;
    }
    dst[4] = src[3] >> 14;
    dst[20] = h[7] << 2;
    dst[5] = src[4] >> 14;
    dst[21] = h[9] << 2;
    dst[6] = src[5] >> 14;
    {
        u16 t = h[11];
        dst[7] = 0;
        dst[23] = 0;
        dst[22] = t << 2;
    }
    dst[8] = src[6] >> 14;
    dst[24] = h[13] << 2;
    dst[9] = src[7] >> 14;
    dst[25] = h[15] << 2;
    dst[10] = src[8] >> 14;
    {
        u16 t = h[17];
        dst[11] = 0;
        dst[27] = 0;
        dst[26] = t << 2;
    }
    {
        u16 t = aux[1];
        dst[28] = 0;
        dst[12] = t;
    }
    {
        u16 t = aux[3];
        dst[29] = 0;
        dst[13] = t;
    }
    {
        u16 t = aux[5];
        dst[30] = 0;
        dst[15] = 1;
        dst[31] = 0;
        dst[14] = t;
    }
}

struct Obj_5 {
    u8 pad0[0x34];
    s32 pos[3];
    s32 rot[3][3];
    u8 pad64[0x80 - 0x64];
};

void func_80013ACC(Obj_5 *src, MtxS_2 *dst, s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        dst->i[0][0] = src->rot[0][0] >> 14;
        dst->f[0][0] = src->rot[0][0] << 2;
        dst->i[0][1] = src->rot[0][1] >> 14;
        dst->f[0][1] = src->rot[0][1] << 2;
        dst->i[0][2] = src->rot[0][2] >> 14;
        dst->f[0][2] = src->rot[0][2] << 2;
        dst->i[0][3] = 0;
        dst->f[0][3] = 0;
        dst->i[1][0] = src->rot[1][0] >> 14;
        dst->f[1][0] = src->rot[1][0] << 2;
        dst->i[1][1] = src->rot[1][1] >> 14;
        dst->f[1][1] = src->rot[1][1] << 2;
        dst->i[1][2] = src->rot[1][2] >> 14;
        dst->f[1][2] = src->rot[1][2] << 2;
        dst->i[1][3] = 0;
        dst->f[1][3] = 0;
        dst->i[2][0] = src->rot[2][0] >> 14;
        dst->f[2][0] = src->rot[2][0] << 2;
        dst->i[2][1] = src->rot[2][1] >> 14;
        dst->f[2][1] = src->rot[2][1] << 2;
        dst->i[2][2] = src->rot[2][2] >> 14;
        dst->f[2][2] = src->rot[2][2] << 2;
        dst->i[2][3] = 0;
        dst->f[2][3] = 0;
        dst->i[3][0] = src->pos[0];
        dst->f[3][0] = 0;
        dst->i[3][1] = src->pos[1];
        dst->f[3][1] = 0;
        dst->i[3][2] = src->pos[2];
        dst->f[3][2] = 0;
        dst->i[3][3] = 1;
        dst->f[3][3] = 0;
        dst++;
        src++;
    }
}

void func_80013C20(f32 *o, f32 *p, f32 *q) {
    f32 p0 = p[0];
    f32 p1 = p[1];
    f32 q0 = q[0];
    f32 q1 = q[1];
    f32 p3 = p[3];
    f32 p2 = p[2];
    f32 q2 = q[2];
    f32 q3 = q[3];
    f32 P1 = (p0 + p1) * (q0 + q1);
    f32 P2 = (p3 - p2) * (q2 - q3);
    f32 P3 = (p1 - p0) * (q2 + q3);
    f32 P4 = (p2 + p3) * (q1 - q0);
    f32 P5 = (p1 + p3) * (q1 + q2);
    f32 P6 = (p1 - p3) * (q1 - q2);
    f32 P7 = (p0 + p2) * (q0 - q3);
    f32 P8 = (p0 - p2) * (q0 + q3);
    o[0] = P2 + ((-P5 - P6) + P7 + P8) * 0.5f;
    o[1] = P1 - ((P5 + P6) + P7 + P8) * 0.5f;
    o[2] = -P3 + ((P5 - P6) + P7 - P8) * 0.5f;
    o[3] = -P4 + ((P5 - P6) - P7 + P8) * 0.5f;
}

f32 func_80013D3C(f32 x)
{
    return x + x;
}
