#include "game.h"

typedef struct { char c[16]; } Name16;
typedef struct { char c[4]; } Ext4;

typedef struct {
    s32 size;
    s32 magic;
    u16 company;
    Ext4 ext;
    Name16 name;
    s16 used;
    u8 data[0x20];
    char buf[0x30];
    u8 status;
    u8 pad[3];
} Rec_47;

typedef struct {
    u8 w;
    u8 pad[7];
} Glyph_2;

typedef struct {
    u32 hdr;
    u32 pal;
    Glyph_2 g[1];
} Font_2;

typedef struct {
    char **lines;
    char *text;
    char *extra;
} Msg;

struct Ctl { u8 b[0x68]; };

extern Rec_47 *D_800F4A10[];
extern Rec_47 D_800CE168;
extern Font_2 *D_800F4A58;
extern Msg D_800CE068[];
extern Ctl D_800E1C78_2[4];

extern Rec_47 *D_800F48E0[];

void func_80088500(s32 p, s32 z)
{
    s32 *cnt;
    s32 i;
    s32 r;
    s32 j;
    Rec_47 *rec;

    D_800F4AE0[p] = 0;
    if (z != 0) {
        D_800F48E0[p * 17] = &D_800CE168;
        D_800F4AE0[p] = 1;
    }
    for (i = 0; i < 16; i++) {
        rec = &D_800F4A10[p][i];
        if (rec->used != -1) {
            if (rec->magic != 0x4E4D5847) {
                r = 1;
            } else {
                r = D_800F4A28_2.fn(rec->data, D_800F47D0, D_800F4A28_2.v == 0);
                if (r == 2) {
                    if (D_800F4A28_2.v == 1) {
                        continue;
                    }
                    r = 1;
                }
            }
            if (r != 1 || D_800F4A28_2.v != 1) {
                func_8000ED10((u8 *)rec->buf, 0, 0x30);
                *(Name16 *)rec->buf = rec->name;
                if (rec->ext.c[0] != 0) {
                    func_8000ED30(rec->buf, D_800DA708);
                    *(Ext4 *)&rec->buf[func_800AEC10(rec->buf)] = rec->ext;
                }
                if (r != 1) {
                    func_8000ED30(rec->buf, D_800DA70C);
                    func_8000ED30(rec->buf, D_800F47D0);
                }
                rec->status = r;
                j = p * 17; j += D_800F4AE0[p];
                D_800F48E0[j] = rec;
                D_800F4AE0[p]++;
            }
        }
    }
}

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

void func_80088AE0(void)
{
    s32 *g;
    s32 wA, hA, wB, hB, wC, wD, hC, hD;
    f32 fa, fb, fc, fd;

    D_800F4A08 = 0;
    D_800F4A5C = 0;
    D_800E2004 = 0;
    D_800F4A78 = -1;
    D_800F4A7C = -1;
    D_800F4A04 = 0;
    D_800F483C = -1;
    D_800F4AF0 = 0;
    D_800F4A28_3[1] = 0;
    D_800F4A28_3[2] = 0;
    D_800F4A28_3[3] = 0;
    D_800F4A28_3[4] = 0;
    D_800F4A28_3[5] = 0;
    D_800F4A28_3[6] = 0;
    g = &D_800F49F8;
    *g = D_800F47C0;
    func_8008C6D0();
    wA = *(volatile s32 *)&D_800F13EC;
    hA = *(volatile s32 *)&D_800F13F0;
    wB = *(volatile s32 *)&D_800F13EC;
    hB = *(volatile s32 *)&D_800F13F0;
    wC = *(volatile s32 *)&D_800F13EC;
    fa = (f32)-(wC / 2);
    wD = *(volatile s32 *)&D_800F13EC;
    fb = (f32)(wD / 2);
    hC = *(volatile s32 *)&D_800F13F0;
    fc = (f32)-(hC / 2);
    hD = *(volatile s32 *)&D_800F13F0;
    fd = (f32)(hD / 2);
    D_800F4868[0] = (s16)(wA << 1);
    D_800F4868[1] = (s16)(hA << 1);
    D_800F4868[4] = (s16)(wB << 1);
    D_800F4868[5] = (s16)(hB << 1);
    func_800AE240((s32 *)D_800F4A88, fa, fb, fc, fd, 50.0f, 16384.0f, 1.0f);
    D_800F4A20 = func_800BF79C(*g, D_800D13A0);
    D_800F49F0 = func_800BF79C(*g, D_800D1488);
    D_800F49F4 = func_800BF79C(*g, D_800D14A8);
    D_800F4A60 = func_800BF79C(*g, D_800D1418);
    D_800F4A58 = func_800BF79C(*g, D_800D1434);
    D_800CE164 = func_800C1E60((s32 *)0, 0x80, 1, 4, 0);
}

typedef struct {
    u32 n;
    u32 pal;
    Img g[1];
} Font_3;

typedef struct {
    u32 size;
    u8 pad[0x3C];
    char name[1];
} Entry_6;

typedef struct {
    s32 mode;
    s32 msg;
    s32 f8;
    s32 pak;
    s32 sel;
    s32 f14;
    s32 scroll;
} PakState;

extern Font_3 *D_800F49F0_2;
extern Font_3 *D_800F49F4_2;
extern Font_3 *D_800F4A20_2;
extern Font_3 *D_800F4A60_2;
extern PakState D_800F4A28_4;
extern Entry_6 *D_800F48E0_2[4][17];

static inline Gfx *drawStr_2(Gfx *gdl, u8 *str, s32 p3, s32 p4, s32 x, s32 y)
{
    s32 i;

    if (p3 == 2) {
        for (i = 0; str[i] != 0; i++) {
            if (str[i] == 0x40) {
                str[i] = 0x7F;
            }
        }
    }
    func_800C1E60(D_800CE164_2, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164_2, (char *)str);
    func_800C0ACC(D_800CE164_2, x, y);
    gdl = func_800C1394(gdl);
    return gdl;
}

