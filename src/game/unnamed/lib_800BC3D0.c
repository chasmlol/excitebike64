#include "game.h"

/* The heap. Memory is handed out in 16-byte units. Every block starts with a one-unit header {next, prev, size,
   check}, where size counts the header too and next + prev + size + check == MEM_CHECK_SUM. Free blocks form a
   circular list sorted by address, whose head is sMemFreeList; a used block's next is its owner (a __FILE__ string)
   and its prev (line << 16) | serial | MEM_USED. The header structs here (and Blk, Block of structs.h) are views of
   the same header, each declared the way its functions' code needs. */

typedef struct MemHeader {
    struct MemHeader *next;
    struct MemHeader *prev;
    s32 size;
    u32 check;
} MemHeader;

/* Append a number to a report line: 8 hex digits, or 5 decimal digits with leading zeros turned into spaces ('7'
   + 10 is 'A'). Each report function has its own copy, written differently for the code each one gives;
   str_cat_hex at the end of the file is the hex one as a function. */
static __inline__ void report_add_hex(char *line, u32 value)
{
    char digits[9];
    char *out;
    u32 *divisor;
    u32 digit;

    divisor = sHexDigitDivisors;
    out = digits;
    do {
        digit = value / *divisor;
        if ((s32)digit < 10) {
            *out = digit + '0';
        } else {
            *out = digit + '7';
        }
        value -= digit * *divisor;
        divisor++;
        out++;
    } while ((s32)divisor < (s32)(sHexDigitDivisors + 8));
    digits[8] = 0;
    str_cat(line, digits);
}

static __inline__ void report_add_dec(char *line, u32 value)
{
    char digits[6];
    char *out;
    u32 *divisor;
    u32 digit;
    s32 i;

    value = (u16)value;
    divisor = sDecDigitDivisors;
    out = digits;
    do {
        digit = value / *divisor;
        if ((s32)digit < 10) {
            *out = digit + '0';
        } else {
            *out = digit + '7';
        }
        value -= digit * *divisor;
        divisor++;
        out++;
    } while ((s32)divisor < (s32)(sDecDigitDivisors + 5));
    digits[5] = 0;
    for (i = 0; i < 4; i++) {
        if (digits[i] != '0') {
            break;
        }
        digits[i] = ' ';
    }
    str_cat(line, digits);
}

/* Logs every block of the heap (debug_log_2) and the "Total Memory Used/Free" lines. The arguments are not used. */
void mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    char buf[0x50];
    s32 result;
    s32 freeBlocks;

    sprintf(buf, "\n");
    debug_log_2(buf);
    freeBlocks = 0;
    result = mem_dump_blocks(sMemHeapStart, sMemHeapEnd, 1);
    if (result > 0) {
        freeBlocks = result;
    }
    if (freeBlocks >= 0) {
        strcpy(buf, "Total Memory Used == 0x");
        report_add_hex(buf, sMemUsedUnits << MEM_UNIT_SHIFT);
        str_cat(buf, "\n");
        debug_log_2(buf);

        strcpy(buf, "Total Memory Free == 0x");
        report_add_hex(buf, gMemFreeUnits << MEM_UNIT_SHIFT);
        str_cat(buf, " in ");
        report_add_dec(buf, (u16)freeBlocks);
        str_cat(buf, " block(s)\n");
        debug_log_2(buf);
    }
}

static inline void totals_add_hex(char *line, u32 value)
{
    char digits[12];
    char *out;
    u32 *divisor;
    u32 digit;
    char c;

    divisor = sHexDigitDivisors;
    out = digits;
    do {
        digit = value / *divisor;
        if ((s32)digit >= 10) c = digit + '7';
        else c = digit + '0';
        *out = c;
        value -= digit * *divisor;
        divisor++;
        out++;
    } while ((s32)divisor < (s32)&sHexDigitDivisors[8]);
    digits[8] = 0;
    str_cat(line, digits);
}

