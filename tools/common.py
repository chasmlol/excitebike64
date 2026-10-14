"""Paths, local settings, the version to build and its tables, shared by the tools. Standard library only.

Settings (toolchain locations) come from environment variables of the same name, else from config.toml at the
repository root (not tracked; see config.example.toml):
  VERSION       the version to build or examine: a directory of versions/ (default lodgenet).
                Every tool also takes --version NAME, which wins over the setting.
  SN64_GCC281, SN64_GCC272_0006, SN64_GCC272_0001   folders with SN64 cppn64.exe, cc1n64.exe and asn64.exe
  ULTRALIB      decompals/ultralib checkout (commit e24c836): libultra headers and sources
  KMC_GCC       KMC GCC 2.7.2 + binutils 2.6 for Linux (gcc, cc1, cpp, as, ar): libultra and libgcc objects
  SWD32         John Brandwood's swd32 executable (compresses the game's code segments)
  MIPS_PREFIX   GNU binutils for MIPS, command prefix (default mips-linux-gnu-)
  WINE          runs the Win32 tools (SN64, swd32.exe) on a non-Windows host (default wine)
  LIBULTRA_WORK Linux-filesystem folder build_libultra_objs.sh builds in (default ~/excitebike64-libultra)

Each version has a directory versions/<name>/: version.toml (the ROM's SHA-1 and layout), functions.csv,
symbols.txt and libultra_map.json. Its extracted images and build go to build/<name>/.
"""
from __future__ import annotations

import csv
import json
import os
import re
import subprocess
import sys
import threading
import tomllib
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
VERSIONS = ROOT / "versions"
VRAM = 0x80000400
IMAGES = ("game", "boot")
DEFAULT_VERSION = "lodgenet"


@lru_cache(None)
def _config() -> dict:
    p = ROOT / "config.toml"
    return tomllib.loads(p.read_text()) if p.exists() else {}


def known_versions() -> list[str]:
    """The versions this tree builds: the directories of versions/ with a version.toml."""
    return sorted(p.parent.name for p in VERSIONS.glob("*/version.toml"))


def version_numbers() -> dict[str, int]:
    """{version: the value of its VERSION_<NAME> macro} from include/version.h."""
    text = (ROOT / "include" / "version.h").read_text()
    return {m.group(1).lower(): int(m.group(2)) for m in re.finditer(r"#define\s+VERSION_(\w+)\s+(\d+)", text)}


def _choose_version() -> str:
    """--version NAME on the command line, else the VERSION setting, else lodgenet."""
    v = None
    for i, arg in enumerate(sys.argv[1:], 1):
        if arg == "--version" and i + 1 < len(sys.argv):
            v = sys.argv[i + 1]
        elif arg.startswith("--version="):
            v = arg.split("=", 1)[1]
    v = v or os.environ.get("VERSION") or _config().get("VERSION") or DEFAULT_VERSION
    if v not in known_versions():
        sys.exit(f"unknown version {v!r}: one of {', '.join(known_versions())} (versions/<name>/version.toml)")
    if v not in version_numbers():
        sys.exit(f"include/version.h has no VERSION_{v.upper()}")
    os.environ["VERSION"] = v     # the tools a tool runs build the same version
    return v


VERSION = _choose_version()
VERSION_DIR = VERSIONS / VERSION
VERSION_NUMBER = version_numbers()[VERSION]
ORIG = BUILD / VERSION / "extracted"     # the original images, written by tools/extract.py
BASEROM = ORIG / "baserom.z64"           # the original ROM, copied there by tools/extract.py
OUT = BUILD / VERSION / "out"            # tools/build_all.py's default output


def argv() -> list[str]:
    """sys.argv without --version NAME (for the tools that read their arguments themselves)."""
    out, skip = [], False
    for arg in sys.argv:
        if skip:
            skip = False
        elif arg == "--version":
            skip = True
        elif not arg.startswith("--version="):
            out.append(arg)
    return out


def add_version_arg(ap):
    """Document --version on a tool's command line (common reads it before argparse runs)."""
    ap.add_argument("--version", default=VERSION, metavar="NAME",
                    help=f"version to build: {', '.join(known_versions())} (default: the VERSION setting, else "
                         f"{DEFAULT_VERSION}; now {VERSION})")


