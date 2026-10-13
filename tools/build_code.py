"""Build the code of both images from the decompiled C: compile every `c` function of functions.csv with its
profile, link it at its original address (relocations resolved, the compiler's literals placed where the game
keeps them) and write the bytes into OUT/game.bin and OUT/boot.bin. Each function's range is zeroed first, so
nothing of the original code survives unnoticed. The boot loader's copies of game functions (`copy`) are built
from the game function's C, linked at the boot address. Reports the bytes that differ from the original images.

usage: python tools/build_code.py OUT [--base DIR] [--jobs N] [--only NAME ...]
  --base DIR   start from the images in DIR (default: the original images, build/extracted)
  --only NAME  build only these functions (e.g. after editing them); the rest stays as in the base images
"""
import argparse
import concurrent.futures as cf
import json
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
    fn, src, prof, incs = job
    with tempfile.TemporaryDirectory() as td:
        obj = Path(td) / "m.o"
        ok, msg = sn64.compile(prof, Path(src), obj, [Path(i) for i in incs])
        if not ok:
            return fn, None, msg
        return fn, obj.read_bytes(), ""


def link_function(fn, symbol, objbytes, addr, size, orig_img, resolver=None):
    """-> (code bytes, [(address, bytes) of placed data], [problems])"""
    resolver = resolver or sn64.resolve
    lnk = LnkFile(objbytes)
    tidx, text = lnk.section_by_name(".text")
    funcs = {n: (s, e) for n, s, e in lnk.functions(".text")}
    if symbol not in funcs:
        return None, [], [f"{symbol} not defined in its object"]
    start, _ = funcs[symbol]
    by_off = {s: n for n, (s, e) in funcs.items()}
    code = bytearray(text.data[start:start + size])
    if len(code) < size:
        return None, [], [f"object has {len(code)} bytes, function is {size}"]
    relocs = [(o, t, e) for o, t, e in text.relocs if start <= (o & ~3) < start + size]
    orig = lambda off: struct.unpack_from(">I", orig_img, addr - VRAM + off)[0]
    problems = []

    # Where this object's own data sections go: solve each section's base from the original lui/lo pairs.
    lows, his = {}, []
    for o, t, e in relocs:
        kind, a, add, _ = sn64.expr_ref(lnk, e)
        if kind == "sec" and a != tidx:
            off = (o & ~3) - start
            if t.startswith("LO16"):
                lows.setdefault(a, set()).add((orig(off) - add) & 0xFFFF)
            elif t.startswith("HI16"):
                his.append((a, add, orig(off) & 0xFFFF))
    bases = {}
    for a, low_set in lows.items():
        cands = set()
        for L in low_set:
            for sa, add, hi in his:
                if sa != a:
                    continue
                H = (hi - ((L + add + 0x8000) >> 16)) & 0xFFFF
                cands.add((H << 16) | L)
        hs = [(add, hi) for sa, add, hi in his if sa == a]
        good = [b for b in cands if all(((b + add + 0x8000) >> 16) & 0xFFFF == hi for add, hi in hs)]
        if len(good) != 1:
            problems.append(f"section {lnk.sections[a].name}: base candidates {[hex(x) for x in sorted(cands)]}")
            continue
        bases[a] = good[0]

    # Sections whose literals are not contiguous in the game (the original object's .rdata had other functions'
    # literals in between): place each referenced literal on its own, at the address the original lui/lo pair forms.
    sext = lambda v: v - 0x10000 if v & 0x8000 else v
    ref_addr, lo_offs = {}, []
    for o, t, e in relocs:
        kind, a, add, _ = sn64.expr_ref(lnk, e)
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
    for o, t, e in relocs:
        kind, a, add, _ = sn64.expr_ref(lnk, e)
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
            local = by_off.get(add)
            return resolver(local) if local else None
        return (bases[a] + add) & 0xFFFFFFFF if a in bases else None

    for o, t, e in relocs:
        off = (o & ~3) - start
        kind, a, add, _ = sn64.expr_ref(lnk, e)
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
        placed.append((base, bytes(data)))
    for a, add, n, target in singles:
        s = lnk.sections[a]
        data = bytearray(s.data[add:add + n])
        for o, t, e in s.relocs:
            if t.startswith("REL32") and add <= (o & ~3) < add + n:
                kind, ra, radd, _ = sn64.expr_ref(lnk, e)
                tw = resolve_ref(kind, ra, radd)
                if tw is not None:
                    struct.pack_into(">I", data, (o & ~3) - add, tw)
        placed.append((target, bytes(data)))
    # the per-literal fallback replaces the section-level problem it solved
    problems = [p for p in problems if not (p.startswith("section ") and not any(
        x for x in problems if "unresolved" in x))]
    return bytes(code), placed, problems


