#include "ctx.h"

typedef struct Xf {
    u8 pad0[0xc];
    f32 f0c;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    f32 f20;
} Xf;

typedef struct Box {
    u8 flag;
    u8 pad1[7];
    f32 f08;
    f32 f0c;
    f32 x;
    f32 y;
    f32 z;
    f32 r;
    struct Box *next;
    struct Box *child;
} Box;

extern Box *D_800F0B10;
extern Box *D_800F0B14;
extern f32 D_800F0B18;

s32 func_800607BC(Box *p1, Box *p2, Xf *m1, Xf *m2, Vec3f *v1, Vec3f *v2)
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
                if (D_800F0B18 < d) {
                    D_800F0B18 = d;
                    D_800F0B10 = p1;
                    D_800F0B14 = p2;
                }
                return 0;
            }
        } else if (p2->child == 0 || p2->r < p1->r) {
            p1 = p1->child;
            do {
                uVar1 = func_800607BC(p1, p2, m1, m2, v1, v2);
                if ((uVar1 & 0xff) != 0) {
                    return 1;
                }
                p1 = p1->next;
            } while (p1 != 0);
            return 0;
        }
        for (p2 = p2->child; p2 != 0; p2 = p2->next) {
            uVar1 = func_800607BC(p1, p2, m1, m2, v1, v2);
            if ((uVar1 & 0xff) != 0) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}
