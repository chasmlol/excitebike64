/* The built-in NES emulator (nesn64.c): the emulated machine and the 6502 opcode table. */
#ifndef STRUCTS_NES_H
#define STRUCTS_NES_H

#include "common.h"

/* One APU channel as the mixer (nes_apu_render) plays it: two squares, the triangle and the noise channel. */
typedef struct NesApuChannel {
    /* 0x00 */ s32 rate;        /* phase step per output sample; the mixer adds its low half to `phase` */
    /* 0x04 */ s32 unk_04;      /* cleared with `rate`; otherwise unused */
    /* 0x08 */ s32 volume;      /* 16.16; 0 = silent */
    /* 0x0C */ s32 decay;       /* taken off `volume` after each audio chunk (the envelope) */
    /* 0x10 */ u16 phase;
    /* 0x12 */ u16 duty;        /* squares: the phase above which the output is high (sNesDutyTable) */
    /* 0x14 */ s32 length;      /* length counter, in audio chunks */
    /* 0x18 */ s32 elapsed;     /* audio chunks played; the channel stops when it reaches `length` */
    /* 0x1C */ s32 lengthStep;  /* added to `elapsed` per chunk: 0 while the length counter is halted */
    /* 0x20 */ u32 flags;       /* NES_APU_ENABLED, NES_APU_LOOP */
} NesApuChannel; /* size 0x24 */

#define NES_APU_ENABLED 8       /* the channel's bit in $4015 */
#define NES_APU_LOOP 0x10       /* the envelope restarts instead of falling silent */

/* Nes.apu[] */
#define NES_APU_SQUARE1     0
#define NES_APU_SQUARE2     1
#define NES_APU_TRIANGLE    2
#define NES_APU_NOISE       3

/* A sprite in OAM. */
typedef struct NesSprite {
    /* 0x00 */ u8 y;            /* top line - 1; 0xF1 and more is off screen */
    /* 0x01 */ u8 tile;
    /* 0x02 */ u8 attr;         /* NES_SPR_* */
    /* 0x03 */ u8 x;
} NesSprite; /* size 0x4 */

#define NES_SPR_PALETTE     3       /* sprite palette 0-3 */
#define NES_SPR_BEHIND      0x20    /* behind the background */
#define NES_SPR_FLIP_X      0x40
#define NES_SPR_Y_OFFSCREEN 0xF1    /* y from here on hides the sprite */

/* The emulated NES: a fixed block at 0x80380000 (NES), outside the heap; nes_init clears it. */
typedef struct Nes {
    /* 0x00000 */ u8 mem[0x10000];  /* CPU address space: 2 KB of RAM (mirrored), register shadows at $2000 and
                                       $4000, PRG ROM at $8000 and $C000 */
    /* 0x10000 */ u8 vram[0x4000];  /* PPU address space: CHR at $0000, name tables at $2000, palettes at $3F00 */
    /* 0x14000 */ union {
        u8 bytes[0x100];        /* as $2003/$2004 and the $4014 DMA write it */
        NesSprite sprites[64];
    } oam;
    /* 0x14100 */ u16 pc;
    /* 0x14102 */ u8 a;
    /* 0x14103 */ u8 x;
    /* 0x14104 */ u8 y;
    /* 0x14105 */ u8 s;             /* stack pointer, page $01 */
    /* 0x14106 */ u8 v;             /* the flags, one byte each: nonzero = set */
    /* 0x14107 */ u8 b;
    /* 0x14108 */ u8 n;
    /* 0x14109 */ u8 d;
    /* 0x1410A */ u8 i;
    /* 0x1410B */ u8 z;
    /* 0x1410C */ u8 c;
    /* 0x1410D */ u8 p;             /* the status byte last built from the flags or restored */
    /* 0x1410E */ u8 unk_1410E[2];
    /* 0x14110 */ u32 cycles;       /* CPU cycles into the current scanline */
    /* 0x14114 */ u16 vramAddr;     /* $2006 / $2007 address */
    /* 0x14116 */ u8 unk_14116[2];
    /* 0x14118 */ u32 ppuLatch;     /* NES_LATCH_* */
    /* 0x1411C */ u8 scrollX;
    /* 0x1411D */ u8 scrollY;
    /* 0x1411E */ u8 unk_1411E;
    /* 0x1411F */ u8 oamAddr;       /* $2003 */
    /* 0x14120 */ u8 joypad;        /* NES_PAD_* buttons as $4016 returns them, A first */
    /* 0x14121 */ u8 unk_14121[3];  /* [0] is cleared with `joypad` when the snapshot is restored; otherwise unused */
    /* 0x14124 */ s32 joypadBit;    /* next button $4016 returns; NES_JOYPAD_STROBE while the strobe is held */
    /* 0x14128 */ s32 unk_14128;    /* cleared with `joypadBit` when the snapshot is restored; otherwise unused */
    /* 0x1412C */ NesApuChannel apu[4];
    /* 0x141BC */ s32 frameCount;
} Nes; /* size 0x141C0 */

