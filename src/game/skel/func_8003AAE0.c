#include "ctx.h"

typedef struct {
    u32 w0, w1;
} Blk;

extern u32 D_800E92DC;
extern u8 D_800E37C0[];

Blk *func_8003AAE0(Blk *gdl, s32 unused, s32 n)
{
    s32 k = n;
    Blk *p0 = gdl++;
    Blk *p1 = gdl++;
    u32 c1 = 0xE3000A01;
    Blk *p2 = gdl++;
    Blk *p3 = gdl++;
    Blk *p4, *p5;
    Blk *p6;
    Blk *q0, *q1, *q2;

    p0->w0 = 0xE7000000;
    p0->w1 = 0;
    p1->w1 = 0x300000;
    p1->w0 = c1;
    p2->w0 = 0xF7000000;
    p2->w1 = 0;
    p3->w0 = 0xED000000;
    p3->w1 = 0x80080;
    p4 = gdl++;
    p4->w0 = 0xE7000000;
    p4->w1 = 0;
    p5 = gdl++;
    p5->w0 = 0xFF68001F;
    p5->w1 = D_800E92DC + k * 0x400;
    p6 = gdl++;
    p6->w0 = 0xF607C07C;
    p6->w1 = 0;
    if (D_800E37C0[k * 0x7E0 + 0x44] != 0) {
        q0 = gdl++;
        q0->w0 = 0xE7000000;
        q0->w1 = 0;
        q1 = gdl++;
        q1->w0 = 0xFF68001F;
        q1->w1 = D_800E92DC + (k + 8) * 0x400;
        q2 = gdl++;
        q2->w0 = 0xF607C07C;
        q2->w1 = 0;
    }
    {
        Blk *e = gdl++;
        e->w0 = 0xE7000000;
        e->w1 = 0;
    }
    {
        Blk *e = gdl++;
        e->w0 = 0xE3000A01;
        e->w1 = 0;
    }
    return gdl;
}
