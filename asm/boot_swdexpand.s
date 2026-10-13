# boot_swdexpand.s -- SwdExpand decompressor (from the original source)
# image: boot
# vram: 0x800113B0-0x80011584
#
# John Brandwood's SWDN64.ASM (https://github.com/jbrandwood/SWD32, Boost Software License 1.0),
# converted from SN Systems' native assembler syntax (ASMN64) to GNU as syntax. The shipped boot image holds
# this module's whole section: its variables and SwdOffsets table (0x800113B0) and then SwdExpand (0x800113E4).
# SN's asn64 2.78 that we have only takes GNU syntax (the native-syntax ASMN64 isn't available), so the
# file was converted mechanically; every instruction, label and comment is the original's, in the same order.
# Changes (syntax only):
#   - comments `;` -> `#`; hex `$1F` -> `0x1F`; registers get a `$` (`t0` -> `$t0`, `r31` -> `$31`, `r0` -> `$0`)
#   - `DW` -> `.word`, `xdef` -> `.global`, labels in column 1 get a colon (`v DW 0` -> `v: .word 0`)
#   - `text group` / `end` commented out; `sectionw n64asm,text` -> `.section .text`
#   - `lui t6,(SwdOffsets>>16)&$0000FFFF` / `ori t6,t6,(SwdOffsets)&$0000FFFF` -> `%hi(SwdOffsets)` /
#     `%lo(SwdOffsets)`: GNU as can't shift a relocatable symbol. Same bytes as long as bit 15 of the
#     address is clear (SwdOffsets = 0x800113C4), since %hi adds the carry for a signed low half.
#   - added `.set noreorder` (SN's assembler doesn't fill delay slots either) and `.set gp=64` (`sd`/`ld r31`
#     are real 64-bit instructions, not o32 macros). `lw t0,pub__LoadCur` / `sw t0,pub__LoadCur` stay
#     macros: GNU as expands them to lui+lw (through t0) / lui+sw (through at), exactly as SN's assembler did.
#   - `.set noat` / `.set at` around the explicit uses of `at` (silences GNU as warnings; no effect on bytes).
# Variables 0x800113B0-0x800113E4 (v, pub__LoadCur/End, pub__SaveCur/End, SwdOffsets), SwdExpand 0x800113E4,
# swdreadbits 0x80011534 (swdloadbits 0x80011558). The bytes after it (0x80011584-) are boot_gamecopy.s.

	.set	noreorder
	.set	gp=64

# **************************************************************************
# **************************************************************************
# **                                                                      **
# ** SWDN64.ASM                                                    MODULE **
# **                                                                      **
# ** Decompress from Elmer's SWD format.                                  **
# **                                                                      **
# ** Copyright John Brandwood 1997-2015.                                  **
# **                                                                      **
# ** Distributed under the Boost Software License, Version 1.0.           **
# ** (See accompanying file LICENSE_1_0.txt or copy at                    **
# **  http://www.boost.org/LICENSE_1_0.txt)                               **
# **                                                                      **
# ** Register Names ...                                                   **
# **                                                                      **
# **  r0 r0     s0 r16                                                    **
# **  at r1     s1 r17                                                    **
# **  v0 r2     s2 r18                                                    **
# **  v1 r3     s3 r19                                                    **
# **  a0 r4     s4 r20                                                    **
# **  a1 r5     s5 r21                                                    **
# **  a2 r6     s6 r22                                                    **
# **  a3 r7     s7 r23                                                    **
# **  t0 r8     t8 r24                                                    **
# **  t1 r9     t9 r25                                                    **
# **  t2 r10    k0 r26                                                    **
# **  t3 r11    k1 r27                                                    **
# **  t4 r12    gp r28                                                    **
# **  t5 r13    sp r29                                                    **
# **  t6 r14    fp r30                                                    **
# **  t7 r15    ra r31                                                    **
# **                                                                      **
# **************************************************************************
# **************************************************************************

#text		group		# SN: group declaration (not needed by GNU as)
		.section .text	# SN: sectionw	n64asm,text

#
# EXTERNAL SYMBOLS
#

#
# EXPORTED SYMBOLS
#

		.global	pub__LoadCur
		.global	pub__LoadEnd

		.global	pub__SaveCur
		.global	pub__SaveEnd

		.global	SwdExpand

#
# DEFINITIONS
#

#
# GLOBAL VARIABLES
#

v:			.word	0	# start of variables

pub__LoadCur:		.word	0	# UB *
pub__LoadEnd:		.word	0	# UB *
pub__SaveCur:		.word	0	# UB *
pub__SaveEnd:		.word	0	# UB *

SwdOffsets:		.word	 5, 0x0000+1
			.word	 7, 0x0020+1
			.word	 9, 0x00A0+1
			.word	10, 0x02A0+1

#
# GLOBAL FUNCTIONS
#


# **************************************************************************
# * SwdExpand ()                                                           *
# **************************************************************************
# *                                                                        *
# **************************************************************************
# * Inputs  -                                                              *
# *                                                                        *
# * Output  -                                                              *
# *                                                                        *
# * Used    v0-v1, a0-a3, t0-t9                                            *
# *                                                                        *
# *         t0 = pub__LoadCur                                              *
# *         t1 = pub__SaveCur                                              *
# *         t2 = match_length                                              *
# *         t3 = match_offset                                              *
# *         t4 = ui__BitsInp                                               *
# *         t5 = constant = 1                                              *
# *         t6 = offsets table                                             *
# *                                                                        *
# * N.B.    The data is encoded as length/offset pairs.                    *
# *                                                                        *
# *         Lengths are encoded ...                                        *
# *                                                                        *
# *             1       : 0  dddddddd                                      *
# *             2       : 10                                               *
# *             3-5     : 11 xx                                            *
# *             6-20    : 11 00 xxxx                                       *
# *            21-275   : 11 00 0000 xxxxxxxx                              *
# *                                                                        *
# *         Offsets are encoded ...                                        *
# *                                                                        *
# *         $0000-$001F : 00       x xxxx                                  *
# *         $0020-$009F : 01     xxx xxxx                                  *
# *         $00A0-$029F : 10  x xxxx xxxx                                  *
# *         $02A0-$069F : 11 xx xxxx xxxx                                  *
# *                                                                        *
# **************************************************************************

