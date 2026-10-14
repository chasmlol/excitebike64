#include "game.h"


void snd_init(void)
{
    s32 l[4];

    gSfxResSet = 0;
    gSndMasterVolume = 0x100;
    softmix_set_decompressor(decoder_decode_sample);
    gSfxDriver = &gN64SndDriver;
    sfx_get_driver_caps(&l[0], &l[1], &l[2], &l[3]);
    {
        s32 p0, p1, p2, p3;
        p0 = l[0];
        p1 = l[1];
        p2 = l[2];
        p3 = l[3];
        sfx_init(p0, p1, p2, p3, (void *)sSndFrameSamples, (void *)sSndOutputRate);
    }
    if (gSfxResSet != 0) {
        sfx_echo_init(0x400);
        sfx_echo_set_level(0x258);
        gDmaIdleCallback = (u32)snd_service;
        gResLoadCallback = (u32)snd_service;
        sSndEnabled = 1;
        sSndServiceBusy = 0;
        tunes_init(4);
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        snd_load_menu_sounds();
        softmix_set_decompress_buf(0, 0);
    }
}

void snd_load_engine_samples(void) {
    u32 i;
    s32 local[4];

    if (sEngineSamplesLoaded == 0) {
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        for (i = 0; i < 7; i++) {
            if (sEngineSamples[i].resId != NULL) {
                sEngineSamples[i].sample = sfx_load_sample_id(*sEngineSamples[i].resId);
            } else {
                sEngineSamples[i].sample = sEngineSamples[i - 1].sample;
            }
            sfx_get_sample_info(sEngineSamples[i].sample, local);
            sEngineSamples[i].length = local[0];
            sEngineSamples[i].data = (u8 *)softmix_get_sample_data(sEngineSamples[i].sample);
        }
        softmix_set_decompress_buf(0, 0);
        sEngineSamplesLoaded = 1;
    }
}

void snd_race_init(Surface *surfaces)
{
    s32 i;
    u32 idx;
    u32 f;
    s32 a;
    volatile s32 pad[4];

    if (sSndEnabled == 0) {
        return;
    }
    if (gPodiumMode != 0) {
        return;
    }
    if (!(gGame.flags & 0x80)) {
        snd_load_engine_samples();
    }
    i = gCurTrack_2 - gTracks;
    if (i == 0xD || i == 0x13 || i == 0x11) {
        sSndReducedSet = 1;
    } else {
        sSndReducedSet = 0;
    }
    if (osMemSize > 0x400000) {
        sSndReducedSet = 0;
    }
    softmix_set_decompress_buf(gZBuffer, 0x25800);
    i = 0;
    sSndRaceLoaded = 1;
    mem_fill((u8 *)sRiderSounds, i, sizeof(sRiderSounds));
    for (i = 0; i < 8; i++) {
        sRiderSounds[i].impactFrame = -0x28;
        sRiderSounds[i].sfxFrame = -0x14;
        sRiderSounds[i].gearHold = 0x3C;
        sRiderSounds[i].engineHandle = -1;
        sRiderSounds[i].idleHandle = -1;
        sRiderSounds[i].gear = -1;
        sRiderSounds[i].idleFade = -1;
        sRiderSounds[i].sfx1Handle = -1;
        sRiderSounds[i].surfaceHandle = -1;
        sRiderSounds[i].revHandle = -1;
        sRiderSounds[i].impactHandle = -1;
    }
    for (i = 0; sRiderSfx[i].resId != 0; i++) {
        if (sSndReducedSet != 0 && i >= 8) {
            sRiderSfx[i].sample = -1;
        } else {
            sRiderSfx[i].sample = sfx_load_sample_id(*sRiderSfx[i].resId);
        }
    }
    sSndSurfaces = surfaces;
    for (i = 0; i < 16; i++) {
        a = sSndSurfaces[i].sfxId;
        if (a == 0) {
            sSndSurfaces[i].sfxSample = -1;
        } else {
            if (a & 0x80000000) {
                a = *(s32 *)a;
            }
            sSndSurfaces[i].sfxSample = sfx_load_sample_id(a);
        }
    }
    if (tunes_is_song_playing() == 0) {
        sRaceTuneStarted = 1;
        if (gReplayPlayback != 0) {
            tunes_play_song(D_800D11D8);
        } else {
            idx = gCurTrack_2 - gTracks;
            f = gGame.flags;
            if ((f & 0x91) == 1 || !(f & 1)) {
                if (f & 0x40000) {
                    tunes_play_song(D_800D11E4);
                } else if (f & 0x1000000) {
                    tunes_play_song(D_800D11D8);
                } else if (f & 0x80) {
                    tunes_play_song(D_800D11EC);
                } else if (f & 1) {
                    tunes_play_song(D_800D1214);
                } else if (gGame.mode != 9) {
                    if (idx < 0x1B) {
                        tunes_play_song(*sTrackTunes[idx]);
                    } else {
                        tunes_play_song(D_800D1224);
                    }
                }
            }
        }
    } else {
        sRaceTuneStarted = 0;
    }
    softmix_set_decompress_buf(0, 0);
    sfx_set_frame_callback(snd_engine_callback);
}

