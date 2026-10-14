"""Build the game's initialized data from C (src/data/*.c) and write it into a build's images.

Every src/data/*.c names its image in a header comment (`image: game` or `image: boot`) and defines symbols that
symbols.txt places. Each file is compiled with the SN compiler (tools/sn64.py; profile sn281 for the game,
sn272_0006 for the boot loader) through a wrapper that also records sizeof() of every symbol the file defines; each
symbol's bytes are taken from the object, its relocations (REL32 words: `(u32)func_80012340`,
`(u32)D_800D5DD8 + 4`, ...) resolved by name through symbols.txt (and libultra_map.json for libultra's names), and
written at its address in OUT/<image>.bin. The ranges in FROM_ROM are not built from C (a bitmap, compressed data):
the C only declares them extern, and they are copied from the extracted images. Then the written bytes are compared
with the originals.

usage: python tools/build_data.py [OUT] [--check] [--version NAME]    (default OUT: build/<version>/out)
  --check   compile and compare only, leave OUT untouched
Exit status 1 if a byte differs or a symbol can't be placed.
"""
import argparse
import re
import struct
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
import sn64  # noqa: E402
from snobj import LnkFile  # noqa: E402

ROOT = common.ROOT
SRC = ROOT / "src" / "data"
VRAM = common.VRAM
IMAGES = common.IMAGES
PROFILE = {"game": "sn281", "boot": "sn272_0006"}
DATA_RANGES = common.DATA_RANGES

# Data bytes that are not C: RSP microcode data sections (Nintendo's F3DEX2 xbus 2.08 binaries, the DMEM image of
# the microcode whose text is in the code; version.toml microcode_data). They stay as in the original images, like
# the microcode text.
NOT_C = {k: [tuple(r) for r in common.layout(k)["microcode_data"]] for k in IMAGES}
# Ranges not built from C (version.toml from_rom): its src/data file declares it extern (rom_decl) and the build
# copies it from the extracted image. (start, end, symbol, element type, what it is)
FROM_ROM = {k: [tuple(r) for r in common.layout(k).get("from_rom", [])] for k in IMAGES}


def rom_decl(lo, hi, name, ctype, what):
    """The lines a src/data file has for a FROM_ROM range."""
    n = (hi - lo) // {"u8": 1, "u16": 2, "u32": 4}[ctype]
    return [f"/* {lo:#010x}-{hi:#010x} ({hi - lo} bytes): {what}, copied from the ROM at build time */",
            f"extern {ctype} {name}[{n:#x}];"]


def resolver(image):
    """name -> address in the given image (symbols.txt, libultra's names)."""
    return common.resolver(image)


def file_image(path: Path) -> str:
    m = re.search(r"\bimage:\s*(\w+)", path.read_text()[:2000])
    if not m or m.group(1) not in IMAGES:
        sys.exit(f"{path.name}: needs an `image: game` or `image: boot` header comment")
    return m.group(1)


def _compile(src: Path, prof: dict, td: Path, tag: str, extra: str = ""):
    """Compile a wrapper that #includes src (so its own "..." includes resolve in src/data) plus `extra`."""
    w = td / f"{tag}.c"
    w.write_text(f'#include "{src.name}"\n{extra}', newline="\n")
    ok, msg = sn64.compile(prof, w, td / f"{tag}.o", [src.parent, ROOT / "include"])
    if not ok:
        raise RuntimeError(f"{src.name}: {msg.strip()[:2000]}")
    return LnkFile((td / f"{tag}.o").read_bytes())


def defined(lnk):
    """[(name, section index, offset, kind)] of the data symbols an object defines (exports, statics, commons)."""
    out = []
    for s in list(lnk.symbols.values()) + lnk.locals:
        if s.kind not in ("export", "local", "common") or s.name.startswith(("$", "data_sizes_")):
            continue
        if s.name in ("gcc2_compiled.", "__gnu_compiled_c"):
            continue
        sec = lnk.sections.get(s.section)
        if s.kind != "common" and (sec is None or sec.name == ".text"):
            continue
        out.append((s.name, s.section, s.offset, s.kind))
    return out


