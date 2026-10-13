#include "ctx.h"

extern void func_800AE240(void *m, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 s);
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 D_800C23F0;
typedef struct { s32 pad[4]; s32 x, y, w, h; } Scis;
extern Scis D_800CE9C0;
typedef struct { u8 pad[0x174C]; s32 vtx[2]; s32 n; } Obj;
extern Obj *D_800CEA40;
extern Mtx D_800C2678;
extern Vp D_800C25E8;

Gfx *func_80097308(u8 *a0, s32 arg1, Gfx *gdl)
{
    Mtx *m;
    s32 i;

    m = (Mtx *)(a0 + 0x300);
    func_800AE240(m, -D_800F13EC / 2, D_800F13EC / 2, -D_800F13F0 / 2, D_800F13F0 / 2, 50.0f,
                  D_800C23F0, 1.0f);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, m, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &D_800C25E8);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0.x, D_800CE9C0.y,
                  D_800CE9C0.x + D_800CE9C0.w - 1, D_800CE9C0.y + D_800CE9C0.h - 1);
    gSPClearGeometryMode(gdl++, 0x3F0604);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gdl++, 0x00442078, 0);
    for (i = 0; i < D_800CEA40->n; i++) {
        gSPVertex(gdl++, D_800CEA40->vtx[arg1] + i * 0x40, 4, 0);
        gDPPipeSync(gdl++);
        gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        gDPPipeSync(gdl++);
    }
    return gdl;
}
