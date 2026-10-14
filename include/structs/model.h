/* Models and textures: the compact model format prim_build_dl compiles into display lists ("N64" resources) and the
   texture sets the models and the 2D images index ("PKG" resources). */
#ifndef STRUCTS_MODEL_H
#define STRUCTS_MODEL_H

#include "common.h"

/* A model as a stream of 4-byte commands; the high nibble of the first byte is the opcode:
   0 end; 1 load vertices {op, count, u16 first}; 4 the same, appended after the previous load (5 restarts at 0);
   3 triangle {op, a, b, c} (two in a row become one gSP2Triangles); 2 texture {op | palette, index,
   set << 6 | 8-bit << 2 | wrap bits, mask t << 4 | mask s}; 6 bone matrix {op, bone}. */
#define PRIM_CMD_END        0
#define PRIM_CMD_VTX        1 /* load count vertices at 0 */
#define PRIM_CMD_TEX        2
#define PRIM_CMD_TRI        3
#define PRIM_CMD_VTX_APPEND 4 /* load count vertices after the previous load */
#define PRIM_CMD_VTX_RESET  5 /* the next appended load starts at 0 again */
#define PRIM_CMD_BONE       6 /* load the bone's matrix (gPrimBoneMtx) */

/* prim_build_dl's flags */
#define PRIM_END_DL       0x01 /* end the display list */
#define PRIM_NO_TEXTURES  0x02 /* skip the texture commands (shadows) */
#define PRIM_LOAD_TLUT    0x04 /* load the set's TLUT with its first texture */
#define PRIM_RESET_TLUT   0x08 /* forget which set's TLUT is loaded */
#define PRIM_SEGMENTED    0x20 /* texture and TLUT addresses in segment 2 + set, relative to the set */
#define PRIM_TLUT_SEGMENT 0x40 /* the TLUT at the start of segment 1 + set */

typedef struct PrimModel {
    /* 0x00 */ u32 numVtx;
    /* 0x04 */ Vtx *vtx;
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ u8 *cmds;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ void *unk_14; /* relocated when not 0 (track models) */
} PrimModel; /* size >= 0x18; the vertices and commands follow */

/* gRiderViewLod: per rider, a byte per view (view * 8): the bike's level of detail in the low nibble, the thrown
   rider's in the high one (render_get_rider_lod). */
#define RIDER_LOD_HIDDEN 0
#define RIDER_LOD_LOW    1 /* far: the one-piece bike (sBikeLowDL), the low body */
#define RIDER_LOD_MID    2
#define RIDER_LOD_HIGH   3

/* render_draw_image's flags */
#define IMG_HALF_BAND 0x001 /* bands of half as many rows */
#define IMG_8BIT      0x002 /* 8-bit texels, else 4-bit CI */
#define IMG_IA        0x004 /* 8-bit IA, else CI */
#define IMG_UPRIGHT   0x020 /* rows top first (images are kept upside down, prim_flip_images) */
#define IMG_TURNED    0x040 /* turned 180 degrees */
#define IMG_WIDTH     0x080 /* drawn `width` texels wide */
#define IMG_HALF_SIZE 0x100

/* TexEntry.flags */
#define TEX_8BIT      0x01 /* 8-bit texels, else 4-bit */
#define TEX_CUTOUT    0x02 /* alpha-tested: the model switches the render mode (prim_build_dl's callback) */
#define TEX_SCROLL    0x08 /* scrolls with the frame counter */
#define TEX_ANIM_RATE 0x34 /* the rate of an animated group (prim_animate_textures), 0 not animated */

/* One texture of a set. */
typedef struct TexEntry {
    /* 0x00 */ u8 width;  /* in units of 8 texels */
    /* 0x01 */ u8 height;
    /* 0x02 */ u8 flags;  /* TEX_* */
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 *data;
} TexEntry; /* size 0x8 */

/* A texture set: a TLUT and the textures, in the order the models' texture commands index them. The header has the
   size of an entry, so `(TexEntry *)set + 1` is the first texture. */
typedef struct TexSet {
    /* 0x00 */ u32 count;
    /* 0x04 */ u16 *pal;
    /* 0x08 */ TexEntry tex[1]; /* [count] */
} TexSet;

#endif
