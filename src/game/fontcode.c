/* FONTCODE.C: loads the fonts of a font set and packs their glyphs into texture pages, measures and queues text,
   and draws the queued glyphs as texture rectangles. */
#include "game.h"

#define FONT_BUFFER_SIZE 0x30000 /* the fonts while they load, the pages growing down from the top */
#define FONT_PAGE_SIZE   0x1000  /* 64x64 IA8 texels */
#define FONT_MAX_PAGES   40
#define FONT_MAX_FONTS   16
#define FONT_MAX_SPRITES 0x3FE   /* glyphs queued between two font_render */

/* Loads the fonts of the resource ids in ids (NULL-terminated) into one block: the fonts' headers and glyph
   tables, then the pages their glyphs were packed into. palette: the text colours (NULL: sFontDefaultPalette).
   Returns 0, or -1 when out of memory. */
s32 font_load(ResFile *arc, u32 **ids, u32 *palette)
{
    s32 *dst;
    s32 *src;
    Font *font;
    u32 *map;
    u32 k, j;

    if (palette != NULL) {
        sFontPalette = palette;
    } else {
        sFontPalette = sFontDefaultPalette;
    }
    k = FONT_BUFFER_SIZE;
    if (gFontLoadBuffer != NULL) {
        dst = (s32 *)gFontLoadBuffer;
    } else {
        dst = mem_calloc_dbg(k, 0, sStrFontcodeFile, 0x124);
    }
    if (dst == NULL) {
        sprintf(sFontErrorText, sStrFontAllocFailed);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop = (u8 *)dst + k - FONT_PAGE_SIZE;
    sFontPageCount = 0;
    /* per page 64 rows of 32 bits, a bit per two texels in use: row 0 and column 0 are kept free */
    map = mem_alloc_dbg(FONT_MAX_PAGES * 64 * sizeof(u32), 0, sStrFontcodeFile, 0x132);
    sFontPageMaps = map;
    for (k = 0; k < FONT_MAX_PAGES; k++) {
        *map++ = -1;
        for (j = 1; j < 64; j++) {
            *map++ = 0x80000000;
        }
    }
    sFontTable[0] = (Font *)dst;
    for (k = 1; k < FONT_MAX_FONTS; k++) {
        if (*ids == NULL) {
            break;
        }
        font = (Font *)res_load(arc, **ids);
        if (font == NULL) {
            mem_free(sFontPageMaps);
            sFontPageMaps = NULL;
            mem_free(sFontTable[0]);
            sFontTable[0] = NULL;
            sprintf(sFontErrorText, sStrFontLoadFailed);
            fatal_halt_mem_report_2();
            return -1;
        }
        j = *(u16 *)font->glyphs; /* until packed: glyph 0's bitmap offset, the size of the tables */
        font_pack_glyphs(font);
        src = (s32 *)font;
        /* the header and the tables, in 16-byte blocks */
        j += 0x10;
        j += 0xF;
        j >>= 2;
        j >>= 1;
        j >>= 1;
        j--;
        sFontTable[k] = (Font *)dst;
        while (j != -1) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            dst += 4;
            src += 4;
            j--;
        }
        res_unload(arc->tbl + **ids);
        ids++;
    }
    for (; k < FONT_MAX_FONTS; k++) {
        sFontTable[k] = NULL;
    }
    mem_free(sFontPageMaps);
    /* the pages follow the fonts */
    src = (s32 *)sFontPageTop - sFontPageCount * (FONT_PAGE_SIZE / 4);
    k = sFontPageCount << 8;
    k |= 0xFF;
    sFontPageMaps = NULL;
    while (k != -1) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst += 4;
        src += 4;
        k--;
    }
    /* keep only what was used */
    k = (u8 *)dst - (u8 *)sFontTable[0];
    if (gFontLoadBuffer != NULL) {
        dst = mem_alloc_dbg(k, 0, sStrFontcodeFile, 0x1A0);
        if (dst == NULL) {
            fatal_halt(1);
        }
        memcpy(dst, (void *)gFontLoadBuffer, k);
    } else {
        dst = mem_realloc_dbg((s32 *)sFontTable[0], k, 0, sStrFontcodeFile, 0x1A9);
    }
    if (dst == NULL) {
        sprintf(sFontErrorText, sStrFontShrinkFailed);
        fatal_halt_mem_report_2();
        return -1;
    }
    sFontPageTop = (u8 *)dst + k - FONT_PAGE_SIZE;
    /* move the font pointers to the new block */
    k = (u8 *)dst - (u8 *)sFontTable[0];
    for (j = 0; j < FONT_MAX_FONTS; j++) {
        font = sFontTable[j];
        if (font == NULL) {
            break;
        }
        sFontTable[j] = (Font *)((u8 *)font + k);
        if (font->hasKerning == 0) {
            font->kerning = NULL;
        } else {
            font->kerning = (u8 *)font + (font->numGlyphs * 8 + 0x10);
        }
    }
    return 0;
}

