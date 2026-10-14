# boot_entry.s -- entry point of the boot loader image (hand-written asm)
# image: boot
# vram: 0x80000400-0x80000450 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# 0x80000400 (hand-written asm): stack 0x803FFEF0, clear the BSS (0x80011600-0x8001B610), call
# boot_main, `break` if it returns. 0x80000444-0x80000450 is zero fill to the next object.
# 0x80000450-0x800004A8 is the boot loader's main, compiled C: boot_main
# (src/boot/boot_main.c), built by tools/build_code.py.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

.global boot_entry
boot_entry:
    /* 80000400 */  lui         $t0, (0xBEEFDEAD >> 16)
    /* 80000404 */  ori         $t0, $t0, (0xBEEFDEAD & 0xFFFF)
    /* 80000408 */  lui         $sp, (0x803FFEF0 >> 16)
    /* 8000040C */  ori         $sp, $sp, (0x803FFEF0 & 0xFFFF)
    /* 80000410 */  lui         $t0, %hi(sBootIdleThread)
    /* 80000414 */  addiu       $t0, $t0, %lo(sBootIdleThread)
    /* 80000418 */  lui         $t1, %hi(gBootBssEnd)
    /* 8000041C */  addiu       $t1, $t1, %lo(gBootBssEnd)
    /* 80000420 */  beq         $t0, $t1, .L80000438
    /* 80000424 */   nop
.L80000428:
    /* 80000428 */  addiu       $t0, $t0, 0x4
    /* 8000042C */  sltu        $at, $t0, $t1
    /* 80000430 */  bne         $at, $zero, .L80000428
    /* 80000434 */   sw          $zero, -0x4($t0)
.L80000438:
    /* 80000438 */  jal         boot_main
    /* 8000043C */   nop
    /* 80000440 */  break       1, 2
    .space 0xC  /* 80000444 */  # zero fill to the next object (16-byte alignment)

