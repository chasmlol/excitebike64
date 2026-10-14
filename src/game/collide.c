#include "game.h"

Node_16 *collide_build_sphere_tree(Param *p)
{
    Node_16 *n = sCollideNodeNext;
    Node_16 *a;
    Node_16 *b;
    f32 pa[3];
    f32 pb[3];
    f32 d[3];
    f32 len;

    sCollideNodeNext = n + 1;
    sCollideNodeCount = sCollideNodeCount + 1;
    if (p->kid == 0) {
        n->x = p->x;
        n->y = p->y - 0.5f;
        n->z = p->z;
        n->r = p->r;
    } else {
        a = collide_build_sphere_tree(p + p->kid);
        n->c2 = a;
        b = a->c1;
        d[0] = b->x - a->x;
        d[1] = b->y - a->y;
        d[2] = b->z - a->z;
        len = __builtin_sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        if (len != 0.0f) {
            d[0] = d[0] / len;
            d[1] = d[1] / len;
            d[2] = d[2] / len;
        }
        pa[0] = d[0] * -a->r;
        pa[1] = d[1] * -a->r;
        pa[2] = d[2] * -a->r;
        pa[0] = pa[0] + a->x;
        pa[1] = pa[1] + a->y;
        pa[2] = pa[2] + a->z;
        pb[0] = d[0] * b->r;
        pb[1] = d[1] * b->r;
        pb[2] = d[2] * b->r;
        pb[0] = pb[0] + b->x;
        pb[1] = pb[1] + b->y;
        pb[2] = pb[2] + b->z;
        n->x = (pa[0] + pb[0]) * 0.5f;
        n->y = (pa[1] + pb[1]) * 0.5f;
        n->z = (pa[2] + pb[2]) * 0.5f;
        pa[0] = pa[0] - pb[0];
        pa[1] = pa[1] - pb[1];
        pa[2] = pa[2] - pb[2];
        n->r = __builtin_sqrtf(pa[0] * pa[0] + pa[1] * pa[1] + pa[2] * pa[2]) * 0.5f;
    }
    if (p->kid2 != 0) {
        n->c1 = collide_build_sphere_tree(p + p->kid2);
    }
    p->r = n->r;
    return n;
}

s32 collide_spheres_overlap(Bv *a, Bv *b, Xf *x1, Xf *x2, Vec3f *o1, Vec3f *o2)
{
    f32 r;
    f32 d;
    f32 t1, t2, t3;
    volatile s32 pad[4];

    if (a->flag == 0) {
        a->f10 = a->f8 * x1->m0 + a->fc * x1->m3 + o1->x;
        a->f14 = a->f8 * x1->m1 + a->fc * x1->m4 + o1->y;
        t1 = x1->m2;
        t2 = x1->m5;
        t3 = o1->z;
        a->flag = 1;
        a->f18 = a->f8 * t1 + a->fc * t2 + t3;
    }
    if (b->flag == 0) {
        b->f10 = b->f8 * x2->m0 + b->fc * x2->m3 + o2->x;
        b->f14 = b->f8 * x2->m1 + b->fc * x2->m4 + o2->y;
        t1 = x2->m2;
        t2 = x2->m5;
        t3 = o2->z;
        b->flag = 1;
        b->f18 = b->f8 * t1 + b->fc * t2 + t3;
    }
    r = a->f1c + b->f1c;
    d = (a->f10 - b->f10) * (a->f10 - b->f10)
      + (a->f14 - b->f14) * (a->f14 - b->f14)
      + (a->f18 - b->f18) * (a->f18 - b->f18);
    if (d < r * r) {
        if (a->child == 0) {
            if (b->child == 0) {
                return 1;
            }
        } else if (b->child == 0 || b->f1c < a->f1c) {
            a = a->child;
            do {
                if (collide_spheres_overlap(a, b, x1, x2, o1, o2) & 0xff) {
                    return 1;
                }
                a = a->next;
            } while (a != 0);
            return 0;
        }
        for (b = b->child; b != 0; b = b->next) {
            if (collide_spheres_overlap(a, b, x1, x2, o1, o2) & 0xff) {
                return 1;
            }
        }
    }
    return 0;
}

