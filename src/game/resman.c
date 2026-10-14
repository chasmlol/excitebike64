#include "game.h"

/* Opens the resource set `path`: reads "<path>.dir" into a new ResSet (or the load buffer), byte-swapping the
   offsets when the file's byte order differs. Returns 0, 1 when the file is missing or 2 when out of memory. */
s32 res_open_set(char *path, ResSet **out)
{
    char dirPath[128];
    s32 word;
    FileHandle *file;
    u32 size;
    u32 count;
    ResSet *set;
    u32 j;
    u8 *p;
    u8 tmp;

    /* the byte order of the machine: which end of a word its first byte is */
    word = 0;
    *(u8 *)&word = 0xFF;
    if (word == 0xFF) {
        sResSwapBytes = 0;
    } else if (word == (s32)0xFF000000) {
        sResSwapBytes = 1;
    } else {
        debug_printf(sStrResConfused);
        fatal_halt(1);
    }

    str_copy(dirPath, path);
    str_cat(dirPath, sStrDirExt);
    file = file_open(dirPath, sStrResModeRb);
    if (file == NULL) {
        return 1;
    }
    file_seek(file, 0, 2); /* the end */
    size = file_tell(file);
    file_seek(file, 0, 0);
    if (sResLoadBuffer == NULL) {
        count = size >> 2;
        /* the header (up to ResSet.data), then three words per resource */
        set = mem_alloc_dbg(count * 12 + 0x518, 0, sStrResmanC, 0xB0);
        if (set == NULL) {
            return 2;
        }
        set->inLoadBuffer = 0;
    } else {
        count = size >> 2;
        set = sResLoadBuffer;
        set->inLoadBuffer = 1;
        sResLoadBuffer = NULL;
    }
    res_set_clear(set);
    set->count = size >> 2;
    if (size >> 2 == 0) {
        return 2;
    }
    set->handles = set->data;
    mem_fill(set->data, 0, size);
    set->offsets = set->data + count * 2;
    file_read(set->data + count * 2, 1, size, file);

    if (sResSwapBytes != 0) {
        word = 0;
        if (set->count > 0) {
            do {
                j = 0;
                do {
                    p = (u8 *)(word * 4 + (s32)set->offsets);
                    tmp = p[j];
                    p[j] = p[3 - j];
                    ((u8 *)((s32)set->offsets + word * 4))[3 - j] = tmp;
                    j++;
                } while (j < 2);
                word++;
            } while (word < set->count);
        }
    }
    file_close(file);
    str_copy(set->name, path);
    sprintf(set->bigPath, sStrBigFmt, path);
    *out = set;
    return 0;
}

/* Reverses the bytes of a 32-bit value read from a set of the other byte order. */
#define SWAP_BYTES(x) { if (sResSwapBytes != 0) { u32 _i; for (_i = 0; _i < 2; _i++) { u8 _t = ((u8 *)&(x))[_i]; ((u8 *)&(x))[_i] = ((u8 *)&(x))[3 - _i]; ((u8 *)&(x))[3 - _i] = _t; } } }

/* Loads resource `id` of a set (or returns it when loaded): reads it from the .BIG file, points its references at
   the handles of the resources they name, loads those, then runs its type's relocation. Returns the data or 0. */