#define NES ((Nes *)0x80380000)

/* Nes.ppuLatch */
#define NES_LATCH_ADDR_LO       1   /* the next $2006 write is the low byte */
#define NES_LATCH_FIRST_READ    2   /* the address was just set: the next $2007 read does not advance it */
#define NES_LATCH_SCROLL_Y      4   /* the next $2005 write is Y */

/* The CPU address space (Nes.mem). Addresses below $2000 are the 2 KB of RAM, mirrored; $2000-$7FFF are registers
   (nes_write_io, nes_read_io); writes to the PRG ROM, $8000 and up, are ignored. */
#define NES_RAM_SIZE        0x800
#define NES_RAM_MASK        0x7FF
#define NES_IO_START        0x2000
#define NES_STACK           0x100   /* the stack page */
#define NES_PRG_ROM         0x8000
#define NES_PRG_ROM_MIRROR  0xC000  /* Excitebike's 16 KB PRG ROM again */
#define NES_PRG_SIZE        0x4000
#define NES_CHR_SIZE        0x2000  /* the CHR ROM (pattern tables), after the PRG ROM in the resource */

/* PPU registers */
#define NES_PPUCTRL     0x2000
#define NES_PPUMASK     0x2001
#define NES_PPUSTATUS   0x2002
#define NES_OAMADDR     0x2003
#define NES_OAMDATA     0x2004
#define NES_PPUSCROLL   0x2005
#define NES_PPUADDR     0x2006
#define NES_PPUDATA     0x2007

/* $2000 PPUCTRL */
#define NES_PPUCTRL_NAMETABLE   3       /* base name table, $2000 + 0x400 * n */
#define NES_PPUCTRL_INC32       4       /* $2007 accesses step the address by 32 instead of 1 */
#define NES_PPUCTRL_SPR_TABLE   8       /* sprite tiles from the pattern table at $1000 */
#define NES_PPUCTRL_BG_TABLE    0x10    /* background tiles from the pattern table at $1000 */
#define NES_PPUCTRL_SPRITE0_NMI 0x40    /* the emulator's own: an NMI on the sprite 0 line too (for Excitebike's status
                                           bar split; master/slave select on the NES) */
#define NES_PPUCTRL_NMI         0x80    /* NMI at the start of vertical blank */

/* $2001 PPUMASK */
#define NES_PPUMASK_BG              8       /* show the background */
#define NES_PPUMASK_SPRITES         0x10    /* show the sprites */
#define NES_PPUMASK_EMPHASIS_SHIFT  5       /* the colour emphasis bits; here they pick sNesBorderColors */

/* $2002 PPUSTATUS */
#define NES_PPUSTATUS_SPRITE0   0x40    /* sprite 0 hit */
#define NES_PPUSTATUS_VBLANK    0x80

