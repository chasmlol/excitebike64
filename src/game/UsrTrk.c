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

typedef struct Node_34 {
    s32 hdr;
    struct Node_34 *next;
    u8 p8[5];
    u8 f0D;
    u16 f0E;
    u8 p10;
    u8 f11;
    u8 p12;
    u8 f13;
    void *f14;
} Node_34;

typedef struct {
    u8 pad0[0x17C];
    Node_34 *list;
    u8 pad1[0x10];
    Node_34 *cur;
    u8 pad2[0x16BA - 0x194];
    Blk10 blk;
    u8 pad3[0x16F4 - 0x16C4];
    s32 n16F4;
    u8 pad4[0x17FB - 0x16F8];
    u8 b17FB;
} Gs_4;

typedef struct { u16 f0; s16 cnt; u8 pad[0x3C]; } Ent40_4;
typedef struct { u16 w[15]; } Row_6;

extern Gs_4 *D_800CEA40_34;
extern Ent40_4 D_800CE280_14[];
extern Row_6 D_800F55B8_3[];

s32 func_800A42D0(u8 *buf)
{
    u8 *p = buf;
    Node_34 *n = D_800CEA40_34->list;
    s32 cnt, curi, k, j, tot;
    u16 e;
    u8 *d;
    s16 c;
    u8 *q;

    if (n != NULL) {
        cnt = 0;
        do {
            if (n == D_800CEA40_34->cur) {
                curi = cnt;
            }
            n = n->next;
            tot = cnt + 1;
            cnt = tot;
        } while (n != NULL && n != D_800CEA40_34->list);

        p[0] = 0x35;
        p[1] = D_800CEA40_34->b17FB;
        p += 2;
        *(Blk10 *)p = D_800CEA40_34->blk;
        p += 10;
        *(u16 *)p = tot;
        p += 2;
        *(u16 *)p = curi;
        p += 2;
        *(u16 *)p = *(u16 *)((u8 *)D_800CEA40_34 + 0x16F6);
        p += 2;
        for (k = 0; k < D_800CEA40_34->n16F4; k++) {
            c = D_800CE280_14[k + 18].cnt;
            *(u16 *)p = c;
            d = p + 2;
            for (j = 0; j < c * 2 - 1; j++) {
                ((u16 *)d)[j] = D_800F55B8_3[k].w[j];
            }
            p += c * 4;
        }
        n = D_800CEA40_34->list;
        do {
            e = ((u32)n->f14 - (u32)D_800CE280_14) >> 6;
            p[0] = ((n->f0D & 0x1F) << 3) | ((n->f0E & 0x1F) >> 2);
            p[1] = ((((u8 *)n)[0xF] & 0x1F) << 6) | (n->f11 & 0x3F);
            p[2] = ((n->f13 & 3) << 6) | (e & 0x3F);
            p += 3;
            n = n->next;
        } while (n != NULL && n != D_800CEA40_34->list);
    }
    return (s16)(p - buf);
}

typedef struct { s16 a, b, c, d, e, pad; } Rec_54;

typedef struct {
    s32 n;
    s32 ntrk;
    s32 id;
    u8 len[8];
    u8 data[8][30];
    Rec_54 rec[1];
} Obj_77;

extern Obj_77 *D_800C9570_3;

void func_800A44EC(u8 *p)
{
    s32 id;
    u16 n;
    u16 start;
    s32 ntrk;
    s16 i;
    s32 k;
    s32 j;
    Obj_77 *o;
    Rec_54 *r;

    id = p[1];
    p += 0xC;
    n = *((u16 *)p)++;
    start = *((u16 *)p)++;
    ntrk = *((s16 *)p)++;
    if (D_800C9570_3 != NULL) {
        func_800BCB04(D_800C9570_3);
    }
    D_800C9570_3 = func_800BC908(((s16)n - 1) * sizeof(Rec_54) + sizeof(Obj_77), 0, (s32)D_800DB040, 0xFD);
    D_800C9570_3->id = id;
    D_800C9570_3->ntrk = ntrk;
    for (i = 0; i < ntrk; i++) {
        D_800C9570_3->len[i] = p[1];
        func_800AEC34((u8 *)(i * 30 + (u32)D_800C9570_3) + 0x14, p + 2, D_800C9570_3->len[i] * 4 - 2);
        p += D_800C9570_3->len[i] * 4;
    }
    D_800C9570_3->n = (s16)n;
    for (k = 0; k < (s16)n; k++) {
        j = (k - (s16)start + (s16)n) % (s16)n;
        D_800C9570_3->rec[j].a = p[0] >> 3;
        D_800C9570_3->rec[j].b = (p[1] >> 6) | ((p[0] & 7) << 2);
        D_800C9570_3->rec[j].c = p[1] & 0x3F;
        D_800C9570_3->rec[j].d = p[2] >> 6;
        D_800C9570_3->rec[j].e = p[2] & 0x3F;
        p += 3;
    }
}

