#include "game.h"

typedef struct {
    f32 x, y, z, w;
    u8 p0[0x46 - 0x10];
    s16 n46;
    s16 n48;
    s16 p1;
    s16 n4c;
    u8 p2[0x6c - 0x4e];
} Node_8;

extern Node_8 *gPathNodes_12;

f32 path_get_distance(s32 a, s32 b)
{
    f32 acc;
    f32 K;
    f32 w, d, dx, dz;
    s32 i;

    acc = 0.0f;
    K = 10000.0f;
    for (;;) {
        if (a == b) {
            return acc;
        }
        if (K <= acc) {
            return acc;
        }
        w = gPathNodes_12[a].w;
        if (gPathNodes_12[a].n48 >= 0) {
            dx = gPathNodes_12[a].x - gPathNodes_12[gPathNodes_12[a].n48].x;
            dz = gPathNodes_12[a].z - gPathNodes_12[gPathNodes_12[a].n48].z;
            d = __builtin_sqrtf(dx * dx + dz * dz);
            break;
        }
        a = gPathNodes_12[a].n46;
        acc += w;
    }
    i = gPathNodes_12[a].n46;
    while (gPathNodes_12[i].n4c < 0) {
        if (i == b) {
            return acc + w + path_get_distance(gPathNodes_12[a].n46, b);
        }
        i = gPathNodes_12[i].n46;
    }
    i = gPathNodes_12[a].n48;
    while (gPathNodes_12[i].n4c < 0) {
        if (i == b) {
            return acc + d + path_get_distance(gPathNodes_12[a].n48, b);
        }
        i = gPathNodes_12[i].n46;
    }
    d += path_get_distance(gPathNodes_12[a].n48, b);
    w += path_get_distance(gPathNodes_12[a].n46, b);
    if (d < w) {
        return d + acc;
    }
    return w + acc;
}

typedef struct Rec_24 {
    f32 x, y, z, w;
    s32 pad10;
    f32 nx;
    f32 ny;
    u8 pad1[0x46 - 0x1C];
    s16 a;
    s16 b;
    s16 c;
    u8 pad2[0x6C - 0x4C];
} Rec_24;

extern Rec_24 *gPathNodes_13;

void path_calc_normals(void)
{
    s32 i;
    Rec_24 *p;
    Rec_24 *a;
    Rec_24 *c;
    f32 sz;
    f32 sx;
    f32 len;

    for (i = 0; i < gPathNodeCount; i++) {
        p = (Rec_24 *)(i * sizeof(Rec_24) + (u32)gPathNodes_13);
        a = (Rec_24 *)(p->a * sizeof(Rec_24) + (u32)gPathNodes_13);
        c = (Rec_24 *)(p->c * sizeof(Rec_24) + (u32)gPathNodes_13);
        sz = (a->z - p->z) / p->w;
        sz = sz * p->w + ((p->z - c->z) / c->w) * c->w;
        sx = -(a->x - p->x) / p->w;
        sx = sx * p->w + (-(p->x - c->x) / c->w) * c->w;
        len = __builtin_sqrtf(sz * sz + sx * sx);
        if (0.0f < len) {
            gPathNodes_13[i].nx = sz / len;
            gPathNodes_13[i].ny = sx / len;
        }
    }
}

typedef struct Rec_25 {
    f32 f00, f04, f08, f0c, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34, f38, f3c, f40;
    s16 pad44;
    s16 parent;
    s16 child;
    u8 rest[0x6c - 0x4a];
} Rec_25;

void path_calc_spline_coeffs(s32 idx, s32 stop)
{
    Rec_25 *a, *b, *base;
    f32 inv, t, P, Q, R, S, nb, nq, f7, f8, f4, f5;
    s32 par;

    do {
    base = gPathNodes;
    a = (Rec_25 *)(idx * 0x6c + (s32)base);
    inv = a->f0c / 3.0f;
    t = -a->f18;
    S = a->f14;
    par = a->parent;
    b = (Rec_25 *)(par * 0x6c + (s32)base);
    P = a->f00 + t * inv;
    Q = a->f08 + S * inv;
    nb = b->f00 - -b->f18 * inv;
    nq = b->f08 - b->f14 * inv;
    a->f34 = t * 3.0f * inv;
    a->f30 = (nb - P) * 3.0f - a->f34;
    a->f2c = b->f00 - a->f00 - a->f30 - a->f34;
    a->f40 = S * 3.0f * inv;
    a->f3c = (nq - Q) * 3.0f - a->f40;
    a->f38 = b->f08 - a->f08 - a->f3c - a->f40;
    if (a->child >= 0) {
        path_calc_spline_coeffs(a->child, stop);
    }
    idx = par;
    } while (idx != stop);
}

typedef struct Ent_44 {
    f32 f00;
    f32 f04;
    f32 f08;
    f32 f0c;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    f32 f20;
    f32 f24;
    u8 pad28[0x1E];
    s16 next;
    s16 child;
    u8 pad4a[0x22];
} Ent_44;

extern Ent_44 *gPathNodes_14;

void path_calc_slope_and_bank(s32 i, s32 end)
{
    Vec3f a;
    Vec3f b;
    f32 p;
    f32 q;
    f32 fa;
    f32 fb;
    s32 nx;

    do {
        nx = gPathNodes_14[i].next;
        gPathNodes_14[i].f24 = (gPathNodes_14[nx].f04 - gPathNodes_14[i].f04) / gPathNodes_14[i].f0c;
        a = *(Vec3f *)&gPathNodes_14[i].f00;
        b = a;
        p = gPathNodes_14[i].f14 * 0.25f;
        q = -gPathNodes_14[i].f18 * 0.25f;
        a.x = a.x + p;
        a.z = a.z - q;
        b.x = b.x - p;
        b.z = b.z + q;
        fa = bike_get_ground_height((f32 *)&a);
        fb = bike_get_ground_height((f32 *)&b);
        gPathNodes_14[i].f20 = (fa - fb) * 2.0f;
        if (gPathNodes_14[i].child >= 0) {
            path_calc_slope_and_bank(gPathNodes_14[i].child, end);
        }
        i = nx;
    } while (i != end);
}

