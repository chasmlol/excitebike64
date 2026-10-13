"""Rebuild Excitebike 64's libultra (game image and boot image copies) from source-built objects.

Objects (built by tools/build_libultra_objs.sh with KMC GCC 2.7.2, on Linux or in WSL):
  build/libultra/ultralib/src/**.o   decompals/ultralib 2.0K libgultra_rom
  build/libultra/libgcc/*.o          src/libgcc/libgcc2.c: __divdi3 __udivdi3 __udivmoddi4 __umoddi3 __clz_tab
libultra_map.json says where each object's sections sit in each image and what every symbol resolves to.

  python tools/build_libultra.py --objects     build the objects: runs tools/build_libultra_objs.sh with bash
                                               (through WSL on Windows), settings ULTRALIB and KMC_GCC
  python tools/build_libultra.py [OUT]         zero libultra (functions.csv's `libultra` functions, the objects'
                                               code and placed .data/.rodata) in OUT/{game,boot}.bin (default
                                               build/out), write it back from the objects with relocations
                                               resolved, report bytes that differ from the original images
  python tools/build_libultra.py --make-maps   derive libultra_map.json from the original images: tile the
                                               libultra .text ranges with the objects, solve the symbols, place
                                               data sections by reference (or by content for vimode tables)
Options: --map FILE, --check (report only, don't write). Standard library only.
"""
import argparse
import json
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402

ROOT = common.ROOT
VRAM = common.VRAM
OBJ_ROOT = common.BUILD / "libultra"
SOURCES = ["ultralib", "libgcc"]  # object trees, relative to OBJ_ROOT
SKIP = {"monutil.o"}  # rmon, IDO-assembled, not in libgultra_rom
DEFAULT_MAP = ROOT / "libultra_map.json"
IMAGES = common.IMAGES
# libultra .text ranges (tools/split.py EXCLUDE) and where its data may be
TEXT_RANGE = {"game": (0x800AC240, 0x800B767C), "boot": (0x80005C70, 0x8000CE10)}
DATA_RANGE = {"game": (0x800C2280, 0x800DC098), "boot": (0x8000CE10, 0x80011600)}

R_MIPS_32, R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16 = 2, 4, 5, 6
RMASK = {R_MIPS_32: 0xFFFFFFFF, R_MIPS_26: 0x03FFFFFF, R_MIPS_HI16: 0xFFFF, R_MIPS_LO16: 0xFFFF}
SHN_UNDEF, SHN_ABS, SHN_COMMON = 0, 0xFFF1, 0xFFF2


def sext16(x):
    return x - 0x10000 if x & 0x8000 else x


def word(img, a):
    return struct.unpack_from(">I", img, a - VRAM)[0]


# ---------------------------------------------------------------- ELF (MIPS BE ELF32 relocatable, REL)
class Section:
    pass


class Symbol:
    __slots__ = ("name", "value", "size", "bind", "type", "shndx")


