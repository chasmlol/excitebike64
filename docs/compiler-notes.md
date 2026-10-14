# Compiler notes

How the original code was built, and how the code generation of SN64's GCC 2.8.1 (`-O2`) maps back to C. Function
names are examples in this repository where the pattern can be seen.

## Toolchains

- **Game image:** SN Systems SN64, `cc1n64` (GNU C 2.8.1-based) with `-O2 -G0 -mcpu=vr4300 -mips3 -mgp32 -mfp32`,
  assembled by SN's `asn64` into Psy-Q LNK objects. MIPS III code with 32-bit registers and the o32 calling
  convention: arguments in `$a0-$a3` (floats in `$f12`, `$f14`), results in `$v0` / `$f0`. No `$gp` (`-G0`): every
  global is a `lui`/`%lo` pair. `char` is **unsigned** by default (`lbu`); `s8` for signed bytes.
- **Boot loader** (`boot_func_*`, a separate program also loaded at 0x80000400): SN64 GNU C 2.7.2, build 0006 for
  most functions, build 0001 for 8 of them and 2.8.1 for 15; functions.csv names the profile of each. 2.7.2's
  epilogues end `addiu $sp; jr $ra; nop`.
- **libultra:** Nintendo's `libgultra_rom` 2.0K, built with KMC GCC 2.7.2 (rebuilt here from decompals/ultralib).
  The 64-bit division routines come from KMC's `libgcc.a` (src/libgcc/libgcc2.c): KMC's big-endian build got the
  little-endian word order, so `__divdi3` and friends work on word-swapped values. The rewrite keeps that bug.
- Preprocessor defines (tools/sn64.py): `F3DEX_GBI_2 _FINALROM NDEBUG _MIPS_SZLONG=32 _MIPS_SZINT=32 _MIPS_SIM=1
  _ULTRA64 _LANGUAGE_C _MIPSEB`, plus the SN predefines (`-D_PSYQ`, `-Dn64`, ...). `cppn64` runs with `-undef`.

## Code shapes that are plain C

- Branch-likely instructions (`beql`, `bnel`); `j` for gotos and loop jumps; `div`/`divu` followed by
  `bnez .. break 7` and the `-1`/`0x80000000` `break 6` check (that is just `/` or `%`); `addu $x, $y, $zero` is a
  move; prologue stores scheduled into the body.
- A function that never returns (`break 0x407` halts: fatal_halt) makes its callers end right after the `jal`.

## Names, globals and literals

- Every name resolves through symbols.txt, per image (libultra's `osRecvMesg` is 0x800B0610 in the game, 0x80008030
  in the boot loader). libultra's names are the library's (libultra_map.json; `sqrtf` is what GCC's
  `__builtin_sqrtf` calls as its NaN fallback). The default names are `func_XXXXXXXX` (game), `boot_func_XXXXXXXX`
  (boot loader) and `D_XXXXXXXX`.
- The C calls libultra by its own names (libultra_map.json), with the prototypes of `<ultra64.h>`;
  include/functions.h declares the internal functions the game calls (`__osDisableInt`). A function whose code
  depends on its own prototype calls a second name (`osAiGetStatus_2`, an `s32` where libultra returns `u32`).
- Globals are `extern` (include/variables.h, or the source file when the type is local to it). Only src/data defines
  storage.
- A `lui`/`%lo` pair may form a struct's base with the field offset in the instruction
  (`addiu $v0, $v0, -0x5BC0 ; sBikeCrashContactPoints` then `swc1 $f0, 0x3C($v0)`): use the base symbol with a struct or array,
  not a separate extern for the field.
- Literals written in the C (floats and doubles, strings, const tables, switch jump tables) land in each object's
  `.rdata`; tools/build_code.py places them at the address the original's `lui`/`%lo` pair forms, so their bytes
  must equal the original's there. Write `0.1f`, `"text"` and `switch` normally, not as externs.
- Images: game code 0x80000400-0x800C2280, data to 0x800DC098, BSS 0x800DC0A0-0x80105580. Boot loader code
  0x80000400-0x8000CE10 and 0x800113E4-0x80011600, data 0x8000CE10-0x800113E4, BSS 0x80011600-0x8001B610.
