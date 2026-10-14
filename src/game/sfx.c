#include "game.h"

/* The static inline helpers of this file repeat sfx_copy_bytes, sfx_str_icmp, sfx_find_sound, sfx_create_sample
   and sfx_stream_read inside their callers. Each copy keeps the loop and label shape its caller compiled from, so
   they stay separate even where two are the same. */

/* Copies `len` bytes (sfx_stream_write, sfx_stream_read). */
static inline void stream_copy(u8 *dst, u8 *src, u32 len)
{
    for (; len != 0; len--) {
        *dst++ = *src++;
    }
}

/* Compares two names ignoring case: 0 when equal, else 1. */
static inline s32 name_icmp(u8 *a, u8 *b)
{
    for (;;) {
        if (char_to_upper(*a) != char_to_upper(*b)) {
            return 1;
        }
        if (*a++ == 0) {
            return 0;
        }
        b++;
    }
}

/* Loads a sample by name, or adds a reference to the one already loaded under that name; returns the sample or a
   negative error. */
s32 sfx_load_sample_name(u8 *name)
{
    s32 i, sample, info[4];
    SfxSample *samples;
    SfxSample *entry;
    u8 *nameCopy;

    if (sSfxInitialized != 0) {
        for (i = 0; i < SFX_NUM_SAMPLES; i++) {
            if (sSfxSamples[i].name != NULL && name_icmp(sSfxSamples[i].name, name) == 0) {
                if (sSfxSamples[i].refCount >= 0) {
                    sSfxSamples[i].refCount++;
                }
                return i;
            }
        }
        sample = gSfxDriver->loadSampleNamed(name);
        if (sample < 0) {
            return sample;
        }
        if (sample < SFX_NUM_SAMPLES) {
            nameCopy = mem_alloc_dbg(strlen(name) + 1, 0, sStrSfxFile, 0x201);
            samples = sSfxSamples;
            entry = &samples[sample];
            entry->name = nameCopy;
            str_copy(nameCopy, name);
            gSfxDriver->getSampleInfo(sample, info);
            entry->length = info[0];
            entry->loopStart = info[1];
            entry->loopEnd = info[2];
            entry->unk_10 = info[3];
            return sample;
        }
        gSfxDriver->freeSample(sample);
    }
    return -1;
}

/* Loads the "SAM" resource `resId` as a new sample; returns the sample or a negative error. */
s32 sfx_load_sample_id(s32 resId)
{
    s32 sample;
    s32 info[4];
    u8 *samples;
    SndDriver *driver;
    SfxSample *entry;

    if (sSfxInitialized != 0) {
        sample = gSfxDriver->loadSampleIndex(resId);
        if (sample < 0) {
            return sample;
        }
        if (sample < SFX_NUM_SAMPLES) {
            samples = (u8 *)sSfxSamples;
            entry = (SfxSample *)(samples + sample * sizeof(SfxSample));
            driver = gSfxDriver;
            entry->name = 0;
            entry->refCount = 1;
            driver->getSampleInfo(sample, info);
            entry->length = info[0];
            entry->loopStart = info[1];
            entry->loopEnd = info[2];
            entry->unk_10 = info[3];
            return sample;
        }
        gSfxDriver->freeSample(sample);
    }
    return -1;
}

/* Drops a reference to a sample (a locked one keeps its count) and frees it when none is left. */
void sfx_free_sample(s32 sample)
{
    SfxSample *samples;
    SfxSample *entry;

    if (sSfxInitialized != 0) {
        samples = sSfxSamples;
        entry = &samples[sample];
        if (entry->refCount > 0) {
            entry->refCount = entry->refCount - 1;
        }
        if (entry->refCount == 0) {
            if (entry->name != 0) {
                mem_free(entry->name);
            }
            mem_fill((u8 *)entry, 0, sizeof(SfxSample));
            gSfxDriver->freeSample(sample);
        }
    }
}

/* sfx_play calls the driver's sample clock with the table as its argument. */
typedef s32 (*MethodFn)(void *);

/* sfx_update_voices reads and writes a sound's voice flags as volatile. */
#define VOICE_FLAGS(n) (((volatile SfxSound *)(n))->voiceFlags)

/* Starts a sound: takes a free entry of the pool, links it into sSfxSounds by priority and returns its handle (-1:
   none free). It gets a voice at the next sfx_update_voices. `unused` is never read. */
