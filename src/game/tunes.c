#include "game.h"

extern SongEvent *sSongCursor;

/* The helpers below are copies of two pieces of code, written once per group of callers so that each caller compiles
   as it did: speech_queue_push* add a clip to the announcer's queue (outside replays, the credits and the podium,
   while there is room), and racer_to_rider* are inline forms of announce_check_rider. They are kept apart on
   purpose. */

static inline void speech_queue_push(s32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
        sSpeechQueue[sSpeechQueueCount] = clip;
        sSpeechQueueCount++;
    }
}

/* Whether racer *racer is left out of the announcements (1: the ghost, soccer, no named rider). Otherwise it stamps
   the racer's announce time, replaces *racer with its rider (gGame.riders) and returns 2 in the tutorial, else 0. */
static inline s32 racer_to_rider(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot) return 1;
    if (gGame.mode == GAME_MODE_SOCCER) return 1;
    if (*racer >= ANNOUNCE_RACERS) return 1;
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) return 1;
    return (gGame.mode == GAME_MODE_TUTORIAL) << 1;
}

static inline void speech_queue_push_2(s32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
        sSpeechQueue[sSpeechQueueCount] = clip;
        sSpeechQueueCount++;
    }
}

static __inline__ void speech_queue_push_3(s32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
        sSpeechQueue[sSpeechQueueCount] = clip;
        sSpeechQueueCount++;
    }
}

static inline s32 racer_to_rider_2(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) {
        return 1;
    }
    return (gGame.mode == GAME_MODE_TUTORIAL) * 2;
}

static inline void speech_queue_push_u32(u32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
        sSpeechQueue[sSpeechQueueCount] = clip;
        sSpeechQueueCount++;
    }
}

static inline void speech_queue_push_u32_2(u32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
        sSpeechQueue[sSpeechQueueCount] = clip;
        sSpeechQueueCount++;
    }
}

/* Opens "BIG.RAW", loads its clip table and sets up the mix ring, the echo ring and `numChannels` stream channels. */
void tunes_init(s32 numChannels)
{
    FileDirEntry *file;
    s32 i;

    sSpeechQueueCount = 0;
    file = file_find(sStrBigRaw);
    if (file != NULL) {
        sStreamDataBase = (s32)file->rom;
        sStreamDataEnd = sStreamDataBase + file->size;
        sStreamSampleTable = res_load(gSfxResSet, sStreamSampleTableResId);
        if (sStreamSampleTable != 0) {
            sStreamMixBuf = mem_alloc_dbg(STREAM_MIX_BUF_BYTES, 0, sStrTunesC, SRC_LINE(0xFC, 0x105));
            sStreamMixFill = 0;
            sStreamMixWritePos = 0;
            sStreamMixReadPos = 0;
            gN64SndMixCallback = tunes_mix_callback;
            mem_fill(sStreamEchoBuf, 0, STREAM_ECHO_BUF_BYTES);
            sStreamEchoPos = 0;
            sStreamChannelCount = numChannels;
            sStreamChannels = mem_alloc_dbg(STREAM_CHANNELS * sizeof(StreamChannel), 0, sStrTunesC, SRC_LINE(0x10C, 0x115));
            mem_fill(sStreamChannels, 0, STREAM_CHANNELS * sizeof(StreamChannel));
            for (i = 0; i < sStreamChannelCount; i++) {
                tunes_init_channel(&sStreamChannels[i]);
            }
            sCurrentSong = 0;
            sStreamLock = 0;
            sTunesActive = 1;
        }
    }
}

/* tunes_apply_echo, inlined: runs `count` samples through the echo ring. */
static inline void echo_ring(s16 *pcm, s32 count)
{
    u16 *echo;
    s32 sample;

    while (count != 0) {
        echo = &sStreamEchoBuf[sStreamEchoPos];
        sample = *pcm + ((s16)*echo >> 1);
        if (sample > 0x7FFF)
            sample = 0x7FFF;
        if (sample < -0x7FFF)
            sample = -0x7FFF;
        *echo = sample;
        *pcm++ = sample;
        sStreamEchoPos = (sStreamEchoPos + 1) % STREAM_ECHO_RING;
        count--;
    }
}

/* Mixes one block of every channel into the mix ring. Returns 0 while a DMA runs or the ring is full, after
   starting the DMAs of channels whose next block is not decoded; 1 after mixing a block (and advancing the song). */
