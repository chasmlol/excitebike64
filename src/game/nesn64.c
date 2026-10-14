#include "game.h"

/* Bus accesses of the opcode handlers and addressing modes: RAM (mirrored), the registers at $2000-$7FFF, or the
   ROM (writes ignored). The variants are copies written per caller so that each one compiles as in the original;
   they are kept apart on purpose. */
static inline void bus_write(s32 addr, s32 value)
{
    if (addr < NES_IO_START) {
        NES->mem[addr & NES_RAM_MASK] = value;
    } else if (addr > 0x7FFF) {
    } else {
        nes_write_io(addr, value);
    }
}

static inline void bus_write_2(s32 addr, s32 value)
{
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
        NES->mem[addr] = value;
    } else if (addr > 0x7FFF) {
    } else {
        nes_write_io(addr, value);
    }
}

static inline u32 bus_read(s32 addr)
{
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr > 0x7FFF) {
    } else {
        return nes_read_io_2(addr);
    }
    return NES->mem[addr];
}

/* Starts the emulator (game state 3): loads Excitebike's ROM and its saved machine state, decodes the CHR ROM into
   CI4 tiles, converts the palette, and runs three frames before drawing starts. */
void nes_init(void)
{
    s32 *set;
    u8 *rom;
    s32 nesAddr, vramOfs, lo, hi;
    u32 r, g, b;
    s32 i, j, n, k;
    u32 planes, pixel;
    u16 *pal;

    sNesFrameCount = 0;
    if (res_open_set(sStrNesres, &set) != 0) {
        fatal_halt(1);
    }
    sNesMemory = NES;
    mem_fill(sNesMemory, 0, sizeof(Nes));
    rom = res_load(set, sNesRomResId);
    __builtin_memcpy(&sNesMemory->mem[NES_PRG_ROM], rom, NES_PRG_SIZE);
    __builtin_memcpy(&sNesMemory->mem[NES_PRG_ROM_MIRROR], rom, NES_PRG_SIZE);
    __builtin_memcpy(sNesMemory->vram, rom + NES_PRG_SIZE, NES_CHR_SIZE);
    nes_cpu_reset(sNesMemory, nes_draw_bg_row);
    sNesSnapshot = mem_alloc_dbg(sizeof(NesSnapshot), 0, sStrNesn64C, 0x9C);
    __builtin_memcpy(sNesSnapshot, rom + 0x6000, sizeof(NesSnapshot)); /* after the PRG and CHR ROM */
    res_close_set(set);
    /* Each 16-byte CHR tile (two bit planes of 8 rows) becomes a 16x8 CI4 tile of 64 bytes, the right half unused. */
    sNesChrTiles = mem_alloc_dbg(0x8000, 0, sStrNesn64C, 0xA4);
    nesAddr = (s32)NES;
    vramOfs = 0x10000; /* nesAddr + vramOfs: NES->vram, the CHR ROM */
    for (i = 0; i < NES_CHR_SIZE; i += 0x10) {
        k = i * 4;
        for (j = 0; j < 8; j++) {
            lo = i + j;
            hi = lo + 8;
            pixel = *(u8 *)(hi + nesAddr + vramOfs);
            planes = *(u8 *)(lo + nesAddr + vramOfs);
            planes |= pixel << 8;
            for (n = 0; n < 8; n += 2) {
                pixel = (planes >> 7) & 1;
                if (planes & 0x8000) {
                    pixel |= 2;
                }
                planes <<= 1;
                sNesChrTiles[k] = pixel << 4;
                pixel = (planes >> 7) & 1;
                if (planes & 0x8000) {
                    pixel |= 2;
                }
                planes <<= 1;
                sNesChrTiles[k] |= pixel;
                k++;
            }
            k += 4;
        }
    }
    /* The RGB palette to RGBA5551 (j, n, k: red, green, blue), opaque, except one colour. */
    sNesPalette = mem_alloc_dbg(0x200, 0, sStrNesn64C, 0xBF);
    pal = sNesPalette;
    for (i = 0; i < 0x100; i++) {
        j = sNesRgbPalette[i * 3] >> 3;
        j <<= 11;
        n = sNesRgbPalette[i * 3 + 1] >> 3;
        n <<= 6;
        k = sNesRgbPalette[i * 3 + 2] >> 3;
        k <<= 1;
        pal[i] = j | n | k | 1;
        if (pal[i] == 0x8685) {
            pal[i] &= 0xFFFE;
        }
    }
    for (i = 0; i < 2; i++) {
        sNesBgTluts[i] = mem_alloc_dbg(0x200, 0, sStrNesn64C, 0xCC);
        sNesSpriteTluts[i] = mem_alloc_dbg(0x200, 0, sStrNesn64C, 0xCD);
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

/* The state's update: maps the game input word to the NES joypad (Z is B too, L is Select, the stick works as the
   D-pad), restores the snapshot when the restore timer runs out, and leaves on L+Z. */
void nes_update_input(volatile s32 *input)
{
    s32 buttons, held;
    s32 pad;
    s32 i;
    s32 stickY, stickX;
    u8 status;
    NesSnapshot *snap;
    Nes *nes;

    buttons = *input;
    pad = ((buttons & INPUT_D_UP) != 0) << 4; /* NES_PAD_UP */
    if (buttons & INPUT_D_DOWN) pad |= NES_PAD_DOWN;
    if (buttons & INPUT_D_LEFT) pad |= NES_PAD_LEFT;
    if (buttons & INPUT_D_RIGHT) pad |= NES_PAD_RIGHT;
    if (buttons & INPUT_A) pad |= NES_PAD_A;
    if (buttons & INPUT_Z) pad |= NES_PAD_B;
    if (buttons & INPUT_B) pad |= NES_PAD_B;
    if (buttons & INPUT_L) pad |= NES_PAD_SELECT;
    if (buttons & INPUT_START) pad |= NES_PAD_START;
    if (sNesRestoreTimer != 0) {
        if (--sNesRestoreTimer == 0) {
            /* Back to the machine state of the resource, with the design track put back from the save block. */
            __builtin_memcpy(NES->mem, sNesSnapshot->ram, 0x800);
            __builtin_memcpy(&NES->vram[NES_VRAM_NAMETABLE], sNesSnapshot->vram, 0x2000);
            snap = sNesSnapshot;
            nes = NES;
            nes->pc = snap->pc;
            nes->vramAddr = snap->vramAddr;
            nes->a = snap->a;
            nes->x = snap->x;
            nes->y = snap->y;
            nes->s = snap->s;
            status = snap->p;
            nes->p = status;
            nes->ppuLatch = snap->ppuLatch;
            nes->scrollX = snap->scrollX;
            nes->scrollY = snap->scrollY;
            nes->oamAddr = snap->oamAddr;
            nes->n = status & NES_FLAG_N;
            nes->v = status & NES_FLAG_V;
            nes->b = status & NES_FLAG_B;
            nes->d = status & NES_FLAG_D;
            nes->i = status & NES_FLAG_I;
            nes->joypad = 0;
            nes->unk_14121[0] = 0;
            nes->joypadBit = 0;
            nes->unk_14128 = 0;
            nes->z = status & NES_FLAG_Z;
            nes->c = status & NES_FLAG_C;
            for (i = 0; i < 4; i++) {
                NES->apu[i].rate = 0;
                NES->apu[i].unk_04 = 0;
            }
            __builtin_memcpy(&NES->mem[NES_TRACK_ADDR], gSaveData.nesTrack, NES_TRACK_SIZE);
        }
    }
    /* The stick counts as the D-pad past half way. */
    held = *input;
    stickX = (held >> INPUT_STICK_X_SHIFT) & 0xF;
    stickY = (held >> INPUT_STICK_Y_SHIFT) & 0xF;
    if ((held & INPUT_STICK_UP) && stickY > 8) pad |= NES_PAD_UP;
    if ((*input & INPUT_STICK_DOWN) && stickY > 8) pad |= NES_PAD_DOWN;
    if ((*input & INPUT_STICK_LEFT) && stickX > 8) pad |= NES_PAD_LEFT;
    if ((*input & INPUT_STICK_RIGHT) && stickX > 8) pad |= NES_PAD_RIGHT;
    NES->joypad = pad;
    if ((*input & (INPUT_L | INPUT_Z)) == (INPUT_L | INPUT_Z)) {
        gRaceExitCode = 2;
        gCurGameState->mode = 3; /* exit requested */
    }
}

/* Starts the frame's display list: RDP and RSP init, a viewport and scissor of the whole screen, the framebuffer
   `fb` (cleared while sNesClearFrames counts down), and the background's texture state with the NES palette. */
Gfx *nes_draw_setup(s32 gfxCtx, s32 fb, Gfx *gdl)
{
    mtx_from_angles_pos_scale(gfxCtx + 0x400, 0, 0, 0, 0, 0, 0, 0x100); /* the context's matrix */
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, gRdpInitDL);
    gSPDisplayList(gdl++, gRspInitDL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    sNesViewport.vp.vscale[0] = gScreenWidth * 2;
    sNesViewport.vp.vscale[1] = gScreenHeight * 2;
    sNesViewport.vp.vtrans[0] = gScreenWidth * 2;
    sNesViewport.vp.vtrans[1] = gScreenHeight * 2;
    gSPViewport(gdl++, &sNesViewport);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
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
        gDPSetRenderMode(gdl++, 0x0C084000, 0);          /* G_RM_OPA_SURF */
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);     /* G_CC_DECALRGB, G_CC_DECALRGB */
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_POINT);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, sNesPalette);
        return gdl;
    }
    return gdl;
}

