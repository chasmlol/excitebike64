#include "game.h"

/* Maths: angles (0x10000 per turn) and the cosine table gCosTable (1.0 = 0x4000), 2.14 fixed-point 3x3 matrices
   (mtx3i_*), f32 3x3 matrices (mtx3f_*), the N64 Mtx (mtx_*), quaternions (w, x, y, z) and the R250 random
   generator. Note that gCosTable[i] is the cosine and gCosTable[i - 0x400] the sine (COS_IDX, SIN_IDX). */

/* The N64 Mtx: 16.16 fixed point, the integer halves of all elements, then the fractions. */
struct MtxS_2 {
    s16 i[4][4];
    u16 f[4][4];
};

/* Store the 16.16 value x in element [r][c] of m, unscaled or times scale / 0x100. */
#define MTX_SET(r, c, x) { v = (x); m->i[r][c] = v >> 16; m->f[r][c] = v; }
#define MTX_SET_SCALED(r, c, x) { v = (x); v = v * scale / 256; m->i[r][c] = v >> 16; m->f[r][c] = v; }

/* Builds an N64 Mtx from three angles (0x10000 per turn), a translation in 1/16 units and a scale (0x100 = 1.0).
   The rotation is the one of mtx3i_from_angles. */
void mtx_from_angles_pos_scale(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale)
{
    s32 sinX, sinY, cosX, cosY, cosZ, sinZ;
    s32 sinYsinX, sinYcosX;
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
    sinX = SIN_IDX(rx);
    sinY = SIN_IDX(ry);
    cosX = COS_IDX(rx);
    cosY = COS_IDX(ry);
    cosZ = COS_IDX(rz);
    sinZ = SIN_IDX(rz);
    sinYsinX = sinY * sinX / FIX14_ONE;
    sinYcosX = sinY * cosX / FIX14_ONE;
    /* products of two 2.14 values >> 12 are 16.16 */
    if (scale == 0x100) {
        MTX_SET(0, 0, (cosY * cosZ) >> 12);
        MTX_SET(0, 1, sinY << 2);
        MTX_SET(0, 2, (cosY * sinZ) >> 12);
        MTX_SET(1, 0, (-cosZ * sinYcosX + -sinX * sinZ) >> 12);
        MTX_SET(1, 1, (cosX * cosY) >> 12);
        MTX_SET(1, 2, (-sinZ * sinYcosX + sinX * cosZ) >> 12);
        MTX_SET(2, 0, (cosZ * sinYsinX + -cosX * sinZ) >> 12);
        MTX_SET(2, 1, (-sinX * cosY) >> 12);
        MTX_SET(2, 2, (sinZ * sinYsinX + cosX * cosZ) >> 12);
    } else {
        MTX_SET_SCALED(0, 0, (cosY * cosZ) >> 12);
        MTX_SET_SCALED(0, 1, sinY << 2);
        MTX_SET_SCALED(0, 2, (cosY * sinZ) >> 12);
        MTX_SET_SCALED(1, 0, (-cosZ * sinYcosX + -sinX * sinZ) >> 12);
        MTX_SET_SCALED(1, 1, (cosX * cosY) >> 12);
        MTX_SET_SCALED(1, 2, (-sinZ * sinYcosX + sinX * cosZ) >> 12);
        MTX_SET_SCALED(2, 0, (cosZ * sinYsinX + -cosX * sinZ) >> 12);
        MTX_SET_SCALED(2, 1, (-sinX * cosY) >> 12);
        MTX_SET_SCALED(2, 2, (sinZ * sinYsinX + cosX * cosZ) >> 12);
    }
}

/* Builds an N64 Mtx rotating about y (by the angle whose cosine and sine are given) and scaling by scale / 0x100,
   with no translation. Elements [1][0] and [1][2] get -cos and -sin without the << 14 of the others: nearly 0. */
void mtx_from_rot_y_scale(MtxS_2 *m, f32 cosY, f32 sinY, s32 scale)
{
    s32 cosFix = (s32)(cosY * FIX14_ONE_F);
    s32 sinFix = (s32)(sinY * FIX14_ONE_F);
    s32 v;

    mem_fill((u8 *)m, 0, 0x40);
    v = (cosFix << 14) >> 12;
    v = (v * scale) / 256;
    m->f[0][0] = v;
    m->i[0][0] = v >> 16;
    m->i[0][1] = 0;
    m->f[0][1] = 0;
    v = (sinFix << 14) >> 12;
    v = (v * scale) / 256;
    m->f[0][2] = v;
    m->i[0][2] = v >> 16;
    v = -cosFix >> 12;
    v = (v * scale) / 256;
    m->f[1][0] = v;
    m->i[1][0] = v >> 16;
    v = scale << 8;
    m->f[1][1] = v;
    m->i[1][1] = v >> 16;
    v = -sinFix >> 12;
    v = (v * scale) / 256;
    m->f[1][2] = v;
    m->i[1][2] = v >> 16;
    v = (-(sinFix << 14)) >> 12;
    v = (v * scale) / 256;
    m->i[2][0] = v >> 16;
    m->f[2][0] = v;
    m->i[2][1] = 0;
    m->f[2][1] = 0;
    v = (cosFix * (scale << 2)) / 256;
    m->i[2][2] = v >> 16;
    m->f[2][2] = v;
}