s32 tunes_mix_streams(void)
{
    StreamChannel *ch;
    s32 i, j, n, v;
    s32 a, src;
    s32 started, mixed;
    s32 pos;
    u8 *addr, *end;
    s16 *in, *out;
    u16 *p;
    s32 vol;
    s32 flags;
    s32 *buf;

    tunes_update_speech();
    if (sSpeechUseSfxVolume == 0) {
        sStreamChannels[STREAM_CH_SPEECH].volume = (gSpeechVolume * gSndMasterVolume) >> 8;
    } else {
        sStreamChannels[STREAM_CH_SPEECH].volume = (gSfxVolume * gSndMasterVolume) >> 8;
    }

    /* finished DMAs: decode the blocks */
    for (i = 0; i < sStreamChannelCount; i++) {
        ch = &sStreamChannels[i];
        if (ch->dmaHandle >= 0) {
            if (dma_get_status(ch->dmaHandle) != DMA_DONE)
                return 0;
            dma_release(ch->dmaHandle);
            ch->dmaHandle = -1;
            if (ch->dmaSample >= 0) {
                osInvalDCache(ch->dmaBuf, STREAM_DMA_BYTES);
                src = ch->dmaBuf;
                out = ch->pcm;
                for (j = 0; j < STREAM_DMA_BLOCKS; j++) {
                    tunes_decode_adpcm_block(src, out, STREAM_BLOCK_SAMPLES);
                    src += STREAM_BLOCK_BYTES;
                    out += STREAM_BLOCK_SAMPLES;
                }
                ch->bufBlock = ch->dmaBlock;
                ch->bufSample = ch->dmaSample;
                if (ch->echo != 0) {
                    echo_ring(ch->pcm, ch->blocks << 7); /* blocks * STREAM_BLOCK_SAMPLES */
                }
            }
        }
    }

    if (sStreamLock != 0)
        return 0;
    if (sStreamMixFill > STREAM_MIX_RING - STREAM_BLOCK_SAMPLES)
        return 0;

    /* channels whose next block is not in pcm: load the next STREAM_DMA_BLOCKS blocks (j: a DMA was started) */
    i = 0;
    j = 0;
    for (; i < sStreamChannelCount; i++) {
        ch = &sStreamChannels[i];
        if (ch->sample == ch->bufSample && ch->block >= ch->bufBlock && ch->block < ch->bufBlock + STREAM_DMA_BLOCKS)
            continue;
        if (ch->sample >= 0) {
            addr = ch->data;
            flags = (s32)(addr + ch->size); /* here the clip's end */
            addr += ch->block * STREAM_BLOCK_BYTES;
            ch->blocks = (u32)(flags - (s32)addr) / STREAM_BLOCK_BYTES;
            if (ch->blocks < 0)
                ch->blocks = 0;
            if (ch->blocks > STREAM_DMA_BLOCKS)
                ch->blocks = STREAM_DMA_BLOCKS;
            if (ch->blocks == 0) {
                if (ch->loop != 0) {
                    ch->block = 0;
                    addr = ch->data + ch->block * STREAM_BLOCK_BYTES;
                    ch->blocks = (u32)(ch->data + ch->size - addr) / STREAM_BLOCK_BYTES;
                    if (ch->blocks < 0)
                        ch->blocks = 0;
                    if (ch->blocks > STREAM_DMA_BLOCKS)
                        ch->blocks = STREAM_DMA_BLOCKS;
                }
                if (ch->blocks == 0) {
                    /* the clip ended */
                    ch->bufSample = -1;
                    ch->sample = -1;
                    ch->bufBlock = ch->block;
                    if (i == STREAM_CH_AMBIENT && sAmbientLocked != 0)
                        sAmbientLocked = 0;
                    continue;
                }
            }
            ch->dmaHandle = dma_start(gCartRomHandle, 0, (s32)addr, ch->dmaBuf, STREAM_DMA_BYTES);
            j = 1;
            ch->dmaSample = ch->sample;
            ch->dmaBlock = ch->block;
        } else {
            ch->bufSample = -1;
            ch->bufBlock = ch->block;
        }
    }
    if (j)
        return 0;

    /* mix a block of each channel: the first one stores, the others add */
    mixed = 0;
    for (i = 0; i < sStreamChannelCount; i++) {
        ch = &sStreamChannels[i];
        in = ch->pcm + (ch->block - ch->bufBlock) * STREAM_BLOCK_SAMPLES;
        if (ch->blocks <= 0)
            ch->bufSample = -1;
        if (ch->bufSample == -1) {
            /* nothing playing: the echo's tail, fed with silence */
            if (ch->echo == 0)
                continue;
            in = ch->pcm;
            if ((ch->flags & 3) != 3) {
                if (ch->echoTail >= 0)
                    mem_fill((u8 *)in, 0, STREAM_BLOCK_SAMPLES * sizeof(s16));
                echo_ring(in, STREAM_BLOCK_SAMPLES);
                if (++ch->echoTail >= STREAM_ECHO_TAIL)
                    ch->echo = 0;
            }
        } else {
            ch->echoTail = -1;
        }
        flags = ch->flags;
        vol = sStreamChannels[i].volume;
#if VERSION != VERSION_LODGENET
        /* retail wraps the mix ring's position with a modulo when it reaches the end */
        if (!(flags & STREAM_HALF_RATE)) {
            pos = sStreamMixWritePos;
            if (!mixed) {
                for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
                    sStreamMixBuf[pos++] = (*in++ * vol) >> 8;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                }
            } else {
                for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
                    sStreamMixBuf[pos++] += (*in++ * vol) >> 8;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                }
            }
            ch->block++;
            ch->blocks--;
        } else {
            /* half rate: half a block, each sample twice (the first interpolated with the previous one) */
            if (flags & STREAM_SECOND_HALF) {
                in += STREAM_BLOCK_SAMPLES / 2;
                ch->block++;
                ch->blocks--;
            }
            pos = sStreamMixWritePos;
            ch->flags ^= STREAM_SECOND_HALF;
            /* `mixed` holds each scaled sample in these loops */
            if (!mixed) {
                for (j = 0; j < STREAM_BLOCK_SAMPLES / 2; j++) {
                    mixed = (*in * vol) >> 8;
                    sStreamMixBuf[pos++] = (mixed + ch->lastSample) / 2;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                    sStreamMixBuf[pos++] = mixed;
                    ch->lastSample = mixed;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                    in++;
                }
            } else {
                for (j = 0; j < STREAM_BLOCK_SAMPLES / 2; j++) {
                    mixed = (*in * vol) >> 8;
                    sStreamMixBuf[pos++] += (mixed + ch->lastSample) / 2;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                    sStreamMixBuf[pos++] += mixed;
                    ch->lastSample = mixed;
                    if (pos >= STREAM_MIX_RING) {
                        pos %= STREAM_MIX_RING;
                    }
                    in++;
                }
            }
        }
        mixed = 1;
    }
    if (!mixed) {
        pos = sStreamMixWritePos;
        for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
            sStreamMixBuf[pos++] = 0;
            if (pos >= STREAM_MIX_RING) {
                pos %= STREAM_MIX_RING;
            }
        }
    }
#else
        if (!(flags & STREAM_HALF_RATE)) {
            pos = sStreamMixWritePos;
            if (!mixed) {
                for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
                    sStreamMixBuf[pos] = (*in++ * vol) >> 8;
                    pos = (pos + 1) & STREAM_MIX_MASK;
                }
            } else {
                for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
                    sStreamMixBuf[pos] += (*in++ * vol) >> 8;
                    pos = (pos + 1) & STREAM_MIX_MASK;
                }
            }
            ch->block++;
            ch->blocks--;
        } else {
            /* half rate: half a block, each sample twice (the first interpolated with the previous one) */
            if (flags & STREAM_SECOND_HALF) {
                in += STREAM_BLOCK_SAMPLES / 2;
                ch->block++;
                ch->blocks--;
            }
            pos = sStreamMixWritePos;
            ch->flags ^= STREAM_SECOND_HALF;
            /* `mixed` holds each scaled sample in these loops; the cast address forms are &buf[pos] */
            if (!mixed) {
                j = 0;
                buf = sStreamMixBuf;
                for (; j < STREAM_BLOCK_SAMPLES / 2; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & STREAM_MIX_MASK;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & STREAM_MIX_MASK;
                    mixed = (*in++ * vol) >> 8;
                    *b0 = (mixed + ch->lastSample) / 2;
                    *b1 = mixed;
                    ch->lastSample = mixed;
                }
            } else {
                j = 0;
                buf = sStreamMixBuf;
                for (; j < STREAM_BLOCK_SAMPLES / 2; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & STREAM_MIX_MASK;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & STREAM_MIX_MASK;
                    mixed = (*in++ * vol) >> 8;
                    *b0 += (mixed + ch->lastSample) / 2;
                    *b1 += mixed;
                    ch->lastSample = mixed;
                }
            }
        }
        mixed = 1;
    }
    if (!mixed) {
        pos = sStreamMixWritePos;
        for (j = 0; j < STREAM_BLOCK_SAMPLES; j++) {
            sStreamMixBuf[pos] = 0;
            pos = (pos + 1) & STREAM_MIX_MASK;
        }
    }
