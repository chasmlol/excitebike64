#include "ctx.h"

typedef struct {
    s32 flags;
    u8 pad4[0x50];
    s32 mode;
    s16 f58;
} State;

extern State D_800EFD70;
extern Vp D_800C25E8;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

Gfx *func_80072F70(Gfx *gdl)
{
    s32 a;
    s32 i;
    s32 c;
    s32 w;
    s32 x;

    if (D_800EFD70.flags & 0x10000000) {
        a = 0xFF;
    } else {
        if (D_800EFD70.mode == 1) {
            return gdl;
        }
        if (D_800EFD70.mode == 0) {
            goto m0;
        }
        if (D_800EFD70.mode == 2) {
            goto m2;
        }
        goto body;
    m0:
        a = 0xFF - D_800EFD70.f58;
        goto body;
    m2:
        a = D_800EFD70.f58;
    }
body:
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPViewport(gdl++, &D_800C25E8);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
    for (i = 0; i < 64; i++) {
        c = a * 2 + i * 8 - 0x100;
        if (c > 0xFF) {
            c = 0xFF;
        }
        if (c < 0) {
            c = 0;
        }
        w = D_800F13EC / 64;
        x = i * D_800F13EC / 64;
        if (D_800EFD70.mode == 2) {
            x = D_800F13EC - w - x;
        }
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, c);
        gDPFillRectangle(gdl++, x, 0, x + w, D_800F13F0);
    }
    return gdl;
}
