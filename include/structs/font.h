/* FONTCODE.C: the fonts of a font set (sFontTable), their glyphs packed into 40 IA8 pages of 64x64 texels, and the
   text objects drawn with them. font_draw_text queues a FontSprite per glyph in the list of its page; font_render
   draws the lists as texture rectangles. */
#ifndef STRUCTS_FONT_H
#define STRUCTS_FONT_H

#include "common.h"

typedef struct FontGlyph {
    /* 0x0 */ s8 s;       /* texel position in its page (in the resource, the two bytes are the offset of the */
    /* 0x1 */ s8 t;       /* glyph's bitmap from this entry, until font_pack_glyphs packs it) */
    /* 0x2 */ s8 x;       /* offset from the pen position */
    /* 0x3 */ s8 y;
    /* 0x4 */ u8 w;
    /* 0x5 */ u8 h;
    /* 0x6 */ s8 advance;
    /* 0x7 */ u8 page;
} FontGlyph; /* size 0x8 */

typedef struct Font {
    /* 0x00 */ u8 *kerning;   /* NULL, or per glyph the offset (in pairs) of its {next char, adjustment} list */
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 hasKerning; /* the kerning table follows the glyphs */
    /* 0x06 */ u8 firstChar;
    /* 0x07 */ u8 numGlyphs;
    /* 0x08 */ s8 spacing;    /* added to each glyph's advance */
    /* 0x09 */ s8 lineGap;    /* a line is lineAscent + lineDescent + lineGap high */
    /* 0x0A */ u8 unk_0A[2];
    /* 0x0C */ s8 lineAscent;
    /* 0x0D */ s8 lineDescent;
    /* 0x0E */ s8 ascent;     /* baseline of the first line below the top of the text */
    /* 0x0F */ u8 unk_0F;
    /* 0x10 */ FontGlyph glyphs[1]; /* numGlyphs, from firstChar on */
} Font;

/* A glyph queued for font_render */
typedef struct FontSprite {
    /* 0x00 */ struct FontSprite *next;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
    /* 0x08 */ u8 w;
    /* 0x09 */ u8 h;
    /* 0x0A */ u8 s;
    /* 0x0B */ u8 t;
    /* 0x0C */ union {
        u32 rgba;   /* font_draw_text stores the colour as one word, font_render reads the bytes */
        struct {
            u8 r, g, b, a;
        } c;
    } color;
} FontSprite; /* size 0x10 */

/* TextObj.flags. The low byte is the alignment: horizontal in bits 0-3, vertical in bits 4-7. */
#define TEXT_HALIGN_LEFT     0x00
#define TEXT_HALIGN_CENTER   0x01
#define TEXT_HALIGN_RIGHT    0x02
#define TEXT_VALIGN_TOP      0x00 /* the position is the top of the text */
#define TEXT_VALIGN_BASELINE 0x10 /* the position is the first line's baseline */
#define TEXT_VALIGN_CENTER   0x20
#define TEXT_VALIGN_BOTTOM   0x30
#define TEXT_DIRECT          0x08000000 /* each glyph goes to gFontDirectDrawFunc instead of the queue */
#define TEXT_SHADOW          0x10000000 /* a black copy 2 texels right and 1 down */
#define TEXT_GREYED          0x20000000 /* first colour of the colour's triple */
#define TEXT_HIGHLIGHT       0x40000000 /* third colour of the triple */
/* The usual alignments: vertically centred on the position, and starting at it, centred on it or ending at it */
#define TEXT_LEFT   (TEXT_VALIGN_CENTER | TEXT_HALIGN_LEFT)
#define TEXT_CENTER (TEXT_VALIGN_CENTER | TEXT_HALIGN_CENTER)
#define TEXT_RIGHT  (TEXT_VALIGN_CENTER | TEXT_HALIGN_RIGHT)

/* TextObj.font: the index in sFontTable of the fonts of sMenuFontIds (the race set, sRaceFontIds, has the first
   three). Font 0 is the first font again; font_text_create takes 0 as "keep the current one". */
#define FONT_MAIN   1
#define FONT_SMALL  2 /* the Controller Pak screen draws its '@' as character 0x7F */
#define FONT_DIGITS 3
#define FONT_TITLE  4
#define FONT_MENU   5

/* TextObj.color: the triple n of gFontColors (greyed, normal, highlighted), named by the normal colour. 0 is black,
   and font_text_create takes it as "keep the current one". */
#define TEXT_COLOR_LIGHT_GREY    2  /* most text */
#define TEXT_COLOR_YELLOW        4  /* the selected item */
#define TEXT_COLOR_BRIGHT_YELLOW 5
#define TEXT_COLOR_CYAN          6
#define TEXT_COLOR_GREEN         7
#define TEXT_COLOR_RED           8
#define TEXT_COLOR_WHITE         9  /* its alpha is faded by shell.c */
#define TEXT_COLOR_GREY          10

/* A text object: font_text_create allocates it with room for the text, font_text_add appends to the text and
   measures it, font_draw_text queues it at a position. */
typedef struct TextObj {
    /* 0x00 */ s32 font;   /* index in sFontTable */
    /* 0x04 */ s32 color;  /* triple of the font palette (sFontPalette) */
    /* 0x08 */ union {
        u32 flags;         /* TEXT_* */
        struct {
            u32 style : 24;
            u32 valign : 4; /* TEXT_VALIGN_* >> 4 */
            u32 halign : 4; /* TEXT_HALIGN_* */
        } align;           /* font_text_add reads the alignment as bit fields */
    } fmt;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 x;      /* where the text starts relative to the position it is drawn at (from the alignment) */
    /* 0x14 */ s32 y;
    /* 0x18 */ u32 w;      /* size of the text, set by font_text_add */
    /* 0x1C */ u32 h;
    /* 0x20 */ char text[1]; /* as long as font_text_create was asked for */
} TextObj;

#endif
