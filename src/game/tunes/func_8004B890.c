#include "ctx.h"

typedef struct Stream {
    s32 f0;
    s32 handle;
    s32 f8;
    s16 *pcm;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 flags;
    u8 *f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    s32 f44;
    s32 vol;
} Stream;

extern s32 D_800EECDC;
extern s32 D_800CA850;
extern s32 D_800C2B58;
extern s32 D_800E32A0;
extern Stream *D_800EFD24;
extern s32 D_800EECCC;
extern s32 D_800EFD54;
extern u16 D_800EECE0[];
extern s32 D_800CA85C;
extern s32 D_800EFD20;
extern s32 D_800EECC0;
extern s32 D_801009A4;
extern s32 D_800EECD0;
extern s32 *D_800EECC4;
extern s32 D_800EFCF4;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800BEF8C(s32 a0);
s32 func_800BEF2C(s32 a0);
s32 func_800BEC20(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void func_8004C644(void);
void func_8004C014(void);
void func_800AD7E0(s32, s32);
void func_800DB628(s32, s16 *, s32);

static inline void noise(s16 *q, s32 n)
{
    u16 *p;
    s32 v;

    while (n != 0) {
        p = &D_800EECE0[D_800EFD54];
        v = *q + ((s16)*p >> 1);
        if (v > 0x7FFF)
            v = 0x7FFF;
        if (v < -0x7FFF)
            v = -0x7FFF;
        *p = v;
        *q++ = v;
        D_800EFD54 = (D_800EFD54 + 1) % 0x800;
        n--;
    }
}

s32 func_8004B890(void)
{
    Stream *s;
    s32 i, j, n, v;
    s32 a, b;
    s32 started, mixed;
    s32 pos;
    u8 *addr, *end;
    s16 *q, *q1;
    u16 *p;
    s32 vol;
    s32 flags;
    s32 *buf;

    func_8004C644();
    if (D_800EECDC == 0) {
        D_800EFD24[2].vol = (D_800CA850 * D_800E32A0) >> 8;
    } else {
        D_800EFD24[2].vol = (D_800C2B58 * D_800E32A0) >> 8;
    }

    for (i = 0; i < D_800EECCC; i++) {
        s = &D_800EFD24[i];
        if (s->handle >= 0) {
            if (func_800BEF2C(s->handle) != 3)
                return 0;
            func_800BEF8C(s->handle);
            s->handle = -1;
            if (s->f38 >= 0) {
                func_800AD7E0(s->f8, 0x220);
                b = s->f8;
                q1 = s->pcm;
                for (j = 0; j < 8; j++) {
                    func_800DB628(b, q1, 0x80);
                    b += 0x44;
                    q1 += 0x80;
                }
                s->f2C = s->f34;
                s->f30 = s->f38;
                if (s->f3C != 0) {
                    noise(s->pcm, s->f18 << 7);
                }
            }
        }
    }

    if (D_800CA85C != 0)
        return 0;
    if (D_800EFD20 > 0x780)
        return 0;

    i = 0;
    j = 0;
    for (; i < D_800EECCC; i++) {
        s = &D_800EFD24[i];
        if (s->f10 == s->f30 && s->f14 >= s->f2C && s->f14 < s->f2C + 8)
            continue;
        if (s->f10 >= 0) {
        addr = s->f20;
        flags = (s32)(addr + s->f24);
        addr += s->f14 * 0x44;
        s->f18 = (u32)(flags - (s32)addr) / 68;
        if (s->f18 < 0)
            s->f18 = 0;
        if (s->f18 > 8)
            s->f18 = 8;
        if (s->f18 == 0) {
            if (s->f40 != 0) {
                s->f14 = 0;
                addr = s->f20 + s->f14 * 0x44;
                s->f18 = (u32)(s->f20 + s->f24 - addr) / 68;
                if (s->f18 < 0)
                    s->f18 = 0;
                if (s->f18 > 8)
                    s->f18 = 8;
            }
            if (s->f18 == 0) {
                s->f30 = -1;
                s->f10 = -1;
                s->f2C = s->f14;
                if (i == 3 && D_800EECC0 != 0)
                    D_800EECC0 = 0;
                continue;
            }
        }
        s->handle = func_800BEC20(D_801009A4, 0, (s32)addr, s->f8, 0x220);
        j = 1;
        s->f38 = s->f10;
        s->f34 = s->f14;
        } else {
            s->f30 = -1;
            s->f2C = s->f14;
        }
    }
    if (j)
        return 0;

    mixed = 0;
    for (i = 0; i < D_800EECCC; i++) {
        s = &D_800EFD24[i];
        q = s->pcm + (s->f14 - s->f2C) * 0x80;
        if (s->f18 <= 0)
            s->f30 = -1;
        if (s->f30 == -1) {
            if (s->f3C == 0)
                continue;
            q = s->pcm;
            if ((s->flags & 3) != 3) {
                if (s->f44 >= 0)
                    func_8000ED10((u8 *)q, 0, 0x100);
                noise(q, 0x80);
                if (++s->f44 >= 0x20)
                    s->f3C = 0;
            }
        } else {
            s->f44 = -1;
        }
        flags = s->flags;
        vol = D_800EFD24[i].vol;
        if (!(flags & 1)) {
            pos = D_800EECD0;
            if (!mixed) {
                for (j = 0; j < 0x80; j++) {
                    D_800EECC4[pos] = (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            } else {
                for (j = 0; j < 0x80; j++) {
                    D_800EECC4[pos] += (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            }
            s->f14++;
            s->f18--;
        } else {
            if (flags & 2) {
                q += 0x40;
                s->f14++;
                s->f18--;
            }
            pos = D_800EECD0;
            s->flags ^= 2;
            if (!mixed) {
                j = 0;
                buf = D_800EECC4;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 = (mixed + s->f28) / 2;
                    *b1 = mixed;
                    s->f28 = mixed;
                }
            } else {
                j = 0;
                buf = D_800EECC4;
                for (; j < 0x40; j++) {
                    s32 *b0 = (s32 *)(pos * 4 + (u32)buf);
                    s32 *b1;
                    pos = (pos + 1) & 0x7FF;
                    b1 = (s32 *)(pos * 4 + (u32)buf);
                    pos = (pos + 1) & 0x7FF;
                    mixed = (*q++ * vol) >> 8;
                    *b0 += (mixed + s->f28) / 2;
                    *b1 += mixed;
                    s->f28 = mixed;
                }
            }
        }
        mixed = 1;
    }
    if (!mixed) {
        pos = D_800EECD0;
        for (j = 0; j < 0x80; j++) {
            D_800EECC4[pos] = 0;
            pos = (pos + 1) & 0x7FF;
        }
    }
    D_800EECD0 = pos;
    D_800EFD20 += 0x80;
    D_800EFCF4 += 1;
    func_8004C014();
    return 1;
}
