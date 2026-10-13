#include "ctx.h"

typedef struct { u16 a0; u16 h2; u8 p4[0x2C]; void *f30; void *f34; u8 p38[8]; } Rec;
typedef struct { u8 b[10]; } Hdr;
typedef struct Node {
    u16 used;
    u16 pad;
    struct Node *next;
    struct Node *prev;
    u16 a, b, c, d;
    Rec *rec;
    u8 pad18[0x24 - 0x18];
} Node;
typedef struct Game {
    u8 p000[0x17C];
    Node *f17C;
    Node *f180;
    Node *f184;
    u8 p188[8];
    Node *f190;
    Node nodes[150];
    u8 p16AC[0x16BA - 0x16AC];
    Hdr hdr;
    u8 f16C4;
    u8 p16C5[0x16F4 - 0x16C5];
    s32 f16F4;
    u8 p16F8[0x17F8 - 0x16F8];
    s32 f17F8;
} Game;

extern Game *D_800CEA40;
extern Rec D_800CE280[];
extern u16 D_800F55B8[][15];

void func_800AEC34(u16 *dst, u8 *src, s32 n);
void func_800A4FF0(void *a, void *b, u16 *c, s32 n);
void func_80096B3C(Node *x);

void func_800A4700(u8 *p)
{
    s16 i;
    s16 n;
    u16 w;
    s16 cnt, sel;
    s32 j;
    Rec *rec;
    u16 *tp;

    {
        Game *g;
        u8 v = p[1];
        p += 2;
        g = D_800CEA40;
        g->f17F8 = v;
        g->hdr = *(Hdr *)p;
    }
    p += 10;
    D_800CEA40->f16C4 = 0;
    cnt = *(s16 *)p;
    p += 2;
    sel = *(s16 *)p;
    p += 2;
    D_800CEA40->f16F4 = *(s16 *)p;
    p += 2;
    for (i = 0; i < D_800CEA40->f16F4; i++) {
        n = *(s16 *)p;
        w = *(u16 *)p;
        tp = D_800F55B8[i];
        func_800AEC34(tp, p + 2, (n * 2 - 1) * 2);
        func_800A4FF0(D_800CE280[i + 0x12].f30, D_800CE280[i + 0x12].f34, tp, n);
        D_800CE280[i + 0x12].h2 = w;
        p = (u8 *)((s16 *)p + n * 2);
    }

    D_800CEA40->f17C = D_800CEA40->f184 = D_800CEA40->nodes;
    D_800CEA40->f180 = &D_800CEA40->nodes[cnt - 1];
    D_800CEA40->f190 = &D_800CEA40->nodes[sel];
    for (j = 0; j < cnt; j++) {
        D_800CEA40->nodes[j].used = 1;
        if (j == 0) {
            D_800CEA40->nodes[j].prev = 0;
        } else {
            D_800CEA40->nodes[j].prev = &D_800CEA40->nodes[j - 1];
        }
        if (j == cnt - 1) {
            D_800CEA40->nodes[j].next = 0;
        } else {
            D_800CEA40->nodes[j].next = &D_800CEA40->nodes[j + 1];
        }
        D_800CEA40->nodes[j].a = p[0] >> 3;
        D_800CEA40->nodes[j].b = (p[1] >> 6) | ((p[0] & 7) << 2);
        D_800CEA40->nodes[j].c = p[1] & 0x3F;
        D_800CEA40->nodes[j].d = p[2] >> 6;
        D_800CEA40->nodes[j].rec = &D_800CE280[p[2] & 0x3F];
        p += 3;
        func_80096B3C(&D_800CEA40->nodes[j]);
    }
    for (j = cnt; j < 150; j++) {
        D_800CEA40->nodes[j].used = 0;
    }
}
