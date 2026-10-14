#include "game.h"

typedef struct {
    u16 time;
    u8 note;
    u8 ch;
} Cmd;

extern Cmd *sSongCursor;

static inline void push_2(s32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue[sSpeechQueueCount] = v;
        sSpeechQueueCount++;
    }
}

static inline s32 check(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot) return 1;
    if (gGame.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) return 1;
    return (gGame.mode == 9) << 1;
}

static inline void queue(s32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue[sSpeechQueueCount] = v;
        sSpeechQueueCount++;
    }
}

static __inline__ void push_4(s32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue[sSpeechQueueCount] = v;
        sSpeechQueueCount++;
    }
}

static inline s32 chk(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame.mode == 9) * 2;
}

static inline void push(u32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue_2[sSpeechQueueCount] = v;
        sSpeechQueueCount++;
    }
}

static inline void push_3(u32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue[sSpeechQueueCount] = v;
        sSpeechQueueCount++;
    }
}

void tunes_init(s32 param_1)
{
    u8 *p;
    s32 i;

    sSpeechQueueCount = 0;
    p = file_find(D_800D7330);
    if (p != NULL) {
        sStreamDataBase = *(s32 *)(p + 0x18);
        sStreamDataEnd = sStreamDataBase + *(s32 *)(p + 0x14);
        sStreamSampleTable = (s32)res_load(gSfxResSet, D_800D1244);
        if (sStreamSampleTable != 0) {
            sStreamMixBuf = mem_alloc_dbg(0x2000, 0, D_800D7338, 0xFC);
            sStreamMixFill = 0;
            sStreamMixWritePos = 0;
            sStreamMixReadPos = 0;
            gN64SndMixCallback = tunes_mix_callback;
            mem_fill(sStreamEchoBuf, 0, 0x1000);
            sStreamEchoPos = 0;
            sStreamChannelCount = param_1;
            sStreamChannels = mem_alloc_dbg(0x130, 0, D_800D7338, 0x10C);
            mem_fill(sStreamChannels, 0, 0x130);
            for (i = 0; i < sStreamChannelCount; i++) {
                tunes_init_channel((s32 *)(sStreamChannels + i * 0x4C));
            }
            sCurrentSong = 0;
            sStreamLock = 0;
            sTunesActive = 1;
        }
    }
}

typedef struct Stream {
    s32 f0;
    s32 handle;
    s32 f8;
    s16 *pcm;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 flags;
    u8 *f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    s32 f44;
    s32 vol;
} Stream;

extern Stream *sStreamChannels_2;

static inline void noise(s16 *q, s32 n)
{
    u16 *p;
    s32 v;

    while (n != 0) {
        p = &sStreamEchoBuf_2[sStreamEchoPos];
        v = *q + ((s16)*p >> 1);
        if (v > 0x7FFF)
            v = 0x7FFF;
        if (v < -0x7FFF)
            v = -0x7FFF;
        *p = v;
        *q++ = v;
        sStreamEchoPos = (sStreamEchoPos + 1) % 0x800;
        n--;
    }
}

