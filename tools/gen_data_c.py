"""Generate C for the game's initialized data (src/data/*.c), mechanically, from the original images.
Needs rabbitizer (the code's accesses to each address come from listings of the original functions).

1. Which bytes need C. Every function of functions.csv and the DATA_RANGES (minus build_data.NOT_C, the RSP
   microcode data) are poisoned, twice (0xA5, then 0x5A), and build_code.py, build_asm.py and build_libultra.py
   run on the result: bytes that stay poisoned both times are written by no other step. Saved in
   build/data_left.json (--reuse skips the builds).
2. Symbols. One starts at every leftover run's start, at every address of the same image the code names
   (symbols.txt, libultra_map.json) and at every address a data word points to.
   A symbol ends where the next one starts. (The listings' `; D_` addresses only tell how an address is accessed:
   their lui/lo pairing is a linear scan that crosses branches and names addresses inside strings.)
3. Relocations. A 4-aligned word whose value is an address of the image becomes `(u32)name` (`(u32)&name` for a
   scalar) when it points into this data or .bss, at a function start (functions.csv, libultra's real names;
   also the three `jr $ra` stubs split.py lists as linker padding), at an address the code names, at microcode
   data, or at a string another step builds. Anything else (inside a function: other code's case labels, masks
   like 0x80040020; the middle of libultra data) stays a number.
4. Types. NUL-terminated printable text -> `char D_X[n] = "..."` (each 4-aligned string its own symbol); f32/f64
   when the C declares it so, or the listings load it with lwc1/ldc1 and every element is a plausible float;
   otherwise the element type the C declares (u8/s8/char, u16/s16), else the listings' access width (lb/lh/lw,
   signed loads -> s8/s16), else u32/u16/u8 by alignment. 1-element symbols are scalars. Trailing zeros are left to
   the array size, zero runs of 32+ bytes skipped with [index] designators.
5. ROM content (build_data.FROM_ROM) is declared extern instead: the build copies it from the extracted images.
6. Check: every file is compiled (build_data.py's compile) and each symbol compared with the original bytes; a
   float symbol that doesn't reproduce is regenerated as u32.
Output: include/data.h and one file per block of leftover runs (runs less than 0x400 bytes apart):
src/data/<image>_<start>.c. Existing src/data/*.c files are replaced. A symbol takes the name symbols.txt has for its
address, else D_XXXXXXXX, which is added to symbols.txt.

usage: python tools/gen_data_c.py [--reuse]
"""
import argparse
import bisect
import json
import re
import struct
import subprocess
import sys
import tempfile
from collections import defaultdict
from decimal import Decimal
from functools import lru_cache
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
import build_data as BD  # noqa: E402
import sn64  # noqa: E402

ROOT = common.ROOT
VRAM = BD.VRAM
IMAGES = BD.IMAGES
SRC = BD.SRC
LEFT_JSON = common.BUILD / "data_left.json"
STEPS = ["build_code.py", "build_asm.py", "build_libultra.py"]
PRINTABLE = set(range(0x20, 0x7F)) | {0x09, 0x0A, 0x0D}
GROUP_GAP = 0x400


def runs_of(addrs):
    out = []
    for a in sorted(addrs):
        if out and out[-1][1] == a:
            out[-1][1] = a + 1
        else:
            out.append([a, a + 1])
    return [tuple(r) for r in out]


def in_not_c(image, a):
    return any(lo <= a < hi for lo, hi, _ in BD.NOT_C.get(image, []))


