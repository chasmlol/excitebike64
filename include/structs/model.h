/* Models and textures: the compact model format prim_build_dl compiles into display lists ("N64" resources) and the
   texture sets the models and the 2D images index ("PKG" resources). */
#ifndef STRUCTS_MODEL_H
#define STRUCTS_MODEL_H

#include "common.h"

/* A model as a stream of 4-byte commands; the high nibble of the first byte is the opcode:
   0 end; 1 load vertices {op, count, u16 first}; 4 the same, appended after the previous load (5 restarts at 0);
   3 triangle {op, a, b, c} (two in a row become one gSP2Triangles); 2 texture {op | palette, index,
   set << 6 | 8-bit << 2 | wrap bits, mask t << 4 | mask s}; 6 bone matrix {op, bone}. */
typedef struct PrimModel {
    /* 0x00 */ u32 numVtx;
    /* 0x04 */ Vtx_t *vtx;
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ u8 *cmds;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ void *unk_14; /* relocated when not 0 (track models) */
} PrimModel; /* size >= 0x18; the vertices and commands follow */

/* One texture of a set. */
typedef struct TexEntry {
    /* 0x00 */ u8 width;  /* in units of 8 texels */
    /* 0x01 */ u8 height;
    /* 0x02 */ u8 flags;  /* 1 8-bit (else 4-bit), 2 cut-out, 8 scrolls, 0x34 animation rate (prim_animate_textures) */
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
