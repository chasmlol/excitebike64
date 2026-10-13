#include "ctx.h"

typedef struct Obj {
    s32 f0;
    struct Obj *next;   /* 0x04 */
    s32 f8;
    s16 x;              /* 0x0C */
    s16 y;              /* 0x0E */
    u16 flags;          /* 0x10 */
    s16 f12;
    s32 f14;
    s16 dir[2];         /* 0x18 */
    s16 len[2];         /* 0x1C */
    s16 w;              /* 0x20 */
    s16 h;              /* 0x22 */
} Obj;

typedef struct Size {
    s16 w;
    s16 h;
} Size;

typedef struct Game {
    u8 pad0[0x170];
    s32 f170;
    u8 pad174[0x17C - 0x174];
    Obj *list;          /* 0x17C */
    u8 pad180[0x16AC - 0x180];
    Size *map;          /* 0x16AC */
    u8 pad16B0[0x16F4 - 0x16B0];
    s32 f16F4;
    u8 pad16F8[0x1702 - 0x16F8];
    s16 f1702;
} Game;

typedef struct Ent {
    s16 dim[2];         /* 0x00 */
    s16 dir[2];         /* 0x04 */
    s16 off[2];         /* 0x08 */
    s16 flags;
    u8 pad[0x40 - 0xE];
} Ent;

typedef struct Slot {
    s32 w0;
    u32 flags;
    s16 n;
    s16 cur;
    u32 mask;
} Slot;

typedef struct Ctl {
    s32 w0;
    s16 count;
    s16 x;
    Slot *slots;
    s32 w3;
    s16 sel;
} Ctl;

typedef struct Grid {
    u8 g[30][30];
} Grid;

typedef struct Rect {
    s32 x0, y0, x1, y1;
} Rect;

extern Game *D_800CEA40;
extern Ent D_800CE280[];
extern Ctl D_800CEB9C;
extern Grid D_800F56B0;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_80090398(Ctl *c);

s32 func_8009B9A8(Obj *o)
{
    Grid grid;
    Rect r;
    s16 k;
    s16 other;
    s32 i, j;
    s32 n, m, lo, hi, best;
    s16 s;
    s32 ret;
    Obj *p;
    Ent *e;

    k = 1;
    if (o->next == NULL || (o->next->flags & 4)) {
        other = 0;
    } else {
        k = 0;
        other = 1;
    }
    switch (o->dir[k]) {
    case 0:
        {
            s32 t, u;
            t = o->x + o->len[k];
            r.x0 = t - 14;
            u = o->y + o->h;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 1:
        {
            s32 t, u;
            t = o->x + o->w;
            r.x0 = t - 15;
            u = o->y + o->len[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    case 2:
        {
            s32 t, u;
            t = o->x + o->len[k];
            r.x0 = t - 14;
            u = o->y;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 3:
        {
            s32 t, u;
            t = o->x;
            r.x0 = t - 15;
            u = o->y + o->len[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    }
    func_8000ED10((u8 *)&grid, 0, 0x384);
    for (j = 0; j < r.y1 - r.y0; j++) {
        for (i = 0; i < r.x1 - r.x0; i++) {
            if (i + r.x0 < 0 || i + r.x0 >= D_800CEA40->map->w
                || j + r.y0 < 0 || j + r.y0 >= D_800CEA40->map->h) {
                switch (o->dir[k]) {
                case 0:
                    grid.g[i][j] = 1;
                    break;
                case 2:
                    grid.g[29 - i][29 - j] = 1;
                    break;
                case 3:
                    grid.g[j][29 - i] = 1;
                    break;
                case 1:
                    grid.g[29 - j][i] = 1;
                    break;
                }
            }
        }
    }
    p = D_800CEA40->list;
    if (p != NULL) {
        do {
            if (!(p->flags & 4) && p->x < r.x1 && r.x0 < p->x + p->w
                && p->y < r.y1 && r.y0 < p->y + p->h) {
                switch (o->dir[k]) {
                case 0:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[i][j] = 1;
                            }
                        }
                    }
                    break;
                case 2:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - i][29 - j] = 1;
                            }
                        }
                    }
                    break;
                case 3:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[j][29 - i] = 1;
                            }
                        }
                    }
                    break;
                case 1:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - j][i] = 1;
                            }
                        }
                    }
                    break;
                }
            }
            p = p->next;
        } while (p != NULL && p != D_800CEA40->list);
    }
    D_800F56B0 = grid;

    for (n = 0; n < 29; n++) {
        e = &D_800CE280[n];
        if (!(e->flags & 8)) {
            continue;
        }
        best = 0;
        if (e->flags & 0x80) {
            lo = 1;
            hi = 8;
        } else {
            hi = e->dim[(e->dir[k] + 1) & 1];
            lo = hi;
        }
        for (m = lo; m <= hi; m++) {
            if ((u32)(n - 18) < 8 && n - 18 >= D_800CEA40->f16F4) {
                e->flags &= ~0x30;
            } else {
                switch (e->dir[other]) {
                case 0:
                    s = e->dim[0] - e->off[other] - 2;
                    break;
                case 3:
                    s = e->dim[1] - e->off[other] - 2;
                    break;
                case 1:
                    s = e->off[other];
                    break;
                case 2:
                    s = e->off[other];
                    break;
                }
                e->flags |= 0x10;
                for (j = 0; j < m; j++) {
                    for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                        if (grid.g[i + (14 - s)][j + 15]) {
                            e->flags &= ~0x10;
                            break;
                        }
                    }
                    if (!(e->flags & 0x10)) {
                        break;
                    }
                }
                if (e->flags & 0x100) {
                    switch (e->dir[k]) {
                    case 0:
                        s = e->dim[0] - e->off[k] - 2;
                        break;
                    case 3:
                        s = e->dim[1] - e->off[k] - 2;
                        break;
                    case 1:
                        s = e->off[k];
                        break;
                    case 2:
                        s = e->off[k];
                        break;
                    }
                    e->flags |= 0x20;
                    j = 0;
                    if (m > 0) {
                        do {
                            for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                                if (grid.g[i + (14 - s)][j + 15]) {
                                    e->flags &= ~0x20;
                                    break;
                                }
                            }
                            j++;
                        } while ((e->flags & 0x20) && j < m);
                    }
                } else {
                    e->flags &= ~0x20;
                }
            }
            if (e->flags & 0x10) {
                best = m;
            } else if (e->flags & 0x80) {
                if (m != lo) {
                    e->flags |= 0x10;
                }
                break;
            }
        }
        if (e->flags & 0x80) {
            if (D_800CEA40->f170 != 6) {
                e->dim[1] = (best < e->dim[1]) ? best : e->dim[1];
            }
            D_800CEA40->f1702 = best;
        }
    }

    ret = 0;
    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            j = 0;
            break;
        case 1:
            j = 2;
            break;
        case 2:
            j = 6;
            break;
        case 3:
            j = 0x12;
            break;
        default:
            continue;
        }
        D_800CEB9C.slots[i].mask = 0;
        for (n = 0; n < D_800CEB9C.slots[i].n; n++) {
            if (D_800CE280[j + n].flags & 0x30) {
                D_800CEB9C.slots[i].mask |= 1 << n;
            }
        }
        if (D_800CEB9C.slots[i].mask == 0) {
            D_800CEB9C.slots[i].flags &= ~0x10;
        } else {
            ret = 1;
            D_800CEB9C.slots[i].flags |= 0x10;
        }
    }
    func_80090398(&D_800CEB9C);
    return ret;
}
