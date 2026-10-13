"""Assemble the ROM from a build's images: boot.bin (uncompressed at ROM 0x1000) and game.bin compressed again
into the container's four segment streams, then the header checksum. Everything else (IPL3, the game's resources
after the container) comes from the original ROM.

The container at ROM 0x12200: the tag "BRSOHYES", the (raw, packed) sizes of the four segments, then the SWD
streams, each padded with 0xFF to a multiple of 4. Each segment is compressed with John Brandwood's SWD32 (setting
SWD32), which reproduces the original streams; --packer own uses tools/swd_pack.py instead (a valid ROM, but the
streams and so the ROM differ from the original).

usage: python tools/build_rom.py [BUILD_DIR] [OUT.z64] [--baserom FILE] [--packer swd32|own]
       (defaults: build/out, BUILD_DIR/excitebike64.z64, baserom.z64)
"""
import argparse
import hashlib
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import common  # noqa: E402
from swd import expand            # noqa: E402
from swd_pack import pack         # noqa: E402

VRAM = common.VRAM
M = 0xFFFFFFFF
SEEDS = {"6101/6102": 0xF8CA4DDC, "6103": 0xA3886759, "6105": 0xDF26F436, "6106": 0x1FEA617A}


def crc(rom, cic):
    seed = SEEDS[cic]
    t1 = t2 = t3 = t4 = t5 = t6 = seed
    for i in range(0x1000, 0x101000, 4):
        d = struct.unpack_from(">I", rom, i)[0]
        if (t6 + d) & M < t6:
            t4 = (t4 + 1) & M
        t6 = (t6 + d) & M
        t3 ^= d
        s = d & 0x1F
        r = ((d << s) | (d >> (32 - s))) & M if s else d
        t5 = (t5 + r) & M
        t2 = t2 ^ r if t2 > d else t2 ^ t6 ^ d
        if cic == "6105":
            t1 = (t1 + (struct.unpack_from(">I", rom, 0x750 + (i & 0xFF))[0] ^ d)) & M
        else:
            t1 = (t1 + (t5 ^ d)) & M
    if cic == "6103":
        return ((t6 ^ t4) + t3) & M, ((t5 ^ t2) + t1) & M
    if cic == "6106":
        return ((t6 * t4) + t3) & M, ((t5 * t2) + t1) & M
    return t6 ^ t4 ^ t3, t5 ^ t2 ^ t1


def swd32(raw: bytes) -> bytes:
    """Compress with SWD32: it writes <name>.swd, a 12-byte header and then the stream."""
    tool = common.setting("SWD32")
    with tempfile.TemporaryDirectory() as td:
        src = Path(td) / "seg.bin"
        src.write_bytes(raw)
        subprocess.run([*common.exe_command(tool), src.name], cwd=td, capture_output=True, check=True)
        return (Path(td) / "seg.swd").read_bytes()[12:]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("build", nargs="?", default=str(common.BUILD / "out"))
    ap.add_argument("out", nargs="?")
    ap.add_argument("--baserom", default=str(common.BASEROM))
    ap.add_argument("--packer", choices=("swd32", "own"), default="swd32")
    a = ap.parse_args()
    build = Path(a.build)
    out = Path(a.out) if a.out else build / "excitebike64.z64"
    base = Path(a.baserom).read_bytes()
    if hashlib.sha1(base).hexdigest() != common.ROM_SHA1:
        sys.exit(f"{a.baserom} is not the Excitebike 64 (USA) (LodgeNet) z64 ROM")
    layout = json.loads((common.ORIG / "layout.json").read_text())
    want = struct.unpack_from(">II", base, 0x10)
    cic = next((c for c in SEEDS if crc(base, c) == want), None)
    if cic is None:
        sys.exit("could not identify the CIC (no seed reproduces the original header checksum)")
    print(f"CIC-{cic}: the checksum algorithm reproduces the original header ({want[0]:08X} {want[1]:08X})")
    rom = bytearray(base)
    boot = (build / "boot.bin").read_bytes()
    b = layout["boot"]
    rom[b["rom"]:b["rom"] + b["size"]] = bytes(b["size"])
    rom[b["rom"]:b["rom"] + len(boot)] = boot
    game = (build / "game.bin").read_bytes()
    # The container, rebuilt from the build's images; it must end where the resources start.
    con = layout["game"]["segments"][0]["rom"] - 0x28
    rom[con:layout["resources_rom"]] = b"\xFF" * (layout["resources_rom"] - con)
    rom[con:con + 8] = b"BRSOHYES"
    pos = con + 0x28
    for k, s in enumerate(layout["game"]["segments"]):
        raw = game[s["vram"] - VRAM:s["vram_end"] - VRAM]
        packed = swd32(raw) if a.packer == "swd32" else pack(raw)
        back, _ = expand(packed)
        assert back == raw, s["name"]
        packed += b"\xFF" * (-len(packed) % 4)
        struct.pack_into(">II", rom, con + 8 + 8 * k, len(raw), len(packed))
        rom[pos:pos + len(packed)] = packed
        same = packed == base[pos:pos + len(packed)]
        print(f"  {s['name']}: {len(raw):#x} -> {len(packed):#x} bytes ({a.packer})"
              + ("  identical to the original stream" if same else ""))
        pos += len(packed)
    if pos > layout["resources_rom"]:
        sys.exit(f"the container ends at {pos:#x}, past the resources at {layout['resources_rom']:#x}")
    c1, c2 = crc(rom, cic)
    struct.pack_into(">II", rom, 0x10, c1, c2)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(bytes(rom))
    # check: decode the new ROM the way the game does and compare with the build's images
    pos = con + 0x28
    sizes = struct.unpack(">8I", rom[con + 8:con + 0x28])
    img = bytearray()
    for i in range(4):
        data, _ = expand(rom, pos)
        img += data
        pos += sizes[2 * i + 1]
    ok = bytes(img) == game and bytes(rom[b["rom"]:b["rom"] + b["size"]]) == boot
    diff = sum(1 for x, y in zip(rom, base) if x != y)
    print(f"wrote {out}: {len(rom):#x} bytes, header checksum {c1:08X} {c2:08X}; decodes back to the build: {ok}")
    h = hashlib.sha1(rom).hexdigest()
    print(f"  {diff} bytes differ from the original ROM; sha1 {h}"
          + ("  == ORIGINAL (matching build)" if h == common.ROM_SHA1 else f"  (original {common.ROM_SHA1})"))
    if not ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