void snd_race_exit(void)
{
    s32 i;
    u32 j;
    RiderSound *p;
    s32 a;

    if (sSndRaceLoaded != 0) {
        sSndRaceLoaded = 0;
        if (sRaceTuneStarted != 0) {
            tunes_stop_song();
        }
        sfx_set_frame_callback(0);
        for (i = 0; i < 8; i++) {
            p = &sRiderSounds[i];
            if (sSndRaceLoaded != 0) {
                snd_stop_handle(&p->engineHandle);
                snd_stop_handle(&p->idleHandle);
                snd_stop_handle(&p->sfx1Handle);
                snd_stop_handle(&p->impactHandle);
                snd_stop_handle(&p->revHandle);
                snd_stop_handle(&p->surfaceHandle);
            }
        }
        tunes_stop_speech();
        sfx_stop_all();
        sfx_update_voices();
        sfx_reap_sounds();
        if (sEngineSamplesLoaded != 0) {
            for (j = 0; j < 7; j++) {
                sfx_free_sample(sEngineSamples[j].sample);
            }
            sEngineSamplesLoaded = 0;
        }
        if (sRiderSfx[0].resId != 0) {
            i = 0;
            do {
                if (sRiderSfx[i].sample != -1) {
                    sfx_free_sample(sRiderSfx[i].sample);
                }
                i++;
            } while (sRiderSfx[i].resId != 0);
        }
        for (i = 0; i < 16; i++) {
            a = sSndSurfaces[i].sfxSample;
            if (a != -1) {
                sfx_free_sample(a);
            }
        }
        n64snd_clear_output();
        while (gN64SndSilenceFrames != 0) {
            n64snd_update();
        }
    }
}

void snd_update_riders(void)
{
    Vec *pts;
    Vec *pt;
    RiderSound *sl;
    Rider *e;
    Rider *base;
    s32 i, bi, dist, n, k, j, best, bv;
    f32 z, t, kk, d, bd, ex, ey, ez, v;

    tunes_update_crowd_queue();
    if (sSndRaceLoaded == 0) {
        return;
    }
    if (gSfxVolume < 0) {
        gSfxVolume = 0;
    }
    if (gSfxVolume >= 0x100) {
        gSfxVolume = 0xFF;
    }
    base = gRiders;
    pts = gCamPos;
    sl = sRiderSounds;
    e = base;
loop:
    if (e->inactive != 0) {
        sl->distanceDelta = 0;
        sl->view = 0;
        sl->distance = 0;
        sl->volume = 0;
    } else {
        bd = 0.0f;
        n = 0;
        if (n < gNumViews) {
            do {
            do {
                pt = &pts[n];
                d = pt->x;
                d -= (f32)e->drawPos.x;
                d *= d;
                d += ((f32)pt->y - (f32)e->drawPos.y) * ((f32)pt->y - (f32)e->drawPos.y);
                d += ((f32)pt->z - (f32)e->drawPos.z) * ((f32)pt->z - (f32)e->drawPos.z);
                if (d < bd || n == 0) {
                    bd = d;
                    bi = n;
                }
                n++;
            } while (n < gNumViews);
            } while (0);
        }
        bd = __builtin_sqrtf(bd);
        if (gCurTrack_7 - gTracks_3 == 20 && ((s32)e ^ (s32)base) == 0 && base->camMode == 4) {
            bd = 10000.0f;
        }
        v = bd;
        dist = (s32)v;
        if (v < 5000.0f) {
            v = 5000.0f;
        }
        if (v > 30000.0f) {
            v = 30000.0f;
        }
        v = 30000.0f - v;
        v = v * 255.0f / 25000.0f;
        if (v < 0.0f) v = 0.0f;
        if (v > 0.0f) k = 1; else k = 2;
        if (v < 25000.0f) k = 1; else k = 2;
        if (v > 25000.0f) k = 1; else k = 2;
        if (v > 255.0f) v = 255.0f;



        sl->volume = (s32)v;
        if (sl->view == bi) {
            sl->distanceDelta = sl->distance - dist;
        } else {
            sl->distanceDelta = 0;
        }
        sl->view = bi;
        sl->distance = dist;
        sl->hasVoice = 0;
    }
    e++;
    sl++;
    if ((s32)e < (s32)(base + 8)) {
        goto loop;
    }

    n = sEngineVoiceCounts[gNumScreens - 1];
    for (k = 0; k < n; k++) {
        bv = -1;
        best = 0;
        for (i = 0; i < 8; i++) {
            if (gRiders[i].inactive == 0 && sRiderSounds[i].hasVoice == 0) {
                if (bv < sRiderSounds[i].volume ||
                    (gRiders[i].isCpu == 0 && gRiders[best].isCpu != 0)) {
                    bv = sRiderSounds[i].volume;
                    best = i;
                }
            }
        }
        if (bv == -1) {
            break;
        }
        sRiderSounds[best].hasVoice = 1;
    }
    for (i = 0; i < 8; i++) {
        if (sRiderSounds[i].hasVoice == 0) {
            sRiderSounds[i].volume = 0;
        }
        snd_update_rider(i);
    }
}

