#include "game.h"

/* Starts the mixer for `numVoices` voices at `rate` Hz with the output mode `flags` (SFX_MODE_*): allocates the voices
   and the volume table, hands them to the asm mixers, opens the "SFX" resource set and registers the "SAM" type.
   Returns 0, 1 (bad arguments, no memory for the voices) or 2 (none for the table). `frameMs` is unused. */
s32 softmix_init(s32 numVoices, u32 flags, s32 rate, s32 frameMs)
{
    s32 *table;
    s32 unusedInt, acc, level, value;
    s16 *unusedPtr;

    /* at most SOFTMIX_MAX_VOICES voices and SFX_MAX_RATE Hz */
    if (numVoices < 9 && rate < 0x5623) goto body;
ret1:
    return 1;
body:
    {
        sSoftmixFlags = flags;
        sSoftmixOutputRate = rate;
        sSoftmixNumVoices = numVoices;
        sSoftmixSampleCount = 0;
        sSoftmixEchoOn = 0;
        sSoftmixVoices = mem_alloc_dbg(numVoices * sizeof(SoftmixVoice), 0, &sStrSoftmixFile, 0x2c8);
        if (sSoftmixVoices == 0) goto ret1;
        {
            mem_fill((u8 *)sSoftmixVoices, 0, numVoices * sizeof(SoftmixVoice));
            mem_fill((u8 *)sSoftmixSamples, 0, sizeof(sSoftmixSamples));
            sSoftmixVolumeTable = table = mem_alloc_dbg(SOFTMIX_VOLUME_TABLE_SIZE, 0, &sStrSoftmixFile, 0x2d4);
            if (table == 0) {
                mem_free(sSoftmixVoices);
                return 2;
            }
            /* Row `level`, entry `value`: the sample byte's value (16-bit: expanded from its companded form, 8-bit:
               as a signed byte) times level / SOFTMIX_VOLUME_ROW_MAX. */
            value = 0;
            if (flags & SFX_MODE_16BIT) {
                s32 *entries = table;
                do {
                    level = 0;
                    do {
                        s32 prod = sSoftmixExpandTable[value] * level;
                        entries[value + level * SOFTMIX_VOLUME_ROW_SIZE] = prod / SOFTMIX_VOLUME_ROW_MAX;
                        level = level + 1;
                    } while (level < SOFTMIX_VOLUME_ROWS);
                    value = value + 1;
                } while (value < SOFTMIX_VOLUME_ROW_SIZE);
            } else {
                s32 *entries = table;
                s32 signedValue;
                do {
                    level = 0;
                    signedValue = (s8)value;
                    acc = 0;
                    do {
                        entries[value + level * SOFTMIX_VOLUME_ROW_SIZE] = acc / SOFTMIX_VOLUME_ROW_MAX;
                        acc = acc + signedValue;
                        level = level + 1;
                    } while (level < SOFTMIX_VOLUME_ROWS);
                    value = value + 1;
                } while (value < SOFTMIX_VOLUME_ROW_SIZE);
            }
            sMixVoices = sSoftmixVoices;
            sMixNumVoices = sSoftmixNumVoices;
            sMixVolumeTable = sSoftmixVolumeTable;
            if (res_open_set(&sStrSfxResSet, &gSfxResSet) != 0) {
                debug_printf((s32)&sStrNoSfxResSet);
                gSfxResSet = 0;
            } else {
                debug_printf((s32)&sStrOpenedResSet);
            }
            res_register_type(&sStrSamType, softmix_sam_decompress);
            return 0;
        }
}
}

/* Loads a sample from the file "<name>.16" or "<name>.08" (a 0x18-byte header like SamHeader, then the data) into a
   free slot. Returns the slot, -1 (none free), -2 (no file) or -4 (no memory). */
