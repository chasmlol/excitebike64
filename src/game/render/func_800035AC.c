#include "ctx.h"

typedef struct { u32 w0; u32 img; } TexHdr;
typedef struct { s16 cnt; s16 pad; TexHdr *tex; } EbState;
typedef struct {
    u8 pad0[0x50];
    s8 cfg[0x24];
    s32 *font;
    u8 pad78[0x2C8 - 0x78];
    s8 ctl[4];
} GState;
typedef struct { s16 x; s16 y; } Pos;

extern EbState D_800E3730;
extern u32 D_800E1718[];
extern s32 D_800E37AC;
extern GState D_800EFD70;
extern char *D_800CDAA0[];
extern s32 D_800CDACC[][9];
extern Pos D_800CDA70[];
extern Pos D_800CDA94[];
extern char *D_800CDABC[];
extern char *D_800CDAC8;

s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_800028D8(Gfx *gdl, s32 fb, u8 *mtx);
Gfx *func_8000A690(u32 a, u32 b, void *c, Gfx *gdl, s32 fb, s32 e, s32 f, s32 g, s32 h);
s32 func_800AEC10(char *s);
void func_800C0ACC(s32 *p, s32 x, s32 y);
char *strcpy(char *, const char *);

#define G(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_800035AC(Gfx *gdl, s32 fb, u8 *mtx)
{
    char buf[32];
    s32 i;
    s32 a3v;
    s32 v;
    s32 sel;
    s32 lo;

    if (D_800E3730.cnt > 0) {
        D_800E3730.cnt--;
        gdl = func_800028D8(gdl, fb, mtx);
    } else if (D_800E3730.cnt == -1) {
    gdl = func_800028D8(gdl, fb, mtx);
    G(gdl++, 0xE200001C, 0x00404240);
    G(gdl++, 0xE3001001, 0x00008000);
    G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    G(gdl++, 0xFD100000, D_800E3730.tex->img);
    G(gdl++, 0xE8000000, 0);
    G(gdl++, 0xF5000100, 0x07000000);
    G(gdl++, 0xE6000000, 0);
    G(gdl++, 0xF0000000, 0x073FC000);
    G(gdl++, 0xE7000000, 0);
    G(gdl++, 0xE7000000, 0);
    gdl = func_8000A690(D_800E1718[0], D_800E1718[1], D_800E3730.tex + 1, gdl, fb, 1, 1, 3, 0);
    func_800C2224();
    func_800C1E60(D_800EFD70.font, 0x40, 4, 2, 0x21);
    strcpy(buf, "Controller ");
    i = 0;
    func_8000EE30(D_800EFD70.ctl[D_800E37AC] + 1, buf + func_800AEC10(buf));
    func_800C1F2C(D_800EFD70.font, buf);
    func_800C0ACC(D_800EFD70.font, 0xBE, 0x2A);
    func_800C1E60(D_800EFD70.font, 0x40, 4, 2, 0x21);
    strcpy(buf, "Configuration ");
    buf[func_800AEC10(buf) + 1] = 0;
    buf[func_800AEC10(buf)] = D_800EFD70.cfg[D_800EFD70.ctl[D_800E37AC]] + 'A';
    func_800C1F2C(D_800EFD70.font, buf);
    func_800C0ACC(D_800EFD70.font, 0xEE, 0xC2);

    for (; i < 9; i++) {
        sel = i & 1;
        a3v = sel ? 4 : 6;
        lo = i < 5;
        v = lo ? 0x22 : 0x20;
        if (!sel) {
            v |= 0x40000000;
        }
        func_800C1E60(D_800EFD70.font, 0x40, 5, a3v, v);
        func_800C1F2C(D_800EFD70.font, D_800CDAA0[D_800CDACC[D_800EFD70.cfg[D_800EFD70.ctl[D_800E37AC]]][i]]);
        func_800C0ACC(D_800EFD70.font, D_800CDA70[i].x + 4, D_800CDA70[i].y + 5);
        if (D_800CDACC[D_800EFD70.cfg[D_800EFD70.ctl[D_800E37AC]]][i] == 6) {
            a3v = sel ? 4 : 6;
            v = lo ? 0x22 : 0x20;
            if (!sel) {
                v |= 0x40000000;
            }
            func_800C1E60(D_800EFD70.font, 0x40, 5, a3v, v);
            func_800C1F2C(D_800EFD70.font, D_800CDAC8);
            func_800C0ACC(D_800EFD70.font, D_800CDA70[i].x + 4, D_800CDA70[i].y + 0x10);
        }
    }
    for (i = 0; i < 3; i++) {
        func_800C1E60(D_800EFD70.font, 0x40, 5, (i == 2) ? 4 : 6, (i != 2) ? 0x40000020 : 0x20);
        func_800C1F2C(D_800EFD70.font, D_800CDABC[i]);
        func_800C0ACC(D_800EFD70.font, D_800CDA94[i].x + 4, D_800CDA94[i].y + 5);
    }
    gdl = func_800C1394(gdl);
    }
    return gdl;
}
