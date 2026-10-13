#include "ctx.h"

typedef struct Blk {
    struct Blk *next;
    struct Blk *prev;
    s32 size;
    u32 check;
} Blk;

extern Blk *D_80100648;
extern u32 D_80100630;
extern s32 D_80100634;
extern s32 D_80100638;
extern s32 D_8010063C;

#define CHECK(b) ((b)->check = 0xB562439F - ((u32)(b)->next + (u32)(b)->prev + (b)->size))

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag)
{
    Blk *sent;
    Blk *b;
    Blk *nb;
    Blk *next;
    Blk *prev;
    s32 n;

    if (size == 0) {
        return 0;
    }
    sent = D_80100648;
    n = ((u32)(size + 15) >> 4) + 1;
    if (!(flags & 1)) {
        b = sent->next;
        for (;;) {
            size = b->size;
            if (n <= size) {
                goto found;
            }
            b = b->next;
            if (b == sent) {
                break;
            }
        }
    } else {
        b = sent->prev;
        for (;;) {
            size = b->size;
            if (n <= size) {
                goto found;
            }
            b = b->prev;
            if (b == sent) {
                break;
            }
        }
    }
    return 0;
found:
    prev = b->prev;
    next = b->next;
    if (size == n) {
        prev->next = next;
        next->prev = prev;
        CHECK(prev);
        CHECK(next);
    } else {
        size -= n;
        if (!(flags & 1)) {
            sent = b + n;
            sent->next = next;
            sent->prev = prev;
            sent->size = size;
            prev->next = sent;
            next->prev = sent;
            CHECK(prev);
            CHECK(sent);
            CHECK(next);
        } else {
            b->size = size;
            b->check = 0xB562439F - ((u32)b->next + (u32)b->prev + size);
            b += size;
        }
    }
    b->next = (Blk *)owner;
    b->size = n;
    tag = (tag << 16) | D_80100630 | 1;
    D_80100630 = (D_80100630 + 2) & 0xFFFE;
    b->prev = (Blk *)tag;
    D_80100634 -= n;
    b->check = 0xB562439F - ((u32)b->next + (u32)b->prev + n);
    D_8010063C += n;
    if (D_80100638 < D_8010063C) {
        D_80100638 = D_8010063C;
    }
    return b + 1;
}