SwdExpand:	addiu	$sp,$sp,-8
		sd	$31,0x00($sp)

		lw	$t0,pub__LoadCur
		lw	$t1,pub__SaveCur

		addi	$t4,$0,1				# bitsinput
		addi	$t5,$0,1				# constant

		lui	$t6,%hi(SwdOffsets)
		ori	$t6,$t6,%lo(SwdOffsets)

#
# Determine whether to move or copy data.
#

swdloop:	beq	$t4,$t5,swdload0			# bit buffer empty ?
		and	$t2,$t4,$t5

		bne	$t2,$0,swdcopy			# 0=move or 1=copy ?
		srl	$t4,$t4,1

swdmove:	lbu	$t3,($t0)				# move single byte
		addiu	$t0,$t0,1
		sb	$t3,($t1)
		j	swdloop
		addi	$t1,$t1,1

swdload0:	lbu	$t4,($t0)				# load bit buffer
		addiu	$t0,$t0,1
		ori	$t4,$t4,256			# set end marker

		and	$t2,$t4,$t5

		beq	$t2,$0,swdmove			# 0=move or 1=copy ?
		srl	$t4,$t4,1

#
# Get copy length.
#

swdcopy:	beq	$t4,$t5,swdload1			# bit buffer empty ?
		and	$t2,$t4,$t5

		beq	$t2,$0,swdaddr			# 0=copy2 or 1=copyN ?
		srl	$t4,$t4,1

swdcpyN:	add	$v0,$0,$0

		jal	swdreadbits			# read 2 bits
		add	$v1,$0,2

		bne	$v0,$0,swdaddr			# 0=more, nz=3-5
		addiu	$t2,$v0,3-3

		jal	swdreadbits			# read 4 bits
		add	$v1,$0,4

		bne	$v0,$0,swdaddr			# 0=more, nz=6-20
		addiu	$t2,$v0,6-3

		lbu	$v0,($t0)				# read 8 bits
		addiu	$t0,$t0,1

		bne	$v0,$0,swdaddr			# 0=more, nz=21-275
		addiu	$t2,$v0,21-3

		j	swddone				# end-of-file
		nop

swdload1:	lbu	$t4,($t0)				# load bit buffer
		addiu	$t0,$t0,1
		ori	$t4,$t4,256			# set end marker

		and	$t2,$t4,$t5

		bne	$t2,$0,swdcpyN			# 0=copy2 or 1=copyN ?
		srl	$t4,$t4,1

#
# Get copy offset.
#

swdaddr:	add	$t2,$t2,2				# minimum copy of 2

		add	$v0,$0,$0

		jal	swdreadbits			# read 2 bits
		add	$v1,$0,2

		sll	$v0,$v0,3
		add	$v0,$v0,$t6

		lw	$v1,0($v0)
		lw	$t3,4($v0)

		.set	noat				# (GNU as: explicit use of at)
		sltiu	$at,$v1,8				# addr > 8 bits long ?
		bne	$at,$0,swdadrN
		add	$v0,$0,$0
		.set	at				# (GNU as: the sw macros below need at)

		lbu	$v0,($t0)				# read top 8 bits
		addiu	$t0,$t0,1

		addiu	$v1,$v1,-8

swdadrN:	jal	swdreadbits			# read N bits
		nop

		add	$t3,$t3,$v0			# calc offset

cpydata:	subu	$a0,$t1,$t3			# calc address

cpyloop:	lb	$a1,0($a0)			# copy bytes
		addiu	$a0,$a0,1
		addiu	$t2,$t2,-1
		sb	$a1,0($t1)
		bne	$t2,$0,cpyloop
		addiu	$t1,$t1,1

		j	swdloop
		nop

#
# All done, return OK.
#

swddone:	ld	$31,($sp)

		sw	$t0,pub__LoadCur
		sw	$t1,pub__SaveCur

		add	$v0,$0,$0

		j	$31
		addiu	$sp,$sp,8

#
# Read a number of bits from the input.
#

		.set	noat				# (GNU as: explicit use of at)
swdreadbits:	beq	$t4,$t5,swdloadbits		# bit buffer empty ?
		and	$at,$t4,$t5

		sll	$v0,$v0,1
		or	$v0,$v0,$at

		addi	$v1,$v1,-1

		bne	$v1,$0,swdreadbits		# bit count finished ?
		srl	$t4,$t4,1

		j	$31
		nop

swdloadbits:	lbu	$t4,($t0)				# load bit buffer
		addiu	$t0,$t0,1
		ori	$t4,$t4,256			# set end marker

		and	$at,$t4,$t5

		sll	$v0,$v0,1
		or	$v0,$v0,$at

		addi	$v1,$v1,-1

		bne	$v1,$0,swdreadbits		# bit count finished ?
		srl	$t4,$t4,1

		j	$31
		nop

#
#
#

#		end			# SN: end of source


# **************************************************************************
# **************************************************************************
# **************************************************************************

