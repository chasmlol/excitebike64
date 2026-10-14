/* The sound effects layer (sfx.c, ../SFX.C) and the driver table it plays through (n64snd.c). */
#ifndef STRUCTS_SFX_H
#define STRUCTS_SFX_H

#include "common.h"

/* The driver's output modes (SndDriver.init, getCaps; also softmix.c's sSoftmixFlags). */
#define SFX_MODE_STEREO     1
#define SFX_MODE_16BIT      2

/* The highest output rate the driver mixes at, in Hz. */
#define SFX_MAX_RATE        22050

/* A driver voice's flags (voiceSetFlags, voiceGetFlags, SfxSound.voiceFlags, SoftmixVoice.flags). */
#define SFX_VOICE_PLAYING   1
#define SFX_VOICE_LOOPED    2

/* Pool sizes: sSfxSamples (as many as the driver's sample slots) and sSfxSoundPool. */
#define SFX_NUM_SAMPLES     0x100
#define SFX_NUM_SOUNDS      0x80

/* SfxSound.flags */
#define SFX_SOUND_STOP      1   /* stop requested: the sound gives up its voice and ends */
#define SFX_SOUND_ENDED     2   /* sfx_reap_sounds unlinks it */

/* SfxSound.endCallbackFlags, and the event endCallback is called with. */
#define SFX_EVENT_END       1

/* Fixed point: SfxSound.position and step are 24.8, fadeVolume and fadeStep 16.16 volumes. */
#define SFX_POS_SHIFT       8
#define SFX_FADE_SHIFT      16
#define SFX_FADE_ONE        0x10000

#define SFX_VOLUME_MAX      0x7F

/* The sound driver: gSfxDriver points at n64snd.c's table, gN64SndDriver (29 slots). sfx.c reaches the mixer only
   through it. */
typedef struct SndDriver {
    /* 0x00 */ s32 (*getCaps)(s32 *voices, s32 *modes, s32 *rate, s32 *frameMs);
    /* 0x04 */ s32 (*init)(s32 voices, s32 mode, s32 rate, s32 frameMs, s32 *frameSamples, s32 *outRate);
    /* 0x08 */ void (*shutdown)(void);
    /* 0x0C */ s32 (*loadSampleNamed)(u8 *name);
    /* 0x10 */ s32 (*loadSampleIndex)(s32 resId);
    /* 0x14 */ s32 (*createSample)(u8 *data, u32 length, s32 loopStart, s32 loopEnd, s32 rate);
    /* 0x18 */ void (*freeSample)(s32 sample);
    /* 0x1C */ void (*getSampleInfo)(s32 sample, s32 *info);    /* length, loop start, loop end, rate (Hz) */
    /* 0x20 */ void (*setSampleInfo)(s32 sample, s32 *info);
    /* 0x24 */ void (*writeSample)(s32 sample, u8 *data, u32 size);
    /* 0x28 */ void (*voiceSetSample)(s32 voice, s32 sample);
    /* 0x2C */ void (*voiceSetRate)(s32 voice, s32 rate);
    /* 0x30 */ void (*voiceSetVolume)(s32 voice, s32 volume);
    /* 0x34 */ void (*voiceSetPan)(s32 voice, s32 pan);
    /* 0x38 */ void (*voiceSetPosition)(s32 voice, u32 position);
    /* 0x3C */ u32 (*voiceGetPosition)(s32 voice);
    /* 0x40 */ void (*voiceSetFlags)(s32 voice, s32 flags);
    /* 0x44 */ s32 (*voiceGetFlags)(s32 voice);
    /* 0x48 */ s32 (*getSampleClock)(void);                     /* output samples mixed so far */
    /* 0x4C */ u32 (*getBufferClock)(void);                     /* audio buffers mixed so far */
    /* 0x50 */ void (*setCallback)(void (*fn)(void));           /* called before each buffer is mixed */
    /* 0x54 */ void (*echoInit)(s32 size);
    /* 0x58 */ void (*echoEnable)(s32 on);
    /* 0x5C */ void (*echoSetLevel)(s32 level);
    /* 0x60 */ void (*echoFree)(void);
    /* 0x64 */ void (*unk_64)(void);                            /* unk_64-unk_6C: empty in n64snd (n64snd_stub1-3) */
    /* 0x68 */ void (*unk_68)(void);
    /* 0x6C */ void (*unk_6C)(void);
    /* 0x70 */ void (*setMute)(s32 on);
} SndDriver; /* size 0x74 */

