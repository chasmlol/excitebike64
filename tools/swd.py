"""SWD decompressor (John Brandwood's LZSS variant used by Excitebike 64).

Port of SwdExpand from SWDN64.ASM (https://github.com/jbrandwood/SWD32, Boost license).
Bits are read LSB-first from flag bytes; multi-bit fields accumulate MSB-first.
"""
import sys

OFFSETS = ((5, 0x0001), (7, 0x0021), (9, 0x00A1), (10, 0x02A1))


def expand(src: bytes, pos: int = 0, limit: int | None = None):
    """Decode one SWD stream starting at src[pos]. Returns (data, end_pos)."""
    out = bytearray()
    bits = 1  # empty buffer: only the end marker left

    def getbit():
        nonlocal bits, pos
        if bits == 1:
            bits = src[pos] | 0x100
            pos += 1
        b = bits & 1
        bits >>= 1
        return b

    def readbits(n, v=0):
        for _ in range(n):
            v = (v << 1) | getbit()
        return v

    while True:
        if limit is not None and len(out) >= limit:
            break
        if not getbit():
            out.append(src[pos]); pos += 1
            continue
        if not getbit():
            length = 2
        else:
            v = readbits(2)
            if v:
                length = v + 2
            else:
                v = readbits(4)
                if v:
                    length = v + 5
                else:
                    v = src[pos]; pos += 1
                    if not v:
                        break  # end of stream
                    length = v + 20
        nbits, base = OFFSETS[readbits(2)]
        v = 0
        if nbits >= 8:
            v = src[pos]; pos += 1
            nbits -= 8
        off = base + readbits(nbits, v)
        start = len(out) - off
        if start < 0:
            raise ValueError(f"bad offset {off} at out {len(out)} in {pos:#x}")
        for i in range(length):
            out.append(out[start + i])
    return bytes(out), pos


if __name__ == "__main__":
    data = open(sys.argv[1], "rb").read()
    p = int(sys.argv[2], 0)
    out, end = expand(data, p)
    print(f"in {p:#x}..{end:#x} ({end - p:#x} bytes) -> {len(out):#x} bytes")
    if len(sys.argv) > 3:
        open(sys.argv[3], "wb").write(out)
