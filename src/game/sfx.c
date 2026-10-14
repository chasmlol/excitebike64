#include "game.h"

static inline void copy(u8 *d, u8 *s, u32 len)
{
    for (; len != 0; len--) {
        *d++ = *s++;
    }
}

static inline s32 cmp(u8 *a, u8 *b)
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

s32 sfx_load_sample_name(u8 *name)
{
    s32 i, iv, loc[4];
    SfxSample *tab;
    SfxSample *e;
    u8 *pc;

    if (sSfxInitialized != 0) {
        for (i = 0; i < 0x100; i++) {
            if (sSfxSamples[i].name != NULL && cmp(sSfxSamples[i].name, name) == 0) {
                if (sSfxSamples[i].refCount >= 0) {
                    sSfxSamples[i].refCount++;
                }
                return i;
            }
        }
        iv = gSfxDriver->loadSampleNamed(name);
        if (iv < 0) {
            return iv;
        }
        if (iv < 0x100) {
            pc = mem_alloc_dbg(strlen(name) + 1, 0, sStrSfxFile, 0x201);
            tab = sSfxSamples;
            e = &tab[iv];
            e->name = pc;
            str_copy(pc, name);
            gSfxDriver->getSampleInfo(iv, loc);
            e->length = loc[0];
            e->loopStart = loc[1];
            e->loopEnd = loc[2];
            e->unk_10 = loc[3];
            return iv;
        }
        gSfxDriver->freeSample(iv);
    }
    return -1;
}

s32 sfx_load_sample_id(s32 resId)
{
    s32 idx;
    s32 buf[4];
    u8 *base;
    SndDriver *fp;
    SfxSample *p;

    if (sSfxInitialized != 0) {
        idx = gSfxDriver->loadSampleIndex(resId);
        if (idx < 0) {
            return idx;
        }
        if (idx < 0x100) {
            base = (u8 *)sSfxSamples;
            p = (SfxSample *)(base + idx * sizeof(SfxSample));
            fp = gSfxDriver;
            p->name = 0;
            p->refCount = 1;
            fp->getSampleInfo(idx, buf);
            p->length = buf[0];
            p->loopStart = buf[1];
            p->loopEnd = buf[2];
            p->unk_10 = buf[3];
            return idx;
        }
        gSfxDriver->freeSample(idx);
    }
    return -1;
}

void sfx_free_sample(s32 param_1)
{
    SfxSample *w;
    SfxSample *e;

    if (sSfxInitialized != 0) {
        w = sSfxSamples;
        e = &w[param_1];
        if (e->refCount > 0) {
            e->refCount = e->refCount - 1;
        }
        if (e->refCount == 0) {
            if (e->name != 0) {
                mem_free(e->name);
            }
            mem_fill((u8 *)e, 0, sizeof(SfxSample));
            gSfxDriver->freeSample(param_1);
        }
    }
}

/* sfx_play calls the driver's sample clock with the table as its argument. */
typedef s32 (*MethodFn)(void *);

/* sfx_update_voices reads and writes a sound's voice flags as volatile. */
#define VOICE_FLAGS(n) (((volatile SfxSound *)(n))->voiceFlags)

s32 sfx_play(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 arg5)
{
    SfxSound *p;
    SfxSound *w;
    SfxSound *q;
    SfxSound *prev;
    SndDriver *obj;
    s32 i;
    s32 cnt;
    s32 quo;
    s32 ret;
    s32 m1;
    s32 t5;
    s32 id;
    MethodFn fn;

    if (sSfxInitialized == 0) {
        return -1;
    }
    if (sSfxSamples[a0].length == 0) {
        return -1;
    }
    i = 0;
    critical_enter();
    m1 = -1;
    __asm__ __volatile__("" : "+r"(m1));
    w = sSfxSoundPool;
    for (i = 0; i < 0x80; i++) {
        if (w->prev == (SfxSound *)m1) {
            w->prev = NULL;
            break;
        }
        w++;
    }
    critical_leave();
    if (i == 0x80) {
        p = NULL;
    } else {
        p = &sSfxSoundPool[i];
    }
    if (p == NULL) {
        return -1;
    }
    mem_fill((u8 *)p, 0, sizeof(SfxSound));
    id = sSfxNextHandle;
    __asm__ __volatile__("" : "+r"(id));
    p->sample = a0;
    p->rate = a1;
    p->volume = a2;
    p->fadeVolume = a2 << 16;
    p->pan = a3;
    t5 = a4;
    __asm__ __volatile__("" : "+r"(t5));
    p->voiceFlags = 0;
    p->fadeStep = 0;
    __asm__ __volatile__("" : "+r"(t5));
    p->priority = t5;
    obj = gSfxDriver;
    p->voice = -1;
    p->handle = id;
    fn = *(MethodFn *)&obj->getSampleClock;
    sSfxNextHandle = id + 1;
    ret = fn(obj);
    quo = (a1 << 8) / sSfxOutputRate;
    cnt = 0;
    p->clock = ret;
    p->step = quo;
    critical_enter();
    q = sSfxSounds;
    prev = NULL;
    while (q != NULL) {
        prev = q;
        if (prev->priority <= p->priority) {
            break;
        }
        if (prev->voiceFlags & 1) {
            cnt++;
        }
        q = prev->next;
    }
    p->next = q;
    if (q != NULL) {
        p->prev = q->prev;
        if (q->prev != NULL) {
            q->prev->next = p;
        }
        q->prev = p;
    } else {
        if (prev != NULL) {
            prev->next = p;
        }
        p->prev = prev;
    }
    if (sSfxSounds == NULL || q == sSfxSounds) {
        sSfxSounds = p;
        p->prev = NULL;
    }
    if (cnt < sSfxNumVoices) {
        sSfxVoiceChangeCount++;
    }
    critical_leave();
    return p->handle;
}

