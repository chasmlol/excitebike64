#include "game.h"

typedef struct { s32 v; } S1;
typedef struct { u8 pad[0x2C]; s32 f2c; } Obj_26;

extern Obj_26 *D_800EFD64_24;

void func_80033E30(s32 arg0)
{
    s32 i, x, y, m;
    Rect *r;
    Rect *q;
    S1 *p;

    if ((D_800EFD70 & 0x10000) == 0) {
        return;
    }
    m = D_800C2594 - 1;
    for (i = 0; i < D_800C2594; i++) {
        if (D_800EFD64_24->f2c == 0) {
            D_800E8EE0[i] = ((Rect *)(D_800E1718_3 + (m * 64 + i * 16)))->h;
        } else {
            D_800E8EE0[i] = 0;
            y = ((Rect *)(D_800E1718_3 + (m * 64 + i * 16)))->y;
            r = (Rect *)(D_800E1718_3 + (m * 64 + i * 16));
            if (y < r->y + r->h) {
            yl:
                p = (S1 *)&D_800E8EE0[i];
                for (x = r->x; x < r->x + r->w; x += 8) {
                    if (*(u16 *)((u32)D_800C29E0 + ((y * D_800F13EC + x) << 1)) > 0xFFEF) {
                        p->v = y - r->y;
                    }
                }
                y += 16;
                if (y < r->y + r->h) goto yl;
            }
            D_800E8EE0[i] += 16;
            if (D_800C98B4[i] > 16 && D_800E8EE0[i] < D_800C98B4[i] - 16) {
                D_800E8EE0[i] = D_800C98B4[i] - 16;
            }
            q = (Rect *)(D_800E1718_3 + (m * 64 + i * 16));
            if (D_800E8EE0[i] > q->h) {
                D_800E8EE0[i] = q->h;
            }
            if (D_800E8EE0[i] < q->h / 2) {
                D_800E8EE0[i] = q->h;
            }
        }
        D_800C98B4[i] = D_800E8EE0[i];
    }
}

void func_80033FF0(void)
{
    s32 i;
    s32 t;
    s32 v;
    s32 n;

    if (D_800DC144_4 != 0) {
        t = D_800DC144_4[9];
        if (t == 0) {
            t = 0x100;
        }
        i = 0;
        if (i < D_800C2594) {
        n = D_800C2594;
        do {
            D_800E8EC0[i] = 0x80;
            D_800E8ED0[i] = 0x68;
            v = D_800E8000[i] & 0x1FFF;
            if (D_800C2364 != 0) {
                v = 0x2000 - v;
            }
            D_800E8EA0[i] = (v / 32 - t) & 0xFF;
            v = D_800E7978[i];
            v = v / 32;
            v += 0x80;
            if (D_800EFD64 == D_800CC070) {
                v -= 0x50;
            }
            D_800E8EB0[i] = v;
            i++;
        } while (i < n);
        }
    }
}

static inline Gfx *drawTex(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                           s32 x0, s32 y0, s32 x1, s32 y1, s32 *flag)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w > 0 && h > 0) {
        if (*flag != 0) {
            gDPLoadTextureBlock(g++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
        return g;
    }
    return gdl;
}

static inline Gfx *drawTexA(Gfx *g, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, Gfx *gdl, s32 du, s32 dv,
                           s32 x0, s32 y0, s32 x1, s32 y1, s32 *flag)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w <= 0 || h <= 0) {
        return gdl;
    }
    {
        if (*flag != 0) {
            gDPLoadTextureBlock(g++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = g++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
    }
    gdl = g;
    return gdl;
}

Gfx *func_80034100(s32 a, s32 b, s32 *tex, Gfx *gdl)
{
    s32 rw, rh, xoff, yoff, tw, th, x0, y0;
    s32 y;
    Gfx *p;
    Gfx *q;

    rh = D_800E1718[D_800C2594 - 1][b].h;
    rw = D_800E1718[D_800C2594 - 1][b].w;
    tw = (rw << 7) / D_800E8EC0[b];
    th = (rh * 4) / D_800E8ED0[b];
    yoff = (a - D_800E8EB0[b]) * (rh / 2) / D_800E8ED0[b];
    xoff = D_800E8EA0[b] * (rw / 2) / D_800E8EC0[b];
    yoff += rh / 2;
    x0 = D_800E1718[D_800C2594 - 1][b].x;
    y0 = D_800E1718[D_800C2594 - 1][b].y;
    q = drawTex(gdl, x0 + xoff - tw, y0 + yoff, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + D_800E8EE0[b] - 1, tex);
    gdl = drawTex(q, x0 + xoff, y0 + yoff, tw, th + 1, 0, 0, q, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw, y0 + D_800E8EE0[b] - 1, tex);
    if (a == 0) {
        y = y0 + yoff;
        while (y > 0) {
            y -= th;
        }
        for (; y < y0 + yoff; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, (p = gdl = 0, tex));
            gdl = p;
            gdl = drawTex(gdl, x0 + xoff, y, tw, th + 1, 0, 0, gdl, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, tex);
        }
    }
    if (a == 0xF8) {
        for (y = y0 + yoff + th; y < y0 + D_800E8EE0[b]; y += th) {
            p = gdl;
            p = drawTexA(p, x0 + xoff - tw, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, (p = gdl = 0, tex));
            gdl = drawTex(p, x0 + xoff, y, tw, th + 1, 0, 0, p, 0x40000 / tw, 0x2000 / th, x0, y0, x0 + rw - 1, y0 + D_800E8EE0[b] - 1, tex);
        }
    }
    return gdl;
}

