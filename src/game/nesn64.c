#include "game.h"

#define NES_2 (*(Nes *)0x80380000)
#define NES_3 ((Nes *)0x80380000)

typedef struct { u8 b[0x2812]; } B2812;
typedef struct { u8 ram[0x10000]; u8 pad[0x4100]; u16 pc; u8 a, x, y, s, v, b, n, d, i, z, c, p; } Nes;

extern B2812 *sNesSnapshot;

static inline void wr(s32 a, s32 v)
{
    if (a < 0x2000) {
        *(u8 *)((a & 0x7FF) + 0x80380000) = v;
    } else if (a > 0x7FFF) {
    } else {
        nes_write_io(a, v);
    }
}

static inline void wr_2(s32 a, s32 v)
{
    if (a < 0x2000) {
        a &= 0x7FF;
        *(u8 *)(a + 0x80380000) = v;
    } else if (a > 0x7FFF) {
    } else {
        nes_write_io(a, v);
    }
}

static inline u32 rd(s32 a)
{
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        return nes_read_io_2(a);
    }
    return *(u8 *)(a + 0x80380000);
}

typedef struct { u8 b[0x4000]; } B4000;
typedef struct { u8 b[0x2000]; } B2000;

void nes_init(void)
{
    s32 *obj;
    u8 *src;
    s32 m, t, x, y;
    u32 r, g, b;
    s32 i, j, n, k;
    u32 v, c;
    u16 *p;

    sNesFrameCount = 0;
    if (res_open_set(D_800DAF30, &obj) != 0) {
        fatal_halt(1);
    }
    sNesMemory = (u8 *)0x80380000;
    mem_fill(sNesMemory, 0, 0x141C0);
    src = res_load(obj, D_800D14D8);
    *(B4000 *)(sNesMemory + 0x8000) = *(B4000 *)src;
    *(B4000 *)(sNesMemory + 0xC000) = *(B4000 *)src;
    *(B2000 *)(sNesMemory + 0x10000) = *(B2000 *)(src + 0x4000);
    nes_cpu_reset(sNesMemory, nes_draw_bg_row);
    sNesSnapshot = mem_alloc_dbg(0x2812, 0, D_800DAF38, 0x9C);
    *sNesSnapshot = *(B2812 *)(src + 0x6000);
    res_close_set(obj);
    sNesChrTiles = mem_alloc_dbg(0x8000, 0, D_800DAF38, 0xA4);
    m = 0x80380000;
    t = 0x10000;
    for (i = 0; i < 0x2000; i += 0x10) {
        k = i * 4;
        for (j = 0; j < 8; j++) {
            x = i + j;
            y = x + 8;
            c = *(u8 *)(y + m + t);
            v = *(u8 *)(x + m + t);
            v |= c << 8;
            for (n = 0; n < 8; n += 2) {
                c = (v >> 7) & 1;
                if (v & 0x8000) {
                    c |= 2;
                }
                v <<= 1;
                sNesChrTiles[k] = c << 4;
                c = (v >> 7) & 1;
                if (v & 0x8000) {
                    c |= 2;
                }
                v <<= 1;
                sNesChrTiles[k] |= c;
                k++;
            }
            k += 4;
        }
    }
    sNesPalette = mem_alloc_dbg(0x200, 0, D_800DAF38, 0xBF);
    p = sNesPalette;
    for (i = 0; i < 0x100; i++) {
        j = sNesRgbPalette[i * 3] >> 3;
        j <<= 11;
        n = sNesRgbPalette[i * 3 + 1] >> 3;
        n <<= 6;
        k = sNesRgbPalette[i * 3 + 2] >> 3;
        k <<= 1;
        p[i] = j | n | k | 1;
        if (p[i] == 0x8685) {
            p[i] &= 0xFFFE;
        }
    }
    for (i = 0; i < 2; i++) {
        sNesBgTluts[i] = mem_alloc_dbg(0x200, 0, D_800DAF38, 0xCC);
        sNesSpriteTluts[i] = mem_alloc_dbg(0x200, 0, D_800DAF38, 0xCD);
    }
    sNesRowCallback = 0;
    for (i = 0; i < 3; i++) {
        nes_run_frame();
    }
    sNesRowCallback = nes_draw_bg_row;
    gNesActive = 1;
    nes_audio_init();
    sNesRestoreTimer = 0;
}

#define NES_4 ((u8 *)0x80380000)

typedef struct {
    u32 a;
    u32 b;
    u8 pad[0x1C];
} Ch;

typedef struct {
    u8 pad[0x1412C];
    Ch ch[4];
} Nes_2;

void nes_update_input(volatile s32 *p)
{
    s32 w, v;
    s32 f;
    s32 k;
    s32 hi, lo;
    u8 b;
    u8 *m;
    u8 *g;

    w = *p;
    f = ((w & 0x800) != 0) << 4;
    if (w & 0x1000) f |= 0x20;
    if (w & 0x2000) f |= 0x40;
    if (w & 0x4000) f |= 0x80;
    if (w & 0x10) f |= 1;
    if (w & 0x20000) f |= 2;
    if (w & 0x20) f |= 2;
    if (w & 0x8000) f |= 4;
    if (w & 0x40) f |= 8;
    if (sNesRestoreTimer != 0) {
        if (--sNesRestoreTimer == 0) {
            __builtin_memcpy(NES_4, sNesSnapshot_2, 0x800);
            __builtin_memcpy(NES_4 + 0x12000, sNesSnapshot_2 + 0x800, 0x2000);
            m = sNesSnapshot_2;
            g = (u8 *)0x80390000;
            *(u16 *)(g + 0x4100) = *(u16 *)(m + 0x2800);
            *(u16 *)(g + 0x4114) = *(u16 *)(m + 0x2802);
            g[0x4102] = m[0x2804];
            g[0x4103] = m[0x2805];
            g[0x4104] = m[0x2806];
            g[0x4105] = m[0x2807];
            b = m[0x2808];
            g[0x410D] = b;
            *(u32 *)(g + 0x4118) = m[0x2809];
            g[0x411C] = m[0x280A];
            g[0x411D] = m[0x280B];
            g[0x411F] = m[0x280C];
            g[0x4108] = b & 0x80;
            g[0x4106] = b & 0x40;
            g[0x4107] = b & 0x10;
            g[0x4109] = b & 0x8;
            g[0x410A] = b & 0x4;
            g[0x4120] = 0;
            g[0x4121] = 0;
            *(u32 *)(g + 0x4124) = 0;
            *(u32 *)(g + 0x4128) = 0;
            g[0x410B] = b & 0x2;
            g[0x410C] = b & 0x1;
            for (k = 0; k < 4; k++) {
                ((Nes_2 *)0x80380000)->ch[k].a = 0;
                ((Nes_2 *)0x80380000)->ch[k].b = 0;
            }
            __builtin_memcpy(NES_4 + 0x5E0, gSaveNesTrack, 0x220);
        }
    }
    v = *p;
    lo = (v >> 24) & 0xF;
    hi = (v >> 28) & 0xF;
    if ((v & 1) && hi > 8) f |= 0x10;
    if ((*p & 2) && hi > 8) f |= 0x20;
    if ((*p & 4) && lo > 8) f |= 0x40;
    if ((*p & 8) && lo > 8) f |= 0x80;
    *(u8 *)0x80394120 = f;
    if ((*p & 0x28000) == 0x28000) {
        gRaceExitCode = 2;
        gCurGameState_4[4] = 3;
    }
}

Gfx *nes_draw_setup(s32 a0, s32 a1, Gfx *gdl)
{
    mtx_from_angles_pos_scale(a0 + 0x400, 0, 0, 0, 0, 0, 0, 0x100);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    sNesViewport.vp.vscale[0] = gScreenWidth * 2;
    sNesViewport.vp.vscale[1] = gScreenHeight * 2;
    sNesViewport.vp.vtrans[0] = gScreenWidth * 2;
    sNesViewport.vp.vtrans[1] = gScreenHeight * 2;
    gSPViewport(gdl++, &sNesViewport);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[a1]);
    if (sNesClearFrames != 0) {
        sNesClearFrames--;
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gDPSetColorDither(gdl++, G_CD_DISABLE);
    if (sNesClearFrames != 0) {
        return gdl;
    }
    {
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_POINT);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, sNesPalette);
        return gdl;
    }
    return gdl;
}

