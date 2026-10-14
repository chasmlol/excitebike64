#include "game.h"

void arned_add_wall_quad(s32 x, s32 z, s32 w, s32 h)
{
    sArnedWallVtx[sArnedWallVtxCount].ob[0] = x;
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = 0;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0;
    sArnedWallVtxCount++;
    sArnedWallVtx[sArnedWallVtxCount].ob[0] = x + w;
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = 0;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z + h;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0x800;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0;
    sArnedWallVtxCount++;
    if (w != 0) {
        z += w / 2;
        x = (x * sArnedGridW + 1) / sArnedGridW;
    }
    if (h != 0) {
        x -= h / 2;
        z = (z * sArnedGridH + 1) / sArnedGridH;
    }
    sArnedWallVtx[sArnedWallVtxCount].ob[0] = x;
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = 500;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0x1000;
    sArnedWallVtxCount++;
    sArnedWallVtx[sArnedWallVtxCount].ob[0] = x + w;
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = 500;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z + h;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0x800;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0x1000;
    sArnedWallVtxCount++;
}

Gfx *arned_draw_walls(Gfx *gdl)
{
    s32 i, j, n;
    TexSet *s;
    TexEntry *t;

    if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        return gdl;
    }
    {
        s = sArnedWallTex;
        gDPLoadTLUT_pal256(gdl++, s->pal);
        t = &s->tex[0];
        gDPLoadTextureBlock(gdl++, t->data, G_IM_FMT_IA, G_IM_SIZ_8b,
                            t->width * 8, t->height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 6, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0; i < sArnedWallVtxCount; i += 32) {
            n = sArnedWallVtxCount - i;
            if (n > 32) n = 32;
            gSPVertex(gdl++, &sArnedWallVtx[i], n, 0);
            for (j = 0; j < n / 4; j++) {
                gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
            }
        }
    }
    return gdl;
}

void arned_init_grid(s32 w, s32 h)
{
    s32 size;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 nx;
    s32 ny;
    s32 hh;
    u8 *p;

    if (sArnedCells != NULL) {
        arned_free_grid();
    }
    sArnedObjViews = mem_alloc_dbg(8 * sizeof(ArnedObjView), 0, (s32)sStrArnedgfxC, 0xD4);
    size = w * ((h << 2) + h << 2);
    sArnedCells = (ArnedCell *)(p = mem_alloc_dbg(size, 0, (s32)sStrArnedgfxC, 0xD7));
    if (p == NULL) {
        p = fatal_halt_2(1);
    }
    sArnedGridW = w;
    sArnedGridH = h;
    mem_fill(p, 0, size);
    n = sArnedGridW * sArnedGridH;
    for (i = 0; i < n; i++) {
        sArnedCells[i].slot = -1;
    }
    n = -1;
    for (i = 0x1C1; i >= 0; i--) {
        sArnedSlotOwner[i] = n;
    }
    sArnedVtxBufs = mem_alloc_dbg(0x708, 0, (s32)sStrArnedgfxC, 0xE4);
    for (i = 0; i < 0x1C2; i++) {
        p = mem_alloc_dbg(0x750, 0, (s32)sStrArnedgfxC, 0xE7);
        sArnedVtxBufs[i] = (Vtx_t *)p;
        mem_fill(p, 0, 0x750);
    }
    hh = h;
    do {
        Vtx_t **pp = &sArnedFloorVtx;
        nx = w + 1;
        ny = hh + 1;
        *pp = mem_alloc_dbg(nx * (ny << 4), 0, (s32)sStrArnedgfxC, 0xEF);
        i = 0;
    } while (0);
    k = 0;
    if (ny > 0) {
        do {
            for (j = 0; j < w + 1; j++) {
                sArnedFloorVtx[k].ob[0] = j * 500 - w * 250;
                sArnedFloorVtx[k].ob[1] = 0;
                sArnedFloorVtx[k].ob[2] = i * 500 - hh * 250;
                k++;
            }
        } while (++i < hh + 1);
    }
    i = 0;
    mem_fill((u8 *)sArnedObjViews, 0, 8 * sizeof(ArnedObjView));
    sArnedWallVtx = mem_alloc_dbg((sArnedGridW + sArnedGridH) * 0x80, 0, (s32)sStrArnedgfxC, 0x100);
    sArnedWallVtxCount = 0;
    for (i = 0; i < sArnedGridW; i++) {
        arned_add_wall_quad(i * 500 - sArnedGridW * 250, sArnedGridH * 250, 500, 0);
        arned_add_wall_quad(sArnedGridW * 250 - i * 500, -(sArnedGridH * 250), -500, 0);
    }
    for (i = 0; i < sArnedGridH; i++) {
        arned_add_wall_quad(-(sArnedGridW * 250), i * 500 - sArnedGridH * 250, 0, 500);
        arned_add_wall_quad(sArnedGridW * 250, sArnedGridH * 250 - i * 500, 0, -500);
    }
    sArnedTexSetIdx = -1;
}

