#include "ctx.h"

typedef struct {
    u8 g0[6];
    s8 kadj;
    u8 g7;
} Glyph;

typedef struct {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    s8 f9;
    u8 pa[2];
    s8 fc;
    s8 fd;
    s8 fe;
    u8 pf;
    Glyph gl[1];
} Font;

u8 *func_800C1228(u8 *s, Font *f, u32 *outMax, s32 *outW)
{
    u8 *t = s;
    s32 w;
    s8 flag = 0;
    u32 mx = 0;
    u32 cur;
    u32 c;
    u32 lo;
    u32 hi;
    s32 adj;
    u8 *kt;
    Glyph *gl;

    *outW = 0;
    *outMax = 0;
    w = f->fe;
    lo = f->base;
    hi = f->count;
    adj = f->adj;
    gl = f->gl;
    while (*t != 0) {
        cur = 0;
        c = *t;
        while (c != 0) {
            t++;
            if (c == 10) {
                flag = 1;
                w += f->fc + f->fd + f->f9;
                break;
            }
            if (c != 0x20) {
                flag = 0;
            }
            c -= lo;
            if ((s32)c < 0 || c >= hi) {
                break;
            }
            cur += gl[c].kadj + adj;
            kt = f->kern;
            if (kt != NULL && kt[c] != 0) {
                kt += kt[c] * 2;
                while (*kt != 0) {
                    if (*kt == *s) {
                        cur += (s8)kt[1];
                        break;
                    }
                    kt += 2;
                }
            }
            c = *t;
        }
        if (mx < cur) {
            mx = cur;
        }
    }
    if (mx != 0) {
        mx -= adj;
    }
    if (flag) {
        w -= f->fc + f->fd + f->f9;
    }
    *outMax = mx;
    *outW = w;
    return t;
}