#define SAVE (*(Blk_25 *)0x803805E0)
#define BIG ((Big_4 *)0x80380000)
#define FLAGS (BIG->flags)
#define CART (*(u16 *)0x80394100)

typedef struct { u8 b[0x220]; } Blk_25;
typedef struct { s32 a; s32 b; s32 pad[7]; } Ent_87;
typedef struct { u8 pad[0x2001]; u8 flags; u8 pad2[0x1412C - 0x2002]; Ent_87 e[4]; } Big_4;
typedef struct { u8 pad[0x10000]; u8 buf[0x2800]; } Vram;

extern Vram *sNesCpuMemory_2;
extern Blk_25 gSaveNesTrack_2;

void nes_draw_frame(s32 a0, s32 a1, Gfx **pgdl)
{
    u8 *p;
    s32 k;
    s32 i;
    s32 t;

    sNesGfx = *pgdl;
    sNesGfx = nes_draw_setup(a0, a1, sNesGfx);
    if (sNesClearFrames != 0) {
        *pgdl = sNesGfx;
        return;
    }
    {
        Gfx *g = sNesGfx++;
        g->words.w0 = 0xED090000;
        g->words.w1 = 0x4703C0;
    }
    sNesBgTlutLoaded = 0;
    sNesLastTileKey = -1;
    nes_run_frame();
    sNesFrameCount++;
    sNesInstrPerFrame = sNesInstrCount / sNesFrameCount;
    gDPPipeSync(sNesGfx++);
    nes_draw_sprites(sNesInstrPerFrame);
    gDPPipeSync(sNesGfx++);
    if (nes_should_show_exit_msg() != 0) {
        k = 0x2342;
        if ((sNesFrameCount / 30) & 1) {
            if (sNesExitMsg[0] != 0) {
                i = 0;
                do {
                    if (sNesExitMsg[i] != ' ') {
                        sNesCpuMemory_2->buf[k] = sNesExitMsg[i] - 0x37;
                        sNesCpuMemory_2->buf[k + 0x400] = sNesExitMsg[i] - 0x37;
                    }
                    i++;
                    k++;
                } while (sNesExitMsg[i] != 0);
            }
            mem_fill(&sNesCpuMemory_2->buf[0x23F0], 0x5F, 8);
        } else {
            t = 0;
            if (sNesExitMsg[0] != 0) {
                do {
                    t++;
                    sNesCpuMemory_2->buf[k] = 0xFC;
                    sNesCpuMemory_2->buf[k + 0x400] = 0xFC;
                    k++;
                } while (sNesExitMsg[t] != 0);
            }
        }
    }
    if (CART == 0xC661 || CART == 0xC66A) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = 0x3C;
            gSaveNesTrack_2 = SAVE;
            save_write(0, 0);
        }
    } else if (CART == 0xC641 || CART == 0xC69A) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = 0x3C;
            SAVE = gSaveNesTrack_2;
        }
    }
    if (!(FLAGS & 8) || sNesFrameCount < 10) {
        gDPPipeSync(sNesGfx++);
        gDPSetCycleType(sNesGfx++, G_CYC_FILL);
        t = FLAGS >> 5;
        gDPSetFillColor(sNesGfx++, sNesBorderColors[t]);
        {
            Gfx *g = sNesGfx++;
            g->words.w0 = 0xF64703C0;
            g->words.w1 = 0x90000;
        }
        gDPPipeSync(sNesGfx++);
        gDPSetCycleType(sNesGfx++, G_CYC_1CYCLE);
    }
    if (sNesRestoreTimer != 0) {
        for (i = 0; i < 4; i++) {
            BIG->e[i].a = 0;
            BIG->e[i].b = 0;
        }
    }
    *pgdl = sNesGfx;
    nes_audio_update();
}

#define NES_5 ((Nes_3 *)0x80380000)

typedef struct Nes_3 {
    u8 cpu[0x10000];
    u8 ppu[0x10000];
} Nes_3;

