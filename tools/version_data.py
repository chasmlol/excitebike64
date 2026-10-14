"""Make src/data build a version's initialized data: compiles the data C for the version (tools/build_data.py) and,
for each symbol whose bytes differ from the version's image, changes its definition.

1. Resource ids: a number that the version has as RES(number) (include/version.h: LodgeNet's RESOURCE numbering in
   that version) becomes RES(number) in the shared definition.
2. Anything else: the definition gets a branch for the version,
       #if VERSION == VERSION_<OTHERS>  <the definition as it was>  #elif VERSION == VERSION_<NEW>  <the new one>  #endif
   The new definition is the old one with each element that differs replaced by the version's value (a pointer
   when the old element was one, resolved through the version's symbols.txt; strings as strings, with octal escapes
   for the bytes after their end).
Prints what it would change; --write changes the files and checks the result by compiling again. Definitions
already inside an #if, symbols whose size differs and symbols missing from a version are reported, not changed.

usage: python tools/version_data.py --version NAME [--write]
"""
import argparse
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
import build_data as BD  # noqa: E402
from gen_data_c import f_text  # noqa: E402

VRAM = common.VRAM
CTYPES = {"u8": (1, False), "s8": (1, True), "char": (1, False), "u16": (2, False), "s16": (2, True),
          "u32": (4, False), "s32": (4, True), "f32": (4, False), "f64": (8, False)}
DEF_RE = r"^(u8|s8|char|u16|s16|u32|s32|f32|f64) ({name})(\[[^\]]*\])? = "


def res_function(version):
    """The RES() of include/version.h for the version, as a Python function (None: no RES there)."""
    text = (common.ROOT / "include" / "version.h").read_text()
    m = re.search(r"#(?:el)?if VERSION == VERSION_" + version.upper() + r"\s*\n#define RES\(id\) ((?:.*\\\n)*.*)",
                  text)
    if not m:
        return None
    expr = m.group(1).replace("\\\n", " ")
    return lambda i: eval(expr, {"id": i})        # the macro is also a Python expression


def find_def(text, name):
    """(start, end) of the definition of `name` (from its line to the end of its `;` line), or None."""
    m = re.search(DEF_RE.format(name=re.escape(name)), text, re.M)
    if not m:
        return None
    i = m.end()
    depth, q = 0, None
    while i < len(text):
        c = text[i]
        if q:
            if c == "\\":
                i += 2
                continue
            if c == q:
                q = None
        elif c in "\"'":
            q = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        elif c == ";" and depth == 0:
            return m.start(), text.index("\n", i) + 1
        i += 1
    return None


def split_elements(init):
    """The initializer's elements: [(index, start, end)] of the text of each value (designators followed)."""
    body = init.strip()
    if not body.startswith("{"):
        s = init.index(body)
        return [(0, s, s + len(body))]
    s0 = init.index("{") + 1
    e0 = init.rindex("}")
    out, idx, i = [], 0, s0
    while i < e0:
        j = i
        depth = 0
        while j < e0 and not (init[j] == "," and depth == 0):
            if init[j] == "(":
                depth += 1
            elif init[j] == ")":
                depth -= 1
            j += 1
        piece = init[i:j]
        stripped = re.sub(r"/\*.*?\*/", lambda m: " " * len(m.group(0)), piece)
        if stripped.strip():
            dm = re.match(r"\s*\[(0x[0-9a-fA-F]+|\d+)\]\s*=\s*", stripped)
            if dm:
                idx = int(dm.group(1), 0)
                vs = i + dm.end()
            else:
                vs = i + len(stripped) - len(stripped.lstrip())
            ve = i + len(stripped.rstrip())
            out.append((idx, vs, ve))
            idx += 1
        i = j + 1
    return out


def c_bytes_string(b: bytes) -> str:
    out = []
    for c in b:
        ch = chr(c)
        if ch in '"\\':
            out.append("\\" + ch)
        elif ch == "\n":
            out.append("\\n")
        elif 0x20 <= c < 0x7F and ch != "?":
            out.append(ch)
        else:
            out.append(f"\\{c:03o}")
    return '"' + "".join(out) + '"'


class Version:
    def __init__(self, version):
        self.version = version
        self.syms = common.symbols(version)
        self.ranges = {}
        for img, t in self.syms.items():
            rs = sorted((s.address, s.address + (s.size or 1), n) for n, s in t.items() if s.alias_of is None)
            self.ranges[img] = rs

    def pointer_text(self, image, value):
        """`(u32)name` / `(u32)name + 0x10` for an address inside a symbol of the image, else None."""
        best = None
        for a, e, n in self.ranges[image]:
            if a <= value < e or (a == value):
                best = (a, n)
        if best is None:
            return None
        a, n = best
        return f"(u32){n}" + (f" + {value - a:#x}" if value != a else "")


