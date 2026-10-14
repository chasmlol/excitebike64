#include "game.h"

/* The ROM file system (FILE.C). A directory of 32-byte entries sits at the start of the file area. Files are read
   through 8 KB pages cached in a most-recently-used list (sFilePageHead to sFilePageTail); an 'sWd' file is
   SWD-compressed page by page, and its header page, which says where each page is, stays cached while a handle
   uses it. */

extern FilePage *sFilePageHead;
extern FilePage *sFilePageTail;

/* Reads the directory at `src` (a ROM address, or an RDRAM one) and allocates the page cache. Returns 0, or -1 when
   memory runs out or the directory does not fit in one page. */
s32 file_init(u8 *src)
{
    FilePage *page;
    FilePage *last;
    u8 *mem;
    u8 *buf;
    s32 bufSize;
    s32 i;
    u32 count;
    s32 dirSize;
    u32 maxSize;
    u32 unpacked;
    u32 pages;
    s32 k;

    if (sFileInitDone == 0) {
        /* sFilePageCount + 1 page buffers, then sFilePageCount + 2 FilePages: the cached pages between two
           sentinels. */
        sFilePageMem = mem = mem_alloc_dbg((sFilePageCount + 2) * sizeof(FilePage)
                                               + (bufSize = (sFilePageCount + 1) << FILE_PAGE_SHIFT),
                                           0, (s32)sStrFileC, 0xC2);
        if (mem == NULL) {
            return -1;
        }
        buf = mem - FILE_PAGE_SIZE;
        page = (FilePage *)(mem + bufSize);
        i = 0;
        sFilePageHead = page;
        for (; i < sFilePageCount + 2U; i++) {
            page->next = page + 1;
            page->prev = page - 1;
            page->buf = buf;
            page->file = 0;
            page->page = -1;
            page++;
            buf += FILE_PAGE_SIZE;
        }
        /* The sentinels have no buffer: the tail's becomes the staging page for directory and compressed reads. */
        sFilePageTail = last = page - 1;
        sFileRomBase = src;
        sFileTempPage = last->buf;
        sFilePageHead->prev = NULL;
        last->next = NULL;
        sFilePageHead->buf = NULL;
        last->buf = NULL;
        /* The directory: the number of entries, then the entries. File offsets count from its end. */
        file_read_rom(src, sFileTempPage, FILE_PAGE_SIZE);
        count = *(u32 *)sFileTempPage;
        sFileRomBase += 4;
        sFileDirCount = count;
        dirSize = count * sizeof(FileDirEntry);
        sFileDataBase = (s32)sFileRomBase + dirSize;
        if (dirSize + 4 > 0x2000U) { /* FILE_PAGE_SIZE: the unsigned literal keeps the compare unsigned */
            mem_free(sFilePageMem);
            sFilePageMem = NULL;
            return -1;
        }
        sFileDir = mem_alloc_dbg(dirSize, 0, (s32)sStrFileC, 0x100);
        memcpy(sFileDir, sFileTempPage + 4, dirSize);
        maxSize = 0;
        i = 0;
        if (sFileDirCount != 0) {
            do {
                /* A compressed file starts with 's' 'W' 'd' 0xB0; the word at 8 is its unpacked size. */
                file_read_rom((sFileDir[i].rom += sFileDataBase), sFileTempPage, 0x10);
                if (sFileTempPage[0] == 's' && sFileTempPage[1] == 'W' && sFileTempPage[2] == 'd'
                    && sFileTempPage[3] == 0xB0) {
                    sFileDir[i].flags |= FILE_FLAG_SWD;
                    unpacked = (sFileTempPage[8] << 24) | (sFileTempPage[9] << 16) | (sFileTempPage[10] << 8)
                             | sFileTempPage[11];
                    sFileDir[i].unpackedSize = unpacked;
                    if (maxSize < unpacked) {
                        maxSize = unpacked;
                    }
                } else {
                    sFileDir[i].unpackedSize = sFileDir[i].size;
                }
                i++;
            } while (i < sFileDirCount);
        }
        /* The largest compressed file's header has (pages + 4) words. If that does not fit in a cached page, the
           header pages get a buffer of their own, sFileBigHeaderPage. */
        i = (maxSize + FILE_PAGE_MASK) >> FILE_PAGE_SHIFT;
        maxSize = i + 4;
        sFileBigHeaderSize = 0;
        mem_fill((u8 *)&sFileBigHeaderPage, 0, sizeof(FilePage));
        if (maxSize > FILE_PAGE_WORDS) {
            sFileBigHeaderSize = (i + 5) * 4;
            sFileBigHeaderPage.buf = mem_alloc_dbg(sFileBigHeaderSize, 0, (s32)sStrFileC, 0x13D);
            if (sFileBigHeaderPage.buf == NULL) {
                return -1;
            }
            sFileBigHeaderPage.file = 0;
            sFileBigHeaderPage.page = -1;
        }
        for (i = FILE_HANDLE_COUNT - 1; i >= 0; i--) {
            sFileHandles[i].file = 0;
        }
        sFileInitDone = 1;
    }
    return 0;
}

