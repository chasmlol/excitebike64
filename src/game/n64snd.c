#include "game.h"

s32 n64snd_init(s32 param_1, s32 param_2, s32 param_3, s32 param_4, u32 *param_5, s32 *param_6)
{
    s32 iVar1;
    s32 r;

    gN64SndSilenceFrames = 0;
    sN64SndMuted = 0;
    if ((param_2 & 3) != 3) {
        return 1;
    }
    {
        sN64SndMode = param_2;
        sN64SndFrequency = (s32)osAiSetFrequency(param_3);
        iVar1 = 1;
        *param_6 = sN64SndFrequency;
        if (param_2 & 1) {
            iVar1 = 2;
        }
        if (param_2 & 2) {
            iVar1 = iVar1 << 1;
        }
        sN64SndFrames = ((((u32)(sN64SndFrequency * param_4) / 1000 + 0x3f) >> 6) << 6);
        *param_5 = sN64SndFrames;
        sN64SndFrames = sN64SndFrames * 2;
        sN64SndAiBufSize = iVar1 * sN64SndFrames;
        sN64SndMixBuf = (s32)mem_alloc_dbg(sN64SndAiBufSize * 4, 0, sStrN64sndC, 0xd7);
        sN64SndAiBuf = (s32 *)mem_alloc_dbg(sN64SndAiBufSize * 2 + 0x10, 0, sStrN64sndC, 0xd9);
        sN64SndAiBufAlloc = sN64SndAiBuf;
        if (((u32)sN64SndAiBufAlloc & 0xff) == 0) {
            sN64SndAiBuf = sN64SndAiBufAlloc + 4;
        }
        if (sN64SndAiBuf == 0) {
            return 1;
        }
        {
            r = softmix_init(param_1, param_2, param_3, param_4);
            if (r != 0) {
                mem_free((s32)sN64SndAiBufAlloc);
                mem_free(sN64SndMixBuf);
                return r;
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

void n64snd_update(void)
{
    s32 uVar1;
    s32 *piVar3;
    u32 h;
    s32 iVar4;

    uVar1 = osAiGetStatus();
    if (uVar1 >= 0) {
        iVar4 = 0;
        if ((uVar1 & 0x40000000) == 0) {
            sN64SndUnderruns = sN64SndUnderruns + 1;
        }
        sN64SndBufIndex = sN64SndBufIndex ^ 1;
        osAiSetNextBuffer(sN64SndAiBuf + sN64SndAiBufSize * sN64SndBufIndex, sN64SndAiBufSize);
        piVar3 = sN64SndMixBuf;
        do {
            if (sN64SndCallback != 0) {
                (*sN64SndCallback)();
            }
            softmix_mix(piVar3, sN64SndFrames >> 1);
            if (gN64SndMixCallback != 0) {
                (*gN64SndMixCallback)(piVar3, sN64SndFrames >> 1);
            }
            h = sN64SndFrames >> 1;
            if (sN64SndMode & 1) {
                piVar3 += h * 2;
            } else {
                piVar3 += h;
            }
            iVar4 = iVar4 + 1;
        } while (iVar4 < 2);
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

s32 n64snd_get_caps(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    softmix_get_caps(arg0, arg1, arg2);
    *arg1 = 3;
    *arg3 = 20;
    if (*arg2 > 22050) {
        *arg2 = 22050;
    }
    return 0;
}

void n64snd_shutdown(void)
{
    sN64SndActive = 0;
    softmix_shutdown();
    mem_free(sN64SndMixBuf);
    mem_free(sN64SndAiBufAlloc);
}

s32 n64snd_load_sample_named(u8 *name) {
    return softmix_load_sample_name(name);
}

s32 n64snd_load_sample_index(s32 resId)
{
    return softmix_load_sample_id(resId);
}

s32 n64snd_create_sample(u8 *data, u32 length, s32 loopStart, s32 loopEnd, s32 unk) {
    return softmix_create_sample(data, length, loopStart, loopEnd, unk);
}

void n64snd_free_sample(u32 sample) {
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

u32 n64snd_get_buffer_clock(void) {
    u32 r = softmix_get_sample_count();
    return r / (sN64SndFrames >> 1);
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

void n64snd_stub1(void)
{
}

void n64snd_stub2(void)
{
}

void n64snd_stub3(void)
{
}

void n64snd_set_mute(s32 a0)
{
    if (a0 != 0) {
        sN64SndMuted = 1;
        mem_fill(sN64SndAiBuf, 0, sN64SndAiBufSize << 1);
        osWritebackDCache(sN64SndAiBuf, sN64SndAiBufSize << 1);
        gN64SndSilenceFrames = 3;
    } else {
        sN64SndMuted = 0;
    }
}

void n64snd_clear_output(void)
{
    mem_fill(sN64SndAiBuf, 0, sN64SndAiBufSize << 1);
    osWritebackDCache((u32)sN64SndAiBuf, sN64SndAiBufSize << 1);
    gN64SndSilenceFrames = 3;
}
