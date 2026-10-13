#include "ctx.h"

typedef struct {
    u8 pad0[0x46];
    s16 next;
    u8 pad48[0x66 - 0x48];
    u16 pos;
    u8 pad68[0x6C - 0x68];
} Node;

typedef struct {
    u8 pad0[0x628];
    s16 node;
    u8 pad62A[0x63C - 0x62A];
    f32 frac;
    u8 pad640[0x7E0 - 0x640];
} Rider;

typedef struct {
    u8 pad0[0x21C];
    u8 r, g, b;
    u8 pad21F[0xA80 - 0x21F];
} Col;

extern Rider D_800E37C0[];
extern Node *D_800EE498;
extern s32 D_800C2388;
extern s32 D_800CCCF0;
extern s32 D_800CCCF4;
extern s32 D_800F13EC;
extern Col D_800DC150[];

Gfx *func_800685D8(Gfx *gdl, s32 idx, s32 half)
{
    Rider *r;
    Rider *rb;
    Node *a;
    Node *b;
    s32 v;
    s32 h;
    s32 k;
    f32 f;

    rb = D_800E37C0;
    r = &rb[idx];
    k = r->node;
    a = &D_800EE498[k];
    v = a->pos;
    k = a->next;
    b = &D_800EE498[k];
    if (b->pos < a->pos) {
        f = r->frac;
        if (f < 0.0f) {
            f = 0.0f;
        }
        if (1.0f < f) {
            f = 1.0f;
        }
        v = (s32)((f32)v + f * (f32)(b->pos - v));
    } else {
        v = b->pos;
    }
    if (D_800C2388 != 0) {
        v -= 0x3DD3;
        v = (v * 0x8000) / 0x422D;
    }
    v = 0x8000 - v;
    if (v < 0) {
        v = 0;
    }
    if (v > 0x8000) {
        v = 0x8000;
    }
    if (D_800C2388 != 0) {
        v = 0x7FFF - v;
    }
    v = (v * (D_800CCCF4 - D_800CCCF0)) >> 15;
    v += D_800CCCF0;
    if (D_800C2388 == 0) {
        Gfx *g = gdl;

        gDPSetPrimColor(g++, 0, 0, D_800DC150[idx].r, D_800DC150[idx].g, D_800DC150[idx].b, 255);
        if (half) {
            gDPFillRectangle(g++, v - 4, 117, v + 4, 123);
        } else {
            gDPFillRectangle(g++, v - 2, 118, v + 2, 122);
        }
        gdl = g;
    } else {
        Gfx *g;

        h = D_800F13EC / 2;
        if (v > 0xD5) {
            v = 0xD5;
        }
        g = gdl;
        gDPSetPrimColor(g++, 0, 0, D_800DC150[idx].r, D_800DC150[idx].g, D_800DC150[idx].b, 255);
        if (half) {
            gDPFillRectangle(g++, h - 4, v - 3, h + 4, v + 3);
        } else {
            gDPFillRectangle(g++, h - 2, v - 2, h + 2, v + 2);
        }
        gdl = g;
    }
    return gdl;
}
