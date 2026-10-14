#include "game.h"

typedef struct Ent_110 { struct Ent_110 *next; s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; u32 color; } Ent_110;

extern Ent_110 *D_801054E0[];

struct Obj_82 {
    u32 count;
    u32 *data;
    u8 pad8[0x400];
    u8 *bytes;
    char name[0x80];
    u8 sub[0x88];
    u32 f514;
    u32 buf[1];
};

s32 func_800BF530(char *path, Obj_82 **out)
{
    char buf[128];
    s32 flag;
    s32 *f;
    u32 n;
    u32 cnt;
    Obj_82 *o;
    u32 j;
    u8 *p;
    u8 t;

    flag = 0;
    *(u8 *)&flag = 0xFF;
    if (flag == 0xFF) {
        D_800D5734 = 0;
    } else if (flag == (s32)0xFF000000) {
        D_800D5734 = 1;
    } else {
        func_8000EB00(D_800DB420);
        func_8000ECB8(1);
    }

    func_8000EF8C(buf, path);
    func_8000ED30(buf, D_800DB434);
    f = func_800BE694(buf, D_800DB43C);
    if (f == NULL) {
        return 1;
    }
    func_800BE7F4(f, 0, 2);
    n = func_800BE83C(f);
    func_800BE7F4(f, 0, 0);
    if (D_80100AB0 == NULL) {
        cnt = n >> 2;
        o = func_800BC908(cnt * 12 + 0x518, 0, D_800DB440, 0xB0);
        if (o == NULL) {
            return 2;
        }
        o->f514 = 0;
    } else {
        cnt = n >> 2;
        o = D_80100AB0;
        o->f514 = 1;
        D_80100AB0 = NULL;
    }
    func_800BFDA4(o);
    o->count = n >> 2;
    if (n >> 2 == 0) {
        return 2;
    }
    o->data = o->buf;
    func_8000ED10(o->buf, 0, n);
    o->bytes = (u8 *)(o->buf + cnt * 2);
    func_800BE848(o->buf + cnt * 2, 1, n, f);

    if (D_800D5734 != 0) {
        flag = 0;
        if ((s32)o->count > 0) {
            do {
                j = 0;
                do {
                    p = (u8 *)(flag * 4 + (s32)o->bytes);
                    t = p[j];
                    p[j] = p[3 - j];
                    ((u8 *)((s32)o->bytes + flag * 4))[3 - j] = t;
                    j++;
                } while (j < 2);
                flag++;
            } while (flag < (s32)o->count);
        }
    }
    func_800BE770(f);
    func_8000EF8C(o->name, path);
    sprintf(o->sub, D_800DB44C, path);
    *out = o;
    return 0;
}

#define SWAP_BYTES(x) { if (D_800D5734 != 0) { u32 _i; for (_i = 0; _i < 2; _i++) { u8 _t = ((u8 *)&(x))[_i]; ((u8 *)&(x))[_i] = ((u8 *)&(x))[3 - _i]; ((u8 *)&(x))[3 - _i] = _t; } } }

typedef struct {
    u8 key[4];
    void (*fn)(u32);
} Ent8_3;

extern Ent8_3 D_80100AB8[16];

