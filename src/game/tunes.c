#include "game.h"

typedef struct {
    u16 time;
    u8 note;
    u8 ch;
} Cmd;

typedef struct {
    u8 pad[0x1C];
    s32 map[12];
    s32 mode;
} S70;

typedef struct {
    u8 pad0[0x1C];
    u32 tab[12];
    s32 mode;
} Blk_3;

typedef struct { u8 pad[0x1C]; u32 tab[12]; u32 mode; } Game;

extern Cmd *D_800EFD48;
extern S70 D_800EFD70_6;
extern Blk_3 D_800EFD70_7;
extern Game D_800EFD70_3;

static inline void push_2(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

static inline s32 check(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) return 1;
    if (D_800EFD70_6.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_6.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70_6.mode == 9) << 1;
}

static inline void queue(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

static __inline__ void push_4(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

static inline s32 chk(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) {
        return 1;
    }
    if (D_800EFD70_3.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_3.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70_3.mode == 9) * 2;
}

static inline void push(u32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00_2[D_800CA858] = v;
        D_800CA858++;
    }
}

static inline void push_3(u32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004B710(s32 param_1)
{
    u8 *p;
    s32 i;

    D_800CA858 = 0;
    p = func_800BE9B0(D_800D7330);
    if (p != NULL) {
        D_800EECD4 = *(s32 *)(p + 0x18);
        D_800EECBC = D_800EECD4 + *(s32 *)(p + 0x14);
        D_800EFD4C = (s32)func_800BF79C(D_800D5604, D_800D1244);
        if (D_800EFD4C != 0) {
            D_800EECC4 = func_800BC908(0x2000, 0, D_800D7338, 0xFC);
            D_800EFD20 = 0;
            D_800EECD0 = 0;
            D_800EFD50 = 0;
            D_800D56B0 = func_8004F670;
            func_8000ED10(D_800EECE0, 0, 0x1000);
            D_800EFD54 = 0;
            D_800EECCC = param_1;
            D_800EFD24 = func_800BC908(0x130, 0, D_800D7338, 0x10C);
            func_8000ED10(D_800EFD24, 0, 0x130);
            for (i = 0; i < D_800EECCC; i++) {
                func_8004F70C((s32 *)(D_800EFD24 + i * 0x4C));
            }
            D_800CA860 = 0;
            D_800CA85C = 0;
            D_800CA844 = 1;
        }
    }
}

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

extern Stream *D_800EFD24_2;

static inline void noise(s16 *q, s32 n)
{
    u16 *p;
    s32 v;

    while (n != 0) {
        p = &D_800EECE0_2[D_800EFD54];
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
        D_800EFD24_2[2].vol = (D_800CA850 * D_800E32A0) >> 8;
    } else {
        D_800EFD24_2[2].vol = (D_800C2B58 * D_800E32A0) >> 8;
    }

    for (i = 0; i < D_800EECCC; i++) {
        s = &D_800EFD24_2[i];
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
        s = &D_800EFD24_2[i];
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
        s = &D_800EFD24_2[i];
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
        vol = D_800EFD24_2[i].vol;
        if (!(flags & 1)) {
            pos = D_800EECD0;
            if (!mixed) {
                for (j = 0; j < 0x80; j++) {
                    D_800EECC4_2[pos] = (*q++ * vol) >> 8;
                    pos = (pos + 1) & 0x7FF;
                }
            } else {
                for (j = 0; j < 0x80; j++) {
                    D_800EECC4_2[pos] += (*q++ * vol) >> 8;
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
                buf = D_800EECC4_2;
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
                buf = D_800EECC4_2;
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
            D_800EECC4_2[pos] = 0;
            pos = (pos + 1) & 0x7FF;
        }
    }
    D_800EECD0 = pos;
    D_800EFD20 += 0x80;
    D_800EFCF4 += 1;
    func_8004C014();
    return 1;
}

typedef struct {
    s32 smp;
    s32 f04;
} Ent_48;

typedef struct {
    s32 x;
    s32 smp;
} Pair_6;

typedef struct {
    s32 count;
    Ent_48 ent[1];
} Song;

typedef struct {
    u8 pad0[8];
    s32 f08;
    u8 pad0C[4];
    s32 f10;
} Smp;

typedef struct {
    u8 pad0[0x10];
    s32 note;
    s32 f14;
    u8 pad18[4];
    s32 f1C;
    s32 f20;
    s32 f24;
    u8 pad28[0x48 - 0x28];
    s32 vol;
} Chan;

extern Chan *D_800EFD24_3;
extern Smp *D_800EFD4C_2;

void func_8004C014(void)
{
    Song *s;
    Cmd *c;
    Smp *sm;
    Pair_6 *e;
    Song *s0;

    if (D_800CA860 == NULL) {
        return;
    }
    while (D_800EFD48->time <= D_800EFCF4) {
        c = D_800EFD48;
        s = D_800CA860;
        if (c->note == 0xFE) {
            D_800EFCF4 = 0;
            D_800EFD48 = (Cmd *)&s->ent[s->count];
        } else {
            if (c->note == 0xFF) {
                D_800EFD24_3[c->ch & 0xF].note = -1;
            } else {
                D_800EFD24_3[c->ch & 0xF].note = c->note;
                D_800EFD24_3[c->ch & 0xF].f14 = 0;
                e = &((Pair_6 *)s)[c->note];
                D_800EFD24_3[c->ch & 0xF].f20 = (sm = &D_800EFD4C_2[e->smp])->f08 + D_800EECD4;
                D_800EFD24_3[c->ch & 0xF].f24 = s->ent[c->note].f04;
                D_800EFD24_3[c->ch & 0xF].f1C = sm->f10;
                D_800EFD24_3[c->ch & 0xF].vol = D_800CA848_2[c->ch & 0xF] * D_800E32A0 >> 8;
            }
            D_800EFD48++;
        }
    }
}

typedef struct {
    u32 key;
    u32 val;
} Pair_7;

typedef struct {
    u32 n;
    Pair_7 e[1];
} Tab_5;

typedef struct {
    u32 n;
    u32 key;
    u32 x;
    u32 val;
    u32 y;
} Rec_29;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    u8 pad2[0x4C - 0x3C];
} Slot_11;

extern Tab_5 D_800EE4B8;
extern Slot_11 *D_800EFD24_4;

void func_8004C22C(s32 param_1)
{
    s32 n;
    u32 j;
    Rec_29 *r;
    s32 k;
    Tab_5 *tab;
    Tab_5 *t;
    u32 *pw;
    u32 key;

    if (D_800CA860_2 != 0) {
        func_8004C3C4();
    }
    if (param_1 >= 0) {
        D_800CA860_2 = func_800BF79C(D_800D5604_4, param_1);
        n = func_800BD924(D_800CA860_2);
        if (n > 0x800) {
            while (1) ;
        }
        tab = &D_800EE4B8;
        func_800AEC34(tab, D_800CA860_2, n);
        func_800BFEA4((u32 *)D_800D5604_4[1] + param_1);
        n = 0;
        D_800CA860_2 = (s32 *)tab;
        if (D_800EE4B8.n != 0) {
            r = D_800EFD4C;
            t = tab;
            pw = (u32 *)t;
            do {
                key = pw[n * 2 + 1];
                for (j = 0; j < r->n; j++) {
                    if (r[j].key == key) {
                        pw[n * 2 + 1] = j;
                        pw[n * 2 + 2] = r[j].val;
                        break;
                    }
                }
                if (j == r->n) {
                    while (1) ;
                }
                n++;
            } while (n < t->n);
        }
        D_800EFCF4 = 0;
        D_800EFD48 = D_800CA860_2 + (D_800CA860_2[0] * 2 + 1);
        for (n = 0; n < 2; n++) {
            D_800EFD24_4[n].f30 = -1;
            D_800EFD24_4[n].f2C = -1;
            D_800EFD24_4[n].f38 = -1;
            D_800EFD24_4[n].f34 = -1;
        }
        func_8004C014();
    }
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
    s32 f14;
    s32 pad18[5];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 pad3C[4];
} Ent_49; /* 0x4C */

extern Ent_49 *D_800EFD24_5;

void func_8004C3C4(void) {
    s32 i;
    s32 n;
    Ent_49 *p;
    Ent_49 *q;

    for (i = 0; i < D_800EECCC; i++) {
        D_800EFD24_5[i].f10 = -1;
        D_800EFD24_5[i].f14 = -1;
        D_800EFD24_5[i].f30 = -1;
        D_800EFD24_5[i].f2C = -1;
        D_800EFD24_5[i].f38 = -1;
        D_800EFD24_5[i].f34 = -1;
    }

    if (D_800CA860 != 0) {
        D_800CA85C = 1;
        for (i = 0; i < D_800EECCC; i++) {
            p = &D_800EFD24_5[i];
            for (;;) {
                if (p->f04 < 0) break;
                if (func_800BEF2C(p->f04) == 3) {
                    func_800BEF8C(p->f04);
                    p->f04 = -1;
                    break;
                }
            }
        }
        D_800CA85C = 0;
        D_800CA860 = 0;
    }
}

typedef struct { s32 idx; s32 cnt; s32 nidx; s32 ncnt; } Seq;
typedef struct { s32 f0, f4, f8, fC, f10; } Ent_50;

typedef struct {
    u8 pad[0xF4];
    s32 fF4; s32 fF8; s32 fFC; s32 f100; s32 f104; s32 f108;
    u8 pad2[0x120 - 0x10C];
    s32 f120; s32 f124; s32 f128; s32 f12C;
} Obj_37;

extern Seq D_800EFCE0_2;
extern Ent_50 *D_800EFD4C_3;

void func_8004C4E4(void)
{
    Obj_37 *obj; Ent_50 *e;
    if (D_800EFCF8 != 0) {
        if (--D_800EFCE0_2.cnt <= 0) {
            obj = D_800EFD24;
            e = &D_800EFD4C_3[D_800EFCE0_2.idx];
            obj->fF4 = D_800EFCE0_2.idx;
            obj->f120 = 0;
            obj->f124 = 0;
            obj->fF8 = 0;
            obj->f104 = e->f8 + D_800EECD4;
            obj->f108 = e->fC;
            obj->f100 = e->f10;
            obj->f12C = (D_800CA854_2[0] * D_800E32A0_2[0]) >> 8;
            D_800EFCF8--;
            D_800EFCE0_2.idx = D_800EFCE0_2.nidx;
            D_800EFCE0_2.cnt = D_800EFCE0_2.ncnt;
        }
    }
}

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
} Ent_51;

typedef struct {
    s32 pad[0x3D];
    s32 f0F4;
    s32 f0F8;
    s32 f0FC;
    s32 f100;
    s32 f104;
    s32 f108;
    s32 pad2[6];
    s32 f124;
} Ctl_8;

extern Ctl_8 *D_800EFD24_6;
extern Ent_51 *D_800EFD4C_4;

void func_8004C5A0(s32 param_1, s32 param_2, s32 arg2)
{
    if (D_800EECC0 != 0) {
        return;
    }
    if (param_2 == 0) {
        if (D_800EFD24_6->f0F4 != -1) {
            D_800EFD24_6->f0F4 = -1;
            D_800EFD24_6->f124 = 0;
            D_800CA854 = 0;
        }
        return;
    }
    if (D_800EFD24_6->f0F4 != param_1) {
        Ent_51 *t;
        s32 v10;
        D_800EFD24_6->f0F4 = param_1;
        t = D_800EFD4C_4 + param_1;
        D_800EFD24_6->f0F8 = 0;
        D_800EFD24_6->f104 = t->f08 + D_800EECD4;
        D_800EFD24_6->f108 = t->f0C;
        v10 = t->f10;
        D_800EFD24_6->f124 = 1;
        D_800EFD24_6->f100 = v10;
    }
    D_800CA854 = param_2;
}

typedef struct Plr_5 {
    u8 pad0[0xA8];
    s32 cur;   /* A8 */
    s32 fAC;   /* AC */
    s32 fB0;
    s32 fB4;
    s32 fB8;
    s32 fBC;
    u8 pad1[0xD4 - 0xC0];
    s32 fD4;
    s32 fD8;
    u8 pad2[0xE0 - 0xDC];
    s32 fE0;
} Plr_5;

typedef struct Rec_30 {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
} Rec_30;

typedef struct { u8 pad[0x662]; s16 f; u8 rest[0x7E0 - 0x664]; } Tbl7E0;

extern Plr_5 *D_800EFD24_7;
extern Rec_30 *D_800EFD4C_5;
extern Tbl7E0 D_800E37C0_52[];

void func_8004C644(void)
{
    s32 v;
    s32 n;
    s32 lo;
    s32 t;
    s32 f;
    s32 c;
    s32 e;
    Plr_5 *p;
    Rec_30 *r;

    if (D_800CA858_2[0] == 0) {
        return;
    }
    if (D_800EFD24_7->cur != -1 && D_800EECC8 >= 0) {
        return;
    }
    v = D_800EFD00[0];
    if (v < 0) {
        t = (v >> 16) & 0xFF;
        if (t == 1) {
            n = (v >> 8) & 0xFF;
            n = D_800E37C0_52[n].f;
            lo = v & 0xFF;
            if (lo < n) {
                n = lo;
            }
            lo = D_800F05CA - 2;
            if (lo < n) {
                n = lo;
            }
        } else if (t == 2) {
            n = (v >> 8) & 0xFF;
            n = D_800E37C0_52[n].f;
            lo = v & 0xFF;
            if (n < lo) {
                n = lo;
            }
            if (n <= 0) {
                n = 1;
            }
        } else {
            goto bad;
        }
        v = D_800CA820[n];
        goto ok;
    bad:
        D_800CA858_2[0]--;
        return;
    }
ok:
    if (D_800C2358 == 0) {
        if (D_800EFD24_7->cur != -1 && D_800EECC8 >= 0) {
            f = 1;
        } else {
            D_800EFD24_7->cur = v;
            r = &D_800EFD4C_5[v];
            D_800EFD24_7->fD4 = 0;
            D_800EFD24_7->fD8 = 0;
            D_800EFD24_7->fAC = 0;
            D_800EFD24_7->fB8 = r->f8 + D_800EECD4;
            D_800EECC8 = 0;
            D_800EFD24_7->fBC = r->fC;
            D_800EECDC = 0;
            D_800EFD24_7->fB4 = r->f10;
            f = 0;
        }
    }
    if (f == 0) {
        p = D_800EFD24_7;
        p->fD4 = 1;
        e = D_800CA850;
        c = --D_800CA858_2[0];
        p->fE0 = e;
        if (c != 0) {
            func_800AEC34(D_800EFD00, D_800EFD00 + 1, c << 2);
        }
        D_800EECB8 = D_800E76D0;
    }
}

typedef struct { u32 pad[7]; u32 map[12]; u32 mode; } Ctl_9;

extern Ctl_9 D_800EFD70_33;

static inline s32 check_2(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700_2) return 1;
    if (D_800EFD70_33.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_33.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70_33.mode == 9) * 2;
}

