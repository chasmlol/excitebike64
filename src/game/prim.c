#include "game.h"

/* The wrap bits of a texture command's third byte: s and t clamp, wrap or mirror. */
#define CMS(b) (((b) & 8) ? G_TX_MIRROR : ((b) & 1) ? G_TX_CLAMP : G_TX_WRAP)
#define CMT(b) (((b) & 2) ? G_TX_CLAMP : G_TX_WRAP)

/* Compiles a model's command stream (see PrimModel) into F3DEX2 display list commands at gdl; flags are PRIM_*.
   sets are the texture sets the texture commands index. Returns the end of the commands. */

Gfx *prim_build_dl(s32 flags, PrimModel *model, TexSet **sets, Gfx *gdl) {
    u32 addr;
    Vtx *vtx;
    s32 is8bit, done;
    u8 op;
    s32 vtxBase, boneLoaded, height, segmented;
    u8 *cmd;
    u8 a, b, c;
    s32 set, width, maskT, maskS;
    u32 tlut;
    s32 palette;

    boneLoaded = 0;
    cmd = model->cmds;
    vtx = model->vtx;
    if (flags & PRIM_RESET_TLUT) sPrimLastPalSeg = -1;
    done = 0;
    segmented = flags & PRIM_SEGMENTED;
    do {
        /* the opcode in the high nibble, then three argument bytes */
        op = cmd[0];
        a = cmd[1];
        b = cmd[2];
        c = cmd[3];
        switch (op >> 4) {
        case PRIM_CMD_VTX_RESET:
            vtxBase = 0;
            cmd += 4;
            break;
        case PRIM_CMD_TRI:
            if ((cmd[4] >> 4) == PRIM_CMD_TRI) {
                gSP2Triangles(gdl++, a, b, c, 0, cmd[5], cmd[6], cmd[7], 0);
                cmd += 8;
            } else {
                gSP1Triangle(gdl++, a, b, c, 0);
                cmd += 4;
            }
            break;
        case PRIM_CMD_VTX:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, 0);
            cmd += 4;
            break;
        case PRIM_CMD_VTX_APPEND:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, vtxBase);
            vtxBase += a;
            cmd += 4;
            break;
        case PRIM_CMD_BONE:
            if (gPrimBoneMtx != 0) {
                gSPMatrix(gdl++, &gPrimBoneMtx[cmd[1]], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                boneLoaded = 1;
            }
            cmd += 4;
            break;
        case PRIM_CMD_TEX:
            /* a: texture index; b: set << 6 | 8-bit << 2 | wrap bits; c: mask t << 4 | mask s */
            if (!(flags & PRIM_NO_TEXTURES)) {
                maskS = c & 0xF;
                set = b >> 6;
                maskT = c >> 4;
                addr = (u32)sets[set]->tex[a].data;
                is8bit = (b >> 2) & 1;
                width = sets[set]->tex[a].width * 8;
                height = sets[set]->tex[a].height;
                if (sets[set]->tex[a].flags & TEX_CUTOUT) {
                    if (gPrimTexAlphaMode != 1) {
                        if (sPrimTexModeCallback != NULL) gdl = sPrimTexModeCallback(gdl, 1, c);
                        gPrimTexAlphaMode = 1;
                    }
                } else if (gPrimTexAlphaMode != 0) {
                    /* the argument is 0, written as an expression of the command's address in the original */
                    if (sPrimTexModeCallback != NULL) gdl = sPrimTexModeCallback(gdl, ((u32)&cmd[1] & 1) >> 1, c);
                    gPrimTexAlphaMode = 0;
                }
                addr = (u32)sets[set]->tex[a].data;
                if (segmented) addr = ((addr - (u32)sets[set]) & 0xFFFFFF) | ((set + 2) << 24);
                if (sPrimTexAddrCallback != NULL) sPrimTexAddrCallback(&addr);
                maskS -= sPrimTexSizeShift;
                maskT -= sPrimTexSizeShift;
                palette = op & 0xF;
                if ((flags & PRIM_LOAD_TLUT) && set != sPrimLastPalSeg) {
                    u32 pal = (u32)sets[set]->pal;
                    if (pal != 0) {
                        tlut = pal;
                        if (segmented) { tlut -= (u32)sets[set]; tlut &= 0xFFFFFF; tlut |= (set + 2) << 24; }
                        else if (flags & PRIM_TLUT_SEGMENT) tlut = (set + 1) << 24;
                        gDPLoadTLUT_pal256(gdl++, tlut);
                    }
                }
                sPrimLastPalSeg = set;
                if (is8bit == 0) {
                    gDPLoadTextureBlock_4b(gdl++, addr, G_IM_FMT_CI, width, height, palette, CMS(b), CMT(b), maskS, maskT, 0, 0);
                } else {
                    gDPLoadTextureBlock(gdl++, addr, gPrimIATextures ? G_IM_FMT_IA : G_IM_FMT_CI, G_IM_SIZ_8b, width, height, 0,
                                        CMS(b), CMT(b), maskS, maskT, 0, 0);
                    /* IA textures take their colour from the palette entry of the same index */
                    if (gPrimIATextures != 0 && sets[set]->pal != NULL) {
                        s32 col = sets[set]->pal[a];
                        s32 r, g, bl;
                        r = (col >> 8) & 0xF8; g = (col >> 3) & 0xF8; bl = (col << 2) & 0xF8;
                        gDPPipeSync(gdl++);
                        gDPSetPrimColor(gdl++, 0, 0, r, g, bl, 0xFF);
                    }
                }
                if (sets[set]->tex[a].flags & TEX_SCROLL) {
                    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, (-(gFrameCounter * 2) & 0xFF) >> sPrimTexSizeShift, 0, 0);
                }
            }
            cmd += 4;
            break;
        case PRIM_CMD_END:
            done = 1;
            break;
        }
    } while (!done);
    if (boneLoaded) gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    if (flags & PRIM_END_DL) gSPEndDisplayList(gdl++);
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
        k = set->tex[i].flags & TEX_ANIM_RATE;
        if (k != 0) {
            idx = ((u32)k >> 5) << 2;
            if (k & 0x10) {
                idx = idx | 2;
            }
            /* bits 2, 4, 5 of the flags make a 3-bit index into sTexAnimRates */
            k = (((u32)k >> 2) & 1) | idx;
            k = rates[k];
            if (frame % k == 0) {
                end = i + 1;
                while (set->tex[end].flags & TEX_ANIM_RATE) {
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

/* Turns an image upside down: rows of rowBytes bytes. */
static inline void flip(u8 *top, s32 rowBytes, s32 height)
{
    s32 j, k;
    u8 *bottom;
    u8 tmp;

    bottom = top + rowBytes * (height - 1);
    for (j = 0; j < height / 2; j++) {
        for (k = 0; k < rowBytes; k++) {
            tmp = *top;
            *top++ = *bottom;
            *bottom++ = tmp;
        }
        bottom -= rowBytes * 2;
    }
}

/* Turns every image of a set upside down. */
void prim_flip_images(TexSet *set)
{
    u32 i;

    for (i = 0; i < set->count; i++) {
        if (!(set->tex[i].flags & TEX_8BIT)) {
            flip(set->tex[i].data, set->tex[i].width * 4, set->tex[i].height);
        } else {
            flip(set->tex[i].data, set->tex[i].width * 8, set->tex[i].height);
        }
    }
}

/* Sets the function prim_build_dl calls when a texture switches between cut-out and opaque. */
void prim_set_tex_mode_callback(s32 func)
{
    sPrimTexModeCallback = func;
}

/* Sets the function prim_build_dl calls to remap each texture address. */
void prim_set_tex_addr_callback(s32 func)
{
    sPrimTexAddrCallback = func;
}

/* Registers the relocation of the "PKG" (texture set) and "N64" (model) resources. */
void prim_init(void)
{
    res_register_type(sStrPrimPkg, prim_pkg_relocate);
    res_register_type(sStrPrimN64, prim_n64_relocate);
    sPrimLastPalSeg = -1;
    sPrimTexSizeShift = 0;
}

/* Nothing to free. */
void prim_exit(void)
{
}

/* Sets how much the texture masks are reduced (lower detail). */
void prim_set_tex_size_shift(s32 shift)
{
    sPrimTexSizeShift = shift;
}

/* Builds a model's display list into a buffer of its size, with texture addresses relative to the sets' segments
   and the vertices in segment 1. */
void prim_alloc_seg_dl(Gfx **out, PrimModel *model, TexSet **sets, u32 flags)
{
    u32 end;
    Vtx *vtx;

    /* built once into the Z buffer to measure it */
    end = (u32)prim_build_dl(flags | PRIM_SEGMENTED | PRIM_END_DL, model, sets, (Gfx *)gZBuffer);
    *out = mem_alloc_dbg(end - gZBuffer, 0, sStrPrimC, 0x323);
    vtx = model->vtx;
    model->vtx = (Vtx *)0x01000000; /* segment 1 */
    prim_build_dl(flags | PRIM_SEGMENTED | PRIM_END_DL, model, sets, *out);
    model->vtx = vtx;
}

/* Builds a model's display list into a buffer of its size. */
void prim_alloc_dl(Gfx **out, PrimModel *model, TexSet **sets, u32 flags)
{
    u32 dl;

    dl = (u32)prim_build_dl(flags | PRIM_END_DL, model, sets, (Gfx *)gZBuffer);
    dl = mem_alloc_dbg(dl - gZBuffer, 0, sStrPrimC, 0x33B);
    *out = (Gfx *)dl;
    prim_build_dl(flags | PRIM_END_DL, model, sets, (Gfx *)dl);
}

/* Turns an image of `rows` rows of `stride` bytes upside down. */
void prim_flip_image_rows(u8 *top, s32 stride, s32 rows) {
    u8 *bottom = top + stride * (rows - 1);
    s32 i;
    s32 j;
    for (i = 0; i < rows / 2; i++) {
        for (j = 0; j < stride; j++) {
            u8 tmp = *top;
            *top = *bottom;
            top++;
            *bottom = tmp;
            bottom++;
        }
        bottom -= stride * 2;
    }
}

/* Makes a texture set's pointers offsets again (to copy it). */
void prim_pkg_unrelocate(TexSet **handle)
{
    TexSet *set = *handle;
    u32 i;

    if (set->pal != NULL) {
        set->pal = (u16 *)((u32)set->pal - (u32)set);
    }
    for (i = 0; i < set->count; i++) {
        set->tex[i].data = (u8 *)((u32)set->tex[i].data - (u32)set);
    }
}

/* "PKG" resources: turns a texture set's offsets into pointers. */
void prim_pkg_relocate(TexSet **handle)
{
    TexSet *set = *handle;
    u32 i;

    set = (TexSet *)((u32)set & ~2); /* the handle's load-buffer bit */
    if (set->pal != NULL) {
        set->pal = (u16 *)((u32)set->pal + (u32)set);
    }
    for (i = 0; i < set->count; i++) {
        set->tex[i].data = (u8 *)((u32)set->tex[i].data + (u32)set);
    }
}

/* "N64" resources: turns a model's offsets into pointers. */
void prim_n64_relocate(PrimModel **handle)
{
    PrimModel *model = (PrimModel *)((u32)*handle & 0xFFFFFFFC);

    model->vtx = (Vtx *)((u32)model->vtx + (u32)model);
    model->cmds = (u8 *)((u32)model->cmds + (u32)model);
}

/* The index of the palette colour nearest to (r, g, b) (5 bits each) among the opaque ones; with opaque 0, a
   transparent entry as soon as one comes. */
s32 prim_find_nearest_color(u16 *pal, s32 r, s32 g, s32 b, s32 opaque, s32 count)
{
    s32 bestDist = 99999999;
    s32 bestIdx = 0;
    s32 i;

    for (i = 0; i < count; i++, pal++) {
        if (opaque == 0) {
            if ((*pal & 1) == 0) {
                return i;
            }
        } else {
            if ((*pal & 1) == 0) {
                continue;
            }
        }
        {
            u32 col = *pal;
            s32 diff;
            s32 dist;
            diff = (s32)(col >> 11) - r;
            dist = diff * diff;
            diff = (s32)((col >> 6) & 0x1F) - g;
            diff = diff * diff;
            dist += diff;
            diff = (s32)((col >> 1) & 0x1F) - b;
            diff = diff * diff;
            dist += diff;
            if (dist < bestDist) {
                bestDist = dist;
                bestIdx = i;
            }
        }
    }
    return bestIdx;
}