/* Builds a 2.14 rotation matrix from three angles (0x10000 per turn). mtx3i_to_angles and quat_to_angles undo it. */
void mtx3i_from_angles(s32 *m, s32 ax, s32 ay, s32 az)
{
    s32 sinYcosX;
    s32 sinZ;
    s32 cosY;
    s32 sinYsinX;
    s32 cosZ;
    s32 cosX;
    s32 sinX;
    s32 sinY;
    s32 sinYsinXFix;
    s32 sinYcosXFix;

    sinX = SIN_IDX(ax >> 4);
    sinY = SIN_IDX(ay >> 4);
    sinYsinX = sinY * sinX;
    cosX = COS_IDX(ax >> 4);
    cosY = COS_IDX(ay >> 4);
    cosZ = COS_IDX(az >> 4);
    sinZ = SIN_IDX(az >> 4);
    /* x / 0x4000 rounded towards 0, written out */
    if (sinYsinX < 0) {
        sinYsinX = sinYsinX + 0x3fff;
    }
    sinYsinXFix = sinYsinX >> 14;
    sinYcosX = sinY * cosX;
    if (sinYcosX < 0) {
        sinYcosX = sinYcosX + 0x3fff;
    }
    sinYcosXFix = sinYcosX >> 14;
    m[1] = sinY;
    m[0] = (cosY * cosZ) >> 14;
    m[2] = (cosY * sinZ) >> 14;
    m[3] = (-cosZ * sinYcosXFix + -sinX * sinZ) >> 14;
    m[4] = (cosX * cosY) >> 14;
    m[5] = (-sinZ * sinYcosXFix + sinX * cosZ) >> 14;
    m[6] = (cosZ * sinYsinXFix + -cosX * sinZ) >> 14;
    m[7] = (-sinX * cosY) >> 14;
    m[8] = (sinZ * sinYsinXFix + cosX * cosZ) >> 14;
}

/* out = a * b, 2.14 matrices. Each sum is divided by 0x4000 rounding towards 0, written out. */
void mtx3i_mul(s32 *a, s32 *b, s32 *out)
{
    s32 t;

    t = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[0] = t >> 14;
    t = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[1] = t >> 14;
    t = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[2] = t >> 14;
    t = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[3] = t >> 14;
    t = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[4] = t >> 14;
    t = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[5] = t >> 14;
    t = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[6] = t >> 14;
    t = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[7] = t >> 14;
    t = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[8] = t >> 14;
}