typedef struct {
    u8 pad[0x1C];
    f32 k;
    u8 pad2[0xC];
    f32 a0, a1, a2;
    f32 b0, b1, b2;
    s16 pad3;
    s16 next;
    s16 pad4;
    s16 link;
    u8 pad5[0x20];
} Node_9;

extern Node_9 *gPathNodes_15;

void path_calc_curvature(s32 start, s32 stop)
{
    s32 p = start;
    f32 v, u, t, x, y, sq, n, n2, r, d, q1, q2, aq;
    Node_9 *m;

    do {
        x = gPathNodes_15[p].a2;
        y = gPathNodes_15[p].b2;
        sq = x * x + y * y;
        t = 2.0f * gPathNodes_15[p].b1;
        u = x * t;
        t = 2.0f * gPathNodes_15[p].a1;
        n = u - y * t;
        r = __builtin_sqrtf(sq);
        d = sq * r;
        m = (Node_9 *)(gPathNodes_15[p].link * sizeof(Node_9) + (u32)gPathNodes_15);
        x = m->a0 * 3.0f + 2.0f * m->a1 + m->a2;
        y = m->b0 * 3.0f + 2.0f * m->b1 + m->b2;
        sq = x * x + y * y;
        q1 = n / d;
        t = 2.0f * m->b1 + m->b0 * 6.0f;
        v = x * t;
        t = 2.0f * m->a1 + m->a0 * 6.0f;
        n2 = v - y * t;
        r = __builtin_sqrtf(sq);
        q2 = n2 / (sq * r);
        aq = q2;
        if (!(q2 > 0.0f)) {
            aq = -q2;
        }
        if (q1 > 0.0f ? q1 < aq : -q1 < aq) {
            gPathNodes_15[p].k = q2;
        } else {
            gPathNodes_15[p].k = q1;
        }
        p = gPathNodes_15[p].next;
    } while (p != stop);
}

typedef struct Ent_45 {
    u8 p0[4];
    f32 f4;
    u8 p8[4];
    f32 fc;
    u8 p10[0x36];
    s16 s46;
    u8 p48[2];
    s16 s4a;
    u8 p4c[0x6c - 0x4c];
} Ent_45;

extern Ent_45 *gPathNodes_16;

u8 path_find_crest_ahead(f32 x, s32 idx)
{
    s32 c;
    s32 A, B;
    f32 d1, d2, v;
    x = x * x;
    for (c = 0; c < 5; c++) {
        A = gPathNodes_16[idx].s46;
        B = gPathNodes_16[idx].s4a;
        d1 = (gPathNodes_16[idx].f4 - gPathNodes_16[A].f4) / gPathNodes_16[A].fc;
        d2 = (gPathNodes_16[B].f4 - gPathNodes_16[idx].f4) / gPathNodes_16[c].fc;
        v = -(((d2 - d1) + (d2 - d1)) / (gPathNodes_16[A].fc + gPathNodes_16[c].fc)) * x;
        if (7.3500004f < v) return c & 0xFF;
        idx = A;
    }
    return 0xFF;
}

typedef struct Rec_26 {
    u8 pad0[0x1C];
    f32 f1c;
    u8 pad20[0x25];
    u8 c45;
    s16 h46;
    s16 h48;
    u8 pad4a[0x6C - 0x4A];
} Rec_26;

extern Rec_26 *gPathNodes_17;

void path_calc_curvatures(void)
{
    s32 i, j, cnt;
    f32 rem;
    Rec_26 *base;

    for (i = 0; i < gPathNodeCount; i++) {
        path_calc_curvature(i, gPathNodes_17[i].h46);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        cnt = 0;
        j = i;
        rem = 0.06f;
        base = gPathNodes_17;
        while (rem > 0.0f) {
            if (base[j].h48 != -1) {
                cnt++;
                break;
            }
            rem -= ((base[j].f1c > 0.0f) ? base[j].f1c : -base[j].f1c) + 0.006f;
            j = base[j].h46;
            cnt++;
        }
        gPathNodes_17[i].c45 = cnt - 1;
    }
}

typedef struct {
    Vec3f pos;
    f32 t;
    f32 f10;
    f32 f14;
    u8 pad18[0x46 - 0x18];
    s16 next;
    s16 f48;
    s16 prev;
    s16 f4c;
    s16 f4e;
    u8 pad50[0x58 - 0x50];
    f32 f58;
    f32 f5c;
    u8 pad60[0x65 - 0x60];
    u8 bits;
    u8 pad66[0x6C - 0x66];
} Node_10;

extern Node_10 *gPathNodes_18;

