#include "ctx.h"

typedef struct Block {
    struct Block *next;
    struct Block *prev;
    s32 size;
    u32 check;
} Block;

#define BLK_CHECKB(b) ((b)->check = 0xB562439F - ((u32)(b)->prev + (u32)(b)->next + (b)->size))
#define BLK_CHECK(b) ((b)->check = 0xB562439F - ((u32)(b)->next + (u32)(b)->prev + (b)->size))

extern Block **D_80100648;
extern u32 D_80100630;
extern s32 D_80100634;
extern s32 D_80100638;
extern s32 D_8010063C;

void *func_800BCCF0(void *ptr, s32 size, s32 tag, s32 id)
{
    Block *hdr;
    Block *b;
    Block *first;
    Block *end;
    Block *lim;
    Block *next;
    Block *prev;
    u32 units;

    if (size == 0) {
        return 0;
    }
    hdr = (Block *)((u32)ptr & ~0xF) - 1;
    do {
    units = (u32)ptr - (u32)hdr;
    units = (units + size + 15) >> 4;
    } while (0);
    lim = hdr + units;
    first = b = *D_80100648;
    for (;;) {
        size = b->size;
        if (hdr < b || (end = b + size) < lim) {
            b = b->next;
            if (b == first) {
                break;
            }
        } else {
            first = hdr + units;
            size = end - first;
            next = b->next;
            prev = b->prev;
            if (size > 0) {
                b->next = first;
                next->prev = first;
                BLK_CHECK(next);
                first->next = next;
                first->prev = b;
                first->size = size;
                next = first;
            }
            if (b < hdr) {
                size = hdr - b;
                b->size = size;
                BLK_CHECK(b);
            } else {
                prev->next = next;
                next->prev = prev;
            }
            do {
                size = D_80100630;
                id = (id << 16) | size | 1;
                size += 2;
                size &= 0xFFFE;
                D_80100630 = size;
                BLK_CHECK(prev);
            } while (0);
            {
                s32 t;
                u32 pv;
                t = D_80100634;
                pv = (u32)next->prev;
                D_80100634 = t - units;
                next->check = 0xB562439F - ((u32)next->next + pv + next->size + ((u16)pv >> 16));
            }
            hdr->next = (Block *)tag;
            hdr->prev = (Block *)id;
            hdr->size = units;
            BLK_CHECK(hdr);
            D_8010063C += units;
            if (D_80100638 < D_8010063C) {
                D_80100638 = D_8010063C;
            }
            return ptr;
        }
    }
    return 0;
}
