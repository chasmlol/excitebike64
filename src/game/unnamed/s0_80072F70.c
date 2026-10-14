#include "game.h"

Gfx *screen_draw_wipe(Gfx *gdl, Dyn *arg1, s32 arg2)
{
    s32 a;
    s32 i;
    s32 c;
    s32 w;
    s32 x;

    if (gGame.flags & 0x10000000) {
        a = 0xFF;
    } else {
        if (gGame.fadeState == 1) {
            return gdl;
        }
        if (gGame.fadeState == 0) {
            goto m0;
        }
        if (gGame.fadeState == 2) {
            goto m2;
        }
        goto body;
    m0:
        a = 0xFF - gGame.fadeLevel;
        goto body;
    m2:
        a = gGame.fadeLevel;
    }
body:
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gSPViewport(gdl++, &gScreenViewport);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
    for (i = 0; i < 64; i++) {
        c = a * 2 + i * 8 - 0x100;
        if (c > 0xFF) {
            c = 0xFF;
        }
        if (c < 0) {
            c = 0;
        }
        w = gScreenWidth / 64;
        x = i * gScreenWidth / 64;
        if (gGame.fadeState == 2) {
            x = gScreenWidth - w - x;
        }
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, c);
        gDPFillRectangle(gdl++, x, 0, x + w, gScreenHeight);
    }
    return gdl;
}