void nes_draw_bg_row(s32 row)
{
    s32 i, j, k, idx;
    u16 **pp;
    u8 *base;
    u8 *ntp;
    s32 nt;
    s32 y;
    s32 sx, fine;
    s32 col, x;
    s32 pal, key, ofs, tile, a, sh;

    if ((NES_5->cpu[0x2001] & 8) && sNesFrameCount >= 10) {
        sNesLastTileKey = -1;
        if (sNesBgTlutLoaded == 0) {
            k = 0x3F00;
            idx = 0;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    sNesBgTluts_2[gGfxFrameIndex][idx] = sNesPalette[NES_5->ppu[k]];
                    k++;
                    if (j == 0) {
                        sNesBgTluts_2[gGfxFrameIndex][idx] &= 0xFFFC;
                    } else {
                        sNesBgTluts_2[gGfxFrameIndex][idx] |= 3;
                    }
                    idx++;
                }
                idx += 12;
            }
            sNesBgTlutLoaded = 1;
            gDPPipeSync(sNesGfx++);
            gDPLoadTLUT_pal256(sNesGfx++, sNesBgTluts_2[gGfxFrameIndex]);
        }
        base = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        if (NES_5->cpu[0x2000] & 0x10) {
            base += 0x4000;
        }
        nt = (NES_5->cpu[0x2000] & 3) << 10;
        nt += 0x2000;
        ntp = &NES_5->ppu[nt];
        y = row << 5;
        sx = NES_5->ppu[0x411C] >> 3;
        fine = NES_5->ppu[0x411C] & 7;
        x = (0x1C - fine) << 2;
        nt += y;
        nt += sx;
        col = sx + 1;
        x += 0x20;
        nt++;

        for (i = col; i < 0x20; i++) {

            tile = NES_5->ppu[nt];
            ofs = tile << 6;
            a = (col >> 2) + ((row >> 2) << 3);
            sh = ((col >> 1) & 1) | (row & 2);
            sh <<= 1;
            pal = (ntp[a + 0x3C0] >> sh) & 3;
            key = tile | (pal << 8);
            if (key != sNesLastTileKey) {
                gDPLoadTextureBlock_4b(sNesGfx++, base + ofs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_WRAP, G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            sNesLastTileKey = key;
            gSPTextureRectangle(sNesGfx++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            nt++;
            col++;
        }
        nt = ((NES_5->cpu[0x2000] & 3) << 10) ^ 0x400;
        nt += 0x2000;
        ntp = &NES_5->ppu[nt];
        col = 0;
        nt += row << 5;
        if (fine != 0) {
            sx++;
        }
        for (i = 0; i < sx; i++) {

            tile = NES_5->ppu[nt];
            ofs = tile << 6;
            a = (col >> 2) + ((row >> 2) << 3);
            sh = ((col >> 1) & 1) | (row & 2);
            sh <<= 1;
            pal = (ntp[a + 0x3C0] >> sh) & 3;
            key = tile | (pal << 8);
            if (key != sNesLastTileKey) {
                gDPLoadTextureBlock_4b(sNesGfx++, base + ofs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            sNesLastTileKey = key;
            gSPTextureRectangle(sNesGfx++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            nt++;
            col++;
        }
    }
}

#define OAM ((u8 *)0x80394000)

void nes_draw_sprites(s32 arg0)
{
    s32 i, j, x, k, s, idx;
    u16 **pp;
    u8 *base;
    u16 *pal;
    u8 *spr;
    s32 t;
    s32 y;
    u16 c;
    u8 *NES = (u8 *)0x80380000;
    u8 *mem;

    if (NES[0x2001] & 0x10) {
        base = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        gDPSetRenderMode(sNesGfx++, 0x0C087008, 0);
        gDPSetCombine(sNesGfx++, 0xFFFFFF, 0xFFFCF279);
        gDPSetAlphaCompare(sNesGfx++, G_AC_THRESHOLD);
        if (NES[0x2000] & 8) {
            base += 0x4000;
        }
        k = 0x3F10;
        idx = 0;
        i = 0;
        mem = (u8 *)0x80380000;
        t = 0x10000;
        pal = sNesPalette;
        pp = sNesSpriteTluts;
    outer:
        for (j = 0; j < 4; j++) {
            x = pal[*(u8 *)(k + (s32)mem + t)] | 2;
            pp[gGfxFrameIndex][idx] = x;
            k++;
            if (j == 0) {
                pp[gGfxFrameIndex][idx] &= 0xFFFE;
            }
            idx++;
        }
        idx += 12;
        if (++i < 4) goto outer;
        gDPPipeSync(sNesGfx++);
        gDPLoadTLUT_pal256(sNesGfx++, sNesSpriteTluts[gGfxFrameIndex]);
        gDPPipeSync(sNesGfx++);
        for (i = 63; i >= 0; i--) {
            spr = OAM + i * 4;
            if (spr[0] < 0xF1 && !(spr[2] & 0x20)) {
                j = spr[1] * 64;
                gDPLoadTextureBlock_4b(sNesGfx++, base + j, G_IM_FMT_CI, 16, 8, spr[2] & 3,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
                x = spr[3];
                x = (x + 0x1C) * 4;
                y = spr[0] + 1;
                y <<= 2;
                if (!(spr[2] & 0x40)) {
                    gSPTextureRectangle(sNesGfx++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                } else {
                    gSPTextureRectangle(sNesGfx++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 7 << 5, 0, -1 << 10, 1 << 10);
                }
            }
        }
        gDPPipeSync(sNesGfx++);
        gDPSetAlphaCompare(sNesGfx++, G_AC_NONE);
    }
}

void nes_draw_back_sprites(s32 param)
{
    u8 *st;
    s32 bas;
    s32 i;
    s32 row, k, col;
    s32 t0;
    u8 *src;
    u16 *dst;
    Pts *e;
    u32 v;

    st = (u8 *)0x80380000;
    if (st[0x2001] & 0x10) {
        osInvalDCache(0, 0x400000);
        bas = sNesChrTiles;
        if (st[0x2000] & 8) {
            bas += 0x4000;
        }
        for (i = 63; i >= 0; i--) {
            e = (Pts *)0x80394000 + i;
            if (e->b0 < 0xF1 && (e->b2 & 0x20)) {
                t0 = e->b2 & 3;
                t0 <<= 4;
                row = e->b1 << 6;
                src = (u8 *)(bas + row);
                col = e->b3 + 0x1c;
                dst = gFramebuffers_2[param] + ((e->b0 + 1) * 0x140 + col);
                for (row = 0; row < 8; row++) {
                    col = e->b3 + 0x1c;
                    if (e->b2 & 0x40) {
                        src += 3;
                        for (k = 0; k < 4; k++) {
                            v = *src & 0xF;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[param][v + t0];
                            }
                            dst++;
                            col++;
                            v = *src >> 4;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[param][v + t0];
                            }
                            dst++;
                            col++;
                            src--;
                        }
                        src += 9;
                    } else {
                        for (k = 0; k < 4; k++) {
                            v = *src >> 4;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[param][v + t0];
                            }
                            dst++;
                            col++;
                            v = *src & 0xF;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[param][v + t0];
                            }
                            dst++;
                            col++;
                            src++;
                        }
                        src += 4;
                    }
                    dst += 0x138;
                }
            }
        }
        osWritebackDCache(gFramebuffers_2[param], 0x25800);
    }
}

void nes_audio_init(void)
{
    s32 i;
    void *q;

    sNesAudioFreq = osAiSetFrequency(0x5622);
    sNesAudioFrameSamples = ((sNesAudioChunks * sNesAudioFreq) / 60 + 7) & -8;
    for (i = 0; i < 2; i++) {
        q = mem_alloc_dbg(sNesAudioFrameSamples << 2, 0, D_800DAF38, 0x396);
        sNesAudioBufs[i] = (s32)q;
        mem_fill(q, 0, sNesAudioFrameSamples << 2);
    }
    sNesAudioBufIndex = 1;
}

#define SND ((SndMem *)0x80380000)

typedef struct {
    u16 f0;
    u16 step;
    s32 f4;
    s32 vol;
    s32 dec;
    u16 pos;
    u16 lim;
    s32 c;
    s32 d;
    s32 e;
    u32 flags;
} Chan_3;

typedef struct {
    u8 pad[0x1412C];
    Chan_3 ch[4];
} SndMem;

void nes_apu_render(s16 *p)
{
    s32 n, i, j, v;
    Chan_3 *c;
    s32 old;

    n = sNesAudioFrameSamples / sNesAudioChunks;
    if (sNesCpuMemory == 0) {
        return;
    }
    if (!(SND->ch[2].flags & 8) || SND->ch[2].d >= SND->ch[2].c || SND->ch[2].vol == 0) {
        mem_fill((u8 *)p, 0, n * 4);
    } else {
        for (j = 0; j < n * 2; j += 2) {
            SND->ch[2].pos += SND->ch[2].step;
            v = SND->ch[2].pos;
            v -= 0x4000;
            if (v > 0x4000) {
                v = 0x8000 - v;
            }
            v >>= 1;
            p[j] = v;
            p[j + 1] = v;
        }
    }
    for (i = 0; i < 2; i++) {
        if ((SND->ch[i].flags & 8) && SND->ch[i].d < SND->ch[i].c && SND->ch[i].vol != 0) {
                for (j = 0; j < n * 2; j += 2) {
                SND->ch[i].pos += SND->ch[i].step;
                if (SND->ch[i].lim < SND->ch[i].pos) {
                    v = 0x1000;
                } else {
                    v = -0x1000;
                }
                v = (v * SND->ch[i].vol) >> 16;
                p[j] += v;
                p[j + 1] += v;
            }
        }
    }
    if ((SND->ch[3].flags & 8) && SND->ch[3].d < SND->ch[3].c && SND->ch[3].vol != 0) {
        for (j = 0; j < n * 2; j += 2) {
            old = SND->ch[3].pos;
            SND->ch[3].pos += SND->ch[3].step;
            if (SND->ch[3].pos < old) {
                v = ((s32)random_byte() - 0x80) << 4;
                v = v * SND->ch[3].vol >> 16;
            }
            p[j] += v;
            p[j + 1] += v;
        }
    }
    for (i = 0; i < 4; i++) {
        SND->ch[i].d += SND->ch[i].e;
        SND->ch[i].vol -= SND->ch[i].dec;
        if (SND->ch[i].vol < 0) {
            if ((SND->ch[i].flags & 0x10) && i < 2) {
                SND->ch[i].vol = 0x10000;
            } else {
                SND->ch[i].vol = 0;
                SND->ch[i].dec = 0;
            }
        }
    }
}

void nes_audio_update(void)
{
    s32 i;
    s16 *p;

    osInvalDCache(0, 0x400000);
    if (osAiGetStatus_2() >= 0) {
        p = (s16 *)sNesAudioBufs[sNesAudioBufIndex];
        for (i = 0; i < sNesAudioChunks; i++) {
            nes_apu_render(p);
            p += (sNesAudioFrameSamples / sNesAudioChunks) * 2;
        }
        osWritebackDCache((s16 *)sNesAudioBufs[sNesAudioBufIndex], sNesAudioFrameSamples << 2);
        osAiSetNextBuffer((s16 *)sNesAudioBufs[sNesAudioBufIndex], sNesAudioFrameSamples << 2);
        sNesAudioBufIndex = sNesAudioBufIndex ^ 1;
    }
}

void nes_exit(void)
{
    s32 i;

    nes_audio_free();
    for (i = 0; i < 2; i++) {
        mem_free(sNesBgTluts[i]);
        mem_free(sNesSpriteTluts[i]);
    }
    mem_free(sNesSnapshot);
    mem_free(sNesChrTiles);
    mem_free(sNesPalette);
    gNesActive = 0;
    gRaceExitCode = 6;
}

void nes_audio_free(void)
{
    u32 *p = &sNesAudioBufs;
    mem_free(p[0]);
    mem_free(p[1]);
}

s32 nes_should_show_exit_msg(void)
{
    u8 *p = sNesMemory + 0x10000;
    if (p[0x2084] == 0x27 && p[0x2108] == 0x9A) {
        return 1;
    }
    return 0;
}

#define NES_6 ((Nes_4 *)0x80380000)

typedef struct { s32 a, b; u8 pad[0x1C]; } E;
typedef struct { u8 cpu[0x10000]; u8 pad[0x412C]; E e[4]; } Nes_4;

void nes_apu_reset_channels(void)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        NES_6->e[i].a = 0;
        NES_6->e[i].b = 0;
    }
}

