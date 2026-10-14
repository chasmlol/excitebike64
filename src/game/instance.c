#include "game.h"

/* Loads the instances of a track: each item is binned to its nearest section and gets a matrix relative to that
   section's centre (or to the origin with flags & 1), then the items are sorted by section. */
s32 instances_load(ResSet *resSet, s32 resIndex, s32 flags, TrackModelList *sections, TexSet *texSet)
{
    s32 ret;
    u32 i, j;
    s32 k;
    s32 dx, dz, dist;
    s32 best;
    u32 bi;
    s32 cx, cz;
    s32 px, pz;
    u32 n;
    PrimModel *mdl;
    Mtx *m;

    ret = (s32)texSet;
    gInstanceMtx = NULL;
    if (resIndex < 0) {
        return ret;
    }
    {
        sInstanceZoneBoxes = sections;
        gInstanceHeader = (InstanceRes *)res_load(resSet, resIndex);
        if (gInstanceHeader->numModels == 0 || gInstanceHeader->numItems == 0) {
            ret = res_unload_2(&resSet->handles[resIndex]);
            gInstanceHeader = NULL;
        } else {
            gInstanceItems = (Instance *)&gInstanceHeader->modelIds[gInstanceHeader->numModels];
            sInstanceZone = mem_alloc_dbg(gInstanceHeader->numItems * 4, 0, (s32)sStrInstanceC, 0x5D);
            m = mem_alloc_dbg(gInstanceHeader->numItems * 64, 0, (s32)sStrInstanceC, 0x5E);
            i = 0;
            n = gInstanceHeader->numItems;
            do {
                gInstanceMtx = m;
            } while (0);
            if (n != 0) do {
                px = gInstanceItems[i].x >> 6;
                pz = gInstanceItems[i].z >> 6;
                for (j = 0; j < sections->count; j++) {
                    cx = (sections->e[j].x0 + sections->e[j].x1) >> 3;
                    cz = (sections->e[j].z0 + sections->e[j].z1) >> 3;
                    dist = (cx - px) * (cx - px) + (cz - pz) * (cz - pz);
                    if (dist < best || j == 0) {
                        best = dist;
                        bi = j;
                    }
                }
                sInstanceZone[i] = bi;
                if (flags & 1) {
                    cx = 0;
                    cz = 0;
                } else {
                    cx = (sections->e[bi].x0 + sections->e[bi].x1) >> 1;
                    cz = (sections->e[bi].z0 + sections->e[bi].z1) >> 1;
                }
                px = gInstanceItems[i].x >> 4;
                pz = gInstanceItems[i].z >> 4;
                mtx_from_angles_pos_scale((MtxS *)&gInstanceMtx[i], 0, 0, gInstanceItems[i].rot << 8, (px - cx) * 16,
                                          gInstanceItems[i].y, (pz - cz) * 16, 0x40);
            } while (++i < gInstanceHeader->numItems);
            sInstanceDLs = mem_alloc_dbg(gInstanceHeader->numModels * 4, 0, (s32)sStrInstanceC, 0x8E);
            for (i = 0; i < gInstanceHeader->numModels; i++) {
                mdl = (PrimModel *)res_load(resSet, gInstanceHeader->modelIds[i]);
                prim_alloc_dl(&sInstanceDLs[i], mdl, &texSet, 0xC);
                /* keep the vertices, drop the commands */
                mem_realloc_dbg(mdl, (s32)mdl->cmds - (s32)mdl, 0, (s32)sStrInstanceC, 0x94);
            }
            sInstanceSortItems = mem_alloc_dbg(gInstanceHeader->numItems * 16, 0, (s32)sStrInstanceC, 0x98);
            sInstanceSortZones = mem_alloc_dbg(gInstanceHeader->numItems * 4, 0, (s32)sStrInstanceC, 0x99);
            sInstanceSortMtx = mem_alloc_dbg(gInstanceHeader->numItems * 64, 0, (s32)sStrInstanceC, 0x9A);
            k = 0;
            for (i = 0; i < sections->count; i++) {
                for (j = 0; j < gInstanceHeader->numItems; j++) {
                    if (sInstanceZone[j] == i) {
                        sInstanceSortZones[k] = i;
                        sInstanceSortItems[k] = gInstanceItems[j];
                        sInstanceSortMtx[k] = gInstanceMtx[j];
                        k++;
                    }
                }
            }
            memcpy(gInstanceMtx, sInstanceSortMtx, gInstanceHeader->numItems * 64);
            memcpy(gInstanceItems, sInstanceSortItems, gInstanceHeader->numItems * 16);
            memcpy(sInstanceZone, sInstanceSortZones, gInstanceHeader->numItems * 4);
            mem_free_2(sInstanceSortMtx);
            mem_free_2(sInstanceSortZones);
            ret = mem_free_2(sInstanceSortItems);
        }
    }
    return ret;
}

/* Draws the instances of the sections whose view bits for `view` equal `vis` (1 near, 3 far). */
Gfx *instances_draw(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis)
{
    u32 i;
    s32 sec;
    TrackModel *o;
    s32 off;

    if (gInstanceMtx == 0) {
        return gdl;
    }
    i = 0;
    while (i < gInstanceHeader->numItems) {
        sec = sInstanceZone[i];
        o = &sInstanceZoneBoxes->e[sec];
        if (((o->mask >> (view * 2)) & 3) != vis) {
            while (sInstanceZone[i] == sec && i < gInstanceHeader->numItems) {
                i++;
            }
        } else {
            gSPMatrix(gdl++, gSectionMtxs[sec * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            while (sInstanceZone[i] == sec && i < gInstanceHeader->numItems) {
                gSPMatrix(gdl++, &gInstanceMtx[i], G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, sInstanceDLs[gInstanceItems[i].model]);
                gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
                i++;
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}

void instances_free(void)
{
    u32 i;
    if (gInstanceMtx == 0) {
        return;
    }
    i = 0;
    mem_free(gInstanceMtx);
    mem_free(sInstanceZone);
    if (gInstanceHeader->numModels != 0) {
        do {
            mem_free(sInstanceDLs[i]);
            i++;
        } while (i < gInstanceHeader->numModels);
    }
    mem_free(sInstanceDLs);
    gInstanceMtx = 0;
}