static inline void totals_add_dec(char *line, u16 value)
{
    char digits[12];
    char *out;
    u32 *divisor;
    u32 digit;
    char c;
    u32 x = value;
    s32 i;

    divisor = sDecDigitDivisors;
    out = digits;
    do {
        digit = x / *divisor;
        if ((s32)digit >= 10) c = digit + '7';
        else c = digit + '0';
        *out = c;
        x -= digit * *divisor;
        divisor++;
        out++;
    } while ((s32)divisor < (s32)&sDecDigitDivisors[5]);
    digits[5] = 0;
    for (i = 0; i < 4 && digits[i] == '0'; i++) digits[i] = ' ';
    str_cat(line, digits);
}

/* Logs the "Total Memory Used/Free" lines (debug_log) without the block list: the variable passed as
   mem_dump_blocks' print flag still holds 0. */
void mem_report_totals(void)
{
    char buf[0x50];
    s32 result;
    s32 freeBlocks;

    sprintf(buf, "\n");
    debug_log(buf);
    freeBlocks = 0;
    result = mem_dump_blocks(sMemHeapStart, sMemHeapEnd, freeBlocks);
    if (result > 0) freeBlocks = result;
    if (freeBlocks < 0) return;

    strcpy(buf, "Total Memory Used == 0x");
    totals_add_hex(buf, sMemUsedUnits << MEM_UNIT_SHIFT);
    str_cat(buf, "\n");
    debug_log(buf);

    strcpy(buf, "Total Memory Free == 0x");
    totals_add_hex(buf, gMemFreeUnits << MEM_UNIT_SHIFT);
    str_cat(buf, " in ");
    totals_add_dec(buf, freeBlocks);
    str_cat(buf, " block(s)\n");
    debug_log(buf);
}

/* Sets a header's check word. */
#define SET_CHECK(b) ((b)->check = MEM_CHECK_SUM - ((u32)(b)->next + (u32)(b)->prev + (b)->size))

/* Allocates `size` bytes for `owner` (a __FILE__ string) at `line`: the first free block that fits, or with
   MEM_ALLOC_TOP the last one, its top part taken. Returns NULL for a size of 0 or when nothing fits. */
void *mem_alloc_dbg(s32 size, u32 flags, s32 owner, s32 line)
{
    MemHeader *list;
    MemHeader *blk;
    MemHeader *nb;
    MemHeader *next;
    MemHeader *prev;
    s32 units;

    if (size == 0) {
        return 0;
    }
    list = sMemFreeList;
    units = ((u32)(size + MEM_UNIT_MASK) >> MEM_UNIT_SHIFT) + 1;
    /* From here `size` is the free block's size in units. */
    if (!(flags & MEM_ALLOC_TOP)) {
        blk = list->next;
        for (;;) {
            size = blk->size;
            if (units <= size) {
                goto found;
            }
            blk = blk->next;
            if (blk == list) {
                break;
            }
        }
    } else {
        blk = list->prev;
        for (;;) {
            size = blk->size;
            if (units <= size) {
                goto found;
            }
            blk = blk->prev;
            if (blk == list) {
                break;
            }
        }
    }
    return 0;
found:
    prev = blk->prev;
    next = blk->next;
    if (size == units) {
        prev->next = next;
        next->prev = prev;
        SET_CHECK(prev);
        SET_CHECK(next);
    } else {
        size -= units;
        if (!(flags & MEM_ALLOC_TOP)) {
            /* `list` now holds the rest of the block, which stays free. */
            list = blk + units;
            list->next = next;
            list->prev = prev;
            list->size = size;
            prev->next = list;
            next->prev = list;
            SET_CHECK(prev);
            SET_CHECK(list);
            SET_CHECK(next);
        } else {
            blk->size = size;
            blk->check = MEM_CHECK_SUM - ((u32)blk->next + (u32)blk->prev + size);
            blk += size;
        }
    }
    blk->next = (MemHeader *)owner;
    blk->size = units;
    line = (line << 16) | sMemAllocSerial | MEM_USED;
    sMemAllocSerial = (sMemAllocSerial + 2) & MEM_SERIAL_MASK;
    blk->prev = (MemHeader *)line;
    gMemFreeUnits -= units;
    blk->check = MEM_CHECK_SUM - ((u32)blk->next + (u32)blk->prev + units);
    sMemUsedUnits += units;
    if (sMemPeakUsedUnits < sMemUsedUnits) {
        sMemPeakUsedUnits = sMemUsedUnits;
    }
    return blk + 1;
}

