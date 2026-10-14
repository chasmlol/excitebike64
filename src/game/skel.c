#include "game.h"

Blk_2 *func_8003AAE0(Blk_2 *gdl, s32 unused, s32 n)
{
    s32 k = n;
    Blk_2 *p0 = gdl++;
    Blk_2 *p1 = gdl++;
    u32 c1 = 0xE3000A01;
    Blk_2 *p2 = gdl++;
    Blk_2 *p3 = gdl++;
    Blk_2 *p4, *p5;
    Blk_2 *p6;
    Blk_2 *q0, *q1, *q2;

    p0->w0 = 0xE7000000;
    p0->w1 = 0;
    p1->w1 = 0x300000;
    p1->w0 = c1;
    p2->w0 = 0xF7000000;
    p2->w1 = 0;
    p3->w0 = 0xED000000;
    p3->w1 = 0x80080;
    p4 = gdl++;
    p4->w0 = 0xE7000000;
    p4->w1 = 0;
    p5 = gdl++;
    p5->w0 = 0xFF68001F;
    p5->w1 = D_800E92DC + k * 0x400;
    p6 = gdl++;
    p6->w0 = 0xF607C07C;
    p6->w1 = 0;
    if (D_800E37C0[k * 0x7E0 + 0x44] != 0) {
        q0 = gdl++;
        q0->w0 = 0xE7000000;
        q0->w1 = 0;
        q1 = gdl++;
        q1->w0 = 0xFF68001F;
        q1->w1 = D_800E92DC + (k + 8) * 0x400;
        q2 = gdl++;
        q2->w0 = 0xF607C07C;
        q2->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE7000000;
        e->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE3000A01;
        e->w1 = 0;
    }
    return gdl;
}

typedef struct {
    u8 pad0[0x220];
    Gfx *dl220;
    u8 pad224[0x22C - 0x224];
    void *seg22C;
    u8 pad230[0x234 - 0x230];
    void *seg234[2];
    void *mtx23C[(0xA80 - 0x23C) / 4];
} Plr_4;

typedef struct {
    u8 pad[0x44];
    u8 f44;
    u8 pad45[0x7E0 - 0x45];
} Blk_9;

extern Blk_9 D_800E37C0_37[];
extern Plr_4 D_800DC150_9[];

Gfx *func_8003AC18(s32 idx, Gfx *gdl, Mtx *m)
{
    f32 mf[4][4];
    s16 rot[64];
    Blk_9 *blk;
    Gfx **tab;
    s32 i;

    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &D_800C99A0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 1, 1, 31, 31);
    gSPClipRatio(gdl++, FRUSTRATIO_6);
    blk = &D_800E37C0_37[idx];
    guOrtho(mf, -15.0f, 15.0f, -15.0f, 15.0f, 50.0f, 4192.0f, 1.0f);
    func_80011490(rot, -0x4000, 0, 0, 0, 0, 0, 4);
    func_800120A4(rot, (s16 *)mf, (s16 *)&D_800E95E8);
    func_80011490(&D_800E9630, 0, 0, 0, 0, 0, 0, 0x100);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
    gSPMatrix(gdl++, &D_800E95E8, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &D_800E9630, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, D_800E92DC + idx * 0x400);

    for (i = 0; i < 6; i++) {
        gSPMatrix(gdl++, (u8 *)D_800DC150_9[idx].mtx23C[D_800E30A8] + i * 0x40, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gdl++, D_800E16C0[i]);
        gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    }
    if (blk->f44) {
        gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, D_800E92DC + (idx + 8) * 0x400);
    }
    gSPSegment(gdl++, 1, D_800DC150_9[0].seg22C);
    gSPSegment(gdl++, 6, D_800DC150_9[idx].seg234[D_800E30A8]);
    if (!(D_800C2280 & 0x20)) {
        gSPDisplayList(gdl++, D_800DC150_9[0].dl220);
    }
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, m + 16, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPSetGeometryMode(gdl++, D_800E19CC);
    return gdl;
}

