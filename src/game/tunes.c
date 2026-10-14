#include "game.h"

typedef struct {
    u8 pad[0x1C];
    s32 map[12];
    s32 mode;
} S70;

typedef struct {
    u8 pad0[0x1C];
    u32 tab[12];
    s32 mode;
} Blk_3;

typedef struct { u8 pad[0x1C]; u32 tab[12]; u32 mode; } Game;

extern SongEvent *sSongCursor;
extern S70 gGame_6;
extern Blk_3 gGame_7;
extern Game gGame_3;

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
    if (gGame_6.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_6.map[*p];
    if (*p >= 6) return 1;
    return (gGame_6.mode == 9) << 1;
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
    if (gGame_3.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_3.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame_3.mode == 9) * 2;
}

static inline void push(u32 v)
{
    if (gReplayPlayback == 0 && gCreditsMode == 0 && gPodiumMode == 0 && sSpeechQueueCount < 8) {
        sSpeechQueue[sSpeechQueueCount] = v;
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
    FileDirEntry *p;
    s32 i;

    sSpeechQueueCount = 0;
    p = file_find(D_800D7330);
    if (p != NULL) {
        sStreamDataBase = (s32)p->rom;
        sStreamDataEnd = sStreamDataBase + p->size;
        sStreamSampleTable = res_load(gSfxResSet, D_800D1244);
        if (sStreamSampleTable != 0) {
            sStreamMixBuf = mem_alloc_dbg(0x2000, 0, D_800D7338, 0xFC);
            sStreamMixFill = 0;
            sStreamMixWritePos = 0;
            sStreamMixReadPos = 0;
            gN64SndMixCallback = tunes_mix_callback;
            mem_fill(sStreamEchoBuf, 0, 0x1000);
            sStreamEchoPos = 0;
            sStreamChannelCount = param_1;
            sStreamChannels = mem_alloc_dbg(4 * sizeof(StreamChannel), 0, D_800D7338, 0x10C);
            mem_fill(sStreamChannels, 0, 4 * sizeof(StreamChannel));
            for (i = 0; i < sStreamChannelCount; i++) {
                tunes_init_channel(&sStreamChannels[i]);
            }
            sCurrentSong = 0;
            sStreamLock = 0;
            sTunesActive = 1;
        }
    }
}

static inline void echo_ring(s16 *q, s32 n)
{
    u16 *p;
    s32 v;

    while (n != 0) {
        p = &sStreamEchoBuf[sStreamEchoPos];
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
    StreamChannel *s;
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
        sStreamChannels[2].volume = (gSpeechVolume * gSndMasterVolume) >> 8;
    } else {
        sStreamChannels[2].volume = (gSfxVolume * gSndMasterVolume) >> 8;
    }

    for (i = 0; i < sStreamChannelCount; i++) {
        s = &sStreamChannels[i];
        if (s->dmaHandle >= 0) {
            if (dma_get_status(s->dmaHandle) != 3)
                return 0;
            dma_release(s->dmaHandle);
            s->dmaHandle = -1;
            if (s->dmaSample >= 0) {
                osInvalDCache(s->dmaBuf, 0x220);
                b = s->dmaBuf;
                q1 = s->pcm;
                for (j = 0; j < 8; j++) {
                    tunes_decode_adpcm_block(b, q1, 0x80);
                    b += 0x44;
                    q1 += 0x80;
                }
                s->bufBlock = s->dmaBlock;
                s->bufSample = s->dmaSample;
                if (s->echo != 0) {
                    echo_ring(s->pcm, s->blocks << 7);
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
        s = &sStreamChannels[i];
        if (s->sample == s->bufSample && s->block >= s->bufBlock && s->block < s->bufBlock + 8)
            continue;
        if (s->sample >= 0) {
        addr = s->data;
        flags = (s32)(addr + s->size);
        addr += s->block * 0x44;
        s->blocks = (u32)(flags - (s32)addr) / 68;
        if (s->blocks < 0)
            s->blocks = 0;
        if (s->blocks > 8)
            s->blocks = 8;
        if (s->blocks == 0) {
            if (s->loop != 0) {
                s->block = 0;
                addr = s->data + s->block * 0x44;
                s->blocks = (u32)(s->data + s->size - addr) / 68;
                if (s->blocks < 0)
                    s->blocks = 0;
                if (s->blocks > 8)
                    s->blocks = 8;
            }
            if (s->blocks == 0) {
                s->bufSample = -1;
                s->sample = -1;
                s->bufBlock = s->block;
                if (i == 3 && sAmbientLocked != 0)
                    sAmbientLocked = 0;
                continue;
            }
        }
        s->dmaHandle = dma_start(gCartRomHandle, 0, (s32)addr, s->dmaBuf, 0x220);
        j = 1;
        s->dmaSample = s->sample;
        s->dmaBlock = s->block;
        } else {
            s->bufSample = -1;
            s->bufBlock = s->block;
        }
    }
    if (j)
        return 0;

    mixed = 0;
    for (i = 0; i < sStreamChannelCount; i++) {
        s = &sStreamChannels[i];
        q = s->pcm + (s->block - s->bufBlock) * 0x80;
        if (s->blocks <= 0)
            s->bufSample = -1;
        if (s->bufSample == -1) {
            if (s->echo == 0)
                continue;
            q = s->pcm;
            if ((s->flags & 3) != 3) {
                if (s->echoTail >= 0)
                    mem_fill((u8 *)q, 0, 0x100);
                echo_ring(q, 0x80);
                if (++s->echoTail >= 0x20)
                    s->echo = 0;
            }
        } else {
            s->echoTail = -1;
        }
        flags = s->flags;
        vol = sStreamChannels[i].volume;
        if (!(flags & 1)) {
            pos = sStreamMixWritePos;
            if (!mixed) {
                for (j = 0; j < 0x80; j++) {
                    sStreamMixBuf[pos] = (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            } else {
                for (j = 0; j < 0x80; j++) {
                    sStreamMixBuf[pos] += (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            }
            s->block++;
            s->blocks--;
        } else {
            if (flags & 2) {
                q += 0x40;
                s->block++;
                s->blocks--;
            }
            pos = sStreamMixWritePos;
            s->flags ^= 2;
            if (!mixed) {
                j = 0;
                buf = sStreamMixBuf;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 = (mixed + s->lastSample) / 2;
                    *b1 = mixed;
                    s->lastSample = mixed;
                }
            } else {
                j = 0;
                buf = sStreamMixBuf;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 += (mixed + s->lastSample) / 2;
                    *b1 += mixed;
                    s->lastSample = mixed;
                }
            }
        }
        mixed = 1;
    }
    if (!mixed) {
        pos = sStreamMixWritePos;
        for (j = 0; j < 0x80; j++) {
            sStreamMixBuf[pos] = 0;
            pos = (pos + 1) & 0x7FF;
        }
    }
    sStreamMixWritePos = pos;
    sStreamMixFill += 0x80;
    sSongTick += 1;
    tunes_update_song();
    return 1;
}

void tunes_update_song(void)
{
    Song *s;
    SongEvent *c;
    StreamSample *sm;
    s32 *e;

    if (sCurrentSong == NULL) {
        return;
    }
    while (sSongCursor->time <= sSongTick) {
        c = sSongCursor;
        s = sCurrentSong;
        if (c->note == 0xFE) {
            sSongTick = 0;
            sSongCursor = (SongEvent *)&s->samples[s->count];
        } else {
            if (c->note == 0xFF) {
                sStreamChannels[c->channel & 0xF].sample = -1;
            } else {
                sStreamChannels[c->channel & 0xF].sample = c->note;
                sStreamChannels[c->channel & 0xF].block = 0;
                e = (s32 *)s + c->note * 2;     /* e[1]: s->samples[c->note].sample */
                sStreamChannels[c->channel & 0xF].data = (u8 *)((sm = &sStreamSampleTable[e[1]])->offset + sStreamDataBase);
                sStreamChannels[c->channel & 0xF].size = s->samples[c->note].size;
                sStreamChannels[c->channel & 0xF].flags = sm->flags;
                sStreamChannels[c->channel & 0xF].volume = gStreamVolumes_2[c->channel & 0xF] * gSndMasterVolume >> 8;
            }
            sSongCursor++;
        }
    }
}

extern Song sSongBuffer;   /* 0x800 bytes */

void tunes_play_song(s32 param_1)
{
    s32 n;
    u32 j;
    StreamSample *r;
    s32 k;
    Song *tab;
    Song *t;
    u32 *pw;
    u32 key;

    if (sCurrentSong != 0) {
        tunes_stop_song();
    }
    if (param_1 >= 0) {
        sCurrentSong = res_load(gSfxResSet_4, param_1);
        n = mem_get_size(sCurrentSong);
        if (n > 0x800) {
            while (1) ;
        }
        tab = &sSongBuffer;
        memcpy(tab, sCurrentSong, n);
        res_unload((u32 *)gSfxResSet_4[1] + param_1);
        n = 0;
        sCurrentSong = tab;
        if (sSongBuffer.count != 0) {
            r = sStreamSampleTable;
            t = tab;
            pw = (u32 *)t;
            do {
                key = pw[n * 2 + 1];
                for (j = 0; j < r->count; j++) {
                    if (r[j].key == key) {
                        pw[n * 2 + 1] = j;
                        pw[n * 2 + 2] = r[j].size;
                        break;
                    }
                }
                if (j == r->count) {
                    while (1) ;
                }
                n++;
            } while ((u32)n < (u32)t->count);
        }
        sSongTick = 0;
        sSongCursor = (SongEvent *)&sCurrentSong->samples[sCurrentSong->count];
        for (n = 0; n < 2; n++) {
            sStreamChannels[n].bufSample = -1;
            sStreamChannels[n].bufBlock = -1;
            sStreamChannels[n].dmaSample = -1;
            sStreamChannels[n].dmaBlock = -1;
        }
        tunes_update_song();
    }
}

void tunes_stop_song(void) {
    s32 i;
    s32 n;
    StreamChannel *p;
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
            p = &sStreamChannels[i];
            for (;;) {
                if (p->dmaHandle < 0) break;
                if (dma_get_status(p->dmaHandle) == 3) {
                    dma_release(p->dmaHandle);
                    p->dmaHandle = -1;
                    break;
                }
            }
        }
        sStreamLock = 0;
        sCurrentSong = 0;
    }
}

void tunes_update_crowd_queue(void)
{
    StreamChannel *obj; StreamSample *e;
    if (sCrowdQueueCount != 0) {
        if (--sCrowdQueue[1] <= 0) {
            obj = sStreamChannels;
            e = &sStreamSampleTable[sCrowdQueue[0]];
            obj[3].sample = sCrowdQueue[0];
            obj[3].echo = 0;
            obj[3].loop = 0;
            obj[3].block = 0;
            obj[3].data = (u8 *)(e->offset + sStreamDataBase);
            obj[3].size = e->size;
            obj[3].flags = e->flags;
            obj[3].volume = (gAmbientVolume_2[0] * gSndMasterVolume_2[0]) >> 8;
            sCrowdQueueCount--;
            sCrowdQueue[0] = sCrowdQueue[2];
            sCrowdQueue[1] = sCrowdQueue[3];
        }
    }
}

void tunes_set_ambient(s32 param_1, s32 param_2, s32 arg2)
{
    if (sAmbientLocked != 0) {
        return;
    }
    if (param_2 == 0) {
        if (sStreamChannels[3].sample != -1) {
            sStreamChannels[3].sample = -1;
            sStreamChannels[3].loop = 0;
            gAmbientVolume = 0;
        }
        return;
    }
    if (sStreamChannels[3].sample != param_1) {
        StreamSample *t;
        s32 v10;
        sStreamChannels[3].sample = param_1;
        t = &sStreamSampleTable[param_1];
        sStreamChannels[3].block = 0;
        sStreamChannels[3].data = (u8 *)(t->offset + sStreamDataBase);
        sStreamChannels[3].size = t->size;
        v10 = t->flags;
        sStreamChannels[3].loop = 1;
        sStreamChannels[3].flags = v10;
    }
    gAmbientVolume = param_2;
}

typedef struct { u8 pad[0x662]; s16 f; u8 rest[0x7E0 - 0x664]; } Tbl7E0;

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
    StreamChannel *p;
    StreamSample *r;

    if (sSpeechQueueCount_2[0] == 0) {
        return;
    }
    if (sStreamChannels[2].sample != -1 && sSpeechPriority >= 0) {
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
            lo = gEventNumRiders - 2;
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
        if (sStreamChannels[2].sample != -1 && sSpeechPriority >= 0) {
            f = 1;
        } else {
            sStreamChannels[2].sample = v;
            r = &sStreamSampleTable[v];
            sStreamChannels[2].echo = 0;
            sStreamChannels[2].loop = 0;
            sStreamChannels[2].block = 0;
            sStreamChannels[2].data = (u8 *)(r->offset + sStreamDataBase);
            sSpeechPriority = 0;
            sStreamChannels[2].size = r->size;
            sSpeechUseSfxVolume = 0;
            sStreamChannels[2].flags = r->flags;
            f = 0;
        }
    }
    if (f == 0) {
        p = sStreamChannels;
        p[2].echo = 1;
        e = gSpeechVolume;
        c = --sSpeechQueueCount_2[0];
        p[2].volume = e;
        if (c != 0) {
            memcpy(sSpeechQueue, sSpeechQueue + 1, c << 2);
        }
        gLastSpeechTime = gRaceFrame;
    }
}

typedef struct { u32 pad[7]; u32 map[12]; u32 mode; } Ctl_9;

extern Ctl_9 gGame_33;

static inline s32 check_2(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot_2) return 1;
    if (gGame_33.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_33.map[*p];
    if (*p >= 6) return 1;
    return (gGame_33.mode == 9) * 2;
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
    if (param_2 == D_800F05C8 - 2) {
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
typedef struct { u32 pad[7]; u32 map[8]; u32 f3C; u32 f40; u32 f44; u32 f48; u32 mode; } Ctl_10;

extern Plr_6 gRiders_53[];
extern Ctl_10 gGame_34;

static inline s32 check_3(u32 *p)
{
    if (gGhostRace != 0 && *p == gGhostSlot) return 1;
    if (gGame_34.mode == 2) return 1;
    if (*p >= 6) return 1;
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_34.map[*p];
    if (*p >= 6) return 1;
    return (gGame_34.mode == 9) * 2;
}

void announce_place(u32 a, u32 b)
{
    if (gEventTime < 0x168) return;
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
    } else if (b == gGame_34.f3C + gGame_34.f44) {
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
    if (gGame_7.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_7.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame_7.mode == 9) << 1;
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
    if (gGame_7.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_7.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame_7.mode == 9) << 1;
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

typedef struct {
    s32 pad[7];
    u32 map[12];
    s32 mode;
} Cfg_7;

extern Cfg_7 gGame_35;

static inline s32 check_4(u32 *p)
{
    if (gGame_35.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    *p = gGame_35.map[*p];
    if (*p >= 6) {
        return 1;
    }
    return (gGame_35.mode == 9) * 2;
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
    if (gEventTime < 0x168) {
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
    if (gGame_3.mode == 2) {
        return 1;
    }
    if (*q >= 6) {
        return 1;
    }
    gAnnounceTimes[*q] = gRaceFrame;
    *q = gGame_3.tab[*q];
    if (*q >= 6) {
        return 1;
    }
    return (gGame_3.mode == 9) * 2;
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
    StreamChannel *b;
    StreamSample *s;

    if (chk(&param_1) != 0) {
        return;
    }
    if (param_1 == 0 || param_1 == 3) {
        return;
    }
    b = sStreamChannels;
    b[3].sample = 0x73;
    b[3].block = 0;
    s = sStreamSampleTable;
    b[3].data = (u8 *)(s[0x73].offset + sStreamDataBase);
    b[3].size = s[0x73].size;
    sAmbientLocked = 1;
    b[3].flags = s[0x73].flags;
    b[3].loop = 0;
    gAmbientVolume_2[0] = gSfxVolume;
}

s32 tunes_is_song_playing(void) {
    return sCurrentSong != 0;
}

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
            mem_free(sStreamChannels[i].dmaBuf);
            mem_free(sStreamChannels[i].pcm);
            i++;
        } while (i < sStreamChannelCount);
    }
    mem_free(sStreamChannels);
    sTunesActive = 0;
}

int tunes_play_speech(s32 param_1, s32 param_2, s32 param_3)
{
    StreamSample *iVar3;

    if (gPodiumMode != 0) {
        return gPodiumMode;
    }
    if (sStreamChannels[2].sample != -1 && param_2 <= sSpeechPriority) {
        return 1;
    }
    iVar3 = &sStreamSampleTable[param_1];
    sStreamChannels[2].echo = 0;
    sStreamChannels[2].loop = 0;
    sStreamChannels[2].sample = param_1;
    sStreamChannels[2].block = 0;
    sStreamChannels[2].data = (u8 *)(iVar3->offset + sStreamDataBase);
    sSpeechPriority = param_2;
    sStreamChannels[2].size = iVar3->size;
    sSpeechUseSfxVolume = param_3;
    sStreamChannels[2].flags = iVar3->flags;
    return 0;
}

s32 tunes_get_speech_queue_time(void)
{
    s32 i, sum, w, f;

    sum = 0;
    for (i = 0; i < sSpeechQueueCount; i++) {
        w = ((StreamSample *)((u8 *)sStreamSampleTable + ((sSpeechQueue[i] * 5) << 2)))->size;
        f = w * 2;
        /* the flags of clip `f`, not of the queued clip */
        if (((StreamSample *)((u8 *)sStreamSampleTable + ((f * 5) << 2)))->flags & 1) {
            f = w << 2;
        }
        sum += (f * 0x3c) / 0x5622;
    }
    return sum;
}

u32 tunes_find_sample(u32 key)
{
    StreamSample *p;
    u32 i = 0;
    u32 n;

    p = sStreamSampleTable;
    if (p->count != 0) {
        n = p->count;
        do {
            if (p[1].count == key) {    /* clip i's tag */
                return i;
            }
            i++;
            p++;
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

void tunes_stop_speech(void)
{
    StreamChannel *o;
    u8 *p;

    o = sStreamChannels;
    p = o[2].dmaBuf;
    o[2].sample = -1;
    o[2].block = -1;
    o[2].bufSample = -1;
    o[2].bufBlock = -1;
    o[2].dmaSample = -1;
    o[2].dmaBlock = -1;
    mem_fill(p, 0, 0x220);
    mem_fill(sStreamChannels[2].pcm, 0, 0x800);
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
            *p = *p + sStreamMixBuf[sStreamMixReadPos];
            p++;
            *p = *p + sStreamMixBuf[sStreamMixReadPos];
            p++;
            sStreamMixReadPos = (sStreamMixReadPos + 1) & 0x7FF;
            n--;
        }
    }
}

void tunes_init_channel(StreamChannel *p) {
    if (p->dmaBuf == 0) {
        p->dmaBuf = mem_alloc_dbg(0x220, 0, (s32)&D_800D7338, 0x128);
        p->pcm = mem_alloc_dbg(0x800, 0, (s32)&D_800D7338, 0x129);
        p->unk_00 = 0;
        p->dmaHandle = -1;
        p->sample = -1;
        p->bufSample = -1;
    }
}

void tunes_free_channel(StreamChannel *param_1)
{
    if (param_1->dmaBuf != 0) {
        mem_free(param_1->dmaBuf);
        mem_free(param_1->pcm);
        mem_fill(param_1, 0, sizeof(StreamChannel));
        param_1->dmaHandle = -1;
        param_1->sample = -1;
        param_1->bufSample = -1;
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
        v = *p + ((s16)sStreamEchoBuf[sStreamEchoPos] >> 1);
        if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        if (v < -0x7FFF) {
            v = -0x7FFF;
        }
        sStreamEchoBuf[sStreamEchoPos] = v;
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

void tunes_update_volumes(void)
{
    s32 k = 0;

    if (sTunesActive != 0) {
        s32 *p = gStreamVolumes_2;
        s32 a2 = gSndMasterVolume;
        StreamChannel *e = sStreamChannels;
        do {
            s32 v = *p;
            p++;
            k++;
            e->volume = (v * a2) >> 8;
            e++;
        } while (k < 4);
    }
}

s32 announce_check_rider(u32 *p)
{
    u32 *base;
    u32 u;
    if (gGhostRace != 0) {
        if (*p == gGhostSlot) {
            return 1;
        }
    }
    base = gGame_5;
    if (base[19] == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    gAnnounceTimes[*p] = gRaceFrame;
    u = base[*p + 7];
    *p = u;
    if (u < 6) {
        return (base[19] == 9) << 1;
    }
    return 1;
}