void func_8004C854(u32 a, u32 b)
{
    if (D_800E76D0 - D_800EFCF0 < 600) return;
    if (check_2(&a)) return;
    if (check_2(&b)) return;
    D_800EFCF0 = D_800E76D0;
    func_8000EFE8();
    if (func_8000EFE8() & 1) {
        if (D_800CA858 >= 5) return;
        push_2(0x75);
        push_2(D_800CA864[a + 12]);
        push_2(0xBA);
        push_2(D_800CA864[b + 18]);
    } else {
        if (D_800CA858 >= 5) return;
        push_2(D_800CA864[a + 12]);
        push_2(0xBA);
        push_2(D_800CA864[b + 18]);
        if (func_8000EFE8() & 1) {
            push_2(0x76);
        } else {
            push_2(0x74);
        }
    }
}

void func_8004CD78(s32 param_1)
{
    if (param_1 == 0) {
        push_2(0x83);
    } else if (param_1 == 1) {
        push_2(0x82);
    } else if (param_1 == 2) {
        push_2(0x81);
    }
}

void func_8004CE80(s32 param_1, s32 param_2)
{
    s32 *t;
    s32 v;

    if (D_800CA858 >= 6) {
        return;
    }
    if (check((u32 *)&param_1)) {
        return;
    }
    if (param_2 == D_800F05C8 - 2) {
        queue(D_800CA8D0[func_8000EFE8() & 1]);
    } else if (param_2 == 0) {
        queue(D_800CA8C4[func_8000EFE8() % 3]);
    } else {
        if (param_2 != 1) {
            return;
        }
        queue(0x88);
    }
    queue(D_800CA864[param_1 + 0x12]);
    if (param_1 != D_800EECD8) {
        queue(D_800CA8D8[func_8000EFE8() & 3]);
    } else {
        queue(D_800CA8E8[func_8000EFE8() % 3]);
    }
    D_800EECD8 = param_1;
}

