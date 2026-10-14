# game_tunes.s -- tunes.c's ADPCM block decoder and its tables (hand-written asm)
# image: game
# vram: 0x800DB550-0x800DB8B8 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# Linked among the game's initialized data, after fontcode.c's and just before the mixer state of game_softmix.s.
# tunes_mix_streams decodes each 0x220-byte DMA of a stream channel as 8 blocks of 0x44 bytes, 128 samples each:
#   tunes_decode_adpcm_block(u8 *src, s16 *dst, s32 count)
# A block has a 5-byte header {s16 sample 0, s16 sample 1 (big-endian), u8 step index}, the predictor in sample 0's
# low 2 bits (cleared in the output), then signed 4-bit codes, high nibble first; count samples out, the header's
# two included (0x44 bytes: 2 + 126 samples). Each code gives sample = prediction + code * step, clamped to s16,
# from the last two samples s1 (the latest) and s2:
#   predictor 0: 0   1: s1   2: (3 * s1 - s2) >> 1   3: 2 * s1 - s2
# The step is IMA ADPCM's step size at the step index, which the code's magnitude moves (0-3 down 1, 4-8 up 2, 4,
# 6, 8, 10) and which doesn't go below 0. Nothing caps it above: the step table has 7 more copies of 32767 after
# IMA's 89 entries. The routine keeps the index doubled (the step table's byte offset), and so does the index table.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

# step index change (x2), by 4-bit code: read at +8 with the signed code (-8..7); the last 8 bytes, the changes
# for codes 8-15 read unsigned, repeat the first 8
sTunesAdpcmIndexTable:
    /* 800DB550 */  .byte 20, 16, 12, 8, 4, -2, -2, -2
    /* 800DB558 */  .byte -2, -2, -2, -2, 4, 8, 12, 16
    /* 800DB560 */  .byte 20, 16, 12, 8, 4, -2, -2, -2

# step size, by step index (IMA ADPCM's 0-88, then 89-95)
sTunesAdpcmStepTable:
    /* 800DB568 */  .half 7, 8, 9, 10, 11, 12, 13, 14
    /* 800DB578 */  .half 16, 17, 19, 21, 23, 25, 28, 31
    /* 800DB588 */  .half 34, 37, 41, 45, 50, 55, 60, 66
    /* 800DB598 */  .half 73, 80, 88, 97, 107, 118, 130, 143
    /* 800DB5A8 */  .half 157, 173, 190, 209, 230, 253, 279, 307
    /* 800DB5B8 */  .half 337, 371, 408, 449, 494, 544, 598, 658
    /* 800DB5C8 */  .half 724, 796, 876, 963, 1060, 1166, 1282, 1411
    /* 800DB5D8 */  .half 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024
    /* 800DB5E8 */  .half 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484
    /* 800DB5F8 */  .half 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899
    /* 800DB608 */  .half 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794
    /* 800DB618 */  .half 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767

# (u8 *src, s16 *dst, s32 count): one block to count s16 samples
.global tunes_decode_adpcm_block
tunes_decode_adpcm_block:
    /* 800DB628 */  lui         $t6, %hi(sTunesAdpcmIndexTable + 8)
    /* 800DB62C */  addiu       $t6, $t6, %lo(sTunesAdpcmIndexTable + 8)
    /* 800DB630 */  lui         $t7, %hi(sTunesAdpcmStepTable)
    /* 800DB634 */  addiu       $t7, $t7, %lo(sTunesAdpcmStepTable)
    /* 800DB638 */  lh          $t1, 0x0($a0)
    /* 800DB63C */  lh          $t0, 0x2($a0)
    /* 800DB640 */  lbu         $t2, 0x4($a0)
    /* 800DB644 */  addiu       $a0, $a0, 0x5
    /* 800DB648 */  addu        $t2, $t2, $t2
    /* 800DB64C */  andi        $t5, $t1, 0x3
    /* 800DB650 */  addiu       $at, $zero, -0x4
    /* 800DB654 */  and         $t1, $t1, $at
    /* 800DB658 */  addi        $a2, $a2, -0x1
    /* 800DB65C */  beq         $a2, $zero, .L800DB6AC
    /* 800DB660 */   sh          $t1, 0x0($a1)
    /* 800DB664 */  addi        $a2, $a2, -0x1
    /* 800DB668 */  beq         $a2, $zero, .L800DB6AC
    /* 800DB66C */   sh          $t0, 0x2($a1)
    /* 800DB670 */  addiu       $a1, $a1, 0x4
    /* 800DB674 */  lui         $v0, %hi(.L800DB68C)
    /* 800DB678 */  addiu       $v0, $v0, %lo(.L800DB68C)
    /* 800DB67C */  sll         $t5, $t5, 3
    /* 800DB680 */  add         $v0, $v0, $t5
    /* 800DB684 */  jr          $v0
    /* 800DB688 */   add         $t5, $zero, $zero