/* APU and joypad registers. The four registers of APU channel n (NES_APU_SQUARE1...) are at $4000 + 4 * n. */
#define NES_APU_REGS        0x4000
#define NES_APU_CTRL        0x4000  /* NES_APU_DUTY_SHIFT, NES_APU_LENGTH_HALT, NES_APU_CONST_VOLUME, NES_APU_VOLUME */
#define NES_APU_SWEEP       0x4001
#define NES_APU_TIMER_LO    0x4002
#define NES_APU_TIMER_HI    0x4003  /* timer bits 8-10 (& 7) and the length (>> 3) */
#define NES_APU_NUM_REGS    0x16    /* $4000-$4015 */
#define NES_OAMDMA          0x4014  /* copies a 256-byte page to OAM */
#define NES_APU_STATUS      0x4015  /* a bit per channel: enabled; reads: still playing */
#define NES_JOY1            0x4016  /* writes: strobe (bit 0); reads: the next button */

/* NES_APU_CTRL bits */
#define NES_APU_DUTY_SHIFT      6       /* squares: duty cycle (sNesDutyTable) */
#define NES_APU_LENGTH_HALT     0x20    /* the length counter is halted (and the envelope loops) */
#define NES_APU_CONST_VOLUME    0x10    /* a constant volume instead of the decay envelope */
#define NES_APU_VOLUME          0xF     /* the volume, or the envelope's period */
#define NES_APU_TRI_HALT        0x80    /* triangle: the linear counter is halted (no decay) */

#define NES_APU_CLOCK   0x36EE8     /* 225000, about the CPU clock / 8 (223722) */
#define NES_AUDIO_RATE  22050       /* output sample rate */

/* sNesEffAddr when the instruction has no memory operand (accumulator and immediate modes): the read-modify-write
   operations then work on A. */
#define NES_EA_NONE (-1)

/* 6502 vectors */
#define NES_VECTOR_NMI      0xFFFA
#define NES_VECTOR_RESET    0xFFFC
#define NES_VECTOR_IRQ      0xFFFE  /* IRQ and BRK */

/* The 6502 status byte (Nes.p). The emulator keeps each flag in a byte of its own (Nes.n ... Nes.c). */
#define NES_FLAG_C  0x01
#define NES_FLAG_Z  0x02
#define NES_FLAG_I  0x04
#define NES_FLAG_D  0x08
#define NES_FLAG_B  0x10
#define NES_FLAG_U  0x20    /* unused: always 1 when pushed */
#define NES_FLAG_V  0x40
#define NES_FLAG_N  0x80

/* NES controller buttons (Nes.joypad), in the order $4016 returns them, bit 0 first. */
#define NES_PAD_A       0x01
#define NES_PAD_B       0x02
#define NES_PAD_SELECT  0x04
#define NES_PAD_START   0x08
#define NES_PAD_UP      0x10
#define NES_PAD_DOWN    0x20
#define NES_PAD_LEFT    0x40
#define NES_PAD_RIGHT   0x80
#define NES_JOYPAD_STROBE 0x7B  /* Nes.joypadBit while the strobe is held */

/* Frame timing (nes_run_frame): the emulator runs fewer lines of more cycles than the NES (262 lines of 113.7). */
#define NES_LINES_PER_FRAME 0xF8    /* 248 */
#define NES_VBLANK_LINE     0xF0    /* after the 240 visible lines: vertical blank and the NMI */
#define NES_CYCLES_PER_LINE 0x78    /* 120 */

/* PPU memory (Nes.vram) */
#define NES_VRAM_SIZE           0x4000  /* $2006/$2007 addresses wrap here */
#define NES_VRAM_NAMETABLE      0x2000
#define NES_NAMETABLE_SIZE      0x400   /* 32 x 30 tiles, then the attribute table */
#define NES_ATTR_TABLE          0x3C0   /* the attribute table's offset in a name table: a palette per 2x2 tiles */
#define NES_VRAM_BG_PALETTE     0x3F00
#define NES_VRAM_SPR_PALETTE    0x3F10

