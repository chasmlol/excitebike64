"""Draft a version's tables (functions.csv, symbols.txt) and layout from another version's, by comparing the
extracted images of the two. Standard library only.

  python tools/map_version.py NEW [--from REF] [--write [--out DIR]]      (REF default: lodgenet)

Both versions must be extracted (tools/extract.py; NEW needs a versions/NEW/version.toml with at least its title,
sha1, cic and [rom]). The comparison:
1. Functions. Each word of both images is masked where it depends on where things are linked (j/jal targets, the
   %hi of a KSEG0 address, the immediate of a load/store/addiu/ori not based on $sp or $zero). A REF function is
   found in NEW where its masked words are; unique hits are anchors (the longest chain in address order), and of
   several hits the one nearest to where the anchors say it should be wins. A function not found that way
   (changed) starts where the calls of the matched functions say (their jal targets), or else where the
   instruction-level alignment of the code between anchors puts it; it ends at the last `jr $ra` before the next
   function. NEW's linker fill between objects (non-zero bytes no function covers) becomes `padding` entries.
2. Symbols. Every %hi/%lo pair and every jal of a matched function, and every word of the aligned initialized data
   (word-level alignment of the data ranges, pointers masked), pairs a REF address with a NEW one; each symbol
   moves by the offset most pairs inside it agree on, else by the offset of the data alignment at its address,
   else (unreferenced BSS) by its neighbour's. Second names follow their symbol, functions their function.
3. Layout: the ranges of version.toml mapped the same way (the segment markers from NEW's container, the RSP
   microcode found by its bytes).
Prints a report: changed functions (with how their start was found), removed ones, calls to code no REF function
covers (new functions), symbols placed by a neighbour's offset or not at all, and the draft layout for
version.toml. --write writes versions/NEW/functions.csv and symbols.txt (refused if they exist; --out DIR writes
them to DIR instead). The drafts need
checking: build NEW (tools/build_all.py --version NEW) and fix what differs.
"""
import argparse
import bisect
import csv
import difflib
import io
import json
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402

VRAM = common.VRAM
LOADSTORE = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2E, 0x31, 0x35, 0x37,
             0x39, 0x3D, 0x3F, 0x30, 0x34, 0x38, 0x3C}
STORES = {0x28, 0x29, 0x2A, 0x2B, 0x2E, 0x39, 0x3D, 0x3F, 0x38, 0x3C}
LO_USERS = LOADSTORE | {0x09, 0x0D, 0x19}
FPU_LS = {0x31, 0x35, 0x39, 0x3D}
JR_RA = 0x03E00008


