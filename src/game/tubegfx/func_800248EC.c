#include "ctx.h"

typedef struct {
    u32 f0, f4, f8, fc, f10, f14, f18, f1c;
} Ent;

void func_800248EC(u32 *param)
{
    u32 *base = (u32 *)*param;
    Ent *e;
    u32 i;
    u32 *q;
    u32 j;
    u32 n;
    if (*base != 0) {
        i = 0;
        do {
            e = (Ent *)base + i;
            e->f4 = (u32)base + e->f4;
            e->f18 = (u32)base + e->f18;
            q = (u32 *)((volatile Ent *)e)->f4;
            q[1] = (u32)base + q[1];
            q[3] = (u32)base + q[3];
            if (q[5] != 0) {
                q[5] = (u32)base + q[5];
            }

            j = 0;
            if (q[0] != 0) {
                do {
                    j++;
                } while (j < q[0]);
            }
            i++;
        } while (i < *base);
    }
}
