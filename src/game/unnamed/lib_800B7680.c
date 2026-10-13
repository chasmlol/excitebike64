#include "game.h"

typedef struct Node_38 {
    u32 w0;
    struct Node_38 *next;
    s32 id;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2c;
    volatile s32 flags30;
    s32 f34;
    s32 f38;
    s32 f3c;
    s32 f40;
    u32 f44;
    s32 f48;
    s32 flags4c;
} Node_38;

typedef struct Ent_5 {
    s32 w0;
    struct Ent_5 *next;
    s32 id;
    s32 w3[4];
    s32 cur;
    s32 val;
    s32 w9[9];
    s32 res;
} Ent_5;

typedef struct {
    u8 pad[0x2C];
    void (*fn)(s32, s32);
} Ops;

typedef struct Node {
    s32 a;
    struct Node *next;
    s32 b;
    s32 c;
    s32 id;
    s32 d[5];
    s32 val;
} Node;

extern Node_38 **D_800FA854;
extern Ops *D_800FE860_2;

static inline void copy(u8 *d, u8 *s, u32 len)
{
    for (; len != 0; len--) {
        *d++ = *s++;
    }
}

typedef struct {
    u8 *name;
    s32 w[4];
    s32 cnt;
} Ent_91;

typedef struct {
    void *f0[3];
    s32 (*open)(u8 *);
    void *f10[2];
    void (*close)(s32);
    void (*info)(s32, s32 *);
} Drv;

extern Ent_91 D_800FA860_2[256];
extern Drv *D_800FE860_3;

static inline s32 cmp(u8 *a, u8 *b)
{
    for (;;) {
        if (func_8000ECE0(*a) != func_8000ECE0(*b)) {
            return 1;
        }
        if (*a++ == 0) {
            return 0;
        }
        b++;
    }
}

s32 func_800B7680(u8 *name)
{
    s32 i, iv, loc[4];
    Ent_91 *tab;
    Ent_91 *e;
    u8 *pc;

    if (D_800D33F0 != 0) {
        for (i = 0; i < 0x100; i++) {
            if (D_800FA860_2[i].name != NULL && cmp(D_800FA860_2[i].name, name) == 0) {
                if (D_800FA860_2[i].cnt >= 0) {
                    D_800FA860_2[i].cnt++;
                }
                return i;
            }
        }
        iv = D_800FE860_3->open(name);
        if (iv < 0) {
            return iv;
        }
        if (iv < 0x100) {
            pc = func_800BC908(func_800AEC10(name) + 1, 0, D_800DB160, 0x201);
            tab = D_800FA860_2;
            e = &tab[iv];
            e->name = pc;
            func_8000EF8C(pc, name);
            D_800FE860_3->info(iv, loc);
            e->w[0] = loc[0];
            e->w[1] = loc[1];
            e->w[2] = loc[2];
            e->w[3] = loc[3];
            return iv;
        }
        D_800FE860_3->close(iv);
    }
    return -1;
}

s32 func_800B781C(s32 arg0)
{
    s32 idx;
    s32 buf[4];
    u8 *base;
    u8 *fp;
    s32 *p;

    if (D_800D33F0 != 0) {
        idx = (*(s32 (**)(void))(D_800FE860_4 + 0x10))();
        if (idx < 0) {
            return idx;
        }
        if (idx < 0x100) {
            base = D_800FA860;
            p = (s32 *)(base + idx * 0x18);
            fp = D_800FE860_4;
            p[0] = 0;
            p[5] = 1;
            (*(void (**)(s32, s32 *))(fp + 0x1C))(idx, buf);
            p[1] = buf[0];
            p[2] = buf[1];
            p[3] = buf[2];
            p[4] = buf[3];
            return idx;
        }
        (*(void (**)(s32))(D_800FE860_4 + 0x18))(idx);
    }
    return -1;
}

typedef struct {
    void *p;
    u8 unk4[0x10];
    s32 cnt;
} Ent_92;

void func_800B78F0(s32 param_1)
{
    s32 *w;
    Ent_92 *e;

    if (D_800D33F0 != 0) {
        w = (s32 *)D_800FA860;
        e = (Ent_92 *)&w[param_1 * 6];
        if (e->cnt > 0) {
            e->cnt = e->cnt - 1;
        }
        if (e->cnt == 0) {
            if (e->p != 0) {
                func_800BCB04(e->p);
            }
            func_8000ED10((u8 *)e, 0, 0x18);
            ((void (*)(s32))D_800FE860[6])(param_1);
        }
    }
}

typedef struct { s32 f0; s32 f4; s32 f8; s32 fc; s32 f10; s32 f14; } TblEnt_3;
typedef s32 (*MethodFn)(void *);

extern TblEnt_3 D_800FA860_3[];