typedef struct MemHeader2 {
    struct MemHeader2 *next;
    struct MemHeader2 *prev;
    s32 size;
    u32 check;
} MemHeader2;

/* Frees a block: checks its header and the next block's, puts it back into the free list in address order and
   merges it with the free blocks either side. A bad header, an overrun or a block not in use is logged with
   the block list. */
void mem_free(void *ptr)
{
    MemHeader2 *hdr;
    MemHeader2 *prev;
    MemHeader2 *next;
    MemHeader2 *after;
    u32 sum;
    char *msg;
    s32 size;
    u32 info;
    u32 owner;

    if (ptr == NULL) {
        return;
    }
    hdr = (MemHeader2 *)ptr - 1;
    owner = (u32)hdr->next;
    info = (u32)hdr->prev;
    size = hdr->size;
    sum = owner + info + size + hdr->check;
    if (sum != MEM_CHECK_SUM) {
        goto bad_header;
    }
    after = hdr + hdr->size;
    if ((u32)after->next + (u32)after->prev + after->size + after->check != sum) {
        goto overrun;
    }
    if (!((s32)hdr->prev & MEM_USED)) {
        goto not_used;
    }
    prev = sMemFreeList;
    hdr->prev = NULL;
    next = prev->next;
    gMemFreeUnits += hdr->size;
    sMemUsedUnits -= hdr->size;
    if (prev == next) {
        goto insert;
    }
    /* Find the free blocks either side; past the last one the list wraps around to its head. */
    for (;;) {
        if (prev < hdr && hdr < next) {
            goto insert;
        }
        if (prev >= next && (prev < hdr || hdr < next)) {
            goto insert;
        }
        prev = next;
        next = next->next;
    }
insert:
    prev->next = hdr;
    next->prev = hdr;
    hdr->prev = prev;
    hdr->next = next;
    if (prev + prev->size == hdr) {
        prev->size += hdr->size;
        prev->next = hdr->next;
        hdr->next->prev = prev;
        hdr = prev;
    }
    if (hdr + hdr->size == next) {
        hdr->size += next->size;
        hdr->next = next->next;
        next = next->next;
        next->prev = hdr;
    }
    prev->check = MEM_CHECK_SUM - ((u32)prev->next + (u32)prev->prev + prev->size);
    hdr->check = MEM_CHECK_SUM - ((u32)hdr->next + (u32)hdr->prev + hdr->size);
    next->check = MEM_CHECK_SUM - ((u32)next->next + (u32)next->prev + next->size);
    return;
bad_header:
    msg = sStrMemFreeInvalid;
    goto fail;
overrun:
    msg = sStrMemFreeOverrun;
    goto fail;
not_used:
    msg = sStrMemFreeTwice;
fail:
    debug_log_3(msg, size, sum, info);
    mem_report_2();
}

extern MemHeader **sMemFreeList_2;

/* Allocates the block at a fixed address `ptr` (its header goes in the unit below it) for `owner` at `line`, out
   of the free block that holds that range. Returns ptr, or NULL when the range is not free. */