typedef struct { u16 a0; u16 h2; u8 p4[0x2C]; void *f30; void *f34; u8 p38[8]; } Rec_55;

typedef struct Node_35 {
    u16 used;
    u16 pad;
    struct Node_35 *next;
    struct Node_35 *prev;
    u16 a, b, c, d;
    Rec_55 *rec;
    u8 pad18[0x24 - 0x18];
} Node_35;

typedef struct Game_43 {
    u8 p000[0x17C];
    Node_35 *f17C;
    Node_35 *f180;
    Node_35 *f184;
    u8 p188[8];
    Node_35 *f190;
    Node_35 nodes[150];
    u8 p16AC[0x16BA - 0x16AC];
    Blk10 hdr;
    u8 f16C4;
    u8 p16C5[0x16F4 - 0x16C5];
    s32 f16F4;
    u8 p16F8[0x17F8 - 0x16F8];
    s32 f17F8;
} Game_43;

extern Game_43 *D_800CEA40_35;
extern Rec_55 D_800CE280_15[];

void func_800A4700(u8 *p)
{
    s16 i;
    s16 n;
    u16 w;
    s16 cnt, sel;
    s32 j;
    Rec_55 *rec;
    u16 *tp;

    {
        Game_43 *g;
        u8 v = p[1];
        p += 2;
        g = D_800CEA40_35;
        g->f17F8 = v;
        g->hdr = *(Blk10 *)p;
    }
    p += 10;
    D_800CEA40_35->f16C4 = 0;
    cnt = *(s16 *)p;
    p += 2;
    sel = *(s16 *)p;
    p += 2;
    D_800CEA40_35->f16F4 = *(s16 *)p;
    p += 2;
    for (i = 0; i < D_800CEA40_35->f16F4; i++) {
        n = *(s16 *)p;
        w = *(u16 *)p;
        tp = D_800F55B8[i];
        func_800AEC34(tp, p + 2, (n * 2 - 1) * 2);
        func_800A4FF0(D_800CE280_15[i + 0x12].f30, D_800CE280_15[i + 0x12].f34, tp, n);
        D_800CE280_15[i + 0x12].h2 = w;
        p = (u8 *)((s16 *)p + n * 2);
    }

    D_800CEA40_35->f17C = D_800CEA40_35->f184 = D_800CEA40_35->nodes;
    D_800CEA40_35->f180 = &D_800CEA40_35->nodes[cnt - 1];
    D_800CEA40_35->f190 = &D_800CEA40_35->nodes[sel];
    for (j = 0; j < cnt; j++) {
        D_800CEA40_35->nodes[j].used = 1;
        if (j == 0) {
            D_800CEA40_35->nodes[j].prev = 0;
        } else {
            D_800CEA40_35->nodes[j].prev = &D_800CEA40_35->nodes[j - 1];
        }
        if (j == cnt - 1) {
            D_800CEA40_35->nodes[j].next = 0;
        } else {
            D_800CEA40_35->nodes[j].next = &D_800CEA40_35->nodes[j + 1];
        }
        D_800CEA40_35->nodes[j].a = p[0] >> 3;
        D_800CEA40_35->nodes[j].b = (p[1] >> 6) | ((p[0] & 7) << 2);
        D_800CEA40_35->nodes[j].c = p[1] & 0x3F;
        D_800CEA40_35->nodes[j].d = p[2] >> 6;
        D_800CEA40_35->nodes[j].rec = &D_800CE280_15[p[2] & 0x3F];
        p += 3;
        func_80096B3C(&D_800CEA40_35->nodes[j]);
    }
    for (j = cnt; j < 150; j++) {
        D_800CEA40_35->nodes[j].used = 0;
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

extern Game_44 *D_800CEA40_36;
extern Out_5 *D_800C9570_4;
extern Tbl_7 D_800CE280_16[];

void func_800A4A20(void)
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
    n = D_800CEA40_36->listA;
    do {
        n = n->next;
        count++;
    } while (n != NULL && n != D_800CEA40_36->listA);
    if (D_800C9570_4 != NULL) {
        func_800BCB04(D_800C9570_4);
    }
    cnt = (s16)count;
    o = func_800BC908((cnt - 1) * 12 + 0x110, 0, D_800DB040, 0x1AE);
    f = D_800CEA40_36->val;
    D_800C9570_4 = o;
    o->cnt = cnt;
    o->val = f;
    k = D_800CEA40_36->nItems;
    i = 0;
    o->n = k;
    if (k > 0) {
        do {
            D_800C9570_4->len[i] = D_800CE280_16[i + 0x12].len;
            func_800AEC34((u8 *)(i * 30 + (s32)D_800C9570_4) + 0x14, D_800F55B8[i], D_800C9570_4->len[i] * 4 - 2);
            i++;
        } while (i < D_800C9570_4->n);
    }
    n = D_800CEA40_36->listB;
    k = 0;
    do {
        t = (s32)n->ref;
        D_800C9570_4->recs[k].a = n->a;
        D_800C9570_4->recs[k].b = n->b;
        D_800C9570_4->recs[k].c = n->c;
        d = n->d;
        D_800C9570_4->recs[k].idx = (t - (s32)D_800CE280_16) >> 6;
        D_800C9570_4->recs[k].d = d;
        n = n->next;
        k++;
        if (n == NULL) {
            n = D_800CEA40_36->listA;
            if (n == NULL) {
                return;
            }
        }
    } while (n != D_800CEA40_36->listB);
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

extern Hdr_11 D_800CE26C_2;
extern Ent_89 D_800CE280_17[];
extern Tab_10 *D_800C9570_5;

void func_800A4BEC(void)
{
    s32 i;
    u32 j, k;
    s32 c;
    s32 n;

    func_800BFC40(D_800DB04C, (s32)D_800248EC);
    func_800BFC40(D_800DB050, (s32)D_80031D08);
    func_800BF530(D_800DB054, &D_800D0680);
    D_800CE26C_2.r10 = func_800BF79C(D_800D0680, *D_800CE26C_2.p8);
    func_80031D24();
    func_8000ED10(D_800F5CC0_2, 0, 0x2C);

    for (i = 0; i < 29; i++) {
        D_800CE280_17[i].r38 = 0;
        D_800CE280_17[i].r3C = 0;
        if (!(D_800CE280_17[i].flags & 0x40)) {
        D_800CE280_17[i].s30 = 0;
        D_800CE280_17[i].s34 = 0;
        if (D_800CE280_17[i].p20 != 0) {
            D_800CE280_17[i].s30 = (Set *)func_800BF79C(D_800D0680, *D_800CE280_17[i].p20);
        }
        if (D_800CE280_17[i].p24 != 0) {
            D_800CE280_17[i].s34 = (Set *)func_800BF79C(D_800D0680, *D_800CE280_17[i].p24);
        }
        if (D_800CE280_17[i].p20 != 0 && (j = 0, j < D_800CE280_17[i].s30->n)) {
            do {
                Mesh *m = D_800CE280_17[i].s30->e[j].mesh;
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
            } while (j < D_800CE280_17[i].s30->n);
        }
        if (D_800CE280_17[i].p28 != 0) {
            D_800CE280_17[i].r38 = func_800BF79C(D_800D0680, *D_800CE280_17[i].p28) + 4;
        }
        if (D_800CE280_17[i].p2C != 0) {
            D_800CE280_17[i].r3C = func_800BF79C(D_800D0680, *D_800CE280_17[i].p2C);
            func_80031D24();
        }
        }
    }

    for (i = 18, n = 0; i < 30; i++, n++) {
        if (!(D_800CE280_17[i].flags & 0x40)) continue;
        D_800CE280_17[i].s30 = func_800BC908(D_800C9570_5->cnt[n] << 6 | 4, 0, D_800DB040, 0x27F);
        D_800CE280_17[i].s34 = func_800BC908(D_800C9570_5->cnt[n] << 6 | 4, 0, D_800DB040, 0x280);
        D_800CE280_17[i].h02 = D_800C9570_5->cnt[n];
        if (D_800CE280_17[i].p28 != 0) {
            D_800CE280_17[i].r38 = func_800BF79C(D_800D0680, *D_800CE280_17[i].p28) + 4;
        }
        if (D_800CE280_17[i].p2C != 0) {
            D_800CE280_17[i].r3C = func_800BF79C(D_800D0680, *D_800CE280_17[i].p2C);
            func_80031D24();
        }
        func_800A5484(D_800CE280_17[i].s30, D_800CE280_17[i].s34, D_800C9570_5->cnt[n]);
        func_800A4FF0(D_800CE280_17[i].s30, D_800CE280_17[i].s34, D_800C9570_5->rec[n], D_800C9570_5->cnt[n]);
        D_800CE280_17[i].h02 = D_800C9570_5->cnt[n];
    }
}

#define E(o, idx, kk) (*(o))->v[idx].v.ob[1] = ((kk) > 0 && (kk) < n * 2) ? h[(kk) - 1] : 0

struct Mesh_8 {
    u32 n;
    Vtx *v;
};

typedef struct {
    s32 x;
    Mesh_8 *mesh;
    u8 pad[0x18];
} Obj_78;

void func_800A4FF0(Mesh_8 **dst, Mesh_8 **src, s16 *h, s32 n)
{
    Mesh_8 **tmpl = D_800CE9B4;
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

extern Glob_10 D_800CE280_18;

void func_800A5484(Set *out1, Set *out2, s32 count)
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

    set1 = D_800CE280_18.s1;
    set2 = D_800CE280_18.s2;
    for (j = 0; j < count; j++) {
        for (i = 0; i < (n = set1->n); i++) {
            size = (set1->e[i].tex[1] << 4) | 4;
            o = &out1->e[j * n + i];
            o2 = &out2->e[j * set2->n + i];
            o->mesh = func_800BC908(sizeof(Mesh) + set1->e[i].mesh->nv * 16 + set1->e[i].mesh->ntri * 4 + size, 0, (s32)D_800DB040, 0x2F8);
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
            func_800AEC34(o->mesh->v, set1->e[i].mesh->v, o->mesh->nv * 16);
            func_800AEC34(o->mesh->tri, set1->e[i].mesh->tri, o->mesh->ntri * 4);
            func_800AEC34(o->tex, set1->e[i].tex, size);

            size2 = (set2->e[i].tex[1] << 4) | 4;
            o2->mesh = func_800BC908(sizeof(Mesh) + set2->e[i].mesh->nv * 16 + set2->e[i].mesh->ntri * 4 + size2, 0, (s32)D_800DB040, 0x30C);
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
            func_800AEC34(o2->mesh->v, set2->e[i].mesh->v, o2->mesh->nv * 16);
            func_800AEC34(o2->mesh->tri, set2->e[i].mesh->tri, o2->mesh->ntri * 4);
            func_800AEC34(o2->tex, set2->e[i].tex, size2);
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

extern Blk_26 D_800F5CC0_3;

void func_800A5840(void)
{
    func_800BFDD4(D_800D0680);
    D_800D0680 = 0;
    if (D_800F5CC0_3.f00 != 0) {
        func_800BCB04(D_800F5CC0_3.f00);
        func_800BCB04(D_800F5CC0_3.f08);
        func_800BCB04(D_800F5CC0_3.f10);
        func_800BCB04(D_800F5CC0_3.f14);
        func_800BCB04(D_800F5CC0_3.f18);
        func_800BCB04(D_800F5CC0_3.f1c);
        func_800BCB04(D_800F5CC0_3.f20);
        if (D_800F5CC0_3.f28->f14 != 0) {
            func_800BCB04(D_800F5CC0_3.f28->f14);
        }
        if (D_800F5CC0_3.f28->f24 != 0) {
            func_800BCB04(D_800F5CC0_3.f28->f24);
        }
        if (D_800F5CC0_3.f28->f1c != 0) {
            func_800BCB04(D_800F5CC0_3.f28->f1c);
        }
        if (D_800F5CC0_3.f28->f28 != 0) {
            func_800BCB04(D_800F5CC0_3.f28->f28);
        }
        if (D_800F5CC0_3.f28->f2c != 0) {
            func_800BCB04(D_800F5CC0_3.f28->f2c);
        }
        func_800BCB04(D_800F5CC0_3.f28);
    }
    func_800A5964(0);
    func_80031D24();
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

extern Rec_56 D_800CE280_19[];

static inline void freeOne(Rec_56 *q, s32 k)
{
    void **a;
    void **b;

    a = &q->f30[k * 8 + 1];
    b = &q->f34[k * 8 + 1];
    func_800BCB04(*a);
    func_800BCB04(*b);
}

void func_800A5964(s32 param_1)
{
    s32 i;
    s32 j;
    s32 n;

    for (n = 18; n < 30; n++) {
        if (D_800CE280_19[n].flags & 0x40) {
            for (i = 0; param_1 == 0 ? i < D_800CE280_19[n].count : i < 8; i++) {
                for (j = 0; j < 2; j++) {
                    freeOne(&D_800CE280_19[n], i * 2 + j);
                }
            }
            func_800BCB04(D_800CE280_19[n].f30);
            func_800BCB04(D_800CE280_19[n].f34);
        }
    }
}

#define PC (D_800C9570_6->pc[i])
#define EN (D_800CE280_20[D_800C9570_6->pc[i].type])
#define EP (D_800CE280_20[p->type])
#define R (D_800D0684[grid[c].rot])
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

extern Tab_11 *D_800C9570_6;
extern Ent_90 D_800CE280_20[];
extern Hdr_12 D_800CE26C_3;
extern Blk_27 D_800F5CC0_4;
extern Rot D_800D0684[];
extern Obj_79 *D_800EFD64_42;

void func_800A5A74(void)
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
    for (i = 0; i < D_800C9570_6->n; i++) {
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
    grid = func_800BC908(ncells * sizeof(Cell_11), 0, D_800DB040, 0x416);
    func_8000ED10(grid, 0, ncells * sizeof(Cell_11));
    ne = D_800CE26C_3.f10->ne;
    nv = D_800CE26C_3.f10->nv;
    nt = D_800CE26C_3.f10->nt;
    for (i = 0, p = D_800C9570_6->pc; i < D_800C9570_6->n; p++, i++) {
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
            grid[c].e = &D_800CE280_20[27];
            grid[c].a = &D_800CE280_20[27].s30->e[0];
            grid[c].col = 0;
            grid[c].row = 0;
            if (D_800CE280_20[27].s34 != NULL) {
                grid[c].b = &D_800CE280_20[27].s34->e[0];
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
    switch (D_800C9570_6->mode) {
    case 0:
        D_800F5CC0_4.f04 = func_800BF79C(D_800D0680, D_800D1348);
        break;
    case 1:
        D_800F5CC0_4.f04 = func_800BF79C(D_800D0680, D_800D134C);
        break;
    case 2:
        D_800F5CC0_4.f04 = func_800BF79C(D_800D0680, D_800D1350);
        break;
    }
    D_800F5CC0_4.f0C = func_800BF79C(D_800D0680, D_800D1370);
    D_800F5CC0_4.f00 = func_800BC908(ncells * sizeof(Entry_8) | 4, 0, D_800DB040, 0x4F5);
    D_800F5CC0_4.f00->n = ncells;
    D_800F5CC0_4.f08 = func_800BC908(ncells * sizeof(Entry_8) | 4, 0, D_800DB040, 0x4F8);
    D_800F5CC0_4.f08->n = ncells;
    D_800F5CC0_4.f24 = func_800BC908((cnt / 5 + 1) * sizeof(Rec_57) + 8, 0, D_800DB040, 0x4FB);
    D_800F5CC0_4.f28 = func_800BC908(sizeof(Geo), 0, D_800DB040, 0x4FC);
    D_800F5CC0_4.f28->v = func_800BC908(nv * sizeof(V4_2), 0, D_800DB040, 0x4FD);
    D_800F5CC0_4.f28->e = func_800BC908(ne * sizeof(V4_2), 0, D_800DB040, 0x4FE);
    D_800F5CC0_4.f28->t = func_800BC908(nt * sizeof(V6), 0, D_800DB040, 0x4FF);
    g68 = func_800BC908(0x8200, 0, D_800DB040, 0x500);
    g6C = func_800BC908(0x10200, 0, D_800DB040, 0x501);
    w = size[0] * 250;
    h = size[1] * 250;
    D_800F5CC0_4.f10 = func_800BC908(texsz, 0, D_800DB040, 0x508);
    D_800F5CC0_4.f14 = func_800BC908(nva, 0, D_800DB040, 0x509);
    D_800F5CC0_4.f18 = func_800BC908(ncells * sizeof(Mesh_9), 0, D_800DB040, 0x50A);
    D_800F5CC0_4.f1C = func_800BC908(nvb, 0, D_800DB040, 0x50C);
    D_800F5CC0_4.f20 = func_800BC908(ncells * sizeof(Mesh_9), 0, D_800DB040, 0x50D);
    ebase = D_800CE26C_3.f10->ne;
    vbase = D_800CE26C_3.f10->nv;
    tbase = D_800CE26C_3.f10->nt;
    D_800F5CC0_4.f28->nv = nv;
    D_800F5CC0_4.f28->nt = nt;
    D_800F5CC0_4.f28->ne = ne;
    D_800F5CC0_4.f28->xmax = -1000000;
    D_800F5CC0_4.f28->ymax = -1000000;
    D_800F5CC0_4.f28->xmin = 1000000;
    D_800F5CC0_4.f28->ymin = 1000000;
    for (j = 0; j < D_800CE26C_3.f10->nv; j++) {
        D_800F5CC0_4.f28->v[j].a = D_800CE26C_3.f10->v[j].a;
        D_800F5CC0_4.f28->v[j].b = D_800CE26C_3.f10->v[j].b;
        D_800F5CC0_4.f28->v[j].c = D_800CE26C_3.f10->v[j].c;
        D_800F5CC0_4.f28->v[j].d = D_800CE26C_3.f10->v[j].d;
    }
    for (j = 0; j < D_800CE26C_3.f10->ne; j++) {
        D_800F5CC0_4.f28->e[j].a = D_800CE26C_3.f10->e[j].a;
        D_800F5CC0_4.f28->e[j].b = D_800CE26C_3.f10->e[j].b;
        D_800F5CC0_4.f28->e[j].c = -0x8000;
        D_800F5CC0_4.f28->e[j].d = 0x40;
    }
    for (j = 0; j < D_800CE26C_3.f10->nt; j++) {
        D_800F5CC0_4.f28->t[j].a = D_800CE26C_3.f10->t[j].a;
        D_800F5CC0_4.f28->t[j].b = D_800CE26C_3.f10->t[j].b;
        D_800F5CC0_4.f28->t[j].c = D_800CE26C_3.f10->t[j].c;
        D_800F5CC0_4.f28->t[j].d = D_800CE26C_3.f10->t[j].d;
        D_800F5CC0_4.f28->t[j].e = D_800CE26C_3.f10->t[j].e;
        D_800F5CC0_4.f28->t[j].f = D_800CE26C_3.f10->t[j].f;
    }
    bmw = (nv + 15) / 16;
    func_8000ED10(D_800C29E0_2 + bmw, 0xFF, nv * 2);
    for (i = 0, p = D_800C9570_6->pc; i < D_800C9570_6->n; p++, i++) {
        e = &D_800CE280_20[p->type];
        if (e->p3C != NULL) {
            D_800F5CC0_4.f28->xmin = -3750;
            D_800F5CC0_4.f28->xmax = 3750;
            D_800F5CC0_4.f28->ymin = -3750;
            D_800F5CC0_4.f28->ymax = 3750;
            if (e->flags & 0x40) {
                reps = e->dim[1] * 2;
            } else {
                reps = 1;
            }
            for (k = 0; k < reps; k++) {
                for (j = 0; j < e->p3C->nv; j++) {
                    D_800C29E0_2[bmw + vbase + j] = i;
                    D_800F5CC0_4.f28->v[vbase + j].c = e->p3C->v[j].c;
                    D_800F5CC0_4.f28->v[vbase + j].d = e->p3C->v[j].d;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0_4.f28->v[vbase + j].a = e->p3C->v[j].a;
                        D_800F5CC0_4.f28->v[vbase + j].b = e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k];
                        }
                        D_800F5CC0_4.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        D_800F5CC0_4.f28->v[vbase + j].b += p->y * 125 - h / 4 + (k * 4000) / 64;
                        break;
                    case 1:
                        D_800F5CC0_4.f28->v[vbase + j].a = e->p3C->v[j].b;
                        D_800F5CC0_4.f28->v[vbase + j].b = e->dim[0] * 125 - e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k];
                        }
                        D_800F5CC0_4.f28->v[vbase + j].a += p->x * 125 - w / 4 + (k * 4000) / 64;
                        D_800F5CC0_4.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    case 2:
                        D_800F5CC0_4.f28->v[vbase + j].a = e->dim[0] * 125 - e->p3C->v[j].a;
                        D_800F5CC0_4.f28->v[vbase + j].b = e->dim[1] * 125 - e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][k];
                        }
                        D_800F5CC0_4.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        D_800F5CC0_4.f28->v[vbase + j].b += p->y * 125 - h / 4 - (k * 4000) / 64;
                        break;
                    case 3:
                        D_800F5CC0_4.f28->v[vbase + j].a = e->dim[1] * 125 - e->p3C->v[j].b;
                        D_800F5CC0_4.f28->v[vbase + j].b = e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            kk = reps - k - 1;
                            if (kk != 0) {
                                D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][reps - k - 2];
                            }
                        } else if ((kr = reps - k - 1) != reps - 1) {
                            D_800F5CC0_4.f28->v[vbase + j].d += D_800C9570_6->rec[e - &D_800CE280_20[18]][kr];
                        }
                        D_800F5CC0_4.f28->v[vbase + j].a += p->x * 125 - w / 4 - ((reps - k - 1) * 4000) / 64;
                        D_800F5CC0_4.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    }
                }
                for (j = 0; j < e->p3C->ne; j++) {
                    D_800F5CC0_4.f28->e[ebase + j].a = e->p3C->e[j].a + vbase;
                    D_800F5CC0_4.f28->e[ebase + j].b = e->p3C->e[j].b + vbase;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0_4.f28->e[ebase + j].c = e->p3C->e[j].c;
                        D_800F5CC0_4.f28->e[ebase + j].d = e->p3C->e[j].d;
                        break;
                    case 1:
                        D_800F5CC0_4.f28->e[ebase + j].c = e->p3C->e[j].d;
                        D_800F5CC0_4.f28->e[ebase + j].d = -e->p3C->e[j].c;
                        break;
                    case 2:
                        D_800F5CC0_4.f28->e[ebase + j].c = -e->p3C->e[j].c;
                        D_800F5CC0_4.f28->e[ebase + j].d = -e->p3C->e[j].d;
                        break;
                    case 3:
                        D_800F5CC0_4.f28->e[ebase + j].c = -e->p3C->e[j].d;
                        D_800F5CC0_4.f28->e[ebase + j].d = e->p3C->e[j].c;
                        break;
                    }
                    D_800F5CC0_4.f28->e[ebase + j].c &= ~1;
                    D_800F5CC0_4.f28->e[ebase + j].d &= ~1;
                }
                for (j = 0; j < e->p3C->nt; j++) {
                    D_800F5CC0_4.f28->t[tbase + j].a = e->p3C->t[j].a + vbase;
                    D_800F5CC0_4.f28->t[tbase + j].b = e->p3C->t[j].b + vbase;
                    D_800F5CC0_4.f28->t[tbase + j].c = e->p3C->t[j].c + vbase;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0_4.f28->t[tbase + j].d = e->p3C->t[j].d;
                        D_800F5CC0_4.f28->t[tbase + j].f = e->p3C->t[j].f;
                        break;
                    case 1:
                        D_800F5CC0_4.f28->t[tbase + j].d = e->p3C->t[j].f;
                        D_800F5CC0_4.f28->t[tbase + j].f = -e->p3C->t[j].d;
                        break;
                    case 2:
                        D_800F5CC0_4.f28->t[tbase + j].d = -e->p3C->t[j].d;
                        D_800F5CC0_4.f28->t[tbase + j].f = -e->p3C->t[j].f;
                        break;
                    case 3:
                        D_800F5CC0_4.f28->t[tbase + j].d = -e->p3C->t[j].f;
                        D_800F5CC0_4.f28->t[tbase + j].f = e->p3C->t[j].d;
                        break;
                    }
                    D_800F5CC0_4.f28->t[tbase + j].e = e->p3C->t[j].e;
                }
                ebase += e->p3C->ne;
                vbase += e->p3C->nv;
                tbase += e->p3C->nt;
            }
        }
    }
    func_8000ED10(D_800C29E0_2, 0, bmw * 2);
    for (j = D_800CE26C_3.f10->nv; j < nv; j++) {
        if (!((D_800C29E0_2[j / 16] >> (j % 16)) & 1)) {
            for (l = j + 1; l < nv; l++) {
                if (!((D_800C29E0_2[l / 16] >> (l % 16)) & 1)) {
                    dx = D_800F5CC0_4.f28->v[j].a - D_800F5CC0_4.f28->v[l].a;
                    dy = D_800F5CC0_4.f28->v[j].b - D_800F5CC0_4.f28->v[l].b;
                    if (dx * dx + dy * dy < 16) {
                        D_800F5CC0_4.f28->v[l].a += dx / 2;
                        D_800F5CC0_4.f28->v[l].b += dy / 2;
                        D_800F5CC0_4.f28->v[j].a -= dx - dx / 2;
                        D_800F5CC0_4.f28->v[j].b -= dy - dy / 2;
                        D_800C29E0_2[l / 16] |= 1 << (l % 16);
                        D_800C29E0_2[j / 16] |= 1 << (j % 16);
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
    for (b = 0; b < D_800F5CC0_4.f28->ne; b++) {
        pt[0] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->e[b].a].a + w / 4;
        pt[1] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->e[b].a].b + h / 4;
        pt[2] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->e[b].b].a + w / 4;
        pt[3] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->e[b].b].b + h / 4;
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
    for (b = 0; b < D_800F5CC0_4.f28->nt; b++) {
        pt[0] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].a].a + w / 4;
        pt[1] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].a].b + h / 4;
        pt[2] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].b].a + w / 4;
        pt[3] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].b].b + h / 4;
        pt[4] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].c].a + w / 4;
        pt[5] = D_800F5CC0_4.f28->v[D_800F5CC0_4.f28->t[b].c].b + h / 4;
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
    D_800F5CC0_4.f28->cell[0].b = func_800BC908(sz2, 0, D_800DB040, 0x6A8);
    D_800F5CC0_4.f28->cell[0].a = func_800BC908(sz1, 0, D_800DB040, 0x6A9);
    for (c = 0; c < 257; c++) {
        if (c > 0) {
            D_800F5CC0_4.f28->cell[c].a = D_800F5CC0_4.f28->cell[c - 1].a + g68[(c - 1) * 65];
            D_800F5CC0_4.f28->cell[c].b = D_800F5CC0_4.f28->cell[c - 1].b + g6C[(c - 1) * 129];
        }
        if (c < 256) {
            if (g68[c * 65] != 0) {
                func_800AEC34(D_800F5CC0_4.f28->cell[c].a, g68 + c * 65 + 1, g68[c * 65] * 2);
            }
            if (g6C[c * 129] != 0) {
                func_800AEC34(D_800F5CC0_4.f28->cell[c].b, g6C + c * 129 + 1, g6C[c * 129] * 2);
            }
        }
    }
    func_800BCB04(g68);
    func_800BCB04(g6C);
    func_80031A78(&D_800F5CC0_4.f28, 0);
    va = D_800F5CC0_4.f14;
    ma = D_800F5CC0_4.f18;
    tex = D_800F5CC0_4.f10;
    vb = D_800F5CC0_4.f1C;
    mb = D_800F5CC0_4.f20;
    ea = D_800F5CC0_4.f00->e;
    eb = D_800F5CC0_4.f08->e;
    *(s32 *)D_800F5CC0_4.f24 = 0xAA000003;
    D_800F5CC0_4.f24 = (RecSet_2 *)((s32 *)D_800F5CC0_4.f24 + 1);
    D_800F5CC0_4.f24->n = 0;
    rec = D_800F5CC0_4.f24->r;
    rc = 0;
    for (i = 0, p = D_800C9570_6->pc; i < D_800C9570_6->n; p++, i++) {
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
            rec->prev = D_800F5CC0_4.f24->n - 1;
            rec->next = ++D_800F5CC0_4.f24->n;
            rec->f12 = -1;
            rec->fE = -1;
            rec++;
        }
        rc -= n;
    }
    D_800F5CC0_4.f24->r[0].prev = D_800F5CC0_4.f24->n - 1;
    D_800F5CC0_4.f24->r[D_800F5CC0_4.f24->n - 1].next = 0;
    D_800F5CC0_4.f24 = (RecSet_2 *)((s32 *)D_800F5CC0_4.f24 - 1);
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
    func_800BCB04(grid);
    D_800EFD64_42->v68.x = pos.x;
    D_800EFD64_42->v68.y = pos.y;
    D_800EFD64_42->v68.z = pos.z;
}

typedef struct Node_37 { u8 pad[4]; struct Node_37 *next; } Node_37;
typedef struct { u8 pad[0x17C]; Node_37 *head; u8 pad2[0x16F4 - 0x180]; s32 count; } Game_45;

extern Game_45 *D_800CEA40_37;

s16 func_800A9B80(void)
{
    s32 cnt = 0;
    Node_37 *n = D_800CEA40_37->head;
    s32 total;
    s32 i;

    if (n != NULL) {
        do {
            n = n->next;
            cnt++;
        } while (n != NULL && n != D_800CEA40_37->head);
    }
    total = (s16)cnt * 3 + 8;
    for (i = 0; i < D_800CEA40_37->count; i++) { total += D_800CE280[i + 18].h1 * 4; }
    return total + 10;
}
