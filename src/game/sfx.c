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

extern Node_38 **sSfxVoiceSounds;
extern Ops *gSfxDriver_2;

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

extern Ent_91 sSfxSamples_2[256];
extern Drv *gSfxDriver_3;

static inline s32 cmp(u8 *a, u8 *b)
{
    for (;;) {
        if (char_to_upper(*a) != char_to_upper(*b)) {
            return 1;
        }
        if (*a++ == 0) {
            return 0;
        }
        b++;
    }
}

s32 sfx_load_sample_name(u8 *name)
{
    s32 i, iv, loc[4];
    Ent_91 *tab;
    Ent_91 *e;
    u8 *pc;

    if (sSfxInitialized != 0) {
        for (i = 0; i < 0x100; i++) {
            if (sSfxSamples_2[i].name != NULL && cmp(sSfxSamples_2[i].name, name) == 0) {
                if (sSfxSamples_2[i].cnt >= 0) {
                    sSfxSamples_2[i].cnt++;
                }
                return i;
            }
        }
        iv = gSfxDriver_3->open(name);
        if (iv < 0) {
            return iv;
        }
        if (iv < 0x100) {
            pc = mem_alloc_dbg(strlen(name) + 1, 0, sStrSfxFile, 0x201);
            tab = sSfxSamples_2;
            e = &tab[iv];
            e->name = pc;
            str_copy(pc, name);
            gSfxDriver_3->info(iv, loc);
            e->w[0] = loc[0];
            e->w[1] = loc[1];
            e->w[2] = loc[2];
            e->w[3] = loc[3];
            return iv;
        }
        gSfxDriver_3->close(iv);
    }
    return -1;
}

s32 sfx_load_sample_id(s32 arg0)
{
    s32 idx;
    s32 buf[4];
    u8 *base;
    u8 *fp;
    s32 *p;

    if (sSfxInitialized != 0) {
        idx = (*(s32 (**)(void))(gSfxDriver_4 + 0x10))();
        if (idx < 0) {
            return idx;
        }
        if (idx < 0x100) {
            base = sSfxSamples;
            p = (s32 *)(base + idx * 0x18);
            fp = gSfxDriver_4;
            p[0] = 0;
            p[5] = 1;
            (*(void (**)(s32, s32 *))(fp + 0x1C))(idx, buf);
            p[1] = buf[0];
            p[2] = buf[1];
            p[3] = buf[2];
            p[4] = buf[3];
            return idx;
        }
        (*(void (**)(s32))(gSfxDriver_4 + 0x18))(idx);
    }
    return -1;
}

typedef struct {
    void *p;
    u8 unk4[0x10];
    s32 cnt;
} Ent_92;

void sfx_free_sample(s32 param_1)
{
    s32 *w;
    Ent_92 *e;

    if (sSfxInitialized != 0) {
        w = (s32 *)sSfxSamples;
        e = (Ent_92 *)&w[param_1 * 6];
        if (e->cnt > 0) {
            e->cnt = e->cnt - 1;
        }
        if (e->cnt == 0) {
            if (e->p != 0) {
                mem_free(e->p);
            }
            mem_fill((u8 *)e, 0, 0x18);
            ((void (*)(s32))gSfxDriver[6])(param_1);
        }
    }
}

typedef struct { s32 f0; s32 f4; s32 f8; s32 fc; s32 f10; s32 f14; } TblEnt_3;
typedef s32 (*MethodFn)(void *);

extern TblEnt_3 sSfxSamples_3[];

s32 sfx_play(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 arg5)
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

    if (sSfxInitialized == 0) {
        return -1;
    }
    if (sSfxSamples_3[a0].f4 == 0) {
        return -1;
    }
    i = 0;
    critical_enter();
    m1 = -1;
    __asm__ __volatile__("" : "+r"(m1));
    w = sSfxSoundPool;
    for (i = 0; i < 0x80; i++) {
        if (*w == m1) {
            *w = 0;
            break;
        }
        w += 0x14;
    }
    critical_leave();
    if (i == 0x80) {
        p = NULL;
    } else {
        p = sSfxSoundPool + i * 0x14;
    }
    if (p == NULL) {
        return -1;
    }
    mem_fill((u8 *)p, 0, 0x50);
    id = sSfxNextHandle;
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
    obj = gSfxDriver;
    p[2] = -1;
    p[4] = id;
    fn = *(MethodFn *)((u8 *)obj + 0x48);
    sSfxNextHandle = id + 1;
    ret = fn(obj);
    quo = (a1 << 8) / sSfxOutputRate;
    cnt = 0;
    p[5] = ret;
    p[18] = quo;
    critical_enter();
    q = sSfxSounds;
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
    if (sSfxSounds == NULL || q == sSfxSounds) {
        sSfxSounds = p;
        p[0] = 0;
    }
    if (cnt < sSfxNumVoices) {
        D_800D33EC++;
    }
    critical_leave();
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

