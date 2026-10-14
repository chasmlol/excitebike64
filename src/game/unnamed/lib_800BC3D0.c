#include "game.h"

#define BLOCK_MAGIC 0xB562439F

typedef struct Blk_4 {
    struct Blk_4 *next;
    struct Blk_4 *prev;
    s32 size;
    u32 check;
} Blk_4;

static __inline__ void addhex(char *b, u32 r)
{
    char s[9];
    char *p;
    u32 *tab;
    u32 q;

    tab = sHexDigitDivisors;
    p = s;
    do {
        q = r / *tab;
        if ((s32)q < 10) {
            *p = q + '0';
        } else {
            *p = q + '7';
        }
        r -= q * *tab;
        tab++;
        p++;
    } while ((s32)tab < (s32)(sHexDigitDivisors + 8));
    s[8] = 0;
    str_cat(b, s);
}

static __inline__ void adddec(char *b, u32 r)
{
    char s[6];
    char *p;
    u32 *tab;
    u32 q;
    s32 i;

    r = (u16)r;
    tab = sDecDigitDivisors;
    p = s;
    do {
        q = r / *tab;
        if ((s32)q < 10) {
            *p = q + '0';
        } else {
            *p = q + '7';
        }
        r -= q * *tab;
        tab++;
        p++;
    } while ((s32)tab < (s32)(sDecDigitDivisors + 5));
    s[5] = 0;
    for (i = 0; i < 4; i++) {
        if (s[i] != '0') {
            break;
        }
        s[i] = ' ';
    }
    str_cat(b, s);
}

void mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    char buf[0x50];
    s32 n;
    s32 v;

    sprintf(buf, "\n");
    debug_log_2(buf);
    v = 0;
    n = mem_dump_blocks(sMemHeapStart, sMemHeapEnd, 1);
    if (n > 0) {
        v = n;
    }
    if (v >= 0) {
        strcpy(buf, "Total Memory Used == 0x");
        addhex(buf, sMemUsedUnits << 4);
        str_cat(buf, "\n");
        debug_log_2(buf);

        strcpy(buf, "Total Memory Free == 0x");
        addhex(buf, gMemFreeUnits << 4);
        str_cat(buf, " in ");
        adddec(buf, (u16)v);
        str_cat(buf, " block(s)\n");
        debug_log_2(buf);
    }
}

static inline void AddHex(char *s, u32 v)
{
    char dig[12];
    char *dp;
    u32 *tp;
    u32 q;
    char c;

    tp = sHexDigitDivisors;
    dp = dig;
    do {
        q = v / *tp;
        if ((s32)q >= 10) c = q + '7';
        else c = q + '0';
        *dp = c;
        v -= q * *tp;
        tp++;
        dp++;
    } while ((s32)tp < (s32)&sHexDigitDivisors[8]);
    dig[8] = 0;
    str_cat(s, dig);
}

static inline void AddDec(char *s, u16 v)
{
    char dig[12];
    char *dp;
    u32 *tp;
    u32 q;
    char c;
    u32 x = v;
    s32 i;

    tp = sDecDigitDivisors;
    dp = dig;
    do {
        q = x / *tp;
        if ((s32)q >= 10) c = q + '7';
        else c = q + '0';
        *dp = c;
        x -= q * *tp;
        tp++;
        dp++;
    } while ((s32)tp < (s32)&sDecDigitDivisors[5]);
    dig[5] = 0;
    for (i = 0; i < 4 && dig[i] == '0'; i++) dig[i] = ' ';
    str_cat(s, dig);
}

void mem_report_totals(void)
{
    char buf[0x50];
    s32 n;
    s32 s0;

    sprintf(buf, "\n");
    debug_log(buf);
    s0 = 0;
    n = mem_dump_blocks(sMemHeapStart, sMemHeapEnd, s0);
    if (n > 0) s0 = n;
    if (s0 < 0) return;

    strcpy(buf, "Total Memory Used == 0x");
    AddHex(buf, sMemUsedUnits << 4);
    str_cat(buf, "\n");
    debug_log(buf);

    strcpy(buf, "Total Memory Free == 0x");
    AddHex(buf, gMemFreeUnits << 4);
    str_cat(buf, " in ");
    AddDec(buf, s0);
    str_cat(buf, " block(s)\n");
    debug_log(buf);
}

