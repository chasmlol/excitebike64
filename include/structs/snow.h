/* Snowflakes (snow.c). */
#ifndef STRUCTS_SNOW_H
#define STRUCTS_SNOW_H

#include "common.h"

#define SNOW_NUM_FLAKES 128

/* One of the 128 snowflakes (sSnowflakes), in camera space: x right, y up, z ahead. A flake is drawn at
   x / z and y / z of the half screen and respawns ahead of the camera once it leaves the view. */
typedef struct Snowflake {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 z;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 fallSpeed; /* subtracted from y each frame */
    /* 0x18 */ s32 visible;   /* 0 when it respawned in a covered area (gCamSegFlags & CAM_SEG_COVERED) */
} Snowflake; /* size 0x1C */

#endif
