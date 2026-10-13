#include "ctx.h"

typedef struct {
    u32 pad0[4];
    s32 x0;
    s32 y0;
    s32 z0;
    s32 x1;
    s32 y1;
    s32 z1;
    s32 cnt;
    s32 idx;
    s16 sh30;
    s16 sk[3];
    u8 bt[4];
    Mtx *base;
} Cell;

typedef struct {
    u32 mask;
    u8 pad[0x1C];
} Ent;

typedef struct {
    u8 pad[0x20];
    Ent e[1];
} Hdr;

extern Cell D_800CCB90[];
extern s32 D_800E8090[];
extern Gfx *D_800F0B48[];
extern Hdr *D_800C9644;

void func_80011490(Mtx *m, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);

Gfx *func_800626CC(Gfx *gdl, s32 unused, s32 p3, s32 p4)
{
    s32 i, k;
    s32 *v;
    s32 *tab;
    s32 sh;
    Cell *c;
    Mtx *m;
    Gfx **tbl;
    s32 x, y, z;

    sh = p4 * 2;
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(gdl++, 0x00443078, 0);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);

    i = 0;
    tab = D_800E8090;
    v = tab + (sh + p4);
    for (; i < 2; i++) {
        c = &D_800CCB90[i];
        if ((D_800C9644->e[c->idx].mask >> sh) & 1) {
            k = 0;
            if (c->cnt > 0) {
                tbl = D_800F0B48;
                do {
                    x = c->x0 + (c->x1 - c->x0) * c->sk[k] / 256;
                    z = c->z0 + (c->z1 - c->z0) * c->sk[k] / 256;
                    y = c->y0;
                    m = &c->base[p3 * (c->cnt << 2) + p4 * c->cnt + k];
                    func_80011490(m, 0, 0, 0x8000 - c->sh30, x - v[0], y - v[1], z - v[2], 0x100);
                    gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(gdl++, tbl[c->bt[k]]);
                    k++;
                } while (k < c->cnt);
            }
        }
    }
    return gdl;
}
