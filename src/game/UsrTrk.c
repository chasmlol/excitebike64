#include "game.h"

typedef struct { u8 b[10]; } Blk10;

typedef struct {
    u32 nv;
    Vtx *v;
    u32 ntri;
    u32 *tri;
    s32 x10;
    s32 x14;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 a, b, c, d;
    s16 *tex;
    s32 e, f;
} Entry_2;

struct Set {
    u32 n;
    Entry_2 e[1];
};

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

typedef struct Node_36 {
    s32 unk0;
    struct Node_36 *next;
    s32 unk8;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    u8 *ref;
} Node_36;

typedef struct {
    u8 pad[0x17C];
    Node_36 *listA;
    u8 pad180[0x190 - 0x180];
    Node_36 *listB;
    u8 pad194[0x16F4 - 0x194];
    s32 nItems;
    u8 pad16F8[0x17F8 - 0x16F8];
    s32 val;
} Game_44;

typedef struct {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s16 idx;
    u16 pad;
} OutRec;

typedef struct {
    s32 cnt;
    s32 n;
    s32 val;
    u8 len[8];
    u8 names[8][30];
    OutRec recs[1];
} Out_5;

typedef struct {
    u8 unk0[3];
    u8 len;
    u8 pad[60];
} Tbl_7;

extern Game_44 *gEditor_36;
extern Out_5 *gUserTrackLayout_4;
extern Tbl_7 gPieceTypes_16[];

void usrtrk_layout_from_editor(void)
{
    Node_36 *n;
    Game_44 *g;
    OutRec *r;
    Out_5 *o;
    s16 count;
    s32 cnt;
    s32 f;
    s32 k;
    s16 i;
    s32 t;
    u16 d;

    count = 0;
    n = gEditor_36->listA;
    do {
        n = n->next;
        count++;
    } while (n != NULL && n != gEditor_36->listA);
    if (gUserTrackLayout_4 != NULL) {
        mem_free(gUserTrackLayout_4);
    }
    cnt = (s16)count;
    o = mem_alloc_dbg((cnt - 1) * 12 + 0x110, 0, sStrUsrTrkFile, 0x1AE);
    f = gEditor_36->val;
    gUserTrackLayout_4 = o;
    o->cnt = cnt;
    o->val = f;
    k = gEditor_36->nItems;
    i = 0;
    o->n = k;
    if (k > 0) {
        do {
            gUserTrackLayout_4->len[i] = gPieceTypes_16[i + 0x12].len;
            memcpy((u8 *)(i * 30 + (s32)gUserTrackLayout_4) + 0x14, gUserPieceHeights[i], gUserTrackLayout_4->len[i] * 4 - 2);
            i++;
        } while (i < gUserTrackLayout_4->n);
    }
    n = gEditor_36->listB;
    k = 0;
    do {
        t = (s32)n->ref;
        gUserTrackLayout_4->recs[k].a = n->a;
        gUserTrackLayout_4->recs[k].b = n->b;
        gUserTrackLayout_4->recs[k].c = n->c;
        d = n->d;
        gUserTrackLayout_4->recs[k].idx = (t - (s32)gPieceTypes_16) >> 6;
        gUserTrackLayout_4->recs[k].d = d;
        n = n->next;
        k++;
        if (n == NULL) {
            n = gEditor_36->listA;
            if (n == NULL) {
                return;
            }
        }
    } while (n != gEditor_36->listB);
}

typedef struct {
    u8 pad00[0x02];
    s16 h02;
    u8 pad04[0x08];
    u16 flags;
    u8 pad0E[0x12];
    u32 *p20;
    u32 *p24;
    u32 *p28;
    u32 *p2C;
    Set *s30;
    Set *s34;
    s32 r38;
    s32 r3C;
} Ent_89;

typedef struct {
    u8 pad[8];
    u32 *p8;
    u8 pad2[4];
    s32 r10;
} Hdr_11;

typedef struct {
    u8 pad[0xC];
    u8 cnt[8];
    u8 rec[1][0x1E];
} Tab_10;

extern Hdr_11 gEditArena_2;
extern Ent_89 gPieceTypes_17[];
extern Tab_10 *gUserTrackLayout_5;

