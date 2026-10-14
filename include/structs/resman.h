/* Resource sets (resman.c). A set "<name>" is three files: "<name>.dir" holds the file offset of each resource in
   "<name>.BIG", "<name>.gid" its names (16 characters each). In the .BIG file a resource is a 4-character type, its
   size, its data, then a count and that many offsets into the data of words that hold another resource's index:
   res_load makes them point to that resource's handle and loads it too. */
#ifndef STRUCTS_RESMAN_H
#define STRUCTS_RESMAN_H

#include "common.h"

/* A name in a set's hash table (res_load_by_name). */
typedef struct ResName {
    /* 0x00 */ struct ResName *next;
    /* 0x04 */ char *name;
    /* 0x08 */ u32 id;
} ResName; /* size 0xC */

typedef struct ResSet {
    /* 0x000 */ s32 count;
    /* 0x004 */ u32 *handles;      /* per resource: its data or 0; bit 1: the data is in a load buffer (not freed);
                                      (depth << 2) | 1: waiting to be loaded by res_load at that depth */
    /* 0x008 */ ResName *names[256]; /* by res_hash_name */
    /* 0x408 */ u32 *offsets;      /* the .dir file */
    /* 0x40C */ char name[0x80];
    /* 0x48C */ char bigPath[0x80];
    /* 0x50C */ u32 depth;         /* nesting of res_load */
    /* 0x510 */ u32 unk_510;
    /* 0x514 */ u32 inLoadBuffer;  /* bit 0: the set itself is in a load buffer (not freed) */
    /* 0x518 */ u32 data[1];       /* handles[count], count unused words, offsets[count] */
} ResSet;

/* A resource type registered with res_register_type: the function run on each resource loaded of that type. */
typedef struct ResType {
    /* 0x00 */ char ext[3];        /* the type's first 3 characters, any case */
    /* 0x03 */ u8 pad03;
    /* 0x04 */ void (*relocate)(u32 *handle);
} ResType; /* size 0x8 */

#endif