@lru_cache(None)
def version_info(version: str | None = None) -> dict:
    """versions/<version>/version.toml: title, sha1, cic, rom, the per-image layout, asm_files."""
    return tomllib.loads((VERSIONS / (version or VERSION) / "version.toml").read_text())


def layout(image: str) -> dict:
    """The per-image layout of the version being built (version.toml [game] / [boot]): data, bss, libultra_text,
    markers, microcode_text, microcode_data, asm, from_rom."""
    return version_info()[image]


ROM_SHA1 = version_info()["sha1"]
TITLE = version_info()["title"]
# Where each image keeps initialized data (code can reference it by value: literals).
DATA_RANGES = {k: tuple(layout(k)["data"]) for k in IMAGES}


def table_path(name: str, version: str | None = None) -> Path:
    """versions/<version>/<name>: functions.csv, symbols.txt, libultra_map.json."""
    return VERSIONS / (version or VERSION) / name


def setting(name: str, default: str | None = None, required: bool = True) -> str | None:
    v = os.environ.get(name) or _config().get(name) or default
    if v is None and required:
        sys.exit(f"{name} is not set: set the environment variable or add `{name} = \"...\"` to config.toml "
                 "(see config.example.toml)")
    return v


def original(image: str) -> bytes:
    p = ORIG / f"{image}.bin"
    if not p.exists():
        sys.exit(f"missing {p.relative_to(ROOT)}: run `python tools/extract.py ROM` with the {TITLE} ROM first")
    return p.read_bytes()


def output_dir(path) -> Path:
    """A build directory: never the folder of the original images."""
    p = Path(path)
    if p.resolve() == ORIG.resolve():
        sys.exit(f"refusing to write into {ORIG.relative_to(ROOT)} (the original images)")
    return p


@lru_cache(None)
def functions(version: str | None = None) -> tuple[dict, ...]:
    """functions.csv of the version (default: the one being built): every function-sized range of both images, with
    how it is built. kind: c (src/), copy (a boot copy of a game function, built from its C), asm (asm/), libultra
    (ultralib or libgcc object), padding (linker fill between objects)."""
    out = []
    with open(table_path("functions.csv", version), newline="") as fh:
        for r in csv.DictReader(fh):
            r["address"] = int(r["address"], 16)
            r["size"] = int(r["size"])
            out.append(r)
    return tuple(out)


@lru_cache(None)
def symbols(version: str | None = None) -> dict:
    """symbols.txt of the version (default: the one being built): {image: {name: Symbol}}, every name the C, asm and
    data use (see the file's header)."""
    out = {k: {} for k in IMAGES}
    path = table_path("symbols.txt", version)
    where = path.relative_to(ROOT).as_posix()
    for n, line in enumerate(path.read_text().splitlines(), 1):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        f = line.split()
        if len(f) not in (5, 6) or f[1] not in IMAGES or f[3] not in ("func", "data"):
            sys.exit(f"{where}:{n}: expected `name image address kind size [shares-address-with]`")
        sym = Symbol(f[0], f[1], int(f[2], 16), f[3], None if f[4] == "-" else int(f[4], 0),
                     f[5] if len(f) == 6 else None)
        if sym.name in out[sym.image]:
            sys.exit(f"{where}:{n}: {sym.name} is listed twice for the {sym.image} image")
        out[sym.image][sym.name] = sym
    return out


class Symbol:
    __slots__ = ("name", "image", "address", "kind", "size", "alias_of")

    def __init__(self, name, image, address, kind, size, alias_of):
        self.name, self.image, self.address, self.kind = name, image, address, kind
        self.size, self.alias_of = size, alias_of


@lru_cache(None)
def libultra_names(image: str, version: str | None = None) -> dict[str, int]:
    """libultra's own names (libultra_map.json): its functions and the symbols its objects define."""
    m = json.loads(table_path("libultra_map.json", version).read_text())["images"][image]
    out = {n: int(a, 16) for n, a in m["symbols"].items()}
    for e in m["objects"]:
        out.update({n: int(a, 16) for n, a in e.get("functions", {}).items()})
    return out