def words(b):
    return list(struct.unpack(f">{len(b) // 4}I", b[:len(b) // 4 * 4]))


def mask(ws):
    out = []
    for w in ws:
        op = w >> 26
        if op in (2, 3):
            w &= 0xFC000000
        elif op == 0x0F:
            if 0x8000 <= (w & 0xFFFF) < 0x8041:
                w &= 0xFFFF0000
        elif op in LO_USERS and ((w >> 21) & 31) not in (29, 0):
            w &= 0xFFFF0000
        out.append(w)
    return out


def mask_data(ws):
    return [0x80000000 if 0x80000000 <= w < 0x80400000 else w for w in ws]


def lis(pairs):
    """The longest chain of pairs increasing in both members."""
    pairs = sorted(pairs)
    tails, idx, prev = [], [], [None] * len(pairs)
    for i, (_, b) in enumerate(pairs):
        k = bisect.bisect_left(tails, b)
        if k == len(tails):
            tails.append(b)
            idx.append(i)
        else:
            tails[k] = b
            idx[k] = i
        prev[i] = idx[k - 1] if k else None
    res, i = [], idx[-1] if idx else None
    while i is not None:
        res.append(pairs[i])
        i = prev[i]
    return res[::-1]


def sext(x):
    return x - 0x10000 if x & 0x8000 else x


def address_pairs(a, b, pa, pb):
    """(REF address, NEW address) for each %hi/%lo pair and jal of two matched instruction runs (same opcodes).
    pa, pb: the runs' start addresses. A lui counts for the low halves after it in its basic block only (the code
    is scanned in a line: at a branch target or after an unconditional jump the registers start unknown)."""
    targets = set()
    for k, x in enumerate(a):
        op = x >> 26
        if op in (0x04, 0x05, 0x06, 0x07, 0x14, 0x15, 0x16, 0x17) or (op == 0x01 and (x >> 16) & 0x1F in
                                                                         (0, 1, 2, 3, 0x10, 0x11, 0x12, 0x13)) \
                or (op == 0x11 and (x >> 21) & 0x1F == 8):
            targets.add(k + 1 + sext(x & 0xFFFF))
        elif op == 0x02:
            t = (((pa + 4 * k) & 0xF0000000) | ((x & 0x3FFFFFF) << 2)) - pa
            if t % 4 == 0:
                targets.add(t // 4)
    hia, hib, out = {}, {}, []
    reset_at = None
    for k, (x, y) in enumerate(zip(a, b)):
        if k in targets or k == reset_at:
            hia.clear()
            hib.clear()
        op = x >> 26
        rs, rt = (x >> 21) & 31, (x >> 16) & 31
        if op == 0x02 or (op == 0 and x & 0x3F == 0x08) or (op == 0x04 and rs == 0 and rt == 0):
            reset_at = k + 2          # j, jr, b: what follows the delay slot is another block
        if op == 3 or op == 2:
            ta = ((pa + 4 * k) & 0xF0000000) | ((x & 0x3FFFFFF) << 2)
            tb = ((pb + 4 * k) & 0xF0000000) | ((y & 0x3FFFFFF) << 2)
            if op == 3:
                out.append((ta, tb))
            continue
        if op == 0x0F:
            hia[rt], hib[rt] = (x & 0xFFFF) << 16, (y & 0xFFFF) << 16
            continue
        if op in LO_USERS and rs in hia and (y >> 21) & 31 == rs:
            if op == 0x0D:       # ori: the low half unsigned
                A, B = (hia[rs] | (x & 0xFFFF)), (hib[rs] | (y & 0xFFFF))
            else:
                A = (hia[rs] + sext(x & 0xFFFF)) & 0xFFFFFFFF
                B = (hib[rs] + sext(y & 0xFFFF)) & 0xFFFFFFFF
            out.append((A, B))
            if op in (0x09, 0x0D, 0x19) or (op not in STORES and op not in FPU_LS):
                hia.pop(rt, None)
                hib.pop(rt, None)
            continue
        if op == 0:
            hia.pop((x >> 11) & 31, None)
            hib.pop((x >> 11) & 31, None)
        elif op not in (0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x14, 0x15, 0x16, 0x17, 0x11) \
                and op not in STORES and op not in FPU_LS:
            hia.pop(rt, None)
            hib.pop(rt, None)
    return out


class Side:
    def __init__(self, version, image):
        self.version, self.image = version, image
        self.data = (common.BUILD / version / "extracted" / f"{image}.bin").read_bytes()
        self.w = words(self.data)
        self.m = mask(self.w)
        self.end = VRAM + len(self.data)

    def word(self, a):
        return self.w[(a - VRAM) // 4]


def match_functions(R, N, funcs):
    """-> {REF function name: (NEW address, how)} for the functions found, how: 'same' (masked words equal) or
    'call' / 'align' (changed: start from calls / from the alignment)."""
    index = defaultdict(list)
    L = 6
    for p in range(len(N.m) - L + 1):
        index[tuple(N.m[p:p + L])].append(p)
    found, cands = {}, {}
    for f in funcs:
        n = f["size"] // 4
        o = (f["address"] - VRAM) // 4
        key = R.m[o:o + min(L, n)]
        if n >= L:
            hits = index.get(tuple(key), [])
        else:
            hits = [p for p in range(len(N.m) - n + 1) if N.m[p:p + n] == key] if n else []
        cands[f["name"]] = [VRAM + 4 * p for p in hits if N.m[p:p + n] == R.m[o:o + n]]
    pairs = [(f["address"], cands[f["name"]][0]) for f in funcs
             if len(cands[f["name"]]) == 1 and f["size"] >= 16 and f["kind"] != "padding"]
    anchors = lis(pairs)
    a_r = [a for a, _ in anchors]

    def expected(addr):
        k = bisect.bisect_right(a_r, addr) - 1
        return addr if k < 0 else anchors[k][1] + (addr - anchors[k][0])
    claimed = set()
    for f in sorted(funcs, key=lambda f: -f["size"]):
        c = [x for x in cands[f["name"]] if x not in claimed]
        if not c:
            continue
        e = expected(f["address"])
        best = min(c, key=lambda x: abs(x - e))
        if (len(c) == 1 and f["size"] >= 24) or abs(best - e) < (0x400 if f["size"] >= 24 else 0x100):
            found[f["name"]] = (best, "same")
            claimed.add(best)
    for f in funcs:   # what is left: unique unclaimed hits (another link order)
        if f["name"] not in found:
            c = [x for x in cands[f["name"]] if x not in claimed]
            if len(c) == 1 and f["size"] >= 16:
                found[f["name"]] = (c[0], "same")
                claimed.add(c[0])
    return found


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("new")
    ap.add_argument("--from", dest="ref", default=common.DEFAULT_VERSION)
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--out", help="write the tables to this folder instead of versions/NEW")
    a = ap.parse_args()
    ref, new = a.ref, a.new
    for v in (ref, new):
        if not (common.BUILD / v / "extracted" / "game.bin").exists():
            sys.exit(f"{v} is not extracted (tools/extract.py)")
    rfuncs = common.functions(ref)
    rsyms = common.symbols(ref)
    rlay = common.version_info(ref)
    new_funcs_rows, new_syms, report, layout_out = [], {}, [], {}
    nlayout_json = json.loads((common.BUILD / new / "extracted" / "layout.json").read_text())
    for image in common.IMAGES:
        R, N = Side(ref, image), Side(new, image)
        funcs = sorted((f for f in rfuncs if f["image"] == image and f["size"] > 0), key=lambda f: f["address"])
        byname = {f["name"]: f for f in funcs}
        found = match_functions(R, N, [f for f in funcs if f["kind"] != "padding"])
        n_map = common.table_path("libultra_map.json", new)
        lib_new, r_text, n_text = {}, {}, {}
        if n_map.exists():      # NEW's libultra map (tools/build_libultra.py --make-maps): where each object is
            def texts(version):
                m = json.loads(common.table_path("libultra_map.json", version).read_text())["images"][image]
                return {e["obj"]: int(e[".text"], 16) for e in m["objects"] if ".text" in e}, m
            r_text, _ = texts(ref)
            n_text, m = texts(new)
            for e in m["objects"]:
                lib_new.update({n: (int(x, 16), e["obj"]) for n, x in e.get("functions", {}).items()
                                if e["obj"] not in r_text})
            for f in funcs:
                if f["kind"] == "libultra" and f["source"] in r_text:
                    found.pop(f["name"], None)
                    if f["source"] in n_text:
                        found[f["name"]] = (n_text[f["source"]] + f["address"] - r_text[f["source"]], "same")
        # every call of NEW's code (for placing the starts the alignment estimates)
        n_calls = Counter()
        for x in range(VRAM, rlay[image]["data"][0] + (N.end - R.end), 4):
            w = N.word(x)
            if w >> 26 == 3:
                n_calls[(x & 0xF0000000) | ((w & 0x3FFFFFF) << 2)] += 1

        # address pairs from the matched functions' code: calls and %hi/%lo
        votes = defaultdict(Counter)

        def vote_code():
            for name, (b, how) in found.items():
                if how != "same":
                    continue
                f = byname[name]
                o, p = (f["address"] - VRAM) // 4, (b - VRAM) // 4
                n = f["size"] // 4
                for A, B in address_pairs(R.w[o:o + n], N.w[p:p + n], f["address"], b):
                    votes[A][B] += 1
        vote_code()
        # initialized data: word alignment, pointers masked
        r_lo, r_hi = rlay[image]["data"]
        last = max((f for f in funcs if f["name"] in found and found[f["name"]][1] == "same"
                    and f["address"] < r_lo), key=lambda f: f["address"])
        n_lo = found[last["name"]][0] + (r_lo - last["address"])
        n_hi = N.end - (R.end - r_hi)
        A = R.w[(r_lo - VRAM) // 4:(r_hi - VRAM) // 4]
        B = N.w[(n_lo - VRAM) // 4:(n_hi - VRAM) // 4]
        sm = difflib.SequenceMatcher(None, mask_data(A), mask_data(B), autojunk=False)
        data_map = {}
        for bl in sm.get_matching_blocks():
            for t in range(bl.size):
                ra, na = r_lo + 4 * (bl.a + t), n_lo + 4 * (bl.b + t)
                data_map[ra] = na
                x, y = A[bl.a + t], B[bl.b + t]
                if 0x80000000 <= x < 0x80400000:      # a pointer: what it points to moved as much
                    votes[x][y] += 1

        def from_votes(how):
            """changed functions: their start from the calls and %hi/%lo pairs of the code, the data's pointers"""
            for f in funcs:
                if f["name"] not in found and f["kind"] != "padding" and votes.get(f["address"]):
                    b, n = votes[f["address"]].most_common(1)[0]
                    if VRAM <= b < N.end:
                        found[f["name"]] = (b, how)

        def from_alignment():
            """changed functions not called: the instruction alignment between the functions found around them"""
            known = sorted((f["address"], found[f["name"]][0]) for f in funcs if f["name"] in found)
            chain = lis(known)
            keys = [x for x, _ in chain]
            for f in funcs:
                if f["name"] in found or f["kind"] in ("padding", "libultra", "asm"):
                    continue
                k = bisect.bisect_right(keys, f["address"]) - 1
                if k < 0 or k + 1 >= len(chain):
                    continue
                (ra, na), (rb, nb) = chain[k], chain[k + 1]
                X = R.m[(ra - VRAM) // 4:(rb - VRAM) // 4]
                Y = N.m[(na - VRAM) // 4:(nb - VRAM) // 4]
                blocks = difflib.SequenceMatcher(None, X, Y, autojunk=False).get_matching_blocks()
                i0, n = (f["address"] - ra) // 4, f["size"] // 4
                inside = [bl for bl in blocks if bl.size and bl.a < i0 + n and bl.a + bl.size > i0]
                sim = sum(min(bl.a + bl.size, i0 + n) - max(bl.a, i0) for bl in inside)
                if not inside or sim < 0.3 * n:
                    continue
                bl = inside[0]
                pos = na + 4 * (bl.b + (i0 - bl.a))   # the first aligned word, back to where the function starts
                near = [t for t in n_calls if abs(t - pos) <= 0x20]
                if near:                              # a call target nearby is the start
                    pos = min(near, key=lambda t: abs(t - pos))
                found[f["name"]] = (pos, "align")

        def vote_changed():
            """the calls and %hi/%lo pairs of the changed functions, in the parts that align"""
            for f in funcs:
                if f["name"] not in found or found[f["name"]][1] == "same" or f["kind"] == "padding":
                    continue
                b = found[f["name"]][0]
                n = f["size"] // 4
                X = R.m[(f["address"] - VRAM) // 4:(f["address"] - VRAM) // 4 + n]
                Y = N.m[(b - VRAM) // 4:(b - VRAM) // 4 + 2 * n]
                for bl in difflib.SequenceMatcher(None, X, Y, autojunk=False).get_matching_blocks():
                    if bl.size < 2:
                        continue
                    o, q = (f["address"] - VRAM) // 4 + bl.a, (b - VRAM) // 4 + bl.b
                    for A_, B_ in address_pairs(R.w[o:o + bl.size], N.w[q:q + bl.size], VRAM + 4 * o, VRAM + 4 * q):
                        votes[A_][B_] += 1

        from_votes("call")
        from_alignment()
        vote_changed()
        from_votes("call")

        # NEW's function entries: sizes of matched functions as in REF; changed ones to their last jr $ra
        starts = sorted({b for b, _ in found.values()})
        rows = []
        for f in funcs:
            if f["name"] not in found:
                if f["kind"] not in ("padding", "asm"):
                    report.append(f"{image}: not found (removed?): {f['name']} ({f['kind']}, {f['size']} bytes)")
                continue
            b, how = found[f["name"]]
            size = f["size"]
            if how != "same" and f["kind"] in ("c", "copy"):
                k = bisect.bisect_right(starts, b)
                nxt = starts[k] if k < len(starts) else b + size
                ends = [x + 8 for x in range(b, nxt - 4, 4) if N.word(x) == JR_RA]
                size = (ends[-1] if ends else nxt) - b
                w0, wp = N.word(b), N.word(b - 8)
                odd = "" if (w0 & 0xFFFF8000) == 0x27BD8000 or wp == JR_RA else "; the start doesn't look like one"
                report.append(f"{image}: changed: {f['name']} {f['address']:#x}+{f['size']:#x} -> {b:#x}+{size:#x} "
                              f"(start from {'the calls' if how == 'call' else 'the alignment: check it'}{odd})")
            elif how != "same":
                report.append(f"{image}: changed {f['kind']}: {f['name']} {f['address']:#x} -> {b:#x}, size kept")
            rows.append({**f, "address": b, "size": size})
        # libultra functions of NEW that REF doesn't have (the functions of objects REF doesn't link)
        have = {r["name"] for r in rows}
        lib_starts = sorted({b for b, _ in lib_new.values()} | {r["address"] for r in rows})
        for name, (b, obj) in sorted(lib_new.items(), key=lambda x: x[1]):
            if name in have:
                continue
            k = bisect.bisect_right(lib_starts, b)
            nxt = lib_starts[k] if k < len(lib_starts) else b + 4
            ends = [x + 8 for x in range(b, nxt - 4, 4) if N.word(x) == JR_RA]
            size = (ends[-1] if ends else nxt) - b
            rows.append({"name": name, "image": image, "address": b, "size": size, "module": "libultra",
                         "kind": "libultra", "profile": "", "source": obj, "copy_of": ""})
            report.append(f"{image}: libultra function only in {new}: {name} {b:#x}+{size:#x}")
        # asm entries not found (their bytes changed): after the entry before them, as in REF
        prev = None
        for f in funcs:
            if f["name"] in found and found[f["name"]][1] == "same":
                prev = f
            elif f["kind"] == "asm" and f["name"] not in found and prev is not None:
                b = found[prev["name"]][0] + (f["address"] - prev["address"])
                rows.append({**f, "address": b})
                found[f["name"]] = (b, "asm")
                report.append(f"{image}: asm {f['name']} {f['address']:#x} -> {b:#x} (after {prev['name']})")
        # REF's padding entries (linker fill, unused library code) that sit after the same entry with the same
        # bytes in NEW
        # (in libultra: in the same object)
        prev = None
        names = {r["name"] for r in rows}
        objs = sorted((x, o) for o, x in r_text.items()) if n_map.exists() else []
        lib0, lib1 = rlay[image]["libultra_text"]
        for f in funcs:
            if f["kind"] != "padding":
                prev = f if f["name"] in found and found[f["name"]][1] == "same" else None
                continue
            k = bisect.bisect_right(objs, (f["address"], "~")) - 1
            if lib0 <= f["address"] < lib1 and k >= 0 and objs[k][1] in n_text:
                b = n_text[objs[k][1]] + (f["address"] - objs[k][0])     # the same object's code: relocated
                rows.append({**f, "address": b})
                continue
            elif prev is not None:
                b = found[prev["name"]][0] + (f["address"] - prev["address"])
            else:
                continue
            if N.data[b - VRAM:b - VRAM + f["size"]] == R.data[f["address"] - VRAM:f["address"] - VRAM + f["size"]]:
                rows.append({**f, "address": b})
        # linker fill between objects: non-zero bytes no entry covers, in the code (not data, not markers or
        # microcode)
        rows.sort(key=lambda r: r["address"])
        code_hi = n_lo
        if image == "game":
            segs = nlayout_json["game"]["segments"]
            n_markers = [[segs[i + 1]["vram"], segs[i + 1]["vram"] + (b - a_)]
                         for i, (a_, b) in enumerate(rlay[image]["markers"])]
        else:
            n_markers = []
        for r0, r1 in zip(rows, rows[1:]):
            e, s = r0["address"] + r0["size"], r1["address"]
            if e < s and s <= code_hi + 0x10 and any(N.data[e - VRAM:s - VRAM]):
                nz = [x for x in range(e, s) if N.data[x - VRAM] and not any(m0 <= x < m1 for m0, m1 in n_markers)]
                if not nz:
                    continue
                p0, p1 = nz[0] & ~3, (nz[-1] | 3) + 1
                if s - e <= 0x10:
                    name = f"{'boot_' if image == 'boot' else ''}func_{p0:08X}"
                    if name in names:
                        name += "_fill"
                    rows.append({"name": name, "image": image, "address": p0, "size": p1 - p0, "module": "padding",
                                 "kind": "padding", "profile": "", "source": "", "copy_of": ""})
                    report.append(f"{image}: linker fill {p0:#x}+{p1 - p0:#x} after {r0['name']} (new entry)")
        rows.sort(key=lambda r: r["address"])
        for r0, r1 in zip(rows, rows[1:]):
            if r0["address"] + r0["size"] > r1["address"] and r0["kind"] == "padding" and r0["address"] < r1["address"]:
                r0["size"] = r1["address"] - r0["address"]     # REF's range ran into the object after it
            if r0["address"] + r0["size"] > r1["address"]:
                report.append(f"{image}: overlap: {r0['name']} {r0['address']:#x}+{r0['size']:#x} and "
                              f"{r1['name']} {r1['address']:#x}")
        new_funcs_rows += rows
        covered = [(r["address"], r["address"] + r["size"]) for r in rows]
        cstarts = [c[0] for c in covered]
        called = Counter()
        for r in rows:
            if r["kind"] == "padding":
                continue
            for x in range(r["address"], r["address"] + r["size"], 4):
                w = N.word(x)
                if w >> 26 == 3:
                    t = (x & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
                    k = bisect.bisect_right(cstarts, t) - 1
                    if not (k >= 0 and covered[k][0] == t) and VRAM <= t < N.end:
                        called[t] += 1
        for t, n in sorted(called.items()):
            report.append(f"{image}: called {n}x, no function starts there: {t:#x} (new function?)")

        # symbols: offsets voted by the pairs inside each symbol, the data alignment, then the neighbours
        fmap = {f["name"]: found[f["name"]][0] for f in funcs if f["name"] in found}
        syms = sorted((s for s in rsyms[image].values() if s.alias_of is None), key=lambda s: s.address)
        addrs = [s.address for s in syms]
        # a symbol without a size spans to the next symbol (libultra's included) or the end of its region
        bounds = sorted(set(addrs) | set(common.libultra_names(image, ref).values())
                        | {x for r in (rlay[image]["data"], rlay[image]["bss"]) for x in r} | {R.end})
        delta_votes = defaultdict(Counter)
        for A_, c in votes.items():
            k = bisect.bisect_right(addrs, A_) - 1
            if k < 0:
                continue
            s = syms[k]
            if s.size:
                lim = s.address + s.size
            elif not (VRAM <= s.address < R.end or rlay[image]["bss"][0] <= s.address < rlay[image]["bss"][1]):
                lim = s.address + 1          # outside the image and its BSS: the address itself only
            else:
                j = bisect.bisect_right(bounds, s.address)
                lim = bounds[j] if j < len(bounds) else s.address + 4
            if A_ < lim or A_ == s.address:
                for B_, n in c.items():
                    delta_votes[s.name][B_ - A_] += n
        out, how_sym = {}, {}
        prev_delta = 0
        for s in syms:
            if s.kind == "func" and s.name in fmap:
                out[s.name], how_sym[s.name] = fmap[s.name], "function"
            elif s.kind == "func" and s.name in byname:
                how_sym[s.name] = "removed"
            elif delta_votes.get(s.name):
                top = delta_votes[s.name].most_common()
                best = [d for d, n in top if n == top[0][1]]
                if len(best) > 1 and s.address in data_map:
                    out[s.name], how_sym[s.name] = data_map[s.address], "data"
                else:       # of equal votes, the offset nearest to the previous symbol's
                    d = min(best, key=lambda x: abs(x - prev_delta))
                    out[s.name], how_sym[s.name] = (s.address + d) & 0xFFFFFFFF, "refs"
            elif s.address in data_map:
                out[s.name], how_sym[s.name] = data_map[s.address], "data"
            if s.name in out:
                prev_delta = out[s.name] - s.address
        # the rest: the offset of the previous placed symbol (BSS without references, labels in asm)
        prev = None
        for s in syms:
            if s.name in out:
                prev = s
            elif how_sym.get(s.name) != "removed" and prev is not None:
                out[s.name] = (s.address + out[prev.name] - prev.address) & 0xFFFFFFFF
                how_sym[s.name] = "neighbour"
                report.append(f"{image}: symbol {s.name} {s.address:#x} -> {out[s.name]:#x} by {prev.name}'s offset")
        for s in syms:
            if s.name not in out:
                report.append(f"{image}: symbol {s.name} {s.address:#x}: not placed ({how_sym.get(s.name, 'no evidence')})")
            elif delta_votes.get(s.name) and len(delta_votes[s.name]) > 1:
                (d1, n1), (d2, n2) = delta_votes[s.name].most_common(2)
                report.append(f"{image}: symbol {s.name}: offsets disagree ({d1:#x} x{n1}, {d2:#x} x{n2})")
        for s in rsyms[image].values():
            if s.alias_of is not None and s.alias_of in out:
                out[s.name] = out[s.alias_of]
        new_syms[image] = out

        # layout
        lay = rlay[image]

        def mp(x, end=False):
            for f in funcs:
                if f["name"] in found and found[f["name"]][1] == "same":
                    if f["address"] == x and not end:
                        return found[f["name"]][0]
                    if f["address"] + f["size"] == x and end:
                        return found[f["name"]][0] + f["size"]
            if x in data_map:
                return data_map[x]
            if x - 4 in data_map and end:
                return data_map[x - 4] + 4
            if x == R.end:
                return N.end
            return None

        def find_bytes(lo, hi):
            blob = R.data[lo - VRAM:hi - VRAM]
            p = N.data.find(blob)
            return None if p < 0 else (VRAM + p, VRAM + p + (hi - lo))
        nl = {"data": [n_lo, n_hi]}
        bss = []
        for x in lay["bss"]:
            hit = [out[s.name] for s in syms if s.address == x and s.name in out]
            bss.append(hit[0] if hit else None)
        nl["bss"] = bss
        lib = [r for r in rows if r["kind"] == "libultra"]
        r_lib = [f for f in funcs if f["kind"] == "libultra"]
        nl["libultra_text"] = [min(r["address"] for r in lib) if lib else None,
                               (max(r["address"] + r["size"] for r in lib) if lib else 0)
                               + (lay["libultra_text"][1] - max(f["address"] + f["size"] for f in r_lib))]
        nl["markers"] = n_markers
        nl["microcode_text"] = [list(find_bytes(a_, b) or (None, None)) for a_, b in lay["microcode_text"]]
        nl["microcode_data"] = [list(find_bytes(a_, b) or (None, None)) + [w] for a_, b, w in lay["microcode_data"]]
        nl["asm"] = [[mp(a_), mp(b, True)] for a_, b in lay["asm"]]
        nl["from_rom"] = [[mp(r[0]), mp(r[1], True)] + r[2:] for r in lay.get("from_rom", [])]
        nl["split_junk"] = [mp(x) for x in lay.get("split_junk", [])]
        layout_out[image] = nl
        layout_out.setdefault("asm_files", {})
        for fname, (a_, b) in rlay["asm_files"].items():
            if fname.startswith(image + "_"):
                layout_out["asm_files"][fname] = [mp(a_), mp(b, True)]
        c = Counter(how for _, how in found.values())
        report.insert(0, f"{image}: {len(funcs)} entries of {ref}: found {sum(c.values())} ({dict(c)}); "
                         f"symbols placed {len(out)} of {len(rsyms[image])} ({dict(Counter(how_sym.values()))})")

    print("\n".join(report))
    print("\ndraft layout for versions/%s/version.toml:" % new)

    def fmt(v):
        if isinstance(v, list):
            return "[" + ", ".join(fmt(x) for x in v) + "]"
        if isinstance(v, int):
            return f"0x{v:08X}"
        if v is None:
            return "None"
        return json.dumps(v)
    for image in common.IMAGES:
        print(f"[{image}]")
        for k, v in layout_out[image].items():
            print(f"{k} = {fmt(v)}")
    print("[asm_files]")
    for k, v in layout_out["asm_files"].items():
        print(f'"{k}" = {fmt(v)}')
    if not a.write:
        return
    d = Path(a.out) if a.out else common.VERSIONS / new
    d.mkdir(parents=True, exist_ok=True)
    for name in ("functions.csv", "symbols.txt"):
        if (d / name).exists() and not a.out:
            sys.exit(f"{d / name} exists: not overwritten")
    rows = sorted(new_funcs_rows, key=lambda r: (r["image"] != "game", r["address"]))
    buf = io.StringIO()
    w = csv.DictWriter(buf, fieldnames=["name", "image", "address", "size", "module", "kind", "profile", "source",
                                        "copy_of"], lineterminator="\n")
    w.writeheader()
    for r in rows:
        w.writerow({**r, "address": f"0x{r['address']:08X}", "size": str(r["size"])})
    (d / "functions.csv").write_text(buf.getvalue(), newline="\n")
    # symbols.txt: REF's file with NEW's addresses; symbols not placed are left out
    lines = []
    for line in common.table_path("symbols.txt", ref).read_text().splitlines():
        f = line.split("#", 1)[0].split()
        if not f:
            lines.append(line)
            continue
        name, image = f[0], f[1]
        if name not in new_syms[image]:
            continue
        f[2] = f"0x{new_syms[image][name]:08X}"
        if f[3] == "func":
            row = next((r for r in rows if r["image"] == image and r["name"] == name), None)
            if row is not None and f[4] != "-":
                f[4] = f"{row['size']:#x}"
        lines.append(f"{f[0]:<36}  {f[1]:4}  {f[2]}  {f[3]:4}  {f[4]:>6}" + (f"  {f[5]}" if len(f) > 5 else ""))
    import rename
    (d / "symbols.txt").write_text(rename.format_symbols("\n".join(lines) + "\n"), newline="\n")
    print(f"wrote {d / 'functions.csv'} and {d / 'symbols.txt'}")


if __name__ == "__main__":
    main()
