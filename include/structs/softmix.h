/* The software mixer (softmix.c, ../softmix.c) and its "SAM" samples (decoder.c, DECODER.C). */
#ifndef STRUCTS_SOFTMIX_H
#define STRUCTS_SOFTMIX_H

#include "common.h"

/* Mixer limits: voices, and sample slots (sSoftmixSamples). The output modes and voice flags are the driver's,
   SFX_MODE_* and SFX_VOICE_* (sfx.h). */
#define SOFTMIX_MAX_VOICES          8
#define SOFTMIX_NUM_SAMPLES         0x100

/* sSoftmixVolumeTable (sMixVolumeTable): SOFTMIX_VOLUME_ROWS rows of SOFTMIX_VOLUME_ROW_SIZE s32, row r holding each
   sample byte's value times r / SOFTMIX_VOLUME_ROW_MAX. */
#define SOFTMIX_VOLUME_ROWS         0x20
#define SOFTMIX_VOLUME_ROW_MAX      0x1f
#define SOFTMIX_VOLUME_ROW_SIZE     0x100
#define SOFTMIX_VOLUME_TABLE_SIZE   0x8000

/* SoftmixVoice.pan: 0 left to 15 right. */
#define SOFTMIX_PAN_MAX             15

/* SamHeader.flags */
#define SAM_FORMAT_MASK             0xF
#define SAM_FORMAT_PCM8             1   /* 8-bit companded: what the others are decoded to */
#define SAM_FORMAT_IMA_ADPCM        3
#define SAM_FORMAT_MSADPCM          4   /* Microsoft ADPCM */
#define SAM_FORMAT_MSADPCM_2        5   /* decoded like 4 */

/* The most samples an ADPCM block decodes to (SamAdpcm.blockSamples). */
#define SAM_MAX_BLOCK_SAMPLES       0x400

/* The header of a "SAM" resource of the "SFX" set, followed by the sample data. */
typedef struct SamHeader {
    /* 0x00 */ u32 flags;           /* low 4 bits: format, SAM_FORMAT_* (2 is not supported) */
    /* 0x04 */ s32 length;          /* samples: bytes once decoded */
    /* 0x08 */ s32 loopStart;
    /* 0x0C */ s32 loopEnd;         /* == loopStart: the sample does not loop */
    /* 0x10 */ s32 rate;            /* Hz */
    /* 0x14 */ s32 unk_14;
} SamHeader; /* size 0x18 */

/* An ADPCM-coded SAM (formats 3 to 5), which decoder_decode_sample turns into an 8-bit one. */
typedef struct SamAdpcm {
    /* 0x00 */ SamHeader hdr;
    /* 0x18 */ s32 blockSamples;    /* samples per block, at most SAM_MAX_BLOCK_SAMPLES */
    /* 0x1C */ s32 blockSize;       /* bytes from one block to the next */
    /* 0x20 */ u8 data[1];
} SamAdpcm;

/* A sample slot (sSoftmixSamples, 256 of them). */
typedef struct SoftmixSample {
    /* 0x00 */ u8 *data;
    /* 0x04 */ s32 length;          /* bytes; 0: the slot is free */
    /* 0x08 */ s32 loopStart;
    /* 0x0C */ s32 loopEnd;
    /* 0x10 */ s32 rate;            /* Hz (SamHeader.rate) */
    /* 0x14 */ SamHeader *sam;      /* the SAM resource the data is in */
    /* 0x18 */ s32 resId;           /* its resource id, or -1 when `data` was allocated (softmix_create_sample) */
} SoftmixSample; /* size 0x1C */

/* A voice (sSoftmixVoices, up to 8). The asm mixers (asm/game_softmix.s) read it through sMixVoices. */
typedef struct SoftmixVoice {
    /* 0x00 */ u8 *data;            /* the sample's data */
    /* 0x04 */ u32 flags;           /* SFX_VOICE_PLAYING, SFX_VOICE_LOOPED */
    /* 0x08 */ s32 position;        /* position and step are 32.32 fixed point */
    /* 0x0C */ u32 positionFrac;
    /* 0x10 */ s32 step;
    /* 0x14 */ u32 stepFrac;
    /* 0x18 */ s32 end;             /* the loop end when looped, else the length */
    /* 0x1C */ s32 loopStart;
    /* 0x20 */ u8 volume;           /* 0-255; also the mono level */
    /* 0x21 */ u8 pan;              /* 0-SOFTMIX_PAN_MAX */
    /* 0x22 */ u8 left;             /* stereo levels from volume and pan */
    /* 0x23 */ u8 right;
    /* 0x24 */ s32 loopEnd;
    /* 0x28 */ s32 sample;          /* index in sSoftmixSamples */
    /* 0x2C */ s32 unk_2C;
} SoftmixVoice; /* size 0x30 */

#endif
