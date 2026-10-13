#include "ctx.h"

typedef struct {
    u8 w;
    u8 pad[7];
} Glyph;

typedef struct {
    u32 hdr;
    u32 pal;
    Glyph g[1];
} Font;

extern u8 D_800F47D0[];
extern s32 *D_800CE164;
extern Font *D_800F4A58;
extern s32 D_800E30A8;

s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_8000A690(s32 x, s32 y, Glyph *g, Gfx *gdl, s32 e, s32 f, s32 h, s32 i, s32 j);

static inline Gfx *drawStr(Gfx *gdl, u8 *str, s32 p3, s32 p4, s32 x, s32 y)
{
    s32 i;

    if (p3 == 2) {
        for (i = 0; str[i] != 0; i++) {
            if (str[i] == 0x40) {
                str[i] = 0x7F;
            }
        }
    }
    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164, (char *)str);
    func_800C0ACC(D_800CE164, x, y);
    gdl = func_800C1394(gdl);
    return gdl;
}

Gfx *func_80088734(Gfx *gdl, u8 *s, s32 p3, s32 p4, s32 x, s32 y)
{
    s32 k = 0;
    s32 code;
    s32 i;

    if (*s != 0) { i = 0; do {
        if (s[i] != 0x40) {
            D_800F47D0[k] = s[i];
            i++;
            k++;
        } else {
            D_800F47D0[k] = 0;
            if (k != 0) {
                if (gdl != NULL) {
                    gdl = drawStr(gdl, D_800F47D0, p3, p4, x, y);
                } else {
                    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
                    func_800C1F2C(D_800CE164, (char *)D_800F47D0);
                }
                x += D_800CE164[6];
            }
            i++;
            k = 0;
            code = s[i] - 0x41;
            if (gdl != NULL) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPLoadTLUT_pal256(gdl++, D_800F4A58->pal);
                {
                    Gfx *g = gdl++;
                    g->words.w0 = 0xFCFFFFFF;
                    g->words.w1 = 0xFFFCF279;
                }
                gdl = func_8000A690(x, y - 4, &D_800F4A58->g[code], gdl, D_800E30A8, 1, 1, 3, 0);
            }
            i++;
            x += D_800F4A58->g[code].w * 8;
        }
    } while (s[i] != 0); }
    D_800F47D0[k] = 0;
    if (k != 0) {
        if (gdl != NULL) {
                    gdl = drawStr(gdl, D_800F47D0, p3, p4, x, y);
                } else {
                    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
                    func_800C1F2C(D_800CE164, (char *)D_800F47D0);
                }
        x += D_800CE164[6];
    }
    if (gdl != NULL) {
        return gdl;
    }
    return (Gfx *)x;
}
