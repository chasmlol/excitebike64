"""Reader for SN Systems / Psy-Q LNK object files (asn64 output), version 2.

Record layout follows pcsx-redux's psyq-obj-parser. Integers in records are little-endian; the code bytes
are big-endian MIPS. Only what the build needs: sections with bytes and relocations, symbols.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field

RELOC = {8: "REL32_BE", 12: "GPREL16_BE", 16: "REL32", 30: "GPREL16_LE", 74: "REL26", 82: "HI16", 84: "LO16",
         92: "REL26_BE", 96: "HI16_BE", 98: "LO16_BE", 100: "GPREL16"}


@dataclass
class Section:
    name: str
    align: int
    group: int
    data: bytearray = field(default_factory=bytearray)
    zeroes: int = 0
    pointer: int = 0
    relocs: list = field(default_factory=list)   # (offset, type name, expr)


@dataclass
class Symbol:
    name: str
    kind: str          # export / import / local / common
    section: int = 0
    offset: int = 0
    size: int = 0


class Reader:
    def __init__(self, b: bytes):
        self.b, self.p = b, 0

    def u8(self):
        v = self.b[self.p]
        self.p += 1
        return v

    def u16(self):
        v = struct.unpack_from("<H", self.b, self.p)[0]
        self.p += 2
        return v

    def u32(self):
        v = struct.unpack_from("<I", self.b, self.p)[0]
        self.p += 4
        return v

    def pstr(self):
        n = self.u8()
        s = self.b[self.p:self.p + n].decode("latin-1")
        self.p += n
        return s

    def take(self, n):
        v = self.b[self.p:self.p + n]
        self.p += n
        return v


def parse_expr(r: Reader):
    """Expressions as tuples: ('value', n) ('sym', idx) ('secbase', idx) ('secstart', idx) ('secend', idx)
    ('add'|'sub'|'div', left, right)."""
    op = r.u8()
    if op == 0:
        return ("value", r.u32())
    if op == 2:
        return ("sym", r.u16())
    if op == 4:
        return ("secbase", r.u16())
    if op == 12:
        return ("secstart", r.u16())
    if op == 22:
        return ("secend", r.u16())
    if op in (44, 46, 50):
        right = parse_expr(r)
        left = parse_expr(r)
        return ({44: "add", 46: "sub", 50: "div"}[op], left, right)
    raise ValueError(f"unknown expression opcode {op} at {r.p - 1:#x}")


class LnkFile:
    def __init__(self, data: bytes):
        r = Reader(data)
        if r.take(3) != b"LNK":
            raise ValueError("not an SN LNK object")
        self.version = r.u8()
        self.sections: dict[int, Section] = {}
        self.symbols: dict[int, Symbol] = {}
        self.locals: list[Symbol] = []
        cur = None
        while True:
            if r.p >= len(data):
                raise ValueError("object ends without END record")
            op = r.u8()
            if op == 0:
                break
            if op == 2:
                n = r.u16()
                s = self.sections[cur]
                s.pointer = len(s.data) + s.zeroes
                if s.zeroes:
                    s.data += bytes(s.zeroes)
                    s.zeroes = 0
                s.data += r.take(n)
            elif op == 6:
                cur = r.u16()
            elif op == 8:
                self.sections[cur].zeroes += r.u32()
            elif op == 10:
                t = r.u8()
                off = r.u16()
                s = self.sections[cur]
                s.relocs.append((off + s.pointer, RELOC.get(t, str(t)), parse_expr(r)))
            elif op == 12:
                idx, sec, off = r.u16(), r.u16(), r.u32()
                self.symbols[idx] = Symbol(r.pstr(), "export", sec, off)
            elif op == 14:
                idx = r.u16()
                self.symbols[idx] = Symbol(r.pstr(), "import")
            elif op == 16:
                idx, group, align = r.u16(), r.u16(), r.u8()
                self.sections[idx] = Section(r.pstr(), align, group)
            elif op == 18:
                sec, off = r.u16(), r.u32()
                sym = Symbol(r.pstr(), "local", sec, off)
                self.locals.append(sym)
            elif op == 20:   # group symbol
                r.u16(); r.u8(); r.pstr()
            elif op == 28:
                r.u16(); r.pstr()
            elif op == 46:
                r.u8()
            elif op == 48:
                idx, sec, size = r.u16(), r.u16(), r.u32()
                self.symbols[idx] = Symbol(r.pstr(), "common", sec, 0, size)
            elif op == 50:
                r.u16()
            elif op == 52:
                r.u16(); r.u8()
            elif op == 54:
                r.u16(); r.u16()
            elif op == 56:
                r.u16(); r.u32()
            elif op == 58:
                r.u16(); r.u32(); r.u16()
            elif op == 60:
                r.u16()
            elif op == 74:
                r.u16(); r.u32(); r.u16(); r.u32(); r.u16(); r.u32(); r.u16(); r.u32(); r.u32(); r.pstr()
            elif op == 76:
                r.u16(); r.u32(); r.u32()
            elif op in (78, 80):
                r.u16(); r.u32(); r.u32()
            elif op == 82:
                r.u16(); r.u32(); r.u16(); r.u16(); r.u32(); r.pstr()
            elif op == 84:
                r.u16(); r.u32(); r.u16(); r.u16(); r.u32()
                for _ in range(r.u16()):
                    r.u16()
                r.pstr(); r.pstr()
            elif op == 86:
                r.u16(); r.u32(); r.u16(); r.u32(); r.u16(); r.u32(); r.u16(); r.u32(); r.u32(); r.u32(); r.u32()
                r.pstr()
            else:
                raise ValueError(f"unknown LNK opcode {op} at {r.p - 1:#x}")
        for s in self.sections.values():
            if s.zeroes:
                s.data += bytes(s.zeroes)
                s.zeroes = 0

    def section_by_name(self, name):
        for i, s in self.sections.items():
            if s.name == name:
                return i, s
        return None, None

    def functions(self, section_name=".text"):
        """[(name, start, end)] of exported/local symbols in a section, ends at the next symbol."""
        idx, sec = self.section_by_name(section_name)
        if sec is None:
            return []
        syms = sorted({(s.offset, s.name) for s in list(self.symbols.values()) + self.locals
                       if s.kind in ("export", "local") and s.section == idx and not s.name.startswith("$")
                       and s.name not in ("gcc2_compiled.", "__gnu_compiled_c")})
        out = []
        for i, (off, name) in enumerate(syms):
            end = syms[i + 1][0] if i + 1 < len(syms) else len(sec.data)
            out.append((name, off, end))
        return out
