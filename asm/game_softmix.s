# game_softmix.s -- softmix.c's voice mixers and output converters (hand-written asm)
# image: game
# vram: 0x800DB8E0-0x800DBD94 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# The inner loops of the software mixer (src/game/softmix.c), written in assembly and linked among the game's
# initialized data, right after the mixer state they use. softmix_mix calls softmix_mix_voices_mono or
# softmix_mix_voices_stereo, softmix_convert_output calls softmix_output_8bit or softmix_output_16bit. None of them
# takes arguments: softmix.c sets the state (src/data: sMixVoices 0x800DB8B8 ... sMixOverwrite 0x800DB8DD) and each
# routine reads it through one base register, 4 bytes before sMixVoices (MIX_VARS).
#
# Voice (0x30 bytes, sMixNumVoices of them at sMixVoices): +0x00 8-bit sample data, +0x04 flags (1 playing,
# 2 looped), +0x08 position (64-bit, 32.32 fixed point), +0x10 step (64-bit), +0x18 end, +0x1C loop start,
# +0x20 volume (mono), +0x22 / +0x23 left / right volume (stereo), +0x24 loop end. At its end a voice goes back to
# the loop start with the loop end as its new end (flag 2), or stops (flag 1 cleared) if loop start == loop end.
# Mixing: each sample byte indexes the voice's row of sMixVolumeTable (32 rows of 256 s32, one per volume / 8) and
# the value is added to the s32 mix buffer sMixSrc (sMixCount frames; left, right in stereo). With sMixOverwrite
# set (softmix_mix always sets it) the first playing voice stores instead of adding, and the buffer is cleared
# when no voice plays.
# Output: (mix + sMixBias) >> sMixShift for each pair of s32 at sMixSrc (sMixCount values) to sMixDst. 16-bit
# clamps both to [sMixClampMin, sMixClampMax] and stores them as two s16; 8-bit stores the first one's low byte
# as the high byte of both s16 of the output frame (bytes 0 and 2) and doesn't clamp.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

# Base of the mixer state, loaded with lui/ori: the upper half is the address's own top 16 bits (0x800D for
# 0x800DB8B4), not %hi's, which rounds up when bit 15 of the lower half is set.
.set MIX_VARS, sMixVoices - 4

# mono: one s32 per frame, the voice volume at +0x20
.global softmix_mix_voices_mono
softmix_mix_voices_mono:
    /* 800DB8E0 */  addi        $sp, $sp, -0x8
    /* 800DB8E4 */  sd          $s0, 0x0($sp)
    /* 800DB8E8 */  lui         $a0, (MIX_VARS >> 16)
    /* 800DB8EC */  ori         $a0, $a0, (MIX_VARS & 0xFFFF)
    /* 800DB8F0 */  lb          $v0, (sMixOverwrite - MIX_VARS)($a0)
    /* 800DB8F4 */  lw          $a1, (sMixVoices - MIX_VARS)($a0)
    /* 800DB8F8 */  lw          $a2, (sMixNumVoices - MIX_VARS)($a0)
    /* 800DB8FC */  bne         $v0, $zero, .L800DB908
    /* 800DB900 */   ori         $a3, $zero, 0x1
    /* 800DB904 */  ori         $a3, $zero, 0x0
.L800DB908:
    /* 800DB908 */  lw          $v0, 0x4($a1)
    /* 800DB90C */  nop
    /* 800DB910 */  andi        $v0, $v0, 0x1
    /* 800DB914 */  beq         $v0, $zero, .L800DBA6C
    /* 800DB918 */   lw          $t0, (sMixSrc - MIX_VARS)($a0)
    /* 800DB91C */  lbu         $v1, 0x20($a1)
    /* 800DB920 */  lw          $t1, (sMixVolumeTable - MIX_VARS)($a0)
    /* 800DB924 */  srl         $v1, $v1, 3
    /* 800DB928 */  sll         $v1, $v1, 10
    /* 800DB92C */  add         $t1, $t1, $v1
    /* 800DB930 */  ld          $t3, 0x10($a1)
    /* 800DB934 */  ld          $t4, 0x8($a1)
    /* 800DB938 */  lw          $t5, 0x18($a1)
    /* 800DB93C */  lw          $t6, 0x1C($a1)
    /* 800DB940 */  lw          $t7, 0x24($a1)
    /* 800DB944 */  lw          $t8, 0x0($a1)
    /* 800DB948 */  beq         $a3, $zero, .L800DB9E4
    /* 800DB94C */   lw          $t9, (sMixCount - MIX_VARS)($a0)
