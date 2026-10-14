#include "game.h"

/* sFontSetLoaded: which font set is loaded. */
#define FONT_SET_NONE 0
#define FONT_SET_MENU 1
#define FONT_SET_RACE 2

/* Loads the race's three fonts (sRaceFontIds), unloading the menu set first. The fonts are unpacked through the Z
   buffer, so it waits until no graphics task uses it. */
void font_load_race_set(void *resSet)
{
    if (sFontSetLoaded != FONT_SET_RACE) {
        if (sFontSetLoaded != FONT_SET_NONE && font_unload() != 0) {
            fatal_halt(1);
        }
        while (sched_get_current_task() != 0) {
        }
        gFontLoadBuffer = gZBuffer;
        if (font_load(resSet, sRaceFontIds, gFontColors) != 0) {
            fatal_halt(1);
        }
        gFontLoadBuffer = 0;
        sFontSetLoaded = FONT_SET_RACE;
    }
}

/* Loads the front end's five fonts (sMenuFontIds), unloading the race set first. */
void font_load_menu_set(u32 *resSet)
{
    if (sFontSetLoaded != FONT_SET_MENU) {
        if (sFontSetLoaded != FONT_SET_NONE && font_unload() != 0) {
            fatal_halt(1);
        }
        if (font_load(resSet, sMenuFontIds, gFontColors) != 0) {
            fatal_halt(1);
        }
        sFontSetLoaded = FONT_SET_MENU;
    }
}

/* Unloads whichever font set is loaded. */
void font_unload_set(void)
{
    font_unload();
    sFontSetLoaded = FONT_SET_NONE;
}

/* Sets up the RDP for text: no TLUT, blend alpha 0x10, no perspective correction, point sampling, texturing on. */
Gfx *font_gfx_setup(Gfx *gdl)
{
    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0x10);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureFilter(gdl++, G_TF_POINT);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    return gdl;
}