typedef struct { u8 pad[0x668]; s16 f668; u8 pad2[0x7E0 - 0x66A]; } Plr_6;
typedef struct { u32 pad[7]; u32 map[8]; u32 f3C; u32 f40; u32 f44; u32 f48; u32 mode; } Ctl_10;

extern Plr_6 D_800E37C0_53[];
extern Ctl_10 D_800EFD70_34;

static inline s32 check_3(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) return 1;
    if (D_800EFD70_34.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_34.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70_34.mode == 9) * 2;
}

void func_8004D1F4(u32 a, u32 b)
{
    if (D_800F05D4 < 0x168) return;
    if (D_800CA858 >= 6) return;
    if (D_800E37C0_53[a].f668 >= 0) return;
    if (check_3(&a)) return;
    push_2(D_800CA864[a + 12]);
    if (b == 1) {
        if (a != D_800EECD8_2) {
            push_2(D_800CA8D8[func_8000EFE8() & 3]);
        } else {
            push_2(0x8A);
        }
        D_800EECD8_2 = a;
    } else if (b == D_800EFD70_34.f3C + D_800EFD70_34.f44) {
        if (func_8000EFE8() & 1) {
            push_2(0x7F);
        } else {
            push_2(0xBE);
            push_2(0x9F);
        }
    } else if (b == 2) {
        if (func_8000EFE8() & 1) {
            push_2(0x80);
        } else {
            push_2(0xBE);
            push_2(0x9E);
        }
    } else {
        push_2(0xBF);
        if (b == 3) {
            push_2(0x9D);
        } else if (b == 4) {
            push_2(0x9C);
        } else if (b == 5) {
            push_2(0x9A);
        }
    }
}

