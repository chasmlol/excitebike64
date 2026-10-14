"""Extract the code images of an Excitebike 64 ROM. The ROM's SHA-1 says which version it is (versions/*/version.toml).

build/<version>/extracted/boot.bin     the boot loader, uncompressed at ROM 0x1000 (version.toml [rom] boot), VRAM
                                       0x80000400 (boot loader + its libultra + SwdExpand)
build/<version>/extracted/game.bin     the four SWD streams of the container (version.toml [rom] container),
                                       decompressed and concatenated, VRAM 0x80000400
build/<version>/extracted/layout.json  segment table (VRAM ranges, ROM offsets, sizes)
build/<version>/extracted/baserom.z64  a copy of the ROM (tools/build_rom.py takes the IPL3 and resources from it)

usage: python tools/extract.py [ROM ...]      (default: every baserom*.z64 at the repository root)
"""
import hashlib
import json
import shutil
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402
from swd import expand  # noqa: E402

VRAM = common.VRAM


def identify(rom: bytes) -> str | None:
    h = hashlib.sha1(rom).hexdigest()
    return next((v for v in common.known_versions() if common.version_info(v)["sha1"] == h), None)


def extract(rom_path: Path):
    rom = rom_path.read_bytes()
    version = identify(rom)
    if version is None:
        sys.exit(f"{rom_path}: not a known Excitebike 64 z64 ROM (no SHA-1 of versions/*/version.toml matches; "
                 "byte-swapped .v64/.n64 dumps need converting to .z64 first)")
    info = common.version_info(version)
    out = common.BUILD / version / "extracted"
    out.mkdir(parents=True, exist_ok=True)
    boot_rom = info["rom"]["boot"]
    con = info["rom"]["container"]
    boot = rom[boot_rom[0]:boot_rom[1]]
    (out / "boot.bin").write_bytes(boot)
    if rom[con:con + 8] != b"BRSOHYES":
        sys.exit(f"container tag not found at {con:#x}")
    sizes = struct.unpack(">8I", rom[con + 8:con + 0x28])
    pos, vram, game, segs = con + 0x28, VRAM, bytearray(), []
    for i in range(4):
        raw, packed = sizes[2 * i], sizes[2 * i + 1]
        data, end = expand(rom, pos)
        assert len(data) == raw and end - pos <= packed, (i, hex(len(data)), hex(end - pos))
        segs.append({"name": f"s{i}", "vram": vram, "vram_end": vram + raw, "rom": pos, "packed": packed,
                     "size": raw})
        game += data
        pos += packed
        vram += raw
    (out / "game.bin").write_bytes(bytes(game))
    layout = {
        "version": version,
        "boot": {"vram": VRAM, "vram_end": VRAM + len(boot), "rom": boot_rom[0], "size": len(boot),
                 "bss": info.get("boot", {}).get("bss")},
        "game": {"vram": VRAM, "vram_end": VRAM + len(game), "size": len(game), "segments": segs,
                 "bss": info.get("game", {}).get("bss")},
        "resources_rom": pos,
    }
    (out / "layout.json").write_text(json.dumps(layout, indent=1))
    if rom_path.resolve() != (out / "baserom.z64").resolve():
        shutil.copyfile(rom_path, out / "baserom.z64")
    print(f"{info['title']} ({version}): boot.bin {len(boot):#x}  game.bin {len(game):#x} "
          f"({VRAM:#x}-{VRAM + len(game):#x}) in {out.relative_to(common.ROOT).as_posix()}")
    for s in segs:
        print(f"  {s['name']} {s['vram']:#x}-{s['vram_end']:#x} from ROM {s['rom']:#x} ({s['packed']:#x} packed)")


def main(paths):
    if not paths:
        paths = sorted(common.ROOT.glob("baserom*.z64"))
        if not paths:
            sys.exit(__doc__)
    for p in paths:
        extract(Path(p))


if __name__ == "__main__":
    main(common.argv()[1:])