void arned_set_cell(s32 x, s32 y, PrimModel *piece, s32 rot, PrimModel *piece2, s32 rot2, s32 flags, s32 dx, s32 dz)
{
    s32 idx = sArnedGridW * y + x;

    if ((flags & 2) || sArnedCells[idx].piece != piece || sArnedCells[idx].piece2 != piece2 ||
        sArnedCells[idx].rot != rot || sArnedCells[idx].rot2 != rot2 || sArnedCells[idx].flags != flags ||
        sArnedCells[idx].dx != dx || sArnedCells[idx].dz != dz) {
        sArnedCells[idx].dirty = 1;
    }
    sArnedCells[idx].piece = piece;
    sArnedCells[idx].piece2 = piece2;
    sArnedCells[idx].rot = rot;
    sArnedCells[idx].rot2 = rot2;
    sArnedCells[idx].flags = flags & ~2;
    sArnedCells[idx].dx = dx;
    sArnedCells[idx].dz = dz;
}

void arned_set_camera(s32 idx, s32 x, s32 y, s32 z, s16 rx, s16 ry)
{
    Mtx persp;
    Mtx rot;
    Mtx a;
    Mtx b;
    Mtx c;
    f32 q;
    s32 f1, f2, f3;

    sArnedCamX = x;
    gFarClip = 0x7FBC;
    sArnedCamY = y;
    sArnedCamZ = z;
    sArnedCamYaw = rx;
    sArnedCamPitch = ry;
    f1 = gScreenHeight;
    f2 = gScreenHeight;
    f3 = gScreenHeight;
    q = (f32)f1 * 0.5f * gCosTable[0] / gCosTable[0xC00];
    *(volatile f32 *)&gProjScale = q;
    gProjScale = q * f2 * 0.00390625f;
    guPerspective(&persp, &gPerspNorm, 33.0f, 320.0f / f3, 50.0f, gFarClip, 1.0f);
    guRotateRPY(&rot, sArnedCamPitch * 360.0f * 1.5258789e-05f, -(f32)sArnedCamYaw * 360.0f * 1.5258789e-05f, 0.0f);
    mtx_transpose_rot((u8 *)&rot);
    mtx_from_angles_pos_scale(&a, 0, 0, 0, -sArnedCamX, -sArnedCamY, -sArnedCamZ, 0x100);
    mtx_mul(&a, &rot, &b);
    mtx_mul(&b, &gViewAxisMtx, &a);
    mtx_mul(&a, &persp, &c);
    gArnedCamMtx[idx] = c;
}

#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]

