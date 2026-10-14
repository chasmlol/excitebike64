#include "game.h"

s32 softmix_init(s32 param_1, u32 param_2, s32 param_3, s32 arg3)
{
    s32 *piVar1;
    s32 iVar2, iVar3, iVar4, iVar6;
    s16 *psVar5;

    if (param_1 < 9 && param_3 < 0x5623) goto body;
ret1:
    return 1;
body:
    {
        sSoftmixFlags = param_2;
        sSoftmixOutputRate = param_3;
        sSoftmixNumVoices = param_1;
        sSoftmixSampleCount = 0;
        sSoftmixEchoOn = 0;
        sSoftmixVoices = mem_alloc_dbg(param_1 * sizeof(SoftmixVoice), 0, &sStrSoftmixFile, 0x2c8);
        if (sSoftmixVoices == 0) goto ret1;
        {
            mem_fill((u8 *)sSoftmixVoices, 0, param_1 * sizeof(SoftmixVoice));
            mem_fill((u8 *)sSoftmixSamples, 0, sizeof(sSoftmixSamples));
            sSoftmixVolumeTable = piVar1 = mem_alloc_dbg(0x8000, 0, &sStrSoftmixFile, 0x2d4);
            if (piVar1 == 0) {
                mem_free(sSoftmixVoices);
                return 2;
            }
            iVar6 = 0;
            if (param_2 & 2) {
                s32 *tp = piVar1;
                do {
                    iVar4 = 0;
                    do {
                        s32 prod = sSoftmixExpandTable[iVar6] * iVar4;
                        tp[iVar6 + iVar4 * 0x100] = prod / 0x1f;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            } else {
                s32 *tp = piVar1;
                s32 sh;
                do {
                    iVar4 = 0;
                    sh = (s8)iVar6;
                    iVar3 = 0;
                    do {
                        tp[iVar6 + iVar4 * 0x100] = iVar3 / 0x1f;
                        iVar3 = iVar3 + sh;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
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

s32 softmix_load_sample_file(s32 param_1)
{
    s32 i;
    SoftmixSample *e;
    s32 *f;
    u8 buf[8];
    u8 *bp;

    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples[i].length == 0) break;
    }
    if (i == 0x100) return -1;
    mem_fill((u8 *)&sSoftmixSamples[i], 0, sizeof(SoftmixSample));
    if (sSoftmixFlags & 2) {
        sprintf(sSoftmixNameBuf, sStrSampleFile16, param_1);
    } else {
        sprintf(sSoftmixNameBuf, sStrSampleFile08, param_1);
    }
    f = file_open(sSoftmixNameBuf, sStrModeRb);
    do { do { do { do { do { do { bp = buf; } while (0); } while (0); } while (0); } while (0); } while (0); } while (0);
    if (f == 0) return -2;
    file_read(bp, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples[i].length, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples[i].loopStart, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples[i].loopEnd, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples[i].unk_10, 1, 4, f);
    file_read(bp, 1, 4, f);
    e = &sSoftmixSamples[i];
    e->data = mem_alloc_dbg(e->length, 0, sStrSoftmixFile, 0x368);
    if (e->data == 0) {
        file_close(f);
        return -4;
    }
    file_read(e->data, 1, e->length, f);
    file_close(f);
    return i;
}

s32 softmix_load_sample_name(s32 param_1, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    SamHeader *e;
    char *pcVar6;
    s32 *q;
    SoftmixSample *ent;
    s32 *pp;

    if (gSfxResSet == 0) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples[i].length == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    mem_fill((u8 *)&sSoftmixSamples[i], 0, sizeof(SoftmixSample));
    if (sSoftmixFlags & 2) {
        sprintf(sSoftmixNameBuf, sStrSamName16, param_1);
    } else {
        sprintf(sSoftmixNameBuf, sStrSamName08, param_1);
    }
    q = (s32 *)res_load_by_name_2(gSfxResSet, sSoftmixNameBuf, &sSoftmixSamples[i].resId);
    ent = &sSoftmixSamples[i];
    /* `sam` stored and read back as volatile, each read a load of its own */
    *(SamHeader *volatile *)&ent->sam = (SamHeader *)q;
    ent->length = ((volatile SamHeader *)q)->length;
    e = *(SamHeader *volatile *)&ent->sam;
    pp = (s32 *)*(SamHeader *volatile *)&ent->sam;
    ent->loopStart = e->loopStart;
    ent->loopEnd = e->loopEnd;
    pcVar6 = (char *)pp;
    pp = (s32 *)((SamHeader *)pp)->unk_10;
    ent->data = (u8 *)pcVar6 + sizeof(SamHeader);
    ent->unk_10 = (s32)pp;
    return i;
}

s32 softmix_load_sample_id(s32 param)
{
    s32 i;
    SoftmixSample *e;

    if (gSfxResSet == NULL) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples[i].length == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    e = &sSoftmixSamples[i];
    mem_fill((u8 *)e, 0, sizeof(SoftmixSample));
    e->resId = param;
    e->sam = res_load(gSfxResSet, param);
    e->length = e->sam->length;
    e->loopStart = e->sam->loopStart;
    e->loopEnd = e->sam->loopEnd;
    e->unk_10 = e->sam->unk_10;
    e->data = (u8 *)(e->sam + 1);
    return i;
}

s32 softmix_create_sample(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 n;
    s32 t;
    SoftmixSample *e;
    SoftmixSample *base;
    SoftmixSample *w;
    void *q;

    base = sSoftmixSamples;
    n = 0;
    w = base;
top:
    if (w->length == 0) {
        goto done;
    }
    n++;
    t = n < 0x100;
    w++;
    if (t) {
        goto top;
    }
done:
    if (n == 0x100) {
        return -1;
    }
    e = (SoftmixSample *)((s32)(n * sizeof(SoftmixSample)) + (s32)base);
    mem_fill((u8 *)e, 0, sizeof(SoftmixSample));
    e->length = p2;
    if (p2 == 0) {
        return -3;
    }
    q = mem_alloc_dbg(p2, 0, sStrSoftmixFile, 0x421);
    e->data = q;
    if (q == 0) {
        return -4;
    }
    sfx_copy_bytes(q, p1, e->length);
    e->loopStart = p3;
    e->loopEnd = p4;
    e->unk_10 = p5;
    e->resId = -1;
    return n;
}

void softmix_voice_set_sample(s32 param_1, s32 param_2)
{
    SoftmixVoice *a;
    SoftmixSample *b;
    SoftmixVoice *ab;
    SoftmixSample *bb;
    u8 *t;
    u32 fl;
    critical_enter();
    ab = sSoftmixVoices;
    a = (SoftmixVoice *)((s32)(param_1 * sizeof(SoftmixVoice)) + (s32)ab);
    bb = sSoftmixSamples;
    b = (SoftmixSample *)((s32)(param_2 * sizeof(SoftmixSample)) + (s32)bb);
    a->position = (u32)a->position % (u32)b->length;
    a->loopStart = b->loopStart;
    a->loopEnd = b->loopEnd;
    t = b->data;
    fl = a->flags;
    a->sample = param_2;
    a->data = t;
    if (fl & 2) {
        a->end = b->loopEnd;
    } else {
        a->end = b->length;
    }
    critical_leave();
}

void softmix_voice_set_volume(s32 i, s32 val)
{
    s32 a, b, x, y;

    sSoftmixVoices[i].volume = val;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices[i].volume;
        b = sSoftmixVoices[i].pan;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices[i].left = x;
        sSoftmixVoices[i].right = y;
    }
}

void softmix_voice_set_pan(s32 i, s32 val)
{
    s32 a, b, x, y;

    sSoftmixVoices[i].pan = val;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices[i].volume;
        b = sSoftmixVoices[i].pan;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices[i].left = x;
        sSoftmixVoices[i].right = y;
    }
}

void softmix_mix(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 n;
    s32 ix;
    u8 *pb;

    sMixCount = param_2;
    sMixSrc = param_1;
    sMixOverwrite = 1;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixSavedLevels[i * 2] = sSoftmixVoices[i].left;
        sSoftmixSavedLevels[i * 2 + 1] = sSoftmixVoices[i].right;
        sSoftmixVoices[i].left = (sSoftmixVoices[i].left * sSoftmixMasterVolume) >> 8;
        sSoftmixVoices[i].right = (sSoftmixVoices[i].left * sSoftmixMasterVolume) >> 8;
    }
    if (sSoftmixFlags & 1) {
        softmix_mix_voices_stereo();
    } else {
        softmix_mix_voices_mono();
    }
    if (sSoftmixEchoOn != 0) {
        n = param_2;
        if (sSoftmixFlags & 1) {
            n = param_2 << 1;
        }
        if (sSoftmixFlags & 2) {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = sSoftmixEchoTable;
                do {
                    pb = sSoftmixEchoBuf;
                    ix = sSoftmixEchoPos;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb) << 8;
                    pb[ix] = param_1[i] >> 10;
                    ix++;
                    sSoftmixEchoPos = ix;
                    if (ix == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < n);
            }
        } else {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = sSoftmixEchoTable;
                do {
                    pb = sSoftmixEchoBuf;
                    ix = sSoftmixEchoPos;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb);
                    pb[ix] = param_1[i] >> 2;
                    ix++;
                    sSoftmixEchoPos = ix;
                    if (ix == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < n);
            }
        }
    }
    sSoftmixSampleCount += param_2;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixVoices[i].left = sSoftmixSavedLevels[i * 2];
        sSoftmixVoices[i].right = sSoftmixSavedLevels[i * 2 + 1];
    }
}

