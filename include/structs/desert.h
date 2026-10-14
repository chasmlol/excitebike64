/* The desert: the fractal terrain tiles (fractal.c) and the cached height-map patches (desgfx.c). */
#ifndef STRUCTS_DESERT_H
#define STRUCTS_DESERT_H

#include "common.h"
#include "structs/model.h"

/* An object standing on the desert, in mm: scenery placed at random on each tile, or a campfire. height.c copies the
   ones near a bike into sDesertObjs and pushes the bikes out of their boxes. */
typedef struct DesertObj {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;       /* the ground's height under it */
    /* 0x08 */ s32 z;
    /* 0x0C */ s32 radius;  /* half the width of its box: 400 (0 for campfires) */
    /* 0x10 */ s32 height;  /* 1000 (0 for campfires) */
    /* 0x14 */ s32 kind;    /* index into sDesObjModels: 0 or 1 scenery, 2 campfire */
    /* 0x18 */ s32 ref;     /* campfires: the checkpoint (gDesWaypoint is the next one) */
} DesertObj; /* size 0x1C */

/* Work items of a tile, op << 24 | arguments; the stack is run from the top, a few items per frame. */
#define DESERT_WORK_SUBDIVIDE 0 /* x << 16 | row << 8 | level: diamond-square step on a square of 1 << level cells */
#define DESERT_WORK_SMOOTH 1    /* row << 7 | col: smooth the next 64 cells, then push the item for the rest */
#define DESERT_WORK_OBJECTS 2   /* place the campfires and the scenery (fractal_place_objects) */

/* One of the 16 tiles of 128x128 height-map cells (2000 mm each) that form a 4x4 window around the camera. Tile
   (x, z) is rows (z & 3) * 128 and columns (x & 3) * 128 on of the 512x512 gDesertHeightMap. Its edges come from
   hashes of its coordinates (fractal_corner_height), so neighbouring tiles meet. */
typedef struct DesertTile {
    /* 0x000 */ s32 x;           /* in tiles (256000 mm); -999 empty */
    /* 0x004 */ s32 z;
    /* 0x008 */ s16 visible;     /* near the camera: fractal_update_tiles finishes its work at once */
    /* 0x00A */ s16 unk_0A;
    /* 0x00C */ s32 seed;        /* sFractalRand between two work slices */
    /* 0x010 */ s32 sp;          /* work items on the stack; 0 when the tile is done */
    /* 0x014 */ u32 stack[64];   /* DESERT_WORK_* items */
    /* 0x114 */ u8 nextRow[129]; /* row 128 (the first row of the next tile in z) */
    /* 0x195 */ u8 nextCol[129]; /* column 128 */
    /* 0x216 */ u8 smooth[2][128]; /* the current and the previous row as they were before smoothing */
    /* 0x316 */ u8 pad316[2];
    /* 0x318 */ s32 smoothCur;   /* which of smooth[] is the current row */
    /* 0x31C */ u8 *map;         /* the tile's first cell in gDesertHeightMap (rows of 512) */
    /* 0x320 */ s32 numObjs;
    /* 0x324 */ DesertObj objs[8];
} DesertTile; /* size 0x404 */

/* The desert's checkpoints (sDesertCheckpoints, in metres), a lap of them; a campfire burns at each. */
#define DESERT_NUM_CHECKPOINTS 10

/* A campfire, one at each of the ten checkpoints (desert_export_checkpoints), in mm. fractal_place_objects puts it
   on the ground of its tile. */
typedef struct DesertFire {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 z;
    /* 0x08 */ s32 y;
} DesertFire; /* size 0xC */

/* The model of each kind of desert object; the list ends with a null res. */
typedef struct DesObjModel {
    /* 0x00 */ s32 *res;    /* resource ids; the first is loaded */
    /* 0x04 */ PrimModel *model;
} DesObjModel; /* size 0x8 */

/* A cached patch of the desert ground: 5x5 vertices, every 1 << lod cells of the height map. 250 of them serve the
   three rings of sDesPatchGrid (32x32 patches each, 500 << lod units apart); a patch not used for two frames is
   free again. Units are 16 mm. */
typedef struct DesPatch {
    /* 0x000 */ Vtx vtx[25];     /* x and z relative to the centre of its block */
    /* 0x190 */ s32 x;           /* corner */
    /* 0x194 */ s32 z;
    /* 0x198 */ s8 life;         /* frames left; 0 free */
    /* 0x199 */ u8 lod;          /* ring */
    /* 0x19A */ u8 near;         /* ring 0 close to the camera: drawn by des_draw_near_terrain instead */
    /* 0x19B */ u8 seams;        /* sides that meet the next ring: which of sDesPatchDLs */
    /* 0x19C */ s16 blockX;      /* the block of 8000 units it lies in */
    /* 0x19E */ s16 blockZ;
    /* 0x1A0 */ s8 block;        /* this frame's matrix of its block in sDesBlockMtx */
    /* 0x1A1 */ s8 visible;
    /* 0x1A2 */ u8 pad1A2[6];
} DesPatch; /* size 0x1A8 */

#endif
