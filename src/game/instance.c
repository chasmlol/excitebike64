#include "game.h"

typedef struct { s32 m[16]; } MtxS_3;
typedef struct { s32 x; s32 y; s32 z; u8 pad[2]; u8 rot; u8 pad2; } Item_6;
struct Box_4 { u32 n; s32 pad; s32 x0; s32 z0; s32 x1; s32 z1; s32 pad2[2]; };
typedef struct { u32 nids; u32 nitems; u32 ids[1]; } Hdr_9;

extern MtxS_3 *gInstanceMtx_2;
extern Hdr_9 *gInstanceHeader_2;
extern Item_6 *gInstanceItems_2;
extern Item_6 *sInstanceSortItems;
extern MtxS_3 *sInstanceSortMtx;

s32 instances_load(u32 *a, s32 b, s32 c, Box_4 *d, s32 e)
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
    Box_4 *bx;
    Item_6 *it;
    s32 *p;
    MtxS_3 *m;

    ret = e;
    gInstanceMtx_2 = NULL;
    if (b < 0) {
        return ret;
    }
    {
        sInstanceZoneBoxes = d;
        gInstanceHeader_2 = (Hdr_9 *)res_load(a, b);
        if (gInstanceHeader_2->nids == 0 || gInstanceHeader_2->nitems == 0) {
            ret = res_unload_2((u32 *)(a[1] + b * 4));
            gInstanceHeader_2 = NULL;
        } else {
            gInstanceItems_2 = (Item_6 *)&gInstanceHeader_2->ids[gInstanceHeader_2->nids];
            sInstanceZone = mem_alloc_dbg(gInstanceHeader_2->nitems * 4, 0, (s32)sStrInstanceC, 0x5D);
            m = mem_alloc_dbg(gInstanceHeader_2->nitems * 64, 0, (s32)sStrInstanceC, 0x5E);
            i = 0;
            n = gInstanceHeader_2->nitems;
            do {
                gInstanceMtx_2 = m;
            } while (0);
            if (n != 0) do {
                px = gInstanceItems_2[i].x >> 6;
                pz = gInstanceItems_2[i].z >> 6;
                for (j = 0; j < d->n; j++) {
                    cx = (d[j].x0 + d[j].x1) >> 3;
                    cz = (d[j].z0 + d[j].z1) >> 3;
                    dist = (cx - px) * (cx - px) + (cz - pz) * (cz - pz);
                    if (dist < best || j == 0) {
                        best = dist;
                        bi = j;
                    }
                }
                sInstanceZone[i] = bi;
                if (c & 1) {
                    cx = 0;
                    cz = 0;
                } else {
                    bx = d + bi;
                    cx = (bx->x0 + bx->x1) >> 1;
                    cz = (bx->z0 + bx->z1) >> 1;
                }
                px = gInstanceItems_2[i].x >> 4;
                pz = gInstanceItems_2[i].z >> 4;
                mtx_from_angles_pos_scale(&gInstanceMtx_2[i], 0, 0, gInstanceItems_2[i].rot << 8, (px - cx) * 16, gInstanceItems_2[i].y,
                              (pz - cz) * 16, 0x40);
            } while (++i < gInstanceHeader_2->nitems);
            sInstanceDLs = mem_alloc_dbg(gInstanceHeader_2->nids * 4, 0, (s32)sStrInstanceC, 0x8E);
            for (i = 0; i < gInstanceHeader_2->nids; i++) {
                p = (s32 *)res_load(a, gInstanceHeader_2->ids[i]);
                prim_alloc_dl(&sInstanceDLs[i], (u32)p, (u32)&e, 0xC);
                mem_realloc_dbg(p, p[3] - (s32)p, 0, (s32)sStrInstanceC, 0x94);
            }
            sInstanceSortItems = mem_alloc_dbg(gInstanceHeader_2->nitems * 16, 0, (s32)sStrInstanceC, 0x98);
            sInstanceSortZones = mem_alloc_dbg(gInstanceHeader_2->nitems * 4, 0, (s32)sStrInstanceC, 0x99);
            sInstanceSortMtx = mem_alloc_dbg(gInstanceHeader_2->nitems * 64, 0, (s32)sStrInstanceC, 0x9A);
            k = 0;
            for (i = 0; i < d->n; i++) {
                for (j = 0; j < gInstanceHeader_2->nitems; j++) {
                    if (sInstanceZone[j] == i) {
                        sInstanceSortZones[k] = i;
                        sInstanceSortItems[k] = gInstanceItems_2[j];
                        sInstanceSortMtx[k] = gInstanceMtx_2[j];
                        k++;
                    }
                }
            }
            memcpy(gInstanceMtx_2, sInstanceSortMtx, gInstanceHeader_2->nitems * 64);
            memcpy(gInstanceItems_2, sInstanceSortItems, gInstanceHeader_2->nitems * 16);
            memcpy(sInstanceZone, sInstanceSortZones, gInstanceHeader_2->nitems * 4);
            mem_free_2(sInstanceSortMtx);
            mem_free_2(sInstanceSortZones);
            ret = mem_free_2(sInstanceSortItems);
        }
    }
    return ret;
}

typedef struct {
    s32 hdr;
    ObjInfo e[1];
} ObjTable;

typedef struct {
    s32 count;
    u32 n;
} ObjList_2;

typedef struct {
    u8 pad[0xF];
    u8 dl;
} ObjInst;

extern ObjList_2 *gInstanceHeader_3;
extern ObjTable *sInstanceZoneBoxes_2;
extern ObjInst *gInstanceItems_3;

Gfx *instances_draw(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis)
{
    u32 i;
    s32 key;
    ObjInfo *o;
    s32 off;

    if (gInstanceMtx_3 == 0) {
        return gdl;
    }
    i = 0;
    while (i < gInstanceHeader_3->n) {
        key = sInstanceZone[i];
        o = &sInstanceZoneBoxes_2->e[key];
        if (((o->flags >> (view * 2)) & 3) != vis) {
            while (sInstanceZone[i] == key && i < gInstanceHeader_3->n) {
                i++;
            }
        } else {
            gSPMatrix(gdl++, gSectionMtxs[key * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            while (sInstanceZone[i] == key && i < gInstanceHeader_3->n) {
                gSPMatrix(gdl++, &gInstanceMtx_3[i], G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, sInstanceDLs[gInstanceItems_3[i].dl]);
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
    if (gInstanceMtx_4 == 0) {
        return;
    }
    i = 0;
    mem_free(gInstanceMtx_4);
    mem_free(sInstanceZone);
    if (*(u32 *)gInstanceHeader != 0) {
        do {
            mem_free(((s32 *)sInstanceDLs)[i]);
            i++;
        } while (i < *(u32 *)gInstanceHeader);
    }
    mem_free(sInstanceDLs);
    gInstanceMtx_4 = 0;
}
