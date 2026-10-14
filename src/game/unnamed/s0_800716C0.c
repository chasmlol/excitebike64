#include "game.h"

typedef struct { s32 x; s32 y; s32 d; s32 id; } Rec_44;
typedef struct { s32 n; Rec_44 r[1]; } List_8;

extern List_8 *sAmbientZones;

void ambient_update(void)
{
    s32 i, best, bi, t, dx, dy, id;
    u32 r;

    if (sAmbientZones != 0 && sAmbientZones->n != 0 && gNumScreens == 1) {
        for (i = 0; i < sAmbientZones->n; i++) {
            dx = sAmbientZones->r[i].x - (gCamPos[0].x >> 4);
            dy = sAmbientZones->r[i].y - (gCamPos[0].z >> 4);
            r = isqrt_u32_2(dx * dx + dy * dy);
            t = 0x200 - (s32)(r << 9) / sAmbientZones->r[i].d;
            if (best < t || i == 0) {
                best = t;
                bi = i;
            }
        }
        best = (best * gSfxVolume) / 255;
        if (best > 255) best = 255;
        if (best < 0) best = 0;
        id = sAmbientZones->r[bi].id;
        if (id == -1) best = 0;
        if (gPauseMenu != 0) best = 0;
        if (gCurTrack_2 - gTracks == 0xd && id == 0x6c && D_800CCB20 != 0) {
            id = 0x72;
        }
        tunes_set_ambient(id, best, -1);
    }
}

void ambient_load(void)
{
    s32 i;
    u32 off;
    u32 r;
    sAmbientZones_2 = (s32 *)track_res_get(D_800D1524);
    if (sAmbientZones_2 != 0 && sAmbientZones_2[0] > 0) {
        i = 0;
        do {
            off = (u32)i << 4;
            r = tunes_find_sample(*(u32 *)((u8 *)sAmbientZones_2 + off + 0x10));
            i++;
            *(u32 *)((u8 *)sAmbientZones_2 + off + 0x10) = r;
        } while (i < sAmbientZones_2[0]);
    }
}

void ambient_clear(void)
{
    sAmbientZones = 0;
}
