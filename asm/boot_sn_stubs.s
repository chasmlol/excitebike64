# boot_sn_stubs.s -- SN Systems debug/exception stub linked into the boot loader (hand-written asm)
# image: boot
# vram: 0x80002CB0-0x80003660 in LodgeNet (each version: versions/<name>/version.toml [asm_files])
#
# SN64 runtime code (hand-written asm in SN's library) that hooks the R4300 exception vectors for SN's
# development hardware: sn_install_exception_hooks (called from sn_debug_init) copies the two 5-instruction
# vector templates over 0x80000180 (general exception) and 0x80000000 (TLB refill), saving the original
# 5 instructions of each vector in D_80002CC4 / D_80002CFC; the 4-instruction stubs right after each save
# area jump back into the original vector (+0x14), so a saved vector still runs as a chain. The handler
# (sn_exception_handler) keeps its state in the BSS block at sSnRegisterDump (full register dump, boot_func_80003350),
# polls PI status and the SN board's registers at 0xB1FFFFD8 / 0xB1FFFFF0, and dispatches `break` codes
# 0x400-0x408 through the jump table jtbl_800031D0 (pointers, real data).
# Not code: D_80002CC4 / D_80002CFC (5-word save areas, zero in the image), jtbl_800031D0 (9 pointers),
# 0x80003654-0x80003660 (linker fill before the RSP microcode at 0x80003660: non-zero leftover bytes, not
# referenced, `.word` only to reproduce them).

.set noat
.set noreorder
.set gp=64
.set fp=64
.section .text, "ax"

# template copied to 0x80000180 (general exception vector): jump to the handler
.global sn_exc_vector_template
sn_exc_vector_template:
    /* 80002CB0 */  mtc0        $k0, $30
    /* 80002CB4 */  lui         $k0, %hi(sn_exception_handler)
    /* 80002CB8 */  addiu       $k0, $k0, %lo(sn_exception_handler)
    /* 80002CBC */  jr          $k0
    /* 80002CC0 */   nop

# the original 5 words of 0x80000180, saved by sn_install_exception_hooks (run as code by the chain)
D_80002CC4:
    /* 80002CC4 */  .word 0x00000000
    /* 80002CC8 */  .word 0x00000000
    /* 80002CCC */  .word 0x00000000
    /* 80002CD0 */  .word 0x00000000
    /* 80002CD4 */  .word 0x00000000

# chain: continue in the original general exception vector
.global sn_exc_vector_chain
sn_exc_vector_chain:
    /* 80002CD8 */  lui         $k0, (0x80000194 >> 16)
    /* 80002CDC */  ori         $k0, $k0, (0x80000194 & 0xFFFF)
    /* 80002CE0 */  jr          $k0
    /* 80002CE4 */   nop

# template copied to 0x80000000 (TLB refill vector)
.global sn_tlb_vector_template
sn_tlb_vector_template:
    /* 80002CE8 */  mtc0        $k0, $30
    /* 80002CEC */  lui         $k0, %hi(sn_exception_handler)
    /* 80002CF0 */  addiu       $k0, $k0, %lo(sn_exception_handler)
    /* 80002CF4 */  jr          $k0
    /* 80002CF8 */   nop

# the original 5 words of 0x80000000
D_80002CFC:
    /* 80002CFC */  .word 0x00000000
    /* 80002D00 */  .word 0x00000000
    /* 80002D04 */  .word 0x00000000
    /* 80002D08 */  .word 0x00000000
    /* 80002D0C */  .word 0x00000000

# chain: continue in the original TLB refill vector
.global sn_tlb_vector_chain
sn_tlb_vector_chain:
    /* 80002D10 */  lui         $k0, (0x80000014 >> 16)
    /* 80002D14 */  ori         $k0, $k0, (0x80000014 & 0xFFFF)
    /* 80002D18 */  jr          $k0
    /* 80002D1C */   nop

# install the vector hooks, flush the caches
.global sn_install_exception_hooks
sn_install_exception_hooks:
    /* 80002D20 */  addi        $sp, $sp, -0x8
    /* 80002D24 */  sw          $ra, 0x0($sp)
    /* 80002D28 */  lui         $t0, %hi(sn_exc_vector_template)
    /* 80002D2C */  addiu       $t0, $t0, %lo(sn_exc_vector_template)
    /* 80002D30 */  lui         $t1, %hi(D_80002CC4)
    /* 80002D34 */  addiu       $t1, $t1, %lo(D_80002CC4)
    /* 80002D38 */  lui         $t2, (0x80000180 >> 16)
    /* 80002D3C */  ori         $t2, $t2, (0x80000180 & 0xFFFF)
    /* 80002D40 */  lui         $t3, %hi(D_80002CC4)
    /* 80002D44 */  addiu       $t3, $t3, %lo(D_80002CC4)
