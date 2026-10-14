#include "game.h"

/* The NES emulator (overview.md, NES emulator). The machine lives at NES (0x80380000). Some accesses to it are
   written as NES_AT(field), as numeric addresses, or as byte offsets added to the base, rather than NES->field:
   each form gives the original's address loads (compiler-notes.md, NES emulator), so they stay as they are. */

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
    /* The RGB palette to RGBA5551 (j, n, k: red, green, blue), opaque except one colour. The table has the NES's 64
       colours; the other 192 entries are converted from the data after it. */
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
            __builtin_memcpy(NES->mem, sNesSnapshot->ram, NES_RAM_SIZE);
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
    /* The message blinks: in both name tables for 30 frames, blanked for the next 30. */
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
            mem_fill(&sNesCpuMemory->vram[0x23F0], 0x5F, 8); /* the attribute bytes of tile rows 24-27 */
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
            save_write(SAVE_SECTION_NES_TRACK, 0);
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
        if ((NES->apu[i].flags & NES_APU_ENABLED) && NES->apu[i].elapsed < NES->apu[i].length
            && NES->apu[i].volume != 0) {
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

/* Frees the two audio buffers. */
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

/* A CPU write to a register: the PPU's $2003-$2007, the OAM DMA, the joypad strobe and the APU, whose channel
   records are updated. Every write also lands in Nes.mem, where the emulator reads PPUCTRL, PPUMASK and the APU
   registers back. */
void nes_write_io(s32 addr, s32 value)
{
    s32 channel;
    u8 *src;

    if (addr == NES_PPUDATA) {
        NES->vram[NES->vramAddr] = value;
        if (NES->mem[NES_PPUCTRL] & NES_PPUCTRL_INC32) {
            NES->vramAddr = (NES->vramAddr + 32) % NES_VRAM_SIZE;
        } else {
            NES->vramAddr = (NES->vramAddr + 1) % NES_VRAM_SIZE;
        }
    } else if (addr == NES_PPUADDR) {
        if (NES->ppuLatch & NES_LATCH_ADDR_LO) {
            NES->vramAddr = (NES->vramAddr & 0xFF00) | value;
            NES->ppuLatch |= NES_LATCH_FIRST_READ;
        } else {
            NES->vramAddr = (NES->vramAddr & 0xFF) | (value << 8);
        }
        NES->ppuLatch ^= NES_LATCH_ADDR_LO;
    } else if (addr == NES_JOY1) {
        if (value & 1) {
            NES->joypadBit = NES_JOYPAD_STROBE;
        } else if (NES->joypadBit == NES_JOYPAD_STROBE) {
            NES->joypadBit = 0;
        }
    } else if (addr == NES_PPUSCROLL) {
        if (NES->ppuLatch & NES_LATCH_SCROLL_Y) {
            NES->scrollY = value;
        } else {
            NES->scrollX = value;
        }
        NES->ppuLatch ^= NES_LATCH_SCROLL_Y;
    } else if (addr == NES_OAMADDR) {
        NES->oamAddr = value;
    } else if (addr == NES_OAMDATA) {
        NES->oam.bytes[NES->oamAddr++] = value;
    } else if (addr == NES_OAMDMA) {
        src = &NES->mem[value << 8];
        __builtin_memcpy(NES->oam.bytes, src, sizeof(NES->oam));
    } else {
        channel = addr - NES_APU_REGS; /* the register's offset, then (>> 2) its channel */
        if ((u32)channel < NES_APU_NUM_REGS) {
            NES->mem[addr] = value;
            channel >>= 2;
            if (channel < 2) {
                nes_apu_update_square(channel, addr & 3);
            } else if (channel == NES_APU_TRIANGLE) {
                nes_apu_update_triangle(NES_APU_TRIANGLE, addr & 3);
            } else if (channel == NES_APU_NOISE) {
                nes_apu_update_noise(NES_APU_NOISE, addr & 3);
            }
        }
    }
    NES->mem[addr] = value;
}

/* A CPU read of a register: $2007 (which does not advance the address on the first read after it was set), the
   joypad's next button and the APU status; other addresses read Nes.mem (where nes_run_frame keeps PPUSTATUS).
   The callers pass nothing in `latch` and `value`: the first is only scratch, and the joypad returns `value` as it
   came once its 8 buttons have been read. */
u32 nes_read_io(s32 addr, s32 latch, u32 value)
{
    u8 *ram;
    Nes *nes;
    s32 pos;
    s32 v;
    s32 bit;
    s32 i;
    u32 mask;

    if (addr == NES_PPUDATA) {
        nes = NES;
        __asm__ __volatile__("");
        addr = nes->vramAddr;
        latch = nes->ppuLatch;
        value = nes->vram[addr];
        if (latch & NES_LATCH_FIRST_READ) {
            nes->ppuLatch = latch & ~NES_LATCH_FIRST_READ;
        } else {
            if (nes->mem[NES_PPUCTRL] & NES_PPUCTRL_INC32) {
                nes->vramAddr = (addr + 0x20) % NES_VRAM_SIZE;
            } else {
                nes->vramAddr = (addr + 1) % NES_VRAM_SIZE;
            }
        }
        return value;
    }
    if (addr == NES_JOY1) {
        nes = NES;
        bit = nes->joypadBit;
        if (bit < 8) {
            nes->joypadBit = bit + 1;
            value = (nes->joypad >> bit) & 1;
        }
        return value;
    }
    if (addr == NES_APU_STATUS) {
        /* A bit per channel still playing. Each channel's count is compared with the noise channel's length
           (sNesNoiseLength is NES->apu[NES_APU_NOISE].length). */
        value = 0;
        for (i = 0; i < 4; i++) {
            if ((NES->apu[i].flags & NES_APU_ENABLED) && NES->apu[i].elapsed < sNesNoiseLength
                && NES->apu[i].volume != 0) {
                value |= 1 << i;
            }
        }
        return value & NES_AT(mem[NES_APU_STATUS]);
    }
    return NES->mem[addr];
}

/* The 6502 core. nes_run_frame calls an addressing mode (nes_am_*, or nes_ea_* for the stores), which leaves the
   effective address in sNesEffAddr and the value read in sNesOperand, then the operation (nes_op_*). sNesOperand16
   and sNesOperand8 are the low half and the low byte of sNesOperand, as is ((u8 *)&sNesOperand)[3]. Each flag is a
   byte of its own, nonzero when set. */

/* The shifts and rotates: on A in accumulator mode (sNesEffAddr NES_EA_NONE), else on the operand, which is written
   back. */
void nes_op_asl(void)
{
    s32 addr = sNesEffAddr;

    if (addr == NES_EA_NONE) {
        NES->c = NES->a & 0x80;
        NES->a <<= 1;
        NES->z = NES->a == 0;
        NES->n = NES->a & 0x80;
    } else {
        u32 result = sNesOperand << 1;
        NES->c = ((u8 *)&sNesOperand)[3] & 0x80;
        sNesOperand = result;
        if (addr < NES_IO_START) {
            NES->mem[addr & NES_RAM_MASK] = result;
        } else if (addr < NES_PRG_ROM) {
            nes_write_io(addr, result);
        }
        NES->z = (sNesOperand & 0xFF) == 0;
        NES->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

/* ASR, the emulator's opcode $4B (ALR, AND then LSR, among the 6502's undocumented ones): an arithmetic shift right.
   With its immediate operand it always works on A. */
void nes_op_asr(void)
{
    if (sNesEffAddr == NES_EA_NONE) {
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

    if (sNesEffAddr == NES_EA_NONE) {
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

    if (sNesEffAddr == NES_EA_NONE) {
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
    s32 addr = sNesEffAddr;

    if (addr == NES_EA_NONE) {
        Nes *nes = NES;
        u8 value = nes->a;
        nes->c = value & 1;
        value = value >> 1;
        nes->a = value;
        nes->z = (value == 0);
        nes->n = value & 0x80;
        return;
    } else {
        u8 *bytes = (u8 *)&sNesOperand;
        u8 low = bytes[3];
        s32 result;

        NES_AT(c) = low & 1;
        result = sNesOperand >> 1;
        sNesOperand = result;
        if (addr < NES_IO_START) {
            *(NES->mem + (addr & NES_RAM_MASK)) = (u8)result;
        } else if (addr <= 0x7FFF) {
            nes_write_io(addr, result);
        }
    }
    {
        Nes *nes = NES;
        nes->z = ((sNesOperand & 0xFF) == 0);
        nes->n = ((u8 *)&sNesOperand)[3] & 0x80;
    }
}

/* BRK: pushes the PC past its padding byte and the status, and jumps through the IRQ vector. */
void nes_op_brk(void)
{
    NES->pc++;
    NES->mem[NES_STACK + NES->s] = NES->pc >> 8;
    NES->s--;
    NES->mem[NES_STACK + NES->s] = NES->pc;
    NES->b = 1;
    NES->s--;
    NES->p = (NES->n ? NES_FLAG_N : 0) | (NES->v ? NES_FLAG_V : 0) | NES_FLAG_U | (NES->b ? NES_FLAG_B : 0)
           | (NES->d ? NES_FLAG_D : 0) | (NES->i ? NES_FLAG_I : 0) | (NES->z ? NES_FLAG_Z : 0)
           | (NES->c ? NES_FLAG_C : 0);
    NES->mem[NES_STACK + NES->s] = NES->p;
    NES->i = 1;
    NES->s--;
    NES->pc = NES->mem[NES_VECTOR_IRQ];
    NES->pc |= NES->mem[NES_VECTOR_IRQ + 1] << 8;
}

/* PHP: pushes the status byte built from the flags. */
void nes_op_php(void)
{
    NES->p = (NES->n ? NES_FLAG_N : 0) | (NES->v ? NES_FLAG_V | NES_FLAG_U : NES_FLAG_U)
        | (NES->b ? NES_FLAG_B : 0) | (NES->d ? NES_FLAG_D : 0) | (NES->i ? NES_FLAG_I : 0)
        | (NES->z ? NES_FLAG_Z : 0) | (NES->c ? NES_FLAG_C : 0);
    NES->mem[NES_STACK + NES->s] = NES->p;
    NES->s--;
}

/* RTI: pulls the status byte into the flags, then the PC as RTS does. */
void nes_op_rti(void)
{
    u32 sp;
    u32 status;
    u32 stack;

    stack = (u32)&NES->mem[NES_STACK];
    sp = (u32)sNesRegS + 1;
    NES_AT(s) = sp;
    sp = sp & 0xFF;
    status = *(u8 *)(stack | sp);
    NES_AT(n) = status & NES_FLAG_N;
    NES_AT(v) = status & NES_FLAG_V;
    NES_AT(b) = status & NES_FLAG_B;
    NES_AT(d) = status & NES_FLAG_D;
    NES_AT(i) = status & NES_FLAG_I;
    NES_AT(z) = status & NES_FLAG_Z;
    NES_AT(c) = status & NES_FLAG_C;
    nes_op_rts();
}

/* The NMI: pushes the PC and the status and jumps through the NMI vector. */
void nes_cpu_nmi(void)
{
    NES->mem[NES_STACK + NES->s] = NES->pc >> 8;
    NES->s--;
    NES->mem[NES_STACK + NES->s] = NES->pc;
    NES->s--;
    NES->p = (NES->n ? NES_FLAG_N : 0) | (NES->v ? NES_FLAG_V : 0) | NES_FLAG_U | (NES->b ? NES_FLAG_B : 0)
           | (NES->d ? NES_FLAG_D : 0) | (NES->i ? NES_FLAG_I : 0) | (NES->z ? NES_FLAG_Z : 0)
           | (NES->c ? NES_FLAG_C : 0);
    NES->mem[NES_STACK + NES->s] = NES->p;
    NES->i = 1;
    NES->s--;
    NES->pc = NES->mem[NES_VECTOR_NMI];
    NES->pc |= NES->mem[NES_VECTOR_NMI + 1] << 8;
}

/* The opcode being run (0x800F5BB4, after sNesFrameCount), reached from a base register. */
#define CUR_OPCODE (*(NesOpcode **)(glob + 0x5BB4))
#define PPUCTRL_REG (ram[NES_PPUCTRL])
#define PPUSTATUS_REG (ram[NES_PPUSTATUS])

/* Runs one frame of NES_LINES_PER_FRAME lines of NES_CYCLES_PER_LINE CPU cycles, from the start of the vertical
   blank: sets the PPUSTATUS bits, raises the NMIs, and calls sNesRowCallback in the middle of each row of tiles. */
void nes_run_frame(void)
{
    s32 i = 0;
    s32 line;
    u8 *glob = (u8 *)0x800F0000;
    u8 *ram = NES->mem;
    Nes *cpu = NES;

    do {
        line = (i + NES_VBLANK_LINE) % NES_LINES_PER_FRAME;
        if (NES_AT(cycles) < NES_CYCLES_PER_LINE) {
            NesOpcode *table = sNesOpcodeTable;
            u8 *dseg = (u8 *)0x800D0000;
            do {
                NesOpcode *opcode;
                CUR_OPCODE = (NesOpcode *)((s32)(*(u8 *)(cpu->pc + (s32)ram) * sizeof(NesOpcode)) + (s32)table);
                CUR_OPCODE->addrMode();
                opcode = CUR_OPCODE;
                cpu->pc += opcode->length;
                opcode->op(opcode);
                (*(s32 *)(dseg - 0x13E4))++; /* sNesInstrCount */
                cpu->cycles += CUR_OPCODE->cycles;
            } while (cpu->cycles < NES_CYCLES_PER_LINE);
        }
        cpu->cycles -= NES_CYCLES_PER_LINE;
        if ((line & 7) == 0) {
            if (line == 0) {
                PPUSTATUS_REG = 0x10; /* the end of vertical blank: VBLANK and SPRITE0 clear */
            } else if (line == NES_VBLANK_LINE) {
                PPUSTATUS_REG |= NES_PPUSTATUS_VBLANK;
                if (PPUCTRL_REG & NES_PPUCTRL_NMI) nes_cpu_nmi();
            }
        }
        if ((line & 7) == 4 && line < NES_VBLANK_LINE && sNesRowCallback != 0) sNesRowCallback(line >> 3);
        if (line == NES_AT(oam.sprites[0].y) && line < NES_VBLANK_LINE) {
            PPUSTATUS_REG |= NES_PPUSTATUS_SPRITE0;
            if (PPUCTRL_REG & NES_PPUCTRL_SPRITE0_NMI) nes_cpu_nmi();
        }
        i++;
    } while (i < NES_LINES_PER_FRAME);
    {
        Nes *nes = NES;
        (nes->frameCount)++;
    }
}

/* Resets the CPU: interrupts disabled, the PC from the reset vector; sets the row callback. */
s32 nes_cpu_reset(Nes *machine, void (*rowCallback)(s32))
{
    Nes *nes = NES;
    u8 *prg = &NES->mem[NES_PRG_ROM];
    sNesCpuMemory = machine;
    sNesRowCallback = rowCallback;
    nes->v = 0;
    nes->b = 0;
    nes->d = 0;
    nes->i = 1;
    nes->z = 0;
    nes->c = 0;
    nes->pc = (u16)(prg[0x7FFC] | (prg[0x7FFD] << 8)); /* NES_VECTOR_RESET */
    return 0;
}

/* ADC. V is set when bit 7 and up of the result differ from A's (a carry included), not the 6502's signed
   overflow. */
void nes_op_adc(void)
{
    Nes *nes = NES;
    u32 sum = nes->a;
    u32 old = sum;
    u32 signMask = 0xFFFFFF80;
    sum = sum + sNesOperand;
    if (nes->c != 0) {
        sum = sum + 1;
    }
    nes->z = (sum & 0xFF) == 0;
    nes->n = sum & signMask;
    nes->c = (sum & 0xFFFFFF00) != 0;
    nes->a = sum;
    nes->v = (old ^ sum) & signMask;
}

/* AND */
void nes_op_and(void)
{
    Nes *nes;
    u8 result;
    u8 zero;
    u8 neg;

    nes = NES;
    result = nes->a & sNesOperand8;
    zero = (result == 0);
    nes->a = result;
    neg = result & 0x80;
    nes->z = zero;
    nes->n = neg;
}

/* The branches: when taken, the PC becomes the target (sNesOperand16, from nes_am_relative) and the branch costs 2
   more cycles. */
void nes_op_bcc(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->c == 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

/* BCS; with sNesCorruptRam set (a debugging switch, 0) it also writes the count register to a random address of
   RDRAM. */
void nes_op_bcs(void)
{
    Nes *nes;
    s32 count;
    s32 junk;
    u32 addr;

    if (sNesCorruptRam != 0) {
        count = osGetCount();
        addr = ((count << 2) & 0xFFFFF) + 0x80000000;
        junk = osGetCount();
        *(s32 *)addr = junk;
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
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->z != 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

/* BIT: Z from A & operand; N and V are the operand's bits 7 and 6. */
void nes_op_bit(void)
{
    Nes *nes = NES;
    nes->z = ((nes->a & sNesOperand) == 0);
    {
        u8 value = ((u8 *)&sNesOperand)[3];
        nes->n = value & 0x80;
        nes->v = value & 0x40;
    }
}

void nes_op_bmi(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->n != 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

void nes_op_bne(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->z == 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

void nes_op_bpl(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->n == 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

void nes_op_bvc(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->v == 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

void nes_op_bvs(void)
{
    Nes *nes;
    u16 target;
    s32 cycles;

    nes = NES;
    if (nes->v != 0) {
        target = sNesOperand16;
        cycles = nes->cycles + 2;
        nes->pc = target;
        nes->cycles = cycles;
    }
}

/* CLC, CLD, CLI, CLV */
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

/* CMP, CPX, CPY. N is set when the register is below the operand, not from bit 7 of the difference as on the
   6502. */
void nes_op_cmp(void)
{
    Nes *nes = NES;
    s32 reg = nes->a;
    s32 operand = sNesOperand;
    if (operand < reg) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (reg == operand) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

void nes_op_cpx(void)
{
    Nes *nes = NES;
    s32 reg = nes->x;
    s32 operand = sNesOperand;
    if (operand < reg) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (reg == operand) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

void nes_op_cpy(void)
{
    Nes *nes = NES;
    s32 reg = nes->y;
    s32 operand = sNesOperand;
    if (operand < reg) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 0;
        return;
    }
    if (reg == operand) {
        nes->n = 0;
        nes->c = 1;
        nes->z = 1;
        return;
    }
    nes->n = 1;
    nes->c = 0;
    nes->z = 0;
}

/* DEC, DEX, DEY */
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
    s32 result = nes->x + -1;
    u8 neg;
    nes->x = result;
    neg = (u8)result & 0x80;
    nes->z = ((u8)result == 0);
    nes->n = neg;
}

void nes_op_dey(void)
{
    Nes *nes = NES;
    s32 result = nes->y + -1;
    u8 neg;
    nes->y = result;
    neg = (u8)result & 0x80;
    nes->z = ((u8)result == 0);
    nes->n = neg;
}

/* EOR */
void nes_op_eor(void)
{
    Nes *nes = NES;
    u8 result = nes->a ^ sNesOperand8;
    nes->a = result;
    nes->z = (result == 0);
    nes->n = result & 0x80;
}

/* INC, INX, INY */
void nes_op_inc(void)
{
    Nes *nes;
    s32 result = sNesOperand + 1;

    sNesOperand = result;
    bus_write_2(sNesEffAddr, result);
    nes = NES;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = ((u8 *)&sNesOperand)[3] & 0x80;
}

void nes_op_inx(void)
{
    Nes *nes = NES;
    s32 result = nes->x + 1;
    u8 neg;
    nes->x = result;
    neg = (u8)result & 0x80;
    nes->z = ((u8)result == 0);
    nes->n = neg;
}

void nes_op_iny(void)
{
    Nes *nes = NES;
    s32 result = nes->y + 1;
    u8 neg;
    nes->y = result;
    neg = (u8)result & 0x80;
    nes->z = ((u8)result == 0);
    nes->n = neg;
}

/* JMP and JSR. JSR pushes the address of the next instruction (the PC has already moved past it), which RTS pulls
   as it is. */
void nes_op_jmp(void)
{
    NES_AT(pc) = sNesOperand16;
}

void nes_op_jsr(void)
{
    Nes *nes = NES;
    u32 ramAddr = (u32)NES->mem;
    u8 pcLo;
    u32 hiSlot;
    u32 loSlot;

    hiSlot = nes->s;
    hiSlot += NES_STACK;
    *(u8 *)(ramAddr + hiSlot) = (u8)(nes->pc >> 8);
    pcLo = nes->pc;
    nes->s = nes->s - 1;
    loSlot = (u8)nes->s;
    loSlot += NES_STACK;
    *(u8 *)(ramAddr + loSlot) = pcLo;
    nes->s = nes->s - 1;
    nes->pc = sNesOperand16;
}

/* LDA, LDX, LDY */
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
    s8 value = sNesOperand;
    nes->x = value;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = value & 0x80;
}

void nes_op_ldy(void)
{
    Nes *nes = NES;

    nes->y = sNesOperand;
    nes->z = (sNesOperand & 0xFF) == 0;
    nes->n = sNesOperand & 0x80;
}

/* NOP */
void nes_op_nop(void)
{
}

/* ORA */
void nes_op_ora(void)
{
    Nes *nes;
    u8 result;
    u8 zero;
    u8 neg;

    nes = NES;
    result = nes->a | sNesOperand8;
    zero = (result == 0);
    nes->a = result;
    neg = result & 0x80;
    nes->z = zero;
    nes->n = neg;
}

/* PHA, PLA, PLP, RTS: the stack is page $01, `s` the next free slot. */
void nes_op_pha(void)
{
    Nes *nes;
    u32 slot;

    nes = NES;
    slot = nes->s | (u32)&NES->mem[NES_STACK];
    *(u8 *)slot = nes->a;
    nes->s = nes->s - 1;
}

void nes_op_pla(void)
{
    Nes *nes;
    u32 slot;

    nes = NES;
    nes->s = nes->s + 1;
    slot = (u8)nes->s | (u32)&NES->mem[NES_STACK];
    nes->a = *(u8 *)slot;
}

/* PLP: pulls the status byte and splits it into the flag bytes. */
void nes_op_plp(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    u32 status;
    s32 slot;

    nes->s++;
    slot = nes->s + NES_STACK;
    status = ram[slot];
    nes->n = status & NES_FLAG_N;
    nes->v = status & NES_FLAG_V;
    nes->b = status & NES_FLAG_B;
    nes->d = status & NES_FLAG_D;
    nes->i = status & NES_FLAG_I;
    nes->z = status & NES_FLAG_Z;
    nes->c = status & NES_FLAG_C;
}

/* RTS: pulls the PC, low byte first. */
void nes_op_rts(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    u32 lo, hi;
    s32 loSlot, hiSlot;

    nes->s++;
    loSlot = nes->s + NES_STACK;
    lo = ram[loSlot];
    nes->s++;
    hiSlot = nes->s + NES_STACK;
    hi = ram[hiSlot];
    nes->pc = lo | (hi << 8);
}

/* SBC, with V as in nes_op_adc. */
void nes_op_sbc(void)
{
    Nes *nes = NES;
    s32 result = nes->a;
    s32 old = result;
    s32 changed;
    result = result - sNesOperand;
    if (nes->c == 0) {
        result = result - 1;
    }
    nes->c = ((u32)~result) >> 31;
    nes->z = ((result & 0xFF) == 0);
    nes->n = result & -0x80;
    changed = old ^ result;
    nes->a = result;
    nes->v = changed & -0x80;
}

/* SEC, SED, SEI. There is no decimal mode: SED halts the game. */
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

/* STA, STX, STY */
void nes_op_sta(void)
{
    Nes *nes = NES;
    s32 addr = sNesEffAddr;
    u32 value = nes->a;
    if (addr < NES_IO_START) {
        NES->mem[addr & NES_RAM_MASK] = value;
    } else if (addr < NES_PRG_ROM) {
        nes_write_io(addr, value);
    }
}

void nes_op_stx(void)
{
    s32 addr = sNesEffAddr;
    u32 value = NES_AT(x);
    if (addr < NES_IO_START) {
        *(NES->mem + (addr & NES_RAM_MASK)) = value;
    } else if (addr < NES_PRG_ROM) {
        nes_write_io(addr, value);
    }
}

void nes_op_sty(void)
{
    s32 addr = sNesEffAddr;
    s32 value = NES_AT(y);

    if (addr < NES_IO_START) {
        *(NES->mem + (addr & NES_RAM_MASK)) = value;
    } else if (addr < NES_PRG_ROM) {
        nes_write_io(addr, value);
    }
}

/* The transfers TAX, TAY, TSX, TXA, TXS, TYA */
void nes_op_tax(void)
{
    Nes *nes = NES;
    u8 value = nes->a;
    nes->x = value;
    nes->z = (value == 0);
    nes->n = value & 0x80;
}

void nes_op_tay(void)
{
    Nes *nes = NES;
    u8 value = nes->a;
    nes->y = value;
    nes->z = (value == 0);
    nes->n = value & 0x80;
}

void nes_op_tsx(void)
{
    Nes *nes = NES;
    u8 value = nes->s;
    nes->x = value;
    nes->z = (value == 0);
    nes->n = value & 0x80;
}

void nes_op_txa(void)
{
    Nes *nes = NES;
    u8 value = nes->x;
    nes->a = value;
    nes->z = (value == 0);
    nes->n = value & 0x80;
}

void nes_op_txs(void)
{
    Nes *nes = NES;
    nes->s = nes->x;
}

void nes_op_tya(void)
{
    Nes *nes = NES;
    u8 value = nes->y;
    nes->a = value;
    nes->z = (value == 0);
    nes->n = value & 0x80;
}

/* The addressing modes. Each has an empty twin before it (NesOpcode.addrModeStub), never called. They run before
   the PC moves past the instruction, so the operand bytes are at pc + 1 and pc + 2. */
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
    sNesEffAddr = NES_EA_NONE;
    sNesOperand = NES_AT(a);
}

void nes_am_absolute_stub(void)
{
}

void nes_am_absolute(void)
{
    u8 *ram = NES->mem;
    s32 pc = NES_AT(pc);
    u8 *instr = ram + pc;
    s32 addr, value;

    sNesEffAddr = instr[1] | (ram[(u16)(pc + 2)] << 8);
    addr = sNesEffAddr;
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr > 0x7FFF) {
    } else {
        value = nes_read_io_2(addr);
        goto store;
    }
    value = NES->mem[addr];
store:
    sNesOperand = value;
}

void nes_am_zeropage_stub(void)
{
}

/* bus_read again, for nes_am_zeropage */
static inline s32 bus_read_2(s32 addr)
{
    if (addr < NES_IO_START) {
        addr = addr & NES_RAM_MASK;
    } else if (addr > 0x7FFF) {
    } else {
        return nes_read_io_2(addr);
    }
    return NES->mem[addr];
}

void nes_am_zeropage(void)
{
    s32 addr = NES->mem[NES_AT(pc) + 1];
    s32 value;
    sNesEffAddr = addr;
    value = bus_read_2(addr);
    sNesOperand = value;
}

void nes_am_immediate_stub(void)
{
}

void nes_am_immediate(void)
{
    Nes *nes;
    u8 *ram;
    u32 pc;

    nes = NES;
    ram = NES->mem;
    sNesEffAddr = NES_EA_NONE;
    pc = nes->pc;
    sNesOperand = ram[pc + 1];
}

void nes_am_absolute_x_stub(void)
{
}

void nes_am_absolute_x(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    s32 sum, value, addr;

    sum = (instr[1] | (ram[(u16)(pc + 2)] << 8)) + nes->x;
    sNesEffAddr = sum;
    addr = (u16)sum;
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr > 0x7FFF) {
    } else {
        value = nes_read_io_2(addr);
        goto store;
    }
    value = NES->mem[addr];
store:
    sNesOperand = value;
}

void nes_am_absolute_y_stub(void)
{
}

void nes_am_absolute_y(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    u32 value;
    s32 addr;

    addr = (instr[1] | (ram[(u16)(pc + 2)] << 8)) + nes->y;
    sNesEffAddr = addr;
    value = bus_read((u16)addr);
    sNesOperand = value;
}

void nes_am_indexed_indirect_stub(void)
{
}

/* (zp,X) */
void nes_am_indexed_indirect(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    s32 ptr;
    u32 value;

    ptr = instr[1] + nes->x;
    *out = ptr;
    *out = ram[ptr] | (ram[(u16)(ptr + 1)] << 8);
    value = bus_read(*out);
    sNesOperand = value;
}

void nes_am_indirect_indexed_stub(void)
{
}

/* (zp),Y */
void nes_am_indirect_indexed(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    s32 ptr, addr;
    u32 value;

    ptr = instr[1];
    *out = ptr;
    addr = (ram[ptr] | (ram[(u16)(ptr + 1)] << 8)) + nes->y;
    *out = addr;
    value = bus_read((u16)addr);
    sNesOperand = value;
}

void nes_am_zeropage_x_stub(void)
{
}

void nes_am_zeropage_x(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 addr, value;

    sNesEffAddr = ram[nes->pc + 1] + nes->x;
    addr = sNesEffAddr;
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr > 0x7FFF) {
    } else {
        value = nes_read_io_2(addr);
        goto store;
    }
    value = NES->mem[addr];
store:
    sNesOperand = value;
}

void nes_am_zeropage_y_stub(void)
{
}

/* bus_read again, for nes_am_zeropage_y */
static inline u32 bus_read_3(s32 addr)
{
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr <= 0x7FFF) {
        return nes_read_io_3(addr);
    }
    return NES->mem[addr];
}

void nes_am_zeropage_y(void)
{
    Nes *nes = NES;
    u8 *ram = NES->mem;
    s32 addr;
    addr = ram[nes->pc + 1] + nes->y;
    sNesEffAddr = addr;
    *(u32 *)&sNesOperand = bus_read_3(addr); /* an unsigned store: as s32 the inlined read schedules differently */
}

void nes_am_relative_stub(void)
{
}

/* The branch target: the next instruction plus the signed offset. */
void nes_am_relative(void)
{
    s32 *dst = &sNesOperand;
    u32 pc = NES_AT(pc);
    s32 target = pc + (s8)NES->mem[pc + 1] + 2;
    *dst = target & 0xFFFF;
}

void nes_am_jump_absolute_stub(void)
{
}

void nes_am_jump_absolute(void)
{
    u32 *out = &sNesOperand;
    u8 *ram = NES->mem;
    s32 pc = NES_AT(pc);
    u8 *instr = ram + pc;
    *out = instr[1] | (ram[(u16)(pc + 2)] << 8);
}

void nes_am_jump_indirect_stub(void)
{
}

/* JMP (addr) */
void nes_am_jump_indirect(void)
{
    u32 *out = &sNesOperand;
    u8 *ram = NES->mem;
    s32 pc = NES_AT(pc);
    u8 *instr = ram + pc;
    u16 ptr = (ram[(u16)(pc + 2)] << 8) | instr[1];
    u8 *vector = ram + ptr;
    u32 target;
    *out = ptr;
    target = vector[0] | (ram[(u16)(ptr + 1)] << 8);
    *out = target;
}

/* The effective address alone, for the stores (nothing is read). */
void nes_ea_absolute(void)
{
    s32 *dst = &sNesEffAddr;
    u8 *ram = NES->mem;
    u32 pc = NES_AT(pc);
    u8 *instr = (u8 *)((u32)ram + pc);
    *dst = (u32)instr[1] | ((u32)ram[(u16)(pc + 2)] << 8);
}

void nes_ea_zeropage(void)
{
    u32 pc = NES_AT(pc);
    sNesEffAddr = NES->mem[pc + 1];
}

void nes_ea_absolute_x(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    *out = (instr[1] | (ram[(u16)(pc + 2)] << 8)) + nes->x;
}

void nes_ea_absolute_y(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    *out = (instr[1] | (ram[(u16)(pc + 2)] << 8)) + nes->y;
}

void nes_ea_indexed_indirect(void)
{
    u32 *out = &sNesEffAddr;
    u8 *ram = NES->mem;
    Nes *nes = NES;
    s32 pc = nes->pc;
    u8 *instr = ram + pc;
    s32 ptr = instr[1] + nes->x;
    *out = ptr;
    *out = ram[ptr] | (ram[(u16)(ptr + 1)] << 8);
}

/* A plain read of the CPU address space (no mirroring or registers), for nes_ea_indirect_indexed */
static inline u32 ram_read(u32 addr) { return NES->mem[addr]; }

void nes_ea_indirect_indexed(void)
{
    Nes *nes = NES;
    u32 ptr = ram_read(nes->pc + 1);
    sNesEffAddr = ptr;
    sNesEffAddr = (ram_read(ptr) | (ram_read(ptr + 1) << 8)) + nes->y;
}

void nes_ea_zeropage_x(void)
{
    Nes *nes = NES;
    u32 pc = nes->pc;
    sNesEffAddr = NES->mem[pc + 1] + nes->x;
}

void nes_ea_zeropage_y(void)
{
    Nes *nes;
    u8 *ram;
    u32 pc;

    nes = NES;
    ram = NES->mem;
    pc = nes->pc;
    sNesEffAddr = ram[pc + 1] + nes->y;
}

/* Unused: a write to an APU register, passed on to its channel (as nes_write_io does). */
void nes_apu_write(s32 addr, u8 value)
{
    s32 channel;

    NES->mem[addr] = value;
    channel = (addr - NES_APU_REGS) >> 2;
    if (channel < 2) {
        nes_apu_update_square(channel, addr & 3);
    } else if (channel == NES_APU_TRIANGLE) {
        nes_apu_update_triangle(NES_APU_TRIANGLE, addr & 3);
    } else if (channel == NES_APU_NOISE) {
        nes_apu_update_noise(NES_APU_NOISE, addr & 3);
    }
}

/* Unused: a CPU read and write of any address. */
s32 nes_cpu_read(s32 addr)
{
    addr &= 0xFFFF;
    if (addr < NES_IO_START) {
        addr &= NES_RAM_MASK;
    } else if (addr < NES_PRG_ROM) {
        return nes_read_io_4(addr);
    }
    return NES->mem[addr];
}

void nes_cpu_write(s32 addr, s32 value)
{
    if (addr < NES_IO_START) {
        u8 *ram = NES->mem;
        ram[addr & NES_RAM_MASK] = value;
    } else if (addr < NES_PRG_ROM) {
        nes_write_io(addr, value);
    }
}