extern SndApi *gSfxDriver_5;
extern Ent24_3 sSfxSamples_4[];

void sfx_update_voices(void)
{
    Node_38 *n;
    s32 i;
    s32 now;
    s32 dt;
    s32 cnt;
    s32 v;
    u32 pos;
    u32 cur;

    if (sSfxInitialized != 0) {
        now = gSfxDriver_5->f48();
        n = sSfxSounds;
        gSfxActiveCount = 0;
        while (n != NULL) {
            i = now - n->f14;
            n->f14 = now;
            gSfxActiveCount++;
            if ((n->flags30 & 1) && n->id == -1) {
                v = n->flags30 & 2;
                pos = n->f44 + i * n->f48;
                n->f44 = pos;
                if (v) {
                    s32 len;
                    len = (sSfxSamples_4[n->f18].fc - sSfxSamples_4[n->f18].f8) << 8;
                    while (n->f44 >= (u32)(sSfxSamples_4[n->f18].fc << 8) && len != 0) {
                        n->f44 -= len;
                    }
                } else {
                    s32 len;
                    len = (sSfxSamples_4[n->f18].fc - sSfxSamples_4[n->f18].f8) << 8;
                    if (n->f44 >= (u32)(sSfxSamples_4[n->f18].f4 << 8)) {
                        if (sSfxSamples_4[n->f18].f8 != sSfxSamples_4[n->f18].fc) {
                            n->flags30 |= 2;
                            cur = n->f44;
                            while (cur >= (u32)(sSfxSamples_4[n->f18].fc << 8)) {
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

        for (n = sSfxSounds; n != NULL; n = n->next) {
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
                        gSfxDriver_5->f30(n->id, n->f24);
                    }
                }
            }
        }

        for (i = 0; i < sSfxNumVoices; i++) {
            n = sSfxVoiceSounds[i];
            if (n != NULL) {
                v = gSfxDriver_5->f44(i);
                n->flags30 = v;
                if (!(v & 1)) {
                    n->flags4c |= 1;
                }
            }
        }

        for (n = sSfxSounds; n != NULL; n = n->next) {
            if (n->flags4c & 1) {
                if (n->id != -1) {
                    gSfxDriver_5->f40(n->id, 0);
                    sSfxVoiceSounds[n->id] = NULL;
                    n->id = -1;
                }
                n->flags4c |= 2;
            }
        }

        i = 0;
        for (n = sSfxSounds; n != NULL; n = n->next) {
            n->f0c = 0;
            if (n->flags4c & 1) {
            } else if (n->flags4c & 2) {
            } else if (i < sSfxNumVoices) {
                n->f0c = 1;
                i++;
            }
        }

        for (i = 0; i < sSfxNumVoices; i++) {
            if (sSfxVoiceSounds[i] != NULL && sSfxVoiceSounds[i]->f0c == 0) {
                sSfxVoiceSounds[i]->f44 = gSfxDriver_5->f3c(i) << 8;
                sSfxVoiceSounds[i]->id = -1;
                sSfxVoiceSounds[i] = NULL;
                gSfxDriver_5->f40(i, 0);
            }
        }

        for (n = sSfxSounds; n != NULL; n = n->next) {
            if (n->f0c != 0 && n->id == -1) {
                for (i = 0; i < sSfxNumVoices; i++) {
                    if (sSfxVoiceSounds[i] == NULL) {
                        break;
                    }
                }
                if (i != sSfxNumVoices) {
                    sSfxVoiceSounds[i] = n;
                    n->id = i;
                    n->flags30 = 1;
                    gSfxDriver_5->f28(i, n->f18);
                    gSfxDriver_5->f2c(i, n->f1c);
                    gSfxDriver_5->f30(i, n->f24);
                    gSfxDriver_5->f34(i, n->f28);
                    gSfxDriver_5->f38(i, n->f44 >> 8);
                    gSfxDriver_5->f40(i, n->flags30);
                }
            }
        }
    }
}

void sfx_set_priority(s32 param_1, s32 param_2)
{
    s32 *p;
    s32 *q;
    s32 *n;
    s32 cnt;

    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    p = sSfxSounds;
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
    critical_leave();
    p = 0;
join:
    if (p == 0) {
        goto done;
    }
    p[0xb] = param_2;
    critical_enter();
    if (p[0] != 0) {
        ((s32 *)p[0])[1] = p[1];
    }
    if (p[1] != 0) {
        ((s32 *)p[1])[0] = p[0];
    }
    if (p == sSfxSounds) {
        sSfxSounds = (s32 *)p[1];
    }
    cnt = 0;
    critical_leave();
    critical_enter();
    q = 0;
    n = sSfxSounds;
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
    critical_leave();
    goto join;
nullins:
    if (q != 0) {
        q[1] = (s32)p;
    }
    p[0] = (s32)q;
tail:
    if (sSfxSounds == 0 || n == sSfxSounds) {
        sSfxSounds = p;
        p[0] = 0;
    }
    if (cnt < sSfxNumVoices) {
        D_800D33EC = D_800D33EC + 1;
    }
    critical_leave();
done:
    ;
}

void sfx_stream_free(s32 *param_1)
{
    s32 *piVar1;
    s32 *piVar2;
    s32 iVar3;
    s32 *iVar4;
    s32 *pv;

    critical_enter();
    piVar1 = (s32 *)sSfxStreams;
    if (piVar1 == param_1) {
        sSfxStreams = *param_1;
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
    critical_leave();
    if (param_1[1] != -1) {
        iVar3 = param_1[1];
        if (sSfxInitialized != 0) {
            critical_enter();
            for (iVar4 = (s32 *)sSfxSounds; iVar4 != NULL; iVar4 = (s32 *)iVar4[1]) {
                if (iVar4[4] == iVar3) {
                    critical_leave();
                    pv = iVar4;
                    goto LAB_800b83c8;
                }
            }
            critical_leave();
            pv = NULL;
LAB_800b83c8:
            if (pv != NULL) {
                pv[0x13] |= 1;
            }
        }
        param_1[1] = -1;
    }
    if (param_1[2] != -1) {
        sfx_free_sample(param_1[2]);
        param_1[2] = -1;
    }
    if (param_1[3] != -1) {
        sfx_free_sample(param_1[3]);
        param_1[3] = -1;
    }
    mem_free(param_1[6]);
    mem_free((s32)param_1);
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

extern Drv_2 *gSfxDriver_6;

static inline Voice *find(s32 key)
{
    Voice *v;

    critical_enter();
    for (v = sSfxSounds; v != 0; v = v->next) {
        if (v->key == key) {
            critical_leave();
            return v;
        }
    }
    critical_leave();
    return 0;
}

static inline void mark(s32 key)
{
    Voice *v;

    if (sSfxInitialized != 0) {
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

    if (sSfxInitialized == 0) {
        return -1;
    }
    r = gSfxDriver_6->alloc(tb, a, 0, 0, d);
    if (r < 0) {
        return r;
    }
    if (r >= 0x100) {
        gSfxDriver_6->free(r);
        return -1;
    }
    {
        Slot_15 *tb = sSfxSamples;
        s = &tb[r];
    }
    s->f0 = 0;
    s->f14 = 1;
    gSfxDriver_6->info(r, &buf);
    s->info.w[0] = buf.w[0];
    s->info.w[1] = buf.w[1];
    s->info.w[2] = buf.w[2];
    s->info.w[3] = buf.w[3];
    return r;
}

void sfx_stream_start(Chan_6 *p, s32 a1, s32 a2, s32 a3, s32 a4)
{
    Voice *v;
    s32 key;

    if (p->key != -1) {
        mark(p->key);
        p->key = -1;
    }
    if (p->id8 != -1) {
        sfx_free_sample(p->id8);
        p->id8 = -1;
    }
    if (p->idC != -1) {
        sfx_free_sample(p->idC);
        p->idC = -1;
    }
    p->b33 = 1;
    p->w28 = a1;
    p->b30 = a2;
    p->b31 = a3;
    p->b32 = a4;
    p->w2C = (p->w28 * sSfxFrameSamples) / sSfxOutputRate;
    p->id8 = alloc(sSfxStreamBuf, p->w2C, p->w28);
    p->idC = alloc(sSfxStreamBuf, p->w2C, p->w28);
    if (p->id8 == -1 || p->idC == -1) {
        fatal_halt(1);
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

    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->id == id) {
            critical_leave();
            return n;
        }
    }
    critical_leave();
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

void sfx_stream_update(Ring *r)
{
    s32 id;
    Node_39 *nd;
    u32 got;

    if (r->slot[0] == -1) {
        return;
    }
    if (r->cur != -1) {
        id = r->cur;
        if (sSfxInitialized != 0) {
            nd = find_node(id);
            if (nd != 0) {
                nd->flags |= 1;
            }
        }
        r->cur = -1;
    }
    r->flag[r->sel] = 0;
    if (r->flag[1 - r->sel] == 0) {
        if ((got = ring_read(r, sSfxStreamBuf, r->len)) != 0) {
            sfx_write_sample(r->slot[1 - r->sel], sSfxStreamBuf, got);
            r->flag[1 - r->sel] = 1;
        }
    }
    if (r->flag[1 - r->sel] != 0) {
        r->cur = sfx_play(r->slot[1 - r->sel], r->arg28, r->b30, r->b31, r->b32, 0);
    }
    if ((got = ring_read2(r, sSfxStreamBuf, r->len)) != 0) {
        sfx_write_sample(r->slot[r->sel], sSfxStreamBuf, got);
        r->flag[r->sel] = 1;
    }
    r->sel = 1 - r->sel;
}

void sfx_get_driver_caps(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    ((void (*)(void))(*gSfxDriver))();
}

typedef struct {
    s32 pad0;
    s32 (*init)(s32, s32, s32, s32, s32 *, s32 *);
    void (*fail)(void);
    s32 pad[17];
    void (*setcb)(void (*)(void));
} Iface;

extern Iface *gSfxDriver_7;

s32 sfx_init(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6)
{
    s32 iv;
    u8 *ptr;
    s32 v5, v6;

    iv = gSfxDriver_7->init(p1, p2, p3, p4, p5, p6);
    if (iv != 0) {
        return iv;
    }
    ptr = mem_alloc_dbg(p1 * 4, 0, sStrSfxFile, 0x19B);
    sSfxVoiceSounds = ptr;
    if (ptr == NULL) {
        gSfxDriver_7->fail();
        return 1;
    }
    sSfxVoiceSounds = ptr;
    sSfxNumVoices = p1;
    sSfxDriverFlags = p2;
    v6 = *p6;
    sSfxFrameMs = p4;
    sSfxOutputRate = v6;
    v5 = *p5;
    sSfxSounds = 0;
    sSfxFrameSamples = v5;
    mem_fill(ptr, 0, p1 * 4);
    mem_fill(sSfxSamples, 0, 0x1800);
    mem_fill(sSfxSoundPool, 0xFF, 0x2800);
    gSfxDriver_7->setcb(sfx_driver_callback);
    sSfxInitialized = 1;
    sSfxStreams = 0;
    return 0;
}

void sfx_shutdown(void)
{
    ((void (*)(void))(((s32 *)gSfxDriver)[8 / 4]))();
    sSfxInitialized = 0;
}

typedef struct {
    s32 a;
    s32 buf[4];
    s32 flag;
} Ent_93;

s32 sfx_create_sample(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 r;
    s32 buf[4];
    Ent_93 *e;
    Ent_93 *base;

    if (sSfxInitialized != 0) {
        r = ((s32 (*)(s32, s32, s32, s32, s32))gSfxDriver[5])(p1, p2, p3, p4, p5);
        if (r < 0) {
            return r;
        }
        if (r < 0x100) {
            base = sSfxSamples;
            e = base + r;
            e->a = 0;
            e->flag = 1;
            ((void (*)(s32, s32 *))gSfxDriver[7])(r, buf);
            e->buf[0] = buf[0];
            e->buf[1] = buf[1];
            e->buf[2] = buf[2];
            e->buf[3] = buf[3];
            return r;
        }
        ((void (*)(s32))gSfxDriver[6])(r);
    }
    return -1;
}

typedef struct {
    s32 f0, f4, f8, fc, f10;
    s32 cnt;
} Ent_94;

extern Ent_94 sSfxSamples_5[];

void sfx_free_all_samples(void)
{
    s32 i;

    if (sSfxInitialized != 0) {
        i = 0;
        do {
            if (0 < sSfxSamples_5[i].cnt) {
                sSfxSamples_5[i].cnt = 1;
                sfx_free_sample(i);
            }
            i++;
        } while (i < 0x100);
    }
}

void sfx_get_sample_info(s32 arg0, s32 *arg1)
{
    if (sSfxInitialized != 0) {
        ((void (*)(void))(((s32 *)gSfxDriver)[0x1C / 4]))();
    }
}

void sfx_set_sample_info(void)
{
    if (sSfxInitialized != 0) {
        ((void (*)(void))(((s32 *)gSfxDriver)[0x20 / 4]))();
    }
}

void sfx_write_sample(s32 arg0, u8 *arg1, u32 arg2)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x24)))();
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

    critical_enter();
    for (s = sSfxSounds; s != NULL; s = s->next) {
        if (s->id == id) {
            critical_leave();
            return s;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_stop(s32 id) {
    if (sSfxInitialized != 0) {
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

void sfx_stop_all(void)
{
    Node_40 *p;

    critical_enter();
    p = sSfxSounds;
    if (p != NULL) {
        do {
            p->flags |= 1;
            p = p->next;
        } while (p != NULL);
    }
    critical_leave();
}

void sfx_pause_all(void)
{
    Ent_5 *e;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != NULL; e = e->next) {
            if (e->cur != 0) {
                e->val = e->cur;
                e->cur = 0;
                e->res = (e->cur << 8) / sSfxOutputRate;
                if (e->id != -1) {
                    gSfxDriver_2->fn(e->id, e->cur);
                }
            }
        }
        critical_leave();
    }
}

void sfx_resume_all(void)
{
    Ent_5 *e;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != NULL; e = e->next) {
            if (e->cur == 0) {
                e->cur = e->val;
                e->res = (e->cur << 8) / sSfxOutputRate;
                if (e->id != -1) {
                    gSfxDriver_2->fn(e->id, e->cur);
                }
            }
        }
        critical_leave();
    }
}

void func_800B9138(void)
{
    ((void (*)(void))(((s32 *)gSfxDriver)[0x68 / 4]))();
}

void func_800B9160(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x6C)))();
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

void sfx_set_volume(s32 key, s32 val)
{
    Ent_95 *n;
    Ent_95 *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = n->next) {
            if (n->id == key) {
                critical_leave();
                q = n;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q->f24 = val;
            q->f38 = val << 16;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))gSfxDriver[12])(q->f8, val);
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