Gfx *func_8003B0AC(s32 id, s32 unused, Gfx *gdl, s32 arg3) {
    s32 i;

    if (D_800E8D68 == 0 || D_800E9670 == 0 || D_800E92E0 == 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetCombine(gdl++, 0xFFB3FF, 0xFF65FEFF);
    if (D_800F14E8 == 0) {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[id % 8] * 97 / 256);
    } else {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[id % 8]);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 1);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPLoadTextureBlock(gdl++, D_800E92DC + (id << 10), G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPVertex(gdl++, &D_800E9628[D_800E30A8 * 1152 + id * 72], D_800E92E0, 0);
    for (i = 0; i < D_800E9670 - 1; i += 2) {
        gSP2Triangles(gdl++, D_800E93D8[i * 3], D_800E93D8[i * 3 + 1], D_800E93D8[i * 3 + 2], 0,
                      D_800E93D8[i * 3 + 3], D_800E93D8[i * 3 + 4], D_800E93D8[i * 3 + 5], 0);
    }
    if (D_800E9670 & 1) {
        gSP1Triangle(gdl++, D_800E93D8[i * 3], D_800E93D8[i * 3 + 1], D_800E93D8[i * 3 + 2], 0);
    }
    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    return gdl;
}

Gfx *func_8003B558(s32 param_1, s32 param_2, Gfx *gdl)
{
    Gfx *g0;
    Gfx *g1;
    Gfx *g2;
    Gfx *g3;
    Gfx *g4;
    Gfx *g5;
    Gfx *g6;
    Gfx *g7;
    Gfx *g8;
    Gfx *g9;
    Gfx *g10;
    Gfx *g11;
    Gfx *g12;
    Gfx *g13;
    Gfx *g14;
    Gfx *g15;
    Gfx *g16;
    Gfx *g17;
    u32 c;

    g0 = gdl++;
    g1 = gdl++;
    c = 0xE200001C;
    ((u32 *)g0)[0] = 0xE7000000;
    ((u32 *)g0)[1] = 0;
    ((u32 *)g1)[0] = c;
    ((u32 *)g1)[1] = 0x0C084000;
    g2 = gdl++;
    ((u32 *)g2)[0] = 0xFCFFFFFF;
    ((u32 *)g2)[1] = 0xFFFCF87C;
    g3 = gdl++;
    ((u32 *)g3)[0] = 0xE3001001;
    ((u32 *)g3)[1] = 0;
    g4 = gdl++;
    ((u32 *)g4)[0] = 0xE3000C00;
    ((u32 *)g4)[1] = 0;
    g5 = gdl++;
    ((u32 *)g5)[0] = 0xD7000002;
    ((u32 *)g5)[1] = 0x80008000;
    g6 = gdl++;
    ((u32 *)g6)[0] = 0xFD700000;
    ((u32 *)g6)[1] = (u32)(D_800E92DC + param_1 * 0x400);
    g7 = gdl++;
    ((u32 *)g7)[0] = 0xF5700000;
    ((u32 *)g7)[1] = 0x07098260;
    g8 = gdl++;
    ((u32 *)g8)[0] = 0xE6000000;
    ((u32 *)g8)[1] = 0;
    g9 = gdl++;
    ((u32 *)g9)[0] = 0xF3000000;
    ((u32 *)g9)[1] = 0x071FF200;
    g10 = gdl++;
    ((u32 *)g10)[0] = 0xE7000000;
    ((u32 *)g10)[1] = 0;
    g11 = gdl++;
    ((u32 *)g11)[0] = 0xF5680800;
    ((u32 *)g11)[1] = 0x00098260;
    g12 = gdl++;
    ((u32 *)g12)[0] = 0xF2000000;
    ((u32 *)g12)[1] = 0x0007C07C;
    g13 = gdl++;
    ((u32 *)g13)[0] = 0xE41C41C4;
    ((u32 *)g13)[1] = 0x000C80C8;
    g14 = gdl++;
    ((u32 *)g14)[0] = 0xE1000000;
    ((u32 *)g14)[1] = 0;
    g15 = gdl++;
    ((u32 *)g15)[0] = 0xF1000000;
    ((u32 *)g15)[1] = 0x04000400;
    g16 = gdl++;
    ((u32 *)g16)[0] = 0xE7000000;
    ((u32 *)g16)[1] = 0;
    g17 = gdl++;
    ((u32 *)g17)[0] = 0xE3000C00;
    ((u32 *)g17)[1] = 0x00080000;
    return gdl;
}

