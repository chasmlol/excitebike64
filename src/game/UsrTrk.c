#include "game.h"

/* The track name without its terminator, copied as one block */
typedef struct { u8 b[10]; } Blk10;

/* Packs the editor's track for a save (at most 0x310 bytes): 0x35, the dirt type, the name, the piece count, the
   start piece, the user piece count, each user piece's segment count and heights, then 3 bytes per piece from the
   first: x (5 bits), y (5), flags (6), rot (2), type (6). Returns the size. */
s32 usrtrk_pack_track(u8 *buf)
{
    u8 *p = buf;
    EditPiece *n = gEditor->first;
    s32 cnt, curi, k, j, tot;
    u16 e;
    u8 *d;
    s16 c;
    u8 *q;

    if (n != NULL) {
        cnt = 0;
        do {
            if (n == gEditor->start) {
                curi = cnt;
            }
            n = n->next;
            tot = cnt + 1;
            cnt = tot;
        } while (n != NULL && n != gEditor->first);

        p[0] = 0x35;
        p[1] = gEditor->dirtType;
        p += 2;
        *(Blk10 *)p = *(Blk10 *)gEditor->name;
        p += 10;
        *(u16 *)p = tot;
        p += 2;
        *(u16 *)p = curi;
        p += 2;
        *(u16 *)p = gEditor->userPieceCount;
        p += 2;
        for (k = 0; k < gEditor->userPieceCount; k++) {
            c = gPieceTypes[k + 18].dim[1];
            *(u16 *)p = c;
            d = p + 2;
            for (j = 0; j < c * 2 - 1; j++) {
                ((u16 *)d)[j] = gUserPieceHeights[k][j];
            }
            p += c * 4;
        }
        n = gEditor->first;
        do {
            e = ((u32)n->type - (u32)gPieceTypes) >> 6;
            p[0] = ((n->x & 0x1F) << 3) | ((n->y & 0x1F) >> 2);
            p[1] = ((n->y & 0x1F) << 6) | (n->flags & 0x3F);
            p[2] = ((n->rot & 3) << 6) | (e & 0x3F);
            p += 3;
            n = n->next;
        } while (n != NULL && n != gEditor->first);
    }
    return (s16)(p - buf);
}

void usrtrk_load_layout(u8 *p)
{
    s32 id;
    u16 n;
    u16 start;
    s32 ntrk;
    s16 i;
    s32 k;
    s32 j;
    UserTrackLayout *o;
    UserTrackPiece *r;

    id = p[1];
    p += 0xC;
    n = *((u16 *)p)++;
    start = *((u16 *)p)++;
    ntrk = *((s16 *)p)++;
    if (gUserTrackLayout != NULL) {
        mem_free(gUserTrackLayout);
    }
    gUserTrackLayout = mem_alloc_dbg(((s16)n - 1) * sizeof(UserTrackPiece) + sizeof(UserTrackLayout), 0, (s32)sStrUsrTrkFile, 0xFD);
    gUserTrackLayout->dirtType = id;
    gUserTrackLayout->userPieceCount = ntrk;
    for (i = 0; i < ntrk; i++) {
        gUserTrackLayout->segmentCounts[i] = p[1];
        memcpy_2((u8 *)(i * 30 + (u32)gUserTrackLayout) + 0x14 /* heights[i] */, p + 2, gUserTrackLayout->segmentCounts[i] * 4 - 2);
        p += gUserTrackLayout->segmentCounts[i] * 4;
    }
    gUserTrackLayout->pieceCount = (s16)n;
    for (k = 0; k < (s16)n; k++) {
        j = (k - (s16)start + (s16)n) % (s16)n;
        gUserTrackLayout->pieces[j].x = p[0] >> 3;
        gUserTrackLayout->pieces[j].y = (p[1] >> 6) | ((p[0] & 7) << 2);
        gUserTrackLayout->pieces[j].flags = p[1] & 0x3F;
        gUserTrackLayout->pieces[j].rot = p[2] >> 6;
        gUserTrackLayout->pieces[j].type = p[2] & 0x3F;
        p += 3;
    }
}