def resolver(image: str):
    """name -> address in one image: symbols.txt, then libultra's own names. None if the name is unknown."""
    tab = symbols()[image]
    lib = libultra_names(image)

    def resolve(name: str):
        s = tab.get(name)
        if s is not None:
            return s.address
        return lib.get(name)
    return resolve


@lru_cache(None)
def names_at(image: str) -> dict[int, str]:
    """address -> the name to show for it: a function of functions.csv, else the first symbol there that isn't a
    second name, else libultra's."""
    out = {}
    for f in functions():
        if f["image"] == image and f["kind"] != "padding":
            out.setdefault(f["address"], f["name"])
    for s in symbols()[image].values():
        if s.alias_of is None:
            out.setdefault(s.address, s.name)
    for n, a in libultra_names(image).items():
        out.setdefault(a, n)
    return out


_GEN_LOCK = threading.Lock()


def generated_include() -> Path:
    """build/<version>/include: symbol_addresses.h, `#define SYMADDR_<name> 0x...` for every symbol of the version's
    symbols.txt and libultra_map.json (include/symaddr.h's SYM_ADDR), rewritten when they are newer. The compiler
    searches it."""
    d = BUILD / VERSION / "include"
    h = d / "symbol_addresses.h"
    newest = max(table_path(n).stat().st_mtime for n in ("symbols.txt", "libultra_map.json"))
    with _GEN_LOCK:
        if h.exists() and h.stat().st_mtime >= newest:
            return d
        lines = [f"/* Generated from versions/{VERSION}/symbols.txt and libultra_map.json by tools/common.py: each "
                 "symbol's address (the game image's, for a name both images have). */"]
        done = set()
        for img in IMAGES:
            names = {n: s.address for n, s in symbols()[img].items()}
            names.update({n: a for n, a in libultra_names(img).items() if n not in names})
            for name, addr in sorted(names.items()):
                if name not in done:
                    done.add(name)
                    lines.append(f"#define SYMADDR_{name} 0x{addr:08X}")
        d.mkdir(parents=True, exist_ok=True)
        tmp = h.with_suffix(".tmp")
        tmp.write_text("\n".join(lines) + "\n", newline="\n")
        tmp.replace(h)
    return d


@lru_cache(None)
def profiles() -> dict:
    return tomllib.loads((ROOT / "platform.toml").read_text())["profiles"]


def profile(name: str) -> dict:
    """A compiler profile from platform.toml with the program paths resolved, and the cc1 flags the version being
    built adds to it (version.toml [cflags], by profile name)."""
    p = dict(profiles()[name])
    base = Path(setting(p["toolchain"]))
    for k in ("cpp", "cc1", "as"):
        p[k] = str(base / p[k])
    extra = version_info().get("cflags", {}).get(name)
    if extra:
        p["cflags"] = f"{p['cflags']} {extra}"
    p["name"] = name
    return p


def exe_command(path: str) -> list[str]:
    """How to start a Win32 tool on this host."""
    if os.name != "nt" and path.lower().endswith(".exe"):
        return [setting("WINE", "wine"), path]
    return [path]


def posix_path(p: Path) -> str:
    """p as the POSIX shell sees it (WSL's /mnt/<drive>/... on Windows)."""
    p = Path(p).resolve()
    if os.name != "nt":
        return str(p)
    return f"/mnt/{p.drive.rstrip(':').lower()}{p.as_posix()[2:]}"


def run_posix(script_path: Path, env: dict | None = None):
    """Run a shell script with bash (through WSL on Windows). -> (returncode, output)."""
    if os.name == "nt":
        exports = "".join(f"export {k}='{v}'; " for k, v in (env or {}).items())
        cmd = ["wsl", "-e", "bash", "-lc", f"{exports}bash '{posix_path(script_path)}'"]
        r = subprocess.run(cmd, capture_output=True, text=True)
    else:
        r = subprocess.run(["bash", str(script_path)], capture_output=True, text=True,
                           env={**os.environ, **(env or {})})
    return r.returncode, (r.stdout + r.stderr).strip()