static inline Gfx *drawCtr(Gfx *gdl, char *str, s32 p3, s32 p4, s32 cx, s32 y)
{
    func_800C1E60(D_800CE164_2, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164_2, str);
    func_800C0ACC(D_800CE164_2, cx - (D_800CE164_2[6] >> 1), y);
    return func_800C1394(gdl);
}

void func_80088CF8(s32 arg0, s32 arg1, Gfx **pgdl)
{
    Gfx *gdl;
    s32 c;
    s32 i;
    s32 j;
    s32 k;
    s32 x;
    s32 y;
    s32 col;
    u32 n;
    Entry_6 *e;
    char **lines;
    char **p;
    s32 w;
    s32 h;
    s32 ly;

    gdl = *pgdl;
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    func_800C2224();
    gDPLoadTLUT_pal256(gdl++, D_800F49F0_2->pal);
    gdl = func_8000A690(0, 0, &D_800F49F0_2->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPLoadTLUT_pal256(gdl++, D_800F4A60_2->pal);
    gdl = func_8000A690(0x4E, 0x16, &D_800F4A60_2->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gdl = func_8000A690(0x4E, 0x13, &D_800F49F4_2->g[3], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    c = 0xFF;
    if (D_800F4A28_4.msg == 0) {
        c = 0x80;
    }
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = func_8000A690(0x2B, 0x32, &D_800F49F4_2->g[2], gdl, arg1, 1, 1, 6, 0);
    c ^= 0x7F;
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = func_8000A690(0x28, 0x95, &D_800F49F4_2->g[1], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    for (j = 0; j < 0x200; j += 0x10) {
        gdl = func_8000A690(j, 0xB4, &D_800F49F4_2->g[0], gdl, arg1, 1, 1, 6, 0);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, D_800F4A20_2->pal);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
    for (i = 0; i < 4; i++) {
        if ((D_800C292C >> i) & 1) {
            c = 0x80;
            if ((D_800E2004 | D_800E2008) & (1 << i)) {
                c = 0xFF;
            }
            gDPSetColor(gdl++, G_SETPRIMCOLOR, c | 0xFFFFFF00);
            gdl = func_8000A690(0x37 + i * 100, 0x9C, &D_800F4A20_2->g[(u32)(D_800F4A78 / 2) % D_800F4A20_2->n], gdl,
                                arg1, 1, 1, 3, 0);
        }
    }
    gdl = drawStr_2(gdl, (u8 *)D_800CDEF0[D_800F4A28_4.mode], 1, 2, 0x62, 0x1D);
    __builtin_strcpy((char *)D_800F47D0, "Pak  ");
    for (i = 0; i < 4; i++) {
        if ((D_800E2004 | D_800E2008) & (1 << i)) {
            D_800F47D0[4] = i + '1';
            gdl = drawStr_2(gdl, D_800F47D0, 1, (i == D_800F4A28_4.pak) ? 4 : 2, 0x55 + i * 100, 0x9F);
        }
    }
    if (D_800E2004 & (1 << D_800F4A28_4.pak)) {
        for (i = 0; i < D_800F4AE0[D_800F4A28_4.pak]; i++) {
            c = i - D_800F4A28_4.scroll;
            if (c >= 0) if (c < 8) {
                e = D_800F48E0_2[D_800F4A28_4.pak][i];
                x = 0x58 - c * 5 / 2;
                y = c * 10 + 0x3B;
                func_8000EF8C((char *)D_800F47D0, e->name);
                if (D_800F4A28_4.msg == 0) {
                    c = 6;
                } else if (i == D_800F4A28_4.sel) {
                    c = 5;
                } else if (e == &D_800CE168) {
                    c = 7;
                    w = 0;
                } else {
                    c = 2;
                }
                gdl = drawStr_2(gdl, D_800F47D0, 2, c, x, y);
                if (e != &D_800CE168) {
                    n = e->size;
                    n += 0xFF;
                    n >>= 8;
                    func_8000EE30(n, (char *)D_800F47D0);
                    func_800C1E60(D_800CE164_2, 0x80, 2, c, 0);
                    func_800C1F2C(D_800CE164_2, (char *)D_800F47D0);
                    func_800C0ACC(D_800CE164_2, x + (0x140 - D_800CE164_2[6]), y);
                    gdl = func_800C1394(gdl);
                    __builtin_strcpy((char *)D_800F47D0, "page");
                    if (n != 1) {
                        func_8000ED30((char *)D_800F47D0, "s");
                    }
                    gdl = drawStr_2(gdl, D_800F47D0, 2, c, x + 0x14A, y);
                }
            }
        }
    } else if (D_800E2008 & (1 << D_800F4A28_4.pak)) {
        gdl = drawCtr(gdl, "Damaged or Corrupted Controller Pak!", 2, 6, 0x100, 0x5C);
    }
    if (D_800F4A28_4.mode == 2 || D_800F4A28_4.mode == 0) {
        if (D_800E2004 & (1 << D_800F4A28_4.pak)) {
            func_8000EE30(D_800F4A68[D_800F4A28_4.pak], (char *)D_800F47D0);
            func_8000ED30((char *)D_800F47D0, " page");
            if (D_800F4A68[D_800F4A28_4.pak] != 1) {
                func_8000ED30((char *)D_800F47D0, "s");
            }
            func_8000ED30((char *)D_800F47D0, " free");
            gdl = drawCtr(gdl, (char *)D_800F47D0, 1, 2, 0x18B, 0x1E);
        }
    }
    if (D_800CE068[D_800F4A28_4.msg].lines != NULL) {
        lines = D_800CE068[D_800F4A28_4.msg].lines;
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xC0);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, 0xB4);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
        }
        h = i * 14 + 0x50;
        gDPFillRectangle(gdl++, 0, 0x4C, 0x200, h);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPLoadTextureBlock(gdl++, D_800F49F4_2->g[0].addr, G_IM_FMT_IA, G_IM_SIZ_8b, D_800F49F4_2->g[0].w * 8,
                            D_800F49F4_2->g[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 4, 8, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, 0, 0x114, 0x800, 0x130, 0, 0, 0, 0x400, 0x400);
        gSPTextureRectangle(gdl++, 0, (h - 1) * 4, 0x800, (h + 7) * 4, 0, 0, 0x5C0, 0x400, 0x400);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
            gdl = drawCtr(gdl, lines[i], 1, 2, 0x100, 0x53 + i * 14);
        }
        if (D_800CE068[D_800F4A28_4.msg].extra != NULL) {
            w = (s32)func_80088734(NULL, (u8 *)D_800CE068[D_800F4A28_4.msg].extra, 1, 4, 0, 0);
            gdl = func_80088734(gdl, (u8 *)D_800CE068[D_800F4A28_4.msg].extra, 1, 4, 0x100 - w / 2, 0xC6);
        }
    } else if (D_800CE068[D_800F4A28_4.msg].text != NULL) {
        gdl = drawCtr(gdl, D_800CE068[D_800F4A28_4.msg].text, 1, 2, 0x100, 0xBE);
        if (D_800CE068[D_800F4A28_4.msg].extra != NULL) {
            w = (s32)func_80088734(NULL, (u8 *)D_800CE068[D_800F4A28_4.msg].extra, 1, 4, 0, 0);
            gdl = func_80088734(gdl, (u8 *)D_800CE068[D_800F4A28_4.msg].extra, 1, 4, 0x100 - w / 2, 0xCE);
        }
    }
    if (D_800F4AF0 != 0) {
        D_800F4AF0--;
    }
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void func_80089E30(s32 p)
{
    s32 n;
    s32 r;
    s32 bit;
    s32 *q;

    n = 3;
    do {
        if (func_8008B7B4(p) == 0) {
            break;
        }
        n = n - 1;
    } while (n != 0);
    if (n == 0) {
        D_800E2008 = D_800E2008 | (1 << p);
        D_800E2004 = D_800E2004 & ~(1 << p);
    }
    n = 3;
    do {
        r = func_8008C068(p);
        if (r < 0) {
            r = r + 0xFF;
        }
        D_800F4A68[p] = r >> 8;
    } while ((r >> 8) < 0 && (n = n - 1) != 0);
    if (n == 0) {
        D_800E2008 = D_800E2008 | (1 << p);
        D_800E2004 = D_800E2004 & ~(1 << p);
    }
    func_80088500(p, 0);
}

typedef struct {
    u32 size;
    u32 code;
    u16 company;
    u8 ext;
    u8 padB[0x1E - 0xB];
    s16 fileNo;
    u8 pad20[0x74 - 0x20];
} Entry_7;

typedef struct {
    Entry_7 *e[16];
} EntryRow;

typedef struct {
    s32 mode;
    s32 state;
    s32 sub;
    s32 pak;
    s32 sel;
    s32 f14;
    s32 scroll;
    s32 pages;
    s32 f20;
    u8 *buf;
    s32 (*cb)(u8 *, s32);
    s32 f2C;
} PakState_2;

extern PakState_2 D_800F4A28_5;
extern PakState_2 D_800F4838;
extern Entry_7 *D_800F4A10_2[];
extern Entry_7 *D_800F48E0_3[4][17];

static inline void SET(s32 s, s32 m) {
    D_800F4A28_5.state = s;
    D_800F4A28_5.sub = 0;
    D_800F4A00 = m;
}

static inline void SND(s32 m) {
    D_800F4A00 = m;
}

static inline void SET2(s32 s) {
    D_800F4A28_5.state = s;
    D_800F4A28_5.sub = 0;
}

static inline void compact(s32 pak) {
    if (D_800F48E0_3[pak][0] == &D_800CE168 && D_800F4AE0[pak] > 0) {
        *(EntryRow *)&D_800F48E0_3[pak][0] = *(EntryRow *)&D_800F48E0_3[pak][1];
        D_800F4AE0[pak]--;
    }
}

void func_80089F2C(Pad16 *arg0) {
    u8 st[8];
    u32 pad;
    u32 prev;
    s32 i;
    s32 n;
    s32 j;
    s32 cnt;
    s32 bit;
    Entry_7 *e;

    if (D_800F4A04 != 0) {
        return;
    }
    D_800F4A00 = -1;
    D_800CE068[5].text = D_800CE05C[D_800F4A28_5.mode];
    if (D_800F4A28_5.mode == 0) {
        D_800CE068[12].lines = D_800CDF64;
    } else if (D_800F01F8 == 1) {
        D_800CE068[12].lines = D_800CDF6C;
    } else {
        D_800CE068[12].lines = D_800CDF78;
    }
    if (D_800F4A28_5.mode == 2) {
        D_800CE068[0].extra = D_800CE034;
    } else {
        D_800CE068[0].extra = D_800CE01C;
    }
    pad = D_800E30B0;
    if (pad & 0x2000) {
        pad |= 4;
    }
    if (pad & 0x4000) {
        pad |= 8;
    }
    if (pad & 0x800) {
        pad |= 1;
    }
    if (pad & 0x1000) {
        pad |= 2;
    }
    i = D_800F49FC;
    D_800F49FC = pad;
    pad &= ~i;
    D_800F4A78++;
    if (D_800F4A78 % 15 == 0 && func_8008CA68_2(st) == 0 && st[0] != D_800F4A5C) {
        for (i = 0; i < 4; i++) {
            if (((st[0] ^ D_800F4A5C) >> i) & 1) {
                D_800F4A08 &= ~(1 << i);
                func_8008B9BC(i);
                if (D_800E2004 & (1 << i)) {
                    func_80089E30(i);
                } else {
                    func_8000ED10((u8 *)D_800F4A10_2[i], 0, 0x740);
                }
                if ((D_800E2008 & (1 << i)) && (D_800F4A28_5.state == 0 || D_800F4A28_5.state == 14)) {
                    D_800F4A28_5.pak = i;
                    SET(1, 3);
                }
            }
        }
        D_800F4A5C = st[0];
    }
    D_800E2008 &= D_800C292C;
    D_800E2004 &= D_800C292C;
    D_800E1C60 &= D_800C292C;
    D_800F4A08 &= D_800E2008;
    if (D_800F4A28_5.state == 0 || D_800F4A28_5.state == 14) {
        for (i = 0; i < 4; i++) {
            n = (D_800F4A28_5.pak + i) % 4;
            if ((D_800E2008 & (1 << n)) && !((D_800F4A08 >> n) & 1)) {
                D_800F4A28_5.pak = n;
                SET(1, 3);
            }
        }
    }
    if (D_800F4A28_5.state == 5 && D_800F4AE0[D_800F4A28_5.pak] == 0) {
        D_800F4A28_5.state = 0;
        D_800F4A28_5.sub = 0;
    }
    if ((D_800F4A28_5.state < 14 || D_800F4A28_5.state > 16) && D_800F4A28_5.state != 20) {
        if ((D_800E2004 | D_800E2008) == 0 && D_800F4A28_5.state != 14) {
            D_800F4A28_5.state = 14;
            D_800F4A28_5.sub = 0;
        } else if (!((D_800E2004 | D_800E2008) & (1 << D_800F4A28_5.pak)) && D_800F4A28_5.state != 0) {
            D_800F4A28_5.state = 0;
            D_800F4A28_5.sub = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        if (!((D_800E2008 | D_800E2004) & (1 << D_800F4A28_5.pak))) {
            D_800F4A28_5.pak = (D_800F4A28_5.pak + 1) % 4;
        }
    }
    if (D_800F4838.state != -1 && D_800F4A28_5.state == 0 && D_800F4A28_5.mode == 0) {
        D_800F4A28_5 = D_800F4838;
        D_800F4838.state = -1;
        D_800F4A28_5.state = 0;
        D_800F4A28_5.sub = 0;
    }
    switch (D_800F4A28_5.state) {
    case 0:
        compact(D_800F4A28_5.pak);
        D_800F4A28_5.sel = 0;
        D_800F4A28_5.scroll = 0;
        if (pad & 4) {
            for (i = D_800F4A28_5.pak - 1; i >= 0; i--) {
                if ((D_800E2004 | D_800E2008) & (1 << i)) {
                    break;
                }
            }
            if (i < 0) {
                D_800F4A00 = 3;
            } else {
                D_800F4A28_5.pak = i;
                D_800F4A00 = 0;
            }
        } else if (pad & 8) {
            for (i = D_800F4A28_5.pak + 1; i < 4; i++) {
                if ((D_800E2004 | D_800E2008) & (1 << i)) {
                    break;
                }
            }
            if (i >= 4) {
                D_800F4A00 = 3;
            } else {
                D_800F4A28_5.pak = i;
                D_800F4A00 = 0;
            }
        }
        if (pad & 0x20) {
            if (D_800F4A28_5.mode == 1) {
                SET(18, 2);
            } else if (D_800F4A28_5.mode == 2) {
                SET(17, 2);
            } else {
                D_800F4AF4 = 0;
                if (D_800CDE70 != 0) {
                    D_800F4A04 = 1;
                    D_800F4A00 = 1;
                    D_800CDE70 = 0;
                    break;
                }
                SET(20, 2);
            }
        } else if (pad & 0x10) {
            if (D_800E2004 & (1 << D_800F4A28_5.pak)) {
                n = 0;
                for (i = 0; i < 16; i++) {
                    if (D_800F4A10_2[D_800F4A28_5.pak][i].fileNo != -1) {
                        n++;
                    }
                }
                D_800F4A28_5.sel = 0;
                if (D_800F4A28_5.mode == 2) {
                    if (n == 16) {
                        SET(8, 1);
                    } else {
                        func_80088500(D_800F4A28_5.pak, 1);
                        SET(5, 1);
                    }
                } else if (D_800F4A28_5.mode == 0 && n == 0) {
                    SET(12, 3);
                } else if (D_800F4A28_5.mode == 1 && D_800F4AE0[D_800F4A28_5.pak] == 0) {
                    SET(12, 3);
                } else {
                    SET(5, 1);
                }
            } else if (D_800E2008 & (1 << D_800F4A28_5.pak)) {
                SET(1, 3);
            }
        }
        break;
    case 5:
        if (D_800F4A28_5.sel >= D_800F4AE0[D_800F4A28_5.pak]) {
            D_800F4A28_5.sel = D_800F4AE0[D_800F4A28_5.pak] - 1;
        }
        if (D_800F4A28_5.sel < 0) {
            D_800F4A28_5.sel = 0;
        }
        j = D_800F4A28_5.sel;
        n = 0;
        if (pad & 2) {
            n = 1;
        }
        i = j;
        if (pad & 1) {
            n = -1;
        }
        j += n;
        cnt = D_800F4AE0[D_800F4A28_5.pak];
        do {
            if (j < 0) {
                if (n != 0) {
                    j = i;
                    goto done;
                }
                j = D_800F4AE0[D_800F4A28_5.pak] - 1;
            }
            if (j >= D_800F4AE0[D_800F4A28_5.pak]) {
                if (n == 0) {
                    j = 0;
                } else {
                    j = i;
                    goto done;
                }
            }
            if (D_800F4A28_5.mode != 2 || D_800F48E0_3[D_800F4A28_5.pak][j] == &D_800CE168 ||
                D_800F48E0_3[D_800F4A28_5.pak][j]->code == 0x4E4D5847) {
                goto done;
            }
            j += n;
        } while (n != 0 || (n = 1, --cnt != 0));
    done:
        D_800F4A28_5.sel = j;
        if (j < D_800F4A28_5.scroll) {
            D_800F4A28_5.scroll = j;
        }
        if (D_800F4A28_5.scroll < j - 7) {
            D_800F4A28_5.scroll = j - 7;
        }
        if (D_800F4AE0[D_800F4A28_5.pak] - 8 < D_800F4A28_5.scroll) {
            D_800F4A28_5.scroll = D_800F4AE0[D_800F4A28_5.pak] - 8;
        }
        if (D_800F4A28_5.scroll < 0) {
            D_800F4A28_5.scroll = 0;
        }
        if (pad & 3) {
            if (i == j) {
                D_800F4A00 = 3;
            } else {
                D_800F4A00 = 0;
            }
        }
        if (pad & 0x20) {
            SET(0, 2);
        }
        if (pad & 0x10) {
            if (D_800F4A28_5.mode == 0) {
                SET(7, 1);
            } else if (D_800F4A28_5.mode == 2) {
                e = D_800F48E0_3[D_800F4A28_5.pak][D_800F4A28_5.sel];
                if (e == &D_800CE168) {
                    if (D_800F4A68[D_800F4A28_5.pak] < D_800F4A28_5.pages) {
                        SET(8, 1);
                        break;
                    }
                } else {
                    j = (e->size + 0xFF) >> 8;
                    if (D_800F4A68[D_800F4A28_5.pak] + j < D_800F4A28_5.pages) {
                        SET(8, 1);
                        break;
                    }
                }
                if (e != &D_800CE168) {
                    SET(6, 1);
                } else {
                    SET(10, 1);
                }
            } else {
                SET(9, 1);
            }
        }
        break;
    case 7:
        if (pad & 0x20) {
            SET(5, 2);
        }
        if (pad & 0x10) {
            SET(13, 1);
        }
        break;
    case 13:
        switch (D_800F4A28_5.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28_5.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28_5.sub = 2;
            }
            break;
        case 2:
            if (func_8008C1FC(D_800F4A28_5.pak, D_800F48E0_3[D_800F4A28_5.pak][D_800F4A28_5.sel]->fileNo) != 0) {
                SET(11, 3);
            } else if (D_800F4A28_5.mode != 0) {
                func_80089E30(D_800F4A28_5.pak);
                SET(0, 1);
            } else {
                func_80089E30(D_800F4A28_5.pak);
                if (D_800F48E0_3[D_800F4A28_5.pak] == NULL) {
                    SET(0, 1);
                } else {
                    SET(5, 1);
                }
            }
            break;
        }
        break;
    case 1:
        D_800CDF00[0][0x11] = D_800F4A28_5.pak + '1';
        if (pad & 0x10) {
            SET(2, 1);
        }
        if (pad & 0x20) {
            SND(2);
            SET2(0);
            D_800F4A08 |= 1 << D_800F4A28_5.pak;
        }
        break;
    case 12:
        if (pad & 0x10) {
            SET(0, 1);
        }
        break;
    case 2:
        switch (D_800F4A28_5.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28_5.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28_5.sub = 2;
            }
            break;
        case 2:
            if (func_8008C55C(D_800F4A28_5.pak) != 0) {
                SET(4, 3);
            } else {
                D_800F4A28_5.state = 3;
                D_800F4A00 = 1;
                D_800F4A28_5.sub = 0;
                D_800F4A5C &= ~(1 << D_800F4A28_5.pak);
            }
            break;
        }
        break;
    case 4:
        if (pad & 0x10) {
            SET(2, 1);
        }
        if (pad & 0x20) {
            SET(0, 1);
            D_800F4A08 |= 1 << D_800F4A28_5.pak;
        }
        break;
    case 3:
        if (pad & 0x10) {
            SET(0, 1);
        }
        break;
    case 6:
        if (pad & 0x20) {
            SET(5, 2);
        }
        if (pad & 0x10) {
            e = D_800F48E0_3[D_800F4A28_5.pak][D_800F4A28_5.sel];
            if (func_8008C1FC(D_800F4A28_5.pak, e->fileNo) != 0) {
                SET(11, 3);
            } else {
                if (e->code == 0x4E4D5847) {
                    D_800F4A7C = e->ext;
                } else {
                    D_800F4A7C = -1;
                }
                SET(10, 1);
            }
        }
        break;
    case 11:
        if (pad & 0x10) {
            SET(5, 1);
            D_800F4A5C = 0;
            D_800E2004 = 0;
            D_800F4A78 = -1;
        }
        break;
    case 14:
        if (D_800E2008 | D_800E2004) {
            D_800F4A28_5.state = 0;
            D_800F4A28_5.sub = 0;
        } else if (pad & 0x20) {
            D_800F4AF4 = 1;
            D_800F4A04 = 1;
            D_800F4A00 = 2;
        }
        break;
    case 9:
        switch (D_800F4A28_5.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28_5.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28_5.sub = 2;
            }
            break;
        case 2:
            e = D_800F48E0_3[D_800F4A28_5.pak][D_800F4A28_5.sel];
            if (D_800F4A28_5.pages < (e->size + 0xFF) >> 8) {
                SET(19, 3);
                return;
            }
            func_8000ED10(D_800F4A28_5.buf, 0, D_800F4A28_5.pages << 8);
            if (func_8008C2BC(D_800F4A28_5.pak, e->fileNo, 0, (e->size + 0xFF) & ~0xFF, D_800F4A28_5.buf) != 0) {
                SET(11, 3);
                return;
            }
            if (D_800F4A28_5.cb != NULL && D_800F4A28_5.cb(D_800F4A28_5.buf, (e->size + 0xFF) & ~0xFF) != 0) {
                SET(19, 3);
                return;
            }
            SET2(16);
            break;
        }
        break;
    case 10:
        switch (D_800F4A28_5.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28_5.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28_5.sub = 2;
            }
            break;
        case 2:
            if (D_800F4A7C == -1) {
                n = 'A';
                for (;;) {
                    for (i = 0; i < D_800F4AE0[D_800F4A28_5.pak]; i++) {
                        e = D_800F48E0_3[D_800F4A28_5.pak][i];
                        if (e->code == 0x4E4D5847 && e->ext == n) {
                            break;
                        }
                    }
                    if (i == D_800F4AE0[D_800F4A28_5.pak]) {
                        break;
                    }
                    if (++n > 'Z') {
                        break;
                    }
                }
            } else {
                n = D_800F4A7C;
            }
            D_800F4A7C = -1;
            func_8000ED10(D_800F47D0, 0, 4);
            D_800F47D0[0] = n;
            j = func_8008C0E4(D_800F4A28_5.pak, D_800CDEFC, D_800F47D0, 0x3031, 0x4E4D5847, D_800F4A28_5.pages << 8);
            if (j < 0) {
                SET(11, 3);
            }
            if (func_8008C370(D_800F4A28_5.pak, j, 0, D_800F4A28_5.pages << 8, D_800F4A28_5.buf) != 0) {
                func_8008C1FC(D_800F4A28_5.pak, j);
                SET(11, 3);
            }
            D_800F4A80 = func_800BC908(D_800F4A28_5.pages << 8, 0, "pak.c", 0x5E6);
            n = func_8008C2BC(D_800F4A28_5.pak, j, 0, D_800F4A28_5.pages << 8, D_800F4A80);
            if (n == 0) {
                for (i = 0; i < D_800F4A28_5.pages << 8; i++) {
                    if (D_800F4A80[i] != D_800F4A28_5.buf[i]) {
                        n = 1;
                    }
                }
            }
            func_800BCB04(D_800F4A80);
            if (n != 0) {
                func_8008C1FC(D_800F4A28_5.pak, j);
                SET(11, 3);
            }
            func_80089E30(D_800F4A28_5.pak);
            D_800F4A28_5.state = 15;
            D_800F4A28_5.sub = 0;
            break;
        }
        break;
    case 19:
        if (pad & 0x10) {
            SET(7, 1);
        } else if (pad & 0x20) {
            SET(0, 1);
            D_800F4A5C = 0;
            D_800E2004 = 0;
            D_800F4A78 = -1;
        }
        break;
    case 8:
        if (pad & 0x20) {
            SET(0, 2);
        } else if (pad & 0x10) {
            D_800F4838 = D_800F4A28_5;
            D_800F4A28_5.mode = 0;
            func_80088500(D_800F4A28_5.pak, 0);
            compact(D_800F4A28_5.pak);
            SET(5, 1);
        }
        break;
    case 17:
        D_800F4AF4 = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 18:
        D_800F4AF4 = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 15:
    case 16:
        D_800F4AF4 = 0;
    case 20:
        if (pad & 0x10) {
            D_800F4A04 = 1;
            D_800F4A00 = 1;
        }
        break;
    }
    if (D_800F4A04 != 0 && D_800F4838.state != -1) {
        D_800F4A28_5 = D_800F4838;
        D_800F4838.state = -1;
        D_800F4A28_5.state = 0;
        D_800F4A28_5.sub = 0;
        D_800F4A04 = 0;
    }
    if (D_800F4A00 != -1) {
        func_8001102C(D_800F4A00);
    }
    if (D_800F4A04 != 0) {
        func_8008BC30();
        if (D_800F4A04 != 0) {
            D_800CDE70 = 0;
        }
    }
}