.L800DB950:
    /* 800DB950 */  dsra32      $v0, $t4, 0
    /* 800DB954 */  sltu        $v1, $v0, $t5
    /* 800DB958 */  beq         $v1, $zero, .L800DB98C
    /* 800DB95C */   addu        $v0, $v0, $t8
    /* 800DB960 */  lbu         $v0, 0x0($v0)
    /* 800DB964 */  daddu       $t4, $t4, $t3
    /* 800DB968 */  sll         $v0, $v0, 2
    /* 800DB96C */  addu        $v1, $v0, $t1
    /* 800DB970 */  lw          $v1, 0x0($v1)
    /* 800DB974 */  addi        $t9, $t9, -0x1
    /* 800DB978 */  sw          $v1, 0x0($t0)
    /* 800DB97C */  bne         $t9, $zero, .L800DB950
    /* 800DB980 */   addiu       $t0, $t0, 0x4
    /* 800DB984 */  j           .L800DBA68
    /* 800DB988 */   sd          $t4, 0x8($a1)
.L800DB98C:
    /* 800DB98C */  beq         $t6, $t7, .L800DB9BC
    /* 800DB990 */   lw          $v0, 0x4($a1)
    /* 800DB994 */  dsll32      $t4, $t4, 0
    /* 800DB998 */  dsrl32      $t4, $t4, 0
    /* 800DB99C */  dsll32      $v1, $t6, 0
    /* 800DB9A0 */  or          $t4, $t4, $v1
    /* 800DB9A4 */  lw          $at, 0x24($a1)
    /* 800DB9A8 */  or          $t5, $t7, $zero
    /* 800DB9AC */  sw          $at, 0x18($a1)
    /* 800DB9B0 */  ori         $v0, $v0, 0x2
    /* 800DB9B4 */  j           .L800DB950
    /* 800DB9B8 */   sw          $v0, 0x4($a1)
.L800DB9BC:
    /* 800DB9BC */  xori        $v0, $v0, 0x1
    /* 800DB9C0 */  sw          $v0, 0x4($a1)
    /* 800DB9C4 */  beq         $t9, $zero, .L800DBA68
    /* 800DB9C8 */   sd          $t4, 0x8($a1)
.L800DB9CC:
    /* 800DB9CC */  addi        $t9, $t9, -0x1
    /* 800DB9D0 */  sw          $zero, 0x0($t0)
    /* 800DB9D4 */  bne         $t9, $zero, .L800DB9CC
    /* 800DB9D8 */   addiu       $t0, $t0, 0x4
    /* 800DB9DC */  j           .L800DBA68
    /* 800DB9E0 */   nop
.L800DB9E4:
    /* 800DB9E4 */  dsra32      $v0, $t4, 0
    /* 800DB9E8 */  sltu        $v1, $v0, $t5
    /* 800DB9EC */  beq         $v1, $zero, .L800DBA28
    /* 800DB9F0 */   addu        $v0, $v0, $t8
    /* 800DB9F4 */  lbu         $v0, 0x0($v0)
    /* 800DB9F8 */  daddu       $t4, $t4, $t3
    /* 800DB9FC */  lw          $s0, 0x0($t0)
    /* 800DBA00 */  sll         $v0, $v0, 2
    /* 800DBA04 */  addu        $v0, $v0, $t1
    /* 800DBA08 */  lw          $v0, 0x0($v0)
    /* 800DBA0C */  addi        $t9, $t9, -0x1
    /* 800DBA10 */  addu        $v0, $v0, $s0
    /* 800DBA14 */  sw          $v0, 0x0($t0)
    /* 800DBA18 */  bne         $t9, $zero, .L800DB9E4
    /* 800DBA1C */   addiu       $t0, $t0, 0x4
    /* 800DBA20 */  j           .L800DBA68
    /* 800DBA24 */   sd          $t4, 0x8($a1)