void snd_update_rider(s32 arg0) {
    RiderSound *s;
    s32 vol;
    s32 vol2;
    s32 v;
    s32 k;
    s32 g;
    s32 n;
    f32 f;
    f32 x;
    s32 buf[4];

    if (sSndEnabled != 0 && sSndRaceLoaded != 0 && sEngineSamplesLoaded != 0) {
        s = &sRiderSounds[arg0];
        if (gRiders[arg0].engine.throttle != 0) {
            s->throttleFrames = 10;
        }
        if (s->throttleFrames != 0) {
            s->throttleFrames--;
        }
        if (gRiders[arg0].input & 0x20000) {
            s->turboFrames = 20;
            s->turboCount++;
        }
        if (s->turboFrames != 0) {
            if (--s->turboFrames == 0) {
                s->turboCount = 0;
            }
        }
        if (s->throttleFrames != 0 && gPauseMenu == 0) {
            if (gRiders[arg0].engine.overheated != 0 && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(arg0);
            }
            if ((gRiders[arg0].input & 0x20000) && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(arg0);
            }
        }
        vol = (gSfxVolume * s->volume) / 256;
        if (vol <= 32) {
            if (s->engineHandle != -1) {
                sfx_stop(s->engineHandle);
                s->engineHandle = -1;
            }
            if (s->idleHandle != -1) {
                sfx_stop(s->idleHandle);
                s->idleHandle = -1;
            }
            if (s->sfx1Handle != -1) {
                sfx_stop(s->sfx1Handle);
                s->sfx1Handle = -1;
            }
            if (s->impactHandle != -1) {
                sfx_stop(s->impactHandle);
                s->impactHandle = -1;
            }
            if (s->revHandle != -1) {
                sfx_stop(s->revHandle);
                s->revHandle = -1;
            }
            if (s->surfaceHandle != -1) {
                sfx_stop(s->surfaceHandle);
                s->surfaceHandle = -1;
            }
            if ((random_byte_2() & 0x3F) == 0 && gRiders[arg0].inactive == 0) {
                particles_start_exhaust_puff(arg0);
            }
            return;
        }

        if (gRiders[arg0].impactFlags & 1) {
            v = gRiders[arg0].landingImpact;
            v = (v * vol) >> 8;
            v = v * 2 / 3;
            gRiders[arg0].impactFlags &= 0xFE;
            if (v > 255) {
                v = 255;
            }
            sfx_play(sRiderSfx[0].sample, 0x5622, v, 7, 0, 1);
        }

        if (gNumViews == 1 && s->view == 0) {
            f = (f32)s->distanceDelta * 60.0f / 1000.0f;
            f = f / 331.45f;
            f = f * 2.0f;
            f = f - 1.0f;
            if (f < 0.1f && f > -0.1f) {
                f = -1.0f;
            }
            f = -1.0f / f;
            if (f > 1.2f) {
                f = 1.2f;
            }
            if (f < 0.8f) {
                f = 0.8f;
            }
        } else {
            f = 1.0f;
        }

        v = 0;
        if (gRiders[arg0].engine.overheated == 0) {
            x = gRiders[arg0].engine.heat;
            if (x > 10000.0f && gRiders[arg0].isCpu == 0 && (gGame.mode != 8 || arg0 != gGhostSlot)) {
                v = (s32)((x - 10000.0f) * 70.0f / 2000.0f) + 64;
                v = (v * vol) >> 8;
                if (v < 0) {
                    v = 0;
                }
            }
        }
        if (v != 0) {
            if (s->revHandle == -1) {
                s->revHandle = sfx_play(sRiderSfx[2].sample, 8000, v, 7, 1, 1);
            }
            if (s->revHandle != -1) {
                sfx_set_volume(s->revHandle, v);
            }
        } else if (s->revHandle != -1) {
            sfx_stop(s->revHandle);
            s->revHandle = -1;
        }

        v = 0;
        if (gRiders[arg0].crashState == 0) {
            v = (s32)(gRiders[arg0].rearWheel.slip * 128.0f * 0.5f);
            if (v > 192) {
                v = 192;
            }
            v = (v * vol) >> 8;
            if (v < 0) {
                v = 0;
            }
        }
        if (v != 0) {
            if (s->sfx1Handle == -1) {
                s->sfx1Handle = sfx_play(sRiderSfx[1].sample, 0x2B11, v, 7, 0, 1);
            } else {
                sfx_set_volume(s->sfx1Handle, v);
            }
        } else if (s->sfx1Handle != -1) {
            sfx_stop(s->sfx1Handle);
            s->sfx1Handle = -1;
        }

        if (gRaceFrame < s->impactFrame) {
            s->impactFrame = -30;
        }
        k = s->impactLevel * vol / 255;
        k = k * 350 / 255;
        if (k < 32) {
            k = 0;
        }
        if (k > 255) {
            k = 255;
        }
        if (k != 0 && gRaceFrame - s->impactFrame < 30) {
            if (s->impactHandle == -1) {
                s->impactHandle = sfx_play(sRiderSfx[5].sample, 0x2B11, k, 7, 0, 1);
                if (gRiders[arg0].crashState != 0) {
                    sfx_get_sample_info(sRiderSfx[5].sample, buf);
                    sfx_set_position(s->impactHandle, buf[1]);
                }
            } else {
                sfx_set_volume(s->impactHandle, k);
            }
        } else if (s->impactHandle != -1) {
            sfx_stop(s->impactHandle);
            s->impactHandle = -1;
        }

        v = gRiders[arg0].engine.gear;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        if (sRiderSounds[arg0].throttleFrames != 0 && v == s->gear) {
            s->gearFrames++;
        } else {
            s->gearFrames = 0;
        }
        if (s->gearFrames == s->gearHold) {
            s->gear = -999;
        }
        vol2 = vol * 200 / 256;

        if (sRiderSounds[arg0].throttleFrames != 0) {
            if (s->idleHandle != -1) {
                sfx_stop(s->idleHandle);
                s->idleHandle = -1;
            }
            s->idleFade = 0;
            if (v != s->gear) {
                if (gPauseMenu == 0) {
                    particles_start_exhaust_puff(arg0);
                }
                if (s->engineHandle != -1) {
                    sfx_stop(s->engineHandle);
                    s->engineHandle = -1;
                }
            }
            g = v;
            if (s->engineHandle == -1) {
                if (v == 3 || v == 4) {
                    g = (random_byte_2() & 1) + 3;
                }
                if (s->gearFrames >= s->gearHold) {
                    s->gearFrames = 0;
                    if (g >= 4) {
                        s->gearHold = random_byte_2() % 20 + 20;
                    } else {
                        s->gearHold = random_byte_2() % 30 + 30;
                    }
                }
                s->engineHandle = sfx_play(sEngineSamples[g].sample, 0, 0xC0, 7, 0, 1);
                if (v == s->gear - 1 && s->engineHandle != -1) {
                    sfx_get_sample_info(sEngineSamples[g].sample, buf);
                    sfx_set_position(s->engineHandle, buf[1]);
                }
                s->engineSample = g;
                s->looped = 0;
                if (s->engineHandle == -1) {
                    goto end;
                }
            }
            k = (s32)(gRiders[arg0].engine.rpm - 5000.0f);
            k = sEngineSamples[s->engineSample].rateLow + (sEngineSamples[s->engineSample].rateHigh - sEngineSamples[s->engineSample].rateLow) * k / 7000;
            k += random_byte_2() * 500 / 256 - 250;
            n = vol2;
            if (s->turboFrames != 0) {
                if (gRiders[arg0].isCpu == 0) {
                    k = 32000;
                }
                n = vol2 * 2;
            } else {
                s32 m = 300;
                k = k * m / 256;
            }
            if (gRiders[arg0].engine.overheated != 0) {
                k /= 2;
                n *= 2;
            }
            if (n > 255) {
                n = 255;
            }
            if (n < 0) {
                n = 0;
            }
            sfx_set_rate(s->engineHandle, (s32)(k * f) * 16 / 22);
            sfx_set_volume(s->engineHandle, n);
        } else {
            if (++s->idleFade >= 7) {
                s->idleFade = 6;
                if (s->engineHandle != -1) {
                    sfx_stop(s->engineHandle);
                    s->engineHandle = -1;
                }
            }
            k = s->idleFade * 255 / 6;
            if (s->engineHandle != -1) {
                sfx_set_volume(s->engineHandle, (255 - k) * 192 / 256 * vol2 / 256);
            }
            if (s->idleHandle == -1) {
                s->idleHandle = sfx_play(sEngineSamples[5].sample, 0, 0xC0, 7, 0, 1);
            }
            if (s->revBlip != 0) {
                s->revBlip += s->revBlipStep;
                if (s->revBlip < 0) {
                    s->revBlip = 0;
                }
            }
            if (s->revBlip > 64) {
                s->revBlipStep = -2;
            }
            if (s->gear == v + 1) {
                s->revBlip = 1;
                s->revBlipStep = 4;
            }
            if (s->idleHandle != -1) {
                s32 t = k / 2 + s->revBlip;
                if (t > 255) {
                    t = 255;
                }
                sfx_set_volume(s->idleHandle, t * vol2 / 256);
                k = (s32)(gRiders[arg0].engine.rpm - 1000.0f);
                k = sEngineSamples[5].rateLow + (sEngineSamples[5].rateHigh - sEngineSamples[5].rateLow) * k / 7000;
                sfx_set_rate(s->idleHandle, (s32)(k * f) * 16 / 22);
            }
        }
end:
        s->gear = v;
        v = (s32)(gRiders[arg0].speed * 255.0f / 15.0f);
        if (v > 255) {
            v = 255;
        }
        if (v < 0) {
            v = 0;
        }
        if (gRiders[arg0].rearWheel.surface->sfxId != 0 && v != 0 && gRiders[arg0].airFrames < 6) {
            if (s->surfaceHandle == -1) {
                s->surfaceHandle = sfx_play(gRiders[arg0].rearWheel.surface->sfxSample, 0x2B11, (vol * v) >> 8, 7, 0, 1);
            } else {
                sfx_set_volume(s->surfaceHandle, (vol * v) >> 8);
            }
        } else if (s->surfaceHandle != -1) {
            sfx_stop(s->surfaceHandle);
            s->surfaceHandle = -1;
        }
    }
}