void nes_init_saved_track(u8 *p)
{
    mem_fill(p, 0x40, 0x220);
    p[0] = 1;
    p[0xCF] = 9;
    mem_fill(p + 0xD0, 0, 0x30);
    p[0x100] = 1;
    p[0x1CF] = 9;
    mem_fill(p + 0x1D0, 0, 0x50);
    *(s16 *)(p + 0x200) = 0x5A10;
    p[0x204] = 0x2D;
    *(s16 *)(p + 0x20C) = -0x73FF;
    p[0x209] = 1;
    p[0x21B] = 1;
}

#define NES_7 (*(Nes_5 *)0x80380000)

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fc;
    s16 f10;
    u16 f12;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Rec_53;

typedef struct {
    u8 ram[0x10000];
    u8 pad[0x4100];
    u16 pc;
    u8 a, x, y, s, v, b, n, d, i, z, c, p;
    u8 pad2[0x1E];
    Rec_53 ch[5];
} Nes_5;

void nes_apu_update_square(s32 idx, s32 mode)
{
    Rec_53 *r;
    s32 per;
    s32 a0;
    s32 v;
    u8 *p;

    per = NES_7.ram[idx * 4 + 0x4002] | ((NES_7.ram[idx * 4 + 0x4003] & 7) << 8);
    r = &NES_7.ch[idx];
    if (per == 0) {
        per = 1;
    }
    per = 0x36EE8 / per;
    per = (per << 15) / 22050;
    r->f0 = per;
    a0 = NES_7.ram[idx * 4 + 0x4000] >> 6;
    r->f12 = sNesDutyTable[a0];
    a0 = NES_7.ram[idx * 4 + 0x4003] >> 3;
    if (a0 & 1) {
        a0 >>= 1;
        if (a0 == 0) {
            r->f14 = 0x7E;
        } else {
            r->f14 = a0;
        }
    } else {
        r->f14 = sNesLengthTable[a0 >> 1];
    }
    if (NES_7.ram[0x4000 + idx * 4] & 0x20) {
        r->f1c = 0;
        r->f18 = 0;
    } else {
        r->f1c = 1;
    }
    if (mode == 3) {
        r->f18 = 0;
    }
    if ((NES_7.ram[0x4015] >> idx) & 1) {
        r->f20 |= 8;
    } else {
        r->f20 &= ~8;
    }
    if (mode == 0) {
        r->f8 = 0xFFFF;
    }
    p = &NES_7.ram[0x4000 + idx * 4];
    a0 = *p & 0x10;
    v = *p & 0xF;
    if (a0) {
        r->f8 = (v << 16) / 16;
        r->fc = 0;
        r->f20 &= ~0x10;
        return;
    }
    r->fc = 0x4000 / (v + 1);
    if (*p & 0x20) {
        r->f20 |= 0x10;
        return;
    }
    r->f20 &= ~0x10;
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Chan_4;

void nes_apu_update_triangle(s32 n, s32 m)
{
    u8 *B;
    Chan_4 *c;
    s32 v;
    s32 bVar1;
    s32 k;
    u8 *p;

    B = (u8 *)0x80380000;
    k = n * 4;
    k += 0x4003;
    p = B + k;
    v = B[n * 4 + 0x4002] | ((*p & 7) << 8);
    c = (Chan_4 *)0x8039412C + n;
    if (v == 0) {
        v = 1;
    }
    v = 0x36EE8 / v;
    v = (v << 14) / 0x5622;
    c->f00 = v;
    bVar1 = *p >> 3;
    if (bVar1 & 1) {
        bVar1 >>= 1;
        if (bVar1 == 0) {
            c->f14 = 0x7e;
        } else {
            c->f14 = bVar1;
        }
    } else {
        c->f14 = sNesLengthTable[bVar1 >> 1];
    }
    if ((*(u8 *)(0x80384000 + n * 4) & 0x20) != 0) {
        c->f1c = 0;
        c->f18 = 0;
    } else {
        c->f1c = 1;
    }
    if (m == 3) {
        c->f18 = 0;
    }
    if (((sNesReg4015 >> n) & 1) != 0) {
        c->f20 = c->f20 | 8;
    } else {
        c->f20 = c->f20 & ~8;
    }
    c->f08 = 0xffff;
    bVar1 = *(u8 *)(0x80384000 + n * 4);
    if ((bVar1 & 0x80) != 0) {
        c->f0c = 0;
        return;
    }
    c->f0c = 0x40000 / ((bVar1 & 0x7f) + 1);
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
} Chan_5;

void nes_apu_update_noise(s32 n, s32 m)
{
    u8 *B;
    Chan_5 *c;
    s32 v;
    s32 b;
    u8 *p;
    u8 *q;
    s32 k;
    s32 t;
    s32 e, mm;
    s32 *tb;

    tb = sNesNoisePeriods;
    B = (u8 *)0x80380000;
    k = n * 4 + 0x4002;
    v = tb[B[k] & 0xF];
    v = (v * 0x28F) / 0xDC;
    p = B + n * 4;
    c = (Chan_5 *)0x8039412C + n;
    c->f00 = v * 4;
    b = p[0x4003] >> 3;
    if (b & 1) {
        b >>= 1;
        if (b == 0) {
            c->f14 = 0x7e;
        } else {
            c->f14 = b;
        }
    } else {
        c->f14 = sNesLengthTable[b >> 1];
    }
    if ((*(u8 *)(0x80384000 + n * 4) & 0x20) != 0) {
        c->f1C = 0;
        c->f18 = 0;
    } else {
        c->f1C = 1;
    }
    if (m == 3) {
        c->f18 = 0;
    }
    if (((sNesReg4015 >> n) & 1) != 0) {
        c->f20 = c->f20 | 8;
    } else {
        c->f20 = c->f20 & ~8;
    }
    if (m == 0) {
        c->f08 = 0xffff;
    }
    k = n * 4;
    q = (u8 *)(0x80384000 + k);
    e = *q;
    t = e & 0x10;
    mm = e & 0xF;
    if (t != 0) {
        c->f08 = (mm << 16) / 16;
        c->f0C = 0;
        c->f20 = c->f20 & ~0x10;
    } else {
        c->f0C = 0x4000 / (mm + 1);
        if ((*q & 0x20) != 0) {
            c->f20 = c->f20 | 0x10;
        } else {
            c->f20 = c->f20 & ~0x10;
        }
    }
}

#define NES_8 (*(Nes_6 *)0x80380000)

typedef struct {
    u8 b[0x100];
} Page;

typedef struct {
    u8 ram[0x10000];
    u8 vram[0x4000];
    u8 oam[0x100];
    u16 pc;
    u8 a, x, y, s, v, b, n, d, i, z, c, p;
    u8 pad0[6];
    u16 vaddr;
    u16 pad1;
    u32 ctl;
    u8 sx;
    u8 sy;
    u8 pad2;
    u8 oamaddr;
    u32 pad3;
    u32 strobe;
} Nes_6;

void nes_write_io(s32 addr, s32 value)
{
    s32 k;
    Page *src;

    if (addr == 0x2007) {
        NES_8.vram[NES_8.vaddr] = value;
        if (NES_8.ram[0x2000] & 4) {
            NES_8.vaddr = (NES_8.vaddr + 32) % 0x4000;
        } else {
            NES_8.vaddr = (NES_8.vaddr + 1) % 0x4000;
        }
    } else if (addr == 0x2006) {
        if (NES_8.ctl & 1) {
            NES_8.vaddr = (NES_8.vaddr & 0xFF00) | value;
            NES_8.ctl |= 2;
        } else {
            NES_8.vaddr = (NES_8.vaddr & 0xFF) | (value << 8);
        }
        NES_8.ctl ^= 1;
    } else if (addr == 0x4016) {
        if (value & 1) {
            NES_8.strobe = 0x7B;
        } else if (NES_8.strobe == 0x7B) {
            NES_8.strobe = 0;
        }
    } else if (addr == 0x2005) {
        if (NES_8.ctl & 4) {
            NES_8.sy = value;
        } else {
            NES_8.sx = value;
        }
        NES_8.ctl ^= 4;
    } else if (addr == 0x2003) {
        NES_8.oamaddr = value;
    } else if (addr == 0x2004) {
        NES_8.oam[NES_8.oamaddr++] = value;
    } else if (addr == 0x4014) {
        src = (Page *)&NES_8.ram[value << 8];
        *(Page *)NES_8.oam = *src;
    } else {
        k = addr - 0x4000;
        if ((u32)k < 0x16) {
            NES_8.ram[addr] = value;
            k >>= 2;
            if (k < 2) {
                nes_apu_update_square(k, addr & 3);
            } else if (k == 2) {
                nes_apu_update_triangle(2, addr & 3);
            } else if (k == 3) {
                nes_apu_update_noise(3, addr & 3);
            }
        }
    }
    NES_8.ram[addr] = value;
}

u32 nes_read_io(s32 code, s32 unused, u32 val)
{
    u8 *ram;
    u8 *reg;
    s32 pos;
    s32 v;
    s32 n;
    s32 i;
    u32 mask;

    if (code == 0x2007) {
        ram = (u8 *)0x80380000;
        __asm__ __volatile__("");
        code = *(u16 *)(ram + 0x14114);
        unused = *(s32 *)(ram + 0x14118);
        val = ram[code + 0x10000];
        if (unused & 2) {
            *(s32 *)(ram + 0x14118) = unused & ~2;
        } else {
            if (ram[0x2000] & 4) {
                *(u16 *)(ram + 0x14114) = (code + 0x20) % 0x4000;
            } else {
                *(u16 *)(ram + 0x14114) = (code + 1) % 0x4000;
            }
        }
        return val;
    }
    if (code == 0x4016) {
        reg = (u8 *)0x80390000;
        n = *(s32 *)(reg + 0x4124);
        if (n < 8) {
            *(s32 *)(reg + 0x4124) = n + 1;
            val = (reg[0x4120] >> n) & 1;
        }
        return val;
    }
    if (code == 0x4015) {
        ram = (u8 *)0x80380000;
        val = 0;
        for (i = 0; i < 4; i++) {
            u8 *c = ram + i * 0x24 + 0x10000;
            if ((*(s32 *)(c + 0x414C) & 8) && *(s32 *)(c + 0x4144) < D_803941AC && *(s32 *)(c + 0x4134) != 0) {
                val |= 1 << i;
            }
        }
        return val & *(u8 *)0x80384015;
    }
    return *(u8 *)(code + 0x80380000);
}

void nes_op_asl(void)
{
    s32 addr = sNesEffAddr;

    if (addr == -1) {
        NES_2.c = NES_2.a & 0x80;
        NES_2.a <<= 1;
        NES_2.z = NES_2.a == 0;
        NES_2.n = NES_2.a & 0x80;
    } else {
        u32 v = sNesOperand << 1;
        NES_2.c = ((u8 *)&sNesOperand)[3] & 0x80;
        sNesOperand = v;
        if (addr < 0x2000) {
            NES_2.ram[addr & 0x7FF] = v;
        } else if (addr < 0x8000) {
            nes_write_io(addr, v);
        }
        NES_2.z = (sNesOperand & 0xFF) == 0;
        NES_2.n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_asr(void)
{
    if (sNesEffAddr == -1) {
        NES_2.c = NES_2.a & 1;
        NES_2.a >>= 1;
        if (NES_2.a & 0x40) {
            NES_2.a |= 0x80;
        }
        NES_2.z = NES_2.a == 0;
        NES_2.n = NES_2.a & 0x80;
    } else {
        NES_2.c = sNesOperand & 1;
        sNesOperand >>= 1;
        if (sNesOperand & 0x40) {
            sNesOperand |= 0x80;
        }
        wr(sNesEffAddr, sNesOperand);
        NES_2.z = (sNesOperand & 0xFF) == 0;
        NES_2.n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_rol(void)
{
    s32 bit;

    if (sNesEffAddr == -1) {
        bit = NES_2.a & 0x80;
        NES_2.a <<= 1;
        if (NES_2.c) {
            NES_2.a |= 1;
        }
        NES_2.c = bit;
        NES_2.z = NES_2.a == 0;
        NES_2.n = NES_2.a & 0x80;
    } else {
        bit = sNesOperand & 0x80;
        sNesOperand <<= 1;
        if (NES_2.c) {
            do { sNesOperand |= 1; } while (0);
        }
        NES_2.c = bit;
        wr(sNesEffAddr, sNesOperand);
        NES_2.z = (sNesOperand & 0xFF) == 0;
        NES_2.n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_ror(void)
{
    s32 bit;

    if (sNesEffAddr == -1) {
        bit = NES_2.a & 1;
        NES_2.a >>= 1;
        if (NES_2.c) {
            NES_2.a |= 0x80;
        }
        NES_2.c = bit;
        NES_2.z = NES_2.a == 0;
        NES_2.n = NES_2.a & 0x80;
    } else {
        bit = sNesOperand & 1;
        sNesOperand >>= 1;
        if (NES_2.c) {
            do { sNesOperand |= 0x80; } while (0);
        }
        NES_2.c = bit;
        wr(sNesEffAddr, sNesOperand);
        NES_2.z = (sNesOperand & 0xFF) == 0;
        NES_2.n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_lsr(void)
{
    s32 a2 = sNesEffAddr;

    if (a2 == -1) {
        u8 *g = (u8 *)0x80390000;
        u8 b = g[0x4102];
        g[0x410C] = b & 1;
        b = b >> 1;
        g[0x4102] = b;
        g[0x410B] = (b == 0);
        g[0x4108] = b & 0x80;
        return;
    } else {
        u8 *pb = (u8 *)&sNesOperand;
        u8 lo = pb[3];
        s32 w;

        *(u8 *)0x8039410C = lo & 1;
        w = sNesOperand >> 1;
        sNesOperand = w;
        if (a2 < 0x2000) {
            *(u8 *)(0x80380000 + (a2 & 0x7FF)) = (u8)w;
        } else if (a2 <= 0x7FFF) {
            nes_write_io(a2, w);
        }
    }
    {
        u8 *t = (u8 *)0x80390000;
        t[0x410B] = ((sNesOperand & 0xFF) == 0);
        t[0x4108] = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_brk(void)
{
    NES_3->pc++;
    NES_3->ram[0x100 + NES_3->s] = NES_3->pc >> 8;
    NES_3->s--;
    NES_3->ram[0x100 + NES_3->s] = NES_3->pc;
    NES_3->b = 1;
    NES_3->s--;
    NES_3->p = (NES_3->n ? 0x80 : 0) | (NES_3->v ? 0x40 : 0) | 0x20 | (NES_3->b ? 0x10 : 0) | (NES_3->d ? 8 : 0) | (NES_3->i ? 4 : 0) | (NES_3->z ? 2 : 0) | (NES_3->c ? 1 : 0);
    NES_3->ram[0x100 + NES_3->s] = NES_3->p;
    NES_3->i = 1;
    NES_3->s--;
    NES_3->pc = NES_3->ram[0xFFFE];
    NES_3->pc |= NES_3->ram[0xFFFF] << 8;
}

void nes_op_php(void)
{
    NES_2.p = (NES_2.n ? 0x80 : 0) | (NES_2.v ? 0x60 : 0x20)
        | (NES_2.b ? 0x10 : 0) | (NES_2.d ? 8 : 0) | (NES_2.i ? 4 : 0)
        | (NES_2.z ? 2 : 0) | (NES_2.c ? 1 : 0);
    NES_2.ram[0x100 + NES_2.s] = NES_2.p;
    NES_2.s--;
}

void nes_op_rti(void)
{
    u32 v;
    u32 bits;
    u32 base;

    base = 0x80380100;
    v = (u32)sNesRegS + 1;
    *(u8 *)0x80394105 = v;
    v = v & 0xFF;
    bits = *(u8 *)(base | v);
    *(u8 *)0x80394108 = bits & 0x80;
    *(u8 *)0x80394106 = bits & 0x40;
    *(u8 *)0x80394107 = bits & 0x10;
    *(u8 *)0x80394109 = bits & 0x8;
    *(u8 *)0x8039410A = bits & 0x4;
    *(u8 *)0x8039410B = bits & 0x2;
    *(u8 *)0x8039410C = bits & 0x1;
    nes_op_rts();
}

void nes_cpu_nmi(void)
{
    NES_3->ram[0x100 + NES_3->s] = NES_3->pc >> 8;
    NES_3->s--;
    NES_3->ram[0x100 + NES_3->s] = NES_3->pc;
    NES_3->s--;
    NES_3->p = (NES_3->n ? 0x80 : 0) | (NES_3->v ? 0x40 : 0) | 0x20 | (NES_3->b ? 0x10 : 0) | (NES_3->d ? 8 : 0) | (NES_3->i ? 4 : 0) | (NES_3->z ? 2 : 0) | (NES_3->c ? 1 : 0);
    NES_3->ram[0x100 + NES_3->s] = NES_3->p;
    NES_3->i = 1;
    NES_3->s--;
    NES_3->pc = NES_3->ram[0xFFFA];
    NES_3->pc |= NES_3->ram[0xFFFB] << 8;
}

#define CUR (*(Ent_88 **)(glob + 0x5BB4))
#define PC (cpu->pc)
#define CYC (cpu->cyc)
#define R2000 (ram[0x2000])
#define R2002 (ram[0x2002])

typedef struct Ent_88 {
    u8 pad0[8];
    void (*f8)(void);
    void (*fC)(struct Ent_88 *);
    s16 f10;
    u16 f12;
    u8 pad1[4];
} Ent_88;

typedef struct { u8 pad[0x4100]; u16 pc; u8 pad2[14]; u32 cyc; } Cpu;

extern Ent_88 sNesOpcodeTable[];

void nes_run_frame(void)
{
    s32 i = 0;
    s32 line;
    u8 *glob = (u8 *)0x800F0000;
    u8 *ram = (u8 *)0x80380000;
    Cpu *cpu = (Cpu *)0x80390000;

    do {
        line = (i + 0xf0) % 0xf8;
        if (*(u32 *)0x80394110 < 0x78) {
            Ent_88 *tb = sNesOpcodeTable;
            u8 *dseg = (u8 *)0x800D0000;
            do {
                Ent_88 *e;
                CUR = (Ent_88 *)((s32)(*(u8 *)(PC + (s32)ram) * sizeof(Ent_88)) + (s32)tb);
                CUR->f8();
                e = CUR;
                PC += e->f12;
                e->fC(e);
                (*(s32 *)(dseg - 0x13E4))++;
                CYC += CUR->f10;
            } while (CYC < 0x78);
        }
        CYC -= 0x78;
        if ((line & 7) == 0) {
            if (line == 0) {
                R2002 = 0x10;
            } else if (line == 0xf0) {
                R2002 |= 0x80;
                if (R2000 & 0x80) nes_cpu_nmi();
            }
        }
        if ((line & 7) == 4 && line < 0xf0 && sNesRowCallback_2 != 0) sNesRowCallback_2(line >> 3);
        if (line == *(u8 *)0x80394000 && line < 0xf0) {
            R2002 |= 0x40;
            if (R2000 & 0x40) nes_cpu_nmi();
        }
        i++;
    } while (i < 0xf8);
    {
        u8 *c2 = (u8 *)0x80390000;
        (*(s32 *)(c2 + 0x41BC))++;
    }
}

s32 nes_cpu_reset(s32 param_1, s32 param_2) {
    u8 *nes = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80388000;
    sNesCpuMemory = param_1;
    sNesRowCallback = param_2;
    nes[0x4106] = 0;
    nes[0x4107] = 0;
    nes[0x4109] = 0;
    nes[0x410A] = 1;
    nes[0x410B] = 0;
    nes[0x410C] = 0;
    *(u16 *)(nes + 0x4100) = (u16)(ram[0x7FFC] | (ram[0x7FFD] << 8));
    return 0;
}

void nes_op_adc(void) {
    u8 *nes = (u8 *)0x80390000;
    u32 sum = nes[0x4102];
    u32 old = sum;
    u32 m = 0xFFFFFF80;
    sum = sum + sNesOperand;
    if (nes[0x410C] != 0) {
        sum = sum + 1;
    }
    nes[0x410B] = (sum & 0xFF) == 0;
    nes[0x4108] = sum & m;
    nes[0x410C] = (sum & 0xFFFFFF00) != 0;
    nes[0x4102] = sum;
    nes[0x4106] = (old ^ sum) & m;
}

void nes_op_and(void)
{
    u8 *nes;
    u8 v;
    u8 z;
    u8 w;

    nes = (u8 *)0x80390000;
    v = nes[0x4102] & sNesOperand8;
    z = (v == 0);
    nes[0x4102] = v;
    w = v & 0x80;
    nes[0x410B] = z;
    nes[0x4108] = w;
}

void nes_op_bcc(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x410C] == 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bcs(void)
{
    u8 *st;
    s32 a;
    s32 b;
    u32 addr;

    if (D_800D05D4 != 0) {
        a = osGetCount();
        addr = ((a << 2) & 0xFFFFF) + 0x80000000;
        b = osGetCount();
        *(s32 *)addr = b;
    }
    st = (u8 *)0x80390000;
    if (st[0x410C] != 0) {
        *(u16 *)(st + 0x4100) = sNesOperand;
        *(s32 *)(st + 0x4110) += 2;
    }
}

void nes_op_beq(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x410B] != 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bit(void) {
    u8 *nes = (u8 *)0x80390000;
    nes[0x410B] = ((nes[0x4102] & sNesOperand) == 0);
    {
        u8 b = ((u8 *)&sNesOperand)[3];
        nes[0x4108] = b & 0x80;
        nes[0x4106] = b & 0x40;
    }
}

void nes_op_bmi(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x4108] != 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bne(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x410b] == 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bpl(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x4108] == 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bvc(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x4106] == 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_bvs(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x4106] != 0) {
        h = sNesOperand16;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}

void nes_op_clc(void)
{
    *(u8 *)0x8039410C = 0;
}

void nes_op_cld(void)
{
    *(u8 *)0x80394109 = 0;
}

void nes_op_cli(void)
{
    *(u8 *)0x8039410A = 0;
}

void nes_op_clv(void)
{
    *(u8 *)0x80394106 = 0;
}

void nes_op_cmp(void) {
    u8 *nes = (u8 *)0x80390000;
    s32 a = nes[0x4102];
    s32 b = sNesOperand;
    if (b < a) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 0;
        return;
    }
    if (a == b) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 1;
        return;
    }
    nes[0x4108] = 1;
    nes[0x410C] = 0;
    nes[0x410B] = 0;
}

void nes_op_cpx(void) {
    u8 *nes = (u8 *)0x80390000;
    s32 a = nes[0x4103];
    s32 b = sNesOperand;
    if (b < a) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 0;
        return;
    }
    if (a == b) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 1;
        return;
    }
    nes[0x4108] = 1;
    nes[0x410C] = 0;
    nes[0x410B] = 0;
}

void nes_op_cpy(void) {
    u8 *nes = (u8 *)0x80390000;
    s32 a = nes[0x4104];
    s32 b = sNesOperand;
    if (b < a) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 0;
        return;
    }
    if (a == b) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 1;
        return;
    }
    nes[0x4108] = 1;
    nes[0x410C] = 0;
    nes[0x410B] = 0;
}

void nes_op_dec(void)
{
    u8 *st;

    sNesOperand = sNesOperand - 1;
    wr_2(sNesEffAddr, sNesOperand);
    st = (u8 *)0x80390000;
    st[0x410B] = (sNesOperand & 0xFF) == 0;
    st[0x4108] = sNesOperand & 0x80;
}

void nes_op_dex(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 v = nes[0x4103] + -1;
    u8 m;
    nes[0x4103] = v;
    m = (u8)v & 0x80;
    nes[0x410B] = ((u8)v == 0);
    nes[0x4108] = m;
}

void nes_op_dey(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 v = nes[0x4104] + -1;
    u8 m;
    nes[0x4104] = v;
    m = (u8)v & 0x80;
    nes[0x410B] = ((u8)v == 0);
    nes[0x4108] = m;
}

void nes_op_eor(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4102] ^ sNesOperand8;
    nes[0x4102] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_op_inc(void) {
    u8 *nes;
    s32 c = sNesOperand + 1;

    sNesOperand = c;
    wr_2(sNesEffAddr, c);
    nes = (u8 *)0x80390000;
    nes[0x410B] = (sNesOperand & 0xFF) == 0;
    nes[0x4108] = ((u8 *)&sNesOperand)[3] & 0x80;
}

void nes_op_inx(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 v = nes[0x4103] + 1;
    u8 m;
    nes[0x4103] = v;
    m = (u8)v & 0x80;
    nes[0x410B] = ((u8)v == 0);
    nes[0x4108] = m;
}

void nes_op_iny(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 v = nes[0x4104] + 1;
    u8 m;
    nes[0x4104] = v;
    m = (u8)v & 0x80;
    nes[0x410B] = ((u8)v == 0);
    nes[0x4108] = m;
}

void nes_op_jmp(void)
{
    *(u16 *)0x80394100 = sNesOperand16;
}

void nes_op_jsr(void) {
    u8 *nes = (u8 *)0x80390000;
    u32 base = 0x80380000;
    u8 v;
    u32 i;
    u32 j;

    i = nes[0x4105];
    i += 0x100;
    *(u8 *)(base + i) = (u8)(*(u16 *)(nes + 0x4100) >> 8);
    v = nes[0x4101];
    nes[0x4105] = nes[0x4105] - 1;
    j = (u8)nes[0x4105];
    j += 0x100;
    *(u8 *)(base + j) = v;
    nes[0x4105] = nes[0x4105] - 1;
    *(u16 *)(nes + 0x4100) = sNesOperand16;
}

void nes_op_lda(void)
{
    u8 *nes = (u8 *)0x80390000;

    nes[0x4102] = sNesOperand;
    nes[0x410B] = (sNesOperand & 0xFF) == 0;
    nes[0x4108] = sNesOperand & 0x80;
}

void nes_op_ldx(void)
{
    u8 *nes = (u8 *)0x80390000;
    s8 b = sNesOperand;
    nes[0x4103] = b;
    nes[0x410B] = (sNesOperand & 0xFF) == 0;
    nes[0x4108] = b & 0x80;
}

void nes_op_ldy(void)
{
    u8 *nes = (u8 *)0x80390000;

    nes[0x4104] = sNesOperand;
    nes[0x410B] = (sNesOperand & 0xFF) == 0;
    nes[0x4108] = sNesOperand & 0x80;
}

void nes_op_nop(void)
{
}

void nes_op_ora(void)
{
    u8 *nes;
    u8 v;
    u8 z;
    u8 w;

    nes = (u8 *)0x80390000;
    v = nes[0x4102] | sNesOperand8;
    z = (v == 0);
    nes[0x4102] = v;
    w = v & 0x80;
    nes[0x410B] = z;
    nes[0x4108] = w;
}

void nes_op_pha(void)
{
    u8 *nes;
    u32 a;

    nes = (u8 *)0x80390000;
    a = nes[0x4105] | 0x80380100;
    *(u8 *)a = nes[0x4102];
    nes[0x4105] = nes[0x4105] - 1;
}

void nes_op_pla(void)
{
    u8 *nes;
    u32 a;

    nes = (u8 *)0x80390000;
    nes[0x4105] = nes[0x4105] + 1;
    a = (u8)nes[0x4105] | 0x80380100;
    nes[0x4102] = *(u8 *)a;
}

/* 6502 PLP: pull the status byte from the stack page and split it into the flag bytes. */
void nes_op_plp(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    u32 v;
    s32 a;

    st[0x4105]++;
    a = st[0x4105] + 0x100;
    v = ram[a];
    st[0x4108] = v & 0x80;
    st[0x4106] = v & 0x40;
    st[0x4107] = v & 0x10;
    st[0x4109] = v & 0x08;
    st[0x410A] = v & 0x04;
    st[0x410B] = v & 0x02;
    st[0x410C] = v & 0x01;
}

/* 6502 RTS-style pull of a 16-bit word from the stack page. */
void nes_op_rts(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    u32 lo, hi;
    s32 a, b;

    st[0x4105]++;
    a = st[0x4105] + 0x100;
    lo = ram[a];
    st[0x4105]++;
    b = st[0x4105] + 0x100;
    hi = ram[b];
    *(u16 *)(st + 0x4100) = lo | (hi << 8);
}

void nes_op_sbc(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 r = nes[0x4102];
    s32 a = r;
    s32 x;
    r = r - sNesOperand;
    if (nes[0x410C] == 0) {
        r = r - 1;
    }
    nes[0x410C] = ((u32)~r) >> 31;
    nes[0x410B] = ((r & 0xFF) == 0);
    nes[0x4108] = r & -0x80;
    x = a ^ r;
    nes[0x4102] = r;
    nes[0x4106] = x & -0x80;
}

void nes_op_sec(void)
{
    *(u8 *)0x8039410C = 1;
}

void nes_op_sed(void)
{
    *(u8 *)0x80394109 = 1;
    fatal_halt(1);
}

void nes_op_sei(void)
{
    *(u8 *)0x8039410A = 1;
}

void nes_op_sta(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 a = sNesEffAddr;
    u32 v = nes[0x4102];
    if (a < 0x2000) {
        *(u8 *)((a & 0x7FF) + 0x80380000) = v;
    } else if (a < 0x8000) {
        nes_write_io(a, v);
    }
}

void nes_op_stx(void) {
    s32 a = sNesEffAddr;
    u32 v = *(u8 *)0x80394103;
    if (a < 0x2000) {
        ((u8 *)0x80380000)[a & 0x7FF] = v;
    } else if (a < 0x8000) {
        nes_write_io(a, v);
    }
}

void nes_op_sty(void) {
    s32 addr = sNesEffAddr;
    s32 value = *(u8 *)0x80394104;

    if (addr < 0x2000) {
        ((u8 *)0x80380000)[addr & 0x7FF] = value;
    } else if (addr < 0x8000) {
        nes_write_io(addr, value);
    }
}

void nes_op_tax(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4102];
    nes[0x4103] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_op_tay(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4102];
    nes[0x4104] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_op_tsx(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4105];
    nes[0x4103] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_op_txa(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4103];
    nes[0x4102] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_op_txs(void)
{
    u8 *b = (u8 *)0x80390000;
    b[0x4105] = b[0x4103];
}

void nes_op_tya(void)
{
    u8 *nes = (u8 *)0x80390000;
    u8 v = nes[0x4104];
    nes[0x4102] = v;
    nes[0x410B] = (v == 0);
    nes[0x4108] = v & 0x80;
}

void nes_am_implied_stub(void)
{
}

void nes_am_implied(void)
{
}

void nes_am_accumulator_stub(void)
{
}

void nes_am_accumulator(void)
{
    sNesEffAddr = -1;
    sNesOperand = *(u8 *)0x80394102;
}

void nes_am_absolute_stub(void)
{
}

void nes_am_absolute(void) {
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    s32 a, r;

    sNesEffAddr = p[1] | (nes[(u16)(i + 2)] << 8);
    a = sNesEffAddr;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = nes_read_io_2(a);
        goto store;
    }
    r = *(u8 *)(0x80380000 + a);
store:
    sNesOperand = r;
}

void nes_am_zeropage_stub(void)
{
}

static inline s32 rd2(s32 a)
{
    if (a < 0x2000) {
        a = a & 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        return nes_read_io_2(a);
    }
    return *(u8 *)(a + 0x80380000);
}

void nes_am_zeropage(void)
{
    s32 a = *(u8 *)(*(u16 *)0x80394100 + 0x80380001);
    s32 r;
    sNesEffAddr = a;
    r = rd2(a);
    sNesOperand = r;
}

void nes_am_immediate_stub(void)
{
}

void nes_am_immediate(void)
{
    u8 *nes;
    u8 *ram;
    u32 idx;

    nes = (u8 *)0x80390000;
    ram = (u8 *)0x80380000;
    sNesEffAddr = -1;
    idx = *(u16 *)(nes + 0x4100);
    sNesOperand = ram[idx + 1];
}

void nes_am_absolute_x_stub(void)
{
}

void nes_am_absolute_x(void) {
    u8 *st = (u8 *)0x80390000;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 s, r, a;

    s = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4103];
    sNesEffAddr = s;
    a = (u16)s;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = nes_read_io_2(a);
        goto store;
    }
    r = *(u8 *)(0x80380000 + a);
store:
    sNesOperand = r;
}

void nes_am_absolute_y_stub(void)
{
}

void nes_am_absolute_y(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    u32 r;
    s32 a;

    a = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4104];
    sNesEffAddr = a;
    r = rd((u16)a);
    sNesOperand = r;
}

