"""Extract the code images of Excitebike 64 (USA) (LodgeNet) from the ROM.

build/extracted/boot.bin     ROM 0x1000-0x12200, VRAM 0x80000400 (boot loader + its libultra + SwdExpand)
build/extracted/game.bin     the four SWD streams of the container at ROM 0x12200, decompressed and concatenated,
                             VRAM 0x80000400
build/extracted/layout.json  segment table (VRAM ranges, ROM offsets, sizes)

usage: python tools/extract.py [baserom.z64]      (default: baserom.z64 at the repository root)
"""
import hashlib
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import common  # noqa: E402
from swd import expand  # noqa: E402

VRAM = common.VRAM
BOOT_ROM = (0x1000, 0x12200)
CONTAINER = 0x12200


def main(rom_path=None):
    rom = Path(rom_path or common.BASEROM).read_bytes()
    if hashlib.sha1(rom).hexdigest() != common.ROM_SHA1:
        sys.exit("not the Excitebike 64 (USA) (LodgeNet) z64 ROM (sha1 mismatch; byte-swapped .v64/.n64 dumps "
                 "need converting to .z64 first)")
    out = common.ORIG
    out.mkdir(parents=True, exist_ok=True)
    boot = rom[BOOT_ROM[0]:BOOT_ROM[1]]
    (out / "boot.bin").write_bytes(boot)
    if rom[CONTAINER:CONTAINER + 8] != b"BRSOHYES":
        sys.exit("container tag not found")
    sizes = struct.unpack(">8I", rom[CONTAINER + 8:CONTAINER + 0x28])
    pos, vram, game, segs = CONTAINER + 0x28, VRAM, bytearray(), []
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
        "boot": {"vram": VRAM, "vram_end": VRAM + len(boot), "rom": BOOT_ROM[0], "size": len(boot),
                 "bss": [0x80011600, 0x8001B610]},
        "game": {"vram": VRAM, "vram_end": VRAM + len(game), "size": len(game), "segments": segs,
                 "bss": [0x800DC0A0, 0x80105580]},
        "resources_rom": pos,
    }
    (out / "layout.json").write_text(json.dumps(layout, indent=1))
    print(f"boot.bin {len(boot):#x}  game.bin {len(game):#x} ({VRAM:#x}-{VRAM + len(game):#x}) in {out}")
    for s in segs:
        print(f"  {s['name']} {s['vram']:#x}-{s['vram_end']:#x} from ROM {s['rom']:#x} ({s['packed']:#x} packed)")


if __name__ == "__main__":
    main(*sys.argv[1:2])