- Original file names survive as strings (`__FILE__` arguments): they are the module names in functions.csv and the
  folders under src/game (`main`, `render`, `bikenew`, `edit`, `height`, `tunes`, `shell`, `nesn64` = the built-in
  NES emulator, `n64snd` = the sound code, ...). Groups of functions between two object-file boundaries that name
  no file are under src/game/unnamed, named after their segment and start address.

## Display lists (F3DEX2 GBI)

common.h includes `<ultra64.h>` with `F3DEX_GBI_2`: the game's display lists use F3DEX2 command bytes. They are
built with the SDK macros on a post-incremented `Gfx *`: `gDPPipeSync(gdl++);`,
`gDPSetPrimColor(gdl++, 0, 0, r, g, b, 255);`, `gDPFillRectangle(gdl++, ulx, uly, lrx, lry);`,
`gDPLoadTLUT_pal256(gdl++, pal);`, `gSPDisplayList(gdl++, dl);`, `gSPEndDisplayList(gdl++);`. Recognise them by the
command byte in `w0`: 0xE7 pipe sync, 0xFA prim color, 0xF6 fill rect, 0xFD set texture image, 0xDE display list,
0xDF end, 0x01 vertex, 0xD7 texture, 0xDB moveword (gSPSegment), 0xE3/0xE2 othermode (gDPSetTextureLUT,
gDPSetCycleType, gDPSetRenderMode), 0xD9 geometry mode, 0xDA matrix.

- Try first a `#define PKT(a,b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }` macro,
  `gdl = f(gdl, ..)`, globals inline; else `gN = gdl++; ((u32 *)gN)[0] = w0; [1] = w1;` per packet, `w0` first;
  pointer locals `Blk *pN = gdl++;` in order with a `u32 cN = K;` between them to place a `lui`; SDK macros, texrect
  E4/E1/F1 by hand.
- A returned-gdl copy for the stores is `{ Gfx *_g = gdl++; .. } return gdl;` (des_draw_arrow). Multi-packet SDK
  macros reuse one `_g` (gSPLoadUcode): copy them from gbi.h (render_load_ucode). PKT if/else: the fall-through arm as a
  plain `p0 = gdl++;`, one `return gdl;` (des_tex_mode_callback). A display-list parameter reassigned per call keeps one
  pseudo. Decode `w0`/`w1` pairs into SDK macros (screen_draw_wipe).

## Globals, constants, fixed addresses

- `lui $at` + `lwc1/ldc1 ($at)` is a float/double literal in the C (`x * 0.0625f`), not an extern. A 2^31 compare
  and sub/trunc/`or 0x80000000` arms are a float<->unsigned conversion: assign the float to a u16/u32 field, or cast
  to s32 first.
- A global loaded via `lui $vN` + `%lo($vN)` is an extern `D_XXXXXXXX`. Data addresses passed as arguments: an extern
  array (`extern char sBootLogFileName[]`), not `(char *)0x80011240` (that gives `ori`).
- The same address needed twice, or two separate `lui`s for it: a second extern name ending in the same
  `_XXXXXXXX`. A global stored, its address passed only later: `p = &D_X; *p = f(); g(p)`.
- Two reads off one constant base (`h = (s32 *)0x800E0000`) are CSE'd into one register and lose each read's own
  `lui` in a delay slot: a plain `extern s32 gPlayerInputs;` per read (event_update_gates); a numeric `*(s32 *)0x800D33F8`
  is one `lw` macro whose `lui` reorg never moves into a slot.
- `D_X - 1` works. `__builtin_sqrtf(x)` is the sqrt with its `jal sqrtf` fallback; a hand-written NaN check gives
  two `jal`s. A pointer to B reused as B-0x30, or hoisted addresses chained off each other: declare the lowest symbol
  (one struct array with pad fields) and use `D_X + 12` (main_sched_event_thread). `D_X[idx*64 + 0x28]` folds 0x28 into the
  symbol: a typed struct `D_X[idx].flags` does not.