void *mem_alloc_at_dbg(void *ptr, s32 size, s32 owner, s32 line)
{
    MemHeader *hdr;
    MemHeader *blk;
    MemHeader *first;
    MemHeader *end;
    MemHeader *limit;
    MemHeader *next;
    MemHeader *prev;
    u32 units;

    if (size == 0) {
        return 0;
    }
    hdr = (MemHeader *)((u32)ptr & ~MEM_UNIT_MASK) - 1;
    do {
        units = (u32)ptr - (u32)hdr;
        units = (units + size + MEM_UNIT_MASK) >> MEM_UNIT_SHIFT;
    } while (0);
    limit = hdr + units;
    first = blk = *sMemFreeList_2;
    for (;;) {
        size = blk->size;
        if (hdr < blk || (end = blk + size) < limit) {
            blk = blk->next;
            if (blk == first) {
                break;
            }
        } else {
            /* `first` now holds what stays free above the new block, `size` its units. */
            first = hdr + units;
            size = end - first;
            next = blk->next;
            prev = blk->prev;
            if (size > 0) {
                blk->next = first;
                next->prev = first;
                SET_CHECK(next);
                first->next = next;
                first->prev = blk;
                first->size = size;
                next = first;
            }
            if (blk < hdr) {
                /* What stays free below. */
                size = hdr - blk;
                blk->size = size;
                SET_CHECK(blk);
            } else {
                prev->next = next;
                next->prev = prev;
            }
            do {
                size = sMemAllocSerial;
                line = (line << 16) | size | MEM_USED;
                size += 2;
                size &= MEM_SERIAL_MASK;
                sMemAllocSerial = size;
                SET_CHECK(prev);
            } while (0);
            {
                /* The ((u16)nextPrev >> 16) term is always 0; the form gives the original's code. */
                s32 freeUnits;
                u32 nextPrev;
                freeUnits = gMemFreeUnits;
                nextPrev = (u32)next->prev;
                gMemFreeUnits = freeUnits - units;
                next->check = MEM_CHECK_SUM - ((u32)next->next + nextPrev + next->size + ((u16)nextPrev >> 16));
            }
            hdr->next = (MemHeader *)owner;
            hdr->prev = (MemHeader *)line;
            hdr->size = units;
            SET_CHECK(hdr);
            sMemUsedUnits += units;
            if (sMemPeakUsedUnits < sMemUsedUnits) {
                sMemPeakUsedUnits = sMemUsedUnits;
            }
            return ptr;
        }
    }
    return 0;
}

/* A used block's header as the block list reads it. */
typedef struct {
    char *file;     /* the owner */
    u32 info;       /* (line << 16) | serial | MEM_USED */
    u32 size;
    u32 check;
} MemHeaderInfo;

static inline void dump_add_hex(char *line, u32 value)
{
    char digits[12];
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 digit = value / sHexDigitDivisors[i];
        if (digit < 10) digits[i] = digit + '0';
        else digits[i] = digit + '7';
        value -= digit * sHexDigitDivisors[i];
    }
    digits[8] = 0;
    str_cat(line, digits);
}

static inline void dump_add_dec(char *line, u32 value)
{
    char digits[12];
    s32 i;

    value &= 0xFFFF;
    for (i = 0; i < 5; i++) {
        s32 digit = value / sDecDigitDivisors[i];
        if (digit < 10) digits[i] = digit + '0';
        else digits[i] = digit + '7';
        value -= digit * sDecDigitDivisors[i];
    }
    digits[5] = 0;
    for (i = 0; i < 4; i++) {
        if (digits[i] != '0') break;
        digits[i] = ' ';
    }
    str_cat(line, digits);
}

/* Walks the heap from `start` to `end` and, if `verbose`, logs a line per block: free blocks, and used blocks
   allocated since the report mark with their serial ("Time"), line and source file name. Returns the number of free
   blocks, or -1 at a corrupt header. */
