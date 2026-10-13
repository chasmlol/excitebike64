#include "ctx.h"

Gfx *func_80048370(Gfx *g)
{
    Gfx *p0, *p1, *p2, *p3, *p4, *p5;
    p0 = g++;
    p0->words.w0 = 0xE7000000;
    p0->words.w1 = 0;
    p1 = g++;
    p1->words.w0 = 0xE3001001;
    p1->words.w1 = 0;
    p2 = g++;
    p2->words.w0 = 0xF9000000;
    p2->words.w1 = 0x10;
    p3 = g++;
    p3->words.w0 = 0xE3000C00;
    p3->words.w1 = 0;
    p4 = g++;
    p4->words.w0 = 0xE3001201;
    p4->words.w1 = 0;
    p5 = g++;
    p5->words.w0 = 0xD7000002;
    p5->words.w1 = 0x80008000;
    return g;
}