s32 softmix_load_sample_file(s32 name)
{
    s32 i;
    SoftmixSample *entry;
    FileHandle *file;
    u8 buf[8];
    u8 *bp;

    for (i = 0; i < SOFTMIX_NUM_SAMPLES; i++) {
        if (sSoftmixSamples[i].length == 0) break;
    }
    if (i == SOFTMIX_NUM_SAMPLES) return -1;
    mem_fill((u8 *)&sSoftmixSamples[i], 0, sizeof(SoftmixSample));
    if (sSoftmixFlags & SFX_MODE_16BIT) {
        sprintf(sSoftmixNameBuf, sStrSampleFile16, name);
    } else {
        sprintf(sSoftmixNameBuf, sStrSampleFile08, name);
    }
    file = file_open(sSoftmixNameBuf, sStrModeRb);
    do { do { do { do { do { do { bp = buf; } while (0); } while (0); } while (0); } while (0); } while (0); } while (0);
    if (file == 0) return -2;
    /* flags and unk_14 are read into `buf` and dropped */
    file_read(bp, 1, 4, file);
    file_read((u8 *)&sSoftmixSamples[i].length, 1, 4, file);
    file_read((u8 *)&sSoftmixSamples[i].loopStart, 1, 4, file);
    file_read((u8 *)&sSoftmixSamples[i].loopEnd, 1, 4, file);
    file_read((u8 *)&sSoftmixSamples[i].rate, 1, 4, file);
    file_read(bp, 1, 4, file);
    entry = &sSoftmixSamples[i];
    entry->data = mem_alloc_dbg(entry->length, 0, sStrSoftmixFile, 0x368);
    if (entry->data == 0) {
        file_close(file);
        return -4;
    }
    file_read(entry->data, 1, entry->length, file);
    file_close(file);
    return i;
}

/* Loads the "SAM" resource "SAM-<name>.16" or "SAM-<name>.08" of the "SFX" set into a free slot; returns the slot or
   -1. */
s32 softmix_load_sample_name(u8 *name)
{
    s32 i;
    SamHeader *sam;
    char *samBytes;
    s32 *res;
    SoftmixSample *entry;
    s32 *samOrRate;

    if (gSfxResSet == 0) {
        return -1;
    }
    for (i = 0; i < SOFTMIX_NUM_SAMPLES; i++) {
        if (sSoftmixSamples[i].length == 0) {
            break;
        }
    }
    if (i == SOFTMIX_NUM_SAMPLES) {
        return -1;
    }
    mem_fill((u8 *)&sSoftmixSamples[i], 0, sizeof(SoftmixSample));
    if (sSoftmixFlags & SFX_MODE_16BIT) {
        sprintf(sSoftmixNameBuf, sStrSamName16, name);
    } else {
        sprintf(sSoftmixNameBuf, sStrSamName08, name);
    }
    res = (s32 *)res_load_by_name_2(gSfxResSet, sSoftmixNameBuf, &sSoftmixSamples[i].resId);
    entry = &sSoftmixSamples[i];
    /* `sam` stored and read back as volatile, each read a load of its own */
    *(SamHeader *volatile *)&entry->sam = (SamHeader *)res;
    entry->length = ((volatile SamHeader *)res)->length;
    sam = *(SamHeader *volatile *)&entry->sam;
    samOrRate = (s32 *)*(SamHeader *volatile *)&entry->sam;
    entry->loopStart = sam->loopStart;
    entry->loopEnd = sam->loopEnd;
    samBytes = (char *)samOrRate;
    samOrRate = (s32 *)((SamHeader *)samOrRate)->rate;
    entry->data = (u8 *)samBytes + sizeof(SamHeader);
    entry->rate = (s32)samOrRate;
    return i;
}

/* Loads the "SAM" resource `resId` of the "SFX" set into a free slot; returns the slot or -1. */
s32 softmix_load_sample_id(s32 resId)
{
    s32 i;
    SoftmixSample *entry;

    if (gSfxResSet == NULL) {
        return -1;
    }
    for (i = 0; i < SOFTMIX_NUM_SAMPLES; i++) {
        if (sSoftmixSamples[i].length == 0) {
            break;
        }
    }
    if (i == SOFTMIX_NUM_SAMPLES) {
        return -1;
    }
    entry = &sSoftmixSamples[i];
    mem_fill((u8 *)entry, 0, sizeof(SoftmixSample));
    entry->resId = resId;
    entry->sam = res_load(gSfxResSet, resId);
    entry->length = entry->sam->length;
    entry->loopStart = entry->sam->loopStart;
    entry->loopEnd = entry->sam->loopEnd;
    entry->rate = entry->sam->rate;
    entry->data = (u8 *)(entry->sam + 1);
    return i;
}

