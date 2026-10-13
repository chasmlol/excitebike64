#include "ctx.h"

typedef struct Bv {
    u8 flag;
    u8 pad[7];
    f32 f8;
    f32 fc;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    struct Bv *next;
    struct Bv *child;
} Bv;

typedef struct Xf {
    u8 pad[0xc];
    f32 m0, m1, m2, m3, m4, m5;
} Xf;

s32 func_80060538(Bv *a, Bv *b, Xf *x1, Xf *x2, Vec3f *o1, Vec3f *o2)
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
                if (func_80060538(a, b, x1, x2, o1, o2) & 0xff) {
                    return 1;
                }
                a = a->next;
            } while (a != 0);
            return 0;
        }
        for (b = b->child; b != 0; b = b->next) {
            if (func_80060538(a, b, x1, x2, o1, o2) & 0xff) {
                return 1;
            }
        }
    }
    return 0;
}