# ---------------------------------------------------------------- 1. leftover bytes
def leftover():
    table = common.functions()
    orig = {k: common.original(k) for k in IMAGES}
    finals, writer = {}, {k: {} for k in IMAGES}
    with tempfile.TemporaryDirectory() as td:
        for poison in (0xA5, 0x5A):
            out = Path(td) / f"p{poison:02x}"
            out.mkdir()
            img = {k: bytearray(v) for k, v in orig.items()}
            for f in table:
                o = f["address"] - VRAM
                img[f["image"]][o:o + f["size"]] = bytes([poison]) * f["size"]
            for k, (lo, hi) in BD.DATA_RANGES.items():
                for a in range(lo, hi):
                    if not in_not_c(k, a):
                        img[k][a - VRAM] = poison
            for k in IMAGES:
                (out / f"{k}.bin").write_bytes(bytes(img[k]))
            prev = {k: bytes(img[k]) for k in IMAGES}
            for step in STEPS:
                args = [out, "--base", out, "--jobs", "8"] if step == "build_code.py" else [out]
                r = subprocess.run([sys.executable, str(HERE / step), *map(str, args)], cwd=ROOT,
                                   capture_output=True, text=True)
                if r.returncode and step != "build_libultra.py":   # libultra exits 1 while code junk differs
                    sys.exit(f"{step} failed:\n{(r.stdout + r.stderr)[-3000:]}")
                cur = {k: (out / f"{k}.bin").read_bytes() for k in IMAGES}
                for k, (lo, hi) in BD.DATA_RANGES.items():
                    for a in range(lo, hi):
                        if cur[k][a - VRAM] != prev[k][a - VRAM]:
                            writer[k][a] = step
                prev = cur
            finals[poison] = prev
    res = {"left": {}, "written": {}}
    for k, (lo, hi) in BD.DATA_RANGES.items():
        left = [a for a in range(lo, hi) if not in_not_c(k, a) and finals[0xA5][k][a - VRAM] == 0xA5
                and finals[0x5A][k][a - VRAM] == 0x5A]
        res["left"][k] = runs_of(left)
        by_step = defaultdict(list)
        for a, s in writer[k].items():
            if a not in set(left):
                by_step[s].append(a)
        res["written"][k] = {s: runs_of(v) for s, v in by_step.items()}
    return res


# ---------------------------------------------------------------- 2. what the code references
def code_refs():
    """-> names {image: {address}}: the symbols the code names (the C, symbols.txt, libultra_map.json);
    ops {image: {address: {mnemonic}}}: how the original functions access an address (listings' `; D_` comments)."""
    names = {k: set() for k in IMAGES}
    ops = {k: defaultdict(set) for k in IMAGES}
    decls = {k: defaultdict(set) for k in IMAGES}
    sources = {(fn["image"], fn["source"]) for fn in common.functions() if fn["kind"] == "c"}
    sources |= {("game", "include/variables.h"), ("boot", "include/boot.h")}
    for k, src in sorted(sources):
        text = (ROOT / src).read_text(errors="replace")
        for a, cat in extern_decls(text, common.symbols()[k]):
            decls[k][a].add(cat)
    lib = json.loads((ROOT / "libultra_map.json").read_text())["images"]
    for k in IMAGES:
        names[k] |= {int(v, 16) for v in lib[k]["symbols"].values()}
        names[k] |= {s.address for s in common.symbols()[k].values()}
    # The listings' `; D_` comments pair each low half with the last lui of its register in a linear scan, which
    # crosses branches (race_update +0x538 reads gResourceSet; its comment says D_800D7728, inside a string):
    # they tell how an address is accessed (lwc1, lhu, ...), not where symbols start.
    for fn in common.functions():
        if fn["kind"] == "padding":
            continue
        k = fn["image"]
        for _, t in sn64.target_lines(k, fn["address"], fn["size"]):
            m = re.match(r"(\w+(?:\.\w+)?) .* ; D_([0-9A-F]{8})$", t)
            if m:
                ops[k][int(m.group(2), 16)].add(m.group(1))
    return names, ops, decls


CATS = {"u8": "u8", "unsigned char": "u8", "s8": "s8", "signed char": "s8", "char": "char", "u16": "u16",
        "unsigned short": "u16", "s16": "s16", "short": "s16", "f32": "f32", "float": "f32", "f64": "f64",
        "double": "f64", "s32": "s32", "int": "s32", "u32": "u32", "unsigned int": "u32", "unsigned": "u32"}


