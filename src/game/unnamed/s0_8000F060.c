#include "game.h"

typedef struct {
    s32 *p;
    s32 v;
} Pair_3;

typedef struct {
    s32 a;
    s32 id;
} Snd;

extern Pair_3 sRiderSfx[];

void snd_init(void)
{
    s32 l[4];

    gSfxResSet = 0;
    gSndMasterVolume = 0x100;
    softmix_set_decompressor((s32)D_800C04EC);
    gSfxDriver = (u32)gN64SndDriver;
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

typedef struct {
    s32 *p;
    s32 b;
    s32 c;
    s32 d;
    s32 pad[2];
} Ent_6;

extern Ent_6 sEngineSamples_2[];

void snd_load_engine_samples(void) {
    u32 i;
    s32 local[4];

    if (sEngineSamplesLoaded == 0) {
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        for (i = 0; i < 7; i++) {
            if (sEngineSamples_2[i].p != NULL) {
                sEngineSamples_2[i].b = sfx_load_sample_id(*sEngineSamples_2[i].p);
            } else {
                sEngineSamples_2[i].b = sEngineSamples_2[i - 1].b;
            }
            sfx_get_sample_info(sEngineSamples_2[i].b, local);
            sEngineSamples_2[i].c = local[0];
            sEngineSamples_2[i].d = softmix_get_sample_data(sEngineSamples_2[i].b);
        }
        softmix_set_decompress_buf(0, 0);
        sEngineSamplesLoaded = 1;
    }
}

typedef struct {
    s32 f0, f4, f8, fC, f10;
    s32 pad14[9];
    s32 f38, f3C;
    s32 pad40;
    s32 f44, f48, f4C, f50;
    s32 pad54[4];
} Ent_7;

struct Slot_2 {
    u8 pad[0x38];
    s32 a;
    s32 r;
};

extern Ent_7 sRiderSounds_2[];

void snd_race_init(Slot_2 *param_1)
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
    mem_fill((u8 *)sRiderSounds_2, i, 800);
    for (i = 0; i < 8; i++) {
        sRiderSounds_2[i].f38 = -0x28;
        sRiderSounds_2[i].f3C = -0x14;
        sRiderSounds_2[i].f0 = 0x3C;
        sRiderSounds_2[i].fC = -1;
        sRiderSounds_2[i].f4 = -1;
        sRiderSounds_2[i].f8 = -1;
        sRiderSounds_2[i].f10 = -1;
        sRiderSounds_2[i].f44 = -1;
        sRiderSounds_2[i].f48 = -1;
        sRiderSounds_2[i].f4C = -1;
        sRiderSounds_2[i].f50 = -1;
    }
    for (i = 0; sRiderSfx[i].p != 0; i++) {
        if (sSndReducedSet != 0 && i >= 8) {
            sRiderSfx[i].v = -1;
        } else {
            sRiderSfx[i].v = sfx_load_sample_id(*sRiderSfx[i].p);
        }
    }
    sSndSurfaces = param_1;
    for (i = 0; i < 16; i++) {
        a = sSndSurfaces[i].a;
        if (a == 0) {
            sSndSurfaces[i].r = -1;
        } else {
            if (a & 0x80000000) {
                a = *(s32 *)a;
            }
            sSndSurfaces[i].r = sfx_load_sample_id(a);
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
    sfx_set_frame_callback((s32)D_800111A0);
}

typedef struct {
    s32 a;
    s32 arg;
    s32 pad[4];
} Ent18;

typedef struct {
    s32 key;
    s32 arg;
} Ent8;

typedef struct {
    u8 pad[0x3C];
    s32 arg;
} Ent40;

extern Ent8 sRiderSfx_2[];
extern Ent18 sEngineSamples_3[];
extern Ent40 *sSndSurfaces_2;

void snd_race_exit(void)
{
    s32 i;
    u32 j;
    u8 *p;
    Ent8 *q;
    s32 a;

    if (sSndRaceLoaded != 0) {
        sSndRaceLoaded = 0;
        if (sRaceTuneStarted != 0) {
            tunes_stop_song();
        }
        sfx_set_frame_callback(0);
        for (i = 0; i < 8; i++) {
            p = sRiderSounds + i * 0x64;
            if (sSndRaceLoaded != 0) {
                snd_stop_handle((s32 *)(p + 0xC));
                snd_stop_handle((s32 *)(p + 0x4));
                snd_stop_handle((s32 *)(p + 0x44));
                snd_stop_handle((s32 *)(p + 0x50));
                snd_stop_handle((s32 *)(p + 0x4C));
                snd_stop_handle((s32 *)(p + 0x48));
            }
        }
        tunes_stop_speech();
        sfx_stop_all();
        sfx_update_voices();
        sfx_reap_sounds();
        if (sEngineSamplesLoaded != 0) {
            for (j = 0; j < 7; j++) {
                sfx_free_sample(sEngineSamples_3[j].arg);
            }
            sEngineSamplesLoaded = 0;
        }
        if (sRiderSfx_2[0].key != 0) {
            i = 0;
            do {
                if (sRiderSfx_2[i].arg != -1) {
                    sfx_free_sample(sRiderSfx_2[i].arg);
                }
                i++;
            } while (sRiderSfx_2[i].key != 0);
        }
        for (i = 0; i < 16; i++) {
            a = sSndSurfaces_2[i].arg;
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

typedef struct Ent_8 {
    s32 x, y, z;
    u8 pad0C[0x5AA - 0xC];
    s16 f5AA;
    u8 pad5AC[0x6E8 - 0x5AC];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Ent_8;

typedef struct Slot_3 {
    u8 pad00[0x28];
    s32 vol;
    s32 best;
    s32 dist;
    s32 delta;
    u8 pad38[0x54 - 0x38];
    s32 taken;
    u8 pad58[0x64 - 0x58];
} Slot_3;

extern Ent_8 gRiders_11[];
extern Slot_3 sRiderSounds_3[];

void snd_update_riders(void)
{
    Vec *pts;
    Vec *pt;
    Slot_3 *sl;
    Ent_8 *e;
    Ent_8 *base;
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
    base = gRiders_11;
    pts = gCamPos;
    sl = sRiderSounds_3;
    e = base;
loop:
    if (e->f7A5 != 0) {
        sl->delta = 0;
        sl->best = 0;
        sl->dist = 0;
        sl->vol = 0;
    } else {
        bd = 0.0f;
        n = 0;
        if (n < gNumViews) {
            do {
            do {
                pt = &pts[n];
                d = pt->x;
                d -= (f32)e->x;
                d *= d;
                d += ((f32)pt->y - (f32)e->y) * ((f32)pt->y - (f32)e->y);
                d += ((f32)pt->z - (f32)e->z) * ((f32)pt->z - (f32)e->z);
                if (d < bd || n == 0) {
                    bd = d;
                    bi = n;
                }
                n++;
            } while (n < gNumViews);
            } while (0);
        }
        bd = __builtin_sqrtf(bd);
        if (gCurTrack_7 - gTracks_3 == 20 && ((s32)e ^ (s32)base) == 0 && base->f5AA == 4) {
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



        sl->vol = (s32)v;
        if (sl->best == bi) {
            sl->delta = sl->dist - dist;
        } else {
            sl->delta = 0;
        }
        sl->best = bi;
        sl->dist = dist;
        sl->taken = 0;
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
            if (gRiders_11[i].f7A5 == 0 && sRiderSounds_3[i].taken == 0) {
                if (bv < sRiderSounds_3[i].vol ||
                    (gRiders_11[i].f6E8 == 0 && gRiders_11[best].f6E8 != 0)) {
                    bv = sRiderSounds_3[i].vol;
                    best = i;
                }
            }
        }
        if (bv == -1) {
            break;
        }
        sRiderSounds_3[best].taken = 1;
    }
    for (i = 0; i < 8; i++) {
        if (sRiderSounds_3[i].taken == 0) {
            sRiderSounds_3[i].vol = 0;
        }
        snd_update_rider(i);
    }
}

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    s32 f44;
    s32 f48;
    s32 f4C;
    s32 f50;
    s32 f54;
    s32 f58;
    s32 f5C;
    s32 f60;
} Snd_2;

typedef struct {
    u8 pad0[0x38];
    s32 f38;
    s32 f3C;
} Surf;

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x420 - 0x45];
    f32 f420;
    u8 pad424[0x4E4 - 0x424];
    Surf *f4E4;
    u8 pad4E8[0x4F0 - 0x4E8];
    f32 f4F0;
    u8 pad4F4[0x5B0 - 0x4F4];
    u32 f5B0;
    u8 pad5B4[0x644 - 0x5B4];
    f32 f644;
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 f655;
    u8 f656;
    u8 pad657[0x6E8 - 0x657];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x764 - 0x748];
    u8 f764;
    u8 pad765[3];
    s32 f768;
    u8 pad76C[0x7A5 - 0x76C];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_3;

extern Bike_3 gRiders_12[];
extern Snd_2 sRiderSounds_4[];

void snd_update_rider(s32 arg0) {
    Snd_2 *s;
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
        s = &sRiderSounds_4[arg0];
        if (gRiders_12[arg0].f656 != 0) {
            s->f58 = 10;
        }
        if (s->f58 != 0) {
            s->f58--;
        }
        if (gRiders_12[arg0].f5B0 & 0x20000) {
            s->f5C = 20;
            s->f60++;
        }
        if (s->f5C != 0) {
            if (--s->f5C == 0) {
                s->f60 = 0;
            }
        }
        if (s->f58 != 0 && gPauseMenu == 0) {
            if (gRiders_12[arg0].f654 != 0 && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(arg0);
            }
            if ((gRiders_12[arg0].f5B0 & 0x20000) && (random_byte_2() & 7) == 0) {
                particles_start_exhaust_puff(arg0);
            }
        }
        vol = (gSfxVolume * s->f28) / 256;
        if (vol <= 32) {
            if (s->fC != -1) {
                sfx_stop(s->fC);
                s->fC = -1;
            }
            if (s->f4 != -1) {
                sfx_stop(s->f4);
                s->f4 = -1;
            }
            if (s->f44 != -1) {
                sfx_stop(s->f44);
                s->f44 = -1;
            }
            if (s->f50 != -1) {
                sfx_stop(s->f50);
                s->f50 = -1;
            }
            if (s->f4C != -1) {
                sfx_stop(s->f4C);
                s->f4C = -1;
            }
            if (s->f48 != -1) {
                sfx_stop(s->f48);
                s->f48 = -1;
            }
            if ((random_byte_2() & 0x3F) == 0 && gRiders_12[arg0].f7A5 == 0) {
                particles_start_exhaust_puff(arg0);
            }
            return;
        }

        if (gRiders_12[arg0].f764 & 1) {
            v = gRiders_12[arg0].f768;
            v = (v * vol) >> 8;
            v = v * 2 / 3;
            gRiders_12[arg0].f764 &= 0xFE;
            if (v > 255) {
                v = 255;
            }
            sfx_play(sRiderSfx[0].v, 0x5622, v, 7, 0, 1);
        }

        if (gNumViews == 1 && s->f2C == 0) {
            f = (f32)s->f34 * 60.0f / 1000.0f;
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
        if (gRiders_12[arg0].f654 == 0) {
            x = gRiders_12[arg0].f648;
            if (x > 10000.0f && gRiders_12[arg0].f6E8 == 0 && (gGame.mode != 8 || arg0 != gGhostSlot)) {
                v = (s32)((x - 10000.0f) * 70.0f / 2000.0f) + 64;
                v = (v * vol) >> 8;
                if (v < 0) {
                    v = 0;
                }
            }
        }
        if (v != 0) {
            if (s->f4C == -1) {
                s->f4C = sfx_play(sRiderSfx[2].v, 8000, v, 7, 1, 1);
            }
            if (s->f4C != -1) {
                sfx_set_volume(s->f4C, v);
            }
        } else if (s->f4C != -1) {
            sfx_stop(s->f4C);
            s->f4C = -1;
        }

        v = 0;
        if (gRiders_12[arg0].f44 == 0) {
            v = (s32)(gRiders_12[arg0].f4F0 * 128.0f * 0.5f);
            if (v > 192) {
                v = 192;
            }
            v = (v * vol) >> 8;
            if (v < 0) {
                v = 0;
            }
        }
        if (v != 0) {
            if (s->f44 == -1) {
                s->f44 = sfx_play(sRiderSfx[1].v, 0x2B11, v, 7, 0, 1);
            } else {
                sfx_set_volume(s->f44, v);
            }
        } else if (s->f44 != -1) {
            sfx_stop(s->f44);
            s->f44 = -1;
        }

        if (gRaceFrame < s->f38) {
            s->f38 = -30;
        }
        k = s->f40 * vol / 255;
        k = k * 350 / 255;
        if (k < 32) {
            k = 0;
        }
        if (k > 255) {
            k = 255;
        }
        if (k != 0 && gRaceFrame - s->f38 < 30) {
            if (s->f50 == -1) {
                s->f50 = sfx_play(sRiderSfx[5].v, 0x2B11, k, 7, 0, 1);
                if (gRiders_12[arg0].f44 != 0) {
                    sfx_get_sample_info(sRiderSfx[5].v, buf);
                    sfx_set_position(s->f50, buf[1]);
                }
            } else {
                sfx_set_volume(s->f50, k);
            }
        } else if (s->f50 != -1) {
            sfx_stop(s->f50);
            s->f50 = -1;
        }

        v = gRiders_12[arg0].f655;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        if (sRiderSounds_4[arg0].f58 != 0 && v == s->f8) {
            s->f1C++;
        } else {
            s->f1C = 0;
        }
        if (s->f1C == s->f0) {
            s->f8 = -999;
        }
        vol2 = vol * 200 / 256;

        if (sRiderSounds_4[arg0].f58 != 0) {
            if (s->f4 != -1) {
                sfx_stop(s->f4);
                s->f4 = -1;
            }
            s->f10 = 0;
            if (v != s->f8) {
                if (gPauseMenu == 0) {
                    particles_start_exhaust_puff(arg0);
                }
                if (s->fC != -1) {
                    sfx_stop(s->fC);
                    s->fC = -1;
                }
            }
            g = v;
            if (s->fC == -1) {
                if (v == 3 || v == 4) {
                    g = (random_byte_2() & 1) + 3;
                }
                if (s->f1C >= s->f0) {
                    s->f1C = 0;
                    if (g >= 4) {
                        s->f0 = random_byte_2() % 20 + 20;
                    } else {
                        s->f0 = random_byte_2() % 30 + 30;
                    }
                }
                s->fC = sfx_play(sEngineSamples[g].f4, 0, 0xC0, 7, 0, 1);
                if (v == s->f8 - 1 && s->fC != -1) {
                    sfx_get_sample_info(sEngineSamples[g].f4, buf);
                    sfx_set_position(s->fC, buf[1]);
                }
                s->f20 = g;
                s->f24 = 0;
                if (s->fC == -1) {
                    goto end;
                }
            }
            k = (s32)(gRiders_12[arg0].f644 - 5000.0f);
            k = sEngineSamples[s->f20].f10 + (sEngineSamples[s->f20].f14 - sEngineSamples[s->f20].f10) * k / 7000;
            k += random_byte_2() * 500 / 256 - 250;
            n = vol2;
            if (s->f5C != 0) {
                if (gRiders_12[arg0].f6E8 == 0) {
                    k = 32000;
                }
                n = vol2 * 2;
            } else {
                s32 m = 300;
                k = k * m / 256;
            }
            if (gRiders_12[arg0].f654 != 0) {
                k /= 2;
                n *= 2;
            }
            if (n > 255) {
                n = 255;
            }
            if (n < 0) {
                n = 0;
            }
            sfx_set_rate(s->fC, (s32)(k * f) * 16 / 22);
            sfx_set_volume(s->fC, n);
        } else {
            if (++s->f10 >= 7) {
                s->f10 = 6;
                if (s->fC != -1) {
                    sfx_stop(s->fC);
                    s->fC = -1;
                }
            }
            k = s->f10 * 255 / 6;
            if (s->fC != -1) {
                sfx_set_volume(s->fC, (255 - k) * 192 / 256 * vol2 / 256);
            }
            if (s->f4 == -1) {
                s->f4 = sfx_play(sEngineSamples[5].f4, 0, 0xC0, 7, 0, 1);
            }
            if (s->f14 != 0) {
                s->f14 += s->f18;
                if (s->f14 < 0) {
                    s->f14 = 0;
                }
            }
            if (s->f14 > 64) {
                s->f18 = -2;
            }
            if (s->f8 == v + 1) {
                s->f14 = 1;
                s->f18 = 4;
            }
            if (s->f4 != -1) {
                s32 t = k / 2 + s->f14;
                if (t > 255) {
                    t = 255;
                }
                sfx_set_volume(s->f4, t * vol2 / 256);
                k = (s32)(gRiders_12[arg0].f644 - 1000.0f);
                k = sEngineSamples[5].f10 + (sEngineSamples[5].f14 - sEngineSamples[5].f10) * k / 7000;
                sfx_set_rate(s->f4, (s32)(k * f) * 16 / 22);
            }
        }
end:
        s->f8 = v;
        v = (s32)(gRiders_12[arg0].f420 * 255.0f / 15.0f);
        if (v > 255) {
            v = 255;
        }
        if (v < 0) {
            v = 0;
        }
        if (gRiders_12[arg0].f4E4->f38 != 0 && v != 0 && gRiders_12[arg0].f744 < 6) {
            if (s->f48 == -1) {
                s->f48 = sfx_play(gRiders_12[arg0].f4E4->f3C, 0x2B11, (vol * v) >> 8, 7, 0, 1);
            } else {
                sfx_set_volume(s->f48, (vol * v) >> 8);
            }
        } else if (s->f48 != -1) {
            sfx_stop(s->f48);
            s->f48 = -1;
        }
    }
}

typedef struct {
    s32 w0[10];
    s32 f28;
    s32 w1[4];
    s32 f3c;
    s32 w2[9];
} Rec25;

extern Rec25 sRiderSounds_5[];

void snd_play_rider_sfx(s32 k, s32 n, s32 vol)
{
    s32 buf[4];
    Snd *s, *tb;
    s32 t, x;

    if (sSndRaceLoaded != 0) {
        if (k == 8 && D_800C2C54 == -1) {
            k = 4;
        }
        if (k == 4 || k == 8) {
            if (gRaceFrame - sRiderSounds_5[n].f3c < 20) {
                return;
            }
            sRiderSounds_5[n].f3c = gRaceFrame;
        }
        tb = sRiderSfx;
        s = &tb[k];
        sfx_get_sample_info(s->id, buf);
        t = gSfxVolume * sRiderSounds_5[n].f28 / 256;
        x = t * vol;
        t = x / 255;
        if (t >= 32) {
            if (t > 255) {
                t = 255;
            }
            sfx_play(s->id, buf[3], t, 7, 1, 0);
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
    Pal *e;
    Pal *b;
    s32 buf[4];
    s32 n;

    if (sSndRaceLoaded != 0) {
        b = sRiderSfx;
        e = b + a0;
        sfx_get_sample_info(e->f4, buf);
        n = gSfxVolume;
        if (n > 0xFF) {
            n = 0xFF;
        }
        sfx_play(e->f4, buf[3], n, 7, 1, 0);
    }
}

void snd_set_master_volume(u32 a) {
    gSndMasterVolume = a;
    tunes_update_volumes();
}

s32 snd_get_master_volume(void) {
    return gSndMasterVolume;
}

void snd_trigger_impact(s32 a0, s32 a1, s32 a2, s32 a3)
{
    u8 *base;
    u8 *rec;
    s32 off;
    s32 v;

    base = sRiderSounds;
    off = a0 * 100;
    v = gRaceFrame;
    rec = base + off;
    *(s32 *)(rec + 0x40) = a1;
    *(s32 *)(rec + 0x38) = v;
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
    u8 *p;
    u8 *hb;

    i = 0;
    hb = (u8 *)0x800C0000;
    p = sRiderSounds;
    do {
        if (*(s32 *)(hb + 0x2B64) != 0) {
            snd_stop_handle((s32 *)(p + 0xC));
            snd_stop_handle((s32 *)(p + 0x4));
            snd_stop_handle((s32 *)(p + 0x44));
            snd_stop_handle((s32 *)(p + 0x50));
            snd_stop_handle((s32 *)(p + 0x4C));
            snd_stop_handle((s32 *)(p + 0x48));
        }
        i++;
        p += 0x64;
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

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
} T18;

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    u8 pad[0x64 - 0x28];
} Ent_9;

extern T18 sEngineSamples_4[];
extern Ent_9 sRiderSounds_6[];

void snd_engine_callback(void)
{
    s32 i;
    s32 loc[4];
    u32 r;

    if (sSndRaceLoaded != 0) {
        for (i = 0; i < 8; i++) {
            if (sRiderSounds_6[i].f0c != -1 && (u32)(sRiderSounds_6[i].f20 - 1) < 4 && sRiderSounds_6[i].f24 == 0) {
                r = sfx_get_position(sRiderSounds_6[i].f0c);
                sfx_get_sample_info(sEngineSamples_4[sRiderSounds_6[i].f20].f04, loc);
                if ((s32)r >= loc[1]) {
                    sfx_set_sample(sRiderSounds_6[i].f0c, sEngineSamples_4[6].f04);
                    sfx_set_position(sRiderSounds_6[i].f0c, r - loc[1]);
                    sRiderSounds_6[i].f24 = 1;
                }
            }
        }
    }
}

void snd_free_engine_samples(void)
{
    Gear *p;
    u32 i;

    if (sEngineSamplesLoaded != 0) {
        i = 0;
        p = sEngineSamples;
        do {
            s32 a = p->f4;
            p++;
            i++;
            sfx_free_sample(a);
        } while (i < 7);
        sEngineSamplesLoaded = 0;
    }
}

void snd_stop_rider(s32 a0)
{
    u8 *s;

    if (sSndRaceLoaded != 0) {
        s = sRiderSounds + a0 * 100;
        snd_stop_handle((s32 *)(s + 0xC));
        snd_stop_handle((s32 *)(s + 0x4));
        snd_stop_handle((s32 *)(s + 0x44));
        snd_stop_handle((s32 *)(s + 0x50));
        snd_stop_handle((s32 *)(s + 0x4C));
        snd_stop_handle((s32 *)(s + 0x48));
    }
}

typedef struct { u8 pad[0x28]; s32 vol; u8 pad2[0x38]; } Ent100;

extern Ent100 sRiderSounds_7[];

void snd_play_rider_sfx_pitch(s32 i, s32 j, s32 vol, s32 pan, s32 arg4)
{
    s32 buf[4];
    s32 v;

    if (sSndRaceLoaded != 0) {
        Snd *tab = sRiderSfx;
        Snd *s = &tab[i];
        sfx_get_sample_info(s->id, buf);
        v = gSfxVolume * sRiderSounds_7[j].vol / 256;
        v = v * vol / 255;
        if (v >= 0x20) {
            if (v > 0xff) v = 0xff;
            sfx_play(s->id, buf[3] * pan / 256, v, 7, 1, 1);
        }
    }
}