void sfx_update_voices(void)
{
    SfxSound *n;
    s32 i;
    s32 now;
    s32 dt;
    s32 cnt;
    s32 v;
    u32 pos;
    u32 cur;

    if (sSfxInitialized != 0) {
        now = gSfxDriver->getSampleClock();
        n = sSfxSounds;
        gSfxActiveCount = 0;
        while (n != NULL) {
            i = now - n->clock;
            n->clock = now;
            gSfxActiveCount++;
            if ((VOICE_FLAGS(n) & 1) && n->voice == -1) {
                v = VOICE_FLAGS(n) & 2;
                pos = n->position + i * n->step;
                n->position = pos;
                if (v) {
                    s32 len;
                    len = (sSfxSamples[n->sample].loopEnd - sSfxSamples[n->sample].loopStart) << 8;
                    while (n->position >= (u32)(sSfxSamples[n->sample].loopEnd << 8) && len != 0) {
                        n->position -= len;
                    }
                } else {
                    s32 len;
                    len = (sSfxSamples[n->sample].loopEnd - sSfxSamples[n->sample].loopStart) << 8;
                    if (n->position >= (u32)(sSfxSamples[n->sample].length << 8)) {
                        if (sSfxSamples[n->sample].loopStart != sSfxSamples[n->sample].loopEnd) {
                            VOICE_FLAGS(n) |= 2;
                            cur = n->position;
                            while (cur >= (u32)(sSfxSamples[n->sample].loopEnd << 8)) {
                                cur -= len;
                                n->position = cur;
                            }
                        } else {
                            n->flags |= 1;
                        }
                    }
                }
            }
            n = n->next;
        }

        for (n = sSfxSounds; n != NULL; n = n->next) {
            if (n->fadeStep != 0) {
                n->fadeVolume += n->fadeStep;
                v = n->fadeVolume / 0x10000;
                n->volume = v;
                if (v < 0) {
                    n->volume = 0;
                    n->flags |= 1;
                } else {
                    if (v >= 0x80) {
                        n->volume = 0x7F;
                    }
                    if (n->voice != -1) {
                        gSfxDriver->voiceSetVolume(n->voice, n->volume);
                    }
                }
            }
        }

        for (i = 0; i < sSfxNumVoices; i++) {
            n = sSfxVoiceSounds[i];
            if (n != NULL) {
                v = gSfxDriver->voiceGetFlags(i);
                VOICE_FLAGS(n) = v;
                if (!(v & 1)) {
                    n->flags |= 1;
                }
            }
        }

        for (n = sSfxSounds; n != NULL; n = n->next) {
            if (n->flags & 1) {
                if (n->voice != -1) {
                    gSfxDriver->voiceSetFlags(n->voice, 0);
                    sSfxVoiceSounds[n->voice] = NULL;
                    n->voice = -1;
                }
                n->flags |= 2;
            }
        }

        i = 0;
        for (n = sSfxSounds; n != NULL; n = n->next) {
            n->wantsVoice = 0;
            if (n->flags & 1) {
            } else if (n->flags & 2) {
            } else if (i < sSfxNumVoices) {
                n->wantsVoice = 1;
                i++;
            }
        }

        for (i = 0; i < sSfxNumVoices; i++) {
            if (sSfxVoiceSounds[i] != NULL && sSfxVoiceSounds[i]->wantsVoice == 0) {
                sSfxVoiceSounds[i]->position = gSfxDriver->voiceGetPosition(i) << 8;
                sSfxVoiceSounds[i]->voice = -1;
                sSfxVoiceSounds[i] = NULL;
                gSfxDriver->voiceSetFlags(i, 0);
            }
        }

        for (n = sSfxSounds; n != NULL; n = n->next) {
            if (n->wantsVoice != 0 && n->voice == -1) {
                for (i = 0; i < sSfxNumVoices; i++) {
                    if (sSfxVoiceSounds[i] == NULL) {
                        break;
                    }
                }
                if (i != sSfxNumVoices) {
                    sSfxVoiceSounds[i] = n;
                    n->voice = i;
                    VOICE_FLAGS(n) = 1;
                    gSfxDriver->voiceSetSample(i, n->sample);
                    gSfxDriver->voiceSetRate(i, n->rate);
                    gSfxDriver->voiceSetVolume(i, n->volume);
                    gSfxDriver->voiceSetPan(i, n->pan);
                    gSfxDriver->voiceSetPosition(i, n->position >> 8);
                    gSfxDriver->voiceSetFlags(i, VOICE_FLAGS(n));
                }
            }
        }
    }
}

