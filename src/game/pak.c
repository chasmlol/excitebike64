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

typedef struct { u8 b[0x68]; } Ctl;

extern Rec_47 *sPakNoteTables[];
extern Rec_47 sPakNewNoteEntry;
extern Font_2 *sPakButtonIcons;
extern Msg sPakMessages[];
extern Ctl sPakFs_2[4];

extern Rec_47 *sPakNoteList[];

void pak_build_note_list(s32 p, s32 z)
{
    s32 *cnt;
    s32 i;
    s32 r;
    s32 j;
    Rec_47 *rec;

    sPakNoteCount[p] = 0;
    if (z != 0) {
        sPakNoteList[p * 17] = &sPakNewNoteEntry;
        sPakNoteCount[p] = 1;
    }
    for (i = 0; i < 16; i++) {
        rec = &sPakNoteTables[p][i];
        if (rec->used != -1) {
            if (rec->magic != 0x4E4D5847) {
                r = 1;
            } else {
                r = gPakScreen.label(rec->data, sPakStrBuf, gPakScreen.mode == 0);
                if (r == 2) {
                    if (gPakScreen.mode == 1) {
                        continue;
                    }
                    r = 1;
                }
            }
            if (r != 1 || gPakScreen.mode != 1) {
                mem_fill((u8 *)rec->buf, 0, 0x30);
                *(Name16 *)rec->buf = rec->name;
                if (rec->ext.c[0] != 0) {
                    str_cat(rec->buf, D_800DA708);
                    *(Ext4 *)&rec->buf[strlen(rec->buf)] = rec->ext;
                }
                if (r != 1) {
                    str_cat(rec->buf, D_800DA70C);
                    str_cat(rec->buf, sPakStrBuf);
                }
                rec->status = r;
                j = p * 17; j += sPakNoteCount[p];
                sPakNoteList[j] = rec;
                sPakNoteCount[p]++;
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
    font_text_create(sPakText, 0x80, p3, p4, 0);
    font_text_add(sPakText, (char *)str);
    font_draw_text(sPakText, x, y);
    gdl = font_render(gdl);
    return gdl;
}

Gfx *pak_draw_button_text(Gfx *gdl, u8 *s, s32 p3, s32 p4, s32 x, s32 y)
{
    s32 k = 0;
    s32 code;
    s32 i;

    if (*s != 0) { i = 0; do {
        if (s[i] != 0x40) {
            sPakStrBuf[k] = s[i];
            i++;
            k++;
        } else {
            sPakStrBuf[k] = 0;
            if (k != 0) {
                if (gdl != NULL) {
                    gdl = drawStr(gdl, sPakStrBuf, p3, p4, x, y);
                } else {
                    font_text_create(sPakText, 0x80, p3, p4, 0);
                    font_text_add(sPakText, (char *)sPakStrBuf);
                }
                x += sPakText->w;
            }
            i++;
            k = 0;
            code = s[i] - 0x41;
            if (gdl != NULL) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPLoadTLUT_pal256(gdl++, sPakButtonIcons->pal);
                {
                    Gfx *g = gdl++;
                    g->words.w0 = 0xFCFFFFFF;
                    g->words.w1 = 0xFFFCF279;
                }
                gdl = render_draw_image(x, y - 4, &sPakButtonIcons->g[code], gdl, gGfxFrameIndex, 1, 1, 3, 0);
            }
            i++;
            x += sPakButtonIcons->g[code].w * 8;
        }
    } while (s[i] != 0); }
    sPakStrBuf[k] = 0;
    if (k != 0) {
        if (gdl != NULL) {
                    gdl = drawStr(gdl, sPakStrBuf, p3, p4, x, y);
                } else {
                    font_text_create(sPakText, 0x80, p3, p4, 0);
                    font_text_add(sPakText, (char *)sPakStrBuf);
                }
        x += sPakText->w;
    }
    if (gdl != NULL) {
        return gdl;
    }
    return (Gfx *)x;
}

void pak_screen_init(void)
{
    s32 *g;
    s32 wA, hA, wB, hB, wC, wD, hC, hD;
    f32 fa, fb, fc, fd;

    sPakRepairDeclined = 0;
    sPakPlugMask = 0;
    sPakValidMask = 0;
    sPakFrameCounter = -1;
    sPakOverwriteExt = -1;
    gPakScreenDone = 0;
    D_800F483C = -1;
    sPakWaitTimer = 0;
    gPakScreen.state = 0;
    gPakScreen.subState = 0;
    gPakScreen.pak = 0;
    gPakScreen.sel = 0;
    gPakScreen.unk_14 = 0;
    gPakScreen.scroll = 0;
    g = &sPakGuiRes;
    *g = gGuiRes;
    pak_alloc_note_tables();
    wA = *(volatile s32 *)&gScreenWidth;
    hA = *(volatile s32 *)&gScreenHeight;
    wB = *(volatile s32 *)&gScreenWidth;
    hB = *(volatile s32 *)&gScreenHeight;
    wC = *(volatile s32 *)&gScreenWidth;
    fa = (f32)-(wC / 2);
    wD = *(volatile s32 *)&gScreenWidth;
    fb = (f32)(wD / 2);
    hC = *(volatile s32 *)&gScreenHeight;
    fc = (f32)-(hC / 2);
    hD = *(volatile s32 *)&gScreenHeight;
    fd = (f32)(hD / 2);
    sPakViewport[0] = (s16)(wA << 1);
    sPakViewport[1] = (s16)(hA << 1);
    sPakViewport[4] = (s16)(wB << 1);
    sPakViewport[5] = (s16)(hB << 1);
    guOrtho((s32 *)sPakOrthoMtx, fa, fb, fc, fd, 50.0f, 16384.0f, 1.0f);
    sPakIconTex = res_load(*g, D_800D13A0);
    sPakBgTex = res_load(*g, D_800D1488);
    sPakUiTex = res_load(*g, D_800D14A8);
    sPakLogoTex = res_load(*g, D_800D1418);
    sPakButtonIcons = res_load(*g, D_800D1434);
    sPakText = font_text_create(NULL, 0x80, 1, 4, 0);
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

extern Font_3 *sPakBgTex_2;
extern Font_3 *sPakUiTex_2;
extern Font_3 *sPakIconTex_2;
extern Font_3 *sPakLogoTex_2;
extern Entry_6 *sPakNoteList_2[4][17];

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
    font_text_create(sPakText, 0x80, p3, p4, 0);
    font_text_add(sPakText, (char *)str);
    font_draw_text(sPakText, x, y);
    gdl = font_render(gdl);
    return gdl;
}

static inline Gfx *drawCtr(Gfx *gdl, char *str, s32 p3, s32 p4, s32 cx, s32 y)
{
    font_text_create(sPakText, 0x80, p3, p4, 0);
    font_text_add(sPakText, str);
    font_draw_text(sPakText, cx - (sPakText->w >> 1), y);
    return font_render(gdl);
}

void pak_screen_draw(s32 arg0, s32 arg1, Gfx **pgdl)
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
    font_reset_queue();
    gDPLoadTLUT_pal256(gdl++, sPakBgTex_2->pal);
    gdl = render_draw_image(0, 0, &sPakBgTex_2->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPLoadTLUT_pal256(gdl++, sPakLogoTex_2->pal);
    gdl = render_draw_image(0x4E, 0x16, &sPakLogoTex_2->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gdl = render_draw_image(0x4E, 0x13, &sPakUiTex_2->g[3], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    c = 0xFF;
    if (gPakScreen.state == 0) {
        c = 0x80;
    }
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = render_draw_image(0x2B, 0x32, &sPakUiTex_2->g[2], gdl, arg1, 1, 1, 6, 0);
    c ^= 0x7F;
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = render_draw_image(0x28, 0x95, &sPakUiTex_2->g[1], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    for (j = 0; j < 0x200; j += 0x10) {
        gdl = render_draw_image(j, 0xB4, &sPakUiTex_2->g[0], gdl, arg1, 1, 1, 6, 0);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, sPakIconTex_2->pal);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
    for (i = 0; i < 4; i++) {
        if ((gContMask >> i) & 1) {
            c = 0x80;
            if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                c = 0xFF;
            }
            gDPSetColor(gdl++, G_SETPRIMCOLOR, c | 0xFFFFFF00);
            gdl = render_draw_image(0x37 + i * 100, 0x9C, &sPakIconTex_2->g[(u32)(sPakFrameCounter / 2) % sPakIconTex_2->n], gdl,
                                arg1, 1, 1, 3, 0);
        }
    }
    gdl = drawStr_2(gdl, (u8 *)sPakTitles[gPakScreen.mode], 1, 2, 0x62, 0x1D);
    __builtin_strcpy((char *)sPakStrBuf, "Pak  ");
    for (i = 0; i < 4; i++) {
        if ((sPakValidMask | sPakBadMask) & (1 << i)) {
            sPakStrBuf[4] = i + '1';
            gdl = drawStr_2(gdl, sPakStrBuf, 1, (i == gPakScreen.pak) ? 4 : 2, 0x55 + i * 100, 0x9F);
        }
    }
    if (sPakValidMask & (1 << gPakScreen.pak)) {
        for (i = 0; i < sPakNoteCount[gPakScreen.pak]; i++) {
            c = i - gPakScreen.scroll;
            if (c >= 0) if (c < 8) {
                e = sPakNoteList_2[gPakScreen.pak][i];
                x = 0x58 - c * 5 / 2;
                y = c * 10 + 0x3B;
                str_copy((char *)sPakStrBuf, e->name);
                if (gPakScreen.state == 0) {
                    c = 6;
                } else if (i == gPakScreen.sel) {
                    c = 5;
                } else if (e == &sPakNewNoteEntry) {
                    c = 7;
                    w = 0;
                } else {
                    c = 2;
                }
                gdl = drawStr_2(gdl, sPakStrBuf, 2, c, x, y);
                if (e != &sPakNewNoteEntry) {
                    n = e->size;
                    n += 0xFF;
                    n >>= 8;
                    int_to_str(n, (char *)sPakStrBuf);
                    font_text_create(sPakText, 0x80, 2, c, 0);
                    font_text_add(sPakText, (char *)sPakStrBuf);
                    font_draw_text(sPakText, x + (0x140 - sPakText->w), y);
                    gdl = font_render(gdl);
                    __builtin_strcpy((char *)sPakStrBuf, "page");
                    if (n != 1) {
                        str_cat((char *)sPakStrBuf, "s");
                    }
                    gdl = drawStr_2(gdl, sPakStrBuf, 2, c, x + 0x14A, y);
                }
            }
        }
    } else if (sPakBadMask & (1 << gPakScreen.pak)) {
        gdl = drawCtr(gdl, "Damaged or Corrupted Controller Pak!", 2, 6, 0x100, 0x5C);
    }
    if (gPakScreen.mode == 2 || gPakScreen.mode == 0) {
        if (sPakValidMask & (1 << gPakScreen.pak)) {
            int_to_str(sPakFreePages[gPakScreen.pak], (char *)sPakStrBuf);
            str_cat((char *)sPakStrBuf, " page");
            if (sPakFreePages[gPakScreen.pak] != 1) {
                str_cat((char *)sPakStrBuf, "s");
            }
            str_cat((char *)sPakStrBuf, " free");
            gdl = drawCtr(gdl, (char *)sPakStrBuf, 1, 2, 0x18B, 0x1E);
        }
    }
    if (sPakMessages[gPakScreen.state].lines != NULL) {
        lines = sPakMessages[gPakScreen.state].lines;
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xC0);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, 0xB4);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
        }
        h = i * 14 + 0x50;
        gDPFillRectangle(gdl++, 0, 0x4C, 0x200, h);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPLoadTextureBlock(gdl++, sPakUiTex_2->g[0].addr, G_IM_FMT_IA, G_IM_SIZ_8b, sPakUiTex_2->g[0].w * 8,
                            sPakUiTex_2->g[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 4, 8, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, 0, 0x114, 0x800, 0x130, 0, 0, 0, 0x400, 0x400);
        gSPTextureRectangle(gdl++, 0, (h - 1) * 4, 0x800, (h + 7) * 4, 0, 0, 0x5C0, 0x400, 0x400);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
            gdl = drawCtr(gdl, lines[i], 1, 2, 0x100, 0x53 + i * 14);
        }
        if (sPakMessages[gPakScreen.state].extra != NULL) {
            w = (s32)pak_draw_button_text(NULL, (u8 *)sPakMessages[gPakScreen.state].extra, 1, 4, 0, 0);
            gdl = pak_draw_button_text(gdl, (u8 *)sPakMessages[gPakScreen.state].extra, 1, 4, 0x100 - w / 2, 0xC6);
        }
    } else if (sPakMessages[gPakScreen.state].text != NULL) {
        gdl = drawCtr(gdl, sPakMessages[gPakScreen.state].text, 1, 2, 0x100, 0xBE);
        if (sPakMessages[gPakScreen.state].extra != NULL) {
            w = (s32)pak_draw_button_text(NULL, (u8 *)sPakMessages[gPakScreen.state].extra, 1, 4, 0, 0);
            gdl = pak_draw_button_text(gdl, (u8 *)sPakMessages[gPakScreen.state].extra, 1, 4, 0x100 - w / 2, 0xCE);
        }
    }
    if (sPakWaitTimer != 0) {
        sPakWaitTimer--;
    }
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void pak_refresh(s32 p)
{
    s32 n;
    s32 r;
    s32 bit;
    s32 *q;

    n = 3;
    do {
        if (pak_read_note_states(p) == 0) {
            break;
        }
        n = n - 1;
    } while (n != 0);
    if (n == 0) {
        sPakBadMask = sPakBadMask | (1 << p);
        sPakValidMask = sPakValidMask & ~(1 << p);
    }
    n = 3;
    do {
        r = pak_get_free_bytes(p);
        if (r < 0) {
            r = r + 0xFF;
        }
        sPakFreePages[p] = r >> 8;
    } while ((r >> 8) < 0 && (n = n - 1) != 0);
    if (n == 0) {
        sPakBadMask = sPakBadMask | (1 << p);
        sPakValidMask = sPakValidMask & ~(1 << p);
    }
    pak_build_note_list(p, 0);
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

extern Entry_7 *sPakNoteTables_2[];
extern Entry_7 *sPakNoteList_3[4][17];

static inline void SET(s32 s, s32 m) {
    gPakScreen.state = s;
    gPakScreen.subState = 0;
    sPakSound = m;
}

static inline void SND(s32 m) {
    sPakSound = m;
}

static inline void SET2(s32 s) {
    gPakScreen.state = s;
    gPakScreen.subState = 0;
}

static inline void compact(s32 pak) {
    if (sPakNoteList_3[pak][0] == &sPakNewNoteEntry && sPakNoteCount[pak] > 0) {
        *(EntryRow *)&sPakNoteList_3[pak][0] = *(EntryRow *)&sPakNoteList_3[pak][1];
        sPakNoteCount[pak]--;
    }
}

void pak_screen_update(Pad16 *arg0) {
    u8 st[8];
    u32 pad;
    u32 prev;
    s32 i;
    s32 n;
    s32 j;
    s32 cnt;
    s32 bit;
    Entry_7 *e;

    if (gPakScreenDone != 0) {
        return;
    }
    sPakSound = -1;
    sPakMessages[5].text = sPakModePrompts[gPakScreen.mode];
    if (gPakScreen.mode == 0) {
        sPakMessages[12].lines = D_800CDF64;
    } else if (D_800F01F8 == 1) {
        sPakMessages[12].lines = D_800CDF6C;
    } else {
        sPakMessages[12].lines = D_800CDF78;
    }
    if (gPakScreen.mode == 2) {
        sPakMessages[0].extra = D_800CE034;
    } else {
        sPakMessages[0].extra = D_800CE01C;
    }
    pad = gPadInput;
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
    i = sPakPrevButtons;
    sPakPrevButtons = pad;
    pad &= ~i;
    sPakFrameCounter++;
    if (sPakFrameCounter % 15 == 0 && pak_get_plug_mask_2(st) == 0 && st[0] != sPakPlugMask) {
        for (i = 0; i < 4; i++) {
            if (((st[0] ^ sPakPlugMask) >> i) & 1) {
                sPakRepairDeclined &= ~(1 << i);
                pak_probe_port(i);
                if (sPakValidMask & (1 << i)) {
                    pak_refresh(i);
                } else {
                    mem_fill((u8 *)sPakNoteTables_2[i], 0, 0x740);
                }
                if ((sPakBadMask & (1 << i)) && (gPakScreen.state == 0 || gPakScreen.state == 14)) {
                    gPakScreen.pak = i;
                    SET(1, 3);
                }
            }
        }
        sPakPlugMask = st[0];
    }
    sPakBadMask &= gContMask;
    sPakValidMask &= gContMask;
    gRumblePakMask &= gContMask;
    sPakRepairDeclined &= sPakBadMask;
    if (gPakScreen.state == 0 || gPakScreen.state == 14) {
        for (i = 0; i < 4; i++) {
            n = (gPakScreen.pak + i) % 4;
            if ((sPakBadMask & (1 << n)) && !((sPakRepairDeclined >> n) & 1)) {
                gPakScreen.pak = n;
                SET(1, 3);
            }
        }
    }
    if (gPakScreen.state == 5 && sPakNoteCount[gPakScreen.pak] == 0) {
        gPakScreen.state = 0;
        gPakScreen.subState = 0;
    }
    if ((gPakScreen.state < 14 || gPakScreen.state > 16) && gPakScreen.state != 20) {
        if ((sPakValidMask | sPakBadMask) == 0 && gPakScreen.state != 14) {
            gPakScreen.state = 14;
            gPakScreen.subState = 0;
        } else if (!((sPakValidMask | sPakBadMask) & (1 << gPakScreen.pak)) && gPakScreen.state != 0) {
            gPakScreen.state = 0;
            gPakScreen.subState = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        if (!((sPakBadMask | sPakValidMask) & (1 << gPakScreen.pak))) {
            gPakScreen.pak = (gPakScreen.pak + 1) % 4;
        }
    }
    if (sPakScreenSaved.state != -1 && gPakScreen.state == 0 && gPakScreen.mode == 0) {
        gPakScreen = sPakScreenSaved;
        sPakScreenSaved.state = -1;
        gPakScreen.state = 0;
        gPakScreen.subState = 0;
    }
    switch (gPakScreen.state) {
    case 0:
        compact(gPakScreen.pak);
        gPakScreen.sel = 0;
        gPakScreen.scroll = 0;
        if (pad & 4) {
            for (i = gPakScreen.pak - 1; i >= 0; i--) {
                if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                    break;
                }
            }
            if (i < 0) {
                sPakSound = 3;
            } else {
                gPakScreen.pak = i;
                sPakSound = 0;
            }
        } else if (pad & 8) {
            for (i = gPakScreen.pak + 1; i < 4; i++) {
                if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                    break;
                }
            }
            if (i >= 4) {
                sPakSound = 3;
            } else {
                gPakScreen.pak = i;
                sPakSound = 0;
            }
        }
        if (pad & 0x20) {
            if (gPakScreen.mode == 1) {
                SET(18, 2);
            } else if (gPakScreen.mode == 2) {
                SET(17, 2);
            } else {
                gPakScreenAborted = 0;
                if (D_800CDE70 != 0) {
                    gPakScreenDone = 1;
                    sPakSound = 1;
                    D_800CDE70 = 0;
                    break;
                }
                SET(20, 2);
            }
        } else if (pad & 0x10) {
            if (sPakValidMask & (1 << gPakScreen.pak)) {
                n = 0;
                for (i = 0; i < 16; i++) {
                    if (sPakNoteTables_2[gPakScreen.pak][i].fileNo != -1) {
                        n++;
                    }
                }
                gPakScreen.sel = 0;
                if (gPakScreen.mode == 2) {
                    if (n == 16) {
                        SET(8, 1);
                    } else {
                        pak_build_note_list(gPakScreen.pak, 1);
                        SET(5, 1);
                    }
                } else if (gPakScreen.mode == 0 && n == 0) {
                    SET(12, 3);
                } else if (gPakScreen.mode == 1 && sPakNoteCount[gPakScreen.pak] == 0) {
                    SET(12, 3);
                } else {
                    SET(5, 1);
                }
            } else if (sPakBadMask & (1 << gPakScreen.pak)) {
                SET(1, 3);
            }
        }
        break;
    case 5:
        if (gPakScreen.sel >= sPakNoteCount[gPakScreen.pak]) {
            gPakScreen.sel = sPakNoteCount[gPakScreen.pak] - 1;
        }
        if (gPakScreen.sel < 0) {
            gPakScreen.sel = 0;
        }
        j = gPakScreen.sel;
        n = 0;
        if (pad & 2) {
            n = 1;
        }
        i = j;
        if (pad & 1) {
            n = -1;
        }
        j += n;
        cnt = sPakNoteCount[gPakScreen.pak];
        do {
            if (j < 0) {
                if (n != 0) {
                    j = i;
                    goto done;
                }
                j = sPakNoteCount[gPakScreen.pak] - 1;
            }
            if (j >= sPakNoteCount[gPakScreen.pak]) {
                if (n == 0) {
                    j = 0;
                } else {
                    j = i;
                    goto done;
                }
            }
            if (gPakScreen.mode != 2 || sPakNoteList_3[gPakScreen.pak][j] == &sPakNewNoteEntry ||
                sPakNoteList_3[gPakScreen.pak][j]->code == 0x4E4D5847) {
                goto done;
            }
            j += n;
        } while (n != 0 || (n = 1, --cnt != 0));
    done:
        gPakScreen.sel = j;
        if (j < gPakScreen.scroll) {
            gPakScreen.scroll = j;
        }
        if (gPakScreen.scroll < j - 7) {
            gPakScreen.scroll = j - 7;
        }
        if (sPakNoteCount[gPakScreen.pak] - 8 < gPakScreen.scroll) {
            gPakScreen.scroll = sPakNoteCount[gPakScreen.pak] - 8;
        }
        if (gPakScreen.scroll < 0) {
            gPakScreen.scroll = 0;
        }
        if (pad & 3) {
            if (i == j) {
                sPakSound = 3;
            } else {
                sPakSound = 0;
            }
        }
        if (pad & 0x20) {
            SET(0, 2);
        }
        if (pad & 0x10) {
            if (gPakScreen.mode == 0) {
                SET(7, 1);
            } else if (gPakScreen.mode == 2) {
                e = sPakNoteList_3[gPakScreen.pak][gPakScreen.sel];
                if (e == &sPakNewNoteEntry) {
                    if (sPakFreePages[gPakScreen.pak] < gPakScreen.pages) {
                        SET(8, 1);
                        break;
                    }
                } else {
                    j = (e->size + 0xFF) >> 8;
                    if (sPakFreePages[gPakScreen.pak] + j < gPakScreen.pages) {
                        SET(8, 1);
                        break;
                    }
                }
                if (e != &sPakNewNoteEntry) {
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
        switch (gPakScreen.subState) {
        case 0:
            sPakWaitTimer = 30;
            gPakScreen.subState = 1;
            break;
        case 1:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = 2;
            }
            break;
        case 2:
            if (pak_delete_note(gPakScreen.pak, sPakNoteList_3[gPakScreen.pak][gPakScreen.sel]->fileNo) != 0) {
                SET(11, 3);
            } else if (gPakScreen.mode != 0) {
                pak_refresh(gPakScreen.pak);
                SET(0, 1);
            } else {
                pak_refresh(gPakScreen.pak);
                if (sPakNoteList_3[gPakScreen.pak] == NULL) {
                    SET(0, 1);
                } else {
                    SET(5, 1);
                }
            }
            break;
        }
        break;
    case 1:
        D_800CDF00[0][0x11] = gPakScreen.pak + '1';
        if (pad & 0x10) {
            SET(2, 1);
        }
        if (pad & 0x20) {
            SND(2);
            SET2(0);
            sPakRepairDeclined |= 1 << gPakScreen.pak;
        }
        break;
    case 12:
        if (pad & 0x10) {
            SET(0, 1);
        }
        break;
    case 2:
        switch (gPakScreen.subState) {
        case 0:
            sPakWaitTimer = 30;
            gPakScreen.subState = 1;
            break;
        case 1:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = 2;
            }
            break;
        case 2:
            if (pak_repair(gPakScreen.pak) != 0) {
                SET(4, 3);
            } else {
                gPakScreen.state = 3;
                sPakSound = 1;
                gPakScreen.subState = 0;
                sPakPlugMask &= ~(1 << gPakScreen.pak);
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
            sPakRepairDeclined |= 1 << gPakScreen.pak;
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
            e = sPakNoteList_3[gPakScreen.pak][gPakScreen.sel];
            if (pak_delete_note(gPakScreen.pak, e->fileNo) != 0) {
                SET(11, 3);
            } else {
                if (e->code == 0x4E4D5847) {
                    sPakOverwriteExt = e->ext;
                } else {
                    sPakOverwriteExt = -1;
                }
                SET(10, 1);
            }
        }
        break;
    case 11:
        if (pad & 0x10) {
            SET(5, 1);
            sPakPlugMask = 0;
            sPakValidMask = 0;
            sPakFrameCounter = -1;
        }
        break;
    case 14:
        if (sPakBadMask | sPakValidMask) {
            gPakScreen.state = 0;
            gPakScreen.subState = 0;
        } else if (pad & 0x20) {
            gPakScreenAborted = 1;
            gPakScreenDone = 1;
            sPakSound = 2;
        }
        break;
    case 9:
        switch (gPakScreen.subState) {
        case 0:
            sPakWaitTimer = 30;
            gPakScreen.subState = 1;
            break;
        case 1:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = 2;
            }
            break;
        case 2:
            e = sPakNoteList_3[gPakScreen.pak][gPakScreen.sel];
            if (gPakScreen.pages < (e->size + 0xFF) >> 8) {
                SET(19, 3);
                return;
            }
            mem_fill(gPakScreen.data, 0, gPakScreen.pages << 8);
            if (pak_read_note(gPakScreen.pak, e->fileNo, 0, (e->size + 0xFF) & ~0xFF, gPakScreen.data) != 0) {
                SET(11, 3);
                return;
            }
            if (gPakScreen.validate != NULL && gPakScreen.validate(gPakScreen.data, (e->size + 0xFF) & ~0xFF) != 0) {
                SET(19, 3);
                return;
            }
            SET2(16);
            break;
        }
        break;
    case 10:
        switch (gPakScreen.subState) {
        case 0:
            sPakWaitTimer = 30;
            gPakScreen.subState = 1;
            break;
        case 1:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = 2;
            }
            break;
        case 2:
            if (sPakOverwriteExt == -1) {
                n = 'A';
                for (;;) {
                    for (i = 0; i < sPakNoteCount[gPakScreen.pak]; i++) {
                        e = sPakNoteList_3[gPakScreen.pak][i];
                        if (e->code == 0x4E4D5847 && e->ext == n) {
                            break;
                        }
                    }
                    if (i == sPakNoteCount[gPakScreen.pak]) {
                        break;
                    }
                    if (++n > 'Z') {
                        break;
                    }
                }
            } else {
                n = sPakOverwriteExt;
            }
            sPakOverwriteExt = -1;
            mem_fill(sPakStrBuf, 0, 4);
            sPakStrBuf[0] = n;
            j = pak_create_note(gPakScreen.pak, sPakGameName, sPakStrBuf, 0x3031, 0x4E4D5847, gPakScreen.pages << 8);
            if (j < 0) {
                SET(11, 3);
            }
            if (pak_write_note(gPakScreen.pak, j, 0, gPakScreen.pages << 8, gPakScreen.data) != 0) {
                pak_delete_note(gPakScreen.pak, j);
                SET(11, 3);
            }
            sPakVerifyBuf = mem_alloc_dbg(gPakScreen.pages << 8, 0, "pak.c", 0x5E6);
            n = pak_read_note(gPakScreen.pak, j, 0, gPakScreen.pages << 8, sPakVerifyBuf);
            if (n == 0) {
                for (i = 0; i < gPakScreen.pages << 8; i++) {
                    if (sPakVerifyBuf[i] != gPakScreen.data[i]) {
                        n = 1;
                    }
                }
            }
            mem_free(sPakVerifyBuf);
            if (n != 0) {
                pak_delete_note(gPakScreen.pak, j);
                SET(11, 3);
            }
            pak_refresh(gPakScreen.pak);
            gPakScreen.state = 15;
            gPakScreen.subState = 0;
            break;
        }
        break;
    case 19:
        if (pad & 0x10) {
            SET(7, 1);
        } else if (pad & 0x20) {
            SET(0, 1);
            sPakPlugMask = 0;
            sPakValidMask = 0;
            sPakFrameCounter = -1;
        }
        break;
    case 8:
        if (pad & 0x20) {
            SET(0, 2);
        } else if (pad & 0x10) {
            sPakScreenSaved = gPakScreen;
            gPakScreen.mode = 0;
            pak_build_note_list(gPakScreen.pak, 0);
            compact(gPakScreen.pak);
            SET(5, 1);
        }
        break;
    case 17:
        gPakScreenAborted = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 18:
        gPakScreenAborted = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 15:
    case 16:
        gPakScreenAborted = 0;
    case 20:
        if (pad & 0x10) {
            gPakScreenDone = 1;
            sPakSound = 1;
        }
        break;
    }
    if (gPakScreenDone != 0 && sPakScreenSaved.state != -1) {
        gPakScreen = sPakScreenSaved;
        sPakScreenSaved.state = -1;
        gPakScreen.state = 0;
        gPakScreen.subState = 0;
        gPakScreenDone = 0;
    }
    if (sPakSound != -1) {
        snd_play_menu_sound(sPakSound);
    }
    if (gPakScreenDone != 0) {
        pak_probe_all();
        if (gPakScreenDone != 0) {
            D_800CDE70 = 0;
        }
    }
}

void pak_screen_exit(void)
{
    res_unload((u32 *)(sPakGuiRes_2[1] + D_800D13A0 * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + D_800D1488 * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + D_800D14A8 * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + D_800D1418 * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + D_800D1434 * 4));
    func_800C204C(sPakText);
    mem_free(sPakText);
    pak_free_note_tables();
    pak_probe_all();
}

typedef struct { u32 w[8]; } Hdr_10;

typedef struct {
    u32 f0;
    u32 magic;
    u8 p8[2];
    u8 a[4];
    u8 b[0x10];
    s16 id;
    Hdr_10 hdr;
    u8 rest[0x74 - 0x40];
} Slot_13;

extern Slot_13 *sPakNoteTables_3[];

s32 pak_read_note_states(u32 p)
{
    Hdr_10 tmp;
    s32 i, k, r;

    if (!(sPakValidMask & (1 << p))) {
        return -1;
    }
    cont_finish_read();
    for (i = 0; i < 16; i++) {
        sPakNoteTables_3[p][i].id = -1;
    }
    k = 0;
    i = 0;
    do {
        r = osPfsFileState(&sPakFs_2[p], i, &sPakNoteTables_3[p][k]);
        if (r == 5) {
            mem_fill((u8 *)&sPakNoteTables_3[p][k], 0, 0x74);
            sPakNoteTables_3[p][k].id = -1;
        } else if (r == 0) {
            sPakNoteTables_3[p][k].id = i;
            pak_pfs_to_ascii(sPakNoteTables_3[p][k].b, 0x10);
            pak_pfs_to_ascii(sPakNoteTables_3[p][k].a, 4);
            if (sPakNoteTables_3[p][k].magic == 0x4E4D5847) {
                if (pak_read_note(p, i, 0, 0x20, (u8 *)&tmp) != 0) {
                    sPakNoteTables_3[p][k].magic = 0;
                }
                sPakNoteTables_3[p][k].hdr = tmp;
            }
            k++;
        } else {
            return -2;
        }
        i++;
    } while (i < 16);
    return 0;
}

void pak_probe_port(s32 chan)
{
    u32 bit = 1 << chan;
    s32 msg[2];
    s32 i;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, msg, 1);
    osContGetQuery(gContStatus);
    gContMask = 0;
    gContCount = 0;
    for (i = 0; i < 4; i++) {
        if (gContStatus[i].errno == 0) {
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask |= 1 << i;
            gContCount++;
        }
    }
    if ((gContMask & bit) == 0) {
        return;
    }
    i = chan;
    sPakValidMask &= ~bit;
    gRumblePakMask &= ~bit;
    sPakBadMask &= ~bit;
    switch (osPfsInitPak(&gSiMesgQueue, &sPakFs_2[i], i)) {
    case 0:
        sPakValidMask |= bit;
        break;
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        break;
    case 11:
        if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) != 0) {
            if (osGbpakInit(&gSiMesgQueue, &sGbPakFs, i) != 0) {
                sPakBadMask |= 1 << i;
            }
        } else {
            __osMotorAccess(&gRumblePfs[i], 0);
            gRumblePakMask |= bit;
        }
        break;
    case 10:
        if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
            __osMotorAccess(&gRumblePfs[i], 0);
            gRumblePakMask |= bit;
        } else {
            sPakBadMask |= bit;
        }
        break;
    }
}