s32 tunes_mix_streams(void)
{
    Stream *s;
    s32 i, j, n, v;
    s32 a, b;
    s32 started, mixed;
    s32 pos;
    u8 *addr, *end;
    s16 *q, *q1;
    u16 *p;
    s32 vol;
    s32 flags;
    s32 *buf;

    tunes_update_speech();
    if (sSpeechUseSfxVolume == 0) {
        sStreamChannels_2[2].vol = (gSpeechVolume * gSndMasterVolume) >> 8;
    } else {
        sStreamChannels_2[2].vol = (gSfxVolume * gSndMasterVolume) >> 8;
    }

    for (i = 0; i < sStreamChannelCount; i++) {
        s = &sStreamChannels_2[i];
        if (s->handle >= 0) {
            if (dma_get_status(s->handle) != 3)
                return 0;
            dma_release(s->handle);
            s->handle = -1;
            if (s->f38 >= 0) {
                osInvalDCache(s->f8, 0x220);
                b = s->f8;
                q1 = s->pcm;
                for (j = 0; j < 8; j++) {
                    tunes_decode_adpcm_block(b, q1, 0x80);
                    b += 0x44;
                    q1 += 0x80;
                }
                s->f2C = s->f34;
                s->f30 = s->f38;
                if (s->f3C != 0) {
                    noise(s->pcm, s->f18 << 7);
                }
            }
        }
    }

    if (sStreamLock != 0)
        return 0;
    if (sStreamMixFill > 0x780)
        return 0;

    i = 0;
    j = 0;
    for (; i < sStreamChannelCount; i++) {
        s = &sStreamChannels_2[i];
        if (s->f10 == s->f30 && s->f14 >= s->f2C && s->f14 < s->f2C + 8)
            continue;
        if (s->f10 >= 0) {
        addr = s->f20;
        flags = (s32)(addr + s->f24);
        addr += s->f14 * 0x44;
        s->f18 = (u32)(flags - (s32)addr) / 68;
        if (s->f18 < 0)
            s->f18 = 0;
        if (s->f18 > 8)
            s->f18 = 8;
        if (s->f18 == 0) {
            if (s->f40 != 0) {
                s->f14 = 0;
                addr = s->f20 + s->f14 * 0x44;
                s->f18 = (u32)(s->f20 + s->f24 - addr) / 68;
                if (s->f18 < 0)
                    s->f18 = 0;
                if (s->f18 > 8)
                    s->f18 = 8;
            }
            if (s->f18 == 0) {
                s->f30 = -1;
                s->f10 = -1;
                s->f2C = s->f14;
                if (i == 3 && sAmbientLocked != 0)
                    sAmbientLocked = 0;
                continue;
            }
        }
        s->handle = dma_start(gCartRomHandle, 0, (s32)addr, s->f8, 0x220);
        j = 1;
        s->f38 = s->f10;
        s->f34 = s->f14;
        } else {
            s->f30 = -1;
            s->f2C = s->f14;
        }
    }
    if (j)
        return 0;

    mixed = 0;
    for (i = 0; i < sStreamChannelCount; i++) {
        s = &sStreamChannels_2[i];
        q = s->pcm + (s->f14 - s->f2C) * 0x80;
        if (s->f18 <= 0)
            s->f30 = -1;
        if (s->f30 == -1) {
            if (s->f3C == 0)
                continue;
            q = s->pcm;
            if ((s->flags & 3) != 3) {
                if (s->f44 >= 0)
                    mem_fill((u8 *)q, 0, 0x100);
                noise(q, 0x80);
                if (++s->f44 >= 0x20)
                    s->f3C = 0;
            }
        } else {
            s->f44 = -1;
        }
        flags = s->flags;
        vol = sStreamChannels_2[i].vol;
        if (!(flags & 1)) {
            pos = sStreamMixWritePos;
            if (!mixed) {
                for (j = 0; j < 0x80; j++) {
                    sStreamMixBuf_2[pos] = (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            } else {
                for (j = 0; j < 0x80; j++) {
                    sStreamMixBuf_2[pos] += (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            }
            s->f14++;
            s->f18--;
        } else {
            if (flags & 2) {
                q += 0x40;
                s->f14++;
                s->f18--;
            }
            pos = sStreamMixWritePos;
            s->flags ^= 2;
            if (!mixed) {
                j = 0;
                buf = sStreamMixBuf_2;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 = (mixed + s->f28) / 2;
                    *b1 = mixed;
                    s->f28 = mixed;
                }
            } else {
                j = 0;
                buf = sStreamMixBuf_2;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 += (mixed + s->f28) / 2;
                    *b1 += mixed;
                    s->f28 = mixed;
                }
            }
        }
        mixed = 1;
    }
    if (!mixed) {
        pos = sStreamMixWritePos;
        for (j = 0; j < 0x80; j++) {
            sStreamMixBuf_2[pos] = 0;
            pos = (pos + 1) & 0x7FF;
        }
    }
    sStreamMixWritePos = pos;
    sStreamMixFill += 0x80;
    sSongTick += 1;
    tunes_update_song();
    return 1;
}

typedef struct {
    s32 smp;
    s32 f04;
} Ent_48;

typedef struct {
    s32 x;
    s32 smp;
} Pair_6;

typedef struct {
    s32 count;
    Ent_48 ent[1];
} Song;

typedef struct {
    u8 pad0[8];
    s32 f08;
    u8 pad0C[4];
    s32 f10;
} Smp;

typedef struct {
    u8 pad0[0x10];
    s32 note;
    s32 f14;
    u8 pad18[4];
    s32 f1C;
    s32 f20;
    s32 f24;
    u8 pad28[0x48 - 0x28];
    s32 vol;
} Chan;

extern Chan *sStreamChannels_3;
extern Smp *sStreamSampleTable_2;

void tunes_update_song(void)
{
    Song *s;
    Cmd *c;
    Smp *sm;
    Pair_6 *e;
    Song *s0;

    if (sCurrentSong == NULL) {
        return;
    }
    while (sSongCursor->time <= sSongTick) {
        c = sSongCursor;
        s = sCurrentSong;
        if (c->note == 0xFE) {
            sSongTick = 0;
            sSongCursor = (Cmd *)&s->ent[s->count];
        } else {
            if (c->note == 0xFF) {
                sStreamChannels_3[c->ch & 0xF].note = -1;
            } else {
                sStreamChannels_3[c->ch & 0xF].note = c->note;
                sStreamChannels_3[c->ch & 0xF].f14 = 0;
                e = &((Pair_6 *)s)[c->note];
                sStreamChannels_3[c->ch & 0xF].f20 = (sm = &sStreamSampleTable_2[e->smp])->f08 + sStreamDataBase;
                sStreamChannels_3[c->ch & 0xF].f24 = s->ent[c->note].f04;
                sStreamChannels_3[c->ch & 0xF].f1C = sm->f10;
                sStreamChannels_3[c->ch & 0xF].vol = gStreamVolumes_2[c->ch & 0xF] * gSndMasterVolume >> 8;
            }
            sSongCursor++;
        }
    }
}

typedef struct {
    u32 key;
    u32 val;
} Pair_7;

typedef struct {
    u32 n;
    Pair_7 e[1];
} Tab_5;

typedef struct {
    u32 n;
    u32 key;
    u32 x;
    u32 val;
    u32 y;
} Rec_29;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    u8 pad2[0x4C - 0x3C];
} Slot_11;

extern Tab_5 sSongBuffer;
extern Slot_11 *sStreamChannels_4;

void tunes_play_song(s32 param_1)
{
    s32 n;
    u32 j;
    Rec_29 *r;
    s32 k;
    Tab_5 *tab;
    Tab_5 *t;
    u32 *pw;
    u32 key;

    if (sCurrentSong_2 != 0) {
        tunes_stop_song();
    }
    if (param_1 >= 0) {
        sCurrentSong_2 = res_load(gSfxResSet_4, param_1);
        n = mem_get_size(sCurrentSong_2);
        if (n > 0x800) {
            while (1) ;
        }
        tab = &sSongBuffer;
        memcpy(tab, sCurrentSong_2, n);
        res_unload((u32 *)gSfxResSet_4[1] + param_1);
        n = 0;
        sCurrentSong_2 = (s32 *)tab;
        if (sSongBuffer.n != 0) {
            r = sStreamSampleTable;
            t = tab;
            pw = (u32 *)t;
            do {
                key = pw[n * 2 + 1];
                for (j = 0; j < r->n; j++) {
                    if (r[j].key == key) {
                        pw[n * 2 + 1] = j;
                        pw[n * 2 + 2] = r[j].val;
                        break;
                    }
                }
                if (j == r->n) {
                    while (1) ;
                }
                n++;
            } while (n < t->n);
        }
        sSongTick = 0;
        sSongCursor = sCurrentSong_2 + (sCurrentSong_2[0] * 2 + 1);
        for (n = 0; n < 2; n++) {
            sStreamChannels_4[n].f30 = -1;
            sStreamChannels_4[n].f2C = -1;
            sStreamChannels_4[n].f38 = -1;
            sStreamChannels_4[n].f34 = -1;
        }
        tunes_update_song();
    }
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
    s32 f14;
    s32 pad18[5];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 pad3C[4];
} Ent_49; /* 0x4C */

extern Ent_49 *sStreamChannels_5;

void tunes_stop_song(void) {
    s32 i;
    s32 n;
    Ent_49 *p;
    Ent_49 *q;

    for (i = 0; i < sStreamChannelCount; i++) {
        sStreamChannels_5[i].f10 = -1;
        sStreamChannels_5[i].f14 = -1;
        sStreamChannels_5[i].f30 = -1;
        sStreamChannels_5[i].f2C = -1;
        sStreamChannels_5[i].f38 = -1;
        sStreamChannels_5[i].f34 = -1;
    }

    if (sCurrentSong != 0) {
        sStreamLock = 1;
        for (i = 0; i < sStreamChannelCount; i++) {
            p = &sStreamChannels_5[i];
            for (;;) {
                if (p->f04 < 0) break;
                if (dma_get_status(p->f04) == 3) {
                    dma_release(p->f04);
                    p->f04 = -1;
                    break;
                }
            }
        }
        sStreamLock = 0;
        sCurrentSong = 0;
    }
}

typedef struct { s32 idx; s32 cnt; s32 nidx; s32 ncnt; } Seq;
typedef struct { s32 f0, f4, f8, fC, f10; } Ent_50;

typedef struct {
    u8 pad[0xF4];
    s32 fF4; s32 fF8; s32 fFC; s32 f100; s32 f104; s32 f108;
    u8 pad2[0x120 - 0x10C];
    s32 f120; s32 f124; s32 f128; s32 f12C;
} Obj_37;

extern Seq sCrowdQueue_2;
extern Ent_50 *sStreamSampleTable_3;

void tunes_update_crowd_queue(void)
{
    Obj_37 *obj; Ent_50 *e;
    if (sCrowdQueueCount != 0) {
        if (--sCrowdQueue_2.cnt <= 0) {
            obj = sStreamChannels;
            e = &sStreamSampleTable_3[sCrowdQueue_2.idx];
            obj->fF4 = sCrowdQueue_2.idx;
            obj->f120 = 0;
            obj->f124 = 0;
            obj->fF8 = 0;
            obj->f104 = e->f8 + sStreamDataBase;
            obj->f108 = e->fC;
            obj->f100 = e->f10;
            obj->f12C = (gAmbientVolume_2[0] * gSndMasterVolume_2[0]) >> 8;
            sCrowdQueueCount--;
            sCrowdQueue_2.idx = sCrowdQueue_2.nidx;
            sCrowdQueue_2.cnt = sCrowdQueue_2.ncnt;
        }
    }
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
} Ent_51;

typedef struct {
    s32 pad[0x3D];
    s32 f0F4;
    s32 f0F8;
    s32 f0FC;
    s32 f100;
    s32 f104;
    s32 f108;
    s32 pad2[6];
    s32 f124;
} Ctl_8;

extern Ctl_8 *sStreamChannels_6;
extern Ent_51 *sStreamSampleTable_4;

void tunes_set_ambient(s32 param_1, s32 param_2, s32 arg2)
{
    if (sAmbientLocked != 0) {
        return;
    }
    if (param_2 == 0) {
        if (sStreamChannels_6->f0F4 != -1) {
            sStreamChannels_6->f0F4 = -1;
            sStreamChannels_6->f124 = 0;
            gAmbientVolume = 0;
        }
        return;
    }
    if (sStreamChannels_6->f0F4 != param_1) {
        Ent_51 *t;
        s32 v10;
        sStreamChannels_6->f0F4 = param_1;
        t = sStreamSampleTable_4 + param_1;
        sStreamChannels_6->f0F8 = 0;
        sStreamChannels_6->f104 = t->f08 + sStreamDataBase;
        sStreamChannels_6->f108 = t->f0C;
        v10 = t->f10;
        sStreamChannels_6->f124 = 1;
        sStreamChannels_6->f100 = v10;
    }
    gAmbientVolume = param_2;
}

typedef struct Plr_5 {
    u8 pad0[0xA8];
    s32 cur;   /* A8 */
    s32 fAC;   /* AC */
    s32 fB0;
    s32 fB4;
    s32 fB8;
    s32 fBC;
    u8 pad1[0xD4 - 0xC0];
    s32 fD4;
    s32 fD8;
    u8 pad2[0xE0 - 0xDC];
    s32 fE0;
} Plr_5;

typedef struct Rec_30 {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
} Rec_30;

typedef struct { u8 pad[0x662]; s16 f; u8 rest[0x7E0 - 0x664]; } Tbl7E0;

extern Plr_5 *sStreamChannels_7;
extern Rec_30 *sStreamSampleTable_5;
extern Tbl7E0 gRiders_52[];

void tunes_update_speech(void)
{
    s32 v;
    s32 n;
    s32 lo;
    s32 t;
    s32 f;
    s32 c;
    s32 e;
    Plr_5 *p;
    Rec_30 *r;

    if (sSpeechQueueCount_2[0] == 0) {
        return;
    }
    if (sStreamChannels_7->cur != -1 && sSpeechPriority >= 0) {
        return;
    }
    v = sSpeechQueue[0];
    if (v < 0) {
        t = (v >> 16) & 0xFF;
        if (t == 1) {
            n = (v >> 8) & 0xFF;
            n = gRiders_52[n].f;
            lo = v & 0xFF;
            if (lo < n) {
                n = lo;
            }
            lo = gEvent.numRiders - 2;
            if (lo < n) {
                n = lo;
            }
        } else if (t == 2) {
            n = (v >> 8) & 0xFF;
            n = gRiders_52[n].f;
            lo = v & 0xFF;
            if (n < lo) {
                n = lo;
            }
            if (n <= 0) {
                n = 1;
            }
        } else {
            goto bad;
        }
        v = sPlaceClips[n];
        goto ok;
    bad:
        sSpeechQueueCount_2[0]--;
        return;
    }
ok:
    if (gPodiumMode == 0) {
        if (sStreamChannels_7->cur != -1 && sSpeechPriority >= 0) {
            f = 1;
        } else {
            sStreamChannels_7->cur = v;
            r = &sStreamSampleTable_5[v];
            sStreamChannels_7->fD4 = 0;
            sStreamChannels_7->fD8 = 0;
            sStreamChannels_7->fAC = 0;
            sStreamChannels_7->fB8 = r->f8 + sStreamDataBase;
            sSpeechPriority = 0;
            sStreamChannels_7->fBC = r->fC;
            sSpeechUseSfxVolume = 0;
            sStreamChannels_7->fB4 = r->f10;
            f = 0;
        }
    }
    if (f == 0) {
        p = sStreamChannels_7;
        p->fD4 = 1;
        e = gSpeechVolume;
        c = --sSpeechQueueCount_2[0];
        p->fE0 = e;
        if (c != 0) {
            memcpy(sSpeechQueue, sSpeechQueue + 1, c << 2);
        }
        gLastSpeechTime = gRaceFrame;
    }
}

static inline s32 check_2(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot_2) return 1;
    if (gGame.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) return 1;
    return (gGame.mode == 9) * 2;
}