def extern_decls(text, symtab):
    """(address, category) for each `extern T name...;` declarator of a symbol: u8 s8 char u16 s16 f32 f64 s32 u32,
    ptr (pointers, function pointers) or other (structs, typedefs)."""
    for m in re.finditer(r"^\s*extern\s+([^;{}]*);", text, re.M):
        t = re.match(r"((?:(?:const|volatile|unsigned|signed|struct|union|enum)\s+)*[A-Za-z_]\w*)\s*(.*)$",
                     m.group(1), re.S)
        if not t:
            continue
        base = re.sub(r"\b(const|volatile|struct|union|enum)\s+", "", t.group(1)).strip()
        decl, depth, cur = [], 0, ""
        for ch in t.group(2):
            depth += 1 if ch in "([" else -1 if ch in ")]" else 0
            if ch == "," and depth == 0:
                decl.append(cur)
                cur = ""
            else:
                cur += ch
        decl.append(cur)
        for d in decl:
            n = next((m for m in re.finditer(r"[A-Za-z_]\w*", d) if m.group(0) in symtab), None)
            if n:
                yield symtab[n.group(0)].address, "ptr" if "*" in d[:n.start()] else CATS.get(base, "other")


def known_names(image):
    """address -> name: libultra's real names, then functions.csv's function names. Entries split.py calls
    linker padding count too: data points at three of them (nes_am_immediate_stub, nes_am_relative_stub, n64snd_stub2 are
    `jr $ra; nop`, empty functions only reached through function-pointer tables)."""
    names = {}
    for f in common.functions():
        if f["image"] == image:
            names[f["address"]] = f["name"]
    funcs = set(names)
    m = json.loads((ROOT / "libultra_map.json").read_text())["images"][image]
    for e in m["objects"]:
        for n, a in e.get("functions", {}).items():
            names[int(a, 16)] = n
            funcs.add(int(a, 16))
    for n, a in m["symbols"].items():
        names.setdefault(int(a, 16), n)
    return names, funcs


@lru_cache(None)
def _data_names(image):
    out = {}
    for sym in common.symbols()[image].values():
        if sym.alias_of is None and sym.kind == "data":
            out.setdefault(sym.address, sym.name)
    return out


def default_name(image, addr):
    """The name symbols.txt gives a data address (not a second name), else D_XXXXXXXX."""
    return _data_names(image).get(addr, f"D_{addr:08X}")


def add_symbols(syms):
    """List the generated symbols symbols.txt doesn't have yet."""
    tab = common.symbols()
    new = [f"{s.name}  {k}  0x{s.addr:08X}  data  {s.size:#x}" for k in IMAGES for s in syms[k]
           if s.name not in tab[k]]
    if new:
        with open(ROOT / "symbols.txt", "a", newline="\n") as fh:
            fh.write("\n".join(new) + "\n")
        common.symbols.cache_clear()
        common.names_at.cache_clear()
        _data_names.cache_clear()
        print(f"added {len(new)} symbols to symbols.txt")


# ---------------------------------------------------------------- 4. symbols and types
class Sym:
    def __init__(self, addr, size, ctype, items=None, text=None):
        self.addr, self.size, self.ctype, self.items, self.text = addr, size, ctype, items, text
        self.name = f"D_{addr:08X}"

    @property
    def scalar(self):
        return self.ctype != "char" and self.size == self.esize

    @property
    def esize(self):
        return {"char": 1, "u8": 1, "s8": 1, "u16": 2, "s16": 2, "u32": 4, "f32": 4, "f64": 8}[self.ctype]


def f_text(raw: bytes):
    fmt = ">f" if len(raw) == 4 else ">d"
    v = struct.unpack(fmt, raw)[0]
    for p in range(1, 18):
        s = f"{v:.{p}g}"
        if struct.pack(fmt, float(s)) == raw:
            break
    if v == 0 or 1e-4 <= abs(v) < 1e9:      # plain notation: 2e+01 -> 20.0
        s = format(Decimal(s), "f")
    if "e" not in s and "." not in s:
        s += ".0"
    return s + ("f" if len(raw) == 4 else "")


def plausible_f32(w):
    e = (w >> 23) & 0xFF
    return w in (0, 0x80000000) or 0x60 <= e <= 0x9F


def plausible_f64(q):
    e = (q >> 52) & 0x7FF
    return q in (0, 1 << 63) or 1023 - 31 <= e <= 1023 + 32