/* out = a * transpose(b), f32 3x3 matrices. */
void mtx3f_mul_transposed(f32 *out, f32 *a, f32 *b)
{
    out[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    out[1] = a[0] * b[3] + a[1] * b[4] + a[2] * b[5];
    out[2] = a[0] * b[6] + a[1] * b[7] + a[2] * b[8];
    out[3] = a[3] * b[0] + a[4] * b[1] + a[5] * b[2];
    out[4] = a[3] * b[3] + a[4] * b[4] + a[5] * b[5];
    out[5] = a[3] * b[6] + a[4] * b[7] + a[5] * b[8];
    out[6] = a[6] * b[0] + a[7] * b[1] + a[8] * b[2];
    out[7] = a[6] * b[3] + a[7] * b[4] + a[8] * b[5];
    out[8] = a[6] * b[6] + a[7] * b[7] + a[8] * b[8];
}

/* Element i (0-15) of an N64 Mtx as a 16.16 value. */
#define MTX_ELEM(m, i) (((s32)(m)[i] << 16) | (u16)(m)[(i) + 16])

/* out = a * b, N64 Mtx, summed in f32. A sum beyond +-2^30 is computed again with both factors / 4 and the result
   << 4, so it wraps instead of overflowing the conversion. */
void mtx_mul(s16 *a, s16 *b, s16 *out)
{
    s32 row, col, k, ea, outIdx, rowStart, eb;
    f32 sum;

    for (row = 0; row < 4; row++) {
        col = 0;
        outIdx = row * 4;
        rowStart = outIdx;
    next:
        sum = 0.0f;
        do {
            for (k = 0; k < 4; k++) {
                ea = MTX_ELEM(a, rowStart + k);
                eb = MTX_ELEM(b, k * 4 + col);
                sum += (f32)ea * (f32)eb * (1.0f / 65536.0f);
            }
        } while (0);
        if (sum < -1073741800.0f || 1073741800.0f < sum) {
            sum = 0.0f;
            do {
                for (k = 0; k < 4; k++) {
                    ea = MTX_ELEM(a, rowStart + k);
                    eb = MTX_ELEM(b, k * 4 + col);
                    ea = (f32)ea * 0.25f;
                    eb = (f32)eb * 0.25f;
                    sum += (f32)ea * (f32)eb * (1.0f / 65536.0f);
                }
            } while (0);
            ea = (s32)sum << 4;
            out[outIdx] = ea >> 16;
            out[outIdx + 16] = ea;
        } else {
            ea = (s32)sum;
            out[outIdx] = ea >> 16;
            out[outIdx + 16] = ea;
        }
        outIdx++;
        col++;
        if (col < 4) {
            goto next;
        }
    }
}

/* Normalizes the quaternion quat (in place) and gives the three angles of mtx3i_from_angles that rotate the same
   way. When the y angle is a quarter turn (its cosine is 0) the outputs are left unchanged. */
void quat_to_angles(f32 *quat, s32 *outX, s32 *outY, s32 *outZ)
{
    s32 rot[9];
    s32 *m;
    f32 sum, len, w, x, y, z;
    f32 xx, xz, xy, yy, zz, yz, wx, wz, wy;
    f32 e1, e2, e3, e4, e5, e6, e7, e8, e0;
    s32 angleY, cosY, ay, az, ax;

    sum = quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3];
    len = __builtin_sqrtf(sum);
    quat[0] /= len;
    quat[1] /= len;
    quat[2] /= len;
    quat[3] /= len;
    w = quat[0];
    x = quat[1];
    y = quat[2];
    z = quat[3];
    xx = x * x;
    xy = x * y;
    xz = x * z;
    yy = y * y;
    yz = y * z;
    zz = z * z;
    wx = w * x;
    wy = w * y;
    wz = w * z;
    /* The rotation matrix in 2.14 (the halved terms times 0x8000), as quat_to_mtx3i. */
    e1 = (wz + xy) * 32768.0f;
    e2 = (xz - wy) * 32768.0f;
    e3 = (xy - wz) * 32768.0f;
    e4 = ((0.5f - xx) - zz) * 32768.0f;
    e5 = (yz + wx) * 32768.0f;
    e6 = (xz + wy) * 32768.0f;
    e7 = (yz - wx) * 32768.0f;
    e8 = ((0.5f - xx) - yy) * 32768.0f;
    e0 = ((0.5f - yy) - zz) * 32768.0f;
    m = rot;
    m[1] = (s32)e1;
    m[2] = (s32)e2;
    m[3] = (s32)e3;
    m[5] = (s32)e5;
    m[6] = (s32)e6;
    m[7] = (s32)e7;
    m[4] = (s32)e4;
    m[8] = (s32)e8;
    m[0] = (s32)e0;
    /* m[1] = sin y, m[2] = cos y sin z, m[7] = -sin x cos y (mtx3i_from_angles) */
    angleY = math_asini(rot[1]);
    cosY = gCosTable[((u32)(angleY << 16)) >> 20];
    ay = (s16)angleY;
    if (cosY != 0) {
        az = math_asini((rot[2] << 14) / cosY);
        /* asin gives -90..90 degrees: take the other angle when the sign of cos z (m[0]) says so */
        if ((rot[0] ^ cosY ^ ((s16)az - ANGLE_90)) >= 0) {
            az = -ANGLE_180 - az;
        }
        ax = math_asini((-(rot[7] << 14)) / cosY);
        if ((rot[4] ^ cosY ^ ((s16)ax - ANGLE_90)) >= 0) {
            ax = -ANGLE_180 - ax;
        }
        *outX = (s16)ax;
        *outY = ay;
        *outZ = (s16)az;
    }
}

/* The angle of the vector (x, y) (0 along +y, ANGLE_90 along +x) and its length, from the atan table (0x1000 *
   the smaller / the larger coordinate) and the length as a coordinate divided by the angle's cosine or sine. */