.L800DBA28:
    /* 800DBA28 */  beq         $t6, $t7, .L800DBA58
    /* 800DBA2C */   lw          $v0, 0x4($a1)
    /* 800DBA30 */  dsll32      $t4, $t4, 0
    /* 800DBA34 */  dsrl32      $t4, $t4, 0
    /* 800DBA38 */  dsll32      $v1, $t6, 0
    /* 800DBA3C */  or          $t4, $t4, $v1
    /* 800DBA40 */  lw          $at, 0x24($a1)
    /* 800DBA44 */  or          $t5, $t7, $zero
    /* 800DBA48 */  sw          $at, 0x18($a1)
    /* 800DBA4C */  ori         $v0, $v0, 0x2
    /* 800DBA50 */  j           .L800DB9E4
    /* 800DBA54 */   sw          $v0, 0x4($a1)
.L800DBA58:
    /* 800DBA58 */  xori        $v0, $v0, 0x1
    /* 800DBA5C */  sw          $v0, 0x4($a1)
    /* 800DBA60 */  j           .L800DBA68
    /* 800DBA64 */   sd          $t4, 0x8($a1)
.L800DBA68:
    /* 800DBA68 */  ori         $a3, $zero, 0x0
.L800DBA6C:
    /* 800DBA6C */  addi        $a2, $a2, -0x1
    /* 800DBA70 */  bne         $a2, $zero, .L800DB908
    /* 800DBA74 */   addi        $a1, $a1, 0x30
    /* 800DBA78 */  lw          $t9, (sMixCount - MIX_VARS)($a0)
    /* 800DBA7C */  lw          $t0, (sMixSrc - MIX_VARS)($a0)
    /* 800DBA80 */  beq         $a3, $zero, .L800DBA94
.L800DBA84:
    /* 800DBA84 */   addi        $t9, $t9, -0x1
    /* 800DBA88 */  sw          $zero, 0x0($t0)
    /* 800DBA8C */  bne         $t9, $zero, .L800DBA84
    /* 800DBA90 */   addiu       $t0, $t0, 0x4
.L800DBA94:
    /* 800DBA94 */  ld          $s0, 0x0($sp)
    /* 800DBA98 */  jr          $ra
    /* 800DBA9C */   addi        $sp, $sp, 0x8

# stereo: two s32 per frame (left, right), the voice volumes at +0x22 / +0x23
.global softmix_mix_voices_stereo
softmix_mix_voices_stereo:
    /* 800DBAA0 */  addi        $sp, $sp, -0x10
    /* 800DBAA4 */  sd          $s0, 0x0($sp)
    /* 800DBAA8 */  sd          $s1, 0x8($sp)
    /* 800DBAAC */  lui         $a0, (MIX_VARS >> 16)
    /* 800DBAB0 */  ori         $a0, $a0, (MIX_VARS & 0xFFFF)
    /* 800DBAB4 */  lb          $v0, (sMixOverwrite - MIX_VARS)($a0)
    /* 800DBAB8 */  lw          $a1, (sMixVoices - MIX_VARS)($a0)
    /* 800DBABC */  lw          $a2, (sMixNumVoices - MIX_VARS)($a0)
    /* 800DBAC0 */  bne         $v0, $zero, .L800DBACC
    /* 800DBAC4 */   ori         $a3, $zero, 0x1
    /* 800DBAC8 */  ori         $a3, $zero, 0x0
.L800DBACC:
    /* 800DBACC */  lw          $v0, 0x4($a1)
    /* 800DBAD0 */  nop
    /* 800DBAD4 */  andi        $v0, $v0, 0x1
    /* 800DBAD8 */  beq         $v0, $zero, .L800DBC68
    /* 800DBADC */   lw          $t0, (sMixSrc - MIX_VARS)($a0)
    /* 800DBAE0 */  lbu         $v1, 0x22($a1)
    /* 800DBAE4 */  lw          $t1, (sMixVolumeTable - MIX_VARS)($a0)
    /* 800DBAE8 */  srl         $v1, $v1, 3
    /* 800DBAEC */  sll         $v1, $v1, 10
    /* 800DBAF0 */  add         $t1, $t1, $v1
    /* 800DBAF4 */  lbu         $v1, 0x23($a1)
    /* 800DBAF8 */  lw          $t2, (sMixVolumeTable - MIX_VARS)($a0)
    /* 800DBAFC */  srl         $v1, $v1, 3
    /* 800DBB00 */  sll         $v1, $v1, 10
    /* 800DBB04 */  add         $t2, $t2, $v1
    /* 800DBB08 */  ld          $t3, 0x10($a1)
    /* 800DBB0C */  ld          $t4, 0x8($a1)
    /* 800DBB10 */  lw          $t5, 0x18($a1)
    /* 800DBB14 */  lw          $t6, 0x1C($a1)
    /* 800DBB18 */  lw          $t7, 0x24($a1)
    /* 800DBB1C */  lw          $t8, 0x0($a1)
    /* 800DBB20 */  beq         $a3, $zero, .L800DBBCC
    /* 800DBB24 */   lw          $t9, (sMixCount - MIX_VARS)($a0)
