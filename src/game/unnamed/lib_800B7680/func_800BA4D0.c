#include "ctx.h"

typedef struct {
    s32 f00[5];
    u32 size;
    u8 *buf;
    u32 head;
    s32 f20;
    u32 count;
} Ring;

static inline void copy(u8 *d, u8 *s, u32 len)
{
    for (; len != 0; len--) {
        *d++ = *s++;
    }
}

u32 func_800BA4D0(Ring *r, u32 n, u8 *dst)
{
    u32 t;

    if (r->count < n) {
        return 0;
    }
    if (n == 0) {
        return 0;
    }
    if (r->size < r->head + n) {
        t = r->size - r->head;
        copy(dst, r->buf + r->head, t);
        copy(dst + t, r->buf, n - t);
        r->head = n - t;
    } else {
        copy(dst, r->buf + r->head, n);
        r->head += n;
    }
    r->head %= r->size;
    r->count -= n;
    return n;
}