#endif
    sStreamMixWritePos = pos;
    sStreamMixFill += STREAM_BLOCK_SAMPLES;
    sSongTick += 1;
    tunes_update_song();
    return 1;
}

/* Runs the song's events that are due: starts and stops clips on its channels, loops at SONG_NOTE_LOOP. */
void tunes_update_song(void)
{
    Song *song;
    SongEvent *ev;
    StreamSample *smp;
    s32 *pair;

    if (sCurrentSong == NULL) {
        return;
    }
    while (sSongCursor->time <= sSongTick) {
        ev = sSongCursor;
        song = sCurrentSong;
        if (ev->note == SONG_NOTE_LOOP) {
            sSongTick = 0;
            sSongCursor = (SongEvent *)&song->samples[song->count];
        } else {
            if (ev->note == SONG_NOTE_STOP) {
                sStreamChannels[ev->channel & 0xF].sample = -1;
            } else {
                sStreamChannels[ev->channel & 0xF].sample = ev->note;
                sStreamChannels[ev->channel & 0xF].block = 0;
                pair = (s32 *)song + ev->note * 2;     /* pair[1]: song->samples[ev->note].sample */
                sStreamChannels[ev->channel & 0xF].data =
                    (u8 *)((smp = &sStreamSampleTable[pair[1]])->offset + sStreamDataBase);
                sStreamChannels[ev->channel & 0xF].size = song->samples[ev->note].size;
                sStreamChannels[ev->channel & 0xF].flags = smp->flags;
                sStreamChannels[ev->channel & 0xF].volume = gStreamVolumes_2[ev->channel & 0xF] * gSndMasterVolume >> 8;
            }
            sSongCursor++;
        }
    }
}

extern Song sSongBuffer;   /* SONG_BUFFER_BYTES */

/* Stops the song playing and starts song resource `songId` (none if negative): copies it to sSongBuffer and turns
   its clip keys into clip indices. Hangs if the song is too big or names a missing clip. */
void tunes_play_song(s32 songId)
{
    s32 n;
    u32 clip;
    StreamSample *table;
    s32 k;
    Song *buf;
    Song *song;
    u32 *words;
    u32 key;

    if (sCurrentSong != 0) {
        tunes_stop_song();
    }
    if (songId >= 0) {
        sCurrentSong = res_load(gSfxResSet_4, songId);
        n = mem_get_size(sCurrentSong);
        if (n > SONG_BUFFER_BYTES) {
            while (1) ;
        }
        buf = &sSongBuffer;
        memcpy(buf, sCurrentSong, n);
        res_unload((u32 *)gSfxResSet_4[1] + songId);   /* the resource's handle */
        /* n: now the sample pair */
        n = 0;
        sCurrentSong = buf;
        if (sSongBuffer.count != 0) {
            table = sStreamSampleTable;
            song = buf;
            words = (u32 *)song;
            do {
                key = words[n * 2 + 1];
                for (clip = 0; clip < table->count; clip++) {
                    if (table[clip].key == key) {
                        words[n * 2 + 1] = clip;
                        words[n * 2 + 2] = table[clip].size;
                        break;
                    }
                }
                if (clip == table->count) {
                    while (1) ;
                }
                n++;
            } while ((u32)n < (u32)song->count);
        }
        sSongTick = 0;
        sSongCursor = (SongEvent *)&sCurrentSong->samples[sCurrentSong->count];
        for (n = 0; n < STREAM_MUSIC_CHANNELS; n++) {
            sStreamChannels[n].bufSample = -1;
            sStreamChannels[n].bufBlock = -1;
            sStreamChannels[n].dmaSample = -1;
            sStreamChannels[n].dmaBlock = -1;
        }
        tunes_update_song();
    }
}

/* Stops every channel and, if a song plays, waits for the channels' DMAs and drops the song. */
void tunes_stop_song(void)
{
    s32 i;
    s32 n;
    StreamChannel *ch;
    StreamChannel *q;

    for (i = 0; i < sStreamChannelCount; i++) {
        sStreamChannels[i].sample = -1;
        sStreamChannels[i].block = -1;
        sStreamChannels[i].bufSample = -1;
        sStreamChannels[i].bufBlock = -1;
        sStreamChannels[i].dmaSample = -1;
        sStreamChannels[i].dmaBlock = -1;
    }

    if (sCurrentSong != 0) {
        sStreamLock = 1;
        for (i = 0; i < sStreamChannelCount; i++) {
            ch = &sStreamChannels[i];
            for (;;) {
                if (ch->dmaHandle < 0) break;
                if (dma_get_status(ch->dmaHandle) == DMA_DONE) {
                    dma_release(ch->dmaHandle);
                    ch->dmaHandle = -1;
                    break;
                }
            }
        }
        sStreamLock = 0;
        sCurrentSong = 0;
    }
}

#if VERSION != VERSION_LODGENET
/* n64snd's post-mix callback: adds up to `count` mixed samples to both channels of the output. Retail's is here
   (LodgeNet's, further down, wraps the read position with a mask). */
void tunes_mix_callback(s32 *out, s32 count)
{
    if (sStreamMixFill < count) {
        count = sStreamMixFill;
    }
    if (sStreamLock == 0) {
        sStreamMixFill = sStreamMixFill - count;
        while (count != 0) {
            *out = *out + sStreamMixBuf[sStreamMixReadPos];
            out++;
            *out = *out + sStreamMixBuf[sStreamMixReadPos];
            out++;
            sStreamMixReadPos++;
            if (sStreamMixReadPos >= STREAM_MIX_RING) {
                sStreamMixReadPos = sStreamMixReadPos % STREAM_MIX_RING;
            }
            count--;
        }
    }
}
#endif