void func_8004D7DC(u32 param_1)
{
    s32 r;

    r = (s32)func_8000EFE8() % 3;
    if (check(&param_1)) {
        return;
    }
    if (r == 0) {
        if (D_800CA858 >= 6) {
            return;
        }
        if (func_8000EFE8() & 1) {
            queue(0xB8);
        } else {
            queue(0xB9);
        }
        queue(D_800CA864[param_1]);
        queue(0x78);
    } else if (r == 1) {
        if (D_800CA858 >= 7) {
            return;
        }
        queue(D_800CA864[param_1]);
        queue(0x99);
    } else {
        if (D_800CA858 >= 7) {
            return;
        }
        queue(D_800CA864[param_1]);
        queue(0x77);
    }
}

static __inline__ s32 chk_2(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700_2) {
        return 1;
    }
    if (D_800EFD70_7.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_7.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70_7.mode == 9) << 1;
}

void func_8004DBF8(u32 param_1)
{
    u32 r;

    if (D_800CA858 != 0) {
        return;
    }
    if (chk_2(&param_1) == 0) {
        r = func_8000EFE8() % 3;
        if (r == 0) {
            push_4(D_800CA864[param_1 + 0x12]);
        } else {
            push_4(D_800CA864[param_1 + 6]);
        }
        push_4(D_800CA8F4[r]);
    }
}

