#include "ctx.h"

typedef struct {
    u32 flags;
    s32 len;
    s32 f08, f0C, f10, f14;
} Hdr;

typedef struct {
    Hdr hdr;
    s32 chunk;
    s32 stride;
    u8 data[1];
} Src;

typedef struct {
    Hdr hdr;
    u8 text[1];
} Out;

extern u8 D_800D3604[];
extern char D_800DB4A0[];
void *func_800BC908(u32 size, s32 zero, char *file, s32 line);
void func_800DBE94(u8 *src, u16 *dst, s32 count);

Out *func_800C055C(Src *p)
{
    Out *r;
    u8 *d;
    u8 *src;
    s32 left, cnt;
    u16 *bp, *pb;
    volatile s32 pad[2];
    u16 buf[1024];

    if (p->chunk > 0x400) {
        return 0;
    }
    left = p->hdr.len;
    r = func_800BC908(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 381);
    src = p->data;
    if (r == 0) {
        return 0;
    }
    d = r->text;
    r->hdr = p->hdr;
    r->hdr.flags = (r->hdr.flags & ~0xF) | 1;
    if (left > 0) {
        bp = buf;
        do {
        cnt = p->chunk;
        if (left < cnt) {
            cnt = left;
        }
        left -= cnt;
        func_800DBE94(src, bp, cnt);
        src += p->stride;
        pb = bp;
        while (--cnt != -1) {
            *d++ = D_800D3604[*pb++ >> 3];
        }
        } while (left > 0);
    }
    d[-1] = 0;
    return r;
}
