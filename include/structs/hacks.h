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

#endif