void sfx_set_priority(s32 param_1, s32 param_2)
{
    SfxSound *p;
    SfxSound *q;
    SfxSound *n;
    s32 cnt;

    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    p = sSfxSounds;
    if (p == 0) {
        goto nf;
    }
loop:
    if (p->handle == param_1) {
        goto found;
    }
    p = p->next;
    if (p != 0) {
        goto loop;
    }
nf:
    critical_leave();
    p = 0;
join:
    if (p == 0) {
        goto done;
    }
    p->priority = param_2;
    critical_enter();
    if (p->prev != 0) {
        p->prev->next = p->next;
    }
    if (p->next != 0) {
        p->next->prev = p->prev;
    }
    if (p == sSfxSounds) {
        sSfxSounds = p->next;
    }
    cnt = 0;
    critical_leave();
    critical_enter();
    q = 0;
    n = sSfxSounds;
    while (n != 0) {
        q = n;
        if (q->priority <= p->priority) {
            break;
        }
        if ((q->voiceFlags & 1) != 0) {
            cnt = cnt + 1;
        }
        n = q->next;
    }
    p->next = n;
    if (n == 0) {
        goto nullins;
    }
    p->prev = n->prev;
    if (n->prev != 0) {
        n->prev->next = p;
    }
    n->prev = p;
    goto tail;
found:
    critical_leave();
    goto join;
nullins:
    if (q != 0) {
        q->next = p;
    }
    p->prev = q;
tail:
    if (sSfxSounds == 0 || n == sSfxSounds) {
        sSfxSounds = p;
        p->prev = 0;
    }
    if (cnt < sSfxNumVoices) {
        sSfxVoiceChangeCount = sSfxVoiceChangeCount + 1;
    }
    critical_leave();
done:
    ;
}

void sfx_stream_free(SfxStream *param_1)
{
    SfxStream *piVar1;
    SfxStream *piVar2;
    s32 iVar3;
    SfxSound *iVar4;
    SfxSound *pv;

    critical_enter();
    piVar1 = sSfxStreams;
    if (piVar1 == param_1) {
        sSfxStreams = param_1->next;
    } else {
        piVar2 = piVar1;
        while (piVar2 != NULL) {
            if (piVar2->next == param_1) {
                piVar2->next = param_1->next;
                break;
            }
            piVar2 = piVar2->next;
        }
    }
    critical_leave();
    if (param_1->handle != -1) {
        iVar3 = param_1->handle;
        if (sSfxInitialized != 0) {
            critical_enter();
            for (iVar4 = sSfxSounds; iVar4 != NULL; iVar4 = iVar4->next) {
                if (iVar4->handle == iVar3) {
                    critical_leave();
                    pv = iVar4;
                    goto LAB_800b83c8;
                }
            }
            critical_leave();
            pv = NULL;
LAB_800b83c8:
            if (pv != NULL) {
                pv->flags |= 1;
            }
        }
        param_1->handle = -1;
    }
    if (param_1->samples[0] != -1) {
        sfx_free_sample(param_1->samples[0]);
        param_1->samples[0] = -1;
    }
    if (param_1->samples[1] != -1) {
        sfx_free_sample(param_1->samples[1]);
        param_1->samples[1] = -1;
    }
    mem_free(param_1->buf);
    mem_free((s32)param_1);
    return;
}