- Stack string copies: `strcpy(buf, "y=")`; an odd-size `lwl/lwr` + `lh` + `lb` copy from .rdata is
  `strcpy(buf + 2, " Place")` (event_set_place_text); a 7-byte `lwl/lwr` copy: memcpy + a hand-written tail; 16-byte
  copies: `typedef struct { s32 w[4]; }`; runtime `(src|dst)&3` copy loops: `__builtin_memcpy`.
- A global's `lui sN` right after the `jal` that produced its value: `G = (s32)f(..);` with a cast.

## NES emulator (module nesn64, state at 0x8038xxxx-0x8039xxxx)

- One `lui $aN, 0x8039` reused by several accesses: `u8 *nes = (u8 *)0x80390000; nes[0x4102]...`; a typed
  `Cpu *cpu` there when stores must stay ordered before loads.
- A separate `lui $at` per store / `lui $v1; lbu $v1, ($v1)` per load: numeric pointers `*(u8 *)0x80394105 = v`.
- RAM reads `lui 0x8038` + `addu`: `*(u8 *)(addr + 0x80380000)` with an s32 index.
- Otherwise one struct: `typedef struct { u8 ram[0x10000]; u8 pad[0x4100]; u16 pc; u8 a,x,y,s; u8 v,b,n,d,i,z,c,p;
  } Nes; #define NES ((Nes *)0x80380000)`, `NES->ram[0x100 + NES->s] = v` (nes_op_brk; also the APU code). The
  opcode handlers are near-clones of each other.
- `lbu v; addiu v,v,0x100` where GCC would give `ori` or an `sb` offset: assign twice, `i = nes[0x4105]; i += 0x100;`
  (pseudos set twice are not combined) (nes_op_jsr).

## Floats

- `mtc1 $zero, $fN` + `swc1` to two fields: chained `p->x = p->z = 0.0f;`; several zero stores: a local
  `f32 z = 0.0f;` stored first (a bare `= 0.0f` gives `sw $zero`).
- Prototype float callees with f32 (`void g(s16, f32)`), or the arguments are promoted to double. An early
  `return 0.0f` for negative input. An int global in float math: `*(volatile s32 *)&D_X`.
- FP register allocation: a result in its dead input's register means that variable is reused
  (`sm = __builtin_sqrtf(sm)`); in the mul temp's register or with operands swapped: a fresh variable; split
  `r = res * h; r = r / sqrtf(..)`.
- Float registers follow which floats are function-level variables (the same odd register in both arms) and which are
  block temps (`$f0`/`$f2`); `f1 = h1; f1 -= f0;` keeps an in-place tie (height_get_desert_f).

## Calls and prototypes

- A `jal` with `addiu $a0,$zero,1` in its delay slot to a void(void) callee: declare it `void f();` and pass the
  value. An extra `addu $a1,$zero,$zero`: a second argument; `$a3` never set: 3 arguments.
- `$v0 = $a0` before a null test and never set again: the function returns its pointer argument.
  `jal F; addu s0,v0,zero` saves the result of the call before F: `r = G(); F(a); return r;`.
- Function-pointer tables: `((s32 (*)(s32,...))G[5])(...)`. A function that ends right after a `jal` (no `lw ra`/`jr`)
  calls a `__attribute__((noreturn))` callee (fatal_halt halt, main_hang, fatal_halt_mem_report); a constant set
  before the `if` goes after it. A u16 result used with no `andi`: declare `s32 f(u32);`; a u8 parameter adds an
  `andi 0xFF` the original lacks: declare it s32 (pak_eeprom_read).
- The wrong instruction in a branch-to-epilogue delay slot (the skipped one writes `$v0`): `s32 f()` with no return,
  `$v0` is live at the end (edit_cam_step). `sw/sw 0x10,0x14(sp)` before a `jalr` with `$a0-$a3` untouched: the
  function's own arguments 5 and 6 passed on (sfx_init).
- Read-backs: `addu a1,s0` / `mov.s` copies or a reload before stores to other fields are a just-stored field read
  again, `e->w10 = e->w15;` (dma_start). A local named like a parameter shadows it.
