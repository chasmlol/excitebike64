"""Build the code of both images from the decompiled C: compile every source file of the `c` functions of
functions.csv with its profile, link each function at its original address (relocations resolved, the compiler's
literals placed where the game keeps them) and write the bytes into OUT/game.bin and OUT/boot.bin. Each function's
range is zeroed first, so nothing of the original code survives unnoticed. The boot loader's copies of game
functions (`copy`, the game function named in `copy_of`) are built from the game function's C, linked at the boot
address. Names resolve through symbols.txt (common.resolver). Reports the bytes and the functions that differ from
the original images.

A source file holds several functions (one object file); each is linked by its symbol. The object's data sections
(.rdata: strings, float literals, jump tables) are placed whole where all of its functions' original lui/lo pairs
agree they sit, or else literal by literal at the address each pair forms.

usage: python tools/build_code.py OUT [--base DIR] [--jobs N] [--only NAME ...] [--version NAME]
  --base DIR   start from the images in DIR (default: the original images, build/<version>/extracted)
  --only NAME  build only these functions (e.g. after editing them); the rest stays as in the base images
"""
import argparse
import concurrent.futures as cf
import struct
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
import sn64  # noqa: E402
from snobj import LnkFile  # noqa: E402

ROOT = common.ROOT
VRAM = common.VRAM


def compile_one(job):
    src, prof, incs = job
    with tempfile.TemporaryDirectory() as td:
        obj = Path(td) / "m.o"
        ok, msg = sn64.compile(prof, Path(src), obj, [Path(i) for i in incs])
        if not ok:
            return src, None, msg
        return src, obj.read_bytes(), ""


def _orig_word(orig_img, addr):
    return lambda off: struct.unpack_from(">I", orig_img, addr - VRAM + off)[0]


def _function_relocs(lnk, symbol, size):
    """-> (start offset in .text, [relocations inside the function]), or (None, problem)."""
    tidx, text = lnk.section_by_name(".text")
    funcs = {n: (s, e) for n, s, e in lnk.functions(".text")}
    if symbol not in funcs:
        return None, f"{symbol} not defined in its object"
    start, _ = funcs[symbol]
    if len(text.data) < start + size:
        return None, f"object has {len(text.data) - start} bytes for {symbol}, function is {size}"
    return start, [(o, t, e) for o, t, e in text.relocs if start <= (o & ~3) < start + size]


def section_refs(lnk, symbol, addr, size, orig_img):
    """What the original says about where a function's object data sit: the low halves (minus the offset into the
    section) and the high halves of its references to them. -> ({section: {low}}, [(section, offset, high)])"""
    tidx, _ = lnk.section_by_name(".text")
    start, relocs = _function_relocs(lnk, symbol, size)
    lows, his = {}, []
    if start is None:
        return lows, his
    orig = _orig_word(orig_img, addr)
    for o, t, e in relocs:
        kind, a, add, _ = sn64.expr_ref(lnk, e)
        if kind == "sec" and a != tidx:
            off = (o & ~3) - start
            if t.startswith("LO16"):
                lows.setdefault(a, set()).add((orig(off) - add) & 0xFFFF)
            elif t.startswith("HI16"):
                his.append((a, add, orig(off) & 0xFFFF))
    return lows, his


def solve_bases(lnk, refs):
    """The address of each data section of an object that all the given functions' references agree on.
    refs: section_refs() of the functions linked from the object into one image. -> ({section: base}, [problems])"""
    lows, his = {}, []
    for lo, hi in refs:
        for a, v in lo.items():
            lows.setdefault(a, set()).update(v)
        his += hi
    bases, problems = {}, []
    for a, low_set in lows.items():
        hs = [(add, hi) for sa, add, hi in his if sa == a]
        cands = set()
        for L in low_set:
            for add, hi in hs:
                H = (hi - ((L + add + 0x8000) >> 16)) & 0xFFFF
                cands.add((H << 16) | L)
        good = [b for b in cands if all(((b + add + 0x8000) >> 16) & 0xFFFF == hi for add, hi in hs)]
        if len(good) != 1:
            problems.append(f"section {lnk.sections[a].name}: base candidates {[hex(x) for x in sorted(cands)]}")
            continue
        bases[a] = good[0]
    return bases, problems