void nes_am_indexed_indirect_stub(void)
{
}

void nes_am_indexed_indirect(void)
{
    u32 *out = &sNesEffAddr;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 a;
    u32 r;

    a = p[1] + st[0x4103];
    *out = a;
    *out = nes[a] | (nes[(u16)(a + 1)] << 8);
    r = rd(*out);
    sNesOperand = r;
}

void nes_am_indirect_indexed_stub(void)
{
}

void nes_am_indirect_indexed(void)
{
    u32 *out = &sNesEffAddr;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 a, b;
    u32 r;

    a = p[1];
    *out = a;
    b = (nes[a] | (nes[(u16)(a + 1)] << 8)) + st[0x4104];
    *out = b;
    r = rd((u16)b);
    sNesOperand = r;
}

void nes_am_zeropage_x_stub(void)
{
}

void nes_am_zeropage_x(void) {
    u8 *st = (u8 *)0x80390000;
    u8 *nes = (u8 *)0x80380000;
    s32 a, r;

    sNesEffAddr = nes[*(u16 *)(st + 0x4100) + 1] + st[0x4103];
    a = sNesEffAddr;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = nes_read_io_2(a);
        goto store;
    }
    r = *(u8 *)(0x80380000 + a);
store:
    sNesOperand = r;
}

