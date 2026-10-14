#include "game.h"

extern FilePage *sFilePageHead;
extern FilePage *sFilePageTail;

s32 file_init(u8 *src)
{
    FilePage *p;
    FilePage *last;
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
        sFilePageMem = mem = mem_alloc_dbg((sFilePageCount + 2) * sizeof(FilePage) + (off = (sFilePageCount + 1) << 13), 0, (s32)D_800DB410, 0xC2);
        if (mem == NULL) {
            return -1;
        }
        buf = mem - 0x2000;
        p = (FilePage *)(mem + off);
        i = 0;
        sFilePageHead = p;
        for (; i < sFilePageCount + 2U; i++) {
            p->next = p + 1;
            p->prev = p - 1;
            p->buf = buf;
            p->file = 0;
            p->page = -1;
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
        size = n * sizeof(FileDirEntry);
        sFileDataBase = (s32)sFileRomBase + size;
        if (size + 4 > 0x2000U) {
            mem_free(sFilePageMem);
            sFilePageMem = NULL;
            return -1;
        }
        sFileDir = mem_alloc_dbg(size, 0, (s32)D_800DB410, 0x100);
        memcpy(sFileDir, sFileTempPage + 4, size);
        max = 0;
        i = 0;
        if (sFileDirCount != 0) {
        do {
            file_read_rom((sFileDir[i].rom += sFileDataBase), sFileTempPage, 0x10);
            if (sFileTempPage[0] == 's' && sFileTempPage[1] == 'W' && sFileTempPage[2] == 'd' && sFileTempPage[3] == 0xB0) {
                sFileDir[i].flags |= 1;
                v = (sFileTempPage[8] << 24) | (sFileTempPage[9] << 16) | (sFileTempPage[10] << 8) | sFileTempPage[11];
                sFileDir[i].unpackedSize = v;
                if (max < v) {
                    max = v;
                }
            } else {
                sFileDir[i].unpackedSize = sFileDir[i].size;
            }
        i++;
        } while (i < sFileDirCount);
        }
        i = (max + 0x1FFF) >> 13;
        max = i + 4;
        D_80100694 = 0;
        mem_fill((u8 *)&D_80100680, 0, sizeof(FilePage));
        if (max > 0x800) {
            D_80100694 = (i + 5) * 4;
            D_80100680.buf = mem_alloc_dbg(D_80100694, 0, (s32)D_800DB410, 0x13D);
            if (D_80100680.buf == NULL) {
                return -1;
            }
            D_80100680.file = 0;
            D_80100680.page = -1;
        }
        for (i = 3; i >= 0; i--) {
            sFileHandles[i].file = 0;
        }
        sFileInitDone = 1;
    }
    return 0;
}

