"""SWD compressor: the inverse of swd.expand (John Brandwood's LZSS variant), with an optimal parse.

Token costs in bits: literal 9; match = 1 + length code + 2 + offset bits, where length 2 costs 1, 3-5 cost 3,
6-20 cost 7, 21-275 cost 15, and offsets 1-0x20 / -0xA0 / -0x2A0 / -0x6A0 cost 5 / 7 / 9 / 10 bits.
Flag bytes are placed exactly where the decoder reads them (lazily, when its bit buffer runs empty).
"""
import sys

OFFSETS = ((5, 0x0001), (7, 0x0021), (9, 0x00A1), (10, 0x02A1))
WINDOW = 0x2A1 + (1 << 10) - 1
MAXLEN = 275


def off_class(o):
    for k, (n, base) in enumerate(OFFSETS):
        if o < base + (1 << n):
            return k, n, base
    raise ValueError(o)


def len_bits(l):
    return 1 if l == 2 else 3 if l <= 5 else 7 if l <= 20 else 15


class BitWriter:
    def __init__(self):
        self.out = bytearray()
        self.flag = None
        self.used = 8

    def bit(self, b):
        if self.used == 8:
            self.flag = len(self.out)
            self.out.append(0)
            self.used = 0
        if b:
            self.out[self.flag] |= 1 << self.used
        self.used += 1

    def bits(self, v, n):
        for i in range(n - 1, -1, -1):
            self.bit((v >> i) & 1)

    def byte(self, v):
        self.out.append(v & 0xFF)


def parse(data):
    """Optimal parse: [(kind, a, b)] with ('lit', byte) or ('match', length, offset)."""
    n = len(data)
    INF = 1 << 60
    cost = [INF] * (n + 1)
    cost[n] = 0
    choice = [None] * (n + 1)
    # candidate positions by 2-byte key, built forward; the parse runs backward over precomputed matches
    heads = {}
    cands = [None] * n
    from bisect import bisect_left
    for i in range(n - 1):
        heads.setdefault(data[i] | (data[i + 1] << 8), []).append(i)
    for i in range(n - 1, -1, -1):
        best = 9 + cost[i + 1]
        ch = ("lit", data[i])
        maxl = min(MAXLEN, n - i)
        got = 1
        lst = heads.get(data[i] | (data[i + 1] << 8)) if maxl >= 2 else None
        j = bisect_left(lst, i) if lst else 0
        tries = 0
        while lst and j > 0 and tries < 96:
            j -= 1
            tries += 1
            p = lst[j]
            o = i - p
            if o > WINDOW:
                break
            l = 2
            while l + 8 <= maxl and data[p + l:p + l + 8] == data[i + l:i + l + 8]:
                l += 8
            while l < maxl and data[p + l] == data[i + l]:
                l += 1
            if l <= got:
                continue
            _, nb, _ = off_class(o)
            ob = 3 + nb   # match flag + 2 selector bits + offset bits
            lo = got + 1
            # all lengths got+1..l are available at this (cheapest so far) offset
            if l - lo > 40:   # long run: try the short codes and the longest few
                ls = list(range(lo, min(l, 21) + 1)) + list(range(max(lo, l - 8), l + 1))
            else:
                ls = range(lo, l + 1)
            for L in ls:
                c = ob + len_bits(L) + cost[i + L]
                if c < best:
                    best, ch = c, ("match", L, o)
            got = l
            if got >= maxl:
                break
        cost[i] = best
        choice[i] = ch
    toks, i = [], 0
    while i < n:
        t = choice[i]
        toks.append(t)
        i += 1 if t[0] == "lit" else t[1]
    return toks


def pack(data):
    w = BitWriter()
    for t in parse(data):
        if t[0] == "lit":
            w.bit(0)
            w.byte(t[1])
            continue
        _, l, o = t
        w.bit(1)
        if l == 2:
            w.bit(0)
        else:
            w.bit(1)
            if l <= 5:
                w.bits(l - 2, 2)
            else:
                w.bits(0, 2)
                if l <= 20:
                    w.bits(l - 5, 4)
                else:
                    w.bits(0, 4)
                    w.byte(l - 20)
        k, nb, base = off_class(o)
        w.bits(k, 2)
        v = o - base
        if nb >= 8:
            w.byte(v >> (nb - 8))
            w.bits(v & ((1 << (nb - 8)) - 1), nb - 8)
        else:
            w.bits(v, nb)
    # end marker: match, long, 00, 0000, byte 0
    w.bit(1); w.bit(1); w.bits(0, 2); w.bits(0, 4); w.byte(0)
    return bytes(w.out)


if __name__ == "__main__":
    sys.path.insert(0, __import__("os").path.dirname(__file__))
    from swd import expand
    data = open(sys.argv[1], "rb").read()
    a, b = int(sys.argv[2], 0), int(sys.argv[3], 0)
    seg = data[a:b]
    packed = pack(seg)
    back, end = expand(packed)
    print(f"{len(seg):#x} -> {len(packed):#x} bytes; round trip {'OK' if back == seg else 'FAILED'}")
