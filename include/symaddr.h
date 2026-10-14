/* A symbol's address as a number, in the version being built: SYM_ADDR(sSndRaceLoaded). For the few places where
   the original code is reproduced only with a constant address (a base register the compiler keeps across calls, an
   address in inline asm). The values come from the version's symbols.txt: the build writes them to
   build/<version>/include/symbol_addresses.h (tools/common.py), so these places follow the symbol table and
   tools/rename.py like any other use of the name. */
#ifndef SYMADDR_H
#define SYMADDR_H

#include "symbol_addresses.h"

#define SYM_ADDR(sym) SYMADDR_##sym
/* The halves a lui/lo pair gives: the upper half rounded for a signed low half, and that low half. */
#define SYM_HI(sym) ((SYM_ADDR(sym) + 0x8000) & 0xFFFF0000)
#define SYM_LO(sym) ((s32)((SYM_ADDR(sym) & 0xFFFF) ^ 0x8000) - 0x8000)
/* SYM_ADDR as a string, for inline asm: "0x800D1E0C" */
#define SYM_STR(sym) SYM_STR_(SYM_ADDR(sym))
#define SYM_STR_(x) SYM_STR__(x)
#define SYM_STR__(x) #x

#endif