def compile_file(src: Path):
    """-> (image, [(name, address, bytes, [(offset, target name/expr text)])]); raises on problems."""
    image = file_image(src)
    prof = common.profile(PROFILE[image])
    with tempfile.TemporaryDirectory() as td:
        td = Path(td)
        names = [n for n, *_ in defined(_compile(src, prof, td, "a"))]
        if not names:
            return image, []
        sizes = "unsigned int data_sizes_[] = {\n" + "".join(f"\tsizeof({n}),\n" for n in names) + "};\n"
        lnk = _compile(src, prof, td, "b", sizes)
    syms = {n: (sec, off, kind) for n, sec, off, kind in defined(lnk)}
    tab = next(s for s in lnk.symbols.values() if s.name == "data_sizes_")
    size_of = {n: struct.unpack_from(">I", lnk.sections[tab.section].data, tab.offset + 4 * i)[0]
               for i, n in enumerate(names)}
    res = resolver(image)
    # section offset -> (name, offset) for section-relative relocations (a symbol of this file)
    spans = {}
    for n, (sec, off, kind) in syms.items():
        spans.setdefault(sec, []).append((off, off + size_of[n], n))
    out, problems = [], []
    for n in names:
        sec, off, kind = syms[n]
        addr = res(n)
        if addr is None:
            problems.append(f"{n}: not in symbols.txt for the {image} image")
            continue
        size = size_of[n]
        s = lnk.sections.get(sec)
        data = bytearray(size) if kind == "common" or s is None or len(s.data) < off + size else \
            bytearray(s.data[off:off + size])
        rels = []
        for o, t, e in (s.relocs if s is not None and kind != "common" else []):
            if not off <= o < off + size:
                continue
            if t not in ("REL32_BE", "REL32") or o % 4 or o + 4 > off + size:
                problems.append(f"{n}+{o - off:#x}: relocation {t} not supported in data")
                continue
            kind_, a, add, sname = sn64.expr_ref(lnk, e)
            if kind_ == "sym":
                base, text = res(a), a
            elif kind_ == "abs":
                base, text = a, hex(a)
            else:  # a symbol of this object: section a, offset add
                hit = [(lo, nm) for lo, hi, nm in spans.get(a, []) if lo <= add < hi]
                if not hit:
                    problems.append(f"{n}+{o - off:#x}: points into anonymous data of {lnk.sections[a].name}")
                    continue
                lo, nm = hit[0]
                base, text, add = res(nm), nm, add - lo
            if base is None:
                problems.append(f"{n}+{o - off:#x}: {a} has no address")
                continue
            # The expression is the whole value; the linker overwrites the word (asn64 leaves junk in it at times:
            # 0x00800000 in a word at section offset 0x7FFC, where it starts a new data record).
            struct.pack_into(">I", data, o - off, (base + add) & 0xFFFFFFFF)
            rels.append((o - off, f"{text}+{add:#x}" if add else text))
        out.append((n, addr, bytes(data), rels))
    if problems:
        raise RuntimeError(f"{src.name}: " + "; ".join(problems[:10]))
    return image, out


def build_all(files, jobs=8):
    """-> {image: [(file, name, address, bytes, relocs)]}, [errors]"""
    out, errors = {k: [] for k in IMAGES}, []

    def one(f):
        try:
            return f, compile_file(f), None
        except RuntimeError as e:
            return f, None, str(e)
    with ThreadPoolExecutor(jobs) as ex:
        for f, r, err in ex.map(one, files):
            if err:
                errors.append(err)
                continue
            image, syms = r
            out[image] += [(f.name, *s) for s in syms]
    return out, errors


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("build_dir", nargs="?", default=str(common.OUT))
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--jobs", type=int, default=8)
    common.add_version_arg(ap)
    a = ap.parse_args()
    build = common.output_dir(a.build_dir)
    files = sorted(SRC.glob("*.c"))
    if not files:
        sys.exit(f"no .c files in {SRC} (tools/gen_data_c.py writes them)")
    built, errors = build_all(files, a.jobs)
    for e in errors:
        print("error:", e)
    total_diff, bad = 0, False
    for image in IMAGES:
        syms = sorted(built[image], key=lambda x: x[2])
        if not syms:
            continue
        orig = common.original(image)
        path = build / f"{image}.bin"
        img = bytearray(path.read_bytes()) if path.exists() else bytearray(orig) if a.check else None
        if img is None:
            sys.exit(f"missing {path}")
        lo, hi = DATA_RANGES[image]
        keep = NOT_C.get(image, [])
        owner, problems, diff, nbytes, nrel = {}, [], [], 0, 0
        for fname, n, addr, data, rels in syms:
            end = addr + len(data)
            if not (lo <= addr and end <= hi):
                problems.append(f"{fname}: {n} {addr:#x}+{len(data):#x} is outside the {image} data {lo:#x}-{hi:#x}")
                continue
            if any(k0 < end and addr < k1 for k0, k1, _ in keep):
                problems.append(f"{fname}: {n} overlaps microcode data")
            if any(k0 < end and addr < k1 for k0, k1, *_ in FROM_ROM.get(image, [])):
                problems.append(f"{fname}: {n} overlaps data copied from the ROM")
            for x in range(addr, end):
                if x in owner:
                    problems.append(f"{fname}: {n} overlaps {owner[x]} at {x:#x}")
                    break
                owner[x] = n
            img[addr - VRAM:end - VRAM] = data
            nbytes += len(data)
            nrel += len(rels)
            d = [addr + i for i in range(len(data)) if data[i] != orig[addr - VRAM + i]]
            diff += [(x, n, addr) for x in d]
        rom_bytes = 0
        for k0, k1, *_ in FROM_ROM.get(image, []):
            img[k0 - VRAM:k1 - VRAM] = orig[k0 - VRAM:k1 - VRAM]
            rom_bytes += k1 - k0
        for p in problems[:20]:
            print("  problem:", p)
        print(f"{image}: {len(syms)} data symbols from {len({s[0] for s in syms})} C files, {nbytes} bytes "
              f"({nrel} relocated words); {len(diff)} bytes differ from the original; "
              f"{rom_bytes} bytes copied from the ROM")
        for x, n, addr in diff[:12]:
            print(f"    {x:#010x} ({n}+{x - addr:#x}): {img[x - VRAM]:02x}, original {orig[x - VRAM]:02x}")
        total_diff += len(diff)
        bad |= bool(problems)
        if not a.check:
            path.write_bytes(bytes(img))
    if errors or bad or total_diff:
        sys.exit(1)


if __name__ == "__main__":
    main()
