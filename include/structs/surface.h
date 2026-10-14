/* Ground surfaces (gSurfaceTable, 16 entries): bits 4-7 of a ground triangle's flags (gGroundSurface) pick one. */
#ifndef STRUCTS_SURFACE_H
#define STRUCTS_SURFACE_H

#include "common.h"

typedef struct Surface {
    /* 0x00 */ char name[16];
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 drag;
    /* 0x18 */ f32 grip;
    /* 0x1C */ f32 bumpHeight;      /* bike_wheel_surface_bumps */
    /* 0x20 */ f32 bumpNoise;
    /* 0x24 */ f32 bumpRate;
    /* 0x28 */ u32 flags;           /* 0x8 mud, 0x400, 0x800 crash, 0x10000 */
    /* 0x2C */ u8 color[12];        /* [0-2] dust puffs ([3] transparency), [4-6] clods, [8-10] tire tracks */
    /* 0x38 */ s32 sfxId;           /* rolling sound's resource id (bit 31: the address of one), 0 none */
    /* 0x3C */ s32 sfxSample;       /* its sample, loaded by snd_race_init; -1 none */
} Surface; /* size 0x40 */

#endif
