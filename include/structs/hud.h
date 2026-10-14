/* The race's text objects (min.c), drawn by the HUD, the track intro and the congratulations screen
   (unnamed/s0_8006C510.c). */
#ifndef STRUCTS_HUD_H
#define STRUCTS_HUD_H

#include "common.h"
#include "structs/font.h"

/* gRaceTextObjs. race_alloc_text_objs creates only the text objects; race_intro_load also loads the pictures. */
typedef struct RaceTexts {
    /* 0x000 */ TextObj *text[74];
    /* 0x128 */ u8 *introImages;   /* picture set of the track intro's frames and markers */
    /* 0x12C */ Pic *roundPics[5]; /* season: the pictures of the round's tracks */
    /* 0x140 */ Pic *introPic;    /* drawn at the top left of the track intro */
    /* 0x144 */ Pic *lapArrows;   /* the arrows beside the lap count */
} RaceTexts; /* size 0x148 */

#endif
