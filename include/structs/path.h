/* The racing line (path.c): gPathNodes, gPathNodeCount nodes of 0x6C bytes, and the records of a path resource. */
#ifndef STRUCTS_PATH_H
#define STRUCTS_PATH_H

#include "common.h"

/* PathNode.flags */
#define PATH_NODE_CAM_PITCH   0x01  /* the camera's pitch follows the path */
#define PATH_NODE_FAST_CORNER 0x04  /* corner speed doubled */
#define PATH_NODE_BAD_LANDING 0x10

/* The header word of a path resource: 0xAA << 24 | flags */
#define PATH_RES_MAGIC          0xAA
#define PATH_RES_WIDTHS         0x01 /* 0x28-byte records: widths and fog ranges */
#define PATH_RES_NODE_FLAGS     0x02 /* a byte of PathNode.flags after the record */
#define PATH_RES_PROGRESS       0x04 /* 2 bytes of lapProgress */
#define PATH_RES_EXTRA          0x08 /* 2 bytes, skipped */
#define PATH_RES_PROGRESS_FIRST 0x10 /* the extra bytes in the order progress, extra, flags */
#define PATH_RES_START_FINISH   0x20 /* the start and finish nodes follow the node count */

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
    /* 0x64 */ u8 flags;            /* PATH_NODE_* */
    /* 0x65 */ s8 speedTweak;       /* corner speed * (1 + speedTweak / 10); the generated straight path (game
                                       mode 6) keeps 2 bits of ground type per lane here */
    /* 0x66 */ u16 lapProgress;     /* 0x8000 * distance left to the finish / lap length */
    /* 0x68 */ f32 crestSpeed;      /* a speed from the resource, at most maxSpeed: a crest that launches the bike at
                                       it limits maxSpeed to landings on the track (path_limit_jump_speeds) */
} PathNode; /* size 0x6C */

/* A node record of a path resource (after the 0xAA header word, the node count and optionally start/finish):
   0x20 bytes, 0x28 with PATH_RES_WIDTHS, then the optional bytes of the other PATH_RES_* flags. */
typedef struct PathResNode {
    /* 0x00 */ s32 x;               /* 1/1000 units */
    /* 0x04 */ s32 z;
    /* 0x08 */ u16 maxSpeed;
    /* 0x0A */ u16 crestSpeed;      /* 1/1000 */
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
    /* 0x20 */ s16 widthNeg;        /* PATH_RES_WIDTHS: width * 62.5 + 62.5 */
    /* 0x22 */ s16 widthPos;
    /* 0x24 */ u32 fogRange;
} PathResNode; /* size 0x28 */

#endif