#define CHECK(b) ((b)->check = 0xB562439F - ((u32)(b)->next + (u32)(b)->prev + (b)->size))

void *mem_alloc_dbg(s32 size, u32 flags, s32 owner, s32 tag)
{
    Blk_4 *sent;
    Blk_4 *b;
    Blk_4 *nb;
    Blk_4 *next;
    Blk_4 *prev;
    s32 n;

    if (size == 0) {
        return 0;
    }
    sent = sMemFreeList;
    n = ((u32)(size + 15) >> 4) + 1;
    if (!(flags & 1)) {
        b = sent->next;
        for (;;) {
            size = b->size;
            if (n <= size) {
                goto found;
            }
            b = b->next;
            if (b == sent) {
                break;
            }
        }
    } else {
        b = sent->prev;
        for (;;) {
            size = b->size;
            if (n <= size) {
                goto found;
            }
            b = b->prev;
            if (b == sent) {
                break;
            }
        }
    }
    return 0;
found:
    prev = b->prev;
    next = b->next;
    if (size == n) {
        prev->next = next;
        next->prev = prev;
        CHECK(prev);
        CHECK(next);
    } else {
        size -= n;
        if (!(flags & 1)) {
            sent = b + n;
            sent->next = next;
            sent->prev = prev;
            sent->size = size;
            prev->next = sent;
            next->prev = sent;
            CHECK(prev);
            CHECK(sent);
            CHECK(next);
        } else {
            b->size = size;
            b->check = 0xB562439F - ((u32)b->next + (u32)b->prev + size);
            b += size;
        }
    }
    b->next = (Blk_4 *)owner;
    b->size = n;
    tag = (tag << 16) | sMemAllocSerial | 1;
    sMemAllocSerial = (sMemAllocSerial + 2) & 0xFFFE;
    b->prev = (Blk_4 *)tag;
    gMemFreeUnits -= n;
    b->check = 0xB562439F - ((u32)b->next + (u32)b->prev + n);
    sMemUsedUnits += n;
    if (sMemPeakUsedUnits < sMemUsedUnits) {
        sMemPeakUsedUnits = sMemUsedUnits;
    }
    return b + 1;
}

typedef struct Blk_28 {
    struct Blk_28 *next;
    struct Blk_28 *prev;
    s32 size;
    u32 sum;
} Blk_28;

void mem_free(void *ptr)
{
    Blk_28 *p;
    Blk_28 *a;
    Blk_28 *n;
    Blk_28 *t;
    u32 sum;
    char *m;
    s32 size;
    u32 flag;
    u32 tv;

    if (ptr == NULL) {
        return;
    }
    p = (Blk_28 *)ptr - 1;
    tv = (u32)p->next;
    flag = (u32)p->prev;
    size = p->size;
    sum = tv + flag + size + p->sum;
    if (sum != 0xB562439F) {
        goto inv;
    }
    t = p + p->size;
    if ((u32)t->next + (u32)t->prev + t->size + t->sum != sum) {
        goto overrun;
    }
    if (!((s32)p->prev & 1)) {
        goto alr;
    }
    a = sMemFreeList;
    p->prev = NULL;
    n = a->next;
    gMemFreeUnits += p->size;
    sMemUsedUnits -= p->size;
    if (a == n) {
        goto insert;
    }
    for (;;) {
        if (a < p && p < n) {
            goto insert;
        }
        if (a >= n && (a < p || p < n)) {
            goto insert;
        }
        a = n;
        n = n->next;
    }
insert:
    a->next = p;
    n->prev = p;
    p->prev = a;
    p->next = n;
    if (a + a->size == p) {
        a->size += p->size;
        a->next = p->next;
        p->next->prev = a;
        p = a;
    }
    if (p + p->size == n) {
        p->size += n->size;
        p->next = n->next;
        n = n->next;
        n->prev = p;
    }
    a->sum = 0xB562439F - ((u32)a->next + (u32)a->prev + a->size);
    p->sum = 0xB562439F - ((u32)p->next + (u32)p->prev + p->size);
    n->sum = 0xB562439F - ((u32)n->next + (u32)n->prev + n->size);
    return;
inv:
    m = D_800DB248;
    goto call;
overrun:
    m = D_800DB288;
    goto call;
alr:
    m = D_800DB2BC;
call:
    debug_log_3(m, size, sum, flag);
    mem_report_2();
}