/* Queues the glyphs of a text object at (x, y), each in the list of its page (or draws them with
   gFontDirectDrawFunc: TEXT_DIRECT). Returns where the text stopped: its end, or the first character the font
   does not have. */
u8 *font_draw_text(TextObj *text, s32 x, s32 y)
{
    u8 *str = (u8 *)text->text;
    u32 flags = text->fmt.flags;
    s32 triple;
    u32 color;
    Font *font;
    FontGlyph *glyph;
    FontSprite *spr;
    u32 ch, index;
    s32 lineX, scale, direct, direct2, advance;
    u8 *ptr, *ptr2, *kern;

    color = text->color;
    triple = color + color * 2;
    color = triple + 1;
    if (flags & TEXT_GREYED) {
        color = triple;
    } else if (flags & TEXT_HIGHLIGHT) {
        color = triple + 2;
    }
    x += text->x;
    y += text->y;
    color = sFontPalette[color];
    font = sFontTable[text->font];
    lineX = x;
    if (sFontScaleY != FONT_SCALE_ONE) {
        scale = 1;
        if (sFontScaleFromCorner != 0)
            scale = sFontScaleY >> 16;
        ch = *str;
        if (ch != 0) {
            direct = flags & TEXT_DIRECT;
            do {
                str++;
                if (ch == '\n') {
                    x = lineX;
                    y += font->lineAscent + font->lineDescent + font->lineGap;
                } else {
                    index = ch - font->firstChar;
                    if (index >= font->numGlyphs)
                        return str;
                    glyph = &font->glyphs[index];
                    if (ch - 0x80 < 0x10) {
                        /* characters 0x80-0x8F: their positions are kept */
                        sFontSpecialGlyphPos[ch - 0x80][0] = x + glyph->x;
                        sFontSpecialGlyphPos[ch - 0x80][1] = y + glyph->y;
                    }
                    if (direct != 0) {
                        ptr = sFontPageTop - (glyph->page << 12);
                        gFontDirectDrawFunc((s32)ptr + (glyph->s + (glyph->t << 6)),
                                   x + glyph->x, y + glyph->y, glyph->w, glyph->h);
                    } else {
                        spr = sFontSprNext;
                        sFontSprNext = spr + 1;
                        spr->next = sFontSprLists[glyph->page];
                        sFontSprLists[glyph->page] = spr;
                        spr->x = (s8)((u8 *)glyph)[2] + x;
                        spr->y = y + glyph->y * scale;
                        spr->w = glyph->w;
                        spr->h = glyph->h;
                        spr->s = glyph->s;
                        spr->t = glyph->t;
                        spr->color.rgba = color;
                        if (flags & TEXT_SHADOW) {
                            spr = sFontSprNext;
                            sFontSprNext = spr + 1;
                            spr->next = sFontSprLists[glyph->page];
                            sFontSprLists[glyph->page] = spr;
                            spr->x = (s8)((u8 *)glyph)[2] + x + 2;
                            spr->y = y + (glyph->y + 1) * scale;
                            spr->w = glyph->w;
                            spr->h = glyph->h;
                            spr->s = glyph->s;
                            spr->t = glyph->t;
                            spr->color.rgba = color & 0xFF; /* black, with the text's alpha */
                        }
                    }
                    advance = glyph->advance + font->spacing;
                    ptr = font->kerning;
                    if (ptr != 0) {
                        do { do { kern = ptr + index; } while (0); } while (0);
                        if (*kern != 0) {
                            /* the {next char, adjustment} pairs of this glyph */
                            ptr += *kern * 2;
                            index = *str;
                            while (*ptr != 0) {
                                if (*ptr == index) {
                                    advance += (s8)ptr[1];
                                    break;
                                }
                                ptr += 2;
                            }
                        }
                    }
                    do { x += (advance * sFontScaleX) >> 16; } while (0);
                }
                ch = *str;
            } while (ch != 0);
        }
    } else {
        ch = *str;
        if (ch != 0) {
            direct2 = flags & TEXT_DIRECT;
            do {
                str++;
                if (ch == '\n') {
                    x = lineX;
                    y += font->lineAscent + font->lineDescent + font->lineGap;
                } else {
                    index = ch - font->firstChar;
                    if (index >= font->numGlyphs)
                        return str;
                    glyph = &font->glyphs[index];
                    if (ch - 0x80 < 0x10) {
                        sFontSpecialGlyphPos[ch - 0x80][0] = x + glyph->x;
                        sFontSpecialGlyphPos[ch - 0x80][1] = y + glyph->y;
                    }
                    if (direct2 != 0) {
                        ptr2 = sFontPageTop - (glyph->page << 12);
                        gFontDirectDrawFunc((s32)ptr2 + (glyph->s + (glyph->t << 6)),
                                   x + glyph->x, y + glyph->y, glyph->w, glyph->h);
                    } else {
                        spr = sFontSprNext;
                        sFontSprNext = spr + 1;
                        spr->next = sFontSprLists[glyph->page];
                        sFontSprLists[glyph->page] = spr;
                        spr->x = (s8)((u8 *)glyph)[2] + x;
                        spr->y = y + glyph->y * (sFontScaleY >> 16);
                        spr->w = glyph->w;
                        spr->h = glyph->h;
                        spr->s = glyph->s;
                        spr->t = glyph->t;
                        spr->color.rgba = color;
                        if (flags & TEXT_SHADOW) {
                            spr = sFontSprNext;
                            sFontSprNext = spr + 1;
                            spr->next = sFontSprLists[glyph->page];
                            sFontSprLists[glyph->page] = spr;
                            spr->x = (s8)((u8 *)glyph)[2] + x + 2;
                            spr->y = (s8)((u8 *)glyph)[3] + y + 1;
                            spr->w = glyph->w;
                            spr->h = glyph->h;
                            spr->s = glyph->s;
                            spr->t = glyph->t;
                            spr->color.rgba = color & 0xFF;
                        }
                    }
                    advance = glyph->advance + font->spacing;
                    ptr2 = font->kerning;
                    if (ptr2 != 0) {
                        do { do { kern = ptr2 + index; } while (0); } while (0);
                        if (*kern != 0) {
                            ptr2 += *kern * 2;
                            index = *str;
                            while (*ptr2 != 0) {
                                if (*ptr2 == index) {
                                    advance += (s8)ptr2[1];
                                    break;
                                }
                                ptr2 += 2;
                            }
                        }
                    }
                    do { x += advance; } while (0);
                }
                ch = *str;
            } while (ch != 0);
        }
    }
    return str;
}

