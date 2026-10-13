#include "ctx.h"

typedef struct {
    s32 f[7];
} Rec;

extern Rec D_800FE8F0[];

void func_800BB590(s32 idx, s32 *out) {
    Rec *base = D_800FE8F0;
    Rec *r = base + idx;
    out[0] = r->f[1];
    out[1] = r->f[2];
    out[2] = r->f[3];
    out[3] = r->f[4];
}