void pak_probe_all(void)
{
    u32 buf[2];
    s32 i;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, buf, 1);
    osContGetQuery(gContStatus);
    gContMask = 0;
    gContCount = 0;
    for (i = 0; i < 4; i++) {
        if (gContStatus[i].errno == 0) {
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask |= 1 << i;
            gContCount++;
        }
    }
    gRumbleActiveMask = 0;
    sPakValidMask = 0;
    gRumblePakMask = 0;
    sPakBadMask = 0;
    for (i = 0; i < 4; i++) {
        gRumbleTimers[i] = 0;
        if ((gContMask >> i) & 1) {
            switch (osPfsInitPak(&gSiMesgQueue, &sPakFs_3[i], i)) {
            case 0:
                sPakValidMask |= 1 << i;
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
                if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
                    __osMotorAccess(&gRumblePfs[i], 0);
                    gRumblePakMask |= 1 << i;
                } else if (osGbpakInit(&gSiMesgQueue, &sGbPakFs, i) != 0) {
                    sPakBadMask |= 1 << i;
                }
                break;
            case 10:
                if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
                    __osMotorAccess(&gRumblePfs[i], 0);
                    gRumblePakMask |= 1 << i;
                } else {
                    sPakBadMask |= 1 << i;
                }
                break;
            }
        }
    }
    sEepromType = osEepromProbe(&gSiMesgQueue);
}

