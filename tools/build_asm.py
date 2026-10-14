"""Assemble the hand-written asm (asm/*.s) and write it into a build's code images.

usage: python tools/build_asm.py [OUT] [--version NAME]      (default: build/<version>/out)

Each asm/*.s names its image in its header (`# image: boot`); version.toml's [asm_files] gives its range in the
version being built (the addresses in the files' comments are LodgeNet's). Every file is assembled with GNU as for
MIPS (MIPS_PREFIX, default mips-linux-gnu-; -march=vr4300 -mabi=32 -G0 -EB) and linked with ld at its range's
start (through WSL on Windows); the symbols of its image (symbols.txt) it uses but doesn't define are given to as
and ld with --defsym (as needs them as numbers for what a relocation can't express, like the halves of a lui/ori
pair: `(sym >> 16)`, `(sym & 0xFFFF)`), and so are VERSION and the VERSION_<NAME> values of include/version.h, for
`.if VERSION == VERSION_US_REV1` blocks. Then, in OUT/boot.bin and OUT/game.bin, every asm block (version.toml
`asm`) and every file's range is zeroed and the assembled bytes are written there, and each image is compared
with the original. Nothing is written if a file fails to assemble.
"""
import re
import shlex
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402

ROOT = common.ROOT
ASM = ROOT / "asm"
VRAM = common.VRAM
# The ranges of the original images that are hand-written asm (functions.csv kind `asm`; the SN stub and SwdExpand
# ranges include the bytes around their functions, SwdExpand's starts at its module's variables, the sound
# routines among the game's data include the decoders' tables). Every byte of these must come from some asm/*.s.
BLOCKS = {k: [tuple(r) for r in common.layout(k)["asm"]] for k in common.IMAGES}


def parse(src: Path):
    text = src.read_text()
    img = re.search(r"^#\s*image:\s*(\w+)", text, re.M)
    if not img:
        sys.exit(f"{src.name}: header needs `# image: <boot|game>`")
    rng = common.version_info()["asm_files"].get(src.name)
    if not rng:
        sys.exit(f"{src.name}: no range in versions/{common.VERSION}/version.toml [asm_files]")
    defined = set(re.findall(r"^\s*([A-Za-z_.$][\w.$]*):", text, re.M))
    code = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    code = re.sub(r"#.*", "", code)
    resolve = common.resolver(img.group(1))
    used = {n for n in re.findall(r"(?<![\w.$])([A-Za-z_]\w*)", code) if n not in defined}
    ext = sorted((n, resolve(n)) for n in used if resolve(n) is not None)
    return img.group(1), rng[0], rng[1], ext


def main():
    args = common.argv()
    build = common.output_dir(args[1] if len(args) > 1 else common.OUT)
    files = sorted(ASM.glob("*.s"))
    if not files:
        sys.exit(f"no .s files in {ASM}")
    x = common.setting("MIPS_PREFIX", "mips-linux-gnu-")
    jobs = []
    with tempfile.TemporaryDirectory() as td:
        tdir = Path(td)
        script = ["set -e"]
        for f in files:
            img, lo, hi, ext = parse(f)
            inside = [n for n, a in ext if lo <= a < hi]
            if inside:
                print(f"warning: {f.name} uses {', '.join(inside)} inside its own range but doesn't define it")
            stem = shlex.quote(common.posix_path(tdir / f.stem))
            defs = " ".join(f"--defsym {n}=0x{a:08X}" for n, a in ext)
            vdefs = " ".join([f"--defsym VERSION={common.VERSION_NUMBER}"]
                             + [f"--defsym VERSION_{v.upper()}={n}" for v, n in common.version_numbers().items()])
            script += [f"echo '== {f.name}'",
                       f"{x}as -march=vr4300 -mabi=32 -G0 -EB {vdefs} {defs} -o {stem}.o "
                       f"{shlex.quote(common.posix_path(f))}",
                       # as gives .text 16-byte alignment; ld would pad a start like 0x80011584 up to 0x80011590
                       f"{x}objcopy --set-section-alignment .text=4 {stem}.o",
                       f"{x}ld -EB -Ttext=0x{lo:08X} -e 0x{lo:08X} {defs} -o {stem}.elf {stem}.o",
                       f"{x}objcopy -O binary -j .text {stem}.elf {stem}.bin"]
            jobs.append((f, img, lo, hi))
        sh = tdir / "build.sh"
        sh.write_text("\n".join(script) + "\n", newline="\n")
        rc, out = common.run_posix(sh)
        if rc:
            sys.exit(f"assembly failed (nothing written):\n{out}")
        if any(not line.startswith("== ") for line in out.splitlines()):
            print(out)                                  # assembler/linker warnings
        blobs = []
        for f, img, lo, hi in jobs:
            b = (tdir / f"{f.stem}.bin").read_bytes()
            n = hi - lo
            if len(b) < n or any(b[n:]):
                sys.exit(f"{f.name}: assembled {len(b):#x} bytes, range {lo:#x}-{hi:#x} needs {n:#x} "
                         f"(nothing written)")
            blobs.append((f, img, lo, hi, b[:n]))

    # The ranges must not overlap and must cover every asm block.
    cover = {}
    for f, img, lo, hi, _ in blobs:
        for a in range(lo, hi):
            if (img, a) in cover:
                sys.exit(f"{f.name} overlaps {cover[img, a]} at {a:#x}")
            cover[img, a] = f.name
    for img, blocks in BLOCKS.items():
        for lo, hi in blocks:
            miss = [a for a in range(lo, hi) if (img, a) not in cover]
            if miss:
                print(f"warning: {img} {miss[0]:#x}-{miss[-1] + 1:#x}: {len(miss)} asm-block bytes no .s covers")

    images = {}
    for img in sorted(set(BLOCKS) | {j[1] for j in blobs}):
        p = build / f"{img}.bin"
        if not p.exists():
            sys.exit(f"missing {p}")
        images[img] = bytearray(p.read_bytes())
    zero = [(img, lo, hi) for img, bl in BLOCKS.items() for lo, hi in bl] + [(j[1], j[2], j[3]) for j in blobs]
    for img, lo, hi in zero:
        images[img][lo - VRAM:hi - VRAM] = bytes(hi - lo)
    for f, img, lo, hi, b in blobs:
        images[img][lo - VRAM:hi - VRAM] = b
    orig = {img: common.original(img) for img in images}
    for f, img, lo, hi, b in blobs:
        d = sum(x != y for x, y in zip(b, orig[img][lo - VRAM:hi - VRAM]))
        print(f"{f.name:20} {img} {lo:#010x}-{hi:#010x} {hi - lo:5} bytes, {d} differ")
    for img, data in images.items():
        (build / f"{img}.bin").write_bytes(data)
        o = orig[img]
        d = sum(x != y for x, y in zip(data, o)) + abs(len(data) - len(o))
        print(f"{img}: {d} bytes differ from the original")


if __name__ == "__main__":
    main()