def text_piece(img, p, end, short_ok):
    """(text, end of the piece) if img[p:end] starts with printable text + NUL; the piece runs on over the zero
    padding to the next 4-aligned nonzero byte (or end)."""
    q = p
    while q < end and img[q - VRAM] in PRINTABLE:
        q += 1
    if q >= end or img[q - VRAM] != 0 or q - p < (1 if short_ok else 3):
        return None
    t = bytes(img[p - VRAM:q - VRAM])
    r = q + 1
    while r < end and img[r - VRAM] == 0:
        r += 1
    if r < end and r % 4:      # something unaligned follows: end the string at the last aligned zero
        r &= ~3
        if r <= q:
            return None
    return t, r


BYTE, HALF, FLOAT = {"u8", "s8", "char"}, {"u16", "s16"}, {"f32", "f64"}


class Ctx:
    """What is known about one image's data: img bytes, listing accesses {addr: {op}}, C declarations
    {addr: {category}}, pointer targets, symbols to keep integer."""

    def __init__(self, image, img, access, decl, targets, no_float):
        self.image, self.img, self.access, self.decl = image, img, access, decl
        self.targets, self.no_float = targets, no_float

    def u(self, a, n):
        return int.from_bytes(self.img[a - VRAM:a - VRAM + n], "big")


def classify(cx, lo, hi, ptrs):
    """Symbols for the region [lo, hi)."""
    out = []
    if any(a in ptrs for a in range(lo, hi)):
        # pointers need a u32 array: unaligned head/tail bytes become their own symbols
        a0, a1 = (lo + 3) & ~3, hi & ~3
        if lo < a0:
            out += classify_plain(cx, lo, a0)
        out.append(numeric(cx, a0, a1, 4, ptrs))
        if a1 < hi:
            out += classify_plain(cx, a1, hi)
        return out
    cats = cx.decl.get(lo, set())
    p = lo
    if not (cats and cats <= HALF | FLOAT):
        # strings: each 4-aligned NUL-terminated text its own symbol
        while p < hi:
            short_ok = p in cx.targets or "addiu" in cx.access.get(p, ()) or bool(cats and cats <= BYTE)
            piece = text_piece(cx.img, p, hi, short_ok)
            if piece is None or (p != lo and p % 4):
                break
            t, r = piece
            out.append(Sym(p, r - p, "char", text=t))
            p = r
    if p < hi:
        out += classify_plain(cx, p, hi)
    return out


def classify_plain(cx, lo, hi):
    n = hi - lo
    ops = cx.access.get(lo, set())
    cats = cx.decl.get(lo, set())
    if (cx.image, lo) not in cx.no_float:
        w4 = [cx.u(a, 4) for a in range(lo, hi - 3, 4)]
        if lo % 4 == 0 and n % 4 == 0 and (
                cats == {"f32"} and all((w >> 23) & 0xFF != 0xFF for w in w4)
                or not cats - {"f32"} and ops & {"lwc1", "swc1"} and all(map(plausible_f32, w4))):
            return [Sym(lo, n, "f32", items=[f_text(cx.img[a - VRAM:a - VRAM + 4]) for a in range(lo, hi, 4)])]
        w8 = [cx.u(a, 8) for a in range(lo, hi - 7, 8)]
        if lo % 8 == 0 and n % 8 == 0 and (
                cats == {"f64"} and all((q >> 52) & 0x7FF != 0x7FF for q in w8)
                or not cats - {"f64"} and ops & {"ldc1", "sdc1"} and all(map(plausible_f64, w8))):
            return [Sym(lo, n, "f64", items=[f_text(cx.img[a - VRAM:a - VRAM + 8]) for a in range(lo, hi, 8)])]
    # element width: the C declaration, else the listings' accesses, else the alignment
    width, signed = None, False
    if cats and cats <= BYTE:
        width, signed = 1, cats == {"s8"}
    elif cats and cats <= HALF:
        width, signed = 2, "s16" in cats and "u16" not in cats
    else:
        widths = {1 if o in ("lb", "lbu", "sb") else 2 if o in ("lh", "lhu", "sh") else 4
                  for o in ops if o in ("lb", "lbu", "sb", "lh", "lhu", "sh", "lw", "sw")}
        if len(widths) == 1:
            width = widths.pop()
            signed = (width == 1 and "lb" in ops and "lbu" not in ops) or \
                (width == 2 and "lh" in ops and "lhu" not in ops)
    if width is None or lo % width or n % width:
        width = 4 if lo % 4 == 0 and n % 4 == 0 else 2 if lo % 2 == 0 and n % 2 == 0 else 1
        signed = False
    return [numeric(cx, lo, hi, width, {}, signed)]