/* Moves a cached page to the front of the most-recently-used list. */
static inline void page_move_to_front(FilePage *page)
{
    FilePage *first;

    first = sFilePageHead->next;
    if (page != first) {
        page->prev->next = page->next;
        page->next->prev = page->prev;
        page->next = first;
        page->prev = first->prev;
        page->prev->next = page;
        first->prev = page;
    }
}

/* Returns the header page of the handle's compressed file, loading it if it is not cached, counts one more handle
   using it and moves it to the front: file_get_page(handle, -1, 1) written out for file_load_page. */
static inline FilePage *header_page_find(FileHandle *handle)
{
    FilePage *page, *next;
    s32 wanted;

    wanted = -1;
    if (sFileBigHeaderPage.file == handle->file) {
        return &sFileBigHeaderPage;
    }
    next = sFilePageHead->next;
    for (;;) {
        page = next;
        next = page->next;
        if (next == NULL) {
            page = file_load_page(handle, -1, 0);
            if (page == NULL) {
                return NULL;
            }
        }
        if (page->file == handle->file) {
            if (page->page < 0) {
                page->page--;
                break;
            }
            if (page->page == wanted) {
                break;
            }
        }
    }
    if (page != &sFileBigHeaderPage) {
        next = sFilePageHead->next;
        if (page != next) {
            page->prev->next = page->next;
            page->next->prev = page->prev;
            page->next = next;
            page->prev = next->prev;
            page->prev->next = page;
            next->prev = page;
        }
    }
    return page;
}

/* Loads page `pageNum` of the handle's file (-1: a compressed file's header page) into the least recently used
   cached page that is not a header page in use, and moves it to the front if `toFront` is set. A compressed file's
   page is found through its header page, which is looked up first. Returns the page, or NULL. */
FilePage *file_load_page(FileHandle *handle, s32 pageNum, s8 toFront)
{
    FilePage *page;
    u32 method;
    u32 start, len;

    if (handle->file->flags != 0 && pageNum >= 0) {
        if (handle->header == NULL
            || (handle->header == &sFileBigHeaderPage && sFileBigHeaderPage.file != handle->file)) {
            handle->header = header_page_find(handle);
            if (handle->header == NULL) {
                return NULL;
            }
        }
    }
    page = sFilePageTail;
    do {
        page = page->prev;
    } while (page->file != 0 && page->page < -1);
    if (!(handle->file->flags & FILE_FLAG_SWD)) {
        page->file = handle->file;
        page->page = pageNum;
        file_read_rom(handle->file->rom + (pageNum << FILE_PAGE_SHIFT), page->buf, FILE_PAGE_SIZE);
    } else if (pageNum < 0) {
        /* The header's length in words; a long one goes to sFileBigHeaderPage. */
        len = ((handle->file->unpackedSize + FILE_PAGE_MASK) >> FILE_PAGE_SHIFT) + 4;
        if (len > FILE_PAGE_WORDS) {
            sFileBigHeaderPage.file = handle->file;
            sFileBigHeaderPage.page = -1;
            file_read_rom(handle->file->rom, sFileBigHeaderPage.buf, sFileBigHeaderSize);
            return &sFileBigHeaderPage;
        }
        page->file = handle->file;
        page->page = pageNum;
        file_read_rom(handle->file->rom, page->buf, FILE_PAGE_SIZE);
    } else {
        page->file = handle->file;
        page->page = pageNum;
        /* Header word 3 + n is page n's (offset << 4) | method, word 4 + n where the next page starts. Method 0 is
           stored as is, any other SWD-compressed (unpacked through the staging page). */
        start = ((u32 *)handle->header->buf)[pageNum + 3];
        len = ((u32 *)handle->header->buf)[pageNum + 4];
        method = start & 0xF;
        start >>= 4;
        len = (len >> 4) - start;
        if (method == 0) {
            file_read_rom(handle->file->rom + start, page->buf, len);
        } else {
            file_read_rom(handle->file->rom + start, sFileTempPage, len);
            gSwdSrc = sFileTempPage;
            gSwdDst = page->buf;
            swd_decompress();
        }
    }
    if (toFront) {
        page_move_to_front(page);
    }
    return page;
}