void path_generate_straight(void)
{
    s32 i, s, a1;
    f32 R, t;
    Vec3f v, w, m;

    gPathNodes_18 = mem_alloc_dbg(540000, 0, sStrPathFile, 0x178);
    mem_fill((u8 *)gPathNodes_18, 0, 540000);
    R = 30.0f;
    m.x = 0.0f;
    m.y = 0.0f;
    m.z = 0.0f;
    for (i = 0; i < 1000; i++) {
        gPathNodes_18[i].prev = i - 1;
        gPathNodes_18[i].next = i + 1;
        gPathNodes_18[i].f4e = -1;
        gPathNodes_18[i].f4c = -1;
        gPathNodes_18[i].f48 = -1;
        t = 3.0f;
        do {
            t -= 1.0f;
            v.x = 0.0f;
            v.y = 40.0f;
            v.z = R - t;
            v.y = bike_get_ground_height((float *)&v);
            if (gGroundFound == 0) {
                break;
            }
            height_get_ground_normal((float *)&w);
        } while (w.z * m.z < -0.01f && 1.1f < t);
        m = w;
        gPathNodes_18[i].pos = v;
        gPathNodes_18[i].t = t;
        gPathNodes_18[i].bits = 0;
        for (s = 3; s >= 0; s--) {
            v.x = s * 2.178f;
            bike_get_ground_height((float *)&v);
            switch ((gGroundSurface >> 4) & 0xF) {
            case 2:
            case 3:
                a1 = 1;
                break;
            case 8:
                a1 = 2;
                break;
            case 7:
                a1 = 3;
                break;
            default:
                a1 = 0;
                break;
            }
            gPathNodes_18[i].bits |= a1 << (s * 2);
        }
        R -= t;
        gPathNodes_18[i].f10 = 50.0f;
        gPathNodes_18[i].f58 = 7.284f;
        gPathNodes_18[i].f5c = 0.75f;
        if (gGroundFound == 0) {
            break;
        }
    }
    gPathNodes_18[0].prev = i;
    gPathNodes_18[i].next = 0;
    gPathNodeCount = i + 1;
    gPathStartNode = 1;
    gPathFinishNode = 0;
    path_calc_normals();
    path_calc_spline_coeffs(0, 0);
    path_calc_slope_and_bank(0, 0);
    path_calc_curvatures();
    for (i = 0; i < gPathNodeCount; i++) {
        gPathNodes_18[i].f10 = 50.0f;
        gPathNodes_18[i].f14 = -1.0f;
    }
}

typedef struct {
    f32 x, y, z;      /* 0x00 */
    f32 dist;         /* 0x0C */
    f32 f10;          /* 0x10 */
    u8 pad14[0x30];
    u8 f44;           /* 0x44 */
    u8 pad45;
    s16 next;         /* 0x46 */
    s16 f48;
    s16 f4A;
    s16 f4C;
    s16 f4E;
    s16 pad50;
    s16 f52;
    s16 f54;
    s16 f56;
    f32 f58;
    f32 f5C;
    s32 f60;
    u8 f64;
    u8 f65;
    u16 f66;
    f32 f68;
} Node_11;

typedef struct {
    s32 x;      /* 0x00 */
    s32 z;      /* 0x04 */
    u16 f10;    /* 0x08 */
    u16 f68;    /* 0x0A */
    s16 next;   /* 0x0C */
    s16 f48;    /* 0x0E */
    s16 f4A;    /* 0x10 */
    s16 f4C;    /* 0x12 */
    s16 f4E;    /* 0x14 */
    s16 pad16;
    u8 f44;     /* 0x18 */
    u8 f65;     /* 0x19 */
    s16 f52;    /* 0x1A */
    s16 f54;    /* 0x1C */
    s16 f56;    /* 0x1E */
    s16 f58;    /* 0x20 */
    s16 f5C;    /* 0x22 */
    s32 f60;    /* 0x24 */
} Rec_27;

extern Node_11 *gPathNodes_19;

void path_load(u32 *p)
{
    u32 w;
    u32 flags;
    s32 stride;
    s32 i;
    Rec_27 *r;
    u8 *e;
    f32 v;
    f32 dx, dz;

    w = *p;
    stride = 0x20;
    if ((w >> 24) == 0xAA) {
        flags = w;
        flags &= 0xFFFFFF;
        p++;
        if (w & 1) stride = 0x28;
        if (w & 2) stride += 1;
        if (w & 4) stride += 2;
        if (w & 8) stride += 2;
        stride = (stride + 3) & ~3;
    }
    gPathNodeCount = *p;
    if (flags & 0x20) {
        gPathStartNode = p[1];
        gPathFinishNode = p[2];
        p += 2;
    } else {
        gPathFinishNode = 0;
        gPathStartNode = 0;
    }
    i = 0;
    gPathNodes_19 = mem_alloc_dbg(gPathNodeCount * sizeof(Node_11), 0, (s32)sStrPathFile, 0x20C);
    mem_fill((u8 *)gPathNodes_19, 0, gPathNodeCount * sizeof(Node_11));
    r = (Rec_27 *)(p + 1);
    for (; i < gPathNodeCount; i++) {
        mem_fill((u8 *)&gPathNodes_19[i], 0, sizeof(Node_11));
        gPathNodes_19[i].f4E = r->f4E;
        gPathNodes_19[i].f4A = r->f4A;
        gPathNodes_19[i].f4C = r->f4C;
        gPathNodes_19[i].next = r->next;
        gPathNodes_19[i].f48 = r->f48;
        gPathNodes_19[i].x = (f32)r->x / 1000.0f;
        gPathNodes_19[i].z = (f32)r->z / 1000.0f;
        if (gPathNodes_19[i].f4A < i) {
            gPathNodes_19[i].y = gPathNodes_19[gPathNodes_19[i].f4A].y + 3.0f;
            if (gCurTrack == D_800CB3F0) {
                gPathNodes_19[i].y = gPathNodes_19[i].y + 4.0f;
            }
        } else {
            gPathNodes_19[i].y = 0.0f;
        }
        gPathNodes_19[i].y = bike_get_ground_height((f32 *)&gPathNodes_19[i]);
        gPathNodes_19[i].f10 = r->f10;
        gPathNodes_19[i].f44 = r->f44;
        gPathNodes_19[i].f65 = r->f65;
        gPathNodes_19[i].f56 = r->f56;
        gPathNodes_19[i].f52 = r->f52;
        gPathNodes_19[i].f54 = r->f54;
        gPathNodes_19[i].f68 = r->f68 / 1000.0f;
        if (gPathNodes_19[i].f10 < gPathNodes_19[i].f68) {
            gPathNodes_19[i].f68 = 0.0f;
        }
        e = (u8 *)r + 0x20;
        if (flags & 1) {
            gPathNodes_19[i].f58 = (f32)r->f58 / 62.5f - 1.0f;
            if (gPathNodes_19[i].f58 < 0.0f) gPathNodes_19[i].f58 = 0.0f;
            gPathNodes_19[i].f5C = (f32)r->f5C / 62.5f - 1.0f;
            if (gPathNodes_19[i].f5C < 0.0f) gPathNodes_19[i].f5C = 0.0f;
            e = (u8 *)r + 0x28;
            gPathNodes_19[i].f60 = r->f60;
        }
        if (flags & 0x10) {
            if (flags & 4) {
                gPathNodes_19[i].f66 = *e++ << 8;
                gPathNodes_19[i].f66 |= *e++;
            }
            if (flags & 8) e += 2;
            if (flags & 2) gPathNodes_19[i].f64 = *e;
        } else {
            if (flags & 2) gPathNodes_19[i].f64 = *e++;
            if (flags & 4) {
                gPathNodes_19[i].f66 = *e++ << 8;
                gPathNodes_19[i].f66 |= *e++;
            }
        }
        r = (Rec_27 *)((u8 *)r + stride);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        flags = gPathNodes_19[i].next;
        dx = gPathNodes_19[flags].x - gPathNodes_19[i].x;
        dz = gPathNodes_19[flags].z - gPathNodes_19[i].z;
        gPathNodes_19[i].dist = __builtin_sqrtf(dx * dx + dz * dz);
    }
    path_calc_normals();
    path_calc_spline_coeffs(0, 0);
    path_calc_slope_and_bank(0, 0);
    path_calc_curvatures();
    gPathLapLength = path_get_distance(gPathFinishNode, gPathNodes_19[gPathFinishNode].f4A)
               + gPathNodes_19[gPathNodes_19[gPathFinishNode].f4A].dist;
    path_calc_corner_speeds(0, gPathNodeCount - 1);
    path_apply_braking_limits(0, gPathNodes_19[0].f4A);
    for (i = 0; i < gPathNodeCount; i++) {
    }
}

