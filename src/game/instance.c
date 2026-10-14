#include "game.h"

typedef struct { s32 m[16]; } MtxS_3;
typedef struct { s32 x; s32 y; s32 z; u8 pad[2]; u8 rot; u8 pad2; } Item_6;
struct Box_4 { u32 n; s32 pad; s32 x0; s32 z0; s32 x1; s32 z1; s32 pad2[2]; };
typedef struct { u32 nids; u32 nitems; u32 ids[1]; } Hdr_9;

extern MtxS_3 *D_800CCCD0_2;
extern Hdr_9 *D_800F1430_2;
extern Item_6 *D_800F143C_2;
extern Item_6 *D_800F1428;
extern MtxS_3 *D_800F1438;

s32 func_80065B10(u32 *a, s32 b, s32 c, Box_4 *d, s32 e)
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
    D_800CCCD0_2 = NULL;
    if (b < 0) {
        return ret;
    }
    {
        D_800F1434 = d;
        D_800F1430_2 = (Hdr_9 *)func_800BF79C(a, b);
        if (D_800F1430_2->nids == 0 || D_800F1430_2->nitems == 0) {
            ret = func_800BFEA4_2((u32 *)(a[1] + b * 4));
            D_800F1430_2 = NULL;
        } else {
            D_800F143C_2 = (Item_6 *)&D_800F1430_2->ids[D_800F1430_2->nids];
            D_800F142C = func_800BC908(D_800F1430_2->nitems * 4, 0, (s32)D_800D8D50, 0x5D);
            m = func_800BC908(D_800F1430_2->nitems * 64, 0, (s32)D_800D8D50, 0x5E);
            i = 0;
            n = D_800F1430_2->nitems;
            do {
                D_800CCCD0_2 = m;
            } while (0);
            if (n != 0) do {
                px = D_800F143C_2[i].x >> 6;
                pz = D_800F143C_2[i].z >> 6;
                for (j = 0; j < d->n; j++) {
                    cx = (d[j].x0 + d[j].x1) >> 3;
                    cz = (d[j].z0 + d[j].z1) >> 3;
                    dist = (cx - px) * (cx - px) + (cz - pz) * (cz - pz);
                    if (dist < best || j == 0) {
                        best = dist;
                        bi = j;
                    }
                }
                D_800F142C[i] = bi;
                if (c & 1) {
                    cx = 0;
                    cz = 0;
                } else {
                    bx = d + bi;
                    cx = (bx->x0 + bx->x1) >> 1;
                    cz = (bx->z0 + bx->z1) >> 1;
                }
                px = D_800F143C_2[i].x >> 4;
                pz = D_800F143C_2[i].z >> 4;
                func_80011490(&D_800CCCD0_2[i], 0, 0, D_800F143C_2[i].rot << 8, (px - cx) * 16, D_800F143C_2[i].y,
                              (pz - cz) * 16, 0x40);
            } while (++i < D_800F1430_2->nitems);
            D_800F1424 = func_800BC908(D_800F1430_2->nids * 4, 0, (s32)D_800D8D50, 0x8E);
            for (i = 0; i < D_800F1430_2->nids; i++) {
                p = (s32 *)func_800BF79C(a, D_800F1430_2->ids[i]);
                func_80014B84(&D_800F1424[i], (u32)p, (u32)&e, 0xC);
                func_800BD798(p, p[3] - (s32)p, 0, (s32)D_800D8D50, 0x94);
            }
            D_800F1428 = func_800BC908(D_800F1430_2->nitems * 16, 0, (s32)D_800D8D50, 0x98);
            D_800F1420 = func_800BC908(D_800F1430_2->nitems * 4, 0, (s32)D_800D8D50, 0x99);
            D_800F1438 = func_800BC908(D_800F1430_2->nitems * 64, 0, (s32)D_800D8D50, 0x9A);
            k = 0;
            for (i = 0; i < d->n; i++) {
                for (j = 0; j < D_800F1430_2->nitems; j++) {
                    if (D_800F142C[j] == i) {
                        D_800F1420[k] = i;
                        D_800F1428[k] = D_800F143C_2[j];
                        D_800F1438[k] = D_800CCCD0_2[j];
                        k++;
                    }
                }
            }
            memcpy(D_800CCCD0_2, D_800F1438, D_800F1430_2->nitems * 64);
            memcpy(D_800F143C_2, D_800F1428, D_800F1430_2->nitems * 16);
            memcpy(D_800F142C, D_800F1420, D_800F1430_2->nitems * 4);
            func_800BCB04_2(D_800F1438);
            func_800BCB04_2(D_800F1420);
            ret = func_800BCB04_2(D_800F1428);
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

extern ObjList_2 *D_800F1430_3;
extern ObjTable *D_800F1434_2;
extern ObjInst *D_800F143C_3;

Gfx *func_80066070(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis)
{
    u32 i;
    s32 key;
    ObjInfo *o;
    s32 off;

    if (D_800CCCD0_3 == 0) {
        return gdl;
    }
    i = 0;
    while (i < D_800F1430_3->n) {
        key = D_800F142C[i];
        o = &D_800F1434_2->e[key];
        if (((o->flags >> (view * 2)) & 3) != vis) {
            while (D_800F142C[i] == key && i < D_800F1430_3->n) {
                i++;
            }
        } else {
            gSPMatrix(gdl++, D_800E77D4[key * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            while (D_800F142C[i] == key && i < D_800F1430_3->n) {
                gSPMatrix(gdl++, &D_800CCCD0_3[i], G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800F1424[D_800F143C_3[i].dl]);
                gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
                i++;
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}

void func_80066268(void)
{
    u32 i;
    if (D_800CCCD0_4 == 0) {
        return;
    }
    i = 0;
    func_800BCB04(D_800CCCD0_4);
    func_800BCB04(D_800F142C);
    if (*(u32 *)D_800F1430 != 0) {
        do {
            func_800BCB04(((s32 *)D_800F1424)[i]);
            i++;
        } while (i < *(u32 *)D_800F1430);
    }
    func_800BCB04(D_800F1424);
    D_800CCCD0_4 = 0;
}