void pak_query_controllers(void)
{
    s32 buf[2];
    s32 k;
    u8 *p;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, buf, 1);
    osContGetQuery(gContStatus);
    k = 0;
    gContMask = 0;
    gContCount = 0;
    p = gContStatus;
    do {
        if (p[3] == 0) {
            if (gContMask == 0) {
                gContFirst = k;
            }
            gContMask = gContMask | (1 << k);
            gContCount = gContCount + 1;
        }
        k++;
        p += 4;
    } while (k < 4);
}

void pak_update_free_pages(void)
{
    s32 i;
    s32 a;
    s32 b;
    s32 c;
    s32 x;

    pak_probe_all();
    for (i = 0; i < 4; i++) {
        gPakFreePages[i] = -2;
        if ((sPakValidMask & (1 << i)) && osPfsNumFiles(&sPakFs[i * 0x68], &a, &b) == 0) {
            if (!(sPakValidMask & (1 << i))) {
                x = -1;
            } else {
                cont_finish_read();
                if (osPfsFreeBlocks(&sPakFs[i * 0x68], &c) != 0) {
                    x = -2;
                } else {
                    x = c;
                }
            }
            x &= ~0xFF;
            if (b == 0x10) {
                gPakFreePages[i] = -1;
            } else {
                gPakFreePages[i] = x / 256;
            }
        }
    }
}

