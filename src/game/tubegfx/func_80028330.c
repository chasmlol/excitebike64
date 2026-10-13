#include "ctx.h"

typedef struct { u32 a, b, c; u32 val; } Rec;
typedef struct { u32 count; Rec *recs; } Sub;
typedef struct { u32 count; Sub *sub; u32 pad[6]; } Ent;

void func_80028330(Ent *p)
{
    u32 i, j;
    for (i = 0; i < p->count; i++) {
        for (j = 0; j < p[i].sub->count; j++) {
            u32 v = p[i].sub->recs[j].val;
            v = (v & 0xFF) | ((v >> 16) & 0xFF00) | ((v << 8) & 0xFFFF0000);
            p[i].sub->recs[j].val = v;
        }
    }
}
