#include "game.h"

typedef struct Node_49 {
    struct Node_49 *next;
    struct Node_49 *prev;
    u8 *buf;
    s32 c;
    s32 d;
} Node_49;

extern Node_49 *sFilePageHead;
extern Node_49 *sFilePageTail;
extern Node_49 D_80100680;

typedef struct {
    u8 pad[0x10];
    u32 flags;
    s32 f14;
    s32 f18;
    u32 f1C;
} Rec_59;

extern Rec_59 *sFileDir_2;

s32 file_init(u8 *src)
{
    Node_49 *p;
    Node_49 *last;
    u8 *mem;
    u8 *buf;
    s32 off;
    s32 i;
    u32 n;
    s32 size;
    u32 max;
    u32 v;
    u32 pages;
    s32 k;

    if (sFileInitDone == 0) {
        sFilePageMem = mem = mem_alloc_dbg((sFilePageCount + 2) * sizeof(Node_49) + (off = (sFilePageCount + 1) << 13), 0, (s32)D_800DB410, 0xC2);
        if (mem == NULL) {
            return -1;
        }
        buf = mem - 0x2000;
        p = (Node_49 *)(mem + off);
        i = 0;
        sFilePageHead = p;
        for (; i < sFilePageCount + 2U; i++) {
            p->next = p + 1;
            p->prev = p - 1;
            p->buf = buf;
            p->c = 0;
            p->d = -1;
            p++;
            buf += 0x2000;
        }
        sFilePageTail = last = p - 1;
        sFileRomBase = src;
        sFileTempPage = last->buf;
        sFilePageHead->prev = NULL;
        last->next = NULL;
        sFilePageHead->buf = NULL;
        last->buf = NULL;
        file_read_rom(src, sFileTempPage, 0x2000);
        n = *(u32 *)sFileTempPage;
        sFileRomBase += 4;
        sFileDirCount = n;
        size = n * 32;
        sFileDataBase = (s32)sFileRomBase + size;
        if (size + 4 > 0x2000U) {
            mem_free(sFilePageMem);
            sFilePageMem = NULL;
            return -1;
        }
        sFileDir_2 = mem_alloc_dbg(size, 0, (s32)D_800DB410, 0x100);
        memcpy(sFileDir_2, sFileTempPage + 4, size);
        max = 0;
        i = 0;
        if (sFileDirCount != 0) {
        do {
            file_read_rom((void *)(sFileDir_2[i].f18 += sFileDataBase), sFileTempPage, 0x10);
            if (sFileTempPage[0] == 's' && sFileTempPage[1] == 'W' && sFileTempPage[2] == 'd' && sFileTempPage[3] == 0xB0) {
                sFileDir_2[i].flags |= 1;
                v = (sFileTempPage[8] << 24) | (sFileTempPage[9] << 16) | (sFileTempPage[10] << 8) | sFileTempPage[11];
                sFileDir_2[i].f1C = v;
                if (max < v) {
                    max = v;
                }
            } else {
                sFileDir_2[i].f1C = sFileDir_2[i].f14;
            }
        i++;
        } while (i < sFileDirCount);
        }
        i = (max + 0x1FFF) >> 13;
        max = i + 4;
        D_80100694 = 0;
        mem_fill((u8 *)&D_80100680, 0, sizeof(Node_49));
        if (max > 0x800) {
            D_80100694 = (i + 5) * 4;
            D_80100680.buf = mem_alloc_dbg(D_80100694, 0, (s32)D_800DB410, 0x13D);
            if (D_80100680.buf == NULL) {
                return -1;
            }
            D_80100680.c = 0;
            D_80100680.d = -1;
        }
        for (i = 3; i >= 0; i--) {
            sFileHandles[i].a = 0;
        }
        sFileInitDone = 1;
    }
    return 0;
}

static inline void mtf(Ent_106 *e)
{
    Ent_106 *h;

    h = sFilePageHead->next;
    if (e != h) {
        e->prev->next = e->next;
        e->next->prev = e->prev;
        e->next = h;
        e->prev = h->prev;
        e->prev->next = e;
        h->prev = e;
    }
}

