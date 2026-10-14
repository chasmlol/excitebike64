#include "game.h"

/* Starts the AI and the mixer (the driver's init). Only stereo 16-bit output is accepted. An AI buffer holds two
   frames of output (`frameMs` at the rate the AI gives, rounded up to 64 samples); both are queued silent. Returns 0,
   1 or the mixer's error. */
s32 n64snd_init(s32 numVoices, s32 mode, s32 rate, s32 frameMs, u32 *frameSamples, s32 *outRate)
{
    s32 bytesPerSample;
    s32 err;

    gN64SndSilenceFrames = 0;
    sN64SndMuted = 0;
    if ((mode & SFX_MODE_STEREO_16BIT) != SFX_MODE_STEREO_16BIT) {
        return 1;
    }
    {
        sN64SndMode = mode;
        sN64SndFrequency = (s32)osAiSetFrequency(rate);
        bytesPerSample = 1;
        *outRate = sN64SndFrequency;
        if (mode & SFX_MODE_STEREO) {
            bytesPerSample = 2;
        }
        if (mode & SFX_MODE_16BIT) {
            bytesPerSample = bytesPerSample << 1;
        }
        sN64SndFrames = ((((u32)(sN64SndFrequency * frameMs) / 1000 + 0x3f) >> 6) << 6);
        *frameSamples = sN64SndFrames;
        sN64SndFrames = sN64SndFrames * 2;
        sN64SndAiBufSize = bytesPerSample * sN64SndFrames;
        sN64SndMixBuf = (s32)mem_alloc_dbg(sN64SndAiBufSize * 4, 0, sStrN64sndC, 0xd7);
        sN64SndAiBuf = (s32 *)mem_alloc_dbg(sN64SndAiBufSize * 2 + 0x10, 0, sStrN64sndC, 0xd9);
        sN64SndAiBufAlloc = sN64SndAiBuf;
        /* An allocation on a 256-byte boundary is moved up 16 bytes (the extra 0x10 allocated). This also turns a
           failed allocation into 0x10, so the NULL check below never fails. */
        if (((u32)sN64SndAiBufAlloc & 0xff) == 0) {
            sN64SndAiBuf = sN64SndAiBufAlloc + 4;
        }
        if (sN64SndAiBuf == 0) {
            return 1;
        }
        {
            err = softmix_init(numVoices, mode, rate, frameMs);
            if (err != 0) {
                mem_free((s32)sN64SndAiBufAlloc);
                mem_free(sN64SndMixBuf);
                return err;
            }
            sN64SndCallback = 0;
            sN64SndBufIndex = 0;
            mem_fill((u8 *)sN64SndAiBuf, 0, sN64SndAiBufSize << 1);
            osAiSetNextBuffer((s32)sN64SndAiBuf, sN64SndAiBufSize);
            osAiSetNextBuffer((s32)sN64SndAiBuf + sN64SndAiBufSize, sN64SndAiBufSize);
            sN64SndActive = 1;
            return 0;
        }
    }
}

/* When the AI has room for another buffer: queues the other AI buffer, then mixes it in two halves (pre-mix
   callback, mixer, post-mix callback for each), converts the mix into it and writes it back. While muted or
   during the silent frames it is cleared after the mix. */
void n64snd_update(void)
{
    s32 status;
    s32 *mix;
    u32 half;
    s32 i;

    status = osAiGetStatus();
    /* AI_STATUS_FIFO_FULL (bit 31) clear */
    if (status >= 0) {
        i = 0;
        /* An idle AI DMA has played both buffers out. */
        if ((status & AI_STATUS_DMA_BUSY) == 0) {
            sN64SndUnderruns = sN64SndUnderruns + 1;
        }
        sN64SndBufIndex = sN64SndBufIndex ^ 1;
        osAiSetNextBuffer(sN64SndAiBuf + sN64SndAiBufSize * sN64SndBufIndex, sN64SndAiBufSize);
        mix = sN64SndMixBuf;
        do {
            if (sN64SndCallback != 0) {
                (*sN64SndCallback)();
            }
            softmix_mix(mix, sN64SndFrames >> 1);
            if (gN64SndMixCallback != 0) {
                (*gN64SndMixCallback)(mix, sN64SndFrames >> 1);
            }
            half = sN64SndFrames >> 1;
            if (sN64SndMode & SFX_MODE_STEREO) {
                mix += half * 2;
            } else {
                mix += half;
            }
            i = i + 1;
        } while (i < 2);
        softmix_convert_output((s32)sN64SndMixBuf, (s32)(sN64SndAiBuf + sN64SndAiBufSize * sN64SndBufIndex), sN64SndFrames);
        if (gN64SndSilenceFrames != 0 || sN64SndMuted != 0) {
            mem_fill(sN64SndAiBuf + sN64SndAiBufSize * sN64SndBufIndex, 0, sN64SndAiBufSize);
            if (gN64SndSilenceFrames != 0) {
                gN64SndSilenceFrames = gN64SndSilenceFrames - 1;
            }
        }
        osWritebackDCache(sN64SndAiBuf + sN64SndAiBufSize * sN64SndBufIndex, sN64SndAiBufSize);
        gN64SndBufferCount = gN64SndBufferCount + 1;
    }
}