void *res_load(ResSet *set, s32 id)
{
    FileHandle *file;
    s32 *data;
    u32 handle;
    u32 i;
    u32 *ref;
    s32 off;
    s32 refId;
    s32 inBuffer;
    u32 size;
    u8 type[4];
    u8 *path;
    s32 typeChar;
    s32 fileChar;

    if (gResLoadCallback != NULL) {
        gResLoadCallback();
    }
    handle = set->handles[id];
    path = set->bigPath;
    if (handle & RES_PENDING) {
        goto inc;
    }
    if (handle != 0) {
        return (void *)handle;
    }
inc:
    set->depth = set->depth + 1;
    file = file_open(path, sStrResModeRb);
    if (file == 0) {
        goto dec0;
    }
    file_seek(file, set->offsets[id], 0);
    file_read(type, 1, 4, file);
    file_read((u8 *)&size, 1, 4, file);
    SWAP_BYTES(size);
    if (sResLoadBuffer != 0) {
        data = sResLoadBuffer;
        inBuffer = 1;
        sResLoadBuffer = 0;
    } else {
        inBuffer = 0;
        data = mem_alloc_dbg(size, 0, sStrResmanC, 0x1cd);
    }
    if (((u32)data & 3) != 0) {
        debug_printf(sStrUnalignedMalloc, data);
        file_close(file);
        return 0;
    }
    if (data == 0) {
        set->depth = set->depth - 1;
        file_close(file);
        return 0;
    }
    off = file_read((u8 *)data, 1, size, file);
    if (off == (s32)size) {
        goto main_path;
    }
    mem_free(data);
    file_close(file);
dec0:
    set->depth = set->depth - 1;
    return 0;

main_path:
    /* the references to other resources */
    file_read((u8 *)&size, 1, 4, file);
    SWAP_BYTES(size);
    for (i = 0; i < size; i++) {
        file_read((u8 *)&off, 1, 4, file);
        SWAP_BYTES(off);
        refId = *(s32 *)((u8 *)data + off);
        SWAP_BYTES(refId);
        *(u32 *)((u8 *)data + off) = (u32)&set->handles[refId];
        ref = (u32 *)(refId * 4 + (u32)set->handles);
        if (*ref == 0) {
            *ref = (set->depth << 2) | RES_PENDING;
        }
    }
    if (inBuffer != 0) {
        set->handles[id] = (u32)data | RES_IN_BUFFER;
    } else {
        set->handles[id] = (u32)data;
    }
    file_close(file);
    for (i = 0; i < set->count; i++) {
        if (set->handles[i] == ((set->depth << 2) | RES_PENDING)) {
            res_load(set, i);
            i = 0xffffffff; /* start over */
        }
    }
    for (i = 0; i < RES_MAX_TYPES; i++) {
        typeChar = char_to_upper(sResTypeHandlers[i].ext[0]);
        fileChar = char_to_upper(type[0]);
        if (typeChar == fileChar) {
            typeChar = char_to_upper(sResTypeHandlers[i].ext[1]);
            fileChar = char_to_upper(type[1]);
            if (typeChar == fileChar) {
                typeChar = char_to_upper(sResTypeHandlers[i].ext[2]);
                fileChar = char_to_upper(type[2]);
                if (typeChar == fileChar) {
                    sResTypeHandlers[i].relocate(&set->handles[id]);
                    break;
                }
            }
        }
    }
    data = (s32 *)set->handles[id];
    set->depth = set->depth - 1;
    return data;
}

/* Registers the function run on each resource loaded whose type starts with `ext` (3 characters, any case);
   registering a type again replaces its function. */
void res_register_type(char *ext, void (*relocate)(u32 *handle))
{
    s32 i;

    if (sResTypesInit == 0) {
        mem_fill((u8 *)sResTypeHandlers, 0, RES_MAX_TYPES * sizeof(ResType));
        sResTypesInit = 1;
    }
    for (i = 0; i < RES_MAX_TYPES; i++) {
        if (char_to_upper(sResTypeHandlers[i].ext[0]) == char_to_upper(ext[0]) &&
            char_to_upper(sResTypeHandlers[i].ext[1]) == char_to_upper(ext[1]) &&
            char_to_upper(sResTypeHandlers[i].ext[2]) == char_to_upper(ext[2])) {
            break;
        }
    }
    if (i == RES_MAX_TYPES) {
        for (i = 0; i < RES_MAX_TYPES; i++) {
            if (sResTypeHandlers[i].relocate == 0) {
                break;
            }
        }
        if (i == RES_MAX_TYPES) {
            debug_printf((s32)sStrTooManyResTypes);
            fatal_halt(1);
        }
    }
    __builtin_memcpy(sResTypeHandlers[i].ext, ext, 3);
    sResTypeHandlers[i].relocate = relocate;
}

/* Empties a set's header and name table. */
void res_set_clear(ResSet *set)
{
    s32 i = 0xFF;

    set->depth = 0;
    set->offsets = 0;
    set->count = 0;
    do {
        set->names[i] = NULL;
        i--;
    } while (i >= 0);
    set->handles = 0;
    set->unk_510 = 0;
}

/* Unloads every resource of a set, frees its names and the set. */
void res_close_set(ResSet *set)
{
    s32 i;
    ResName *entry;
    ResName *next;

    i = 0;
    if (set->count > 0) {
        do {
            res_unload(&set->handles[i]);
            i = i + 1;
        } while (i < set->count);
    }
    i = 0;
    do {
        entry = set->names[i];
        while (entry != 0) {
            next = entry->next;
            mem_free(entry->name);
            mem_free(entry);
            entry = next;
        }
        i = i + 1;
    } while (i < 0x100);
    if ((set->inLoadBuffer & 1) == 0) {
        mem_free(set);
    }
}

/* Frees a resource's data (unless it is in a load buffer) and clears its handle. */
void res_unload(u32 *handle) {
    u32 data = *handle;
    if (data != 0) {
        if ((data & RES_IN_BUFFER) == 0) {
            mem_free(data);
            *handle = 0;
        } else {
            *handle = 0;
        }
    }
}