/* Where character n of a text object drawn at x lands: (the glyph's centre << 16) | its width, or -1. */
s32 font_get_char_pos(TextObj *text, s32 x, s32 unused, s32 n)
{
    FontGlyph *glyph;
    u8 *str;
    Font *font;
    u8 *kern;
    s32 lineX;
    u32 index;
    s32 ch;
    s32 next;
    u8 width;

    glyph = *(FontGlyph *volatile *)&text->fmt.flags; /* a load of the flags the result never uses */
    str = (u8 *)text->text;
    goto setup;

ret:
    width = glyph->w;
    return ((x + glyph->x + (width >> 1)) << 16) | width;

setup:
    x = x + text->x;
    lineX = x;
    ch = *str;
    font = sFontTable[text->font];
    if (ch == 0) {
        goto fail;
    }
    do {
        index = ch - font->firstChar;
        if (index >= font->numGlyphs) {
            goto fail;
        }
        str++;
        n--;
        glyph = &font->glyphs[index];
        if (n == -1) {
            goto ret;
        }
        if (ch == '\n') {
            x = lineX;
        } else {
            x = x + (glyph->advance + font->spacing);
            kern = font->kerning;
            if (kern != 0 && kern[index] != 0) {
                kern = kern + kern[index] * 2;
                index = *str;
                while ((next = *kern) != 0) {
                    if (next == index) {
                        x = x + (s8)kern[1];
                        break;
                    }
                    kern = kern + 2;
                }
            }
        }
        ch = *str;
    } while (ch != 0);
fail:
    return -1;
}

