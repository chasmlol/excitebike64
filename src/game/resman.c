#include "game.h"

struct Obj_82 {
    u32 count;
    u32 *data;
    u8 pad8[0x400];
    u8 *bytes;
    char name[0x80];
    u8 sub[0x88];
    u32 f514;
    u32 buf[1];
};

s32 res_open_set(char *path, Obj_82 **out)
{
    char buf[128];
    s32 flag;
    FileHandle *f;
    u32 n;
    u32 cnt;
    Obj_82 *o;
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
        o->f514 = 0;
    } else {
        cnt = n >> 2;
        o = sResLoadBuffer;
        o->f514 = 1;
        sResLoadBuffer = NULL;
    }
    res_set_clear(o);
    o->count = n >> 2;
    if (n >> 2 == 0) {
        return 2;
    }
    o->data = o->buf;
    mem_fill(o->buf, 0, n);
    o->bytes = (u8 *)(o->buf + cnt * 2);
    file_read(o->buf + cnt * 2, 1, n, f);

    if (sResSwapBytes != 0) {
        flag = 0;
        if ((s32)o->count > 0) {
            do {
                j = 0;
                do {
                    p = (u8 *)(flag * 4 + (s32)o->bytes);
                    t = p[j];
                    p[j] = p[3 - j];
                    ((u8 *)((s32)o->bytes + flag * 4))[3 - j] = t;
                    j++;
                } while (j < 2);
                flag++;
            } while (flag < (s32)o->count);
        }
    }
    file_close(f);
    str_copy(o->name, path);
    sprintf(o->sub, D_800DB44C, path);
    *out = o;
    return 0;
}

#define SWAP_BYTES(x) { if (sResSwapBytes != 0) { u32 _i; for (_i = 0; _i < 2; _i++) { u8 _t = ((u8 *)&(x))[_i]; ((u8 *)&(x))[_i] = ((u8 *)&(x))[3 - _i]; ((u8 *)&(x))[3 - _i] = _t; } } }

typedef struct {
    u8 key[4];
    void (*fn)(u32);
} Ent8_3;

extern Ent8_3 sResTypeHandlers[16];