.L80002D48:
    /* 80002D48 */  lw          $t4, 0x0($t2)
    /* 80002D4C */  lw          $t5, 0x0($t0)
    /* 80002D50 */  addiu       $t0, $t0, 0x4
    /* 80002D54 */  sw          $t4, 0x0($t1)
    /* 80002D58 */  addiu       $t1, $t1, 0x4
    /* 80002D5C */  sw          $t5, 0x0($t2)
    /* 80002D60 */  bne         $t0, $t3, .L80002D48
    /* 80002D64 */   addiu       $t2, $t2, 0x4
    /* 80002D68 */  lui         $t0, %hi(sn_tlb_vector_template)
    /* 80002D6C */  addiu       $t0, $t0, %lo(sn_tlb_vector_template)
    /* 80002D70 */  lui         $t1, %hi(D_80002CFC)
    /* 80002D74 */  addiu       $t1, $t1, %lo(D_80002CFC)
    /* 80002D78 */  lui         $t2, 0x8000
    /* 80002D7C */  lui         $t3, %hi(D_80002CFC)
    /* 80002D80 */  addiu       $t3, $t3, %lo(D_80002CFC)
.L80002D84:
    /* 80002D84 */  lw          $t4, 0x0($t2)
    /* 80002D88 */  lw          $t5, 0x0($t0)
    /* 80002D8C */  addiu       $t0, $t0, 0x4
    /* 80002D90 */  sw          $t4, 0x0($t1)
    /* 80002D94 */  addiu       $t1, $t1, 0x4
    /* 80002D98 */  sw          $t5, 0x0($t2)
    /* 80002D9C */  bne         $t0, $t3, .L80002D84
    /* 80002DA0 */   addiu       $t2, $t2, 0x4
    /* 80002DA4 */  jal         sn_flush_dcache
    /* 80002DA8 */   nop
    /* 80002DAC */  jal         sn_inval_icache
    /* 80002DB0 */   nop
    /* 80002DB4 */  addiu       $t0, $zero, -0x802
    /* 80002DB8 */  lui         $at, %hi(D_80018448)
    /* 80002DBC */  sw          $t0, %lo(D_80018448)($at)
    /* 80002DC0 */  lw          $ra, 0x0($sp)
    /* 80002DC4 */  jr          $ra
    /* 80002DC8 */   addi        $sp, $sp, 0x8

# write back + invalidate the data cache (8 KB)
.global sn_flush_dcache
sn_flush_dcache:
    /* 80002DCC */  lui         $t0, 0x8000
    /* 80002DD0 */  addiu       $t1, $t0, 0x1FF0
.L80002DD4:
    /* 80002DD4 */  cache       0x01, 0x0($t0)
    /* 80002DD8 */  bne         $t0, $t1, .L80002DD4
    /* 80002DDC */   addiu       $t0, $t0, 0x10
    /* 80002DE0 */  jr          $ra
    /* 80002DE4 */   nop

# invalidate the instruction cache (16 KB)
.global sn_inval_icache
sn_inval_icache:
    /* 80002DE8 */  lui         $t0, 0x8000
    /* 80002DEC */  addiu       $t1, $t0, 0x3FE0
.L80002DF0:
    /* 80002DF0 */  cache       0x00, 0x0($t0)
    /* 80002DF4 */  bne         $t0, $t1, .L80002DF0
    /* 80002DF8 */   addiu       $t0, $t0, 0x20
    /* 80002DFC */  jr          $ra
    /* 80002E00 */   nop

# read the Status register
.global sn_get_status_reg
sn_get_status_reg:
    /* 80002E04 */  mfc0        $v0, $12
    /* 80002E08 */  jr          $ra
    /* 80002E0C */   nop

# write the Status register
.global sn_set_status_reg
sn_set_status_reg:
    /* 80002E10 */  mtc0        $a0, $12
    /* 80002E14 */  jr          $ra
    /* 80002E18 */   nop

# exception handler (entered from the vector templates, k0 = old k0 in ErrorEPC)
.global sn_exception_handler
sn_exception_handler:
    /* 80002E1C */  lui         $k0, %hi(sSnRegisterDump)
    /* 80002E20 */  addiu       $k0, $k0, %lo(sSnRegisterDump)
    /* 80002E24 */  sd          $at, 0x328($k0)
    /* 80002E28 */  sd          $v0, 0x330($k0)
    /* 80002E2C */  mfc0        $at, $13
    /* 80002E30 */  lw          $v0, 0x338($k0)
    /* 80002E34 */  srl         $at, $at, 2
    /* 80002E38 */  andi        $at, $at, 0x1F
    /* 80002E3C */  beq         $at, $zero, .L80002E60
    /* 80002E40 */   srlv        $v0, $v0, $at
    /* 80002E44 */  andi        $v0, $v0, 0x1
    /* 80002E48 */  bne         $v0, $zero, .L80002F9C
    /* 80002E4C */   nop
.L80002E50:
    /* 80002E50 */  ld          $at, 0x328($k0)
    /* 80002E54 */  ld          $v0, 0x330($k0)
    /* 80002E58 */  bgez        $zero, D_80002CC4
    /* 80002E5C */   nop
.L80002E60:
    /* 80002E60 */  mfc0        $at, $13
    /* 80002E64 */  nop
    /* 80002E68 */  andi        $at, $at, 0x800
    /* 80002E6C */  beq         $at, $zero, .L80002E50
    /* 80002E70 */   nop
