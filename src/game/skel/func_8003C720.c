#include "ctx.h"
typedef struct { u8 pad[0x34]; s32 x, y, z; u8 pad2[0x40]; } Blk;
typedef struct { s16 a, b, c; u16 idx; u8 pad[8]; } Ent;
typedef struct { u32 n; Ent *e; } List;

void func_8003C720(Blk *tbl, List *list) {
    u32 i;
    for (i = 0; i < list->n; i++) {
        u16 k = list->e[i].idx;
        list->e[i].a -= tbl[k].x;
        list->e[i].b -= tbl[k].y;
        list->e[i].c -= tbl[k].z;
    }
}
