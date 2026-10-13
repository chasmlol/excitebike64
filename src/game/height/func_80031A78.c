#include "ctx.h"

typedef struct {
    s16 x;
    s16 y;
    s32 pad;
} Pt;

typedef struct {
    s16 a;
    s16 b;
    s16 nx;
    s16 ny;
} Edge;

typedef struct {
    u8 pad[10];
    s16 flags;
} Rec;

typedef struct {
    s32 a;
    s32 b;
} Pair;

typedef struct {
    s32 pad[5];
    Pt *pts;
    s32 nedges;
    Edge *edges;
    s32 nrecs;
    Rec *recs;
    Pair tab[0x101];
} Hdr;

extern u8 *func_800BC908(s32 a0, s32 a1, void *a2, s32 a3);
extern u8 D_800D6C20[];
extern u8 *D_800C9800;
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_8000EB18(s32 r);

void func_80031A78(Hdr **param_1, s32 param_2)
{
    Hdr *h = *param_1;
    s32 i;
    s32 dx;
    s32 dy;
    s32 len;

    if (param_2 != 0) {
        h->pts = (Pt *)((s32)h->pts + (s32)h);
        h->edges = (Edge *)((s32)h->edges + (s32)h);
        h->recs = (Rec *)((s32)h->recs + (s32)h);
        for (i = 0; i < 0x101; i++) {
            h->tab[i].a += (s32)h;
            h->tab[i].b += (s32)h;
        }
    }
    D_800C9800 = func_800BC908(h->nedges, 0, D_800D6C20, 0x87F);
    func_8000ED10(D_800C9800, 0, h->nedges);
    for (i = 0; i < h->nedges; i++) {
        D_800C9800[i] |= 1;
        if (h->edges[i].nx == -0x8000) {
            D_800C9800[i] = ((u8 *)&h->edges[i])[7] ^ 1;
        }
        dx = h->pts[h->edges[i].b].x - h->pts[h->edges[i].a].x;
        dy = h->pts[h->edges[i].b].y - h->pts[h->edges[i].a].y;
        len = func_8000EB18(dx * dx + dy * dy);
        if (len != 0) {
            dx = (dx * 0x4000) / len;
            dy = (dy * 0x4000) / len;
        }
        h->edges[i].nx = dy;
        h->edges[i].ny = -dx;
    }
    for (i = 0; i < h->nedges; i++) {
        h->edges[i].ny &= ~1;
    }
    for (i = 0; i < h->nrecs; i++) {
        h->recs[i].flags &= ~1;
    }
}