.L80002E74:
    /* 80002E74 */  lui         $at, (0xA4600010 >> 16)
    /* 80002E78 */  ori         $at, $at, (0xA4600010 & 0xFFFF)
    /* 80002E7C */  lw          $at, 0x0($at)
    /* 80002E80 */  nop
    /* 80002E84 */  andi        $at, $at, 0x3
    /* 80002E88 */  bne         $at, $zero, .L80002E74
    /* 80002E8C */   nop
    /* 80002E90 */  lui         $at, 0xB200
    /* 80002E94 */  lw          $at, -0x28($at)
    /* 80002E98 */  nop
    /* 80002E9C */  andi        $at, $at, 0xFFFF
    /* 80002EA0 */  addi        $at, $at, -0x202
    /* 80002EA4 */  bne         $at, $zero, .L80002F60
    /* 80002EA8 */   nop
.L80002EAC:
    /* 80002EAC */  lui         $at, (0xA4600010 >> 16)
    /* 80002EB0 */  ori         $at, $at, (0xA4600010 & 0xFFFF)
    /* 80002EB4 */  lw          $at, 0x0($at)
    /* 80002EB8 */  nop
    /* 80002EBC */  andi        $at, $at, 0x3
    /* 80002EC0 */  bne         $at, $zero, .L80002EAC
    /* 80002EC4 */   nop
    /* 80002EC8 */  lui         $at, (0x3030303 >> 16)
    /* 80002ECC */  ori         $at, $at, (0x3030303 & 0xFFFF)
    /* 80002ED0 */  lui         $v0, 0xB200
    /* 80002ED4 */  sw          $at, -0x28($v0)
.L80002ED8:
    /* 80002ED8 */  lui         $at, (0xA4600010 >> 16)
    /* 80002EDC */  ori         $at, $at, (0xA4600010 & 0xFFFF)
    /* 80002EE0 */  lw          $at, 0x0($at)
    /* 80002EE4 */  nop
    /* 80002EE8 */  andi        $at, $at, 0x3
    /* 80002EEC */  bne         $at, $zero, .L80002ED8
    /* 80002EF0 */   nop
    /* 80002EF4 */  lui         $v0, 0xB200
    /* 80002EF8 */  lw          $at, -0x28($v0)
    /* 80002EFC */  lui         $v0, (0x3030303 >> 16)
    /* 80002F00 */  ori         $v0, $v0, (0x3030303 & 0xFFFF)
    /* 80002F04 */  sub         $at, $at, $v0
    /* 80002F08 */  bne         $at, $zero, .L80002F28
    /* 80002F0C */   nop
    /* 80002F10 */  lui         $at, (0x10001 >> 16)
    /* 80002F14 */  ori         $at, $at, (0x10001 & 0xFFFF)
.L80002F18:
    /* 80002F18 */  bne         $at, $zero, .L80002F18
    /* 80002F1C */   addi        $at, $at, -0x1
    /* 80002F20 */  beq         $at, $zero, .L80002ED8
    /* 80002F24 */   nop
.L80002F28:
    /* 80002F28 */  lui         $at, (0xA4600010 >> 16)
    /* 80002F2C */  ori         $at, $at, (0xA4600010 & 0xFFFF)
    /* 80002F30 */  lw          $at, 0x0($at)
    /* 80002F34 */  nop
    /* 80002F38 */  andi        $at, $at, 0x3
    /* 80002F3C */  bne         $at, $zero, .L80002F28
    /* 80002F40 */   nop
    /* 80002F44 */  lui         $v0, 0xB200
    /* 80002F48 */  sw          $zero, -0x28($v0)
    /* 80002F4C */  mfc0        $at, $13
    /* 80002F50 */  nop
    /* 80002F54 */  ori         $at, $at, 0x100
    /* 80002F58 */  j           .L80002E50
    /* 80002F5C */   mtc0        $at, $13
.L80002F60:
    /* 80002F60 */  lui         $at, (0xA4600010 >> 16)
    /* 80002F64 */  ori         $at, $at, (0xA4600010 & 0xFFFF)
    /* 80002F68 */  lw          $at, 0x0($at)
    /* 80002F6C */  nop
    /* 80002F70 */  andi        $at, $at, 0x3
    /* 80002F74 */  bne         $at, $zero, .L80002F60
    /* 80002F78 */   nop
    /* 80002F7C */  lui         $at, 0xB200
    /* 80002F80 */  lw          $at, -0x10($at)
    /* 80002F84 */  nop
    /* 80002F88 */  andi        $at, $at, 0xFFFF
    /* 80002F8C */  beq         $at, $zero, .L80002E50
    /* 80002F90 */   nop
    /* 80002F94 */  bgez        $zero, .L80002FD4
    /* 80002F98 */   nop
.L80002F9C:
    /* 80002F9C */  addiu       $v0, $zero, 0x9
    /* 80002FA0 */  bne         $at, $v0, .L80002FCC
    /* 80002FA4 */   nop
    /* 80002FA8 */  mfc0        $v0, $14
    /* 80002FAC */  nop
    /* 80002FB0 */  lw          $v0, 0x0($v0)
    /* 80002FB4 */  nop
    /* 80002FB8 */  srl         $v0, $v0, 6
    /* 80002FBC */  addiu       $at, $v0, -0x400
    /* 80002FC0 */  bgez        $at, .L800031A0
    /* 80002FC4 */   nop
