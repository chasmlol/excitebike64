#include "game.h"

typedef struct {
    s32 flags;
    u8 pad4[0x50];
    s32 mode;
    s16 f58;
} State_6;

extern State_6 gGame_52;

Gfx *screen_draw_wipe(Gfx *gdl, Dyn *arg1, s32 arg2)
{
    s32 a;
    s32 i;
    s32 c;
    s32 w;
    s32 x;

    if (gGame_52.flags & 0x10000000) {
        a = 0xFF;
    } else {
        if (gGame_52.mode == 1) {
            return gdl;
        }
        if (gGame_52.mode == 0) {
            goto m0;
        }
        if (gGame_52.mode == 2) {
            goto m2;
        }
        goto body;
    m0:
        a = 0xFF - gGame_52.f58;
        goto body;
    m2:
        a = gGame_52.f58;
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
        if (gGame_52.mode == 2) {
            x = gScreenWidth - w - x;
        }
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, c);
        gDPFillRectangle(gdl++, x, 0, x + w, gScreenHeight);
    }
    return gdl;
}
