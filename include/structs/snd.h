/* The game's sound glue (unnamed/s0_8000F060.c, snd_*): engine and rider sounds over the sfx layer. */
#ifndef STRUCTS_SND_H
#define STRUCTS_SND_H

#include "common.h"

/* Rates passed to sfx_play, and the centre of its pans (0-15). */
#define SND_RATE_FULL           0x5622  /* 22050 Hz */
#define SND_RATE_HALF           0x2B11  /* 11025 Hz */
#define SND_PAN_CENTRE          7

/* Rider sound effects (sRiderSfx), after their resources. */
#define RIDER_SFX_LANDING       0
#define RIDER_SFX_SKID          1       /* the rear wheel slips */
#define RIDER_SFX_OVERHEAT      2       /* the engine heat warning */
#define RIDER_SFX_WATER_SPLASH  3
#define RIDER_SFX_CRASH_HIT     4
#define RIDER_SFX_SCRAPE        5       /* after snd_trigger_impact: a hit that throws sparks */
#define RIDER_SFX_WATER_PLUNGE  6
#define RIDER_SFX_CRASH_HIT_2   8
#define RIDER_SFX_REDUCED       8       /* the reduced set (sSndReducedSet) loads only the effects below this */

/* Engine samples (sEngineSamples): 0-4 the gears, then the idle and the loop a gear sample continues with. */
#define ENGINE_SAMPLES          7
#define ENGINE_TOP_GEAR         4
#define ENGINE_SAMPLE_IDLE      5
#define ENGINE_SAMPLE_LOOP      6

/* sRiderSounds: one per entry of gRiders. */
#define SND_RIDERS              8

/* RiderSound.gear: start the gear sample again. */
#define SND_GEAR_RESTART        (-999)

/* For the Doppler shift, in m/s. */
#define SND_SPEED_OF_SOUND      331.45f

/* A sound effect loaded for the race (sRiderSfx, up to the entry with a NULL resId). */
typedef struct SndSample {
    /* 0x00 */ s32 *resId;          /* the "SAM" resource's id */
    /* 0x04 */ s32 sample;          /* sfx sample, -1: not loaded */
} SndSample; /* size 0x8 */

/* An engine sample (sEngineSamples, ENGINE_SAMPLES). */
typedef struct EngineSample {
    /* 0x00 */ s32 *resId;          /* NULL: the previous entry's sample */
    /* 0x04 */ s32 sample;
    /* 0x08 */ s32 length;
    /* 0x0C */ u8 *data;
    /* 0x10 */ s32 rateLow;         /* playback rates at 5000 and 12000 rpm (the idle: 1000 and 8000) */
    /* 0x14 */ s32 rateHigh;
} EngineSample; /* size 0x18 */

/* The sounds of a rider (sRiderSounds, SND_RIDERS). The *Handle fields are sfx handles, -1: not playing. */
typedef struct RiderSound {
    /* 0x00 */ s32 gearHold;        /* frames a gear sample plays before it starts again */
    /* 0x04 */ s32 idleHandle;      /* ENGINE_SAMPLE_IDLE, while the throttle is off */
    /* 0x08 */ s32 gear;            /* last gear, SND_GEAR_RESTART: start its sample again */
    /* 0x0C */ s32 engineHandle;
    /* 0x10 */ s32 idleFade;        /* 0-6: frames since the throttle went off */
    /* 0x14 */ s32 revBlip;         /* idle volume added after a downshift */
    /* 0x18 */ s32 revBlipStep;
    /* 0x1C */ s32 gearFrames;      /* frames in the same gear */
    /* 0x20 */ s32 engineSample;    /* the gear sample playing */
    /* 0x24 */ s32 looped;          /* engineHandle went on with ENGINE_SAMPLE_LOOP (snd_engine_callback) */
    /* 0x28 */ s32 volume;          /* 0-255, from the distance to the nearest camera */
    /* 0x2C */ s32 view;            /* that camera */
    /* 0x30 */ s32 distance;
    /* 0x34 */ s32 distanceDelta;   /* change since the last frame, for the Doppler shift */
    /* 0x38 */ s32 impactFrame;     /* gRaceFrame of the last impact (snd_trigger_impact) */
    /* 0x3C */ s32 sfxFrame;        /* gRaceFrame of the last crash hit (snd_play_rider_sfx) */
    /* 0x40 */ s32 impactLevel;
    /* 0x44 */ s32 skidHandle;      /* RIDER_SFX_SKID */
    /* 0x48 */ s32 surfaceHandle;   /* the ground's sound */
    /* 0x4C */ s32 overheatHandle;  /* RIDER_SFX_OVERHEAT, above 10000 heat */
    /* 0x50 */ s32 impactHandle;    /* RIDER_SFX_SCRAPE */
    /* 0x54 */ s32 hasVoice;        /* among the riders whose engine is heard */
    /* 0x58 */ s32 throttleFrames;  /* counts down from 10 after the throttle */
    /* 0x5C */ s32 turboFrames;     /* counts down from 20 after the turbo button */
    /* 0x60 */ s32 turboCount;
} RiderSound; /* size 0x64 */

#endif