s32 mem_dump_blocks(s32 start, u32 end, s32 verbose)
{
    char buf[0x50];
    s32 count;
    MemHeaderInfo *hdr;

    char *name;
    char *file;

    sprintf(buf, "\n");
    debug_log_2(buf);
    count = 0;
    if (start == 0) {
        return count;
    }

    /* The first two blocks are the free list's head and the used block after it (mem_format_heap). */
    for (hdr = (MemHeaderInfo *)(start + 0x20); (u32)hdr < end;
         hdr = (MemHeaderInfo *)((u8 *)hdr + (hdr->size << MEM_UNIT_SHIFT))) {
        if ((u32)hdr->file + hdr->info + hdr->size + hdr->check != MEM_CHECK_SUM) {
            strcpy(buf, " Addr 0x");
            dump_add_hex(buf, (u32)hdr);
            str_cat(buf, " Size ?????????? - Corrupt\n");
            if (verbose) {
                debug_log_2(buf);
            }
            count = -1;
            break;
        }
        if (!(hdr->info & MEM_USED)) {
            strcpy(buf, " Addr 0x");
            dump_add_hex(buf, (u32)hdr);
            str_cat(buf, " Size 0x");
            dump_add_hex(buf, hdr->size << MEM_UNIT_SHIFT);
            str_cat(buf, " - Free\n");
            if (verbose) {
                debug_log_2(buf);
            }
            count++;
            continue;
        }
        if ((hdr->info & MEM_SERIAL_MASK) < sMemReportMark) {
            continue;
        }
        file = hdr->file;
        if (file == 0) {
            strcpy(buf, " Addr 0x");
            dump_add_hex(buf, (u32)hdr);
            str_cat(buf, " Size 0x");
            dump_add_hex(buf, hdr->size << MEM_UNIT_SHIFT);
            str_cat(buf, " - Time ");
            dump_add_dec(buf, (hdr->info & MEM_SERIAL_MASK) >> 1);
        } else {
            /* The file name without its directories. */
            name = file;
            while (*name++) {
            }
            for (name--; name >= file; name--) {
                if (*name == '\\' || *name == '/') break;
            }
            name++;
            strcpy(buf, " Addr 0x");
            dump_add_hex(buf, (u32)hdr);
            str_cat(buf, " Size 0x");
            dump_add_hex(buf, hdr->size << MEM_UNIT_SHIFT);
            str_cat(buf, " - Time ");
            dump_add_dec(buf, (hdr->info & MEM_SERIAL_MASK) >> 1);
            str_cat(buf, " Line ");
            dump_add_dec(buf, *(u16 *)&hdr->info);
            str_cat(buf, " File ");
            str_cat(buf, name);
        }
        str_cat(buf, "\n");
        if (verbose) {
            debug_log_2(buf);
        }
    }
    sprintf(buf, "\n");
    if (verbose) {
        debug_log_2(buf);
    }
    return count;
}

/* The block list shows only the used blocks allocated after the mark (by serial). */
void mem_report_clear_mark(void)
{
    sMemReportMark = 0;
}

void mem_report_set_mark(void)
{
    sMemReportMark = sMemAllocSerial;
}

/* Sets up the heap over `size` bytes at `start`. The first two arguments are not used. */
s32 mem_init(s32 unused0, s32 unused1, s32 start, s32 size)
{
    s32 end;
    s32 *startp;
    s32 *endp;

    startp = &sMemHeapStart;
    endp = &sMemHeapEnd;
    sMemPeakUsedUnits = 0;
    sMemUsedUnits = 0;
    gMemFreeUnits = 0;
    sMemAllocSerial = 2;
    *startp = start;
    end = start + size;
    sMemHeapEnd = end;
    sMemFreeList = mem_format_heap(startp, endp);
    gMemFreeUnits = gMemFreeUnits + ((sMemHeapEnd - sMemHeapStart) >> MEM_UNIT_SHIFT);
    return 0;
}

/* mem_alloc_dbg, then clears the block 16 bytes at a time. */
s32 *mem_calloc_dbg(s32 size, s32 flags, s32 owner, s32 line)
{
    s32 *mem = 0;
    s32 units;
    if (size == 0) {
        return 0;
    }
    mem = mem_alloc_dbg(size, flags, owner, line);
    if (mem != 0) {
        s32 *dst = mem;
        units = mem[-2] - 1; /* the header's size, less the header */
        do {
            dst[0] = 0;
            dst[1] = 0;
            dst[2] = 0;
            dst[3] = 0;
            dst += 4;
            units--;
        } while (units != 0);
    }
    return mem;
}