s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 arg5)
{
    s32 *p;
    s32 *w;
    s32 *q;
    s32 *prev;
    void *obj;
    s32 i;
    s32 cnt;
    s32 quo;
    s32 ret;
    s32 m1;
    s32 t5;
    s32 id;
    MethodFn fn;

    if (D_800D33F0 == 0) {
        return -1;
    }
    if (D_800FA860_3[a0].f4 == 0) {
        return -1;
    }
    i = 0;
    func_8000EE20();
    m1 = -1;
    __asm__ __volatile__("" : "+r"(m1));
    w = D_800FC060;
    for (i = 0; i < 0x80; i++) {
        if (*w == m1) {
            *w = 0;
            break;
        }
        w += 0x14;
    }
    func_8000EE28();
    if (i == 0x80) {
        p = NULL;
    } else {
        p = D_800FC060 + i * 0x14;
    }
    if (p == NULL) {
        return -1;
    }
    func_8000ED10((u8 *)p, 0, 0x50);
    id = D_800D33E4;
    __asm__ __volatile__("" : "+r"(id));
    p[6] = a0;
    p[7] = a1;
    p[9] = a2;
    p[14] = a2 << 16;
    p[10] = a3;
    t5 = a4;
    __asm__ __volatile__("" : "+r"(t5));
    p[12] = 0;
    p[13] = 0;
    __asm__ __volatile__("" : "+r"(t5));
    p[11] = t5;
    obj = D_800FE860;
    p[2] = -1;
    p[4] = id;
    fn = *(MethodFn *)((u8 *)obj + 0x48);
    D_800D33E4 = id + 1;
    ret = fn(obj);
    quo = (a1 << 8) / D_800FA844;
    cnt = 0;
    p[5] = ret;
    p[18] = quo;
    func_8000EE20();
    q = D_800D33E8;
    prev = NULL;
    while (q != NULL) {
        prev = q;
        if (prev[11] <= p[11]) {
            break;
        }
        if (prev[12] & 1) {
            cnt++;
        }
        q = (s32 *)prev[1];
    }
    p[1] = (s32)q;
    if (q != NULL) {
        p[0] = q[0];
        if (q[0] != 0) {
            ((s32 *)q[0])[1] = (s32)p;
        }
        q[0] = (s32)p;
    } else {
        if (prev != NULL) {
            prev[1] = (s32)p;
        }
        p[0] = (s32)prev;
    }
    if (D_800D33E8 == NULL || q == D_800D33E8) {
        D_800D33E8 = p;
        p[0] = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC++;
    }
    func_8000EE28();
    return p[4];
}

typedef struct {
    s32 w0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 w10;
    s32 w14;
} Ent24_3;

typedef struct {
    u8 pad[0x28];
    void (*f28)(s32, s32);
    void (*f2c)(s32, s32);
    void (*f30)(s32, s32);
    void (*f34)(s32, s32);
    void (*f38)(s32, u32);
    s32 (*f3c)(s32);
    void (*f40)(s32, s32);
    s32 (*f44)(s32);
    s32 (*f48)(void);
} SndApi;

extern SndApi *D_800FE860_5;
extern Ent24_3 D_800FA860_4[];

void func_800B7BE0(void)
{
    Node_38 *n;
    s32 i;
    s32 now;
    s32 dt;
    s32 cnt;
    s32 v;
    u32 pos;
    u32 cur;

    if (D_800D33F0 != 0) {
        now = D_800FE860_5->f48();
        n = D_800D33E8;
        D_800D33F8 = 0;
        while (n != NULL) {
            i = now - n->f14;
            n->f14 = now;
            D_800D33F8++;
            if ((n->flags30 & 1) && n->id == -1) {
                v = n->flags30 & 2;
                pos = n->f44 + i * n->f48;
                n->f44 = pos;
                if (v) {
                    s32 len;
                    len = (D_800FA860_4[n->f18].fc - D_800FA860_4[n->f18].f8) << 8;
                    while (n->f44 >= (u32)(D_800FA860_4[n->f18].fc << 8) && len != 0) {
                        n->f44 -= len;
                    }
                } else {
                    s32 len;
                    len = (D_800FA860_4[n->f18].fc - D_800FA860_4[n->f18].f8) << 8;
                    if (n->f44 >= (u32)(D_800FA860_4[n->f18].f4 << 8)) {
                        if (D_800FA860_4[n->f18].f8 != D_800FA860_4[n->f18].fc) {
                            n->flags30 |= 2;
                            cur = n->f44;
                            while (cur >= (u32)(D_800FA860_4[n->f18].fc << 8)) {
                                cur -= len;
                                n->f44 = cur;
                            }
                        } else {
                            n->flags4c |= 1;
                        }
                    }
                }
            }
            n = n->next;
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->f34 != 0) {
                n->f38 += n->f34;
                v = n->f38 / 0x10000;
                n->f24 = v;
                if (v < 0) {
                    n->f24 = 0;
                    n->flags4c |= 1;
                } else {
                    if (v >= 0x80) {
                        n->f24 = 0x7F;
                    }
                    if (n->id != -1) {
                        D_800FE860_5->f30(n->id, n->f24);
                    }
                }
            }
        }

        for (i = 0; i < D_800FA840; i++) {
            n = D_800FA854[i];
            if (n != NULL) {
                v = D_800FE860_5->f44(i);
                n->flags30 = v;
                if (!(v & 1)) {
                    n->flags4c |= 1;
                }
            }
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->flags4c & 1) {
                if (n->id != -1) {
                    D_800FE860_5->f40(n->id, 0);
                    D_800FA854[n->id] = NULL;
                    n->id = -1;
                }
                n->flags4c |= 2;
            }
        }

        i = 0;
        for (n = D_800D33E8; n != NULL; n = n->next) {
            n->f0c = 0;
            if (n->flags4c & 1) {
            } else if (n->flags4c & 2) {
            } else if (i < D_800FA840) {
                n->f0c = 1;
                i++;
            }
        }

        for (i = 0; i < D_800FA840; i++) {
            if (D_800FA854[i] != NULL && D_800FA854[i]->f0c == 0) {
                D_800FA854[i]->f44 = D_800FE860_5->f3c(i) << 8;
                D_800FA854[i]->id = -1;
                D_800FA854[i] = NULL;
                D_800FE860_5->f40(i, 0);
            }
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->f0c != 0 && n->id == -1) {
                for (i = 0; i < D_800FA840; i++) {
                    if (D_800FA854[i] == NULL) {
                        break;
                    }
                }
                if (i != D_800FA840) {
                    D_800FA854[i] = n;
                    n->id = i;
                    n->flags30 = 1;
                    D_800FE860_5->f28(i, n->f18);
                    D_800FE860_5->f2c(i, n->f1c);
                    D_800FE860_5->f30(i, n->f24);
                    D_800FE860_5->f34(i, n->f28);
                    D_800FE860_5->f38(i, n->f44 >> 8);
                    D_800FE860_5->f40(i, n->flags30);
                }
            }
        }
    }
}

