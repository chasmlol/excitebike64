#include "ctx.h"

typedef struct Block {
    struct Block *next;
    s32 unk4;
    s32 size;
    s32 check;
} Block;

#define BLOCK_MAGIC 0xB562439F
#define BLOCK_SUM(b) ((u32)(b)->next + (b)->unk4 + (b)->size + (b)->check)

s32 func_800BDB44(Block *start, Block *end, Block *list) {
    Block *b;
    s32 n = 0x400;

    if (start != NULL) {
        b = start;
        do {
            if (b >= end) {
                goto final;
            }
            if (BLOCK_SUM(b) != BLOCK_MAGIC) {
                return -1;
            }
            b += b->size;
        } while (b >= start);
    final:
        if (BLOCK_SUM(b) != BLOCK_MAGIC) {
            return -1;
        }
        b = list->next;
        while (--n != -1) {
            if (b == list) {
                goto done;
            }
            if (b < start || b >= end || ((u32)b & 0xF) != 0) {
                break;
            }
            b = b->next;
        }
        if (b != list) {
            return -1;
        }
    }
done:
    return 0x400 - n;
}