.L80002FC8:
    /* 80002FC8 */  addiu       $at, $zero, 0x9
.L80002FCC:
    /* 80002FCC */  sb          $at, 0x320($k0)
    /* 80002FD0 */  sb          $zero, 0x322($k0)
.L80002FD4:
    /* 80002FD4 */  addiu       $at, $zero, 0x1
    /* 80002FD8 */  sw          $at, 0x33C($k0)
    /* 80002FDC */  ld          $at, 0x328($k0)
    /* 80002FE0 */  ld          $v0, 0x330($k0)
    /* 80002FE4 */  sd          $ra, 0xF8($k0)
    /* 80002FE8 */  jal         boot_func_80003350
    /* 80002FEC */   nop
    /* 80002FF0 */  lui         $sp, %hi(sSnDebugStackEnd)
    /* 80002FF4 */  addiu       $sp, $sp, %lo(sSnDebugStackEnd)
    /* 80002FF8 */  jal         sn_debug_command_loop
    /* 80002FFC */   nop
    /* 80003000 */  jal         sn_flush_dcache
    /* 80003004 */   nop
    /* 80003008 */  jal         sn_inval_icache
    /* 8000300C */   nop
    /* 80003010 */  lui         $k0, %hi(sSnRegisterDump)
    /* 80003014 */  addiu       $k0, $k0, %lo(sSnRegisterDump)
    /* 80003018 */  addiu       $at, $zero, 0x1
    /* 8000301C */  sb          $at, 0x320($k0)
    /* 80003020 */  ld          $t0, 0x108($k0)
    /* 80003024 */  nop
    /* 80003028 */  mthi        $t0
    /* 8000302C */  ld          $t0, 0x100($k0)
    /* 80003030 */  nop
    /* 80003034 */  mtlo        $t0
    /* 80003038 */  ld          $t0, 0x178($k0)
    /* 8000303C */  nop
    /* 80003040 */  mtc0        $t0, $12
    /* 80003044 */  ld          $t0, 0x110($k0)
    /* 80003048 */  nop
    /* 8000304C */  mtc0        $t0, $14
    /* 80003050 */  ld          $t0, 0xD0($k0)
    /* 80003054 */  nop
    /* 80003058 */  mtc0        $t0, $30
    /* 8000305C */  ld          $t0, 0x178($k0)
    /* 80003060 */  lui         $t1, (0x20000000 >> 16)
    /* 80003064 */  and         $t1, $t1, $t0
    /* 80003068 */  beq         $t1, $zero, .L8000311C
    /* 8000306C */   nop
    /* 80003070 */  lw          $t0, 0x318($k0)
    /* 80003074 */  nop
    /* 80003078 */  ctc1        $t0, $0
    /* 8000307C */  lw          $t0, 0x31C($k0)
    /* 80003080 */  nop
    /* 80003084 */  ctc1        $t0, $31
    /* 80003088 */  ldc1        $f0, 0x218($k0)
    /* 8000308C */  ldc1        $f2, 0x228($k0)
    /* 80003090 */  ldc1        $f4, 0x238($k0)
    /* 80003094 */  ldc1        $f6, 0x248($k0)
    /* 80003098 */  ldc1        $f8, 0x258($k0)
    /* 8000309C */  ldc1        $f10, 0x268($k0)
    /* 800030A0 */  ldc1        $f12, 0x278($k0)
    /* 800030A4 */  ldc1        $f14, 0x288($k0)
    /* 800030A8 */  ldc1        $f16, 0x298($k0)
    /* 800030AC */  ldc1        $f18, 0x2A8($k0)
    /* 800030B0 */  ldc1        $f20, 0x2B8($k0)
    /* 800030B4 */  ldc1        $f22, 0x2C8($k0)
    /* 800030B8 */  ldc1        $f24, 0x2D8($k0)
    /* 800030BC */  ldc1        $f26, 0x2E8($k0)
    /* 800030C0 */  ldc1        $f28, 0x2F8($k0)
    /* 800030C4 */  ldc1        $f30, 0x308($k0)
    /* 800030C8 */  ld          $t0, 0x178($k0)
    /* 800030CC */  lui         $t1, (0x4000000 >> 16)
    /* 800030D0 */  and         $t1, $t1, $t0
    /* 800030D4 */  beq         $t1, $zero, .L8000311C
    /* 800030D8 */   nop
    /* 800030DC */  ldc1        $f1, 0x220($k0)
    /* 800030E0 */  ldc1        $f3, 0x230($k0)
    /* 800030E4 */  ldc1        $f5, 0x240($k0)
    /* 800030E8 */  ldc1        $f7, 0x250($k0)
    /* 800030EC */  ldc1        $f9, 0x260($k0)
    /* 800030F0 */  ldc1        $f11, 0x270($k0)
    /* 800030F4 */  ldc1        $f13, 0x280($k0)
    /* 800030F8 */  ldc1        $f15, 0x290($k0)
    /* 800030FC */  ldc1        $f17, 0x2A0($k0)
    /* 80003100 */  ldc1        $f19, 0x2B0($k0)
    /* 80003104 */  ldc1        $f21, 0x2C0($k0)
    /* 80003108 */  ldc1        $f23, 0x2D0($k0)
    /* 8000310C */  ldc1        $f25, 0x2E0($k0)
    /* 80003110 */  ldc1        $f27, 0x2F0($k0)
    /* 80003114 */  ldc1        $f29, 0x300($k0)
    /* 80003118 */  ldc1        $f31, 0x310($k0)