/* Counts down the first crowd reaction (sCrowdQueue: clip, frames, clip, frames) and plays it on the ambience
   channel when due. */
void tunes_update_crowd_queue(void)
{
    StreamChannel *ch; StreamSample *smp;
    if (sCrowdQueueCount != 0) {
        if (--sCrowdQueue[1] <= 0) {
            ch = sStreamChannels;
            smp = &sStreamSampleTable[sCrowdQueue[0]];
            ch[STREAM_CH_AMBIENT].sample = sCrowdQueue[0];
            ch[STREAM_CH_AMBIENT].echo = 0;
            ch[STREAM_CH_AMBIENT].loop = 0;
            ch[STREAM_CH_AMBIENT].block = 0;
            ch[STREAM_CH_AMBIENT].data = (u8 *)(smp->offset + sStreamDataBase);
            ch[STREAM_CH_AMBIENT].size = smp->size;
            ch[STREAM_CH_AMBIENT].flags = smp->flags;
            ch[STREAM_CH_AMBIENT].volume = (gAmbientVolume_2[0] * gSndMasterVolume_2[0]) >> 8;
            sCrowdQueueCount--;
            sCrowdQueue[0] = sCrowdQueue[2];
            sCrowdQueue[1] = sCrowdQueue[3];
        }
    }
}

/* Loops ambience clip `clip` on the ambience channel at `volume` (0 stops it), unless a one-shot clip holds the
   channel (sAmbientLocked). */
void tunes_set_ambient(s32 clip, s32 volume, s32 unused)
{
    if (sAmbientLocked != 0) {
        return;
    }
    if (volume == 0) {
        if (sStreamChannels[STREAM_CH_AMBIENT].sample != -1) {
            sStreamChannels[STREAM_CH_AMBIENT].sample = -1;
            sStreamChannels[STREAM_CH_AMBIENT].loop = 0;
            gAmbientVolume = 0;
        }
        return;
    }
    if (sStreamChannels[STREAM_CH_AMBIENT].sample != clip) {
        StreamSample *smp;
        s32 flags;
        sStreamChannels[STREAM_CH_AMBIENT].sample = clip;
        smp = &sStreamSampleTable[clip];
        sStreamChannels[STREAM_CH_AMBIENT].block = 0;
        sStreamChannels[STREAM_CH_AMBIENT].data = (u8 *)(smp->offset + sStreamDataBase);
        sStreamChannels[STREAM_CH_AMBIENT].size = smp->size;
        flags = smp->flags;
        sStreamChannels[STREAM_CH_AMBIENT].loop = 1;
        sStreamChannels[STREAM_CH_AMBIENT].flags = flags;
    }
    gAmbientVolume = volume;
}

/* Starts the next clip of the speech queue when the announcer's channel is free (or playing a queued clip),
   resolving place entries to the racer's place now. */
void tunes_update_speech(void)
{
    s32 entry;
    s32 place;
    s32 limit;
    s32 kind;
    s32 busy;
    s32 count;
    s32 volume;
    StreamChannel *ch;
    StreamSample *smp;

    if (sSpeechQueueCount_2[0] == 0) {
        return;
    }
    if (sStreamChannels[STREAM_CH_SPEECH].sample != -1 && sSpeechPriority >= 0) {
        return;
    }
    entry = sSpeechQueue[0];
    if (entry < 0) {
        kind = (entry >> 16) & 0xFF;
        if (kind == SPEECH_PLACE_KIND_UP) {
            place = (entry >> 8) & 0xFF;
            place = gRiders[place].race.place;
            limit = entry & 0xFF;
            if (limit < place) {
                place = limit;
            }
            limit = gEvent.numRiders - 2;
            if (limit < place) {
                place = limit;
            }
        } else if (kind == SPEECH_PLACE_KIND_DOWN) {
            place = (entry >> 8) & 0xFF;
            place = gRiders[place].race.place;
            limit = entry & 0xFF;
            if (place < limit) {
                place = limit;
            }
            if (place <= 0) {
                place = 1;
            }
        } else {
            goto bad;
        }
        entry = sPlaceClips[place];
        goto ok;
    bad:
        sSpeechQueueCount_2[0]--;
        return;
    }
ok:
    /* busy is left unset on the podium */
    if (gPodiumMode == 0) {
        if (sStreamChannels[STREAM_CH_SPEECH].sample != -1 && sSpeechPriority >= 0) {
            busy = 1;
        } else {
            sStreamChannels[STREAM_CH_SPEECH].sample = entry;
            smp = &sStreamSampleTable[entry];
            sStreamChannels[STREAM_CH_SPEECH].echo = 0;
            sStreamChannels[STREAM_CH_SPEECH].loop = 0;
            sStreamChannels[STREAM_CH_SPEECH].block = 0;
            sStreamChannels[STREAM_CH_SPEECH].data = (u8 *)(smp->offset + sStreamDataBase);
            sSpeechPriority = 0;
            sStreamChannels[STREAM_CH_SPEECH].size = smp->size;
            sSpeechUseSfxVolume = 0;
            sStreamChannels[STREAM_CH_SPEECH].flags = smp->flags;
            busy = 0;
        }
    }
    if (busy == 0) {
        ch = sStreamChannels;
        ch[STREAM_CH_SPEECH].echo = 1;
        volume = gSpeechVolume;
        count = --sSpeechQueueCount_2[0];
        ch[STREAM_CH_SPEECH].volume = volume;
        if (count != 0) {
            memcpy(sSpeechQueue, sSpeechQueue + 1, count << 2);
        }
        gLastSpeechTime = gRaceFrame;
    }
}

static inline s32 racer_to_rider_3(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot_2) return 1;
    if (gGame.mode == GAME_MODE_SOCCER) return 1;
    if (*racer >= ANNOUNCE_RACERS) return 1;
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) return 1;
    return (gGame.mode == GAME_MODE_TUTORIAL) * 2;
}

/* The announce_* functions queue a phrase of clips: rider names (sRiderNameClips, rider + intonation *
   ANNOUNCER_RIDERS) and the CLIP_* words. Most give up unless the queue has room for the whole phrase (a phrase of n
   clips needs sSpeechQueueCount < SPEECH_QUEUE_SIZE + 1 - n). */