void func_800B8164(s32 param_1, s32 param_2)
{
    s32 *p;
    s32 *q;
    s32 *n;
    s32 cnt;

    if (D_800D33F0 == 0) {
        goto done;
    }
    func_8000EE20();
    p = D_800D33E8;
    if (p == 0) {
        goto nf;
    }
loop:
    if (p[4] == param_1) {
        goto found;
    }
    p = (s32 *)p[1];
    if (p != 0) {
        goto loop;
    }
nf:
    func_8000EE28();
    p = 0;
join:
    if (p == 0) {
        goto done;
    }
    p[0xb] = param_2;
    func_8000EE20();
    if (p[0] != 0) {
        ((s32 *)p[0])[1] = p[1];
    }
    if (p[1] != 0) {
        ((s32 *)p[1])[0] = p[0];
    }
    if (p == D_800D33E8) {
        D_800D33E8 = (s32 *)p[1];
    }
    cnt = 0;
    func_8000EE28();
    func_8000EE20();
    q = 0;
    n = D_800D33E8;
    while (n != 0) {
        q = n;
        if (q[0xb] <= p[0xb]) {
            break;
        }
        if ((q[0xc] & 1) != 0) {
            cnt = cnt + 1;
        }
        n = (s32 *)q[1];
    }
    p[1] = (s32)n;
    if (n == 0) {
        goto nullins;
    }
    p[0] = n[0];
    if (n[0] != 0) {
        ((s32 *)n[0])[1] = (s32)p;
    }
    n[0] = (s32)p;
    goto tail;
found:
    func_8000EE28();
    goto join;
nullins:
    if (q != 0) {
        q[1] = (s32)p;
    }
    p[0] = (s32)q;
tail:
    if (D_800D33E8 == 0 || n == D_800D33E8) {
        D_800D33E8 = p;
        p[0] = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC = D_800D33EC + 1;
    }
    func_8000EE28();
done:
    ;
}

void func_800B82FC(s32 *param_1)
{
    s32 *piVar1;
    s32 *piVar2;
    s32 iVar3;
    s32 *iVar4;
    s32 *pv;

    func_8000EE20();
    piVar1 = (s32 *)D_800D33E0;
    if (piVar1 == param_1) {
        D_800D33E0 = *param_1;
    } else {
        piVar2 = piVar1;
        while (piVar2 != NULL) {
            if (*piVar2 == (s32)param_1) {
                *piVar2 = *param_1;
                break;
            }
            piVar2 = (s32 *)*piVar2;
        }
    }
    func_8000EE28();
    if (param_1[1] != -1) {
        iVar3 = param_1[1];
        if (D_800D33F0 != 0) {
            func_8000EE20();
            for (iVar4 = (s32 *)D_800D33E8; iVar4 != NULL; iVar4 = (s32 *)iVar4[1]) {
                if (iVar4[4] == iVar3) {
                    func_8000EE28();
                    pv = iVar4;
                    goto LAB_800b83c8;
                }
            }
            func_8000EE28();
            pv = NULL;
LAB_800b83c8:
            if (pv != NULL) {
                pv[0x13] |= 1;
            }
        }
        param_1[1] = -1;
    }
    if (param_1[2] != -1) {
        func_800B78F0(param_1[2]);
        param_1[2] = -1;
    }
    if (param_1[3] != -1) {
        func_800B78F0(param_1[3]);
        param_1[3] = -1;
    }
    func_800BCB04(param_1[6]);
    func_800BCB04((s32)param_1);
    return;
}

typedef struct { s32 f0; Blk info; s32 f14; } Slot_15;

typedef struct {
    u8 pad[0x14];
    s32 (*alloc)(u8 *tb, u32 a, s32 b, s32 c, s32 d);
    void (*free)(s32 id);
    void (*info)(s32 id, Blk *out);
} Drv_2;

typedef struct Voice {
    u8 pad0[4];
    struct Voice *next;
    u8 pad8[0x10 - 8];
    s32 key;
    u8 pad14[0x4C - 0x14];
    u32 flags;
} Voice;

struct Chan_6 {
    u8 pad0[4];
    s32 key;
    s32 id8;
    s32 idC;
    u8 b10;
    u8 b11;
    u8 pad12[0x28 - 0x12];
    s32 w28;
    u32 w2C;
    u8 b30;
    u8 b31;
    u8 b32;
    u8 b33;
};

extern Drv_2 *D_800FE860_6;

static inline Voice *find(s32 key)
{
    Voice *v;

    func_8000EE20();
    for (v = D_800D33E8; v != 0; v = v->next) {
        if (v->key == key) {
            func_8000EE28();
            return v;
        }
    }
    func_8000EE28();
    return 0;
}

static inline void mark(s32 key)
{
    Voice *v;

    if (D_800D33F0 != 0) {
        v = find(key);
        if (v != 0) {
            v->flags |= 1;
        }
    }
}