.L8000311C:
    /* 8000311C */  ld          $at, 0x8($k0)
    /* 80003120 */  ld          $v0, 0x10($k0)
    /* 80003124 */  ld          $v1, 0x18($k0)
    /* 80003128 */  ld          $a0, 0x20($k0)
    /* 8000312C */  ld          $a1, 0x28($k0)
    /* 80003130 */  ld          $a2, 0x30($k0)
    /* 80003134 */  ld          $a3, 0x38($k0)
    /* 80003138 */  ld          $t0, 0x40($k0)
    /* 8000313C */  ld          $t1, 0x48($k0)
    /* 80003140 */  ld          $t2, 0x50($k0)
    /* 80003144 */  ld          $t3, 0x58($k0)
    /* 80003148 */  ld          $t4, 0x60($k0)
    /* 8000314C */  ld          $t5, 0x68($k0)
    /* 80003150 */  ld          $t6, 0x70($k0)
    /* 80003154 */  ld          $t7, 0x78($k0)
    /* 80003158 */  ld          $s0, 0x80($k0)
    /* 8000315C */  ld          $s1, 0x88($k0)
    /* 80003160 */  ld          $s2, 0x90($k0)
    /* 80003164 */  ld          $s3, 0x98($k0)
    /* 80003168 */  ld          $s4, 0xA0($k0)
    /* 8000316C */  ld          $s5, 0xA8($k0)
    /* 80003170 */  ld          $s6, 0xB0($k0)
    /* 80003174 */  ld          $s7, 0xB8($k0)
    /* 80003178 */  ld          $t8, 0xC0($k0)
    /* 8000317C */  ld          $t9, 0xC8($k0)
    /* 80003180 */  ld          $k1, 0xD8($k0)
    /* 80003184 */  ld          $gp, 0xE0($k0)
    /* 80003188 */  ld          $sp, 0xE8($k0)
    /* 8000318C */  ld          $fp, 0xF0($k0)
    /* 80003190 */  ld          $ra, 0xF8($k0)
    /* 80003194 */  sw          $zero, 0x33C($k0)
    /* 80003198 */  mfc0        $k0, $30
    /* 8000319C */  eret
.L800031A0:
    /* 800031A0 */  addiu       $at, $v0, -0x409
    /* 800031A4 */  bgez        $at, .L80002FC8
    /* 800031A8 */   nop
    /* 800031AC */  lui         $at, %hi(jtbl_800031D0)
    /* 800031B0 */  addiu       $at, $at, %lo(jtbl_800031D0)
    /* 800031B4 */  addiu       $v0, $v0, -0x400
    /* 800031B8 */  sll         $v0, $v0, 2
    /* 800031BC */  addu        $v0, $v0, $at
    /* 800031C0 */  lw          $v0, 0x0($v0)
    /* 800031C4 */  nop
    /* 800031C8 */  jr          $v0
    /* 800031CC */   nop

# jump table for break codes 0x400-0x408 (pointers)
jtbl_800031D0:
    /* 800031D0 */  .word .L800031F4
    /* 800031D4 */  .word .L80003248
    /* 800031D8 */  .word .L80003254
    /* 800031DC */  .word .L80002FC8
    /* 800031E0 */  .word .L80003260
    /* 800031E4 */  .word .L80003268
    /* 800031E8 */  .word .L800032F8
    /* 800031EC */  .word .L80003304
    /* 800031F0 */  .word .L80003320
.L800031F4:
    /* 800031F4 */  lw          $at, 0x33C($k0)
    /* 800031F8 */  nop
    /* 800031FC */  bne         $at, $zero, .L80003330
    /* 80003200 */   nop
    /* 80003204 */  addiu       $at, $zero, 0x64
.L80003208:
    /* 80003208 */  bne         $at, $zero, .L80003208
    /* 8000320C */   addi        $at, $at, -0x1
    /* 80003210 */  lui         $at, (0xA4600010 >> 16)
    /* 80003214 */  ori         $at, $at, (0xA4600010 & 0xFFFF)
.L80003218:
    /* 80003218 */  lw          $v0, 0x0($at)
    /* 8000321C */  nop
    /* 80003220 */  andi        $v0, $v0, 0x3
    /* 80003224 */  bne         $v0, $zero, .L80003218
    /* 80003228 */   nop
    /* 8000322C */  lui         $at, 0xB200
    /* 80003230 */  lw          $at, -0x10($at)
    /* 80003234 */  nop
    /* 80003238 */  beq         $at, $zero, .L80003330
    /* 8000323C */   nop
    /* 80003240 */  bgez        $zero, .L80002FD4
    /* 80003244 */   nop
.L80003248:
    /* 80003248 */  addiu       $at, $zero, -0x1
    /* 8000324C */  j           .L80002FCC
    /* 80003250 */   nop
.L80003254:
    /* 80003254 */  addiu       $at, $zero, -0x1
    /* 80003258 */  j           .L80002FCC
    /* 8000325C */   nop