void usrtrk_load_into_editor(u8 *p)
{
    s16 i;
    s16 n;
    u16 w;
    s16 cnt, sel;
    s32 j;
    PieceType *rec;
    u16 *tp;

    {
        Editor *g;
        u8 v = p[1];
        p += 2;
        g = gEditor;
        g->dirtType = v;
        *(Blk10 *)g->name = *(Blk10 *)p;
    }
    p += 10;
    gEditor->name[10] = 0;
    cnt = *(s16 *)p;
    p += 2;
    sel = *(s16 *)p;
    p += 2;
    gEditor->userPieceCount = *(s16 *)p;
    p += 2;
    for (i = 0; i < gEditor->userPieceCount; i++) {
        n = *(s16 *)p;
        w = *(u16 *)p;
        tp = gUserPieceHeights[i];
        memcpy(tp, p + 2, (n * 2 - 1) * 2);
        usrtrk_apply_heights(gPieceTypes[i + 0x12].meshA, gPieceTypes[i + 0x12].meshB, tp, n);
        gPieceTypes[i + 0x12].dim[1] = w;
        p = (u8 *)((s16 *)p + n * 2);
    }

    gEditor->first = gEditor->cur = gEditor->pieces;
    gEditor->last = &gEditor->pieces[cnt - 1];
    gEditor->start = &gEditor->pieces[sel];
    for (j = 0; j < cnt; j++) {
        gEditor->pieces[j].used = 1;
        if (j == 0) {
            gEditor->pieces[j].prev = 0;
        } else {
            gEditor->pieces[j].prev = &gEditor->pieces[j - 1];
        }
        if (j == cnt - 1) {
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
    for (j = cnt; j < 150; j++) {
        gEditor->pieces[j].used = 0;
    }
}

void usrtrk_layout_from_editor(void)
{
    EditPiece *n;
    Editor *g;
    UserTrackPiece *r;
    UserTrackLayout *o;
    s16 count;
    s32 cnt;
    s32 f;
    s32 k;
    s16 i;
    s32 t;
    u16 d;

    count = 0;
    n = gEditor->first;
    do {
        n = n->next;
        count++;
    } while (n != NULL && n != gEditor->first);
    if (gUserTrackLayout != NULL) {
        mem_free(gUserTrackLayout);
    }
    cnt = (s16)count;
    o = mem_alloc_dbg((cnt - 1) * 12 + 0x110, 0, sStrUsrTrkFile, 0x1AE);
    f = gEditor->dirtType;
    gUserTrackLayout = o;
    o->pieceCount = cnt;
    o->dirtType = f;
    k = gEditor->userPieceCount;
    i = 0;
    o->userPieceCount = k;
    if (k > 0) {
        do {
            gUserTrackLayout->segmentCounts[i] = gPieceTypes[i + 0x12].dim[1];
            memcpy((u8 *)(i * 30 + (s32)gUserTrackLayout) + 0x14 /* heights[i] */, gUserPieceHeights[i], gUserTrackLayout->segmentCounts[i] * 4 - 2);
            i++;
        } while (i < gUserTrackLayout->userPieceCount);
    }
    n = gEditor->start;
    k = 0;
    do {
        t = (s32)n->type;
        gUserTrackLayout->pieces[k].x = n->x;
        gUserTrackLayout->pieces[k].y = n->y;
        gUserTrackLayout->pieces[k].flags = n->flags;
        d = n->rot;
        gUserTrackLayout->pieces[k].type = (t - (s32)gPieceTypes) >> 6;
        gUserTrackLayout->pieces[k].rot = d;
        n = n->next;
        k++;
        if (n == NULL) {
            n = gEditor->first;
            if (n == NULL) {
                return;
            }
        }
    } while (n != gEditor->start);
}

void usrtrk_load_pieces(void)
{
    s32 i;
    u32 j, k;
    s32 c;
    s32 n;

    res_register_type(sStrUsrTrkArn, (s32)D_800248EC);
    res_register_type(sStrUsrTrkWal, (s32)D_80031D08);
    res_open_set(sStrUsrTrkPieces, &sPiecesResSet);
    gEditArena.collision = (struct CollisionMesh *)res_load(sPiecesResSet, *gEditArena.collisionId);
    height_free_walls();
    mem_fill((u8 *)&gUserTrackData, 0, sizeof(UserTrackData));

    for (i = 0; i < 29; i++) {
        gPieceTypes[i].path = 0;
        gPieceTypes[i].collision = 0;
        if (!(gPieceTypes[i].flags & 0x40)) {
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
                PieceMesh *m = gPieceTypes[i].meshA->cells[j].mesh;
                for (k = 0; k < m->vtxCount; k++) {
                    s32 l;
                    for (l = 0; l < 3; l += 2) {
                        s32 v = (s16)m->vtx[k].v.ob[l];
                        if (v < 0) {
                            c = -v;
                            v = c / 25;
                            v += (c % 25 >= 13);
                            v *= 25;
                            v = -v;
                        } else {
                            v = v / 25 + (v % 25 >= 13);
                            v *= 25;
                        }
                        m->vtx[k].v.ob[l] = v;
                    }
                }
                j++;
            } while (j < gPieceTypes[i].meshA->count);
        }
        if (gPieceTypes[i].pathId != 0) {
            gPieceTypes[i].path = (s32 *)(res_load(sPiecesResSet, *gPieceTypes[i].pathId) + 4);
        }
        if (gPieceTypes[i].collisionId != 0) {
            gPieceTypes[i].collision = (struct CollisionMesh *)res_load(sPiecesResSet, *gPieceTypes[i].collisionId);
            height_free_walls();
        }
        }
    }

    for (i = 18, n = 0; i < 30; i++, n++) {
        if (!(gPieceTypes[i].flags & 0x40)) continue;
        gPieceTypes[i].meshA = mem_alloc_dbg(gUserTrackLayout->segmentCounts[n] << 6 | 4, 0, sStrUsrTrkFile, 0x27F);
        gPieceTypes[i].meshB = mem_alloc_dbg(gUserTrackLayout->segmentCounts[n] << 6 | 4, 0, sStrUsrTrkFile, 0x280);
        gPieceTypes[i].dim[1] = gUserTrackLayout->segmentCounts[n];
        if (gPieceTypes[i].pathId != 0) {
            gPieceTypes[i].path = (s32 *)(res_load(sPiecesResSet, *gPieceTypes[i].pathId) + 4);
        }
        if (gPieceTypes[i].collisionId != 0) {
            gPieceTypes[i].collision = (struct CollisionMesh *)res_load(sPiecesResSet, *gPieceTypes[i].collisionId);
            height_free_walls();
        }
        usrtrk_clone_piece_meshes(gPieceTypes[i].meshA, gPieceTypes[i].meshB, gUserTrackLayout->segmentCounts[n]);
        usrtrk_apply_heights(gPieceTypes[i].meshA, gPieceTypes[i].meshB, gUserTrackLayout->heights[n], gUserTrackLayout->segmentCounts[n]);
        gPieceTypes[i].dim[1] = gUserTrackLayout->segmentCounts[n];
    }
}

#define E(o, idx, kk) (*(o))->vtx[idx].v.ob[1] = ((kk) > 0 && (kk) < n * 2) ? h[(kk) - 1] : 0

/* Sets the heights of a user piece's meshes (n segments, 2 cells each) from h, the 2n - 1 heights between them,
   on the template piece's vertices. */
void usrtrk_apply_heights(PieceMeshSet *dst, PieceMeshSet *src, s16 *h, s32 n)
{
    PieceMeshSet *tmpl = gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;
    s32 i, j, k;
    s32 idx, c, a, b;
    u32 m;
    Vtx *v;
    PieceMesh **d, **s, **t;

    k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 2; j++) {
            s = &src->cells[i * 2 + j].mesh;
            t = &tmpl->cells[j].mesh;
            d = &dst->cells[i * 2 + j].mesh;
            for (m = 0; m < (*s)->vtxCount; m++) {
                {
                    s32 x = i * 2 + ((*s)->vtx[m].v.ob[2] + 250) / 250;
                    if (x > 0 && x < n * 2) a = h[x - 1]; else a = 0;
                }
                {
                    s32 x = i * 2 + ((*s)->vtx[m].v.ob[2] + 250) / 250;
                    s32 y = x + 1;
                    if (y > 0 && y < n * 2) b = h[x]; else b = 0;
                }
                c = ((*s)->vtx[m].v.ob[2] + 250) % 250;
                (*s)->vtx[m].v.ob[1] = (*t)->vtx[m].v.ob[1] + a + (b - a) * c / 250;
            }
            if (j == 0) {
                E(d, 4, k);
                E(d, 10, k);
                E(d, 11, k);
                E(d, 1, k + 1);
                E(d, 6, k + 1);
                E(d, 7, k + 1);
                E(d, 3, k + 2);
                E(d, 8, k + 2);
                E(d, 9, k + 2);
            } else {
                E(d, 4, k);
                E(d, 5, k);
                E(d, 11, k);
                E(d, 1, k + 1);
                E(d, 6, k + 1);
                E(d, 0, k + 1);
                E(d, 3, k + 2);
                E(d, 8, k + 2);
                E(d, 2, k + 2);
            }
        }
        k += 2;
    }
}

