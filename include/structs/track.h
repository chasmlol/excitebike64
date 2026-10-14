/* The track's world: the model lists of sections, objects and water (gTrackSections, gTrackObjects, gWaterModels),
   the ground triangles of each section, the wall mesh, surface polygons and object instances. Coordinates in these
   resources are in units of 16 mm unless noted. */
#ifndef STRUCTS_TRACK_H
#define STRUCTS_TRACK_H

#include "common.h"
#include "structs/model.h"

/* The environment type of a track (gTracks +0x2C): which code draws it. */
#define TRACK_ENV_STADIUM 0 /* arngfx.c */
#define TRACK_ENV_OUTDOOR 1 /* tubegfx.c */
#define TRACK_ENV_DESERT  2 /* desgfx.c and fractal.c */

/* gDrawFlags: bit n is item n of the debug menu, all set in a normal race. */
#define DRAW_BIKES        0x001
#define DRAW_SKY          0x002
#define DRAW_HI_ARN       0x004 /* the near track */
#define DRAW_LO_ARN       0x008 /* the far track */
#define DRAW_STAND        0x010
#define DRAW_BALES        0x020
#define DRAW_SHADOWS      0x040
#define DRAW_MUD          0x080
#define DRAW_CLEAR_SCREEN 0x100
#define DRAW_COLLISION    0x200
#define DRAW_VTX_WIND     0x400
#define DRAW_HUD          0x800

/* Values of the game's setup that the track and rendering code tests. */

/* gGame.mode */
#define GAME_MODE_EXHIBITION        0
#define GAME_MODE_SEASON            1
#define GAME_MODE_SOCCER            2
#define GAME_MODE_DESERT            5
#define GAME_MODE_EXCITE_3D         6
#define GAME_MODE_HILL_CLIMB        7
#define GAME_MODE_TIME_TRIAL        8
#define GAME_MODE_TUTORIAL          9
#define GAME_MODE_STUNT_COURSE      10
#define GAME_MODE_ORIGINAL          12  /* Original Excitebike, the NES game */
#define GAME_MODE_TEST_LOOP         13  /* the editor's test ride, closed track */
#define GAME_MODE_TEST_OPEN         14  /* the editor's test ride, track not closed */

/* gGame.flags */
#define GAME_FLAG_FRONT_END_RACE    0x00000001 /* a race the front end runs behind the title or rider select */
#define GAME_FLAG_RIDER_SELECT      0x00000010
#define GAME_FLAG_TRACK_CHOSEN      0x00000080
#define GAME_FLAG_REPLAY            0x00008000 /* replay playback */
#define GAME_FLAG_PODIUM            0x00040000 /* the podium ceremony (gPodiumMode) */

/* gCheatFlags: bit n is cheat n of sCheatTable */
#define CHEAT_BIG_HEAD              0x0001
#define CHEAT_MIRROR                0x0002
#define CHEAT_DEBUG                 0x0004
#define CHEAT_DOWN_HILL             0x0008
#define CHEAT_STUNT_BONUS           0x0010
#define CHEAT_INVISIBLE_RIDER       0x0020
#define CHEAT_NIGHT                 0x0040
#define CHEAT_ODD_COLOR             0x0080
#define CHEAT_TRANSPARENT_RIDER     0x0100
#define CHEAT_ALL_STUNTS            0x0200
#define CHEAT_INVINCIBLE            0x0400
#define CHEAT_TEAM_PHOTO            0x0800
#define CHEAT_BEAT_THIS             0x1000
#define CHEAT_PIN_HEAD              0x2000

/* The game's input word (gPadInput, gPlayerInputs, Rider.input; main_convert_pad_input), not libultra's button
   bits. Bits 24-27 and 28-31 hold the stick's x and y magnitude; the direction bits give the signs. */
#define INPUT_STICK_UP              0x00000001
#define INPUT_STICK_DOWN            0x00000002
#define INPUT_STICK_LEFT            0x00000004
#define INPUT_STICK_RIGHT           0x00000008
#define INPUT_A                     0x00000010 /* throttle */
#define INPUT_B                     0x00000020 /* brake */
#define INPUT_START                 0x00000040
#define INPUT_C_UP                  0x00000080
#define INPUT_C_DOWN                0x00000100
#define INPUT_C_LEFT                0x00000200
#define INPUT_C_RIGHT               0x00000400
#define INPUT_D_UP                  0x00000800
#define INPUT_D_DOWN                0x00001000
#define INPUT_D_LEFT                0x00002000
#define INPUT_D_RIGHT               0x00004000
#define INPUT_L                     0x00008000
#define INPUT_R                     0x00010000 /* air tricks; with B, a power slide */
#define INPUT_Z                     0x00020000 /* turbo */
#define INPUT_STICK_X_SHIFT         24
#define INPUT_STICK_Y_SHIFT         28

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

/* gObjectFlags: per object of gTrackObjects, copied from its mask in the resource. */
#define OBJ_BREAKABLE  0x01 /* shatters when a rider rides through it (shatter.c) */
#define OBJ_WALL_GROUP 0x06 /* bits 1-2: its group of removable walls (remove.c) */
#define OBJ_HIDDEN     0x08 /* removed: no longer drawn */

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

/* gWallEdgeFlags: one byte per edge of the wall mesh. Bits 1-2 are the kind of wall a hit reports
   (sWallHitType). */
#define WALL_EDGE_GROUP(flags) (((flags) >> 3) & 3) /* bits 3-4: removable group (remove.c), 0 none */
#define WALL_EDGE_REMOVED 0x20 /* its group was removed: nothing collides with it */
#define WALL_EDGE_CAMERA  0x40 /* the camera collides with it */

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