s32 sfx_play(s32 sample, s32 rate, s32 volume, s32 pan, s32 priority, s32 unused)
{
    SfxSound *snd;
    SfxSound *slot;
    SfxSound *next;
    SfxSound *prev;
    SndDriver *driver;
    s32 i;
    s32 numPlaying;
    s32 step;
    s32 clock;
    s32 freeMark;
    s32 prio;
    s32 handle;
    MethodFn getClock;

    if (sSfxInitialized == 0) {
        return -1;
    }
    if (sSfxSamples[sample].length == 0) {
        return -1;
    }
    i = 0;
    critical_enter();
    freeMark = -1; /* the `prev` of a free pool entry */
    /* The empty asm statements hold freeMark, handle and prio in registers of their own, as the original allocated
       them. */
    __asm__ __volatile__("" : "+r"(freeMark));
    slot = sSfxSoundPool;
    for (i = 0; i < SFX_NUM_SOUNDS; i++) {
        if (slot->prev == (SfxSound *)freeMark) {
            slot->prev = NULL;
            break;
        }
        slot++;
    }
    critical_leave();
    if (i == SFX_NUM_SOUNDS) {
        snd = NULL;
    } else {
        snd = &sSfxSoundPool[i];
    }
    if (snd == NULL) {
        return -1;
    }
    mem_fill((u8 *)snd, 0, sizeof(SfxSound));
    handle = sSfxNextHandle;
    __asm__ __volatile__("" : "+r"(handle));
    snd->sample = sample;
    snd->rate = rate;
    snd->volume = volume;
    snd->fadeVolume = volume << SFX_FADE_SHIFT;
    snd->pan = pan;
    prio = priority;
    __asm__ __volatile__("" : "+r"(prio));
    snd->voiceFlags = 0;
    snd->fadeStep = 0;
    __asm__ __volatile__("" : "+r"(prio));
    snd->priority = prio;
    driver = gSfxDriver;
    snd->voice = -1;
    snd->handle = handle;
    getClock = *(MethodFn *)&driver->getSampleClock;
    sSfxNextHandle = handle + 1;
    clock = getClock(driver);
    step = (rate << SFX_POS_SHIFT) / sSfxOutputRate;
    numPlaying = 0;
    snd->clock = clock;
    snd->step = step;
    /* Insert before the first sound of no higher priority (sfx_insert_sound). */
    critical_enter();
    next = sSfxSounds;
    prev = NULL;
    while (next != NULL) {
        prev = next;
        if (prev->priority <= snd->priority) {
            break;
        }
        if (prev->voiceFlags & SFX_VOICE_PLAYING) {
            numPlaying++;
        }
        next = prev->next;
    }
    snd->next = next;
    if (next != NULL) {
        snd->prev = next->prev;
        if (next->prev != NULL) {
            next->prev->next = snd;
        }
        next->prev = snd;
    } else {
        if (prev != NULL) {
            prev->next = snd;
        }
        snd->prev = prev;
    }
    if (sSfxSounds == NULL || next == sSfxSounds) {
        sSfxSounds = snd;
        snd->prev = NULL;
    }
    if (numPlaying < sSfxNumVoices) {
        sSfxVoiceChangeCount++;
    }
    critical_leave();
    return snd->handle;
}

/* The per-buffer update (from sfx_driver_callback): advances the sounds without a voice, steps the fades, ends the
   sounds that stopped, and gives the voices to the first sSfxNumVoices sounds of the list that still play. */
void sfx_update_voices(void)
{
    SfxSound *snd;
    s32 i;
    s32 now;
    s32 dt;
    s32 cnt;
    s32 v;
    u32 pos;
    u32 wrapped;

    if (sSfxInitialized != 0) {
        now = gSfxDriver->getSampleClock();
        snd = sSfxSounds;
        gSfxActiveCount = 0;
        while (snd != NULL) {
            /* i: the output samples since the last update */
            i = now - snd->clock;
            snd->clock = now;
            gSfxActiveCount++;
            if ((VOICE_FLAGS(snd) & SFX_VOICE_PLAYING) && snd->voice == -1) {
                /* A playing sound without a voice moves on by itself, looping or ending as the voice would. */
                v = VOICE_FLAGS(snd) & SFX_VOICE_LOOPED;
                pos = snd->position + i * snd->step;
                snd->position = pos;
                if (v) {
                    s32 loopLen;
                    loopLen = (sSfxSamples[snd->sample].loopEnd - sSfxSamples[snd->sample].loopStart) << SFX_POS_SHIFT;
                    while (snd->position >= (u32)(sSfxSamples[snd->sample].loopEnd << SFX_POS_SHIFT) && loopLen != 0) {
                        snd->position -= loopLen;
                    }
                } else {
                    s32 loopLen;
                    loopLen = (sSfxSamples[snd->sample].loopEnd - sSfxSamples[snd->sample].loopStart) << SFX_POS_SHIFT;
                    if (snd->position >= (u32)(sSfxSamples[snd->sample].length << SFX_POS_SHIFT)) {
                        if (sSfxSamples[snd->sample].loopStart != sSfxSamples[snd->sample].loopEnd) {
                            VOICE_FLAGS(snd) |= SFX_VOICE_LOOPED;
                            wrapped = snd->position;
                            while (wrapped >= (u32)(sSfxSamples[snd->sample].loopEnd << SFX_POS_SHIFT)) {
                                wrapped -= loopLen;
                                snd->position = wrapped;
                            }
                        } else {
                            snd->flags |= SFX_SOUND_STOP;
                        }
                    }
                }
            }
            snd = snd->next;
        }

        /* Fades; a sound faded below 0 stops. */
        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            if (snd->fadeStep != 0) {
                snd->fadeVolume += snd->fadeStep;
                v = snd->fadeVolume / SFX_FADE_ONE;
                snd->volume = v;
                if (v < 0) {
                    snd->volume = 0;
                    snd->flags |= SFX_SOUND_STOP;
                } else {
                    if (v >= 0x80) {
                        snd->volume = SFX_VOLUME_MAX;
                    }
                    if (snd->voice != -1) {
                        gSfxDriver->voiceSetVolume(snd->voice, snd->volume);
                    }
                }
            }
        }

        /* A voice that stopped playing ends its sound. */
        for (i = 0; i < sSfxNumVoices; i++) {
            snd = sSfxVoiceSounds[i];
            if (snd != NULL) {
                v = gSfxDriver->voiceGetFlags(i);
                VOICE_FLAGS(snd) = v;
                if (!(v & SFX_VOICE_PLAYING)) {
                    snd->flags |= SFX_SOUND_STOP;
                }
            }
        }

        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            if (snd->flags & SFX_SOUND_STOP) {
                if (snd->voice != -1) {
                    gSfxDriver->voiceSetFlags(snd->voice, 0);
                    sSfxVoiceSounds[snd->voice] = NULL;
                    snd->voice = -1;
                }
                snd->flags |= SFX_SOUND_ENDED;
            }
        }

        /* The first sSfxNumVoices live sounds of the list want a voice. */
        i = 0;
        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            snd->wantsVoice = 0;
            if (snd->flags & SFX_SOUND_STOP) {
            } else if (snd->flags & SFX_SOUND_ENDED) {
            } else if (i < sSfxNumVoices) {
                snd->wantsVoice = 1;
                i++;
            }
        }

        /* Take the voices from the others, keeping their positions. */
        for (i = 0; i < sSfxNumVoices; i++) {
            if (sSfxVoiceSounds[i] != NULL && sSfxVoiceSounds[i]->wantsVoice == 0) {
                sSfxVoiceSounds[i]->position = gSfxDriver->voiceGetPosition(i) << SFX_POS_SHIFT;
                sSfxVoiceSounds[i]->voice = -1;
                sSfxVoiceSounds[i] = NULL;
                gSfxDriver->voiceSetFlags(i, 0);
            }
        }

        /* Give a free voice to each sound that wants one and has none. */
        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            if (snd->wantsVoice != 0 && snd->voice == -1) {
                for (i = 0; i < sSfxNumVoices; i++) {
                    if (sSfxVoiceSounds[i] == NULL) {
                        break;
                    }
                }
                if (i != sSfxNumVoices) {
                    sSfxVoiceSounds[i] = snd;
                    snd->voice = i;
                    VOICE_FLAGS(snd) = SFX_VOICE_PLAYING;
                    gSfxDriver->voiceSetSample(i, snd->sample);
                    gSfxDriver->voiceSetRate(i, snd->rate);
                    gSfxDriver->voiceSetVolume(i, snd->volume);
                    gSfxDriver->voiceSetPan(i, snd->pan);
                    gSfxDriver->voiceSetPosition(i, snd->position >> SFX_POS_SHIFT);
                    gSfxDriver->voiceSetFlags(i, VOICE_FLAGS(snd));
                }
            }
        }
    }
}