/* Resizes a block: a size of 0 frees it; a smaller size frees the rest in place; a larger one allocates a new block,
   copies and frees the old one. The copy takes old size * 4 words, header unit included, so it reads 16 bytes past
   the old block's data. */
s32 *mem_realloc_dbg(s32 *ptr, u32 size, s32 flags, s32 owner, s32 line)
{
    Blk *hdr;
    Blk *rest;
    u32 units;
    u32 oldUnits;
    s32 *newMem;
    s32 *src;
    s32 *dst;
    s32 cnt;

    if (size == 0) {
        if (ptr == 0) {
            return 0;
        }
        mem_free(ptr);
        return 0;
    }
    if (ptr != 0) {
        /* w[0..3]: next, prev, size, check */
        hdr = (Blk *)ptr - 1;
        units = ((size + MEM_UNIT_MASK) >> MEM_UNIT_SHIFT) + 1;
        oldUnits = hdr->w[2];
        if (units == oldUnits) {
            return ptr;
        }
        if (units <= oldUnits) {
            /* The rest becomes a used block without owner, then is freed. */
            rest = hdr + units;
            rest->w[2] = oldUnits - units;
            rest->w[0] = 0;
            rest->w[1] = MEM_USED;
            {
                s32 sum = rest->w[0] + 1;
                rest->w[3] = MEM_CHECK_SUM - (sum + rest->w[2]);
            }
            hdr->w[2] = units;
            hdr->w[3] = MEM_CHECK_SUM - (hdr->w[0] + hdr->w[1] + units);
            mem_free(rest + 1);
            return ptr;
        }
    }
    newMem = mem_alloc_dbg(size, flags, owner, line);
    if (newMem != 0 && ptr != 0) {
        src = ptr;
        dst = newMem;
        units = ((Blk *)ptr - 1)->w[2] * 4;
        while (--units != -1) {
            *dst++ = *src++;
        }
        mem_free(ptr);
    }
    return newMem;
}

/* Validates the heap and hangs if it is corrupt. */
s32 mem_check(void)
{
    s32 result = mem_validate(sMemHeapStart, sMemHeapEnd, sMemFreeList);

    if (result < 0) {
        while (1) {
        }
    }
    return 0;
}

/* Returns the size in bytes a block can hold, or -1 (logged with the block list) for a bad header, an overrun or a
   block not in use. */
s32 mem_get_size(s32 *ptr)
{
    s32 *hdr = ptr - 4;
    s32 *after;
    s32 sum;
    char *msg;

    sum = hdr[0] + hdr[1] + hdr[2] + hdr[3];
    if (sum != MEM_CHECK_SUM) {
        goto bad_head;
    }
    after = hdr + hdr[2] * 4;
    if (after[0] + after[1] + after[2] + after[3] != MEM_CHECK_SUM) {
        goto bad_tail;
    }
    if (hdr[1] & MEM_USED) {
        return (hdr[2] - 1) * 16;
    }
    goto bad_free;
bad_head:
    msg = sStrMemSizeInvalid;
    goto fail;
bad_tail:
    msg = sStrMemSizeOverrun;
    goto fail;
bad_free:
    msg = sStrMemSizeFreed;
fail:
    debug_log_3(msg);
    mem_report_2();
    return -1;
}

/* The allocation functions without an owner and line. They are declared void: the result is what the call leaves in
   $v0. */
void mem_alloc(s32 size, s32 flags)
{
    mem_alloc_dbg(size, flags, 0, 0);
}

void mem_calloc(u32 size, u32 flags)
{
    mem_calloc_dbg(size, flags, 0, 0);
}