- A register read but never written: an uninitialised local in the original; the bug is kept.
- Repeated blocks, or a block with its own early exit: a `static inline` helper (needs `inline`; gotos get threaded,
  inline returns do not); a spilled parameter passed as `chk(&param)`; a stack array passed as a helper parameter
  (CSE cannot merge its address). Per-arm constants after reloaded globals: ternary `#define`s. One `jal` shared by
  both arms: write the call in both, s32 locals assigned straight from the float expression (particles_project_point).
- Prototype `void f(s32)` so a constant's `li` lands in the delay slot.

## Scheduling and `lui` order

- A global's `lui` at the very top: take its address first, `s32 *dst = &D_X;` ... `*dst = v`.
- A store hoisted above independent accesses: make the earlier stores volatile. A global pointer reloaded after a
  store: `G->field` each time; kept ahead of the stores: a local copy just before them.
- Scalar-global loads hoist above struct-field stores: `extern s32 G[]; G[0]` keeps the reload after them, `*G` lets
  it move above; typed-pointer reads or a volatile store also hold the order. Stores to distinct symbols reorder:
  move one up; loads in one call's arguments: volatile reads there only.
- A constant materialised before a pointer copy or `lui`: a local set at that spot (`m = 0xfffffff7;`, `m = -1;`);
  read the global into a local first, or write the base as the first statement, to put its `lui` ahead; a hi base
  after the counter init: `hb = (u8 *)0x800C0000;`. A copy in a `beqz` slot goes before the guard.
- Several pointers from one global base: derive them from one local copy. A table base used only in a guarded arm: a
  block-local pointer declared inside it; used only late: `r = &G` after the last early call.
- A delay-slot store before `if (x == 0) return;`: the last statement before the test. A value copied in a `bne`
  slot is assigned in both arms. Load, null test, copy: `nt = e->name; if (nt) { n = nt; ..}`.
- The prologue saves an s-register first (`sw s1,0x14` before `sw ra`): its variable's init goes above the first
  branch, `i = 0;` above `if (count > 0) {` (path_calc_normals). GCC stores `ra`, `s8..s0` by default; the scheduler
  stores a saved register first when its variable is written before the first branch. A `lui` before the parameter
  copy `addu s0,a0,zero`: copy the parameter into a local after that work (collide_reset_tree).
- In-place updates interleave by statement order; the register of each first load says which statement is first;
  permute them (height_get_ground_normal). `do { D_X = 0; } while (0);` puts loop notes around a store, a barrier that keeps it
  right after its `lui`.

## Loops

- A plain `for (i..) D[i].f` over a typed global array or pointer global (`D_X[i].f` in every access, no base locals):
  strength reduction makes the walking pointers, hoisted `lui`s and constants; a pointer set up only after the entry
  test is this form; `p = &D[i]; p->f` keeps the offset unfolded; `((Mtx *)B)[i + 1]` for `(i + 1) * 0x40`; a list
  walk re-reading a pointer global: `D_G->head` in the test.
- Hand-walked cursors lose to indexing: `p += 0x24` plus a counter gives two biased induction variables, so
  `p = (Part *)((s32)base + i * 0x24);` at the top of the body (particles_update); a `lui; addiu; addu tN,v0,zero` base
  copy is a direct `D.b50[i]` with no local pointers (shell_restore_race_results); an offset init after a hoisted `lui` is the giv
  `i*0x18`. Pad the struct to the exact stride (check sizeof), index `D[i].w` (shatter_free); `p = &D[i].e[j]` folds
  the array offset into the j giv (fractal_emit_fire_smoke). A field offset folded into the walking pointer (`lw -4(s0)`): index
  the global array with an s32 counter, or a struct with a pad field first and `&ARR[i]`.
- Counter/pointer register swaps: `for (i = 0, p = ARR; ...; i++, p++)`, one counter for every loop or the second
  loop's own, `i = 0` inside `if (n != 0) {`; an offset skipped on one arm: its own index.
- Smaller cases: strcat with the increment in a `j` slot: a labelled goto; a bit accumulator `r = r << 1`; `% N` for a
  magic multiplication; `D;` for volatile reads; a volatile cast at a return reload.
