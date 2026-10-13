"""Rename symbols everywhere: the C (src/, include/), the asm, the data C, symbols.txt, functions.csv, the tools
and the docs; then rebuild to show that nothing else changed.

usage: python tools/rename.py OLD NEW [OLD NEW ...] [--no-aliases] [--no-build]
       python tools/rename.py --file PAIRS.txt          (one `OLD NEW` pair per line, # comments)

OLD must be in symbols.txt. The second names of OLD (`OLD_<n>`, listed in symbols.txt as sharing OLD's address)
become `NEW_<n>` too, unless --no-aliases. Refused, with nothing changed, when a new name is not a C identifier or
is already used: by a symbol, by any identifier of the sources or the headers, by libultra (unless it is libultra's
name for that very address: naming a libultra function or variable after the library), or by another pair.
libultra's own names (libultra_map.json) are not renamed. Names are replaced as whole identifiers: in the C and
asm outside string literals (comments included), in symbols.txt and functions.csv only in their name fields,
elsewhere as words that are not part of a path. Then tools/build_all.py runs (unless --no-build) and must end with
the ROM's SHA-1 == ORIGINAL.
"""
import argparse
import csv
import io
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402

ROOT = common.ROOT
IDENT = re.compile(r"[A-Za-z_]\w*")
C_KEYWORDS = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum",
              "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "return", "short",
              "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
              "while", "asm", "__asm__", "__attribute__", "__inline__", "__volatile__"}


def word(name):
    return re.compile(r"(?<![A-Za-z0-9_])" + re.escape(name) + r"(?![A-Za-z0-9_])")


def replace_code(text, mapping, comment="c"):
    """Replace whole identifiers outside string and character literals. comment: 'c' (/* */ and //) or 'asm' (#)."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if comment == "c" and text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(_replace_words(text[i:j], mapping))
            i = j
        elif (comment == "c" and text.startswith("//", i)) or (comment == "asm" and c == "#"):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(_replace_words(text[i:j], mapping))
            i = j
        elif c == '"' or (c == "'" and comment == "c"):
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif c.isalpha() or c == "_":
            m = IDENT.match(text, i)
            w = m.group(0)
            out.append(mapping.get(w, w))
            i = m.end()
        elif c.isdigit():
            m = re.compile(r"[0-9A-Za-z_.]+").match(text, i)
            out.append(m.group(0))
            i = m.end()
        else:
            out.append(c)
            i += 1
    return "".join(out)


def _replace_words(text, mapping):
    return IDENT.sub(lambda m: mapping.get(m.group(0), m.group(0)), text)


def replace_prose(text, mapping):
    """Whole words, except as part of a path (src/game/main.c), and not in examples of renaming."""
    if "rename.py" in text and "\n" in text:
        return "\n".join(ln if "rename.py" in ln else replace_prose(ln, mapping) for ln in text.split("\n"))

    def sub(m):
        w = m.group(0)
        if w not in mapping:
            return w
        before = text[m.start() - 1] if m.start() else ""
        after = text[m.end():m.end() + 2]
        if before == "/" or re.match(r"\.[a-z]", after):
            return w
        return mapping[w]
    return IDENT.sub(sub, text)


def format_symbols(text, mapping=None):
    """symbols.txt with names mapped and the columns aligned; comment lines stay as they are."""
    mapping = mapping or {}
    rows = []
    for line in text.splitlines():
        f = line.split("#", 1)[0].split()
        if not f:
            rows.append(line)
            continue
        f[0] = mapping.get(f[0], f[0])
        if len(f) > 5:
            f[5] = mapping.get(f[5], f[5])
        rows.append(f)
    w = max(len(r[0]) for r in rows if isinstance(r, list))
    out = []
    for r in rows:
        if isinstance(r, str):
            out.append(r)
        else:
            out.append(f"{r[0]:<{w}}  {r[1]:4}  {r[2]}  {r[3]:4}  {r[4]:>7}" + (f"  {r[5]}" if len(r) > 5 else ""))
    return "\n".join(out) + "\n"


def code_identifiers(p):
    """The identifiers of a source file's code (not its comments or string literals)."""
    text = p.read_text(errors="replace")
    if p.suffix == ".s":
        text = re.sub(r"#.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))
    else:
        text = re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.S)
        text = re.sub(r'"(\\.|[^"\\\n])*"', '""', text)
    return set(IDENT.findall(text))


def source_files():
    out = []
    for d, pats in (("src", ("*.c",)), ("include", ("*.h",)), ("asm", ("*.s",))):
        for pat in pats:
            out += sorted((ROOT / d).rglob(pat))
    return out


def ultra_identifiers():
    """Identifiers of libultra's headers (the C includes them), if ULTRALIB is set."""
    base = common.setting("ULTRALIB", required=False)
    if not base:
        return None
    out = set()
    for p in (Path(base) / "include").rglob("*.h"):
        out.update(IDENT.findall(p.read_text(errors="replace")))
    return out


