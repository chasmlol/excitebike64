#include "ctx.h"

typedef struct {
    s32 n;
    u32 *list;
} ListHdr;

void func_800BCB04(u32 a);

void func_800BFEE4(ListHdr *h)
{
    s32 i;
    u32 *e;
    i = 0;
    while (i < h->n) {
        e = (u32 *)((i << 2) + (s32)h->list);
        if (*e != 0) {
            if ((*e & 2) == 0) {
                func_800BCB04(*e);
            }
            *e = 0;
        }
        i++;
    }
}
