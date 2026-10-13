#include "ctx.h"

typedef struct Ent {
    struct Ent *next;
    struct Ent *prev;
    u8 *buf;
    s32 owner;
    s32 idx;
} Ent;

typedef struct X {
    u8 pad[0x10];
    s32 flags;
    s32 pad2;
    u8 *base;
    u32 size;
} X;

typedef struct Obj {
    X *x;
    Ent *ent;
} Obj;

extern Ent D_80100680;
extern Ent *D_80100678;
extern Ent *D_8010067C;
extern s32 D_80100694;
extern u8 *D_801006F0;
extern u8 *D_8010065C;
extern u8 *D_80100660;

void func_800BEB68(void *param_1, void *param_2, s32 param_3);
s32 func_800BDE38(void);
Ent *func_800BE2D0(Obj *so, s32 si, s8 sf);

static inline void mtf(Ent *e)
{
    Ent *h;

    h = D_80100678->next;
    if (e != h) {
        e->prev->next = e->next;
        e->next->prev = e->prev;
        e->next = h;
        e->prev = h->prev;
        e->prev->next = e;
        h->prev = e;
    }
}

static inline Ent *find(Obj *so)
{
    Ent *cur, *nx;
    s32 want;

    want = -1;
    if (D_80100680.owner == (s32)so->x) {
        return &D_80100680;
    }
    nx = D_80100678->next;
    for (;;) {
        cur = nx;
        nx = cur->next;
        if (nx == NULL) {
            cur = func_800BE2D0(so, -1, 0);
            if (cur == NULL) {
                return NULL;
            }
        }
        if (cur->owner == (s32)so->x) {
            if (cur->idx < 0) {
                cur->idx--;
                break;
            }
            if (cur->idx == want) {
                break;
            }
        }
    }
    if (cur != &D_80100680) {
        nx = D_80100678->next;
        if (cur != nx) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            cur->next = nx;
            cur->prev = nx->prev;
            cur->prev->next = cur;
            nx->prev = cur;
        }
    }
    return cur;
}

Ent *func_800BE2D0(Obj *so, s32 si, s8 sf)
{
    Ent *e;
    u32 n;
    u32 a3, a2;

    if (so->x->flags != 0 && si >= 0) {
        if (so->ent == NULL || (so->ent == &D_80100680 && D_80100680.owner != (s32)so->x)) {
            so->ent = find(so);
            if (so->ent == NULL) {
                return NULL;
            }
        }
    }
    e = D_8010067C;
    do {
        e = e->prev;
    } while (e->owner != 0 && e->idx < -1);
    if (!(so->x->flags & 1)) {
        e->owner = (s32)so->x;
        e->idx = si;
        func_800BEB68(so->x->base + (si << 13), e->buf, 0x2000);
    } else if (si < 0) {
        a2 = ((so->x->size + 0x1FFF) >> 13) + 4;
        if (a2 > 0x800) {
            D_80100680.owner = (s32)so->x;
            D_80100680.idx = -1;
            func_800BEB68(so->x->base, D_80100680.buf, D_80100694);
            return &D_80100680;
        }
        e->owner = (s32)so->x;
        e->idx = si;
        func_800BEB68(so->x->base, e->buf, 0x2000);
    } else {
        e->owner = (s32)so->x;
        e->idx = si;
        a3 = ((u32 *)so->ent->buf)[si + 3];
        a2 = ((u32 *)so->ent->buf)[si + 4];
        n = a3 & 0xF;
        a3 >>= 4;
        a2 = (a2 >> 4) - a3;
        if (n == 0) {
            func_800BEB68(so->x->base + a3, e->buf, a2);
        } else {
            func_800BEB68(so->x->base + a3, D_801006F0, a2);
            D_8010065C = D_801006F0;
            D_80100660 = e->buf;
            func_800BDE38();
        }
    }
    if (sf) {
        mtf(e);
    }
    return e;
}
