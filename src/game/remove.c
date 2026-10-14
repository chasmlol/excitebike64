#include "game.h"

typedef struct {
    u32 pad0;
    s32 type;
    u32 flags;
    s32 index;
    s32 count;
    u16 *list;
} SubEnt;

extern SubEnt sRemoveGroups[];

typedef struct {
    u32 flags;
    u8 pad[0x1C];
} Rec_45;

struct Mdl2 {
    u32 n;
    u8 pad[0x1C];
    Rec_45 r[1];
};

struct Obj2 {
    u8 pad[0x18];
    s32 n;
};

void remove_walls_init(Mdl2 *m, Obj2 *o)
{
    s32 i, j, k;
    SubEnt *e;

    sRemoveGroupCount = 0;
    for (i = 0; i < m->n; i++) {
        if (m->r[i].flags & 6) {
            sRemoveGroups[sRemoveGroupCount].index = i;
            sRemoveGroups[sRemoveGroupCount].flags = m->r[i].flags;
            sRemoveGroups[sRemoveGroupCount++].type = (m->r[i].flags >> 1) & 3;
        }
    }
    if (sRemoveGroupCount == 0) return;
    for (i = 0; i < sRemoveGroupCount; i++) {
        sRemoveGroups[i].count = 0;
    }
    for (k = 0; k < o->n; k++) {
        i = (gWallEdgeFlags[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < sRemoveGroupCount; j++) {
                if (sRemoveGroups[j].type == i) break;
            }
            sRemoveGroups[j].count++;
        }
    }
    for (i = 0; i < sRemoveGroupCount; i++) {
        if (sRemoveGroups[i].count) {
            sRemoveGroups[i].list = mem_alloc_dbg(sRemoveGroups[i].count * 2, 0, (s32)D_800D91A0, 0x5D);
            sRemoveGroups[i].count = 0;
        }
    }
    for (k = 0; k < o->n; k++) {
        i = (gWallEdgeFlags[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < sRemoveGroupCount; j++) {
                if (sRemoveGroups[j].type == i) break;
            }
            sRemoveGroups[j].list[sRemoveGroups[j].count++] = k;
        }
    }
}

typedef struct {
    s32 f0;
    s32 id;
    s32 f8;
    s32 idx;
    s32 n;
    u16 *tab;
} Ent_81;

extern Ent_81 sRemoveGroups_2[];

void remove_walls_trigger(s32 id)
{
    s32 i;
    s32 k;

    for (i = 0; i < sRemoveGroupCount; i++) {
        if (sRemoveGroups_2[i].id == id) {
            break;
        }
    }
    if (i != sRemoveGroupCount) {
        for (k = 0; k < sRemoveGroups_2[i].n; k++) {
            gWallEdgeFlags[sRemoveGroups_2[i].tab[k]] |= 0x20;
        }
        k = sRemoveGroups_2[i].idx;
        if (gObjectFlags[k] & 1) {
            shatter_break_object(k);
        } else {
            gObjectFlags[k] |= 8;
        }
    }
}

typedef struct {
    s32 pad[4];
    s32 f10;
    s32 f14;
} Ent_82;

extern Ent_82 sRemoveGroups_3[];

void remove_walls_free(void)
{
    s32 i = 0;
    if (0 < sRemoveGroupCount) {
        do {
            if (sRemoveGroups_3[i].f10 != 0) {
                mem_free(sRemoveGroups_3[i].f14);
            }
            i++;
        } while (sRemoveGroupCount > i);
    }
}

typedef struct {
    u8 pad[0x10];
    s32 n;
    u16 *idx;
} Ent_83;

void remove_walls_reset(void) {
    s32 i;
    s32 j;
    for (i = 0; i < sRemoveGroupCount; i++) {
        Ent_83 *e = &sRemoveGroups[i];
        for (j = 0; j < e->n; j++) {
            u8 *b = gWallEdgeFlags + e->idx[j];
            *b &= 0xDF;
        }
    }
}
