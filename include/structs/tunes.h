/* Streamed audio (tunes.c): music, the announcer and the crowd, ADPCM clips streamed from "BIG.RAW". */
#ifndef STRUCTS_TUNES_H
#define STRUCTS_TUNES_H

#include "common.h"

/* The stream channels (sStreamChannels). */
#define STREAM_CHANNELS         4
#define STREAM_CH_MUSIC         0       /* 0 and 1: the song's two channels */
#define STREAM_MUSIC_CHANNELS   2
#define STREAM_CH_SPEECH        2       /* the announcer */
#define STREAM_CH_AMBIENT       3       /* ambience, the crowd and the cliff fall */

/* A clip is ADPCM blocks of 0x44 bytes (a 5-byte header, then 126 4-bit codes) that decode to 128 samples each. A
   channel loads 8 blocks per DMA (Europe: 10, for the 50 Hz frame's longer audio) and decodes them into its pcm
   buffer. */
#define STREAM_BLOCK_BYTES      0x44
#define STREAM_BLOCK_SAMPLES    0x80
#if VERSION == VERSION_EU
#define STREAM_DMA_BLOCKS       10
#else
#define STREAM_DMA_BLOCKS       8
#endif
#define STREAM_DMA_BYTES        (STREAM_DMA_BLOCKS * STREAM_BLOCK_BYTES)    /* 0x220 (Europe: 0x2A8) */
#define STREAM_PCM_BYTES        (STREAM_DMA_BLOCKS * STREAM_BLOCK_SAMPLES * 2) /* StreamChannel.pcm: s16 samples */

/* The output rate the clips are mixed at, in Hz. */
#define STREAM_SAMPLE_RATE      0x5622

/* The mix ring (sStreamMixBuf, s32 samples), which tunes_mix_callback adds to both output channels (Europe: 0xA00
   samples; retail wraps the positions with a modulo, LodgeNet with STREAM_MIX_MASK). */
#if VERSION == VERSION_EU
#define STREAM_MIX_RING         0xA00
#else
#define STREAM_MIX_RING         0x800
#endif
#define STREAM_MIX_MASK         0x7FF
#define STREAM_MIX_BUF_BYTES    (STREAM_MIX_RING * 4)

/* The echo ring (sStreamEchoBuf, s16 samples): each sample adds half of the one a ring earlier. */
#define STREAM_ECHO_RING        0x800
#define STREAM_ECHO_BUF_BYTES   0x1000
#define STREAM_ECHO_TAIL        0x20    /* blocks of echo mixed after the clip ends */

/* StreamSample.flags and StreamChannel.flags */
#define STREAM_HALF_RATE        1       /* each sample is played twice (the first interpolated) */
#define STREAM_SECOND_HALF      2       /* channel, half rate: the second half of the block comes next */

/* SongEvent.note */
#define SONG_NOTE_LOOP          0xFE    /* start again at the first event */
#define SONG_NOTE_STOP          0xFF    /* stop the channel */

#define SONG_BUFFER_BYTES       0x800   /* sSongBuffer */

/* The announcer's queue (sSpeechQueue): clip indices, or place entries, resolved to a sPlaceClips clip when they play:
   SPEECH_PLACE_UP / SPEECH_PLACE_DOWN | (racer << 8) | place. UP says the racer's place, but no lower than `place` or
   last; DOWN says it no higher than `place` or first. */
#define SPEECH_QUEUE_SIZE       8
#define SPEECH_PLACE_UP         0x80010000
#define SPEECH_PLACE_DOWN       0x80020000
#define SPEECH_PLACE_KIND_UP    1       /* (entry >> 16) & 0xFF */
#define SPEECH_PLACE_KIND_DOWN  2

/* The crowd's queue (sCrowdQueue): (clip, frames to wait) pairs. */
#define CROWD_QUEUE_SIZE        2
#define CROWD_DELAY             0x1E    /* frames from the event to the crowd's reaction */

/* The announcer names racer slots 0-5 (a race has 6 racers at most) and riders 0-5 (gGame.riders). sRiderNameClips
   holds 4 intonations of each rider's name: rider r's intonation i is sRiderNameClips[r + i * ANNOUNCER_RIDERS]. */
#define ANNOUNCE_RACERS         6
#define ANNOUNCER_RIDERS        6
#define ANNOUNCER_INTONATIONS   4

/* The announcer's intervals, in frames. */
#define ANNOUNCE_NEVER                  (-FRAMES(1800)) /* the times announce_reset sets: long enough ago */
#define ANNOUNCE_START_DELAY            FRAMES(360) /* no place calls in the first 6 seconds of the race */
#define ANNOUNCE_COLLISION_INTERVAL     FRAMES(600) /* between two announce_lead_collision calls */
#define ANNOUNCE_PLACE_CHANGE_INTERVAL  FRAMES(900) /* between two announce_place_change calls */
#define ANNOUNCE_TAKEOUT_INTERVAL       FRAMES(480) /* a racer named this recently: only the victim's crash is called */

/* Stream clips (indices into sStreamSampleTable), after their names in sfx.GID (smp_*, cut to 8 characters there).
   Clips with the same name are other takes of it. */