/* Changes a sound's priority: unlinks it and inserts it again at its new place in the list. */
void sfx_set_priority(s32 handle, s32 priority)
{
    SfxSound *snd;
    SfxSound *prev;
    SfxSound *next;
    s32 numPlaying;

    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    snd = sSfxSounds;
    if (snd == 0) {
        goto nf;
    }
loop:
    if (snd->handle == handle) {
        goto found;
    }
    snd = snd->next;
    if (snd != 0) {
        goto loop;
    }
nf:
    critical_leave();
    snd = 0;
join:
    if (snd == 0) {
        goto done;
    }
    snd->priority = priority;
    critical_enter();
    if (snd->prev != 0) {
        snd->prev->next = snd->next;
    }
    if (snd->next != 0) {
        snd->next->prev = snd->prev;
    }
    if (snd == sSfxSounds) {
        sSfxSounds = snd->next;
    }
    numPlaying = 0;
    critical_leave();
    critical_enter();
    prev = 0;
    next = sSfxSounds;
    while (next != 0) {
        prev = next;
        if (prev->priority <= snd->priority) {
            break;
        }
        if ((prev->voiceFlags & SFX_VOICE_PLAYING) != 0) {
            numPlaying = numPlaying + 1;
        }
        next = prev->next;
    }
    snd->next = next;
    if (next == 0) {
        goto nullins;
    }
    snd->prev = next->prev;
    if (next->prev != 0) {
        next->prev->next = snd;
    }
    next->prev = snd;
    goto tail;
found:
    critical_leave();
    goto join;
nullins:
    if (prev != 0) {
        prev->next = snd;
    }
    snd->prev = prev;
tail:
    if (sSfxSounds == 0 || next == sSfxSounds) {
        sSfxSounds = snd;
        snd->prev = 0;
    }
    if (numPlaying < sSfxNumVoices) {
        sSfxVoiceChangeCount = sSfxVoiceChangeCount + 1;
    }
    critical_leave();
done:
    ;
}

/* Unlinks a stream, stops its sound and frees its samples, its ring buffer and itself. */
void sfx_stream_free(SfxStream *stream)
{
    SfxStream *head;
    SfxStream *s;
    s32 handle;
    SfxSound *snd;
    SfxSound *match;

    critical_enter();
    head = sSfxStreams;
    if (head == stream) {
        sSfxStreams = stream->next;
    } else {
        s = head;
        while (s != NULL) {
            if (s->next == stream) {
                s->next = stream->next;
                break;
            }
            s = s->next;
        }
    }
    critical_leave();
    if (stream->handle != -1) {
        handle = stream->handle;
        if (sSfxInitialized != 0) {
            critical_enter();
            for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
                if (snd->handle == handle) {
                    critical_leave();
                    match = snd;
                    goto stop;
                }
            }
            critical_leave();
            match = NULL;
stop:
            if (match != NULL) {
                match->flags |= SFX_SOUND_STOP;
            }
        }
        stream->handle = -1;
    }
    if (stream->samples[0] != -1) {
        sfx_free_sample(stream->samples[0]);
        stream->samples[0] = -1;
    }
    if (stream->samples[1] != -1) {
        sfx_free_sample(stream->samples[1]);
        stream->samples[1] = -1;
    }
    mem_free(stream->buf);
    mem_free((s32)stream);
    return;
}

/* sfx_find_sound, inlined in sfx_stream_start. */
static inline SfxSound *stream_start_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return 0;
}

/* sfx_stop, inlined in sfx_stream_start. */
static inline void stream_start_stop_sound(s32 handle)
{
    SfxSound *snd;

    if (sSfxInitialized != 0) {
        snd = stream_start_find_sound(handle);
        if (snd != 0) {
            snd->flags |= SFX_SOUND_STOP;
        }
    }
}

/* sfx_create_sample without loop points, inlined in sfx_stream_start. */
static inline s32 stream_start_create_sample(u8 *data, u32 length, s32 rate)
{
    s32 sample;
    SfxSample *entry;
    s32 info[4];

    if (sSfxInitialized == 0) {
        return -1;
    }
    sample = gSfxDriver->createSample(data, length, 0, 0, rate);
    if (sample < 0) {
        return sample;
    }
    if (sample >= SFX_NUM_SAMPLES) {
        gSfxDriver->freeSample(sample);
        return -1;
    }
    {
        SfxSample *samples = sSfxSamples;
        entry = &samples[sample];
    }
    entry->name = 0;
    entry->refCount = 1;
    gSfxDriver->getSampleInfo(sample, info);
    entry->length = info[0];
    entry->loopStart = info[1];
    entry->loopEnd = info[2];
    entry->unk_10 = info[3];
    return sample;
}