void announce_lead_collision(u32 a, u32 b)
{
    if (gRaceFrame - sLastLeadCollisionTime < 600) return;
    if (check_2(&a)) return;
    if (check_2(&b)) return;
    sLastLeadCollisionTime = gRaceFrame;
    random_byte();
    if (random_byte() & 1) {
        if (sSpeechQueueCount >= 5) return;
        push_2(0x75);
        push_2(sRiderNameClips[a + 12]);
        push_2(0xBA);
        push_2(sRiderNameClips[b + 18]);
    } else {
        if (sSpeechQueueCount >= 5) return;
        push_2(sRiderNameClips[a + 12]);
        push_2(0xBA);
        push_2(sRiderNameClips[b + 18]);
        if (random_byte() & 1) {
            push_2(0x76);
        } else {
            push_2(0x74);
        }
    }
}

void announce_lap(s32 param_1)
{
    if (param_1 == 0) {
        push_2(0x83);
    } else if (param_1 == 1) {
        push_2(0x82);
    } else if (param_1 == 2) {
        push_2(0x81);
    }
}

void announce_leader_lap(s32 param_1, s32 param_2)
{
    s32 *t;
    s32 v;

    if (sSpeechQueueCount >= 6) {
        return;
    }
    if (check((u32 *)&param_1)) {
        return;
    }
    if (param_2 == gEvent.numLaps - 2) {
        queue(sFinalLapClips[random_byte() & 1]);
    } else if (param_2 == 0) {
        queue(sLeaderFirstLapClips[random_byte() % 3]);
    } else {
        if (param_2 != 1) {
            return;
        }
        queue(0x88);
    }
    queue(sRiderNameClips[param_1 + 0x12]);
    if (param_1 != sLastLeader) {
        queue(sTakesLeadClips[random_byte() & 3]);
    } else {
        queue(sStillLeadsClips[random_byte() % 3]);
    }
    sLastLeader = param_1;
}

