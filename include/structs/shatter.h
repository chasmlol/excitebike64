/* Breakable objects (shatter.c) and removable wall groups (remove.c). */
#ifndef STRUCTS_SHATTER_H
#define STRUCTS_SHATTER_H

#include "common.h"
#include "structs/track.h"

/* A triangle of a broken object flying off: its three vertices around a centre, in 16 mm units relative to the
   object's box centre. The object's model has one triangle per three vertices, in the same order. */
typedef struct ShatterPiece {
    /* 0x00 */ s16 homeX;   /* centre when whole */
    /* 0x02 */ s16 homeY;
    /* 0x04 */ s16 homeZ;
    /* 0x06 */ s16 x;       /* centre */
    /* 0x08 */ s16 y;
    /* 0x0A */ s16 z;
    /* 0x0C */ s16 vx;      /* velocity per frame */
    /* 0x0E */ s16 vy;
    /* 0x10 */ s16 vz;
    /* 0x12 */ s16 cx[3];   /* the vertices relative to the centre */
    /* 0x18 */ s16 cy[3];
    /* 0x1E */ s16 cz[3];
    /* 0x24 */ s16 rx;      /* spin per frame */
    /* 0x26 */ s16 ry;
    /* 0x28 */ s16 rz;
    /* 0x2A */ s16 bounces; /* left before it sinks; 0 stops it */
} ShatterPiece; /* size 0x2C */

/* A breakable object (object flag 1 in gObjectFlags). */
typedef struct ShatterObj {
    /* 0x00 */ TrackModel *obj;  /* its entry in gTrackObjects */
    /* 0x04 */ s32 numPieces;    /* triangles */
    /* 0x08 */ s32 timer;        /* -1 whole, else frames since it broke; hidden at 181 */
    /* 0x0C */ s32 x0;           /* its box, 16 mm units */
    /* 0x10 */ s32 x1;
    /* 0x14 */ s32 z0;
    /* 0x18 */ s32 z1;
    /* 0x1C */ s32 minY;         /* of the model's vertices */
    /* 0x20 */ s32 maxY;
    /* 0x24 */ s32 nx;           /* normal of the box's diagonal (1.0 = 0x4000): riders cross it */
    /* 0x28 */ s32 nz;
    /* 0x2C */ ShatterPiece *pieces;
} ShatterObj; /* size 0x30 */

/* Walls removed together with an object (remove.c): the wall edges whose flags carry the group (bits 3-4). */
typedef struct RemoveGroup {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ s32 id;           /* group 1-3: the object's flags bits 1-2, the trigger's id */
    /* 0x08 */ u32 flags;        /* the object's flags */
    /* 0x0C */ s32 objIndex;     /* in gTrackObjects */
    /* 0x10 */ s32 count;
    /* 0x14 */ u16 *edges;       /* wall edge indices */
} RemoveGroup; /* size 0x18 */

#endif