static __inline__ s32 chk_3(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) {
        return 1;
    }
    if (D_800EFD70_7.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_7.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70_7.mode == 9) << 1;
}

void func_8004DDD4(u32 param_1)
{
    if (*(s32 *)(D_800EFD64 + 0x2c) != 0) {
        func_8004DBF8_2(param_1);
        return;
    }
    if (D_800CA858 != 0) {
        return;
    }
    if (chk_3(&param_1) == 0) {
        push_4(D_800CA864[param_1 + 6]);
        push_4(D_800CA900[func_8000EFE8() % 3]);
    }
}

void func_8004DFB4(u32 a, u32 b)
{
    u32 r;

    if (D_800CA858 != 0) {
        return;
    }
    if ((s32)(D_800E76D0 - D_800EFD28[a]) < 0x1E0) {
        func_8004DBF8(b);
        return;
    }
    if (chk(&a) != 0) {
        return;
    }
    if (chk(&b) != 0) {
        return;
    }
    r = func_8000EFE8() % 3;
    if (r == 0) {
        push(D_800CA864[a + 12]);
    } else {
        push(D_800CA864[a + 6]);
    }
    push(D_800CA90C[r]);
    push(D_800CA864[b + 18]);
}

typedef struct {
    s32 pad[7];
    u32 map[12];
    s32 mode;
} Cfg_7;

