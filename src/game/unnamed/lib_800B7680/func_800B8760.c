#include "ctx.h"

typedef struct Node {
    struct Node *f00;
    struct Node *next;
    s32 f08;
    s32 f0c;
    s32 id;
    u8 pad[0x38];
    s32 flags;
} Node;

typedef struct Ring {
    s32 f00;
    s32 cur;        /* 0x04 */
    s32 slot[2];    /* 0x08 */
    u8 flag[2];     /* 0x10 */
    u8 f12[2];      /* 0x12 */
    u32 size;       /* 0x14 */
    u8 *buf;        /* 0x18 */
    u32 pos;        /* 0x1c */
    u32 f20;        /* 0x20 */
    u32 avail;      /* 0x24 */
    s32 arg28;      /* 0x28 */
    u32 len;        /* 0x2c */
    u8 b30;
    u8 b31;
    u8 b32;
    u8 sel;         /* 0x33 */
} Ring;

extern s32 D_800D33F0;
extern Node *D_800D33E8;
extern u8 D_800F8840[];

void func_8000EE20(void);
void func_8000EE28(void);
void func_800B8E70(s32 a, u8 *b, u32 n);
s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);

static __inline__ Node *find_node(s32 id)
{
    Node *n;

    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == id) {
            func_8000EE28();
            return n;
        }
    }
    func_8000EE28();
    return 0;
}

static __inline__ void copy_bytes(u8 *dst, u8 *src, u32 n)
{
    while (n != 0) {
        *dst++ = *src++;
        n--;
    }
}

static __inline__ u32 ring_read(Ring *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->avail < n) {
fail:
        n = 0;
        goto done;
    }
    if (n == 0) goto fail;
    if (r->size < r->pos + n) {
        k = r->size - r->pos;
        copy_bytes(dst, r->buf + r->pos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->pos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->pos, n);
        r->pos += n;
    }
    r->pos = r->pos % r->size;
    r->avail -= n;
done:
    return n;
}

static __inline__ u32 ring_read2(Ring *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->avail < n) goto fail;
    if (n != 0) goto ok;
fail:
    n = 0;
    goto done;
ok:
    if (r->size < r->pos + n) {
        k = r->size - r->pos;
        copy_bytes(dst, r->buf + r->pos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->pos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->pos, n);
        r->pos += n;
    }
    r->pos = r->pos % r->size;
    r->avail -= n;
done:
    return n;
}

void func_800B8760(Ring *r)
{
    s32 id;
    Node *nd;
    u32 got;

    if (r->slot[0] == -1) {
        return;
    }
    if (r->cur != -1) {
        id = r->cur;
        if (D_800D33F0 != 0) {
            nd = find_node(id);
            if (nd != 0) {
                nd->flags |= 1;
            }
        }
        r->cur = -1;
    }
    r->flag[r->sel] = 0;
    if (r->flag[1 - r->sel] == 0) {
        if ((got = ring_read(r, D_800F8840, r->len)) != 0) {
            func_800B8E70(r->slot[1 - r->sel], D_800F8840, got);
            r->flag[1 - r->sel] = 1;
        }
    }
    if (r->flag[1 - r->sel] != 0) {
        r->cur = func_800B7990(r->slot[1 - r->sel], r->arg28, r->b30, r->b31, r->b32, 0);
    }
    if ((got = ring_read2(r, D_800F8840, r->len)) != 0) {
        func_800B8E70(r->slot[r->sel], D_800F8840, got);
        r->flag[r->sel] = 1;
    }
    r->sel = 1 - r->sel;
}