/* (Re)starts a stream: stops its sound, and creates its two samples of one buffer's worth of data at `rate`. */
void sfx_stream_start(SfxStream *stream, s32 rate, s32 volume, s32 pan, s32 priority)
{
    SfxSound *unusedSound;
    s32 unusedHandle;

    if (stream->handle != -1) {
        stream_start_stop_sound(stream->handle);
        stream->handle = -1;
    }
    if (stream->samples[0] != -1) {
        sfx_free_sample(stream->samples[0]);
        stream->samples[0] = -1;
    }
    if (stream->samples[1] != -1) {
        sfx_free_sample(stream->samples[1]);
        stream->samples[1] = -1;
    }
    stream->cur = 1;
    stream->rate = rate;
    stream->volume = volume;
    stream->pan = pan;
    stream->priority = priority;
    stream->chunkSize = (stream->rate * sSfxFrameSamples) / sSfxOutputRate;
    stream->samples[0] = stream_start_create_sample(sSfxStreamBuf, stream->chunkSize, stream->rate);
    stream->samples[1] = stream_start_create_sample(sSfxStreamBuf, stream->chunkSize, stream->rate);
    if (stream->samples[0] == -1 || stream->samples[1] == -1) {
        fatal_halt(1);
    } else {
        stream->filled[0] = 0;
    }
    *(volatile u8 *)&stream->filled[1] = 0;
}

/* sfx_find_sound, inlined in sfx_stream_update. */
static __inline__ SfxSound *stream_update_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return 0;
}

/* sfx_copy_bytes, inlined in the two ring reads of sfx_stream_update. */
static __inline__ void stream_update_copy(u8 *dst, u8 *src, u32 n)
{
    while (n != 0) {
        *dst++ = *src++;
        n--;
    }
}

/* sfx_stream_read, inlined in sfx_stream_update for the sample that plays next; reads all `size` bytes or none. */
static __inline__ u32 stream_update_read_next(SfxStream *stream, u8 *dst, u32 size)
{
    u32 firstPart;
    u32 n = size;
    if (stream->count < n) {
fail:
        n = 0;
        goto done;
    }
    if (n == 0) goto fail;
    if (stream->size < stream->readPos + n) {
        firstPart = stream->size - stream->readPos;
        stream_update_copy(dst, stream->buf + stream->readPos, firstPart);
        stream_update_copy(dst + firstPart, stream->buf, n - firstPart);
        stream->readPos = n - firstPart;
    } else {
        stream_update_copy(dst, stream->buf + stream->readPos, n);
        stream->readPos += n;
    }
    stream->readPos = stream->readPos % stream->size;
    stream->count -= n;
done:
    return n;
}

/* The same for the sample that just finished. */
static __inline__ u32 stream_update_read_current(SfxStream *stream, u8 *dst, u32 size)
{
    u32 firstPart;
    u32 n = size;
    if (stream->count < n) goto fail;
    if (n != 0) goto ok;
fail:
    n = 0;
    goto done;
ok:
    if (stream->size < stream->readPos + n) {
        firstPart = stream->size - stream->readPos;
        stream_update_copy(dst, stream->buf + stream->readPos, firstPart);
        stream_update_copy(dst + firstPart, stream->buf, n - firstPart);
        stream->readPos = n - firstPart;
    } else {
        stream_update_copy(dst, stream->buf + stream->readPos, n);
        stream->readPos += n;
    }
    stream->readPos = stream->readPos % stream->size;
    stream->count -= n;
done:
    return n;
}

/* The per-buffer step of a stream (from sfx_driver_callback): stops the sound of the last buffer, plays the other
   sample if it holds data (filling it from the ring first if needed), and refills the sample just played. */
void sfx_stream_update(SfxStream *stream)
{
    s32 handle;
    SfxSound *snd;
    u32 got;

    if (stream->samples[0] == -1) {
        return;
    }
    if (stream->handle != -1) {
        handle = stream->handle;
        if (sSfxInitialized != 0) {
            snd = stream_update_find_sound(handle);
            if (snd != 0) {
                snd->flags |= SFX_SOUND_STOP;
            }
        }
        stream->handle = -1;
    }
    stream->filled[stream->cur] = 0;
    if (stream->filled[1 - stream->cur] == 0) {
        if ((got = stream_update_read_next(stream, sSfxStreamBuf, stream->chunkSize)) != 0) {
            sfx_write_sample(stream->samples[1 - stream->cur], sSfxStreamBuf, got);
            stream->filled[1 - stream->cur] = 1;
        }
    }
    if (stream->filled[1 - stream->cur] != 0) {
        stream->handle = sfx_play(stream->samples[1 - stream->cur], stream->rate, stream->volume, stream->pan,
                                  stream->priority, 0);
    }
    if ((got = stream_update_read_current(stream, sSfxStreamBuf, stream->chunkSize)) != 0) {
        sfx_write_sample(stream->samples[stream->cur], sSfxStreamBuf, got);
        stream->filled[stream->cur] = 1;
    }
    stream->cur = 1 - stream->cur;
}

void sfx_get_driver_caps(s32 *voices, s32 *modes, s32 *rate, s32 *frameMs)
{
    gSfxDriver->getCaps(voices, modes, rate, frameMs);
}