typedef struct { u8 pad[0x668]; s16 f668; u8 pad2[0x7E0 - 0x66A]; } Plr_6;
extern Plr_6 gRiders_53[];

static inline s32 check_3(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot) return 1;
    if (gGame.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) return 1;
    return (gGame.mode == 9) * 2;
}

void announce_place(u32 a, u32 b)
{
    if (gEvent.time < 0x168) return;
    if (sSpeechQueueCount >= 6) return;
    if (gRiders_53[a].f668 >= 0) return;
    if (check_3(&a)) return;
    push_2(sRiderNameClips[a + 12]);
    if (b == 1) {
        if (a != sLastLeader_2) {
            push_2(sTakesLeadClips[random_byte() & 3]);
        } else {
            push_2(0x8A);
        }
        sLastLeader_2 = a;
    } else if (b == gGame.numPlayers + gGame.numCpuRiders) {
        if (random_byte() & 1) {
            push_2(0x7F);
        } else {
            push_2(0xBE);
            push_2(0x9F);
        }
    } else if (b == 2) {
        if (random_byte() & 1) {
            push_2(0x80);
        } else {
            push_2(0xBE);
            push_2(0x9E);
        }
    } else {
        push_2(0xBF);
        if (b == 3) {
            push_2(0x9D);
        } else if (b == 4) {
            push_2(0x9C);
        } else if (b == 5) {
            push_2(0x9A);
        }
    }
}

