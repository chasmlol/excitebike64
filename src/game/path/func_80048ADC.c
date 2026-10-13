#include "ctx.h"

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
} Node;

extern Node *D_800EE498;

void func_80048ADC(s32 start, s32 stop)
{
    s32 p = start;
    f32 v, u, t, x, y, sq, n, n2, r, d, q1, q2, aq;
    Node *m;

    do {
        x = D_800EE498[p].a2;
        y = D_800EE498[p].b2;
        sq = x * x + y * y;
        t = 2.0f * D_800EE498[p].b1;
        u = x * t;
        t = 2.0f * D_800EE498[p].a1;
        n = u - y * t;
        r = __builtin_sqrtf(sq);
        d = sq * r;
        m = (Node *)(D_800EE498[p].link * sizeof(Node) + (u32)D_800EE498);
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
            D_800EE498[p].k = q2;
        } else {
            D_800EE498[p].k = q1;
        }
        p = D_800EE498[p].next;
    } while (p != stop);
}