void func_8008B6DC(void)
{
    func_800BFEA4((u32 *)(D_800F49F8_2[1] + D_800D13A0 * 4));
    func_800BFEA4((u32 *)(D_800F49F8_2[1] + D_800D1488 * 4));
    func_800BFEA4((u32 *)(D_800F49F8_2[1] + D_800D14A8 * 4));
    func_800BFEA4((u32 *)(D_800F49F8_2[1] + D_800D1418 * 4));
    func_800BFEA4((u32 *)(D_800F49F8_2[1] + D_800D1434 * 4));
    func_800C204C(D_800CE164);
    func_800BCB04(D_800CE164);
    func_8008C744();
    func_8008BC30();
}

typedef struct { u32 w[8]; } Hdr_10;

struct Slot_13 {
    u32 f0;
    u32 magic;
    u8 p8[2];
    u8 a[4];
    u8 b[0x10];
    s16 id;
    Hdr_10 hdr;
    u8 rest[0x74 - 0x40];
};

s32 func_8008B7B4(u32 p)
{
    Hdr_10 tmp;
    s32 i, k, r;

    if (!(D_800E2004 & (1 << p))) {
        return -1;
    }
    func_8000CD00();
    for (i = 0; i < 16; i++) {
        D_800F4A10_3[p][i].id = -1;
    }
    k = 0;
    i = 0;
    do {
        r = func_800B44D0(&D_800E1C78_2[p], i, &D_800F4A10_3[p][k]);
        if (r == 5) {
            func_8000ED10((u8 *)&D_800F4A10_3[p][k], 0, 0x74);
            D_800F4A10_3[p][k].id = -1;
        } else if (r == 0) {
            D_800F4A10_3[p][k].id = i;
            func_8008C670(D_800F4A10_3[p][k].b, 0x10);
            func_8008C670(D_800F4A10_3[p][k].a, 4);
            if (D_800F4A10_3[p][k].magic == 0x4E4D5847) {
                if (func_8008C2BC(p, i, 0, 0x20, (u8 *)&tmp) != 0) {
                    D_800F4A10_3[p][k].magic = 0;
                }
                D_800F4A10_3[p][k].hdr = tmp;
            }
            k++;
        } else {
            return -2;
        }
        i++;
    } while (i < 16);
    return 0;
}

