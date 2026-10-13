"""The SN Systems SN64 toolchain: compile a C file to an SN LNK object, resolve names to addresses, list code.

compile(): cppn64 -> cc1n64 -> asn64 with a platform.toml profile, producing a Psy-Q LNK object (tools/snobj.py).
resolve(): names that encode an address (func_XXXXXXXX, D_XXXXXXXX, any name ending in _XXXXXXXX) and the names in
symbol_addrs.txt.

Listings (need rabbitizer: pip install rabbitizer), one instruction per line, the same notation for the original
and for an object: relocations resolved to the original's addresses (`lui $v0, 0x800C` / `addiu $v0, $v0, 0x2384`),
a `; D_XXXXXXXX` comment on each low half naming the address its %hi/%lo pair forms, branches and in-function jumps
as `L+0x1c` (offset from the function start), calls as `jal func_XXXXXXXX`. Literals in an object's own .rdata
show as `%lo(rdata+0x10:3dcccccd)` (their bytes), jump tables as `jtbl=L+0x1c,...`.

usage: python tools/sn64.py func_XXXXXXXX        listing of a function of the original images
       python tools/sn64.py file.c [profile]     compile (default profile sn281) and list the object's functions
"""
from __future__ import annotations

import re
import struct
import subprocess
import sys
import tempfile
from functools import lru_cache
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
from snobj import LnkFile  # noqa: E402

ROOT = common.ROOT
VRAM = common.VRAM
DATA_RANGES = common.DATA_RANGES
LO_OPS = {"addiu", "lb", "lbu", "lh", "lhu", "lw", "lwu", "ld", "sb", "sh", "sw", "sd", "lwc1", "swc1", "ldc1",
          "sdc1", "lwl", "lwr", "swl", "swr", "ll", "sc"}
CPP_DEFINES = ["-undef", "-D__GNUC__=2", "-Dmips", "-D__mips__", "-D__mips", "-Dn64", "-D__n64__", "-D__n64",
               "-D_PSYQ", "-D__EXTENSIONS__", "-D_MIPSEB", "-D_LANGUAGE_C", "-DF3DEX_GBI_2", "-D_FINALROM", "-DNDEBUG",
               "-D_MIPS_SZLONG=32", "-D_MIPS_SZINT=32", "-D_MIPS_SIM=1", "-D_ULTRA64"]


# --- names -------------------------------------------------------------------------------------------------