void snd_play_rider_sfx(s32 k, s32 n, s32 vol)
{
    s32 buf[4];
    SndSample *s, *tb;
    s32 t, x;

    if (sSndRaceLoaded != 0) {
        if (k == 8 && sRiderSfx[8].sample == -1) {
            k = 4;
        }
        if (k == 4 || k == 8) {
            if (gRaceFrame - sRiderSounds[n].sfxFrame < 20) {
                return;
            }
            sRiderSounds[n].sfxFrame = gRaceFrame;
        }
        tb = sRiderSfx;
        s = &tb[k];
        sfx_get_sample_info(s->sample, buf);
        t = gSfxVolume * sRiderSounds[n].volume / 256;
        x = t * vol;
        t = x / 255;
        if (t >= 32) {
            if (t > 255) {
                t = 255;
            }
            sfx_play(s->sample, buf[3], t, 7, 1, 0);
        }
    }
}

void snd_drain(void)
{
    s32 iVar1;
    s32 a;

    if (sSndRaceLoaded == 0) {
        while (gSfxActiveCount != 0) {
            if (sSndEnabled != 0 && sSndServiceBusy == 0) {
                a = 0; a = 2; a = 3;
                tunes_update_volumes();
                softmix_set_master_volume(gSndMasterVolume);
                sSndServiceBusy = 1;
                n64snd_update();
                sfx_reap_sounds();
                dma_update();
                tunes_update();
                sSndServiceBusy = 0;
                if (sSndRaceLoaded == 0) goto z1;
                a = 0;
                if (gPauseMenu != 0) goto c1;
                if (gNumScreens != 1) goto c1;
                a = 1;
                if (gCamSegFlags[0] & 0x40) goto c1;
            z1:
                a = 0;
            c1:
                sfx_echo_enable(a);
            }
        }
        iVar1 = gN64SndBufferCount;
        {
        while (gN64SndBufferCount < iVar1 + 2) {
            if (sSndEnabled != 0 && sSndServiceBusy == 0) {
                a = 0; a = 2; a = 3;
                tunes_update_volumes();
                softmix_set_master_volume(gSndMasterVolume);
                sSndServiceBusy = 1;
                n64snd_update();
                sfx_reap_sounds();
                dma_update();
                tunes_update();
                sSndServiceBusy = 0;
                if (sSndRaceLoaded == 0) goto z2;
                a = 0;
                if (gPauseMenu != 0) goto c2;
                if (gNumScreens != 1) goto c2;
                a = 1;
                if (gCamSegFlags[0] & 0x40) goto c2;
            z2:
                a = 0;
            c2:
                sfx_echo_enable(a);
            }
        }
        }
    }
}