/* Drawing */
#define NES_SCREEN_X        0x1C    /* the NES picture's left edge in the 320-pixel framebuffer */
#define NES_TILES_TABLE_SIZE 0x4000 /* a pattern table decoded into sNesChrTiles: 256 tiles of 16x8 CI4 pixels */
#define NES_BLANK_FRAMES    10      /* frames drawn blank (border colour only) after nes_init */

/* The exit message ("HOLD L AND Z BUTTONS TO EXIT", sNesExitMsg) in Excitebike's name tables */
#define NES_EXIT_MSG_ADDR   0x2342  /* row 26, column 2 of the first name table (also written to the second) */
#define NES_TILE_FROM_ASCII 0x37    /* taken off an ASCII letter: 'A' is tile 0x0A */
#define NES_TILE_BLANK      0xFC

/* Where Excitebike's PC stops in design mode while it waits for the Data Recorder (nes_draw_frame) */
#define NES_PC_SAVE_1       0xC661
#define NES_PC_SAVE_2       0xC66A
#define NES_PC_LOAD_1       0xC641
#define NES_PC_LOAD_2       0xC69A
#define NES_RESTORE_DELAY   0x3C    /* frames from a SAVE or LOAD to the snapshot restore (sNesRestoreTimer) */

/* Excitebike's design-mode track in NES RAM, which the Famicom version saved to the Data Recorder: the emulator copies
   it to and from the save block (gSaveData.nesTrack). */
#define NES_TRACK_ADDR 0x5E0
#define NES_TRACK_SIZE 0x220

/* The same field as NES->field, reached through its constant address: a `lui $at` of its own at each access, where
   NES->field lets the compiler keep the base in a register (both forms are in the original). */
#define NES_AT(field) (*&NES->field)

/* The machine state saved in the NES resource after the PRG and CHR ROM (sNesSnapshot). nes_update_input restores it
   60 frames after a design-mode SAVE or LOAD, then puts the track back from the save block. */
typedef struct NesSnapshot {
    /* 0x0000 */ u8 ram[0x800];
    /* 0x0800 */ u8 vram[0x2000];   /* PPU $2000-$3FFF: name tables and palettes */
    /* 0x2800 */ u16 pc;
    /* 0x2802 */ u16 vramAddr;
    /* 0x2804 */ u8 a;
    /* 0x2805 */ u8 x;
    /* 0x2806 */ u8 y;
    /* 0x2807 */ u8 s;
    /* 0x2808 */ u8 p;
    /* 0x2809 */ u8 ppuLatch;
    /* 0x280A */ u8 scrollX;
    /* 0x280B */ u8 scrollY;
    /* 0x280C */ u8 oamAddr;
    /* 0x280D */ u8 unk_280D[5];
} NesSnapshot; /* size 0x2812 */

/* An entry of the opcode table (sNesOpcodeTable, 256 entries continued by sNesOpcodeTableTail). nes_run_frame
   calls the addressing mode, advances the PC by `length`, calls the operation and adds `cycles`. */
typedef struct NesOpcode {
    /* 0x00 */ char *name;                      /* mnemonic */
    /* 0x04 */ void (*addrModeStub)(void);      /* the addressing mode's empty twin, never called */
    /* 0x08 */ void (*addrMode)(void);          /* leaves sNesEffAddr and sNesOperand */
    /* 0x0C */ void (*op)(struct NesOpcode *);
    /* 0x10 */ s16 cycles;                      /* -1 (with `length` 0xFFFF) for an undefined opcode */
    /* 0x12 */ u16 length;
    /* 0x14 */ u32 unk_14;
} NesOpcode; /* size 0x18 */

#endif
