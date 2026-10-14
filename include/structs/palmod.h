/* Palettes rewritten every frame (palmod.c): the lightning flashes of the night track, the podium's fade to grey,
   the credits' sepia and the sun glare. */
#ifndef STRUCTS_PALMOD_H
#define STRUCTS_PALMOD_H

#include "common.h"
#include "structs/model.h"

/* A texture set's palette registered by palmod_register. Each frame the set's pal points to pal[gGfxFrameIndex]:
   the set's own palette for frame 0 and a copy for frame 1, so the one being rewritten is not the one drawn. */
typedef struct PalMod {
    /* 0x00 */ u8 *rgb;     /* 256 x r, g, b (5 bits each), then the copy pal[1] at +0x300 */
    /* 0x04 */ s32 kind;    /* 1 terrain, 2 objects, else 0; not read */
    /* 0x08 */ TexSet *set;
    /* 0x0C */ u16 *pal[2];
} PalMod; /* size 0x14 */

#endif