# by predictor: load the first code byte and enter that predictor's loop
.L800DB68C:
    /* 800DB68C */  j           .L800DB6B4
    /* 800DB690 */   lb          $t4, 0x0($a0)
    /* 800DB694 */  j           .L800DB730
    /* 800DB698 */   lb          $t4, 0x0($a0)
    /* 800DB69C */  j           .L800DB7AC
    /* 800DB6A0 */   lb          $t4, 0x0($a0)
    /* 800DB6A4 */  j           .L800DB834
    /* 800DB6A8 */   lb          $t4, 0x0($a0)

# count was 1 or 2: the header's samples only
.L800DB6AC:
    /* 800DB6AC */  jr          $ra
    /* 800DB6B0 */   nop

# predictor 0: code * step
.L800DB6B4:
    /* 800DB6B4 */  beq         $t5, $zero, .L800DB6C8
    /* 800DB6B8 */   xori        $t5, $t5, 0x1
    /* 800DB6BC */  addi        $a0, $a0, 0x1
    /* 800DB6C0 */  sll         $t4, $t4, 28
    /* 800DB6C4 */  sra         $t4, $t4, 24
.L800DB6C8:
    /* 800DB6C8 */  add         $v0, $t7, $t2
    /* 800DB6CC */  lhu         $t3, 0x0($v0)
    /* 800DB6D0 */  sra         $t4, $t4, 4
    /* 800DB6D4 */  mult        $t4, $t3
    /* 800DB6D8 */  add         $v0, $zero, $zero
    /* 800DB6DC */  mflo        $v1
    /* 800DB6E0 */  add         $t1, $t0, $zero
    /* 800DB6E4 */  add         $t0, $v0, $v1
    /* 800DB6E8 */  slti        $v0, $t0, 0x7FFF
    /* 800DB6EC */  beql        $v0, $zero, .L800DB700
    /* 800DB6F0 */   addi        $t0, $zero, 0x7FFF
    /* 800DB6F4 */  slti        $v0, $t0, -0x8000
    /* 800DB6F8 */  bnel        $v0, $zero, .L800DB700
    /* 800DB6FC */   addi        $t0, $zero, -0x8000
.L800DB700:
    /* 800DB700 */  sh          $t0, 0x0($a1)
    /* 800DB704 */  add         $v0, $t6, $t4
    /* 800DB708 */  lb          $v0, 0x0($v0)
    /* 800DB70C */  addiu       $a1, $a1, 0x2
    /* 800DB710 */  add         $t2, $t2, $v0
    /* 800DB714 */  bltzl       $t2, .L800DB71C
    /* 800DB718 */   addi        $t2, $zero, 0x0
.L800DB71C:
    /* 800DB71C */  addi        $a2, $a2, -0x1
    /* 800DB720 */  bne         $a2, $zero, .L800DB6B4
    /* 800DB724 */   lb          $t4, 0x0($a0)
    /* 800DB728 */  jr          $ra
    /* 800DB72C */   nop

# predictor 1: s1 + code * step
.L800DB730:
    /* 800DB730 */  beq         $t5, $zero, .L800DB744
    /* 800DB734 */   xori        $t5, $t5, 0x1
    /* 800DB738 */  addi        $a0, $a0, 0x1
    /* 800DB73C */  sll         $t4, $t4, 28
    /* 800DB740 */  sra         $t4, $t4, 24
.L800DB744:
    /* 800DB744 */  add         $v0, $t7, $t2
    /* 800DB748 */  lhu         $t3, 0x0($v0)
    /* 800DB74C */  sra         $t4, $t4, 4
    /* 800DB750 */  mult        $t4, $t3
    /* 800DB754 */  add         $v0, $t0, $zero
    /* 800DB758 */  mflo        $v1
    /* 800DB75C */  add         $t1, $t0, $zero
    /* 800DB760 */  add         $t0, $v0, $v1
    /* 800DB764 */  slti        $v0, $t0, 0x7FFF
    /* 800DB768 */  beql        $v0, $zero, .L800DB77C
    /* 800DB76C */   addi        $t0, $zero, 0x7FFF
    /* 800DB770 */  slti        $v0, $t0, -0x8000
    /* 800DB774 */  bnel        $v0, $zero, .L800DB77C
    /* 800DB778 */   addi        $t0, $zero, -0x8000
.L800DB77C:
    /* 800DB77C */  sh          $t0, 0x0($a1)
    /* 800DB780 */  add         $v0, $t6, $t4
    /* 800DB784 */  lb          $v0, 0x0($v0)
    /* 800DB788 */  addiu       $a1, $a1, 0x2
    /* 800DB78C */  add         $t2, $t2, $v0
    /* 800DB790 */  bltzl       $t2, .L800DB798
    /* 800DB794 */   addi        $t2, $zero, 0x0
.L800DB798:
    /* 800DB798 */  addi        $a2, $a2, -0x1
    /* 800DB79C */  bne         $a2, $zero, .L800DB730
    /* 800DB7A0 */   lb          $t4, 0x0($a0)
    /* 800DB7A4 */  jr          $ra
    /* 800DB7A8 */   nop

