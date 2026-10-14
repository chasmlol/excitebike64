/* The track editor's 3D view (arnedgfx.c): a grid of 500-unit cells, each holding up to two pieces whose vertices
   are turned and moved into one of 450 vertex buffers, the floor, the walls round the grid, the cursor, and the
   views that draw single objects in their own viewport. */
#ifndef STRUCTS_ARNED_H
#define STRUCTS_ARNED_H

#include "common.h"
#include "structs/model.h"

#define ARNED_VTX_BUFS     450      /* sArnedVtxBufs, sArnedSlotOwner */
#define ARNED_BUF_VTX      117      /* vertices of a buffer: a cell's pieces with more are not drawn */
#define ARNED_SLOT_FREE    -1       /* sArnedSlotOwner: else the cell that owns the buffer */
#define ARNED_SLOT_RELEASED -2      /* given up by a cell this frame: free again at the next arned_draw */
#define ARNED_OBJ_VIEWS    8
#define ARNED_VIEW_OBJS    12

/* ArnedCell.flags */
#define ARNED_CELL_SELECTED 1       /* drawn pulsing; piece2 hidden while a user piece is edited */
#define ARNED_CELL_REBUILD  2       /* arned_set_cell: rebuild even if unchanged (not kept) */

/* A cell of the grid (arned_set_cell). The pieces' vertices are rebuilt into sArnedVtxBufs[slot] when dirty. */
typedef struct ArnedCell {
    /* 0x00 */ PrimModel *piece;
    /* 0x04 */ PrimModel *piece2; /* drawn over the first, moved by dx, dz */
    /* 0x08 */ s16 slot;          /* vertex buffer, -1 none */
    /* 0x0A */ u8 rot;            /* quarter turns of piece */
    /* 0x0B */ u8 rot2;
    /* 0x0C */ u8 flags;          /* ARNED_CELL_* */
    /* 0x0D */ u8 dirty;
    /* 0x0E */ s16 dx;
    /* 0x10 */ s16 dz;
    /* 0x12 */ s16 pad12;
} ArnedCell; /* size 0x14 */

/* An object placed in an object view (arned_add_object), with its matrix for each frame buffer. */
typedef struct ArnedObject {
    /* 0x00 */ PrimModel *model;
    /* 0x04 */ s32 pad04;
    /* 0x08 */ Mtx mtx[2];
} ArnedObject; /* size 0x88 */

/* One of the 8 views that draw objects in their own viewport (arned_set_object_view); vp and proj per frame
   buffer. Emptied when drawn. */
typedef struct ArnedObjView {
    /* 0x000 */ s32 x;
    /* 0x004 */ s32 y;
    /* 0x008 */ Vp vp[2];
    /* 0x028 */ Mtx proj[2];
    /* 0x0A8 */ s32 count;
    /* 0x0AC */ u16 perspNorm;
    /* 0x0AE */ u8 unk_AE;
    /* 0x0AF */ u8 padAF;
    /* 0x0B0 */ ArnedObject objs[ARNED_VIEW_OBJS];
} ArnedObjView; /* size 0x710 */

#endif