s32 collide_find_deepest_overlap(Box_3 *p1, Box_3 *p2, Xf_2 *m1, Xf_2 *m2, Vec3f *v1, Vec3f *v2)
{
    f32 fVar3;
    f32 fVar4;
    f32 fVar5;
    f32 fz;

    f32 d;
    s32 uVar1;
    Vec3f tmp;

    if (p1->flag == 0) {
        p1->x = p1->f08 * m1->f0c + p1->f0c * m1->f18 + v1->x;
        p1->y = p1->f08 * m1->f10 + p1->f0c * m1->f1c + v1->y;
        fVar3 = m1->f14;
        fVar5 = m1->f20;
        fz = v1->z;
        p1->flag = 1;
        p1->z = p1->f08 * fVar3 + p1->f0c * fVar5 + fz;
    }
    if (p2->flag == 0) {
        p2->x = p2->f08 * m2->f0c + p2->f0c * m2->f18 + v2->x;
        p2->y = p2->f08 * m2->f10 + p2->f0c * m2->f1c + v2->y;
        fVar3 = m2->f14;
        fVar5 = m2->f20;
        fz = v2->z;
        p2->flag = 1;
        p2->z = p2->f08 * fVar3 + p2->f0c * fVar5 + fz;
    }
    fVar4 = p1->r + p2->r;
    d = (p1->x - p2->x) * (p1->x - p2->x) +
        (p1->y - p2->y) * (p1->y - p2->y) +
        (p1->z - p2->z) * (p1->z - p2->z);
    if (d < fVar4 * fVar4) {
        if (p1->child == 0) {
            if (p2->child == 0) {
                d = (p1->r + p2->r) - __builtin_sqrtf(d);
                if (sCollideDepth < d) {
                    sCollideDepth = d;
                    sCollideHitA = p1;
                    sCollideHitB = p2;
                }
                return 0;
            }
        } else if (p2->child == 0 || p2->r < p1->r) {
            p1 = p1->child;
            do {
                uVar1 = collide_find_deepest_overlap(p1, p2, m1, m2, v1, v2);
                if ((uVar1 & 0xff) != 0) {
                    return 1;
                }
                p1 = p1->next;
            } while (p1 != 0);
            return 0;
        }
        for (p2 = p2->child; p2 != 0; p2 = p2->next) {
            uVar1 = collide_find_deepest_overlap(p1, p2, m1, m2, v1, v2);
            if ((uVar1 & 0xff) != 0) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

#define GU8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define GS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define GS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define GF32(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define IDX(p)     ((s32)((Obj_41 *)(p) - gRiders_75))

typedef struct { u8 pad[0x7E0]; } Obj_41;

extern Obj_41 gRiders_75[];

s32 collide_resolve_rider_hit(void *p1, void *p2)
{
    f32 vec[3];
    f32 o1[3];
    f32 o2[3];
    s32 a, b;
    f32 *q;
    f32 *pa;
    s32 c;

    if (gCheatFlags & 0x400) {
        if (GU8(p1, 0x6E8) != 0) {
            if (GU8(p2, 0x6E8) != 0) goto skip0;
        } else if (GU8(p2, 0x6E8) != 0) {
            bike_crash_begin(p2, 0x1c);
            return 1;
        }
        if (GU8(p1, 0x6E8) == 0) goto skip0;
        bike_crash_begin(p1, 0x1c);
        return 1;
    }
skip0:

    if (GS32(p1, 0x744) >= 0 && GS32(p2, 0x744) >= 0) return 0;
    if (GS32(p1, 0x744) < 0x3c) goto L12c;
    if (GS32(p2, 0x744) >= 0) goto L12c;

    a = GU8(p1, 0x6E8);
    b = GU8(p2, 0x6E8);
    if ((a & b) == 0 || gEvent.time < 300) announce_takeout(IDX(p1), IDX(p2));
    GS8(p2, 0x45) = -0x19;
    goto L1c8;

L12c:
    if (GS32(p2, 0x744) < 0x3c) goto L1d8;
    if (GS32(p1, 0x744) >= 0) goto L1d8;
    a = GU8(p1, 0x6E8);
    b = GU8(p2, 0x6E8);
    if ((a & b) == 0 || gEvent.time < 300) announce_takeout(IDX(p2), IDX(p1));
    GS8(p1, 0x45) = -0x1C;

L1c8:
    GU8(p1, 0x7A8) = 1;
    GS32(p1, 0x648) = 0;

L1d8:
    if (gGame.mode == 6) {
        if (GF32(p1, 0x50) < GF32(p2, 0x50)) {
            if (GF32(p1, 0x48) < GF32(p2, 0x48)) {
                GS8(p2, 0x45) = -0x1F;
            } else {
                GS8(p2, 0x45) = -0x22;
            }
            *(V3_2 *)((u8 *)p2 + 0x43C) = *(V3_2 *)((u8 *)p2 + 0x54);
            return 1;
        }
        if (GF32(p2, 0x48) < GF32(p1, 0x48)) {
            GS8(p1, 0x45) = -0x1F;
        } else {
            GS8(p1, 0x45) = -0x22;
        }
        *(V3_2 *)((u8 *)p1 + 0x43C) = *(V3_2 *)((u8 *)p1 + 0x54);
        return 1;
    }

    if (gRaceFrame - *(s32 *)((s32)p1 + (IDX(p2) << 2) + 0x784) < 0x3c) return 0;

    pa = (f32 *)((u8 *)p1 + 0x48);
    vec[0] = gCollidePoint_2[0] - pa[0];
    vec[1] = gCollidePoint_2[1] - pa[1];
    vec[2] = gCollidePoint_2[2] - pa[2];
    q = (f32 *)((u8 *)p2 + 0x48);
    mtx3f_mul_vec3(vec, (f32 *)((u8 *)p1 + 0xA0), o1);
    if (-0.2f < o1[2] && o1[2] < 0.2f) return 0;

    vec[0] = gCollidePoint_2[0] - q[0];
    vec[1] = gCollidePoint_2[1] - q[1];
    vec[2] = gCollidePoint_2[2] - q[2];
    mtx3f_mul_vec3(vec, (f32 *)((u8 *)q + 0x58), o2);
    if (-0.2f < o2[2] && o2[2] < 0.2f) return 0;

    if (o2[2] > 0.0f && o1[2] < 0.0f) {
        if (GU8(p2, 0x6E8) == 0) {
            if (GS32(p2, 0x5B0) & 0x20) return 0;
            if ((GS32(p2, 0x5B0) & 0x20010) == 0) return 0;
        }
        GU8(p1, 0x7A8) = 1;
        GS32(p1, 0x648) = 0;
        if (GU8(p1, 0x6E8) == 0 || GU8(p2, 0x6E8) == 0) announce_takeout(IDX(p1), IDX(p2));
        if (o1[0] - o2[0] > 0.0f) {
            GS8(p2, 0x45) = -0x1F;
        } else {
            GS8(p2, 0x45) = -0x22;
        }
        {
            f64 *d = (f64 *)((u8 *)p2 + 0x43C);
            f64 *sp = (f64 *)((u8 *)p2 + 0x54);
            __builtin_memcpy(d, sp, 12);
        }
        return 1;
    }
    if (o1[2] > 0.0f && o2[2] < 0.0f) {
        if (GU8(p1, 0x6E8) == 0) {
            if (GS32(p1, 0x5B0) & 0x20) return 0;
            if ((GS32(p1, 0x5B0) & 0x20010) == 0) return 0;
        }
        GU8(p2, 0x7A8) = 1;
        GS32(p2, 0x648) = 0;
        if (GU8(p1, 0x6E8) == 0 || GU8(p2, 0x6E8) == 0) announce_takeout(IDX(p2), IDX(p1));
        if (o2[0] - o1[0] > 0.0f) {
            GS8(p1, 0x45) = -0x1F;
        } else {
            GS8(p1, 0x45) = -0x22;
        }
        *(V3_2 *)((u8 *)p1 + 0x43C) = *(V3_2 *)((u8 *)p1 + 0x54);
        return 2;
    }
    return 0;
}

#define NORM(v) { f32 len = __builtin_sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); if (len != 0.0f) { v.x /= len; v.y /= len; v.z /= len; } }

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[0x54 - 0x46];
    Vec3f f54;
    u8 pad60[0x3D0 - 0x60];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[0x420 - 0x418];
    f32 f420;
    u8 pad424[0x43C - 0x424];
    Vec3f f43C;
    u8 pad448[0x458 - 0x448];
    s32 f458;
    u8 pad45C[0x4D8 - 0x45C];
    f32 f4D8[5];
    f32 f4EC;
    u8 pad4F0[0x4FC - 0x4F0];
    s32 f4FC;
    u8 pad500[0x57C - 0x500];
    f32 f57C[5];
    f32 f590;
    u8 pad594[0x662 - 0x594];
    s16 f662;
    u8 pad664[0x6E8 - 0x664];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x784 - 0x748];
    s32 f784[12];
    f32 f7B4;
    u8 pad7B8[0x7E0 - 0x7B8];
} Bike_36;

struct Obj_42 {
    Vec3f pos;
    Vec3f vel;
    u8 pad18[0x40 - 0x18];
    Vec3f rot;
    u8 pad4C[0x174 - 0x4C];
    f32 mass;
    f32 inertia;
    u8 type;
    u8 pad17D[3];
    Bike_36 *bike;
};

typedef struct {
    u8 pad0[0x10];
    Vec3f pos;
    f32 r;
} Ball;

extern Ball *sCollideHitA_2;
extern Ball *sCollideHitB_2;
extern Bike_36 gRiders_76[];

s32 collide_apply_impulse(Obj_42 *a, Obj_42 *b) {
    Vec3f n;
    Vec3f ra;
    Vec3f rb;
    Vec3f va;
    Vec3f vb;
    Vec3f dv;
    Vec3f t;
    Vec3f tq;
    Vec3f imp;
    f32 k, len, h, d, d1, ka, kb, j, lim;
    Bike_36 *ba, *bb, *pa, *pb;
    s32 r, vol, ia, ib;

    n.x = sCollideHitA_2->pos.x - sCollideHitB_2->pos.x;
    n.y = sCollideHitA_2->pos.y - sCollideHitB_2->pos.y;
    n.z = sCollideHitA_2->pos.z - sCollideHitB_2->pos.z;
    NORM(n);
    if (n.y > 0.25f) {
        n.y = 0.25f;
        NORM(n);
    } else if (n.y < -0.25f) {
        n.y = -0.25f;
        NORM(n);
    }
    h = sCollideDepth * 0.5f;
    gCollidePoint.x = n.x * (sCollideHitB_2->r - h);
    gCollidePoint.y = n.y * (sCollideHitB_2->r - h);
    gCollidePoint.z = n.z * (sCollideHitB_2->r - h);
    gCollidePoint.x = sCollideHitB_2->pos.x + gCollidePoint.x;
    gCollidePoint.y = sCollideHitB_2->pos.y + gCollidePoint.y;
    gCollidePoint.z = sCollideHitB_2->pos.z + gCollidePoint.z;
    gCollidePoint.y = (a->pos.y + b->pos.y) * 0.5f;
    ra.x = gCollidePoint.x - a->pos.x;
    ra.y = gCollidePoint.y - a->pos.y;
    ra.z = gCollidePoint.z - a->pos.z;
    vec3_cross(&ra, &a->rot, &va);
    va.x = a->vel.x + va.x;
    va.y = a->vel.y + va.y;
    va.z = a->vel.z + va.z;
    rb.x = gCollidePoint.x - b->pos.x;
    rb.y = gCollidePoint.y - b->pos.y;
    rb.z = gCollidePoint.z - b->pos.z;
    vec3_cross(&rb, &b->rot, &vb);
    vb.x = b->vel.x + vb.x;
    vb.y = b->vel.y + vb.y;
    vb.z = b->vel.z + vb.z;
    dv.x = va.x - vb.x;
    dv.y = va.y - vb.y;
    dv.z = va.z - vb.z;
    d = vec3_dot(&dv, &n);
    if (d > 0.0f) {
        return 0;
    }
    if (a->type < 2 && b->type < 2) {
        ba = a->bike;
        bb = b->bike;
        a->mass *= ba->f7B4;
        a->inertia *= ba->f7B4;
        b->mass *= bb->f7B4;
        b->inertia *= bb->f7B4;
        if (ba->f44 == 0 && bb->f44 == 0) {
            r = collide_resolve_rider_hit(ba, bb);
            bb->f784[ba - gRiders_76] = gRaceFrame;
            ba->f784[bb - gRiders_76] = gRaceFrame;
            if (r != 0) {
                return r;
            }
        }
        if (a->type == 0) {
            particles_start_sparks(ba);
        }
        if (b->type == 0) {
            particles_start_sparks(bb);
        }
        vol = (s32)(-d) * 128.0f / 7.0f + 128.0f;
        if (vol > 255) {
            vol = 255;
        }
        if (vol < 0) {
            vol = 0;
        }
        if (vol > 32) {
            snd_play_rider_sfx(4, ba - gRiders_76, vol);
        }
    }
    d1 = vec3_dot(&n, &ra);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = ra.x - t.x;
    t.y = ra.y - t.y;
    t.z = ra.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    ka = a->mass / (k * a->mass / a->inertia + 1.0f);
    d1 = vec3_dot(&n, &rb);
    t.x = n.x * d1;
    t.y = n.y * d1;
    t.z = n.z * d1;
    t.x = rb.x - t.x;
    t.y = rb.y - t.y;
    t.z = rb.z - t.z;
    k = t.x * t.x + t.y * t.y + t.z * t.z;
    kb = b->mass / (k * b->mass / b->inertia + 1.0f);
    if (a->type == 4 || b->type == 4) {
        j = (d - 2.0f) * -1.5f * (ka * kb) / (ka + kb);
    } else {
        j = (d - 2.0f) * -1.1f * (ka * kb) / (ka + kb);
    }
    imp.x = n.x * (j / a->mass);
    imp.y = n.y * (j / a->mass);
    imp.z = n.z * (j / a->mass);
    a->vel.x = a->vel.x + imp.x;
    a->vel.y = a->vel.y + imp.y;
    a->vel.z = a->vel.z + imp.z;
    imp.x = n.x * (j / b->mass);
    imp.y = n.y * (j / b->mass);
    imp.z = n.z * (j / b->mass);
    b->vel.x = b->vel.x - imp.x;
    b->vel.y = b->vel.y - imp.y;
    b->vel.z = b->vel.z - imp.z;
    vec3_cross(&ra, &n, &tq);
    a->rot.x = a->rot.x + tq.x * (j / a->inertia);
    a->rot.y = a->rot.y + tq.y * (j / a->inertia);
    a->rot.z = a->rot.z + tq.z * (j / a->inertia);
    vec3_cross(&rb, &n, &tq);
    b->rot.x = b->rot.x - tq.x * (j / b->inertia);
    b->rot.y = b->rot.y - tq.y * (j / b->inertia);
    b->rot.z = b->rot.z - tq.z * (j / b->inertia);
    if (a->type < 2 && b->type < 2) {
        a->mass = 250.0f;
        a->inertia = 250.0f;
        b->mass = 250.0f;
        b->inertia = 250.0f;
        pa = a->bike;
        pb = b->bike;
        if (pa->f458 <= 0) {
            pa->f4EC = vec3_dot((Vec3f *)pa->f4D8, &pa->f54);
        }
        if (pa->f4FC <= 0) {
            pa->f590 = vec3_dot((Vec3f *)pa->f57C, &pa->f54);
        }
        pa->f420 = __builtin_sqrtf(pa->f54.x * pa->f54.x + pa->f54.y * pa->f54.y + pa->f54.z * pa->f54.z);
        if (pb->f458 <= 0) {
            pb->f4EC = vec3_dot((Vec3f *)pb->f4D8, &pb->f54);
        }
        if (pb->f4FC <= 0) {
            pb->f590 = vec3_dot((Vec3f *)pb->f57C, &pb->f54);
        }
        pb->f420 = __builtin_sqrtf(pb->f54.x * pb->f54.x + pb->f54.y * pb->f54.y + pb->f54.z * pb->f54.z);
        if ((pa->f416 != 0 || pa->f3D0 < 17) && pa->f744 <= 0) {
            pa->f45 = -25;
            pa->f43C = pa->f54;
            return 1;
        }
        if ((pb->f416 != 0 || pb->f3D0 < 16) && pb->f744 <= 0) {
            pb->f45 = -25;
            pb->f43C = pb->f54;
            return 1;
        }
        if (pa->f662 + pb->f662 == 1 && gEvent.time < 360 && !(pa->f6E8 & pb->f6E8)) {
            if (gRaceFrame - gAnnounceTimes[pa - gRiders_76] < 600) {
                announce_place(pb - gRiders_76, pb->f662);
                return 1;
            }
            ib = pb - gRiders_76;
            if (gRaceFrame - gAnnounceTimes[ib] < 600) {
                announce_place(pa - gRiders_76, pa->f662);
                return 1;
            }
            announce_lead_collision(pa - gRiders_76, ib);
        }
    }
    return 1;
}

s32 collide_bodies(u8 *p1, u8 *p2)
{
    u8 *s0;
    u8 *v;
    u8 *tb;
    s32 a, b;

    if (p2 == p1) {
        return 0;
    }
    v = *(u8 **)(p1 + 0x7C);
    sCollideDepth = 0.0f;
    s0 = v;
    if (s0 != 0 && *s0 != 0) {
        *s0 = 0;
        s0 = *(u8 **)(s0 + 0x24);
        while (s0 != 0) {
            collide_tree_clear_cache(s0);
            s0 = *(u8 **)(s0 + 0x20);
        }
    }
    if (*(u8 **)(p1 + 0x180) == *(u8 **)(p2 + 0x180) &&
        *(*(u8 **)(p1 + 0x180) + 0x44) != 2) {
        if (collide_spheres_overlap(*(u8 **)(p1 + 0x7C), *(u8 **)(p2 + 0x7C), p1 + 0x58, p2 + 0x58, p1, p2) & 0xff) {
            return 0;
        }
        *(*(u8 **)(p1 + 0x180) + 0x44) = 2;
    }
    collide_find_deepest_overlap(*(u8 **)(p1 + 0x7C), *(u8 **)(p2 + 0x7C), p1 + 0x58, p2 + 0x58, p1, p2);
    if (0.0f < sCollideDepth) {
        if (collide_apply_impulse(p1, p2) != 0) {
            p2[0x17D] = p2[0x17D] | 1;
            if ((p1[0x17C] == 4 && p2[0x17C] == 0) || (p2[0x17C] == 4 && p1[0x17C] == 0)) {
                if (p1[0x17C] == 4) {
                    p1 = p2;
                }
            } else {
                return 1;
            }
            tb = (u8 *)D_800E3808_2;
            if (p1 == tb) {
                gEvent.ballToucher = 0;
            } else if (p1 == tb + 0x7E0) {
                gEvent.ballToucher = 1;
            } else if (p1 == tb + 0xFC0) {
                gEvent.ballToucher = 2;
            } else if (p1 == tb + 0x17A0) {
                gEvent.ballToucher = 3;
            }
            return 1;
        }
        p1[0x17E] = 0xf;
        p2[0x17E] = 0xf;
    }
    return 0;
}

void collide_tree_clear_cache(Node_17 *p) {
    if (p != NULL && p->flag != 0) {
        p->flag = 0;
        p = p->child;
        while (p != NULL) {
            collide_tree_clear_cache(p);
            p = p->next;
        }
    }
}

void collide_free_node_pool(void) {
    mem_free(sCollideNodePool);
}

void collide_init_node_pool(void)
{
    sCollideNodeCount = 0;
    sCollideNodePool = mem_alloc_dbg(16000, 0, sStrCollideC, 0x103);
    sCollideNodeNext = sCollideNodePool;
    mem_fill(sCollideNodePool, 0, 16000);
}

void collide_reset_body(u8 *p)
{
    f32 one = 1.0f;
    *(s32 *)(p + 0xC) = 0;
    *(s32 *)(p + 0x10) = 0;
    *(s32 *)(p + 0x14) = 0;
    *(s32 *)(p + 0x18) = 0;
    *(s32 *)(p + 0x1C) = 0;
    *(s32 *)(p + 0x20) = 0;
    *(s32 *)(p + 0x40) = 0;
    *(s32 *)(p + 0x44) = 0;
    *(s32 *)(p + 0x48) = 0;
    *(s32 *)(p + 0x4C) = 0;
    *(s32 *)(p + 0x50) = 0;
    *(s32 *)(p + 0x54) = 0;
    *(s32 *)(p + 0x30) = 0;
    *(s32 *)(p + 0x34) = 0;
    *(s32 *)(p + 0x3C) = 0;
    *(f32 *)(p + 0x38) = one;
    quat_to_mtx3f((f32 *)(p + 0x58), (f32 *)(p + 0x30));
}

void collide_update_bike_speeds(u8 *p)
{
    f32 s;
    f32 r;

    if (*(s32 *)(p + 0x458) < 1) {
        *(f32 *)(p + 0x4EC) = vec3_dot((f32 *)(p + 0x4D8), (f32 *)(p + 0x54));
    }
    if (*(s32 *)(p + 0x4FC) < 1) {
        *(f32 *)(p + 0x590) = vec3_dot((f32 *)(p + 0x57C), (f32 *)(p + 0x54));
    }
    s = *(f32 *)(p + 0x54) * *(f32 *)(p + 0x54)
      + *(f32 *)(p + 0x58) * *(f32 *)(p + 0x58)
      + *(f32 *)(p + 0x5C) * *(f32 *)(p + 0x5C);
    r = __builtin_sqrtf(s);
    *(f32 *)(p + 0x420) = r;
}

void collide_reset_tree(Node_18 *p) {
    Node_18 *q;
    sCollideDepth = 0;
    q = p;
    if (q != NULL && q->flag != 0) {
        q->flag = 0;
        for (q = q->first; q != NULL; q = q->sib) {
            collide_tree_clear_cache(q);
        }
    }
}
