#include "game.h"

typedef struct Ent_110 { struct Ent_110 *next; s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; u32 color; } Ent_110;

extern Ent_110 *sFontSprLists[];

typedef struct Hdr_16 {
    struct Hdr_16 *next;
    u8 pad4;
    u8 b5;
    u8 pad6;
    u8 b7;
    u8 pad8[8];
    u16 size;
} Hdr_16;

s32 font_load(ResFile *arc, u32 **ids, char *name)
{
    s32 *base;
    s32 *src;
    s32 *h;
    s32 *tbl;
    u32 k, j;

    if (name != NULL) {
        sFontPalette = name;
    } else {
        sFontPalette = sFontDefaultPalette;
    }
    k = 0x30000;
    if (gFontLoadBuffer != NULL) {
        base = gFontLoadBuffer;
    } else {
        base = mem_calloc_dbg(k, 0, D_800DB4D0, 0x124);
    }
    if (base == NULL) {
        sprintf(D_800EE380, D_800DB4DC);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop_2 = (s32 *)((u8 *)base + k - 0x1000);
    sFontPageCount = 0;
    tbl = mem_alloc_dbg(0x2800, 0, D_800DB4D0, 0x132);
    sFontPageMaps = tbl;
    for (k = 0; k < 40; k++) {
        *tbl++ = -1;
        for (j = 1; j < 64; j++) {
            *tbl++ = 0x80000000;
        }
    }
    sFontTable[0] = base;
    for (k = 1; k < 16; k++) {
        if (*ids == NULL) {
            break;
        }
        h = res_load(arc, **ids);
        if (h == NULL) {
            mem_free(sFontPageMaps);
            sFontPageMaps = NULL;
            mem_free(sFontTable[0]);
            sFontTable[0] = NULL;
            sprintf(D_800EE380, D_800DB504);
            fatal_halt_mem_report_2();
            return -1;
        }
        j = ((Hdr_16 *)h)->size;
        font_pack_glyphs(h);
        src = h;
        j += 0x10;
        j += 0xF;
        j >>= 2;
        j >>= 1;
        j >>= 1;
        j--;
        sFontTable[k] = base;
        while (j != -1) {
            base[0] = src[0];
            base[1] = src[1];
            base[2] = src[2];
            base[3] = src[3];
            base += 4;
            src += 4;
            j--;
        }
        res_unload(arc->tbl + **ids);
        ids++;
    }
    for (; k < 16; k++) {
        sFontTable[k] = NULL;
    }
    mem_free(sFontPageMaps);
    src = sFontPageTop_2 - sFontPageCount * 0x400;
    k = sFontPageCount << 8;
    k |= 0xFF;
    sFontPageMaps = NULL;
    while (k != -1) {
        base[0] = src[0];
        base[1] = src[1];
        base[2] = src[2];
        base[3] = src[3];
        base += 4;
        src += 4;
        k--;
    }
    k = (u8 *)base - (u8 *)sFontTable[0];
    if (gFontLoadBuffer != NULL) {
        base = mem_alloc_dbg(k, 0, D_800DB4D0, 0x1A0);
        if (base == NULL) {
            fatal_halt(1);
        }
        memcpy(base, gFontLoadBuffer, k);
    } else {
        base = mem_realloc_dbg(sFontTable[0], k, 0, D_800DB4D0, 0x1A9);
    }
    if (base == NULL) {
        sprintf(D_800EE380, D_800DB520);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop_2 = (s32 *)((u8 *)base + k - 0x1000);
    k = (u8 *)base - (u8 *)sFontTable[0];
    for (j = 0; j < 16; j++) {
        h = sFontTable[j];
        if (h == NULL) {
            break;
        }
        sFontTable[j] = (s32 *)((u8 *)h + k);
        if (((Hdr_16 *)h)->b5 == 0) {
            ((Hdr_16 *)h)->next = NULL;
        } else {
            ((Hdr_16 *)h)->next = (Hdr_16 *)((u8 *)h + (((Hdr_16 *)h)->b7 * 8 + 0x10));
        }
    }
    return 0;
}

typedef struct Glyph_3 { s8 a; s8 b; s8 x; s8 y; u8 w; u8 h; s8 adv; u8 page; } Glyph_3;
typedef struct Font_4 { u8 *kern; u8 pad4[2]; u8 first; u8 count; s8 spacing; s8 h9; u8 padA[2]; s8 hC; s8 hD; u8 padE[2]; Glyph_3 g[1]; } Font_4;

u8 *font_draw_text(s32 *p, s32 x, s32 y)
{
    u8 *s = (u8 *)(p + 8);
    u32 flags = p[2];
    s32 n;
    u32 color;
    Font_4 *f;
    Glyph_3 *g;
    Ent_110 *e;
    u32 c, i;
    s32 x0, scale, m8, m8b, adv;
    u8 *kp, *kq, *kern;

    color = p[1];
    n = color + color * 2;
    color = n + 1;
    if (flags & 0x20000000) {
        color = n;
    } else if (flags & 0x40000000) {
        color = n + 2;
    }
    x += p[4];
    y += p[5];
    color = sFontPalette_2[color];
    f = sFontTable[p[0]];
    x0 = x;
    if (sFontScaleY != 0x10000) {
        scale = 1;
        if (D_800D576C != 0)
            scale = sFontScaleY >> 16;
        c = *s;
        if (c != 0) {
            m8 = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8 != 0) {
                        kp = (u8 *)(sFontPageTop - (g->page << 12));
                        gFontDirectDrawFunc((s32)kp + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = sFontSprNext;
                        sFontSprNext = e + 1;
                        e->next = sFontSprLists[g->page];
                        sFontSprLists[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * scale;
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = sFontSprNext;
                            sFontSprNext = e + 1;
                            e->next = sFontSprLists[g->page];
                            sFontSprLists[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = y + (g->y + 1) * scale;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kp = f->kern;
                    if (kp != 0) {
                        do { do { kern = kp + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kp += *kern * 2;
                        i = *s;
                        while (*kp != 0) {
                            if (*kp == i) {
                                adv += (s8)kp[1];
                                break;
                            }
                            kp += 2;
                        }
                        }
                    }
                    do { x += (adv * sFontScaleX) >> 16; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    } else {
        c = *s;
        if (c != 0) {
            m8b = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8b != 0) {
                        kq = (u8 *)(sFontPageTop - (g->page << 12));
                        gFontDirectDrawFunc((s32)kq + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = sFontSprNext;
                        sFontSprNext = e + 1;
                        e->next = sFontSprLists[g->page];
                        sFontSprLists[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * (sFontScaleY >> 16);
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = sFontSprNext;
                            sFontSprNext = e + 1;
                            e->next = sFontSprLists[g->page];
                            sFontSprLists[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = (s8)((u8 *)g)[3] + y + 1;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kq = f->kern;
                    if (kq != 0) {
                        do { do { kern = kq + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kq += *kern * 2;
                        i = *s;
                        while (*kq != 0) {
                            if (*kq == i) {
                                adv += (s8)kq[1];
                                break;
                            }
                            kq += 2;
                        }
                        }
                    }
                    do { x += adv; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    }
    return s;
}

typedef struct {
    u8 g0;
    u8 g1;
    s8 xoff;
    u8 g3;
    u8 adv;
    u8 g5;
    s8 kadj;
    u8 g7;
} Glyph_4;

typedef struct {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    u8 p9[7];
    Glyph_4 gl[1];
} Font_5;

struct Obj_83 {
    s32 font;
    s32 w4;
    Glyph_4 * volatile cur;
    s32 wc;
    s32 off;
    s32 w14;
    s32 w18;
    s32 w1c;
    u8 text[1];
};

s32 font_get_char_pos(Obj_83 *p, s32 x, s32 unused, s32 n)
{
    Glyph_4 *g;
    u8 *t;
    Font_5 *f;
    u8 *kt;
    u8 *pr;
    s32 start;
    u32 idx;
    s32 c;
    s32 k;
    s32 ch;
    u8 adv;

    g = p->cur;
    t = p->text;
    goto setup;

ret:
    adv = g->adv;
    return ((x + g->xoff + (adv >> 1)) << 16) | adv;

setup:
    x = x + p->off;
    start = x;
    c = *t;
    f = sFontTable[p->font];
    if (c == 0) {
        goto fail;
    }
    do {
        idx = c - f->base;
        if (idx >= f->count) {
            goto fail;
        }
        t++;
        n--;
        g = &f->gl[idx];
        if (n == -1) {
            goto ret;
        }
        if (c == 10) {
            x = start;
        } else {
            x = x + (g->kadj + f->adj);
            kt = f->kern;
            if (kt != 0 && kt[idx] != 0) {
                kt = kt + kt[idx] * 2;
                idx = *t;
                while ((k = *kt) != 0) {
                    if (k == idx) {
                        x = x + (s8)kt[1];
                        break;
                    }
                    kt = kt + 2;
                }
            }
        }
        c = *t;
    } while (c != 0);
fail:
    return -1;
}

typedef struct {
    u8 g0[6];
    s8 kadj;
    u8 g7;
} Glyph_5;

struct Font_6 {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    s8 f9;
    u8 pa[2];
    s8 fc;
    s8 fd;
    s8 fe;
    u8 pf;
    Glyph_5 gl[1];
};

u8 *font_measure_text(u8 *s, Font_6 *f, u32 *outMax, s32 *outW)
{
    u8 *t = s;
    s32 w;
    s8 flag = 0;
    u32 mx = 0;
    u32 cur;
    u32 c;
    u32 lo;
    u32 hi;
    s32 adj;
    u8 *kt;
    Glyph_5 *gl;

    *outW = 0;
    *outMax = 0;
    w = f->fe;
    lo = f->base;
    hi = f->count;
    adj = f->adj;
    gl = f->gl;
    while (*t != 0) {
        cur = 0;
        c = *t;
        while (c != 0) {
            t++;
            if (c == 10) {
                flag = 1;
                w += f->fc + f->fd + f->f9;
                break;
            }
            if (c != 0x20) {
                flag = 0;
            }
            c -= lo;
            if ((s32)c < 0 || c >= hi) {
                break;
            }
            cur += gl[c].kadj + adj;
            kt = f->kern;
            if (kt != NULL && kt[c] != 0) {
                kt += kt[c] * 2;
                while (*kt != 0) {
                    if (*kt == *s) {
                        cur += (s8)kt[1];
                        break;
                    }
                    kt += 2;
                }
            }
            c = *t;
        }
        if (mx < cur) {
            mx = cur;
        }
    }
    if (mx != 0) {
        mx -= adj;
    }
    if (flag) {
        w -= f->fc + f->fd + f->f9;
    }
    *outMax = mx;
    *outW = w;
    return t;
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *font_render(Gfx *gdl)
{
    u32 i;

    gdl = font_gfx_setup(gdl);
    PKT(0xE2001E01, 1);
    PKT(0xE200001C, 0x504240);
    PKT(0xFC119623, 0xFF2FFFFF);
    if (sFontScaleX == 0x10000 && sFontScaleY == 0x10000) {
        gdl = font_render_pages(gdl, D_80105440);
        gdl = font_render_pages(gdl, sFontSprLists);
    } else {
        gdl = font_render_pages_scaled(gdl, D_80105440);
        gdl = font_render_pages_scaled(gdl, sFontSprLists);
    }
    PKT(0xE7000000, 0);
    sFontSprNext = sFontSprPool;
    PKT(0xE2001E01, 0);
    sFontSprPoolEnd = sFontSprPool + 0x3FE0;
    for (i = 0; i < 0x28; i++) {
        D_80105440[i] = 0;
        sFontSprLists[i] = 0;
    }
    return gdl;
}

Gfx *font_render_pages_scaled(Gfx *gdl, Spr **list)
{
    u32 i;
    Spr *s;
    s32 sy;

    for (i = 0; i < 40; i++) {
        if (list[i] != NULL) {
            gDPLoadTextureBlock(gdl++, sFontPageTop - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            sy = sFontScaleY;
            s = list[i];
            do {
                if ((u16)(s->x + 63) < 703) {
                    gDPSetPrimColor(gdl++, 0, 0, s->r, s->g, s->b, s->a);
                    if (D_800D576C == 0) {
                        gSPTextureRectangle(gdl++,
                            (s->x + (((0x10000 - sFontScaleX) * s->w) >> 17)) << 2,
                            (s->y + (((0x10000 - sy) * s->h) >> 17)) << 2,
                            (s->x + (((sFontScaleX + 0x10000) * s->w) >> 17)) << 2,
                            (s->y + (((sy + 0x10000) * s->h) >> 17)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, sFontTexStepX, sFontTexStepY);
                    } else {
                        gSPTextureRectangle(gdl++,
                            s->x << 2,
                            s->y << 2,
                            (s->x + ((sFontScaleX * s->w) >> 16)) << 2,
                            (s->y + ((sy * s->h) >> 16)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, sFontTexStepX, sFontTexStepY);
                    }
                }
                s = s->next;
            } while (s != NULL);
        }
    }
    return gdl;
}

#define gDPWord_2(pkt, a, b)     {         Gfx *_g = (Gfx *)(pkt);         _g->words.w0 = (a);         _g->words.w1 = (b);     }

Gfx *font_render_pages(Gfx *gdl, Spr **lists)
{
    s32 base;
    u32 i;
    Spr *n;

    base = sFontPageTop;
    for (i = 0; i < 40; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, base - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            n = lists[i];
            do {
                if (n->x >= -63 && n->x < 640) {
                    gDPSetPrimColor(gdl++, 0, 0, n->r, n->g, n->b, n->a);
                    { s32 x = n->x;
                    gDPWord_2(gdl++, _SHIFTL(0xE4, 24, 8) | _SHIFTL((n->w + x) << 2, 12, 12) | _SHIFTL((n->h + n->y) << 2, 0, 12),
                            _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(n->x << 2, 12, 12) | _SHIFTL(n->y << 2, 0, 12)); }
                    gDPWord_2(gdl++, _SHIFTL(0xE1, 24, 8), _SHIFTL(n->s << 5, 16, 16) | _SHIFTL(n->t << 5, 0, 16));
                    gDPWord_2(gdl++, _SHIFTL(0xF1, 24, 8), _SHIFTL(1 << 10, 16, 16) | _SHIFTL(1 << 10, 0, 16));
                }
                n = n->next;
            } while (n != NULL);
        }
    }
    return gdl;
}

s32 font_pack_glyphs(u8 *hdr)
{
    u8 *e;
    u32 i;
    u32 j;
    u32 h, w;
    u8 *src;
    u8 *dst;
    u8 *tv;

    e = hdr + 0x10;
    for (i = 0; i < hdr[7]; i++) {
        {
            j = e[5] >> 8;
            sFontAllocW = e[4] + 1;
            sFontAllocH = e[5] + 1;
            while (j < 0x28) {
                if (font_alloc_glyph_space(sFontPageMaps_2 + (j << 8)) != 0) break;
                j++;
            }
            if (j == 0x28) {
                return -1;
            }
            if (sFontPageCount < j) {
                sFontPageCount = j;
            }
            h = e[5];
            w = e[4];
            src = e + *(u16 *)e;
            dst = (u8 *)(sFontPageTop - (j << 12));
            dst = dst + sFontAllocX + sFontAllocY * 0x40;
            while (--h != (u32)-1) {
                while (--w != (u32)-1) {
                    *dst++ = *src++;
                }
                w = e[4];
                tv = dst + 0x40;
                dst = tv - w;
            }
            e[7] = j;
            e[0] = sFontAllocX;
            e[1] = sFontAllocY;
            e += 8;
        }
    }
    return 0;
}

s32 font_alloc_glyph_space(u32 *map)
{
    u32 w = (sFontAllocW_2 + 1) >> 1;
    s32 h = sFontAllocH;
    s32 y = 1;
    s32 xmax = 0x20 - w;
    s32 ymax = 0x40 - h;
    u32 mask = sFontAllocMasks[w - 1];
    u32 bits0 = sFontAllocBits[w - 1];
    u32 acc;
    u32 bits;
    u32 *pp;
    s32 i;
    s32 x;

    if (y < ymax) {
        do {
            acc = 0;
            pp = map + h;
            for (i = h; i != 0; i--) {
                pp--;
                acc |= *pp;
                if ((acc & mask) == mask) {
                    break;
                }
            }
            if (i != 0) {
                y += i;
                map += i;
                continue;
            }
            acc = ~acc;
            bits = bits0;
            x = 0;
            if (xmax > 0) {
                do {
                    if ((acc & bits) == bits) {
                        goto found;
                    }
                    x++;
                    bits >>= 1;
                } while (x < xmax);
            }
            y++;
            map++;
        } while (y < ymax);
    }
    return 0;

found:
    while (--h != -1) {
        *map++ |= bits;
    }
    sFontAllocX = x << 1;
    sFontAllocY = y;
    return 1;
}

void font_set_scale(s32 a, s32 b, s32 c, s32 d)
{
    sFontScaleX = a;
    sFontScaleY = b;
    D_800D5768 = c;
    D_800D576C = d;
    sFontTexStepX = 0x4000000 / a;
    sFontTexStepY = 0x4000000 / b;
}

u32 font_unload(void) {
    u32 i;
    if (sFontPageMaps != 0) {
        mem_free(sFontPageMaps);
        sFontPageMaps = 0;
    }
    if (sFontTable[0] != 0) {
        mem_free(sFontTable[0]);
        sFontTable[0] = 0;
    }
    for (i = 1; i < 16; i++) {
        sFontTable[i] = 0;
    }
    return 0;
}

s32 *font_text_create(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5)
{
    s32 t;

    if (param_2 == 0) {
        param_2 = 0x40;
    }
    t = (param_2 + 0xF) & -0x10;
    if (param_1 == 0) {
        param_1 = mem_calloc_dbg(t + 0x20, 4, (s32)D_800DB4D0, 0x22A);
        if (param_1 == 0) {
            return 0;
        }
    }
    if (param_3 != 0) {
        param_1[0] = param_3;
    }
    if (param_4 != 0) {
        param_1[1] = param_4;
    }
    if (param_5 != 0) {
        param_1[2] = param_5;
    }
    font_text_clear((u8 *)param_1);
    return param_1;
}

u8 *font_text_clear(u8 *p)
{
    if (p != 0) {
        p[0x20] = 0;
        *(s32 *)(p + 0x10) = 0;
        *(s32 *)(p + 0x14) = 0;
        *(s32 *)(p + 0x18) = 0;
        *(s32 *)(p + 0x1C) = 0;
    }
    return p;
}

typedef struct { u8 pad[0xE]; s8 asc; } Font_7;

struct Text {
    s32 font;
    s32 f4;
    u32 pad8 : 24;
    u32 valign : 4;
    u32 halign : 4;
    s32 fC;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    char text[1];
};

char *font_text_add(Text *p, char *s)
{
    Font_7 *font;

    if (p != NULL) {
        str_cat(p->text, s);
        font = sFontTable[p->font];
        font_measure_text(p->text, font, &p->w, &p->h);
        if (p->halign == 1) {
            p->x = -((p->w + 1) / 2);
        } else if (p->halign == 2) {
            p->x = -p->w;
        } else {
            p->x = 0;
        }
        if (p->valign == 1) {
            p->y = 0;
        } else if (p->valign == 0) {
            p->y = font->asc - 1;
        } else if (p->valign == 2) {
            p->y = font->asc - 1 - (p->h + 1) / 2;
        } else if (p->valign == 3) {
            p->y = font->asc - 1 - p->h;
        }
    }
    return s;
}

void func_800C204C(s32 arg0)
{
}

void str_make_initials(char *dst, char *src)
{
    do {
        *dst++ = *src++;
        *dst++ = '.';
        *dst++ = ' ';
        while (*src != 0) {
            if (*src == ' ' || *src == '.') {
                break;
            }
            src++;
        }
        while (*src != 0) {
            if (*src != ' ' && *src != '.') {
                break;
            }
            src++;
        }
    } while (*src != 0);
    dst[-1] = 0;
}

void func_800C20F0(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x61) < 0x1A) {
            *s = c - 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

void font_name_upper(u8 *s) {
    u8 *p = s;
    u8 c;

    if (s[0] != 0) {
        do {
            c = *p;
            if ((u32)(c - 0x61) < 0x1A) {
                *p = c - 0x20;
            }
            p++;
        } while (*p != 0);
    }
    if (s[0] == 0x4D) {
        if (s[1] == 0x43) {
            s[1] = 0x63;
        }
    }
    if (s[0] == 0x44 && s[1] == 0x45 && s[2] == 0x43 && s[3] == 0x4C) {
        s[1] = 0x65;
    }
}

void func_800C21BC(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x41) < 0x1A) {
            *s = c + 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

void func_800C21F0(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x61) < 0x1A) {
            *s = c + 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

s32 font_reset_queue(void) {
    u32 i = 0;
    s32 *a = sFontSprLists;
    s32 *b = D_80105440;
    sFontSprNext = sFontSprPool;
    sFontSprPoolEnd = sFontSprPool + 0x3FE0;
    do {
        *b = 0;
        *a = 0;
        a++;
        i++;
        b++;
    } while (i < 0x28);
    return 0;
}