void math_to_polar(s32 x, s32 y, s16 *outAngle, s32 *outLength)
{
    s32 angle;
    s32 t;
    s32 negX, negY;
    s32 unused;

    if (x == 0 && y == 0) {
        *outLength = 0;
        *outAngle = 0;
        return;
    }
    if (x > 0) {
        if (y > 0) {
            if (x < y) {
                t = sAtanTable[(x << 12) / y];
                *outLength = (y << 14) / COS_IDX(t >> 4);
                angle = t;
                *outAngle = angle;
                return;
            } else {
                angle = ANGLE_90 - sAtanTable[(y << 12) / x];
                *outLength = (x << 14) / SIN_IDX(angle >> 4);
                *outAngle = angle;
                return;
            }
        } else {
            if (-y < x) {
                angle = sAtanTable[(-y << 12) / x] + ANGLE_90;
                *outLength = (x << 14) / SIN_IDX(angle >> 4);
                *outAngle = angle;
                return;
            } else {
                angle = ANGLE_180 - sAtanTable[(x << 12) / -y];
                *outLength = (y << 14) / COS_IDX(angle >> 4);
                *outAngle = angle;
                return;
            }
        }
    } else {
        negX = -x;
        if (y < 0) {
            negY = -y;
            if (negX < negY) {
                angle = sAtanTable[(negX << 12) / negY] + ANGLE_180;
                *outLength = (y << 14) / COS_IDX(angle >> 4);
                *outAngle = angle;
                return;
            } else {
                angle = ANGLE_270 - sAtanTable[(negY << 12) / negX];
                *outLength = (x << 14) / SIN_IDX(angle >> 4);
                *outAngle = angle;
                return;
            }
        } else {
            if (y < negX) {
                angle = sAtanTable[(y << 12) / negX] + ANGLE_270;
                *outLength = (x << 14) / SIN_IDX(angle >> 4);
                *outAngle = angle;
                return;
            } else {
                angle = -sAtanTable[(negX << 12) / y];
                *outLength = (y << 14) / COS_IDX(angle >> 4);
                *outAngle = angle;
                return;
            }
        }
    }
}

/* The f32 version of mtx3i_from_angles. */
void mtx3f_from_angles(f32 *m, s16 ax, s16 ay, s16 az)
{
    f32 cosX, cosY, sinY, cosZ, sinZ;
    f32 sinX;
    s32 idxX, idxY, idxZ;
    s32 cosXFix, sinXFix;
    s16 *tbl;
    f32 sinYsinX, sinYcosX;

    tbl = gCosTable;
    idxX = ax >> 4;
    cosXFix = tbl[idxX & 0xFFF];
    sinXFix = tbl[(idxX - 0x400) & 0xFFF];
    idxY = ay >> 4;
    cosX = cosXFix * FIX14_TO_FLOAT;
    sinX = sinXFix * FIX14_TO_FLOAT;
    cosY = tbl[idxY & 0xFFF] * FIX14_TO_FLOAT;
    sinY = tbl[(idxY - 0x400) & 0xFFF] * FIX14_TO_FLOAT;
    idxZ = az >> 4;
    cosZ = tbl[idxZ & 0xFFF] * FIX14_TO_FLOAT;
    sinZ = tbl[(idxZ - 0x400) & 0xFFF] * FIX14_TO_FLOAT;
    sinYsinX = sinY * sinX;
    sinYcosX = sinY * cosX;
    m[0] = cosY * cosZ;
    m[1] = sinY;
    m[2] = cosY * sinZ;
    m[3] = -cosZ * sinYcosX + -sinX * sinZ;
    m[4] = cosX * cosY;
    m[5] = -sinZ * sinYcosX + sinX * cosZ;
    m[6] = cosZ * sinYsinX + -cosX * sinZ;
    m[7] = -sinX * cosY;
    m[8] = sinZ * sinYsinX + cosX * cosZ;
}

/* r250_next again, for r250_init to inline. */
static inline u8 r250_next_inline(void)
{
    u32 i = sR250Index;
    u32 j;
    u8 r;

    if (i >= R250_LAG) {
        j = i - R250_LAG;
    } else {
        j = i + (R250_SIZE - R250_LAG);
    }
    r = sR250Buffer[i] ^= sR250Buffer[j];
    if (++sR250Index >= R250_SIZE) {
        sR250Index = 0;
    }
    return r;
}

/* Seeds the R250 generator: fills sR250Buffer from a small multiplicative generator, makes eight bytes linearly
   independent (one bit each, the standard R250 setup) and runs it 500 times. */
void r250_init(void)
{
    u32 seed;
    s32 i;
    u8 *p;
    u8 msb;
    u8 mask;

    sR250Index = 0;
    p = sR250Buffer;
    seed = 12345;
    for (i = R250_SIZE; i != 0; i--) {
        seed = (seed * 1277) & 0xFFFF;
        *p++ = seed >> 8;
    }
    msb = 0x80;
    mask = 0xFF;
    p = &sR250Buffer[14];
    while (msb != 0) {
        *p &= mask;
        *p |= msb;
        p += 11;
        msb >>= 1;
        mask >>= 1;
    }
    for (i = 0; (u32)i < 500; i++) {
        r250_next_inline();
    }
}

