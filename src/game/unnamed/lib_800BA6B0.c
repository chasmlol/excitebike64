#include "game.h"

typedef struct {
    u8 pad[0x20];
    u8 a;
    u8 b;
    s8 c;
    s8 d;
    u8 pad2[0x0C];
} Ent;

extern Ent *D_800FE8D4_2;

s32 func_800BA6B0(s32 param_1, u32 param_2, s32 param_3, s32 arg3)
{
    s32 *piVar1;
    s32 iVar2, iVar3, iVar4, iVar6;
    s16 *psVar5;

    if (param_1 < 9 && param_3 < 0x5623) goto body;
ret1:
    return 1;
body:
    {
        D_800FE8DC = param_2;
        D_800FE8D8 = param_3;
        D_800FE8E0 = param_1;
        D_801004F0 = 0;
        D_800D5608 = 0;
        D_800FE8D4 = func_800BC908(param_1 * 0x30, 0, &D_800DB170, 0x2c8);
        if (D_800FE8D4 == 0) goto ret1;
        {
            func_8000ED10((u8 *)D_800FE8D4, 0, param_1 * 0x30);
            func_8000ED10((u8 *)&D_800FE8F0, 0, 0x1c00);
            D_800FE8D0 = piVar1 = func_800BC908(0x8000, 0, &D_800DB170, 0x2d4);
            if (piVar1 == 0) {
                func_800BCB04(D_800FE8D4);
                return 2;
            }
            iVar6 = 0;
            if (param_2 & 2) {
                s32 *tp = piVar1;
                do {
                    iVar4 = 0;
                    do {
                        s32 prod = D_800D3404[iVar6] * iVar4;
                        tp[iVar6 + iVar4 * 0x100] = prod / 0x1f;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            } else {
                s32 *tp = piVar1;
                s32 sh;
                do {
                    iVar4 = 0;
                    sh = (s8)iVar6;
                    iVar3 = 0;
                    do {
                        tp[iVar6 + iVar4 * 0x100] = iVar3 / 0x1f;
                        iVar3 = iVar3 + sh;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            }
            D_800DB8B8 = D_800FE8D4;
            D_800DB8BC = D_800FE8E0;
            D_800DB8CC = D_800FE8D0;
            if (func_800BF530(&D_800DB180, &D_800D5604) != 0) {
                func_8000EB00((s32)&D_800DB184);
                D_800D5604 = 0;
            } else {
                func_8000EB00((s32)&D_800DB1A4);
            }
            func_800BFC40(&D_800DB1BC, func_800BB350);
            return 0;
        }
}
}

typedef struct {
    s32 *data;
    s32 len;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;
} Ent_102;

extern Ent_102 D_800FE8F0_2[];

s32 func_800BA934(s32 param_1)
{
    s32 i;
    Ent_102 *e;
    s32 *f;
    u8 buf[8];
    u8 *bp;

    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0_2[i].len == 0) break;
    }
    if (i == 0x100) return -1;
    func_8000ED10((u8 *)&D_800FE8F0_2[i], 0, 0x1c);
    if (D_800FE8DC & 2) {
        func_800AEC60_3(D_800FE880, D_800DB1C0, param_1);
    } else {
        func_800AEC60_3(D_800FE880, D_800DB1C8, param_1);
    }
    f = func_800BE694(D_800FE880, D_800DB1D0);
    do { do { do { do { do { do { bp = buf; } while (0); } while (0); } while (0); } while (0); } while (0); } while (0);
    if (f == 0) return -2;
    func_800BE848(bp, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0_2[i].len, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0_2[i].b, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0_2[i].c, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0_2[i].d, 1, 4, f);
    func_800BE848(bp, 1, 4, f);
    e = &D_800FE8F0_2[i];
    e->data = func_800BC908(e->len, 0, D_800DB170, 0x368);
    if (e->data == 0) {
        func_800BE770(f);
        return -4;
    }
    func_800BE848((u8 *)e->data, 1, e->len, f);
    func_800BE770(f);
    return i;
}

typedef struct {
    void *p0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
    volatile s32 f14;
    s32 f18;
} SamEnt;

extern SamEnt D_800FE8F0_3[];

s32 func_800BAB1C(s32 param_1, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    SamEnt *e;
    char *pcVar6;
    s32 *q;
    SamEnt *ent;
    s32 *pp;

    if (D_800D5604 == 0) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0_3[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    func_8000ED10((u8 *)&D_800FE8F0_3[i], 0, 0x1c);
    if (D_800FE8DC & 2) {
        func_800AEC60_4(D_800FE880, D_800DB1D4, param_1);
    } else {
        func_800AEC60_4(D_800FE880, D_800DB1E0, param_1);
    }
    q = (s32 *)func_800BFF60_2(D_800D5604, D_800FE880, &D_800FE8F0_3[i].f18);
    ent = &D_800FE8F0_3[i];
    ent->f14 = (s32)q;
    ent->f4 = ((volatile s32 *)q)[1];
    e = (SamEnt *)ent->f14;
    pp = (s32 *)ent->f14;
    ent->f8 = ((s32 *)e)[2];
    ent->fc = ((s32 *)e)[3];
    pcVar6 = (char *)pp;
    pp = (s32 *)pp[4];
    ent->p0 = pcVar6 + 0x18;
    ent->f10 = (s32)pp;
    return i;
}

typedef struct {
    s32 data;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    Gear *hdr;
    s32 id;
} Slot_16;

extern Slot_16 D_800FE8F0_4[];

s32 func_800BAC58(s32 param)
{
    s32 i;
    Slot_16 *e;

    if (D_800D5604 == NULL) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0_4[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    e = &D_800FE8F0_4[i];
    func_8000ED10((u8 *)e, 0, sizeof(Slot_16));
    e->id = param;
    e->hdr = func_800BF79C(D_800D5604, param);
    e->f4 = e->hdr->f4;
    e->f8 = e->hdr->f8;
    e->fC = e->hdr->fC;
    e->f10 = e->hdr->f10;
    e->data = (s32)(e->hdr + 1);
    return i;
}

typedef struct {
    void *ptr;
    s32 size;
    s32 f8;
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
} Ent_103;

extern Ent_103 D_800FE8F0_5[];

s32 func_800BAD40(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 n;
    s32 t;
    Ent_103 *e;
    Ent_103 *base;
    Ent_103 *w;
    void *q;

    base = D_800FE8F0_5;
    n = 0;
    w = base;
top:
    if (w->size == 0) {
        goto done;
    }
    n++;
    t = n < 0x100;
    w++;
    if (t) {
        goto top;
    }
done:
    if (n == 0x100) {
        return -1;
    }
    e = (Ent_103 *)((s32)(n * 0x1C) + (s32)base);
    func_8000ED10((u8 *)e, 0, 0x1c);
    e->size = p2;
    if (p2 == 0) {
        return -3;
    }
    q = func_800BC908(p2, 0, D_800DB170, 0x421);
    e->ptr = q;
    if (q == 0) {
        return -4;
    }
    func_800B9FE0(q, p1, e->size);
    e->f8 = p3;
    e->fc = p4;
    e->f10 = p5;
    e->f18 = -1;
    return n;
}

typedef struct { u32 w[12]; } RecA_3;

void func_800BAE54(s32 param_1, s32 param_2)
{
    RecA_3 *a;
    Rec_3 *b;
    RecA_3 *ab;
    Rec_3 *bb;
    u32 t;
    u32 fl;
    func_8000EE20();
    ab = D_800FE8D4;
    a = (RecA_3 *)((s32)(param_1 * 0x30) + (s32)ab);
    bb = D_800FE8F0_6;
    b = (Rec_3 *)((s32)(param_2 * 0x1c) + (s32)bb);
    a->w[2] = a->w[2] % b->w[1];
    a->w[7] = b->w[2];
    a->w[9] = b->w[3];
    t = b->w[0];
    fl = a->w[1];
    a->w[10] = param_2;
    a->w[0] = t;
    if (fl & 2) {
        a->w[6] = b->w[3];
    } else {
        a->w[6] = b->w[1];
    }
    func_8000EE28();
}

void func_800BAF14(s32 i, s32 val)
{
    s32 a, b, x, y;

    D_800FE8D4_2[i].a = val;
    if (D_800FE8DC & 1) {
        a = D_800FE8D4_2[i].a;
        b = D_800FE8D4_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        D_800FE8D4_2[i].c = x;
        D_800FE8D4_2[i].d = y;
    }
}

void func_800BAFD8(s32 i, s32 val)
{
    s32 a, b, x, y;

    D_800FE8D4_2[i].b = val;
    if (D_800FE8DC & 1) {
        a = D_800FE8D4_2[i].a;
        b = D_800FE8D4_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        D_800FE8D4_2[i].c = x;
        D_800FE8D4_2[i].d = y;
    }
}

typedef struct {
    u8 pad[0x22];
    u8 a;
    u8 b;
    u8 pad2[0xC];
} Ch_2;

extern Ch_2 *D_800FE8D4_3;

void func_800BB09C(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 n;
    s32 ix;
    u8 *pb;

    D_800DB8C0 = param_2;
    D_800DB8C4 = param_1;
    D_800DB8DD = 1;
    for (i = 0; i < D_800FE8E0; i++) {
        D_80100600[i * 2] = D_800FE8D4_3[i].a;
        D_80100600[i * 2 + 1] = D_800FE8D4_3[i].b;
        D_800FE8D4_3[i].a = (D_800FE8D4_3[i].a * D_800D3400) >> 8;
        D_800FE8D4_3[i].b = (D_800FE8D4_3[i].a * D_800D3400) >> 8;
    }
    if (D_800FE8DC & 1) {
        func_800DBAA0();
    } else {
        func_800DB8E0();
    }
    if (D_800D5608 != 0) {
        n = param_2;
        if (D_800FE8DC & 1) {
            n = param_2 << 1;
        }
        if (D_800FE8DC & 2) {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = D_80100500;
                do {
                    pb = D_800FE870;
                    ix = D_800FE874;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb) << 8;
                    pb[ix] = param_1[i] >> 10;
                    ix++;
                    D_800FE874 = ix;
                    if (ix == D_800FE878) {
                        D_800FE874 = 0;
                    }
                    i++;
                } while (i < n);
            }
        } else {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = D_80100500;
                do {
                    pb = D_800FE870;
                    ix = D_800FE874;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb);
                    pb[ix] = param_1[i] >> 2;
                    ix++;
                    D_800FE874 = ix;
                    if (ix == D_800FE878) {
                        D_800FE874 = 0;
                    }
                    i++;
                } while (i < n);
            }
        }
    }
    D_801004F0 += param_2;
    for (i = 0; i < D_800FE8E0; i++) {
        D_800FE8D4_3[i].a = D_80100600[i * 2];
        D_800FE8D4_3[i].b = D_80100600[i * 2 + 1];
    }
}