void snd_shutdown(void)
{
    if (sSndEnabled != 0) {
        tunes_shutdown();
        gDmaIdleCallback = 0;
        sSndEnabled = 0;
        gResLoadCallback = 0;
        sfx_shutdown();
    }
}

void snd_update(void) {
    if (sSndEnabled != 0 && gNesActive == 0) {
        snd_service();
        sfx_reap_sounds();
    }
}

void snd_play_sfx(s32 a0) {
    SndSample *e;
    SndSample *b;
    s32 buf[4];
    s32 n;

    if (sSndRaceLoaded != 0) {
        b = sRiderSfx;
        e = b + a0;
        sfx_get_sample_info(e->sample, buf);
        n = gSfxVolume;
        if (n > 0xFF) {
            n = 0xFF;
        }
        sfx_play(e->sample, buf[3], n, 7, 1, 0);
    }
}

void snd_set_master_volume(u32 a) {
    gSndMasterVolume = a;
    tunes_update_volumes();
}

s32 snd_get_master_volume(void) {
    return gSndMasterVolume;
}

void snd_trigger_impact(s32 a0, s32 a1)
{
    u8 *base;
    RiderSound *rec;
    s32 off;
    s32 v;

    base = (u8 *)sRiderSounds;
    off = a0 * sizeof(RiderSound);
    v = gRaceFrame;
    rec = (RiderSound *)(base + off);
    rec->impactLevel = a1;
    rec->impactFrame = v;
}