/* Copies `length` bytes of sample data into a new allocation in a free slot. Returns the slot, -1 (none free), -3
   (no data) or -4 (no memory). */
s32 softmix_create_sample(u8 *data, s32 length, s32 loopStart, s32 loopEnd, s32 rate)
{
    s32 slot;
    s32 more;
    SoftmixSample *entry;
    SoftmixSample *samples;
    SoftmixSample *walk;
    void *buf;

    samples = sSoftmixSamples;
    slot = 0;
    walk = samples;
top:
    if (walk->length == 0) {
        goto done;
    }
    slot++;
    more = slot < SOFTMIX_NUM_SAMPLES;
    walk++;
    if (more) {
        goto top;
    }
done:
    if (slot == SOFTMIX_NUM_SAMPLES) {
        return -1;
    }
    entry = (SoftmixSample *)((s32)(slot * sizeof(SoftmixSample)) + (s32)samples);
    mem_fill((u8 *)entry, 0, sizeof(SoftmixSample));
    entry->length = length;
    if (length == 0) {
        return -3;
    }
    buf = mem_alloc_dbg(length, 0, sStrSoftmixFile, 0x421);
    entry->data = buf;
    if (buf == 0) {
        return -4;
    }
    sfx_copy_bytes(buf, data, entry->length);
    entry->loopStart = loopStart;
    entry->loopEnd = loopEnd;
    entry->rate = rate;
    entry->resId = -1;
    return slot;
}

/* Gives a voice a sample, keeping its position within the new sample's length. */
void softmix_voice_set_sample(s32 voice, s32 sample)
{
    SoftmixVoice *voiceEntry;
    SoftmixSample *sampleEntry;
    SoftmixVoice *voices;
    SoftmixSample *samples;
    u8 *data;
    u32 flags;
    critical_enter();
    voices = sSoftmixVoices;
    voiceEntry = (SoftmixVoice *)((s32)(voice * sizeof(SoftmixVoice)) + (s32)voices);
    samples = sSoftmixSamples;
    sampleEntry = (SoftmixSample *)((s32)(sample * sizeof(SoftmixSample)) + (s32)samples);
    voiceEntry->position = (u32)voiceEntry->position % (u32)sampleEntry->length;
    voiceEntry->loopStart = sampleEntry->loopStart;
    voiceEntry->loopEnd = sampleEntry->loopEnd;
    data = sampleEntry->data;
    flags = voiceEntry->flags;
    voiceEntry->sample = sample;
    voiceEntry->data = data;
    if (flags & SFX_VOICE_LOOPED) {
        voiceEntry->end = sampleEntry->loopEnd;
    } else {
        voiceEntry->end = sampleEntry->length;
    }
    critical_leave();
}

/* softmix_voice_set_volume and softmix_voice_set_pan each write out softmix_voice_update_levels: in stereo a
   voice's left and right levels are (SOFTMIX_PAN_MAX - pan) and pan times volume / 10, at most the volume. */
void softmix_voice_set_volume(s32 voice, s32 value)
{
    s32 volume, pan, left, right;

    sSoftmixVoices[voice].volume = value;
    if (sSoftmixFlags & SFX_MODE_STEREO) {
        volume = sSoftmixVoices[voice].volume;
        pan = sSoftmixVoices[voice].pan;

        left = SOFTMIX_PAN_MAX - pan;
        left = left * volume / 10;
        right = pan * volume / 10;
        if (left > volume) left = volume;
        if (right > volume) right = volume;
        sSoftmixVoices[voice].left = left;
        sSoftmixVoices[voice].right = right;
    }
}

void softmix_voice_set_pan(s32 voice, s32 value)
{
    s32 volume, pan, left, right;

    sSoftmixVoices[voice].pan = value;
    if (sSoftmixFlags & SFX_MODE_STEREO) {
        volume = sSoftmixVoices[voice].volume;
        pan = sSoftmixVoices[voice].pan;

        left = SOFTMIX_PAN_MAX - pan;
        left = left * volume / 10;
        right = pan * volume / 10;
        if (left > volume) left = volume;
        if (right > volume) right = volume;
        sSoftmixVoices[voice].left = left;
        sSoftmixVoices[voice].right = right;
    }
}

/* Mixes `count` output samples into `buf` (s32, interleaved in stereo): scales the voices' levels by the master
   volume (0x100 = 1) for the asm mixer, then adds the echo ring. */