static inline s32 alloc(u8 *tb, u32 a, s32 d)
{
    s32 r;
    Slot_15 *s;
    Blk buf;

    if (D_800D33F0 == 0) {
        return -1;
    }
    r = D_800FE860_6->alloc(tb, a, 0, 0, d);
    if (r < 0) {
        return r;
    }
    if (r >= 0x100) {
        D_800FE860_6->free(r);
        return -1;
    }
    {
        Slot_15 *tb = D_800FA860;
        s = &tb[r];
    }
    s->f0 = 0;
    s->f14 = 1;
    D_800FE860_6->info(r, &buf);
    s->info.w[0] = buf.w[0];
    s->info.w[1] = buf.w[1];
    s->info.w[2] = buf.w[2];
    s->info.w[3] = buf.w[3];
    return r;
}

void func_800B8444(Chan_6 *p, s32 a1, s32 a2, s32 a3, s32 a4)
{
    Voice *v;
    s32 key;

    if (p->key != -1) {
        mark(p->key);
        p->key = -1;
    }
    if (p->id8 != -1) {
        func_800B78F0(p->id8);
        p->id8 = -1;
    }
    if (p->idC != -1) {
        func_800B78F0(p->idC);
        p->idC = -1;
    }
    p->b33 = 1;
    p->w28 = a1;
    p->b30 = a2;
    p->b31 = a3;
    p->b32 = a4;
    p->w2C = (p->w28 * D_800FA850) / D_800FA844;
    p->id8 = alloc(D_800F8840, p->w2C, p->w28);
    p->idC = alloc(D_800F8840, p->w2C, p->w28);
    if (p->id8 == -1 || p->idC == -1) {
        func_8000ECB8(1);
    } else {
        p->b10 = 0;
    }
    *(volatile u8 *)&p->b11 = 0;
}

typedef struct Node_39 {
    struct Node_39 *f00;
    struct Node_39 *next;
    s32 f08;
    s32 f0c;
    s32 id;
    u8 pad[0x38];
    s32 flags;
} Node_39;

static __inline__ Node_39 *find_node(s32 id)
{
    Node_39 *n;

    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == id) {
            func_8000EE28();
            return n;
        }
    }
    func_8000EE28();
    return 0;
}

static __inline__ void copy_bytes(u8 *dst, u8 *src, u32 n)
{
    while (n != 0) {
        *dst++ = *src++;
        n--;
    }
}

static __inline__ u32 ring_read(Ring *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->avail < n) {
fail:
        n = 0;
        goto done;
    }
    if (n == 0) goto fail;
    if (r->size < r->pos + n) {
        k = r->size - r->pos;
        copy_bytes(dst, r->buf + r->pos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->pos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->pos, n);
        r->pos += n;
    }
    r->pos = r->pos % r->size;
    r->avail -= n;
done:
    return n;
}

static __inline__ u32 ring_read2(Ring *r, u8 *dst, u32 n0)
{
    u32 k;
    u32 n = n0;
    if (r->avail < n) goto fail;
    if (n != 0) goto ok;
fail:
    n = 0;
    goto done;
ok:
    if (r->size < r->pos + n) {
        k = r->size - r->pos;
        copy_bytes(dst, r->buf + r->pos, k);
        copy_bytes(dst + k, r->buf, n - k);
        r->pos = n - k;
    } else {
        copy_bytes(dst, r->buf + r->pos, n);
        r->pos += n;
    }
    r->pos = r->pos % r->size;
    r->avail -= n;
done:
    return n;
}

void func_800B8760(Ring *r)
{
    s32 id;
    Node_39 *nd;
    u32 got;

    if (r->slot[0] == -1) {
        return;
    }
    if (r->cur != -1) {
        id = r->cur;
        if (D_800D33F0 != 0) {
            nd = find_node(id);
            if (nd != 0) {
                nd->flags |= 1;
            }
        }
        r->cur = -1;
    }
    r->flag[r->sel] = 0;
    if (r->flag[1 - r->sel] == 0) {
        if ((got = ring_read(r, D_800F8840, r->len)) != 0) {
            func_800B8E70(r->slot[1 - r->sel], D_800F8840, got);
            r->flag[1 - r->sel] = 1;
        }
    }
    if (r->flag[1 - r->sel] != 0) {
        r->cur = func_800B7990(r->slot[1 - r->sel], r->arg28, r->b30, r->b31, r->b32, 0);
    }
    if ((got = ring_read2(r, D_800F8840, r->len)) != 0) {
        func_800B8E70(r->slot[r->sel], D_800F8840, got);
        r->flag[r->sel] = 1;
    }
    r->sel = 1 - r->sel;
}

void func_800B8B0C(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    ((void (*)(void))(*D_800FE860))();
}

typedef struct {
    s32 pad0;
    s32 (*init)(s32, s32, s32, s32, s32 *, s32 *);
    void (*fail)(void);
    s32 pad[17];
    void (*setcb)(void (*)(void));
} Iface;

extern Iface *D_800FE860_7;

s32 func_800B8B34(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6)
{
    s32 iv;
    u8 *ptr;
    s32 v5, v6;

    iv = D_800FE860_7->init(p1, p2, p3, p4, p5, p6);
    if (iv != 0) {
        return iv;
    }
    ptr = func_800BC908(p1 * 4, 0, D_800DB160, 0x19B);
    D_800FA854 = ptr;
    if (ptr == NULL) {
        D_800FE860_7->fail();
        return 1;
    }
    D_800FA854 = ptr;
    D_800FA840 = p1;
    D_800FA848 = p2;
    v6 = *p6;
    D_800FA84C = p4;
    D_800FA844 = v6;
    v5 = *p5;
    D_800D33E8 = 0;
    D_800FA850 = v5;
    func_8000ED10(ptr, 0, p1 * 4);
    func_8000ED10(D_800FA860, 0, 0x1800);
    func_8000ED10(D_800FC060, 0xFF, 0x2800);
    D_800FE860_7->setcb(func_800B9F74);
    D_800D33F0 = 1;
    D_800D33E0 = 0;
    return 0;
}