static inline SfxSound *find(s32 key)
{
    SfxSound *v;

    critical_enter();
    for (v = sSfxSounds; v != 0; v = v->next) {
        if (v->handle == key) {
            critical_leave();
            return v;
        }
    }
    critical_leave();
    return 0;
}

static inline void mark(s32 key)
{
    SfxSound *v;

    if (sSfxInitialized != 0) {
        v = find(key);
        if (v != 0) {
            v->flags |= 1;
        }
    }
}

static inline s32 alloc(u8 *tb, u32 a, s32 d)
{
    s32 r;
    SfxSample *s;
    s32 buf[4];

    if (sSfxInitialized == 0) {
        return -1;
    }
    r = gSfxDriver->createSample(tb, a, 0, 0, d);
    if (r < 0) {
        return r;
    }
    if (r >= 0x100) {
        gSfxDriver->freeSample(r);
        return -1;
    }
    {
        SfxSample *tb = sSfxSamples;
        s = &tb[r];
    }
    s->name = 0;
    s->refCount = 1;
    gSfxDriver->getSampleInfo(r, buf);
    s->length = buf[0];
    s->loopStart = buf[1];
    s->loopEnd = buf[2];
    s->unk_10 = buf[3];
    return r;
}

void sfx_stream_start(SfxStream *p, s32 a1, s32 a2, s32 a3, s32 a4)
{
    SfxSound *v;
    s32 key;

    if (p->handle != -1) {
        mark(p->handle);
        p->handle = -1;
    }
    if (p->samples[0] != -1) {
        sfx_free_sample(p->samples[0]);
        p->samples[0] = -1;
    }
    if (p->samples[1] != -1) {
        sfx_free_sample(p->samples[1]);
        p->samples[1] = -1;
    }
    p->cur = 1;
    p->rate = a1;
    p->volume = a2;
    p->pan = a3;
    p->priority = a4;
    p->chunkSize = (p->rate * sSfxFrameSamples) / sSfxOutputRate;
    p->samples[0] = alloc(sSfxStreamBuf, p->chunkSize, p->rate);
    p->samples[1] = alloc(sSfxStreamBuf, p->chunkSize, p->rate);
    if (p->samples[0] == -1 || p->samples[1] == -1) {
        fatal_halt(1);
    } else {
        p->filled[0] = 0;
    }
    *(volatile u8 *)&p->filled[1] = 0;
}

static __inline__ SfxSound *find_node(s32 id)
{
    SfxSound *n;

    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->handle == id) {
            critical_leave();
            return n;
        }
    }
    critical_leave();
    return 0;
}

static __inline__ void copy_bytes(u8 *dst, u8 *src, u32 n)
{
    while (n != 0) {
        *dst++ = *src++;
        n--;
    }
}

static __inline__ u32 ring_read(SfxStream *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->count < n) {
fail:
        n = 0;
        goto done;
    }
    if (n == 0) goto fail;
    if (r->size < r->readPos + n) {
        k = r->size - r->readPos;
        copy_bytes(dst, r->buf + r->readPos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->readPos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->readPos, n);
        r->readPos += n;
    }
    r->readPos = r->readPos % r->size;
    r->count -= n;
done:
    return n;
}

static __inline__ u32 ring_read2(SfxStream *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->count < n) goto fail;
    if (n != 0) goto ok;
fail:
    n = 0;
    goto done;
ok:
    if (r->size < r->readPos + n) {
        k = r->size - r->readPos;
        copy_bytes(dst, r->buf + r->readPos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->readPos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->readPos, n);
        r->readPos += n;
    }
    r->readPos = r->readPos % r->size;
    r->count -= n;
done:
    return n;
}

void sfx_stream_update(SfxStream *r)
{
    s32 id;
    SfxSound *nd;
    u32 got;

    if (r->samples[0] == -1) {
        return;
    }
    if (r->handle != -1) {
        id = r->handle;
        if (sSfxInitialized != 0) {
            nd = find_node(id);
            if (nd != 0) {
                nd->flags |= 1;
            }
        }
        r->handle = -1;
    }
    r->filled[r->cur] = 0;
    if (r->filled[1 - r->cur] == 0) {
        if ((got = ring_read(r, sSfxStreamBuf, r->chunkSize)) != 0) {
            sfx_write_sample(r->samples[1 - r->cur], sSfxStreamBuf, got);
            r->filled[1 - r->cur] = 1;
        }
    }
    if (r->filled[1 - r->cur] != 0) {
        r->handle = sfx_play(r->samples[1 - r->cur], r->rate, r->volume, r->pan, r->priority, 0);
    }
    if ((got = ring_read2(r, sSfxStreamBuf, r->chunkSize)) != 0) {
        sfx_write_sample(r->samples[r->cur], sSfxStreamBuf, got);
        r->filled[r->cur] = 1;
    }
    r->cur = 1 - r->cur;
}