typedef struct {
    u8 pad0[0x10];
    f32 f10;
    u8 pad1[0x8];
    f32 f1c;
    f32 f20;
    u8 pad2[0x40];
    u8 b64;
    s8 b65;
    u8 rest[6];
} Ent_46;

extern Ent_46 *gPathNodes_20;

void path_calc_corner_speed(s32 idx)
{
    f32 f;
    f32 w;
    f32 v;

    f = gPathNodes_20[idx].f1c;
    if ((f > 0.0f) ? (f > 0.005f) : (-f > 0.005f)) {
        w = gPathNodes_20[idx].f1c;
        if (w > 0.0f) {
            v = gPathNodes_20[idx].f20 * 14.700001f + 19.11f;
            w = v / w;
        } else {
            v = 19.11f - gPathNodes_20[idx].f20 * 14.700001f;
            v = -v;
            w = v / w;
        }
        if (w < 1.0f) {
            w = 1.0f;
        }
        gPathNodes_20[idx].f10 = __builtin_sqrtf(w);
    } else {
        gPathNodes_20[idx].f10 = 50.0f;
    }
    if (gPathNodes_20[idx].b64 & 4) {
        gPathNodes_20[idx].f10 += gPathNodes_20[idx].f10;
    }
    gPathNodes_20[idx].f10 *= gPathNodes_20[idx].b65 / 10.0f + 1.0f;
}

#define sqrtf __builtin_sqrtf

typedef struct Node_12 {
    u8 pad0[0xc];
    f32 f0c;
    f32 f10;
    u8 pad14[0x8];
    f32 f1c;
    f32 f20;
    f32 f24;
    u8 pad28[0x20];
    s16 s48;
    s16 s4a;
    s16 s4c;
    u8 pad4e[0x6c - 0x4e];
} Node_12;

extern Node_12 *gPathNodes_21;

void path_apply_braking_limits(s32 end, s32 i)
{
    f32 v;
    f32 t;
    f32 q;
    f32 sm;
    f32 r;
    f32 a;
    s32 nx;
    s32 c;
    s32 p;

    for (; end != i; i = gPathNodes_21[i].s4a) {
        if (i == 0) {
            return;
        }
        v = gPathNodes_21[i].f10;
        t = v * v * gPathNodes_21[i].f1c - gPathNodes_21[i].f20 * 14.700001f;
        t = t * t;
        nx = gPathNodes_21[i].s4a;
        a = gPathNodes_21[i].f0c;
        if (t > 365.39212f) {
            q = 0.0f;
        } else {
            q = sqrtf(365.39212f - t);
        }
        if (q > 10.29f) {
            q = 10.29f;
        }
        q = q + gPathNodes_21[i].f24 * 14.700001f;
        sm = (q + q) * a + v * v;
        if (sm < 1.0f) {
            sm = 1.0f;
        }
        sm = sqrtf(sm);
        if (sm < gPathNodes_21[nx].f10) {
            gPathNodes_21[nx].f10 = sm;
        }
        p = gPathNodes_21[i].s4c;
        if (p >= 0) {
            while (c = gPathNodes_21[p].s4a, gPathNodes_21[c].s48 == -1) {
                p = c;
            }
            path_apply_braking_limits(p, gPathNodes_21[i].s4c);
        }
    }
}

#define ND gPathNodes_22[0]
#define ND2 (*(Node_13 **)(u32)gPathNodes_22)

typedef struct {
    Vec3f pos;      /* 0x00 */
    f32 f0C;        /* 0x0C */
    f32 f10;        /* 0x10 */
    u8 pad14[0x18];
    f32 c2C;        /* 0x2C */
    f32 c30;
    f32 c34;
    f32 c38;
    f32 c3C;
    f32 c40;        /* 0x40 */
    u8 pad44[2];
    s16 prev;       /* 0x46 */
    u8 pad48[2];
    s16 next;       /* 0x4A */
    u8 pad4C[0xC];
    f32 f58;
    f32 f5C;
    u8 pad60[8];
    f32 f68;
} Node_13;

