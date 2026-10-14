#include "game.h"

/* The track name without its terminator, copied as one block */
typedef struct { u8 b[EDIT_NAME_LEN]; } NameBlock;

/* Packs the editor's track for a save (at most 0x310 bytes): 0x35, the dirt type, the name, the piece count, the
   start piece, the user piece count, each user piece's segment count and heights, then 3 bytes per piece from the
   first: x (5 bits), y (5), flags (6), rot (2), type (6). Returns the size. */
s32 usrtrk_pack_track(u8 *buf)
{
    u8 *out = buf;
    EditPiece *piece = gEditor->first;
    s32 count, startIdx, k, j, total;
    u16 typeIdx;
    u8 *heights;
    s16 segments;

    if (piece != NULL) {
        count = 0;
        do {
            if (piece == gEditor->start) {
                startIdx = count;
            }
            piece = piece->next;
            total = count + 1;
            count = total;
        } while (piece != NULL && piece != gEditor->first);

        out[0] = USER_TRACK_TAG;
        out[1] = gEditor->dirtType;
        out += 2;
        *(NameBlock *)out = *(NameBlock *)gEditor->name;
        out += EDIT_NAME_LEN;
        *(u16 *)out = total;
        out += 2;
        *(u16 *)out = startIdx;
        out += 2;
        *(u16 *)out = gEditor->userPieceCount;
        out += 2;
        for (k = 0; k < gEditor->userPieceCount; k++) {
            segments = gPieceTypes[k + PIECE_TYPE_USER].dim[1];
            *(u16 *)out = segments;
            heights = out + 2;
            for (j = 0; j < segments * 2 - 1; j++) {
                ((u16 *)heights)[j] = gUserPieceHeights[k][j];
            }
            out += segments * 4;
        }
        piece = gEditor->first;
        do {
            typeIdx = ((u32)piece->type - (u32)gPieceTypes) >> 6; /* / sizeof(PieceType) */
            out[0] = ((piece->x & 0x1F) << 3) | ((piece->y & 0x1F) >> 2);
            out[1] = ((piece->y & 0x1F) << 6) | (piece->flags & 0x3F);
            out[2] = ((piece->rot & 3) << 6) | (typeIdx & 0x3F);
            out += 3;
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
    }
    return (s16)(out - buf);
}

/* Unpacks a packed user track into gUserTrackLayout, the pieces in order from the start piece. */
void usrtrk_load_layout(u8 *p)
{
    s32 dirtType;
    u16 count;
    u16 start;
    s32 userPieces;
    s16 i;
    s32 k;
    s32 j;

    dirtType = p[1];
    p += 2 + EDIT_NAME_LEN;
    count = *((u16 *)p)++;
    start = *((u16 *)p)++;
    userPieces = *((s16 *)p)++;
    if (gUserTrackLayout != NULL) {
        mem_free(gUserTrackLayout);
    }
    gUserTrackLayout = mem_alloc_dbg(((s16)count - 1) * sizeof(UserTrackPiece) + sizeof(UserTrackLayout), 0, (s32)sStrUsrTrkFile, 0xFD);
    gUserTrackLayout->dirtType = dirtType;
    gUserTrackLayout->userPieceCount = userPieces;
    for (i = 0; i < userPieces; i++) {
        gUserTrackLayout->segmentCounts[i] = p[1];
        memcpy_2((u8 *)(i * 30 + (u32)gUserTrackLayout) + 0x14 /* heights[i] */, p + 2, gUserTrackLayout->segmentCounts[i] * 4 - 2);
        p += gUserTrackLayout->segmentCounts[i] * 4;
    }
    gUserTrackLayout->pieceCount = (s16)count;
    for (k = 0; k < (s16)count; k++) {
        j = (k - (s16)start + (s16)count) % (s16)count;
        gUserTrackLayout->pieces[j].x = p[0] >> 3;
        gUserTrackLayout->pieces[j].y = (p[1] >> 6) | ((p[0] & 7) << 2);
        gUserTrackLayout->pieces[j].flags = p[1] & 0x3F;
        gUserTrackLayout->pieces[j].rot = p[2] >> 6;
        gUserTrackLayout->pieces[j].type = p[2] & 0x3F;
        p += 3;
    }
}

/* Unpacks a packed user track into the editor: the name, the user pieces' heights and the pieces as one open list
   (edit.c closes the loop). */
void usrtrk_load_into_editor(u8 *p)
{
    s16 i;
    s16 segments;
    u16 segmentsU;
    s16 count, startIdx;
    s32 j;
    u16 *heights;

    {
        Editor *ed;
        u8 dirtType = p[1];
        p += 2;
        ed = gEditor;
        ed->dirtType = dirtType;
        *(NameBlock *)ed->name = *(NameBlock *)p;
    }
    p += EDIT_NAME_LEN;
    gEditor->name[EDIT_NAME_LEN] = 0;
    count = *(s16 *)p;
    p += 2;
    startIdx = *(s16 *)p;
    p += 2;
    gEditor->userPieceCount = *(s16 *)p;
    p += 2;
    for (i = 0; i < gEditor->userPieceCount; i++) {
        segments = *(s16 *)p;
        segmentsU = *(u16 *)p;
        heights = gUserPieceHeights[i];
        memcpy(heights, p + 2, (segments * 2 - 1) * 2);
        usrtrk_apply_heights(gPieceTypes[i + PIECE_TYPE_USER].meshA, gPieceTypes[i + PIECE_TYPE_USER].meshB, heights, segments);
        gPieceTypes[i + PIECE_TYPE_USER].dim[1] = segmentsU;
        p = (u8 *)((s16 *)p + segments * 2);
    }

    gEditor->first = gEditor->cur = gEditor->pieces;
    gEditor->last = &gEditor->pieces[count - 1];
    gEditor->start = &gEditor->pieces[startIdx];
    for (j = 0; j < count; j++) {
        gEditor->pieces[j].used = 1;
        if (j == 0) {
            gEditor->pieces[j].prev = 0;
        } else {
            gEditor->pieces[j].prev = &gEditor->pieces[j - 1];
        }
        if (j == count - 1) {
            gEditor->pieces[j].next = 0;
        } else {
            gEditor->pieces[j].next = &gEditor->pieces[j + 1];
        }
        gEditor->pieces[j].x = p[0] >> 3;
        gEditor->pieces[j].y = (p[1] >> 6) | ((p[0] & 7) << 2);
        gEditor->pieces[j].flags = p[1] & 0x3F;
        gEditor->pieces[j].rot = p[2] >> 6;
        gEditor->pieces[j].type = &gPieceTypes[p[2] & 0x3F];
        p += 3;
        edit_update_piece_ports(&gEditor->pieces[j]);
    }
    for (j = count; j < EDIT_MAX_PIECES; j++) {
        gEditor->pieces[j].used = 0;
    }
}

/* The editor's track as gUserTrackLayout for the test race, from the start piece on. */
void usrtrk_layout_from_editor(void)
{
    EditPiece *piece;
    UserTrackLayout *layout;
    s16 count;
    s32 numPieces;
    s32 dirtType;
    s32 k;
    s16 i;
    s32 type;
    u16 rot;

    count = 0;
    piece = gEditor->first;
    do {
        piece = piece->next;
        count++;
    } while (piece != NULL && piece != gEditor->first);
    if (gUserTrackLayout != NULL) {
        mem_free(gUserTrackLayout);
    }
    numPieces = (s16)count;
    layout = mem_alloc_dbg((numPieces - 1) * 12 + 0x110 /* sizeof(UserTrackLayout) with numPieces pieces */, 0, sStrUsrTrkFile, 0x1AE);
    dirtType = gEditor->dirtType;
    gUserTrackLayout = layout;
    layout->pieceCount = numPieces;
    layout->dirtType = dirtType;
    k = gEditor->userPieceCount;
    i = 0;
    layout->userPieceCount = k;
    if (k > 0) {
        do {
            gUserTrackLayout->segmentCounts[i] = gPieceTypes[i + PIECE_TYPE_USER].dim[1];
            memcpy((u8 *)(i * 30 + (s32)gUserTrackLayout) + 0x14 /* heights[i] */, gUserPieceHeights[i], gUserTrackLayout->segmentCounts[i] * 4 - 2);
            i++;
        } while (i < gUserTrackLayout->userPieceCount);
    }
    piece = gEditor->start;
    k = 0;
    do {
        type = (s32)piece->type;
        gUserTrackLayout->pieces[k].x = piece->x;
        gUserTrackLayout->pieces[k].y = piece->y;
        gUserTrackLayout->pieces[k].flags = piece->flags;
        rot = piece->rot;
        gUserTrackLayout->pieces[k].type = (type - (s32)gPieceTypes) >> 6; /* / sizeof(PieceType) */
        gUserTrackLayout->pieces[k].rot = rot;
        piece = piece->next;
        k++;
        if (piece == NULL) {
            piece = gEditor->first;
            if (piece == NULL) {
                return;
            }
        }
    } while (piece != gEditor->start);
}

/* For a race on the user track: loads the arena's walls and the piece types' meshes, paths and walls from the "Pieces"
   resources (the ground vertices snapped to multiples of 25), and builds the user pieces of gUserTrackLayout. */
void usrtrk_load_pieces(void)
{
    s32 i;
    u32 j, k;
    s32 neg;
    s32 userIdx;

    res_register_type(sStrUsrTrkArn, (s32)arn_relocate);
    res_register_type(sStrUsrTrkWal, (s32)height_init_walls_reloc);
    res_open_set(sStrUsrTrkPieces, &sPiecesResSet);
    gEditArena.collision = (WallMesh *)res_load(sPiecesResSet, *gEditArena.collisionId);
    height_free_walls();
    mem_fill((u8 *)&gUserTrackData, 0, sizeof(UserTrackData));

    for (i = 0; i < PIECE_TYPE_COUNT; i++) {
        gPieceTypes[i].path = 0;
        gPieceTypes[i].collision = 0;
        if (!(gPieceTypes[i].flags & PIECE_USER)) {
        gPieceTypes[i].meshA = 0;
        gPieceTypes[i].meshB = 0;
        if (gPieceTypes[i].meshAId != 0) {
            gPieceTypes[i].meshA = (PieceMeshSet *)res_load(sPiecesResSet, *gPieceTypes[i].meshAId);
        }
        if (gPieceTypes[i].meshBId != 0) {
            gPieceTypes[i].meshB = (PieceMeshSet *)res_load(sPiecesResSet, *gPieceTypes[i].meshBId);
        }
        if (gPieceTypes[i].meshAId != 0 && (j = 0, j < gPieceTypes[i].meshA->count)) {
            do {
                PieceMesh *mesh = gPieceTypes[i].meshA->cells[j].mesh;
                for (k = 0; k < mesh->vtxCount; k++) {
                    s32 axis;
                    for (axis = 0; axis < 3; axis += 2) {
                        s32 v = (s16)mesh->vtx[k].v.ob[axis];
                        if (v < 0) {
                            neg = -v;
                            v = neg / 25;
                            v += (neg % 25 >= 13);
                            v *= 25;
                            v = -v;
                        } else {
                            v = v / 25 + (v % 25 >= 13);
                            v *= 25;
                        }
                        mesh->vtx[k].v.ob[axis] = v;
                    }
                }
                j++;
            } while (j < gPieceTypes[i].meshA->count);
        }
        if (gPieceTypes[i].pathId != 0) {
            gPieceTypes[i].path = (s32 *)(res_load(sPiecesResSet, *gPieceTypes[i].pathId) + 4);
        }
        if (gPieceTypes[i].collisionId != 0) {
            gPieceTypes[i].collision = (WallMesh *)res_load(sPiecesResSet, *gPieceTypes[i].collisionId);
            height_free_walls();
        }
        }
    }

    /* the user pieces, cloned from the template (types 18 to 29: one past the table) */
    for (i = PIECE_TYPE_USER, userIdx = 0; i < 30; i++, userIdx++) {
        if (!(gPieceTypes[i].flags & PIECE_USER)) continue;
        gPieceTypes[i].meshA = mem_alloc_dbg(gUserTrackLayout->segmentCounts[userIdx] << 6 | 4, 0, sStrUsrTrkFile, 0x27F);
        gPieceTypes[i].meshB = mem_alloc_dbg(gUserTrackLayout->segmentCounts[userIdx] << 6 | 4, 0, sStrUsrTrkFile, 0x280);
        gPieceTypes[i].dim[1] = gUserTrackLayout->segmentCounts[userIdx];
        if (gPieceTypes[i].pathId != 0) {
            gPieceTypes[i].path = (s32 *)(res_load(sPiecesResSet, *gPieceTypes[i].pathId) + 4);
        }
        if (gPieceTypes[i].collisionId != 0) {
            gPieceTypes[i].collision = (WallMesh *)res_load(sPiecesResSet, *gPieceTypes[i].collisionId);
            height_free_walls();
        }
        usrtrk_clone_piece_meshes(gPieceTypes[i].meshA, gPieceTypes[i].meshB, gUserTrackLayout->segmentCounts[userIdx]);
        usrtrk_apply_heights(gPieceTypes[i].meshA, gPieceTypes[i].meshB, gUserTrackLayout->heights[userIdx], gUserTrackLayout->segmentCounts[userIdx]);
        gPieceTypes[i].dim[1] = gUserTrackLayout->segmentCounts[userIdx];
    }
}

/* Sets ground vertex `idx` of a cell to height point `point` (0 at the piece's ends) */
#define SET_GROUND_HEIGHT(mesh, idx, point) \
    (*(mesh))->vtx[idx].v.ob[1] = ((point) > 0 && (point) < segments * 2) ? heights[(point) - 1] : 0

/* Sets the heights of a user piece's meshes (segments rows of 2 cells) from heights, the 2 * segments - 1 points
   half a cell apart between the piece's ends (which stay at 0), on the template piece's vertices: the objects' meshes
   follow the heights interpolated along the piece, the ground's vertices take the points' heights. */
void usrtrk_apply_heights(PieceMeshSet *ground, PieceMeshSet *objects, s16 *heights, s32 segments)
{
    PieceMeshSet *tmpl = gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;
    s32 i, j, k;
    s32 frac, h0, h1;
    u32 m;
    PieceMesh **dstMesh, **srcMesh, **tmplMesh;

    k = 0;
    for (i = 0; i < segments; i++) {
        for (j = 0; j < 2; j++) {
            srcMesh = &objects->cells[i * 2 + j].mesh;
            tmplMesh = &tmpl->cells[j].mesh;
            dstMesh = &ground->cells[i * 2 + j].mesh;
            for (m = 0; m < (*srcMesh)->vtxCount; m++) {
                {
                    s32 point = i * 2 + ((*srcMesh)->vtx[m].v.ob[2] + 250) / 250;
                    if (point > 0 && point < segments * 2) h0 = heights[point - 1]; else h0 = 0;
                }
                {
                    s32 point = i * 2 + ((*srcMesh)->vtx[m].v.ob[2] + 250) / 250;
                    s32 nextPoint = point + 1;
                    if (nextPoint > 0 && nextPoint < segments * 2) h1 = heights[point]; else h1 = 0;
                }
                frac = ((*srcMesh)->vtx[m].v.ob[2] + 250) % 250;
                (*srcMesh)->vtx[m].v.ob[1] = (*tmplMesh)->vtx[m].v.ob[1] + h0 + (h1 - h0) * frac / 250;
            }
            if (j == 0) {
                SET_GROUND_HEIGHT(dstMesh, 4, k);
                SET_GROUND_HEIGHT(dstMesh, 10, k);
                SET_GROUND_HEIGHT(dstMesh, 11, k);
                SET_GROUND_HEIGHT(dstMesh, 1, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 6, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 7, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 3, k + 2);
                SET_GROUND_HEIGHT(dstMesh, 8, k + 2);
                SET_GROUND_HEIGHT(dstMesh, 9, k + 2);
            } else {
                SET_GROUND_HEIGHT(dstMesh, 4, k);
                SET_GROUND_HEIGHT(dstMesh, 5, k);
                SET_GROUND_HEIGHT(dstMesh, 11, k);
                SET_GROUND_HEIGHT(dstMesh, 1, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 6, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 0, k + 1);
                SET_GROUND_HEIGHT(dstMesh, 3, k + 2);
                SET_GROUND_HEIGHT(dstMesh, 8, k + 2);
                SET_GROUND_HEIGHT(dstMesh, 2, k + 2);
            }
        }
        k += 2;
    }
}

/* Fills a user piece's two mesh sets with `count` copies (segments) of the template piece's cells, each cell's mesh,
   triangles and ground grid in one allocation. */
void usrtrk_clone_piece_meshes(PieceMeshSet *outA, PieceMeshSet *outB, s32 count)
{
    PieceMeshSet *tmplA;
    PieceMeshSet *tmplB;
    s32 seg;
    u32 i;
    u32 cellsPerSeg;
    PieceCell *cellA;
    PieceCell *cellB;
    s32 gridSizeA;
    s32 gridSizeB;

    tmplA = gPieceTypes[PIECE_TYPE_TEMPLATE].meshA;
    tmplB = gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;
    for (seg = 0; seg < count; seg++) {
        for (i = 0; i < (cellsPerSeg = tmplA->count); i++) {
            gridSizeA = (tmplA->cells[i].grid[1] << 4) | 4;
            cellA = &outA->cells[seg * cellsPerSeg + i];
            cellB = &outB->cells[seg * tmplB->count + i];
            cellA->mesh = mem_alloc_dbg(sizeof(PieceMesh) + tmplA->cells[i].mesh->vtxCount * sizeof(Vtx) + tmplA->cells[i].mesh->triCount * 4 + gridSizeA, 0, (s32)sStrUsrTrkFile, 0x2F8);
            *cellA->mesh = *tmplA->cells[i].mesh;
            cellA->visible = tmplA->cells[i].visible;
            cellA->mask = tmplA->cells[i].mask;
            cellA->x0 = tmplA->cells[i].x0;
            cellA->y0 = tmplA->cells[i].y0;
            cellA->x1 = tmplA->cells[i].x1;
            cellA->y1 = tmplA->cells[i].y1;
            cellA->mesh->vtxCount = tmplA->cells[i].mesh->vtxCount;
            cellA->mesh->vtx = (Vtx *)(cellA->mesh + 1);
            cellA->mesh->tris = (u32 *)(cellA->mesh->vtx + cellA->mesh->vtxCount);
            cellA->mesh->triCount = tmplA->cells[i].mesh->triCount;
            cellA->grid = (s16 *)(cellA->mesh->tris + cellA->mesh->triCount);
            memcpy(cellA->mesh->vtx, tmplA->cells[i].mesh->vtx, cellA->mesh->vtxCount * sizeof(Vtx));
            memcpy(cellA->mesh->tris, tmplA->cells[i].mesh->tris, cellA->mesh->triCount * 4);
            memcpy(cellA->grid, tmplA->cells[i].grid, gridSizeA);

            gridSizeB = (tmplB->cells[i].grid[1] << 4) | 4;
            cellB->mesh = mem_alloc_dbg(sizeof(PieceMesh) + tmplB->cells[i].mesh->vtxCount * sizeof(Vtx) + tmplB->cells[i].mesh->triCount * 4 + gridSizeB, 0, (s32)sStrUsrTrkFile, 0x30C);
            *cellB->mesh = *tmplB->cells[i].mesh;
            cellB->visible = tmplB->cells[i].visible;
            cellB->mask = tmplB->cells[i].mask;
            cellB->x0 = tmplB->cells[i].x0;
            cellB->y0 = tmplB->cells[i].y0 + seg * CELL_VTX_UNITS;
            cellB->x1 = tmplB->cells[i].x1;
            cellB->y1 = tmplB->cells[i].y1 + seg * CELL_VTX_UNITS;
            cellB->mesh->vtxCount = tmplB->cells[i].mesh->vtxCount;
            cellB->mesh->vtx = (Vtx *)(cellB->mesh + 1);
            cellB->mesh->tris = (u32 *)(cellB->mesh->vtx + cellB->mesh->vtxCount);
            cellB->mesh->triCount = tmplB->cells[i].mesh->triCount;
            cellB->grid = (s16 *)(cellB->mesh->tris + cellB->mesh->triCount);
            memcpy(cellB->mesh->vtx, tmplB->cells[i].mesh->vtx, cellB->mesh->vtxCount * sizeof(Vtx));
            memcpy(cellB->mesh->tris, tmplB->cells[i].mesh->tris, cellB->mesh->triCount * 4);
            memcpy(cellB->grid, tmplB->cells[i].grid, gridSizeB);
        }
    }
}

/* After a race on the user track: closes the "Pieces" resources and frees what usrtrk_build_track and
   usrtrk_load_pieces made. */
void usrtrk_unload(void)
{
    res_close_set(sPiecesResSet);
    sPiecesResSet = 0;
    if (gUserTrackData.meshA != 0) {
        mem_free(gUserTrackData.meshA);
        mem_free(gUserTrackData.meshB);
        mem_free(gUserTrackData.grids);
        mem_free(gUserTrackData.vtxA);
        mem_free(gUserTrackData.meshesA);
        mem_free(gUserTrackData.vtxB);
        mem_free(gUserTrackData.meshesB);
        if (gUserTrackData.collision->vtx != 0) {
            mem_free(gUserTrackData.collision->vtx);
        }
        if (gUserTrackData.collision->tris != 0) {
            mem_free(gUserTrackData.collision->tris);
        }
        if (gUserTrackData.collision->edges != 0) {
            mem_free(gUserTrackData.collision->edges);
        }
        if (gUserTrackData.collision->cells[0].tris != 0) {
            mem_free(gUserTrackData.collision->cells[0].tris);
        }
        if (gUserTrackData.collision->cells[0].edges != 0) {
            mem_free(gUserTrackData.collision->cells[0].edges);
        }
        mem_free(gUserTrackData.collision);
    }
    usrtrk_free_user_piece_meshes(0);
    height_free_walls();
}

static inline void free_cell_meshes(PieceType *type, s32 cell)
{
    PieceMesh **meshA;
    PieceMesh **meshB;

    meshA = &type->meshA->cells[cell].mesh;
    meshB = &type->meshB->cells[cell].mesh;
    mem_free(*meshA);
    mem_free(*meshB);
}

/* Frees the user pieces' cloned meshes: their segments, or all 8 the editor makes (allSegments). Types 18 to 29, one
   past the table. */
void usrtrk_free_user_piece_meshes(s32 allSegments)
{
    s32 i;
    s32 j;
    s32 n;

    for (n = PIECE_TYPE_USER; n < 30; n++) {
        if (gPieceTypes[n].flags & PIECE_USER) {
            for (i = 0; allSegments == 0 ? i < gPieceTypes[n].dim[1] : i < USER_PIECE_MAX_SEGMENTS; i++) {
                for (j = 0; j < 2; j++) {
                    free_cell_meshes(&gPieceTypes[n], i * 2 + j);
                }
            }
            mem_free(gPieceTypes[n].meshA);
            mem_free(gPieceTypes[n].meshB);
        }
    }
}

#define LAYOUT_PIECE (gUserTrackLayout->pieces[i])
#define LAYOUT_TYPE (gPieceTypes[gUserTrackLayout->pieces[i].type])
#define TYPE (gPieceTypes[piece->type])
#define ROT (sPieceRotations[grid[c].rot])
#define CELL (grid[gy * size[0] + gx])
#define TYPE_W (TYPE.dim[0])
#define TYPE_H (TYPE.dim[1])
/* The cell of the piece at (col, row) of the grid, for each rotation */
#define CELL_ROT0 (row * TYPE_W + col)
#define CELL_ROT1 (col * TYPE_W + TYPE_W - 1 - row)
#define CELL_ROT2 ((TYPE_H - 1 - row) * TYPE_W + TYPE_W - 1 - col)
#define CELL_ROT3 ((TYPE_H - 1 - col) * TYPE_W + row)
#define MIN_2(a, b) ((a) < (b) ? (a) : (b))
#define MAX_2(a, b) ((a) > (b) ? (a) : (b))

/* The 8x8 cell bits of a ground triangle (8 rows of a byte) turned with the piece: REVERSE_BITS mirrors a row, the
   others build a row of the transposed matrix from bit k of every source row. */
#define REVERSE_BITS(x)                                                                                      \
    ((((x) >> 7) & 1) | (((x) >> 5) & 2) | (((x) >> 3) & 4) | (((x) >> 1) & 8) |                             \
     (((x) << 1) & 0x10) | (((x) << 3) & 0x20) | (((x) << 5) & 0x40) | (((x) << 7) & 0x80))
/* output bit (7 - i) from source row i's bit k */
#define BL(i, k) ((rows[i] << (7 - (i) - (k))) & (0x80 >> (i)))
#define BR(i, k) ((rows[i] >> ((k) - 7 + (i))) & (0x80 >> (i)))
#define T80                                                                                                  \
    ((rows[0] & 0x80) | ((rows[1] & 0x80) >> 1) | ((rows[2] & 0x80) >> 2) | ((rows[3] & 0x80) >> 3) |        \
     ((rows[4] & 0x80) >> 4) | ((rows[5] & 0x80) >> 5) | ((rows[6] & 0x80) >> 6) | ((rows[7] & 0x80) >> 7))
#define T01                                                                                                  \
    (((rows[0] & 1) << 7) | ((rows[1] & 1) << 6) | ((rows[2] & 1) << 5) | ((rows[3] & 1) << 4) |             \
     ((rows[4] & 1) << 3) | ((rows[5] & 1) << 2) | ((rows[6] & 1) << 1) | (rows[7] & 1))
#define BX(i, m, k) ((7 - (i)) > (k) ? BL(i, k) : (7 - (i)) == (k) ? (rows[i] & (m)) : BR(i, k))
#define TM(m, k)                                                                                             \
    (((rows[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) |                \
     BX(5, m, k) | BX(6, m, k) | ((rows[7] & (m)) >> (k)))
#define TS(m, k)                                                                                             \
    (((rows[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) |                \
     BX(5, m, k) | BX(6, m, k) | ({ s32 z; if (rows[7] & (m)) z = 1; else z = 0; z; }))
/* the triangle's vertices and flags, unchanged */
#define COPY8                                                                                                \
    out[8] = rows[8]; out[9] = rows[9]; out[10] = rows[10]; out[11] = rows[11];                               \
    out[12] = rows[12]; out[13] = rows[13]; out[14] = rows[14]; out[15] = rows[15]

/* A grid cell of the track being built: the piece cell drawn there. */
typedef struct {
    PieceType *type;
    PieceCell *cellA;
    PieceCell *cellB;
    s16 rot, col, row, ox, oy;      /* ox, oy: where the B mesh's cell sits in the grid cell */
    s16 pad;
} UsrTrkGridCell;

/* A node of the path being built (header 0xAA000003: a PathResNode and the PATH_RES_NODE_FLAGS byte). */
typedef struct {
    PathResNode node;
    u8 flags;
    u8 pad29[3];
} UsrTrkPathRec; /* size 0x2C */

/* sPieceRotations: a vertex of a piece turned by rot quarter turns into a cell is
   (ob[xSrc] * xSign + xOff, ob[zSrc] * zSign + zOff), from the cell's corner. */
typedef struct {
    s16 xSrc, zSrc, xOff, zOff, xSign, zSign;
} PieceRotation;

/* gUserTrackData.path while the nodes are added (after the header word). */
typedef struct UsrTrkPathList {
    s32 count;
    UsrTrkPathRec nodes[1];
} UsrTrkPathList;

typedef struct {
    u8 pad[0x68];
    Vec3f v68;
} UsrTrkTrackView;

extern PieceRotation sPieceRotations[];
extern UsrTrkTrackView *gCurTrack_42;

#define PATH_LIST ((UsrTrkPathList *)gUserTrackData.path)

/* Builds the user track of gUserTrackLayout for a race (gUserTrackData): lays the pieces' cells on the grid (empty
   ground elsewhere), merges the arena's and the pieces' walls into one wall mesh (welding vertices less than 4 units
   apart) with its 16x16 hash, builds the racing line from the pieces' paths (every 5th point) and the two mesh sets
   with each cell's vertices and ground triangle grid turned into place. */
void usrtrk_build_track(void)
{
    Vtx *vtxA;
    PieceMesh *meshA;
    PieceCell *cellA;
    Vtx *vtxB;
    PieceMesh *meshB;
    PieceCell *cellB;
    s16 *triLists;
    s16 *edgeLists;
    s32 vtxBytesA;
    s32 vtxBytesB;
    s32 numVtx;
    s32 numEdges;
    s32 numTris;
    s32 tri;
    s32 numCells;
    s32 bitmapWords;
    s32 edgeBase;
    s32 triBase;
    s32 copies;
    s16 lo[2];
    s16 hi[2];
    s16 size[2];
    Vec3f pos;
    s32 pts[6];
    UsrTrkGridCell *grid;
    s32 i, j, k, m;
    s32 gx, gy, col, row;
    s32 numPathPts, gridBytes;
    s32 vtxBase;
    s32 minx, maxx, miny, maxy;
    s32 tminx, tmaxx, tminy, tmaxy;
    s32 triCellW, triCellH;
    s32 edgeCellW, edgeCellH;
    s32 c, n;
    s32 dx, dy;
    s32 triBytes, edgeBytes;
    PieceType *type;
    UserTrackPiece *piece;
    s32 v, elem, other;
    Vtx *src;
    UsrTrkPathRec *rec;
    PieceCell *objCell;
    PieceType *ptype;
    s32 pathPos;
    s16 *gridOut;
    u8 *rows;
    u8 *out;
    s32 halfW, halfH;
    s32 kk;
    s32 kr;

    gridBytes = 0;
    vtxBytesA = 0;
    vtxBytesB = 0;
    /* the grid: at least 15 x 15, as big as the pieces reach */
    size[0] = -32000;
    size[1] = -32000;
    for (i = 0; i < gUserTrackLayout->pieceCount; i++) {
        if (size[0] < LAYOUT_PIECE.x + LAYOUT_TYPE.dim[LAYOUT_PIECE.rot & 1]) {
            size[0] = LAYOUT_PIECE.x + LAYOUT_TYPE.dim[LAYOUT_PIECE.rot & 1];
        }
        if (size[1] < LAYOUT_PIECE.y + LAYOUT_TYPE.dim[(LAYOUT_PIECE.rot + 1) & 1]) {
            size[1] = LAYOUT_PIECE.y + LAYOUT_TYPE.dim[(LAYOUT_PIECE.rot + 1) & 1];
        }
    }
    size[0] = (size[0] < 15) ? 15 : size[0];
    size[1] = (size[1] < 15) ? 15 : size[1];
    numPathPts = 0;
    numCells = size[0] * size[1];
    grid = mem_alloc_dbg(numCells * sizeof(UsrTrkGridCell), 0, sStrUsrTrkFile, 0x416);
    mem_fill(grid, 0, numCells * sizeof(UsrTrkGridCell));
    numEdges = gEditArena.collision->numEdges;
    numVtx = gEditArena.collision->numVtx;
    numTris = gEditArena.collision->numTris;
    /* count the path points and the walls, and put each piece's cells on the grid */
    for (i = 0, piece = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; piece++, i++) {
        if (TYPE.flags & PIECE_USER) {
            numPathPts += TYPE.dim[1] * 8;
        } else {
            numPathPts += *TYPE.path - 1;
        }
        if (TYPE.collision != NULL) {
            if (TYPE.flags & PIECE_USER) {
                m = TYPE.dim[1] * 2;
                numVtx += TYPE.collision->numVtx * m;
                numTris += TYPE.collision->numTris * m;
                numEdges += TYPE.collision->numEdges * m;
            } else {
                numVtx += TYPE.collision->numVtx;
                numTris += TYPE.collision->numTris;
                numEdges += TYPE.collision->numEdges;
            }
        }
        lo[0] = piece->x;
        lo[1] = piece->y;
        hi[0] = lo[0] + TYPE.dim[piece->rot & 1];
        hi[1] = lo[1] + TYPE.dim[(piece->rot + 1) & 1];
        for (gy = lo[1], row = 0; gy < hi[1]; gy++, row++) {
            for (gx = lo[0], col = 0; gx < hi[0]; gx++, col++) {
                CELL.rot = piece->rot;
                CELL.type = &TYPE;
                CELL.col = col;
                CELL.row = row;
                switch (piece->rot) {
                case 0:
                    CELL.cellA = &TYPE.meshA->cells[CELL_ROT0];
                    CELL.cellB = TYPE.meshB ? &TYPE.meshB->cells[CELL_ROT0] : NULL;
                    break;
                case 1:
                    CELL.cellA = &TYPE.meshA->cells[CELL_ROT1];
                    CELL.cellB = TYPE.meshB ? &TYPE.meshB->cells[CELL_ROT1] : NULL;
                    break;
                case 2:
                    CELL.cellA = &TYPE.meshA->cells[CELL_ROT2];
                    CELL.cellB = TYPE.meshB ? &TYPE.meshB->cells[CELL_ROT2] : NULL;
                    break;
                case 3:
                    CELL.cellA = &TYPE.meshA->cells[CELL_ROT3];
                    CELL.cellB = TYPE.meshB ? &TYPE.meshB->cells[CELL_ROT3] : NULL;
                    break;
                }
                objCell = CELL.cellB;
                if (objCell != NULL) {
                    ptype = &TYPE;
                    switch (piece->rot) {
                    case 0:
                        CELL.ox = (objCell->x0 + objCell->x1) / 2 - 250 - col * 500;
                        CELL.oy = (objCell->y0 + objCell->y1) / 2 - 250 - row * 500;
                        break;
                    case 1:
                        CELL.ox = (objCell->y0 + objCell->y1) / 2 - 250 - col * 500;
                        CELL.oy = (ptype->dim[0] - row - 1) * 500 + (250 - (objCell->x0 + objCell->x1) / 2);
                        break;
                    case 2:
                        CELL.ox = (ptype->dim[0] - col - 1) * 500 + (250 - (objCell->x0 + objCell->x1) / 2);
                        CELL.oy = (ptype->dim[1] - row - 1) * 500 + (250 - (objCell->y0 + objCell->y1) / 2);
                        break;
                    case 3:
                        CELL.ox = (ptype->dim[1] - col - 1) * 500 + (250 - (objCell->y0 + objCell->y1) / 2);
                        CELL.oy = (objCell->x0 + objCell->x1) / 2 - 250 - row * 500;
                        break;
                    }
                }
            }
        }
    }
    for (c = 0; c < numCells; c++) {
        if (grid[c].cellA == NULL) {
            grid[c].rot = 0;
            grid[c].type = &gPieceTypes[PIECE_TYPE_GROUND];
            grid[c].cellA = &gPieceTypes[PIECE_TYPE_GROUND].meshA->cells[0];
            grid[c].col = 0;
            grid[c].row = 0;
            if (gPieceTypes[PIECE_TYPE_GROUND].meshB != NULL) {
                grid[c].cellB = &gPieceTypes[PIECE_TYPE_GROUND].meshB->cells[0];
            } else {
                grid[c].cellB = NULL;
            }
        }
        vtxBytesA += grid[c].cellA->mesh->vtxCount * sizeof(Vtx);
        if (grid[c].cellB != NULL) {
            vtxBytesB += grid[c].cellB->mesh->vtxCount * sizeof(Vtx);
        }
        if (grid[c].cellA->grid[0] & 1) {
            gridBytes += (grid[c].cellA->grid[1] * 16) | 4;
        } else {
            gridBytes += 0x54;
        }
    }
    switch (gUserTrackLayout->dirtType) {
    case 0:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, sDirtTex0ResId);
        break;
    case 1:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, sDirtTex1ResId);
        break;
    case 2:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, sDirtTex2ResId);
        break;
    }
    gUserTrackData.objectTex = res_load(sPiecesResSet, gPieceObjectTexResId);
    gUserTrackData.meshA = mem_alloc_dbg(numCells * sizeof(PieceCell) | 4, 0, sStrUsrTrkFile, 0x4F5);
    gUserTrackData.meshA->count = numCells;
    gUserTrackData.meshB = mem_alloc_dbg(numCells * sizeof(PieceCell) | 4, 0, sStrUsrTrkFile, 0x4F8);
    gUserTrackData.meshB->count = numCells;
    gUserTrackData.path = mem_alloc_dbg((numPathPts / 5 + 1) * sizeof(UsrTrkPathRec) + 8, 0, sStrUsrTrkFile, 0x4FB);
    /* one cell short of a WallMesh: the cell lists below also set cells[256], past the end */
    gUserTrackData.collision = mem_alloc_dbg(sizeof(WallMesh) - sizeof(WallCell), 0, sStrUsrTrkFile, 0x4FC);
    gUserTrackData.collision->vtx = mem_alloc_dbg(numVtx * sizeof(WallVtx), 0, sStrUsrTrkFile, 0x4FD);
    gUserTrackData.collision->edges = mem_alloc_dbg(numEdges * sizeof(WallEdge), 0, sStrUsrTrkFile, 0x4FE);
    gUserTrackData.collision->tris = mem_alloc_dbg(numTris * sizeof(WallTri), 0, sStrUsrTrkFile, 0x4FF);
    /* the 16x16 hash's lists while they are built: per cell a count and up to 64 triangles / 128 edges */
    triLists = mem_alloc_dbg(0x8200, 0, sStrUsrTrkFile, 0x500);
    edgeLists = mem_alloc_dbg(0x10200, 0, sStrUsrTrkFile, 0x501);
    halfW = size[0] * (CELL_VTX_UNITS / 2);
    halfH = size[1] * (CELL_VTX_UNITS / 2);
    gUserTrackData.grids = mem_alloc_dbg(gridBytes, 0, sStrUsrTrkFile, 0x508);
    gUserTrackData.vtxA = mem_alloc_dbg(vtxBytesA, 0, sStrUsrTrkFile, 0x509);
    gUserTrackData.meshesA = mem_alloc_dbg(numCells * sizeof(PieceMesh), 0, sStrUsrTrkFile, 0x50A);
    gUserTrackData.vtxB = mem_alloc_dbg(vtxBytesB, 0, sStrUsrTrkFile, 0x50C);
    gUserTrackData.meshesB = mem_alloc_dbg(numCells * sizeof(PieceMesh), 0, sStrUsrTrkFile, 0x50D);
    edgeBase = gEditArena.collision->numEdges;
    vtxBase = gEditArena.collision->numVtx;
    triBase = gEditArena.collision->numTris;
    gUserTrackData.collision->numVtx = numVtx;
    gUserTrackData.collision->numTris = numTris;
    gUserTrackData.collision->numEdges = numEdges;
    gUserTrackData.collision->maxX = -1000000;
    gUserTrackData.collision->maxZ = -1000000;
    gUserTrackData.collision->minX = 1000000;
    gUserTrackData.collision->minZ = 1000000;
    /* the arena's walls first */
    for (j = 0; j < gEditArena.collision->numVtx; j++) {
        gUserTrackData.collision->vtx[j].x = gEditArena.collision->vtx[j].x;
        gUserTrackData.collision->vtx[j].z = gEditArena.collision->vtx[j].z;
        gUserTrackData.collision->vtx[j].y = gEditArena.collision->vtx[j].y;
        gUserTrackData.collision->vtx[j].h = gEditArena.collision->vtx[j].h;
    }
    for (j = 0; j < gEditArena.collision->numEdges; j++) {
        gUserTrackData.collision->edges[j].v0 = gEditArena.collision->edges[j].v0;
        gUserTrackData.collision->edges[j].v1 = gEditArena.collision->edges[j].v1;
        gUserTrackData.collision->edges[j].nx = -0x8000;
        gUserTrackData.collision->edges[j].nz = 0x40;
    }
    for (j = 0; j < gEditArena.collision->numTris; j++) {
        gUserTrackData.collision->tris[j].v[0] = gEditArena.collision->tris[j].v[0];
        gUserTrackData.collision->tris[j].v[1] = gEditArena.collision->tris[j].v[1];
        gUserTrackData.collision->tris[j].v[2] = gEditArena.collision->tris[j].v[2];
        gUserTrackData.collision->tris[j].nx = gEditArena.collision->tris[j].nx;
        gUserTrackData.collision->tris[j].ny = gEditArena.collision->tris[j].ny;
        gUserTrackData.collision->tris[j].nz = gEditArena.collision->tris[j].nz;
    }
    /* the z-buffer as scratch: a bitmap of the welded vertices, then each vertex's piece (-1 the arena) */
    bitmapWords = (numVtx + 15) / 16;
    mem_fill(gZBuffer_2 + bitmapWords, 0xFF, numVtx * 2);
    /* each piece's walls turned and moved into place; a user piece's once per half segment, raised by its heights */
    for (i = 0, piece = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; piece++, i++) {
        type = &gPieceTypes[piece->type];
        if (type->collision != NULL) {
            gUserTrackData.collision->minX = -3750;
            gUserTrackData.collision->maxX = 3750;
            gUserTrackData.collision->minZ = -3750;
            gUserTrackData.collision->maxZ = 3750;
            if (type->flags & PIECE_USER) {
                copies = type->dim[1] * 2;
            } else {
                copies = 1;
            }
            for (k = 0; k < copies; k++) {
                for (j = 0; j < type->collision->numVtx; j++) {
                    gZBuffer_2[bitmapWords + vtxBase + j] = i;
                    gUserTrackData.collision->vtx[vtxBase + j].y = type->collision->vtx[j].y;
                    gUserTrackData.collision->vtx[vtxBase + j].h = type->collision->vtx[j].h;
                    switch (piece->rot) {
                    case 0:
                        gUserTrackData.collision->vtx[vtxBase + j].x = type->collision->vtx[j].x;
                        gUserTrackData.collision->vtx[vtxBase + j].z = type->collision->vtx[j].z;
                        if (type->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k - 1];
                            }
                        } else if (k != copies - 1) {
                            gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k];
                        }
                        gUserTrackData.collision->vtx[vtxBase + j].x += piece->x * CELL_WALL_UNITS - halfW / 4;
                        gUserTrackData.collision->vtx[vtxBase + j].z += piece->y * CELL_WALL_UNITS - halfH / 4 + (k * 4000) / 64;
                        break;
                    case 1:
                        gUserTrackData.collision->vtx[vtxBase + j].x = type->collision->vtx[j].z;
                        gUserTrackData.collision->vtx[vtxBase + j].z = type->dim[0] * CELL_WALL_UNITS - type->collision->vtx[j].x;
                        if (type->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k - 1];
                            }
                        } else if (k != copies - 1) {
                            gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k];
                        }
                        gUserTrackData.collision->vtx[vtxBase + j].x += piece->x * CELL_WALL_UNITS - halfW / 4 + (k * 4000) / 64;
                        gUserTrackData.collision->vtx[vtxBase + j].z += piece->y * CELL_WALL_UNITS - halfH / 4;
                        break;
                    case 2:
                        gUserTrackData.collision->vtx[vtxBase + j].x = type->dim[0] * CELL_WALL_UNITS - type->collision->vtx[j].x;
                        gUserTrackData.collision->vtx[vtxBase + j].z = type->dim[1] * CELL_WALL_UNITS - type->collision->vtx[j].z;
                        if (type->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k - 1];
                            }
                        } else if (k != copies - 1) {
                            gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][k];
                        }
                        gUserTrackData.collision->vtx[vtxBase + j].x += piece->x * CELL_WALL_UNITS - halfW / 4;
                        gUserTrackData.collision->vtx[vtxBase + j].z += piece->y * CELL_WALL_UNITS - halfH / 4 - (k * 4000) / 64;
                        break;
                    case 3:
                        gUserTrackData.collision->vtx[vtxBase + j].x = type->dim[1] * CELL_WALL_UNITS - type->collision->vtx[j].z;
                        gUserTrackData.collision->vtx[vtxBase + j].z = type->collision->vtx[j].x;
                        if (type->collision->vtx[j].z == 0) {
                            kk = copies - k - 1;
                            if (kk != 0) {
                                gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][copies - k - 2];
                            }
                        } else if ((kr = copies - k - 1) != copies - 1) {
                            gUserTrackData.collision->vtx[vtxBase + j].h += gUserTrackLayout->heights[type - &gPieceTypes[PIECE_TYPE_USER]][kr];
                        }
                        gUserTrackData.collision->vtx[vtxBase + j].x += piece->x * CELL_WALL_UNITS - halfW / 4 - ((copies - k - 1) * 4000) / 64;
                        gUserTrackData.collision->vtx[vtxBase + j].z += piece->y * CELL_WALL_UNITS - halfH / 4;
                        break;
                    }
                }
                for (j = 0; j < type->collision->numEdges; j++) {
                    gUserTrackData.collision->edges[edgeBase + j].v0 = type->collision->edges[j].v0 + vtxBase;
                    gUserTrackData.collision->edges[edgeBase + j].v1 = type->collision->edges[j].v1 + vtxBase;
                    switch (piece->rot) {
                    case 0:
                        gUserTrackData.collision->edges[edgeBase + j].nx = type->collision->edges[j].nx;
                        gUserTrackData.collision->edges[edgeBase + j].nz = type->collision->edges[j].nz;
                        break;
                    case 1:
                        gUserTrackData.collision->edges[edgeBase + j].nx = type->collision->edges[j].nz;
                        gUserTrackData.collision->edges[edgeBase + j].nz = -type->collision->edges[j].nx;
                        break;
                    case 2:
                        gUserTrackData.collision->edges[edgeBase + j].nx = -type->collision->edges[j].nx;
                        gUserTrackData.collision->edges[edgeBase + j].nz = -type->collision->edges[j].nz;
                        break;
                    case 3:
                        gUserTrackData.collision->edges[edgeBase + j].nx = -type->collision->edges[j].nz;
                        gUserTrackData.collision->edges[edgeBase + j].nz = type->collision->edges[j].nx;
                        break;
                    }
                    gUserTrackData.collision->edges[edgeBase + j].nx &= ~1;
                    gUserTrackData.collision->edges[edgeBase + j].nz &= ~1;
                }
                for (j = 0; j < type->collision->numTris; j++) {
                    gUserTrackData.collision->tris[triBase + j].v[0] = type->collision->tris[j].v[0] + vtxBase;
                    gUserTrackData.collision->tris[triBase + j].v[1] = type->collision->tris[j].v[1] + vtxBase;
                    gUserTrackData.collision->tris[triBase + j].v[2] = type->collision->tris[j].v[2] + vtxBase;
                    switch (piece->rot) {
                    case 0:
                        gUserTrackData.collision->tris[triBase + j].nx = type->collision->tris[j].nx;
                        gUserTrackData.collision->tris[triBase + j].nz = type->collision->tris[j].nz;
                        break;
                    case 1:
                        gUserTrackData.collision->tris[triBase + j].nx = type->collision->tris[j].nz;
                        gUserTrackData.collision->tris[triBase + j].nz = -type->collision->tris[j].nx;
                        break;
                    case 2:
                        gUserTrackData.collision->tris[triBase + j].nx = -type->collision->tris[j].nx;
                        gUserTrackData.collision->tris[triBase + j].nz = -type->collision->tris[j].nz;
                        break;
                    case 3:
                        gUserTrackData.collision->tris[triBase + j].nx = -type->collision->tris[j].nz;
                        gUserTrackData.collision->tris[triBase + j].nz = type->collision->tris[j].nx;
                        break;
                    }
                    gUserTrackData.collision->tris[triBase + j].ny = type->collision->tris[j].ny;
                }
                edgeBase += type->collision->numEdges;
                vtxBase += type->collision->numVtx;
                triBase += type->collision->numTris;
            }
        }
    }
    /* weld the pieces' vertices that are less than 4 units apart (each at most once) */
    mem_fill(gZBuffer_2, 0, bitmapWords * 2);
    for (j = gEditArena.collision->numVtx; j < numVtx; j++) {
        if (!((gZBuffer_2[j / 16] >> (j % 16)) & 1)) {
            for (other = j + 1; other < numVtx; other++) {
                if (!((gZBuffer_2[other / 16] >> (other % 16)) & 1)) {
                    dx = gUserTrackData.collision->vtx[j].x - gUserTrackData.collision->vtx[other].x;
                    dy = gUserTrackData.collision->vtx[j].z - gUserTrackData.collision->vtx[other].z;
                    if (dx * dx + dy * dy < 16) {
                        gUserTrackData.collision->vtx[other].x += dx / 2;
                        gUserTrackData.collision->vtx[other].z += dy / 2;
                        gUserTrackData.collision->vtx[j].x -= dx - dx / 2;
                        gUserTrackData.collision->vtx[j].z -= dy - dy / 2;
                        gZBuffer_2[other / 16] |= 1 << (other % 16);
                        gZBuffer_2[j / 16] |= 1 << (j % 16);
                        break;
                    }
                }
            }
        }
    }
    /* the 16x16 hash: each edge and triangle listed in the cells its box touches */
    for (c = 0; c < 256; c++) {
        triLists[c * 65] = 0;
        edgeLists[c * 129] = 0;
    }
    for (elem = 0; elem < gUserTrackData.collision->numEdges; elem++) {
        pts[0] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[elem].v0].x + halfW / 4;
        pts[1] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[elem].v0].z + halfH / 4;
        pts[2] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[elem].v1].x + halfW / 4;
        pts[3] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[elem].v1].z + halfH / 4;
        minx = MIN_2(pts[0], pts[2]);
        maxx = MAX_2(pts[0], pts[2]);
        miny = MIN_2(pts[1], pts[3]);
        maxy = MAX_2(pts[1], pts[3]);
        edgeCellW = size[0] * CELL_WALL_UNITS / 16;
        edgeCellH = size[1] * CELL_WALL_UNITS / 16;
        for (gy = miny / edgeCellH; gy <= maxy / edgeCellH; gy++) {
            for (gx = minx / edgeCellW; gx <= maxx / edgeCellW; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    edgeLists[c * 129 + ++edgeLists[c * 129]] = elem;
                }
            }
        }
    }
    for (elem = 0; elem < gUserTrackData.collision->numTris; elem++) {
        pts[0] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[0]].x + halfW / 4;
        pts[1] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[0]].z + halfH / 4;
        pts[2] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[1]].x + halfW / 4;
        pts[3] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[1]].z + halfH / 4;
        pts[4] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[2]].x + halfW / 4;
        pts[5] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[elem].v[2]].z + halfH / 4;
        tminx = MIN_2(MIN_2(pts[0], pts[2]), pts[4]);
        tmaxx = MAX_2(MAX_2(pts[0], pts[2]), pts[4]);
        tminy = MIN_2(MIN_2(pts[1], pts[3]), pts[5]);
        tmaxy = MAX_2(MAX_2(pts[1], pts[3]), pts[5]);
        /* a track cell, where the edges use a 16th of the track */
        triCellH = size[1] * CELL_WALL_UNITS / size[1];
        triCellW = size[0] * CELL_WALL_UNITS / size[0];
        for (gy = tminy / triCellH; gy <= tmaxy / triCellH; gy++) {
            for (gx = tminx / triCellW; gx <= tmaxx / triCellW; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    triLists[c * 65 + ++triLists[c * 65]] = elem;
                }
            }
        }
    }
    edgeBytes = 0;
    triBytes = 0;
    for (c = 0; c < 256; c++) {
        edgeBytes += edgeLists[c * 129] * 2;
        triBytes += triLists[c * 65] * 2;
    }
    gUserTrackData.collision->cells[0].edges = mem_alloc_dbg(edgeBytes, 0, sStrUsrTrkFile, 0x6A8);
    gUserTrackData.collision->cells[0].tris = mem_alloc_dbg(triBytes, 0, sStrUsrTrkFile, 0x6A9);
    for (c = 0; c < 257; c++) {
        if (c > 0) {
            gUserTrackData.collision->cells[c].tris = gUserTrackData.collision->cells[c - 1].tris + triLists[(c - 1) * 65];
            gUserTrackData.collision->cells[c].edges = gUserTrackData.collision->cells[c - 1].edges + edgeLists[(c - 1) * 129];
        }
        if (c < 256) {
            if (triLists[c * 65] != 0) {
                memcpy(gUserTrackData.collision->cells[c].tris, triLists + c * 65 + 1, triLists[c * 65] * 2);
            }
            if (edgeLists[c * 129] != 0) {
                memcpy(gUserTrackData.collision->cells[c].edges, edgeLists + c * 129 + 1, edgeLists[c * 129] * 2);
            }
        }
    }
    mem_free(triLists);
    mem_free(edgeLists);
    height_init_walls(&gUserTrackData.collision, 0);
    vtxA = gUserTrackData.vtxA;
    meshA = gUserTrackData.meshesA;
    gridOut = gUserTrackData.grids;
    vtxB = gUserTrackData.vtxB;
    meshB = gUserTrackData.meshesB;
    cellA = gUserTrackData.meshA->cells;
    cellB = gUserTrackData.meshB->cells;
    /* the racing line: every 5th point of each piece's path (backwards on a flipped piece), turned into place */
    *gUserTrackData.path = 0xAA000003; /* PATH_RES_MAGIC, PATH_RES_WIDTHS | PATH_RES_NODE_FLAGS */
    gUserTrackData.path = (u32 *)((s32 *)gUserTrackData.path + 1);
    PATH_LIST->count = 0;
    rec = PATH_LIST->nodes;
    pathPos = 0;
    for (i = 0, piece = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; piece++, i++) {
        if (TYPE.flags & PIECE_USER) {
            n = TYPE.dim[1] * 8;
        } else {
            n = *TYPE.path - 1;
        }
        for (; pathPos < n; pathPos += 5) {
            s32 x, y, offset;

            /* edgeBytes reused: the point's record */
            if (piece->flags & EDIT_PIECE_FLIPPED) {
                edgeBytes = n - pathPos;
            } else {
                edgeBytes = pathPos;
            }
            offset = edgeBytes * 44 + 4; /* records of sizeof(UsrTrkPathRec) after the count */
            __builtin_memcpy(rec, (u8 *)TYPE.path + offset, sizeof(UsrTrkPathRec));
            switch (piece->rot) {
            case 0:
                x = rec->node.x + piece->x * CELL_MM - halfW * 16;
                y = rec->node.z + piece->y * CELL_MM - halfH * 16;
                break;
            case 1:
                x = rec->node.z + piece->x * CELL_MM - halfW * 16;
                y = TYPE.dim[0] * CELL_MM - rec->node.x + piece->y * CELL_MM - halfH * 16;
                break;
            case 2:
                x = TYPE.dim[0] * CELL_MM - rec->node.x + piece->x * CELL_MM - halfW * 16;
                y = TYPE.dim[1] * CELL_MM - rec->node.z + piece->y * CELL_MM - halfH * 16;
                break;
            case 3:
                x = TYPE.dim[1] * CELL_MM - rec->node.z + piece->x * CELL_MM - halfW * 16;
                y = rec->node.x + piece->y * CELL_MM - halfH * 16;
                break;
            }
            rec->node.x = x;
            rec->node.z = y;
            rec->node.prev = PATH_LIST->count - 1;
            rec->node.next = ++PATH_LIST->count;
            rec->node.join = -1;
            rec->node.branch = -1;
            rec++;
        }
        pathPos -= n;
    }
    PATH_LIST->nodes[0].node.prev = PATH_LIST->count - 1;
    PATH_LIST->nodes[PATH_LIST->count - 1].node.next = 0;
    gUserTrackData.path = (u32 *)((s32 *)gUserTrackData.path - 1);
    /* the mesh sets: each grid cell's A and B meshes with their vertices turned into place */
    c = 0;
    for (gy = 0; gy < size[1]; gy++) {
        for (gx = 0; gx < size[0]; gx++) {
            cellA->mesh = meshA;
            cellA->x0 = gx * CELL_VTX_UNITS - halfW;
            cellA->y0 = gy * CELL_VTX_UNITS - halfH;
            cellA->x1 = gx * CELL_VTX_UNITS - halfW + CELL_VTX_UNITS;
            cellA->y1 = gy * CELL_VTX_UNITS - halfH + CELL_VTX_UNITS;
            cellA->grid = gridOut;
            cellA->mask = 0;
            meshA->vtxCount = grid[c].cellA->mesh->vtxCount;
            meshA->triCount = grid[c].cellA->mesh->triCount;
            meshA->vtx = vtxA;
            meshA->tris = grid[c].cellA->mesh->tris;
            meshA->unk_10 = 0;
            meshA->unk_14 = 0;
            src = grid[c].cellA->mesh->vtx;
            for (v = 0; v < grid[c].cellA->mesh->vtxCount; v++, src++) {
                vtxA->v.ob[0] = ROT.xOff + (cellA->x0 + (src->v.ob[ROT.xSrc] + 250) * ROT.xSign);
                vtxA->v.ob[1] = src->v.ob[1];
                vtxA->v.ob[2] = ROT.zOff + (cellA->y0 + (src->v.ob[ROT.zSrc] + 250) * ROT.zSign);
                vtxA->v.flag = src->v.flag;
                vtxA->v.tc[0] = src->v.tc[0];
                vtxA->v.tc[1] = src->v.tc[1];
                vtxA->v.cn[0] = src->v.cn[ROT.xSrc] * ROT.xSign;
                vtxA->v.cn[1] = src->v.cn[1];
                vtxA->v.cn[2] = src->v.cn[ROT.zSrc] * ROT.zSign;
                vtxA->v.cn[3] = 0xFF;
                vtxA++;
            }
            /* the ground triangle grid, its cell bits turned with the piece; flag 4 cleared: the vertices are no
               longer relative to the cell's centre */
            if (grid[c].cellA->grid[0] & 1) {
                *gridOut = grid[c].cellA->grid[0] & 0xFFFB;
                gridOut++;
                *gridOut = grid[c].cellA->grid[1];
                gridOut++;
                for (tri = 0; tri < grid[c].cellA->grid[1]; tri++) {
                    rows = (u8 *)grid[c].cellA->grid + (tri * 16 + 4);
                    out = (u8 *)gridOut;
                    switch (grid[c].rot) {
                    case 0:
                        out[0] = rows[0];
                        out[1] = rows[1];
                        out[2] = rows[2];
                        out[3] = rows[3];
                        out[4] = rows[4];
                        out[5] = rows[5];
                        out[6] = rows[6];
                        out[7] = rows[7];
                        COPY8;
                        break;
                    case 1:
                        out[0] = REVERSE_BITS(T80);
                        out[1] = REVERSE_BITS(TS(0x40, 6));
                        out[2] = REVERSE_BITS(TS(0x20, 5));
                        out[3] = REVERSE_BITS(TS(0x10, 4));
                        out[4] = REVERSE_BITS(TS(8, 3));
                        out[5] = REVERSE_BITS(TS(4, 2));
                        out[6] = REVERSE_BITS(TS(2, 1));
                        out[7] = REVERSE_BITS((((rows[0] & 1) << 7) | ((rows[1] & 1) * 64) | ((rows[2] & 1) * 32) | ((rows[3] & 1) * 16) | ((rows[4] & 1) * 8) | ((rows[5] & 1) * 4) | ((rows[6] & 1) * 2) | (rows[7] & 1)));
                        COPY8;
                        break;
                    case 2:
                        out[0] = REVERSE_BITS(rows[7]);
                        out[1] = REVERSE_BITS(rows[6]);
                        out[2] = REVERSE_BITS(rows[5]);
                        out[3] = REVERSE_BITS(rows[4]);
                        out[4] = REVERSE_BITS(rows[3]);
                        out[5] = REVERSE_BITS(rows[2]);
                        out[6] = REVERSE_BITS(rows[1]);
                        out[7] = REVERSE_BITS(rows[0]);
                        COPY8;
                        break;
                    case 3:
                        {
                            s32 r = T01;

                            out[0] = r;
                        }
                        {
                            s32 r = TM(2, 1);

                            out[1] = r;
                        }
                        {
                            s32 r = TM(4, 2);

                            out[2] = r;
                        }
                        {
                            s32 r = TM(8, 3);

                            out[3] = r;
                        }
                        {
                            s32 r = TM(0x10, 4);

                            out[4] = r;
                        }
                        {
                            s32 r = TM(0x20, 5);

                            out[5] = r;
                        }
                        {
                            s32 r = TM(0x40, 6);

                            out[6] = r;
                        }
                        {
                            s32 r = T80;

                            out[7] = r;
                        }
                        COPY8;
                        break;
                    }
                    gridOut += 8;
                }
            }
            cellA++;
            meshA++;
            if (grid[c].cellB != NULL) {
                cellB->mesh = meshB;
                cellB->x0 = gx * CELL_VTX_UNITS - halfW;
                cellB->y0 = gy * CELL_VTX_UNITS - halfH;
                cellB->x1 = gx * CELL_VTX_UNITS - halfW + CELL_VTX_UNITS;
                cellB->y1 = gy * CELL_VTX_UNITS - halfH + CELL_VTX_UNITS;
                cellB->grid = NULL;
                cellB->mask = 0;
                meshB->vtxCount = grid[c].cellB->mesh->vtxCount;
                meshB->triCount = grid[c].cellB->mesh->triCount;
                meshB->vtx = vtxB;
                meshB->tris = grid[c].cellB->mesh->tris;
                meshB->unk_10 = 0;
                meshB->unk_14 = 0;
                src = grid[c].cellB->mesh->vtx;
                for (v = 0; v < grid[c].cellB->mesh->vtxCount; v++, src++) {
                    vtxB->v.ob[0] = ROT.xOff + (cellB->x0 + (src->v.ob[ROT.xSrc] + 250) * ROT.xSign);
                    vtxB->v.ob[1] = src->v.ob[1];
                    vtxB->v.ob[2] = ROT.zOff + (cellB->y0 + (src->v.ob[ROT.zSrc] + 250) * ROT.zSign);
                    vtxB->v.ob[0] += grid[c].ox;
                    vtxB->v.ob[2] += grid[c].oy;
                    vtxB->v.flag = src->v.flag;
                    vtxB->v.tc[0] = src->v.tc[0];
                    vtxB->v.tc[1] = src->v.tc[1];
                    vtxB->v.cn[0] = 0xFF;
                    vtxB->v.cn[1] = 0xFF;
                    vtxB->v.cn[2] = 0xFF;
                    vtxB->v.cn[3] = 0xFF;
                    vtxB++;
                }
            } else {
                cellB->mesh = meshB;
                cellB->x0 = gx * CELL_VTX_UNITS - halfW;
                cellB->y0 = gy * CELL_VTX_UNITS - halfH;
                cellB->x1 = gx * CELL_VTX_UNITS - halfW + CELL_VTX_UNITS;
                cellB->y1 = gy * CELL_VTX_UNITS - halfH + CELL_VTX_UNITS;
                cellB->grid = NULL;
                cellB->mask = 0;
                meshB->vtxCount = 0;
                meshB->triCount = 0;
                meshB->vtx = NULL;
                meshB->tris = sEmptyCellTris;
                meshB->unk_10 = 0;
                meshB->unk_14 = 0;
            }
            cellB++;
            meshB++;
            c++;
        }
    }
    mem_free(grid);
    /* pos is never set: the original copies an uninitialised vector here */
    gCurTrack_42->v68.x = pos.x;
    gCurTrack_42->v68.y = pos.y;
    gCurTrack_42->v68.z = pos.z;
}

/* The size of the editor's track packed (usrtrk_pack_track). */
s16 usrtrk_get_packed_size(void)
{
    s32 count = 0;
    EditPiece *piece = gEditor->first;
    s32 total;
    s32 i;

    if (piece != NULL) {
        do {
            piece = piece->next;
            count++;
        } while (piece != NULL && piece != gEditor->first);
    }
    total = (s16)count * 3 + 8;
    for (i = 0; i < gEditor->userPieceCount; i++) { total += gPieceTypes[i + PIECE_TYPE_USER].dim[1] * 4; }
    return total + 10;
}
