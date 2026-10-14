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
    /* 0x2C */ u8 color[12];        /* two colours (dust, tire tracks) */
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s32 unk_3C;
} Surface; /* size 0x40 */

#endif