struct MQ { u8 b[0x20]; };
struct CStat { u16 type; u8 status; u8 errnum; };

extern CStat D_800E1C68_2[4];

void func_8008B9BC(s32 chan)
{
    u32 bit = 1 << chan;
    s32 msg[2];
    s32 i;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(&D_800E1C48);
    func_800B0610(&D_800E1C48, msg, 1);
    func_800ADA40(D_800E1C68_2);
    D_800C292C = 0;
    D_800C292D = 0;
    for (i = 0; i < 4; i++) {
        if (D_800E1C68_2[i].errnum == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C |= 1 << i;
            D_800C292D++;
        }
    }
    if ((D_800C292C & bit) == 0) {
        return;
    }
    i = chan;
    D_800E2004 &= ~bit;
    D_800E1C60 &= ~bit;
    D_800E2008 &= ~bit;
    switch (func_800B4D20(&D_800E1C48, &D_800E1C78_2[i], i)) {
    case 0:
        D_800E2004 |= bit;
        break;
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        break;
    case 11:
        if (func_800B3704(&D_800E1C48, &D_800E1E60[i], i) != 0) {
            if (func_800B33C0(&D_800E1C48, D_800F4878, i) != 0) {
                D_800E2008 |= 1 << i;
            }
        } else {
            func_800B35C0(&D_800E1E60[i], 0);
            D_800E1C60 |= bit;
        }
        break;
    case 10:
        if (func_800B3704(&D_800E1C48, &D_800E1E60[i], i) == 0) {
            func_800B35C0(&D_800E1E60[i], 0);
            D_800E1C60 |= bit;
        } else {
            D_800E2008 |= bit;
        }
        break;
    }
}