void announce_winner(u32 param_1)
{
    s32 r;

    r = (s32)random_byte() % 3;
    if (check(&param_1)) {
        return;
    }
    if (r == 0) {
        if (sSpeechQueueCount >= 6) {
            return;
        }
        if (random_byte() & 1) {
            queue(0xB8);
        } else {
            queue(0xB9);
        }
        queue(sRiderNameClips[param_1]);
        queue(0x78);
    } else if (r == 1) {
        if (sSpeechQueueCount >= 7) {
            return;
        }
        queue(sRiderNameClips[param_1]);
        queue(0x99);
    } else {
        if (sSpeechQueueCount >= 7) {
            return;
        }
        queue(sRiderNameClips[param_1]);
        queue(0x77);
    }
}

static __inline__ s32 chk_2(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot_2) {
        return 1;
    }
    if (gGame.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame.mode == 9) << 1;
}

void announce_crash(u32 param_1)
{
    u32 r;

    if (sSpeechQueueCount != 0) {
        return;
    }
    if (chk_2(&param_1) == 0) {
        r = random_byte() % 3;
        if (r == 0) {
            push_4(sRiderNameClips[param_1 + 0x12]);
        } else {
            push_4(sRiderNameClips[param_1 + 6]);
        }
        push_4(sCrashClips[r]);
    }
}