void sfx_get_driver_caps(s32 *voices, s32 *modes, s32 *rate, s32 *frameMs)
{
    gSfxDriver->getCaps(voices, modes, rate, frameMs);
}

s32 sfx_init(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6)
{
    s32 iv;
    u8 *ptr;
    s32 v5, v6;

    iv = gSfxDriver->init(p1, p2, p3, p4, p5, p6);
    if (iv != 0) {
        return iv;
    }
    ptr = mem_alloc_dbg(p1 * 4, 0, sStrSfxFile, 0x19B);
    sSfxVoiceSounds = ptr;
    if (ptr == NULL) {
        gSfxDriver->shutdown();
        return 1;
    }
    sSfxVoiceSounds = ptr;
    sSfxNumVoices = p1;
    sSfxDriverFlags = p2;
    v6 = *p6;
    sSfxFrameMs = p4;
    sSfxOutputRate = v6;
    v5 = *p5;
    sSfxSounds = 0;
    sSfxFrameSamples = v5;
    mem_fill(ptr, 0, p1 * 4);
    mem_fill((u8 *)sSfxSamples, 0, sizeof(sSfxSamples));
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

s32 sfx_create_sample(u8 *p1, u32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 r;
    s32 buf[4];
    SfxSample *e;
    SfxSample *base;

    if (sSfxInitialized != 0) {
        r = gSfxDriver->createSample(p1, p2, p3, p4, p5);
        if (r < 0) {
            return r;
        }
        if (r < 0x100) {
            base = sSfxSamples;
            e = base + r;
            e->name = 0;
            e->refCount = 1;
            gSfxDriver->getSampleInfo(r, buf);
            e->length = buf[0];
            e->loopStart = buf[1];
            e->loopEnd = buf[2];
            e->unk_10 = buf[3];
            return r;
        }
        gSfxDriver->freeSample(r);
    }
    return -1;
}

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
        } while (i < 0x100);
    }
}

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

static inline SfxSound *findSnd(s32 id) {
    SfxSound *s;

    critical_enter();
    for (s = sSfxSounds; s != NULL; s = s->next) {
        if (s->handle == id) {
            critical_leave();
            return s;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_stop(s32 id) {
    if (sSfxInitialized != 0) {
        SfxSound *s = findSnd(id);

        if (s != NULL) {
            s->flags |= 1;
        }
    }
}

void sfx_stop_all(void)
{
    SfxSound *p;

    critical_enter();
    p = sSfxSounds;
    if (p != NULL) {
        do {
            p->flags |= 1;
            p = p->next;
        } while (p != NULL);
    }
    critical_leave();
}

void sfx_pause_all(void)
{
    SfxSound *e;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != NULL; e = e->next) {
            if (e->rate != 0) {
                e->pausedRate = e->rate;
                e->rate = 0;
                e->step = (e->rate << 8) / sSfxOutputRate;
                if (e->voice != -1) {
                    gSfxDriver->voiceSetRate(e->voice, e->rate);
                }
            }
        }
        critical_leave();
    }
}

void sfx_resume_all(void)
{
    SfxSound *e;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != NULL; e = e->next) {
            if (e->rate == 0) {
                e->rate = e->pausedRate;
                e->step = (e->rate << 8) / sSfxOutputRate;
                if (e->voice != -1) {
                    gSfxDriver->voiceSetRate(e->voice, e->rate);
                }
            }
        }
        critical_leave();
    }
}

void sfx_driver_stub2(void)
{
    gSfxDriver->unk_68();
}

void sfx_driver_stub3(void)
{
    gSfxDriver->unk_6C();
}

void sfx_set_volume(s32 key, s32 val)
{
    SfxSound *n;
    SfxSound *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = n->next) {
            if (n->handle == key) {
                critical_leave();
                q = n;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q->volume = val;
            q->fadeVolume = val << 16;
            if (q->voice != -1) {
                gSfxDriver->voiceSetVolume(q->voice, val);
            }
        }
    }
}