/* A sample of sSfxSamples (256, indexed like the driver's), reference counted. */
typedef struct SfxSample {
    /* 0x00 */ u8 *name;            /* copy of the name it was loaded by (sfx_load_sample_name), else NULL */
    /* 0x04 */ s32 length;          /* the driver's sample info: */
    /* 0x08 */ s32 loopStart;
    /* 0x0C */ s32 loopEnd;
    /* 0x10 */ s32 unk_10;          /* the sample's own rate in Hz (what the game passes to sfx_play) */
    /* 0x14 */ s32 refCount;        /* -1: locked, never freed (sfx_lock_sample) */
} SfxSample; /* size 0x18 */

/* A playing sound (sSfxSoundPool, 128 of them), linked in sSfxSounds by falling priority. The first `numVoices` that
   are not stopping get a driver voice. */
typedef struct SfxSound {
    /* 0x00 */ struct SfxSound *prev;   /* -1 when the pool entry is free */
    /* 0x04 */ struct SfxSound *next;
    /* 0x08 */ s32 voice;           /* driver voice, -1 while it has none */
    /* 0x0C */ s32 wantsVoice;      /* set while it is among the sounds that get one */
    /* 0x10 */ s32 handle;          /* what sfx_play returned; the other sfx_* functions find the sound by it */
    /* 0x14 */ s32 clock;           /* the driver's sample clock at the last update */
    /* 0x18 */ s32 sample;
    /* 0x1C */ s32 rate;            /* Hz; 0 while paused */
    /* 0x20 */ s32 pausedRate;
    /* 0x24 */ s32 volume;          /* 0-0x7F */
    /* 0x28 */ s32 pan;
    /* 0x2C */ s32 priority;
    /* 0x30 */ s32 voiceFlags;      /* SFX_VOICE_*, as the driver reports them */
    /* 0x34 */ s32 fadeStep;        /* added to `fadeVolume` each update */
    /* 0x38 */ s32 fadeVolume;      /* volume << 16 */
    /* 0x3C */ void (*endCallback)(s32 handle, s32 event);
    /* 0x40 */ s32 endCallbackFlags;    /* SFX_EVENT_END: call endCallback when the sound ends */
    /* 0x44 */ u32 position;        /* 24.8, kept while the sound has no voice */
    /* 0x48 */ s32 step;            /* position step per output sample, 24.8 */
    /* 0x4C */ u32 flags;           /* SFX_SOUND_STOP, SFX_SOUND_ENDED */
} SfxSound; /* size 0x50 */

/* A streamed sound (sfx_stream_create), linked in sSfxStreams: a ring buffer the game writes to and two driver
   samples played in turn, each refilled from the ring while the other plays. */
typedef struct SfxStream {
    /* 0x00 */ struct SfxStream *next;
    /* 0x04 */ s32 handle;          /* the sound playing, -1: none */
    /* 0x08 */ s32 samples[2];      /* -1: none */
    /* 0x10 */ u8 filled[2];        /* samples[i] holds data not played yet */
    /* 0x12 */ u8 unk_12[2];
    /* 0x14 */ u32 size;            /* of the ring buffer */
    /* 0x18 */ u8 *buf;
    /* 0x1C */ u32 readPos;
    /* 0x20 */ u32 writePos;
    /* 0x24 */ u32 count;           /* bytes in the ring */
    /* 0x28 */ s32 rate;
    /* 0x2C */ u32 chunkSize;       /* bytes per driver buffer at `rate` */
    /* 0x30 */ u8 volume;
    /* 0x31 */ u8 pan;
    /* 0x32 */ u8 priority;
    /* 0x33 */ u8 cur;              /* the sample that plays next */
} SfxStream; /* size 0x34 */

#endif