void usrtrk_load_pieces(void)
{
    s32 i;
    u32 j, k;
    s32 c;
    s32 n;

    res_register_type(sStrUsrTrkArn, (s32)D_800248EC);
    res_register_type(sStrUsrTrkWal, (s32)D_80031D08);
    res_open_set(sStrUsrTrkPieces, &sPiecesResSet);
    gEditArena_2.r10 = res_load(sPiecesResSet, *gEditArena_2.p8);
    height_free_walls();
    mem_fill((u8 *)&gUserTrackData, 0, sizeof(UserTrackData));

    for (i = 0; i < 29; i++) {
        gPieceTypes_17[i].r38 = 0;
        gPieceTypes_17[i].r3C = 0;
        if (!(gPieceTypes_17[i].flags & 0x40)) {
        gPieceTypes_17[i].s30 = 0;
        gPieceTypes_17[i].s34 = 0;
        if (gPieceTypes_17[i].p20 != 0) {
            gPieceTypes_17[i].s30 = (Set *)res_load(sPiecesResSet, *gPieceTypes_17[i].p20);
        }
        if (gPieceTypes_17[i].p24 != 0) {
            gPieceTypes_17[i].s34 = (Set *)res_load(sPiecesResSet, *gPieceTypes_17[i].p24);
        }
        if (gPieceTypes_17[i].p20 != 0 && (j = 0, j < gPieceTypes_17[i].s30->n)) {
            do {
                Mesh *m = gPieceTypes_17[i].s30->e[j].mesh;
                for (k = 0; k < m->nv; k++) {
                    s32 l;
                    for (l = 0; l < 3; l += 2) {
                        s32 v = (s16)m->v[k].v.ob[l];
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
                        m->v[k].v.ob[l] = v;
                    }
                }
                j++;
            } while (j < gPieceTypes_17[i].s30->n);
        }
        if (gPieceTypes_17[i].p28 != 0) {
            gPieceTypes_17[i].r38 = res_load(sPiecesResSet, *gPieceTypes_17[i].p28) + 4;
        }
        if (gPieceTypes_17[i].p2C != 0) {
            gPieceTypes_17[i].r3C = res_load(sPiecesResSet, *gPieceTypes_17[i].p2C);
            height_free_walls();
        }
        }
    }

    for (i = 18, n = 0; i < 30; i++, n++) {
        if (!(gPieceTypes_17[i].flags & 0x40)) continue;
        gPieceTypes_17[i].s30 = mem_alloc_dbg(gUserTrackLayout_5->cnt[n] << 6 | 4, 0, sStrUsrTrkFile, 0x27F);
        gPieceTypes_17[i].s34 = mem_alloc_dbg(gUserTrackLayout_5->cnt[n] << 6 | 4, 0, sStrUsrTrkFile, 0x280);
        gPieceTypes_17[i].h02 = gUserTrackLayout_5->cnt[n];
        if (gPieceTypes_17[i].p28 != 0) {
            gPieceTypes_17[i].r38 = res_load(sPiecesResSet, *gPieceTypes_17[i].p28) + 4;
        }
        if (gPieceTypes_17[i].p2C != 0) {
            gPieceTypes_17[i].r3C = res_load(sPiecesResSet, *gPieceTypes_17[i].p2C);
            height_free_walls();
        }
        usrtrk_clone_piece_meshes(gPieceTypes_17[i].s30, gPieceTypes_17[i].s34, gUserTrackLayout_5->cnt[n]);
        usrtrk_apply_heights(gPieceTypes_17[i].s30, gPieceTypes_17[i].s34, gUserTrackLayout_5->rec[n], gUserTrackLayout_5->cnt[n]);
        gPieceTypes_17[i].h02 = gUserTrackLayout_5->cnt[n];
    }
}

#define E(o, idx, kk) (*(o))->v[idx].v.ob[1] = ((kk) > 0 && (kk) < n * 2) ? h[(kk) - 1] : 0

struct Mesh_8 {
    u32 n;
    Vtx *v;
};

