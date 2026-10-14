"""Find every function in the extracted images (build/extracted/boot.bin, game.bin). Needs rabbitizer.

Seeds are jal targets and region starts; each function's extent is what is reachable from its entry
(branches, in-function jumps, jump tables) up to the next seed; the first non-nop word after a function
starts the next one. Modules are the source file names the functions' strings give (__FILE__ arguments),
or segment_address for groups of functions between two object-file boundaries that name none.
Output: build/split.json, one record per function: {image, address, size, name, segment, module, exclude?},
and a comparison with functions.csv (which was made from it; its `kind` column says how each range is built).

usage: python tools/split.py
"""
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402

try:
    import rabbitizer
except ImportError:
    sys.exit("split.py needs rabbitizer: python -m pip install rabbitizer")

rabbitizer.config.regNames_gprAbiNames = rabbitizer.Abi.O32
VRAM = 0x80000400
# Ranges that are not SN-compiled C (listed with the reason).
EXCLUDE = {
    "game": [(0x80000400, 0x80000450, "entry point stub (hand-written asm)"),
             (0x800AC240, 0x800B767C, "libultra (Nintendo's libgultra_rom, built with KMC GCC 2.7.2)")],
    "boot": [(0x80000400, 0x80000450, "entry point stub (hand-written asm)"),
             (0x80002CB0, 0x80003660, "SN debug/exception stub (hand-written asm)"),
             (0x80005C70, 0x8000CE10, "libultra (same code as the game's copy)"),
             (0x800113E4, 0x80011600, "SwdExpand decompressor (hand-written asm, Brandwood's SWDN64.ASM)")],
}
# Junk padding found by hand (not caught by the rules below).
JUNK = {"game": {0x800B767C}}

# Code ranges per image, and ranges inside them that are not CPU code.
IMAGES = {
    "game": {
        "code": [(0x80000400, 0x800C2280)],
        "data": (0x800C2280, None),
        "not_code": [
            (0x800731D0, 0x80073200, "segment markers"),
            (0x800A42C0, 0x800A42D0, "segment marker"),
            (0x800A9C20, 0x800AC240, "segment marker + RSP microcode text"),
        ],
        "segments": [("s0", 0x80000400, 0x800731D0), ("s1", 0x800731D0, 0x800A42C0),
                     ("s2", 0x800A42C0, 0x800A9C20), ("s3", 0x800A9C20, 0x800DC098)],
    },
    "boot": {
        "code": [(0x80000400, 0x8000CE10), (0x800113E4, 0x80011600)],
        "data": (0x8000CE10, 0x800113E4),
        "not_code": [(0x80003660, 0x80005C70, "RSP microcode text (rspboot, F3DEX, F3DLX)")],
        "segments": [("boot", 0x80000400, 0x80011600)],
    },
}


class Image:
    def __init__(self, name, data):
        self.name, self.data = name, data
        self.end = VRAM + len(data)

    def word(self, a):
        return struct.unpack(">I", self.data[a - VRAM:a - VRAM + 4])[0]

    def ins(self, a):
        return rabbitizer.Instruction(self.word(a), a)


def never_returns(img, a):
    """The function at a spins on itself (`j .`) before any `jr` in its first 16 instructions."""
    for k in range(16):
        x = a + 4 * k
        i = img.ins(x)
        if not i.isValid() or i.isJrRa() or i.isJrNotRa():
            return False
        if i.isJump() and not i.doesLink() and i.isJumpWithAddress() and i.getInstrIndexAsVram() == x:
            return True
    return False


def is_prologue(img, a):
    """A function that allocates a frame starts at a: `addiu $sp, $sp, -N` within its first 4 instructions, with
    no branch, jump or $sp use before it (GCC schedules loads like `lui; lw` ahead of the allocation)."""
    for k in range(4):
        w = img.word(a + 4 * k)
        if (w & 0xFFFF0000) == 0x27BD0000:
            # what runs before the allocation must not read registers nothing set: main (0x8000D2E0) had three junk
            # words ('sw $zero, 4($t2)') in front of its prologue, and every branch target was off by 0xC
            return (w & 0x8000) != 0 and not (k and reads_unset(img, a, a + 4 * k))
        i = img.ins(a + 4 * k)
        if not i.isValid() or i.isBranch() or i.isJump() or ((w >> 21) & 31) == 29 or ((w >> 16) & 31) == 29:
            return False
    return False


