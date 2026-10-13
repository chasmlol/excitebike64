#include "ctx.h"

typedef struct { u8 w8, h, flags, pad; u32 addr; } Tex;
typedef struct { u32 f0; u16 *pal; Tex tex[1]; } Seg;
typedef struct { u32 f0; Vtx *vtx; u32 f8; u8 *cmd; } Model;

extern Mtx *D_800C8C70;
extern s32 D_800C8C74;
extern s32 D_800C8C78;
extern s32 D_800C8C7C;
extern Gfx *(*D_800C8C80)(Gfx *, s32, s32);
extern void (*D_800C8C84)(u32 *);
extern s32 D_800E2038;
extern u32 D_800E36D0;

#define TILEBITS(s3, fp, t4) \
    ((((s3) & 2) ? G_TX_CLAMP : G_TX_WRAP) << 18 | ((fp) & 0xF) << 14 | \
     (((s3) & 8) ? G_TX_MIRROR : ((s3) & 1) ? G_TX_CLAMP : G_TX_WRAP) << 8 | ((t4) & 0xF) << 4)

#define CMS(b) (((b) & 8) ? G_TX_MIRROR : ((b) & 1) ? G_TX_CLAMP : G_TX_WRAP)
#define CMT(b) (((b) & 2) ? G_TX_CLAMP : G_TX_WRAP)

Gfx *func_80013D70(s32 flags, Model *mdl, Seg **segs, Gfx *gdl) {
    u32 addr;
    Vtx *vtx;
    s32 blk, done;
    u8 c0;
    s32 vbase, mtx, h, rel;
    u8 *cmd;
    u8 a, b, c;
    s32 seg, w, fp, t4;
    u32 tl;
    s32 pl;

    mtx = 0;
    cmd = mdl->cmd;
    vtx = mdl->vtx;
    if (flags & 8) D_800E36D0 = -1;
    done = 0;
    rel = flags & 0x20;
    do {
        c0 = cmd[0];
        a = cmd[1];
        b = cmd[2];
        c = cmd[3];
        switch (c0 >> 4) {
        case 5:
            vbase = 0;
            cmd += 4;
            break;
        case 3:
            if ((cmd[4] >> 4) == 3) {
                gSP2Triangles(gdl++, a, b, c, 0, cmd[5], cmd[6], cmd[7], 0);
                cmd += 8;
            } else {
                gSP1Triangle(gdl++, a, b, c, 0);
                cmd += 4;
            }
            break;
        case 1:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, 0);
            cmd += 4;
            break;
        case 4:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, vbase);
            vbase += a;
            cmd += 4;
            break;
        case 6:
            if (D_800C8C70 != 0) {
                gSPMatrix(gdl++, &D_800C8C70[cmd[1]], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                mtx = 1;
            }
            cmd += 4;
            break;
        case 2:
            if (!(flags & 2)) {
                t4 = c & 0xF;
                seg = b >> 6;
                fp = c >> 4;
                addr = segs[seg]->tex[a].addr;
                blk = (b >> 2) & 1;
                w = segs[seg]->tex[a].w8 * 8;
                h = segs[seg]->tex[a].h;
                if (segs[seg]->tex[a].flags & 2) {
                    if (D_800C8C7C != 1) {
                        if (D_800C8C80 != NULL) gdl = D_800C8C80(gdl, 1, c);
                        D_800C8C7C = 1;
                    }
                } else if (D_800C8C7C != 0) {
                    if (D_800C8C80 != NULL) gdl = D_800C8C80(gdl, ((u32)&cmd[1] & 1) >> 1, c);
                    D_800C8C7C = 0;
                }
                addr = segs[seg]->tex[a].addr;
                if (rel) addr = ((addr - (u32)segs[seg]) & 0xFFFFFF) | ((seg + 2) << 24);
                if (D_800C8C84 != NULL) D_800C8C84(&addr);
                t4 -= D_800C8C78;
                fp -= D_800C8C78;
                pl = c0 & 0xF;
                if ((flags & 4) && seg != D_800E36D0) {
                    u32 pv = (u32)segs[seg]->pal;
                    if (pv != 0) {
                        tl = pv;
                        if (rel) { tl -= (u32)segs[seg]; tl &= 0xFFFFFF; tl |= (seg + 2) << 24; }
                        else if (flags & 0x40) tl = (seg + 1) << 24;
                        gDPLoadTLUT_pal256(gdl++, tl);
                    }
                }
                D_800E36D0 = seg;
                if (blk == 0) {
                    gDPLoadTextureBlock_4b(gdl++, addr, G_IM_FMT_CI, w, h, pl, CMS(b), CMT(b), t4, fp, 0, 0);
                } else {
                    gDPLoadTextureBlock(gdl++, addr, D_800C8C74 ? G_IM_FMT_IA : G_IM_FMT_CI, G_IM_SIZ_8b, w, h, 0,
                                        CMS(b), CMT(b), t4, fp, 0, 0);
                    if (D_800C8C74 != 0 && segs[seg]->pal != NULL) {
                        s32 col = segs[seg]->pal[a];
                        s32 r, g, bb;
                        r = (col >> 8) & 0xF8; g = (col >> 3) & 0xF8; bb = (col << 2) & 0xF8;
                        gDPPipeSync(gdl++);
                        gDPSetPrimColor(gdl++, 0, 0, r, g, bb, 0xFF);
                    }
                }
                if (segs[seg]->tex[a].flags & 8) {
                    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, (-(D_800E2038 * 2) & 0xFF) >> D_800C8C78, 0, 0);
                }
            }
            cmd += 4;
            break;
        case 0:
            done = 1;
            break;
        }
    } while (!done);
    if (mtx) gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    if (flags & 1) gSPEndDisplayList(gdl++);
    return gdl;
}
