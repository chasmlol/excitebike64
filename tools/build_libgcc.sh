#!/bin/sh
# Build the libgcc objects Excitebike 64 links (64-bit division + __clz_tab) from libgcc2.c with KMC GCC 2.7.2.
# Run from a folder holding libgcc2.c, on the Linux filesystem (the static i386 tools fail with EOVERFLOW on
# Windows drives mounted in WSL); tools/build_libultra_objs.sh does this:
#   sh build_libgcc.sh <folder with KMC gcc, cc1, as> <outdir>
# -Wa,-O00: gas 2.6 reads -O0 as -O2 (it checks the 2nd character); -O00 gives its mips_optimize=1 (precise
# hazard nops, no branch-delay swapping: the original epilogues end "addiu sp; jr ra; nop").
set -e
TOOLS=$1; OUT=$2
mkdir -p "$OUT"
for f in divdi3 udivdi3 udivmoddi4 umoddi3 clz; do
  COMPILER_PATH=$TOOLS "$TOOLS/gcc" -c -nostdinc -O2 -mips3 -mgp32 -mfp32 -G 0 -Wa,-O00 \
    -DIN_LIBGCC2 -DL_$f libgcc2.c -o "$OUT/_$f.o"
done
