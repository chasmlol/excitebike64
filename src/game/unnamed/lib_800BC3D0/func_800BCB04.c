#include "ctx.h"

typedef struct Blk {
    struct Blk *next;
    struct Blk *prev;
    s32 size;
    u32 sum;
} Blk;

extern Blk *D_80100648;
extern s32 D_80100634;
extern s32 D_8010063C;
extern char D_800DB248[];
extern char D_800DB288[];
extern char D_800DB2BC[];

void func_8000EFB0(char *fmt, ...);
void func_800BC3D0(void);

void func_800BCB04(void *ptr)
{
    Blk *p;
    Blk *a;
    Blk *n;
    Blk *t;
    u32 sum;
    char *m;
    s32 size;
    u32 flag;
    u32 tv;

    if (ptr == NULL) {
        return;
    }
    p = (Blk *)ptr - 1;
    tv = (u32)p->next;
    flag = (u32)p->prev;
    size = p->size;
    sum = tv + flag + size + p->sum;
    if (sum != 0xB562439F) {
        goto inv;
    }
    t = p + p->size;
    if ((u32)t->next + (u32)t->prev + t->size + t->sum != sum) {
        goto overrun;
    }
    if (!((s32)p->prev & 1)) {
        goto alr;
    }
    a = D_80100648;
    p->prev = NULL;
    n = a->next;
    D_80100634 += p->size;
    D_8010063C -= p->size;
    if (a == n) {
        goto insert;
    }
    for (;;) {
        if (a < p && p < n) {
            goto insert;
        }
        if (a >= n && (a < p || p < n)) {
            goto insert;
        }
        a = n;
        n = n->next;
    }
insert:
    a->next = p;
    n->prev = p;
    p->prev = a;
    p->next = n;
    if (a + a->size == p) {
        a->size += p->size;
        a->next = p->next;
        p->next->prev = a;
        p = a;
    }
    if (p + p->size == n) {
        p->size += n->size;
        p->next = n->next;
        n = n->next;
        n->prev = p;
    }
    a->sum = 0xB562439F - ((u32)a->next + (u32)a->prev + a->size);
    p->sum = 0xB562439F - ((u32)p->next + (u32)p->prev + p->size);
    n->sum = 0xB562439F - ((u32)n->next + (u32)n->prev + n->size);
    return;
inv:
    m = D_800DB248;
    goto call;
overrun:
    m = D_800DB288;
    goto call;
alr:
    m = D_800DB2BC;
call:
    func_8000EFB0(m, size, sum, flag);
    func_800BC3D0();
}
