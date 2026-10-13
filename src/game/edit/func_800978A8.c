#include "ctx.h"

typedef struct Cell {
    s32 f0;
    s32 f4;
    s32 pad8[6];
} Cell;

typedef struct Ent {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 pad18[2];
} Ent;

typedef struct Def {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    s16 fE;
    u8 pad10[0x20];
    Cell *cells;
    Ent *ents;
} Def;

typedef struct Obj {
    s32 f0;
    struct Obj *next;
    struct Obj *prev;
    s16 x;
    s16 y;
    u16 flags;
    s16 rot;
    Def *def;
    u8 pad18[8];
    s16 w;
    s16 h;
} Obj;

typedef struct {
    s16 w;
    s16 h;
} Map;

typedef struct {
    s32 dist;
    s32 ry;
    s32 y;
} Cam;

typedef struct {
    u8 pad0[0x170];
    s32 mode;
    s32 flags;
    s32 pad178;
    Obj *head;
    s32 pad180;
    Obj *sel;
    Obj *mark0;
    Obj *mark1;
    Obj *cur;
    u8 pad194[0x16AC - 0x194];
    Map *map;
    u8 pad16B0[0x16F8 - 0x16B0];
    s32 f16F8;
    s32 f16FC;
    s16 f1700;
    u8 pad1702[0x172C - 0x1702];
    Cam cam[2];
    u8 pad1744[0x174C - 0x1744];
    void *buf[2];
    s32 n;
    u8 pad1758[8];
    f32 mtx[4][4];
    u8 pad17A0[0x17C8 - 0x17A0];
    Vp vp;
    u8 pad17D8[0x17FC - 0x17D8];
    s16 f17FC;
    s16 pad17FE;
    s16 f1800;
} Edit;

typedef struct {
    s32 pad[4];
    s32 x, y, w, h;
} Scis;

typedef struct {
    s32 x;
    s32 z;
    u32 ang;
} CamPos;

extern Edit *D_800CEA40;
extern Scis D_800CE9C0;
extern CamPos D_800CE21C;
extern u16 D_800C2C60[];
extern s16 D_800F55B8[][15];
extern Mtx D_800F4F08[];
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern u8 D_800F1BC4;