def plan(pairs, aliases):
    """-> {old: new} for every name, aliases included; exits on a collision."""
    tabs = common.symbols()
    known = {n for t in tabs.values() for n in t}
    mapping = {}
    for old, new in pairs:
        if old not in known:
            sys.exit(f"{old} is not in symbols.txt")
        if any(common.libultra_names(img).get(old) == t[old].address for img, t in tabs.items() if old in t):
            sys.exit(f"{old} is libultra's own name (libultra_map.json): it stays")
        mapping[old] = new
        if aliases:
            for t in tabs.values():
                for s in t.values():
                    m = re.fullmatch(re.escape(old) + r"_(\d+)", s.name)
                    if m and s.alias_of == old:
                        mapping[s.name] = f"{new}_{m.group(1)}"
    olds, news = set(mapping), list(mapping.values())
    if len(set(news)) != len(news):
        sys.exit("two symbols would get the same name")
    used = set()
    for p in source_files():
        used.update(code_identifiers(p))
    ultra = ultra_identifiers()
    if ultra is None:
        print("warning: ULTRALIB is not set, so new names are not checked against libultra's headers")
        ultra = set()
    problems = []
    for old, new in mapping.items():
        if not re.fullmatch(r"[A-Za-z_]\w*", new) or new in C_KEYWORDS:
            problems.append(f"{new}: not a C identifier")
            continue
        if new in olds:
            problems.append(f"{new}: is renamed itself")
            continue
        # libultra's name for the same address is allowed (that is what the library calls it)
        lib_ok = all(common.libultra_names(img).get(new) in (None, tabs[img][old].address)
                     for img in common.IMAGES if old in tabs[img]) and \
            any(common.libultra_names(img).get(new) == tabs[img][old].address
                for img in common.IMAGES if old in tabs[img])
        if new in known:
            problems.append(f"{new}: already a symbol")
        elif new in used and not lib_ok:
            problems.append(f"{new}: already an identifier of the sources")
        elif any(new in common.libultra_names(img) for img in common.IMAGES) and not lib_ok:
            problems.append(f"{new}: a libultra name for another address")
        elif new in ultra and not lib_ok:
            problems.append(f"{new}: an identifier of libultra's headers")
    if problems:
        sys.exit("refused, nothing changed:\n  " + "\n  ".join(problems))
    return mapping


def rewrite(mapping):
    changed = {}

    def put(p, new_text):
        old_text = p.read_text()
        if new_text != old_text:
            p.write_text(new_text, newline="\n")
            changed[p.relative_to(ROOT).as_posix()] = True
    for p in source_files():
        put(p, replace_code(p.read_text(), mapping, "asm" if p.suffix == ".s" else "c"))
    # symbols.txt: the name and shares-address-with fields (columns realigned)
    put(ROOT / "symbols.txt", format_symbols((ROOT / "symbols.txt").read_text(), mapping))
    # functions.csv: name and copy_of
    with open(ROOT / "functions.csv", newline="") as fh:
        rows = list(csv.DictReader(fh))
    for r in rows:
        for k in ("name", "copy_of"):
            if r.get(k) in mapping:
                r[k] = mapping[r[k]]
    buf = io.StringIO()
    w = csv.DictWriter(buf, fieldnames=list(rows[0].keys()), lineterminator="\n")
    w.writeheader()
    w.writerows(rows)
    put(ROOT / "functions.csv", buf.getvalue())
    # the tools (names they hold, examples in their comments) and the docs
    for p in sorted((ROOT / "tools").glob("*.py")):
        put(p, _replace_words(p.read_text(), mapping))
    for p in [ROOT / "README.md"] + sorted((ROOT / "docs").glob("*.md")):
        put(p, replace_prose(p.read_text(), mapping))
    return sorted(changed)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("names", nargs="*", metavar="OLD NEW")
    ap.add_argument("--file", help="a file of `OLD NEW` lines")
    ap.add_argument("--no-aliases", action="store_true", help="leave the OLD_<n> second names as they are")
    ap.add_argument("--no-build", action="store_true", help="don't run tools/build_all.py afterwards")
    a = ap.parse_args()
    words = list(a.names)
    if a.file:
        for line in Path(a.file).read_text().splitlines():
            words += line.split("#", 1)[0].split()
    if not words or len(words) % 2:
        sys.exit("give OLD NEW pairs")
    pairs = list(zip(words[0::2], words[1::2]))
    mapping = plan(pairs, not a.no_aliases)
    files = rewrite(mapping)
    print(f"renamed {len(mapping)} symbols in {len(files)} files: "
          + ", ".join(f"{o} -> {n}" for o, n in list(mapping.items())[:8]) + (" ..." if len(mapping) > 8 else ""))
    if a.no_build:
        return
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "build_all.py")], cwd=ROOT, capture_output=True,
                       text=True)
    lines = (r.stdout + r.stderr).splitlines()
    sha = [ln.strip() for ln in lines if "sha1" in ln]
    print(sha[-1] if sha else "\n".join(lines[-15:]))
    if r.returncode or not any("== ORIGINAL" in ln for ln in sha):
        sys.exit("the build no longer matches: see tools/build_all.py's output")


if __name__ == "__main__":
    main()