Gfx *arned_draw(s32 idx, Gfx *gdl, s32 selX, s32 selY, s32 selW, s32 selH, s32 cursorX, s32 cursorZ, s32 cursorRot,
                 s32 drawGrid) {
    s32 total;
    s32 x0, z0;
    s32 i, j, x, y, n;
    s32 dx, dz, a;
    s32 r, g, b;
    ArnedCell *cell;
    Vtx_t *out;
    ArnedObjView *os;
    Mtx *m;

    total = sArnedGridW * sArnedGridH;
    for (i = 0; i < 450; i++) {
        if (sArnedSlotOwner[i] == -2) {
            sArnedSlotOwner[i] = -1;
        }
    }
    i = 0;
    x0 = -(sArnedGridW * 250);
    z0 = -(sArnedGridH * 250);
    for (y = 0; y < sArnedGridH; y++) {
        for (x = 0; x < sArnedGridW; x++, i++) {
            cell = &sArnedCells[i];
            if (cell->dirty != 0) {
                if (cell->slot != -1) {
                    sArnedSlotOwner[cell->slot] = -2;
                }
                for (n = 0; n < 450; n++) {
                    if (sArnedSlotOwner[n] == -1) {
                        break;
                    }
                }
                if (cell->piece == NULL && cell->piece2 == NULL) {
                    n = 450;
                }
                if (n == 450) {
                    cell->slot = -1;
                } else {
                    sArnedSlotOwner[n] = i;
                    cell->slot = n;
                    out = sArnedVtxBufs[n];
                    cell->dirty = 0;
                    n = 0;
                    if (cell->piece != NULL && cell->piece->numVtx < 0x76) {
                        arned_transform_piece_verts(cell->piece->vtx, cell->piece->numVtx, cell->rot, out, x0 + x * 500, z0 + y * 500, 1);
                        n = cell->piece->numVtx;
                    }
                    if (cell->piece2 != NULL && cell->piece2->numVtx + n < 0x76) {
                        arned_transform_piece_verts(cell->piece2->vtx, cell->piece2->numVtx, cell->rot2, &out[n], x0 + x * 500 + cell->dx, z0 + y * 500 + cell->dz, 0);
                        n += cell->piece2->numVtx;
                    }
                    for (; n < 0x75; n++) {
                        out[n].flag = 2;
                    }
                }
            }
        }
    }
    dx = gCosTable[(((gFrameCounter << 8) >> 4) - 0x400) & 0xFFF] * 80 / 0x4000;
    dz = gCosTable[((gFrameCounter << 8) >> 4) & 0xFFF] * 80 / 0x4000;
    {
    Lights1 light = gdSPDefLights1(0x28, 0x28, 0x28, 0xFF, 0xFF, 0xFF, dx, 0x7F, dz);
    sArnedLights[idx] = light;
    }
    gSPSetLights1(gdl++, sArnedLights[idx]);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sArnedViewX, sArnedViewY, sArnedViewX + sArnedViewW, sArnedViewY + sArnedViewH);
    gSPViewport(gdl++, &gArnedViewport);
    gSPPerspNormalize(gdl++, gPerspNorm);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &gArnedCamMtx[idx], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    if (drawGrid != 0) {
        gDPSetRenderMode(gdl++, 0x0F0A4000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        for (i = 0; i < total; i++) {
            x = i % sArnedGridW;
            y = i / sArnedGridW;
            n = (x + y) & 1;
            if (n) {
                r = 16;
                g = 16;
                b = 16;
            } else {
                r = g = x * 100 / sArnedGridW + 35;
                b = x * 60 / sArnedGridW + 140;
            }
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, r, g, b, 0xFF);
            gSPVertex(gdl++, &sArnedFloorVtx[y * (sArnedGridW + 1) + x], 2, 0);
            gSPVertex(gdl++, &sArnedFloorVtx[(y + 1) * (sArnedGridW + 1) + x], 2, 2);
            gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    if (drawGrid != 0) {
        gdl = arned_draw_walls(gdl);
    }
    gDPPipeSync(gdl++);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetEnvColor(gdl++, 0xFF, 0x82, 0x82, 0xFF);
    for (i = 0; i < total; i++) {
        cell = &sArnedCells[i];
        if (cell->slot != -1) {
            n = 0;
            if (cell->piece != NULL && cell->piece->numVtx < 0x76) {
                gDPPipeSync(gdl++);
                if (cell->flags & 1) {
                    s32 s;
                    s = gFrameCounter << 10;
                    gDPSetPrimColor(gdl++, 0, 0, (SINS(s) * 25 >> 13) + 128, (SINS(s) * 25 >> 13) + 128, (SINS(s) * 127 >> 14) + 128, 0xFF);
                    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
                    gSPClearGeometryMode(gdl++, 0x220004);
                } else {
                    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
                    gSPSetGeometryMode(gdl++, 0x220004);
                }
                out = cell->piece->vtx;
                cell->piece->vtx = sArnedVtxBufs[cell->slot];
                gdl = prim_build_dl(12, cell->piece, &sArnedPieceTex, gdl);
                cell->piece->vtx = out;
                n = cell->piece->numVtx;
            }
            if (cell->piece2 != NULL && (gEditor->mode != 6 || !(cell->flags & 1)) && cell->piece2->numVtx + n < 0x76) {
                gDPPipeSync(gdl++);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
                gSPClearGeometryMode(gdl++, 0x220004);
                out = cell->piece2->vtx;
                cell->piece2->vtx = sArnedVtxBufs[cell->slot] + n;
                gdl = prim_build_dl(12, cell->piece2, &sArnedPieceTex2, gdl);
                cell->piece2->vtx = out;
            }
        }
    }
    if (cursorRot != -1 && gEditor->mode != 6) {
        gDPPipeSync(gdl++);
        m = &sArnedCursorMtx[idx];
        mtx_from_angles_pos_scale(m, 0, 0, cursorRot + 0x4000, cursorX, 2500, cursorZ, 0x100);
        gSPMatrix(gdl++, m, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPSetRenderMode(gdl++, 0x504A50, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPPipeSync(gdl++);
        gdl = prim_build_dl(12, gEditor->cursorModel, &gEditor->cursorTex, gdl);
        gDPPipeSync(gdl++);
        gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    for (i = 0; i < 8; i++) {
        n = i & 1;
    }
    if (drawGrid != 0) {
        gDPPipeSync(gdl++);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x80);
        gSPClearGeometryMode(gdl++, G_CULL_BOTH);
        gSPSetGeometryMode(gdl++, G_CULL_FRONT);
        gdl = arned_draw_walls(gdl);
        gDPPipeSync(gdl++);
        gSPClearGeometryMode(gdl++, G_CULL_BOTH);
        gSPSetGeometryMode(gdl++, G_CULL_BACK);
    }
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gSPSetGeometryMode(gdl++, 0x220004);
    for (i = 0; i < 8; i++) {
        os = sArnedObjViews + i;
        if (os->count != 0) {
            gSPMatrix(gdl++, &os->proj[idx], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(gdl++, os->perspNorm);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sArnedScissorX0, sArnedScissorY0, sArnedScissorX1, sArnedScissorY1);
            gSPViewport(gdl++, &os->vp[idx]);
            for (j = 0; j < os->count; j++) {
                gSPMatrix(gdl++, &os->objs[j].mtx[idx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gdl = prim_build_dl(12, os->objs[j].model, &sArnedPieceTex, gdl);
            }
            sArnedObjViews[i].count = 0;
        }
    }
    gSPClearGeometryMode(gdl++, 0x220004);
    return gdl;
}

void arned_load_piece_textures(s32 texSet, TexSet *pieceTex2, TexSet *wallTex)
{
    s32 cur;
    s32 *w;
    ResSet *q;
    u32 *slot;
    Editor *p;

    cur = sArnedTexSetIdx;
    sArnedPieceTex2 = pieceTex2;
    sArnedWallTex = wallTex;
    if (cur != texSet) {
        if (cur == -1) {
            sArnedPieceTex = res_load(gEditor->pieceRes, *sArnedTexResIds[texSet]);
        } else {
            /* load into the previous set's memory, which the new handle then owns */
            res_set_load_buffer(sArnedPieceTex);
            res_load(gEditor->pieceRes, *sArnedTexResIds[texSet]);
            res_set_load_buffer(0);
            p = gEditor;
            p->pieceRes->handles[*sArnedTexResIds[sArnedTexSetIdx]] = 0;
            w = sArnedTexResIds[texSet];
            q = p->pieceRes;
            slot = (u32 *)(*w * 4 + (u32)q->handles);
            *slot = *slot & 0xFFFFFFFC;
        }
        sArnedTexSetIdx = texSet;
    }
}

void arned_set_object_view(s32 frame, s32 view, s32 x, s32 y, s32 w, s32 h, s32 camX, s32 camY, s32 camZ,
                           s32 ry, s32 rx, s32 unk)
{
    Mtx persp;
    Mtx rot;
    Mtx m1;
    Mtx m2;
    Mtx res;
    ArnedObjView *c; ArnedObjView *base;
    f32 t, t2, asp;

    base = sArnedObjViews;
    c = (ArnedObjView *)(view * sizeof(ArnedObjView) + (u32)base);
    c->count = 0;
    c->x = x;
    c->y = y;
    c->vp[frame].vp.vscale[0] = w * 2;
    c->vp[frame].vp.vscale[1] = h * 2;
    c->vp[frame].vp.vscale[2] = 0x1FF;
    c->vp[frame].vp.vtrans[0] = (x + w / 2) * 4;
    c->vp[frame].vp.vtrans[1] = (y + h / 2) * 4;
    c->vp[frame].vp.vtrans[2] = 0x1FF;
    c->unk_AE = unk;
    t = (f32)gScreenHeight * 0.5f * (f32)gCosTable[0] / (f32)gCosTable[0xC00];
    t2 = t * (f32)gScreenHeight * 0.00390625f;
    asp = 320.0f / (f32)gScreenHeight;
    *(volatile f32 *)&gProjScale = t;
    gProjScale = t2;
    guPerspective(&persp, &sArnedObjViews[view].perspNorm, 33.0f, asp, 50.0f, 16384.0f, 1.0f);
    guRotateRPY(&rot, (f32)ry * 360.0f * 1.5258789e-05f, -(f32)rx * 360.0f * 1.5258789e-05f, 0.0f);
    mtx_transpose_rot(&rot);
    mtx_from_angles_pos_scale(&m1, 0, 0, 0, -camX, -camY, -camZ, 0x100);
    mtx_mul(&m1, &rot, &m2);
    mtx_mul(&m2, &gViewAxisMtx, &m1);
    mtx_mul(&m1, &persp, &res);
    sArnedObjViews[view].proj[frame] = res;
}

void arned_add_object(s32 frame, s32 view, PrimModel *model, s32 x, s32 z)
{
    s32 off;
    ArnedObjView *v;
    s32 n;
    ArnedObjView *base;

    base = sArnedObjViews;
    off = view * sizeof(ArnedObjView);
    v = (ArnedObjView *)(off + (s32)base);
    n = v->count;
    if (n < 12) {
        v->objs[n].model = model;
        mtx_from_angles_pos_scale(&v->objs[v->count].mtx[frame], 0, 0, 0, x, 0, z, 0x100);
        ((ArnedObjView *)(off + (s32)sArnedObjViews))->count += 1;
    }
}

/* The vertex of the cell's first piece at x, z, in its vertex buffer. */
Vtx_t *arned_find_vertex(s32 x, s32 z)
{
    s32 w;
    s32 h;
    s32 cz;
    s32 cx;
    ArnedCell *cell;
    PrimModel *piece;
    Vtx_t *v;
    Vtx_t *base;
    u32 cnt;
    s32 slot;
    s32 row;
    s32 col;
    u32 n;
    u32 i;

    w = sArnedGridW;
    cx = x - w * 250;
    h = sArnedGridH;
    cz = z - h * 250;
    row = cz / 500;
    col = cx / 500;
    cell = &sArnedCells[row * w + col];
    piece = cell->piece;
    if (piece == NULL || (slot = cell->slot) < 0) {
        return NULL;
    }
    cnt = piece->numVtx;
    base = sArnedVtxBufs[slot];
    i = 0;
    if (cnt != 0) {
        n = cnt;
        v = base;
        do {
            i = i + 1;
            if (v->ob[0] == cx && v->ob[2] == cz) {
                return v;
            }
            v++;
        } while (i < n);
    }
    return NULL;
}

void arned_free_grid(void)
{
    s32 i;
    void **tbl;

    i = 0;
    do {
        tbl = (void **)sArnedVtxBufs;
        mem_free(tbl[i]);
        i++;
    } while (i < 0x1c2);
    mem_free(sArnedVtxBufs);
    mem_free(sArnedCells);
    mem_free(sArnedFloorVtx);
    mem_free(sArnedWallVtx);
    mem_free(sArnedObjViews);
    sArnedCells = 0;
}

void arned_transform_piece_verts(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag)
{
    s32 x, z, nx, nz;

    for (; n != 0; n--) {
        switch (rot) {
        case 0:
            x = in->ob[0];
            z = in->ob[2];
            nx = in->cn[0];
            nz = in->cn[2];
            break;
        case 1:
            x = in->ob[2];
            nx = in->cn[2];
            z = -in->ob[0];
            nz = -in->cn[0];
            break;
        case 2:
            x = -in->ob[0];
            z = -in->ob[2];
            nx = -in->cn[0];
            nz = -in->cn[2];
            break;
        case 3:
            z = in->ob[0];
            nz = in->cn[0];
            x = -in->ob[2];
            nx = -in->cn[2];
            break;
        }
        out->ob[0] = dx + x + 0xfa;
        out->ob[1] = in->ob[1];
        out->ob[2] = dz + z + 0xfa;
        out->tc[0] = in->tc[0];
        out->tc[1] = in->tc[1];
        out->cn[0] = nx;
        out->cn[1] = in->cn[1];
        out->cn[2] = nz;
        if (flag != 0) {
            out->flag = 1;
        } else {
            out->flag = 0;
        }
        if (x == -0xfa || x == 0xfa || z == -0xfa || z == 0xfa) {
            out->cn[3] = 2;
        } else {
            out->cn[3] = 1;
        }
        out++;
        in++;
    }
}

void arned_set_viewport(s32 x, s32 y, s32 w, s32 h) {
    Vp_t *q = &gArnedViewport.vp;
    s32 k = 0x1FF;

    q->vscale[0] = w << 1;
    q->vscale[1] = h << 1;
    q->vscale[2] = k;
    q->vtrans[0] = (x + w / 2) * 4;
    q->vtrans[1] = (y + h / 2) * 4;
    sArnedViewX = x;
    sArnedViewY = y;
    sArnedViewW = w;
    q->vtrans[2] = k;
    sArnedViewH = h;
}

void arned_set_viewport_stretch(s32 x, s32 y, s32 w, s32 h, s32 extraH)
{
    Vp_t *b = &gArnedViewport.vp;

    b->vscale[0] = w * 2;
    b->vscale[2] = 0x1FF;
    b->vtrans[0] = (x + w / 2) * 4;
    b->vtrans[1] = (y + h / 2) * 4;
    sArnedViewX = x;
    sArnedViewY = y;
    sArnedViewW = w;
    b->vtrans[2] = 0x1FF;
    sArnedViewH = h;
    b->vscale[1] = (h + extraH) * 2;
}

void arned_set_scissor(s32 a0, s32 a1, s32 a2, s32 a3)
{
    sArnedScissorX0 = a0;
    sArnedScissorY0 = a1;
    sArnedScissorX1 = a0 + a2;
    sArnedScissorY1 = a1 + a3;
}