/* Measures a text in a font: the width of its widest line and its height (a trailing newline adds none). */
u8 *font_measure_text(u8 *str, Font *font, u32 *outW, u32 *outH)
{
    u8 *p = str;
    s32 h;
    s8 endsWithNewline = 0;
    u32 maxW = 0;
    u32 lineW;
    u32 ch;
    u32 firstChar;
    u32 numGlyphs;
    s32 spacing;
    u8 *kern;
    FontGlyph *glyphs;

    *outH = 0;
    *outW = 0;
    h = font->ascent;
    firstChar = font->firstChar;
    numGlyphs = font->numGlyphs;
    spacing = font->spacing;
    glyphs = font->glyphs;
    while (*p != 0) {
        lineW = 0;
        ch = *p;
        while (ch != 0) {
            p++;
            if (ch == '\n') {
                endsWithNewline = 1;
                h += font->lineAscent + font->lineDescent + font->lineGap;
                break;
            }
            if (ch != ' ') {
                endsWithNewline = 0;
            }
            ch -= firstChar;
            if ((s32)ch < 0 || ch >= numGlyphs) {
                break;
            }
            lineW += glyphs[ch].advance + spacing;
            kern = font->kerning;
            if (kern != NULL && kern[ch] != 0) {
                kern += kern[ch] * 2;
                while (*kern != 0) {
                    if (*kern == *str) { /* the text's first character, not the next one: an original bug */
                        lineW += (s8)kern[1];
                        break;
                    }
                    kern += 2;
                }
            }
            ch = *p;
        }
        if (maxW < lineW) {
            maxW = lineW;
        }
    }
    if (maxW != 0) {
        maxW -= spacing;
    }
    if (endsWithNewline) {
        h -= font->lineAscent + font->lineDescent + font->lineGap;
    }
    *outW = maxW;
    *outH = h;
    return p;
}

/* Draws the queued glyphs (the back lists first) and empties the queue. */
Gfx *font_render(Gfx *gdl)
{
    u32 i;

    gdl = font_gfx_setup(gdl);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if (sFontScaleX == FONT_SCALE_ONE && sFontScaleY == FONT_SCALE_ONE) {
        gdl = font_render_pages(gdl, sFontSprListsBack);
        gdl = font_render_pages(gdl, sFontSprLists);
    } else {
        gdl = font_render_pages_scaled(gdl, sFontSprListsBack);
        gdl = font_render_pages_scaled(gdl, sFontSprLists);
    }
    gDPPipeSync(gdl++);
    sFontSprNext = sFontSprPool;
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    sFontSprPoolEnd = sFontSprPool + FONT_MAX_SPRITES;
    for (i = 0; i < FONT_MAX_PAGES; i++) {
        sFontSprListsBack[i] = NULL;
        sFontSprLists[i] = NULL;
    }
    return gdl;
}

/* Draws the glyphs of each page's list scaled by sFontScaleX and sFontScaleY, about their centres or (with
   sFontScaleFromCorner) from their top left corners. */