int pak_get_free_bytes(u32 param_1)
{
    int local_10[2];

    if ((sPakValidMask & (1 << param_1)) == 0) {
        return -1;
    }
    cont_finish_read();
    if (osPfsFreeBlocks((u32 *)(sPakFs + param_1 * 0x68), local_10) != 0) {
        return -2;
    }
    return local_10[0];
}

s32 pak_create_note(u32 param_1, char *param_2, char *param_3, s32 param_4, s32 param_5, s32 param_6)
{
    s32 iVar2;
    u8 acStack_38[16];
    u8 acStack_28[8];
    s32 local_20[2];

    if ((sPakValidMask & (1 << param_1)) == 0) {
        iVar2 = -1;
    } else {
        cont_finish_read();
        mem_fill(acStack_28, 0, 4);
        str_copy(acStack_28, param_3);
        mem_fill(acStack_38, 0, 0x10);
        str_copy(acStack_38, param_2);
        pak_ascii_to_pfs(acStack_38, 0x10);
        pak_ascii_to_pfs(acStack_28, 4);
        iVar2 = osPfsAllocateFile(sPakFs + param_1 * 0x68, (u16)param_4, param_5, acStack_38, acStack_28, param_6, local_20);
        if (iVar2 == 0) {
            iVar2 = local_20[0];
        } else {
            iVar2 = -iVar2;
        }
    }
    return iVar2;
}