u32 res_load(u32 *pa0, s32 pa1)
{
    u32 *p1 = pa0;
    s32 p2 = pa1;
    FileHandle *piVar2;
    s32 *piVar3;
    u32 uVar5;
    u32 i;
    u32 *puVar7;
    s32 local_34;
    s32 local_30;
    s32 local_2c;
    u32 local_38;
    u8 local_40[4];
    u8 *bb;
    s32 iVar8;
    s32 iVar4;

    if (gResLoadCallback_2 != 0) {
        (*gResLoadCallback_2)();
    }
    uVar5 = ((u32 *)p1[1])[p2];
    bb = (u8 *)(p1 + 0x123);
    if (uVar5 & 1) {
        goto inc;
    }
    if (uVar5 != 0) {
        return uVar5;
    }
inc:
    p1[0x143] = p1[0x143] + 1;
    piVar2 = file_open(bb, D_800DB43C);
    if (piVar2 == 0) {
        goto dec0;
    }
    file_seek(piVar2, ((s32 *)p1[0x102])[p2], 0);
    file_read(local_40, 1, 4, piVar2);
    file_read((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    if (sResLoadBuffer != 0) {
        piVar3 = sResLoadBuffer;
        local_2c = 1;
        sResLoadBuffer = 0;
    } else {
        local_2c = 0;
        piVar3 = mem_alloc_dbg(local_38, 0, D_800DB440, 0x1cd);
    }
    if (((u32)piVar3 & 3) != 0) {
        debug_printf(D_800DB464, (s32 *)piVar3);
        file_close(piVar2);
        return 0;
    }
    if (piVar3 == 0) {
        p1[0x143] = p1[0x143] - 1;
        file_close(piVar2);
        return 0;
    }
    local_34 = file_read((u8 *)piVar3, 1, local_38, piVar2);
    if (local_34 == (s32)local_38) {
        goto main_path;
    }
    mem_free((s32)piVar3);
    file_close(piVar2);
dec0:
    p1[0x143] = p1[0x143] - 1;
    return 0;

main_path:
    file_read((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    for (i = 0; i < local_38; i++) {
        file_read((u8 *)&local_34, 1, 4, piVar2);
        SWAP_BYTES(local_34);
        local_30 = *(s32 *)((u8 *)piVar3 + local_34);
        SWAP_BYTES(local_30);
        *(u32 *)((u8 *)piVar3 + local_34) = p1[1] + local_30 * 4;
        puVar7 = (u32 *)(local_30 * 4 + p1[1]);
        if (*puVar7 == 0) {
            *puVar7 = (p1[0x143] << 2) | 1;
        }
    }
    if (local_2c != 0) {
        ((u32 *)p1[1])[p2] = (u32)piVar3 | 2;
    } else {
        ((s32 *)p1[1])[p2] = (s32)piVar3;
    }
    file_close(piVar2);
    for (i = 0; i < p1[0]; i++) {
        if (((u32 *)p1[1])[i] == ((p1[0x143] << 2) | 1)) {
            res_load(p1, i);
            i = 0xffffffff;
        }
    }
    for (i = 0; i < 16; i++) {
        iVar8 = char_to_upper(sResTypeHandlers[i].key[0]);
        iVar4 = char_to_upper(local_40[0]);
        if (iVar8 == iVar4) {
            iVar8 = char_to_upper(sResTypeHandlers[i].key[1]);
            iVar4 = char_to_upper(local_40[1]);
            if (iVar8 == iVar4) {
                iVar8 = char_to_upper(sResTypeHandlers[i].key[2]);
                iVar4 = char_to_upper(local_40[2]);
                if (iVar8 == iVar4) {
                    sResTypeHandlers[i].fn(p1[1] + p2 * 4);
                    break;
                }
            }
        }
    }
    piVar3 = (s32 *)((u32 *)p1[1])[p2];
    p1[0x143] = p1[0x143] - 1;
    return (u32)piVar3;
}

typedef struct {
    u8 name[3];
    u8 pad;
    s32 val;
} Ent_109;

extern Ent_109 sResTypeHandlers_2[16];

void res_register_type(u8 *name, s32 val)
{
    s32 i;

    if (sResTypesInit == 0) {
        mem_fill((u8 *)sResTypeHandlers_2, 0, 0x80);
        sResTypesInit = 1;
    }
    for (i = 0; i < 16; i++) {
        if (char_to_upper(sResTypeHandlers_2[i].name[0]) == char_to_upper(name[0]) &&
            char_to_upper(sResTypeHandlers_2[i].name[1]) == char_to_upper(name[1]) &&
            char_to_upper(sResTypeHandlers_2[i].name[2]) == char_to_upper(name[2])) {
            break;
        }
    }
    if (i == 16) {
        for (i = 0; i < 16; i++) {
            if (sResTypeHandlers_2[i].val == 0) {
                break;
            }
        }
        if (i == 16) {
            debug_printf((s32)D_800DB47C);
            fatal_halt(1);
        }
    }
    __builtin_memcpy(sResTypeHandlers_2[i].name, name, 3);
    sResTypeHandlers_2[i].val = val;
}

void res_set_clear(s32 *p)
{
    s32 i = 0xFF;
    s32 *q = p + 0xFF;
    p[0x143] = 0;
    p[0x102] = 0;
    p[0] = 0;
    do {
        q[2] = 0;
        i = i - 1;
        q = q - 1;
    } while (i >= 0);
    p[1] = 0;
    p[0x144] = 0;
}

void res_close_set(s32 *param_1)
{
    s32 i;
    s32 *pp;
    s32 *n;
    s32 *nx;

    i = 0;
    if (param_1[0] > 0) {
        do {
            res_unload((u32 *)(param_1[1] + i * 4));
            i = i + 1;
        } while (i < param_1[0]);
    }
    i = 0;
    pp = param_1;
    do {
        n = (s32 *)pp[2];
        while (n != 0) {
            nx = (s32 *)n[0];
            mem_free((void *)n[1]);
            mem_free(n);
            n = nx;
        }
        i = i + 1;
        pp = pp + 1;
    } while (i < 0x100);
    if ((param_1[0x145] & 1) == 0) {
        mem_free(param_1);
    }
}

void res_unload(u32 *p) {
    u32 v = *p;
    if (v != 0) {
        if ((v & 2) == 0) {
            mem_free(v);
            *p = 0;
        } else {
            *p = 0;
        }
    }
}

struct ListHdr {
    s32 n;
    u32 *list;
};

void res_unload_all(ListHdr *h)
{
    s32 i;
    u32 *e;
    i = 0;
    while (i < h->n) {
        e = (u32 *)((i << 2) + (s32)h->list);
        if (*e != 0) {
            if ((*e & 2) == 0) {
                mem_free(*e);
            }
            *e = 0;
        }
        i++;
    }
}

typedef struct Node_52 {
    struct Node_52 *next;
    u8 *name;
    u32 val;
} Node_52;

void res_load_by_name(u32 *param_1, u8 *param_2, u32 *param_3)
{
    s32 i = 0;
    u32 sum = 0;
    u32 n;
    Node_52 *p;

    for (;;) {
        n = strlen(param_2);
        if (!((u32)i < n)) break;
        if (!(i < 16)) break;
        sum += char_to_upper(param_2[i]);
        i++;
    }
    {
        s32 base = (s32)param_1;
        p = *(Node_52 **)(base + ((sum & 0xFF) << 2) + 8);
    }
    if (p != NULL) {
        do {
            if (str_icmp(p->name, param_2) == 0) break;
            p = p->next;
        } while (p != NULL);
        if (p != NULL) goto LAB;
    }
    debug_printf((s32)D_800DB454, param_2);
    fatal_halt(1);
LAB:
    if (param_3 != NULL) *param_3 = p->val;
    res_load(param_1, p->val);
}

s32 res_find_gid(u8 *a0, u8 *a1)
{
    u8 buf[0x50];
    s32 cnt;
    FileHandle *q;
    u32 k;
    s32 i;

    str_copy((char *)buf, (char *)(a0 + 0x40c));
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
            if (str_icmp(a1, buf) == 0) {
                file_close(q);
                return i;
            }
            i++;
        } while (i < cnt);
    }
    file_close(q);
    return -1;
}

struct ResFile_2 {
    u8 unk0[0x408];
    s32 *offsets;
    u8 unk40C[0x48C - 0x40C];
    char path[1];
};

s32 res_get_size(ResFile_2 *r, s32 idx) {
    FileHandle *f;
    u8 tmp[8];
    s32 val;
    u32 i;

    f = file_open(r->path, "rb");
    if (f == NULL) {
        return -1;
    }
    file_seek(f, r->offsets[idx], 0);
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

void res_set_load_buffer(s32 a0)
{
    sResLoadBuffer = a0;
}

void func_800C0264(s32 *param_1, u32 param_2)
{
    s32 i;
    u32 *p;
    u32 v;

    if (param_2 == 0) {
        return;
    }
    i = 0;
    if (param_1[0] <= 0) {
        return;
    }
    do {
        if ((param_1[1] & ~3) == (param_2 & ~3)) {
            p = (u32 *)(i * 4 + param_1[1]);
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
    } while (i < param_1[0]);
}

u32 res_hash_name(u8 *p)
{
    u32 i = 0;
    u32 sum = 0;
    u32 len;
    u8 c;

    for (;;) {
        len = strlen(p);
        if (len <= i || (s32)i >= 16) {
            break;
        }
        c = p[i];
        i = i + 1;
        sum = sum + char_to_upper(c);
    }
    return sum & 0xFF;
}