extern Cfg_7 D_800EFD70_35;

static inline s32 check_4(u32 *p)
{
    if (D_800EFD70_35.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70_35.map[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70_35.mode == 9) * 2;
}

void func_8004E2E0(u32 a, s32 b)
{
    s32 r;
    u32 *pa;

    if (D_800CA858 < 7) {
        pa = &a;
        if (D_800C8CD8 != 0 && a == D_800E3700) {
            r = 1;
        } else {
            r = check_4(pa);
        }
        if (r != 1 && D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
            D_800EFD00[D_800CA858++] = b;
        }
    }
}

void func_8004E3EC(u32 a, u32 b, s32 c)
{
    if (D_800F05D4 < 0x168) {
        return;
    }
    if (D_800CA858 >= 6) {
        return;
    }
    if (chk(&a) != 0) {
        return;
    }
    if (chk(&b) != 0) {
        return;
    }
    push(D_800CA864[a + 12]);
    if (c != 0) {
        push(0x95);
    } else {
        push(0x96);
    }
    push(D_800CA864[b + 18]);
}

static inline s32 chk_4(u32 p, u32 *q)
{
    if (D_800C8CD8 != 0 && p == D_800E3700) {
        return 1;
    }
    if (D_800EFD70_3.mode == 2) {
        return 1;
    }
    if (*q >= 6) {
        return 1;
    }
    D_800EFD28_2[*q] = D_800E76D0;
    *q = D_800EFD70_3.tab[*q];
    if (*q >= 6) {
        return 1;
    }
    return (D_800EFD70_3.mode == 9) * 2;
}

void func_8004E6D8(u32 param_1, s32 param_2, u32 param_3)
{
    u32 t1 = param_1;
    u32 uVar2;
    u32 sh;
    if (chk_4(t1, &param_1) != 0) {
        return;
    }
    if (D_800C8CD8 != 0) {
        return;
    }
    if ((s32)(D_800E76D0 - D_800EE4B0) < 900) {
        return;
    }
    D_800EE4B0 = D_800E76D0;
    if (D_800CA858 >= 6) {
        return;
    }
    push_3(D_800CA864[param_1 + 0xc]);
    if (param_2 < (s32)param_3) {
        push_3(0x97);
        sh = t1 << 8;
        uVar2 = 0x80010000;
    } else {
        push_3(0x98);
        sh = t1 << 8;
        uVar2 = 0x80020000;
    }
    push_3(sh | (param_3 | uVar2));
}

void func_8004E960(u32 p)
{
    if (chk(&p) != 0) {
        return;
    }
    if (D_800CA858 >= 7) {
        return;
    }
    push(D_800CA864[p + 12]);
    push(0x79);
}

void func_8004EADC(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (D_800CA858 >= 6) {
        return;
    }
    push(0xB8);
    push(D_800CA864[param_1]);
    push(0x91);
}

void func_8004ECB4(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (D_800CA858 >= 7) {
        return;
    }
    push(D_800CA864[param_1]);
    push(0x7A);
}

void func_8004EE2C(u32 param_1)
{
    if (chk(&param_1) != 0) {
        return;
    }
    if (D_800CA858 >= 7) {
        return;
    }
    push(D_800CA864[param_1]);
    push(0x7B);
}

void func_8004EFA4(void)
{
    push_3(0x94);
    if (D_800EFD64_4->f2C == 0) {
        s32 k = D_800EFCF8;
        s32 v = D_800CA840;
        if (k < 2) {
            D_800EFCE0[k * 2] = v;
            D_800EFCE0[k * 2 + 1] = 0x1E;
            D_800EFCF8 = k + 1;
        }
    }
}

void func_8004F064(u32 param_1)
{
    s32 *b;
    s32 *s;

    if (chk(&param_1) != 0) {
        return;
    }
    if (param_1 == 0 || param_1 == 3) {
        return;
    }
    b = D_800EFD24_8;
    b[61] = 0x73;
    b[62] = 0;
    s = D_800EFD4C;
    b[65] = s[577] + D_800EECD4;
    b[66] = s[578];
    D_800EECC0 = 1;
    b[64] = s[579];
    b[73] = 0;
    D_800CA854_2[0] = D_800C2B58;
}

s32 func_8004F180(void) {
    return D_800CA860 != 0;
}

typedef struct {
    s32 p0;
    s32 p1;
    s32 w8;
    s32 wC;
    s32 rest[15];
} Ent4C;

extern Ent4C *D_800EFD24_9;

void func_8004F190(void)
{
    s32 i;
    if (D_800CA860 != 0) {
        func_8004C3C4();
    }
    func_800BFEA4((u32 *)((u8 *)D_800D5604_2[1] + D_800D1244 * 4));
    i = 0;
    func_800BCB04(D_800EECC4);
    if (0 < D_800EECCC) {
        do {
            func_800BCB04(D_800EFD24_9[i].w8);
            func_800BCB04(D_800EFD24_9[i].wC);
            i++;
        } while (i < D_800EECCC);
    }
    func_800BCB04(D_800EFD24_9);
    D_800CA844 = 0;
}

typedef struct {
    u8 pad0[0xA8];
    s32 a8;
    s32 ac;
    s32 b0;
    s32 b4;
    s32 b8;
    s32 bc;
    u8 pad1[0x14];
    s32 d4;
    s32 d8;
} Obj_38;

typedef struct {
    u8 pad0[8];
    s32 e8;
    s32 ec;
    s32 e10;
} Ent_52;

extern Obj_38 *D_800EFD24_10;
extern Ent_52 *D_800EFD4C_6;

int func_8004F270(s32 param_1, s32 param_2, s32 param_3)
{
    Ent_52 *iVar3;

    if (D_800C2358 != 0) {
        return D_800C2358;
    }
    if (D_800EFD24_10->a8 != -1 && param_2 <= D_800EECC8) {
        return 1;
    }
    iVar3 = D_800EFD4C_6 + param_1;
    D_800EFD24_10->d4 = 0;
    D_800EFD24_10->d8 = 0;
    D_800EFD24_10->a8 = param_1;
    D_800EFD24_10->ac = 0;
    D_800EFD24_10->b8 = iVar3->e8 + D_800EECD4;
    D_800EECC8 = param_2;
    D_800EFD24_10->bc = iVar3->ec;
    D_800EECDC = param_3;
    D_800EFD24_10->b4 = iVar3->e10;
    return 0;
}

s32 func_8004F328(void)
{
    s32 i, sum, w, f;

    sum = 0;
    for (i = 0; i < D_800CA858; i++) {
        w = *(s32 *)(D_800EFD4C + ((D_800EFD00[i] * 5) << 2) + 0xc);
        f = w * 2;
        if (*(s32 *)(D_800EFD4C + ((f * 5) << 2) + 0x10) & 1) {
            f = w << 2;
        }
        sum += (f * 0x3c) / 0x5622;
    }
    return sum;
}

u32 func_8004F3D0(u32 key)
{
    u32 *p;
    u32 i = 0;
    u32 n;

    p = D_800EFD4C;
    if (p[0] != 0) {
        n = p[0];
        do {
            if (p[5] == key) {
                return i;
            }
            i++;
            p += 5;
        } while (i < n);
    }
    return 0xFFFFFFFF;
}

static inline void push_5(s32 v, s32 w) {
    s32 n = D_800EFCF8;
    if (n < 2) {
        do {
            do {
                D_800EFCE0[n * 2] = v;
            } while (0);
        } while (0);
        D_800EFCE0[n * 2 + 1] = w;
        D_800EFCF8 = n + 1;
    }
}

void func_8004F410(s32 p) {
    if (D_800EFD64_3[11] == 0) {
        push_5(D_800CA838[p], 0x1E);
    }
}

void func_8004F47C(void)
{
    s32 val = -0x708;
    s32 i = 7;

    do {
        D_800EFD28[i] = val;
        i--;
    } while (i >= 0);
    D_800EECD8 = -1;
    D_800EECB8 = -1;
    D_800EE4B0 = -0x708;
    D_800EFCF0 = -0x708;
    D_800EFCF8 = 0;
    D_800EECC0 = 0;
}

void func_8004F4DC(s32 a0)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0) {
        s32 n = D_800CA858;
        if (n < 8) {
            D_800EFD00[n] = a0;
            D_800CA858 = n + 1;
        }
    }
}