void sfx_set_pan(s32 a, s32 b) {
    Ent_96 *p;
    Ent_96 *q;
    if (sSfxInitialized != 0) {
        critical_enter();
        for (p = sSfxSounds; p != 0; p = p->next) {
            if (p->id == a) {
                critical_leave();
                q = p;
                goto common;
            }
        }
        critical_leave();
        q = 0;
common:
        if (q != 0) {
            q->f28 = b;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))gSfxDriver[13])(q->f8, b);
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

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->key == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_set_rate(s32 key, s32 v)
{
    Ent_97 *e;

    if (sSfxInitialized != 0) {
        e = find_2(key);
        if (e != NULL) {
            e->f1c = v;
            e->f48 = (e->f1c << 8) / sSfxOutputRate;
            if (e->f8 != -1) {
                gSfxDriver_2->fn(e->f8, e->f1c);
            }
        }
    }
}

void sfx_set_position(s32 p1, u32 p2)
{
    s32 *n;
    s32 *r;
    u32 v;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = (s32 *)n[1]) {
            if (n[4] == p1) {
                critical_leave();
                r = n;
                goto found;
            }
        }
        critical_leave();
        r = 0;
    found:
        if (r != 0) {
            v = p2 << 8;
            r[0x11] = v;
            if (r[2] != -1) {
                ((void (*)(s32, s32))gSfxDriver[14])(r[2], v >> 8);
            }
        }
    }
}

