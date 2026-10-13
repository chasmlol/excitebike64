#include "ctx.h"

typedef struct Node {
    s32 w0;
    f32 x;
    f32 y;
    f32 z;
    s32 w10;
    s32 w14;
    s32 w18;
    f32 r;
    struct Node *c1;
    struct Node *c2;
} Node;

typedef struct Param {
    s32 kid;
    s32 kid2;
    f32 x;
    f32 y;
    f32 z;
    f32 r;
} Param;

extern Node *D_800F0B04;
extern s32 D_800F0B08;

Node *func_80060270(Param *p)
{
    Node *n = D_800F0B04;
    Node *a;
    Node *b;
    f32 pa[3];
    f32 pb[3];
    f32 d[3];
    f32 len;

    D_800F0B04 = n + 1;
    D_800F0B08 = D_800F0B08 + 1;
    if (p->kid == 0) {
        n->x = p->x;
        n->y = p->y - 0.5f;
        n->z = p->z;
        n->r = p->r;
    } else {
        a = func_80060270(p + p->kid);
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
        n->c1 = func_80060270(p + p->kid2);
    }
    p->r = n->r;
    return n;
}
