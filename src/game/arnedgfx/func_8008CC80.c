#include "ctx.h"

typedef struct {
    u8 w, h;
    u8 pad[2];
    void *img;
} TexInfo;

typedef struct {
    u32 x0;
    void *pal;
    TexInfo tex;
} TexSet;

typedef struct {
    u8 pad[0x170];
    s32 mode;
} GState;

extern GState *D_800CEA40;
extern TexSet *D_800F5578;
extern s32 D_800F4FD4;
extern Vtx *D_800F557C;

Gfx *func_8008CC80(Gfx *gdl)
{
    s32 i, j, n;
    TexSet *s;
    TexInfo *t;

    if (D_800CEA40->mode == 6) {
        return gdl;
    }
    {
        s = D_800F5578;
        gDPLoadTLUT_pal256(gdl++, s->pal);
        t = &s->tex;
        gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b,
                            t->w * 8, t->h, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 6, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0; i < D_800F4FD4; i += 32) {
            n = D_800F4FD4 - i;
            if (n > 32) n = 32;
            gSPVertex(gdl++, &D_800F557C[i], n, 0);
            for (j = 0; j < n / 4; j++) {
                gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
            }
        }
    }
    return gdl;
}