void softmix_mix(s32 *buf, s32 count)
{
    s32 i;
    s32 total;
    s32 pos;
    u8 *echoBuf;

    sMixCount = count;
    sMixSrc = buf;
    sMixOverwrite = 1;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixSavedLevels[i * 2] = sSoftmixVoices[i].left;
        sSoftmixSavedLevels[i * 2 + 1] = sSoftmixVoices[i].right;
        sSoftmixVoices[i].left = (sSoftmixVoices[i].left * sSoftmixMasterVolume) >> 8;
        /* The right level is made from the already scaled left one (an original bug). */
        sSoftmixVoices[i].right = (sSoftmixVoices[i].left * sSoftmixMasterVolume) >> 8;
    }
    if (sSoftmixFlags & SFX_MODE_STEREO) {
        softmix_mix_voices_stereo();
    } else {
        softmix_mix_voices_mono();
    }
    /* Echo: each output value gets the ring's byte at the current position through sSoftmixEchoTable (the level),
       and the byte is replaced by the output scaled down. */
    if (sSoftmixEchoOn != 0) {
        total = count;
        if (sSoftmixFlags & SFX_MODE_STEREO) {
            total = count << 1;
        }
        if (sSoftmixFlags & SFX_MODE_16BIT) {
            i = 0;
            if (0 < total) {
                s8 *table;
                table = sSoftmixEchoTable;
                do {
                    echoBuf = sSoftmixEchoBuf;
                    pos = sSoftmixEchoPos;
                    buf[i] += *(s8 *)((s32)echoBuf[pos] + (s32)table) << 8;
                    echoBuf[pos] = buf[i] >> 10;
                    pos++;
                    sSoftmixEchoPos = pos;
                    if (pos == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < total);
            }
        } else {
            i = 0;
            if (0 < total) {
                s8 *table;
                table = sSoftmixEchoTable;
                do {
                    echoBuf = sSoftmixEchoBuf;
                    pos = sSoftmixEchoPos;
                    buf[i] += *(s8 *)((s32)echoBuf[pos] + (s32)table);
                    echoBuf[pos] = buf[i] >> 2;
                    pos++;
                    sSoftmixEchoPos = pos;
                    if (pos == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < total);
            }
        }
    }
    sSoftmixSampleCount += count;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixVoices[i].left = sSoftmixSavedLevels[i * 2];
        sSoftmixVoices[i].right = sSoftmixSavedLevels[i * 2 + 1];
    }
}

/* The "SAM" resource type's load hook: a sample in a format other than 8-bit is copied into the decompression
   buffer (it hangs when the buffer is too small; a missing decompressor halts), freed, and replaced by what the
   decompressor (decoder_decode_sample) makes of it. */
void softmix_sam_decompress(SamHeader **resp)
{
    SamHeader *sam;
    SamHeader *decoded;
    u8 **bufp;

    sam = *resp;
    if ((sam->flags & SAM_FORMAT_MASK) >= 2) {
        if (sSoftmixDecompressFn != 0) {
            if (sSoftmixDecompressBufSize < (u32)sam->length + 0x1c) goto spin;
            memcpy(sSoftmixDecompressBuf, sam, (u32)sam->length + 0x1c);
            mem_free(*resp);
            sam = (SamHeader *)sSoftmixDecompressBuf;
            goto join;
        spin:
            while (1) {}
        join:
            decoded = sSoftmixDecompressFn(sam);
            bufp = &sSoftmixDecompressBuf;
            if (decoded == 0) {
                fatal_halt(1);
            }
            if (sam != (SamHeader *)*bufp) {
                mem_free(sam);
            }
            /* A sample returned as it was still lies in the buffer: move it to memory of its own. */
            if (decoded == (SamHeader *)*bufp) {
                sam = mem_alloc_dbg((u32)sam->length + 0x1c, 0, sStrSoftmixFile, 0x67b);
                memcpy(sam, decoded, (u32)sam->length + 0x1c);
                decoded = sam;
            }
            *resp = decoded;
        } else {
            fatal_halt(1);
        }
    }
}