void sfx_set_pan(s32 a, s32 b) {
    SfxSound *p;
    SfxSound *q;
    if (sSfxInitialized != 0) {
        critical_enter();
        for (p = sSfxSounds; p != 0; p = p->next) {
            if (p->handle == a) {
                critical_leave();
                q = p;
                goto common;
            }
        }
        critical_leave();
        q = 0;
common:
        if (q != 0) {
            q->pan = b;
            if (q->voice != -1) {
                gSfxDriver->voiceSetPan(q->voice, b);
            }
        }
    }
}

static inline SfxSound *find_2(s32 id)
{
    SfxSound *e;

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->handle == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_set_rate(s32 key, s32 v)
{
    SfxSound *e;

    if (sSfxInitialized != 0) {
        e = find_2(key);
        if (e != NULL) {
            e->rate = v;
            e->step = (e->rate << 8) / sSfxOutputRate;
            if (e->voice != -1) {
                gSfxDriver->voiceSetRate(e->voice, e->rate);
            }
        }
    }
}

void sfx_set_position(s32 p1, u32 p2)
{
    SfxSound *n;
    SfxSound *r;
    u32 v;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = n->next) {
            if (n->handle == p1) {
                critical_leave();
                r = n;
                goto found;
            }
        }
        critical_leave();
        r = 0;
    found:
        if (r != 0) {
            v = p2 << 8;
            r->position = v;
            if (r->voice != -1) {
                gSfxDriver->voiceSetPosition(r->voice, v >> 8);
            }
        }
    }
}

void sfx_set_end_callback(s32 param_1, s32 param_2, void (*param_3)(s32, s32))
{
    SfxSound *n;
    SfxSound *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = n->next) {
            if (n->handle == param_1) {
                critical_leave();
                q = n;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q->endCallback = param_3;
            q->endCallbackFlags = param_2;
        }
    }
}

void sfx_set_fade(s32 id, s32 v)
{
    SfxSound *p;
    SfxSound *q;
    s32 iv;

    if (sSfxInitialized == 0) {
        return;
    }
    critical_enter();
    for (p = sSfxSounds; p != 0; p = p->next) {
        if (p->handle == id) {
            critical_leave();
            q = p;
            goto common;
        }
    }
    critical_leave();
    q = 0;
common:
    if (q != 0) {
        iv = v * sSfxFrameMs;
        q->fadeVolume = q->volume << 16;
        q->fadeStep = (iv * 0x10000) / 1000;
    }
}

s32 sfx_get_volume(s32 param_1)
{
    SfxSound *n;
    SfxSound *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != NULL; n = n->next) {
        if (n->handle == param_1) {
            critical_leave();
            r = n;
            goto found;
        }
    }
    critical_leave();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->volume;
}

s32 sfx_get_pan(s32 p1)
{
    SfxSound *n;
    SfxSound *q;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->handle == p1) {
            critical_leave();
            q = n;
            goto found;
        }
    }
    critical_leave();
    q = 0;
found:
    if (q == 0) {
        return -1;
    }
    return q->pan;
}

s32 sfx_get_rate(s32 param_1)
{
    SfxSound *n;
    SfxSound *found;
    s32 r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->handle == param_1) {
            critical_leave();
            found = n;
            goto check;
        }
    }
    critical_leave();
    found = 0;
check:
    r = -1;
    if (found != 0) {
        r = found->rate;
        if (r == 0) {
            r = found->pausedRate;
        }
    }
    return r;
}

s32 sfx_get_priority(s32 param_1)
{
    SfxSound *p;
    SfxSound *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    p = sSfxSounds;
    while (p != NULL) {
        if (p->handle == param_1) {
            critical_leave();
            r = p;
            goto done;
        }
        p = p->next;
    }
    critical_leave();
    r = NULL;
done:
    if (r == NULL) {
        return -1;
    }
    return r->priority;
}

s32 sfx_get_frame_count(void) {
    if (sSfxInitialized == 0) {
        return -1;
    }
    return gSfxDriver->getBufferClock();
}

void sfx_reap_sounds(void) {
    SfxSound *n;
    SfxSound *nx;

    if (sSfxInitialized != 0) {
        critical_enter();
        n = sSfxSounds;
        while (n != 0) {
            nx = n->next;
            if (n->flags & 2) {
                if (n->endCallback != 0 && (n->endCallbackFlags & 1)) {
                    n->endCallback(n->handle, 1);
                }
                critical_enter();
                if (n->prev != 0) {
                    n->prev->next = n->next;
                }
                if (n->next != 0) {
                    n->next->prev = n->prev;
                }
                if (n == sSfxSounds) {
                    sSfxSounds = n->next;
                }
                critical_leave();
                n->prev = (SfxSound *)-1;
            }
            n = nx;
        }
        critical_leave();
    }
}