def extent(img, entry, limit, jt_targets):
    """End address (exclusive) of the code reachable from entry, never past limit."""
    seen = set()
    todo = [entry]
    while todo:
        a = todo.pop()
        while entry <= a < limit and a not in seen:
            seen.add(a)
            i = img.ins(a)
            if not i.isValid():
                break
            if i.isJrRa():
                seen.add(a + 4)
                break
            if i.isJrNotRa():
                # Switch cases, not function pointers that happen to point into range (a stack-allocating prologue
                # is never a case label; shell_update swallowed the function its pointer table named).
                todo.extend(t for t in jt_targets if entry < t < limit and not is_prologue(img, t))
                seen.add(a + 4)
                break
            if i.isBranch() or (i.isJump() and not i.doesLink() and i.isJumpWithAddress()):
                t = i.getBranchVramGeneric() if i.isBranch() else i.getInstrIndexAsVram()
                inside = entry <= t < limit
                if inside:
                    todo.append(t)
                if i.isUnconditionalBranch() or i.isJump():
                    seen.add(a + 4)
                    break
            a += 4
    return max(seen) + 4 if seen else entry


def code_refs(img, ranges, not_code):
    """Code addresses that code forms with lui/addiu (function pointers), plus jal targets."""
    out = set()
    hi = {}
    for lo_, hi_ in ranges:
        for a in range(lo_, hi_, 4):
            if in_ranges(a, not_code):
                continue
            i = img.ins(a)
            op = i.getOpcodeName()
            if op == "jal":
                out.add(i.getInstrIndexAsVram())
            elif op == "lui":
                hi[i.rt] = i.getProcessedImmediate() << 16
            elif op == "addiu" and i.rs in hi:
                out.add((hi[i.rs] + i.getProcessedImmediate()) & 0xFFFFFFFF)
    return out


def in_ranges(a, ranges):
    return any(lo <= a < hi for lo, hi, *_ in ranges)


def split_image(img, spec, ignore=()):
    """ignore: (start, end) ranges known to be junk; jal words inside them are not calls."""
    not_code = spec["not_code"]
    ranges = spec["code"]
    in_code = lambda a: any(lo <= a < hi for lo, hi in ranges) and not in_ranges(a, not_code)  # noqa: E731
    # Seeds: jal targets inside code, range starts, the end of each non-code hole.
    seeds = {lo for lo, _ in ranges} | {hi for _, hi, _ in not_code}
    for j in JUNK.get(img.name, ()):
        seeds |= {j, (j | 15) + 1}
    for lo, hi in ranges:
        for a in range(lo, hi, 4):
            w = img.word(a)
            if w >> 26 == 3 and in_code(a) and not any(lo_ <= a < hi_ for lo_, hi_ in ignore):
                t = ((w & 0x3FFFFFF) << 2) | 0x80000000
                if in_code(t):
                    seeds.add(t)
    # Jump-table targets: words outside code that point into code.
    jt = set()
    for a in range(VRAM, img.end - 3, 4):
        if in_code(a):
            continue
        w = img.word(a)
        if in_code(w) and w % 4 == 0:
            jt.add(w)
    refs = code_refs(img, ranges, not_code) | jt
    # Data words pointing at a stack-allocating prologue are function pointers: those functions start there.
    seeds |= {t for t in jt if is_prologue(img, t)}
    # After a call to a function that never returns (fatal_halt: `break 0x407; j .`), GCC emits no epilogue, so
    # a function pointed to from data, or starting with a prologue, right after the call's delay slot begins there
    # (nes_op_sed had swallowed the NES opcode handler at 0x800A3834).
    for lo, hi in ranges:
        for a in range(lo, hi, 4):
            w = img.word(a)
            if w >> 26 == 3 and in_code(a) and not any(lo_ <= a < hi_ for lo_, hi_ in ignore):
                t = ((w & 0x3FFFFFF) << 2) | 0x80000000
                nxt = a + 8
                if in_code(t) and in_code(nxt) and never_returns(img, t) and (nxt in jt or is_prologue(img, nxt)):
                    seeds.add(nxt)
    junk = set()
    funcs = []
    for lo, hi in ranges:
        order = sorted(x for x in seeds if lo <= x < hi)
        i = 0
        while i < len(order):
            s = order[i]
            nxt = order[i + 1] if i + 1 < len(order) else hi
            for nlo, nhi, _ in not_code:
                if s < nlo < nxt:
                    nxt = nlo
            e = max(extent(img, s, nxt, jt), s + 4)
            funcs.append((s, e))
            p = e
            while p < nxt and img.word(p) == 0:
                p += 4
            if p < nxt and p not in order:
                # Junk padding before a function that is only reached through a pointer: the referenced,
                # 16-byte aligned address is the real start. Unreferenced ones (anti-piracy checks, reached by
                # computed addresses) show it by reading registers nothing set (`subu $v0, $v0, $v1` at entry).
                q = (p | 15) + 1
                # Never when p is itself a prologue: a junk word that decodes as `jal q` made func_800B8300 lose
                # its `addiu $sp` at 0x800B82FC.
                if p % 16 and q < nxt and (q in refs or reads_unset(img, p, q)) and q not in order \
                        and not is_prologue(img, p):
                    funcs.append((p, q))
                    junk.add(p)
                    p = q
                order = sorted(order + [p])
            i = order.index(s) + 1
    return sorted(funcs), ranges[-1][1], junk