void func_800B8C8C(void)
{
    ((void (*)(void))(((s32 *)D_800FE860)[8 / 4]))();
    D_800D33F0 = 0;
}

typedef struct {
    s32 a;
    s32 buf[4];
    s32 flag;
} Ent_93;

s32 func_800B8CBC(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 r;
    s32 buf[4];
    Ent_93 *e;
    Ent_93 *base;

    if (D_800D33F0 != 0) {
        r = ((s32 (*)(s32, s32, s32, s32, s32))D_800FE860[5])(p1, p2, p3, p4, p5);
        if (r < 0) {
            return r;
        }
        if (r < 0x100) {
            base = D_800FA860;
            e = base + r;
            e->a = 0;
            e->flag = 1;
            ((void (*)(s32, s32 *))D_800FE860[7])(r, buf);
            e->buf[0] = buf[0];
            e->buf[1] = buf[1];
            e->buf[2] = buf[2];
            e->buf[3] = buf[3];
            return r;
        }
        ((void (*)(s32))D_800FE860[6])(r);
    }
    return -1;
}

typedef struct {
    s32 f0, f4, f8, fc, f10;
    s32 cnt;
} Ent_94;

extern Ent_94 D_800FA860_5[];

void func_800B8D98(void)
{
    s32 i;

    if (D_800D33F0 != 0) {
        i = 0;
        do {
            if (0 < D_800FA860_5[i].cnt) {
                D_800FA860_5[i].cnt = 1;
                func_800B78F0(i);
            }
            i++;
        } while (i < 0x100);
    }
}

void func_800B8E08(s32 arg0, s32 *arg1)
{
    if (D_800D33F0 != 0) {
        ((void (*)(void))(((s32 *)D_800FE860)[0x1C / 4]))();
    }
}

void func_800B8E3C(void)
{
    if (D_800D33F0 != 0) {
        ((void (*)(void))(((s32 *)D_800FE860)[0x20 / 4]))();
    }
}

void func_800B8E70(s32 arg0, u8 *arg1, u32 arg2)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x24)))();
}

typedef struct Snd_5 {
    s32 unk0;
    struct Snd_5 *next;
    u8 unk8[8];
    s32 id;
    u8 unk14[0x4C - 0x14];
    u32 flags;
} Snd_5;

static inline Snd_5 *findSnd(s32 id) {
    Snd_5 *s;

    func_8000EE20();
    for (s = D_800D33E8; s != NULL; s = s->next) {
        if (s->id == id) {
            func_8000EE28();
            return s;
        }
    }
    func_8000EE28();
    return NULL;
}

void func_800B8E98(s32 id) {
    if (D_800D33F0 != 0) {
        Snd_5 *s = findSnd(id);

        if (s != NULL) {
            s->flags |= 1;
        }
    }
}

typedef struct Node_40 {
    u32 f0;
    struct Node_40 *next;
    u8 pad[0x44];
    u32 flags;
} Node_40;

void func_800B8F2C(void)
{
    Node_40 *p;

    func_8000EE20();
    p = D_800D33E8;
    if (p != NULL) {
        do {
            p->flags |= 1;
            p = p->next;
        } while (p != NULL);
    }
    func_8000EE28();
}

void func_800B8F78(void)
{
    Ent_5 *e;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (e = D_800D33E8; e != NULL; e = e->next) {
            if (e->cur != 0) {
                e->val = e->cur;
                e->cur = 0;
                e->res = (e->cur << 8) / D_800FA844;
                if (e->id != -1) {
                    D_800FE860_2->fn(e->id, e->cur);
                }
            }
        }
        func_8000EE28();
    }
}

void func_800B905C(void)
{
    Ent_5 *e;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (e = D_800D33E8; e != NULL; e = e->next) {
            if (e->cur == 0) {
                e->cur = e->val;
                e->res = (e->cur << 8) / D_800FA844;
                if (e->id != -1) {
                    D_800FE860_2->fn(e->id, e->cur);
                }
            }
        }
        func_8000EE28();
    }
}

void func_800B9138(void)
{
    ((void (*)(void))(((s32 *)D_800FE860)[0x68 / 4]))();
}

void func_800B9160(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x6C)))();
}

typedef struct Ent_95 {
    s32 f0;
    struct Ent_95 *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 p14[4];
    s32 f24;
    s32 p28[4];
    s32 f38;
} Ent_95;

void func_800B9188(s32 key, s32 val)
{
    Ent_95 *n;
    Ent_95 *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = n->next) {
            if (n->id == key) {
                func_8000EE28();
                q = n;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q->f24 = val;
            q->f38 = val << 16;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[12])(q->f8, val);
            }
        }
    }
}

typedef struct Ent_96 {
    s32 f0;
    struct Ent_96 *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    s32 f28;
} Ent_96;

void func_800B9244(s32 a, s32 b) {
    Ent_96 *p;
    Ent_96 *q;
    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (p = D_800D33E8; p != 0; p = p->next) {
            if (p->id == a) {
                func_8000EE28();
                q = p;
                goto common;
            }
        }
        func_8000EE28();
        q = 0;
common:
        if (q != 0) {
            q->f28 = b;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[13])(q->f8, b);
            }
        }
    }
}

typedef struct Ent_97 {
    s32 w0;
    struct Ent_97 *next;
    s32 f8;
    s32 w0c;
    s32 key;
    s32 w14;
    s32 w18;
    s32 f1c;
    u8 pad[0x28];
    s32 f48;
} Ent_97;