void sfx_set_frame_callback(void (*a0)(void))
{
    if (sSfxInitialized != 0) {
        sSfxFrameCallback = a0;
    }
}

void (*sfx_get_frame_callback(void))(void)
{
    if (sSfxInitialized == 0) {
        return 0;
    }
    return sSfxFrameCallback;
}

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

SfxStream *sfx_stream_create(s32 p1)
{
    SfxStream *a;
    u8 *b;
    SfxStream *old;

    a = mem_alloc_dbg(sizeof(SfxStream), 0, sStrSfxFile, 0x6AC);
    if (a != NULL) {
        b = mem_alloc_dbg(p1, 0, sStrSfxFile, 0x6AF);
        a->buf = b;
        if (b != NULL) {
            a->size = p1;
            a->readPos = 0;
            a->writePos = 0;
            a->count = 0;
            a->handle = -1;
            a->samples[0] = -1;
            a->samples[1] = -1;
            critical_enter();
            old = sSfxStreams;
            sSfxStreams = a;
            a->next = old;
            critical_leave();
            return a;
        }
        mem_free(a);
    }
    return NULL;
}

void sfx_stream_write(SfxStream *r, u32 n, u8 *src)
{
    u32 t;

    critical_enter();
    if (r->size < r->writePos + n) {
        t = r->size - r->writePos;
        copy(r->buf + r->writePos, src, t);
        copy(r->buf, src + t, n - t);
        r->writePos = n - t;
    } else {
        copy(r->buf + r->writePos, src, n);
        r->writePos += n;
    }
    r->writePos %= r->size;
    r->count += n;
    critical_leave();
}

void sfx_stream_set_rate(SfxStream *o, s32 a1)
{
    u32 q = (u32)(a1 * sSfxFrameSamples) / sSfxOutputRate;
    o->rate = a1;
    o->chunkSize = q;
}

static inline SfxSound *find_3(s32 id)
{
    SfxSound *e;

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->handle == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_stream_set_volume(SfxStream *h, s32 v)
{
    SfxSound *e;
    s32 id;

    h->volume = v;
    critical_enter();
    if (h->handle != -1) {
        id = h->handle;
        if (sSfxInitialized == 0) {
            goto out;
        }
        e = find_3(id);
        if (e != NULL) {
            e->volume = v;
            e->fadeVolume = v << 16;
            if (e->voice != -1) {
                gSfxDriver->voiceSetVolume(e->voice, v);
            }
        }
    }
out:
    critical_leave();
}

void sfx_stream_set_pan(SfxStream *o, s32 v)
{
    SfxSound *n;
    SfxSound *r;
    s32 t;
    s32 id;

    o->pan = v;
    critical_enter();
    t = o->handle;
    if (t == -1) {
        goto done;
    }
    id = t;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->handle == id) {
            critical_leave();
            r = n;
            goto common;
        }
    }
    critical_leave();
    r = 0;
common:
    if (r != 0) {
        r->pan = v;
        if (r->voice != -1) {
            gSfxDriver->voiceSetPan(r->voice, v);
        }
    }
done:
    critical_leave();
}

/* Sets the stream's priority byte but, like sfx_stream_set_pan, the sound's pan. */
void sfx_stream_set_priority(SfxStream *o, s32 v)
{
    SfxSound *n;
    SfxSound *r;
    s32 t;
    s32 id;

    o->priority = v;
    critical_enter();
    t = o->handle;
    if (t == -1) {
        goto done;
    }
    id = t;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->handle == id) {
            critical_leave();
            r = n;
            goto common;
        }
    }
    critical_leave();
    r = 0;