typedef struct {
    u8 pad0[0xA0];
    u8 *p0;
    u8 *p1;
    s32 fA8;
    s32 fAC;
    u8 pad1[0xC4 - 0xB0];
    s32 fC4;
    s32 fC8;
    s32 fCC;
    s32 fD0;
} Obj_39;

extern Obj_39 *D_800EFD24_11;

void func_8004F534(void)
{
    Obj_39 *o;
    u8 *p;

    o = D_800EFD24_11;
    p = o->p0;
    o->fA8 = -1;
    o->fAC = -1;
    o->fC8 = -1;
    o->fC4 = -1;
    o->fD0 = -1;
    o->fCC = -1;
    func_8000ED10(p, 0, 0x220);
    func_8000ED10(D_800EFD24_11->p1, 0, 0x800);
    D_800CA858 = 0;
    func_8000ED10(D_800EECE0, 0, 0x1000);
    func_8000ED10(D_800EECC4, 0, 0x2000);
}

void func_8004F5C8(s32 on)
{
    if (on) {
        push_3(0x92);
    } else {
        push_3(0x93);
    }
}

void func_8004F670(s32 *p, s32 n)
{
    if (D_800EFD20 < n) {
        n = D_800EFD20;
    }
    if (D_800CA85C == 0) {
        D_800EFD20 = D_800EFD20 - n;
        while (n != 0) {
            *p = *p + D_800EECC4_2[D_800EFD50];
            p++;
            *p = *p + D_800EECC4_2[D_800EFD50];
            p++;
            D_800EFD50 = (D_800EFD50 + 1) & 0x7FF;
            n--;
        }
    }
}