s32 softmix_get_caps(s32 *voices, s32 *modes, s32 *rate)
{
    *voices = SOFTMIX_MAX_VOICES;
    *modes = SFX_MODE_STEREO_16BIT;
    *rate = SFX_MAX_RATE;
    return 0;
}

void softmix_shutdown(void)
{
    mem_free(sSoftmixVolumeTable);
    mem_free(sSoftmixVoices);
    if (gSfxResSet != 0) {
        res_close_set(gSfxResSet);
    }
    gSfxResSet = 0;
}

/* Frees a slot: unloads its resource or frees its data. The check lets slot 0x100, one past the end, through. */
void softmix_free_sample(u32 sample)
{
    SoftmixSample *samples;
    SoftmixSample *entry;
    if (sample < 0x101) {
        samples = sSoftmixSamples;
        entry = &samples[sample];
        if (entry->length != 0) {
            if (entry->resId != -1) {
                res_unload((u32 *)(gSfxResSet_2[1] + entry->resId * 4));
            } else {
                mem_free(entry->data);
            }
            mem_fill((u8 *)&sSoftmixSamples[sample], 0, sizeof(SoftmixSample));
        }
    }
}

/* A slot's info as the driver passes it: length, loop start, loop end, rate. */
void softmix_get_sample_info(s32 sample, s32 *info) {
    SoftmixSample *samples = sSoftmixSamples;
    SoftmixSample *entry = samples + sample;
    info[0] = entry->length;
    info[1] = entry->loopStart;
    info[2] = entry->loopEnd;
    info[3] = entry->rate;
}

void softmix_set_sample_info(s32 sample, s32 *info)
{
    SoftmixSample *samples = sSoftmixSamples;
    SoftmixSample *entry = &samples[sample];
    entry->length = info[0];
    entry->loopStart = info[1];
    entry->loopEnd = info[2];
    entry->rate = info[3];
}

/* A voice's step per output sample, rate / output rate in 32.32 fixed point (16 bits of the fraction computed). */
void softmix_voice_set_rate(s32 voice, s32 rate)
{
    s32 scratch;
    s32 frac;

    critical_enter();
    scratch = rate % sSoftmixOutputRate * 65536;
    frac = scratch / sSoftmixOutputRate;
    scratch = voice * sizeof(SoftmixVoice) + (s32)sSoftmixVoices;
    ((SoftmixVoice *)scratch)->step = rate / sSoftmixOutputRate;
    ((SoftmixVoice *)scratch)->stepFrac = frac * 65536;
    critical_leave();
}

void softmix_voice_set_position(s32 voice, s32 position)
{
    sSoftmixVoices[voice].position = position;
}

s32 softmix_voice_get_position(s32 voice)
{
    return sSoftmixVoices[voice].position;
}

/* Sets a voice's flags (SFX_VOICE_*); it ends at the loop end when looped, else at the sample's end. */
void softmix_voice_set_flags(s32 voice, u32 flags)
{
    SoftmixVoice *voices = sSoftmixVoices;
    SoftmixVoice *entry = (SoftmixVoice *)((u32)(voice * sizeof(SoftmixVoice)) + (u32)voices);
    entry->flags = flags;
    if (flags & SFX_VOICE_LOOPED) {
        entry->end = sSoftmixSamples[entry->sample].loopEnd;
        return;
    }
    entry->end = sSoftmixSamples[entry->sample].length;
}

s32 softmix_voice_get_flags(s32 voice)
{
    return sSoftmixVoices[voice].flags;
}

/* The output samples mixed so far. */
s32 softmix_get_sample_count(void) {
    return sSoftmixSampleCount;
}

/* Converts `count` mixed samples from `src` (s32) into the output buffer `dst`, clamped to 16 bits or 8 bits. */
void softmix_convert_output(s32 src, s32 dst, s32 count) {
    s32 flags;

    sMixSrc = src;
    sMixDst = dst;
    flags = sSoftmixFlags;
    sMixCount = count;
    if (flags & SFX_MODE_STEREO) {
        sMixCount = count << 1;
    }
    if (flags & SFX_MODE_16BIT) {
        sMixBias = 0;
        sMixShift = 0;
        sMixClampMin = (s32)0xFFFF8001;
        sMixClampMax = 0x7FFF;
        softmix_output_16bit();
    } else {
        sMixShift = 0;
        sMixBias = 0;
        sMixClampMin = 0;
        sMixClampMax = 0xFF;
        softmix_output_8bit();
    }
}

