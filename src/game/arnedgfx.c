#include "game.h"

#define HALF_CELL (CELL_VTX_UNITS / 2)

/* Adds a quad of the arena wall from (x, z) along (w, h), one cell high, its top edge half a cell further out. */
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
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = CELL_VTX_UNITS;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0x1000;
    sArnedWallVtxCount++;
    sArnedWallVtx[sArnedWallVtxCount].ob[0] = x + w;
    sArnedWallVtx[sArnedWallVtxCount].ob[1] = CELL_VTX_UNITS;
    sArnedWallVtx[sArnedWallVtxCount].ob[2] = z + h;
    sArnedWallVtx[sArnedWallVtxCount].tc[0] = 0x800;
    sArnedWallVtx[sArnedWallVtxCount].tc[1] = 0x1000;
    sArnedWallVtxCount++;
}

/* Draws the arena walls with their texture, 32 vertices (8 quads) at a time; not while a user piece is edited. */
Gfx *arned_draw_walls(Gfx *gdl)
{
    s32 i, j, count;
    TexSet *texSet;
    TexEntry *tex;

    if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        return gdl;
    }
    {
        texSet = sArnedWallTex;
        gDPLoadTLUT_pal256(gdl++, texSet->pal);
        tex = &texSet->tex[0];
        gDPLoadTextureBlock(gdl++, tex->data, G_IM_FMT_IA, G_IM_SIZ_8b,
                            tex->width * 8, tex->height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 6, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0; i < sArnedWallVtxCount; i += 32) {
            count = sArnedWallVtxCount - i;
            if (count > 32) count = 32;
            gSPVertex(gdl++, &sArnedWallVtx[i], count, 0);
            for (j = 0; j < count / 4; j++) {
                gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
            }
        }
    }
    return gdl;
}

/* Allocates the w x h grid of cells, the vertex buffers, the floor's vertices, the object views and the walls round
   the grid. */
void arned_init_grid(s32 w, s32 h)
{
    s32 cellsSize;
    s32 count;
    s32 i;
    s32 j;
    s32 k;
    s32 cols;
    s32 rows;
    s32 h2;
    u8 *mem;

    if (sArnedCells != NULL) {
        arned_free_grid();
    }
    sArnedObjViews = mem_alloc_dbg(ARNED_OBJ_VIEWS * sizeof(ArnedObjView), 0, (s32)sStrArnedgfxC, 0xD4);
    cellsSize = w * ((h << 2) + h << 2); /* w * h * sizeof(ArnedCell) */
    sArnedCells = (ArnedCell *)(mem = mem_alloc_dbg(cellsSize, 0, (s32)sStrArnedgfxC, 0xD7));
    if (mem == NULL) {
        mem = fatal_halt_2(1);
    }
    sArnedGridW = w;
    sArnedGridH = h;
    mem_fill(mem, 0, cellsSize);
    count = sArnedGridW * sArnedGridH;
    for (i = 0; i < count; i++) {
        sArnedCells[i].slot = -1;
    }
    count = ARNED_SLOT_FREE;
    for (i = ARNED_VTX_BUFS - 1; i >= 0; i--) {
        sArnedSlotOwner[i] = count;
    }
    sArnedVtxBufs = mem_alloc_dbg(ARNED_VTX_BUFS * sizeof(Vtx_t *), 0, (s32)sStrArnedgfxC, 0xE4);
    for (i = 0; i < ARNED_VTX_BUFS; i++) {
        mem = mem_alloc_dbg(ARNED_BUF_VTX * sizeof(Vtx_t), 0, (s32)sStrArnedgfxC, 0xE7);
        sArnedVtxBufs[i] = (Vtx_t *)mem;
        mem_fill(mem, 0, ARNED_BUF_VTX * sizeof(Vtx_t));
    }
    /* the floor: a vertex at each grid corner */
    h2 = h;
    do {
        Vtx_t **floorVtx = &sArnedFloorVtx;
        cols = w + 1;
        rows = h2 + 1;
        *floorVtx = mem_alloc_dbg(cols * (rows << 4), 0, (s32)sStrArnedgfxC, 0xEF);
        i = 0;
    } while (0);
    k = 0;
    if (rows > 0) {
        do {
            for (j = 0; j < w + 1; j++) {
                sArnedFloorVtx[k].ob[0] = j * CELL_VTX_UNITS - w * HALF_CELL;
                sArnedFloorVtx[k].ob[1] = 0;
                sArnedFloorVtx[k].ob[2] = i * CELL_VTX_UNITS - h2 * HALF_CELL;
                k++;
            }
        } while (++i < h2 + 1);
    }
    i = 0;
    mem_fill((u8 *)sArnedObjViews, 0, ARNED_OBJ_VIEWS * sizeof(ArnedObjView));
    /* a quad per cell on each side */
    sArnedWallVtx = mem_alloc_dbg((sArnedGridW + sArnedGridH) * 0x80, 0, (s32)sStrArnedgfxC, 0x100);
    sArnedWallVtxCount = 0;
    for (i = 0; i < sArnedGridW; i++) {
        arned_add_wall_quad(i * CELL_VTX_UNITS - sArnedGridW * HALF_CELL, sArnedGridH * HALF_CELL, CELL_VTX_UNITS, 0);
        arned_add_wall_quad(sArnedGridW * HALF_CELL - i * CELL_VTX_UNITS, -(sArnedGridH * HALF_CELL), -CELL_VTX_UNITS, 0);
    }
    for (i = 0; i < sArnedGridH; i++) {
        arned_add_wall_quad(-(sArnedGridW * HALF_CELL), i * CELL_VTX_UNITS - sArnedGridH * HALF_CELL, 0, CELL_VTX_UNITS);
        arned_add_wall_quad(sArnedGridW * HALF_CELL, sArnedGridH * HALF_CELL - i * CELL_VTX_UNITS, 0, -CELL_VTX_UNITS);
    }
    sArnedTexSetIdx = -1;
}