Gfx *font_render_pages_scaled(Gfx *gdl, FontSprite **lists)
{
    u32 i;
    FontSprite *spr;
    s32 scaleY;

    for (i = 0; i < FONT_MAX_PAGES; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, sFontPageTop - i * FONT_PAGE_SIZE, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            scaleY = sFontScaleY;
            spr = lists[i];
            do {
                if ((u16)(spr->x + 63) < 703) { /* -63 <= x < 640 */
                    gDPSetPrimColor(gdl++, 0, 0, spr->color.c.r, spr->color.c.g, spr->color.c.b, spr->color.c.a);
                    if (sFontScaleFromCorner == 0) {
                        gSPTextureRectangle(gdl++,
                            (spr->x + (((0x10000 - sFontScaleX) * spr->w) >> 17)) << 2,
                            (spr->y + (((0x10000 - scaleY) * spr->h) >> 17)) << 2,
                            (spr->x + (((sFontScaleX + 0x10000) * spr->w) >> 17)) << 2,
                            (spr->y + (((scaleY + 0x10000) * spr->h) >> 17)) << 2,
                            G_TX_RENDERTILE, spr->s << 5, spr->t << 5, sFontTexStepX, sFontTexStepY);
                    } else {
                        gSPTextureRectangle(gdl++,
                            spr->x << 2,
                            spr->y << 2,
                            (spr->x + ((sFontScaleX * spr->w) >> 16)) << 2,
                            (spr->y + ((scaleY * spr->h) >> 16)) << 2,
                            G_TX_RENDERTILE, spr->s << 5, spr->t << 5, sFontTexStepX, sFontTexStepY);
                    }
                }
                spr = spr->next;
            } while (spr != NULL);
        }
    }
    return gdl;
}

#define gDPWord_2(pkt, a, b)     {         Gfx *_g = (Gfx *)(pkt);         _g->words.w0 = (a);         _g->words.w1 = (b);     }

/* Draws the glyphs of each page's list unscaled. */
Gfx *font_render_pages(Gfx *gdl, FontSprite **lists)
{
    u8 *pageTop;
    u32 i;
    FontSprite *spr;

    pageTop = sFontPageTop;
    for (i = 0; i < FONT_MAX_PAGES; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, pageTop - i * FONT_PAGE_SIZE, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            spr = lists[i];
            do {
                if (spr->x >= -63 && spr->x < 640) {
                    gDPSetPrimColor(gdl++, 0, 0, spr->color.c.r, spr->color.c.g, spr->color.c.b, spr->color.c.a);
                    /* gSPTextureRectangle by hand */
                    { s32 x = spr->x;
                    gDPWord_2(gdl++, _SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL((spr->w + x) << 2, 12, 12)
                                         | _SHIFTL((spr->h + spr->y) << 2, 0, 12),
                            _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(spr->x << 2, 12, 12) | _SHIFTL(spr->y << 2, 0, 12)); }
                    gDPWord_2(gdl++, _SHIFTL(G_RDPHALF_1, 24, 8), _SHIFTL(spr->s << 5, 16, 16) | _SHIFTL(spr->t << 5, 0, 16));
                    gDPWord_2(gdl++, _SHIFTL(G_RDPHALF_2, 24, 8), _SHIFTL(1 << 10, 16, 16) | _SHIFTL(1 << 10, 0, 16));
                }
                spr = spr->next;
            } while (spr != NULL);
        }
    }
    return gdl;
}

/* Copies each glyph's bitmap of a font just loaded into the first page with room for it, and turns its bitmap
   offset into its place in the page. Returns -1 when the pages are full. */