/* The mixer's capabilities, limited to stereo 16-bit, 20 ms frames and SFX_MAX_RATE. */
s32 n64snd_get_caps(s32 *voices, s32 *modes, s32 *rate, s32 *frameMs)
{
    softmix_get_caps(voices, modes, rate);
    *modes = SFX_MODE_STEREO_16BIT;
    *frameMs = 20;
    if (*rate > SFX_MAX_RATE) {
        *rate = SFX_MAX_RATE;
    }
    return 0;
}

/* Stops the driver: the mixer and the mix and AI buffers. */
void n64snd_shutdown(void)
{
    sN64SndActive = 0;
    softmix_shutdown();
    mem_free(sN64SndMixBuf);
    mem_free(sN64SndAiBufAlloc);
}

/* Samples, voices, clocks and echo: the mixer's functions. */
s32 n64snd_load_sample_named(u8 *name)
{
    return softmix_load_sample_name(name);
}

s32 n64snd_load_sample_index(s32 resId)
{
    return softmix_load_sample_id(resId);
}

s32 n64snd_create_sample(u8 *data, u32 length, s32 loopStart, s32 loopEnd, s32 rate)
{
    return softmix_create_sample(data, length, loopStart, loopEnd, rate);
}

void n64snd_free_sample(u32 sample)
{
    softmix_free_sample(sample);
}

void n64snd_get_sample_info(s32 sample, s32 *info)
{
    softmix_get_sample_info(sample, info);
}

void n64snd_set_sample_info(s32 sample, s32 *info)
{
    softmix_set_sample_info(sample, info);
}

void n64snd_write_sample_data(s32 sample, u8 *data, u32 size)
{
    softmix_write_sample(sample, data, size);
}

void n64snd_voice_set_sample(s32 voice, s32 sample)
{
    softmix_voice_set_sample(voice, sample);
}

void n64snd_voice_set_rate(s32 voice, s32 rate)
{
    softmix_voice_set_rate(voice, rate);
}

void n64snd_voice_set_volume(s32 voice, s32 volume)
{
    softmix_voice_set_volume(voice, volume);
}

void n64snd_voice_set_pan(s32 voice, s32 pan)
{
    softmix_voice_set_pan(voice, pan);
}

void n64snd_voice_set_position(s32 voice, u32 position)
{
    softmix_voice_set_position(voice, position);
}

u32 n64snd_voice_get_position(s32 voice)
{
    return softmix_voice_get_position(voice);
}

void n64snd_voice_set_flags(s32 voice, s32 flags)
{
    softmix_voice_set_flags(voice, flags);
}

s32 n64snd_voice_get_flags(s32 voice)
{
    return softmix_voice_get_flags(voice);
}

s32 n64snd_get_sample_clock(void)
{
    return softmix_get_sample_count();
}

/* The frames mixed so far: output samples over the frame size (half an AI buffer, what each softmix_mix mixes). */
u32 n64snd_get_buffer_clock(void)
{
    u32 count = softmix_get_sample_count();
    return count / (sN64SndFrames >> 1);
}

void n64snd_set_callback(void (*fn)(void))
{
    sN64SndCallback = fn;
}

void n64snd_echo_init(s32 size)
{
    softmix_echo_init(size);
}

void n64snd_echo_enable(s32 on)
{
    softmix_echo_enable(on);
}

void n64snd_echo_set_level(s32 level)
{
    softmix_echo_set_level(level);
}

void n64snd_echo_free(void)
{
    softmix_echo_free();
}

/* The driver slots unk_64-unk_6C. */
void n64snd_stub1(void)
{
}

void n64snd_stub2(void)
{
}

void n64snd_stub3(void)
{
}

/* Muting clears both AI buffers at once and keeps the next 3 mixed ones silent; the mix goes on underneath. */
void n64snd_set_mute(s32 on)
{
    if (on != 0) {
        sN64SndMuted = 1;
        mem_fill(sN64SndAiBuf, 0, sN64SndAiBufSize << 1);
        osWritebackDCache(sN64SndAiBuf, sN64SndAiBufSize << 1);
        gN64SndSilenceFrames = 3;
    } else {
        sN64SndMuted = 0;
    }
}

/* Clears both AI buffers and keeps the next 3 mixed ones silent. */
void n64snd_clear_output(void)
{
    mem_fill(sN64SndAiBuf, 0, sN64SndAiBufSize << 1);
    osWritebackDCache((u32)sN64SndAiBuf, sN64SndAiBufSize << 1);
    gN64SndSilenceFrames = 3;
}