void func_8000ED10(void *a0, u8 a1, s32 a2);
void func_8008F07C(s32 a0, s32 a1, s32 a2, s32 a3);
void func_8008D3B4(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
s32 func_80097658(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 a7, s32 a8);
Gfx *func_8008F230(Gfx *gdl, s32 a1, s32 a2, char *msg);
Gfx *func_8008F458(Gfx *gdl, s32 a1, s32 a2, char *msg, char *b0, char *b1, s32 sel, s32 n);
void func_8008D4C4(s32 idx, s32 x, s32 y, s32 z, s32 rx, s32 ry);
Gfx *func_80097308(s32 a0, s32 arg1, Gfx *gdl);
void func_80096FB0(s32 idx, s32 x0, s32 y0, s32 z0, s32 x1, s32 y1, s32 z1, s32 r, s32 g, s32 b);
Gfx *func_8008D728(s32 idx, Gfx *gdl, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void func_800B31C0(f32 mf[4][4], Mtx *m);

#define gDPRaw(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

#define SINT(i) ((s16)D_800C2C60[(i) & 0xFFF] >> 4)

#define CellAt(d, k) ((Cell *)((u8 *)(d)->cells + ((k) << 5)))
#define EntAt(d, k) ((Ent *)((u8 *)(d)->ents + ((k) << 5)))

#define W (o->def->dim[0])
#define H (o->def->dim[1])
#define K0 (j * W + i)
#define K1 (i * W + W - 1 - j)
#define K2 ((H - 1 - j) * W + W - 1 - i)
#define K3 ((H - 1 - i) * W + j)

#define E D_800CEA40
#define XB(a, b) (E->sel->x * 8000 - E->map->a * 4000)
#define ZB(a) (E->sel->y * 8000 - E->map->a * 4000)

Gfx *func_800978A8(s32 arg0, s32 arg1, Gfx *gdl) {
    s32 p0[3];
    s32 p1[3];
    s32 p2[3];
    s32 p3[3];
    Obj *o;
    s32 flag;
    s32 sel;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 x;
    s32 y;
    s16 rot;
    s32 fl;
    u32 a;
    s32 r;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0.x, D_800CE9C0.y, D_800CE9C0.x + D_800CE9C0.w - 1,
                  D_800CE9C0.y + D_800CE9C0.h - 1);
    gSPViewport(gdl++, &E->vp);
    gSPClearGeometryMode(gdl, G_ZBUFFER);
    gDPRaw(gdl++, 0xE200001C, 0x0C084000);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 1);
    gDPFillRectangle(gdl++, D_800CE9C0.x, D_800CE9C0.y, D_800CE9C0.x + D_800CE9C0.w - 1,
                     D_800CE9C0.y + D_800CE9C0.h - 1);
    gDPPipeSync(gdl++);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    gDPRaw(gdl++, 0xE200001C, 0x004041C8);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);

    if (E->mode == 4) {
        func_8008D4C4(arg1, 0, 250000, 0, 0, 0x4000);
    } else if (E->mode == 6) {
        a = D_800CE21C.ang;
        func_8008D4C4(arg1, D_800CE21C.x + ((SINT((a >> 4) - 0x400) * E->cam[1].dist) >> 10), E->cam[1].y,
                      D_800CE21C.z - ((SINT(a >> 4) * E->cam[1].dist) >> 10), a, E->cam[1].ry);
    } else {
        a = D_800CE21C.ang;
        func_8008D4C4(arg1, D_800CE21C.x + ((SINT((a >> 4) - 0x400) * E->cam[0].dist) >> 10), E->cam[0].y,
                      D_800CE21C.z - ((SINT(a >> 4) * E->cam[0].dist) >> 10), a, E->cam[0].ry);
    }

    o = E->head;
    if (o != NULL) {
        do {
            if (E->sel != NULL && E->mode == 6 && (E->sel->def->flags & 0x40)) {
                fl = o->flags & ~8;
            } else {
                fl = o->flags | 8;
            }
            o->flags = fl;
            o = o->next;
        } while (o != NULL && o != E->head);
    }
    if (E->sel != NULL && E->mode == 6 && (E->sel->def->flags & 0x40)) {
        for (o = E->sel->next; o != NULL; o = o->next) {
            o->flags |= 8;
            if (o->def->flags & 0x800) {
                break;
            }
        }
        for (o = E->sel; o != NULL; o = o->prev) {
            o->flags |= 8;
            if (o->def->flags & 0x800) {
                break;
            }
        }
    }

    o = E->head;
    flag = 0;
    if (o != NULL) {
        do {
            if (E->mark0 != NULL && E->mark1 != NULL) {
                if (flag != 0 && o->prev == E->mark1) {
                    flag = 0;
                }
                if (o == E->mark0) {
                    flag = 1;
                }
            }
            sel = 0;
            if ((o->flags & 4) || o == E->sel) {
                sel = 1;
            }
            sel |= 2;
            for (j = 0; j < o->def->dim[(o->rot + 1) & 1]; j++) {
                for (i = 0; i < o->def->dim[(rot = o->rot) & 1]; i++) {
                    if (o->x + i < 0 || o->x + i >= E->map->w) {
                        continue;
                    }
                    if (o->y + j < 0 || o->y + j >= E->map->h) {
                        continue;
                    }
                    if (o->flags & 8) {
                        switch (rot) {
                        case 0:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K0)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K0)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (EntAt(o->def, K0)->f8 + EntAt(o->def, K0)->f10) / 2 - 250 - i * 500 : 0,
                                          o->def->ents ? (EntAt(o->def, K0)->fC + EntAt(o->def, K0)->f14) / 2 - 250 - j * 500 : 0);
                            break;
                        case 1:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K1)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K1)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (EntAt(o->def, K1)->fC + EntAt(o->def, K1)->f14) / 2 - 250 - i * 500 : 0,
                                          o->def->ents ? (o->h - j - 1) * 500 + (250 - (EntAt(o->def, K1)->f8 + EntAt(o->def, K1)->f10) / 2) : 0);
                            break;
                        case 2:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K2)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K2)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (o->w - i - 1) * 500 + (250 - (EntAt(o->def, K2)->f8 + EntAt(o->def, K2)->f10) / 2) : 0,
                                          o->def->ents ? (o->h - j - 1) * 500 + (250 - (EntAt(o->def, K2)->fC + EntAt(o->def, K2)->f14) / 2) : 0);
                            break;
                        case 3:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K3)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K3)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (o->w - i - 1) * 500 + (250 - (EntAt(o->def, K3)->fC + EntAt(o->def, K3)->f14) / 2) : 0,
                                          o->def->ents ? (EntAt(o->def, K3)->f8 + EntAt(o->def, K3)->f10) / 2 - 250 - j * 500 : 0);
                            break;
                        }
                    } else {
                        func_8008D3B4(o->x + i, o->y + j, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            o = o->next;
        } while (o != NULL && o != E->head);
    }

    func_8008F07C(0, 0, D_800F13EC, D_800F13F0);
    if (E->cur != NULL) {
        r = ((-E->cur->rot * 2 - E->cur->def->fE) & 7) << 13;
        gdl = func_8008D728(arg1, gdl, E->sel->x, E->sel->y, E->sel->w, E->sel->h,
                            (E->cur->x * 2 + E->cur->w) * 4000 - 60000, (E->cur->y * 2 + E->cur->h) * 4000 - 60000,
                            (E->cur->def->flags & 2) ? r : -1, 1);
        if (E->mode == 6) {
            E->n = 0;
            func_800B31C0(E->mtx, &D_800F4F08[arg1]);
            func_8000ED10(E->buf[arg1], 0, 0xF00);
            if (E->sel != NULL && (E->sel->def->flags & 0x40)) {
                switch (E->sel->rot) {
                case 0:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        func_80097658(XB(w, 0) + 2000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(w, 0) + 14000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == k) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            func_80097658(XB(w, 0) + 2000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(h) + (k - 1) * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 2000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(h) + (k - 1) * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(h) + k * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == k || E->f16F8 == k - 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 1:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        func_80097658(XB(h, 0) + k * 4000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(h, 0) + k * 4000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(w) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == k) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            func_80097658(XB(h, 0) + (k - 1) * 4000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(w) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(w) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(w) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == k || E->f16F8 == k - 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        n = E->sel->def->dim[1] * 2 - k;
                        func_80097658(XB(w, 0) + 2000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(w, 0) + 14000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == n) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < E->sel->def->dim[1] * 2) {
                            func_80097658(XB(w, 0) + 2000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 2000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(h) + (k - 1) * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(h) + k * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(h) + (k - 1) * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == n || E->f16F8 == n + 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 3:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        n = E->sel->def->dim[1] * 2 - k;
                        func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(w) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == n) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < E->sel->def->dim[1] * 2) {
                            func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(w) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(w) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(w) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == n || E->f16F8 == n + 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                }
            }
            gdl = func_80097308(arg0, arg1, gdl);
        }
    } else {
        gdl = func_8008D728(arg1, gdl, 0, 0, 0, 0, 0, 0, -1, 1);
    }

    if (E->flags & 0x800) {
        switch (E->f1800) {
        case 0:
            gdl = func_8008F230(gdl, arg0, arg1, "No Track Found On Game Pak");
            break;
        case 1:
            gdl = func_8008F230(gdl, arg0, arg1, "Save Canceled");
            break;
        case 2:
            gdl = func_8008F230(gdl, arg0, arg1, "Track Complete!");
            break;
        case 3:
            gdl = func_8008F230(gdl, arg0, arg1, "You Ran Out Of Room!");
            break;
        case 4:
            gdl = func_8008F230(gdl, arg0, arg1, "Name Your Track Before Saving!");
            break;
        }
    } else if (E->flags & 0x1000) {
        switch (E->f17FC) {
        case 0:
            gdl = func_8008F458(gdl, arg0, arg1, "Add Segment?", "OK", "Cancel", 1, 1);
            break;
        case 1:
            gdl = func_8008F458(gdl, arg0, arg1, "Delete Will Reset The Piece", "OK", "Cancel", 1, 1);
            break;
        case 3:
        case 7:
            gdl = func_8008F458(gdl, arg0, arg1, "Lose Changes?", "OK", "Cancel", 1, 1);
            break;
        case 4:
            gdl = func_8008F458(gdl, arg0, arg1, "Really Clear Track?", "OK", "Cancel", 1, 1);
            break;
        case 5:
            gdl = func_8008F458(gdl, arg0, arg1, "Save The Track To...", "Game Pak", "Controller Pak", 0, 1);
            break;
        case 6:
            gdl = func_8008F458(gdl, arg0, arg1, "Load A Track From...", "Game Pak", "Controller Pak",
                                (D_800F1BC4 >> 3) & 1, 1);
            break;
        case 8:
            gdl = func_8008F458(gdl, arg0, arg1, "Overwrite Track On Game Pak?", "OK", "Cancel", 1, 1);
            break;
        }
    }
    return gdl;
}
