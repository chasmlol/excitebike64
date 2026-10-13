#include "ctx.h"

typedef struct Block {
    struct Block *next;
    s32 unk4;
    s32 size;
    s32 check;
} Block;

#define BLOCK_MAGIC 0xB562439F
#define BLOCK_SUM(b) ((u32)(b)->next + (b)->unk4 + (b)->size)

Block *func_800BDA54(Block **start, Block **end)
{
    Block *p = *start;
    Block *e = *end;
    u32 n;

    if (p != NULL) {
        p = (Block *)(((u32)p + 0xF) & ~0xF);
        e = (Block *)((u32)e & ~0xF) - 1;
        n = e - p;
        if (n < 4) {
            return NULL;
        }
        *start = p;
        *end = e;
        e->next = e;
        e->unk4 = (s32)e;
        e->size = 0;
        e->check = BLOCK_MAGIC - BLOCK_SUM(e);
        p[0].next = &p[2];
        p[0].unk4 = (s32)&p[2];
        p[0].size = 1;
        p[0].check = BLOCK_MAGIC - BLOCK_SUM(&p[0]);
        p[1].next = NULL;
        p[1].unk4 = 1;
        p[1].size = 1;
        p[1].check = BLOCK_MAGIC - BLOCK_SUM(&p[1]);
        p[2].next = p;
        p[2].unk4 = (s32)p;
        p[2].size = n - 2;
        p[2].check = BLOCK_MAGIC - BLOCK_SUM(&p[2]);
    }
    return p;
}