s32 font_pack_glyphs(Font *font)
{
    FontGlyph *glyph;
    u32 i;
    u32 page;
    u32 h, w;
    u8 *src;
    u8 *dst;
    u8 *next;

    glyph = font->glyphs;
    for (i = 0; i < font->numGlyphs; i++) {
        {
            page = glyph->h >> 8; /* 0 */
            sFontAllocW = glyph->w + 1;
            sFontAllocH = glyph->h + 1;
            while (page < FONT_MAX_PAGES) {
                if (font_alloc_glyph_space(sFontPageMaps + (page << 6)) != 0) break;
                page++;
            }
            if (page == FONT_MAX_PAGES) {
                return -1;
            }
            if (sFontPageCount < page) {
                sFontPageCount = page;
            }
            h = glyph->h;
            w = glyph->w;
            src = (u8 *)glyph + *(u16 *)glyph; /* the bitmap's offset, until packed */
            dst = sFontPageTop - (page << 12);
            dst = dst + sFontAllocX + sFontAllocY * 0x40;
            while (--h != (u32)-1) {
                while (--w != (u32)-1) {
                    *dst++ = *src++;
                }
                w = glyph->w;
                next = dst + 0x40;
                dst = next - w;
            }
            glyph->page = page;
            glyph->s = sFontAllocX;
            glyph->t = sFontAllocY;
            glyph++;
        }
    }
    return 0;
}

/* Looks in a page's map for a free sFontAllocW x sFontAllocH texels and marks it used: sFontAllocX and
   sFontAllocY get its position. Returns 0 when the page has no room. */
s32 font_alloc_glyph_space(u32 *map)
{
    u32 cols = (sFontAllocW + 1) >> 1; /* in pairs of texels */
    s32 rows = sFontAllocH;
    s32 y = 1;
    s32 maxX = 0x20 - cols;
    s32 maxY = 0x40 - rows;
    u32 mask = sFontAllocMasks[cols - 1]; /* a bit every cols columns: all of them used, no room in the row */
    u32 firstRun = sFontAllocBits[cols - 1]; /* cols bits from the left */
    u32 used;
    u32 run;
    u32 *row;
    s32 i;
    s32 x;

    if (y < maxY) {
        do {
            used = 0;
            row = map + rows;
            for (i = rows; i != 0; i--) {
                row--;
                used |= *row;
                if ((used & mask) == mask) {
                    break;
                }
            }
            if (i != 0) {
                y += i;
                map += i;
                continue;
            }
            used = ~used;
            run = firstRun;
            x = 0;
            if (maxX > 0) {
                do {
                    if ((used & run) == run) {
                        goto found;
                    }
                    x++;
                    run >>= 1;
                } while (x < maxX);
            }
            y++;
            map++;
        } while (y < maxY);
    }
    return 0;

found:
    while (--rows != -1) {
        *map++ |= run;
    }
    sFontAllocX = x << 1;
    sFontAllocY = y;
    return 1;
}

/* Sets the scale the text is drawn at (16.16 fixed point, FONT_SCALE_ONE = 1.0) and whether glyphs scale from
   their corners rather than their centres. */
void font_set_scale(s32 scaleX, s32 scaleY, s32 unk, s32 fromCorner)
{
    sFontScaleX = scaleX;
    sFontScaleY = scaleY;
    D_800D5768 = unk;
    sFontScaleFromCorner = fromCorner;
    sFontTexStepX = 0x4000000 / scaleX;
    sFontTexStepY = 0x4000000 / scaleY;
}

/* Frees the fonts. */
u32 font_unload(void)
{
    u32 i;
    if (sFontPageMaps != NULL) {
        mem_free(sFontPageMaps);
        sFontPageMaps = NULL;
    }
    if (sFontTable[0] != NULL) {
        mem_free(sFontTable[0]);
        sFontTable[0] = NULL;
    }
    for (i = 1; i < FONT_MAX_FONTS; i++) {
        sFontTable[i] = NULL;
    }
    return 0;
}

/* Allocates a text object for size characters (text NULL; size 0: 0x40), or reuses one, and empties it. A font,
   colour or flags of 0 keeps the object's own. */
TextObj *font_text_create(TextObj *text, s32 size, s32 font, s32 color, u32 flags)
{
    s32 bufSize;

    if (size == 0) {
        size = 0x40;
    }
    bufSize = (size + 0xF) & -0x10;
    if (text == NULL) {
        text = (TextObj *)mem_calloc_dbg(bufSize + 0x20, 4, (s32)sStrFontcodeFile, 0x22A); /* + the header */
        if (text == NULL) {
            return NULL;
        }
    }
    if (font != 0) {
        text->font = font;
    }
    if (color != 0) {
        text->color = color;
    }
    if (flags != 0) {
        text->fmt.flags = flags;
    }
    font_text_clear(text);
    return text;
}

