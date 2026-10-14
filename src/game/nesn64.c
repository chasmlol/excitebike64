#include "game.h"

static inline void wr(s32 a, s32 v)
{
    if (a < 0x2000) {
        NES->mem[a & 0x7FF] = v;
    } else if (a > 0x7FFF) {
    } else {
        nes_write_io(a, v);
    }
}

static inline void wr_2(s32 a, s32 v)
{
    if (a < 0x2000) {
        a &= 0x7FF;
        NES->mem[a] = v;
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
    return NES->mem[a];
}

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
    sNesMemory = NES;
    mem_fill(sNesMemory, 0, sizeof(Nes));
    src = res_load(obj, D_800D14D8);
    __builtin_memcpy(&sNesMemory->mem[0x8000], src, 0x4000);
    __builtin_memcpy(&sNesMemory->mem[0xC000], src, 0x4000);
    __builtin_memcpy(sNesMemory->vram, src + 0x4000, 0x2000);
    nes_cpu_reset(sNesMemory, nes_draw_bg_row);
    sNesSnapshot = mem_alloc_dbg(sizeof(NesSnapshot), 0, D_800DAF38, 0x9C);
    __builtin_memcpy(sNesSnapshot, src + 0x6000, sizeof(NesSnapshot));
    res_close_set(obj);
    sNesChrTiles = mem_alloc_dbg(0x8000, 0, D_800DAF38, 0xA4);
    m = (s32)NES;
    t = 0x10000; /* m + t: NES->vram, the CHR ROM */
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

void nes_update_input(volatile s32 *p)
{
    s32 w, v;
    s32 f;
    s32 k;
    s32 hi, lo;
    u8 b;
    NesSnapshot *m;
    Nes *nes;

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
            __builtin_memcpy(NES->mem, sNesSnapshot->ram, 0x800);
            __builtin_memcpy(&NES->vram[0x2000], sNesSnapshot->vram, 0x2000);
            m = sNesSnapshot;
            nes = NES;
            nes->pc = m->pc;
            nes->vramAddr = m->vramAddr;
            nes->a = m->a;
            nes->x = m->x;
            nes->y = m->y;
            nes->s = m->s;
            b = m->p;
            nes->p = b;
            nes->ppuLatch = m->ppuLatch;
            nes->scrollX = m->scrollX;
            nes->scrollY = m->scrollY;
            nes->oamAddr = m->oamAddr;
            nes->n = b & 0x80;
            nes->v = b & 0x40;
            nes->b = b & 0x10;
            nes->d = b & 0x8;
            nes->i = b & 0x4;
            nes->joypad = 0;
            nes->unk_14121[0] = 0;
            nes->joypadBit = 0;
            nes->unk_14128 = 0;
            nes->z = b & 0x2;
            nes->c = b & 0x1;
            for (k = 0; k < 4; k++) {
                NES->apu[k].rate = 0;
                NES->apu[k].unk_04 = 0;
            }
            __builtin_memcpy(&NES->mem[NES_TRACK_ADDR], gSaveNesTrack, NES_TRACK_SIZE);
        }
    }
    v = *p;
    lo = (v >> 24) & 0xF;
    hi = (v >> 28) & 0xF;
    if ((v & 1) && hi > 8) f |= 0x10;
    if ((*p & 2) && hi > 8) f |= 0x20;
    if ((*p & 4) && lo > 8) f |= 0x40;
    if ((*p & 8) && lo > 8) f |= 0x80;
    NES->joypad = f;
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
                        sNesCpuMemory->vram[k] = sNesExitMsg[i] - 0x37;
                        sNesCpuMemory->vram[k + 0x400] = sNesExitMsg[i] - 0x37;
                    }
                    i++;
                    k++;
                } while (sNesExitMsg[i] != 0);
            }
            mem_fill(&sNesCpuMemory->vram[0x23F0], 0x5F, 8);
        } else {
            t = 0;
            if (sNesExitMsg[0] != 0) {
                do {
                    t++;
                    sNesCpuMemory->vram[k] = 0xFC;
                    sNesCpuMemory->vram[k + 0x400] = 0xFC;
                    k++;
                } while (sNesExitMsg[t] != 0);
            }
        }
    }
    if (NES->pc == 0xC661 || NES->pc == 0xC66A) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = 0x3C;
            __builtin_memcpy(gSaveNesTrack, &NES->mem[NES_TRACK_ADDR], NES_TRACK_SIZE);
            save_write(0, 0);
        }
    } else if (NES->pc == 0xC641 || NES->pc == 0xC69A) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = 0x3C;
            __builtin_memcpy(&NES->mem[NES_TRACK_ADDR], gSaveNesTrack, NES_TRACK_SIZE);
        }
    }
    if (!(NES->mem[0x2001] & 8) || sNesFrameCount < 10) {
        gDPPipeSync(sNesGfx++);
        gDPSetCycleType(sNesGfx++, G_CYC_FILL);
        t = NES->mem[0x2001] >> 5;
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
            NES->apu[i].rate = 0;
            NES->apu[i].unk_04 = 0;
        }
    }
    *pgdl = sNesGfx;
    nes_audio_update();
}

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

    if ((NES->mem[0x2001] & 8) && sNesFrameCount >= 10) {
        sNesLastTileKey = -1;
        if (sNesBgTlutLoaded == 0) {
            k = 0x3F00;
            idx = 0;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    sNesBgTluts[gGfxFrameIndex][idx] = sNesPalette[NES->vram[k]];
                    k++;
                    if (j == 0) {
                        sNesBgTluts[gGfxFrameIndex][idx] &= 0xFFFC;
                    } else {
                        sNesBgTluts[gGfxFrameIndex][idx] |= 3;
                    }
                    idx++;
                }
                idx += 12;
            }
            sNesBgTlutLoaded = 1;
            gDPPipeSync(sNesGfx++);
            gDPLoadTLUT_pal256(sNesGfx++, sNesBgTluts[gGfxFrameIndex]);
        }
        base = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        if (NES->mem[0x2000] & 0x10) {
            base += 0x4000;
        }
        nt = (NES->mem[0x2000] & 3) << 10;
        nt += 0x2000;
        ntp = &NES->vram[nt];
        y = row << 5;
        sx = NES->scrollX >> 3;
        fine = NES->scrollX & 7;
        x = (0x1C - fine) << 2;
        nt += y;
        nt += sx;
        col = sx + 1;
        x += 0x20;
        nt++;

        for (i = col; i < 0x20; i++) {

            tile = NES->vram[nt];
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
        nt = ((NES->mem[0x2000] & 3) << 10) ^ 0x400;
        nt += 0x2000;
        ntp = &NES->vram[nt];
        col = 0;
        nt += row << 5;
        if (fine != 0) {
            sx++;
        }
        for (i = 0; i < sx; i++) {

            tile = NES->vram[nt];
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

void nes_draw_sprites(s32 arg0)
{
    s32 i, j, x, k, s, idx;
    u16 **pp;
    u8 *base;
    u16 *pal;
    NesSprite *spr;
    s32 t;
    s32 y;
    u16 c;
    Nes *nes = NES;
    u8 *mem;

    if (nes->mem[0x2001] & 0x10) {
        base = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        gDPSetRenderMode(sNesGfx++, 0x0C087008, 0);
        gDPSetCombine(sNesGfx++, 0xFFFFFF, 0xFFFCF279);
        gDPSetAlphaCompare(sNesGfx++, G_AC_THRESHOLD);
        if (nes->mem[0x2000] & 8) {
            base += 0x4000;
        }
        k = 0x3F10;
        idx = 0;
        i = 0;
        mem = NES->mem;
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
            spr = &NES->oam.sprites[i];
            if (spr->y < 0xF1 && !(spr->attr & 0x20)) {
                j = spr->tile * 64;
                gDPLoadTextureBlock_4b(sNesGfx++, base + j, G_IM_FMT_CI, 16, 8, spr->attr & 3,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
                x = spr->x;
                x = (x + 0x1C) * 4;
                y = spr->y + 1;
                y <<= 2;
                if (!(spr->attr & 0x40)) {
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
    Nes *nes;
    s32 bas;
    s32 i;
    s32 row, k, col;
    s32 t0;
    u8 *src;
    u16 *dst;
    NesSprite *e;
    u32 v;

    nes = NES;
    if (nes->mem[0x2001] & 0x10) {
        osInvalDCache(0, 0x400000);
        bas = sNesChrTiles;
        if (nes->mem[0x2000] & 8) {
            bas += 0x4000;
        }
        for (i = 63; i >= 0; i--) {
            e = &NES->oam.sprites[i];
            if (e->y < 0xF1 && (e->attr & 0x20)) {
                t0 = e->attr & 3;
                t0 <<= 4;
                row = e->tile << 6;
                src = (u8 *)(bas + row);
                col = e->x + 0x1c;
                dst = gFramebuffers_2[param] + ((e->y + 1) * 0x140 + col);
                for (row = 0; row < 8; row++) {
                    col = e->x + 0x1c;
                    if (e->attr & 0x40) {
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

void nes_apu_render(s16 *p)
{
    s32 n, i, j, v;
    NesApuChannel *c;
    s32 old;

    n = sNesAudioFrameSamples / sNesAudioChunks;
    if (sNesCpuMemory == 0) {
        return;
    }
    if (!(NES->apu[2].flags & NES_APU_ENABLED) || NES->apu[2].elapsed >= NES->apu[2].length || NES->apu[2].volume == 0) {
        mem_fill((u8 *)p, 0, n * 4);
    } else {
        for (j = 0; j < n * 2; j += 2) {
            NES->apu[2].phase += (u16)NES->apu[2].rate;
            v = NES->apu[2].phase;
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
        if ((NES->apu[i].flags & NES_APU_ENABLED) && NES->apu[i].elapsed < NES->apu[i].length && NES->apu[i].volume != 0) {
                for (j = 0; j < n * 2; j += 2) {
                NES->apu[i].phase += (u16)NES->apu[i].rate;
                if (NES->apu[i].duty < NES->apu[i].phase) {
                    v = 0x1000;
                } else {
                    v = -0x1000;
                }
                v = (v * NES->apu[i].volume) >> 16;
                p[j] += v;
                p[j + 1] += v;
            }
        }
    }
    if ((NES->apu[3].flags & NES_APU_ENABLED) && NES->apu[3].elapsed < NES->apu[3].length && NES->apu[3].volume != 0) {
        for (j = 0; j < n * 2; j += 2) {
            old = NES->apu[3].phase;
            NES->apu[3].phase += (u16)NES->apu[3].rate;
            if (NES->apu[3].phase < old) {
                v = ((s32)random_byte() - 0x80) << 4;
                v = v * NES->apu[3].volume >> 16;
            }
            p[j] += v;
            p[j + 1] += v;
        }
    }
    for (i = 0; i < 4; i++) {
        NES->apu[i].elapsed += NES->apu[i].lengthStep;
        NES->apu[i].volume -= NES->apu[i].decay;
        if (NES->apu[i].volume < 0) {
            if ((NES->apu[i].flags & NES_APU_LOOP) && i < 2) {
                NES->apu[i].volume = 0x10000;
            } else {
                NES->apu[i].volume = 0;
                NES->apu[i].decay = 0;
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
    u8 *p = sNesMemory->vram;
    if (p[0x2084] == 0x27 && p[0x2108] == 0x9A) {
        return 1;
    }
    return 0;
}

void nes_apu_reset_channels(void)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        NES->apu[i].rate = 0;
        NES->apu[i].unk_04 = 0;
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

void nes_apu_update_square(s32 idx, s32 mode)
{
    NesApuChannel *r;
    s32 per;
    s32 a0;
    s32 v;
    u8 *p;

    per = NES->mem[idx * 4 + 0x4002] | ((NES->mem[idx * 4 + 0x4003] & 7) << 8);
    r = &NES->apu[idx];
    if (per == 0) {
        per = 1;
    }
    per = 0x36EE8 / per;
    per = (per << 15) / 22050;
    r->rate = per;
    a0 = NES->mem[idx * 4 + 0x4000] >> 6;
    r->duty = sNesDutyTable[a0];
    a0 = NES->mem[idx * 4 + 0x4003] >> 3;
    if (a0 & 1) {
        a0 >>= 1;
        if (a0 == 0) {
            r->length = 0x7E;
        } else {
            r->length = a0;
        }
    } else {
        r->length = sNesLengthTable[a0 >> 1];
    }
    if (NES->mem[0x4000 + idx * 4] & 0x20) {
        r->lengthStep = 0;
        r->elapsed = 0;
    } else {
        r->lengthStep = 1;
    }
    if (mode == 3) {
        r->elapsed = 0;
    }
    if ((NES->mem[0x4015] >> idx) & 1) {
        r->flags |= NES_APU_ENABLED;
    } else {
        r->flags &= ~NES_APU_ENABLED;
    }
    if (mode == 0) {
        r->volume = 0xFFFF;
    }
    p = &NES->mem[0x4000 + idx * 4];
    a0 = *p & 0x10;
    v = *p & 0xF;
    if (a0) {
        r->volume = (v << 16) / 16;
        r->decay = 0;
        r->flags &= ~NES_APU_LOOP;
        return;
    }
    r->decay = 0x4000 / (v + 1);
    if (*p & 0x20) {
        r->flags |= NES_APU_LOOP;
        return;
    }
    r->flags &= ~NES_APU_LOOP;
}

void nes_apu_update_triangle(s32 n, s32 m)
{
    Nes *nes;
    NesApuChannel *c;
    s32 v;
    s32 bVar1;
    s32 k;
    u8 *p;

    nes = NES;
    k = n * 4;
    k += 0x4003;
    p = &nes->mem[k];
    v = nes->mem[n * 4 + 0x4002] | ((*p & 7) << 8);
    c = &NES->apu[n];
    if (v == 0) {
        v = 1;
    }
    v = 0x36EE8 / v;
    v = (v << 14) / 0x5622;
    c->rate = v;
    bVar1 = *p >> 3;
    if (bVar1 & 1) {
        bVar1 >>= 1;
        if (bVar1 == 0) {
            c->length = 0x7e;
        } else {
            c->length = bVar1;
        }
    } else {
        c->length = sNesLengthTable[bVar1 >> 1];
    }
    if ((NES->mem[0x4000 + n * 4] & 0x20) != 0) {
        c->lengthStep = 0;
        c->elapsed = 0;
    } else {
        c->lengthStep = 1;
    }
    if (m == 3) {
        c->elapsed = 0;
    }
    if (((NES->mem[0x4015] >> n) & 1) != 0) {
        c->flags = c->flags | NES_APU_ENABLED;
    } else {
        c->flags = c->flags & ~NES_APU_ENABLED;
    }
    c->volume = 0xffff;
    bVar1 = NES->mem[0x4000 + n * 4];
    if ((bVar1 & 0x80) != 0) {
        c->decay = 0;
        return;
    }
    c->decay = 0x40000 / ((bVar1 & 0x7f) + 1);
}

void nes_apu_update_noise(s32 n, s32 m)
{
    Nes *nes;
    NesApuChannel *c;
    s32 v;
    s32 b;
    u8 *p;
    u8 *q;
    s32 k;
    s32 t;
    s32 e, mm;
    s32 *tb;

    tb = sNesNoisePeriods;
    nes = NES;
    k = n * 4 + 0x4002;
    v = tb[nes->mem[k] & 0xF];
    v = (v * 0x28F) / 0xDC;
    p = &nes->mem[n * 4];
    c = &NES->apu[n];
    c->rate = v * 4;
    b = p[0x4003] >> 3;
    if (b & 1) {
        b >>= 1;
        if (b == 0) {
            c->length = 0x7e;
        } else {
            c->length = b;
        }
    } else {
        c->length = sNesLengthTable[b >> 1];
    }
    if ((NES->mem[0x4000 + n * 4] & 0x20) != 0) {
        c->lengthStep = 0;
        c->elapsed = 0;
    } else {
        c->lengthStep = 1;
    }
    if (m == 3) {
        c->elapsed = 0;
    }
    if (((NES->mem[0x4015] >> n) & 1) != 0) {
        c->flags = c->flags | NES_APU_ENABLED;
    } else {
        c->flags = c->flags & ~NES_APU_ENABLED;
    }
    if (m == 0) {
        c->volume = 0xffff;
    }
    k = n * 4;
    q = &NES->mem[0x4000 + k];
    e = *q;
    t = e & 0x10;
    mm = e & 0xF;
    if (t != 0) {
        c->volume = (mm << 16) / 16;
        c->decay = 0;
        c->flags = c->flags & ~NES_APU_LOOP;
    } else {
        c->decay = 0x4000 / (mm + 1);
        if ((*q & 0x20) != 0) {
            c->flags = c->flags | NES_APU_LOOP;
        } else {
            c->flags = c->flags & ~NES_APU_LOOP;
        }
    }
}

void nes_write_io(s32 addr, s32 value)
{
    s32 k;
    u8 *src;

    if (addr == 0x2007) {
        NES->vram[NES->vramAddr] = value;
        if (NES->mem[0x2000] & 4) {
            NES->vramAddr = (NES->vramAddr + 32) % 0x4000;
        } else {
            NES->vramAddr = (NES->vramAddr + 1) % 0x4000;
        }
    } else if (addr == 0x2006) {
        if (NES->ppuLatch & 1) {
            NES->vramAddr = (NES->vramAddr & 0xFF00) | value;
            NES->ppuLatch |= 2;
        } else {
            NES->vramAddr = (NES->vramAddr & 0xFF) | (value << 8);
        }
        NES->ppuLatch ^= 1;
    } else if (addr == 0x4016) {
        if (value & 1) {
            NES->joypadBit = 0x7B;
        } else if (NES->joypadBit == 0x7B) {
            NES->joypadBit = 0;
        }
    } else if (addr == 0x2005) {
        if (NES->ppuLatch & 4) {
            NES->scrollY = value;
        } else {
            NES->scrollX = value;
        }
        NES->ppuLatch ^= 4;
    } else if (addr == 0x2003) {
        NES->oamAddr = value;
    } else if (addr == 0x2004) {
        NES->oam.bytes[NES->oamAddr++] = value;
    } else if (addr == 0x4014) {
        src = &NES->mem[value << 8];
        __builtin_memcpy(NES->oam.bytes, src, sizeof(NES->oam));
    } else {
        k = addr - 0x4000;
        if ((u32)k < 0x16) {
            NES->mem[addr] = value;
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
    NES->mem[addr] = value;
}

u32 nes_read_io(s32 code, s32 unused, u32 val)
{
    u8 *ram;
    Nes *nes;
    s32 pos;
    s32 v;
    s32 n;
    s32 i;
    u32 mask;

    if (code == 0x2007) {
        nes = NES;
        __asm__ __volatile__("");
        code = nes->vramAddr;
        unused = nes->ppuLatch;
        val = nes->vram[code];
        if (unused & 2) {
            nes->ppuLatch = unused & ~2;
        } else {
            if (nes->mem[0x2000] & 4) {
                nes->vramAddr = (code + 0x20) % 0x4000;
            } else {
                nes->vramAddr = (code + 1) % 0x4000;
            }
        }
        return val;
    }
    if (code == 0x4016) {
        nes = NES;
        n = nes->joypadBit;
        if (n < 8) {
            nes->joypadBit = n + 1;
            val = (nes->joypad >> n) & 1;
        }
        return val;
    }
    if (code == 0x4015) {
        val = 0;
        for (i = 0; i < 4; i++) {
            if ((NES->apu[i].flags & NES_APU_ENABLED) && NES->apu[i].elapsed < sNesNoiseLength && NES->apu[i].volume != 0) {
                val |= 1 << i;
            }
        }
        return val & NES_AT(mem[0x4015]);
    }
    return NES->mem[code];
}

void nes_op_asl(void)
{
    s32 addr = sNesEffAddr;

    if (addr == -1) {
        NES->c = NES->a & 0x80;
        NES->a <<= 1;
        NES->z = NES->a == 0;
        NES->n = NES->a & 0x80;
    } else {
        u32 v = sNesOperand << 1;
        NES->c = ((u8 *)&sNesOperand)[3] & 0x80;
        sNesOperand = v;
        if (addr < 0x2000) {
            NES->mem[addr & 0x7FF] = v;
        } else if (addr < 0x8000) {
            nes_write_io(addr, v);
        }
        NES->z = (sNesOperand & 0xFF) == 0;
        NES->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_asr(void)
{
    if (sNesEffAddr == -1) {
        NES->c = NES->a & 1;
        NES->a >>= 1;
        if (NES->a & 0x40) {
            NES->a |= 0x80;
        }
        NES->z = NES->a == 0;
        NES->n = NES->a & 0x80;
    } else {
        NES->c = sNesOperand & 1;
        sNesOperand >>= 1;
        if (sNesOperand & 0x40) {
            sNesOperand |= 0x80;
        }
        wr(sNesEffAddr, sNesOperand);
        NES->z = (sNesOperand & 0xFF) == 0;
        NES->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_rol(void)
{
    s32 bit;

    if (sNesEffAddr == -1) {
        bit = NES->a & 0x80;
        NES->a <<= 1;
        if (NES->c) {
            NES->a |= 1;
        }
        NES->c = bit;
        NES->z = NES->a == 0;
        NES->n = NES->a & 0x80;
    } else {
        bit = sNesOperand & 0x80;
        sNesOperand <<= 1;
        if (NES->c) {
            do { sNesOperand |= 1; } while (0);
        }
        NES->c = bit;
        wr(sNesEffAddr, sNesOperand);
        NES->z = (sNesOperand & 0xFF) == 0;
        NES->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_ror(void)
{
    s32 bit;

    if (sNesEffAddr == -1) {
        bit = NES->a & 1;
        NES->a >>= 1;
        if (NES->c) {
            NES->a |= 0x80;
        }
        NES->c = bit;
        NES->z = NES->a == 0;
        NES->n = NES->a & 0x80;
    } else {
        bit = sNesOperand & 1;
        sNesOperand >>= 1;
        if (NES->c) {
            do { sNesOperand |= 0x80; } while (0);
        }
        NES->c = bit;
        wr(sNesEffAddr, sNesOperand);
        NES->z = (sNesOperand & 0xFF) == 0;
        NES->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_lsr(void)
{
    s32 a2 = sNesEffAddr;

    if (a2 == -1) {
        Nes *nes = NES;
        u8 b = nes->a;
        nes->c = b & 1;
        b = b >> 1;
        nes->a = b;
        nes->z = (b == 0);
        nes->n = b & 0x80;
        return;
    } else {
        u8 *pb = (u8 *)&sNesOperand;
        u8 lo = pb[3];
        s32 w;

        NES_AT(c) = lo & 1;
        w = sNesOperand >> 1;
        sNesOperand = w;
        if (a2 < 0x2000) {
            *(NES->mem + (a2 & 0x7FF)) = (u8)w;
        } else if (a2 <= 0x7FFF) {
            nes_write_io(a2, w);
        }
    }
    {
        Nes *t = NES;
        t->z = ((sNesOperand & 0xFF) == 0);
        t->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

void nes_op_brk(void)
{
    NES->pc++;
    NES->mem[0x100 + NES->s] = NES->pc >> 8;
    NES->s--;
    NES->mem[0x100 + NES->s] = NES->pc;
    NES->b = 1;
    NES->s--;
    NES->p = (NES->n ? 0x80 : 0) | (NES->v ? 0x40 : 0) | 0x20 | (NES->b ? 0x10 : 0) | (NES->d ? 8 : 0) | (NES->i ? 4 : 0) | (NES->z ? 2 : 0) | (NES->c ? 1 : 0);
    NES->mem[0x100 + NES->s] = NES->p;
    NES->i = 1;
    NES->s--;
    NES->pc = NES->mem[0xFFFE];
    NES->pc |= NES->mem[0xFFFF] << 8;
}

void nes_op_php(void)
{
    NES->p = (NES->n ? 0x80 : 0) | (NES->v ? 0x60 : 0x20)
        | (NES->b ? 0x10 : 0) | (NES->d ? 8 : 0) | (NES->i ? 4 : 0)
        | (NES->z ? 2 : 0) | (NES->c ? 1 : 0);
    NES->mem[0x100 + NES->s] = NES->p;
    NES->s--;
}

void nes_op_rti(void)
{
    u32 v;
    u32 bits;
    u32 base;

    base = (u32)&NES->mem[0x100];
    v = (u32)sNesRegS + 1;
    NES_AT(s) = v;
    v = v & 0xFF;
    bits = *(u8 *)(base | v);
    NES_AT(n) = bits & 0x80;
    NES_AT(v) = bits & 0x40;
    NES_AT(b) = bits & 0x10;
    NES_AT(d) = bits & 0x8;
    NES_AT(i) = bits & 0x4;
    NES_AT(z) = bits & 0x2;
    NES_AT(c) = bits & 0x1;
    nes_op_rts();
}

void nes_cpu_nmi(void)
{
    NES->mem[0x100 + NES->s] = NES->pc >> 8;
    NES->s--;
    NES->mem[0x100 + NES->s] = NES->pc;
    NES->s--;
    NES->p = (NES->n ? 0x80 : 0) | (NES->v ? 0x40 : 0) | 0x20 | (NES->b ? 0x10 : 0) | (NES->d ? 8 : 0) | (NES->i ? 4 : 0) | (NES->z ? 2 : 0) | (NES->c ? 1 : 0);
    NES->mem[0x100 + NES->s] = NES->p;
    NES->i = 1;
    NES->s--;
    NES->pc = NES->mem[0xFFFA];
    NES->pc |= NES->mem[0xFFFB] << 8;
}

/* The opcode being run (0x800F5BB4, after sNesFrameCount), reached from a base register. */
#define CUR (*(NesOpcode **)(glob + 0x5BB4))
#define R2000 (ram[0x2000])
#define R2002 (ram[0x2002])

void nes_run_frame(void)
{
    s32 i = 0;
    s32 line;
    u8 *glob = (u8 *)0x800F0000;
    u8 *ram = NES->mem;
    Nes *cpu = NES;

    do {
        line = (i + 0xf0) % 0xf8;
        if (NES_AT(cycles) < 0x78) {
            NesOpcode *tb = sNesOpcodeTable;
            u8 *dseg = (u8 *)0x800D0000;
            do {
                NesOpcode *e;
                CUR = (NesOpcode *)((s32)(*(u8 *)(cpu->pc + (s32)ram) * sizeof(NesOpcode)) + (s32)tb);
                CUR->addrMode();
                e = CUR;
                cpu->pc += e->length;
                e->op(e);
                (*(s32 *)(dseg - 0x13E4))++;
                cpu->cycles += CUR->cycles;
            } while (cpu->cycles < 0x78);
        }
        cpu->cycles -= 0x78;
        if ((line & 7) == 0) {
            if (line == 0) {
                R2002 = 0x10;
            } else if (line == 0xf0) {
                R2002 |= 0x80;
                if (R2000 & 0x80) nes_cpu_nmi();
            }
        }
        if ((line & 7) == 4 && line < 0xf0 && sNesRowCallback != 0) sNesRowCallback(line >> 3);
        if (line == NES_AT(oam.sprites[0].y) && line < 0xf0) {
            R2002 |= 0x40;
            if (R2000 & 0x40) nes_cpu_nmi();
        }
        i++;
    } while (i < 0xf8);
    {
        Nes *c2 = NES;
        (c2->frameCount)++;
    }
}

s32 nes_cpu_reset(Nes *machine, void (*rowCallback)(s32)) {
    Nes *nes = NES;
    u8 *ram = &NES->mem[0x8000];
    sNesCpuMemory = machine;
    sNesRowCallback = rowCallback;
    nes->v = 0;
    nes->b = 0;
    nes->d = 0;
    nes->i = 1;
    nes->z = 0;
    nes->c = 0;
    nes->pc = (u16)(ram[0x7FFC] | (ram[0x7FFD] << 8));
    return 0;
}

void nes_op_adc(void) {
    Nes *nes = NES;
    u32 sum = nes->a;
    u32 old = sum;
    u32 m = 0xFFFFFF80;
    sum = sum + sNesOperand;
    if (nes->c != 0) {
        sum = sum + 1;
    }
    nes->z = (sum & 0xFF) == 0;
    nes->n = sum & m;
    nes->c = (sum & 0xFFFFFF00) != 0;
    nes->a = sum;
    nes->v = (old ^ sum) & m;
}

void nes_op_and(void)
{
    Nes *nes;
    u8 v;
    u8 z;
    u8 w;

    nes = NES;
    v = nes->a & sNesOperand8;
    z = (v == 0);
    nes->a = v;
    w = v & 0x80;
    nes->z = z;
    nes->n = w;
}

void nes_op_bcc(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->c == 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bcs(void)
{
    Nes *nes;
    s32 a;
    s32 b;
    u32 addr;

    if (D_800D05D4 != 0) {
        a = osGetCount();
        addr = ((a << 2) & 0xFFFFF) + 0x80000000;
        b = osGetCount();
        *(s32 *)addr = b;
    }
    nes = NES;
    if (nes->c != 0) {
        nes->pc = sNesOperand;
        nes->cycles += 2;
    }
}

void nes_op_beq(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->z != 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bit(void) {
    Nes *nes = NES;
    nes->z = ((nes->a & sNesOperand) == 0);
    {
        u8 b = ((u8 *)&sNesOperand)[3];
        nes->n = b & 0x80;
        nes->v = b & 0x40;
    }
}

void nes_op_bmi(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->n != 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bne(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->z == 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bpl(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->n == 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bvc(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->v == 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_bvs(void)
{
    Nes *nes;
    u16 h;
    s32 c;

    nes = NES;
    if (nes->v != 0) {
        h = sNesOperand16;
        c = nes->cycles + 2;
        nes->pc = h;
        nes->cycles = c;
    }
}

void nes_op_clc(void)
{
    NES_AT(c) = 0;
}

void nes_op_cld(void)
{
    NES_AT(d) = 0;
}

void nes_op_cli(void)
{
    NES_AT(i) = 0;
}

void nes_op_clv(void)
{
    NES_AT(v) = 0;
}

void nes_op_cmp(void) {
    Nes *nes = NES;
    s32 a = nes->a;
    s32 b = sNesOperand;
    if (b < a) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (a == b) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

void nes_op_cpx(void) {
    Nes *nes = NES;
    s32 a = nes->x;
    s32 b = sNesOperand;
    if (b < a) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (a == b) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

void nes_op_cpy(void) {
    Nes *nes = NES;
    s32 a = nes->y;
    s32 b = sNesOperand;
    if (b < a) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (a == b) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

void nes_op_dec(void)
{
    Nes *nes;

    sNesOperand = sNesOperand - 1;
    wr_2(sNesEffAddr, sNesOperand);
    nes = NES;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = sNesOperand & 0x80;
}

void nes_op_dex(void)
{
    Nes *nes = NES;
    s32 v = nes->x + -1;
    u8 m;
    nes->x = v;
    m = (u8)v & 0x80;
    nes->z = ((u8)v == 0);
    nes->n = m;
}

void nes_op_dey(void)
{
    Nes *nes = NES;
    s32 v = nes->y + -1;
    u8 m;
    nes->y = v;
    m = (u8)v & 0x80;
    nes->z = ((u8)v == 0);
    nes->n = m;
}

void nes_op_eor(void)
{
    Nes *nes = NES;
    u8 v = nes->a ^ sNesOperand8;
    nes->a = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
}

void nes_op_inc(void) {
    Nes *nes;
    s32 c = sNesOperand + 1;

    sNesOperand = c;
    wr_2(sNesEffAddr, c);
    nes = NES;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = ((u8 *)&sNesOperand)[3] & 0x80;
}

void nes_op_inx(void)
{
    Nes *nes = NES;
    s32 v = nes->x + 1;
    u8 m;
    nes->x = v;
    m = (u8)v & 0x80;
    nes->z = ((u8)v == 0);
    nes->n = m;
}

void nes_op_iny(void)
{
    Nes *nes = NES;
    s32 v = nes->y + 1;
    u8 m;
    nes->y = v;
    m = (u8)v & 0x80;
    nes->z = ((u8)v == 0);
    nes->n = m;
}

void nes_op_jmp(void)
{
    NES_AT(pc) = sNesOperand16;
}

void nes_op_jsr(void) {
    Nes *nes = NES;
    u32 base = (u32)NES->mem;
    u8 v;
    u32 i;
    u32 j;

    i = nes->s;
    i += 0x100;
    *(u8 *)(base + i) = (u8)(nes->pc >> 8);
    v = nes->pc;
    nes->s = nes->s - 1;
    j = (u8)nes->s;
    j += 0x100;
    *(u8 *)(base + j) = v;
    nes->s = nes->s - 1;
    nes->pc = sNesOperand16;
}

void nes_op_lda(void)
{
    Nes *nes = NES;

    nes->a = sNesOperand;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = sNesOperand & 0x80;
}

void nes_op_ldx(void)
{
    Nes *nes = NES;
    s8 b = sNesOperand;
    nes->x = b;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = b & 0x80;
}

void nes_op_ldy(void)
{
    Nes *nes = NES;

    nes->y = sNesOperand;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = sNesOperand & 0x80;
}

void nes_op_nop(void)
{
}

void nes_op_ora(void)
{
    Nes *nes;
    u8 v;
    u8 z;
    u8 w;

    nes = NES;
    v = nes->a | sNesOperand8;
    z = (v == 0);
    nes->a = v;
    w = v & 0x80;
    nes->z = z;
    nes->n = w;
}

void nes_op_pha(void)
{
    Nes *nes;
    u32 a;

    nes = NES;
    a = nes->s | (u32)&NES->mem[0x100];
    *(u8 *)a = nes->a;
    nes->s = nes->s - 1;
}

void nes_op_pla(void)
{
    Nes *nes;
    u32 a;

    nes = NES;
    nes->s = nes->s + 1;
    a = (u8)nes->s | (u32)&NES->mem[0x100];
    nes->a = *(u8 *)a;
}

/* 6502 PLP: pull the status byte from the stack page and split it into the flag bytes. */
void nes_op_plp(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    u32 v;
    s32 a;

    nes->s++;
    a = nes->s + 0x100;
    v = ram[a];
    nes->n = v & 0x80;
    nes->v = v & 0x40;
    nes->b = v & 0x10;
    nes->d = v & 0x08;
    nes->i = v & 0x04;
    nes->z = v & 0x02;
    nes->c = v & 0x01;
}

/* 6502 RTS-style pull of a 16-bit word from the stack page. */
void nes_op_rts(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    u32 lo, hi;
    s32 a, b;

    nes->s++;
    a = nes->s + 0x100;
    lo = ram[a];
    nes->s++;
    b = nes->s + 0x100;
    hi = ram[b];
    nes->pc = lo | (hi << 8);
}

void nes_op_sbc(void)
{
    Nes *nes = NES;
    s32 r = nes->a;
    s32 a = r;
    s32 x;
    r = r - sNesOperand;
    if (nes->c == 0) {
        r = r - 1;
    }
    nes->c = ((u32)~r) >> 31;
    nes->z = ((r & 0xFF) == 0);
    nes->n = r & -0x80;
    x = a ^ r;
    nes->a = r;
    nes->v = x & -0x80;
}

void nes_op_sec(void)
{
    NES_AT(c) = 1;
}

void nes_op_sed(void)
{
    NES_AT(d) = 1;
    fatal_halt(1);
}

void nes_op_sei(void)
{
    NES_AT(i) = 1;
}

void nes_op_sta(void)
{
    Nes *nes = NES;
    s32 a = sNesEffAddr;
    u32 v = nes->a;
    if (a < 0x2000) {
        NES->mem[a & 0x7FF] = v;
    } else if (a < 0x8000) {
        nes_write_io(a, v);
    }
}

void nes_op_stx(void) {
    s32 a = sNesEffAddr;
    u32 v = NES_AT(x);
    if (a < 0x2000) {
        *(NES->mem + (a & 0x7FF)) = v;
    } else if (a < 0x8000) {
        nes_write_io(a, v);
    }
}

void nes_op_sty(void) {
    s32 addr = sNesEffAddr;
    s32 value = NES_AT(y);

    if (addr < 0x2000) {
        *(NES->mem + (addr & 0x7FF)) = value;
    } else if (addr < 0x8000) {
        nes_write_io(addr, value);
    }
}

void nes_op_tax(void)
{
    Nes *nes = NES;
    u8 v = nes->a;
    nes->x = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
}

void nes_op_tay(void)
{
    Nes *nes = NES;
    u8 v = nes->a;
    nes->y = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
}

void nes_op_tsx(void)
{
    Nes *nes = NES;
    u8 v = nes->s;
    nes->x = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
}

void nes_op_txa(void)
{
    Nes *nes = NES;
    u8 v = nes->x;
    nes->a = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
}

void nes_op_txs(void)
{
    Nes *nes = NES;
    nes->s = nes->x;
}

void nes_op_tya(void)
{
    Nes *nes = NES;
    u8 v = nes->y;
    nes->a = v;
    nes->z = (v == 0);
    nes->n = v & 0x80;
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
    sNesOperand = NES_AT(a);
}

void nes_am_absolute_stub(void)
{
}

void nes_am_absolute(void) {
    u8 *nes = NES->mem;
    s32 i = NES_AT(pc);
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
    r = NES->mem[a];
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
    return NES->mem[a];
}

void nes_am_zeropage(void)
{
    s32 a = NES->mem[NES_AT(pc) + 1];
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
    Nes *nes;
    u8 *ram;
    u32 idx;

    nes = NES;
    ram = NES->mem;
    sNesEffAddr = -1;
    idx = nes->pc;
    sNesOperand = ram[idx + 1];
}

void nes_am_absolute_x_stub(void)
{
}

void nes_am_absolute_x(void) {
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 i = nes->pc;
    u8 *p = ram + i;
    s32 s, r, a;

    s = (p[1] | (ram[(u16)(i + 2)] << 8)) + nes->x;
    sNesEffAddr = s;
    a = (u16)s;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = nes_read_io_2(a);
        goto store;
    }
    r = NES->mem[a];
store:
    sNesOperand = r;
}

void nes_am_absolute_y_stub(void)
{
}

void nes_am_absolute_y(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 i = nes->pc;
    u8 *p = ram + i;
    u32 r;
    s32 a;

    a = (p[1] | (ram[(u16)(i + 2)] << 8)) + nes->y;
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
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 i = nes->pc;
    u8 *p = ram + i;
    s32 a;
    u32 r;

    a = p[1] + nes->x;
    *out = a;
    *out = ram[a] | (ram[(u16)(a + 1)] << 8);
    r = rd(*out);
    sNesOperand = r;
}

void nes_am_indirect_indexed_stub(void)
{
}

void nes_am_indirect_indexed(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 i = nes->pc;
    u8 *p = ram + i;
    s32 a, b;
    u32 r;

    a = p[1];
    *out = a;
    b = (ram[a] | (ram[(u16)(a + 1)] << 8)) + nes->y;
    *out = b;
    r = rd((u16)b);
    sNesOperand = r;
}

void nes_am_zeropage_x_stub(void)
{
}

void nes_am_zeropage_x(void) {
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 a, r;

    sNesEffAddr = ram[nes->pc + 1] + nes->x;
    a = sNesEffAddr;
    if (a < 0x2000) {
        a &= 0x7FF;
    } else if (a > 0x7FFF) {
    } else {
        r = nes_read_io_2(a);
        goto store;
    }
    r = NES->mem[a];
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
    return NES->mem[a];
}

void nes_am_zeropage_y(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 a;
    a = ram[nes->pc + 1] + nes->y;
    sNesEffAddr = a;
    *(u32 *)&sNesOperand = rd2_2(a); /* an unsigned store: as s32 the inlined read schedules differently */
}

void nes_am_relative_stub(void)
{
}

void nes_am_relative(void)
{
    s32 *dst = &sNesOperand;
    u32 a = NES_AT(pc);
    s32 v = a + (s8)NES->mem[a + 1] + 2;
    *dst = v & 0xFFFF;
}

void nes_am_jump_absolute_stub(void)
{
}

void nes_am_jump_absolute(void)
{
    u32 *out = &sNesOperand;
    u8 *nes = NES->mem;
    s32 i = NES_AT(pc);
    u8 *p = nes + i;
    *out = p[1] | (nes[(u16)(i + 2)] << 8);
}

void nes_am_jump_indirect_stub(void)
{
}

void nes_am_jump_indirect(void)
{
    u32 *out = &sNesOperand;
    u8 *nes = NES->mem;
    s32 i = NES_AT(pc);
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
    u8 *base = NES->mem;
    u32 h = NES_AT(pc);
    u8 *p = (u8 *)((u32)base + h);
    *dst = (u32)p[1] | ((u32)base[(u16)(h + 2)] << 8);
}

void nes_ea_zeropage(void)
{
    u32 a = NES_AT(pc);
    sNesEffAddr = NES->mem[a + 1];
}

void nes_ea_absolute_x(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 i = nes->pc;
    u8 *p = ram + i;
    *out = (p[1] | (ram[(u16)(i + 2)] << 8)) + nes->x;
}

void nes_ea_absolute_y(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 i = nes->pc;
    u8 *p = ram + i;
    *out = (p[1] | (ram[(u16)(i + 2)] << 8)) + nes->y;
}

void nes_ea_indexed_indirect(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 i = nes->pc;
    u8 *p = ram + i;
    s32 a = p[1] + nes->x;
    *out = a;
    *out = ram[a] | (ram[(u16)(a + 1)] << 8);
}

static inline u32 rd_2(u32 addr) { return NES->mem[addr]; }

void nes_ea_indirect_indexed(void)
{
    Nes *nes = NES;
    u32 a = rd_2(nes->pc + 1);
    sNesEffAddr = a;
    sNesEffAddr = (rd_2(a) | (rd_2(a + 1) << 8)) + nes->y;
}

void nes_ea_zeropage_x(void)
{
    Nes *nes = NES;
    u32 a = nes->pc;
    sNesEffAddr = NES->mem[a + 1] + nes->x;
}

void nes_ea_zeropage_y(void)
{
    Nes *nes;
    u8 *ram;
    u32 idx;

    nes = NES;
    ram = NES->mem;
    idx = nes->pc;
    sNesEffAddr = ram[idx + 1] + nes->y;
}

void nes_apu_write(s32 p, u8 v)
{
    s32 u;

    NES->mem[p] = v;
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
    return NES->mem[addr];
}

void nes_cpu_write(s32 addr, s32 val) {
    if (addr < 0x2000) {
        u8 *ram = NES->mem;
        ram[addr & 0x7FF] = val;
    } else if (addr < 0x8000) {
        nes_write_io(addr, val);
    }
}
