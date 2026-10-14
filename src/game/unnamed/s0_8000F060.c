#include "game.h"

/* Starts the sound: the sfx layer on the N64 driver, its echo, the streams, and loads the menu sounds. */
void snd_init(void)
{
    s32 caps[4];

    gSfxResSet = 0;
    gSndMasterVolume = 0x100;
    softmix_set_decompressor(decoder_decode_sample);
    gSfxDriver = &gN64SndDriver;
    sfx_get_driver_caps(&caps[0], &caps[1], &caps[2], &caps[3]);
    {
        s32 voices, modes, rate, frameMs;
        voices = caps[0];
        modes = caps[1];
        rate = caps[2];
        frameMs = caps[3];
        sfx_init(voices, modes, rate, frameMs, (void *)sSndFrameSamples, (void *)sSndOutputRate);
    }
    if (gSfxResSet != 0) {
        sfx_echo_init(0x400);
        sfx_echo_set_level(0x258);
        gDmaIdleCallback = (u32)snd_service;
        gResLoadCallback = (u32)snd_service;
        sSndEnabled = 1;
        sSndServiceBusy = 0;
        tunes_init(STREAM_CHANNELS);
        /* the Z buffer serves as the samples' decompression buffer */
        softmix_set_decompress_buf(gZBuffer, FB_320X240_SIZE);
        snd_load_menu_sounds();
        softmix_set_decompress_buf(0, 0);
    }
}

/* Loads the engine samples once and notes their lengths and data. */
void snd_load_engine_samples(void)
{
    u32 i;
    s32 info[4];    /* length, loop start, loop end, rate */

    if (sEngineSamplesLoaded == 0) {
        softmix_set_decompress_buf(gZBuffer, FB_320X240_SIZE);
        for (i = 0; i < ENGINE_SAMPLES; i++) {
            if (sEngineSamples[i].resId != NULL) {
                sEngineSamples[i].sample = sfx_load_sample_id(*sEngineSamples[i].resId);
            } else {
                sEngineSamples[i].sample = sEngineSamples[i - 1].sample;
            }
            sfx_get_sample_info(sEngineSamples[i].sample, info);
            sEngineSamples[i].length = info[0];
            sEngineSamples[i].data = (u8 *)softmix_get_sample_data(sEngineSamples[i].sample);
        }
        softmix_set_decompress_buf(0, 0);
        sEngineSamplesLoaded = 1;
    }
}

/* Sets up the race's sounds: the engines, the riders' effects (fewer on some tracks without the Expansion Pak), the
   sounds of the ground surfaces, and starts the race's song. */