#define BLK_CHECK(b) ((b)->check = 0xB562439F - ((u32)(b)->next + (u32)(b)->prev + (b)->size))

extern Blk_4 **sMemFreeList_2;

void *mem_alloc_at_dbg(void *ptr, s32 size, s32 tag, s32 id)
{
    Blk_4 *hdr;
    Blk_4 *b;
    Blk_4 *first;
    Blk_4 *end;
    Blk_4 *lim;
    Blk_4 *next;
    Blk_4 *prev;
    u32 units;

    if (size == 0) {
        return 0;
    }
    hdr = (Blk_4 *)((u32)ptr & ~0xF) - 1;
    do {
    units = (u32)ptr - (u32)hdr;
    units = (units + size + 15) >> 4;
    } while (0);
    lim = hdr + units;
    first = b = *sMemFreeList_2;
    for (;;) {
        size = b->size;
        if (hdr < b || (end = b + size) < lim) {
            b = b->next;
            if (b == first) {
                break;
            }
        } else {
            first = hdr + units;
            size = end - first;
            next = b->next;
            prev = b->prev;
            if (size > 0) {
                b->next = first;
                next->prev = first;
                BLK_CHECK(next);
                first->next = next;
                first->prev = b;
                first->size = size;
                next = first;
            }
            if (b < hdr) {
                size = hdr - b;
                b->size = size;
                BLK_CHECK(b);
            } else {
                prev->next = next;
                next->prev = prev;
            }
            do {
                size = sMemAllocSerial;
                id = (id << 16) | size | 1;
                size += 2;
                size &= 0xFFFE;
                sMemAllocSerial = size;
                BLK_CHECK(prev);
            } while (0);
            {
                s32 t;
                u32 pv;
                t = gMemFreeUnits;
                pv = (u32)next->prev;
                gMemFreeUnits = t - units;
                next->check = 0xB562439F - ((u32)next->next + pv + next->size + ((u16)pv >> 16));
            }
            hdr->next = (Blk_4 *)tag;
            hdr->prev = (Blk_4 *)id;
            hdr->size = units;
            BLK_CHECK(hdr);
            sMemUsedUnits += units;
            if (sMemPeakUsedUnits < sMemUsedUnits) {
                sMemPeakUsedUnits = sMemUsedUnits;
            }
            return ptr;
        }
    }
    return 0;
}

typedef struct {
    char *file;
    u32 info;
    u32 size;
    u32 chk;
} MemBlk;

static inline void AddHex_2(char *s, u32 v)
{
    char dig[12];
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 q = v / sHexDigitDivisors[i];
        if (q < 10) dig[i] = q + '0';
        else dig[i] = q + '7';
        v -= q * sHexDigitDivisors[i];
    }
    dig[8] = 0;
    str_cat(s, dig);
}

static inline void AddDec_2(char *s, u32 v)
{
    char dig[12];
    s32 i;

    v &= 0xFFFF;
    for (i = 0; i < 5; i++) {
        s32 q = v / sDecDigitDivisors[i];
        if (q < 10) dig[i] = q + '0';
        else dig[i] = q + '7';
        v -= q * sDecDigitDivisors[i];
    }
    dig[5] = 0;
    for (i = 0; i < 4; i++) {
        if (dig[i] != '0') break;
        dig[i] = ' ';
    }
    str_cat(s, dig);
}

