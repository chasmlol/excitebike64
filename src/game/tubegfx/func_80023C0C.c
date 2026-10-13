#include "ctx.h"

typedef struct Ent {
    u32 pad[8];
} Ent;

typedef struct List {
    u32 n;
    u32 pad[7];
    struct { u32 flags; u32 pad[7]; } e[1];
} List;

extern s32 D_800E8000[];
extern u8 D_800E7730[];
extern Vp *D_800E19C0;
extern s32 D_800E1898;
extern u16 *D_800EFD64;
extern List *D_800C9640;
extern u8 D_800C9588[];
extern u8 D_800C95A0[];
extern Gfx **D_800E7828;
extern u8 D_800DC0E0[];

void func_80011A58(s32 *p1, s32 p2, s32 p3, s32 p4);
void func_80012DC8(s32 *p1, s32 *p2, s32 *p3);
void func_80011490(void *p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8);
Gfx *func_800281F4(Gfx *p1, s32 p2);

Gfx *func_80023C0C(Gfx *gdl, s32 a1, s32 a2, s32 mode)
{
    s32 pos[4];
    s32 out[3];
    s32 mtx[10];
    u16 *fog;
    u32 i;

    func_80011A58(mtx, 0, 0, D_800E8000[mode]);
    pos[0] = 0;
    pos[1] = -30;
    pos[2] = 0;
    func_80012DC8(pos, mtx, out);
    func_80011490(D_800E7730, 0, 0, 0, out[0], out[1], out[2], 0x100);

    gSPViewport(gdl++, &D_800E19C0[mode + 4]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, 0x200004);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    fog = (u16 *)((u8 *)D_800EFD64 + ((D_800E1898 - 1) << 1));
    gSPFogPosition(gdl++, fog[0x2C], fog[0x30]);
    gSPSegment(gdl++, 6, D_800C9588);
    gSPSegment(gdl++, 7, D_800C95A0);

    for (i = 0; i < D_800C9640->n; i++) {
        if ((D_800C9640->e[i].flags & (1 << (mode * 2))) && (D_800C9640->e[i].flags & (2 << (mode * 2)))) {
            gdl = func_800281F4(gdl, i);
            gSPDisplayList(gdl++, D_800E7828[i]);
        }
    }

    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}