void usrtrk_clone_piece_meshes(PieceMeshSet *out1, PieceMeshSet *out2, s32 count)
{
    PieceMeshSet *set1;
    PieceMeshSet *set2;
    s32 j;
    u32 i;
    u32 n;
    s32 off;
    PieceCell *o;
    PieceCell *o2;
    s32 size;
    s32 size2;

    set1 = gPieceTypes[PIECE_TYPE_TEMPLATE].meshA;
    set2 = gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;
    for (j = 0; j < count; j++) {
        for (i = 0; i < (n = set1->count); i++) {
            size = (set1->cells[i].tex[1] << 4) | 4;
            o = &out1->cells[j * n + i];
            o2 = &out2->cells[j * set2->count + i];
            o->mesh = mem_alloc_dbg(sizeof(PieceMesh) + set1->cells[i].mesh->vtxCount * 16 + set1->cells[i].mesh->triCount * 4 + size, 0, (s32)sStrUsrTrkFile, 0x2F8);
            *o->mesh = *set1->cells[i].mesh;
            o->unk_18 = set1->cells[i].unk_18;
            o->unk_1C = set1->cells[i].unk_1C;
            o->x0 = set1->cells[i].x0;
            o->y0 = set1->cells[i].y0;
            o->x1 = set1->cells[i].x1;
            o->y1 = set1->cells[i].y1;
            o->mesh->vtxCount = set1->cells[i].mesh->vtxCount;
            o->mesh->vtx = (Vtx *)(o->mesh + 1);
            o->mesh->tris = (u32 *)(o->mesh->vtx + o->mesh->vtxCount);
            o->mesh->triCount = set1->cells[i].mesh->triCount;
            o->tex = (s16 *)(o->mesh->tris + o->mesh->triCount);
            memcpy(o->mesh->vtx, set1->cells[i].mesh->vtx, o->mesh->vtxCount * 16);
            memcpy(o->mesh->tris, set1->cells[i].mesh->tris, o->mesh->triCount * 4);
            memcpy(o->tex, set1->cells[i].tex, size);

            size2 = (set2->cells[i].tex[1] << 4) | 4;
            o2->mesh = mem_alloc_dbg(sizeof(PieceMesh) + set2->cells[i].mesh->vtxCount * 16 + set2->cells[i].mesh->triCount * 4 + size2, 0, (s32)sStrUsrTrkFile, 0x30C);
            *o2->mesh = *set2->cells[i].mesh;
            o2->unk_18 = set2->cells[i].unk_18;
            o2->unk_1C = set2->cells[i].unk_1C;
            o2->x0 = set2->cells[i].x0;
            o2->y0 = set2->cells[i].y0 + j * 500;
            o2->x1 = set2->cells[i].x1;
            o2->y1 = set2->cells[i].y1 + j * 500;
            o2->mesh->vtxCount = set2->cells[i].mesh->vtxCount;
            o2->mesh->vtx = (Vtx *)(o2->mesh + 1);
            o2->mesh->tris = (u32 *)(o2->mesh->vtx + o2->mesh->vtxCount);
            o2->mesh->triCount = set2->cells[i].mesh->triCount;
            o2->tex = (s16 *)(o2->mesh->tris + o2->mesh->triCount);
            memcpy(o2->mesh->vtx, set2->cells[i].mesh->vtx, o2->mesh->vtxCount * 16);
            memcpy(o2->mesh->tris, set2->cells[i].mesh->tris, o2->mesh->triCount * 4);
            memcpy(o2->tex, set2->cells[i].tex, size2);
        }
    }
}

