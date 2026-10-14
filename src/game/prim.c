#include "game.h"

#define CMS(b) (((b) & 8) ? G_TX_MIRROR : ((b) & 1) ? G_TX_CLAMP : G_TX_WRAP)
#define CMT(b) (((b) & 2) ? G_TX_CLAMP : G_TX_WRAP)

Gfx *prim_build_dl(s32 flags, PrimModel *mdl, TexSet **segs, Gfx *gdl) {
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
    cmd = mdl->cmds;
    vtx = mdl->vtx;
    if (flags & 8) sPrimLastPalSeg = -1;
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
            if (gPrimBoneMtx != 0) {
                gSPMatrix(gdl++, &gPrimBoneMtx[cmd[1]], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                mtx = 1;
            }
            cmd += 4;
            break;
        case 2:
            if (!(flags & 2)) {
                t4 = c & 0xF;
                seg = b >> 6;
                fp = c >> 4;
                addr = (u32)segs[seg]->tex[a].data;
                blk = (b >> 2) & 1;
                w = segs[seg]->tex[a].width * 8;
                h = segs[seg]->tex[a].height;
                if (segs[seg]->tex[a].flags & 2) {
                    if (gPrimTexAlphaMode != 1) {
                        if (sPrimTexModeCallback != NULL) gdl = sPrimTexModeCallback(gdl, 1, c);
                        gPrimTexAlphaMode = 1;
                    }
                } else if (gPrimTexAlphaMode != 0) {
                    if (sPrimTexModeCallback != NULL) gdl = sPrimTexModeCallback(gdl, ((u32)&cmd[1] & 1) >> 1, c);
                    gPrimTexAlphaMode = 0;
                }
                addr = (u32)segs[seg]->tex[a].data;
                if (rel) addr = ((addr - (u32)segs[seg]) & 0xFFFFFF) | ((seg + 2) << 24);
                if (sPrimTexAddrCallback != NULL) sPrimTexAddrCallback(&addr);
                t4 -= sPrimTexSizeShift;
                fp -= sPrimTexSizeShift;
                pl = c0 & 0xF;
                if ((flags & 4) && seg != sPrimLastPalSeg) {
                    u32 pv = (u32)segs[seg]->pal;
                    if (pv != 0) {
                        tl = pv;
                        if (rel) { tl -= (u32)segs[seg]; tl &= 0xFFFFFF; tl |= (seg + 2) << 24; }
                        else if (flags & 0x40) tl = (seg + 1) << 24;
                        gDPLoadTLUT_pal256(gdl++, tl);
                    }
                }
                sPrimLastPalSeg = seg;
                if (blk == 0) {
                    gDPLoadTextureBlock_4b(gdl++, addr, G_IM_FMT_CI, w, h, pl, CMS(b), CMT(b), t4, fp, 0, 0);
                } else {
                    gDPLoadTextureBlock(gdl++, addr, gPrimIATextures ? G_IM_FMT_IA : G_IM_FMT_CI, G_IM_SIZ_8b, w, h, 0,
                                        CMS(b), CMT(b), t4, fp, 0, 0);
                    if (gPrimIATextures != 0 && segs[seg]->pal != NULL) {
                        s32 col = segs[seg]->pal[a];
                        s32 r, g, bb;
                        r = (col >> 8) & 0xF8; g = (col >> 3) & 0xF8; bb = (col << 2) & 0xF8;
                        gDPPipeSync(gdl++);
                        gDPSetPrimColor(gdl++, 0, 0, r, g, bb, 0xFF);
                    }
                }
                if (segs[seg]->tex[a].flags & 8) {
                    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, (-(gFrameCounter * 2) & 0xFF) >> sPrimTexSizeShift, 0, 0);
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

/* Rotates each animated group of textures (consecutive entries with rate bits, ended by the first without) by one
   frame when `frame` is a multiple of the group's rate; with IA textures the palette entries rotate with them. */
void prim_animate_textures(TexSet *set, s32 frame)
{
    u32 i;
    s32 k;
    s32 end;
    u32 idx;
    s32 ia;
    s32 *rates;
    u16 col;
    u8 *data;

    if (set->count == 0) {
        return;
    }
    i = 0;
    rates = sTexAnimRates;
    ia = gPrimIATextures;
    do {
        k = set->tex[i].flags & 0x34;
        if (k != 0) {
            idx = ((u32)k >> 5) << 2;
            if (k & 0x10) {
                idx = idx | 2;
            }
            k = (((u32)k >> 2) & 1) | idx;
            k = rates[k];
            if (frame % k == 0) {
                end = i + 1;
                while (set->tex[end].flags & 0x34) {
                    end++;
                }
                if (ia != 0) {
                    col = set->pal[i];
                    for (k = i; k < end; k++) {
                        set->pal[k] = set->pal[k + 1];
                    }
                    set->pal[end] = col;
                }
                data = set->tex[i].data;
                for (k = i; k < end; k++) {
                    set->tex[k].data = set->tex[k + 1].data;
                }
                set->tex[end].data = data;
                i = end;
            }
        }
        i = i + 1;
    } while (i < set->count);
}

static inline void flip(u8 *pa, s32 row, s32 h)
{
    s32 j, k;
    u8 *pb;
    u8 tmp;

    pb = pa + row * (h - 1);
    for (j = 0; j < h / 2; j++) {
        for (k = 0; k < row; k++) {
            tmp = *pa;
            *pa++ = *pb;
            *pb++ = tmp;
        }
        pb -= row * 2;
    }
}

void prim_flip_images(TexSet *set)
{
    u32 i;

    for (i = 0; i < set->count; i++) {
        if (!(set->tex[i].flags & 1)) {
            flip(set->tex[i].data, set->tex[i].width * 4, set->tex[i].height);
        } else {
            flip(set->tex[i].data, set->tex[i].width * 8, set->tex[i].height);
        }
    }
}

void prim_set_tex_mode_callback(s32 a0)
{
    sPrimTexModeCallback = a0;
}

void prim_set_tex_addr_callback(s32 a0)
{
    sPrimTexAddrCallback = a0;
}

void prim_init(void)
{
    res_register_type(sStrPrimPkg, prim_pkg_relocate);
    res_register_type(sStrPrimN64, prim_n64_relocate);
    sPrimLastPalSeg = -1;
    sPrimTexSizeShift = 0;
}

void prim_exit(void)
{
}

void prim_set_tex_size_shift(s32 x)
{
    sPrimTexSizeShift = x;
}

/* Builds a model's display list into a buffer of its size, with texture addresses relative to the sets' segments
   and the vertices in segment 1. */
void prim_alloc_seg_dl(Gfx **out, PrimModel *mdl, TexSet **sets, u32 flags)
{
    u32 r;
    Vtx *vtx;

    r = (u32)prim_build_dl(flags | 0x21, mdl, sets, (Gfx *)gZBuffer);
    *out = mem_alloc_dbg(r - gZBuffer, 0, sStrPrimC, 0x323);
    vtx = mdl->vtx;
    mdl->vtx = (Vtx *)0x01000000;
    prim_build_dl(flags | 0x21, mdl, sets, *out);
    mdl->vtx = vtx;
}

/* Builds a model's display list into a buffer of its size. */
void prim_alloc_dl(Gfx **out, PrimModel *mdl, TexSet **sets, u32 flags)
{
    u32 r;

    r = (u32)prim_build_dl(flags | 1, mdl, sets, (Gfx *)gZBuffer);
    r = mem_alloc_dbg(r - gZBuffer, 0, sStrPrimC, 0x33B);
    *out = (Gfx *)r;
    prim_build_dl(flags | 1, mdl, sets, (Gfx *)r);
}

void prim_flip_image_rows(u8 *p, s32 stride, s32 rows) {
    u8 *q = p + stride * (rows - 1);
    s32 i;
    s32 j;
    for (i = 0; i < rows / 2; i++) {
        for (j = 0; j < stride; j++) {
            u8 t = *p;
            *p = *q;
            p++;
            *q = t;
            q++;
        }
        q -= stride * 2;
    }
}

void prim_pkg_unrelocate(TexSet **pp)
{
    TexSet *set = *pp;
    u32 i;

    if (set->pal != NULL) {
        set->pal = (u16 *)((u32)set->pal - (u32)set);
    }
    for (i = 0; i < set->count; i++) {
        set->tex[i].data = (u8 *)((u32)set->tex[i].data - (u32)set);
    }
}

void prim_pkg_relocate(TexSet **pp)
{
    TexSet *set = *pp;
    u32 i;

    set = (TexSet *)((u32)set & ~2);
    if (set->pal != NULL) {
        set->pal = (u16 *)((u32)set->pal + (u32)set);
    }
    for (i = 0; i < set->count; i++) {
        set->tex[i].data = (u8 *)((u32)set->tex[i].data + (u32)set);
    }
}

void prim_n64_relocate(PrimModel **pp)
{
    PrimModel *mdl = (PrimModel *)((u32)*pp & 0xFFFFFFFC);

    mdl->vtx = (Vtx *)((u32)mdl->vtx + (u32)mdl);
    mdl->cmds = (u8 *)((u32)mdl->cmds + (u32)mdl);
}

s32 prim_find_nearest_color(u16 *p, s32 r, s32 g, s32 b, s32 flag, s32 n)
{
    s32 best = 99999999;
    s32 bi = 0;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        if (flag == 0) {
            if ((*p & 1) == 0) {
                return i;
            }
        } else {
            if ((*p & 1) == 0) {
                continue;
            }
        }
        {
            u32 c = *p;
            s32 t;
            s32 d;
            t = (s32)(c >> 11) - r;
            d = t * t;
            t = (s32)((c >> 6) & 0x1F) - g;
            t = t * t;
            d += t;
            t = (s32)((c >> 1) & 0x1F) - b;
            t = t * t;
            d += t;
            if (d < best) {
                best = d;
                bi = i;
            }
        }
    }
    return bi;
}
