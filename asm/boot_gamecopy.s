# boot_gamecopy.s -- copy-and-jump stub after SwdExpand in the boot loader (hand-written asm)
# image: boot
# vram: 0x80011584-0x80011600 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# Not part of SWDN64.ASM. Copies gBootStubSize bytes from gBootStubSrc to gBootStubDst, invalidates the
# instruction cache and jumps to gBootStubEntry. The boot loader's C code (0x800014B8-0x8000162C) copies the
# stub (0x80011584-0x800115F8) to 0x802F0000, stores the four parameter words (zero in the image) and calls
# the copy, which moves the decompressed game over the boot loader and starts it at 0x80000400.
# 0x800115F4-0x80011600 is 0xFF fill: the end of the ROM range boot.bin is cut from (ROM 0x1000-0x12200),
# not part of the program.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

.global boot_game_copy_stub
boot_game_copy_stub:
    /* 80011584 */  lui         $t0, %hi(gBootStubSrc)
    /* 80011588 */  lw          $t0, %lo(gBootStubSrc)($t0)
    /* 8001158C */  lui         $t1, %hi(gBootStubSize)
    /* 80011590 */  lw          $t1, %lo(gBootStubSize)($t1)
    /* 80011594 */  lui         $t2, %hi(gBootStubDst)
    /* 80011598 */  lw          $t2, %lo(gBootStubDst)($t2)
    /* 8001159C */  lui         $t3, %hi(gBootStubEntry)
    /* 800115A0 */  lw          $t3, %lo(gBootStubEntry)($t3)
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
gBootStubSrc:
    /* 800115E4 */  .word 0x00000000
gBootStubSize:
    /* 800115E8 */  .word 0x00000000
gBootStubDst:
    /* 800115EC */  .word 0x00000000
gBootStubEntry:
    /* 800115F0 */  .word 0x00000000

# 0xFF ROM fill (end of boot.bin's ROM range)
    /* 800115F4 */  .word 0xFFFFFFFF
    /* 800115F8 */  .word 0xFFFFFFFF
    /* 800115FC */  .word 0xFFFFFFFF