/* Starts the driver (`mode`: SFX_MODE_*) and the sfx layer; returns 0, or the driver's error. */
s32 sfx_init(s32 numVoices, s32 mode, s32 rate, s32 frameMs, s32 *frameSamples, s32 *outRate)
{
    s32 err;
    u8 *voiceSounds;
    s32 numFrameSamples, outputRate;

    err = gSfxDriver->init(numVoices, mode, rate, frameMs, frameSamples, outRate);
    if (err != 0) {
        return err;
    }
    voiceSounds = mem_alloc_dbg(numVoices * 4, 0, sStrSfxFile, 0x19B);
    sSfxVoiceSounds = voiceSounds;
    if (voiceSounds == NULL) {
        gSfxDriver->shutdown();
        return 1;
    }
    sSfxVoiceSounds = voiceSounds;
    sSfxNumVoices = numVoices;
    sSfxDriverFlags = mode;
    outputRate = *outRate;
    sSfxFrameMs = frameMs;
    sSfxOutputRate = outputRate;
    numFrameSamples = *frameSamples;
    sSfxSounds = 0;
    sSfxFrameSamples = numFrameSamples;
    mem_fill(voiceSounds, 0, numVoices * 4);
    mem_fill((u8 *)sSfxSamples, 0, sizeof(sSfxSamples));
    /* Every pool entry's prev becomes -1: free. */
    mem_fill((u8 *)sSfxSoundPool, 0xFF, sizeof(sSfxSoundPool));
    gSfxDriver->setCallback(sfx_driver_callback);
    sSfxInitialized = 1;
    sSfxStreams = 0;
    return 0;
}

void sfx_shutdown(void)
{
    gSfxDriver->shutdown();
    sSfxInitialized = 0;
}

/* Creates a sample from `length` bytes of data; returns the sample or a negative error. */
s32 sfx_create_sample(u8 *data, u32 length, s32 loopStart, s32 loopEnd, s32 rate)
{
    s32 sample;
    s32 info[4];
    SfxSample *entry;
    SfxSample *samples;

    if (sSfxInitialized != 0) {
        sample = gSfxDriver->createSample(data, length, loopStart, loopEnd, rate);
        if (sample < 0) {
            return sample;
        }
        if (sample < SFX_NUM_SAMPLES) {
            samples = sSfxSamples;
            entry = samples + sample;
            entry->name = 0;
            entry->refCount = 1;
            gSfxDriver->getSampleInfo(sample, info);
            entry->length = info[0];
            entry->loopStart = info[1];
            entry->loopEnd = info[2];
            entry->unk_10 = info[3];
            return sample;
        }
        gSfxDriver->freeSample(sample);
    }
    return -1;
}

/* Frees every sample that is not locked, whatever its reference count. */
void sfx_free_all_samples(void)
{
    s32 i;

    if (sSfxInitialized != 0) {
        i = 0;
        do {
            if (0 < sSfxSamples[i].refCount) {
                sSfxSamples[i].refCount = 1;
                sfx_free_sample(i);
            }
            i++;
        } while (i < SFX_NUM_SAMPLES);
    }
}

/* Driver pass-throughs. */
void sfx_get_sample_info(s32 sample, s32 *info)
{
    if (sSfxInitialized != 0) {
        gSfxDriver->getSampleInfo(sample, info);
    }
}

void sfx_set_sample_info(s32 sample, s32 *info)
{
    if (sSfxInitialized != 0) {
        gSfxDriver->setSampleInfo(sample, info);
    }
}

void sfx_write_sample(s32 sample, u8 *data, u32 size)
{
    gSfxDriver->writeSample(sample, data, size);
}

/* sfx_find_sound, inlined in sfx_stop. */
static inline SfxSound *stop_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return NULL;
}

/* Asks a sound to stop; sfx_update_voices ends it. */
void sfx_stop(s32 handle)
{
    if (sSfxInitialized != 0) {
        SfxSound *snd = stop_find_sound(handle);

        if (snd != NULL) {
            snd->flags |= SFX_SOUND_STOP;
        }
    }
}

/* Asks every sound to stop (sfx_update_voices ends them). */
void sfx_stop_all(void)
{
    SfxSound *snd;

    critical_enter();
    snd = sSfxSounds;
    if (snd != NULL) {
        do {
            snd->flags |= SFX_SOUND_STOP;
            snd = snd->next;
        } while (snd != NULL);
    }
    critical_leave();
}

/* Pauses every sound by setting its rate to 0 (the rate is kept in pausedRate). */
void sfx_pause_all(void)
{
    SfxSound *snd;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            if (snd->rate != 0) {
                snd->pausedRate = snd->rate;
                snd->rate = 0;
                snd->step = (snd->rate << SFX_POS_SHIFT) / sSfxOutputRate;
                if (snd->voice != -1) {
                    gSfxDriver->voiceSetRate(snd->voice, snd->rate);
                }
            }
        }
        critical_leave();
    }
}

/* Gives every paused sound its rate back. */
void sfx_resume_all(void)
{
    SfxSound *snd;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
            if (snd->rate == 0) {
                snd->rate = snd->pausedRate;
                snd->step = (snd->rate << SFX_POS_SHIFT) / sSfxOutputRate;
                if (snd->voice != -1) {
                    gSfxDriver->voiceSetRate(snd->voice, snd->rate);
                }
            }
        }
        critical_leave();
    }
}

/* Calls of the driver's empty slots. */
void sfx_driver_stub2(void)
{
    gSfxDriver->unk_68();
}

void sfx_driver_stub3(void)
{
    gSfxDriver->unk_6C();
}

/* The setters and getters below find the sound by its handle with sfx_find_sound written out in each; a getter
   returns -1 when there is no such sound. */
void sfx_set_volume(s32 handle, s32 volume)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != 0; snd = snd->next) {
            if (snd->handle == handle) {
                critical_leave();
                match = snd;
                goto found;
            }
        }
        critical_leave();
        match = 0;
    found:
        if (match != 0) {
            match->volume = volume;
            match->fadeVolume = volume << SFX_FADE_SHIFT;
            if (match->voice != -1) {
                gSfxDriver->voiceSetVolume(match->voice, volume);
            }
        }
    }
}