/* The two leaders collide: CLIP_AND_BATTLING A CLIP_AND B, or A CLIP_AND B CLIP_ARE_BATTLING / CLIP_ARE_FIGHTING. */
void announce_lead_collision(u32 racerA, u32 racerB)
{
    if (gRaceFrame - sLastLeadCollisionTime < ANNOUNCE_COLLISION_INTERVAL) return;
    if (racer_to_rider_3(&racerA)) return;
    if (racer_to_rider_3(&racerB)) return;
    sLastLeadCollisionTime = gRaceFrame;
    random_byte();
    if (random_byte() & 1) {
        if (sSpeechQueueCount >= 5) return;
        speech_queue_push(CLIP_AND_BATTLING);
        speech_queue_push(sRiderNameClips[racerA + 2 * ANNOUNCER_RIDERS]);
        speech_queue_push(CLIP_AND);
        speech_queue_push(sRiderNameClips[racerB + 3 * ANNOUNCER_RIDERS]);
    } else {
        if (sSpeechQueueCount >= 5) return;
        speech_queue_push(sRiderNameClips[racerA + 2 * ANNOUNCER_RIDERS]);
        speech_queue_push(CLIP_AND);
        speech_queue_push(sRiderNameClips[racerB + 3 * ANNOUNCER_RIDERS]);
        if (random_byte() & 1) {
            speech_queue_push(CLIP_ARE_BATTLING);
        } else {
            speech_queue_push(CLIP_ARE_FIGHTING);
        }
    }
}

/* "Lap two" to "Lap four" when lap `lap` (0: the first) ends. */
void announce_lap(s32 lap)
{
    if (lap == 0) {
        speech_queue_push(CLIP_LAP_TWO);
    } else if (lap == 1) {
        speech_queue_push(CLIP_LAP_THREE);
    } else if (lap == 2) {
        speech_queue_push(CLIP_LAP_FOUR);
    }
}

/* The leader as the first or second lap ends or the final lap starts: that lap's clip, the rider, then a takes-
   the-lead or still-leads clip. */
void announce_leader_lap(s32 racer, s32 lap)
{
    s32 *t;
    s32 v;

    if (sSpeechQueueCount >= 6) {
        return;
    }
    if (racer_to_rider((u32 *)&racer)) {
        return;
    }
    if (lap == gEvent.numLaps - 2) {
        speech_queue_push_2(sFinalLapClips[random_byte() & 1]);
    } else if (lap == 0) {
        speech_queue_push_2(sLeaderFirstLapClips[random_byte() % 3]);
    } else {
        if (lap != 1) {
            return;
        }
        speech_queue_push_2(CLIP_AT_THE_END_2);
    }
    speech_queue_push_2(sRiderNameClips[racer + 3 * ANNOUNCER_RIDERS]);
    if (racer != sLastLeader) {
        speech_queue_push_2(sTakesLeadClips[random_byte() & 3]);
    } else {
        speech_queue_push_2(sStillLeadsClips[random_byte() % 3]);
    }
    sLastLeader = racer;
}

static inline s32 racer_to_rider_4(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot) return 1;
    if (gGame.mode == GAME_MODE_SOCCER) return 1;
    if (*racer >= ANNOUNCE_RACERS) return 1;
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) return 1;
    return (gGame.mode == GAME_MODE_TUTORIAL) * 2;
}

/* A racer's place (1: first): takes the lead, is in second, third ... or last place. */
void announce_place(u32 racer, u32 place)
{
    if (gEvent.time < ANNOUNCE_START_DELAY) return;
    if (sSpeechQueueCount >= 6) return;
    if (gRiders[racer].race.finishPlace >= 0) return;
    if (racer_to_rider_4(&racer)) return;
    speech_queue_push(sRiderNameClips[racer + 2 * ANNOUNCER_RIDERS]);
    if (place == 1) {
        if (racer != sLastLeader_2) {
            speech_queue_push(sTakesLeadClips[random_byte() & 3]);
        } else {
            speech_queue_push(CLIP_CONTINUES);
        }
        sLastLeader_2 = racer;
    } else if (place == gGame.numPlayers + gGame.numCpuRiders) {
        if (random_byte() & 1) {
            speech_queue_push(CLIP_IS_BEHIND);
        } else {
            speech_queue_push(CLIP_IS_IN);
            speech_queue_push(CLIP_LAST_PLACE);
        }
    } else if (place == 2) {
        if (random_byte() & 1) {
            speech_queue_push(CLIP_IS_BEHIND_2);
        } else {
            speech_queue_push(CLIP_IS_IN);
            speech_queue_push(CLIP_SECOND_PLACE);
        }
    } else {
        speech_queue_push(CLIP_IS_IN_2);
        if (place == 3) {
            speech_queue_push(CLIP_THIRD_PLACE);
        } else if (place == 4) {
            speech_queue_push(CLIP_FOURTH_PLACE);
        } else if (place == 5) {
            speech_queue_push(CLIP_FIFTH_PLACE);
        }
    }
}

/* The winner crosses the line: "(And) it's <rider> with the ...", "<rider> takes first" or "<rider> takes the ...". */
void announce_winner(u32 racer)
{
    s32 pick;

    pick = (s32)random_byte() % 3;
    if (racer_to_rider(&racer)) {
        return;
    }
    if (pick == 0) {
        if (sSpeechQueueCount >= 6) {
            return;
        }
        if (random_byte() & 1) {
            speech_queue_push_2(CLIP_AND_ITS);
        } else {
            speech_queue_push_2(CLIP_ITS);
        }
        speech_queue_push_2(sRiderNameClips[racer]);
        speech_queue_push_2(CLIP_WITH_THE);
    } else if (pick == 1) {
        if (sSpeechQueueCount >= 7) {
            return;
        }
        speech_queue_push_2(sRiderNameClips[racer]);
        speech_queue_push_2(CLIP_TAKES_FIRST);
    } else {
        if (sSpeechQueueCount >= 7) {
            return;
        }
        speech_queue_push_2(sRiderNameClips[racer]);
        speech_queue_push_2(CLIP_TAKES_THE);
    }
}

static __inline__ s32 racer_to_rider_5(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot_2) {
        return 1;
    }
    if (gGame.mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) {
        return 1;
    }
    return (gGame.mode == GAME_MODE_TUTORIAL) << 1;
}

