/* Streamed audio (tunes.c): music, the announcer and the crowd, ADPCM clips streamed from "BIG.RAW". */
#ifndef STRUCTS_TUNES_H
#define STRUCTS_TUNES_H

#include "common.h"

/* The clip table of the stream data (sStreamSampleTable): the number of clips, then 5 words per clip (key, offset,
   size, flags, tag). The game indexes it in 5-word records from its start, so record i holds clip i's key, offset,
   size and flags after a first word that is the clip count in record 0 and clip i - 1's tag in the others. */
typedef struct StreamSample {
    /* 0x00 */ u32 count;           /* record 0: the number of clips; record i: the tag of clip i - 1 */
    /* 0x04 */ u32 key;             /* how a song names the clip (tunes_play_song) */
    /* 0x08 */ s32 offset;          /* from sStreamDataBase */
    /* 0x0C */ s32 size;            /* bytes: 0x44 per block of 128 samples */
    /* 0x10 */ u32 flags;           /* 1: played at half rate */
} StreamSample; /* size 0x14 */

/* A stream channel (sStreamChannels, 4): 0 and 1 music, 2 the announcer, 3 ambience and the crowd. */
typedef struct StreamChannel {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 dmaHandle;       /* -1: no DMA running */
    /* 0x08 */ u8 *dmaBuf;          /* 0x220 bytes: 8 ADPCM blocks */
    /* 0x0C */ s16 *pcm;            /* the 8 blocks decoded, 128 samples each */
    /* 0x10 */ s32 sample;          /* the clip playing, -1: none */
    /* 0x14 */ s32 block;           /* its next block to mix */
    /* 0x18 */ s32 blocks;          /* decoded blocks left in pcm */
    /* 0x1C */ u32 flags;           /* the clip's flags; 2 toggles each half-rate block */
    /* 0x20 */ u8 *data;            /* the clip in ROM */
    /* 0x24 */ s32 size;
    /* 0x28 */ s32 lastSample;      /* half rate: interpolated with the next one */
    /* 0x2C */ s32 bufBlock;        /* first block in pcm */
    /* 0x30 */ s32 bufSample;       /* the clip pcm holds, -1: none */
    /* 0x34 */ s32 dmaBlock;        /* first block the DMA loads */
    /* 0x38 */ s32 dmaSample;       /* the clip it loads */
    /* 0x3C */ s32 echo;            /* mixed through the echo ring */
    /* 0x40 */ s32 loop;            /* starts again at the end */
    /* 0x44 */ s32 echoTail;        /* echo blocks played after the end (32 at most), -1 while playing */
    /* 0x48 */ s32 volume;          /* 0-0x100 */
} StreamChannel; /* size 0x4C */

/* A song's (clip, size) pairs: tunes_play_song turns each `sample` from a StreamSample key into its index. */
typedef struct SongSample {
    /* 0x00 */ s32 sample;
    /* 0x04 */ s32 size;
} SongSample; /* size 0x8 */

/* A song event: at `time` (in mixed blocks), start sample `note` of the song on `channel`. */
typedef struct SongEvent {
    /* 0x00 */ u16 time;
    /* 0x02 */ u8 note;             /* 0xFF: stop the channel, 0xFE: loop to the first event */
    /* 0x03 */ u8 channel;
} SongEvent; /* size 0x4 */

/* A song resource, copied to sSongBuffer: its samples, then the events (SongEvent). */
typedef struct Song {
    /* 0x00 */ s32 count;
    /* 0x04 */ SongSample samples[1];
} Song;

#endif
