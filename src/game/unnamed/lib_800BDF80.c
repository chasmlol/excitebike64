#include "game.h"

typedef struct Node_49 {
    struct Node_49 *next;
    struct Node_49 *prev;
    u8 *buf;
    s32 c;
    s32 d;
} Node_49;

extern Node_49 *D_80100678;
extern Node_49 *D_8010067C;
extern Node_49 D_80100680;

typedef struct {
    u8 pad[0x10];
    u32 flags;
    s32 f14;
    s32 f18;
    u32 f1C;
} Rec_59;

extern Rec_59 *D_801006A4_2;

s32 func_800BDF80(u8 *src)
{
    Node_49 *p;
    Node_49 *last;
    u8 *mem;
    u8 *buf;
    s32 off;
    s32 i;
    u32 n;
    s32 size;
    u32 max;
    u32 v;
    u32 pages;
    s32 k;

    if (D_80100670 == 0) {
        D_80100674 = mem = func_800BC908((D_800D5700 + 2) * sizeof(Node_49) + (off = (D_800D5700 + 1) << 13), 0, (s32)D_800DB410, 0xC2);
        if (mem == NULL) {
            return -1;
        }
        buf = mem - 0x2000;
        p = (Node_49 *)(mem + off);
        i = 0;
        D_80100678 = p;
        for (; i < D_800D5700 + 2U; i++) {
            p->next = p + 1;
            p->prev = p - 1;
            p->buf = buf;
            p->c = 0;
            p->d = -1;
            p++;
            buf += 0x2000;
        }
        D_8010067C = last = p - 1;
        D_8010069C = src;
        D_801006F0 = last->buf;
        D_80100678->prev = NULL;
        last->next = NULL;
        D_80100678->buf = NULL;
        last->buf = NULL;
        func_800BEB68(src, D_801006F0, 0x2000);
        n = *(u32 *)D_801006F0;
        D_8010069C += 4;
        D_80100698 = n;
        size = n * 32;
        D_801006A0 = (s32)D_8010069C + size;
        if (size + 4 > 0x2000U) {
            func_800BCB04(D_80100674);
            D_80100674 = NULL;
            return -1;
        }
        D_801006A4_2 = func_800BC908(size, 0, (s32)D_800DB410, 0x100);
        memcpy(D_801006A4_2, D_801006F0 + 4, size);
        max = 0;
        i = 0;
        if (D_80100698 != 0) {
        do {
            func_800BEB68((void *)(D_801006A4_2[i].f18 += D_801006A0), D_801006F0, 0x10);
            if (D_801006F0[0] == 's' && D_801006F0[1] == 'W' && D_801006F0[2] == 'd' && D_801006F0[3] == 0xB0) {
                D_801006A4_2[i].flags |= 1;
                v = (D_801006F0[8] << 24) | (D_801006F0[9] << 16) | (D_801006F0[10] << 8) | D_801006F0[11];
                D_801006A4_2[i].f1C = v;
                if (max < v) {
                    max = v;
                }
            } else {
                D_801006A4_2[i].f1C = D_801006A4_2[i].f14;
            }
        i++;
        } while (i < D_80100698);
        }
        i = (max + 0x1FFF) >> 13;
        max = i + 4;
        D_80100694 = 0;
        func_8000ED10((u8 *)&D_80100680, 0, sizeof(Node_49));
        if (max > 0x800) {
            D_80100694 = (i + 5) * 4;
            D_80100680.buf = func_800BC908(D_80100694, 0, (s32)D_800DB410, 0x13D);
            if (D_80100680.buf == NULL) {
                return -1;
            }
            D_80100680.c = 0;
            D_80100680.d = -1;
        }
        for (i = 3; i >= 0; i--) {
            D_801006B0[i].a = 0;
        }
        D_80100670 = 1;
    }
    return 0;
}