common:
    if (r != 0) {
        r->pan = v;
        if (r->voice != -1) {
            gSfxDriver->voiceSetPan(r->voice, v);
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

void sfx_driver_callback(void) {
    SfxStream *p;

    if (sSfxInitialized != 0) {
        if (sSfxFrameCallback != 0) {
            sSfxFrameCallback();
        }
        p = sSfxStreams;
        while (p != 0) {
            sfx_stream_update(p);
            p = p->next;
        }
        sfx_update_voices();
    }
}

void sfx_copy_bytes(u8 *d, u8 *s, s32 n)
{
    while (n != 0) {
        *d++ = *s++;
        n = n - 1;
    }
}

s32 sfx_str_icmp(u8 *a, u8 *b)
{
    s32 x;
    s32 y;
    for (;;) {
        x = char_to_upper(*a);
        y = char_to_upper(*b);
        if (x != y) {
            return 1;
        }
        if (*a == 0) {
            return 0;
        }
        a++;
        b++;
    }
}

void sfx_get_voice_handles(s32 *out) {
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

SfxSound *sfx_alloc_sound(void)
{
    s32 i;
    critical_enter();
    for (i = 0; i < 0x80; i++) {
        if (sSfxSoundPool[i].prev == (SfxSound *)-1) {
            sSfxSoundPool[i].prev = NULL;
            break;
        }
    }
    critical_leave();
    if (i == 0x80) {
        return NULL;
    }
    return &sSfxSoundPool[i];
}

void sfx_free_sound(SfxSound *p)
{
    p->prev = (SfxSound *)-1;
}

void sfx_insert_sound(SfxSound *n) {
    SfxSound *p;
    SfxSound *q;
    s32 cnt;

    cnt = 0;
    critical_enter();
    p = 0;
    q = sSfxSounds;
    if (q != 0) {
        s32 k = n->priority;
        do {
            p = q;
            if (k < p->priority) {
                if (p->voiceFlags & 1) cnt++;
                q = p->next;
            } else {
                break;
            }
        } while (q != 0);
    }
    n->next = q;
    if (q != 0) {
        n->prev = q->prev;
        if (q->prev != 0) q->prev->next = n;
        q->prev = n;
    } else {
        if (p != 0) p->next = n;
        n->prev = p;
    }
    if (sSfxSounds == 0 || q == sSfxSounds) {
        sSfxSounds = n;
        n->prev = 0;
    }
    if (cnt < sSfxNumVoices) {
        sSfxVoiceChangeCount++;
    }
    critical_leave();
}

void sfx_unlink_sound(SfxSound *n)
{
    critical_enter();
    if (n->prev != NULL) {
        n->prev->next = n->next;
    }
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }
    if (n == sSfxSounds) {
        sSfxSounds = n->next;
    }
    critical_leave();
}

SfxSound *sfx_find_sound(s32 param_1)
{
    SfxSound *p;

    critical_enter();
    p = sSfxSounds;
    while (p != NULL) {
        if (p->handle == param_1) {
            critical_leave();
            return p;
        }
        p = p->next;
    }
    critical_leave();
    return 0;
}

static inline SfxSound *find_4(s32 id)
{
    SfxSound *e;

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->handle == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

u32 sfx_get_position(s32 p)
{
    SfxSound *e;

    if (sSfxInitialized == 0) {
        return -1;
    }
    e = find_4(p);
    if (!(e->voiceFlags & 1)) {
        return -1;
    }
    if (e->voice == -1) {
        return e->position >> 8;
    }
    return gSfxDriver->voiceGetPosition(e->voice);
}

void sfx_set_sample(s32 p1, s32 p2) {
    SfxSound *e;
    SfxSound *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != 0; e = e->next) {
            if (e->handle == p1) {
                critical_leave();
                q = e;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q->sample = p2;
            if (q->voice != -1) {
                gSfxDriver->voiceSetSample(q->voice, p2);
            }
        }
    }
}

void sfx_echo_enable(s32 on)
{
    gSfxDriver->echoEnable(on);
}

u32 sfx_stream_read(SfxStream *r, u32 n, u8 *dst)
{
    u32 t;

    if (r->count < n) {
        return 0;
    }
    if (n == 0) {
        return 0;
    }
    if (r->size < r->readPos + n) {
        t = r->size - r->readPos;
        copy(dst, r->buf + r->readPos, t);
        copy(dst + t, r->buf, n - t);
        r->readPos = n - t;
    } else {
        copy(dst, r->buf + r->readPos, n);
        r->readPos += n;
    }
    r->readPos %= r->size;
    r->count -= n;
    return n;
}

void sfx_lock_sample(s32 i)
{
    sSfxSamples[i].refCount = -1;
}

void sfx_unlock_sample(s32 a0)
{
    sSfxSamples[a0].refCount = 1;
}

s32 sfx_get_voice(s32 param_1)
{
    SfxSound *n;
    SfxSound *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != NULL; n = n->next) {
        if (n->handle == param_1) {
            critical_leave();
            r = n;
            goto found;
        }
    }
    critical_leave();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->voice;
}