#define CLIP_TRAIN              0x6C    /* smp_train */
#define CLIP_TRAIN_2            0x72    /* smp_train2 */
#define CLIP_CLIFF_FALL         0x73    /* smp_Cliff_Fa */
#define CLIP_ARE_FIGHTING       0x74    /* smp_Are_Figh */
#define CLIP_AND_BATTLING       0x75    /* smp_And_Batt */
#define CLIP_ARE_BATTLING       0x76    /* smp_Are_Batt */
#define CLIP_TAKES_THE          0x77    /* smp_Takes_Th */
#define CLIP_WITH_THE           0x78    /* smp_With_The */
#define CLIP_CROSSES            0x79    /* smp_Crosses_ */
#define CLIP_HAS_SET            0x7A    /* smp_Has_Set_ */
#define CLIP_SETS_A_NEW         0x7B    /* smp_Sets_A_N */
#define CLIP_IS_BEHIND          0x7F    /* smp_Is_Behin */
#define CLIP_IS_BEHIND_2        0x80    /* smp_Is_Behin */
#define CLIP_LAP_FOUR           0x81    /* smp_Lap_Four */
#define CLIP_LAP_THREE          0x82    /* smp_Lap_Thre */
#define CLIP_LAP_TWO            0x83    /* smp_Lap_Two */
#define CLIP_AT_THE_END_2       0x88    /* smp_At_The_E */
#define CLIP_CONTINUES          0x8A    /* smp_Continue */
#define CLIP_WITH_THE_2         0x91    /* smp_With_The */
#define CLIP_LAST_RACE          0x92    /* smp_Last_Rac */
#define CLIP_STARTING           0x93    /* smp_Starting */
#define CLIP_GOAL               0x94    /* smp_Goal */
#define CLIP_TAKES_THE_2        0x95    /* smp_Takes_Th */
#define CLIP_OVERTAKES          0x96    /* smp_Overtake */
#define CLIP_MOVES_UP           0x97    /* smp_Moves_Up */
#define CLIP_DROPS_BACK         0x98    /* smp_Drops_Ba */
#define CLIP_TAKES_FIRST        0x99    /* smp_Takes_Fi */
#define CLIP_FIFTH_PLACE        0x9A    /* smp_Fifth_Pl */
#define CLIP_FIRST_PLACE        0x9B    /* smp_First_Pl */
#define CLIP_FOURTH_PLACE       0x9C    /* smp_Forth_Pl */
#define CLIP_THIRD_PLACE        0x9D    /* smp_Third_Pl */
#define CLIP_SECOND_PLACE       0x9E    /* smp_Second_P */
#define CLIP_LAST_PLACE         0x9F    /* smp_Last_Pla */
#define CLIP_AND_ITS            0xB8    /* smp_And_Its */
#define CLIP_ITS                0xB9    /* smp_Its */
#define CLIP_AND                0xBA    /* smp_And */
#define CLIP_IS_IN              0xBE    /* smp_Is_In_v1 */
#define CLIP_IS_IN_2            0xBF    /* smp_Is_In_v2 */

/* The clip table of the stream data (sStreamSampleTable): the number of clips, then 5 words per clip (key, offset,
   size, flags, tag). The game indexes it in 5-word records from its start, so record i holds clip i's key, offset,
   size and flags after a first word that is the clip count in record 0 and clip i - 1's tag in the others. */
typedef struct StreamSample {
    /* 0x00 */ u32 count;           /* record 0: the number of clips; record i: the tag of clip i - 1 */
    /* 0x04 */ u32 key;             /* how a song names the clip (tunes_play_song): its sfx.GID index */
    /* 0x08 */ s32 offset;          /* from sStreamDataBase */
    /* 0x0C */ s32 size;            /* bytes: STREAM_BLOCK_BYTES per block */
    /* 0x10 */ u32 flags;           /* STREAM_HALF_RATE */
} StreamSample; /* size 0x14 */

/* A stream channel (sStreamChannels, STREAM_CHANNELS). */
typedef struct StreamChannel {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 dmaHandle;       /* -1: no DMA running */
    /* 0x08 */ u8 *dmaBuf;          /* STREAM_DMA_BYTES: STREAM_DMA_BLOCKS ADPCM blocks */
    /* 0x0C */ s16 *pcm;            /* those blocks decoded */
    /* 0x10 */ s32 sample;          /* the clip playing, -1: none */
    /* 0x14 */ s32 block;           /* its next block to mix */
    /* 0x18 */ s32 blocks;          /* decoded blocks left in pcm */
    /* 0x1C */ u32 flags;           /* the clip's flags, STREAM_SECOND_HALF */
    /* 0x20 */ u8 *data;            /* the clip in ROM */
    /* 0x24 */ s32 size;
    /* 0x28 */ s32 lastSample;      /* half rate: interpolated with the next one */
    /* 0x2C */ s32 bufBlock;        /* first block in pcm */
    /* 0x30 */ s32 bufSample;       /* the clip pcm holds, -1: none */
    /* 0x34 */ s32 dmaBlock;        /* first block the DMA loads */
    /* 0x38 */ s32 dmaSample;       /* the clip it loads */
    /* 0x3C */ s32 echo;            /* mixed through the echo ring */
    /* 0x40 */ s32 loop;            /* starts again at the end */
    /* 0x44 */ s32 echoTail;        /* echo blocks played after the end (STREAM_ECHO_TAIL at most), -1 while playing */
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
    /* 0x02 */ u8 note;             /* SONG_NOTE_LOOP, SONG_NOTE_STOP or a sample */
    /* 0x03 */ u8 channel;
} SongEvent; /* size 0x4 */

/* A song resource, copied to sSongBuffer: its samples, then the events (SongEvent). */
typedef struct Song {
    /* 0x00 */ s32 count;
    /* 0x04 */ SongSample samples[1];
} Song;

#endif