/* A racer crashes: "<rider> <crash clip>", when nothing else is queued. */
void announce_crash(u32 racer)
{
    u32 pick;

    if (sSpeechQueueCount != 0) {
        return;
    }
    if (racer_to_rider_5(&racer) == 0) {
        pick = random_byte() % 3;
        if (pick == 0) {
            speech_queue_push_3(sRiderNameClips[racer + 3 * ANNOUNCER_RIDERS]);
        } else {
            speech_queue_push_3(sRiderNameClips[racer + 1 * ANNOUNCER_RIDERS]);
        }
        speech_queue_push_3(sCrashClips[pick]);
    }
}

static __inline__ s32 racer_to_rider_6(u32 *racer)
{
    if (gGhostRace != 0 && *racer == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) {
        return 1;
    }
    return (gGame.mode == GAME_MODE_TUTORIAL) << 1;
}

/* A racer crashes on landing: "<rider> <crash landing clip>" in a stadium, announce_crash elsewhere. */
void announce_crash_landing(u32 racer)
{
    if (*(s32 *)(gCurTrack + 0x2c) != 0) {  /* the track's environment type: not a stadium */
        announce_crash_2(racer);
        return;
    }
    if (sSpeechQueueCount != 0) {
        return;
    }
    if (racer_to_rider_6(&racer) == 0) {
        speech_queue_push_3(sRiderNameClips[racer + 1 * ANNOUNCER_RIDERS]);
        speech_queue_push_3(sCrashLandingClips[random_byte() % 3]);
    }
}

/* `attacker` knocks `victim` down: "<attacker> takes out / knocks off <victim>", or only the victim's crash if the
   attacker was named recently. */
void announce_takeout(u32 attacker, u32 victim)
{
    u32 pick;

    if (sSpeechQueueCount != 0) {
        return;
    }
    if ((s32)(gRaceFrame - gAnnounceTimes[attacker]) < ANNOUNCE_TAKEOUT_INTERVAL) {
        announce_crash(victim);
        return;
    }
    if (racer_to_rider_2(&attacker) != 0) {
        return;
    }
    if (racer_to_rider_2(&victim) != 0) {
        return;
    }
    pick = random_byte() % 3;
    if (pick == 0) {
        speech_queue_push_u32(sRiderNameClips[attacker + 2 * ANNOUNCER_RIDERS]);
    } else {
        speech_queue_push_u32(sRiderNameClips[attacker + 1 * ANNOUNCER_RIDERS]);
    }
    speech_queue_push_u32(sTakeoutClips[pick]);
    speech_queue_push_u32(sRiderNameClips[victim + 3 * ANNOUNCER_RIDERS]);
}

/* racer_to_rider without the ghost test, which announce_trick makes itself. */
static inline s32 racer_to_rider_no_ghost(u32 *racer)
{
    if (gGame.mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) {
        return 1;
    }
    return (gGame.mode == GAME_MODE_TUTORIAL) * 2;
}

/* Queues a trick's clip (also in the tutorial). */
void announce_trick(u32 racer, s32 clip)
{
    s32 result;
    u32 *racerPtr;

    if (sSpeechQueueCount < 7) {
        racerPtr = &racer;
        if (gGhostRace != 0 && racer == gGhostSlot) {
            result = 1;
        } else {
            result = racer_to_rider_no_ghost(racerPtr);
        }
        if (result != 1 && gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0
            && sSpeechQueueCount < SPEECH_QUEUE_SIZE) {
            sSpeechQueue[sSpeechQueueCount++] = clip;
        }
    }
}

/* "<racer> overtakes / takes the ... <other>". */
void announce_overtake(u32 racer, u32 other, s32 takesThe)
{
    if (gEvent.time < ANNOUNCE_START_DELAY) {
        return;
    }
    if (sSpeechQueueCount >= 6) {
        return;
    }
    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    if (racer_to_rider_2(&other) != 0) {
        return;
    }
    speech_queue_push_u32(sRiderNameClips[racer + 2 * ANNOUNCER_RIDERS]);
    if (takesThe != 0) {
        speech_queue_push_u32(CLIP_TAKES_THE_2);
    } else {
        speech_queue_push_u32(CLIP_OVERTAKES);
    }
    speech_queue_push_u32(sRiderNameClips[other + 3 * ANNOUNCER_RIDERS]);
}

/* racer_to_rider, testing the ghost on a copy of the racer slot. */
static inline s32 racer_to_rider_7(u32 slot, u32 *racer)
{
    if (gGhostRace != 0 && slot == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    *racer = gGame.riders[*racer];
    if (*racer >= ANNOUNCER_RIDERS) {
        return 1;
    }
    return (gGame.mode == GAME_MODE_TUTORIAL) * 2;
}

/* A player's place changes from `prevPlace` to `place`: "<rider> moves up / drops back", then the place, which the
   queue resolves when it plays. */
void announce_place_change(u32 racer, s32 place, u32 prevPlace)
{
    u32 slot = racer;
    u32 kind;
    u32 slotBits;
    if (racer_to_rider_7(slot, &racer) != 0) {
        return;
    }
    if (gGhostRace != 0) {
        return;
    }
    if ((s32)(gRaceFrame - sLastPlaceChangeTime) < ANNOUNCE_PLACE_CHANGE_INTERVAL) {
        return;
    }
    sLastPlaceChangeTime = gRaceFrame;
    if (sSpeechQueueCount >= 6) {
        return;
    }
    speech_queue_push_u32_2(sRiderNameClips[racer + 2 * ANNOUNCER_RIDERS]);
    if (place < (s32)prevPlace) {
        speech_queue_push_u32_2(CLIP_MOVES_UP);
        slotBits = slot << 8;
        kind = SPEECH_PLACE_UP;
    } else {
        speech_queue_push_u32_2(CLIP_DROPS_BACK);
        slotBits = slot << 8;
        kind = SPEECH_PLACE_DOWN;
    }
    speech_queue_push_u32_2(slotBits | (prevPlace | kind));
}

/* "<rider> crosses (the line)". */
void announce_finish(u32 racer)
{
    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    speech_queue_push_u32(sRiderNameClips[racer + 2 * ANNOUNCER_RIDERS]);
    speech_queue_push_u32(CLIP_CROSSES);
}

/* "And it's <rider> with the ..." for the early leader. */
void announce_early_leader(u32 racer)
{
    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 6) {
        return;
    }
    speech_queue_push_u32(CLIP_AND_ITS);
    speech_queue_push_u32(sRiderNameClips[racer]);
    speech_queue_push_u32(CLIP_WITH_THE_2);
}

