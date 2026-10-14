/* The ROM file system (file.c, FILE.C): a directory of files after the game image, read through 8 KB pages. */
#ifndef STRUCTS_FILE_H
#define STRUCTS_FILE_H

#include "common.h"

/* A directory entry (sFileDir, sFileDirCount of them). */
typedef struct FileDirEntry {
    /* 0x00 */ char name[16];
    /* 0x10 */ u32 flags;           /* 1: 'sWd' file, SWD-compressed per 8 KB page */
    /* 0x14 */ s32 size;            /* bytes in ROM */
    /* 0x18 */ u8 *rom;             /* ROM address; an offset from sFileDataBase until file_init adds it */
    /* 0x1C */ u32 unpackedSize;
} FileDirEntry; /* size 0x20 */

/* A cached page, in the most-recently-used list from sFilePageHead to sFilePageTail (whose buffers are NULL). */
typedef struct FilePage {
    /* 0x00 */ struct FilePage *next;
    /* 0x04 */ struct FilePage *prev;
    /* 0x08 */ u8 *buf;             /* 0x2000 bytes */
    /* 0x0C */ FileDirEntry *file;  /* the file it holds a page of, NULL: free */
    /* 0x10 */ s32 page;            /* page index; for a compressed file's header page (-1) the number of open
                                       handles that use it, counted down from -1 */
} FilePage; /* size 0x14 */

/* An open file (sFileHandles, 4): file_open returns one. */
typedef struct FileHandle {
    /* 0x00 */ FileDirEntry *file;  /* NULL: the handle is free */
    /* 0x04 */ FilePage *header;    /* a compressed file's header page (its page offsets) */
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ u32 pos;
} FileHandle; /* size 0x10 */

#endif