typedef struct {
    u8 f0[3];
    u8 f3;
} Pad4;

extern Pad4 D_800E1C68_3[];

void func_8008BC30(void)
{
    u32 buf[2];
    s32 i;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(D_800E1C48);
    func_800B0610(D_800E1C48, buf, 1);
    func_800ADA40(D_800E1C68_3);
    D_800C292C = 0;
    D_800C292D = 0;
    for (i = 0; i < 4; i++) {
        if (D_800E1C68_3[i].f3 == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C |= 1 << i;
            D_800C292D++;
        }
    }
    D_800E2020 = 0;
    D_800E2004 = 0;
    D_800E1C60 = 0;
    D_800E2008 = 0;
    for (i = 0; i < 4; i++) {
        D_800E2010[i] = 0;
        if ((D_800C292C >> i) & 1) {
            switch (func_800B4D20(D_800E1C48, &D_800E1C78_3[i], i)) {
            case 0:
                D_800E2004 |= 1 << i;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                break;
            case 11:
                if (func_800B3704(D_800E1C48, &D_800E1E60[i], i) == 0) {
                    func_800B35C0(&D_800E1E60[i], 0);
                    D_800E1C60 |= 1 << i;
                } else if (func_800B33C0(D_800E1C48, D_800F4878, i) != 0) {
                    D_800E2008 |= 1 << i;
                }
                break;
            case 10:
                if (func_800B3704(D_800E1C48, &D_800E1E60[i], i) == 0) {
                    func_800B35C0(&D_800E1E60[i], 0);
                    D_800E1C60 |= 1 << i;
                } else {
                    D_800E2008 |= 1 << i;
                }
                break;
            }
        }
    }
    D_800C2928 = func_800AC810(D_800E1C48);
}