.L800DBB28:
    /* 800DBB28 */  dsra32      $v0, $t4, 0
    /* 800DBB2C */  sltu        $v1, $v0, $t5
    /* 800DBB30 */  beq         $v1, $zero, .L800DBB70
    /* 800DBB34 */   addu        $v0, $v0, $t8
    /* 800DBB38 */  lbu         $v0, 0x0($v0)
    /* 800DBB3C */  daddu       $t4, $t4, $t3
    /* 800DBB40 */  sll         $v0, $v0, 2
    /* 800DBB44 */  addu        $v1, $v0, $t1
    /* 800DBB48 */  addu        $v0, $v0, $t2
    /* 800DBB4C */  lw          $v1, 0x0($v1)
    /* 800DBB50 */  lw          $v0, 0x0($v0)
    /* 800DBB54 */  addi        $t9, $t9, -0x1
    /* 800DBB58 */  sw          $v1, 0x0($t0)
    /* 800DBB5C */  sw          $v0, 0x4($t0)
    /* 800DBB60 */  bne         $t9, $zero, .L800DBB28
    /* 800DBB64 */   addiu       $t0, $t0, 0x8
    /* 800DBB68 */  j           .L800DBC64
    /* 800DBB6C */   sd          $t4, 0x8($a1)
.L800DBB70:
    /* 800DBB70 */  beq         $t6, $t7, .L800DBBA0
    /* 800DBB74 */   lw          $v0, 0x4($a1)
    /* 800DBB78 */  dsll32      $t4, $t4, 0
    /* 800DBB7C */  dsrl32      $t4, $t4, 0
    /* 800DBB80 */  dsll32      $v1, $t6, 0
    /* 800DBB84 */  or          $t4, $t4, $v1
    /* 800DBB88 */  lw          $at, 0x24($a1)
    /* 800DBB8C */  or          $t5, $t7, $zero
    /* 800DBB90 */  sw          $at, 0x18($a1)
    /* 800DBB94 */  ori         $v0, $v0, 0x2
    /* 800DBB98 */  j           .L800DBB28
    /* 800DBB9C */   sw          $v0, 0x4($a1)
.L800DBBA0:
    /* 800DBBA0 */  xori        $v0, $v0, 0x1
    /* 800DBBA4 */  sw          $v0, 0x4($a1)
    /* 800DBBA8 */  beq         $t9, $zero, .L800DBC64
    /* 800DBBAC */   sd          $t4, 0x8($a1)
.L800DBBB0:
    /* 800DBBB0 */  addi        $t9, $t9, -0x1
    /* 800DBBB4 */  sw          $zero, 0x0($t0)
    /* 800DBBB8 */  sw          $zero, 0x4($t0)
    /* 800DBBBC */  bne         $t9, $zero, .L800DBBB0
    /* 800DBBC0 */   addiu       $t0, $t0, 0x8
    /* 800DBBC4 */  j           .L800DBC64
    /* 800DBBC8 */   nop