class Obj:
    def __init__(self, path, rel):
        data = Path(path).read_bytes()
        self.rel = rel
        if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 2:
            raise ValueError(f"{path}: not a big-endian ELF32 file")
        shoff, = struct.unpack_from(">I", data, 32)
        shentsize, shnum, shstrndx = struct.unpack_from(">HHH", data, 46)
        self.secs = []
        for i in range(shnum):
            s = Section()
            (s.name_off, s.type, s.flags, s.addr, s.offset, s.size, s.link, s.info, s.align,
             s.entsize) = struct.unpack_from(">10I", data, shoff + i * shentsize)
            s.index = i
            s.data = bytes(s.size) if s.type == 8 else data[s.offset:s.offset + s.size]
            self.secs.append(s)
        names = self.secs[shstrndx].data
        for s in self.secs:
            s.name = names[s.name_off:names.index(b"\0", s.name_off)].decode()
        self.syms, self.rels = [], {}
        for s in self.secs:
            if s.type == 2:
                strtab = self.secs[s.link].data
                for o in range(0, s.size, 16):
                    y = Symbol()
                    no, y.value, y.size, info, _, y.shndx = struct.unpack_from(">IIIBBH", s.data, o)
                    y.name = strtab[no:strtab.index(b"\0", no)].decode()
                    y.bind, y.type = info >> 4, info & 15
                    if y.type == 3:
                        y.name = self.secs[y.shndx].name
                    self.syms.append(y)
            elif s.type == 9:
                self.rels[s.info] = [(off, info & 0xFF, info >> 8) for off, info in struct.iter_unpack(">II", s.data)]
            elif s.type == 4:
                raise ValueError(f"{path}: RELA relocations not supported")
        self.text = self.sec(".text")
        self.defs = {y.name: y for y in self.syms
                     if y.bind in (1, 2) and y.shndx not in (SHN_UNDEF, SHN_ABS, SHN_COMMON) and y.shndx < 0xFF00}
        self.abs_defs = {y.name: y.value for y in self.syms if y.bind in (1, 2) and y.shndx == SHN_ABS}
        # code extent: up to the end of the last sized function (C objects); the rest of .text is alignment
        # padding, zero here but linker junk in some places of the original (libgcc), so it is not compared
        self.code_size = self.text.size if self.text else 0
        funcs = [y for y in self.syms if self.text and y.shndx == self.text.index and y.type == 2]
        labels = [y for y in self.syms if self.text and y.shndx == self.text.index and y.type in (0, 1)
                  and y.name and y.name != "gcc2_compiled."]
        if funcs and all(y.size for y in funcs) and not labels:
            end = max(y.value + y.size for y in funcs)
            if any(self.text.data[end:]):
                raise ValueError(f"{path}: nonzero bytes after the last function")
            self.code_size = end

    def sec(self, name):
        return next((s for s in self.secs if s.name == name), None)

    def data_secs(self):
        """sections with contents other than .text (.data, .rodata, ...). KMC gcc's .rodata lacks SHF_ALLOC
        (ultralib's fix_objfile.py --fix-section-flags reproduces that); it is linked all the same."""
        return [s for s in self.secs if s.type == 1 and s.size and s.name != ".text"
                and (s.flags & 2 or s.name.startswith((".rodata", ".rdata")))]

    def bss_secs(self):
        return [s for s in self.secs if s.flags & 2 and s.type == 8 and s.size]

    def text_words(self):
        n = self.code_size // 4
        if not n:
            return [], []
        words = list(struct.unpack(">%dI" % n, self.text.data[:4 * n]))
        masks = [0] * n
        for off, ty, si in self.rels.get(self.text.index, []):
            masks[off // 4] |= RMASK.get(ty, 0xFFFFFFFF)
        return words, masks

    def functions(self):
        """[(name, offset)] of the code symbols in .text (FUNC symbols, or global labels in asm files)"""
        t = self.text
        pts = {}
        for y in self.syms:
            if t and y.shndx == t.index and y.type in (0, 1, 2) and y.name and y.name != "gcc2_compiled.":
                if y.type == 2 or y.bind in (1, 2):
                    prev = pts.get(y.value)
                    if prev is None or (y.type == 2 and prev.type != 2):
                        pts[y.value] = y
        return [(pts[o].name, o) for o in sorted(pts)]


def relocate(obj, sec, base, resolve, size=None):
    """sec's bytes linked at base; resolve(obj, symidx) -> (S, is_local)"""
    orig = sec.data
    out = bytearray(orig)
    rels = obj.rels.get(sec.index, [])
    for i, (off, ty, si) in enumerate(rels):
        S, local = resolve(obj, si)
        w, = struct.unpack_from(">I", orig, off)
        P = base + off
        if ty == R_MIPS_26:
            A = (w & 0x3FFFFFF) << 2
            if not local and A & 0x8000000:
                A -= 0x10000000
            tgt = (A + S) & 0xFFFFFFFF
            if (tgt ^ (P + 4)) & 0xF0000000:
                raise ValueError(f"{obj.rel}: jal out of its 256MB segment")
            w = (w & 0xFC000000) | ((tgt >> 2) & 0x3FFFFFF)
        elif ty == R_MIPS_HI16:
            lo = next((struct.unpack_from(">I", orig, o2)[0] for o2, t2, s2 in rels[i + 1:]
                       if t2 == R_MIPS_LO16 and s2 == si), None)
            if lo is None:
                raise ValueError(f"{obj.rel}: HI16 at {off:#x} without LO16")
            v = ((w & 0xFFFF) << 16) + sext16(lo & 0xFFFF) + S
            w = (w & 0xFFFF0000) | (((v + 0x8000) >> 16) & 0xFFFF)
        elif ty == R_MIPS_LO16:
            w = (w & 0xFFFF0000) | ((sext16(w & 0xFFFF) + S) & 0xFFFF)
        elif ty == R_MIPS_32:
            # the original linker added a COMMON symbol's st_value (its alignment) to R_MIPS_32 words:
            # __osCurrentHandle[] = {&__Dom1SpeedParam + 8, &__Dom2SpeedParam + 8}. HI16/LO16/26 don't get it.
            y = obj.syms[si]
            w = (w + S + (y.value if y.shndx == SHN_COMMON else 0)) & 0xFFFFFFFF
        else:
            raise ValueError(f"{obj.rel}: relocation type {ty} not supported")
        struct.pack_into(">I", out, off, w)
    return bytes(out[:size] if size is not None else out)


def load_objects(rels=None):
    for src in SOURCES:
        if not (OBJ_ROOT / src).is_dir():
            raise SystemExit(f"{OBJ_ROOT / src} missing: build the objects first "
                             "(python tools/build_libultra.py --objects)")
    if rels is None:
        rels = []
        for src in SOURCES:
            d = OBJ_ROOT / src
            rels += sorted(p.relative_to(OBJ_ROOT).as_posix() for p in d.rglob("*.o") if p.name not in SKIP)
    return {r: Obj(OBJ_ROOT / r, r) for r in rels}


# ---------------------------------------------------------------- map making
def matches(img, words, masks, a):
    return all(not ((word(img, a + 4 * i) ^ w) & ~m & 0xFFFFFFFF) for i, (w, m) in enumerate(zip(words, masks)))


def tile(objs, img, lo, hi):
    """Cover [lo,hi) with the objects' code, each 16-aligned -> slots [(addr, [candidates], size)], unknown runs"""
    tw = [(o.rel, *o.text_words()) for o in objs.values()]
    tw = [x for x in tw if x[1]]
    slots, unknown = [], []
    a = lo
    while a < hi:
        cands = [(r, w) for r, w, m in tw if a + 4 * len(w) <= hi and matches(img, w, m, a)]
        if cands:
            big = max(len(w) for r, w in cands)  # a tiny object can match the start of a bigger one
            slots.append((a, sorted(r for r, w in cands if len(w) == big), 4 * big))
            a = (a + 4 * big + 15) & ~15
            continue
        b = a + 16
        while b < hi and not any(len(w) > 4 and b + 4 * len(w) <= hi and matches(img, w, m, b) for r, w, m in tw):
            b += 16
        unknown.append((a, min(b, hi) - a))
        a = b
    return slots, unknown


def sym_key(obj, y):
    """(key, adjust) for a symbol reference: key = ('s', rel, shndx) section base, ('g', name) global,
    ('abs', name, value) absolute"""
    if y.shndx == SHN_ABS:
        return ("abs", y.name, y.value), 0
    if y.type == 3:
        return ("s", obj.rel, y.shndx), 0
    if y.shndx in (SHN_UNDEF, SHN_COMMON):
        return ("g", y.name), 0
    return ("s", obj.rel, y.shndx), y.value


def raw_equations(obj, sec, base, img):
    """[(key, value)] that placing obj's section sec at base implies (value = address of the key)"""
    eqs = []
    rels = obj.rels.get(sec.index, [])
    for i, (off, ty, si) in enumerate(rels):
        if sec is obj.text and off >= obj.code_size:
            continue
        y = obj.syms[si]
        key, adj = sym_key(obj, y)
        ow, = struct.unpack_from(">I", sec.data, off)
        gw = word(img, base + off)
        if ty == R_MIPS_26:
            A = (ow & 0x3FFFFFF) << 2
            if y.bind != 0 and A & 0x8000000:
                A -= 0x10000000
            tgt = ((base + off + 4) & 0xF0000000) | ((gw & 0x3FFFFFF) << 2)
            eqs.append((key, (tgt - A - adj) & 0xFFFFFFFF))
        elif ty == R_MIPS_HI16:
            j = next((k for k in range(i + 1, len(rels)) if rels[k][1] == R_MIPS_LO16 and rels[k][2] == si), None)
            if j is None:
                continue
            olo, = struct.unpack_from(">I", sec.data, rels[j][0])
            glo = word(img, base + rels[j][0])
            oahl = ((ow & 0xFFFF) << 16) + sext16(olo & 0xFFFF)
            gahl = ((gw & 0xFFFF) << 16) + sext16(glo & 0xFFFF)
            eqs.append((key, (gahl - oahl - adj) & 0xFFFFFFFF))
        elif ty == R_MIPS_32:
            if y.shndx == SHN_COMMON:
                adj = y.value  # see relocate()
            eqs.append((key, (gw - ow - adj) & 0xFFFFFFFF))
    return eqs


class Solver:
    """Section bases and global addresses, from the placed objects' relocated words."""

    def __init__(self, objs):
        self.objs = objs
        self.placed = {}      # rel -> .text address
        self.data_only = []   # rels placed by a data section only
        self.known = {}       # key -> address
        self.defs = {}        # global name -> defining rel

    def canon(self, key):
        if key[0] == "abs":
            return key, 0
        if key[0] == "g" and key[1] in self.defs:
            rel = self.defs[key[1]]
            y = self.objs[rel].defs[key[1]]
            return ("s", rel, y.shndx), y.value
        return key, 0

    def value(self, k):
        return k[2] if k[0] == "abs" else self.known.get(k)

    def spans(self, extra=()):
        out = []
        for k, b in list(self.known.items()) + list(extra):
            if k[0] == "s":
                out.append((b, b + self.objs[k[1]].secs[k[2]].size))
        return out

    def place(self, rel, secs, img, commit=False):
        """Try placing rel's sections {shndx: addr}; True if it agrees with everything known."""
        o = self.objs[rel]
        if any(n in self.defs and self.defs[n] != rel for n in o.defs):
            return False
        saved = dict(self.defs)
        for n in o.defs:
            self.defs[n] = rel
        new = {("s", rel, i): a for i, a in secs.items()}
        ok = True
        for i, a in secs.items():
            for key, val in raw_equations(o, o.secs[i], a, img):
                k, adj = self.canon(key)
                v = (val - adj) & 0xFFFFFFFF
                have = new.get(k, self.value(k))
                if have is not None and have != v:
                    ok = False
                    break
                new[k] = v
            if not ok:
                break
        if ok:  # globals referenced by name before must be where this object defines them
            for n, y in o.defs.items():
                g = self.known.get(("g", n))
                base = new.get(("s", rel, y.shndx), self.known.get(("s", rel, y.shndx)))
                if g is not None and base is not None and (base + y.value) & 0xFFFFFFFF != g:
                    ok = False
                    break
            for n, v in o.abs_defs.items():
                if self.known.get(("g", n), v) != v:
                    ok = False
        if ok:  # a global no placed object defines can't point inside a placed section
            sp = self.spans(new.items())
            ok = not any(k[0] == "g" and any(b <= v < e for b, e in sp) for k, v in new.items())
        if not (ok and commit):
            self.defs = saved
            return ok
        if o.text is not None and o.text.index in secs:
            self.placed[rel] = secs[o.text.index]
        else:
            self.data_only.append(rel)
        self.known.update(new)
        for n, v in o.abs_defs.items():
            self.known[("g", n)] = v
        for n, y in o.defs.items():  # fold name-only knowledge into this object's section bases
            g = self.known.pop(("g", n), None)
            if g is not None:
                self.known[("s", rel, y.shndx)] = (g - y.value) & 0xFFFFFFFF
        return True

    def add_section(self, rel, sec, img):
        """fold in what a placed data section's pointers say (e.g. __osTimerList = &__osBaseTimer)"""
        for key, val in raw_equations(self.objs[rel], sec, self.known[("s", rel, sec.index)], img):
            k, adj = self.canon(key)
            v = (val - adj) & 0xFFFFFFFF
            if self.value(k) not in (None, v):
                raise SystemExit(f"{rel} {sec.name}: {k} = {v:#x} contradicts {self.value(k):#x}")
            if k[0] != "abs":
                self.known[k] = v

    def resolver(self):
        def resolve(obj, si):
            y = obj.syms[si]
            key, adj = sym_key(obj, y)
            if key[0] == "abs":
                return key[2], False
            if key[0] == "g":
                key, adj = self.canon(key)
            return (self.known[key] + adj) & 0xFFFFFFFF, y.bind == 0 and key[1] == obj.rel
        return resolve


def masked(sec, obj):
    m = bytearray(sec.size)
    for off, ty, si in obj.rels.get(sec.index, []):
        m[off:off + 4] = b"\1\1\1\1"
    return m


def search_data(img, data, mask, lo, hi, align):
    """addresses in [lo,hi) where data matches (bytes under mask ignored)"""
    n = len(data)
    best, cur, start = (0, 0), 0, 0  # anchor on the longest unmasked run
    for i in range(n):
        if mask[i]:
            cur = 0
            continue
        if cur == 0:
            start = i
        cur += 1
        if cur > best[1]:
            best = (start, cur)
    s0, ln = best[0], min(best[1], 64)
    if not ln:
        return []
    hits = []
    p = img.find(data[s0:s0 + ln], lo - VRAM + s0)
    while p != -1 and p - s0 + n <= hi - VRAM:
        b = p - s0
        if b % align == 0 and all(mask[i] or img[b + i] == data[i] for i in range(n)):
            hits.append(b + VRAM)
        p = img.find(data[s0:s0 + ln], p + 1)
    return hits


def make_maps(args):
    objs = load_objects()
    out = {"build": {"objects": {"ultralib": "build/libultra/ultralib (decompals/ultralib build/K/libgultra_rom)",
                                 "libgcc": "build/libultra/libgcc (src/libgcc/libgcc2.c)"},
                     "ultralib": "decompals/ultralib " + git_rev(Path(common.setting("ULTRALIB"))),
                     "version": "2.0K", "target": "libgultra_rom",
                     "notes": "tools/build_libultra_objs.sh builds the objects (KMC GCC 2.7.2)"},
           "images": {}}
    for im in IMAGES:
        img = common.original(im)
        lo, hi = TEXT_RANGE[im]
        slots, unknown = tile(objs, img, lo, hi)
        sv = Solver(objs)
        pending = []
        for a, cands, size in slots:
            if len(cands) == 1:
                if not sv.place(cands[0], {objs[cands[0]].text.index: a}, img, commit=True):
                    raise SystemExit(f"{im}: {cands[0]} at {a:#x} contradicts the other objects")
            else:
                pending.append((a, cands))
        changed = True
        while pending and changed:  # same-looking objects: keep the one whose symbols agree
            changed = False
            for slot in list(pending):
                a, cands = slot
                ok = [r for r in cands if r not in sv.placed and sv.place(r, {objs[r].text.index: a}, img)]
                if len(ok) == 1:
                    sv.place(ok[0], {objs[ok[0]].text.index: a}, img, commit=True)
                    pending.remove(slot)
                    changed = True
        ambiguous = []
        for a, cands in pending:  # identical bytes either way
            ok = [r for r in cands if r not in sv.placed and sv.place(r, {objs[r].text.index: a}, img)]
            ambiguous.append({"addr": hex(a), "candidates": ok})
            sv.place(ok[0], {objs[ok[0]].text.index: a}, img, commit=True)
        # data-only objects (vitbl, vimodes, libm_vals, libgcc's _clz) by a referenced global
        dlo, dhi = DATA_RANGE[im]
        loose = [o for r, o in objs.items() if r not in sv.placed and not o.code_size and o.data_secs()]
        for o in loose:
            secs = {}
            for n, y in o.defs.items():
                g = sv.known.get(("g", n))
                if g is not None:
                    secs[y.shndx] = (g - y.value) & 0xFFFFFFFF
            if secs and not sv.place(o.rel, secs, img, commit=True):
                raise SystemExit(f"{im}: {o.rel} conflicts at {secs}")
        # unreferenced data sections by content (the game code, not libultra, uses the vimode tables):
        # largest first, never overlapping a placed section, unique hits only
        by_content = []
        todo = []
        for o in objs.values():
            linked = o.rel in sv.placed or o.rel in sv.data_only
            if linked or not o.code_size:
                todo += [(o, s) for s in o.data_secs() if ("s", o.rel, s.index) not in sv.known]
        for o, s in sorted(todo, key=lambda x: -x[1].size):
            linked = o.rel in sv.placed or o.rel in sv.data_only
            mask = masked(s, o)
            if not linked and (s.size < 0x40 or not any(b for b, m in zip(s.data, mask) if not m)):
                continue  # too little to recognise an object the code doesn't reference
            sp = sv.spans()
            hits = [h for h in search_data(img, s.data, mask, dlo, dhi, min(max(s.align, 1), 16))
                    if not any(b < h + s.size and h < e for b, e in sp)]
            if len(hits) != 1:
                continue
            if linked:
                sv.known[("s", o.rel, s.index)] = hits[0]
            elif not sv.place(o.rel, {s.index: hits[0]}, img, commit=True):
                continue
            by_content.append(f"{o.rel} {s.name} {hits[0]:#x}")
        done = set()
        while True:
            todo = [(r, s) for r in list(sv.placed) + sv.data_only for s in objs[r].data_secs()
                    if ("s", r, s.index) in sv.known and (r, s.index) not in done]
            if not todo:
                break
            for r, s in todo:
                sv.add_section(r, s, img)
                done.add((r, s.index))
        resolve = sv.resolver()
        # verify every placed section byte for byte
        problems, objects = [], []
        for rel in sorted(sv.placed, key=sv.placed.get) + sv.data_only:
            o = objs[rel]
            entry = {"obj": rel}
            if rel in sv.placed:
                a = sv.placed[rel]
                entry[".text"] = hex(a)
                got = relocate(o, o.text, a, resolve, o.code_size)
                if got != img[a - VRAM:a - VRAM + len(got)]:
                    problems.append(f"{rel} .text differs after relocation")
            for s in o.data_secs() + o.bss_secs():
                base = sv.known.get(("s", rel, s.index))
                if base is None:
                    if rel in sv.placed:
                        problems.append(f"{rel} {s.name} ({s.size:#x} bytes) not placed")
                    continue
                entry[s.name] = hex(base)
                if s.type == 1:
                    got = relocate(o, s, base, resolve)
                    if got != img[base - VRAM:base - VRAM + len(got)]:
                        problems.append(f"{rel} {s.name} at {base:#x} differs")
            if rel in sv.placed:
                entry["functions"] = {n: hex(sv.placed[rel] + off) for n, off in o.functions()}
            objects.append(entry)
        symbols = {k[1]: hex(v) for k, v in sv.known.items() if k[0] == "g"}
        for rel in list(sv.placed) + sv.data_only:
            for n, y in objs[rel].defs.items():
                base = sv.known.get(("s", rel, y.shndx))
                if base is not None:
                    symbols[n] = hex((base + y.value) & 0xFFFFFFFF)
        covered = set()
        for rel, a in sv.placed.items():
            covered.update(range(a, a + objs[rel].code_size, 4))
        padding, a = [], lo
        while a < hi:
            if a in covered:
                a += 4
                continue
            b = a
            while b < hi and b not in covered:
                b += 4
            padding.append({"addr": hex(a), "size": b - a, "zero": not any(img[a - VRAM:b - VRAM])})
            a = b
        out["images"][im] = {
            "text_range": [hex(lo), hex(hi)], "objects": objects, "symbols": dict(sorted(symbols.items())),
            "same_bytes_ambiguous": ambiguous, "data_placed_by_content": by_content,
            "padding": padding, "unexplained": [{"addr": hex(a), "size": s} for a, s in unknown]}
        nfun = sum(len(e.get("functions", {})) for e in objects)
        junk = [(p["addr"], p["size"]) for p in padding if not p["zero"]]
        print(f"{im}: {len(sv.placed)} objects' code ({nfun} functions) + {len(sv.data_only)} data-only objects "
              f"placed, {len(symbols)} symbols; ambiguous (same bytes) {[x['candidates'] for x in ambiguous]}; "
              f"nonzero padding {junk}; unexplained {[(hex(a), s) for a, s in unknown]}")
        for p in problems:
            print("  problem:", p)
    Path(args.map).write_text(json.dumps(out, indent=1) + "\n")
    print("wrote", args.map)


def git_rev(path):
    try:
        r = subprocess.run(["git", "-C", str(path), "rev-parse", "HEAD"], capture_output=True, text=True)
        return r.stdout.strip()[:12] or "?"
    except OSError:
        return "?"


def make_objects():
    """Run tools/build_libultra_objs.sh (bash; WSL on Windows) with the ULTRALIB and KMC_GCC settings."""
    env = {"ULTRALIB": common.posix_path(Path(common.setting("ULTRALIB"))),
           "KMC_GCC": common.posix_path(Path(common.setting("KMC_GCC"))),
           "OUT": common.posix_path(OBJ_ROOT),
           "LIBGCC_SRC": common.posix_path(ROOT / "src" / "libgcc")}
    work = common.setting("LIBULTRA_WORK", required=False)
    if work:
        env["LIBULTRA_WORK"] = work
    OBJ_ROOT.mkdir(parents=True, exist_ok=True)
    rc, out = common.run_posix(ROOT / "tools" / "build_libultra_objs.sh", env)
    print(out)
    if rc:
        sys.exit("building the libultra objects failed")


# ---------------------------------------------------------------- build
def build(args):
    mp = json.loads(Path(args.map).read_text())
    func_table = common.functions()
    total = 0
    for im in IMAGES:
        m = mp["images"][im]
        objs = load_objects([e["obj"] for e in m["objects"]])
        symbols = {k: int(v, 16) for k, v in m["symbols"].items()}
        bases = {}
        for e in m["objects"]:
            for s in objs[e["obj"]].secs:
                if s.name in e:
                    bases[(e["obj"], s.index)] = int(e[s.name], 16)

        def resolve(obj, si):
            y = obj.syms[si]
            if y.shndx == SHN_ABS:
                return y.value, False
            if y.type == 3 or y.shndx not in (SHN_UNDEF, SHN_COMMON):
                return (bases[(obj.rel, y.shndx)] + (0 if y.type == 3 else y.value)) & 0xFFFFFFFF, y.bind == 0
            return symbols[y.name], False

        path = Path(args.build_dir) / f"{im}.bin"
        img = bytearray(path.read_bytes())
        orig = common.original(im)
        lib_fns = [f for f in func_table if f["image"] == im and f["kind"] == "libultra"]
        writes = []
        for e in m["objects"]:
            o = objs[e["obj"]]
            if ".text" in e:
                a = int(e[".text"], 16)
                writes.append((a, relocate(o, o.text, a, resolve, o.code_size), ".text"))
            for s in o.data_secs():
                if s.name in e:
                    b = int(e[s.name], 16)
                    writes.append((b, relocate(o, s, b, resolve), s.name))
        # 1. zero libultra: functions.csv's libultra functions, the objects' code, the placed data. Entries
        #    no object's code covers are linker junk after libgcc (map "padding"), not functions: kept.
        code = [(a, a + len(d)) for a, d, k in writes if k == ".text"]
        junk = [f for f in lib_fns if not any(s < f["address"] + f["size"] and f["address"] < e for s, e in code)]
        for a, n in [(f["address"], f["size"]) for f in lib_fns if f not in junk] + [(a, len(d)) for a, d, _ in writes]:
            img[a - VRAM:a - VRAM + n] = bytes(n)
        # 2. write it back from the objects
        for a, d, _ in writes:
            img[a - VRAM:a - VRAM + len(d)] = d
        # 3. compare with the original image
        diff = sum(1 for x, y in zip(img, orig) if x != y) + abs(len(img) - len(orig))
        bad = [f["name"] for f in lib_fns
               if img[f["address"] - VRAM:f["address"] - VRAM + f["size"]] !=
               orig[f["address"] - VRAM:f["address"] - VRAM + f["size"]]]
        ntext = sum(1 for _, _, k in writes if k == ".text")
        ndata = sum(len(d) for _, d, k in writes if k != ".text")
        print(f"{im}: {ntext} objects' code ({sum(len(d) for _, d, k in writes if k == '.text')} bytes) and "
              f"{len(writes) - ntext} data sections ({ndata} bytes) written; libultra functions identical: "
              f"{len(lib_fns) - len(bad) - len(junk)}/{len(lib_fns) - len(junk)}; {diff} bytes differ from "
              "the original" + (f"; differing: {bad}" if bad else "")
              + (f"; not code, kept: {[f['name'] for f in junk]}" if junk else ""))
        total += diff
        if not args.check:
            path.write_bytes(img)
    return total


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("build_dir", nargs="?", default=str(common.BUILD / "out"))
    ap.add_argument("--objects", action="store_true", help="build the objects (tools/build_libultra_objs.sh)")
    ap.add_argument("--make-maps", action="store_true")
    ap.add_argument("--map", default=str(DEFAULT_MAP))
    ap.add_argument("--check", action="store_true", help="report only, leave BUILD_DIR untouched")
    args = ap.parse_args()
    if args.objects:
        make_objects()
    elif args.make_maps:
        make_maps(args)
    else:
        common.output_dir(args.build_dir)
        sys.exit(1 if build(args) else 0)


if __name__ == "__main__":
    main()
