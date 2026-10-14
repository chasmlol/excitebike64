#include "game.h"

struct Hdr_14 {
    u32 flags;
    s32 len;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
};

struct Src_4 {
    Hdr_14 h;
    s32 chunk;
    s32 stride;
    u8 data[1];
};

Hdr_14 *decoder_decode_adpcm(Src_4 *s)
{
    u8 st[8];
    u16 buf[1024];
    s32 left;
    s32 n;
    Hdr_14 *out;
    u8 *dst;
    u8 *in;
    u16 *bp;

    if (s->chunk > 0x400) {
        return NULL;
    }
    left = s->h.len;
    out = mem_alloc_dbg(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 0x1DE);
    if (out == NULL) {
        return NULL;
    }
    *out = s->h;
    out->flags = (out->flags & ~0xF) | 1;
    mem_fill(st, 0, 4);
    in = s->data;
    dst = (u8 *)(out + 1);
    while (left > 0) {
        n = s->chunk;
        if (left < n) {
            n = left;
        }
        left -= n;
        if (gDmaIdleCallback != NULL) {
            gDmaIdleCallback();
        }
        decoder_decode_msadpcm_block(in, buf, n, st);
        in += s->stride;
        bp = buf;
        while (--n != -1) {
            *dst++ = sDecoderCompandTable[*bp++ >> 3];
        }
    }
    dst[-1] = 0;
    return out;
}

u32 *decoder_decode_sample(u32 *param_1)
{
    switch (*param_1 & 0xF) {
    case 1:
        return param_1;
    case 2:
        return NULL;
    case 3:
        return func_800C055C(param_1);
    case 4:
    case 5:
        return decoder_decode_adpcm(param_1);
    default:
        return NULL;
    }
}

typedef struct {
    u32 flags;
    s32 len;
    s32 f08, f0C, f10, f14;
} Hdr_15;

struct Src_5 {
    Hdr_15 hdr;
    s32 chunk;
    s32 stride;
    u8 data[1];
};

struct Out_6 {
    Hdr_15 hdr;
    u8 text[1];
};

Out_6 *func_800C055C(Src_5 *p)
{
    Out_6 *r;
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
    r = mem_alloc_dbg(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 381);
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
        decoder_decode_ima_block(src, bp, cnt);
        src += p->stride;
        pb = bp;
        while (--cnt != -1) {
            *d++ = sDecoderCompandTable[*pb++ >> 3];
        }
        } while (left > 0);
    }
    d[-1] = 0;
    return r;
}