def derive_syms(objbytes, symbol, addr, size, orig_img):
    """Boot-side addresses of the symbols a boot copy references, read off the original relocated fields."""
    lnk = LnkFile(objbytes)
    tidx, text = lnk.section_by_name(".text")
    start = {n: s for n, s, e in lnk.functions(".text")}[symbol]
    orig = lambda off: struct.unpack_from(">I", orig_img, addr - VRAM + off)[0]
    sext = lambda v: v - 0x10000 if v & 0x8000 else v
    out = {}
    for o, t, e in text.relocs:
        off = (o & ~3) - start
        if not 0 <= off < size:
            continue
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


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("outdir")
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--base", help="start from the images in this dir (default: the original images)")
    ap.add_argument("--only", nargs="+", metavar="NAME")
    a = ap.parse_args()
    out = common.output_dir(a.outdir)
    table = {f["name"]: f for f in common.functions()}
    items = [f for f in table.values() if f["kind"] == "c"]
    if a.only:
        missing = [n for n in a.only if n not in table or table[n]["kind"] != "c"]
        if missing:
            sys.exit(f"not decompiled C functions: {' '.join(missing)}")
        items = [table[n] for n in a.only]
    orig = {k: common.original(k) for k in common.IMAGES}
    base_dir = Path(a.base) if a.base else common.ORIG
    new = {k: bytearray((base_dir / f"{k}.bin").read_bytes()) for k in common.IMAGES}
    # Zero every function first: nothing of the original code can survive into the build unnoticed.
    for f in items:
        new[f["image"]][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = bytes(f["size"])
    profs = {}
    jobs = []
    for f in items:
        name = f["profile"] or "sn281"
        if name not in profs:
            profs[name] = common.profile(name)
        jobs.append((f["name"], str(ROOT / f["source"]), profs[name], [str(ROOT / "include")]))
    objs, fails = {}, []
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        for fn, ob, msg in ex.map(compile_one, jobs):
            if ob is None:
                fails.append((fn, msg[:200]))
            else:
                objs[fn] = ob
    print(f"compiled {len(objs)} of {len(items)}" + (f"; failed: {fails[:5]}" if fails else ""))
    problems, data_writes = {}, 0
    data_bytes = {"game": 0, "boot": 0}
    for f in items:
        fn = f["name"]
        if fn not in objs:
            continue
        img = f["image"]
        code, placed, probs = link_function(fn, fn, objs[fn], f["address"], f["size"], orig[img])
        if probs:
            problems[fn] = probs
        if code is None:
            continue
        new[img][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = code
        for base, data in placed:
            o = base - VRAM
            if 0 <= o and o + len(data) <= len(new[img]):
                new[img][o:o + len(data)] = data
                data_writes += 1
                data_bytes[img] += len(data)
            else:
                problems.setdefault(fn, []).append(f"data at {base:#x} outside the {img} image")
    # The boot loader links its own copies of some game functions (same C). They are built from the game's source,
    # linked at the boot address; the symbols they use live elsewhere in the boot image, and their boot addresses
    # come from boot_syms.json (written once from the original relocated fields, then reused).
    dups = [(f, Path(f["source"]).stem) for f in table.values() if f["kind"] == "copy"]
    if a.only:
        dups = [(f, g) for f, g in dups if g in objs]
    sym_path = ROOT / "boot_syms.json"
    boot_syms = json.loads(sym_path.read_text()) if sym_path.exists() else None
    derived = {}
    for f, g in dups:
        if g not in objs:
            problems.setdefault(f["name"], []).append(f"{g} has no object")
            continue
        new["boot"][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = bytes(f["size"])
        if boot_syms is None:
            derived.update(derive_syms(objs[g], g, f["address"], f["size"], orig["boot"]))
    if boot_syms is None:
        boot_syms = derived
        sym_path.write_text(json.dumps({k: f"{v:#010x}" for k, v in sorted(boot_syms.items())}, indent=1))
        print(f"wrote {sym_path.name}: {len(boot_syms)} boot-side symbol addresses")
    else:
        boot_syms = {k: int(v, 16) for k, v in boot_syms.items()}
    bres = lambda name: boot_syms.get(name, sn64.resolve(name))  # noqa: E731
    for f, g in dups:
        if g not in objs:
            continue
        code, placed, probs = link_function(f["name"], g, objs[g], f["address"], f["size"], orig["boot"],
                                            resolver=bres)
        if probs:
            problems[f["name"]] = probs
        if code is None:
            continue
        new["boot"][f["address"] - VRAM:f["address"] - VRAM + f["size"]] = code
        for base, data in placed:
            new["boot"][base - VRAM:base - VRAM + len(data)] = data
    print(f"boot copies of game functions: {len(dups)} built from the game's C")
    out.mkdir(parents=True, exist_ok=True)
    for k in common.IMAGES:
        (out / f"{k}.bin").write_bytes(bytes(new[k]))
        diff = [i for i in range(len(orig[k])) if orig[k][i] != new[k][i]]
        print(f"{k}.bin: {len(new[k]):#x} bytes, {len(diff)} differ from the original"
              + (f" (first at {VRAM + diff[0]:#x})" if diff else ""))
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