s32 mem_dump_blocks(s32 a, u32 b, s32 c)
{
    char buf[0x50];
    s32 count;
    MemBlk *p;

    char *s;
    char *f;

    sprintf(buf, "\n");
    debug_log_2(buf);
    count = 0;
    if (a == 0) {
        return count;
    }

    for (p = (MemBlk *)(a + 0x20); (u32)p < b; p = (MemBlk *)((u8 *)p + (p->size << 4))) {
        if ((u32)p->file + p->info + p->size + p->chk != 0xB562439F) {
            strcpy(buf, " Addr 0x");
            AddHex_2(buf, (u32)p);
            str_cat(buf, " Size ?????????? - Corrupt\n");
            if (c) {
                debug_log_2(buf);
            }
            count = -1;
            break;
        }
        if (!(p->info & 1)) {
            strcpy(buf, " Addr 0x");
            AddHex_2(buf, (u32)p);
            str_cat(buf, " Size 0x");
            AddHex_2(buf, p->size << 4);
            str_cat(buf, " - Free\n");
            if (c) {
                debug_log_2(buf);
            }
            count++;
            continue;
        }
        if ((p->info & 0xFFFE) < sMemReportMark) {
            continue;
        }
        f = p->file;
        if (f == 0) {
            strcpy(buf, " Addr 0x");
            AddHex_2(buf, (u32)p);
            str_cat(buf, " Size 0x");
            AddHex_2(buf, p->size << 4);
            str_cat(buf, " - Time ");
            AddDec_2(buf, (p->info & 0xFFFE) >> 1);
        } else {
            s = f;
            while (*s++) {
            }
            for (s--; s >= f; s--) {
                if (*s == 0x5C || *s == 0x2F) break;
            }
            s++;
            strcpy(buf, " Addr 0x");
            AddHex_2(buf, (u32)p);
            str_cat(buf, " Size 0x");
            AddHex_2(buf, p->size << 4);
            str_cat(buf, " - Time ");
            AddDec_2(buf, (p->info & 0xFFFE) >> 1);
            str_cat(buf, " Line ");
            AddDec_2(buf, *(u16 *)&p->info);
            str_cat(buf, " File ");
            str_cat(buf, s);
        }
        str_cat(buf, "\n");
        if (c) {
            debug_log_2(buf);
        }
    }
    sprintf(buf, "\n");
    if (c) {
        debug_log_2(buf);
    }
    return count;
}

void mem_report_clear_mark(void)
{
    sMemReportMark = 0;
}

void mem_report_set_mark(void)
{
    sMemReportMark = sMemAllocSerial;
}

s32 mem_init(s32 p0, s32 p1, s32 a, s32 b)
{
    s32 sum;
    s32 *pa;
    s32 *pb;

    pa = &sMemHeapStart;
    pb = &sMemHeapEnd;
    sMemPeakUsedUnits = 0;
    sMemUsedUnits = 0;
    gMemFreeUnits = 0;
    sMemAllocSerial = 2;
    *pa = a;
    sum = a + b;
    sMemHeapEnd = sum;
    sMemFreeList = mem_format_heap(pa, pb);
    gMemFreeUnits = gMemFreeUnits + ((sMemHeapEnd - sMemHeapStart) >> 4);
    return 0;
}

s32 *mem_calloc_dbg(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 *p = 0;
    s32 k;
    if (a0 == 0) {
        return 0;
    }
    p = mem_alloc_dbg(a0, a1, a2, a3);
    if (p != 0) {
        s32 *q = p;
        k = p[-2] - 1;
        do {
            q[0] = 0;
            q[1] = 0;
            q[2] = 0;
            q[3] = 0;
            q += 4;
            k--;
        } while (k != 0);
    }
    return p;
}

s32 *mem_realloc_dbg(s32 *p, u32 size, s32 a, s32 b, s32 c)
{
    Blk *blk;
    Blk *nb;
    u32 need;
    u32 have;
    s32 *q;
    s32 *src;
    s32 *dst;
    s32 cnt;

    if (size == 0) {
        if (p == 0) {
            return 0;
        }
        mem_free(p);
        return 0;
    }
    if (p != 0) {
        blk = (Blk *)p - 1;
        need = ((size + 0xF) >> 4) + 1;
        have = blk->w[2];
        if (need == have) {
            return p;
        }
        if (need <= have) {
            nb = blk + need;
            nb->w[2] = have - need;
            nb->w[0] = 0;
            nb->w[1] = 1;
            {
                s32 sum = nb->w[0] + 1;
                nb->w[3] = 0xB562439F - (sum + nb->w[2]);
            }
            blk->w[2] = need;
            blk->w[3] = 0xB562439F - (blk->w[0] + blk->w[1] + need);
            mem_free(nb + 1);
            return p;
        }
    }
    q = mem_alloc_dbg(size, a, b, c);
    if (q != 0 && p != 0) {
        src = p;
        dst = q;
        need = ((Blk *)p - 1)->w[2] * 4;
        while (--need != -1) {
            *dst++ = *src++;
        }
        mem_free(p);
    }
    return q;
}

