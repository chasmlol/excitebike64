#include "game.h"

/* An ambient sound zone of the track's ambient resource (gGidAmbient): a looping stream clip heard around a point. */
typedef struct AmbientZone {
    /* 0x00 */ s32 x;               /* the centre, in camera units >> 4 */
    /* 0x04 */ s32 z;
    /* 0x08 */ s32 radius;          /* silent from here out */
    /* 0x0C */ s32 clip;            /* the clip's tag in the resource; ambient_load makes it a clip index, -1: none */
} AmbientZone; /* size 0x10 */

typedef struct AmbientZoneList {
    /* 0x00 */ s32 count;
    /* 0x04 */ AmbientZone zones[1];
} AmbientZoneList;

extern AmbientZoneList *sAmbientZones;

/* Plays the clip of the loudest zone around the first camera on the ambience channel (one screen only). */
void ambient_update(void)
{
    s32 i, volume, loudest, level, dx, dz, clip;
    u32 dist;

    if (sAmbientZones != 0 && sAmbientZones->count != 0 && gNumScreens == 1) {
        for (i = 0; i < sAmbientZones->count; i++) {
            dx = sAmbientZones->zones[i].x - (gCamPos[0].x >> 4);
            dz = sAmbientZones->zones[i].z - (gCamPos[0].z >> 4);
            dist = isqrt_u32_2(dx * dx + dz * dz);
            /* 0x200 at the centre, 0 at the radius */
            level = 0x200 - (s32)(dist << 9) / sAmbientZones->zones[i].radius;
            if (volume < level || i == 0) {
                volume = level;
                loudest = i;
            }
        }
        volume = (volume * gSfxVolume) / 255;
        if (volume > 255) volume = 255;
        if (volume < 0) volume = 0;
        clip = sAmbientZones->zones[loudest].clip;
        if (clip == -1) volume = 0;
        if (gPauseMenu != 0) volume = 0;
        /* the Gravel Pit's train sounds different while it moves */
        if (gCurTrack_2 - gTracks == TRACK_GRAVEL_PIT && clip == CLIP_TRAIN && gGravelPitMoverTimer != 0) {
            clip = CLIP_TRAIN_2;
        }
        tunes_set_ambient(clip, volume, -1);
    }
}

/* Loads the track's ambient zones and turns their clip tags into clip indices. */
void ambient_load(void)
{
    s32 i;
    u32 off;
    u32 clip;
    sAmbientZones_2 = (s32 *)track_res_get(gGidAmbient);
    if (sAmbientZones_2 != 0 && sAmbientZones_2[0] > 0) {
        i = 0;
        do {
            /* the clip of zone i: sAmbientZones->zones[i].clip */
            off = (u32)i << 4;
            clip = tunes_find_sample(*(u32 *)((u8 *)sAmbientZones_2 + off + 0x10));
            i++;
            *(u32 *)((u8 *)sAmbientZones_2 + off + 0x10) = clip;
        } while (i < sAmbientZones_2[0]);
    }
}

void ambient_clear(void)
{
    sAmbientZones = 0;
}