static __inline__ s32 chk_3(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame.mode == 9) << 1;
}

void announce_crash_landing(u32 param_1)
{
    if (*(s32 *)(gCurTrack + 0x2c) != 0) {
        announce_crash_2(param_1);
        return;
    }
    if (sSpeechQueueCount != 0) {
        return;
    }
    if (chk_3(&param_1) == 0) {
        push_4(sRiderNameClips[param_1 + 6]);
        push_4(sCrashLandingClips[random_byte() % 3]);
    }
}

void announce_takeout(u32 a, u32 b)
{
    u32 r;

    if (sSpeechQueueCount != 0) {
        return;
    }
    if ((s32)(gRaceFrame - gAnnounceTimes[a]) < 0x1E0) {
        announce_crash(b);
        return;
    }
    if (chk(&a) != 0) {
        return;
    }
    if (chk(&b) != 0) {
        return;
    }
    r = random_byte() % 3;
    if (r == 0) {
        push(sRiderNameClips[a + 12]);
    } else {
        push(sRiderNameClips[a + 6]);
    }
    push(sTakeoutClips[r]);
    push(sRiderNameClips[b + 18]);
}

static inline s32 check_4(u32 *p)
{
    if (gGame.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame.riders[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame.mode == 9) * 2;
}

void announce_trick(u32 a, s32 b)
{
    s32 r;
    u32 *pa;

    if (sSpeechQueueCount < 7) {
        pa = &a;
        if (gGhostRace != 0 && a == gGhostSlot) {
            r = 1;
        } else {
            r = check_4(pa);
        }
        if (r != 1 && gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
            sSpeechQueue[sSpeechQueueCount++] = b;
        }
    }
}

void func_8004E3EC(u32 a, u32 b, s32 c)
{
    if (gEvent.time < 0x168) {
        return;
    }
    if (sSpeechQueueCount >= 6) {
        return;
    }
    if (chk(&a) != 0) {
        return;
    }
    if (chk(&b) != 0) {
        return;
    }
    push(sRiderNameClips[a + 12]);
    if (c != 0) {
        push(0x95);
    } else {
        push(0x96);
    }
    push(sRiderNameClips[b + 18]);
}

static inline s32 chk_4(u32 p, u32 *q)
{
    if (gGhostRace != 0 && p == gGhostSlot) {
        return 1;
    }
    if (gGame.mode == 2) {
        return 1;
    }
    if (*q >= 6) {
        return 1;
    }
    gAnnounceTimes_2[*q] = gRaceFrame;
    *q = gGame.riders[*q];
    if (*q >= 6) {
        return 1;
    }
    return (gGame.mode == 9) * 2;
}

void announce_place_change(u32 param_1, s32 param_2, u32 param_3)
{
    u32 t1 = param_1;
    u32 uVar2;
    u32 sh;
    if (chk_4(t1, &param_1) != 0) {
        return;
    }
    if (gGhostRace != 0) {
        return;
    }
    if ((s32)(gRaceFrame - sLastPlaceChangeTime) < 900) {
        return;
    }
    sLastPlaceChangeTime = gRaceFrame;
    if (sSpeechQueueCount >= 6) {
        return;
    }
    push_3(sRiderNameClips[param_1 + 0xc]);
    if (param_2 < (s32)param_3) {
        push_3(0x97);
        sh = t1 << 8;
        uVar2 = 0x80010000;
    } else {
        push_3(0x98);
        sh = t1 << 8;
        uVar2 = 0x80020000;
    }
    push_3(sh | (param_3 | uVar2));
}

void announce_finish(u32 p)
{
    if (chk(&p) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    push(sRiderNameClips[p + 12]);
    push(0x79);
}

void announce_early_leader(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 6) {
        return;
    }
    push(0xB8);
    push(sRiderNameClips[param_1]);
    push(0x91);
}

void announce_best_lap(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    push(sRiderNameClips[param_1]);
    push(0x7A);
}

void announce_record(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (sSpeechQueueCount >= 7) {
        return;
    }
    push(sRiderNameClips[param_1]);
    push(0x7B);
}

void announce_goal(void)
{
    push_3(0x94);
    if (gCurTrack_4->f2C == 0) {
        s32 k = sCrowdQueueCount;
        s32 v = sCrowdCheerClip;
        if (k < 2) {
            sCrowdQueue[k * 2] = v;
            sCrowdQueue[k * 2 + 1] = 0x1E;
            sCrowdQueueCount = k + 1;
        }
    }
}

void func_8004F064(u32 param_1)
{
    s32 *b;
    s32 *s;

    if (chk(&param_1) != 0) {
        return;
    }
    if (param_1 == 0 || param_1 == 3) {
        return;
    }
    b = sStreamChannels_8;
    b[61] = 0x73;
    b[62] = 0;
    s = sStreamSampleTable;
    b[65] = s[577] + sStreamDataBase;
    b[66] = s[578];
    sAmbientLocked = 1;
    b[64] = s[579];
    b[73] = 0;
    gAmbientVolume_2[0] = gSfxVolume;
}

s32 tunes_is_song_playing(void) {
    return sCurrentSong != 0;
}

typedef struct {
    s32 p0;
    s32 p1;
    s32 w8;
    s32 wC;
    s32 rest[15];
} Ent4C;

extern Ent4C *sStreamChannels_9;

void tunes_shutdown(void)
{
    s32 i;
    if (sCurrentSong != 0) {
        tunes_stop_song();
    }
    res_unload((u32 *)((u8 *)gSfxResSet_2[1] + D_800D1244 * 4));
    i = 0;
    mem_free(sStreamMixBuf);
    if (0 < sStreamChannelCount) {
        do {
            mem_free(sStreamChannels_9[i].w8);
            mem_free(sStreamChannels_9[i].wC);
            i++;
        } while (i < sStreamChannelCount);
    }
    mem_free(sStreamChannels_9);
    sTunesActive = 0;
}

typedef struct {
    u8 pad0[0xA8];
    s32 a8;
    s32 ac;
    s32 b0;
    s32 b4;
    s32 b8;
    s32 bc;
    u8 pad1[0x14];
    s32 d4;
    s32 d8;
} Obj_38;

typedef struct {
    u8 pad0[8];
    s32 e8;
    s32 ec;
    s32 e10;
} Ent_52;

extern Obj_38 *sStreamChannels_10;
extern Ent_52 *sStreamSampleTable_6;

int tunes_play_speech(s32 param_1, s32 param_2, s32 param_3)
{
    Ent_52 *iVar3;

    if (gPodiumMode != 0) {
        return gPodiumMode;
    }
    if (sStreamChannels_10->a8 != -1 && param_2 <= sSpeechPriority) {
        return 1;
    }
    iVar3 = sStreamSampleTable_6 + param_1;
    sStreamChannels_10->d4 = 0;
    sStreamChannels_10->d8 = 0;
    sStreamChannels_10->a8 = param_1;
    sStreamChannels_10->ac = 0;
    sStreamChannels_10->b8 = iVar3->e8 + sStreamDataBase;
    sSpeechPriority = param_2;
    sStreamChannels_10->bc = iVar3->ec;
    sSpeechUseSfxVolume = param_3;
    sStreamChannels_10->b4 = iVar3->e10;
    return 0;
}

s32 tunes_get_speech_queue_time(void)
{
    s32 i, sum, w, f;

    sum = 0;
    for (i = 0; i < sSpeechQueueCount; i++) {
        w = *(s32 *)(sStreamSampleTable + ((sSpeechQueue[i] * 5) << 2) + 0xc);
        f = w * 2;
        if (*(s32 *)(sStreamSampleTable + ((f * 5) << 2) + 0x10) & 1) {
            f = w << 2;
        }
        sum += (f * 0x3c) / 0x5622;
    }
    return sum;
}

u32 tunes_find_sample(u32 key)
{
    u32 *p;
    u32 i = 0;
    u32 n;

    p = sStreamSampleTable;
    if (p[0] != 0) {
        n = p[0];
        do {
            if (p[5] == key) {
                return i;
            }
            i++;
            p += 5;
        } while (i < n);
    }
    return 0xFFFFFFFF;
}

static inline void push_5(s32 v, s32 w) {
    s32 n = sCrowdQueueCount;
    if (n < 2) {
        do {
            do {
                sCrowdQueue[n * 2] = v;
            } while (0);
        } while (0);
        sCrowdQueue[n * 2 + 1] = w;
        sCrowdQueueCount = n + 1;
    }
}

void tunes_queue_crowd(s32 p) {
    if (gCurTrack_3[11] == 0) {
        push_5(sCrowdClips[p], 0x1E);
    }
}

void announce_reset(void)
{
    s32 val = -0x708;
    s32 i = 7;

    do {
        gAnnounceTimes[i] = val;
        i--;
    } while (i >= 0);
    sLastLeader = -1;
    gLastSpeechTime = -1;
    sLastPlaceChangeTime = -0x708;
    sLastLeadCollisionTime = -0x708;
    sCrowdQueueCount = 0;
    sAmbientLocked = 0;
}

void tunes_queue_speech(s32 a0)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0) {
        s32 n = sSpeechQueueCount;
        if (n < 8) {
            sSpeechQueue[n] = a0;
            sSpeechQueueCount = n + 1;
        }
    }
}

