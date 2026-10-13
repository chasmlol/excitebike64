#include "ctx.h"

typedef struct {
    u32 flags;
    s32 len;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
} Hdr;

typedef struct {
    Hdr h;
    s32 chunk;
    s32 stride;
    u8 data[1];
} Src;

extern u8 D_800D3604[];
extern u8 D_800DB4A0[];
extern void (*D_800D5710)(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
Hdr *func_800BC908(u32 sz, s32 z, u8 *tag, s32 line);
void func_800DBF94(u8 *a, u16 *b, s32 c, u8 *d);

Hdr *func_800C0350(Src *s)
{
    u8 st[8];
    u16 buf[1024];
    s32 left;
    s32 n;
    Hdr *out;
    u8 *dst;
    u8 *in;
    u16 *bp;

    if (s->chunk > 0x400) {
        return NULL;
    }
    left = s->h.len;
    out = func_800BC908(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 0x1DE);
    if (out == NULL) {
        return NULL;
    }
    *out = s->h;
    out->flags = (out->flags & ~0xF) | 1;
    func_8000ED10(st, 0, 4);
    in = s->data;
    dst = (u8 *)(out + 1);
    while (left > 0) {
        n = s->chunk;
        if (left < n) {
            n = left;
        }
        left -= n;
        if (D_800D5710 != NULL) {
            D_800D5710();
        }
        func_800DBF94(in, buf, n, st);
        in += s->stride;
        bp = buf;
        while (--n != -1) {
            *dst++ = D_800D3604[*bp++ >> 3];
        }
    }
    dst[-1] = 0;
    return out;
}