extern Node_13 *gPathNodes_22[];

void path_limit_jump_speeds(void)
{
    Vec3f pts[20];
    Vec3f pos;
    s32 cur, prv, nx, j, idx, i;
    u8 flag;
    Vec3f *pa, *pb;
    Node_13 *e;
    f32 w;
    f32 t, t2, d, s, x, h, v;

    cur = 0;
    while (cur != ND[0].next) {
        j = ND[cur].next;
        prv = ND[cur].prev;
        {
        f32 bq = (ND[cur].pos.y - ND[j].pos.y) / ND[j].f0C;
        if (7.3500004f < -(((ND[prv].pos.y - ND[cur].pos.y) / ND[cur].f0C - bq) * 2.0f / (ND[j].f0C + ND[cur].f0C)) * (ND[cur].f68 * ND[cur].f68)) {
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i] = ND[j].pos;
                e = ND;
                pts[i].x += e[j].c34 * t + e[j].c30 * (t * t) + e[j].c2C * (t * t * t);
                pts[i].z += e[j].c40 * t + e[j].c3C * (t * t) + e[j].c38 * (t * t * t);
                pts[i].y = bike_get_ground_height(&pts[i]);
            }
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i + 10] = ND[cur].pos;
                e = ND;
                pts[i + 10].x += e[cur].c34 * t + e[cur].c30 * (t * t) + e[cur].c2C * (t * t * t);
                pts[i + 10].z += e[cur].c40 * t + e[cur].c3C * (t * t) + e[cur].c38 * (t * t * t);
                pts[i + 10].y = bike_get_ground_height(&pts[i + 10]);
            }
            for (i = 0; i < 18; i++) {
                if (pts[i].y < pts[i + 1].y && pts[i + 2].y < pts[i + 1].y) {
                    d = __builtin_sqrtf((pts[i + 1].x - pts[i].x) * (pts[i + 1].x - pts[i].x)
                                        + (pts[i + 1].z - pts[i].z) * (pts[i + 1].z - pts[i].z));
                    s = (pts[i + 1].y - pts[i].y) / d;
                    if (s > 1.0f) {
                        s = 1.0f;
                    }
                    flag = 0;
                    pb = &pts[i + 1];
                    pa = &pts[i];
                    while (!((x = ND2[cur].f10 * ND2[cur].f10 * 2.0f * s / 14.700001f) < 10.0f)) {
                        pos.x = pb->x + x * (pb->x - pa->x) / d;
                        pos.z = pb->z + x * (pb->z - pa->z) / d;
                        idx = path_find_segment(&pos, cur, 20, 2);
                        h = path_get_lateral_offset(&pos, idx) * 1000.0f;
                        if (-h < ND2[idx].f5C && h < ND2[idx].f58 && ND2[cur].f10 <= ND2[idx].f10) {
                            break;
                        }
                        w = ND2[cur].f10 * 0.95f;
                        ND2[cur].f10 = w;
                        flag = 1;
                    }
                    if (flag) {
                        j = cur;
                        do {
                            s = ND2[ND2[j].next].f10;
                            path_apply_braking_limits(ND2[j].next, j);
                            nx = ND2[j].next;
                            j = nx;
                        } while (ND2[nx].f10 < s);
                    }
                }
            }
        }
        }
        cur = prv;
    }
}

typedef struct {
    V3 pos;
    f32 f0C;
    u8 pad10[4];
    f32 f14;
    f32 f18;
    u8 pad1C[0x46 - 0x1C];
    s16 next;
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x66 - 0x60];
    u16 h66;
    u8 pad68[0x6C - 0x68];
} Rec_28;

extern Rec_28 *gPathNodes_23;

void path_calc_widths_and_progress(void)
{
    s32 i;
    V3 v;
    f32 st[4];
    f32 l[2];
    f32 acc;
    Rec_28 *r;

    for (i = 0; i < gPathNodeCount; i++) {
        height_gather_walls((s32)((gPathNodes_23[i].pos.x - 8.0f) * 1000.0f),
                      (s32)((gPathNodes_23[i].pos.z - 8.0f) * 1000.0f),
                      (s32)((gPathNodes_23[i].pos.x + 8.0f) * 1000.0f),
                      (s32)((gPathNodes_23[i].pos.z + 8.0f) * 1000.0f));
        v = gPathNodes_23[i].pos;
        v.x -= gPathNodes_23[i].f14 * 8.0f;
        v.z -= gPathNodes_23[i].f18 * 8.0f;
        l[0] = 1.0f;
        height_raycast_walls(&gPathNodes_23[i], &v, st, l, 0);
        gPathNodes_23[i].f58 = l[0] * 8.0f;
        v = gPathNodes_23[i].pos;
        v.x += gPathNodes_23[i].f14 * 8.0f;
        v.z += gPathNodes_23[i].f18 * 8.0f;
        l[0] = 1.0f;
        height_raycast_walls(&gPathNodes_23[i], &v, st, l, 0);
        gPathNodes_23[i].f5C = l[0] * 8.0f;
    }

    acc = 0.0f;
    i = 1;
    if (gPathNodes_23[i].next != 1) {
        r = gPathNodes_23;
        do {
            r[i].h66 = ((gPathLapLength - acc) * 32768.0f) / gPathLapLength;
            acc += r[i].f0C;
            i = r[i].next;
        } while (r[i].next != 1);
    }
}