struct Hdr_13 {
    u32 flags;
    u32 size;
};

void func_800BB350(Hdr_13 **param_1)
{
    Hdr_13 *p;
    Hdr_13 *q;
    u8 **bp;

    p = *param_1;
    if ((p->flags & 0xF) >= 2) {
        if (D_800D560C != 0) {
            if (D_800D5614 < p->size + 0x1c) goto spin;
            func_800AEC34(D_800D5610, p, p->size + 0x1c);
            func_800BCB04(*param_1);
            p = (Hdr_13 *)D_800D5610;
            goto join;
        spin:
            while (1) {}
        join:
            q = D_800D560C(p);
            bp = &D_800D5610;
            if (q == 0) {
                func_8000ECB8(1);
            }
            if (p != (Hdr_13 *)*bp) {
                func_800BCB04(p);
            }
            if (q == (Hdr_13 *)*bp) {
                p = func_800BC908(p->size + 0x1c, 0, D_800DB170, 0x67b);
                func_800AEC34(p, q, p->size + 0x1c);
                q = p;
            }
            *param_1 = q;
        } else {
            func_8000ECB8(1);
        }
    }
}

s32 func_800BB47C(s32 *a, s32 *b, s32 *c)
{
    *a = 8;
    *b = 3;
    *c = 0x5622;
    return 0;
}

