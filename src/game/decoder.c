#include "game.h"

SamHeader *decoder_decode_adpcm(SamAdpcm *s)
{
    u8 st[8];
    u16 buf[1024];
    s32 left;
    s32 n;
    SamHeader *out;
    u8 *dst;
    u8 *in;
    u16 *bp;

    if (s->blockSamples > 0x400) {
        return NULL;
    }
    left = s->hdr.length;
    out = mem_alloc_dbg(((left + 3) & ~3) + sizeof(SamHeader), 0, sStrDecoderC, 0x1DE);
    if (out == NULL) {
        return NULL;
    }
    *out = s->hdr;
    out->flags = (out->flags & ~0xF) | 1;
    mem_fill(st, 0, 4);
    in = s->data;
    dst = (u8 *)(out + 1);
    while (left > 0) {
        n = s->blockSamples;
        if (left < n) {
            n = left;
        }
        left -= n;
        if (gDmaIdleCallback != NULL) {
            gDmaIdleCallback();
        }
        decoder_decode_msadpcm_block(in, buf, n, st);
        in += s->blockSize;
        bp = buf;
        while (--n != -1) {
            *dst++ = sDecoderCompandTable[*bp++ >> 3];
        }
    }
    dst[-1] = 0;
    return out;
}

SamHeader *decoder_decode_sample(SamHeader *sam)
{
    switch (sam->flags & 0xF) {
    case 1:
        return sam;
    case 2:
        return NULL;
    case 3:
        return decoder_decode_ima_adpcm((SamAdpcm *)sam);
    case 4:
    case 5:
        return decoder_decode_adpcm((SamAdpcm *)sam);
    default:
        return NULL;
    }
}

SamHeader *decoder_decode_ima_adpcm(SamAdpcm *p)
{
    SamHeader *r;
    u8 *d;
    u8 *src;
    s32 left, cnt;
    u16 *bp, *pb;
    volatile s32 pad[2];
    u16 buf[1024];

    if (p->blockSamples > 0x400) {
        return 0;
    }
    left = p->hdr.length;
    r = mem_alloc_dbg(((left + 3) & ~3) + sizeof(SamHeader), 0, sStrDecoderC, 381);
    src = p->data;
    if (r == 0) {
        return 0;
    }
    d = (u8 *)(r + 1);
    *r = p->hdr;
    r->flags = (r->flags & ~0xF) | 1;
    if (left > 0) {
        bp = buf;
        do {
        cnt = p->blockSamples;
        if (left < cnt) {
            cnt = left;
        }
        left -= cnt;
        decoder_decode_ima_block(src, bp, cnt);
        src += p->blockSize;
        pb = bp;
        while (--cnt != -1) {
            *d++ = sDecoderCompandTable[*pb++ >> 3];
        }
        } while (left > 0);
    }
    d[-1] = 0;
    return r;
}