/* out = in * the rotation of mtx3i_from_angles(rx, ry, rz), with tmp for the rotation. */
static __inline__ void mtx3i_rotate_inline(s32 *in, s32 *tmp, s32 *out, s32 rx, s32 ry, s32 rz)
{
    mtx3i_from_angles(tmp, rx, ry, rz);
    mtx3i_mul(in, tmp, out);
}

/* A test of mtx3i_to_angles's method: builds a matrix from (0, 0, 0x2000), takes the angles back out one rotation
   at a time and prints them (debug_printf prints nothing). */
void mtx3i_test_angles(void)
{
    s32 mtxA[10];
    s32 step[10];
    s32 mtxB[10];
    s16 angles[4];
    s32 len;
    s16 angleY;
    s16 angleX;
    s32 *pA;

    mtx3i_from_angles(mtxA, 0, 0, 0x2000);     /* an eighth of a turn about z */
    pA = mtxA;
    math_to_polar(pA[2], mtxA[0], &angles[3], &len);
    mtx3i_rotate_inline(pA, step, mtxB, 0, 0, -angles[3]);
    math_to_polar(mtxB[1], mtxB[0], &angleY, &len);
    mtx3i_rotate_inline(mtxB, step, pA, 0, -angleY, 0);
    math_to_polar(pA[5], pA[4], &angleX, &len);
    mtx3i_rotate_inline(pA, step, mtxB, -angleX, 0, 0);
    angles[0] = angleX;
    angles[1] = angleY;
    angles[2] = angles[3];
    debug_printf(sStrTestAnglesFmt, angleX, angleY, angles[3]);
}

/* Transposes the 3x3 rotation of an N64 Mtx in place: the s16 integer halves at +0x00 (row * 8 + column * 2), the
   u16 fractions at +0x20. */
void mtx_transpose_rot(u8 *mtx)
{
    u16 tmp = *(u16 *)(mtx + 0x2);
    u16 i10 = *(u16 *)(mtx + 0x8);
    u16 i20 = *(u16 *)(mtx + 0x10);
    u16 i21 = *(u16 *)(mtx + 0x12);
    u16 f10 = *(u16 *)(mtx + 0x28);
    u16 f20 = *(u16 *)(mtx + 0x30);
    u16 f21 = *(u16 *)(mtx + 0x32);
    *(u16 *)(mtx + 0x8) = tmp;
    tmp = *(u16 *)(mtx + 0x4);
    *(u16 *)(mtx + 0x10) = tmp;
    tmp = *(u16 *)(mtx + 0xC);
    *(u16 *)(mtx + 0x12) = tmp;
    tmp = *(u16 *)(mtx + 0x22);
    *(u16 *)(mtx + 0x28) = tmp;
    tmp = *(u16 *)(mtx + 0x24);
    *(u16 *)(mtx + 0x30) = tmp;
    tmp = *(u16 *)(mtx + 0x2C);
    *(u16 *)(mtx + 0x2) = i10;
    *(u16 *)(mtx + 0x4) = i20;
    *(u16 *)(mtx + 0xC) = i21;
    *(u16 *)(mtx + 0x22) = f10;
    *(u16 *)(mtx + 0x24) = f20;
    *(u16 *)(mtx + 0x2C) = f21;
    *(u16 *)(mtx + 0x32) = tmp;
}

/* Transposes a 3x3 matrix (s32[9]) in place. */
void mtx3i_transpose(s32 *m)
{
    s32 t;
    t = m[1];
    m[1] = m[3];
    m[3] = t;
    t = m[2];
    m[2] = m[6];
    m[6] = t;
    t = m[5];
    m[5] = m[7];
    m[7] = t;
}

/* out = v * m (a row vector times a 2.14 matrix), rounding towards 0. */
void vec3i_mul_mtx3i(s32 *v, s32 *m, s32 *out)
{
    s32 t;

    t = v[0] * m[0] + v[1] * m[3] + v[2] * m[6];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[0] = t >> 14;
    t = v[0] * m[1] + v[1] * m[4] + v[2] * m[7];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[1] = t >> 14;
    t = v[0] * m[2] + v[1] * m[5] + v[2] * m[8];
    if (t < 0) {
        t = t + 0x3fff;
    }
    out[2] = t >> 14;
}

/* mtx3i_to_angles's locals, laid out as the original's stack frame. */
typedef struct {
    s32 step[10];       /* the rotation that undoes one angle */
    s32 work[2];        /* with pad: a 3x3 matrix */
    s32 pad[8];
    u16 angleZ[2];
    s32 len;
    u16 angleY;
    u16 angleX[3];
} AnglesFrame;

/* The three angles of mtx3i_from_angles that give the 2.14 rotation m, found by undoing the z, then the y, then the
   x rotation (math_to_polar on two elements each time). m is changed. */
