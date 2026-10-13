#include "ctx.h"

extern void func_800BCB04(void *p);
extern s32 *func_800BC908(u32 size, s32 a, s32 b, s32 c);

typedef struct { s32 w[4]; } Blk;

s32 *func_800BD798(s32 *p, u32 size, s32 a, s32 b, s32 c)
{
    Blk *blk;
    Blk *nb;
    u32 need;
    u32 have;
    s32 *q;
    s32 *src;
    s32 *dst;
    s32 cnt;

    if (size == 0) {
        if (p == 0) {
            return 0;
        }
        func_800BCB04(p);
        return 0;
    }
    if (p != 0) {
        blk = (Blk *)p - 1;
        need = ((size + 0xF) >> 4) + 1;
        have = blk->w[2];
        if (need == have) {
            return p;
        }
        if (need <= have) {
            nb = blk + need;
            nb->w[2] = have - need;
            nb->w[0] = 0;
            nb->w[1] = 1;
            {
                s32 sum = nb->w[0] + 1;
                nb->w[3] = 0xB562439F - (sum + nb->w[2]);
            }
            blk->w[2] = need;
            blk->w[3] = 0xB562439F - (blk->w[0] + blk->w[1] + need);
            func_800BCB04(nb + 1);
            return p;
        }
    }
    q = func_800BC908(size, a, b, c);
    if (q != 0 && p != 0) {
        src = p;
        dst = q;
        need = ((Blk *)p - 1)->w[2] * 4;
        while (--need != -1) {
            *dst++ = *src++;
        }
        func_800BCB04(p);
    }
    return q;
}