void func_800BB49C(void)
{
    func_800BCB04(D_800FE8D0);
    func_800BCB04(D_800FE8D4);
    if (D_800D5604 != 0) {
        func_800BFDD4(D_800D5604);
    }
    D_800D5604 = 0;
}

void func_800BB4EC(u32 p)
{
    s32 *w;
    s32 *e;
    if (p < 0x101) {
        w = D_800FE8F0_7;
        e = &w[p * 7];
        if (e[1] != 0) {
            if (e[6] != -1) {
                func_800BFEA4((u32 *)(D_800D5604_2[1] + e[6] * 4));
            } else {
                func_800BCB04(e[0]);
            }
            func_8000ED10((u8 *)&D_800FE8F0_7[p * 7], 0, 0x1c);
        }
    }
}

typedef struct {
    s32 f[7];
} Rec_58;

extern Rec_58 D_800FE8F0_8[];

void func_800BB590(s32 idx, s32 *out) {
    Rec_58 *base = D_800FE8F0_8;
    Rec_58 *r = base + idx;
    out[0] = r->f[1];
    out[1] = r->f[2];
    out[2] = r->f[3];
    out[3] = r->f[4];
}

typedef struct {
    u32 pad;
    u32 w[4];
    u32 tail[2];
} Ent_104;

