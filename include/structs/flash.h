/* Camera flashes in the stands (flash.c). */
#ifndef STRUCTS_FLASH_H
#define STRUCTS_FLASH_H

#include "common.h"

/* A quad of the stands where flashes go off: a flash spawns at v[0] + a * (v[1] - v[0]) + b * (v[3] - v[0]) with
   random a, b, while its track section is visible. */
typedef struct FlashQuad {
    /* 0x00 */ u16 section; /* the nearest track section, set by flash_init */
    /* 0x02 */ s16 v[4][3]; /* x, y, z */
} FlashQuad; /* size 0x1A */

/* The track resource of the stands' quads (sFlashTris). */
typedef struct FlashQuadList {
    /* 0x00 */ u16 count;
    /* 0x02 */ FlashQuad e[1]; /* [count] */
} FlashQuadList;

#endif