# predictor 2: ((3 * s1 - s2) >> 1) + code * step
.L800DB7AC:
    /* 800DB7AC */  beq         $t5, $zero, .L800DB7C0
    /* 800DB7B0 */   xori        $t5, $t5, 0x1
    /* 800DB7B4 */  addi        $a0, $a0, 0x1
    /* 800DB7B8 */  sll         $t4, $t4, 28
    /* 800DB7BC */  sra         $t4, $t4, 24
.L800DB7C0:
    /* 800DB7C0 */  add         $v0, $t7, $t2
    /* 800DB7C4 */  lhu         $t3, 0x0($v0)
    /* 800DB7C8 */  sra         $t4, $t4, 4
    /* 800DB7CC */  mult        $t4, $t3
    /* 800DB7D0 */  add         $v0, $t0, $t0
    /* 800DB7D4 */  add         $v0, $v0, $t0
    /* 800DB7D8 */  sub         $v0, $v0, $t1
    /* 800DB7DC */  sra         $v0, $v0, 1
    /* 800DB7E0 */  mflo        $v1
    /* 800DB7E4 */  add         $t1, $t0, $zero
    /* 800DB7E8 */  add         $t0, $v0, $v1
    /* 800DB7EC */  slti        $v0, $t0, 0x7FFF
    /* 800DB7F0 */  beql        $v0, $zero, .L800DB804
    /* 800DB7F4 */   addi        $t0, $zero, 0x7FFF
    /* 800DB7F8 */  slti        $v0, $t0, -0x8000
    /* 800DB7FC */  bnel        $v0, $zero, .L800DB804
    /* 800DB800 */   addi        $t0, $zero, -0x8000
.L800DB804:
    /* 800DB804 */  sh          $t0, 0x0($a1)
    /* 800DB808 */  add         $v0, $t6, $t4
    /* 800DB80C */  lb          $v0, 0x0($v0)
    /* 800DB810 */  addiu       $a1, $a1, 0x2
    /* 800DB814 */  add         $t2, $t2, $v0
    /* 800DB818 */  bltzl       $t2, .L800DB820
    /* 800DB81C */   addi        $t2, $zero, 0x0
.L800DB820:
    /* 800DB820 */  addi        $a2, $a2, -0x1
    /* 800DB824 */  bne         $a2, $zero, .L800DB7AC
    /* 800DB828 */   lb          $t4, 0x0($a0)
    /* 800DB82C */  jr          $ra
    /* 800DB830 */   nop

# predictor 3: 2 * s1 - s2 + code * step
.L800DB834:
    /* 800DB834 */  beq         $t5, $zero, .L800DB848
    /* 800DB838 */   xori        $t5, $t5, 0x1
    /* 800DB83C */  addi        $a0, $a0, 0x1
    /* 800DB840 */  sll         $t4, $t4, 28
    /* 800DB844 */  sra         $t4, $t4, 24
.L800DB848:
    /* 800DB848 */  add         $v0, $t7, $t2
    /* 800DB84C */  lhu         $t3, 0x0($v0)
    /* 800DB850 */  sra         $t4, $t4, 4
    /* 800DB854 */  mult        $t4, $t3
    /* 800DB858 */  add         $v0, $t0, $t0
    /* 800DB85C */  sub         $v0, $v0, $t1
    /* 800DB860 */  mflo        $v1
    /* 800DB864 */  add         $t1, $t0, $zero
    /* 800DB868 */  add         $t0, $v0, $v1
    /* 800DB86C */  slti        $v0, $t0, 0x7FFF
    /* 800DB870 */  beql        $v0, $zero, .L800DB884
    /* 800DB874 */   addi        $t0, $zero, 0x7FFF
    /* 800DB878 */  slti        $v0, $t0, -0x8000
    /* 800DB87C */  bnel        $v0, $zero, .L800DB884
    /* 800DB880 */   addi        $t0, $zero, -0x8000
.L800DB884:
    /* 800DB884 */  sh          $t0, 0x0($a1)
    /* 800DB888 */  add         $v0, $t6, $t4
    /* 800DB88C */  lb          $v0, 0x0($v0)
    /* 800DB890 */  addiu       $a1, $a1, 0x2
    /* 800DB894 */  add         $t2, $t2, $v0
    /* 800DB898 */  bltzl       $t2, .L800DB8A0
    /* 800DB89C */   addi        $t2, $zero, 0x0
.L800DB8A0:
    /* 800DB8A0 */  addi        $a2, $a2, -0x1
    /* 800DB8A4 */  bne         $a2, $zero, .L800DB834
    /* 800DB8A8 */   lb          $t4, 0x0($a0)
    /* 800DB8AC */  jr          $ra
    /* 800DB8B0 */   nop

# a zero word; game_softmix.s's mixers address their state from here (MIX_VARS)
    /* 800DB8B4 */  .word 0