typedef struct {
    u8 pad0[0xA0];
    u8 *p0;
    u8 *p1;
    s32 fA8;
    s32 fAC;
    u8 pad1[0xC4 - 0xB0];
    s32 fC4;
    s32 fC8;
    s32 fCC;
    s32 fD0;
} Obj_39;

extern Obj_39 *sStreamChannels_11;

void tunes_stop_speech(void)
{
    Obj_39 *o;
    u8 *p;

    o = sStreamChannels_11;
    p = o->p0;
    o->fA8 = -1;
    o->fAC = -1;
    o->fC8 = -1;
    o->fC4 = -1;
    o->fD0 = -1;
    o->fCC = -1;
    mem_fill(p, 0, 0x220);
    mem_fill(sStreamChannels_11->p1, 0, 0x800);
    sSpeechQueueCount = 0;
    mem_fill(sStreamEchoBuf, 0, 0x1000);
    mem_fill(sStreamMixBuf, 0, 0x2000);
}

void func_8004F5C8(s32 on)
{
    if (on) {
        push_3(0x92);
    } else {
        push_3(0x93);
    }
}

void tunes_mix_callback(s32 *p, s32 n)
{
    if (sStreamMixFill < n) {
        n = sStreamMixFill;
    }
    if (sStreamLock == 0) {
        sStreamMixFill = sStreamMixFill - n;
        while (n != 0) {
            *p = *p + sStreamMixBuf_2[sStreamMixReadPos];
            p++;
            *p = *p + sStreamMixBuf_2[sStreamMixReadPos];
            p++;
            sStreamMixReadPos = (sStreamMixReadPos + 1) & 0x7FF;
            n--;
        }
    }
}