void sfx_set_end_callback(s32 param_1, s32 param_2, s32 param_3)
{
    s32 *n;
    s32 *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (n = sSfxSounds; n != 0; n = (s32 *)n[1]) {
            if (n[4] == param_1) {
                critical_leave();
                q = n;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q[0xf] = param_3;
            q[0x10] = param_2;
        }
    }
}

void sfx_set_fade(s32 id, s32 v)
{
    s32 *p;
    s32 *q;
    s32 iv;

    if (sSfxInitialized == 0) {
        return;
    }
    critical_enter();
    for (p = (s32 *)sSfxSounds; p != 0; p = (s32 *)p[1]) {
        if (p[4] == id) {
            critical_leave();
            q = p;
            goto common;
        }
    }
    critical_leave();
    q = 0;
common:
    if (q != 0) {
        iv = v * sSfxFrameMs;
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

s32 sfx_get_volume(s32 param_1)
{
    Node_41 *n;
    Node_41 *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != NULL; n = n->next) {
        if (n->key == param_1) {
            critical_leave();
            r = n;
            goto found;
        }
    }
    critical_leave();
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

s32 sfx_get_pan(s32 p1)
{
    Node_42 *n;
    Node_42 *q;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->id == p1) {
            critical_leave();
            q = n;
            goto found;
        }
    }
    critical_leave();
    q = 0;
found:
    if (q == 0) {
        return -1;
    }
    return q->val;
}

