/* Ground surfaces (gSurfaceTable, 16 entries): bits 4-7 of a ground triangle's flags (gGroundSurface) pick one. */
#ifndef STRUCTS_SURFACE_H
#define STRUCTS_SURFACE_H

#include "common.h"

/* Surface.flags. Not named: 0x20 (crashes a fast front wheel on it, keeps a wheelie going), 0x40 (the tire tracks
   go on for 45 frames after leaving it), 0x10000. */
#define SURFACE_TIRE_TRACKS         0x0001 /* a slipping rear wheel leaves tracks (decal.c) */
#define SURFACE_DUST                0x0002 /* the rear wheel throws dust */
#define SURFACE_CLODS               0x0004 /* turbo and landings throw clods */
#define SURFACE_WATER               0x0008 /* splashes where the bike crosses the water level */
#define SURFACE_SHAKE               0x0010 /* the camera shakes with the rear wheel's bump noise */
#define SURFACE_CRASH_ON_TOUCH      0x0080 /* a wheel on it crashes the rider */
#define SURFACE_OUT_OF_BOUNDS       0x0100 /* both wheels on it for 10 frames: respawn */
#define SURFACE_COOLS_ENGINE        0x0200 /* resets the engine heat */
#define SURFACE_BOOST_RAMP          0x0400 /* Excite 3D: a ramp's speed boost; no lane change onto it */
#define SURFACE_FALL                0x0800 /* a drop: a wheel on it crashes the rider, the camera watches the fall */

typedef struct Surface {
    /* 0x00 */ char name[16];
    /* 0x10 */ f32 rollingResistance; /* drag per unit of tire force */
    /* 0x14 */ f32 drag;
    /* 0x18 */ f32 grip;
    /* 0x1C */ f32 bumpHeight;      /* bike_wheel_surface_bumps */
    /* 0x20 */ f32 bumpNoise;
    /* 0x24 */ f32 bumpRate;
    /* 0x28 */ u32 flags;           /* SURFACE_* */
    /* 0x2C */ u8 color[12];        /* [0-2] dust puffs ([3] transparency), [4-6] clods, [8-10] tire tracks */
    /* 0x38 */ s32 sfxId;           /* rolling sound's resource id (bit 31: the address of one), 0 none */
    /* 0x3C */ s32 sfxSample;       /* its sample, loaded by snd_race_init; -1 none */
} Surface; /* size 0x40 */

#endif
