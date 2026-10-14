"""Full build of one version of Excitebike 64 from source, with proof of what is reused from the original images.

The version: --version NAME, else the VERSION setting, else lodgenet (tools/common.py; versions/<name>/).
1. Copy the original images (build/<version>/extracted/game.bin, boot.bin) to OUT and poison every byte -- replace
   it with its complement (default; no original byte can survive unnoticed, not even one that equals a fixed poison
   value) or with --poison 0xNN -- except the ranges keep() lists (RSP microcode text and data, segment markers;
   version.toml): every
   function -- C, libultra, hand-written asm, linker padding alike --, all initialized data, the gaps between
   objects.
2. build_code.py      decompiled C (game, boot loader, boot copies of game functions), the compilers' literals
   build_asm.py       hand-written asm (entry stubs, SN exception handler, SwdExpand from SWDN64.ASM, its data,
                      the sound mixers and ADPCM decoders among the game's data)
   build_libultra.py  libultra 2.0K libgultra_rom (ultralib, KMC GCC 2.7.2) + libgcc, and their data
   build_data.py      the game's and boot loader's initialized data from src/data/*.c, and the data that is ROM
                      content (build_data.FROM_ROM), copied from the extracted images
3. Bytes still poisoned: inside a `padding` entry of functions.csv they are linker fill between objects (no
   compiler or assembler produces it): copied from the original and listed in OUT/restored_fill.txt. Between
   objects they are alignment: zero-filled, as the linker does. Then both images must equal the originals byte for
   byte.
4. build_rom.py       boot.bin + SWD32-packed game.bin into the original ROM layout, header checksum; SHA1 check.
5. Coverage: every byte of both images, by where it came from.

usage: python tools/build_all.py [OUT] [--version NAME] [--poison invert|0xNN] [--jobs N]
       (default OUT: build/<version>/out)
"""
import argparse
import subprocess
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
import build_data  # noqa: E402

VRAM = common.VRAM
IMAGES = common.IMAGES
DATA_RANGES = common.DATA_RANGES


def keep(image):
    """Taken from the original images, never poisoned (version.toml: markers, microcode_text, microcode_data)."""
    lay = common.layout(image)
    return ([(a, b, "segment markers") for a, b in lay["markers"]]
            + [(a, b, "RSP microcode text") for a, b in lay["microcode_text"]]
            + [(a, b, "RSP microcode data") for a, b, _ in lay["microcode_data"]])


KEEP = {k: keep(k) for k in IMAGES}
STEPS = [("build_code.py", "C"), ("build_asm.py", "asm"), ("build_libultra.py", "libultra"),
         ("build_data.py", "data C")]


