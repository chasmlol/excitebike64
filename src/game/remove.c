#include "game.h"

/* Groups the objects whose flags (bits 1-2) name a wall group with the wall edges of that group (edge flags
   bits 3-4). */
void remove_walls_init(TrackModelList *m, WallMesh *o)
{
    s32 i, j, k;
    RemoveGroup *e;

    sRemoveGroupCount = 0;
    for (i = 0; i < m->count; i++) {
        if (m->e[i].mask & 6) {
            sRemoveGroups[sRemoveGroupCount].objIndex = i;
            sRemoveGroups[sRemoveGroupCount].flags = m->e[i].mask;
            sRemoveGroups[sRemoveGroupCount++].id = (m->e[i].mask >> 1) & 3;
        }
    }
    if (sRemoveGroupCount == 0) return;
    for (i = 0; i < sRemoveGroupCount; i++) {
        sRemoveGroups[i].count = 0;
    }
    for (k = 0; k < o->numEdges; k++) {
        i = (gWallEdgeFlags[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < sRemoveGroupCount; j++) {
                if (sRemoveGroups[j].id == i) break;
            }
            sRemoveGroups[j].count++;
        }
    }
    for (i = 0; i < sRemoveGroupCount; i++) {
        if (sRemoveGroups[i].count) {
            sRemoveGroups[i].edges = mem_alloc_dbg(sRemoveGroups[i].count * 2, 0, (s32)sStrRemoveC, 0x5D);
            sRemoveGroups[i].count = 0;
        }
    }
    for (k = 0; k < o->numEdges; k++) {
        i = (gWallEdgeFlags[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < sRemoveGroupCount; j++) {
                if (sRemoveGroups[j].id == i) break;
            }
            sRemoveGroups[j].edges[sRemoveGroups[j].count++] = k;
        }
    }
}

/* A lap or segment event of the racing line: removes the walls of group id and breaks (or hides) its object. */
void remove_walls_trigger(s32 id)
{
    s32 i;
    s32 k;

    for (i = 0; i < sRemoveGroupCount; i++) {
        if (sRemoveGroups[i].id == id) {
            break;
        }
    }
    if (i != sRemoveGroupCount) {
        for (k = 0; k < sRemoveGroups[i].count; k++) {
            gWallEdgeFlags[sRemoveGroups[i].edges[k]] |= 0x20;
        }
        k = sRemoveGroups[i].objIndex;
        if (gObjectFlags[k] & 1) {
            shatter_break_object(k);
        } else {
            gObjectFlags[k] |= 8;
        }
    }
}

void remove_walls_free(void)
{
    s32 i = 0;
    if (0 < sRemoveGroupCount) {
        do {
            if (sRemoveGroups[i].count != 0) {
                mem_free(sRemoveGroups[i].edges);
            }
            i++;
        } while (sRemoveGroupCount > i);
    }
}

/* Puts all removed walls back. */
void remove_walls_reset(void) {
    s32 i;
    s32 j;
    for (i = 0; i < sRemoveGroupCount; i++) {
        RemoveGroup *e = &sRemoveGroups[i];
        for (j = 0; j < e->count; j++) {
            u8 *b = gWallEdgeFlags + e->edges[j];
            *b &= 0xDF;
        }
    }
}