void path_init(u32 *param_1)
{
    s32 iVar1;
    u32 uVar2;

    if (gGame.mode == 6) {
        path_generate_straight();
        goto end;
    }
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0) {
        sPathRawData = gUserTrackPath;
        path_load(gUserTrackPath);
        path_calc_widths_and_progress();
        path_limit_jump_speeds();
        gPathFinishNode = 1;
        gPathStartNode = 1;
        mem_free((s32)sPathRawData);
        goto end;
    }
    if (*(s32 *)(gCurTrack + 0x2c) != 2) {
        if (gGame.mode == 9 && (gTutorialLessons[gSeason->round * 40 + 0xC] & 2)) {
            iVar1 = D_800D10F0;
            goto L134;
        }
        if (*(volatile s32 *)&gGame.mode == 7 && (gCheatFlags & 8)) {
            iVar1 = D_800D0D50;
            goto L134;
        }
        iVar1 = track_res_find(D_800D14F8);
L134:
        res_set_load_buffer(gZBuffer);
        uVar2 = res_load(param_1, iVar1);
        sPathRawData = (u32 *)(uVar2 & 0xfffffffc);
        path_load(sPathRawData);
        res_unload((u32 *)(param_1[1] + iVar1 * 4));
        res_set_load_buffer(0);
    }
end:
    return;
}

typedef struct {
    Vec3f pos;
    u8 unkC[0x46 - 0xC];
    s16 next;
    s16 branch;
    s16 prev;
    s16 unk4C;
    s16 alt;
    u8 unk50[0x6C - 0x50];
} Seg_9;

extern Seg_9 *gPathNodes_24;

s32 path_find_nearest_node(Vec3f *pos, s32 start, s32 count, u8 mode) {
    f32 best = 100000000.0f;
    s32 bestIdx;
    s32 seg;
    s32 i;
    f32 dx;
    f32 dz;
    f32 d;

    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes_24[seg].pos.x;
        dz = pos->z - gPathNodes_24[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        if (gPathNodes_24[seg].branch >= 0 && mode == 2) {
            seg = gPathNodes_24[seg].branch;
        } else {
            seg = gPathNodes_24[seg].next;
        }
    }
    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes_24[seg].pos.x;
        dz = pos->z - gPathNodes_24[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        seg = gPathNodes_24[seg].prev;
    }
    if (mode == 0) {
        seg = gPathNodes_24[bestIdx].alt;
        if (seg >= 0) {
            dx = pos->x - gPathNodes_24[seg].pos.x;
            dz = pos->z - gPathNodes_24[seg].pos.z;
            d = dx * dx + dz * dz;
            if (d < best) {
                bestIdx = seg;
            }
        }
    }
    return bestIdx;
}

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 pad0C;
    f32 pad10;
    f32 nz;
    f32 nx;
    u8 pad1C[0x10];
    f32 cx[3];
    f32 cz[3];
    u8 pad44[2];
    s16 link;
    u8 pad48[0x24];
} Node_14;

f32 path_get_lateral_offset(Vec3f *pos, s32 idx)
{
    Node_14 *b;
    Node_14 *a;
    Node_14 *base;
    f32 dx, dz;
    f32 dA, dB, t, t2, t3;
    f32 sx, sz, r;

    base = gPathNodes;
    b = (Node_14 *)(idx * sizeof(Node_14) + (u32)base);
    a = (Node_14 *)(b->link * sizeof(Node_14) + (u32)base);
    dz = a->z - pos->z;
    dx = a->x - pos->x;
    dA = dz * a->nz - dx * a->nx;
    dz = b->z - pos->z;
    dx = b->x - pos->x;
    dB = dz * b->nz - dx * b->nx;
    t = dB / (dB - dA);
    t2 = t * t;
    t3 = t2 * t;
    dx += t3 * b->cx[0] + t2 * b->cx[1] + t * b->cx[2];
    dz += t3 * b->cz[0] + t2 * b->cz[1] + t * b->cz[2];
    sx = t2 * 3.0f * b->cx[0] + (t + t) * b->cx[1] + b->cx[2];
    sz = t2 * 3.0f * b->cz[0] + (t + t) * b->cz[1] + b->cz[2];
    dB = __builtin_sqrtf(sx * sx + sz * sz);
    gPathSegmentT = t;
    gPathTangentX = sx / dB;
    gPathTangentZ = sz / dB;
    return (sx * dz - sz * dx) / dB;
}

typedef struct Seg_10 {
    f32 x;        /* 0x00 */
    f32 y;        /* 0x04 */
    f32 z;        /* 0x08 */
    f32 f0c;
    f32 f10;
    f32 dx;       /* 0x14 */
    f32 dz;       /* 0x18 */
    u8 pad1c[0x46 - 0x1c];
    s16 h46;      /* 0x46 */
    s16 h48;
    s16 h4a;      /* 0x4a */
    u8 pad4c[0x6c - 0x4c];
} Seg_10;

void path_get_segment_and_t(Vec3f *pos, s32 *area, f32 *t, s32 p4)
{
    s32 i;
    Seg_10 *s;
    Seg_10 *tab;
    Seg_10 *a;
    Seg_10 *b;
    f32 fa;
    f32 fb;
    f32 r;
    f32 d, m1, m2, m3, m4;

    i = path_find_nearest_node(pos, p4, 4, 1);
    tab = gPathNodes;
    s = (Seg_10 *)(i * sizeof(Seg_10) + (u32)tab);
    if ((pos->z - s->z) * s->dx - (pos->x - s->x) * s->dz < 0.0f) {
        i = s->h4a;
    }
    *area = i;
    a = (Seg_10 *)(i * sizeof(Seg_10) + (u32)gPathNodes);
    b = (Seg_10 *)(a->h46 * sizeof(Seg_10) + (u32)gPathNodes);
    d = b->z - pos->z;
    m1 = d * b->dx;
    d = b->x - pos->x;
    m2 = d * b->dz;
    d = a->z - pos->z;
    m3 = d * a->dx;
    d = a->x - pos->x;
    m4 = d * a->dz;
    fb = m1 - m2;
    fa = m3 - m4;
    r = fa / (fa - fb);
    *t = r;
    if (r < 0.0f) {
        *t = 0.0f;
    }
    if (1.0f < *t) {
        *t = 1.0f;
    }
}