def run(args, check=True):
    print("$", " ".join(str(a) for a in args[1:]))
    r = subprocess.run([sys.executable, *map(str, args)], cwd=common.ROOT, capture_output=True, text=True)
    out = (r.stdout + r.stderr).strip().splitlines()
    for line in out[-8:]:
        print("   ", line)
    if r.returncode and check:
        sys.exit(f"step failed ({r.returncode})")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("out", nargs="?", default=str(common.OUT))
    ap.add_argument("--poison", default="invert", help="invert (each byte's complement, default) or a byte value")
    ap.add_argument("--jobs", type=int, default=8)
    common.add_version_arg(ap)
    a = ap.parse_args()
    print(f"building {common.TITLE} (version {common.VERSION})")
    out = common.output_dir(a.out)
    fixed = None if a.poison == "invert" else int(a.poison, 0)
    bad_byte = lambda k, i: orig[k][i] ^ 0xFF if fixed is None else fixed  # noqa: E731
    out.mkdir(parents=True, exist_ok=True)
    table = common.functions()
    orig = {k: common.original(k) for k in IMAGES}
    keep = {k: bytearray(len(orig[k])) for k in IMAGES}          # 1 = never poisoned
    for k in IMAGES:
        for lo, hi, _ in KEEP[k]:
            keep[k][lo - VRAM:hi - VRAM] = b"\1" * (hi - lo)
    img = {k: bytearray(v) for k, v in orig.items()}
    for k in IMAGES:
        for i in range(len(img[k])):
            if not keep[k][i]:
                img[k][i] = bad_byte(k, i)
        (out / f"{k}.bin").write_bytes(bytes(img[k]))
    n_keep = {k: sum(keep[k]) for k in IMAGES}
    print(f"poisoned ({'complement of each byte' if fixed is None else f'{fixed:#04x}'}) everything but RSP "
          "microcode and segment markers: "
          + ", ".join(f"{k} {len(img[k]) - n_keep[k]} of {len(img[k])} bytes" for k in IMAGES))

    # 2. the build steps; who changed each byte
    writer = {k: [None] * len(orig[k]) for k in IMAGES}
    prev = {k: bytes(img[k]) for k in IMAGES}
    for step, label in STEPS:
        if step == "build_code.py":
            run([HERE / step, out, "--base", out, "--jobs", a.jobs])
        else:
            run([HERE / step, out], check=step != "build_libultra.py")  # libultra exits 1 while fill differs
        cur = {k: (out / f"{k}.bin").read_bytes() for k in IMAGES}
        for k in IMAGES:
            p, c, w = prev[k], cur[k], writer[k]
            for i in range(len(c)):
                if c[i] != p[i]:
                    w[i] = label
        prev = cur
    for k in IMAGES:                     # data the build copies from the ROM, not from C
        for lo, hi, *_ in build_data.FROM_ROM.get(k, []):
            for i in range(lo - VRAM, hi - VRAM):
                if writer[k][i] == "data C":
                    writer[k][i] = "ROM data"
    img = {k: bytearray(prev[k]) for k in IMAGES}

    # 3. linker fill inside padding entries (copied), alignment between objects (zero), nothing else
    entry = {k: bytearray(len(orig[k])) for k in IMAGES}       # 1 = function entry, 2 = padding entry
    for f in table:
        o = f["address"] - VRAM
        entry[f["image"]][o:o + f["size"]] = (b"\2" if f["kind"] == "padding" else b"\1") * f["size"]
    restored = {k: 0 for k in IMAGES}
    fill_list, zero_fill, bad = [], {k: 0 for k in IMAGES}, []
    for f in table:
        k, o, n = f["image"], f["address"] - VRAM, f["size"]
        diff = [i for i in range(o, o + n) if img[k][i] != orig[k][i]]
        if diff and entry[k][o] == 2:
            for i in diff:
                img[k][i] = orig[k][i]
                writer[k][i] = "fill"
            restored[k] += len(diff)
            fill_list.append(f"{k} {f['name']} {f['address']:#x}+{n}: {len(diff)} bytes {orig[k][o:o + n].hex()}")
    for k in IMAGES:
        lo, hi = DATA_RANGES[k]
        for i in range(len(img[k])):
            if not keep[k][i] and not entry[k][i] and not lo <= i + VRAM < hi and writer[k][i] is None \
                    and img[k][i] == bad_byte(k, i):
                img[k][i] = 0
                writer[k][i] = "zero"
                zero_fill[k] += 1
        diff = [i for i in range(len(img[k])) if img[k][i] != orig[k][i]]
        if diff:
            runs = []                   # [first, last] of each run of differing bytes
            for i in diff:
                if runs and i == runs[-1][1] + 1:
                    runs[-1][1] = i
                else:
                    runs.append([i, i])
            where = []
            for s, e in runs[:24]:
                ad = s + VRAM
                f = next((f["name"] for f in table
                          if f["image"] == k and f["address"] <= ad < f["address"] + f["size"]),
                         "data" if lo <= ad < hi else "between objects")
                where.append(f"{ad:#010x}+{e - s + 1} ({f}): {img[k][s:e + 1].hex()}, original "
                             f"{orig[k][s:e + 1].hex()}")
            bad.append(f"{k}: {len(diff)} bytes in {len(runs)} runs differ from the original:\n    "
                       + "\n    ".join(where))
    if bad:
        print("NOT REBUILT:", *bad, sep="\n  ")
        sys.exit(1)
    (out / "restored_fill.txt").write_text("\n".join(fill_list) + "\n")
    print(f"linker fill copied from the original: {len(fill_list)} entries listed in {out / 'restored_fill.txt'}")
    for k in IMAGES:
        (out / f"{k}.bin").write_bytes(bytes(img[k]))
        print(f"{k}.bin: identical to the original: {bytes(img[k]) == orig[k]}; linker fill copied: {restored[k]} "
              f"bytes, alignment zero-filled: {zero_fill[k]} bytes")

    # 4. ROM
    lines = run([HERE / "build_rom.py", out, out / "excitebike64.z64"])
    if not any("== ORIGINAL" in line for line in lines):
        sys.exit("ROM does not match the original")

    # 5. coverage: every byte by origin
    print("coverage (bytes of the decompressed images):")
    total = Counter()
    labels = [s[1] for s in STEPS]
    for k in IMAGES:
        lo, hi = DATA_RANGES[k]
        c = Counter()
        kept = {}
        for x, y, name in KEEP[k]:
            kept[name] = kept.get(name, 0) + (y - x)
        for i, w in enumerate(writer[k]):
            if keep[k][i]:
                continue
            area = "data" if lo <= i + VRAM < hi else "code"
            if w is None:
                w = f"unwritten, original byte equals the poison {fixed or 0:#04x} (try --poison invert)"
            c[(area, w)] += 1
        src = {area: {w: n for (ar, w), n in c.items() if ar == area and w in labels} for area in ("code", "data")}
        orig_parts = {**kept, "ROM data": c[("data", "ROM data")], "linker fill": c[("code", "fill")]}
        other = {f"{ar} {w}": n for (ar, w), n in c.items() if w not in labels + ["fill", "zero", "ROM data"]}
        print(f"  {k} ({len(orig[k])} bytes):")
        names = {"code": {"C": "C", "asm": "asm", "libultra": "libultra"},
                 "data": {"C": "literals of the C", "asm": "asm", "libultra": "libultra", "data C": "src/data C"}}
        for area in ("code", "data"):
            print(f"    {area} from source: {sum(src[area].values())} ("
                  + ", ".join(f"{names[area].get(w, w)} {n}"
                              for w, n in sorted(src[area].items(), key=lambda x: -x[1])) + ")")
        print(f"    alignment between objects, zero-filled: {c[('code', 'zero')] + c[('data', 'zero')]}")
        print(f"    from the original image: {sum(orig_parts.values())} ("
              + ", ".join(f"{w} {n}" for w, n in orig_parts.items() if n) + ")")
        for w, n in other.items():
            print(f"    {w}: {n}")
        total["code"] += sum(src["code"].values())
        total["data"] += sum(src["data"].values())
        total["zero"] += c[("code", "zero")] + c[("data", "zero")]
        total["orig"] += sum(orig_parts.values())
    print(f"  total: code {total['code']} + data {total['data']} bytes from source, {total['zero']} alignment, "
          f"{total['orig']} from the original images")
    print(f"OK: {out / 'excitebike64.z64'} is the original ROM, built from source")


if __name__ == "__main__":
    main()