extern Ent_104 D_800FE8F0_9[];

void func_800BB5CC(s32 i, u32 *src)
{
    Ent_104 *base = D_800FE8F0_9;
    Ent_104 *e = &base[i];
    e->w[0] = src[0];
    e->w[1] = src[1];
    e->w[2] = src[2];
    e->w[3] = src[3];
}

void func_800BB608(s32 param_1, s32 param_2)
{
    s32 iVar1;
    s32 iVar2;

    func_8000EE20();
    iVar1 = param_2 % D_800FE8D8 * 65536;
    iVar2 = iVar1 / D_800FE8D8;
    iVar1 = param_1 * 0x30 + D_800FE8D4;
    *(s32 *)(iVar1 + 0x10) = param_2 / D_800FE8D8;
    *(s32 *)(iVar1 + 0x14) = iVar2 * 65536;
    func_8000EE28();
}

void func_800BB6CC(s32 i, s32 v)
{
    *(s32 *)((u8 *)D_800FE8D4 + i * 48 + 8) = v;
}

s32 func_800BB6EC(s32 a0)
{
    return *(s32 *)(D_800FE8D4 + a0 * 0x30 + 8);
}

typedef struct { s32 f0; s32 f1; s32 f2; s32 f3; s32 f4; s32 f5; s32 f6; } Row_7;
typedef struct { u8 pad0[4]; u32 flags; u8 pad8[0x10]; s32 val; u8 pad1c[0xC]; s32 idx; u8 pad2c[4]; } Ent_105;

extern Row_7 D_800FE8F0_10[];

void func_800BB710(s32 n, u32 flags)
{
    Ent_105 *base = D_800FE8D4;
    Ent_105 *e = (Ent_105 *)((u32)(n * 0x30) + (u32)base);
    e->flags = flags;
    if (flags & 2) {
        e->val = D_800FE8F0_10[e->idx].f3;
        return;
    }
    e->val = D_800FE8F0_10[e->idx].f1;
}

