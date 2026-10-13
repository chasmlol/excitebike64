#include "game.h"

void func_80048230(void *arc)
{
    if (D_800CA770 != 2) {
        if (D_800CA770 != 0 && func_800C1DEC() != 0) {
            func_8000ECB8(1);
        }
        while (func_80065858() != 0) {
        }
        D_800D5870 = D_800C29E0;
        if (func_800C06D0(arc, D_800CA774, D_800CA79C) != 0) {
            func_8000ECB8(1);
        }
        D_800D5870 = 0;
        D_800CA770 = 2;
    }
}

void func_800482D0(u32 *param_1)
{
    if (D_800CA770 != 1) {
        if (D_800CA770 != 0 && func_800C1DEC() != 0) {
            func_8000ECB8(1);
        }
        if (func_800C06D0(param_1, D_800CA784, D_800CA79C) != 0) {
            func_8000ECB8(1);
        }
        D_800CA770 = 1;
    }
}

void func_8004834C(void) {
    func_800C1DEC();
    D_800CA770 = 0;
}

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
