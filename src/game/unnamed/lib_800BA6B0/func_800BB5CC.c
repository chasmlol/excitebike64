#include "ctx.h"

typedef struct {
    u32 pad;
    u32 w[4];
    u32 tail[2];
} Ent;

extern Ent D_800FE8F0[];

void func_800BB5CC(s32 i, u32 *src)
{
    Ent *base = D_800FE8F0;
    Ent *e = &base[i];
    e->w[0] = src[0];
    e->w[1] = src[1];
    e->w[2] = src[2];
    e->w[3] = src[3];
}