Gfx *func_80035178(Gfx *gdl, s32 unused, u8 *s)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *q0, *q1, *q2;
    s32 n;
    s32 i, j;
    s32 local;

    n = s[9];
    if (n == 0) {
        n = 0x100;
    }
    g0 = gdl++; ((u32 *)g0)[0] = 0xFD100000; ((u32 *)g0)[1] = *(u32 *)(s + 4);
    g1 = gdl++; ((u32 *)g1)[0] = 0xE8000000; ((u32 *)g1)[1] = 0;
    g2 = gdl++; ((u32 *)g2)[0] = 0xF5000100; ((u32 *)g2)[1] = 0x07000000;
    g3 = gdl++; ((u32 *)g3)[0] = 0xE6000000; ((u32 *)g3)[1] = 0;
    g4 = gdl++; ((u32 *)g4)[0] = 0xF0000000; ((u32 *)g4)[1] = 0x073FC000;
    g5 = gdl++; ((u32 *)g5)[0] = 0xE7000000; ((u32 *)g5)[1] = 0;
    g6 = gdl++; ((u32 *)g6)[0] = 0xE3000C00; ((u32 *)g6)[1] = 0;
    g7 = gdl++; ((u32 *)g7)[0] = 0xE3001201; ((u32 *)g7)[1] = 0;
    g8 = gdl++; ((u32 *)g8)[0] = 0xE3001801; ((u32 *)g8)[1] = 0x80;

    if (D_800C2384 == 0) {
        for (i = 0; i < n; i += 8) {
            local = *(s32 *)(s + 0xC) + i * 0x100;
            for (j = 0; j < D_800C2594; j++) {
                gdl = func_80034100(i, j, &local, gdl);
            }
        }
    } else {
        D_800E8EE4 = D_800E1774;
        for (j = 0; j < D_800C2594; j++) {
            for (i = 0; i < n; i += 8) {
                local = *(s32 *)(s + 0xC) + i * 0x100;
                gdl = func_80034100(i, j, &local, gdl);
            }
        }
    }

    q0 = gdl++; ((u32 *)q0)[0] = 0xE3001201; ((u32 *)q0)[1] = 0x2000;
    q1 = gdl++; ((u32 *)q1)[0] = 0xE3000C00; ((u32 *)q1)[1] = 0x00080000;
    q2 = gdl++; ((u32 *)q2)[0] = 0xE3001801; ((u32 *)q2)[1] = 0;
    return gdl;
}

typedef struct { s32 f0, f4, f8, fc; } G1718;

extern G1718 D_800E1718_9;

s32 func_800353EC(s32 *outx, s32 *outy, s32 p3, s32 p4)
{
    s32 x, y, z, w;
    s32 t;
    s32 m;

    if (D_800C98B0 == 1) {
        t = (D_800E1718_9.f8 << 7) / D_800E8EC0[p4];
        x = (s16)(D_800E8002 - 0x7AEC);
        m = x * t;
        t = (D_800E1718_9.fc << 2) / D_800E8ED0[p4];
        x = m / 8192;
        y = D_800E7978[p4];
        y = 45 - y * t / 1024;
        y = 0xB9 - D_800E8EB0[p4];
    } else {
        if (D_800EFD64_7 - D_800CB210_3 == 1) {
            if (func_80033AC8(300, 0x14, 50, &x, &y, &z, &w, p3, p4)) {
                return 1;
            }
        } else {
            if (func_80033AC8(0, 0x14, -300, &x, &y, &z, &w, p3, p4)) {
                return 1;
            }
        }
    }
    if (x < -50 || y < -50 || D_800F13EC < x || D_800F13F0 + 50 < y) {
        return 1;
    }
    *outx = x;
    *outy = y;
    return 0;
}

void func_80035630(void)
{
}

void func_80035638(void) {
    func_8000ED10(D_800E8EE0, 0, 0x10);
}

Gfx *func_80035660(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv,
                   s32 x0, s32 y0, s32 x1, s32 y1, u32 *flag)
{
    if (x1 < x + w) {
        w = x1 - x;
    }
    if (y1 < y + h) {
        h = y1 - y;
    }
    if (x < x0) {
        w += x - x0;
        u += (du * (x0 - x)) >> 5;
        x = x0;
    }
    if (y < y0) {
        h += y - y0;
        v += (dv * (y0 - y)) >> 5;
        y = y0;
    }
    if (w > 0 && h > 0) {
        if (*flag != 0) {
            gDPLoadTextureBlock(gdl++, *flag, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            *flag = 0;
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((x + w) << 2, 12, 12) | _SHIFTL((y + h) << 2, 0, 12);
            _g->words.w1 = _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(x << 2, 12, 12) | _SHIFTL(y << 2, 0, 12);
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = 0xE1000000;
            _g->words.w1 = _SHIFTL(u, 16, 16) | _SHIFTL(v, 0, 16);
        }
        {
            Gfx *_g = gdl++;
            _g->words.w0 = 0xF1000000;
            _g->words.w1 = _SHIFTL(du, 16, 16) | _SHIFTL(dv, 0, 16);
        }
        return gdl;
    }
    return gdl;
}
