#include "game.h"

/* Columns of the wipe across the screen. */
#define WIPE_COLUMNS 64

/* Draws the fade (gGame.fadeState, fadeLevel) as a wipe: 64 black columns whose alpha ramps across the screen, left to
   right while fading out and right to left while fading in. Game flag 0x10000000 draws it fully black. In any other
   state `level` is left unset; the callers only draw the wipe while fading. */
Gfx *screen_draw_wipe(Gfx *gdl, Dyn *dyn, s32 fb)
{
    s32 level;
    s32 i;
    s32 alpha;
    s32 width;
    s32 x;

    if (gGame.flags & 0x10000000) {
        level = 0xFF;
    } else {
        if (gGame.fadeState == FADE_SWAP) {
            return gdl;
        }
        if (gGame.fadeState == FADE_OUT) {
            goto fade_out;
        }
        if (gGame.fadeState == FADE_IN) {
            goto fade_in;
        }
        goto body;
    fade_out:
        level = 0xFF - gGame.fadeLevel;
        goto body;
    fade_in:
        level = gGame.fadeLevel;
    }
body:
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, 0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gSPViewport(gdl++, &gScreenViewport);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
    for (i = 0; i < WIPE_COLUMNS; i++) {
        alpha = level * 2 + i * 8 - 0x100;
        if (alpha > 0xFF) {
            alpha = 0xFF;
        }
        if (alpha < 0) {
            alpha = 0;
        }
        width = gScreenWidth / WIPE_COLUMNS;
        x = i * gScreenWidth / WIPE_COLUMNS;
        if (gGame.fadeState == FADE_IN) {
            x = gScreenWidth - width - x;
        }
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
        gDPFillRectangle(gdl++, x, 0, x + width, gScreenHeight);
    }
    return gdl;
}