/* Empties a text object. */
TextObj *font_text_clear(TextObj *text)
{
    if (text != NULL) {
        text->text[0] = 0;
        text->x = 0;
        text->y = 0;
        text->w = 0;
        text->h = 0;
    }
    return text;
}

/* Appends to a text object's text, measures it and places it by its alignment. */
char *font_text_add(TextObj *text, char *str)
{
    Font *font;

    if (text != NULL) {
        str_cat(text->text, str);
        font = sFontTable[text->font];
        font_measure_text((u8 *)text->text, font, &text->w, &text->h);
        if (text->fmt.align.halign == TEXT_HALIGN_CENTER) {
            text->x = -((text->w + 1) / 2);
        } else if (text->fmt.align.halign == TEXT_HALIGN_RIGHT) {
            text->x = -text->w;
        } else {
            text->x = 0;
        }
        if (text->fmt.align.valign == TEXT_VALIGN_BASELINE >> 4) {
            text->y = 0;
        } else if (text->fmt.align.valign == TEXT_VALIGN_TOP >> 4) {
            text->y = font->ascent - 1;
        } else if (text->fmt.align.valign == TEXT_VALIGN_CENTER >> 4) {
            text->y = font->ascent - 1 - (text->h + 1) / 2;
        } else if (text->fmt.align.valign == TEXT_VALIGN_BOTTOM >> 4) {
            text->y = font->ascent - 1 - text->h;
        }
    }
    return str;
}

/* Does nothing: the callers free text objects with mem_free. */
void font_text_destroy(TextObj *text)
{
}

/* Writes the initials of the words of src ("John Smith": "J. S."). */
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

/* Upper-cases a string. */
void font_str_upper(u8 *str)
{
    u8 c = *str;
    while (c != 0) {
        if ((u32)(c - 'a') < 26) {
            *str = c - ('a' - 'A');
        }
        str = str + 1;
        c = *str;
    }
}

/* Upper-cases a name, but for "Mc" ("MCGRATH": "McGRATH") and "De" before "CL". */
void font_name_upper(u8 *str)
{
    u8 *p = str;
    u8 c;

    if (str[0] != 0) {
        do {
            c = *p;
            if ((u32)(c - 'a') < 26) {
                *p = c - ('a' - 'A');
            }
            p++;
        } while (*p != 0);
    }
    if (str[0] == 'M') {
        if (str[1] == 'C') {
            str[1] = 'c';
        }
    }
    if (str[0] == 'D' && str[1] == 'E' && str[2] == 'C' && str[3] == 'L') {
        str[1] = 'e';
    }
}

/* Lower-cases a string. */
void font_str_lower(u8 *str)
{
    u8 c = *str;
    while (c != 0) {
        if ((u32)(c - 'A') < 26) {
            *str = c + ('a' - 'A');
        }
        str = str + 1;
        c = *str;
    }
}

/* As font_str_lower, but it tests for lower-case letters: they become 0x81-0x9A. */
void func_800C21F0(u8 *str)
{
    u8 c = *str;
    while (c != 0) {
        if ((u32)(c - 'a') < 26) {
            *str = c + ('a' - 'A');
        }
        str = str + 1;
        c = *str;
    }
}

/* Empties the glyph queue. */
s32 font_reset_queue(void)
{
    u32 i = 0;
    FontSprite **lists = sFontSprLists;
    FontSprite **listsBack = sFontSprListsBack;
    sFontSprNext = sFontSprPool;
    sFontSprPoolEnd = sFontSprPool + FONT_MAX_SPRITES;
    do {
        *listsBack = NULL;
        *lists = NULL;
        lists++;
        i++;
        listsBack++;
    } while (i < FONT_MAX_PAGES);
    return 0;
}
