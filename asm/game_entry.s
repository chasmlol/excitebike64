# game_entry.s -- entry point of the main game image (hand-written asm)
# image: game
# vram: 0x80000400-0x80000450
#
# The boot loader copies the decompressed game to 0x80000400 and jumps here. Sets the stack to 0x803FFEF0,
# clears the BSS (0x800DC0A0-0x80105580), calls the game's main (func_80073200) and stops on `break` if it
# ever returns. 0x80000444-0x80000450 is zero fill up to the first C object (16-byte alignment).

.set noat
.set noreorder
.set gp=64
.section .text, "ax"

.global func_80000400
func_80000400:
    /* 80000400 */  lui         $t0, (0xDEADBEEF >> 16)
    /* 80000404 */  ori         $t0, $t0, (0xDEADBEEF & 0xFFFF)
    /* 80000408 */  lui         $sp, (0x803FFEF0 >> 16)
    /* 8000040C */  ori         $sp, $sp, (0x803FFEF0 & 0xFFFF)
    /* 80000410 */  lui         $t0, %hi(D_800DC0A0)
    /* 80000414 */  addiu       $t0, $t0, %lo(D_800DC0A0)
    /* 80000418 */  lui         $t1, %hi(D_80105580)
    /* 8000041C */  addiu       $t1, $t1, %lo(D_80105580)
    /* 80000420 */  beq         $t0, $t1, .L80000438
    /* 80000424 */   nop
.L80000428:
    /* 80000428 */  addiu       $t0, $t0, 0x4
    /* 8000042C */  sltu        $at, $t0, $t1
    /* 80000430 */  bne         $at, $zero, .L80000428
    /* 80000434 */   sw          $zero, -0x4($t0)
.L80000438:
    /* 80000438 */  jal         func_80073200
    /* 8000043C */   nop
    /* 80000440 */  break       1, 2
    .space 0xC  /* 80000444 */  # zero fill to the next object (16-byte alignment)
