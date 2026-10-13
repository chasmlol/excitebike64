#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
    u8 pad8[0x5D0 - 8];
    s8 a;
    s8 pad5D1;
    s8 b;
    s8 pad5D3;
    s32 widx;
    s32 wbuf[8];
    s32 wavg;
    s8 bidx;
    s8 bbuf[8];
    s8 pad605[8];
    s8 bavg;
    s8 cidx;
    s8 cbuf[16];
} Obj;

void func_8003F004(Obj *p, s32 n)
{
    s32 v;
    s32 w;
    s32 i;
    s32 sum;
    s32 sum2;

    v = p->b;
    w = p->a;
    if (v > 15) {
        v = 15;
    }
    for (i = 0; i < n; i++) {
        p->bbuf[p->bidx] = v;
        p->bidx = (p->bidx + 1) % 8;
        p->cbuf[p->cidx] = w;
        p->cidx = (p->cidx + 1) & 0xF;
    }
    p->wbuf[p->widx] = p->f4;
    p->widx = (p->widx + 1) % 8;
    sum = 0;
    for (i = 0; i < 8; i++) {
        sum += p->bbuf[i];
    }
    p->bavg = sum / 8;
    sum2 = 0;
    for (i = 0; i < 8; i++) {
        sum2 += p->wbuf[i];
    }
    p->wavg = sum2 / 8;
}
