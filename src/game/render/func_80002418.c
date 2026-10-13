#include "ctx.h"

typedef struct {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} Rect;

extern Rect D_800E1718[][4];
extern Vp *D_800E19C0;
extern s32 D_800C2594;
extern s32 D_800E1A9C;

Gfx *func_80002418(Gfx *dl, Mtx *m, s32 n, s32 flag)
{
    Rect *r;
    s32 i;
    u8 *tab;

    if (n == D_800E1A9C) {
        return dl;
    }
    tab = (u8 *)D_800E1718;
    r = (Rect *)(tab + (n * 16 + (D_800C2594 - 1) * 64));
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, r->x, r->y, r->x + r->w, r->y + r->h);
    if (flag) {
        gSPMatrix(dl++, &m[n], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    gSPViewport(dl++, &D_800E19C0[n]);
    D_800E1A9C = n;
    return dl;
}