s32 mem_check(void) {
    s32 r = mem_validate(sMemHeapStart, sMemHeapEnd, sMemFreeList);

    if (r < 0) {
        while (1) {
        }
    }
    return 0;
}

s32 mem_get_size(s32 *p)
{
    s32 *h = p - 4;
    s32 *t;
    s32 sum;
    char *msg;

    sum = h[0] + h[1] + h[2] + h[3];
    if (sum != 0xB562439F) {
        goto bad_head;
    }
    t = h + h[2] * 4;
    if (t[0] + t[1] + t[2] + t[3] != 0xB562439F) {
        goto bad_tail;
    }
    if (h[1] & 1) {
        return (h[2] - 1) * 16;
    }
    goto bad_free;
bad_head:
    msg = D_800DB2F4;
    goto fail;
bad_tail:
    msg = D_800DB340;
    goto fail;
bad_free:
    msg = D_800DB364;
fail:
    debug_log_3(msg);
    mem_report_2();
    return -1;
}

void mem_alloc(s32 a, s32 b) {
    mem_alloc_dbg(a, b, 0, 0);
}

void mem_calloc(u32 a, u32 b) {
    mem_calloc_dbg(a, b, 0, 0);
}

void mem_realloc(s32 *a, s32 b, s32 c) {
    mem_realloc_dbg(a, b, c, 0, 0);
}

void mem_alloc_at(s32 a, s32 b) {
    mem_alloc_at_dbg(a, b, 0, 0);
}

#define BLOCK_SUM(b) ((u32)(b)->next + (b)->unk4 + (b)->size)

Block *mem_format_heap(Block **start, Block **end)
{
    Block *p = *start;
    Block *e = *end;
    u32 n;

    if (p != NULL) {
        p = (Block *)(((u32)p + 0xF) & ~0xF);
        e = (Block *)((u32)e & ~0xF) - 1;
        n = e - p;
        if (n < 4) {
            return NULL;
        }
        *start = p;
        *end = e;
        e->next = e;
        e->unk4 = (s32)e;
        e->size = 0;
        e->check = BLOCK_MAGIC - BLOCK_SUM(e);
        p[0].next = &p[2];
        p[0].unk4 = (s32)&p[2];
        p[0].size = 1;
        p[0].check = BLOCK_MAGIC - BLOCK_SUM(&p[0]);
        p[1].next = NULL;
        p[1].unk4 = 1;
        p[1].size = 1;
        p[1].check = BLOCK_MAGIC - BLOCK_SUM(&p[1]);
        p[2].next = p;
        p[2].unk4 = (s32)p;
        p[2].size = n - 2;
        p[2].check = BLOCK_MAGIC - BLOCK_SUM(&p[2]);
    }
    return p;
}

#define BLOCK_SUM_2(b) ((u32)(b)->next + (b)->unk4 + (b)->size + (b)->check)

s32 mem_validate(Block *start, Block *end, Block *list) {
    Block *b;
    s32 n = 0x400;

    if (start != NULL) {
        b = start;
        do {
            if (b >= end) {
                goto final;
            }
            if (BLOCK_SUM_2(b) != BLOCK_MAGIC) {
                return -1;
            }
            b += b->size;
        } while (b >= start);
    final:
        if (BLOCK_SUM_2(b) != BLOCK_MAGIC) {
            return -1;
        }
        b = list->next;
        while (--n != -1) {
            if (b == list) {
                goto done;
            }
            if (b < start || b >= end || ((u32)b & 0xF) != 0) {
                break;
            }
            b = b->next;
        }
        if (b != list) {
            return -1;
        }
    }
done:
    return 0x400 - n;
}

void str_cat_hex(char *a0, u32 a1)
{
    u32 *a2;
    char *a3;
    char buf[9];
    s32 q;

    a2 = sHexDigitDivisors;
    a3 = buf;
    do {
        q = (s32)(a1 / *a2);
        if (q < 10) {
            *a3 = (char)(q + '0');
        } else {
            *a3 = (char)(q + '7');
        }
        a1 = a1 - q * *a2;
        a2 = a2 + 1;
        a3 = a3 + 1;
    } while ((s32)a2 < (s32)(sHexDigitDivisors + 8));
    buf[8] = 0;
    str_cat(a0, buf);
}
