/* The track's world: the model lists of sections, objects and water (gTrackSections, gTrackObjects, gWaterModels),
   the ground triangles of each section, the wall mesh, surface polygons and object instances. Coordinates in these
   resources are in units of 16 mm unless noted. */
#ifndef STRUCTS_TRACK_H
#define STRUCTS_TRACK_H

#include "common.h"
#include "structs/model.h"

/* A ground triangle of a section, binned on the section's 8x8 grid of its bounding box. */
typedef struct CollTri {
    /* 0x00 */ union {
                   u16 h[4];
                   u32 w[2];
               } cells;      /* bit z * 8 + x: the triangle touches that grid cell */
    /* 0x08 */ u16 vtx[3];  /* indices into the section model's vertices */
    /* 0x0E */ u16 flags;   /* bits 4-7 surface (gSurfaceTable), 0x100 skipped by the ground search, 0x400 bridge */
} CollTri; /* size 0x10 */

typedef struct CollGrid {
    /* 0x00 */ u16 flags;   /* 1 valid; 4 vertices relative to the centre of the box */
    /* 0x02 */ u16 count;
    /* 0x04 */ CollTri tris[1]; /* [count] */
} CollGrid;

/* A model placed on the track. Sections are the height areas of height.c: gGroundArea indexes them. */
typedef struct TrackModel {
    /* 0x00 */ PrimModel *model;
    /* 0x04 */ s32 x0;      /* bounding box */
    /* 0x08 */ s32 z0;
    /* 0x0C */ s32 x1;
    /* 0x10 */ s32 z1;
    /* 0x14 */ CollGrid *grid; /* sections: the ground triangles */
    /* 0x18 */ s32 visible; /* arn_cull_models */
    /* 0x1C */ u32 mask;    /* per view v: bit 2v visible, 2v+1 far, 0x100 << v fogged; objects: section << 16 and,
                               in the resource, the object flags (gObjectFlags) */
} TrackModel; /* size 0x20 */

typedef struct TrackModelList {
    /* 0x00 */ u32 count;
    /* 0x04 */ TrackModel e[1]; /* [count] */
} TrackModelList;

/* Outdoor tracks: an animated texture a model uses, drawn through segment 8 + its place in the model's list
   (tube_remap_tex_addr, tube_set_anim_tex_segs). */
typedef struct TexSegRef {
    /* 0x00 */ u8 groupStart; /* first texture of the animation group in gAnimTexSet */
    /* 0x01 */ u8 groupLen;
    /* 0x02 */ u8 offset;     /* the texture's place in the group */
    /* 0x03 */ u8 pad03;
} TexSegRef; /* size 0x4 */

/* Outdoor tracks: the box of a section grown to hold the objects assigned to it (sSectionBounds). */
typedef struct TrackBounds {
    /* 0x00 */ s32 x0;
    /* 0x04 */ s32 x1;
    /* 0x08 */ s32 z0;
    /* 0x0C */ s32 z1;
} TrackBounds; /* size 0x10 */

/* height.c bins the sections on a 16x16 grid of the track: each cell lists the sections it overlaps. */
typedef struct HeightCell {
    /* 0x00 */ u16 count;
    /* 0x02 */ u16 ids[1]; /* [count] section indices */
} HeightCell;

/* A ground triangle near a point, in mm (height_collect_ground_tris, for shadows and tire tracks). */
typedef struct GroundTri {
    /* 0x00 */ s32 area;    /* the place of its section in the grid cell's list */
    /* 0x04 */ s32 tri;     /* its index in the section's CollGrid (desert: the height-map triangle) */
    /* 0x08 */ Vec v[3];
} GroundTri; /* size 0x2C */

/* Surface polygons: areas of the ground with their own surface, in units of 64 mm. */
typedef struct SurfacePoint {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 z;
} SurfacePoint; /* size 0x4 */

typedef struct SurfacePoly {
    /* 0x00 */ s32 surface; /* read into gGroundSurface */
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 count;
    /* 0x08 */ SurfacePoint pts[1]; /* [count] */
} SurfacePoly;

/* The wall mesh: vertical edges with a height, and triangles, binned on a 16x16 grid of the track. A track's is a
   resource (also the user track arena's and each piece type's); a user track's is merged from those
   (usrtrk_build_track). */
typedef struct WallVtx {
    /* 0x00 */ s16 x;       /* 64 mm units */
    /* 0x02 */ s16 z;
    /* 0x04 */ s16 y;       /* 16 mm units */
    /* 0x06 */ s16 h;       /* height of the wall above y */
} WallVtx; /* size 0x8 */

typedef struct WallEdge {
    /* 0x00 */ s16 v0;
    /* 0x02 */ s16 v1;
    /* 0x04 */ s16 nx;      /* normal (1.0 = 0x4000); -0x8000 in the resource: nz holds the edge's flags */
    /* 0x06 */ s16 nz;      /* bit 0: already gathered */
} WallEdge; /* size 0x8 */

typedef struct WallTri {
    /* 0x00 */ s16 v[3];
    /* 0x06 */ s16 nx;
    /* 0x08 */ s16 ny;
    /* 0x0A */ s16 nz;      /* bit 0: already gathered */
} WallTri; /* size 0xC */

/* Each cell's lists end where the next cell's begin (257 entries). */
typedef struct WallCell {
    /* 0x00 */ u16 *tris;
    /* 0x04 */ u16 *edges;
} WallCell; /* size 0x8 */

typedef struct WallMesh {
    /* 0x00 */ s32 minX;
    /* 0x04 */ s32 minZ;
    /* 0x08 */ s32 maxX;
    /* 0x0C */ s32 maxZ;
    /* 0x10 */ s32 numVtx;
    /* 0x14 */ WallVtx *vtx;
    /* 0x18 */ s32 numEdges;
    /* 0x1C */ WallEdge *edges;
    /* 0x20 */ s32 numTris;
    /* 0x24 */ WallTri *tris;
    /* 0x28 */ WallCell cells[257];
} WallMesh; /* size 0x830 */

/* Trackside object instances (instance.c): a resource of model ids followed by the placed items. */
typedef struct Instance {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 z;
    /* 0x0C */ u16 unk_0C;
    /* 0x0E */ u8 rot;      /* yaw / 256 */
    /* 0x0F */ u8 model;    /* index into modelIds */
} Instance; /* size 0x10 */

typedef struct InstanceRes {
    /* 0x00 */ u32 numModels;
    /* 0x04 */ u32 numItems;
    /* 0x08 */ u32 modelIds[1]; /* [numModels], then Instance[numItems] */
} InstanceRes;

#endif
