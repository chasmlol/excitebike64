#include "ctx.h"

typedef struct {
    u32 size;
    void *addr;
} Ent;

typedef struct {
    u32 h0;
    u32 h1;
    Ent e[1];
} Bank;

extern Bank *D_800E77F8;
extern u8 **D_800C9584;

Gfx *func_800281F4(Gfx *gdl, s32 idx)
{
    if (D_800E77F8 != NULL && D_800C9584 != NULL) {
        u8 *cur = D_800C9584[idx];
        u8 *end = D_800C9584[idx + 1];
        s32 i;

        for (i = 0; cur != end; cur += 4, i++) {
            gSPSegment(gdl++, i + 8, D_800E77F8->e[cur[2] + cur[0]].addr);
        }
    }
    return gdl;
}