def link_function(fn, symbol, lnk, addr, size, orig_img, resolver, bases=None):
    """-> (code bytes, [(address, bytes, (section, offset) or None for a whole section)], [problems]).
    lnk: the object (LnkFile or bytes); resolver: name -> address (common.resolver(image));
    bases: solve_bases() for the object (default: from this function alone).
    The data sections with a base are placed whole; the literals of the others one by one."""
    if not isinstance(lnk, LnkFile):
        lnk = LnkFile(lnk)
    tidx, text = lnk.section_by_name(".text")
    start, relocs = _function_relocs(lnk, symbol, size)
    if start is None:
        return None, [], [relocs]
    spans = sorted((s, e, n) for n, s, e in lnk.functions(".text"))
    code = bytearray(text.data[start:start + size])
    orig = _orig_word(orig_img, addr)
    problems = []
    if bases is None:   # sections it can't place whole get the per-literal placement below
        bases, _ = solve_bases(lnk, [section_refs(lnk, symbol, addr, size, orig_img)])
    refs = [(o, t, sn64.expr_ref(lnk, e)) for o, t, e in relocs]
    used = {a for o, t, (kind, a, add, _) in refs if kind == "sec" and a != tidx}

    # Sections without a base (their literals are not where the object's layout puts them: the original object's
    # .rdata had other functions' literals in between): place each referenced literal on its own, at the address the
    # original lui/lo pair forms.
    sext = lambda v: v - 0x10000 if v & 0x8000 else v
    ref_addr, lo_offs = {}, []
    for o, t, (kind, a, add, _) in refs:
        if kind == "sec" and a != tidx and a not in bases and t.startswith("LO16"):
            off = (o & ~3) - start
            w = orig(off)
            reg = (w >> 21) & 31
            for k in range(off - 4, -4, -4):
                h = orig(k)
                if h >> 26 == 0x0F and (h >> 16) & 31 == reg:
                    ref_addr[off] = (((h & 0xFFFF) << 16) + sext(w & 0xFFFF)) & 0xFFFFFFFF
                    lo_offs.append((off, reg, a, add))
                    break
    for o, t, (kind, a, add, _) in refs:
        if kind == "sec" and a != tidx and a not in bases and t.startswith("HI16"):
            off = (o & ~3) - start
            reg = (orig(off) >> 16) & 31
            nxt = sorted(x for x in lo_offs if x[0] > off and x[1] == reg)
            if nxt:
                ref_addr[off] = ref_addr[nxt[0][0]]
    sizes = {0x23: 4, 0x31: 4, 0x35: 8, 0x37: 8, 0x21: 2, 0x25: 2, 0x20: 1, 0x24: 1}
    singles = []
    for off, reg, a, add in lo_offs:
        s = lnk.sections[a]
        n = sizes.get(orig(off) >> 26)
        if n is None:   # address taken: a jump table (words relocated in the section) or a string
            rel = {o & ~3 for o, t, e in s.relocs if t.startswith("REL32")}
            if add in rel:
                n = 0
                while add + n in rel:
                    n += 4
            else:
                z = bytes(s.data[add:add + 256]).find(bytes(1))
                n = z + 1 if z >= 0 else 4
        singles.append((a, add, n, ref_addr[off]))

    def resolve_ref(kind, a, add):
        if kind == "sym":
            b = resolver(a)
            return None if b is None else (b + add) & 0xFFFFFFFF
        if kind == "abs":
            return (a + add) & 0xFFFFFFFF
        if a == tidx:
            if start <= add < start + size:
                return addr + (add - start)
            # another function of the object (a call, or a jump table entry inside it)
            for s0, e0, name in spans:
                if s0 <= add < e0 or add == s0:
                    b = resolver(name)
                    return None if b is None else b + (add - s0)
            return None
        return (bases[a] + add) & 0xFFFFFFFF if a in bases else None

    for o, t, (kind, a, add, _) in refs:
        off = (o & ~3) - start
        target = resolve_ref(kind, a, add)
        if target is None and kind == "sec" and a != tidx:
            target = ref_addr.get(off)
        if target is None:
            problems.append(f"+{off:#x}: unresolved {t} {kind} {a} {add:#x}")
            continue
        w = struct.unpack_from(">I", code, off)[0]
        struct.pack_into(">I", code, off, sn64.patch(w, t, target))

    placed = []
    for a, base in bases.items():
        if a not in used:
            continue
        s = lnk.sections[a]
        data = bytearray(s.data)
        for o, t, e in s.relocs:
            if not t.startswith("REL32"):
                continue
            kind, ra, add, _ = sn64.expr_ref(lnk, e)
            target = resolve_ref(kind, ra, add)
            if target is None:
                problems.append(f"{s.name}+{o:#x}: unresolved data word")
                continue
            struct.pack_into(">I", data, o & ~3, target)
        placed.append((base, bytes(data), None))
    for a, add, n, target in singles:
        s = lnk.sections[a]
        data = bytearray(s.data[add:add + n])
        for o, t, e in s.relocs:
            if t.startswith("REL32") and add <= (o & ~3) < add + n:
                kind, ra, radd, _ = sn64.expr_ref(lnk, e)
                tw = resolve_ref(kind, ra, radd)
                if tw is not None:
                    struct.pack_into(">I", data, (o & ~3) - add, tw)
        placed.append((target, bytes(data), (a, add)))
    return bytes(code), placed, problems


