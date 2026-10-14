/* hacks.c: the code some tracks run on top of the generic race (Lefty's Mill's floating planks, The Gravel Pit's
   conveyors). render.c calls the draw hooks. */
#ifndef STRUCTS_HACKS_H
#define STRUCTS_HACKS_H

#include "common.h"

/* An entry of sTrackHacks: the hooks of one track; sCurTrackHack is the current track's, or NULL. */
typedef struct TrackHack {
    /* 0x00 */ s32 track;               /* gTracks index */
    /* 0x04 */ void (*init)(void);
    /* 0x08 */ void (*exit)(void);
    /* 0x0C */ void (*update)(void);
    /* 0x10 */ s32 (*drawPass1)(s32 arg);
    /* 0x14 */ s32 (*drawPass2)(s32 arg);
    /* 0x18 */ s32 numBufs;             /* 0x40-byte buffers, two banks (sHackBuffers) */
    /* 0x1C */ s32 numRes;              /* resources loaded into sHackResources */
    /* 0x20 */ s32 **resNames;
} TrackHack; /* size 0x24 */

/* The Gravel Pit: a mover (sGravelPitMovers, 2) carries `count` items, each one of the four models of
   sGravelPitModels picked at random, along a straight line, over and over. */
#define GRAVEL_PIT_NUM_MOVERS   2
#define GRAVEL_PIT_NUM_MODELS   4
#define GRAVEL_PIT_ITEM_LENGTH  11500   /* mm of line per item */

typedef struct GravelPitMover {
    /* 0x00 */ s32 lineX0, lineZ0;      /* the line as the data gives it: its direction and middle count */
    /* 0x08 */ s32 lineX1, lineZ1;
    /* 0x10 */ Vec3i start;             /* hack_gravel_pit_init: the line through the middle, count items long, at */
    /* 0x1C */ Vec3i end;               /* the ground height of the middle */
    /* 0x28 */ s32 count;               /* items on the line, 3 at most */
    /* 0x2C */ s32 section;             /* the track section (gGroundArea) of the middle */
    /* 0x30 */ u16 angle;               /* the line's direction */
    /* 0x32 */ s16 progress[3];         /* each item's place on the line, 0-0xFF */
    /* 0x38 */ u8 model[4];             /* each item's model */
    /* 0x3C */ Mtx *mtx;                /* count matrices per view (4) and frame (2) */
} GravelPitMover; /* size 0x40 */

/* Lefty's Mill: the track's object instances (gInstanceItems) are planks floating on the water. Each bobs on a sine
   and is pressed down by the wheels near its axis; sMillObjState holds one Vec3i per instance: x its resting
   height, y its offset, z how far the riders press it this frame (0-0x100). */
#define MILL_REACH      0x10000 /* only wheels this close to a plank in x and in z are tested (mm) */
#define MILL_PRESS_DIST 400     /* a wheel this close to a plank's axis presses it down (mm) */

#endif
