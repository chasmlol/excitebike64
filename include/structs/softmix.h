/* The software mixer (softmix.c, ../softmix.c) and its "SAM" samples (decoder.c, DECODER.C). */
#ifndef STRUCTS_SOFTMIX_H
#define STRUCTS_SOFTMIX_H

#include "common.h"

/* The header of a "SAM" resource of the "SFX" set, followed by the sample data. */
typedef struct SamHeader {
    /* 0x00 */ u32 flags;           /* low 4 bits: format, 1 8-bit (companded), 2 unsupported, 3 IMA ADPCM,
                                       4 and 5 Microsoft ADPCM */
    /* 0x04 */ s32 length;          /* samples: bytes once decoded */
    /* 0x08 */ s32 loopStart;
    /* 0x0C */ s32 loopEnd;         /* == loopStart: the sample does not loop */
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} SamHeader; /* size 0x18 */

/* An ADPCM-coded SAM (formats 3 to 5), which decoder_decode_sample turns into an 8-bit one. */
typedef struct SamAdpcm {
    /* 0x00 */ SamHeader hdr;
    /* 0x18 */ s32 blockSamples;    /* samples per block, at most 0x400 */
    /* 0x1C */ s32 blockSize;       /* bytes from one block to the next */
    /* 0x20 */ u8 data[1];
} SamAdpcm;

/* A sample slot (sSoftmixSamples, 256 of them). */
typedef struct SoftmixSample {
    /* 0x00 */ u8 *data;
    /* 0x04 */ s32 length;          /* bytes; 0: the slot is free */
    /* 0x08 */ s32 loopStart;
    /* 0x0C */ s32 loopEnd;
    /* 0x10 */ s32 unk_10;          /* SamHeader.unk_10 */
    /* 0x14 */ SamHeader *sam;      /* the SAM resource the data is in */
    /* 0x18 */ s32 resId;           /* its resource id, or -1 when `data` was allocated (softmix_create_sample) */
} SoftmixSample; /* size 0x1C */

/* A voice (sSoftmixVoices, up to 8). The asm mixers (asm/game_softmix.s) read it through sMixVoices. */
typedef struct SoftmixVoice {
    /* 0x00 */ u8 *data;            /* the sample's data */
    /* 0x04 */ u32 flags;           /* 1: playing, 2: looped */
    /* 0x08 */ s32 position;        /* position and step are 32.32 fixed point */
    /* 0x0C */ u32 positionFrac;
    /* 0x10 */ s32 step;
    /* 0x14 */ u32 stepFrac;
    /* 0x18 */ s32 end;             /* the loop end when looped, else the length */
    /* 0x1C */ s32 loopStart;
    /* 0x20 */ u8 volume;           /* 0-255; also the mono level */
    /* 0x21 */ u8 pan;              /* 0-15 */
    /* 0x22 */ u8 left;             /* stereo levels from volume and pan */
    /* 0x23 */ u8 right;
    /* 0x24 */ s32 loopEnd;
    /* 0x28 */ s32 sample;          /* index in sSoftmixSamples */
    /* 0x2C */ s32 unk_2C;
} SoftmixVoice; /* size 0x30 */

#endif
