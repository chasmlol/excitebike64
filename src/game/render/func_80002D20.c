#include "ctx.h"

typedef struct { u32 w0; u32 img; } TexHdr;
typedef struct {
    s16 cnt;
    s16 pad;
    TexHdr *tex;
    u8 *bar;
    TexHdr *tex2;
    TexHdr *anim;
    TexHdr *tex3;
} EbState;
typedef struct {
    u8 pad0[0x74];
    s32 *font;
    u8 pad78[0x17C - 0x78];
    s32 frame;
} GState;
typedef struct {
    u8 pad0[0x2C4];
    u8 flag;
} Opts;

extern EbState D_800E3730;
extern s32 D_800E36FC;
extern GState D_800EFD70;
extern Opts D_800F1900;
extern u8 D_800F1BC5[];
extern s16 D_800C27B8[];
extern char *D_800CD274[];
extern char *D_800CD26C[];

s32 func_800C2224(void);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_800028D8();
Gfx *func_8000A690(s32 a, s32 b, void *c, Gfx *gdl, s32 fb, s32 e, s32 f, s32 g, s32 h);
void func_800C0ACC(s32 *p, s32 x, s32 y);

#define G(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_80002D20(Gfx *gdl, s32 fb)
{
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    TexHdr *t;
    s32 xi;
    u8 *p;

    if (D_800E3730.cnt > 0) {
        D_800E3730.cnt--;
        gdl = func_800028D8(gdl, fb);
    } else if (D_800E3730.cnt == -1) {
        gdl = func_800028D8(gdl, fb);
        G(gdl++, 0xE200001C, 0x0C084000);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, D_800E3730.tex->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = func_8000A690(0, 0, D_800E3730.tex + 1, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xFA000000, 0xE6E6E6FF);
        G(gdl++, 0xE3000C00, 0);
        G(gdl++, 0xE200001C, 0x00404240);
        func_800C2224();
        t = D_800E3730.tex2 + 1;
        G(gdl++, 0xFD100000, D_800E3730.tex2->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = func_8000A690(0x45, 0x3D, t, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xE3001001, 0);
        G(gdl++, 0xD7000002, 0x80008000);
        G(gdl++, 0xE200001C, 0x00404240);
        G(gdl++, 0xFC119623, 0xFF2FFFFF);
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 29; i++) {
                xi = i * 6 + 282;
                x = xi - j * 7;
                y = D_800C27B8[j] - 1;
                if (i <= D_800F1BC5[j] * 29 / 255) {
                    if (j == D_800E36FC) {
                        G(gdl++, 0xFA000000, (((i * 255 / 29) & 0xFF) << 16) | 0xFF0000FF);
                    } else {
                        G(gdl++, 0xFA000000, (((i * 140 / 29) & 0xFF) << 16) | 0x8C0000FF);
                    }
                } else {
                    G(gdl++, 0xFA000000, 0x4B4B4BFF);
                }
                gdl = func_8000A690(x, y, D_800E3730.bar + 0x28, gdl, fb, 1, 1, 6, 0);
            }
        }
        G(gdl++, 0xFA000000, 0xD2D2D2FF);
        t = (TexHdr *)(D_800E3730.bar + 0x30);
        gdl = func_8000A690(0x100 - ((u8 *)t)[0] * 4, 0x78 - (((u8 *)t)[1] >> 1), t, gdl, fb, 1, 1, 6, ((u32)t & 1) >> 1);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, &D_800E3730.tex3->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        t = D_800E3730.anim + ((D_800EFD70.frame / 2) % 30 + 1);
        for (j = 0; j < 4; j++) {
            if (j < 3) {
                x = 0x48 - j * 7;
                y = D_800C27B8[j];
            } else {
                xi = ((j - 3) & 1) * 194 + 0x48;
                x = xi - ((j - 3) / 2 + 3) * 7;
                y = D_800C27B8[j];
            }
            func_800C1E60(D_800EFD70.font, 0x40, 1, (j == D_800E36FC) ? 4 : 2, 0x20);
            func_800C1F2C(D_800EFD70.font, D_800CD274[j]);
            func_800C0ACC(D_800EFD70.font, x + 0x24, y + 8);
            if (j == D_800E36FC) {
                gdl = func_8000A690(x - 2, y, t, gdl, fb, 1, 1, 3, 0);
            }
            func_800C1E60(D_800EFD70.font, 0x40, 1, (j == D_800E36FC) ? 6 : 2, 0x22);
            if (j == 3) {
                func_800C1F2C(D_800EFD70.font, D_800CD26C[D_800F1900.flag & 1]);
                func_800C0ACC(D_800EFD70.font, x + 0xB4, y + 8);
            }
        }
        gdl = func_800C1394(gdl);
    }
    return gdl;
}
