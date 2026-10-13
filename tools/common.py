"""Paths, local settings and the function table, shared by the tools. Standard library only.

Settings (toolchain locations) come from environment variables of the same name, else from config.toml at the
repository root (not tracked; see config.example.toml):
  SN64_GCC281, SN64_GCC272_0006, SN64_GCC272_0001   folders with SN64 cppn64.exe, cc1n64.exe and asn64.exe
  ULTRALIB      decompals/ultralib checkout (commit e24c836): libultra headers and sources
  KMC_GCC       KMC GCC 2.7.2 + binutils 2.6 for Linux (gcc, cc1, cpp, as, ar): libultra and libgcc objects
  SWD32         John Brandwood's swd32 executable (compresses the game's code segments)
  MIPS_PREFIX   GNU binutils for MIPS, command prefix (default mips-linux-gnu-)
  WINE          runs the Win32 tools (SN64, swd32.exe) on a non-Windows host (default wine)
  LIBULTRA_WORK Linux-filesystem folder build_libultra_objs.sh builds in (default ~/excitebike64-libultra)
"""
from __future__ import annotations

import csv
import json
import os
import subprocess
import sys
import tomllib
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
ORIG = BUILD / "extracted"          # the original images, written by tools/extract.py
BASEROM = ROOT / "baserom.z64"
VRAM = 0x80000400
IMAGES = ("game", "boot")
ROM_SHA1 = "3a043dc4c8b1351ca56c3ab9bb061bd7fa99c312"
# Where each image keeps initialized data (code can reference it by value: literals).
DATA_RANGES = {"game": (0x800C2280, 0x800DC098), "boot": (0x8000CE10, 0x800113E4)}


@lru_cache(None)
def _config() -> dict:
    p = ROOT / "config.toml"
    return tomllib.loads(p.read_text()) if p.exists() else {}


def setting(name: str, default: str | None = None, required: bool = True) -> str | None:
    v = os.environ.get(name) or _config().get(name) or default
    if v is None and required:
        sys.exit(f"{name} is not set: set the environment variable or add `{name} = \"...\"` to config.toml "
                 "(see config.example.toml)")
    return v


def original(image: str) -> bytes:
    p = ORIG / f"{image}.bin"
    if not p.exists():
        sys.exit(f"missing {p.relative_to(ROOT)}: run `python tools/extract.py baserom.z64` first")
    return p.read_bytes()


def output_dir(path) -> Path:
    """A build directory: never the folder of the original images."""
    p = Path(path)
    if p.resolve() == ORIG.resolve():
        sys.exit(f"refusing to write into {ORIG.relative_to(ROOT)} (the original images)")
    return p


@lru_cache(None)
def functions() -> tuple[dict, ...]:
    """functions.csv: every function-sized range of both images, with how it is built.
    kind: c (src/), copy (a boot copy of a game function, built from its C), asm (asm/), libultra (ultralib or
    libgcc object), padding (linker fill between objects)."""
    out = []
    with open(ROOT / "functions.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            r["address"] = int(r["address"], 16)
            r["size"] = int(r["size"])
            out.append(r)
    return tuple(out)


@lru_cache(None)
def symbols() -> dict:
    """symbols.txt: {image: {name: Symbol}}, every name the C, asm and data use (see the file's header)."""
    out = {k: {} for k in IMAGES}
    for n, line in enumerate((ROOT / "symbols.txt").read_text().splitlines(), 1):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        f = line.split()
        if len(f) not in (5, 6) or f[1] not in IMAGES or f[3] not in ("func", "data"):
            sys.exit(f"symbols.txt:{n}: expected `name image address kind size [shares-address-with]`")
        sym = Symbol(f[0], f[1], int(f[2], 16), f[3], None if f[4] == "-" else int(f[4], 0),
                     f[5] if len(f) == 6 else None)
        if sym.name in out[sym.image]:
            sys.exit(f"symbols.txt:{n}: {sym.name} is listed twice for the {sym.image} image")
        out[sym.image][sym.name] = sym
    return out


class Symbol:
    __slots__ = ("name", "image", "address", "kind", "size", "alias_of")

    def __init__(self, name, image, address, kind, size, alias_of):
        self.name, self.image, self.address, self.kind = name, image, address, kind
        self.size, self.alias_of = size, alias_of


@lru_cache(None)
def libultra_names(image: str) -> dict[str, int]:
    """libultra's own names (libultra_map.json): its functions and the symbols its objects define."""
    m = json.loads((ROOT / "libultra_map.json").read_text())["images"][image]
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


@lru_cache(None)
def profiles() -> dict:
    return tomllib.loads((ROOT / "platform.toml").read_text())["profiles"]


def profile(name: str) -> dict:
    """A compiler profile from platform.toml with the program paths resolved."""
    p = dict(profiles()[name])
    base = Path(setting(p["toolchain"]))
    for k in ("cpp", "cc1", "as"):
        p[k] = str(base / p[k])
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
