/* Tire-track decals (decal.c). */
#ifndef STRUCTS_DECAL_H
#define STRUCTS_DECAL_H

#include "common.h"

#define DECAL_NUM_TRACKS 128

/* One quad of the ring of 128 tire tracks (sTireTracks), laid under a skidding bike. Its four vertices are
   sTireTrackVtx[i * 4 ..]: 0 and 1 the near edge, 2 and 3 the far edge, relative to the centre of the area
   in units of 16 mm. A rider's current quad (sTireTrackCur) is stretched while it keeps its direction. */
typedef struct TireTrack {
    /* 0x00 */ u8 active;
    /* 0x01 */ u8 surface; /* gSurfaceTable entry giving the colour */
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s32 area;   /* height area (sHeightAreas); desert: tile x << 16 | tile z */
    /* 0x08 */ s32 x;      /* centre of the far edge (mm) */
    /* 0x0C */ s32 z;
    /* 0x10 */ s32 dirX;   /* direction of the quad, 1.0 = 0x4000 */
    /* 0x14 */ s32 dirZ;
} TireTrack; /* size 0x18 */

#endif