void nes_am_zeropage_y_stub(void)
{
}

static inline u32 rd2_2(s32 a)
{
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a <= 0x7FFF) {
        return nes_read_io_3(a);
    }
    return *(u8 *)(a + 0x80380000);
}

void nes_am_zeropage_y(void)
{
    u8 *st = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80380000;
    s32 a;
    a = ram[*(u16 *)(st + 0x4100) + 1] + st[0x4104];
    sNesEffAddr = a;
    sNesOperand_2 = rd2_2(a);
}

void nes_am_relative_stub(void)
{
}

void nes_am_relative(void)
{
    s32 *dst = &sNesOperand;
    u32 a = *(u16 *)0x80394100;
    s32 v = a + *(s8 *)(a + 0x80380000 + 1) + 2;
    *dst = v & 0xFFFF;
}

void nes_am_jump_absolute_stub(void)
{
}

void nes_am_jump_absolute(void)
{
    u32 *out = &sNesOperand;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    *out = p[1] | (nes[(u16)(i + 2)] << 8);
}

void nes_am_jump_indirect_stub(void)
{
}

void nes_am_jump_indirect(void)
{
    u32 *out = &sNesOperand;
    u8 *nes = (u8 *)0x80380000;
    s32 i = *(u16 *)0x80394100;
    u8 *p = nes + i;
    u16 v = (nes[(u16)(i + 2)] << 8) | p[1];
    u8 *q = nes + v;
    u32 w;
    *out = v;
    w = q[0] | (nes[(u16)(v + 1)] << 8);
    *out = w;
}