s32 pak_delete_note(s32 channel, s32 file_no) {
    volatile s32 pad[6];
    OSPfsState state;
    s32 ret;
    s32 r2;

    if (!(sPakValidMask & (1 << channel))) {
        return -1;
    }
    cont_finish_read();
    ret = osPfsFileState(&sPakFs_4[channel], file_no, &state);
    if (ret != 0) {
        return 100 - ret;
    }
    r2 = osPfsDeleteFile(&sPakFs_4[channel], state.company_code, state.game_code, state.game_name, state.ext_name);
    if (r2 != 0) {
        return -r2;
    }
    return 0;
}

s32 pak_read_note(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5)
{
    s32 r;

    if ((sPakValidMask & (1 << param_1)) == 0) {
        return -1;
    }
    cont_finish_read();
    r = osPfsReadWriteFile((u8 *)sPakFs + param_1 * 0x68, param_2, 0, param_3, param_4, param_5);
    if (r == 0) {
        return 0;
    }
    return -r;
}

s32 pak_write_note(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5)
{
    s32 r;

    if ((sPakValidMask & (1u << p1)) == 0) {
        return -1;
    }
    cont_finish_read();
    r = osPfsReadWriteFile(sPakFs + p1 * 0x68, p2, 1, p3, p4, p5);
    if (r != 0) {
        return -r;
    }
    return 0;
}