void tunes_init_channel(s32 *p) {
    if (p[2] == 0) {
        p[2] = mem_alloc_dbg(0x220, 0, (s32)&D_800D7338, 0x128);
        p[3] = mem_alloc_dbg(0x800, 0, (s32)&D_800D7338, 0x129);
        p[0] = 0;
        p[1] = -1;
        p[4] = -1;
        p[0xC] = -1;
    }
}

void tunes_free_channel(u8 *param_1)
{
    if (*(s32 *)(param_1 + 8) != 0) {
        mem_free(*(s32 *)(param_1 + 8));
        mem_free(*(s32 *)(param_1 + 0xC));
        mem_fill(param_1, 0, 0x4C);
        *(s32 *)(param_1 + 4) = -1;
        *(s32 *)(param_1 + 0x10) = -1;
        *(s32 *)(param_1 + 0x30) = -1;
    }
}

void tunes_update(void) {
    if (sTunesActive != 0) {
        while (tunes_mix_streams() != 0) {
        }
    }
}

void tunes_apply_echo(s16 *p, s32 n) {
    s32 v;
    s32 i2;
    s32 t;
    for (; n != 0; n--) {
        v = *p + ((s16)sStreamEchoBuf_2[sStreamEchoPos] >> 1);
        if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        if (v < -0x7FFF) {
            v = -0x7FFF;
        }
        sStreamEchoBuf_2[sStreamEchoPos] = v;
        *p = v;
        p++;
        i2 = sStreamEchoPos + 1;
        t = i2;
        if (i2 < 0) {
            t = sStreamEchoPos + 0x800;
        }
        sStreamEchoPos = i2 - (t >> 11) * 0x800;
    }
}

typedef struct {
    char pad[0x48];
    s32 f48;
} Ent_53;

void tunes_update_volumes(void)
{
    s32 k = 0;

    if (sTunesActive != 0) {
        s32 *p = gStreamVolumes_2;
        s32 a2 = gSndMasterVolume;
        Ent_53 *e = sStreamChannels;
        do {
            s32 v = *p;
            p++;
            k++;
            e->f48 = (v * a2) >> 8;
            e++;
        } while (k < 4);
    }
}

s32 announce_check_rider(u32 *p)
{
    Game *base;
    u32 u;
    if (gGhostRace != 0) {
        if (*p == gGhostSlot) {
            return 1;
        }
    }
    base = &gGame;
    if (base->mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    u = base->riders[*p];
    *p = u;
    if (u < 6) {
        return (base->mode == 9) << 1;
    }
    return 1;
}