# Registers a function may read before writing them: arguments, stack, return address, $zero, and the
# callee-saved ones its prologue stores.
ENTRY_GPRS = {"zero", "a0", "a1", "a2", "a3", "sp", "ra", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "fp"}


def reads_unset(img, s, e):
    """Words [s, e) read a GPR that nothing set: not the start of a function."""
    written = set()
    for a in range(s, e, 4):
        i = rabbitizer.Instruction(img.word(a), a)
        if not i.isValid():
            return True
        reads = ([i.rs.name] if i.readsRs() else []) + ([i.rt.name] if i.readsRt() else []) +                 ([i.rd.name] if i.readsRd() else [])
        if any(r not in ENTRY_GPRS and r not in written for r in reads):
            return True
        for mod, reg in ((i.modifiesRd, "rd"), (i.modifiesRt, "rt"), (i.modifiesRs, "rs")):
            if mod():
                written.add(getattr(i, reg).name)
    return False


def jt_and_calls(img, spec):
    """Addresses that are called (jal) or pointed to by a data word."""
    out = set()
    for a in range(VRAM, img.end - 3, 4):
        w = img.word(a)
        if w >> 26 == 3:
            out.add(((w & 0x3FFFFFF) << 2) | 0x80000000)
        elif VRAM <= w < img.end:
            out.add(w)
    return out


def is_junk(img, s, e, nxt, refd, junk, name):
    """Linker padding between object files is sometimes junk bytes that decode as code."""
    if s in refd and s not in JUNK.get(name, ()) and e <= nxt and has_return(img, s, min(e, nxt)) \
            and not reads_unset(img, s, min(e, nxt)):
        return False       # pointed to from data and returns: a real function (empty callbacks `jr $ra; nop`)
    if s in junk or s in JUNK.get(name, ()) or e > nxt:
        return True        # junk-prefixed start, found by hand, or a 'return' whose delay slot is the next function
    if e - s < 16 and s not in refd and not has_return(img, s, e) and (e % 16 in (0, 4) or (e + 4) % 16 == 0):
        return True        # a few words, never called or pointed to, no return, ending at a file boundary
    words = [img.word(a) for a in range(s, min(e, nxt), 4)]
    calls = any(w >> 26 == 3 for w in words)
    saves_ra = any((w & 0xFFFF0000) == 0xAFBF0000 for w in words)     # sw $ra, X($sp)
    return calls and not saves_ra   # GCC always saves $ra before a call


def has_return(img, s, e):
    return any(img.word(a) == 0x03E00008 for a in range(s, e, 4))


def string_at(img, a):
    off = a - VRAM
    if not (0 <= off < len(img.data)):
        return None
    m = re.match(rb"([A-Za-z0-9_]+\.c)\x00", img.data[off:off + 40])
    return m.group(1).decode() if m else None


def file_refs(img, s, e):
    """Source-file name strings (__FILE__-style) the function builds with lui/addiu pairs."""
    his, names = {}, []
    for a in range(s, e, 4):
        i = img.ins(a)
        op = i.getOpcodeName()
        if op == "lui":
            his[i.rt] = i.getProcessedImmediate() << 16
        elif op in ("addiu", "ori") and i.rs in his:
            v = (his[i.rs] + (i.getProcessedImmediate() if op == "addiu" else i.getImmediate())) & 0xFFFFFFFF
            n = string_at(img, v)
            if n and n not in names:
                names.append(n)
    return names


def skeleton(img, s, size):
    """Instruction words with addresses masked (jal/j targets, lui and %lo immediates)."""
    out = []
    for a in range(s, s + size, 4):
        w = img.word(a)
        op = w >> 26
        if op in (2, 3):
            w &= 0xFC000000
        elif op in (0x0F, 0x09, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B, 0x31, 0x35, 0x39, 0x3D):
            w &= 0xFFFF0000
        out.append(w)
    return tuple(out)