void snd_set_volume(s32 mode, s32 v)
{
    s32 *p;

    if (mode == 1) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        goto end;
    }
    if (mode == 2) {
        goto m2;
    }
    goto end;
m0:
    gSfxVolume = v;
    gAmbientVolume = v;
    goto end;
m1:
    gStreamVolumes = v;
    p = &gStreamVolumes;
    p[1] = v;
    goto end;
m2:
    gSpeechVolume = v;
end:
    tunes_update_volumes();
}

s32 snd_get_volume(s32 param_1)
{
    switch (param_1) {
    case 0:
        return gSfxVolume;
    case 1:
        return gStreamVolumes;
    case 2:
        return gSpeechVolume;
    default:
        return 0;
    }
}

void snd_stop_all_riders(void) {
    s32 i;
    RiderSound *p;
    u8 *hb;

    i = 0;
    hb = (u8 *)0x800C0000;
    p = sRiderSounds;
    do {
        if (*(s32 *)(hb + 0x2B64) != 0) {   /* sSndRaceLoaded */
            snd_stop_handle(&p->engineHandle);
            snd_stop_handle(&p->idleHandle);
            snd_stop_handle(&p->sfx1Handle);
            snd_stop_handle(&p->impactHandle);
            snd_stop_handle(&p->revHandle);
            snd_stop_handle(&p->surfaceHandle);
        }
        i++;
        p++;
    } while (i < 8);
}