/* Unloads every resource of a set. */
void res_unload_all(ResSet *set)
{
    s32 i;
    u32 *handle;
    i = 0;
    while (i < set->count) {
        handle = (u32 *)((i << 2) + (s32)set->handles);
        if (*handle != 0) {
            if ((*handle & RES_IN_BUFFER) == 0) {
                mem_free(*handle);
            }
            *handle = 0;
        }
        i++;
    }
}

/* Loads a resource by name from the set's name table (halts when it is not there); its index goes to *id. */
void *res_load_by_name(ResSet *set, char *name, u32 *id)
{
    s32 i = 0;
    u32 hash = 0;
    u32 len;
    ResName *entry;

    /* res_hash_name */
    for (;;) {
        len = strlen(name);
        if (!((u32)i < len)) break;
        if (!(i < 16)) break;
        hash += char_to_upper(name[i]);
        i++;
    }
    entry = set->names[hash & 0xFF];
    if (entry != NULL) {
        do {
            if (str_icmp(entry->name, name) == 0) break;
            entry = entry->next;
        } while (entry != NULL);
        if (entry != NULL) goto found;
    }
    debug_printf((s32)sStrNoResource, name);
    fatal_halt(1);
found:
    if (id != NULL) *id = entry->id;
    return res_load(set, entry->id);
}

/* The index of a resource name in the set's ".gid" file, or -1. */
s32 res_find_gid(ResSet *set, char *name)
{
    u8 buf[0x50];
    s32 count;
    FileHandle *file;
    u32 k;
    s32 i;

    str_copy((char *)buf, set->name);
    str_cat((char *)buf, sStrGidExt);
    file = file_open(buf, sStrResModeRb);
    if (file == 0) {
        return -1;
    }
    file_read((u8 *)&count, 1, 4, file);
    k = 0;
    if (sResSwapBytes != 0) {
        do {
            u8 t1 = ((u8 *)&count)[k];
            u8 t2 = ((u8 *)&count)[3 - k];
            ((u8 *)&count)[k] = t2;
            ((u8 *)&count)[3 - k] = t1;
            k++;
        } while (k < 2);
    }
    i = 0;
    if (count > 0) {
        do {
            file_read(buf, 1, 0x10, file);
            if (str_icmp(name, buf) == 0) {
                file_close(file);
                return i;
            }
            i++;
        } while (i < count);
    }
    file_close(file);
    return -1;
}

/* The size of a resource's data, read from the .BIG file, or -1. */
s32 res_get_size(ResSet *set, s32 id) {
    FileHandle *file;
    u8 type[8];
    s32 size;
    u32 i;

    file = file_open(set->bigPath, "rb");
    if (file == NULL) {
        return -1;
    }
    file_seek(file, set->offsets[id], 0);
    file_read(type, 1, 4, file);
    file_read(&size, 1, 4, file);
    if (sResSwapBytes != 0) {
        for (i = 0; i < 2; i++) {
            u8 t = ((u8 *)&size)[i];

            ((u8 *)&size)[i] = ((u8 *)&size)[3 - i];
            ((u8 *)&size)[3 - i] = t;
        }
    }
    file_close(file);
    return size;
}

/* Makes the next set or resource load into `buf` instead of a new allocation. */
void res_set_load_buffer(void *buf)
{
    sResLoadBuffer = buf;
}

/* Meant to unload the resource whose data is `data`, but compares the handle table's address, not each handle. */
void res_unload_by_data(ResSet *set, u32 data)
{
    s32 i;
    u32 *handle;
    u32 value;

    if (data == 0) {
        return;
    }
    i = 0;
    if (set->count <= 0) {
        return;
    }
    do {
        if (((u32)set->handles & ~3) == (data & ~3)) {
            handle = (u32 *)(i * 4 + (u32)set->handles);
            value = *handle;
            if (value == 0) {
                return;
            }
            if ((value & RES_IN_BUFFER) == 0) {
                mem_free(value);
            }
            *handle = 0;
            return;
        }
        do { i++; } while (0);
    } while (i < set->count);
}

/* A name's slot in a set's name table: the sum of its first 16 characters in upper case, modulo 256. */
u32 res_hash_name(char *name)
{
    u32 i = 0;
    u32 sum = 0;
    u32 len;
    u8 c;

    for (;;) {
        len = strlen(name);
        if (len <= i || (s32)i >= 16) {
            break;
        }
        c = name[i];
        i = i + 1;
        sum = sum + char_to_upper(c);
    }
    return sum & 0xFF;
}