/* Sets a sound's pan (0-15), and its voice's if it has one. */
void sfx_set_pan(s32 handle, s32 pan)
{
    SfxSound *snd;
    SfxSound *match;
    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != 0; snd = snd->next) {
            if (snd->handle == handle) {
                critical_leave();
                match = snd;
                goto common;
            }
        }
        critical_leave();
        match = 0;
common:
        if (match != 0) {
            match->pan = pan;
            if (match->voice != -1) {
                gSfxDriver->voiceSetPan(match->voice, pan);
            }
        }
    }
}

/* sfx_find_sound, inlined in sfx_set_rate. */
static inline SfxSound *set_rate_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return NULL;
}

/* Sets a sound's rate in Hz and the position step that follows from it. */
void sfx_set_rate(s32 handle, s32 rate)
{
    SfxSound *snd;

    if (sSfxInitialized != 0) {
        snd = set_rate_find_sound(handle);
        if (snd != NULL) {
            snd->rate = rate;
            snd->step = (snd->rate << SFX_POS_SHIFT) / sSfxOutputRate;
            if (snd->voice != -1) {
                gSfxDriver->voiceSetRate(snd->voice, snd->rate);
            }
        }
    }
}

/* Moves a sound to sample `position`. */
void sfx_set_position(s32 handle, u32 position)
{
    SfxSound *snd;
    SfxSound *match;
    u32 pos;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != 0; snd = snd->next) {
            if (snd->handle == handle) {
                critical_leave();
                match = snd;
                goto found;
            }
        }
        critical_leave();
        match = 0;
    found:
        if (match != 0) {
            pos = position << SFX_POS_SHIFT;
            match->position = pos;
            if (match->voice != -1) {
                gSfxDriver->voiceSetPosition(match->voice, pos >> SFX_POS_SHIFT);
            }
        }
    }
}

/* Sets the function called with the sound's handle on the events in `events` (SFX_EVENT_END). */
void sfx_set_end_callback(s32 handle, s32 events, void (*callback)(s32, s32))
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != 0; snd = snd->next) {
            if (snd->handle == handle) {
                critical_leave();
                match = snd;
                goto found;
            }
        }
        critical_leave();
        match = 0;
    found:
        if (match != 0) {
            match->endCallback = callback;
            match->endCallbackFlags = events;
        }
    }
}

/* Fades a sound's volume by `perSecond` (volume units per second, negative to fade out). */
void sfx_set_fade(s32 handle, s32 perSecond)
{
    SfxSound *snd;
    SfxSound *match;
    s32 scaled;

    if (sSfxInitialized == 0) {
        return;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto common;
        }
    }
    critical_leave();
    match = 0;
common:
    if (match != 0) {
        scaled = perSecond * sSfxFrameMs;
        match->fadeVolume = match->volume << SFX_FADE_SHIFT;
        match->fadeStep = (scaled * SFX_FADE_ONE) / 1000;
    }
}

/* The getters: a sound's volume, pan, rate or priority, -1 when there is no such sound. */
s32 sfx_get_volume(s32 handle)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto found;
        }
    }
    critical_leave();
    match = NULL;
found:
    if (match == NULL) {
        return -1;
    }
    return match->volume;
}

s32 sfx_get_pan(s32 handle)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto found;
        }
    }
    critical_leave();
    match = 0;
found:
    if (match == 0) {
        return -1;
    }
    return match->pan;
}

/* Returns a sound's rate, the one it had before sfx_pause_all while paused. */
s32 sfx_get_rate(s32 handle)
{
    SfxSound *snd;
    SfxSound *found;
    s32 rate;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            found = snd;
            goto check;
        }
    }
    critical_leave();
    found = 0;
check:
    rate = -1;
    if (found != 0) {
        rate = found->rate;
        if (rate == 0) {
            rate = found->pausedRate;
        }
    }
    return rate;
}

s32 sfx_get_priority(s32 handle)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    snd = sSfxSounds;
    while (snd != NULL) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto done;
        }
        snd = snd->next;
    }
    critical_leave();
    match = NULL;
done:
    if (match == NULL) {
        return -1;
    }
    return match->priority;
}

/* The driver's count of audio buffers mixed. */
s32 sfx_get_frame_count(void)
{
    if (sSfxInitialized == 0) {
        return -1;
    }
    return gSfxDriver->getBufferClock();
}

/* Unlinks the sounds that ended and returns them to the pool, calling their end callbacks first. */
void sfx_reap_sounds(void)
{
    SfxSound *snd;
    SfxSound *next;

    if (sSfxInitialized != 0) {
        critical_enter();
        snd = sSfxSounds;
        while (snd != 0) {
            next = snd->next;
            if (snd->flags & SFX_SOUND_ENDED) {
                if (snd->endCallback != 0 && (snd->endCallbackFlags & SFX_EVENT_END)) {
                    snd->endCallback(snd->handle, SFX_EVENT_END);
                }
                critical_enter();
                if (snd->prev != 0) {
                    snd->prev->next = snd->next;
                }
                if (snd->next != 0) {
                    snd->next->prev = snd->prev;
                }
                if (snd == sSfxSounds) {
                    sSfxSounds = snd->next;
                }
                critical_leave();
                snd->prev = (SfxSound *)-1;
            }
            snd = next;
        }
        critical_leave();
    }
}

/* The function sfx_driver_callback calls first, before each buffer is mixed. */
void sfx_set_frame_callback(void (*callback)(void))
{
    if (sSfxInitialized != 0) {
        sSfxFrameCallback = callback;
    }
}

void (*sfx_get_frame_callback(void))(void)
{
    if (sSfxInitialized == 0) {
        return 0;
    }
    return sSfxFrameCallback;
}