static inline void mtf(Ent_106 *e)
{
    Ent_106 *h;

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

static inline Ent_106 *find(Obj_81 *so)
{
    Ent_106 *cur, *nx;
    s32 want;

    want = -1;
    if (D_80100680_2.owner == (s32)so->x) {
        return &D_80100680_2;
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
    if (cur != &D_80100680_2) {
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

Ent_106 *func_800BE2D0(Obj_81 *so, s32 si, s8 sf)
{
    Ent_106 *e;
    u32 n;
    u32 a3, a2;

    if (so->x->flags != 0 && si >= 0) {
        if (so->ent == NULL || (so->ent == &D_80100680_2 && D_80100680_2.owner != (s32)so->x)) {
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
            D_80100680_2.owner = (s32)so->x;
            D_80100680_2.idx = -1;
            func_800BEB68(so->x->base, D_80100680_2.buf, D_80100694);
            return &D_80100680_2;
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

s32 func_800BE5D8(void)
{
    s32 i;
    s32 *p;
    s32 *base;

    if (D_80100670 == 0) {
        return 0;
    }
    i = 3;
    base = D_801006B0;
    p = base + 12;
    do {
        *p = 0;
        i = i - 1;
        p = p - 4;
    } while (i >= 0);
    if (D_80100680_3[2] != 0) {
        func_800BCB04((void *)D_80100680_3[2]);
    }
    D_80100694 = 0;
    func_8000ED10((u8 *)D_80100680_3, 0, 0x14);
    func_800BCB04((void *)D_801006A4);
    func_800BCB04((void *)D_80100674);
    D_801006A4 = 0;
    D_80100674 = 0;
    D_80100670 = 0;
    return 0;
}

struct Slot_17 {
    u8 *ptr;
    s32 a;
    s32 b;
    s32 c;
};

Slot_17 *func_800BE694(u8 *param_1, char *arg1)
{
    u32 n = 0;
    s32 j;
    Slot_17 *s;
    Slot_17 *base;
    u8 *p = D_801006A4;

    if (D_80100698 != 0) {
        do {
            if (func_8000EDA0(p, param_1) == 0) {
                break;
            }
            p += 0x20;
            n++;
        } while (n < D_80100698);
    }
    if (n != D_80100698) {
        j = 0;
        base = D_801006B0;
        s = base;
        for (; j < 4; j++) {
            if (s->ptr == 0) {
                break;
            }
            s++;
        }
        if (j != 4) {
            base[j].ptr = p;
            base[j].a = 0;
            base[j].b = 0;
            base[j].c = 0;
            return &base[j];
        }
    }
    return 0;
}

void func_800BE770(Q_2 *q)
{
    Node_50 *n;
    Node_50 *h;
    Node_50 *hp;

    if (q->a == 0) {
        return;
    }
    n = q->b;
    if (n != 0 && n != &D_80100680) {
        n->cnt = n->cnt + 1;
        h = D_80100678->next;
        if (n == h) {
            q->a = 0;
            q->b = 0;
            q->c = 0;
            q->d = 0;
            return;
        }
        n->prev->next = n->next;
        n->next->prev = n->prev;
        n->next = h;
        hp = h->prev;
        n->prev = hp;
        hp->next = n;
        h->prev = n;
    }
    q->a = 0;
    q->b = 0;
    q->c = 0;
    q->d = 0;
}

void func_800BE7F4(s32 *o, s32 v, s32 mode) {
    s32 t;
    if (mode == 1) {
        t = o[3];
    } else {
        if (mode == 0) {
            o[3] = v;
            return;
        }
        if (mode != 2) {
            return;
        }
        t = ((s32 *)o[0])[7];
    }
    o[3] = v + t;
}

s32 func_800BE83C(s32 *p)
{
    return p[3];
}

typedef struct { u8 pad[0x1C]; u32 size; } FileHdr;
struct Stream_2 { FileHdr *hdr; s32 f4; s32 f8; u32 pos; };
typedef struct { s32 f0; s32 f4; u8 *data; } Block_2;

s32 func_800BE848(u8 *buf, s32 size, s32 count, Stream_2 *f)
{
    s32 n = count * size;
    u32 blk;
    u32 off;
    s32 chunk;
    Block_2 *b;
    u8 *dst;

    if (f->hdr->size < f->pos + n) {
        do { do { do {
            n = f->hdr->size - f->pos;
        } while (0); } while (0); } while (0);
        if (size > 1) {
            do {
                count = n / size;
                n = count * size;
            } while (0);
        }
    }
    dst = buf;
    blk = f->pos >> 13;
    off = f->pos & 0x1FFF;
    while (n != 0) {
        chunk = 0x2000 - off;
        if (n < chunk) {
            do { do { do {
                chunk = n;
            } while (0); } while (0); } while (0);
        }
        b = func_800BEA40(f, blk, chunk != 0x2000);
        if (b == NULL) {
            return 0;
        }
        memcpy(dst, b->data + off, chunk);
        n -= chunk;
        dst += chunk;
        do { do {
            f->pos += chunk;
            off += chunk;
        } while (0); } while (0);
        if (off == 0x2000) {
            off = 0;
            blk++;
        }
    }
    return count;
}

s32 func_800BE998(s32 *a0)
{
    u8 *p;
    u32 n;

    p = (u8 *)a0[0];
    n = (u32)a0[3];
    return *(u32 *)(p + 0x1C) <= n;
}

u8 *func_800BE9B0(u8 *param_1)
{
    u32 i;
    u8 *p;

    i = 0;
    p = D_801006A4;
    if (D_80100698 != 0) {
        do {
            if (func_8000EDA0(p, param_1) == 0) {
                break;
            }
            p += 0x20;
        } while (++i < D_80100698);
    }
    if (i == D_80100698) {
        return NULL;
    }
    return p;
}

Node_51 *func_800BEA40(s32 *k, s32 id, s8 flag)
{
    Node_51 *p;
    Node_51 *nx;

    if (id < 0) {
        p = &D_80100680;
        if (p->key == *k) {
            return p;
        }
    }
    nx = D_80100678->next;
    for (;;) {
        p = nx;
        nx = p->next;
        if (nx == NULL) {
            p = func_800BE2D0(k, id, 0);
            if (p == NULL) {
                return NULL;
            }
        }
        if (p->key != *k) {
            continue;
        }
        if (id < 0 && p->cnt < 0) {
            p->cnt--;
            break;
        }
        if (p->cnt == id) {
            break;
        }
    }
    if (flag != 0 && p != &D_80100680) {
        nx = D_80100678->next;
        if (p != nx) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = nx;
            p->prev = nx->prev;
            p->prev->next = p;
            nx->prev = p;
        }
    }
    return p;
}

void func_800BEB68(void *param_1, void *param_2, s32 param_3)
{
    s32 r;
    osInvalDCache(param_2, param_3);
    if ((D_8010069C_2 & 0xF0000000) == 0x80000000) {
        memcpy(param_2, param_1, param_3);
    } else {
        do {
            r = func_800BEC20(D_801009A4, 0, param_1, param_2, param_3);
        } while (r < 0);
        func_800BEFB4(r);
        osInvalDCache(param_2, param_3);
    }
}