.L800DBBCC:
    /* 800DBBCC */  dsra32      $v0, $t4, 0
    /* 800DBBD0 */  sltu        $v1, $v0, $t5
    /* 800DBBD4 */  beq         $v1, $zero, .L800DBC24
    /* 800DBBD8 */   addu        $v0, $v0, $t8
    /* 800DBBDC */  lbu         $v0, 0x0($v0)
    /* 800DBBE0 */  daddu       $t4, $t4, $t3
    /* 800DBBE4 */  lw          $s0, 0x0($t0)
    /* 800DBBE8 */  lw          $s1, 0x4($t0)
    /* 800DBBEC */  sll         $v0, $v0, 2
    /* 800DBBF0 */  addu        $v1, $v0, $t1
    /* 800DBBF4 */  addu        $v0, $v0, $t2
    /* 800DBBF8 */  lw          $v1, 0x0($v1)
    /* 800DBBFC */  lw          $v0, 0x0($v0)
    /* 800DBC00 */  addi        $t9, $t9, -0x1
    /* 800DBC04 */  addu        $v1, $v1, $s0
    /* 800DBC08 */  addu        $v0, $v0, $s1
    /* 800DBC0C */  sw          $v1, 0x0($t0)
    /* 800DBC10 */  sw          $v0, 0x4($t0)
    /* 800DBC14 */  bne         $t9, $zero, .L800DBBCC
    /* 800DBC18 */   addiu       $t0, $t0, 0x8
    /* 800DBC1C */  j           .L800DBC64
    /* 800DBC20 */   sd          $t4, 0x8($a1)
.L800DBC24:
    /* 800DBC24 */  beq         $t6, $t7, .L800DBC54
    /* 800DBC28 */   lw          $v0, 0x4($a1)
    /* 800DBC2C */  dsll32      $t4, $t4, 0
    /* 800DBC30 */  dsrl32      $t4, $t4, 0
    /* 800DBC34 */  dsll32      $v1, $t6, 0
    /* 800DBC38 */  or          $t4, $t4, $v1
    /* 800DBC3C */  lw          $at, 0x24($a1)
    /* 800DBC40 */  or          $t5, $t7, $zero
    /* 800DBC44 */  sw          $at, 0x18($a1)
    /* 800DBC48 */  ori         $v0, $v0, 0x2
    /* 800DBC4C */  j           .L800DBBCC
    /* 800DBC50 */   sw          $v0, 0x4($a1)
.L800DBC54:
    /* 800DBC54 */  xori        $v0, $v0, 0x1
    /* 800DBC58 */  sw          $v0, 0x4($a1)
    /* 800DBC5C */  j           .L800DBC64
    /* 800DBC60 */   sd          $t4, 0x8($a1)
.L800DBC64:
    /* 800DBC64 */  ori         $a3, $zero, 0x0
.L800DBC68:
    /* 800DBC68 */  addi        $a2, $a2, -0x1
    /* 800DBC6C */  bne         $a2, $zero, .L800DBACC
    /* 800DBC70 */   addi        $a1, $a1, 0x30
    /* 800DBC74 */  lw          $t9, (sMixCount - MIX_VARS)($a0)
    /* 800DBC78 */  lw          $t0, (sMixSrc - MIX_VARS)($a0)
    /* 800DBC7C */  beq         $a3, $zero, .L800DBC94
.L800DBC80:
    /* 800DBC80 */   addi        $t9, $t9, -0x1
    /* 800DBC84 */  sw          $zero, 0x0($t0)
    /* 800DBC88 */  sw          $zero, 0x4($t0)
    /* 800DBC8C */  bne         $t9, $zero, .L800DBC80
    /* 800DBC90 */   addiu       $t0, $t0, 0x8
.L800DBC94:
    /* 800DBC94 */  ld          $s0, 0x0($sp)
    /* 800DBC98 */  ld          $s1, 0x8($sp)
    /* 800DBC9C */  jr          $ra
    /* 800DBCA0 */   addi        $sp, $sp, 0x10

# the mix to 8-bit output (sMixClampMin / sMixClampMax unused)
.global softmix_output_8bit
softmix_output_8bit:
    /* 800DBCA4 */  lui         $v0, (MIX_VARS >> 16)
    /* 800DBCA8 */  ori         $v0, $v0, (MIX_VARS & 0xFFFF)
    /* 800DBCAC */  lw          $t0, (sMixSrc - MIX_VARS)($v0)
    /* 800DBCB0 */  lw          $t1, (sMixDst - MIX_VARS)($v0)
    /* 800DBCB4 */  lw          $t2, (sMixCount - MIX_VARS)($v0)
    /* 800DBCB8 */  lw          $t3, (sMixBias - MIX_VARS)($v0)
    /* 800DBCBC */  lbu         $t4, (sMixShift - MIX_VARS)($v0)
    /* 800DBCC0 */  sra         $t2, $t2, 1
    /* 800DBCC4 */  beq         $t2, $zero, .L800DBCF8