/* Echo: driver pass-throughs. */
void sfx_echo_init(s32 size)
{
    gSfxDriver->echoInit(size);
}

void sfx_echo_set_level(s32 level)
{
    gSfxDriver->echoSetLevel(level);
}

void sfx_echo_free(void)
{
    gSfxDriver->echoFree();
}

void sfx_driver_stub1(void)
{
    gSfxDriver->unk_64();
}

/* Creates a stream with a ring buffer of `size` bytes and links it into sSfxStreams; sfx_stream_start starts it. */
SfxStream *sfx_stream_create(s32 size)
{
    SfxStream *stream;
    u8 *buf;
    SfxStream *head;

    stream = mem_alloc_dbg(sizeof(SfxStream), 0, sStrSfxFile, 0x6AC);
    if (stream != NULL) {
        buf = mem_alloc_dbg(size, 0, sStrSfxFile, 0x6AF);
        stream->buf = buf;
        if (buf != NULL) {
            stream->size = size;
            stream->readPos = 0;
            stream->writePos = 0;
            stream->count = 0;
            stream->handle = -1;
            stream->samples[0] = -1;
            stream->samples[1] = -1;
            critical_enter();
            head = sSfxStreams;
            sSfxStreams = stream;
            stream->next = head;
            critical_leave();
            return stream;
        }
        mem_free(stream);
    }
    return NULL;
}

/* Appends `size` bytes to a stream's ring buffer (there is no check for overflow). */
void sfx_stream_write(SfxStream *stream, u32 size, u8 *src)
{
    u32 firstPart;

    critical_enter();
    if (stream->size < stream->writePos + size) {
        firstPart = stream->size - stream->writePos;
        stream_copy(stream->buf + stream->writePos, src, firstPart);
        stream_copy(stream->buf, src + firstPart, size - firstPart);
        stream->writePos = size - firstPart;
    } else {
        stream_copy(stream->buf + stream->writePos, src, size);
        stream->writePos += size;
    }
    stream->writePos %= stream->size;
    stream->count += size;
    critical_leave();
}

/* Sets a stream's rate and the bytes it plays per output frame. */
void sfx_stream_set_rate(SfxStream *stream, s32 rate)
{
    u32 chunkSize = (u32)(rate * sSfxFrameSamples) / sSfxOutputRate;
    stream->rate = rate;
    stream->chunkSize = chunkSize;
}

/* sfx_find_sound, inlined in sfx_stream_set_volume. */
static inline SfxSound *stream_volume_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return NULL;
}

/* Sets a stream's volume, and its playing sound's (sfx_set_volume). */
void sfx_stream_set_volume(SfxStream *stream, s32 volume)
{
    SfxSound *snd;
    s32 handle;

    stream->volume = volume;
    critical_enter();
    if (stream->handle != -1) {
        handle = stream->handle;
        if (sSfxInitialized == 0) {
            goto out;
        }
        snd = stream_volume_find_sound(handle);
        if (snd != NULL) {
            snd->volume = volume;
            snd->fadeVolume = volume << SFX_FADE_SHIFT;
            if (snd->voice != -1) {
                gSfxDriver->voiceSetVolume(snd->voice, volume);
            }
        }
    }
out:
    critical_leave();
}

/* Sets a stream's pan, and its playing sound's (sfx_set_pan). */
void sfx_stream_set_pan(SfxStream *stream, s32 pan)
{
    SfxSound *snd;
    SfxSound *match;
    s32 cur;
    s32 handle;

    stream->pan = pan;
    critical_enter();
    cur = stream->handle;
    if (cur == -1) {
        goto done;
    }
    handle = cur;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto common;
        }
    }
    critical_leave();
    match = 0;
common:
    if (match != 0) {
        match->pan = pan;
        if (match->voice != -1) {
            gSfxDriver->voiceSetPan(match->voice, pan);
        }
    }
done:
    critical_leave();
}

/* Sets the stream's priority byte but, like sfx_stream_set_pan, the sound's pan (an original bug). */
void sfx_stream_set_priority(SfxStream *stream, s32 priority)
{
    SfxSound *snd;
    SfxSound *match;
    s32 cur;
    s32 handle;

    stream->priority = priority;
    critical_enter();
    cur = stream->handle;
    if (cur == -1) {
        goto done;
    }
    handle = cur;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != 0; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto common;
        }
    }
    critical_leave();
    match = 0;
common:
    if (match != 0) {
        match->pan = priority;
        if (match->voice != -1) {
            gSfxDriver->voiceSetPan(match->voice, priority);
        }
    }
done:
    critical_leave();
}

void sfx_mute(void)
{
    gSfxDriver->setMute(1);
}

void sfx_unmute(void)
{
    gSfxDriver->setMute(0);
}

/* The driver's callback before each buffer is mixed: the frame callback, the streams, then the voices. */
void sfx_driver_callback(void)
{
    SfxStream *stream;

    if (sSfxInitialized != 0) {
        if (sSfxFrameCallback != 0) {
            sSfxFrameCallback();
        }
        stream = sSfxStreams;
        while (stream != 0) {
            sfx_stream_update(stream);
            stream = stream->next;
        }
        sfx_update_voices();
    }
}

/* Copies n bytes. */
void sfx_copy_bytes(u8 *dst, u8 *src, s32 n)
{
    while (n != 0) {
        *dst++ = *src++;
        n = n - 1;
    }
}

/* Compares two strings ignoring case: 0 when equal, else 1. */
s32 sfx_str_icmp(u8 *a, u8 *b)
{
    s32 ca;
    s32 cb;
    for (;;) {
        ca = char_to_upper(*a);
        cb = char_to_upper(*b);
        if (ca != cb) {
            return 1;
        }
        if (*a == 0) {
            return 0;
        }
        a++;
        b++;
    }
}

