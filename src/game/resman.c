#include "game.h"

s32 res_open_set(char *path, ResSet **out)
{
    char buf[128];
    s32 flag;
    s32 *f;
    u32 n;
    u32 cnt;
    ResSet *o;
    u32 j;
    u8 *p;
    u8 t;

    flag = 0;
    *(u8 *)&flag = 0xFF;
    if (flag == 0xFF) {
        sResSwapBytes = 0;
    } else if (flag == (s32)0xFF000000) {
        sResSwapBytes = 1;
    } else {
        debug_printf(D_800DB420);
        fatal_halt(1);
    }

    str_copy(buf, path);
    str_cat(buf, D_800DB434);
    f = file_open(buf, D_800DB43C);
    if (f == NULL) {
        return 1;
    }
    file_seek(f, 0, 2);
    n = file_tell(f);
    file_seek(f, 0, 0);
    if (sResLoadBuffer == NULL) {
        cnt = n >> 2;
        o = mem_alloc_dbg(cnt * 12 + 0x518, 0, D_800DB440, 0xB0);
        if (o == NULL) {
            return 2;
        }
        o->inLoadBuffer = 0;
    } else {
        cnt = n >> 2;
        o = sResLoadBuffer;
        o->inLoadBuffer = 1;
        sResLoadBuffer = NULL;
    }
    res_set_clear(o);
    o->count = n >> 2;
    if (n >> 2 == 0) {
        return 2;
    }
    o->handles = o->data;
    mem_fill(o->data, 0, n);
    o->offsets = o->data + cnt * 2;
    file_read(o->data + cnt * 2, 1, n, f);

    if (sResSwapBytes != 0) {
        flag = 0;
        if (o->count > 0) {
            do {
                j = 0;
                do {
                    p = (u8 *)(flag * 4 + (s32)o->offsets);
                    t = p[j];
                    p[j] = p[3 - j];
                    ((u8 *)((s32)o->offsets + flag * 4))[3 - j] = t;
                    j++;
                } while (j < 2);
                flag++;
            } while (flag < o->count);
        }
    }
    file_close(f);
    str_copy(o->name, path);
    sprintf(o->bigPath, D_800DB44C, path);
    *out = o;
    return 0;
}

#define SWAP_BYTES(x) { if (sResSwapBytes != 0) { u32 _i; for (_i = 0; _i < 2; _i++) { u8 _t = ((u8 *)&(x))[_i]; ((u8 *)&(x))[_i] = ((u8 *)&(x))[3 - _i]; ((u8 *)&(x))[3 - _i] = _t; } } }