void mem_realloc(s32 *ptr, s32 size, s32 flags)
{
    mem_realloc_dbg(ptr, size, flags, 0, 0);
}

void mem_alloc_at(s32 ptr, s32 size)
{
    mem_alloc_at_dbg(ptr, size, 0, 0);
}

#define BLOCK_SUM(b) ((u32)(b)->next + (b)->unk4 + (b)->size)

/* Aligns the heap's ends to 16 bytes and lays it out: the free list's head (first[0], one unit), a used one-unit
   block (first[1]) that keeps the head from merging, one free block with the rest (first[2]) and at the top an end
   block of size 0 linked to itself. Returns the head, or NULL when the heap has fewer than 4 units. */
Block *mem_format_heap(Block **start, Block **end)
{
    Block *first = *start;
    Block *last = *end;
    u32 units;

    if (first != NULL) {
        first = (Block *)(((u32)first + MEM_UNIT_MASK) & ~MEM_UNIT_MASK);
        last = (Block *)((u32)last & ~MEM_UNIT_MASK) - 1;
        units = last - first;
        if (units < 4) {
            return NULL;
        }
        *start = first;
        *end = last;
        last->next = last;
        last->unk4 = (s32)last;
        last->size = 0;
        last->check = MEM_CHECK_SUM - BLOCK_SUM(last);
        first[0].next = &first[2];
        first[0].unk4 = (s32)&first[2];
        first[0].size = 1;
        first[0].check = MEM_CHECK_SUM - BLOCK_SUM(&first[0]);
        first[1].next = NULL;
        first[1].unk4 = MEM_USED;
        first[1].size = 1;
        first[1].check = MEM_CHECK_SUM - BLOCK_SUM(&first[1]);
        first[2].next = first;
        first[2].unk4 = (s32)first;
        first[2].size = units - 2;
        first[2].check = MEM_CHECK_SUM - BLOCK_SUM(&first[2]);
    }
    return first;
}

#define BLOCK_SUM_2(b) ((u32)(b)->next + (b)->unk4 + (b)->size + (b)->check)

/* The most free-list entries mem_validate follows. */
#define MEM_VALIDATE_MAX_FREE 0x400

/* Checks every header from `start` to `end` and follows the free list from `list` (at most MEM_VALIDATE_MAX_FREE
   steps, each block inside the heap and aligned). Returns -1 when something is wrong, else the steps taken: the
   number of free blocks plus one. */
s32 mem_validate(Block *start, Block *end, Block *list)
{
    Block *blk;
    s32 left = MEM_VALIDATE_MAX_FREE;

    if (start != NULL) {
        blk = start;
        do {
            if (blk >= end) {
                goto final;
            }
            if (BLOCK_SUM_2(blk) != MEM_CHECK_SUM) {
                return -1;
            }
            blk += blk->size;
        } while (blk >= start);
    final:
        if (BLOCK_SUM_2(blk) != MEM_CHECK_SUM) {
            return -1;
        }
        blk = list->next;
        while (--left != -1) {
            if (blk == list) {
                goto done;
            }
            if (blk < start || blk >= end || ((u32)blk & MEM_UNIT_MASK) != 0) {
                break;
            }
            blk = blk->next;
        }
        if (blk != list) {
            return -1;
        }
    }
done:
    return MEM_VALIDATE_MAX_FREE - left;
}

/* Appends `value` to `dst` as 8 hex digits. */
void str_cat_hex(char *dst, u32 value)
{
    u32 *divisor;
    char *out;
    char digits[9];
    s32 digit;

    divisor = sHexDigitDivisors;
    out = digits;
    do {
        digit = (s32)(value / *divisor);
        if (digit < 10) {
            *out = (char)(digit + '0');
        } else {
            *out = (char)(digit + '7');
        }
        value = value - digit * *divisor;
        divisor = divisor + 1;
        out = out + 1;
    } while ((s32)divisor < (s32)(sHexDigitDivisors + 8));
    digits[8] = 0;
    str_cat(dst, digits);
}