def derive_syms(lnk, symbol, addr, size, orig_img):
    """Boot-side addresses of the symbols a boot copy references, read off the original relocated fields."""
    start, relocs = _function_relocs(lnk, symbol, size)
    orig = _orig_word(orig_img, addr)
    sext = lambda v: v - 0x10000 if v & 0x8000 else v
    out = {}
    for o, t, e in relocs:
        off = (o & ~3) - start
        kind, a, add, _ = sn64.expr_ref(lnk, e)
        if kind != "sym":
            continue
        w = orig(off)
        if t.startswith("REL26"):
            out[a] = (((w & 0x3FFFFFF) << 2) | 0x80000000) - add
        elif t.startswith("LO16"):
            reg = (w >> 21) & 31
            for k in range(off - 4, -4, -4):
                h = orig(k)
                if h >> 26 == 0x0F and (h >> 16) & 31 == reg:
                    out[a] = ((((h & 0xFFFF) << 16) + sext(w & 0xFFFF)) - add) & 0xFFFFFFFF
                    break
    return out


def link_image(entries, objs, orig_img, resolver):
    """Link functions into one image. entries: [(name, symbol, source, address, size)].
    -> {name: (code, placed, problems)}"""
    by_src = {}
    for e in entries:
        by_src.setdefault(e[2], []).append(e)
    out = {}
    for src, es in by_src.items():
        lnk = LnkFile(objs[src])
        bases, base_problems = solve_bases(lnk, [section_refs(lnk, sym, a, n, orig_img) for _, sym, _, a, n in es])
        singles = {}
        for name, sym, _, a, n in es:
            code, placed, probs = out[name] = link_function(name, sym, lnk, a, n, orig_img, resolver, bases)
            for base, data, where in placed:
                if where:
                    singles[where] = (base, len(data), name)
        # Literals placed one by one that are neighbours both in the object and in the image: the zero bytes
        # between them are alignment padding there as well.
        last = None
        for (sec, off), (base, n, name) in sorted(singles.items()):
            if last and last[0] == sec and off > last[1] and off - last[1] == base - last[2] and off - last[1] < 8:
                gap = lnk.sections[sec].data[last[1]:off]
                if not any(gap):
                    out[last[3]][1].append((last[2], bytes(gap), None))
            if not last or last[0] != sec or off + n >= last[1]:
                last = (sec, off + n, base + n, name)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("outdir")
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--base", help="start from the images in this dir (default: the original images)")
    ap.add_argument("--only", nargs="+", metavar="NAME")
    common.add_version_arg(ap)
    a = ap.parse_args()
    out = common.output_dir(a.outdir)
    table = {f["name"]: f for f in common.functions()}
    items = [f for f in table.values() if f["kind"] == "c"]
    if a.only:
        missing = [n for n in a.only if n not in table or table[n]["kind"] != "c"]
        if missing:
            sys.exit(f"not decompiled C functions: {' '.join(missing)}")
        items = [table[n] for n in a.only]
    # The boot loader links its own copies of some game functions (same C), at the boot address.
    dups = [f for f in table.values() if f["kind"] == "copy"]
    for f in dups:
        if table.get(f["copy_of"], {}).get("kind") != "c":
            sys.exit(f"{f['name']}: copy_of {f['copy_of']!r} is not a C function")
    if a.only:
        dups = [f for f in dups if f["copy_of"] in a.only]
    orig = {k: common.original(k) for k in common.IMAGES}
    base_dir = Path(a.base) if a.base else common.ORIG
    new = {k: bytearray((base_dir / f"{k}.bin").read_bytes()) for k in common.IMAGES}
    # Zero every function first: nothing of the original code can survive into the build unnoticed.
    for f in items + dups:
        new[f["image"]][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = bytes(f["size"])
    # One compile per source file; every function of a file must use the same compiler profile.
    src_prof = {}
    for f in items:
        p = f["profile"] or "sn281"
        if src_prof.setdefault(f["source"], p) != p:
            sys.exit(f"{f['source']}: functions with different profiles ({src_prof[f['source']]}, {p})")
    profs = {p: common.profile(p) for p in set(src_prof.values())}
    jobs = [(str(ROOT / s), profs[p], [str(ROOT / "include")]) for s, p in sorted(src_prof.items())]
    objs, fails = {}, []
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        for src, ob, msg in ex.map(compile_one, jobs):
            rel = Path(src).relative_to(ROOT).as_posix()
            if ob is None:
                fails.append((rel, msg[:400]))
            else:
                objs[rel] = ob
    print(f"compiled {len(objs)} of {len(jobs)} source files" + (f"; failed: {fails[:5]}" if fails else ""))
    problems, data_writes = {}, 0
    data_bytes = {"game": 0, "boot": 0}
    writes = []          # (image, function, address, bytes, padding?)

    def link(entries, img, resolver):
        for fn, (code, placed, probs) in link_image(entries, objs, orig[img], resolver).items():
            f = table[fn]
            if probs:
                problems[fn] = probs
            if code is None:
                continue
            new[img][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = code
            writes.extend((img, fn, base, data) for base, data, where in placed)
    for img in common.IMAGES:
        link([(f["name"], f["name"], f["source"], f["address"], f["size"]) for f in items
              if f["image"] == img and f["source"] in objs], img, common.resolver(img))
    # The boot copies are linked with the boot image's symbols: the game names they use are listed for the boot
    # image in symbols.txt as well.
    dup_entries = [(f["name"], f["copy_of"], table[f["copy_of"]]["source"], f["address"], f["size"]) for f in dups
                   if table[f["copy_of"]]["source"] in objs]
    link(dup_entries, "boot", common.resolver("boot"))
    for name, sym, src, ad, n in dup_entries:
        if any("unresolved" in x for x in problems.get(name, [])):
            known = common.symbols()["boot"]
            hint = {k: v for k, v in derive_syms(LnkFile(objs[src]), sym, ad, n, orig["boot"]).items()
                    if k not in known}
            problems[name].append("symbols.txt has no boot address for "
                                  + ", ".join(f"{k} (the original uses {v:#010x})" for k, v in hint.items()))
    print(f"boot copies of game functions: {len(dup_entries)} built from the game's C")
    for img, fn, base, data in writes:
        o = base - VRAM
        if 0 <= o and o + len(data) <= len(new[img]):
            new[img][o:o + len(data)] = data
            data_writes += 1
            data_bytes[img] += len(data)
        else:
            problems.setdefault(fn, []).append(f"data at {base:#x} outside the {img} image")
    out.mkdir(parents=True, exist_ok=True)
    for k in common.IMAGES:
        (out / f"{k}.bin").write_bytes(bytes(new[k]))
        diff = [i for i in range(len(orig[k])) if orig[k][i] != new[k][i]]
        print(f"{k}.bin: {len(new[k]):#x} bytes, {len(diff)} differ from the original"
              + (f" (first at {VRAM + diff[0]:#x})" if diff else ""))
    bad = [f["name"] for f in items + dups
           if new[f["image"]][f["address"] - VRAM:f["address"] - VRAM + f["size"]]
           != orig[f["image"]][f["address"] - VRAM:f["address"] - VRAM + f["size"]]]
    if bad:
        print(f"{len(bad)} functions differ from the original: {' '.join(bad[:40])}")
    code_bytes = sum(f["size"] for f in items)
    print(f"code from C: {code_bytes} bytes in {len(items)} functions; "
          f"{data_writes} literal blocks placed ({data_bytes['game'] + data_bytes['boot']} bytes)")
    if problems:
        print(f"{len(problems)} functions with link problems:")
        for fn, ps in list(problems.items())[:15]:
            print(" ", fn, "; ".join(ps[:3]))
    if fails:
        sys.exit(1)


if __name__ == "__main__":
    main()