void func_8004F70C(s32 *p) {
    if (p[2] == 0) {
        p[2] = func_800BC908(0x220, 0, (s32)&D_800D7338, 0x128);
        p[3] = func_800BC908(0x800, 0, (s32)&D_800D7338, 0x129);
        p[0] = 0;
        p[1] = -1;
        p[4] = -1;
        p[0xC] = -1;
    }
}

void func_8004F788(u8 *param_1)
{
    if (*(s32 *)(param_1 + 8) != 0) {
        func_800BCB04(*(s32 *)(param_1 + 8));
        func_800BCB04(*(s32 *)(param_1 + 0xC));
        func_8000ED10(param_1, 0, 0x4C);
        *(s32 *)(param_1 + 4) = -1;
        *(s32 *)(param_1 + 0x10) = -1;
        *(s32 *)(param_1 + 0x30) = -1;
    }
}

void func_8004F7E8(void) {
    if (D_800CA844 != 0) {
        while (func_8004B890() != 0) {
        }
    }
}

void func_8004F818(s16 *p, s32 n) {
    s32 v;
    s32 i2;
    s32 t;
    for (; n != 0; n--) {
        v = *p + ((s16)D_800EECE0_2[D_800EFD54] >> 1);
        if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        if (v < -0x7FFF) {
            v = -0x7FFF;
        }
        D_800EECE0_2[D_800EFD54] = v;
        *p = v;
        p++;
        i2 = D_800EFD54 + 1;
        t = i2;
        if (i2 < 0) {
            t = D_800EFD54 + 0x800;
        }
        D_800EFD54 = i2 - (t >> 11) * 0x800;
    }
}

typedef struct {
    char pad[0x48];
    s32 f48;
} Ent_53;

void func_8004F8A4(void)
{
    s32 k = 0;

    if (D_800CA844 != 0) {
        s32 *p = D_800CA848_2;
        s32 a2 = D_800E32A0;
        Ent_53 *e = D_800EFD24;
        do {
            s32 v = *p;
            p++;
            k++;
            e->f48 = (v * a2) >> 8;
            e++;
        } while (k < 4);
    }
}

s32 func_8004F8FC(u32 *p)
{
    u32 *base;
    u32 u;
    if (D_800C8CD8 != 0) {
        if (*p == D_800E3700) {
            return 1;
        }
    }
    base = D_800EFD70_5;
    if (base[19] == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    u = base[*p + 7];
    *p = u;
    if (u < 6) {
        return (base[19] == 9) << 1;
    }
    return 1;
}
