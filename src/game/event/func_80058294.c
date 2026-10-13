#include "ctx.h"

#define gDPWords(pkt, a, b) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        _g->words.w0 = (a); \
        _g->words.w1 = (b); \
    }

typedef struct {
    u8 w;
    u8 h;
    u8 unk2[6];
} Glyph;

typedef struct {
    s32 unk0;
    u16 *palette;
    Glyph glyphs[1];
} Font;

typedef struct {
    u8 unk0[0x730];
    Font *font;
} Hud;

extern Hud D_800EFD70;

Gfx *func_8000A690(s32 x, s32 y, Glyph *glyph, Gfx *gdl, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8);

Gfx *func_80058294(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y) {
    while (*str != 0) {
        s32 w; s32 hh;

        gDPWords(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPWords(gdl++, 0xE3001001, 0x8000);
        gDPWords(gdl++, 0xFD100000, (u32)D_800EFD70.font->palette);
        gDPWords(gdl++, 0xE8000000, 0);
        gDPWords(gdl++, 0xF5000100, 0x07000000);
        gDPWords(gdl++, 0xE6000000, 0);
        gDPWords(gdl++, 0xF0000000, 0x073FC000);
        gDPWords(gdl++, 0xE7000000, 0);
        w = D_800EFD70.font->glyphs[*str].w << 3;
        hh = D_800EFD70.font->glyphs[*str].h;
        gdl = func_8000A690(x, y - (hh >> 1), &D_800EFD70.font->glyphs[*str], gdl, arg1, 1, 1, 3, 0);
        str++;
        x += w;
    }
    return gdl;
}