/* Closes every handle and frees the directory and the page cache. */
s32 file_shutdown(void)
{
    s32 i;
    s32 *handleWord;
    s32 *handles;

    if (sFileInitDone == 0) {
        return 0;
    }
    /* Clears sFileHandles[3..0].file, 4 words a handle. */
    i = FILE_HANDLE_COUNT - 1;
    handles = (s32 *)sFileHandles;
    handleWord = handles + 12;
    do {
        *handleWord = 0;
        i = i - 1;
        handleWord = handleWord - 4;
    } while (i >= 0);
    if (sFileBigHeaderPage.buf != 0) {
        mem_free(sFileBigHeaderPage.buf);
    }
    sFileBigHeaderSize = 0;
    mem_fill((u8 *)&sFileBigHeaderPage, 0, sizeof(FilePage));
    mem_free(sFileDir);
    mem_free(sFilePageMem);
    sFileDir = 0;
    sFilePageMem = 0;
    sFileInitDone = 0;
    return 0;
}

/* Opens the file `name` (compared without case) in a free handle; `mode` ("rb") is not used. Returns NULL when there
   is no such file or no free handle. */
FileHandle *file_open(u8 *name, char *mode)
{
    u32 entryNum = 0;
    s32 slot;
    FileHandle *handle;
    FileHandle *handles;
    FileDirEntry *entry = sFileDir;

    if (sFileDirCount != 0) {
        do {
            if (str_icmp(entry->name, name) == 0) {
                break;
            }
            entry++;
            entryNum++;
        } while (entryNum < sFileDirCount);
    }
    if (entryNum != sFileDirCount) {
        slot = 0;
        handles = sFileHandles;
        handle = handles;
        for (; slot < FILE_HANDLE_COUNT; slot++) {
            if (handle->file == 0) {
                break;
            }
            handle++;
        }
        if (slot != FILE_HANDLE_COUNT) {
            handles[slot].file = entry;
            handles[slot].header = 0;
            handles[slot].unk_08 = 0;
            handles[slot].pos = 0;
            return &handles[slot];
        }
    }
    return 0;
}

/* Closes a handle. Its header page counts one handle less and moves to the front of the list. */
void file_close(FileHandle *handle)
{
    FilePage *header;
    FilePage *first;
    FilePage *firstPrev;

    if (handle->file == 0) {
        return;
    }
    header = handle->header;
    if (header != 0 && header != &sFileBigHeaderPage) {
        header->page = header->page + 1;
        first = sFilePageHead->next;
        if (header == first) {
            handle->file = 0;
            handle->header = 0;
            handle->unk_08 = 0;
            handle->pos = 0;
            return;
        }
        header->prev->next = header->next;
        header->next->prev = header->prev;
        header->next = first;
        firstPrev = first->prev;
        header->prev = firstPrev;
        firstPrev->next = header;
        first->prev = header;
    }
    handle->file = 0;
    handle->header = 0;
    handle->unk_08 = 0;
    handle->pos = 0;
}

/* Sets the handle's position to `offset` from the start, the current position or the end (FILE_SEEK_*); other
   modes do nothing. */
void file_seek(FileHandle *handle, s32 offset, s32 mode)
{
    s32 base;

    if (mode == FILE_SEEK_CUR) {
        base = handle->pos;
    } else {
        if (mode == FILE_SEEK_SET) {
            handle->pos = offset;
            return;
        }
        if (mode != FILE_SEEK_END) {
            return;
        }
        base = handle->file->unpackedSize;
    }
    handle->pos = offset + base;
}

s32 file_tell(FileHandle *handle)
{
    return handle->pos;
}