static inline Ent_97 *find_2(s32 id)
{
    Ent_97 *e;

    func_8000EE20();
    for (e = D_800D33E8; e != NULL; e = e->next) {
        if (e->key == id) {
            func_8000EE28();
            return e;
        }
    }
    func_8000EE28();
    return NULL;
}

void func_800B92F8(s32 key, s32 v)
{
    Ent_97 *e;

    if (D_800D33F0 != 0) {
        e = find_2(key);
        if (e != NULL) {
            e->f1c = v;
            e->f48 = (e->f1c << 8) / D_800FA844;
            if (e->f8 != -1) {
                D_800FE860_2->fn(e->f8, e->f1c);
            }
        }
    }
}

void func_800B93E8(s32 p1, u32 p2)
{
    s32 *n;
    s32 *r;
    u32 v;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = (s32 *)n[1]) {
            if (n[4] == p1) {
                func_8000EE28();
                r = n;
                goto found;
            }
        }
        func_8000EE28();
        r = 0;
    found:
        if (r != 0) {
            v = p2 << 8;
            r[0x11] = v;
            if (r[2] != -1) {
                ((void (*)(s32, s32))D_800FE860[14])(r[2], v >> 8);
            }
        }
    }
}

void func_800B94A0(s32 param_1, s32 param_2, s32 param_3)
{
    s32 *n;
    s32 *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = (s32 *)n[1]) {
            if (n[4] == param_1) {
                func_8000EE28();
                q = n;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q[0xf] = param_3;
            q[0x10] = param_2;
        }
    }
}

void func_800B9548(s32 id, s32 v)
{
    s32 *p;
    s32 *q;
    s32 iv;

    if (D_800D33F0 == 0) {
        return;
    }
    func_8000EE20();
    for (p = (s32 *)D_800D33E8; p != 0; p = (s32 *)p[1]) {
        if (p[4] == id) {
            func_8000EE28();
            q = p;
            goto common;
        }
    }
    func_8000EE28();
    q = 0;
common:
    if (q != 0) {
        iv = v * D_800FA84C;
        q[14] = q[9] << 16;
        q[13] = (iv * 0x10000) / 1000;
    }
}

typedef struct Node_41 {
    s32 f0;
    struct Node_41 *next;
    s32 f8;
    s32 fc;
    s32 key;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 val;
} Node_41;

s32 func_800B9618(s32 param_1)
{
    Node_41 *n;
    Node_41 *r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != NULL; n = n->next) {
        if (n->key == param_1) {
            func_8000EE28();
            r = n;
            goto found;
        }
    }
    func_8000EE28();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}

typedef struct Node_42 {
    u8 pad0[4];
    struct Node_42 *next;
    u8 pad1[8];
    s32 id;
    u8 pad2[0x14];
    s32 val;
} Node_42;

s32 func_800B96AC(s32 p1)
{
    Node_42 *n;
    Node_42 *q;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == p1) {
            func_8000EE28();
            q = n;
            goto found;
        }
    }
    func_8000EE28();
    q = 0;
found:
    if (q == 0) {
        return -1;
    }
    return q->val;
}

s32 func_800B9740(s32 param_1)
{
    s32 *n;
    s32 *found;
    s32 r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = (s32 *)D_800D33E8; n != 0; n = (s32 *)n[1]) {
        if (n[4] == param_1) {
            func_8000EE28();
            found = n;
            goto check;
        }
    }
    func_8000EE28();
    found = 0;
check:
    r = -1;
    if (found != 0) {
        r = found[7];
        if (r == 0) {
            r = found[8];
        }
    }
    return r;
}

typedef struct Node_43 {
    s32 a;
    struct Node_43 *next;
    s32 b;
    s32 c;
    s32 key;
    s32 d[6];
    s32 val;
} Node_43;

s32 func_800B97E0(s32 param_1)
{
    Node_43 *p;
    Node_43 *r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    p = (Node_43 *)D_800D33E8;
    while (p != NULL) {
        if (p->key == param_1) {
            func_8000EE28();
            r = p;
            goto done;
        }
        p = p->next;
    }
    func_8000EE28();
    r = NULL;
done:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}

typedef struct {
    u8 pad0[0x4C];
    s32 (*fn)(void);
} Vt_2;

extern Vt_2 *D_800FE860_8;

s32 func_800B9874(void) {
    if (D_800D33F0 == 0) {
        return -1;
    }
    return D_800FE860_8->fn();
}

typedef struct Node_44 {
    struct Node_44 *prev;
    struct Node_44 *next;
    s32 w08[2];
    s32 arg;
    s32 w14[10];
    void (*fn)(s32, s32);
    s32 flags40;
    s32 w44[2];
    s32 flags4C;
} Node_44;

void func_800B98B4(void) {
    Node_44 *n;
    Node_44 *nx;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        n = D_800D33E8;
        while (n != 0) {
            nx = n->next;
            if (n->flags4C & 2) {
                if (n->fn != 0 && (n->flags40 & 1)) {
                    n->fn(n->arg, 1);
                }
                func_8000EE20();
                if (n->prev != 0) {
                    n->prev->next = n->next;
                }
                if (n->next != 0) {
                    n->next->prev = n->prev;
                }
                if (n == D_800D33E8) {
                    D_800D33E8 = n->next;
                }
                func_8000EE28();
                n->prev = (Node_44 *)-1;
            }
            n = nx;
        }
        func_8000EE28();
    }
}

void func_800B99B0(s32 a0)
{
    if (D_800D33F0 != 0) {
        D_800D33F4 = a0;
    }
}

s32 func_800B99CC(void)
{
    if (D_800D33F0 == 0) {
        return 0;
    }
    return D_800D33F4;
}

void func_800B99F0(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x54)))();
}