- Countdown copies: `while (--n != -1) *d++ = *s++;` (`n-- != 0` keeps a copy). Test at the top: `for (;;)` with the
  test in a declaration block; goto loops are not loops to loop.c, hoists need a real `while (1)` (replay_write_bits);
  `break` exits rotate it: `goto insert;` past the loop; `beql a,b,epilogue` at the top is
  `for (;;) { if (a == b) return acc; ..}` (path_get_distance). A for-header increment or `continue`: `beql` with the bump
  in the slot.
- Hoisting tells: a bound copy `addu t0,v1,zero` after the hoisted invariants means the test re-reads the global,
  `n = D_X; if (n > 0) do {..} while (i < D_X);` (render_clear_viewports); an invariant product recomputed in the loop is set in
  two statements, `j = p * 17; j += cnt;` (pak_build_note_list); an outer latch with a fresh `lui; lw` is a real inner
  loop, `while (rem > 0.0f) {..}` (path_calc_curvatures).
- `lw` x4 + `sw` x4 loops and 12/24/32-byte `lw`/`sw` runs are assignments of a typedef'd struct, `D[0] = D[-1];`
  (s16-only members give `lwl/lwr`), as are rotating temps `lw tA,0(p); lw tB,4(p)`, `s4-s7` plus a dead spill, two
  temps for 16 bytes, odd-offset `lwl/lwr` (cam_snap_to_moving); 12 single `lbu`/`sb`: element assignments.

## Branch layout

- Out-of-line case blocks, or `bnel` with a store in the delay slot: cases as gotos to labels.
- Which value is tested first follows operand order: flip `a < b` to `b > a`. Separate pointer variables per arm give
  separate temps. List searches: a separate result pointer, `goto found`. A join register's OR or adjustment: its own
  statement.
- Shared returns as labels (`ret1: return 1;`), early checks `goto ret1`; an early-exit label holding the
  fall-through's store; labelled blocks in source order give the block order.
- The arm the original falls into goes first: `else if (flags & 0x8000) {..} else {body}`, `(flag & 2) != 0` body
  first, a big then-arm with the shared stores in the else; a short else-arm before the join breaks the cross-jump;
  `bnez` where the original has `beqz`: flip the test and the arms; the set-bit path first. Early exits to a shared
  tail: nested if/else-if. An error return behind a `bnez`: the last else arm. `switch` default after the last case.
- Dispatch `beq m==1; beqz m==0; beq 2; beq 3; default`: `if (m==1) goto m1; ..` with the bodies in textual order
  (edit_ease_delta); a `beq 1; slti 2; beqz 0; beq 2` tree is a plain 4-case `switch`, each case in full
  (edit_place_piece_before). `beq x,8 -> v=4; bne x,11 (slot v=3)` is `if (n == 8 || n == 11) n = 4; else n = 3;`; a `bne` slot
  holding the else value: `if (G == 2) z = y + 0x2C; else z = y + 0x20;` (event_draw_hud_gates).
- Shared tail blocks reached by goto, or an operation after the join: write it in full in each arm; jump2 merges them
  again and the extra references raise allocation priority (fixes permuted s-registers); sign: a `j` to a tail
  starting with `mflo` plus `sw v0,0x10(sp); lw v0,0x10(sp)` (height_get_desert). A constant materialised again after a
  single-use if-then label: a diamond with the call in both arms (event_format_time). Range folds (`andi; sltiu 2` is
  `f == 16 || f == 17`; `F&1 || F&2` folded into `andi 3`: separate `if (F & 1) goto body;`); gotos around a test stop
  CSE sharing a sign extension.

## Frames

- A frame bigger than the locals: `volatile s32 pad[N];` (frame = N*4 + 0x10 + 8, 8-aligned; a 5-argument call needs
  16 more); a buffer at sp+0x20: `char buf[56]; char *s = buf + 16;`.
- Fatal halts `break 1, 7` + `j .`: `__asm__ volatile("break 0x407"); while (1) {}`.
- Mixed stack locals in the wrong slots: one struct in the original's order; but out-parameters read back before
  record stores are separate scalars `s32 A, B, C;` (struct locals are in-struct MEMs). u16 pairs next to an s32
  out-parameter (any local array/struct takes an 8-byte padded slot): declare the s32s, pass `(u16 *)&w`
  (func_8009DC6C). Address-taken parameters: `&param`. No dead-store elimination on address-taken stack locals. A
  scalar at a fixed stack address never aliases array/struct elements: its load may move above their stores
  (cam_shift_xz).