void softmix_sam_decompress(SamHeader **param_1)
{
    SamHeader *p;
    SamHeader *q;
    u8 **bp;

    p = *param_1;
    if ((p->flags & 0xF) >= 2) {
        if (sSoftmixDecompressFn != 0) {
            if (sSoftmixDecompressBufSize < (u32)p->length + 0x1c) goto spin;
            memcpy(sSoftmixDecompressBuf, p, (u32)p->length + 0x1c);
            mem_free(*param_1);
            p = (SamHeader *)sSoftmixDecompressBuf;
            goto join;
        spin:
            while (1) {}
        join:
            q = sSoftmixDecompressFn(p);
            bp = &sSoftmixDecompressBuf;
            if (q == 0) {
                fatal_halt(1);
            }
            if (p != (SamHeader *)*bp) {
                mem_free(p);
            }
            if (q == (SamHeader *)*bp) {
                p = mem_alloc_dbg((u32)p->length + 0x1c, 0, sStrSoftmixFile, 0x67b);
                memcpy(p, q, (u32)p->length + 0x1c);
                q = p;
            }
            *param_1 = q;
        } else {
            fatal_halt(1);
        }
    }
}

s32 softmix_get_caps(s32 *a, s32 *b, s32 *c)
{
    *a = 8;
    *b = 3;
    *c = 0x5622;
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

void softmix_free_sample(u32 p)
{
    SoftmixSample *w;
    SoftmixSample *e;
    if (p < 0x101) {
        w = sSoftmixSamples;
        e = &w[p];
        if (e->length != 0) {
            if (e->resId != -1) {
                res_unload((u32 *)(gSfxResSet_2[1] + e->resId * 4));
            } else {
                mem_free(e->data);
            }
            mem_fill((u8 *)&sSoftmixSamples[p], 0, sizeof(SoftmixSample));
        }
    }
}

void softmix_get_sample_info(s32 idx, s32 *out) {
    SoftmixSample *base = sSoftmixSamples;
    SoftmixSample *r = base + idx;
    out[0] = r->length;
    out[1] = r->loopStart;
    out[2] = r->loopEnd;
    out[3] = r->unk_10;
}

void softmix_set_sample_info(s32 i, u32 *src)
{
    SoftmixSample *base = sSoftmixSamples;
    SoftmixSample *e = &base[i];
    e->length = src[0];
    e->loopStart = src[1];
    e->loopEnd = src[2];
    e->unk_10 = src[3];
}

void softmix_voice_set_rate(s32 param_1, s32 param_2)
{
    s32 iVar1;
    s32 iVar2;

    critical_enter();
    iVar1 = param_2 % sSoftmixOutputRate * 65536;
    iVar2 = iVar1 / sSoftmixOutputRate;
    iVar1 = param_1 * sizeof(SoftmixVoice) + (s32)sSoftmixVoices;
    ((SoftmixVoice *)iVar1)->step = param_2 / sSoftmixOutputRate;
    ((SoftmixVoice *)iVar1)->stepFrac = iVar2 * 65536;
    critical_leave();
}

void softmix_voice_set_position(s32 i, s32 v)
{
    sSoftmixVoices[i].position = v;
}

s32 softmix_voice_get_position(s32 a0)
{
    return sSoftmixVoices[a0].position;
}

void softmix_voice_set_flags(s32 n, u32 flags)
{
    SoftmixVoice *base = sSoftmixVoices;
    SoftmixVoice *e = (SoftmixVoice *)((u32)(n * sizeof(SoftmixVoice)) + (u32)base);
    e->flags = flags;
    if (flags & 2) {
        e->end = sSoftmixSamples[e->sample].loopEnd;
        return;
    }
    e->end = sSoftmixSamples[e->sample].length;
}

s32 softmix_voice_get_flags(s32 i)
{
    return sSoftmixVoices[i].flags;
}

s32 softmix_get_sample_count(s32 arg0) {
    return sSoftmixSampleCount;
}

void softmix_convert_output(s32 a, s32 b, s32 c) {
    s32 flags;

    sMixSrc = a;
    sMixDst = b;
    flags = sSoftmixFlags;
    sMixCount = c;
    if (flags & 1) {
        sMixCount = c << 1;
    }
    if (flags & 2) {
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

s32 softmix_echo_init(s32 size)
{
    s32 i;
    u8 *q;

    if (sSoftmixEchoOn != 0) {
        return -1;
    }
    if (sSoftmixFlags & 1) {
        size <<= 1;
    }
    sSoftmixEchoBuf = q = mem_alloc_dbg(size, 0, sStrSoftmixFile, 0x601);
    if (q == NULL) {
        return 1;
    }
    mem_fill(q, 0, size);
    sSoftmixEchoSize = size;
    i = 0xFF;
    {
        u8 *pp = sSoftmixEchoTable + i;
        do {
            *pp = i;
            i--;
            pp--;
        } while (i >= 0);
    }
    sSoftmixEchoPos = 0;
    sSoftmixEchoOn = 1;
    return 0;
}

void softmix_echo_enable(s32 p)
{
    if (p != 0) {
        sSoftmixEchoOn = 1;
        return;
    }
    if (sSoftmixEchoOn != 0) {
        mem_fill(sSoftmixEchoBuf, 0, sSoftmixEchoSize);
    }
    sSoftmixEchoOn = 0;
}

void softmix_echo_set_level(s32 k)
{
    s32 i = 0;
    s32 x;
    u8 *dst;
    do {
        x = (s8)i * k;
        dst = sSoftmixEchoTable + i;
        if (x < 0) {
            x += 0xFF;
        }
        *dst = (u8)(x >> 8);
        i++;
    } while (i < 0x100);
}

void softmix_echo_free(s32 arg0)
{
    if (sSoftmixEchoOn != 0) {
        mem_free(sSoftmixEchoBuf);
        sSoftmixEchoOn = 0;
    }
}

void softmix_write_sample(u32 idx, u8 *src, s32 n) {
    s32 i;

    if (idx <= 0x100) {
        if ((u32)sSoftmixSamples[idx].length < n) {
            n = (u32)sSoftmixSamples[idx].length;
        }
        if (sSoftmixFlags & 2) {
            sfx_copy_bytes(sSoftmixSamples[idx].data, src, n);
        } else {
            for (i = 0; i < n; i++) {
                sSoftmixSamples[idx].data[i] = sSoftmixExpandTable[src[i]] >> 8;
            }
        }
    }
}

void softmix_set_decompressor(SamHeader *(*fn)(SamHeader *))
{
    sSoftmixDecompressFn = fn;
}

void softmix_set_decompress_buf(s32 a, s32 b)
{
    sSoftmixDecompressBuf = a;
    sSoftmixDecompressBufSize = b;
}

void softmix_voice_update_levels(s32 i)
{
    s32 a, b, x, y;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices[i].volume;
        b = sSoftmixVoices[i].pan;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices[i].left = x;
        sSoftmixVoices[i].right = y;
    }
}

s32 softmix_get_sample_data(s32 a0)
{
    return (s32)sSoftmixSamples[a0].data;
}

void softmix_set_master_volume(s32 x)
{
    sSoftmixMasterVolume = x;
}