static inline void mtf(FilePage *e)
{
    FilePage *h;

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

static inline FilePage *find(FileHandle *so)
{
    FilePage *cur, *nx;
    s32 want;

    want = -1;
    if (D_80100680.file == so->file) {
        return &D_80100680;
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
        if (cur->file == so->file) {
            if (cur->page < 0) {
                cur->page--;
                break;
            }
            if (cur->page == want) {
                break;
            }
        }
    }
    if (cur != &D_80100680) {
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

FilePage *file_load_page(FileHandle *so, s32 si, s8 sf)
{
    FilePage *e;
    u32 n;
    u32 a3, a2;

    if (so->file->flags != 0 && si >= 0) {
        if (so->header == NULL || (so->header == &D_80100680 && D_80100680.file != so->file)) {
            so->header = find(so);
            if (so->header == NULL) {
                return NULL;
            }
        }
    }
    e = sFilePageTail;
    do {
        e = e->prev;
    } while (e->file != 0 && e->page < -1);
    if (!(so->file->flags & 1)) {
        e->file = so->file;
        e->page = si;
        file_read_rom(so->file->rom + (si << 13), e->buf, 0x2000);
    } else if (si < 0) {
        a2 = ((so->file->unpackedSize + 0x1FFF) >> 13) + 4;
        if (a2 > 0x800) {
            D_80100680.file = so->file;
            D_80100680.page = -1;
            file_read_rom(so->file->rom, D_80100680.buf, D_80100694);
            return &D_80100680;
        }
        e->file = so->file;
        e->page = si;
        file_read_rom(so->file->rom, e->buf, 0x2000);
    } else {
        e->file = so->file;
        e->page = si;
        a3 = ((u32 *)so->header->buf)[si + 3];
        a2 = ((u32 *)so->header->buf)[si + 4];
        n = a3 & 0xF;
        a3 >>= 4;
        a2 = (a2 >> 4) - a3;
        if (n == 0) {
            file_read_rom(so->file->rom + a3, e->buf, a2);
        } else {
            file_read_rom(so->file->rom + a3, sFileTempPage, a2);
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
    base = (s32 *)sFileHandles;
    p = base + 12;
    do {
        *p = 0;
        i = i - 1;
        p = p - 4;
    } while (i >= 0);
    if (D_80100680.buf != 0) {
        mem_free(D_80100680.buf);
    }
    D_80100694 = 0;
    mem_fill((u8 *)&D_80100680, 0, sizeof(FilePage));
    mem_free(sFileDir);
    mem_free(sFilePageMem);
    sFileDir = 0;
    sFilePageMem = 0;
    sFileInitDone = 0;
    return 0;
}

FileHandle *file_open(u8 *param_1, char *arg1)
{
    u32 n = 0;
    s32 j;
    FileHandle *s;
    FileHandle *base;
    FileDirEntry *p = sFileDir;

    if (sFileDirCount != 0) {
        do {
            if (str_icmp(p->name, param_1) == 0) {
                break;
            }
            p++;
            n++;
        } while (n < sFileDirCount);
    }
    if (n != sFileDirCount) {
        j = 0;
        base = sFileHandles;
        s = base;
        for (; j < 4; j++) {
            if (s->file == 0) {
                break;
            }
            s++;
        }
        if (j != 4) {
            base[j].file = p;
            base[j].header = 0;
            base[j].unk_08 = 0;
            base[j].pos = 0;
            return &base[j];
        }
    }
    return 0;
}

void file_close(FileHandle *q)
{
    FilePage *n;
    FilePage *h;
    FilePage *hp;

    if (q->file == 0) {
        return;
    }
    n = q->header;
    if (n != 0 && n != &D_80100680) {
        n->page = n->page + 1;
        h = sFilePageHead->next;
        if (n == h) {
            q->file = 0;
            q->header = 0;
            q->unk_08 = 0;
            q->pos = 0;
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
    q->file = 0;
    q->header = 0;
    q->unk_08 = 0;
    q->pos = 0;
}

void file_seek(FileHandle *o, s32 v, s32 mode) {
    s32 t;
    if (mode == 1) {
        t = o->pos;
    } else {
        if (mode == 0) {
            o->pos = v;
            return;
        }
        if (mode != 2) {
            return;
        }
        t = o->file->unpackedSize;
    }
    o->pos = v + t;
}

s32 file_tell(FileHandle *p)
{
    return p->pos;
}

s32 file_read(u8 *buf, s32 size, s32 count, FileHandle *f)
{
    s32 n = count * size;
    u32 blk;
    u32 off;
    s32 chunk;
    FilePage *b;
    u8 *dst;

    if (f->file->unpackedSize < f->pos + n) {
        do { do { do {
            n = f->file->unpackedSize - f->pos;
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
        memcpy(dst, b->buf + off, chunk);
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

s32 file_eof(FileHandle *a0)
{
    FileDirEntry *p;
    u32 n;

    p = a0->file;
    n = a0->pos;
    return p->unpackedSize <= n;
}

FileDirEntry *file_find(u8 *param_1)
{
    u32 i;
    FileDirEntry *p;

    i = 0;
    p = sFileDir;
    if (sFileDirCount != 0) {
        do {
            if (str_icmp(p->name, param_1) == 0) {
                break;
            }
            p++;
        } while (++i < sFileDirCount);
    }
    if (i == sFileDirCount) {
        return NULL;
    }
    return p;
}

FilePage *file_get_page(FileHandle *k, s32 id, s8 flag)
{
    FilePage *p;
    FilePage *nx;

    if (id < 0) {
        p = &D_80100680;
        if (p->file == k->file) {
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
        if (p->file != k->file) {
            continue;
        }
        if (id < 0 && p->page < 0) {
            p->page--;
            break;
        }
        if (p->page == id) {
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
    if (((u32)sFileRomBase & 0xF0000000) == 0x80000000) {
        memcpy(param_2, param_1, param_3);
    } else {
        do {
            r = dma_start(gCartRomHandle, 0, param_1, param_2, param_3);
        } while (r < 0);
        dma_wait(r);
        osInvalDCache(param_2, param_3);
    }
}
