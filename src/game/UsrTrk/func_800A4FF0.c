#include "ctx.h"

typedef struct {
    u32 n;
    Vtx *v;
} Mesh;

typedef struct {
    s32 x;
    Mesh *mesh;
    u8 pad[0x18];
} Obj;

extern Mesh **D_800CE9B4;

#define E(o, idx, kk) (*(o))->v[idx].v.ob[1] = ((kk) > 0 && (kk) < n * 2) ? h[(kk) - 1] : 0

void func_800A4FF0(Mesh **dst, Mesh **src, s16 *h, s32 n)
{
    Mesh **tmpl = D_800CE9B4;
    s32 i, j, k;
    s32 idx, c, a, b;
    u32 m;
    Vtx *v;
    Mesh **d, **s, **t;

    k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 2; j++) {
            s = &src[(i * 2 + j) * 8 + 1];
            t = &tmpl[j * 8 + 1];
            d = &dst[(i * 2 + j) * 8 + 1];
            for (m = 0; m < (*s)->n; m++) {
                {
                    s32 x = i * 2 + ((*s)->v[m].v.ob[2] + 250) / 250;
                    if (x > 0 && x < n * 2) a = h[x - 1]; else a = 0;
                }
                {
                    s32 x = i * 2 + ((*s)->v[m].v.ob[2] + 250) / 250;
                    s32 y = x + 1;
                    if (y > 0 && y < n * 2) b = h[x]; else b = 0;
                }
                c = ((*s)->v[m].v.ob[2] + 250) % 250;
                (*s)->v[m].v.ob[1] = (*t)->v[m].v.ob[1] + a + (b - a) * c / 250;
            }
            if (j == 0) {
                E(d, 4, k);
                E(d, 10, k);
                E(d, 11, k);
                E(d, 1, k + 1);
                E(d, 6, k + 1);
                E(d, 7, k + 1);
                E(d, 3, k + 2);
                E(d, 8, k + 2);
                E(d, 9, k + 2);
            } else {
                E(d, 4, k);
                E(d, 5, k);
                E(d, 11, k);
                E(d, 1, k + 1);
                E(d, 6, k + 1);
                E(d, 0, k + 1);
                E(d, 3, k + 2);
                E(d, 8, k + 2);
                E(d, 2, k + 2);
            }
        }
        k += 2;
    }
}