typedef struct {
    s32 x, y, z;
    s32 x2, y2, z2;
    u8 pad[0x7E0 - 0x18];
} Player_11;

typedef struct {
    u8 pad[8];
    Vec3i v[3];
} Facet;

extern Player_11 D_800E37C0_38[];
extern Facet D_800E80D8_2[];

void func_8003B7E0(s32 n)
{
    s32 x, y, z;
    s32 i, j, k, cnt;
    Vtx *v;
    u32 vb;
    Vec3i *src;

    if (n < 8) {
        x = D_800E37C0_38[n].x;
        y = D_800E37C0_38[n].y;
        z = D_800E37C0_38[n].z;
    } else {
        x = D_800E37C0_38[n % 8].x2;
        y = D_800E37C0_38[n % 8].y2;
        z = D_800E37C0_38[n % 8].z2;
    }
    func_8002F62C(x - 1000, z - 1000, x + 1000, z + 1000, D_800E37C0_38[n].y + 1000);
    if (D_800E8D68 == 0) return;
    D_800E92E0 = 0;
    D_800E9670 = 0;
    k = D_800E30A8 * 0x480;
    k += n * 0x48;
    vb = (u32)D_800E9628 + k * 16;
    for (i = 0; i < D_800E8D68; i++) {
        for (j = 0; j < 3; j++) {
            D_800E80D8_2[i].v[j].x -= x;
            D_800E80D8_2[i].v[j].y -= y;
            D_800E80D8_2[i].v[j].z -= z;
        }
        func_8003BB1C(&D_800E80D8_2[i], 1000);
        cnt = D_800E9670 - 2;
        if (cnt + D_800E92D8 >= 0x48) return;
        if (D_800E92E0 + D_800E92D8 >= 0x20) return;
        j = D_800E92E0;
        for (k = 0; k < D_800E92D8; k++) {
            v = (Vtx *)(D_800E92E0 * 16 + vb);
            v->v.ob[0] = D_800E92E8[k].x;
            v->v.ob[1] = D_800E92E8[k].y;
            v->v.ob[2] = D_800E92E8[k].z;
            v->v.tc[0] = ((D_800E92E8[k].x + 1000) << 11) / 2000;
            D_800E92E0++;
            v->v.tc[1] = ((-D_800E92E8[k].z + 1000) << 11) / 2000;
        }
        for (k = 2; k < D_800E92D8; k++) {
            D_800E93D8[D_800E9670 * 3] = j;
            D_800E93D8[D_800E9670 * 3 + 1] = j + (k - 1);
            D_800E93D8[D_800E9670 * 3 + 2] = j + k;
            D_800E9670++;
        }
    }
}