def numeric(cx, lo, hi, width, ptrs, signed=False):
    ctype = {1: "s8" if signed else "u8", 2: "s16" if signed else "u16", 4: "u32"}[width]
    items = []
    for a in range(lo, hi, width):
        if a in ptrs:
            items.append(ptrs[a])
            continue
        v = cx.u(a, width)
        if signed and v >= 1 << (8 * width - 1):
            v -= 1 << (8 * width)
        items.append(v)
    return Sym(lo, hi - lo, ctype, items=items)


# ---------------------------------------------------------------- 5. writing C
def c_string(t: bytes) -> str:
    out = []
    for i, c in enumerate(t):
        ch = chr(c)
        if ch == '"' or ch == "\\":
            out.append("\\" + ch)
        elif ch == "?" and (i + 1 < len(t) and t[i + 1] == ord("?")):
            out.append("\\?")
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == "\r":
            out.append("\\r")
        else:
            out.append(ch)
    return "".join(out)


def item_text(s: Sym, v):
    if isinstance(v, str):
        return v
    if isinstance(v, tuple):          # (name, offset) relocation
        name, off = v
        return f"(u32){name}" + (f" + {off:#x}" if off else "")
    if s.ctype in ("s8", "s16"):
        return str(v)
    return "0" if v == 0 else f"{v:#X}".replace("0X", "0x")


def is_zero(v):
    return v == 0 or (isinstance(v, str) and v in ("0.0f", "0.0"))