void mtx3i_to_angles(s32 *m, u16 *outX, u16 *outY, u16 *outZ)
{
    AnglesFrame f;

    math_to_polar(m[2], m[0], f.angleZ, &f.len);
    mtx3i_from_angles(f.step, 0, 0, -(s16)f.angleZ[0]);
    mtx3i_mul(m, f.step, f.work);
    math_to_polar(f.work[1], f.work[0], &f.angleY, &f.len);
    mtx3i_from_angles(f.step, 0, -(s16)f.angleY, 0);
    mtx3i_mul(f.work, f.step, m);
    math_to_polar(m[5], m[4], f.angleX, &f.len);
    mtx3i_from_angles(f.step, -(s16)f.angleX[0], 0, 0);
    mtx3i_mul(m, f.step, f.work);
    *outX = f.angleX[0];
    *outY = f.angleY;
    *outZ = f.angleZ[0];
}

/* acos of a 2.14 value, as an angle (ANGLE_90 - asin). */
s32 math_acosi(s32 x)
{
    s32 angle = math_asini(x);
    return (s16)(ANGLE_90 - angle);
}

/* asin of a 2.14 value as an angle (-ANGLE_90..ANGLE_90), from sAsinTable at (x + 2) / 4 (the division written
   out). */
s32 math_asini(s32 x)
{
    s32 t;
    u16 *tbl;
    u16 *tblNeg;
    if (x > 0) {
        if (x >= FIX14_ONE) {
            return ANGLE_90;
        }
        t = x + 2;
        tbl = sAsinTable;
        if (t < 0) {
            t = x + 5;
        }
        return ((s16 *)tbl)[t >> 2];
    }
    if (x < -0x3FFF) {
        return -ANGLE_90;
    }
    x = 2 - x;
    tblNeg = sAsinTable;
    if (x < 0) {
        x = x + 3;
    }
    return (s16)(-tblNeg[x >> 2]);
}

/* The angle of (x, y) from the x axis in radians, -pi..pi, from the atan table. Note the order: x first. */
f32 math_atan2f(f32 x, f32 y)
{
    f32 r = 0.0f;
    if (y < 0.0f) {
        r = math_atan2f(x, -y);
        r = -r;
    } else if (x < 0.0f) {
        r = math_atan2f(-x, y);
        r = MATH_PI_F - r;
    } else if (x < y) {
        r = math_atan2f(y, x);
        r = MATH_HALF_PI_F - r;
    } else if (x != 0.0f || y != 0.0f) {
        r = (f32)sAtanTable[(s32)(y * 4096.0f / x)] * MATH_PI_F * 3.0517578e-05f;   /* / 0x8000 */
    }
    return r;
}

/* asin in radians, through math_asini's table. */
f32 math_asinf(f32 s)
{
    s32 x;
    s32 idx;
    s32 angle;

    x = (s32)(s * FIX14_ONE_F);
    if (x <= 0) {
        goto neg;
    }
    if (x < FIX14_ONE) {
        goto tbl;
    }
    angle = ANGLE_90;
    goto done;
tbl:
    {
        u16 *tb;
        idx = x + 2;
        tb = sAsinTable;
        if (idx < 0) {
            idx = x + 5;
        }
        angle = tb[idx >> 2];
    }
    goto done;
neg:
    if (x < -0x3FFF) {
        angle = -ANGLE_90;
        goto done;
    }
    {
        u16 *tb;
        idx = 2 - x;
        tb = sAsinTable;
        if (idx < 0) {
            idx = idx + 3;
        }
        angle = -tb[idx >> 2];
    }
done:
    return (f32)(s16)angle * MATH_PI_F * 3.0517578e-05f;    /* / 0x8000 */
}

/* Transposes an f32 3x3 matrix in place. */
void mtx3f_transpose(f32 m[3][3])
{
    f32 t;

    t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t;
    t = m[0][2]; m[0][2] = m[2][0]; m[2][0] = t;
    t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
}

/* out = v * m (a row vector times an f32 3x3 matrix). */
void vec3_mul_mtx3f(f32 *v, f32 *m, f32 *out)
{
    out[0] = v[0] * m[0] + v[1] * m[3] + v[2] * m[6];
    out[1] = v[0] * m[1] + v[1] * m[4] + v[2] * m[7];
    out[2] = v[0] * m[2] + v[1] * m[5] + v[2] * m[8];
}

/* out = m * v (each row of the f32 3x3 matrix m dotted with v): v times the transpose of m. */
void mtx3f_mul_vec3(f32 *v, f32 *m, f32 *out)
{
    out[0] = v[0] * m[0] + v[1] * m[1] + v[2] * m[2];
    out[1] = v[0] * m[3] + v[1] * m[4] + v[2] * m[5];
    out[2] = v[0] * m[6] + v[1] * m[7] + v[2] * m[8];
}