void *res_load(ResSet *set, s32 id)
{
    s32 *f;
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
    s32 c1;
    s32 c2;

    if (gResLoadCallback != NULL) {
        gResLoadCallback();
    }
    handle = set->handles[id];
    path = set->bigPath;
    if (handle & 1) {
        goto inc;
    }
    if (handle != 0) {
        return (void *)handle;
    }
inc:
    set->depth = set->depth + 1;
    f = file_open(path, D_800DB43C);
    if (f == 0) {
        goto dec0;
    }
    file_seek(f, set->offsets[id], 0);
    file_read(type, 1, 4, f);
    file_read((u8 *)&size, 1, 4, f);
    SWAP_BYTES(size);
    if (sResLoadBuffer != 0) {
        data = sResLoadBuffer;
        inBuffer = 1;
        sResLoadBuffer = 0;
    } else {
        inBuffer = 0;
        data = mem_alloc_dbg(size, 0, D_800DB440, 0x1cd);
    }
    if (((u32)data & 3) != 0) {
        debug_printf(D_800DB464, data);
        file_close(f);
        return 0;
    }
    if (data == 0) {
        set->depth = set->depth - 1;
        file_close(f);
        return 0;
    }
    off = file_read((u8 *)data, 1, size, f);
    if (off == (s32)size) {
        goto main_path;
    }
    mem_free(data);
    file_close(f);
dec0:
    set->depth = set->depth - 1;
    return 0;

main_path:
    /* the references to other resources */
    file_read((u8 *)&size, 1, 4, f);
    SWAP_BYTES(size);
    for (i = 0; i < size; i++) {
        file_read((u8 *)&off, 1, 4, f);
        SWAP_BYTES(off);
        refId = *(s32 *)((u8 *)data + off);
        SWAP_BYTES(refId);
        *(u32 *)((u8 *)data + off) = (u32)&set->handles[refId];
        ref = (u32 *)(refId * 4 + (u32)set->handles);
        if (*ref == 0) {
            *ref = (set->depth << 2) | 1;
        }
    }
    if (inBuffer != 0) {
        set->handles[id] = (u32)data | 2;
    } else {
        set->handles[id] = (u32)data;
    }
    file_close(f);
    for (i = 0; i < set->count; i++) {
        if (set->handles[i] == ((set->depth << 2) | 1)) {
            res_load(set, i);
            i = 0xffffffff;
        }
    }
    for (i = 0; i < 16; i++) {
        c1 = char_to_upper(sResTypeHandlers[i].ext[0]);
        c2 = char_to_upper(type[0]);
        if (c1 == c2) {
            c1 = char_to_upper(sResTypeHandlers[i].ext[1]);
            c2 = char_to_upper(type[1]);
            if (c1 == c2) {
                c1 = char_to_upper(sResTypeHandlers[i].ext[2]);
                c2 = char_to_upper(type[2]);
                if (c1 == c2) {
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

void res_register_type(char *ext, void (*relocate)(u32 *handle))
{
    s32 i;

    if (sResTypesInit == 0) {
        mem_fill((u8 *)sResTypeHandlers, 0, 0x80);
        sResTypesInit = 1;
    }
    for (i = 0; i < 16; i++) {
        if (char_to_upper(sResTypeHandlers[i].ext[0]) == char_to_upper(ext[0]) &&
            char_to_upper(sResTypeHandlers[i].ext[1]) == char_to_upper(ext[1]) &&
            char_to_upper(sResTypeHandlers[i].ext[2]) == char_to_upper(ext[2])) {
            break;
        }
    }
    if (i == 16) {
        for (i = 0; i < 16; i++) {
            if (sResTypeHandlers[i].relocate == 0) {
                break;
            }
        }
        if (i == 16) {
            debug_printf((s32)D_800DB47C);
            fatal_halt(1);
        }
    }
    __builtin_memcpy(sResTypeHandlers[i].ext, ext, 3);
    sResTypeHandlers[i].relocate = relocate;
}

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

void res_close_set(ResSet *set)
{
    s32 i;
    ResName *n;
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
        n = set->names[i];
        while (n != 0) {
            next = n->next;
            mem_free(n->name);
            mem_free(n);
            n = next;
        }
        i = i + 1;
    } while (i < 0x100);
    if ((set->inLoadBuffer & 1) == 0) {
        mem_free(set);
    }
}

void res_unload(u32 *handle) {
    u32 v = *handle;
    if (v != 0) {
        if ((v & 2) == 0) {
            mem_free(v);
            *handle = 0;
        } else {
            *handle = 0;
        }
    }
}

void res_unload_all(ResSet *set)
{
    s32 i;
    u32 *e;
    i = 0;
    while (i < set->count) {
        e = (u32 *)((i << 2) + (s32)set->handles);
        if (*e != 0) {
            if ((*e & 2) == 0) {
                mem_free(*e);
            }
            *e = 0;
        }
        i++;
    }
}

void *res_load_by_name(ResSet *set, char *name, u32 *id)
{
    s32 i = 0;
    u32 sum = 0;
    u32 n;
    ResName *p;

    for (;;) {
        n = strlen(name);
        if (!((u32)i < n)) break;
        if (!(i < 16)) break;
        sum += char_to_upper(name[i]);
        i++;
    }
    p = set->names[sum & 0xFF];
    if (p != NULL) {
        do {
            if (str_icmp(p->name, name) == 0) break;
            p = p->next;
        } while (p != NULL);
        if (p != NULL) goto LAB;
    }
    debug_printf((s32)D_800DB454, name);
    fatal_halt(1);
LAB:
    if (id != NULL) *id = p->id;
    return res_load(set, p->id);
}

s32 res_find_gid(ResSet *set, char *name)
{
    u8 buf[0x50];
    s32 cnt;
    s32 *q;
    u32 k;
    s32 i;

    str_copy((char *)buf, set->name);
    str_cat((char *)buf, D_800DB494);
    q = file_open(buf, D_800DB43C);
    if (q == 0) {
        return -1;
    }
    file_read((u8 *)&cnt, 1, 4, q);
    k = 0;
    if (sResSwapBytes != 0) {
        do {
            u8 t1 = ((u8 *)&cnt)[k];
            u8 t2 = ((u8 *)&cnt)[3 - k];
            ((u8 *)&cnt)[k] = t2;
            ((u8 *)&cnt)[3 - k] = t1;
            k++;
        } while (k < 2);
    }
    i = 0;
    if (cnt > 0) {
        do {
            file_read(buf, 1, 0x10, q);
            if (str_icmp(name, buf) == 0) {
                file_close(q);
                return i;
            }
            i++;
        } while (i < cnt);
    }
    file_close(q);
    return -1;
}

s32 res_get_size(ResSet *set, s32 id) {
    void *f;
    u8 tmp[8];
    s32 val;
    u32 i;

    f = file_open(set->bigPath, "rb");
    if (f == NULL) {
        return -1;
    }
    file_seek(f, set->offsets[id], 0);
    file_read(tmp, 1, 4, f);
    file_read(&val, 1, 4, f);
    if (sResSwapBytes != 0) {
        for (i = 0; i < 2; i++) {
            u8 t = ((u8 *)&val)[i];

            ((u8 *)&val)[i] = ((u8 *)&val)[3 - i];
            ((u8 *)&val)[3 - i] = t;
        }
    }
    file_close(f);
    return val;
}

void res_set_load_buffer(void *buf)
{
    sResLoadBuffer = buf;
}

/* Meant to unload the resource whose data is `data`, but compares the handle table's address, not each handle. */
void func_800C0264(ResSet *set, u32 data)
{
    s32 i;
    u32 *p;
    u32 v;

    if (data == 0) {
        return;
    }
    i = 0;
    if (set->count <= 0) {
        return;
    }
    do {
        if (((u32)set->handles & ~3) == (data & ~3)) {
            p = (u32 *)(i * 4 + (u32)set->handles);
            v = *p;
            if (v == 0) {
                return;
            }
            if ((v & 2) == 0) {
                mem_free(v);
            }
            *p = 0;
            return;
        }
        do { i++; } while (0);
    } while (i < set->count);
}

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
