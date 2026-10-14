/* The sun, lens flares and stars (sun.c) and the sky backdrop. */
#ifndef STRUCTS_SUN_H
#define STRUCTS_SUN_H

#include "common.h"

/* A light that can flare: the sun or a light of the track. Each frame sun_update_screen_pos projects it,
   sun_check_occlusion samples the Z buffer around it, and sun_draw_flare draws its rings and glare sprite. */
#define FLARE_SUN       0x01 /* placed where the sky puts the sun */
#define FLARE_FIXED     0x02 /* at a fixed point of the view */
#define FLARE_ON_SCREEN 0x08
#define FLARE_SAMPLED   0x10 /* its visibility was read from the Z buffer */

#define SUN_NUM_VTX   128 /* the vertex buffer of each frame (sSunVtxBufs) */
#define SUN_NUM_STARS 64

/* gSkyMode: what the background of a race is */
#define SKY_FILL        0 /* the fog colour */
#define SKY_IMAGE       1 /* the sky image (sky_draw) */
#define SKY_MODEL       2 /* the sky model, shaded */
#define SKY_MODEL_IMAGE 3 /* the sky model textured with the sky image */

typedef struct Flare {
    /* 0x00 */ u32 flags;   /* FLARE_* */
    /* 0x04 */ s32 x;       /* position when neither FLARE_SUN nor FLARE_FIXED */
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 sx;      /* screen position */
    /* 0x18 */ s32 sy;
    /* 0x1C */ s32 sz;      /* screen depth, 0x7FFF for the sun */
    /* 0x20 */ s32 visible; /* 0-5: how much of it the Z buffer shows, -1 hidden; also picks the glare sprite */
} Flare; /* size 0x24 */

/* The track's flare resource (sSunList). The glare of the first entry (gSunGlare) brightens the outdoor
   palettes (palmod.c). */
typedef struct FlareList {
    /* 0x00 */ s32 texRes;  /* resource of the glare textures (sFlareTextures) */
    /* 0x04 */ s32 count;
    /* 0x08 */ Flare e[1];  /* [count] */
} FlareList;

#endif