/* out = a x b. */
void vec3_cross(Vec3f *a, Vec3f *b, Vec3f *out)
{
    out->x = a->y * b->z - b->y * a->z;
    out->y = a->z * b->x - b->z * a->x;
    out->z = a->x * b->y - b->x * a->y;
}

f32 vec3_dot(f32 *a, f32 *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/* The quaternion (w, x, y, z) of three angles in radians. */
void quat_from_euler(f32 *out, f32 x, f32 y, f32 z)
{
    f32 hx, hy, hz;
    f32 c1, s1, c2, s2, c3, s3;
    f32 c2c1, c2s1, s2c1, s2s1;

    hx = x * 0.5f;
    c1 = cosf(hx);
    s1 = sinf(hx);
    hy = y * 0.5f;
    c2 = cosf(hy);
    s2 = sinf(hy);
    hz = z * 0.5f;
    c3 = cosf(hz);
    s3 = sinf(hz);

    c2c1 = c2 * c1;
    c2s1 = c2 * s1;
    s2c1 = s2 * c1;
    s2s1 = s2 * s1;
    out[0] = c2c1 * c3 + s2s1 * s3;
    out[1] = -s2c1 * s3 + c2s1 * c3;
    out[2] = -c2c1 * s3 + s2s1 * c3;
    out[3] = c2s1 * s3 + s2c1 * c3;
}

/* The f32 rotation matrix of a unit quaternion q (w, x, y, z). */
void quat_to_mtx3f(f32 *m, f32 *q)
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

/* The 2.14 rotation matrix of a unit quaternion q (w, x, y, z): the halved terms times 0x8000. */
void quat_to_mtx3i(s32 *m, f32 *q)
{
    f32 w, x, y, z;
    f32 wx, wy, wz, xx, xy, xz, yy, yz, zz;

    w = q[0];
    x = q[1];
    y = q[2];
    z = q[3];
    xx = x * x;
    xy = x * y;
    xz = x * z;
    yy = y * y;
    yz = y * z;
    zz = z * z;
    wx = w * x;
    wy = w * y;
    wz = w * z;
    m[0] = (s32)((0.5f - yy - zz) * 32768.0f);
    m[1] = (s32)((wz + xy) * 32768.0f);
    m[2] = (s32)((xz - wy) * 32768.0f);
    m[3] = (s32)((xy - wz) * 32768.0f);
    m[4] = (s32)((0.5f - xx - zz) * 32768.0f);
    m[5] = (s32)((yz + wx) * 32768.0f);
    m[6] = (s32)((xz + wy) * 32768.0f);
    m[7] = (s32)((yz - wx) * 32768.0f);
    m[8] = (s32)((0.5f - xx - yy) * 32768.0f);
}

/* e^x by its series, reducing x to -1..1 with factors of e (2.7182817f). Outside -10..10 it returns 0. */
f32 math_exp(f32 x)
{
    f32 sum, term, termScaled;
    s32 n;

    if (10.0f < x) {
        return 0.0f;
    }
    if (x < -10.0f) {
        return 0.0f;
    }
    if (1.0f < x) {
        return math_exp(x - 1.0f) * 2.7182817f;
    }
    if (x < -1.0f) {
        return math_exp(x + 1.0f) / 2.7182817f;
    }
    sum = x;
    term = x;
    n = 2;
    do {
        term = term * x / n;
        n++;
        sum += term;
        term = term * x / n;
        n++;
        termScaled = term * 65536.0f;
        sum += term;
    } while (sum > 0.0f ? termScaled < -sum : termScaled < sum);
    return sum + 1.0f;
}

/* The next byte of the R250 generator (0x93 = R250_LAG, 0x67 = R250_SIZE - R250_LAG). */
u8 r250_next(void)
{
    u32 i = sR250Index;
    u32 j = i + 0x67;
    u8 r;
    u32 next;

    if (i > 0x92) {
        j = i - 0x93;
    }
    r = sR250Buffer[i] ^ sR250Buffer[j];
    sR250Buffer[i] = r;
    next = sR250Index + 1;
    sR250Index = next;
    if (next > 0xF9) {
        sR250Index = 0;
    }
    return r;
}

/* A pseudo-random byte from a sine: the fraction of sin(*state + 4) * 256, which also becomes the new *state,
   times 256. */
u32 math_sin_rand(f32 *state)
{
    f32 frac, scaled;
    s32 whole;
    u32 r;

    frac = sinf(*state + 4.0f) * 256.0f;
    whole = (s32)frac;
    frac = frac - (f32)whole;
    if (frac < 0.0f) {
        frac = -frac;
    }
    scaled = frac * 256.0f;
    *state = frac;
    /* (u32)scaled, written out */
    if (!(2147483648.0f <= scaled)) {
        r = (s32)scaled;
    } else {
        r = (s32)(scaled - 2147483648.0f);
        r = r | 0x80000000;
    }
    return r;
}

/* Builds an N64 Mtx from a 2.14 rotation (rot, s32[9]) and a position (the low halves of three s32, read as u16
   pairs: pos[1], pos[3], pos[5]). */
void mtx_from_rot3i_pos(s32 *rot, u16 *pos, s16 *dst)
{
    u16 *half = (u16 *)rot;     /* half[2 * i + 1]: the low 16 bits of rot[i] */
    dst[0] = rot[0] >> 14;
    dst[16] = half[1] << 2;
    dst[1] = rot[1] >> 14;
    dst[17] = half[3] << 2;
    dst[2] = rot[2] >> 14;
    {
        u16 t = half[5];
        dst[3] = 0;
        dst[19] = 0;
        dst[18] = t << 2;
    }
    dst[4] = rot[3] >> 14;
    dst[20] = half[7] << 2;
    dst[5] = rot[4] >> 14;
    dst[21] = half[9] << 2;
    dst[6] = rot[5] >> 14;
    {
        u16 t = half[11];
        dst[7] = 0;
        dst[23] = 0;
        dst[22] = t << 2;
    }
    dst[8] = rot[6] >> 14;
    dst[24] = half[13] << 2;
    dst[9] = rot[7] >> 14;
    dst[25] = half[15] << 2;
    dst[10] = rot[8] >> 14;
    {
        u16 t = half[17];
        dst[11] = 0;
        dst[27] = 0;
        dst[26] = t << 2;
    }
    {
        u16 t = pos[1];
        dst[28] = 0;
        dst[12] = t;
    }
    {
        u16 t = pos[3];
        dst[29] = 0;
        dst[13] = t;
    }
    {
        u16 t = pos[5];
        dst[30] = 0;
        dst[15] = 1;
        dst[31] = 0;
        dst[14] = t;
    }
}

/* Builds an N64 Mtx for each of n bones from its world rotation (2.14) and world position. */
void mtx_from_bones(Bone *src, MtxS_2 *dst, s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        dst->i[0][0] = src->worldRot.m[0] >> 14;
        dst->f[0][0] = src->worldRot.m[0] << 2;
        dst->i[0][1] = src->worldRot.m[1] >> 14;
        dst->f[0][1] = src->worldRot.m[1] << 2;
        dst->i[0][2] = src->worldRot.m[2] >> 14;
        dst->f[0][2] = src->worldRot.m[2] << 2;
        dst->i[0][3] = 0;
        dst->f[0][3] = 0;
        dst->i[1][0] = src->worldRot.m[3] >> 14;
        dst->f[1][0] = src->worldRot.m[3] << 2;
        dst->i[1][1] = src->worldRot.m[4] >> 14;
        dst->f[1][1] = src->worldRot.m[4] << 2;
        dst->i[1][2] = src->worldRot.m[5] >> 14;
        dst->f[1][2] = src->worldRot.m[5] << 2;
        dst->i[1][3] = 0;
        dst->f[1][3] = 0;
        dst->i[2][0] = src->worldRot.m[6] >> 14;
        dst->f[2][0] = src->worldRot.m[6] << 2;
        dst->i[2][1] = src->worldRot.m[7] >> 14;
        dst->f[2][1] = src->worldRot.m[7] << 2;
        dst->i[2][2] = src->worldRot.m[8] >> 14;
        dst->f[2][2] = src->worldRot.m[8] << 2;
        dst->i[2][3] = 0;
        dst->f[2][3] = 0;
        dst->i[3][0] = src->worldPos.x;
        dst->f[3][0] = 0;
        dst->i[3][1] = src->worldPos.y;
        dst->f[3][1] = 0;
        dst->i[3][2] = src->worldPos.z;
        dst->f[3][2] = 0;
        dst->i[3][3] = 1;
        dst->f[3][3] = 0;
        dst++;
        src++;
    }
}

/* out = p * q (quaternions (w, x, y, z)), with eight multiplications. */
void quat_mul(f32 *out, f32 *p, f32 *q)
{
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
    out[0] = P2 + ((-P5 - P6) + P7 + P8) * 0.5f;
    out[1] = P1 - ((P5 + P6) + P7 + P8) * 0.5f;
    out[2] = -P3 + ((P5 - P6) + P7 - P8) * 0.5f;
    out[3] = -P4 + ((P5 - P6) - P7 + P8) * 0.5f;
}

/* Returns x + x. */
f32 math_double(f32 x)
{
    return x + x;
}