void func_8003BB1C(Facet_2 *a, s32 lim)
{
    Vec *o;
    Vec *p;
    Vec *q;
    s32 i;
    s32 t;


    D_800E92E8_2[0] = a->v[0];
    D_800E92E8_2[1] = a->v[1];
    D_800E92E8_2[2] = a->v[2];
    D_800E92D4 = 3;
    o = D_800E94F8;
    i = 0;
    D_800E92D8 = 0;
    for (; i < D_800E92D4; i++) {
        p = &D_800E92E8_2[i];
        q = &D_800E92E8_2[(i + 1) % D_800E92D4];
        if (p->x <= lim) {
            *o++ = *p;
            D_800E92D8++;
        }
        if ((p->x <= lim && q->x > lim) || (p->x > lim && q->x <= lim)) {
            t = ((lim - p->x) << 8) / (q->x - p->x);
            o->x = lim;
            o->y = p->y + t * (q->y - p->y) / 256;
            o->z = p->z + t * (q->z - p->z) / 256;
            o++;
            D_800E92D8++;
        }
    }

    D_800E92D4 = D_800E92D8;
    o = D_800E92E8_2;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E94F8[i];
            q = &D_800E94F8[(i + 1) % D_800E92D4];
            if (p->x >= -lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->x >= -lim && q->x < -lim) || (p->x < -lim && q->x >= -lim)) {
                t = ((-lim - p->x) << 8) / (q->x - p->x);
                o->x = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->z = p->z + t * (q->z - p->z) / 256;
                o++;
                D_800E92D8++;
            }
        }

    D_800E92D4 = D_800E92D8;
    o = D_800E94F8;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E92E8_2[i];
            q = &D_800E92E8_2[(i + 1) % D_800E92D4];
            if (p->z <= lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->z <= lim && q->z > lim) || (p->z > lim && q->z <= lim)) {
                t = ((lim - p->z) << 8) / (q->z - p->z);
                o->z = lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                D_800E92D8++;
            }
        }

    D_800E92D4 = D_800E92D8;
    o = D_800E92E8_2;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E94F8[i];
            q = &D_800E94F8[(i + 1) % D_800E92D4];
            if (p->z >= -lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->z >= -lim && q->z < -lim) || (p->z < -lim && q->z >= -lim)) {
                t = ((-lim - p->z) << 8) / (q->z - p->z);
                o->z = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                D_800E92D8++;
            }
        }
}

void func_8003C21C(void)
{
    u32 x;

    D_800E9628 = func_800BC908(0x9000, 0, D_800D6DF4, 0x1F6);
    D_800E92D0 = func_800BC908(0x4040, 0, D_800D6DF4, 0x1F9);
    func_8000ED10((u8 *)D_800E92D0, 0, 0x4040);
    x = (u32)D_800E92D0;
    D_800E92DC = x;
    while ((x & 0x3F) != 0) {
        D_800E92DC = D_800E92DC + 0x10;
        x = D_800E92DC;
    }
}

void func_8003C2AC(void)
{
    func_800BCB04(D_800E9628);
    func_800BCB04(D_800E92D0);
}

typedef struct {
    s32 f0;
    s32 f4[9];
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
} Ent_33;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10[9];
    s32 pad[19];
} Out_3;

extern Out_3 D_800E9680[];

static inline void walk(Ent_33 *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = D_800EA680;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            D_800EA680++;
            func_8003C834(base, i, base[D_800EA680 - 1].f34);
        } else {
            D_800EA680++;
        }
        n--;
    }
}

void *func_8003C2E0(u32 *param_1, s32 param_2)
{
    Ent_33 *ents;
    Ent_33 *p;
    s32 sum;
    s32 cnt;
    s32 i;
    s32 j;
    void *r;

    ents = func_800BF79C(param_1, param_2);
    sum = 0;
    cnt = 0;
    do {
        ents[cnt].f0 = -1;
        sum += ents[cnt].f34;
        sum--;
        cnt++;
    } while (sum >= 0);
    D_800EA680 = 0;
    walk(ents, -1, 1);
    for (i = 0; i < cnt; i++) {
        D_800E9680[i].f4 = ents[i].f28;
        D_800E9680[i].f8 = ents[i].f2C;
        D_800E9680[i].fC = ents[i].f30;
        D_800E9680[i].f0 = ents[i].f0;
        for (sum = 0; sum < 9; sum++) {
            D_800E9680[i].f10[sum] = ents[i].f4[sum];
        }
    }
    func_800BFEA4((u32 *)(param_1[1] + param_2 * 4));
    r = func_800BC908(cnt * 0x80, 0, D_800D6E00, 0x74);
    memcpy(r, D_800E9680, cnt * 0x80);
    return r;
}