/* Allocates an echo ring of `size` bytes per channel and turns the echo on at full level. Returns 0, -1 (already on)
   or 1 (no memory). */
s32 softmix_echo_init(s32 size)
{
    s32 i;
    u8 *buf;

    if (sSoftmixEchoOn != 0) {
        return -1;
    }
    if (sSoftmixFlags & SFX_MODE_STEREO) {
        size <<= 1;
    }
    sSoftmixEchoBuf = buf = mem_alloc_dbg(size, 0, sStrSoftmixFile, 0x601);
    if (buf == NULL) {
        return 1;
    }
    mem_fill(buf, 0, size);
    sSoftmixEchoSize = size;
    i = 0xFF;
    {
        u8 *p = sSoftmixEchoTable + i;
        do {
            *p = i;
            i--;
            p--;
        } while (i >= 0);
    }
    sSoftmixEchoPos = 0;
    sSoftmixEchoOn = 1;
    return 0;
}

/* Turns the echo on or off; turning it off clears the ring. */
void softmix_echo_enable(s32 on)
{
    if (on != 0) {
        sSoftmixEchoOn = 1;
        return;
    }
    if (sSoftmixEchoOn != 0) {
        mem_fill(sSoftmixEchoBuf, 0, sSoftmixEchoSize);
    }
    sSoftmixEchoOn = 0;
}

/* Fills the echo table with each signed byte times level / 256, rounded toward zero. */
void softmix_echo_set_level(s32 level)
{
    s32 i = 0;
    s32 scaled;
    u8 *dst;
    do {
        scaled = (s8)i * level;
        dst = sSoftmixEchoTable + i;
        if (scaled < 0) {
            scaled += 0xFF;
        }
        *dst = (u8)(scaled >> 8);
        i++;
    } while (i < 0x100);
}

void softmix_echo_free(void)
{
    if (sSoftmixEchoOn != 0) {
        mem_free(sSoftmixEchoBuf);
        sSoftmixEchoOn = 0;
    }
}

/* Writes up to the slot's length of companded bytes into a sample: copied as they are for 16-bit output, expanded to
   linear 8-bit otherwise. The check lets slot 0x100, one past the end, through. */
void softmix_write_sample(u32 sample, u8 *src, s32 size) {
    s32 i;

    if (sample <= SOFTMIX_NUM_SAMPLES) {
        if ((u32)sSoftmixSamples[sample].length < size) {
            size = (u32)sSoftmixSamples[sample].length;
        }
        if (sSoftmixFlags & SFX_MODE_16BIT) {
            sfx_copy_bytes(sSoftmixSamples[sample].data, src, size);
        } else {
            for (i = 0; i < size; i++) {
                sSoftmixSamples[sample].data[i] = sSoftmixExpandTable[src[i]] >> 8;
            }
        }
    }
}

/* The decoder softmix_sam_decompress uses (decoder_decode_sample), and the buffer it decodes from. */
void softmix_set_decompressor(SamHeader *(*fn)(SamHeader *))
{
    sSoftmixDecompressFn = fn;
}

void softmix_set_decompress_buf(s32 buf, s32 size)
{
    sSoftmixDecompressBuf = buf;
    sSoftmixDecompressBufSize = size;
}

/* Recomputes a voice's stereo levels from its volume and pan (see softmix_voice_set_volume). */
void softmix_voice_update_levels(s32 voice)
{
    s32 volume, pan, left, right;
    if (sSoftmixFlags & SFX_MODE_STEREO) {
        volume = sSoftmixVoices[voice].volume;
        pan = sSoftmixVoices[voice].pan;

        left = SOFTMIX_PAN_MAX - pan;
        left = left * volume / 10;
        right = pan * volume / 10;
        if (left > volume) left = volume;
        if (right > volume) right = volume;
        sSoftmixVoices[voice].left = left;
        sSoftmixVoices[voice].right = right;
    }
}

s32 softmix_get_sample_data(s32 sample)
{
    return (s32)sSoftmixSamples[sample].data;
}

/* The master volume softmix_mix scales every voice by: 0x100 = 1. */
void softmix_set_master_volume(s32 volume)
{
    sSoftmixMasterVolume = volume;
}
