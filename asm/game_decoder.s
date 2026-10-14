# game_decoder.s -- DECODER.C's ADPCM block decoders and their tables (hand-written asm)
# image: game
# vram: 0x800DBD94-0x800DC098 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# Linked among the game's initialized data, after game_softmix.s; the last bytes of the game image. decoder.c
# decodes a "SAM" sample one chunk at a time through these and compands the 16-bit output to 8 bits
# (sDecoderCompandTable):
#   decoder_decode_ima_block(u8 *src, u16 *dst, s32 count)  IMA ADPCM, for decoder_decode_ima_adpcm (sample type 3): a 4-byte
#       header {s16 first sample (little-endian), u8 step index, pad}, then 4-bit codes, low nibble first; count
#       samples out, the header's included.
#   decoder_decode_msadpcm_block(u8 *src, u16 *dst, s32 count, u8 *state)  Microsoft ADPCM, for
#       decoder_decode_adpcm (sample types 4 and 5): a 2-byte little-endian header, the coefficient pair in bits 0-2
#       and the initial delta in the others, then signed 4-bit codes, high nibble first; the two previous samples
#       (s16 state[2]) carry over from block to block. The delta doesn't go below 8.
# The tables before them (read only by these routines) are the standard IMA ADPCM index changes and step sizes and
# the Microsoft ADPCM coefficient pairs and adaptation factors.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

# IMA ADPCM step index change, by 4-bit code
sDecoderImaIndexTable:
    /* 800DBD94 */  .byte -1, -1, -1, -1, 2, 4, 6, 8
    /* 800DBD9C */  .byte -1, -1, -1, -1, 2, 4, 6, 8

# IMA ADPCM step size, by step index (0-88)
sDecoderImaStepTable:
    /* 800DBDA4 */  .half 7, 8, 9, 10, 11, 12, 13, 14
    /* 800DBDB4 */  .half 16, 17, 19, 21, 23, 25, 28, 31
    /* 800DBDC4 */  .half 34, 37, 41, 45, 50, 55, 60, 66
    /* 800DBDD4 */  .half 73, 80, 88, 97, 107, 118, 130, 143
    /* 800DBDE4 */  .half 157, 173, 190, 209, 230, 253, 279, 307
    /* 800DBDF4 */  .half 337, 371, 408, 449, 494, 544, 598, 658
    /* 800DBE04 */  .half 724, 796, 876, 963, 1060, 1166, 1282, 1411
    /* 800DBE14 */  .half 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024
    /* 800DBE24 */  .half 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484
    /* 800DBE34 */  .half 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899
    /* 800DBE44 */  .half 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794
    /* 800DBE54 */  .half 32767
    /* 800DBE56 */  .half 0  # pad to the next word

# Microsoft ADPCM coefficient pairs (/ 256), by the header's predictor
sDecoderMsAdpcmCoefs:
    /* 800DBE58 */  .half 256, 0
    /* 800DBE5C */  .half 512, -256
    /* 800DBE60 */  .half 0, 0
    /* 800DBE64 */  .half 192, 64
    /* 800DBE68 */  .half 240, 0
    /* 800DBE6C */  .half 460, -208
    /* 800DBE70 */  .half 392, -232

# Microsoft ADPCM delta adaptation (/ 256), by 4-bit code
sDecoderMsAdpcmAdaptTable:
    /* 800DBE74 */  .half 230, 230, 230, 230, 307, 409, 512, 614
    /* 800DBE84 */  .half 768, 614, 512, 409, 307, 230, 230, 230

# (u8 *src, u16 *dst, s32 count): one IMA ADPCM block to count s16 samples
.global decoder_decode_ima_block
decoder_decode_ima_block:
    /* 800DBE94 */  lui         $t0, %hi(sDecoderImaIndexTable)
    /* 800DBE98 */  addiu       $t0, $t0, %lo(sDecoderImaIndexTable)
    /* 800DBE9C */  lui         $t1, %hi(sDecoderImaStepTable)
    /* 800DBEA0 */  addiu       $t1, $t1, %lo(sDecoderImaStepTable)
    /* 800DBEA4 */  lbu         $t2, 0x2($a0)
    /* 800DBEA8 */  lb          $v1, 0x1($a0)
    /* 800DBEAC */  lbu         $v0, 0x0($a0)
    /* 800DBEB0 */  addi        $a0, $a0, 0x4
    /* 800DBEB4 */  sll         $t5, $v1, 8
    /* 800DBEB8 */  add         $t5, $t5, $v0
    /* 800DBEBC */  addu        $v0, $t2, $t2
    /* 800DBEC0 */  addu        $v0, $v0, $t1
    /* 800DBEC4 */  lhu         $t3, 0x0($v0)
    /* 800DBEC8 */  sh          $t5, 0x0($a1)
    /* 800DBECC */  addi        $a1, $a1, 0x2
    /* 800DBED0 */  addi        $a2, $a2, -0x1
    /* 800DBED4 */  beq         $a2, $zero, .L800DBF8C
    /* 800DBED8 */   add         $t7, $zero, $zero
    /* 800DBEDC */  lbu         $t4, 0x0($a0)