void pak_eeprom_read(s32 c, u8 *buf, s32 n)
{
    s32 full;
    s32 v;
    u32 i;
    u32 j;
    u8 *p;

    v = 0x55;
    cont_finish_read();
    mem_fill_4(buf, 0x55, n << 3);
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
                        mem_fill_4(p, v + 1, 8);
                        osWritebackDCache(p, 8);
                        osEepromRead((s32)&gSiMesgQueue, (u8)(c + i), p);
                        osInvalDCache(p, 8);
                    }
                    i = i + 1;
                    p = p + 8;
                } while (i < n);
            }
            v = v + 1;
        } while (full < n);
    }
    osInvalDCache(buf, n << 3);
}

s32 pak_repair(u32 param_1)
{
    u8 *p;
    s32 r;

    cont_finish_read_2();
    p = sPakFs + param_1 * 0x68;
    osPfsInitPak(&gSiMesgQueue, p, param_1);
    r = osPfsRepairId(p);
    osPfsInitPak(&gSiMesgQueue, p, param_1);
    return r;
}

void pak_ascii_to_pfs(u8 *p, s32 n)
{
    u8 *q;
    u8 c;
    s32 j;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        j = 0;
        if (sPakCharset[0] == 0) goto nf;
        q = p;
        c = *p;
        while (sPakCharset[j] != 0) {
            if (c == sPakCharset[j]) break;
            j++;
        }
        if (sPakCharset[j] != 0) goto st;
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

void pak_pfs_to_ascii(u8 *buf, s32 n)
{
    s32 i = 0;
    u8 *p;
    if (n > 0) {
        do {
            p = buf + i;
            if ((u8)(*p - 0xF) < 0x33) {
                *p = sPakCharset[*p - 0xF];
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

void pak_alloc_note_tables(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        sPakNoteTables[i] = mem_alloc_dbg(0x740, 0, (s32)"pak.c", 0x75F);
        mem_fill(sPakNoteTables[i], 0x77, 0x740);
    }
}

void pak_free_note_tables(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        mem_free(sPakNoteTables[i]);
    }
}

typedef struct {
    s32 f0;
    s32 w[16];
} Ent_84;

extern Ent_84 sPakNoteList_4[];

void pak_remove_new_note_entry(s32 idx)
{
    Quad *dst;
    Quad *src;
    Quad *end;

    if (sPakNoteList_4[idx].f0 == (s32)sPakNewNoteEntry_2 && sPakNoteCount[idx] > 0) {
        dst = (Quad *)&sPakNoteList_4[idx];
        src = (Quad *)sPakNoteList_4[idx].w;
        end = src + 4;
        do {
            *dst = *src;
            dst++;
            src++;
        } while (src != end);
        sPakNoteCount[idx] = sPakNoteCount[idx] - 1;
    }
}

void pak_draw_text(s32 *p1, char *p2, s32 p3, s32 p4, s32 p5, s32 p6)
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
    font_text_create(sPakText, 0x80, p3, p4, 0);
    font_text_add(sPakText, p2);
    font_draw_text(sPakText, p5, p6);
    font_render(p1);
}

void pak_set_text(char *param_1, s32 param_2, s32 param_3) {
    font_text_create(sPakText, 0x80, param_2, param_3, 0);
    font_text_add(sPakText, param_1);
}

void pak_draw_text_centered(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6)
{
    font_text_create(sPakText, 0x80, param_3, param_4, 0);
    font_text_add(sPakText, param_2);
    font_draw_text(sPakText, param_5 - (sPakText->w >> 1), param_6);
    font_render(param_1);
}

void pak_draw_text_right(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6)
{
    font_text_create(sPakText, 0x80, param_3, param_4, 0);
    font_text_add(sPakText, param_2);
    font_draw_text(sPakText, param_5 - sPakText->w, param_6);
    font_render(param_1);
}

void pak_set_state(s32 a0, s32 a1)
{
    PakScreen *p;

    p = &gPakScreen;
    p->state = a0;
    p->subState = 0;
    if (a1 != -1) {
        sPakSound = a1;
    }
}

void pak_get_plug_mask(u8 *param_1)
{
    s32 i;
    u8 *q;

    cont_finish_read();
    i = 0;
    gContMask = 0;
    gContCount = 0;
    do {
        q = gContStatus_2 + i * 4;
        if (q[3] == 0) {
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask = gContMask | (1 << i);
            gContCount = gContCount + 1;
        }
        i++;
    } while (i < 4);
    osPfsIsPlug(&gSiMesgQueue, param_1);
}