/* The state's draw: runs one emulated frame (the background rows are drawn as it goes), draws the sprites, blinks
   the exit message, catches design-mode SAVE and LOAD, fills the picture with the border colour while the
   background is off, and feeds the audio. */
void nes_draw_frame(s32 gfxCtx, s32 fb, Gfx **pgdl)
{
    u8 *p;
    s32 ntAddr;
    s32 i;
    s32 t;

    sNesGfx = *pgdl;
    sNesGfx = nes_draw_setup(gfxCtx, fb, sNesGfx);
    if (sNesClearFrames != 0) {
        *pgdl = sNesGfx;
        return;
    }
    {
        /* gDPSetScissor(G_SC_NON_INTERLACE, 36, 0, 284, 240): the NES picture without its leftmost 8 pixels */
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
    /* Twice a second the message is written into both name tables, then blanked again. */
    if (nes_should_show_exit_msg() != 0) {
        ntAddr = NES_EXIT_MSG_ADDR;
        if ((sNesFrameCount / 30) & 1) {
            if (sNesExitMsg[0] != 0) {
                i = 0;
                do {
                    if (sNesExitMsg[i] != ' ') {
                        sNesCpuMemory->vram[ntAddr] = sNesExitMsg[i] - NES_TILE_FROM_ASCII;
                        sNesCpuMemory->vram[ntAddr + NES_NAMETABLE_SIZE] = sNesExitMsg[i] - NES_TILE_FROM_ASCII;
                    }
                    i++;
                    ntAddr++;
                } while (sNesExitMsg[i] != 0);
            }
            mem_fill(&sNesCpuMemory->vram[0x23F0], 0x5F, 8); /* the palettes of the bottom rows */
        } else {
            t = 0;
            if (sNesExitMsg[0] != 0) {
                do {
                    t++;
                    sNesCpuMemory->vram[ntAddr] = NES_TILE_BLANK;
                    sNesCpuMemory->vram[ntAddr + NES_NAMETABLE_SIZE] = NES_TILE_BLANK;
                    ntAddr++;
                } while (sNesExitMsg[t] != 0);
            }
        }
    }
    /* Design mode's SAVE writes the track to the save block, LOAD reads it back; the snapshot is restored later. */
    if (NES->pc == NES_PC_SAVE_1 || NES->pc == NES_PC_SAVE_2) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = NES_RESTORE_DELAY;
            __builtin_memcpy(gSaveData.nesTrack, &NES->mem[NES_TRACK_ADDR], NES_TRACK_SIZE);
            save_write(0, 0);
        }
    } else if (NES->pc == NES_PC_LOAD_1 || NES->pc == NES_PC_LOAD_2) {
        if (sNesRestoreTimer == 0) {
            sNesRestoreTimer = NES_RESTORE_DELAY;
            __builtin_memcpy(&NES->mem[NES_TRACK_ADDR], gSaveData.nesTrack, NES_TRACK_SIZE);
        }
    }
    if (!(NES->mem[NES_PPUMASK] & NES_PPUMASK_BG) || sNesFrameCount < NES_BLANK_FRAMES) {
        gDPPipeSync(sNesGfx++);
        gDPSetCycleType(sNesGfx++, G_CYC_FILL);
        /* PPUMASK's top 3 bits index a table of 4 colours: values 4-7 read the words after it */
        t = NES->mem[NES_PPUMASK] >> NES_PPUMASK_EMPHASIS_SHIFT;
        gDPSetFillColor(sNesGfx++, sNesBorderColors[t]);
        {
            /* gDPFillRectangle(36, 0, 284, 240) */
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

/* sNesRowCallback, called in the middle of each row of 8 lines: loads the background TLUT once a frame, then draws
   the tile row `row` of the visible name table and, past the X scroll, of the next one, a texture rectangle per tile
   (a texture load only when the tile or palette changes). */
void nes_draw_bg_row(s32 row)
{
    s32 i, j, palAddr, tlutIndex;
    u16 **tluts;
    u8 *tiles;
    u8 *nameTable;
    s32 ntAddr;
    s32 top;
    s32 coarseX, fineX;
    s32 col, x;
    s32 pal, key, tileOfs, tile, attrIndex, attrShift;

    if ((NES->mem[NES_PPUMASK] & NES_PPUMASK_BG) && sNesFrameCount >= NES_BLANK_FRAMES) {
        sNesLastTileKey = -1;
        if (sNesBgTlutLoaded == 0) {
            /* 4 palettes of 4 colours, 16 entries apart. Colour 0 gets alpha and bit 1 clear, the others both set:
               bit 1 marks opaque background pixels for nes_draw_back_sprites. */
            palAddr = NES_VRAM_BG_PALETTE;
            tlutIndex = 0;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    sNesBgTluts[gGfxFrameIndex][tlutIndex] = sNesPalette[NES->vram[palAddr]];
                    palAddr++;
                    if (j == 0) {
                        sNesBgTluts[gGfxFrameIndex][tlutIndex] &= 0xFFFC;
                    } else {
                        sNesBgTluts[gGfxFrameIndex][tlutIndex] |= 3;
                    }
                    tlutIndex++;
                }
                tlutIndex += 12;
            }
            sNesBgTlutLoaded = 1;
            gDPPipeSync(sNesGfx++);
            gDPLoadTLUT_pal256(sNesGfx++, sNesBgTluts[gGfxFrameIndex]);
        }
        tiles = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        if (NES->mem[NES_PPUCTRL] & NES_PPUCTRL_BG_TABLE) {
            tiles += NES_TILES_TABLE_SIZE;
        }
        ntAddr = (NES->mem[NES_PPUCTRL] & NES_PPUCTRL_NAMETABLE) << 10;
        ntAddr += NES_VRAM_NAMETABLE;
        nameTable = &NES->vram[ntAddr];
        /* Screen coordinates in 10.2 fixed point; the first tile column (coarseX) is skipped. */
        top = row << 5;
        coarseX = NES->scrollX >> 3;
        fineX = NES->scrollX & 7;
        x = (NES_SCREEN_X - fineX) << 2;
        ntAddr += top;
        ntAddr += coarseX;
        col = coarseX + 1;
        x += 0x20;
        ntAddr++;

        for (i = col; i < 0x20; i++) {

            tile = NES->vram[ntAddr];
            tileOfs = tile << 6;
            attrIndex = (col >> 2) + ((row >> 2) << 3);
            attrShift = ((col >> 1) & 1) | (row & 2);
            attrShift <<= 1;
            pal = (nameTable[attrIndex + NES_ATTR_TABLE] >> attrShift) & 3;
            key = tile | (pal << 8);
            if (key != sNesLastTileKey) {
                gDPLoadTextureBlock_4b(sNesGfx++, tiles + tileOfs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_WRAP, G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            sNesLastTileKey = key;
            gSPTextureRectangle(sNesGfx++, x, top, x + 32, top + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            ntAddr++;
            col++;
        }
        /* The rest of the row from the horizontally adjacent name table. */
        ntAddr = ((NES->mem[NES_PPUCTRL] & NES_PPUCTRL_NAMETABLE) << 10) ^ NES_NAMETABLE_SIZE;
        ntAddr += NES_VRAM_NAMETABLE;
        nameTable = &NES->vram[ntAddr];
        col = 0;
        ntAddr += row << 5;
        if (fineX != 0) {
            coarseX++;
        }
        for (i = 0; i < coarseX; i++) {

            tile = NES->vram[ntAddr];
            tileOfs = tile << 6;
            attrIndex = (col >> 2) + ((row >> 2) << 3);
            attrShift = ((col >> 1) & 1) | (row & 2);
            attrShift <<= 1;
            pal = (nameTable[attrIndex + NES_ATTR_TABLE] >> attrShift) & 3;
            key = tile | (pal << 8);
            if (key != sNesLastTileKey) {
                gDPLoadTextureBlock_4b(sNesGfx++, tiles + tileOfs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            sNesLastTileKey = key;
            gSPTextureRectangle(sNesGfx++, x, top, x + 32, top + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            ntAddr++;
            col++;
        }
    }
}

/* Loads the sprite TLUT and draws the sprites in front of the background with the RDP, sprite 0 last (on top). */
void nes_draw_sprites(s32 instrPerFrame)
{
    s32 i, j, x, palAddr, s, tlutIndex;
    u16 **tluts;
    u8 *tiles;
    u16 *palette;
    NesSprite *spr;
    s32 vramOfs;
    s32 y;
    u16 c;
    Nes *nes = NES;
    u8 *mem;

    if (nes->mem[NES_PPUMASK] & NES_PPUMASK_SPRITES) {
        tiles = sNesChrTiles;
        gDPPipeSync(sNesGfx++);
        gDPSetRenderMode(sNesGfx++, 0x0C087008, 0);         /* G_RM_TEX_EDGE */
        gDPSetCombine(sNesGfx++, 0xFFFFFF, 0xFFFCF279);    /* G_CC_DECALRGBA, G_CC_DECALRGBA */
        gDPSetAlphaCompare(sNesGfx++, G_AC_THRESHOLD);
        if (nes->mem[NES_PPUCTRL] & NES_PPUCTRL_SPR_TABLE) {
            tiles += NES_TILES_TABLE_SIZE;
        }
        /* 4 palettes of 4 colours, 16 entries apart: colour 0 transparent, bit 1 set (as opaque background) */
        palAddr = NES_VRAM_SPR_PALETTE;
        tlutIndex = 0;
        i = 0;
        mem = NES->mem;
        vramOfs = 0x10000; /* mem + vramOfs: NES->vram */
        palette = sNesPalette;
        tluts = sNesSpriteTluts;
    outer:
        for (j = 0; j < 4; j++) {
            x = palette[*(u8 *)(palAddr + (s32)mem + vramOfs)] | 2;
            tluts[gGfxFrameIndex][tlutIndex] = x;
            palAddr++;
            if (j == 0) {
                tluts[gGfxFrameIndex][tlutIndex] &= 0xFFFE;
            }
            tlutIndex++;
        }
        tlutIndex += 12;
        if (++i < 4) goto outer;
        gDPPipeSync(sNesGfx++);
        gDPLoadTLUT_pal256(sNesGfx++, sNesSpriteTluts[gGfxFrameIndex]);
        gDPPipeSync(sNesGfx++);
        for (i = 63; i >= 0; i--) {
            spr = &NES->oam.sprites[i];
            if (spr->y < NES_SPR_Y_OFFSCREEN && !(spr->attr & NES_SPR_BEHIND)) {
                j = spr->tile * 64;
                gDPLoadTextureBlock_4b(sNesGfx++, tiles + j, G_IM_FMT_CI, 16, 8, spr->attr & NES_SPR_PALETTE,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
                x = spr->x;
                x = (x + NES_SCREEN_X) * 4;
                y = spr->y + 1;
                y <<= 2;
                if (!(spr->attr & NES_SPR_FLIP_X)) {
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

/* Called from the graphics-done handler (main.c) for framebuffer `fb`: draws the sprites behind the background into
   it with the CPU, only over background pixels of colour 0 (bit 1 clear) and inside the visible picture. */
void nes_draw_back_sprites(s32 fb)
{
    Nes *nes;
    s32 tiles;
    s32 i;
    s32 row, k, col;
    s32 palBase;
    u8 *src;
    u16 *dst;
    NesSprite *spr;
    u32 pixel;

    nes = NES;
    if (nes->mem[NES_PPUMASK] & NES_PPUMASK_SPRITES) {
        osInvalDCache(0, 0x400000);
        tiles = sNesChrTiles;
        if (nes->mem[NES_PPUCTRL] & NES_PPUCTRL_SPR_TABLE) {
            tiles += NES_TILES_TABLE_SIZE;
        }
        for (i = 63; i >= 0; i--) {
            spr = &NES->oam.sprites[i];
            if (spr->y < NES_SPR_Y_OFFSCREEN && (spr->attr & NES_SPR_BEHIND)) {
                palBase = spr->attr & NES_SPR_PALETTE;
                palBase <<= 4;
                row = spr->tile << 6;
                src = (u8 *)(tiles + row);
                col = spr->x + NES_SCREEN_X;
                /* 320-pixel framebuffer rows; columns 36-283 are visible (the scissor of nes_draw_frame) */
                dst = gFramebuffers_2[fb] + ((spr->y + 1) * 0x140 + col);
                for (row = 0; row < 8; row++) {
                    col = spr->x + NES_SCREEN_X;
                    if (spr->attr & NES_SPR_FLIP_X) {
                        src += 3;
                        for (k = 0; k < 4; k++) {
                            pixel = *src & 0xF;
                            if (pixel != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[fb][pixel + palBase];
                            }
                            dst++;
                            col++;
                            pixel = *src >> 4;
                            if (pixel != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[fb][pixel + palBase];
                            }
                            dst++;
                            col++;
                            src--;
                        }
                        src += 9;
                    } else {
                        for (k = 0; k < 4; k++) {
                            pixel = *src >> 4;
                            if (pixel != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[fb][pixel + palBase];
                            }
                            dst++;
                            col++;
                            pixel = *src & 0xF;
                            if (pixel != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = sNesSpriteTluts[fb][pixel + palBase];
                            }
                            dst++;
                            col++;
                            src++;
                        }
                        src += 4;
                    }
                    dst += 0x138; /* the next row: 320 - 8 */
                }
            }
        }
        osWritebackDCache(gFramebuffers_2[fb], 0x25800); /* 320 x 240 x 2 */
    }
}

/* Sets the audio rate and allocates two output buffers, each a frame of stereo s16 samples. */
void nes_audio_init(void)
{
    s32 i;
    void *buf;

    sNesAudioFreq = osAiSetFrequency(NES_AUDIO_RATE);
    sNesAudioFrameSamples = ((sNesAudioChunks * sNesAudioFreq) / 60 + 7) & -8;
    for (i = 0; i < 2; i++) {
        buf = mem_alloc_dbg(sNesAudioFrameSamples << 2, 0, sStrNesn64C, 0x396);
        sNesAudioBufs[i] = (s32)buf;
        mem_fill(buf, 0, sNesAudioFrameSamples << 2);
    }
    sNesAudioBufIndex = 1;
}

/* Synthesizes one audio chunk into `out` (interleaved stereo): the triangle (or silence), plus the two squares and
   the noise; then advances each channel's length count and envelope. */
void nes_apu_render(s16 *out)
{
    s32 count, i, j, sample;
    NesApuChannel *ch;
    s32 oldPhase;

    count = sNesAudioFrameSamples / sNesAudioChunks;
    if (sNesCpuMemory == 0) {
        return;
    }
    if (!(NES->apu[NES_APU_TRIANGLE].flags & NES_APU_ENABLED)
        || NES->apu[NES_APU_TRIANGLE].elapsed >= NES->apu[NES_APU_TRIANGLE].length
        || NES->apu[NES_APU_TRIANGLE].volume == 0) {
        mem_fill((u8 *)out, 0, count * 4);
    } else {
        for (j = 0; j < count * 2; j += 2) {
            NES->apu[NES_APU_TRIANGLE].phase += (u16)NES->apu[NES_APU_TRIANGLE].rate;
            sample = NES->apu[NES_APU_TRIANGLE].phase;
            sample -= 0x4000;
            if (sample > 0x4000) {
                sample = 0x8000 - sample;
            }
            sample >>= 1;
            out[j] = sample;
            out[j + 1] = sample;
        }
    }
    /* the squares */
    for (i = 0; i < 2; i++) {
        if ((NES->apu[i].flags & NES_APU_ENABLED) && NES->apu[i].elapsed < NES->apu[i].length && NES->apu[i].volume != 0) {
            for (j = 0; j < count * 2; j += 2) {
                NES->apu[i].phase += (u16)NES->apu[i].rate;
                if (NES->apu[i].duty < NES->apu[i].phase) {
                    sample = 0x1000;
                } else {
                    sample = -0x1000;
                }
                sample = (sample * NES->apu[i].volume) >> 16;
                out[j] += sample;
                out[j + 1] += sample;
            }
        }
    }
    /* The noise: a new random level each time the phase wraps, held in between (at the start of the chunk, `sample`
       still holds whatever the code above left in it). */
    if ((NES->apu[NES_APU_NOISE].flags & NES_APU_ENABLED)
        && NES->apu[NES_APU_NOISE].elapsed < NES->apu[NES_APU_NOISE].length
        && NES->apu[NES_APU_NOISE].volume != 0) {
        for (j = 0; j < count * 2; j += 2) {
            oldPhase = NES->apu[NES_APU_NOISE].phase;
            NES->apu[NES_APU_NOISE].phase += (u16)NES->apu[NES_APU_NOISE].rate;
            if (NES->apu[NES_APU_NOISE].phase < oldPhase) {
                sample = ((s32)random_byte() - 0x80) << 4;
                sample = sample * NES->apu[NES_APU_NOISE].volume >> 16;
            }
            out[j] += sample;
            out[j + 1] += sample;
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

/* Once a frame: when the AI can take a buffer, renders a frame of audio into the next one and queues it. */
void nes_audio_update(void)
{
    s32 i;
    s16 *out;

    osInvalDCache(0, 0x400000);
    if (osAiGetStatus_2() >= 0) {
        out = (s16 *)sNesAudioBufs[sNesAudioBufIndex];
        for (i = 0; i < sNesAudioChunks; i++) {
            nes_apu_render(out);
            out += (sNesAudioFrameSamples / sNesAudioChunks) * 2;
        }
        osWritebackDCache((s16 *)sNesAudioBufs[sNesAudioBufIndex], sNesAudioFrameSamples << 2);
        osAiSetNextBuffer((s16 *)sNesAudioBufs[sNesAudioBufIndex], sNesAudioFrameSamples << 2);
        sNesAudioBufIndex = sNesAudioBufIndex ^ 1;
    }
}

/* The state's exit: frees the emulator's buffers and gives the sound back to the game. */
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
    u32 *bufs = &sNesAudioBufs;
    mem_free(bufs[0]);
    mem_free(bufs[1]);
}

/* Whether the exit message goes on this screen: two tiles of the first name table identify it. */
s32 nes_should_show_exit_msg(void)
{
    u8 *vram = sNesMemory->vram;
    if (vram[0x2084] == 0x27 && vram[0x2108] == 0x9A) {
        return 1;
    }
    return 0;
}

/* Silences the four channels. */
void nes_apu_reset_channels(void)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        NES->apu[i].rate = 0;
        NES->apu[i].unk_04 = 0;
    }
}

/* Fills a save block's design track (gSaveData.nesTrack) with Excitebike's empty track. */
void nes_init_saved_track(u8 *track)
{
    mem_fill(track, 0x40, NES_TRACK_SIZE);
    track[0] = 1;
    track[0xCF] = 9;
    mem_fill(track + 0xD0, 0, 0x30);
    track[0x100] = 1;
    track[0x1CF] = 9;
    mem_fill(track + 0x1D0, 0, 0x50);
    *(s16 *)(track + 0x200) = 0x5A10;
    track[0x204] = 0x2D;
    *(s16 *)(track + 0x20C) = -0x73FF;
    track[0x209] = 1;
    track[0x21B] = 1;
}

/* After a write to register `reg` (0-3) of square channel `idx`: updates its rate, duty, length, enable bit and
   envelope. A write to register 0 restarts the volume, one to register 3 the length count. */
void nes_apu_update_square(s32 idx, s32 reg)
{
    NesApuChannel *ch;
    s32 period;
    s32 bits;
    s32 volume;
    u8 *ctrl;

    period = NES->mem[idx * 4 + NES_APU_TIMER_LO] | ((NES->mem[idx * 4 + NES_APU_TIMER_HI] & 7) << 8);
    ch = &NES->apu[idx];
    if (period == 0) {
        period = 1;
    }
    period = NES_APU_CLOCK / period;
    period = (period << 15) / NES_AUDIO_RATE;
    ch->rate = period;
    bits = NES->mem[idx * 4 + NES_APU_CTRL] >> NES_APU_DUTY_SHIFT;
    ch->duty = sNesDutyTable[bits];
    /* the length: with bit 3 set, bits 4-7 (0 meaning 0x7E); otherwise from sNesLengthTable */
    bits = NES->mem[idx * 4 + NES_APU_TIMER_HI] >> 3;
    if (bits & 1) {
        bits >>= 1;
        if (bits == 0) {
            ch->length = 0x7E;
        } else {
            ch->length = bits;
        }
    } else {
        ch->length = sNesLengthTable[bits >> 1];
    }
    if (NES->mem[NES_APU_CTRL + idx * 4] & NES_APU_LENGTH_HALT) {
        ch->lengthStep = 0;
        ch->elapsed = 0;
    } else {
        ch->lengthStep = 1;
    }
    if (reg == 3) {
        ch->elapsed = 0;
    }
    if ((NES->mem[NES_APU_STATUS] >> idx) & 1) {
        ch->flags |= NES_APU_ENABLED;
    } else {
        ch->flags &= ~NES_APU_ENABLED;
    }
    if (reg == 0) {
        ch->volume = 0xFFFF;
    }
    ctrl = &NES->mem[NES_APU_CTRL + idx * 4];
    bits = *ctrl & NES_APU_CONST_VOLUME;
    volume = *ctrl & NES_APU_VOLUME;
    if (bits) {
        ch->volume = (volume << 16) / 16;
        ch->decay = 0;
        ch->flags &= ~NES_APU_LOOP;
        return;
    }
    ch->decay = 0x4000 / (volume + 1);
    if (*ctrl & NES_APU_LENGTH_HALT) {
        ch->flags |= NES_APU_LOOP;
        return;
    }
    ch->flags &= ~NES_APU_LOOP;
}

/* The same for the triangle (channel `idx`, 2): an octave lower for the same period, full volume, decaying by the
   linear counter's reload value unless that is halted. */
void nes_apu_update_triangle(s32 idx, s32 reg)
{
    Nes *nes;
    NesApuChannel *ch;
    s32 period;
    s32 bits;
    s32 regAddr;
    u8 *timerHi;

    nes = NES;
    regAddr = idx * 4;
    regAddr += NES_APU_TIMER_HI;
    timerHi = &nes->mem[regAddr];
    period = nes->mem[idx * 4 + NES_APU_TIMER_LO] | ((*timerHi & 7) << 8);
    ch = &NES->apu[idx];
    if (period == 0) {
        period = 1;
    }
    period = NES_APU_CLOCK / period;
    period = (period << 14) / NES_AUDIO_RATE;
    ch->rate = period;
    bits = *timerHi >> 3;
    if (bits & 1) {
        bits >>= 1;
        if (bits == 0) {
            ch->length = 0x7e;
        } else {
            ch->length = bits;
        }
    } else {
        ch->length = sNesLengthTable[bits >> 1];
    }
    /* the squares' halt bit (the triangle's is NES_APU_TRI_HALT) */
    if ((NES->mem[NES_APU_CTRL + idx * 4] & NES_APU_LENGTH_HALT) != 0) {
        ch->lengthStep = 0;
        ch->elapsed = 0;
    } else {
        ch->lengthStep = 1;
    }
    if (reg == 3) {
        ch->elapsed = 0;
    }
    if (((NES->mem[NES_APU_STATUS] >> idx) & 1) != 0) {
        ch->flags = ch->flags | NES_APU_ENABLED;
    } else {
        ch->flags = ch->flags & ~NES_APU_ENABLED;
    }
    ch->volume = 0xffff;
    bits = NES->mem[NES_APU_CTRL + idx * 4];
    if ((bits & NES_APU_TRI_HALT) != 0) {
        ch->decay = 0;
        return;
    }
    ch->decay = 0x40000 / ((bits & 0x7f) + 1);
}

/* The same for the noise channel (`idx`, 3): its rate from sNesNoisePeriods, the length and the envelope as for the
   squares. */
void nes_apu_update_noise(s32 idx, s32 reg)
{
    Nes *nes;
    NesApuChannel *ch;
    s32 rate;
    s32 bits;
    u8 *regs;
    u8 *ctrl;
    s32 regAddr;
    s32 constVolume;
    s32 ctrlByte, volume;
    s32 *periods;

    periods = sNesNoisePeriods;
    nes = NES;
    regAddr = idx * 4 + NES_APU_TIMER_LO;
    rate = periods[nes->mem[regAddr] & 0xF];
    rate = (rate * 0x28F) / 0xDC;
    regs = &nes->mem[idx * 4];
    ch = &NES->apu[idx];
    ch->rate = rate * 4;
    bits = regs[NES_APU_TIMER_HI] >> 3;
    if (bits & 1) {
        bits >>= 1;
        if (bits == 0) {
            ch->length = 0x7e;
        } else {
            ch->length = bits;
        }
    } else {
        ch->length = sNesLengthTable[bits >> 1];
    }
    if ((NES->mem[NES_APU_CTRL + idx * 4] & NES_APU_LENGTH_HALT) != 0) {
        ch->lengthStep = 0;
        ch->elapsed = 0;
    } else {
        ch->lengthStep = 1;
    }
    if (reg == 3) {
        ch->elapsed = 0;
    }
    if (((NES->mem[NES_APU_STATUS] >> idx) & 1) != 0) {
        ch->flags = ch->flags | NES_APU_ENABLED;
    } else {
        ch->flags = ch->flags & ~NES_APU_ENABLED;
    }
    if (reg == 0) {
        ch->volume = 0xffff;
    }
    regAddr = idx * 4;
    ctrl = &NES->mem[NES_APU_CTRL + regAddr];
    ctrlByte = *ctrl;
    constVolume = ctrlByte & NES_APU_CONST_VOLUME;
    volume = ctrlByte & NES_APU_VOLUME;
    if (constVolume != 0) {
        ch->volume = (volume << 16) / 16;
        ch->decay = 0;
        ch->flags = ch->flags & ~NES_APU_LOOP;
    } else {
        ch->decay = 0x4000 / (volume + 1);
        if ((*ctrl & NES_APU_LENGTH_HALT) != 0) {
            ch->flags = ch->flags | NES_APU_LOOP;
        } else {
            ch->flags = ch->flags & ~NES_APU_LOOP;
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
        bus_write(sNesEffAddr, sNesOperand);
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
        bus_write(sNesEffAddr, sNesOperand);
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
        bus_write(sNesEffAddr, sNesOperand);
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

    if (sNesCorruptRam != 0) {
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
    bus_write_2(sNesEffAddr, sNesOperand);
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
    bus_write_2(sNesEffAddr, c);
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

static inline s32 bus_read_2(s32 a)
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
    r = bus_read_2(a);
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
    r = bus_read((u16)a);
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
    r = bus_read(*out);
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
    r = bus_read((u16)b);
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

static inline u32 bus_read_3(s32 a)
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
    *(u32 *)&sNesOperand = bus_read_3(a); /* an unsigned store: as s32 the inlined read schedules differently */
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

static inline u32 ram_read(u32 addr) { return NES->mem[addr]; }

void nes_ea_indirect_indexed(void)
{
    Nes *nes = NES;
    u32 a = ram_read(nes->pc + 1);
    sNesEffAddr = a;
    sNesEffAddr = (ram_read(a) | (ram_read(a + 1) << 8)) + nes->y;
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