void func_8008BE9C(void)
{
    s32 buf[2];
    s32 k;
    u8 *p;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(D_800E1C48);
    func_800B0610(D_800E1C48, buf, 1);
    func_800ADA40(D_800E1C68);
    k = 0;
    D_800C292C = 0;
    D_800C292D = 0;
    p = D_800E1C68;
    do {
        if (p[3] == 0) {
            if (D_800C292C == 0) {
                D_800C292E = k;
            }
            D_800C292C = D_800C292C | (1 << k);
            D_800C292D = D_800C292D + 1;
        }
        k++;
        p += 4;
    } while (k < 4);
}

void func_8008BF60(void)
{
    s32 i;
    s32 a;
    s32 b;
    s32 c;
    s32 x;

    func_8008BC30();
    for (i = 0; i < 4; i++) {
        D_800F4AC8[i] = -2;
        if ((D_800E2004 & (1 << i)) && func_800B4C20(&D_800E1C78[i * 0x68], &a, &b) == 0) {
            if (!(D_800E2004 & (1 << i))) {
                x = -1;
            } else {
                func_8000CD00();
                if (func_800B4B10(&D_800E1C78[i * 0x68], &c) != 0) {
                    x = -2;
                } else {
                    x = c;
                }
            }
            x &= ~0xFF;
            if (b == 0x10) {
                D_800F4AC8[i] = -1;
            } else {
                D_800F4AC8[i] = x / 256;
            }
        }
    }
}

