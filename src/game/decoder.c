#include "game.h"

/* Decodes a Microsoft ADPCM sample (formats 4 and 5) into a new 8-bit companded one (SAM_FORMAT_PCM8) with the same
   header. Returns NULL when its blocks are too long or there is no memory. */
SamHeader *decoder_decode_adpcm(SamAdpcm *sam)
{
    u8 state[8];
    u16 pcm[SAM_MAX_BLOCK_SAMPLES];
    s32 left;
    s32 n;
    SamHeader *out;
    u8 *dst;
    u8 *src;
    u16 *pcmp;

    if (sam->blockSamples > SAM_MAX_BLOCK_SAMPLES) {
        return NULL;
    }
    left = sam->hdr.length;
    out = mem_alloc_dbg(((left + 3) & ~3) + sizeof(SamHeader), 0, sStrDecoderC, 0x1DE);
    if (out == NULL) {
        return NULL;
    }
    *out = sam->hdr;
    out->flags = (out->flags & ~SAM_FORMAT_MASK) | SAM_FORMAT_PCM8;
    mem_fill(state, 0, 4);
    src = sam->data;
    dst = (u8 *)(out + 1);
    while (left > 0) {
        n = sam->blockSamples;
        if (left < n) {
            n = left;
        }
        left -= n;
        if (gDmaIdleCallback != NULL) {
            gDmaIdleCallback();
        }
        decoder_decode_msadpcm_block(src, pcm, n, state);
        src += sam->blockSize;
        pcmp = pcm;
        /* the top 13 bits of each 16-bit sample pick its companded byte */
        while (--n != -1) {
            *dst++ = sDecoderCompandTable[*pcmp++ >> 3];
        }
    }
    /* the sample ends on 0 */
    dst[-1] = 0;
    return out;
}

/* The decompressor softmix_sam_decompress calls: an 8-bit sample is returned as it is, an ADPCM one decoded into a
   new 8-bit one; NULL for format 2 and unknown formats. */
SamHeader *decoder_decode_sample(SamHeader *sam)
{
    switch (sam->flags & SAM_FORMAT_MASK) {
    case SAM_FORMAT_PCM8:
        return sam;
    case 2:
        return NULL;
    case SAM_FORMAT_IMA_ADPCM:
        return decoder_decode_ima_adpcm((SamAdpcm *)sam);
    case SAM_FORMAT_MSADPCM:
    case SAM_FORMAT_MSADPCM_2:
        return decoder_decode_adpcm((SamAdpcm *)sam);
    default:
        return NULL;
    }
}

/* Decodes an IMA ADPCM sample (format 3) into a new 8-bit companded one, like decoder_decode_adpcm. */
SamHeader *decoder_decode_ima_adpcm(SamAdpcm *sam)
{
    SamHeader *out;
    u8 *dst;
    u8 *src;
    s32 left, n;
    u16 *block, *pcmp;
    volatile s32 pad[2];        /* unused: places pcm in the frame as the original did */
    u16 pcm[SAM_MAX_BLOCK_SAMPLES];

    if (sam->blockSamples > SAM_MAX_BLOCK_SAMPLES) {
        return 0;
    }
    left = sam->hdr.length;
    out = mem_alloc_dbg(((left + 3) & ~3) + sizeof(SamHeader), 0, sStrDecoderC, 381);
    src = sam->data;
    if (out == 0) {
        return 0;
    }
    dst = (u8 *)(out + 1);
    *out = sam->hdr;
    out->flags = (out->flags & ~SAM_FORMAT_MASK) | SAM_FORMAT_PCM8;
    if (left > 0) {
        block = pcm;
        do {
            n = sam->blockSamples;
            if (left < n) {
                n = left;
            }
            left -= n;
            decoder_decode_ima_block(src, block, n);
            src += sam->blockSize;
            pcmp = block;
            while (--n != -1) {
                *dst++ = sDecoderCompandTable[*pcmp++ >> 3];
            }
        } while (left > 0);
    }
    dst[-1] = 0;
    return out;
}
