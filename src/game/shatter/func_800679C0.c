#include "ctx.h"

typedef struct Vt {
    s16 ob[3];
    u16 flag;
    s16 tc[2];
    u8 cn[4];
} Vt;

typedef struct {
    s16 o[3];
    s16 p[3];
    s16 pad[3];
    s16 x[3];
    s16 y[3];
    s16 z[3];
    s16 pad2[4];
} Src;

typedef struct {
    s32 pad;
    Vt *v;
} Obj;

typedef struct {
    Obj **obj;
    s32 count;
    s32 flag;
    s32 pad[8];
    Src *src;
} Rec;

extern s32 D_800CCCE0;
extern Rec *D_800F1444;
void func_800AD910(s32, s32);

void func_800679C0(void)
{
    s32 n;
    s32 nt;
    s32 m;
    Rec *base;
    s32 i, j, k;
    Rec *r;
    Src *s;
    Vt *v;

    nt = D_800CCCE0;
    i = 0;
    if (nt > 0) {
    m = -1;
    n = nt;
    base = D_800F1444;
    do {
        r = &base[i];
        s = r->src;
        r->flag = m;
        v = (*r->obj)->v;
        for (j = 0; j < r->count; j++, s++, v += 3) {
            s->p[0] = s->o[0];
            s->p[1] = s->o[1];
            s->p[2] = s->o[2];
            for (k = 0; k < 3; k++) {
                v[k].ob[0] = (s->x[k] + s->p[0]) >> 4;
                v[k].ob[1] = (s->y[k] + s->p[1]) >> 4;
                v[k].ob[2] = (s->z[k] + s->p[2]) >> 4;
            }
        }
        i++;
    } while (i < n);
    }
    func_800AD910(0, 0x800000);
}
