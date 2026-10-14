/* The built-in NES emulator (nesn64.c): the emulated machine and the 6502 opcode table. */
#ifndef STRUCTS_NES_H
#define STRUCTS_NES_H

#include "common.h"

/* One APU channel as the mixer (nes_apu_render) plays it: two squares, the triangle and the noise channel. */
typedef struct NesApuChannel {
    /* 0x00 */ s32 rate;        /* phase step per output sample; the mixer adds its low half to `phase` */
    /* 0x04 */ s32 unk_04;
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

/* A sprite in OAM. */
typedef struct NesSprite {
    /* 0x00 */ u8 y;            /* top line - 1; 0xF1 and more is off screen */
    /* 0x01 */ u8 tile;
    /* 0x02 */ u8 attr;         /* 3: palette, 0x20: behind the background, 0x40: flip X */
    /* 0x03 */ u8 x;
} NesSprite; /* size 0x4 */

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
    /* 0x14118 */ u32 ppuLatch;     /* 1: next $2006 write is the low byte, 2: next $2007 read is the buffered one,
                                       4: next $2005 write is Y */
    /* 0x1411C */ u8 scrollX;
    /* 0x1411D */ u8 scrollY;
    /* 0x1411E */ u8 unk_1411E;
    /* 0x1411F */ u8 oamAddr;       /* $2003 */
    /* 0x14120 */ u8 joypad;        /* buttons as $4016 returns them, A first */
    /* 0x14121 */ u8 unk_14121[3];
    /* 0x14124 */ s32 joypadBit;    /* next button $4016 returns; 0x7B while the strobe is held */
    /* 0x14128 */ s32 unk_14128;
    /* 0x1412C */ NesApuChannel apu[4];
    /* 0x141BC */ s32 frameCount;
} Nes; /* size 0x141C0 */

#define NES ((Nes *)0x80380000)

/* Excitebike's design-mode track in NES RAM, which the Famicom version saved to the Data Recorder: the emulator copies
   it to and from the save block (gSaveNesTrack). */
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