/* "<rider> has set (the best lap)". */
void announce_best_lap(u32 racer)
{
    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    speech_queue_push_u32(sRiderNameClips[racer]);
    speech_queue_push_u32(CLIP_HAS_SET);
}

/* "<rider> sets a new (record)". */
void announce_record(u32 racer)
{
    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    speech_queue_push_u32(sRiderNameClips[racer]);
    speech_queue_push_u32(CLIP_SETS_A_NEW);
}

/* Soccer: "Goal!", and the crowd cheers in a stadium. */
void announce_goal(void)
{
    speech_queue_push_u32_2(CLIP_GOAL);
    if (gCurTrack_4->f2C == 0) {    /* the track's environment type: a stadium */
        s32 count = sCrowdQueueCount;
        s32 clip = sCrowdCheerClip;
        if (count < CROWD_QUEUE_SIZE) {
            sCrowdQueue[count * 2] = clip;
            sCrowdQueue[count * 2 + 1] = CROWD_DELAY;
            sCrowdQueueCount = count + 1;
        }
    }
}

/* A racer falls off a cliff: plays the cliff fall clip on the ambience channel, holding it until the clip ends. */
void tunes_play_cliff_fall(u32 racer)
{
    StreamChannel *ch;
    StreamSample *table;

    if (racer_to_rider_2(&racer) != 0) {
        return;
    }
    /* not for riders 0 and 3 (Sarah and Vikki, by their name clips) */
    if (racer == 0 || racer == 3) {
        return;
    }
    ch = sStreamChannels;
    ch[STREAM_CH_AMBIENT].sample = CLIP_CLIFF_FALL;
    ch[STREAM_CH_AMBIENT].block = 0;
    table = sStreamSampleTable;
    ch[STREAM_CH_AMBIENT].data = (u8 *)(table[CLIP_CLIFF_FALL].offset + sStreamDataBase);
    ch[STREAM_CH_AMBIENT].size = table[CLIP_CLIFF_FALL].size;
    sAmbientLocked = 1;
    ch[STREAM_CH_AMBIENT].flags = table[CLIP_CLIFF_FALL].flags;
    ch[STREAM_CH_AMBIENT].loop = 0;
    gAmbientVolume_2[0] = gSfxVolume;
}

s32 tunes_is_song_playing(void)
{
    return sCurrentSong != 0;
}

/* Stops the song and frees the clip table, the mix ring and the channels. */
void tunes_shutdown(void)
{
    s32 i;
    if (sCurrentSong != 0) {
        tunes_stop_song();
    }
    res_unload((u32 *)((u8 *)gSfxResSet_2[1] + sStreamSampleTableResId * 4));   /* the clip table's handle */
    i = 0;
    mem_free(sStreamMixBuf);
    if (0 < sStreamChannelCount) {
        do {
            mem_free(sStreamChannels[i].dmaBuf);
            mem_free(sStreamChannels[i].pcm);
            i++;
        } while (i < sStreamChannelCount);
    }
    mem_free(sStreamChannels);
    sTunesActive = 0;
}

/* Plays `clip` on the announcer's channel now unless a clip of `priority` or higher plays (a negative priority can
   be cut by the queue). Returns 0 if it plays. */
int tunes_play_speech(s32 clip, s32 priority, s32 useSfxVolume)
{
    StreamSample *smp;

    if (gPodiumMode != 0) {
        return gPodiumMode;
    }
    if (sStreamChannels[STREAM_CH_SPEECH].sample != -1 && priority <= sSpeechPriority) {
        return 1;
    }
    smp = &sStreamSampleTable[clip];
    sStreamChannels[STREAM_CH_SPEECH].echo = 0;
    sStreamChannels[STREAM_CH_SPEECH].loop = 0;
    sStreamChannels[STREAM_CH_SPEECH].sample = clip;
    sStreamChannels[STREAM_CH_SPEECH].block = 0;
    sStreamChannels[STREAM_CH_SPEECH].data = (u8 *)(smp->offset + sStreamDataBase);
    sSpeechPriority = priority;
    sStreamChannels[STREAM_CH_SPEECH].size = smp->size;
    sSpeechUseSfxVolume = useSfxVolume;
    sStreamChannels[STREAM_CH_SPEECH].flags = smp->flags;
    return 0;
}

/* The frames the queued clips last: 2 samples per byte (twice that at half rate), 60 frames a second. */
s32 tunes_get_speech_queue_time(void)
{
    s32 i, sum, size, samples;

    sum = 0;
    for (i = 0; i < sSpeechQueueCount; i++) {
        size = ((StreamSample *)((u8 *)sStreamSampleTable + ((sSpeechQueue[i] * 5) << 2)))->size;
        samples = size * 2;
        /* the flags of clip `samples`, not of the queued clip */
        if (((StreamSample *)((u8 *)sStreamSampleTable + ((samples * 5) << 2)))->flags & STREAM_HALF_RATE) {
            samples = size << 2;
        }
        sum += (samples * 60) / STREAM_SAMPLE_RATE;
    }
    return sum;
}

/* The index of the clip whose tag is `tag`, -1 if none. */
u32 tunes_find_sample(u32 tag)
{
    StreamSample *rec;
    u32 i = 0;
    u32 count;

    rec = sStreamSampleTable;
    if (rec->count != 0) {
        count = rec->count;
        do {
            if (rec[1].count == tag) {    /* clip i's tag */
                return i;
            }
            i++;
            rec++;
        } while (i < count);
    }
    return 0xFFFFFFFF;
}

static inline void crowd_queue_push(s32 clip, s32 delay)
{
    s32 count = sCrowdQueueCount;
    if (count < CROWD_QUEUE_SIZE) {
        do {
            do {
                sCrowdQueue[count * 2] = clip;
            } while (0);
        } while (0);
        sCrowdQueue[count * 2 + 1] = delay;
        sCrowdQueueCount = count + 1;
    }
}

/* Queues crowd reaction `kind` (sCrowdClips) in a stadium. */
void tunes_queue_crowd(s32 kind)
{
    if (gCurTrack_3[11] == 0) {     /* the track's environment type: a stadium */
        crowd_queue_push(sCrowdClips[kind], CROWD_DELAY);
    }
}

/* Clears the announcer's memory before a race. */
void announce_reset(void)
{
    s32 never = ANNOUNCE_NEVER;
    s32 i = 7;

    do {
        gAnnounceTimes[i] = never;
        i--;
    } while (i >= 0);
    sLastLeader = -1;
    gLastSpeechTime = -1;
    sLastPlaceChangeTime = ANNOUNCE_NEVER;
    sLastLeadCollisionTime = ANNOUNCE_NEVER;
    sCrowdQueueCount = 0;
    sAmbientLocked = 0;
}