s32 sfx_get_rate(s32 param_1)
{
    s32 *n;
    s32 *found;
    s32 r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = (s32 *)sSfxSounds; n != 0; n = (s32 *)n[1]) {
        if (n[4] == param_1) {
            critical_leave();
            found = n;
            goto check;
        }
    }
    critical_leave();
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

s32 sfx_get_priority(s32 param_1)
{
    Node_43 *p;
    Node_43 *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    p = (Node_43 *)sSfxSounds;
    while (p != NULL) {
        if (p->key == param_1) {
            critical_leave();
            r = p;
            goto done;
        }
        p = p->next;
    }
    critical_leave();
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

extern Vt_2 *gSfxDriver_8;

s32 sfx_get_frame_count(void) {
    if (sSfxInitialized == 0) {
        return -1;
    }
    return gSfxDriver_8->fn();
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

void sfx_reap_sounds(void) {
    Node_44 *n;
    Node_44 *nx;

    if (sSfxInitialized != 0) {
        critical_enter();
        n = sSfxSounds;
        while (n != 0) {
            nx = n->next;
            if (n->flags4C & 2) {
                if (n->fn != 0 && (n->flags40 & 1)) {
                    n->fn(n->arg, 1);
                }
                critical_enter();
                if (n->prev != 0) {
                    n->prev->next = n->next;
                }
                if (n->next != 0) {
                    n->next->prev = n->prev;
                }
                if (n == sSfxSounds) {
                    sSfxSounds = n->next;
                }
                critical_leave();
                n->prev = (Node_44 *)-1;
            }
            n = nx;
        }
        critical_leave();
    }
}

void sfx_set_frame_callback(s32 a0)
{
    if (sSfxInitialized != 0) {
        sSfxFrameCallback = a0;
    }
}

s32 sfx_get_frame_callback(void)
{
    if (sSfxInitialized == 0) {
        return 0;
    }
    return sSfxFrameCallback;
}

void sfx_echo_init(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x54)))();
}