struct Node_6 {
    s32 parent;
    s32 pos[3];
    s32 rot[9];
    s32 wpos[3];
    s32 wmtx[16];
};

void func_8003C4A4(Node_6 *n, s32 count)
{
    s32 i;
    Node_6 *par;
    s32 v[3];

    n[0].wpos[0] = n[0].pos[0];
    n[0].wpos[1] = n[0].pos[1];
    n[0].wpos[2] = n[0].pos[2];
    for (i = 1; i < count; i++) {
        par = &n[n[i].parent];
        func_80011C1C(n[i].rot, par->wmtx, n[i].wmtx);
        func_80012DC8(n[i].pos, par->wmtx, n[i].wpos);
        n[i].wpos[0] += par->wpos[0];
        n[i].wpos[1] += par->wpos[1];
        n[i].wpos[2] += par->wpos[2];
    }
}

struct Rec_20 {
    u8 pad0[0x34];
    s32 v34[3];
    s32 m40[16];
};

s32 func_8003C588(Rec_20 *recs, s32 n, s32 *out, s32 *mat, s32 *pos, s32 idx)
{
    s32 rel[3];
    s32 wp[3];
    s32 d, sum;
    s32 i;

    d = pos[0] - D_800E8090[idx].x;
    if (d < 0) d = -d;
    sum = d;
    d = pos[1] - D_800E8090[idx].y;
    if (d < 0) d = -d;
    sum += d;
    d = pos[2] - D_800E8090[idx].z;
    if (d < 0) d = -d;
    sum += d;
    if (sum > 0x8000) return 1;
    for (i = 0; i < n; i++) {
        func_80012DC8(recs[i].v34, mat, wp);
        wp[0] += pos[0];
        wp[1] += pos[1];
        wp[2] += pos[2];
        rel[0] = (D_800E8090[idx].x >> 6) - wp[0];
        rel[1] = (D_800E8090[idx].y >> 6) - wp[1];
        rel[2] = (D_800E8090[idx].z >> 6) - wp[2];
        func_80012D94(recs[i].m40);
        func_80012DC8(rel, recs[i].m40, &out[i * 3]);
        func_80012D94(recs[i].m40);
    }
    return 0;
}

struct Blk_10 { u8 pad[0x34]; s32 x, y, z; u8 pad2[0x40]; };
typedef struct { s16 a, b, c; u16 idx; u8 pad[8]; } Ent_34;
struct List_6 { u32 n; Ent_34 *e; };

void func_8003C720(Blk_10 *tbl, List_6 *list) {
    u32 i;
    for (i = 0; i < list->n; i++) {
        u16 k = list->e[i].idx;
        list->e[i].a -= tbl[k].x;
        list->e[i].b -= tbl[k].y;
        list->e[i].c -= tbl[k].z;
    }
}

void func_8003C7A8(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 *p;

    i = 0;
    if (param_2 > 0) {
        p = param_1;
        do {
            func_80011A58(p + 4, 0, 0, 0);
            func_80011A58(p + 0x10, 0, 0, 0);
            i++;
            p += 0x20;
        } while (i < param_2);
    }
    func_8003C4A4(param_1, param_2);
}

struct Ent_35 {
    s32 f0;
    s32 pad[12];
    s32 f34;
};

void func_8003C834(Ent_35 *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = D_800EA680;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            D_800EA680++;
            func_8003C834(base, i, base[D_800EA680 - 1].f34);
        } else {
            D_800EA680++;
        }
        n--;
    }
}