/* Queues an announcer clip (not during replays, the credits or the podium; dropped when the queue is full). */
void tunes_queue_speech(s32 clip)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0) {
        s32 count = sSpeechQueueCount;
        if (count < SPEECH_QUEUE_SIZE) {
            sSpeechQueue[count] = clip;
            sSpeechQueueCount = count + 1;
        }
    }
}

/* Silences the announcer: stops its channel, empties the queue and clears its buffers, the echo and the mix ring. */
void tunes_stop_speech(void)
{
    StreamChannel *ch;
    u8 *dmaBuf;

    ch = sStreamChannels;
    dmaBuf = ch[STREAM_CH_SPEECH].dmaBuf;
    ch[STREAM_CH_SPEECH].sample = -1;
    ch[STREAM_CH_SPEECH].block = -1;
    ch[STREAM_CH_SPEECH].bufSample = -1;
    ch[STREAM_CH_SPEECH].bufBlock = -1;
    ch[STREAM_CH_SPEECH].dmaSample = -1;
    ch[STREAM_CH_SPEECH].dmaBlock = -1;
    mem_fill(dmaBuf, 0, STREAM_DMA_BYTES);
    mem_fill(sStreamChannels[STREAM_CH_SPEECH].pcm, 0, STREAM_PCM_BYTES);
    sSpeechQueueCount = 0;
    mem_fill(sStreamEchoBuf, 0, STREAM_ECHO_BUF_BYTES);
    mem_fill(sStreamMixBuf, 0, STREAM_MIX_BUF_BYTES);
}

/* "Last race" or "Starting ..." before the start. */
void announce_race_start(s32 lastRace)
{
    if (lastRace) {
        speech_queue_push_u32_2(CLIP_LAST_RACE);
    } else {
        speech_queue_push_u32_2(CLIP_STARTING);
    }
}

#if VERSION == VERSION_LODGENET
/* n64snd's post-mix callback: adds up to `count` mixed samples to both channels of the output. */
void tunes_mix_callback(s32 *out, s32 count)
{
    if (sStreamMixFill < count) {
        count = sStreamMixFill;
    }
    if (sStreamLock == 0) {
        sStreamMixFill = sStreamMixFill - count;
        while (count != 0) {
            *out = *out + sStreamMixBuf[sStreamMixReadPos];
            out++;
            *out = *out + sStreamMixBuf[sStreamMixReadPos];
            out++;
            sStreamMixReadPos = (sStreamMixReadPos + 1) & STREAM_MIX_MASK;
            count--;
        }
    }
}
#endif

/* Allocates a channel's buffers once. */
void tunes_init_channel(StreamChannel *ch)
{
    if (ch->dmaBuf == 0) {
        ch->dmaBuf = mem_alloc_dbg(STREAM_DMA_BYTES, 0, (s32)&sStrTunesC, SRC_LINE(0x128, 0x131));
        ch->pcm = mem_alloc_dbg(STREAM_PCM_BYTES, 0, (s32)&sStrTunesC, SRC_LINE(0x129, 0x132));
        ch->unk_00 = 0;
        ch->dmaHandle = -1;
        ch->sample = -1;
        ch->bufSample = -1;
    }
}

/* Frees a channel's buffers and clears it. */
void tunes_free_channel(StreamChannel *ch)
{
    if (ch->dmaBuf != 0) {
        mem_free(ch->dmaBuf);
        mem_free(ch->pcm);
        mem_fill(ch, 0, sizeof(StreamChannel));
        ch->dmaHandle = -1;
        ch->sample = -1;
        ch->bufSample = -1;
    }
}

/* Mixes blocks until the mix ring is full or a DMA is needed. */
void tunes_update(void)
{
    if (sTunesActive != 0) {
        while (tunes_mix_streams() != 0) {
        }
    }
}

/* Runs `count` samples through the echo ring: each gets half of the sample a ring earlier added, clamped. */
void tunes_apply_echo(s16 *pcm, s32 count)
{
    s32 sample;
    s32 next;
    s32 rounded;
    for (; count != 0; count--) {
        sample = *pcm + ((s16)sStreamEchoBuf[sStreamEchoPos] >> 1);
        if (sample > 0x7FFF) {
            sample = 0x7FFF;
        }
        if (sample < -0x7FFF) {
            sample = -0x7FFF;
        }
        sStreamEchoBuf[sStreamEchoPos] = sample;
        *pcm = sample;
        pcm++;
        /* sStreamEchoPos = (sStreamEchoPos + 1) % STREAM_ECHO_RING, written out */
        next = sStreamEchoPos + 1;
        rounded = next;
        if (next < 0) {
            rounded = sStreamEchoPos + STREAM_ECHO_RING;
        }
        sStreamEchoPos = next - (rounded >> 11) * STREAM_ECHO_RING;
    }
}

/* Sets each channel's volume from gStreamVolumes and the master volume. */
void tunes_update_volumes(void)
{
    s32 i = 0;

    if (sTunesActive != 0) {
        s32 *volumes = gStreamVolumes_2;
        s32 master = gSndMasterVolume;
        StreamChannel *ch = sStreamChannels;
        do {
            s32 v = *volumes;
            volumes++;
            i++;
            ch->volume = (v * master) >> 8;
            ch++;
        } while (i < STREAM_CHANNELS);
    }
}

/* Whether racer *racer is left out of the announcements (1: the ghost, soccer, no named rider). Otherwise it stamps
   the racer's announce time, replaces *racer with its rider (gGame.riders) and returns 2 in the tutorial, else 0. */
s32 announce_check_rider(u32 *racer)
{
    Game *game;
    u32 rider;
    if (gGhostRace != 0) {
        if (*racer == gGhostSlot) {
            return 1;
        }
    }
    game = &gGame;
    if (game->mode == GAME_MODE_SOCCER) {
        return 1;
    }
    if (*racer >= ANNOUNCE_RACERS) {
        return 1;
    }
    gAnnounceTimes[*racer] = gRaceFrame;
    rider = game->riders[*racer];
    *racer = rider;
    if (rider < ANNOUNCER_RIDERS) {
        return (game->mode == GAME_MODE_TUTORIAL) << 1;
    }
    return 1;
}