.L800DBEE0:
    /* 800DBEE0 */  beq         $t7, $zero, .L800DBEF0
    /* 800DBEE4 */   xori        $t7, $t7, 0x1
    /* 800DBEE8 */  addi        $a0, $a0, 0x1
    /* 800DBEEC */  srl         $t4, $t4, 4
.L800DBEF0:
    /* 800DBEF0 */  andi        $t4, $t4, 0xF
    /* 800DBEF4 */  addi        $a2, $a2, -0x1
    /* 800DBEF8 */  add         $v0, $t0, $t4
    /* 800DBEFC */  lb          $v0, 0x0($v0)
    /* 800DBF00 */  add         $t2, $t2, $v0
    /* 800DBF04 */  bltzl       $t2, .L800DBF18
    /* 800DBF08 */   addi        $t2, $zero, 0x0
    /* 800DBF0C */  slti        $v0, $t2, 0x58
    /* 800DBF10 */  beql        $v0, $zero, .L800DBF18
    /* 800DBF14 */   addi        $t2, $zero, 0x58
.L800DBF18:
    /* 800DBF18 */  srl         $t6, $t3, 3
    /* 800DBF1C */  andi        $v0, $t4, 0x4
    /* 800DBF20 */  bnel        $v0, $zero, .L800DBF28
    /* 800DBF24 */   add         $t6, $t6, $t3
.L800DBF28:
    /* 800DBF28 */  srl         $t3, $t3, 1
    /* 800DBF2C */  andi        $v0, $t4, 0x2
    /* 800DBF30 */  bnel        $v0, $zero, .L800DBF38
    /* 800DBF34 */   add         $t6, $t6, $t3
.L800DBF38:
    /* 800DBF38 */  srl         $t3, $t3, 1
    /* 800DBF3C */  andi        $v0, $t4, 0x1
    /* 800DBF40 */  bnel        $v0, $zero, .L800DBF48
    /* 800DBF44 */   add         $t6, $t6, $t3
.L800DBF48:
    /* 800DBF48 */  andi        $v0, $t4, 0x8
    /* 800DBF4C */  bnel        $v0, $zero, .L800DBF54
    /* 800DBF50 */   sub         $t6, $zero, $t6
.L800DBF54:
    /* 800DBF54 */  add         $t5, $t5, $t6
    /* 800DBF58 */  slti        $v0, $t5, 0x7FFF
    /* 800DBF5C */  beql        $v0, $zero, .L800DBF70
    /* 800DBF60 */   addi        $t5, $zero, 0x7FFF
    /* 800DBF64 */  slti        $v0, $t5, -0x8000
    /* 800DBF68 */  bnel        $v0, $zero, .L800DBF70
    /* 800DBF6C */   addi        $t5, $zero, -0x8000
.L800DBF70:
    /* 800DBF70 */  addu        $v0, $t2, $t2
    /* 800DBF74 */  addu        $v0, $v0, $t1
    /* 800DBF78 */  lhu         $t3, 0x0($v0)
    /* 800DBF7C */  sh          $t5, 0x0($a1)
    /* 800DBF80 */  addi        $a1, $a1, 0x2
    /* 800DBF84 */  bne         $a2, $zero, .L800DBEE0
    /* 800DBF88 */   lbu         $t4, 0x0($a0)
.L800DBF8C:
    /* 800DBF8C */  jr          $ra
    /* 800DBF90 */   nop