void nes_ea_absolute(void)
{
    s32 *dst = &sNesEffAddr;
    u8 *base = (u8 *)0x80380000;
    u32 h = *(u16 *)0x80394100;
    u8 *p = (u8 *)((u32)base + h);
    *dst = (u32)p[1] | ((u32)base[(u16)(h + 2)] << 8);
}

void nes_ea_zeropage(void)
{
    u32 a = *(u16 *)0x80394100;
    sNesEffAddr = *(u8 *)(a + 0x80380000 + 1);
}

void nes_ea_absolute_x(void)
{
    u32 *out = &sNesEffAddr;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    *out = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4103];
}

void nes_ea_absolute_y(void)
{
    u32 *out = &sNesEffAddr;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    *out = (p[1] | (nes[(u16)(i + 2)] << 8)) + st[0x4104];
}

void nes_ea_indexed_indirect(void)
{
    u32 *out = &sNesEffAddr;
    u8 *nes = (u8 *)0x80380000;
    u8 *st = (u8 *)0x80390000;
    s32 i = *(u16 *)(st + 0x4100);
    u8 *p = nes + i;
    s32 a = p[1] + st[0x4103];
    *out = a;
    *out = nes[a] | (nes[(u16)(a + 1)] << 8);
}

#define RAM ((u8 *)0x80380000)