.L80003260:
    /* 80003260 */  j           .L80002FC8
    /* 80003264 */   nop
.L80003268:
    /* 80003268 */  sd          $v1, 0x18($k0)
    /* 8000326C */  sd          $a0, 0x20($k0)
    /* 80003270 */  sd          $ra, 0xF8($k0)
    /* 80003274 */  bgezal      $zero, boot_func_8000328C
    /* 80003278 */   nop
    /* 8000327C */  ld          $ra, 0xF8($k0)
    /* 80003280 */  ld          $a0, 0x20($k0)
    /* 80003284 */  bgez        $zero, .L80003330
    /* 80003288 */   ld          $v1, 0x18($k0)

# restore the original vectors
.global boot_func_8000328C
boot_func_8000328C:
    /* 8000328C */  lui         $v1, (0x80000180 >> 16)
    /* 80003290 */  ori         $v1, $v1, (0x80000180 & 0xFFFF)
    /* 80003294 */  lui         $v0, %hi(D_80002CC4)
    /* 80003298 */  addiu       $v0, $v0, %lo(D_80002CC4)
    /* 8000329C */  addi        $a0, $v0, 0x14
.L800032A0:
    /* 800032A0 */  lw          $at, 0x0($v0)
    /* 800032A4 */  addi        $v0, $v0, 0x4
    /* 800032A8 */  sw          $at, 0x0($v1)
    /* 800032AC */  addi        $v1, $v1, 0x4
    /* 800032B0 */  bne         $v0, $a0, .L800032A0
    /* 800032B4 */   nop
    /* 800032B8 */  lui         $v1, 0x8000
    /* 800032BC */  lui         $v0, %hi(D_80002CFC)
    /* 800032C0 */  addiu       $v0, $v0, %lo(D_80002CFC)
    /* 800032C4 */  addi        $a0, $v0, 0x14
.L800032C8:
    /* 800032C8 */  lw          $at, 0x0($v0)
    /* 800032CC */  addi        $v0, $v0, 0x4
    /* 800032D0 */  sw          $at, 0x0($v1)
    /* 800032D4 */  addi        $v1, $v1, 0x4
    /* 800032D8 */  bne         $v0, $a0, .L800032C8
    /* 800032DC */   nop
    /* 800032E0 */  jal         sn_flush_dcache
    /* 800032E4 */   nop
    /* 800032E8 */  jal         sn_inval_icache
    /* 800032EC */   nop
    /* 800032F0 */  jr          $ra
    /* 800032F4 */   nop
.L800032F8:
    /* 800032F8 */  sw          $a0, 0x338($k0)
    /* 800032FC */  j           .L80003330
    /* 80003300 */   nop
.L80003304:
    /* 80003304 */  mfc0        $at, $14
    /* 80003308 */  nop
    /* 8000330C */  addiu       $at, $at, 0x4
    /* 80003310 */  mtc0        $at, $14
    /* 80003314 */  addiu       $at, $zero, 0x0
    /* 80003318 */  j           .L80002FCC
    /* 8000331C */   nop
.L80003320:
    /* 80003320 */  lui         $v0, %hi(sn_debug_init)
    /* 80003324 */  addiu       $v0, $v0, %lo(sn_debug_init)
    /* 80003328 */  j           .L80003334
    /* 8000332C */   nop
.L80003330:
    /* 80003330 */  ld          $v0, 0x330($k0)
.L80003334:
    /* 80003334 */  mfc0        $at, $14
    /* 80003338 */  nop
    /* 8000333C */  addiu       $at, $at, 0x4
    /* 80003340 */  mtc0        $at, $14
    /* 80003344 */  ld          $at, 0x328($k0)
    /* 80003348 */  mfc0        $k0, $30
    /* 8000334C */  eret

