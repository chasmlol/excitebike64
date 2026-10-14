/* The racing line (path.c): gPathNodes, gPathNodeCount nodes of 0x6C bytes, and the records of a path resource. */
#ifndef STRUCTS_PATH_H
#define STRUCTS_PATH_H

#include "common.h"

/* A node of the racing line. Links are node indices, -1 for none. The segment from a node to its `next` is a cubic
   in t (0..1): x(t) = pos.x + splineX[0] t^3 + splineX[1] t^2 + splineX[2] t, the same for z. */
typedef struct PathNode {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 length;          /* xz distance to the next node */
    /* 0x10 */ f32 maxSpeed;        /* corner, braking and jump limits (the CPU riders' target speed) */
    /* 0x14 */ f32 lateralX;        /* unit vector across the track */
    /* 0x18 */ f32 lateralZ;
    /* 0x1C */ f32 curvature;       /* signed, the smaller of the segment's two ends */
    /* 0x20 */ f32 bank;            /* ground height difference across the track */
    /* 0x24 */ f32 slope;           /* height change per unit of length to the next node */
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ f32 splineX[3];
    /* 0x38 */ f32 splineZ[3];
    /* 0x44 */ s8 respawnDir;       /* != 0: no respawn here, look further on (> 0) or back (< 0) */
    /* 0x45 */ s8 lookahead;        /* nodes until the summed |curvature| reaches 0.06 (or a fork) */
    /* 0x46 */ s16 next;
    /* 0x48 */ s16 branch;          /* second way out of a fork */
    /* 0x4A */ s16 prev;
    /* 0x4C */ s16 join;            /* where a branch comes back */
    /* 0x4E */ s16 alt;             /* a parallel node that may be nearer */
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 camWidthNeg;     /* camera limits across the track (1/62.5 units) */
    /* 0x54 */ s16 camWidthPos;
    /* 0x56 */ s16 camCeiling;      /* camera height limit above the node (1/62.5 units); < 0: walls instead */
    /* 0x58 */ f32 widthNeg;        /* free width on the -lateral side (raycast against the walls, up to 8) */
    /* 0x5C */ f32 widthPos;        /* free width on the +lateral side */
    /* 0x60 */ u32 fogRange;        /* per screen count (1, 2, 3+): two signed 4-bit fog offsets in byte n */
    /* 0x64 */ u8 flags;            /* 1 camera pitch follows the path, 4 corner speed doubled, 0x10 bad landing */
    /* 0x65 */ s8 speedTweak;       /* corner speed * (1 + speedTweak / 10); the generated straight path (game
                                       mode 6) keeps 2 bits of ground type per lane here */
    /* 0x66 */ u16 lapProgress;     /* 0x8000 * distance left to the finish / lap length */
    /* 0x68 */ f32 unk_68;          /* a speed from the resource, at most maxSpeed; crests are tested with it */
} PathNode; /* size 0x6C */

/* A node record of a path resource (after the 0xAA header word, the node count and optionally start/finish):
   0x20 bytes, 0x28 with header flag 1, then optional bytes for flags 2, 4 and 8. */
typedef struct PathResNode {
    /* 0x00 */ s32 x;               /* 1/1000 units */
    /* 0x04 */ s32 z;
    /* 0x08 */ u16 maxSpeed;
    /* 0x0A */ u16 unk_68;          /* 1/1000 */
    /* 0x0C */ s16 next;
    /* 0x0E */ s16 branch;
    /* 0x10 */ s16 prev;
    /* 0x12 */ s16 join;
    /* 0x14 */ s16 alt;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ u8 respawnDir;
    /* 0x19 */ u8 speedTweak;
    /* 0x1A */ s16 camWidthNeg;
    /* 0x1C */ s16 camWidthPos;
    /* 0x1E */ s16 camCeiling;
    /* 0x20 */ s16 widthNeg;        /* header flag 1: width * 62.5 + 62.5 */
    /* 0x22 */ s16 widthPos;
    /* 0x24 */ u32 fogRange;
} PathResNode; /* size 0x28 */

#endif