void usrtrk_apply_heights(Mesh_8 **dst, Mesh_8 **src, s16 *h, s32 n)
{
    Mesh_8 **tmpl = (Mesh_8 **)gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;
    s32 i, j, k;
    s32 idx, c, a, b;
    u32 m;
    Vtx *v;
    Mesh_8 **d, **s, **t;

    k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 2; j++) {
            s = &src[(i * 2 + j) * 8 + 1];
            t = &tmpl[j * 8 + 1];
            d = &dst[(i * 2 + j) * 8 + 1];
            for (m = 0; m < (*s)->n; m++) {
                {
                    s32 x = i * 2 + ((*s)->v[m].v.ob[2] + 250) / 250;
                    if (x > 0 && x < n * 2) a = h[x - 1]; else a = 0;
                }
                {
                    s32 x = i * 2 + ((*s)->v[m].v.ob[2] + 250) / 250;
                    s32 y = x + 1;
                    if (y > 0 && y < n * 2) b = h[x]; else b = 0;
                }
                c = ((*s)->v[m].v.ob[2] + 250) % 250;
                (*s)->v[m].v.ob[1] = (*t)->v[m].v.ob[1] + a + (b - a) * c / 250;
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

typedef struct {
    u8 pad[0x730];
    Set *s1;
    Set *s2;
} Glob_10;

extern Glob_10 gPieceTypes_18;

void usrtrk_clone_piece_meshes(Set *out1, Set *out2, s32 count)
{
    Set *set1;
    Set *set2;
    s32 j;
    u32 i;
    u32 n;
    s32 off;
    Entry_2 *o;
    Entry_2 *o2;
    s32 size;
    s32 size2;

    set1 = gPieceTypes_18.s1;
    set2 = gPieceTypes_18.s2;
    for (j = 0; j < count; j++) {
        for (i = 0; i < (n = set1->n); i++) {
            size = (set1->e[i].tex[1] << 4) | 4;
            o = &out1->e[j * n + i];
            o2 = &out2->e[j * set2->n + i];
            o->mesh = mem_alloc_dbg(sizeof(Mesh) + set1->e[i].mesh->nv * 16 + set1->e[i].mesh->ntri * 4 + size, 0, (s32)sStrUsrTrkFile, 0x2F8);
            *o->mesh = *set1->e[i].mesh;
            o->e = set1->e[i].e;
            o->f = set1->e[i].f;
            o->a = set1->e[i].a;
            o->b = set1->e[i].b;
            o->c = set1->e[i].c;
            o->d = set1->e[i].d;
            o->mesh->nv = set1->e[i].mesh->nv;
            o->mesh->v = (Vtx *)(o->mesh + 1);
            o->mesh->tri = (u32 *)(o->mesh->v + o->mesh->nv);
            o->mesh->ntri = set1->e[i].mesh->ntri;
            o->tex = (s16 *)(o->mesh->tri + o->mesh->ntri);
            memcpy(o->mesh->v, set1->e[i].mesh->v, o->mesh->nv * 16);
            memcpy(o->mesh->tri, set1->e[i].mesh->tri, o->mesh->ntri * 4);
            memcpy(o->tex, set1->e[i].tex, size);

            size2 = (set2->e[i].tex[1] << 4) | 4;
            o2->mesh = mem_alloc_dbg(sizeof(Mesh) + set2->e[i].mesh->nv * 16 + set2->e[i].mesh->ntri * 4 + size2, 0, (s32)sStrUsrTrkFile, 0x30C);
            *o2->mesh = *set2->e[i].mesh;
            o2->e = set2->e[i].e;
            o2->f = set2->e[i].f;
            o2->a = set2->e[i].a;
            o2->b = set2->e[i].b + j * 500;
            o2->c = set2->e[i].c;
            o2->d = set2->e[i].d + j * 500;
            o2->mesh->nv = set2->e[i].mesh->nv;
            o2->mesh->v = (Vtx *)(o2->mesh + 1);
            o2->mesh->tri = (u32 *)(o2->mesh->v + o2->mesh->nv);
            o2->mesh->ntri = set2->e[i].mesh->ntri;
            o2->tex = (s16 *)(o2->mesh->tri + o2->mesh->ntri);
            memcpy(o2->mesh->v, set2->e[i].mesh->v, o2->mesh->nv * 16);
            memcpy(o2->mesh->tri, set2->e[i].mesh->tri, o2->mesh->ntri * 4);
            memcpy(o2->tex, set2->e[i].tex, size2);
        }
    }
}

typedef struct Sub_13 {
    s32 pad0[5];
    s32 *f14;
    s32 pad18;
    s32 *f1c;
    s32 pad20;
    s32 *f24;
    s32 *f28;
    s32 *f2c;
} Sub_13;

typedef struct Blk_26 {
    s32 *f00;
    s32 pad04;
    s32 *f08;
    s32 pad0c;
    s32 *f10;
    s32 *f14;
    s32 *f18;
    s32 *f1c;
    s32 *f20;
    s32 pad24;
    Sub_13 *f28;
} Blk_26;

extern Blk_26 gUserTrackData_3;

void usrtrk_unload(void)
{
    res_close_set(sPiecesResSet);
    sPiecesResSet = 0;
    if (gUserTrackData_3.f00 != 0) {
        mem_free(gUserTrackData_3.f00);
        mem_free(gUserTrackData_3.f08);
        mem_free(gUserTrackData_3.f10);
        mem_free(gUserTrackData_3.f14);
        mem_free(gUserTrackData_3.f18);
        mem_free(gUserTrackData_3.f1c);
        mem_free(gUserTrackData_3.f20);
        if (gUserTrackData_3.f28->f14 != 0) {
            mem_free(gUserTrackData_3.f28->f14);
        }
        if (gUserTrackData_3.f28->f24 != 0) {
            mem_free(gUserTrackData_3.f28->f24);
        }
        if (gUserTrackData_3.f28->f1c != 0) {
            mem_free(gUserTrackData_3.f28->f1c);
        }
        if (gUserTrackData_3.f28->f28 != 0) {
            mem_free(gUserTrackData_3.f28->f28);
        }
        if (gUserTrackData_3.f28->f2c != 0) {
            mem_free(gUserTrackData_3.f28->f2c);
        }
        mem_free(gUserTrackData_3.f28);
    }
    usrtrk_free_user_piece_meshes(0);
    height_free_walls();
}

typedef struct {
    s16 f0;
    s16 count;
    u8 pad4[8];
    u16 flags;
    u8 padE[0x22];
    void **f30;
    void **f34;
    u8 pad38[8];
} Rec_56;

extern Rec_56 gPieceTypes_19[];

static inline void freeOne(Rec_56 *q, s32 k)
{
    void **a;
    void **b;

    a = &q->f30[k * 8 + 1];
    b = &q->f34[k * 8 + 1];
    mem_free(*a);
    mem_free(*b);
}

void usrtrk_free_user_piece_meshes(s32 param_1)
{
    s32 i;
    s32 j;
    s32 n;

    for (n = 18; n < 30; n++) {
        if (gPieceTypes_19[n].flags & 0x40) {
            for (i = 0; param_1 == 0 ? i < gPieceTypes_19[n].count : i < 8; i++) {
                for (j = 0; j < 2; j++) {
                    freeOne(&gPieceTypes_19[n], i * 2 + j);
                }
            }
            mem_free(gPieceTypes_19[n].f30);
            mem_free(gPieceTypes_19[n].f34);
        }
    }
}

#define PC (gUserTrackLayout_6->pc[i])
#define EN (gPieceTypes_20[gUserTrackLayout_6->pc[i].type])
#define EP (gPieceTypes_20[p->type])
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

typedef struct { s16 a, b, c, d; } V4_2;
typedef struct { s16 a, b, c, d, e, f; } V6;

typedef struct {
    s16 *a;
    s16 *b;
} GridPtr;

typedef struct Geo {
    s32 xmin, ymin, xmax, ymax;
    s32 nv; V4_2 *v;
    s32 ne; V4_2 *e;
    s32 nt; V6 *t;
    GridPtr cell[256];
} Geo;

typedef struct {
    u32 nv;
    Vtx *v;
    s32 f8;
    void *fC;
    s32 f10, f14;
} Mesh_9;

typedef struct {
    Mesh_9 *mesh;
    s32 x0, y0, x1, y1;
    s16 *tex;
    s32 f18, f1C;
} Entry_8;

typedef struct {
    u32 n;
    Entry_8 e[1];
} Set_2;

typedef struct {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    u8 padE[0x22];
    Set_2 *s30;
    Set_2 *s34;
    s32 *r38;
    Geo *p3C;
} Ent_90;

typedef struct {
    s16 x, y;
    u16 flags;
    s16 rot;
    s16 type;
    s16 pad;
} Piece;

typedef struct {
    s32 n;
    s32 f4;
    s32 mode;
    u8 padC[8];
    u16 rec[8][15];
    Piece pc[1];
} Tab_11;

typedef struct {
    Ent_90 *e;
    Entry_8 *a;
    Entry_8 *b;
    s16 rot, col, row, ox, oy;
    s16 pad;
} Cell_11;

typedef struct {
    s32 x, y;
    s32 f8;
    s16 next, fE, prev, f12;
    u8 pad[0x18];
} Rec_57;

typedef struct {
    s16 ix, iy, ox, oy, sx, sy;
} Rot;

typedef struct RecSet_2 {
    s32 n;
    Rec_57 r[1];
} RecSet_2;

typedef struct {
    Set_2 *f00;
    s32 f04;
    Set_2 *f08;
    s32 f0C;
    s16 *f10;
    Vtx *f14;
    Mesh_9 *f18;
    Vtx *f1C;
    Mesh_9 *f20;
    struct RecSet_2 *f24;
    Geo *f28;
} Blk_27;

typedef struct {
    u8 pad[0x10];
    Geo *f10;
} Hdr_12;

typedef struct {
    u8 pad[0x68];
    Vec3f v68;
} Obj_79;

extern Tab_11 *gUserTrackLayout_6;
extern Ent_90 gPieceTypes_20[];
extern Hdr_12 gEditArena_3;
extern Blk_27 gUserTrackData_4;
extern Rot sPieceRotations[];
extern Obj_79 *gCurTrack_42;

void usrtrk_build_track(void)
{
    Vtx *va;
    Mesh_9 *ma;
    Entry_8 *ea;
    Vtx *vb;
    Mesh_9 *mb;
    Entry_8 *eb;
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
    Cell_11 *grid;
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
    Ent_90 *e;
    Piece *p;
    s32 v, q, b, l;
    V4_2 *dv;
    V4_2 *sv;
    Vtx *src;
    Rec_57 *rec;
    Entry_8 *bb;
    Ent_90 *d;
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
    for (i = 0; i < gUserTrackLayout_6->n; i++) {
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
    grid = mem_alloc_dbg(ncells * sizeof(Cell_11), 0, sStrUsrTrkFile, 0x416);
    mem_fill(grid, 0, ncells * sizeof(Cell_11));
    ne = gEditArena_3.f10->ne;
    nv = gEditArena_3.f10->nv;
    nt = gEditArena_3.f10->nt;
    for (i = 0, p = gUserTrackLayout_6->pc; i < gUserTrackLayout_6->n; p++, i++) {
        if (EP.flags & 0x40) {
            cnt += EP.dim[1] * 8;
        } else {
            cnt += *EP.r38 - 1;
        }
        if (EP.p3C != NULL) {
            if (EP.flags & 0x40) {
                m = EP.dim[1] * 2;
                nv += EP.p3C->nv * m;
                nt += EP.p3C->nt * m;
                ne += EP.p3C->ne * m;
            } else {
                nv += EP.p3C->nv;
                nt += EP.p3C->nt;
                ne += EP.p3C->ne;
            }
        }
        lo[0] = p->x;
        lo[1] = p->y;
        hi[0] = lo[0] + EP.dim[p->rot & 1];
        hi[1] = lo[1] + EP.dim[(p->rot + 1) & 1];
        for (gy = lo[1], row = 0; gy < hi[1]; gy++, row++) {
            for (gx = lo[0], col = 0; gx < hi[0]; gx++, col++) {
                G.rot = p->rot;
                G.e = &EP;
                G.col = col;
                G.row = row;
                switch (p->rot) {
                case 0:
                    G.a = &EP.s30->e[K0];
                    G.b = EP.s34 ? &EP.s34->e[K0] : NULL;
                    break;
                case 1:
                    G.a = &EP.s30->e[K1];
                    G.b = EP.s34 ? &EP.s34->e[K1] : NULL;
                    break;
                case 2:
                    G.a = &EP.s30->e[K2];
                    G.b = EP.s34 ? &EP.s34->e[K2] : NULL;
                    break;
                case 3:
                    G.a = &EP.s30->e[K3];
                    G.b = EP.s34 ? &EP.s34->e[K3] : NULL;
                    break;
                }
                bb = G.b;
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
        if (grid[c].a == NULL) {
            grid[c].rot = 0;
            grid[c].e = &gPieceTypes_20[27];
            grid[c].a = &gPieceTypes_20[27].s30->e[0];
            grid[c].col = 0;
            grid[c].row = 0;
            if (gPieceTypes_20[27].s34 != NULL) {
                grid[c].b = &gPieceTypes_20[27].s34->e[0];
            } else {
                grid[c].b = NULL;
            }
        }
        nva += grid[c].a->mesh->nv * sizeof(Vtx);
        if (grid[c].b != NULL) {
            nvb += grid[c].b->mesh->nv * sizeof(Vtx);
        }
        if (grid[c].a->tex[0] & 1) {
            texsz += (grid[c].a->tex[1] * 16) | 4;
        } else {
            texsz += 0x54;
        }
    }
    switch (gUserTrackLayout_6->mode) {
    case 0:
        gUserTrackData_4.f04 = res_load(sPiecesResSet, D_800D1348);
        break;
    case 1:
        gUserTrackData_4.f04 = res_load(sPiecesResSet, D_800D134C);
        break;
    case 2:
        gUserTrackData_4.f04 = res_load(sPiecesResSet, D_800D1350);
        break;
    }
    gUserTrackData_4.f0C = res_load(sPiecesResSet, D_800D1370);
    gUserTrackData_4.f00 = mem_alloc_dbg(ncells * sizeof(Entry_8) | 4, 0, sStrUsrTrkFile, 0x4F5);
    gUserTrackData_4.f00->n = ncells;
    gUserTrackData_4.f08 = mem_alloc_dbg(ncells * sizeof(Entry_8) | 4, 0, sStrUsrTrkFile, 0x4F8);
    gUserTrackData_4.f08->n = ncells;
    gUserTrackData_4.f24 = mem_alloc_dbg((cnt / 5 + 1) * sizeof(Rec_57) + 8, 0, sStrUsrTrkFile, 0x4FB);
    gUserTrackData_4.f28 = mem_alloc_dbg(sizeof(Geo), 0, sStrUsrTrkFile, 0x4FC);
    gUserTrackData_4.f28->v = mem_alloc_dbg(nv * sizeof(V4_2), 0, sStrUsrTrkFile, 0x4FD);
    gUserTrackData_4.f28->e = mem_alloc_dbg(ne * sizeof(V4_2), 0, sStrUsrTrkFile, 0x4FE);
    gUserTrackData_4.f28->t = mem_alloc_dbg(nt * sizeof(V6), 0, sStrUsrTrkFile, 0x4FF);
    g68 = mem_alloc_dbg(0x8200, 0, sStrUsrTrkFile, 0x500);
    g6C = mem_alloc_dbg(0x10200, 0, sStrUsrTrkFile, 0x501);
    w = size[0] * 250;
    h = size[1] * 250;
    gUserTrackData_4.f10 = mem_alloc_dbg(texsz, 0, sStrUsrTrkFile, 0x508);
    gUserTrackData_4.f14 = mem_alloc_dbg(nva, 0, sStrUsrTrkFile, 0x509);
    gUserTrackData_4.f18 = mem_alloc_dbg(ncells * sizeof(Mesh_9), 0, sStrUsrTrkFile, 0x50A);
    gUserTrackData_4.f1C = mem_alloc_dbg(nvb, 0, sStrUsrTrkFile, 0x50C);
    gUserTrackData_4.f20 = mem_alloc_dbg(ncells * sizeof(Mesh_9), 0, sStrUsrTrkFile, 0x50D);
    ebase = gEditArena_3.f10->ne;
    vbase = gEditArena_3.f10->nv;
    tbase = gEditArena_3.f10->nt;
    gUserTrackData_4.f28->nv = nv;
    gUserTrackData_4.f28->nt = nt;
    gUserTrackData_4.f28->ne = ne;
    gUserTrackData_4.f28->xmax = -1000000;
    gUserTrackData_4.f28->ymax = -1000000;
    gUserTrackData_4.f28->xmin = 1000000;
    gUserTrackData_4.f28->ymin = 1000000;
    for (j = 0; j < gEditArena_3.f10->nv; j++) {
        gUserTrackData_4.f28->v[j].a = gEditArena_3.f10->v[j].a;
        gUserTrackData_4.f28->v[j].b = gEditArena_3.f10->v[j].b;
        gUserTrackData_4.f28->v[j].c = gEditArena_3.f10->v[j].c;
        gUserTrackData_4.f28->v[j].d = gEditArena_3.f10->v[j].d;
    }
    for (j = 0; j < gEditArena_3.f10->ne; j++) {
        gUserTrackData_4.f28->e[j].a = gEditArena_3.f10->e[j].a;
        gUserTrackData_4.f28->e[j].b = gEditArena_3.f10->e[j].b;
        gUserTrackData_4.f28->e[j].c = -0x8000;
        gUserTrackData_4.f28->e[j].d = 0x40;
    }
    for (j = 0; j < gEditArena_3.f10->nt; j++) {
        gUserTrackData_4.f28->t[j].a = gEditArena_3.f10->t[j].a;
        gUserTrackData_4.f28->t[j].b = gEditArena_3.f10->t[j].b;
        gUserTrackData_4.f28->t[j].c = gEditArena_3.f10->t[j].c;
        gUserTrackData_4.f28->t[j].d = gEditArena_3.f10->t[j].d;
        gUserTrackData_4.f28->t[j].e = gEditArena_3.f10->t[j].e;
        gUserTrackData_4.f28->t[j].f = gEditArena_3.f10->t[j].f;
    }
    bmw = (nv + 15) / 16;
    mem_fill(gZBuffer_2 + bmw, 0xFF, nv * 2);
    for (i = 0, p = gUserTrackLayout_6->pc; i < gUserTrackLayout_6->n; p++, i++) {
        e = &gPieceTypes_20[p->type];
        if (e->p3C != NULL) {
            gUserTrackData_4.f28->xmin = -3750;
            gUserTrackData_4.f28->xmax = 3750;
            gUserTrackData_4.f28->ymin = -3750;
            gUserTrackData_4.f28->ymax = 3750;
            if (e->flags & 0x40) {
                reps = e->dim[1] * 2;
            } else {
                reps = 1;
            }
            for (k = 0; k < reps; k++) {
                for (j = 0; j < e->p3C->nv; j++) {
                    gZBuffer_2[bmw + vbase + j] = i;
                    gUserTrackData_4.f28->v[vbase + j].c = e->p3C->v[j].c;
                    gUserTrackData_4.f28->v[vbase + j].d = e->p3C->v[j].d;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData_4.f28->v[vbase + j].a = e->p3C->v[j].a;
                        gUserTrackData_4.f28->v[vbase + j].b = e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k];
                        }
                        gUserTrackData_4.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        gUserTrackData_4.f28->v[vbase + j].b += p->y * 125 - h / 4 + (k * 4000) / 64;
                        break;
                    case 1:
                        gUserTrackData_4.f28->v[vbase + j].a = e->p3C->v[j].b;
                        gUserTrackData_4.f28->v[vbase + j].b = e->dim[0] * 125 - e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k];
                        }
                        gUserTrackData_4.f28->v[vbase + j].a += p->x * 125 - w / 4 + (k * 4000) / 64;
                        gUserTrackData_4.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    case 2:
                        gUserTrackData_4.f28->v[vbase + j].a = e->dim[0] * 125 - e->p3C->v[j].a;
                        gUserTrackData_4.f28->v[vbase + j].b = e->dim[1] * 125 - e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][k];
                        }
                        gUserTrackData_4.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        gUserTrackData_4.f28->v[vbase + j].b += p->y * 125 - h / 4 - (k * 4000) / 64;
                        break;
                    case 3:
                        gUserTrackData_4.f28->v[vbase + j].a = e->dim[1] * 125 - e->p3C->v[j].b;
                        gUserTrackData_4.f28->v[vbase + j].b = e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            kk = reps - k - 1;
                            if (kk != 0) {
                                gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][reps - k - 2];
                            }
                        } else if ((kr = reps - k - 1) != reps - 1) {
                            gUserTrackData_4.f28->v[vbase + j].d += gUserTrackLayout_6->rec[e - &gPieceTypes_20[18]][kr];
                        }
                        gUserTrackData_4.f28->v[vbase + j].a += p->x * 125 - w / 4 - ((reps - k - 1) * 4000) / 64;
                        gUserTrackData_4.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    }
                }
                for (j = 0; j < e->p3C->ne; j++) {
                    gUserTrackData_4.f28->e[ebase + j].a = e->p3C->e[j].a + vbase;
                    gUserTrackData_4.f28->e[ebase + j].b = e->p3C->e[j].b + vbase;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData_4.f28->e[ebase + j].c = e->p3C->e[j].c;
                        gUserTrackData_4.f28->e[ebase + j].d = e->p3C->e[j].d;
                        break;
                    case 1:
                        gUserTrackData_4.f28->e[ebase + j].c = e->p3C->e[j].d;
                        gUserTrackData_4.f28->e[ebase + j].d = -e->p3C->e[j].c;
                        break;
                    case 2:
                        gUserTrackData_4.f28->e[ebase + j].c = -e->p3C->e[j].c;
                        gUserTrackData_4.f28->e[ebase + j].d = -e->p3C->e[j].d;
                        break;
                    case 3:
                        gUserTrackData_4.f28->e[ebase + j].c = -e->p3C->e[j].d;
                        gUserTrackData_4.f28->e[ebase + j].d = e->p3C->e[j].c;
                        break;
                    }
                    gUserTrackData_4.f28->e[ebase + j].c &= ~1;
                    gUserTrackData_4.f28->e[ebase + j].d &= ~1;
                }
                for (j = 0; j < e->p3C->nt; j++) {
                    gUserTrackData_4.f28->t[tbase + j].a = e->p3C->t[j].a + vbase;
                    gUserTrackData_4.f28->t[tbase + j].b = e->p3C->t[j].b + vbase;
                    gUserTrackData_4.f28->t[tbase + j].c = e->p3C->t[j].c + vbase;
                    switch (p->rot) {
                    case 0:
                        gUserTrackData_4.f28->t[tbase + j].d = e->p3C->t[j].d;
                        gUserTrackData_4.f28->t[tbase + j].f = e->p3C->t[j].f;
                        break;
                    case 1:
                        gUserTrackData_4.f28->t[tbase + j].d = e->p3C->t[j].f;
                        gUserTrackData_4.f28->t[tbase + j].f = -e->p3C->t[j].d;
                        break;
                    case 2:
                        gUserTrackData_4.f28->t[tbase + j].d = -e->p3C->t[j].d;
                        gUserTrackData_4.f28->t[tbase + j].f = -e->p3C->t[j].f;
                        break;
                    case 3:
                        gUserTrackData_4.f28->t[tbase + j].d = -e->p3C->t[j].f;
                        gUserTrackData_4.f28->t[tbase + j].f = e->p3C->t[j].d;
                        break;
                    }
                    gUserTrackData_4.f28->t[tbase + j].e = e->p3C->t[j].e;
                }
                ebase += e->p3C->ne;
                vbase += e->p3C->nv;
                tbase += e->p3C->nt;
            }
        }
    }
    mem_fill(gZBuffer_2, 0, bmw * 2);
    for (j = gEditArena_3.f10->nv; j < nv; j++) {
        if (!((gZBuffer_2[j / 16] >> (j % 16)) & 1)) {
            for (l = j + 1; l < nv; l++) {
                if (!((gZBuffer_2[l / 16] >> (l % 16)) & 1)) {
                    dx = gUserTrackData_4.f28->v[j].a - gUserTrackData_4.f28->v[l].a;
                    dy = gUserTrackData_4.f28->v[j].b - gUserTrackData_4.f28->v[l].b;
                    if (dx * dx + dy * dy < 16) {
                        gUserTrackData_4.f28->v[l].a += dx / 2;
                        gUserTrackData_4.f28->v[l].b += dy / 2;
                        gUserTrackData_4.f28->v[j].a -= dx - dx / 2;
                        gUserTrackData_4.f28->v[j].b -= dy - dy / 2;
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
    for (b = 0; b < gUserTrackData_4.f28->ne; b++) {
        pt[0] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->e[b].a].a + w / 4;
        pt[1] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->e[b].a].b + h / 4;
        pt[2] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->e[b].b].a + w / 4;
        pt[3] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->e[b].b].b + h / 4;
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
    for (b = 0; b < gUserTrackData_4.f28->nt; b++) {
        pt[0] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].a].a + w / 4;
        pt[1] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].a].b + h / 4;
        pt[2] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].b].a + w / 4;
        pt[3] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].b].b + h / 4;
        pt[4] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].c].a + w / 4;
        pt[5] = gUserTrackData_4.f28->v[gUserTrackData_4.f28->t[b].c].b + h / 4;
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
    gUserTrackData_4.f28->cell[0].b = mem_alloc_dbg(sz2, 0, sStrUsrTrkFile, 0x6A8);
    gUserTrackData_4.f28->cell[0].a = mem_alloc_dbg(sz1, 0, sStrUsrTrkFile, 0x6A9);
    for (c = 0; c < 257; c++) {
        if (c > 0) {
            gUserTrackData_4.f28->cell[c].a = gUserTrackData_4.f28->cell[c - 1].a + g68[(c - 1) * 65];
            gUserTrackData_4.f28->cell[c].b = gUserTrackData_4.f28->cell[c - 1].b + g6C[(c - 1) * 129];
        }
        if (c < 256) {
            if (g68[c * 65] != 0) {
                memcpy(gUserTrackData_4.f28->cell[c].a, g68 + c * 65 + 1, g68[c * 65] * 2);
            }
            if (g6C[c * 129] != 0) {
                memcpy(gUserTrackData_4.f28->cell[c].b, g6C + c * 129 + 1, g6C[c * 129] * 2);
            }
        }
    }
    mem_free(g68);
    mem_free(g6C);
    height_init_walls(&gUserTrackData_4.f28, 0);
    va = gUserTrackData_4.f14;
    ma = gUserTrackData_4.f18;
    tex = gUserTrackData_4.f10;
    vb = gUserTrackData_4.f1C;
    mb = gUserTrackData_4.f20;
    ea = gUserTrackData_4.f00->e;
    eb = gUserTrackData_4.f08->e;
    *(s32 *)gUserTrackData_4.f24 = 0xAA000003;
    gUserTrackData_4.f24 = (RecSet_2 *)((s32 *)gUserTrackData_4.f24 + 1);
    gUserTrackData_4.f24->n = 0;
    rec = gUserTrackData_4.f24->r;
    rc = 0;
    for (i = 0, p = gUserTrackLayout_6->pc; i < gUserTrackLayout_6->n; p++, i++) {
        if (EP.flags & 0x40) {
            n = EP.dim[1] * 8;
        } else {
            n = *EP.r38 - 1;
        }
        for (; rc < n; rc += 5) {
            s32 x, y, o;

            if (p->flags & 2) {
                sz2 = n - rc;
            } else {
                sz2 = rc;
            }
            o = sz2 * 44 + 4;
            __builtin_memcpy(rec, (u8 *)EP.r38 + o, sizeof(Rec_57));
            switch (p->rot) {
            case 0:
                x = rec->x + p->x * 8000 - w * 16;
                y = rec->y + p->y * 8000 - h * 16;
                break;
            case 1:
                x = rec->y + p->x * 8000 - w * 16;
                y = EP.dim[0] * 8000 - rec->x + p->y * 8000 - h * 16;
                break;
            case 2:
                x = EP.dim[0] * 8000 - rec->x + p->x * 8000 - w * 16;
                y = EP.dim[1] * 8000 - rec->y + p->y * 8000 - h * 16;
                break;
            case 3:
                x = EP.dim[1] * 8000 - rec->y + p->x * 8000 - w * 16;
                y = rec->x + p->y * 8000 - h * 16;
                break;
            }
            rec->x = x;
            rec->y = y;
            rec->prev = gUserTrackData_4.f24->n - 1;
            rec->next = ++gUserTrackData_4.f24->n;
            rec->f12 = -1;
            rec->fE = -1;
            rec++;
        }
        rc -= n;
    }
    gUserTrackData_4.f24->r[0].prev = gUserTrackData_4.f24->n - 1;
    gUserTrackData_4.f24->r[gUserTrackData_4.f24->n - 1].next = 0;
    gUserTrackData_4.f24 = (RecSet_2 *)((s32 *)gUserTrackData_4.f24 - 1);
    c = 0;
    for (gy = 0; gy < size[1]; gy++) {
        for (gx = 0; gx < size[0]; gx++) {
            ea->mesh = ma;
            ea->x0 = gx * 500 - w;
            ea->y0 = gy * 500 - h;
            ea->x1 = gx * 500 - w + 500;
            ea->y1 = gy * 500 - h + 500;
            ea->tex = tex;
            ea->f1C = 0;
            ma->nv = grid[c].a->mesh->nv;
            ma->f8 = grid[c].a->mesh->f8;
            ma->v = va;
            ma->fC = grid[c].a->mesh->fC;
            ma->f10 = 0;
            ma->f14 = 0;
            src = grid[c].a->mesh->v;
            for (v = 0; v < grid[c].a->mesh->nv; v++, src++) {
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
            if (grid[c].a->tex[0] & 1) {
                *tex = grid[c].a->tex[0] & 0xFFFB;
                tex++;
                *tex = grid[c].a->tex[1];
                tex++;
                for (ti = 0; ti < grid[c].a->tex[1]; ti++) {
                    ts = (u8 *)grid[c].a->tex + (ti * 16 + 4);
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
            if (grid[c].b != NULL) {
                eb->mesh = mb;
                eb->x0 = gx * 500 - w;
                eb->y0 = gy * 500 - h;
                eb->x1 = gx * 500 - w + 500;
                eb->y1 = gy * 500 - h + 500;
                eb->tex = NULL;
                eb->f1C = 0;
                mb->nv = grid[c].b->mesh->nv;
                mb->f8 = grid[c].b->mesh->f8;
                mb->v = vb;
                mb->fC = grid[c].b->mesh->fC;
                mb->f10 = 0;
                mb->f14 = 0;
                src = grid[c].b->mesh->v;
                for (v = 0; v < grid[c].b->mesh->nv; v++, src++) {
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
                eb->f1C = 0;
                mb->nv = 0;
                mb->f8 = 0;
                mb->v = NULL;
                mb->fC = D_800D06B4;
                mb->f10 = 0;
                mb->f14 = 0;
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

typedef struct Node_37 { u8 pad[4]; struct Node_37 *next; } Node_37;
typedef struct { u8 pad[0x17C]; Node_37 *head; u8 pad2[0x16F4 - 0x180]; s32 count; } Game_45;

extern Game_45 *gEditor_37;

s16 usrtrk_get_packed_size(void)
{
    s32 cnt = 0;
    Node_37 *n = gEditor_37->head;
    s32 total;
    s32 i;

    if (n != NULL) {
        do {
            n = n->next;
            cnt++;
        } while (n != NULL && n != gEditor_37->head);
    }
    total = (s16)cnt * 3 + 8;
    for (i = 0; i < gEditor_37->count; i++) { total += gPieceTypes[i + 18].dim[1] * 4; }
    return total + 10;
}