typedef struct {
    u8 pad0[0x28];
    u32 flags;
} Part_4;

typedef struct {
    f32 x, y, z;      /* 0x00 */
    u8 pad0C[0x8];
    f32 f14;          /* 0x14 */
    f32 f18;          /* 0x18 */
    u8 pad1C[0x28];
    s8 f44;           /* 0x44 */
    u8 pad45;
    s16 f46;          /* 0x46 */
    u8 pad48[2];
    s16 f4A;          /* 0x4A */
    u8 pad4C[0xC];
    f32 f58;          /* 0x58 */
    f32 f5C;          /* 0x5C */
    u8 pad60[6];
    u16 f66;          /* 0x66 */
    u8 pad68[4];
} Seg_11;

struct Bike_28 {
    s32 x, y, z;      /* 0x000 */
    u8 pad00C[0x38];
    u8 f44;           /* 0x044 */
    u8 pad045[3];
    Vec3f pos;        /* 0x048 */
    f32 f54, f58, f5C;/* 0x054 */
    u8 pad060[0x58];
    f32 fB8;          /* 0x0B8 */
    f32 fBC;
    f32 fC0;          /* 0x0C0 */
    u8 pad0C4[0x30C];
    s8 f3D0;          /* 0x3D0 */
    u8 pad3D1[0x2E];
    u8 f3FF;          /* 0x3FF */
    u8 pad400[0x18];
    s8 f418;          /* 0x418 */
    u8 pad419[0xB];
    f32 f424;         /* 0x424 */
    u8 pad428[0x30];
    s32 f458;         /* 0x458 */
    u8 pad45C[0x88];
    Part_4 *f4E4;       /* 0x4E4 */
    u8 pad4E8[0x14];
    s32 f4FC;         /* 0x4FC */
    u8 pad500[0x88];
    Part_4 *f588;       /* 0x588 */
    u8 pad58C[0x98];
    s16 f624;         /* 0x624 */
    s16 f626;         /* 0x626 */
    s16 seg;          /* 0x628 */
    s16 pad62A;
    f32 f62C;         /* 0x62C */
    f32 f630;         /* 0x630 */
    f32 f634;         /* 0x634 */
    f32 f638;         /* 0x638 */
    f32 f63C;         /* 0x63C */
    u8 pad640[8];
    s32 f648;         /* 0x648 */
    u8 pad64C[0x14];
    s16 f660;         /* 0x660 */
    s16 f662;         /* 0x662 */
    u8 pad664[4];
    s16 f668;         /* 0x668 */
    s16 f66A;         /* 0x66A */
    u8 pad66C[0x20];
    s32 f68C;         /* 0x68C */
    u8 pad690[0x22];
    s16 f6B2;         /* 0x6B2 */
    u8 pad6B4[0x34];
    u8 f6E8;          /* 0x6E8 */
    u8 pad6E9[2];
    u8 f6EB;          /* 0x6EB */
    u8 pad6EC;
    s8 f6ED;          /* 0x6ED */
    u8 f6EE;          /* 0x6EE */
    u8 pad6EF[0x55];
    s32 f744;         /* 0x744 */
    u8 pad748[0x64];
    s16 f7AC;         /* 0x7AC */
};

typedef struct {
    s16 id;
    s16 arg;
    s32 (*fn)(Bike_28 *, s32);
} Trig;

typedef struct {
    s16 a, b, c, d;
} Ev;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x54];
    Trig *f84;
    Ev ev[3];
} Track_10;

extern Track_10 *gCurTrack_26;
extern Seg_11 *gPathNodes_25;