static inline Ent_106 *find(Obj_81 *so)
{
    Ent_106 *cur, *nx;
    s32 want;

    want = -1;
    if (D_80100680_2.owner == (s32)so->x) {
        return &D_80100680_2;
    }
    nx = sFilePageHead->next;
    for (;;) {
        cur = nx;
        nx = cur->next;
        if (nx == NULL) {
            cur = file_load_page(so, -1, 0);
            if (cur == NULL) {
                return NULL;
            }
        }
        if (cur->owner == (s32)so->x) {
            if (cur->idx < 0) {
                cur->idx--;
                break;
            }
            if (cur->idx == want) {
                break;
            }
        }
    }
    if (cur != &D_80100680_2) {
        nx = sFilePageHead->next;
        if (cur != nx) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            cur->next = nx;
            cur->prev = nx->prev;
            cur->prev->next = cur;
            nx->prev = cur;
        }
    }
    return cur;
}

Ent_106 *file_load_page(Obj_81 *so, s32 si, s8 sf)
{
    Ent_106 *e;
    u32 n;
    u32 a3, a2;

    if (so->x->flags != 0 && si >= 0) {
        if (so->ent == NULL || (so->ent == &D_80100680_2 && D_80100680_2.owner != (s32)so->x)) {
            so->ent = find(so);
            if (so->ent == NULL) {
                return NULL;
            }
        }
    }
    e = sFilePageTail;
    do {
        e = e->prev;
    } while (e->owner != 0 && e->idx < -1);
    if (!(so->x->flags & 1)) {
        e->owner = (s32)so->x;
        e->idx = si;
        file_read_rom(so->x->base + (si << 13), e->buf, 0x2000);
    } else if (si < 0) {
        a2 = ((so->x->size + 0x1FFF) >> 13) + 4;
        if (a2 > 0x800) {
            D_80100680_2.owner = (s32)so->x;
            D_80100680_2.idx = -1;
            file_read_rom(so->x->base, D_80100680_2.buf, D_80100694);
            return &D_80100680_2;
        }
        e->owner = (s32)so->x;
        e->idx = si;
        file_read_rom(so->x->base, e->buf, 0x2000);
    } else {
        e->owner = (s32)so->x;
        e->idx = si;
        a3 = ((u32 *)so->ent->buf)[si + 3];
        a2 = ((u32 *)so->ent->buf)[si + 4];
        n = a3 & 0xF;
        a3 >>= 4;
        a2 = (a2 >> 4) - a3;
        if (n == 0) {
            file_read_rom(so->x->base + a3, e->buf, a2);
        } else {
            file_read_rom(so->x->base + a3, sFileTempPage, a2);
            gSwdSrc = sFileTempPage;
            gSwdDst = e->buf;
            swd_decompress();
        }
    }
    if (sf) {
        mtf(e);
    }
    return e;
}

s32 file_shutdown(void)
{
    s32 i;
    s32 *p;
    s32 *base;

    if (sFileInitDone == 0) {
        return 0;
    }
    i = 3;
    base = sFileHandles;
    p = base + 12;
    do {
        *p = 0;
        i = i - 1;
        p = p - 4;
    } while (i >= 0);
    if (D_80100680_3[2] != 0) {
        mem_free((void *)D_80100680_3[2]);
    }
    D_80100694 = 0;
    mem_fill((u8 *)D_80100680_3, 0, 0x14);
    mem_free((void *)sFileDir);
    mem_free((void *)sFilePageMem);
    sFileDir = 0;
    sFilePageMem = 0;
    sFileInitDone = 0;
    return 0;
}

struct Slot_17 {
    u8 *ptr;
    s32 a;
    s32 b;
    s32 c;
};

Slot_17 *file_open(u8 *param_1, char *arg1)
{
    u32 n = 0;
    s32 j;
    Slot_17 *s;
    Slot_17 *base;
    u8 *p = sFileDir;

    if (sFileDirCount != 0) {
        do {
            if (str_icmp(p, param_1) == 0) {
                break;
            }
            p += 0x20;
            n++;
        } while (n < sFileDirCount);
    }
    if (n != sFileDirCount) {
        j = 0;
        base = sFileHandles;
        s = base;
        for (; j < 4; j++) {
            if (s->ptr == 0) {
                break;
            }
            s++;
        }
        if (j != 4) {
            base[j].ptr = p;
            base[j].a = 0;
            base[j].b = 0;
            base[j].c = 0;
            return &base[j];
        }
    }
    return 0;
}