u32 func_800BF79C(u32 *pa0, s32 pa1)
{
    u32 *p1 = pa0;
    s32 p2 = pa1;
    s32 *piVar2;
    s32 *piVar3;
    u32 uVar5;
    u32 i;
    u32 *puVar7;
    s32 local_34;
    s32 local_30;
    s32 local_2c;
    u32 local_38;
    u8 local_40[4];
    u8 *bb;
    s32 iVar8;
    s32 iVar4;

    if (D_800D5730_2 != 0) {
        (*D_800D5730_2)();
    }
    uVar5 = ((u32 *)p1[1])[p2];
    bb = (u8 *)(p1 + 0x123);
    if (uVar5 & 1) {
        goto inc;
    }
    if (uVar5 != 0) {
        return uVar5;
    }
inc:
    p1[0x143] = p1[0x143] + 1;
    piVar2 = func_800BE694(bb, D_800DB43C);
    if (piVar2 == 0) {
        goto dec0;
    }
    func_800BE7F4(piVar2, ((s32 *)p1[0x102])[p2], 0);
    func_800BE848(local_40, 1, 4, piVar2);
    func_800BE848((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    if (D_80100AB0 != 0) {
        piVar3 = D_80100AB0;
        local_2c = 1;
        D_80100AB0 = 0;
    } else {
        local_2c = 0;
        piVar3 = func_800BC908(local_38, 0, D_800DB440, 0x1cd);
    }
    if (((u32)piVar3 & 3) != 0) {
        func_8000EB00(D_800DB464, (s32 *)piVar3);
        func_800BE770(piVar2);
        return 0;
    }
    if (piVar3 == 0) {
        p1[0x143] = p1[0x143] - 1;
        func_800BE770(piVar2);
        return 0;
    }
    local_34 = func_800BE848((u8 *)piVar3, 1, local_38, piVar2);
    if (local_34 == (s32)local_38) {
        goto main_path;
    }
    func_800BCB04((s32)piVar3);
    func_800BE770(piVar2);
dec0:
    p1[0x143] = p1[0x143] - 1;
    return 0;

main_path:
    func_800BE848((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    for (i = 0; i < local_38; i++) {
        func_800BE848((u8 *)&local_34, 1, 4, piVar2);
        SWAP_BYTES(local_34);
        local_30 = *(s32 *)((u8 *)piVar3 + local_34);
        SWAP_BYTES(local_30);
        *(u32 *)((u8 *)piVar3 + local_34) = p1[1] + local_30 * 4;
        puVar7 = (u32 *)(local_30 * 4 + p1[1]);
        if (*puVar7 == 0) {
            *puVar7 = (p1[0x143] << 2) | 1;
        }
    }
    if (local_2c != 0) {
        ((u32 *)p1[1])[p2] = (u32)piVar3 | 2;
    } else {
        ((s32 *)p1[1])[p2] = (s32)piVar3;
    }
    func_800BE770(piVar2);
    for (i = 0; i < p1[0]; i++) {
        if (((u32 *)p1[1])[i] == ((p1[0x143] << 2) | 1)) {
            func_800BF79C(p1, i);
            i = 0xffffffff;
        }
    }
    for (i = 0; i < 16; i++) {
        iVar8 = func_8000ECE0(D_80100AB8[i].key[0]);
        iVar4 = func_8000ECE0(local_40[0]);
        if (iVar8 == iVar4) {
            iVar8 = func_8000ECE0(D_80100AB8[i].key[1]);
            iVar4 = func_8000ECE0(local_40[1]);
            if (iVar8 == iVar4) {
                iVar8 = func_8000ECE0(D_80100AB8[i].key[2]);
                iVar4 = func_8000ECE0(local_40[2]);
                if (iVar8 == iVar4) {
                    D_80100AB8[i].fn(p1[1] + p2 * 4);
                    break;
                }
            }
        }
    }
    piVar3 = (s32 *)((u32 *)p1[1])[p2];
    p1[0x143] = p1[0x143] - 1;
    return (u32)piVar3;
}

typedef struct {
    u8 name[3];
    u8 pad;
    s32 val;
} Ent_109;

extern Ent_109 D_80100AB8_2[16];

void func_800BFC40(u8 *name, s32 val)
{
    s32 i;

    if (D_800D5738 == 0) {
        func_8000ED10((u8 *)D_80100AB8_2, 0, 0x80);
        D_800D5738 = 1;
    }
    for (i = 0; i < 16; i++) {
        if (func_8000ECE0(D_80100AB8_2[i].name[0]) == func_8000ECE0(name[0]) &&
            func_8000ECE0(D_80100AB8_2[i].name[1]) == func_8000ECE0(name[1]) &&
            func_8000ECE0(D_80100AB8_2[i].name[2]) == func_8000ECE0(name[2])) {
            break;
        }
    }
    if (i == 16) {
        for (i = 0; i < 16; i++) {
            if (D_80100AB8_2[i].val == 0) {
                break;
            }
        }
        if (i == 16) {
            func_8000EB00((s32)D_800DB47C);
            func_8000ECB8(1);
        }
    }
    __builtin_memcpy(D_80100AB8_2[i].name, name, 3);
    D_80100AB8_2[i].val = val;
}

void func_800BFDA4(s32 *p)
{
    s32 i = 0xFF;
    s32 *q = p + 0xFF;
    p[0x143] = 0;
    p[0x102] = 0;
    p[0] = 0;
    do {
        q[2] = 0;
        i = i - 1;
        q = q - 1;
    } while (i >= 0);
    p[1] = 0;
    p[0x144] = 0;
}

void func_800BFDD4(s32 *param_1)
{
    s32 i;
    s32 *pp;
    s32 *n;
    s32 *nx;

    i = 0;
    if (param_1[0] > 0) {
        do {
            func_800BFEA4((u32 *)(param_1[1] + i * 4));
            i = i + 1;
        } while (i < param_1[0]);
    }
    i = 0;
    pp = param_1;
    do {
        n = (s32 *)pp[2];
        while (n != 0) {
            nx = (s32 *)n[0];
            func_800BCB04((void *)n[1]);
            func_800BCB04(n);
            n = nx;
        }
        i = i + 1;
        pp = pp + 1;
    } while (i < 0x100);
    if ((param_1[0x145] & 1) == 0) {
        func_800BCB04(param_1);
    }
}

void func_800BFEA4(u32 *p) {
    u32 v = *p;
    if (v != 0) {
        if ((v & 2) == 0) {
            func_800BCB04(v);
            *p = 0;
        } else {
            *p = 0;
        }
    }
}

struct ListHdr {
    s32 n;
    u32 *list;
};

void func_800BFEE4(ListHdr *h)
{
    s32 i;
    u32 *e;
    i = 0;
    while (i < h->n) {
        e = (u32 *)((i << 2) + (s32)h->list);
        if (*e != 0) {
            if ((*e & 2) == 0) {
                func_800BCB04(*e);
            }
            *e = 0;
        }
        i++;
    }
}

typedef struct Node_52 {
    struct Node_52 *next;
    u8 *name;
    u32 val;
} Node_52;

void func_800BFF60(u32 *param_1, u8 *param_2, u32 *param_3)
{
    s32 i = 0;
    u32 sum = 0;
    u32 n;
    Node_52 *p;

    for (;;) {
        n = strlen(param_2);
        if (!((u32)i < n)) break;
        if (!(i < 16)) break;
        sum += func_8000ECE0(param_2[i]);
        i++;
    }
    {
        s32 base = (s32)param_1;
        p = *(Node_52 **)(base + ((sum & 0xFF) << 2) + 8);
    }
    if (p != NULL) {
        do {
            if (func_8000EDA0(p->name, param_2) == 0) break;
            p = p->next;
        } while (p != NULL);
        if (p != NULL) goto LAB;
    }
    func_8000EB00((s32)D_800DB454, param_2);
    func_8000ECB8(1);
LAB:
    if (param_3 != NULL) *param_3 = p->val;
    func_800BF79C(param_1, p->val);
}

s32 func_800C0050(u8 *a0, u8 *a1)
{
    u8 buf[0x50];
    s32 cnt;
    s32 *q;
    u32 k;
    s32 i;

    func_8000EF8C((char *)buf, (char *)(a0 + 0x40c));
    func_8000ED30((char *)buf, D_800DB494);
    q = func_800BE694(buf, D_800DB43C);
    if (q == 0) {
        return -1;
    }
    func_800BE848((u8 *)&cnt, 1, 4, q);
    k = 0;
    if (D_800D5734 != 0) {
        do {
            u8 t1 = ((u8 *)&cnt)[k];
            u8 t2 = ((u8 *)&cnt)[3 - k];
            ((u8 *)&cnt)[k] = t2;
            ((u8 *)&cnt)[3 - k] = t1;
            k++;
        } while (k < 2);
    }
    i = 0;
    if (cnt > 0) {
        do {
            func_800BE848(buf, 1, 0x10, q);
            if (func_8000EDA0(a1, buf) == 0) {
                func_800BE770(q);
                return i;
            }
            i++;
        } while (i < cnt);
    }
    func_800BE770(q);
    return -1;
}

struct ResFile_2 {
    u8 unk0[0x408];
    s32 *offsets;
    u8 unk40C[0x48C - 0x40C];
    char path[1];
};

s32 func_800C0174(ResFile_2 *r, s32 idx) {
    void *f;
    u8 tmp[8];
    s32 val;
    u32 i;

    f = func_800BE694(r->path, "rb");
    if (f == NULL) {
        return -1;
    }
    func_800BE7F4(f, r->offsets[idx], 0);
    func_800BE848(tmp, 1, 4, f);
    func_800BE848(&val, 1, 4, f);
    if (D_800D5734 != 0) {
        for (i = 0; i < 2; i++) {
            u8 t = ((u8 *)&val)[i];

            ((u8 *)&val)[i] = ((u8 *)&val)[3 - i];
            ((u8 *)&val)[3 - i] = t;
        }
    }
    func_800BE770(f);
    return val;
}

void func_800C0258(s32 a0)
{
    D_80100AB0 = a0;
}

void func_800C0264(s32 *param_1, u32 param_2)
{
    s32 i;
    u32 *p;
    u32 v;

    if (param_2 == 0) {
        return;
    }
    i = 0;
    if (param_1[0] <= 0) {
        return;
    }
    do {
        if ((param_1[1] & ~3) == (param_2 & ~3)) {
            p = (u32 *)(i * 4 + param_1[1]);
            v = *p;
            if (v == 0) {
                return;
            }
            if ((v & 2) == 0) {
                func_800BCB04(v);
            }
            *p = 0;
            return;
        }
        do { i++; } while (0);
    } while (i < param_1[0]);
}

u32 func_800C02E4(u8 *p)
{
    u32 i = 0;
    u32 sum = 0;
    u32 len;
    u8 c;

    for (;;) {
        len = strlen(p);
        if (len <= i || (s32)i >= 16) {
            break;
        }
        c = p[i];
        i = i + 1;
        sum = sum + func_8000ECE0(c);
    }
    return sum & 0xFF;
}

struct Hdr_14 {
    u32 flags;
    s32 len;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
};

struct Src_4 {
    Hdr_14 h;
    s32 chunk;
    s32 stride;
    u8 data[1];
};

Hdr_14 *func_800C0350(Src_4 *s)
{
    u8 st[8];
    u16 buf[1024];
    s32 left;
    s32 n;
    Hdr_14 *out;
    u8 *dst;
    u8 *in;
    u16 *bp;

    if (s->chunk > 0x400) {
        return NULL;
    }
    left = s->h.len;
    out = func_800BC908(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 0x1DE);
    if (out == NULL) {
        return NULL;
    }
    *out = s->h;
    out->flags = (out->flags & ~0xF) | 1;
    func_8000ED10(st, 0, 4);
    in = s->data;
    dst = (u8 *)(out + 1);
    while (left > 0) {
        n = s->chunk;
        if (left < n) {
            n = left;
        }
        left -= n;
        if (D_800D5710 != NULL) {
            D_800D5710();
        }
        func_800DBF94(in, buf, n, st);
        in += s->stride;
        bp = buf;
        while (--n != -1) {
            *dst++ = D_800D3604[*bp++ >> 3];
        }
    }
    dst[-1] = 0;
    return out;
}

u32 *func_800C04EC(u32 *param_1)
{
    switch (*param_1 & 0xF) {
    case 1:
        return param_1;
    case 2:
        return NULL;
    case 3:
        return func_800C055C(param_1);
    case 4:
    case 5:
        return func_800C0350(param_1);
    default:
        return NULL;
    }
}

typedef struct {
    u32 flags;
    s32 len;
    s32 f08, f0C, f10, f14;
} Hdr_15;

struct Src_5 {
    Hdr_15 hdr;
    s32 chunk;
    s32 stride;
    u8 data[1];
};

struct Out_6 {
    Hdr_15 hdr;
    u8 text[1];
};

Out_6 *func_800C055C(Src_5 *p)
{
    Out_6 *r;
    u8 *d;
    u8 *src;
    s32 left, cnt;
    u16 *bp, *pb;
    volatile s32 pad[2];
    u16 buf[1024];

    if (p->chunk > 0x400) {
        return 0;
    }
    left = p->hdr.len;
    r = func_800BC908(((left + 3) & ~3) + 0x18, 0, D_800DB4A0, 381);
    src = p->data;
    if (r == 0) {
        return 0;
    }
    d = r->text;
    r->hdr = p->hdr;
    r->hdr.flags = (r->hdr.flags & ~0xF) | 1;
    if (left > 0) {
        bp = buf;
        do {
        cnt = p->chunk;
        if (left < cnt) {
            cnt = left;
        }
        left -= cnt;
        func_800DBE94(src, bp, cnt);
        src += p->stride;
        pb = bp;
        while (--cnt != -1) {
            *d++ = D_800D3604[*pb++ >> 3];
        }
        } while (left > 0);
    }
    d[-1] = 0;
    return r;
}

typedef struct Hdr_16 {
    struct Hdr_16 *next;
    u8 pad4;
    u8 b5;
    u8 pad6;
    u8 b7;
    u8 pad8[8];
    u16 size;
} Hdr_16;

s32 func_800C06D0(ResFile *arc, u32 **ids, char *name)
{
    s32 *base;
    s32 *src;
    s32 *h;
    s32 *tbl;
    u32 k, j;

    if (name != NULL) {
        D_8010537C = name;
    } else {
        D_8010537C = D_800D5740;
    }
    k = 0x30000;
    if (D_800D5870 != NULL) {
        base = D_800D5870;
    } else {
        base = func_800BD730(k, 0, D_800DB4D0, 0x124);
    }
    if (base == NULL) {
        sprintf(D_800EE380, D_800DB4DC);
        func_8000ECC4_2();
        return -1;
    }
    D_80105378_2 = (s32 *)((u8 *)base + k - 0x1000);
    D_80105374 = 0;
    tbl = func_800BC908(0x2800, 0, D_800DB4D0, 0x132);
    D_80105370 = tbl;
    for (k = 0; k < 40; k++) {
        *tbl++ = -1;
        for (j = 1; j < 64; j++) {
            *tbl++ = 0x80000000;
        }
    }
    D_80105380[0] = base;
    for (k = 1; k < 16; k++) {
        if (*ids == NULL) {
            break;
        }
        h = func_800BF79C(arc, **ids);
        if (h == NULL) {
            func_800BCB04(D_80105370);
            D_80105370 = NULL;
            func_800BCB04(D_80105380[0]);
            D_80105380[0] = NULL;
            sprintf(D_800EE380, D_800DB504);
            func_8000ECC4_2();
            return -1;
        }
        j = ((Hdr_16 *)h)->size;
        func_800C1A6C(h);
        src = h;
        j += 0x10;
        j += 0xF;
        j >>= 2;
        j >>= 1;
        j >>= 1;
        j--;
        D_80105380[k] = base;
        while (j != -1) {
            base[0] = src[0];
            base[1] = src[1];
            base[2] = src[2];
            base[3] = src[3];
            base += 4;
            src += 4;
            j--;
        }
        func_800BFEA4(arc->tbl + **ids);
        ids++;
    }
    for (; k < 16; k++) {
        D_80105380[k] = NULL;
    }
    func_800BCB04(D_80105370);
    src = D_80105378_2 - D_80105374 * 0x400;
    k = D_80105374 << 8;
    k |= 0xFF;
    D_80105370 = NULL;
    while (k != -1) {
        base[0] = src[0];
        base[1] = src[1];
        base[2] = src[2];
        base[3] = src[3];
        base += 4;
        src += 4;
        k--;
    }
    k = (u8 *)base - (u8 *)D_80105380[0];
    if (D_800D5870 != NULL) {
        base = func_800BC908(k, 0, D_800DB4D0, 0x1A0);
        if (base == NULL) {
            func_8000ECB8(1);
        }
        memcpy(base, D_800D5870, k);
    } else {
        base = func_800BD798(D_80105380[0], k, 0, D_800DB4D0, 0x1A9);
    }
    if (base == NULL) {
        sprintf(D_800EE380, D_800DB520);
        func_8000ECC4_2();
        return -1;
    }
    D_80105378_2 = (s32 *)((u8 *)base + k - 0x1000);
    k = (u8 *)base - (u8 *)D_80105380[0];
    for (j = 0; j < 16; j++) {
        h = D_80105380[j];
        if (h == NULL) {
            break;
        }
        D_80105380[j] = (s32 *)((u8 *)h + k);
        if (((Hdr_16 *)h)->b5 == 0) {
            ((Hdr_16 *)h)->next = NULL;
        } else {
            ((Hdr_16 *)h)->next = (Hdr_16 *)((u8 *)h + (((Hdr_16 *)h)->b7 * 8 + 0x10));
        }
    }
    return 0;
}

typedef struct Glyph_3 { s8 a; s8 b; s8 x; s8 y; u8 w; u8 h; s8 adv; u8 page; } Glyph_3;
typedef struct Font_4 { u8 *kern; u8 pad4[2]; u8 first; u8 count; s8 spacing; s8 h9; u8 padA[2]; s8 hC; s8 hD; u8 padE[2]; Glyph_3 g[1]; } Font_4;

u8 *func_800C0ACC(s32 *p, s32 x, s32 y)
{
    u8 *s = (u8 *)(p + 8);
    u32 flags = p[2];
    s32 n;
    u32 color;
    Font_4 *f;
    Glyph_3 *g;
    Ent_110 *e;
    u32 c, i;
    s32 x0, scale, m8, m8b, adv;
    u8 *kp, *kq, *kern;

    color = p[1];
    n = color + color * 2;
    color = n + 1;
    if (flags & 0x20000000) {
        color = n;
    } else if (flags & 0x40000000) {
        color = n + 2;
    }
    x += p[4];
    y += p[5];
    color = D_8010537C_2[color];
    f = D_80105380[p[0]];
    x0 = x;
    if (D_800D575C != 0x10000) {
        scale = 1;
        if (D_800D576C != 0)
            scale = D_800D575C >> 16;
        c = *s;
        if (c != 0) {
            m8 = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8 != 0) {
                        kp = (u8 *)(D_80105378 - (g->page << 12));
                        D_80105368((s32)kp + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = D_8010536C;
                        D_8010536C = e + 1;
                        e->next = D_801054E0[g->page];
                        D_801054E0[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * scale;
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = D_8010536C;
                            D_8010536C = e + 1;
                            e->next = D_801054E0[g->page];
                            D_801054E0[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = y + (g->y + 1) * scale;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kp = f->kern;
                    if (kp != 0) {
                        do { do { kern = kp + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kp += *kern * 2;
                        i = *s;
                        while (*kp != 0) {
                            if (*kp == i) {
                                adv += (s8)kp[1];
                                break;
                            }
                            kp += 2;
                        }
                        }
                    }
                    do { x += (adv * D_800D5758) >> 16; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    } else {
        c = *s;
        if (c != 0) {
            m8b = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8b != 0) {
                        kq = (u8 *)(D_80105378 - (g->page << 12));
                        D_80105368((s32)kq + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = D_8010536C;
                        D_8010536C = e + 1;
                        e->next = D_801054E0[g->page];
                        D_801054E0[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * (D_800D575C >> 16);
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = D_8010536C;
                            D_8010536C = e + 1;
                            e->next = D_801054E0[g->page];
                            D_801054E0[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = (s8)((u8 *)g)[3] + y + 1;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kq = f->kern;
                    if (kq != 0) {
                        do { do { kern = kq + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kq += *kern * 2;
                        i = *s;
                        while (*kq != 0) {
                            if (*kq == i) {
                                adv += (s8)kq[1];
                                break;
                            }
                            kq += 2;
                        }
                        }
                    }
                    do { x += adv; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    }
    return s;
}

typedef struct {
    u8 g0;
    u8 g1;
    s8 xoff;
    u8 g3;
    u8 adv;
    u8 g5;
    s8 kadj;
    u8 g7;
} Glyph_4;

typedef struct {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    u8 p9[7];
    Glyph_4 gl[1];
} Font_5;

struct Obj_83 {
    s32 font;
    s32 w4;
    Glyph_4 * volatile cur;
    s32 wc;
    s32 off;
    s32 w14;
    s32 w18;
    s32 w1c;
    u8 text[1];
};

s32 func_800C1118(Obj_83 *p, s32 x, s32 unused, s32 n)
{
    Glyph_4 *g;
    u8 *t;
    Font_5 *f;
    u8 *kt;
    u8 *pr;
    s32 start;
    u32 idx;
    s32 c;
    s32 k;
    s32 ch;
    u8 adv;

    g = p->cur;
    t = p->text;
    goto setup;

ret:
    adv = g->adv;
    return ((x + g->xoff + (adv >> 1)) << 16) | adv;

setup:
    x = x + p->off;
    start = x;
    c = *t;
    f = D_80105380[p->font];
    if (c == 0) {
        goto fail;
    }
    do {
        idx = c - f->base;
        if (idx >= f->count) {
            goto fail;
        }
        t++;
        n--;
        g = &f->gl[idx];
        if (n == -1) {
            goto ret;
        }
        if (c == 10) {
            x = start;
        } else {
            x = x + (g->kadj + f->adj);
            kt = f->kern;
            if (kt != 0 && kt[idx] != 0) {
                kt = kt + kt[idx] * 2;
                idx = *t;
                while ((k = *kt) != 0) {
                    if (k == idx) {
                        x = x + (s8)kt[1];
                        break;
                    }
                    kt = kt + 2;
                }
            }
        }
        c = *t;
    } while (c != 0);
fail:
    return -1;
}

typedef struct {
    u8 g0[6];
    s8 kadj;
    u8 g7;
} Glyph_5;

struct Font_6 {
    u8 *kern;
    u8 p4[2];
    u8 base;
    u8 count;
    s8 adj;
    s8 f9;
    u8 pa[2];
    s8 fc;
    s8 fd;
    s8 fe;
    u8 pf;
    Glyph_5 gl[1];
};

u8 *func_800C1228(u8 *s, Font_6 *f, u32 *outMax, s32 *outW)
{
    u8 *t = s;
    s32 w;
    s8 flag = 0;
    u32 mx = 0;
    u32 cur;
    u32 c;
    u32 lo;
    u32 hi;
    s32 adj;
    u8 *kt;
    Glyph_5 *gl;

    *outW = 0;
    *outMax = 0;
    w = f->fe;
    lo = f->base;
    hi = f->count;
    adj = f->adj;
    gl = f->gl;
    while (*t != 0) {
        cur = 0;
        c = *t;
        while (c != 0) {
            t++;
            if (c == 10) {
                flag = 1;
                w += f->fc + f->fd + f->f9;
                break;
            }
            if (c != 0x20) {
                flag = 0;
            }
            c -= lo;
            if ((s32)c < 0 || c >= hi) {
                break;
            }
            cur += gl[c].kadj + adj;
            kt = f->kern;
            if (kt != NULL && kt[c] != 0) {
                kt += kt[c] * 2;
                while (*kt != 0) {
                    if (*kt == *s) {
                        cur += (s8)kt[1];
                        break;
                    }
                    kt += 2;
                }
            }
            c = *t;
        }
        if (mx < cur) {
            mx = cur;
        }
    }
    if (mx != 0) {
        mx -= adj;
    }
    if (flag) {
        w -= f->fc + f->fd + f->f9;
    }
    *outMax = mx;
    *outW = w;
    return t;
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_800C1394(Gfx *gdl)
{
    u32 i;

    gdl = func_80048370(gdl);
    PKT(0xE2001E01, 1);
    PKT(0xE200001C, 0x504240);
    PKT(0xFC119623, 0xFF2FFFFF);
    if (D_800D5758 == 0x10000 && D_800D575C == 0x10000) {
        gdl = func_800C1838(gdl, D_80105440);
        gdl = func_800C1838(gdl, D_801054E0);
    } else {
        gdl = func_800C14F4(gdl, D_80105440);
        gdl = func_800C14F4(gdl, D_801054E0);
    }
    PKT(0xE7000000, 0);
    D_8010536C = D_80101368;
    PKT(0xE2001E01, 0);
    D_80101360 = D_80101368 + 0x3FE0;
    for (i = 0; i < 0x28; i++) {
        D_80105440[i] = 0;
        D_801054E0[i] = 0;
    }
    return gdl;
}

Gfx *func_800C14F4(Gfx *gdl, Spr **list)
{
    u32 i;
    Spr *s;
    s32 sy;

    for (i = 0; i < 40; i++) {
        if (list[i] != NULL) {
            gDPLoadTextureBlock(gdl++, D_80105378 - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            sy = D_800D575C;
            s = list[i];
            do {
                if ((u16)(s->x + 63) < 703) {
                    gDPSetPrimColor(gdl++, 0, 0, s->r, s->g, s->b, s->a);
                    if (D_800D576C == 0) {
                        gSPTextureRectangle(gdl++,
                            (s->x + (((0x10000 - D_800D5758) * s->w) >> 17)) << 2,
                            (s->y + (((0x10000 - sy) * s->h) >> 17)) << 2,
                            (s->x + (((D_800D5758 + 0x10000) * s->w) >> 17)) << 2,
                            (s->y + (((sy + 0x10000) * s->h) >> 17)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, D_800D5760, D_800D5764);
                    } else {
                        gSPTextureRectangle(gdl++,
                            s->x << 2,
                            s->y << 2,
                            (s->x + ((D_800D5758 * s->w) >> 16)) << 2,
                            (s->y + ((sy * s->h) >> 16)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, D_800D5760, D_800D5764);
                    }
                }
                s = s->next;
            } while (s != NULL);
        }
    }
    return gdl;
}

#define gDPWord_2(pkt, a, b)     {         Gfx *_g = (Gfx *)(pkt);         _g->words.w0 = (a);         _g->words.w1 = (b);     }

Gfx *func_800C1838(Gfx *gdl, Spr **lists)
{
    s32 base;
    u32 i;
    Spr *n;

    base = D_80105378;
    for (i = 0; i < 40; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, base - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            n = lists[i];
            do {
                if (n->x >= -63 && n->x < 640) {
                    gDPSetPrimColor(gdl++, 0, 0, n->r, n->g, n->b, n->a);
                    { s32 x = n->x;
                    gDPWord_2(gdl++, _SHIFTL(0xE4, 24, 8) | _SHIFTL((n->w + x) << 2, 12, 12) | _SHIFTL((n->h + n->y) << 2, 0, 12),
                            _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(n->x << 2, 12, 12) | _SHIFTL(n->y << 2, 0, 12)); }
                    gDPWord_2(gdl++, _SHIFTL(0xE1, 24, 8), _SHIFTL(n->s << 5, 16, 16) | _SHIFTL(n->t << 5, 0, 16));
                    gDPWord_2(gdl++, _SHIFTL(0xF1, 24, 8), _SHIFTL(1 << 10, 16, 16) | _SHIFTL(1 << 10, 0, 16));
                }
                n = n->next;
            } while (n != NULL);
        }
    }
    return gdl;
}

s32 func_800C1A6C(u8 *hdr)
{
    u8 *e;
    u32 i;
    u32 j;
    u32 h, w;
    u8 *src;
    u8 *dst;
    u8 *tv;

    e = hdr + 0x10;
    for (i = 0; i < hdr[7]; i++) {
        {
            j = e[5] >> 8;
            D_80101358 = e[4] + 1;
            D_8010135C = e[5] + 1;
            while (j < 0x28) {
                if (func_800C1C20(D_80105370_2 + (j << 8)) != 0) break;
                j++;
            }
            if (j == 0x28) {
                return -1;
            }
            if (D_80105374 < j) {
                D_80105374 = j;
            }
            h = e[5];
            w = e[4];
            src = e + *(u16 *)e;
            dst = (u8 *)(D_80105378 - (j << 12));
            dst = dst + D_80101350 + D_80101354 * 0x40;
            while (--h != (u32)-1) {
                while (--w != (u32)-1) {
                    *dst++ = *src++;
                }
                w = e[4];
                tv = dst + 0x40;
                dst = tv - w;
            }
            e[7] = j;
            e[0] = D_80101350;
            e[1] = D_80101354;
            e += 8;
        }
    }
    return 0;
}

s32 func_800C1C20(u32 *map)
{
    u32 w = (D_80101358_2 + 1) >> 1;
    s32 h = D_8010135C;
    s32 y = 1;
    s32 xmax = 0x20 - w;
    s32 ymax = 0x40 - h;
    u32 mask = D_800D5770[w - 1];
    u32 bits0 = D_800D57F0[w - 1];
    u32 acc;
    u32 bits;
    u32 *pp;
    s32 i;
    s32 x;

    if (y < ymax) {
        do {
            acc = 0;
            pp = map + h;
            for (i = h; i != 0; i--) {
                pp--;
                acc |= *pp;
                if ((acc & mask) == mask) {
                    break;
                }
            }
            if (i != 0) {
                y += i;
                map += i;
                continue;
            }
            acc = ~acc;
            bits = bits0;
            x = 0;
            if (xmax > 0) {
                do {
                    if ((acc & bits) == bits) {
                        goto found;
                    }
                    x++;
                    bits >>= 1;
                } while (x < xmax);
            }
            y++;
            map++;
        } while (y < ymax);
    }
    return 0;

found:
    while (--h != -1) {
        *map++ |= bits;
    }
    D_80101350 = x << 1;
    D_80101354 = y;
    return 1;
}

void func_800C1D54(s32 a, s32 b, s32 c, s32 d)
{
    D_800D5758 = a;
    D_800D575C = b;
    D_800D5768 = c;
    D_800D576C = d;
    D_800D5760 = 0x4000000 / a;
    D_800D5764 = 0x4000000 / b;
}

u32 func_800C1DEC(void) {
    u32 i;
    if (D_80105370 != 0) {
        func_800BCB04(D_80105370);
        D_80105370 = 0;
    }
    if (D_80105380[0] != 0) {
        func_800BCB04(D_80105380[0]);
        D_80105380[0] = 0;
    }
    for (i = 1; i < 16; i++) {
        D_80105380[i] = 0;
    }
    return 0;
}

s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5)
{
    s32 t;

    if (param_2 == 0) {
        param_2 = 0x40;
    }
    t = (param_2 + 0xF) & -0x10;
    if (param_1 == 0) {
        param_1 = func_800BD730(t + 0x20, 4, (s32)D_800DB4D0, 0x22A);
        if (param_1 == 0) {
            return 0;
        }
    }
    if (param_3 != 0) {
        param_1[0] = param_3;
    }
    if (param_4 != 0) {
        param_1[1] = param_4;
    }
    if (param_5 != 0) {
        param_1[2] = param_5;
    }
    func_800C1F04((u8 *)param_1);
    return param_1;
}

u8 *func_800C1F04(u8 *p)
{
    if (p != 0) {
        p[0x20] = 0;
        *(s32 *)(p + 0x10) = 0;
        *(s32 *)(p + 0x14) = 0;
        *(s32 *)(p + 0x18) = 0;
        *(s32 *)(p + 0x1C) = 0;
    }
    return p;
}

typedef struct { u8 pad[0xE]; s8 asc; } Font_7;

struct Text {
    s32 font;
    s32 f4;
    u32 pad8 : 24;
    u32 valign : 4;
    u32 halign : 4;
    s32 fC;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    char text[1];
};

char *func_800C1F2C(Text *p, char *s)
{
    Font_7 *font;

    if (p != NULL) {
        func_8000ED30(p->text, s);
        font = D_80105380[p->font];
        func_800C1228(p->text, font, &p->w, &p->h);
        if (p->halign == 1) {
            p->x = -((p->w + 1) / 2);
        } else if (p->halign == 2) {
            p->x = -p->w;
        } else {
            p->x = 0;
        }
        if (p->valign == 1) {
            p->y = 0;
        } else if (p->valign == 0) {
            p->y = font->asc - 1;
        } else if (p->valign == 2) {
            p->y = font->asc - 1 - (p->h + 1) / 2;
        } else if (p->valign == 3) {
            p->y = font->asc - 1 - p->h;
        }
    }
    return s;
}

void func_800C204C(s32 arg0)
{
}

void func_800C2054(char *dst, char *src)
{
    do {
        *dst++ = *src++;
        *dst++ = '.';
        *dst++ = ' ';
        while (*src != 0) {
            if (*src == ' ' || *src == '.') {
                break;
            }
            src++;
        }
        while (*src != 0) {
            if (*src != ' ' && *src != '.') {
                break;
            }
            src++;
        }
    } while (*src != 0);
    dst[-1] = 0;
}

void func_800C20F0(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x61) < 0x1A) {
            *s = c - 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

void func_800C2124(u8 *s) {
    u8 *p = s;
    u8 c;

    if (s[0] != 0) {
        do {
            c = *p;
            if ((u32)(c - 0x61) < 0x1A) {
                *p = c - 0x20;
            }
            p++;
        } while (*p != 0);
    }
    if (s[0] == 0x4D) {
        if (s[1] == 0x43) {
            s[1] = 0x63;
        }
    }
    if (s[0] == 0x44 && s[1] == 0x45 && s[2] == 0x43 && s[3] == 0x4C) {
        s[1] = 0x65;
    }
}

void func_800C21BC(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x41) < 0x1A) {
            *s = c + 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

void func_800C21F0(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x61) < 0x1A) {
            *s = c + 0x20;
        }
        s = s + 1;
        c = *s;
    }
}

s32 func_800C2224(void) {
    u32 i = 0;
    s32 *a = D_801054E0;
    s32 *b = D_80105440;
    D_8010536C = D_80101368;
    D_80101360 = D_80101368 + 0x3FE0;
    do {
        *b = 0;
        *a = 0;
        a++;
        i++;
        b++;
    } while (i < 0x28);
    return 0;
}