def mark_duplicates(recs, game_recs, img):
    gimg = Image("game", common.original("game"))
    seen, exact = {}, {}
    for g in game_recs:
        if "exclude" not in g:
            seen.setdefault(skeleton(gimg, g["address"], g["size"]), g["name"])
            exact.setdefault(gimg.data[g["address"] - VRAM:g["address"] - VRAM + g["size"]], g["name"])
    for r in recs:
        if "exclude" not in r:
            # an identical copy first: the skeleton masks every addiu immediate, so toupper (-0x61/-0x20) and
            # tolower (-0x41/+0x20) looked alike and boot_char_to_lower was paired with the wrong one
            twin = exact.get(img.data[r["address"] - VRAM:r["address"] - VRAM + r["size"]]) or \
                seen.get(skeleton(img, r["address"], r["size"]))
            if twin:
                r["exclude"] = f"same code as the game's {twin}"


def main():
    out = []
    for name, spec in IMAGES.items():
        img = Image(name, common.original(name))
        refd = jt_and_calls(img, spec)
        ignore: list = []
        for _ in range(6):
            funcs, code_hi, junk = split_image(img, spec, ignore)
            starts = sorted(x for x, _ in funcs)
            nexts = dict(zip(starts, starts[1:] + [code_hi]))
            bad = sorted((s, min(e, nexts[s])) for s, e in funcs if is_junk(img, s, e, nexts[s], refd, junk, name))
            if bad == ignore:
                break
            ignore = bad
        # A function that starts in an excluded asm range and runs past its end is two: the boot entry stub
        # (asm, to 0x80000450) falls through into the boot loader's main, compiled C.
        cut = []
        for s, e in funcs:
            for lo, hi, why in EXCLUDE[name]:
                if lo <= s < hi < min(e, nexts[s]):
                    cut.append((s, hi))
                    s = hi
                    break
            if not any(nlo <= s < nhi for nlo, nhi, _ in spec.get("not_code", [])):
                cut.append((s, e))      # (a remainder that is RSP microcode is not a function)
        if len(cut) != len(funcs):
            funcs = cut
            starts = sorted(x for x, _ in funcs)
            nexts = dict(zip(starts, starts[1:] + [code_hi]))
        recs = []
        for s, e in funcs:
            e = min(e, nexts[s])
            seg = next((n for n, lo, hi in spec["segments"] if lo <= s < hi), "?")
            r = {"image": name, "address": s, "size": e - s, "segment": seg, "files": file_refs(img, s, e)}
            if (s, e) in ignore:
                r["exclude"] = "linker padding (junk bytes between object files)"
            recs.append(r)
        # Object-file boundaries: padding gaps and junk records. Group functions into chunks between them.
        chunk, chunks = [], []
        prev_end = None
        for r in recs:
            boundary = prev_end is None or r["address"] != prev_end or "exclude" in r
            if boundary and chunk:
                chunks.append(chunk)
                chunk = []
            if "exclude" not in r:
                chunk.append(r)
            prev_end = r["address"] + r["size"]
        if chunk:
            chunks.append(chunk)
        for ch in chunks:
            names = [f for r in ch for f in r["files"]]
            mod = max(set(names), key=names.count).removesuffix(".c") if names else None
            for r in ch:
                r["module"] = mod or f"{r['segment']}_{ch[0]['address']:08X}"
                r["chunk"] = ch[0]["address"]
        for r in recs:
            r["name"] = f"{'boot_' if name == 'boot' else ''}func_{r['address']:08X}"
            r.setdefault("module", "padding")
            if name == "game" and 0x800AC240 <= r["address"] < 0x800B767C:
                r["module"] = "libultra"
            elif name == "game" and r["address"] >= 0x800B767C and r["module"].startswith("s3_"):
                r["module"] = "lib_" + r["module"][3:]
            elif name == "boot":
                r["module"] = "boot"
            for lo, hi, why in EXCLUDE[name]:
                if lo <= r["address"] < hi and "exclude" not in r:
                    r["exclude"] = why
        if name == "boot":
            mark_duplicates(recs, out, img)
        out += recs
        print(f"{name}: {len(recs)} functions, code end {code_hi:#x}")
    (common.BUILD / "split.json").write_text(json.dumps(out, indent=0))
    have = {(f["image"], f["address"], f["size"]) for f in common.functions()}
    got = {(r["image"], r["address"], r["size"]) for r in out}
    print(f"wrote build/split.json: {len(out)} records; functions.csv has {len(have)}, "
          f"{len(have & got)} with the same address and size")
    for k, a, n in sorted(have ^ got)[:20]:
        print(f"  only in {'functions.csv' if (k, a, n) in have else 'split.json'}: {k} {a:#010x} +{n:#x}")


if __name__ == "__main__":
    main()
