#include "ctx.h"

typedef struct {
    s32 f00[5];
    u32 size;
    u8 *buf;
    s32 f1C;
    u32 pos;
    u32 total;
} Ring;

void func_8000EE20(void);
void func_8000EE28(void);

static inline void copy(u8 *d, u8 *s, u32 len)
{
    for (; len != 0; len--) {
        *d++ = *s++;
    }
}

void func_800B9B4C(Ring *r, u32 n, u8 *src)
{
    u32 t;

    func_8000EE20();
    if (r->size < r->pos + n) {
        t = r->size - r->pos;
        copy(r->buf + r->pos, src, t);
        copy(r->buf, src + t, n - t);
        r->pos = n - t;
    } else {
        copy(r->buf + r->pos, src, n);
        r->pos += n;
    }
    r->pos %= r->size;
    r->total += n;
    func_8000EE28();
}