/* Writes the handle of each voice's sound (-1: none), then the number of sounds in the list. */
void sfx_get_voice_handles(s32 *out)
{
    s32 i;
    for (i = 0; i < sSfxNumVoices; i++) {
        if (sSfxVoiceSounds[i] != NULL) {
            out[i] = sSfxVoiceSounds[i]->handle;
        } else {
            out[i] = -1;
        }
    }
    out[sSfxNumVoices] = gSfxActiveCount;
}

/* Takes a free entry of sSfxSoundPool (prev -1: free); NULL when none is. */
SfxSound *sfx_alloc_sound(void)
{
    s32 i;
    critical_enter();
    for (i = 0; i < SFX_NUM_SOUNDS; i++) {
        if (sSfxSoundPool[i].prev == (SfxSound *)-1) {
            sSfxSoundPool[i].prev = NULL;
            break;
        }
    }
    critical_leave();
    if (i == SFX_NUM_SOUNDS) {
        return NULL;
    }
    return &sSfxSoundPool[i];
}

void sfx_free_sound(SfxSound *snd)
{
    snd->prev = (SfxSound *)-1;
}

/* Links a sound into sSfxSounds before the first sound of no higher priority. A voice change is counted when fewer
   than sSfxNumVoices playing sounds are ahead of it. */
void sfx_insert_sound(SfxSound *snd)
{
    SfxSound *prev;
    SfxSound *next;
    s32 numPlaying;

    numPlaying = 0;
    critical_enter();
    prev = 0;
    next = sSfxSounds;
    if (next != 0) {
        s32 priority = snd->priority;
        do {
            prev = next;
            if (priority < prev->priority) {
                if (prev->voiceFlags & SFX_VOICE_PLAYING) numPlaying++;
                next = prev->next;
            } else {
                break;
            }
        } while (next != 0);
    }
    snd->next = next;
    if (next != 0) {
        snd->prev = next->prev;
        if (next->prev != 0) next->prev->next = snd;
        next->prev = snd;
    } else {
        if (prev != 0) prev->next = snd;
        snd->prev = prev;
    }
    if (sSfxSounds == 0 || next == sSfxSounds) {
        sSfxSounds = snd;
        snd->prev = 0;
    }
    if (numPlaying < sSfxNumVoices) {
        sSfxVoiceChangeCount++;
    }
    critical_leave();
}

/* Takes a sound out of sSfxSounds. */
void sfx_unlink_sound(SfxSound *snd)
{
    critical_enter();
    if (snd->prev != NULL) {
        snd->prev->next = snd->next;
    }
    if (snd->next != NULL) {
        snd->next->prev = snd->prev;
    }
    if (snd == sSfxSounds) {
        sSfxSounds = snd->next;
    }
    critical_leave();
}

/* Returns the sound with this handle, NULL when there is none. */
SfxSound *sfx_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    snd = sSfxSounds;
    while (snd != NULL) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
        snd = snd->next;
    }
    critical_leave();
    return 0;
}

/* sfx_find_sound, inlined in sfx_get_position. */
static inline SfxSound *get_position_find_sound(s32 handle)
{
    SfxSound *snd;

    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            return snd;
        }
    }
    critical_leave();
    return NULL;
}

/* Returns a sound's sample position, -1 when it is not playing. The original does not check that the sound
   exists. */
u32 sfx_get_position(s32 handle)
{
    SfxSound *snd;

    if (sSfxInitialized == 0) {
        return -1;
    }
    snd = get_position_find_sound(handle);
    if (!(snd->voiceFlags & SFX_VOICE_PLAYING)) {
        return -1;
    }
    if (snd->voice == -1) {
        return snd->position >> SFX_POS_SHIFT;
    }
    return gSfxDriver->voiceGetPosition(snd->voice);
}

/* Changes the sample a sound plays. */
void sfx_set_sample(s32 handle, s32 sample)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (snd = sSfxSounds; snd != 0; snd = snd->next) {
            if (snd->handle == handle) {
                critical_leave();
                match = snd;
                goto found;
            }
        }
        critical_leave();
        match = 0;
    found:
        if (match != 0) {
            match->sample = sample;
            if (match->voice != -1) {
                gSfxDriver->voiceSetSample(match->voice, sample);
            }
        }
    }
}

void sfx_echo_enable(s32 on)
{
    gSfxDriver->echoEnable(on);
}

/* Takes `size` bytes from a stream's ring buffer: all of them, or none (returns 0) when fewer are there. */
u32 sfx_stream_read(SfxStream *stream, u32 size, u8 *dst)
{
    u32 firstPart;

    if (stream->count < size) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }
    if (stream->size < stream->readPos + size) {
        firstPart = stream->size - stream->readPos;
        stream_copy(dst, stream->buf + stream->readPos, firstPart);
        stream_copy(dst + firstPart, stream->buf, size - firstPart);
        stream->readPos = size - firstPart;
    } else {
        stream_copy(dst, stream->buf + stream->readPos, size);
        stream->readPos += size;
    }
    stream->readPos %= stream->size;
    stream->count -= size;
    return size;
}

/* A locked sample (refCount -1) is never freed by sfx_free_sample; unlocking leaves one reference. */
void sfx_lock_sample(s32 sample)
{
    sSfxSamples[sample].refCount = -1;
}

void sfx_unlock_sample(s32 sample)
{
    sSfxSamples[sample].refCount = 1;
}

/* Returns the driver voice of a sound, -1 when it has none or does not exist. */
s32 sfx_get_voice(s32 handle)
{
    SfxSound *snd;
    SfxSound *match;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (snd = sSfxSounds; snd != NULL; snd = snd->next) {
        if (snd->handle == handle) {
            critical_leave();
            match = snd;
            goto found;
        }
    }
    critical_leave();
    match = NULL;
found:
    if (match == NULL) {
        return -1;
    }
    return match->voice;
}