@lru_cache(None)
def symbol_map() -> dict[str, int]:
    """Names a C file may use for an address, beyond the _XXXXXXXX convention (symbol_addrs.txt: name = 0x...;)."""
    out = {}
    f = ROOT / "symbol_addrs.txt"
    if f.exists():
        for line in f.read_text().splitlines():
            m = re.match(r"\s*([A-Za-z_]\w*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line)
            if m:
                out[m.group(1)] = int(m.group(2), 16)
    return out


def resolve(name: str) -> int | None:
    m = re.search(r"_([0-9A-Fa-f]{8})$", name)
    if m:
        return int(m.group(1), 16)
    return symbol_map().get(name)


# --- compiling ---------------------------------------------------------------------------------------------

def _run(args, cwd):
    p = subprocess.run(args, cwd=cwd, capture_output=True, timeout=120)
    return p.returncode, (p.stdout + p.stderr).decode("latin-1", "replace")


def compile(profile: dict, src: Path, obj: Path, include_dirs: list[Path]):
    """-> (ok, messages). profile: common.profile(name)."""
    src, obj = Path(src).resolve(), Path(obj).resolve()
    work = obj.parent
    stem = obj.stem
    pre, asm = work / f"{stem}.i", work / f"{stem}.s"
    cpp = [*common.exe_command(profile["cpp"]), "-lang-c", *CPP_DEFINES, *profile.get("cppflags", "").split()]
    # The source's own folder is searched for "..." includes anyway; passing it again as -I makes the 2.7.2
    # builds' cppn64 fail with 'ultra64.h: Invalid argument'.
    for d in include_dirs:
        if Path(d).resolve() != src.parent:
            cpp.append(f"-I{Path(d).resolve()}")
    ultralib = Path(common.setting("ULTRALIB"))
    cpp += [f"-I{ultralib / 'include'}", f"-I{ultralib / 'include' / 'compiler' / 'gcc'}"]
    cpp += [str(src), str(pre)]
    rc, msg = _run(cpp, work)
    if rc or not pre.exists():
        return False, "preprocessor: " + msg
    rc, msg2 = _run([*common.exe_command(profile["cc1"]), "-quiet", *profile["cflags"].split(), str(pre),
                     "-o", str(asm)], work)
    if rc or not asm.exists():
        return False, (msg2 or msg).replace(str(pre), src.name)
    # asn64 mis-parses bare file names: give it .\ paths, run in the object's folder.
    rc, msg3 = _run([*common.exe_command(profile["as"]), *profile.get("asflags", "-q -G0").split(),
                     f".\\{asm.name}", "-o", f".\\{obj.name}"], work)
    if rc or not obj.exists():
        return False, "assembler: " + msg3
    return True, (msg + msg2 + msg3).strip()


# --- reading objects ---------------------------------------------------------------------------------------

def expr_ref(lnk: LnkFile, e):
    """A relocation expression as (kind, value, addend, name): ('sym', name, addend, name) for an import,
    ('sec', section index, offset, name) for this object's own sections, ('abs', value, 0, None)."""
    if e[0] == "sym":
        s = lnk.symbols[e[1]]
        if s.kind in ("export", "local"):
            return ("sec", s.section, s.offset, s.name)
        return ("sym", s.name, 0, s.name)
    if e[0] in ("secbase", "secstart"):
        return ("sec", e[1], 0, None)
    if e[0] == "add":
        a, b = e[1], e[2]
        if a[0] == "value":
            a, b = b, a
        if b[0] != "value":
            raise ValueError(f"unsupported relocation expression {e}")
        r = expr_ref(lnk, a)
        return (r[0], r[1], r[2] + b[1], r[3])
    if e[0] == "sub" and e[2][0] == "value":     # `&D_80100000[-1]`, `D - 1`: symbol minus a constant
        r = expr_ref(lnk, e[1])
        return (r[0], r[1], r[2] - e[2][1], r[3])
    if e[0] == "value":
        return ("abs", e[1], 0, None)
    raise ValueError(f"unsupported relocation expression {e}")


def patch(w: int, t: str, addr: int) -> int:
    """An instruction word with its relocated field set to addr."""
    if t.startswith("REL26"):
        return (w & 0xFC000000) | ((addr >> 2) & 0x3FFFFFF)
    if t.startswith("HI16"):
        return (w & 0xFFFF0000) | (((addr + 0x8000) >> 16) & 0xFFFF)
    if t.startswith("LO16"):
        return (w & 0xFFFF0000) | (addr & 0xFFFF)
    return w


# --- listings (rabbitizer) ---------------------------------------------------------------------------------

@lru_cache(None)
def _rabbitizer():
    try:
        import rabbitizer
    except ImportError:
        sys.exit("listings need rabbitizer: python -m pip install rabbitizer")
    rabbitizer.config.regNames_gprAbiNames = rabbitizer.Abi.O32
    rabbitizer.config.regNames_fprAbiNames = rabbitizer.Abi.O32
    rabbitizer.config.misc_opcodeLJust = 0
    return rabbitizer


def word(img: str, a: int) -> int:
    return struct.unpack_from(">I", _image(img), a - VRAM)[0]


@lru_cache(None)
def _image(name: str) -> bytes:
    return common.original(name)


def render(w: int, off: int, size: int, call: str | None = None, jump: int | None = None,
           sym: str | None = None) -> str:
    """One instruction as text. off/size: offset in the function and the function's size.
    call: rendered callee for jal; jump: function-relative target of j; sym: unresolved reference text."""
    i = _rabbitizer().Instruction(w, VRAM + off)
    op = i.getOpcodeName()
    if not i.isValid():
        return f"<unknown {w:08x}>"
    if op == "jal":
        return f"jal {call}"
    if op == "j":
        if jump is not None and 0 <= jump < size:
            return f"j L+{jump:#x}"
        return f"j {call or hex(i.getInstrIndexAsVram())}"
    if i.isBranch():
        t = off + 4 + (i.getBranchOffsetGeneric() - 4)
        text = i.disassemble()
        head = text.rsplit(",", 1)[0] if "," in text else text.split(" ")[0]
        if "," not in text:          # b offset
            return f"{op} L+{t:#x}" if t >= 0 else f"{op} L-{-t:#x}"
        return f"{head}, L+{t:#x}" if t >= 0 else f"{head}, L-{-t:#x}"
    text = re.sub(r"\s+", " ", i.disassemble(), count=1).strip()
    if sym is not None:
        text = re.sub(r"-?0x[0-9A-Fa-f]+(?=\(|$)", sym, text, count=1)
    return text


def pair_comments(texts: list[str]) -> list[str]:
    """Append `; D_XXXXXXXX` to low halves of lui pairs (linear scan, per register). Existing comments stay."""
    hi: dict[str, int] = {}
    out = []
    for t in texts:
        body, sep, comment = t.partition(" ; ")
        m = re.match(r"(\w+(?:\.\w+)?) (.*)$", body)
        if not m:
            out.append(t)
            continue
        op, args = m.groups()
        regs = [a.strip() for a in args.split(",")]
        add = None
        if op == "lui" and len(regs) == 2 and regs[1].startswith("0x"):
            hi[regs[0]] = int(regs[1], 16) << 16
            out.append(t)
            continue
        if op in LO_OPS:
            if op == "addiu" and len(regs) == 3 and regs[1] in hi and re.fullmatch(r"-?0x[0-9A-Fa-f]+", regs[2]):
                add = (hi[regs[1]] + int(regs[2], 16)) & 0xFFFFFFFF
            elif len(regs) == 2:
                mm = re.fullmatch(r"(-?0x[0-9A-Fa-f]+)\((\$\w+)\)", regs[1])
                if mm and mm.group(2) in hi:
                    add = (hi[mm.group(2)] + int(mm.group(1), 16)) & 0xFFFFFFFF
        if add is not None and not sep:
            t = f"{body} ; D_{add:08X}"
        out.append(t)
        # A register written by anything else loses its lui value.
        if regs and regs[0].startswith("$") and op not in ("sb", "sh", "sw", "sd", "swc1", "sdc1", "swl", "swr",
                                                          "sc") and not op.startswith("b") and op not in ("jr",):
            hi.pop(regs[0], None)
    return out


def jtbl_entries(img: str, a: int, start: int, end: int) -> list[int]:
    lo, hi = DATA_RANGES[img]
    out = []
    while lo <= a < hi:
        w = word(img, a)
        if not (start <= w < end):
            break
        out.append(w - start)
        a += 4
    return out


def target_lines(img: str, start: int, size: int) -> list[tuple[int, str]]:
    """Listing of the original's bytes at [start, start + size) of an image."""
    rz = _rabbitizer()
    texts = []
    for off in range(0, size, 4):
        w = word(img, start + off)
        i = rz.Instruction(w, start + off)
        op = i.getOpcodeName()
        call = jump = None
        if op == "jal":
            call = f"func_{i.getInstrIndexAsVram():08X}"
        elif op == "j":
            t = i.getInstrIndexAsVram()
            jump = t - start
            call = f"func_{t:08X}"
        texts.append(render(w, off, size, call, jump))
    texts = pair_comments(texts)
    out = []
    for k, t in enumerate(texts):    # jump tables: a low half whose address holds offsets into this function
        m = re.search(r" ; D_([0-9A-F]{8})$", t)
        if m:
            ents = jtbl_entries(img, int(m.group(1), 16), start, start + size)
            if len(ents) >= 2:
                t = t[:m.start()] + " ; jtbl " + ",".join(f"L+{e:#x}" for e in ents)
        out.append((k * 4, t))
    return out


def _literal_key(lnk: LnkFile, sec: int, off: int, op: str, fstart: int, part: str) -> str:
    """A reference to the object's own data: `rdata+0x10`, plus for the low half the bytes it reads
    (`rdata+0x10:3dcccccd`) or, for a jump table, its case offsets (`rdata+0x10:jtbl=L+0x1c,L+0x30`)."""
    s = lnk.sections[sec]
    label = f"{s.name.strip('.')}+{off:#x}"
    if part == "hi":
        return label
    relocs = {o & ~3: r for o, t, r in s.relocs if t.startswith("REL32")}
    if off in relocs:   # a jump table: words relocated against .text
        ents, a = [], off
        while a in relocs:
            ents.append(expr_ref(lnk, relocs[a])[2] - fstart)
            a += 4
        return f"{label}:jtbl=" + ",".join(f"L+{e:#x}" for e in ents)
    n = {"lwc1": 4, "swc1": 4, "lw": 4, "sw": 4, "ldc1": 8, "sdc1": 8, "ld": 8, "lh": 2, "lhu": 2, "lb": 1,
         "lbu": 1}.get(op, 0)
    data = bytes(s.data[off:off + 64])
    if n == 0:   # address taken: a string or table; up to the NUL or 32 bytes
        z = data.find(bytes(1))
        n = z + 1 if 0 <= z < 32 else min(32, len(data))
    return f"{label}:{data[:n].hex()}"


def disassemble(obj: Path) -> dict[str, list[tuple[int, str]]]:
    """Listing of every function of an SN object, relocations resolved where the name gives an address."""
    rz = _rabbitizer()
    lnk = LnkFile(Path(obj).read_bytes())
    tidx, text = lnk.section_by_name(".text")
    if text is None:
        return {}
    relocs = {o & ~3: (t, e) for o, t, e in text.relocs}
    funcs = lnk.functions(".text")
    by_off = {s0: n for n, s0, _ in funcs}
    out = {}
    for name, start, end in funcs:
        size = end - start
        # drop alignment padding after the last instruction
        while size > 8 and struct.unpack_from(">I", text.data, start + size - 4)[0] == 0 and \
                not rz.Instruction(struct.unpack_from(">I", text.data, start + size - 8)[0]).hasDelaySlot():
            size -= 4
        texts = []
        for off in range(0, size, 4):
            w = struct.unpack_from(">I", text.data, start + off)[0]
            op = rz.Instruction(w).getOpcodeName()
            call = jump = sym = None
            r = relocs.get(start + off)
            if r:
                t, e = r
                kind, a, add, _ = expr_ref(lnk, e)
                part = "hi" if "HI" in t else "lo"
                addr = label = None
                if kind == "sec" and a == tidx:
                    if t.startswith("REL26") and op == "j" and start <= add < start + size:
                        jump = add - start
                    else:
                        local = by_off.get(add)
                        addr = resolve(local) if local else None
                        label = local or f".text+{add:#x}"
                elif kind == "sec":
                    label = _literal_key(lnk, a, add, op, start, part)
                elif kind == "sym":
                    base = resolve(a)
                    if base is None:
                        label = f"{a}+{add:#x}" if add else a
                    else:
                        addr = (base + add) & 0xFFFFFFFF
                else:
                    addr = (a + add) & 0xFFFFFFFF
                if addr is not None:
                    w = patch(w, t, addr)
                    if t.startswith("REL26"):
                        call = f"func_{addr:08X}"
                elif jump is None:
                    if t.startswith("REL26"):
                        call = label
                    else:
                        sym = f"%{part}({label})"
            if op == "jal" and call is None:
                call = f"func_{rz.Instruction(w, VRAM).getInstrIndexAsVram():08X}"
            texts.append(render(w, off, size, call, jump, sym))
        out[name] = [(k * 4, t) for k, t in enumerate(pair_comments(texts))]
    return out


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    arg = sys.argv[1]
    if arg.endswith(".c"):
        prof = common.profile(sys.argv[2] if len(sys.argv) > 2 else "sn281")
        with tempfile.TemporaryDirectory() as td:
            obj = Path(td) / "m.o"
            ok, msg = compile(prof, Path(arg), obj, [ROOT / "include"])
            if not ok:
                sys.exit(msg)
            listings = disassemble(obj)
        for name, lines in listings.items():
            print(f"{name}:")
            for off, t in lines:
                print(f"  {off:5x}  {t}")
        return
    f = next((f for f in common.functions() if f["name"] == arg), None)
    if f is None:
        sys.exit(f"{arg} is not in functions.csv")
    print(f"{f['name']} ({f['image']} {f['address']:#010x}, {f['size']} bytes, {f['kind']}):")
    for off, t in target_lines(f["image"], f["address"], f["size"]):
        print(f"  {off:5x}  {t}")


if __name__ == "__main__":
    main()