void path_update_bike(Bike_28 *b)
{
    s32 oldSeg;
    s32 n;
    s32 i;
    Seg_11 *s;
    Trig *t;
    Ev *e;
    s32 w;
    s32 nx;
    f32 v;
    f32 old;
    f32 sq;
    s32 k;
    s32 cur;

    oldSeg = b->seg;
    if ((b->f4E4->flags & 0x80) && b->f458 <= 0) {
        bike_crash_begin(b, 0x19);
    }
    if ((b->f588->flags & 0x80) && b->f4FC <= 0) {
        bike_crash_begin(b, 0x1C);
    }
    if (gCurTrack_26->f2C == 1) {
        height_get_ground(b->x, b->y, b->z);
        if (gGroundFound == 0) {
            bike_respawn(b);
        }
    }
    if (b->f458 <= 0 && b->f4FC <= 0 && b->f44 == 0) {
        if (!(b->f4E4->flags & 0x100) && !(b->f588->flags & 0x100)) {
            b->f626 = oldSeg;
            b->f624 = 0;
        } else if (gGame.mode != 0xE && (b->f4E4->flags & 0x100) && (b->f588->flags & 0x100)) {
            if (++b->f624 > 10) {
                bike_respawn(b);
            }
        }
    }
    if (gEvent.state < 5 && gGame.mode != 9) {
        gPathSegmentT = 0.0f;
        b->seg = D_800EE496;
    } else {
        Vec3f *pos = &b->pos;
        n = path_find_nearest_node(pos, oldSeg, 2, b->f6EB);
        if ((pos->z - gPathNodes_25[n].z) * gPathNodes_25[n].f14 - (b->pos.x - gPathNodes_25[n].x) * gPathNodes_25[n].f18 < 0.0f) {
            n = gPathNodes_25[n].f4A;
        }
        b->seg = n;
        if ((s16)n != oldSeg) {
            if (b->f6E8) {
                t = gCurTrack_26->f84;
                if (t != NULL) {
                    while (t->id >= 0) {
                        if (t->id != 0 && b->seg == t->id) {
                            b->f6EB = t->fn(b, t->arg) + 1;
                        }
                        t++;
                    }
                } else {
                    b->f6EB = 3;
                }
            }
            for (i = 0; i < 3; i++) {
                if (gCurTrack_26->ev[i].a == -1) {
                    break;
                }
                if (gCurTrack_26->ev[i].a == b->f660 + b->f6ED
                    && (gCurTrack_26->ev[i].b == b->seg || gCurTrack_26->ev[i].c == b->seg)) {
                    remove_walls_trigger(gCurTrack_26->ev[i].d);
                }
            }
            if (gGame.mode == 7) {
                if (b->seg == gPathNodes_25->f4A) {
                    b->seg = 0;
                }
                n = b->seg;
                if (gPathNodes_25[n].f44 == 0 && b->f3FF == 0 && b->f44 == 0 && b->f6B2 < n) {
                    if (n < 0x78) {
                        b->f648 = 0;
                        b->f66A = 0x78;
                        b->f6B2 = b->seg;
                    } else {
                        b->f3FF = 1;
                        b->f662 = 0;
                        b->f68C = gEvent.time;
                        event_check_record_time(b);
                        b->f6E8 = 1;
                    }
                }
            }
            path_check_lap(b, oldSeg);
        }
        b->f638 = path_get_lateral_offset(&b->pos, b->seg);
        b->f63C = gPathSegmentT;
        if (b->f6E8 == 0) {
            b->f6EE = b->fB8 * gPathTangentX + b->fC0 * gPathTangentZ < 0.0f && b->f424 > -0.5f && b->f3D0 != 0xF;
        }
    }
    if (b->f44 == 0) {
        k = b->seg;
        if (b->f63C > 0.5f) {
            k = gPathNodes_25[k].f46;
        }
        if (gCurTrack_26->f2C == 0
            && (gPathNodes_25[k].f5C < b->f638 - 3.5f || gPathNodes_25[k].f58 < -(b->f638 + 3.5f))) {
            if (++b->f7AC > 180 && b->f458 <= 0 && b->f4FC <= 0) {
                bike_respawn(b);
            }
        } else {
            b->f626 = oldSeg;
            b->f7AC = 0;
        }
    }
    {
        w = gPathSegmentT * 256.0f;
        k = gPathNodes_25[b->seg].f46;
        cur = gPathNodes_25[b->seg].f66;
        nx = gPathNodes_25[k].f66;
    }
    if (nx == 0x8000) {
        nx = 0;
    }
    v = (f32)(s32)(cur * (0x100 - w) + nx * w) * gPathLapLength * 1.1920929e-07f;
    sq = b->f54 * b->f54 + b->f5C * b->f5C;
    old = b->f62C;
    b->f62C = v;
    b->f634 = (v - old) / gPhysDeltaTime;
    if (sq > 1.0f && b->f744 < -15) {
        sq = __builtin_sqrtf(sq);
        if (b->f54 * gPathTangentX + b->f5C * gPathTangentZ / sq > 0.7f) {
            b->f418 = path_find_crest_ahead(sq, b->seg);
        }
    } else {
        b->f418 = -1;
    }
    if (b->f668 == -1) {
        b->f630 = gPathLapLength * (f32)(b->f6ED + 1 + b->f660) - b->f62C;
    }
    if (b->f6E8 == 0 && b->seg >= 0x13A && b->seg < 0x140 && b->f744 > 30
        && gCurTrack_26 == &D_800CB5D0 && b->f58 < 0.0f && b->pos.y < gPathNodes_25[0x140].y) {
        bike_crash_begin(b, 0x1C);
        cam_start_fall_view(b);
    }
}

#define SEG(i) ((Seg_12 *)((s32)((i) * 0x6C) + (s32)gPathNodes))

typedef struct {
    u8 pad[0x46];
    s16 f46;
    u8 p2[2];
    s16 f4A;
    u8 p3[0x6C - 0x4C];
} Seg_12;

struct Ent_47 {
    u8 pad[0x628];
    s16 f628;
    u8 p1[0x6ED - 0x62A];
    u8 f6ED;
    u8 p2[0x7E0 - 0x6EE];
};

extern Ent_47 gRiders_51[];

void path_check_lap(Ent_47 *p, s32 a1)
{
    s32 h;
    s32 cur;
    s32 iv;
    s32 v;

    if (a1 == gPathFinishNode) {
        if (p->f628 == SEG(a1)->f4A) {
            p->f6ED--;
        }
    }
    h = p->f628;
    cur = gPathFinishNode;
    if (h == cur || h == SEG(cur)->f46) {
        iv = SEG(cur)->f4A;
        if (a1 == iv || a1 == SEG(iv)->f4A) {
            if ((s8)p->f6ED == 0 && (gGame.flags & 1) == 0) {
                v = p - gRiders_51;
                if (v < gEvent.numRiders) {
                    event_rider_lap_completed(v);
                    return;
                }
            }
            p->f6ED++;
        }
    }
}

s32 path_find_segment(Vec3f *pos, s32 start, s32 count, u8 mode)
{
    s32 idx;
    f32 *e;
    u8 *base;

    idx = path_find_nearest_node(pos, start, count, mode);
    base = gPathNodes;
    e = (f32 *)((u32)(idx * 0x6c) + (u32)base);
    if ((pos->z - e[2]) * e[5] - (pos->x - e[0]) * e[6] < 0.0f) {
        idx = *(s16 *)((u8 *)e + 0x4a);
    }
    return idx;
}

void path_calc_corner_speeds(s32 param_1, s32 param_2)
{
    s32 i;
    for (i = 0; i <= param_2; i++) {
        path_calc_corner_speed(i);
    }
}

typedef struct {
    char pad[0x2C];
    s32 f2C;
} Obj_36;

extern Obj_36 *gCurTrack_27;

void path_free(void)
{
    if (gCurTrack_27->f2C != 2) {
        mem_free(gPathNodes);
    }
}
