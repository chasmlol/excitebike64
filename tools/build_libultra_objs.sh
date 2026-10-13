#!/bin/sh
# Build the objects tools/build_libultra.py links into the images, with KMC GCC 2.7.2:
#   $OUT/ultralib/src/**.o   decompals/ultralib, VERSION=K (2.0K) TARGET=libgultra_rom
#   $OUT/libgcc/*.o          src/libgcc/libgcc2.c (64-bit division, __clz_tab), see tools/build_libgcc.sh
# Usually run as `python tools/build_libultra.py --objects`, which sets (as Linux paths):
#   ULTRALIB    decompals/ultralib checkout (commit e24c836)
#   KMC_GCC     KMC GCC 2.7.2 + binutils 2.6 for Linux, extracted into one folder (gcc, cc1, cpp, as, ar, ...):
#               the gcc-2.7.2-linux / binutils-2.6-linux archives of decompals/mips-gcc-2.7.2 and
#               decompals/mips-binutils-2.6 (what ultralib's own setup downloads into its tools/gcc)
#   OUT         output folder (build/libultra)
#   LIBGCC_SRC  folder holding libgcc2.c (src/libgcc)
# Needs make, rsync, cpp and GNU binutils for MIPS (mips-linux-gnu-*).
# KMC's tools are static i386 programs whose stat() fails with EOVERFLOW on Windows drives mounted in WSL (/mnt/c),
# so the build runs in a copy on the Linux filesystem ($LIBULTRA_WORK, default ~/excitebike64-libultra).
set -e
: "${ULTRALIB:?}" "${KMC_GCC:?}" "${OUT:?}" "${LIBGCC_SRC:?}"
HERE=$(cd "$(dirname "$0")" && pwd)
W=${LIBULTRA_WORK:-$HOME/excitebike64-libultra}
case $W in "~"/*) W="$HOME/${W#"~/"}" ;; esac
case $W in /*) ;; *) echo "LIBULTRA_WORK must be an absolute Linux path: $W"; exit 1 ;; esac
mkdir -p "$W"
rsync -a --delete --exclude /build --exclude /.git --exclude /tools/gcc "$ULTRALIB/" "$W/"
mkdir -p "$W/tools/gcc"
cp -r "$KMC_GCC/." "$W/tools/gcc/"
chmod +x "$W/tools/gcc/"*
# COMPARE=0: no Nintendo archive to compare against. src/monutil.s (rmon, assembled with IDO) is not part of
# libgultra_rom; its rule fails without IDO, -k builds everything else.
make -C "$W" VERSION=K TARGET=libgultra_rom COMPARE=0 -j8 -k >"$W/make.log" 2>&1 || true
n=$(find "$W/build/K/libgultra_rom/src" -name '*.o' | wc -l)
if grep -v 'monutil' "$W/make.log" | grep -q 'Error'; then grep 'Error' "$W/make.log"; exit 1; fi
mkdir -p "$OUT/ultralib"
rsync -a --delete "$W/build/K/libgultra_rom/" "$OUT/ultralib/"
mkdir -p "$W/libgcc"
cp "$LIBGCC_SRC/libgcc2.c" "$HERE/build_libgcc.sh" "$W/libgcc/"
(cd "$W/libgcc" && rm -rf out && VR4300MUL=ON sh build_libgcc.sh "$W/tools/gcc" out)
mkdir -p "$OUT/libgcc"
rm -f "$OUT/libgcc/"*.o
cp "$W/libgcc/out/"*.o "$OUT/libgcc/"
echo "ultralib 2.0K libgultra_rom: $n objects; libgcc: $(ls "$OUT/libgcc" | wc -l) objects"