void usrtrk_unload(void)
{
    res_close_set(sPiecesResSet);
    sPiecesResSet = 0;
    if (gUserTrackData.meshA != 0) {
        mem_free(gUserTrackData.meshA);
        mem_free(gUserTrackData.meshB);
        mem_free(gUserTrackData.tex);
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

static inline void freeOne(PieceType *q, s32 k)
{
    PieceMesh **a;
    PieceMesh **b;

    a = &q->meshA->cells[k].mesh;
    b = &q->meshB->cells[k].mesh;
    mem_free(*a);
    mem_free(*b);
}

void usrtrk_free_user_piece_meshes(s32 param_1)
{
    s32 i;
    s32 j;
    s32 n;

    for (n = 18; n < 30; n++) {
        if (gPieceTypes[n].flags & 0x40) {
            for (i = 0; param_1 == 0 ? i < gPieceTypes[n].dim[1] : i < 8; i++) {
                for (j = 0; j < 2; j++) {
                    freeOne(&gPieceTypes[n], i * 2 + j);
                }
            }
            mem_free(gPieceTypes[n].meshA);
            mem_free(gPieceTypes[n].meshB);
        }
    }
}

#define PC (gUserTrackLayout->pieces[i])
#define EN (gPieceTypes[gUserTrackLayout->pieces[i].type])
#define EP (gPieceTypes[p->type])
#define R (sPieceRotations[grid[c].rot])
#define G (grid[gy * size[0] + gx])
#define W (EP.dim[0])
#define H (EP.dim[1])
#define K0 (row * W + col)
#define K1 (col * W + W - 1 - row)
#define K2 ((H - 1 - row) * W + W - 1 - col)
#define K3 ((H - 1 - col) * W + row)
#define MIN_2(a, b) ((a) < (b) ? (a) : (b))
#define MAX_2(a, b) ((a) > (b) ? (a) : (b))

#define REV(x) ((((x) >> 7) & 1) | (((x) >> 5) & 2) | (((x) >> 3) & 4) | (((x) >> 1) & 8) |                 (((x) << 1) & 0x10) | (((x) << 3) & 0x20) | (((x) << 5) & 0x40) | (((x) << 7) & 0x80))
/* transpose terms: output bit (7 - i) from source bit k */

#define BL(i, k) ((ts[i] << (7 - (i) - (k))) & (0x80 >> (i)))
#define BR(i, k) ((ts[i] >> ((k) - 7 + (i))) & (0x80 >> (i)))
#define T80 ((ts[0] & 0x80) | ((ts[1] & 0x80) >> 1) | ((ts[2] & 0x80) >> 2) | ((ts[3] & 0x80) >> 3) |              ((ts[4] & 0x80) >> 4) | ((ts[5] & 0x80) >> 5) | ((ts[6] & 0x80) >> 6) | ((ts[7] & 0x80) >> 7))
#define T01 (((ts[0] & 1) << 7) | ((ts[1] & 1) << 6) | ((ts[2] & 1) << 5) | ((ts[3] & 1) << 4) |              ((ts[4] & 1) << 3) | ((ts[5] & 1) << 2) | ((ts[6] & 1) << 1) | (ts[7] & 1))
#define BX(i, m, k) ((7 - (i)) > (k) ? BL(i, k) : (7 - (i)) == (k) ? (ts[i] & (m)) : BR(i, k))
#define TM(m, k) (((ts[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) |                   BX(5, m, k) | BX(6, m, k) | ((ts[7] & (m)) >> (k)))
#define TS(m, k) (((ts[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) | BX(5, m, k) | BX(6, m, k) | ({ s32 z; if (ts[7] & (m)) z = 1; else z = 0; z; }))
#define COPY8 td[8] = ts[8]; td[9] = ts[9]; td[10] = ts[10]; td[11] = ts[11];               td[12] = ts[12]; td[13] = ts[13]; td[14] = ts[14]; td[15] = ts[15]

/* A grid cell of the track being built: the piece cell drawn there. */
typedef struct {
    PieceType *type;
    PieceCell *cellA;
    PieceCell *cellB;
    s16 rot, col, row, ox, oy;
    s16 pad;
} UsrTrkGridCell;

/* A node of the path being built (header 0xAA000003: a PathResNode and the flag 2 byte). */
typedef struct {
    PathResNode node;
    u8 unk_28;
    u8 pad29[3];
} UsrTrkPathRec; /* size 0x2C */

typedef struct {
    s16 ix, iy, ox, oy, sx, sy;
} Rot;

/* gUserTrackData.path while the nodes are added (after the header word). */
typedef struct UsrTrkPathList {
    s32 count;
    UsrTrkPathRec nodes[1];
} UsrTrkPathList;

typedef struct {
    u8 pad[0x68];
    Vec3f v68;
} Obj_79;

extern Rot sPieceRotations[];
extern Obj_79 *gCurTrack_42;

#define PATH_LIST ((UsrTrkPathList *)gUserTrackData.path)

void usrtrk_build_track(void)
{
    Vtx *va;
    PieceMesh *ma;
    PieceCell *ea;
    Vtx *vb;
    PieceMesh *mb;
    PieceCell *eb;
    s16 *g68;
    s16 *g6C;
    s32 nva;
    s32 nvb;
    s32 nv;
    s32 ne;
    s32 nt;
    s32 ti;
    s32 ncells;
    s32 bmw;
    s32 ebase;
    s32 tbase;
    s32 reps;
    s16 lo[2];
    s16 hi[2];
    s16 size[2];
    Vec3f pos;
    s32 pt[6];
    UsrTrkGridCell *grid;
    s32 i, j, k, m;
    s32 gx, gy, col, row;
    s32 cnt, texsz;
    s32 vbase;
    s32 minx, maxx, miny, maxy;
    s32 tminx, tmaxx, tminy, tmaxy;
    s32 tcw, tch;
    s32 cw, ch;
    s32 c, n;
    s32 dx, dy;
    s32 sz1, sz2;
    PieceType *e;
    UserTrackPiece *p;
    s32 v, q, b, l;
    CollisionVtx *dv;
    CollisionVtx *sv;
    Vtx *src;
    UsrTrkPathRec *rec;
    PieceCell *bb;
    PieceType *d;
    s32 rc;
    s32 idx;
    s16 *tex;
    u8 *ts;
    u8 *td;
    s32 w, h;
    s32 kk;
    s32 kr;

    texsz = 0;
    nva = 0;
    nvb = 0;
    size[0] = -32000;
    size[1] = -32000;
    for (i = 0; i < gUserTrackLayout->pieceCount; i++) {
        if (size[0] < PC.x + EN.dim[PC.rot & 1]) {
            size[0] = PC.x + EN.dim[PC.rot & 1];
        }
        if (size[1] < PC.y + EN.dim[(PC.rot + 1) & 1]) {
            size[1] = PC.y + EN.dim[(PC.rot + 1) & 1];
        }
    }
    size[0] = (size[0] < 15) ? 15 : size[0];
    size[1] = (size[1] < 15) ? 15 : size[1];
    cnt = 0;
    ncells = size[0] * size[1];
    grid = mem_alloc_dbg(ncells * sizeof(UsrTrkGridCell), 0, sStrUsrTrkFile, 0x416);
    mem_fill(grid, 0, ncells * sizeof(UsrTrkGridCell));
    ne = gEditArena.collision->edgeCount;
    nv = gEditArena.collision->vtxCount;
    nt = gEditArena.collision->triCount;
    for (i = 0, p = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; p++, i++) {
        if (EP.flags & 0x40) {
            cnt += EP.dim[1] * 8;
        } else {
            cnt += *EP.path - 1;
        }
        if (EP.collision != NULL) {
            if (EP.flags & 0x40) {
                m = EP.dim[1] * 2;
                nv += EP.collision->vtxCount * m;
                nt += EP.collision->triCount * m;
                ne += EP.collision->edgeCount * m;
            } else {
                nv += EP.collision->vtxCount;
                nt += EP.collision->triCount;
                ne += EP.collision->edgeCount;
            }
        }
        lo[0] = p->x;
        lo[1] = p->y;
        hi[0] = lo[0] + EP.dim[p->rot & 1];
        hi[1] = lo[1] + EP.dim[(p->rot + 1) & 1];
        for (gy = lo[1], row = 0; gy < hi[1]; gy++, row++) {
            for (gx = lo[0], col = 0; gx < hi[0]; gx++, col++) {
                G.rot = p->rot;
                G.type = &EP;
                G.col = col;
                G.row = row;
                switch (p->rot) {
                case 0:
                    G.cellA = &EP.meshA->cells[K0];
                    G.cellB = EP.meshB ? &EP.meshB->cells[K0] : NULL;
                    break;
                case 1:
                    G.cellA = &EP.meshA->cells[K1];
                    G.cellB = EP.meshB ? &EP.meshB->cells[K1] : NULL;
                    break;
                case 2:
                    G.cellA = &EP.meshA->cells[K2];
                    G.cellB = EP.meshB ? &EP.meshB->cells[K2] : NULL;
                    break;
                case 3:
                    G.cellA = &EP.meshA->cells[K3];
                    G.cellB = EP.meshB ? &EP.meshB->cells[K3] : NULL;
                    break;
                }
                bb = G.cellB;
                if (bb != NULL) {
                    d = &EP;
                    switch (p->rot) {
                    case 0:
                        G.ox = (bb->x0 + bb->x1) / 2 - 250 - col * 500;
                        G.oy = (bb->y0 + bb->y1) / 2 - 250 - row * 500;
                        break;
                    case 1:
                        G.ox = (bb->y0 + bb->y1) / 2 - 250 - col * 500;
                        G.oy = (d->dim[0] - row - 1) * 500 + (250 - (bb->x0 + bb->x1) / 2);
                        break;
                    case 2:
                        G.ox = (d->dim[0] - col - 1) * 500 + (250 - (bb->x0 + bb->x1) / 2);
                        G.oy = (d->dim[1] - row - 1) * 500 + (250 - (bb->y0 + bb->y1) / 2);
                        break;
                    case 3:
                        G.ox = (d->dim[1] - col - 1) * 500 + (250 - (bb->y0 + bb->y1) / 2);
                        G.oy = (bb->x0 + bb->x1) / 2 - 250 - row * 500;
                        break;
                    }
                }
            }
        }
    }
    for (c = 0; c < ncells; c++) {
        if (grid[c].cellA == NULL) {
            grid[c].rot = 0;
            grid[c].type = &gPieceTypes[27];
            grid[c].cellA = &gPieceTypes[27].meshA->cells[0];
            grid[c].col = 0;
            grid[c].row = 0;
            if (gPieceTypes[27].meshB != NULL) {
                grid[c].cellB = &gPieceTypes[27].meshB->cells[0];
            } else {
                grid[c].cellB = NULL;
            }
        }
        nva += grid[c].cellA->mesh->vtxCount * sizeof(Vtx);
        if (grid[c].cellB != NULL) {
            nvb += grid[c].cellB->mesh->vtxCount * sizeof(Vtx);
        }
        if (grid[c].cellA->tex[0] & 1) {
            texsz += (grid[c].cellA->tex[1] * 16) | 4;
        } else {
            texsz += 0x54;
        }
    }
    switch (gUserTrackLayout->dirtType) {
    case 0:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, D_800D1348);
        break;
    case 1:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, D_800D134C);
        break;
    case 2:
        gUserTrackData.dirtTex = res_load(sPiecesResSet, D_800D1350);
        break;
    }
    gUserTrackData.objectTex = res_load(sPiecesResSet, D_800D1370);
    gUserTrackData.meshA = mem_alloc_dbg(ncells * sizeof(PieceCell) | 4, 0, sStrUsrTrkFile, 0x4F5);
    gUserTrackData.meshA->count = ncells;
    gUserTrackData.meshB = mem_alloc_dbg(ncells * sizeof(PieceCell) | 4, 0, sStrUsrTrkFile, 0x4F8);
    gUserTrackData.meshB->count = ncells;
    gUserTrackData.path = mem_alloc_dbg((cnt / 5 + 1) * sizeof(UsrTrkPathRec) + 8, 0, sStrUsrTrkFile, 0x4FB);
    gUserTrackData.collision = mem_alloc_dbg(sizeof(CollisionMesh), 0, sStrUsrTrkFile, 0x4FC);
    gUserTrackData.collision->vtx = mem_alloc_dbg(nv * sizeof(CollisionVtx), 0, sStrUsrTrkFile, 0x4FD);
    gUserTrackData.collision->edges = mem_alloc_dbg(ne * sizeof(CollisionEdge), 0, sStrUsrTrkFile, 0x4FE);
    gUserTrackData.collision->tris = mem_alloc_dbg(nt * sizeof(CollisionTri), 0, sStrUsrTrkFile, 0x4FF);
    g68 = mem_alloc_dbg(0x8200, 0, sStrUsrTrkFile, 0x500);
    g6C = mem_alloc_dbg(0x10200, 0, sStrUsrTrkFile, 0x501);
    w = size[0] * 250;
    h = size[1] * 250;
    gUserTrackData.tex = mem_alloc_dbg(texsz, 0, sStrUsrTrkFile, 0x508);
    gUserTrackData.vtxA = mem_alloc_dbg(nva, 0, sStrUsrTrkFile, 0x509);
    gUserTrackData.meshesA = mem_alloc_dbg(ncells * sizeof(PieceMesh), 0, sStrUsrTrkFile, 0x50A);
    gUserTrackData.vtxB = mem_alloc_dbg(nvb, 0, sStrUsrTrkFile, 0x50C);
    gUserTrackData.meshesB = mem_alloc_dbg(ncells * sizeof(PieceMesh), 0, sStrUsrTrkFile, 0x50D);
    ebase = gEditArena.collision->edgeCount;
    vbase = gEditArena.collision->vtxCount;
    tbase = gEditArena.collision->triCount;
    gUserTrackData.collision->vtxCount = nv;
    gUserTrackData.collision->triCount = nt;
    gUserTrackData.collision->edgeCount = ne;
    gUserTrackData.collision->xMax = -1000000;
    gUserTrackData.collision->zMax = -1000000;
    gUserTrackData.collision->xMin = 1000000;
    gUserTrackData.collision->zMin = 1000000;
    for (j = 0; j < gEditArena.collision->vtxCount; j++) {
        gUserTrackData.collision->vtx[j].x = gEditArena.collision->vtx[j].x;
        gUserTrackData.collision->vtx[j].z = gEditArena.collision->vtx[j].z;
        gUserTrackData.collision->vtx[j].unk_04 = gEditArena.collision->vtx[j].unk_04;
        gUserTrackData.collision->vtx[j].y = gEditArena.collision->vtx[j].y;
    }
    for (j = 0; j < gEditArena.collision->edgeCount; j++) {
        gUserTrackData.collision->edges[j].v0 = gEditArena.collision->edges[j].v0;
        gUserTrackData.collision->edges[j].v1 = gEditArena.collision->edges[j].v1;
        gUserTrackData.collision->edges[j].nx = -0x8000;
        gUserTrackData.collision->edges[j].nz = 0x40;
    }
    for (j = 0; j < gEditArena.collision->triCount; j++) {
        gUserTrackData.collision->tris[j].v0 = gEditArena.collision->tris[j].v0;
        gUserTrackData.collision->tris[j].v1 = gEditArena.collision->tris[j].v1;
        gUserTrackData.collision->tris[j].v2 = gEditArena.collision->tris[j].v2;
        gUserTrackData.collision->tris[j].nx = gEditArena.collision->tris[j].nx;
        gUserTrackData.collision->tris[j].ny = gEditArena.collision->tris[j].ny;
        gUserTrackData.collision->tris[j].nz = gEditArena.collision->tris[j].nz;
    }
    bmw = (nv + 15) / 16;
    mem_fill(gZBuffer_2 + bmw, 0xFF, nv * 2);
    for (i = 0, p = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; p++, i++) {
        e = &gPieceTypes[p->type];
        if (e->collision != NULL) {
            gUserTrackData.collision->xMin = -3750;
            gUserTrackData.collision->xMax = 3750;
            gUserTrackData.collision->zMin = -3750;
            gUserTrackData.collision->zMax = 3750;
            if (e->flags & 0x40) {
                reps = e->dim[1] * 2;
            } else {
                reps = 1;
            }
            for (k = 0; k < reps; k++) {
                for (j = 0; j < e->collision->vtxCount; j++) {
                    gZBuffer_2[bmw + vbase + j] = i;
                    gUserTrackData.collision->vtx[vbase + j].unk_04 = e->collision->vtx[j].unk_04;
                    gUserTrackData.collision->vtx[vbase + j].y = e->collision->vtx[j].y;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData.collision->vtx[vbase + j].x = e->collision->vtx[j].x;
                        gUserTrackData.collision->vtx[vbase + j].z = e->collision->vtx[j].z;
                        if (e->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k];
                        }
                        gUserTrackData.collision->vtx[vbase + j].x += p->x * 125 - w / 4;
                        gUserTrackData.collision->vtx[vbase + j].z += p->y * 125 - h / 4 + (k * 4000) / 64;
                        break;
                    case 1:
                        gUserTrackData.collision->vtx[vbase + j].x = e->collision->vtx[j].z;
                        gUserTrackData.collision->vtx[vbase + j].z = e->dim[0] * 125 - e->collision->vtx[j].x;
                        if (e->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k];
                        }
                        gUserTrackData.collision->vtx[vbase + j].x += p->x * 125 - w / 4 + (k * 4000) / 64;
                        gUserTrackData.collision->vtx[vbase + j].z += p->y * 125 - h / 4;
                        break;
                    case 2:
                        gUserTrackData.collision->vtx[vbase + j].x = e->dim[0] * 125 - e->collision->vtx[j].x;
                        gUserTrackData.collision->vtx[vbase + j].z = e->dim[1] * 125 - e->collision->vtx[j].z;
                        if (e->collision->vtx[j].z == 0) {
                            if (k != 0) {
                                gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][k];
                        }
                        gUserTrackData.collision->vtx[vbase + j].x += p->x * 125 - w / 4;
                        gUserTrackData.collision->vtx[vbase + j].z += p->y * 125 - h / 4 - (k * 4000) / 64;
                        break;
                    case 3:
                        gUserTrackData.collision->vtx[vbase + j].x = e->dim[1] * 125 - e->collision->vtx[j].z;
                        gUserTrackData.collision->vtx[vbase + j].z = e->collision->vtx[j].x;
                        if (e->collision->vtx[j].z == 0) {
                            kk = reps - k - 1;
                            if (kk != 0) {
                                gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][reps - k - 2];
                            }
                        } else if ((kr = reps - k - 1) != reps - 1) {
                            gUserTrackData.collision->vtx[vbase + j].y += gUserTrackLayout->heights[e - &gPieceTypes[18]][kr];
                        }
                        gUserTrackData.collision->vtx[vbase + j].x += p->x * 125 - w / 4 - ((reps - k - 1) * 4000) / 64;
                        gUserTrackData.collision->vtx[vbase + j].z += p->y * 125 - h / 4;
                        break;
                    }
                }
                for (j = 0; j < e->collision->edgeCount; j++) {
                    gUserTrackData.collision->edges[ebase + j].v0 = e->collision->edges[j].v0 + vbase;
                    gUserTrackData.collision->edges[ebase + j].v1 = e->collision->edges[j].v1 + vbase;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData.collision->edges[ebase + j].nx = e->collision->edges[j].nx;
                        gUserTrackData.collision->edges[ebase + j].nz = e->collision->edges[j].nz;
                        break;
                    case 1:
                        gUserTrackData.collision->edges[ebase + j].nx = e->collision->edges[j].nz;
                        gUserTrackData.collision->edges[ebase + j].nz = -e->collision->edges[j].nx;
                        break;
                    case 2:
                        gUserTrackData.collision->edges[ebase + j].nx = -e->collision->edges[j].nx;
                        gUserTrackData.collision->edges[ebase + j].nz = -e->collision->edges[j].nz;
                        break;
                    case 3:
                        gUserTrackData.collision->edges[ebase + j].nx = -e->collision->edges[j].nz;
                        gUserTrackData.collision->edges[ebase + j].nz = e->collision->edges[j].nx;
                        break;
                    }
                    gUserTrackData.collision->edges[ebase + j].nx &= ~1;
                    gUserTrackData.collision->edges[ebase + j].nz &= ~1;
                }
                for (j = 0; j < e->collision->triCount; j++) {
                    gUserTrackData.collision->tris[tbase + j].v0 = e->collision->tris[j].v0 + vbase;
                    gUserTrackData.collision->tris[tbase + j].v1 = e->collision->tris[j].v1 + vbase;
                    gUserTrackData.collision->tris[tbase + j].v2 = e->collision->tris[j].v2 + vbase;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData.collision->tris[tbase + j].nx = e->collision->tris[j].nx;
                        gUserTrackData.collision->tris[tbase + j].nz = e->collision->tris[j].nz;
                        break;
                    case 1:
                        gUserTrackData.collision->tris[tbase + j].nx = e->collision->tris[j].nz;
                        gUserTrackData.collision->tris[tbase + j].nz = -e->collision->tris[j].nx;
                        break;
                    case 2:
                        gUserTrackData.collision->tris[tbase + j].nx = -e->collision->tris[j].nx;
                        gUserTrackData.collision->tris[tbase + j].nz = -e->collision->tris[j].nz;
                        break;
                    case 3:
                        gUserTrackData.collision->tris[tbase + j].nx = -e->collision->tris[j].nz;
                        gUserTrackData.collision->tris[tbase + j].nz = e->collision->tris[j].nx;
                        break;
                    }
                    gUserTrackData.collision->tris[tbase + j].ny = e->collision->tris[j].ny;
                }
                ebase += e->collision->edgeCount;
                vbase += e->collision->vtxCount;
                tbase += e->collision->triCount;
            }
        }
    }
    mem_fill(gZBuffer_2, 0, bmw * 2);
    for (j = gEditArena.collision->vtxCount; j < nv; j++) {
        if (!((gZBuffer_2[j / 16] >> (j % 16)) & 1)) {
            for (l = j + 1; l < nv; l++) {
                if (!((gZBuffer_2[l / 16] >> (l % 16)) & 1)) {
                    dx = gUserTrackData.collision->vtx[j].x - gUserTrackData.collision->vtx[l].x;
                    dy = gUserTrackData.collision->vtx[j].z - gUserTrackData.collision->vtx[l].z;
                    if (dx * dx + dy * dy < 16) {
                        gUserTrackData.collision->vtx[l].x += dx / 2;
                        gUserTrackData.collision->vtx[l].z += dy / 2;
                        gUserTrackData.collision->vtx[j].x -= dx - dx / 2;
                        gUserTrackData.collision->vtx[j].z -= dy - dy / 2;
                        gZBuffer_2[l / 16] |= 1 << (l % 16);
                        gZBuffer_2[j / 16] |= 1 << (j % 16);
                        break;
                    }
                }
            }
        }
    }
    for (c = 0; c < 256; c++) {
        g68[c * 65] = 0;
        g6C[c * 129] = 0;
    }
    for (b = 0; b < gUserTrackData.collision->edgeCount; b++) {
        pt[0] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[b].v0].x + w / 4;
        pt[1] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[b].v0].z + h / 4;
        pt[2] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[b].v1].x + w / 4;
        pt[3] = gUserTrackData.collision->vtx[gUserTrackData.collision->edges[b].v1].z + h / 4;
        minx = MIN_2(pt[0], pt[2]);
        maxx = MAX_2(pt[0], pt[2]);
        miny = MIN_2(pt[1], pt[3]);
        maxy = MAX_2(pt[1], pt[3]);
        cw = size[0] * 125 / 16;
        ch = size[1] * 125 / 16;
        for (gy = miny / ch; gy <= maxy / ch; gy++) {
            for (gx = minx / cw; gx <= maxx / cw; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    g6C[c * 129 + ++g6C[c * 129]] = b;
                }
            }
        }
    }
    for (b = 0; b < gUserTrackData.collision->triCount; b++) {
        pt[0] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v0].x + w / 4;
        pt[1] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v0].z + h / 4;
        pt[2] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v1].x + w / 4;
        pt[3] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v1].z + h / 4;
        pt[4] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v2].x + w / 4;
        pt[5] = gUserTrackData.collision->vtx[gUserTrackData.collision->tris[b].v2].z + h / 4;
        tminx = MIN_2(MIN_2(pt[0], pt[2]), pt[4]);
        tmaxx = MAX_2(MAX_2(pt[0], pt[2]), pt[4]);
        tminy = MIN_2(MIN_2(pt[1], pt[3]), pt[5]);
        tmaxy = MAX_2(MAX_2(pt[1], pt[3]), pt[5]);
        tch = size[1] * 125 / size[1];
        tcw = size[0] * 125 / size[0];
        for (gy = tminy / tch; gy <= tmaxy / tch; gy++) {
            for (gx = tminx / tcw; gx <= tmaxx / tcw; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    g68[c * 65 + ++g68[c * 65]] = b;
                }
            }
        }
    }
    sz2 = 0;
    sz1 = 0;
    for (c = 0; c < 256; c++) {
        sz2 += g6C[c * 129] * 2;
        sz1 += g68[c * 65] * 2;
    }
    gUserTrackData.collision->cells[0].edges = mem_alloc_dbg(sz2, 0, sStrUsrTrkFile, 0x6A8);
    gUserTrackData.collision->cells[0].tris = mem_alloc_dbg(sz1, 0, sStrUsrTrkFile, 0x6A9);
    for (c = 0; c < 257; c++) {
        if (c > 0) {
            gUserTrackData.collision->cells[c].tris = gUserTrackData.collision->cells[c - 1].tris + g68[(c - 1) * 65];
            gUserTrackData.collision->cells[c].edges = gUserTrackData.collision->cells[c - 1].edges + g6C[(c - 1) * 129];
        }
        if (c < 256) {
            if (g68[c * 65] != 0) {
                memcpy(gUserTrackData.collision->cells[c].tris, g68 + c * 65 + 1, g68[c * 65] * 2);
            }
            if (g6C[c * 129] != 0) {
                memcpy(gUserTrackData.collision->cells[c].edges, g6C + c * 129 + 1, g6C[c * 129] * 2);
            }
        }
    }
    mem_free(g68);
    mem_free(g6C);
    height_init_walls(&gUserTrackData.collision, 0);
    va = gUserTrackData.vtxA;
    ma = gUserTrackData.meshesA;
    tex = gUserTrackData.tex;
    vb = gUserTrackData.vtxB;
    mb = gUserTrackData.meshesB;
    ea = gUserTrackData.meshA->cells;
    eb = gUserTrackData.meshB->cells;
    *gUserTrackData.path = 0xAA000003;
    gUserTrackData.path = (u32 *)((s32 *)gUserTrackData.path + 1);
    PATH_LIST->count = 0;
    rec = PATH_LIST->nodes;
    rc = 0;
    for (i = 0, p = gUserTrackLayout->pieces; i < gUserTrackLayout->pieceCount; p++, i++) {
        if (EP.flags & 0x40) {
            n = EP.dim[1] * 8;
        } else {
            n = *EP.path - 1;
        }
        for (; rc < n; rc += 5) {
            s32 x, y, o;

            if (p->flags & 2) {
                sz2 = n - rc;
            } else {
                sz2 = rc;
            }
            o = sz2 * 44 + 4;
            __builtin_memcpy(rec, (u8 *)EP.path + o, sizeof(UsrTrkPathRec));
            switch (p->rot) {
            case 0:
                x = rec->node.x + p->x * 8000 - w * 16;
                y = rec->node.z + p->y * 8000 - h * 16;
                break;
            case 1:
                x = rec->node.z + p->x * 8000 - w * 16;
                y = EP.dim[0] * 8000 - rec->node.x + p->y * 8000 - h * 16;
                break;
            case 2:
                x = EP.dim[0] * 8000 - rec->node.x + p->x * 8000 - w * 16;
                y = EP.dim[1] * 8000 - rec->node.z + p->y * 8000 - h * 16;
                break;
            case 3:
                x = EP.dim[1] * 8000 - rec->node.z + p->x * 8000 - w * 16;
                y = rec->node.x + p->y * 8000 - h * 16;
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
        rc -= n;
    }
    PATH_LIST->nodes[0].node.prev = PATH_LIST->count - 1;
    PATH_LIST->nodes[PATH_LIST->count - 1].node.next = 0;
    gUserTrackData.path = (u32 *)((s32 *)gUserTrackData.path - 1);
    c = 0;
    for (gy = 0; gy < size[1]; gy++) {
        for (gx = 0; gx < size[0]; gx++) {
            ea->mesh = ma;
            ea->x0 = gx * 500 - w;
            ea->y0 = gy * 500 - h;
            ea->x1 = gx * 500 - w + 500;
            ea->y1 = gy * 500 - h + 500;
            ea->tex = tex;
            ea->unk_1C = 0;
            ma->vtxCount = grid[c].cellA->mesh->vtxCount;
            ma->triCount = grid[c].cellA->mesh->triCount;
            ma->vtx = va;
            ma->tris = grid[c].cellA->mesh->tris;
            ma->unk_10 = 0;
            ma->unk_14 = 0;
            src = grid[c].cellA->mesh->vtx;
            for (v = 0; v < grid[c].cellA->mesh->vtxCount; v++, src++) {
                va->v.ob[0] = R.ox + (ea->x0 + (src->v.ob[R.ix] + 250) * R.sx);
                va->v.ob[1] = src->v.ob[1];
                va->v.ob[2] = R.oy + (ea->y0 + (src->v.ob[R.iy] + 250) * R.sy);
                va->v.flag = src->v.flag;
                va->v.tc[0] = src->v.tc[0];
                va->v.tc[1] = src->v.tc[1];
                va->v.cn[0] = src->v.cn[R.ix] * R.sx;
                va->v.cn[1] = src->v.cn[1];
                va->v.cn[2] = src->v.cn[R.iy] * R.sy;
                va->v.cn[3] = 0xFF;
                va++;
            }
            if (grid[c].cellA->tex[0] & 1) {
                *tex = grid[c].cellA->tex[0] & 0xFFFB;
                tex++;
                *tex = grid[c].cellA->tex[1];
                tex++;
                for (ti = 0; ti < grid[c].cellA->tex[1]; ti++) {
                    ts = (u8 *)grid[c].cellA->tex + (ti * 16 + 4);
                    td = (u8 *)tex;
                    switch (grid[c].rot) {
                    case 0:
                        td[0] = ts[0];
                        td[1] = ts[1];
                        td[2] = ts[2];
                        td[3] = ts[3];
                        td[4] = ts[4];
                        td[5] = ts[5];
                        td[6] = ts[6];
                        td[7] = ts[7];
                        COPY8;
                        break;
                    case 1:
                        td[0] = REV(T80);
                        td[1] = REV(TS(0x40, 6));
                        td[2] = REV(TS(0x20, 5));
                        td[3] = REV(TS(0x10, 4));
                        td[4] = REV(TS(8, 3));
                        td[5] = REV(TS(4, 2));
                        td[6] = REV(TS(2, 1));
                        td[7] = REV((((ts[0] & 1) << 7) | ((ts[1] & 1) * 64) | ((ts[2] & 1) * 32) | ((ts[3] & 1) * 16) | ((ts[4] & 1) * 8) | ((ts[5] & 1) * 4) | ((ts[6] & 1) * 2) | (ts[7] & 1)));
                        COPY8;
                        break;
                    case 2:
                        td[0] = REV(ts[7]);
                        td[1] = REV(ts[6]);
                        td[2] = REV(ts[5]);
                        td[3] = REV(ts[4]);
                        td[4] = REV(ts[3]);
                        td[5] = REV(ts[2]);
                        td[6] = REV(ts[1]);
                        td[7] = REV(ts[0]);
                        COPY8;
                        break;
                    case 3:
                        {
                            s32 r = T01;

                            td[0] = r;
                        }
                        {
                            s32 r = TM(2, 1);

                            td[1] = r;
                        }
                        {
                            s32 r = TM(4, 2);

                            td[2] = r;
                        }
                        {
                            s32 r = TM(8, 3);

                            td[3] = r;
                        }
                        {
                            s32 r = TM(0x10, 4);

                            td[4] = r;
                        }
                        {
                            s32 r = TM(0x20, 5);

                            td[5] = r;
                        }
                        {
                            s32 r = TM(0x40, 6);

                            td[6] = r;
                        }
                        {
                            s32 r = T80;

                            td[7] = r;
                        }
                        COPY8;
                        break;
                    }
                    tex += 8;
                }
            }
            ea++;
            ma++;
            if (grid[c].cellB != NULL) {
                eb->mesh = mb;
                eb->x0 = gx * 500 - w;
                eb->y0 = gy * 500 - h;
                eb->x1 = gx * 500 - w + 500;
                eb->y1 = gy * 500 - h + 500;
                eb->tex = NULL;
                eb->unk_1C = 0;
                mb->vtxCount = grid[c].cellB->mesh->vtxCount;
                mb->triCount = grid[c].cellB->mesh->triCount;
                mb->vtx = vb;
                mb->tris = grid[c].cellB->mesh->tris;
                mb->unk_10 = 0;
                mb->unk_14 = 0;
                src = grid[c].cellB->mesh->vtx;
                for (v = 0; v < grid[c].cellB->mesh->vtxCount; v++, src++) {
                    vb->v.ob[0] = R.ox + (eb->x0 + (src->v.ob[R.ix] + 250) * R.sx);
                    vb->v.ob[1] = src->v.ob[1];
                    vb->v.ob[2] = R.oy + (eb->y0 + (src->v.ob[R.iy] + 250) * R.sy);
                    vb->v.ob[0] += grid[c].ox;
                    vb->v.ob[2] += grid[c].oy;
                    vb->v.flag = src->v.flag;
                    vb->v.tc[0] = src->v.tc[0];
                    vb->v.tc[1] = src->v.tc[1];
                    vb->v.cn[0] = 0xFF;
                    vb->v.cn[1] = 0xFF;
                    vb->v.cn[2] = 0xFF;
                    vb->v.cn[3] = 0xFF;
                    vb++;
                }
            } else {
                eb->mesh = mb;
                eb->x0 = gx * 500 - w;
                eb->y0 = gy * 500 - h;
                eb->x1 = gx * 500 - w + 500;
                eb->y1 = gy * 500 - h + 500;
                eb->tex = NULL;
                eb->unk_1C = 0;
                mb->vtxCount = 0;
                mb->triCount = 0;
                mb->vtx = NULL;
                mb->tris = D_800D06B4;
                mb->unk_10 = 0;
                mb->unk_14 = 0;
            }
            eb++;
            mb++;
            c++;
        }
    }
    mem_free(grid);
    gCurTrack_42->v68.x = pos.x;
    gCurTrack_42->v68.y = pos.y;
    gCurTrack_42->v68.z = pos.z;
}

s16 usrtrk_get_packed_size(void)
{
    s32 cnt = 0;
    EditPiece *n = gEditor->first;
    s32 total;
    s32 i;

    if (n != NULL) {
        do {
            n = n->next;
            cnt++;
        } while (n != NULL && n != gEditor->first);
    }
    total = (s16)cnt * 3 + 8;
    for (i = 0; i < gEditor->userPieceCount; i++) { total += gPieceTypes[i + 18].dim[1] * 4; }
    return total + 10;
}