# (u8 *src, u16 *dst, s32 count, u8 *state): one Microsoft ADPCM block to count s16 samples
.global decoder_decode_msadpcm_block
decoder_decode_msadpcm_block:
    /* 800DBF94 */  lui         $t0, %hi(sDecoderMsAdpcmCoefs)
    /* 800DBF98 */  addiu       $t0, $t0, %lo(sDecoderMsAdpcmCoefs)
    /* 800DBF9C */  lui         $t8, %hi(sDecoderMsAdpcmAdaptTable)
    /* 800DBFA0 */  addiu       $t8, $t8, %lo(sDecoderMsAdpcmAdaptTable)
    /* 800DBFA4 */  lbu         $v1, 0x1($a0)
    /* 800DBFA8 */  lbu         $v0, 0x0($a0)
    /* 800DBFAC */  sll         $t5, $v1, 8
    /* 800DBFB0 */  add         $t5, $t5, $v0
    /* 800DBFB4 */  addiu       $a0, $a0, 0x2
    /* 800DBFB8 */  andi        $t6, $t5, 0x7
    /* 800DBFBC */  addiu       $at, $zero, -0x8
    /* 800DBFC0 */  and         $t5, $t5, $at
    /* 800DBFC4 */  lh          $t2, 0x0($a3)
    /* 800DBFC8 */  lh          $t3, 0x2($a3)
    /* 800DBFCC */  sll         $t6, $t6, 2
    /* 800DBFD0 */  add         $t6, $t6, $t0
    /* 800DBFD4 */  lh          $t0, 0x0($t6)
    /* 800DBFD8 */  lh          $t1, 0x2($t6)
    /* 800DBFDC */  mult        $t0, $t2
    /* 800DBFE0 */  beq         $a2, $zero, .L800DC088
    /* 800DBFE4 */   add         $t7, $zero, $zero
    /* 800DBFE8 */  lb          $t4, 0x0($a0)
.L800DBFEC:
    /* 800DBFEC */  beq         $t7, $zero, .L800DC000
    /* 800DBFF0 */   xori        $t7, $t7, 0x1
    /* 800DBFF4 */  addi        $a0, $a0, 0x1
    /* 800DBFF8 */  sll         $t4, $t4, 28
    /* 800DBFFC */  sra         $t4, $t4, 24
.L800DC000:
    /* 800DC000 */  mflo        $v0
    /* 800DC004 */  addi        $a2, $a2, -0x1
    /* 800DC008 */  addi        $a1, $a1, 0x2
    /* 800DC00C */  mult        $t1, $t3
    /* 800DC010 */  sra         $t4, $t4, 4
    /* 800DC014 */  nop
    /* 800DC018 */  mflo        $v1
    /* 800DC01C */  add         $v0, $v0, $v1
    /* 800DC020 */  sra         $v0, $v0, 8
    /* 800DC024 */  mult        $t4, $t5
    /* 800DC028 */  andi        $t4, $t4, 0xF
    /* 800DC02C */  sll         $t4, $t4, 1
    /* 800DC030 */  addu        $t4, $t4, $t8
    /* 800DC034 */  lhu         $t4, 0x0($t4)
    /* 800DC038 */  mflo        $v1
    /* 800DC03C */  add         $t6, $v0, $v1
    /* 800DC040 */  addu        $t3, $t2, $zero
    /* 800DC044 */  multu       $t4, $t5
    /* 800DC048 */  slti        $v0, $t6, 0x7FFF
    /* 800DC04C */  beql        $v0, $zero, .L800DC060
    /* 800DC050 */   addi        $t6, $zero, 0x7FFF
    /* 800DC054 */  slti        $v0, $t6, -0x8000
    /* 800DC058 */  bnel        $v0, $zero, .L800DC060
    /* 800DC05C */   addi        $t6, $zero, -0x8000
.L800DC060:
    /* 800DC060 */  sh          $t6, -0x2($a1)
    /* 800DC064 */  addu        $t2, $t6, $zero
    /* 800DC068 */  mflo        $t5
    /* 800DC06C */  srl         $t5, $t5, 8
    /* 800DC070 */  slti        $v0, $t5, 0x8
    /* 800DC074 */  mult        $t0, $t2
    /* 800DC078 */  bnel        $v0, $zero, .L800DC080
    /* 800DC07C */   addi        $t5, $zero, 0x8
.L800DC080:
    /* 800DC080 */  bne         $a2, $zero, .L800DBFEC
    /* 800DC084 */   lb          $t4, 0x0($a0)
.L800DC088:
    /* 800DC088 */  sh          $t2, 0x0($a3)
    /* 800DC08C */  sh          $t3, 0x2($a3)
    /* 800DC090 */  jr          $ra
    /* 800DC094 */   nop