# save all registers to the block at k0
.global boot_func_80003350
boot_func_80003350:
    /* 80003350 */  sd          $zero, 0x0($k0)
    /* 80003354 */  sd          $at, 0x8($k0)
    /* 80003358 */  sd          $v0, 0x10($k0)
    /* 8000335C */  sd          $v1, 0x18($k0)
    /* 80003360 */  sd          $a0, 0x20($k0)
    /* 80003364 */  sd          $a1, 0x28($k0)
    /* 80003368 */  sd          $a2, 0x30($k0)
    /* 8000336C */  sd          $a3, 0x38($k0)
    /* 80003370 */  sd          $t0, 0x40($k0)
    /* 80003374 */  sd          $t1, 0x48($k0)
    /* 80003378 */  sd          $t2, 0x50($k0)
    /* 8000337C */  sd          $t3, 0x58($k0)
    /* 80003380 */  sd          $t4, 0x60($k0)
    /* 80003384 */  sd          $t5, 0x68($k0)
    /* 80003388 */  sd          $t6, 0x70($k0)
    /* 8000338C */  sd          $t7, 0x78($k0)
    /* 80003390 */  sd          $s0, 0x80($k0)
    /* 80003394 */  sd          $s1, 0x88($k0)
    /* 80003398 */  sd          $s2, 0x90($k0)
    /* 8000339C */  sd          $s3, 0x98($k0)
    /* 800033A0 */  sd          $s4, 0xA0($k0)
    /* 800033A4 */  sd          $s5, 0xA8($k0)
    /* 800033A8 */  sd          $s6, 0xB0($k0)
    /* 800033AC */  sd          $s7, 0xB8($k0)
    /* 800033B0 */  sd          $t8, 0xC0($k0)
    /* 800033B4 */  sd          $t9, 0xC8($k0)
    /* 800033B8 */  sd          $k1, 0xD8($k0)
    /* 800033BC */  sd          $gp, 0xE0($k0)
    /* 800033C0 */  sd          $sp, 0xE8($k0)
    /* 800033C4 */  sd          $fp, 0xF0($k0)
    /* 800033C8 */  mfc0        $t0, $30
    /* 800033CC */  sd          $t0, 0xD0($k0)
    /* 800033D0 */  mflo        $t0
    /* 800033D4 */  sd          $t0, 0x100($k0)
    /* 800033D8 */  mfhi        $t0
    /* 800033DC */  sd          $t0, 0x108($k0)
    /* 800033E0 */  mfc0        $t0, $0
    /* 800033E4 */  nop
    /* 800033E8 */  sd          $t0, 0x118($k0)
    /* 800033EC */  mfc0        $t0, $1
    /* 800033F0 */  nop
    /* 800033F4 */  sd          $t0, 0x120($k0)
    /* 800033F8 */  mfc0        $t0, $2
    /* 800033FC */  nop
    /* 80003400 */  sd          $t0, 0x128($k0)
    /* 80003404 */  mfc0        $t0, $3
    /* 80003408 */  nop
    /* 8000340C */  sd          $t0, 0x130($k0)
    /* 80003410 */  mfc0        $t0, $4
    /* 80003414 */  nop
    /* 80003418 */  sd          $t0, 0x138($k0)
    /* 8000341C */  mfc0        $t0, $5
    /* 80003420 */  nop
    /* 80003424 */  sd          $t0, 0x140($k0)
    /* 80003428 */  mfc0        $t0, $6
    /* 8000342C */  nop
    /* 80003430 */  sd          $t0, 0x148($k0)
    /* 80003434 */  mfc0        $t0, $7
    /* 80003438 */  nop
    /* 8000343C */  sd          $t0, 0x150($k0)
    /* 80003440 */  mfc0        $t0, $8
    /* 80003444 */  nop
    /* 80003448 */  sd          $t0, 0x158($k0)
    /* 8000344C */  mfc0        $t0, $9
    /* 80003450 */  nop
    /* 80003454 */  sd          $t0, 0x160($k0)
    /* 80003458 */  mfc0        $t0, $10
    /* 8000345C */  nop
    /* 80003460 */  sd          $t0, 0x168($k0)
    /* 80003464 */  mfc0        $t0, $11
    /* 80003468 */  nop
    /* 8000346C */  sd          $t0, 0x170($k0)
    /* 80003470 */  mfc0        $t0, $12
    /* 80003474 */  nop
    /* 80003478 */  sd          $t0, 0x178($k0)
    /* 8000347C */  mfc0        $t0, $13
    /* 80003480 */  nop
    /* 80003484 */  sd          $t0, 0x180($k0)
    /* 80003488 */  mfc0        $t0, $14
    /* 8000348C */  nop
    /* 80003490 */  sd          $t0, 0x188($k0)
    /* 80003494 */  mfc0        $t0, $15
    /* 80003498 */  nop
    /* 8000349C */  sd          $t0, 0x190($k0)
    /* 800034A0 */  mfc0        $t0, $16
    /* 800034A4 */  nop
    /* 800034A8 */  sd          $t0, 0x198($k0)
    /* 800034AC */  mfc0        $t0, $17
    /* 800034B0 */  nop
    /* 800034B4 */  sd          $t0, 0x1A0($k0)
    /* 800034B8 */  mfc0        $t0, $18
    /* 800034BC */  nop
    /* 800034C0 */  sd          $t0, 0x1A8($k0)
    /* 800034C4 */  mfc0        $t0, $19
    /* 800034C8 */  nop
    /* 800034CC */  sd          $t0, 0x1B0($k0)
    /* 800034D0 */  mfc0        $t0, $20
    /* 800034D4 */  nop
    /* 800034D8 */  sd          $t0, 0x1B8($k0)
    /* 800034DC */  mfc0        $t0, $21
    /* 800034E0 */  nop
    /* 800034E4 */  sd          $t0, 0x1C0($k0)
    /* 800034E8 */  mfc0        $t0, $22
    /* 800034EC */  nop
    /* 800034F0 */  sd          $t0, 0x1C8($k0)
    /* 800034F4 */  mfc0        $t0, $23
    /* 800034F8 */  nop
    /* 800034FC */  sd          $t0, 0x1D0($k0)
    /* 80003500 */  mfc0        $t0, $24
    /* 80003504 */  nop
    /* 80003508 */  sd          $t0, 0x1D8($k0)
    /* 8000350C */  mfc0        $t0, $25
    /* 80003510 */  nop
    /* 80003514 */  sd          $t0, 0x1E0($k0)
    /* 80003518 */  mfc0        $t0, $26
    /* 8000351C */  nop
    /* 80003520 */  sd          $t0, 0x1E8($k0)
    /* 80003524 */  mfc0        $t0, $27
    /* 80003528 */  nop
    /* 8000352C */  sd          $t0, 0x1F0($k0)
    /* 80003530 */  mfc0        $t0, $28
    /* 80003534 */  nop
    /* 80003538 */  sd          $t0, 0x1F8($k0)
    /* 8000353C */  mfc0        $t0, $29
    /* 80003540 */  nop
    /* 80003544 */  sd          $t0, 0x200($k0)
    /* 80003548 */  mfc0        $t0, $30
    /* 8000354C */  nop
    /* 80003550 */  sd          $t0, 0x208($k0)
    /* 80003554 */  mfc0        $t0, $31
    /* 80003558 */  nop
    /* 8000355C */  sd          $t0, 0x210($k0)
    /* 80003560 */  ld          $t0, 0x178($k0)
    /* 80003564 */  lui         $t1, (0x20000000 >> 16)
    /* 80003568 */  and         $t1, $t1, $t0
    /* 8000356C */  beq         $t1, $zero, .L80003620
    /* 80003570 */   nop
    /* 80003574 */  cfc1        $t0, $0
    /* 80003578 */  nop
    /* 8000357C */  sw          $t0, 0x318($k0)
    /* 80003580 */  cfc1        $t0, $31
    /* 80003584 */  nop
    /* 80003588 */  sw          $t0, 0x31C($k0)
    /* 8000358C */  sdc1        $f0, 0x218($k0)
    /* 80003590 */  sdc1        $f2, 0x228($k0)
    /* 80003594 */  sdc1        $f4, 0x238($k0)
    /* 80003598 */  sdc1        $f6, 0x248($k0)
    /* 8000359C */  sdc1        $f8, 0x258($k0)
    /* 800035A0 */  sdc1        $f10, 0x268($k0)
    /* 800035A4 */  sdc1        $f12, 0x278($k0)
    /* 800035A8 */  sdc1        $f14, 0x288($k0)
    /* 800035AC */  sdc1        $f16, 0x298($k0)
    /* 800035B0 */  sdc1        $f18, 0x2A8($k0)
    /* 800035B4 */  sdc1        $f20, 0x2B8($k0)
    /* 800035B8 */  sdc1        $f22, 0x2C8($k0)
    /* 800035BC */  sdc1        $f24, 0x2D8($k0)
    /* 800035C0 */  sdc1        $f26, 0x2E8($k0)
    /* 800035C4 */  sdc1        $f28, 0x2F8($k0)
    /* 800035C8 */  sdc1        $f30, 0x308($k0)
    /* 800035CC */  ld          $t0, 0x178($k0)
    /* 800035D0 */  lui         $t1, (0x4000000 >> 16)
    /* 800035D4 */  and         $t1, $t1, $t0
    /* 800035D8 */  beq         $t1, $zero, .L80003620
    /* 800035DC */   nop
    /* 800035E0 */  sdc1        $f1, 0x220($k0)
    /* 800035E4 */  sdc1        $f3, 0x230($k0)
    /* 800035E8 */  sdc1        $f5, 0x240($k0)
    /* 800035EC */  sdc1        $f7, 0x250($k0)
    /* 800035F0 */  sdc1        $f9, 0x260($k0)
    /* 800035F4 */  sdc1        $f11, 0x270($k0)
    /* 800035F8 */  sdc1        $f13, 0x280($k0)
    /* 800035FC */  sdc1        $f15, 0x290($k0)
    /* 80003600 */  sdc1        $f17, 0x2A0($k0)
    /* 80003604 */  sdc1        $f19, 0x2B0($k0)
    /* 80003608 */  sdc1        $f21, 0x2C0($k0)
    /* 8000360C */  sdc1        $f23, 0x2D0($k0)
    /* 80003610 */  sdc1        $f25, 0x2E0($k0)
    /* 80003614 */  sdc1        $f27, 0x2F0($k0)
    /* 80003618 */  sdc1        $f29, 0x300($k0)
    /* 8000361C */  sdc1        $f31, 0x310($k0)
.L80003620:
    /* 80003620 */  ld          $t0, 0x178($k0)
    /* 80003624 */  nop
    /* 80003628 */  andi        $t0, $t0, 0x4
    /* 8000362C */  bne         $t0, $zero, .L80003640
    /* 80003630 */   nop
    /* 80003634 */  ld          $t0, 0x188($k0)
    /* 80003638 */  j           .L80003648
    /* 8000363C */   nop
.L80003640:
    /* 80003640 */  ld          $t0, 0x208($k0)
    /* 80003644 */  nop
.L80003648:
    /* 80003648 */  sd          $t0, 0x110($k0)
    /* 8000364C */  jr          $ra
    /* 80003650 */   nop

# linker fill (leftover bytes) up to the RSP microcode at 0x80003660 (other bytes in each version)
.if VERSION == VERSION_LODGENET
    /* 80003654 */  .word 0x61746510
    /* 80003658 */  .word 0xDE008002
    /* 8000365C */  .word 0x08737764
.elseif VERSION == VERSION_US_REV1
    .word 0xE43D0BB8, 0x312812EC, 0x32FEE12F
.else
    .error "the linker fill of this version"
.endif