def emit(s: Sym) -> list[str]:
    n_el = s.size // s.esize
    if s.ctype == "char":
        head = f"char {s.name}[{s.size:#x}] = "
        parts = [x for x in re.split(r"(?<=\\n)", c_string(s.text)) if x]   # one literal per line of text
        if len(head) + len(c_string(s.text)) < 110 or len(parts) == 1:
            return [f'{head}"{c_string(s.text)}";']
        return [head + f'"{parts[0]}"'] + [" " * len(head) + f'"{x}"' for x in parts[1:-1]] + \
            [" " * len(head) + f'"{parts[-1]}";']
    items = s.items
    last = max((i for i, v in enumerate(items) if not is_zero(v)), default=-1)
    if s.scalar:
        return [f"{s.ctype} {s.name} = {item_text(s, items[0])};"]
    head = f"{s.ctype} {s.name}[{n_el:#x}] = {{"
    if last < 0:
        return [head + " 0 };"]
    per_line = max(1, 16 // s.esize)
    if s.ctype == "u8" or s.ctype == "s8":
        per_line = 16
    zero_skip = max(2, 32 // s.esize)
    lines, i, need_desig = [head], 0, False
    while i <= last:
        # skip a long zero run
        j = i
        while j <= last and is_zero(items[j]):
            j += 1
        if j - i >= zero_skip:
            i, need_desig = j, True
            continue
        row_end = min(last + 1, (i // per_line + 1) * per_line)
        cells = []
        for k in range(i, row_end):
            t = item_text(s, items[k])
            if need_desig:
                t, need_desig = f"[{k:#x}] = {t}", False
            cells.append(t)
        off = f"/* {i * s.esize:#05x} */ " if s.size > 16 else ""
        lines.append(f"    {off}{', '.join(cells)},")
        i = row_end
    lines.append("};")
    if len(lines) == 3 and len(lines[1]) < 90:     # short array: one line
        body = lines[1].strip().rstrip(",")
        body = re.sub(r"^/\* 0x\w+ \*/ ", "", body)
        return [f"{head} {body} }};"]
    return lines


# ---------------------------------------------------------------- main
def generate(res, no_float):
    refs, ops, decls = code_refs()
    layout = json.loads((common.ORIG / "layout.json").read_text())
    files = {}
    all_syms = {}
    for image in IMAGES:
        img = common.original(image)
        img_end = VRAM + len(img)
        bss_end = layout[image]["bss"][1]
        dlo, dhi = BD.DATA_RANGES[image]
        names, funcs = known_names(image)
        left = [tuple(r) for r in res["left"][image]]
        rom = [(lo, hi, rest) for lo, hi, *rest in BD.FROM_ROM.get(image, [])]
        # ROM content: no symbols of its own, only declared (rom_decl); split the runs around it
        cut = []
        for lo, hi in left:
            for k0, k1, _ in rom:
                if lo < k1 and k0 < hi:
                    if lo < k0:
                        cut.append((lo, k0))
                    lo = max(lo, k1)
            if lo < hi:
                cut.append((lo, hi))
        left = cut
        mask = bytearray(bss_end - VRAM + 4)
        for lo, hi in left:
            mask[lo - VRAM:hi - VRAM] = b"\1" * (hi - lo)
        isleft = lambda a: VRAM <= a < bss_end and mask[a - VRAM]
        access = {a: o for a, o in ops[image].items() if isleft(a)}
        bounds = {lo for lo, hi in left} | {a for a in refs[image] if isleft(a)}
        word = lambda a: struct.unpack_from(">I", img, a - VRAM)[0]
        # pointer words
        cx = Ctx(image, img, access, {a: c for a, c in decls[image].items() if isleft(a)}, set(), no_float)
        raw_ptrs = {}
        for lo, hi in left:
            for a in range((lo + 3) & ~3, hi - 3, 4):
                w = word(a)
                if not VRAM <= w < bss_end:
                    continue
                if isleft(w) or w >= img_end or w in funcs or w in refs[image] or w in names:
                    raw_ptrs[a] = w   # into this data or .bss, a function, an address the code names
                elif dlo <= w < dhi and (any(k0 <= w < k1 for k0, k1, _ in BD.NOT_C.get(image, [])) or
                                         img[w - VRAM] in PRINTABLE and (w % 4 == 0 or img[w - VRAM - 1] == 0)):
                    raw_ptrs[a] = w   # microcode data, a string another step builds (a literal)
                # else a number: inside a function (other code's case labels, bit masks like 0x80040020) or
                # into the middle of data another step builds
        cx.targets = targets = {w for w in raw_ptrs.values() if isleft(w)}
        bounds |= targets
        # regions
        syms = []
        for lo, hi in left:
            cuts = sorted({lo, hi} | {b for b in bounds if lo < b < hi})
            for a, b in zip(cuts, cuts[1:]):
                ptrs = {x: w for x, w in raw_ptrs.items() if a <= x and x + 4 <= b}
                syms += classify(cx, a, b, ptrs)
        syms.sort(key=lambda s: s.addr)
        assert sum(s.size for s in syms) == sum(hi - lo for lo, hi in left)
        for s in syms:
            s.name = default_name(image, s.addr)
        # relocation expressions
        addrs = [s.addr for s in syms]

        def expr(w):
            """(name, offset, declared type) for an address a data word holds"""
            i = bisect.bisect_right(addrs, w) - 1
            if i >= 0 and syms[i].addr <= w < syms[i].addr + syms[i].size:
                return (syms[i].name, w - syms[i].addr, syms[i].ctype + " scalar" * syms[i].scalar)
            if w in names:
                return (names[w], 0, "func" if w in funcs else "u8")
            return (default_name(image, w), 0, "u8")
        refd = defaultdict(set)
        for s in syms:
            if s.items and s.ctype == "u32":
                for k, v in enumerate(s.items):
                    a = s.addr + 4 * k
                    if a in raw_ptrs and v == raw_ptrs[a]:
                        name, off, kind = expr(v)
                        s.items[k] = (("&" if kind.endswith("scalar") else "") + name, off)
                        refd[s.addr].add((name, kind))
        all_syms[image] = syms
        # files: blocks of runs less than GROUP_GAP apart
        blocks = []
        for lo, hi in sorted(left + [(k0, k1) for k0, k1, _ in rom]):
            if blocks and lo - blocks[-1][1] < GROUP_GAP:
                blocks[-1][1] = hi
            else:
                blocks.append([lo, hi])
        written = {s: [tuple(r) for r in v] for s, v in res["written"][image].items()}
        for lo, hi in blocks:
            fs = [s for s in syms if lo <= s.addr < hi]
            rs = [(k0, k1, r) for k0, k1, r in rom if lo <= k0 < hi]
            used = sorted({n for s in fs for n in refd.get(s.addr, ())})
            fname = f"{image}_{lo:08X}.c"
            nbytes = sum(s.size for s in fs)
            out = [f"/* {image} data {lo:#010x}-{hi:#010x}: {len(fs)} symbols, {nbytes} bytes from C",
                   f" * image: {image}",
                   " * Generated by tools/gen_data_c.py from the original image; built by tools/build_data.py,",
                   " * which places every symbol at its address in symbols.txt. Gaps are bytes other steps build. */",
                   '#include "data.h"', ""]
            if used:
                out += [f"extern void {n}();" if kind == "func" else
                        f"extern {kind.split()[0]} {n};" if kind.endswith("scalar") else f"extern {kind} {n}[];"
                        for n, kind in used] + [""]
            prev = lo
            items = [(s.addr, s.addr + s.size, s) for s in fs] + [(k0, k1, r) for k0, k1, r in rs]
            for a0, a1, s in sorted(items, key=lambda x: x[0]):
                if a0 > prev:
                    out.append(gap_comment(prev, a0, written))
                out += emit(s) if isinstance(s, Sym) else BD.rom_decl(a0, a1, *s)
                prev = a1
            if prev < hi:
                out.append(gap_comment(prev, hi, written))
            files[fname] = "\n".join(out) + "\n"
    return files, all_syms


def gap_comment(a, b, written):
    who = sorted({s for s, rs in written.items() for lo, hi in rs if lo < b and a < hi})
    what = {"build_code.py": "literals of the C (build_code.py)", "build_asm.py": "asm (build_asm.py)",
            "build_libultra.py": "libultra data (build_libultra.py)"}
    desc = ", ".join(what.get(w, w) for w in who) or "not built here"
    return f"/* {a:#010x}-{b:#010x} ({b - a} bytes): {desc} */"


DATA_H = """/* Types for the data files in src/data (tools/gen_data_c.py). */
#ifndef DATA_H
#define DATA_H
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;
#endif
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--reuse", action="store_true", help=f"reuse {LEFT_JSON.name} instead of rebuilding")
    a = ap.parse_args()
    if a.reuse and LEFT_JSON.exists():
        res = json.loads(LEFT_JSON.read_text())
    else:
        print("finding the data bytes no other step builds (poisoned builds, 0xA5 and 0x5A) ...")
        res = leftover()
        LEFT_JSON.write_text(json.dumps(res))
    for k in IMAGES:
        n = sum(hi - lo for lo, hi in res["left"][k])
        print(f"{k}: {n} data bytes need C, in {len(res['left'][k])} runs")
    no_float = set()
    for _ in range(3):
        files, syms = generate(res, no_float)
        SRC.mkdir(parents=True, exist_ok=True)
        for old in SRC.glob("*.c"):
            old.unlink()
        (ROOT / "include" / "data.h").write_text(DATA_H, newline="\n")
        for name, text in files.items():
            (SRC / name).write_text(text, newline="\n")
        add_symbols(syms)
        built, errors = BD.build_all(sorted(SRC.glob("*.c")))
        for e in errors:
            print("error:", e)
        if errors:
            sys.exit(1)
        bad = []
        for image in IMAGES:
            orig = common.original(image)
            for fname, n, addr, data, rels in built[image]:
                if data != orig[addr - VRAM:addr - VRAM + len(data)]:
                    bad.append((image, n, addr))
        fl = [(k, a) for k, n, a in bad if (k, a) not in no_float]
        print(f"wrote {len(files)} files, {sum(len(v) for v in syms.values())} symbols; "
              f"{len(bad)} symbols differ from the original" + (f": {bad[:8]}" if bad else ""))
        if not bad:
            break
        # it can only be a float that the compiler reads differently: write it as u32
        no_float |= set(fl)
        if not fl:
            sys.exit(1)
    for k in IMAGES:
        ss = syms[k]
        kinds = defaultdict(int)
        for s in ss:
            kinds[s.ctype] += s.size
        print(f"{k}: {len(ss)} symbols, " + ", ".join(f"{t} {n}" for t, n in sorted(kinds.items())) + " bytes")


if __name__ == "__main__":
    main()