int func_8008C068(u32 param_1)
{
    int local_10[2];

    if ((D_800E2004 & (1 << param_1)) == 0) {
        return -1;
    }
    func_8000CD00();
    if (func_800B4B10((u32 *)(D_800E1C78 + param_1 * 0x68), local_10) != 0) {
        return -2;
    }
    return local_10[0];
}

s32 func_8008C0E4(u32 param_1, char *param_2, char *param_3, s32 param_4, s32 param_5, s32 param_6)
{
    s32 iVar2;
    u8 acStack_38[16];
    u8 acStack_28[8];
    s32 local_20[2];

    if ((D_800E2004 & (1 << param_1)) == 0) {
        iVar2 = -1;
    } else {
        func_8000CD00();
        func_8000ED10(acStack_28, 0, 4);
        func_8000EF8C(acStack_28, param_3);
        func_8000ED10(acStack_38, 0, 0x10);
        func_8000EF8C(acStack_38, param_2);
        func_8008C5E8(acStack_38, 0x10);
        func_8008C5E8(acStack_28, 4);
        iVar2 = func_800B3A40(D_800E1C78 + param_1 * 0x68, (u16)param_4, param_5, acStack_38, acStack_28, param_6, local_20);
        if (iVar2 == 0) {
            iVar2 = local_20[0];
        } else {
            iVar2 = -iVar2;
        }
    }
    return iVar2;
}

s32 func_8008C1FC(s32 channel, s32 file_no) {
    volatile s32 pad[6];
    OSPfsState state;
    s32 ret;
    s32 r2;

    if (!(D_800E2004 & (1 << channel))) {
        return -1;
    }
    func_8000CD00();
    ret = func_800B44D0(&D_800E1C78_4[channel], file_no, &state);
    if (ret != 0) {
        return 100 - ret;
    }
    r2 = func_800B3E70(&D_800E1C78_4[channel], state.company_code, state.game_code, state.game_name, state.ext_name);
    if (r2 != 0) {
        return -r2;
    }
    return 0;
}