void snd_load_menu_sounds(void)
{
    sMenuSounds[0] = sfx_load_sample_id(D_800D11D0);
    sMenuSounds[1] = sfx_load_sample_id(D_800D1230);
    sMenuSounds[2] = sfx_load_sample_id(D_800D1240);
    sMenuSounds[3] = sfx_load_sample_id(D_800D11CC);
}

void snd_play_menu_sound(s32 param_1) {
    sfx_play(sMenuSounds[param_1], 0x2B11, 0xFF, 7, 0, 0);
}

void func_8001106C(void) {
    tunes_stop_song();
    tunes_play_song(D_800D11EC);
}

void snd_stop_handle(s32 *p) {
    if (*p != -1) {
        sfx_stop(*p);
        *p = -1;
    }
}

void snd_service(void)
{
    s32 flag;

    if (sSndEnabled != 0 && sSndServiceBusy == 0) {
        tunes_update_volumes();
        softmix_set_master_volume(gSndMasterVolume);
        sSndServiceBusy = 1;
        n64snd_update();
        sfx_reap_sounds();
        dma_update();
        tunes_update();
        sSndServiceBusy = 0;
        if (sSndRaceLoaded == 0 || gPauseMenu != 0 || gNumScreens != 1 || !(gCamSegFlags[0] & 0x40)) {
            flag = 0;
        } else {
            flag = 1;
        }
        sfx_echo_enable(flag);
    }
}

void snd_engine_callback(void)
{
    s32 i;
    s32 loc[4];
    u32 r;

    if (sSndRaceLoaded != 0) {
        for (i = 0; i < 8; i++) {
            if (sRiderSounds[i].engineHandle != -1 && (u32)(sRiderSounds[i].engineSample - 1) < 4 && sRiderSounds[i].looped == 0) {
                r = sfx_get_position(sRiderSounds[i].engineHandle);
                sfx_get_sample_info(sEngineSamples[sRiderSounds[i].engineSample].sample, loc);
                if ((s32)r >= loc[1]) {
                    sfx_set_sample(sRiderSounds[i].engineHandle, sEngineSamples[6].sample);
                    sfx_set_position(sRiderSounds[i].engineHandle, r - loc[1]);
                    sRiderSounds[i].looped = 1;
                }
            }
        }
    }
}

void snd_free_engine_samples(void)
{
    EngineSample *p;
    u32 i;

    if (sEngineSamplesLoaded != 0) {
        i = 0;
        p = sEngineSamples;
        do {
            s32 a = p->sample;
            p++;
            i++;
            sfx_free_sample(a);
        } while (i < 7);
        sEngineSamplesLoaded = 0;
    }
}

void snd_stop_rider(s32 a0)
{
    RiderSound *s;

    if (sSndRaceLoaded != 0) {
        s = &sRiderSounds[a0];
        snd_stop_handle(&s->engineHandle);
        snd_stop_handle(&s->idleHandle);
        snd_stop_handle(&s->sfx1Handle);
        snd_stop_handle(&s->impactHandle);
        snd_stop_handle(&s->revHandle);
        snd_stop_handle(&s->surfaceHandle);
    }
}

void snd_play_rider_sfx_pitch(s32 i, s32 j, s32 vol, s32 pan, s32 arg4)
{
    s32 buf[4];
    s32 v;

    if (sSndRaceLoaded != 0) {
        SndSample *tab = sRiderSfx;
        SndSample *s = &tab[i];
        sfx_get_sample_info(s->sample, buf);
        v = gSfxVolume * sRiderSounds[j].volume / 256;
        v = v * vol / 255;
        if (v >= 0x20) {
            if (v > 0xff) v = 0xff;
            sfx_play(s->sample, buf[3] * pan / 256, v, 7, 1, 1);
        }
    }
}