void func_800B9A18(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x5c)))();
}

void func_800B9A40(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x60)))();
}

void func_800B9A68(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x64)))();
}

s32 *func_800B9A90(s32 p1)
{
    s32 *a;
    s32 *b;
    s32 *old;

    a = func_800BC908(0x34, 0, D_800DB160, 0x6AC);
    if (a != NULL) {
        b = func_800BC908(p1, 0, D_800DB160, 0x6AF);
        a[6] = (s32)b;
        if (b != NULL) {
            a[5] = p1;
            a[7] = 0;
            a[8] = 0;
            a[9] = 0;
            a[1] = -1;
            a[2] = -1;
            a[3] = -1;
            func_8000EE20();
            old = D_800D33E0;
            D_800D33E0 = a;
            a[0] = (s32)old;
            func_8000EE28();
            return a;
        }
        func_800BCB04(a);
    }
    return NULL;
}

struct Ring_2 {
    s32 f00[5];
    u32 size;
    u8 *buf;
    s32 f1C;
    u32 pos;
    u32 total;
};

void func_800B9B4C(Ring_2 *r, u32 n, u8 *src)
{
    u32 t;

    func_8000EE20();
    if (r->size < r->pos + n) {
        t = r->size - r->pos;
        copy(r->buf + r->pos, src, t);
        copy(r->buf, src + t, n - t);
        r->pos = n - t;
    } else {
        copy(r->buf + r->pos, src, n);
        r->pos += n;
    }
    r->pos %= r->size;
    r->total += n;
    func_8000EE28();
}

struct Obj_80 {
    u8 pad[0x28];
    s32 a;
    u32 q;
};

void func_800B9C64(Obj_80 *o, s32 a1)
{
    u32 q = (u32)(a1 * D_800FA850) / D_800FA844;
    o->a = a1;
    o->q = q;
}

typedef struct Ent_98 {
    s32 w0;
    struct Ent_98 *next;
    s32 w2;
    s32 w3;
    s32 id;
    s32 w5[4];
    s32 f24;
    s32 w10[4];
    s32 f38;
} Ent_98;

struct Handle {
    s32 w0;
    s32 id;
    u8 pad[0x28];
    u8 b30;
};

typedef struct {
    u8 pad[0x30];
    void (*fn)(s32, s32);
} Ops_2;

extern Ops_2 *D_800FE860_9;

static inline Ent_98 *find_3(s32 id)
{
    Ent_98 *e;

    func_8000EE20();
    for (e = D_800D33E8; e != NULL; e = e->next) {
        if (e->id == id) {
            func_8000EE28();
            return e;
        }
    }
    func_8000EE28();
    return NULL;
}

void func_800B9CA0(Handle *h, s32 v)
{
    Ent_98 *e;
    s32 id;

    h->b30 = v;
    func_8000EE20();
    if (h->id != -1) {
        id = h->id;
        if (D_800D33F0 == 0) {
            goto out;
        }
        e = find_3(id);
        if (e != NULL) {
            e->f24 = v;
            e->f38 = v << 16;
            if (e->w2 != -1) {
                D_800FE860_9->fn(e->w2, v);
            }
        }
    }
out:
    func_8000EE28();
}

void func_800B9D7C(s32 *o, s32 v)
{
    Node *n;
    Node *r;
    s32 t;
    s32 id;

    *(u8 *)((u8 *)o + 0x31) = (u8)v;
    func_8000EE20();
    t = o[1];
    if (t == -1) {
        goto done;
    }
    id = t;
    if (D_800D33F0 == 0) {
        goto done;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == id) {
            func_8000EE28();
            r = n;
            goto common;
        }
    }
    func_8000EE28();
    r = 0;
common:
    if (r != 0) {
        r->val = v;
        if (r->b != -1) {
            ((void (*)(s32, s32))D_800FE860[13])(r->b, v);
        }
    }
done:
    func_8000EE28();
}

void func_800B9E50(s32 *o, s32 v)
{
    Node *n;
    Node *r;
    s32 t;
    s32 id;

    *(u8 *)((u8 *)o + 0x32) = (u8)v;
    func_8000EE20();
    t = o[1];
    if (t == -1) {
        goto done;
    }
    id = t;
    if (D_800D33F0 == 0) {
        goto done;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == id) {
            func_8000EE28();
            r = n;
            goto common;
        }
    }
    func_8000EE28();
    r = 0;
common:
    if (r != 0) {
        r->val = v;
        if (r->b != -1) {
            ((void (*)(s32, s32))D_800FE860[13])(r->b, v);
        }
    }
done:
    func_8000EE28();
}

void func_800B9F24(void)
{
    ((void (*)(s32))(((s32 *)D_800FE860)[0x70 / 4]))(1);
}

void func_800B9F4C(void)
{
    ((void (*)(s32))(*(u32 *)((u8 *)D_800FE860 + 0x70)))(0);
}

void func_800B9F74(void) {
    s32 *p;

    if (D_800D33F0 != 0) {
        if (D_800D33F4 != 0) {
            ((void (*)(void))D_800D33F4)();
        }
        p = D_800D33E0;
        while (p != 0) {
            func_800B8760(p);
            p = (s32 *)*p;
        }
        func_800B7BE0();
    }
}

void func_800B9FE0(u8 *d, u8 *s, s32 n)
{
    while (n != 0) {
        *d++ = *s++;
        n = n - 1;
    }
}

s32 func_800BA008(u8 *a, u8 *b)
{
    s32 x;
    s32 y;
    for (;;) {
        x = func_8000ECE0(*a);
        y = func_8000ECE0(*b);
        if (x != y) {
            return 1;
        }
        if (*a == 0) {
            return 0;
        }
        a++;
        b++;
    }
}