.L800DBCC8:
    /* 800DBCC8 */   lw          $v0, 0x0($t0)
    /* 800DBCCC */  lw          $v1, 0x4($t0)
    /* 800DBCD0 */  addi        $t2, $t2, -0x1
    /* 800DBCD4 */  add         $v0, $v0, $t3
    /* 800DBCD8 */  add         $v1, $v1, $t3
    /* 800DBCDC */  srav        $v0, $v0, $t4
    /* 800DBCE0 */  srav        $v1, $v1, $t4
    /* 800DBCE4 */  sb          $v0, 0x0($t1)
    /* 800DBCE8 */  sb          $v0, 0x2($t1)
    /* 800DBCEC */  addi        $t0, $t0, 0x8
    /* 800DBCF0 */  bne         $t2, $zero, .L800DBCC8
    /* 800DBCF4 */   addi        $t1, $t1, 0x4
.L800DBCF8:
    /* 800DBCF8 */  jr          $ra
    /* 800DBCFC */   nop

# the mix to 16-bit output, clamped
.global softmix_output_16bit
softmix_output_16bit:
    /* 800DBD00 */  lui         $v0, (MIX_VARS >> 16)
    /* 800DBD04 */  ori         $v0, $v0, (MIX_VARS & 0xFFFF)
    /* 800DBD08 */  lw          $t0, (sMixSrc - MIX_VARS)($v0)
    /* 800DBD0C */  lw          $t1, (sMixDst - MIX_VARS)($v0)
    /* 800DBD10 */  lw          $t2, (sMixCount - MIX_VARS)($v0)
    /* 800DBD14 */  lw          $t3, (sMixBias - MIX_VARS)($v0)
    /* 800DBD18 */  lbu         $t4, (sMixShift - MIX_VARS)($v0)
    /* 800DBD1C */  lw          $t5, (sMixClampMax - MIX_VARS)($v0)
    /* 800DBD20 */  lw          $t6, (sMixClampMin - MIX_VARS)($v0)
    /* 800DBD24 */  sra         $t2, $t2, 1
    /* 800DBD28 */  beq         $t2, $zero, .L800DBD8C
.L800DBD2C:
    /* 800DBD2C */   lw          $v0, 0x0($t0)
    /* 800DBD30 */  lw          $v1, 0x4($t0)
    /* 800DBD34 */  addi        $t2, $t2, -0x1
    /* 800DBD38 */  add         $v0, $v0, $t3
    /* 800DBD3C */  add         $v1, $v1, $t3
    /* 800DBD40 */  srav        $v0, $v0, $t4
    /* 800DBD44 */  srav        $v1, $v1, $t4
    /* 800DBD48 */  slt         $t7, $v0, $t5
    /* 800DBD4C */  beql        $t7, $zero, .L800DBD60
    /* 800DBD50 */   add         $v0, $zero, $t5
    /* 800DBD54 */  slt         $t7, $v0, $t6
    /* 800DBD58 */  bnel        $t7, $zero, .L800DBD60
    /* 800DBD5C */   add         $v0, $zero, $t6
.L800DBD60:
    /* 800DBD60 */  slt         $t7, $v1, $t5
    /* 800DBD64 */  beql        $t7, $zero, .L800DBD78
    /* 800DBD68 */   add         $v1, $zero, $t5
    /* 800DBD6C */  slt         $t7, $v1, $t6
    /* 800DBD70 */  bnel        $t7, $zero, .L800DBD78
    /* 800DBD74 */   add         $v1, $zero, $t6
.L800DBD78:
    /* 800DBD78 */  sh          $v0, 0x0($t1)
    /* 800DBD7C */  sh          $v1, 0x2($t1)
    /* 800DBD80 */  addi        $t0, $t0, 0x8
    /* 800DBD84 */  bne         $t2, $zero, .L800DBD2C
    /* 800DBD88 */   addi        $t1, $t1, 0x4
.L800DBD8C:
    /* 800DBD8C */  jr          $ra
    /* 800DBD90 */   nop