void sfx_echo_set_level(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x5c)))();
}

void sfx_echo_free(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x60)))();
}

void func_800B9A68(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x64)))();
}

s32 *sfx_stream_create(s32 p1)
{
    s32 *a;
    s32 *b;
    s32 *old;

    a = mem_alloc_dbg(0x34, 0, sStrSfxFile, 0x6AC);
    if (a != NULL) {
        b = mem_alloc_dbg(p1, 0, sStrSfxFile, 0x6AF);
        a[6] = (s32)b;
        if (b != NULL) {
            a[5] = p1;
            a[7] = 0;
            a[8] = 0;
            a[9] = 0;
            a[1] = -1;
            a[2] = -1;
            a[3] = -1;
            critical_enter();
            old = sSfxStreams;
            sSfxStreams = a;
            a[0] = (s32)old;
            critical_leave();
            return a;
        }
        mem_free(a);
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

void sfx_stream_write(Ring_2 *r, u32 n, u8 *src)
{
    u32 t;

    critical_enter();
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
    critical_leave();
}

struct Obj_80 {
    u8 pad[0x28];
    s32 a;
    u32 q;
};

void sfx_stream_set_rate(Obj_80 *o, s32 a1)
{
    u32 q = (u32)(a1 * sSfxFrameSamples) / sSfxOutputRate;
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

extern Ops_2 *gSfxDriver_9;

static inline Ent_98 *find_3(s32 id)
{
    Ent_98 *e;

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->id == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

void sfx_stream_set_volume(Handle *h, s32 v)
{
    Ent_98 *e;
    s32 id;

    h->b30 = v;
    critical_enter();
    if (h->id != -1) {
        id = h->id;
        if (sSfxInitialized == 0) {
            goto out;
        }
        e = find_3(id);
        if (e != NULL) {
            e->f24 = v;
            e->f38 = v << 16;
            if (e->w2 != -1) {
                gSfxDriver_9->fn(e->w2, v);
            }
        }
    }
out:
    critical_leave();
}

void sfx_stream_set_pan(s32 *o, s32 v)
{
    Node *n;
    Node *r;
    s32 t;
    s32 id;

    *(u8 *)((u8 *)o + 0x31) = (u8)v;
    critical_enter();
    t = o[1];
    if (t == -1) {
        goto done;
    }
    id = t;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->id == id) {
            critical_leave();
            r = n;
            goto common;
        }
    }
    critical_leave();
    r = 0;
common:
    if (r != 0) {
        r->val = v;
        if (r->b != -1) {
            ((void (*)(s32, s32))gSfxDriver[13])(r->b, v);
        }
    }
done:
    critical_leave();
}

void sfx_stream_set_priority(s32 *o, s32 v)
{
    Node *n;
    Node *r;
    s32 t;
    s32 id;

    *(u8 *)((u8 *)o + 0x32) = (u8)v;
    critical_enter();
    t = o[1];
    if (t == -1) {
        goto done;
    }
    id = t;
    if (sSfxInitialized == 0) {
        goto done;
    }
    critical_enter();
    for (n = sSfxSounds; n != 0; n = n->next) {
        if (n->id == id) {
            critical_leave();
            r = n;
            goto common;
        }
    }
    critical_leave();
    r = 0;
common:
    if (r != 0) {
        r->val = v;
        if (r->b != -1) {
            ((void (*)(s32, s32))gSfxDriver[13])(r->b, v);
        }
    }
done:
    critical_leave();
}

void sfx_mute(void)
{
    ((void (*)(s32))(((s32 *)gSfxDriver)[0x70 / 4]))(1);
}

void sfx_unmute(void)
{
    ((void (*)(s32))(*(u32 *)((u8 *)gSfxDriver + 0x70)))(0);
}

void sfx_driver_callback(void) {
    s32 *p;

    if (sSfxInitialized != 0) {
        if (sSfxFrameCallback != 0) {
            ((void (*)(void))sSfxFrameCallback)();
        }
        p = sSfxStreams;
        while (p != 0) {
            sfx_stream_update(p);
            p = (s32 *)*p;
        }
        sfx_update_voices();
    }
}

void sfx_copy_bytes(u8 *d, u8 *s, s32 n)
{
    while (n != 0) {
        *d++ = *s++;
        n = n - 1;
    }
}

s32 sfx_str_icmp(u8 *a, u8 *b)
{
    s32 x;
    s32 y;
    for (;;) {
        x = char_to_upper(*a);
        y = char_to_upper(*b);
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

void sfx_get_voice_handles(s32 *out) {
    s32 i;
    for (i = 0; i < sSfxNumVoices; i++) {
        if (sSfxVoiceSounds_2[i] != NULL) {
            out[i] = sSfxVoiceSounds_2[i][4];
        } else {
            out[i] = -1;
        }
    }
    out[sSfxNumVoices] = gSfxActiveCount;
}

struct Ent_99 { s32 a; u8 pad[0x4C]; };

extern Ent_99 sSfxSoundPool_2[];

Ent_99 *sfx_alloc_sound(void)
{
    s32 i;
    critical_enter();
    for (i = 0; i < 0x80; i++) {
        if (sSfxSoundPool_2[i].a == -1) {
            sSfxSoundPool_2[i].a = 0;
            break;
        }
    }
    critical_leave();
    if (i == 0x80) {
        return NULL;
    }
    return &sSfxSoundPool_2[i];
}

void sfx_free_sound(s32 *p)
{
    *p = -1;
}

void sfx_insert_sound(Node_45 *n) {
    Node_45 *p;
    Node_45 *q;
    s32 cnt;

    cnt = 0;
    critical_enter();
    p = 0;
    q = sSfxSounds;
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
    if (sSfxSounds == 0 || q == sSfxSounds) {
        sSfxSounds = n;
        n->prev = 0;
    }
    if (cnt < sSfxNumVoices) {
        D_800D33EC++;
    }
    critical_leave();
}

void sfx_unlink_sound(Node_46 *n)
{
    critical_enter();
    if (n->prev != NULL) {
        n->prev->next = n->next;
    }
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }
    if (n == sSfxSounds) {
        sSfxSounds = n->next;
    }
    critical_leave();
}

typedef struct Node_47 {
    u32 f0;
    struct Node_47 *next;
    u8 pad[0x8];
    s32 key;
} Node_47;

s32 sfx_find_sound(s32 param_1)
{
    Node_47 *p;

    critical_enter();
    p = sSfxSounds;
    while (p != NULL) {
        if (p->key == param_1) {
            critical_leave();
            return (s32)p;
        }
        p = p->next;
    }
    critical_leave();
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

extern Ops_3 *gSfxDriver_10;

static inline Ent_100 *find_4(s32 id)
{
    Ent_100 *e;

    critical_enter();
    for (e = sSfxSounds; e != NULL; e = e->next) {
        if (e->id == id) {
            critical_leave();
            return e;
        }
    }
    critical_leave();
    return NULL;
}

u32 sfx_get_position(s32 p)
{
    Ent_100 *e;

    if (sSfxInitialized == 0) {
        return -1;
    }
    e = find_4(p);
    if (!(e->flags & 1)) {
        return -1;
    }
    if (e->w2 == -1) {
        return e->val >> 8;
    }
    return gSfxDriver_10->fn(e->w2);
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

void sfx_set_sample(s32 p1, s32 p2) {
    Ent_101 *e;
    Ent_101 *q;

    if (sSfxInitialized != 0) {
        critical_enter();
        for (e = sSfxSounds; e != 0; e = e->next) {
            if (e->id == p1) {
                critical_leave();
                q = e;
                goto found;
            }
        }
        critical_leave();
        q = 0;
    found:
        if (q != 0) {
            q->val = p2;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))gSfxDriver[10])(q->f8, p2);
            }
        }
    }
}

void sfx_echo_enable(s32 arg0)
{
    ((void (*)(void))(*(u32 *)((u8 *)gSfxDriver + 0x58)))();
}

struct Ring_3 {
    s32 f00[5];
    u32 size;
    u8 *buf;
    u32 head;
    s32 f20;
    u32 count;
};

u32 sfx_stream_read(Ring_3 *r, u32 n, u8 *dst)
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

extern RecA_2 sSfxSamples_6[];

void sfx_lock_sample(s32 i)
{
    sSfxSamples_6[i].f = -1;
}

typedef struct {
    s32 w[5];
    s32 f14;
} Row24;

extern Row24 sSfxSamples_7[];

void func_800BA5F0(s32 a0)
{
    sSfxSamples_7[a0].f14 = 1;
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

s32 sfx_get_voice(s32 param_1)
{
    Node_48 *n;
    Node_48 *r;

    if (sSfxInitialized == 0) {
        return -1;
    }
    critical_enter();
    for (n = sSfxSounds; n != NULL; n = n->next) {
        if (n->key == param_1) {
            critical_leave();
            r = n;
            goto found;
        }
    }
    critical_leave();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}
