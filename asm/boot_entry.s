# boot_entry.s -- entry point of the boot loader image (hand-written asm)
# image: boot
# vram: 0x80000400-0x80000450
#
# 0x80000400 (hand-written asm): stack 0x803FFEF0, clear the BSS (0x80011600-0x8001B610), call
# boot_func_80000450, `break` if it returns. 0x80000444-0x80000450 is zero fill to the next object.
# 0x80000450-0x800004A8 is the boot loader's main, compiled C: boot_func_80000450
# (src/boot/boot_func_80000450.c), built by tools/build_code.py.

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

.global boot_func_80000400
boot_func_80000400:
    /* 80000400 */  lui         $t0, (0xBEEFDEAD >> 16)
    /* 80000404 */  ori         $t0, $t0, (0xBEEFDEAD & 0xFFFF)
    /* 80000408 */  lui         $sp, (0x803FFEF0 >> 16)
    /* 8000040C */  ori         $sp, $sp, (0x803FFEF0 & 0xFFFF)
    /* 80000410 */  lui         $t0, %hi(D_80011600)
    /* 80000414 */  addiu       $t0, $t0, %lo(D_80011600)
    /* 80000418 */  lui         $t1, %hi(D_8001B610)
    /* 8000041C */  addiu       $t1, $t1, %lo(D_8001B610)
    /* 80000420 */  beq         $t0, $t1, .L80000438
    /* 80000424 */   nop
.L80000428:
    /* 80000428 */  addiu       $t0, $t0, 0x4
    /* 8000042C */  sltu        $at, $t0, $t1
    /* 80000430 */  bne         $at, $zero, .L80000428
    /* 80000434 */   sw          $zero, -0x4($t0)
.L80000438:
    /* 80000438 */  jal         boot_func_80000450
    /* 8000043C */   nop
    /* 80000440 */  break       1, 2
    .space 0xC  /* 80000444 */  # zero fill to the next object (16-byte alignment)

