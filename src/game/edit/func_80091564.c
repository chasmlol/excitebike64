#include "ctx.h"

typedef struct { u8 c[11]; } Name11;
typedef struct { u8 pad[0xC]; u32 flags; } Obj;
typedef struct {
    u8 pad0[0x170];
    s32 f170;
    u8 pad174[0x16BA - 0x174];
    Name11 name;
    u8 buf[11];
    s32 sel;
    s32 x;
    s32 y;
    s32 len;
    Obj *obj;
} Game;

extern u32 D_800E30B0[];
extern u32 D_800E25B0;
extern Game *D_800CEA40;
extern u8 *D_800CD1AC[];

void func_8001102C(s32 param_1);
s32 func_800AEC10(u8 *p);

#define PRESSED(b) ((D_800E30B0[0] & (b)) && !(D_800E25B0 & (b)))

void func_80091564(void)
{
    s32 i;
    s32 old;
    s32 lo, hi, lo2, hi2;

    for (i = 0; i < 4; i++) {
        if ((((s32)D_800E30B0[i] >> 24) & 0xF) < 6) {
            D_800E30B0[i] &= ~0xC;
        }
        if ((s32)(D_800E30B0[i] >> 28) < 6) {
            D_800E30B0[i] &= ~3;
        }
    }

    if (PRESSED(0x10)) {
        if (D_800CEA40->sel == 0x26 && D_800CEA40->len == 0) {
            func_8001102C(3);
        } else {
            func_8001102C(1);
        }
        {
            Game *g = D_800CEA40;
            if (g->sel == 0x26) {
                s32 n = g->len;
                if (n != 0) {
                    g->buf[n] = 0;
                    g->len = n - 1;
                }
            } else if (g->sel == 0x27) {
                g->name = *(Name11 *)g->buf;
                D_800CEA40->f170 = 0;
                D_800CEA40->obj->flags |= 1;
            } else {
                g->len++;
                if (g->len >= 10) {
                    g->x = 799;
                    g->y = 799;
                    g->sel = 0x27;
                }
            }
        }
        return;
    }
    if (PRESSED(0x20)) {
        if (D_800CEA40->len == 0) {
            func_8001102C(3);
            return;
        }
        func_8001102C(1);
        D_800CEA40->buf[D_800CEA40->len - 1] = D_800CEA40->buf[D_800CEA40->len];
        {
            Game *g = D_800CEA40;
            s32 n = g->len;
            g->buf[n--] = 0;
            g->len = n;
        }
        return;
    }

    {
        s32 y;
        y = (s32)D_800CEA40;
        old = ((Game *)y)->sel;
        if (((Game *)y)->len < 10) {
            Game *g;
            u32 v;
            s32 x, nx;
            lo = 0;
            lo2 = lo;
            hi = 800;
            v = D_800E30B0[0];
            g = (Game *)y;
            hi2 = hi;
            x = g->x;
            if (v & 0xC) {
                if (v & 8) {
                    nx = x + (((s32)v >> 24) & 0xF);
                } else {
                    nx = x - (((s32)v >> 24) & 0xF);
                }
            } else {
                nx = x;
            }
            g->x = nx;
            if (PRESSED(0x2000)) {
                D_800CEA40->x -= 100;
            }
            if (PRESSED(0x4000)) {
                D_800CEA40->x += 100;
            }
            {
                Game *h = D_800CEA40;
                s32 c = h->x;
                if (c >= hi) {
                    h->x = lo;
                } else if (c < lo) {
                    h->x = hi - 1;
                }
            }
            {
                Game *h = D_800CEA40;
                u32 v2 = D_800E30B0[0];
                s32 ny;
                y = h->y;
                if (v2 & 3) {
                    if (v2 & 1) {
                        ny = y - ((s32)((v2 >> 28) * 3) >> 1);
                    } else {
                        ny = y - (-(s32)(v2 >> 28) * 3) / 2;
                    }
                } else {
                    ny = y;
                }
                h->y = ny;
            }
            if (PRESSED(0x800)) {
                D_800CEA40->y -= 0xA0;
            }
            if (PRESSED(0x1000)) {
                D_800CEA40->y += 0xA0;
            }
            {
                Game *h = D_800CEA40;
                s32 c = h->y;
                if (c >= hi2) {
                    h->y = lo2;
                } else if (c < lo2) {
                    h->y = hi2 - 1;
                }
            }
            D_800CEA40->sel = D_800CEA40->x / 100 + (D_800CEA40->y / 0xA0) * 8;
        } else if (PRESSED(4) || PRESSED(0x2000) || PRESSED(8) || PRESSED(0x4000)) {
            Game *h = D_800CEA40;
            if (h->sel == 0x27) {
                h->x = 600;
                h->y = 799;
                h->sel = 0x26;
            } else {
                h->x = 799;
                h->y = 799;
                h->sel = 0x27;
            }
        }
    }

    if (D_800CEA40->sel < 0x26) {
        if (func_800AEC10(D_800CEA40->buf) == 0) {
            D_800CEA40->buf[1] = 0;
        }
        D_800CEA40->buf[D_800CEA40->len] = *D_800CD1AC[D_800CEA40->sel];
    } else {
        D_800CEA40->buf[D_800CEA40->len] = 0;
    }
    if (old != D_800CEA40->sel) {
        func_8001102C(0);
    }
}