static inline u32 rd_2(u32 addr) { return RAM[addr]; }

void nes_ea_indirect_indexed(void)
{
    u8 *st = (u8 *)0x80390000;
    u32 a = rd_2(*(u16 *)(st + 0x4100) + 1);
    sNesEffAddr = a;
    sNesEffAddr = (rd_2(a) | (rd_2(a + 1) << 8)) + st[0x4104];
}

void nes_ea_zeropage_x(void)
{
    u8 *nes = (u8 *)0x80390000;
    u32 a = *(u16 *)(nes + 0x4100);
    sNesEffAddr = *(u8 *)(a + 0x80380000 + 1) + nes[0x4103];
}

void nes_ea_zeropage_y(void)
{
    u8 *nes;
    u8 *ram;
    u32 idx;

    nes = (u8 *)0x80390000;
    ram = (u8 *)0x80380000;
    idx = *(u16 *)(nes + 0x4100);
    sNesEffAddr = ram[idx + 1] + nes[0x4104];
}

void nes_apu_write(s32 p, u8 v)
{
    s32 u;

    ((u8 *)0x80380000)[p] = v;
    u = (p - 0x4000) >> 2;
    if (u < 2) {
        nes_apu_update_square(u, p & 3);
    } else if (u == 2) {
        nes_apu_update_triangle(2, p & 3);
    } else if (u == 3) {
        nes_apu_update_noise(3, p & 3);
    }
}

s32 nes_cpu_read(s32 addr)
{
    addr &= 0xFFFF;
    if (addr < 0x2000) {
        addr &= 0x7FF;
    } else if (addr < 0x8000) {
        return nes_read_io_4(addr);
    }
    return *(u8 *)(addr + 0x80380000);
}

void nes_cpu_write(s32 addr, s32 val) {
    if (addr < 0x2000) {
        u8 *ram = (u8 *)0x80380000;
        ram[addr & 0x7FF] = val;
    } else if (addr < 0x8000) {
        nes_write_io(addr, val);
    }
}
