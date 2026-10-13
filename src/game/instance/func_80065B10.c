#include "ctx.h"

typedef struct { s32 m[16]; } MtxS;
typedef struct { s32 x; s32 y; s32 z; u8 pad[2]; u8 rot; u8 pad2; } Item;
typedef struct { u32 n; s32 pad; s32 x0; s32 z0; s32 x1; s32 z1; s32 pad2[2]; } Box;
typedef struct { u32 nids; u32 nitems; u32 ids[1]; } Hdr;

extern MtxS *D_800CCCD0;
extern Box *D_800F1434;
extern Hdr *D_800F1430;
extern Item *D_800F143C;
extern u32 *D_800F142C;
extern u32 *D_800F1424;
extern Item *D_800F1428;
extern u32 *D_800F1420;
extern MtxS *D_800F1438;
extern char D_800D8D50[];

s32 func_800BFEA4(u32 *p);
void func_80014B84(u32 *p1, u32 a1, u32 a2, u32 a3);
s32 *func_800BD798(s32 *p, u32 size, s32 a, s32 b, s32 c);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
s32 func_800BCB04(void *ptr);
void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
u32 func_800BF79C(u32 *pa0, s32 pa1);
void func_800AEC34(void *src, void *dst, s32 n);

s32 func_80065B10(u32 *a, s32 b, s32 c, Box *d, s32 e)
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
    Box *bx;
    Item *it;
    s32 *p;
    MtxS *m;

    ret = e;
    D_800CCCD0 = NULL;
    if (b < 0) {
        return ret;
    }
    {
        D_800F1434 = d;
        D_800F1430 = (Hdr *)func_800BF79C(a, b);
        if (D_800F1430->nids == 0 || D_800F1430->nitems == 0) {
            ret = func_800BFEA4((u32 *)(a[1] + b * 4));
            D_800F1430 = NULL;
        } else {
            D_800F143C = (Item *)&D_800F1430->ids[D_800F1430->nids];
            D_800F142C = func_800BC908(D_800F1430->nitems * 4, 0, (s32)D_800D8D50, 0x5D);
            m = func_800BC908(D_800F1430->nitems * 64, 0, (s32)D_800D8D50, 0x5E);
            i = 0;
            n = D_800F1430->nitems;
            do {
                D_800CCCD0 = m;
            } while (0);
            if (n != 0) do {
                px = D_800F143C[i].x >> 6;
                pz = D_800F143C[i].z >> 6;
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
                px = D_800F143C[i].x >> 4;
                pz = D_800F143C[i].z >> 4;
                func_80011490(&D_800CCCD0[i], 0, 0, D_800F143C[i].rot << 8, (px - cx) * 16, D_800F143C[i].y,
                              (pz - cz) * 16, 0x40);
            } while (++i < D_800F1430->nitems);
            D_800F1424 = func_800BC908(D_800F1430->nids * 4, 0, (s32)D_800D8D50, 0x8E);
            for (i = 0; i < D_800F1430->nids; i++) {
                p = (s32 *)func_800BF79C(a, D_800F1430->ids[i]);
                func_80014B84(&D_800F1424[i], (u32)p, (u32)&e, 0xC);
                func_800BD798(p, p[3] - (s32)p, 0, (s32)D_800D8D50, 0x94);
            }
            D_800F1428 = func_800BC908(D_800F1430->nitems * 16, 0, (s32)D_800D8D50, 0x98);
            D_800F1420 = func_800BC908(D_800F1430->nitems * 4, 0, (s32)D_800D8D50, 0x99);
            D_800F1438 = func_800BC908(D_800F1430->nitems * 64, 0, (s32)D_800D8D50, 0x9A);
            k = 0;
            for (i = 0; i < d->n; i++) {
                for (j = 0; j < D_800F1430->nitems; j++) {
                    if (D_800F142C[j] == i) {
                        D_800F1420[k] = i;
                        D_800F1428[k] = D_800F143C[j];
                        D_800F1438[k] = D_800CCCD0[j];
                        k++;
                    }
                }
            }
            func_800AEC34(D_800CCCD0, D_800F1438, D_800F1430->nitems * 64);
            func_800AEC34(D_800F143C, D_800F1428, D_800F1430->nitems * 16);
            func_800AEC34(D_800F142C, D_800F1420, D_800F1430->nitems * 4);
            func_800BCB04(D_800F1438);
            func_800BCB04(D_800F1420);
            ret = func_800BCB04(D_800F1428);
        }
    }
    return ret;
}
