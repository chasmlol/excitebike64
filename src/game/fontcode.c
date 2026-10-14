#include "game.h"

s32 font_load(ResFile *arc, u32 **ids, u32 *palette)
{
    s32 *base;
    s32 *src;
    Font *h;
    u32 *tbl;
    u32 k, j;

    if (palette != NULL) {
        sFontPalette = palette;
    } else {
        sFontPalette = sFontDefaultPalette;
    }
    k = 0x30000;
    if (gFontLoadBuffer != NULL) {
        base = (s32 *)gFontLoadBuffer;
    } else {
        base = mem_calloc_dbg(k, 0, sStrFontcodeFile, 0x124);
    }
    if (base == NULL) {
        sprintf(sFontErrorText, sStrFontAllocFailed);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop = (u8 *)base + k - 0x1000;
    sFontPageCount = 0;
    tbl = mem_alloc_dbg(0x2800, 0, sStrFontcodeFile, 0x132);
    sFontPageMaps = tbl;
    for (k = 0; k < 40; k++) {
        *tbl++ = -1;
        for (j = 1; j < 64; j++) {
            *tbl++ = 0x80000000;
        }
    }
    sFontTable[0] = (Font *)base;
    for (k = 1; k < 16; k++) {
        if (*ids == NULL) {
            break;
        }
        h = (Font *)res_load(arc, **ids);
        if (h == NULL) {
            mem_free(sFontPageMaps);
            sFontPageMaps = NULL;
            mem_free(sFontTable[0]);
            sFontTable[0] = NULL;
            sprintf(sFontErrorText, sStrFontLoadFailed);
            fatal_halt_mem_report_2();
            return -1;
        }
        j = *(u16 *)h->glyphs; /* until packed: glyph 0's bitmap offset, the size of the tables */
        font_pack_glyphs(h);
        src = (s32 *)h;
        j += 0x10;
        j += 0xF;
        j >>= 2;
        j >>= 1;
        j >>= 1;
        j--;
        sFontTable[k] = (Font *)base;
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
    src = (s32 *)sFontPageTop - sFontPageCount * 0x400;
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
        base = mem_alloc_dbg(k, 0, sStrFontcodeFile, 0x1A0);
        if (base == NULL) {
            fatal_halt(1);
        }
        memcpy(base, (void *)gFontLoadBuffer, k);
    } else {
        base = mem_realloc_dbg((s32 *)sFontTable[0], k, 0, sStrFontcodeFile, 0x1A9);
    }
    if (base == NULL) {
        sprintf(sFontErrorText, sStrFontShrinkFailed);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop = (u8 *)base + k - 0x1000;
    k = (u8 *)base - (u8 *)sFontTable[0];
    for (j = 0; j < 16; j++) {
        h = sFontTable[j];
        if (h == NULL) {
            break;
        }
        sFontTable[j] = (Font *)((u8 *)h + k);
        if (h->hasKerning == 0) {
            h->kerning = NULL;
        } else {
            h->kerning = (u8 *)h + (h->numGlyphs * 8 + 0x10);
        }
    }
    return 0;
}

u8 *font_draw_text(TextObj *t, s32 x, s32 y)
{
    u8 *s = (u8 *)t->text;
    u32 flags = t->fmt.flags;
    s32 n;
    u32 color;
    Font *f;
    FontGlyph *g;
    FontSprite *e;
    u32 c, i;
    s32 x0, scale, m8, m8b, adv;
    u8 *kp, *kq, *kern;

    color = t->color;
    n = color + color * 2;
    color = n + 1;
    if (flags & TEXT_GREYED) {
        color = n;
    } else if (flags & TEXT_HIGHLIGHT) {
        color = n + 2;
    }
    x += t->x;
    y += t->y;
    color = sFontPalette[color];
    f = sFontTable[t->font];
    x0 = x;
    if (sFontScaleY != 0x10000) {
        scale = 1;
        if (sFontScaleFromCorner != 0)
            scale = sFontScaleY >> 16;
        c = *s;
        if (c != 0) {
            m8 = flags & TEXT_DIRECT;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->lineAscent + f->lineDescent + f->lineGap;
                } else {
                    i = c - f->firstChar;
                    if (i >= f->numGlyphs)
                        return s;
                    g = &f->glyphs[i];
                    if (c - 0x80 < 0x10) {
                        sFontSpecialGlyphPos[c - 0x80][0] = x + g->x;
                        sFontSpecialGlyphPos[c - 0x80][1] = y + g->y;
                    }
                    if (m8 != 0) {
                        kp = sFontPageTop - (g->page << 12);
                        gFontDirectDrawFunc((s32)kp + (g->s + (g->t << 6)),
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
                        e->s = g->s;
                        e->t = g->t;
                        e->color.rgba = color;
                        if (flags & TEXT_SHADOW) {
                            e = sFontSprNext;
                            sFontSprNext = e + 1;
                            e->next = sFontSprLists[g->page];
                            sFontSprLists[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = y + (g->y + 1) * scale;
                            e->w = g->w;
                            e->h = g->h;
                            e->s = g->s;
                            e->t = g->t;
                            e->color.rgba = color & 0xFF;
                        }
                    }
                    adv = g->advance + f->spacing;
                    kp = f->kerning;
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
            m8b = flags & TEXT_DIRECT;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->lineAscent + f->lineDescent + f->lineGap;
                } else {
                    i = c - f->firstChar;
                    if (i >= f->numGlyphs)
                        return s;
                    g = &f->glyphs[i];
                    if (c - 0x80 < 0x10) {
                        sFontSpecialGlyphPos[c - 0x80][0] = x + g->x;
                        sFontSpecialGlyphPos[c - 0x80][1] = y + g->y;
                    }
                    if (m8b != 0) {
                        kq = sFontPageTop - (g->page << 12);
                        gFontDirectDrawFunc((s32)kq + (g->s + (g->t << 6)),
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
                        e->s = g->s;
                        e->t = g->t;
                        e->color.rgba = color;
                        if (flags & TEXT_SHADOW) {
                            e = sFontSprNext;
                            sFontSprNext = e + 1;
                            e->next = sFontSprLists[g->page];
                            sFontSprLists[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = (s8)((u8 *)g)[3] + y + 1;
                            e->w = g->w;
                            e->h = g->h;
                            e->s = g->s;
                            e->t = g->t;
                            e->color.rgba = color & 0xFF;
                        }
                    }
                    adv = g->advance + f->spacing;
                    kq = f->kerning;
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

s32 font_get_char_pos(TextObj *p, s32 x, s32 unused, s32 n)
{
    FontGlyph *g;
    u8 *t;
    Font *f;
    u8 *kt;
    u8 *pr;
    s32 start;
    u32 idx;
    s32 c;
    s32 k;
    s32 ch;
    u8 adv;

    g = *(FontGlyph *volatile *)&p->fmt.flags; /* a load of the flags the result never uses */
    t = (u8 *)p->text;
    goto setup;

ret:
    adv = g->w;
    return ((x + g->x + (adv >> 1)) << 16) | adv;

setup:
    x = x + p->x;
    start = x;
    c = *t;
    f = sFontTable[p->font];
    if (c == 0) {
        goto fail;
    }
    do {
        idx = c - f->firstChar;
        if (idx >= f->numGlyphs) {
            goto fail;
        }
        t++;
        n--;
        g = &f->glyphs[idx];
        if (n == -1) {
            goto ret;
        }
        if (c == 10) {
            x = start;
        } else {
            x = x + (g->advance + f->spacing);
            kt = f->kerning;
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

u8 *font_measure_text(u8 *s, Font *f, u32 *outMax, u32 *outW)
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
    FontGlyph *gl;

    *outW = 0;
    *outMax = 0;
    w = f->ascent;
    lo = f->firstChar;
    hi = f->numGlyphs;
    adj = f->spacing;
    gl = f->glyphs;
    while (*t != 0) {
        cur = 0;
        c = *t;
        while (c != 0) {
            t++;
            if (c == 10) {
                flag = 1;
                w += f->lineAscent + f->lineDescent + f->lineGap;
                break;
            }
            if (c != 0x20) {
                flag = 0;
            }
            c -= lo;
            if ((s32)c < 0 || c >= hi) {
                break;
            }
            cur += gl[c].advance + adj;
            kt = f->kerning;
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
        w -= f->lineAscent + f->lineDescent + f->lineGap;
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
        gdl = font_render_pages(gdl, sFontSprListsBack);
        gdl = font_render_pages(gdl, sFontSprLists);
    } else {
        gdl = font_render_pages_scaled(gdl, sFontSprListsBack);
        gdl = font_render_pages_scaled(gdl, sFontSprLists);
    }
    PKT(0xE7000000, 0);
    sFontSprNext = sFontSprPool;
    PKT(0xE2001E01, 0);
    sFontSprPoolEnd = sFontSprPool + 0x3FE;
    for (i = 0; i < 0x28; i++) {
        sFontSprListsBack[i] = NULL;
        sFontSprLists[i] = NULL;
    }
    return gdl;
}

Gfx *font_render_pages_scaled(Gfx *gdl, FontSprite **list)
{
    u32 i;
    FontSprite *s;
    s32 sy;

    for (i = 0; i < 40; i++) {
        if (list[i] != NULL) {
            gDPLoadTextureBlock(gdl++, sFontPageTop - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            sy = sFontScaleY;
            s = list[i];
            do {
                if ((u16)(s->x + 63) < 703) {
                    gDPSetPrimColor(gdl++, 0, 0, s->color.c.r, s->color.c.g, s->color.c.b, s->color.c.a);
                    if (sFontScaleFromCorner == 0) {
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

Gfx *font_render_pages(Gfx *gdl, FontSprite **lists)
{
    u8 *base;
    u32 i;
    FontSprite *n;

    base = sFontPageTop;
    for (i = 0; i < 40; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, base - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            n = lists[i];
            do {
                if (n->x >= -63 && n->x < 640) {
                    gDPSetPrimColor(gdl++, 0, 0, n->color.c.r, n->color.c.g, n->color.c.b, n->color.c.a);
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

s32 font_pack_glyphs(Font *font)
{
    FontGlyph *e;
    u32 i;
    u32 j;
    u32 h, w;
    u8 *src;
    u8 *dst;
    u8 *tv;

    e = font->glyphs;
    for (i = 0; i < font->numGlyphs; i++) {
        {
            j = e->h >> 8;
            sFontAllocW = e->w + 1;
            sFontAllocH = e->h + 1;
            while (j < 0x28) {
                if (font_alloc_glyph_space(sFontPageMaps + (j << 6)) != 0) break;
                j++;
            }
            if (j == 0x28) {
                return -1;
            }
            if (sFontPageCount < j) {
                sFontPageCount = j;
            }
            h = e->h;
            w = e->w;
            src = (u8 *)e + *(u16 *)e; /* the bitmap's offset, until packed */
            dst = sFontPageTop - (j << 12);
            dst = dst + sFontAllocX + sFontAllocY * 0x40;
            while (--h != (u32)-1) {
                while (--w != (u32)-1) {
                    *dst++ = *src++;
                }
                w = e->w;
                tv = dst + 0x40;
                dst = tv - w;
            }
            e->page = j;
            e->s = sFontAllocX;
            e->t = sFontAllocY;
            e++;
        }
    }
    return 0;
}

s32 font_alloc_glyph_space(u32 *map)
{
    u32 w = (sFontAllocW + 1) >> 1;
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
    sFontScaleFromCorner = d;
    sFontTexStepX = 0x4000000 / a;
    sFontTexStepY = 0x4000000 / b;
}

u32 font_unload(void) {
    u32 i;
    if (sFontPageMaps != NULL) {
        mem_free(sFontPageMaps);
        sFontPageMaps = NULL;
    }
    if (sFontTable[0] != NULL) {
        mem_free(sFontTable[0]);
        sFontTable[0] = NULL;
    }
    for (i = 1; i < 16; i++) {
        sFontTable[i] = NULL;
    }
    return 0;
}

TextObj *font_text_create(TextObj *t, s32 size, s32 font, s32 color, u32 flags)
{
    s32 n;

    if (size == 0) {
        size = 0x40;
    }
    n = (size + 0xF) & -0x10;
    if (t == NULL) {
        t = (TextObj *)mem_calloc_dbg(n + 0x20, 4, (s32)sStrFontcodeFile, 0x22A);
        if (t == NULL) {
            return NULL;
        }
    }
    if (font != 0) {
        t->font = font;
    }
    if (color != 0) {
        t->color = color;
    }
    if (flags != 0) {
        t->fmt.flags = flags;
    }
    font_text_clear(t);
    return t;
}

TextObj *font_text_clear(TextObj *t)
{
    if (t != NULL) {
        t->text[0] = 0;
        t->x = 0;
        t->y = 0;
        t->w = 0;
        t->h = 0;
    }
    return t;
}

char *font_text_add(TextObj *t, char *s)
{
    Font *font;

    if (t != NULL) {
        str_cat(t->text, s);
        font = sFontTable[t->font];
        font_measure_text((u8 *)t->text, font, &t->w, &t->h);
        if (t->fmt.align.halign == TEXT_HALIGN_CENTER) {
            t->x = -((t->w + 1) / 2);
        } else if (t->fmt.align.halign == TEXT_HALIGN_RIGHT) {
            t->x = -t->w;
        } else {
            t->x = 0;
        }
        if (t->fmt.align.valign == TEXT_VALIGN_BASELINE >> 4) {
            t->y = 0;
        } else if (t->fmt.align.valign == TEXT_VALIGN_TOP >> 4) {
            t->y = font->ascent - 1;
        } else if (t->fmt.align.valign == TEXT_VALIGN_CENTER >> 4) {
            t->y = font->ascent - 1 - (t->h + 1) / 2;
        } else if (t->fmt.align.valign == TEXT_VALIGN_BOTTOM >> 4) {
            t->y = font->ascent - 1 - t->h;
        }
    }
    return s;
}

void font_text_destroy(TextObj *t)
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

void font_str_upper(u8 *s)
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

void font_str_lower(u8 *s)
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
    FontSprite **a = sFontSprLists;
    FontSprite **b = sFontSprListsBack;
    sFontSprNext = sFontSprPool;
    sFontSprPoolEnd = sFontSprPool + 0x3FE;
    do {
        *b = NULL;
        *a = NULL;
        a++;
        i++;
        b++;
    } while (i < 0x28);
    return 0;
}