/* Reads `count` items of `size` bytes from the handle's position (fread), stopping at the end of the file. Returns
   the number of items, or 0 when a page cannot be loaded. With a size of 1 a short read still returns `count`. A
   page read only in part moves to the front of the cache. The do-while(0) nests are kept for the code they give. */
s32 file_read(u8 *buf, s32 size, s32 count, FileHandle *handle)
{
    s32 bytes = count * size;
    u32 pageNum;
    u32 offset;
    s32 chunk;
    FilePage *page;
    u8 *dst;

    if (handle->file->unpackedSize < handle->pos + bytes) {
        do { do { do {
            bytes = handle->file->unpackedSize - handle->pos;
        } while (0); } while (0); } while (0);
        if (size > 1) {
            do {
                count = bytes / size;
                bytes = count * size;
            } while (0);
        }
    }
    dst = buf;
    pageNum = handle->pos >> FILE_PAGE_SHIFT;
    offset = handle->pos & FILE_PAGE_MASK;
    while (bytes != 0) {
        chunk = FILE_PAGE_SIZE - offset;
        if (bytes < chunk) {
            do { do { do {
                chunk = bytes;
            } while (0); } while (0); } while (0);
        }
        page = file_get_page(handle, pageNum, chunk != FILE_PAGE_SIZE);
        if (page == NULL) {
            return 0;
        }
        memcpy(dst, page->buf + offset, chunk);
        bytes -= chunk;
        dst += chunk;
        do { do {
            handle->pos += chunk;
            offset += chunk;
        } while (0); } while (0);
        if (offset == FILE_PAGE_SIZE) {
            offset = 0;
            pageNum++;
        }
    }
    return count;
}

/* Returns whether the handle is at (or past) the end of its file. */
s32 file_eof(FileHandle *handle)
{
    FileDirEntry *file;
    u32 pos;

    file = handle->file;
    pos = handle->pos;
    return file->unpackedSize <= pos;
}

/* Returns the directory entry of the file `name` (compared without case), or NULL. */
FileDirEntry *file_find(u8 *name)
{
    u32 i;
    FileDirEntry *entry;

    i = 0;
    entry = sFileDir;
    if (sFileDirCount != 0) {
        do {
            if (str_icmp(entry->name, name) == 0) {
                break;
            }
            entry++;
        } while (++i < sFileDirCount);
    }
    if (i == sFileDirCount) {
        return NULL;
    }
    return entry;
}

/* Returns page `pageNum` of the handle's file (-1: its header page, counting one more handle using it), loading it
   when it is not cached, and moves it to the front of the list if `toFront` is set. Returns NULL when it cannot be
   loaded. */
FilePage *file_get_page(FileHandle *handle, s32 pageNum, s8 toFront)
{
    FilePage *page;
    FilePage *next;

    if (pageNum < 0) {
        page = &sFileBigHeaderPage;
        if (page->file == handle->file) {
            return page;
        }
    }
    next = sFilePageHead->next;
    for (;;) {
        page = next;
        next = page->next;
        if (next == NULL) {
            page = file_load_page(handle, pageNum, 0);
            if (page == NULL) {
                return NULL;
            }
        }
        if (page->file != handle->file) {
            continue;
        }
        if (pageNum < 0 && page->page < 0) {
            page->page--;
            break;
        }
        if (page->page == pageNum) {
            break;
        }
    }
    if (toFront != 0 && page != &sFileBigHeaderPage) {
        next = sFilePageHead->next;
        if (page != next) {
            page->prev->next = page->next;
            page->next->prev = page->prev;
            page->next = next;
            page->prev = next->prev;
            page->prev->next = page;
            next->prev = page;
        }
    }
    return page;
}

/* Copies `size` bytes of the file system at `rom` to `ram`: a memcpy when the file system is in RDRAM (KSEG0),
   else a cartridge DMA, retried until a request slot is free. */
void file_read_rom(void *rom, void *ram, s32 size)
{
    s32 req;
    osInvalDCache(ram, size);
    if (((u32)sFileRomBase & 0xF0000000) == 0x80000000) {
        memcpy(ram, rom, size);
    } else {
        do {
            req = dma_start(gCartRomHandle, OS_READ, rom, ram, size);
        } while (req < 0);
        dma_wait(req);
        osInvalDCache(ram, size);
    }
}