## Register allocation: variable reuse

- local-alloc ties only pseudos that die once; a variable set twice goes to global-alloc and never ties to a dying
  input. So a result in its input's register (or in a register no input could tie to) means the source reused a
  variable: `v = *src; g = (v>>3)&0xF8; b = (v<<2)&0xF8; v = (v>>8)&8;` (palmod_desaturate_pal); channels in place
  `r += ((c - r) * D) >> 8;`, then `c = (r<<11)|..; c |= *dst & 1;`; `p = (u32)p >> 11` last also stops sched1
  hoisting it; one temp for repeated terms `t = ..; t = t*t; d += t;` (prim_find_nearest_color); `dx = dx*dx + dz*dz;`; an entry
  copy reused later is the parameter reused; a temp in a later counter's register is that counter.
- A parameter kept in `$a0` as a walking pointer but copied to `tN` (giv init `addiu tK,a0,4`): walk a local copy
  (boot_relocate_stub).
- Priority = floor_log2(refs) * refs / live length, refs counted before combine and doubled in loops, so splitting
  `y = a - y` in two raises it; `for` vs `do/while` swaps counters. More cases: one variable for unrelated values
  sharing a register; a new variable for a continued product; a dead variable as the next counter; a single-set
  variable takes chain temps.

## More C forms

- Types: `s32 c` instead of `u8 c` removes an `andi 0xFF` (a u8 halved with `sra` needs an s32 copy); no `& 0x1f` on
  shift counts; `* 65536` vs `<< 16`, `<< 7` vs `* 0x80` allocate differently; a u16 field read into an s32 local
  keeps register-constant ANDs; u16/s16 intermediates change the schedule; `j = j / 4` in place.
- Struct views keep field offsets in the load (`((Blk *)D)[i].f`, not `D[i*32+6]`); raw `*(s16 *)(G + 0x184)`
  arithmetic becomes typed structs with pad fields. `p - ARR` shows as `x * 0xCCCCCCCD >> 5` or an `sll`/`addu`
  chain + `negu` + `sra`.
- Element address order: constant last, `*(s32 *)((u8 *)G + (i << 2) + off)`; base first from a byte-offset form or
  a row pointer `q = p + n * 0x88; *(s32 *)(q + 0xb0)`, or `p = &G[idx]; f(*p)`; offset first from `(s32)base + off`,
  `(i*0x30) + (s32)base` with a local base; else the offset as its own statement.
- Float compares: `if (!(r > 0.0f))` for `c.lt.s` + `bc1fl`; clamps `if (v < lo) v = lo; else if (hi < v) v = hi;`;
  `(f > 0 ? f > K : -f > K)`: a ternary of compares; constants first when their `lwc1` comes first; shared products
  as a local.
- Reloads the original keeps: `extern volatile` on that global or field, e.g. one global read 2-3 times with no store
  between; a value copied to a saved register in a delay slot: copy it right after the load; a final volatile store
  stays out of a `j` slot.

## Inline asm

Six functions keep inline asm because the original has instructions no C produces: the `break 0x407` halts
(fatal_halt, fatal_halt_mem_report, func_8000E9C0), a debug `break 0x400` (sn_host_call), an
`addu $t1, $fp, $zero` (func_8000EA5C) and a library function built by another compiler (dma_cart_rom_init: its
`lui`/`addiu` pairs and a macro `lw` are asm, with a register variable). Five others (func_80019EC0, race_grid_from_standings,
nes_read_io, sfx_play, dma_epi_start) hold empty asm statements (`__asm__("" : "+r"(p))`, memory clobbers)
that force an allocation or an order the plain C did not reproduce; replacing them with plain C is open work.
`register` alone does nothing.

## Looking inside cc1n64

`cc1n64 -dL` (loop.c 'moved' / 'not desirable'), `-dl` (refs), `-dg` (conflicts), `-dS -dR` (scheduling) on the
preprocessed `.i` file explain hoists and register choices.
