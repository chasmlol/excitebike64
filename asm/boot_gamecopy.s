# boot_gamecopy.s -- copy-and-jump stub after SwdExpand in the boot loader (hand-written asm)
# image: boot
# vram: 0x80011584-0x80011600
#
# Not part of SWDN64.ASM. Copies D_800115E8 bytes from D_800115E4 to D_800115EC, invalidates the
# instruction cache and jumps to D_800115F0. The boot loader's C code (0x800014B8-0x8000162C) copies the
# stub (0x80011584-0x800115F8) to 0x802F0000, stores the four parameter words (zero in the image) and calls
# the copy, which moves the decompressed game over the boot loader and starts it at 0x80000400.
# 0x800115F4-0x80011600 is 0xFF fill: the end of the ROM range boot.bin is cut from (ROM 0x1000-0x12200),
# not part of the program.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

.global boot_func_80011584
boot_func_80011584:
    /* 80011584 */  lui         $t0, %hi(D_800115E4)
    /* 80011588 */  lw          $t0, %lo(D_800115E4)($t0)
    /* 8001158C */  lui         $t1, %hi(D_800115E8)
    /* 80011590 */  lw          $t1, %lo(D_800115E8)($t1)
    /* 80011594 */  lui         $t2, %hi(D_800115EC)
    /* 80011598 */  lw          $t2, %lo(D_800115EC)($t2)
    /* 8001159C */  lui         $t3, %hi(D_800115F0)
    /* 800115A0 */  lw          $t3, %lo(D_800115F0)($t3)
.L800115A4:
    /* 800115A4 */  lw          $t5, 0x0($t0)
    /* 800115A8 */  sw          $t5, 0x0($t2)
    /* 800115AC */  addiu       $t0, $t0, 0x4
    /* 800115B0 */  addiu       $t1, $t1, -0x4
    /* 800115B4 */  bne         $t1, $zero, .L800115A4
    /* 800115B8 */   addiu       $t2, $t2, 0x4
    /* 800115BC */  lui         $a0, 0x8000
    /* 800115C0 */  nop
    /* 800115C4 */  addiu       $a1, $zero, 0x200
.L800115C8:
    /* 800115C8 */  cache       0x00, 0x0($a0)
    /* 800115CC */  addiu       $a0, $a0, 0x20
    /* 800115D0 */  addi        $a1, $a1, -0x1
    /* 800115D4 */  bne         $a1, $zero, .L800115C8
    /* 800115D8 */   nop
    /* 800115DC */  jr          $t3
    /* 800115E0 */   nop

# parameters: source, byte count, destination, entry point
D_800115E4:
    /* 800115E4 */  .word 0x00000000
D_800115E8:
    /* 800115E8 */  .word 0x00000000
D_800115EC:
    /* 800115EC */  .word 0x00000000
D_800115F0:
    /* 800115F0 */  .word 0x00000000

# 0xFF ROM fill (end of boot.bin's ROM range)
    /* 800115F4 */  .word 0xFFFFFFFF
    /* 800115F8 */  .word 0xFFFFFFFF
    /* 800115FC */  .word 0xFFFFFFFF