def element_text(ctype, raw, signed):
    if ctype == "f32":
        return f_text(raw)
    if ctype == "f64":
        return f_text(raw)
    v = int.from_bytes(raw, "big")
    if signed and v >= 1 << (8 * len(raw) - 1):
        v -= 1 << (8 * len(raw))
        return str(v)
    return "0" if v == 0 else f"{v:#X}".replace("0X", "0x")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--write", action="store_true")
    common.add_version_arg(ap)
    a = ap.parse_args()
    version = common.VERSION
    res = res_function(version)
    ver = Version(version)
    files = sorted(BD.SRC.glob("*.c"))
    built, errors = BD.build_all(files)
    for e in errors:
        print("error:", e[:500])
    if errors:
        sys.exit(1)
    texts = {f.name: f.read_text() for f in files}
    edits = {}          # file -> [(start, end, new text)]
    sizes = {}          # (image, name) -> new size for symbols.txt
    report = []
    for image in common.IMAGES:
        orig = common.original(image)
        order = sorted(built[image], key=lambda x: x[2])
        nexts = {x[1]: (order[k + 1][2] if k + 1 < len(order) else 1 << 32) for k, x in enumerate(order)}
        for fname, name, addr, data, rels in built[image]:
            # a symbol that runs into the next one in this version is shorter here (a list lost an element)
            shrink = addr + len(data) > nexts[name]
            if shrink:
                data = data[:nexts[name] - addr]
                rels = [(o, t) for o, t in rels if o < len(data)]
                sizes[image, name] = len(data)
            want = orig[addr - VRAM:addr - VRAM + len(data)]
            if data == want and not shrink:
                continue
            sym = ver.syms[image].get(name)
            if sym is not None and sym.size and sym.size != len(data) and not shrink:
                report.append(f"{name}: size {len(data):#x} in the C, {sym.size:#x} in symbols.txt: by hand")
                continue
            text = texts[fname]
            span = find_def(text, name)
            if span is None:
                report.append(f"{name}: definition not found in {fname}")
                continue
            s, e = span
            prev_line = text[:s].rstrip("\n").rsplit("\n", 1)[-1]
            branch = None
            if re.match(r"#\s*(if|elif|else)\b", prev_line):
                # already in branches: replace this version's branch when it has one
                m2 = re.search(r"^#elif VERSION == VERSION_" + version.upper() + r"\n", text[e:], re.M)
                end_if = text.find("#endif\n", e)
                if m2 is None or e + m2.start() > end_if:
                    report.append(f"{name}: inside an #if already: by hand")
                    continue
                s2, e2 = find_def(text[e + m2.end():], name)
                branch = (e + m2.end() + s2, e + m2.end() + e2)
            if shrink:
                d = text[s:e]
                m = re.match(DEF_RE.format(name=re.escape(name)), d)
                ctype = m.group(1)
                esize, signed = CTYPES[ctype]
                init = d[m.end():d.rindex(";")]
                old_vals = {}
                for i, vs, ve in split_elements(init):
                    t = init[vs:ve]
                    if re.fullmatch(r"(0x[0-9A-Fa-f]+|\d+)", t):
                        old_vals.setdefault(res(int(t, 0)) if res else int(t, 0), t)
                vals = []
                for i in range(len(want) // esize):
                    v = int.from_bytes(want[i * esize:(i + 1) * esize], "big")
                    t = old_vals.get(v)
                    if t is not None and res is not None and res(int(t, 0)) != int(t, 0):
                        vals.append(f"RES({t})")
                    else:
                        vals.append(element_text(ctype, want[i * esize:(i + 1) * esize], signed))
                n = len(vals)
                array = n > 1 or "[" in d[:m.end()]
                head = f"{ctype} {name}[{n:#x}] = " if array else f"{ctype} {name} = "
                body = ("{ " + ", ".join(vals) + " }") if array else vals[0]
                new_def = head + body + ";\n"
                if branch:
                    edits.setdefault(fname, []).append((branch[0], branch[1], new_def))
                else:
                    edits.setdefault(fname, []).append((s, e, wrap(d, new_def, version)))
                report.append(f"{name}: {len(data):#x} bytes here (it ran into the next symbol)")
                continue
            if branch:
                report.append(f"{name}: its branch for this version differs: by hand")
                continue
            d = text[s:e]
            m = re.match(DEF_RE.format(name=re.escape(name)), d)
            ctype = m.group(1)
            esize, signed = CTYPES[ctype]
            init = d[m.end():d.rindex(";")]
            relocs = {o for o, t in rels}
            if ctype == "char" and init.strip().startswith('"'):
                new_def = d[:m.end()] + c_bytes_string(want) + ";\n" if len(want) else d
                if want.rstrip(b"\0").find(b"\0") < 0 and all(0x20 <= c < 0x7F or c in (9, 10) for c in want.rstrip(b"\0")):
                    new_def = d[:m.end()] + c_bytes_string(want.rstrip(b"\0")) + ";\n"
                edits.setdefault(fname, []).append((s, e, wrap(d, new_def, version)))
                report.append(f"{name}: string")
                continue
            elems = split_elements(init)
            base = m.end()
            n = len(data) // esize
            by_index = {i: (vs, ve) for i, vs, ve in elems}
            res_subst, other = [], []
            for i in range(n):
                got, exp = data[i * esize:(i + 1) * esize], want[i * esize:(i + 1) * esize]
                if got == exp:
                    continue
                gv, ev = int.from_bytes(got, "big"), int.from_bytes(exp, "big")
                if (res is not None and ctype not in ("f32", "f64") and i in by_index and i * esize not in relocs
                        and res(gv) == ev and gv != ev
                        and re.fullmatch(r"(0x[0-9A-Fa-f]+|\d+)", init[by_index[i][0]:by_index[i][1]])):
                    res_subst.append(i)
                else:
                    other.append(i)
            if not other:
                # RES() in the shared definition
                new_init = init
                for i in sorted(res_subst, key=lambda i: -by_index[i][0]):
                    vs, ve = by_index[i]
                    new_init = new_init[:vs] + f"RES({new_init[vs:ve]})" + new_init[ve:]
                edits.setdefault(fname, []).append((s, e, d[:base] + new_init + d[base + len(init):]))
                report.append(f"{name}: {len(res_subst)} resource ids")
                continue
            # a branch for the version: the old definition with the differing elements replaced
            if any(i not in by_index for i in other) or not init.strip().startswith(("{", "0", "1", "2", "3", "4",
                                                                                     "5", "6", "7", "8", "9",
                                                                                     "-", "(")):
                # elements the initializer leaves to the zero fill: write them all out
                vals = []
                for i in range(n):
                    raw = want[i * esize:(i + 1) * esize]
                    t = None
                    if i * esize in relocs or (i in by_index and "(u32)" in init[by_index[i][0]:by_index[i][1]]):
                        t = ver.pointer_text(image, int.from_bytes(raw, "big"))
                    vals.append(t or element_text(ctype, raw, signed))
                last = max([i for i, v in enumerate(vals) if v not in ("0", "0.0f", "0.0")], default=-1)
                rows = []
                per = 4 if esize >= 4 else (8 if esize == 2 else 16)
                for k in range(0, last + 1, per):
                    rows.append("    " + (f"/* {k * esize:#05x} */ " if len(want) > 16 else "")
                                + ", ".join(vals[k:k + per]) + ",")
                if n == 1 and "[" not in d[:base]:
                    new_def = d[:base] + vals[0] + ";\n"
                elif last < 0:
                    new_def = d[:base] + "{ 0 };\n"
                else:
                    new_def = d[:base] + "{\n" + "\n".join(rows) + "\n};\n"
            else:
                new_init = init
                for i in sorted(other, key=lambda i: -by_index[i][0]):
                    vs, ve = by_index[i]
                    raw = want[i * esize:(i + 1) * esize]
                    t = None
                    if i * esize in relocs:
                        t = ver.pointer_text(image, int.from_bytes(raw, "big"))
                    new_init = new_init[:vs] + (t or element_text(ctype, raw, signed)) + new_init[ve:]
                for i in sorted(res_subst, key=lambda i: -by_index[i][0]):
                    vs, ve = by_index[i]
                    raw = want[i * esize:(i + 1) * esize]
                    new_init = new_init[:vs] + element_text(ctype, raw, signed) + new_init[ve:]
                new_def = d[:base] + new_init + d[base + len(init):]
            edits.setdefault(fname, []).append((s, e, wrap(d, new_def, version)))
            report.append(f"{name}: {len(other)} elements differ")
    for line in report:
        print(line)
    print(f"{sum(len(v) for v in edits.values())} definitions to change in {len(edits)} files")
    if not a.write:
        return
    for fname, es in edits.items():
        text = texts[fname]
        for s, e, new in sorted(es, key=lambda x: -x[0]):
            text = text[:s] + new + text[e:]
        (BD.SRC / fname).write_text(text, newline="\n")
    if sizes:       # the sizes of the shortened symbols in symbols.txt
        import rename
        path = common.table_path("symbols.txt")
        lines = []
        for line in path.read_text().splitlines():
            f = line.split("#", 1)[0].split()
            if len(f) >= 5 and (f[1], f[0]) in sizes:
                f[4] = f"{sizes[f[1], f[0]]:#x}"
                line = " ".join(f)
            lines.append(line)
        path.write_text(rename.format_symbols("\n".join(lines) + "\n"), newline="\n")
    print("written; compiling again:")
    built, errors = BD.build_all(files)
    left = []
    for image in common.IMAGES:
        orig = common.original(image)
        for fname, name, addr, data, rels in built[image]:
            if data != orig[addr - VRAM:addr - VRAM + len(data)]:
                left.append(name)
    print("errors:", errors[:5], "; symbols still differing:", left[:40], len(left))


def wrap(old, new, version):
    """old and new definitions as branches by version: the old one for every other version so far."""
    others = [v for v in common.known_versions() if v != version]
    cond = " || ".join(f"VERSION == VERSION_{v.upper()}" for v in others)
    return f"#if {cond}\n{old}#elif VERSION == VERSION_{version.upper()}\n{new}#endif\n"


if __name__ == "__main__":
    main()