void snd_race_init(Surface *surfaces)
{
    s32 i;
    u32 track;
    u32 flags;
    s32 sfxId;
    volatile s32 pad[4];

    if (sSndEnabled == 0) {
        return;
    }
    if (gPodiumMode != 0) {
        return;
    }
    if (!(gGame.flags & GAME_FLAG_TRACK_INTRO)) {
        snd_load_engine_samples();
    }
    i = gCurTrack_2 - gTracks;
    if (i == TRACK_GRAVEL_PIT || i == TRACK_BLIZZARD_BLITZ || i == TRACK_CONSTRUCTION_YARD) {
        sSndReducedSet = 1;
    } else {
        sSndReducedSet = 0;
    }
    if (osMemSize > 0x400000) {     /* the Expansion Pak */
        sSndReducedSet = 0;
    }
    softmix_set_decompress_buf(gZBuffer, FB_320X240_SIZE);
    i = 0;
    sSndRaceLoaded = 1;
    mem_fill((u8 *)sRiderSounds, i, sizeof(sRiderSounds));
    for (i = 0; i < SND_RIDERS; i++) {
        sRiderSounds[i].impactFrame = -0x28;
        sRiderSounds[i].sfxFrame = -0x14;
        sRiderSounds[i].gearHold = 0x3C;
        sRiderSounds[i].engineHandle = -1;
        sRiderSounds[i].idleHandle = -1;
        sRiderSounds[i].gear = -1;
        sRiderSounds[i].idleFade = -1;
        sRiderSounds[i].skidHandle = -1;
        sRiderSounds[i].surfaceHandle = -1;
        sRiderSounds[i].overheatHandle = -1;
        sRiderSounds[i].impactHandle = -1;
    }
    for (i = 0; sRiderSfx[i].resId != 0; i++) {
        if (sSndReducedSet != 0 && i >= RIDER_SFX_REDUCED) {
            sRiderSfx[i].sample = -1;
        } else {
            sRiderSfx[i].sample = sfx_load_sample_id(*sRiderSfx[i].resId);
        }
    }
    sSndSurfaces = surfaces;
    for (i = 0; i < 16; i++) {
        sfxId = sSndSurfaces[i].sfxId;
        if (sfxId == 0) {
            sSndSurfaces[i].sfxSample = -1;
        } else {
            if (sfxId & 0x80000000) {   /* the address of the id */
                sfxId = *(s32 *)sfxId;
            }
            sSndSurfaces[i].sfxSample = sfx_load_sample_id(sfxId);
        }
    }
    if (tunes_is_song_playing() == 0) {
        sRaceTuneStarted = 1;
        if (gReplayPlayback != 0) {
            tunes_play_song(gReplaySongResId);
        } else {
            track = gCurTrack_2 - gTracks;
            flags = gGame.flags;
            /* gGame.flags: 1 a race run by the front end, 0x10 rider select, 0x40000 the podium */
            if ((flags & 0x91) == 1 || !(flags & 1)) {
                if (flags & 0x40000) {
                    tunes_play_song(gPodiumSongResId);
                } else if (flags & 0x1000000) {
                    tunes_play_song(gReplaySongResId);
                } else if (flags & 0x80) {
                    tunes_play_song(sFinishSongResId);
                } else if (flags & 1) {
                    tunes_play_song(sDemoSongResId);
                } else if (gGame.mode != GAME_MODE_TUTORIAL) {
                    if (track < TRACK_COUNT) {
                        tunes_play_song(*sTrackTunes[track]);
                    } else {
                        tunes_play_song(sRaceSong5ResId);
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

/* Ends the race's sounds: stops the song and every sound, frees the race's samples and lets the output fall
   silent. */
void snd_race_exit(void)
{
    s32 i;
    u32 j;
    RiderSound *rs;
    s32 sample;

    if (sSndRaceLoaded != 0) {
        sSndRaceLoaded = 0;
        if (sRaceTuneStarted != 0) {
            tunes_stop_song();
        }
        sfx_set_frame_callback(0);
        for (i = 0; i < SND_RIDERS; i++) {
            rs = &sRiderSounds[i];
            /* snd_stop_rider, inlined: does nothing, sSndRaceLoaded was just cleared */
            if (sSndRaceLoaded != 0) {
                snd_stop_handle(&rs->engineHandle);
                snd_stop_handle(&rs->idleHandle);
                snd_stop_handle(&rs->skidHandle);
                snd_stop_handle(&rs->impactHandle);
                snd_stop_handle(&rs->overheatHandle);
                snd_stop_handle(&rs->surfaceHandle);
            }
        }
        tunes_stop_speech();
        sfx_stop_all();
        sfx_update_voices();
        sfx_reap_sounds();
        if (sEngineSamplesLoaded != 0) {
            for (j = 0; j < ENGINE_SAMPLES; j++) {
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
            sample = sSndSurfaces[i].sfxSample;
            if (sample != -1) {
                sfx_free_sample(sample);
            }
        }
        n64snd_clear_output();
        while (gN64SndSilenceFrames != 0) {
            n64snd_update();
        }
    }
}

/* Each frame of a race: plays the crowd queue, sets each rider's volume from its distance to the nearest camera,
   gives voices to the loudest engines (players first) and updates every rider's sounds. */
void snd_update_riders(void)
{
    Vec *cams;
    Vec *cam;
    RiderSound *rs;
    Rider *rider;
    Rider *riders;
    s32 i, view, dist, n, k, j, best, bestVolume;
    f32 z, t, kk, d, nearest, ex, ey, ez, vol;

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
    riders = gRiders;
    cams = gCamPos;
    rs = sRiderSounds;
    rider = riders;
loop:
    if (rider->inactive != 0) {
        rs->distanceDelta = 0;
        rs->view = 0;
        rs->distance = 0;
        rs->volume = 0;
    } else {
        /* the nearest camera */
        nearest = 0.0f;
        n = 0;
        if (n < gNumViews) {
            do {
            do {
                cam = &cams[n];
                d = cam->x;
                d -= (f32)rider->drawPos.x;
                d *= d;
                d += ((f32)cam->y - (f32)rider->drawPos.y) * ((f32)cam->y - (f32)rider->drawPos.y);
                d += ((f32)cam->z - (f32)rider->drawPos.z) * ((f32)cam->z - (f32)rider->drawPos.z);
                if (d < nearest || n == 0) {
                    nearest = d;
                    view = n;
                }
                n++;
            } while (n < gNumViews);
            } while (0);
        }
        nearest = __builtin_sqrtf(nearest);
        /* Excite 3D's side view: the first rider is heard from a fixed distance */
        if (gCurTrack_7 - gTracks_3 == TRACK_EXCITE_3D && ((s32)rider ^ (s32)riders) == 0 && riders->camMode == 4) {
            nearest = 10000.0f;
        }
        /* volume: 255 up to 5000 away, falling to 0 at 30000 */
        vol = nearest;
        dist = (s32)vol;
        if (vol < 5000.0f) {
            vol = 5000.0f;
        }
        if (vol > 30000.0f) {
            vol = 30000.0f;
        }
        vol = 30000.0f - vol;
        vol = vol * 255.0f / 25000.0f;
        if (vol < 0.0f) vol = 0.0f;
        /* the stores to k are dead; they are kept as they compile */
        if (vol > 0.0f) k = 1; else k = 2;
        if (vol < 25000.0f) k = 1; else k = 2;
        if (vol > 25000.0f) k = 1; else k = 2;
        if (vol > 255.0f) vol = 255.0f;



        rs->volume = (s32)vol;
        if (rs->view == view) {
            rs->distanceDelta = rs->distance - dist;
        } else {
            rs->distanceDelta = 0;
        }
        rs->view = view;
        rs->distance = dist;
        rs->hasVoice = 0;
    }
    rider++;
    rs++;
    if ((s32)rider < (s32)(riders + SND_RIDERS)) {
        goto loop;
    }

    /* the engines heard: the loudest riders, players before CPU riders */
    n = sEngineVoiceCounts[gNumScreens - 1];
    for (k = 0; k < n; k++) {
        bestVolume = -1;
        best = 0;
        for (i = 0; i < SND_RIDERS; i++) {
            if (gRiders[i].inactive == 0 && sRiderSounds[i].hasVoice == 0) {
                if (bestVolume < sRiderSounds[i].volume ||
                    (gRiders[i].isCpu == 0 && gRiders[best].isCpu != 0)) {
                    bestVolume = sRiderSounds[i].volume;
                    best = i;
                }
            }
        }
        if (bestVolume == -1) {
            break;
        }
        sRiderSounds[best].hasVoice = 1;
    }
    for (i = 0; i < SND_RIDERS; i++) {
        if (sRiderSounds[i].hasVoice == 0) {
            sRiderSounds[i].volume = 0;
        }
        snd_update_rider(i);
    }
}

/* A rider's sounds for this frame: exhaust puffs, the landing, the overheat warning, the skid, the scrape, the
   engine (a gear sample, or the idle with the throttle off, pitched by rpm, turbo and Doppler shift) and the
   ground. Everything stops while the rider is too quiet. */
void snd_update_rider(s32 riderIdx)
{
    RiderSound *rs;
    s32 vol;
    s32 engineVol;
    s32 v;          /* a sound's volume, then the gear, then the speed */
    s32 k;          /* the scrape's volume, then the engine's rate */
    s32 gearSample;
    s32 gearVol;
    f32 doppler;
    f32 heat;
    s32 info[4];    /* length, loop start, loop end, rate */

    if (sSndEnabled != 0 && sSndRaceLoaded != 0 && sEngineSamplesLoaded != 0) {
        rs = &sRiderSounds[riderIdx];
        if (gRiders[riderIdx].engine.throttle != 0) {
            rs->throttleFrames = 10;
        }
        if (rs->throttleFrames != 0) {
            rs->throttleFrames--;
        }
        if (gRiders[riderIdx].input & INPUT_Z) {
            rs->turboFrames = 20;
            rs->turboCount++;
        }
        if (rs->turboFrames != 0) {
            if (--rs->turboFrames == 0) {
                rs->turboCount = 0;
            }
        }
        if (rs->throttleFrames != 0 && gPauseMenu == 0) {
            if (gRiders[riderIdx].engine.overheated != 0 && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(riderIdx);
            }
            if ((gRiders[riderIdx].input & INPUT_Z) && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(riderIdx);
            }
        }
        vol = (gSfxVolume * rs->volume) / 256;
        if (vol <= 32) {
            if (rs->engineHandle != -1) {
                sfx_stop(rs->engineHandle);
                rs->engineHandle = -1;
            }
            if (rs->idleHandle != -1) {
                sfx_stop(rs->idleHandle);
                rs->idleHandle = -1;
            }
            if (rs->skidHandle != -1) {
                sfx_stop(rs->skidHandle);
                rs->skidHandle = -1;
            }
            if (rs->impactHandle != -1) {
                sfx_stop(rs->impactHandle);
                rs->impactHandle = -1;
            }
            if (rs->overheatHandle != -1) {
                sfx_stop(rs->overheatHandle);
                rs->overheatHandle = -1;
            }
            if (rs->surfaceHandle != -1) {
                sfx_stop(rs->surfaceHandle);
                rs->surfaceHandle = -1;
            }
            if ((random_byte_2() & 0x3F) == 0 && gRiders[riderIdx].inactive == 0) {
                particles_start_exhaust_puff(riderIdx);
            }
            return;
        }

        /* the landing */
        if (gRiders[riderIdx].impactFlags & 1) {
            v = gRiders[riderIdx].landingImpact;
            v = (v * vol) >> 8;
            v = v * 2 / 3;
            gRiders[riderIdx].impactFlags &= 0xFE;
            if (v > 255) {
                v = 255;
            }
            sfx_play(sRiderSfx[RIDER_SFX_LANDING].sample, SND_RATE_FULL, v, SND_PAN_CENTRE, 0, 1);
        }

        /* Doppler shift, with one view: v the speed towards the camera (mm per frame to m/s), the rate factor
           1 / (1 - 2v / c), clamped to 0.8-1.2 */
        if (gNumViews == 1 && rs->view == 0) {
            doppler = (f32)rs->distanceDelta * 60.0f / 1000.0f;
            doppler = doppler / SND_SPEED_OF_SOUND;
            doppler = doppler * 2.0f;
            doppler = doppler - 1.0f;
            if (doppler < 0.1f && doppler > -0.1f) {
                doppler = -1.0f;
            }
            doppler = -1.0f / doppler;
            if (doppler > 1.2f) {
                doppler = 1.2f;
            }
            if (doppler < 0.8f) {
                doppler = 0.8f;
            }
        } else {
            doppler = 1.0f;
        }

        /* the overheat warning: a player's engine above 10000 heat */
        v = 0;
        if (gRiders[riderIdx].engine.overheated == 0) {
            heat = gRiders[riderIdx].engine.heat;
            if (heat > 10000.0f && gRiders[riderIdx].isCpu == 0
                && (gGame.mode != GAME_MODE_TIME_TRIAL || riderIdx != gGhostSlot)) {
                v = (s32)((heat - 10000.0f) * 70.0f / 2000.0f) + 64;
                v = (v * vol) >> 8;
                if (v < 0) {
                    v = 0;
                }
            }
        }
        if (v != 0) {
            if (rs->overheatHandle == -1) {
                rs->overheatHandle = sfx_play(sRiderSfx[RIDER_SFX_OVERHEAT].sample, 8000, v, SND_PAN_CENTRE, 1, 1);
            }
            if (rs->overheatHandle != -1) {
                sfx_set_volume(rs->overheatHandle, v);
            }
        } else if (rs->overheatHandle != -1) {
            sfx_stop(rs->overheatHandle);
            rs->overheatHandle = -1;
        }

        /* the skid: the rear wheel's slip */
        v = 0;
        if (gRiders[riderIdx].crashState == 0) {
            v = (s32)(gRiders[riderIdx].rearWheel.slip * 128.0f * 0.5f);
            if (v > 192) {
                v = 192;
            }
            v = (v * vol) >> 8;
            if (v < 0) {
                v = 0;
            }
        }
        if (v != 0) {
            if (rs->skidHandle == -1) {
                rs->skidHandle = sfx_play(sRiderSfx[RIDER_SFX_SKID].sample, SND_RATE_HALF, v, SND_PAN_CENTRE, 0, 1);
            } else {
                sfx_set_volume(rs->skidHandle, v);
            }
        } else if (rs->skidHandle != -1) {
            sfx_stop(rs->skidHandle);
            rs->skidHandle = -1;
        }

        /* the scrape, for 30 frames after snd_trigger_impact */
        if (gRaceFrame < rs->impactFrame) {
            rs->impactFrame = -30;
        }
        k = rs->impactLevel * vol / 255;
        k = k * 350 / 255;
        if (k < 32) {
            k = 0;
        }
        if (k > 255) {
            k = 255;
        }
        if (k != 0 && gRaceFrame - rs->impactFrame < 30) {
            if (rs->impactHandle == -1) {
                rs->impactHandle = sfx_play(sRiderSfx[RIDER_SFX_SCRAPE].sample, SND_RATE_HALF, k, SND_PAN_CENTRE, 0, 1);
                if (gRiders[riderIdx].crashState != 0) {
                    sfx_get_sample_info(sRiderSfx[RIDER_SFX_SCRAPE].sample, info);
                    sfx_set_position(rs->impactHandle, info[1]);
                }
            } else {
                sfx_set_volume(rs->impactHandle, k);
            }
        } else if (rs->impactHandle != -1) {
            sfx_stop(rs->impactHandle);
            rs->impactHandle = -1;
        }

        /* the engine */
        v = gRiders[riderIdx].engine.gear;
        if (v < 0) {
            v = 0;
        }
        if (v > ENGINE_TOP_GEAR) {
            v = ENGINE_TOP_GEAR;
        }
        if (sRiderSounds[riderIdx].throttleFrames != 0 && v == rs->gear) {
            rs->gearFrames++;
        } else {
            rs->gearFrames = 0;
        }
        if (rs->gearFrames == rs->gearHold) {
            rs->gear = SND_GEAR_RESTART;
        }
        engineVol = vol * 200 / 256;

        if (sRiderSounds[riderIdx].throttleFrames != 0) {
            /* throttle: the gear's sample, started again on a gear change and after gearHold frames */
            if (rs->idleHandle != -1) {
                sfx_stop(rs->idleHandle);
                rs->idleHandle = -1;
            }
            rs->idleFade = 0;
            if (v != rs->gear) {
                if (gPauseMenu == 0) {
                    particles_start_exhaust_puff(riderIdx);
                }
                if (rs->engineHandle != -1) {
                    sfx_stop(rs->engineHandle);
                    rs->engineHandle = -1;
                }
            }
            gearSample = v;
            if (rs->engineHandle == -1) {
                /* the top two gears use either sample */
                if (v == 3 || v == 4) {
                    gearSample = (random_byte_2() & 1) + 3;
                }
                if (rs->gearFrames >= rs->gearHold) {
                    rs->gearFrames = 0;
                    if (gearSample >= ENGINE_TOP_GEAR) {
                        rs->gearHold = random_byte_2() % 20 + 20;
                    } else {
                        rs->gearHold = random_byte_2() % 30 + 30;
                    }
                }
                rs->engineHandle = sfx_play(sEngineSamples[gearSample].sample, 0, 0xC0, SND_PAN_CENTRE, 0, 1);
                /* a downshift starts at the loop */
                if (v == rs->gear - 1 && rs->engineHandle != -1) {
                    sfx_get_sample_info(sEngineSamples[gearSample].sample, info);
                    sfx_set_position(rs->engineHandle, info[1]);
                }
                rs->engineSample = gearSample;
                rs->looped = 0;
                if (rs->engineHandle == -1) {
                    goto end;
                }
            }
            /* rate: rateLow at 5000 rpm to rateHigh at 12000, with some jitter */
            k = (s32)(gRiders[riderIdx].engine.rpm - 5000.0f);
            k = sEngineSamples[rs->engineSample].rateLow
                + (sEngineSamples[rs->engineSample].rateHigh - sEngineSamples[rs->engineSample].rateLow) * k / 7000;
            k += random_byte_2() * 500 / 256 - 250;
            gearVol = engineVol;
            if (rs->turboFrames != 0) {
                if (gRiders[riderIdx].isCpu == 0) {
                    k = 32000;
                }
                gearVol = engineVol * 2;
            } else {
                s32 m = 300;
                k = k * m / 256;
            }
            if (gRiders[riderIdx].engine.overheated != 0) {
                k /= 2;
                gearVol *= 2;
            }
            if (gearVol > 255) {
                gearVol = 255;
            }
            if (gearVol < 0) {
                gearVol = 0;
            }
            sfx_set_rate(rs->engineHandle, (s32)(k * doppler) * 16 / 22);
            sfx_set_volume(rs->engineHandle, gearVol);
        } else {
            /* throttle off: the gear sample fades out over 6 frames under the idle */
            if (++rs->idleFade >= 7) {
                rs->idleFade = 6;
                if (rs->engineHandle != -1) {
                    sfx_stop(rs->engineHandle);
                    rs->engineHandle = -1;
                }
            }
            k = rs->idleFade * 255 / 6;
            if (rs->engineHandle != -1) {
                sfx_set_volume(rs->engineHandle, (255 - k) * 192 / 256 * engineVol / 256);
            }
            if (rs->idleHandle == -1) {
                rs->idleHandle = sfx_play(sEngineSamples[ENGINE_SAMPLE_IDLE].sample, 0, 0xC0, SND_PAN_CENTRE, 0, 1);
            }
            /* a downshift blips the idle's volume */
            if (rs->revBlip != 0) {
                rs->revBlip += rs->revBlipStep;
                if (rs->revBlip < 0) {
                    rs->revBlip = 0;
                }
            }
            if (rs->revBlip > 64) {
                rs->revBlipStep = -2;
            }
            if (rs->gear == v + 1) {
                rs->revBlip = 1;
                rs->revBlipStep = 4;
            }
            if (rs->idleHandle != -1) {
                s32 t = k / 2 + rs->revBlip;
                if (t > 255) {
                    t = 255;
                }
                sfx_set_volume(rs->idleHandle, t * engineVol / 256);
                k = (s32)(gRiders[riderIdx].engine.rpm - 1000.0f);
                k = sEngineSamples[ENGINE_SAMPLE_IDLE].rateLow
                    + (sEngineSamples[ENGINE_SAMPLE_IDLE].rateHigh - sEngineSamples[ENGINE_SAMPLE_IDLE].rateLow) * k / 7000;
                sfx_set_rate(rs->idleHandle, (s32)(k * doppler) * 16 / 22);
            }
        }
end:
        rs->gear = v;

        /* the ground: louder with speed, while the rear wheel is down */
        v = (s32)(gRiders[riderIdx].speed * 255.0f / 15.0f);
        if (v > 255) {
            v = 255;
        }
        if (v < 0) {
            v = 0;
        }
        if (gRiders[riderIdx].rearWheel.surface->sfxId != 0 && v != 0 && gRiders[riderIdx].airFrames < 6) {
            if (rs->surfaceHandle == -1) {
                rs->surfaceHandle = sfx_play(gRiders[riderIdx].rearWheel.surface->sfxSample, SND_RATE_HALF,
                                             (vol * v) >> 8, SND_PAN_CENTRE, 0, 1);
            } else {
                sfx_set_volume(rs->surfaceHandle, (vol * v) >> 8);
            }
        } else if (rs->surfaceHandle != -1) {
            sfx_stop(rs->surfaceHandle);
            rs->surfaceHandle = -1;
        }
    }
}

/* Plays rider effect `sfx` at the rider's volume scaled by `vol` (0-255). Crash hits wait 20 frames between two;
   RIDER_SFX_CRASH_HIT_2 falls back to RIDER_SFX_CRASH_HIT when the reduced set left it out. */
void snd_play_rider_sfx(s32 sfx, s32 riderIdx, s32 vol)
{
    s32 info[4];    /* length, loop start, loop end, rate */
    SndSample *smp, *table;
    s32 level, product;

    if (sSndRaceLoaded != 0) {
        if (sfx == RIDER_SFX_CRASH_HIT_2 && sRiderSfx[RIDER_SFX_CRASH_HIT_2].sample == -1) {
            sfx = RIDER_SFX_CRASH_HIT;
        }
        if (sfx == RIDER_SFX_CRASH_HIT || sfx == RIDER_SFX_CRASH_HIT_2) {
            if (gRaceFrame - sRiderSounds[riderIdx].sfxFrame < 20) {
                return;
            }
            sRiderSounds[riderIdx].sfxFrame = gRaceFrame;
        }
        table = sRiderSfx;
        smp = &table[sfx];
        sfx_get_sample_info(smp->sample, info);
        level = gSfxVolume * sRiderSounds[riderIdx].volume / 256;
        product = level * vol;
        level = product / 255;
        if (level >= 32) {
            if (level > 255) {
                level = 255;
            }
            sfx_play(smp->sample, info[3], level, SND_PAN_CENTRE, 1, 0);
        }
    }
}

/* Outside a race: services the sound until no effect plays, then for two more audio buffers. Each loop is
   snd_service inlined; the dead stores to `echoOn` are kept as they compile. */
void snd_drain(void)
{
    s32 buffers;
    s32 echoOn;

    if (sSndRaceLoaded == 0) {
        while (gSfxActiveCount != 0) {
            if (sSndEnabled != 0 && sSndServiceBusy == 0) {
                echoOn = 0; echoOn = 2; echoOn = 3;
                tunes_update_volumes();
                softmix_set_master_volume(gSndMasterVolume);
                sSndServiceBusy = 1;
                n64snd_update();
                sfx_reap_sounds();
                dma_update();
                tunes_update();
                sSndServiceBusy = 0;
                if (sSndRaceLoaded == 0) goto z1;
                echoOn = 0;
                if (gPauseMenu != 0) goto c1;
                if (gNumScreens != 1) goto c1;
                echoOn = 1;
                if (gCamSegFlags[0] & 0x40) goto c1;
            z1:
                echoOn = 0;
            c1:
                sfx_echo_enable(echoOn);
            }
        }
        buffers = gN64SndBufferCount;
        {
        while (gN64SndBufferCount < buffers + 2) {
            if (sSndEnabled != 0 && sSndServiceBusy == 0) {
                echoOn = 0; echoOn = 2; echoOn = 3;
                tunes_update_volumes();
                softmix_set_master_volume(gSndMasterVolume);
                sSndServiceBusy = 1;
                n64snd_update();
                sfx_reap_sounds();
                dma_update();
                tunes_update();
                sSndServiceBusy = 0;
                if (sSndRaceLoaded == 0) goto z2;
                echoOn = 0;
                if (gPauseMenu != 0) goto c2;
                if (gNumScreens != 1) goto c2;
                echoOn = 1;
                if (gCamSegFlags[0] & 0x40) goto c2;
            z2:
                echoOn = 0;
            c2:
                sfx_echo_enable(echoOn);
            }
        }
        }
    }
}

/* Stops the streams and the sfx layer. */
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

/* The per-frame service of the sound system, while the NES emulator is not running. */
void snd_update(void)
{
    if (sSndEnabled != 0 && gNesActive == 0) {
        snd_service();
        sfx_reap_sounds();
    }
}

/* Plays rider effect `sfx` at the effects volume, wherever the riders are. */
void snd_play_sfx(s32 sfx)
{
    SndSample *smp;
    SndSample *table;
    s32 info[4];    /* length, loop start, loop end, rate */
    s32 volume;

    if (sSndRaceLoaded != 0) {
        table = sRiderSfx;
        smp = table + sfx;
        sfx_get_sample_info(smp->sample, info);
        volume = gSfxVolume;
        if (volume > 0xFF) {
            volume = 0xFF;
        }
        sfx_play(smp->sample, info[3], volume, SND_PAN_CENTRE, 1, 0);
    }
}

void snd_set_master_volume(u32 volume)
{
    gSndMasterVolume = volume;
    tunes_update_volumes();
}

s32 snd_get_master_volume(void)
{
    return gSndMasterVolume;
}

/* A rider hits something hard enough to throw sparks, with strength `level`: snd_update_rider plays
   RIDER_SFX_SCRAPE for 30 frames. */
void snd_trigger_impact(s32 riderIdx, s32 level)
{
    u8 *base;
    RiderSound *rec;
    s32 off;
    s32 frame;

    base = (u8 *)sRiderSounds;
    off = riderIdx * sizeof(RiderSound);
    frame = gRaceFrame;
    rec = (RiderSound *)(base + off);
    rec->impactLevel = level;
    rec->impactFrame = frame;
}

/* Sets a volume (SND_VOLUME_*) and applies it to the streams. */
void snd_set_volume(s32 kind, s32 volume)
{
    s32 *p;

    if (kind == SND_VOLUME_MUSIC) {
        goto music;
    }
    if (kind < 2) {
        if (kind == SND_VOLUME_SFX) {
            goto sfx;
        }
        goto end;
    }
    if (kind == SND_VOLUME_SPEECH) {
        goto speech;
    }
    goto end;
sfx:
    gSfxVolume = volume;
    gAmbientVolume = volume;
    goto end;
music:
    gStreamVolumes = volume;
    p = &gStreamVolumes;
    p[1] = volume;
    goto end;
speech:
    gSpeechVolume = volume;
end:
    tunes_update_volumes();
}

/* The volume of a kind (SND_VOLUME_*). */
s32 snd_get_volume(s32 kind)
{
    switch (kind) {
    case SND_VOLUME_SFX:
        return gSfxVolume;
    case SND_VOLUME_MUSIC:
        return gStreamVolumes;
    case SND_VOLUME_SPEECH:
        return gSpeechVolume;
    default:
        return 0;
    }
}

/* snd_stop_rider for every rider. */
void snd_stop_all_riders(void)
{
    s32 i;
    RiderSound *rs;
    u8 *hb;

    i = 0;
    hb = (u8 *)0x800C0000;
    rs = sRiderSounds;
    do {
        if (*(s32 *)(hb + 0x2B64) != 0) {   /* sSndRaceLoaded */
            snd_stop_handle(&rs->engineHandle);
            snd_stop_handle(&rs->idleHandle);
            snd_stop_handle(&rs->skidHandle);
            snd_stop_handle(&rs->impactHandle);
            snd_stop_handle(&rs->overheatHandle);
            snd_stop_handle(&rs->surfaceHandle);
        }
        i++;
        rs++;
    } while (i < SND_RIDERS);
}

/* Loads the four menu sounds (MENU_SOUND_*). */
void snd_load_menu_sounds(void)
{
    sMenuSounds[MENU_SOUND_MOVE] = sfx_load_sample_id(sMenuMoveSfxResId);
    sMenuSounds[MENU_SOUND_SELECT] = sfx_load_sample_id(sMenuSelectSfxResId);
    sMenuSounds[MENU_SOUND_BACK] = sfx_load_sample_id(sMenuBackSfxResId);
    sMenuSounds[MENU_SOUND_ERROR] = sfx_load_sample_id(sMenuErrorSfxResId);
}

/* Plays menu sound `sound` (MENU_SOUND_*) at full volume. */
void snd_play_menu_sound(s32 sound)
{
    sfx_play(sMenuSounds[sound], SND_RATE_HALF, 0xFF, SND_PAN_CENTRE, 0, 0);
}

void snd_play_finish_song(void)
{
    tunes_stop_song();
    tunes_play_song(sFinishSongResId);
}

/* Stops the sound of handle *handle, if any, and clears it. */
void snd_stop_handle(s32 *handle)
{
    if (*handle != -1) {
        sfx_stop(*handle);
        *handle = -1;
    }
}

/* The sound's service, run each frame and while DMA or resource loading waits: volumes, the driver's mixing, the
   streams, and the echo, which is on in one-screen races while the camera is under cover. */
void snd_service(void)
{
    s32 echoOn;

    if (sSndEnabled != 0 && sSndServiceBusy == 0) {
        tunes_update_volumes();
        softmix_set_master_volume(gSndMasterVolume);
        sSndServiceBusy = 1;
        n64snd_update();
        sfx_reap_sounds();
        dma_update();
        tunes_update();
        sSndServiceBusy = 0;
        /* gCamSegFlags 0x40: a covered area */
        if (sSndRaceLoaded == 0 || gPauseMenu != 0 || gNumScreens != 1 || !(gCamSegFlags[0] & 0x40)) {
            echoOn = 0;
        } else {
            echoOn = 1;
        }
        sfx_echo_enable(echoOn);
    }
}

/* The sfx layer's frame callback: a gear sample other than the first (1-4) that played past its loop start goes on
   with the loop sample from the same point. */
void snd_engine_callback(void)
{
    s32 i;
    s32 info[4];    /* length, loop start, loop end, rate */
    u32 pos;

    if (sSndRaceLoaded != 0) {
        for (i = 0; i < SND_RIDERS; i++) {
            if (sRiderSounds[i].engineHandle != -1 && (u32)(sRiderSounds[i].engineSample - 1) < 4
                && sRiderSounds[i].looped == 0) {
                pos = sfx_get_position(sRiderSounds[i].engineHandle);
                sfx_get_sample_info(sEngineSamples[sRiderSounds[i].engineSample].sample, info);
                if ((s32)pos >= info[1]) {
                    sfx_set_sample(sRiderSounds[i].engineHandle, sEngineSamples[ENGINE_SAMPLE_LOOP].sample);
                    sfx_set_position(sRiderSounds[i].engineHandle, pos - info[1]);
                    sRiderSounds[i].looped = 1;
                }
            }
        }
    }
}

/* Frees the engine samples, if they are loaded. */
void snd_free_engine_samples(void)
{
    EngineSample *es;
    u32 i;

    if (sEngineSamplesLoaded != 0) {
        i = 0;
        es = sEngineSamples;
        do {
            s32 sample = es->sample;
            es++;
            i++;
            sfx_free_sample(sample);
        } while (i < ENGINE_SAMPLES);
        sEngineSamplesLoaded = 0;
    }
}

/* Stops every sound of a rider. */
void snd_stop_rider(s32 riderIdx)
{
    RiderSound *rs;

    if (sSndRaceLoaded != 0) {
        rs = &sRiderSounds[riderIdx];
        snd_stop_handle(&rs->engineHandle);
        snd_stop_handle(&rs->idleHandle);
        snd_stop_handle(&rs->skidHandle);
        snd_stop_handle(&rs->impactHandle);
        snd_stop_handle(&rs->overheatHandle);
        snd_stop_handle(&rs->surfaceHandle);
    }
}

/* snd_play_rider_sfx with the sample's rate scaled by `pitch` / 256. */
void snd_play_rider_sfx_pitch(s32 sfx, s32 riderIdx, s32 vol, s32 pitch, s32 unused)
{
    s32 info[4];    /* length, loop start, loop end, rate */
    s32 level;

    if (sSndRaceLoaded != 0) {
        SndSample *table = sRiderSfx;
        SndSample *smp = &table[sfx];
        sfx_get_sample_info(smp->sample, info);
        level = gSfxVolume * sRiderSounds[riderIdx].volume / 256;
        level = level * vol / 255;
        if (level >= 0x20) {
            if (level > 0xff) level = 0xff;
            sfx_play(smp->sample, info[3] * pitch / 256, level, SND_PAN_CENTRE, 1, 1);
        }
    }
}
