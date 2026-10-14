/* The minimap (map.c): an image of the racing line drawn once per race, with the riders' dots on it; with several
   views a progress bar takes its place. */
#ifndef STRUCTS_MAP_H
#define STRUCTS_MAP_H

#include "common.h"

/* The racing line drawn by map_build_image into a 128x128 distance map, then halved into the texture. */
typedef struct MapImage {
    /* 0x00 */ s32 scaleX;  /* map pixels (of 128) per world unit, * 0x4000 */
    /* 0x04 */ s32 scaleZ;
    /* 0x08 */ u8 tex[64 * 64]; /* IA8 */
} MapImage; /* size 0x1008; only 0x1003 bytes are allocated */

#endif