/* Sets what cell (x, y) shows; it is rebuilt at the next arned_draw if anything changed (or ARNED_CELL_REBUILD). */
void arned_set_cell(s32 x, s32 y, PrimModel *piece, s32 rot, PrimModel *piece2, s32 rot2, s32 flags, s32 dx, s32 dz)
{
    s32 idx = sArnedGridW * y + x;

    if ((flags & ARNED_CELL_REBUILD) || sArnedCells[idx].piece != piece || sArnedCells[idx].piece2 != piece2 ||
        sArnedCells[idx].rot != rot || sArnedCells[idx].rot2 != rot2 || sArnedCells[idx].flags != flags ||
        sArnedCells[idx].dx != dx || sArnedCells[idx].dz != dz) {
        sArnedCells[idx].dirty = 1;
    }
    sArnedCells[idx].piece = piece;
    sArnedCells[idx].piece2 = piece2;
    sArnedCells[idx].rot = rot;
    sArnedCells[idx].rot2 = rot2;
    sArnedCells[idx].flags = flags & ~ARNED_CELL_REBUILD;
    sArnedCells[idx].dx = dx;
    sArnedCells[idx].dz = dz;
}

/* The camera of frame buffer `frame`: at (x, y, z), turned by yaw and pitch (1/65536 turns), into gArnedCamMtx. */
void arned_set_camera(s32 frame, s32 x, s32 y, s32 z, s16 yaw, s16 pitch)
{
    Mtx persp;
    Mtx rot;
    Mtx tmp1;
    Mtx tmp2;
    Mtx res;
    f32 projScale;
    s32 screenH1, screenH2, screenH3;

    sArnedCamX = x;
    gFarClip = 0x7FBC;
    sArnedCamY = y;
    sArnedCamZ = z;
    sArnedCamYaw = yaw;
    sArnedCamPitch = pitch;
    screenH1 = gScreenHeight;
    screenH2 = gScreenHeight;
    screenH3 = gScreenHeight;
    projScale = (f32)screenH1 * 0.5f * gCosTable[0] / gCosTable[0xC00];
    *(volatile f32 *)&gProjScale = projScale;
    gProjScale = projScale * screenH2 * 0.00390625f;
    guPerspective(&persp, &gPerspNorm, 33.0f, 320.0f / screenH3, 50.0f, gFarClip, 1.0f);
    guRotateRPY(&rot, sArnedCamPitch * 360.0f * 1.5258789e-05f, -(f32)sArnedCamYaw * 360.0f * 1.5258789e-05f, 0.0f);
    mtx_transpose_rot((u8 *)&rot);
    mtx_from_angles_pos_scale(&tmp1, 0, 0, 0, -sArnedCamX, -sArnedCamY, -sArnedCamZ, 0x100);
    mtx_mul(&tmp1, &rot, &tmp2);
    mtx_mul(&tmp2, &gViewAxisMtx, &tmp1);
    mtx_mul(&tmp1, &persp, &res);
    gArnedCamMtx[frame] = res;
}