void file_close(Q_2 *q)
{
    Node_50 *n;
    Node_50 *h;
    Node_50 *hp;

    if (q->a == 0) {
        return;
    }
    n = q->b;
    if (n != 0 && n != &D_80100680) {
        n->cnt = n->cnt + 1;
        h = sFilePageHead->next;
        if (n == h) {
            q->a = 0;
            q->b = 0;
            q->c = 0;
            q->d = 0;
            return;
        }
        n->prev->next = n->next;
        n->next->prev = n->prev;
        n->next = h;
        hp = h->prev;
        n->prev = hp;
        hp->next = n;
        h->prev = n;
    }
    q->a = 0;
    q->b = 0;
    q->c = 0;
    q->d = 0;
}

void file_seek(s32 *o, s32 v, s32 mode) {
    s32 t;
    if (mode == 1) {
        t = o[3];
    } else {
        if (mode == 0) {
            o[3] = v;
            return;
        }
        if (mode != 2) {
            return;
        }
        t = ((s32 *)o[0])[7];
    }
    o[3] = v + t;
}

s32 file_tell(s32 *p)
{
    return p[3];
}

typedef struct { u8 pad[0x1C]; u32 size; } FileHdr;
struct Stream_2 { FileHdr *hdr; s32 f4; s32 f8; u32 pos; };
typedef struct { s32 f0; s32 f4; u8 *data; } Block_2;

s32 file_read(u8 *buf, s32 size, s32 count, Stream_2 *f)
{
    s32 n = count * size;
    u32 blk;
    u32 off;
    s32 chunk;
    Block_2 *b;
    u8 *dst;

    if (f->hdr->size < f->pos + n) {
        do { do { do {
            n = f->hdr->size - f->pos;
        } while (0); } while (0); } while (0);
        if (size > 1) {
            do {
                count = n / size;
                n = count * size;
            } while (0);
        }
    }
    dst = buf;
    blk = f->pos >> 13;
    off = f->pos & 0x1FFF;
    while (n != 0) {
        chunk = 0x2000 - off;
        if (n < chunk) {
            do { do { do {
                chunk = n;
            } while (0); } while (0); } while (0);
        }
        b = file_get_page(f, blk, chunk != 0x2000);
        if (b == NULL) {
            return 0;
        }
        memcpy(dst, b->data + off, chunk);
        n -= chunk;
        dst += chunk;
        do { do {
            f->pos += chunk;
            off += chunk;
        } while (0); } while (0);
        if (off == 0x2000) {
            off = 0;
            blk++;
        }
    }
    return count;
}

s32 file_eof(s32 *a0)
{
    u8 *p;
    u32 n;

    p = (u8 *)a0[0];
    n = (u32)a0[3];
    return *(u32 *)(p + 0x1C) <= n;
}

u8 *file_find(u8 *param_1)
{
    u32 i;
    u8 *p;

    i = 0;
    p = sFileDir;
    if (sFileDirCount != 0) {
        do {
            if (str_icmp(p, param_1) == 0) {
                break;
            }
            p += 0x20;
        } while (++i < sFileDirCount);
    }
    if (i == sFileDirCount) {
        return NULL;
    }
    return p;
}

Node_51 *file_get_page(s32 *k, s32 id, s8 flag)
{
    Node_51 *p;
    Node_51 *nx;

    if (id < 0) {
        p = &D_80100680;
        if (p->key == *k) {
            return p;
        }
    }
    nx = sFilePageHead->next;
    for (;;) {
        p = nx;
        nx = p->next;
        if (nx == NULL) {
            p = file_load_page(k, id, 0);
            if (p == NULL) {
                return NULL;
            }
        }
        if (p->key != *k) {
            continue;
        }
        if (id < 0 && p->cnt < 0) {
            p->cnt--;
            break;
        }
        if (p->cnt == id) {
            break;
        }
    }
    if (flag != 0 && p != &D_80100680) {
        nx = sFilePageHead->next;
        if (p != nx) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = nx;
            p->prev = nx->prev;
            p->prev->next = p;
            nx->prev = p;
        }
    }
    return p;
}

void file_read_rom(void *param_1, void *param_2, s32 param_3)
{
    s32 r;
    osInvalDCache(param_2, param_3);
    if ((sFileRomBase_2 & 0xF0000000) == 0x80000000) {
        memcpy(param_2, param_1, param_3);
    } else {
        do {
            r = dma_start(gCartRomHandle, 0, param_1, param_2, param_3);
        } while (r < 0);
        dma_wait(r);
        osInvalDCache(param_2, param_3);
    }
}