void func_800BA074(s32 *out) {
    s32 i;
    for (i = 0; i < D_800FA840; i++) {
        if (D_800FA854_2[i] != NULL) {
            out[i] = D_800FA854_2[i][4];
        } else {
            out[i] = -1;
        }
    }
    out[D_800FA840] = D_800D33F8;
}

struct Ent_99 { s32 a; u8 pad[0x4C]; };

extern Ent_99 D_800FC060_2[];

Ent_99 *func_800BA0E0(void)
{
    s32 i;
    func_8000EE20();
    for (i = 0; i < 0x80; i++) {
        if (D_800FC060_2[i].a == -1) {
            D_800FC060_2[i].a = 0;
            break;
        }
    }
    func_8000EE28();
    if (i == 0x80) {
        return NULL;
    }
    return &D_800FC060_2[i];
}

void func_800BA160(s32 *p)
{
    *p = -1;
}

void func_800BA16C(Node_45 *n) {
    Node_45 *p;
    Node_45 *q;
    s32 cnt;

    cnt = 0;
    func_8000EE20();
    p = 0;
    q = D_800D33E8;
    if (q != 0) {
        s32 k = n->key;
        do {
            p = q;
            if (k < p->key) {
                if (p->flags & 1) cnt++;
                q = p->next;
            } else {
                break;
            }
        } while (q != 0);
    }
    n->next = q;
    if (q != 0) {
        n->prev = q->prev;
        if (q->prev != 0) q->prev->next = n;
        q->prev = n;
    } else {
        if (p != 0) p->next = n;
        n->prev = p;
    }
    if (D_800D33E8 == 0 || q == D_800D33E8) {
        D_800D33E8 = n;
        n->prev = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC++;
    }
    func_8000EE28();
}

void func_800BA254(Node_46 *n)
{
    func_8000EE20();
    if (n->prev != NULL) {
        n->prev->next = n->next;
    }
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }
    if (n == D_800D33E8) {
        D_800D33E8 = n->next;
    }
    func_8000EE28();
}

typedef struct Node_47 {
    u32 f0;
    struct Node_47 *next;
    u8 pad[0x8];
    s32 key;
} Node_47;

s32 func_800BA2C0(s32 param_1)
{
    Node_47 *p;

    func_8000EE20();
    p = D_800D33E8;
    while (p != NULL) {
        if (p->key == param_1) {
            func_8000EE28();
            return (s32)p;
        }
        p = p->next;
    }
    func_8000EE28();
    return 0;
}

typedef struct Ent_100 {
    s32 w0;
    struct Ent_100 *next;
    s32 w2;
    s32 w3;
    s32 id;
    s32 w5[7];
    s32 flags;
    s32 w13[4];
    u32 val;
} Ent_100;

typedef struct {
    u8 pad[0x3C];
    u32 (*fn)(s32);
} Ops_3;

extern Ops_3 *D_800FE860_10;

static inline Ent_100 *find_4(s32 id)
{
    Ent_100 *e;

    func_8000EE20();
    for (e = D_800D33E8; e != NULL; e = e->next) {
        if (e->id == id) {
            func_8000EE28();
            return e;
        }
    }
    func_8000EE28();
    return NULL;
}

u32 func_800BA330(s32 p)
{
    Ent_100 *e;

    if (D_800D33F0 == 0) {
        return -1;
    }
    e = find_4(p);
    if (!(e->flags & 1)) {
        return -1;
    }
    if (e->w2 == -1) {
        return e->val >> 8;
    }
    return D_800FE860_10->fn(e->w2);
}

typedef struct Ent_101 {
    s32 f0;
    struct Ent_101 *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 f14;
    s32 val;
} Ent_101;

void func_800BA3F4(s32 p1, s32 p2) {
    Ent_101 *e;
    Ent_101 *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (e = D_800D33E8; e != 0; e = e->next) {
            if (e->id == p1) {
                func_8000EE28();
                q = e;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q->val = p2;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[10])(q->f8, p2);
            }
        }
    }
}

void func_800BA4A8(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x58)))();
}

struct Ring_3 {
    s32 f00[5];
    u32 size;
    u8 *buf;
    u32 head;
    s32 f20;
    u32 count;
};

u32 func_800BA4D0(Ring_3 *r, u32 n, u8 *dst)
{
    u32 t;

    if (r->count < n) {
        return 0;
    }
    if (n == 0) {
        return 0;
    }
    if (r->size < r->head + n) {
        t = r->size - r->head;
        copy(dst, r->buf + r->head, t);
        copy(dst + t, r->buf, n - t);
        r->head = n - t;
    } else {
        copy(dst, r->buf + r->head, n);
        r->head += n;
    }
    r->head %= r->size;
    r->count -= n;
    return n;
}

typedef struct {
    s32 a[5];
    s32 f;
} RecA_2;

extern RecA_2 D_800FA860_6[];

void func_800BA5CC(s32 i)
{
    D_800FA860_6[i].f = -1;
}

typedef struct {
    s32 w[5];
    s32 f14;
} Row24;

extern Row24 D_800FA860_7[];

void func_800BA5F0(s32 a0)
{
    D_800FA860_7[a0].f14 = 1;
}

typedef struct Node_48 {
    s32 f0;
    struct Node_48 *next;
    s32 val;
    s32 fc;
    s32 key;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Node_48;

s32 func_800BA614(s32 param_1)
{
    Node_48 *n;
    Node_48 *r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != NULL; n = n->next) {
        if (n->key == param_1) {
            func_8000EE28();
            r = n;
            goto found;
        }
    }
    func_8000EE28();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}
