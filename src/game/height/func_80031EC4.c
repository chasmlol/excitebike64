#include "ctx.h"

typedef struct {
    s16 x, z;
} Pt;

typedef struct {
    u8 hdr[6];
    u16 n;
    Pt pts[1];
} Poly;

s32 func_80031EC4(Poly *poly, s32 px, s32 pz)
{
    s32 pos = 0;
    s32 neg;
    s32 i = 0, j, n, cr;

    n = poly->n;
    neg = 0;
    px = px >> 6;
    pz = pz >> 6;
    if (n != 0) {
        do {
            j = (i + 1) % n;
            cr = (pz - poly->pts[i].z) * (poly->pts[j].x - poly->pts[i].x);
            cr -= (px - poly->pts[i].x) * (poly->pts[j].z - poly->pts[i].z);
            if (cr >= 0) {
                pos++;
            }
            if (cr <= 0) {
                neg++;
            }
            n = poly->n;
            i++;
        } while (i < n);
    }
    if (pos == poly->n) {
        goto ret1;
    }
    if (neg != poly->n) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}