s32 func_8008C2BC(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5)
{
    s32 r;

    if ((D_800E2004 & (1 << param_1)) == 0) {
        return -1;
    }
    func_8000CD00();
    r = func_800B4090((u8 *)D_800E1C78 + param_1 * 0x68, param_2, 0, param_3, param_4, param_5);
    if (r == 0) {
        return 0;
    }
    return -r;
}

s32 func_8008C370(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5)
{
    s32 r;

    if ((D_800E2004 & (1u << p1)) == 0) {
        return -1;
    }
    func_8000CD00();
    r = func_800B4090(D_800E1C78 + p1 * 0x68, p2, 1, p3, p4, p5);
    if (r != 0) {
        return -r;
    }
    return 0;
}

void func_8008C424(s32 c, u8 *buf, s32 n)
{
    s32 full;
    s32 v;
    u32 i;
    u32 j;
    u8 *p;

    v = 0x55;
    func_8000CD00();
    func_8000ED10_4(buf, 0x55, n << 3);
    if (0 < n) {
        do {
            i = 0;
            full = 0;
            if (n != 0) {
                p = buf;
                do {
                    for (j = 0; j < 8; j++) {
                        if (buf[i * 8 + j] != v) {
                            break;
                        }
                    }
                    if (j != 8) {
                        full = full + 1;
                    } else {
                        func_8000ED10_4(p, v + 1, 8);
                        func_800AD910(p, 8);
                        func_800AC240((s32)D_800E1C48, (u8)(c + i), p);
                        func_800AD7E0(p, 8);
                    }
                    i = i + 1;
                    p = p + 8;
                } while (i < n);
            }
            v = v + 1;
        } while (full < n);
    }
    func_800AD7E0(buf, n << 3);
}

s32 func_8008C55C(u32 param_1)
{
    u8 *p;
    s32 r;

    func_8000CD00_2();
    p = D_800E1C78 + param_1 * 0x68;
    func_800B4D20(D_800E1C48, p, param_1);
    r = func_800B5820(p);
    func_800B4D20(D_800E1C48, p, param_1);
    return r;
}

void func_8008C5E8(u8 *p, s32 n)
{
    u8 *q;
    u8 c;
    s32 j;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        j = 0;
        if (D_800CDE74[0] == 0) goto nf;
        q = p;
        c = *p;
        while (D_800CDE74[j] != 0) {
            if (c == D_800CDE74[j]) break;
            j++;
        }
        if (D_800CDE74[j] != 0) goto st;
    nf:
        if (*p != 0) goto nz;
        *p = 0;
        goto next;
    nz:
        *p = 0xF;
        goto next;
    st:
        *q = j + 0xF;
    next:
        ;
    }
}

void func_8008C670(u8 *buf, s32 n)
{
    s32 i = 0;
    u8 *p;
    if (n > 0) {
        do {
            p = buf + i;
            if ((u8)(*p - 0xF) < 0x33) {
                *p = D_800CDE74[*p - 0xF];
            } else {
                if (*p != 0) {
                    goto nonzero;
                }
                *p = 0;
                goto next;
            nonzero:
                *p = 0x2A;
            }
        next:
            i++;
        } while (i < n);
    }
}

void func_8008C6D0(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        D_800F4A10[i] = func_800BC908(0x740, 0, (s32)"pak.c", 0x75F);
        func_8000ED10(D_800F4A10[i], 0x77, 0x740);
    }
}

void func_8008C744(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        func_800BCB04(D_800F4A10[i]);
    }
}

typedef struct {
    s32 f0;
    s32 w[16];
} Ent_84;

extern Ent_84 D_800F48E0_4[];

void func_8008C790(s32 idx)
{
    Quad *dst;
    Quad *src;
    Quad *end;

    if (D_800F48E0_4[idx].f0 == (s32)D_800CE168_2 && D_800F4AE0[idx] > 0) {
        dst = (Quad *)&D_800F48E0_4[idx];
        src = (Quad *)D_800F48E0_4[idx].w;
        end = src + 4;
        do {
            *dst = *src;
            dst++;
            src++;
        } while (src != end);
        D_800F4AE0[idx] = D_800F4AE0[idx] - 1;
    }
}

void func_8008C830(s32 *p1, char *p2, s32 p3, s32 p4, s32 p5, s32 p6)
{
    char *q;
    if (p3 == 2) {
        q = p2;
        if (*q != 0) {
            do {
                if (*q == 0x40) {
                    *q = 0x7F;
                }
                q++;
            } while (*q != 0);
        }
    }
    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164, p2);
    func_800C0ACC(D_800CE164, p5, p6);
    func_800C1394(p1);
}

void func_8008C8D8(char *param_1, s32 param_2, s32 param_3) {
    func_800C1E60(D_800CE164, 0x80, param_2, param_3, 0);
    func_800C1F2C(D_800CE164, param_1);
}

void func_8008C92C(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6)
{
    func_800C1E60(D_800CE164, 0x80, param_3, param_4, 0);
    func_800C1F2C(D_800CE164, param_2);
    func_800C0ACC(D_800CE164, param_5 - ((u32)D_800CE164[6] >> 1), param_6);
    func_800C1394(param_1);
}

void func_8008C9B8(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6)
{
    func_800C1E60(D_800CE164, 0x80, param_3, param_4, 0);
    func_800C1F2C(D_800CE164, param_2);
    func_800C0ACC(D_800CE164, param_5 - D_800CE164[6], param_6);
    func_800C1394(param_1);
}

void func_8008CA40(s32 a0, s32 a1)
{
    u8 *p;

    p = D_800F4A28_6;
    *(s32 *)(p + 4) = a0;
    *(s32 *)(p + 8) = 0;
    if (a1 != -1) {
        D_800F4A00 = a1;
    }
}

void func_8008CA68(u8 *param_1)
{
    s32 i;
    u8 *q;

    func_8000CD00();
    i = 0;
    D_800C292C = 0;
    D_800C292D = 0;
    do {
        q = D_800E1C68 + i * 4;
        if (q[3] == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C = D_800C292C | (1 << i);
            D_800C292D = D_800C292D + 1;
        }
        i++;
    } while (i < 4);
    func_800B4830(D_800E1C48, param_1);
}