s32 func_800BB780(s32 i)
{
    return *(s32 *)((u8 *)D_800FE8D4 + i * 48 + 4);
}

s32 func_800BB7A4(s32 arg0) {
    return D_801004F0;
}

void func_800BB7B4(s32 a, s32 b, s32 c) {
    s32 flags;

    D_800DB8C4 = a;
    D_800DB8C8 = b;
    flags = D_800FE8DC;
    D_800DB8C0 = c;
    if (flags & 1) {
        D_800DB8C0 = c << 1;
    }
    if (flags & 2) {
        D_800DB8D0 = 0;
        D_800DB8DC = 0;
        D_800DB8D4 = (s32)0xFFFF8001;
        D_800DB8D8 = 0x7FFF;
        func_800DBD00();
    } else {
        D_800DB8DC = 0;
        D_800DB8D0 = 0;
        D_800DB8D4 = 0;
        D_800DB8D8 = 0xFF;
        func_800DBCA4();
    }
}

s32 func_800BB85C(s32 size)
{
    s32 i;
    u8 *q;

    if (D_800D5608 != 0) {
        return -1;
    }
    if (D_800FE8DC & 1) {
        size <<= 1;
    }
    D_800FE870 = q = func_800BC908(size, 0, D_800DB170, 0x601);
    if (q == NULL) {
        return 1;
    }
    func_8000ED10(q, 0, size);
    D_800FE878 = size;
    i = 0xFF;
    {
        u8 *pp = D_80100500 + i;
        do {
            *pp = i;
            i--;
            pp--;
        } while (i >= 0);
    }
    D_800FE874 = 0;
    D_800D5608 = 1;
    return 0;
}

void func_800BB920(s32 p)
{
    if (p != 0) {
        D_800D5608 = 1;
        return;
    }
    if (D_800D5608 != 0) {
        func_8000ED10(D_800FE870, 0, D_800FE878);
    }
    D_800D5608 = 0;
}

void func_800BB978(s32 k)
{
    s32 i = 0;
    s32 x;
    u8 *dst;
    do {
        x = (s8)i * k;
        dst = D_80100500 + i;
        if (x < 0) {
            x += 0xFF;
        }
        *dst = (u8)(x >> 8);
        i++;
    } while (i < 0x100);
}

void func_800BB9C0(s32 arg0)
{
    if (D_800D5608 != 0) {
        func_800BCB04(D_800FE870);
        D_800D5608 = 0;
    }
}

typedef struct {
    u8 *buf;
    u32 len;
    u8 unk8[0x1C - 0x8];
} Chan_7;

extern Chan_7 D_800FE8F0_11[];

void func_800BB9FC(u32 idx, u8 *src, s32 n) {
    s32 i;

    if (idx <= 0x100) {
        if (D_800FE8F0_11[idx].len < n) {
            n = D_800FE8F0_11[idx].len;
        }
        if (D_800FE8DC & 2) {
            func_800B9FE0(D_800FE8F0_11[idx].buf, src, n);
        } else {
            for (i = 0; i < n; i++) {
                D_800FE8F0_11[idx].buf[i] = D_800D3404[src[i]] >> 8;
            }
        }
    }
}

void func_800BBAB0(s32 a0)
{
    D_800D560C = a0;
}

void func_800BBABC(s32 a, s32 b)
{
    D_800D5610 = a;
    D_800D5614 = b;
}

void func_800BBAD0(s32 i)
{
    s32 a, b, x, y;
    if (D_800FE8DC & 1) {
        a = D_800FE8D4_2[i].a;
        b = D_800FE8D4_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        D_800FE8D4_2[i].c = x;
        D_800FE8D4_2[i].d = y;
    }
}

s32 func_800BBB88(s32 a0)
{
    return *(s32 *)(D_800FE8F0_12 + a0 * 0x1C);
}

void func_800BBBAC(s32 x)
{
    D_800D3400 = x;
}