#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]

/* Draws the editor's 3D view for frame buffer `frame`: rebuilds the vertex buffers of the changed cells, then draws
   the floor (drawGrid), the walls' outsides, the cells' pieces (selected ones pulsing), the start cursor (cursorRot
   -1: none), the walls' insides and the object views. */
Gfx *arned_draw(s32 frame, Gfx *gdl, s32 selX, s32 selY, s32 selW, s32 selH, s32 cursorX, s32 cursorZ, s32 cursorRot,
                 s32 drawGrid) {
    s32 numCells;
    s32 originX, originZ;
    s32 i, j, x, y, n;
    s32 lightX, lightZ;
    s32 r, g, b;
    ArnedCell *cell;
    Vtx_t *vtx;
    ArnedObjView *view;
    Mtx *mtx;

    numCells = sArnedGridW * sArnedGridH;
    for (i = 0; i < ARNED_VTX_BUFS; i++) {
        if (sArnedSlotOwner[i] == ARNED_SLOT_RELEASED) {
            sArnedSlotOwner[i] = ARNED_SLOT_FREE;
        }
    }
    i = 0;
    originX = -(sArnedGridW * HALF_CELL);
    originZ = -(sArnedGridH * HALF_CELL);
    for (y = 0; y < sArnedGridH; y++) {
        for (x = 0; x < sArnedGridW; x++, i++) {
            cell = &sArnedCells[i];
            if (cell->dirty != 0) {
                if (cell->slot != -1) {
                    sArnedSlotOwner[cell->slot] = ARNED_SLOT_RELEASED;
                }
                for (n = 0; n < ARNED_VTX_BUFS; n++) {
                    if (sArnedSlotOwner[n] == ARNED_SLOT_FREE) {
                        break;
                    }
                }
                if (cell->piece == NULL && cell->piece2 == NULL) {
                    n = ARNED_VTX_BUFS;
                }
                if (n == ARNED_VTX_BUFS) {
                    cell->slot = -1;
                } else {
                    sArnedSlotOwner[n] = i;
                    cell->slot = n;
                    vtx = sArnedVtxBufs[n];
                    cell->dirty = 0;
                    n = 0;
                    if (cell->piece != NULL && cell->piece->numVtx < ARNED_BUF_VTX + 1) {
                        arned_transform_piece_verts(cell->piece->vtx, cell->piece->numVtx, cell->rot, vtx, originX + x * CELL_VTX_UNITS, originZ + y * CELL_VTX_UNITS, 1);
                        n = cell->piece->numVtx;
                    }
                    if (cell->piece2 != NULL && cell->piece2->numVtx + n < ARNED_BUF_VTX + 1) {
                        arned_transform_piece_verts(cell->piece2->vtx, cell->piece2->numVtx, cell->rot2, &vtx[n], originX + x * CELL_VTX_UNITS + cell->dx, originZ + y * CELL_VTX_UNITS + cell->dz, 0);
                        n += cell->piece2->numVtx;
                    }
                    for (; n < ARNED_BUF_VTX; n++) {
                        vtx[n].flag = 2;
                    }
                }
            }
        }
    }
    /* the light circles overhead */
    lightX = gCosTable[(((gFrameCounter << 8) >> 4) - 0x400) & 0xFFF] * 80 / 0x4000;
    lightZ = gCosTable[((gFrameCounter << 8) >> 4) & 0xFFF] * 80 / 0x4000;
    {
    Lights1 light = gdSPDefLights1(0x28, 0x28, 0x28, 0xFF, 0xFF, 0xFF, lightX, 0x7F, lightZ);
    sArnedLights[frame] = light;
    }
    gSPSetLights1(gdl++, sArnedLights[frame]);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sArnedViewX, sArnedViewY, sArnedViewX + sArnedViewW, sArnedViewY + sArnedViewH);
    gSPViewport(gdl++, &gArnedViewport);
    gSPPerspNormalize(gdl++, gPerspNorm);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &gArnedCamMtx[frame], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    if (drawGrid != 0) {
        /* the floor: a checkerboard of dark cells and cells shaded blue across the grid */
        gDPSetRenderMode(gdl++, 0x0F0A4000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        for (i = 0; i < numCells; i++) {
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
    for (i = 0; i < numCells; i++) {
        cell = &sArnedCells[i];
        if (cell->slot != -1) {
            n = 0;
            if (cell->piece != NULL && cell->piece->numVtx < ARNED_BUF_VTX + 1) {
                gDPPipeSync(gdl++);
                if (cell->flags & ARNED_CELL_SELECTED) {
                    s32 phase;
                    phase = gFrameCounter << 10;
                    gDPSetPrimColor(gdl++, 0, 0, (SINS(phase) * 25 >> 13) + 128, (SINS(phase) * 25 >> 13) + 128, (SINS(phase) * 127 >> 14) + 128, 0xFF);
                    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
                    gSPClearGeometryMode(gdl++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
                } else {
                    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
                    gSPSetGeometryMode(gdl++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
                }
                /* drawn from the cell's vertex buffer in place of the model's own vertices */
                vtx = cell->piece->vtx;
                cell->piece->vtx = sArnedVtxBufs[cell->slot];
                gdl = prim_build_dl(12, cell->piece, &sArnedPieceTex, gdl);
                cell->piece->vtx = vtx;
                n = cell->piece->numVtx;
            }
            if (cell->piece2 != NULL && (gEditor->mode != EDIT_MODE_USER_PIECE || !(cell->flags & ARNED_CELL_SELECTED)) && cell->piece2->numVtx + n < ARNED_BUF_VTX + 1) {
                gDPPipeSync(gdl++);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
                gSPClearGeometryMode(gdl++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
                vtx = cell->piece2->vtx;
                cell->piece2->vtx = sArnedVtxBufs[cell->slot] + n;
                gdl = prim_build_dl(12, cell->piece2, &sArnedPieceTex2, gdl);
                cell->piece2->vtx = vtx;
            }
        }
    }
    if (cursorRot != -1 && gEditor->mode != EDIT_MODE_USER_PIECE) {
        gDPPipeSync(gdl++);
        mtx = &sArnedCursorMtx[frame];
        mtx_from_angles_pos_scale(mtx, 0, 0, cursorRot + 0x4000, cursorX, 2500, cursorZ, 0x100);
        gSPMatrix(gdl++, mtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPSetRenderMode(gdl++, 0x504A50, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
        gDPPipeSync(gdl++);
        gdl = prim_build_dl(12, gEditor->cursorModel, &gEditor->cursorTex, gdl);
        gDPPipeSync(gdl++);
        gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    for (i = 0; i < 8; i++) {
        n = i & 1; /* does nothing, as in the original */
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
    gSPSetGeometryMode(gdl++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
    for (i = 0; i < ARNED_OBJ_VIEWS; i++) {
        view = sArnedObjViews + i;
        if (view->count != 0) {
            gSPMatrix(gdl++, &view->proj[frame], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(gdl++, view->perspNorm);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sArnedScissorX0, sArnedScissorY0, sArnedScissorX1, sArnedScissorY1);
            gSPViewport(gdl++, &view->vp[frame]);
            for (j = 0; j < view->count; j++) {
                gSPMatrix(gdl++, &view->objs[j].mtx[frame], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gdl = prim_build_dl(12, view->objs[j].model, &sArnedPieceTex, gdl);
            }
            sArnedObjViews[i].count = 0;
        }
    }
    gSPClearGeometryMode(gdl++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
    return gdl;
}

/* Loads the pieces' texture set of dirt type texSet from the "Pieces" resources (over the previous set's memory) and
   takes the other two sets the view draws with. */
void arned_load_piece_textures(s32 texSet, TexSet *pieceTex2, TexSet *wallTex)
{
    s32 curSet;
    s32 *idPtr;
    ResSet *res;
    u32 *slot;
    Editor *ed;

    curSet = sArnedTexSetIdx;
    sArnedPieceTex2 = pieceTex2;
    sArnedWallTex = wallTex;
    if (curSet != texSet) {
        if (curSet == -1) {
            sArnedPieceTex = res_load(gEditor->pieceRes, *sArnedTexResIds[texSet]);
        } else {
            /* load into the previous set's memory, which the new handle then owns */
            res_set_load_buffer(sArnedPieceTex);
            res_load(gEditor->pieceRes, *sArnedTexResIds[texSet]);
            res_set_load_buffer(0);
            ed = gEditor;
            ed->pieceRes->handles[*sArnedTexResIds[sArnedTexSetIdx]] = 0;
            idPtr = sArnedTexResIds[texSet];
            res = ed->pieceRes;
            slot = (u32 *)(*idPtr * 4 + (u32)res->handles);
            *slot = *slot & 0xFFFFFFFC;
        }
        sArnedTexSetIdx = texSet;
    }
}

/* Sets up object view `view` for frame buffer `frame`: its viewport at (x, y, w, h) and a camera at camX, camY, camZ
   turned by yaw and pitch; emptied of objects. */
void arned_set_object_view(s32 frame, s32 view, s32 x, s32 y, s32 w, s32 h, s32 camX, s32 camY, s32 camZ,
                           s32 pitch, s32 yaw, s32 unk)
{
    Mtx persp;
    Mtx rot;
    Mtx tmp1;
    Mtx tmp2;
    Mtx res;
    ArnedObjView *ov; ArnedObjView *views;
    f32 projScale, projScale2, aspect;

    views = sArnedObjViews;
    ov = (ArnedObjView *)(view * sizeof(ArnedObjView) + (u32)views);
    ov->count = 0;
    ov->x = x;
    ov->y = y;
    ov->vp[frame].vp.vscale[0] = w * 2;
    ov->vp[frame].vp.vscale[1] = h * 2;
    ov->vp[frame].vp.vscale[2] = G_MAXZ / 2;
    ov->vp[frame].vp.vtrans[0] = (x + w / 2) * 4;
    ov->vp[frame].vp.vtrans[1] = (y + h / 2) * 4;
    ov->vp[frame].vp.vtrans[2] = G_MAXZ / 2;
    ov->unk_AE = unk;
    projScale = (f32)gScreenHeight * 0.5f * (f32)gCosTable[0] / (f32)gCosTable[0xC00];
    projScale2 = projScale * (f32)gScreenHeight * 0.00390625f;
    aspect = 320.0f / (f32)gScreenHeight;
    *(volatile f32 *)&gProjScale = projScale;
    gProjScale = projScale2;
    guPerspective(&persp, &sArnedObjViews[view].perspNorm, 33.0f, aspect, 50.0f, 16384.0f, 1.0f);
    guRotateRPY(&rot, (f32)pitch * 360.0f * 1.5258789e-05f, -(f32)yaw * 360.0f * 1.5258789e-05f, 0.0f);
    mtx_transpose_rot(&rot);
    mtx_from_angles_pos_scale(&tmp1, 0, 0, 0, -camX, -camY, -camZ, 0x100);
    mtx_mul(&tmp1, &rot, &tmp2);
    mtx_mul(&tmp2, &gViewAxisMtx, &tmp1);
    mtx_mul(&tmp1, &persp, &res);
    sArnedObjViews[view].proj[frame] = res;
}

/* Adds model at (x, z) to object view `view` (at most ARNED_VIEW_OBJS). */
void arned_add_object(s32 frame, s32 view, PrimModel *model, s32 x, s32 z)
{
    s32 offset;
    ArnedObjView *ov;
    s32 count;
    ArnedObjView *views;

    views = sArnedObjViews;
    offset = view * sizeof(ArnedObjView);
    ov = (ArnedObjView *)(offset + (s32)views);
    count = ov->count;
    if (count < ARNED_VIEW_OBJS) {
        ov->objs[count].model = model;
        mtx_from_angles_pos_scale(&ov->objs[ov->count].mtx[frame], 0, 0, 0, x, 0, z, 0x100);
        ((ArnedObjView *)(offset + (s32)sArnedObjViews))->count += 1;
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
    Vtx_t *buf;
    u32 count;
    s32 slot;
    s32 row;
    s32 col;
    u32 n;
    u32 i;

    w = sArnedGridW;
    cx = x - w * HALF_CELL;
    h = sArnedGridH;
    cz = z - h * HALF_CELL;
    row = cz / CELL_VTX_UNITS;
    col = cx / CELL_VTX_UNITS;
    cell = &sArnedCells[row * w + col];
    piece = cell->piece;
    if (piece == NULL || (slot = cell->slot) < 0) {
        return NULL;
    }
    count = piece->numVtx;
    buf = sArnedVtxBufs[slot];
    i = 0;
    if (count != 0) {
        n = count;
        v = buf;
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

/* Frees what arned_init_grid allocated. */
void arned_free_grid(void)
{
    s32 i;
    void **bufs;

    i = 0;
    do {
        bufs = (void **)sArnedVtxBufs;
        mem_free(bufs[i]);
        i++;
    } while (i < ARNED_VTX_BUFS);
    mem_free(sArnedVtxBufs);
    mem_free(sArnedCells);
    mem_free(sArnedFloorVtx);
    mem_free(sArnedWallVtx);
    mem_free(sArnedObjViews);
    sArnedCells = 0;
}

/* Copies n vertices of a piece turned by rot quarter turns (their normals too) and moved to the cell at (dx, dz).
   The vertices' flag is 1 for the cell's first piece, 0 for the second; cn[3] is 2 on the cell's edge, else 1. */
void arned_transform_piece_verts(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 first)
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
        out->ob[0] = dx + x + HALF_CELL;
        out->ob[1] = in->ob[1];
        out->ob[2] = dz + z + HALF_CELL;
        out->tc[0] = in->tc[0];
        out->tc[1] = in->tc[1];
        out->cn[0] = nx;
        out->cn[1] = in->cn[1];
        out->cn[2] = nz;
        if (first != 0) {
            out->flag = 1;
        } else {
            out->flag = 0;
        }
        if (x == -HALF_CELL || x == HALF_CELL || z == -HALF_CELL || z == HALF_CELL) {
            out->cn[3] = 2;
        } else {
            out->cn[3] = 1;
        }
        out++;
        in++;
    }
}

/* The 3D view's viewport and scissor box. */
void arned_set_viewport(s32 x, s32 y, s32 w, s32 h) {
    Vp_t *vp = &gArnedViewport.vp;
    s32 halfZ = G_MAXZ / 2;

    vp->vscale[0] = w << 1;
    vp->vscale[1] = h << 1;
    vp->vscale[2] = halfZ;
    vp->vtrans[0] = (x + w / 2) * 4;
    vp->vtrans[1] = (y + h / 2) * 4;
    sArnedViewX = x;
    sArnedViewY = y;
    sArnedViewW = w;
    vp->vtrans[2] = halfZ;
    sArnedViewH = h;
}

/* The same with the viewport's height scaled up by extraH (the menu panel sliding). */
void arned_set_viewport_stretch(s32 x, s32 y, s32 w, s32 h, s32 extraH)
{
    Vp_t *vp = &gArnedViewport.vp;

    vp->vscale[0] = w * 2;
    vp->vscale[2] = G_MAXZ / 2;
    vp->vtrans[0] = (x + w / 2) * 4;
    vp->vtrans[1] = (y + h / 2) * 4;
    sArnedViewX = x;
    sArnedViewY = y;
    sArnedViewW = w;
    vp->vtrans[2] = G_MAXZ / 2;
    sArnedViewH = h;
    vp->vscale[1] = (h + extraH) * 2;
}

/* The object views' scissor box. */
void arned_set_scissor(s32 x, s32 y, s32 w, s32 h)
{
    sArnedScissorX0 = x;
    sArnedScissorY0 = y;
    sArnedScissorX1 = x + w;
    sArnedScissorY1 = y + h;
}
